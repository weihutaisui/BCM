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

#include <bcmtypes.h>
#include <bcm_omci_api.h>

#include "bcm_omci.h"
#include "bcm_omciUsr.h"

#include <bcm_map.h>
#include <linux/bcm_log.h>
#include <rdpa_cpu.h>
#include <rdpa_ag_cpu.h>
#include <rdpa_mw_cpu_queue_ids.h>
#include <rdpa_gpon_misc.h>

#undef OMCI_CYCLE_PROFILE

#define OMCI_CRC32_POLYNOMIAL 0x04c11db7L /* Standard CRC-32 polynomial */

/**
 * Local Types
 **/

typedef struct OmciDevCtrl {
    int created;
    int initialized;
   int      discardTxCount;             /* debug feature: discard next TxCount outgoing messages*/
    uint8_t reassemblyBuf[BCM_OMCI_RX_MSG_MAX_SIZE_BYTES];
    uint8_t *reassemblyBufp;
    int rxDataAvl;
    
   struct tasklet_struct rxTasklet;

    uint32_t dataDumpQid;

    BCM_OmciCounters counters;

} OmciDevCtrl;

/**
 * Local variables
 **/
static OmciDevCtrl omciDevCtrl = {
  .created = 0,
  .initialized = 0
};

static OmciDevCtrl *pOmciDevCtrl = &omciDevCtrl;

/**
 * Local Functions:
 **/

/*
 *  omciRxTasklet: this tasklet processes received packets. called by
 *  the interrupt handler
 */
static void omciRxTasklet(unsigned long data) 
{
  uint32_t packetSize=0;
  bdmf_error_t rc = BDMF_ERR_OK;
  rdpa_cpu_rx_info_t info = {};
  uint32_t crcCalc, crcRx;
  uint8_t *pBufEnd = pOmciDevCtrl->reassemblyBuf + sizeof(pOmciDevCtrl->reassemblyBuf);
  uint8_t *real_data;
  BCM_ASSERT(pOmciDevCtrl->initialized);
  /* clear the interrupt here */
  rdpa_cpu_int_clear(rdpa_cpu_host, RDPA_OMCI_CPU_RX_QUEUE_ID);


  while (1)
 {
       rc = rdpa_cpu_packet_get(rdpa_cpu_host, RDPA_OMCI_CPU_RX_QUEUE_ID, &info);      
       if (rc != BDMF_ERR_OK)
       {
           /* Usually there are no more packets to read so rc=BDMF_ERR_NO_MORE */
         if (rc != BDMF_ERR_NO_MORE)
         {
             BCM_LOG_ERROR(BCM_LOG_ID_OMCI,"Error %d while trying to get packet from CPU queue %d (OMCI queue)",
              rc,RDPA_OMCI_CPU_RX_QUEUE_ID);
         }
         rdpa_cpu_int_enable(rdpa_cpu_host, RDPA_OMCI_CPU_RX_QUEUE_ID);
         break;
       }

       packetSize = info.size;
       real_data = (uint8_t *)info.data + info.data_offset;
       if (packetSize > BCM_OMCI_RX_MSG_MAX_SIZE_BYTES)
       {
                BCM_LOG_ERROR(BCM_LOG_ID_OMCI,"packetSize: %u",packetSize);
          #define BCM_OMCI_RX_TASKLET_FRAG_DDID 1
                BCM_DATADUMP_IMPORTANT(BCM_LOG_ID_OMCI, pOmciDevCtrl->dataDumpQid, BCM_OMCI_RX_TASKLET_FRAG_DDID,
                                real_data, packetSize);
 
                BCM_ASSERT(0);
        }/*read packet is too long*/

        #define BCM_OMCI_RX_TASKLET_FRAG_DDID 1
        BCM_DATADUMP_DETAIL(BCM_LOG_ID_OMCI, pOmciDevCtrl->dataDumpQid, BCM_OMCI_RX_TASKLET_FRAG_DDID,
                        real_data, packetSize);
        BCM_LOG_INFO(BCM_LOG_ID_OMCI,"OMCI fragment dumped");

        /*Check for overflow*/
        if (pBufEnd - pOmciDevCtrl->reassemblyBufp < packetSize)
        {
          BCM_LOG_INFO(BCM_LOG_ID_OMCI,"OMCI reassembly buffer overflow");

          /*Reset the reassembly buffer pointer*/
          pOmciDevCtrl->reassemblyBufp = pOmciDevCtrl->reassemblyBuf;
        }/*overflow*/

        /* copy packet to reassembly buffer */
        memcpy(pOmciDevCtrl->reassemblyBufp,  (void*)real_data, packetSize);
        bdmf_sysb_databuf_free((void*)info.data, 0); /*The original pointer should be used by free*/

        pOmciDevCtrl->reassemblyBufp += packetSize;
        pOmciDevCtrl->counters.rxBytes += packetSize;
        ++pOmciDevCtrl->counters.rxFragments;
        
         if (packetSize > 0)
        {
            BCM_LOG_DEBUG(BCM_LOG_ID_OMCI, "OMCI message reassembled. msgLen=%d", packetSize);

#define BCM_OMCI_RX_TASKLET_MSG_DDID 2
            BCM_DATADUMP_INFO(BCM_LOG_ID_OMCI, pOmciDevCtrl->dataDumpQid, BCM_OMCI_RX_TASKLET_MSG_DDID,
                                pOmciDevCtrl->reassemblyBuf, packetSize);

             if (packetSize < 4) {
                BCM_LOG_DEBUG(BCM_LOG_ID_OMCI, "OMCI message < 4 bytes.");
               ++pOmciDevCtrl->counters.rxDiscarded;
                /*Reset the reassembly buffer pointer*/
                pOmciDevCtrl->reassemblyBufp = pOmciDevCtrl->reassemblyBuf;
                continue;
             }

              crcCalc = rdpa_calc_ds_omci_crc_or_mic (pOmciDevCtrl->reassemblyBuf, packetSize - 4);

              crcRx = (pOmciDevCtrl->reassemblyBuf[packetSize-4]<<24) |
                (pOmciDevCtrl->reassemblyBuf[packetSize-3]<<16) |
                (pOmciDevCtrl->reassemblyBuf[packetSize-2]<<8) |
                (pOmciDevCtrl->reassemblyBuf[packetSize-1]);

              if (crcCalc != crcRx) {
                    BCM_LOG_NOTICE(BCM_LOG_ID_OMCI, "OMCI message CRC error. Calculated: 0x%x, Received: 0x%x", crcCalc, crcRx);
                ++pOmciDevCtrl->counters.rxDiscarded;
                /*Reset the reassembly buffer pointer*/
                pOmciDevCtrl->reassemblyBufp = pOmciDevCtrl->reassemblyBuf;
                    continue;
              }

               ++pOmciDevCtrl->counters.rxPackets;
               pOmciDevCtrl->rxDataAvl=1;

              /* wake up readers */
              /* currently only one device is supported, so set minor to 0 */
              bcm_omciUsrWakeupReaders(0);
              /* Tasklet can't be disabled from tasklet excecution function itself.
                   * To take care of the concurrency - we keep the OMCI_RX interrupt
                   * disabled until the reassembly buffer is free again. The interrupt
                   * is enabled by OMCID (OMCI receive application during bcm_omciRelease) */
              break;
        }/*if  (packetSize > 0)*/
    }
}


#ifdef OMCI_CYCLE_PROFILE
int cyclesSet=0;
#endif

void bcm_omciGetSwCounters(OUT BCM_OmciCounters* counters, int reset) {
  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(pOmciDevCtrl->initialized);
  BCM_ASSERT(counters);
  *counters = pOmciDevCtrl->counters;

  if (reset)
    memset(&pOmciDevCtrl->counters, 0, sizeof(BCM_OmciCounters));
}

void* bcm_omciDevCtrlPtr(void) {
  return pOmciDevCtrl;
}

int bcm_omciRxDataAvl(void) {
  BCM_ASSERT(pOmciDevCtrl->initialized);
  return pOmciDevCtrl->rxDataAvl;
}

/*Release the Reassembly Buffer*/
void bcm_omciRelease(void) {
  static int read_cycle=0;
  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(pOmciDevCtrl->initialized);
  
  pOmciDevCtrl->rxDataAvl=0;
  pOmciDevCtrl->reassemblyBufp = pOmciDevCtrl->reassemblyBuf;
  /* OMCI_RX interrupt is enabled by tasklet */
  /* No need to enable tasklet because we not did not disable them */

  /* Also reschedule the rx tasklet. There might be more
   * data pending.*/
  
  /* In order to speed up reading packets from the OMCI RX Qeueu, we call 
     the taskelt function directly. We limit this operation to BL_OMCI_QUEUE_SIZE
     in order to prevent starvation from other tasks.  */
  if (read_cycle < RDPA_OMCI_QUEUE_SIZE) 
  {
      read_cycle ++;
      omciRxTasklet(0);
  }
  else
  {
      read_cycle = 0;
      tasklet_schedule(&pOmciDevCtrl->rxTasklet);
  }
}

/*
 *  bcm_omciReceive: this function receives an omci packet
 */
uint8_t* bcm_omciReceive(size_t *length) {
  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(pOmciDevCtrl->initialized);

  *length = pOmciDevCtrl->reassemblyBufp - pOmciDevCtrl->reassemblyBuf;
  return pOmciDevCtrl->reassemblyBuf;
}




int bcm_omciGetTransmitSpace(void) {
    return 1;
}



void bcm_omciSetDiscardTxCount(int discardTxCount) {
  BCM_ASSERT(pOmciDevCtrl->initialized);

  pOmciDevCtrl->discardTxCount = discardTxCount;
}

static int bcm_omciDiscardTx(void) {
  int res = 0;
  BCM_ASSERT(pOmciDevCtrl->initialized);

  if (pOmciDevCtrl->discardTxCount > 0) {
    res = 1;
    --pOmciDevCtrl->discardTxCount;
  }

  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"OMCI Tx Discard = %d", res);

  return res;
}

int OmciPacketTrailerSet(uint8_t *buf, unsigned int length)
{
    uint32 crc;
    int new_len = length + SVC_UINT32_BYTE_COUNT;

    if (*(buf + OMCI_MESSAGE_DEVID_OFFSET) == 0xa)
    {
        SvcHtonl(buf + OMCI_MESSAGE_SET_A_SDU_OFFSET, OMCI_PACKET_CPCS_SDU_LEN);
    }
    crc = rdpa_calc_us_omci_crc_or_mic (buf, length);
    SvcHtonl(buf + length, crc);

    return new_len; 
}

/************************************************************************/
/*
 *  bcm_omciTransmit: this function transmits a omci packet
 */
/************************************************************************/
int bcm_omciTransmit(const char __user *pBuf, unsigned long length)
{
    int length_with_mic, rv = 0;
    char *Omcibuff = NULL;
    rdpa_cpu_tx_info_t info = {
        .method = rdpa_cpu_tx_port,
        .port = rdpa_if_none,
        .cpu_port = rdpa_cpu_host,
        .no_lock = 0,
        .x.wan.queue_id = RDPA_OMCI_TCONT_QUEUE_ID,
        .x.wan.flow = RDPA_OMCI_FLOW_ID,
    };

    info.port = rdpa_wan_type_to_if(rdpa_wan_gpon); 
    BCM_ASSERT(pOmciDevCtrl->initialized);
    BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"Length %lu", length);

    /* error checking */
    if (!pBuf || !length || (length > BCM_OMCI_TX_MSG_MAX_SIZE_BYTES - SVC_UINT32_BYTE_COUNT)) {
        BCM_LOG_ERROR(BCM_LOG_ID_OMCI, "Invalid input parameter(s): pBuf %p length %lu",
                pBuf, length);

        return -EINVAL;
    }

    /* debug feature check if outgoing message should be discarded, to simulate msg loss*/
    if (bcm_omciDiscardTx())
        return  0;

    /* Allocate extra space for MIC, which will be added later */
    Omcibuff = kmalloc(length + SVC_UINT32_BYTE_COUNT, GFP_KERNEL);
    if (!Omcibuff)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_OMCI, "Failed to allocate buffer for OMCI message\n");
        return -EFAULT;
    }

    rv = bcm_omciUsrGetPacket(Omcibuff, pBuf, length);

#define BCM_OMCI_TRANSMIT_PACKET_DDID 3
    BCM_DATADUMP_DETAIL(BCM_LOG_ID_OMCI, pOmciDevCtrl->dataDumpQid, BCM_OMCI_TRANSMIT_PACKET_DDID, Omcibuff, length);
    if (rv)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_OMCI, "Error copying packet from user: from=%p to=%p n=%lu",
                pBuf, Omcibuff, length);
        rv = -EFAULT;
        goto exit;
    }
    length_with_mic = OmciPacketTrailerSet(Omcibuff, length);

    rv = rdpa_cpu_send_raw(Omcibuff, length_with_mic, &info);
    if (rv)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_OMCI, "Error sending OMCI packet to pon : buff=%p length=%lu",
                Omcibuff, length);

        goto exit;
    }

    /* update counters */
    pOmciDevCtrl->counters.txPackets++;
    pOmciDevCtrl->counters.txBytes += length;

exit:
    if (Omcibuff)
        kfree(Omcibuff);
    return rv;
}

void OmciCpuRxCallBack(long dummy)
{
    rdpa_cpu_int_disable(rdpa_cpu_host, RDPA_OMCI_CPU_RX_QUEUE_ID);
    /* schedule the rx tasklet */
    tasklet_schedule(&pOmciDevCtrl->rxTasklet);

}/* End of OmciCpuRxCallBack() */

int __devinit bcm_omciCreate() 
{
    int rv=0;
    bdmf_object_handle cpu = NULL;
    bdmf_object_handle system_obj = NULL;
    rdpa_cpu_rxq_cfg_t rxq_cfg;
    bdmf_error_t rc = BDMF_ERR_OK;

    rc = rdpa_system_get(&system_obj);
    rc = rc ? rc : rdpa_cpu_get(rdpa_cpu_host, &cpu);
    rc = rc ? rc : rdpa_cpu_rxq_cfg_get(cpu, RDPA_OMCI_CPU_RX_QUEUE_ID, &rxq_cfg);

    rxq_cfg.size = RDPA_OMCI_QUEUE_SIZE;
    rxq_cfg.isr_priv = RDPA_OMCI_CPU_RX_QUEUE_ID;
    rxq_cfg.rx_isr = OmciCpuRxCallBack;
    rc = rc ? rc : rdpa_cpu_rxq_cfg_set(cpu, RDPA_OMCI_CPU_RX_QUEUE_ID, &rxq_cfg);
    
#ifdef XRDP
    /* Map reason OMCI to special TC, and TC to RDPA_OMCI_CPU_RX_QUEUE_ID.
       For simplicity, use TC = RDPA_OMCI_CPU_RX_QUEUE_ID */
    rc = rdpa_system_cpu_reason_to_tc_set(system_obj, rdpa_cpu_rx_reason_omci, RDPA_OMCI_CPU_RX_QUEUE_ID);
    rc = rc ? rc : rdpa_cpu_tc_to_rxq_set(cpu, RDPA_OMCI_CPU_RX_QUEUE_ID, RDPA_OMCI_CPU_RX_QUEUE_ID);
#else
    {
        rdpa_cpu_reason_cfg_t reason_cfg = {};
        rdpa_cpu_reason_index_t cpu_reason;  

        cpu_reason.reason = rdpa_cpu_rx_reason_omci;
        cpu_reason.dir = rdpa_dir_ds;
        reason_cfg.queue = RDPA_OMCI_CPU_RX_QUEUE_ID;
        reason_cfg.meter = BDMF_INDEX_UNASSIGNED;
        rc = rc ? rc : rdpa_cpu_reason_cfg_set(cpu, &cpu_reason, &reason_cfg);
    }
#endif

    if (cpu)
        bdmf_put(cpu);
    if (system_obj)
        bdmf_put(system_obj);

    if (rc)
    {
        BCM_LOG_ERROR(BCM_LOG_ID_OMCI, "Failed to configure OMCI Rx CPU queue: rc=%d", rc);
        return rc;
    }

    memset(pOmciDevCtrl, 0, sizeof(OmciDevCtrl));

    pOmciDevCtrl->dataDumpQid = BCM_DATADUMP_CREATE_Q("omci");


    /* initialize the omci tasklet */
    tasklet_init(&pOmciDevCtrl->rxTasklet, omciRxTasklet, 0);

/* initialize the user interface */
    rv = bcm_omciUsrCreate();
    if (rv) 
    {
        BCM_LOG_NOTICE(BCM_LOG_ID_OMCI,"omciUsrCreate failed: %d", rv);
        bcm_omciDelete();
        return rv;
    }

    pOmciDevCtrl->created = 1;

    BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"omci driver created.");
    return 0;
}


void bcm_omciInit(void) {
  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(pOmciDevCtrl->created);

  BCM_LOG_INFO(BCM_LOG_ID_OMCI,"(Re)Initializing omci driver.");

  memset(&pOmciDevCtrl->counters, 0, sizeof(BCM_OmciCounters));

  pOmciDevCtrl->initialized = 1;

  bcm_omciReset();

  BCM_LOG_INFO(BCM_LOG_ID_OMCI,"omci driver initialized.");
}

void bcm_omciReset(void) {
  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");
  BCM_ASSERT(pOmciDevCtrl->initialized);

  BCM_LOG_INFO(BCM_LOG_ID_OMCI,"Reseting omci driver.");

  /* disable tasklets before updating the shared resources
   * NOTE : There is a possibilty that OMCI task is reading/writing the data
   * while we are resetting the BDs pointers; 
   * At this time I am not spending time to completely change the driver design */
  tasklet_disable(&pOmciDevCtrl->rxTasklet);
  pOmciDevCtrl->reassemblyBufp = pOmciDevCtrl->reassemblyBuf;
  pOmciDevCtrl->rxDataAvl=0;

  /* enable tasklets */
  tasklet_enable(&pOmciDevCtrl->rxTasklet);
  bcm_omciUsrWakeupWriters(0);
}


void bcm_omciDelete(void) {
  BCM_LOG_DEBUG(BCM_LOG_ID_OMCI,"");

  if (pOmciDevCtrl->created)
    bcm_omciUsrDelete();

  BCM_DATADUMP_DELETE_Q(pOmciDevCtrl->dataDumpQid);

  pOmciDevCtrl->created=0;

  BCM_LOG_NOTICE(BCM_LOG_ID_OMCI,"omci driver deleted.");
}




