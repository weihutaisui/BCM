/*
* <:copyright-BRCM:2012:proprietary:epon
* 
*    Copyright (c) 2012 Broadcom 
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Teknovus.h"

#include "Build.h"
#include "OnuOs.h"
#include "Holdover.h"
#include "OamOnu.h"
#include "OntConfigDb.h"
#include "OntmTimer.h"
#include "EponDevInfo.h"
#include "PonManager.h"
#include "TkOs.h"
#include "UniManager.h"
#include "PonMgrEncrypt.h"
#include "UniConfigDb.h"
#if defined(DPOE_OAM_BUILD)  || defined(BCM_OAM_BUILD)
#include "Stats.h"
#endif
#if INCLUDE_PPPOE
#include "PppoE.h"
#endif

#include "PowerSave.h"
#include "UserTraffic.h"
#include "OamProcessInit.h"
#include "OsAstMsgQ.h"
#include "eponctl_api.h"
#include "rdpa_types.h"
#include "rdpactl_api.h"

#ifdef CTC_OAM_BUILD
#include "CtcOamIf.h"
#endif

#ifdef DPOE_OAM_BUILD
#include "DpoeOamIf.h"
#endif

#ifdef BCM_OAM_BUILD
#include "TekOamIf.h"
#include "SampleOam.h"
#include "SampleOamIf.h"
#endif


OamHandlerCB *oamVenderHandlerHead = NULL;

OamHandlerCB *OamProcessAllocCB(void)
{   
    OamHandlerCB *cbPtr = NULL;

    cbPtr = malloc(sizeof(OamHandlerCB));
    if (cbPtr == NULL)
    {
        printf("allocate memory for CB failed\n");
        return NULL;
    }
    
    //init the cb structure
    memset(cbPtr, 0, sizeof(OamHandlerCB));
    cbPtr->next = NULL;

    //add these CB to list tail
    if(oamVenderHandlerHead == NULL)//head
        oamVenderHandlerHead = cbPtr;
    else
    {
        OamHandlerCB *tmpPtr = oamVenderHandlerHead;
        
        while(tmpPtr->next)
            tmpPtr = tmpPtr->next;

        tmpPtr->next = cbPtr;
    }

    return cbPtr;
}

OuiVendor oamValToOamVendor(U32 oamVal)
{
    OuiVendor oam = OuiOamEnd;

    switch (oamVal)
    {
        case TekOuiVal:
            oam = OuiTeknovus;
            break;
            
        case CtcOuiVal:
            oam = OuiCtc;
            break;
            
        case NttOuiVal:
            oam = OuiNtt;
            break;
            
        case DasanOuiVal:
            oam = OuiDasan;
            break;
            
        case DpoeOuiVal:
            oam = OuiDpoe;
            break;
            
        case KtOuiVal:
            oam = OuiKt;
            break;
            
        case PmcOuiVal:
            oam = OuiPmc;
            break;

        default:
            break;
    }

    return oam;
}


static int OamModeSet(U8 vendorMask)
{
    if (vendorMask & OAM_DPOE_SUPPORT)
        return rdpaCtl_set_epon_mode(rdpa_epon_dpoe);
    else if (vendorMask & OAM_BCM_SUPPORT)
        return rdpaCtl_set_epon_mode(rdpa_epon_bcm);
    else if (vendorMask & OAM_CUC_SUPPORT)
        return rdpaCtl_set_epon_mode(rdpa_epon_cuc);
    else
        return rdpaCtl_set_epon_mode(rdpa_epon_ctc);
}


static int EponDataPathInit(U8 vendorMask)
{
    int rc = 0;
   
#ifdef CTC_OAM_BUILD
    if ((!vendorMask) || 
        (vendorMask & OAM_CTC_SUPPORT) || (vendorMask & OAM_CUC_SUPPORT))
    {
        rc = VendorCtcDataPathInit();
    }
#endif

#ifdef DPOE_OAM_BUILD
    if (vendorMask & OAM_DPOE_SUPPORT)
    {
        rc = VendorDpoeDataPathInit();
    }
#endif

#ifdef BCM_OAM_BUILD
    if (vendorMask & OAM_BCM_SUPPORT)
    {
        rc = VendorTekDataPathInit();
    }
#endif
    
    rc = (rc != 0)? rc : eponStack_CtlLoadEponCfg(EponLoadCfgCur);  

    if (EponDevGetTxLaserPowerOff())
    {
        rc = (rc != 0)? rc : eponStack_CtlSetTxPower(TRUE, 0xFFFF);
    }
    else
    {
        rc = (rc != 0)? rc : eponStack_CtlSetTxPower(TRUE, 0);
    }
    
    return rc;
}


void OamProcessInit(U8 vendorMask)
{
    OamModeSet(vendorMask);
#ifndef DESKTOP_LINUX 
    EponDevInit();
    EponDataPathInit(vendorMask);
#endif
    OnuOsInit();
    TkOsInit ();
    OntCfgDbInit();    
    OsAstMsgQInit();
    OntmTimerInit ();
#if defined(DPOE_OAM_BUILD)  || defined(BCM_OAM_BUILD)     
    StatsInit();
#endif
    //if vendorMask equals zero, it means no configuration from eponapp
    //just read the value from MDM
#ifdef CTC_OAM_BUILD
    //ctc callback registration
    VendorCtcInit(vendorMask);
#endif

#ifdef DPOE_OAM_BUILD
    //dpoe callback registration
    VendorDpoeInit(vendorMask);
#endif

#ifdef BCM_OAM_BUILD
    //tek callback registration
    VendorTekInit(vendorMask);
    VendorSampleInit(vendorMask);
#endif

    HoldoverInit ();
    OamOnuInit ();
    
#if defined(DPOE_OAM_BUILD)  || defined(BCM_OAM_BUILD)
    EncryptInit();
#endif

#if defined(BCM_OAM_BUILD) && defined(CLOCK_TRANSPORT)
    ClockTransInit();
#endif

#if RSTP
    RstpInit ();
#endif
#if INCLUDE_PPPOE
    OnuPPPoeInit();
#endif
    PowerSaveInit();
} // main

// end Main.c
