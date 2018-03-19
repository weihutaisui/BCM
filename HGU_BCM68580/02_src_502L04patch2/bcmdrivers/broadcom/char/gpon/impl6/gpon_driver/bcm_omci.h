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
#ifndef _BCM_OMCI_H_
#define _BCM_OMCI_H_

#include "bcm_OS_Deps.h"

#include <bcm_map.h>

#include "bcm_gponBasicDefs.h"


/* Type definitions. */
#define OMCI_MESSAGE_DEVID_OFFSET 3
#define OMCI_PACKET_CPCS_SDU_LEN 0x0028
#define OMCI_MESSAGE_SET_A_SDU_OFFSET OMCI_PACKET_CPCS_SDU_LEN

#define SVC_UINT32_BYTE_COUNT 4

/* Big-endian alignment alignment un-restricted. */
#define SvcHtons(addrM, valueM)    (*(uint16*)(addrM) = htons(valueM))
#define SvcHtonl(addrM, valueM)    (*(uint32*)(addrM) = htonl(valueM))


/* Type definitions. */

typedef struct {
  uint32_t rxIrqCount;
  uint32_t txIrqCount;
  uint32_t txPackets;
  uint32_t txBytes;
  uint32_t rxPackets;
  uint32_t rxFragments;
  uint32_t rxDiscarded;
  uint32_t rxBytes;

} BCM_OmciCounters;

/*Debug feature: discard next x messages*/
void bcm_omciSetDiscardTxCount(int discardTxCount);

void bcm_omciGetSwCounters(OUT BCM_OmciCounters* counters, int reset);

uint32_t getCrc32(uint32_t crcAccum, uint8_t *pBuf, uint32_t size);
int bcm_omciRxDataAvl(void);
void* bcm_omciDevCtrlPtr(void);
void bcm_omciRelease(void);
uint8_t* bcm_omciReceive(size_t *length);
int bcm_omciGetTransmitSpace(void);
int bcm_omciTransmit(const char __user *pBuf, unsigned long length);
void bcm_omciTxHandler(void);

#define BCM_OMCI_IRQ_MASK_OMCI_RX 1
#define BCM_OMCI_IRQ_MASK_OMCI_TX 2
void bcm_omciSetIrqMask(uint32_t irqMask);

/*Equivalent of a constructor. Performs one-time setup actions.*/
int __devinit bcm_omciCreate(void);

/*Reset the OMCI module. Reset-safe data will be preserved.*/
void bcm_omciReset(void);

/*(Re)Initialize this module*/
void bcm_omciInit(void);

/*Equivalent of a destructor. Performs clean-up actions.*/
void bcm_omciDelete(void);

#endif /*_BCM_OMCI_H_*/
