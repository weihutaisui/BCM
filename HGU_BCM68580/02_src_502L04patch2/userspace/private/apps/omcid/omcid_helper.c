/***********************************************************************
 *
 *  Copyright (c) 2017 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:omcid

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


/*****************************************************************************
*    Description:
*
*      OMCID helper functions.
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include "owsvc_api.h"
#include "omcid_me.h"
#include "mdmlite_api.h"
#include "owrut_api.h"
#include "omcid_helper.h"
#include "me_handlers.h"
#include "omciutl_cmn.h"


/* ---- Private Constants and Types --------------------------------------- */

#define OMCID_LOCK_TIMEOUT  (3 * MSECS_IN_SEC)


/* ---- Private Function Prototypes --------------------------------------- */

extern CmsRet addDefaultGponObjects(void);
extern CmsRet rutGpon_deleteAllRules(void);
extern omciMsgResult omci_msg_get_meInfo(UINT16 meClass, omciMeInfo_t *pMeInfo);


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */


/* ---- Functions --------------------------------------------------------- */


/*****************************************************************************
*  FUNCTION:  deleteFunc
*  PURPOSE:   Call back function when an object is deleted.
*  PARAMETERS:
*      objNode - pointer to an object node.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void deleteFunc(_MdmObjectNode *objNode, InstanceIdStack *iidStack
  __attribute__((unused)), void *cbContext __attribute__((unused)))
{
    OmciMdmOidClassId_t info;
    UINT16 meClass = 0;
    omciMeInfo_t meInfo;
    omciMsgResult getMeInfoRes;
    InstanceIdStack iidStack1;
    InstanceIdStack iidStack2;
    CmsRet ret = CMSRET_SUCCESS;
    _MdmObjectNode *objNodeDel;

    memset(&info, 0, sizeof(OmciMdmOidClassId_t));
    info.mdmOid = objNode->oid;
    omciUtl_oidToClassId(&info);
    meClass = info.classId;

    // Software Image (Class ID = 7) is dynamic object
    // but it should not be deleted
    getMeInfoRes = omci_msg_get_meInfo(meClass, &meInfo);
    if (getMeInfoRes == OMCI_MSG_RESULT_SUCCESS)
    {
        if (((meInfo.flags & OMCI_ME_FLAG_DYNAMIC) != 0) &&
          (meClass != OMCI_ME_SOFTWARE_IMAGE))
        {
            INIT_INSTANCE_ID_STACK(&iidStack1);
            INIT_INSTANCE_ID_STACK(&iidStack2);
            while ((ret = _cmsObj_getNext(objNode->oid, &iidStack1,
              (void**)&objNodeDel)) == CMSRET_SUCCESS)
            {
                ret = _cmsObj_deleteInstance(objNode->oid, &iidStack1);
                if (ret != CMSRET_SUCCESS)
                {
                    cmsLog_error("cmsObj_deleteInstance() failed, ret=%d",
                      ret);
                }
                iidStack1 = iidStack2;
                _cmsObj_free((void **)&objNodeDel);
            }
        }
    }
}

/*****************************************************************************
*  FUNCTION:  omciMibInternalDataReset
*  PURPOSE:   Delete internal objects.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
static void omciMibInternalDataReset(void)
{
    InstanceIdStack iidStack;
    BCM_MulticastSubscriberMonitorObject *mon = NULL;

    INIT_INSTANCE_ID_STACK(&iidStack);
    while (_cmsObj_getNext(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR,
      &iidStack, (void**)&mon) == CMSRET_SUCCESS)
    {
        _cmsObj_deleteInstance(MDMOID_BC_MULTICAST_SUBSCRIBER_MONITOR, &iidStack);
        _cmsObj_free((void**)&mon);
        INIT_INSTANCE_ID_STACK(&iidStack);
    }
}

/*****************************************************************************
*  FUNCTION:  omciMibDataReset
*  PURPOSE:   OMCI MIB reset actions.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet omciMibDataReset(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    /* Inform other processes. */
    _owapi_rut_announceMibreset();

    ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return ret;
    }

    rutGpon_setOmciMibResetState(TRUE);

    rutGpon_deleteAllRules();

    /* Delete OMCI ME instances. */
    mdmlite_traverseOidNodes(deleteFunc, NULL);

    omciMibInternalDataReset();

    rutGpon_setOmciMibResetState(FALSE);

    rutGpon_reloadMcpd();

    omciDm_removeAllUniEntries();

    /* Re-create the default MIB. */
    ret = addDefaultGponObjects();

    _cmsLck_releaseLock();

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciDebug_dumpInfo
*  PURPOSE:   OMCI debug function to dump internal data.
*  PARAMETERS:
*      flag - input flag.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omciDebug_dumpInfo(UINT32 flag)
{
    if (flag == 0)
    {
        omcimib_dumpAll();
        printAllOmciVlanCtlRules();
#if DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
        printAllOmciServiceRules();
#endif
        omciDm_dumpVlanFlowIfPair();
        omciDm_dumpGemList();
        omciDm_dumpGemUniList();
        omciDm_dumpUniList();
    }
    else if (flag == 0xffff)
    {
        (void)omciMibDataReset();
    }
    else
    {
        omcimib_dumpObjectId(flag);
    }
}

#if DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
/*****************************************************************************
*  FUNCTION:  omci_gpon_wan_service_status_change_handler
*  PURPOSE:   Update service entry when the RG sevice status is changed.
*  PARAMETERS:
*      pService - pointer to the RG sevice message.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet omci_gpon_wan_service_status_change_handler(OmciServiceMsgBody *pService)
{
    POMCI_SERVICE_ENTRY pOmciServiceEntry = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    if (pService->serviceParams.serviceStatus == FALSE)
    {
        return ret;
    }

    pOmciServiceEntry = omci_service_get(pService->serviceParams.pbits,
      pService->serviceParams.vlanId, pService->l2Ifname);

    while (pOmciServiceEntry != NULL)
    {
        /* 
         * should use pbits and vlanId that are received in
         * pService (from SSK) since these value might have
         * don't care value (0xFFFF) in pOmciServiceEntry->service
         */
        pOmciServiceEntry->service.serviceParams.pbits = 
          pService->serviceParams.pbits;
        pOmciServiceEntry->service.serviceParams.vlanId = 
          pService->serviceParams.vlanId;

        _owapi_rut_sendServiceInfoMsg(&(pOmciServiceEntry->service));
        pOmciServiceEntry = omci_service_getNext(pService->serviceParams.pbits,
          pService->serviceParams.vlanId, pService->l2Ifname, pOmciServiceEntry);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omci_gpon_link_status_change_handler
*  PURPOSE:   Update service entry status when the GPON link status is changed.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet omci_gpon_link_status_change_handler(void)
{
    char ifName[CMS_IFNAME_LENGTH];
    UBOOL8 linkStatus = FALSE;
    BCM_Ploam_StateInfo info;
    POMCI_SERVICE_ENTRY pOmciServiceEntry = NULL;
    CmsRet ret = CMSRET_SUCCESS;
    int retGpon = gponCtl_getControlStates(&info);

    if (retGpon == 0 && info.operState == BCM_PLOAM_OSTATE_OPERATION_O5)
    {
        linkStatus = TRUE;
    }

    ret = rutVeip_getVeipVlanName(ifName);

    omciDebugPrint(OMCI_DEBUG_MODULE_OMCI,
      "\n===> omci_gpon_link_status_change_handler: operState[%d], "
      "linkStatus[%d], ifName[%s]\n\n",
      info.operState, linkStatus, ifName);

    pOmciServiceEntry = omci_service_get(OMCI_FILTER_DONT_CARE,
      OMCI_FILTER_DONT_CARE, ifName);

    while (pOmciServiceEntry != NULL)
    {
        if (pOmciServiceEntry->service.serviceParams.serviceStatus != linkStatus)
        {
            pOmciServiceEntry->service.serviceParams.serviceStatus = linkStatus;

            _owapi_rut_sendServiceInfoMsg(&(pOmciServiceEntry->service));
            omciDebugPrint(OMCI_DEBUG_MODULE_OMCI,
              "\n===> omci_gpon_link_status_change_handler: "
              "pbits[%d], vlanId[%d], l2Ifname[%s]\n\n",
            pOmciServiceEntry->service.serviceParams.pbits,
            pOmciServiceEntry->service.serviceParams.vlanId,
            pOmciServiceEntry->service.l2Ifname);
        }
        else
        {
            omciDebugPrint(OMCI_DEBUG_MODULE_OMCI,
              "\n===> omci_gpon_link_status_change_handler2: "
              "pbits[%d], vlanId[%d], l2Ifname[%s], S%d\n\n",
              pOmciServiceEntry->service.serviceParams.pbits,
              pOmciServiceEntry->service.serviceParams.vlanId,
              pOmciServiceEntry->service.l2Ifname,
              linkStatus);
        }

        pOmciServiceEntry = omci_service_getNext(OMCI_FILTER_DONT_CARE,
          OMCI_FILTER_DONT_CARE, ifName, pOmciServiceEntry);
    }

    return ret;
}
#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */
