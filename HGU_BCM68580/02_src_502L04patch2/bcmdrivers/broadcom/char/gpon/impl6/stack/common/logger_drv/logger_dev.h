/*
* <:copyright-BRCM:2015:proprietary:gpon
* 
*    Copyright (c) 2015 Broadcom 
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

/* -*- C -*-
 * gpon_drv_main.h -- definitions for the gpon driver module
 *
  */

#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>

#define LOGGER_DEVICE_NAME					"rgs_logger"


/*
 * The bare device is a variable-length region of memory.
 * Use a linked list of indirect blocks.
 *
 * "gpon_drv_dev->data" points to an array of pointers, each
 * pointer refers to a memory page.
 *
 * The array (quantum-set) is LOGGER_DRV_QSET long.
 */
#define LOGGER_DRV_QUANTUM  4000 /* use a quantum size like scull */
#define LOGGER_DRV_QSET     500

#ifdef __KERNEL__
struct logger_dev_t {
	void **data;
	struct logger_dev_t *next;  /* next listitem */
	int vmas;                 /* active mappings */
	int quantum;              /* the current allocation size */
	int qset;                 /* the current array size */
	size_t size;              /* 32-bit will suffice */
	struct semaphore sem;     /* Mutual exclusion */
	struct cdev cdev;
	int count;
	unsigned long log_id;
};

#endif


/*
 * Prototypes for shared functions
 */
int logger_drv_trim(struct logger_dev_t *dev);
struct logger_dev_t *logger_drv_follow(struct logger_dev_t *dev, int n);


/*
 * Ioctl definitions
 */

#define LOGGER_DRV_IOC_MAGIC  'L'

#define LOGGER_DRV_IOCRESET    _IO(LOGGER_DRV_IOC_MAGIC, 0)

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly
 * G means "Get" (to a pointed var)
 * Q means "Query", response is on the return value
 * X means "eXchange": G and S atomically
 * H means "sHift": T and Q atomically
 */
#define LOGGER_DRV_VERSION     	_IOR(LOGGER_DRV_IOC_MAGIC,   1, int)
#define LOGGER_DRV_SESSION_INIT	_IOW(LOGGER_DRV_IOC_MAGIC,   2, int)

#define LOGGER_DRV_IOC_MAXNR 4


