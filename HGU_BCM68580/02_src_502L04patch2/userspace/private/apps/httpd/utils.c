/***********************************************************************
 *
 *  Copyright (c) 2000-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include "cms_util.h"
#include "cgi_main.h"
#include "secdefs.h"
#include "syscall.h"


/*****************************************************************************

         THIS FILE IS OBSOLETE AND WILL BE DELETED SOON.
         DO NOT USE THE FUNCTIONS IN THIS FILE OR ADD
         ANY MORE FUNCTIONS TO THIS FILE.

*****************************************************************************/



/***************************************************************************
// Function Name: BcmDb_isQosEnabled.
// Description  : check any PVC that has Qos enabled.
// Returns      : TRUE or FALSE.
****************************************************************************/
int BcmDb_isQosEnabled(void) {
    int enabled = FALSE;
#if 0 //~    
    UINT16 vccId = 0;   // set to 0 to search vcc from beginning
    IFC_ATM_VCC_INFO vccInfo;
   
    while ( BcmDb_getVccInfoNext(&vccId, &vccInfo) == DB_VCC_GET_OK ) {
        if ( vccInfo.enblQos == TRUE ) {
            enabled = TRUE;
            break;
        }
    }
#endif //~

    return enabled;

}


//**************************************************************************
// Function Name: BcmDb_getNumberOfUsedQueue
// Description  : get number of current used queues.
// Parameters   : none.
// Returns      : number of current used queues.
//**************************************************************************
int BcmDb_getNumberOfUsedQueue(void) {
#if 0//~
    int numQueue = 0;
    UINT16 retLen = 0, vccId = 0;
    IFC_ATM_VCC_INFO vccInfo;
    PSI_STATUS status = PSI_STS_OK;
    PSI_HANDLE psiVcc = BcmPsi_appOpen(ATM_VCC_APPID);

    for ( vccId = 1; vccId <= ATM_VCC_MAX; vccId++ ) {
        status = BcmPsi_objRetrieve(psiVcc, vccId, &vccInfo,
                                    sizeof(vccInfo), &retLen);
        if ( status == PSI_STS_OK ) { 
            numQueue++;   // use 1 queue for each PVC
#ifndef SUPPORT_TC
            if ( vccInfo.enblQos == TRUE )
                numQueue += 2;  // use 2 more queues if PVC has QoS enabled
#endif
        }
    }

    return numQueue;
#endif //~
return 0;//~
}

SEC_STATUS BcmScm_removePortTrigger(PSEC_PRT_TRG_ENTRY pObject) {
//~   SecCfgMngr *objScm = SecCfgMngr::getInstance();
//~   return (objScm->getPrtTrgTbl()->remove(pObject));
   return 0;
}

SEC_STATUS BcmScm_addPortTrigger(PSEC_PRT_TRG_ENTRY pObject) {
//~   SecCfgMngr *objScm = SecCfgMngr::getInstance();
//~   return (objScm->getPrtTrgTbl()->set(pObject));
   return 0;
}



//**************************************************************************
// Function Name: BcmDb_getNumberOfWanConnections
// Description  : get number of WAN connections.
// Returns      : number of WAN connections.
//**************************************************************************
int BcmDb_getNumberOfWanConnections(void) {
    int counter = 0;
#if 0 //~    
    char appWan[BUFLEN_16];
    UINT16 retLen = 0, vccId = 0, conId = 0;
    WAN_CON_INFO wanInfo;
    IFC_ATM_VCC_INFO vccInfo;
    PSI_STATUS status = PSI_STS_OK;
    PSI_HANDLE psiVcc = NULL;
    PSI_HANDLE psiWan = NULL;

    psiVcc = BcmPsi_appOpen(ATM_VCC_APPID);
 
    for ( vccId = 1; vccId <= ATM_VCC_MAX; vccId++ ) {
        status = BcmPsi_objRetrieve(psiVcc, vccId, &vccInfo,
                                    sizeof(vccInfo), &retLen);
        if ( status == PSI_STS_OK ) {
            sprintf(appWan, "wan_%d_%d", vccInfo.vccAddr.usVpi, vccInfo.vccAddr.usVci);
            psiWan = BcmPsi_appOpen(appWan);
            for ( conId = 1; conId <= IFC_WAN_MAX; conId++ ) {
                status = BcmPsi_objRetrieve(psiWan, conId, &wanInfo,
                                            sizeof(wanInfo), &retLen);
                if ( status == PSI_STS_OK )
                    counter++;
            }
        }
    }
#endif //~  
    return counter;
  
}


