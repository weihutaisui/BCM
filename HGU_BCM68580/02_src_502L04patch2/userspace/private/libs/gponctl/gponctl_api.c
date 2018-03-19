/*
* <:copyright-BRCM:2007:proprietary:standard
* 
*    Copyright (c) 2007 Broadcom 
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


#include "bcm_OS_Deps_Usr.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "bcmtypes.h"
#include "gponctl_api.h"

/*
 * Macros
 */
#define GPONCTL_IOCTL_PLOAM_FILE_NAME "/dev/bcm_ploam"
#define GPONCTL_IOCTL_OMCI_FILE_NAME  "/dev/bcm_omci"

/*
 * Private functions
 */
static inline int gponCtl_open(const char *filename)
{
    int fd = bcm_dev_open(filename, O_RDWR);

    if (fd < 0)
    {
        GPONCTL_LOG_ERROR("%s: %s", filename, gponCtl_getMsgError(errno));
    }

    return fd;
}

static int gponctl_execCmd(int cmdId  __attribute__((unused)),
  void *info  __attribute__((unused)),
  const char* filename  __attribute__((unused)))
{
#ifdef DESKTOP_LINUX
    //GPONCTL_LOG_INFO("CMD %d, info 0x%p, file: %s\n", cmdId, info, filename);
    return (0);
#else /* DESKTOP_LINUX */
    int ret = GPON_CTL_STATUS_SUCCESS;
    int fd = gponCtl_open(filename);

    if (fd < 0)
    {
        GPONCTL_LOG_ERROR("Failed to bcm_dev_open %s\n", filename);
        ret = GPON_CTL_STATUS_INIT_FAILED;
        goto out;
    }

    ret = bcm_dev_ioctl(fd, cmdId, info);
    if (ret)
    {
        ret = errno;
    }

    bcm_dev_close(fd);

out:
    return ret;
#endif /* DESKTOP_LINUX */
}

static int gponctl_execPloamCmd(int cmdId, void *info)
{
	return gponctl_execCmd(cmdId, info, GPONCTL_IOCTL_PLOAM_FILE_NAME);
}


static int gponctl_execOmciCmd(int cmdId, void *info)
{
	return gponctl_execCmd(cmdId, info, GPONCTL_IOCTL_OMCI_FILE_NAME);
}

/*
 * Public variables
 */
char errMsg[256];

/*
 * Public functions
 */

char *gponCtl_getMsgError(int err)
{
   memset(errMsg, 0, 256);

   switch (err)
   {
       case EFAULT:
           sprintf(errMsg, "EFAULT (%d)", err);
           break;
       case EINVAL:
           sprintf(errMsg, "EINVAL (%d)", err);
           break;
       case ENODATA:
           sprintf(errMsg, "ENODATA (%d)", err);
           break;
       case EINVAL_PLOAM_DUPLICATE:
           sprintf(errMsg, "EINVAL_PLOAM_DUPLICATE (%d)", err);
           break;
       case EINVAL_PLOAM_INIT_OPER_STATE:
           sprintf(errMsg, "EINVAL_PLOAM_INIT_OPER_STATE (%d)", err);
           break;
       case EINVAL_PLOAM_GEM_PORT:
           sprintf(errMsg, "EINVAL_PLOAM_GEM_PORT (%d)", err);
           break;
       case EINVAL_PLOAM_GEM_PORT_ENABLED:
           sprintf(errMsg, "EINVAL_PLOAM_GEM_PORT_ENABLED (%d)", err);
           break;
       case EINVAL_PLOAM_STATE:
           sprintf(errMsg, "EINVAL_PLOAM_STATE (%d)", err);
           break;
       case EINVAL_PLOAM_ARG:
           sprintf(errMsg, "EINVAL_PLOAM_ARG (%d)", err);
           break;
       case EINVAL_PLOAM_NOENT:
           sprintf(errMsg, "EINVAL_PLOAM_NOENT (%d)", err);
           break;
       case EINVAL_PLOAM_BT_OUT_OF_RANGE:
           sprintf(errMsg, "EINVAL_PLOAM_BT_OUT_OF_RANGE (%d)", err);
           break;
       case EINVAL_PLOAM_INTERNAL_ERR:
           sprintf(errMsg, "EINVAL_PLOAM_INTERNAL_ERR (%d)", err);
           break;
       case EINVAL_PLOAM_GEM_MIB_IDX:
           sprintf(errMsg, "EINVAL_PLOAM_GEM_MIB_IDX (%d)", err);
           break;
       case EINVAL_PLOAM_US_QUEUE_MAPPED:
           sprintf(errMsg, "EINVAL_PLOAM_US_QUEUE_MAPPED (%d)", err);
           break;
       case EINVAL_PLOAM_US_QUEUE_IDX:
           sprintf(errMsg, "EINVAL_PLOAM_US_QUEUE_IDX (%d)", err);
           break;
       case EINVAL_PLOAM_US_QUEUE_PRIORITY:
           sprintf(errMsg, "EINVAL_PLOAM_US_QUEUE_PRIORITY (%d)", err);
           break;
       case EINVAL_PLOAM_US_QUEUE_WEIGHT:
           sprintf(errMsg, "EINVAL_PLOAM_US_QUEUE_WEIGHT (%d)", err);
           break;
       case EINVAL_PLOAM_RESOURCE_UNAVAIL:
           sprintf(errMsg, "EINVAL_PLOAM_RESOURCE_UNAVAIL (%d)", err);
           break;
       default:
           sprintf(errMsg, "Unknown error (%d)", err);
           break;
   }

   return errMsg;
}


/*****************************************
 ** gponCtl Event Handling Commands API **
 *****************************************/

int gponCtl_getEventStatus(BCM_Ploam_EventStatusInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_EVENT_STATUS, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_maskEvent(BCM_Ploam_EventMaskInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_MASK_EVENT, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

/*****************************************
 ** gponCtl Alarm Handling Commands API **
 *****************************************/

int gponCtl_getAlarmStatus(BCM_Ploam_AlarmStatusInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_ALARM_STATUS, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_maskAlarm(BCM_Ploam_MaskAlarmInfo *mask)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_MASK_ALARM, mask);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_setSFSDThreshold(BCM_Ploam_SFSDthresholdInfo *threshold)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_SET_SF_SD_THRESHOLD, threshold);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getSFSDThreshold(BCM_Ploam_SFSDthresholdInfo *threshold)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_SF_SD_THRESHOLD, threshold);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}


/*********************************************
 ** gponCtl State Control Commands API **
 *********************************************/

int gponCtl_startAdminState(BCM_Ploam_StartInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_START, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_stopAdminState(BCM_Ploam_StopInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_STOP, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getControlStates(BCM_Ploam_StateInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_STATE, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_setTO1TO2(BCM_Ploam_TO1TO2Info *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_SET_TO1_TO2, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getTO1TO2(BCM_Ploam_TO1TO2Info *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_TO1_TO2, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}


int gponCtl_setMcastEncryptionKeys(BCM_Ploam_McastEncryptionKeysInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_SET_MCAST_ENCRYPTION_KEY, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}



#if defined(CONFIG_BCM_GPON_AE_AUTO_SWITCH)
int gponCtl_disableGponAeAutoSwitch(void)
{
   int ret = GPON_CTL_STATUS_SUCCESS;
   ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_DISABLE_GPON_AE_AUTO_SWITCH, &ret);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getGponAeAutoSwitch(BCM_Ploam_GponAeAutoSwitchInfo *info)
{
   int ret = GPON_CTL_STATUS_SUCCESS;
   ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_GPON_AE_AUTO_SWITCH, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}
#endif /* CONFIG_BCM_GPON_AE_AUTO_SWITCH */

/*********************************************
 ** gponCtl Counters Commands API **
 *********************************************/

int gponCtl_getMessageCounters(BCM_Ploam_MessageCounters *counters)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_MESSAGE_COUNTERS, counters);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getGtcCounters(BCM_Ploam_GtcCounters *counters)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_GTC_COUNTERS, counters);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getFecCounters(BCM_Ploam_fecCounters *counters)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_FEC_COUNTERS, counters);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getGemPortCounters(BCM_Ploam_GemPortCounters *counters)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_GEM_PORT_COUNTERS, counters);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}


/*********************************************
 ** gponCtl GEM Port Provisioning Commands API
 *********************************************/

int gponCtl_configGemPort(BCM_Ploam_CfgGemPortInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_CFG_GEM_PORT, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
#ifdef DESKTOP_LINUX
   info->gemPortIndex = info->gemPortID & 0x3f; 
#endif
   return ret;
}

int gponCtl_configDsGemPortEncryptionByIX(BCM_Ploam_GemPortEncryption *conf)
{
   int ret;

   conf->gemPortId = BCM_PLOAM_GEM_PORT_ID_UNASSIGNED ;

   ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_CFG_DS_GEM_PORT_ENCRYPTION, conf);
   if (ret)
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_configDsGemPortEncryptionByID(BCM_Ploam_GemPortEncryption *conf)
{
   int ret; 

   conf->gemIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED ;

   ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_CFG_DS_GEM_PORT_ENCRYPTION, conf);
   if (ret)
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getTcontCfg(BCM_Ploam_TcontInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_TCONT_CFG, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_configTcontAllocId(BCM_Ploam_TcontAllocIdInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_CFG_TCONT_ALLOCID, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}
int gponCtl_deconfigTcontAllocId(BCM_Ploam_TcontAllocIdInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_DECFG_TCONT_ALLOCID, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}



int gponCtl_deconfigGemPort(BCM_Ploam_DecfgGemPortInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_DECFG_GEM_PORT, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_enableGemPort(BCM_Ploam_EnableGemPortInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_ENABLE_GEM_PORT, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getGemPort(BCM_Ploam_GemPortInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_GEM_PORT_CFG, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
#ifdef DESKTOP_LINUX
   info->gemPortIndex = info->gemPortID & 0x3f; 
#endif
   return ret;
}

int gponCtl_getAllocIds(BCM_Ploam_AllocIDs *allocIds)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_ALLOC_IDS, allocIds);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getOmciPort(BCM_Ploam_OmciPortInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_OMCI_PORT_INFO, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}




/*********************************************
 ** gponCtl GTC Parameters Commands API
 *********************************************/



int gponCtl_getOnuId(BCM_Ploam_GetOnuIdInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_ONU_ID, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_setGemBlockLength(BCM_Ploam_GemBlkLenInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_SET_GEM_BLOCK_LENGTH, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}
int gponCtl_getGemBlockLength(BCM_Ploam_GemBlkLenInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_GEM_BLOCK_LENGTH, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}
int gponCtl_setTodInfo(BCM_Ploam_TimeOfDayInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_SET_TIME_OF_DAY, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}
int gponCtl_getTodInfo(BCM_Ploam_TimeOfDayInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_TIME_OF_DAY, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getFecMode(BCM_Ploam_GetFecModeInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_FEC_MODE, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getEncryptionKey(BCM_Ploam_GetEncryptionKeyInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_ENCRYPTION_KEY, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}




/***************************************************
 ** gponCtl Serial Number & Password Commands API **
 ***************************************************/

int gponCtl_setSerialPasswd(BCM_Ploam_SerialPasswdInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_SET_SERIAL_PASSWD, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getSerialPasswd(BCM_Ploam_SerialPasswdInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_SERIAL_PASSWD, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getPloamDriverVersion(BCM_Gpon_DriverVersionInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_DRIVER_VERSION, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

/****************************************
 ** gponCtl Test Functions API         **
 ****************************************/
int gponCtl_generatePrbsSequence(BCM_Ploam_GenPrbsInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GEN_PRBS, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}



/*********************************************
 ** gponCtl OMCI Commands API **
 *********************************************/

int gponCtl_getOmciCounters(BCM_Omci_Counters *counters)
{
   int ret = gponctl_execOmciCmd(BCM_OMCI_IOC_GET_COUNTERS, counters);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getOmciDriverVersion(BCM_Gpon_DriverVersionInfo *info)
{
   int ret = gponctl_execOmciCmd(BCM_OMCI_IOC_GET_DRIVER_VERSION, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }
   return ret;
}

int gponCtl_getSRIndication(BCM_Ploam_SRIndInfo *info)
{
   int ret = gponctl_execPloamCmd(BCM_PLOAM_IOC_GET_SR_IND, info);
   if ( ret )
   {
      GPONCTL_LOG_ERROR("%s\n", gponCtl_getMsgError(ret));
   }

    return ret;
}
