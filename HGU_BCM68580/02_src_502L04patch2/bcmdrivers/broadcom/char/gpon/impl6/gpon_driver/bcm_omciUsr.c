/* 
* <:copyright-BRCM:2007:proprietary:gpon
* 
*    Copyright (c) 2007 Broadcom 
*    All Rights Reserved
* 
*  This program is the proprietary software of Broadcom and/or its
*  licensors, and may only be used, duplicated, modified or distributed pursuant
*  to the terms and conditions of a separate, written license agreement executed
*  between you and Broadcom (an "Authorized License").  Except as set forth in
*  an Authorized License, Broadcom grants no license (express or implied), right
*  to use, or waiver of any kind with respect to the Software, and Broadcom
*  expressly reserves all rights in and to the Software and all intellectual
*  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*  Except as expressly set forth in the Authorized License,
* 
*  1. This program, including its structure, sequence and organization,
*     constitutes the valuable trade secrets of Broadcom, and you shall use
*     all reasonable efforts to protect the confidentiality thereof, and to
*     use this information only in connection with your use of Broadcom
*     integrated circuit products.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*     PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*     LIMITED REMEDY.
* :>
*/ 

#include <bcmtypes.h>
#include <bcm_omci_api.h>

#include "bcm_omciUsr.h"    /* local definitions */
#include "bcm_omci.h"
#include "bcm_ploamPortCtrl.h"


#include <linux/bcm_log.h>

#ifndef OMCI_MAJOR
  #define OMCI_MAJOR 3018
#endif

/**
 * Local Types
 **/
typedef struct {
    wait_queue_head_t readWaitQueue;    /* read wait queue */
    wait_queue_head_t writeWaitQueue;   /* write wait queue */
    int nreaders, nwriters;             /* number of openings for r/w */
    struct semaphore rxSem;             /* RX semaphore*/
    struct semaphore txSem;             /* TX semaphore*/
    struct semaphore ioctlSem;          /* ioctl serialization semaphore*/
    bool created;

} BCM_OmciUsrState;

/*
 * local variables
 */
static int omciMajor = OMCI_MAJOR;
#ifdef CONFIG_BCM_LOG
static int omciMinor = 0;
#endif

static BCM_OmciUsrState state;
static BCM_OmciUsrState *statep=&state;

/**
 * omci fops
 **/

static int omci_open(struct inode *inode, struct file *filp) {
  int rv=0;

  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(statep->created);
  BCM_ASSERT(filp);

  if (down_interruptible(&statep->rxSem)) {
    rv = -ERESTARTSYS;
    goto open_fail_1;
  }

  if (down_interruptible(&statep->txSem)) {
    rv = -ERESTARTSYS;
    goto open_fail_2;
  }

  /* use f_mode, not f_flags: it's cleaner (fs/open.c tells why) */
  if (filp->f_mode & FMODE_READ) {
    statep->nreaders++;
  }
  if (filp->f_mode & FMODE_WRITE) {
    statep->nwriters++;
  }

  up(&statep->txSem);
open_fail_2:
  up(&statep->rxSem);
open_fail_1:
  return rv;
}

static int omci_release(struct inode *inode, struct file *filp) {
  int rv=0;

  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(statep->created);
  BCM_ASSERT(filp);

  if (down_interruptible(&statep->rxSem)) {
    rv = -ERESTARTSYS;
    goto rel_fail_1;
  }

  if (down_interruptible(&statep->txSem)) {
    rv = -ERESTARTSYS;
    goto rel_fail_2;
  }

  if (filp->f_mode & FMODE_READ) {
    statep->nreaders--;
  }
  if (filp->f_mode & FMODE_WRITE) {
    statep->nwriters--;
  }

  up(&statep->txSem);
rel_fail_2:
  up(&statep->rxSem);
rel_fail_1:
  return rv;

}

static long omci_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
  int ret=0;

  BCM_LOG_INFO(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(statep->created);

  switch (cmd) {
  case BCM_OMCI_IOC_GET_DRIVER_VERSION:
  {
    static BCM_Gpon_DriverVersionInfo driverVersionInfo = {
      BCM_OMCI_VERSION_API_MAJOR,
      BCM_OMCI_VERSION_API_MINOR,
      BCM_OMCI_VERSION_DRIVER_MAJOR,
      BCM_OMCI_VERSION_DRIVER_MINOR,
      BCM_OMCI_VERSION_DRIVER_FIX
    };

    ret = copy_to_user((void*)arg, &driverVersionInfo, sizeof(driverVersionInfo));
    if (ret)
      ret = -EINVAL;

    break;
  }
  case BCM_OMCI_IOC_GET_COUNTERS:
  {


    BCM_Omci_Counters apiOmciCounters;
    BCM_OmciCounters intOmciCounters;

    ret = copy_from_user(&apiOmciCounters, (const void*)arg, sizeof(apiOmciCounters));
    if (ret) {
      BCM_LOG_ERROR(BCM_LOG_ID_OMCI,"invalid argument (2), %lu", arg);
      ret = -EINVAL;
      break;
    }

    if (down_interruptible(&statep->ioctlSem))
      return -ERESTARTSYS;

    bcm_omciGetSwCounters(&intOmciCounters, apiOmciCounters.reset);

    apiOmciCounters.rxBytes = intOmciCounters.rxBytes;
    apiOmciCounters.rxFragments = intOmciCounters.rxFragments;

    apiOmciCounters.rxMessagesDiscarded = intOmciCounters.rxDiscarded;
    apiOmciCounters.rxMessagesTotal = intOmciCounters.rxPackets;
    apiOmciCounters.txBytes = intOmciCounters.txBytes;

    up(&statep->ioctlSem);

    __copy_to_user((void*)arg, &apiOmciCounters, sizeof(apiOmciCounters));
    break;
  }
  default:
    BCM_LOG_ERROR(BCM_LOG_ID_OMCI,"invalid command, %d", cmd);
  };

  return ret;
}

static ssize_t omci_read(struct file *filp, char __user *buf, size_t count,
                         loff_t *f_pos) {
  uint8_t* kBuf;
  int rv=0;

  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(statep->created);

  if (down_interruptible(&statep->rxSem))
  {
     /*Release the reassembly buffer and rearm interrupt*/
     bcm_omciRelease();
    return -ERESTARTSYS;
  }

  while (!bcm_omciRxDataAvl()) {
    /* nothing to read */
    /* release the lock */
    up(&statep->rxSem);

    /* return if user has requested a non-blocking I/O */
    if (filp->f_flags & O_NONBLOCK)
    {
       /*Release the reassembly buffer and rearm interrupt*/
       bcm_omciRelease();
      return -EAGAIN;
    }

    BCM_LOG_INFO(BCM_LOG_ID_OMCI,"\"%s\" reading: going to sleep", current->comm);
    if (wait_event_interruptible(statep->readWaitQueue, bcm_omciRxDataAvl()))
    {
       /*Release the reassembly buffer and rearm interrupt*/
       bcm_omciRelease();
      return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
    }

    /* check if data is available, but first reacquire the
       lock. we have to check if data is available after we wake up in case
       another process contending for the data gets the lock first */
    if (down_interruptible(&statep->rxSem))
    {
       /*Release the reassembly buffer and rearm interrupt*/
       bcm_omciRelease();
      return -ERESTARTSYS;
    }
  }

  /* if we get here, a packet is available for reading */
  kBuf = bcm_omciReceive(&count);

  BCM_ASSERT(kBuf);

  if (copy_to_user(buf, kBuf, count)) {
#if __BITS_PER_LONG == 64
    BCM_LOG_ERROR(BCM_LOG_ID_OMCI, "Error copying packet to user: from=%p to=%p n=%lu",
        kBuf, buf, count);
#else
    BCM_LOG_ERROR(BCM_LOG_ID_OMCI, "Error copying packet to user: from=%p to=%p n=%u",
        kBuf, buf, count);
#endif
    rv = -EFAULT;
  }

  /*Release the reassembly buffer and rearm interrupt*/
  bcm_omciRelease();

  up (&statep->rxSem);

  if (rv!=0) {
    /* XXX bcm_omciReceive() should never return -EAGAIN, since we check for
       data availability before calling it. it can, however, return an error
       generated by copy_to_user() */
    return rv;
  }
  else {
#if __BITS_PER_LONG == 64
    BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"Read %lu bytes", count);
#else
    BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"Read %u bytes", count);
#endif
    return count;
  }
}

static int omciPortUp(void) {
  BCM_OmciPortInfo omciPortInfo;
  int res=0;

  /*PLOAM driver accesses must be atomic*/
   /* BCM_GPON_LOCK(flags); */ 
  res = bcm_ploamPortCtrlGetOmciPortInfo(&omciPortInfo);
/*  BCM_GPON_UNLOCK(flags);*/

  return (res==0) && omciPortInfo.enabled;
}

static ssize_t omci_write(struct file *filp, const char __user *buf, size_t count,
                          loff_t *f_pos) {
  int rv=0;

  BCM_ASSERT(statep->created);

  //BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");

  if (down_interruptible(&statep->txSem))
    return -ERESTARTSYS;

  while (omciPortUp() &&
         !bcm_omciGetTransmitSpace()) {
    /* no free tx buffers */
    /* release the lock */
    up(&statep->txSem);

    /* return if user has requested a non-blocking I/O */
    if (filp->f_flags & O_NONBLOCK)
      return -EAGAIN;

    BCM_LOG_NOTICE(BCM_LOG_ID_OMCI,"\"%s\" writing: going to sleep", current->comm);
    if (wait_event_interruptible(statep->writeWaitQueue,
                                 !omciPortUp() ||
                                 bcm_omciGetTransmitSpace()))
      return -ERESTARTSYS; /* signal: tell the fs layer to handle it */

    /* check if space is available, but first reacquire the
       lock. we have to check if space is available after we wake up in case
       another process contending for the data gets the lock first */
    if (down_interruptible(&statep->txSem))
      return -ERESTARTSYS;
  }

  if (!omciPortUp()) {
    up (&statep->txSem);
    BCM_LOG_INFO(BCM_LOG_ID_OMCI,"OMCI write failed. OMCI channel not up.");
    return -ENOLINK; /*OMCI channel not enabled*/
  }

  /* if we get here, and the there is space to transmit the packet */
  rv = bcm_omciTransmit(buf, count);

  up (&statep->txSem);

  if (rv) {
    /* XXX bcm_omciTransmit() should never return -EAGAIN, since we check for
       tx buffer space before calling it. it can, however, return an error
       generated by copy_from_user() */
    BCM_LOG_ERROR(BCM_LOG_ID_OMCI, "Failed to transmit packet (%d)", rv);
    return rv;
  }
  else {
#if __BITS_PER_LONG == 64
    BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"Wrote %lu bytes", count);
#else
    BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"Wrote %u bytes", count);
#endif
    return count;
  }
}

static unsigned int omci_poll(struct file *filp, poll_table *wait) {
  unsigned int mask = 0;

  BCM_ASSERT(statep->created);

  poll_wait(filp, &statep->readWaitQueue,  wait);
  poll_wait(filp, &statep->writeWaitQueue, wait);

  if (bcm_omciRxDataAvl())
    mask |= (POLLIN | POLLRDNORM);  /* readable */
  if (bcm_omciGetTransmitSpace())
    mask |= (POLLOUT | POLLWRNORM); /* writable */

  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"mask=0x%x\n", mask);

  return mask;
}

/*
 * The file operations for the omci device
 */
static struct file_operations omci_fops = {
  .owner = THIS_MODULE,
  .llseek = no_llseek,
  .read = omci_read,
  .write = omci_write,
  .unlocked_ioctl = omci_ioctl,
#if defined(CONFIG_COMPAT)
  .compat_ioctl = omci_ioctl,
#endif
  .poll = omci_poll,
  .open = omci_open,
  .release = omci_release,
};

/**
 * Local Functions:
 **/

/**
 * Public Functions:
 **/

void bcm_omciUsrWakeupReaders(int minor) {
  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(statep->created);

  if (statep->nreaders)
    wake_up_interruptible(&statep->readWaitQueue);
}

void bcm_omciUsrWakeupWriters(int minor) {
  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(statep->created);

  wake_up_interruptible(&statep->writeWaitQueue);
}

int bcm_omciUsrGetPacket(char *to, const char __user *from, unsigned int n) {
  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  if (copy_from_user(to, from, n)) {
    BCM_LOG_ERROR(BCM_LOG_ID_OMCI, "Error copying packet from user: from=%p to=%p n=%d",
                  from, to, n);
    return -EFAULT;
  }

  return 0;
}

int __devinit bcm_omciUsrCreate(void) {
  int rv=0;

  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(statep->created==0);

  /* Register the driver and link it to our fops */
  rv = register_chrdev(omciMajor, BCM_OMCI_DEVICE_NAME, &omci_fops);
  if (rv < 0) {
    BCM_LOG_ERROR(BCM_LOG_ID_OMCI,"unable to register %s device (major=%d)",
                  BCM_OMCI_DEVICE_NAME, omciMajor);
    return rv;
  }

  memset(statep, 0, sizeof(BCM_OmciUsrState));
  init_waitqueue_head(&(statep->readWaitQueue));
  init_waitqueue_head(&(statep->writeWaitQueue));
  init_MUTEX(&statep->rxSem);
  init_MUTEX(&statep->txSem);
  init_MUTEX(&statep->ioctlSem);

  statep->created = 1;

  /* succeeded */
  BCM_LOG_INFO(BCM_LOG_ID_OMCI,"OMCI Device %s Major %d Minor %d created",
               BCM_OMCI_DEVICE_NAME, omciMajor, omciMinor);
  return 0;
}

void bcm_omciUsrDelete(void) {
  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");

 /**
  * Delete dev
  **/
  if (statep->created) {
    unregister_chrdev(omciMajor, BCM_OMCI_DEVICE_NAME);
    omciMajor=OMCI_MAJOR;
    statep->created=0;
  }

  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"omciUsr deleted.");
}

