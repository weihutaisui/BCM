/*
*
*    Copyright (c) 2011-2012 Broadcom Corporation
*    All Rights Reserved
*
*  <:label-BRCM:2012:proprietary:standard
*  
*   This program is the proprietary software of Broadcom and/or its
*   licensors, and may only be used, duplicated, modified or distributed pursuant
*   to the terms and conditions of a separate, written license agreement executed
*   between you and Broadcom (an "Authorized License").  Except as set forth in
*   an Authorized License, Broadcom grants no license (express or implied), right
*   to use, or waiver of any kind with respect to the Software, and Broadcom
*   expressly reserves all rights in and to the Software and all intellectual
*   property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*   NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*   BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*  
*   Except as expressly set forth in the Authorized License,
*  
*   1. This program, including its structure, sequence and organization,
*      constitutes the valuable trade secrets of Broadcom, and you shall use
*      all reasonable efforts to protect the confidentiality thereof, and to
*      use this information only in connection with your use of Broadcom
*      integrated circuit products.
*  
*   2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*      AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*      WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*      RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*      ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*      FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*      COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*      TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*      PERFORMANCE OF THE SOFTWARE.
*  
*   3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*      ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*      INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*      WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*      IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*      OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*      SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*      SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*      LIMITED REMEDY.
*  :>
*/

/****************************************************************************
*
*  dal_voice.c
*
*  PURPOSE: Provide interface to the DAL functions related to voice configuration.
*
*  NOTES:
*
****************************************************************************/


#ifdef BRCM_VOICE_SUPPORT

/* ---- Include Files ---------------------------------------------------- */

#include <sched.h>
#include <pthread.h>

#include "cms_core.h"
#include "cms_dal.h"
#include "cms_cli.h"
#include "cms_util.h"
#include "cms_qos.h"
#include "cms_net.h"
#include "dal_voice.h"
#include "cms_msg.h"

#include <mdm.h>
#ifdef DMP_VOICE_SERVICE_1
#include "rut_voice.h"
#endif /* DMP_VOICE_SERVICE_1 */


/* Debug: Turn CMD parms dump on/off */
#define DALVOICE_DEBUG_CMD_PARMS    0

/*============================= TODOS =========================*/
/* TODO: Functions needs to change when multiple vp's are supported          */
/* TODO: Functions needs to change when line creatiion/deletion is supported */
/*============================= TODOS =========================*/

/* ---- Public Variables ------------------------------------------------- */
/* ---- Constants and Types ---------------------------------------------- */

#define ANY_INSTANCE             (-1)
#define TEMP_CHARBUF_SIZE        40
#define REGION_A3_CHARBUF_SIZE   4
#define LIST_CHAR_BUFFER         2048
#define MAX_NUM_CODECS           64

#define ZERO_ADDRESS_IPV4 "0.0.0.0"
#define ZERO_ADDRESS_IPV6 "::/128"

/* Minimum and maximum values for TX/RX gains */
#define MINGAIN -20
#define MAXGAIN  20

/* Gain is set in 0.1dB units */
#define GAINUNIT 10

typedef struct
{
   MdmObjectId id;                   /* MDM Object ID */
   int instId;                       /* Instance ID */
   InstanceIdStack iidStack;         /* Instance ID Stack */
}LEVELINFO;

/*============================= Helper Function Prototypes ===========================*/
#ifdef DMP_VOICE_SERVICE_1

/* Set helper functions */
#ifdef SIPLOAD
static CmsRet setVlSipURI ( unsigned int vpInst, unsigned int lineInst, const char * setVal );
static CmsRet setSipTransport ( unsigned int vpInst, char * setVal );
static CmsRet setBackToPrimOption ( unsigned int vpInst, char * setVal );
#endif /* SIPLOAD */
static CmsRet setVlCFFeatureValue( CFCALLTYPE callType, unsigned int vpInst, unsigned int lineInst, DAL_VOICE_FEATURE_CODE callFeatId, UINT32 setVal );
static void setVlCFFeatureField(CFCALLTYPE callType, VoiceLineCallingFeaturesObject *cfObject, DAL_VOICE_FEATURE_CODE callFeatId, UINT32 setVal);
static CmsRet setSrtpOption ( unsigned int vpInst, char * setVal );
static CmsRet setRedOption ( unsigned int vpInst, char * setVal );
static CmsRet setVlCLCodecList ( unsigned int vpInst, unsigned int lineInst, const char * setVal );
static CmsRet setVlCLPacketizationPeriod ( unsigned int vpInst, unsigned int lineInst, const char * setVal );
static CmsRet setVlCLSilenceSuppression ( unsigned int vpInst, unsigned int lineInst, unsigned int setVal );
static CmsRet setVlEnable ( unsigned int vpInst, unsigned int lineInst, unsigned int setVal );
static CmsRet setVlCFCallFwdNum ( unsigned int vpInst, unsigned int lineInst, char *setVal );

/* Get helper functions */
static CmsRet getVlCFFeatureValue(CFCALLTYPE callType, unsigned int vpInst, unsigned int lineInst, DAL_VOICE_FEATURE_CODE callFeatId, int *getVal );
static void getVlCFFeatureField(CFCALLTYPE callType, VoiceLineCallingFeaturesObject *cfObject, DAL_VOICE_FEATURE_CODE callFeatId, int* getVal);
#ifdef SIPLOAD
static CmsRet getVlCLCodecList( unsigned int vpInst, unsigned int lineInst, void *codec, unsigned int length );
#endif /* SIPLOAD */
static CmsRet getVlCLEncoder ( unsigned int vpInst, unsigned int lineInst, unsigned int priority, void *codec );

/* Dump functions */
static void dumpCmdParam( DAL_VOICE_PARMS *parms, char *value );
static CmsRet dumpSupportedCodecList( void );
static CmsRet dumpSupportedLocalesList( void );
static CmsRet dumpVoiceParams( void );
#ifdef SIPLOAD
static CmsRet dalVoice_DumpVlStatus(DAL_VOICE_PARMS *parms );
static CmsRet dumpVoiceStats( void );
#endif /* SIPLOAD */
#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
static CmsRet dumpPstnParams();
#endif

/* Mapping functions */
static CmsRet mapSpNumToVpInst ( int spNum, int * vpInst );
static CmsRet mapAcntNumToLineInst ( int vpInst, int acntNum, int * lineInst );
static CmsRet mapVpInstLineInstToCMAcnt( int vpInst, int lineInst, int * cmAcntNum );
static CmsRet validateCodec( char * codec, UBOOL8 * found);
static CmsRet validateAlpha2Locale( char * country, UBOOL8 * found );
static CmsRet mapAlpha2toAlpha3( char *locale, char *alpha3, UBOOL8 *found, unsigned int length );

/* Common helper functions */
static CmsRet getObj(LEVELINFO info[], InstanceIdStack *iidStack, UINT32 flags, void **obj);
#ifdef MGCPLOAD
static CmsRet getVpObj ( unsigned int vpInst, InstanceIdStack * iidStack, VoiceProfObject ** obj );
#endif
#if 0
#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
static CmsRet getPstnObj ( unsigned int pstnInst, InstanceIdStack * iidStack, VoicePstnObject ** obj );
#endif
static CmsRet getVpLineObj ( unsigned int vpInst, unsigned int LineInst, InstanceIdStack * iidStack, VoiceLineObject ** obj );
#endif
static int getIndexInCliCodecList( char ** codecList, const char * codec, int numSetCodec );
static void selectionSort(VoiceLineCodecListObject **codecList, unsigned int codecCount);
#ifdef DMP_BASELINE_1
static CmsRet performFilterOperation(InstanceIdStack *iidStack, DAL_VOICE_FIREWALL_CTL_BLK *fwCtlBlk, MdmObjectId oid, void **obj);
#endif
#if VOICE_IPV6_SUPPORT
static CmsRet dal_Voice_StripIpv6PrefixLength(VoiceObject *voiceObj, char *ipAddress);
#endif /* VOICE_IPV6_SUPPORT */

/*<START>===================================== DAL MGMT functions =====================================<START>*/


/***************************************************************************
* Function Name: dalVoice_voiceStop
* Description  : Stops voice application
*
* Parameters   : msgHandleArg - pointer to the calling app's message handle
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_voiceStop(void* msgHandleArg)
{
   CmsMsgHeader msgHdr = EMPTY_MSG_HEADER;
   CmsRet ret;

   cmsLog_debug( "Stop voice application" );

   msgHdr.dst = EID_SSK;
   msgHdr.src = cmsMsg_getHandleEid(msgHandleArg);
   msgHdr.type = CMS_MSG_DEINIT_VOICE;
   msgHdr.flags_event = 1;

   if(msgHandleArg == NULL)
   {
      cmsLog_error("msgHandleArg is NULL!");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else if ( (ret = cmsMsg_send(msgHandleArg, &msgHdr)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Could not send CMS_MSG_SHUTDOWN_VOICE msg to ssk, ret=%d", ret );
   }

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_voiceStart
* Description  : Starts voice application
*
* Parameters   : msgHandleArg - pointer to the calling app's message handle
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_voiceStart(void* msgHandleArg)
{
   CmsMsgHeader msgHdr = EMPTY_MSG_HEADER;
   CmsRet ret;

   /* We send a msg to the SSK instead of SMD, This message causes SSK to start voice by   *
    * sending msg to the SMD, the advantage here is that the code which handles checking   *
    * for LAN/WAN connections and passing the right IP address to voice which resides      *
    * within SSK is reused ;-)                                                            */

   cmsLog_debug( "Start voice application" );

   msgHdr.dst = EID_SSK;
   msgHdr.src = cmsMsg_getHandleEid(msgHandleArg);
   msgHdr.type = CMS_MSG_INIT_VOICE;
   msgHdr.flags_event = 1;

   if(msgHandleArg == NULL)
   {
      cmsLog_error("msgHandleArg is NULL!");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else if ( (ret = cmsMsg_send(msgHandleArg, &msgHdr)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Could not send CMS_MSG_INIT_VOICE msg to ssk, ret=%d", ret );
   }

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_voiceReboot
* Description  : Restarts voice application
*
* Parameters   : msgHandleArg - pointer to the calling app's message handle
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
#define VOICE_REBOOT_WAIT_SECONDS 10
CmsRet dalVoice_voiceReboot(void* msgHandleArg)
{
   CmsRet ret;
   int i = 0;
   char voiceStatus[MAX_TR104_OBJ_SIZE];
   DAL_VOICE_PARMS parms;

   cmsLog_debug("voice reboot requested");

   /* Setup parms */
   parms.msgHandle = msgHandleArg;

   /* De-initialize Voice */
   if((ret = dalVoice_voiceStop(msgHandleArg)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "dalVoice_voiceStop() Failed, ret=%d", ret );
      return ret;
   }

   for( i=0; i<VOICE_REBOOT_WAIT_SECONDS; i++ )
   {
      dalVoice_GetStatus( &parms, voiceStatus, MAX_TR104_OBJ_SIZE);

      /* Check if voice has completely de-initialized */
      if( atoi(voiceStatus) )
      {
         /* Delay of 1 sec */
         sleep(1);
      }
      else
      {
         break;
      }
   }

   if( i < VOICE_REBOOT_WAIT_SECONDS )
   {
      /* Re-Initialize Voice */
      if((ret = dalVoice_voiceStart(msgHandleArg)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "dalVoice_voiceStart() Failed, ret=%d", ret );
         return ret;
      }
   }
   else
   {
      cmsLog_error("voice reboot timedout after %d seconds", VOICE_REBOOT_WAIT_SECONDS);
   }

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_Save
* Description  : saves voice params to flash
*
* Parameters   : none
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_Save(void)
{
   CmsRet              ret;
   pthread_t           tid;
   int                 policy = SCHED_OTHER;
   int                 oldpriority = 0;
   int                 result;
   struct sched_param  param;

   /* Get pthread ID */
   tid = pthread_self();

   /* Get pthread's scheduling policy and parameters */
   if ( (result = pthread_getschedparam(tid, &policy, &param)) != 0 )
   {
      cmsLog_error( "pthread_getschedparam failed %d\n", result);
   }

   /* Downgrade thread's policy if it is a realtime thread */
   if ( policy == SCHED_FIFO || policy == SCHED_RR )
   {
      /* Save original priority and set priority to maximum priority allowed for SCHED_OTHER */
      oldpriority = param.__sched_priority;
      param.__sched_priority = sched_get_priority_max(SCHED_OTHER);
      if ( (result = pthread_setschedparam(tid, SCHED_OTHER, &param)) != 0 )
      {
         cmsLog_error( "pthread_setschedparam failed %d\n", result);
      }
   }

   if ( (ret = cmsMgm_saveConfigToFlash()) != CMSRET_SUCCESS )
   {
      cmsLog_error( "SaveConfigToFlash failed, ret=%d", ret);
   }
   else
   {
      cmsLog_debug( "Voice config written to flash\n" );
   }

   /* Upgrade thread's policy back to realtime */
   if ( policy == SCHED_FIFO || policy == SCHED_RR )
   {
      param.__sched_priority = oldpriority;
      if ( (result = pthread_setschedparam(tid, policy, &param)) != 0 )
      {
         cmsLog_error( "pthread_setschedparam failed %d\n", result);
      }
   }

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetDefaults
* Description  : Sets up default values to setup IAD in peer-peer mode.
*
* Parameters   : None ( parameters are ignored );
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
CmsRet dalVoice_SetDefaults( DAL_VOICE_PARMS *parms, char *value )
{
   DAL_VOICE_PARMS parmsList;
   char temp[TEMP_CHARBUF_SIZE];
#if VOICE_IPV6_SUPPORT
   char getVal[TEMP_CHARBUF_SIZE];
#ifdef SIPLOAD
   int isIpv6 = 0;
#endif /* SIPLOAD */
#endif /* VOICE_IPV6_SUPPORT */
   int numPhysFxs;
   int i,spNum,vpInst;
   int j,numAcc,lineInst;
   int numPhysFxo;
#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   int fxoInst;
#endif /* DMP_X_BROADCOM_COM_PSTNENDPOINT_1 */
#ifdef SIPLOAD
   int fxoAdded = 0;
#endif /* SIPLOAD */
   (void)parms;
   (void)value;

   memset( &parmsList, 0, sizeof(parmsList) );

#if VOICE_IPV6_SUPPORT
   dalVoice_GetIpFamily( &parmsList, (char*)getVal, TEMP_CHARBUF_SIZE);
#ifdef SIP_LOAD
   isIpv6 = !(cmsUtl_strcmp( getVal, MDMVS_IPV6 ));
#endif /* SIP_LOAD */
#endif /* VOICE_IPV6_SUPPORT */

   /* get maximum number of physical fxs endpoints */
   dalVoice_GetNumPhysFxsEndpt( &numPhysFxs );

   /* get maximum number of FXO endpoints */
   dalVoice_GetNumPhysFxoEndpt( &numPhysFxo );

   /* get maximum number of service providers configured */
   dalVoice_GetNumSrvProv( &spNum );

   for ( i = 0; i<spNum; i++ )
   {
      /* map serviceprovider index to voice profile */
      dalVoice_mapSpNumToVpInst( i, &vpInst );

      parmsList.op[0] = vpInst;

      dalVoice_SetBoundIfName( &parmsList, "LAN");
      dalVoice_SetRtpDSCPMark( &parmsList, "46");
      /* Set VP level params */
#ifdef SIPLOAD
      dalVoice_SetDigitMap( &parmsList, "[1-9]xxx|xx+*|xx+#|00x.T|011x.T|x+T");
      dalVoice_SetCriticalDigitTimer( &parmsList, "4000" );
      dalVoice_SetPartialDigitTimer( &parmsList, "16000" );
      dalVoice_SetSipUserAgentDomain( &parmsList, "" );
      dalVoice_SetSipUserAgentPort( &parmsList, "5060" );
      dalVoice_SetSipProxyServerPort( &parmsList, "5060" );
      dalVoice_SetSipOutboundProxyPort( &parmsList, "5060" );
      dalVoice_SetSipRegistrarServerPort( &parmsList, "5060" );
      dalVoice_SetSipSecDomainName( &parmsList, "" );
      dalVoice_SetSipSecProxyPort( &parmsList, "5060" );
      dalVoice_SetSipSecOutboundProxyPort( &parmsList, "5060");
      dalVoice_SetSipSecRegistrarPort( &parmsList, "5060" );
      dalVoice_SetSipRegisterExpires (&parmsList, "3600");
      dalVoice_SetSipRegisterRetryInterval( &parmsList, "20");
      dalVoice_SetSipConferencingURI( &parmsList, "");
      dalVoice_SetSipConferencingOption( &parmsList, MDMVS_LOCAL);
      dalVoice_SetSipDSCPMark( &parmsList, "46");
      dalVoice_SetCCTKDigitMap( &parmsList, "");
      dalVoice_SetSipBackToPrimOption( &parmsList, MDMVS_LOCAL);
      dalVoice_SetEuroFlashEnable( &parmsList, MDMVS_OFF);
      dalVoice_SetSipFailoverEnable(&parmsList, "No");
      dalVoice_SetSipOptionsEnable(&parmsList, "No");
      dalVoice_SetSipBackToPrimOption(&parmsList, MDMVS_DISABLED);

#if VOICE_IPV6_SUPPORT
      if ( isIpv6 )
      {
         dalVoice_SetSipProxyServer( &parmsList, ZERO_ADDRESS_IPV6);
         dalVoice_SetSipOutboundProxy( &parmsList, ZERO_ADDRESS_IPV6);
         dalVoice_SetSipRegistrarServer( &parmsList, ZERO_ADDRESS_IPV6);
         dalVoice_SetSipMusicServer( &parmsList, ZERO_ADDRESS_IPV6);
         dalVoice_SetLogServer( &parmsList, ZERO_ADDRESS_IPV6);
         dalVoice_SetSipSecProxyAddr( &parmsList, ZERO_ADDRESS_IPV6);
         dalVoice_SetSipSecOutboundProxyAddr( &parmsList, ZERO_ADDRESS_IPV6);
         dalVoice_SetSipSecRegistrarAddr( &parmsList, ZERO_ADDRESS_IPV6);
      }
      else
#endif /* VOICE_IPV6_SUPPORT */
      {
         dalVoice_SetSipProxyServer( &parmsList, ZERO_ADDRESS_IPV4);
         dalVoice_SetSipOutboundProxy( &parmsList, ZERO_ADDRESS_IPV4);
         dalVoice_SetSipRegistrarServer( &parmsList, ZERO_ADDRESS_IPV4);
         dalVoice_SetSipMusicServer( &parmsList, ZERO_ADDRESS_IPV4);
         dalVoice_SetLogServer( &parmsList, ZERO_ADDRESS_IPV4);
         dalVoice_SetSipSecProxyAddr( &parmsList, ZERO_ADDRESS_IPV4);
         dalVoice_SetSipSecOutboundProxyAddr( &parmsList, ZERO_ADDRESS_IPV4);
         dalVoice_SetSipSecRegistrarAddr( &parmsList, ZERO_ADDRESS_IPV4);
      }
#endif /* SIPLOAD */

#ifdef MGCPLOAD
      dalVoice_SetMgcpCallAgentIpAddress( &parmsList, "192.168.1.100");
      dalVoice_SetMgcpGatewayName( &parmsList, "BRCM_VOICE_GW_01");
#endif /* MGCPLOAD */
      char defaultlocale[4];
      rutVoice_getDefaultAlpha3Locale(defaultlocale,4);
      dalVoice_SetRegion( &parmsList,defaultlocale);
      /* Get max accounts per service provider */
      dalVoice_GetNumAccPerSrvProv( i, &numAcc );

      /* Disable SRTP by default */
      dalVoice_SetSrtpOption( &parmsList, "Disabled" );

#ifdef SIPLOAD
      /* Disable TLS by default */
      dalVoice_SetSipTransport( &parmsList, "UDP" );
#endif

      /* Attach FXS/FXO ports to lines */
      for( j = 0; j < numAcc; j++ )
      {
         /* map vpInst and account num to line instance */
         dalVoice_mapAcntNumToLineInst( vpInst, j, &lineInst );

         parmsList.op[1] = lineInst;

#ifdef SIPLOAD
         /* Assign actual FXO/FXS interface IDs */
         if( j && (numPhysFxo > 0) && j == (numAcc - 1) && !fxoAdded)
         {
            /* If last account ( and not the only) then assign FXO interface to it */
            snprintf( temp,TEMP_CHARBUF_SIZE, "%d", 0 );

            /* Add FXO endpoint to this specific line only. This is the FXO sip account */
            dalVoice_SetVlFxoPhyReferenceList( &parmsList, temp);
            fxoAdded = 1;
         }
         else
         {
            /* Assign FXS ports  in ( accNum % maxfxsports ) order */
            snprintf( temp,TEMP_CHARBUF_SIZE, "%d", (numPhysFxs > 0) ? (j % numPhysFxs) : (j) );
            dalVoice_SetVlPhyReferenceList( &parmsList, temp);

            /* Clear FXO port */
            temp[0]='\0';
            dalVoice_SetVlFxoPhyReferenceList( &parmsList, temp);
         }
#else
         /* Set line level params- assign endpoint ids */
         snprintf( temp,TEMP_CHARBUF_SIZE, "%d", j );
         dalVoice_SetVlPhyReferenceList( &parmsList, temp);
#endif /* SIPLOAD */

         /* Enable the line */
         dalVoice_SetVlEnable( &parmsList, "on" );

#ifdef SIPLOAD
         /* For all accounts with the new CM, disable all advanced features */
         dalVoice_SetVlCFAnonCallBlck( &parmsList, "off");
         dalVoice_SetVlCFAnonymousCalling( &parmsList, "off");

         dalVoice_SetVlCFMWIEnable( &parmsList, "off" );

         dalVoice_SetVlCFWarmLine ( &parmsList, "off" );
         dalVoice_SetVlCFWarmLineNum ( &parmsList, "" );

         dalVoice_SetVlCFCallFwdNum ( &parmsList, "" );
         dalVoice_SetVlCFCallFwdAll( &parmsList, "off" );
         dalVoice_SetVlCFCallFwdNoAns ( &parmsList, "off" );
         dalVoice_SetVlCFCallFwdBusy ( &parmsList, "off" );

         dalVoice_SetVlCFCallWaiting ( &parmsList, "on" );

         dalVoice_SetVlCFAnonCallBlck ( &parmsList, "off" );
         dalVoice_SetVlCFAnonymousCalling ( &parmsList, "on" );
         dalVoice_SetVlCFDoNotDisturb ( &parmsList, "off" );

         dalVoice_SetVlCFCallBarring ( &parmsList, "off" );
         dalVoice_SetVlCFCallBarringMode( &parmsList, "0" );
         dalVoice_SetVlCFCallBarringPin ( &parmsList, "9999" );
         dalVoice_SetVlCFCallBarringDigitMap ( &parmsList, "" );
#endif /* SIPLOAD */
      }
   }

#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   for ( i = 0; i < numPhysFxo; i++ )
   {
      dalVoice_mapCmPstnLineToPstnInst( i, &fxoInst);
      parmsList.op[0] = fxoInst;
      dalVoice_SetPstnDialPlan( &parmsList, "911|102");
   }
#endif /* DMP_X_BROADCOM_COM_PSTNENDPOINT_1 */

   return CMSRET_SUCCESS;
}

CmsRet dalVoice_GetStatus(DAL_VOICE_PARMS * parms, char* value, unsigned int length )
{
   CmsMsgHeader msgHdr = EMPTY_MSG_HEADER;
   CmsRet ret;

   (void)parms;

   msgHdr.dst = EID_SMD;
   msgHdr.src = cmsMsg_getHandleEid(parms->msgHandle);
   msgHdr.type = CMS_MSG_IS_APP_RUNNING;
   msgHdr.flags_request = 1;
   msgHdr.wordData = EID_VOICE;

   if(parms->msgHandle == NULL)
   {
      cmsLog_error("parms->msgHandle is NULL!");
      ret = CMSRET_INVALID_ARGUMENTS;
   }
   else
   {
      ret = cmsMsg_sendAndGetReply(parms->msgHandle, &msgHdr);
   }

   if ( ret == CMSRET_SUCCESS )
   {
      strncpy( value, "1", length );
   }
   else
   if ( ret == CMSRET_OBJECT_NOT_FOUND )
   {
      strncpy( value, "0", length );
   }
   else
   {
      cmsLog_error( "Could not send CMS_MSG_IS_APP_RUNNING msg to smd, ret=%d", ret );
   }

   return CMSRET_SUCCESS;
}


/*<END>===================================== DAL MGMT functions =====================================<END>*/

/*<START>===================================== CLI DAL get functions =====================================<START>*/

/***************************************************************************
* Function Name: dalVoice_cliDumpParams
* Description  : Dumps voice parameters on console
*
* Parameters   : none
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_cliDumpParams(void)
{
   return (dumpVoiceParams());
}
#ifdef SIPLOAD
/***************************************************************************
* Function Name: dalVoice_cliDumpStats
* Description  : Dumps voice call statistics on console
*
* Parameters   : none
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_cliDumpStats(void)
{
   return (dumpVoiceStats());
}
#endif /* SIPLOAD */
/*<END>===================================== CLI DAL get functions =======================================<END>*/

/*<START>===================================== DAL Set functions =====================================<START>*/

/*****************************************************************
**  FUNCTION:       dalVoice_SetLoggingLevel
**
**  PUROPOSE:
**
**  INPUT PARMS:    mdm log level for voice
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetLoggingLevel( DAL_VOICE_PARMS *parms, char * setVal )
{
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Only allow specific values */
   if (     cmsUtl_strcmp( setVal,MDMVS_ERROR  )
         && cmsUtl_strcmp( setVal,MDMVS_NOTICE )
         && cmsUtl_strcmp( setVal,MDMVS_DEBUG  )   )
   {
      cmsLog_error( "Invalid log level value\n" );
      return CMSRET_INVALID_PARAM_VALUE;
   }

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }
   else
   {
      REPLACE_STRING_IF_NOT_EQUAL(obj->X_BROADCOM_COM_LoggingLevel, setVal);
      if ( (ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "could not set voice obj, ret=%d", ret);
      }

      cmsObj_free((void **) &obj);

   }

   return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetModuleLoggingLevel
**
**  PURPOSE:        Set a specific module's logging level
**
**  INPUT PARMS:    module name, mdm log level for voice
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetModuleLoggingLevel( DAL_VOICE_PARMS *parms, char* modName, char* setVal )
{
   char *pMod, *pModAfter;
   unsigned int level;
   char modLevels[MAX_TR104_OBJ_SIZE];
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   level = atoi(setVal);
   if(level > 7)
   {
      cmsLog_error( "Invalid log level value. Must be between 0 and 7.\n" );
      return CMSRET_INVALID_PARAM_VALUE;
   }

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }

   pMod = cmsUtl_strstr(obj->X_BROADCOM_COM_ModuleLogLevels, modName);
   if(pMod)
   {
      pModAfter = cmsUtl_strstr(pMod, ",");
      *pMod = '\0';
      if(pModAfter)
      {
         pModAfter++;
         snprintf(modLevels, sizeof(modLevels), "%s%s,%s=%d", obj->X_BROADCOM_COM_ModuleLogLevels, pModAfter, modName, level);
      }
      else
      {
         snprintf(modLevels, sizeof(modLevels), "%s%s=%d", obj->X_BROADCOM_COM_ModuleLogLevels, modName, level);
      }
   }
   else
   {
      snprintf(modLevels, sizeof(modLevels), "%s,%s=%d", obj->X_BROADCOM_COM_ModuleLogLevels, modName, level);
   }
   REPLACE_STRING_IF_NOT_EQUAL(obj->X_BROADCOM_COM_ModuleLogLevels, modLevels);

   if ( (ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "could not set voice obj, ret=%d", ret);
   }

   cmsObj_free((void **) &obj);
   return (ret);
}


/*****************************************************************
**  FUNCTION:       dalVoice_SetModuleLoggingLevels
**
**  PURPOSE:        Set the entire log levels string
**
**  INPUT PARMS:    mdm log level string for all voice modules
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetModuleLoggingLevels( DAL_VOICE_PARMS *parms, char* setVal )
{
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }

   REPLACE_STRING_IF_NOT_EQUAL(obj->X_BROADCOM_COM_ModuleLogLevels, setVal);

   if ( (ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "could not set voice obj, ret=%d", ret);
   }

   cmsObj_free((void **) &obj);
   return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetManagementProtocol
**
**  PUROPOSE:       Track Protocol used to Manage Voice
**
**  INPUT PARMS:    Protocol Identifier
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetManagementProtocol(  DAL_VOICE_PARMS *parms, char * setVal )
{
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Only allow specific values */
   if (     cmsUtl_strcmp( setVal,MDMVS_TR69  )
         && cmsUtl_strcmp( setVal,MDMVS_OAM  )
         && cmsUtl_strcmp( setVal,MDMVS_OMCI  )   )
   {
      cmsLog_error( "Invalid Management Protocol\n" );
      return CMSRET_INVALID_PARAM_VALUE;
   }

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }
   else
   {
      REPLACE_STRING_IF_NOT_EQUAL(obj->X_BROADCOM_COM_ManagementProtocol, setVal);
      if ( (ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "could not set voice obj, ret=%d", ret);
      }

      cmsObj_free((void **) &obj);
   }

   return (ret);
}

#ifdef SIPLOAD
/*****************************************************************
**  FUNCTION:       dalVoice_SetCCTKTraceLevel
**
**  PUROPOSE:
**
**  INPUT PARMS:    cctk trace level
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCCTKTraceLevel(  DAL_VOICE_PARMS *parms, char * setVal )
{
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }
   else
   {
      REPLACE_STRING_IF_NOT_EQUAL(obj->X_BROADCOM_COM_CCTKTraceLevel, setVal);
      if ( (ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "could not set voice obj, ret=%d", ret);
      }

      cmsObj_free((void **) &obj);

   }

   return (ret);
}
#endif /* SIPLOAD */

#ifdef SIPLOAD
/*****************************************************************
**  FUNCTION:       dalVoice_SetCCTKTraceGroup
**
**  PUROPOSE:
**
**  INPUT PARMS:    cctk trace group
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCCTKTraceGroup(  DAL_VOICE_PARMS *parms, char * setVal )
{
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }
   else
   {
      REPLACE_STRING_IF_NOT_EQUAL(obj->X_BROADCOM_COM_CCTKTraceGroup, setVal);
      if ( (ret = cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "could not set voice obj, ret=%d", ret);
      }

      cmsObj_free((void **) &obj);

   }

   return (ret);
}
#endif /* SIPLOAD */

/*****************************************************************
**  FUNCTION:       dalVoice_SetBoundIfName
**
**  PUROPOSE:
**
**  INPUT PARMS:    bound ifname for  voice
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetBoundIfName( DAL_VOICE_PARMS *parms, char * setVal )
{
   VoiceObject *voiceObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug( "Value = %s", setVal );

   /* get and set the variable */

   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &voiceObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "could not get voice object, ret=%d", ret);
   }
   else
   {
      REPLACE_STRING_IF_NOT_EQUAL(voiceObj->X_BROADCOM_COM_BoundIfName, setVal);
      if ( (ret = cmsObj_set(voiceObj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "could not set voice obj, ret=%d", ret);
      }

      cmsObj_free((void **) &voiceObj);
   }


   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetBoundIPAddr
**
**  PUROPOSE:       Stores the specified bound IP address in MDM.
**
**  INPUT PARMS:    bound ipaddr for  voice
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetBoundIPAddr( DAL_VOICE_PARMS *parms, char *setVal )
{
   VoiceObject *voiceObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   /* get and set the variable */

   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &voiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "could not get voice object, ret=%d", ret);
   }
   else
   {
      REPLACE_STRING_IF_NOT_EQUAL(voiceObj->X_BROADCOM_COM_BoundIpAddr, setVal);
      if ( (ret = cmsObj_set(voiceObj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "could not set voice obj, ret=%d", ret);
      }

      cmsObj_free((void **) &voiceObj);
   }

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetIpFamily
**
**  PUROPOSE:       Stores the specified IP family list for voice
**
**  INPUT PARMS:    IP family for voice
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetIpFamily( DAL_VOICE_PARMS *parms, char *setVal )
{
   VoiceObject *voiceObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   /* get and set the variable */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &voiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "could not get voice object, ret=%d", ret);
   }
   else
   {
#if VOICE_IPV6_SUPPORT
      REPLACE_STRING_IF_NOT_EQUAL(voiceObj->X_BROADCOM_COM_IpAddressFamily, setVal);
#else
      REPLACE_STRING_IF_NOT_EQUAL(voiceObj->X_BROADCOM_COM_IpAddressFamily, MDMVS_IPV4);
#endif /* VOICE_IPV6_SUPPORT */

      if ( (ret = cmsObj_set(voiceObj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "could not set voice obj, ret=%d", ret);
      }

      cmsObj_free((void **) &voiceObj);
   }

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetDnsServerAddr
**
**  PUROPOSE:       Set IP address of the voice DNS server
**                  (X_BROADCOM_COM_VoiceDnsServer)
**
**  INPUT PARMS:    DNS IP addr for voice
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetDnsServerAddr ( DAL_VOICE_PARMS *parms, char *setVal )
{
   VoiceObject *voiceObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   /* get and set the variable */

   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &voiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "could not get voice object, ret=%d", ret);
   }
   else
   {
      REPLACE_STRING_IF_NOT_EQUAL(voiceObj->X_BROADCOM_COM_VoiceDnsServer, setVal);
      if ( (ret = cmsObj_set(voiceObj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "could not set voice obj, ret=%d", ret);
      }

      cmsObj_free((void **) &voiceObj);
   }

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetT38Enable
* Description  : Enable Fax
*                VoiceProfile.{i}.FaxT38.Enable == new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetT38Enable( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet                 ret;
   InstanceIdStack        iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfFaxT38Object *obj      = NULL;
   unsigned int           setValInt;
   unsigned int           vpInst   = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]     = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_FAX_T38, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", value ))
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", value ))
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", value );
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve fax T38 object \n" );
      return ( ret );
   }

   /* write new values */
   obj->enable = setValInt;
   cmsLog_debug( "T38 Enable = %u for [vpInst] = [%u]\n", setValInt, vpInst );
   if ( ( ret = cmsObj_set( obj,  &iidStack )) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set fax T38 ret = %d\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetV18Enable
* Description  : Enable V.18 detection
*                VoiceProfile.{i}.V18.Enable == new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetV18Enable( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet                 ret;
   InstanceIdStack        iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject       *obj      = NULL;
   unsigned int           setValInt;
   unsigned int           vpInst   = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]     = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", value ))
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", value ))
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", value );
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* write new values */
   obj->X_BROADCOM_COM_V18_Support = setValInt;

   cmsLog_debug( "V18 Enable = %u for [vpInst] = [%u]\n", setValInt, vpInst );
   if ( ( ret = cmsObj_set( obj,  &iidStack )) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set V18 ret = %d\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}


/***************************************************************************
* Function Name: dalVoice_SetRegion
* Description  : Set the region identically for all Voice Profiles
*                VoiceProfile.{i}.region = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetRegion( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj    = NULL;
   UBOOL8              found      = 0;
   char                region[REGION_A3_CHARBUF_SIZE]; /* To store alpha 2 or 3 code + null */

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */
   int vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* We expect either an alpha-3 or alpha-2 country code */
   strncpy ( region, value, strlen(value)+1 );

   dalVoice_mapCountryCode3To2( region ,&found, REGION_A3_CHARBUF_SIZE );

   if (!found)
   {
      /* Check if its Alpha-2 and VALID */
      validateAlpha2Locale( region, &found );
   }

   /* If still not found, value entered was not a valid Alpha-3 or Alpha-2 Locale */
   if (!found)
   {
      cmsLog_error( "Invalid region: %s\n",region );
      dumpSupportedLocalesList();
      return CMSRET_INVALID_ARGUMENTS;
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   cmsMem_free ( profObj->region );
   profObj->region = cmsMem_strdup ( region );

   cmsLog_debug( "Region == %s for [vpInst] = [%u]\n", region, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( profObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set region ret = %d\n", ret);
   }

   cmsObj_free((void **) &profObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetDTMFMethod
* Description  : Set the method by which DTMF digits must be passed
*                Set VoiceProfile.{i}.DTMFMethod
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDTMFMethod( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject     *profObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   int vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* validate the new value is an allowable value */
   if ( strcmp( value, MDMVS_RFC2833 ) && strcmp( value, MDMVS_INBAND ) && strcmp( value, MDMVS_SIPINFO ))
   {
      return ( CMSRET_INVALID_PARAM_NAME );
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free ( profObj->DTMFMethod );
   profObj->DTMFMethod = cmsMem_strdup ( value );

   cmsLog_debug( "DTMF Method = %s for [vpInst] = [%u]\n", value, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( profObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "setDTMFMethod: Can't set DTMF method ret = %d\n", ret);
   }

   cmsObj_free((void **) &profObj);

   return ret;
}


/***************************************************************************
* Function Name: dalVoice_SetDigitMap
* Description  : Set the method by which DTMF digits must be passed
*                Set VoiceProfile.{i}.digitMap
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetDigitMap( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK}, {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   REPLACE_STRING_IF_NOT_EQUAL( profObj->digitMap, value );

   /* enable digitMap flag */
   profObj->digitMapEnable = 1;

   cmsLog_debug( "Digit Map = %s for [vpInst] = [%u]\n", value, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( profObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Digit map ret = %d\n", ret);
   }

   cmsObj_free((void **) &profObj);

   return ret;
}
#ifdef SIPLOAD
/***************************************************************************
* Function Name: dalVoice_SetCCTKDigitMap
* Description  : Set the method by which DTMF digits must be passed (custom)
*                Set VoiceProfile.{i}.X_BROADCOM_COM_CCTK_DigitMap
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetCCTKDigitMap( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK}, {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   REPLACE_STRING_IF_NOT_EQUAL( profObj->X_BROADCOM_COM_CCTK_DigitMap, value );

   cmsLog_debug( "Digit Map = %s for [vpInst] = [%u]\n", value, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( profObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Digit map ret = %d\n", ret);
   }

   cmsObj_free((void **) &profObj);

   return ret;
}
#endif

/***************************************************************************
* Function Name: dalVoice_SetCriticalDigitTimer
* Description  : Set VoiceProfile.{i}.X_BROADCOM_COM_CriticalDigitTimer
*
* Parameters   : parms->op[0] = Voice Profile Instance
*                value        = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetCriticalDigitTimer ( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj    = NULL;

   dumpCmdParam(parms, value);

   unsigned int vpInst = parms->op[0];
   unsigned int setVal = atoi(value);

   if ( setVal == 0 )
   {
      cmsLog_error( "timer must be > 0\n");
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK}, {0, 0, EMPTY_INSTANCE_ID_STACK}, };
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "_get returned %d\n", ret );
      return ( ret );
   }

   profObj->X_BROADCOM_COM_CriticalDigitTimer = setVal;
   cmsLog_debug( "CriticalDigitTimer[%u] = %u\n", vpInst, setVal );

   if ( ( ret = cmsObj_set( profObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "_set returned %d\n", ret);
   }

   cmsObj_free((void **) &profObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetPartialDigitTimer
* Description  : Set VoiceProfile.{i}.X_BROADCOM_COM_PartialDigitTimer
*
* Parameters   : parms->op[0] = Voice Profile Instance
*                value        = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetPartialDigitTimer ( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj    = NULL;

   dumpCmdParam(parms, value);

   unsigned int vpInst = parms->op[0];
   unsigned int setVal = atoi(value);

   if ( setVal == 0 )
   {
      cmsLog_error( "timer must be > 0\n");
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK}, {0, 0, EMPTY_INSTANCE_ID_STACK}, };
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "_get returned %d\n", ret );
      return ( ret );
   }

   profObj->X_BROADCOM_COM_PartialDigitTimer = setVal;
   cmsLog_debug( "PartialDigitTimer[%u] = %u\n", vpInst, setVal );

   if ( ( ret = cmsObj_set( profObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "_set returned %d\n", ret);
   }

   cmsObj_free((void **) &profObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetHookFlashMethod
* Description  : set hook flash method
*                VoiceProfile.{i}.X_BROADCOM_COM_HookFlashMethod = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetHookFlashMethod( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject     *profObj   = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* validate the new value is an allowable value */
   if ( strcmp( value, MDMVS_NONE ) && strcmp( value, MDMVS_SIPINFO ))
   {
      cmsLog_error( "Invalide method %s\n", value);
      return ( CMSRET_INVALID_PARAM_NAME );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free ( profObj->X_BROADCOM_COM_HookFlashMethod );
   profObj->X_BROADCOM_COM_HookFlashMethod = cmsMem_strdup ( value );

   cmsLog_debug( "Hook flash method = %s for [vpInst] = [%u]\n", value, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( profObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Hook flash method ret = %d\n", ret);
   }

   cmsObj_free((void **) &profObj);

   return ret;
}

#ifdef STUN_CLIENT
/***************************************************************************
* Function Name: dalVoice_SetSTUNServer
* Description  : Set Domain name or IP address of the STUN server
*                Set VoiceProfile.{i}.STUNServer = newVal
*                Set VoiceProfile.{i}.STUNEnable = 1
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSTUNServer( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject     *profObj = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   unsigned int vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free ( profObj->STUNServer );
   profObj->STUNServer = cmsMem_strdup ( value );

   /* enable STUN Server Status */
   profObj->STUNEnable = 1;

   cmsLog_debug( "STUN Server = %s for [vpInst] = [%u]\n", value, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( profObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set STUN server ret = %d\n", ret );
   }

   cmsObj_free((void **) &profObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSTUNServerPort
* Description  : set the STUNServer port
*                VoiceProfile.{i}.X_BROADCOM_COM_STUNServerPort = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSTUNServerPort( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject     *profObj   = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);

   /* validate port is within 0 - 65535 */
   if ( setVal > 65535 )
   {
      cmsLog_error( "port=%i > 65535\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   profObj->X_BROADCOM_COM_STUNServerPort = setVal;

   cmsLog_debug( "STUN Server Port = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( profObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set STUN server port ret = %d\n", ret);
   }

   cmsObj_free((void **) &profObj);

   return ret;
}
#endif /* STUN_CLIENT */


#ifdef SIPLOAD

/***************************************************************************
* Function Name: dalVoice_SetLogServer
* Description  : set Host name or IP address of the log server
*                VoiceProfile.{i}.X_BROADCOM_COM_LogServer = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetLogServer( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject     *profObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( profObj->X_BROADCOM_COM_LogServer );
   profObj->X_BROADCOM_COM_LogServer = cmsMem_strdup ( value );

   cmsLog_debug( "Log Server = %s for [vpInst] = [%u]\n", value, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( profObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Log server ret = %d\n", ret);
   }

   cmsObj_free((void **) &profObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetLogServerPort
* Description  : set Host name or IP address of the log server port
*                VoiceProfile.{i}.X_BROADCOM_COM_LogServerPort = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetLogServerPort( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject     *profObj   = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);

   /* validate port is within 0 - 65535 */
   if ( setVal > 65535 )
   {
      cmsLog_error( "port=%i > 65535\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   profObj->X_BROADCOM_COM_LogServerPort = setVal;

   cmsLog_debug( "Log Server Port = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( profObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Log server port ret = %d\n", ret);
   }

   cmsObj_free((void **) &profObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipRegistrarServer
* Description  : set Host name or IP address of the SIP registrar server
*                VoiceProfile.{i}.Sip.RegistrarServer = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipRegistrarServer( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   unsigned int vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->registrarServer );
   sipObj->registrarServer = cmsMem_strdup ( value );

   cmsLog_debug( "SIP Registrar Server = %s for [vpInst] = [%u]\n", value, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Registrar server ret = %d", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipRegistrarServerPort
* Description  : set Host name or IP address of the SIP registrar server port
*                VoiceProfile.{i}.Sip.RegistrarServerPort = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipRegistrarServerPort( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);

   /* validate port is within 0 - 65535 */
   if ( setVal > 65535 )
   {
      cmsLog_error( "Port=%i > 65535\n", setVal );
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->registrarServerPort = setVal;

   cmsLog_debug( "SIP Registrar Server Port = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Registrar server port ret = %d\n", ret );
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipProxyServer
* Description  : set Host name or IP address of the SIP Proxy server
*                VoiceProfile.{i}.Sip.ProxyServer = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipProxyServer( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   unsigned int vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->proxyServer );
   sipObj->proxyServer = cmsMem_strdup ( value );

   cmsLog_debug( "SIP Proxy Server = %s for [vpInst] = [%u]\n", value, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Proxy server ret = %d\n", ret );
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipProxyServerPort
* Description  : set Host name or IP address of the SIP Proxy server port
*                VoiceProfile.{i}.Sip.ProxyServerPort = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipProxyServerPort( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);

   /* validate port is within 0 - 65535 */
   if ( setVal > 65535 )
   {
      cmsLog_error( "Port=%i > 65535\n", setVal );
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->proxyServerPort = setVal;

   cmsLog_debug( "SIP Proxy Server Port = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Proxy server port ret = %d\n", ret );
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipUserAgentDomain
* Description  : CPE domain string
*                VoiceProfile.{i}.Sip.UserAgentDomain = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipUserAgentDomain( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->userAgentDomain );
   sipObj->userAgentDomain = cmsMem_strdup ( value );

   cmsLog_debug( "SIP User Agent Domain = %s for [vpInst] = [%u]\n", value, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set User agent domain ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipUserAgentPort
* Description  : Port used for incoming call control signaling
*                VoiceProfile.{i}.Sip.ProxyServerPort = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipUserAgentPort( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);
   unsigned int vpInst = parms->op[0];

   /* validate port is within 0 - 65535 */
   if ( setVal > 65535 )
   {
      cmsLog_error( "port=%i > 65535\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->userAgentPort = setVal;

   cmsLog_debug( "SIP User Agent Port = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set User agent port ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipOutboundProxy
* Description  : Host name or IP address of the outbound proxy
*                VoiceProfile.{i}.Sip.OutboundProxy = new value
*                Current Implementation ignores 'lineInst' param because
*                the variable being set is global in Call Manager
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipOutboundProxy( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = {  {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->outboundProxy );
   sipObj->outboundProxy = cmsMem_strdup ( value );

   cmsLog_debug( "SIP Outbound Proxy Server = %s for [vpInst] = [%u]\n", value, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Outbound proxy ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipOutboundProxyPort
* Description  : Destination port to be used in connecting to the outbound proxy
*                VoiceProfile.{i}.Sip.OutboundProxyPort = new value
*                Current Implementation ignores 'lineInst' param because
*                the variable being set is global in Call Manager
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipOutboundProxyPort( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);

   /* validate port is within 0 - 65535 */
   if ( setVal > 65535 )
   {
      cmsLog_error( "port=%i > 65535\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = {  {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->outboundProxyPort = setVal;

   cmsLog_debug( "SIP Outbound Proxy Server Port = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Outbound proxy port ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipTimerB
* Description  : set SIP protocol timer B in milli-seconds
*                VoiceProfile.{i}.Sip.TimerB = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipTimerB( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);
   unsigned int vpInst = parms->op[0];

   /* must bigger than 0 */
   if ( setVal < 1 )
   {
      cmsLog_error( "period=%i < 1\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = {  {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->timerB = setVal;

   cmsLog_debug( "SIP Timer B = %u \n", setVal );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set timer B ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipTimerF
* Description  : set SIP protocol timer F in milli-seconds
*                VoiceProfile.{i}.Sip.TimerF = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipTimerF( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);
   unsigned int vpInst = parms->op[0];

   /* must bigger than 0 */
   if ( setVal < 1 )
   {
      cmsLog_error( "period=%i < 1\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = {  {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->timerF = setVal;

   cmsLog_debug( "SIP Timer F = %u \n", setVal );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Timer F ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}


/***************************************************************************
* Function Name: dalVoice_SetSipRegistrationPeriod
* Description  : Period over which the user agent must periodicallyregister, in seconds
*                VoiceProfile.{i}.Sip.RegistrationPeriod = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipRegistrationPeriod( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);
   unsigned int vpInst = parms->op[0];

   /* validate port is within [1:] */
   if ( setVal < 1 )
   {
      cmsLog_error( "period=%i < 1\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = {  {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->registrationPeriod = setVal;

   cmsLog_debug( "SIP Registration Period = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Registration period ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipRegisterRetryInterval
* Description  : Register retry interval, in seconds
*                VoiceProfile.{i}.Sip.RegisterRetryInterval = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipRegisterRetryInterval( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse number as int */
   int setVal = atoi(value);
   unsigned int vpInst = parms->op[0];

   /* validate period is within [1:] */
   if ( setVal < 1 )
   {
      cmsLog_error( "period=%i < 1\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = {  {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->registerRetryInterval = setVal;

   cmsLog_debug( "SIP Register retry interval = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Retry interval ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;}


/***************************************************************************
* Function Name: dalVoice_SetSipRegisterExpires
* Description  : Register request Expires header value, in seconds
*                VoiceProfile.{i}.Sip.RegisterExpires = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipRegisterExpires( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse number as int */
   int setVal = atoi(value);
   unsigned int vpInst = parms->op[0];

   /* validate time >= 0 */
   if ( setVal < 0 )
   {
      cmsLog_error( "input=%i < 0 \n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = {  {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->registerExpires = setVal;

   cmsLog_debug( "SIP User Agent Port = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Expires interval ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipDSCPMark
* Description  : Diffserv code point to be used for outgoing SIP signaling packets.
*                VoiceProfile.{i}.Sip.DSCPMark = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipDSCPMark( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse port number as unsigned unsigned int */
   unsigned long setVal = atol(value);
   unsigned int vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->DSCPMark = setVal;

   cmsLog_debug( "SIP DSCP Mark = %d for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set DSCP mark ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipTransport
* Description  : Transport protocol to be used in connecting to the SIP server
*                VoiceProfile.{i}.Sip.ProxyServerTransport = new value
*                VoiceProfile.{i}.Sip.RegistrarServerTransport = new value
*                VoiceProfile.{i}.Sip.UserAgentTransport = new value
*                We only support one protocol at a time, so we write to all
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipTransport( DAL_VOICE_PARMS *parms, char *value )
{
   char*          setVal = value;
   unsigned int   vpInst = parms->op[0];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* validate the new value is an allowable value */
   if ( !strcmp( setVal, MDMVS_UDP ) || !strcmp( setVal, MDMVS_TCP )
        || !strcmp( setVal, MDMVS_TLS ) || !strcmp( setVal, MDMVS_SCTP ))
   {
      /* A vpInst of 0 indicates that this parameter needs to be set for all    *
       * voice profiles because the equivalent callmanager parameter has global *
       * scope and cant be set per service provider or per line                 */
      return ( setSipTransport( vpInst, setVal ));
   }
   else   /* new value is not an allowed value; return error */
   {
      return ( CMSRET_INVALID_PARAM_NAME );
   }
}

/***************************************************************************
* Function Name: dalVoice_SetSipMusicServer
* Description  : set Host name or IP address of the music server
*                VoiceProfile.{i}.X_BROADCOM_COM_MusicServer = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipMusicServer( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->X_BROADCOM_COM_MusicServer );
   sipObj->X_BROADCOM_COM_MusicServer = cmsMem_strdup ( value );

   cmsLog_debug( "Music Server = %s for [vpInst] = [%u]\n", value, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Music server ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipConferencingURI
* Description  : set Host name or IP address of the conferencing server
*                VoiceProfile.{i}.SIP.X_BROADCOM_COM_ConferencingURI = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipConferencingURI( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->X_BROADCOM_COM_ConferencingURI );
   sipObj->X_BROADCOM_COM_ConferencingURI = cmsMem_strdup ( value );

   cmsLog_debug( "Conferencing Server = %s for [vpInst] = [%u]\n", value, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set conferencing server ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipConferencingOption
* Description  : set SIP conferencing option
*                VoiceProfile.{i}.SIP.X_BROADCOM_COM_ConferencingOption = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipConferencingOption( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* validate the new value is an allowable value */
   if ( !strcmp( value, MDMVS_LOCAL )
#ifdef VOICE_IMS_SUPPORT
        || !strcmp( value, MDMVS_REFERPARTICIPANTS )
        || !strcmp( value, MDMVS_REFERSERVER )
#endif
        )
   {
      /*  Get the SIP object */
      if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve SIP object \n" );
         return ( ret );
      }

      /* set the new value in local copy, after freeing old memory */
      cmsMem_free( sipObj->X_BROADCOM_COM_ConferencingOption );
      sipObj->X_BROADCOM_COM_ConferencingOption = cmsMem_strdup ( value );

      cmsLog_debug( "Conferencing Server = %s for [vpInst] = [%u]\n", value, vpInst);
      /* copy new value from local copy to MDM */
      if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set conferencing server ret = %d\n", ret);
      }

      cmsObj_free((void **) &sipObj);
      return ret;
   }
   else   /* new value is not an allowed value; return error */
   {
      return ( CMSRET_INVALID_PARAM_NAME );
   }
}

/***************************************************************************
* Function Name: dalVoice_SetSipMusicServerPort
* Description  : set Host name or IP address of the music server port
*                VoiceProfile.{i}.X_BROADCOM_COM_MusicServerPort = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipMusicServerPort( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);

   /* validate port is within 0 - 65535 */
   if ( setVal > 65535 )
   {
      cmsLog_error( "port=%i > 65535\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->X_BROADCOM_COM_MusicServerPort = setVal;

   cmsLog_debug( "Music Server Port = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Music server port ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipSecDomainName
* Description  : set IP address of the secondary domain name
*                VoiceProfile.{i}.X_BROADCOM_COM_SecondaryDomainName = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipSecDomainName( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->X_BROADCOM_COM_SecondaryDomainName );
   sipObj->X_BROADCOM_COM_SecondaryDomainName = cmsMem_strdup ( value );

   cmsLog_debug( "Secondary domain name= %s for [vpInst] = [%u]\n", value, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Secondary domain name ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipSecProxyAddr
* Description  : set IP address of the secondary proxy
*                VoiceProfile.{i}.X_BROADCOM_COM_SecondaryProxyAddress = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipSecProxyAddr( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->X_BROADCOM_COM_SecondaryProxyAddress );
   sipObj->X_BROADCOM_COM_SecondaryProxyAddress = cmsMem_strdup ( value );

   cmsLog_debug( "Secondary proxy address= %s for [vpInst] = [%u]\n", value, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Secondary proxy address ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipSecProxyPort
* Description  : set the port value of the secondary proxy
*                VoiceProfile.{i}.X_BROADCOM_COM_SecondaryProxyPort = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipSecProxyPort( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);

   /* validate port is within 0 - 65535 */
   if ( setVal > 65535 )
   {
      cmsLog_error( "port=%i > 65535\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->X_BROADCOM_COM_SecondaryProxyPort = setVal;

   cmsLog_debug( "Secondary proxy port = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Secondary proxy port ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipSecOutboundProxyAddr
* Description  : set IP address of the secondary outbound proxy
*                VoiceProfile.{i}.X_BROADCOM_COM_SecondaryOutboundProxyAddress = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipSecOutboundProxyAddr( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->X_BROADCOM_COM_SecondaryOutboundProxyAddress );
   sipObj->X_BROADCOM_COM_SecondaryOutboundProxyAddress = cmsMem_strdup ( value );

   cmsLog_debug( "Secondary outbound proxy address= %s for [vpInst] = [%u]\n", value, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Secondary outbound proxy address ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipSecOutboundProxyPort
* Description  : set the port value of the secondary outbound proxy
*                VoiceProfile.{i}.X_BROADCOM_COM_SecondaryOutboundProxyPort = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipSecOutboundProxyPort( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);

   /* validate port is within 0 - 65535 */
   if ( setVal > 65535 )
   {
      cmsLog_error( "port=%i > 65535\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->X_BROADCOM_COM_SecondaryOutboundProxyPort = setVal;

   cmsLog_debug( "Secondary outbound proxy port = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Secondary outbound proxy port ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipSecRegistrarAddr
* Description  : set IP address of the secondary registrar
*                VoiceProfile.{i}.X_BROADCOM_COM_SecondaryRegistrarAddress = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipSecRegistrarAddr( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->X_BROADCOM_COM_SecondaryRegistrarAddress );
   sipObj->X_BROADCOM_COM_SecondaryRegistrarAddress = cmsMem_strdup ( value );

   cmsLog_debug( "Secondary registrar address= %s for [vpInst] = [%u]\n", value, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Secondary registrar address ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipSecRegistrarPort
* Description  : set the port value of the secondary registrar
*                VoiceProfile.{i}.X_BROADCOM_COM_SecondaryRegistrarPort = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipSecRegistrarPort( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);

   /* validate port is within 0 - 65535 */
   if ( setVal > 65535 )
   {
      cmsLog_error( "port=%i > 65535\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   sipObj->X_BROADCOM_COM_SecondaryRegistrarPort = setVal;

   cmsLog_debug( "Secondary registrar port = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Secondary registrar port ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipToTagMatching
* Description  : set value of SIP to tag matching
*                VoiceProfile.{i}.X_BROADCOM_COM_ToTagMatching = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipToTagMatching( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        setVal;
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   UINT8               isOff;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   if ( (isOff = strncasecmp(value, MDMVS_ON, strlen(MDMVS_ON))) &&
        strncasecmp(value, MDMVS_OFF, strlen(MDMVS_OFF)) )
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   setVal = isOff ?0 :1;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   sipObj->X_BROADCOM_COM_ToTagMatching = setVal;

   cmsLog_debug( "To Tag Matching = %d for [vpInst] = [%u]\n", setVal, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set SIP to tag matching ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipFailoverEnable
* Description  : set value of SIP failover enable flag
*                VoiceProfile.{i}.X_BROADCOM_COM_SipFailoverEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipFailoverEnable( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        setVal;
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   UINT8               isOff;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   if ( (isOff = strncasecmp(value, MDMVS_ON, strlen(MDMVS_ON))) &&
        strncasecmp(value, MDMVS_OFF, strlen(MDMVS_OFF)) )
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   setVal = isOff ?0 :1;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   sipObj->X_BROADCOM_COM_SipFailoverEnable = setVal;

   cmsLog_debug( "SIP failover = %d for [vpInst] = [%u]\n", setVal, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set SIP failover enable ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipOptionsEnable
* Description  : set value of SIP OPTIONS ping enable flag
*                VoiceProfile.{i}.X_BROADCOM_COM_SipOptionsEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst,
*                value = value to be set. One of:
*                MDMVS_ON  = "ON"
*                MDMVS_OFF = "OFF"
* Returns      : CMSRET_SUCCESS when successful.
*                CMSRET_INVALID_ARGUMENTS if value is unrecognized.
*                CMSRET error code if error as defined in cms.h
****************************************************************************/
CmsRet dalVoice_SetSipOptionsEnable( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        setVal;
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   UINT8               isOff;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   if ( (isOff = strncasecmp(value, MDMVS_ON, strlen(MDMVS_ON))) &&
        strncasecmp(value, MDMVS_OFF, strlen(MDMVS_OFF)) )
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   setVal = isOff ?0 :1;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   sipObj->X_BROADCOM_COM_SipOptionsEnable = setVal;

   cmsLog_debug( "SIP OPTIONS = %d for [vpInst] = [%u]\n", setVal, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set SIP OPTIONS enable ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipBackToPrimOption
* Description  : SRTP Protocol Usage Option (mandatory, optional or disabled)
*                VoiceService.{i}.VoiceProfile.{i}.SIP.X_BROADCOM_COM_BackToPrimMode = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipBackToPrimOption( DAL_VOICE_PARMS *parms, char *value )
{
   char*          setVal = value;
   unsigned int   vpInst = parms->op[0];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* validate the new value is an allowable value */
   if ( !strcmp( setVal, MDMVS_SILENT ) || !strcmp( setVal, MDMVS_DEREGISTRATION )
          || !strcmp( setVal, MDMVS_SILENTDEREGISTRATION ) || !strcmp( setVal, MDMVS_DISABLED ))
   {
      /* A vpInst of 0 indicates that this parameter needs to be set for all    *
       * voice profiles because the equivalent callmanager parameter has global *
       * scope and cant be set per service provider or per line                 */
      return ( setBackToPrimOption( vpInst, setVal ));
   }
   else   /* new value is not an allowed value; return error */
   {
      return ( CMSRET_INVALID_PARAM_NAME );
   }
}

#endif /* SIPLOAD */

/***************************************************************************
* Function Name: dalVoice_SetSrtpOption
* Description  : SRTP Protocol Usage Option (mandatory, optional or disabled)
*                VoiceProfile.{i}.RTP.SRTP.X_BROADCOM_COM_SrtpUsageOption = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSrtpOption( DAL_VOICE_PARMS *parms, char *value )
{
   char*          setVal = value;
   unsigned int   vpInst = parms->op[0];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* validate the new value is an allowable value */
   if ( !strcmp( setVal, MDMVS_MANDATORY ) || !strcmp( setVal, MDMVS_OPTIONAL )
        || !strcmp( setVal, MDMVS_DISABLED ))
   {
      /* A vpInst of 0 indicates that this parameter needs to be set for all    *
       * voice profiles because the equivalent callmanager parameter has global *
       * scope and cant be set per service provider or per line                 */
      return ( setSrtpOption( vpInst, setVal ));
   }
   else   /* new value is not an allowed value; return error */
   {
      return ( CMSRET_INVALID_PARAM_NAME );
   }
}


/***************************************************************************
* Function Name: dalVoice_SetRedOption
* Description  : RFC2198 Usage Option (Disabled, 1, 2, 3)
*                VoiceProfile.{i}.RTP.Redundancy = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetRedOption( DAL_VOICE_PARMS *parms, char *value )
{
   char*          setVal = value;
   unsigned int   vpInst = parms->op[0];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* validate the new value is an allowable value */
   if ( !strcmp( setVal, "1") || !strcmp( setVal, "2") || !strcmp( setVal, "3")
        || !strcmp( setVal, MDMVS_DISABLED ))
   {
      /* A vpInst of 0 indicates that this parameter needs to be set for all    *
       * voice profiles because the equivalent callmanager parameter has global *
       * scope and cant be set per service provider or per line                 */
      return ( setRedOption( vpInst, setVal ));
   }
   else   /* new value is not an allowed value; return error */
   {
      return ( CMSRET_INVALID_PARAM_NAME );
   }
}


/***************************************************************************
* Function Name: dalVoice_SetVlEnable
* Description  : CLI wrapper for setVlEnable
*                Enable voice line, will also enable vprofile if disabled
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlEnable( DAL_VOICE_PARMS *parms, char *value )
{
   const char * setVal = value;
   unsigned int setValInt;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];
   /* parse Account Number as unsigned int */
   unsigned int lineInst = parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   /* wrapper has set up the required variables, now call inner function */
   return ( setVlEnable( vpInst, lineInst, setValInt ));
}

/***************************************************************************
* Function Name: dalVoice_SetVlPhyReferenceList
* Description  : A comma separated list of Physical Interface
*                Identifiers that this Line is associated with
*                VoiceProfile.{i}.Line{i}.PhyReferenceList = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlPhyReferenceList( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   int                 numFXS     = 0;
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineObject    *lineObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };
   if(value == NULL)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* Get max number of physical FXS endpoints */
   if( ( ret = dalVoice_GetNumPhysFxsEndpt( &numFXS ) ) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Couldn't retrieve number of FXS endpoints, ret = %d\n", ret);
      return ( ret );
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&lineObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line instance object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   REPLACE_STRING_IF_NOT_EQUAL( lineObj->phyReferenceList, value );

   /* the NonFxsPhyInterface and phyReferenceList is mutually exclusive */
   if( strlen(value) && !strcasecmp(lineObj->X_BROADCOM_COM_AssociatedNonFxsPhyIntfType, MDMVS_FXO))
   {
      CMSMEM_REPLACE_STRING(lineObj->X_BROADCOM_COM_AssociatedNonFxsPhyIntfType, MDMVS_UNASSIGNED);
      CMSMEM_FREE_BUF_AND_NULL_PTR(lineObj->X_BROADCOM_COM_NonFxsPhyReferenceList);
   }

   cmsLog_debug( "Physical Reference List = %s for [vpInst, lineInst] = [%u, %u]\n", value, vpInst, lineInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( lineObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Phy reference list ret = %d\n", ret);
   }

   cmsObj_free((void **) &lineObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlFxoPhyReferenceList
* Description  : A comma separated list of Physical Interface
*                Identifiers that this Line is associated with
*                VoiceProfile.{i}.Line{i}.PhyReferenceList = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlFxoPhyReferenceList( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   int                 numFXO     = 0;
   int                 valueInt   = 0;
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineObject    *lineObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* Get max number of physical FXS endpoints */
   if( ( ret = dalVoice_GetNumPhysFxoEndpt( &numFXO ) ) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Couldn't retrieve number of FXS endpoints, ret = %d\n", ret);
      return ( ret );
   }

   valueInt = atoi ( value );
   /* Check if valid value passed in */
   if( numFXO && ( 0 > valueInt || valueInt >= numFXO ) )
   {
      cmsLog_error( "Invalid value: %s, must be between 0 and %d, inclusive \n", value, numFXO-1 );
      ret = CMSRET_INVALID_PARAM_VALUE;
      return ( ret );
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&lineObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line instance object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   CMSMEM_REPLACE_STRING(lineObj->X_BROADCOM_COM_NonFxsPhyReferenceList,( (numFXO)?value:"" ) );

   /* Set Assocated nonFxs phy interface as FXO */
   if( strlen(value) && numFXO )
   {
      CMSMEM_REPLACE_STRING(lineObj->X_BROADCOM_COM_AssociatedNonFxsPhyIntfType, MDMVS_FXO);

      /* the NonFxsPhyInterface and phyReferenceList is mutually exclusive */
      CMSMEM_FREE_BUF_AND_NULL_PTR( lineObj->phyReferenceList );
   }
   else
   {
      CMSMEM_REPLACE_STRING(lineObj->X_BROADCOM_COM_AssociatedNonFxsPhyIntfType, MDMVS_UNASSIGNED);
   }

   cmsLog_debug( "FXO Physical Reference List = %s for [vpInst, lineInst] = [%u, %u]\n", value, vpInst, lineInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( lineObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Phy reference list ret = %d\n", ret);
   }

   cmsObj_free((void **) &lineObj);

   return ret;
}




#ifdef SIPLOAD

/***************************************************************************
* Function Name: dalVoice_SetVlSipAuthUserName
* Description  : Set the SIP Authentication Username for a specified line
*                VoiceProfile.{i}.Line{i}.Sip.AuthUserName = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlSipAuthUserName( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineSipObject  *sipObj = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];
   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->authUserName );
   sipObj->authUserName = cmsMem_strdup ( value );

   cmsLog_debug( "SIP Authentication Name = %s for [vpInst, lineInst] = [%u, %u]\n", value, vpInst, lineInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set SIP auth name ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlSipAuthPassword
* Description  : Set the SIP Authentication Username for a specified line
*                VoiceProfile.{i}.Line{i}.Sip.AuthUserName = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlSipAuthPassword( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineSipObject  *sipObj = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];
   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];


   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->authPassword );
   sipObj->authPassword = cmsMem_strdup ( value );

   cmsLog_debug( "SIP Authentication Password = %s for [vpInst, lineInst] = [%u, %u]\n", value, vpInst, lineInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set SIP auth password ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}


/***************************************************************************
* Function Name: dalVoice_SetVlSipURI
* Description  : URI by which the user agent will identify itself for this line
*                VoiceProfile.{i}.Line{i}.Sip.URI = new value
*                VoiceProfile.{i}.Line.{i}.DirectoryNumber = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlSipURI( DAL_VOICE_PARMS *parms, char *value )
{
   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];
   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   const char * setVal = value;

   return ( setVlSipURI( vpInst, lineInst, setVal ));
}
#endif /* SIPLOAD */
/***************************************************************************
* Function Name: dalVoice_SetVlCFCallFwdAll
* Description  : Enable or disable callforward unconditional by the endpoint
*                VoiceProfile.{i}.Line{i}.CallingFeatures.CallForwardUnconditionalEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallFwdAll( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst = parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_FWD_ALL, setValInt));
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallFwdNoAns
* Description  : Enable or disable call forward on no answer by the endpoint
*                VoiceProfile.{i}.Line{i}.CallingFeatures.CallForwardOnNoAnswerEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallFwdNoAns( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine==%s\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_FWD_NOANS, setValInt));
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallFwdBusy
* Description  : Enable or disable call forward on busy by the endpoint
*                VoiceProfile.{i}.Line{i}.CallingFeatures.CallForwardOnBusyEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallFwdBusy( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine==%s\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_FWD_BUSY, setValInt));
}

/***************************************************************************
** Function Name: dalVoice_SetVlCFCallFwdNum
** Description  : Set a call forwarding number for a given line
**
** Parameters   : vpInst              - parms->op[0]
**                lineInst            - parms->op[1]
**                call forward number - value
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallFwdNum( DAL_VOICE_PARMS *parms, char * value )
{
   unsigned int       vpInst   = parms->op[0];
   unsigned int       lineInst = parms->op[1];
   CmsRet             ret;

   cmsLog_debug( "Line:%d \n",parms->op[1] );

   if( (ret = setVlCFCallFwdNum(vpInst, lineInst, value)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set call forwarding number\n" );
      return ret;
   }

   return ret;
}

#ifdef SIPLOAD

/***************************************************************************
** Function Name: dalVoice_SetVlCFWarmLineNum
** Description  : Set a warm line number for a given line
**
** Parameters   : vpInst              - parms->op[0]
**                lineInst            - parms->op[1]
**                warm line number - value
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFWarmLineNum( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int       vpInst   = parms->op[0];
   unsigned int       lineInst = parms->op[1];
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "Line:%d \n",parms->op[1] );

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( voiceLineCallingFeaturesObj->X_BROADCOM_COM_WarmLineNumber );
   voiceLineCallingFeaturesObj->X_BROADCOM_COM_WarmLineNumber = cmsMem_strdup ( value );

   cmsLog_debug( "Warm Line Num == %s for [vpInst, lineInst] = [%u, %u]\n", value, vpInst, lineInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( voiceLineCallingFeaturesObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Warm line number ret = %d\n", ret);
   }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallWaiting
* Description  : Enable or disable call waiting by the endpoint
*                VoiceProfile.{i}.Line{i}.CallingFeatures.CallWaitingEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallWaiting( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine==%s\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_CALLWAIT, setValInt));
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFAnonCallBlck
* Description  : Enable or disable Anonymous call blocking by the endpoint
*                VoiceProfile.{i}.Line{i}.CallingFeatures.MWIEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFAnonCallBlck( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine==%s\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_ANON_REJECT, setValInt));
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFAnonymousCalling
* Description  : Enable or disable anonymous calling by the endpoint
*                VoiceProfile.{i}.Line{i}.CallingFeatures.AnonymousCallEnable = !(new value)
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFAnonymousCalling( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine==%s\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_ANON_CALL, setValInt));
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFDoNotDisturb
* Description  : Enable or disable do not distrub feature
*                VoiceProfile.{i}.Line{i}.CallingFeatures.DoNotDisturbEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFDoNotDisturb( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine==%s\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_DND, setValInt));
}


/***************************************************************************
* Function Name: dalVoice_SetVlCFCallCompletionOnBusy
* Description  : Enable or disable call completion on busy
*                VoiceProfile.{i}.Line{i}.CallingFeatures.X_BROADCOM_COM_CAllCompletionOnBusyEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallCompletionOnBusy( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", setVal );
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_CCBS, setValInt));
}


/***************************************************************************
* Function Name: dalVoice_SetVlCFSpeedDial
* Description  : Enable or disable call completion on busy
*                VoiceProfile.{i}.Line{i}.CallingFeatures.X_BROADCOM_COM_SpeedDialEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFSpeedDial( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine==%s\n", setVal );
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_SPEEDDIAL, setValInt));
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFWarmLine
* Description  : Enable or disable warm line
*                VoiceProfile.{i}.Line{i}.CallingFeatures.X_BROADCOM_COM_WarmLineEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFWarmLine( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine==%s\n", setVal );
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_WARM_LINE, setValInt));
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFVisualMWI
* Description  : Enable or disable visual message waiting indication feature
*                VoiceProfile.{i}.Line{i}.CallingFeatures.X_BROADCOM_COM_VMWIEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFVisualMWI( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;
   unsigned int setValInt;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                    {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                    {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                    {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", value ))
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", value ))
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n",  value );
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   voiceLineCallingFeaturesObj->X_BROADCOM_COM_VMWIEnable = setValInt;

   if ( (ret = cmsObj_set(voiceLineCallingFeaturesObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "could not save call feature object ret = %d\n", ret);
   }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);
   return ret;

}

/***************************************************************************
* Function Name: dalVoice_SetVlCFNetworkPrivacy
* Description  : Enable or disable network privacy feature
*                VoiceProfile.{i}.Line{i}.CallingFeatures.X_BROADCOM_COM_NetworkPrivacyEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFNetworkPrivacy( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine==%s\n", setVal );
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_NET_PRIV, setValInt));
}

/***************************************************************************
** Function Name: dalVoice_SetVlCFFeatureStarted
** Description  : Activate/Deactivate a call feature
**
** Parameters   : vpInst              - parms->op[0]
**                lineInst            - parms->op[1]
**                call feature ID     - parms->op[2]
**                call feature value  - value
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFFeatureStarted( DAL_VOICE_PARMS *parms, char * value )
{
   unsigned int       vpInst, lineInst, featId, featValue;
   CmsRet             ret;

   cmsLog_debug( "Line:%d \n",parms->op[1]);

   vpInst = parms->op[0];
   lineInst = parms->op[1];
   featId = (DAL_VOICE_FEATURE_CODE)parms->op[2];
   featValue = atoi(value);

   if( (ret = setVlCFFeatureValue(CFCALLTYPE_STARTED, vpInst, lineInst, featId, featValue)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set call feature STARTED value\n" );
      return ret;
   }

   return ret;
}

/***************************************************************************
** Function Name: dalVoice_SetVlCFFeatureEnabled
** Description  : Enable/Disable a call feature
**
** Parameters   : vpInst              - parms->op[0]
**                lineInst            - parms->op[1]
**                call feature ID     - parms->op[2]
**                call feature value  - value
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFFeatureEnabled( DAL_VOICE_PARMS *parms, char * value )
{
   unsigned int       vpInst, lineInst, featId, featValue;
   CmsRet             ret;

   cmsLog_debug( "Line:%d \n",parms->op[1] );

   vpInst = parms->op[0];
   lineInst = parms->op[1];
   featId = (DAL_VOICE_FEATURE_CODE)parms->op[2];
   featValue = atoi(value);

   if( (ret = setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, featId, featValue)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set call feature ENABLED value\n" );
      return ret;
   }

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallBarring
* Description  : Enable or disable call barring feature
*                VoiceProfile.{i}.Line{i}.CallingFeatures.X_BROADCOM_COM_CallBarringEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallBarring( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   const char * setVal = value;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];

   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine==%s\n", setVal );
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, DAL_VOICE_FEATURE_CODE_CALL_BARRING, setValInt));
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallBarringMode
* Description  : Set the Barring mode to none (0), all (1), or per digit map (2)
*                VoiceProfile.{i}.Line{i}.CallingFeatures.X_BROADCOM_COM_CallBarringMode = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallBarringMode( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                    {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                    {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                    {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringMode );
   voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringMode = cmsMem_strdup ( value );

   cmsLog_debug( "Call barring mode == %s for [vpInst, lineInst] = [%u, %u]\n", value, vpInst, lineInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( voiceLineCallingFeaturesObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Call barring mode ret = %d\n", ret);
   }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallBarringPin
* Description  : Call barring pin number
*                VoiceProfile.{i}.Line{i}.CallingFeatures.X_BROADCOM_COM_CallBarringUserPin = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallBarringPin( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];
   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                    {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                    {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                    {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   if ( !atoi(value) && strcmp(value,"0") )
   {
      cmsLog_error( "callBarrPin must be an integer value\n" );
      ret = CMSRET_INVALID_PARAM_TYPE;
   }
   else
   {
      dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

      /*  Get the Voice Profile object */
      if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve calling features object \n" );
         return ( ret );
      }

      /* set the new value in local copy, after freeing old memory */
      cmsMem_free( voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringUserPin );
      voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringUserPin = cmsMem_strdup ( value );

      cmsLog_debug( "Call barring pin == %s for [vpInst, lineInst] = [%u, %u]\n", value, vpInst, lineInst );
      /* copy new value from local copy to MDM */
      if ( ( ret = cmsObj_set( voiceLineCallingFeaturesObj,  &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set Call barring PIN ret = %d\n", ret);
      }

      cmsObj_free((void **) &voiceLineCallingFeaturesObj);
   }
   return ret;
}

#endif /* SIPLOAD */

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallBarringDigitMap
* Description  : Set the method by which DTMF digits must be passed for call barring
*                Set VoiceProfile.{i}.digitMap
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallBarringDigitMap ( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];
   /* parse Account Number as unsigned int */
   unsigned int lineInst = parms->op[1];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringDigitMap );
   voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringDigitMap = cmsMem_strdup ( value );

   cmsLog_debug( "Call barring digit map = %s for [vpInst, lineInst] = [%u, %u]\n", value, vpInst, lineInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( voiceLineCallingFeaturesObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Call barring digit map ret = %d\n", ret);
   }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallerIDName
* Description  : String used to identify the caller also SIP display name
*                VoiceProfile.{i}.Line{i}.CallingFeatures.CallerIDName = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallerIDName( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( voiceLineCallingFeaturesObj->callerIDName );
   voiceLineCallingFeaturesObj->callerIDName = cmsMem_strdup ( value );

    cmsLog_debug( "Caller ID = %s for [vpInst, lineInst] = [%u, %u]\n", value, vpInst, lineInst );
    /* copy new value from local copy to MDM */
    if ( ( ret = cmsObj_set( voiceLineCallingFeaturesObj, &iidStack)) != CMSRET_SUCCESS )
    {
       cmsLog_error( "Can't set Caller ID ret = %d\n", ret);
    }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ret;
}


/***************************************************************************
* Function Name: dalVoice_SetVlCFMWIEnable
* Description  : Enable or disable Message Waiting Indication by the endpoint
*                VoiceProfile.{i}.Line{i}.CallingFeatures.MWIEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFMWIEnable( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   unsigned int        setValInt;
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* parse setVal as unsigned int */
   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", value ))
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", value ))
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", value);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   voiceLineCallingFeaturesObj->MWIEnable = setValInt;

   cmsLog_debug( "MWIEnable = %u for [vpInst, lineInst] = [%u, %u]\n", setValInt, vpInst, lineInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( voiceLineCallingFeaturesObj, &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set MWIEnable ret = %d\n", ret);
   }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlVPReceiveGain
* Description  : Gain in units of 0.1 dB to apply to the received
*                voice signal after decoding.
*                VoiceProfile.{i}.Line{i}.VoiceProcessing.ReceiveGain = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlVPReceiveGain( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineProcessingObject *voiceLineProcessingObj = NULL;
   unsigned int vpInst = parms->op[0];
   unsigned int lineInst = parms->op[1];
   int setVal = atoi(value);

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* Check if gain is valid */
   if ( setVal > MAXGAIN || setVal < MINGAIN )
   {
      cmsLog_error( "gain=%i is invalid. Valid range: %d <= gain <= %d\n", setVal, MINGAIN, MAXGAIN);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_PROCESSING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineProcessingObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line processing object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   voiceLineProcessingObj->receiveGain = setVal * GAINUNIT;

   cmsLog_debug( "Receive Gain = %i units of 0.1 dB for [vpInst, lineInst] = [%u, %u]\n", setVal, vpInst, lineInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( voiceLineProcessingObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Receive gain ret = %d\n", ret);
   }

   cmsObj_free((void **) &voiceLineProcessingObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlVPTransmitGain
* Description  : Gain in units of 0.1 dB to apply to the transmitted
*                voice signal prior to encoding.
*                VoiceProfile.{i}.Line{i}.VoiceProcessing.TransmitGain = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlVPTransmitGain( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineProcessingObject *voiceLineProcessingObj = NULL;
   unsigned int vpInst = parms->op[0];
   unsigned int lineInst = parms->op[1];
   int setVal = atoi(value);

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* Check if gain is valid */
   if ( setVal > MAXGAIN || setVal < MINGAIN )
   {
      cmsLog_error( "gain=%i is invalid. Valid range: %d <= gain <= %d\n", setVal, MINGAIN, MAXGAIN);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_PROCESSING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineProcessingObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line processing object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   voiceLineProcessingObj->transmitGain = setVal * GAINUNIT;

   cmsLog_debug( "Transmit Gain = %i units of 0.1 dB for [vpInst, lineInst] = [%u, %u]\n", setVal, vpInst, lineInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( voiceLineProcessingObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Transmit gain ret = %d\n", ret);
   }

   cmsObj_free((void **) &voiceLineProcessingObj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlCLSilenceSuppression
* Description  : CLI wrapper for SetVlCLSilenceSuppression()
*                Indicates support for silence suppression for this codec.
*                VoiceProfile.{i}.Line{i}.Codec.List.{i}.SilenceSuppression = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCLSilenceSuppression( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int setValInt;
   unsigned int vpInst     = parms->op[0];
   unsigned int lineInst   = parms->op[1];
   const char * setVal     = value;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* Check for correct CLI parameter value */
   if ( !strcmp( "on", setVal ) )
   {
      setValInt = 1;
   }
   else if ( !strcmp( "off", setVal ) )
   {
      setValInt = 0;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   return ( setVlCLSilenceSuppression( vpInst, lineInst, setValInt ));
}

/***************************************************************************
* Function Name: dalVoice_SetVlCLPacketizationPeriod
* Description  : CLI wrapper for SetVlCLPacketizationPeriod()
*                Comma-separate list of supported packetization periods, in milliseconds,
*                or continuous ranges of packetization periods
*                VoiceProfile.{i}.Line{i}.Codec.List.{i}.PacketizationPeriod = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCLPacketizationPeriod( DAL_VOICE_PARMS *parms, char *value )
{
   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];
   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   const char * setVal = value;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   return ( setVlCLPacketizationPeriod( vpInst, lineInst, setVal ));
}

/***************************************************************************
* Function Name: dalVoice_SetVlCLCodecList
* Description  : CLI wrapper for SetVlCLCodecList()
*                Comma-separate list of codecs (no whitespaces or other delimiters).
*                First in list is highest priority (priority == 1).
*                Last in list is lowest priority (priority == max(all_codec_priorities)).
*                VoiceProfile.{i}.Line{i}.Codec.List.{i}. = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCLCodecList( DAL_VOICE_PARMS *parms, char *value )
{
   /* parse Service Provider Number as unsigned int */
   unsigned int vpInst = parms->op[0];
   /* parse Account Number as unsigned int */
   unsigned int lineInst= parms->op[1];

   const char * setVal = value;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   return ( setVlCLCodecList( vpInst, lineInst, setVal ));
}

/***************************************************************************
* Function Name: dalVoice_SetRtpDSCPMark
* Description  : Diffserv code point to be used for outgoing RTP
*                packets for this profile
*                VoiceProfile.{i}.Sip.DSCPMark = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetRtpDSCPMark( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfRtpObject *voiceProfRtpObj = NULL;

   /* parse port number as unsigned unsigned int */
   unsigned int setVal = atoi(value);
   unsigned int vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_PROF_RTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceProfRtpObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve RTP object \n" );
      return ( ret );
   }

   /* set the new value in local copy */
   voiceProfRtpObj->DSCPMark = setVal;

   cmsLog_debug( "RTP DSCP Mark = %u for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( voiceProfRtpObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set RTP DSCP mark ret = %d\n", ret);
   }

   cmsObj_free((void **) &voiceProfRtpObj);

   return ret;
}

#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
/***************************************************************************
* Function Name: dalVoice_SetPstnDialPlan
* Description  : Set the PSTN outgoing dial plan
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetPstnDialPlan( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        pstnInst   = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoicePstnObject    *pstnObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PSTN, pstnInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&pstnObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve PSTN object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free ( pstnObj->outgoingCallDialPlan );
   pstnObj->outgoingCallDialPlan = cmsMem_strdup ( value );

   cmsLog_debug( "PSTN Dial Plan = %s for [PstnInst] = [%u]\n", value, pstnInst );
   /* copy new value from local copy to MDM */
   if ( (ret = cmsObj_set( pstnObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't save PSTN dial plan ret = %d\n", ret);
   }

   cmsObj_free( (void **) &pstnObj );

   return (ret);
}

/***************************************************************************
* Function Name: dalVoice_SetPstnRouteRule
* Description  : Set the PSTN outgoing routing rule
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetPstnRouteRule( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        pstnInst   = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoicePstnObject    *pstnObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PSTN, pstnInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /* validate the new value is an allowable value */
   if ( strcmp( value, MDMVS_AUTO ) && strcmp( value, MDMVS_LINE ) && strcmp( value, MDMVS_VOIP ) )
   {
      cmsLog_error( "Invalid routing rule %s \n",value);
      return ( CMSRET_INVALID_PARAM_NAME );
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&pstnObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve PSTN object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free ( pstnObj->incomingCallRoutingMode );
   pstnObj->incomingCallRoutingMode = cmsMem_strdup ( value );

   cmsLog_debug( "PSTN routing rule = %s for [PstnInst] = [%u]\n", value, pstnInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( pstnObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't save PSTN route rule ret = %d\n", ret);
   }

   cmsObj_free( (void **) &pstnObj );

   return (ret);
}

/***************************************************************************
* Function Name: dalVoice_SetPstnRouteData
* Description  : Set the PSTN outgoing routing data
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetPstnRouteData( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        pstnInst   = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoicePstnObject    *pstnObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PSTN, pstnInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&pstnObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve PSTN object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free ( pstnObj->incomingCallRoutingDest );
   pstnObj->incomingCallRoutingDest = cmsMem_strdup ( value );

   cmsLog_debug( "PSTN routing rule = %s for [PstnInst] = [%u]\n", value, pstnInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( pstnObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't save PSTN route rule ret = %d\n", ret);
   }

   cmsObj_free( (void **) &pstnObj );

   return (ret);
}
#endif /* DMP_X_BROADCOM_COM_PSTNENDPOINT_1 */

#ifdef MGCPLOAD
/*****************************************************************
**  FUNCTION:       dalVoice_SetMgcpCallAgentIpAddress
**
**  PUROPOSE:       Set the call agent name in the mdm object
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   callAgentIpAddress - MGCP Call Agent IP Address
**
**  RETURNS:        CMSRET_SUCCESS - Write Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetMgcpCallAgentIpAddress(DAL_VOICE_PARMS *parms, char *callAgentIpAddress )
{
   CmsRet               ret;
   InstanceIdStack      iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfMgcpObject *mgcpObj    = NULL;
   int                  vpInst     = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]     = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_MGCP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, callAgentIpAddress); /* Debug: display command parameters passed here */

   /*  Get the MGCP Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&mgcpObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve mgcp voice profile object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free ( mgcpObj->callAgent1 );
   mgcpObj->callAgent1 = cmsMem_strdup ( callAgentIpAddress );

   cmsLog_debug( "Call agent = %s for [vpInst] = [%u]\n", callAgentIpAddress, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( mgcpObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set call agent ip address ret = %d\n", ret);
      cmsObj_free((void **) &mgcpObj);
   }
   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetMgcpGatewayName
**
**  PUROPOSE:       Set the gateway name in the mdm object. In the
**                  mdm object it is the domain.
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   gatewayName - MGCP Gateway Name
**
**  RETURNS:        CMSRET_SUCCESS - Write Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetMgcpGatewayName(DAL_VOICE_PARMS *parms, char *gatewayName )
{
   CmsRet               ret;
   InstanceIdStack      iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfMgcpObject *mgcpObj    = NULL;
   int                  vpInst     = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]     = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_MGCP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   dumpCmdParam(parms, gatewayName); /* Debug: display command parameters passed here */

   /*  Get the MGCP Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&mgcpObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve mgcp voice profile object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free ( mgcpObj->user );
   mgcpObj->user = cmsMem_strdup ( gatewayName );

   cmsLog_debug( "Gateway name = %s for [vpInst] = [%u]\n", mgcpObj->user, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( mgcpObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set gateway name ret = %d\n", ret);
      cmsObj_free((void **) &mgcpObj);
   }

   return ret;
}
#endif /* MGCPLOAD */


#if defined( DMP_X_BROADCOM_COM_NTR_1 )
/***************************************************************************
* Function Name: dalVoice_SetNtrEnable
* Description  : Enable NTR mode to apply a feedback offset to the PCM feedback control registers
*                X_BROADCOM_COM_Ntr.Enable = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrEnable(DAL_VOICE_PARMS *parms, char *setVal)
{
   int                 ntrInst  = parms->op[0];
   int                 setValInt;
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   if ( !strncasecmp( setVal, MDMVS_OFF, strlen( MDMVS_OFF )))
   {
      setValInt = 0;
   }
   else if ( !strncasecmp( setVal, MDMVS_ON, strlen( MDMVS_ON  )))
   {
      setValInt = 1;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", setVal);
      return CMSRET_INVALID_ARGUMENTS;
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsLog_debug( "NtrEnable = %s\n ", (obj->enable)? "enable":"disable");

   obj->enable = setValInt;

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Enable ret = %d\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ( ret );
}

/***************************************************************************
* Function Name: dalVoice_SetNtrAutoModeEnable
* Description  : set NTR mode to automatically calculate the most appropriate
*                feedback offset
*                X_BROADCOM_COM_Ntr.AutoModeEnable = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrAutoModeEnable(DAL_VOICE_PARMS *parms, char *setVal )
{
   int                 ntrInst  = parms->op[0];
   int                 setValInt;
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   if ( !strncasecmp( setVal, MDMVS_OFF, strlen( MDMVS_OFF )))
   {
      setValInt = 0;
   }
   else if ( !strncasecmp( setVal, MDMVS_ON,  strlen( MDMVS_ON  )))
   {
      setValInt = 1;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsLog_debug( "NtrAutoModeEnable = %s\n ", (obj->autoModeEnable)? "enable":"disable");

   obj->autoModeEnable = setValInt;

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Auto Mode  ret = %d\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrManualOffset
* Description  : set the user-specified manual offset for NTR to use with the
*                PCM highway feedback control registers
*                X_BROADCOM_COM_Ntr.ManualOffset = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrManualOffset(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->manualOffset);
   obj->manualOffset = cmsMem_strdup(setVal);

   cmsLog_debug( "NtrManualOffset = %s for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Manual Offset ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrManualPpm
* Description  : set the automatically-calculated offset in PPM for NTR to use with the
*                PCM highway feedback control registers
*                X_BROADCOM_COM_Ntr.ManualPpm = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrManualPpm(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   unsigned int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->manualPpm);
   obj->manualPpm = cmsMem_strdup(setVal);

   cmsLog_debug( "NtrManualPpm = %s for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Auto Offset ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrManualSteps
* Description  : set the manual offset in PLL steps for NTR to use with the
*                PCM feedback control registers
*                X_BROADCOM_COM_Ntr.ManualSteps = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrManualSteps(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   int ntrInst = parms->op[0];

   /* parse auto offset as signed long int */
   long setValInt = atoi(setVal);

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   obj->manualSteps = setValInt;

   cmsLog_debug( "NtrManualSteps = %i for [vpInst] = [%i]\n ", setValInt, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Manual Steps ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrDebugEnable
* Description  : set the debug information display settings
*                for the NTR task in automatic mode
*                X_BROADCOM_COM_Ntr.DebugEnable = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrDebugEnable(DAL_VOICE_PARMS *parms, char *setVal )
{
   int                 ntrInst  = parms->op[0];
   int                 setValInt;
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   if ( !strncasecmp( setVal, MDMVS_OFF, strlen( MDMVS_OFF )))
   {
      setValInt = 0;
   }
   else if ( !strncasecmp( setVal, MDMVS_ON,  strlen( MDMVS_ON  )))
   {
      setValInt = 1;
   }
   else
   {
      cmsLog_error( "cmdLine=%s\n", setVal);
      return ( CMSRET_INVALID_ARGUMENTS );
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsLog_debug( "NtrDebugEnable = %s\n ", (obj->debugEnable)? "enable":"disable");

   obj->debugEnable = setValInt;

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Debug Enable  ret = %d\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;

}

/***************************************************************************
* Function Name: dalVoice_SetNtrPcmMipsTallyCurrent
* Description  : set the current PCM-MIPS Tally
*                for the NTR task in automatic mode
*                X_BROADCOM_COM_Ntr.PcmMipsTallyCurrent = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrPcmMipsTallyCurrent(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   int ntrInst = parms->op[0];


   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->pcmMipsTallyCurrent);
   obj->pcmMipsTallyCurrent = cmsMem_strdup(setVal);

   cmsLog_debug( "NtrPcmMipsTallyCurrent = %s for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Current PCM-MIPS Tally ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrPcmMipsTallyPrevious
* Description  : set the previous PCM-MIPS Tally
*                for the NTR task in automatic mode
*                X_BROADCOM_COM_Ntr.PcmMipsTallyPrevious = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrPcmMipsTallyPrevious(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->pcmMipsTallyPrevious);
   obj->pcmMipsTallyPrevious = cmsMem_strdup(setVal);

   cmsLog_debug( "NtrPcmMipsTallyPrevious = %s for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Previous PCM-MIPS Tally ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrPcmNtrTallyCurrent
* Description  : set the current PCM-NTR Tally
*                for the NTR task in automatic mode
*                X_BROADCOM_COM_Ntr.PcmNtrTallyCurrent = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrPcmNtrTallyCurrent(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->pcmNtrTallyCurrent);
   obj->pcmNtrTallyCurrent = cmsMem_strdup(setVal);

   cmsLog_debug( "NtrPcmNtrTallyCurrent = %s for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Current PCM-NTR Tally ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrPcmNtrTallyPrevious
* Description  : set the previous PCM-NTR Tally
*                for the NTR task in automatic mode
*                X_BROADCOM_COM_Ntr.PcmNtrTallyPrevious = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrPcmNtrTallyPrevious(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->pcmNtrTallyPrevious);
   obj->pcmNtrTallyPrevious = cmsMem_strdup(setVal);

   cmsLog_debug( "PcmNtrTallyPrevious = %i for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Previous PCM-NTR Tally ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrDslMipsTallyCurrent
* Description  : set the current DSL-MIPS Tally
*                for the NTR task in automatic mode
*                X_BROADCOM_COM_Ntr.DslMipsTallyCurrent = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrDslMipsTallyCurrent(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->dslMipsTallyCurrent);
   obj->dslMipsTallyCurrent = cmsMem_strdup(setVal);

   cmsLog_debug( "DslMipsTallyCurrent = %s for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Current DSL-MIPS Tally ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrDslMipsTallyPrevious
* Description  : set the previous DSL-MIPS Tally
*                for the NTR task in automatic mode
*                X_BROADCOM_COM_Ntr.DslMipsTallyPrevious = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrDslMipsTallyPrevious(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->dslMipsTallyPrevious);
   obj->dslMipsTallyPrevious = cmsMem_strdup(setVal);

   cmsLog_debug( "DslMipsTallyPrevious = %s for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Previous DSL-MIPS Tally ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrDslNtrTallyCurrent
* Description  : set the current DSL-NTR Tally
*                for the NTR task in automatic mode
*                X_BROADCOM_COM_Ntr.DslNtrTallyCurrent = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrDslNtrTallyCurrent(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->dslNtrTallyCurrent);
   obj->dslNtrTallyCurrent = cmsMem_strdup(setVal);

   cmsLog_debug( "DslNtrTallyCurrent = %i for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Current DSL-NTR Tally ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrDslNtrTallyPrevious
* Description  : set the previous DSL-NTR Tally
*                for the NTR task in automatic mode
*                X_BROADCOM_COM_Ntr.DslNtrTallyPrevious = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrDslNtrTallyPrevious(DAL_VOICE_PARMS *parms, char *setVal )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->dslNtrTallyPrevious);
   obj->dslNtrTallyPrevious = cmsMem_strdup(setVal);

   cmsLog_debug( "DslNtrTallyPrevious = %s for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR Previous DSL-NTR Tally ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrSampleRate
* Description  : set the NTR sampling rate in automatic mode
*                X_BROADCOM_COM_Ntr.SampleRate = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrSampleRate(DAL_VOICE_PARMS *parms, char *setVal )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->sampleRate);
   obj->sampleRate = cmsMem_strdup(setVal);

   cmsLog_debug( "SampleRate = %s for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR SampleRate ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrPllBandwidth
* Description  : set the NTR PLL bandwidth in automatic mode
*                X_BROADCOM_COM_Ntr.PllBandwidth = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrPllBandwidth(DAL_VOICE_PARMS *parms, char *setVal )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->pllBandwidth);
   obj->pllBandwidth = cmsMem_strdup(setVal);

   cmsLog_debug( "PllBandwidth = %s for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR PllBandwidth ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrDampingFactor
* Description  : set the NTR damping factor in automatic mode
*                X_BROADCOM_COM_Ntr.DampingFactor = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrDampingFactor(DAL_VOICE_PARMS *parms, char *setVal )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                                {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->dampingFactor);
   obj->dampingFactor = cmsMem_strdup(setVal);

   cmsLog_debug( "DampingFactor = %s for [vpInst] = [%i]\n ", setVal, ntrInst );

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set NTR DampingFactor ret = %i\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrDefaults
* Description  : set NTR default values for sampling, bandwidth, damping.
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrDefaults( DAL_VOICE_PARMS *parms, char *value )
{
   (void)parms;
   (void)value;

   int vpInst;
   int spNum = 0;

   /* Map to vpInst */
   dalVoice_mapSpNumToVpInst( spNum, &vpInst ); /* always returns (*vpInst)++, but this may change in future*/
   DAL_VOICE_PARMS dalVoiceParm = { {vpInst, 0, 0, 0, 0, 0} };

   /* Changes to these default values require changes to #defines in ntrSync.c!!! */
   dalVoice_SetNtrSampleRate( &dalVoiceParm, "1000" );
   dalVoice_SetNtrPllBandwidth( &dalVoiceParm, "0.096323" );
   dalVoice_SetNtrDampingFactor( &dalVoiceParm, "0.761" );

   return CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrAutoOffset
* Description  : set the most recent automatically-calculated offset in Hz for NTR to use with the
*                PCM highway feedback control registers without shifting history
*                X_BROADCOM_COM_Ntr.History.{i}.AutoOffset = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrAutoOffset(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   unsigned int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->autoOffset);
   obj->autoOffset = cmsMem_strdup(setVal);

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Failed to set NTR AutoOffset Object");
   }

   cmsObj_free((void **) &obj );

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrAutoPpmHistory
* Description  : set the most recent automatically-calculated offset in PPM for NTR to use with the
*                PCM highway feedback control registers without shifting history
*                X_BROADCOM_COM_Ntr.History.{i}.AutoPpm = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrAutoPpm(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject      *obj      = NULL;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   unsigned int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsMem_free(obj->autoPpm);
   obj->autoPpm = cmsMem_strdup(setVal);

   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Failed to set NTR AutoPPM Object");
   }

   cmsObj_free((void **) &obj );

   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrAutoStepsHistory
* Description  : set the most recent feedback offset applied to the PLL
*                in steps for the NTR task in automatic mode
*                X_BROADCOM_COM_Ntr.History.{i} = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrAutoStepsHistory(DAL_VOICE_PARMS *parms, char *setVal )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject *obj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };


   int current = atoi(setVal);
   int previous;

   /*  Get the Voice Profile object */
   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj); ret != CMSRET_NO_MORE_INSTANCES;
        ret = cmsObj_getNextInSubTree( MDMOID_VOICE_NTR_HISTORY, &info[0].iidStack, &iidStack,
                                       (void **)&obj) )
   {
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve NTR object \n" );
         return ( ret );
      }

      //Shift autostep history
      previous = obj->autoSteps;
      obj->autoSteps = current;
      current = previous;

      if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Failed to set NTR History-AutoSteps Object");
      }

      cmsObj_free((void **) &obj );
   }

   return (ret != CMSRET_NO_MORE_INSTANCES) ? ret : CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrAutoOffsetHistory
* Description  : set the most recent automatically-calculated offset in Hz for NTR to use with the
*                PCM highway feedback control registers
*                X_BROADCOM_COM_Ntr.History.{i}.AutoOffset = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrAutoOffsetHistory(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject      *obj      = NULL;
   int stringLength = 12;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   unsigned int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   char current[stringLength];
   double previous;
   int i = 1;

   snprintf( (char*)current, stringLength, (char*)setVal);

   /*  Get the Voice Profile object */
   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj); ret != CMSRET_NO_MORE_INSTANCES;
        ret = cmsObj_getNextInSubTree( MDMOID_VOICE_NTR_HISTORY, &info[0].iidStack, &iidStack,
                                       (void **)&obj) )
   {
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve NTR object \n" );
         return ( ret );
      }

      //Shift auto offset history
      previous = atof( (char*) obj->autoOffset);
      cmsMem_free(obj->autoOffset);
      obj->autoOffset = cmsMem_strdup(current);
      snprintf( (char*)current, stringLength, "%f", previous);

      if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Failed to set NTR History-AutoOffset Object");
      }

      cmsObj_free((void **) &obj );
      i++;
   }

   return (ret != CMSRET_NO_MORE_INSTANCES) ? ret : CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrAutoPpmHistory
* Description  : set the most recent automatically-calculated offset in PPM for NTR to use with the
*                PCM highway feedback control registers
*                X_BROADCOM_COM_Ntr.History.{i}.AutoPpm = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrAutoPpmHistory(DAL_VOICE_PARMS *parms, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject      *obj      = NULL;
   int stringLength = 12;

   dumpCmdParam(parms, setVal); /* Debug: display command parameters passed here */

   /* parse Service Provider Number as int */
   unsigned int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };
   char current[stringLength];
   double previous;
   int i = 1;

   snprintf( (char*)current, stringLength, (char*)setVal);

   /*  Get the Voice Profile object */
   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj); ret != CMSRET_NO_MORE_INSTANCES;
        ret = cmsObj_getNextInSubTree( MDMOID_VOICE_NTR_HISTORY, &info[0].iidStack, &iidStack,
                                       (void **)&obj) )
   {
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve NTR object \n" );
         return ( ret );
      }

      //Shift autoPPM history
      previous = atof( (char*) obj->autoPpm);
      cmsMem_free(obj->autoPpm);
      obj->autoPpm = cmsMem_strdup(current);
      snprintf( (char*)current, stringLength, "%f", previous);

      if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Failed to set NTR History-AutoPpm Object");
      }

      cmsObj_free((void **) &obj );
      i++;
   }

   return (ret != CMSRET_NO_MORE_INSTANCES) ? ret : CMSRET_SUCCESS;
}

/***************************************************************************
* Function Name: dalVoice_SetNtrAutoPhaseErrorHistory
* Description  : set the most recent automatically-calculated phase error from calculations
*                X_BROADCOM_COM_Ntr.History.{i}.AutoPhaseError = new value
*
* Parameters   : parms->op[0] = vpInst, setVal = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrAutoPhaseErrorHistory(DAL_VOICE_PARMS *parms, char *setVal )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject *obj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };


   int current = atoi(setVal);
   int previous;

   /*  Get the Voice Profile object */
   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj); ret != CMSRET_NO_MORE_INSTANCES;
        ret = cmsObj_getNextInSubTree( MDMOID_VOICE_NTR_HISTORY, &info[0].iidStack, &iidStack,
                                       (void **)&obj) )
   {
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve NTR object \n" );
         return ( ret );
      }

      //Shift autostep history
      previous = obj->autoPhaseError;
      obj->autoPhaseError = current;
      current = previous;

      if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Failed to set NTR History-autoPhaseError Object");
      }

      cmsObj_free((void **) &obj );
   }

   return (ret != CMSRET_NO_MORE_INSTANCES) ? ret : CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_SetNtrResetHistory
* Description  : Resets history parameters to 0
*                X_BROADCOM_COM_Ntr.History.{i}.AutoSteps = 0
*                X_BROADCOM_COM_Ntr.History.{i}.AutoOffset = 0
*                X_BROADCOM_COM_Ntr.History.{i}.AutoPpm = 0
*                X_BROADCOM_COM_Ntr.History.{i}.AutoPhaseError = 0
*
* Parameters   : parms->op[0] = vpInst
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetNtrResetHistory(DAL_VOICE_PARMS *parms, char *setVal )
{
   (void)setVal;

   CmsRet              ret;
   InstanceIdStack     iidStack  = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject      *obj      = NULL;

   /* parse Service Provider Number as int */
   unsigned int ntrInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj); ret != CMSRET_NO_MORE_INSTANCES;
        ret = cmsObj_getNextInSubTree( MDMOID_VOICE_NTR_HISTORY, &info[0].iidStack, &iidStack,
                                       (void **)&obj) )
   {
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve NTR object \n" );
         return ( ret );
      }

      /* Reset AutoSteps */
      obj->autoSteps = 0;

      /* Reset AutoOffset */
      cmsMem_free(obj->autoOffset);
      obj->autoOffset = cmsMem_strdup("0");

      /* Reset AutoPPM */
      cmsMem_free(obj->autoPpm);
      obj->autoPpm = cmsMem_strdup("0");

      /* Reset Auto Phase Error */
      obj->autoPhaseError = 0;

      /* Set Object back into CMS */
      if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Failed to set NTR History-AutoPpm Object");
      }

      cmsObj_free((void **) &obj );

   }
   return (ret != CMSRET_NO_MORE_INSTANCES) ? ret : CMSRET_SUCCESS;
}
#endif /* DMP_X_BROADCOM_COM_NTR_1 */



/*<END>===================================== DAL Set functions =======================================<END>*/

/*<START>==================================== DAL Get Functions ====================================<START>*/


#ifdef STUN_CLIENT
/*****************************************************************
**  FUNCTION:       dalVoice_GetSTUNServer
**
**  PUROPOSE:
**
**  INPUT PARAMS:   vpInst  - parms->op[0]
**
**  OUTPUT PARMS:   stunServer - STUN server IP address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetSTUNServer(DAL_VOICE_PARMS *parms, char *stunServer, unsigned int length )
{
   unsigned int       vpInst;
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj = NULL;

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* Copy STUN server value */
   if( profObj->STUNServer != NULL )
   {
      cmsLog_debug( "Stun Server = %s\n ", profObj->STUNServer);
      strncpy((char *)stunServer, profObj->STUNServer, length );
   }

   cmsObj_free( (void **) &profObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSTUNServerPort
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   port - STUN server port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSTUNServerPort(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
   unsigned int       vpInst     = parms->op[0];
   CmsRet             ret;
   InstanceIdStack    iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]   = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                     {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* Copy STUN server port value */
   cmsLog_debug( "STUN Server Port = %d\n ", profObj->X_BROADCOM_COM_STUNServerPort);
   snprintf( (char*)port, length, "%u",profObj->X_BROADCOM_COM_STUNServerPort );
   cmsObj_free( (void **) &profObj );

   return  ( ret );
}
#endif /* STUN_CLIENT */


/*****************************************************************
**  FUNCTION:       dalVoice_GetDigitMap
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   map - Dialing Digits Mapping
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetDigitMap(DAL_VOICE_PARMS *parms, char *map, unsigned int length )
{
   unsigned int       vpInst;
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj    = NULL;

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* Copy digitmap value */
   if( profObj->digitMap != NULL )
   {
      cmsLog_debug( "Digits map = %s\n ", profObj->digitMap);
      strncpy( (char *)map, profObj->digitMap, length);
   }
   else
   {
      cmsLog_error( "digitMap object is not set\n" );
      ret =  CMSRET_INVALID_PARAM_VALUE;
   }

   cmsObj_free( (void **) &profObj );

   return ( ret );
}

#ifdef SIPLOAD
/*****************************************************************
**  FUNCTION:       dalVoice_GetCCTKDigitMap
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   map - Dialing Digits Mapping (custom)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCCTKDigitMap(DAL_VOICE_PARMS *parms, char *map, unsigned int length )
{
   unsigned int       vpInst;
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj    = NULL;

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* Copy digitmap value */
   if( profObj->X_BROADCOM_COM_CCTK_DigitMap != NULL )
   {
      cmsLog_debug( "CCTK Digits map = %s\n ", profObj->X_BROADCOM_COM_CCTK_DigitMap);
      strncpy( (char *)map, profObj->X_BROADCOM_COM_CCTK_DigitMap, length);
   }
   else
   {
      cmsLog_debug( "X_BROADCOM_COM_CCTK_DigitMap object is not set\n" );
      ret =  CMSRET_INVALID_PARAM_VALUE;
   }

   cmsObj_free( (void **) &profObj );

   return ( ret );
}
#endif

/*****************************************************************
**  FUNCTION:       dalVoice_GetCriticalDigitTimer
**
**  PUROPOSE:       Return value of
**                  VoiceProfile.{i}.X_BROADCOM_COM_CriticalDigitTimer
**
**  INPUT PARMS:    parms->op[0] = Voice Profile Instance
**                  length       = Maximum length of output string
**
**  OUTPUT PARMS:   timer - Critical Digit Timing
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCriticalDigitTimer ( DAL_VOICE_PARMS *parms, char *timer, unsigned int length )
{
   unsigned int       vpInst;
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj    = NULL;

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "_get returned %d\n", ret );
      return ( ret );
   }

   /* DAL interface protects against this but Network CMS may not */
   if (profObj->X_BROADCOM_COM_CriticalDigitTimer < 1)
   {
      cmsLog_error( "value %d out-of-range\n", profObj->X_BROADCOM_COM_CriticalDigitTimer );
      cmsObj_free( (void **) &profObj );

      return ( CMSRET_INVALID_PARAM_VALUE );
   }

   snprintf( (char *)timer, length, "%u", profObj->X_BROADCOM_COM_CriticalDigitTimer);
   cmsObj_free( (void **) &profObj );

   return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetPartialDigitTimer
**
**  PUROPOSE:       Return value of
**                  VoiceProfile.{i}.X_BROADCOM_COM_PartialDigitTimer
**
**  INPUT PARMS:    parms->op[0] = Voice Profile Instance
**                  length       = Maximum length of output string
**
**  OUTPUT PARMS:   timer - Partial Digit Timing
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPartialDigitTimer ( DAL_VOICE_PARMS *parms, char *timer, unsigned int length )
{
   unsigned int       vpInst;
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj    = NULL;

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);
   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "_get returned %d\n", ret );
      return ( ret );
   }

   /* DAL interface protects against this but Network CMS may not */
   if (profObj->X_BROADCOM_COM_PartialDigitTimer < 1)
   {
      cmsLog_error( "value %d out-of-range\n", profObj->X_BROADCOM_COM_PartialDigitTimer );
      cmsObj_free( (void **) &profObj );

      return ( CMSRET_INVALID_PARAM_VALUE );
   }

   snprintf((char *)timer, length, "%u", profObj->X_BROADCOM_COM_PartialDigitTimer);
   cmsObj_free( (void **) &profObj );

   return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetIpAddr
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   Interface IP Address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetIpAddr(DAL_VOICE_PARMS *parms, char *ipAddr, unsigned int length )
{
   CmsRet       ret = CMSRET_SUCCESS;

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetFlexTermSupport
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   True if CMGR type is CCTK,
**                  False if type is CALLCTL.
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetFlexTermSupport( DAL_VOICE_PARMS *parms, char* type, unsigned int length )
{
   *type = '1';
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRegion
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**
**  OUTPUT PARMS:   country Alpha3 string
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetRegion(DAL_VOICE_PARMS *parms, char *country, unsigned int length )
{
   char               localeAlpha3[TEMP_CHARBUF_SIZE];
   unsigned int       vpInst;
   UBOOL8             found;
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj = NULL;

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* Mdm will return (null) point if no default value present, so alway check for null pointer */
   if( profObj->region != NULL )
   {
      cmsLog_debug( "region = %s\n ", profObj->region);
      strncpy( (char *)country, profObj->region, length );
      if( (ret = mapAlpha2toAlpha3( (char*) country, localeAlpha3, &found, TEMP_CHARBUF_SIZE) ) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Unknow region = %s\n", country );
         cmsObj_free( (void **) &profObj );
         return ( ret );
      }
      strncpy( (char *)country, localeAlpha3, length);
   }

   cmsObj_free( (void **) &profObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRegionVrgCode
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**
**  OUTPUT PARMS:   vrg country code
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetRegionVrgCode(DAL_VOICE_PARMS *parms, char *country, unsigned int length )
{
   char               localeAlpha2[TEMP_CHARBUF_SIZE];
   unsigned int       vpInst;
   int                countryInt;
   UBOOL8                found;
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj = NULL;

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   if( profObj->region != NULL ) /* Mdm will return (null) point if no default value present, so alway check for null pointer */
   {
      cmsLog_debug( "region = %s\n ", profObj->region);
      strncpy( (char *)localeAlpha2, profObj->region, TEMP_CHARBUF_SIZE);
      if( dalVoice_mapAlpha2toVrg( localeAlpha2, &countryInt, &found, TEMP_CHARBUF_SIZE ) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Unknown region = %s\n", localeAlpha2 );
      }
      else
      {
         snprintf( (char*)country, length, "%d",countryInt );
      }
   }

   cmsObj_free( (void **) &profObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRegionSuppString
**
**  PUROPOSE:
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:   value - list of supported locales in Alpha3
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetRegionSuppString( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   return(rutVoice_getSupportedAlpha3Locales(value, length));
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetT38Enable
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**
**  OUTPUT PARMS:   enabled - T38 Enabled
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetT38Enable(DAL_VOICE_PARMS *parms, char *enabled, unsigned int length )
{
   unsigned int           vpInst   = parms->op[0];
   CmsRet                 ret;
   InstanceIdStack        iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfFaxT38Object *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]     = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_FAX_T38, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve fax T38 object \n" );
      return ( ret );
   }

   cmsLog_debug( "T38Enable = %s\n ", (obj->enable)? "enable":"disable");

   snprintf( (char*)enabled, length, "%u", obj->enable );

   cmsObj_free( (void **) &obj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetV18Enable
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**
**  OUTPUT PARMS:   enabled - V18 Enabled
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetV18Enable(DAL_VOICE_PARMS *parms, char *enabled, unsigned int length )
{
   unsigned int           vpInst   = parms->op[0];
   CmsRet                 ret;
   InstanceIdStack        iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject       *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]     = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   cmsLog_debug( "V18Enable = %s\n ", (obj->X_BROADCOM_COM_V18_Support)? "enable":"disable");

   snprintf( (char*)enabled, length, "%u", obj->X_BROADCOM_COM_V18_Support );

   cmsObj_free( (void **) &obj );

   return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetVBDEnable
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**
**  OUTPUT PARMS:   enabled - VBD Enabled
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
** NOTE: There is no VBD object in TR104 specifying whether voice-band
**       data mode is enabled or disabled. This function simply
**       returns the inverted value of dalVoice_GetT38Enable
**
*******************************************************************/
CmsRet dalVoice_GetVBDEnable(DAL_VOICE_PARMS *parms, char *enabled, unsigned int length )
{
   unsigned int           vpInst   = parms->op[0];
   CmsRet                 ret;
   InstanceIdStack        iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfFaxT38Object *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]     = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_FAX_T38, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve fax T38 object \n" );
      return ( ret );
   }

   cmsLog_debug( "VBDEnable = %s\n ", (obj->enable)? "disable":"enable");

   /* Invert the T38 enable value to get VBD enable value */
   snprintf( (char*)enabled, length, "%u", !obj->enable );

   cmsObj_free( (void **) &obj );

   return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetLoggingLevel
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
CmsRet dalVoice_GetLoggingLevel( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length)
{
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }
   else
   {
      snprintf( getVal, length, "%s",obj->X_BROADCOM_COM_LoggingLevel );
   }

   /* Free object */
   cmsObj_free((void **) &obj);

   return (ret);
}

/***************************************************************************
* Function Name: dalVoice_GetModuleLoggingLevel
* Description  : Gets the specific voice module's logging level
*
* Parameters   : none
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetModuleLoggingLevel( DAL_VOICE_PARMS *parms, char* modName, char * getVal, unsigned int length )
{
   char *pMod, *pVal, *pModAfter;
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }

   pMod = cmsUtl_strstr(obj->X_BROADCOM_COM_ModuleLogLevels, modName);
   if(!pMod)
   {
      cmsLog_debug( "Invalid module name\n" );
      cmsObj_free((void **) &obj);
      return CMSRET_INVALID_PARAM_NAME;
   }

   pVal = cmsUtl_strstr(pMod, "=");
   if(!pVal)
   {
      cmsLog_debug( "Invalid value associated with module %s\n", modName );
      cmsObj_free((void **) &obj);
      return CMSRET_INVALID_PARAM_VALUE;
   }
   while(*pVal == ' ' || *pVal == '=')
   {
      pVal++;
   }

   pModAfter = cmsUtl_strstr(pVal, ",");
   if(pModAfter)
   {
      *pModAfter = '\0';
   }

   snprintf(getVal, length, "%s", pVal);

   /* Free object */
   cmsObj_free((void **) &obj);

   return (ret);
}

/***************************************************************************
* Function Name: dalVoice_GetModuleLoggingLevels
* Description  : Gets all the voice module logging levels
*
* Parameters   : none
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetModuleLoggingLevels( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length )
{
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }

   snprintf(getVal, length, "%s", obj->X_BROADCOM_COM_ModuleLogLevels);

   /* Free object */
   cmsObj_free((void **) &obj);

   return (ret);
}

/***************************************************************************
* Function Name: dalVoice_GetManagementProtocol
* Description  : Gets the Protocol used to manage the Voice Service
*
* Parameters   : none
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetManagementProtocol( DAL_VOICE_PARMS *parms, void * getVal, unsigned int length )
{
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }
   else
   {
      snprintf( (char *)getVal, length, "%s",obj->X_BROADCOM_COM_ManagementProtocol );
   }

   /* Free object */
   cmsObj_free((void **) &obj);

   return (ret);
}

#ifdef SIPLOAD
/*****************************************************************
**  FUNCTION:       dalVoice_GetCCTKTraceLevel
**
**  PUROPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   cctk trace level
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCCTKTraceLevel( DAL_VOICE_PARMS *parms, void * getVal, unsigned int length)
{
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }
   else
   {
      snprintf( (char *)getVal, length, "%s",obj->X_BROADCOM_COM_CCTKTraceLevel );
   }

   /* Free object */
   cmsObj_free((void **) &obj);

   return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCCTKTraceGroup
**
**  PUROPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   cctk trace group
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCCTKTraceGroup( DAL_VOICE_PARMS *parms, void * getVal, unsigned int length)
{
   VoiceObject *obj = NULL;
   CmsRet ret;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }
   else
   {
      snprintf( (char *)getVal, length, "%s",obj->X_BROADCOM_COM_CCTKTraceGroup );
   }

   /* Free object */
   cmsObj_free((void **) &obj);

   return (ret);
}
#endif /* SIPLOAD */

/*****************************************************************
**  FUNCTION:       dalVoice_GetBoundIfName
**
**  PUROPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   bound ifname for  voice
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetBoundIfName( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length )
{
   VoiceObject *voiceObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   /* get and set the variable */

   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &voiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "could not get voice object, ret=%d", ret);
   }
   else
   {
      if ( voiceObj->X_BROADCOM_COM_BoundIfName != NULL )
      {
         snprintf( getVal, length, "%s",voiceObj->X_BROADCOM_COM_BoundIfName );
      }
      else
      {
         cmsLog_error( "X_BROADCOM_COM_BoundIfName is null");
         sprintf( getVal,"undefined" );
      }
   }

   cmsObj_free((void **) &voiceObj);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetBoundIPAddr
**
**  PUROPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   bound ipaddr for  voice
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetBoundIPAddr( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length )
{
   VoiceObject *voiceObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   /* get and set the variable */

   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &voiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "could not get voice object, ret=%d", ret);
   }
   else
   {
      if ( voiceObj->X_BROADCOM_COM_BoundIpAddr != NULL )
      {
         snprintf( getVal, length, "%s",voiceObj->X_BROADCOM_COM_BoundIpAddr );
#if VOICE_IPV6_SUPPORT
         if ( (ret = dal_Voice_StripIpv6PrefixLength(voiceObj, getVal)) != CMSRET_SUCCESS )
         {
            cmsLog_error( "Could not strip address prefix length, ret=%d", ret);
      }
#endif /* VOICE_IPV6_SUPPORT */
      }
      else
      {
         cmsLog_error( "X_BROADCOM_COM_BoundIpAddr is null");
         snprintf( getVal, length, "undefined" );
      }
   }

   cmsObj_free((void **) &voiceObj);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetIpFamilyList
**
**  PUROPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   IP address family list for  voice
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetIpFamilyList( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length )
{
   return ( rutVoice_getSupportedIpFamilyList( getVal, length ) );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDnsServerAddr
**
**  PUROPOSE:       Get IP address of the voice DNS server
*                   (X_BROADCOM_COM_VoiceDnsServer)
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   getVal - DNS server address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetDnsServerAddr(DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   VoiceObject *voiceObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   /* get and set the variable */

   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &voiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "could not get voice object, ret=%d", ret);
   }
   else
   {
      if ( voiceObj->X_BROADCOM_COM_VoiceDnsServer != NULL )
      {
         snprintf( getVal, length, "%s",voiceObj->X_BROADCOM_COM_VoiceDnsServer );
#if VOICE_IPV6_SUPPORT
         if ( (ret = dal_Voice_StripIpv6PrefixLength(voiceObj, getVal)) != CMSRET_SUCCESS )
         {
            cmsLog_error( "Could not strip address prefix length, ret=%d", ret);
      }
#endif /* VOICE_IPV6_SUPPORT */
      }
      else
      {
         cmsLog_error( "X_BROADCOM_COM_VoiceDnsServer is null");
         snprintf( getVal, length, "undefined" );
      }
   }

   cmsObj_free((void **) &voiceObj);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetIpFamily
**
**  PUROPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   IP address family for  voice
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetIpFamily( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length )
{
   CmsRet ret = CMSRET_SUCCESS;
#if VOICE_IPV6_SUPPORT
   VoiceObject *voiceObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* get and set the variable */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &voiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "could not get voice object, ret=%d", ret);
   }
   else
   {
      if ( voiceObj->X_BROADCOM_COM_IpAddressFamily != NULL )
      {
         snprintf( getVal, length, "%s",voiceObj->X_BROADCOM_COM_IpAddressFamily );
      }
      else
      {
         cmsLog_error( "X_BROADCOM_COM_IpAddressFamily is null");
         snprintf( getVal, length, "undefined" );
      }
   }

   cmsObj_free((void **) &voiceObj);
#else
   /* if IPv6 is not supported, return IPv4 only */
   snprintf( getVal, length, MDMVS_IPV4);
#endif /* VOICE_IPV6_SUPPORT */

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetSignalingProtocol
*
* PURPOSE:     Get signaling protocol capability
*
* PARAMETERS:  None
*
* RETURNS:     Supported signalling protocols
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetSignalingProtocol( DAL_VOICE_PARMS *parms, char* sigProt, unsigned int length )
{
   CmsRet            ret;
   VoiceCapObject*   obj = NULL;
   InstanceIdStack   iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Get VoiceService.{1}. Object */
   if ( (ret = cmsObj_getNext(MDMOID_VOICE_CAP, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve object\n" );
      return (ret);
   }
   else
   {
      if ( obj->signalingProtocols != NULL )
      {
         strncpy( sigProt, obj->signalingProtocols, length);
      }
   }

   /* Free object */
   cmsObj_free((void **) &obj);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetSupportedTransports
*
* PURPOSE:     Get list of available Transport layer protocols
*
* PARAMETERS:  None
*
* RETURNS:     Supported Transport layer protocols
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetSupportedTransports( DAL_VOICE_PARMS *parms, char* transports, unsigned int length )
{
   return ( rutVoice_getSupportedTransports( transports, length ) );
}

/****************************************************************************
* FUNCTION:    dalVoice_GetSupportedSrtpOptions
*
* PURPOSE:     Get list of available SRTP options
*
* PARAMETERS:  None
*
* RETURNS:     Supported SRTP options
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetSupportedSrtpOptions( DAL_VOICE_PARMS *parms, char* options, unsigned int length )
{
   return ( rutVoice_getSupportedSrtpOptions( options, length ) );
}

/****************************************************************************
* FUNCTION:    dalVoice_GetSupportedBackToPrimOptions
*
* PURPOSE:     Get list of available SRTP options
*
* PARAMETERS:  None
*
* RETURNS:     Supported SRTP options
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetSupportedBackToPrimOptions( DAL_VOICE_PARMS *parms, char* options, unsigned int length )
{
   return ( rutVoice_getSupportedBackToPrimOptions( options, length ) );
}

/****************************************************************************
* FUNCTION:    dalVoice_GetSupportedRedOptions
*
* PURPOSE:     Get list of available SRTP options
*
* PARAMETERS:  None
*
* RETURNS:     Supported SRTP options
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetSupportedRedOptions( DAL_VOICE_PARMS *parms, char* options, unsigned int length )
{
   return ( rutVoice_getSupportedRedOptions( options, length ) );
}

/****************************************************************************
* FUNCTION:    dalVoice_GetSupportedConfOptions
*
* PURPOSE:     Get list of available conferencing options
*
* PARAMETERS:  None
*
* RETURNS:     Supported SRTP options
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetSupportedConfOptions( DAL_VOICE_PARMS *parms, char* options, unsigned int length )
{
   return ( rutVoice_getSupportedConfOptions( options, length ) );
}

/***************************************************************************
* Function Name: dalVoice_SetEuroFlashEnable
* Description  : Enable or disable Euro flash
*                MDMVS_ON for enable, MDMVS_OFF for disable (case insensitive)
*                VoiceService.{i}.VoiceProfile.{i}.SIP.X_BROADCOM_COM_EuroFlashEnable
*
* Parameters   : parms->op[0] = vpInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetEuroFlashEnable( DAL_VOICE_PARMS *parms, char *value )
{
   unsigned int        setVal;
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   UINT8               isOff;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   dumpCmdParam(parms, value); /* Debug: display command parameters passed here */

   if ( (isOff = strncasecmp(value, MDMVS_ON, strlen(MDMVS_ON))) &&
        strncasecmp(value, MDMVS_OFF, strlen(MDMVS_OFF)) )
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   setVal = isOff ?0 :1;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   sipObj->X_BROADCOM_COM_EuroFlashEnable = setVal;

   cmsLog_debug( "SIP failover = %d for [vpInst] = [%u]\n", setVal, vpInst);
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set SIP euro flash enable ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

#ifdef DMP_BASELINE_1

/****************************************************************************
* FUNCTION:    dalVoice_GetNetworkIntfList
*
* PURPOSE:     Get list of available network instances
*
* PARAMETERS:  None
*
* RETURNS:     Supported signalling protocols
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetNetworkIntfList_igd( DAL_VOICE_PARMS *parms, char* intfList, unsigned int length )
{
   WanPppConnObject *pppConn=NULL;
   WanIpConnObject *ipConn=NULL;
   InstanceIdStack iidStack1 = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack2 = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   char tmp[20];

   /* Add the default interfaces */
   snprintf(intfList, length, "%s %s",MDMVS_LAN, MDMVS_ANY_WAN);

   /* Add WAN ip interfaces */
   while( (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack1, (void **) &ipConn)) == CMSRET_SUCCESS )
   {
      cmsLog_debug("WAN IP CONNECTION INTERFACE=%s", ipConn->X_BROADCOM_COM_IfName);
      if( ( !(cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_ROUTED)) || !(cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_BRIDGED)) )
           && ipConn->X_BROADCOM_COM_IfName != NULL )
      {
         sprintf(tmp, " %s", ipConn->X_BROADCOM_COM_IfName);
         strncat(intfList, tmp, length);
      }

      cmsObj_free((void **) &ipConn);
   }

   if( ret != CMSRET_NO_MORE_INSTANCES )
   {
      cmsLog_error("failed to get MDMOID_WAN_IP_CONN object ret=%d", ret);
      return ret;
   }

   /* Add WAN ppp interfces */
   while( (ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack2, (void **) &pppConn)) == CMSRET_SUCCESS )
   {
      cmsLog_debug("WAN PPP CONNECTION INTERFACE=%s", pppConn->X_BROADCOM_COM_IfName);
      if( !(cmsUtl_strcmp(pppConn->connectionType, MDMVS_IP_ROUTED)) && pppConn->X_BROADCOM_COM_IfName != NULL )
      {
         sprintf(tmp, " %s", pppConn->X_BROADCOM_COM_IfName);
         strncat(intfList, tmp, length);
      }

      cmsObj_free((void **) &pppConn);
   }

   if( ret != CMSRET_NO_MORE_INSTANCES )
   {
      cmsLog_error("failed to get MDMOID_WAN_PPP_CONN object ret=%d", ret);
   }

   return CMSRET_SUCCESS;

}

#endif  /* DMP_BASELINE_1 */

CmsRet dalVoice_GetNetworkIntfList( DAL_VOICE_PARMS *parms, char* intfList, unsigned int length )
{
#if defined(SUPPORT_DM_LEGACY98)
   return dalVoice_GetNetworkIntfList_igd(parms, intfList, length);
#elif defined(SUPPORT_DM_HYBRID)
   return dalVoice_GetNetworkIntfList_igd(parms, intfList, length);
#elif defined(SUPPORT_DM_PURE181)
   return dalVoice_GetNetworkIntfList_dev2(parms, intfList, length);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
      return dalVoice_GetNetworkIntfList_dev2(parms, intfList, length);
   }
   else
   {
      return dalVoice_GetNetworkIntfList_igd(parms, intfList, length);
   }
#endif
}




#ifdef SIPLOAD

/*****************************************************************
**  FUNCTION:       dalVoice_GetFeatureString
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**                  call feature   - parms->op[1]
**
**  OUTPUT PARMS:   Call feature string
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetFeatureString(DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetLogServer
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   logServer - SIP log server address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetLogServer(DAL_VOICE_PARMS *parms, char *logServer, unsigned int length )
{
   unsigned int       vpInst     = parms->op[0];
   CmsRet             ret;
   InstanceIdStack    iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]   = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                     {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n", parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* Copy log server value */
   if( profObj->X_BROADCOM_COM_LogServer != NULL )
   {
      cmsLog_debug( "Log Server Address = %s\n ", profObj->X_BROADCOM_COM_LogServer);
      strncpy( (char *) logServer, profObj->X_BROADCOM_COM_LogServer, length);

#if VOICE_IPV6_SUPPORT
         if ( (ret = dal_Voice_StripIpv6PrefixLength(NULL, logServer)) != CMSRET_SUCCESS )
         {
            cmsLog_error( "Could not strip address prefix length, ret=%d", ret);
         }
#endif /* VOICE_IPV6_SUPPORT */
   }

   cmsObj_free( (void **) &profObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetLogServerPort
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   port - SIP log server port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetLogServerPort(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject     *profObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* Copy log server port value */
   cmsLog_debug( "Log Server Port = %d\n ", profObj->X_BROADCOM_COM_LogServerPort);
   snprintf( (char*)port, length, "%u",profObj->X_BROADCOM_COM_LogServerPort );
   cmsObj_free( (void **) &profObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetHookFlashMethod
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   method - Hook flash method
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetHookFlashMethod(DAL_VOICE_PARMS *parms, char *method, unsigned int length )
{
   unsigned int        i;
   unsigned int        vpInst           = parms->op[0];
   char                temp[TEMP_CHARBUF_SIZE];
   CmsRet              ret;
   InstanceIdStack     iidStack         = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject     *profObj         = NULL;

   /* The positions of the strings should correspond to the DTMF moethod positions in the enum */
   char*               hookfMethods[]   = { MDMVS_NONE, MDMVS_NONE, MDMVS_SIPINFO };

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]          = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                            {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* Copy hook flash method value */
   cmsLog_debug( "Hook flash method = %s\n ", profObj->X_BROADCOM_COM_HookFlashMethod);

   /* Copy DTMF relay value */
   if( profObj->X_BROADCOM_COM_HookFlashMethod != NULL )
   {
      cmsLog_debug( "hookflash relay = %s\n ",
                    profObj->X_BROADCOM_COM_HookFlashMethod);

      strncpy( (char *)temp, profObj->X_BROADCOM_COM_HookFlashMethod,
               strlen(profObj->X_BROADCOM_COM_HookFlashMethod) + 1);

      for (i = 0; i <=  DAL_VOICE_DTMF_RELAY_SIPINFO; i++)
      {
         if( strncasecmp(temp, hookfMethods[i], TEMP_CHARBUF_SIZE) == 0 )
         {
            snprintf( (char*)method, length, "%u",i );
            break;
         }
      }
   }

   cmsObj_free( (void **) &profObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetHookFlashMethodString
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   method - Hook flash method
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetHookFlashMethodString(DAL_VOICE_PARMS *parms, char *method, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject     *profObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* Copy hook flash method value */
   cmsLog_debug( "Hook flash method = %s\n ", profObj->X_BROADCOM_COM_HookFlashMethod);

   /* Copy DTMF relay value */
   if( profObj->X_BROADCOM_COM_HookFlashMethod != NULL )
   {
      cmsLog_debug( "hookflash relay = %s\n ",
                    profObj->X_BROADCOM_COM_HookFlashMethod);

      strncpy( (char *)method, profObj->X_BROADCOM_COM_HookFlashMethod, length );

   }

   cmsObj_free( (void **) &profObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDTMFMethodIntValue
**
**  PUROPOSE:
**
**  INPUT PARAMS:   vpInst  - parms->op[0]
**
**  OUTPUT PARMS:   0 - InBand
**                  1 - RFC2833
**                  2 - SIPINFO
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetDTMFMethodIntValue(DAL_VOICE_PARMS *parms, char *dtmfRelay, unsigned int length )
{
   char               temp[TEMP_CHARBUF_SIZE];
   unsigned int       vpInst;
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj    = NULL;
   UINT16             i;

   /* The positions of the strings should correspond to the DTMF *
    * moethod positions in the enum DAL_VOICE_DTMF_RELAY_SERVICE */
   char* dtmfMethods[] = { MDMVS_INBAND, MDMVS_RFC2833, MDMVS_SIPINFO };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* Copy DTMF relay value */
   if( profObj->DTMFMethod != NULL )
   {
      cmsLog_debug( "DTMF Method = %s\n ", profObj->DTMFMethod);
      strncpy( (char *)temp, profObj->DTMFMethod, strlen(profObj->DTMFMethod) + 1);

      for (i = 0; i <=  DAL_VOICE_DTMF_RELAY_SIPINFO; i++)
      {
         if( strncasecmp(temp, dtmfMethods[i], TEMP_CHARBUF_SIZE) == 0 )
         {
            snprintf( (char*)dtmfRelay, length, "%u",i );
            break;
         }
      }
   }

   cmsObj_free((void **)&profObj );
   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDTMFMethod
**
**  PUROPOSE:
**
**  INPUT PARAMS:   vpInst - parms->op[0]
**
**  OUTPUT PARMS:   dtmfRelay - InBand
**                            - SIPINFO
**                            - IFC2833
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetDTMFMethod(DAL_VOICE_PARMS *parms, char *dtmfRelay, unsigned int length )
{
   unsigned int       vpInst;
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj = NULL;

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return ( ret );
   }

   /* Copy DTMF relay value */
   if( profObj->DTMFMethod != NULL )
   {
      cmsLog_debug( "DTMF Method = %s\n ", profObj->DTMFMethod);
      strncpy( (char *)dtmfRelay, profObj->DTMFMethod, length);
   }

   cmsObj_free((void **)&profObj );

   return ( ret );
}

CmsRet dalVoice_GetMaxPrefCodecs( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   int maxPrefCodecs;
   rutVoice_getMaxPrefCodecs( &maxPrefCodecs );
   snprintf( value, length, "%d", maxPrefCodecs );
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipProxyServer
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   proxyAddr - SIP Proxy Server address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipProxyServer(DAL_VOICE_PARMS *parms, char *proxyAddr, unsigned int length )
{
   unsigned int        vpInst;
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj = NULL;

   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n", parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy proxy server value */
   if( sipObj->proxyServer != NULL )
   {
      cmsLog_debug( "Proxy server address = %s\n ", sipObj->proxyServer );
      strncpy( (char *) proxyAddr, sipObj->proxyServer, length);

#if VOICE_IPV6_SUPPORT
      if ( (ret = dal_Voice_StripIpv6PrefixLength(NULL, proxyAddr)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Could not strip address prefix length, ret=%d", ret);
      }
#endif /* VOICE_IPV6_SUPPORT */
   }
   else
   {
      proxyAddr[0] = '\0';
   }

   cmsObj_free( (void **) &sipObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipProxyServerPort
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   proxyPort - Proxy server port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipProxyServerPort(DAL_VOICE_PARMS *parms, char *proxyPort, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0] );

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy proxy server port value */
   cmsLog_debug( "Proxy Server Port = %d ", sipObj->proxyServerPort );
   snprintf( (char*)proxyPort, length, "%d",sipObj->proxyServerPort );
   cmsObj_free( (void **) &sipObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipRegistrarServer
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   regSvrAddr - RegistrarServer URL
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipRegistrarServer(DAL_VOICE_PARMS *parms, char *regSvrAddr, unsigned int length )
{
   unsigned int        vpInst;
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj = NULL;

   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy registrar server value */
   if( sipObj->registrarServer != NULL )
   {
      cmsLog_debug( "Registrar Server Address = %s\n ", sipObj->registrarServer);
      strncpy( (char *)regSvrAddr, sipObj->registrarServer, length );

#if VOICE_IPV6_SUPPORT
      if ( (ret = dal_Voice_StripIpv6PrefixLength(NULL, regSvrAddr)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Could not strip address prefix length, ret=%d", ret);
      }
#endif /* VOICE_IPV6_SUPPORT */
   }
   else
   {
      regSvrAddr[0] = '\0';
   }

   cmsObj_free ((void **) &sipObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipRegistrarServerPort
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   port - Registrar Server Port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipRegistrarServerPort(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
   unsigned int        vpInst;
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj = NULL;

   cmsLog_debug( "vpInst:%d \n",parms->op[0] );

   vpInst = parms->op[0];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy registrar server port value */
   cmsLog_debug( "Registrar Server Port = %d ", sipObj->registrarServerPort );
   snprintf( (char*)port, length, "%d",sipObj->registrarServerPort );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipOutboundProxy
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   outgoingProxy - Outgoing Proxy Address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipOutboundProxy(DAL_VOICE_PARMS *parms, char *outgoingProxy, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = {  {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n", parms->op[0] );

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy outbound proxy value */
   if( sipObj->outboundProxy != NULL )
         {
      cmsLog_debug( "Outbound Proxy Server Address = %s\n ", sipObj->outboundProxy );
      strncpy( (char *) outgoingProxy, sipObj->outboundProxy, length );

#if VOICE_IPV6_SUPPORT
      if ( (ret = dal_Voice_StripIpv6PrefixLength(NULL, outgoingProxy)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Could not strip address prefix length, ret=%d", ret);
   }
#endif /* VOICE_IPV6_SUPPORT */
   }

   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipOutboundProxyPort
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   port - Outgoing Proxy Address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipOutboundProxyPort(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = {  {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0] );

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy outbound proxy port value */
   cmsLog_debug( "Outbound proxy Server Port = %d ", sipObj->outboundProxyPort );
   snprintf( (char*)port, length, "%d",sipObj->outboundProxyPort );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipRegisterExpires
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   regExpire - Registration Expire Timeout
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipRegisterExpires(DAL_VOICE_PARMS *parms, char *regExpire, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy expires time value */
   cmsLog_debug( "Register Expire time = %d\n ", sipObj->registerExpires);
   snprintf( (char*)regExpire, length,"%u",sipObj->registerExpires );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipTimerB
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**
**  OUTPUT PARMS:   tmrB - B timer value
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipTimerB(DAL_VOICE_PARMS *parms, char *tmrB, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy expires time value */
   cmsLog_debug( "SIP Timer B = %d\n ", sipObj->timerB);
   snprintf( (char*)tmrB, length,"%u",sipObj->timerB );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipTimerF
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**
**  OUTPUT PARMS:   tmrF - F timer value
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipTimerF(DAL_VOICE_PARMS *parms, char *tmrF, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy expires time value */
   cmsLog_debug( "SIP Timer F = %d\n ", sipObj->timerF);
   snprintf( (char*)tmrF, length,"%u",sipObj->timerF );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetSipRegisterRetryInterval
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   regRetry - Registration retry interval
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipRegisterRetryInterval(DAL_VOICE_PARMS *parms, char *regRetry, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = {  {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy expires time value */
   cmsLog_debug( "Register Retry Interval = %d\n ", sipObj->registerRetryInterval);
   snprintf( (char*)regRetry, length, "%u",sipObj->registerRetryInterval );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipUserAgentDomain
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   User Agent Domain ( FQDN )
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipUserAgentDomain(DAL_VOICE_PARMS *parms, char *fqdn, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;
   CmsRet              ret;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy user agent domain value */
   if( sipObj->userAgentDomain != NULL )
   {
      cmsLog_debug( "User Agent Domain = %s\n ", sipObj->userAgentDomain);
      strncpy( (char *)fqdn, sipObj->userAgentDomain, length );
   }

   cmsObj_free( (void **) &sipObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipUserAgentPort
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   port - User Agent port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetSipUserAgentPort(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = {  {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy user agent port value */
   cmsLog_debug( "User Agent Port = %d\n ", sipObj->userAgentPort);
   snprintf( (char*)port, length, "%u",sipObj->userAgentPort );
   cmsObj_free( (void **) &sipObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipDSCPMark
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   dscpMark - Value of SIP DSCP mark
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipDSCPMark(DAL_VOICE_PARMS *parms, char *dscpMark, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy DSCP mark value */
   snprintf( (char*)dscpMark, length, "%u",sipObj->DSCPMark );
   cmsObj_free((void **) &sipObj);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipMusicServer
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   musicServer - Music server address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipMusicServer(DAL_VOICE_PARMS *parms, char *musicServer, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n", parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy Music server port value */
   if( sipObj->X_BROADCOM_COM_MusicServer != NULL )
   {
      cmsLog_debug( "Music Server Address = %s\n ", sipObj->X_BROADCOM_COM_MusicServer);
      strncpy( (char *) musicServer, sipObj->X_BROADCOM_COM_MusicServer, length );

#if VOICE_IPV6_SUPPORT
      if ( (ret = dal_Voice_StripIpv6PrefixLength(NULL, musicServer)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Could not strip address prefix length, ret=%d", ret);
   }
#endif /* VOICE_IPV6_SUPPORT */
   }

   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipMusicServerPort
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   port - Music server port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipMusicServerPort(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy music server port value */
   cmsLog_debug( "Music Server Port = %d\n ", sipObj->X_BROADCOM_COM_MusicServerPort);
   snprintf( (char*)port, length, "%u",sipObj->X_BROADCOM_COM_MusicServerPort );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipConferencingURI
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   conferencingURI - Conferencing server URI
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipConferencingURI(DAL_VOICE_PARMS *parms, char *conferencingURI, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n", parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy conferencing URI port value */
   if( sipObj->X_BROADCOM_COM_ConferencingURI != NULL )
   {
      cmsLog_debug( "Conferencing Server URI = %s\n ", sipObj->X_BROADCOM_COM_ConferencingURI);
      strncpy( (char *) conferencingURI, sipObj->X_BROADCOM_COM_ConferencingURI, length );
   }

   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipConferencingOption
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   conferencingOption - Conferencing option
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipConferencingOption(DAL_VOICE_PARMS *parms, char *conferencingOption, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n", parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy conferencing URI port value */
   if( sipObj->X_BROADCOM_COM_ConferencingOption != NULL )
   {
      cmsLog_debug( "Conferencing Server URI = %s\n ", sipObj->X_BROADCOM_COM_ConferencingOption);
      strncpy( (char *) conferencingOption, sipObj->X_BROADCOM_COM_ConferencingOption, length );
   }

   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipSecDomainName
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   secDomainName - Secondary domain name
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipSecDomainName(DAL_VOICE_PARMS *parms, char *secDomainName, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n", parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy Secondary Domain Name value */
   if( sipObj->X_BROADCOM_COM_SecondaryDomainName != NULL )
   {
      cmsLog_debug( "Secondary Domain Name = %s\n ", sipObj->X_BROADCOM_COM_SecondaryDomainName);
      strncpy( (char *) secDomainName, sipObj->X_BROADCOM_COM_SecondaryDomainName, length );
   }

   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipSecProxyAddr
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   secProxyAddr - Secondary proxy address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipSecProxyAddr(DAL_VOICE_PARMS *parms, char *secProxyAddr, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n", parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy Secondary proxy address value */
   if( sipObj->X_BROADCOM_COM_SecondaryProxyAddress != NULL )
   {
      cmsLog_debug( "Secondary proxy address = %s\n ", sipObj->X_BROADCOM_COM_SecondaryProxyAddress);
      strncpy( (char *) secProxyAddr, sipObj->X_BROADCOM_COM_SecondaryProxyAddress, length );

#if VOICE_IPV6_SUPPORT
      if ( (ret = dal_Voice_StripIpv6PrefixLength(NULL, secProxyAddr)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Could not strip address prefix length, ret=%d", ret);
   }
#endif /* VOICE_IPV6_SUPPORT */
   }

   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipSecProxyPort
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   port - Secondary proxy port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipSecProxyPort(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy Secondary proxy port value */
   cmsLog_debug( "Secondary proxy port = %d\n ", sipObj->X_BROADCOM_COM_SecondaryProxyPort);
   snprintf( (char*)port, length, "%u",sipObj->X_BROADCOM_COM_SecondaryProxyPort );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipSecOutboundProxyAddr
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   secObProxyAddr - Secondary outbound proxy address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipSecOutboundProxyAddr(DAL_VOICE_PARMS *parms, char *secObProxyAddr, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n", parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy Secondary outbound proxy address value */
   if( sipObj->X_BROADCOM_COM_SecondaryOutboundProxyAddress != NULL )
   {
      cmsLog_debug( "Secondary outbound proxy address = %s\n ", sipObj->X_BROADCOM_COM_SecondaryOutboundProxyAddress);
      strncpy( (char *) secObProxyAddr, sipObj->X_BROADCOM_COM_SecondaryOutboundProxyAddress, length );

#if VOICE_IPV6_SUPPORT
      if ( (ret = dal_Voice_StripIpv6PrefixLength(NULL, secObProxyAddr)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Could not strip address prefix length, ret=%d", ret);
   }
#endif /* VOICE_IPV6_SUPPORT */
   }

   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipSecOutboundProxyPort
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   port - Secondary outbound proxy port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipSecOutboundProxyPort(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy Secondary outbound proxy port value */
   cmsLog_debug( "Secondary outbound proxy port = %d\n ", sipObj->X_BROADCOM_COM_SecondaryOutboundProxyPort);
   snprintf( (char*)port, length, "%u",sipObj->X_BROADCOM_COM_SecondaryOutboundProxyPort );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipSecRegistrarAddr
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   secRegistrarAddr - Secondary registrar address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipSecRegistrarAddr(DAL_VOICE_PARMS *parms, char *secRegistrarAddr, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n", parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy Secondary outbound proxy address value */
   if( sipObj->X_BROADCOM_COM_SecondaryRegistrarAddress != NULL )
   {
      cmsLog_debug( "Secondary registrar address = %s\n ", sipObj->X_BROADCOM_COM_SecondaryRegistrarAddress);
      strncpy( (char *) secRegistrarAddr, sipObj->X_BROADCOM_COM_SecondaryRegistrarAddress, length );

#if VOICE_IPV6_SUPPORT
      if ( (ret = dal_Voice_StripIpv6PrefixLength(NULL, secRegistrarAddr)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Could not strip address prefix length, ret=%d", ret);
   }
#endif /* VOICE_IPV6_SUPPORT */
   }

   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipSecRegistrarPort
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   port - Secondary registrar port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipSecRegistrarPort(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy Secondary registrar port value */
   cmsLog_debug( "Secondary registrar port = %d\n ", sipObj->X_BROADCOM_COM_SecondaryRegistrarPort);
   snprintf( (char*)port, length, "%u",sipObj->X_BROADCOM_COM_SecondaryRegistrarPort );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipFailoverEnable
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   failover - SIP failover enable
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipFailoverEnable(DAL_VOICE_PARMS *parms, char *failover, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy SIP tag matching value */
   cmsLog_debug( "SIP failover = %d\n ", sipObj->X_BROADCOM_COM_SipFailoverEnable);
   snprintf( (char*)failover, length, "%u",sipObj->X_BROADCOM_COM_SipFailoverEnable );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipOptionsEnable
**
**  PURPOSE:        Retreive SIP OPTIONS enable flag from MDM.
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   sipOptions - char * containing SIP OPTIONS
**                               config setting.  Can be:
**                  "0" for OFF
**                  "1" for ON
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  CMSRET error code if error as defined in cms.h
*******************************************************************/
CmsRet dalVoice_GetSipOptionsEnable(DAL_VOICE_PARMS *parms, char *sipOptions, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy SIP tag matching value */
   cmsLog_debug( "SIP OPTIONS = %d\n ", sipObj->X_BROADCOM_COM_SipOptionsEnable);
   snprintf( (char*)sipOptions, length, "%u",sipObj->X_BROADCOM_COM_SipOptionsEnable );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipToTagMatching
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   tagMatching - SIP to tag matching
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipToTagMatching(DAL_VOICE_PARMS *parms, char *tagMatching, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy SIP tag matching value */
   cmsLog_debug( "SIP To Tag Matching = %d\n ", sipObj->X_BROADCOM_COM_ToTagMatching);
   snprintf( (char*)tagMatching, length, "%u",sipObj->X_BROADCOM_COM_ToTagMatching );
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipTransport
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   transport[0] - enumerated transport types defined in
**                  DAL_VOICE_SIP_TRANSPORTS
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetSipTransport(DAL_VOICE_PARMS *parms, char *transport, unsigned int length )
{
   char                temp[TEMP_CHARBUF_SIZE];
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy transport value */
   if( sipObj->registrarServerTransport != NULL )
   {
      cmsLog_debug( "transport = %s\n ", sipObj->registrarServerTransport);
      strncpy( (char *)temp, sipObj->registrarServerTransport, strlen(sipObj->registrarServerTransport) + 1);

      if( strcasecmp(temp, MDMVS_TLS) == 0 )
      {
		 *transport = DAL_VOICE_SIP_TRANSPORT_TLS;
      }
      else if ( strcasecmp(temp, MDMVS_TCP) == 0 )
      {
		 *transport = DAL_VOICE_SIP_TRANSPORT_TCP;
      }
      else
      {
         /* UDP */
         *transport = DAL_VOICE_SIP_TRANSPORT_UDP;
      }
   }

   cmsObj_free ((void **) &sipObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipTransportString
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   String Value of SIP transport
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipTransportString(DAL_VOICE_PARMS *parms, char *transport, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy transport value */
   if( sipObj->registrarServerTransport != NULL )
   {
      cmsLog_debug( "transport = %s\n ", sipObj->registrarServerTransport);
      strncpy( (char *)transport, sipObj->registrarServerTransport, length);
   }

   cmsObj_free ((void **) &sipObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipBackToPrimOption
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetSipBackToPrimOption(DAL_VOICE_PARMS *parms, char *failover, unsigned int length )
{
   char                temp[TEMP_CHARBUF_SIZE];
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy transport value */
   if( sipObj->X_BROADCOM_COM_BackToPrimMode != NULL )
   {
      cmsLog_debug( "failover = %s\n ", sipObj->X_BROADCOM_COM_BackToPrimMode);
      strncpy( (char *)temp, sipObj->X_BROADCOM_COM_BackToPrimMode, strlen(sipObj->X_BROADCOM_COM_BackToPrimMode) + 1);

      if ( strcasecmp(temp, MDMVS_SILENT) == 0 )
      {
		 *failover = DAL_VOICE_BACKTOPRIM_SILENT;
      }
      else if ( strcasecmp(temp, MDMVS_DEREGISTRATION) == 0 )
      {
		 *failover = DAL_VOICE_BACKTOPRIM_DEREG;
      }
      else if ( strcasecmp(temp, MDMVS_SILENTDEREGISTRATION) == 0 )
      {
		 *failover = DAL_VOICE_BACKTOPRIM_SILENT_DEREG;
      }
      else
      {
         /* UDP */
		 *failover = DAL_VOICE_BACKTOPRIM_DISABLED;
      }
   }

   cmsObj_free ((void **) &sipObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipBackToPrimOptionString
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   String Value of SIP failover back-to-primary option
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipBackToPrimOptionString(DAL_VOICE_PARMS *parms, char *failover, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy transport value */
   if( sipObj->X_BROADCOM_COM_BackToPrimMode != NULL )
   {
      cmsLog_debug( "failover = %s\n ", sipObj->X_BROADCOM_COM_BackToPrimMode);
      strncpy( (char *)failover, sipObj->X_BROADCOM_COM_BackToPrimMode, length);
   }

   cmsObj_free ((void **) &sipObj );

   return ( ret );
}

#endif /* SIPLOAD */

/*****************************************************************
**  FUNCTION:       dalVoice_GetSrtpOptionString
**
**  PURPOSE:        Get SRTP usage option (mandatory, optional or disabled)
**                    in srting form
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   String Value of SRTP option
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSrtpOptionString(DAL_VOICE_PARMS *parms, char *option, unsigned int length )
{
   unsigned int            vpInst          = parms->op[0];
   CmsRet                  ret;
   InstanceIdStack         iidStack        = EMPTY_INSTANCE_ID_STACK;
   VoiceProfRtpSrtpObject *obj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]         = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP_SRTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice object \n" );
      return ( ret );
   }

   /* Copy transport value */
   if( obj->X_BROADCOM_COM_SrtpUsageOption != NULL )
   {
      cmsLog_debug( "SRTP option = %s\n ", obj->X_BROADCOM_COM_SrtpUsageOption);
      strncpy( (char *)option, obj->X_BROADCOM_COM_SrtpUsageOption, length);
   }

   cmsObj_free ((void **) &obj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSrtpOption
**
**  PURPOSE:        Get SRTP usage option (mandatory, optional or disabled)
**                    in enum form
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   String Value of SRTP option
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSrtpOption(DAL_VOICE_PARMS *parms, char *option, unsigned int length )
{
   char                    temp[TEMP_CHARBUF_SIZE];
   unsigned int            vpInst          = parms->op[0];
   CmsRet                  ret;
   InstanceIdStack         iidStack        = EMPTY_INSTANCE_ID_STACK;
   VoiceProfRtpSrtpObject *obj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]         = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP_SRTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice object \n" );
      return ( ret );
   }

   /* Copy SRTP option value */
   if( obj->X_BROADCOM_COM_SrtpUsageOption != NULL )
   {
      cmsLog_debug( "SRTP option = %s\n ", obj->X_BROADCOM_COM_SrtpUsageOption);
      strncpy( (char *)temp, obj->X_BROADCOM_COM_SrtpUsageOption,
            strlen(obj->X_BROADCOM_COM_SrtpUsageOption) + 1);

      if( strcasecmp(temp, MDMVS_OPTIONAL) == 0 )
      {
         *option = DAL_VOICE_SRTP_OPTIONAL;
      }
      else if ( strcasecmp(temp, MDMVS_MANDATORY) == 0 )
      {
         *option = DAL_VOICE_SRTP_MANDATORY;
      }
      else
      {
         /* Disabled */
         *option = DAL_VOICE_SRTP_DISABLED;
      }
   }

   cmsObj_free ((void **) &obj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlStats
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst - parms->op[1]
**
**  OUTPUT PARMS:   DAL_VOICE_CALL_STATS_BLK stats block
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlStats(DAL_VOICE_PARMS *parms, DAL_VOICE_CALL_STATS_BLK *statsBlk )
{
   unsigned int            vpInst     = parms->op[0];
   unsigned int            lineInst   = parms->op[1];
   CmsRet                  ret;
   InstanceIdStack         iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineStatsObject   *statsObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                             {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                             {MDMOID_VOICE_LINE_STATS, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                             {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile Line object */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&statsObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line instance object \n" );
      return ( ret );
   }
   /* If value of object doesnt exist, return */
   if( statsObj == NULL )
   {
      cmsLog_error( "Object value does not exist \n" );
      cmsObj_free((void **) &statsObj);
      return CMSRET_REQUEST_DENIED;
   }

   /* Copy over stats object */
   *statsBlk = *statsObj;

   cmsObj_free((void **) &statsObj);

   return ret;
}
/*****************************************************************
**  FUNCTION:       dalVoice_GetVlEnable
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   Line Enable status (1 if enabled)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlEnable(DAL_VOICE_PARMS *parms, char *lineEnabled, unsigned int length )
{
   char                *temp[TEMP_CHARBUF_SIZE];
   unsigned int        vpInst        = parms->op[0];
   unsigned int        lineInst      = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack      = EMPTY_INSTANCE_ID_STACK;
   VoiceLineObject     *voiceLineObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]       = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                         {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                         {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n",parms->op[1]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object (vpInst = %d, lineInst = %d) \n",
                    vpInst, lineInst );
      return ( ret );
   }

   /* Copy Line enabled value */
   if( voiceLineObj->enable != NULL )
   {
      cmsLog_debug( "line enable status = %s\n ", voiceLineObj->enable);
      strncpy((char *)temp, voiceLineObj->enable, strlen(voiceLineObj->enable) + 1);

      /* convert line status string to boolean value */
      if( strcasecmp((const char *)temp, MDMVS_ENABLED) == 0 )
      {
         snprintf( (char*)lineEnabled, length, "%u", TRUE );
      }
      else if( strcasecmp((const char *)temp, MDMVS_DISABLED) == 0 )
      {
         snprintf( (char*)lineEnabled, length, "%u", FALSE );
      }
      else
      {
         /* Brcm don't support quenscient state */
         cmsLog_error( "INVALID parameter value \n" );
         ret = CMSRET_INVALID_PARAM_VALUE;
      }
   }

   cmsObj_free( (void **) &voiceLineObj );


   return  ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetVlDisable
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst - parms->op[1]
**
**  OUTPUT PARMS:   Line Disable status (1 if disabled)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
** NOTE: There is no line disable object in TR104 specifying
**       whether the line is enabled or disabled. This function simply
**       returns the inverted value of dalVoice_GetVlEnable
**
*******************************************************************/
CmsRet dalVoice_GetVlDisable(DAL_VOICE_PARMS *parms, char *lineDisabled, unsigned int length )
{
   char                *temp[TEMP_CHARBUF_SIZE];
   unsigned int        vpInst        = parms->op[0];
   unsigned int        lineInst      = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack      = EMPTY_INSTANCE_ID_STACK;
   VoiceLineObject     *voiceLineObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]       = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                         {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                         {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d, line:%d\n", parms->op[0], parms->op[1]);

   if (lineInst < 1 || vpInst < 1)
   {
      /* Not necessary an error because the line may not be created */
      cmsLog_debug("invalid lineInst number = %d or vpInst number = %d\n", lineInst, vpInst);
      snprintf( (char*)lineDisabled, length, "%u", TRUE );
      return CMSRET_SUCCESS;
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object (vpInst = %d, lineInst = %d) \n",
                    vpInst, lineInst );
      return ( ret );
   }

   /* Copy Line enabled value */
   if( voiceLineObj->enable != NULL )
   {
      cmsLog_debug( "line enable status = %s\n ", voiceLineObj->enable);
      strncpy((char *)temp, voiceLineObj->enable, strlen(voiceLineObj->enable) + 1);

      /* convert line status string to boolean value */
      if( strcasecmp((const char *)temp, MDMVS_ENABLED) == 0 )
      {
         snprintf( (char*)lineDisabled, length, "%u", FALSE );
      }
      else if( strcasecmp((const char *)temp, MDMVS_DISABLED) == 0 )
      {
         snprintf( (char*)lineDisabled, length, "%u", TRUE );
      }
      else
      {
         /* Brcm don't support quenscient state */
         cmsLog_error( "INVALID parameter value \n" );
         ret = CMSRET_INVALID_PARAM_VALUE;
      }
   }

   cmsObj_free( (void **) &voiceLineObj );

   return  ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetVlVoipStatus
**
**  PUROPOSE:       Get VOIP service status.
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   lineStatus - String of voip service status
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlVoipStatus(DAL_VOICE_PARMS *parms, char *lineStatus, unsigned int length )
{
   unsigned int        vpInst        = parms->op[0];
   unsigned int        lineInst      = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack      = EMPTY_INSTANCE_ID_STACK;
   VoiceLineObject     *voiceLineObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]       = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                         {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                         {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object (vpInst = %d, lineInst = %d) \n",
                    vpInst, lineInst );
      return ( ret );
   }

   /* Copy Line status value */
   if( voiceLineObj->status != NULL )
   {
      strncpy((char *)lineStatus, voiceLineObj->status, length);
      lineStatus[length-1] = '\0';
   }

   cmsObj_free( (void **) &voiceLineObj );

   return  ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetVoicePhyInterfaceList
**
**  PUROPOSE:
**
**  INPUT PARMS:    none
**
**  OUTPUT PARMS:   list  - list of physical endpoint ID
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVoicePhyInterfaceList(DAL_VOICE_PARMS *parms, char *list, unsigned int length )
{
   CmsRet          ret      = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   VoicePhyIntfObject *obj = NULL;
   char tmp[20];

   if(list == NULL)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* Iterate through list of all pstn objects in the system */
   list[0] = '\0';
   while ( (ret = cmsObj_getNextFlags(MDMOID_VOICE_PHY_INTF, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&obj)) == CMSRET_SUCCESS )
   {
      sprintf(tmp, "%u", obj->interfaceID);
      if(strlen(list))
         strncat(list, ",", length);
      strncat(list, tmp, length);
      cmsObj_free( (void **)&obj);
   }

   return CMSRET_SUCCESS;

}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlPhyReferenceList
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   physEptId - Physical endpoint ID
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlPhyReferenceList(DAL_VOICE_PARMS *parms, char *physEptId, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineObject     *lineObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vp:%d line:%d \n",parms->op[0], parms->op[1]);

   /* Do not execute code if the lineInst is invalid */
   if (lineInst < 1)
   {
      cmsLog_debug("invalid lineInst number = %d", lineInst);
      strncpy( physEptId, "", length);
      return CMSRET_SUCCESS;
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&lineObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line instance object \n" );
      return ( ret );
   }

   /* Mdm will return (null) point if no default value present in phyintflist, so alway check for null pointer */
   if ( lineObj->phyReferenceList == NULL )
   {
      cmsObj_free((void **) &lineObj);

      /* CCTK allows a line to have no associated phyFxs endpoints */
      snprintf( physEptId, length, "%c",'\0');
   }
   else
   {
      /* CMGR-CCTK accepts multiple physical endpoints attached to the same line */
      cmsLog_debug( "phyReferenceList = %s\n ", lineObj->phyReferenceList);
      snprintf( physEptId, length, "%s", lineObj->phyReferenceList );
   }


   cmsObj_free((void **) &lineObj);

   return ( ret );
   }

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlAssociatedNonFxsPhyType
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   physEptId - Physical endpoint ID
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlAssociatedNonFxsPhyType(DAL_VOICE_PARMS *parms, char *type, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineObject     *lineObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vp:%d line:%d \n",parms->op[0], parms->op[1]);

   /* Do not execute code if the lineInst is invalid */
   if (lineInst < 1)
   {
      cmsLog_debug("invalid lineInst number = %d", lineInst);
      snprintf( type, length, "%s", MDMVS_UNASSIGNED );
      return CMSRET_SUCCESS;
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&lineObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line instance object \n" );
      return ( ret );
   }

   /* check type */
   snprintf( type, length, "%s", lineObj->X_BROADCOM_COM_AssociatedNonFxsPhyIntfType );

   cmsObj_free((void **) &lineObj);

   return ( ret );
}




#ifdef SIPLOAD

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlSipURI
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst  - parms->op[1]
**
**
**  OUTPUT PARMS:   Account Id/extension
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlSipURI(DAL_VOICE_PARMS *parms, char *userId, unsigned int length )
{
   unsigned int       vpInst       = parms->op[0];
   unsigned int       lineInst     = parms->op[1];
   CmsRet             ret;
   InstanceIdStack    iidStack     = EMPTY_INSTANCE_ID_STACK;
   VoiceLineSipObject *sipObj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]     = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_LINE_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                       {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "Line:%d \n",parms->op[1] );

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line SIP object \n" );
      return ( ret );
   }

   /* Copy SIP URI value */
   if( sipObj->URI != NULL )
      {
      cmsLog_debug( "User Id = %s\n ", sipObj->URI );
      strncpy( (char *) userId, sipObj->URI, length);
   }

   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlSipAuthUserName
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   authName - SIP Auth username
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetVlSipAuthUserName(DAL_VOICE_PARMS *parms, char *authName, unsigned int length )
{
   unsigned int       vpInst, lineInst;
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineSipObject *sipObj = NULL;

   cmsLog_debug( "Line:%d \n",parms->op[1]);

   vpInst = parms->op[0];
   lineInst = parms->op[1];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line SIP object \n" );
      return ( ret );
   }

   /* Copy auth username value */
   if( sipObj->authUserName != NULL )
   {
      cmsLog_debug( "authUserName = %s\n ", sipObj->authUserName);
      strncpy( (char *) authName, sipObj->authUserName, length);
   }

   cmsObj_free( (void **) &sipObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlSipAuthPassword
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   passwd - Password
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlSipAuthPassword(DAL_VOICE_PARMS *parms, char *passwd, unsigned int length )
{
   unsigned int       vpInst, lineInst;
   CmsRet             ret;
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineSipObject *sipObj = NULL;

   vpInst = parms->op[0];
   lineInst = parms->op[1];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "Line:%d \n",parms->op[1] );

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line SIP object \n" );
      return ( ret );
   }

   /* Copy auth password value */
   if( sipObj->authPassword != NULL )
   {
      cmsLog_debug( "AuthPasswd = %s\n ", sipObj->authPassword );
      strncpy( (char *) passwd, sipObj->authPassword, length);
   }

   cmsObj_free( (void **) &sipObj );

   return ( ret );
}

#endif /* SIPLOAD */
/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFFeatureEnabled
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**                  lineInst - parms->op[1]
**                  call feature - parms->op[2]
**
**  OUTPUT PARMS:   Call feature enabled flag
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFFeatureEnabled(DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   unsigned int       vpInst, lineInst, featId;
   CmsRet             ret;
   /* if getVlCFFeatureValue doesn't change getValInt, it will end up -1 later on */
   int                getValInt = -1;

   vpInst = parms->op[0];
   lineInst = parms->op[1];
   featId = (DAL_VOICE_FEATURE_CODE)parms->op[2];

   if( (ret = getVlCFFeatureValue(CFCALLTYPE_ENABLED, vpInst, lineInst, featId, &getValInt)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't get call feature ENABLED value\n" );
   }
   else if(getValInt != -1)
   {
      snprintf( (char*)getVal, length, "%d",getValInt );
   }

   cmsLog_debug( "feature get: value %d\n", getValInt);
   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFFeatureAction
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**                  lineInst - parms->op[1]
**                  call feature - parms->op[2]
**
**  OUTPUT PARMS:   Call feature action flag
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFFeatureAction(DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   unsigned int       vpInst, lineInst, featId;
   CmsRet             ret;
   int                getValInt = 0;

   cmsLog_debug( "line:%d \n",parms->op[1]);

   vpInst = parms->op[0];
   lineInst = parms->op[1];
   featId = (DAL_VOICE_FEATURE_CODE)parms->op[2];

   if( (ret = getVlCFFeatureValue(CFCALLTYPE_ACTION, vpInst, lineInst, featId, &getValInt)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't get call feature ACTION value\n" );
   }
   else
   {
      snprintf( (char*)getVal, length, "%d",getValInt );
   }

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFFeatureStarted
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**                  lineInst - parms->op[1]
**                  call feature - parms->op[2]
**
**  OUTPUT PARMS:   Call feature action flag
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFFeatureStarted(DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   unsigned int       vpInst, lineInst, featId;
   CmsRet             ret;
   int                getValInt = 0;

   cmsLog_debug( "line:%d \n",parms->op[1]);

   vpInst     = parms->op[0];
   lineInst   = parms->op[1];
   featId     = (DAL_VOICE_FEATURE_CODE)parms->op[2];

   if( (ret = getVlCFFeatureValue(CFCALLTYPE_STARTED, vpInst, lineInst, featId, &getValInt)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't get call feature STARTED value\n" );
   }
   else
   {
      snprintf( (char*)getVal, length, "%d",getValInt );
   }

   return ( ret );
}


CmsRet dalVoice_GetVlCFCallFwdAll( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   DAL_VOICE_PARMS featureParms;
   featureParms.op[0] = parms->op[0];
   featureParms.op[1] = parms->op[1];
   featureParms.op[2] = DAL_VOICE_FEATURE_CODE_FWD_ALL;

   return(dalVoice_GetVlCFFeatureEnabled( &featureParms, getVal, length ));
}


CmsRet dalVoice_GetVlCFCallFwdBusy( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   DAL_VOICE_PARMS featureParms;
   featureParms.op[0] = parms->op[0];
   featureParms.op[1] = parms->op[1];
   featureParms.op[2] = DAL_VOICE_FEATURE_CODE_FWD_BUSY;

   return(dalVoice_GetVlCFFeatureEnabled( &featureParms, getVal, length ));
}


CmsRet dalVoice_GetVlCFCallFwdNoAns( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   DAL_VOICE_PARMS featureParms;
   featureParms.op[0] = parms->op[0];
   featureParms.op[1] = parms->op[1];
   featureParms.op[2] = DAL_VOICE_FEATURE_CODE_FWD_NOANS;

   return(dalVoice_GetVlCFFeatureEnabled( &featureParms, getVal, length ));
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallFwdNum
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**                  lineInst - parms->op[1]
**
**  OUTPUT PARMS:   cfNumber - Call Forward Number
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallFwdNum(DAL_VOICE_PARMS *parms, char *cfNumber, unsigned int length )
{
   unsigned int       vpInst, lineInst;
   CmsRet             ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   cmsLog_debug( "line:%d \n",parms->op[1]);

   vpInst = parms->op[0];
   lineInst = parms->op[1];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* Copy call fowarding number */
   if ( voiceLineCallingFeaturesObj->callForwardUnconditionalNumber != NULL )
   {
      cmsLog_debug( "call forwarding number = %s\n",
               voiceLineCallingFeaturesObj->callForwardUnconditionalNumber);

      strncpy(cfNumber, voiceLineCallingFeaturesObj->callForwardUnconditionalNumber, length);

   }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFWarmLineNum
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**                  lineInst - parms->op[1]
**
**  OUTPUT PARMS:   cfNumber - Warm Line Number
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFWarmLineNum(DAL_VOICE_PARMS *parms, char *warmLineNumber, unsigned int length )
{
   unsigned int        vpInst, lineInst;
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   cmsLog_debug( "line:%d \n",parms->op[1]);

   vpInst = parms->op[0];
   lineInst = parms->op[1];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* Copy warm line number */
   if ( voiceLineCallingFeaturesObj->X_BROADCOM_COM_WarmLineNumber != NULL )
   {
      cmsLog_debug( "warm line number = %s\n",
               voiceLineCallingFeaturesObj->X_BROADCOM_COM_WarmLineNumber);

      strncpy(warmLineNumber, voiceLineCallingFeaturesObj->X_BROADCOM_COM_WarmLineNumber, length);

   }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ( ret );
}

CmsRet dalVoice_GetVlCFCallWaiting( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   DAL_VOICE_PARMS featureParms;
   featureParms.op[0] = parms->op[0];
   featureParms.op[1] = parms->op[1];
   featureParms.op[2] = DAL_VOICE_FEATURE_CODE_CALLWAIT;

   return(dalVoice_GetVlCFFeatureEnabled( &featureParms, getVal, length ));
}


CmsRet dalVoice_GetVlCFCallBarring( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   DAL_VOICE_PARMS featureParms;
   featureParms.op[0] = parms->op[0];
   featureParms.op[1] = parms->op[1];
   featureParms.op[2] = DAL_VOICE_FEATURE_CODE_CALL_BARRING;

   return(dalVoice_GetVlCFFeatureEnabled( &featureParms, getVal, length ));
}

CmsRet dalVoice_GetVlCFWarmLine( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   DAL_VOICE_PARMS featureParms;
   featureParms.op[0] = parms->op[0];
   featureParms.op[1] = parms->op[1];
   featureParms.op[2] = DAL_VOICE_FEATURE_CODE_WARM_LINE;

   return(dalVoice_GetVlCFFeatureEnabled( &featureParms, getVal, length ));
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallBarringMode
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**                  lineInst - parms->op[1]
**
**  OUTPUT PARMS:   mode - Call barring mode
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallBarringMode(DAL_VOICE_PARMS *parms, char *mode, unsigned int length )
{
   unsigned int       vpInst      = parms->op[0];
   unsigned int       lineInst    = parms->op[1];
   CmsRet             ret;
   InstanceIdStack    iidStack    = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n",parms->op[1]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* Copy call barring user PIN */
   if ( voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringMode != NULL )
   {
      cmsLog_debug( "call barring mode = %s\n",
                    voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringMode);

      strncpy( mode, voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringMode,
               length );
   }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);
   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallBarringPin
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   pin - Call barring user PIN
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallBarringPin(DAL_VOICE_PARMS *parms, char *pin, unsigned int length )
{
   unsigned int       vpInst      = parms->op[0];
   unsigned int       lineInst    = parms->op[1];
   CmsRet             ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n",parms->op[1]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* Copy call barring user PIN */
   if ( voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringUserPin != NULL )
   {
      cmsLog_debug( "call barring user PIN = %s\n",
                    voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringUserPin);

      strncpy( pin, voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringUserPin,
               length );
   }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);
   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallBarringDigitMap
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   digitMap - Call barring digit map
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallBarringDigitMap(DAL_VOICE_PARMS *parms, char *digitMap, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n",parms->op[1]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* Copy call barring digit map */
   if ( voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringDigitMap != NULL )
   {
      cmsLog_debug( "call barring digit map = %s\n",
                    voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringDigitMap);

      strncpy( digitMap, voiceLineCallingFeaturesObj->X_BROADCOM_COM_CallBarringDigitMap, length);

   }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFVisualMWI
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   vwmi - Visual Message waiting indication
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFVisualMWI(DAL_VOICE_PARMS *parms, char *vmwi, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n",parms->op[1]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* Copy VMWI value */
   cmsLog_debug( "vmwi = %d\n",
                voiceLineCallingFeaturesObj->X_BROADCOM_COM_VMWIEnable);
   snprintf( (char*)vmwi, length, "%u",voiceLineCallingFeaturesObj->X_BROADCOM_COM_VMWIEnable );
   cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ( ret );
}

CmsRet dalVoice_GetVlCFAnonCallBlck( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   DAL_VOICE_PARMS featureParms;
   featureParms.op[0] = parms->op[0];
   featureParms.op[1] = parms->op[1];
   featureParms.op[2] = DAL_VOICE_FEATURE_CODE_ANON_REJECT;

   return(dalVoice_GetVlCFFeatureEnabled( &featureParms, getVal, length ));
}


CmsRet dalVoice_GetVlCFAnonymousCalling( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   DAL_VOICE_PARMS featureParms;
   featureParms.op[0] = parms->op[0];
   featureParms.op[1] = parms->op[1];
   featureParms.op[2] = DAL_VOICE_FEATURE_CODE_ANON_CALL;

   return(dalVoice_GetVlCFFeatureEnabled( &featureParms, getVal, length ));
}


CmsRet dalVoice_GetVlCFDoNotDisturb( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   DAL_VOICE_PARMS featureParms;
   featureParms.op[0] = parms->op[0];
   featureParms.op[1] = parms->op[1];
   featureParms.op[2] = DAL_VOICE_FEATURE_CODE_DND;

   return(dalVoice_GetVlCFFeatureEnabled( &featureParms, getVal, length ));
}

CmsRet dalVoice_GetRedOptionString( DAL_VOICE_PARMS *parms, char *option, unsigned int length )
{
   unsigned int            vpInst          = parms->op[0];
   CmsRet                  ret;
   InstanceIdStack         iidStack        = EMPTY_INSTANCE_ID_STACK;
   VoiceProfRtpRedundancyObject *obj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]         = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP_REDUNDANCY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice object \n" );
      return ( ret );
   }

   /* Copy transport value */
   if( obj->enable )
   {
      cmsLog_debug( "Red enabled");
      snprintf( (char*)option, length, "%u",obj->voiceRedundancy);
   }
   else
   {
      snprintf( (char*)option, length, "Disabled");
   }

   cmsObj_free ((void **) &obj );

   return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallerIDName
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   userName - Username/DisplayName
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallerIDName(DAL_VOICE_PARMS *parms, char *userName, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "Line:%d \n",parms->op[1] );

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* Copy caller ID value */
   if ( voiceLineCallingFeaturesObj->callerIDName != NULL )
   {
      cmsLog_debug( "CallerId name = %s\n ", voiceLineCallingFeaturesObj->callerIDName );
      strncpy( (char *) userName, voiceLineCallingFeaturesObj->callerIDName, length );
   }

   cmsObj_free( (void **) &voiceLineCallingFeaturesObj );

   return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFMWIEnable
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   enable - MWI enable
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFMWIEnable(DAL_VOICE_PARMS *parms, char *enable, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n",parms->op[1]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   cmsLog_debug( "mwiEnable = %d\n", voiceLineCallingFeaturesObj->MWIEnable);
   snprintf( (char*)enable, length, "%u",voiceLineCallingFeaturesObj->MWIEnable );

   cmsObj_free( (void **) &voiceLineCallingFeaturesObj );

   return ( ret );
}

#ifdef SIPLOAD

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCLCodecList
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   codec - Priority sorted list of encoders
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCLCodecList(DAL_VOICE_PARMS *parms, char *codec, unsigned int length )
{
   unsigned int       vpInst   = parms->op[0];
   unsigned int       lineInst = parms->op[1];
   CmsRet             ret;

   cmsLog_debug( "line:%d \n",parms->op[1]);

   if( (ret = getVlCLCodecList( vpInst, lineInst, codec, length)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't get codec \n" );
   }

   return ( ret );
}

#ifdef BRCM_SIP_TLS_SUPPORT
/*****************************************************************
**  FUNCTION:       dalVoice_GetLocalSipCertPrivKey
**
**  PUROPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   Private key of the "sipcert" local certificate,
**                   provided it is configured in MDM
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetLocalSipCertPrivKey(DAL_VOICE_PARMS *parms, char *privKey, unsigned int length)
{
   CertificateCfgObject *certCfg = NULL;
   char sipCertName[64] = "sipcert";
   CmsRet ret = CMSRET_SUCCESS;

   if (cmsObj_get(MDMOID_CERTIFICATE_CFG, NULL,
                  OGF_DEFAULT_VALUES, (void **) &certCfg) != CMSRET_SUCCESS)
   {
      return CMSRET_INTERNAL_ERROR;
   }

   ret = dalCert_getCert((char *)sipCertName, CERT_LOCAL, certCfg);

   if (certCfg->privKey != NULL)
   {
      strncpy((char *)privKey, certCfg->privKey, strlen(certCfg->privKey));
   }

   cmsObj_free((void **) &certCfg);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetLocalSipCertContents
**
**  PUROPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   Contents of the "sipcert" local certificate,
**                   provided it is configured in MDM
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetLocalSipCertContents(DAL_VOICE_PARMS *parms, char *contents, unsigned int length)
{
   CertificateCfgObject *certCfg = NULL;
   char sipCertName[64] = "sipcert";
   CmsRet ret = CMSRET_SUCCESS;

   if (cmsObj_get(MDMOID_CERTIFICATE_CFG, NULL,
                  OGF_DEFAULT_VALUES, (void **) &certCfg) != CMSRET_SUCCESS)
   {
      return CMSRET_INTERNAL_ERROR;
   }

   ret = dalCert_getCert((char *)sipCertName, CERT_LOCAL, certCfg);

   if (certCfg->content != NULL)
   {
      strncpy((char *)contents, certCfg->content, strlen(certCfg->content));
   }

   cmsObj_free((void **) &certCfg);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetTrustedCaSipCertContents
**
**  PUROPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   Contents of the "sipcert" trusted certificate,
**                   provided it is configured in MDM
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetTrustedCaSipCertContents(DAL_VOICE_PARMS *parms, char *contents, unsigned int length)
{
   CertificateCfgObject *certCfg = NULL;
   char sipCertName[64] = "sipcert";
   CmsRet ret = CMSRET_SUCCESS;

   if (cmsObj_get(MDMOID_CERTIFICATE_CFG, NULL,
                  OGF_DEFAULT_VALUES, (void **) &certCfg) != CMSRET_SUCCESS)
   {
      return CMSRET_INTERNAL_ERROR;
   }

   ret = dalCert_getCert((char *)sipCertName, CERT_CA, certCfg);

   if (certCfg->content != NULL)
   {
      strncpy((char *)contents, certCfg->content, strlen(certCfg->content));
   }

   cmsObj_free((void **) &certCfg);

   return ret;
}

#endif /* BRCM_SIP_TLS_SUPPORT */
#endif /* SIPLOAD */

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCLPacketizationPeriod
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   ptime - Packetization period
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCLPacketizationPeriod(DAL_VOICE_PARMS *parms, char *ptime, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCodecListObject *voiceLineCodecListObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_CODEC_LIST, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n",parms->op[1]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCodecListObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve codec line object \n" );
      return ( ret );
   }

   /* If the packetization period does not exist return failure */
   if ( voiceLineCodecListObj->packetizationPeriod == NULL )
   {
      cmsLog_error( "packetization period value does not exist\n" );
      cmsObj_free((void **) &voiceLineCodecListObj);
      return CMSRET_REQUEST_DENIED;
   }

   /* Copy the first token from the ptime string, since we support a single ptime setting per account */
   snprintf( (char*)ptime, length, "%u",atoi(strtok(voiceLineCodecListObj->packetizationPeriod, ",\n")) );
   cmsObj_free((void **) &voiceLineCodecListObj);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCLSilenceSuppression
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   vad - Silence suppression
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCLSilenceSuppression(DAL_VOICE_PARMS *parms, char *vad, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCodecListObject *voiceLineCodecListObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_CODEC_LIST, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n",parms->op[1]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCodecListObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve codec line object \n" );
      return ( ret );
   }

   /* Copy first VAD value, since we support a single VAD setting per account */
   snprintf( (char*)vad, length, "%u",voiceLineCodecListObj->silenceSuppression );
   cmsObj_free((void **) &voiceLineCodecListObj);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCLEncoder
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**                  lineInst  - parms->op[1]
**                  codec priority - parms->op[2]
**
**  OUTPUT PARMS:   codec - Encoder of specific priority
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCLEncoder(DAL_VOICE_PARMS *parms, char *codec, unsigned int length )
{
   unsigned int       vpInst    = parms->op[0];
   unsigned int       lineInst  = parms->op[1];
   unsigned int       codecPrio = parms->op[2];
   CmsRet             ret;
   UINT32             codecInt  = 0;

   cmsLog_debug( "line:%d \n",parms->op[1]);

   if( (ret = getVlCLEncoder( vpInst, lineInst, codecPrio, &codecInt)) == CMSRET_SUCCESS )
   {
      snprintf( (char*)codec, length, "%u",codecInt );
   }

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetVlVPTransmitGain
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   tx - Tx gain
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlVPTransmitGain(DAL_VOICE_PARMS *parms, char *tx, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineProcessingObject *obj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_PROCESSING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n",parms->op[1]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line processing object \n" );
      return ( ret );
   }

   cmsLog_debug( "TxGain = %d\n ", obj->transmitGain);
   snprintf( (char*)tx, length, "%d",obj->transmitGain / GAINUNIT);
   cmsObj_free( (void **) &obj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlVPRecieveGain
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   rx - Rx gain
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlVPRecieveGain(DAL_VOICE_PARMS *parms, char *rx, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineProcessingObject *obj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE_PROCESSING, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d line:%d\n",parms->op[0], parms->op[1]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line processing object \n" );
      return ( ret );
   }

   cmsLog_debug( "RxGain = %d\n ", obj->receiveGain);
   snprintf( (char*)rx, length, "%d",obj->receiveGain / GAINUNIT );

   cmsObj_free( (void **) &obj );

   return ( ret );
}

#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1

/*****************************************************************
**  FUNCTION:       dalVoice_GetVoiceFxoPhyInterfaceList
**
**  PUROPOSE:
**
**  INPUT PARMS:    none
**
**  OUTPUT PARMS:   list  - list of physical endpoint ID
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVoiceFxoPhyInterfaceList(DAL_VOICE_PARMS *parms, char *list, unsigned int length )
{
   CmsRet          ret      = CMSRET_SUCCESS;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   VoicePstnObject           *obj = NULL;
   char tmp[20];

   if(list == NULL)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* Iterate through list of all pstn objects in the system */
   list[0] = '\0';
   while ( (ret = cmsObj_getNextFlags(MDMOID_VOICE_PSTN, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&obj)) == CMSRET_SUCCESS )
   {
      sprintf(tmp, "%u", obj->pstnInterfaceID);
      if(strlen( list ))
         strncat(list, ",", length);
      strncat(list, tmp, length);
      cmsObj_free( (void **)&obj);
   }

   return CMSRET_SUCCESS;

}

/***************************************************************************
* Function Name: dalVoice_mapFxoInterfaceIDToPstnInst
* Description  : This returns the Voice Pstn instance number corresponding
*                to a certain Fxo interface Id.
*
* Parameters   : (INPUT) Fxo Interface Id - Fxo Physical interface ID
*                (OUTPUT) inst - PSTN instance index
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_mapFxoInterfaceIDToPstnInst(int id, int *inst)
{
   return rutVoice_mapFxoInterfaceIDToInst(id, inst);
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlFxoPhyReferenceList
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   physEptId - Fxo Physical endpoint ID
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlFxoPhyReferenceList(DAL_VOICE_PARMS *parms, char *physEptId, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   unsigned int        lineInst   = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceLineObject     *lineObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vp:%d line:%d \n",parms->op[0], parms->op[1]);

   /* Do not execute code if the lineInst is invalid */
   if (lineInst < 1)
   {
      cmsLog_debug("invalid lineInst number = %d", lineInst);
      strncpy( physEptId, "", length);
      return CMSRET_SUCCESS;
   }

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&lineObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line instance object \n" );
      return ( ret );
   }

   /* Return empty string if nonFxs phy list doesnt contain FXO */
   if ( lineObj->X_BROADCOM_COM_NonFxsPhyReferenceList == NULL || strcmp(lineObj->X_BROADCOM_COM_AssociatedNonFxsPhyIntfType, MDMVS_FXO) )
   {
      cmsObj_free((void **) &lineObj);
      strncpy( physEptId, "", length);
   }
   else
   {
      /* Copy first token from the physical enpoint string, since we dont support multi-physept to acc mapping */
      cmsLog_debug( "FxoPhyReferenceList = %s\n ", lineObj->X_BROADCOM_COM_NonFxsPhyReferenceList);
      snprintf( physEptId, length, "%s", lineObj->X_BROADCOM_COM_NonFxsPhyReferenceList );
   }

   cmsObj_free((void **) &lineObj);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPstnDialPlan
**
**  PUROPOSE:
**
**  INPUT PARMS:    PSTN instance  - parms->op[0]
**
**  OUTPUT PARMS:   dialPlan - PSTN outgoing dial plan
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPstnDialPlan(DAL_VOICE_PARMS *parms, char *dialPlan, unsigned int length )
{
   int                 pstnInst   = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoicePstnObject     *pstnObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PSTN, pstnInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "PSTN instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&pstnObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve PSTN object instance %d\n", parms->op[0] );
      return ( ret );
   }

   /* Copy PSTN call dial plan value */
   if( pstnObj->outgoingCallDialPlan != NULL )
   {
      cmsLog_debug( "PSTN outgoing call dial plan = %s\n ", pstnObj->outgoingCallDialPlan);
      strncpy( (char *) dialPlan, pstnObj->outgoingCallDialPlan, length);
   }

   cmsObj_free( (void **) &pstnObj );

   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPstnRouteRule
**
**  PUROPOSE:
**
**  INPUT PARMS:    PSTN instance  - parms->op[0]
**
**  OUTPUT PARMS:   mode - PSTN call routing mode
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPstnRouteRule(DAL_VOICE_PARMS *parms, char *mode, unsigned int length )
{
   int                 pstnInst   = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoicePstnObject     *pstnObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PSTN, pstnInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "PSTN instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&pstnObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve PSTN object \n" );
      return ( ret );
   }

   /* Copy PSTN call routing mode value */
   if( pstnObj->incomingCallRoutingMode != NULL )
   {
      cmsLog_debug( "PSTN incoming call routing mode = %s\n ", pstnObj->incomingCallRoutingMode);
      strncpy( (char *) mode, pstnObj->incomingCallRoutingMode, length);
   }


   cmsObj_free( (void **) &pstnObj );
   return  ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPstnRouteData
**
**  PUROPOSE:
**
**  INPUT PARMS:    PSTN instance  - parms->op[0]
**
**  OUTPUT PARMS:   dest - PSTN call routing destination
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPstnRouteData(DAL_VOICE_PARMS *parms, char *dest, unsigned int length )
{
   int                 pstnInst   = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoicePstnObject     *pstnObj   = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PSTN, pstnInst, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "PSTN instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&pstnObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve PSTN object \n" );
      return ( ret );
   }

   /* Copy PSTN call routing destination value */
   if( pstnObj->incomingCallRoutingDest != NULL )
   {
      cmsLog_debug( "PSTN incoming call routing destination = %s\n ", pstnObj->incomingCallRoutingDest);
      strncpy( (char *) dest, pstnObj->incomingCallRoutingDest, length);
   }


   cmsObj_free( (void **) &pstnObj );
   return  ( ret );
}
#endif /* DMP_X_BROADCOM_COM_PSTNENDPOINT_1 */

/*****************************************************************
**  FUNCTION:       dalVoice_GetRtpDSCPMark
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   dscpMark - Value of RTP DSCP mark
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetRtpDSCPMark(DAL_VOICE_PARMS *parms, char *dscpMark, unsigned int length )
{
   unsigned int        vpInst   = parms->op[0];
   CmsRet             ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfRtpObject *voiceProfRtpObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_PROF_RTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceProfRtpObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve RTP object \n" );
      return ( ret );
   }

   /* Copy DSCP mark value */
   snprintf( (char*)dscpMark, length, "%u",voiceProfRtpObj->DSCPMark );
   cmsObj_free((void **) &voiceProfRtpObj);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSrtpEnabled
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   port - Value of RTP min port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSrtpEnabled(DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
   unsigned int            vpInst          = parms->op[0];
   CmsRet                  ret;
   InstanceIdStack         iidStack        = EMPTY_INSTANCE_ID_STACK;
   VoiceProfRtpSrtpObject *obj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]         = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP_SRTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SRTP object \n" );
      return ( ret );
   }

   /* Copy base RTP port value */
   cmsLog_debug( "SRTP Enable = %s\n ", ( obj->enable )?"True":"False");
   snprintf( (char*)getVal, length, "%u",obj->enable );
   cmsObj_free((void **) &obj);

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetRtpLocalPortMin
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   port - Value of RTP min port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetRtpLocalPortMin(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
   unsigned int        vpInst          = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack        = EMPTY_INSTANCE_ID_STACK;
   VoiceProfRtpObject *voiceProfRtpObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]         = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceProfRtpObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve RTP object \n" );
      return ( ret );
   }

   /* Copy base RTP port value */
   cmsLog_debug( "RTP port min = %d\n ", voiceProfRtpObj->localPortMin);
   snprintf( (char*)port, length, "%u",voiceProfRtpObj->localPortMin );
   cmsObj_free((void **) &voiceProfRtpObj);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRtpLocalPortMax
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   port - Value of RTP max port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetRtpLocalPortMax(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
   unsigned int        vpInst          = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack        = EMPTY_INSTANCE_ID_STACK;
   VoiceProfRtpObject *voiceProfRtpObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]         = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceProfRtpObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve RTP object \n" );
      return ( ret );
   }

   /* Copy top RTP port value */
   cmsLog_debug( "RTP port max = %d\n ", voiceProfRtpObj->localPortMax);
   snprintf( (char*)port, length, "%u",voiceProfRtpObj->localPortMax );
   cmsObj_free((void **) &voiceProfRtpObj);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetIpv6Enabled
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   enabled - Enabled flag
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetIpv6Enabled(DAL_VOICE_PARMS *parms, char *enabled, unsigned int length )
{
   CmsRet ret;
   char   family[TEMP_CHARBUF_SIZE];

   if ( (ret = dalVoice_GetIpFamily( parms, (char*)family, TEMP_CHARBUF_SIZE)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't get IPv6 ENABLED value\n" );
   }
   else
   {
      snprintf( (char*)enabled, length, "%u", !(cmsUtl_strcmp( family, MDMVS_IPV6 )) ? 1 : 0);
      cmsLog_debug( "feature get: value %s\n", enabled);
   }

   return ( ret );
}

#ifdef MGCPLOAD
/*****************************************************************
**  FUNCTION:       dalVoice_GetMgcpCallAgentIpAddress
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   call agent ip address
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetMgcpCallAgentIpAddress(DAL_VOICE_PARMS *parms, char *callAgentIpAddress, unsigned int length )
{
   unsigned int         vpInst      = parms->op[0];
   CmsRet               ret;
   InstanceIdStack      iidStack    = EMPTY_INSTANCE_ID_STACK;
   VoiceProfMgcpObject *mgcpObj     = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]      = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                        {MDMOID_VOICE_PROF_MGCP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                        {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n", parms->op[0]);

   /*  Get the MGCP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&mgcpObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve MGCP object \n" );
      return ( ret );
   }

   /* Copy call agent ip address */
   if( mgcpObj->callAgent1 != NULL )
   {
      cmsLog_debug( "call agent ip address = %s\n ", mgcpObj->callAgent1);
      strncpy( (char *) callAgentIpAddress, mgcpObj->callAgent1, length );
   }
   else
      cmsLog_error( "Can't retrieve call agent ip address \n" );

   cmsObj_free( (void **) &mgcpObj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetMgcpGatewayName
**
**  PUROPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   gateway name
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetMgcpGatewayName(DAL_VOICE_PARMS *parms, char *gatewayName, unsigned int length )
{
   unsigned int         vpInst      = parms->op[0];
   CmsRet               ret;
   InstanceIdStack      iidStack    = EMPTY_INSTANCE_ID_STACK;
   VoiceProfMgcpObject *mgcpObj     = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]      = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                        {MDMOID_VOICE_PROF_MGCP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                        {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n", parms->op[0]);

   /*  Get the MGCP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&mgcpObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve MGCP object \n" );
      return ( ret );
   }

   /* Copy gateway name */
   if( mgcpObj->user != NULL )
   {
      cmsLog_debug( "gateway name = %s\n ", mgcpObj->user);
      strncpy( (char *) gatewayName, mgcpObj->user, length);
   }
   else
      cmsLog_error( "Can't retrieve gateway name\n" );

   cmsObj_free( (void **) &mgcpObj );

   return ( ret );
}
#endif /* MGCPLOAD */

CmsRet dalVoice_GetNumFxoEndpt( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   int numFxoEndpt;
   rutVoice_getNumFxoEndpt( &numFxoEndpt );
   snprintf( value, length, "%d",numFxoEndpt );
   return CMSRET_SUCCESS;
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetMaxLinesPerVoiceProfile
**
**  PUROPOSE:       Returns max no. of lines that can be configured
**                  configured in a string
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - max no. of voice profiles that can be configured
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetMaxLinesPerVoiceProfile( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   /* Return value from RUT layer */
   int maxLine;

   rutVoice_getMaxLineInstances( &maxLine );

   snprintf(  (char*)value, length, "%d",maxLine );
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetTotalNumLines
**
**  PUROPOSE:       Returns no. of lines In the System
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - no. of lines that is configured
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetTotalNumLines( int * numTotLines )
{
   /* Return value from RUT layer */
   int numLines;

   /* Call RUT function with special vp of NULL */
   rutVoice_getNumLines( (int)NULL, &numLines );

   cmsLog_debug("numLines = %d\n", numLines);

   *numTotLines = numLines;

   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumPhysEndpts
**
**  PUROPOSE:       Returns no. of physical voice endpoints In the System
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - no. of lines that is configured
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumPhysEndpts( int * numPhysEndpts )
{
   /* Return value from RUT layer */
   int numEndpts;

   /* Call RUT function with special vp of NULL */
   rutVoice_getMaxPhysEndpt( &numEndpts );

   cmsLog_debug("numEndpts = %d\n", numEndpts);

   *numPhysEndpts = numEndpts;

   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumLinesPerVoiceProfile
**
**  PUROPOSE:       Returns no. of lines that are
**                  configured in a string
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - no. of lines that is configured
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumLinesPerVoiceProfile( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   /* Return value from RUT layer */
   int numLines;
   int vp = parms->op[0];

   cmsLog_debug("vp = %d\n", vp);

   if (vp <= 0)
   {
      cmsLog_error("invalid vp instance number: %d\n", vp);
      return CMSRET_INVALID_ARGUMENTS;
   }

   rutVoice_getNumLines( vp, &numLines );

   cmsLog_debug("numLines = %d\n", numLines);

   snprintf(  (char*)value, length, "%d", numLines );
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetMaxVoiceProfiles
**
**  PUROPOSE:       Returns maximum no. of service providers that can
**                  be configured in a string
**                  ( i.e corresponds to total no. of Voice profiles)
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - max no. of service providers that can be configured
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetMaxVoiceProfiles( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   /* Return value from RUT layer */
   int maxVp;
   CmsRet ret;

   ret = rutVoice_getMaxVoiceProfiles(&maxVp);

   snprintf(  (char*)value, length, "%d",maxVp );
   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumVoiceProfiles
**
**  PUROPOSE:       Returns no. of voice profile that are
**                  configured in a string
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - no. of voice profiles that is configured
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumVoiceProfiles( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   /* Return value from RUT layer */
   int numVp;
   CmsRet ret;

   /* Retrieve the number of service provider is the same is retrieve the number of voice
    * profiles
    */
   ret = rutVoice_getNumSrvProv(&numVp);

   snprintf(  (char*)value, length, "%d",numVp );
   return ret;
}


CmsRet dalVoice_GetSuppCodecsString( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   return(rutVoice_getSupportedCodecs(value, length));
}

CmsRet dalVoice_GetMaxSuppCodecs( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   int numCodecs;
   rutVoice_getNumSupportedCodecs(&numCodecs);
   numCodecs -= 2; /* Take out T38 and NTE */
   snprintf(  (char*)value, length, "%d",numCodecs );
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumSrvProv
**
**  PUROPOSE:       Returns total no. of service providers configured
**                  ( i.e corresponds to total no. of Voice profiles)
**
**  INPUT PARMS:    None;
**
**  OUTPUT PARMS:   Number of service providers ( voice profiles ) configured
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumSrvProv( int * numSp )
{
   return rutVoice_getNumSrvProv( numSp );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumAccPerSrvProv
**
**  PUROPOSE:       returns total accounts per specific serviceprovider
**                  ( i.e. corresponds to number of lines per specific voice profile )
**
**  INPUT PARMS:    srvProvNum - Index of service provider;
**
**  OUTPUT PARMS:   Number of accounts per this service provider (num vplines per vp)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumAccPerSrvProv( int srvProvNum, int * numAcc )
{
   int numSrvProv;
   rutVoice_getNumSrvProv( &numSrvProv );

   /* check to see if service provider number is vaild */
   if ( srvProvNum < 0 || srvProvNum >= numSrvProv )
   {
      cmsLog_debug("invalid srvProvNum \n");
      return CMSRET_INVALID_ARGUMENTS;
   }

   rutVoice_getNumAccPerSrvProv( srvProvNum, numAcc );

   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumPhysFxsEndpt
**
**  PUROPOSE:       returns total number of physical fxs endpoints in system
**
**  INPUT PARMS:    none;
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumPhysFxsEndpt( int * numPhysFxs )
{
   rutVoice_getNumFxsEndpt( numPhysFxs );
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumPhysFxsEndptStr
**
**  PUROPOSE:       returns total number of physical fxs endpoints in system
**
**  INPUT PARMS:    none;
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumPhysFxsEndptStr( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   int numFxsEndpt = 0;
   rutVoice_getNumFxsEndpt( &numFxsEndpt );
   snprintf( value, length, "%d", numFxsEndpt );
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumPhysFxoEndpt
**
**  PUROPOSE:       returns total number of physical fxo endpoints in system
**
**  INPUT PARMS:    none;
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumPhysFxoEndpt( int * numPhysFxo )
{
   rutVoice_getNumFxoEndpt( numPhysFxo );
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumPhysFxoEndptStr
**
**  PUROPOSE:       returns total number of physical fxo endpoints in system
**
**  INPUT PARMS:    none;
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumPhysFxoEndptStr( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   int numFxoEndpt = 0;
   rutVoice_getNumFxoEndpt( &numFxoEndpt );
   snprintf( value, length, "%d",numFxoEndpt );
   return CMSRET_SUCCESS;
}



/* GetNtrEnable is used by the GUI even if NTR support is off */
/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrEnable
**
**  PUROPOSE:       Get NTR Enable flag
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   enabled - NTR Enabled
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrEnable(DAL_VOICE_PARMS *parms, char *enabled, unsigned int length )
{
#ifndef DMP_X_BROADCOM_COM_NTR_1
   /* This is used for non-NTR builds to not display the NTR web page. */
   snprintf( (char*)enabled, length, "2");
   return CMSRET_SUCCESS;
#endif

   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsLog_debug( "NtrEnable = %s\n ", (obj->enable)? "enable":"disable");

   snprintf( (char*)enabled, length, "%u", obj->enable );

   cmsObj_free( (void **) &obj );

   return ( ret );
}

#if defined( DMP_X_BROADCOM_COM_NTR_1 )
/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrAutoModeEnable
**
**  PUROPOSE:       Get NTR Auto Mode on (1) or off (0)
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   enabled - NTR Automatic Offset Calculate Mode Enabled
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrAutoModeEnable(DAL_VOICE_PARMS *parms, char *enabled, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsLog_debug( "NtrAutoModeEnable = %s\n ", (obj->autoModeEnable)? "enable":"disable");

   snprintf( (char*)enabled, length, "%u", obj->autoModeEnable );

   cmsObj_free( (void **) &obj );

   return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrManualOffset
**
**  PUROPOSE:       Get Offset for NTR Manual Mode
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   manualOffset - Manually specified feedback offset in Hz
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrManualOffset(DAL_VOICE_PARMS *parms, char *manualOffset, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->manualOffset != NULL )
   {
      cmsLog_debug( "NtrManualOffset = %s\n ", obj->manualOffset);
      strncpy( (char*)manualOffset, obj->manualOffset, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrManualPpm
**
**  PUROPOSE:       Get Offset in PPM for NTR Manual Mode
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   manualPpm - Manually specified feedback offset in PPM
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrManualPpm(DAL_VOICE_PARMS *parms, char *manualPpm, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsLog_debug( "NtrManualPpm = %s\n ", obj->manualPpm);

   snprintf( (char*)manualPpm, length, "%s", obj->manualPpm );

   cmsObj_free( (void **) &obj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrManualSteps
**
**  PUROPOSE:       Get feedback offset in PLL steps for NTR manual mode
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   manualSteps - Manually specified feedback offset in PLL steps
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrManualSteps(DAL_VOICE_PARMS *parms, char *manualSteps, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsLog_debug( "NtrManualSteps = %d\n ", obj->manualSteps);

   snprintf( (char*)manualSteps, length, "%i", obj->manualSteps );

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrDebugEnable
**
**  PUROPOSE:       Get NTR Debug flag
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   enabled - NTR Debug Enabled
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrDebugEnable(DAL_VOICE_PARMS *parms, char *enabled, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   cmsLog_debug( "NtrDebugEnable = %s\n ", (obj->debugEnable)? "enable":"disable");

   snprintf( (char*)enabled, length, "%u", obj->debugEnable );

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrPcmMipsTallyCurrent
**
**  PUROPOSE:       Get current PCM-MIPS tally for NTR task
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   pcmMipsTallyCurrent - Tally
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrPcmMipsTallyCurrent(DAL_VOICE_PARMS *parms, char *pcmMipsTallyCurrent, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->pcmMipsTallyCurrent != NULL )
   {
      cmsLog_debug( "NtrPcmMipsTallyCurrent = %s\n ", obj->pcmMipsTallyCurrent);
      strncpy( (char*)pcmMipsTallyCurrent, obj->pcmMipsTallyCurrent, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrPcmMipsTallyPrevious
**
**  PUROPOSE:       Get previous PCM-MIPS tally for NTR task
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   pcmMipsTallyPrevious - Tally
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrPcmMipsTallyPrevious(DAL_VOICE_PARMS *parms, char *pcmMipsTallyPrevious, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->pcmMipsTallyPrevious != NULL )
   {
      cmsLog_debug( "NtrPcmMipsTallyPrevious = %s\n ", obj->pcmMipsTallyPrevious);
      strncpy( (char*)pcmMipsTallyPrevious, obj->pcmMipsTallyPrevious, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrPcmNtrTallyCurrent
**
**  PUROPOSE:       Get current PCM-NTR tally for NTR task
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   pcmNtrTallyCurrent - Tally
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrPcmNtrTallyCurrent(DAL_VOICE_PARMS *parms, char *pcmNtrTallyCurrent, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->pcmNtrTallyCurrent != NULL )
   {
      cmsLog_debug( "NtrPcmNtrTallyCurrent = %s\n ", obj->pcmNtrTallyCurrent);
      strncpy( (char*)pcmNtrTallyCurrent, obj->pcmNtrTallyCurrent, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrPcmNtrTallyPrevious
**
**  PUROPOSE:       Get previous PCM-NTR tally for NTR task
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   pcmNtrTallyPrevious - Tally
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrPcmNtrTallyPrevious(DAL_VOICE_PARMS *parms, char *pcmNtrTallyPrevious, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->pcmNtrTallyPrevious != NULL )
   {
      cmsLog_debug( "NtrPcmNtrTallyPrevious = %s\n ", obj->pcmNtrTallyPrevious);
      strncpy( (char*)pcmNtrTallyPrevious, obj->pcmNtrTallyPrevious, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrDslMipsTallyCurrent
**
**  PUROPOSE:       Get current DSL-MIPS tally for NTR task
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   dslMipsTallyCurrent - Tally
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrDslMipsTallyCurrent(DAL_VOICE_PARMS *parms, char *dslMipsTallyCurrent, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->dslMipsTallyCurrent != NULL )
   {
      cmsLog_debug( "NtrDslMipsTallyCurrent = %s\n ", obj->dslMipsTallyCurrent);
      strncpy( (char*)dslMipsTallyCurrent, obj->dslMipsTallyCurrent, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrDslMipsTallyPrevious
**
**  PUROPOSE:       Get previous DSL-MIPS tally for NTR task
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   dslMipsTallyPrevious - Tally
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrDslMipsTallyPrevious(DAL_VOICE_PARMS *parms, char *dslMipsTallyPrevious, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->dslMipsTallyPrevious != NULL )
   {
      cmsLog_debug( "NtrDslMipsTallyPrevious = %s\n ", obj->dslMipsTallyPrevious);
      strncpy( (char*)dslMipsTallyPrevious, obj->dslMipsTallyPrevious, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrDslNtrTallyCurrent
**
**  PUROPOSE:       Get current DSL-NTR tally for NTR task
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   dslNtrTallyCurrent - Tally
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrDslNtrTallyCurrent(DAL_VOICE_PARMS *parms, char *dslNtrTallyCurrent, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->dslNtrTallyCurrent != NULL )
   {
      cmsLog_debug( "NtrDslNtrTallyCurrent = %s\n ", obj->dslNtrTallyCurrent);
      strncpy( (char*)dslNtrTallyCurrent, obj->dslNtrTallyCurrent, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrDslNtrTallyPrevious
**
**  PUROPOSE:       Get previous DSL-NTR tally for NTR task
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   dslNtrTallyPrevious - Tally
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrDslNtrTallyPrevious(DAL_VOICE_PARMS *parms, char *dslNtrTallyPrevious, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->dslNtrTallyPrevious != NULL )
   {
      cmsLog_debug( "NtrDslNtrTallyPrevious = %s\n ", obj->dslNtrTallyPrevious);
      strncpy( (char*)dslNtrTallyPrevious, obj->dslNtrTallyPrevious, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrSampleRate
**
**  PUROPOSE:       Get previous SampleRate for NTR task
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   sampleRate
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrSampleRate(DAL_VOICE_PARMS *parms, char *sampleRate, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->sampleRate != NULL )
   {
      cmsLog_debug( "sampleRate = %s\n ", obj->sampleRate);
      strncpy( (char*)sampleRate, obj->sampleRate, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrPllBandwidth
**
**  PUROPOSE:       Get previous PllBandwidth for NTR task
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   pllBandwidth
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrPllBandwidth(DAL_VOICE_PARMS *parms, char *pllBandwidth, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->pllBandwidth != NULL )
   {
      cmsLog_debug( "pllBandwidth = %s\n ", obj->pllBandwidth);
      strncpy( (char*)pllBandwidth, obj->pllBandwidth, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrDampingFactor
**
**  PUROPOSE:       Get previous DampingFactor for NTR task
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   dampingFactor
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrDampingFactor(DAL_VOICE_PARMS *parms, char *dampingFactor, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrObject     *obj      = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   /* Copy value */
   if( obj->dampingFactor != NULL )
   {
      cmsLog_debug( "dampingFactor = %s\n ", obj->dampingFactor);
      strncpy( (char*)dampingFactor, obj->dampingFactor, length );
   }

   cmsObj_free( (void **) &obj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrAutoOffset
**
**  PUROPOSE:       Get the most recent feedback offsets applied in Hz in NTR automatic mode separated by ','
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   autoOffset - Most recent automatically-calculated feedback offset in Hz
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrAutoOffset(DAL_VOICE_PARMS *parms, char *autoOffset, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject     *obj      = NULL;


   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }


   /* Copy Value */
   if( obj->autoOffset != NULL )
   {
      snprintf( autoOffset, length, "%s", obj->autoOffset );
   }
   cmsObj_free((void **) &obj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrAutoPpm
**
**  PUROPOSE:       Get the most recent feedback offsets applied in PPM in NTR automatic mode separated by ','
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   autoPpm - Most recent automatically-calculated feedback offset in PPM
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrAutoPpm(DAL_VOICE_PARMS *parms, char *autoPpm, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject     *obj      = NULL;


   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   if (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve NTR object \n" );
      return ( ret );
   }

   // if ( ret = cmsObj_getNextInSubTree( MDMOID_VOICE_NTR_HISTORY, &info[0].iidStack, &iidStack, (void **)&obj) != CMSRET_SUCCESS )
   // {
      // cmsLog_error( "Can't retrieve NTR History object \n" );
      // return ( ret );
   // }

   /* Copy Value */
   if( obj->autoPpm != NULL )
   {
      snprintf( autoPpm, length, "%s", obj->autoPpm );
   }
   cmsObj_free((void **) &obj );

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrAutoStepsHistory
**
**  PUROPOSE:       Get the history of feedback offsets applied in PLL steps in NTR automatic mode separated by ','
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   autoSteps - History of feedback offset applied in PLL steps
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrAutoStepsHistory(DAL_VOICE_PARMS *parms, char *autoSteps, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject     *obj      = NULL;
   char tmp[20];


   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   autoSteps[0] = '\0';
   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj); ret != CMSRET_NO_MORE_INSTANCES;
        ret = cmsObj_getNextInSubTree( MDMOID_VOICE_NTR_HISTORY, &info[0].iidStack, &iidStack,
                                       (void **)&obj) )
   {
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve NTR object \n" );
         return ( ret );
      }
      else
      {
         sprintf( tmp, "%i", obj->autoSteps );
         if(strlen(autoSteps))
            strncat(autoSteps, ",", length);
         strncat(autoSteps, tmp, length);
         cmsObj_free((void **) &obj );
      }
   }

   return (ret != CMSRET_NO_MORE_INSTANCES) ? ret : CMSRET_SUCCESS;
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrAutoOffsetHistory
**
**  PUROPOSE:       Get the history of feedback offsets applied in Hz in NTR automatic mode separated by ','
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   autoOffset - History of automatically-calculated feedback offset in Hz
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrAutoOffsetHistory(DAL_VOICE_PARMS *parms, char *autoOffset, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject     *obj      = NULL;
   char tmp[40];


   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   autoOffset[0] = '\0';
   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj); ret != CMSRET_NO_MORE_INSTANCES;
        ret = cmsObj_getNextInSubTree( MDMOID_VOICE_NTR_HISTORY, &info[0].iidStack, &iidStack,
                                       (void **)&obj) )
   {
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve NTR object \n" );
         return ( ret );
      }
      else
      {
         if( obj->autoOffset != NULL )
         {
            sprintf(tmp, "%s", obj->autoOffset);
            if(strlen(autoOffset))
               strncat(autoOffset, ",", length);
            strncat(autoOffset, tmp, length);
         }
         cmsObj_free((void **) &obj );
      }
   }


   return (ret != CMSRET_NO_MORE_INSTANCES) ? ret : CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrAutoPpmHistory
**
**  PUROPOSE:       Get the history of feedback offsets applied in PPM in NTR automatic mode separated by ','
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   autoPpm - History of automatically-calculated feedback offset in PPM
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrAutoPpmHistory(DAL_VOICE_PARMS *parms, char *autoPpm, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject     *obj      = NULL;
   char tmp[40];


   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj); ret != CMSRET_NO_MORE_INSTANCES;
        ret = cmsObj_getNextInSubTree( MDMOID_VOICE_NTR_HISTORY, &info[0].iidStack, &iidStack,
                                       (void **)&obj) )
   {
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve NTR object \n" );
         return ( ret );
      }
      else
      {
         if( obj->autoPpm != NULL )
         {
            sprintf(tmp, "%s", obj->autoPpm );
            if(strlen(autoPpm))
               strncat(autoPpm, ",", length);
            strncat(autoPpm, tmp, length);
         }
         cmsObj_free((void **) &obj );
      }
   }

   return (ret != CMSRET_NO_MORE_INSTANCES) ? ret : CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNtrAutoPhaseErrorHistory
**
**  PUROPOSE:       Get the history of automatically calculated phase error separated by ','
**
**  INPUT PARMS:    NTR instance  - parms->op[0]
**
**  OUTPUT PARMS:   autoPpm - History of automatically-calculated phase error
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNtrAutoPhaseErrorHistory(DAL_VOICE_PARMS *parms, char *autoPhaseError, unsigned int length )
{
   int                 ntrInst  = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceNtrHistoryObject     *obj      = NULL;
   char tmp[40];


   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_NTR, ntrInst, EMPTY_INSTANCE_ID_STACK},
                                       {MDMOID_VOICE_NTR_HISTORY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "NTR instance:%d \n",parms->op[0]);

   /*  Get the Voice Profile object */
   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj); ret != CMSRET_NO_MORE_INSTANCES;
        ret = cmsObj_getNextInSubTree( MDMOID_VOICE_NTR_HISTORY, &info[0].iidStack, &iidStack,
                                       (void **)&obj) )
   {
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve NTR object \n" );
         return ( ret );
      }
      else
      {
         sprintf(tmp, "%s", obj->autoPhaseError );
         if(strlen(autoPhaseError))
            strncat(autoPhaseError, ",", length);
         strncat(autoPhaseError, tmp, length);
         cmsObj_free((void **) &obj );
      }
   }

   return (ret != CMSRET_NO_MORE_INSTANCES) ? ret : CMSRET_SUCCESS;
}
#endif /* DMP_X_BROADCOM_COM_NTR_1 */

/*****************************************************************
**  FUNCTION:       dalVoice_GetEuroFlashEnable
**
**  PUROPOSE:       Retrieve European flash enable
**
**  INPUT PARMS:    vpInst - parms->op[0]
**                  length - buffer length
**
**  OUTPUT PARMS:
**                  pEuroFlashEn - European flash enable
**                                 MDMVS_ON for enabled, MDMVS_OFF for disabled
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetEuroFlashEnable(DAL_VOICE_PARMS *parms, char *pEuroFlashEn, unsigned int length )
{
   unsigned int        vpInst     = parms->op[0];
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject  *sipObj    = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "vpInst:%d \n",parms->op[0]);

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* Copy SIP tag matching value */
   cmsLog_debug( "SIP failover = %d\n ", sipObj->X_BROADCOM_COM_EuroFlashEnable);
   if (sipObj->X_BROADCOM_COM_EuroFlashEnable)
   {
      snprintf( pEuroFlashEn, length, "%s", MDMVS_ON );
   }
   else
   {
      snprintf( pEuroFlashEn, length, "%s", MDMVS_OFF );
   }
   cmsObj_free( (void **) &sipObj );

   return  ( ret );
}

/*<END>==================================== DAL Get Functions ======================================<END>*/

/*<START>===================================== Set Helper Functions ======================================<START>*/

/***************************************************************************
* Function Name: setVlCFCallFwdNum
* Description  : Directory number to which all incoming calls to
*                this line should be forwarded
*                VoiceProfile.{i}.Line{i}.CallingFeatures.callForwardUnconditionalNumber = new value
*                VoiceProfile.{i}.Line{i}.CallingFeatures.callForwardOnBusyNumber = new value
*                VoiceProfile.{i}.Line{i}.CallingFeatures.callForwardOnNoAnswerNumber = new value
*
* Parameters   : vpInst (IN) Voice Profile Instance Number
*              : lineInst (IN) Line Instance Number
*              : setVal (IN) value to assign to MDM
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setVlCFCallFwdNum ( unsigned int vpInst, unsigned int lineInst, char *setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

      /*  Get the Voice Profile object */
      if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't retrieve calling features object \n" );
         return ( ret );
      }

      /* set the new value in local copy, after freeing old memory */
      cmsMem_free( voiceLineCallingFeaturesObj->callForwardUnconditionalNumber );
      voiceLineCallingFeaturesObj->callForwardUnconditionalNumber = cmsMem_strdup ( setVal );

      cmsMem_free( voiceLineCallingFeaturesObj->callForwardOnBusyNumber );
      voiceLineCallingFeaturesObj->callForwardOnBusyNumber = cmsMem_strdup ( setVal );

      cmsMem_free( voiceLineCallingFeaturesObj->callForwardOnNoAnswerNumber );
      voiceLineCallingFeaturesObj->callForwardOnNoAnswerNumber = cmsMem_strdup ( setVal );

      cmsLog_debug( "CallForward Num == %s for [vpInst, lineInst] = [%u, %u]\n", setVal, vpInst, lineInst );
      /* copy new value from local copy to MDM */
      if ( ( ret = cmsObj_set( voiceLineCallingFeaturesObj, &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set Call forward number ret = %d\n", ret);
      }

      cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ret;
}

/***************************************************************************
* Function Name: setVlCLSilenceSuppression
* Description  : Indicates support for silence suppression for this codec.
*                VoiceProfile.{i}.Line{i}.Codec.List.{i}.SilenceSuppression = new value
*
* Parameters   : vpInst (IN) Voice Profile Instance Number
*              : lineInst (IN) Line Instance Number
*              : setVal (IN) value to assign to MDM
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setVlCLSilenceSuppression ( unsigned int vpInst, unsigned int lineInst,
                                          unsigned int setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack;
   VoiceLineCodecListObject *voiceLineCodecListObj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_CODEC_LIST, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );

   /*  Get the Voice Line Codec object */
   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCodecListObj); ret != CMSRET_NO_MORE_INSTANCES;
        ret = cmsObj_getNextInSubTree( MDMOID_VOICE_LINE_CODEC_LIST, &info[1].iidStack, &iidStack,
                                       (void **)&voiceLineCodecListObj) )
   {
      /* set the new value in local copy */
      voiceLineCodecListObj->silenceSuppression = setVal;

      cmsLog_debug( "Silence Suppression == %i for [vpInst, lineInst] = [%u, %u]\n", setVal, vpInst, lineInst );
      /* copy new value from local copy to MDM */
      if ( ( ret = cmsObj_set( voiceLineCodecListObj,  &iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error( "Can't set Silence suppression ret = %d\n", ret);
      }

      cmsObj_free((void **) &voiceLineCodecListObj);
   }

   return ret;
}

/***************************************************************************
* Function Name: setVlCLPacketizationPeriod
* Description  : Indicates support for silence suppression for this codec.
*                VoiceProfile.{i}.Line{i}.Codec.List.{i}.PacketizationPeriod = new value
*
* Parameters   : vpInst (IN) Voice Profile Instance Number
*              : lineInst (IN) Line Instance Number
*              : setVal (IN) value to assign to MDM
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setVlCLPacketizationPeriod ( unsigned int vpInst, unsigned int lineInst
   , const char * setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack;
   VoiceLineCodecListObject *voiceLineCodecListObj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_CODEC_LIST, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   if ( !atoi(setVal) && strcmp(setVal,"0") )
   {
      cmsLog_error( "pTime must be an integer value\n" );
      ret = CMSRET_INVALID_PARAM_TYPE;
   }
   else
   {
      /* initialize iid stack */
      INIT_INSTANCE_ID_STACK( &iidStack );

      /*  Get the Voice Line Codec object */
      for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCodecListObj); ret != CMSRET_NO_MORE_INSTANCES;
           ret = cmsObj_getNextInSubTree( MDMOID_VOICE_LINE_CODEC_LIST, &info[1].iidStack, &iidStack,
                                          (void **)&voiceLineCodecListObj) )
      {
         /* set the new value in local copy, after freeing old memory */
         cmsMem_free ( voiceLineCodecListObj->packetizationPeriod );
         voiceLineCodecListObj->packetizationPeriod = cmsMem_strdup ( setVal );

         cmsLog_debug( "Packetization Period == %s for [vpInst, lineInst] = [%u, %u]\n", setVal, vpInst, lineInst );
         /* copy new value from local copy to MDM */
         if ( ( ret = cmsObj_set( voiceLineCodecListObj,  &iidStack)) != CMSRET_SUCCESS )
         {
            cmsLog_error( "Can't set _Packetization period ret = %d\n", ret);
         }

         cmsObj_free((void **) &voiceLineCodecListObj);
      }
   }
   return ret;
}

/***************************************************************************
* Function Name: setSrtpOption
* Description  : SRTP Protocol Usage Option
*                VoiceProfile.{i}.RTP.SRTP.X_BROADCOM_COM_SrtpUsageOption
*
* Parameters   : vpInst (IN) Voice Profile Instance Number
*              : setVal (IN) value to assign to MDM
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setSrtpOption ( unsigned int vpInst, char * setVal )
{
   CmsRet                  ret;
   InstanceIdStack         iidStack        = EMPTY_INSTANCE_ID_STACK;
   VoiceProfRtpSrtpObject *obj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]         = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP_SRTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   REPLACE_STRING_IF_NOT_EQUAL(obj->X_BROADCOM_COM_SrtpUsageOption, setVal);

   cmsLog_debug( "SRTP Option == %s for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "setSrtpOption: Can't set SRTP Option ret = %d\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: setRedOption
* Description  : Rfc2198 Option
*                VoiceProfile.{i}.RTP.Redundancy
*
* Parameters   : vpInst (IN) Voice Profile Instance Number
*              : setVal (IN) value to assign to MDM
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setRedOption ( unsigned int vpInst, char * setVal )
{
   CmsRet                  ret;
   InstanceIdStack         iidStack        = EMPTY_INSTANCE_ID_STACK;
   VoiceProfRtpRedundancyObject *obj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]         = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_PROF_RTP_REDUNDANCY, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                           {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );


   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   if (!strcmp(setVal, MDMVS_DISABLED))
   {
       obj->enable = 0;
       obj->payloadType = 0;
       obj->voiceRedundancy = -1;
   }
   else if (setVal[0] != '\0' && setVal[0] < '4' && setVal[0] > '0')
   {
       obj->enable = 1;
       obj->payloadType = 102;
       obj->voiceRedundancy = setVal[0] - '0';
   }

   cmsLog_debug( "Red Option == %s for vpInst [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "setSrtpOption: Can't set RED Option ret = %d\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

/***************************************************************************
* Function Name: setVlCLCodecList
* Description  : Comma-separate list of codecs (no whitespaces or other delimiters).
*                First in list is highest priority (priority == 1).
*                Last in list is lowest priority (priority == max(all_codec_priorities)).
*                VoiceProfile.{i}.Line{i}.Codec.List.{i}. = new value
*
* Parameters   : vpInst (IN) Voice Profile Instance Number
*              : lineInst (IN) Line Instance Number
*              : setVal (IN) value to assign to MDM
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setVlCLCodecList ( unsigned int vpInst, unsigned int lineInst,
                               const char * setVal )
{
   InstanceIdStack    iidStack = EMPTY_INSTANCE_ID_STACK;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* unless getVpLineSipObjRet fails (changes from CMSRET_SUCCESS), then return getVpLineObjRet */
   CmsRet getVpLineObjRet;
   CmsRet getVpLineCodecListObjRet = CMSRET_SUCCESS;

   /* Pointers to objects we instantiate in order to be able to set fields */
   VoiceLineObject *voiceLineObj = NULL;
   VoiceLineCodecListObject *voiceLineCodecListObj = NULL;

   char * tokResult;                 /* Points to the tokenized result */
   char tokString[LIST_CHAR_BUFFER]; /* copy of entire command line of codec list */
   char * cliCodecList[MAX_NUM_CODECS]; /* tokenized array of each command line codec token (supports max 64 codecs) */
   char delim[2] = ",";              /* 'setVal' list delimiter */

   short b_match = 0;                /* records whether at least one voice profile was successfully processed */
   int numSetCodec = 0;              /* count the total number of codecs being set */

   int numSetCodecNotSupport;        /* decrement once for each codec being set from 'setVal' that */
                                     /* is defined in Codec.List.{i}; if non-zero when done, then */
                                     /* there must be codecs being set from 'setVal' which are not supported */

   int indexInCliCodecList;          /* index of a token which matches */
   int i;

   /* parse the command line list of codecs being set into 'codecList[]' as individual tokens */
   /* initialize string to be tokenized; strncpy mutates so make copy */
   strncpy( tokString, setVal, LIST_CHAR_BUFFER );

   /* parse first token */
   tokResult = strtok( tokString, delim );
   while ( tokResult != NULL )
   {
      /* allocate memory for this token, including a null-terminator */
      cliCodecList[numSetCodec] = ( char * ) malloc( sizeof( char ) * ( strlen( tokResult ) +1 ));

      /* retain copy of this token */
      strcpy( cliCodecList[numSetCodec], tokResult );

      /* increment the count of total number of tokens */
      numSetCodec++;

      /* Get next token */
      tokResult = strtok( NULL, delim );
   }

   /* Set all other codec list entries to NULL */
   for ( i = numSetCodec; i < MAX_NUM_CODECS; i++ )
   {
      cliCodecList[ i ] = NULL;
   }

   /* Dump tokenized codeclist */

   i=0;
   UBOOL8 found = 0;

   for ( i = 0 ; i < numSetCodec; i++ )
   {
      /* Check if codec is valid */
      validateCodec( cliCodecList[i], &found);

      /* If not found, may require X_BROADCOM_COM_ prefix. Check again */
      if ( !found )
      {
         /* used to store edited codec and original so memory can be reallocated */
         char *tempBuf = ( char * ) malloc( sizeof( char ) * (strlen( cliCodecList[i] ) +
                                                              strlen( "X_BROADCOM_COM_" ) + 2) );
         char *tempCodec = ( char * ) malloc( sizeof( char ) * (strlen( cliCodecList[i] ) + 1) );
         char *newMem;

         /* Store original and new codec name */
         sprintf( tempCodec, "%s", cliCodecList[i] );
         sprintf( tempBuf, "X_BROADCOM_COM_%s", cliCodecList[i] );

         /* Reallocate memory for larger string */
         newMem = realloc( cliCodecList[i], sizeof( char ) * ( strlen( cliCodecList[i] ) + 2 +
                                                                        strlen( "X_BROADCOM_COM_" ) ));
         cliCodecList[i] = newMem;

         /* Copy new codec name with prefix into list */
         strcpy( cliCodecList[i], tempBuf );
         /* Check again */
         validateCodec( cliCodecList[i], &found);

         if ( !found )
         {
            cmsLog_error( "Invalid codec: %s\n", tempCodec);
            /* free up dynamic memory used for storing the tokens from the command line */
            for ( i = 0; i < numSetCodec; i++ )
            {
               free( cliCodecList[ i ] );
            }

            /* display list of supported codecs */
            dumpSupportedCodecList();

            return CMSRET_INVALID_ARGUMENTS;
         }
         else
         {
            cmsLog_debug( "cliCodecList[%i]==%s\n", i,cliCodecList[ i ]);
         }

         free(tempBuf);
         free(tempCodec);
      }
      else
      {
         cmsLog_debug( "cliCodecList[%i]==%s\n", i,cliCodecList[ i ]);
      }
   }

   /* Initialize count of number of unsupp codecs to be equal to total codecs to be set */
   numSetCodecNotSupport = numSetCodec;

   /* Get the Voice Line Object to update */
   if ( ( getVpLineObjRet = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineObj )) == CMSRET_SUCCESS )
   {
      /* re-initialize iid stack */
      INIT_INSTANCE_ID_STACK( &iidStack );

      /*  Get the Voice Line Codec List object */
      while (( getVpLineCodecListObjRet = cmsObj_getNextInSubTree( MDMOID_VOICE_LINE_CODEC_LIST
         , &info[1].iidStack, &iidStack, (void **)&voiceLineCodecListObj)) == CMSRET_SUCCESS )
      {
         b_match = 1; /* record that at least one Voice Profile was processed */

         /* is the codec of this voiceLineCodecListObj in the list of codecs being set? */
         if ( ( indexInCliCodecList = getIndexInCliCodecList( cliCodecList, voiceLineCodecListObj->codec, numSetCodec )) >= 0)
         {
            /* set 'priority' to equal the index of the codec as it appears in 'setVal' */
            /* add 1 because priority starts from 1, whereas getIndexInCliCodecList starts from 0 */
            voiceLineCodecListObj->priority = indexInCliCodecList + 1;

            numSetCodecNotSupport--;  /* this codec is supported, so decrement unsupported codec counter */
         }
         else  /* this supported codec was not explicitly defined in 'setVal'; we assign an implicit value */
         {
            /* bump this implicit codec to lower priority to make way for the explicit (higher priority) codecs */
            voiceLineCodecListObj->priority += numSetCodec;
         }

         cmsLog_debug( "Codec List Priority == %i for [vpInst, lineInst, codec] = [%u, %u, %s]\n" \
            "", voiceLineCodecListObj->priority, vpInst, lineInst, voiceLineCodecListObj->codec );

         /* copy new value from local copy to MDM */
         if ( ( getVpLineCodecListObjRet = cmsObj_set( voiceLineCodecListObj,  &iidStack)) == CMSRET_SUCCESS )
         {
            cmsLog_debug( "Codec List Priority == %i for [vpInst, lineInst, codec] = [%u, %u, %s]\n" \
               "", voiceLineCodecListObj->priority, vpInst, lineInst, voiceLineCodecListObj->codec );
         }
         else
         {
            cmsLog_error( "getVpLineCodecListObjRet=cmsObj_set()=%i\n"
               , getVpLineCodecListObjRet);
         }

         cmsObj_free((void **) &voiceLineCodecListObj);
      }

      /* if numSetCodecNotSupport is less than zero, then this indicates a weird condition where    *
       * a codec exists in the system (i.e. via XDRV defines) but does not exist in mdm             */
      if ( numSetCodecNotSupport < 0 )
      {
         cmsLog_error( "numSetCodecNotSupport=%i != 0\n", numSetCodecNotSupport );
      }

      cmsObj_free((void **) &voiceLineObj);
   }
   else  /* Get Voice Line was not successful */
   {
      cmsLog_error( "getVpLineObjRet=getVpLineObj()=%i\n", getVpLineObjRet);
   }

   /* free up dynamic memory used for storing the tokens from the command line */
   for ( i = 0; i < numSetCodec; i++ )
   {
      free( cliCodecList[ i ] );
   }

   /* if no Codec Lines were updated, or the very last Codec Line was not reached */
   if ( !b_match || getVpLineCodecListObjRet != CMSRET_NO_MORE_INSTANCES )
   {
      cmsLog_error( "getVpLineCodecListObjRet=cmsObj_getNextInSubTree()=%i\n"
      , getVpLineCodecListObjRet);
   }

   if ( getVpLineCodecListObjRet != CMSRET_SUCCESS )
      return ( getVpLineCodecListObjRet );
   return ( getVpLineObjRet );
}

/***************************************************************************
* Function Name: setVlEnable
* Description  : Enables or disables this line
*                VoiceService.{i}.VoiceProfile.{i}. Line.{i}.Enable = newVal
*                VoiceService.{i}.VoiceProfile.{i}.Enable = "Enabled"
*
* Parameters   : vpInst (IN) Voice Profile Instance Number
*              : lineInst (IN) Line Instance Number
*              : setVal (IN) value to assign to MDM
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setVlEnable ( unsigned int vpInst, unsigned int lineInst, unsigned int setVal )
{
   InstanceIdStack     iidStack1;
   InstanceIdStack     iidStack2;
   CmsRet              getVpObjRet;
   CmsRet              getVpLineObjRet = CMSRET_SUCCESS;
   VoiceProfObject *voiceProfObj = NULL;
   VoiceLineObject *voiceLineObj = NULL;
   LEVELINFO   info1[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                    {0, 0, EMPTY_INSTANCE_ID_STACK}, };
   LEVELINFO   info2[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                    {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                    {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack1 );
   INIT_INSTANCE_ID_STACK( &iidStack2 );

   /*  Get the Voice Profile object */
   if ( (getVpObjRet = getObj(info1, &iidStack1, OGF_NO_VALUE_UPDATE, (void**)&voiceProfObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object \n" );
      return (getVpObjRet);
   }

   /*  Get the Voice Profile object */
   if ( (getVpLineObjRet = getObj(info2, &iidStack2, OGF_NO_VALUE_UPDATE, (void**)&voiceLineObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve line object \n" );
      return (getVpLineObjRet);
   }

   if ( setVal )  /* "on" */
   {
      /* Enable Profile as well, since Disabled profiles cant have enabled lines */
      cmsMem_free ( voiceProfObj->enable );
      voiceProfObj->enable = cmsMem_strdup ( MDMVS_ENABLED );

      /* Set VP Status */
      if ( ( getVpObjRet = cmsObj_set( voiceProfObj,  &iidStack1 )) != CMSRET_SUCCESS )
      {
         cmsLog_error( "getVpLineObjRet=cmsObj_set())=%i\n", getVpObjRet );
         cmsObj_free((void **) &voiceLineObj);
         cmsObj_free((void **) &voiceProfObj);
         return ( getVpObjRet );
      }
      cmsLog_debug( "Successfully set Profile Status == %s for [vpInst] = [%u]\n", voiceProfObj->enable, vpInst);
      cmsObj_free((void **) &voiceProfObj);

      /* Enable Line */
      cmsMem_free ( voiceLineObj->enable );
      voiceLineObj->enable = cmsMem_strdup ( MDMVS_ENABLED );

   }
   else   /* "off" */
   {
      /* Disable Line */
      cmsMem_free ( voiceLineObj->enable );
      voiceLineObj->enable = cmsMem_strdup ( MDMVS_DISABLED );
   }

   /* Set VPLine Status */
   cmsLog_debug( "Line Status == %s for [vpInst, lineInst] = [%u, %u]\n", voiceLineObj->enable, vpInst, lineInst );

   if ( ( getVpLineObjRet = cmsObj_set( voiceLineObj,  &iidStack2 )) != CMSRET_SUCCESS )
   {
      cmsLog_error( "getVpLineObjRet=cmsObj_set()=%i\n", getVpLineObjRet );
      cmsObj_free((void **) &voiceLineObj);
      return ( getVpLineObjRet );
   }

   cmsLog_debug( "Successfully set Line Status == %s for [vpInst, lineInst] = [%u, %u]\n", voiceLineObj->enable, vpInst, lineInst);
   cmsObj_free((void **) &voiceLineObj);
   return ( getVpObjRet );
}



#ifdef SIPLOAD
/***************************************************************************
* Function Name: setVlSipURI
*
* Description  : URI by which the user agent will identify itself for this line
*                VoiceProfile.{i}.Line{i}.Sip.URI = new value
*                VoiceProfile.{i}.Line.{i}.DirectoryNumber = new value
*
* Parameters   : vpInst (IN) Voice Profile Instance Number
*              : lineInst (IN) Line Instance Number
*              : setVal (IN) value to assign to MDM
*
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setVlSipURI ( unsigned int vpInst, unsigned int lineInst, const char * setVal )
{
   InstanceIdStack     iidStack;
   CmsRet              getVpLineObjRet, getVpLineSipObjRet = CMSRET_SUCCESS;
   VoiceLineObject     *voiceLineObj = NULL;
   VoiceLineSipObject  *voiceLineSipObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );

   /* Get the Voice Line Object to update */
   if ( ( getVpLineObjRet = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineObj)) == CMSRET_SUCCESS )
   {
      /* to do: we store setVal in two locations, ie Line->directoryNumber, and Line.{i}.Sip->URI */
      /* This may need to be changed in future */
      cmsMem_free( voiceLineObj->directoryNumber );
      voiceLineObj->directoryNumber = cmsMem_strdup ( setVal );

      cmsLog_debug( "Line Directory Number == %s for [vpInst, lineInst] = [%u, %u]\n", setVal, vpInst, lineInst );

      /* copy new value from local copy to MDM */
      if ( ( getVpLineObjRet = cmsObj_set( voiceLineObj,  &iidStack)) == CMSRET_SUCCESS )
      {
         cmsLog_debug( "Successfully set Line Directory Name == %s\n", voiceLineObj->directoryNumber );
      }
      else
      {
         cmsLog_error( "getVpLineObjRet=cmsObj_set()=%i\n", getVpLineObjRet );
      }

       /* re-initialize iid stack */
      INIT_INSTANCE_ID_STACK( &iidStack );

      /*  Get the Voice Line Sip object */
      if ( ( getVpLineSipObjRet = cmsObj_getNextInSubTree( MDMOID_VOICE_LINE_SIP
         , &info[1].iidStack, &iidStack, (void **)&voiceLineSipObj)) == CMSRET_SUCCESS )
      {
         cmsMem_free( voiceLineSipObj->URI );
         voiceLineSipObj->URI = cmsMem_strdup ( setVal );

         cmsLog_debug( "SIP URI == %s for [vpInst, lineInst] = [%u, %u]\n", setVal, vpInst, lineInst );
         /* copy new value from local copy to MDM */
         if ( ( getVpLineSipObjRet = cmsObj_set( voiceLineSipObj,  &iidStack)) == CMSRET_SUCCESS )
         {
            cmsLog_debug( "Successfully set SIP URI == %s\n", voiceLineSipObj->URI);
         }
         else
         {
            cmsLog_error( "getVpLineSipObjRet=cmsObj_set()=%i\n", getVpLineSipObjRet);
         }

         cmsObj_free((void **) &voiceLineObj);
         cmsObj_free((void **) &voiceLineSipObj);
      }
      else  /* Get Voice Line Sip was not successful */
      {
         cmsLog_error( "getVpLineSipObjRet=cmsObj_getNextInSubTree()=%i\n", getVpLineSipObjRet);
         cmsObj_free((void **) &voiceLineObj);
      }
   }
   else  /* Get Voice Line was not successful */
   {
      cmsLog_error( "getVpLineObjRet=getVpLineObj())=%i\n", getVpLineObjRet );
   }

   return getVpLineSipObjRet != CMSRET_SUCCESS? getVpLineSipObjRet: getVpLineObjRet;
}


/***************************************************************************
* Function Name: setSipTransport
* Description  : Transport protocol to be used in connecting to the SIP server
*                VoiceProfile.{i}.Sip.ProxyServerTransport = new value
*                VoiceProfile.{i}.Sip.RegistrarServerTransport = new value
*                VoiceProfile.{i}.Sip.UserAgentTransport = new value
*                We only support one protocol at a time, so we write to all
*
* Parameters   : vpInst (IN) Voice Profile Instance Number
*              : setVal (IN) value to assign to MDM
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setSipTransport ( unsigned int vpInst, char * setVal )
{
   CmsRet               ret;
   InstanceIdStack      iidStack;
   VoiceProfSipObject   *sipObj     = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]      = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                         {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                         {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&sipObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->proxyServerTransport );
   sipObj->proxyServerTransport = cmsMem_strdup ( setVal );

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->registrarServerTransport );
   sipObj->registrarServerTransport = cmsMem_strdup ( setVal );

   /* set the new value in local copy, after freeing old memory */
   cmsMem_free( sipObj->userAgentTransport );
   sipObj->userAgentTransport = cmsMem_strdup ( setVal );

   cmsLog_debug( "SIP Transport Protocl == %s for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( sipObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "setSipTransport: Can't set sip transport ret = %d\n", ret);
   }

   cmsObj_free((void **) &sipObj);

   return ret;
}

/***************************************************************************
* Function Name: setBackToPrimOption
* Description  : SIP failover back-to-primary configuration
*                VoiceProfile.{i}.SIP.X_BROADCOM_COM_BackToPrimMode
*
* Parameters   : vpInst (IN) Voice Profile Instance Number
*              : setVal (IN) value to assign to MDM
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setBackToPrimOption ( unsigned int vpInst, char * setVal )
{
   CmsRet                  ret;
   InstanceIdStack         iidStack        = EMPTY_INSTANCE_ID_STACK;
   VoiceProfSipObject    *obj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]    = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                      {MDMOID_VOICE_PROF_SIP, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                      {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );

   /*  Get the SIP object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve SIP object \n" );
      return ( ret );
   }

   REPLACE_STRING_IF_NOT_EQUAL(obj->X_BROADCOM_COM_BackToPrimMode, setVal);

   cmsLog_debug( "Back-to-primary configuration == %s for [vpInst] = [%u]\n", setVal, vpInst );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "setBackToPrimOption: Can't set back-to-primary option ret = %d\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

#endif /* SIPLOAD */


/***************************************************************************
** Function Name: setVlCFFeatureValue
** Description  : Sets a call feature value from a CM endpoint
**
** Parameters   : callType (IN) Type of call feature field
**              : vpInst (IN) Voice Profile Instance Number
**              : lineInst (IN) Line Instance Number
**              : callFeatId (IN) Call feature ID
**              : setVal (IN) Call feature field value
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet setVlCFFeatureValue( CFCALLTYPE callType, unsigned int vpInst, unsigned int lineInst,
                                               DAL_VOICE_FEATURE_CODE callFeatId, UINT32 setVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                    {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                    {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                    {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* Set call feature value */
   setVlCFFeatureField(callType, voiceLineCallingFeaturesObj, callFeatId, setVal);

   if ( (ret = cmsObj_set(voiceLineCallingFeaturesObj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "could not save call feature object ret = %d\n", ret);
   }

   cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ret;
}

/***************************************************************************
** Function Name: setVlCFFeatureField
** Description  : Stores the call feature field to the MDM objects
**
** Parameters   : callType (IN) Type of call feature field
**              : cfObject (IN) Call feature object
**              : callFeatId (IN) Call feature ID
**              : setVal (OUT) Call feature field value
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static void setVlCFFeatureField( CFCALLTYPE callType, VoiceLineCallingFeaturesObject *cfObject,
                                         DAL_VOICE_FEATURE_CODE callFeatId, UINT32 setVal)
{
   switch(callFeatId)
   {
      case DAL_VOICE_FEATURE_CODE_CALLWAIT:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->callWaitingEnable = (UINT8)setVal;               break;
            case CFCALLTYPE_STARTED: cfObject->X_BROADCOM_COM_CallWaitingStart = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_CALLWAIT_ONCE:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->callWaitingEnable = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_FWD_OFF:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->callForwardOnNoAnswerEnable = cfObject->callForwardOnBusyEnable = 0;                             break;
            case CFCALLTYPE_STARTED: cfObject->X_BROADCOM_COM_CallForwardOnNoAnswerStart = cfObject->X_BROADCOM_COM_CallForwardOnBusyStart = 0; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_FWD_NOANS:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->callForwardOnNoAnswerEnable = (UINT8)setVal;               break;
            case CFCALLTYPE_STARTED: cfObject->X_BROADCOM_COM_CallForwardOnNoAnswerStart = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_FWD_BUSY:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->callForwardOnBusyEnable = (UINT8)setVal;               break;
            case CFCALLTYPE_STARTED: cfObject->X_BROADCOM_COM_CallForwardOnBusyStart = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_FWD_ALL:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->callForwardUnconditionalEnable = (UINT8)setVal;               break;
            case CFCALLTYPE_STARTED: cfObject->X_BROADCOM_COM_CallForwardUnconditionalStart = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_CALLRETURN:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->callReturnEnable = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_CALLREDIAL:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->repeatDialEnable = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_ANON_REJECT:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->anonymousCallBlockEnable = (UINT8)setVal;               break;
            case CFCALLTYPE_STARTED: cfObject->X_BROADCOM_COM_AnonymousCallBlockStart = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_CID_BLOCK:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->callerIDEnable = (UINT8)setVal ; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_ANON_CALL:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->anonymousCallEnable = (UINT8)setVal;               break;
            case CFCALLTYPE_STARTED: cfObject->X_BROADCOM_COM_AnonymousCallStart = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_DND:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->doNotDisturbEnable = (UINT8)setVal;               break;
            case CFCALLTYPE_STARTED: cfObject->X_BROADCOM_COM_DoNotDisturbStart = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_TRANSFER:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->callTransferEnable = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_CONFERENCING:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED: cfObject->maxSessions = setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_CCBS:
         switch(callType)
         {
           case CFCALLTYPE_ENABLED: cfObject->X_BROADCOM_COM_CAllCompletionOnBusyEnable = setVal; break;
           default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_SPEEDDIAL:
         switch(callType)
         {
           case CFCALLTYPE_ENABLED: cfObject->X_BROADCOM_COM_SpeedDialEnable = setVal; break;
           default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_WARM_LINE:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED:    cfObject->X_BROADCOM_COM_WarmLineEnable = (UINT8)setVal; break;
            case CFCALLTYPE_STARTED: cfObject->X_BROADCOM_COM_WarmLineStart = (UINT8)setVal;  break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_CALL_BARRING:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED:    cfObject->X_BROADCOM_COM_CallBarringEnable = (UINT8)setVal; break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_NET_PRIV:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED:    cfObject->X_BROADCOM_COM_NetworkPrivacyEnable = (UINT8)setVal; break;
            case CFCALLTYPE_STARTED: cfObject->X_BROADCOM_COM_NetworkPrivacyStart = (UINT8)setVal;  break;
            default: break;
         }
         break;

      case DAL_VOICE_FEATURE_CODE_ENUM:
         switch(callType)
         {
            case CFCALLTYPE_ENABLED:    cfObject->X_BROADCOM_COM_EnumEnable = (UINT8)setVal; break;
            case CFCALLTYPE_STARTED:    cfObject->X_BROADCOM_COM_EnumStart = (UINT8)setVal; break;
            default: break;
         }


      default:
         break;
   }
}

/*<END>===================================== Set Helper Functions ========================================<END>*/

/*<START>================================= Get Helper Functions ==========================================<START>*/


/***************************************************************************
** Function Name: getVlCFFeatureValue
** Description  : Gets a call feature value for a CM endpoint
**
** Parameters   : callType (IN) Type of call feature field
**              : vpInst (IN) Voice Profile Instance Number
**              : lineInst (IN) Line Instance Number
**              : callFeatId (IN) Call feature ID
**              : getVal (OUT) Call feature field value
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet getVlCFFeatureValue( CFCALLTYPE callType, unsigned int vpInst, unsigned int lineInst,
                                               DAL_VOICE_FEATURE_CODE callFeatId, int* getVal )
{
   CmsRet              ret;
   InstanceIdStack     iidStack;
   VoiceLineCallingFeaturesObject *voiceLineCallingFeaturesObj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_CALLING_FEATURES, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCallingFeaturesObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve calling features object \n" );
      return ( ret );
   }

   /* Get call feature value */
   getVlCFFeatureField(callType, voiceLineCallingFeaturesObj, callFeatId, getVal);
   cmsObj_free((void **) &voiceLineCallingFeaturesObj);

   return ret;
}

/***************************************************************************
** Function Name: getVlCFFeatureField
** Description  : Retrieves the call feature field from the MDM objects
**
** Parameters   : callType (IN) Type of call feature field
**              : cfObject (IN) Call feature object
**              : callFeatId (IN) Call feature ID
**              : getVal (OUT) Call feature field value
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static void getVlCFFeatureField(CFCALLTYPE callType, VoiceLineCallingFeaturesObject *cfObject,
                                         DAL_VOICE_FEATURE_CODE callFeatId, int* getVal)
{
    switch(callFeatId)
    {
        case DAL_VOICE_FEATURE_CODE_CALLWAIT:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:   *getVal = cfObject->callWaitingEnable;                 break;
              case CFCALLTYPE_STARTED: *getVal = cfObject->X_BROADCOM_COM_CallWaitingStart; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_CALLWAIT_ONCE:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:   *getVal = cfObject->callWaitingEnable; break;
              case CFCALLTYPE_STARTED:   *getVal = cfObject->X_BROADCOM_COM_CallWaitingStart; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_FWD_OFF:
        case DAL_VOICE_FEATURE_CODE_FWD_NOANS:
        case DAL_VOICE_FEATURE_CODE_FWDNUMBER:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->callForwardOnNoAnswerEnable;               break;
              case CFCALLTYPE_STARTED: *getVal = cfObject->X_BROADCOM_COM_CallForwardOnNoAnswerStart; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_FWD_BUSY:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->callForwardOnBusyEnable;               break;
              case CFCALLTYPE_STARTED: *getVal = cfObject->X_BROADCOM_COM_CallForwardOnBusyStart; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_FWD_ALL:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->callForwardUnconditionalEnable;               break;
              case CFCALLTYPE_STARTED: *getVal = cfObject->X_BROADCOM_COM_CallForwardUnconditionalStart; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_CALLRETURN:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->callReturnEnable; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_CALLREDIAL:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->repeatDialEnable; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_ANON_REJECT:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->anonymousCallBlockEnable;               break;
              case CFCALLTYPE_STARTED: *getVal = cfObject->X_BROADCOM_COM_AnonymousCallBlockStart; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_CID_BLOCK:
        case DAL_VOICE_FEATURE_CODE_CID_UNBLOCK:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->callerIDEnable; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_ANON_CALL:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->anonymousCallEnable;               break;
              case CFCALLTYPE_STARTED: *getVal = cfObject->X_BROADCOM_COM_AnonymousCallStart; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_DND:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->doNotDisturbEnable;               break;
              case CFCALLTYPE_STARTED: *getVal = cfObject->X_BROADCOM_COM_DoNotDisturbStart; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_TRANSFER:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->callTransferEnable; break;
              default: break;
           }
           break;

        case DAL_VOICE_FEATURE_CODE_CONFERENCING:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->maxSessions; break;
              default: break;
           }
           break;

       case DAL_VOICE_FEATURE_CODE_CCBS:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->X_BROADCOM_COM_CAllCompletionOnBusyEnable; break;
              default: break;
           }
           break;

       case DAL_VOICE_FEATURE_CODE_SPEEDDIAL:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->X_BROADCOM_COM_SpeedDialEnable; break;
              default: break;
           }
           break;

       case DAL_VOICE_FEATURE_CODE_WARM_LINE:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->X_BROADCOM_COM_WarmLineEnable; break;
              case CFCALLTYPE_STARTED: *getVal = cfObject->X_BROADCOM_COM_WarmLineStart;  break;
              default: break;
           }
           break;

       case DAL_VOICE_FEATURE_CODE_CALL_BARRING:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->X_BROADCOM_COM_CallBarringEnable; break;
              default: break;
           }
           break;

       case DAL_VOICE_FEATURE_CODE_NET_PRIV:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->X_BROADCOM_COM_NetworkPrivacyEnable; break;
              case CFCALLTYPE_STARTED: *getVal = cfObject->X_BROADCOM_COM_NetworkPrivacyStart;  break;
              default: break;
           }
           break;

       case DAL_VOICE_FEATURE_CODE_ENUM:
           switch(callType)
           {
              case CFCALLTYPE_ENABLED:    *getVal = cfObject->X_BROADCOM_COM_EnumEnable; break;
              case CFCALLTYPE_STARTED:    *getVal = cfObject->X_BROADCOM_COM_EnumStart; break;
              default: break;
           }
           break;

       default:
           break;
    }
}

#ifdef SIPLOAD
/***************************************************************************
** Function Name: getVlCLCodecList
** Description  : Gets the voice encoder priority list for a CM endpoint
**
** Parameters   : vpInst (IN) Voice Profile Instance Number
**              : lineInst (IN) Line Instance Number
**              : codec (OUT) Voice encoder concat sorted priority list
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet getVlCLCodecList( unsigned int vpInst, unsigned int lineInst, void *codec, unsigned int length )
{
   CmsRet              ret;
   InstanceIdStack     iidStack;
   int                 codecCount = 0;
   int                 i,maxPrefCodecs,maxCodecs;
   VoiceLineCodecListObject *voiceLineCodecListObj = NULL;
   VoiceLineCodecListObject **codecList = NULL;
   char tmp[40];

   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_CODEC_LIST, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );

   maxCodecs = 0;
   if ( (CMSRET_SUCCESS == rutVoice_getMaxCodecs( &maxCodecs ) ) && maxCodecs != 0 )
   {
      codecList = ( VoiceLineCodecListObject ** ) malloc( sizeof( VoiceLineCodecListObject *) * maxCodecs );
      if (codecList == NULL )
      {
         cmsLog_error( "Can not allocate memory");
         return CMSRET_RESOURCE_EXCEEDED;
      }
   }
   else
   {
      cmsLog_error( "No codec supported!! maxCodecs = %d", maxCodecs );
      return CMSRET_INTERNAL_ERROR;
   }

   /* initialize codec list */
   for ( i=0;i<maxCodecs;i++ )
   {
      codecList[i] = NULL;
   }

   /*  Get the Voice Line Codec object */
   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCodecListObj);
        (ret != CMSRET_NO_MORE_INSTANCES) && (codecCount < maxCodecs);
        ret = cmsObj_getNextInSubTreeFlags( MDMOID_VOICE_LINE_CODEC_LIST, &info[1].iidStack, &iidStack, OGF_NO_VALUE_UPDATE,
                                            (void **)&voiceLineCodecListObj) )
   {
      if ( voiceLineCodecListObj->enable )
      {
         codecList[codecCount++] = voiceLineCodecListObj;
      }
   }

   /* Sort the codec list based on priority */
   selectionSort(codecList, codecCount);

   /* Get maximum prefcodecs */
   rutVoice_getMaxPrefCodecs( &maxPrefCodecs );
   if (maxPrefCodecs > maxCodecs)
   {
      maxPrefCodecs = maxCodecs;
   }

   /* Ensure a null-terminated, initially zero-length, string */
   ((char *)codec)[0] = '\0';

   for ( i = 0; i < maxPrefCodecs; i++ )
   {
      /* If the field's value does not exist return failure */
      if ( codecList[i] == NULL )
      {
         cmsLog_debug( "No codec enabled at priority %d", i );
      }
      else
      {
         sprintf(tmp, "%s", codecList[i]->codec);
         if(i)
            strncat(codec, ",", length);
         strncat(codec, tmp, length);
      }
   }

   /* Free codec List objects */
   while(--codecCount >= 0)
   {
      cmsObj_free((void **) &codecList[codecCount]);
   }

   free(codecList);

   return (ret != CMSRET_NO_MORE_INSTANCES) ? ret : CMSRET_SUCCESS;
}

#endif /* SIPLOAD */


/***************************************************************************
** Function Name: getVlCLEncoder
** Description  : Gets the voice encoder value for a CM endpoint
**
** Parameters   : vpInst (IN) Voice Profile Instance Number
**              : lineInst (IN) Line Instance Number
**              : priority (IN) Voice encoder priority
**              : codec (OUT) Voice encoder
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet getVlCLEncoder( unsigned int vpInst, unsigned int lineInst, unsigned int priority, void *codec )
{
   CmsRet              ret;
   InstanceIdStack     iidStack;
   VoiceLineCodecListObject *voiceLineCodecListObj = NULL;
   LEVELINFO   info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                   {MDMOID_VOICE_LINE_CODEC_LIST, ANY_INSTANCE, EMPTY_INSTANCE_ID_STACK},
                                   {0, 0, EMPTY_INSTANCE_ID_STACK}, };
   VoiceLineCodecListObject *codecList[50];
   int codecCount = 0;


   /* initialize iid stack */
   INIT_INSTANCE_ID_STACK( &iidStack );

   /*  Get the Voice Line Codec object */
   for( ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&voiceLineCodecListObj); ret != CMSRET_NO_MORE_INSTANCES;
        ret = cmsObj_getNextInSubTreeFlags( MDMOID_VOICE_LINE_CODEC_LIST, &info[1].iidStack, &iidStack,
                                       OGF_NO_VALUE_UPDATE, (void **)&voiceLineCodecListObj) )
   {
      if ( voiceLineCodecListObj->enable )
      {
         codecList[codecCount++] = voiceLineCodecListObj;
      }
   }

   /* Sort the codec list based on priority */
   selectionSort(codecList, codecCount);

   /* If the field's value does not exist return failure */
   if ( codecList[priority] == NULL )
   {
      cmsLog_debug( "No codec selected at priority %u", priority );
      *((int*)codec) = 0;
      ret = CMSRET_REQUEST_DENIED;
   }
   else
   {
      /* Copy codec value */
      *((int*)codec) = codecList[priority]->entryID;
   }

   /* Free codec List objects */
   while(--codecCount >= 0)
   {
      cmsObj_free((void **) &codecList[codecCount]);
   }


   return (ret != CMSRET_NO_MORE_INSTANCES) ? ret : CMSRET_SUCCESS;
}



/*****************************************************************************
* FUNCTION:   selectionSort
*
* PURPOSE:    A simple insertion sort algorithm used to sort the codec
*             entries of a list based on their priorities.
*
* PARAMETERS: The codec list to be sorted.
*
* RETURNS:
*
* NOTE:
*****************************************************************************/
static void selectionSort(VoiceLineCodecListObject **codecList, unsigned int codecCount)
{
   int maxindex;
   VoiceLineCodecListObject *entry;
   int i, j;

   for(i = codecCount - 1; i > 0; i--)
   {
      for(maxindex = j = 0; j <= i; j++)
      {
         if ( codecList[j]->priority >
              codecList[maxindex]->priority )
            maxindex = j;
      }

      if ( maxindex != i )
      {
         entry = codecList[i];
         codecList[i] = codecList[maxindex];
         codecList[maxindex] = entry;
      }
   }
}



/*<END>================================= Get Helper Functions ============================================<END>*/

/*<START>================================= Common Helper Functions =======================================<START>*/

#ifdef MGCPLOAD
/***************************************************************************
* Get a TR104 VoiceProfile.{i}
*
* The caller must have already acquired a CMS lock via cmsLck_acquireLock(), as well as
* initialized '*iidStack' to EMPTY_INSTANCE_ID_STACK
*
* @param vpInst    (IN) The Voice Profile Instance
* @param iidStack  (OUT) The instance information for the requested MdmObject.
* @param obj       (OUT) On successful return, obj points to the requested MdmObject.
*                   If 'vpInst' == 0, then the very first object instantiatied by cmsObj_getNext
*                   is returned (no match checking).  Else, iterate until cmsObj_getNext
*                   instantiates a matching 'vpInst'.
*                   The caller must free this object by calling cmsObj_free()
*                  (not cmsMem_free).
* @return CmsRet enum.
****************************************************************************/
static CmsRet getVpObj ( unsigned int vpInst, InstanceIdStack * iidStack, VoiceProfObject ** obj )
{
   CmsRet ret;

   if ( vpInst != 0 )  /* return the object whose Voice Profile Instance == 'vpInst' */
   {
      /* Traverse objects until match found, or getting next object is not successful */
      while (( ret = cmsObj_getNext ( MDMOID_VOICE_PROF, iidStack, (void **) obj )) == CMSRET_SUCCESS )
      {
         /* If match found, then break.  'obj' now points to correct object */
         if ( ( PEEK_INSTANCE_ID ( iidStack )) == vpInst )
            break;

         /* free up unwanted object */
         cmsObj_free(( void ** ) obj );
      }
   }
   else  /* return the first object instantiated */
   {
      return ( cmsObj_getNext ( MDMOID_VOICE_PROF, iidStack, (void **) obj ));
   }

   if ( ret != CMSRET_SUCCESS )
      cmsLog_error( "ret=cmsObj_getNext())=%i\n", ret);

   return ret;
}
#endif

#if 0
#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
/***************************************************************************
* Get a TR104 X_BROADCOM_COM_PSTN.{i}
*
* The caller must have already acquired a CMS lock via cmsLck_acquireLock(), as well as
* initialized '*iidStack' to EMPTY_INSTANCE_ID_STACK
*
* @param pstnInst  (IN) The PSTN Profile Instance
* @param iidStack  (OUT) The instance information for the requested MdmObject.
* @param obj       (OUT) On successful return, obj points to the requested MdmObject.
*                   If 'pstnInst' == 0, then the very first object instantiatied by cmsObj_getNext
*                   is returned (no match checking).  Else, iterate until cmsObj_getNext
*                   instantiates a matching 'vpInst'.
*                   The caller must free this object by calling cmsObj_free()
*                  (not cmsMem_free).
* @return CmsRet enum.
****************************************************************************/
static CmsRet getPstnObj ( unsigned int pstnInst, InstanceIdStack * iidStack, VoicePstnObject ** obj )
{
   CmsRet ret;

   if ( pstnInst != 0 )  /* return the object whose PSTN Profile Instance == 'pstnInst' */
   {
      /* Traverse objects until match found, or getting next object is not successful */
      while (( ret = cmsObj_getNext ( MDMOID_VOICE_PSTN, iidStack, (void **) obj )) == CMSRET_SUCCESS )
      {
         /* If match found, then break.  'obj' now points to correct object */
         if ( ( PEEK_INSTANCE_ID ( iidStack )) == pstnInst )
            break;

         /* free up unwanted object */
         cmsObj_free(( void ** ) obj );
      }
   }
   else  /* return the first object instantiated */
   {
      return ( cmsObj_getNext ( MDMOID_VOICE_PSTN, iidStack, (void **) obj ));
   }

   if ( ret != CMSRET_SUCCESS )
      cmsLog_error( "ret=cmsObj_getNext()=%i\n", ret);

   return ret;
}
#endif

/***************************************************************************
* Get a TR104 VoiceProfile.{i}.Line{i}
*
* The caller must have already acquired a CMS lock via cmsLck_acquireLock(), as well as
* initialized '*iidStack' to EMPTY_INSTANCE_ID_STACK
*
* @param vpInst    (IN) The Voice Profile Instance
* @param LineInst  (IN) The Line Instance
* @param iidStack  (OUT) The instance information for the requested MdmObject.
* @param obj       (OUT) On successful return, obj points to the requested MdmObject.
*                   If 'vpInst' == 0, then the very first object instantiatied by cmsObj_getNextInSubTree
*                   is returned (no match checking).  Else, iterate until cmsObj_getNextInSubTree
*                   instantiates a matching 'vpInst'.
*                   The caller must free this object by calling cmsObj_free()
*                  (not cmsMem_free).
* @return CmsRet enum.
****************************************************************************/
static CmsRet getVpLineObj ( unsigned int vpInst, unsigned int LineInst, InstanceIdStack * iidStack, VoiceLineObject ** obj )
{
   CmsRet ret;
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject * voiceProfObj = NULL;

   if ( (ret = getVpObj(vpInst, &parentIidStack, &voiceProfObj)) == CMSRET_SUCCESS)
   {
      /* free up unwanted object; we only need parentIidStack filled-in */
      cmsObj_free((void **) &voiceProfObj);

      if ( vpInst != 0 )  /* return the object whose Voice Profile Instance == 'vpInst' */
      {
         /* Traverse objects until match found, or gettting next object is not successful */
         while ((ret = cmsObj_getNextInSubTree(MDMOID_VOICE_LINE, &parentIidStack, iidStack, (void **) obj)) == CMSRET_SUCCESS)
         {
            /* If match found, then break.  'obj' now points to correct object */
            if ( ( PEEK_INSTANCE_ID ( iidStack )) == LineInst )
               break;

            /* free up unwanted object */
            cmsObj_free((void **) obj);
         }

         if (ret != CMSRET_SUCCESS)
            cmsLog_error( "ret=cmsObj_getNextInSubTree()=%i)\n", ret);
      }
      else  /* return the first object instantiated */
      {
         return ( cmsObj_getNextInSubTree(MDMOID_VOICE_LINE, &parentIidStack, iidStack, (void **) obj ));
      }
   }
   else
   {
      cmsLog_error( "ret=getVpObj()=%i\n", ret);
   }
   return ret;
}
#endif


/***************************************************************************
* Function Name: getIndexInCliCodecList
* Description  : Tells whether a particular codec exists in the tokenized codec list
*
* Parameters   : codecList (IN)
* Returns      : index (base 0) of 'codecList' which equals 'codec' using strcmp()
****************************************************************************/
static int getIndexInCliCodecList( char ** codecList, const char * codec, int numSetCodec )
{
   int i = 0;

   if ( codec == NULL )
   {
      cmsLog_error( "codec=NULL\n" );fflush(stdout);fflush(stderr);
      return -2;
   }

   for ( i=0; i<numSetCodec; i++ )
   {

      if ( !strcmp( codecList[i], codec ))
         return ( i ) ;

   }

   return -1;
}


/***************************************************************************
* Function Name: dumpGlobalParams
* Description  : Dump global parmaters
****************************************************************************/
static CmsRet dumpGlobalParams()
{
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue = NULL;

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   /* Fill in the parameter structure needed for dalVoice_GetXYZ.
   ** Since these are global parameters, the voice profile and
   ** line instance are irrelevant so we hard-code them. */
   parms.op[0] = 1;
   parms.op[1] = 1;

   printf( "    \n\n" );
   printf( "Global Parameters:\n" );
   printf( "------------------\n" );

   if ( dalVoice_GetBoundIfName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "BoundIfName          : %s\n", objValue );
   }

   if ( dalVoice_GetIpFamily( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "IP address family    : %s\n", objValue );
   }

   /* Reset objValue */
   objValue[0] = '\0';

   if ( dalVoice_GetModuleLoggingLevels( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "Voice Log Levels     : %s\n", objValue );
   }

   /* Reset objValue */
   objValue[0] = '\0';

   if ( dalVoice_GetManagementProtocol( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "Management Protocol  : %s\n", objValue );
   }

#ifdef BRCM_SIP_VOICE_DNS
   /* Reset objValue */
   objValue[0] = '\0';

   if ( dalVoice_GetDnsServerAddr( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "Voice DNS Server     : %s\n", objValue );
   }
#endif // BRCM_SIP_VOICE_DNS

   cmsMem_free( objValue );

   return (ret);
}

#ifdef SIPLOAD
/***************************************************************************
* Function Name: dumpServiceProviderParams
* Description  : Dump service provider specific parmaters.
* These parameters have a per line scope in callmanager and are stored
* at the voice profile level in TR104
****************************************************************************/
static CmsRet dumpServiceProviderParams(int spNum)
{
   int vpInst;
   DAL_VOICE_PARMS parms;
   CmsRet ret = CMSRET_SUCCESS;
   char* objValue = NULL;

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   /* Mapping spnum to vpInst */
   mapSpNumToVpInst ( spNum, &vpInst ) ;

   /* Fill in the parameter structure needed for dalVoice_GetXYZ.
   ** Since these are voice profile parameters, the line instance
   ** is irrelevant so we hard-code it. */
   parms.op[0] = vpInst;
   parms.op[1] = 1;

   /* Get voiceProfile object */

   printf( "\n" );
   printf( "Service Provider %d:\n", spNum);
   printf( "--------------------\n" );

   printf( "   Associated Voice Profile: %d\n", vpInst );

   if ( dalVoice_GetRegion( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {

      printf( "   Locale                  : %s\n", objValue );
   }

   /* Reset objValue */
   objValue[0] = '\0';
   if ( dalVoice_GetDTMFMethod( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "   DTMFMethod              : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetHookFlashMethodString( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "   HookFlashMethod         : %s\n", objValue );
   }

#ifdef STUN_CLIENT
   objValue[0] = '\0';
   if ( dalVoice_GetSTUNServer( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "   STUNServer              : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSTUNServerPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "   STUNServerPort          : %d\n", objValue );
   }
#endif

   objValue[0] = '\0';
   if ( dalVoice_GetDigitMap( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "   DigitMap                : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetLogServer( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "   Log Server Addr         : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetLogServerPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "   Log Server Port         : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetT38Enable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "   T38                     : %s\n", atoi(objValue) ? "on" : "off" );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetV18Enable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "   V18                     : %s\n", atoi(objValue) ? "on" : "off" );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetRtpDSCPMark( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "   RTPDSCPMark             : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetEuroFlashEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "   European flash          : %s\n", objValue );
   }

   /* SIP parameters */
   printf( "   SIP:                       \n" );

   objValue[0] = '\0';
   if ( dalVoice_GetSipUserAgentDomain( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Domain               : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipUserAgentPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Port                 : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipTransportString( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Transport            : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipRegisterExpires( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      RegExpires           : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipRegisterRetryInterval( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      RegRetryInterval     : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipDSCPMark( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      DSCPMark             : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipRegistrarServer( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Registrar Addr       : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipRegistrarServerPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Registrar Port       : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipProxyServer( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Proxy Addr           : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipProxyServerPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Proxy Port           : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipOutboundProxy( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      OutBoundProxy Addr   : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipOutboundProxyPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      OutBoundProxy Port   : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipMusicServer( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Music Server Addr    : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipMusicServerPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Music Server Port    : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipConferencingURI( &parms, objValue, 128 ) == CMSRET_SUCCESS )
   {
      printf( "      Conferencing URI     : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipConferencingOption( &parms, objValue, 32 ) == CMSRET_SUCCESS )
   {
      printf( "      Conferencing Option  : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipFailoverEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Failover Enable      : %s\n", atoi(objValue) ?MDMVS_ON :MDMVS_OFF);
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipOptionsEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Sip OPTIONS Enable   : %s\n", atoi(objValue) ?MDMVS_ON :MDMVS_OFF);
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipBackToPrimOptionString( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Back-to-primary cfg  : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipSecDomainName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Secondary Domain     : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipSecProxyAddr( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Secondary Registrar Addr : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipSecProxyPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Secondary Registrar Port : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipSecProxyAddr( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Secondary Proxy Addr : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipSecProxyPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Secondary Proxy Port : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipSecOutboundProxyAddr( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Secondary Outbound Proxy Addr : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipSecOutboundProxyPort( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Secondary Outbound Proxy Port : %s\n", objValue );
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipToTagMatching( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      To Tag Matching      : %s\n", atoi(objValue) ?MDMVS_ON :MDMVS_OFF);
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipTimerB( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Timer B ( in ms )    : %s\n", objValue);
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSipTimerF( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Timer F ( in ms )    : %s\n", objValue);
   }

   objValue[0] = '\0';
   if ( dalVoice_GetSrtpOptionString( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      SRTP Usage Option    : %s\n", objValue);
   }

   cmsMem_free( objValue );

   return (ret);
}

/***************************************************************************
* Function Name: dumpAccountParams
* Description  : Dump Account specific parmaters
****************************************************************************/
static CmsRet dumpAccountParams(int spNum, int accNum)
{
   CmsRet ret = CMSRET_SUCCESS;
   int vpInst,lineInst,i;
   DAL_VOICE_PARMS parms;
   char* objValue = NULL;

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   /* Mapping spnum to vpInst */
   mapSpNumToVpInst ( spNum, &vpInst ) ;

   /* Mapping accNum to vpLine inst */
   mapAcntNumToLineInst ( vpInst, accNum , &lineInst );

   /* Fill in the parameter structure needed for dalVoice_GetXYZ */
   parms.op[0] = vpInst;
   parms.op[1] = lineInst;

   /* Print Parameters */
   printf( "\n" );
   printf( "   Account %d:\n", accNum);
   printf( "   -----------\n" );

   dalVoice_DumpVlStatus( &parms );

   printf( "      Associated LineInst     : %d\n", lineInst );

   if ( dalVoice_GetVlPhyReferenceList( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      PhysEndpt               : %s\n", objValue );
   }

#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   objValue[0] = '\0';

   if ( dalVoice_GetVlAssociatedNonFxsPhyType( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS && !cmsUtl_strcasecmp(objValue, MDMVS_FXO))
   {
      objValue[0] = '\0';

      if ( dalVoice_GetVlFxoPhyReferenceList( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
      {
         printf( "      FxoPhysEndpt            : %s\n", objValue );
      }
   }
#endif /*DMP_X_BROADCOM_COM_PSTNENDPOINT_1 */

   objValue[0] = '\0';

   if ( dalVoice_GetVlSipURI( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      Extension               : %s\n", objValue );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFCallerIDName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      DisplayName             : %s\n", objValue );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlSipAuthUserName( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      AuthName                : %s\n", objValue );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlSipAuthPassword( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      AuthPwd                 : %s\n", objValue );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlVPTransmitGain( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      TxGain                  : %s dB\n", objValue );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlVPRecieveGain( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "      RxGain                  : %s dB\n", objValue );
   }


   printf( "      CALLFEATURES:               \n" );

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFMWIEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         MWI                  : %s\n", atoi(objValue) ? "on" : "off" );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_CALLWAIT;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         CallWaiting          : %s\n", atoi(objValue) ? "on" : "off" );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFCallFwdNum( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         CFWDNum              : %s\n", objValue );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_FWD_ALL;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         CallFwdAll           : %s\n", atoi(objValue) ? "on" : "off" );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_FWD_BUSY;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         CallFwdBusy          : %s\n", atoi(objValue) ? "on" : "off" );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_FWD_NOANS;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         CallFwdNoans         : %s\n", atoi(objValue) ? "on" : "off" );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_ANON_CALL;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         AnonymousOutgoingCall: %s\n", atoi(objValue) ? "on" : "off" );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_ANON_REJECT;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         AnonymousCallRcvBlock: %s\n", atoi(objValue) ? "on" : "off" );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_DND;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         DoNotDisturb         : %s\n", atoi(objValue) ? "on" : "off" );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_CCBS;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         CallCompOnBusy       : %s\n", atoi(objValue) ? "on" : "off" );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_SPEEDDIAL;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         SpeedDial            : %s\n", atoi(objValue) ? "on" : "off" );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_WARM_LINE;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         WarmLine             : %s\n", atoi(objValue) ? "on" : "off" );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFWarmLineNum( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         WarmLineNum          : %s\n", objValue );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_CALL_BARRING;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         CallBarring          : %s\n", atoi(objValue) ? "on" : "off" );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFCallBarringMode( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      if( strchr( objValue, '2' ) != NULL )
      {
         printf( "         CallBarringMode      : %s\n", "DigitMap" );
      }
      else if( strchr( objValue, '1' ) != NULL )
      {
         printf( "         CallBarringMode      : %s\n", "All" );
      }
      else
      {
         printf( "         CallBarringMode      : %s\n", "None" );
      }
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFCallBarringPin( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         CallBarringPin       : %s\n", objValue );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFCallBarringDigitMap( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         CallBarringDigitMap  : %s\n", objValue );
   }

   parms.op[2] = DAL_VOICE_FEATURE_CODE_NET_PRIV;

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFFeatureEnabled( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         NetPrivacy           : %s\n", atoi(objValue) ? "on" : "off" );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlCFVisualMWI( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         VMWI                 : %s\n", atoi(objValue) ? "on" : "off" );
   }

   printf( "      CODECSETTINGS:           \n" );

   objValue[0] = '\0';

   if ( dalVoice_GetVlCLSilenceSuppression( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         VAD                  : %s\n", atoi(objValue) ? "on" : "off" );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlCLPacketizationPeriod( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "         pTime                : %s\n", objValue );
   }

   objValue[0] = '\0';

   if ( dalVoice_GetVlCLCodecList( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      char  delim[2] = ",";
      char *pToken = NULL, *pLast = NULL, *pPrefix = NULL;;
      char  tempBuf[TEMP_CHARBUF_SIZE];

      memset( tempBuf, 0, TEMP_CHARBUF_SIZE );

      /* Print out the codec list, one codec at the time */
      pToken = strtok_r( objValue, delim, &pLast );
      if ( pToken != NULL )
      {
         /* Check for X_BROADCOM_COM_ prefix, remove, print 1st codec */
         if( (pPrefix = strstr( pToken, "X_BROADCOM_COM_" )) != NULL )
         {
            strcpy( tempBuf, (pPrefix + strlen( "X_BROADCOM_COM_" )) );
            i = 0;
            printf( "         CodecList            : (%d) %s\n", i, tempBuf );
            /* Reset buffer */
            memset( tempBuf, 0, TEMP_CHARBUF_SIZE );
         }
         else
         {
            i = 0;
            printf( "         CodecList            : (%d) %s\n", i, pToken );
         }
      }

      while ( pToken != NULL )
      {
         /* Find next Codec in list */
         pToken = strtok_r( NULL, delim, &pLast );
         if ( pToken != NULL )
         {
            /* If found, check for X_BROADCOM_COM_ prefix, remove, print Codec */
            if( (pPrefix = strstr( pToken, "X_BROADCOM_COM_" )) != NULL )
            {
               strcpy( tempBuf, (pPrefix + strlen( "X_BROADCOM_COM_" )) );
               i++;
               printf( "                                (%d) %s\n", i, tempBuf );
               /* Reset buffer */
               memset( tempBuf, 0, TEMP_CHARBUF_SIZE );
            }
            else
            {
               i++;
               printf( "                                (%d) %s\n", i, pToken );
            }
         }
      }
   }

   cmsMem_free( objValue );

   return (ret);
}
#endif /* SIPLOAD */

#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
/***************************************************************************
* Function Name: dumpPstnParams
* Description  : Dump Pstn parmaters
****************************************************************************/
static CmsRet dumpPstnParams()
{
   CmsRet ret = CMSRET_SUCCESS;
   DAL_VOICE_PARMS parms;
   char* objValue = NULL;

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   /* Fill in the parameter structure needed for dalVoice_GetXYZ.
   ** Since these are global parameters, the voice profile and
   ** line instance are irrelevant so we hard-code them. */
   parms.op[0] = 1;
   parms.op[1] = 1;

   /* Print Parameters */
   printf( "    \n\n" );
   printf( "PSTN Parameters:\n" );
   printf( "------------------\n" );

   if ( dalVoice_GetPstnDialPlan( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "PSTN dial Plan        : %s\n", objValue );
   }

   if ( dalVoice_GetPstnRouteRule( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "PSTN routing rule     : %s\n", objValue );
   }

   if ( dalVoice_GetPstnRouteData( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      printf( "PSTN routing data     : %s\n", objValue );
   }

   cmsMem_free( objValue );

   return (ret);
}
#endif

#ifdef DMP_X_BROADCOM_COM_NTR_1
/***************************************************************************
* Function Name: dumpNtrParams
* Description  : Dump NTR parmaters
*                Since there are many parameters for the NTR task, only
*                the settings used by the current mode will be displayed.
****************************************************************************/
static CmsRet dumpNtrParams()
{
   CmsRet ret = CMSRET_SUCCESS;
   DAL_VOICE_PARMS parms;
   char* objValue = NULL;

   /* Allocate memory for the object to be obtained through DAL APIs. */
   objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );
   if ( objValue == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object.\n" );
      return ( CMSRET_RESOURCE_EXCEEDED );
   }

   /* Fill in the parameter structure needed for dalVoice_GetXYZ.
   ** Since these are global parameters, the voice profile and
   ** line instance are irrelevant so we hard-code them. */
   parms.op[0] = 1;
   parms.op[1] = 1;

   /* Print Parameters */
   printf( "    \n\n" );
   printf( "NTR Parameters:\n" );
   printf( "------------------\n" );

   if ( dalVoice_GetNtrEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
   {
      int ntrEnabled = strncmp(objValue,"0",1);
      printf( "Voice NTR Enabled          : %s\n", ntrEnabled?"Enabled":"Disabled" );

      /* Print NTR information if NTR is enabled */
      if(ntrEnabled)
      {
         if ( dalVoice_GetNtrAutoModeEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
         {
            int autoEnabled = strncmp(objValue,"0",1);
            printf( "   Voice NTR Mode          : %s\n", autoEnabled?"Auto":"Manual" );

            if(autoEnabled)
            {
               double sampleRate;
               if ( dalVoice_GetNtrSampleRate( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
               {
                  sampleRate = atof(objValue);
                  printf( "   Voice NTR Sample Rate   : %.4fms\n", sampleRate );
               }
               if ( dalVoice_GetNtrPllBandwidth( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
               {
                  double pllBandwidth = atof(objValue);
                  printf( "   Voice NTR PLL Bandwidth : %.6fHz\n", pllBandwidth );
                  if( pllBandwidth > (sampleRate / 5) )
                  {
                     printf( "   Warning! PLL Bandwidth is higher than ( SampleRate / 5 ) \n");
                  }
               }
               if ( dalVoice_GetNtrDampingFactor( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
               {
                  printf( "   Voice NTR Damping Factor: %s\n", objValue );
               }

               if ( dalVoice_GetNtrAutoOffset( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
               {
                  printf( "   Voice NTR Auto Offset   : %sHz\n", objValue );
               }

               if ( dalVoice_GetNtrAutoPpm( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
               {
                  printf( "   Voice NTR Auto PPM      : %s\n", objValue );
               }

               if ( dalVoice_GetNtrDebugEnable( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
               {
                  int debugEnabled = strncmp(objValue,"0",1);
                  printf( "   Voice NTR Debug Enabled : %s\n", debugEnabled?"Enabled":"Disabled" );
                  /* Only display debug information if debug is enabled */
                  if(debugEnabled)
                  {
                     if ( dalVoice_GetNtrAutoOffsetHistory( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR AutoOffset History: %s\n", objValue );
                     }
                     if ( dalVoice_GetNtrAutoPpmHistory( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR AutoPPM History   : %s\n", objValue );
                     }
                     if ( dalVoice_GetNtrAutoStepsHistory( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR AutoSteps History : %s\n", objValue );
                     }
                     if ( dalVoice_GetNtrAutoPhaseErrorHistory( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR PhaseError History: %s\n", objValue );
                     }
                     if ( dalVoice_GetNtrPcmMipsTallyCurrent( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR Pcm MIPS Current  : %s\n", objValue );
                     }

                     if ( dalVoice_GetNtrPcmMipsTallyPrevious( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR Pcm MIPS Previous : %s\n", objValue );
                     }

                     if ( dalVoice_GetNtrPcmNtrTallyCurrent( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR PCM NTR Current   : %s\n", objValue );
                     }

                     if ( dalVoice_GetNtrPcmNtrTallyPrevious( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR PCM NTR Previous  : %s\n", objValue );
                     }

                     if ( dalVoice_GetNtrDslMipsTallyCurrent( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR DSL MIPS Current  : %s\n", objValue );
                     }

                     if ( dalVoice_GetNtrDslMipsTallyPrevious( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR DSL MIPS Previous : %s\n", objValue );
                     }

                     if ( dalVoice_GetNtrDslNtrTallyCurrent( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR DSL NTR Current   : %s\n", objValue );
                     }

                     if ( dalVoice_GetNtrDslNtrTallyPrevious( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                     {
                        printf( "      Voice NTR DSL NTR Previous  : %s\n", objValue );
                     }
                  }
               }
            }
            else /* Manual Mode */
            {
               if ( dalVoice_GetNtrManualOffset( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
               {
                  printf( "   Voice NTR Manual Offset : %sHz\n", objValue );
               }

               if ( dalVoice_GetNtrManualSteps( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
               {
                  printf( "   Voice NTR Manual Steps  : %s\n", objValue );
               }

               if ( dalVoice_GetNtrManualPpm( &parms, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
               {
                  printf( "   Voice NTR Manual PPM    : %s\n", objValue );
               }
            }
         }
      }
   }

   cmsMem_free( objValue );
   return (ret);
}
#endif /* DMP_X_BROADCOM_COM_NTR_1 */

#ifdef MGCPLOAD
/***************************************************************************
* Function Name: dumpMgcpParams
* Description  : Dump Mgcp parmaters
****************************************************************************/
static CmsRet dumpMgcpParams(void)
{
   CmsRet ret;
   int vpInst = 1;

   /* Objects to be retrieved */
   VoiceProfObject *voiceProfObj = NULL;
   VoiceProfMgcpObject *voiceProfMgcpObj = NULL;

   /* iidStacks */
   InstanceIdStack parentIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Dumping Service Provider Parameters. These parameters have a per line    *
    * scope in callmanager and are stored at the voice profile level in TR104  */

   /* Get voiceProfile object */
   if ( ( ret = getVpObj ( vpInst, &parentIidStack, &voiceProfObj )) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Failed to get vpObj\n" );
      return (ret);
   }

   /* get VoiceProfile.{i}.SIP object */
   if ( ( ret = cmsObj_getNextInSubTree( MDMOID_VOICE_PROF_MGCP,
         &parentIidStack, &iidStack, (void **)&voiceProfMgcpObj )) != CMSRET_SUCCESS )
   {
      cmsLog_error( "dumpServiceProviderParams: Failed to get voiceProfMgcpObj\n" );
      cmsObj_free((void **) &voiceProfObj);
      return (ret);
   }

   /* Print Parameters */
   printf( "\n" );
   printf( "MGCP Paramaters:\n" );
   printf( "--------------------\n" );
   printf( "   Locale                 : %s\n",voiceProfObj->region);
   printf( "   Gateway Name           : %s\n",voiceProfMgcpObj->user);
   printf( "   CallAgent Address      : %s\n",voiceProfMgcpObj->callAgent1);

   /* Free objects */
   cmsObj_free((void **) &voiceProfMgcpObj);
   cmsObj_free((void **) &voiceProfObj);

   return (ret);
}
#endif /* MGCPLOAD */

/***************************************************************************
* Function Name: dumpVoiceParams
* Description  : Dump voice params
* NOTE: This function is used for the 'voice show' CLI command
****************************************************************************/
static CmsRet dumpVoiceParams( void )
{
   CmsRet ret;
#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   int numFxoEndpts;
#endif /* DMP_X_BROADCOM_COM_PSTNENDPOINT_1 */

#ifdef SIPLOAD
   int numPhysFxs;
   int spNum;
   int numAcc;
   int i,j;
#endif /* SIPLOAD  */

   /* Dump global parameters */
   ret = dumpGlobalParams();
   if ( ret != CMSRET_SUCCESS )
   {
      cmsLog_error( "ret = %d \n", ret);
      return(ret);
   }

#ifdef SIPLOAD
   /* get maximum number of physical fxs endpoints */
   dalVoice_GetNumPhysFxsEndpt( &numPhysFxs );

   /* get maximum number of service providers configured */
   dalVoice_GetNumSrvProv( &spNum );

   for ( i = 0; i < spNum; i++ )
   {
      /* Dump per-serviceprovider parameters */
      ret = dumpServiceProviderParams( i );
      if ( ret != CMSRET_SUCCESS )
      {
         cmsLog_error( "ret = %d \n", ret);
         return(ret);
      }

      /* Get max accounts per service provider */
      dalVoice_GetNumAccPerSrvProv( i, &numAcc );

      /* Dump per-account parameters */
      for ( j = 0; j < numAcc; j++ )
      {
         ret = dumpAccountParams( i, j );
         if ( ret != CMSRET_SUCCESS )
         {
            cmsLog_error( "ret = %d \n", ret);
            return(ret);
         }
      }
   }
#endif /* SIPLOAD */

#ifdef MGCPLOAD
   ret = dumpMgcpParams();
#endif /* MGCPLOAD */

#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
   if ( rutVoice_getNumFxoEndpt(&numFxoEndpts) == CMSRET_SUCCESS )
   {
      if ( numFxoEndpts > 0 )
      {
         ret = dumpPstnParams();
      }
   }
#endif /* DMP_X_BROADCOM_COM_PSTNENDPOINT_1 */

#ifdef DMP_X_BROADCOM_COM_NTR_1
   ret = dumpNtrParams();
#endif /* DMP_X_BROADCOM_COM_NTR_1 */
   return (ret);

}

#ifdef SIPLOAD
/***************************************************************************
* Function Name: dumpVoiceStats
* Description  : Dump voice call statistics
* NOTE: This function is used for the 'voice show stats' CLI command
****************************************************************************/
static CmsRet dumpVoiceStats( void )
{
   DAL_VOICE_CALL_STATS_BLK   callStats;
   DAL_VOICE_PARMS            parms;
   int                        vpInst;
   int                        lineInst;
   int                        spNum;
   int                        numAcc;
   int                        i,j;
   CmsRet                     ret;

   printf( "\nCall Statistics:\n" );
   printf( "----------------\n\n" );
   /* get maximum number of service providers configured */
   dalVoice_GetNumSrvProv( &spNum );

   /* Loop through voice profiles */
   for ( i = 0; i < spNum; i++ )
   {
      printf( "Service Provider %d:\n", i);
      printf( "--------------------\n\n" );
      /* Get max accounts per service provider */
      dalVoice_GetNumAccPerSrvProv( i, &numAcc );

      /* Loop through each account in voice profile */
      for ( j = 0; j < numAcc; j++ )
      {
         printf( "   Account %d:\n", j);
         printf( "   -----------\n\n" );
         /* Mapping i to vpInst */
         mapSpNumToVpInst ( i, &vpInst ) ;

         /* Mapping j to vpLine inst */
         mapAcntNumToLineInst ( vpInst, j, &lineInst );

         parms.op[0] = vpInst;
         parms.op[1] = lineInst;

         /* Get call statistics for specific line and profile */
         ret = dalVoice_GetVlStats( &parms, &callStats );

         if( ret != CMSRET_SUCCESS )
         {
            cmsLog_error( "Could not get call stats for voice profile: %d, line: %d. ret = %d\n",
                          vpInst, lineInst, ret );
         }
         else
         {
            /* Print statistics which are currently supported, the rest are always 0 */
            /* The rest of the statistics can be printed here when they are supported */
            printf( "      ResetStatistics                 : %s\n",
                    callStats.resetStatistics ? "True" : "False" );
            printf( "      PacketsSent                     : %u\n", callStats.packetsSent );
            printf( "      PacketsReceived                 : %u\n", callStats.packetsReceived );
            printf( "      BytesSent                       : %u\n", callStats.bytesSent );
            printf( "      BytesReceived                   : %u\n", callStats.bytesReceived );
            printf( "      PacketsLost                     : %u\n", callStats.packetsLost );
            printf( "      IncomingCallsReceived           : %u\n", callStats.incomingCallsReceived );
            printf( "      IncomingCallsAnswered           : %u\n", callStats.incomingCallsAnswered );
            printf( "      IncomingCallsConnected          : %u\n", callStats.incomingCallsConnected );
            printf( "      IncomingCallsFailed             : %u\n", callStats.incomingCallsFailed );
            printf( "      OutgoingCallsAttempted          : %u\n", callStats.outgoingCallsAttempted );
            printf( "      OutgoingCallsAnswered           : %u\n", callStats.outgoingCallsAnswered );
            printf( "      OutgoingCallsConnected          : %u\n", callStats.outgoingCallsConnected );
            printf( "      OutgoingCallsFailed             : %u\n\n", callStats.outgoingCallsFailed );

            /* Clear call stats block for next account */
            memset( &callStats, 0, sizeof(DAL_VOICE_CALL_STATS_BLK) );
         }
      } /* Voice accounts */
   } /* Voice profiles */
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_DumpVlStatus
**
**  PUROPOSE:       Updates and dumps the stats variables in voice line object
**
**  INPUT PARMS:    vpInst    - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
static CmsRet dalVoice_DumpVlStatus(DAL_VOICE_PARMS *parms )
{
   unsigned int        vpInst        = parms->op[0];
   unsigned int        lineInst      = parms->op[1];
   CmsRet              ret;
   InstanceIdStack     iidStack      = EMPTY_INSTANCE_ID_STACK;
   VoiceLineObject     *voiceLineObj = NULL;

   LEVELINFO   info[DAL_VOICE_MAXLEVELS]       = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                         {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                         {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   cmsLog_debug( "line:%d \n",parms->op[1]);

   /*  Get the Voice Profile object. Call getObj with OGF_NORMAL_UPDATE flag to trigger STL function */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&voiceLineObj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice profile object (vpInst = %d, lineInst = %d) \n",
                    vpInst, lineInst );
      return ( ret );
   }

   printf( "      ActivationStatus        : %s\n", voiceLineObj->enable );
   printf( "      VoipServiceStatus       : %s\n", voiceLineObj->status );
   printf( "      CallStatus              : %s\n", voiceLineObj->callState );

   cmsObj_free( (void **) &voiceLineObj );

   return  ( ret );
}

#endif /* SIPLOAD */
/***************************************************************************
* Function Name: dumpCmdParam
* Description  : Displays the command line parameters individually square-bracketed.
*                The parameters are assumed to be NULL-terminated
*
* Parameters   : argBlk (IN) - parameter structure
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
static void dumpCmdParam( DAL_VOICE_PARMS *parms, char *value )
{
#if DALVOICE_DEBUG_CMD_PARMS
   int i;

   printf( "params = " );

   for (i=0; i<DAL_VOICE_MAX_VOIP_ARGS; i++)
   {
      printf( "[%d]", parms->op[i] );
   }

   printf( " %s\n", value );
#endif /* DALVOICE_DEBUG_CMD_PARMS */
}

 /***************************************************************************
* Function Name: dalVoice_mapSpNumToVpInst
* Description  : This returns the Voice Profile instance number corresponding
*                to a certain service provider index.
*
* Parameters   : spNum (IN)     - service provider index
*                vpInst (OUT)   - pointer to VoiceProfile instance number
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_mapSpNumToVpInst ( int spNum, int * vpInst )
{
   return mapSpNumToVpInst( spNum, vpInst );
}

/***************************************************************************
* Function Name: dalVoice_mapVpInstLineInstToCMAcnt
* Description  : This returns the call manager account number corresponding
*                to a certain Voice Profile instance number and line instance
*                number.
*
* Parameters   : vpInst (IN)    - voice profile instance
*                lineInst (IN)  - line instance
*                cmAcnt (OUT)   - pointer to call manager account number
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_mapVpInstLineInstToCMAcnt ( int vpInst, int lineInst, int * cmAcnt )
{
   return mapVpInstLineInstToCMAcnt( vpInst, lineInst, cmAcnt );
}

/***************************************************************************
* Function Name: dalVoice_mapAcntNumToLineInst
* Description  : This returns the Line instance number corresponding
*                to a Voice Profile number and account index.
*
* Parameters   : vpInst (IN)    - VoiceProfile instance number
*                acntNum (IN)   - Account index
*                lineInst (OUT) - pointer to line instance number
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
CmsRet dalVoice_mapAcntNumToLineInst ( int vpInst, int acntNum, int * lineInst )
{
   return(mapAcntNumToLineInst( vpInst, acntNum, lineInst));
}

/***************************************************************************
* Function Name: dalVoice_mapCmLineToVpInstLineInst
* Description  : This returns the Voice Profile instance number and line instance
*                number corresponding to a callmanager line index
*
* Parameters   : cmLine (IN)    - callmanger line index
*                vpInst (OUT)   - pointer to VoiceProfile instance number
*                lineInst (OUT) - pointer to line instance number
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
CmsRet dalVoice_mapCmLineToVpInstLineInst ( int cmLine, int * vpInst, int * lineInst )
{
   return rutVoice_mapCmLineToVpInstLineInst( cmLine, vpInst, lineInst );
}

#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1
/***************************************************************************
* Function Name: dalVoice_mapCmPstnLineToPstnInst
* Description  : This returns the Voice Profile instance number corresponding
*                to a certain service provider index.
*
* Parameters   : cmPstnLineNum (IN)     - pstnLineNumber
*                pstnInst (OUT)         - pointer to mdm pstnInst
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
/* TODO: Function needs to change when multiple vp's are supported          */
/* TODO: Function needs to change when line creatiion/deletion is supported */
CmsRet dalVoice_mapCmPstnLineToPstnInst ( int cmPstnLineNum, int * pstnInst )
{
   *pstnInst = cmPstnLineNum + 1;
   return(CMSRET_SUCCESS);
}
#endif

#ifdef SIPLOAD
/***************************************************************************
* Function Name: dalVoice_isValidVP
* Description  : Checks if Voice Profile is valid
*
* Parameters   : vpInst (IN)  - voice profile number
* Returns      : CMSRET_SUCCESS when valid.
****************************************************************************/
CmsRet dalVoice_isValidVP( int vpInst )
{
   CmsRet              ret;
   InstanceIdStack     iidStack   = EMPTY_INSTANCE_ID_STACK;
   VoiceProfObject    *profObj    = NULL;

   LEVELINFO info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                           {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object */
   if ( (ret = getObj(info, &iidStack, OGF_NO_VALUE_UPDATE, (void**)&profObj)) == CMSRET_SUCCESS )
   {
      cmsObj_free((void **) &profObj);
   }
   return ret;
}

/***************************************************************************
* Function Name: dalVoice_isValidVPVL
* Description  : Checks if Voice Profile & Voice line is valid
*
* Parameters   : vpInst (IN)    - voice profile number
*                lineInst (IN)  - voice line number
* Returns      : CMSRET_SUCCESS when valid.
****************************************************************************/
CmsRet dalVoice_isValidVPVL( int vpInst, int lineInst )
{
   CmsRet              ret;
   InstanceIdStack     iidStack      = EMPTY_INSTANCE_ID_STACK;
   VoiceLineObject     *voiceLineObj = NULL;

   LEVELINFO info[DAL_VOICE_MAXLEVELS] = { {MDMOID_VOICE_PROF, vpInst, EMPTY_INSTANCE_ID_STACK},
                                           {MDMOID_VOICE_LINE, lineInst, EMPTY_INSTANCE_ID_STACK},
                                           {0, 0, EMPTY_INSTANCE_ID_STACK}, };

   /*  Get the Voice Profile object. Call getObj with OGF_NORMAL_UPDATE flag to trigger STL function */
   if ( (ret = getObj(info, &iidStack, OGF_NORMAL_UPDATE, (void**)&voiceLineObj)) == CMSRET_SUCCESS )
   {
      cmsObj_free( (void **) &voiceLineObj );
   }

   return  ( ret );
}
#endif /* SIPLOAD */

/***************************************************************************
* Function Name: validateCodec
* Description  : Checks if codec is valid/supported
*
* Parameters   : codec (IN)    - codec
*                found (OUT)   - true indicates codec is supported
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
static CmsRet validateCodec( char * codec, UBOOL8 * found )
{
   /* Call associated RUT function */
   return (rutVoice_validateCodec( codec, found ));
}

/***************************************************************************
* Function Name: validateAlpha2Locale
* Description  : Checks if country is valid Alpha-2 locale
*
* Parameters   : country (IN)    - country
*                found (OUT)     - true indicates country is valid Alpha-2
*                                  locale
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
static CmsRet validateAlpha2Locale( char * country, UBOOL8 * found )
{
   /* Call associated RUT function */
   return (rutVoice_validateAlpha2Locale( country, found ));
}

/***************************************************************************
* Function Name: dumpSupportedCodecList
* Description  : Dumps list of supported codecs
*
* Parameters   : None
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
static CmsRet dumpSupportedCodecList( void )
{
   char * tokResult, * pPrefix;
   char delim[2] = ",";
   char codecList[LIST_CHAR_BUFFER];
   char tempBuf[TEMP_CHARBUF_SIZE];

   rutVoice_getSupportedCodecs( codecList, LIST_CHAR_BUFFER );

   printf( "Supported codecs:\n");

   /* parses first token */
   tokResult = strtok( codecList, delim);
   while ( tokResult != NULL)
   {
      /* Check for X_BROADCOM_COM_ prefix, remove, print Codec */
      if( (pPrefix = strstr( tokResult, "X_BROADCOM_COM_" )) != NULL )
      {
         strncpy( tempBuf, (pPrefix + strlen( "X_BROADCOM_COM_" )), TEMP_CHARBUF_SIZE );

         /* print out the supported codec, reset buffer */
         printf( "%s\n", tempBuf);
         memset( tempBuf, 0, TEMP_CHARBUF_SIZE );
      }
      else
      {
         /* print out the supported codec */
         printf( "%s\n", tokResult);
      }
      /* Get next token */
      tokResult = strtok( NULL, delim );
   }
   return(CMSRET_SUCCESS);
}

/***************************************************************************
* Function Name: dumpSupportedLocalesList
* Description  : dumps list of supported locales
*
* Parameters   : None
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
static CmsRet dumpSupportedLocalesList( void )
{
   char localeList[LIST_CHAR_BUFFER];
   rutVoice_getSupportedLocales( localeList, LIST_CHAR_BUFFER );
   printf( "Supported locales: Alpha3/Alpha2 - Country Name\n\n%s\n\n",localeList);
   return(CMSRET_SUCCESS);
}

/***************************************************************************
* Function Name: dalVoice_mapCountryCode3To2
* Description  : Maps Alpha-3 locale to Alpha-2. Also checks if locale is valid
*
* Parameters   : country (INOUT) - locale(Alpha3), on success exec contains Alpha2 locale
*                found (OUT)   - true indicates locale is supported
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
CmsRet dalVoice_mapCountryCode3To2 ( char * country, UBOOL8 * found, unsigned int length )
{
   return (rutVoice_mapAlpha3toAlpha2Locale ( country, found, length ));
}

/***************************************************************************
* Function Name: mapSpNumToVpInst
* Description  : This returns the Voice Profile instance number corresponding
*                to a certain service provider index.
*
* Parameters   : spNum (IN)     - service provider index
*                vpInst (OUT)   - pointer to VoiceProfile instance number
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
static CmsRet mapSpNumToVpInst ( int spNum, int * vpInst )
{
   return rutVoice_mapSpNumToVpInst( spNum, vpInst );
}

/***************************************************************************
* Function Name: mapAcntNumToLineInst
* Description  : This returns the Line instance number corresponding
*                to a Voice Profile number and account index.
*
* Parameters   : vpInst (IN)    - VoiceProfile instance number
*                acntNum (IN)   - Account index
*                lineInst (OUT) - pointer to line instance number
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
static CmsRet mapAcntNumToLineInst ( int vpInst, int acntNum, int * lineInst )
{
   return (rutVoice_mapAcntNumToLineInst( vpInst, acntNum, lineInst ));
}

/***************************************************************************
* Function Name: mapVpInstLineInstToCMAcnt
* Description  : This returns the Line instance number corresponding
*                to a Voice Profile number and account index.
*
* Parameters   : vpInst (IN)    - VoiceProfile instance number
*                lineInst (IN)  - Line instance number
*                cmAcntNum (OUT)- pointer to call manager account number
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet mapVpInstLineInstToCMAcnt( int vpInst, int lineInst, int * cmAcntNum )
{
   return (rutVoice_mapVpInstLineInstToCMAcnt( vpInst, lineInst, cmAcntNum ));
}

/***************************************************************************
** Function Name: dalVoice_mapAlpha2toVrg
** Description  : Given an alpha2 country string returns a VRG country code
**
** Parameters   : locale (IN) - callmanger line index
**                id (OUT)    - VRG country code
**                found (OUT) - Flag that indicates if code is found
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_mapAlpha2toVrg( char *locale, int *id, UBOOL8 *found, unsigned int length )
{
   return(rutVoice_mapAlpha2toVrg( locale, id, found, length ));
}

/***************************************************************************
** Function Name: mapAlpha2toAlpha3
** Description  : Given an alpha2 country string returns a alpha3 string
**
** Parameters   : locale (IN) - callmanger line index
**                id (OUT)    - VRG country code
**                found (OUT) - Flag that indicates if code is found
** Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet mapAlpha2toAlpha3( char *locale, char *alpha3, UBOOL8 *found, unsigned int length )
{
   return(rutVoice_mapAlpha2toAlpha3( locale, alpha3, found, length ));
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
         if ( info[i].instId == -1 || (PEEK_INSTANCE_ID(&info[i].iidStack)) == info[i].instId )
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




#if VOICE_IPV6_SUPPORT
/*****************************************************************
**  FUNCTION:       dal_Voice_StripIpv6PrefixLength
**
**  PUROPOSE:       Helper function to strip IPv6 prefix lenght
**                  from an IPv6 address.
**
**  INPUT PARMS:    voiceObj  - MDM voice object
**                  ipAddress - IPv6 address to be stripped
**
**  OUTPUT PARMS:   Stripped IPv6 address
**
**  RETURNS:        CMSRET_SUCCESS - Success
**
**  NOTE:           This function assumes that is it invoked after
**                  taking the CMS lock.
*******************************************************************/
static CmsRet dal_Voice_StripIpv6PrefixLength(VoiceObject *voiceObj, char *ipAddress)
{
   VoiceObject *voiceObject = NULL;
   UBOOL8 objLocallyAllocated = 0;

   /* Sanity check */
   if ( ipAddress == NULL )
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   /* If no voiceObject passed, look it up from MDM */
   if ( voiceObj == NULL )
   {
      CmsRet ret;
      InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

      if ( (ret = cmsObj_getNext(MDMOID_VOICE, &iidStack, (void **) &voiceObject)) != CMSRET_SUCCESS)
      {
         cmsLog_error( "could not get voice object, ret=%d", ret);
         return ret;
      }

      objLocallyAllocated = 1;
   }
   else
   {
      voiceObject = voiceObj;
   }

   if ( voiceObject->X_BROADCOM_COM_IpAddressFamily != NULL  &&
        !strncmp(voiceObject->X_BROADCOM_COM_IpAddressFamily, MDMVS_IPV6, strlen(voiceObject->X_BROADCOM_COM_IpAddressFamily)) )
   {
      char *tempAt;

      tempAt = strchr( ipAddress, '/' );
      if ( tempAt != NULL )
      {
         *tempAt = '\0';
      }
   }

   /* Free voice object if locally allocated */
   if ( objLocallyAllocated )
   {
      cmsObj_free((void**) &voiceObject);
   }

   return CMSRET_SUCCESS;
}
#endif /* VOICE_IPV6_SUPPORT */

#endif /* DMP_VOICE_SERVICE_1 */
/*<END>================================= Common Helper Functions =========================================<END>*/

#ifdef DMP_BASELINE_1
/*
** Macro used to conveniently update the fields of exception object. This macro
** has been created because IP WAN and PPP WAN have seperate firewall exception
** objects but those objects have the exact same fields.
*/
#define SET_EXPCEPTION_OBJ_FIELDS(obj, fwCtlBlk) \
   obj->enable = fwCtlBlk->enable; \
   cmsMem_free(obj->filterName); \
   obj->filterName = cmsMem_strdup(fwCtlBlk->filterName); \
   cmsMem_free(obj->protocol); \
   obj->protocol = cmsMem_strdup(fwCtlBlk->protocol); \
   obj->sourcePortStart = obj->sourcePortEnd = fwCtlBlk->sourcePort; \
   obj->destinationPortStart = obj->destinationPortEnd = fwCtlBlk->destinationPort; \
   cmsMem_free(obj->IPVersion); \
   obj->IPVersion = cmsMem_strdup( (strchr(fwCtlBlk->sourceIPAddress, ':') == NULL) ? MDMVS_4 : MDMVS_6 ); \
   cmsMem_free(obj->sourceIPAddress); \
   obj->sourceIPAddress = cmsMem_strdup( (!strncmp(fwCtlBlk->sourceIPAddress, ZERO_ADDRESS_IPV4, sizeof(ZERO_ADDRESS_IPV4)) || \
                                          !strncmp(fwCtlBlk->sourceIPAddress, ZERO_ADDRESS_IPV6, 2) \
                                          ? "" : fwCtlBlk->sourceIPAddress) ); \
   cmsMem_free(obj->sourceNetMask); \
   obj->sourceNetMask = cmsMem_strdup( (!strncmp(fwCtlBlk->sourceNetMask, ZERO_ADDRESS_IPV4, sizeof(ZERO_ADDRESS_IPV4)) || \
                                          !strncmp(fwCtlBlk->sourceNetMask, ZERO_ADDRESS_IPV6, 2) \
                                          ? "" : fwCtlBlk->sourceNetMask) ); \
   cmsMem_free(obj->destinationIPAddress); \
   obj->destinationIPAddress = cmsMem_strdup( (!strncmp(fwCtlBlk->destinationIPAddress, ZERO_ADDRESS_IPV4, sizeof(ZERO_ADDRESS_IPV4)) || \
                                               !strncmp(fwCtlBlk->destinationIPAddress, ZERO_ADDRESS_IPV6, 2) \
                                               ? "" : fwCtlBlk->destinationIPAddress) ); \
   cmsMem_free(obj->destinationNetMask); \
   obj->destinationNetMask = cmsMem_strdup( (!strncmp(fwCtlBlk->destinationNetMask, ZERO_ADDRESS_IPV4, sizeof(ZERO_ADDRESS_IPV4)) || \
                                               !strncmp(fwCtlBlk->destinationNetMask, ZERO_ADDRESS_IPV6, 2) \
                                               ? "" : fwCtlBlk->destinationNetMask) )


/****************************************************************************
* FUNCTION: performFilterOperation
*
* PURPOSE:  Adds or deletes a filter to the firewall based on the value of the
*           enabled field in the firewall control block.
*
* PARAMETERS: iidStack - Instance ID stack of WAN object
*             fwCtlBlk - Firewall control block
*             oid      - ID of the exception object to be added/deleted
*             obj      - Address of the prointer the object. This field
*                        is updated when a new object is added to MDM
*
* RETURNS: CMSRET_SUCCESS when successful.
****************************************************************************/
static CmsRet performFilterOperation(InstanceIdStack *iidStack, DAL_VOICE_FIREWALL_CTL_BLK *fwCtlBlk, MdmObjectId oid, void **obj)
{
   CmsRet ret;
   InstanceIdStack searchIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack lastIidStack = EMPTY_INSTANCE_ID_STACK;

   /* Enable a rule means add a filter to the firewall */
   if ( fwCtlBlk->enable )
   {
      if ( (ret = cmsObj_addInstance(oid, iidStack)) != CMSRET_SUCCESS )
      {
         cmsLog_error("could not add Exception obj, ret=%d", ret);
         return ret;
      }

      ret = cmsObj_get(oid, iidStack, 0, obj);
   }
   /* Find the filter and remove it from the firewall */
   else
   {
      while ( (ret = cmsObj_getNextInSubTree(oid, iidStack, &searchIidStack, obj )) == CMSRET_SUCCESS )
      {
         /* Object is identified based on the filter name.
          * Compare FirewallExceptionObject filterNames only up to the length of cwCtlBlk's filterName.
          *  This is because in the case of the DELETE ALL operation, the substring of "VoiceFilter"
          *  is used for comparsion. If the object's filter name contains "VoiceFilter", this
          *  indicates that the filter should be deleted.
          */
         if( !strncmp((oid == MDMOID_WAN_IP_CONN_FIREWALL_EXCEPTION)
                         ? (*((WanIpConnFirewallExceptionObject**)obj))->filterName
                         : (*((WanPppConnFirewallExceptionObject**)obj))->filterName,
                      fwCtlBlk->filterName,
                      strlen(fwCtlBlk->filterName)) )
         {
            cmsObj_deleteInstance(oid, &searchIidStack);
            searchIidStack = lastIidStack;
         }
         else
         {
            lastIidStack = searchIidStack;
         }

         cmsObj_free((void**)obj);
      }
   }

   return (ret == CMSRET_NO_MORE_INSTANCES) ? CMSRET_SUCCESS : ret;
}

/****************************************************************************
* FUNCTION: dalVoice_performFilterOperation
*
* PURPOSE:  Adds or deletes a filter to the firewall based on the WAN interface
*           used for voice.
*
* PARAMETERS: parms    - Not used
*             fwCtlBlk - Firewall control block. Contains information about the
*                        filter.
*
* RETURNS: CMSRET_SUCCESS - Success
*          other failed, check with reason code
*
* NOTE:   This function is both a get and set function and if relies on the
*       boundIfName object to determine the WAN object to which the filter
*       should be added or deleted.
*
****************************************************************************/
CmsRet dalVoice_performFilterOperation_igd( DAL_VOICE_PARMS *parms __attribute__((unused)),
                                       DAL_VOICE_FIREWALL_CTL_BLK *fwCtlBlk )
{
   DAL_VOICE_PARMS parms1;
   CmsRet ret;
   char ifName[MAX_TR104_OBJ_SIZE];
   InstanceIdStack iidStack;
   WanIpConnObject  *ipConn = NULL;
   WanPppConnObject *pppConn = NULL;
   WanIpConnFirewallExceptionObject *ipExpObj = NULL;
   WanPppConnFirewallExceptionObject *pppExpObj = NULL;

   /* Get voice interface name */
   parms1.op[0 ]= parms1.op[1] = 1;
   if ( (ret = dalVoice_GetBoundIfName( &parms1, ifName, MAX_TR104_OBJ_SIZE )) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to get interface name");
      return ret;
   }

   /* Check to see if we are in LAN, if so, no need to proceed */
   if( !(cmsUtl_strcmp( ifName, MDMVS_LAN )) )
   {
      cmsLog_debug( "Interface is LAN. No need for filter operations\n" );
      return CMSRET_SUCCESS;
   }

   /* Check for IP WAN interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( (ret = cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void**)&ipConn )) == CMSRET_SUCCESS )
   {
      if( ipConn->X_BROADCOM_COM_IfName != NULL &&
          !(cmsUtl_strcmp(ipConn->connectionType, MDMVS_IP_ROUTED)) &&
          (!strncmp(ipConn->X_BROADCOM_COM_IfName, ifName, strlen(ifName)) ||
           !strncmp(ifName, MDMVS_ANY_WAN, strlen(ifName))) )
      {
         cmsObj_free((void**)&ipConn);
         if ( (ret = performFilterOperation(&iidStack, fwCtlBlk,
                        MDMOID_WAN_IP_CONN_FIREWALL_EXCEPTION, (void**)&ipExpObj)) != CMSRET_SUCCESS )
         {
            cmsLog_error( "Can't perform firewall operation = %d\n", ret );
            return ret;
         }

         /* Update object only if a filter is being added */
         if ( fwCtlBlk->enable )
         {
            /* Populate firewall exception object and update MDM */
            SET_EXPCEPTION_OBJ_FIELDS(ipExpObj, fwCtlBlk);
            if ( ( ret = cmsObj_set(ipExpObj, &iidStack)) != CMSRET_SUCCESS )
            {
               /* Setting values failed, must delete previously created ipExpObj */
               cmsObj_deleteInstance(MDMOID_WAN_IP_CONN_FIREWALL_EXCEPTION, &iidStack);
               cmsLog_error( "Can't set IP firewall exception object = %d\n", ret );
            }
         }

         cmsObj_free((void**)&ipExpObj);
         return ret;
      }

      cmsObj_free((void**)&ipConn);
   }

   if( ret != CMSRET_NO_MORE_INSTANCES )
   {
      cmsLog_error("Failed to get IP WAN Interface object");
      return ret;
   }

   /* Check for PPP WAN interfaces */
   INIT_INSTANCE_ID_STACK(&iidStack);
   while ( (ret = cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void**)&pppConn )) == CMSRET_SUCCESS )
   {
      if(pppConn->X_BROADCOM_COM_IfName != NULL && !(cmsUtl_strcmp(pppConn->connectionType, MDMVS_IP_ROUTED)) &&
         (!strncmp(pppConn->X_BROADCOM_COM_IfName, ifName, strlen(pppConn->X_BROADCOM_COM_IfName)) ||
          !strncmp(ifName, MDMVS_ANY_WAN, strlen(ifName))) )
      {
         cmsObj_free((void**)&pppConn);
         if ( (ret = performFilterOperation(&iidStack, fwCtlBlk,
                        MDMOID_WAN_PPP_CONN_FIREWALL_EXCEPTION, (void**)&pppExpObj)) != CMSRET_SUCCESS )
         {
            cmsLog_error( "Can't perform firewall operation = %d\n", ret );
            return ret;
         }

         /* Update object only if a filter is being added */
         if ( fwCtlBlk->enable )
         {
            /* Populate firewall exception object and update MDM */
            SET_EXPCEPTION_OBJ_FIELDS(pppExpObj, fwCtlBlk);
            if ( ( ret = cmsObj_set(pppExpObj,  &iidStack)) != CMSRET_SUCCESS )
            {
               /* Setting values failed, must delete previously created pppExpObj */
               cmsObj_deleteInstance(MDMOID_WAN_PPP_CONN_FIREWALL_EXCEPTION, &iidStack);
               cmsLog_error( "Can't set PPP firewall exception object = %d\n", ret );
            }
         }

         cmsObj_free((void**)&pppExpObj);
         return ret;
      }

      cmsObj_free((void**)&pppConn);
   }

   if( ret != CMSRET_NO_MORE_INSTANCES )
   {
      cmsLog_error("Failed to get PPP WAN Interface object");
      return ret;
   }
   return ( ret );
}
#endif  /* DMP_BASELINE_1 */
#endif /* BRCM_VOICE_SUPPORT */
