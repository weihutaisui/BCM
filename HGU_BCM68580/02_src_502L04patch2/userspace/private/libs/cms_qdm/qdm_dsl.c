#ifdef DMP_BASELINE_1  

#ifdef DMP_ADSLWAN_1

#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_qdm.h"
#include "cms_util.h"
#include "AdslMibDef.h"
#include "devctl_adsl.h"

/*!\file qdm_dsl.c
 *
 */
#ifdef DMP_PTMWAN_1
extern CmsRet rutWl2_getPtmDslIntfObject(InstanceIdStack *iidStack, WanDslIntfCfgObject **wanDslIntfObj);
#endif
extern CmsRet rutWl2_getAtmDslIntfObject(InstanceIdStack *iidStack, WanDslIntfCfgObject **wanDslIntfObj);

void qdmDsl_getPath1LineRateLocked_igd(int *lineRate)
{
   WanDslIntfCfgObject *dslIntfObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 found = FALSE;
   int rate = 0;

   while (!found &&
          cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **) &dslIntfObj) == CMSRET_SUCCESS)
   {
      if (dslIntfObj->enable && !cmsUtl_strcmp(dslIntfObj->status, MDMVS_UP))
      {
         /* Path/Bearer 1: Up Stream Rate */
         rate = dslIntfObj->X_BROADCOM_COM_UpstreamCurrRate_2;
         found = TRUE;
      }
      cmsObj_free((void **) &dslIntfObj);
   }
   *lineRate = rate;
}

/* isVdsl is TRUE if VDSL, else ADSL of some sort. isAtm is TRUE if ATM mode, else PTM mode */
void qdmDsl_getDSLTrainedModeLocked_igd(UBOOL8 *isVdsl, UBOOL8 *isAtm)
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslCfgObj=NULL;
   UBOOL8 found=FALSE;
   *isVdsl = FALSE;
   *isAtm = TRUE;
   UINT32 bondingLineNumber=0;
   adslMibInfo adslMib;
   long adslMibSize=sizeof(adslMib);

   /* first check to see if the Line is trained for VDSL */
   while (!found &&
          cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **)&dslCfgObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("got DSL_INTF_CFG with enable=%d link status=%s", dslCfgObj->enable, dslCfgObj->status);
      if (dslCfgObj->enable && !cmsUtl_strcmp(dslCfgObj->status, MDMVS_UP))
      {
         found = TRUE;
         bondingLineNumber = dslCfgObj->X_BROADCOM_COM_BondingLineNumber;
         if (!cmsUtl_strcmp(dslCfgObj->modulationType, MDMVS_VDSL2))
         {
            *isVdsl = TRUE;
         }
      }
      cmsObj_free((void **) &dslCfgObj);
   }
   if (found)
   {
      if (CMSRET_SUCCESS == xdslCtl_GetObjectValue(bondingLineNumber, NULL, 0, (char *) &adslMib, &adslMibSize))
      {
         /* the following logic is copied from rut_dsl.c to figure out if the channel is trained for ATM or PTM mode */
#if defined(DMP_VDSL2WAN_1) || defined(DMP_X_BROADCOM_COM_VDSL2WAN_1)
         cmsLog_debug("MIB rcv2Info %d %d", adslMib.vdslInfo[0].rcv2Info.tmType[0], adslMib.vdslInfo[0].rcv2Info.tmType[1]);
         cmsLog_debug("MIB xmt2Info %d %d", adslMib.vdslInfo[0].xmt2Info.tmType[0], adslMib.vdslInfo[0].xmt2Info.tmType[1]);
         
         /* a 1 in tmType[0] means DPAPI_DATA_ATM */
         /* a 4 in tmType[0] means DPAPI_DATA_NITRO - ATM with header compression, possible with Broadcom CO */
         if (*isVdsl)
         {
            if ((adslMib.vdslInfo[0].xmt2Info.tmType[0] != 1) &&
                (adslMib.vdslInfo[0].xmt2Info.tmType[0] != 4))
            {
               /* this is a PTM connection */
               *isAtm = FALSE;
            }
         }
         else
#endif /* defined(DMP_VDSL2WAN_1) || defined(DMP_X_BROADCOM_COM_VDSL2WAN_1) */
         {
            UINT8 connType=adslMib.adsl2Info2lp[0].xmtChanInfo.connectionType;
            
            cmsLog_debug("adsl connType=%d", connType);
            if ((connType != 1) && (connType != 4))
            {
               /* this is a PTM connection */
               *isAtm = FALSE;
            }
         }
      } /* ADSL mib structure retrieved */
   } /* found line */
   cmsLog_debug("isVdsl=%d, isAtm=%d", *isVdsl, *isAtm);
}

UBOOL8 qdmDsl_isXdslLinkUpLocked_igd()
{
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   WanDslIntfCfgObject *dslCfgObj=NULL;
   UBOOL8 isXdslLinkUp=FALSE;

   /*
    * Hmm, I could also send a message to ssk asking it for link status intead of
    * going through the DSL_INTF objects here.
    */

   while (!isXdslLinkUp &&
          cmsObj_getNext(MDMOID_WAN_DSL_INTF_CFG, &iidStack, (void **)&dslCfgObj) == CMSRET_SUCCESS)
   {
      cmsLog_debug("got DSL_INTF_CFG with enable=%d link status=%s", dslCfgObj->enable, dslCfgObj->status);
      if (dslCfgObj->enable && !cmsUtl_strcmp(dslCfgObj->status, MDMVS_UP))
      {
         isXdslLinkUp = TRUE;
      }
      cmsObj_free((void **) &dslCfgObj);
   }

   cmsLog_debug("return %d", isXdslLinkUp);

   return isXdslLinkUp;
}

#ifdef DMP_X_BROADCOM_COM_DSLBONDING_1
void qdmDsl_isDslBondingEnabled_igd(UBOOL8 *enabled)
{
   WanDslIntfCfgObject *dslIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

#ifdef DMP_PTMWAN_1
   ret = rutWl2_getPtmDslIntfObject(&iidStack, &dslIntfObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get PTM DslIntf obj, ret=%d", ret);
      *enabled = FALSE;
      return;
   }
#endif

#ifdef DMP_X_BROADCOM_COM_ATMWAN_1
   if (dslIntfObj == NULL) {
      memset (&iidStack, 0, sizeof (InstanceIdStack)) ;
      ret = rutWl2_getAtmDslIntfObject(&iidStack, &dslIntfObj);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("failed to get ATM DslIntf obj, ret=%d", ret);
         *enabled = FALSE;
         return;
      }
   }
#endif

   if (dslIntfObj) {
      *enabled = dslIntfObj->X_BROADCOM_COM_EnableBonding;
      cmsObj_free((void **) &dslIntfObj);
   }
   else
      *enabled = (UBOOL8) 0 ;
   
   cmsLog_debug("returning enabled=%d", *enabled);
}
#endif /* DMP_X_BROADCOM_COM_DSLBONDING_1 */


#endif /*  DMP_ADSLWAN_1 */

#endif /* DMP_BASELINE_1 */

