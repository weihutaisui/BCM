/****************************************************************************
# <:copyright-BRCM:2016:proprietary:standard
# 
#    Copyright (c) 2016 Broadcom 
#    All Rights Reserved
# 
#  This program is the proprietary software of Broadcom and/or its
#  licensors, and may only be used, duplicated, modified or distributed pursuant
#  to the terms and conditions of a separate, written license agreement executed
#  between you and Broadcom (an "Authorized License").  Except as set forth in
#  an Authorized License, Broadcom grants no license (express or implied), right
#  to use, or waiver of any kind with respect to the Software, and Broadcom
#  expressly reserves all rights in and to the Software and all intellectual
#  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
#  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
#  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
# 
#  Except as expressly set forth in the Authorized License,
# 
#  1. This program, including its structure, sequence and organization,
#     constitutes the valuable trade secrets of Broadcom, and you shall use
#     all reasonable efforts to protect the confidentiality thereof, and to
#     use this information only in connection with your use of Broadcom
#     integrated circuit products.
# 
#  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
#     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
#     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
#     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
#     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
#     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
#     PERFORMANCE OF THE SOFTWARE.
# 
#  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
#     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
#     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
#     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
#     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
#     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
#     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
#     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
#     LIMITED REMEDY.
# :>
****************************************************************************
*
*  Filename: dal2_dect.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/

/****************************************************************************
*
*  dal2_dect.c
*
*  PURPOSE: Provide interface to dect configuration in TR104v2 data model.
*
*  NOTES:
*
****************************************************************************/


#ifdef BRCM_VOICE_SUPPORT
#ifdef DMP_VOICE_SERVICE_2

/* ---- Include Files ---------------------------------------------------- */

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_cli.h"
#include "cms_util.h"
#include "cms_qos.h"
#include "cms_net.h"
#include "dal_voice.h"
#include "cms_msg.h"
#include "rut2_voice.h"

#include <mdm.h>


/* Debug: Turn CMD parms dump on/off */
#define DALVOICE_DEBUG_CMD_PARMS    0

/*============================= TODOS =========================*/
/* TODO: Functions needs to change when multiple vp's are supported          */
/* TODO: Functions needs to change when line creatiion/deletion is supported */
/*============================= TODOS =========================*/

/* ---- Public Variables ------------------------------------------------- */
/* ---- Constants and Types ---------------------------------------------- */

#define TEMP_CHARBUF_SIZE        20
#define REGION_A3_CHARBUF_SIZE   4
#define LIST_CHAR_BUFFER         1024
/* Defines for the maximum size of call lists */
#define MAX_SIZE_OUTGOING_CALL_LIST   8
#define MAX_SIZE_INCOMING_CALL_LIST   8
#define MAX_SIZE_MISSED_CALL_LIST     20
#define MAX_CONTACT_NUMBER_INSTANCE    3

#define CHECK_PARMS_AND_RETURN_ON_ERROR(parms, val, length)     \
   do                                                           \
   {                                                            \
      if( parms == NULL || val == NULL || length == 0 )         \
      {                                                         \
         cmsLog_error( "parameter is NULL pointer\n" );         \
         return  CMSRET_INVALID_PARAM_VALUE;                    \
      }                                                         \
   } while( 0 )


/* Size of the maximum TR104 string is 389 characters (for URI).
** We add some margin if we need to allocate memory for any TR104 object
** without knowing which object we deal with. */
#define MAX_TR104_OBJ_SIZE       512

typedef struct
{
   MdmObjectId id;                   /* MDM Object ID */
   int instId;                       /* Instance ID */
   InstanceIdStack iidStack;         /* Instance ID Stack */
} LEVELINFO;

typedef struct
{
   VOICECALLTYPE type;
   const char *txt;
} CallTypeMapping;


/*============================= Helper Function Prototypes ===========================*/

/* Mapping functions */
/* Common helper functions */

/*<START>===================================== DAL MGMT functions =====================================<START>*/

#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1

#ifdef DMP_X_BROADCOM_COM_DECTEEPROM_1
CmsRet dalVoice_SetDectEepromData( DAL_VOICE_PARMS *parms, char * setVal )
{
   return CMSRET_SUCCESS;
}

CmsRet dalVoice_GetDectEepromData( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length )
{
   return CMSRET_SUCCESS;
}
#endif /* DMP_X_BROADCOM_COM_DECTEEPROM_1 */


/***************************************************************************
* Function Name: dalVoice_dectCtlRegWnd
* Description  : Controls (Open or closes) the registration window
*                for DECT support.
*
* Parameters   : open - 1 to open the DECT registration window, 0 to close
*                       the DECT registration window.
*                msgHandleArg - the calling application's message handle
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_dectCtlRegWnd( unsigned int open, void* msgHandleArg )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_dectCtlDelHset
* Description  : Deletes a registered handset from the base station.
*
* Parameters   : hset - the handset index to delete.
*                msgHandleArg - the calling application's message handle
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_dectCtlDelHset( unsigned int hset, void* msgHandleArg )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_dectCtlPingHset
* Description  : Pings a registered handset (i.e. apply short ring on the
*                handset to locate it).
*
* Parameters   : hset - the handset index to ping.
*                msgHandleArg - the calling application's message handle
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_dectCtlPingHset( unsigned int hset, void* msgHandleArg )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectRegWnd
* Description  : Gets information from the DECT module about registration
*                window setup.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectRegWnd(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_SetDectRegWnd
* Description  : Sets information from the DECT module about registration
*                window setup.
*
* Parameters   : parms
*                value - value to set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectRegWnd(DAL_VOICE_PARMS *parms, unsigned int value)
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_GetDectStatus
* Description  : Gets information about whether the DECT module was
*                initialized properly or not.
*
* Parameters   : parms->op[1] - get flags, either OGF_NORMAL_UPDATE or
*                               OGF_NO_VALUE_UPDATE
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectStatus(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectStatus
* Description  : Sets information from the DECT module about whether the
*                module was initialized properly or not.
*
* Parameters   : parms
*                value - value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectStatus(DAL_VOICE_PARMS *parms, unsigned value)
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_GetDectAc
* Description  : Gets information from the DECT module the access code used
*                for handset registration.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectAc(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_SetDectAc
* Description  : Sets information to the DECT module the access code used
*                for handset registration.
*
*                This function is used to send a command to the DECT
*                module in order to set (update) the access code that is
*                known there.  Once we get confirmation from the DECT
*                module that the code has been updated successfully, the
*                DECT support framework will update the information in
*                MDM flash as will if appropriate.
*
* Parameters   : parms
*                value - value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectAc( DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_SetDectAcPersist
* Description  : Sets the DECT access code information into persistent
*                storage (the MDM).
*
*                This function is used by the DECT support framework to
*                save information about the access code to flash once we
*                received confirmation from the DECT module that attempt to
*                update the access code has been successful.
*
* Parameters   : parms
*                value - value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectAcPersist( DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_GetDectFwVer
* Description  : Gets information from the DECT module about the firmware
*                version being used.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectFwVer(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectInfo
* Description  : Gets information from the DECT module 
*
* Parameters   : parms
*                value - returned value information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectInfo(DAL_VOICE_PARMS *parms, DAL_VOICE_DECT_INFO *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectLnk
* Description  : Gets information from the DECT module about the firmware
*                link date being used.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectLnk(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_SetDectLnk
* Description  : Sets information from the DECT module about the firmware
*                link date being used.
*
* Parameters   : parms
*                value - value to set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectLnk(DAL_VOICE_PARMS *parms, char *value)
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_GetDectType
* Description  : Gets information from the DECT module about the firmware
*                type being used.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectType(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_SetDectType
* Description  : Sets information from the DECT module about the firmware
*                type being used.
*
* Parameters   : parms
*                value - value to set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectType(DAL_VOICE_PARMS *parms, unsigned int value)
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_GetDectId
* Description  : Gets information from the DECT module about the unique
*                identifier.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectId(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_SetDectId
* Description  : Sets information from the DECT module about the unique
*                identifier.
*
* Parameters   : parms
*                value - value to set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectId(DAL_VOICE_PARMS *parms, char *value)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectManic
* Description  : Gets information from the DECT module about the
*                manufacturer identifier of the DECT device.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectManic(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_SetDectManic
* Description  : Sets information from the DECT module about the
*                manufacturer identifier of the DECT device.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectManic(DAL_VOICE_PARMS *parms, unsigned int value)
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_GetDectModic
* Description  : Gets information from the DECT module about the modele
*                identifier of the DECT module.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectModic(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_SetDectModic
* Description  : Sets information from the DECT module about the
*                model identifier of the DECT device.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectModic(DAL_VOICE_PARMS *parms, unsigned int value)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHsetInstanceList
* Description  : Obtains DECT handset instane list
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHsetInstanceList(DAL_VOICE_PARMS *parms, unsigned int *total, unsigned int *list )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectCurHsetList
* Description  : Gets the current number of handset registered with the
*                DECT module.
*
* Parameters   : parms
*                value - returned value information
*                list  - list of registered handset id
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectCurHsetList(DAL_VOICE_PARMS *parms, unsigned int *total, unsigned int *list )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectCurHset
* Description  : Gets the current number of handset registered with the
*                DECT module.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectCurHset(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectMaxHset
* Description  : Gets the maximum number of handset to be registered with the
*                DECT module.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectMaxHset(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHsStatus
* Description  : Gets handset status for the handset associated with the
*                DECT module.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHsStatus(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHsManic
* Description  : Gets information from the DECT module about the
*                manufacturer identifier of the handset registered.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHsManic(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHsModic
* Description  : Gets information from the DECT module about the modele
*                identifier of the handset registered.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHsModic(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHsIpei
* Description  : Gets information from the DECT module about the IPEI
*                of the handset registered.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHsIpei(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHsSubTime
* Description  : Gets information from the DECT module about the subscription
*                time of the handset registered.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHsSubTime(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHandsetObj
* Description  : Gets information from the DECT module about the subscription
*                time of the handset registered.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHandsetObj(DAL_VOICE_PARMS *parms, DECTPortableObject **obj, InstanceIdStack *iidStack)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetCallInterception
* Description  : Gets information from the DECT module about call interception
*                support on this handset.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHsCallInterception(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHsId
* Description  : Gets information from the DECT module about the
*                internal identifier of the handset registered.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHsId(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHsetName
* Description  : Get registered dect handset name
*
* Parameters   : (INPUT) parms->op[0] - voice service instance
*                (INPUT) parms->op[1] - handset instance
*                (INPUT) value - name information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHsetName(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectHsetName
* Description  : Set registered dect handset name
*
* Parameters   : (INPUT) parms->op[0] - voice service instance
*                (INPUT) parms->op[1] - instance id
*                (INPUT) value - name information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectHsetName(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectHsCallInterception
* Description  : sets dect HS call interception status
*
* Parameters   : parms
*                value - returned value information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectHsCallInterception(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectHsIpuiPersist
* Description  : Sets the DECT handset IPUI information into persistent
*                storage (the MDM).
*
*                This function is used by the DECT support framework to
*                save information about the handset IPUI to flash once we
*                received confirmation from the DECT module about the IPUI
*                of the registered handset(s).
*
* Parameters   : (INPUT) parms->op[0] voice service instance id, alway be 1
*                (INPUT) parms->op[1] handset Id
*                (INPUT) value - value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectHsIpuiPersist(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_GetMaxHset
* Description  : Gets the maximum number of handset that the MDM software
*                allow the DECT module to support, note this is different
*                from the dalVoice_GetDectMaxHset API which is used to
*                query from the DECT module directly how many handset can
*                be supported at once.  Ideally those two values should be
*                in synch.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS - Always.
****************************************************************************/
CmsRet dalVoice_GetMaxHset(DAL_VOICE_PARMS *parms, int *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectResetBase
*
* Description  : Gets Dect Reset base value from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectResetBase(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectClockMaster
*
* Description  : Gets Dect clock master value from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectClockMaster(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectFwVersion
*
* Description  : Gets Dect Firmware version from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectFwVersion(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHwVersion
*
* Description  : Gets Dect Hardware version from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHwVersion(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectIpAddrType
*
* Description  : Dummy function
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectIpAddrType(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectIpAddress
*
* Description  : Dummy function
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectIpAddress(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectIpSubnetMask
*
* Description  : Dummy function
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectIpSubnetMask(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectIpGateway
*
* Description  : Dummy function
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectIpGateway(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectIpDns
*
* Description  : Dummy function
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectIpDns(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectEmissionMode
*
* Description  : Dummy function
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectEmissionMode(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectNewPinCode
*
* Description  : Dummy function
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectNewPinCode(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_GetDectRomVersion
*
* Description  : Gets Dect EEPROM version from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectRomVersion(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectPinCode
*
* Description  : Gets Pin code from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectPinCode(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHandsetName
*
* Description  : Gets internal name from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - handset instance Id
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHandsetName(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectHandsetNumber
*
* Description  : Gets handset id from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - handset instance Id
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectHandsetNumber(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}
/***************************************************************************
* Function Name: dalVoice_GetDectContactListName
*
* Description  : Gets Dect Reset base value from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - contact list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectContactListName(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectContactListFirstName
*
* Description  : Gets Dect Reset base value from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - contact list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectContactListFirstName(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectContactListMelody
*
* Description  : Gets Dect Reset base value from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - contact list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectContactListMelody(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectContactListLineId
*
* Description  : Gets Dect Reset base value from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - contact list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectContactListLineId(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectContactListNumber
*
* Description  : Gets Dect contact number from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - contact list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectContactListNumber(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetVoiceCallNumber
*
* Description  : Gets voice call number from call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - call list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetVoiceCallNumber(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetVoiceCallName
*
* Description  : Gets calling party name from call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - call list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetVoiceCallName(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetVoiceCallLineName
*
* Description  : Gets line name from call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - call list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetVoiceCallLineName(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetVoiceCallLineId
*
* Description  : Gets line Id from call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - call list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetVoiceCallLineId(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetVoiceCallNewFlag
*
* Description  : Gets line Id from call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - call list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetVoiceCallNewFlag(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetVoiceCallDateTime
*
* Description  : Gets call date and time from call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - call list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetVoiceCallDateTime(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetVoiceCallType
*
* Description  : Gets call type from call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - call list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetVoiceCallType(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetVoiceNumberOfMissedCalls
*
* Description  : Gets call type from call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - call list instance
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetVoiceNumberOfMissedCalls(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectTotalInternalName
*
* Description  : Gets total number of internal name from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - total number of instances
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectTotalInternalName(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectTotalContacts
*
* Description  : Gets total number of contacts from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - total number of instances
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectTotalContacts(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectTotalSystemSettings
*
* Description  : Gets total number of system settings from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - total number of instances
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectTotalSystemSettings(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetDectTotalLineSettings
*
* Description  : Gets total number of system settings from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - total number of instances
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectTotalLineSettings(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetTotalVoiceCalls
*
* Description  : Gets total number of voice calls from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - total number of instances
*                (OUTPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetTotalVoiceCalls(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetTotalVoiceCallsWithType
*
* Description  : Gets total number of typed voice calls from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - voice line id (optional)
*                (OUTPUT) value - total number of instances
*                (OUTPUT) list -  returned instance id list
*                (INPUT)  type -  voice call type
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetTotalVoiceCallsWithType(DAL_VOICE_PARMS *parms, unsigned int *value, char *type, unsigned int *list)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetTotalMissedVoiceCalls
*
* Description  : Gets total number of missed voice calls from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - voice line id (optional)
*                (OUTPUT) value - total number of instances
*                (OUTPUT) list -  returned instance id list
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetTotalMissedVoiceCalls(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetTotalIncomingVoiceCalls
*
* Description  : Gets total number of incoming voice calls from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - voice line id (optional)
*                (OUTPUT) value - total number of instances
*                (OUTPUT) list -  returned instance id list
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetTotalIncomingVoiceCalls(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetTotalOutgoingVoiceCalls
*
* Description  : Gets total number of outgoing voice calls from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - voice line id (optional)
*                (OUTPUT) value - total number of instances
*                (OUTPUT) list -  returned instance id list
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetTotalOutgoingVoiceCalls(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetTotalAllIncomingVoiceCalls
*
* Description  : Gets total number of all incoming voice calls from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - voice line id (optional)
*                (OUTPUT) value - total number of instances
*                (OUTPUT) list -  returned instance id list
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetTotalAllIncomingVoiceCalls(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetVoiceCallListStats
*
* Description  : Gets total numbers of each voice call type from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) stats - stats block to fill in
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetVoiceCallListStats(DAL_VOICE_PARMS *parms, DAL_VOICE_DECT_CALL_LIST_STATS *stats)
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetLineName
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   name - Dect Line name
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetLineName(DAL_VOICE_PARMS *parms, char *Name, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDectLineId
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   name - Dect Line name
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetDectLineId(DAL_VOICE_PARMS *parms, char *line, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDectRegisteredHsList
**
**  PUROPOSE:
**
**  INPUT PARMS:    parms->op[0] - voice service instance
**
**  OUTPUT PARMS:   list - list of registered handset id, seperated by ','
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetDectRegisteredHsList(DAL_VOICE_PARMS *parms, char *list, unsigned int length )
{
   return CMSRET_SUCCESS;
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetLineAttachedHandsetMask
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   mask - handset mask, each handset present as 1 bit in UINT32 value
**                  from LSB to MSB 
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetLineAttachedHandsetMask(DAL_VOICE_PARMS *parms, UINT32 *mask)
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetTotalNumberOfAttachedHs
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   num - total number of handsets that attached to the line
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetTotalNumberOfAttachedHs(DAL_VOICE_PARMS *parms, char *num, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetLineAttachedHandsetList
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   list - list of Dect Handset Id, seperate by ","
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetLineAttachedHandsetList(DAL_VOICE_PARMS *parms, char *list, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDectLineMelody
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   melody - Dect Handset Melody
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetLineMelody(DAL_VOICE_PARMS *parms, char *melody, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDectLineMultiCallMode
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   mode - Dect Line call mode, "single" or "mutliple"
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetLineMultiCallMode(DAL_VOICE_PARMS *parms, char *mode, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetLineIntrusionCallMode
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   mode - Dect Line intrustion call mode, "enable" or "disable"
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetLineIntrusionCallMode(DAL_VOICE_PARMS *parms, char *mode, unsigned int length )
{
   return CMSRET_SUCCESS;
}

#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */

/***************************************************************************
* Function Name: dalVoice_GetDectSupport
* Description  : Checks whether DECT is supported as part of this voice
*                application or not.
*
* Parameters   : parms
*                enabled - returned value information, will be '1' is DECT
*                          is supported on this voice application, or will
*                          be '0' otherwise.
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetDectSupport(DAL_VOICE_PARMS *parms, char *enabled, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDectLoggingLevel
**
**  PUROPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   mdm log level for voice
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetDectLoggingLevel( DAL_VOICE_PARMS *parms, void * getVal, unsigned int length)
{
   return CMSRET_SUCCESS;
}

/*<END>===================================== DAL MGMT functions =====================================<END>*/

/*<START>===================================== CLI DAL get functions =====================================<START>*/
#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1



#endif  /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */
/*<END>===================================== CLI DAL get functions =======================================<END>*/

/*<START>===================================== DAL Set functions =====================================<START>*/

#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1

/*****************************************************************
**  FUNCTION:       dalVoice_SetDectLineName
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   name - Dect Line name
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_SetDectLineName(DAL_VOICE_PARMS *parms, char *Name )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetDectLineId
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   name - Dect Line name
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_SetDectLineId(DAL_VOICE_PARMS *parms, char *line )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetDectLineMelody
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   melody - Dect Handset Melody
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_SetLineMelody(DAL_VOICE_PARMS *parms, char *melody )
{
   return CMSRET_SUCCESS;
}


/*****************************************************************
**  FUNCTION:       dalVoice_SetLineMultiCallMode
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   mode - Dect Line call mode, "single" or "mutliple"
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_SetLineMultiCallMode(DAL_VOICE_PARMS *parms, char *mode )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetLineIntrusionCall
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   mode - Dect Line intrustion call mode, "enable" or "disable"
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_SetLineIntrusionCall(DAL_VOICE_PARMS *parms, char *mode )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetLineAttachedHandsetList
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**                  list - Dect Handset List
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_SetLineAttachedHandsetList(DAL_VOICE_PARMS *parms, char *list)
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetDectLoggingLevel
**
**  PUROPOSE:
**
**  INPUT PARMS:    setVal - log mdm log level for dectd
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetDectLoggingLevel(  DAL_VOICE_PARMS *parms, char * setVal )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectClockMaster
* Description  : Gets information from the DECT module about the modele
*                identifier of the handset registered.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectClockMaster(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectResetBase
* Description  : Gets information from the DECT module about the modele
*                identifier of the handset registered.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectResetBase(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectPinCode
* Description  : Gets information from the DECT module about the modele
*                identifier of the handset registered.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectPinCode(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectFwVersion
* Description  : Gets information from the DECT module about the modele
*                identifier of the handset registered.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectFwVersion(DAL_VOICE_PARMS *parms, char *value)
{
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_SetVoiceMissedCalls
*
* Description  : Set calling party name in call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - list object instance
*                (INPUT)  value - returned value information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVoiceMissedCalls(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetVoiceCallName
*
* Description  : Set calling party name in call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - list object instance
*                (INPUT)  value - returned value information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVoiceCallName(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetVoiceCallNumber
*
* Description  : Set calling party number in call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - list object instance
*                (INPUT)  value - returned value information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVoiceCallNumber(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectCallLineName
*
* Description  : Set calling party number in call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - list object instance
*                (INPUT)  value - returned value information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVoiceCallLineName(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectCallLineId
*
* Description  : Set calling party number in call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - list object instance
*                (INPUT)  value - returned value information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVoiceCallLineId(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetVoiceCallCallType
*
* Description  : Set calling party number in call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - list object instance
*                (INPUT)  value - returned value information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVoiceCallType(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetVoiceCallDateTime
*
* Description  : Set calling party number in call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - list object instance
*                (INPUT)  value - returned value information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVoiceCallDateTime(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetVoiceCallNewFlag
*
* Description  : Set calling party number in call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - list object instance
*                (INPUT)  value - returned value information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVoiceCallNewFlag(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectContactListName
*
* Description  : Gets Dect Reset base value from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - list object instance
*                (INPUT)  value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectContactListName(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectContactListFirstName
*
* Description  : Gets Dect Reset base value from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - contact list instance
*                (INPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectContactListFirstName(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectContactListMelody
*
* Description  : Gets Dect Reset base value from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - contact list instance
*                (INPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectContactListMelody(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectContactListLineId
*
* Description  : Gets Dect Reset base value from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - contact list instance
*                (INPUT) value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectContactListLineId(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectContactListNumber
*
* Description  : Gets Dect contact number from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - contact list instance
*                (INPUT)  value - returned value information
*                (INPUT)  length - size of the returned information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectContactListNumber(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetDectHandsetName
*
* Description  : Set dect internal terminal id
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - handsetId
*                (INPUT)  value - returned value information
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDectHandsetName(DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_AddVoiceCallListEntry
*
* Description  : Adds an entry to DECT call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  name, number, date/time, lineName - call-specific parameters
*                (INPUT)  lineID - line IDof the DECT handset
*                (INPUT)  callType - internal or external
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_AddVoiceCallListEntry(DAL_VOICE_PARMS *parms, char *name, char *number, char *dateTime, char *lineName, int lineId, char *callType)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_MapDectLineToCmLine
*
* Description  : Maps DECT line to call manager line
*
* Parameters   : dectline - DECT line to be mapped
*                cmLine - returned call manager line
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_MapDectLineToCmLine( int dectline, int *cmline )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_MapCmLineToDectLine
*
* Description  : Maps call manager line to DECT line
*
* Parameters   : cmline - call manager line to be mapped
*                cmLine - returned DECT line
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_MapCmLineToDectLine( int cmline, int *dectline )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetLineSettingId
*
* Description  : Obtains line setting ID
*
* Parameters   : 
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetLineSettingId( DAL_VOICE_PARMS *parms, UINT32 *lineId, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_GetLineSettingName
*
* Description  : Obtains line setting name
*
* Parameters   : 
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetLineSettingName( DAL_VOICE_PARMS *parms, char *lineName, unsigned int length )
{
   return CMSRET_SUCCESS;
}

#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */

/*<END>===================================== DAL Set functions =======================================<END>*/

/*<START>===================================== Set Helper Functions ======================================<START>*/


/*<END>===================================== Set Helper Functions ========================================<END>*/

/*<START>================================= Get Helper Functions ==========================================<START>*/
/*<END>================================= Get Helper Functions ============================================<END>*/


/*<START>================================= DAL Add Functions ==========================================<START>*/
#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1


/***************************************************************************
* Function Name: dalVoice_AddVoiceCallInstance
*
* Description  : Add instance into call list in MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - last added instance id
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_AddVoiceCallInstance(DAL_VOICE_PARMS *parms, unsigned int *value)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_AddVoiceContactInstance
*
* Description  : Gets total number of contacts from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (OUTPUT) value - last added instance id
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_AddVoiceContactInstance(DAL_VOICE_PARMS *parms, unsigned int *value)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_AddHandset
* Description  : add one registered handset instance in MDM
*
* Parameters   : (INPUT) handsetId - handset Id to be added
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_AddHandset(int handsetId)
{
   return CMSRET_SUCCESS;
}



#endif /*  DMP_X_BROADCOM_COM_DECTENDPOINT_1 */
/*<END>================================= DAL Add Functions ============================================<END>*/

/*<START>================================= DAL Delete Functions ==========================================<START>*/
#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1

/***************************************************************************
* Function Name: dalVoice_DeleteVoiceCallInstance
*
* Description  : Delete one instance from call list
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - list object instance
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_DeleteVoiceCallInstance(DAL_VOICE_PARMS *parms)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_DeleteVoiceContactInstance
*
* Description  : Gets total number of contacts from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - contact list instance
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_DeleteVoiceContactInstance(DAL_VOICE_PARMS *parms)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_DeleteDectHandsetNameInstance
*
* Description  : Gets total number of contacts from MDM
*
* Parameters   : (INPUT)  parms->op[0] - voice service instance
*                (INPUT)  parms->op[1] - dect handset instance id
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_DeleteDectHandsetNameInstance(DAL_VOICE_PARMS *parms)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_DelHandset
* Description  : delete one registered handset instance in MDM
*
* Parameters   : (INPUT) handsetId - handset Id to be deleted
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_DelHandset(int handsetId)
{
   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_DelAllHandset
* Description  : delete all registered handset instance in MDM
*
* Parameters   : none
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_DelAllHandset( void )
{
   return CMSRET_SUCCESS;
}


#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */
/*<END>================================= DAL Delete Functions ============================================<END>*/


/*<START>================================= Common Helper Functions =======================================<START>*/
#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1
CmsRet dalVoice_mapIPEIToHandsetId(char *ipei, int *handsetId)
{
   (void)ipei;
   (void)handsetId;

   return CMSRET_SUCCESS;
}

CmsRet dalVoice_mapHandsetIdToIPEI(int handsetId, char *ipei)
{
   (void)ipei;
   (void)handsetId;

   return CMSRET_SUCCESS;
}

CmsRet dalVoice_mapHsetId2Instance(int handsetId, int *inst)
{
   return CMSRET_SUCCESS;
}

#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */
/*<END>================================= Common Helper Functions =========================================<END>*/
#endif /* DMP_VOICE_SERVICE_2 */
#endif /* BRCM_VOICE_SUPPORT */
