/***************************************************************************
 *     Copyright (c) 2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile:$
 * $brcm_Revision:$
 * $brcm_Date:$
 *
 * [File Description:]
 *      CPE DSL MIB handler helper functions.
 * Revision History:
 *      Create 1/2005   yt
 * $brcm_Log:$
 *
 ***************************************************************************/

/* General includes */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <net/route.h>
#include <signal.h>
#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "asn1.h"
#include "snmp.h"
#include "agtMib.h"
#include "agtEngine.h"
#include "vdslMib.h"
#include "vdslMibHelper.h"

#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

//#include "psidefs.h"
//#include "psiapi.h"
#include "AdslMibDef.h"
#include "syscall.h"
#include "board.h"
//#include "board_api.h"
#include "sysdiag.h"
#include "adslctlapi.h"
//#include "ifcdefs.h"
//#include "dbapi.h"
//#include "snmpstruc.h"
//#include "ifcrtwanapi.h"
//#include "atmMibDefs.h"
//#include "atmMibHelper.h"
//#include "pmapdefs.h"

#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
extern SNMP_DEBUG_FLAG snmpDebug;
#endif


WanDslIntfCfgObject           g_dslIntfObj;
WanDslIntfStatsTotalObject  g_dslStatsObj;


void cpeGetXdslSts(void)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslIntfObj = NULL;
   WanDslIntfStatsTotalObject *dslTotalStatsObj = NULL;
   int xDsl2Mode = 0, vdslMode = 0;
   UBOOL8 foundLinkUp=FALSE;;


   /*
    * On the 6368, there are 2 WAN DSLIntfCfg objects, one for ATM and one for PTM.
    * I don't know which one will have link up.  So if I find one that has link up,
    * use that one.  Otherwise, use the first one, which is the ATM one (and it 
    * will still have link down.)
    * This algorithm will work even when we have 1 WANDevice.
    */
   while ((cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfObj) == CMSRET_SUCCESS))
   {
      if ((dslIntfObj->enable) &&
          (0 == cmsUtl_strcmp(dslIntfObj->status, MDMVS_UP)))
      {
         foundLinkUp = TRUE;
         /* dslIntfObj is still pointing to the object that is up. */
         break;
      }

      cmsObj_free((void **) &dslIntfObj);
   }

   if (!foundLinkUp)
   {
      /*
       * WAN link is not up, so just get the first instance of the DSLIntfObj
       * and report based on that object (which currently has link down anyways).
       */
      INIT_INSTANCE_ID_STACK(&iidStack);
      cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfObj);
   }

    memcpy(&g_dslIntfObj,dslIntfObj,sizeof(WanDslIntfCfgObject));


    if (cmsUtl_strcmp(dslIntfObj->status,MDMVS_UP) == 0)
    {
         if (cmsObj_get(MDMOID_WAN_DSL_INTF_STATS_TOTAL, &iidStack, 0, (void **) &dslTotalStatsObj) == CMSRET_SUCCESS)
         {
               memcpy(&g_dslStatsObj,dslTotalStatsObj,sizeof(WanDslIntfStatsTotalObject));
		 cmsObj_free((void **) &dslTotalStatsObj);
         }
    }
	
    cmsObj_free((void **) &dslIntfObj);

}

