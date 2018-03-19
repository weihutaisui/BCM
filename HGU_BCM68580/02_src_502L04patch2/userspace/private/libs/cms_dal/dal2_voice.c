/*
* <:copyright-BRCM:2016:proprietary:standard
*
*    Copyright (c) 2016 Broadcom
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
:>
*/


#ifdef BRCM_VOICE_SUPPORT
#include <sched.h>
#include <pthread.h>

#include <mdm.h>
#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "cms_qdm.h"
#include "cms_qos.h"
#include "cms_net.h"
#include "cms_msg.h"
#include "dal.h"
#include "dal_voice.h"
#ifdef DMP_VOICE_SERVICE_2
#include "rut2_voice.h"

/* Debug: Turn CMD parms dump on/off */
#define DALVOICE_DEBUG_CMD_PARMS    0

/*============================= TODOS =========================*/
/* TODO: Functions needs to change when multiple vp's are supported          */
/* TODO: Functions needs to change when line creatiion/deletion is supported */
/*============================= TODOS =========================*/

/* ---- Public Variables ------------------------------------------------- */
/* ---- Constants and Types ---------------------------------------------- */

#define ANY_INSTANCE             (-1)
#define TEMP_CHARBUF_SIZE        64
#define REGION_A3_CHARBUF_SIZE   4
#define LIST_CHAR_BUFFER         2048
#define MAX_NUM_CODECS           64
#define FULL_PATH_SIZE           256

#define ZERO_ADDRESS_IPV4 "0.0.0.0"
#define ZERO_ADDRESS_IPV6 "::"

const char *defaultCodecList[] = {"G.711MuLaw", "G.711ALaw", "G.729a", "G.726", "AMR" };


const char *ext_status_valid_string[] = { MDMVS_UP, MDMVS_ERROR, MDMVS_DISABLED, 0};
const char *ext_call_status_valid_string[] = { MDMVS_IDLE, MDMVS_DIALING, MDMVS_CONNECTED, MDMVS_ALERTING, MDMVS_DISCONNECTED, 0};
const char *ext_conf_status_valid_string[] = { MDMVS_IDLE, MDMVS_DISABLED, MDMVS_INCONFERENCECALL, 0};
const char *ext_cw_status_valid_string[] = { MDMVS_IDLE, MDMVS_DISABLED, MDMVS_SECONDARYCONNECTED,  0};
const char *loglevel_valid_string[] = { MDMVS_NOTICE, MDMVS_DEBUG, MDMVS_ERROR, 0};
const char *protocol_valid_string[] = { MDMVS_TR69, MDMVS_OAM, MDMVS_OMCI, 0};
const char *DTMFMethod_valid_string[] = { MDMVS_INBAND, MDMVS_SIPINFO, MDMVS_RFC4733, 0};
const char *sip_transport_valid_string[] = { MDMVS_UDP, MDMVS_TCP, MDMVS_TLS, MDMVS_SCTP, 0};
const char *cctk_trace_level_valid_string[] = { MDMVS_DEBUG, MDMVS_INFO, MDMVS_WARN, MDMVS_OFF };
const char *sip_conf_option_valid_string[] = {  MDMVS_LOCAL,
#ifdef VOICE_IMS_SUPPORT
                                                MDMVS_REFERPARTICIPANTS,
                                                MDMVS_REFERSERVER,
#endif
                                                0
                                             };
const char *srtp_option_valid_string[] = {  MDMVS_MANDATORY, MDMVS_OPTIONAL, MDMVS_DISABLED };


/* Minimum and maximum values for TX/RX gains */
#define MINGAIN -20
#define MAXGAIN  20

/* Gain is set in 0.1dB units */
#define GAINUNIT 10

static CmsRet isValidString( char *input, const char **validStr );
static CmsRet convertStringToBool( const char *input, UBOOL8 *value );
static CmsRet getObject( MdmObjectId oid, int L1Idx, int L2Idx, int L3Idx, int L4Idx, UINT32 flags, InstanceIdStack *outStack, void **obj);
static CmsRet addL2Object( MdmObjectId oid, int L1_inst, int *L2_inst );
static CmsRet delL2Object( MdmObjectId oid, int L1_inst, int L2_inst );
static CmsRet mapL2ObjectNumToInst ( MdmObjectId oid, int L1_Inst, int Num, int *L2_Inst );
static CmsRet mapL2ObjectInstToNum ( MdmObjectId oid, int L1_Inst, int L2_Inst, int *Num );
static CmsRet getL2ToL2ObjAssoc( MdmObjectId srcOid, MdmObjectId destOid, int svcInst, int srcInst, int *destInst );
static CmsRet setL2ToL2ObjAssoc( MdmObjectId srcOid, MdmObjectId destOid, int svcInst, int srcInst, int destInst );

static CmsRet addL3Object( MdmObjectId oid, int L1_inst, int L2_inst, int *L3_inst );
static CmsRet delL3Object( MdmObjectId oid, int L1_inst, int L2_inst, int L3_inst );
static CmsRet mapL3ObjectNumToInst ( MdmObjectId oid, int L1_Inst, int L2_Inst, int Num, int *L3_Inst );
#if 0
static CmsRet mapL3ObjectInstToNum ( MdmObjectId oid, int L1_Inst, int L2_Inst, int L3_Inst, int *Num );
#endif

#if VOICE_IPV6_SUPPORT
static CmsRet stripIpv6PrefixLength(VoiceObject *voiceObj, char *ipAddress);
#endif /* VOICE_IPV6_SUPPORT */

/****************************************************/
/* BASE GET/SET macros.  Should use this everywhere */
/****************************************************/

/* Takes value as an INTEGER */
#define SET_L4OBJ_PARAM_UINT(i1, i2, i3, i4, n, v, f)   \
{                                                       \
    UINT32   __input = v;                               \
    UINT32   __max_range = f;                           \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;\
    if( __max_range > 0 && __input > __max_range ){     \
        cmsLog_error( "%s, value %u is out of range of %u\n", __FUNCTION__, __input, __max_range );\
        return CMSRET_INVALID_PARAM_VALUE;              \
    }                                                   \
    ret = getObject( __oid, i1, i2, i3, i4, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj );\
    if( CMSRET_SUCCESS == ret ){                        \
        obj->n = __input;                               \
        ret = cmsObj_set((const void *)obj, &iidStack); \
        cmsObj_free((void **)&obj);                     \
        if( CMSRET_SUCCESS != ret )                     \
            cmsLog_error( "%s() failed to get object (%d)\n", __FUNCTION__, __oid);\
    }                                                   \
    else{ cmsLog_error( "%s() failed to get object (%d) \n",__FUNCTION__, __oid);}\
}

/* Takes value as an UBOOL8 */
#define SET_L4OBJ_PARAM_BOOL(i1, i2, i3, i4, n, v)   \
{                                                       \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;\
    ret = getObject( __oid, i1, i2, i3, i4, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj );\
    if( CMSRET_SUCCESS == ret ){                        \
        obj->n = v?TRUE:FALSE;                               \
        ret = cmsObj_set((const void *)obj, &iidStack); \
        cmsObj_free((void **)&obj);                     \
        if( CMSRET_SUCCESS != ret )                     \
            cmsLog_error( "%s() failed to get object (%d)\n", __FUNCTION__, __oid);\
    }                                                   \
    else{ cmsLog_error( "%s() failed to get object (%d) \n",__FUNCTION__, __oid);}\
}

/* Returns value as a INTEGER */
#define GET_L4OBJ_PARAM_UINT(i1, i2, i3, i4, n, v) \
{                                     \
    ret = getObject( __oid, i1, i2, i3, i4, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj );\
    if( CMSRET_SUCCESS == ret ){      \
        *v = obj->n;                  \
        cmsObj_free((void **)&obj);   \
    }                                 \
    else{ cmsLog_error( "%s() failed to get object (%d) parameter\n",__FUNCTION__, __oid);}\
}

/* Returns value as a UBOOL8 */
#define GET_L4OBJ_PARAM_BOOL(i1, i2, i3, i4, n, v) \
{                                     \
    ret = getObject( __oid, i1, i2, i3, i4, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj );\
    if( CMSRET_SUCCESS == ret ){      \
        *v = obj->n;                  \
        cmsObj_free((void **)&obj);   \
    }                                 \
    else{ cmsLog_error( "%s() failed to get object (%d) parameter\n",__FUNCTION__, __oid);}\
}

/****************************************************/
/* End                                              */
/****************************************************/


#define SET_L2OBJ_PARAM_STR(i, p, n, v, f)              \
{                                                       \
    char __buf[MAX_TR104_OBJ_SIZE];                     \
    const char **__valid_str = f;                       \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;\
    if( v != NULL ){                                    \
        memset(__buf, 0, sizeof(__buf));                \
        strncpy(__buf, v, strlen(v));                   \
    }                                                   \
    if( v != NULL && __valid_str != NULL){              \
        /* validation check */                          \
        ret = isValidString(__buf, __valid_str );       \
    }                                                   \
    else{                                               \
        ret = CMSRET_SUCCESS;                           \
    }                                                   \
    if( ret == CMSRET_SUCCESS ){                        \
        ret = getObject( __oid, i, p, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj ); \
        if( CMSRET_SUCCESS == ret ){                    \
            if( v == NULL )                             \
            {                                           \
                cmsMem_free(obj->n);                    \
            }                                           \
            else{                                       \
                REPLACE_STRING_IF_NOT_EQUAL(obj->n, __buf); \
            }                                           \
            ret = cmsObj_set((const void *)obj, &iidStack); \
            cmsObj_free((void **)&obj);                 \
            if( CMSRET_SUCCESS != ret )                 \
                cmsLog_error( "failed to set L2 object (%d)\n", __oid);   \
        }                                               \
        else{                                           \
            cmsLog_error( "failed to get required L2 object (%d)\n", __oid);   \
        }                                               \
    }                                                   \
    else                                                \
    {                                               \
        cmsLog_error("%d() invalid argument value %s\n", __FUNCTION__, __buf);\
    }                                               \
}

/* Takes value as an STRING */
#define SET_L2OBJ_PARAM_BOOL(i, p, n, v)                \
{                                                       \
    const char *__buf = (const char *)v;                \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;\
    UBOOL8  setvalue = FALSE;                           \
    if( __buf != NULL ){                                \
        ret = convertStringToBool( __buf, &setvalue);   \
    }                                                   \
    else{\
        ret = CMSRET_INVALID_ARGUMENTS; \
    }\
    if( ret == CMSRET_SUCCESS ){ \
        ret = getObject( __oid, i, p, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj ); \
        if( CMSRET_SUCCESS == ret ){                        \
            obj->n = setvalue;                              \
            ret = cmsObj_set((const void *)obj, &iidStack); \
            cmsObj_free((void **)&obj);                     \
            if( CMSRET_SUCCESS != ret )                     \
                cmsLog_error( "%s() failed to set object (%d)\n", __FUNCTION__, __oid);   \
        }                                                   \
        else{                                               \
            cmsLog_error( "%s() failed to get object(%d) at (%d)\n", __FUNCTION__, __oid, p);   \
        }                                                   \
    }\
    else                     \
    {                                               \
        cmsLog_error("%s() invalid argument value %s\n", __FUNCTION__, __buf);\
    }                                               \
}

/* Takes value as an INTEGER */
#define SET_L2OBJ_PARAM_UINT(i, p, n, v, f)             \
{                                                       \
    UINT32   __input = v;                               \
    UINT32   __max_range = f;                           \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;\
    if( __max_range > 0 && __input > __max_range ){     \
        cmsLog_error( "%s, value %u is out of range of %u\n", __FUNCTION__, __input, __max_range );\
        return CMSRET_INVALID_PARAM_VALUE;              \
    }                                                   \
    ret = getObject( __oid, i, p, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj );\
    if( CMSRET_SUCCESS == ret ){                        \
        obj->n = __input;                               \
        ret = cmsObj_set((const void *)obj, &iidStack); \
        cmsObj_free((void **)&obj);                     \
        if( CMSRET_SUCCESS != ret )                     \
            cmsLog_error( "%s() failed to get object (%d)\n", __FUNCTION__, __oid);\
    }                                                   \
    else{ cmsLog_error( "%s() failed to get object (%d) \n",__FUNCTION__, __oid);}\
}

/* Takes value as an INTEGER */
#define SET_L2OBJ_PARAM_SINT(i, p, n, v, f)             \
{                                                       \
    SINT32   __input = v;                               \
    SINT32   __max_range = f;                           \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;\
    if( __input > __max_range ){     \
        cmsLog_error( "%s, value %d is out of range of %d\n", __FUNCTION__, __input, __max_range );\
        return CMSRET_INVALID_PARAM_VALUE;              \
    }                                                   \
    ret = getObject( __oid, i, p, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj );\
    if( CMSRET_SUCCESS == ret ){                        \
        obj->n = __input;                               \
        ret = cmsObj_set((const void *)obj, &iidStack); \
        cmsObj_free((void **)&obj);                     \
        if( CMSRET_SUCCESS != ret )                     \
            cmsLog_error( "%s() failed to set object (%d)\n", __FUNCTION__, __oid);\
    }                                                   \
    else{ cmsLog_error( "%s() failed to get object (%d) \n",__FUNCTION__, __oid);}\
}

/* Returns value as a STRING */
#define GET_L2OBJ_PARAM_UINT(i, p, n, v, l) \
{                                     \
    ret = getObject( __oid, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj );\
    if( CMSRET_SUCCESS == ret ){      \
        snprintf(v, l, "%u", obj->n); \
        cmsObj_free((void **)&obj);   \
    }                                 \
    else{ cmsLog_error( "%s() failed to get object (%d) parameter\n",__FUNCTION__, __oid);}\
}

/* Returns value as a STRING */
#define GET_L2OBJ_PARAM_BOOL(i, p, n, v, l) \
{                                           \
    memset( v, 0, l );                      \
    ret = getObject( __oid, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){            \
        snprintf(v, l, "%s", obj->n?MDMVS_YES:MDMVS_NO); \
        cmsObj_free((void **)&obj);         \
    }                                       \
    else{                                   \
        snprintf(v, l, "%s", MDMVS_NO );           \
        cmsLog_error( "%s() failed to retrieve object (%d)\n", __FUNCTION__, __oid);   \
    }                                       \
}

#define GET_L2OBJ_PARAM_STR(i, p, n, v, l) \
{                                          \
    memset((void *)v, 0, l );              \
    ret = getObject( __oid, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ) {          \
        if( obj->n && strlen(obj->n)>0 ){snprintf(v, l, "%s", obj->n);} \
        cmsObj_free((void **)&obj);        \
    } \
    else{ \
        cmsLog_error( "%s() failed to retrieve object (%d)\n", __FUNCTION__, __oid);   \
    } \
}

#define SET_L1OBJ_PARAM_STR(i, n, v, f)     SET_L2OBJ_PARAM_STR(i, 0, n, v, f)

#define GET_L1OBJ_PARAM_STR(i, n, v, l)     GET_L2OBJ_PARAM_STR(i, 0, n, v, l)

#define GET_L1OBJ_PARAM_UINT(i, n, v) \
{                                     \
    ret = getObject( __oid, i, 0, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj );\
    if( CMSRET_SUCCESS == ret ){      \
        *v = obj->n;                  \
        cmsObj_free((void **)&obj);   \
    }                                 \
    else{ cmsLog_error( "%s() failed to get object (%d) parameter\n",__FUNCTION__, __oid);}\
}

#define GET_VOICE_SVC_PARAM_STR(i, n, v, l, f)      \
{                                                   \
   VoiceObject *obj=NULL;                           \
   memset(v, 0, l);                                 \
   ret = getObject( MDMOID_VOICE, i, 0, 0, 0, 0, NULL, (void **)&obj); \
   if ( ret ==  CMSRET_SUCCESS){                    \
      if ( obj->n && strlen(obj->n) > 0 ){          \
         snprintf( v, l, "%s", obj->n );    \
      }                                     \
      else {                                \
         cmsLog_error( "%s() voice object %s is null", __FUNCTION__, #n);   \
      }                                     \
      if( f == TRUE){                       \
         stripIpv6PrefixLength(obj, v);     \
      }                                     \
      cmsObj_free((void **)&obj);           \
  }                                         \
}

#define GET_VOICE_CAP_PARAM_STR(i, p, n, v, l) \
    VoiceCapObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_CAP, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        snprintf(v, l, "%s", obj->n );            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        snprintf(v, l, "null" );            \
        cmsLog_error( "failed to retrieve Capabilities object\n");   \
    }

#define GET_VOICE_CAP_PARAM_UINT(i, n, v) \
{ \
    VoiceCapObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_CAP, i, 0, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        *v = obj->n;            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        *v = 0;            \
        cmsLog_error( "failed to retrieve Capabilities object\n");   \
    }\
}

#define GET_VOICE_CAP_PARAM_INT_AS_STR(i, p, n, v, l) \
    VoiceCapObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_CAP, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        snprintf(v, l, "%d", obj->n );            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        *v = 0;            \
        cmsLog_error( "failed to retrieve Capabilities object\n");   \
    }

#define SET_VOICE_CAP_PARAM_INT(i, p, n, v, f) \
{ \
    VoiceCapObject *obj=NULL;                 \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK; \
    if( f > 0 && v > f ){ cmsLog_error( "%s, value %u is out of range of %u\n", __FUNCTION__, i, f ); return CMSRET_INVALID_PARAM_VALUE;} \
    ret = getObject( MDMOID_VOICE_CAP, i, p, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        obj->n = v;            \
        ret = cmsObj_set((const void *)obj, &iidStack); \
        cmsObj_free((void **)&obj);                      \
        if( CMSRET_SUCCESS != ret )                     \
            cmsLog_error( "failed to get SipNetwork object\n");   \
    } \
    else{ cmsLog_error( "failed to get SipNetwork object\n");} \
}

#define GET_SIP_CLIENT_CAP_PARAM_STR(i, p, n, v, l) \
    VoiceCapSipClientObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_CAP_SIP_CLIENT, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        snprintf(v, l, "%s", obj->n );            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        snprintf(v, l, "null" );            \
        cmsLog_error( "failed to retrieve Capabilities object\n");   \
    }

#define GET_POTS_CAP_PARAM_BOOL(i, p, n, v) \
    VoiceCapPotsObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_CAP_POTS, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        *v = obj->n;            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        *v = 0;            \
        cmsLog_error( "failed to retrieve Capabilities object\n");   \
    }

#define GET_POTS_CAP_PARAM_STR(i, p, n, v, l) \
    VoiceCapPotsObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_CAP_POTS, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        snprintf(v, l, "%s", obj->n );            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        snprintf(v, l, "null" );            \
        cmsLog_error( "failed to retrieve Capabilities object\n");   \
    }

#define GET_QI_CAP_PARAM_INT(i, p, n, v) \
    QualityIndicatorObject *obj=NULL;                 \
    ret = getObject( MDMOID_QUALITY_INDICATOR, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        *v = obj->n;            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        *v = 0;            \
        cmsLog_error( "failed to retrieve Capabilities object\n");   \
    }

#define GET_RESPORT_CAP_PARAM_STR(i, p, n, v, l) \
    VoiceReservedPortsObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_RESERVED_PORTS, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        snprintf(v, l, "%s", obj->n );            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        snprintf(v, l, "null" );            \
        cmsLog_error( "failed to retrieve Capabilities object\n");   \
    }

#define SET_RESPORT_CAP_PARAM_STR(i, p, n, v, f) \
{ \
    VoiceReservedPortsObject *obj=NULL;                          \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;\
    if( f != NULL )                                     \
    {                                                   \
        /* validation check */                          \
        ret = isValidString( v, f );                    \
        if( CMSRET_SUCCESS != ret )                     \
        {                                               \
            cmsLog_error( "invalid argument value %s\n", v);   \
            return ret;                                 \
        }                                               \
    }                                                   \
    ret = getObject( MDMOID_VOICE_RESERVED_PORTS, i, p, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                        \
        REPLACE_STRING_IF_NOT_EQUAL(obj->n, v);         \
        ret = cmsObj_set((const void *)obj, &iidStack); \
        cmsObj_free((void **)obj);                      \
        if( CMSRET_SUCCESS != ret )                     \
            cmsLog_error( "failed to get SipNetwork object\n");   \
    } \
    else{ \
        cmsLog_error( "failed to get SipNetwork object\n");   \
    } \
}

#define GET_FXS_VOICE_PROC_PARAM_INT(i, p, n, v) \
    VoiceProcessingObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_PROCESSING, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        *v = obj->n;            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        *v = 0;            \
        cmsLog_error( "failed to retrieve SipNetwork object\n");   \
    }

#define SET_POTS_PARAM_STR(i, p, n, v, f) \
{ \
    VoiceServicePotsObject *obj=NULL;                   \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;\
    if( f != NULL )                                     \
    {                                                   \
        /* validation check */                          \
        ret = isValidString( v, f );                    \
        if( CMSRET_SUCCESS != ret )                     \
        {                                               \
            cmsLog_error( "invalid argument value %s\n", v);   \
            return ret;                                 \
        }                                               \
    }                                                   \
    ret = getObject( MDMOID_VOICE_SERVICE_POTS, i, p, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                        \
        REPLACE_STRING_IF_NOT_EQUAL(obj->n, v);         \
        ret = cmsObj_set((const void *)obj, &iidStack); \
        cmsObj_free((void **)obj);                      \
        if( CMSRET_SUCCESS != ret )                     \
            cmsLog_error( "failed to get POTS object\n");   \
    } \
    else{ \
        cmsLog_error( "failed to get POTS object\n");   \
    } \                                                  \
}

#define SET_FXS_VOICE_PROC_PARAM_INT(i, p, n, v) \
{ \
    VoiceProcessingObject *obj=NULL;                 \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK; \
    ret = getObject( MDMOID_VOICE_PROCESSING, i, p, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                        \
        obj->n = v;                              \
        ret = cmsObj_set((const void *)obj, &iidStack); \
        cmsObj_free((void **)&obj);                      \
        if( CMSRET_SUCCESS != ret )                     \
            cmsLog_error( "failed to get FXS object\n");   \
    }                                                   \
    else{                                               \
        cmsLog_error( "failed to get FXS object\n");   \
    }                                                   \
}

#define GET_CODEC_CAP_PARAM_STR(i, p, n, v, l) \
    VoiceCapCodecsObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_CAP_CODECS, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        snprintf(v, l, "%s", obj->n );            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        snprintf(v, l, "null" );            \
        cmsLog_error( "failed to retrieve Capabilities object\n");   \
    }

#define GET_CODEC_CAP_PARAM_INT(i, p, n, v) \
    VoiceCapCodecsObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_CAP_CODECS, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        *v = obj->n;            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        *v = 0;            \
        cmsLog_error( "failed to retrieve Capabilities object\n");   \
    }

#define GET_CODEC_CAP_PARAM_BOOL(i, p, n, v, l) \
    VoiceCapCodecsObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_CAP_CODECS, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        snprintf(v, l, "%s", obj->n?"true":"false");            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        snprintf(v, l, "false" );            \
        cmsLog_error( "failed to retrieve Capabilities object\n");   \
    }


/* Returns value as an INTEGER */
#define GET_VOICE_SVC_PARAM_UINT( i, n, v) \
    VoiceObject *obj=NULL;\
    ret = getObject( MDMOID_VOICE, i, 0, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        *v = obj->n;            \
        cmsObj_free((void **)&obj);               \
    } \
    else{ \
        cmsLog_error( "failed to retrieve voice service object\n");   \
    }

#define SET_VOICE_SVC_PARAM_STR(i, n, v, f) \
{ \
    VoiceObject *obj=NULL;                          \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;\
    if( f != NULL ){                                    \
        /* validation check */                          \
        ret = isValidString( v, f );                    \
        if( CMSRET_SUCCESS != ret ){                    \
            cmsLog_error( "invalid argument value %s\n", v);   \
            return ret;                                 \
        }                                               \
    }                                                   \
    ret = getObject( MDMOID_VOICE, i, 0, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                        \
        REPLACE_STRING_IF_NOT_EQUAL(obj->n, v);         \
        ret = cmsObj_set((const void *)obj, &iidStack); \
        cmsObj_free((void **)&obj);                      \
        if( CMSRET_SUCCESS != ret ){cmsLog_error( "failed to set voice service object, ret %d\n", ret);}\
    } \
    else{ cmsLog_error( "failed to get voice service object, ret %d\n", ret);} \
}

#if 0
#define SET_VOICE_SVC_PARAM_BOOL(i, n, v) \
{ \
    VoiceObject *obj=NULL;                          \
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;\
    UBOOL8  setvalue = FALSE;                           \
    if( __buf != NULL ){                                \
        ret = convertStringToBool( __buf, &setvalue);   \
    }                                                   \
    else{\
        ret = CMSRET_INVALID_ARGUMENTS; \
    }\

if( f != NULL ){                                    \
        /* validation check */                          \
        ret = isValidString( v, f );                    \
        if( CMSRET_SUCCESS != ret ){                    \
            cmsLog_error( "invalid argument value %s\n", v);   \
            return ret;                                 \
        }                                               \
    }                                                   \



    ret = getObject( MDMOID_VOICE, i, 0, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                        \
        REPLACE_STRING_IF_NOT_EQUAL(obj->n, v);         \
        ret = cmsObj_set((const void *)obj, &iidStack); \
        cmsObj_free((void **)&obj);                      \
        if( CMSRET_SUCCESS != ret ){cmsLog_error( "failed to set voice service object, ret %d\n", ret);}\
    } \
    else{ cmsLog_error( "failed to get voice service object, ret %d\n", ret);} \
}

#endif

#ifdef DMP_SIPCLIENT_1

#define GET_CALLING_FEATURES_PARAM_UNSUPPORTED( v, l)  { snprintf(v, l, "%s", "false"); ret = CMSRET_METHOD_NOT_SUPPORTED; }

#define GET_SIP_OBJ_PARAM_UINT(i, n, v) \
{   \
    VoiceServiceSipObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_SERVICE_SIP, i, 0, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        *v =  obj->n;            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        cmsLog_error( "failed to retrieve Sip object\n");   \
    } \
}

#endif /*  DMP_SIPCLIENT_1 */

#ifdef DMP_BASELINE_1

#define  GET_CODEC_PROFILE_PARAM_BOOL( i, p, n, v, l ) \
    CodecProfileObject *obj=NULL;                 \
    ret = getObject( MDMOID_CODEC_PROFILE, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        snprintf(v, l, "%s", obj->n?"true":"false");            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        snprintf(v, l, "null" );            \
        cmsLog_error( "failed to retrieve SipNetwork object\n");   \
    }

#define  GET_CODEC_PROFILE_PARAM_STR( i, p, n, v, l ) \
    CodecProfileObject *obj=NULL;                 \
    ret = getObject( MDMOID_CODEC_PROFILE, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        snprintf(v, l, "%s", obj->n );            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        snprintf(v, l, "null" );            \
        cmsLog_error( "failed to retrieve SipNetwork object\n");   \
    }

#define  GET_VOICE_CAP_CODEC_PARAM_STR( i, p, n, v, l ) \
    VoiceCapCodecsObject *obj=NULL;                 \
    ret = getObject( MDMOID_VOICE_CAP_CODECS, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        snprintf(v, l, "%s", obj->n );            \
        cmsObj_free((void **)&obj);                       \
    } \
    else{ \
        snprintf(v, l, "null" );            \
        cmsLog_error( "failed to retrieve SipNetwork object\n");   \
    }

#endif /* DMP_BASELINE_1 */

typedef struct
{
   MdmObjectId id;                   /* MDM Object ID */
   int instId;                       /* Instance ID */
   InstanceIdStack iidStack;         /* Instance ID Stack */
}LEVELINFO;


/*============================= Helper Function Prototypes ===========================*/

/* Set helper functions */
static CmsRet setVlCFFeatureValue( CFCALLTYPE callType, unsigned int vpInst, unsigned int lineInst, DAL_VOICE_FEATURE_CODE callFeatId, UINT32 setVal );
static CmsRet setRedOption ( unsigned int vpInst, char * setVal );
static CmsRet setVlCLCodecList ( unsigned int vpInst, unsigned int lineInst, const char * setVal );

/* Get helper functions */
static void dumpCmdParam( DAL_VOICE_PARMS *parms, char *value );

/* Mapping functions */
static CmsRet mapSpNumToVpInst ( int spNum, int * vpInst );
static CmsRet mapVpInstLineInstToCMAcnt( int vpInst, int lineInst, int * cmAcntNum );
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
    CmsRet  ret = CMSRET_SUCCESS;
    DAL_VOICE_PARMS parmsList;
    char getVal[TEMP_CHARBUF_SIZE];
#if VOICE_IPV6_SUPPORT
   int isIpv6 = 0;
#endif /* VOICE_IPV6_SUPPORT */
   int i,spNum;
   int j,numAcc,lineInst, numFxs, numFxo;
   int numLines = 0, numExt = 0;
   (void)parms;
   (void)value;
   int  spInst;    /* sip service instance */

    cmsLog_debug("%s() Enter", __FUNCTION__);
    memset( &parmsList, 0, sizeof(parmsList) );
    memset(getVal, 0, sizeof(getVal));

    dalVoice_GetNumSrvProv( &spNum );
    if( spNum > 0 )
    {
        dalVoice_mapSpNumToSvcInst( 0, &spInst );
        parmsList.op[0] = spInst;
    }
#if VOICE_IPV6_SUPPORT
    dalVoice_GetIpFamily( &parmsList, (char*)getVal, TEMP_CHARBUF_SIZE);
    isIpv6 = !(cmsUtl_strcmp( getVal, MDMVS_IPV6 ));
    memset( getVal, 0, sizeof(getVal));
    if( isIpv6 )
    {
        strcpy( getVal, ZERO_ADDRESS_IPV6);
    }
    else
#endif /* VOICE_IPV6_SUPPORT */
    {
        strcpy( getVal, ZERO_ADDRESS_IPV4);
    }

    /* get maximum number of service providers configured */
    dalVoice_GetNumPhysFxsEndpt( &numFxs );
    dalVoice_GetNumPhysFxoEndpt( &numFxo );

    for ( i = 0; i<spNum; i++ )
    {
        int  netwkInst; /* network instance */
        int  vpInst;  /* voip profile instance */
        int  cpInst;  /* codec profile instance */
        int  fxsInst;  /* Pots FXS instance */
        int  ccLineInst; /* call control line instance */
        int  ccExtInst; /* call control extension instance */
        int  ccOutMapInst; /* call control outgoing map instance */
        int  ccInMapInst;  /* call control incoming map instance */
        int  ccFeatureInst; /* calling feature set instance */
        char codecList[64];
        int  contactInst;

        memset( codecList, 0, sizeof(codecList));
        strncat( codecList, defaultCodecList[0], strlen(defaultCodecList[0]));
        for( j = 1; j < 4; j++)
        {
            strcat( codecList, "," );
            strncat( codecList, defaultCodecList[j], strlen(defaultCodecList[j]));
        }

        /* map voice provider number to svc instance */
        dalVoice_mapSpNumToSvcInst( i, &spInst );
        parmsList.op[0] = spInst;

        /* Set Voice Svc level params */        
#if defined(EPON_SFU) && defined(BRCM_PKTCBL_SUPPORT)
        dalVoice_SetBoundIfName( &parmsList, EPON_VOICE_WAN_IF_NAME);
#else
        dalVoice_SetBoundIfName( &parmsList, "LAN");
#endif

        dalVoice_SetDigitMap( &parmsList, "[1-9]xxx|xx+*|xx+#|00x.T|011x.T|x+T");

        dalVoice_GetNumVoipProfile( &parmsList, &numAcc);
        if( numAcc > 0 )
        {
            for(j = numAcc-1; j >= 0 ;j--)
            {
                parmsList.op[0] = spInst;
                parmsList.op[1] = j;
                if( dalVoice_mapVoipProfNumToInst(&parmsList, &vpInst) == CMSRET_SUCCESS)
                {
                    parmsList.op[0] = spInst;
                    parmsList.op[1] = vpInst;
                    dalVoice_DeleteVoipProfile(&parmsList);
                }
            }
        }

        /* create sip voip profile */
        if( dalVoice_AddVoipProfile( &parmsList, &vpInst) == CMSRET_SUCCESS)
        {
            parmsList.op[1] = vpInst;
            dalVoice_SetDTMFMethod( &parmsList, MDMVS_INBAND );
            dalVoice_SetV18Enable( &parmsList,  MDMVS_NO );
            dalVoice_SetSrtpEnable( &parmsList, MDMVS_YES );
            dalVoice_SetVoipProfName( &parmsList, "voipProfile1" );
            dalVoice_SetVoipProfEnable( &parmsList, MDMVS_YES );
        }
        else
        {
            cmsLog_error("%s() Create Voice Codec Profile error!!!!", __FUNCTION__);
            break;
        }

        /* first check how mnay voice codec profile */
        dalVoice_GetNumCodecProfile( &parmsList, &numAcc );
        if( numAcc > 0 )
        {
            cmsLog_debug("%s() exist codec profile number (%d) \n", __FUNCTION__, numAcc);
            for(j = numAcc -1 ; j >= 0 ; j--)
            {
                parmsList.op[1] = j;
                if( dalVoice_mapCpNumToInst(&parmsList, &cpInst) == CMSRET_SUCCESS)
                {
                    parmsList.op[1] = cpInst;
                    dalVoice_DeleteCodecProfile(&parmsList);
                }
            }
        }
        /* add new codecProfile */
        for(j = 0; j < 4; j++ )
        {
            dalVoice_AddCodecProfileByName( &parmsList, defaultCodecList[j], &cpInst);
        }

        /* first check how many network exist*/
        dalVoice_GetNumSipNetwork( &parmsList, &numAcc );
        if( numAcc > 0 )
        {
            for(j = numAcc-1; j >= 0; j--)
            {
                parmsList.op[0] = spInst;
                parmsList.op[1] = j;
                if( dalVoice_mapNetworkNumToInst( &parmsList , &netwkInst ) == CMSRET_SUCCESS)
                {
                    parmsList.op[0] = spInst;
                    parmsList.op[1] = netwkInst;
                    dalVoice_DeleteSipNetwork(&parmsList);
                }
            }
        }

        /* create sip network */
        parmsList.op[0] = spInst;
        dalVoice_AddSipNetwork( &parmsList, &netwkInst);

        parmsList.op[1] = netwkInst;
        dalVoice_SetSipNetworkEnabled( &parmsList, "Yes");
        dalVoice_SetSipUserAgentPort( &parmsList, "5060");
        dalVoice_SetSipOutboundProxyPort( &parmsList, "5060");
        dalVoice_SetSipOutboundProxy( &parmsList, getVal);
        dalVoice_SetSipProxyServerPort( &parmsList, "5060");
        dalVoice_SetSipProxyServer( &parmsList, getVal);
        dalVoice_SetSipRegistrarServerPort( &parmsList, "5060");
        dalVoice_SetSipRegistrarServer( &parmsList, getVal );
        dalVoice_SetSipRegisterExpires( &parmsList, "3600");
        dalVoice_SetSipSecDomainName( &parmsList, "");
        dalVoice_SetSipSecProxyPort( &parmsList, "5060");
        dalVoice_SetSipSecProxyAddr( &parmsList, getVal);
        dalVoice_SetSipSecOutboundProxyPort( &parmsList, "5060");
        dalVoice_SetSipSecOutboundProxyAddr( &parmsList, getVal);
        dalVoice_SetSipSecRegistrarPort( &parmsList, "5060");
        dalVoice_SetSipSecRegistrarAddr( &parmsList, getVal);
        dalVoice_SetSipRegisterRetryInterval( &parmsList, "20");
        dalVoice_SetSipConferencingURI( &parmsList, "");
        dalVoice_SetSipConferencingOption( &parmsList, MDMVS_LOCAL);
        dalVoice_SetSipDSCPMark( &parmsList, "46");
        dalVoice_SetSipFailoverEnable(&parmsList, "No");
        dalVoice_SetSipOptionsEnable(&parmsList, "No");
        dalVoice_SetSipBackToPrimOption(&parmsList, MDMVS_DISABLED);

        parmsList.op[2] = vpInst;
        dalVoice_SetSipNetworkVoipProfileAssoc(&parmsList);

        parmsList.op[0] = spInst;
        parmsList.op[1] = netwkInst;
        dalVoice_SetSipNetworkCodecList( &parmsList, codecList );

        /* delete existing feature set */
        dalVoice_GetNumCallFeatureSet( &parmsList, &numAcc );
        if( numAcc > 0 )
        {
            for( j=0; j < numAcc ; j++)
            {
                parmsList.op[0] = spInst;
                parmsList.op[1] = 0;   /* alway delete the first one */
                if( dalVoice_mapCallFeatureSetNumToInst( &parmsList , &lineInst ) == CMSRET_SUCCESS)
                {
                    parmsList.op[1] = lineInst;
                    dalVoice_DeleteCallFeatureSet( &parmsList );
                }
            }
        }

        /* first check how many sip client exist*/
        dalVoice_GetNumSipClient( &parmsList, &numAcc );
        if( numAcc > 0 )
        {
            for( j=numAcc-1; j >= 0; j--)
            {
                parmsList.op[0] = spInst;
                parmsList.op[1] = j;
                if( dalVoice_mapAcntNumToClientInst( &parmsList , &lineInst ) == CMSRET_SUCCESS)
                {
                    parmsList.op[1] = lineInst;
                    dalVoice_DeleteSipClient( &parmsList );
                }
            }
        }

        dalVoice_GetNumOfLine( &parmsList, &numAcc );
        if( numAcc > 0 )
        {
            for( j=numAcc-1; j >= 0; j--)
            {
                parmsList.op[0] = spInst;
                if( dalVoice_mapAcntNumToLineInst( spInst, j, &ccLineInst ) == CMSRET_SUCCESS)
                {
                    parmsList.op[1] = ccLineInst;
                    dalVoice_DeleteCallCtrlLine( &parmsList );
                }
            }
        }

        /* create SIP Client  and CallCtl line, call feature, 1:1 mapping */
        for( j = 0; j < numFxs; j++ )
        {
            parmsList.op[0] = spInst;
            /* map vpInst and account num to line instance */
            dalVoice_AddSipClient( &parmsList, &lineInst );

            /* add override sip contact header */
            parmsList.op[1] = lineInst;
            dalVoice_AddSipContactUri( &parmsList, &contactInst );

            /* enable this account */
            dalVoice_SetVlEnable( &parmsList, "on" );
            dalVoice_SetT38Enable( &parmsList, "off" );
            /* 1 calling feature set per sip client */
            dalVoice_AddCallFeatureSet( &parmsList, &ccFeatureInst );
            /* For all accounts with the new CM, disable all advanced features */
            parmsList.op[0] = spInst;
            parmsList.op[1] = ccFeatureInst;
            dalVoice_SetVlCFAnonCallBlck( &parmsList, "off");
            dalVoice_SetVlCFAnonymousCalling( &parmsList, "off");
            dalVoice_SetVlCFMWIEnable( &parmsList, "off" );
            dalVoice_SetVlCFWarmLine ( &parmsList, "off" );
            dalVoice_SetVlCFWarmLineNum ( &parmsList, "" );
            dalVoice_SetVlCFCallFwdAll( &parmsList, "off" );
            dalVoice_SetVlCFCallFwdNoAns ( &parmsList, "off" );
            dalVoice_SetVlCFCallFwdBusy ( &parmsList, "off" );
            dalVoice_SetVlCFCallWaiting ( &parmsList, "on" );
            dalVoice_SetVlCFDoNotDisturb ( &parmsList, "off" );
            dalVoice_SetVlCFCallBarring ( &parmsList, "off" );
            dalVoice_SetVlCFCallBarringMode( &parmsList, "0" );
            dalVoice_SetVlCFCallBarringPin ( &parmsList, "9999" );
            dalVoice_SetVlCFCallBarringDigitMap ( &parmsList, "" );
            dalVoice_SetVlCFCallReturn ( &parmsList, "off" );
            dalVoice_SetVlCFCallRedial ( &parmsList, "off" );
            dalVoice_SetEuroFlashEnable ( &parmsList, "off" );

            parmsList.op[2] = netwkInst;
            dalVoice_SetSipClientNetworkAssoc(&parmsList);

            if( dalVoice_AddCallCtrlLine( &parmsList, &ccLineInst ) == CMSRET_SUCCESS)
            {
                parmsList.op[1] = ccLineInst ;
                parmsList.op[2] = lineInst ;
                dalVoice_SetCallCtrlLineEnabled( &parmsList, "on" );
                dalVoice_SetCcLineSipClientAssoc( &parmsList );

                parmsList.op[2] = ccFeatureInst ;
                dalVoice_SetCcLineFeatureSetAssoc( &parmsList );
            }
        }
        /* add PSTN line, only one PSTN port is supported */
        if( numFxo > 0 )
        {
            int  fxoInst;

            parmsList.op[0] = spInst;
            parmsList.op[1] = 0;
            dalVoice_mapPotsFxoNumToInst( &parmsList, &fxoInst );
            if( dalVoice_AddCallCtrlLine( &parmsList, &ccLineInst ) == CMSRET_SUCCESS)
            {
                parmsList.op[1] = ccLineInst ;
                parmsList.op[2] = fxoInst ;
                dalVoice_SetCallCtrlLineEnabled( &parmsList, "on" );
                dalVoice_SetCcLinePotsFxoAssoc( &parmsList );
            }

            dalVoice_SetPstnDialPlan( &parmsList, "911|102");
        }

        /* create extension based on FXS and FXO */
        /* first check how many extension exist*/
        dalVoice_GetNumOfExtension( &parmsList, &numAcc );
        if( numAcc > 0 )
        {
            for( j=numAcc-1; j >= 0; j--)
            {
                parmsList.op[0] = spInst;
                parmsList.op[1] = j;
                if( dalVoice_mapExtNumToExtInst( spInst, j,  &ccExtInst ) == CMSRET_SUCCESS)
                {
                    parmsList.op[1] = ccExtInst;
                    dalVoice_DeleteCallCtrlExt( &parmsList );
                }
            }
        }

        /* create CallCtl Extension */
        for( j = 0; j < numFxs; j++ )
        {
            char extNumber[64];
            char extName[64];

            parmsList.op[0] = spInst;
            /* map vpInst and account num to line instance */
            dalVoice_AddCallCtrlExt( &parmsList, &ccExtInst );

            parmsList.op[1] = j;

            dalVoice_mapPotsFxsNumToInst( &parmsList, &fxsInst );
            parmsList.op[1] = ccExtInst;
            dalVoice_SetCallCtrlExtEnabled( &parmsList, "on" );
            parmsList.op[2] = fxsInst;
            dalVoice_SetCallCtrlExtFxsAssoc(&parmsList);

            memset(extNumber, 0, sizeof(extNumber));
            memset(extName, 0, sizeof(extName));
            sprintf( extNumber, "%u", j+1 );
            sprintf( extName, "Extension_%u", j );
            dalVoice_SetCallCtrlExtNumber(&parmsList, extNumber );
            dalVoice_SetCallCtrlExtName(&parmsList, extName );
        }

        dalVoice_GetNumOfLine( &parmsList, &numLines );
        dalVoice_GetNumOfExtension( &parmsList, &numExt );

        /* create outgoing call map */
        dalVoice_GetNumOutgoingMap( &parmsList, &numAcc );
        if( numAcc > 0 )
        {
            for( j=numAcc-1; j >= 0; j--)
            {
                parmsList.op[0] = spInst;
                parmsList.op[1] = j;
                if( dalVoice_mapOutgoingMapNumToInst( &parmsList , &ccOutMapInst ) == CMSRET_SUCCESS)
                {
                    parmsList.op[1] = ccOutMapInst;
                    dalVoice_DeleteCallCtrlOutgoingMap( &parmsList );
                }
            }
        }

        parmsList.op[0] = spInst;
        /* create CallCtl outgoing map */
        for( j = 0; j < numLines*numExt; j++ )
        {
            /* map vpInst and account num to line instance */
            dalVoice_AddCallCtrlOutgoingMap( &parmsList, &ccOutMapInst );
            dalVoice_mapExtNumToExtInst( spInst, (j%numExt), &ccExtInst);
            dalVoice_mapAcntNumToLineInst( spInst, (j/numExt), &ccLineInst);

            parmsList.op[1] = ccOutMapInst;
            dalVoice_SetOutgoingMapEnabled( &parmsList, "enabled" );
            parmsList.op[2] = ccLineInst;
            parmsList.op[3] = ccExtInst;
            dalVoice_SetOutgoingMapLineExt( &parmsList );
        }

        /* create incoming call map */
        dalVoice_GetNumIncomingMap( &parmsList, &numAcc );
        if( numAcc > 0 )
        {
            for( j=numAcc-1; j >= 0; j--)
            {
                parmsList.op[0] = spInst;
                parmsList.op[1] = j;
                if( dalVoice_mapIncomingMapNumToInst( &parmsList , &ccInMapInst ) == CMSRET_SUCCESS)
                {
                    parmsList.op[1] = ccInMapInst;
                    dalVoice_DeleteCallCtrlIncomingMap( &parmsList );
                }
            }
        }

        parmsList.op[0] = spInst;
        /* create CallCtl incoming map */
        for( j = 0; j < numLines*numExt; j++ )
        {
            /* map vpInst and account num to line instance */
            dalVoice_AddCallCtrlIncomingMap( &parmsList, &ccInMapInst );
            dalVoice_mapExtNumToExtInst( spInst, (j%numExt), &ccExtInst);
            dalVoice_mapAcntNumToLineInst(spInst, (j/numExt), &ccLineInst);

            parmsList.op[1] = ccInMapInst;
            dalVoice_SetIncomingMapEnabled( &parmsList, "enabled" );
            parmsList.op[2] = ccLineInst;
            parmsList.op[3] = ccExtInst;
            dalVoice_SetIncomingMapLineExt( &parmsList );
        }
   }

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetStatus
**
**  PURPOSE:        Obtains the voice application status from MDM
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:   value "1" if running, "0" if not
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
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

/*<START>===================================== DAL Set functions =====================================<START>*/

#define __PEER(src,dest)   ((src<<16)+dest)

/*****************************************************************
**  FUNCTION:       getL2ToL2ObjAssocType
**
**  PURPOSE:        get object type of link between Level2 objects
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:   destInst = associated L2 object instance
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
static CmsRet getL2ToL2ObjAssocType( MdmObjectId srcOid, MdmObjectId *destOid, int svcInst, int srcInst, int *destInst )
{
    CmsRet ret = CMSRET_SUCCESS;
    char *destFullPath=NULL;
    MdmPathDescriptor pathDesc;
    void    *obj = NULL;

    cmsLog_debug ("%s() enter", __FUNCTION__);
    if( destInst == NULL || destOid == NULL )
    {
        return CMSRET_INVALID_PARAM_VALUE;
    }

    if( srcInst > 0 && getObject( srcOid, svcInst, srcInst, 0 , 0, OGF_NO_VALUE_UPDATE, NULL, &obj ) == CMSRET_SUCCESS )
    {
        switch(srcOid){
            case  MDMOID_CALL_CONTROL_EXTENSION:
            {
                destFullPath = ((CallControlExtensionObject *)obj)->provider;
            }
            break;
            case  MDMOID_CALL_CONTROL_LINE:
            {
                destFullPath = ((CallControlLineObject *)obj)->provider;
            }
            break;
            default:
                cmsLog_error ("%s() unknown src obj id (%u)\n", __FUNCTION__, srcOid);
        }
    }
    else
    {
        cmsLog_debug ("%s() unknown L2 object (%u) instance", __FUNCTION__, srcOid);
    }

    if( destFullPath != NULL && strlen( destFullPath ) > 0){
        ret = cmsMdm_fullPathToPathDescriptor(destFullPath, &pathDesc);
        if(ret == CMSRET_SUCCESS && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
        {
            *destInst = POP_INSTANCE_ID(&pathDesc.iidStack);
            *destOid  = pathDesc.oid;
        }
    }

    if( obj )
    {
        cmsObj_free( (void **)&obj );
    }

    return (ret);
}

/*****************************************************************
**  FUNCTION:       getL2ToL2ObjAssoc
**
**  PURPOSE:        get link pointer between Level2 objects
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:   destInst = associated L2 object instance
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
static CmsRet getL2ToL2ObjAssoc( MdmObjectId srcOid, MdmObjectId destOid, int svcInst, int srcInst, int *destInst )
{
    CmsRet ret = CMSRET_SUCCESS;
    char *destFullPath=NULL;
    MdmPathDescriptor pathDesc;
    UINT32   associationPair = __PEER( srcOid, destOid);
    void    *obj = NULL;

    cmsLog_debug ("%s() enter", __FUNCTION__);
    if( destInst == NULL )
    {
        return CMSRET_INVALID_PARAM_VALUE;
    }

    if( srcInst > 0 && getObject( srcOid, svcInst, srcInst, 0 , 0, OGF_NO_VALUE_UPDATE, NULL, &obj ) == CMSRET_SUCCESS )
    {
        switch(associationPair){
            case  __PEER(MDMOID_CALL_CONTROL_LINE, MDMOID_CALLING_FEATURES_SET):
            {
                destFullPath = ((CallControlLineObject *)obj)->callingFeatures;
            }
            break;
            case  __PEER(MDMOID_CALL_CONTROL_LINE, MDMOID_SIP_CLIENT):
            case  __PEER(MDMOID_CALL_CONTROL_LINE, MDMOID_POTS_FXO):
            {
                destFullPath = ((CallControlLineObject *)obj)->provider;
            }
            break;
            case  __PEER(MDMOID_SIP_CLIENT, MDMOID_SIP_NETWORK):
            {
                destFullPath = ((SipClientObject *)obj)->network;
            }
            break;
            case  __PEER(MDMOID_SIP_NETWORK, MDMOID_IP_PROFILE):
            {
                destFullPath = ((SIPNetworkObject *)obj)->voIPProfile;
            }
            break;
            case  __PEER(MDMOID_CODEC_PROFILE, MDMOID_VOICE_CAP_CODECS):
            {
                destFullPath = ((CodecProfileObject *)obj)->codec;
            }
            break;
            case  __PEER(MDMOID_CALL_CONTROL_EXTENSION, MDMOID_POTS_FXS):
            {
                destFullPath = ((CallControlExtensionObject *)obj)->provider;
            }
            break;
            case  __PEER(MDMOID_VOICE_CALL_LOG, MDMOID_CALL_CONTROL_LINE):
            {
                destFullPath = ((VoiceCallLogObject *)obj)->usedLine;
            }
            break;
            default:
                cmsLog_error ("%s() unknown association peer src (%u) dest (%u)", __FUNCTION__, srcOid, destOid);
        }

    }
    else
    {
        cmsLog_debug ("%s() unknown L2 object (%u) instance", __FUNCTION__, srcOid);
    }

    if( destFullPath != NULL && strlen( destFullPath ) > 0){
        ret = cmsMdm_fullPathToPathDescriptor(destFullPath, &pathDesc);
        if(ret == CMSRET_SUCCESS && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
        {
            *destInst = POP_INSTANCE_ID(&pathDesc.iidStack);
        }
    }

    if( obj )
    {
        cmsObj_free( (void **)&obj );
    }

    return (ret);
}

/*****************************************************************
**  FUNCTION:       setL2ToL2ObjAssoc
**
**  PURPOSE:        set link pointer between Level2 objects
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:   value "1" if running, "0" if not
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
static CmsRet setL2ToL2ObjAssoc( MdmObjectId srcOid, MdmObjectId destOid, int svcInst, int srcInst, int destInst )
{
    CmsRet ret = CMSRET_SUCCESS;
    char *destFullPath=NULL;
    MdmPathDescriptor pathDesc;
    UINT32   associationPair = __PEER( srcOid, destOid);
    void    *obj = NULL;
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;

    cmsLog_debug ("%s() enter", __FUNCTION__);
    /* initialize pathDesc for codec */
    INIT_PATH_DESCRIPTOR(&pathDesc);

    if( destInst > 0 )
    {
        pathDesc.oid = destOid;
        PUSH_INSTANCE_ID( &(pathDesc.iidStack), svcInst);
        PUSH_INSTANCE_ID( &(pathDesc.iidStack), destInst);

        /* check pathDesc exist and convert to full path string */
        if( cmsMdm_isPathDescriptorExist(&pathDesc) &&
            cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &destFullPath) == CMSRET_SUCCESS)
        {
            cmsLog_debug ("%s() L2 object ( %u ) path = %s", __FUNCTION__, destOid, destFullPath);
        }
        else
        {
            cmsLog_error ("%s() invalid destination path for destOid (%d)", __FUNCTION__, destOid);
        }
    }

    if( srcInst > 0 && getObject( srcOid, svcInst, srcInst, 0 , 0, OGF_NO_VALUE_UPDATE, &iidStack, &obj ) == CMSRET_SUCCESS )
    {
        switch(associationPair){
            case  __PEER(MDMOID_CALL_CONTROL_LINE, MDMOID_CALLING_FEATURES_SET):
            {
                if( destFullPath != NULL ){
                    REPLACE_STRING_IF_NOT_EQUAL(((CallControlLineObject *)obj)->callingFeatures, destFullPath);
                }
                else{
                    cmsMem_free(((CallControlLineObject *)obj)->callingFeatures);
                }
            }
            break;
            case  __PEER(MDMOID_CALL_CONTROL_LINE, MDMOID_SIP_CLIENT):
            case  __PEER(MDMOID_CALL_CONTROL_LINE, MDMOID_POTS_FXO):
            {
                if( destFullPath != NULL ){
                    REPLACE_STRING_IF_NOT_EQUAL(((CallControlLineObject *)obj)->provider, destFullPath);
                }
                else{
                    cmsMem_free(((CallControlLineObject *)obj)->provider);
                }
            }
            break;
            case  __PEER(MDMOID_SIP_CLIENT, MDMOID_SIP_NETWORK):
            {
                if( destFullPath != NULL ){
                    REPLACE_STRING_IF_NOT_EQUAL(((SipClientObject *)obj)->network, destFullPath);
                }
                else{
                    cmsMem_free(((SipClientObject *)obj)->network);
                }
            }
            break;
            case  __PEER(MDMOID_SIP_NETWORK, MDMOID_IP_PROFILE):
            {
                if( destFullPath != NULL ){
                    REPLACE_STRING_IF_NOT_EQUAL(((SIPNetworkObject *)obj)->voIPProfile, destFullPath);
                }
                else{
                    cmsMem_free(((SIPNetworkObject *)obj)->voIPProfile);
                }
            }
            break;
            case  __PEER(MDMOID_CODEC_PROFILE, MDMOID_VOICE_CAP_CODECS):
            {
                if( destFullPath != NULL ){ /* only allow true path to be set */
                    REPLACE_STRING_IF_NOT_EQUAL(((CodecProfileObject *)obj)->codec, destFullPath);
                }
            }
            break;
            case  __PEER(MDMOID_CALL_CONTROL_EXTENSION, MDMOID_POTS_FXS):
            {
                if( destFullPath != NULL ){ /* only allow true path to be set */
                    REPLACE_STRING_IF_NOT_EQUAL(((CallControlExtensionObject *)obj)->provider, destFullPath);
                }
            }
            break;
            case  __PEER(MDMOID_CALL_CONTROL_INCOMING_MAP, MDMOID_CALL_CONTROL_LINE):
            {
                if( destFullPath != NULL ){ /* only allow true path to be set */
                    REPLACE_STRING_IF_NOT_EQUAL(((CallControlIncomingMapObject *)obj)->line, destFullPath);
                }
            }
            break;
            case  __PEER(MDMOID_CALL_CONTROL_INCOMING_MAP, MDMOID_CALL_CONTROL_EXTENSION):
            {
                if( destFullPath != NULL ){ /* only allow true path to be set */
                    REPLACE_STRING_IF_NOT_EQUAL(((CallControlIncomingMapObject *)obj)->extension, destFullPath);
                }
            }
            break;
            case  __PEER(MDMOID_CALL_CONTROL_OUTGOING_MAP, MDMOID_CALL_CONTROL_LINE):
            {
                if( destFullPath != NULL ){ /* only allow true path to be set */
                    REPLACE_STRING_IF_NOT_EQUAL(((CallControlOutgoingMapObject *)obj)->line, destFullPath);
                }
            }
            break;
            case  __PEER(MDMOID_CALL_CONTROL_OUTGOING_MAP, MDMOID_CALL_CONTROL_EXTENSION):
            {
                if( destFullPath != NULL ){ /* only allow true path to be set */
                    REPLACE_STRING_IF_NOT_EQUAL(((CallControlOutgoingMapObject *)obj)->extension, destFullPath);
                }
            }
            break;
            case  __PEER(MDMOID_VOICE_CALL_LOG, MDMOID_CALL_CONTROL_LINE):
            {
                if( destFullPath != NULL ){ /* only allow true path to be set */
                    REPLACE_STRING_IF_NOT_EQUAL(((VoiceCallLogObject *)obj)->usedLine, destFullPath);
                }
            }
            break;
            default:
                cmsLog_error ("%s() unknown association peer src (%u) dest (%u)", __FUNCTION__, srcOid, destOid);
        }

        ret = cmsObj_set( (const void *)obj, &iidStack );
        cmsObj_free( (void **)&obj );
    }
    else
    {
        cmsLog_debug ("%s() unknown L2 object (%u) instance", __FUNCTION__, srcOid);
    }

    if( destFullPath != NULL ){
        CMSMEM_FREE_BUF_AND_NULL_PTR(destFullPath);
    }

    return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCcLineFeatureSetAssoc
**
**  PURPOSE:        set sip client point to the network instance
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - call ctrl line instance
**                  op[2] - sip client instance
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_SetCcLineFeatureSetAssoc( DAL_VOICE_PARMS *parms )
{
    return setL2ToL2ObjAssoc( MDMOID_CALL_CONTROL_LINE, MDMOID_CALLING_FEATURES_SET, parms->op[0], parms->op[1], parms->op[2]);
}

CmsRet  dalVoice_GetCcLineFeatureSetAssoc( DAL_VOICE_PARMS *parms )
{
    return getL2ToL2ObjAssoc( MDMOID_CALL_CONTROL_LINE, MDMOID_CALLING_FEATURES_SET, parms->op[0], parms->op[1], &parms->op[2]);
}
/*****************************************************************
**  FUNCTION:       dalVoice_SetCcLineSipClientAssoc
**
**  PURPOSE:        Set association between call control line ans SIP client
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - call ctrl line instance
**                  op[2] - sip client instance
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCcLineSipClientAssoc( DAL_VOICE_PARMS *parms )
{
    return setL2ToL2ObjAssoc( MDMOID_CALL_CONTROL_LINE, MDMOID_SIP_CLIENT, parms->op[0], parms->op[1], parms->op[2]);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCcLinePotsFxoAssoc
**
**  PURPOSE:        Set association between call control line and FXO port
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - call ctrl line instance
**                  op[2] - fxo instance
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCcLinePotsFxoAssoc( DAL_VOICE_PARMS *parms )
{
    return setL2ToL2ObjAssoc( MDMOID_CALL_CONTROL_LINE, MDMOID_POTS_FXO, parms->op[0], parms->op[1], parms->op[2]);
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCcLinePotsFxoAssoc
**
**  PURPOSE:        Get association between call control line and FXO port
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - call ctrl line instance
**
**
**  OUTPUT PARMS:   op[2] - fxo instance
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCcLinePotsFxoAssoc( DAL_VOICE_PARMS *parms )
{
    return getL2ToL2ObjAssoc( MDMOID_CALL_CONTROL_LINE, MDMOID_POTS_FXO, parms->op[0], parms->op[1], &parms->op[2]);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetSipClientNetworkAssoc
**
**  PURPOSE:        Set sip client point to the network instance
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - SIP client instance
**                  op[2] - network instance
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetSipClientNetworkAssoc( DAL_VOICE_PARMS *parms )
{
    return setL2ToL2ObjAssoc( MDMOID_SIP_CLIENT, MDMOID_SIP_NETWORK, parms->op[0], parms->op[1], parms->op[2]);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetSipClientNetworkAssoc
**
**  PURPOSE:        Set sip client point to the network instance
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - SIP client instance
**
**  OUTPUT PARMS:   op[2] - associated network instance
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipClientNetworkAssoc( DAL_VOICE_PARMS *parms )
{
    return getL2ToL2ObjAssoc( MDMOID_SIP_CLIENT, MDMOID_SIP_NETWORK, parms->op[0], parms->op[1], &parms->op[2]);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCodecProfileAssoc
**
**  PURPOSE:        set codec profile point to the codec instance
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - codec profile instance
**                  op[2] - codec instance
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCodecProfileAssoc( DAL_VOICE_PARMS *parms )
{
    return setL2ToL2ObjAssoc( MDMOID_CODEC_PROFILE, MDMOID_VOICE_CAP_CODECS, parms->op[0], parms->op[1], parms->op[2]);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetLoggingLevel
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;

    SET_VOICE_SVC_PARAM_STR(parms->op[0], X_BROADCOM_COM_LoggingLevel, setVal, loglevel_valid_string );

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
   if ( (ret = getObject(MDMOID_VOICE, parms->op[0], 0, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error( "Can't retrieve voice service object\n" );
      return (ret);
   }

   if(obj->X_BROADCOM_COM_ModuleLogLevels == NULL)
   {
      snprintf(modLevels, sizeof(modLevels), "%s=%d", modName, level);
   }
   else
   {
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
   CmsRet ret = CMSRET_SUCCESS;

   SET_VOICE_SVC_PARAM_STR(parms->op[0], X_BROADCOM_COM_ModuleLogLevels, setVal, NULL );

   return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetManagementProtocol
**
**  PURPOSE:       Track Protocol used to Manage Voice
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
    CmsRet ret = CMSRET_SUCCESS;
    int     vpInst;

    /* ssk code will call this function with NULL in parms */
    if( parms == NULL ) /* get first default voice service instance */
    {
        mapSpNumToVpInst( 0, &vpInst );
    }
    else
    {
        vpInst = parms->op[0];
    }

    SET_VOICE_SVC_PARAM_STR( vpInst, X_BROADCOM_COM_ManagementProtocol, setVal, protocol_valid_string );

    return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCCTKTraceLevel
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;

    SET_VOICE_SVC_PARAM_STR(parms->op[0], X_BROADCOM_COM_CCTKTraceLevel, setVal, cctk_trace_level_valid_string );

    return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCCTKTraceGroup
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;

    SET_VOICE_SVC_PARAM_STR(parms->op[0], X_BROADCOM_COM_CCTKTraceGroup, setVal, NULL );

    return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetBoundIfName
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;

    SET_VOICE_SVC_PARAM_STR(parms->op[0], X_BROADCOM_COM_BoundIfName, setVal, NULL );

    return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetBoundIPAddr
**
**  PURPOSE:       Stores the specified bound IP address in MDM.
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
    CmsRet ret = CMSRET_SUCCESS;

    SET_VOICE_SVC_PARAM_STR(parms->op[0], X_BROADCOM_COM_BoundIpAddr, setVal, NULL );

    return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetIpFamily
**
**  PURPOSE:       Stores the specified IP family list for voice
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
    CmsRet ret = CMSRET_SUCCESS;
    int     vpInst;

   /* because ssk will call this function without correct parameters 
     * using default value instead of passed in value.
     */
    if( parms == NULL )
    {
        mapSpNumToVpInst( 0, &vpInst );
    }
    else
    {
        vpInst = parms->op[0];
    }

#if VOICE_IPV6_SUPPORT
    SET_VOICE_SVC_PARAM_STR(vpInst, X_BROADCOM_COM_IpAddressFamily, setVal, NULL );
#else
    SET_VOICE_SVC_PARAM_STR(vpInst, X_BROADCOM_COM_IpAddressFamily, MDMVS_IPV4, NULL );
#endif /* VOICE_IPV6_SUPPORT */

    return (ret);

}

/*****************************************************************
**  FUNCTION:       dalVoice_SetPktcMtaDevFQDN
**
**  PURPOSE:        Stores the MTA FQDN in MDM.
**
**  INPUT PARMS:    MTA FQDN for voice
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetPktcMtaDevFQDN( DAL_VOICE_PARMS *parms, char *setVal )
{
    CmsRet ret = CMSRET_SUCCESS;

    SET_VOICE_SVC_PARAM_STR(parms->op[0], X_BROADCOM_COM_PKTC_MtaDevFQDN, setVal, NULL );

    return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetPktcMtaDevEnabled
**
**  PURPOSE:        Stores the MTA Enabled flag in MDM.
**
**  INPUT PARMS:    MTA Enabled flag for voice
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetPktcMtaDevEnabled( DAL_VOICE_PARMS *parms, UBOOL8 setVal )
{
    CmsRet ret = CMSRET_SUCCESS;
    VoiceObject *obj=NULL;
    MdmObjectId __oid = MDMOID_VOICE;

    SET_L4OBJ_PARAM_BOOL( parms->op[0], 0, 0, 0, X_BROADCOM_COM_PKTC_MtaDevEnabled, setVal);

    return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetPktcMtaDevProvisioningState
**
**  PURPOSE:        Stores the MTA Provisioning State in MDM.
**
**  INPUT PARMS:    MTA Provisioning State for voice
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetPktcMtaDevProvisioningState( DAL_VOICE_PARMS *parms, unsigned int setVal )
{
    CmsRet ret = CMSRET_SUCCESS;
    VoiceObject *obj=NULL;
    MdmObjectId __oid = MDMOID_VOICE;

    SET_L4OBJ_PARAM_UINT( parms->op[0], 0, 0, 0, X_BROADCOM_COM_PKTC_MtaDevProvisioningState, setVal, 0);

    return (ret);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetMaxLinesPerVoiceProfile
**
**  PURPOSE:       Sets max no. of lines that can be configured, as int
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetMaxLinesPerVoiceProfile( DAL_VOICE_PARMS *parms, unsigned int value )
{
    CmsRet   ret = CMSRET_SUCCESS;
#ifdef DMP_CALLCONTROL_1
    SET_VOICE_CAP_PARAM_INT(parms->op[0], parms->op[1], maxLineCount, value, 65536);
#endif
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetMaxExtPerVoiceProfile
**
**  PURPOSE:       Sets max no. of extensions that can be configured, as int
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetMaxExtPerVoiceProfile( DAL_VOICE_PARMS *parms, unsigned int value )
{
    CmsRet   ret = CMSRET_SUCCESS;
#ifdef DMP_CALLCONTROL_1
    SET_VOICE_CAP_PARAM_INT(parms->op[0], parms->op[1], maxExtensionCount, value, 65536);
#endif
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetMaxSessionsPerLine
**
**  PURPOSE:       Sets max no. of sessions per line, as int
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetMaxSessionsPerLine( DAL_VOICE_PARMS *parms, unsigned int value )
{
    CmsRet   ret = CMSRET_SUCCESS;
#ifdef DMP_CALLCONTROL_1
    SET_VOICE_CAP_PARAM_INT(parms->op[0], parms->op[1], maxSessionsPerExtension, value, 65536);
#endif
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetMaxSessionCount
**
**  PURPOSE:       Sets max no. of sessions supported across all lines, as int
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetMaxSessionCount( DAL_VOICE_PARMS *parms, unsigned int value )
{
    CmsRet   ret = CMSRET_SUCCESS;
#ifdef DMP_CALLCONTROL_1
    SET_VOICE_CAP_PARAM_INT(parms->op[0], parms->op[1], maxSessionCount, value, 65536);
#endif
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetDnsServerAddr
**
**  PURPOSE:       Set IP address of the voice DNS server
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
   CmsRet ret = CMSRET_SUCCESS;
   SET_VOICE_SVC_PARAM_STR( parms->op[0], X_BROADCOM_COM_VoiceDnsServer, setVal, NULL);
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
    CmsRet   ret = CMSRET_SUCCESS;
    VoiceServicePotsObject *obj=NULL;
    MdmObjectId           __oid = MDMOID_VOICE_SERVICE_POTS;
    char     country[TEMP_CHARBUF_SIZE];

    if( parms == NULL || value == NULL || strlen( value ) > 3 || strlen(value) < 2 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    cmsLog_debug("%s() locale = (%s)\n", __FUNCTION__, value );
    memset( country, 0, sizeof(country));
    if( strlen(value) == 2 )
    {
        snprintf( country, sizeof(country), "%s", value );
        ret = rutVoice_validateAlpha2Locale( country );
    }
    else if( strlen( value ) == 3 )
    {
        ret = rutVoice_mapAlpha3toAlpha2Locale((const char *) value, country, sizeof(country));
    }

    if( ret == CMSRET_SUCCESS )
    {
        SET_L1OBJ_PARAM_STR( parms->op[0], region, country, NULL);
    }

    return ret;
}

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
   CmsRet ret = CMSRET_SUCCESS;
   InstanceIdStack     iidStack = EMPTY_INSTANCE_ID_STACK;
   CallControlObject    *obj    = NULL;

   /*  Get the Voice Profile object */
   if ( (ret = getObject(MDMOID_CALL_CONTROL, parms->op[0], 0, 0, 0, OGF_NO_VALUE_UPDATE, &iidStack, (void**)&obj)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't retrieve voice call control  object \n" );
      return ( ret );
   }

   /* set the new value in local copy, after freeing old memory */
   REPLACE_STRING_IF_NOT_EQUAL( obj->X_BROADCOM_COM_CCTK_DigitMap, value );

   cmsLog_debug( "Digit Map = %s for [voice service idx] = [%u]\n", value, parms->op[0] );
   /* copy new value from local copy to MDM */
   if ( ( ret = cmsObj_set( obj,  &iidStack)) != CMSRET_SUCCESS )
   {
      cmsLog_error( "Can't set Digit map ret = %d\n", ret);
   }

   cmsObj_free((void **) &obj);

   return ret;
}

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
   CmsRet ret = CMSRET_SUCCESS;
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
   CmsRet ret = CMSRET_SUCCESS;
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
   CmsRet ret = CMSRET_SUCCESS;
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], STUNServer, value, NULL);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], X_BROADCOM_COM_STUNServerPort, value, 65535);
    return ret;
}
#endif /* STUN_CLIENT */

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
   CmsRet ret = CMSRET_SUCCESS;
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
   CmsRet ret = CMSRET_SUCCESS;
   return ret;
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

#define GET_FXO_LINE_PARAM_BOOL(i, p, n, v, l)  \
{                                               \
    POTSFxoObject *obj=NULL;                    \
    MdmObjectId   __oid=MDMOID_POTS_FXO;        \
    GET_L2OBJ_PARAM_BOOL( i , p, n, v, l);      \
}

#define GET_FXS_LINE_PARAM_BOOL(i, p, n, v, l)  \
{                                               \
    POTSFxsObject *obj=NULL;                    \
    MdmObjectId   __oid=MDMOID_POTS_FXS;        \
    GET_L2OBJ_PARAM_BOOL( i , p, n, v, l);      \
}

#define GET_FXS_LINE_PARAM_STR(i, p, n, v, l)   \
{                                               \
    POTSFxsObject *obj=NULL;                    \
    MdmObjectId    __oid=MDMOID_POTS_FXS;       \
    GET_L2OBJ_PARAM_STR( i , p, n, v, l);       \
}

#define SET_FXS_LINE_PARAM_BOOL(i, p, n, v)     \
{                                               \
    POTSFxsObject *obj=NULL;                    \
    MdmObjectId   __oid=MDMOID_POTS_FXS;        \
    SET_L2OBJ_PARAM_BOOL(i, p, n, v);           \
}

#define SET_FXS_LINE_PARAM_STR(i, p, n, v, f)   \
{                                               \
    POTSFxsObject *obj=NULL;                    \
    MdmObjectId   __oid=MDMOID_POTS_FXS;        \
    SET_L2OBJ_PARAM_STR(i, p, n, v, f);         \
}

#define GET_FXS_DIAG_PARAM_STR(i, p, n, v, l)   \
{                                               \
    DiagTestsObject *obj=NULL;                  \
    MdmObjectId    __oid=MDMOID_DIAG_TESTS;     \
    GET_L2OBJ_PARAM_STR( i , p, n, v, l);       \
}

#define SET_FXS_DIAG_PARAM_STR(i, p, n, v, f)   \
{                                               \
    DiagTestsObject *obj=NULL;                  \
    MdmObjectId    __oid=MDMOID_DIAG_TESTS;     \
    SET_L2OBJ_PARAM_STR(i, p, n, v, f);         \
}

/****************************************************************************
* FUNCTION:    dalVoice_SetVoiceFxsLineEnable
*
* PURPOSE:     Set enable value for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     true if enabled, false otherwise
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetVoiceFxsLineEnable( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_FXS_LINE_PARAM_BOOL(parms->op[0], parms->op[1], enable, value);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetVoiceFxsLineStatus
*
* PURPOSE:     Set status value for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     true if enabled, false otherwise
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetVoiceFxsLineStatus( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_FXS_LINE_PARAM_STR(parms->op[0], parms->op[1], status, value, NULL);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetVoiceFxsLineFaxPass
*
* PURPOSE:     Set fax passthrough config for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     string to indicate fax config, as per TR-104v2 spec
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetVoiceFxsLineFaxPass( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_FXS_LINE_PARAM_STR(parms->op[0], parms->op[1], faxPassThrough, value, NULL);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetVoiceFxsLineModemPass
*
* PURPOSE:     Set modem passthrough config for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     string to indicate modem config, as per TR-104v2 spec
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetVoiceFxsLineModemPass( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_FXS_LINE_PARAM_STR(parms->op[0], parms->op[1], modemPassThrough, value, NULL);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetVoiceFxsLineDialType
*
* PURPOSE:     Set dial type config for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     string to indicate dial type config, as per TR-104v2 spec
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetVoiceFxsLineDialType( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_FXS_LINE_PARAM_STR(parms->op[0], parms->op[1], dialType, value, NULL);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetVoiceFxsLineClipGen
*
* PURPOSE:     Set dial type config for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     string to indicate whether CLIP is generated by the board
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetVoiceFxsLineClipGen( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_FXS_LINE_PARAM_BOOL(parms->op[0], parms->op[1], clipGeneration, value);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetVoiceFxsLineTermType
*
* PURPOSE:     Set terminal type config for the given FXS line
*
* PARAMETERS:  None
*
* RETURNS:     string to indicate the terminal type
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetVoiceFxsLineTermType( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   return ret;
}


/****************************************************************************
* FUNCTION:    dalVoice_SetFxsLineTest
*
* PURPOSE:     Wrapper of set diag test selector and test state
*
* PARAMETERS:  None
*
* RETURNS:     string to indicate the terminal type
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetFxsLineTest( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;
   char     cmd[20];
    
   if( !cmsUtl_strcmp(value, "hazardv"))
   {
      sprintf(cmd, "%s", MDMVS_HAZARD_POTENTIAL);
   }
   else if( !cmsUtl_strcmp(value, "foreignv"))
   {
      sprintf(cmd, "%s", MDMVS_FOREIGN_VOLTAGE);
   }
   else if( !cmsUtl_strcmp(value, "resistance"))
   {
      sprintf(cmd, "%s", MDMVS_RESISTIVE_FAULTS);
   }
   else if( !cmsUtl_strcmp(value, "offhook"))
   {
      sprintf(cmd, "%s", MDMVS_OFF_HOOK);
   }
   else if( !cmsUtl_strcmp(value, "REN"))
   {
      sprintf(cmd, "%s", MDMVS_REN);
   }
   else
   {
       return CMSRET_INVALID_ARGUMENTS;
   }

   SET_FXS_DIAG_PARAM_STR(parms->op[0], parms->op[1], testSelector, cmd, NULL);

   if(ret == CMSRET_SUCCESS )
   {
       SET_FXS_DIAG_PARAM_STR(parms->op[0], parms->op[1], diagnosticsState, MDMVS_REQUESTED, NULL);
   }

   return ret;
}


/****************************************************************************
* FUNCTION:    dalVoice_SetFxsDiagTestSelector
*
* PURPOSE:     Set FXS line test case selection
*
* PARAMETERS:  None
*
* RETURNS:     string to indicate the terminal type
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetFxsDiagTestSelector( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_FXS_DIAG_PARAM_STR(parms->op[0], parms->op[1], testSelector, value, NULL);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetFxsDiagTestState
*
* PURPOSE:     Set FXS line test request and status
*
* PARAMETERS:  None
*
* RETURNS:
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetFxsDiagTestState( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_FXS_DIAG_PARAM_STR(parms->op[0], parms->op[1], diagnosticsState, value, NULL);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetFxsDiagTestResult
*
* PURPOSE:     Set FXS line test result
*
* PARAMETERS:  None
*
* RETURNS:
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetFxsDiagTestResult( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_FXS_DIAG_PARAM_STR(parms->op[0], parms->op[1], testResult, value, NULL);

   return ret;
}


/****************************************************************************
* FUNCTION:    dalVoice_GetFxsDiagTestSelector
*
* PURPOSE:     Set FXS line test case selection
*
* PARAMETERS:  None
*
* RETURNS:     string to indicate the terminal type
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetFxsDiagTestSelector( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_FXS_DIAG_PARAM_STR(parms->op[0], parms->op[1], testSelector, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetFxsDiagTestState
*
* PURPOSE:     Set FXS line test request and status
*
* PARAMETERS:  None
*
* RETURNS:
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetFxsDiagTestState( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_FXS_DIAG_PARAM_STR(parms->op[0], parms->op[1], diagnosticsState, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetFxsDiagTestResult
*
* PURPOSE:     Set FXS line test result
*
* PARAMETERS:  None
*
* RETURNS:
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetFxsDiagTestResult( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_FXS_DIAG_PARAM_STR(parms->op[0], parms->op[1], testResult, value, length);

   return ret;
}


/***********************************************************************
****           Voice Processing Interface                           ****
************************************************************************/

#define SET_FXS_VOICE_PROC_PARAM_SINT(i, p, n, v, f)\
{                                                   \
    VoiceProcessingObject *obj=NULL;                \
    MdmObjectId    __oid = MDMOID_VOICE_PROCESSING; \
    SINT32            __value = 0;                  \
    char             *__value_str = v;              \
    if(__value_str!=NULL&&strlen(__value_str)>0){   \
        __value = atoi(__value_str);                \
    }                                               \
    SET_L2OBJ_PARAM_SINT(i, p, n, __value, f);      \
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
    CmsRet ret = CMSRET_SUCCESS;
    VoiceProcessingObject *obj=NULL;
    MdmObjectId    __oid = MDMOID_VOICE_PROCESSING;
    SINT32            __gainvalue = 0;
    double            __value = 0.0;

    if(value!=NULL&&strlen(value)>0){
        __value = atof(value);
    }

    __gainvalue = (SINT32) (__value * GAINUNIT);
    SET_L2OBJ_PARAM_SINT(parms->op[0], parms->op[1], receiveGain, __gainvalue, 0xFF);
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
    CmsRet ret = CMSRET_SUCCESS;
    VoiceProcessingObject *obj=NULL;
    MdmObjectId    __oid = MDMOID_VOICE_PROCESSING;
    SINT32            __gainvalue = 0;
    double            __value = 0.0;

    if(value!=NULL&&strlen(value)>0){
        __value = atof(value);
    }

    __gainvalue = (SINT32) (__value * GAINUNIT);
    SET_L2OBJ_PARAM_SINT(parms->op[0], parms->op[1], transmitGain, __gainvalue, 0xFF);
    return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetVoiceFxsLineTxGain
*
* PURPOSE:     Set TX gain for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     TX gain for the given FXS line
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetVoiceFxsLineTxGain( DAL_VOICE_PARMS *parms, int txGain)
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_FXS_VOICE_PROC_PARAM_INT(parms->op[0], parms->op[1], transmitGain, txGain);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetVoiceFxsLineRxGain
*
* PURPOSE:     Set RX gain for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     RX gain for the given FXS line
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetVoiceFxsLineRxGain( DAL_VOICE_PARMS *parms, int rxGain)
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_FXS_VOICE_PROC_PARAM_INT(parms->op[0], parms->op[1], receiveGain, rxGain);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetVoiceFxsLineTxGainStr
*
* PURPOSE:     Set TX gain for the given FXS line, in string form
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetVoiceFxsLineTxGainStr( DAL_VOICE_PARMS *parms, char *txGain)
{
   int txGainInt = atoi(txGain);

   return dalVoice_SetVoiceFxsLineTxGain(parms, txGainInt);
}

/****************************************************************************
* FUNCTION:    dalVoice_SetVoiceFxsLineRxGainStr
*
* PURPOSE:     Set RX gain for the given FXS line, in string form
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetVoiceFxsLineRxGainStr( DAL_VOICE_PARMS *parms, char *rxGain)
{
   int rxGainInt = atoi(rxGain);

   return dalVoice_SetVoiceFxsLineRxGain(parms, rxGainInt);
}

/****************************************************************************
* FUNCTION:    dalVoice_SetWanPortRange
*
* PURPOSE:     Set the range of ports used by WAN interface
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list WAN port ranges
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetWanPortRange( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_RESPORT_CAP_PARAM_STR(parms->op[0], parms->op[1], WANPortRange, value, NULL);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_SetLanPortRange
*
* PURPOSE:     Set the range of ports used by LAN interface
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list LAN port ranges
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_SetLanPortRange( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_RESPORT_CAP_PARAM_STR(parms->op[0], parms->op[1], LANPortRange, value, NULL);

   return ret;
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
   CmsRet ret = CMSRET_SUCCESS;
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
   CmsRet ret = CMSRET_SUCCESS;
   return ret;
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
   return CMSRET_SUCCESS;
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
* Function Name: dalVoice_SetPstnRouteData
* Description  : Set the PSTN outgoing routing data
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetPstnRouteData( DAL_VOICE_PARMS *parms, char *value )
{
   return CMSRET_SUCCESS;
}

#ifdef DMP_BASELINE_1
#endif  /* DMP_BASELINE_1 */

/*<END>===================================== DAL Set functions =======================================<END>*/

/*<START>==================================== DAL Get Functions ====================================<START>*/

CmsRet dalVoice_GetNumAccPerSrvProv( int srvProvNum, int * numAcc )
{
    CmsRet   ret = CMSRET_SUCCESS;
    DAL_VOICE_PARMS  parms;
    int     Inst;

    ret = dalVoice_mapSpNumToSvcInst( srvProvNum, &Inst );
    if ( ret == CMSRET_SUCCESS){
        parms.op[0] = Inst;
        return dalVoice_GetNumSipClient( &parms, numAcc );
    }

    return ret;
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetNumSipClient
**
**  PURPOSE:        returns total accounts per specific serviceprovider
**                  ( i.e. corresponds to number of clients per specific voice network instance )
**
**  INPUT PARMS:    op[0] - SIP service instnace;
**                  op[1] - SIP network instnace;
**
**  OUTPUT PARMS:   Number of accounts per this service provider (num vplines per vp)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumSipClient( DAL_VOICE_PARMS *parms, int *numAcc )
{
    CmsRet   ret = CMSRET_SUCCESS;

    *numAcc = 0;
    GET_SIP_OBJ_PARAM_UINT( parms->op[0], clientNumberOfEntries, numAcc );

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumSipClientStr
**
**  PURPOSE:        returns total accounts per specific service provider, in string form
**                  ( i.e. corresponds to number of clients per specific voice network instance )
**
**  INPUT PARMS:    op[0] - SIP service instnace;
**                  op[1] - SIP network instnace;
**
**  OUTPUT PARMS:   Number of accounts per this service provider (num vplines per vp)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumSipClientStr( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
    int numAcc;
    CmsRet  ret = dalVoice_GetNumSipClient(parms, &numAcc);

    if (ret == CMSRET_SUCCESS)
    {
       sprintf(value, "%d", numAcc);
    }

    return ( ret );
}

#ifdef STUN_CLIENT
/*****************************************************************
**  FUNCTION:       dalVoice_GetSTUNServer
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;

    GET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], STUNServer, stunServer, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSTUNServerPort
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;

    GET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], X_BROADCOM_COM_STUNServerPort, port, length);

    return ( ret );
}
#endif /* STUN_CLIENT */

/****************************************************************************
* FUNCTION:    dalVoice_GetMaxCallLogCount
*
* PURPOSE:     Get maximum call log instance number
*
* PARAMETERS:  None
*
* RETURNS:     maximum call log instance number
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetMaxCallLogCount( DAL_VOICE_PARMS *parms, int* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_VOICE_CAP_PARAM_UINT(parms->op[0], maxCallLogCount, value);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetToneFileFormats
*
* PURPOSE:     Get supported tone file formats
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated tone file formats
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetToneFileFormats( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_VOICE_CAP_PARAM_STR(parms->op[0], parms->op[1], toneFileFormats, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetRingFileFormats
*
* PURPOSE:     Get supported ring file formats
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated ring file formats
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetRingFileFormats( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_VOICE_CAP_PARAM_STR(parms->op[0], parms->op[1], ringFileFormats, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetFacilityActions
*
* PURPOSE:     Get facility actions supported by this voice service instance
*
* PARAMETERS:  None
*
* RETURNS:     String of facility actions supported by this voice service instance
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetFacilityActions( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_VOICE_CAP_PARAM_STR(parms->op[0], parms->op[1], facilityActions, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetExtensions
*
* PURPOSE:     Get supported SIP extensions
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list of SIP extensions
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetExtensions( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_SIP_CLIENT_CAP_PARAM_STR(parms->op[0], parms->op[1], extensions, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetURISchemes
*
* PURPOSE:     Get supported URI schemes
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list of URI schemes
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetURISchemes( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_SIP_CLIENT_CAP_PARAM_STR(parms->op[0], parms->op[1], URISchemes, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetEventTypes
*
* PURPOSE:     Get supported SIP event types
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list of SIP event types
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetEventTypes( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_SIP_CLIENT_CAP_PARAM_STR(parms->op[0], parms->op[1], eventTypes, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetTLSAuthenticationProtocols
*
* PURPOSE:     Get supported TLS Authentication Protocols
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list of TLS Authentication Protocols
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetTLSAuthenticationProtocols( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_SIP_CLIENT_CAP_PARAM_STR(parms->op[0], parms->op[1], TLSAuthenticationProtocols, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetTLSAuthenticationKeySizes
*
* PURPOSE:     Get supported TLS Authentication key sizes
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list of TLS Authentication key sizes
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetTLSAuthenticationKeySizes( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_SIP_CLIENT_CAP_PARAM_STR(parms->op[0], parms->op[1], TLSAuthenticationKeySizes, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetTLSEncryptionProtocols
*
* PURPOSE:     Get supported TLS Encryption Protocols
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list of TLS Encryption Protocols
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetTLSEncryptionProtocols( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_SIP_CLIENT_CAP_PARAM_STR(parms->op[0], parms->op[1], TLSEncryptionProtocols, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetTLSEncryptionKeySizes
*
* PURPOSE:     Get supported TLS Encryption key sizes
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list of TLS Encryption key sizes
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetTLSEncryptionKeySizes( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_SIP_CLIENT_CAP_PARAM_STR(parms->op[0], parms->op[1], TLSEncryptionKeySizes, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetTLSKeyExchangeProtocols
*
* PURPOSE:     Get supported TLS key exchange protocols
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list of TLS key exchange protocols
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetTLSKeyExchangeProtocols( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_SIP_CLIENT_CAP_PARAM_STR(parms->op[0], parms->op[1], TLSKeyExchangeProtocols, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetPotsDialType
*
* PURPOSE:     Get dial type from POTS capabilities
*
* PARAMETERS:  None
*
* RETURNS:     Supported dial type
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetPotsDialType( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_POTS_CAP_PARAM_STR(parms->op[0], parms->op[1], dialType, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetPotsClipGeneration
*
* PURPOSE:     Get CLIP generation from POTS capabilities
*
* PARAMETERS:  None
*
* RETURNS:     true if CLIP generation is supported, false otherwise
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetPotsClipGeneration( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_POTS_CAP_PARAM_BOOL(parms->op[0], parms->op[1], clipGeneration, value);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetPotsChargingPulse
*
* PURPOSE:     Get charging pulse support value from POTS capabilities
*
* PARAMETERS:  None
*
* RETURNS:     true if pulse charging is supported, false otherwise
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetPotsChargingPulse( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_POTS_CAP_PARAM_BOOL(parms->op[0], parms->op[1], chargingPulse, value);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetMaxQIValues
*
* PURPOSE:     Get the maximum number of QI values which can be reported for a session
*
* PARAMETERS:  None
*
* RETURNS:     maximum number of QI values which can be reported for a session
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetMaxQIValues( DAL_VOICE_PARMS *parms, unsigned int* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_QI_CAP_PARAM_INT(parms->op[0], parms->op[1], maxQIValues, value);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetMaxWorstQIValues
*
* PURPOSE:     Get the maximum number of worst QI values which can be reported
*
* PARAMETERS:  None
*
* RETURNS:     maximum number of worst QI values that CPE can store and report
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetMaxWorstQIValues( DAL_VOICE_PARMS *parms, unsigned int* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_QI_CAP_PARAM_INT(parms->op[0], parms->op[1], maxWorstQIValues, value);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetWanPortRange
*
* PURPOSE:     Get the range of ports used by WAN interface
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list WAN port ranges
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetWanPortRange( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_RESPORT_CAP_PARAM_STR(parms->op[0], parms->op[1], WANPortRange, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetLanPortRange
*
* PURPOSE:     Get the range of ports used by LAN interface
*
* PARAMETERS:  None
*
* RETURNS:     Comma-separated list LAN port ranges
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetLanPortRange( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_RESPORT_CAP_PARAM_STR(parms->op[0], parms->op[1], LANPortRange, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoiceFxsLineEnable
*
* PURPOSE:     Get enable value for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     true if enabled, false otherwise
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoiceFxsLineEnable( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_FXS_LINE_PARAM_BOOL(parms->op[0], parms->op[1], enable, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoiceFxsLineStatus
*
* PURPOSE:     Get status for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     string to indicate line status, as per TR-104v2 spec
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoiceFxsLineStatus( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_FXS_LINE_PARAM_STR(parms->op[0], parms->op[1], status, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoiceFxsLineFaxPass
*
* PURPOSE:     Get fax passthrough config for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     string to indicate fax config, as per TR-104v2 spec
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoiceFxsLineFaxPass( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_FXS_LINE_PARAM_STR(parms->op[0], parms->op[1], faxPassThrough, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoiceFxsLineModemPass
*
* PURPOSE:     Get modem passthrough config for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     string to indicate modem config, as per TR-104v2 spec
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoiceFxsLineModemPass( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_FXS_LINE_PARAM_STR(parms->op[0], parms->op[1], modemPassThrough, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoiceFxsLineDialType
*
* PURPOSE:     Get dial type config for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     string to indicate dial type config, as per TR-104v2 spec
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoiceFxsLineDialType( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_FXS_LINE_PARAM_STR(parms->op[0], parms->op[1], dialType, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoiceFxsLineClipGen
*
* PURPOSE:     Get dial type config for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     string to indicate whether CLIP is generated by the board
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoiceFxsLineClipGen( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_FXS_LINE_PARAM_BOOL(parms->op[0], parms->op[1], clipGeneration, value, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoiceFxsLineTermType
*
* PURPOSE:     Get terminal type config for the given FXS line
*
* PARAMETERS:  None
*
* RETURNS:     string to indicate the terminal type
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoiceFxsLineTermType( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   strncpy(value, "Any", length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoiceFxsLineTxGain
*
* PURPOSE:     Get TX gain for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     TX gain for the given FXS line
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoiceFxsLineTxGain( DAL_VOICE_PARMS *parms, int* txGain)
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_FXS_VOICE_PROC_PARAM_INT(parms->op[0], parms->op[1], transmitGain, txGain);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoiceFxsLineRxGain
*
* PURPOSE:     Get RX gain for the given FXS line
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     RX gain for the given FXS line
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoiceFxsLineRxGain( DAL_VOICE_PARMS *parms, int* rxGain)
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_FXS_VOICE_PROC_PARAM_INT(parms->op[0], parms->op[1], receiveGain, rxGain);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoiceFxsLineTxGainStr
*
* PURPOSE:     Get TX gain for the given FXS line, in string format
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     TX gain for the given FXS line
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoiceFxsLineTxGainStr( DAL_VOICE_PARMS *parms, char* transmitGain, unsigned int length)
{
   int   txGain = 0;
   CmsRet   ret;

   ret = dalVoice_GetVoiceFxsLineTxGain(parms, &txGain);
   if (ret == CMSRET_SUCCESS)
   {
      sprintf(transmitGain, "%d", txGain/GAINUNIT);
   }
   else
   {
      cmsLog_debug("Failed to obtain TX gain");
   }

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoiceFxsLineRxGainStr
*
* PURPOSE:     Get RX gain for the given FXS line, in string format
*
* PARAMETERS:  Voice profile and FXS line #
*
* RETURNS:     RX gain for the given FXS line
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoiceFxsLineRxGainStr( DAL_VOICE_PARMS *parms, char* receiveGain, unsigned int length)
{
   int   rxGain = 0;
   CmsRet   ret;

   ret = dalVoice_GetVoiceFxsLineRxGain(parms, &rxGain);
   if (ret == CMSRET_SUCCESS)
   {
      sprintf(receiveGain, "%d", rxGain/GAINUNIT);
   }
   else
   {
      cmsLog_debug("Failed to obtain RX gain");
   }

   return ret;
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetCriticalDigitTimer
**
**  PURPOSE:       Return value of
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
   return ( CMSRET_SUCCESS );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetPartialDigitTimer
**
**  PURPOSE:       Return value of
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
   return ( CMSRET_SUCCESS );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetIpAddr
**
**  PURPOSE:
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
**  PURPOSE:
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
**  PURPOSE:
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
    CmsRet   ret = CMSRET_SUCCESS;
    VoiceServicePotsObject *obj=NULL;
    MdmObjectId           __oid = MDMOID_VOICE_SERVICE_POTS;
    char             localeAlpha3[TEMP_CHARBUF_SIZE];
    char             localeAlpha2[TEMP_CHARBUF_SIZE];

    GET_L1OBJ_PARAM_STR( parms->op[0], region, localeAlpha2, sizeof(localeAlpha2));
    if( ret == CMSRET_SUCCESS && strlen(localeAlpha2) > 0 )
    {
        cmsLog_debug( "region = %s\n ", localeAlpha2);
        memset(localeAlpha3, 0, sizeof(localeAlpha3));
        ret = rutVoice_mapAlpha2toAlpha3Locale( (const char*) localeAlpha2, localeAlpha3, sizeof(localeAlpha3));
        if( ret == CMSRET_SUCCESS )
        {
            strncpy( (char *)country, localeAlpha3, length);
        }
        else{
            cmsLog_error( "Unknow region = %s\n", localeAlpha2 );
        }
    }

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRegionVrgCode
**
**  PURPOSE:
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
    CmsRet      ret ;
    UBOOL8      found;
    char               localeAlpha2[TEMP_CHARBUF_SIZE];
    int                countryInt;
    VoiceServicePotsObject *obj=NULL;
    MdmObjectId           __oid = MDMOID_VOICE_SERVICE_POTS;

    memset( localeAlpha2, 0, TEMP_CHARBUF_SIZE);
    GET_L1OBJ_PARAM_STR( parms->op[0], region, localeAlpha2, sizeof(localeAlpha2));
    if( ret == CMSRET_SUCCESS && strlen(localeAlpha2) > 0 )
    {
        if( dalVoice_mapAlpha2toVrg( localeAlpha2, &countryInt, &found, TEMP_CHARBUF_SIZE ) != CMSRET_SUCCESS )
        {
            cmsLog_error( "Unknown region = %s\n", localeAlpha2 );
        }
        else
        {
            snprintf( (char*)country, length, "%d",countryInt );
        }
    }

    return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRegionSuppString
**
**  PURPOSE:
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

/****************************************************************
***                 VoIP Profile Interface                    ***
*****************************************************************/
#define GET_VOIP_PROFILE_PARAM_STR(i, p, n, v, l)   \
{                                                   \
    VoIPProfileObject *obj=NULL;                    \
    MdmObjectId      __oid=MDMOID_IP_PROFILE;       \
    GET_L2OBJ_PARAM_STR( i , p, n, v, l);           \
}

#define GET_VOIP_PROFILE_PARAM_BOOL(i, p, n, v, l)  \
{                                                   \
    VoIPProfileObject *obj=NULL;                    \
    MdmObjectId      __oid=MDMOID_IP_PROFILE;       \
    GET_L2OBJ_PARAM_BOOL( i , p, n, v, l);          \
}

#define GET_RTP_PARAM_UINT(i, p, n, v, l)           \
{                                                   \
    VoIPProfileRTPObject *obj = NULL;               \
    MdmObjectId    __oid = MDMOID_IP_PROFILE_R_T_P; \
    GET_L2OBJ_PARAM_UINT( i , p, n, v, l);          \
}


#define SET_RTP_PARAM_UINT(i, p, n, v, f)           \
{                                                   \
    VoIPProfileRTPObject *obj = NULL;               \
    MdmObjectId    __oid = MDMOID_IP_PROFILE_R_T_P; \
    UINT32            __value = 0;                  \
    char             *__value_str = v;              \
    if(__value_str!=NULL&&strlen(__value_str)>0){   \
        __value = atoi(__value_str);                \
    }                                               \
    SET_L2OBJ_PARAM_UINT(i, p, n, __value, f);      \
}

/*****************************************************************
**  FUNCTION:       dalVoice_MapVoIPProfileNameToVoIPProfileInst
**
**  PURPOSE:        maps a VoIP profile name to VoIP profile instance
**
**  INPUT PARMS:    vpInst - parms->op[0],
**                  name   - name of the codec to be mapped
**
**  OUTPUT PARMS:   inst - instance of the mapped VOIP profile
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_MapVoIPProfileNameToVoIPProfileInst( DAL_VOICE_PARMS *parms, const char *name, int *voipProfInst )
{
    CmsRet ret =  CMSRET_SUCCESS;

    VoIPProfileObject *vpObj = NULL;
    int    numVoIPProfile = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack searchIidStack = EMPTY_INSTANCE_ID_STACK;
    UBOOL8  found = FALSE;

    if( parms == NULL || name == NULL || voipProfInst == NULL )
    {
        cmsLog_error("%s invalid value", __FUNCTION__ );
        return CMSRET_INVALID_PARAM_VALUE;
    }
    else
    {
        *voipProfInst = 0;
    }

    PUSH_INSTANCE_ID(&iidStack, parms->op[0]);
    ret = dalVoice_GetNumVoipProfile( parms, &numVoIPProfile);
    if( ret == CMSRET_SUCCESS && numVoIPProfile > 0 )
    {
        while( !found && (cmsObj_getNextInSubTreeFlags(MDMOID_IP_PROFILE, &iidStack, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **) &vpObj)) == CMSRET_SUCCESS )
        {
           if( vpObj->name && !cmsUtl_strcasecmp( name, vpObj->name ) )
           {
              *voipProfInst = PEEK_INSTANCE_ID(&searchIidStack);
              cmsLog_debug("%s found match profile ( %s ) voip profile instance (%d)", __FUNCTION__, name, *voipProfInst );
              found = TRUE;
           }

           cmsObj_free((void **)&vpObj);
        }
        if( found ){
            return CMSRET_SUCCESS;
        }
        else{
            return CMSRET_INVALID_PARAM_NAME;
        }
    }

    return CMSRET_INVALID_PARAM_VALUE;
}

/*****************************************************************
**  FUNCTION:       dalVoice_AddVoipProfile
**
**  PURPOSE:        Adds a VoIP profile object in MDM
**
**  INPUT PARAMS:   svc inst - parms->op[0]
**
**  OUTPUT PARMS:   instance of the added object
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_AddVoipProfile( DAL_VOICE_PARMS *parms, int *inst )
{
    return addL2Object( MDMOID_IP_PROFILE, parms->op[0], inst );
}

/*****************************************************************
**  FUNCTION:       dalVoice_DeleteVoipProfile
**
**  PURPOSE:        Deletes a VoIP profile object in MDM
**
**  INPUT PARAMS:   svc inst - parms->op[0]
**                  VoIP profile instance to be deleted - parms->op[1]
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_DeleteVoipProfile( DAL_VOICE_PARMS *parms )
{
    return delL2Object( MDMOID_IP_PROFILE, parms->op[0], parms->op[1] );
}

/*****************************************************************
**  FUNCTION:       dalVoice_mapVoipProfNumToInst
**
**  PURPOSE:        maps VoIP profile object number to instance
**
**  INPUT PARAMS:   svc inst - parms->op[0]
**                  VoIP profile object number - parms->op[1]
**
**  OUTPUT PARMS:   vpInst - object instance that input parameters map to
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_mapVoipProfNumToInst( DAL_VOICE_PARMS *parms, int *vpInst )
{
    return mapL2ObjectNumToInst( MDMOID_IP_PROFILE, parms->op[0], parms->op[1], vpInst);
}

/*****************************************************************
**  FUNCTION:       dalVoice_mapCodecProfNumToInst
**
**  PURPOSE:        maps Codec profile object number to instance
**
**  INPUT PARAMS:   svc inst - parms->op[0]
**                  Codec profile object number - parms->op[1]
**
**  OUTPUT PARMS:   vpInst - object instance that input parameters map to
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_mapCodecProfNumToInst( DAL_VOICE_PARMS *parms, int *vpInst )
{
    return mapL2ObjectNumToInst( MDMOID_CODEC_PROFILE, parms->op[0], parms->op[1], vpInst);
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDTMFMethod
**
**  PURPOSE:
**
**  INPUT PARAMS:   svc inst - parms->op[0]
**                  network inst - parms->op[1]
**
**  OUTPUT PARMS:   dtmfRelay - InBand
**                            - SIPINFO
**                            - RFC2833
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetDTMFMethod(DAL_VOICE_PARMS *parms, char *dtmfRelay, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;

    if( parms == NULL || dtmfRelay == NULL || length <= 0 )
        return CMSRET_INVALID_ARGUMENTS;

    GET_VOIP_PROFILE_PARAM_STR( parms->op[0], parms->op[1], DTMFMethod, dtmfRelay, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetV18Enable
**
**  PURPOSE:
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
    CmsRet   ret = CMSRET_SUCCESS;

    if( parms == NULL || enabled == NULL || length <= 0 )
        return CMSRET_INVALID_ARGUMENTS;

    GET_VOIP_PROFILE_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_V18_Enabled, enabled, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVoipProfileEnable
**
**  PURPOSE:        Obtains the enable status of a particular VOIP profile
**
**  INPUT PARMS:    op[0] - voice service instance;
**                  op[1] - voip profile  instance;
**
**  OUTPUT PARMS:   enable - enable flag
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVoipProfileEnable(DAL_VOICE_PARMS *parms, char *enable, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;

    if( parms == NULL || enable == NULL || length <= 0 )
        return CMSRET_INVALID_ARGUMENTS;

    GET_VOIP_PROFILE_PARAM_BOOL(parms->op[0], parms->op[1], enable, enable, length);

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVoipProfileName
**
**  PURPOSE:        Obtains the name of a particular VOIP profile
**
**  INPUT PARMS:    op[0] - voice service instance;
**                  op[1] - voip profile instance;
**
**  OUTPUT PARMS:   name - VOIP profile name
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVoipProfileName(DAL_VOICE_PARMS *parms, char *name, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;

    if( parms == NULL || name == NULL || length <= 0 )
        return CMSRET_INVALID_ARGUMENTS;

    GET_VOIP_PROFILE_PARAM_STR(parms->op[0], parms->op[1], name, name, length);

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRtpDSCPMark
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;

    if( parms == NULL || dscpMark == NULL || length <= 0 )
        return CMSRET_INVALID_ARGUMENTS;

    GET_RTP_PARAM_UINT(parms->op[0], parms->op[1], DSCPMark,  dscpMark, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRtpLocalPortMin
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;

    if( parms == NULL || port == NULL || length <= 0 )
        return CMSRET_INVALID_ARGUMENTS;

    GET_RTP_PARAM_UINT(parms->op[0], parms->op[1], localPortMin,  port, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRtpLocalPortMax
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;

    if( parms == NULL || port == NULL || length <= 0 )
        return CMSRET_INVALID_ARGUMENTS;

    GET_RTP_PARAM_UINT(parms->op[0], parms->op[1], localPortMax ,  port, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRtcpEnabled
**
**  PURPOSE:        Obtains the RTCP enable flag
**
**  INPUT PARMS:    op[0] - voice service instance;
**                  op[1] - voip profile  instance;
**
**  OUTPUT PARMS:   enable - RTCP enable flag
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetRtcpEnabled(DAL_VOICE_PARMS *parms, char *enable, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;
    VoIPProfileRTCPObject *obj=NULL;
    MdmObjectId          __oid=MDMOID_IP_PROFILE_R_T_C_P;

    if( parms == NULL || enable == NULL || length <= 0 )
        return CMSRET_INVALID_ARGUMENTS;

    GET_L2OBJ_PARAM_BOOL( parms->op[0], parms->op[1], enable, enable, length );

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRtcpInterval
**
**  PURPOSE:        Obtains RTCP TX interval
**
**  INPUT PARMS:    op[0] - voice service instance;
**                  op[1] - voip profile  instance;
**
**  OUTPUT PARMS:   interval - RTCP TX interval
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetRtcpInterval(DAL_VOICE_PARMS *parms, char *interval, unsigned int length )
{
    CmsRet  ret = CMSRET_SUCCESS;
    VoIPProfileRTCPObject *obj=NULL;
    MdmObjectId          __oid=MDMOID_IP_PROFILE_R_T_C_P;

    if( parms == NULL || interval == NULL || length <= 0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    GET_L2OBJ_PARAM_UINT( parms->op[0], parms->op[1], txRepeatInterval, interval, length );

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSrtpEnabled
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   true/false for whether SRTP is enabled
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSrtpEnabled(DAL_VOICE_PARMS *parms, char *enable, unsigned int length )
{
    CmsRet  ret = CMSRET_SUCCESS;
    VoIPProfileSRTPObject *obj=NULL;
    MdmObjectId  __oid = MDMOID_IP_PROFILE_S_R_T_P;

    if( parms == NULL || enable == NULL || length <= 0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    GET_L2OBJ_PARAM_BOOL(parms->op[0], parms->op[1], enable, enable, length );

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
    CmsRet  ret = CMSRET_SUCCESS;
    VoIPProfileSRTPObject *obj=NULL;
    MdmObjectId  __oid = MDMOID_IP_PROFILE_S_R_T_P;
    char    tmp[16];

    if( parms == NULL || option == NULL || length <= 0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }
    cmsLog_debug("%s() enter vp (%d) prof (%d)\n", parms->op[0], parms->op[1]);

    memset( option, 0, length );
    memset( tmp, 0, sizeof(tmp) );
    GET_L2OBJ_PARAM_BOOL(parms->op[0], parms->op[1], enable, tmp, sizeof(tmp) );
    if( ret == CMSRET_SUCCESS && strlen( tmp ) > 0 )
    {
        if( strcasecmp( tmp, MDMVS_YES ) == 0 ){
            cmsLog_debug("%s() SRTP is enabled");
            memset( tmp, 0, sizeof(tmp) );
            GET_L2OBJ_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_SrtpUsageOption, tmp, sizeof(tmp) );
            if( ret == CMSRET_SUCCESS && strlen(tmp)){
                if( strcasecmp( tmp, MDMVS_MANDATORY ) == 0 ){
                    cmsLog_debug("%s() SRTP is mandatory");
                    snprintf( option, length, "%d", DAL_VOICE_SRTP_MANDATORY);
                }
                else{
                    cmsLog_debug("%s() SRTP is optional");
                    snprintf( option, length, "%d", DAL_VOICE_SRTP_OPTIONAL);
                }
            }
            else{
                snprintf( option, length, "%d", DAL_VOICE_SRTP_OPTIONAL);
            }
        }
        else{
            cmsLog_debug("%s() SRTP is disabled");
            snprintf( option, length, "%d", DAL_VOICE_SRTP_DISABLED);
        }
    }

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
    CmsRet ret = CMSRET_SUCCESS;
    VoIPProfileObject *obj=NULL;
    MdmObjectId  __oid = MDMOID_IP_PROFILE;

    if( parms == NULL || value == NULL || strlen(value) <=0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    SET_L2OBJ_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_V18_Enabled, value);

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVoipProfEnable
* Description  : Set the "Enable" value in VOIP profile
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = profile inst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVoipProfEnable( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet ret = CMSRET_SUCCESS;
    VoIPProfileObject *obj=NULL;
    MdmObjectId  __oid = MDMOID_IP_PROFILE;

    if( parms == NULL || value == NULL || strlen(value) <=0 )
        return CMSRET_INVALID_ARGUMENTS;

    SET_L2OBJ_PARAM_BOOL(parms->op[0], parms->op[1], enable, value);

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVoipProfName
* Description  : Set the name in VOIP profile
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = profile inst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVoipProfName( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet ret = CMSRET_SUCCESS;
    VoIPProfileObject *obj=NULL;
    MdmObjectId  __oid = MDMOID_IP_PROFILE;

    if( parms == NULL || value == NULL || strlen(value) <=0 )
        return CMSRET_INVALID_ARGUMENTS;

    SET_L2OBJ_PARAM_STR(parms->op[0], parms->op[1], name, value, NULL);

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
    CmsRet ret = CMSRET_SUCCESS;
    VoIPProfileObject *obj=NULL;
    MdmObjectId  __oid = MDMOID_IP_PROFILE;

    if( parms == NULL || value == NULL || strlen(value) <=0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    SET_L2OBJ_PARAM_STR(parms->op[0], parms->op[1], DTMFMethod, value, DTMFMethod_valid_string);

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetRtpLocalPortMin
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   port - Value of RTP min port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetRtpLocalPortMin(DAL_VOICE_PARMS *parms, char *port )
{
    CmsRet ret = CMSRET_SUCCESS;

    if( parms == NULL || port == NULL )
        return CMSRET_INVALID_ARGUMENTS;

    SET_RTP_PARAM_UINT(parms->op[0], parms->op[1], localPortMin, port, 65535);

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetRtpLocalPortMax
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   port - Value of RTP max port
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetRtpLocalPortMax(DAL_VOICE_PARMS *parms, char *port )
{
    CmsRet ret = CMSRET_SUCCESS;

    if( parms == NULL || port == NULL )
        return CMSRET_INVALID_ARGUMENTS;

    SET_RTP_PARAM_UINT(parms->op[0], parms->op[1], localPortMax, port, 65535);

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetRtpDSCPMark
* Description  : Diffserv code point to be used for outgoing RTP
*                packets for this profile
*                VoiceProfile.{i}.DSCPMark = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetRtpDSCPMark( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet ret = CMSRET_SUCCESS;
    VoIPProfileRTPObject *obj=NULL;
    MdmObjectId  __oid = MDMOID_IP_PROFILE_R_T_P;
    UINT32            val = 0;

    if( parms == NULL || value == NULL || strlen(value) <=0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    val = atoi(value);

    SET_L2OBJ_PARAM_UINT(parms->op[0], parms->op[1], DSCPMark, val, 63);

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetRtcpInterval
* Description  : Sets RTCP interval in VOIP profile RTCP object
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetRtcpInterval( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet  ret = CMSRET_SUCCESS;
    VoIPProfileRTCPObject *obj=NULL;
    MdmObjectId          __oid=MDMOID_IP_PROFILE_R_T_C_P;
    UINT32               val = 0;

    if( parms == NULL || value == NULL )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    val = atoi(value);

    SET_L2OBJ_PARAM_UINT( parms->op[0], parms->op[1], txRepeatInterval, val, 20000 );

    return ( ret );
}

/***************************************************************************
* Function Name: dalVoice_SetRtcpEnable
* Description  : Sets RTCP enable value in VOIP profile RTCP object
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetRtcpEnable( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet  ret = CMSRET_SUCCESS;
    VoIPProfileRTCPObject *obj=NULL;
    MdmObjectId          __oid=MDMOID_IP_PROFILE_R_T_C_P;

    if( parms == NULL || value == NULL )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    SET_L2OBJ_PARAM_BOOL( parms->op[0], parms->op[1], enable, value );

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetSrtpEnable
**
**  PURPOSE:        Set SRTP enable flag
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetSrtpEnable(DAL_VOICE_PARMS *parms, char *enable )
{
    CmsRet ret = CMSRET_SUCCESS;
    VoIPProfileSRTPObject *obj=NULL;
    MdmObjectId  __oid = MDMOID_IP_PROFILE_S_R_T_P;

    if( parms == NULL || enable == NULL || strlen(enable) <=0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    SET_L2OBJ_PARAM_BOOL(parms->op[0], parms->op[1], enable, enable);

    return ( ret );
}

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
    CmsRet   ret = CMSRET_SUCCESS;
    VoIPProfileSRTPObject  *obj = NULL;
    MdmObjectId  __oid = MDMOID_IP_PROFILE_S_R_T_P;

    if( parms == NULL || value == NULL || strlen(value) <=0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    SET_L2OBJ_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_SrtpUsageOption, value, srtp_option_valid_string );

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetRedEnable
**
**  PURPOSE:        Set Redundancy enable flag
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetRedEnable( DAL_VOICE_PARMS *parms, char *value)
{
    CmsRet ret = CMSRET_SUCCESS;
    VoIPProfileRTPRedundancyObject *obj=NULL;
    MdmObjectId        __oid = MDMOID_IP_PROFILE_R_T_P_REDUNDANCY;

    if( parms == NULL || value == NULL || strlen(value) <=0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    SET_L2OBJ_PARAM_BOOL(parms->op[0], parms->op[1], enable, value);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetRedOptionString
**
**  PURPOSE:        Get RED option (-1,0,1,2,3,4,5)
**
**  INPUT PARMS:    parms->op[0] (voice service instance)
**                  partm->op[1] (VOIP profile instance)
**
**  OUTPUT PARMS:   String Value of RED option
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetRedOptionString( DAL_VOICE_PARMS *parms, char *option, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;
    VoIPProfileRTPRedundancyObject *obj=NULL;
    MdmObjectId        __oid = MDMOID_IP_PROFILE_R_T_P_REDUNDANCY;

    if( parms == NULL || option == NULL )
    {
        cmsLog_error( "%s: Invalid arguments\n", __FUNCTION__ );
        return CMSRET_INVALID_ARGUMENTS;
    }

    GET_L2OBJ_PARAM_UINT(parms->op[0], parms->op[1], voiceRedundancy, option, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVBDEnable
**
**  PURPOSE:
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

   /* Invert the T38 enable value to get VBD enable value */
   snprintf( (char*)enabled, length, "1" );

   return ( CMSRET_SUCCESS );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetLoggingLevel
**
**  PURPOSE:
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
    CmsRet  ret = CMSRET_SUCCESS;
    GET_VOICE_SVC_PARAM_STR(parms->op[0], X_BROADCOM_COM_LoggingLevel, getVal, length, FALSE );
    return ret;
}

/***************************************************************************
* Function Name: dalVoice_GetModuleLoggingLevels
* Description  : Gets the logging levels for all modules
*
* Parameters   : none
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetModuleLoggingLevels( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length )
{
    CmsRet  ret = CMSRET_SUCCESS;
    GET_VOICE_SVC_PARAM_STR(parms->op[0], X_BROADCOM_COM_ModuleLogLevels, getVal, MAX_TR104_OBJ_SIZE, FALSE );
    return ret;
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
    CmsRet  ret = CMSRET_SUCCESS;
    char *pMod, *pVal, *pModAfter;
    char  logLevels[MAX_TR104_OBJ_SIZE];

    if( parms == NULL || modName == NULL || getVal == NULL || length <= 0 )
        return CMSRET_INVALID_ARGUMENTS;

    memset(logLevels, 0, sizeof(logLevels));

    GET_VOICE_SVC_PARAM_STR(parms->op[0], X_BROADCOM_COM_ModuleLogLevels, logLevels, MAX_TR104_OBJ_SIZE, FALSE );
    if( ret != CMSRET_SUCCESS || strlen( logLevels ) <= 0){
        return CMSRET_INVALID_PARAM_NAME;
    }
    else{
        memset(getVal, 0, length);
    }

    /* TODO: added module debugging level parser */
    pMod = cmsUtl_strstr(logLevels, modName);
    if(!pMod)
    {
        cmsLog_debug( "Invalid module name\n" );
        return CMSRET_INVALID_PARAM_NAME;
    }

    pVal = cmsUtl_strstr(pMod, "=");
    if(!pVal)
    {
        cmsLog_debug( "Invalid value associated with module %s\n", modName );
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

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_GetManagementProtocol
* Description  : Gets the Protocol used to manage the Voice Service
*
* Parameters   : none
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetManagementProtocol( DAL_VOICE_PARMS *parms, char* getVal, unsigned int length )
{
    CmsRet  ret = CMSRET_SUCCESS;
    int     vpInst;

    /* ssk code will call this function with NULL in parms */
    if( parms == NULL ) /* get first default voice service instance */
    {
        mapSpNumToVpInst( 0, &vpInst );
    }
    else
    {
        vpInst = parms->op[0];
    }

    GET_VOICE_SVC_PARAM_STR(vpInst, X_BROADCOM_COM_ManagementProtocol, getVal, length, FALSE );

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCCTKTraceLevel
**
**  PURPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   cctk trace level
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCCTKTraceLevel( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length)
{
    CmsRet  ret = CMSRET_SUCCESS;
    int     vpInst;

    if( parms == NULL ) /* get first default voice service instance */
    {
        mapSpNumToVpInst( 0, &vpInst );
    }
    else
    {
        vpInst = parms->op[0];
    }

    GET_VOICE_SVC_PARAM_STR(vpInst, X_BROADCOM_COM_CCTKTraceLevel, getVal, length, FALSE );
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCCTKTraceGroup
**
**  PURPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   cctk trace group
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCCTKTraceGroup( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length)
{
    CmsRet  ret = CMSRET_SUCCESS;
    int     vpInst;

    if( parms == NULL ) /* get first default voice service instance */
    {
        mapSpNumToVpInst( 0, &vpInst );
    }
    else
    {
        vpInst = parms->op[0];
    }

    GET_VOICE_SVC_PARAM_STR(vpInst, X_BROADCOM_COM_CCTKTraceGroup, getVal, length, FALSE );
    return ret;
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetBoundIfName
**
**  PURPOSE:
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
   CmsRet  ret = CMSRET_SUCCESS;
   GET_VOICE_SVC_PARAM_STR( parms->op[0], X_BROADCOM_COM_BoundIfName, getVal, length, FALSE );

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetBoundIPAddr
**
**  PURPOSE:
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
   CmsRet  ret = CMSRET_SUCCESS;

   GET_VOICE_SVC_PARAM_STR( parms->op[0], X_BROADCOM_COM_BoundIpAddr, getVal, length, TRUE );

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetIpFamilyList
**
**  PURPOSE:
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
**  FUNCTION:       dalVoice_GetPktcMtaDevFQDN
**
**  PURPOSE:        Retrieves the MTA FQDN from MDM.
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   MTA FQDN for voice
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPktcMtaDevFQDN( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length )
{
   CmsRet  ret = CMSRET_SUCCESS;

   GET_VOICE_SVC_PARAM_STR( parms->op[0], X_BROADCOM_COM_PKTC_MtaDevFQDN, getVal, length, TRUE );

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPktcMtaDevEnabled
**
**  PURPOSE:        Retrieves the MTA enabled flag from MDM.
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   MTA enabled flag for voice
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPktcMtaDevEnabled( DAL_VOICE_PARMS *parms, UBOOL8 * getVal )
{
   CmsRet ret = CMSRET_SUCCESS;
   VoiceObject *obj=NULL;
   MdmObjectId __oid = MDMOID_VOICE;

   GET_L4OBJ_PARAM_BOOL( parms->op[0], 0, 0, 0, X_BROADCOM_COM_PKTC_MtaDevEnabled, getVal );

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPktcMtaDevProvisioningState
**
**  PURPOSE:        Retrieves the MTA provisioning state from MDM.
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   MTA provisioning state for voice
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPktcMtaDevProvisioningState( DAL_VOICE_PARMS *parms, unsigned int * getVal )
{
   CmsRet ret = CMSRET_SUCCESS;
   VoiceObject *obj=NULL;
   MdmObjectId __oid = MDMOID_VOICE;

   GET_L4OBJ_PARAM_UINT( parms->op[0], 0, 0, 0, X_BROADCOM_COM_PKTC_MtaDevProvisioningState, getVal);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetMgtProtList
**
**  PURPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   List of management protocols for  voice
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetMgtProtList( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length )
{
   if (getVal == NULL || length == 0)
      return CMSRET_SUCCESS;

   /* prepare list of supported IP families */
   snprintf(getVal, length, "%s ", MDMVS_TR69);

   strcat(getVal, MDMVS_OMCI);

   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSupportedDtmfMethods
**
**  PURPOSE:
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   Set of supported DTMF methods
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSupportedDtmfMethods( DAL_VOICE_PARMS *parms, char * getVal, unsigned int length )
{
   if (getVal == NULL || length == 0)
   {
      return CMSRET_SUCCESS;
   }

   /* prepare list of supported IP families */
   snprintf(getVal, length, "%s ", MDMVS_INBAND);

   strcat(getVal, MDMVS_RFC4733);
   strcat(getVal, " ");
   strcat(getVal, MDMVS_SIPINFO);

   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDnsEnable
**
**  PURPOSE:        Check if voice DNS is enabled
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   getVal - DNS enable value
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetDnsEnable(DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
#if BRCM_SIP_VOICE_DNS
   strcpy(getVal, "true");
#else
   strcpy(getVal, "false");
#endif
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDnsServerAddr
**
**  PURPOSE:       Get IP address of the voice DNS server
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
   CmsRet  ret = CMSRET_SUCCESS;
   GET_VOICE_SVC_PARAM_STR( parms->op[0], X_BROADCOM_COM_VoiceDnsServer, getVal, length, TRUE );
   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetIpFamily
**
**  PURPOSE:
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
    CmsRet  ret = CMSRET_SUCCESS;
    int     vpInst;

    /* because ssk will call this function without correct parameters 
     * using default value instead of passed in value.
     */
    if( parms == NULL )
    {
        mapSpNumToVpInst( 0, &vpInst );
    }
    else
    {
        vpInst = parms->op[0];
    }

    GET_VOICE_SVC_PARAM_STR( vpInst, X_BROADCOM_COM_IpAddressFamily, getVal, length, FALSE );

    return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetSignalingProtocol
*
* PURPOSE:     Get network signaling protocol capability
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
   CmsRet ret = CMSRET_SUCCESS;

   GET_VOICE_CAP_PARAM_STR(parms->op[0], parms->op[1], networkConnectionModes, sigProt, length);

   return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetUserSignalingProtocol
*
* PURPOSE:     Get user signaling protocol capability
*
* PARAMETERS:  None
*
* RETURNS:     Supported signalling protocols
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetUserSignalingProtocol( DAL_VOICE_PARMS *parms, char* sigProt, unsigned int length )
{
   CmsRet ret = CMSRET_SUCCESS;

   GET_VOICE_CAP_PARAM_STR(parms->op[0], parms->op[1], userConnectionModes, sigProt, length);

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
* PURPOSE:     Get list of available back-to-primary failover options
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




/*****************************************************************
**  FUNCTION:       dalVoice_GetFeatureString
**
**  PURPOSE:
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
**  FUNCTION:       dalVoice_GetHookFlashMethod
**
**  PURPOSE:
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
   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetHookFlashMethodString
**
**  PURPOSE:
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
   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetMaxPrefCodecs
**
**  PUROPOSE:
**
**  INPUT PARAMS:   None
**
**  OUTPUT PARMS:   value - maximum number of preferred codecs
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetMaxPrefCodecs( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   int maxPrefCodecs;
   rutVoice_getMaxPrefCodecs( &maxPrefCodecs );
   snprintf( value, length, "%d", maxPrefCodecs );
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSrtpOptionString
**
**  PURPOSE:        Get SRTP usage option (mandatory, optional or disabled)
**                    in string form
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
    CmsRet  ret = CMSRET_SUCCESS;
    VoIPProfileSRTPObject *obj=NULL;
    MdmObjectId  __oid = MDMOID_IP_PROFILE_S_R_T_P;
    char    tmp[16];

    if( parms == NULL || option == NULL || length <= 0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }
    cmsLog_debug("%s() enter vp (%d) prof (%d)\n", parms->op[0], parms->op[1]);

    memset( option, 0, length );
    memset( tmp, 0, sizeof(tmp) );
    GET_L2OBJ_PARAM_BOOL(parms->op[0], parms->op[1], enable, tmp, sizeof(tmp) );
    if( ret == CMSRET_SUCCESS && strlen( tmp ) > 0 )
    {
        if( strcasecmp( tmp, MDMVS_YES ) == 0 ){
            cmsLog_debug("%s() SRTP is enabled");
            memset( tmp, 0, sizeof(tmp) );
            GET_L2OBJ_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_SrtpUsageOption, tmp, sizeof(tmp) );
            if( ret == CMSRET_SUCCESS && strlen(tmp)){
                if( strcasecmp( tmp, MDMVS_MANDATORY ) == 0 ){
                    cmsLog_debug("%s() SRTP is optional");
                    snprintf( option, length, "%s", MDMVS_MANDATORY);
                }
                else {
                    /* Only Optional or Mandatory SRTP are possible when SRTP is enabled at runtime */
                    cmsLog_debug("%s() SRTP is mandatory");
                    snprintf( option, length, "%s", MDMVS_OPTIONAL);
                }
            }
            else{
                /* Default to Optional SRTP if SRTP usage option cannot be obtained */
                snprintf( option, length, "%s", MDMVS_OPTIONAL);
            }
        }
        else{
            cmsLog_debug("%s() SRTP is disabled");
            snprintf( option, length, "%s", MDMVS_DISABLED);
        }
    }

    return ret;
}
/*****************************************************************
**  FUNCTION:       dalVoice_GetVlDisable
**
**  PURPOSE:
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
   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVoicePhyInterfaceList
**
**  PURPOSE:
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
   return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlPhyReferenceList
**
**  PURPOSE:
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
   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlAssociatedNonFxsPhyType
**
**  PURPOSE:
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
   return ( CMSRET_SUCCESS );
}


/****************************************************************
**             Calling Feature Set  Interface                  **
****************************************************************/
#define SET_CALLING_FEATURES_PARAM_STR( i, p, n, v, f)\
{                                                       \
    CallingFeaturesSetObject *obj=NULL;                 \
    MdmObjectId       __oid = MDMOID_CALLING_FEATURES_SET;\
    SET_L2OBJ_PARAM_STR(i, p, n, v, f);               \
}

#define GET_CALLING_FEATURES_PARAM_STR(i, p, n, v, l)       \
{                                                           \
    CallingFeaturesSetObject *obj=NULL;                     \
    MdmObjectId       __oid = MDMOID_CALLING_FEATURES_SET;  \
    GET_L2OBJ_PARAM_STR(i, p, n, v, l);                     \
}

#define GET_CALLING_FEATURES_PARAM_BOOL(i, p, n, v, l)      \
{                                                           \
    CallingFeaturesSetObject *obj=NULL;                     \
    MdmObjectId       __oid = MDMOID_CALLING_FEATURES_SET;  \
    GET_L2OBJ_PARAM_BOOL(i, p, n, v, l);                    \
}

#define SET_CALLING_FEATURES_PARAM_BOOL(i, p, n, v)         \
{                                                           \
    CallingFeaturesSetObject *obj=NULL;                     \
    MdmObjectId       __oid = MDMOID_CALLING_FEATURES_SET;  \
    SET_L2OBJ_PARAM_BOOL(i, p, n, v);                       \
}

#define GET_CALL_CONTROL_FEATURE_PARAM_UINT(i, n, v)        \
{                                                           \
    CallControlCallingFeaturesObject *obj=NULL;                     \
    MdmObjectId       __oid = MDMOID_CALL_CONTROL_CALLING_FEATURES;  \
    GET_L1OBJ_PARAM_UINT(i, n, v);                       \
}

/*****************************************************************
**  FUNCTION:       dalVoice_AddCallFeatureSet
**
**  PURPOSE:        creates a calling features set object in MDM
**
**  INPUT PARMS:    parms->op[0] - voice service instance
**
**  OUTPUT PARMS:   inst         - pointer to integer where instance ID
**                                 of the created object will be stored
**
**  RETURNS:        CMSRET_SUCCESS - object creation success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_AddCallFeatureSet( DAL_VOICE_PARMS *parms, int *inst )
{
    return addL2Object( MDMOID_CALLING_FEATURES_SET, parms->op[0], inst );
}

/*****************************************************************
**  FUNCTION:       dalVoice_DeleteCallFeatureSet
**
**  PURPOSE:        deletes a calling features set object in MDM
**
**  INPUT PARMS:    parms->op[0] - voice service instance
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS - object creation success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_DeleteCallFeatureSet( DAL_VOICE_PARMS *parms )
{
    return delL2Object( MDMOID_CALLING_FEATURES_SET, parms->op[0], parms->op[1] );
}

/*****************************************************************
**  FUNCTION:       dalVoice_mapCallFeatureSetNumToInst
**
**  PURPOSE:        maps a calling features set object number to instance ID
**
**  INPUT PARMS:    parms->op[0] - voice service instance
**                  parms->op[1] - object number to map
**
**  OUTPUT PARMS:   setInst - the object instance ID which the object number maps to
**
**  RETURNS:        CMSRET_SUCCESS - object creation success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_mapCallFeatureSetNumToInst( DAL_VOICE_PARMS *parms,  int *setInst )
{
    return mapL2ObjectNumToInst( MDMOID_CALLING_FEATURES_SET, parms->op[0], parms->op[1], setInst);
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumCallFeatureSet
**
**  PURPOSE:        obtains the number of calling feature set objects from MDM
**
**  INPUT PARMS:    parms->op[0] - voice service instance
**
**  OUTPUT PARMS:   numOfSet - number of calling feature set objects
**
**  RETURNS:        CMSRET_SUCCESS - object creation success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumCallFeatureSet( DAL_VOICE_PARMS *parms, int *numOfSet )
{
    CmsRet   ret = CMSRET_SUCCESS;

    *numOfSet = 0;
    GET_CALL_CONTROL_FEATURE_PARAM_UINT( parms->op[0], setNumberOfEntries, numOfSet );

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumCallFeatureSetStr
**
**  PURPOSE:        obtains the number of calling feature set objects from MDM, in string format
**
**  INPUT PARMS:    parms->op[0] - voice service instance
**                  length - max length of the output
**
**  OUTPUT PARMS:   value - number of calling feature set objects
**
**  RETURNS:        CMSRET_SUCCESS - object creation success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumCallFeatureSetStr( DAL_VOICE_PARMS *parms, char *value , unsigned int length )
{
    CmsRet   ret = CMSRET_SUCCESS;

    int numSets = 0;
    memset( value, 0, length );

    ret = dalVoice_GetNumCallFeatureSet( parms, &numSets );

    if (ret == CMSRET_SUCCESS)
    {
       snprintf( value, length, "%u", numSets);
    }

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFFeatureEnabled
**
**  PURPOSE:
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - call control line instance
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
    CmsRet ret = CMSRET_SUCCESS;

    switch(parms->op[2]){
        case DAL_VOICE_FEATURE_CODE_CALLWAIT:        /* Enable CallWaiting feature */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callWaitingEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CALLWAIT_ONCE:/* Enable CallWaiting feature for current call */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callWaitingEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_NOANS:       /* Forward calls on NoAnswer */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callForwardOnNoAnswerEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_BUSY:        /* Forward calls if Busy */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callForwardOnBusyEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_ALL:         /* Forward all calls */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callForwardUnconditionalEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CALLREDIAL:      /* Redial call */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], repeatDialEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_WARM_LINE:       /* Activate warm line */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_WarmLineEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_ANON_REJECT:     /* Activate anonymous call rejection */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], anonymousCallRejectionEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_ANON_CALL:       /* Activate permanent CID blocking */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], anonymousCallEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CALL_BARRING:    /* Outgoing call barring feature */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallBarringEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_DND:             /* Activate do-not-disturb */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], doNotDisturbEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_NET_PRIV:        /* Activate network privacy */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_NetworkPrivacyEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_TRANSFER:        /* Call transfer */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callTransferEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_MWI:        /* MWI */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], MWIEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_VMWI:        /* Visual MWI */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], VMWIEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CID:        /* Caller ID */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callerIDEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CID_NAME:        /* Caller ID + name */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callerIDNameEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CALLRETURN:        /* last call return */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallReturnEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CONFERENCING:
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallConferenceEnable, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_EUROFLASH:        /* Euro flash */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_EuroFlashEnable, getVal, length);
        }
        break;
        default:
        {
            GET_CALLING_FEATURES_PARAM_UNSUPPORTED(getVal, length);
        }
        break;
    }

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFFeatureAction
**
**  PURPOSE:
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
   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFFeatureStarted
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;

    switch(parms->op[2]){
        case DAL_VOICE_FEATURE_CODE_CALLWAIT:        /* Enable CallWaiting feature */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallWaitingStart, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_NOANS:       /* Forward calls on NoAnswer */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallForwardOnNoAnswerStart, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_BUSY:        /* Forward calls if Busy */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallForwardOnBusyStart, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_ALL:         /* Forward all calls */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallForwardUnconditionalStart, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_WARM_LINE:       /* Activate warm line */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_WarmLineStart, getVal, length);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_DND:             /* Activate do-not-disturb */
        {
            GET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_DoNotDisturbStart, getVal, length);
        }
        break;
        default:
            GET_CALLING_FEATURES_PARAM_UNSUPPORTED(getVal, length);
        break;
    }

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallFwdAll
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'unconditional call forwarding' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallFwdAll( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_FWD_ALL;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallFwdBusy
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'call forwarding on busy' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallFwdBusy( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_FWD_BUSY;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallFwdNoAns
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'call forwarding on no answer' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallFwdNoAns( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_FWD_NOANS;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallFwdNum
**
**  PURPOSE:
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - call control extension instance
**
**  OUTPUT PARMS:   cfNumber - Call Forward Number
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallFwdNum(DAL_VOICE_PARMS *parms, char *cfNumber, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;
    GET_CALLING_FEATURES_PARAM_STR(parms->op[0], parms->op[1], callForwardUnconditionalNumber, cfNumber, length);
    return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFWarmLineNum
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_CALLING_FEATURES_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_WarmLineNumber, warmLineNumber, length);
    return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallWaiting
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'call waiting' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallWaiting( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CALLWAIT;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallBarring
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'call barring' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallBarring( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CALL_BARRING;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFWarmLine
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'warm line' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFWarmLine( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_WARM_LINE;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallReturn
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'call return' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallReturn( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CALLRETURN;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallRedial
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'call redial' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallRedial( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CALLREDIAL;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallTransfer
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'call transfer' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallTransfer( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_TRANSFER;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallId
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'call ID' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallId( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CID;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallIdName
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'call ID name' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallIdName( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CID_NAME;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallBarringMode
**
**  PURPOSE:
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
    CmsRet ret;

    if( parms && mode && length > 0)
    {
        GET_CALLING_FEATURES_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_CallBarringMode, mode, length);
    }
    else
    {
        ret = CMSRET_INVALID_ARGUMENTS;
    }

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallBarringPin
**
**  PURPOSE:
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
    CmsRet ret;

    if( parms && pin && length > 0)
    {
        GET_CALLING_FEATURES_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_CallBarringUserPin, pin, length);
    }
    else
    {
        ret = CMSRET_INVALID_ARGUMENTS;
    }

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallBarringDigitMap
**
**  PURPOSE:
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
    CmsRet ret;

    if( parms && digitMap && length > 0)
    {
        GET_CALLING_FEATURES_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_CallBarringDigitMap, digitMap, length);
    }
    else
    {
        ret = CMSRET_INVALID_ARGUMENTS;
    }

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFVisualMWI
**
**  PURPOSE:
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
    if( parms && vmwi && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_VMWI;

        return(dalVoice_GetVlCFFeatureEnabled( parms, vmwi, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFAnonCallBlck
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'anonymous call rejection' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFAnonCallBlck( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_ANON_REJECT;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFAnonymousCalling
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'anonymous outgoing call' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFAnonymousCalling( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_ANON_CALL;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFDoNotDisturb
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   'anonymous outgoing call' enable flag (Yes/No)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFDoNotDisturb( DAL_VOICE_PARMS *parms, char *getVal, unsigned int length )
{
    if( parms && getVal && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_DND;

        return(dalVoice_GetVlCFFeatureEnabled( parms, getVal, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFMWIEnable
**
**  PURPOSE:
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
    if( parms && enable && length > 0)
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_MWI;

        return(dalVoice_GetVlCFFeatureEnabled( parms, enable, length ));
    }
    return CMSRET_INVALID_ARGUMENTS;
}

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
    return dalVoice_GetVoiceSvcCodecList(parms, codec, length );
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
    CmsRet ret = CMSRET_SUCCESS;

    cmsLog_debug("%s() command %u\n", __FUNCTION__, parms->op[2]);
    switch(parms->op[2]){
        case DAL_VOICE_FEATURE_CODE_CALLWAIT:        /* Enable CallWaiting feature */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallWaitingStart, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_NOANS:       /* Forward calls on NoAnswer */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallForwardOnNoAnswerStart, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_BUSY:        /* Forward calls if Busy */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallForwardOnBusyStart, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_ALL:         /* Forward all calls */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallForwardUnconditionalStart, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_WARM_LINE:       /* Activate warm line */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_WarmLineStart, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_DND:             /* Activate do-not-disturb */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_DoNotDisturbStart, value);
        }
        break;
        default:
            ret = CMSRET_INVALID_ARGUMENTS;
        break;
    }

    return ( ret );
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
    CmsRet ret = CMSRET_SUCCESS;

    switch(parms->op[2]){
        case DAL_VOICE_FEATURE_CODE_CALLWAIT:        /* Enable CallWaiting feature */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callWaitingEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CALLWAIT_ONCE:/* Enable CallWaiting feature for current call */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callWaitingEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_NOANS:       /* Forward calls on NoAnswer */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callForwardOnNoAnswerEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_BUSY:        /* Forward calls if Busy */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callForwardOnBusyEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_FWD_ALL:         /* Forward all calls */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callForwardUnconditionalEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CALLREDIAL:      /* Redial call */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], repeatDialEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_WARM_LINE:       /* Activate warm line */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_WarmLineEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_ANON_REJECT:     /* Activate anonymous call rejection */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], anonymousCallRejectionEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_ANON_CALL:       /* Activate permanent CID blocking */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], anonymousCallEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CALL_BARRING:    /* Outgoing call barring feature */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallBarringEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_DND:             /* Activate do-not-disturb */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], doNotDisturbEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_NET_PRIV:        /* Activate network privacy */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_NetworkPrivacyEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_TRANSFER:        /* Call transfer */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callTransferEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_MWI:        /* Call transfer */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], MWIEnable, value);
        }
        case DAL_VOICE_FEATURE_CODE_VMWI:        /* Call transfer */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], VMWIEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CID:        /* Call transfer */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callerIDEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CID_NAME:        /* Call transfer */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], callerIDNameEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CALLRETURN:        /* Call transfer */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallReturnEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_CONFERENCING:        /* Call conf */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_CallConferenceEnable, value);
        }
        break;
        case DAL_VOICE_FEATURE_CODE_EUROFLASH:        /* Euro flash */
        {
            SET_CALLING_FEATURES_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_EuroFlashEnable, value);
        }
        break;
        default:
            ret = CMSRET_INVALID_ARGUMENTS;
        break;
    }

    return ( ret );
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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_NET_PRIV;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CALL_BARRING;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
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
   CmsRet ret = CMSRET_SUCCESS;
   SET_CALLING_FEATURES_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_CallBarringMode, value, NULL);
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
   CmsRet ret = CMSRET_SUCCESS;
   SET_CALLING_FEATURES_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_CallBarringUserPin, value, NULL);
   return ret;
}


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
    CmsRet ret = CMSRET_SUCCESS;
    SET_CALLING_FEATURES_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_CallBarringDigitMap, value, NULL);
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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_MWI;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
}

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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_FWD_ALL;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallRedial
* Description  : Enable or disable callforward unconditional by the endpoint
*                VoiceProfile.{i}.Line{i}.CallingFeatures.CallForwardUnconditionalEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallRedial( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CALLREDIAL;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallReturn
* Description  : Enable or disable callforward unconditional by the endpoint
*                VoiceProfile.{i}.Line{i}.CallingFeatures.CallForwardUnconditionalEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallReturn( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CALLRETURN;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_FWD_NOANS;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_FWD_BUSY;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
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
   CmsRet ret = CMSRET_SUCCESS;
   cmsLog_debug("%s()\n", __FUNCTION__);
   SET_CALLING_FEATURES_PARAM_STR(parms->op[0], parms->op[1], callForwardUnconditionalNumber, value, NULL);
   return ret;
}


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
   CmsRet ret = CMSRET_SUCCESS;
   cmsLog_debug("%s()\n", __FUNCTION__);
   SET_CALLING_FEATURES_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_WarmLineNumber, value, NULL);
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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CALLWAIT;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallId
* Description  : Enable or disable call ID by the endpoint
*                VoiceProfile.{i}.Line{i}.CallingFeatures.CallWaitingEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallId( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CID;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallIdName
* Description  : Enable or disable call ID name by the endpoint
*                VoiceProfile.{i}.Line{i}.CallingFeatures.CallWaitingEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallIdName( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_CID_NAME;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetVlCFCallTransfer
* Description  : Enable or disable call transfer by the endpoint
*
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = feature set, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetVlCFCallTransfer( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_TRANSFER;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_ANON_REJECT;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_ANON_CALL;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_DND;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
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
   CmsRet ret = CMSRET_SUCCESS;

   return ( ret );
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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_WARM_LINE;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
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
    CmsRet  ret = CMSRET_SUCCESS;
    cmsLog_debug("%s()\n", __FUNCTION__);
    if( parms && value )
    {
        parms->op[2] = DAL_VOICE_FEATURE_CODE_VMWI;
        return ( dalVoice_SetVlCFFeatureEnabled(parms, value));
    }

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCLCodecList
**
**  PURPOSE:        Obtains the codec list for entire voice service
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
CmsRet dalVoice_GetVoiceSvcCodecList(DAL_VOICE_PARMS *parms, char *codec, unsigned int length )
{
    CmsRet ret =  CMSRET_SUCCESS;
    VoiceCapCodecsObject *obj = NULL;
    int    nameLen;
    int    numCodec = 0;
    int    svcIdx = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack searchIidStack = EMPTY_INSTANCE_ID_STACK;

    if( parms->op[0] <= 0 || codec == NULL || length <= 0 )
    {
        return CMSRET_INVALID_PARAM_VALUE;
    }

    svcIdx = parms->op[0];
    ret = dalVoice_GetNumOfCodecs(svcIdx, &numCodec);
    if( ret == CMSRET_SUCCESS && numCodec > 0 )
    {
        PUSH_INSTANCE_ID(&iidStack, svcIdx);
        while ( (ret = cmsObj_getNextInSubTreeFlags(MDMOID_VOICE_CAP_CODECS, &iidStack, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **) &obj)) == CMSRET_SUCCESS )
        {
            nameLen = strlen(obj->codec);
            if( length > nameLen+1 )
            {
                length = length - nameLen - 1;
                strncat( codec, obj->codec, nameLen);
                strncat( codec, ",", strlen(","));
            }
            cmsObj_free((void **)&obj);
        }
    }

    cmsLog_debug("%s final codeclist (%s)", __FUNCTION__, codec );

    return CMSRET_SUCCESS;
}

#ifdef BRCM_SIP_TLS_SUPPORT
/*****************************************************************
**  FUNCTION:       dalVoice_GetLocalSipCertPrivKey
**
**  PURPOSE:
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
   char sipCertName[TEMP_CHARBUF_SIZE] = "sipcert";
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
**  PURPOSE:
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
   char sipCertName[TEMP_CHARBUF_SIZE] = "sipcert";
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
**  PURPOSE:
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
   char sipCertName[TEMP_CHARBUF_SIZE] = "sipcert";
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

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCLPacketizationPeriod
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**                  codecInst  - parms->op[1]
**
**  OUTPUT PARMS:   ptime - Packetization period
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCLPacketizationPeriod(DAL_VOICE_PARMS *parms, char *ptime, unsigned int length )
{
   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCLEncoder
**
**  PURPOSE:
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
   return ( CMSRET_SUCCESS );
}



#ifdef DMP_X_BROADCOM_COM_PSTNENDPOINT_1

/*****************************************************************
**  FUNCTION:       dalVoice_GetVoiceFxoPhyInterfaceList
**
**  PURPOSE:
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
    CmsRet  ret =  CMSRET_SUCCESS;
    return  ret;
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
   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlFxoPhyReferenceList
**
**  PURPOSE:
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
   return ( CMSRET_SUCCESS );
}

#endif /* DMP_X_BROADCOM_COM_PSTNENDPOINT_1 */

/*****************************************************************
**  FUNCTION:       dalVoice_GetIpv6Enabled
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   enabled - Enabled flag.  '1' - enabled, '0' - disabled.
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


CmsRet dalVoice_GetNumFxoEndpt( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   return ( CMSRET_SUCCESS );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetMaxLinesPerVoiceProfile
**
**  PURPOSE:       Returns max no. of lines that can be configured
**                  configured in a string
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:   value - max no. of voice profiles that can be configured
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetMaxLinesPerVoiceProfile( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
    CmsRet   ret = CMSRET_SUCCESS;
#ifdef DMP_CALLCONTROL_1
    GET_VOICE_CAP_PARAM_INT_AS_STR(parms->op[0], parms->op[1], maxLineCount, value, length);
#endif
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetMaxExtensionPerVoiceProfile
**
**  PURPOSE:       Returns max no. of extensions that can be configured
**                  configured in a string
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:   value - max no. of voice profiles that can be configured
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetMaxExtensionPerVoiceProfile( DAL_VOICE_PARMS *parms, int* value )
{
    CmsRet   ret = CMSRET_SUCCESS;
#ifdef DMP_CALLCONTROL_1
    GET_VOICE_CAP_PARAM_UINT(parms->op[0], maxExtensionCount, value);
#endif
    return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetMaxCallsPerLine
*
* PURPOSE:     Get maximum number of calls per line
*
* PARAMETERS:  None
*
* RETURNS:     maximum number of calls per line
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetMaxCallsPerLine( DAL_VOICE_PARMS *parms, int* value )
{
    CmsRet   ret = CMSRET_SUCCESS;
#ifdef DMP_CALLCONTROL_1
    GET_VOICE_CAP_PARAM_UINT(parms->op[0], maxSessionsPerLine, value);
#endif
    return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetMaxCallsPerExtension
*
* PURPOSE:     Get maximum number of calls per extension
*
* PARAMETERS:  None
*
* RETURNS:     maximum number of calls per extension
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetMaxCallsPerExtension( DAL_VOICE_PARMS *parms, int* value )
{
    CmsRet   ret = CMSRET_SUCCESS;
#ifdef DMP_CALLCONTROL_1
    GET_VOICE_CAP_PARAM_UINT(parms->op[0], maxSessionsPerExtension, value);
#endif
    return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetMaxCallsPerExtension
*
* PURPOSE:     Get maximum number of calls per extension
*
* PARAMETERS:  None
*
* RETURNS:     maximum number of calls per extension
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetMaxCalls( DAL_VOICE_PARMS *parms, int* value )
{
    CmsRet   ret = CMSRET_SUCCESS;
#ifdef DMP_CALLCONTROL_1
    GET_VOICE_CAP_PARAM_UINT(parms->op[0], maxSessionCount, value);
#endif
    return ret;
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetTotalNumLines
**
**  PURPOSE:       Returns no. of lines In the System
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
   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumPhysEndpts
**
**  PURPOSE:       Returns no. of physical voice endpoints In the System
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
   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumLinesPerVoiceProfile
**
**  PURPOSE:       Returns no. of lines that are
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
   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumSipNetwork
**
**  PURPOSE:       Returns no. of network in this voice service
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - no. of lines that is configured
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumSipNetwork( DAL_VOICE_PARMS *parms, int *value )
{
    CmsRet ret;
    GET_SIP_OBJ_PARAM_UINT( parms->op[0], networkNumberOfEntries, value );
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetMaxVoiceProfiles
**
**  PURPOSE:       Returns maximum no. of service providers that can
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
   return ( CMSRET_SUCCESS );
}

/***************************************************************************
* Function Name: dalVoice_SetEuroFlashEnable
* Description  : Enable or disable Euro flash
*                MDMVS_YES for enable, MDMVS_NO for disable.  Also takes
*                other boolean strings used in convertStringToBool().
*                VoiceProfile.{i}.Line{0}.CallingFeatures.X_BROADCOM_COM_EuroFlashEnable
*
* Parameters   : parms->op[0] = vpInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
* NOTES        : Although the data model contains one enable flag per line,
*                we will only use the first line's flag.
****************************************************************************/
CmsRet dalVoice_SetEuroFlashEnable( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet  ret = CMSRET_SUCCESS;
   cmsLog_debug("%s()\n", __FUNCTION__);
   if( parms && value )
   {
      DAL_VOICE_PARMS parmsList = *parms;
      int lineInst;

      /* Force lineInst to first one */
      parmsList.op[1] = 0;
      if( dalVoice_mapCallFeatureSetNumToInst( &parmsList , &lineInst ) == CMSRET_SUCCESS)
      {
         parmsList.op[1] = lineInst;
         parmsList.op[2] = DAL_VOICE_FEATURE_CODE_EUROFLASH;

         return ( dalVoice_SetVlCFFeatureEnabled( &parmsList, value ) );
      }
   }

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetEuroFlashEnable
**
**  PUROPOSE:       Retrieve European flash enable
**
**  INPUT PARMS:    vpInst   - parms->op[0]
**                  length   - buffer length
**
**  OUTPUT PARMS:
**                  pEuroFlashEn - European flash enable
**                                 MDMVS_YES for enabled, MDMVS_NO for disabled
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
**  NOTES:          Although the data model contains one enable flag per line,
**                  we will only use the first line's flag.
*******************************************************************/
CmsRet dalVoice_GetEuroFlashEnable(DAL_VOICE_PARMS *parms, char *pEuroFlashEn, unsigned int length )
{
   if( parms && pEuroFlashEn && length > 0)
   {
      DAL_VOICE_PARMS parmsList = *parms;
      int lineInst;

      /* Force lineInst to first one */
      parmsList.op[1] = 0;
      if( dalVoice_mapCallFeatureSetNumToInst( &parmsList , &lineInst ) == CMSRET_SUCCESS)
      {
          parmsList.op[1] = lineInst;
          parmsList.op[2] = DAL_VOICE_FEATURE_CODE_EUROFLASH;

          return(dalVoice_GetVlCFFeatureEnabled( &parmsList, pEuroFlashEn, length ));
      }
   }
   return CMSRET_INVALID_ARGUMENTS;
}

/***************** Sip Client Interface ************************/

#define GET_SIP_CLIENT_PARAM_BOOL(i, p, n, v, l)    \
{                                                   \
    SipClientObject *obj=NULL;                      \
    MdmObjectId    __oid= MDMOID_SIP_CLIENT;        \
    GET_L2OBJ_PARAM_BOOL( i, p, n, v, l );          \
}

#define SET_SIP_CLIENT_PARAM_BOOL(i, p, n, v)           \
{                                                       \
    SipClientObject  *obj=NULL;                         \
    MdmObjectId     __oid = MDMOID_SIP_CLIENT;          \
    SET_L2OBJ_PARAM_BOOL(i, p, n, v)                    \
}

#define GET_SIP_CLIENT_PARAM_STR(i, p, n, v, l)     \
{                                                   \
    SipClientObject *obj=NULL;                      \
    MdmObjectId    __oid= MDMOID_SIP_CLIENT;        \
    GET_L2OBJ_PARAM_STR( i, p, n, v, l );           \
}

#define GET_SIP_CLIENT_PARAM_UINT(i, p, n, v)     \
{                                                 \
    SipClientObject *obj=NULL;                    \
    ret = getObject( MDMOID_SIP_CLIENT, i, p, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj ); \
    if( CMSRET_SUCCESS == ret ){                 \
        *v = obj->n;            \
        cmsObj_free((void **)&obj);               \
    } \
    else{ \
        cmsLog_error( "failed to retrieve voice service object\n");   \
    }\
}

#define SET_SIP_CLIENT_PARAM_STR(i, p, n, v, f)         \
{                                                       \
    SipClientObject  *obj=NULL;                         \
    MdmObjectId     __oid = MDMOID_SIP_CLIENT;          \
    SET_L2OBJ_PARAM_STR(i, p, n, v, f);                 \
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_CLIENT_PARAM_BOOL(parms->op[0], parms->op[1], T38Enable, value);
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetT38Enable
**
**  PURPOSE:
**
**  INPUT PARMS:    voice service Inst    - parms->op[0]
**                  SIP client Inst    - parms->op[1]
**
**  OUTPUT PARMS:   enabled - T38 Enabled
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetT38Enable(DAL_VOICE_PARMS *parms, char *enabled, unsigned int length )
{
    CmsRet   ret = CMSRET_SUCCESS;

    GET_SIP_CLIENT_PARAM_BOOL(parms->op[0], parms->op[1], T38Enable, enabled, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlEnable
**
**  PURPOSE:
**
**  INPUT PARMS:    voice service instance    - parms->op[0]
**                  client instance  - parms->op[1]
**
**  OUTPUT PARMS:   Line Enable status (1 if enabled)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlEnable(DAL_VOICE_PARMS *parms, char *lineEnabled, unsigned int length )
{
    CmsRet  ret = CMSRET_SUCCESS;

    GET_SIP_CLIENT_PARAM_BOOL(parms->op[0], parms->op[1], enable, lineEnabled, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlSipURI
**
**  PURPOSE:
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
    CmsRet  ret = CMSRET_SUCCESS;

    GET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], registerURI, userId, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlSipURIWrapper
**
**  PURPOSE:        Wrap dalVoice_GetVlSipURI() and only return
**                  user or extension part of URI.
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
CmsRet dalVoice_GetVlSipURIWrapper(DAL_VOICE_PARMS *parms, char *userId, unsigned int length )
{
   char *pStr;
   int i;
   CmsRet ret = dalVoice_GetVlSipURI(parms, userId, length);

   if (CMSRET_SUCCESS == ret)
   {
      /* Only use user portion of URI: sip:<user>@<domain> */

      /* strip domain */
      pStr = strchr(userId, '@');
      if(NULL != pStr)
      {
         *pStr = '\0'; /* end the string */
      }

      /* strip type - in-place copy */
      pStr = strchr(userId, ':');
      if (NULL != pStr)
      {
         pStr++;
         for (i = 0; pStr[i]; i++)
         {
            userId[i] = pStr[i];
         }
         userId[i] = '\0';
      }
   }

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlSipAuthUserName
**
**  PURPOSE:
**
**  INPUT PARMS:    voice service instance  - parms->op[0]
**                  client instance - parms->op[1]
**
**  OUTPUT PARMS:   authName - SIP Auth username
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetVlSipAuthUserName(DAL_VOICE_PARMS *parms, char *authName, unsigned int length )
{
    CmsRet  ret = CMSRET_SUCCESS;

    GET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], authUserName, authName, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlSipAuthPassword
**
**  PURPOSE:
**
**  INPUT PARMS:    voice service instance  - parms->op[0]
**                  client instance - parms->op[1]
**
**  OUTPUT PARMS:   passwd - Password
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlSipAuthPassword(DAL_VOICE_PARMS *parms, char *passwd, unsigned int length )
{
    CmsRet  ret = CMSRET_SUCCESS;

    GET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], authPassword, passwd, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetVlCFCallerIDName
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**                  lineInst  - parms->op[1]
**
**  OUTPUT PARMS:   userName - DisplayName
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetVlCFCallerIDName(DAL_VOICE_PARMS *parms, char *userName, unsigned int length )
{
    CmsRet ret;

    GET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_DisplayName, userName, length);

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipClientEnable
**
**  PURPOSE:        Obtains SIP client enable flag
**
**  INPUT PARMS:    voice service instance    - parms->op[0]
**                  client instance  - parms->op[1]
**
**  OUTPUT PARMS:   Line Enable status ("true" if enabled)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipClientEnable(DAL_VOICE_PARMS *parms, char *lineEnabled, unsigned int length )
{
    CmsRet  ret = CMSRET_SUCCESS;

    GET_SIP_CLIENT_PARAM_BOOL(parms->op[0], parms->op[1], enable, lineEnabled, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipClientStatus
**
**  PURPOSE:        Obtains SIP client status
**
**  INPUT PARMS:    voice service instance    - parms->op[0]
**                  client instance  - parms->op[1]
**
**  OUTPUT PARMS:   Line status ("true" if enabled)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipClientStatus(DAL_VOICE_PARMS *parms, char *lineStatus, unsigned int length )
{
    CmsRet  ret = CMSRET_SUCCESS;

    GET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], status, lineStatus, length);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetSipClientStatus
**
**  PURPOSE:        Obtains SIP client status
**
**  INPUT PARMS:    voice service instance    - parms->op[0]
**                  client instance  - parms->op[1]
**
**  OUTPUT PARMS:   Line status ("true" if enabled)
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetSipClientStatus(DAL_VOICE_PARMS *parms, char *status )
{
    CmsRet  ret = CMSRET_SUCCESS;

    SET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], status, status, NULL);

    return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetSipClientAttachedNetworkInst
**
**  PURPOSE:        Obtains the network instance attached to a particular SIP client
**
**  INPUT PARMS:    voice service instance    - parms->op[0]
**                  client instance  - parms->op[1]
**
**  OUTPUT PARMS:   Sip Network Instance number which this client attached to.
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipClientAttachedNetworkInst(DAL_VOICE_PARMS *parms, char *network, unsigned int length )
{
    CmsRet  ret = CMSRET_SUCCESS;
    char    networkFullPath[MAX_TR104_OBJ_SIZE];
    MdmPathDescriptor pathDesc;

    memset( networkFullPath, 0, sizeof(networkFullPath));
    GET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], network, networkFullPath, length);
    if( ret == CMSRET_SUCCESS && strlen( networkFullPath ) > 0 )
    {
        ret = cmsMdm_fullPathToPathDescriptor(networkFullPath, &pathDesc);
        if(ret == CMSRET_SUCCESS && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
        {
            snprintf( network, length, "%u", POP_INSTANCE_ID(&pathDesc.iidStack));
        }
    }

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipClientAttachedNetworkIdx
**
**  PURPOSE:        Obtains the network ID attached to a particular SIP client
**
**  INPUT PARMS:    voice service instance    - parms->op[0]
**                  client instance  - parms->op[1]
**
**  OUTPUT PARMS:   Sip Network index which this client attached to.
**                  the index is from 0 to n
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipClientAttachedNetworkIdx(DAL_VOICE_PARMS *parms, char *network, unsigned int length )
{
    int     totalNumOfNetwork = 0, networkInst, i;
    CmsRet  ret = CMSRET_SUCCESS;
    char    networkFullPath[MAX_TR104_OBJ_SIZE];
    MdmPathDescriptor pathDesc;
    DAL_VOICE_PARMS  localParms;

    memset( networkFullPath, 0, sizeof(networkFullPath));
    GET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], network, networkFullPath, length);
    if( ret == CMSRET_SUCCESS && strlen( networkFullPath ) > 0 )
    {
        /* convert full path to network instance stack */
        ret = cmsMdm_fullPathToPathDescriptor(networkFullPath, &pathDesc);
        if(ret == CMSRET_SUCCESS && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
        {
            localParms.op[0] = parms->op[0];   /* voice service instance */

            dalVoice_GetNumSipNetwork( &localParms, &totalNumOfNetwork );
            if( totalNumOfNetwork <= 0 )
            {
                cmsLog_error("%s No valid Sip Network Instance\n", __FUNCTION__);
                return  CMSRET_INVALID_PARAM_VALUE;
            }
            else
            {

                /* iterate index for corrolated instance */
                for( i = 0; i < totalNumOfNetwork; i++ )
                {
                    localParms.op[1] = i;   /* network index */
                    if( dalVoice_mapNetworkNumToInst ( &localParms, &networkInst ) == CMSRET_SUCCESS &&
                        networkInst == (PEEK_INSTANCE_ID( &pathDesc.iidStack )))
                    {
                        /* found */
                        snprintf( network, length, "%d", i );
                        return CMSRET_SUCCESS;
                    }
                }

                return CMSRET_INVALID_PARAM_VALUE;
            }
        }
        else
        {
           return CMSRET_INVALID_PARAM_VALUE;
        }
    }

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPktcIMPUIdType
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  netInst - parms->op[1]
**
**  OUTPUT PARMS:   getVal - IMPU ID type
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPktcIMPUIdType(DAL_VOICE_PARMS *parms, unsigned int *getVal)
{
   CmsRet  ret = CMSRET_SUCCESS;
   SipClientObject  *obj=NULL;
   MdmObjectId     __oid = MDMOID_SIP_CLIENT;

   GET_L4OBJ_PARAM_UINT( parms->op[0], parms->op[1], 0, 0, X_BROADCOM_COM_PKTC_IMPUIdType, getVal);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPktcIMPUIMPIIndexRef
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  netInst - parms->op[1]
**
**  OUTPUT PARMS:   getVal - IMPU-IMPI index reference
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPktcIMPUIMPIIndexRef(DAL_VOICE_PARMS *parms, unsigned int *getVal)
{
   CmsRet  ret = CMSRET_SUCCESS;
   SipClientObject  *obj=NULL;
   MdmObjectId     __oid = MDMOID_SIP_CLIENT;

   GET_L4OBJ_PARAM_UINT( parms->op[0], parms->op[1], 0, 0, X_BROADCOM_COM_PKTC_IMPUIMPIIndexRef, getVal);

   return ret;
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetPktcIMPUSigSecurity
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  netInst - parms->op[1]
**
**  OUTPUT PARMS:   getVal - IMPU signal security
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPktcIMPUSigSecurity(DAL_VOICE_PARMS *parms, UBOOL8 *getVal)
{
   CmsRet  ret = CMSRET_SUCCESS;
   SipClientObject  *obj=NULL;
   MdmObjectId     __oid = MDMOID_SIP_CLIENT;

   GET_L4OBJ_PARAM_BOOL( parms->op[0], parms->op[1], 0, 0, X_BROADCOM_COM_PKTC_IMPUSigSecurity, getVal);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPktcIMPIIdType
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  netInst - parms->op[1]
**
**  OUTPUT PARMS:   getVal - IMPI ID type
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPktcIMPIIdType(DAL_VOICE_PARMS *parms, unsigned int *getVal)
{
   CmsRet  ret = CMSRET_SUCCESS;
   SipClientObject  *obj=NULL;
   MdmObjectId     __oid = MDMOID_SIP_CLIENT;

   GET_L4OBJ_PARAM_UINT( parms->op[0], parms->op[1], 0, 0, X_BROADCOM_COM_PKTC_IMPIIdType, getVal);

   return ret;
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
    CmsRet   ret = CMSRET_SUCCESS;
    SET_SIP_CLIENT_PARAM_BOOL(parms->op[0], parms->op[1], enable, value);
    return ( ret );
}

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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], authUserName, value, NULL);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], authPassword, value, NULL);
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
    CmsRet  ret = CMSRET_SUCCESS;
    SET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], registerURI, value, NULL);
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
    CmsRet  ret = CMSRET_SUCCESS;
    if( parms && value )
    {
        SET_SIP_CLIENT_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_DisplayName, value, NULL);
    }

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetPktcIMPUIdType
**
**  PURPOSE:        Stores the IMPU ID type in MDM.
**
**  INPUT PARMS:    IMPU ID type for voice
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetPktcIMPUIdType( DAL_VOICE_PARMS *parms, unsigned int setVal )
{
   CmsRet  ret = CMSRET_SUCCESS;
   SipClientObject  *obj=NULL;
   MdmObjectId     __oid = MDMOID_SIP_CLIENT;

   SET_L4OBJ_PARAM_UINT( parms->op[0], parms->op[1], 0, 0, X_BROADCOM_COM_PKTC_IMPUIdType, setVal, 0);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetPktcIMPUIMPIIndexRef
**
**  PURPOSE:        Stores the IMPU-IMPI index reference in MDM.
**
**  INPUT PARMS:    IMPU-IMPI index reference for voice
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetPktcIMPUIMPIIndexRef( DAL_VOICE_PARMS *parms, unsigned int setVal )
{
   CmsRet  ret = CMSRET_SUCCESS;
   SipClientObject  *obj=NULL;
   MdmObjectId     __oid = MDMOID_SIP_CLIENT;

   SET_L4OBJ_PARAM_UINT( parms->op[0], parms->op[1], 0, 0, X_BROADCOM_COM_PKTC_IMPUIMPIIndexRef, setVal, 0);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetPktcIMPUSigSecurity
**
**  PURPOSE:        Stores the IMPU signal security in MDM.
**
**  INPUT PARMS:    IMPU signal security for voice
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetPktcIMPUSigSecurity( DAL_VOICE_PARMS *parms, UBOOL8 setVal )
{
   CmsRet  ret = CMSRET_SUCCESS;
   SipClientObject  *obj=NULL;
   MdmObjectId     __oid = MDMOID_SIP_CLIENT;

   SET_L4OBJ_PARAM_BOOL( parms->op[0], parms->op[1], 0, 0, X_BROADCOM_COM_PKTC_IMPUSigSecurity, setVal);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetPktcIMPIIdType
**
**  PURPOSE:        Stores the IMPI ID type in MDM.
**
**  INPUT PARMS:    IMPI ID type for voice
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetPktcIMPIIdType( DAL_VOICE_PARMS *parms, unsigned int setVal )
{
   CmsRet  ret = CMSRET_SUCCESS;
   SipClientObject  *obj=NULL;
   MdmObjectId     __oid = MDMOID_SIP_CLIENT;

   SET_L4OBJ_PARAM_UINT( parms->op[0], parms->op[1], 0, 0, X_BROADCOM_COM_PKTC_IMPIIdType, setVal, 0);

   return ret;
}


/****************************************************************
**             Sip Client Contact Interface                   **
****************************************************************/
CmsRet dalVoice_GetNumOfContact( DAL_VOICE_PARMS *parms, int  *contact )
{
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_CLIENT_PARAM_UINT( parms->op[0], parms->op[1], contactNumberOfEntries, (UINT32 *)contact );
    return ret;
}

CmsRet dalVoice_AddSipContactUri( DAL_VOICE_PARMS *parms, int  *inst )
{
    return addL3Object( MDMOID_SIP_CLIENT_CONTACT, parms->op[0], parms->op[1], inst );
}

CmsRet dalVoice_DeleteSipContactUri( DAL_VOICE_PARMS *parms )
{
    return delL3Object( MDMOID_SIP_CLIENT_CONTACT, parms->op[0], parms->op[1], parms->op[2] );
}

/* only support 1 sip contact header override */
CmsRet dalVoice_SetSipContactUri( DAL_VOICE_PARMS *parms , char *value)
{
    CmsRet  ret = CMSRET_SUCCESS;
    SIPClientContactObject *obj = NULL;
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;
    int     contactInst;

    /* hard code first contact uri */
    parms->op[2] = 0;
    ret = dalVoice_mapSipContactNumToInst ( parms, &contactInst );
    if( ret != CMSRET_SUCCESS )
    {
        cmsLog_error("%d() can't find contact instance \n", __FUNCTION__);
        return CMSRET_INTERNAL_ERROR;
    }

    if( value != NULL )
    {
        ret = getObject( MDMOID_SIP_CLIENT_CONTACT, parms->op[0], 
                         parms->op[1], contactInst, 0, OGF_NO_VALUE_UPDATE,
                         &iidStack, (void **)&obj );
        if( CMSRET_SUCCESS == ret ){
            REPLACE_STRING_IF_NOT_EQUAL(obj->contactURI, value);
            ret = cmsObj_set((const void *)obj, &iidStack);
            cmsObj_free((void **)&obj);
            if( CMSRET_SUCCESS != ret )
                cmsLog_error( "failed to set L3 object (%d)\n", MDMOID_SIP_CLIENT_CONTACT);
        }
        else
        {
            cmsLog_error( "failed to get required L3 object (%d)\n", MDMOID_SIP_CLIENT_CONTACT);
            ret = CMSRET_INTERNAL_ERROR;
        }
    }
    else
    {
        cmsLog_error("%s() invalid argument value \n", __FUNCTION__);
        ret = CMSRET_INVALID_PARAM_VALUE;
    }

    return ret;
}

/* only support 1 override contact header */
CmsRet dalVoice_GetSipContactUri( DAL_VOICE_PARMS *parms , char *value, unsigned int length)
{
    CmsRet  ret = CMSRET_SUCCESS;
    SIPClientContactObject *obj = NULL;
    int    contactInst;

    /* hard code first contact uri */
    parms->op[2] = 0;
    ret = dalVoice_mapSipContactNumToInst ( parms, &contactInst );
    if( ret != CMSRET_SUCCESS )
    {
        cmsLog_error("%d() can't find contact instance \n", __FUNCTION__);
        return CMSRET_INTERNAL_ERROR;
    }

    memset((void *)value, 0, length );
    ret = getObject( MDMOID_SIP_CLIENT_CONTACT, parms->op[0], parms->op[1], 
                     contactInst, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj );
    if( CMSRET_SUCCESS == ret )
    {
        if( obj->enable && obj->contactURI && strlen(obj->contactURI)>0 )
        {
            snprintf(value, length, "%s", obj->contactURI);
        }
        cmsObj_free((void **)&obj);
    }
    else
    {
        cmsLog_error( "%s() failed to retrieve object (%d)\n", __FUNCTION__, MDMOID_SIP_CLIENT_CONTACT);
    }

    return ret;
}

#if 0
/***************************************************************************
* Function Name: dalVoice_mapSipContactInstToNum
* Description  : This returns the sip client contact number corresponding
*                to a sip client contact instance number
*
* Parameters   : vpInst (IN)    - voice profile instance
*                clientInst (IN)  - sip client instance
*                contactInst (IN)  - sip contact instance
*                number (OUT)   - number pointer
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_mapSipContactInstToNum ( int vpInst, int clientInst, int contactInst, int *num )
{
   return mapL3ObjectInstToNum( MDMOID_SIP_CLIENT_CONTACT, vpInst, clientInst, contactInst, num );
}
#endif

/***************************************************************************
* Function Name: dalVoice_mapNetworkNumToInst
* Description  : This returns the Line instance number corresponding
*                to a Voice Profile number and account index.
*
* Parameters   : vpInst (IN)    - VoiceProfile instance number
*                acntNum (IN)   - Account index
*                lineInst (OUT) - pointer to line instance number
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
CmsRet dalVoice_mapSipContactNumToInst ( DAL_VOICE_PARMS *parms, int *Inst )
{
    return mapL3ObjectNumToInst( MDMOID_SIP_CLIENT_CONTACT, parms->op[0], parms->op[1], parms->op[2], Inst);
}

/****************************************************************
**                 Sip Network Interface                       **
****************************************************************/
#define GET_SIP_NETWORK_PARAM_BOOL(i, p, n, v, l)   \
{                                                   \
    SIPNetworkObject *obj=NULL;                     \
    MdmObjectId __oid = MDMOID_SIP_NETWORK;         \
    GET_L2OBJ_PARAM_BOOL(i, p, n, v, l)             \
}

#define SET_SIP_NETWORK_PARAM_BOOL(i, p, n, v)          \
{                                                       \
    SIPNetworkObject *obj=NULL;                         \
    MdmObjectId     __oid = MDMOID_SIP_NETWORK;         \
    SET_L2OBJ_PARAM_BOOL(i, p, n, v)                    \
}

#define GET_SIP_NETWORK_PARAM_STR(i, p, n, v, l)    \
{                                                   \
    SIPNetworkObject *obj=NULL;                     \
    MdmObjectId __oid = MDMOID_SIP_NETWORK;         \
    GET_L2OBJ_PARAM_STR(i, p, n, v, l)              \
}

#define SET_SIP_NETWORK_PARAM_STR(i, p, n, v, f)        \
{                                                       \
    SIPNetworkObject *obj=NULL;                         \
    MdmObjectId     __oid = MDMOID_SIP_NETWORK;         \
    SET_L2OBJ_PARAM_STR(i, p, n, v, f);                 \
}

/* Returns value as a STRING */
#define GET_SIP_NETWORK_PARAM_UINT(i, p, n, v, l)       \
{                                                       \
    SIPNetworkObject *obj=NULL;                         \
    MdmObjectId     __oid = MDMOID_SIP_NETWORK;         \
    GET_L2OBJ_PARAM_UINT(i, p, n, v, l);                \
}

/* Takes value as a STRING */
#define SET_SIP_NETWORK_PARAM_UINT(i, p, n, v, f)   \
{                                                   \
    SIPNetworkObject *obj=NULL;                     \
    MdmObjectId      __oid = MDMOID_SIP_NETWORK;    \
    SINT32           __value = -1;                  \
    char             *__value_str = v;              \
    if(__value_str!=NULL&&strlen(__value_str)>0){   \
        __value = atoi(__value_str);                \
    }                                               \
    if(__value>=0){                                 \
        SET_L2OBJ_PARAM_UINT(i, p, n, __value, f);  \
    }                                               \
}

/*****************************************************************
**  FUNCTION:       dalVoice_AddSipNetwork
**
**  PURPOSE:        Adds a SIP network object
**
**  INPUT PARMS:    op[0] - voice service instance
**
**  OUTPUT PARMS:   inst - instance of added SIP network object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_AddSipNetwork( DAL_VOICE_PARMS *parms, int *inst )
{
    return addL2Object( MDMOID_SIP_NETWORK, parms->op[0], inst );
}

/*****************************************************************
**  FUNCTION:       dalVoice_DeleteSipNetwork
**
**  PURPOSE:        Deletes a SIP network object
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - instance of the SIP network object to delete
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_DeleteSipNetwork( DAL_VOICE_PARMS *parms )
{
    return delL2Object( MDMOID_SIP_NETWORK, parms->op[0], parms->op[1] );
}

/***************************************************************************
* Function Name: dalVoice_SetSipNetworkEnabled
* Description  : set enable/disable flag for this network object
*
* Parameters   : parms->op[0] = voice service idx, parms->op[1] = network idx, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipNetworkEnabled( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_BOOL( parms->op[0], parms->op[1], enable, value);
    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipNetworkStatus
* Description  : set network status to indicates its status
*                "up, dislabed, resolving, error_dns, error_other"
*
* Parameters   : parms->op[0] = voice service idx, parms->op[1] = netwk idx, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipNetworkStatus( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], status, value, NULL);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], registrarServer, value, NULL);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], registrarServerPort, value, 65535);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], proxyServer, value, NULL);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], proxyServerPort, value, 65535);
    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipUserAgentDomain
* Description  : CPE domain string
*                VoiceProfile.{i}.Sip.UserAgentDomain = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = network inst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipUserAgentDomain( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], userAgentDomain, value, NULL);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], userAgentPort, value, 65535);
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
   CmsRet ret = CMSRET_SUCCESS;
   SET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], outboundProxy, value, NULL);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], outboundProxyPort, value, 65535);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], timerB, value, 0);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], timerF, value, 0);
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
   CmsRet ret = CMSRET_SUCCESS;
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], registerRetryInterval, value, 0 );
    return ret;
}


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
   CmsRet ret = CMSRET_SUCCESS;
   SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], registerExpires, value, 0 );
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], DSCPMark, value, 63);
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
    CmsRet   ret;
    SET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], proxyServerTransport, value, sip_transport_valid_string );
    SET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], registrarServerTransport, value, sip_transport_valid_string );
    SET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], userAgentTransport, value, sip_transport_valid_string );
    return ( ret );
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
   CmsRet ret = CMSRET_SUCCESS;
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], conferenceCallDomainURI, value, NULL );
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_ConferencingOption, value, sip_conf_option_valid_string );
    return ret;
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
   CmsRet ret = CMSRET_SUCCESS;
   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipFailoverEnable
* Description  : Enables SIP failover feature
*                VoiceProfile.{i}.X_BROADCOM_COM_SipFailoverEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipFailoverEnable( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet ret = CMSRET_SUCCESS;
   SET_SIP_NETWORK_PARAM_BOOL( parms->op[0], parms->op[1], X_BROADCOM_COM_SipFailoverEnable, value);
   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipOptionsEnable
* Description  : Enables SIP OPTIONS ping feature
*                VoiceProfile.{i}.X_BROADCOM_COM_SipOptionsEnable = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipOptionsEnable( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet ret = CMSRET_SUCCESS;
   SET_SIP_NETWORK_PARAM_BOOL( parms->op[0], parms->op[1], X_BROADCOM_COM_SipOptionsEnable, value);
   return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipSecDomainName
* Description  : set value of the secondary domain name
*                VoiceProfile.{i}.X_BROADCOM_COM_SecondaryDomainName = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipSecDomainName( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet ret = CMSRET_SUCCESS;
   SET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryDomainName, value, NULL);
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
   CmsRet ret = CMSRET_SUCCESS;
   SET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryProxyAddress, value, NULL);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryProxyPort, value, 65535);
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
   CmsRet ret = CMSRET_SUCCESS;
   SET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryOutboundProxyAddress, value, NULL);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryOutboundProxyPort, value, 65535);
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
   CmsRet ret = CMSRET_SUCCESS;
   SET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryRegistrarAddress, value, NULL);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryRegistrarPort, value, 65535);
    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSipBackToPrimOption
* Description  : set back-to-primary option for SIP failover
*                VoiceProfile.{i}.X_BROADCOM_COM_BackToPrimMode = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSipBackToPrimOption( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet ret = CMSRET_SUCCESS;
   SET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], X_BROADCOM_COM_BackToPrimMode, value, NULL);
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
    CmsRet ret = CMSRET_SUCCESS;
    SET_SIP_NETWORK_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_ToTagMatching, value  );
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetSipNetworkVoipProfileAssoc
**
**  PURPOSE:        set network voip profile point to the voip instance
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - network instance
**                  op[2] - voip instance
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetSipNetworkVoipProfileAssoc( DAL_VOICE_PARMS *parms )
{
    return setL2ToL2ObjAssoc( MDMOID_SIP_NETWORK, MDMOID_IP_PROFILE, parms->op[0], parms->op[1], parms->op[2]);
}

CmsRet dalVoice_GetSipNetworkVoipProfileAssoc( DAL_VOICE_PARMS *parms )
{
    return getL2ToL2ObjAssoc( MDMOID_SIP_NETWORK, MDMOID_IP_PROFILE, parms->op[0], parms->op[1], &parms->op[2]);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetSipNetworkCodecList
**
**  PURPOSE:        Set codec list for a given network
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - network instance
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetSipNetworkCodecList( DAL_VOICE_PARMS *parms, char *codecList )
{
    CmsRet ret = CMSRET_SUCCESS;
    char *tokResult, *saveptr;
    char delim[2] = ",";
    char *codecNameList[TEMP_CHARBUF_SIZE];
    UINT32  codecProfileInst;
    char   *codecProfileFullPath;
    char    codecListFullPath[MAX_TR104_OBJ_SIZE];
    int  i, totalCodec = 0;
    MdmPathDescriptor  pathDesc;

    cmsLog_debug("%s()\n", __FUNCTION__);
    memset(codecListFullPath, 0, sizeof(codecListFullPath));

    if( parms == NULL || codecList == NULL)
        return CMSRET_INVALID_PARAM_VALUE;

    /* parses first token */
    tokResult = strtok_r( codecList, delim, &saveptr);
    while ( tokResult != NULL)
    {
        codecNameList[totalCodec] = tokResult;
        cmsLog_debug("%s() codec token (%d) = (%s)\n", __FUNCTION__, totalCodec, codecNameList[totalCodec]);
        totalCodec++;
        /* Get next token */
        tokResult = strtok_r( NULL, delim, &saveptr );
    }

    cmsLog_debug("%s() total codec (%d)\n", __FUNCTION__, totalCodec);
    if( totalCodec > 0 )
    {
        for( i = 0; i < totalCodec; i++ )
        {
            ret = dalVoice_MapCodecNameToCodecProfileInst( parms, codecNameList[i], (int *)&codecProfileInst );
            if( ret != CMSRET_SUCCESS || codecProfileInst <= 0 )
            {
                ret = dalVoice_AddCodecProfileByName( parms, codecNameList[i], (int *)&codecProfileInst );
                cmsLog_debug("%s() added new codec profile (%s) instance (%d) \n", __FUNCTION__, codecNameList[i], codecProfileInst);
            }

            if( ret == CMSRET_SUCCESS && codecProfileInst > 0 )
            {
                cmsLog_debug("%s() map codec (%s) to codec profile (%d) \n", __FUNCTION__, codecNameList[i], codecProfileInst);
                /* initialize pathDesc for codec */
                INIT_PATH_DESCRIPTOR(&pathDesc);

                pathDesc.oid = MDMOID_CODEC_PROFILE;
                PUSH_INSTANCE_ID(&pathDesc.iidStack, parms->op[0]);
                PUSH_INSTANCE_ID(&pathDesc.iidStack, codecProfileInst);

                ret = cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &codecProfileFullPath);
                if(ret == CMSRET_SUCCESS)
                {
                    cmsLog_debug("%s() codec profile full path (%s)\n", __FUNCTION__, codecProfileFullPath);
                    strncat(codecListFullPath, codecProfileFullPath, strlen(codecProfileFullPath));
                    if( i < totalCodec -1 ){
                        strncat(codecListFullPath, ",", strlen(","));
                    }
                    cmsMem_free((void *)codecProfileFullPath);
                }
            }
        }

        cmsLog_debug("%s() codec list full path (%s)\n", __FUNCTION__, codecListFullPath);
        SET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], codecList, codecListFullPath, NULL );
    }
    else {
        cmsLog_debug("%s() codec list is empty\n", __FUNCTION__);
        return CMSRET_INVALID_PARAM_VALUE;
    }

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetPktcOutboundProxyType
* Description  : Outbound proxy type
* Parameters   : parms->op[0] = vpInst, parms->op[1] = netInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetPktcOutboundProxyType( DAL_VOICE_PARMS *parms, unsigned int value )
{
   CmsRet ret = CMSRET_SUCCESS;
   SIPNetworkObject *obj=NULL;
   MdmObjectId __oid = MDMOID_SIP_NETWORK;

   SET_L4OBJ_PARAM_UINT( parms->op[0], parms->op[1], 0, 0, X_BROADCOM_COM_PKTC_OutboundProxyType, value, 0);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipNetworkEnabled
**
**  PURPOSE:       Returns network enabled flag
**
**  INPUT PARMS:    op[0] - voice service index
**                  op[1] - sip network index
**
**  OUTPUT PARMS:   value - true or false
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipNetworkEnabled( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_BOOL(parms->op[0], parms->op[1], enable,  value, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipNetworkStatus
**
**  PURPOSE:       Returns network enabled flag
**
**  INPUT PARMS:    op[0] - voice service index
**                  op[1] - sip network index
**
**  OUTPUT PARMS:   value - status of network
**                          "up, disable, resolving, errro_dns, error_other"
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipNetworkStatus( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], status,  value, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipProxyServer
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], proxyServer, proxyAddr, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipProxyServerPort
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], proxyServerPort, proxyPort, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipRegistrarServer
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], registrarServer, regSvrAddr, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipRegistrarServerPort
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], registrarServerPort, port, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipOutboundProxy
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], outboundProxy, outgoingProxy, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipOutboundProxyPort
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], outboundProxyPort, port, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipRegisterExpires
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], registerExpires, regExpire, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipTimerB
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], timerB, tmrB, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipTimerF
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], timerF, tmrF, length);
    return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetSipRegisterRetryInterval
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], registerRetryInterval, regRetry, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipUserAgentDomain
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], userAgentDomain, fqdn, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipUserAgentPort
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], userAgentPort, port, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipDSCPMark
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], DSCPMark, dscpMark, length);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipConferencingURI
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], conferenceCallDomainURI, conferencingURI, length );
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipConferencingOption
**
**  PURPOSE:
**
**  INPUT PARMS:    svc instance - parms->op[0]
**                  network instance - parms->op[1]
**
**  OUTPUT PARMS:   conferencingOption - Conferencing option
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipConferencingOption(DAL_VOICE_PARMS *parms, char *conferencingOption, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], X_BROADCOM_COM_ConferencingOption, conferencingOption, length );
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipFailoverEnable
**
**  PURPOSE:        obtains the "Enable" value of the SIP failover feature
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   enable - true or false
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipFailoverEnable(DAL_VOICE_PARMS *parms, char *enable, unsigned int length )
{
   CmsRet ret = CMSRET_SUCCESS;
   GET_SIP_NETWORK_PARAM_BOOL( parms->op[0], parms->op[1], X_BROADCOM_COM_SipFailoverEnable, enable, length);
   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipOptionsEnable
**
**  PURPOSE:        obtains the "Enable" value of the SIP OPTIONS ping feature
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   enable - true or false
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipOptionsEnable(DAL_VOICE_PARMS *parms, char *enable, unsigned int length )
{
   CmsRet ret = CMSRET_SUCCESS;
   GET_SIP_NETWORK_PARAM_BOOL( parms->op[0], parms->op[1], X_BROADCOM_COM_SipOptionsEnable, enable, length);
   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipBackToPrimOption
**
**  PURPOSE:        obtains the back-to-primary value of the SIP failover feature
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   option - from the list of available back-to-primary options
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipBackToPrimOption(DAL_VOICE_PARMS *parms, char *option, unsigned int length )
{
   CmsRet ret = CMSRET_SUCCESS;
   GET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], X_BROADCOM_COM_BackToPrimMode, option, length);
   return ret;
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
   CmsRet ret = CMSRET_SUCCESS;
   GET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryDomainName, secDomainName, length);
   return ret;
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
   CmsRet ret = CMSRET_SUCCESS;
   GET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryProxyAddress, secProxyAddr, length);
   return ret;
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryProxyPort, port, length);
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipSecOutboundProxyAddr
**
**  PURPOSE:
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
   CmsRet ret = CMSRET_SUCCESS;
   GET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryOutboundProxyAddress, secObProxyAddr, length);
   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipSecOutboundProxyPort
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryOutboundProxyPort, port, length);
    return ret;
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
   CmsRet ret = CMSRET_SUCCESS;
   GET_SIP_NETWORK_PARAM_STR( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryRegistrarAddress, secRegistrarAddr, length);
   return ret;
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_UINT( parms->op[0], parms->op[1], X_BROADCOM_COM_SecondaryRegistrarPort, port, length);
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipToTagMatching
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    GET_SIP_NETWORK_PARAM_BOOL(parms->op[0], parms->op[1], X_BROADCOM_COM_ToTagMatching, tagMatching, length );
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipTransport
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0]
**
**  OUTPUT PARMS:   transport - enum DAL_VOICE_SIP_TRANSPORTS in string format
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
**  NOTE:           This function should be obsoleted.  Use dalVoice_GetSipTransportString() instead.
**
*******************************************************************/
CmsRet  dalVoice_GetSipTransport(DAL_VOICE_PARMS *parms, char *transport, unsigned int length )
{
    char  tmp[32];
    CmsRet ret = CMSRET_SUCCESS;

    memset( tmp, 0, sizeof(tmp));
    GET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], registrarServerTransport, tmp, sizeof(tmp) );

    if( ret != CMSRET_SUCCESS )
        return ret;

    if( !cmsUtl_strcasecmp( tmp, MDMVS_UDP ))
    {
        snprintf( transport, length, "%d", DAL_VOICE_SIP_TRANSPORT_UDP);
    }
    else if( !cmsUtl_strcasecmp( tmp, MDMVS_TCP ))
    {
        snprintf( transport, length, "%d", DAL_VOICE_SIP_TRANSPORT_TCP);
    }
    else if( !cmsUtl_strcasecmp( tmp, MDMVS_TLS ))
    {
        snprintf( transport, length, "%d", DAL_VOICE_SIP_TRANSPORT_TLS);
    }
    else if( !cmsUtl_strcasecmp( tmp, MDMVS_SCTP ))
    {
        snprintf( transport, length, "%d", DAL_VOICE_SIP_TRANSPORT_SCTP);
    }
    else
    {
        return CMSRET_INVALID_PARAM_VALUE;
    }

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipNetworkCodecList
**
**  PURPOSE:        Gets codec list for a given SIP network
**
**  INPUT PARMS:    vpInst - parms->op[0],
**                  network instance - parms->op[1]
**
**  OUTPUT PARMS:   codec - list of codecs for the SIP network of interest
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipNetworkCodecList(DAL_VOICE_PARMS *parms, char *codec, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;

    char codecListFullPath[MAX_TR104_OBJ_SIZE];
    char *codecProfileFullPath = NULL;

    memset(codec, 0, length);
    memset(codecListFullPath, 0, MAX_TR104_OBJ_SIZE);

    /* get codecList full path, this might contains multiple codec profile pathes */
    GET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], codecList, codecListFullPath, sizeof(codecListFullPath));

    if( ret == CMSRET_SUCCESS && strlen( codecListFullPath ) > 0 )
    {
        int   numCP = 0;
        char *saveptr;
        char  delim[2] = ",";
        MdmPathDescriptor  pathDesc;

        /* find maximum codec profile in the system */
        ret = dalVoice_GetNumCodecProfile( parms, &numCP);
        if( ret == CMSRET_SUCCESS && numCP > 0 )
        {
            /* split codecList full path to individal codec profile path */
            codecProfileFullPath = strtok_r( codecListFullPath, delim, &saveptr);
            while(codecProfileFullPath != NULL)
            {
                /* convert codec profile path to iidStack */
                ret = cmsMdm_fullPathToPathDescriptor(codecProfileFullPath, &pathDesc);
                if(ret == CMSRET_SUCCESS && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
                {
                    DAL_VOICE_PARMS  localParms;
                    char codecName[TEMP_CHARBUF_SIZE];

                    memset(codecName, 0, sizeof(codecName));
                    memset(&localParms, 0, sizeof(DAL_VOICE_PARMS));
                    localParms.op[0] = parms->op[0];
                    localParms.op[1] = PEEK_INSTANCE_ID( &pathDesc.iidStack );
                    ret = dalVoice_GetCodecProfileName( &localParms, codecName, sizeof(codecName));
                    if( ret == CMSRET_SUCCESS && strlen( codecName ) > 0 )
                    {
                        strncat( codec, codecName, strlen(codecName));
                        strncat( codec, ",", strlen(","));
                    }
                }

                codecProfileFullPath = strtok_r( NULL, delim, &saveptr);
            }
        }
    }

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPktcOutboundProxyType
**
**  INPUT PARMS:    vpInst  - parms->op[0]
**                  netInst - parms->op[1]
**
**  OUTPUT PARMS:   getVal - Outbound Proxy type
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetPktcOutboundProxyType(DAL_VOICE_PARMS *parms, unsigned int *getVal)
{
    CmsRet ret = CMSRET_SUCCESS;
    SIPNetworkObject *obj=NULL;
    MdmObjectId __oid = MDMOID_SIP_NETWORK;

    GET_L4OBJ_PARAM_UINT( parms->op[0], parms->op[1], 0, 0, X_BROADCOM_COM_PKTC_OutboundProxyType, getVal);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSipNetworkVoipProfileIdx
**
**  PURPOSE:        Gets VoIP profile ID for the given SIP network
**
**  INPUT PARMS:    vpInst - parms->op[0],
**                  network instance - parms->op[1]
**
**  OUTPUT PARMS:   idx - VoIP profile ID for the SIPnetwork of interest
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSipNetworkVoipProfileIdx(DAL_VOICE_PARMS *parms, char *idx, unsigned int length )
{
    int     vpInst, vpIdx;
    CmsRet  ret = CMSRET_SUCCESS;
    char    profileFullPath[MAX_TR104_OBJ_SIZE];
    MdmPathDescriptor pathDesc;

    memset( idx, 0, length);
    memset( profileFullPath, 0, sizeof(profileFullPath));
    GET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], voIPProfile, profileFullPath, length);
    if( ret == CMSRET_SUCCESS && strlen( profileFullPath ) > 0 )
    {
        /* convert full path to network instance stack */
        ret = cmsMdm_fullPathToPathDescriptor(profileFullPath, &pathDesc);
        if(ret == CMSRET_SUCCESS && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
        {
            vpInst = POP_INSTANCE_ID( &pathDesc.iidStack );  /* voip profile instance */
            mapL2ObjectInstToNum( pathDesc.oid, POP_INSTANCE_ID(&pathDesc.iidStack), vpInst, &vpIdx );
            snprintf( idx, length, "%d", vpIdx );
        }
    }

    return ( ret );
}

/*******************************************************************
**             Codec Profile Interface                           ***
*******************************************************************/
#define SET_CODEC_PROF_PARAM_STR(i, p, n, v, f)         \
{                                                       \
    CodecProfileObject  *obj=NULL;                      \
    MdmObjectId        __oid = MDMOID_CODEC_PROFILE;    \
    SET_L2OBJ_PARAM_STR(i, p, n, v, f);                 \
}

#define GET_CODEC_PROF_PARAM_STR(i, p, n, v, l)         \
{                                                       \
    CodecProfileObject *obj=NULL;                       \
    MdmObjectId        __oid = MDMOID_CODEC_PROFILE;    \
    GET_L2OBJ_PARAM_STR(i, p, n, v, l);                 \
}

#define GET_CODEC_PROF_PARAM_BOOL(i, p, n, v, l)        \
{                                                       \
    CodecProfileObject *obj=NULL;                       \
    MdmObjectId        __oid = MDMOID_CODEC_PROFILE;    \
    GET_L2OBJ_PARAM_BOOL(i, p, n, v, l);                \
}

#define SET_CODEC_PROF_PARAM_BOOL(i, p, n, v)           \
{                                                       \
    CodecProfileObject *obj=NULL;                       \
    MdmObjectId        __oid = MDMOID_CODEC_PROFILE;    \
    SET_L2OBJ_PARAM_BOOL(i, p, n, v);                   \
}

/*****************************************************************
**  FUNCTION:       dalVoice_mapCpNumToInst
**
**  PURPOSE:        maps codec profile number to instance
**
**  INPUT PARMS:    vpInst - parms->op[0],
**                  codec profile number - parms->op[1]
**
**  OUTPUT PARMS:   cpInst - instance ID of the mapped codec profile
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_mapCpNumToInst(DAL_VOICE_PARMS *parms, int *cpInst)
{
    return mapL2ObjectNumToInst( MDMOID_CODEC_PROFILE, parms->op[0], parms->op[1], cpInst);
}

/*****************************************************************
**  FUNCTION:       dalVoice_DeleteCodecProfile
**
**  PURPOSE:        deletes a codec profile
**
**  INPUT PARMS:    vpInst - parms->op[0],
**                  codec profile instance - parms->op[1]
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_DeleteCodecProfile( DAL_VOICE_PARMS *parms )
{
    return delL2Object( MDMOID_CODEC_PROFILE, parms->op[0], parms->op[1] );
}

/*****************************************************************
**  FUNCTION:       dalVoice_AddCodecProfile
**
**  PURPOSE:        adds a codec profile
**
**  INPUT PARMS:    vpInst - parms->op[0],
**
**  OUTPUT PARMS:   inst - instance of the added codec profile
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_AddCodecProfile( DAL_VOICE_PARMS *parms, int *inst )
{
    return addL2Object( MDMOID_CODEC_PROFILE, parms->op[0], inst );
}

/*****************************************************************
**  FUNCTION:       dalVoice_AddCodecProfileByName
**
**  PURPOSE:        adds a codec profile
**
**  INPUT PARMS:    vpInst - parms->op[0],
**                  name   - name of the codec profile to be added
**
**  OUTPUT PARMS:   inst - instance of the added codec profile
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_AddCodecProfileByName( DAL_VOICE_PARMS *parms, const char *name, int *inst )
{
    CmsRet  ret = CMSRET_SUCCESS;
    DAL_VOICE_PARMS  parmsList;
    UINT32  cpInst = 0, codecInst = 0;

    if( parms == NULL || name == NULL || strlen(name) <=0 || inst == NULL )
        return CMSRET_INVALID_ARGUMENTS;

    memset(&parmsList, 0, sizeof(parmsList));
    parmsList.op[0] = parms->op[0];

    ret = dalVoice_MapCodecNameToCodecInst( &parmsList, name, (int *)&codecInst );
    if( ret == CMSRET_SUCCESS && codecInst > 0)
    {
        ret = dalVoice_AddCodecProfile( &parmsList, (int *)&cpInst);
        if(ret == CMSRET_SUCCESS && cpInst > 0)
        {
            parmsList.op[1] = cpInst;
            parmsList.op[2] = codecInst;
            dalVoice_SetCodecProfileAssoc( &parmsList );
            dalVoice_SetCodecProfPacketPeriod( &parmsList, "10,20" );
            *inst = codecInst;
        }
    }
    else
    {
        cmsLog_error("%s() invalid codec name (%s)", __FUNCTION__, name );
    }

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_MapCodecNameToCodecProfileInst
**
**  PURPOSE:        maps a codec name to codec profile instance
**
**  INPUT PARMS:    vpInst - parms->op[0],
**                  name   - name of the codec to be mapped
**
**  OUTPUT PARMS:   inst - instance of the mapped codec profile
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_MapCodecNameToCodecProfileInst( DAL_VOICE_PARMS *parms, const char *name, int *codecInst )
{
    CmsRet ret =  CMSRET_SUCCESS;
    VoiceCapCodecsObject *obj = NULL;
    CodecProfileObject *cpObj = NULL;
    int    numCodecProfile = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack searchIidStack = EMPTY_INSTANCE_ID_STACK;
    MdmPathDescriptor pathDesc;
    UBOOL8  found = FALSE;


    if( parms == NULL || name == NULL || codecInst == NULL )
    {
        cmsLog_error("%s invalid value", __FUNCTION__ );
        return CMSRET_INVALID_PARAM_VALUE;
    }
    else
    {
        *codecInst = 0;
    }

    PUSH_INSTANCE_ID(&iidStack, parms->op[0]);
    ret = dalVoice_GetNumCodecProfile( parms, &numCodecProfile);
    if( ret == CMSRET_SUCCESS && numCodecProfile > 0 )
    {
        while( !found && (cmsObj_getNextInSubTreeFlags(MDMOID_CODEC_PROFILE, &iidStack, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **) &cpObj)) == CMSRET_SUCCESS )
        {
            if( cpObj->codec && strlen(cpObj->codec)>0){
                cmsLog_debug("%s codec profile (%d) full path( %s )\n", __FUNCTION__, PEEK_INSTANCE_ID(&searchIidStack), cpObj->codec );
                ret = cmsMdm_fullPathToPathDescriptor(cpObj->codec, &pathDesc);
                if( ret == CMSRET_SUCCESS )
                {
                    ret = cmsObj_get( MDMOID_VOICE_CAP_CODECS, &(pathDesc.iidStack), OGF_NO_VALUE_UPDATE, (void **)&obj );
                    if( ret == CMSRET_SUCCESS )
                    {
                        if( obj->codec && !cmsUtl_strcasecmp( name, obj->codec ) ){
                            *codecInst = PEEK_INSTANCE_ID(&searchIidStack);
                            cmsLog_debug("%s found match codec ( %s ) codec profile instance (%d)", __FUNCTION__, name, *codecInst );
                            found = TRUE;
                        }
                        cmsObj_free((void **)&obj);
                    }
                }
            }

            cmsObj_free((void **)&cpObj);
        }
        if( found ){
            return CMSRET_SUCCESS;
        }
        else{
            return CMSRET_INVALID_PARAM_NAME;
        }
    }

    return CMSRET_INVALID_PARAM_VALUE;
}

/*****************************************************************
**  FUNCTION:       dalVoice_MapCodecNameToCodecInst
**
**  PURPOSE:        maps a codec name to codec capability instance
**
**  INPUT PARMS:    vpInst - parms->op[0],
**                  name   - name of the codec to be mapped
**
**  OUTPUT PARMS:   inst - instance of the mapped codec capability
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_MapCodecNameToCodecInst( DAL_VOICE_PARMS *parms, const char *name, int *codecInst )
{
    CmsRet ret =  CMSRET_SUCCESS;
    VoiceCapCodecsObject *obj = NULL;
    int    numCodec = 0;
    int    svcIdx =  0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack searchIidStack = EMPTY_INSTANCE_ID_STACK;


    if( parms == NULL || name == NULL || strlen( name ) == 0 || codecInst == NULL )
    {
        cmsLog_error("%s invalid value", __FUNCTION__ );
        return CMSRET_INVALID_PARAM_VALUE;
    }
    else
    {
        *codecInst = 0;
    }

    svcIdx = parms->op[0];
    cmsLog_debug("%s codec name (%s)", __FUNCTION__, name );

    ret = dalVoice_GetNumOfCodecs(svcIdx, &numCodec);
    if( ret == CMSRET_SUCCESS && numCodec > 0 )
    {
        PUSH_INSTANCE_ID(&iidStack, svcIdx);
        while ( (ret = cmsObj_getNextInSubTreeFlags(MDMOID_VOICE_CAP_CODECS, &iidStack, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **) &obj)) == CMSRET_SUCCESS )
        {
            if( !cmsUtl_strcasecmp( name, obj->codec )){
                *codecInst = PEEK_INSTANCE_ID(&searchIidStack);
                cmsObj_free((void **)&obj);

                cmsLog_debug("%s found match codec ( %s ) instance (%d)", __FUNCTION__, name, *codecInst );
                return ret;
            }

            cmsObj_free((void **)&obj);
        }
        return CMSRET_INVALID_PARAM_NAME;
    }

    return CMSRET_INVALID_PARAM_VALUE;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSuppCodecsString
**
**  PURPOSE:        Returns a list of supported codecs from Capabilities object
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - comma separated list of codecs
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSuppCodecsString( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   unsigned int count = 0;
   unsigned int numCodecs = 0;
   char tempBuf[TEMP_CHARBUF_SIZE];

   ret = dalVoice_GetMaxSuppCodecs(parms, tempBuf, TEMP_CHARBUF_SIZE);
   if (ret != CMSRET_SUCCESS)
   {
      return ret;
   }

   numCodecs = atoi(tempBuf);

   for (count = 0; count < numCodecs; count++)
   {
      GET_CODEC_CAP_PARAM_STR(parms->op[0], count, codec, tempBuf, TEMP_CHARBUF_SIZE);
      strcat(value, tempBuf);
      strcat(value, ",");
   }

   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetMaxSuppCodecs
**
**  PURPOSE:        Returns the number of supported codecs from Capabilities object
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - integer value of number of supp codecs
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetMaxSuppCodecs( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_VOICE_CAP_PARAM_INT_AS_STR(parms->op[0], parms->op[1], codecNumberOfEntries, value, length);

   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSuppCodecBitRate
**
**  PURPOSE:        Returns the bit rate of a given codec from Capabilities object
**
**  INPUT PARMS:
**
**  OUTPUT PARMS:   rate - integer value of number of supp codecs
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSuppCodecBitRate( DAL_VOICE_PARMS *parms, int* rate )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CODEC_CAP_PARAM_INT(parms->op[0], parms->op[1], bitRate, rate);

   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSuppCodecPacketizationPeriod
**
**  PURPOSE:        Returns the packetization period of codec from Capabilities object
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - string with integer values
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSuppCodecPacketizationPeriod( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CODEC_CAP_PARAM_STR(parms->op[0], parms->op[1], packetizationPeriod, value, length);

   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSuppCodecSilSupp
**
**  PURPOSE:        Returns the silence supp param for a given codec from Capabilities object
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - true if silence supp is supported, false otherwise
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSuppCodecSilSupp( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CODEC_CAP_PARAM_BOOL(parms->op[0], parms->op[1], silenceSuppression, value, length);

   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCodecProfileName
**
**  PURPOSE:        Returns the name of the codec in codec profile
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - codec name
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCodecProfileName( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
    CmsRet   ret = CMSRET_SUCCESS;
    char  codecFullPath[MAX_TR104_OBJ_SIZE];
    MdmPathDescriptor pathDesc;

    if( parms == NULL || value == NULL || length <= 0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    memset( codecFullPath, 0, sizeof( codecFullPath ));
    /* get codec full path */
    GET_CODEC_PROF_PARAM_STR(parms->op[0], parms->op[1], codec, codecFullPath, MAX_TR104_OBJ_SIZE);

    /* convert codec full path to iidStack */
    if( ret == CMSRET_SUCCESS && strlen( codecFullPath ) > 0 )
    {
        ret = cmsMdm_fullPathToPathDescriptor(codecFullPath, &pathDesc);
        if(ret == CMSRET_SUCCESS && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
        {
            VoiceCapCodecsObject *capCodecObj = NULL;
            /* get codec name */
            ret = cmsObj_get( MDMOID_VOICE_CAP_CODECS, &pathDesc.iidStack, OGF_NO_VALUE_UPDATE, (void **)&capCodecObj );
            if( ret == CMSRET_SUCCESS && capCodecObj->codec ){
                strncpy( value, capCodecObj->codec, length);
                cmsObj_free((void **)&capCodecObj );
            }
        }
    }

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCodecProfileEnable
**
**  PURPOSE:        Returns the packetization period of the codec in codec profile
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - codec packetization period(s), comma separated
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCodecProfileEnable( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CODEC_PROF_PARAM_BOOL(parms->op[0], parms->op[1], enable, value, length);

   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCodecProfPacketPeriod
**
**  PURPOSE:        Returns the packetization period of the codec in codec profile
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - codec packetization period(s), comma separated
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCodecProfPacketPeriod( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CODEC_PROF_PARAM_STR(parms->op[0], parms->op[1], packetizationPeriod, value, length);

   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCodecProfSilSupp
**
**  PURPOSE:        Returns the packetization period of the codec in codec profile
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - silence suppression, true/false
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCodecProfSilSupp( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CODEC_PROF_PARAM_BOOL(parms->op[0], parms->op[1], silenceSuppression, value, length);

   return ( CMSRET_SUCCESS );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetSilenceSuppression
**
**  PURPOSE:
**
**  INPUT PARMS:    vpInst - parms->op[0];
**
**  OUTPUT PARMS:   vad - Silence suppression
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetSilenceSuppression(DAL_VOICE_PARMS *parms, char *vad, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;
    int    cpInst, numberOfProf;

    if( parms == NULL || vad == NULL || length <=0 )
        return CMSRET_INVALID_ARGUMENTS;

    GET_VOICE_SVC_PARAM_UINT( parms->op[0], codecProfileNumberOfEntries, &numberOfProf);
    if( ret == CMSRET_SUCCESS && numberOfProf > 0 )
    {
        /* get the first codec profile silence suppression flag
        ** assume that the vad flag is same across all codec profiles
        */
        mapL2ObjectNumToInst( MDMOID_CODEC_PROFILE, parms->op[0], 0, &cpInst );
        GET_CODEC_PROF_PARAM_BOOL( parms->op[0], cpInst, silenceSuppression, vad, length );
    }
    else
    {
        strncpy( vad, MDMVS_OFF, length);
    }

    return ret;
}

/***************************************************************************
* Function Name: dalVoice_SetSilenceSuppression
* Description  : CLI wrapper for SetVlCLSilenceSuppression()
*                Indicates support for silence suppression for this codec.
*                VoiceProfile.{i}.Line{i}.Codec.List.{i}.SilenceSuppression = new value
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetSilenceSuppression( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet ret = CMSRET_SUCCESS;
    int    i, numberOfProf, cpInst;

    if( parms == NULL || value == NULL || strlen(value) <=0 )
        return CMSRET_INVALID_ARGUMENTS;

    GET_VOICE_SVC_PARAM_UINT( parms->op[0], codecProfileNumberOfEntries, &numberOfProf);
    if( ret == CMSRET_SUCCESS && numberOfProf > 0 )
    {
        for( i=0; i < numberOfProf; i++)
        {
            mapL2ObjectNumToInst( MDMOID_CODEC_PROFILE, parms->op[0], i, &cpInst );
            SET_CODEC_PROF_PARAM_BOOL( parms->op[0], cpInst, silenceSuppression, value );
        }
    }

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCodecProfPacketPeriod
**
**  PURPOSE:        Sets the packetization period of the codec in codec profile
**
**  INPUT PARMS:    length - size of string buffer
**
**  OUTPUT PARMS:   value - codec packetization period(s), comma separated
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCodecProfPacketPeriod( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_CODEC_PROF_PARAM_STR(parms->op[0], parms->op[1], packetizationPeriod, value, NULL);

   return ( CMSRET_SUCCESS );
}



/********************************************************************
**           Incoming/Outgoing Map Interface                       **
******************************************** ***********************/
#define GET_INCOMING_MAP_PARAM_STR( i, p, n, v, l ) \
{                                                   \
    CallControlIncomingMapObject *obj=NULL;         \
    MdmObjectId __oid = MDMOID_CALL_CONTROL_INCOMING_MAP;\
    GET_L2OBJ_PARAM_STR(i, p, n, v, l)              \
}

#define GET_INCOMING_MAP_PARAM_UINT( i, p, n, v, l ) \
{                                                    \
    CmsRet ret = CMSRET_SUCCESS;                     \
    CallControlIncomingMapObject *obj=NULL;          \
    MdmObjectId __oid = MDMOID_CALL_CONTROL_INCOMING_MAP;\
    GET_L2OBJ_PARAM_UINT(i, p, n, v, l)              \
    return ret;                                      \
}

#define GET_OUTGOING_MAP_PARAM_STR( i, p, n, v, l ) \
{                                                   \
    CallControlOutgoingMapObject *obj=NULL;         \
    MdmObjectId __oid = MDMOID_CALL_CONTROL_OUTGOING_MAP;\
    GET_L2OBJ_PARAM_STR(i, p, n, v, l)              \
}

/*****************************************************************
**  FUNCTION:       dalVoice_AddCallCtrlIncomingMap
**
**  PURPOSE:        Adds a call control incoming call map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**
**  OUTPUT PARMS:   instance of added object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_AddCallCtrlIncomingMap( DAL_VOICE_PARMS *parms, int *inst )
{
    return addL2Object( MDMOID_CALL_CONTROL_INCOMING_MAP, parms->op[0], inst );
}

/*****************************************************************
**  FUNCTION:       dalVoice_DeleteCallCtrlIncomingMap
**
**  PURPOSE:        deletes a call control incoming call map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the object to be deleted
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_DeleteCallCtrlIncomingMap( DAL_VOICE_PARMS *parms )
{
    return delL2Object( MDMOID_CALL_CONTROL_INCOMING_MAP, parms->op[0], parms->op[1] );
}

/*****************************************************************
**  FUNCTION:       dalVoice_mapIncomingMapNumToInst
**
**  PURPOSE:        maps incoming map to map instance
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the object to be mapped
**
**  OUTPUT PARMS:   mapInst - instance of the map object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_mapIncomingMapNumToInst( DAL_VOICE_PARMS *parms,  int *mapInst )
{
    return mapL2ObjectNumToInst( MDMOID_CALL_CONTROL_INCOMING_MAP, parms->op[0], parms->op[1], mapInst);
}

/*****************************************************************
**  FUNCTION:       dalVoice_mapLineExtToIncomingMapInst
**
**  PURPOSE:        maps line+ext number to incoming map instance
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - line number to be mapped
**                  parms->op[2] - ext number to be mapped
**
**  OUTPUT PARMS:   mapInst - instance of the map object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_mapLineExtToIncomingMapInst( DAL_VOICE_PARMS *parms,  int *mapInst )
{
    int numMap = 0;
    DAL_VOICE_PARMS localParms;
    UBOOL8    enabled = FALSE;
    CmsRet    ret;

    int ccInMapInst, mapLineNum, mapExtNum;
    char tmpStr[LINE_EXT_BUF_SIZE_MAX];

    if( parms == NULL || mapInst == NULL)
    {
       /* No map instances in the system. Valid scenario where no line-extension mappings exist */
       return CMSRET_INVALID_ARGUMENTS;
    }

    (*mapInst) = -1;  /* set mapInst as invalid value */

    localParms.op[0] = parms->op[0];
    ret = dalVoice_GetNumIncomingMap(&localParms, &numMap);
    if (CMSRET_SUCCESS != ret )
    {
       /* No map instances in the system. Valid scenario where no line-extension mappings exist */
       return ret;
    }

    for (int i = 0; i < numMap; i++)
    {
        localParms.op[1] = i;
        if( dalVoice_mapIncomingMapNumToInst( &localParms, &ccInMapInst ) == CMSRET_SUCCESS)
        {
           localParms.op[1] = ccInMapInst;
           dalVoice_GetIncomingMapLineNum(&localParms, tmpStr, LINE_EXT_BUF_SIZE_MAX);
           mapLineNum = atoi(tmpStr);
           dalVoice_GetIncomingMapExtNum(&localParms, tmpStr, LINE_EXT_BUF_SIZE_MAX);
           mapExtNum = atoi(tmpStr);
           dalVoice_GetIncomingMapEnable(&localParms, tmpStr, LINE_EXT_BUF_SIZE_MAX);
           ret = convertStringToBool( tmpStr, &enabled );
           if( ret == CMSRET_SUCCESS && enabled && (mapLineNum == parms->op[1]) && (mapExtNum == parms->op[2]))
           {
               *mapInst = ccInMapInst;
               return CMSRET_SUCCESS;
           }
        }
    }

    /* Could not find any mappings */
    return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetIncomingMapEnable
**
**  PURPOSE:        gets the enable flag of the given map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**
**  OUTPUT PARMS:   value - value of the enable flag
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetIncomingMapEnable( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   CallControlIncomingMapObject *obj=NULL;
   MdmObjectId __oid = MDMOID_CALL_CONTROL_INCOMING_MAP;

   GET_L2OBJ_PARAM_BOOL(parms->op[0], parms->op[1], enable, value, length);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetIncomingMapLineNum
**
**  PURPOSE:        gets the line number of the given map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**
**  OUTPUT PARMS:   value - value of the line number
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetIncomingMapLineNum( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   char     fullPath[MAX_TR104_OBJ_SIZE];
   MdmPathDescriptor pathDesc;

   GET_INCOMING_MAP_PARAM_STR( parms->op[0], parms->op[1], line, fullPath, sizeof(fullPath));
   if( ret == CMSRET_SUCCESS && strlen( fullPath ) > 0 )
   {
      cmsLog_debug("%s() full path(%s)", __FUNCTION__, fullPath);
      ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
      if ( CMSRET_SUCCESS == ret && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
      {
         int L1Inst, L2Inst, num = -1;
         L2Inst = POP_INSTANCE_ID( &pathDesc.iidStack );
         L1Inst = POP_INSTANCE_ID( &pathDesc.iidStack );
         ret = mapL2ObjectInstToNum( pathDesc.oid, L1Inst, L2Inst, &num);
         if( ret == CMSRET_SUCCESS && num >= 0 ){
            snprintf( value, length, "%d", num );
         }
         else{
            ret = CMSRET_INVALID_PARAM_VALUE;
         }
      }
   }

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetIncomingMapExtNum
**
**  PURPOSE:        gets the extension number of the given map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**
**  OUTPUT PARMS:   value - value of the ext number
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetIncomingMapExtNum( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   char     fullPath[MAX_TR104_OBJ_SIZE];
   MdmPathDescriptor pathDesc;

   GET_INCOMING_MAP_PARAM_STR( parms->op[0], parms->op[1], extension, fullPath, sizeof(fullPath));
   if( ret == CMSRET_SUCCESS && strlen( fullPath ) > 0 )
   {
      cmsLog_debug("%s() full path(%s)", __FUNCTION__, fullPath);
      ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
      if ( CMSRET_SUCCESS == ret && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
      {
         int L1Inst, L2Inst, num = -1;
         L2Inst = POP_INSTANCE_ID( &pathDesc.iidStack );
         L1Inst = POP_INSTANCE_ID( &pathDesc.iidStack );
         ret = mapL2ObjectInstToNum( pathDesc.oid, L1Inst, L2Inst, &num);
         if( ret == CMSRET_SUCCESS && num >= 0 ){
            snprintf( value, length, "%d", num );
         }
         else{
            ret = CMSRET_INVALID_PARAM_VALUE;
         }
      }
   }

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetIncomingMapOrder
**
**  PURPOSE:        gets the order parameter of the given map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**
**  OUTPUT PARMS:   value - value of the order parameter
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetIncomingMapOrder( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   GET_INCOMING_MAP_PARAM_UINT( parms->op[0], parms->op[1], order, value, length);
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetIncomingMapTimeout
**
**  PURPOSE:        gets the timeout parameter of the given map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**
**  OUTPUT PARMS:   value - value of the timeout parameter
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetIncomingMapTimeout( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   GET_INCOMING_MAP_PARAM_UINT( parms->op[0], parms->op[1], timeout, value, length);
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetIncomingMapEnabled
**
**  PURPOSE:        gets the enable flag of the given map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**
**  OUTPUT PARMS:   value - value of the enable flag
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetIncomingMapEnabled( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet   ret = CMSRET_SUCCESS;
   CallControlIncomingMapObject *obj=NULL;
   MdmObjectId __oid = MDMOID_CALL_CONTROL_INCOMING_MAP;

   SET_L2OBJ_PARAM_BOOL(parms->op[0], parms->op[1], enable, value);

   return ret;
}


/*****************************************************************
**  FUNCTION:       dalVoice_SetIncomingMapLineExt
**
**  PURPOSE:        sets the incoming map's line and ext number
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**                  parms->op[2] - instance of the line object
**                  parms->op[3] - instance of the ext object
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetIncomingMapLineExt( DAL_VOICE_PARMS *parms )
{
    CmsRet   ret = CMSRET_SUCCESS;

    ret = setL2ToL2ObjAssoc( MDMOID_CALL_CONTROL_INCOMING_MAP, MDMOID_CALL_CONTROL_LINE, parms->op[0], parms->op[1], parms->op[2]);
    if( ret != CMSRET_SUCCESS )
        return ret;

    ret = setL2ToL2ObjAssoc( MDMOID_CALL_CONTROL_INCOMING_MAP, MDMOID_CALL_CONTROL_EXTENSION, parms->op[0], parms->op[1], parms->op[3]);
    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_mapLineExtToOutgoingMapInst
**
**  PURPOSE:        maps line+ext number to outgoing map instance
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - line number to be mapped
**                  parms->op[2] - ext number to be mapped
**
**  OUTPUT PARMS:   mapInst - instance of the map object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_mapLineExtToOutgoingMapInst( DAL_VOICE_PARMS *parms,  int *mapInst )
{
    int numMap = 0;
    DAL_VOICE_PARMS localParms;

    int ccOutMapInst, mapLineNum, mapExtNum;
    char tmpStr[LINE_EXT_BUF_SIZE_MAX];

    localParms.op[0] = parms->op[0];
    if (CMSRET_SUCCESS != dalVoice_GetNumOutgoingMap(&localParms, &numMap))
    {
       /* No map instances in the system. Valid scenario where no line-extension mappings exist */
       *mapInst = -1;
       return CMSRET_SUCCESS;
    }

    for (int i = 0; i < numMap; i++)
    {
        localParms.op[1] = i;
        if( dalVoice_mapOutgoingMapNumToInst( &localParms, &ccOutMapInst ) == CMSRET_SUCCESS)
        {
           localParms.op[1] = ccOutMapInst;
           dalVoice_GetOutgoingMapLineNum(&localParms, tmpStr, LINE_EXT_BUF_SIZE_MAX);
           mapLineNum = atoi(tmpStr);
           dalVoice_GetOutgoingMapExtNum(&localParms, tmpStr, LINE_EXT_BUF_SIZE_MAX);
           mapExtNum = atoi(tmpStr);
           dalVoice_GetOutgoingMapEnable(&localParms, tmpStr, LINE_EXT_BUF_SIZE_MAX);
           if((!strcmp(tmpStr, "Yes")) && (mapLineNum == parms->op[1]) && (mapExtNum == parms->op[2]))
           {
               *mapInst = ccOutMapInst;
               return CMSRET_SUCCESS;
           }
        }
    }

    /* Could not find any mappings */
    *mapInst = -1;
    return CMSRET_SUCCESS;
}

/*****************************************************************
**  FUNCTION:       dalVoice_AddCallCtrlOutgoingMap
**
**  PURPOSE:        Adds a call control outgoing call map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**
**  OUTPUT PARMS:   instance of added object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_AddCallCtrlOutgoingMap( DAL_VOICE_PARMS *parms, int *inst )
{
    return addL2Object( MDMOID_CALL_CONTROL_OUTGOING_MAP, parms->op[0], inst );
}

/*****************************************************************
**  FUNCTION:       dalVoice_DeleteCallCtrlOutgoingMap
**
**  PURPOSE:        deletes a call control outgoing call map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the object to be deleted
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_DeleteCallCtrlOutgoingMap( DAL_VOICE_PARMS *parms )
{
    return delL2Object( MDMOID_CALL_CONTROL_OUTGOING_MAP, parms->op[0], parms->op[1] );
}

/*****************************************************************
**  FUNCTION:       dalVoice_mapOutgoingMapNumToInst
**
**  PURPOSE:        maps outgoing map to map instance
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the object to be mapped
**
**  OUTPUT PARMS:   mapInst - instance of the map object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_mapOutgoingMapNumToInst( DAL_VOICE_PARMS *parms, int *mapInst )
{
    return mapL2ObjectNumToInst( MDMOID_CALL_CONTROL_OUTGOING_MAP, parms->op[0], parms->op[1], mapInst);
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetOutgoingMapEnable
**
**  PURPOSE:        gets the enable flag of the given map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**
**  OUTPUT PARMS:   value - value of the enable flag
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetOutgoingMapEnable( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   CallControlOutgoingMapObject *obj=NULL;
   MdmObjectId __oid = MDMOID_CALL_CONTROL_OUTGOING_MAP;

   GET_L2OBJ_PARAM_BOOL( parms->op[0], parms->op[1], enable, value, length);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetOutgoingMapLineNum
**
**  PURPOSE:        gets the line number of the given map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**
**  OUTPUT PARMS:   value - value of the line number
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetOutgoingMapLineNum( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   char     fullPath[MAX_TR104_OBJ_SIZE];
   MdmPathDescriptor pathDesc;

   GET_OUTGOING_MAP_PARAM_STR( parms->op[0], parms->op[1], line, fullPath, sizeof(fullPath));
   if( ret == CMSRET_SUCCESS && strlen( fullPath ) > 0 )
   {
      cmsLog_debug("%s() full path(%s)", __FUNCTION__, fullPath);
      ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
      if ( CMSRET_SUCCESS == ret && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
      {
         int L1Inst, L2Inst, num = -1;
         L2Inst = POP_INSTANCE_ID( &pathDesc.iidStack );
         L1Inst = POP_INSTANCE_ID( &pathDesc.iidStack );
         ret = mapL2ObjectInstToNum( pathDesc.oid, L1Inst, L2Inst, &num);
         if( ret == CMSRET_SUCCESS && num >= 0 ){
            snprintf( value, length, "%d", num );
         }
         else{
            ret = CMSRET_INVALID_PARAM_VALUE;
         }
      }
   }

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetOutgoingMapExtNum
**
**  PURPOSE:        gets the extension number of the given map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**
**  OUTPUT PARMS:   value - value of the ext number
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetOutgoingMapExtNum( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   char     fullPath[MAX_TR104_OBJ_SIZE];
   MdmPathDescriptor pathDesc;

   GET_OUTGOING_MAP_PARAM_STR( parms->op[0], parms->op[1], extension, fullPath, sizeof(fullPath));
   if( ret == CMSRET_SUCCESS && strlen( fullPath ) > 0 )
   {
      cmsLog_debug("%s() full path(%s)", __FUNCTION__, fullPath);
      ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
      if ( CMSRET_SUCCESS == ret && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
      {
         int L1Inst, L2Inst, num = -1;
         L2Inst = POP_INSTANCE_ID( &pathDesc.iidStack );
         L1Inst = POP_INSTANCE_ID( &pathDesc.iidStack );
         ret = mapL2ObjectInstToNum( pathDesc.oid, L1Inst, L2Inst, &num);
         if( ret == CMSRET_SUCCESS && num >= 0 ){
            snprintf( value, length, "%d", num );
         }
         else{
            ret = CMSRET_INVALID_PARAM_VALUE;
         }
      }
   }

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetOutgoingMapOrder
**
**  PURPOSE:        gets the order parameter of the given map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**
**  OUTPUT PARMS:   value - value of the order parameter
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetOutgoingMapOrder( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
   CmsRet ret = CMSRET_SUCCESS;
   CallControlOutgoingMapObject *obj=NULL;
   MdmObjectId __oid = MDMOID_CALL_CONTROL_OUTGOING_MAP;

   GET_L2OBJ_PARAM_UINT(parms->op[0], parms->op[1], order, value, length);

   return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetOutgoingMapEnable
**
**  PURPOSE:        set the enable flag of the given map object
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**
**  OUTPUT PARMS:   value - value of the enable flag
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetOutgoingMapEnabled( DAL_VOICE_PARMS *parms, char *value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   CallControlOutgoingMapObject *obj=NULL;
   MdmObjectId __oid = MDMOID_CALL_CONTROL_OUTGOING_MAP;

   SET_L2OBJ_PARAM_BOOL( parms->op[0], parms->op[1], enable, value);

   return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_SetOutgoingMapLineExt
**
**  PURPOSE:        sets the incoming map's line and ext number
**
**  INPUT PARMS:    parms->op[0] - voice service profile instance
**                  parms->op[1] - instance of the map object of interest
**                  parms->op[2] - instance of the line object
**                  parms->op[3] - instance of the ext object
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetOutgoingMapLineExt( DAL_VOICE_PARMS *parms )
{
    CmsRet   ret = CMSRET_SUCCESS;
    ret = setL2ToL2ObjAssoc( MDMOID_CALL_CONTROL_OUTGOING_MAP, MDMOID_CALL_CONTROL_LINE, parms->op[0], parms->op[1], parms->op[2]);
    if( ret != CMSRET_SUCCESS )
        return ret;

    ret = setL2ToL2ObjAssoc( MDMOID_CALL_CONTROL_OUTGOING_MAP, MDMOID_CALL_CONTROL_EXTENSION, parms->op[0], parms->op[1], parms->op[3]);
    return ( ret );
}

/***********************************************************************
***                   Call Control Interface                        ****
***********************************************************************/
#define GET_CALLCTL_PARAM_UINT(i, n, v)             \
{                                                   \
    CallControlObject *obj=NULL;                    \
    MdmObjectId       __oid = MDMOID_CALL_CONTROL;  \
    GET_L1OBJ_PARAM_UINT( i, n, v );                \
}

#define GET_CALLCTL_PARAM_STR(i, n, v, l)           \
{                                                   \
    CallControlObject *obj=NULL;                    \
    MdmObjectId       __oid = MDMOID_CALL_CONTROL;  \
    GET_L1OBJ_PARAM_STR( i, n, v, l);               \
}

#define SET_CALLCTL_PARAM_STR(i, n, v, f)           \
{                                                   \
    CallControlObject *obj=NULL;                    \
    MdmObjectId       __oid = MDMOID_CALL_CONTROL;  \
    SET_L1OBJ_PARAM_STR( i, n, v, f);               \
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
    CmsRet   ret = CMSRET_SUCCESS;
    if( parms && value && strlen(value) > 0 )
    {
        SET_CALLCTL_PARAM_STR( parms->op[0], X_BROADCOM_COM_DialPlan, value, NULL);
    }
    else
    {
        ret = CMSRET_INVALID_ARGUMENTS;
    }

    return   ret;
}

/***************************************************************************
* Function Name: dalVoice_SetPstnDialPlan
* Description  : Set the PSTN outgoing dial plan
*
* Parameters   : parms->op[0] = vpInst, parms->op[1] = lineInst, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_SetPstnDialPlan( DAL_VOICE_PARMS *parms, char *value )
{
    CmsRet ret = CMSRET_SUCCESS;
    if( parms && value )
    {
        SET_CALLCTL_PARAM_STR( parms->op[0], X_BROADCOM_COM_PstnDialPlan, value, NULL);
    }
    else
    {
        ret = CMSRET_INVALID_ARGUMENTS;
    }
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPstnDialPlan
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;
    if( parms && dialPlan && length > 0 )
    {
        GET_CALLCTL_PARAM_STR( parms->op[0], X_BROADCOM_COM_PstnDialPlan, dialPlan, length);
    }
    else
    {
        ret = CMSRET_INVALID_ARGUMENTS;
    }
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetDigitMap
**
**  PURPOSE:
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
    CmsRet   ret = CMSRET_SUCCESS;
    if( parms && map && length > 0 )
    {
        GET_CALLCTL_PARAM_STR( parms->op[0], X_BROADCOM_COM_DialPlan, map, length);
    }
    else
    {
        ret = CMSRET_INVALID_ARGUMENTS;
    }

    return   ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCCTKDigitMap
**
**  PURPOSE:
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
    CmsRet ret = CMSRET_SUCCESS;

    if( parms && map && length > 0 )
    {
        GET_CALLCTL_PARAM_STR( parms->op[0], X_BROADCOM_COM_CCTK_DigitMap, map, length);
    }
    else
    {
        ret = CMSRET_INVALID_ARGUMENTS;
    }

    return ret;
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNumIncomingMap
**
**  PURPOSE:        returns total number of incoming map entries per specific voice service
**
**  INPUT PARMS:    op[0] - SIP service instnace;
**
**  OUTPUT PARMS:   Number of incoming map entries in this service provider
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumIncomingMap( DAL_VOICE_PARMS *parms, int *numOfMap )
{
    CmsRet   ret = CMSRET_SUCCESS;

    *numOfMap = 0;
    GET_CALLCTL_PARAM_UINT( parms->op[0], incomingMapNumberOfEntries, numOfMap );

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumOutgoingMap
**
**  PURPOSE:        returns total number of outgoing map entries per specific voice service
**
**  INPUT PARMS:    op[0] - SIP service instnace;
**
**  OUTPUT PARMS:   Number of outgoing map entries in this service provider
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumOutgoingMap( DAL_VOICE_PARMS *parms, int *numOfMap )
{
    CmsRet   ret = CMSRET_SUCCESS;

    *numOfMap = 0;
    GET_CALLCTL_PARAM_UINT( parms->op[0], outgoingMapNumberOfEntries, numOfMap );

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumOfLine
**
**  PURPOSE:       returns total number of extension per specific voice service
**
**  INPUT PARMS:    op[0] - SIP service instnace;
**
**  OUTPUT PARMS:   Number of extension in this service provider
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumOfLine( DAL_VOICE_PARMS *parms, int *numAcc )
{
    CmsRet   ret = CMSRET_SUCCESS;

    *numAcc = 0;
    GET_CALLCTL_PARAM_UINT( parms->op[0], lineNumberOfEntries, numAcc );

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumOfLineStr
**
**  PURPOSE:        returns total number of extension per specific voice service, in string form
**
**  INPUT PARMS:    op[0] - SIP service instnace;
**
**  OUTPUT PARMS:   Number of extension in this service provider
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumOfLineStr( DAL_VOICE_PARMS *parms, char *value, unsigned int length)
{
    CmsRet   ret = CMSRET_SUCCESS;

    int numLine = 0;
    memset( value, 0, length );
    ret = dalVoice_GetNumOfLine( parms, &numLine );

    if (ret == CMSRET_SUCCESS)
    {
       snprintf( value, length, "%u", numLine);
    }

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumOfActiveLine
**
**  PURPOSE:       returns total number of actived line
**                 the actived line defined as:
**                     line is enabled
**                     line has associated provider ( SIP client )
**                     provider is active  ( SIP client is enabled as well )
**
**  INPUT PARMS:    op[0] - SIP service instnace;
**
**  OUTPUT PARMS:   Number of active call control line
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumOfActiveLine( DAL_VOICE_PARMS *parms, int *numAcc )
{
    CmsRet   ret = CMSRET_SUCCESS;
    int      i, srcInst, destInst, total;
    MdmObjectId  destOid;
    UBOOL8   found = FALSE, enabled;
    char     tmp[32];
    DAL_VOICE_PARMS localparms;

    *numAcc = 0;
    GET_CALLCTL_PARAM_UINT( parms->op[0], lineNumberOfEntries, &total );
    if( ret != CMSRET_SUCCESS || total <= 0 )
        return ret;

    for( i = 0; i < total; i++ )
    {
        found = FALSE;
        ret = mapL2ObjectNumToInst( MDMOID_CALL_CONTROL_LINE, parms->op[0], i, &srcInst);
        if(ret != CMSRET_SUCCESS || srcInst <= 0 )
            continue;

        memset( tmp, 0, sizeof(tmp));
        localparms.op[0] = parms->op[0];
        localparms.op[1] = srcInst;

        ret = dalVoice_GetCallCtrlLineEnable( &localparms, tmp, sizeof(tmp));
        if( ret == CMSRET_SUCCESS && convertStringToBool( tmp, &enabled ) == CMSRET_SUCCESS && enabled )
        {
            ret = getL2ToL2ObjAssocType( MDMOID_CALL_CONTROL_LINE, &destOid, parms->op[0], srcInst, &destInst);
            if( ret != CMSRET_SUCCESS || destInst <= 0 )
                continue;

            if( destOid  == MDMOID_SIP_CLIENT ){
                memset( tmp, 0, sizeof(tmp));
                GET_SIP_CLIENT_PARAM_BOOL( parms->op[0], destInst, enable, tmp, sizeof(tmp));
                if( ret == CMSRET_SUCCESS && convertStringToBool( tmp, &enabled ) == CMSRET_SUCCESS && enabled ){
                    found = TRUE;
                }
            }
        }
        (*numAcc) += found?1:0;
    }

    return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetNumOfExtension
**
**  PURPOSE:        returns total number of extension per specific voice service
**
**  INPUT PARMS:    op[0] - SIP service instnace;
**
**  OUTPUT PARMS:   Number of extension in this service provider
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumOfExtension( DAL_VOICE_PARMS *parms, int *numAcc )
{
    CmsRet   ret = CMSRET_SUCCESS;

    *numAcc = 0;
    GET_CALLCTL_PARAM_UINT( parms->op[0], extensionNumberOfEntries, numAcc );

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumOfExtensionStr
**
**  PURPOSE:        returns total number of extension per specific voice service, in string form
**
**  INPUT PARMS:    op[0] - SIP service instnace;
**
**  OUTPUT PARMS:   Number of extension in this service provider
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumOfExtensionStr( DAL_VOICE_PARMS *parms, char *value, unsigned int length)
{
    CmsRet   ret = CMSRET_SUCCESS;

    int numExt = 0;
    memset( value, 0, length );

    ret = dalVoice_GetNumOfExtension( parms, &numExt );

    if (ret == CMSRET_SUCCESS)
    {
       snprintf( value, length, "%u", numExt);
    }

    return ( ret );
}

/***********************************************************************
***          Call Control Line/Extension Interface                  ****
***********************************************************************/
#define GET_CC_EXT_PARAM_STR( i, p, n, v, l )       \
{                                                   \
    CallControlExtensionObject *obj=NULL;           \
    MdmObjectId __oid = MDMOID_CALL_CONTROL_EXTENSION;\
    GET_L2OBJ_PARAM_STR(i, p, n, v, l)              \
}

#define GET_CC_EXT_PARAM_BOOL( i, p, n, v, l )      \
{                                                   \
    CallControlExtensionObject *obj=NULL;           \
    MdmObjectId __oid = MDMOID_CALL_CONTROL_EXTENSION;\
    GET_L2OBJ_PARAM_BOOL(i, p, n, v, l)             \
}

#define SET_CC_EXT_PARAM_STR( i, p, n, v, f )       \
{                                                   \
    CallControlExtensionObject *obj=NULL;            \
    MdmObjectId  __oid = MDMOID_CALL_CONTROL_EXTENSION;\
    SET_L2OBJ_PARAM_STR(i, p, n, v, f);             \
}

#define SET_CC_LINE_RTP_STATS_UINT( i, p, n, v, f )  \
{                                                    \
    LineRtpStatsObject *obj=NULL;                    \
    MdmObjectId __oid = MDMOID_LINE_RTP_STATS;       \
    SET_L2OBJ_PARAM_UINT( i, p, n, v, f);            \
}

#define GET_CC_LINE_RTP_STATS_UINT( i, p, n, v, l )  \
{                                                    \
    LineRtpStatsObject *obj=NULL;                    \
    MdmObjectId __oid = MDMOID_LINE_RTP_STATS;       \
    GET_L2OBJ_PARAM_UINT( i, p, n, v, l);            \
}

#define SET_CC_LINE_STATS_INCALL_UINT( i, p, n, v, f )\
{                                                    \
    LineIncomingCallsObject *obj=NULL;               \
    MdmObjectId __oid = MDMOID_LINE_INCOMING_CALLS;  \
    SET_L2OBJ_PARAM_UINT( i, p, n, v, f);            \
}

#define SET_CC_LINE_STATS_OUTCALL_UINT( i, p, n, v, f )\
{                                                    \
    LineOutgoingCallsObject *obj=NULL;               \
    MdmObjectId __oid = MDMOID_LINE_OUTGOING_CALLS;  \
    SET_L2OBJ_PARAM_UINT( i, p, n, v, f);            \
}

#define GET_CC_LINE_STATS_INCALL_UINT( i, p, n, v, l )\
{                                                    \
    LineIncomingCallsObject *obj=NULL;               \
    MdmObjectId __oid = MDMOID_LINE_INCOMING_CALLS;  \
    GET_L2OBJ_PARAM_UINT( i, p, n, v, l);            \
}

#define GET_CC_LINE_STATS_OUTCALL_UINT( i, p, n, v, l )\
{                                                    \
    LineOutgoingCallsObject *obj=NULL;               \
    MdmObjectId __oid = MDMOID_LINE_OUTGOING_CALLS;  \
    GET_L2OBJ_PARAM_UINT( i, p, n, v, l);            \
}

/*****************************************************************
**  FUNCTION:       dalVoice_AddCallCtrlLine
**
**  PURPOSE:        Adds a call control line object to the given voice service instance
**
**  INPUT PARMS:    op[0] - voice service instance
**
**  OUTPUT PARMS:   inst - instance number of a created object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_AddCallCtrlLine( DAL_VOICE_PARMS *parms, int *inst )
{
    return addL2Object( MDMOID_CALL_CONTROL_LINE, parms->op[0], inst );
}

/*****************************************************************
**  FUNCTION:       dalVoice_AddCallCtrlExt
**
**  PURPOSE:        Adds a call control extension object to the given voice service instance
**
**  INPUT PARMS:    op[0] - voice service instance
**
**  OUTPUT PARMS:   inst - instance number of a created object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_AddCallCtrlExt( DAL_VOICE_PARMS *parms, int *inst )
{
    return addL2Object( MDMOID_CALL_CONTROL_EXTENSION, parms->op[0], inst );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtFxsList
**
**  PURPOSE:       Returns list of extensions which associate with FXS
**
**  INPUT PARMS:    op[0] - voice service instance
**
**  OUTPUT PARMS:   value - true/false
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlExtFxsList( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   int      i, numExt, type, extInst;
   UBOOL8   enabled = FALSE;
   DAL_VOICE_PARMS   localparms;
   char tmp[32];

   if( parms == NULL || value == NULL || length == 0 )
      return CMSRET_INVALID_PARAM_VALUE;

   memset(value, 0, length);

   /* get total number of extension */
   ret = dalVoice_GetNumOfExtension(parms, &numExt );
   if( ret != CMSRET_SUCCESS || numExt <= 0 )
      return CMSRET_INVALID_PARAM_VALUE;


   localparms.op[0] = parms->op[0];
   for( i = 0; i < numExt; i++)
   {
      /* map extension number to instance */
      ret = dalVoice_mapExtNumToExtInst( parms->op[0], i, &extInst );
      if( ret != CMSRET_SUCCESS || extInst <= 0 )
      {
         cmsLog_debug("%s() invalid mapping from number (%d)\n", __FUNCTION__, i );
         continue;
      }

      localparms.op[1] = extInst;
      memset(tmp, 0, sizeof(tmp));
      /* get enable status */
      GET_CC_EXT_PARAM_BOOL( parms->op[0], extInst, enable, tmp, sizeof(tmp));
      if( ret != CMSRET_SUCCESS || !strlen(tmp) || convertStringToBool( tmp, &enabled ) != CMSRET_SUCCESS || !enabled )
      {
         cmsLog_debug("%s() invalid mapping instance (%d), enabled (%s) \n", __FUNCTION__, extInst, enabled );
         continue;
      }

      /* get extension associated device type, could be FXS or DECT
      ** localparms.op[2] contains device instance
      */
      ret = dalVoice_GetCallCtrlExtAssocType( &localparms, &type );
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_debug("%s() invalid extension mapping (%d), type (%d) \n", __FUNCTION__, localparms.op[2], type );
         continue;
      }

      switch( type ){
         case TERMTYPE_FXS:
         {
            memset(tmp, 0, sizeof(tmp));
            GET_FXS_LINE_PARAM_BOOL(localparms.op[0], localparms.op[2], enable, tmp, sizeof(tmp));
            if( ret == CMSRET_SUCCESS )
            {
               cmsLog_debug("%s() extension associated fxs instance (%d), enabled (%s) \n", __FUNCTION__, localparms.op[2], tmp );
               if( convertStringToBool( tmp, &enabled ) == CMSRET_SUCCESS && enabled )
               {
                  memset(tmp, 0, sizeof(tmp));
                  sprintf(tmp, "%u", i );
                  if(strlen(value))
                     strncat(value, ",", length);
                  strncat(value, tmp, length);
               }
            }
         }
         break;
         case TERMTYPE_DECT:
         {
            /* TODO: check device enable status */
         }
         break;
      }
   }

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtEnable
**
**  PURPOSE:        Returns the extension enable status
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - true/false
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlExtEnable( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CC_EXT_PARAM_BOOL( parms->op[0], parms->op[1], enable, value, length);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCallCtrlExtEnabled
**
**  PURPOSE:        set the extension enable status
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - true/false
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCallCtrlExtEnabled( DAL_VOICE_PARMS *parms, char* value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    CallControlExtensionObject *obj=NULL;
    MdmObjectId __oid = MDMOID_CALL_CONTROL_EXTENSION;

    SET_L2OBJ_PARAM_BOOL( parms->op[0], parms->op[1], enable, value);

    return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtStatus
**
**  PURPOSE:        Returns the extension status
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - Up/Disabled/Initializing ...
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlExtStatus( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], status, value, length);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCallCtrlExtStatus
**
**  PURPOSE:        Sets the extension status
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - Up/Disabled/Initializing ...
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCallCtrlExtStatus( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], status, value, ext_status_valid_string);

   return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtCallStatus
**
**  PURPOSE:        Returns the extension call status
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - Idle/Dialing/Connected ....
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlExtCallStatus( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], callStatus, value, length);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCallCtrlExtCallStatus
**
**  PURPOSE:        Sets the extension call status
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - Idle/Dialing/Connected ....
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCallCtrlExtCallStatus( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], callStatus, value, ext_call_status_valid_string);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtName
**
**  PURPOSE:        Returns the extension name
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - extension name string
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlExtName( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], name, value, length);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCallCtrlExtName
**
**  PURPOSE:        Sets the extension name
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - extension name string
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCallCtrlExtName( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;

   SET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], name, value, NULL);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtNumber
**
**  PURPOSE:        Returns the extension number
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - extension number
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlExtNumber( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], extensionNumber, value, length);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCallCtrlExtNumber
**
**  PURPOSE:        Sets the extension number
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - extension number
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_SetCallCtrlExtNumber(DAL_VOICE_PARMS *parms, char *extNumber )
{
    CmsRet ret = CMSRET_SUCCESS;
    SET_CC_EXT_PARAM_STR(parms->op[0], parms->op[1], extensionNumber, extNumber, NULL );
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtConfStatus
**
**  PURPOSE:        Returns the conferencing call status
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - conf call status "Disabled/Idle/InConferenceCall..."
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlExtConfStatus( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   GET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], conferenceCallingStatus, value, length);
   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCallCtrlExtConfStatus
**
**  PURPOSE:        Sets the conferencing call status
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - conf call status "Disabled/Idle/InConferenceCall..."
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCallCtrlExtConfStatus( DAL_VOICE_PARMS *parms, char* value )
{
   CmsRet   ret = CMSRET_SUCCESS;
   SET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], conferenceCallingStatus, value, ext_conf_status_valid_string);
   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtCWStatus
**
**  PURPOSE:        Returns call waiting status
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - call waiting status "Disabled/Idle/SecondaryRinging...."
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlExtCWStatus( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;

   GET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], callWaitingStatus, value, length);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtCallFeatureSet
**
**  PURPOSE:        Returns call feature set name
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - call feature set name
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlExtCallFeatureSet( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   char     fullPath[MAX_TR104_OBJ_SIZE];
   MdmPathDescriptor  pathDesc;
   int      idx = 0, inst = 0;

   memset( fullPath, 0, sizeof(fullPath));
   GET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], callingFeatures, fullPath, sizeof(fullPath));
   if(ret == CMSRET_SUCCESS && strlen( fullPath ) > 0 )
   {
      ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
      if ( CMSRET_SUCCESS == ret && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
      {
         memset( value, 0, length );
         inst = POP_INSTANCE_ID(&pathDesc.iidStack);
         mapL2ObjectInstToNum( pathDesc.oid, POP_INSTANCE_ID(&pathDesc.iidStack), inst, &idx );
         snprintf( value, length, "CallFeatureSet_%u" , idx);
      }
   }

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtNumberPlan
**
**  PURPOSE:        Returns numbering plan name
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - numbering plan name "numberPlan_xx" which
**                  xx is numbering plan index
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlExtNumberPlan( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   char     fullPath[MAX_TR104_OBJ_SIZE];
   MdmPathDescriptor  pathDesc;
   int      idx = 0, inst = 0;

   memset( fullPath, 0, sizeof(fullPath));
   GET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], numberingPlan, fullPath, sizeof(fullPath));
   if(ret == CMSRET_SUCCESS && strlen( fullPath ) > 0 )
   {
      ret = cmsMdm_fullPathToPathDescriptor(fullPath, &pathDesc);
      if ( CMSRET_SUCCESS == ret && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
      {
         memset( value, 0, length );
         inst = POP_INSTANCE_ID(&pathDesc.iidStack);
         mapL2ObjectInstToNum( pathDesc.oid, POP_INSTANCE_ID(&pathDesc.iidStack), inst, &idx );
         snprintf( value, length, "NumberPlan_%u" , idx);
      }
   }

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtProvider
**
**  PURPOSE:        Returns the extension provider
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - extension provide name: FXS/FXO/DECT..
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlExtProvider( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   char     providerFullPath[MAX_TR104_OBJ_SIZE];
   MdmPathDescriptor  pathDesc;
   int      idx = 0, inst = 0;

   memset( providerFullPath, 0, sizeof(providerFullPath));
   GET_CC_EXT_PARAM_STR( parms->op[0], parms->op[1], provider, providerFullPath, sizeof(providerFullPath));
   if(ret == CMSRET_SUCCESS && strlen( providerFullPath ) > 0 )
   {
      ret = cmsMdm_fullPathToPathDescriptor(providerFullPath, &pathDesc);
      if ( CMSRET_SUCCESS == ret && cmsMdm_isPathDescriptorExist((const MdmPathDescriptor *)&pathDesc))
      {
         memset( value, 0, length );
         inst = POP_INSTANCE_ID(&pathDesc.iidStack);
         mapL2ObjectInstToNum( pathDesc.oid, POP_INSTANCE_ID(&pathDesc.iidStack), inst, &idx );
         switch( pathDesc.oid ){
            case MDMOID_POTS_FXS:
               {
                  snprintf( value, length, "FXS_%u" , idx);
               }
               break;
            case MDMOID_DECT_PORTABLE:
               {
                  snprintf( value, length, "DECT_%u", idx );
               }
               break;
            default:
               {
                  snprintf( value, length, "Unknown" );
                  ret = CMSRET_INVALID_PARAM_VALUE;
               }
               break;
         }
      }
   }

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCallCtrlExtFxsAssoc
**
**  PURPOSE:        Associates an extension with FXS port
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**                  op[2] - FXS object instance to associate the extension to
**
**  OUTPUT PARMS:   None
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_SetCallCtrlExtFxsAssoc( DAL_VOICE_PARMS *parms )
{
    return setL2ToL2ObjAssoc( MDMOID_CALL_CONTROL_EXTENSION, MDMOID_POTS_FXS, parms->op[0], parms->op[1], parms->op[2]);
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtFxsAssoc
**
**  PURPOSE:        Get extension associated FXS instance
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**
**  OUTPUT PARMS:   op[2] - FXS object instance the extension associates to
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetCallCtrlExtFxsAssoc( DAL_VOICE_PARMS *parms )
{
    return getL2ToL2ObjAssoc( MDMOID_CALL_CONTROL_EXTENSION, MDMOID_POTS_FXS, parms->op[0], parms->op[1], &parms->op[2]);
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlExtAssocType
**
**  PURPOSE:        Get the POTS type of associated to an extension
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   op[2] - MDMOID_POTS_FXS, MDMOID_DECT_PORTABLE
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetCallCtrlExtAssocType( DAL_VOICE_PARMS *parms, int *type )
{
    CmsRet   ret = CMSRET_SUCCESS;
    MdmObjectId destOid = MDM_MAX_OID;

    ret = getL2ToL2ObjAssocType( MDMOID_CALL_CONTROL_EXTENSION, &destOid, parms->op[0], parms->op[1], &parms->op[2]);
    if( ret == CMSRET_SUCCESS ){
        switch( destOid ){
            case MDMOID_POTS_FXS:
                *type = (int) TERMTYPE_FXS;
                break;
            case MDMOID_DECT_PORTABLE:
                *type = (int) TERMTYPE_DECT;
                break;
        }
    }
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlLineAssocType
**
**  PURPOSE:        Get the POTS type of associated to an extension
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Line instance
**
**  OUTPUT PARMS:   op[2] - MDMOID_POTS_FXS, MDMOID_DECT_PORTABLE
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet  dalVoice_GetCallCtrlLineAssocType( DAL_VOICE_PARMS *parms, int *type )
{
    CmsRet   ret = CMSRET_SUCCESS;
    MdmObjectId destOid = MDM_MAX_OID;

    ret = getL2ToL2ObjAssocType( MDMOID_CALL_CONTROL_LINE, &destOid, parms->op[0], parms->op[1], &parms->op[2]);
    if( ret == CMSRET_SUCCESS ){
        switch( destOid ){
            case MDMOID_SIP_CLIENT:
                *type = (int) TERMTYPE_SIP;
                break;
            case MDMOID_POTS_FXO:
                *type = (int) TERMTYPE_FXO;
                break;
        }
    }
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlLineEnable
**
**  PURPOSE:       Returns the line enable status
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - true/false
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlLineEnable( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
    CmsRet   ret = CMSRET_SUCCESS;
    CallControlLineObject *obj=NULL;
    MdmObjectId __oid = MDMOID_CALL_CONTROL_LINE;

    GET_L2OBJ_PARAM_BOOL(parms->op[0], parms->op[1], enable, value, length);

    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCallCtrlLineEnabled
**
**  PURPOSE:       Returns the line enable status
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - Extension instance
**
**  OUTPUT PARMS:   value - true/false
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCallCtrlLineEnabled( DAL_VOICE_PARMS *parms, char* value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    CallControlLineObject *obj=NULL;
    MdmObjectId __oid = MDMOID_CALL_CONTROL_LINE;

    SET_L2OBJ_PARAM_BOOL(parms->op[0], parms->op[1], enable, value);
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_SetCallCtrlLineCallStatus
**
**  PURPOSE:        set call status of line
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - line instance
**                  value - idle, dialing, connected, delivered, alerting, disconnected
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_SetCallCtrlLineCallStatus( DAL_VOICE_PARMS *parms, char* value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    CallControlLineObject *obj=NULL;
    MdmObjectId __oid = MDMOID_CALL_CONTROL_LINE;

    SET_L2OBJ_PARAM_STR(parms->op[0], parms->op[1], callStatus, value, NULL);
    return ret;
}

CmsRet dalVoice_SetCcLineStatsRtpPacketSent( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], packetsSent, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsRtpPacketRecv( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], packetsReceived, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsRtpPacketLost( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], packetsLost, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsRtpBytesSent( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], bytesSent, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsRtpBytesRecv( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], bytesReceived, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsInCallRecv( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsReceived, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsInCallConn( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsConnected, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsInCallFailed( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsFailed, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsInCallDrop( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsDropped, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsOutCallAttempt( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsAttempted, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsOutCallConn( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsConnected, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsOutCallFailed( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsFailed, value, 0xFFFFFFFF );
    return ret;
}

CmsRet dalVoice_SetCcLineStatsOutCallDrop( DAL_VOICE_PARMS *parms, UINT32 value )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsDropped, value, 0xFFFFFFFF );
    return ret;
}



CmsRet dalVoice_ResetCcLineStats( DAL_VOICE_PARMS *parms )
{
    CmsRet   ret = CMSRET_SUCCESS;
    SET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], packetsSent, 0, 0xFFFFFFFF );
    SET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], packetsReceived, 0, 0xFFFFFFFF );
    SET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], packetsLost, 0, 0xFFFFFFFF );
    SET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], bytesSent, 0, 0xFFFFFFFF );
    SET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], bytesReceived, 0, 0xFFFFFFFF );
    SET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsReceived, 0, 0xFFFFFFFF );
    SET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsConnected, 0, 0xFFFFFFFF );
    SET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsFailed, 0, 0xFFFFFFFF );
    SET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsDropped, 0, 0xFFFFFFFF );
    SET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsAttempted, 0, 0xFFFFFFFF );
    SET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsConnected, 0, 0xFFFFFFFF );
    SET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsFailed, 0, 0xFFFFFFFF );
    SET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsDropped, 0, 0xFFFFFFFF );
    return ret;
}


CmsRet dalVoice_GetCcLineStatsRtpPacketSentString( DAL_VOICE_PARMS *parms, char* value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], packetsSent, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsRtpPacketRecvString( DAL_VOICE_PARMS *parms, char* value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], packetsReceived, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsRtpPacketLostString( DAL_VOICE_PARMS *parms, char* value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], packetsLost, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsRtpBytesSentString( DAL_VOICE_PARMS *parms, char* value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], bytesSent, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsRtpBytesRecvString( DAL_VOICE_PARMS *parms, char* value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_RTP_STATS_UINT( parms->op[0], parms->op[1], bytesReceived, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsInCallRecvString( DAL_VOICE_PARMS *parms, char *value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsReceived, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsInCallConnString( DAL_VOICE_PARMS *parms, char *value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsConnected, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsInCallFailedString( DAL_VOICE_PARMS *parms, char *value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsFailed, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsInCallDropString( DAL_VOICE_PARMS *parms, char *value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_STATS_INCALL_UINT( parms->op[0], parms->op[1], callsDropped, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsOutCallAttemptString( DAL_VOICE_PARMS *parms, char *value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsAttempted, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsOutCallConnString( DAL_VOICE_PARMS *parms, char *value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsConnected, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsOutCallFailedString( DAL_VOICE_PARMS *parms, char *value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsFailed, value, len );
    return ret;
}

CmsRet dalVoice_GetCcLineStatsOutCallDropString( DAL_VOICE_PARMS *parms, char *value, unsigned int len )
{
    CmsRet   ret = CMSRET_SUCCESS;
    GET_CC_LINE_STATS_OUTCALL_UINT( parms->op[0], parms->op[1], callsDropped, value, len );
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetCallCtrlLineFxoList
**
**  PURPOSE:       Returns list of lines which associate with FXO
**
**  INPUT PARMS:    op[0] - voice service instance
**
**  OUTPUT PARMS:   value - true/false
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetCallCtrlLineFxoList( DAL_VOICE_PARMS *parms, char* value, unsigned int length )
{
   CmsRet   ret = CMSRET_SUCCESS;
   int      i, numLine, type, lineInst;
   UBOOL8   enabled = FALSE;
   DAL_VOICE_PARMS   localparms;
   char tmp[32];

   if( parms == NULL || value == NULL || length == 0 )
      return CMSRET_INVALID_PARAM_VALUE;

   memset(value, 0, length);

   /* get total number of extension */
   ret = dalVoice_GetNumOfLine(parms, &numLine );
   if( ret != CMSRET_SUCCESS || numLine <= 0 )
      return CMSRET_INVALID_PARAM_VALUE;


   localparms.op[0] = parms->op[0];
   for( i = 0; i < numLine; i++)
   {
      /* map extension number to instance */
      ret = dalVoice_mapAcntNumToLineInst( parms->op[0], i, &lineInst );
      if( ret != CMSRET_SUCCESS || lineInst <= 0 )
      {
         cmsLog_debug("%s() invalid mapping from number (%d)\n", __FUNCTION__, i );
         continue;
      }

      localparms.op[1] = lineInst;
      memset(tmp, 0, sizeof(tmp));
      /* get enable status */
      ret = dalVoice_GetCallCtrlLineEnable( &localparms, tmp, sizeof(tmp));
      if( ret != CMSRET_SUCCESS || !strlen(tmp) || convertStringToBool( tmp, &enabled ) != CMSRET_SUCCESS || !enabled )
      {
         cmsLog_debug("%s() invalid mapping instance (%d), enabled (%s) \n", __FUNCTION__, lineInst, enabled );
         continue;
      }

      /* get line associated device type, could be SIP or FXO
      ** localparms.op[2] contains device instance
      */
      ret = dalVoice_GetCallCtrlLineAssocType( &localparms, &type );
      if( ret != CMSRET_SUCCESS )
      {
         cmsLog_debug("%s() invalid extension mapping (%d), type (%d) \n", __FUNCTION__, localparms.op[2], type );
         continue;
      }

      if( type == TERMTYPE_FXO )
      {
         memset(tmp, 0, sizeof(tmp));
         GET_FXO_LINE_PARAM_BOOL(localparms.op[0], localparms.op[2], enable, tmp, sizeof(tmp));
         if( ret == CMSRET_SUCCESS )
         {
            cmsLog_debug("%s() line associated fxo instance (%d), enabled (%s) \n", __FUNCTION__, localparms.op[2], tmp );
            if( convertStringToBool( tmp, &enabled ) == CMSRET_SUCCESS && enabled )
            {
               memset(tmp, 0, sizeof(tmp));
               sprintf(tmp, "%u", i );
               if(strlen(value))
                  strncat(value, ",", length);
               strncat(value, tmp, length);
            }
         }
      }
   }

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPstnRouteRule
**
**  PURPOSE:
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
    CmsRet   ret  = CMSRET_SUCCESS;
    GET_CALLCTL_PARAM_STR( parms->op[0], X_BROADCOM_COM_PstnRoutingMode, mode, length);
    return  ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetPstnRouteData
**
**  PURPOSE:
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
    CmsRet   ret  = CMSRET_SUCCESS;
    GET_CALLCTL_PARAM_STR( parms->op[0], X_BROADCOM_COM_PstnRoutingDest, dest, length);
    return  ret;
}



/*****************************************************************
**  FUNCTION:       dalVoice_GetNumVoiceSrv
**
**  PUROPOSE:       Returns total of voice service provider instances configured
**                  ( i.e corresponds to total no. of Voice Network Instances)
**
**  INPUT PARMS:    voice service instance - op[0];
**
**  OUTPUT PARMS:   Number of service providers configured
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumVoiceProfiles( DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    Dev2ServicesObject   *obj=NULL;

    ret = cmsObj_get( MDMOID_DEV2_SERVICES, &iidStack, OGF_NO_VALUE_UPDATE, (void **)&obj );
    if( CMSRET_SUCCESS == ret ){
        snprintf( value, length, "%u", obj->voiceServiceNumberOfEntries);
        cmsObj_free((void **)&obj);
    }
    else{
        cmsLog_error( "failed to retrieve Device object\n");
    }

    return ret;

}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumVoiceSrv
**
**  PURPOSE:        Returns total of voice service provider instances configured
**                  ( i.e corresponds to total no. of Voice Network Instances)
**
**  INPUT PARMS:    voice service instance - op[0];
**
**  OUTPUT PARMS:   Number of service providers configured
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumSrvProv( int * numSp )
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    Dev2ServicesObject   *obj=NULL;
    int   i;

    /* can't use voiceServiceNumberOfEntries counter because 
     * TR98 data model doesn't support it
     */
    for( i = 0; ; i++ )
    {
        ret = cmsObj_getNextFlags(MDMOID_VOICE, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &obj);
        if( ret == CMSRET_SUCCESS )
        {
            cmsLog_debug("%s successfully get voice service obj\n", __FUNCTION__);
            cmsObj_free((void **) &obj);
        }
        else
            break;
    }

    *numSp = i;

    if( i <=0 )
    {
        cmsLog_error( "failed to retrieve Device object\n");
        return CMSRET_RESOURCE_EXCEEDED;
    }

    return CMSRET_SUCCESS;
}


/******************************************************************************
***                Voice POTS Interface                                     ***
*******************************************************************************/
#define GET_POTS_PARAM_UINT(i, n, v)                            \
{                                                               \
    VoiceServicePotsObject *obj=NULL;                           \
    MdmObjectId           __oid = MDMOID_VOICE_SERVICE_POTS;    \
    GET_L1OBJ_PARAM_UINT( i, n, v );                            \
}

#define GET_POTS_PARAM_UINT_AS_STR(i, n, v, l)                  \
{                                                               \
    VoiceServicePotsObject *obj=NULL;                           \
    MdmObjectId           __oid = MDMOID_VOICE_SERVICE_POTS;    \
    UINT32                __value = 0;                          \
    memset(v, 0, l);                                            \
    GET_L1OBJ_PARAM_UINT( i, n, &__value );                     \
    if( CMSRET_SUCCESS == ret ){                                \
        snprintf(v, l, "%d", __value );                         \
    }                                                           \
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetNumPhysFxsEndpt
**
**  PURPOSE:        Returns total number of physical fxs endpoints in system
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
   CmsRet   ret = CMSRET_SUCCESS;
   int      spInst = 0;

   /* pick the first available voice service instance as default*/
   ret = dalVoice_mapSpNumToSvcInst( 0, &spInst );
   if( ret == CMSRET_SUCCESS && spInst > 0 )
   {
      GET_POTS_PARAM_UINT( spInst, FXSNumberOfEntries, numPhysFxs);
   }
   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumPhysFxsEndptStr
**
**  PURPOSE:        Returns total number of physical fxs endpoints in system
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
   CmsRet   ret = CMSRET_SUCCESS;
   UINT32   FxsNum = 0;

   memset( value, 0, length );
   GET_POTS_PARAM_UINT(parms->op[0], FXSNumberOfEntries, &FxsNum);
   if( ret == CMSRET_SUCCESS )
   {
      snprintf( value, length, "%u", FxsNum);
   }


   return ( ret );
}


/*****************************************************************
**  FUNCTION:       dalVoice_GetNumPhysFxoEndpt
**
**  PURPOSE:        Returns total number of physical fxo endpoints in system
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
   CmsRet   ret = CMSRET_SUCCESS;

   GET_POTS_PARAM_UINT( 1, FXONumberOfEntries, numPhysFxo);

   return ( ret );
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumPhysFxoEndptStr
**
**  PURPOSE:        Returns total number of physical fxs endpoints in system
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
   CmsRet   ret = CMSRET_SUCCESS;
   UINT32   FxoNum = 0;

   memset( value, 0, length );
   GET_POTS_PARAM_UINT(parms->op[0], FXONumberOfEntries, &FxoNum);
   if( ret == CMSRET_SUCCESS )
   {
      snprintf( value, length, "%u", FxoNum);
   }

   return ( ret );
}


/*<END>==================================== DAL Get Functions ======================================<END>*/

/*<START>===================================== Set Helper Functions ======================================<START>*/

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
   CmsRet ret = CMSRET_SUCCESS;
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
   CmsRet ret = CMSRET_SUCCESS;

   return ( ret );
}


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
   CmsRet ret = CMSRET_SUCCESS;
   return ret;
}

/*<END>===================================== Set Helper Functions ========================================<END>*/

/*<START>================================= Get Helper Functions ==========================================<START>*/

/*****************************************************************
**  FUNCTION:       dumpCmdParam
**
**  PURPOSE:        Outputs command parameters. Used for debugging
**
**  INPUT PARMS:    none
**
**  OUTPUT PARMS:
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
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
   return;
}

/*****************************************************************
**  FUNCTION:       dalVoice_mapPotsFxsNumToInst
**
**  PURPOSE:        Maps FXS object number to object instance number
**
**  INPUT PARMS:    parms->op[0] - voice service instance number
**                  parms->op[1] - FXS object number
**
**  OUTPUT PARMS:   fxsInst - FXS object instance
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_mapPotsFxsNumToInst(DAL_VOICE_PARMS *parms, int *fxsInst)
{
    return mapL2ObjectNumToInst( MDMOID_POTS_FXS, parms->op[0], parms->op[1], fxsInst);
}

CmsRet dalVoice_mapPotsFxsInstToNum(DAL_VOICE_PARMS *parms, int *num)
{
    return mapL2ObjectInstToNum( MDMOID_POTS_FXS, parms->op[0], parms->op[1], num);
}

CmsRet dalVoice_mapPotsFxoNumToInst(DAL_VOICE_PARMS *parms, int *fxoInst)
{
    return mapL2ObjectNumToInst( MDMOID_POTS_FXO, parms->op[0], parms->op[1], fxoInst);
}

/***************************************************************************
* Function Name: dalVoice_GetNumVoiceCallLog
* Description  : Get total number of Voice call log entries
*
* Parameters   : parms->op[0] = voice service instance, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetNumVoiceCallLog( DAL_VOICE_PARMS *parms, int *value )
{
    CmsRet ret = CMSRET_SUCCESS;
#ifdef DMP_VOIPPROFILE_1
    GET_VOICE_SVC_PARAM_UINT( parms->op[0], callLogNumberOfEntries, value);
#endif /* DMP_VOIPPROFILE_1 */
    return ret;
}


/***************************************************************************
* Function Name: dalVoice_GetNumVoipProfile
* Description  : Get total number of VoIP profile in the system
*
* Parameters   : parms->op[0] = voice service instance, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetNumVoipProfile( DAL_VOICE_PARMS *parms, int *value )
{
    CmsRet ret = CMSRET_SUCCESS;
#ifdef DMP_VOIPPROFILE_1
    GET_VOICE_SVC_PARAM_UINT( parms->op[0], voIPProfileNumberOfEntries, value);
#endif /* DMP_VOIPPROFILE_1 */
    return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetVoipProfileList
*
* PURPOSE:     Get list of available VOIP profiles
*
* PARAMETERS:  parms - voice service parameters to use for queries
*              profList - placeholder for the VOIP profile list
*
* RETURNS:     CMSRET_SUCCESS when successful
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetVoipProfileList( DAL_VOICE_PARMS *parms, char* profList, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;
    DAL_VOICE_PARMS parmsList;
    int numProfiles, vpInst;

    char *objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );

    if (parms == NULL) {
        return CMSRET_INVALID_ARGUMENTS;
    }

    if (dalVoice_GetNumVoipProfile(parms, &numProfiles) == CMSRET_SUCCESS)
    {
       for(unsigned int i = 0; i < numProfiles; i++)
       {
          parmsList.op[0] = parms->op[0];
          parmsList.op[1] = i;
          if( dalVoice_mapVoipProfNumToInst(&parmsList, &vpInst) == CMSRET_SUCCESS)
          {
             parmsList.op[1] = vpInst;
             if ( dalVoice_GetVoipProfileEnable( &parmsList, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
             {
                if(1) /* Phase2 TODO: add later when profiles are enabled !strcmp(objValue, "Yes")) */
                {
                    if (dalVoice_GetVoipProfileName( &parmsList, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
                    {
                        strcat(profList, objValue);
                        if (i != (numProfiles-1))
                        {
                           strcat(profList, ",");
                        }
                    }
                }
             }
          }
       }
    }

    cmsMem_free( objValue );
    return ret;
}

/*****************************************************************
**  FUNCTION:       dalVoice_GetNumOfCodecs
**
**  PURPOSE:        gets the number of codecs for a given voice service
**
**  INPUT PARMS:    svcIdx - voice service instance number
**
**  OUTPUT PARMS:   numCodec - number of codecs
**
**  RETURNS:        CMSRET_SUCCESS - Read Success
**                  other failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_GetNumOfCodecs(int svcIdx, int *numCodec)
{
    CmsRet ret = CMSRET_SUCCESS;

    VoiceCapObject *obj=NULL;
    ret = getObject( MDMOID_VOICE_CAP, svcIdx, 0, 0, 0, OGF_NO_VALUE_UPDATE, NULL, (void **)&obj );
    if( CMSRET_SUCCESS == ret )
    {
        *numCodec = obj->codecNumberOfEntries;
        cmsObj_free((void **)&obj);
    }
    else
    {
        *numCodec = 0;
        cmsLog_error( "failed to retrieve Capabilities object\n");
    }
    return ret;
}

/****************************************************************************
* FUNCTION:    dalVoice_GetCodecProfileList
*
* PURPOSE:     Get list of available codec profiles
*
* PARAMETERS:  parms - voice service parameters to use for queries
*              profList - placeholder for the codec profile list
*
* RETURNS:     CMSRET_SUCCESS when successful
*
* NOTE:
*
****************************************************************************/
CmsRet dalVoice_GetCodecProfileList( DAL_VOICE_PARMS *parms, char* profList, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;
    DAL_VOICE_PARMS parmsList;
    int numProfiles, vpInst;

    char *objValue = cmsMem_alloc( MAX_TR104_OBJ_SIZE, ALLOC_ZEROIZE );

    if (parms == NULL) {
        return CMSRET_INVALID_ARGUMENTS;
    }

    if (dalVoice_GetNumCodecProfile(parms, &numProfiles) == CMSRET_SUCCESS)
    {
       for(unsigned int i = 0; i < numProfiles; i++)
       {
          parmsList.op[0] = parms->op[0];
          parmsList.op[1] = i;
          if( dalVoice_mapCodecProfNumToInst(&parmsList, &vpInst) == CMSRET_SUCCESS)
          {
             parmsList.op[1] = vpInst;
             if (dalVoice_GetCodecProfileName( &parmsList, objValue, MAX_TR104_OBJ_SIZE ) == CMSRET_SUCCESS )
             {
                strcat(profList, objValue);
                if (i != (numProfiles-1))
                {
                   strcat(profList, ",");
                }
             }
          }
       }
    }

    cmsMem_free( objValue );
    return ret;
}

/***************************************************************************
* Function Name: dalVoice_GetNumCodecProfile
* Description  : Get total number of VoIP profile in the system
*
* Parameters   : parms->op[0] = voice service instance, value = value to be set
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_GetNumCodecProfile( DAL_VOICE_PARMS *parms, int *value )
{
    CmsRet ret;
    GET_VOICE_SVC_PARAM_UINT( parms->op[0], codecProfileNumberOfEntries, value);
    return ret;
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
   return CMSRET_SUCCESS;
}


/***************************************************************************
* Function Name: dalVoice_mapExtNumToExtInst
* Description  : This returns the Callctrl Extension instance number corresponding
*                to a Extension index.
*
* Parameters   : vpInst (IN)    - VoiceProfile instance number
*                acntNum (IN)   - Account index
*                lineInst (OUT) - pointer to line instance number
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
CmsRet dalVoice_mapExtNumToExtInst ( int vpInst, int num, int *extInst )
{
    return mapL2ObjectNumToInst( MDMOID_CALL_CONTROL_EXTENSION, vpInst, num, extInst);
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
    return mapL2ObjectNumToInst( MDMOID_CALL_CONTROL_LINE, vpInst, acntNum, lineInst);
}

/***************************************************************************
* Function Name: dalVoice_mapAcntNumToClientInst
* Description  : This returns the Sip Client instance number corresponding
*                to a account index.
*
* Parameters   : vpInst (IN)    - VoiceProfile instance number
*                acntNum (IN)   - Account index
*                lineInst (OUT) - pointer to line instance number
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
CmsRet dalVoice_mapAcntNumToClientInst ( DAL_VOICE_PARMS *parms, int *Inst )
{
    return mapL2ObjectNumToInst( MDMOID_SIP_CLIENT, parms->op[0], parms->op[1], Inst);
}

/***************************************************************************
* Function Name: dalVoice_mapSipClientInstToNum
* Description  : This returns the call manager account number corresponding
*                to a certain Voice Profile instance number and line instance
*                number.
*
* Parameters   : vpInst (IN)    - voice profile instance
*                lineInst (IN)  - line instance
*                cmAcnt (OUT)   - pointer to call manager account number
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_mapSipClientInstToNum ( int vpInst, int clientInst, int *num )
{
   return mapL2ObjectInstToNum( MDMOID_SIP_CLIENT, vpInst, clientInst, num );
}

/***************************************************************************
* Function Name: dalVoice_mapNetworkNumToInst
* Description  : This returns the Line instance number corresponding
*                to a Voice Profile number and account index.
*
* Parameters   : vpInst (IN)    - VoiceProfile instance number
*                acntNum (IN)   - Account index
*                lineInst (OUT) - pointer to line instance number
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
CmsRet dalVoice_mapNetworkNumToInst ( DAL_VOICE_PARMS *parms, int *Inst )
{
    return mapL2ObjectNumToInst( MDMOID_SIP_NETWORK, parms->op[0], parms->op[1], Inst);
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
* Function Name: dalVoice_mapSpNumToSvcInst
* Description  : This returns the Voice Profile instance number corresponding
*                to a certain service provider index.
*
* Parameters   : spNum (IN)     - service provider index
*                vpInst (OUT)   - pointer to VoiceProfile instance number
* Returns      : CMSRET_SUCCESS when successful.
****************************************************************************/
CmsRet dalVoice_mapSpNumToSvcInst ( int spNum, int * vpInst )
{
   return mapSpNumToVpInst( spNum, vpInst );
}

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
   CmsRet  ret = CMSRET_SUCCESS;

   return  ( ret );
}

/***************************************************************************
* Function Name: dalVoice_mapCountryCode3To2
* Description  : Maps Alpha-3 locale to Alpha-2. Also checks if locale is valid
*
* Parameters   : country (INOUT) - locale(Alpha3), on success exec contains Alpha2 locale
*                found (OUT)   - true indicates locale is supported
* Returns      : CMSRET_SUCCESS when successfule.
****************************************************************************/
CmsRet dalVoice_mapCountryCode3To2 ( char *alpha3, char *alpha2, unsigned int length )
{
   return (rutVoice_mapAlpha3toAlpha2Locale ( alpha3, alpha2, length ));
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
   return rutVoice_mapSpNumToSvcInst( spNum, vpInst );
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

/*****************************************************************
**  FUNCTION:       dalVoice_AddSipClient
**
**  PURPOSE:        Adds a SIP client object
**
**  INPUT PARMS:    op[0] - voice service instance
**
**  OUTPUT PARMS:   inst - instance of the added object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_AddSipClient( DAL_VOICE_PARMS *parms, int *inst )
{
    return addL2Object( MDMOID_SIP_CLIENT, parms->op[0], inst );
}

/*****************************************************************
**  FUNCTION:       dalVoice_DeleteSipClient
**
**  PURPOSE:        Deletes a SIP client object
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - instance of the SIP client object to delete
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_DeleteSipClient( DAL_VOICE_PARMS *parms )
{
    return delL2Object( MDMOID_SIP_CLIENT, parms->op[0], parms->op[1] );
}

/*****************************************************************
**  FUNCTION:       dalVoice_DeleteCallCtrlLine
**
**  PURPOSE:        Deletes a call control line object
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - instance of the object to delete
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_DeleteCallCtrlLine( DAL_VOICE_PARMS *parms )
{
    return delL2Object( MDMOID_CALL_CONTROL_LINE, parms->op[0], parms->op[1] );
}

/*****************************************************************
**  FUNCTION:       dalVoice_DeleteCallCtrlExt
**
**  PURPOSE:        Deletes a call control ext object
**
**  INPUT PARMS:    op[0] - voice service instance
**                  op[1] - instance of the object to delete
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
CmsRet dalVoice_DeleteCallCtrlExt( DAL_VOICE_PARMS *parms )
{
    return delL2Object( MDMOID_CALL_CONTROL_EXTENSION, parms->op[0], parms->op[1] );
}

/*****************************************************************
**  FUNCTION:       delL2Object
**
**  PURPOSE:        Deletes a level 2 object in MDM
**
**  INPUT PARMS:    oid - object ID to delete
**                  L1_inst - level 1 object instance
**                  L2_inst - level 2 object instance to be deleted
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
static CmsRet delL2Object( MdmObjectId oid, int L1_inst, int L2_inst )
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    if( L1_inst <= 0 || L2_inst <= 0 )
    {
        return CMSRET_INVALID_PARAM_VALUE;
    }

    cmsLog_debug("%s(), delete oid (%u) instance (%u)\n", __FUNCTION__, oid, L2_inst);

    PUSH_INSTANCE_ID( &iidStack, L1_inst );
    PUSH_INSTANCE_ID( &iidStack, L2_inst );

    return cmsObj_deleteInstance(oid, &iidStack);
}

/*****************************************************************
**  FUNCTION:       addL2Object
**
**  PURPOSE:        Adds a level 2 object in MDM
**
**  INPUT PARMS:    oid - object ID to add
**                  L1_inst - level 1 object instance
**
**  OUTPUT PARMS:   inst - instance of the added object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
static CmsRet addL2Object( MdmObjectId oid, int L1_inst, int *inst )
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    if( L1_inst <= 0 )
    {
        return CMSRET_INVALID_PARAM_VALUE;
    }

    PUSH_INSTANCE_ID( &iidStack, L1_inst );

    ret = cmsObj_addInstance(oid, &iidStack);
    if( ret == CMSRET_SUCCESS && inst != NULL)
    {
        *inst = PEEK_INSTANCE_ID( &iidStack );
        cmsLog_debug("%s(), add oid (%u) instance (%u)\n", __FUNCTION__, oid, *inst);
    }

    return ret;
}

/*****************************************************************
**  FUNCTION:       mapL2ObjectNumToInst
**
**  PURPOSE:        Maps a level 2 object number to instance
**
**  INPUT PARMS:    oid - object ID to add
**                  L1_inst - level 1 object instance
**                  Num - object number to be mapped
**
**  OUTPUT PARMS:   L2_Inst - instance of the mapped object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
static CmsRet mapL2ObjectNumToInst ( MdmObjectId oid, int L1_Inst, int Num, int *L2_Inst )
{
    int  i;
    CmsRet ret  = CMSRET_SUCCESS;
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack  searchIidStack = EMPTY_INSTANCE_ID_STACK;
    void  *obj = NULL;

    PUSH_INSTANCE_ID(&iidStack, L1_Inst);
    for( i = Num ; i >= 0; i-- )
    {
        ret = cmsObj_getNextInSubTreeFlags(oid, &iidStack, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **)&obj);
        if( ret == CMSRET_SUCCESS ){
            cmsObj_free((void **)&obj );
        }
        else{
            break;
        }
    }

    if( ret == CMSRET_SUCCESS )
    {
        *L2_Inst = PEEK_INSTANCE_ID(&searchIidStack);
        cmsLog_debug("%s() map L2 object (%d) number (%d) to instance (%d)", __FUNCTION__, oid, Num, *L2_Inst );
    }
    else
    {
        cmsLog_error("%s() could not find L2 object (%d) instance for number (%d)", __FUNCTION__, oid, Num);
    }
    return ret;
}

/*****************************************************************
**  FUNCTION:       mapL2ObjectInstToNum
**
**  PURPOSE:        Maps a level 2 object instance to number
**
**  INPUT PARMS:    oid - object ID to add
**                  L1_inst - level 1 object instance
**                  L2_Inst - instance number to be mapped
**
**  OUTPUT PARMS:   Num - number of the mapped object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
static CmsRet mapL2ObjectInstToNum ( MdmObjectId oid, int L1_Inst, int L2_Inst, int *Num )
{
    int  i = -1;
    CmsRet ret  = CMSRET_SUCCESS;
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack  searchIidStack = EMPTY_INSTANCE_ID_STACK;
    void  *obj = NULL;

    PUSH_INSTANCE_ID(&iidStack, L1_Inst);
    ret = cmsObj_getNextInSubTreeFlags(oid, &iidStack, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **)&obj);
    while( ret == CMSRET_SUCCESS ){
        cmsObj_free((void **)&obj );
        i++;
        if( L2_Inst == (PEEK_INSTANCE_ID(&searchIidStack))){
            break;
        }
        ret = cmsObj_getNextInSubTreeFlags(oid, &iidStack, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **)&obj);
    }

    if( ret == CMSRET_SUCCESS )
    {
        *Num = i;
        cmsLog_debug("%s() map L2 object (%d) instance (%d) to number (%d)", __FUNCTION__, oid, L2_Inst, *Num );
    }
    else
    {
        cmsLog_error("%s() could not find L2 object (%d) instance (%d)", __FUNCTION__, oid, L2_Inst);
    }
    return ret;
}

/*****************************************************************
**  FUNCTION:       delL3Object
**
**  PURPOSE:        Deletes a level 3 object in MDM
**
**  INPUT PARMS:    oid - object ID to delete
**                  L1_inst - level 1 object instance
**                  L2_inst - level 2 object instance to be deleted
**                  L3_inst - level 3 object instance to be deleted
**
**  OUTPUT PARMS:   none
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
static CmsRet delL3Object( MdmObjectId oid, int L1_inst, int L2_inst, int L3_inst )
{
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    if( L1_inst <= 0 || L2_inst <= 0 || L3_inst <= 0)
    {
        return CMSRET_INVALID_PARAM_VALUE;
    }

    cmsLog_debug("%s(), delete oid (%u) instance (%u)\n", __FUNCTION__, oid, L3_inst);

    PUSH_INSTANCE_ID( &iidStack, L1_inst );
    PUSH_INSTANCE_ID( &iidStack, L2_inst );
    PUSH_INSTANCE_ID( &iidStack, L3_inst );

    return cmsObj_deleteInstance(oid, &iidStack);
}


/*****************************************************************
**  FUNCTION:       addL3Object
**
**  PURPOSE:        Adds a level 3 object in MDM
**
**  INPUT PARMS:    oid - object ID to add
**                  L1_inst - level 1 object instance
**                  L2_inst - level 2 object instance
**
**  OUTPUT PARMS:   inst - instance of the added object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
static CmsRet addL3Object( MdmObjectId oid, int L1_inst, int L2_inst, int *inst )
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    if( L1_inst <= 0 || L2_inst <= 0 )
    {
        return CMSRET_INVALID_PARAM_VALUE;
    }

    PUSH_INSTANCE_ID( &iidStack, L1_inst );
    PUSH_INSTANCE_ID( &iidStack, L2_inst );

    ret = cmsObj_addInstance(oid, &iidStack);
    if( ret == CMSRET_SUCCESS && inst != NULL)
    {
        *inst = PEEK_INSTANCE_ID( &iidStack );
        cmsLog_debug("%s(), add oid (%u) instance (%u)\n", __FUNCTION__, oid, *inst);
    }

    return ret;
}

/*****************************************************************
**  FUNCTION:       mapL3ObjectNumToInst
**
**  PURPOSE:        Maps a level 3 object number to instance
**
**  INPUT PARMS:    oid - object ID to add
**                  L1_inst - level 1 object instance
**                  L2_inst - level 2 object instance
**                  Num - object number to be mapped
**
**  OUTPUT PARMS:   L3_Inst - instance of the mapped object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
static CmsRet mapL3ObjectNumToInst ( MdmObjectId oid, int L1_Inst, int L2_Inst, int Num, int *L3_Inst )
{
    int  i;
    CmsRet ret  = CMSRET_SUCCESS;
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack  searchIidStack = EMPTY_INSTANCE_ID_STACK;
    void  *obj = NULL;

    PUSH_INSTANCE_ID(&iidStack, L1_Inst);
    PUSH_INSTANCE_ID(&iidStack, L2_Inst);
    cmsLog_debug("%s() map L3 object (%d) number (%d) to on L1(%d) L2(%d)", __FUNCTION__, oid, Num, L1_Inst, L2_Inst );
    for( i = Num ; i >= 0; i-- )
    {
        ret = cmsObj_getNextInSubTreeFlags(oid, &iidStack, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **)&obj);
        if( ret == CMSRET_SUCCESS ){
            cmsObj_free((void **)&obj );
        }
        else{
            break;
        }
    }

    if( ret == CMSRET_SUCCESS )
    {
        *L3_Inst = PEEK_INSTANCE_ID(&searchIidStack);
        cmsLog_debug("%s() map L3 object (%d) number (%d) to instance (%d)", __FUNCTION__, oid, Num, *L3_Inst );
    }
    else
    {
        cmsLog_error("%s() could not find L3 object (%d) instance for number (%d)", __FUNCTION__, oid, Num);
    }
    return ret;
}

#if 0
/*****************************************************************
**  FUNCTION:       mapL3ObjectInstToNum
**
**  PURPOSE:        Maps a level 3 object instance to number
**
**  INPUT PARMS:    oid - object ID to add
**                  L1_inst - level 1 object instance
**                  L2_Inst - level 2 object instance 
**                  L3_Inst - instance number to be mapped
**
**  OUTPUT PARMS:   Num - number of the mapped object
**
**  RETURNS:        CMSRET_SUCCESS if success
**                  otherwise failed, check with reason code
**
*******************************************************************/
static CmsRet mapL3ObjectInstToNum ( MdmObjectId oid, int L1_Inst, int L2_Inst, int L3_Inst, int *Num )
{
    int  i = -1;
    CmsRet ret  = CMSRET_SUCCESS;
    InstanceIdStack  iidStack = EMPTY_INSTANCE_ID_STACK;
    InstanceIdStack  searchIidStack = EMPTY_INSTANCE_ID_STACK;
    void  *obj = NULL;

    PUSH_INSTANCE_ID(&iidStack, L1_Inst);
    PUSH_INSTANCE_ID(&iidStack, L2_Inst);
    ret = cmsObj_getNextInSubTreeFlags(oid, &iidStack, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **)&obj);
    while( ret == CMSRET_SUCCESS ){
        cmsObj_free((void **)&obj );
        i++;
        if( L3_Inst == (PEEK_INSTANCE_ID(&searchIidStack))){
            break;
        }
        ret = cmsObj_getNextInSubTreeFlags(oid, &iidStack, &searchIidStack, OGF_NO_VALUE_UPDATE, (void **)&obj);
    }

    if( ret == CMSRET_SUCCESS )
    {
        *Num = i;
        cmsLog_debug("%s() map L3 object (%d) instance (%d) to number (%d)", __FUNCTION__, oid, L3_Inst, *Num );
    }
    else
    {
        cmsLog_error("%s() could not find L3 object (%d) instance (%d)", __FUNCTION__, oid, L3_Inst);
    }
    return ret;
}
#endif

#if VOICE_IPV6_SUPPORT
/*****************************************************************
**  FUNCTION:       stripIpv6PrefixLength
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
static CmsRet stripIpv6PrefixLength(VoiceObject *voiceObj, char *ipAddress)
{
   /* Sanity check */
   if ( ipAddress == NULL || voiceObj == NULL )
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   if ( voiceObj->X_BROADCOM_COM_IpAddressFamily != NULL  &&
        !strncmp(voiceObj->X_BROADCOM_COM_IpAddressFamily, MDMVS_IPV6, strlen(voiceObj->X_BROADCOM_COM_IpAddressFamily)) )
   {
      char *tempAt;

      tempAt = strchr( ipAddress, '/' );
      if ( tempAt != NULL )
      {
         *tempAt = '\0';
      }
   }

   return CMSRET_SUCCESS;
}
#endif /* VOICE_IPV6_SUPPORT */


/* current TR104v2 model only needs 4 levels of multiple instance tree */
static CmsRet getObject( MdmObjectId oid, int L1Idx, int L2Idx, int L3Idx, int L4Idx, UINT32 flags, InstanceIdStack *outStack, void **obj)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

    if( L1Idx > 0 ){
        PUSH_INSTANCE_ID( &iidStack, L1Idx );
    }
    if( L2Idx > 0 ){
        PUSH_INSTANCE_ID( &iidStack, L2Idx );
    }
    if( L3Idx > 0 ){
        PUSH_INSTANCE_ID( &iidStack, L3Idx );
    }
    if( L4Idx > 0 ){
        PUSH_INSTANCE_ID( &iidStack, L4Idx );
    }

    ret = cmsObj_get( oid, &iidStack, flags, obj );
    if( CMSRET_SUCCESS == ret && outStack )
    {
        memcpy( outStack, &iidStack, sizeof(InstanceIdStack));
    }

    return ret;
}

static CmsRet convertStringToBool( const char *input, UBOOL8 *value )
{
    if ( !strncasecmp(input, MDMVS_ON, strlen(MDMVS_ON)) ||
         !strncasecmp(input, MDMVS_YES, strlen(MDMVS_YES)) ||
         !strncasecmp(input, MDMVS_ENABLED, strlen(MDMVS_ENABLED)) ||
         !strncasecmp(input, "true", strlen("true")))
    {

        *value = 1;
        return CMSRET_SUCCESS;
    }

    if ( !strncasecmp(input, MDMVS_OFF, strlen(MDMVS_OFF)) ||
         !strncasecmp(input, MDMVS_NO, strlen(MDMVS_NO)) ||
         !strncasecmp(input, MDMVS_DISABLED, strlen(MDMVS_DISABLED)) ||
         !strncasecmp(input, "false", strlen("false")))
    {

        *value = 0;
        return CMSRET_SUCCESS;
    }

    return CMSRET_INVALID_ARGUMENTS;
}

static CmsRet isValidString( char *input, const char **validStr )
{
    int   i;

    cmsLog_debug( "%s \n", __FUNCTION__ );

    if( validStr == NULL )
        return CMSRET_SUCCESS;

    for (i = 0; input != NULL && validStr[i] != NULL; i++)
    {
        cmsLog_debug( "%s input string = %s, validString = %s\n", __FUNCTION__, input, validStr[i] );
        if( strncasecmp(input, validStr[i], strlen(input)) == 0 )
        {
            strncpy( input, validStr[i], strlen(validStr[i])); /* replace input string by standard valid string */
            return CMSRET_SUCCESS;
        }
    }

    return CMSRET_INVALID_ARGUMENTS;
}

/*
** TODO: Following DAL interface should be obesoleted,
** only keep them here because linking error during compile
**
*/

CmsRet dalVoice_GetNtrEnable(DAL_VOICE_PARMS *parms, char *enabled, unsigned int length )
{
    snprintf( (char*)enabled, length, "0" ); /* always return false */
    return (CMSRET_SUCCESS);
}

CmsRet dalVoice_SetNtrEnable(DAL_VOICE_PARMS *parms, char *setVal )
{
    return (CMSRET_SUCCESS);
}

CmsRet dalVoice_GetLogServer(DAL_VOICE_PARMS *parms, char *logServer, unsigned int length )
{
    return (CMSRET_SUCCESS);
}

CmsRet dalVoice_GetLogServerPort(DAL_VOICE_PARMS *parms, char *port, unsigned int length )
{
    return (CMSRET_SUCCESS);
}

CmsRet dalVoice_GetSipTransportString(DAL_VOICE_PARMS *parms, char *transport, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;

    GET_SIP_NETWORK_PARAM_STR(parms->op[0], parms->op[1], registrarServerTransport, transport, length );

    return ret;
}


#define GET_CALL_LOG_PARAM_STR(i, p, n, v, l) \
{                                                   \
    VoiceCallLogObject *obj=NULL;                   \
    MdmObjectId __oid = MDMOID_VOICE_CALL_LOG;      \
    GET_L2OBJ_PARAM_STR(i, p, n, v, l)              \
}

#define GET_CALL_LOG_PARAM_UINT(i, p, n, v, l)      \
{                                                   \
    VoiceCallLogObject *obj=NULL;                   \
    MdmObjectId     __oid = MDMOID_VOICE_CALL_LOG;  \
    GET_L2OBJ_PARAM_UINT(i, p, n, v, l);            \
}

#define SET_CALL_LOG_PARAM_STR(i, p, n, v, f)       \
{                                                   \
    VoiceCallLogObject *obj=NULL;                   \
    MdmObjectId     __oid = MDMOID_VOICE_CALL_LOG;  \
    SET_L2OBJ_PARAM_STR(i, p, n, v, f);             \
}

CmsRet dalVoice_mapCallLogNumToInst( DAL_VOICE_PARMS *parms, int *inst ) 
{
   return mapL2ObjectNumToInst( MDMOID_VOICE_CALL_LOG, parms->op[0], parms->op[1], inst);
}

CmsRet dalVoice_AddCallLogInstance(DAL_VOICE_PARMS *parms, int *value)
{
   CmsRet           ret;

   /* Add the voice call to the list */
   ret = addL2Object( MDMOID_VOICE_CALL_LOG, parms->op[0], (int *)value );
   cmsLog_debug("Added voice call instance %u\n", (*value));
   return ret;
}

CmsRet dalVoice_DeleteCallLogInstance(DAL_VOICE_PARMS *parms)
{
   CmsRet           ret;

   cmsLog_debug("delete voice call instance %u\n", parms->op[1]);
   /* Delete the voice call to the list */
   ret = delL2Object( MDMOID_VOICE_CALL_LOG, parms->op[0], parms->op[1]);
   return ret;
}

CmsRet dalVoice_TrimCallLogInstance(DAL_VOICE_PARMS *parms, unsigned int *value)
{
   CmsRet  ret;
   int     i, maxCount, num, inst;

   ret = dalVoice_GetMaxCallLogCount( parms, &maxCount );
   if( ret == CMSRET_SUCCESS && maxCount > 0 )
   {
      ret = dalVoice_GetNumVoiceCallLog( parms, &num);
      if( ret == CMSRET_SUCCESS )
      {
	 if( num <= maxCount ) /* no need to trim */
 	 {
            *value = 0;
            return CMSRET_SUCCESS;
         }
	 else
         {
            cmsLog_debug("voice call instance %u, max count %d\n", num, maxCount);
            for( i = 0, (*value) = 0; i < ( num - maxCount); i++,(*value)++ )
            {
               /* assume that oldest entry is alway at begin */
               ret = mapL2ObjectNumToInst( MDMOID_VOICE_CALL_LOG, parms->op[0], 0, &inst );
               if( ret == CMSRET_SUCCESS && inst > 0 )
               {
                   delL2Object( MDMOID_VOICE_CALL_LOG, parms->op[0], inst);
                   cmsLog_debug("Removed voice call instance %u\n", inst);
               }
            }
         }
      }
   }

   return ret;
}

CmsRet  dalVoice_GetVoiceCallLogUsedLine(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
    int  lineInst, lineNum;
    CmsRet ret = CMSRET_SUCCESS;

    ret = getL2ToL2ObjAssoc( MDMOID_VOICE_CALL_LOG, MDMOID_CALL_CONTROL_LINE, parms->op[0], parms->op[1], &lineInst);
    if( ret == CMSRET_SUCCESS )
    {
        mapL2ObjectInstToNum( MDMOID_CALL_CONTROL_LINE, parms->op[0], lineInst, &lineNum );
        snprintf( value, length, "%u" , lineNum);
    }

    return ret;
}

CmsRet  dalVoice_GetVoiceCallLogDirection(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;

    GET_CALL_LOG_PARAM_STR(parms->op[0], parms->op[1], direction, value, length);

    return ret;
}

CmsRet  dalVoice_GetVoiceCallLogCaller(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;

    GET_CALL_LOG_PARAM_STR(parms->op[0], parms->op[1], callingPartyNumber, value, length);

    return ret;
}

CmsRet  dalVoice_GetVoiceCallLogStartTime(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;

    GET_CALL_LOG_PARAM_STR(parms->op[0], parms->op[1], start, value, length);

    return ret;
}

CmsRet  dalVoice_GetVoiceCallLogDuration(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;

    GET_CALL_LOG_PARAM_UINT(parms->op[0], parms->op[1], duration, value, length);

    return ret;
}

CmsRet  dalVoice_GetVoiceCallLogReason(DAL_VOICE_PARMS *parms, char *value, unsigned int length )
{
    CmsRet ret = CMSRET_SUCCESS;

    GET_CALL_LOG_PARAM_STR(parms->op[0], parms->op[1], callTerminationCause, value, length);

    return ret;
}

CmsRet  dalVoice_SetCallLogCallingParty(DAL_VOICE_PARMS *parms, char *value)
{
    CmsRet ret;

    SET_CALL_LOG_PARAM_STR(parms->op[0], parms->op[1], callingPartyNumber, value, NULL );
    
    return ret;
}

CmsRet  dalVoice_SetCallLogCalledParty(DAL_VOICE_PARMS *parms, char *value)
{
    CmsRet ret;

    SET_CALL_LOG_PARAM_STR(parms->op[0], parms->op[1], calledPartyNumber, value, NULL );

    return ret;
}

CmsRet  dalVoice_SetCallLogDuration(DAL_VOICE_PARMS *parms, unsigned int value)
{
    CmsRet ret;
    VoiceCallLogObject *obj = NULL;
    MdmObjectId  __oid = MDMOID_VOICE_CALL_LOG;

    if( parms == NULL )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    SET_L2OBJ_PARAM_UINT(parms->op[0], parms->op[1], duration, value, 0xFFFFFFFF );
    return ret;
}

CmsRet  dalVoice_SetCallLogUsedLine(DAL_VOICE_PARMS *parms, char *value)
{
    CmsRet ret;

    SET_CALL_LOG_PARAM_STR(parms->op[0], parms->op[1], usedLine, value, NULL );

    return ret;
}

CmsRet  dalVoice_SetCallLogReason(DAL_VOICE_PARMS *parms, char *value)
{
    CmsRet ret;

    SET_CALL_LOG_PARAM_STR(parms->op[0], parms->op[1], callTerminationCause, value, NULL );

    return ret;
}

CmsRet  dalVoice_SetCallLogStartDateTime(DAL_VOICE_PARMS *parms, char *value)
{
    CmsRet ret;
    VoiceCallLogObject *obj = NULL;
    MdmObjectId  __oid = MDMOID_VOICE_CALL_LOG;

    if( parms == NULL || value == NULL || strlen(value) <=0 )
    {
        return CMSRET_INVALID_ARGUMENTS;
    }

    SET_L2OBJ_PARAM_STR(parms->op[0], parms->op[1], start, value, NULL );
    return ret;
}



CmsRet dalVoice_AddVoiceCallLogEntry(DAL_VOICE_PARMS *parms, char *caller, char *callee, char* direction, char *dateTime, unsigned int duration, int lineId, char *reason, int *logInst)
{
   CmsRet ret;
   int   lineInst;
   unsigned int count = 0;

   /* Sanity on the callt ype */
   if( parms == NULL || caller == NULL || callee == NULL ||
       direction == NULL || dateTime == NULL || lineId < 0 ||
       reason == NULL || logInst == NULL )
   {
      cmsLog_error("%s: Invalid call parameters.", __FUNCTION__);
      return CMSRET_INVALID_PARAM_VALUE;
   }

   ret = dalVoice_AddCallLogInstance(parms, logInst);
   if(ret == CMSRET_SUCCESS && (*logInst) > 0)
   {
      CmsRet setRetVal;
      /* op[1] = Voice call list instance id */
      parms->op[1] = (*logInst);

      setRetVal = dalVoice_SetCallLogCallingParty(parms, caller);
      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add caller number, ret = %d\n", __FUNCTION__, ret);
         ret = setRetVal;
      }

      setRetVal = dalVoice_SetCallLogCalledParty(parms, callee);
      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add callee number, ret = %d\n", __FUNCTION__, ret);
         ret = setRetVal;
      }

      setRetVal = dalVoice_SetCallLogStartDateTime(parms, dateTime);
      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add date-time, ret = %d\n", __FUNCTION__, ret);
         ret = setRetVal;
      }

      setRetVal = dalVoice_SetCallLogDuration(parms, duration);
      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add duration, ret = %d\n", __FUNCTION__, ret);
         ret = setRetVal;
      }

      setRetVal = mapL2ObjectNumToInst( MDMOID_CALL_CONTROL_LINE, parms->op[0], lineId, &lineInst);
      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add line id, ret = %d\n", __FUNCTION__, ret);
      }
      else
      {
          cmsLog_debug("%s() map callctrl line (%u) to line path\n", __FUNCTION__, lineInst);
          /* initialize pathDesc for codec */
          setRetVal = setL2ToL2ObjAssoc( MDMOID_VOICE_CALL_LOG, MDMOID_CALL_CONTROL_LINE, parms->op[0], (*logInst), lineInst);
          if( setRetVal != CMSRET_SUCCESS )
          {
             cmsLog_error("%s: Can't add call type, ret = %d\n", __FUNCTION__, ret);
             ret = setRetVal;
          }
      }

      setRetVal = dalVoice_SetCallLogReason(parms, reason);
      if( setRetVal != CMSRET_SUCCESS )
      {
         cmsLog_error("%s: Can't add call type, ret = %d\n", __FUNCTION__, ret);
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
   dalVoice_TrimCallLogInstance(parms, &count);
   cmsLog_debug("%s trimed %u old log entries \n", __FUNCTION__, count);

   return ret;
}







#endif /* DMP_VOICE_SERVICE_2 */
/*<END>================================= Common Helper Functions =========================================<END>*/


/*<START>================================= Common Nwk Interface/Firewall Helper Functions =========================================<START>*/
#ifdef DMP_DEVICE2_BASELINE_1
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

/*****************************************************************
**  FUNCTION:       dalVoice_GetNetworkIntfList_dev2
**
**  PUROPOSE:       Obtains the list of available network interfaces
**
**  INPUT PARMS:    None
**
**  OUTPUT PARMS:   intfList - obtained list of network interfaces
**
**  RETURNS:        CMSRET_SUCCESS - Success
**
**  NOTE:
*******************************************************************/
CmsRet dalVoice_GetNetworkIntfList_dev2( DAL_VOICE_PARMS *parms __attribute__((unused)),
                                         char *intfList, unsigned int length )
{
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

   /* Add the default interfaces */
   snprintf(intfList, length, "%s %s", MDMVS_LAN, MDMVS_ANY_WAN);

   /*
    * Append all non bridged WAN interface.  Why does TR98 code also add
    * bridged interfaces?  The voice code in ssk will not start voice on a
    * bridged WAN connection.
    */
   while (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ipIntfObj) == CMSRET_SUCCESS)
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream &&
          !ipIntfObj->X_BROADCOM_COM_BridgeService)
      {
         cmsUtl_strncat(intfList, length, " ");
         cmsUtl_strncat(intfList, length, ipIntfObj->name);
      }

      cmsObj_free((void **) &ipIntfObj);
   }

   return CMSRET_SUCCESS;
}



/* helper functions for dalVoice_performFilterOperation_dev2 */
static void addVoiceFirewallException_dev2(const char *ifName, const DAL_VOICE_FIREWALL_CTL_BLK *fwCtlBlk);
static void deleteVoiceFirewallException_dev2(const char *ifName, const char *filterName);

CmsRet dalVoice_performFilterOperation_dev2( DAL_VOICE_PARMS *parms __attribute__((unused)),
                                       DAL_VOICE_FIREWALL_CTL_BLK *fwCtlBlk )
{
   DAL_VOICE_PARMS parms1;
   char ifName[MAX_TR104_OBJ_SIZE]={0};
   Dev2IpInterfaceObject *ipIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;


   /* Get voice interface name */
   parms1.op[0 ]= parms1.op[1] = 1;
   if ( (ret = dalVoice_GetBoundIfName( &parms1, ifName, sizeof(ifName) )) != CMSRET_SUCCESS )
   {
      cmsLog_error("Failed to get interface name");
      return ret;
   }

   cmsLog_debug("boundIfName=%s enable=%d filterName=%s",
                 ifName, fwCtlBlk->enable, ifName);

   /* Check to see if we are in LAN, if so, no need to proceed */
   if( !(cmsUtl_strcmp( ifName, MDMVS_LAN )) )
   {
      cmsLog_debug( "Interface is LAN. No need for filter operations\n" );
      return CMSRET_SUCCESS;
   }


   /* loop through all WAN, non-bridged, and UP IP.Interface and call helper
    * functions for each matching IP.Interface
    */
   while (cmsObj_getNextFlags(MDMOID_DEV2_IP_INTERFACE, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ipIntfObj) == CMSRET_SUCCESS)
   {
      if (ipIntfObj->X_BROADCOM_COM_Upstream &&
          !ipIntfObj->X_BROADCOM_COM_BridgeService &&
          !cmsUtl_strcmp(ipIntfObj->status, MDMVS_UP) &&
          (  !cmsUtl_strcmp(ifName, MDMVS_ANY_WAN) ||
             !cmsUtl_strncmp(ifName, ipIntfObj->name, strlen(ifName)))  )
      {
         if (fwCtlBlk->enable)
         {
            addVoiceFirewallException_dev2(ipIntfObj->name, fwCtlBlk);
         }
         else
         {
            deleteVoiceFirewallException_dev2(ipIntfObj->name, fwCtlBlk->filterName);
         }
      }

      cmsObj_free((void **) &ipIntfObj);
   }

   return ret;
}


void addVoiceFirewallException_dev2(const char *ifName,
                                    const DAL_VOICE_FIREWALL_CTL_BLK *fwCtlBlk)
{
   char srcPortBuf[BUFLEN_32]={0};
   char dstPortBuf[BUFLEN_32]={0};
   CmsRet ret;

   cmsLog_debug("Entered: ifName=%s fwCtlBlk->filterName=%s",
                ifName, fwCtlBlk->filterName);

   sprintf(srcPortBuf, "%d", fwCtlBlk->sourcePort);
   sprintf(dstPortBuf, "%d", fwCtlBlk->destinationPort);


   /*
    * Everybody should just use the DAL function to add exception.  But
    * to avoid regression, I left the original TR98 function and its
    * SET_EXPCEPTION_OBJ_FIELDS macro alone.  The logic for extracting values
    * from fwCtlBlk is the same as that macro.
    */
   ret = dalSec_addIpFilterIn_dev2(fwCtlBlk->filterName,
               ((strchr(fwCtlBlk->sourceIPAddress, ':') == NULL) ?
                                                        MDMVS_4 : MDMVS_6),
               fwCtlBlk->protocol,
               (cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX,
                                         fwCtlBlk->sourceIPAddress) ?
                                         "" : fwCtlBlk->sourceIPAddress),
               (cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX,
                                         fwCtlBlk->sourceNetMask) ?
                                         "" : fwCtlBlk->sourceNetMask),
               srcPortBuf,
               (cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX,
                                         fwCtlBlk->destinationIPAddress) ?
                                         "" : fwCtlBlk->destinationIPAddress),
               (cmsUtl_isZeroIpvxAddress(CMS_AF_SELECT_IPVX,
                                         fwCtlBlk->destinationNetMask) ?
                                         "" : fwCtlBlk->destinationNetMask),
               dstPortBuf,
               ifName);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("dalSec_addIpFilterIn_dev2 failed, ret=%d", ret);
   }

   return;
}


void deleteVoiceFirewallException_dev2(const char *ifName,
                                       const char *filterName)
{
   UBOOL8 found=FALSE;
   char *ipIntfFullPath=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2FirewallExceptionObject *fwExceptionObj=NULL;

   qdmIntf_intfnameToFullPathLocked(ifName, FALSE, &ipIntfFullPath);

   cmsLog_debug("Entered: ipIntfFullPath=%s filterName=%s",
                ipIntfFullPath, filterName);


   /* loop through all firewall exceptions and delete the exception under
    * the matching ipIntfFullPath.
    */
   while (!found &&
          (cmsObj_getNextFlags(MDMOID_DEV2_FIREWALL_EXCEPTION, &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void **)&fwExceptionObj) == CMSRET_SUCCESS))
   {
      if(!cmsUtl_strcmp(fwExceptionObj->IPInterface, ipIntfFullPath))
      {
         InstanceIdStack searchIidStack = EMPTY_INSTANCE_ID_STACK;
         InstanceIdStack savedIidStack = EMPTY_INSTANCE_ID_STACK;
         Dev2FirewallExceptionRuleObject *fwExRuleObj = NULL;

         found = TRUE;

         while (cmsObj_getNextInSubTreeFlags(MDMOID_DEV2_FIREWALL_EXCEPTION_RULE,
                                     &iidStack, &searchIidStack, 0,
                                     (void **) &fwExRuleObj) == CMSRET_SUCCESS)
         {
            /* See comment in performFilterOperation in dal_voice.c */
            if (!cmsUtl_strncmp(fwExRuleObj->filterName, filterName, strlen(filterName)))
            {
               cmsObj_deleteInstance(MDMOID_DEV2_FIREWALL_EXCEPTION_RULE,
                                     &searchIidStack);

               /* since we deleted this instance, restore searchIidStack to last good instance */
               searchIidStack = savedIidStack;
            }

            savedIidStack = searchIidStack;
            cmsObj_free((void **) &fwExRuleObj);
         }
      }

      cmsObj_free((void **) &fwExceptionObj);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(ipIntfFullPath);

   return;
}

#endif /* DMP_DEVICE2_BASELINE_1 */
#endif  /* BRCM_VOICE_SUPPORT */
