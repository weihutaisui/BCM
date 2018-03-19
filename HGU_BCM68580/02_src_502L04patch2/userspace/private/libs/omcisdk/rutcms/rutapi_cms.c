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
*      OMCI Wrapper (OW) run-time utility (RUT) implementation based on CMS.
*
*      In the turn-key solution, OMCI is linked with CMS_CORE via the RUT API.
*      Notes:
*       - An OW RUT function is OMCI-related glue/adaptation code.
*       - RUT API is uni-directional. It is an "outbound" API from OMCI
*         perspective.
*       - A RUT function may access the CMS MDM.
*       - A RUT function does not access the OMCI MIB.
*       - A RUT function may send messages to another process.
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include <pthread.h>
#include <fcntl.h>
#include "owsvc_api.h"
#include "owrut_api.h"
#include "owrut_cmsdefs.h"
#include "ctlutils_api.h"
#include "me_handlers.h"

#include "omci_api.h"
#include "omcipm_api.h"

#include "mdm.h"
#include "rut_tmctl_wrap.h"
#include "rut_moca.h"
#include "rut_multicast.h"
#include "rut_route.h"
#include "rut_util.h"
#include "rut_lan.h"
#include "rut_wan.h"
#include "rut_network.h"
#include "rut_dnsproxy.h"
#include "rut_pmirror.h"
#include "rut_gponwan.h"
#include "rut_iptables.h"
#include "rut_omci.h"
#include "owrut_cmsdefs.h"


/* ---- Private Constants and Types --------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */

rutCmsCxt_t RutCmsCxt;


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  owapi_rut_cxtInit
*  PURPOSE:   Initialize the OMCI context to be used by the RUT.
*  PARAMETERS:
*      msgHandle - CMS message handler.
*      eid - OMCI entity ID.
*      shmId - shared memory ID.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_cxtInit(void *msgHandle, const UINT32 eid, SINT32 shmId)
{
    CmsRet ret = CMSRET_SUCCESS;

    RutCmsCxt.eid = eid;
    RutCmsCxt.msgHandle = msgHandle;

    if ((ret = cmsMdm_init(eid, msgHandle, &shmId)) != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsMdm_init failed, ret=%d", ret);
    }
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_initAppCfgData
*  PURPOSE:   Get OmciSystemObject parameters used by OMCI handler.
*  PARAMETERS:
*      omciCfgData - pointer to OmciSystemObject.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_initAppCfgData(OmciSystemObject *omciCfgData)
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciSystemObject *obj = NULL;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 ethLanObjCount = 0;
    char buf[BUFLEN_16] = {0};
    int count;
    UINT32 ponAppType = OMCI_PON_APP_TYPE_984;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return ret;
    }

    count = cmsPsp_get(RDPA_WAN_TYPE_PSP_KEY, buf, sizeof(buf));
    if (count <= 0)
    {
#ifndef DESKTOP_LINUX
        cmsLog_error("Could not get WAN type from scratch pad");
#endif /* DESKTOP_LINUX */
    }

    if ((!cmsUtl_strncasecmp((const char*)buf, RDPA_WAN_TYPE_VALUE_XGPON1,
      strlen(RDPA_WAN_TYPE_VALUE_XGPON1))) ||
      (!cmsUtl_strncasecmp((const char*)buf, RDPA_WAN_TYPE_VALUE_NGPON2,
      strlen(RDPA_WAN_TYPE_VALUE_NGPON2))) ||
      (!cmsUtl_strncasecmp((const char*)buf, RDPA_WAN_TYPE_VALUE_XGS,
      strlen(RDPA_WAN_TYPE_VALUE_XGS))))
    {
        ponAppType = OMCI_PON_APP_TYPE_987;
    }
    else
    {
        ponAppType = OMCI_PON_APP_TYPE_984;
    }

    /* Count the number of LanDevice.{i}.LANEthernetInterface objs in MDM. */
    {
       InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
       LanEthIntfObject *lanEthObj = NULL;
       while (cmsObj_getNextFlags(MDMOID_LAN_ETH_INTF, &iidStack1,
         OGF_NO_VALUE_UPDATE, (void**)&lanEthObj) == CMSRET_SUCCESS)
       {
           if (strstr(lanEthObj->X_BROADCOM_COM_IfName, ".") == NULL)
           {
               ethLanObjCount++;
           }
           cmsObj_free((void **) &lanEthObj);
       }
       cmsLog_debug("EthLanObjCount=%d", ethLanObjCount);
    }

    ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void*)&obj);
    if (ret == CMSRET_SUCCESS)
    {
        if (obj->numberOfEthernetPorts != ethLanObjCount)
        {
            obj->numberOfEthernetPorts = ethLanObjCount;
            cmsObj_set(obj, &iidStack);
        }

        if (obj->ponAppType != ponAppType)
        {
            obj->ponAppType = ponAppType;
            cmsObj_set(obj, &iidStack);
        }

#ifndef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
        obj->numberOfVeipPorts = 0;
        cmsObj_set(obj, &iidStack);
#endif /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

        memcpy(omciCfgData, obj, sizeof(OmciSystemObject));
        omciCfgData->_oid = MDMOID_OMCI_SYSTEM;
        cmsObj_free((void**)&obj);
    }
    else
    {
        cmsLog_error("cmsObj_get() failed, ret=%d", ret);
    }

    cmsLck_releaseLock();
    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_initLoggingFromConfig
*  PURPOSE:   Initialize logging level.
*  PARAMETERS:
*      useConfiguredLogLevel - true: use MDMOID_OMCID_CFG defined log level.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_initLoggingFromConfig(UBOOL8 useConfiguredLogLevel)
{
    OmcidCfgObject *obj;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return;
    }

    if ((ret = cmsObj_get(MDMOID_OMCID_CFG, &iidStack, 0, (void**)&obj))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("get OMCID_CFG object failed, ret=%d", ret);
    }
    else
    {
        if (useConfiguredLogLevel)
        {
            cmsLog_setLevel(cmsUtl_logLevelStringToEnum(obj->loggingLevel));
        }

        cmsLog_setDestination(cmsUtl_logDestinationStringToEnum
          (obj->loggingDestination));

        cmsObj_free((void**)&obj);
    }

    cmsLck_releaseLock();
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_doSystemAction
*  PURPOSE:   Execute a shell command.
*  PARAMETERS:
*      task - calling task name string.
*      cmd - shell command string.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_doSystemAction(const char *task, char *cmd)
{
    rut_doSystemAction(task, cmd);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_systemReboot
*  PURPOSE:   Perform a system reboot.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_systemReboot(void)
{
    CmsMsgHeader msg = EMPTY_MSG_HEADER;

    msg.type = CMS_MSG_REBOOT_SYSTEM;
    msg.src = getCmsEid();
    msg.dst = EID_SMD;
    msg.flags_request = 1;

    cmsMsg_sendAndGetReply(getCmsMsgHandle(), &msg);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_isUserTMCtlOwner
*  PURPOSE:   Check whether the user is TMCtl owner.
*  PARAMETERS:
*      user - application that may use TMCtl.
*  RETURNS:
*      TRUE - TMCtl owner; FALSE - otherwise.
*  NOTES:
*      None.
*****************************************************************************/
BOOL owapi_rut_tmctl_isUserTMCtlOwner(eTmctlUser user)
{
    return rut_tmctl_isUserTMCtlOwner(user);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_getQueueOwner
*  PURPOSE:   Check whether the TM queue owner is backhaul(BH) or frontend(FE)
*             in a dual-managed device.
*  PARAMETERS:
*      None.
*  RETURNS:
*      TMCtl owner.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 owapi_rut_tmctl_getQueueOwner(void)
{
    return rut_tmctl_getQueueOwner();
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_getQueueMap
*  PURPOSE:   Get the TM queue mapping scheme (Q0P7 or Q7P7).
*  PARAMETERS:
*      None.
*  RETURNS:
*      Queue mapping scheme.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 owapi_rut_tmctl_getQueueMap(void)
{
    return rut_tmctl_getQueueMap();
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_getQueueScheme
*  PURPOSE:   Get both TM queue owner and mapping scheme.
*  PARAMETERS:
*      owner - pointer to queue owner.
*      scheme - pointer to queue mapping scheme.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_tmctl_getQueueScheme(UINT32 *owner, UINT32 *scheme)
{
    return rut_tmctl_getQueueScheme(owner, scheme);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_portTmInit
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_portTmInit(tmctl_devType_e devType,
  tmctl_if_t *if_p, uint32_t cfgFlags)
{
    return rutomci_tmctl_portTmInit(devType, if_p, cfgFlags);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_portTmUninit
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_portTmUninit(tmctl_devType_e devType,
  tmctl_if_t *if_p)
{
    return rutomci_tmctl_portTmUninit(devType, if_p);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_getQueueCfg
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_getQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueCfg_t *qcfg_p)
{
    return rutomci_tmctl_getQueueCfg(devType, if_p, queueId, qcfg_p);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_delQueueCfg
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_delQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId)
{
    return rutomci_tmctl_delQueueCfg(devType, if_p, queueId);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_getPortShaper
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_getPortShaper(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_shaper_t *shaper_p)
{
    return rutomci_tmctl_getPortShaper(devType, if_p, shaper_p);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_setPortShaper
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_setPortShaper(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_shaper_t *shaper_p)
{
    return rutomci_tmctl_setPortShaper(devType, if_p, shaper_p);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_getQueueDropAlg
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_getQueueDropAlg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p)
{
    return rutomci_tmctl_getQueueDropAlg(devType, if_p, queueId, dropAlg_p);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_setQueueDropAlg
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_setQueueDropAlg(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p)
{
    return rutomci_tmctl_setQueueDropAlg(devType, if_p, queueId, dropAlg_p);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_getPortTmParms
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_getPortTmParms(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_portTmParms_t *tmParms_p)
{
    return rutomci_tmctl_getPortTmParms(devType, if_p, tmParms_p);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_setQueueCfg
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_setQueueCfg(tmctl_devType_e devType,
  tmctl_if_t *if_p, tmctl_queueCfg_t *qcfg_p)
{
   return rutomci_tmctl_setQueueCfg(devType, if_p, qcfg_p);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_getQueueStats
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_getQueueStats(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueStats_t *stats_p)
{
    return rutomci_tmctl_getQueueStats(devType,if_p, queueId, stats_p);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_tmctl_setQueueDropAlgExt
*  PURPOSE:   Refer tmctl_api.c.
*  PARAMETERS:
*      Refer tmctl_api.c.
*  RETURNS:
*      tmctl_ret_e.
*  NOTES:
*      None.
*****************************************************************************/
tmctl_ret_e owapi_rut_tmctl_setQueueDropAlgExt(tmctl_devType_e devType,
  tmctl_if_t *if_p, int queueId, tmctl_queueDropAlg_t *dropAlg_p)
{
    return rutomci_tmctl_setQueueDropAlgExt(devType, if_p, queueId,
      dropAlg_p);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_processHostCtrlChange
*  PURPOSE:   Process host-controlled multicast configuration update.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_processHostCtrlChange(void)
{
#if defined(DMP_X_BROADCOM_COM_MCAST_1)
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return ret;
    }

    ret = rutMulti_processHostCtrlChange(rutMulti_getHostCtrlConfig());
    cmsLck_releaseLock();
    return ret;

#else
    return CMSRET_SUCCESS;
#endif
}

/*****************************************************************************
*  FUNCTION:  owapi_mcast_getHostCtrlConfig
*  PURPOSE:   Get host-controlled multicast mode. It is configured outside of
*             OMCI.
*  PARAMETERS:
*      None.
*  RETURNS:
*      TRUE - host-controlled multicast mode enabled. FALSE - otherwise.
*  NOTES:
*      None.
*****************************************************************************/
UBOOL8 owapi_mcast_getHostCtrlConfig(void)
{
#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1) || defined(DMP_X_BROADCOM_COM_MLDSNOOP_1)
    CmsRet ret = CMSRET_SUCCESS;
    UBOOL8 mode = FALSE;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return mode;
    }

    mode = rutMulti_getHostCtrlConfig();
    cmsLck_releaseLock();
    return mode;
#else
    return FALSE;
#endif
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_updateIgmpMldSnoopingIntfList
*  PURPOSE:   Update the list of interfaces for IGMP/MLD snooping.
*  PARAMETERS:
*      None.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_updateIgmpMldSnoopingIntfList(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return;
    }

    rutMulti_updateIgmpMldSnoopingIntfList();
    cmsLck_releaseLock();
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_reloadMcastCtrl
*  PURPOSE:   Reload an external IGMP/MLD snooping process.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_reloadMcastCtrl(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return CMSRET_INTERNAL_ERROR;
    }

    ret = rutMulti_reloadMcpd();
    cmsLck_releaseLock();

    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_getOmciMcastCfgInfo
*  PURPOSE:   Get multicast configuration info. It is owned/configured by OMCI,
*             shared with external modules.
*  PARAMETERS:
*      mcastCfgInfo - pointer to omciMcastCfgInfo.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_getOmciMcastCfgInfo(omciMcastCfgInfo *mcastCfgInfo)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciRtdMcastObject *obj = NULL;

    ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return CMSRET_INTERNAL_ERROR;
    }

    ret = cmsObj_get(MDMOID_BCM_OMCI_RTD_MCAST, &iidStack, 0, (void*)&obj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsObj_get(MDMOID_BCM_OMCI_RTD_MCAST) failed, ret=%d",
          ret);
    }

    mcastCfgInfo->igmpAdmission = obj->igmpAdmission;
    mcastCfgInfo->joinForceForward = obj->joinForceForward;
    mcastCfgInfo->upstreamIgmpRate = obj->upstreamIgmpRate;
    cmsObj_free((void**)&obj);

    cmsLck_releaseLock();

    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_setOmciMcastCfgInfo
*  PURPOSE:   Set multicast configuration info. It is owned/configured by OMCI,
*             shared with external modules.
*  PARAMETERS:
*      mcastCfgInfo - pointer to omciMcastCfgInfo.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_setOmciMcastCfgInfo(omciMcastCfgInfo *mcastCfgInfo)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciRtdMcastObject *obj = NULL;

    ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return CMSRET_INTERNAL_ERROR;
    }

    ret = cmsObj_get(MDMOID_BCM_OMCI_RTD_MCAST, &iidStack, 0, (void*)&obj);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("cmsObj_get(MDMOID_BCM_OMCI_RTD_MCAST) failed, ret=%d",
          ret);
        return CMSRET_INTERNAL_ERROR;
    }

    obj->igmpAdmission = mcastCfgInfo->igmpAdmission;
    obj->joinForceForward = mcastCfgInfo->joinForceForward;
    obj->upstreamIgmpRate = mcastCfgInfo->upstreamIgmpRate;
    cmsObj_set(obj, &iidStack);
    cmsObj_free((void**)&obj);

    cmsLck_releaseLock();

    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_startPing
*  PURPOSE:   Start ping command with command line options.
*  PARAMETERS:
*      cmdLine - pointer to command line options.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 owapi_rut_startPing(char *cmdLine)
{
    return rut_sendMsgToSmd(CMS_MSG_START_APP, EID_PING, cmdLine,
      strlen(cmdLine) + 1);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_startTraceroute
*  PURPOSE:   Start traceroute command with command line options.
*  PARAMETERS:
*      cmdLine - pointer to command line options.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 owapi_rut_startTraceroute(char *cmdLine)
{
    return rut_sendMsgToSmd(CMS_MSG_START_APP, EID_TRACERT, cmdLine,
      strlen(cmdLine) + 1);
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_sendServiceInfoMsg
*  PURPOSE:   Notify external Gateway module about the GPON WAN service update.
*  PARAMETERS:
*      pService - pointer to OmciServiceMsgBody.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_sendServiceInfoMsg(const OmciServiceMsgBody *pService)
{
    UINT32 msgDataLen = sizeof(OmciServiceMsgBody);
    char buf[sizeof(CmsMsgHeader) + sizeof(OmciServiceMsgBody)] = {0};
    CmsMsgHeader *msg = (CmsMsgHeader*)buf;
    OmciServiceMsgBody *info =
      (OmciServiceMsgBody*)&(buf[sizeof(CmsMsgHeader)]);
    CmsRet ret = CMSRET_SUCCESS;

    msg->type = CMS_MSG_OMCI_GPON_WAN_SERVICE_STATUS_CHANGE;
    msg->src = getCmsEid();
    msg->dst = EID_SSK;
    msg->flags_response = 0;
    msg->flags_event = 0;
    msg->flags_request = 1;
    msg->dataLength = msgDataLen;

    /* copy service info to message body */
    memcpy(info, pService, msgDataLen);

    cmsLog_debug("===> sendServiceInfoMsg, gemPortIndex=%d, "
        "pbits=%d, vlanId=%d, ifName=%s, serviceType=%d, serviceStatus=%d\n",
        info->linkParams.gemPortIndex, info->serviceParams.pbits,
        info->serviceParams.vlanId, info->l2Ifname,
        info->linkParams.serviceType, info->serviceParams.serviceStatus);

    if ((ret = cmsMsg_sendAndGetReply(getCmsMsgHandle(), msg))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Send CMS_MSG_OMCI_GPON_WAN_SERVICE_STATUS_CHANGE "
          "failed, ret=%d", ret);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  rut_sendMibResetToMcpd
*  PURPOSE:   Send OMCI MIB reset command to MCPD (multicast process).
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet rut_sendMibResetToMcpd(void)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader msg;

    memset(&msg, 0, sizeof(CmsMsgHeader));
    msg.type = CMS_MSG_OMCI_MCPD_MIB_RESET;
    msg.src = getCmsEid();
    msg.dst = EID_MCPD;
    msg.dataLength = 0;
    msg.flags_event = 1;
    msg.flags_response = 1;
    msg.flags_request = 1;
    msg.sequenceNumber = 1;

    if ((ret = cmsMsg_sendAndGetReplyWithTimeout(getCmsMsgHandle(),
      &msg, 5*MSECS_IN_SEC)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Send CMS_MSG_OMCI_MCPD_MIB_RESET failed, ret=%d",
          ret);
    }
    else
    {
        if (msg.wordData != 0)
        {
            ret = CMSRET_INTERNAL_ERROR;
        }
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  rut_sendMibResetToVoip
*  PURPOSE:   Send OMCI MIB reset command to Voice process.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet rut_sendMibResetToVoip(void)
{
    CmsRet cmsResult;
    CmsMsgHeader msgHdr = EMPTY_MSG_HEADER;

    msgHdr.src = getCmsEid();
    msgHdr.dst = EID_SSK;
    msgHdr.type = CMS_MSG_OMCI_VOIP_MIB_RESET;
    msgHdr.dataLength = 0;
    msgHdr.flags_event = 1;
    msgHdr.flags_response = 1;
    msgHdr.flags_request = 1;
    msgHdr.sequenceNumber = 1;

    if ((cmsResult = cmsMsg_sendAndGetReplyWithTimeout(getCmsMsgHandle(),
      &msgHdr, 5*MSECS_IN_SEC)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Send CMS_MSG_OMCI_VOIP_MIB_RESET failed, ret=%d",
          cmsResult);
    }
    else
    {
        if (msgHdr.wordData != 0)
        {
            cmsResult = CMSRET_INTERNAL_ERROR;
        }
    }

    return cmsResult;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_announceMibreset
*  PURPOSE:   Announce OMCI MIB reset to external modules.
*  PARAMETERS:
*      None.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_announceMibreset(void)
{
#if defined(DMP_X_ITU_ORG_VOICE_1)
    /* Send CMS_MSG_OMCI_VOIP_MIB_RESET message to voice stack to clean up. */
    rut_sendMibResetToVoip();
#endif /* #defined(DMP_X_ITU_ORG_VOICE_1) */

#ifdef DMP_X_BROADCOM_COM_IGMP_1
     /* Send CMS_MSG_OMCI_MCPD_MIB_RESET message to MCPD to clean up. */
#ifndef DESKTOP_LINUX
    rut_sendMibResetToMcpd();
#endif /* DESKTOP_LINUX */
#endif /* DMP_X_BROADCOM_COM_IGMP_1 aka SUPPORT_IGMP */

    return CMSRET_SUCCESS;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_updateMcastCtrlIfNames
*  PURPOSE:   Update the list of multicast interfaces that an external
*             multicast process may use.
*  PARAMETERS:
*      mcastIfNames - pointer to the multicast interface name.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_updateMcastCtrlIfNames(char *mcastIfNames)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    IgmpCfgObject *igmpObj = NULL;

    if (mcastIfNames == NULL)
        return CMSRET_INVALID_ARGUMENTS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return ret;
    }

    if ((ret = cmsObj_get(MDMOID_IGMP_CFG,
      &iidStack,
      0, (void**)&igmpObj)) != CMSRET_SUCCESS)
    {
        cmsLog_notice("could not get MDMOID_IGMP_CFG, ret=%d", ret);

        /*
         * Note we are getting the default values for this object.
         * This call should always work, and does not need an
         * iidStack pointer.
         */
        if ((ret = cmsObj_get(MDMOID_IGMP_CFG,
          NULL,
          OGF_DEFAULT_VALUES,
          (void *)&igmpObj)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Could not get default MDMOID_IGMP_CFG, ret=%d", ret);
            cmsLck_releaseLock();
            return CMSRET_INTERNAL_ERROR;
        }
    }

    if (igmpObj->igmpMcastIfNames != NULL &&
      mcastIfNames[0] != '\0' &&
      strstr(igmpObj->igmpMcastIfNames, mcastIfNames) == NULL)
    {
       char buf[BUFLEN_1024];

       memset(buf, 0, BUFLEN_1024);
       snprintf(buf, BUFLEN_1024, "%s %s", igmpObj->igmpMcastIfNames,
         mcastIfNames);

       if (strlen(buf) > BUFLEN_128)
       {
           cmsLog_error("Multicast interfaces are too long");
           cmsObj_free((void**)&igmpObj);
           cmsLck_releaseLock();
           return CMSRET_INVALID_ARGUMENTS;
       }
       else
       {
           CMSMEM_REPLACE_STRING_FLAGS(igmpObj->igmpMcastIfNames, buf, 0);
       }
    }
    else if (igmpObj->igmpMcastIfNames == NULL && mcastIfNames[0] != '\0')
    {
        CMSMEM_REPLACE_STRING_FLAGS(igmpObj->igmpMcastIfNames, mcastIfNames, 0);
    }

    ret = cmsObj_set(igmpObj, &iidStack);

    cmsObj_free((void**)&igmpObj);
    cmsLck_releaseLock();

    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_configDns
*  PURPOSE:   Configure DNS servers.
*  PARAMETERS:
*      dns1 - address of the primary DNS server.
*      dns1 - address of the secondary DNS server.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_configDns(const char *dns1, const char *dns2)
{
    CmsRet setupResult = CMSRET_INVALID_ARGUMENTS;
    char dnsServers[BUFLEN_1024];
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    NetworkConfigObject *networkCfg=NULL;

    if ((dns1 == NULL) || (dns2 == NULL))
    {
        cmsLog_error("NULL pointer");
        return setupResult;
    }

    if ((*dns1 == '\0') && (*dns2 == '\0'))
    {
        cmsLog_error("Both params strings are empty");
        return setupResult;
    }

    snprintf(dnsServers, sizeof(dnsServers), "%s,%s", dns1, dns2);

    if ((setupResult = cmsObj_get(MDMOID_NETWORK_CONFIG, &iidStack, 0,
      (void**)&networkCfg)) == CMSRET_SUCCESS)
    {
        CMSMEM_FREE_BUF_AND_NULL_PTR(networkCfg->DNSIfName);
        CMSMEM_REPLACE_STRING_FLAGS(networkCfg->DNSServers, dnsServers, 0);

        if ((setupResult = cmsObj_set(networkCfg, &iidStack)) != CMSRET_SUCCESS)
            cmsLog_error("Could not set NETWORK_CONFIG, ret=%d", setupResult);
        else
            cmsLog_debug("Dns set OK: dnsServers=%s", dnsServers);

        cmsObj_free((void **) &networkCfg);
    }
    else
        cmsLog_error("Could not get NETWORK_CONFIG, ret=%d", setupResult);

    return setupResult;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_configTcpUdp
*  PURPOSE:   Trigger TCP/UDP config data related configuration.
*  PARAMETERS:
*      tcpUdp - pointer to a tcpUdp object.
*      add - whether the TCP/UDP config data instance is newly added.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void owapi_rut_configTcpUdp(const TcpUdpConfigDataObject *tcpUdp, UBOOL8 add)
{
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return;
    }

    rutOmci_configTcpUdp((BcmOmciRtdTcpUdpConfigDataObject*)tcpUdp, add);

    cmsLck_releaseLock();
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_configIpv4Host
*  PURPOSE:   Configure IPv4 config data.
*  PARAMETERS:
*      meId - OMCI ME id.
*      ifName - interface name associated with the IPv4 config data ME. It is
*               assigned by OMCI handler.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_configIpv4Host(UINT32 meId, char *ifName)
{
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return ret;
    }

    rutOmci_configIpv4Host(meId, ifName);

    cmsLck_releaseLock();
    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_configIpv6Host
*  PURPOSE:   Configure IPv6 config data.
*  PARAMETERS:
*      meId - OMCI ME id.
*      ifName - interface name associated with the IPv6 config data ME. It is
*               assigned by OMCI handler.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_configIpv6Host(UINT32 meId, char *ifName)
{
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return ret;
    }

    rutOmci_configIpv6Host(meId, ifName);

    cmsLck_releaseLock();
    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_getDnsStats
*  PURPOSE:   GET DNS related statistics.
*  PARAMETERS:
*      dnsStats - pointer to DNS statistics.
*  RETURNS:
*      BCM_OMCI_PM_STATUS.
*  NOTES:
*      None.
*****************************************************************************/
#ifdef DMP_X_BROADCOM_COM_DNSPROXY_1
BCM_OMCI_PM_STATUS owapi_rut_getDnsStats(void *dnsStats)
{
    CmsMsgHeader reqMsg = EMPTY_MSG_HEADER;
    UINT8 replyMsgBuf[sizeof(CmsMsgHeader) + sizeof(DnsGetStatsMsgBody)] = {0};
    UINT8 *rspMsgPtr = &replyMsgBuf[0];
    CmsRet ret = CMSRET_SUCCESS;
    BCM_OMCI_PM_STATUS pmRet = OMCI_PM_STATUS_SUCCESS;

    memset(&reqMsg, 0, sizeof(CmsMsgHeader));
    memset(&replyMsgBuf, 0, sizeof(CmsMsgHeader) + sizeof(DnsGetStatsMsgBody));
    reqMsg.type = CMS_MSG_DNSPROXY_GET_STATS;
    reqMsg.src = getCmsEid();
    reqMsg.dst = EID_DNSPROXY;
    reqMsg.flags_request = 1;
    reqMsg.dataLength = 0;
    reqMsg.sequenceNumber = 1;

    ret = cmsMsg_sendAndGetReplyBufWithTimeout(getCmsMsgHandle(),
      (CmsMsgHeader*)&reqMsg, (CmsMsgHeader**)&rspMsgPtr,
      5*MSECS_IN_SEC);
    if (ret == CMSRET_SUCCESS)
    {
        rspMsgPtr += sizeof(CmsMsgHeader);
        memcpy(dnsStats, rspMsgPtr, sizeof(DnsGetStatsMsgBody));
    }
    else
    {
        cmsLog_error("cmsMsg_sendAndGetReplyBufWithTimeout() failed, ret=%d",
          ret);
        pmRet = OMCI_PM_STATUS_ERROR;
    }

    return pmRet;
}

#endif /* DMP_X_BROADCOM_COM_DNSPROXY_1 */

/*****************************************************************************
*  FUNCTION:  owapi_rut_getBrgFwdMask
*  PURPOSE:   Get linux bridge forwarding mask setting. OMCI looks up the
*             value when creating a new Linux bridge. The configuration is
*             done outside of OMCI. It defines the Linux bridge forwarding
*             mask at /sys/class/net/brx/bridge/group_fwd_mask, based on the
*             following bit map:
*               Bridge Group Address 01-80-C2-00-00-00 - BIT 0
*               (MAC Control) 802.3 01-80-C2-00-00-01 - BIT 1
*               (Link Aggregation) 802.3 01-80-C2-00-00-02 - BIT 2
*               802.1X PAE address 01-80-C2-00-00-03 - BIT 3
*  PARAMETERS:
*      brgFwdMask - linux bridge forwarding mask.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_getBrgFwdMask(UINT32 *brgFwdMask)
{
    CmsRet ret = CMSRET_SUCCESS;

    ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return CMSRET_INTERNAL_ERROR;
    }

    ret = rutOmci_getBrgFwdMask(brgFwdMask);
    cmsLck_releaseLock();

    return ret;
}

/*****************************************************************************
*  FUNCTION:  omciToCmsOidMap
*  PURPOSE:   Find the object id of a copied OMCI object instance in CMS.
*  PARAMETERS:
*      oid - OMCI object id.
*  RETURNS:
*      Object id of the copied instance in CMS.
*  NOTES:
*      None.
*****************************************************************************/
static UINT32 omciToCmsOidMap(UINT32 oid)
{
    UINT32 cmsOid = 0;

    switch (oid)
    {
        case MDMOID_IP_HOST_CONFIG_DATA:
            cmsOid = MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA;
            break;
        case MDMOID_BC_IP_HOST_CONFIG_DATA:
            cmsOid = MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA_EXT;
            break;
        case MDMOID_IPV6_HOST_CONFIG_DATA:
            cmsOid = MDMOID_BCM_OMCI_RTD_IPV6_HOST_CONFIG_DATA;
            break;
        case MDMOID_IPV6_CURRENT_ADDRESS_TABLE:
            cmsOid = MDMOID_BCM_OMCI_RTD_IPV6_CURRENT_ADDRESS_TABLE;
            break;
        case MDMOID_IPV6_CURRENT_DEFAULT_ROUTER_TABLE:
            cmsOid = MDMOID_BCM_OMCI_RTD_IPV6_CURRENT_DEFAULT_ROUTER_TABLE;
            break;
        case MDMOID_IPV6_CURRENT_DNS_TABLE:
            cmsOid = MDMOID_BCM_OMCI_RTD_IPV6_CURRENT_DNS_TABLE;
            break;
        case MDMOID_IPV6_CURRENT_ONLINK_PREFIX_TABLE:
            cmsOid = MDMOID_BCM_OMCI_RTD_IPV6_CURRENT_ONLINK_PREFIX_TABLE;
            break;
        case MDMOID_BC_IPV6_HOST_CONFIG_DATA:
            cmsOid = MDMOID_BCM_OMCI_RTD_IPV6_HOST_CONFIG_DATA_EXT;
            break;
        case MDMOID_TCP_UDP_CONFIG_DATA:
            cmsOid = MDMOID_BCM_OMCI_RTD_TCP_UDP_CONFIG_DATA;
            break;
        default:
            cmsLog_error("Unknown oid=%d", oid);
            break;
    }

    return cmsOid;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_addAutoObject
*  PURPOSE:   Add an auto object instance.
*  PARAMETERS:
*      oid - OMCI object ID (not class ID).
*      meId - OMCI ME ID.
*      persistent - for compatibility with legacy OMCI code, not used.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      addAutoObject, setAutoObject and delAutoObject functions are to create
*      a copy of IP host config data or TCP/UDP config data ME instance in RUT.
*      These two MEs can be closely related to an IP stack or a Gateway
*      software package, and the interactions are put outside of OMCI handler.
*****************************************************************************/
CmsRet owapi_rut_addAutoObject(UINT32 oid, UINT32 meId, UBOOL8 persistent
   __attribute__((unused)))
{
    UINT32 flags = 0;
    void *obj = NULL;
    UINT32 cmsOid = omciToCmsOidMap(oid);
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return ret;
    }

    /* Do nothing if instance is already created. */
    if (rutOmci_isObjectExisted(cmsOid, meId) == TRUE)
    {
        cmsLog_debug("cmsOid=%d, id = %d already exists",
          cmsOid, meId);
        cmsLck_releaseLock();
        return ret;
    }

    /* Add new instance. */
    if ((ret = cmsObj_addInstance(cmsOid, &iidStack)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Could not create cmsOid=%d, meId=%d, ret=%d",
          cmsOid, meId, ret);
        cmsLck_releaseLock();
        return ret;
    }

    cmsLog_debug("New cmsOid=%d, meId=%d, created at %s",
      cmsOid, meId, cmsMdm_dumpIidStack(&iidStack));

    if ((ret = cmsObj_get(cmsOid, &iidStack, flags, (void**)&obj)) !=
      CMSRET_SUCCESS)
    {
        cmsLog_error("Could not get cmsOid=%d, meId=%d, ret=%d",
          cmsOid, meId, ret);
        cmsLck_releaseLock();
        return ret;
    }

    /* MacBridgePortBridgeTableDataObject is used as generic type. */
    ((MacBridgePortBridgeTableDataObject*)obj)->managedEntityId = meId;

    /* Set new instance. */
    if ((ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Could not set cmsOid=%d, meId=%d, ret=%d",
          cmsOid, meId, ret);
    }

    cmsLck_releaseLock();
    cmsObj_free((void**)&obj);
    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_setAutoObject
*  PURPOSE:   Set an auto object instance.
*  PARAMETERS:
*      oid - OMCI object ID (not class ID).
*      meId - OMCI ME ID.
*      omcObj - pointer to the OMCI object.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_setAutoObject(UINT32 oid, UINT32 meId, void *omciObj)
{
    UINT32 cmsOid = omciToCmsOidMap(oid);
    UBOOL8 found = FALSE;
    void *obj = NULL;
    void *dupObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return ret;
    }

    /* Search instance that has matched ME ID. */
    while ((!found) && ((ret = cmsObj_getNext(cmsOid, &iidStack, (void**)&obj))
      == CMSRET_SUCCESS))
    {
        found = (((MacBridgePortBridgeTableDataObject*)obj)->managedEntityId
          == meId);
        /* Free obj if not found. */
        if (found == FALSE)
        {
            cmsObj_free((void**)&obj);
        }
    }

    /* Set the object. */
    if (found == TRUE)
    {
        cmsObj_free((void**)&obj);

        /* Set to CMS OID temporarily. */
        ((MacBridgePortBridgeTableDataObject*)omciObj)->_oid = cmsOid;
        /* Duplicate the OMCI object to CMS object. */
        dupObj = mdm_dupObject(omciObj, ALLOC_ZEROIZE);
        /* Restore OMCI OID. */
        ((MacBridgePortBridgeTableDataObject*)omciObj)->_oid = oid;

        cmsLog_debug("Set cmsOid=%d, meId=%d, at %s",
          cmsOid, meId, cmsMdm_dumpIidStack(&iidStack));
        ret = cmsObj_set(dupObj, &iidStack);
        cmsObj_free((void**)&dupObj);
        if (ret != CMSRET_SUCCESS)
        {
            cmsLog_error("Could not set cmsOid=%d, meId=%d, ret=%d",
              cmsOid, meId, ret);
        }
    }
    else
    {
        cmsLog_error("Could not find cmsOid=%d with meId=%d",
          cmsOid, meId);
        ret = CMSRET_OBJECT_NOT_FOUND;
    }

    cmsLck_releaseLock();
    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_delAutoObject
*  PURPOSE:   Delete an auto object instance.
*  PARAMETERS:
*      oid - OMCI object ID (not class ID).
*      meId - OMCI ME ID.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_delAutoObject(UINT32 oid, UINT32 meId)
{
    UINT32 cmsOid = omciToCmsOidMap(oid);
    UBOOL8 found = FALSE;
    void *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return ret;
    }

    /* Search instance that has matched ME ID. */
    while ((!found) && ((ret = cmsObj_getNext(cmsOid, &iidStack, (void**)&obj))
      == CMSRET_SUCCESS))
    {
        found = (((MacBridgePortBridgeTableDataObject*)obj)->managedEntityId
          == meId);
        if (found == TRUE)
        {
            cmsLog_debug("Delete cmsOid=%d, meId=%d, at %s",
              cmsOid, meId, cmsMdm_dumpIidStack(&iidStack));
            ret = cmsObj_deleteInstance(cmsOid, &iidStack);
        }
        cmsObj_free((void**)&obj);
        INIT_INSTANCE_ID_STACK(&iidStack);
    }

    if (found != TRUE)
    {
        cmsLog_error("Could not find cmsOid=%d with meId=%d",
          cmsOid, meId);
        ret = CMSRET_OBJECT_NOT_FOUND;
    }

    cmsLck_releaseLock();
    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_updateAcsCfg
*  PURPOSE:   Update ACS configuration.
*  PARAMETERS:
*      asc - ACS network address defined in TR-69 management server ME.
*      username - username defined in an Authentication Security Method ME.
*      password - password defined in an Authentication Security Method ME.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_updateAcsCfg(UINT32 ipHostMeId, char *acs, UINT8 *username,
  UINT8 *password)
{
    UBOOL8 foundBcmIpHost = FALSE;
#ifdef DMP_X_BROADCOM_COM_IPV6_1
    BcmOmciRtdIpv6HostConfigDataExtObject *bcmIpv6Host = NULL;
#endif
    BcmOmciRtdIpHostConfigDataExtObject *bcmIpHost = NULL;
    InstanceIdStack iidBcmIpHost = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack iidManServer = EMPTY_INSTANCE_ID_STACK;
    ManagementServerObject *acsCfg = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return ret;
    }

#ifdef DMP_X_BROADCOM_COM_IPV6_1
    // get bound interface name
    while (!foundBcmIpHost &&
      cmsObj_getNextFlags(MDMOID_BCM_OMCI_RTD_IPV6_HOST_CONFIG_DATA_EXT,
      &iidBcmIpHost, OGF_NO_VALUE_UPDATE, (void**)&bcmIpv6Host)
      == CMSRET_SUCCESS)
    {
        foundBcmIpHost = (bcmIpv6Host->managedEntityId == ipHostMeId);
        // update tr069 settings
        if (foundBcmIpHost)
        {
            if (cmsObj_get(MDMOID_MANAGEMENT_SERVER,
              &iidManServer, 0, (void*)&acsCfg) == CMSRET_SUCCESS)
            {
                CMSMEM_REPLACE_STRING_FLAGS(acsCfg->X_BROADCOM_COM_BoundIfName,
                  bcmIpv6Host->interfaceName, 0);
                CMSMEM_REPLACE_STRING_FLAGS(acsCfg->URL, acs, 0);
                CMSMEM_REPLACE_STRING_FLAGS(acsCfg->username,
                  (const char*)username, 0);
                CMSMEM_REPLACE_STRING_FLAGS(acsCfg->password,
                  (const char*)password, 0);
                cmsObj_set(acsCfg, &iidManServer);
                cmsObj_free((void**)&acsCfg);
            }
        }
        cmsObj_free((void**)&bcmIpv6Host);
    }
    INIT_INSTANCE_ID_STACK(&iidBcmIpHost);
#endif    // DMP_X_BROADCOM_COM_IPV6_1

    // get bound interface name
    while (!foundBcmIpHost &&
      cmsObj_getNextFlags(MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA_EXT,
      &iidBcmIpHost, OGF_NO_VALUE_UPDATE, (void**)&bcmIpHost) == CMSRET_SUCCESS)
    {
        foundBcmIpHost = (bcmIpHost->managedEntityId == ipHostMeId);
        // update tr069 settings
        if (foundBcmIpHost)
        {
             if (cmsObj_get(MDMOID_MANAGEMENT_SERVER,
               &iidManServer, 0, (void*)&acsCfg) == CMSRET_SUCCESS)
             {
                CMSMEM_REPLACE_STRING_FLAGS(acsCfg->X_BROADCOM_COM_BoundIfName,
                  bcmIpHost->interfaceName, 0);
                CMSMEM_REPLACE_STRING_FLAGS(acsCfg->URL, acs, 0);
                CMSMEM_REPLACE_STRING_FLAGS(acsCfg->username,
                  (const char*)username, 0);
                CMSMEM_REPLACE_STRING_FLAGS(acsCfg->password,
                  (const char*)password, 0);
                cmsObj_set(acsCfg, &iidManServer);
                cmsObj_free((void**)&acsCfg);
            }
        }
        cmsObj_free((void**)&bcmIpHost);
    }

    cmsLck_releaseLock();
    return ret;
}

/*****************************************************************************
*  FUNCTION:  owapi_rut_getIpv4HostInfo
*  PURPOSE:   Get IPv4 host information. The information may be assigned by
*             DHCP when DHCP is enabled.
*  PARAMETERS:
*      meId - OMCI ME id.
*      hostInfo - pointer to omciIpv4HostInfo.
*  RETURNS:
*      CmsRet.
*  NOTES:
*      None.
*****************************************************************************/
CmsRet owapi_rut_getIpv4HostInfo(UINT32 meId, omciIpv4HostInfo *hostInfo)
{
    UBOOL8 found = FALSE;
    BcmOmciRtdIpHostConfigDataObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 size = 0;
    UINT8 *buf = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(OMCIAPP_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        cmsLck_dumpInfo();
        return ret;
    }

    /* Search instance that has matched ME ID. */
    while ((!found) && ((ret = cmsObj_getNextFlags(
      MDMOID_BCM_OMCI_RTD_IP_HOST_CONFIG_DATA, &iidStack, 0,
      (void**)&obj)) == CMSRET_SUCCESS))
    {
        found = (((BcmOmciRtdIpHostConfigDataObject*)obj)->managedEntityId
          == meId);
        /* Free obj if not found. */
        if (found == FALSE)
        {
            cmsObj_free((void**)&obj);
        }
    }

    if (found == TRUE)
    {
        memset(hostInfo, 0x0, sizeof(omciIpv4HostInfo));
        hostInfo->currentAddress = obj->currentAddress;
        hostInfo->currentMask = obj->currentMask;
        hostInfo->currentGateway = obj->currentGateway;
        hostInfo->currentPrimaryDns = obj->currentPrimaryDns;
        hostInfo->currentSecondaryDns = obj->currentSecondaryDns;

        if (!IS_EMPTY_STRING(obj->domainName))
        {
            cmsUtl_hexStringToBinaryBuf(obj->domainName, &buf, &size);
            memcpy(hostInfo->domainName, buf,
              (size > OWRUT_DOMAIN_NAME_LEN)? OWRUT_DOMAIN_NAME_LEN: size);
            cmsMem_free(buf);
        }
        if (!IS_EMPTY_STRING(obj->hostName))
        {
            cmsUtl_hexStringToBinaryBuf(obj->hostName, &buf, &size);
            memcpy(hostInfo->hostName, buf,
              (size > OWRUT_HOST_NAME_LEN)? OWRUT_HOST_NAME_LEN: size);
            cmsMem_free(buf);
        }

        cmsObj_free((void**)&obj);
    }
    else
    {
        cmsLog_notice("Could not find iphost with meId=%d", meId);
        ret = CMSRET_OBJECT_NOT_FOUND;
    }

    cmsLck_releaseLock();
    return ret;
}
