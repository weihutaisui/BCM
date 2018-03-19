/****************************************************************************
# <:copyright-BRCM:2011:proprietary:standard
# 
#    Copyright (c) 2011 Broadcom 
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
*  Filename: dal_dect.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/

/****************************************************************************
*
*  dal_dect.c
*
*  PURPOSE: Provide interface to the DAL functions related to dect configuration.
*
*  NOTES:
*
****************************************************************************/


#ifdef BRCM_VOICE_SUPPORT

/* ---- Include Files ---------------------------------------------------- */

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_cli.h"
#include "cms_util.h"
#include "cms_qos.h"
#include "cms_net.h"
#include "dal_voice.h"
#include "cms_msg.h"
#include "rut_voice.h"

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

#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1
static CallTypeMapping map[] =
{
   { MISSED,   MDMVS_MISSED   },
   { INCOMING, MDMVS_INCOMING },
   { OUTGOING, MDMVS_OUTGOING }
};
#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */

/*============================= Helper Function Prototypes ===========================*/

/* Mapping functions */
/* Common helper functions */
#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1
static CmsRet getObj(LEVELINFO info[], InstanceIdStack *iidStack, UINT32 flags, void **obj);
static CmsRet getDectSystemObjectElement(DAL_VOICE_PARMS *parms, void *value, unsigned int length );
static CmsRet getDectSystemObject(DAL_VOICE_PARMS *parms, InstanceIdStack *iidStack, void **obj );
static CmsRet setDectSystemObjectElement(DAL_VOICE_PARMS *parms, void *value );
static CmsRet getDectListObjectElement(DAL_VOICE_PARMS *parms, void *value, unsigned int length );
static CmsRet getDectListObjectInstance(DAL_VOICE_PARMS *parms, InstanceIdStack *iidStack, void **obj );
static CmsRet getDectTotalNumberOfInstancesInList(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list );
static CmsRet addDectListObjectInstance(DAL_VOICE_PARMS *parms, InstanceIdStack *iidStack );
static CmsRet setDectListObjectElement(DAL_VOICE_PARMS *parms, void *value );
static CmsRet delDectListObjectInstance(DAL_VOICE_PARMS *parms, InstanceIdStack *iidStack );
static CmsRet dalVoice_TrimCallLists();
static CmsRet mapCallType2Txt(VOICECALLTYPE calltype, char *txt);
static VOICECALLTYPE mapCallTxt2Type(char* txt);
#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */

/*<START>===================================== DAL MGMT functions =====================================<START>*/

#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1

#ifdef DMP_X_BROADCOM_COM_DECTEEPROM_1
CmsRet dalVoice_SetDectEepromData( DAL_VOICE_PARMS *parms, char * setVal )
{
   VoiceObject *obj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   /* get and set the variable */

   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "could not get voice object, ret=%d", ret);
   }
   else
   {
      /* set the new value in local copy, after freeing old memory */
      cmsMem_free( obj->X_BROADCOM_COM_DectEepromData );
      obj->X_BROADCOM_COM_DectEepromData =  cmsMem_strdup( setVal );

         /* copy new value from local copy to MDM */
      if ( ( ret = cmsObj_set( obj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set DECT eeprom data ret = %d", ret);
      }
   }
   return ret;
}

CmsRet dalVoice_GetDectEepromData( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length )
{
   VoiceObject *obj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   /* get and set the variable */

   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "could not get voice object, ret=%d", ret);
   }
   else
   {
      if ( obj->X_BROADCOM_COM_DectEepromData != NULL )
      {
         strncpy( getVal, obj->X_BROADCOM_COM_DectEepromData, length );
      }
      else
      {
         cmsLog_error( "obj->X_BROADCOM_COM_DectEepromData is null");
         sprintf( getVal,"undefined" );
      }
   }

   cmsObj_free((void **) &obj);

   return ret;
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
   CmsMsgHeader msgHdr = EMPTY_MSG_HEADER;
   CmsRet ret;

   cmsLog_debug( "Voice - %s DECT Registration Window",
                 ( open ? "Open" : "Close" ) );

   msgHdr.dst = EID_DECT;
   msgHdr.src = cmsMsg_getHandleEid(msgHandleArg);
   msgHdr.type = ( open ? CMS_MSG_VOICE_DECT_OPEN_REG_WND :
                          CMS_MSG_VOICE_DECT_CLOSE_REG_WND );
   msgHdr.flags_event = 1;

   if(msgHandleArg == NULL)
   {
      cmsLog_error("msgHandleArg is NULL!");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else if ( (ret = cmsMsg_send(msgHandleArg, &msgHdr)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Could not send %s msg to voice, ret=%d",
                    ( open ? "CMS_MSG_VOICE_DECT_OPEN_REG_WND" :
                             "CMS_MSG_VOICE_DECT_CLOSE_REG_WND" ),
                    ret );
   }

   return ret;
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
   CmsMsgHeader msgHdr = EMPTY_MSG_HEADER;
   CmsRet ret;

   cmsLog_debug( "Voice - Delete Handset %d from DECT Module",
                 hset );

   msgHdr.dst = EID_DECT;
   msgHdr.src = cmsMsg_getHandleEid(msgHandleArg);
   msgHdr.type = CMS_MSG_VOICE_DECT_HS_DELETE;
   msgHdr.flags_event = 1;
   msgHdr.wordData = hset;

   if(msgHandleArg == NULL)
   {
      cmsLog_error("msgHandleArg is NULL!");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else if ( (ret = cmsMsg_send(msgHandleArg, &msgHdr)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Could not send CMS_MSG_VOICE_DECT_HS_DELETE msg to voice, ret=%d",
                    ret );
   }

   return ret;
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
   CmsMsgHeader msgHdr = EMPTY_MSG_HEADER;
   CmsRet ret;

   cmsLog_debug( "Voice - Ping Handset %d registered with DECT Module",
                 hset );

   msgHdr.dst = EID_DECT;
   msgHdr.src = cmsMsg_getHandleEid(msgHandleArg);
   msgHdr.type = CMS_MSG_VOICE_DECT_HS_PING;
   msgHdr.flags_event = 1;
   msgHdr.wordData = hset;

   if(msgHandleArg == NULL)
   {
      cmsLog_error("msgHandleArg is NULL!");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else if ( (ret = cmsMsg_send(msgHandleArg, &msgHdr)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Could not send CMS_MSG_VOICE_DECT_HS_PING msg to voice, ret=%d",
                    ret );
   }

   return ret;
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Get the VoiceDectSystemSettingObject object - Note that we are using 'OGF_NORMAL_UPDATE' because
   ** the information is kept in the kernel (DECT module) and we want to ensure we query the
   ** most up to date information.
   */
   if ( (ret = getObj(info, &iidStack, parms->op[1], (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   snprintf( (char*)value,
             length,
             "%s",
             dectObj->waitingSubscription ? "Open" : "Closed" );

   cmsObj_free( (void **) &dectObj );

   return  ( ret );
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Retrieve the current data.
   */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   dectObj->waitingSubscription = value ? 1 : 0;
   if( (ret = cmsObj_set(dectObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Unable to set DECT Reg Window status (ret %d)\n", ret );
   }

   cmsObj_free( (void **) &dectObj );

   return ret;
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
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Get the VoiceDectSystemSettingObject object - Note that we are using 'OGF_NORMAL_UPDATE' because
   ** the information is kept in the kernel (DECT module) and we want to ensure we query the
   ** most up to date information.
   */
   if ( (ret = getObj(info, &iidStack, parms->op[1], (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   snprintf( (char*)value,
             length,
             "%s",
             dectObj->X_BROADCOM_COM_ServiceEnabled ? "Enabled" : "Disabled" );

   cmsObj_free( (void **) &dectObj );

   return  ( ret );
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Retrieve the current data.
   */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   dectObj->X_BROADCOM_COM_ServiceEnabled = value;
   if( (ret = cmsObj_set(dectObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Unable to set DECT Status (ret %d)\n", ret );
   }

   cmsObj_free( (void **) &dectObj );

   return ret;
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
   /* This setting has now moved to LAS */
   return dalVoice_GetDectPinCode(parms, value, length);
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
   CmsMsgHeader *msg;
   CmsRet ret;
   char *data;
   void *msgBuf;

   cmsLog_debug( "Voice - Set DECT Access Code (%s)", value );

   if(parms->msgHandle == NULL)
   {
      cmsLog_error("parms->msgHandle is NULL!");
      return CMSRET_INVALID_ARGUMENTS;
   }

   if( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader) +
                               (strlen(value) + 1),
                               ALLOC_ZEROIZE)) == NULL)
   {
      return CMSRET_INTERNAL_ERROR;
   }

   msg  = (CmsMsgHeader *) msgBuf;
   data = (char *) (msg + 1);

   msg->dst = EID_DECT;
   msg->src = cmsMsg_getHandleEid(parms->msgHandle);
   msg->type = CMS_MSG_VOICE_DECT_AC_SET;
   msg->flags_event = 1;
   msg->dataLength = strlen( value );

   strncpy( data, value, strlen(value) );

   if ( (ret = cmsMsg_send(parms->msgHandle, msg)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Could not send CMS_MSG_VOICE_DECT_AC_SET msg to dectd, ret=%d", ret );
   }

   return ret;
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
   /* This setting has now moved to LAS */
   return dalVoice_SetDectPinCode(parms, value);
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
   /* This setting has now moved to LAS */
   return dalVoice_GetDectFwVersion(parms, value, length);
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
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Get the VoiceDectSystemSettingObject object - Note that we are using 'OGF_NORMAL_UPDATE' because
   ** the information is kept in the kernel (DECT module) and we want to ensure we query the
   ** most up to date information.
   */
   if ( (ret = getObj(info, &iidStack, parms->op[1], (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   value->serviceEnabled = dectObj->X_BROADCOM_COM_ServiceEnabled;
   if (dectObj->X_BROADCOM_COM_LinkDate) 
   {
      strcpy(value->linkDate, dectObj->X_BROADCOM_COM_LinkDate);
   }
   value->type = dectObj->X_BROADCOM_COM_Type;
   if (dectObj->X_BROADCOM_COM_DectId)
   {
      strcpy(value->dectId, dectObj->X_BROADCOM_COM_DectId);
   }
   value->manic = dectObj->X_BROADCOM_COM_MANIC;
   value->modic = dectObj->X_BROADCOM_COM_MODIC;
   value->maxNumberOfHandsets = dectObj->X_BROADCOM_COM_MaxNumberOfHandsets;
   value->waitingSubscription = dectObj->waitingSubscription;
   if (dectObj->X_BROADCOM_COM_AccessCode)
   {
      strcpy(value->accessCode, dectObj->X_BROADCOM_COM_AccessCode);
   }

   cmsObj_free( (void **) &dectObj );

   return  ( ret );
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Get the VoiceDectSystemSettingObject object - Note that we are using 'OGF_NORMAL_UPDATE' because
   ** the information is kept in the kernel (DECT module) and we want to ensure we query the
   ** most up to date information.
   */
   if ( (ret = getObj(info, &iidStack, parms->op[1], (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   snprintf( (char*)value,
             length,
             "%s",
             dectObj->X_BROADCOM_COM_LinkDate );

   cmsObj_free( (void **) &dectObj );

   return  ( ret );
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Retrieve the current data.
   */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   REPLACE_STRING_IF_NOT_EQUAL(dectObj->X_BROADCOM_COM_LinkDate, value);
   if( (ret = cmsObj_set(dectObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Unable to set DECT link date (ret %d)\n", ret );
   }

   cmsObj_free( (void **) &dectObj );

   return ret;
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Get the VoiceDectSystemSettingObject object - Note that we are using 'OGF_NORMAL_UPDATE' because
   ** the information is kept in the kernel (DECT module) and we want to ensure we query the
   ** most up to date information.
   */
   if ( (ret = getObj(info, &iidStack, parms->op[1], (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   snprintf( (char*)value,
             length,
             "%s",
             (dectObj->X_BROADCOM_COM_Type == 0x00) ? "Euro" :
                ((dectObj->X_BROADCOM_COM_Type == 0x01) ? "US" : "Invalid") );

   cmsObj_free( (void **) &dectObj );

   return  ( ret );
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Retrieve the current data.
   */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   dectObj->X_BROADCOM_COM_Type = value;
   if( (ret = cmsObj_set(dectObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Unable to set DECT Type (ret %d)\n", ret );
   }

   cmsObj_free( (void **) &dectObj );

   return ret;
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Get the VoiceDectSystemSettingObject object - Note that we are using 'OGF_NORMAL_UPDATE' because
   ** the information is kept in the kernel (DECT module) and we want to ensure we query the
   ** most up to date information.
   */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   snprintf( (char*)value,
             length,
             "%s",
             dectObj->X_BROADCOM_COM_DectId );

   cmsObj_free( (void **) &dectObj );

   return  ( ret );
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Retrieve the current data.
   */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   REPLACE_STRING_IF_NOT_EQUAL(dectObj->X_BROADCOM_COM_DectId, value);
   if( (ret = cmsObj_set(dectObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Unable to set DECT id (ret %d)\n", ret );
   }

   cmsObj_free( (void **) &dectObj );

   return ret;
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Get the VoiceDectSystemSettingObject object - Note that we are using 'OGF_NORMAL_UPDATE' because
   ** the information is kept in the kernel (DECT module) and we want to ensure we query the
   ** most up to date information.
   */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   snprintf( (char*)value,
             length,
             "0x%8X",
             dectObj->X_BROADCOM_COM_MANIC );

   cmsObj_free( (void **) &dectObj );

   return  ( ret );
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Retrieve the current data.
   */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   dectObj->X_BROADCOM_COM_MANIC = value;
   if( (ret = cmsObj_set(dectObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Unable to set DECT MANIC (ret %d)\n", ret );
   }

   cmsObj_free( (void **) &dectObj );

   return ret;
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Get the VoiceDectSystemSettingObject object - Note that we are using 'OGF_NORMAL_UPDATE' because
   ** the information is kept in the kernel (DECT module) and we want to ensure we query the
   ** most up to date information.
   */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   snprintf( (char*)value,
             length,
             "0x%8X",
             dectObj->X_BROADCOM_COM_MODIC );

   cmsObj_free( (void **) &dectObj );

   return  ( ret );
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
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceDectSystemSettingObject *dectObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, -1,  EMPTY_INSTANCE_ID_STACK},
                                                {0,                0,   EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Retrieve the current data.
   */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&dectObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   dectObj->X_BROADCOM_COM_MODIC = value;
   if( (ret = cmsObj_set(dectObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Unable to set DECT MODIC (ret %d)\n", ret );
   }

   cmsObj_free( (void **) &dectObj );

   return ret;
}


CmsRet dalVoice_GetDectHsetInstanceList(DAL_VOICE_PARMS *parms, unsigned int *total, unsigned int *list )
{
   int               i = 0;
   void             *obj;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;

   if(parms == NULL || total == NULL)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* Traverse objects until match found, or getting next object is not successful */
   while (cmsObj_getNextFlags ( MDMOID_DECT_HANDSET, &iidStack, OGF_NO_VALUE_UPDATE, &obj ) == CMSRET_SUCCESS )
   {
      if(list)
      {
         list[i] = PEEK_INSTANCE_ID(&iidStack);
      }
      i++;
      /* free up unwanted object */
      cmsObj_free( &obj );
   }

   (*total) = i;

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
   int               i = 0;
   void             *obj;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;

   if(parms == NULL || list == NULL || total == NULL)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* Traverse objects until match found, or getting next object is not successful */
   while (cmsObj_getNextFlags ( MDMOID_DECT_HANDSET, &iidStack, OGF_NO_VALUE_UPDATE, &obj ) == CMSRET_SUCCESS )
   {
      list[i] = ((_VoiceDECTHandsetObject *)obj)->X_BROADCOM_COM_ID;
      i++;
      /* free up unwanted object */
      cmsObj_free( &obj );
   }

   (*total) = i;

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
   int   currHset;

   rutVoice_getCurrDectHset( &currHset );
   /* Done with the query...  Just set the final result.
   */
   snprintf( (char*)value,
             length,
             "%d",
             currHset );

   return  CMSRET_SUCCESS;
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
   CmsRet              ret;
   int                 max;

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Get the VoiceDectSystemSettingObject object - Note that we are using 'OGF_NORMAL_UPDATE' because
   ** the information is kept in the kernel (DECT module) and we want to ensure we query the
   ** most up to date information.
   */
   if ( (ret = dalVoice_GetMaxHset(parms, &max )) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve DECT object\n" );
      return ( ret );
   }

   snprintf( (char*)value,
             length,
             "%d",
             max );

   return  ( ret );
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
   CmsRet                 ret = CMSRET_INTERNAL_ERROR;
   VoiceDECTHandsetObject *dectObj   = NULL;
   int                    inst       = ANY_INSTANCE;

   /* Note that since we only can support one DECT interface, we can directly query the handset
   ** object associated with this interface without having to create an additional info level
   ** for the DECT interface itself.
   */
   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_DECT_HANDSET, inst, EMPTY_INSTANCE_ID_STACK},
                                                {0,                   0,        EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d, DECT handset: %d\n", parms->op[0], parms->op[1] );

   if(dalVoice_mapHsetId2Instance(parms->op[1], &inst) == CMSRET_SUCCESS && inst != ANY_INSTANCE )
   {
      /* Create the instance identifier stack for this CMS object request.
      */
      INIT_INSTANCE_ID_STACK( &info[0].iidStack );
      PUSH_INSTANCE_ID( &info[0].iidStack, parms->op[0] );  /* Voice Service Object
                                                            ** (always the same).
                                                            */
      PUSH_INSTANCE_ID( &info[0].iidStack, inst );      /* Handset instance. */

      /* Optimize the query of the object instance to just get the one we want since
      ** we are querying information into the DECT module (kernel space) and we are doing
      ** so asynchronously (blocking), in such case, we want to limit the traffic to the
      ** minimum necessary, which means not going through the 'getObj' function which
      ** would potentially go through all the possible MDMOID_DECT_HANDSET objects instances
      ** in order to query the one we want.
      */
      ret = cmsObj_get( info[0].id,
                        &info[0].iidStack,
                        OGF_NORMAL_UPDATE,
                        (void**)&dectObj );

      if ( ret == CMSRET_SUCCESS )
      {
         snprintf( (char*)value,
                   length,
                   "%s",
                   dectObj->status );

         cmsObj_free( (void **) &dectObj );
      }
      else
      {
         cmsLog_error( "Can't retrieve DECT handset %d object\n", inst );
      }
   }

   return  ( ret );
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
   CmsRet                 ret        = CMSRET_INTERNAL_ERROR;
   VoiceDECTHandsetObject *dectObj   = NULL;
   int                    inst = ANY_INSTANCE;

   /* Note that since we only can support one DECT interface, we can directly query the handset
   ** object associated with this interface without having to create an additional info level
   ** for the DECT interface itself.
   */
   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_DECT_HANDSET, inst, EMPTY_INSTANCE_ID_STACK},
                                                {0,                   0,        EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d, DECT handset: %d\n", parms->op[0], parms->op[1] );

   if(dalVoice_mapHsetId2Instance(parms->op[1], &inst) == CMSRET_SUCCESS && inst != ANY_INSTANCE )
   {
      /* Create the instance identifier stack for this CMS object request.
      */
      INIT_INSTANCE_ID_STACK( &info[0].iidStack );
      PUSH_INSTANCE_ID( &info[0].iidStack, parms->op[0] );  /* Voice Service Object
                                                            ** (always the same).
                                                            */
      PUSH_INSTANCE_ID( &info[0].iidStack, inst );      /* Handset instance. */

      /* Optimize the query of the object instance to just get the one we want since
      ** we are querying information into the DECT module (kernel space) and we are doing
      ** so asynchronously (blocking), in such case, we want to limit the traffic to the
      ** minimum necessary, which means not going through the 'getObj' function which
      ** would potentially go through all the possible MDMOID_DECT_HANDSET objects instances
      ** in order to query the one we want.
      */
      ret = cmsObj_get( info[0].id,
                        &info[0].iidStack,
                        OGF_NO_VALUE_UPDATE,
                        (void**)&dectObj );

      if ( ret == CMSRET_SUCCESS )
      {
         snprintf( (char*)value,
                   length,
                   "0x%8X",
                   dectObj->X_BROADCOM_COM_MANIC );

         cmsObj_free( (void **) &dectObj );
      }
      else
      {
         cmsLog_error( "Can't retrieve DECT handset %d object\n", inst );
      }
   }

   return  ( ret );
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
   CmsRet                 ret        = CMSRET_INTERNAL_ERROR;
   VoiceDECTHandsetObject *dectObj   = NULL;
   int                    inst       = ANY_INSTANCE;

   /* Note that since we only can support one DECT interface, we can directly query the handset
   ** object associated with this interface without having to create an additional info level
   ** for the DECT interface itself.
   */
   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_DECT_HANDSET, inst, EMPTY_INSTANCE_ID_STACK},
                                                {0,                   0,        EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d, DECT handset: %d\n", parms->op[0], parms->op[1] );

   if(dalVoice_mapHsetId2Instance(parms->op[1], &inst) == CMSRET_SUCCESS && inst != ANY_INSTANCE )
   {
      /* Create the instance identifier stack for this CMS object request.
      */
      INIT_INSTANCE_ID_STACK( &info[0].iidStack );
      PUSH_INSTANCE_ID( &info[0].iidStack, parms->op[0] );  /* Voice Service Object
                                                            ** (always the same).
                                                            */
      PUSH_INSTANCE_ID( &info[0].iidStack, inst );      /* Handset instance. */

      /* Optimize the query of the object instance to just get the one we want since
      ** we are querying information into the DECT module (kernel space) and we are doing
      ** so asynchronously (blocking), in such case, we want to limit the traffic to the
      ** minimum necessary, which means not going through the 'getObj' function which
      ** would potentially go through all the possible MDMOID_DECT_HANDSET objects instances
      ** in order to query the one we want.
      */
      ret = cmsObj_get( info[0].id,
                        &info[0].iidStack,
                        OGF_NO_VALUE_UPDATE,
                        (void**)&dectObj );

      if ( ret == CMSRET_SUCCESS )
      {
         snprintf( (char*)value,
                   length,
                   "0x%8X",
                   dectObj->X_BROADCOM_COM_MODIC );

         cmsObj_free( (void **) &dectObj );
      }
      else
      {
         cmsLog_error( "Can't retrieve DECT handset %d object\n", inst );
      }
   }

   return  ( ret );
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
   CmsRet                 ret = CMSRET_INTERNAL_ERROR;
   VoiceDECTHandsetObject *dectObj   = NULL;
   int                    inst       = ANY_INSTANCE;

   /* Note that since we only can support one DECT interface, we can directly query the handset
   ** object associated with this interface without having to create an additional info level
   ** for the DECT interface itself.
   */
   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_DECT_HANDSET, inst,     EMPTY_INSTANCE_ID_STACK},
                                                {0,                   0,            EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d, DECT handset: %d\n", parms->op[0], parms->op[1] );

   if(dalVoice_mapHsetId2Instance(parms->op[1], &inst) == CMSRET_SUCCESS && inst != ANY_INSTANCE )
   {
      /* Create the instance identifier stack for this CMS object request.
      */
      INIT_INSTANCE_ID_STACK( &info[0].iidStack );
      PUSH_INSTANCE_ID( &info[0].iidStack, parms->op[0] );  /* Voice Service Object
                                                            ** (always the same).
                                                            */
      PUSH_INSTANCE_ID( &info[0].iidStack, inst );      /* Handset instance. */

      /* Optimize the query of the object instance to just get the one we want since
      ** we are querying information into the DECT module (kernel space) and we are doing
      ** so asynchronously (blocking), in such case, we want to limit the traffic to the
      ** minimum necessary, which means not going through the 'getObj' function which
      ** would potentially go through all the possible MDMOID_DECT_HANDSET objects instances
      ** in order to query the one we want.
      **
      ** Note that we are using 'OGF_NO_VALUE_UPDATE' because the IPEI is saved in the
      ** persistent flash under MDM control and this is where we want to read the value
      ** from for accuracy.
      */
      ret = cmsObj_get( info[0].id,
                        &info[0].iidStack,
                        OGF_NO_VALUE_UPDATE,
                        (void**)&dectObj );

      if ( ret == CMSRET_SUCCESS )
      {
         /* Note that we are using the IPUI which is the shadow read/write variable
         ** which is used because the IPEI is a TR-104 read-only defined variable, so we
         ** cannot use that one for saving the information in the flash as it would have
         ** to be read/write instead which would break the TR-104 requirement.
         */
         snprintf( (char*)value,
                   length,
                   "%s",
                   dectObj->X_BROADCOM_COM_IPUI );

         cmsObj_free( (void **) &dectObj );
      }
      else
      {
         cmsLog_error( "Can't retrieve DECT handset %d object\n", inst );
      }
   }

   return  ( ret );
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
   CmsRet                 ret = CMSRET_INTERNAL_ERROR;
   VoiceDECTHandsetObject *dectObj   = NULL;
   int           inst       = ANY_INSTANCE;

   /* Note that since we only can support one DECT interface, we can directly query the handset
   ** object associated with this interface without having to create an additional info level
   ** for the DECT interface itself.
   */
   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_DECT_HANDSET, inst, EMPTY_INSTANCE_ID_STACK},
                                                {0,                   0,        EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( "DECT instance: %d, DECT handset: %d\n", parms->op[0], parms->op[1] );

   if(dalVoice_mapHsetId2Instance(parms->op[1], &inst) == CMSRET_SUCCESS && inst != ANY_INSTANCE )
   {
      /* Create the instance identifier stack for this CMS object request.
      */
      INIT_INSTANCE_ID_STACK( &info[0].iidStack );
      PUSH_INSTANCE_ID( &info[0].iidStack, parms->op[0] );  /* Voice Service Object
                                                            ** (always the same).
                                                            */
      PUSH_INSTANCE_ID( &info[0].iidStack, inst );      /* Handset instance. */

      /* Optimize the query of the object instance to just get the one we want since
      ** we are querying information into the DECT module (kernel space) and we are doing
      ** so asynchronously (blocking), in such case, we want to limit the traffic to the
      ** minimum necessary, which means not going through the 'getObj' function which
      ** would potentially go through all the possible MDMOID_DECT_HANDSET objects instances
      ** in order to query the one we want.
      */
      ret = cmsObj_get( info[0].id,
                        &info[0].iidStack,
                        OGF_NO_VALUE_UPDATE,
                        (void**)&dectObj );

      if ( ret == CMSRET_SUCCESS )
      {
         snprintf( (char*)value,
                   length,
                   "%s",
                   dectObj->subscriptionTime );

         cmsObj_free( (void **) &dectObj );
      }
      else
      {
         cmsLog_error( "Can't retrieve DECT handset %d object\n", inst );
      }
   }

   return  ( ret );
}

CmsRet dalVoice_GetDectHandsetObj(DAL_VOICE_PARMS *parms, VoiceDECTHandsetObject **obj, InstanceIdStack *iidStack)
{
   CmsRet                 ret = CMSRET_SUCCESS;

   cmsLog_debug( "%s DECT handset instance: (%d)\n", __FUNCTION__, parms->op[1] );

   INIT_INSTANCE_ID_STACK( iidStack );
   PUSH_INSTANCE_ID( iidStack, parms->op[0] );  /* Voice Service Object instance  */
   PUSH_INSTANCE_ID( iidStack, parms->op[1] );  /* Handset instance. */

   ret = cmsObj_get( MDMOID_DECT_HANDSET, iidStack, OGF_NO_VALUE_UPDATE, (void**)obj);
   if(ret != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve DECT handset instance (%d), return (%d)\n", parms->op[1], ret );
   }

   return ret;
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
   CmsRet                 ret = CMSRET_INTERNAL_ERROR;
   VoiceDECTHandsetObject *dectObj   = NULL;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   ret = dalVoice_GetDectHandsetObj(parms, &dectObj, &iidStack);
   if ( ret == CMSRET_SUCCESS && dectObj != NULL)
   {
      snprintf( (char*)value, length, "%s", dectObj->X_BROADCOM_COM_Call_Interception );

      cmsObj_free( (void **) &dectObj );
   }

   return  ( ret );
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
   int  inst = ANY_INSTANCE;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   /* check whether the instance already exist */
   if(dalVoice_mapHsetId2Instance(parms->op[1], &inst) == CMSRET_SUCCESS && inst != ANY_INSTANCE )
   {
      parms->op[1] = inst;
      return(dalVoice_GetDectHandsetNumber(parms, value, length));
   }
   else
      cmsLog_error( "%s Can't get Handset instance (%d) \n", __FUNCTION__, parms->op[1]);

   return  CMSRET_INVALID_PARAM_VALUE;
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
   int  inst = ANY_INSTANCE;

   /* check whether the instance already exist */
   if(dalVoice_mapHsetId2Instance(parms->op[1], &inst) == CMSRET_SUCCESS && inst != ANY_INSTANCE )
   {
      parms->op[1] = inst;
      return(dalVoice_GetDectHandsetName(parms, value, length));
   }
   else
      cmsLog_error( "%s Can't get Handset instance (%d) \n", __FUNCTION__, parms->op[1]);

   return  CMSRET_INVALID_PARAM_VALUE;
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
   CmsRet           ret = CMSRET_INTERNAL_ERROR;
   int              inst = ANY_INSTANCE;

   /* check whether the instance already exist */
   ret = dalVoice_mapHsetId2Instance(parms->op[1], &inst);
   if( ret == CMSRET_SUCCESS && inst != ANY_INSTANCE )
   {
      parms->op[1] = inst;
      return( dalVoice_SetDectHandsetName(parms, value));
   }

   return ret;
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
   CmsRet                 ret;
   VoiceDECTHandsetObject *dectObj   = NULL;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, strlen(value));

   ret = dalVoice_GetDectHandsetObj(parms, &dectObj, &iidStack );
   if( ret == CMSRET_SUCCESS && dectObj != NULL)
   {
      REPLACE_STRING_IF_NOT_EQUAL(dectObj->X_BROADCOM_COM_Call_Interception, value);

      ret = cmsObj_set( dectObj, &iidStack );
      if ( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "%s Can't set Handset instance (%d) intercept ret = %d\n", __FUNCTION__, parms->op[1], ret);
      }

      cmsObj_free( (void **) &dectObj );
   }

   return  ( ret );
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
   CmsRet                  ret;
   VoiceDECTHandsetObject *dectObj   = NULL;
   int                     hsetInst   = 0;
   int                     handsetId  = 0;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);


   handsetId = parms->op[1];

   /* Note that since we only can support one DECT interface, we can directly query the handset
   ** object associated with this interface without having to create an additional info level
   ** for the DECT interface itself.
   */
   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_DECT_HANDSET, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0,                   0,        EMPTY_INSTANCE_ID_STACK},
                                              };

   cmsLog_debug( " DECT handset id: %d\n", handsetId );

   /* Create the instance identifier stack for this CMS object request.
   */
   INIT_INSTANCE_ID_STACK( &info[0].iidStack );
   PUSH_INSTANCE_ID( &info[0].iidStack,  parms->op[0] );  /* Voice Service Object (always 1). */

   ret = dalVoice_mapHsetId2Instance(handsetId, &hsetInst );
   if( ret != CMSRET_SUCCESS )
   {
      cmsLog_error( "DECT handset: %d didn't exist\n", handsetId );
      return CMSRET_INTERNAL_ERROR;
   }

   PUSH_INSTANCE_ID( &info[0].iidStack, hsetInst );      /* Handset instance. */

   /* Optimize the query of the object instance to just get the one we want since
   ** we are querying information into the DECT module (kernel space) and we are doing
   ** so asynchronously (blocking), in such case, we want to limit the traffic to the
   ** minimum necessary, which means not going through the 'getObj' function which
   ** would potentially go through all the possible MDMOID_DECT_HANDSET objects instances
   ** in order to query the one we want.
   */
   ret = cmsObj_get( info[0].id,
                     &info[0].iidStack,
                     OGF_NO_VALUE_UPDATE,
                     (void**)&dectObj );

   if ( ret == CMSRET_SUCCESS )
   {
      /* Check whether there is a need to update.
      */

      /* Only update IPUI when valid value is specified, otherwise leave it as null */
      if( cmsUtl_strcmp(  DAL_VOICE_DECT_INVALID_HSET, (char *)value ) )
      {
         REPLACE_STRING_IF_NOT_EQUAL(dectObj->X_BROADCOM_COM_IPUI, value);
      }

      if ( ( ret = cmsObj_set( dectObj, &info[0].iidStack )) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set Handset %d IPUI ret = %d\n", hsetInst, ret);
      }

      cmsObj_free((void **) &dectObj);
   }
   else
   {
      cmsLog_error( "DECT DECT handset: %d didn't exist\n", handsetId );
   }

   return ret;
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
   rutVoice_getMaxDectHset( value );

   /* Always successful.
   */
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   parms->op[1] = MDMOID_VOICE_DECT_RESET_BASE;

   return ( getDectSystemObjectElement(parms, (void *)value, length ));
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   parms->op[1] = MDMOID_VOICE_DECT_CLOCK_MASTER;

   return ( getDectSystemObjectElement(parms, (void *)value, length ));
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   parms->op[1] = MDMOID_VOICE_DECT_FIRMWARE_VERSION;

   return ( getDectSystemObjectElement(parms, (void *)value, length ));
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   parms->op[1] = MDMOID_VOICE_DECT_HARDWARE_VERSION;

   return ( getDectSystemObjectElement(parms, (void *)value, length ));
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   sprintf(value, " ");
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   sprintf(value, " ");
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   sprintf(value, " ");
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   sprintf(value, " ");
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   sprintf(value, " ");
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   sprintf(value, " ");
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   sprintf(value, " ");
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   parms->op[1] = MDMOID_EEPROM_VERSION;

   return ( getDectSystemObjectElement(parms, (void *)value, length ));
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   parms->op[1] = MDMOID_VOICE_DECT_PIN_CODE;

   return ( getDectSystemObjectElement(parms, (void *)value, length ));
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
   CmsRet                 ret;
   VoiceDECTHandsetObject *dectObj   = NULL;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   ret = dalVoice_GetDectHandsetObj( parms, &dectObj, &iidStack);
   if ( ret == CMSRET_SUCCESS && dectObj != NULL)
   {
      snprintf( (char*)value, length, "%s", dectObj->X_BROADCOM_COM_Name);

      cmsObj_free( (void **) &dectObj );
   }

   return  ( ret );
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
   CmsRet                 ret;
   VoiceDECTHandsetObject *dectObj   = NULL;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   ret = dalVoice_GetDectHandsetObj( parms, &dectObj, &iidStack);
   if ( ret == CMSRET_SUCCESS && dectObj != NULL)
   {
      snprintf( (char*)value, length, "%d", dectObj->X_BROADCOM_COM_ID);
      cmsLog_debug( " DECT handset id: %d\n", dectObj->X_BROADCOM_COM_ID );
      cmsObj_free( (void **) &dectObj );
   }
   else
      cmsLog_error( "%s Can't get Handset instance (%d) ret = %d\n", __FUNCTION__, parms->op[1], ret);

   return  ( ret );
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CONTACT_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CONTACT_NAME;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CONTACT_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CONTACT_FIRST_NAME;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CONTACT_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CONTACT_MELODY;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CONTACT_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CONTACT_LINE_ID;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CONTACT_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CONTACT_NUMBER;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_NUMBER;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_NAME;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_LINE_NAME;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_LINE_ID;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   DAL_VOICE_PARMS  local_parms;
   CmsRet ret;
   CmsRet temp_ret;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_NEW_FLAG;
   local_parms.op[4] = 0;

   ret = getDectListObjectElement(&local_parms, (void *)value, length );

   if(ret == CMSRET_SUCCESS)
   {
      if(!cmsUtl_strcasecmp(value, MDMVS_YES))
      {
         temp_ret = dalVoice_SetVoiceCallNewFlag(parms, MDMVS_NO);
         if(temp_ret != CMSRET_SUCCESS)
         {
            cmsLog_error( "%s Set call flag failed", __FUNCTION__);
            return temp_ret;
         }
      }
   }
   else
   {
      cmsLog_error( "%s Can't get call flag", __FUNCTION__);
   }

   return ret;
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_DATE_TIME;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_TYPE;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, length);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_NUMBER_OF_MISSED_CALLS;
   local_parms.op[4] = 0;

   return ( getDectListObjectElement(&local_parms, (void *)value, length ));
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   parms->op[1] = MDMOID_DECT_HANDSET;

   return ( getDectTotalNumberOfInstancesInList(parms, value, list ));
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   parms->op[1] = MDMOID_VOICE_CONTACT_LIST;

   return ( getDectTotalNumberOfInstancesInList(parms, value, list ));
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   parms->op[1] = MDMOID_VOICE_DECT_SYSTEM_SETTING;

   return ( getDectTotalNumberOfInstancesInList(parms, value, list ));
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   parms->op[1] = MDMOID_VOICE_LINE_SETTING;

   return ( getDectTotalNumberOfInstancesInList(parms, value, list ));
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   parms->op[1] = MDMOID_VOICE_CALL_LIST;

   return ( getDectTotalNumberOfInstancesInList(parms, value, list ));
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
   CmsRet            ret;
   unsigned int      total, i, j;
   unsigned int      lineId;
   unsigned int      lengthOfCallType = strlen(type);
   char              callType[20];
   unsigned int      iidlist[DAL_MAX_INSTANCE_IN_LIST];

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   lineId = parms->op[1];

   ret = dalVoice_GetTotalVoiceCalls(parms, &total, iidlist);

   if(ret != CMSRET_SUCCESS )
   {
      return ret;
   }

   if(lengthOfCallType == strlen(MDMVS_MISSED) ||
      lengthOfCallType == strlen(MDMVS_INCOMING) ||
      lengthOfCallType == strlen(MDMVS_OUTGOING))
   {
      for( i=0, j=0; i< total; i++ )
      {
         parms->op[1] = iidlist[i];

         memset(callType, 0, sizeof(callType));
         ret = dalVoice_GetVoiceCallType(parms, callType, sizeof(callType));
         if(ret == CMSRET_SUCCESS)
         {
            if(!cmsUtl_strcasecmp(callType, type))
            {
               int   callLineId = 0;

               /* lineId == 0 indicates get calls related to all lines */
               if( lineId != 0 )
               {
                  char  callLineIdStr[20];
                  const char type_delim[2] = ":";
                  char *p = NULL;
                  char *saveptr;

                  dalVoice_GetVoiceCallLineId(parms, callLineIdStr, sizeof(callLineIdStr));
                  /* get line number */
                  p = strtok_r((char *)callLineIdStr, type_delim, &saveptr);
                  if(p != NULL)
                  {
                    callLineId = atoi(p);
                  }
               }

               if( lineId == callLineId )
               {
                  if(list)
                  {
                     list[j] = iidlist[i];
                  }
                  j++;
               }
            }
         }
      }
   }
   else
   {
      char *callType_MISSED = strtok(type, ",");
      if(callType_MISSED == NULL)
      {
         cmsLog_error( "%s failed getting call type for generating the list\n", __FUNCTION__);
         return CMSRET_INTERNAL_ERROR;
      }

      char *callType_INCOMING_ACCEPTED = strtok(NULL, "\0");
      if(callType_INCOMING_ACCEPTED == NULL)
      {
         cmsLog_error( "%s failed getting call type for generating the list\n", __FUNCTION__);
         return CMSRET_INTERNAL_ERROR;
      }

      for( i=0, j=0; i< total; i++ )
      {
         parms->op[1] = iidlist[i];

         memset(callType, 0, sizeof(callType));
         ret = dalVoice_GetVoiceCallType(parms, callType, sizeof(callType));
         if(ret == CMSRET_SUCCESS)
         {
            if(!cmsUtl_strcasecmp(callType, callType_MISSED) || !cmsUtl_strcasecmp(callType, callType_INCOMING_ACCEPTED))
            {
               int   callLineId = 0;

               if( lineId != 0 )
               {
                  char  callLineIdStr[20];
                  const char type_delim[2] = ":";
                  char *p = NULL;
                  char *saveptr;

                  dalVoice_GetVoiceCallLineId(parms, callLineIdStr, sizeof(callLineIdStr));
                  /* get line number */
                  p = strtok_r((char *)callLineIdStr, type_delim, &saveptr);
                  if(p != NULL)
                  {
                    callLineId = atoi(p);
                  }
               }

               if( lineId == callLineId )
               {
                  if(list)
                  {
                     list[j] = iidlist[i];
                  }
                  j++;
               }
            }
         }
      }
   }

   *value = j;
   return ret;
}

CmsRet dalVoice_GetTotalMissedVoiceCalls(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   char type[20] = MDMVS_MISSED;

   return(dalVoice_GetTotalVoiceCallsWithType(parms, value, type, list));
}

CmsRet dalVoice_GetTotalIncomingVoiceCalls(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   char type[20] = MDMVS_INCOMING;

   return(dalVoice_GetTotalVoiceCallsWithType(parms, value, type, list));
}

CmsRet dalVoice_GetTotalOutgoingVoiceCalls(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   char type[20] = MDMVS_OUTGOING;

   return(dalVoice_GetTotalVoiceCallsWithType(parms, value, type, list));
}

CmsRet dalVoice_GetTotalAllIncomingVoiceCalls(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list)
{
   char type[20] = "";
   strcpy(type, MDMVS_MISSED);
   strcat(type, ",");
   strcat(type, MDMVS_INCOMING);

   return(dalVoice_GetTotalVoiceCallsWithType(parms, value, type, list));
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
   CmsRet                   ret;
   InstanceIdStack          iidStack = EMPTY_INSTANCE_ID_STACK;
   DAL_VOICE_PARMS          localParms;
   void                    *obj = NULL;
   _VoiceCallTypeObject    *typeObj = NULL;
   _VoiceCallNewFlagObject *newFlgObj = NULL;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, stats, 1);

   cmsLog_debug( "Line: %d\n", parms->op[1]);

   while( cmsObj_getNextFlags( MDMOID_VOICE_CALL_LIST, &iidStack, OGF_NO_VALUE_UPDATE, &obj ) == CMSRET_SUCCESS )
   {
      InstanceIdStack lineIidStack = EMPTY_INSTANCE_ID_STACK;
      InstanceIdStack typeIidStack = EMPTY_INSTANCE_ID_STACK;
      /* We don't actually care about the call list object itself, so free it now */
      cmsObj_free(&obj);

      if(parms->op[1] != -1)
      {
         memset(&localParms, 0, sizeof(localParms));
         localParms.op[0] = parms->op[0];
         localParms.op[1] = MDMOID_VOICE_CALL_LIST;
         localParms.op[2] = PEEK_INSTANCE_ID(&iidStack);
         localParms.op[3] = MDMOID_VOICE_CALL_LINE_ID;

         ret = getDectListObjectInstance(&localParms, &lineIidStack, (void**)&obj);
         if(ret == CMSRET_SUCCESS)
         {
            /* If the call's line id doesn't match the requested list line id, then skip it */
            if(((_VoiceCallLineIdObject*)obj)->element != parms->op[1])
            {
               cmsObj_free(&obj);
               continue;
            }
            cmsObj_free(&obj);
         }
         else
         {
            cmsLog_error("%s: Unable to get line id");
            continue;
         }
      }

      memset(&localParms, 0, sizeof(localParms));
      localParms.op[0] = parms->op[0];
      localParms.op[1] = MDMOID_VOICE_CALL_LIST;
      localParms.op[2] = PEEK_INSTANCE_ID(&iidStack);
      localParms.op[3] = MDMOID_VOICE_CALL_TYPE;

      /* Get the list object instance's type and new flag */
      ret = getDectListObjectInstance(&localParms, &typeIidStack, (void**)&typeObj);
      if(ret == CMSRET_SUCCESS)
      {
         switch(typeObj->element)
         {
            case INCOMING:
            {
               stats->numIncoming++;
            }
            break;

            case OUTGOING:
            {
               stats->numOutgoing++;
            }
            break;

            case MISSED:
            {
               InstanceIdStack newFlgIidStack = EMPTY_INSTANCE_ID_STACK;
               stats->numMissed++;

               /* Check if the missed call is unread */
               memset(&localParms, 0, sizeof(localParms));

               localParms.op[0] = parms->op[0];
               localParms.op[1] = MDMOID_VOICE_CALL_LIST;
               localParms.op[2] = PEEK_INSTANCE_ID(&iidStack);
               localParms.op[3] = MDMOID_VOICE_CALL_NEW_FLAG;

               ret = getDectListObjectInstance(&localParms, &newFlgIidStack, (void**)&newFlgObj);
               if(ret == CMSRET_SUCCESS)
               {
                  if(newFlgObj->element)
                  {
                     stats->numUnreadMissed++;
                  }
                  cmsObj_free((void**)&newFlgObj);
               }
               else
               {
                  cmsLog_error("%s: Couldn't get unread flag for call %d", __FUNCTION__, localParms.op[2]);
               }
            }
            break;

            default:
            {
               cmsLog_error("%s: Unknown call type %d", __FUNCTION__, typeObj->element);
            }
            break;
         }
         cmsObj_free((void**)&typeObj);
      }
      else
      {
         cmsLog_error("%s: Couldn't get call type.", __FUNCTION__);
      }
   }

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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineNameObject *nameObj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                             {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                             {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                             {MDMOID_VOICE_LINE_NAME, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                             {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, Name, length);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d", parms->op[1] );

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&nameObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line name object\n" );
      return ( ret );
   }

   /* Copy auth username value */
   if( nameObj->element != NULL )
   {
      cmsLog_debug( "dect line name = %s", nameObj->element);
      strncpy( (char *) Name, nameObj->element, length);
   }

   cmsObj_free( (void **) &nameObj );

   return ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineDectLineIdObject *obj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_DECT_LINE_ID, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, line, length);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d\n",info[1].instId);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line id object\n" );
      return ( ret );
   }

   /* Copy line id value */
   if( obj != NULL )
   {
      cmsLog_debug( "dect line id = %u\n ", obj->element);
      snprintf( (char*)line, length, "%u",obj->element );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
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
   CmsRet              ret;
   int                 absMax;
   int                 total;
   unsigned int       *idlist = NULL;
   int                 ix;
   char                tmp[20];

   /* The number of currently registered handset is retrieved from the actual
   ** number of valid handset IPUI that we have stroed in the persistent storage.
   **
   ** The VOICE application is responsible for accuracy of the registered handset
   ** information data in the persistent storage, so it is safe to query the information
   ** like so.
   */

   /* Note that since we only can support one DECT interface, we can directly query the handset
   ** object associated with this interface without having to create an additional info level
   ** for the DECT interface itself.
   */

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, list, length);

   cmsLog_debug( "DECT instance: %d\n", parms->op[0] );

   /* Get the 'absolute' maximum number of handset that can be supported by this
   ** application.
   */
   rutVoice_getMaxDectHset(&absMax);


   /* allocate memory */
   if( (idlist = cmsMem_alloc( sizeof(unsigned int) * ( absMax + 1), ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("failed to allocate memory\n");
      return CMSRET_INTERNAL_ERROR;
   }

   if(( ret = dalVoice_GetDectCurHsetList( parms, (unsigned int *)&total, idlist )) != CMSRET_SUCCESS )
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(idlist);
      return ret;
   }

   /* Loop through all the possible handset to see if some are valid, index starts
   ** at 1 in MDM instances.
   */
   list[0] = '\0';
   for ( ix = 0 ; ix < total ; ix++ )
   {
      sprintf(tmp, "%u", idlist[ix]);
      if(ix)
         strncat(list, ",", length);
      strncat(list, tmp, length);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(idlist);
   return  ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineAttachedHandsetObject *obj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, -1, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, -1, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_ATTACHED_HANDSET, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, mask, 1);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "%s Can't retrieve dect object\n", __FUNCTION__ );
      return ( ret );
   }

   /* Copy handset id mask value */
   *mask = (UINT32)obj->element;
   cmsObj_free( (void **) &obj );

   return ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineAttachedHandsetObject *obj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_ATTACHED_HANDSET, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, num, length);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d\n",info[1].instId);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line id object\n" );
      return ( ret );
   }

   /* Copy line id value */
   cmsLog_debug( "total (%u) attached dect handset on line id %u\n ", obj->totalNumber, info[1].instId);
   snprintf( (char*)num, length, "%u",obj->totalNumber );

   cmsObj_free( (void **) &obj );

   return ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineAttachedHandsetObject *obj = NULL;
   unsigned int       i, total    = 0;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, -1, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, -1, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_ATTACHED_HANDSET, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };
   char               tmp[20];

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, list, length);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d\n",info[1].instId);
   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line id object\n" );
      return ( ret );
   }

   /* Copy line id value */
   cmsLog_debug( "total (%u) attached dect handset on line id %u\n ", obj->totalNumber, info[1].instId);

   list[0] = '\0';
   for ( i=0, total=0 ;i<32 && total < obj->totalNumber;i++)
   {
      /* If the field's value does not exist return failure */
      if ( obj->element & ( 1 << i ) )
      {
         /* Copy dect line number */
         sprintf(tmp, "%u", i + 1);
         if(total)
            strncat(list, ",", length);
         strncat(list, tmp, length);
         total++;
      }
   }
   cmsObj_free( (void **) &obj );

   return ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineMelodyObject *obj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_MELODY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, melody, length);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d\n",info[1].instId);


   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line melody object\n" );
      return ( ret );
   }

   cmsLog_debug( "Melody (%u) on dect line id %u\n ", obj->element, info[1].instId);
   snprintf( melody, length, "%u", obj->element );

   cmsObj_free( (void **) &obj );

   return ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineMultiCallModeObject *obj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_MULTI_CALL_MODE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, mode, length);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d\n",info[1].instId);


   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line call mode object\n" );
      return ( ret );
   }

   cmsLog_debug( "Multiple call mode (%s) on dect line id %u\n ", obj->element?MDMVS_SINGLE:MDMVS_MULTIPLE, info[1].instId);
   snprintf( mode, length, "%s", obj->element?MDMVS_SINGLE:MDMVS_MULTIPLE);

   cmsObj_free( (void **) &obj );

   return ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineIntrusionCallObject *obj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_INTRUSION_CALL, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, mode, length);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d\n",info[1].instId);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line intrusion call object\n" );
      return ( ret );
   }

   cmsLog_debug( "Intrusion call (%s) on dect line id %u\n ", obj->element?MDMVS_ENABLED:MDMVS_DISABLED, info[1].instId);
   snprintf( mode, length, "%s", obj->element?MDMVS_ENABLED:MDMVS_DISABLED);

   cmsObj_free( (void **) &obj );

   return ( ret );
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
   CmsRet              ret = CMSRET_SUCCESS;
   int numDectHandsets;

   rutVoice_getNumDectEndpt( &numDectHandsets );

   snprintf( enabled, length, (numDectHandsets > 0) ? "1" : "0" );

   return ( ret );
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
   DectdCfgObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   if( getVal == NULL || length == 0 )
   {
      cmsLog_error( " getVal is NULL pointer\n" );
      return  CMSRET_INVALID_PARAM_VALUE;
   }

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_DECTD_CFG, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }
   else
   {
      snprintf( (char *)getVal, length, "%s",obj->loggingLevel );
   }

   /* Free object */
   cmsObj_free((void **) &obj);

   return (ret);
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineNameObject *nameObj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_NAME, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, Name, 1);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d",parms->op[1]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&nameObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line name object" );
      return ( ret );
   }
   else
   {
      /* Copy auth username value */
      REPLACE_STRING_IF_NOT_EQUAL(nameObj->element, Name);
      if( nameObj->element != NULL )
      {
         cmsLog_debug( "dect line name = %s\n ", nameObj->element);

         /* copy new value from local copy to MDM */
         if ( ( ret = cmsObj_set( nameObj,  &iidStack)) != CMSRET_SUCCESS )
         {
            cmsLog_error( "Can't set dect line name (ret = %d)", ret);
         }

      }

      cmsObj_free( (void **) &nameObj );
   }

   return ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineDectLineIdObject *obj = NULL;
   unsigned int       setValInt;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_DECT_LINE_ID, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, line, 1);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d\n",info[1].instId);

   setValInt = atoi(line);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line id object\n" );
      return ( ret );
   }
   else
   {
      /* Copy line id value */
      obj->element = setValInt;
      cmsLog_debug( "dect line id = %u\n ", obj->element);

      /* copy new value from local copy to MDM */
      if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set dect line id ret = %d\n", ret);
      }

      cmsObj_free( (void **) &obj );
   }
   return ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineMelodyObject *obj = NULL;
   unsigned int       setValInt;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_MELODY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, melody, 1);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d\n",info[1].instId);

   setValInt = atoi(melody);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line melody object\n" );
      return ( ret );
   }
   else
   {
      obj->element = setValInt;
      cmsLog_debug( "Melody (%u) on dect line id %u\n ", obj->element, info[1].instId);

      /* copy new value from local copy to MDM */
      if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set dect line melody ret = %d\n", ret);
      }

      cmsObj_free( (void **) &obj );
   }

   return ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineMultiCallModeObject *obj = NULL;
   unsigned int       setValInt;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_MULTI_CALL_MODE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, mode, 1);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d\n",info[1].instId);

   /* convert mode from text to digit */
   if ( !cmsUtl_strcasecmp( MDMVS_SINGLE, mode ))
   {
      setValInt = 1;
   }
   else if ( !cmsUtl_strcasecmp( MDMVS_MULTIPLE, mode ))
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", mode );
      return ( CMSRET_INVALID_PARAM_VALUE );
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line call mode object\n" );
      return ( ret );
   }
   else
   {
      obj->element = setValInt;
      cmsLog_debug( "Multiple call mode (%u) on dect line id %u\n ", obj->element, info[1].instId);

      if ( ( ret = cmsObj_set( obj,  &iidStack )) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set dect line intrusion call mode ret = %d\n", ret);
      }

      cmsObj_free( (void **) &obj );
   }
   return ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineIntrusionCallObject *obj = NULL;
   unsigned int       setValInt;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_INTRUSION_CALL, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, mode, 1);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d\n", info[1].instId);

   if ( !cmsUtl_strcasecmp( MDMVS_ENABLED, mode ))
   {
      setValInt = 1;
   }
   else if ( !cmsUtl_strcasecmp( MDMVS_DISABLED, mode ))
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", mode );
      return ( CMSRET_INVALID_PARAM_VALUE );
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect line intrusion call object\n" );
      return ( ret );
   }
   else
   {
      cmsLog_debug( "Intrusion call (%s) on dect line id %u\n ", mode, info[1].instId);

      obj->element = setValInt;

      if ( ( ret = cmsObj_set( obj,  &iidStack )) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set dect line intrusion call mode ret = %d\n", ret);
      }

      cmsObj_free( (void **) &obj );
   }

   return ( ret );
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
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineAttachedHandsetObject *obj = NULL;
   char * tokResult;                 /* Points to the tokenized result */
   char tokString[LIST_CHAR_BUFFER]; /* copy of entire command line of codec list */
   char * handsetList[32];           /* tokenized array of each handset token (supports max 32 handset) */
   char delim[2] = ",";              /* 'setVal' list delimiter */
   int numSetHandset = 0;            /* count the total number of handset being set */
   int i, j;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_ATTACHED_HANDSET, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, list, 1);

   info[0].instId = parms->op[0];
   info[1].instId = parms->op[1];

   cmsLog_debug( "Line: %d\n", info[1].instId);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve dect attached handset object\n" );
      return ( ret );
   }

   tokString[0] = '\0';
   strncpy( tokString, list, sizeof(tokString)-1 );

   /* Set all list entries to NULL */
   for ( i = 0; i < 32; i++ )
   {
      handsetList[ i ] = NULL;
   }

   /* parse first token */
   tokResult = strtok( tokString, delim );
   while ( tokResult != NULL )
   {
      /* allocate memory for this token, including a null-terminator */
      handsetList[numSetHandset] = tokResult;

      /* increment the count of total number of tokens */
      numSetHandset++;

      /* Get next token */
      tokResult = strtok( NULL, delim );
   }

   /* Copy line id value */
   if(numSetHandset >= 0 )
   {
      obj->totalNumber = numSetHandset;
      obj->element = 0;

      cmsLog_debug( "total (%u) attached dect handset on line id %u\n ", obj->totalNumber, info[1].instId);

      for ( i=0; i< numSetHandset; i++)
      {
         j = atoi(handsetList[i]);
         j = (j == 0 || j >= 32 ) ? 0 : (j - 1);
         obj->element |=  ( 1 << j );
      }

      /* copy new value from local copy to MDM */
      if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set dect handset on line id %u in MDM\n", info[1].instId );
      }
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
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
   DectdCfgObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Only allow specific values */
   if (     cmsUtl_strcasecmp( setVal,MDMVS_ERROR  )
         && cmsUtl_strcasecmp( setVal,MDMVS_NOTICE )
         && cmsUtl_strcasecmp( setVal,MDMVS_DEBUG  )   )
   {
      cmsLog_error( "Invalid log level value\n" );
      return CMSRET_INVALID_PARAM_VALUE;
   }

   /* Get AppCfg.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_DECTD_CFG, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }
   else
   {
      REPLACE_STRING_IF_NOT_EQUAL(obj->loggingLevel, setVal);
      if ( (ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "could not set voice obj, ret=%d", ret);
      }

      cmsObj_free((void **) &obj);

   }

   return (ret);
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   parms->op[1] = MDMOID_VOICE_DECT_CLOCK_MASTER;

   return( setDectSystemObjectElement(parms, (void *)value));
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   parms->op[1] = MDMOID_VOICE_DECT_RESET_BASE;

   return( setDectSystemObjectElement(parms, (void *)value));
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
   CmsRet ret;
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   /* If DECT is calling this function, it has already updated the PIN code
    * in the EEPROM, so we just have to make MDM reflect the correct value.
    */
   parms->op[1] = MDMOID_VOICE_DECT_PIN_CODE;
   ret = setDectSystemObjectElement(parms, (void *)value);

   return ret;
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
   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   parms->op[1] = MDMOID_VOICE_DECT_FIRMWARE_VERSION;
   return setDectSystemObjectElement(parms, (void *)value);
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_NUMBER_OF_MISSED_CALLS;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_NAME;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_NUMBER;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_LINE_NAME;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_LINE_ID;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_TYPE;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_DATE_TIME;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CALL_NEW_FLAG;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[1] = MDMOID_VOICE_CONTACT_LIST;
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[3] = MDMOID_VOICE_CONTACT_NAME;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[1] = MDMOID_VOICE_CONTACT_LIST;
   local_parms.op[3] = MDMOID_VOICE_CONTACT_FIRST_NAME;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[1] = MDMOID_VOICE_CONTACT_LIST;
   local_parms.op[3] = MDMOID_VOICE_CONTACT_MELODY;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[1] = MDMOID_VOICE_CONTACT_LIST;
   local_parms.op[3] = MDMOID_VOICE_CONTACT_LINE_ID;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   DAL_VOICE_PARMS  local_parms;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[1] = MDMOID_VOICE_CONTACT_LIST;
   local_parms.op[3] = MDMOID_VOICE_CONTACT_NUMBER;
   local_parms.op[4] = 0;

   return ( setDectListObjectElement(&local_parms, (void *)value ));
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
   CmsRet                 ret;
   VoiceDECTHandsetObject *dectObj   = NULL;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, strlen(value));

   ret = dalVoice_GetDectHandsetObj(parms, &dectObj, &iidStack );
   if( ret == CMSRET_SUCCESS && dectObj != NULL)
   {
      REPLACE_STRING_IF_NOT_EQUAL(dectObj->X_BROADCOM_COM_Name, value);

      ret = cmsObj_set( dectObj, &iidStack );
      if ( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set Handset %d name ret = %d\n", parms->op[1], ret);
      }

      cmsObj_free( (void **) &dectObj );
   }

   return  ( ret );
}

CmsRet dalVoice_AddVoiceCallListEntry(DAL_VOICE_PARMS *parms, char *name, char *number, char *dateTime, char *lineName, int lineId, char *callType)
{
   CmsRet ret;
   char* sLineId;
   unsigned int instId;

   /* op[0] = VoiceService  */
   parms->op[0] = 1;

   /* Sanity on the callt ype */
   if(mapCallTxt2Type(callType) == VOICE_CALL_TYPE_MAX)
   {
      cmsLog_error("%s: Invalid call type.", __FUNCTION__);
      return CMSRET_INVALID_PARAM_VALUE;
   }

   ret = dalVoice_AddVoiceCallInstance(parms, &instId);
   if(ret == CMSRET_SUCCESS )
   {
      CmsRet setRetVal;
      /* op[1] = Voice call list instance id */
      parms->op[1] = instId;

      setRetVal = dalVoice_SetVoiceCallName(parms, name);
      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add name, ret = %d\n", __FUNCTION__, ret);
         ret = setRetVal;
      }

      setRetVal = dalVoice_SetVoiceCallNumber(parms, number);
      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add number, ret = %d\n", __FUNCTION__, ret);
         ret = setRetVal;
      }

      setRetVal = dalVoice_SetVoiceCallDateTime(parms, dateTime);
      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add date-time, ret = %d\n", __FUNCTION__, ret);
         ret = setRetVal;
      }

      setRetVal = dalVoice_SetVoiceCallLineName(parms, lineName);
      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add line name, ret = %d\n", __FUNCTION__, ret);
         ret = setRetVal;
      }

      setRetVal = dalVoice_SetVoiceCallType(parms, callType);
      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add call type, ret = %d\n", __FUNCTION__, ret);
         ret = setRetVal;
      }

      VOICECALLTYPE type = mapCallTxt2Type(callType);
      if(type == INCOMING || type == OUTGOING )
      {
         /* clear newflag on outgoing and incoming answered call */
         setRetVal = dalVoice_SetVoiceCallNewFlag(parms, MDMVS_NO);
      }
      else /* MISSED calls */
      {
         /* set newflag on missed call */
         setRetVal = dalVoice_SetVoiceCallNewFlag(parms, MDMVS_YES);
      }

      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't set newflag, ret = %d\n", __FUNCTION__, ret);
         ret = setRetVal;
      }

      if( (sLineId = cmsMem_alloc( sizeof(unsigned char) * ( MAX_TR104_OBJ_SIZE + 1), ALLOC_ZEROIZE)) == NULL)
      {
         cmsLog_error("failed to allocate memory\n");
         return CMSRET_INTERNAL_ERROR;
      }

      sprintf(sLineId, "%d", lineId);
      setRetVal = dalVoice_SetVoiceCallLineId(parms, sLineId);
      CMSMEM_FREE_BUF_AND_NULL_PTR(sLineId);
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add line id, ret = %d\n", __FUNCTION__, ret);
         ret = setRetVal;
      }
   }
   else
   {
      cmsLog_error("Can't add new instance in contact list ret = %d\n", ret);
   }

   /* Make sure the sizes of our call lists are under the thresholds we've
    * predefined. If they are over, delete old entries until we are within the
    * limit. */
   dalVoice_TrimCallLists();

   return ret;
}

CmsRet dalVoice_MapDectLineToCmLine( int dectline, int *cmline )
{
   /* because dectLine number is just cmLine number plus 1 so ..... */
   if(dectline > 0 )
   {
      *cmline = dectline - 1;

      return CMSRET_SUCCESS;
   }
   else
   {
      return CMSRET_INVALID_PARAM_VALUE;
   }
}

CmsRet dalVoice_MapCmLineToDectLine( int cmline, int *dectline )
{
   if(cmline >= 0 )
   {
      *dectline = cmline + 1;
      return CMSRET_SUCCESS;
   }
   else
   {
      return CMSRET_INVALID_PARAM_VALUE;
   }
}

CmsRet dalVoice_GetLineSettingId( DAL_VOICE_PARMS *parms, UINT32 *lineId, unsigned int length )
{
   unsigned int               vpInst     = parms->op[0];
   unsigned int               lineInst   = parms->op[1];
   CmsRet                     ret;
   InstanceIdStack            iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineDectLineIdObject *lineIdObj  = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                             {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                             {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                             {MDMOID_VOICE_LINE_DECT_LINE_ID, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                             {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile Line object */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&lineIdObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line instance object\n" );
      return ( ret );
   }
   /* If value of object doesnt exist, return */
   if( lineIdObj == NULL )
   {
      cmsLog_error( "Object value does not exist\n" );
      cmsObj_free((void **) &lineIdObj);
      return CMSRET_REQUEST_DENIED;
   }

   cmsLog_debug( "lineIdObj->element is: [%d]\n", (UINT8)lineIdObj->element );

   /* Copy over the Line id */
   *lineId = (UINT32)lineIdObj->element;

   cmsObj_free((void **) &lineIdObj);

   return ret;

}

CmsRet dalVoice_GetLineSettingName( DAL_VOICE_PARMS *parms, char *lineName, unsigned int length )
{
   unsigned int             vpInst       = parms->op[0];
   unsigned int             lineInst     = parms->op[1];
   CmsRet                   ret;
   InstanceIdStack          iidStack     = EMPTY_INSTANCE_ID_STACK;
   VoiceLineNameObject     *lineNameObj  = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                             {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                             {MDMOID_VOICE_LINE_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                             {MDMOID_VOICE_LINE_NAME, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                             {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile Line object */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&lineNameObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line instance object\n" );
      return ( ret );
   }
   /* If value of object doesnt exist, return */
   if( lineNameObj == NULL )
   {
      cmsLog_error( "Object value does not exist\n" );
      cmsObj_free((void **) &lineNameObj);
      return CMSRET_REQUEST_DENIED;
   }

   cmsLog_debug( "lineNameObj->element is: [%s]\n", lineNameObj->element );

   /* Copy over the Line id */
   //*lineId = (UINT8)lineIdObj->element;
   snprintf( lineName, length, "%s", lineNameObj->element );

   cmsObj_free((void **) &lineNameObj);

   return ret;

}

#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */

/*<END>===================================== DAL Set functions =======================================<END>*/

/*<START>===================================== Set Helper Functions ======================================<START>*/

#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1


/***************************************************************************
* Function Name: setDectListObjectElement
* Description  : Gets system setting value
*
* Parameters   : (INPUT) parms->op[0] - voice service instance, right now alway be 1
*                (INPUT) parms->op[1] - list object id
*                (INPUT) parms->op[2] - list object instance id
*                (INPUT) parms->op[3] - list sub-field object id
*                (INPUT) length - size of value
*                (OUTPUT) value - return value
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setDectListObjectElement(DAL_VOICE_PARMS *parms, void *value )
{
   CmsRet                 ret;
   void                  *obj = NULL;
   InstanceIdStack        iidStack   = EMPTY_INSTANCE_ID_STACK;


   cmsLog_debug("parms = %p, value = %p", parms, value);
   ret = getDectListObjectInstance(parms, &iidStack, &obj);
   if( ret == CMSRET_SUCCESS )
   {
      switch(parms->op[3])
      {
         case MDMOID_VOICE_CONTACT_NAME:
            {
               REPLACE_STRING_IF_NOT_EQUAL(((_VoiceContactNameObject *)obj)->element, (char *)value);
            }
            break;

         case MDMOID_VOICE_CONTACT_FIRST_NAME:
            {
               REPLACE_STRING_IF_NOT_EQUAL(((_VoiceContactFirstNameObject *)obj)->element, (char *)value);
            }
            break;

         case MDMOID_VOICE_CONTACT_NUMBER:
            {
               /* the format of contact numbers should be: "numberType:number,"
               ** numberType: 1-3
               ** for multiple numbers: "1:6045557788,2:6047773355,3:6049997722"
               */
               int  i;
               void      *numberObj = NULL;
               const char num_delim[2] = ",";
               const char type_delim[2] = ":";
               char *parse_str, *number, *type;
               char *saveptr1, *saveptr2;


               if(value != NULL)
               {
                  for(i = 1, parse_str = (char *) value; i <= MAX_CONTACT_NUMBER_INSTANCE ; i++, parse_str = NULL)
                  {
                     memset(&iidStack, 0, sizeof(iidStack));
                     parms->op[4] = i;

                     ret = getDectListObjectInstance(parms, &iidStack, &numberObj);
                     if( ret == CMSRET_SUCCESS )
                     {
                        number = strtok_r(parse_str, num_delim, &saveptr1);

                        if( number != NULL )
                        {
                           /* get number type */
                           type = strtok_r(number, type_delim, &saveptr2);
                           if(type != NULL)
                           {
                              ((_VoiceContactNumberObject *)numberObj)->numberType = atoi(type) & 0x3; /* limited to 1 -3 range */
                           }

                           /* get number string */
                           number = strtok_r(NULL, type_delim, &saveptr2);
                           if(number != NULL)
                           {
                              REPLACE_STRING_IF_NOT_EQUAL(((_VoiceContactNumberObject *)numberObj)->number, number);
                           }

                        }
                        else
                        {
                           ((_VoiceContactNumberObject *)numberObj)->numberType = 0;
                           CMSMEM_FREE_BUF_AND_NULL_PTR(((_VoiceContactNumberObject *)numberObj)->number);
                        }

                        if ( ( ret = cmsObj_set( numberObj, &iidStack)) != CMSRET_SUCCESS )
                        {
                           cmsLog_error( "Can't set Voice Call List object ret = %d\n", ret);
                        }
                        
                        cmsObj_free(&numberObj);
                     }
                  }
               }
            }
            break;

         case MDMOID_VOICE_CONTACT_MELODY:
            {
               ((_VoiceContactMelodyObject *)obj)->element = (unsigned int)atoi(value);
            }
            break;

         case MDMOID_VOICE_CONTACT_LINE_ID:
         case MDMOID_VOICE_CALL_LINE_ID:
            {
               const char type_delim[2] = ":";
               char *p = NULL;
               int lineId = 0, type = 0;
               char *saveptr;

               /* get line number */
               p = strtok_r((char *)value, type_delim, &saveptr);
               if(p != NULL)
               {
                 lineId = atoi(p);
               }

               /* get line type */
               p = strtok_r(NULL, type_delim, &saveptr);
               if(p != NULL)
               {
                  type = atoi(p);
               }

               ((_VoiceContactLineIdObject *)obj)->element = (unsigned int)lineId;
               ((_VoiceContactLineIdObject *)obj)->lineIdSubType = (unsigned int)type;
            }
            break;
         case MDMOID_VOICE_CALL_NAME:
            {
               REPLACE_STRING_IF_NOT_EQUAL(((_VoiceCallNameObject *)obj)->element, (char *)value);
            }
            break;
         case MDMOID_VOICE_CALL_NUMBER:
            {
               REPLACE_STRING_IF_NOT_EQUAL(((_VoiceCallNumberObject *)obj)->element, (char *)value);
            }
            break;
         case MDMOID_VOICE_CALL_LINE_NAME:
            {
               REPLACE_STRING_IF_NOT_EQUAL(((_VoiceCallLineNameObject *)obj)->element, (char *)value);
            }
            break;
         case MDMOID_VOICE_CALL_NEW_FLAG:
            {
               if( !cmsUtl_strcasecmp(MDMVS_YES, value) )
               {
                  ((_VoiceCallNewFlagObject *)obj)->element = 1;
               }
               else
               {
                  ((_VoiceCallNewFlagObject *)obj)->element = 0;
               }
            }
            break;
         case MDMOID_VOICE_CALL_DATE_TIME:
            {
               REPLACE_STRING_IF_NOT_EQUAL(((_VoiceCallDateTimeObject *)obj)->element, (char *)value);
            }
            break;
         case MDMOID_VOICE_CALL_NUMBER_OF_MISSED_CALLS:
            {
               ((_VoiceCallNumberOfMissedCallsObject *)obj)->element = (unsigned int)atoi(value);
            }
            break;
         case MDMOID_VOICE_CALL_TYPE:
            {
               VOICECALLTYPE type = mapCallTxt2Type(value);
               if(type == INCOMING || type == OUTGOING || type == MISSED)
               {
                  ((_VoiceCallTypeObject *)obj)->element = type;
               }
               else
               {
                  ret = CMSRET_INVALID_PARAM_VALUE;
               }
            }
            break;

         default:
            cmsLog_error( "mdm_oid parameter (%u) is out of range\n", parms->op[3] );
            ret = CMSRET_INVALID_PARAM_VALUE;
      }

      if(ret == CMSRET_SUCCESS )
      {
         if ( ( ret = cmsObj_set( obj, &iidStack)) != CMSRET_SUCCESS )
         {
            cmsLog_error( "Can't set Voice Call List object ret = %d\n", ret);
         }
      }

      cmsObj_free(&obj);
   }

   return ( ret );
}


/***************************************************************************
* Function Name: setDectSystemObjectElement
* Description  : Gets system setting value
*
* Parameters   : (INPUT) parms->op[0] - voice service instance, right now alway be 1
*                (INPUT) parms->op[1] - dect system setting object id
*                (INPUT) value - set value
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setDectSystemObjectElement(DAL_VOICE_PARMS *parms, void *value )
{
   CmsRet                 ret;
   void                  *obj = NULL;
   InstanceIdStack        iidStack   = EMPTY_INSTANCE_ID_STACK;

   cmsLog_debug( "setDectSystemObjectElement()");
   ret = getDectSystemObject(parms, &iidStack, &obj);
   if( ret == CMSRET_SUCCESS )
   {
      switch(parms->op[1])
      {
         case MDMOID_VOICE_DECT_PIN_CODE:
            {
               REPLACE_STRING_IF_NOT_EQUAL(((_VoiceDectPinCodeObject *)obj)->element, (char *)value);
            }
            break;

         case MDMOID_VOICE_DECT_CLOCK_MASTER:
            {
               if(!cmsUtl_strcasecmp(MDMVS_PP, (char *)value))
               {
                  ((_VoiceDectClockMasterObject *)obj)->element = 1;
               }
               else if(!cmsUtl_strcasecmp(MDMVS_FP, (char *)value))
               {
                  ((_VoiceDectClockMasterObject *)obj)->element = 0;
               }
               else
               {
                  ret = CMSRET_INVALID_PARAM_VALUE;
               }
            }
            break;

         case MDMOID_VOICE_DECT_RESET_BASE:
            {
               if(!cmsUtl_strcasecmp(MDMVS_YES, (char *)value))
               {
                  ((_VoiceDectResetBaseObject *)obj)->element = 1;
               }
               else if(!cmsUtl_strcasecmp(MDMVS_NO, (char *)value))
               {
                  ((_VoiceDectResetBaseObject *)obj)->element = 0;
               }
               else
               {
                  ret = CMSRET_INVALID_PARAM_VALUE;
               }
            }
            break;

         case MDMOID_VOICE_DECT_FIRMWARE_VERSION:
            {
               REPLACE_STRING_IF_NOT_EQUAL(((_VoiceDectFirmwareVersionObject *)obj)->element, (char *)value);
            }
            break;

         default:
            cmsLog_error( "mdm_oid parameter (%u) is out of range pointer\n", parms->op[1] );
            ret = CMSRET_INVALID_PARAM_VALUE;
      }

      if(ret == CMSRET_SUCCESS )
      {
         cmsLog_debug( "setDectSystemObjectElement() 1");
         if ( ( ret = cmsObj_set( obj, &iidStack)) != CMSRET_SUCCESS )
         {
            cmsLog_error( "Can't set Dect System Setting object ret = %d\n", ret);
         }
      }

      cmsObj_free(&obj);
   }

   return ( ret );
}

#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */

/*<END>===================================== Set Helper Functions ========================================<END>*/

/*<START>================================= Get Helper Functions ==========================================<START>*/
#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1

/***************************************************************************
* Function Name: getDectSystemObjectElement
* Description  : Gets system setting value
*
* Parameters   : (INPUT) parms->op[0] - voice service instance, right now alway be 1
*                (INPUT) parms->op[1] - dect system setting object id
*                (INPUT) length - size of value
*                (OUTPUT) value - return value
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet getDectSystemObjectElement(DAL_VOICE_PARMS *parms, void *value, unsigned int length )
{
   CmsRet                 ret;
   void                  *obj = NULL;
   InstanceIdStack        iidStack   = EMPTY_INSTANCE_ID_STACK;

   ret = getDectSystemObject(parms, &iidStack, &obj);
   if( ret == CMSRET_SUCCESS )
   {
      switch(parms->op[1])
      {
         case MDMOID_VOICE_DECT_PIN_CODE:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceDectPinCodeObject *)obj)->element);
            }
            break;

         case MDMOID_VOICE_DECT_CLOCK_MASTER:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceDectClockMasterObject *)obj)->element ? MDMVS_PP : MDMVS_FP);
            }
            break;

         case MDMOID_VOICE_DECT_RESET_BASE:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceDectResetBaseObject *)obj)->element ? MDMVS_YES : MDMVS_NO);
            }
            break;

         case MDMOID_VOICE_DECT_FIRMWARE_VERSION:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceDectFirmwareVersionObject *)obj)->element);
            }
            break;

         case MDMOID_VOICE_DECT_HARDWARE_VERSION:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceDectHardwareVersionObject *)obj)->element);
            }
            break;

         case MDMOID_EEPROM_VERSION:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceDectEEPROMVersionObject *)obj)->element);
            }
            break;

         default:
            cmsLog_error( "mdm_oid parameter (%u) is out of range pointer\n", parms->op[1] );
            ret = CMSRET_INVALID_PARAM_VALUE;
      }
      cmsObj_free(&obj);
   }

   return ( ret );
}




/***************************************************************************
* Function Name: getDectListObjectElement
* Description  : Gets list object elements
*
* Parameters   : (INPUT) parms->op[0] - voice service instance, right now alway be 1
*                (INPUT) parms->op[1] - list object id
*                (INPUT) parms->op[2] - list object instance id
*                (INPUT) parms->op[3] - list sub-field object id
*                (INPUT) length - size of value
*                (OUTPUT) value - return value
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet getDectListObjectElement(DAL_VOICE_PARMS *parms, void *value, unsigned int length )
{
   CmsRet                 ret;
   void                  *obj = NULL;
   InstanceIdStack        iidStack   = EMPTY_INSTANCE_ID_STACK;

   ret = getDectListObjectInstance(parms, &iidStack, &obj);
   if( ret == CMSRET_SUCCESS )
   {
      switch(parms->op[3])
      {
         case MDMOID_VOICE_CONTACT_NAME:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceContactNameObject *)obj)->element);
            }
            break;

         case MDMOID_VOICE_CONTACT_FIRST_NAME:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceContactFirstNameObject *)obj)->element);
            }
            break;

         case MDMOID_VOICE_CONTACT_NUMBER:
            {
               int  i;
               char temp[64];
               void *numberObj = NULL;
               (void)value;
               /* we know that contact number has 3 instances */
               for(i = 1; i <= MAX_CONTACT_NUMBER_INSTANCE ; i++ )
               {
                  parms->op[4] = i;

                  ret = getDectListObjectInstance(parms, &iidStack, &numberObj);
                  if( ret == CMSRET_SUCCESS )
                  {
                     if(((_VoiceContactNumberObject *)numberObj)->number != NULL)
                     {
                        memset(temp, 0 , sizeof(temp));

                        snprintf( temp,
                                  length,
                                  "%1d:%s,",
                                  ((_VoiceContactNumberObject *)numberObj)->numberType,
                                  ((_VoiceContactNumberObject *)numberObj)->number);

                        strncat(value, temp, strlen(temp));
                     }

                     cmsObj_free(&numberObj);
                  }
               }
/* 
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceContactNumberObject *)obj)->element);
*/
            }
            break;

         case MDMOID_VOICE_CONTACT_MELODY:
            {
               snprintf( (char*)value,
                         length,
                         "%u",
                         ((_VoiceContactMelodyObject *)obj)->element);
            }
            break;

         case MDMOID_VOICE_CONTACT_LINE_ID:
         case MDMOID_VOICE_CALL_LINE_ID:
            {
               snprintf( (char*)value,
                         length,
                         "%1u:%u",
                         ((_VoiceContactLineIdObject *)obj)->element,
                         ((_VoiceContactLineIdObject *)obj)->lineIdSubType);
            }
            break;

         case MDMOID_VOICE_CALL_NAME:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceCallNameObject *)obj)->element);
            }
            break;
         case MDMOID_VOICE_CALL_NUMBER:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceCallNumberObject *)obj)->element);
            }
            break;
         case MDMOID_VOICE_CALL_LINE_NAME:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceCallLineNameObject *)obj)->element);
            }
            break;
         case MDMOID_VOICE_CALL_NEW_FLAG:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceCallNewFlagObject *)obj)->element?MDMVS_YES:MDMVS_NO);
            }
            break;
         case MDMOID_VOICE_CALL_DATE_TIME:
            {
               snprintf( (char*)value,
                         length,
                         "%s",
                         ((_VoiceCallDateTimeObject *)obj)->element);
            }
            break;
         case MDMOID_VOICE_CALL_NUMBER_OF_MISSED_CALLS:
            {
               snprintf( (char*)value,
                         length,
                         "%u",
                         ((_VoiceCallNumberOfMissedCallsObject *)obj)->element);
            }
            break;

         case MDMOID_VOICE_CALL_TYPE:
            {
               char calltype[20];

               mapCallType2Txt((VOICECALLTYPE)((_VoiceCallTypeObject *)obj)->element, calltype);
               snprintf( (char*)value,
                         length,
                         "%s",
                         calltype);
            }
            break;
         default:
            cmsLog_error( "mdm_oid parameter (%u) is out of range pointer\n", parms->op[2] );
            ret = CMSRET_INVALID_PARAM_VALUE;
      }
      cmsObj_free(&obj);
   }

   return ( ret );
}


/***************************************************************************
* Function Name: getDectTotalNumberOfInstancesInList
* Description  : Gets total number of instances in contact list
*
* Parameters   : parms->op[0] - voice service instance
*                parms->op[1] - list object id
*                value - returned number instance in list
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet getDectTotalNumberOfInstancesInList(DAL_VOICE_PARMS *parms, unsigned int *value, unsigned int *list )
{
   unsigned int *buf = NULL;
   MdmObjectId  objId;
   int          i = 0;
   void        *obj = NULL;
   InstanceIdStack  iidStack =  EMPTY_INSTANCE_ID_STACK;

   objId = parms->op[1];

   buf = list;

   /* Traverse objects until match found, or getting next object is not successful */
   while (cmsObj_getNextFlags ( objId, &iidStack, OGF_NO_VALUE_UPDATE, &obj ) == CMSRET_SUCCESS )
   {
      if( buf != NULL)
      {
         buf[i] = PEEK_INSTANCE_ID(&iidStack);
      }
      i++;
      /* free up unwanted object */
      cmsObj_free( &obj );
   }

   (*value) = i;
   return CMSRET_SUCCESS;
}



/***************************************************************************
* Function Name: getLasSystemObject
* Description  : Gets information from the DECT module about the modele
*                identifier of the handset registered.
*
* Parameters   : parms
*                value - returned value information
*                length - size of the returned information
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet getDectSystemObject(DAL_VOICE_PARMS *parms, InstanceIdStack *iidStack, void **obj )
{
   /* Note that since we only can support one DECT interface, we can directly query the handset
   ** object associated with this interface without having to create an additional info level
   ** for the DECT interface itself.
   */
   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_DECT_SYSTEM_SETTING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {MDM_MAX_OID, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0,                   0,        EMPTY_INSTANCE_ID_STACK},
                                              };

   if(parms == NULL)
   {
      cmsLog_error( "parameter is NULL pointer\n" );
      return CMSRET_INVALID_PARAM_VALUE;
   }

   info[1].id = parms->op[1];
   info[1].instId = parms->op[2];
   if( info[1].id >= MDM_MAX_OID )
   {
      cmsLog_error( "mdm_oid parameter (%u) is out of range pointer\n", info[1].id );
      return CMSRET_INVALID_PARAM_VALUE;
   }

   return ( getObj( info, iidStack, OGF_NO_VALUE_UPDATE, obj ));
}

/***************************************************************************
* Function Name: getDectListObjectInstance
* Description  : Gets contact list object instance
*
* Parameters   : (INPUT) parms->op[0]  voice service instance
*                (INPUT) parms->op[1]  list object id
*                (INPUT) parms->op[2]  list object instance
*                (INPUT) parms->op[3]  sub-object id
*                (INPUT) parms->op[4]  sub-object instance
*                (OUTPUT) obj          return object point
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet getDectListObjectInstance(DAL_VOICE_PARMS *parms, InstanceIdStack *iidStack, void **obj )
{
   /* Note that since we only can support one DECT interface, we can directly query the handset
   ** object associated with this interface without having to create an additional info level
   ** for the DECT interface itself.
   */
   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDM_MAX_OID, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {MDM_MAX_OID, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0,                   0,        EMPTY_INSTANCE_ID_STACK},
                                              };

   if(parms == NULL)
   {
      cmsLog_error( "parameter is NULL pointer\n" );
      return CMSRET_INVALID_PARAM_VALUE;
   }

   info[0].id = parms->op[1];
   info[0].instId = parms->op[2];
   info[1].id = parms->op[3];
   if(parms->op[4] > 0){
      info[1].instId = parms->op[4];
   }

   cmsLog_debug("request field (%d),subinstance (%d) at instance (%d) at list (%d)", info[1].id, info[1].instId, info[0].instId, info[0].id);

   /* make sure the list object instance within range of tatal instances */

   if( info[0].id >= MDM_MAX_OID || info[1].id >= MDM_MAX_OID )
   {
      cmsLog_error( "mdm_oid parameter (%u) is out of range pointer\n", info[1].id );
      return CMSRET_INVALID_PARAM_VALUE;
   }
   else
   {
      return( getObj( info, iidStack, OGF_NO_VALUE_UPDATE, obj ));
   }

   return  CMSRET_INVALID_PARAM_VALUE;
}



#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */

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
   CmsRet           ret;
   InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   parms->op[1] = MDMOID_VOICE_CALL_LIST;

   /* Add the voice call to the list */
   if((ret = addDectListObjectInstance(parms, &iidStack )) == CMSRET_SUCCESS )
   {
      *value = PEEK_INSTANCE_ID(&iidStack);
   }

   cmsLog_debug("finished dalVoice_addInstance %u\n", (*value));
   return ret;
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
   CmsRet           ret;
   InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, value, 1);

   parms->op[1] = MDMOID_VOICE_CONTACT_LIST;

   if((ret = addDectListObjectInstance(parms, &iidStack )) == CMSRET_SUCCESS )
   {
      *value = PEEK_INSTANCE_ID(&iidStack);
   }

   cmsLog_debug("finished dalVoice_addInstance %u\n", (*value));
   return ret;
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
   CmsRet          ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   DAL_VOICE_PARMS parms;
   int             absMax, curr, inst  = ANY_INSTANCE;
   void           *obj = NULL;

   /* sanity check, make sure the total handset is not exceed the limit*/
   rutVoice_getMaxDectHset( &absMax );
   rutVoice_getCurrDectHset(&curr);
   if(curr >= absMax)
   {
      return CMSRET_RESOURCE_EXCEEDED;
   }

   /* check whether the instance already exist */
   if((ret = dalVoice_mapHsetId2Instance(handsetId, &inst)) == CMSRET_SUCCESS && inst == ANY_INSTANCE )
   {
      memset((void *)&parms, 0, sizeof(parms));
      parms.op[0] = 1;  /* voice service instance */
      parms.op[1] = MDMOID_DECT_HANDSET;

      /* add new handset instance */
      ret = addDectListObjectInstance(&parms, &iidStack);
      if(ret == CMSRET_SUCCESS)
      {
         inst = PEEK_INSTANCE_ID(&iidStack);
         parms.op[2] = inst;

         if( ( ret = getDectSystemObject(&parms, &iidStack, &obj)) == CMSRET_SUCCESS)
         {
            ((_VoiceDECTHandsetObject *)obj)->X_BROADCOM_COM_ID = (unsigned int)handsetId;

            if( ( ret = cmsObj_set( obj, &iidStack )) != CMSRET_SUCCESS )
            {
               cmsLog_error( "Can't set Dect handset instance %d ret = %d\n", inst, ret);
            }

            cmsObj_free(&obj);
         }
      }
   }

   return ret;
}

/***************************************************************************
* Function Name: addDectListObjectInstance
* Description  : Gets contact list object instance
*
* Parameters   : (INPUT) parms->op[0]  voice service instance
*                (INPUT) parms->op[1]  list object id
*                (OUTPUT) iidStack
*                (OUTPUT) obj          return object point
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet addDectListObjectInstance(DAL_VOICE_PARMS *parms, InstanceIdStack *iidStack )
{
   CmsRet   ret;
   /* Note that since we only can support one DECT interface, we can directly query the handset
   ** object associated with this interface without having to create an additional info level
   ** for the DECT interface itself.
   */
   if(parms == NULL)
   {
      cmsLog_error( "parameter is NULL pointer\n" );
      return CMSRET_INVALID_PARAM_VALUE;
   }

   INIT_INSTANCE_ID_STACK(iidStack);
   PUSH_INSTANCE_ID( iidStack, parms->op[0] );  /* Voice Service Object instance id*/

   if ( (ret = cmsObj_addInstance(parms->op[1], iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "could not add new instance at %d level\n", parms->op[1]);
   }

   return  ret;
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
   DAL_VOICE_PARMS  local_parms;
   InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, "", 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[1] = MDMOID_VOICE_CALL_LIST;

   return (delDectListObjectInstance(&local_parms, &iidStack ));
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
   DAL_VOICE_PARMS  local_parms;
   InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, "", 1);

   local_parms.op[0] = parms->op[0];
   local_parms.op[2] = parms->op[1];  /* move instance id to next parms.op */
   local_parms.op[1] = MDMOID_VOICE_CONTACT_LIST;

   return( delDectListObjectInstance(&local_parms, &iidStack ));
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
   CmsRet                 ret;
   VoiceDECTHandsetObject *dectObj   = NULL;
   InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;

   CHECK_PARMS_AND_RETURN_ON_ERROR(parms, "", 1);

   cmsLog_debug( "%s DECT instance: %d\n", __FUNCTION__, parms->op[1] );

   /* Create the instance identifier stack for this CMS object request.
   */
   ret = dalVoice_GetDectHandsetObj( parms, &dectObj, &iidStack);

   if ( ret == CMSRET_SUCCESS )
   {
      /* 
      ** we are not delete mdm instance directly, instead we send de-registration
      ** command to dectd, dectd will delete instance after handset de-reigstered 
      ** 
      ** set X_BROADCOM_COM_Delete flag will trigger RCL function 
      */
      dectObj->X_BROADCOM_COM_Delete = 1;

      if ( ( ret = cmsObj_set( dectObj, &iidStack )) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set Handset instance %d ret = %d\n", parms->op[1], ret);
      }

      cmsObj_free( (void **) &dectObj );
   }
   else
   {
      cmsLog_error( "Can't retrieve DECT handset %d object\n", parms->op[1] );
   }

   return  ( ret );
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
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   DAL_VOICE_PARMS parms;
   int             inst;

   if(dalVoice_mapHsetId2Instance(handsetId, &inst) == CMSRET_SUCCESS && inst != ANY_INSTANCE )
   {
      memset((void *)&parms, 0, sizeof(parms));
      parms.op[0] = 1;  /* voice service instance */
      parms.op[1] = MDMOID_DECT_HANDSET;
      parms.op[2] = inst;

      return delDectListObjectInstance(&parms, &iidStack);
   }

   return CMSRET_INVALID_PARAM_VALUE;
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
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   DAL_VOICE_PARMS parms;
   unsigned int   *iidlist = NULL, total, i;
   CmsRet          ret;

   rutVoice_getMaxDectHset( (int *)&total );

   if( (iidlist = cmsMem_alloc( sizeof(unsigned int) * ( total + 1), ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("failed to allocate memory\n");
      return CMSRET_INTERNAL_ERROR;
   }

   memset((void *)&parms, 0, sizeof(parms));
   parms.op[0] = 1; /* voice service instance; */
   parms.op[1] = MDMOID_DECT_HANDSET;

   /* get total number of registered handset */
   ret = getDectTotalNumberOfInstancesInList( &parms, &total, iidlist );
   if(ret != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get hset instance list\n");
      return CMSRET_INTERNAL_ERROR;
   }

   for( i = 0; i < total; i++)
   {
      parms.op[2] = iidlist[i];

      ret = delDectListObjectInstance(&parms, &iidStack);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(iidlist);

   return ret;
}

/***************************************************************************
* Function Name: delDectListObjectInstance
* Description  : Gets contact list object instance
*
* Parameters   : (INPUT) parms->op[0]  voice service instance
*                (INPUT) parms->op[1]  list object id
*                (INPUT) parms->op[2]  list object instance
*                (OUTPUT) iidStack
*                (OUTPUT) obj          return object point
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet delDectListObjectInstance(DAL_VOICE_PARMS *parms, InstanceIdStack *iidStack )
{
   CmsRet   ret;
   /* Note that since we only can support one DECT interface, we can directly query the handset
   ** object associated with this interface without having to create an additional info level
   ** for the DECT interface itself.
   */
   if(parms == NULL)
   {
      cmsLog_error( "parameter is NULL pointer\n" );
      return CMSRET_INVALID_PARAM_VALUE;
   }

   INIT_INSTANCE_ID_STACK(iidStack);
   PUSH_INSTANCE_ID( iidStack, parms->op[0] );  /* Voice Service Object instance id*/
   PUSH_INSTANCE_ID( iidStack, parms->op[2] );  /* Voice Service Object instance id*/

   if ( (ret = cmsObj_deleteInstance(parms->op[1], iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "could not del instance %d at %d level\n", parms->op[2], parms->op[1]);
   }

   return  ret;
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
   CmsRet                   ret;
   DAL_VOICE_PARMS          parms;
   int                      total, i;
   unsigned int            *iidlist = NULL;
   _VoiceDECTHandsetObject *obj = NULL;
   InstanceIdStack          iidStack   = EMPTY_INSTANCE_ID_STACK;
   void                    *msgBuf;

   rutVoice_getMaxDectHset( &total );

   if( ( msgBuf = cmsMem_alloc(sizeof(unsigned int) * (total + 1), ALLOC_ZEROIZE) ) == NULL)
   {
      cmsLog_error("failed to allocate memory\n");
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      iidlist = msgBuf;
   }

   parms.op[0] = 1; /* voice service instance; */
   parms.op[1] = MDMOID_DECT_HANDSET;
   ret = getDectTotalNumberOfInstancesInList( &parms, (unsigned int*)&total, iidlist );
   if( ret != CMSRET_SUCCESS )
   {
      cmsLog_error("failed to get hset instance list\n");
      return CMSRET_INTERNAL_ERROR;
   }

   *inst = ANY_INSTANCE;
   for( i = 0; i < total; i++)
   {
      parms.op[2] = iidlist[i];

      if( (ret = getDectSystemObject(&parms, &iidStack, (void *)&obj)) == CMSRET_SUCCESS )
      {
         if(obj->X_BROADCOM_COM_ID == (unsigned int)handsetId )
         {
            *inst = PEEK_INSTANCE_ID(&iidStack);
            cmsObj_free((void *)&obj);
            break;
         }

         cmsObj_free((void *)&obj);
      }
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(iidlist);

   return ret;
}

/******************************************************************************
* Function Name: dalVoice_TrimCallLists
* Description  : If the call lists are over their respective thresholds, this
*                function deletes the oldest calls until the list is the proper
*                size.
*
* Parameters   : None
*
* Returns      : CMSRET_SUCCESS when successful.
******************************************************************************/
static CmsRet dalVoice_TrimCallLists()
{
   void                    *obj = NULL;
   DAL_VOICE_PARMS          parms;
   DAL_VOICE_DECT_CALL_LIST_STATS stats;
   CmsRet                   ret;

   memset(&parms, 0, sizeof(parms));
   memset(&stats, 0, sizeof(stats));

   parms.op[0] = 1;  /* VoiceService */
   parms.op[1] = -1; /* -1 corresponds to stats for all lines */
   dalVoice_GetVoiceCallListStats(&parms, &stats);

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   ret = cmsObj_getNextFlags(MDMOID_VOICE_CALL_LIST, &iidStack, OGF_NO_VALUE_UPDATE, &obj);
   if(ret == CMSRET_SUCCESS)
   {
      /* Since we don't actually need it, free the object right away */
      cmsObj_free(&obj);
   }

   /* Loop through the call list from oldest to newest, deleting entries as needed */
   while(ret == CMSRET_SUCCESS)
   {
      CmsRet          localRet;
      void           *typeObj;
      InstanceIdStack typeIidStack = EMPTY_INSTANCE_ID_STACK;

      /* Setup the parameters to get the call type */
      parms.op[0] = 1;
      parms.op[1] = MDMOID_VOICE_CALL_LIST;
      parms.op[2] = PEEK_INSTANCE_ID(&iidStack);
      parms.op[3] = MDMOID_VOICE_CALL_TYPE;

      /* Get the next call list item right away. If we need to delete the
       * current item, we have to already have the next item or our
       * cmsObj_getNext() will not know about the current object. */
      ret = cmsObj_getNextFlags(MDMOID_VOICE_CALL_LIST, &iidStack, OGF_NO_VALUE_UPDATE, &obj);
      if(ret == CMSRET_SUCCESS)
      {
         /* Free the object right away */
         cmsObj_free(&obj);
      }

      if(getDectListObjectInstance(&parms, &typeIidStack, &typeObj) == CMSRET_SUCCESS)
      {
         if(((_VoiceCallTypeObject*)typeObj)->element == INCOMING &&
            stats.numIncoming > MAX_SIZE_INCOMING_CALL_LIST)
         {
            if( (localRet = cmsObj_deleteInstance(parms.op[1], &typeIidStack)) != CMSRET_SUCCESS )
            {
               cmsLog_error("%s: Unable to delete call (ret %d)", __FUNCTION__, localRet);
            }
            else
            {
               stats.numIncoming--;
            }
         }
         else if(((_VoiceCallTypeObject*)typeObj)->element == OUTGOING &&
                 stats.numOutgoing > MAX_SIZE_OUTGOING_CALL_LIST)
         {
            if( (localRet = cmsObj_deleteInstance(parms.op[1], &typeIidStack)) != CMSRET_SUCCESS )
            {
               cmsLog_error("%s: Unable to delete call (ret %d)", __FUNCTION__, localRet);
            }
            else
            {
               stats.numOutgoing--;
            }
         }
         else if(((_VoiceCallTypeObject*)typeObj)->element == MISSED &&
                 stats.numMissed > MAX_SIZE_MISSED_CALL_LIST)
         {
            if( (localRet = cmsObj_deleteInstance(parms.op[1], &typeIidStack)) != CMSRET_SUCCESS )
            {
               cmsLog_error("%s: Unable to delete call (ret %d)", __FUNCTION__, localRet);
            }
            else
            {
               stats.numMissed--;
            }
         }
         cmsObj_free(&typeObj);
      }
      else
      {
         cmsLog_notice("%s: Error getting type of call.", __FUNCTION__);
      }

      /* If the list size is small enough, we are done */
      if(stats.numIncoming <= MAX_SIZE_INCOMING_CALL_LIST &&
         stats.numOutgoing <= MAX_SIZE_OUTGOING_CALL_LIST &&
         stats.numMissed <= MAX_SIZE_MISSED_CALL_LIST)
      {
         return CMSRET_SUCCESS;
      }
   }

   return CMSRET_SUCCESS;
}

/***************************************************************************
** Function Name: getObj
** Description  : Gets a specific instance of an object lacated at a specific
**                level in the TR-104 hierarchy.
**
** Parameters   : info (IN)      - Array that identifies objects and their instances
**                iidStack (OUT) - Instance ID stack
**                flags (IN)     - Data access flags ( determine if STL needs to be called )
**                obj (OUT)      - The fetched object
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet getObj(LEVELINFO info[], InstanceIdStack *iidStack, UINT32 flags, void **obj)
{
   CmsRet ret = CMSRET_INTERNAL_ERROR;
   int i;

   /* Traverse until we reach the final level or run out of levels */
   for(i = 0; i < DAL_VOICE_MAXLEVELS && info[i].id > 0; i++)
   {
      INIT_INSTANCE_ID_STACK(&info[i].iidStack);

      /* Traverse object instances.*/
      while( (ret = (i == 0) ? cmsObj_getNextFlags(info[i].id, &info[i].iidStack, flags, obj) :
              cmsObj_getNextInSubTreeFlags(info[i].id, &info[i-1].iidStack, &info[i].iidStack, flags, obj)) == CMSRET_SUCCESS )
      {
         /* Found the specified instance or the first instance is instId = -1 */
         if ( info[i].instId <= 0 || (PEEK_INSTANCE_ID(&info[i].iidStack)) == info[i].instId )
         {
            *iidStack = info[i].iidStack;
            break;
         }

         /* Free the object if it not the final object the we are interested in */
         cmsObj_free(obj);
      }

      /* No need to continue if we could not fetch an object from the current level */
      if ( ret != CMSRET_SUCCESS )
      {
         return ret;
      }

      /* Free the object if it not the final object the we are interested in */
      if ( i != DAL_VOICE_MAXLEVELS-1 && info[i+1].id != 0 )
      {
         cmsObj_free(obj);
      }
   }

   return ret;
}

static CmsRet mapCallType2Txt(VOICECALLTYPE calltype, char *txt)
{
   unsigned int i;
   for( i = 0; i < (sizeof(map) / sizeof(CallTypeMapping)); i++ )
   {
      if(map[i].type == calltype)
      {
         sprintf(txt, "%s", map[i].txt);
         return CMSRET_SUCCESS;
      }
   }
   sprintf(txt, "unknown");
   return CMSRET_SUCCESS;
}

static VOICECALLTYPE mapCallTxt2Type(char* txt)
{
   if(cmsUtl_strcasecmp(txt, MDMVS_IN) == 0)
   {
      return INCOMING;
   }
   else if(cmsUtl_strcasecmp(txt, MDMVS_OUT) == 0)
   {
      return OUTGOING;
   }
   else if(cmsUtl_strcasecmp(txt, MDMVS_MISSED) == 0)
   {
      return MISSED;
   }
   return VOICE_CALL_TYPE_MAX;
}

#endif /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */
/*<END>================================= Common Helper Functions =========================================<END>*/

#endif /* BRCM_VOICE_SUPPORT */
