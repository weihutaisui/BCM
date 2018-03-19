/*****************************************************************************
 *
 *  Copyright (c) 2005-2008  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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

/* General includes */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

/* SNMP includes */
#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"
#include "AdslMibDef.h"
#include "vdslMibHelper.h"
#include "vdslMib.h"
#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
extern SNMP_DEBUG_FLAG snmpDebug;
#endif

extern int write_ifEntry(int action,unsigned char *var_val, unsigned char varval_type, int var_val_len,
                  unsigned char *statP, Oid *name, int name_len);

extern WanDslIntfCfgObject           g_dslIntfObj;
extern WanDslIntfStatsTotalObject  g_dslStatsObj;



VDSL_PHYS_LIST           vdslPhysList;
VDSL_PERFDATA_LIST    vdslPerfDataList;
VDSL_CONF_LIST           vdslConfList;


void vdslAddPhysToList(pVDSL_PHYS pPhysNew) 
{
  if (vdslPhysList.pIf == NULL) 
  {
     vdslPhysList.pIf = pPhysNew;
     vdslPhysList.pIfTail = pPhysNew;
  }
  else 
  {
     vdslPhysList.pIfTail->next = pPhysNew;
     vdslPhysList.pIfTail = pPhysNew;
  }

}

void vdslPhysMibFree(void)
{
   pVDSL_PHYS pPhys;

   while (vdslPhysList.pIf) 
   {
      pPhys = vdslPhysList.pIf;
      vdslPhysList.pIf = vdslPhysList.pIf->next;
      free(pPhys);
   }
}

void vdslAddPerfDataToList(pVDSL_PERFDATA pPerfDataNew) 
{
  if (vdslPerfDataList.pIf == NULL) 
  {
     vdslPerfDataList.pIf = pPerfDataNew;
     vdslPerfDataList.pIfTail = pPerfDataNew;
  }
  else 
  {
     vdslPerfDataList.pIfTail->next = pPerfDataNew;
     vdslPerfDataList.pIfTail = pPerfDataNew;
  }

}

void vdslPerfDataMibFree(void)
{
   pVDSL_PERFDATA pPerfData;

   while (vdslPerfDataList.pIf) 
   {
      pPerfData = vdslPerfDataList.pIf;
      vdslPerfDataList.pIf = vdslPerfDataList.pIf->next;
      free(pPerfData);
   }
}


void vdslAddConfToList(pVDSL_CONF pConfNew) 
{
  if (vdslConfList.pIf == NULL) 
  {
     vdslConfList.pIf = pConfNew;
     vdslConfList.pIfTail = pConfNew;
  }
  else 
  {
     vdslConfList.pIfTail->next = pConfNew;
     vdslConfList.pIfTail = pConfNew;
  }

}

void vdslConfMibFree(void)
{
    pVDSL_CONF pConf;

   while (vdslConfList.pIf) 
   {
      pConf = vdslConfList.pIf;
      vdslConfList.pIf = vdslConfList.pIf->next;
      free(pConf);
   }
}

void  init_vdslPhysList(void)
{
    int i;
    pVDSL_PHYS pPhysNew;
	
    cpeGetXdslSts();

    vdslPhysMibFree();
	
    for(i= 0; i <2 ;i++)
    {
          pPhysNew = (pVDSL_PHYS)calloc(1, sizeof(VDSL_PHYS));
          if (pPhysNew == NULL) 
	   {
               cmsLog_error("Fail to allocate VDSL_PHYS in init_vdslPhysList()\n");
               return ;
           }
           memset(pPhysNew,0,sizeof(VDSL_PHYS));

	   pPhysNew->side = i+1;

	   if (cmsUtl_strcmp(g_dslIntfObj.status,MDMVS_UP) == 0)
	   {
	       pPhysNew->currSnrMgn =   (i==0)   ?   g_dslIntfObj.downstreamNoiseMargin     :   g_dslIntfObj.upstreamNoiseMargin;
               // [JIRA SWBCACPE-10306]: TR-098 requires
               // upstreamCurrRate, downstreamCurrRate, upstreamMaxRate, downstreamMaxRate
               // in Kbps instead of Bps
               pPhysNew->attRate    =   (i==0)   ?   g_dslIntfObj.downstreamMaxRate :    g_dslIntfObj.upstreamMaxRate;
               pPhysNew->currRate   =   (i==0)   ?   g_dslIntfObj.downstreamCurrRate :   g_dslIntfObj.upstreamCurrRate;
               pPhysNew->currAtn    =   (i==0)   ?   g_dslIntfObj.downstreamAttenuation :   g_dslIntfObj.upstreamAttenuation;
               pPhysNew->outPower   =   (i==0)   ?   g_dslIntfObj.downstreamPower :   g_dslIntfObj.upstreamPower;
	   }
	   vdslAddPhysToList(pPhysNew);
	   
    }
 
}

void  init_vdslPerfDataList(void)
{
    int i;
    pVDSL_PERFDATA pPerfData;
	
    cpeGetXdslSts();

    vdslPerfDataMibFree();
	
    for(i= 0; i <2 ;i++)
    {
          pPerfData = (pVDSL_PERFDATA)calloc(1, sizeof(VDSL_PERFDATA));
          if (pPerfData == NULL) 
	   {
               cmsLog_error("Fail to allocate VDSL_PERFDATA in init_vdslPerfDataList()\n");
               return ;
           }
           memset(pPerfData,0,sizeof(VDSL_PERFDATA));

	   pPerfData->index = i+1;

	   if (cmsUtl_strcmp(g_dslIntfObj.status,MDMVS_UP) == 0)
	   {
		pPerfData->dataInits =   0; // should come from g_dslStatsObj.initTimeouts, but this param is not implemented in data model
		pPerfData->dataLofs  =   0; // should come from g_dslStatsObj.lossOfFraming, but this param is not implemented in data model
		pPerfData->dataLols  =    0; //?
		pPerfData->dataLoss  =   0;  //?
		pPerfData->dataLprs  =   0;
#if 0
      /* Comment out for compilation, Yongchao to fix */
		pPerfData->dataESs  =    (i==0)   ?    g_dslStatsObj.erroredSecs :     g_dslStatsObj.X_BROADCOM_COM_UpstreamEs;
		pPerfData->dataSESs =   (i==0)   ?   g_dslStatsObj.severelyErroredSecs : g_dslStatsObj.X_BROADCOM_COM_UpstreamSes;
		pPerfData->dataUASs =   (i==0)   ?   g_dslStatsObj.X_BROADCOM_COM_DownstreamUas  : g_dslStatsObj.X_BROADCOM_COM_UpstreamUas;
#endif
	   }
	   vdslAddPerfDataToList(pPerfData);  
    }
 
}


void  init_vdslConfList(void)
{
    int i;
    pVDSL_CONF pConf;
	
    cpeGetXdslSts();

    vdslConfMibFree();
	
    for(i= 0; i <1 ;i++)
    {
          pConf = (pVDSL_CONF)calloc(1, sizeof(VDSL_CONF));
          if (pConf == NULL) 
	   {
               cmsLog_error("Fail to allocate VDSL_CONF in init_vdslConfList()\n");
               return ;
           }
           memset(pConf,0,sizeof(VDSL_CONF));

	   pConf->index = i+1;

	   if (cmsUtl_strcmp(g_dslIntfObj.status,MDMVS_UP) == 0)
	   {
	     #if 0
		pConf->maxDownSnr =0 ;
		pConf->minDownSnr= 0  ;
		pConf->targetDownSnr=    0; //?
		pConf->maxUpSnr=   0;  //?
		pConf->minUpSnr=   0;
		pConf->targetUpSnr  =     0;
	     #endif

#if 0
      /* Comment out for compilation, Yongchao to fix */
		pConf->maxDownInv =   g_dslIntfObj.X_BROADCOM_COM_DownstreamDelay;
		pConf->maxUpInv     =    g_dslIntfObj.X_BROADCOM_COM_UpstreamDelay;
#endif
	   }
	   vdslAddConfToList(pConf);  
    }
 
}


/* CPE_DSL_MIB initialisation (must also register the MIB module tree) */
void  init_vdsl_mib()
{
      register_subtrees_of_vdsl_mib();
}

unsigned char *
var_vdslPhysEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType,
            snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  /* Last subOID of COLUMNAR OID is column */
  int	column = newoid->name[(newoid->namelen - 1)];
  int	result;
  pVDSL_PHYS pPhysNew;
  
  int index;

  init_vdslPhysList();

    /* Add indexes for the entry to OID newname */
  /*     ifIndex =  derived from ifInfoList somehow */
  pPhysNew = vdslPhysList.pIf;
  /* newoid: column (namelen-1), ifIndex (name_len) , value */
  index = newoid->namelen;
  newoid->namelen++;
  while (pPhysNew) 
  {
    newoid->name[index] = pPhysNew->side;
    /* Determine whether it is the requested OID  */

    result = compare(reqoid, newoid);

    if (((searchType == EXACT) && (result == 0)) ||
        ((searchType == NEXT) && (result < 0)))
      break; /* found */
    pPhysNew= pPhysNew->next;
  } /* for all interfaces */

  /* should return end-mibinstance, so next can return the next index of the next table; shouldn't be nomibinstance */
  if (pPhysNew == NULL)
    return NO_MIBINSTANCE;
 
    
  *var_len = sizeof(long);    

#ifdef BUILD_SNMP_SET
  /* Set write-function */
  *write_method = (WRITE_METHOD)write_ifEntry;
#else
  *write_method = 0;
#endif

    /* Set size (in bytes) and return address of the variable */
    switch (column)
    {
      case I_vdslPhysSide:
         long_return =  pPhysNew->side;
         return (unsigned char *)&long_return;
		
      case I_vdslPhysInvSerialNumber:
         return NO_MIBINSTANCE;
		
      case I_vdslPhysInvVendorID:
         return NO_MIBINSTANCE;

      case I_vdslPhysInvVersionNumbe:
          return NO_MIBINSTANCE;
		  
      case I_vdslPhysCurrSnrMgn:
          long_return =  pPhysNew->currSnrMgn;
          return (unsigned char *)&long_return;
		
      case I_vdslPhysCurrAtn:
         long_return =  pPhysNew->currAtn;
          return (unsigned char *)&long_return;
		
      case I_vdslPhysCurrStatus:
        return NO_MIBINSTANCE;
		
      case I_vdslPhysCurrOutputPwr:
          long_return =  pPhysNew->outPower;
          return (unsigned char *)&long_return;
		
      case I_vdslPhysCurrAttainableRate:
          long_return =  pPhysNew->attRate;
          return (unsigned char *)&long_return;;
		
      case I_vdslPhysCurrLineRate:
          long_return =  pPhysNew->currRate;
          return (unsigned char *)&long_return;
		
      default:
        return NO_MIBINSTANCE;
      }
} 


unsigned char *
var_vdslPerfDataEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType,
            snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  /* Last subOID of COLUMNAR OID is column */
  int	column = newoid->name[(newoid->namelen - 1)];
  int	result;
  pVDSL_PERFDATA pPerfData;
  
  int index;

  init_vdslPerfDataList();

    /* Add indexes for the entry to OID newname */
  /*     ifIndex =  derived from ifInfoList somehow */
  pPerfData = vdslPerfDataList.pIf;
  /* newoid: column (namelen-1), ifIndex (name_len) , value */
  index = newoid->namelen;
  newoid->namelen++;
  while (pPerfData) 
  {
    newoid->name[index] = pPerfData->index;
    /* Determine whether it is the requested OID  */

    result = compare(reqoid, newoid);

    if (((searchType == EXACT) && (result == 0)) ||
        ((searchType == NEXT) && (result < 0)))
      break; /* found */
    pPerfData= pPerfData->next;
  } /* for all interfaces */

  /* should return end-mibinstance, so next can return the next index of the next table; shouldn't be nomibinstance */
  if (pPerfData == NULL)
    return NO_MIBINSTANCE;
 
    
  *var_len = sizeof(long);    

#ifdef BUILD_SNMP_SET
  /* Set write-function */
  *write_method = (WRITE_METHOD)write_ifEntry;
#else
  *write_method = 0;
#endif


    /* Set size (in bytes) and return address of the variable */
    switch (column)
    {
      case I_vdslPerfDataValidIntervals:
          return NO_MIBINSTANCE;;
		
      case I_vdslPerfDataInvalidIntervals:
         return NO_MIBINSTANCE;
		
      case I_vdslPerfDataLofs:
           long_return =  pPerfData->dataLofs;
           return (unsigned char *)&long_return;;

      case I_vdslPerfDataLoss:
          long_return =  pPerfData->dataLoss;
          return (unsigned char *)&long_return;;
		  
      case I_vdslPerfDataLprs:
          long_return =  pPerfData->dataLprs;
          return (unsigned char *)&long_return;
		
      case I_vdslPerfDataLols:
          long_return =  pPerfData->dataLols;
          return (unsigned char *)&long_return;
		
      case I_vdslPerfDataESs:
         long_return =  pPerfData->dataESs;
          return (unsigned char *)&long_return;
		
      case I_vdslPerfDataSESs:
            long_return =  pPerfData->dataSESs;
          return (unsigned char *)&long_return;
		
      case I_vdslPerfDataUASs:
           long_return =  pPerfData->dataUASs;
          return (unsigned char *)&long_return;
		
      case I_vdslPerfDataInits:
            long_return =  pPerfData->dataInits;
          return (unsigned char *)&long_return;;
		
      default:
        return NO_MIBINSTANCE;
      }
} 


unsigned char *
var_vdslConfEntry(int *var_len, Oid *newoid, Oid *reqoid, int searchType,
            snmp_info_t *mesg, WRITE_METHOD *write_method)
{
  /* Last subOID of COLUMNAR OID is column */
  int	column = newoid->name[(newoid->namelen - 1)];
  int	result;
  pVDSL_CONF pConf;
  
  int index;

  init_vdslConfList();

    /* Add indexes for the entry to OID newname */
  /*     ifIndex =  derived from ifInfoList somehow */
  pConf = vdslConfList.pIf;
  /* newoid: column (namelen-1), ifIndex (name_len) , value */
  index = newoid->namelen;
  newoid->namelen++;
  while (pConf) 
  {
    newoid->name[index] = pConf->index;
    /* Determine whether it is the requested OID  */

    result = compare(reqoid, newoid);

    if (((searchType == EXACT) && (result == 0)) ||
        ((searchType == NEXT) && (result < 0)))
      break; /* found */
    pConf= pConf->next;
  } /* for all interfaces */

  /* should return end-mibinstance, so next can return the next index of the next table; shouldn't be nomibinstance */
  if (pConf == NULL)
    return NO_MIBINSTANCE;
 
    
  *var_len = sizeof(long);    

#ifdef BUILD_SNMP_SET
  /* Set write-function */
  *write_method = (WRITE_METHOD)write_ifEntry;
#else
  *write_method = 0;
#endif

    /* Set size (in bytes) and return address of the variable */
    switch (column)
    {
      case I_vdslLineConfDownMaxSnrMgn:
	     return NO_MIBINSTANCE;
            long_return =  pConf->maxDownSnr;
           return (unsigned char *)&long_return;;
		
      case I_vdslLineConfDownMinSnrMgn:
	      return NO_MIBINSTANCE;
            long_return =  pConf->minDownSnr;
           return (unsigned char *)&long_return;;
		
      case I_vdslLineConfDownTargetSnrMgn:
	     return NO_MIBINSTANCE;
           long_return =  pConf->targetDownSnr;
           return (unsigned char *)&long_return;;

      case I_vdslLineConfUpMaxSnrMgn:
	    return NO_MIBINSTANCE;	
          long_return =  pConf->maxUpSnr;
          return (unsigned char *)&long_return;;
		  
      case I_vdslLineConfUpMinSnrMgn:
	    return NO_MIBINSTANCE;
          long_return =  pConf->minUpSnr;
          return (unsigned char *)&long_return;
		
      case I_vdslLineConfUpTargetSnrMgn:
	    return NO_MIBINSTANCE;
          long_return =  pConf->targetUpSnr;
          return (unsigned char *)&long_return;
		
      case I_vdslLineConfDownMaxInterDelay:
          long_return = pConf->maxDownInv;
          return (unsigned char *)&long_return;
		
      case I_vdslLineConfUpMaxInterDelay:
            long_return =  pConf->maxUpInv;
          return (unsigned char *)&long_return;
		
      default:
        return NO_MIBINSTANCE;
      }
} 


static oid vdslPhys_oid[] = {O_vdslPhysTable};
static Object vdslPhys_variables[] = {
   	 { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_vdslPhysEntry,
                 {2, { I_vdslPhysEntry,I_vdslPhysSide}}},

        { SNMP_STRING, (RONLY| COLUMN), (void*)var_vdslPhysEntry,
                 {2, { I_vdslPhysEntry,I_vdslPhysInvSerialNumber}}},

	 { SNMP_STRING, (RONLY| COLUMN), (void*)var_vdslPhysEntry,
                 {2, {I_vdslPhysEntry, I_vdslPhysInvVendorID }}},
                 
         { SNMP_STRING, (RONLY| COLUMN), (void*)var_vdslPhysEntry,
                 {2, {I_vdslPhysEntry, I_vdslPhysInvVersionNumbe}}},        
                 
        { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_vdslPhysEntry,
                 {2, {I_vdslPhysEntry, I_vdslPhysCurrSnrMgn}}},

        { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslPhysEntry,
                 {2, {I_vdslPhysEntry, I_vdslPhysCurrAtn}}},

        { SNMP_BITS  , (RONLY| COLUMN), (void*)var_vdslPhysEntry,
                 {2, {I_vdslPhysEntry, I_vdslPhysCurrStatus}}},

	 { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_vdslPhysEntry,
                 {2, { I_vdslPhysEntry,I_vdslPhysCurrOutputPwr }}},
                 
        { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslPhysEntry,
                 {2, { I_vdslPhysEntry,I_vdslPhysCurrAttainableRate }}},

	  { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslPhysEntry,
                 {2, {I_vdslPhysEntry, I_vdslPhysCurrLineRate}}},
                 
       { (int)NULL }
};

static SubTree  vdslPhys_tree =  { NULL,  vdslPhys_variables,
                (sizeof(vdslPhys_oid)/sizeof(oid)), vdslPhys_oid};



static oid vdslPerfData_oid[] = {O_vdslPerfDataTable};
static Object vdslPerfData_variables[] = {
   	 { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_vdslPerfDataEntry,
                 {2, { I_vdslPerfDataEntry,I_vdslPerfDataValidIntervals}}},

        { SNMP_INTEGER, (RONLY| COLUMN), (void*)var_vdslPerfDataEntry,
                 {2, { I_vdslPerfDataEntry,I_vdslPerfDataInvalidIntervals}}},

	 { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslPerfDataEntry,
                 {2, {I_vdslPerfDataEntry, I_vdslPerfDataLofs }}},
                 
        { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslPerfDataEntry,
                 {2, {I_vdslPerfDataEntry, I_vdslPerfDataLoss}}},
                 
        { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslPerfDataEntry,
                 {2, {I_vdslPerfDataEntry, I_vdslPerfDataLprs}}},

        { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslPerfDataEntry,
                 {2, {I_vdslPerfDataEntry, I_vdslPerfDataLols}}},

        { SNMP_GAUGE  , (RONLY| COLUMN), (void*)var_vdslPerfDataEntry,
                 {2, {I_vdslPerfDataEntry, I_vdslPerfDataESs}}},

	 { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslPerfDataEntry,
                 {2, { I_vdslPerfDataEntry,I_vdslPerfDataSESs }}},
                 
        { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslPerfDataEntry,
                 {2, { I_vdslPerfDataEntry,I_vdslPerfDataUASs }}},

	  { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslPerfDataEntry,
                 {2, {I_vdslPerfDataEntry, I_vdslPerfDataInits}}},
                 
       { (int)NULL }
};

static SubTree  vdslPerfData_tree =  { NULL,  vdslPerfData_variables,
                (sizeof(vdslPerfData_oid)/sizeof(oid)), vdslPerfData_oid};


static oid vdslLineConfProfileTable_oid[] = {O_vdslLineConfProfileTable};
static Object vdslLineConfProfileTable_variables[] = {

         { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslConfEntry,
                 {2, { I_vdslLineConfProfileEntry ,I_vdslLineConfDownMaxSnrMgn}}},

        { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslConfEntry,
                 {2, { I_vdslLineConfProfileEntry ,I_vdslLineConfDownMinSnrMgn}}},

         { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslConfEntry,
                 {2, { I_vdslLineConfProfileEntry ,I_vdslLineConfDownTargetSnrMgn}}},

        { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslConfEntry,
                 {2, { I_vdslLineConfProfileEntry ,I_vdslLineConfUpMaxSnrMgn}}},

         { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslConfEntry,
                 {2, { I_vdslLineConfProfileEntry ,I_vdslLineConfUpMinSnrMgn}}},

        { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslConfEntry,
                 {2, { I_vdslLineConfProfileEntry ,I_vdslLineConfUpTargetSnrMgn}}},
                 
   	 { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslConfEntry,
                 {2, { I_vdslLineConfProfileEntry ,I_vdslLineConfDownMaxInterDelay}}},

        { SNMP_GAUGE, (RONLY| COLUMN), (void*)var_vdslConfEntry,
                 {2, { I_vdslLineConfProfileEntry ,I_vdslLineConfUpMaxInterDelay}}},
                 
       { (int)NULL }
};

static SubTree  vdslLineConfProfileTable_tree =  { NULL,  vdslLineConfProfileTable_variables,
                (sizeof(vdslLineConfProfileTable_oid)/sizeof(oid)), vdslLineConfProfileTable_oid};

/* This is the MIB registration function. This should be called */
/* within the init_cpe_dsl_mib-function */
void register_subtrees_of_vdsl_mib()
{
    insert_group_in_mib(&vdslPhys_tree);
    insert_group_in_mib(&vdslPerfData_tree);
    insert_group_in_mib(&vdslLineConfProfileTable_tree);
}




