/**********************************************************************************
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
***********************************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <bcm_local_kernel_include/linux/sockios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bcm_local_kernel_include/linux/if_arp.h>

#include <bcmtypes.h>
#include <bcmnet.h>

#include "adslMibHelper.h"
#include <AdslMibDef.h>
#include "ifMib.h"

static int adslMibCurrAdminStatus=IF_ADMIN_STATUS_UP;

int
getAdslIfIndex(void)
{
  int ifIndex=0;

    /*
     * This should be the index for the ifType[index]=adsl in the ifTable 
     */
#if 0
  ifIndex = Interface_Index_By_Name(ADSL_MIB_IFNAME,strlen(ADSL_MIB_IFNAME));
#endif
  if (ifIndex == 0)
    return (1);
  else
    return (ifIndex);
}

/* code copied from adslctl.c */
unsigned long
getAdslIfSpeed(void)
{
    adslMibInfo data;
    long size = sizeof(adslMibInfo);
    unsigned long speed = 0;
    int xK, rK;

    BcmAdsl_GetObjectValue((char*)NULL, 0,(char*)&data, &size);
  
    if (kAdslTrainingConnected == data.adslTrainingState) {
      if (data.adslConnection.chType) {
	xK = data.adslConnection.xmtInfo.K;
	if (xK != 0)
	  speed = (unsigned long)((xK-1)*8*4*1000);
      }
      else {
	rK = data.adslConnection.rcvInfo.K;
	if (rK != 0)
	  speed = (unsigned long)((rK-1)*8*4*1000);
      } 
    }
    return speed;
}

int
getAdslOperStatus(void)
{
    adslMibInfo data;
    long size = sizeof(adslMibInfo);

    BcmAdsl_GetObjectValue((char*)NULL, 0,(char*)&data, &size);
    
    if (data.adslPhys.adslCurrStatus == ADSL_MIB_CURR_STATUS_NO_DEFECT)
      return (IF_OPER_STATUS_UP);
    else
      return (IF_OPER_STATUS_DOWN);
}

int
getAdslAdminStatus(void)
{
  return (adslMibCurrAdminStatus);
}

int
setAdslAdminStatus(int status)
{
  int ret=0;

  if (status != adslMibCurrAdminStatus) {
    if (status == IF_ADMIN_STATUS_UP) {
      if (BcmAdsl_ConnectionStart() == 0)
	adslMibCurrAdminStatus = IF_ADMIN_STATUS_UP;
      else
	ret = -1;
    }
    else if (status == IF_ADMIN_STATUS_DOWN) {
      if (BcmAdsl_ConnectionStop() == 0)
	adslMibCurrAdminStatus = IF_ADMIN_STATUS_DOWN;
      else
	ret = -1;
    }
    else
      ret =  -1; /* not supporting anything else */
  }
  return (ret);
}

#ifdef CPE_DSL_MIB
int getAdslLineTrellisMode(void)
{
    adslMibInfo data;
    long size = sizeof(adslMibInfo);

    BcmAdsl_GetObjectValue((char*)NULL, 0,(char*)&data, &size);
    return (data.adslConnection.trellisCoding);
}
#endif /* CPE_DSL_MIB */

#ifdef BUILD_SNMP_ADSL_MIB
int
adslMibHelper_getIfCount()
{
    return (1);
}

/** free context of adslAturPhysTable table */
#ifdef DO_ADSLATURPHYSTABLE
/*
 * Fill out the ADSL adslAturPhysTable 
 */
void
adslMibHelper_filladslAturPhysTable(pADSL_MIB_ADSLATURPHYSTABLE pTable)
{
    adslVersionInfo adslVer;
    adslMibInfo data;
    long size = sizeof(adslMibInfo);

    pADSL_MIB_ADSLATURPHYSTABLE p = pTable;

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

    /* need to get modulation type, LineCoding type */
    BcmAdsl_GetObjectValue((char*)NULL, 0,(char*)&data, &size);


    /* serial number */
    cmsUtl_strncpy(p->adslAturInvSerialNumber, ADSL_MIB_INV_SERIAL_NUMBER, 
	    strlen(ADSL_MIB_INV_SERIAL_NUMBER));
    
    
    switch (data.adslConnection.modType)
      {
      case kAdslModGdmt:
	cmsUtl_strncpy(p->adslAturInvVendorID, ADSL_MIB_VENDOR_ID_G_DMT,
		sizeof(p->adslAturInvVendorID));
	break;
      case kAdslModT1413:
	cmsUtl_strncpy(p->adslAturInvVendorID, ADSL_MIB_VENDOR_ID_T1_413,
		sizeof(p->adslAturInvVendorID));	
	break;
      case kAdslModGlite: 
      default:            
	cmsUtl_strncpy(p->adslAturInvVendorID,"",sizeof(p->adslAturInvVendorID));
      }
    
    /* get phy version number first */
    BcmAdsl_GetVersion(&adslVer);
    cmsUtl_strncpy(p->adslAturInvVersionNumber,adslVer.phyVerStr,sizeof(p->adslAturInvVersionNumber));
    p->adslAturCurrSnrMgn = (int)data.adslPhys.adslCurrSnrMgn;
    p->adslAturCurrAtn = (unsigned long)data.adslPhys.adslCurrAtn;
    p->adslAturCurrStatus = (int)data.adslPhys.adslCurrStatus;
    p->adslAturCurrOutputPwr = (unsigned char)data.adslPhys.adslCurrOutputPwr;
    p->adslAturCurrAttainableRate = (unsigned long)data.adslPhys.adslCurrAttainableRate;

    p->next = NULL;
}
#endif                          /* DO_ADSLATURPHYSTABLE */
/** free context of adslLineConfProfileTable table */
#ifdef DO_ADSLLINECONFPROFILETABLE
/*
 * Fill out the ADSL adslLineConfProfileTable 
 */
void
adslMibHelper_filladslLineConfProfileTable
    (pADSL_MIB_ADSLLINECONFPROFILETABLE pTable)
{
    char            adslEntryOid[4];
    adslLineEntry   data;
    long            size = sizeof(adslLineEntry);

    pADSL_MIB_ADSLLINECONFPROFILETABLE p = pTable;

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

   /***************************************/
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLLINECONFPROFILETABLE */
/** free context of adslAturChanTable table */
#ifdef DO_ADSLATURCHANTABLE
/*
 * Fill out the ADSL adslAturChanTable 
 */
void
adslMibHelper_filladslAturChanTable(pADSL_MIB_ADSLATURCHANTABLE pTable)
{
    adslMibInfo data;
    long size = sizeof(adslMibInfo);
    adslChanEntry   *pChanData;
    pADSL_MIB_ADSLATURCHANTABLE p = pTable;

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

   /***************************************/
    /* need to get modulation type, LineCoding type */
    BcmAdsl_GetObjectValue((char*)NULL, 0,(char*)&data, &size);

    if (data.adslConnection.chType == kAdslIntlChannel) 
      pChanData = &data.adslChanIntl;
    else 
      pChanData = &data.adslChanFast;

    p->adslAturChanInterleaveDelay = (unsigned long)pChanData->adslChanIntlDelay;
    p->adslAturChanCurrTxRate = (unsigned long) pChanData->adslChanCurrTxRate;
    p->adslAturChanPrevTxRate = (unsigned long) pChanData->adslChanPrevTxRate;
    p->adslAturChanCrcBlockLength= (unsigned long) pChanData->adslChanCrcBlockLength;
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLATURCHANTABLE */
/** free context of adslAtucPerfDataTable table */
#ifdef DO_ADSLATUCPERFDATATABLE
/*
 * Fill out the ADSL adslAtucPerfDataTable 
 */
void
adslMibHelper_filladslAtucPerfDataTable(pADSL_MIB_ADSLATUCPERFDATATABLE
                                        pTable)
{
    char            adslEntryOid[4];
    adslPerfDataEntry    data;
    long            size = sizeof(adslPerfDataEntry);

    pADSL_MIB_ADSLATUCPERFDATATABLE p = pTable;

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

   /***************************************/
    /* this is also needs support from driver.  The following are ATUR data */
    adslEntryOid[0] = kOidAdsl;
    adslEntryOid[1] = kOidAdslLine;
    adslEntryOid[2] = kOidAdslMibObjects;
    adslEntryOid[3] = kOidAdslPerfDataEntry;
    BcmAdsl_GetObjectValue((char*)&adslEntryOid, (int)sizeof(adslEntryOid),
                           (char*)&data, &size);

    p->adslAtucPerfLofs = (unsigned long) data.perfTotal.adslLofs;
    p->adslAtucPerfLoss = (unsigned long) data.perfTotal.adslLoss;
    p->adslAtucPerfLprs = (unsigned long) data.perfTotal.adslLprs;
    p->adslAtucPerfESs = (unsigned long) data.perfTotal.adslESs;
    p->adslAtucPerfValidIntervals = (int) data.adslPerfValidIntervals
    p->adslAtucPerfInvalidIntervals = (int) data.adslPerfInvalidIntervals;
    p->adslAtucPerfCurr15MinTimeElapsed = (unsigned long) data.adslPerfCurr15MinTimeElapsed;
    p->adslAtucPerfCurr15MinLofs = (unsigned long) data.perfCurr15Min.adslLofs;
    p->adslAtucPerfCurr15MinLoss = (unsigned long) data.perfCurr15Min.adslLoss;
    p->adslAtucPerfCurr15MinLprs = (unsigned long) data.perfCurr15Min.adslLprs;
    p->adslAtucPerfCurr15MinESs = (unsigned long) data.perfCurr15Min.adslESs;
    p->adslAtucPerfCurr1DayTimeElapsed = (unsigned long) data.adslPerfCurr1DayTimeElapsed;
    p->adslAtucPerfCurr1DayLofs = (unsigned long) data.perfCurr1Day.adslLofs;
    p->adslAtucPerfCurr1DayLoss = (unsigned long) data.perfCurr1Day.adslLoss;
    p->adslAtucPerfCurr1DayLprs = (unsigned long) data.perfCurr1Day.adslLprs;
    p->adslAtucPerfCurr1DayESs = (unsigned long) data.perfCurr1Day.adslESs;
    p->adslAtucPerfPrev1DayMoniSecs = (int) data.adslAtucPerfPrev1DayMoniSecs;
    p->adslAtucPerfPrev1DayLofs = (unsigned long) data.perfPrev1Day.adslLofs;
    p->adslAtucPerfPrev1DayLoss = (unsigned long) data.perfPrev1Day.adslLoss;
    p->adslAtucPerfPrev1DayLprs = (unsigned long) data.perfPrev1Day.adslLprs;
    p->adslAtucPerfPrev1DayESs = (unsigned long) data.perfPrev1Day.adslESs;
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLATUCPERFDATATABLE */
/** free context of adslAtucIntervalTable table */
#ifdef DO_ADSLATUCINTERVALTABLE
/*
 * Fill out the ADSL adslAtucIntervalTable 
 */
void
adslMibHelper_filladslAtucIntervalTable(pADSL_MIB_ADSLATUCINTERVALTABLE
                                        pTable)
{
    char            adslEntryOid[4];
    adslPerfCounters data[kAdslMibPerfIntervals];
    long            size=sizeof(data);

    pADSL_MIB_ADSLATUCINTERVALTABLE p = pTable;

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

   /***************************************/
    /* this table needs support from ADSL driver; using aturIntervalTable's data now */
    adslEntryOid[0] = kOidAdsl;
    adslEntryOid[1] = kOidAdslLine;
    adslEntryOid[2] = kOidAdslMibObjects;
    adslEntryOid[3] = kOidAdslAturPerfIntervalTable;
    BcmAdsl_GetObjectValue((char*)&adslEntryOid, (int)sizeof(adslEntryOid),
                           (char*)&data, &size);

    numOfInterval = size/sizeof(adslPerfCounters);

    for (i=0; i<numOfInterval; i++) {
      p->pTableBeginning = pTable;
      p->ifIndex = index;
      p->adslAturIntervalNumber = i+1;
      memcpy(&p->adslAturIntervalLofs,&data[i],(sizeof(adslPerfCounters)));
      p->adslAturIntervalValidData = 1;
      if (i == (numOfInterval-1))
	p->next=NULL;
      else {
	p->next = (p+1);
	p++;
      }
    }
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLATUCINTERVALTABLE */
/** free context of adslAturPerfDataTable table */
#ifdef DO_ADSLATURPERFDATATABLE
/*
 * Fill out the ADSL adslAturPerfDataTable 
 */
void
adslMibHelper_filladslAturPerfDataTable(pADSL_MIB_ADSLATURPERFDATATABLE
                                        pTable)
{
    char            adslEntryOid[4];
    adslPerfDataEntry    data;
    long            size = sizeof(adslPerfDataEntry);

    pADSL_MIB_ADSLATURPERFDATATABLE p = pTable;

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

   /***************************************/
    adslEntryOid[0] = kOidAdsl;
    adslEntryOid[1] = kOidAdslLine;
    adslEntryOid[2] = kOidAdslMibObjects;
    adslEntryOid[3] = kOidAdslAturPerfDataTable;

    BcmAdsl_GetObjectValue((char*)&adslEntryOid, (int)sizeof(adslEntryOid),
                           (char*)&data, &size);

    p->adslAturPerfLofs = (unsigned long) data.perfTotal.adslLofs;
    p->adslAturPerfLoss = (unsigned long) data.perfTotal.adslLoss;
    p->adslAturPerfLprs = (unsigned long) data.perfTotal.adslLprs;
    p->adslAturPerfESs = (unsigned long) data.perfTotal.adslESs;
    p->adslAturPerfValidIntervals = (int) data.adslPerfValidIntervals;
    p->adslAturPerfInvalidIntervals = (int) data.adslPerfInvalidIntervals;
    p->adslAturPerfCurr15MinTimeElapsed = (unsigned long) data.adslPerfCurr15MinTimeElapsed;
    p->adslAturPerfCurr15MinLofs = (unsigned long) data.perfCurr15Min.adslLofs;
    p->adslAturPerfCurr15MinLoss = (unsigned long) data.perfCurr15Min.adslLoss;
    p->adslAturPerfCurr15MinLprs = (unsigned long) data.perfCurr15Min.adslLprs;
    p->adslAturPerfCurr15MinESs = (unsigned long) data.perfCurr15Min.adslESs;
    p->adslAturPerfCurr1DayTimeElapsed = (unsigned long) data.adslPerfCurr1DayTimeElapsed;
    p->adslAturPerfCurr1DayLofs = (unsigned long) data.perfCurr1Day.adslLofs;
    p->adslAturPerfCurr1DayLoss = (unsigned long) data.perfCurr1Day.adslLoss;
    p->adslAturPerfCurr1DayLprs = (unsigned long) data.perfCurr1Day.adslLprs;
    p->adslAturPerfCurr1DayESs = (unsigned long) data.perfCurr1Day.adslESs;
    p->adslAturPerfPrev1DayMoniSecs = (int) data.adslAturPerfPrev1DayMoniSecs;
    p->adslAturPerfPrev1DayLofs = (unsigned long) data.perfPrev1Day.adslLofs;
    p->adslAturPerfPrev1DayLoss = (unsigned long) data.perfPrev1Day.adslLoss;
    p->adslAturPerfPrev1DayLprs = (unsigned long) data.perfPrev1Day.adslLprs;
    p->adslAturPerfPrev1DayESs = (unsigned long) data.perfPrev1Day.adslESs;
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLATURPERFDATATABLE */
/** free context of adslLineAlarmConfProfileTable table */
#ifdef DO_ADSLLINEALARMCONFPROFILETABLE
/*
 * Fill out the ADSL adslLineAlarmConfProfileTable 
 */
void
adslMibHelper_filladslLineAlarmConfProfileTable
    (pADSL_MIB_ADSLLINEALARMCONFPROFILETABLE pTable)
{
    char profileName[32];

    pADSL_MIB_ADSLLINEALARMCONFPROFILETABLE p = pTable;

    /* we are supposed to get these configurations from the adsl driver; howevever,
       it doesn't have this implemented yet.  We are just going to hardcode them
       to 0 which means disabled */
    memset(p,0,sizeof(p));

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

    /***************************************/
    sprintf(profileName,"%d",p->ifIndex);
    cmsUtl_strncpy(p->adslLineAlarmConfProfileName,profileName,sizeof(p->adslLineAlarmConfProfileName));
    p->adslAtucInitFailureTrapEnable = ADSL_MIB_ALARM_ATUC_INIT_FAIL_DISABLE;
    p->adslLineAlarmConfProfileRowStatus = ADSL_MIB_ALARM_ROW_ACTIVE;
    /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLLINEALARMCONFPROFILETABLE */
/** free context of adslAturChanPerfDataTable table */
#ifdef DO_ADSLATURCHANPERFDATATABLE
/*
 * Fill out the ADSL adslAturChanPerfDataTable 
 */
void
adslMibHelper_filladslAturChanPerfDataTable
    (pADSL_MIB_ADSLATURCHANPERFDATATABLE pTable)
{
    adslMibInfo data;
    long size = sizeof(adslMibInfo);
    adslChanPerfDataEntry  *pChanData;

    pADSL_MIB_ADSLATURCHANPERFDATATABLE p = pTable;

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

   /***************************************/
    BcmAdsl_GetObjectValue((char*)NULL,0,(char*)&data, &size);
    if (data.adslConnection.chType == kAdslIntlChannel) 
      pChanData = &data.adslChanIntlPerfData;
    else 
      pChanData = &data.adslChanFastPerfData;
    memcpy((void *)&p->adslAturChanReceivedBlks,(void *)&pChanData->perfTotal,sizeof(adslChanCounters));
    p->adslAturChanPerfValidIntervals = pChanData->adslChanPerfValidIntervals;
    p->adslAturChanPerfInvalidIntervals = pChanData->adslChanPerfInvalidIntervals;
    p->adslAturChanPerfCurr15MinTimeElapsed = pChanData->adslPerfCurr15MinTimeElapsed;
    memcpy((void *)&p->adslAturChanPerfCurr15MinReceivedBlks,(void *)&pChanData->perfCurr15Min,sizeof(adslChanCounters));
    p->adslAturChanPerfCurr1DayTimeElapsed = pChanData->adslPerfCurr1DayTimeElapsed;
    memcpy((void *)&p->adslAturChanPerfCurr1DayReceivedBlks,(void *)&pChanData->perfCurr1Day,sizeof(adslChanCounters));
    p->adslAturChanPerfPrev1DayMoniSecs = pChanData->adslAturPerfPrev1DayMoniSecs;
    memcpy((void *)&p->adslAturChanPerfPrev1DayReceivedBlks,(void *)&pChanData->perfPrev1Day,sizeof(adslChanCounters));
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLATURCHANPERFDATATABLE */
/** free context of adslAturIntervalTable table */
#ifdef DO_ADSLATURINTERVALTABLE
/*
 * Fill out the ADSL adslAturIntervalTable 
 */
void
adslMibHelper_filladslAturIntervalTable(pADSL_MIB_ADSLATURINTERVALTABLE
                                        pTable)
{
    adslMibInfo data;
    long size;
    adslPerfCounters *pChanData;
    int numOfInterval, i;
    int index;
    int validInterval;
    pADSL_MIB_ADSLATURINTERVALTABLE p = pTable;

    size = sizeof(adslMibInfo);
    index = getAdslIfIndex();

   /***************************************/
    BcmAdsl_GetObjectValue((char*)NULL,0,(char*)&data, &size);
    pChanData = data.adslPerfIntervals;
    validInterval = data.adslPerfData.adslPerfValidIntervals;

    /* the max interval we support */
    numOfInterval = kAdslMibPerfIntervals;
    for (i=0; i<numOfInterval; i++) {
      p->pTableBeginning = pTable;
      p->ifIndex = index;
      p->adslAturIntervalNumber = i+1;
      memcpy(&p->adslAturIntervalLofs,&pChanData[i],(sizeof(adslPerfCounters)));
      if (validInterval > i)
	p->adslAturIntervalValidData = 1;
      else 
	p->adslAturIntervalValidData = 0;
      if (i == (numOfInterval-1))
	p->next=NULL;
      else {
	p->next = (p+1);
	p++;
      }
    }
   /***************************************/
}
#endif                          /* DO_ADSLATURINTERVALTABLE */
/** free context of adslAtucPhysTable table */
#ifdef DO_ADSLATUCPHYSTABLE
/*
 * Fill out the ADSL adslAtucPhysTable 
 */
void
adslMibHelper_filladslAtucPhysTable(pADSL_MIB_ADSLATUCPHYSTABLE pTable)
{
    adslMibInfo data;
    long size = sizeof(adslMibInfo);
    adslFullPhysEntry *pPhysEntry;

    pADSL_MIB_ADSLATUCPHYSTABLE p = pTable;

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

   /***************************************/
    BcmAdsl_GetObjectValue((char*)NULL,0,(char*)&data, &size);

    pPhysEntry = &data.adslAtucPhys;
    cmsUtl_strncpy(p->adslAtucInvSerialNumber,pPhysEntry->adslSerialNumber,
	    sizeof(p->adslAtucInvSerialNumber));
    cmsUtl_strncpy(p->adslAtucInvVendorID,pPhysEntry->adslVendorID,sizeof(p->adslAtucInvVendorID));
    cmsUtl_strncpy(p->adslAtucInvVersionNumber,pPhysEntry->adslVersionNumber,
	    sizeof(p->adslAtucInvVersionNumber));
    p->adslAtucCurrSnrMgn = (int)pPhysEntry->adslCurrSnrMgn;
    p->adslAtucCurrAtn = (unsigned long)pPhysEntry->adslCurrAtn;
    p->adslAtucCurrStatus = (int)pPhysEntry->adslCurrStatus;
    p->adslAtucCurrOutputPwr = (int)pPhysEntry->adslCurrOutputPwr;
    p->adslAtucCurrAttainableRate = (unsigned long)pPhysEntry->adslCurrAttainableRate;
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLATUCPHYSTABLE */
/** free context of adslAtucChanPerfDataTable table */
#ifdef DO_ADSLATUCCHANPERFDATATABLE
/*
 * Fill out the ADSL adslAtucChanPerfDataTable 
 */
void
adslMibHelper_filladslAtucChanPerfDataTable
    (pADSL_MIB_ADSLATUCCHANPERFDATATABLE pTable)
{
    adslMibInfo data;
    long size = sizeof(adslMibInfo);
    adslChanPerfDataEntry  *pChanData;

    pADSL_MIB_ADSLATUCCHANPERFDATATABLE p = pTable;

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

   /***************************************/
    BcmAdsl_GetObjectValue((char*)NULL,0,(char*)&data, &size);
    /* this table needs support from ADSL driver; using aturChanPerf's data now */
    if (data.adslConnection.chType == kAdslIntlChannel) 
      pChanData = &data.adslChanIntlPerfData;
    else 
      pChanData = &data.adslChanFastPerfData;
    memcpy((void *)&p->adslAtucChanReceivedBlks,(void *)&pChanData->perfTotal,sizeof(adslChanCounters));
    p->adslAtucChanPerfValidIntervals = pChanData->adslChanPerfValidIntervals;
    p->adslAtucChanPerfInvalidIntervals = pChanData->adslChanPerfInvalidIntervals;
    p->adslAtucChanPerfCurr15MinTimeElapsed = pChanData->adslPerfCurr15MinTimeElapsed;
    memcpy((void *)&p->adslAtucChanPerfCurr15MinReceivedBlks,(void *)&pChanData->perfCurr15Min,sizeof(adslChanCounters));
    p->adslAtucChanPerfCurr1DayTimeElapsed = pChanData->adslPerfCurr1DayTimeElapsed;
    memcpy((void *)&p->adslAtucChanPerfCurr1DayReceivedBlks,(void *)&pChanData->perfCurr1Day,sizeof(adslChanCounters));
    p->adslAtucChanPerfPrev1DayMoniSecs = pChanData->adslAtucPerfPrev1DayMoniSecs;
    memcpy((void *)&p->adslAtucChanPerfPrev1DayReceivedBlks,(void *)&pChanData->perfPrev1Day,sizeof(adslChanCounters));
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLATUCCHANPERFDATATABLE */
/** free context of adslAturChanIntervalTable table */
#ifdef DO_ADSLATURCHANINTERVALTABLE
/*
 * Fill out the ADSL adslAturChanIntervalTable 
 */
void
adslMibHelper_filladslAturChanIntervalTable
    (pADSL_MIB_ADSLATURCHANINTERVALTABLE pTable)
{
    adslMibInfo data;
    long size = sizeof(adslMibInfo);
    adslChanCounters *pChanData;
    int numOfInterval, i;
    int index;
    int validInterval;

    pADSL_MIB_ADSLATURCHANINTERVALTABLE p = pTable;

    index = getAdslIfIndex();

   /***************************************/
    BcmAdsl_GetObjectValue((char*)NULL,0,(char*)&data, &size);
    if (data.adslConnection.chType == kAdslIntlChannel) {
      pChanData = data.adslChanIntlPerfIntervals;
      validInterval = data.adslChanIntlPerfData.adslChanPerfValidIntervals;
    }
    else {
      pChanData = data.adslChanFastPerfIntervals;
      validInterval = data.adslChanFastPerfData.adslChanPerfValidIntervals;
    }

    /* the max interval we support */
    numOfInterval = kAdslMibChanPerfIntervals;
    for (i=0; i<numOfInterval; i++) {
      p->pTableBeginning = pTable;
      p->ifIndex = index;
      p->adslAturChanIntervalNumber = i+1;
      memcpy(&p->adslAturChanIntervalReceivedBlks,&pChanData[i],sizeof(adslChanCounters));
      if (validInterval > i)
	p->adslAturChanIntervalValidData = 1;
      else
	p->adslAturChanIntervalValidData = 0;
      if (i == (numOfInterval-1))
	p->next=NULL;
      else {
	p->next = (p+1);
	p++;
      }
    }
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLATURCHANINTERVALTABLE */
/** free context of adslAtucChanIntervalTable table */
#ifdef DO_ADSLATUCCHANINTERVALTABLE
/*
 * Fill out the ADSL adslAtucChanIntervalTable 
 */
void
adslMibHelper_filladslAtucChanIntervalTable
    (pADSL_MIB_ADSLATUCCHANINTERVALTABLE pTable)
{
    adslMibInfo data;
    long size = sizeof(adslMibInfo);
    adslChanCounters *pChanData;
    int numOfInterval, i;
    int index;
    int validInterval;

    pADSL_MIB_ADSLATUCCHANINTERVALTABLE p = pTable;

    p->pTableBeginning = pTable;
    index = getAdslIfIndex();

   /***************************************/
    BcmAdsl_GetObjectValue((char*)NULL,0,(char*)&data, &size);
    if (data.adslConnection.chType == kAdslIntlChannel) {
      pChanData = data.adslChanIntlPerfIntervals;
      validInterval = data.adslChanIntlPerfData.adslChanPerfValidIntervals;
    }
    else {
      pChanData = data.adslChanFastPerfIntervals;
      validInterval = data.adslChanFastPerfData.adslChanPerfValidIntervals;
    }

    /* the max interval we support */
    numOfInterval = kAdslMibChanPerfIntervals;
    for (i=0; i<numOfInterval; i++) {
      p->pTableBeginning = pTable;
      p->ifIndex = index;
      p->adslAtucChanIntervalNumber = i+1;
      memcpy(&p->adslAtucChanIntervalReceivedBlks,&pChanData[i],sizeof(adslChanCounters));
      if (validInterval > i)
	p->adslAtucChanIntervalValidData = 1;
      else
	p->adslAtucChanIntervalValidData = 0;
      if (i == (numOfInterval-1))
	p->next=NULL;
      else {
	p->next = (p+1);
	p++;
      }
    }
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLATUCCHANINTERVALTABLE */
/** free context of adslLineTable table */
#ifdef DO_ADSLLINETABLE
/*
 * Fill out the ADSL adslLineTable 
 */
void
adslMibHelper_filladslLineTable(pADSL_MIB_ADSLLINETABLE pTable)
{
    char profileName[32];
    char            adslEntryOid[4];
    adslLineEntry   data;
    long            size = sizeof(adslLineEntry);

    pADSL_MIB_ADSLLINETABLE p = pTable;

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

   /***************************************/
    adslEntryOid[0] = kOidAdsl;
    adslEntryOid[1] = kOidAdslLine;
    adslEntryOid[2] = kOidAdslMibObjects;
    adslEntryOid[3] = kOidAdslLineTable;
    BcmAdsl_GetObjectValue((char*)&adslEntryOid, (int)sizeof(adslEntryOid),
                           (char*)&data, &size);

    p->adslLineCoding = (int) data.adslLineCoding;
    p->adslLineType = (int) data.adslLineType;
    cmsUtl_strncpy((char*)p->adslLineSpecific, "", strlen(""));
    cmsUtl_strncpy((char*)p->adslLineConfProfile, ADSL_MIB_CONFIG_PROFILE_NAME,
	    sizeof(p->adslLineConfProfile));
    sprintf(profileName,"%d",p->ifIndex);
    cmsUtl_strncpy(p->adslLineAlarmConfProfile, profileName,
	    sizeof(p->adslLineAlarmConfProfile));
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLLINETABLE */
/** free context of adslAtucChanTable table */
#ifdef DO_ADSLATUCCHANTABLE
/*
 * Fill out the ADSL adslAtucChanTable 
 */
void
adslMibHelper_filladslAtucChanTable(pADSL_MIB_ADSLATUCCHANTABLE pTable)
{
    adslMibInfo data;
    long size = sizeof(adslMibInfo);
    adslChanEntry   *pChanData;
    pADSL_MIB_ADSLATUCCHANTABLE p = pTable;

    p->pTableBeginning = pTable;
    p->ifIndex = getAdslIfIndex();

   /***************************************/
    BcmAdsl_GetObjectValue((char*)NULL, 0,(char*)&data, &size);
	
    /* driver support needed, atuc chan table; this one is atur table */
    if (data.adslConnection.chType == kAdslIntlChannel) 
      pChanData = &data.adslChanIntl;
    else 
      pChanData = &data.adslChanFast;

    p->adslAturChanInterleaveDelay = (unsigned long)pChanData->adslChanIntlDelay;
    p->adslAturChanCurrTxRate = (unsigned long) pChanData->adslChanCurrTxRate;
    p->adslAturChanPrevTxRate = (unsigned long) pChanData->adslChanPrevTxRate;
    p->adslAturChanCrcBlockLength= (unsigned long) pChanData->adslChanCrcBlockLength;
   /***************************************/

    p->next = NULL;
}
#endif                          /* DO_ADSLATUCCHANTABLE */

#endif /* BUILD_SNMP_ADSL_MIB */
