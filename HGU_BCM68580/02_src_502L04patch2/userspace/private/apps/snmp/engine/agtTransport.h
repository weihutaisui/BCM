/***************************************************************************
** <:copyright-BRCM:2017:proprietary:standard
** 
**    Copyright (c) 2017 Broadcom 
**    All Rights Reserved
** 
**  This program is the proprietary software of Broadcom and/or its
**  licensors, and may only be used, duplicated, modified or distributed pursuant
**  to the terms and conditions of a separate, written license agreement executed
**  between you and Broadcom (an "Authorized License").  Except as set forth in
**  an Authorized License, Broadcom grants no license (express or implied), right
**  to use, or waiver of any kind with respect to the Software, and Broadcom
**  expressly reserves all rights in and to the Software and all intellectual
**  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
**  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
**  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
** 
**  Except as expressly set forth in the Authorized License,
** 
**  1. This program, including its structure, sequence and organization,
**     constitutes the valuable trade secrets of Broadcom, and you shall use
**     all reasonable efforts to protect the confidentiality thereof, and to
**     use this information only in connection with your use of Broadcom
**     integrated circuit products.
** 
**  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
**     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
**     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
**     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
**     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
**     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
**     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
**     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
**     PERFORMANCE OF THE SOFTWARE.
** 
**  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
**     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
**     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
**     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
**     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
**     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
**     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
**     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
**     LIMITED REMEDY.
** :>
**
 * $brcm_Workfile:$
 * $brcm_Revision:$
 * $brcm_Date:$
 *
 * [File Description:]
 *       UDP, ATM PVC and ADSL EOC transports implemention.
 * Revision History:
 *       author:  ytran
 * $brcm_Log:$
 *
 ***************************************************************************/

#ifndef _AGT_TRANSPORT_H_
#define _AGT_TRANSPORT_H_

#include <sys/types.h>

#define ADSL_DEVICE     "/dev/bcmadsl0"
#define TRANSPORT_NAME_LEN_MAX   3
#define TRANSPORT_NAME_UDP       "UDP"
#define TRANSPORT_NAME_UDP_TRAP  "UDT"
#define TRANSPORT_NAME_EOC       "EOC"
#define TRANSPORT_NAME_ATM       "ATM"
#define TRANSPORT_NAME_SMD       "SMD"

typedef struct transport_info {
  char name[TRANSPORT_NAME_LEN_MAX];
  int socket;
  int (*send_fn) (struct transport_info* pTransport, void* msg);
  int (*receive_fn) (struct transport_info* pTransport, raw_snmp_info_t *message);
  int (*close_fn) (void);
  //  int (*formataddr_fn) ();
  struct transport_info* next;
  int trapEnable;                 /* send trap only if TRUE */
  int dataEnable;                 /* send data only if TRUE */
  int linkup;                     /* can only send if this is UP, used for EOC channel */
  int temp;                       /* this is a temporary link; it would be closed if another
								  transport is active */
} TRANSPORT_INFO, *pTRANSPORT_INFO;

typedef struct transport_list {
  pTRANSPORT_INFO transport;
  pTRANSPORT_INFO tail;
} TRANSPORT_LIST, *pTRANSPORT_LIST;

#define ADSL_EOC_LINKUP_MSG_LEN    3
#define AAL5_TRANSPORT_VPI         0
#define AAL5_TRANSPORT_VCI         16

void snmpOpenTransport(void);
void snmpCloseTransport(void);
void snmpAddTransportToList(pTRANSPORT_INFO pNew);
pTRANSPORT_INFO snmpRemoveTransportFromList(char *name);
int snmpTransportSelect(fd_set *readfds);
int snmpTransportRead(fd_set *readfds);

#endif /* _AGT_TRANSPORT_H_ */

