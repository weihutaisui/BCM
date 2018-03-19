/***********************************************************************
 *
 *  Copyright (c) 2007 Broadcom
 *  All Rights Reserved
 *
 * <:label-BRCM:2007:proprietary:omcid
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
 *
 ************************************************************************/


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#include "omcid_me.h"
#include "owsvc_api.h"
#include "ctlutils_api.h"
#include "owrut_api.h"
#include "me_handlers.h"

#include "omcid.h"
#include "omciutl_cmn.h"
#include "omci_util.h"
#include "omci_pm.h"
#include "omcid_capture.h"
#include "omcid_pm.h"
#include "omcid_priv.h"
#include "omcid_helper.h"
#include "omcipm_ipc_priv.h"

/*
 * Macros
 */
/** Max number of ms to wait for a MDM read lock. */
#define OMCID_LOCK_TIMEOUT  (3 * MSECS_IN_SEC)

#define OMCI_MSG_GET_ATTR_MASK(_pPacket)            \
    ( (OMCI_PACKET_MSG(_pPacket)[0] << 8) | OMCI_PACKET_MSG(_pPacket)[1] )
#define OMCI_MSG_GET_SEQUENCE_NUM(_pPacket)            \
    ( (OMCI_PACKET_MSG(_pPacket)[2] << 8) | OMCI_PACKET_MSG(_pPacket)[3] )
#define OMCI_MSG_SET_ATTR_MASK(_pPacket, off, mask) \
    do {                                                 \
        OMCI_PACKET_MSG(_pPacket)[off] = (UINT8)((mask >> 8) & 0xff); \
        OMCI_PACKET_MSG(_pPacket)[off+1] = (UINT8)(mask & 0xff);      \
    } while (0)

#define OMCI_SWIMAGE_VER_LEN 14
#define CMS_SWIMAGE_VER_LEN  ((OMCI_SWIMAGE_VER_LEN * 2))

/* Some OLT implementations do not work properly if the window size > 64. */
#ifdef OMCIPROV_WORKAROUND
#define OMCI_WINDOW_SIZE_DEF (63)  /*  (64 - 1) */
#else
#define OMCI_WINDOW_SIZE_DEF (127) /* (128 - 1) */
#endif /* OMCIPROV_WORKAROUND */

#define IsSwdlInEndState() \
  ((SoftwareDownload.State == SW_IMG_DL_STATE_END_PENDING) || \
  (SoftwareDownload.State == SW_IMG_DL_STATE_END_FAIL) || \
  (SoftwareDownload.State == SW_IMG_DL_STATE_END_SUCCESS))

/*
 * local enumerations
 */
typedef enum
{
    MIB_DATA_SYNC_OP_RESET,
    MIB_DATA_SYNC_OP_INC,
    MIB_DATA_SYNC_OP_MAX
} mibDataSyncOp_enum;

typedef enum {
    OMCI_SOFTWARE_IMAGE_ID=1,
    OMCI_SOFTWARE_IMAGE_VERSION,
    OMCI_SOFTWARE_IMAGE_COMMIT,
    OMCI_SOFTWARE_IMAGE_ACTIVE,
    OMCI_SOFTWARE_IMAGE_VALID
} omciSoftwareImageField;

/*
 * local type definitions
 */
typedef struct
{
    int (*pMsgHandler)(omciPacket *, int);
    UINT8 incMibDataSync;
    char  name[BUFLEN_32];
} omciMsgHandler_t;

/*
 * local variables
 */
static const omciMeInfo_t omciMeInfo[OMCI_ME_CLASS_MAX] = {OMCID_ME_INFO_GENERIC \
                                                           OMCID_ME_INFO_VOIP    \
                                                           OMCID_ME_INFO_SIP     \
                                                           OMCID_ME_INFO_MGCP    };

static mibUpload_t mibUpload_mibCopy[OMCI_MIB_UPLOAD_INST_MAX];
static UINT16 mibUpload_count = 0; /* number of fragments to upload */
static UINT16 mibUpload_seqNbr = 0;
static UINT32 mibUpload_instIndex = 0; /* current object in mibCopy during "upload next" phase */
static UINT16 mibUpload_nextAttrMask = 0;
static UINT16 mibUpload_fragCount = 1;

static UINT16 numberOf15MinInterval = 0;


typedef enum
{
    SW_IMG_DL_STATE_INIT,
    SW_IMG_DL_STATE_START,
    SW_IMG_DL_STATE_DOWNLOAD,
    SW_IMG_DL_STATE_END_PENDING,
    SW_IMG_DL_STATE_END_SUCCESS,
    SW_IMG_DL_STATE_END_FAIL,
    SW_IMG_DL_STATE_ACTIVATE,
    SW_IMG_DL_STATE_COMMIT
} OmciSwImgDlState_t;

typedef struct
{
    UINT16              WindowSize;
    UINT32              ImageSize;
    UINT32              CurrentImageSize;
    UINT32              CurrentWindowSize;
    UINT32              ExpectedSectionNumber;
    UINT8*              pWindowBuffer;
    int                 FlashPid;
    OmciSwImgDlState_t  State;
    struct
    {
        UINT32  SectionMissing:1;
        UINT32  DownloadActive:1;   // SW img dl is considered active from the start up to a reboot, activation, or restart of dl.
    } Flags;
    UINT8               Instance;
    UINT8               ActiveInstance;

} SoftwareDownload_t;

static SoftwareDownload_t SoftwareDownload = {0,0,0,0,0,NULL,-1,SW_IMG_DL_STATE_INIT, {0,0},0,0};
static IMGUTIL_HANDLE imgutilHandle = NULL;
static int imgBootState = -1;

typedef struct
{
    CmsTimestamp    Start;
    CmsTimestamp    Current;
    CmsTimestamp    Delta;
} MibUploadTime_t;

static MibUploadTime_t MibUploadTime;

static char resultMessageTable[OMCI_MSG_RESULT_MAX][BUFLEN_64] = {
    [OMCI_MSG_RESULT_SUCCESS]         = "Success",
    [OMCI_MSG_RESULT_PROC_ERROR]      = "Process error",
    [OMCI_MSG_RESULT_NOT_SUPPORTED]   = "Not Supported",
    [OMCI_MSG_RESULT_PARM_ERROR]      = "Parameter error",
    [OMCI_MSG_RESULT_UNKNOWN_ME]      = "Unknown managed entity",
    [OMCI_MSG_RESULT_UNKNOWN_ME_INST] = "Unknown managed entity instance",
    [OMCI_MSG_RESULT_DEV_BUSY]        = "Device is busy",
    [OMCI_MSG_RESULT_ME_INST_EXISTS]  = "Managed entity instance already exists",
    [OMCI_MSG_RESULT_ATTR_FAILED]     = "Attribute(s) failed or unknown"
};

static UINT8 Inst2PartTable [2] =
{
    [0]=CMS_IMAGE_WR_OPT_PART1,
    [1]=CMS_IMAGE_WR_OPT_PART2
};

static UINT8 Inst2BootPartTableActivate [2] =
{
    [0]=BOOT_SET_PART1_IMAGE_ONCE,
    [1]=BOOT_SET_PART2_IMAGE_ONCE
};

static UINT8 Inst2BootPartTableCommit [2] =
{
    [0]=BOOT_SET_PART1_IMAGE,
    [1]=BOOT_SET_PART2_IMAGE
};

static char ImageVersionTable[2][CMS_SWIMAGE_VER_LEN + 1] =
{
    [0]=OMCI_SW_IMAGE_0_VERSION,
    [1]=OMCI_SW_IMAGE_1_VERSION
};

OmciDbgErr_t
    OmciDbgErr=OMCI_ERR_NONE;


// scheduleHandleModelPath() is implemented in omcid.c
extern void scheduleHandleModelPath(UBOOL8);
extern UBOOL8 bcmImg_willFitInFlash(UINT32 imageSize);

static void sendSystemReboot(void *handle __attribute__((unused)))
{
    _owapi_rut_systemReboot();
}

static CmsRet modifySoftwareImageObject(UINT8 instanceId, omciSoftwareImageField field, char * pNewState);

#if 0
static void getDownloadedImageVersionFromFlash(UINT8 instance, char *asciiVerStrP)
{
    int asciiStrLen;

    if (0 != (asciiStrLen = devCtl_getImageVersion(instance + 1, asciiVerStrP,
      OMCI_SWIMAGE_VER_LEN)))
    {
        printf("#################################################################\n");
        printf("Instance %d Version string %s extracted from image\n",
          instance, asciiVerStrP);
        printf("#################################################################\n");
    }
    else
    {
        memcpy(asciiVerStrP, (instance == 0) ? OMCI_SW_IMAGE_0_VERDFLT :
          OMCI_SW_IMAGE_1_VERDFLT, OMCI_SWIMAGE_VER_LEN);
        printf("0 bytes copied from flash to ME version string for software "
          "image %d, using default value %s\n",
          instance, asciiVerStrP);
    }
}
#endif

static void updateDownloadedSoftwareImageObject(char *versionStr)
{
    UBOOL8 newState = TRUE;
    CmsRet cmsRet = CMSRET_SUCCESS;
    char version[CMS_SWIMAGE_VER_LEN + 1];
    int imgVerLen = strlen(versionStr);
    int i;

    cmsRet = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT);
    if (cmsRet != CMSRET_SUCCESS)
    {
        cmsLog_error("_cmsLck_acquireLockWithTimeout() failed, ret=%d", cmsRet);
        return;
    }

    memset(&version, 0x0, CMS_SWIMAGE_VER_LEN + 1);
    printf("AsciiVerStr len =%d, %s ...\n", imgVerLen, versionStr);
    if (imgVerLen > OMCI_SWIMAGE_VER_LEN)
    {
        imgVerLen = OMCI_SWIMAGE_VER_LEN;
    }

    for (i = 0; i < (int)imgVerLen; i++)
    {
        sprintf(version + (2 * i), "%.2X", versionStr[i]);
    }
    modifySoftwareImageObject(SoftwareDownload.Instance,
      OMCI_SOFTWARE_IMAGE_VALID, (char*)&newState);
    modifySoftwareImageObject(SoftwareDownload.Instance,
      OMCI_SOFTWARE_IMAGE_VERSION, (char*)&version);

    _cmsLck_releaseLock();
}

static void flashImageInThread(void *data __attribute__((unused)))
{
    int rc1, rc2;
    imgutil_img_info_t imgInfo;
    char imgVersion[OMCI_SWIMAGE_VER_LEN +1];

    imgBootState = devCtl_getImageState();

    devCtl_setImageState(BOOT_SET_OLD_IMAGE);

    rc1 = img_util_get_imginfo(imgutilHandle, IMGINFO_SRC_BLTIN,
      IMGUTIL_IMGINFO_BIT_VERSION, &imgInfo);
    rc2 = img_util_close(imgutilHandle, NULL, NULL);
    imgutilHandle = NULL;
    if (rc2 < 0)
    {
       SoftwareDownload.State = SW_IMG_DL_STATE_END_FAIL;
       cmsLog_error("img_util_close() failed, ret=%d", rc2);
       goto flashing_exit;
    }
    else
    {
       SoftwareDownload.State = SW_IMG_DL_STATE_END_SUCCESS;
       if (rc1 == 0)
       {
          strncpy(imgVersion, (char*)imgInfo.version,
            OMCI_SWIMAGE_VER_LEN);
          updateDownloadedSoftwareImageObject(imgVersion);
       }
       printf("End flashing image!!\n");
    }

flashing_exit:
    if (imgBootState >= 0)
    {
        devCtl_setImageState(BOOT_SET_OLD_IMAGE);
        imgBootState = -1;
    }
    SoftwareDownload.FlashPid = -1;
    pthread_exit(0);
}

static void createFlashImageThread(void)
{
    pthread_t flashThread;

    // create a child thread to flash image
    if (pthread_create(&flashThread, NULL, (void *) &flashImageInThread, NULL) == CMSRET_SUCCESS)
    {
        SoftwareDownload.FlashPid = flashThread;
    }
    else
    {
        SoftwareDownload.FlashPid = -1;
        cmsLog_error("Failed to create thread to flash image");
    }
}

#if 0 /* not used */
static void omci_msg_dump_stats(void)
{
    DECLARE_PGPONOMCI();

    cmsLog_setHeaderMask(0);

    cmsLog_notice("*** OMCI Stack Statistics ***\n");
    cmsLog_notice("rxPackets : %d", pGponOmci->stats.rxGoodPackets);
    cmsLog_notice("rxErrors  : %d",
                  pGponOmci->stats.rxLengthErrors +
                  pGponOmci->stats.rxCrcErrors +
                  pGponOmci->stats.rxOtherErrors);
    cmsLog_notice("   rxLengthErrors : %d", pGponOmci->stats.rxLengthErrors);
    cmsLog_notice("   rxCrcErrors    : %d", pGponOmci->stats.rxCrcErrors);
    cmsLog_notice("   rxOtherErrors  : %d\n", pGponOmci->stats.rxOtherErrors);
    cmsLog_notice("txPackets : %d",
                  pGponOmci->stats.txAvcPackets +
                  pGponOmci->stats.txResponsePackets +
                  pGponOmci->stats.txRetransmissions);
    cmsLog_notice("   txAttrValChg      : %d", pGponOmci->stats.txAvcPackets);
    cmsLog_notice("   txResponses       : %d", pGponOmci->stats.txResponsePackets);
    cmsLog_notice("   txRetransmissions : %d", pGponOmci->stats.txRetransmissions);
    cmsLog_notice("txErrors  : %d", pGponOmci->stats.txErrors);
    cmsLog_notice("\n");

    cmsLog_setHeaderMask(DEFAULT_LOG_HEADER_MASK);
}
#endif

static inline int omci_msg_padding(omciPacket *pPacket, int start)
{
    int n = OMCI_PACKET_A_MSG_SIZE - start;

    /* No padding for Extended messages, set msglen instead */
    if (OMCI_CHECK_DEV_ID_B(pPacket))
    {
        OMCI_HTONS(&pPacket->B.msgLen, start);
        return 0;
    }

    if(n < 0)
    {
        cmsLog_error("Invalid Padding Size (n %d, start %d)", n, start);
        return -1;
    }

    memset(&OMCI_PACKET_MSG(pPacket)[start], 0, n);
    return 0;
}

static int omci_msg_send_response(omciPacket *pPacket)
{
    int rv = 0;

    // if request bit is 0, then do not send response
    if (OMCI_PACKET_AR(pPacket) == 0)
    {
        return rv;
    }

#if !defined(OMCID_DISABLE_RESPONSE)
    int txCount;
    int prio = OMCI_PACKET_PRIORITY(pPacket);
    DECLARE_PGPONOMCI();
    omciPacket *pRespPacket = &pGponOmci->prevRespPacket[prio];

    /* update header */
    pRespPacket->tcId[0] = pPacket->tcId[0];
    pRespPacket->tcId[1] = pPacket->tcId[1];
    pRespPacket->msgType = 0x20 | OMCI_PACKET_MT(pPacket);
    pRespPacket->devId   = pPacket->devId;
    pRespPacket->classNo[0] = pPacket->classNo[0];
    pRespPacket->classNo[1] = pPacket->classNo[1];
    pRespPacket->instId[0]  = pPacket->instId[0];
    pRespPacket->instId[1]  = pPacket->instId[1];

    /* save message */
    memcpy(OMCI_PACKET_MSG(pRespPacket), OMCI_PACKET_MSG(pPacket), OMCI_PACKET_MSG_LEN(pPacket));

    if (OMCI_CHECK_DEV_ID_A(pPacket))
    {
        pRespPacket->A.trailer[3] = OMCI_PACKET_CPCS_SDU_LEN;
    }
    else if (OMCI_CHECK_DEV_ID_B(pPacket))
    {
        pRespPacket->B.msgLen[0] = pPacket->B.msgLen[0];
        pRespPacket->B.msgLen[1] = pPacket->B.msgLen[1];
    }

    /* save TC ID */
    pGponOmci->prevTcId[prio] = OMCI_PACKET_TC_ID(pPacket);

    cmsLog_notice(" Response Message:\n");
    OMCID_DUMP_PACKET(pRespPacket, OMCI_PACKET_A_SIZE);

    // if replay is active, then do not send response
    if (TRUE == OmciCapture.Flags.Replaying)
    {
        return rv;
    }

    // capture every omci rsp packet
    omci_msg_capture((UINT8 *)pRespPacket, OMCI_PACKET_SIZE(pRespPacket));

    /* only send response packet to driver if requester is not HTTPD */
    if (pPacket->src_eid != EID_HTTPD)
    {
        int tx_size = OMCI_PACKET_SIZE(pRespPacket) - OMCI_PACKET_MIC_SIZE;

        /* transmit response packet */
        txCount = gpon_omci_api_transmit(pRespPacket, tx_size);

        if((txCount == -1) && (errno == ENOLINK))
        {
            /* Return success: the OMCI Port has been deleted */
            cmsLog_notice("OMCI Port has been deleted, continue");
            rv = 0;
        }
        else if(txCount != tx_size)
        {
            cmsLog_error("Failed to transmit Response Packet (count = %d)\n",
                         txCount);
            OMCID_STATS_INC(txErrors);
            rv = -1;
        }
        else
        {
            OMCID_STATS_INC(txResponsePackets);
        }
    }
#endif

    return rv;
}

omciMsgResult omci_msg_get_meInfo(UINT16 meClass,
                                  omciMeInfo_t *pMeInfo)
{
    UINT32 mdmOid;

    if(meClass >= OMCI_ME_CLASS_MAX)
    {
        cmsLog_notice("Invalid ME Class: %d (max %d)", meClass, OMCI_ME_CLASS_MAX);
        return OMCI_MSG_RESULT_UNKNOWN_ME;
    }

    mdmOid = omciMeInfo[meClass].mdmOid;

    if(mdmOid == 0)
    {
        cmsLog_notice("ME Class %d is not supported", meClass);
        return OMCI_MSG_RESULT_UNKNOWN_ME;
    }

    /* return mdm object information */
    memcpy(pMeInfo, &omciMeInfo[meClass], sizeof(omciMeInfo_t));

    return OMCI_MSG_RESULT_SUCCESS;
}

UBOOL8 isAttributeTable(UINT16 meClass, UINT16 attrMask)
{
    omciMsgResult msgRet;
    omciMeInfo_t meInfo;

    msgRet = omci_msg_get_meInfo(meClass, &meInfo);
    if (msgRet == OMCI_MSG_RESULT_SUCCESS)
    {
        if ((attrMask & meInfo.tableAttrMask) == attrMask)
        {
            return TRUE;
        }
    }
    return FALSE;
}
static CmsRet mib_data_sync_update(mibDataSyncOp_enum op)
{
    OntDataObject *obj;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        return ret;
    }

    if ((ret = _cmsObj_get(MDMOID_ONT_DATA, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("get of GPON_ONT_DATA object failed, ret=%d", ret);
    }
    else
    {
        switch(op)
        {
            case MIB_DATA_SYNC_OP_RESET:
                obj->mibDataSync = 0;
                break;

            case MIB_DATA_SYNC_OP_INC:
                if(obj->mibDataSync == 255)
                {
                    obj->mibDataSync = 1;
                }
                else
                {
                    obj->mibDataSync++;
                }
                break;

            default:
                cmsLog_error("Invalid argument, op=%d", op);
                ret = CMSRET_INVALID_ARGUMENTS;
                break;
        }

        if(ret == CMSRET_SUCCESS)
        {
            if ((ret = _cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
            {
                cmsLog_error("set of GPON_ONT_DATA failed, ret=%d", ret);
                /* XXX Debug This !!! */
                ret = CMSRET_SUCCESS;
            }
            else
            {
                /* debugging only */
                OMCID_DUMP_OBJECT(obj);
            }
        }

        _cmsObj_free((void **) &obj);
    }

    _cmsLck_releaseLock();

    return ret;
}

static CmsRet omci_msg_create_mdmObj(UINT32 meInst,
                                     omciMeInfo_t *meInfo,
                                     void **mdmObj,
                                     InstanceIdStack *iidStack)
{
    CmsRet ret = CMSRET_SUCCESS;

    INIT_INSTANCE_ID_STACK(iidStack);

    /* add object instance */
    ret = _cmsObj_addInstance(meInfo->mdmOid, iidStack);
    if(ret != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to create instance of object id %d, ret=%d",
                     meInfo->mdmOid, ret);

        ret = CMSRET_INTERNAL_ERROR;
    }
    else
    {
        /* get mdm object we have just created */
        ret = _cmsObj_get(meInfo->mdmOid, iidStack, OGF_NO_VALUE_UPDATE, mdmObj);
        if(ret != CMSRET_SUCCESS)
        {
            cmsLog_error("failed to get object id %d, ret=%d",
                         meInfo->mdmOid, ret);

            ret = CMSRET_INTERNAL_ERROR;
        }
        else
        {
            /* Set the Managed Entity ID */
            ret = _cmsObj_setNthParam(*mdmObj, 0, &meInst);
            if(ret != CMSRET_SUCCESS)
            {
                cmsLog_error("failed to set ME ID of object id %d, ret=%d",
                             meInfo->mdmOid, ret);

                /* Since we return error, free the mdmObj. */
                _cmsObj_free(mdmObj);  // _cmsObj_free will NULL the ptr
                ret = CMSRET_INTERNAL_ERROR;
            }
        }
    }

    return ret;
}


/** This function will return a valid mdmObj if return value is OMCI_MSG_RESULT_SUCCESS;
 *  Caller is responsible for calling _cmsObj_free on the returned mdmObj
 */
static omciMsgResult omci_msg_get_mdmObj(UINT16 meClass,
                                         UINT32 meInst,
                                         omciMeInfo_t *pMeInfo,
                                         void **pMdmObj,
                                         InstanceIdStack *iidStack)
{
    omciMsgResult msgResult;
    UINT32 meId;
    void *mdmObj;
    _MdmObjParamInfo paramInfo;
    CmsRet ret;
    UINT32 meInstCount;

//    cmsLog_notice("meInst %d (0x%04X)", meInst, meInst);

    msgResult = omci_msg_get_meInfo(meClass, pMeInfo);
    if (msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        return msgResult;
    }

    *pMdmObj = NULL;

    if(pMeInfo->flags & OMCI_ME_FLAG_PRESENT)
    {

        if ((ret = _cmsObj_get(pMeInfo->mdmOid, iidStack, 0, &mdmObj)) != CMSRET_SUCCESS)
        {

            cmsLog_error("_cmsObj_get of %s (oid %d) failed, ret=%d", pMeInfo->name, pMeInfo->mdmOid, ret);

            if(ret == CMSRET_INVALID_PARAM_NAME)
            {
                msgResult = OMCI_MSG_RESULT_UNKNOWN_ME_INST;
            }
            else
            {
                msgResult = OMCI_MSG_RESULT_PROC_ERROR;
            }
        }
        else
        {
            if(OMCI_GET_INSTANCE_BY_NBR & meInst)
            {
                *pMdmObj = mdmObj;
                msgResult = OMCI_MSG_RESULT_SUCCESS;
            }
            else
            {
                if((ret = _cmsObj_getNthParam(mdmObj, 0, &paramInfo)) != CMSRET_SUCCESS)
                {
                    cmsLog_error("_cmsObj_getNthParam of parameter %d from %s (oid %d) failed, ret=%d",
                                 0, pMeInfo->name, pMeInfo->mdmOid, ret);

                    _cmsObj_free(&mdmObj);

                    msgResult = OMCI_MSG_RESULT_PROC_ERROR;
                }
                else
                {
                    meId = *(UINT32 *)(paramInfo.val);

                    cmsLog_notice("meId %d meInst %d", meId, meInst);

                    if(meId == meInst)
                    {
                        *pMdmObj = mdmObj;
                        msgResult = OMCI_MSG_RESULT_SUCCESS;
                    }
                    else
                    {
                        _cmsObj_free(&mdmObj);

                        msgResult = OMCI_MSG_RESULT_UNKNOWN_ME_INST;
                    }
                }
            }
        }
    }
    else
    {
        /* multiple mdm objects may have been created, search for the mdm object
           that matches the instance number */
        meInstCount = 0;
        while(1)
        {
            ret = _cmsObj_getNext(pMeInfo->mdmOid, iidStack, &mdmObj);
            if (ret == CMSRET_NO_MORE_INSTANCES)
            {
                /* no more instances available */
                cmsLog_notice("_cmsObj_getNext: Unknown Instance of %s (oid %d)",
                              pMeInfo->name, pMeInfo->mdmOid);
                msgResult = OMCI_MSG_RESULT_UNKNOWN_ME_INST;
                break;
            }
            else if (ret != CMSRET_SUCCESS)
            {
                cmsLog_error("_cmsObj_getNext %d of %s (oid %d) failed, ret=%d",
                             meInstCount, pMeInfo->name, pMeInfo->mdmOid, ret);

                msgResult = OMCI_MSG_RESULT_PROC_ERROR;
                break;
            }
            else
            {
                if(OMCI_GET_INSTANCE_BY_NBR & meInst)
                {
                    cmsLog_notice("%d: Searching Instance Nbr %d : Found Nbr %d",
                                  meInstCount, meInst & ~OMCI_GET_INSTANCE_BY_NBR, meInstCount);

                    /* get instance by number */
                    if(meInstCount == (meInst & ~OMCI_GET_INSTANCE_BY_NBR))
                    {
                        *pMdmObj = mdmObj;
                        msgResult = OMCI_MSG_RESULT_SUCCESS;
                        break;
                    }
                    else
                    {
                        _cmsObj_free(&mdmObj);
                    }
                }
                else
                {
                    if((ret = _cmsObj_getNthParam(mdmObj, 0, &paramInfo)) != CMSRET_SUCCESS)
                    {
                        cmsLog_error("_cmsObj_getNthParam of parameter %d from %s (oid %d) failed, ret=%d",
                                     0, pMeInfo->name, pMeInfo->mdmOid, ret);

                        _cmsObj_free(&mdmObj);

                        msgResult = OMCI_MSG_RESULT_PROC_ERROR;
                        break;
                    }

                    meId = *(UINT32 *)(paramInfo.val);

                    cmsLog_notice("%d: Searching Instance Id %d : Found Id %d (0x%04X)",
                                  meInstCount, meInst, meId, meId);

                    /* get instance that matches ME Id */
                    if(meId == meInst)
                    {
                        *pMdmObj = mdmObj;
                        msgResult = OMCI_MSG_RESULT_SUCCESS;
                        break;
                    }
                    else
                    {
                        _cmsObj_free(&mdmObj);
                    }
                }
            }

            meInstCount++;
        }
    }

    return msgResult;
}

#if 0
static UINT32 omci_msg_get_mdmObj_size(void *mdmObj, UINT32 firstParam)
{
    MdmObjParamInfo paramInfo;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 paramNbr = 0;
    UINT32 objSize = 0;

    /* copy requested attributes */
    do
    {
        if((ret = _cmsObj_getNthParam(mdmObj, paramNbr, &paramInfo)) != CMSRET_SUCCESS)
        {
            cmsLog_error("get of parameter %d from object id %d failed, ret=%d",
                         paramNbr, OMCID_GET_MDM_OBJ_OID(mdmObj), ret);
            break;
        }

        if(paramNbr >= firstParam)
        {
            objSize += omciUtl_getParamSize(paramInfo.type, paramInfo.maxVal);
        }

    } while(++paramNbr < paramInfo.totalParams);

    return (ret == CMSRET_SUCCESS) ? objSize : 0;
}
#endif

/******************************************************************************
 ** omci_msg_get_next_attribute_position
 *
 *  FILENAME:       omcid_msg.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:    the purpose of this routine is to take the static attribute
 *                  mask and moving bit position and use these to determine the
 *                  position of the next attribute in the mask.
 *  RETURNS:
 *
 */

static UINT32 omci_msg_get_next_attribute_position (UINT16 AttributeList, UINT16 * pCurrentBitPosition)
{

UINT32 AttributePosition;


    switch (*pCurrentBitPosition)
    {
        case 0x8000://bit 15
        {
            AttributePosition = 1;
            break;
        }
        case 0x4000://bit 14
        {
            AttributePosition = 2;
            break;
        }
        case 0x2000://bit 13
        {
            AttributePosition = 3;
            break;
        }
        case 0x1000://bit 12
        {
            AttributePosition = 4;
            break;
        }
        case 0x0800:// bit 11
        {
            AttributePosition = 5;
            break;
        }
        case 0x0400://bit 10
        {
            AttributePosition = 6;
            break;
        }
        case 0x0200://bit 9
        {
            AttributePosition = 7;
            break;
        }
        case 0x0100://bit 8
        {
            AttributePosition = 8;
            break;
        }
        case 0x0080://bit 7
        {
            AttributePosition = 9;
            break;
        }
        case 0x0040:// bit 6
        {
            AttributePosition = 10;
            break;
        }
        case 0x0020://bit 5
        {
            AttributePosition = 11;
            break;
        }
        case 0x0010://bit 4
        {
            AttributePosition = 12;
            break;
        }
        case 0x0008://bit 3
        {
            AttributePosition = 13;
            break;
        }
        case 0x0004://bit 2
        {
            AttributePosition = 14;
            break;
        }
        case 0x0002:// bit 1
        {
            AttributePosition = 15;
            break;
        }
        case 0x0001://bit 0
        {
            AttributePosition = 16;
            break;
        }
        default:
        {
            AttributePosition = 0;
            break;
        }
    }

    if (AttributePosition > 0)
    {
        /* scan attrib list from l to r to determine bit position of next attribute */
        for (; AttributePosition <= 16; AttributePosition++, *pCurrentBitPosition>>=1)
        {
            /* check if the current attribute needs to be included */
            if (*pCurrentBitPosition & AttributeList)
            {
                /* yep, but first advance the bit position indicator for the next go round */
                *pCurrentBitPosition >>= 1;

                break;
            }
        }
        if (AttributePosition == 17)
        {
            AttributePosition = 0;
        }
    }

    return AttributePosition;
}
static UINT32 omci_msg_get_frag_count(void *mdmObj, UINT16 meClass)
{
    _MdmObjParamInfo paramInfo;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 paramNbr = 0;
    UINT32 paramSize;
    UINT32 msgSize = 0;
    UINT32 fragCount = 1;
    UINT16 BitPosition = 0x8000; //bit position starts on attribute 1
    UINT16 AttributeList;
    omciMsgResult getMeInfoRes;
    omciMeInfo_t meInfo;

    getMeInfoRes = omci_msg_get_meInfo(meClass, &meInfo);
    if (getMeInfoRes == OMCI_MSG_RESULT_SUCCESS)
    {
        AttributeList = meInfo.MibUploadAttributeList;
    }
    else
    {
        return 0;
    }
    /* get starting attribute position */
    paramNbr = omci_msg_get_next_attribute_position( AttributeList, &BitPosition);

    if (paramNbr > 0)
    {
        do
        {
            /* copy requested attributes */
            if((ret = _cmsObj_getNthParam(mdmObj, paramNbr, &paramInfo)) != CMSRET_SUCCESS)
            {
                cmsLog_error("get of parameter %d from object id %d failed, ret=%d",
                         paramNbr, OMCID_GET_MDM_OBJ_OID(mdmObj), ret);
                break;
            }

            paramSize = omciUtl_getParamSize(paramInfo.type, paramInfo.maxVal);

            msgSize += paramSize;

            if(msgSize > (OMCI_PACKET_A_MSG_SIZE - OMCI_MIB_UPLOAD_OVERHEAD))
            {
                fragCount++;

                msgSize = paramSize;
            }

            paramNbr = omci_msg_get_next_attribute_position(AttributeList, &BitPosition);

        }
        while(paramNbr > 0 && paramNbr < paramInfo.totalParams);
    }

    return (ret == CMSRET_SUCCESS) ? fragCount : 0;
}

static CmsRet omci_msg_copy_to_packet(omciPacket *pPacket, UINT16 msgIndex, void *mdmObj, UINT16 *pAttrMask, UINT16 meClass)
{
    UINT32 attrNbr;
    UINT32 attrSize;
    UINT16 attrMaskSel = 0x8000;
    UINT16 attrMask = *pAttrMask;
    UINT16 attrMaskBackup = attrMask;
    _MdmObjParamInfo paramInfo;
    CmsRet ret = CMSRET_SUCCESS;
    UINT32 i;

    SHORT_NOTICE("attrMask = 0x%04X", attrMask);

    /* The Managed Entity identifier, which is an attribute of each managed
    entity, has no corresponding bit in the attribute mask. Thus, the
    attributes are counted starting from the first attribute after the
    Managed Entity identifier. (G.988) */
    attrNbr = 1;

    /* copy requested attributes */
    while(attrMask && (ret == CMSRET_SUCCESS))
    {
        /* process if current attribute is selected */
        if (attrMask & attrMaskSel)
        {
            // special handling for table attributes for Get, and exclude for MibUploadNext
            if (meClass < OMCI_ME_CLASS_MAX &&
                isAttributeTable(meClass, (attrMask & attrMaskSel)) == TRUE )
            {
                if (OMCI_MSG_TYPE_GET == OMCI_PACKET_MT(pPacket))
                {
                    void *obj = NULL;
                    void *mdmObject = NULL;
                    UINT16 objSize = 0;
                    UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
                    omciMeInfo_t meInfo;
                    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;

                    /* use omci_msg_get_mdmObj to retrieve iidStack.
                     * We only want the iidStack, so we can free the mdmObj
                     * immediately after return.
                     */
                    omci_msg_get_mdmObj(meClass, meInst, &meInfo, &mdmObject, &iidStack);
                    _cmsObj_free(&mdmObject);

                    // get the whole table object to obj
                    if (omciMeInfo[meClass].custom_get != NULL)
                    {
                        meInfo.custom_get(&obj, &objSize, &iidStack, (attrMask & attrMaskSel));
                        CMSMEM_FREE_BUF_AND_NULL_PTR(obj);
                    }
                    else
                    {
                        objSize = 0;
                    }
                    attrSize = 4;
                    if((msgIndex + attrSize) > OMCI_PACKET_A_MSG_SIZE)
                    {
                        cmsLog_error("Object id %d is too big: mask 0x%04X, index %d, attr size %d",
                            OMCID_GET_MDM_OBJ_OID(mdmObj), attrMaskBackup, msgIndex, attrSize);
                        ret = CMSRET_RESOURCE_EXCEEDED;
                        break;
                    }
                    // return size of attribute table instead of table value
                    OMCI_PACKET_MSG(pPacket)[msgIndex] = 0;
                    OMCI_PACKET_MSG(pPacket)[msgIndex+1] = 0;
                    OMCI_HTONS(&OMCI_PACKET_MSG(pPacket)[msgIndex+2], objSize);
                    msgIndex += attrSize;
                }
            }
            else //not a table attribute)
            {
                if((ret = _cmsObj_getNthParam(mdmObj, attrNbr, &paramInfo)) != CMSRET_SUCCESS)
                {
                    cmsLog_error("get of parameter %d from object id %d failed, ret=%d",attrNbr, OMCID_GET_MDM_OBJ_OID(mdmObj), ret);
                    break;
                }

                /* get attribute size */
                attrSize = omciUtl_getParamSize(paramInfo.type, paramInfo.maxVal);
#if 1
                SHORT_NOTICE("attrNbr %d: name %s, msgIndex %d, attrSize %d, mask 0x%04X",
                    attrNbr, paramInfo.name, msgIndex, attrSize, attrMask);
#endif
                if((msgIndex + attrSize) > OMCI_PACKET_A_MSG_SIZE)
                {
                    cmsLog_notice("Object id %d is too big: mask 0x%04X, index %d, attr size %d",
                        OMCID_GET_MDM_OBJ_OID(mdmObj), attrMaskBackup, msgIndex, attrSize);

                    ret = CMSRET_RESOURCE_EXCEEDED;

                    break;
                }

                switch(paramInfo.type)
                {
                case _MPT_STRING:
                case _MPT_DATE_TIME:
                case _MPT_BASE64:
                    if (attrSize == 0)
                    {
                        cmsLog_error("invalid string size, object id %d, size %d",
                            OMCID_GET_MDM_OBJ_OID(mdmObj), attrSize);
                        ret = CMSRET_INTERNAL_ERROR;
                    }
                    else
                    {
                        char *buf = NULL;

                        memcpy(&OMCI_PACKET_MSG(pPacket)[msgIndex], (UINT8 *)paramInfo.val, attrSize);

                        /* below here is debugging only */
                        buf = cmsMem_alloc(attrSize+1, ALLOC_ZEROIZE);
                        if (buf == NULL)
                        {
                            cmsLog_error("failed to allocate memory, size %d", attrSize);
                            ret = CMSRET_INTERNAL_ERROR;
                            break;
                        }
                        memcpy(buf, (UINT8 *)paramInfo.val, attrSize);
                        SHORT_NOTICE("           value: '%s'", buf);
                        cmsMem_free(buf);
                    }
                    break;

                case _MPT_HEX_BINARY:
                    if (attrSize == 0)
                    {
                        cmsLog_error("invalid string size, object id %d, size %d",
                            OMCID_GET_MDM_OBJ_OID(mdmObj), attrSize);
                        ret = CMSRET_INTERNAL_ERROR;
                    }
                    else
                    {
                        UINT8 *buf = NULL;
                        UINT32 bufSize = 0;

                        ret = cmsUtl_hexStringToBinaryBuf((char *)paramInfo.val, &buf, &bufSize);
                        if (ret != CMSRET_SUCCESS)
                        {
                            cmsLog_error("cmsUtl_hexStringToBinaryBuf() return error %d", ret);
                            break;
                        }
                        memcpy(&OMCI_PACKET_MSG(pPacket)[msgIndex], buf, bufSize);
                        SHORT_NOTICE("           value: '%s'", buf);
                        cmsMem_free(buf);
                        break;
                    }

                case _MPT_INTEGER:
                    {
                        SINT32 value = *(SINT32 *)(paramInfo.val);

                        for (i = 0; i<attrSize; i++)
                        {
                            OMCI_PACKET_MSG(pPacket)[msgIndex+i] =
                                (UINT8)((value >> (8*(attrSize-1-i))) & 0xFF);
                        }

                        SHORT_NOTICE("           value: %d (0x%08X)",
                            *(SINT32*)paramInfo.val, *(SINT32*)paramInfo.val);
                        break;
                    }

                case _MPT_UNSIGNED_INTEGER:
                    {
                        UINT32 value = *(UINT32 *)(paramInfo.val);
                        for(i=0; i<attrSize; i++)
                        {
                            OMCI_PACKET_MSG(pPacket)[msgIndex+i] =
                                (UINT8)((value >> (8*(attrSize-1-i))) & 0xFF);
                        }

                        SHORT_NOTICE("           value: %u (0x%08X)",
                            *(UINT32*)paramInfo.val, *(UINT32*)paramInfo.val);

                        break;
                    }

                case _MPT_BOOLEAN:
                    {
                        UINT8 value = *(UINT8 *)(paramInfo.val);
                        OMCI_PACKET_MSG(pPacket)[msgIndex] = value;

                        SHORT_NOTICE("           value: %u", *(UINT8*)paramInfo.val);

                        break;
                    }

                case _MPT_LONG64:
                case _MPT_UNSIGNED_LONG64:
                    {
                        UINT64 value = *((UINT64*)paramInfo.val);

                        for(i=0; i<attrSize; i++)
                        {
                            OMCI_PACKET_MSG(pPacket)[msgIndex+i] =
                                (UINT8)((value >> (8*(attrSize-1-i))) & 0xFF);
                        }

                        SHORT_NOTICE("           value: %llu", *(UINT64*)paramInfo.val);

                        break;
                    }
                default:
                    {
                        cmsLog_error("invalid parameter type, %s %d",
                            paramInfo.name, paramInfo.type);
                        ret = CMSRET_INTERNAL_ERROR;
                        break;
                    }

                } /* end of switch on paramInfo.type */

                msgIndex += attrSize;


            } // end of table attrib verses not a table attrib

        }// end if (attrMask & attrMaskSel)

        attrNbr++;

        /*
                clear attrib mask bit for uploaded param
              */
        attrMask &= ~attrMaskSel;
        attrMaskSel >>= 1;

    } /* while */

    omci_msg_padding(pPacket, msgIndex);

    /* return attribute mask, so the ME can continue on the next packet if possible */
    *pAttrMask = attrMask;

    return ret;
}

static CmsRet omci_msg_copy_from_packet(omciPacket *pPacket, UINT16 msgIndex,
  void *mdmObj, UINT16 attrMask, UINT16 setTableBufSize)
{
    _MdmObjParamInfo paramInfo;
    MdmObjectId mdmOid = *((MdmObjectId *) mdmObj);
    UINT16 attrMaskBackup = attrMask;
    UINT32 attrNbr;
    UINT32 attrSize;
    UINT32 val32;
    UINT64 val64;
    void *pSetVal = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    /* The Managed Entity identifier, which is an attribute of each managed
       entity, has no corresponding bit in the attribute mask. Thus, the
       attributes are counted starting from the first attribute after the
       Managed Entity identifier. */
    attrNbr = 1;

    /* copy requested attributes */
    while(attrMask && (ret == CMSRET_SUCCESS))
    {
        if(attrMask & 0x8000)
        {
            if((ret = _cmsObj_getNthParam(mdmObj, attrNbr, &paramInfo)) != CMSRET_SUCCESS)
            {
                cmsLog_error("get of parameter %d from object id %d failed, ret=%d",
                             attrNbr, mdmOid, ret);
                break;
            }

            /* get attribute size */
            attrSize = omciUtl_getParamSize(paramInfo.type, paramInfo.maxVal);

            if((msgIndex + attrSize) > OMCI_PACKET_A_MSG_SIZE)
            {
                cmsLog_error("attribute mask is invalid, object id %d mask %d attr size %d",
                             mdmOid, attrMaskBackup, attrSize);
                ret = CMSRET_RESOURCE_EXCEEDED;
                break;
            }
#if 1
            SHORT_NOTICE("name *%s* : attrNbr %d/%d, attrSize %d, type %d, minVal %d maxVal %d",
                         paramInfo.name, attrNbr, paramInfo.totalParams, attrSize,
                         (UINT32)paramInfo.type, paramInfo.minVal, paramInfo.maxVal);
#endif

            switch(paramInfo.type)
            {
                case _MPT_STRING:
                case _MPT_DATE_TIME:
                case _MPT_BASE64:
                    {
                        if(attrSize == 0)
                        {
                            cmsLog_error("invalid attribute size, object id %d, size %d",
                                         mdmOid, attrSize);
                            ret = CMSRET_INTERNAL_ERROR;
                            break;
                        }

                        /* allocate memory for the new value, add 1 to the size
                           to account for the NULL character at the end of the
                           string */
                        pSetVal = cmsMem_alloc(attrSize+1, ALLOC_ZEROIZE);
                        if(pSetVal == NULL)
                        {
                            cmsLog_error("failed to allocate memory, size %d", attrSize);
                            ret = CMSRET_INTERNAL_ERROR;
                            break;
                        }
                        memcpy(pSetVal, &OMCI_PACKET_MSG(pPacket)[msgIndex], attrSize);
                        SHORT_NOTICE("curr: '%s', new: '%s'",
                                     (char *)paramInfo.val, (char *)pSetVal);
                        break;
                    }

                case _MPT_HEX_BINARY:
                    {
                        char *hexStr = NULL;
                        UINT32 attrCopySize;

                        if(attrSize == 0)
                        {
                            cmsLog_error("invalid attribute size, object id %d, size %d",
                                         mdmOid, attrSize);
                            ret = CMSRET_INTERNAL_ERROR;
                            break;
                        }

                        attrCopySize = (setTableBufSize == 0) ? \
                          attrSize : (UINT32)setTableBufSize;
                        ret = cmsUtl_binaryBufToHexString(
                          &OMCI_PACKET_MSG(pPacket)[msgIndex], attrCopySize, &hexStr);
                        if(ret != CMSRET_SUCCESS)
                        {
                            cmsLog_error("cmsUtl_binaryBufToHexString() returns error %d", ret);
                            cmsMem_free(hexStr);
                            break;
                        }
                        pSetVal = (void *)hexStr;
                        SHORT_NOTICE("curr: '%s', new: '%s'",
                                     (char *)paramInfo.val, (char *)pSetVal);
                        break;
                    }

                case _MPT_INTEGER:
                case _MPT_UNSIGNED_INTEGER:
                    {
                        UINT32 i;
                        val32 = 0;
                        for(i=0; i<attrSize; i++)
                        {
                            val32 |= (OMCI_PACKET_MSG(pPacket)[msgIndex+i] << (8*(attrSize-1-i)));
                        }

                        SHORT_NOTICE("curr: %u (0x%08X), new: %u (0x%08X)",
                                     *(UINT32*)paramInfo.val, *(UINT32*)paramInfo.val, val32, val32);

                        pSetVal = &val32;
                        break;
                    }

                case _MPT_LONG64:
                case _MPT_UNSIGNED_LONG64:
                    {
                        UINT32 i;
                        val64 = 0;
                        for(i=0; i<attrSize; i++)
                        {
                            val64 |= (OMCI_PACKET_MSG(pPacket)[msgIndex+i] << (8*(attrSize-1-i)));
                        }

                        SHORT_NOTICE("curr: %u (0x%08X), new: %llu (0x%08llX)",
                                     *(UINT32*)paramInfo.val, *(UINT32*)paramInfo.val, val64, val64);

                        pSetVal = &val64;
                        break;
                    }
                case _MPT_BOOLEAN:
                    {
                        pSetVal = &OMCI_PACKET_MSG(pPacket)[msgIndex];

                        SHORT_NOTICE("curr: %u, new: %u",
                                     *(UINT8*)paramInfo.val, *(UINT8*)pSetVal);

                        break;
                    }

                default:
                    {
                        cmsLog_error("invalid parameter type, %s %d",
                                     paramInfo.name, paramInfo.type);
                        ret = CMSRET_INTERNAL_ERROR;
                        break;
                    }

            } /* end of switch on paramInfo.type */

            if(ret == CMSRET_SUCCESS)
            {
                /* _cmsObj_setNthParam() applies cmsMem_free to the old
                 * parameter pointer that points to paramInfo.val
                 * then sets this parameter pointer to the new pSetVal so
                 * both paramInfo.val and pSetVal should not be free in this
                 * function if _cmsObj_setNthParam returns CMSRET_SUCCESS
                 */
                if((ret = _cmsObj_setNthParam(mdmObj, attrNbr, pSetVal)) != CMSRET_SUCCESS)
                {
                    cmsLog_error("Internal error, ret=%d", ret);
                    // clean up if error before break out of while loop
                    if ((pSetVal != NULL) &&
                        (paramInfo.type == _MPT_STRING ||
                         paramInfo.type == _MPT_DATE_TIME ||
                         paramInfo.type == _MPT_BASE64 ||
                         paramInfo.type == _MPT_HEX_BINARY))
                    {
                        cmsMem_free(pSetVal);
                    }
                    break;
                }
            }

            msgIndex += attrSize;
        }

        attrNbr++;
        attrMask <<= 1;
    } /* while */

    return ret;
}


/******************************************************************************
 * OMCI Command Handlers
 ******************************************************************************/
static int omci_msg_handler_InvalidCmd(omciPacket *pPacket, int len __attribute__((unused)))
{
    int rv;

    cmsLog_notice("*** Invalid Message Type: %d ***\n\n", OMCI_PACKET_MT(pPacket));

    OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_NOT_SUPPORTED;
    if((rv=omci_msg_padding(pPacket, 1)))
    {
        return rv;
    }

    return omci_msg_send_response(pPacket);
}

extern const char const *omciMsgName[OMCI_PACKET_MT_MAX];

static int omci_msg_handler_NotSupported(omciPacket *pPacket, int len __attribute__((unused)))
{
    int rv;
    cmsLog_notice("*** Message Type '%s' is not supported ***\n\n",
                  omciMsgName[OMCI_PACKET_MT(pPacket)]);

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_NOT_SUPPORTED;

    if((rv=omci_msg_padding(pPacket, 1)))
    {
      return rv;
    }

    return omci_msg_send_response(pPacket);
}


static UBOOL8 isDataPathlAttrUpdatedBySet(UINT32 mdmOid, UINT16 attrMask __attribute__((unused)))
{
    UBOOL8 updated = FALSE;

    if ((mdmOid == MDMOID_MAPPER_SERVICE_PROFILE) ||
      (mdmOid == MDMOID_VLAN_TAGGING_FILTER_DATA) ||
      (mdmOid == MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) ||
      (mdmOid == MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA) ||
      (mdmOid == MDMOID_MAC_BRIDGE_SERVICE_PROFILE) ||
      (mdmOid == MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA) ||
      (mdmOid == MDMOID_T_CONT) ||
      (mdmOid == MDMOID_GEM_PORT_NETWORK_CTP) ||
      (mdmOid == MDMOID_MULTICAST_GEM_INTERWORKING_TP) ||
      (mdmOid == MDMOID_GEM_INTERWORKING_TP) ||
      (mdmOid == MDMOID_MULTICAST_SUBSCRIBER_CONFIG_INFO) ||
      (mdmOid == MDMOID_MULTICAST_OPERATIONS_PROFILE) ||
      (mdmOid == MDMOID_PRIORITY_QUEUE_G) ||
      (mdmOid == MDMOID_TRAFFIC_SCHEDULER_G) ||
      (mdmOid == MDMOID_GEM_TRAFFIC_DESCRIPTOR) ||
      (mdmOid == MDMOID_PPTP_ETHERNET_UNI) ||
      (mdmOid == MDMOID_IP_HOST_CONFIG_DATA) ||
      (mdmOid == MDMOID_PPTP_MOCA_UNI) ||
      (mdmOid == MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT) ||
      (mdmOid == MDMOID_IPV6_HOST_CONFIG_DATA))
    {
        updated = TRUE;
    }

    return updated;
}

static int omci_msg_handler_Create(omciPacket *pPacket, int len __attribute__((unused)))
{
    omciMeInfo_t meInfo;
    void *mdmObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
    UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
    omciMsgResult msgResult;
    CmsRet ret;
    CmsRet ret2;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);

        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    // Schedule model to handle possible change of configuration
    scheduleHandleModelPath(FALSE);

    msgResult = omci_msg_get_mdmObj(meClass, meInst, &meInfo, &mdmObj, &iidStack);
    if(msgResult == OMCI_MSG_RESULT_SUCCESS)
    {
        /* instance already exists */
        cmsLog_notice("*** Creating existing Managed Entity '%s' ***\n",
                      meInfo.name);

        _cmsLck_releaseLock();

        _cmsObj_free(&mdmObj);  // it is OK to call this outside of CMS lock

        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_ME_INST_EXISTS;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }
    else if(msgResult == OMCI_MSG_RESULT_UNKNOWN_ME_INST)
    {
        /* create new mdm object instance */
        cmsLog_notice("*** Create ME: '%s' ***\n", meInfo.name);

        ret = omci_msg_create_mdmObj(meInst, &meInfo, &mdmObj, &iidStack);
    }
    else
    {
        _cmsLck_releaseLock();

        if(OMCID_PROMISC_MODE())
        {
            cmsLog_notice("*** Promiscuous mode: Unknown ME %d ***\n", meClass);
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
        }
        else
        {
            /* send response with error code */
            OMCI_PACKET_MSG(pPacket)[0] = msgResult;
        }
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    /* if we reach this point, mdmObj points to the object being processed, so
       we can now set the remaining 'set-by-create' parameters */

    /* It should be noted that the message contents for the "Create" message
       apply only to attributes that are "Set-by-create". Thus, the first byte
       of Message Contents field begins with the attribute value for the first
       Set-by-create attribute and so forth. (G.988) */
    if(ret == CMSRET_SUCCESS)
    {
        ret = omci_msg_copy_from_packet(pPacket, OMCI_CREATE_OVERHEAD, mdmObj,
          meInfo.setByCreateMask, 0);
        if(ret != CMSRET_SUCCESS)
        {
            if(OMCID_PROMISC_MODE())
            {
                cmsLog_notice("*** Promiscuous mode: Unknown ME Attribute (meClass %d) ***\n",
                              meClass);
                ret = CMSRET_SUCCESS;
            }
            else
            {
                cmsLog_error("failed to set parameters of object id %d, ret=%d",
                             meInfo.mdmOid, ret);
            }
            //remove instance
            ret2 = _cmsObj_deleteInstance(meInfo.mdmOid, &iidStack);
            if (ret2 != CMSRET_SUCCESS)
                cmsLog_error("could not delete instance of object id %d, ret2=%d", meInfo.mdmOid, ret2);
        }
        else
        {
            /* write object back to the MDM database */
            if ((ret = _cmsObj_set(mdmObj, &iidStack)) != CMSRET_SUCCESS)
            {
                if(OMCID_PROMISC_MODE())
                {
                    cmsLog_notice("*** Promiscuous mode: meClass %d, set of object id %d (0x%08X, 0x%08X) failed, ret=%d ***",
                                 meClass, meInfo.mdmOid, (UINT32)mdmObj, (UINT32)&iidStack, ret);
                    ret = CMSRET_SUCCESS;
                }
                else
                {
                    cmsLog_error("Set of object id %d (0x%08X, 0x%08X) failed, ret=%d",
                                 meInfo.mdmOid, (UINT32)mdmObj, (UINT32)&iidStack, ret);
                }
                //remove instance
                ret2 = _cmsObj_deleteInstance(meInfo.mdmOid, &iidStack);
                if (ret2 != CMSRET_SUCCESS)
                    cmsLog_error("could not delete instance of object id %d, ret2=%d", meInfo.mdmOid, ret2);
            }
            else
            {
                /* debugging only */
                OMCID_DUMP_OBJECT(mdmObj);
            }
        }
    }

    _cmsObj_free(&mdmObj);

    _cmsLck_releaseLock();

    /* send response message */
    OMCI_PACKET_MSG(pPacket)[0] = (ret == CMSRET_SUCCESS) ? OMCI_MSG_RESULT_SUCCESS : OMCI_MSG_RESULT_PROC_ERROR;
    omci_msg_padding(pPacket, 1);
    omci_msg_send_response(pPacket);
    return OMCI_PACKET_MSG(pPacket)[0];
}

static int omci_msg_handler_Delete(omciPacket *pPacket, int len __attribute__((unused)))
{
    omciMeInfo_t meInfo;
    void *mdmObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
    UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
    omciMsgResult msgResult = OMCI_MSG_RESULT_SUCCESS;
    CmsRet ret = CMSRET_SUCCESS;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);

        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    msgResult = omci_msg_get_mdmObj(meClass, meInst, &meInfo, &mdmObj, &iidStack);
    if(msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        _cmsLck_releaseLock();

        if(OMCID_PROMISC_MODE())
        {
            cmsLog_notice("*** Promiscuous mode: Unknown ME class %d, inst %d ***\n", meClass, meInst);
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
        }
        else
        {
            /* send response with error code */
            OMCI_PACKET_MSG(pPacket)[0] = msgResult;
        }

        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    // can only delete instance of dynamic object
    if ((meInfo.flags & OMCI_ME_FLAG_DYNAMIC) == 0)
    {
        _cmsLck_releaseLock();
        _cmsObj_free(&mdmObj);

        cmsLog_error("Cannot delete instance of static object");

        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    // delete instance of the found object
    cmsLog_notice("*** Delete ME: '%s' ***\n", meInfo.name);
    ret = _cmsObj_deleteInstance(meInfo.mdmOid, &iidStack);

    if (ret == CMSRET_SUCCESS)
    {
        // Schedule model to handle possible change of configuration
        scheduleHandleModelPath(FALSE);

        /* debugging only */
        OMCID_DUMP_OBJECT(mdmObj);
    }

    _cmsObj_free(&mdmObj);

    _cmsLck_releaseLock();

    /* send response message */
    OMCI_PACKET_MSG(pPacket)[0] = (ret == CMSRET_SUCCESS) ? OMCI_MSG_RESULT_SUCCESS : OMCI_MSG_RESULT_PARM_ERROR;
    omci_msg_padding(pPacket, 1);
    omci_msg_send_response(pPacket);

    return OMCI_PACKET_MSG(pPacket)[0];
}

static int omci_msg_handler_Set(omciPacket *pPacket, int len __attribute__((unused)))
{
    omciMeInfo_t meInfo;
    void *mdmObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
    UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
    UINT16 attrMask = OMCI_MSG_GET_ATTR_MASK(pPacket);
    omciMsgResult msgResult = OMCI_MSG_RESULT_SUCCESS;
    CmsRet ret;
    UINT16 unsupportMask = 0;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        msgResult = OMCI_MSG_RESULT_PROC_ERROR;
        goto out;
    }

    msgResult = omci_msg_get_mdmObj(meClass, meInst, &meInfo, &mdmObj, &iidStack);
    if (msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        cmsLog_error("omci_msg_get_mdmObj() failed, ME class %d, inst %d, ret=%d",
          meClass, meInst, msgResult);
        _cmsLck_releaseLock();
        goto out;
    }

    if (attrMask & meInfo.roAttrMask)
    {
        cmsLog_error("Set operation on RO attribute, ME class %d, inst %d, "
          "attrMask=0x%04X",
          meClass, meInst, attrMask);
        msgResult = OMCI_MSG_RESULT_PARM_ERROR;
        _cmsObj_free(&mdmObj);
        _cmsLck_releaseLock();
        goto out;
    }

    // Schedule model to handle possible change of configuration
    if (isDataPathlAttrUpdatedBySet(meInfo.mdmOid, attrMask) == TRUE)
    {
        scheduleHandleModelPath(FALSE);
    }

    cmsLog_notice("*** Set ME: '%s' ***\n", meInfo.name);

    // check supported attribute mask
    if (!OMCID_PROMISC_MODE())
    {
        unsupportMask = attrMask & (~meInfo.supportedMask);
    }
    // support all attributes of attrMask
    if (!unsupportMask)
    {
        ret = omci_msg_copy_from_packet(pPacket, OMCI_SET_OVERHEAD,
          mdmObj, attrMask, 0);
        if (ret == CMSRET_SUCCESS)
        {
            /* write object back to the MDM database */
            if ((ret = _cmsObj_set(mdmObj, &iidStack)) != CMSRET_SUCCESS)
            {
                cmsLog_error("set of object id %d (0x%08X, 0x%08X) failed, ret=%d",
                             meInfo.mdmOid, (UINT32)mdmObj, (UINT32)&iidStack, ret);
            }
            else
            {
                /* if management entity has customized set function then use it */
                if (meInfo.custom_set != NULL)
                {
                    meInfo.custom_set(mdmObj, &iidStack, attrMask);
                }
                /* debugging only */
                OMCID_DUMP_OBJECT(mdmObj);
            }
        }
        else
        {
            cmsLog_error("omci_msg_copy_from_packet() failed, "
              "ME class %d, inst %d\n", meClass, meInst);
        }

        msgResult = (ret == CMSRET_SUCCESS) ?
          OMCI_MSG_RESULT_SUCCESS : OMCI_MSG_RESULT_PARM_ERROR;
    }

    _cmsObj_free(&mdmObj);

    _cmsLck_releaseLock();

out:
    /* send response message */
    if (unsupportMask)
    {
       cmsLog_error("Set ME failed(attr failed or unknown): meClass=%d "
         "attrMask=0x%04X supportMask=0x%04X optionMask=0x%04X unsupportMask=0x%04X",
         meClass, attrMask, meInfo.supportedMask, meInfo.optionalMask, unsupportMask);

       OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_ATTR_FAILED;
       // all unsupported attributes are optional
       if ((unsupportMask & (~meInfo.optionalMask)) == 0 )
       {
           //set optional attribute mask
           OMCI_MSG_SET_ATTR_MASK(pPacket, 1, unsupportMask);
           omci_msg_padding(pPacket, 3);
       }
       else
       {
           //set optional attribute mask as 0
           OMCI_PACKET_MSG(pPacket)[1] = 0;
           OMCI_PACKET_MSG(pPacket)[2] = 0;
           //set execution attribute mask
           OMCI_MSG_SET_ATTR_MASK(pPacket, 3, unsupportMask);
           omci_msg_padding(pPacket, 5);
       }
    }
    else
    {
        if (OMCID_PROMISC_MODE())
        {
            cmsLog_notice("*** Promiscuous mode: ME class %d, inst %d ***\n",
              meClass, meInst);
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
        }
        else
        {
            /* send response with error code */
            OMCI_PACKET_MSG(pPacket)[0] = msgResult;
        }
        omci_msg_padding(pPacket, 1);
    }

    omci_msg_send_response(pPacket);

    return OMCI_PACKET_MSG(pPacket)[0];
}

static int omci_msg_handler_Get(omciPacket *pPacket, int len __attribute__((unused)))
{
    omciMeInfo_t meInfo;
    void *mdmObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
    UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
    UINT16 attrMask = OMCI_MSG_GET_ATTR_MASK(pPacket);
    UINT16 unsupportMask = 0;
    omciMsgResult msgResult;
    CmsRet ret = CMSRET_SUCCESS;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    /* copy attribute mask to the response packet */
    OMCI_PACKET_MSG(pPacket)[2] = OMCI_PACKET_MSG(pPacket)[1];
    OMCI_PACKET_MSG(pPacket)[1] = OMCI_PACKET_MSG(pPacket)[0];

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);

        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    msgResult = omci_msg_get_mdmObj(meClass, meInst, &meInfo, &mdmObj, &iidStack);
    if(msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        if (OMCID_PROMISC_MODE())
        {
            cmsLog_notice("*** Promiscuous mode: Unknown ME class %d, inst %d ***\n", meClass, meInst);
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
            omci_msg_padding(pPacket, 3);
        }
        else
        {
            /* send response, indicating the cause of the error */
            OMCI_PACKET_MSG(pPacket)[0] = msgResult;
            omci_msg_padding(pPacket, 1);
        }

        goto out;
    }

    cmsLog_notice("*** Get ME: '%s' ***\n", meInfo.name);

    // check supported attribute mask
    unsupportMask = attrMask & (~meInfo.supportedMask);
    if (OMCID_PROMISC_MODE() && (unsupportMask != 0))
    {
        cmsLog_notice("ME masks: attr=0x%04X, support=0x%04X, option=0x%04X, unsupport=0x%04X",
          attrMask, meInfo.supportedMask, meInfo.optionalMask, unsupportMask);
        unsupportMask = 0;
    }

    if (!unsupportMask)
    {
        ret = omci_msg_copy_to_packet(pPacket, OMCI_GET_OVERHEAD, mdmObj, &attrMask, meClass);

        _cmsObj_free(&mdmObj);

        /* set result, add padding */
        if(ret == CMSRET_SUCCESS)
        {
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
        }
        else
        {
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
            omci_msg_padding(pPacket, 1);
        }
    }
    else
    {
        cmsLog_error("Get ME failed(attr failed or unknown): meClass=%d attrMask=0x%04X supportMask=0x%04X optionMask=0x%04X unsupportMask=0x%04X",
                     meClass, attrMask, meInfo.supportedMask, meInfo.optionalMask, unsupportMask);

        _cmsObj_free(&mdmObj);

        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_ATTR_FAILED;
        omci_msg_padding(pPacket, 1);
        // all unsupported attributes are optional
        if ((unsupportMask & (~meInfo.optionalMask)) == 0 )
        {
            //set optional attribute mask
            OMCI_MSG_SET_ATTR_MASK(pPacket, 28, unsupportMask);
        }
        else
        {
            //set execution attribute mask
            OMCI_MSG_SET_ATTR_MASK(pPacket, 30, unsupportMask);
        }
    }

out:
    _cmsLck_releaseLock();

    /* send response message */
    omci_msg_send_response(pPacket);

    return OMCI_PACKET_MSG(pPacket)[0];
}


static int omci_msg_handler_Get_Current(omciPacket *pPacket, int len __attribute__((unused)))
{
    omciMeInfo_t meInfo;
    void *mdmObj;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
    UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
    UINT16 attrMask = OMCI_MSG_GET_ATTR_MASK(pPacket);
    UINT16 unsupportMask = 0;
    omciMsgResult msgResult;
    OmciSystemObject *obj = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    /* copy attribute mask to the response packet */
    OMCI_PACKET_MSG(pPacket)[2] = OMCI_PACKET_MSG(pPacket)[1];
    OMCI_PACKET_MSG(pPacket)[1] = OMCI_PACKET_MSG(pPacket)[0];

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);

        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    if ((ret = _cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void *) &obj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get OmciSystemObject, ret=%d", ret);

        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
        goto out;
    }

    // set GetPmMode to get current PM counters
    // before calling omci_msg_get_mdmObj
    obj->getPmMode = OMCI_PM_GET_CURRENT;
    _cmsObj_set(obj, &iidStack);
    _cmsObj_free((void **) &obj);

    INIT_INSTANCE_ID_STACK(&iidStack);

    msgResult = omci_msg_get_mdmObj(meClass, meInst, &meInfo, &mdmObj, &iidStack);
    if(msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        if(OMCID_PROMISC_MODE())
        {
            cmsLog_notice("*** Promiscuous mode: Unknown ME class %d, inst %d ***\n", meClass, meInst);
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
            omci_msg_padding(pPacket, 3);
        }
        else
        {
            /* send response, indicating the cause of the error */
            OMCI_PACKET_MSG(pPacket)[0] = msgResult;
            omci_msg_padding(pPacket, 1);
        }
        goto out;
    }

    cmsLog_notice("*** Get ME: '%s' ***\n", meInfo.name);

    // check supported attribute mask
    unsupportMask = attrMask & (~meInfo.supportedMask);
    if (OMCID_PROMISC_MODE() && (unsupportMask != 0))
    {
        cmsLog_notice("ME masks: attr=0x%04X, support=0x%04X, option=0x%04X, unsupport=0x%04X",
          attrMask, meInfo.supportedMask, meInfo.optionalMask, unsupportMask);
        unsupportMask = 0;
    }

    if (!unsupportMask)
    {
        ret = omci_msg_copy_to_packet(pPacket, OMCI_GET_OVERHEAD, mdmObj, &attrMask, meClass);

        _cmsObj_free(&mdmObj);

        /* set result, add padding */
        if(ret == CMSRET_SUCCESS)
        {
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
        }
        else
        {
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
            omci_msg_padding(pPacket, 1);
        }
    }
    else
    {
        cmsLog_error("Get ME failed(attr failed or unknown): meClass=%d attrMask=0x%04X supportMask=0x%04X optionMask=0x%04X unsupportMask=0x%04X",
                     meClass, attrMask, meInfo.supportedMask, meInfo.optionalMask, unsupportMask);

        _cmsObj_free(&mdmObj);

        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_ATTR_FAILED;
        omci_msg_padding(pPacket, 1);
        // all unsupported attributes are optional
        if ((unsupportMask & (~meInfo.optionalMask)) == 0 )
        {
            //set optional attribute mask
            OMCI_MSG_SET_ATTR_MASK(pPacket, 28, unsupportMask);
        }
        else
        {
            //set execution attribute mask
            OMCI_MSG_SET_ATTR_MASK(pPacket, 30, unsupportMask);
        }
    }

out:
    INIT_INSTANCE_ID_STACK(&iidStack);
    if (_cmsObj_get(MDMOID_OMCI_SYSTEM, &iidStack, 0, (void *) &obj) == CMSRET_SUCCESS)
    {
        // set GetPmMode to get 15 minutes PM counters
        obj->getPmMode = OMCI_PM_GET_15_MINUTES;
        _cmsObj_set(obj, &iidStack);
        _cmsObj_free((void **) &obj);
    }

    _cmsLck_releaseLock();

    /* send response message */
    omci_msg_send_response(pPacket);

    return OMCI_PACKET_MSG(pPacket)[0];
}


static int omci_msg_handler_GetAllAlarms(omciPacket *pPacket, int len __attribute__((unused)))
{
    UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
    UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
    UINT16 alarmCount = 0;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    if ((meClass != 2) || (meInst != 0))
    {
        cmsLog_notice("ERROR: Invalid header: meClass=%d, meInst=%d ***\n",
                      OMCI_PACKET_ME_CLASS(pPacket), OMCI_PACKET_ME_INST(pPacket));

        /* send response, with all 0 in message */
        omci_msg_padding(pPacket, 0);
        omci_msg_send_response(pPacket);

        return OMCI_MSG_RESULT_UNKNOWN_ME;
    }

    omci_alarm_setSequenceNumber(INIT_ALARM_SEQ_VALUE);

    alarmCount = omci_alarm_getFrozenAll(OMCI_PACKET_MSG(pPacket)[0] & 0x01);

    cmsLog_notice("*** GetAllAlarms: number of alarms %d, arcFlag %d ***\n", alarmCount, OMCI_PACKET_MSG(pPacket)[0]);

    OMCI_PACKET_MSG(pPacket)[0] = (UINT8)((alarmCount >> 8) & 0xff);
    OMCI_PACKET_MSG(pPacket)[1] = (UINT8)(alarmCount & 0xff);

    omci_msg_padding(pPacket, 2);

    /* send response message */
    omci_msg_send_response(pPacket);

    return OMCI_PACKET_MSG(pPacket)[0];
}


static int omci_msg_handler_GetAllAlarmsNext(omciPacket *pPacket, int len __attribute__((unused)))
{
    UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
    UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
    UINT16 seqNumber = 0;
    BCM_OMCIPM_ALARM_ENTRY* pAlarm = NULL;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    if ((meClass != 2) || (meInst != 0))
    {
        cmsLog_notice("ERROR: Invalid header: meClass=%d, meInst=%d ***\n",
                      OMCI_PACKET_ME_CLASS(pPacket), OMCI_PACKET_ME_INST(pPacket));

        /* send response, with all 0 in message */
        omci_msg_padding(pPacket, 0);
        omci_msg_send_response(pPacket);

        return OMCI_MSG_RESULT_UNKNOWN_ME;
    }

    seqNumber = (OMCI_PACKET_MSG(pPacket)[0] << 8) | OMCI_PACKET_MSG(pPacket)[1];

    pAlarm = omci_alarm_getFrozen(seqNumber);

    if (pAlarm != NULL)
    {
        OmciMdmOidClassId_t info;

        memset(&info, 0, sizeof(OmciMdmOidClassId_t));
        info.mdmOid = pAlarm->objectType;
        omciUtl_oidToClassId(&info);

        cmsLog_notice("*** GetAllAlarmsNext: seqNumber %d, meClass %d, meInst %d, bitmap %d ***\n",
                     seqNumber, info.classId, pAlarm->objectID, pAlarm->alarmBitmap);

        /* setup response data */
        OMCI_PACKET_MSG(pPacket)[0] = (UINT8)((info.classId >> 8) & 0xff);
        OMCI_PACKET_MSG(pPacket)[1] = (UINT8)(info.classId & 0xff);
        OMCI_PACKET_MSG(pPacket)[2] = (UINT8)((pAlarm->objectID >> 8) & 0xff);
        OMCI_PACKET_MSG(pPacket)[3] = (UINT8)(pAlarm->objectID & 0xff);
        OMCI_PACKET_MSG(pPacket)[4] = (UINT8)((pAlarm->alarmBitmap >> 8) & 0xff);
        OMCI_PACKET_MSG(pPacket)[5] = (UINT8)(pAlarm->alarmBitmap & 0xff);

        omci_msg_padding(pPacket, 6);
    }
    else
    {
        cmsLog_notice("*** GetAllAlarmsNext: Cannot find alarm with sequence number %d ***\n",
                      seqNumber);

        /* padding with all 0 in message */
        omci_msg_padding(pPacket, 0);
    }

    /* send response message */
    omci_msg_send_response(pPacket);

    return OMCI_MSG_RESULT_SUCCESS;
}

static int omci_msg_handler_SetTable(omciPacket *pPacket,
  int len __attribute__((unused)))
{
    omciMeInfo_t meInfo;
    void *mdmObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
    UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
    UINT16 attrMask = OMCI_MSG_GET_ATTR_MASK(pPacket);
    omciMsgResult msgResult = OMCI_MSG_RESULT_SUCCESS;
    CmsRet ret = CMSRET_SUCCESS;
    UINT16 unsupportMask = 0;
    UINT16 setTableBufSize = 0;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        msgResult = OMCI_MSG_RESULT_PROC_ERROR;
        cmsLog_error("failed to get lock, ret=%d", ret);
        goto out2;
    }

    msgResult = omci_msg_get_mdmObj(meClass, meInst, &meInfo, &mdmObj,
      &iidStack);
    if (msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        msgResult = OMCI_MSG_RESULT_PROC_ERROR;
        cmsLog_error("omci_msg_get_mdmObj() failed, meClass=%d, meInst=%d",
          meClass, meInst);
        goto out1;
    }

    if (((attrMask & meInfo.tableAttrMask) != attrMask) || 
      (IS_ONE_BIT_SET(attrMask) != TRUE))
    {
        msgResult = OMCI_MSG_RESULT_PROC_ERROR;
        cmsLog_error("Set table only applies to one table attribute, "
          "ME:'%s', set mask 0x%08x, table mask 0x%08x", meInfo.name,
          attrMask, meInfo.tableAttrMask);
        goto out1;
    }

    setTableBufSize = OMCI_NTOHS(&pPacket->B.msgLen) - OMCI_SET_OVERHEAD;

    if (isDataPathlAttrUpdatedBySet(meInfo.mdmOid, attrMask) == TRUE)
    {
        scheduleHandleModelPath(FALSE);
    }

    cmsLog_notice("*** Set table ME: '%s' ***\n", meInfo.name);
    if (!OMCID_PROMISC_MODE())
    {
        unsupportMask = attrMask & (~meInfo.supportedMask);
    }

    ret = CMSRET_INTERNAL_ERROR;

    if (!unsupportMask)
    {
        ret = omci_msg_copy_from_packet(pPacket, OMCI_SET_OVERHEAD, mdmObj,
          attrMask, setTableBufSize);
        if (ret == CMSRET_SUCCESS)
        {
            if (meInfo.custom_set != NULL)
            {
                ret = meInfo.custom_set(mdmObj, &iidStack, attrMask);
            }
            OMCID_DUMP_OBJECT(mdmObj);
        }
    }

    if (ret == CMSRET_SUCCESS)
    {
        msgResult = OMCI_MSG_RESULT_SUCCESS;
    }
    else
    {
        msgResult = OMCI_MSG_RESULT_PROC_ERROR;
        cmsLog_error("Set table failed, ME '%s', set mask 0x%08x",
          meInfo.name, attrMask);
    }

    _cmsObj_free(&mdmObj);

out1:
    _cmsLck_releaseLock();
out2:
    if (msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        msgResult = OMCID_PROMISC_MODE() ?
          OMCI_MSG_RESULT_SUCCESS : msgResult;
    }
    OMCI_PACKET_MSG(pPacket)[0] = msgResult;
    omci_msg_padding(pPacket, 1);
    omci_msg_send_response(pPacket);
    return OMCI_PACKET_MSG(pPacket)[0];
}

static void omci_msg_mib_upload_clean(void)
{
    UINT32 count = 0;
    int i;

    /* free all pending objects */
    for(i=0; i<OMCI_MIB_UPLOAD_INST_MAX; ++i)
    {
        if(mibUpload_mibCopy[i].mdmObj)
        {
            _cmsObj_free(&mibUpload_mibCopy[i].mdmObj);

            mibUpload_mibCopy[i].mdmObj = NULL;

            count++;
        }
    }

    /* initialize MIB Upload variables */
    mibUpload_count = 0;
    mibUpload_seqNbr = 0;
    mibUpload_instIndex = 0;
    mibUpload_nextAttrMask = 0;
    mibUpload_fragCount = 1;

    cmsLog_notice("MIB Upload Cleanup: %d objects", count);
}

static int omci_msg_mib_upload_error(omciPacket *pPacket)
{
    /* cleanup MIB upload allocations */
    omci_msg_mib_upload_clean();

    /* send error response message */
    omci_msg_padding(pPacket, 0);
    return omci_msg_send_response(pPacket);
}

static omciMsgResult omci_msg_get_nbr_instances(UINT16 meClass,
                                                UINT32 *nbrOfInstances)
{
    CmsRet ret;
    omciMsgResult msgResult = OMCI_MSG_RESULT_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    omciMeInfo_t meInfo;
    UINT32 meInstCount;
    void *mdmObj;

    msgResult = omci_msg_get_meInfo(meClass, &meInfo);
    if (msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        return msgResult;
    }

    if(meInfo.flags & OMCI_ME_FLAG_PRESENT)
    {
        *nbrOfInstances = 1;
    }
    else
    {
        /* multiple mdm objects may have been created, search for the mdm object
           that matches the instance number */

        meInstCount = 0;

        while(1)
        {
            ret = _cmsObj_getNext(meInfo.mdmOid, &iidStack, &mdmObj);
            if (ret == CMSRET_NO_MORE_INSTANCES)
            {
                /* no more instances available */

                *nbrOfInstances = meInstCount;

                break;
            }
            else if(ret != CMSRET_SUCCESS)
            {
                cmsLog_error("_cmsObj_getNext %d of %s (oid %d) failed (class %d), ret=%d",
                             meInstCount, meInfo.name, meInfo.mdmOid, meClass, ret);

                msgResult = OMCI_MSG_RESULT_PROC_ERROR;

                break;
            }

            _cmsObj_free(&mdmObj);

            meInstCount++;
        }
    }

    if(msgResult == OMCI_MSG_RESULT_SUCCESS)
    {
        cmsLog_notice("%s -> Found %d instance(s)", meInfo.name, *nbrOfInstances);
    }

    return msgResult;
}

/******************************************************************************
 *  FUNCTION:       omci_msg_handler_MIBUpload
 *
 *  FILENAME:       omcid_msg.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:    the purpose of this routine is to process the OMCI MibUpload
 *                  msg from the OLT.  This routine does so by compiling a list
 *                  of Managed Entities to include in the upload, and a count of
 *                  the number of fragments that must be sent to complete the
 *                  MibUpload operation.  The OLT will receive this data as the
 *                  number of MibUploadNext msgs it must send to complete the
 *                  full Mib audit.
 *  RETURNS:
 *
******************************************************************************/
static int omci_msg_handler_MIBUpload(omciPacket *pPacket, int len __attribute__((unused)))
{
    omciMeInfo_t meInfo;
    InstanceIdStack iidStack;
    UINT16 meClass;
    UINT32 meInst;
    UINT16 meClassPkt;
    UINT16 meInstPkt;
    UINT32 instCount = 0;
    UINT32 instIndex = 0;
    omciMsgResult msgResult;
    CmsRet ret;

    /* XXX Need to implement MIB Upload timeout !!! */
    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    meClassPkt = OMCI_PACKET_ME_CLASS(pPacket);
    meInstPkt = OMCI_PACKET_ME_INST(pPacket);

    if((meClassPkt != 2) || (meInstPkt != 0))
    {
        cmsLog_notice("ERROR: Invalid header: meClass=%d, meInst=%d ***\n",
                      meClassPkt, meInstPkt);

        return 0;
    }

    if(mibUpload_count)
    {   /* last MIB Upload is still in progress */

        //Get current timestamp
        cmsTms_get(&MibUploadTime.Current);

        //Get Delta time
        cmsTms_delta(&MibUploadTime.Current, &MibUploadTime.Start, &MibUploadTime.Delta);

        //compare to start of mibupload to check for timeout
        if (MibUploadTime.Delta.sec >= 60)
        {
            // the previous mib upload has timed out so cleanup and allow a new one to start
            cmsLog_error("ERROR: MIB Upload time out detected while starting new MIB Upload");
            omci_msg_mib_upload_clean();
        }
        else
        {
            cmsLog_error("ERROR: Attempt to start MIB Upload while previous MIB Upload is still in progress");
            omci_msg_padding(pPacket, 0);
            return omci_msg_send_response(pPacket);
        }
    }

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);

        /* send response with ME count equal to 0 */
        return omci_msg_mib_upload_error(pPacket);
    }

    /* find ME instances in all ME classes that will be uploaded to the OLT */
    for(meClass=0; meClass<OMCI_ME_CLASS_MAX; ++meClass)
    {
        if(OMCI_ME_FLAG_MIB_UPLOAD & omciMeInfo[meClass].flags)
        {
            if(instIndex == OMCI_MIB_UPLOAD_INST_MAX)
            {
                cmsLog_error("Too many instances to upload (instIndex=%d)", instIndex);

                omci_msg_mib_upload_error(pPacket);

                return -1;
            }

            INIT_INSTANCE_ID_STACK(&iidStack);

            meInst = OMCI_GET_INSTANCE_BY_NBR;

            if(omciMeInfo[meClass].nbrInstancesUpload < -1)
            {
                cmsLog_error("%s : nbrInstancesUpload = %d",
                             omciMeInfo[meClass].name,
                             omciMeInfo[meClass].nbrInstancesUpload);

                _cmsLck_releaseLock();

                /* internal error */
                omci_msg_mib_upload_error(pPacket);

                return -1;
            }
            else if(omciMeInfo[meClass].nbrInstancesUpload == -1)
            {
                msgResult = omci_msg_get_nbr_instances(meClass, &instCount);
                if(msgResult != OMCI_MSG_RESULT_SUCCESS)
                {
                    _cmsLck_releaseLock();

                    /* send response with ME count equal to 0 */
                    return omci_msg_mib_upload_error(pPacket);
                }
            }
            else /* nbrInstancesUpload >= 0 */
            {
                instCount = omciMeInfo[meClass].nbrInstancesUpload;
            }

            if((instIndex + instCount ) >= OMCI_MIB_UPLOAD_INST_MAX)
            {
                cmsLog_error("Too many instances to upload (instIndex=%d)", instIndex);
                _cmsLck_releaseLock();
                omci_msg_mib_upload_error(pPacket);

                return -1;
            }

            while(instCount)
            {
                /* since we don't re-initialize the IId Stack each time we get
                                an instance of the object, we should always request instance 0 */
                msgResult = omci_msg_get_mdmObj(meClass, meInst, &meInfo,&mibUpload_mibCopy[instIndex].mdmObj, &iidStack);
                if(msgResult != OMCI_MSG_RESULT_SUCCESS)
                {
                    cmsLog_error("No instances found for %s : nbrInstancesUpload = %d",
                                 omciMeInfo[meClass].name,
                                 omciMeInfo[meClass].nbrInstancesUpload);

                    _cmsLck_releaseLock();

                    /* send response with ME count equal to 0 */
                    return omci_msg_mib_upload_error(pPacket);
                }

                mibUpload_count += omci_msg_get_frag_count(mibUpload_mibCopy[instIndex].mdmObj, meClass);

                SHORT_NOTICE("Copying %s/%d in entry %d (oid=%d, currCount=%d)",
                             meInfo.name, instCount, instIndex, meInfo.mdmOid, mibUpload_count);

                mibUpload_mibCopy[instIndex].meClass = meClass;

                instIndex++;

                instCount--;
            }
        }
    }

    _cmsLck_releaseLock();

    /* send MIB Upload response */
    OMCI_PACKET_MSG(pPacket)[0] = mibUpload_count >> 8;
    OMCI_PACKET_MSG(pPacket)[1] = mibUpload_count & 0xFF;
    omci_msg_padding(pPacket, 2);

    //Get timestamp of start of mibupload
    cmsTms_get(&MibUploadTime.Start);

    return omci_msg_send_response(pPacket);
}

/*****************************************************************************
 *  FUNCTION:       omci_msg_handler_MIBUploadNext
 *
 *  FILENAME:       omcid_msg.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *  RETURNS:
 *
*****************************************************************************/
static int omci_msg_handler_MIBUploadNext(omciPacket *pPacket, int len __attribute__((unused)))
{
    _MdmObjParamInfo paramInfo;
    UINT32 meInst;
    UINT16 meClassPkt;
    UINT16 meInstPkt;
    UINT32 attrMask = 0;
    CmsRet ret;
    SINT32 rv;
    UINT16 packetSeqNbr;

//    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);
    meClassPkt = OMCI_PACKET_ME_CLASS(pPacket);
    meInstPkt = OMCI_PACKET_ME_INST(pPacket);

    if((meClassPkt != 2) || (meInstPkt != 0))
    {
        cmsLog_notice("ERROR: Invalid header: meClass=%d, meInst=%d ***\n",
                      meClassPkt, meInstPkt);

        return 0;
    }

    /*
        The upload count tells us how many fragments
    */
    if(!mibUpload_count)
    {
        cmsLog_error("ERROR: MIB Upload is not in progress");

        /* MIB Upload is not in progress */
        return omci_msg_mib_upload_error(pPacket);
    }
    else
    {   /* MIB Upload is in progress so check for timeout */

        //Get current timestamp
        cmsTms_get(&MibUploadTime.Current);

        //Get Delta time
        cmsTms_delta(&MibUploadTime.Current, &MibUploadTime.Start, &MibUploadTime.Delta);

        //compare to start of mibupload to check for timeout
        if (MibUploadTime.Delta.sec >= 60)
        {
            // the previous mib upload has timed out so cleanup and allow a new one to start
            cmsLog_error("ERROR: MIB Upload time out has occurred");
            return omci_msg_mib_upload_error(pPacket);
        }
    }


    /* check sequence number */
    packetSeqNbr = (OMCI_PACKET_MSG(pPacket)[0] << 8) | OMCI_PACKET_MSG(pPacket)[1];
    if(mibUpload_seqNbr++ != packetSeqNbr)
    {
        cmsLog_notice("ERROR: Invalid sequence number: %d (expected %d)",
                      packetSeqNbr, mibUpload_seqNbr - 1);

        return omci_msg_mib_upload_error(pPacket);
    }

    if(mibUpload_mibCopy[mibUpload_instIndex].mdmObj == NULL)
    {
        cmsLog_error("mibUpload_mibCopy[%d].mdmObj == NULL", mibUpload_instIndex);

        /* internal error, send response with ME count equal to 0 and exit */
        omci_msg_mib_upload_error(pPacket);

        return -1;
    }

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);

        /* try again later */
        return omci_msg_mib_upload_error(pPacket);
    }

    /* get ME instance */
    if((ret = _cmsObj_getNthParam(mibUpload_mibCopy[mibUpload_instIndex].mdmObj, 0, &paramInfo)) != CMSRET_SUCCESS)
    {
        cmsLog_error("get of parameter %d of Object Id %d failed, ret=%d",
                     0, OMCID_GET_MDM_OBJ_OID(mibUpload_mibCopy[mibUpload_instIndex].mdmObj), ret);

        _cmsLck_releaseLock();

        /* internal error, send response with ME count equal to 0 and exit */
        omci_msg_mib_upload_error(pPacket);

        return -1;
    }

    /*
        ME includes MEID in count, but we are only interested in the 16 params that can be uploaded.
    */
    paramInfo.totalParams -= 1;

    meInst = *(UINT32 *)(paramInfo.val);

    /* managed entity id should not be included in the attribute mask */
    if((paramInfo.totalParams) > OMCI_ATTR_MASK_MAX)
    {
        cmsLog_error("Object is too big: totalParams = %d", paramInfo.totalParams);

        _cmsLck_releaseLock();

        /* internal error, send response with ME count equal to 0 and exit */
        omci_msg_mib_upload_error(pPacket);

        return -1;
    }
    /*
        right here we check if we are starting a new ME or continuing with the
        previous one.  If mibUpload_nextAttrMask is 0 we are starting a new ME
        and load the attribute mask of that ME, ow we continue working the existing
        list of parameters
    */
    if(!mibUpload_nextAttrMask)
    {
        /* managed entity id should not be included in the attribute mask */
        attrMask = (1 << paramInfo.totalParams) - 1;
        attrMask <<= OMCI_ATTR_MASK_MAX - paramInfo.totalParams;

        /*
            Adjust the mask based on the mibuploadattributelist so we know whether or not to
            upload all or just specific attributes.
        */
        attrMask &= omciMeInfo[mibUpload_mibCopy[mibUpload_instIndex].meClass].MibUploadAttributeList;

        mibUpload_nextAttrMask = attrMask;
    }
    else
    {
        attrMask = mibUpload_nextAttrMask;
    }

    OMCI_PACKET_MSG(pPacket)[0] = mibUpload_mibCopy[mibUpload_instIndex].meClass >> 8;
    OMCI_PACKET_MSG(pPacket)[1] = mibUpload_mibCopy[mibUpload_instIndex].meClass & 0xFF;
    OMCI_PACKET_MSG(pPacket)[2] = meInst >> 8;
    OMCI_PACKET_MSG(pPacket)[3] = meInst & 0xFF;

    /* debugging only */
    OMCID_DUMP_OBJECT(mibUpload_mibCopy[mibUpload_instIndex].mdmObj);

    SHORT_NOTICE("MIB Upload: count <%d>, instIndex <%d>, frag <%d>",
                 mibUpload_count, mibUpload_instIndex, mibUpload_fragCount);

    ret = omci_msg_copy_to_packet(pPacket, OMCI_MIB_UPLOAD_OVERHEAD,
                                  mibUpload_mibCopy[mibUpload_instIndex].mdmObj, &mibUpload_nextAttrMask, mibUpload_mibCopy[mibUpload_instIndex].meClass);
    if(ret == CMSRET_RESOURCE_EXCEEDED)
    {
        /* object does not fit in a single OMCI message */
        if(++mibUpload_fragCount > OMCI_MIB_UPLOAD_FRAG_MAX)
        {
            cmsLog_error("Exceeded maximum number of Upload fragments: %d", mibUpload_fragCount);

            _cmsLck_releaseLock();

            /* internal error, send response with ME count equal to 0 and exit */
            omci_msg_mib_upload_error(pPacket);

            return -1;
        }
    }
    else if(ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to create Upload Next Response (ret %d)", ret);

        _cmsLck_releaseLock();

        /* internal error, send response with ME count equal to 0 and exit */
        omci_msg_mib_upload_error(pPacket);

        return -1;
    }

    attrMask ^= mibUpload_nextAttrMask;
    OMCI_PACKET_MSG(pPacket)[4] = attrMask >> 8;
    OMCI_PACKET_MSG(pPacket)[5] = attrMask & 0xFF;

    if(!mibUpload_nextAttrMask)
    {
        mibUpload_fragCount = 1;
    }

    _cmsLck_releaseLock();

    /* send MIB Upload Next response */
    rv = omci_msg_send_response(pPacket);

    //reset timestamp of start of 60 second msg to msg timeout period
    cmsTms_get(&MibUploadTime.Start);

    if(rv)
    {
        return rv;
    }

    mibUpload_count--;

    if(!mibUpload_count)
    {
        omci_msg_mib_upload_clean();

        cmsLog_notice("*** End of MIB Upload ***");
    }
    else
    {
        if(!mibUpload_nextAttrMask)
        {
            mibUpload_instIndex++;
        }
    }

    return 0;
}


static int omci_msg_handler_MIBReset(omciPacket *pPacket, int len __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    /*
     * The MIB reset command applies to the ONU data ME (G.988/9.1.3) only.
     * Reset the MIB data sync attribute to 0x00 and reset the MIB of
     * the ONU to its default.
     */

    /* delete all OMCI objects that are created dynamically */
    omciMibDataReset();

    if (ret != CMSRET_SUCCESS)
    {
        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 3);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    /* reset the mib data sync counter */
    ret = mib_data_sync_update(MIB_DATA_SYNC_OP_RESET);
    if(ret != CMSRET_SUCCESS)
    {
        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 3);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    /* cleanup pending MIB Uploads, if any */
    omci_msg_mib_upload_clean();

    omci_pm_release();

    /* XXX for now, do nothing else... */
    OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
    omci_msg_padding(pPacket, 1);
    omci_msg_send_response(pPacket);
    return OMCI_PACKET_MSG(pPacket)[0];
}

static int omci_msg_handler_Test(omciPacket *pPacket, int len __attribute__((unused)))
{
    UBOOL8 found = FALSE;
    UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
    UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
    UINT16 tcId = OMCI_PACKET_TC_ID(pPacket);
    void* objPtr = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    omciMsgResult msgResult = OMCI_MSG_RESULT_SUCCESS;
    omciMeInfo_t meInfo;
    CmsRet ret = CMSRET_SUCCESS;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    // check MT=test supported ME class, ONT-G=256, ANI-G=263, circuit pack=6,
    // IP host confing=134, PPTP POTS UNI=53, PPTP ISDN UNI=80, dot1ag MEP=304
    if (meClass != 256 && meClass != 263 && meClass != 6 &&
        meClass != 134 && meClass != 53 && meClass != 80 &&
        meClass != 304 )
    {
        cmsLog_error("unsupported ME class %d", meClass);
        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_UNKNOWN_ME;
        goto out;
    }

    // check supported ME class
    msgResult = omci_msg_get_meInfo(meClass , &meInfo);
    if (msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        cmsLog_error("no such ME class %d", meClass);
        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_UNKNOWN_ME;
        goto out;
    }

    // attempt to lock OMCIMIB.
    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        goto out;
    }

    while (found == FALSE &&
           _cmsObj_getNextFlags(meInfo.mdmOid,
                               &iidStack,
                               OGF_NO_VALUE_UPDATE,
                               (void**)&objPtr) == CMSRET_SUCCESS)
    {
        found = (((MacBridgePortBridgeTableDataObject *)objPtr)->managedEntityId == meInst);

        _cmsObj_free((void**)&objPtr);
    }

    if (found == FALSE)
    {
        cmsLog_error("no such ME managed entity ID %d", meInst);
        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_UNKNOWN_ME_INST;
        goto out_unlock;
    }

    switch (meClass)
    {
        case 134: //IP host config
            switch (OMCI_PACKET_MSG(pPacket)[0])
            {
                case 1: // ping test
                    OMCI_PACKET_MSG(pPacket)[0] = omci_test_iphost_ping(tcId,
                                                            meClass, meInst,
                                                            OMCI_PACKET_MSG(pPacket)[0],
                                                            &OMCI_PACKET_MSG(pPacket)[1]);
                    break;
                case 2: // trace route
                    OMCI_PACKET_MSG(pPacket)[0] = omci_test_iphost_trace_route(tcId,
                                                                   meClass, meInst,
                                                                   OMCI_PACKET_MSG(pPacket)[0],
                                                                   &OMCI_PACKET_MSG(pPacket)[1]);
                    break;
                default:
                    // not support
                    OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_NOT_SUPPORTED;
                break;
            }
			break;
        case 263: //ANI-G
              OMCI_PACKET_MSG(pPacket)[0] = omci_test_aniG(OMCI_PACKET_TC_ID(pPacket), meClass, meInst);
              break;
        case 6: //circuit pack
        case 53: //PPTP POTS UNI
        case 80: //PPTP ISDN UNI
        case 256: //ONT-G
        case 304: //dot1ag MEP
        default:
            // not support
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_NOT_SUPPORTED;
            break;
    }

out_unlock:
    // release CMS lock.
    _cmsLck_releaseLock();

out:
    // send response with result.
    omci_msg_padding(pPacket, 1);
    omci_msg_send_response(pPacket);

    return OMCI_PACKET_MSG(pPacket)[0];
}

static UBOOL8 getSoftwareImageAttribute(UBOOL8 instanceId, omciSoftwareImageField field)
{
    UBOOL8 retVal = 0;
    SoftwareImageObject *obj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret = CMSRET_SUCCESS;

    // get first software image
    ret = _cmsObj_getNext(MDMOID_SOFTWARE_IMAGE, &iidStack, (void **) &obj);

    // get second software image if instance Id is not 0
    if (instanceId != 0 && ret == CMSRET_SUCCESS)
    {
        _cmsObj_free((void **) &obj);
        ret = _cmsObj_getNext(MDMOID_SOFTWARE_IMAGE, &iidStack, (void **) &obj);
    }

    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("get of SoftwareImageObject for instance Id %d failed, ret=%d", instanceId, ret);
    }
    else
    {
        switch (field)
        {
            case OMCI_SOFTWARE_IMAGE_VALID:
                retVal = obj->isValid;
                break;
            case OMCI_SOFTWARE_IMAGE_ACTIVE:
                retVal = obj->isActive;
                break;
            case OMCI_SOFTWARE_IMAGE_COMMIT:
                retVal = obj->isCommitted;
                break;
            default:
                break;
        }

        _cmsObj_free((void **) &obj);
    }

    return retVal;
}

// after call modifySoftwareImageObject() function
// MUST call cmsMgm_saveConfigToFlash() function
//static CmsRet modifySoftwareImageObject(UINT8 instanceId, omciSoftwareImageField field, UBOOL8 state)
static CmsRet modifySoftwareImageObject(UINT8 instanceId, omciSoftwareImageField field, char * pNewState)
{
    SoftwareImageObject *obj;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;

    // get first software image
    ret = _cmsObj_getNext(MDMOID_SOFTWARE_IMAGE, &iidStack, (void **) &obj);

    // get second software image if instance Id is not 0
    if (instanceId != 0 && ret == CMSRET_SUCCESS)
    {
        _cmsObj_free((void **) &obj);
        ret = _cmsObj_getNext(MDMOID_SOFTWARE_IMAGE, &iidStack, (void **) &obj);
    }

    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("get of SoftwareImageObject for instance Id %d failed, ret=%d", instanceId, ret);
    }
    else
    {
        switch (field)
        {
            case OMCI_SOFTWARE_IMAGE_VERSION:
                CMSMEM_REPLACE_STRING(obj->version, pNewState);
                break;
            case OMCI_SOFTWARE_IMAGE_VALID:
                obj->isValid = *(UBOOL8*)pNewState;
                break;
            case OMCI_SOFTWARE_IMAGE_ACTIVE:
                obj->isActive = *(UBOOL8*)pNewState;
                break;
            case OMCI_SOFTWARE_IMAGE_COMMIT:
                obj->isCommitted = *(UBOOL8*)pNewState;
                break;
            default:
                break;
        }
        if ((ret = _cmsObj_set(obj, &iidStack)) != CMSRET_SUCCESS)
        {
            cmsLog_error("set of SoftwareImageObject for instance Id %d failed, ret=%d", instanceId, ret);
        }

        _cmsObj_free((void **) &obj);
    }

    return ret;
}

static CmsRet validateMessageIdentifier(omciPacket *pPacket)
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
    UINT8 meMsInst = (OMCI_PACKET_ME_INST(pPacket) >> 8);
    UINT8 meLsInst = (OMCI_PACKET_ME_INST(pPacket) & 0xFF);

    /* verify me class and me instance id
       ME Class ID must be OMCI_ME_SOFTWARE_IMAGE (7)
       me Most Sig. instance must be ONT-G (0)
       me Least Sig. instance must be either instance 0, or 1
       instance id must remain constant throughout software
       download process
    */
    if (meClass != OMCI_ME_SOFTWARE_IMAGE)
    {
        cmsLog_error("unknown managed entity");
        // response with parameter error result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_UNKNOWN_ME;
        omci_msg_padding(pPacket, 1);
        ret = CMSRET_INVALID_PARAM_VALUE;
    }
    // make sure the MEID values are all valid
    else if (meMsInst != 0 || (meLsInst != 0 && meLsInst != 1))
    {
        cmsLog_error("unknown managed entity instance");
        // response with parameter error result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_UNKNOWN_ME_INST;
        omci_msg_padding(pPacket, 1);
        ret = CMSRET_INVALID_PARAM_VALUE;
    }
    // make sure the instance number doesn't change during the download process
    else if ((TRUE == SoftwareDownload.Flags.DownloadActive) && (meLsInst != SoftwareDownload.Instance))
    {
        cmsLog_error("software image instance doesn't match instance provided in Start Software Download");
        // response with parameter error result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
        omci_msg_padding(pPacket, 1);
        ret = CMSRET_INVALID_PARAM_VALUE;
    }

    return ret;
}

static UBOOL8 isFlashImageInProgress(omciPacket *pPacket)
{
    UBOOL8 ret = FALSE;

    if (SoftwareDownload.FlashPid == -1)
    {
        // response with successful result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
        omci_msg_padding(pPacket, 1);
    }
    else
    {
        // flash image is still in progress
        ret = TRUE;
        // response with device busy result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_DEV_BUSY;
        omci_msg_padding(pPacket, 1);
    }

    return ret;
}

static int omci_msg_handler_StartSoftwareDownload(omciPacket *pPacket, int len __attribute__((unused)))
{
    StartSoftwareDownloadMsg_t* pStartSwDownloadMsg;
    UINT32 BootPartition;
    UBOOL8 imgIfEnabled;
    imgutil_open_parms_t openParams;

    pStartSwDownloadMsg = (StartSoftwareDownloadMsg_t*)OMCI_PACKET_MSG(pPacket);
    BootPartition = devCtl_getBootedImagePartition();

    imgBootState = devCtl_getImageState();

    SoftwareDownload.State = SW_IMG_DL_STATE_INIT;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    if ( BOOTED_PART1_IMAGE == BootPartition)
    {
        SoftwareDownload.ActiveInstance = 0;
    }
    else
    {
        SoftwareDownload.ActiveInstance = 1;
    }

    if (validateMessageIdentifier(pPacket) != CMSRET_SUCCESS)
    {
        // send error response message
        return omci_msg_send_response(pPacket);
    }

    SoftwareDownload.Flags.SectionMissing  = FALSE;
    SoftwareDownload.CurrentImageSize      = 0;
    SoftwareDownload.CurrentWindowSize     = 0;
    SoftwareDownload.ExpectedSectionNumber = 0;
    SoftwareDownload.ImageSize             = OMCI_NTOHL(&(pStartSwDownloadMsg->ImageSize));
    SoftwareDownload.WindowSize            = pStartSwDownloadMsg->WindowSize > OMCI_WINDOW_SIZE_DEF ? \
                                             OMCI_WINDOW_SIZE_DEF : pStartSwDownloadMsg->WindowSize;
    SoftwareDownload.Instance              = OMCI_PACKET_SW_IMG_INST(pPacket);

    // invalid all attributes of the given Software Image instance
    if (_cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT) == CMSRET_SUCCESS)
    {
    UBOOL8
        RetVal = getSoftwareImageAttribute(SoftwareDownload.Instance, OMCI_SOFTWARE_IMAGE_COMMIT);

        _cmsLck_releaseLock();

        if (RetVal == TRUE)
        {
            cmsLog_error("Cannot overwrite committed software image %d\n", SoftwareDownload.Instance);
            // response with process error result
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
            omci_msg_padding(pPacket, 1);
            omci_msg_send_response(pPacket);
            return OMCI_PACKET_MSG(pPacket)[0];
        }
    }
    else
    {
        cmsLog_error("Processing error encountered while processing start software download.\nFailed to acquire lock needed to allow checking status of committed attribute\n");
        // response with process error result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    if (SoftwareDownload.ActiveInstance == SoftwareDownload.Instance)
    {
        // can't replace the currently active image
        cmsLog_error("Cannot download software image %d while it is active\n", SoftwareDownload.Instance);
        // response with process error result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    if (SoftwareDownload.ImageSize > devCtl_getSdramSize() ||
      TRUE != bcmImg_willFitInFlash(SoftwareDownload.ImageSize))
    {
        // can't accept an image that won't fit in RAM or Flash
        cmsLog_error("software image size of %d bytes exceeds RAM and/or Flash capacity\n", SoftwareDownload.ImageSize);
        // response with process error result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    // Re initialize window size and image size
            //    SoftwareDownload.WindowSize = SoftwareDownload.ImageSize = SoftwareDownload.CurrentImageSize = SoftwareDownload.ExpectedSectionNumber = 0;

    // free old download image and segment if any
    CMSMEM_FREE_BUF_AND_NULL_PTR(SoftwareDownload.pWindowBuffer);

    // keep new download image size
            //    memcpy(&SoftwareDownload.ImageSize, &OMCI_PACKET_MSG(pPacket)[1], 4);
    // keep new download (window size - 1) and copy it to the response packet
    cmsLog_notice("start software download, image size = %u, window size = %u", SoftwareDownload.ImageSize, SoftwareDownload.WindowSize);
    printf("\nStart Software Download\n.");
    printf ("\tBoot Partition 1 value = %x, Boot Partition 2 value = %x, Booted Partition = %x\n", BOOTED_PART1_IMAGE, BOOTED_PART2_IMAGE, BootPartition);
    printf ("\tRequested window size = %d, final window size = %d\n", pStartSwDownloadMsg->WindowSize+1, SoftwareDownload.WindowSize+1);

    // allocate new segment with the given size
    // SoftwareDownload.WindowSize + 1 since section number starts from 0
    // OMCI_PACKET_B_MSG_SIZE_MAX - 1 (1 byte for download section number)
    SoftwareDownload.pWindowBuffer = cmsMem_alloc((SoftwareDownload.WindowSize + 1) * (OMCI_PACKET_B_MSG_SIZE_MAX - 1), 0);

    if (SoftwareDownload.pWindowBuffer == NULL)
    {
        cmsLog_error("failed to allocate memory for segment size = %u", (SoftwareDownload.WindowSize + 1) * (OMCI_PACKET_B_MSG_SIZE_MAX -1));
        // response with process error result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    // invalid all attributes of the given Software Image instance
    if (_cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT) == CMSRET_SUCCESS)
    {
    UBOOL8
        NewState = FALSE;

        modifySoftwareImageObject(SoftwareDownload.Instance, OMCI_SOFTWARE_IMAGE_VALID, (char *)&NewState);
        modifySoftwareImageObject(SoftwareDownload.Instance, OMCI_SOFTWARE_IMAGE_ACTIVE, (char *)&NewState);
        modifySoftwareImageObject(SoftwareDownload.Instance, OMCI_SOFTWARE_IMAGE_COMMIT, (char *)&NewState);
        modifySoftwareImageObject(SoftwareDownload.Instance, OMCI_SOFTWARE_IMAGE_VERSION, (char *)ImageVersionTable[SoftwareDownload.Instance]);
        _cmsLck_releaseLock();
    }
    else
    {
        cmsLog_error("Processing error encountered while processing start software download.\nFailed to acquire lock needed to allow invalidation of ME attributes\n");
        // response with process error result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    if (imgutilHandle != NULL)
    {
        img_util_abort(imgutilHandle);
        imgutilHandle = NULL;
    }

    imgIfEnabled = img_util_get_incflash_mode();
    memset(&openParams, 0x0, sizeof(imgutil_open_parms_t));
    openParams.maxBufSize = SoftwareDownload.ImageSize;
    openParams.forceWholeFlashB = (imgIfEnabled == TRUE) ? 0 : 1;
    openParams.clientCtxP = msgHandle;
    openParams.options = CMS_IMAGE_WR_OPT_NO_REBOOT | Inst2PartTable[SoftwareDownload.Instance];
    openParams.calCrc32Func = (IMGUTIL_CAL_CRC32_FUNC)omciUtl_getCrc32Staged;
    imgutilHandle = img_util_open(&openParams);
    if (imgutilHandle != NULL)
    {
        printf("omcid: %s image flashing.\n",
          (imgIfEnabled == TRUE) ? "incremental" : "whole");
    }
    else
    {
        cmsLog_error("img_util_open() failed");
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    // copy (window size - 1) to the response packet
    OMCI_PACKET_MSG(pPacket)[1] = SoftwareDownload.WindowSize;

    // response with successful result
    OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
    omci_msg_padding(pPacket, 2);

    /* send response message */
    omci_msg_send_response(pPacket);

    SoftwareDownload.State = SW_IMG_DL_STATE_START;

    SoftwareDownload.Flags.DownloadActive = TRUE;

    return OMCI_PACKET_MSG(pPacket)[0];
}

/******************************************************************************
 *  FUNCTION:       omci_msg_handler_DownloadSection
 *
 *  FILENAME:       omcid_msg.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:    this routine processes the download section msg as a part of
 *                  the omci software download.
 *
 *  RETURNS:
 *
******************************************************************************/

static int omci_msg_handler_DownloadSection(omciPacket *pPacket, int len __attribute__((unused)))
{
    UINT8 CurrentSectionNumber = OMCI_PACKET_MSG(pPacket)[0];
    UBOOL8 AckRequested = OMCI_PACKET_ACK_REQ(pPacket);
    /* SectionLen garanteed to be valid value (checked in omci_message_handler) */
    UINT16 SectionLen = OMCI_CHECK_DEV_ID_A(pPacket) ? 
      OMCI_SOFTWARE_DOWNLOAD_SECTION_SIZE : (OMCI_NTOHS(&pPacket->B.msgLen) - 1);
    // RemainingImageLen is the amount of image data still to be received
    UINT32 RemainingImageLen = (SoftwareDownload.CurrentImageSize + 
      SoftwareDownload.CurrentWindowSize >= SoftwareDownload.ImageSize) ?
      0 : (SoftwareDownload.ImageSize - 
      (SoftwareDownload.CurrentImageSize + SoftwareDownload.CurrentWindowSize));

    if (OMCI_CHECK_DEV_ID_B(pPacket))
    {
        if (SoftwareDownload.CurrentImageSize + SectionLen > SoftwareDownload.ImageSize)
        {
            cmsLog_error("Download Section msg with invalid length. currentimagesize %d, expecting imagesize of %d.",
                    SoftwareDownload.CurrentImageSize + SectionLen, SoftwareDownload.ImageSize);

            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
            omci_msg_padding(pPacket, 1);
            goto Exit;
        }
    }
    else if (RemainingImageLen < SectionLen)
    {
        // change section len if at the end of the image to avoid copying pad bytes
        SectionLen = RemainingImageLen;
    }

    if (TRUE == SoftwareDownload.Flags.DownloadActive)
    {
        // validate the MEID and make sure each msg targets the original software image instance
        if (validateMessageIdentifier(pPacket) == CMSRET_SUCCESS)
        {
            // note that this block of code is for handling debugging cli cmds
            if (OMCI_ERR_NONE != OmciDbgErr)
            {
                printf("error opt is defined as ");

                if (OMCI_ERR_SWDL_SECTION_HOLE == OmciDbgErr)
                {
                    if (TRUE != AckRequested)
                    {
                        // if this is any section other than the last one of a window
                        // clear error state and return to effect dropping the packet

                        printf ("1\n");
                        OmciDbgErr = OMCI_ERR_NONE;
                        return (OMCI_MSG_RESULT_SUCCESS);
                    }
                }
                else if (OMCI_ERR_SWDL_SECTION_RSP == OmciDbgErr)
                {
                    if (TRUE == AckRequested)
                    {
                        // if this is the last section of a window clear error state
                        // and return to effect dropping the packet

                        printf ("2\n");
                        OmciDbgErr = OMCI_ERR_NONE;
                        return (OMCI_MSG_RESULT_SUCCESS);
                    }
                }
                else if (OMCI_ERR_SWDL_IMAGE_CRC == OmciDbgErr)
                {
                    printf ("3\n");
                    // Overwrite some section data in current pkt to cause CRC error
                    OMCI_PACKET_MSG(pPacket)[1] = 0;
                    OMCI_PACKET_MSG(pPacket)[2] = 0xFF;
                    OMCI_PACKET_MSG(pPacket)[3] = 0x55;
                    OMCI_PACKET_MSG(pPacket)[4] = 0xAA;

                    //clear error state and process pkt like usual
                    OmciDbgErr = OMCI_ERR_NONE;
                }
            }
            // end debugging code

            if (CurrentSectionNumber > SoftwareDownload.WindowSize)
            {
                cmsLog_error("Download Section msg with invalid section #.");
                OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
                omci_msg_padding(pPacket, 1);
                omci_msg_send_response(pPacket);
                return (OMCI_PACKET_MSG(pPacket)[0]);
            }

            // check for section holes
            if (CurrentSectionNumber == SoftwareDownload.ExpectedSectionNumber)
            {
                SoftwareDownload.ExpectedSectionNumber++;
            }
            else
            {
                cmsLog_error("Download Section message missing, transId %d, current %d, expected %d, windowSize %d\n",
                  OMCI_PACKET_TC_ID(pPacket), CurrentSectionNumber,
                  SoftwareDownload.ExpectedSectionNumber, SoftwareDownload.WindowSize);
                SoftwareDownload.Flags.SectionMissing = TRUE;
                //we found a hole and flagged it, so now set the next expected section to 1 past the current
                SoftwareDownload.ExpectedSectionNumber = CurrentSectionNumber+1;

                OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
                omci_msg_padding(pPacket, 1);
                goto Exit;
            }

            // copy the current section into the window buffer
            memcpy(&SoftwareDownload.pWindowBuffer[SoftwareDownload.CurrentWindowSize], &OMCI_PACKET_MSG(pPacket)[1], SectionLen);
            SoftwareDownload.CurrentWindowSize += SectionLen;

            /* by default respond with success result */
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;

            // only process end of window and send response if AckRequested is TRUE
            if (TRUE == AckRequested)
            {
                cmsLog_notice("SWDL_INFO before send Download Section response, "
                  "WindowSize = %u, CurrentSectionNumber = %u, "
                  "ImageSize = %u, CurrentImageSize = %u, Segment Size = %u, "
                  "RemainingImageSize = %u\n",
                  SoftwareDownload.WindowSize,
                  CurrentSectionNumber,
                  SoftwareDownload.ImageSize,
                  SoftwareDownload.CurrentImageSize,
                  SoftwareDownload.CurrentWindowSize,
                  RemainingImageLen);
                putchar('.');
                fflush(stdout);

                /* copy download section number to the response packet */
                OMCI_PACKET_MSG(pPacket)[1] = CurrentSectionNumber;
                omci_msg_padding(pPacket, 2);

                // only copy the window to the image buffer if no missing section error
                if (FALSE == SoftwareDownload.Flags.SectionMissing)
                {
                    // copy the current segment buffer into the image buffer
                    {
                        int byteCount;

                        byteCount = img_util_write(imgutilHandle,
                          (UINT8*)&SoftwareDownload.pWindowBuffer[0],
                          SoftwareDownload.CurrentWindowSize);
                        if ((byteCount < 0) ||
                          (byteCount != (int)SoftwareDownload.CurrentWindowSize))
                        {
                            cmsLog_error("img_util_write() failed, towrite=%d, ret=%d",
                              SoftwareDownload.CurrentWindowSize, byteCount);
                            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
                            omci_msg_padding(pPacket, 1);
                        }
                    }
                    SoftwareDownload.CurrentImageSize += SoftwareDownload.CurrentWindowSize;
                    // free segment memory but not image
                    if (SoftwareDownload.CurrentImageSize == SoftwareDownload.ImageSize)
                    {
                        CMSMEM_FREE_BUF_AND_NULL_PTR(SoftwareDownload.pWindowBuffer);
                    }
                }
                else
                {
                    cmsLog_error("Download section -- Section Missing");
                    // report section hole if one has been detected
                    OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
                    omci_msg_padding(pPacket, 1);
                    SoftwareDownload.Flags.SectionMissing = FALSE;
                }

                // AR==1 means end of window so next section should be 0 at start of next window
                SoftwareDownload.ExpectedSectionNumber = 0;
                SoftwareDownload.CurrentWindowSize = 0;
            }
        }
        else
        {
            // bad MEID must be reported
        }
    }
    else
    {
        cmsLog_error("Download section msg received when download not in progress");
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
    }

Exit:
    omci_msg_send_response(pPacket);

    return (OMCI_PACKET_MSG(pPacket)[0]);
}


static int omci_msg_handler_EndSoftwareDownload(omciPacket *pPacket, int len __attribute__((unused)))
{
    UINT32 imageCrc = 0, imageSize = 0;
    CmsTimestamp begin_tms, end_tms;
    imgutil_img_info_t imgInfo;
    int rc;

    cmsTms_get(&begin_tms);

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    // we terminate the software download when this message is received period!

    if (TRUE == SoftwareDownload.Flags.DownloadActive)
    {
        if (validateMessageIdentifier(pPacket) == CMSRET_SUCCESS)
        {
            // any active download ends with receipt of valid occurrence of this msg type
            // regardless of whether the image is valid, is flashed, etc...

            if ((SoftwareDownload.FlashPid == -1) &&
              (IsSwdlInEndState() == TRUE))
            {
                // if flash child process is terminated then
                // set DownloadActive to FALSE
                omciMsgResult msgResult;

                SoftwareDownload.Flags.DownloadActive = FALSE;
                cmsTms_get(&end_tms);
                msgResult = (SoftwareDownload.State == SW_IMG_DL_STATE_END_SUCCESS) ?
                  OMCI_MSG_RESULT_SUCCESS : OMCI_MSG_RESULT_PROC_ERROR;
                OMCI_PACKET_MSG(pPacket)[0] = msgResult;
                printf("\nEnd software download is responded with %s\n"
                  "End software download msg received when flash child progress is terminated\n"
                  "OMCI message response time in %d milli-seconds\n\n",
                  ((msgResult == OMCI_MSG_RESULT_SUCCESS) ? "SUCCESS" : "ERROR"),
                  cmsTms_deltaInMilliSeconds(&end_tms, &begin_tms));
                return omci_msg_send_response(pPacket);
            }
            else if (SoftwareDownload.FlashPid != -1)
            {
                // if flash child process is still in progress
                // then send device busy message back since
                // isFlashImageInProgress changes pPacket to
                // return OMCI_MSG_RESULT_DEV_BUSY
                // otherwise it changes pPacket to return
                // OMCI_MSG_RESULT_SUCCESS
                isFlashImageInProgress(pPacket);
                cmsTms_get(&end_tms);
                printf("\nEnd software download msg received when flash child process is still in progess\n"
                "OMCI message response time in %d milli-seconds\n\n",
                cmsTms_deltaInMilliSeconds(&end_tms, &begin_tms));
                return omci_msg_send_response(pPacket);
            }

            // get the given CRC
            imageCrc = OMCI_NTOHL(&OMCI_PACKET_MSG(pPacket)[0]);
            // get the given image size
            imageSize = OMCI_NTOHL(&OMCI_PACKET_MSG(pPacket)[4]);
            cmsLog_notice("End software download, image CRC = %u, image size = %u", imageCrc, imageSize);
            printf("\nEnd Software Download\n\n");

            if (imageSize != SoftwareDownload.ImageSize)
            {
                cmsLog_error("image size in start software download %u does not match "
                  "with image size in end software download %u",
                  SoftwareDownload.ImageSize, imageSize);
                // response with parameter error result
                OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
                omci_msg_padding(pPacket, 1);
                // send error response message
                omci_msg_send_response(pPacket);

                SoftwareDownload.Flags.DownloadActive = FALSE;
                img_util_abort(imgutilHandle);
                imgutilHandle = NULL;

                return OMCI_PACKET_MSG(pPacket)[0];
            }

            rc = img_util_get_imginfo(imgutilHandle, IMGINFO_SRC_CALCOMBO,
              IMGUTIL_IMGINFO_BIT_CRC, &imgInfo);
            if ((rc < 0) || ((rc == 0) && (imageCrc != imgInfo.crc)))
            {
                cmsLog_error("image crc 0x%08x in end software download does not match"
                  " with the calculated image crc 0x%08x",
                  imageCrc, imgInfo.crc);
                OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
                omci_msg_padding(pPacket, 1);
                omci_msg_send_response(pPacket);

                SoftwareDownload.Flags.DownloadActive = FALSE;
                img_util_abort(imgutilHandle);
                imgutilHandle = NULL;

                return OMCI_PACKET_MSG(pPacket)[0];
            }

            if (SoftwareDownload.FlashPid == -1)
            {
                createFlashImageThread();
                printf("flash thread created separately...\n");
            }

            SoftwareDownload.State = SW_IMG_DL_STATE_END_PENDING;

            // if no errors then send device busy response message
            // since image should be flashed next
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_DEV_BUSY;
            omci_msg_padding(pPacket, 1);
            // send device busy response message
            cmsTms_get(&end_tms);
            printf("\nEnd software download msg received when flashing image\n"
              "OMCI message response time in %d milli-seconds\n\n",
              cmsTms_deltaInMilliSeconds(&end_tms, &begin_tms));
        }
    }
    else
    {
        cmsLog_error("End software download msg received when download not in progress");
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
    }

    return omci_msg_send_response(pPacket);
}

static int omci_msg_handler_ActivateImage(omciPacket *pPacket, int len __attribute__((unused)))
{
    int rv = 0;
    UINT8 CurrentImageInstance = 0, OtherImageInstance = 0;
    CmsTimestamp begin_tms, end_tms;

    cmsTms_get(&begin_tms);

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    if (validateMessageIdentifier(pPacket) != CMSRET_SUCCESS)
    {
        // send error response message
        return omci_msg_send_response(pPacket);
    }

    //msg can be sent at any time so can't rely on value being from must recent sw img dl
    CurrentImageInstance = OMCI_PACKET_SW_IMG_INST(pPacket);

    if (CurrentImageInstance == 0)
    {
        OtherImageInstance = 1;
    }

    /*

    ADD CODE HERE TO FULLY VALIDATE THE CURRENT SOFTWARE IMAGE DOWNLOAD
    STATE TO ENSURE THIS IS A LEGAL OPERATION NOW

    */

    // if flash child process is still in progress
    // then send device busy message back
    if (SoftwareDownload.FlashPid != -1 &&
        isFlashImageInProgress(pPacket) == TRUE)
    {
        cmsTms_get(&end_tms);
        printf("\nActivate image msg received when flash child process is still in progess\nOMCI message response time in %d milli-seconds\n\n", cmsTms_deltaInMilliSeconds(&end_tms, &begin_tms));
        // send device busy response message
        return omci_msg_send_response(pPacket);
    }

    if (_cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT) == CMSRET_SUCCESS)
    {
        if (getSoftwareImageAttribute(CurrentImageInstance, OMCI_SOFTWARE_IMAGE_VALID) == TRUE)
        {
            UBOOL8 NewState = TRUE;

            // respond with successful result
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
            // padding packet before sending it back
            omci_msg_padding(pPacket, 1);
            // send successful response message right away
            // since accessing flash is slow in some boards
            cmsTms_get(&end_tms);
            printf("\nActivate image is responded with SUCCESS\nOMCI message response time in %d milli-seconds\n\n", cmsTms_deltaInMilliSeconds(&end_tms, &begin_tms));
            rv = omci_msg_send_response(pPacket);

            // set "isActive" attribute of the given Software Image
            // instance to be active
            modifySoftwareImageObject(CurrentImageInstance, OMCI_SOFTWARE_IMAGE_ACTIVE, (char *)&NewState);
            // set "isActive" attribute of the other Software Image
            // instance to be inactive
            NewState = FALSE;
            modifySoftwareImageObject(OtherImageInstance, OMCI_SOFTWARE_IMAGE_ACTIVE, (char *)&NewState);

            // now set boot state
            if (getSoftwareImageAttribute(CurrentImageInstance, OMCI_SOFTWARE_IMAGE_COMMIT) == TRUE)
            {
                // set image state to BOOT_SET_NEW_IMAGE to reactivate existing image
                if (devCtl_setImageState(Inst2BootPartTableCommit[CurrentImageInstance]) == CMSRET_SUCCESS)
                {
                    cmsLog_notice("OMCID sleeps 1 seconds before sending Reboot message to SMD");
                    if (cmsTmr_set(tmrHandle, sendSystemReboot, NULL, 1000, "omci_reboot") != CMSRET_SUCCESS)
                        cmsLog_error("setting delayed signal processing timer failed");
                }
                else
                    cmsLog_error("setting boot state = %x failed\n", Inst2BootPartTableCommit[CurrentImageInstance]);
            }
            else
            {
                // set image state to BOOT_SET_NEW_IMAGE_ONCE for initial activation
                if (devCtl_setImageState(Inst2BootPartTableActivate[CurrentImageInstance]) == CMSRET_SUCCESS)
                {
                    cmsLog_notice("OMCID sleeps 1 seconds before sending Reboot message to SMD");
                    if (cmsTmr_set(tmrHandle, sendSystemReboot, NULL, 1000, "omci_reboot") != CMSRET_SUCCESS)
                        cmsLog_error("setting delayed signal processing timer failed");
                }
                else
                    cmsLog_error("setting boot state = %x failed\n", Inst2BootPartTableActivate[CurrentImageInstance]);
            }
        }
        else
        {
            cmsLog_error("cannot activate an image whose valid attribute is not set");
            // response with error result
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
            // padding packet before sending it back
            omci_msg_padding(pPacket, 1);
            cmsTms_get(&end_tms);
            printf("\nActivate image is responded with PARM_ERROR\nOMCI message response time in %d milli-seconds\n\n", cmsTms_deltaInMilliSeconds(&end_tms, &begin_tms));
            rv = omci_msg_send_response(pPacket);
        }

        // release lock
        _cmsLck_releaseLock();
    }
    else
    {
        cmsLog_error("failed to get lock");
        // response with error result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        // padding packet before sending it back
        omci_msg_padding(pPacket, 1);
        cmsTms_get(&end_tms);
        printf("\nActivate image is responded with PROC_ERROR\nOMCI message response time in %d milli-seconds\n\n", cmsTms_deltaInMilliSeconds(&end_tms, &begin_tms));
        rv = omci_msg_send_response(pPacket);
    }

    SoftwareDownload.State = SW_IMG_DL_STATE_ACTIVATE;

    return rv;
}

static int omci_msg_handler_CommitImage(omciPacket *pPacket, int len __attribute__((unused)))
{
    int rv = 0;
    UINT8 CurrentImageInstance = 0, OtherImageInstance = 0;
    CmsTimestamp begin_tms, end_tms;

    cmsTms_get(&begin_tms);

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    if (validateMessageIdentifier(pPacket) != CMSRET_SUCCESS)
    {
        // send error response message
        return omci_msg_send_response(pPacket);
    }

    //msg can be sent at any time so can't rely on value being from must recent sw img dl
    CurrentImageInstance = OMCI_PACKET_SW_IMG_INST(pPacket);

    /*

    ADD CODE HERE TO FULLY VALIDATE THE CURRENT SOFTWARE IMAGE DOWNLOAD
    STATE TO ENSURE THIS IS A LEGAL OPERATION NOW

    */

    if (CurrentImageInstance == 0)
        OtherImageInstance = 1;

    if (_cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT) == CMSRET_SUCCESS)
    {
        if (getSoftwareImageAttribute(CurrentImageInstance, OMCI_SOFTWARE_IMAGE_VALID) == TRUE)
        {
            UBOOL8 NewState = TRUE;

            // response with successful result
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
            // padding packet before sending it back
            omci_msg_padding(pPacket, 1);
            // send successful response message right away
            // since accessing flash is slow in some boards
            cmsTms_get(&end_tms);
            printf("\nCommit image is responded with SUCCESS\nOMCI message response time in %d milli-seconds\n\n", cmsTms_deltaInMilliSeconds(&end_tms, &begin_tms));
            rv = omci_msg_send_response(pPacket);

            // set "isCommited" attribute of the given Software Image
            // instance to be commited
            modifySoftwareImageObject(CurrentImageInstance, OMCI_SOFTWARE_IMAGE_COMMIT, (char *)&NewState);
            // set "isCommited" attribute of the other Software Image
            // instance to be un-commited
            NewState = FALSE;
            modifySoftwareImageObject(OtherImageInstance, OMCI_SOFTWARE_IMAGE_COMMIT, (char *)&NewState);

            if (devCtl_setImageState(Inst2BootPartTableCommit[CurrentImageInstance]) != CMSRET_SUCCESS)
            {
                cmsLog_error("setting boot state = %x failed\n",Inst2BootPartTableCommit[CurrentImageInstance] );
            }
        }
        else
        {
            cmsLog_error("cannot commit an image whose valid attribute is not set");
            // response with error result
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
            // padding packet before sending it back
            omci_msg_padding(pPacket, 1);
            cmsTms_get(&end_tms);
            printf("\nCommit image is responded with PARM_ERROR\nOMCI message response time in %d milli-seconds\n\n", cmsTms_deltaInMilliSeconds(&end_tms, &begin_tms));
            rv = omci_msg_send_response(pPacket);
        }

        // release lock
        _cmsLck_releaseLock();
    }
    else
    {
        cmsLog_error("failed to get lock");
        // response with error result
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        // padding packet before sending it back
        omci_msg_padding(pPacket, 1);
        cmsTms_get(&end_tms);
        printf("\nCommit image is responded with PROC_ERROR\nOMCI message response time in %d milli-seconds\n\n", cmsTms_deltaInMilliSeconds(&end_tms, &begin_tms));
        rv = omci_msg_send_response(pPacket);
    }

    SoftwareDownload.State = SW_IMG_DL_STATE_COMMIT;

    return rv;
}

static void synchronizeTime(void* handle __attribute__((unused)))
{
    numberOf15MinInterval++;
    // value of numberOf15MinInterval should not greater than 255
    if (numberOf15MinInterval > 255)
    {
        numberOf15MinInterval = 0;
    }
}

static int omci_msg_handler_SynchronizeTime(omciPacket *pPacket, int len __attribute__((unused)))
{
    int ret = 0;
    CmsRet cmsRet = CMSRET_SUCCESS;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
    omci_msg_padding(pPacket, 1);
    ret = omci_msg_send_response(pPacket);

    // start synchronize time
    numberOf15MinInterval = 0;
    cmsTmr_cancel(tmrHandle, synchronizeTime, NULL);
    cmsLog_notice("OMCID starts to synchronize time with OLT for 15 minutes interval");
    cmsRet = cmsTmr_set(tmrHandle, synchronizeTime, NULL, 900000, "omci_synchronizeTime");
    if (cmsRet != CMSRET_SUCCESS)
       cmsLog_error("Time synchronization is failed since setting delayed signal processing timer failed, ret=%d", cmsRet);

#ifdef BUILD_BCMIPC
    omcid_pm_send_sync_time();
#endif /* BUILD_BCMIPC */

    return ret;
}

static int omci_msg_handler_Reboot(omciPacket *pPacket, int len __attribute__((unused)))
{
    int ret = 0;
    CmsRet cmsRet;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
    omci_msg_padding(pPacket, 1);
    ret = omci_msg_send_response(pPacket);

    cmsLog_notice("OMCID sleeps 2 seconds before sending Reboot message to SMD");
    cmsRet = cmsTmr_set(tmrHandle, sendSystemReboot, NULL, 2000, "omci_reboot");
    if (cmsRet != CMSRET_SUCCESS)
    {
       cmsLog_error("setting delayed signal processing timer failed, ret=%d", cmsRet);
    }

    return ret;
}

static int omci_msg_handler_Get_Next(omciPacket *pPacket, int len __attribute__((unused)))
{
    omciMeInfo_t meInfo;
    void *mdmObj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
    UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
    UINT16 attrMask = OMCI_MSG_GET_ATTR_MASK(pPacket);
    UINT16 sequenceNum = OMCI_MSG_GET_SEQUENCE_NUM(pPacket);
    omciMsgResult msgResult;
    CmsRet ret;

    OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

    /* copy attribute mask to the response packet */
    OMCI_PACKET_MSG(pPacket)[2] = OMCI_PACKET_MSG(pPacket)[1];
    OMCI_PACKET_MSG(pPacket)[1] = OMCI_PACKET_MSG(pPacket)[0];

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);

        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PROC_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);
        return OMCI_PACKET_MSG(pPacket)[0];
    }

    /* use omci_msg_get_mdmObj to retrieve iidStack */
    msgResult = omci_msg_get_mdmObj(meClass, meInst, &meInfo, &mdmObj, &iidStack);

    if(msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        /* send response, indicating the cause of the error */
        OMCI_PACKET_MSG(pPacket)[0] = msgResult;
        omci_msg_padding(pPacket, 1);
    }
    else
    {
        void *obj = NULL;
        UINT16 objSize = 0;
        CmsRet result;

        if (meInfo.custom_get == NULL)
        {
            cmsLog_error("Invalid operation, classId=%d, instance=%d, attrMask=%d",
              meClass, meInst, attrMask);

            /* send response, indicating the cause of the error */
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
            omci_msg_padding(pPacket, 1);
            omci_msg_send_response(pPacket);
            _cmsObj_free(&mdmObj);
            _cmsLck_releaseLock();
            return OMCI_PACKET_MSG(pPacket)[0];
        }

        result = meInfo.custom_get(&obj, &objSize, &iidStack, attrMask);

        if (result == CMSRET_SUCCESS)
        {
            UINT16 respSize = 29; /* OMCI_PACKET_A_MSG_SIZE - OMCI_GET_OVERHEAD (32 - 3) */
            int len = objSize - (sequenceNum * respSize);
            /* sequence number is in range, send response with sucess */
            if (len >= 0)
            {
               OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_SUCCESS;
                /* response data >= response size so only return response size */
               if (len >= respSize)
               {
                   memcpy(&OMCI_PACKET_MSG(pPacket)[OMCI_GET_OVERHEAD], &((UINT8 *)obj)[sequenceNum*respSize], respSize);
               }
                /* response data < response size so return data with padding */
               else
               {
                   memcpy(&OMCI_PACKET_MSG(pPacket)[OMCI_GET_OVERHEAD], &((UINT8 *)obj)[sequenceNum*respSize], len);
                   omci_msg_padding(pPacket, len + OMCI_GET_OVERHEAD);
               }
            }
            /* sequence number is out of range, send response with error */
            else
            {
                OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
                omci_msg_padding(pPacket, 1);
            }
            cmsMem_free(obj);
        }
        else
        {
            OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
            omci_msg_padding(pPacket, 1);
        }

        /* debugging only */
        //OMCID_DUMP_OBJECT(mdmObj);
        _cmsObj_free(&mdmObj);
    }

    cmsLog_notice("*** Get Next ME: '%s' ***\n", meInfo.name);

    _cmsLck_releaseLock();

    /* send response message */
    omci_msg_send_response(pPacket);

    return OMCI_PACKET_MSG(pPacket)[0];
}

/*
 * Command handlers
 */
/* G.988 - OMCI Message Types */
static omciMsgHandler_t omciMsgHandler[OMCI_PACKET_MT_MAX] = {
    [0]  = {&omci_msg_handler_InvalidCmd, 0, "Invalid"},
    [1]  = {&omci_msg_handler_InvalidCmd, 0, "Invalid"},
    [2]  = {&omci_msg_handler_InvalidCmd, 0, "Invalid"},
    [3]  = {&omci_msg_handler_InvalidCmd, 0, "Invalid"},
    [4]  = {&omci_msg_handler_Create, 1, "Create"},
    [5]  = {&omci_msg_handler_InvalidCmd, 0, "Invalid"},
    [6]  = {&omci_msg_handler_Delete, 1, "Delete"},
    [7]  = {&omci_msg_handler_InvalidCmd, 0, "Invalid"},
    [8]  = {&omci_msg_handler_Set, 1, "Set"},
    [9]  = {&omci_msg_handler_Get, 0, "Get"},
    [10] = {&omci_msg_handler_InvalidCmd, 0, "Invalid"},
    [11] = {&omci_msg_handler_GetAllAlarms, 0, "Get All Alarms"},
    [12] = {&omci_msg_handler_GetAllAlarmsNext, 0, "Get All Alarms Next"},
    [13] = {&omci_msg_handler_MIBUpload, 0, "MIB Upload"},
    [14] = {&omci_msg_handler_MIBUploadNext, 0, "MIB Upload Next"},
    [15] = {&omci_msg_handler_MIBReset, 0, "MIB Reset"},
    [16] = {&omci_msg_handler_InvalidCmd, 0, "Alarm"},
    [17] = {&omci_msg_handler_InvalidCmd, 0, "Attribute Value Change"},
    [18] = {&omci_msg_handler_Test, 0, "Test"},
    [19] = {&omci_msg_handler_StartSoftwareDownload, 1, "Start Software Download"},
    [20] = {&omci_msg_handler_DownloadSection, 0, "Download Section"},
    [21] = {&omci_msg_handler_EndSoftwareDownload, 1, "End Software Download"},
    [22] = {&omci_msg_handler_ActivateImage, 1, "Activate Image"},
    [23] = {&omci_msg_handler_CommitImage, 1, "Commit Image"},
    [24] = {&omci_msg_handler_SynchronizeTime, 0, "Synchronize Time"},
    [25] = {&omci_msg_handler_Reboot, 0, "Reboot"},
    [26] = {&omci_msg_handler_Get_Next, 0, "Get Next"},
    [27] = {&omci_msg_handler_InvalidCmd, 0, "Test result"},
    [28] = {&omci_msg_handler_Get_Current, 0, "Get Current Data"},
    [29] = {&omci_msg_handler_SetTable, 1, "Set Table"},
    [30] = {&omci_msg_handler_InvalidCmd, 0, "Invalid"},
    [31] = {&omci_msg_handler_InvalidCmd, 0, "Invalid"}
};


static CmsRet initOmciObject(void)
{
    CmsRet ret = CMSRET_SUCCESS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    OmciObject *omciObj = NULL;
    char *meTypesHexString = NULL, *msgTypesHexString = NULL;
    UINT8 meTypes[OMCI_ME_CLASS_MAX*2], msgTypes[OMCI_PACKET_MT_MAX];
    UINT16 i = 0, j = 0;

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        return ret;
    }

    if ((ret = _cmsObj_get(MDMOID_OMCI, &iidStack, 0, (void **)&omciObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("get of object id %d failed, ret=%d", MDMOID_OMCI, ret);
        _cmsLck_releaseLock();
        return ret;
    }

    // The hexstring in MDM is stored in network order. The byte order
    // conversion may be needed after cmsUtl_hexStringToBinaryBuf(),
    // it is case-by-case based.
    for (i = 0, j = 0; i < OMCI_ME_CLASS_MAX; i++)
    {
        if (omciMeInfo[i].mdmOid != 0)
        {
            OMCI_HTONS(&meTypes[j], i);
            j += 2;
        }
    }
    if (j > 0)
    {
        ret = cmsUtl_binaryBufToHexString(meTypes, j, &meTypesHexString);
        cmsMem_free(omciObj->meTypeTable);
        omciObj->meTypeTable = meTypesHexString;
    }

    for (i = 0, j = 0; i < OMCI_PACKET_MT_MAX; i++)
    {
        if (omciMsgHandler[i].pMsgHandler != omci_msg_handler_InvalidCmd &&
            omciMsgHandler[i].pMsgHandler != omci_msg_handler_NotSupported)
        {
            msgTypes[j++] = i;
        }
    }
    if (j > 0)
    {
        ret = cmsUtl_binaryBufToHexString(msgTypes, j, &msgTypesHexString);
        cmsMem_free(omciObj->messageTypeTable);
        omciObj->messageTypeTable = msgTypesHexString;
    }

    if ((ret = _cmsObj_set(omciObj, &iidStack)) != CMSRET_SUCCESS)
    {
        cmsLog_error("set of object id %d failed, ret=%d", MDMOID_OMCI, ret);
    }

    _cmsObj_free((void **)&omciObj);
    _cmsLck_releaseLock();

    return ret;
}

/******************************************************************************
 * Public Functions
 ******************************************************************************/
CmsRet omci_msg_dump_me(UINT16 meClass, UINT16 meInst)
{
    omciMeInfo_t meInfo;
    void *mdmObj;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;
    omciMsgResult msgResult;
    UINT32 headerMask;

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT)) != CMSRET_SUCCESS)
    {
        cmsLog_error("failed to get lock, ret=%d", ret);
        return ret;
    }

    msgResult = omci_msg_get_mdmObj(meClass, meInst, &meInfo, &mdmObj, &iidStack);
    if(msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        cmsLog_error("mdm object not found, ret=%d", ret);
        ret = CMSRET_OBJECT_NOT_FOUND;
    }
    else
    {
        headerMask = cmsLog_getHeaderMask();
        cmsLog_setHeaderMask(0);

        omcimib_dumpObject(mdmObj);

        cmsLog_setHeaderMask(headerMask);

        _cmsObj_free(&mdmObj);
    }

    _cmsLck_releaseLock();

    return ret;
}

#define OMCI_BANNER_BEGIN "\n********************* OMCI MESSAGE BEGIN *********************\n"
#define OMCI_BANNER_END   "********************** OMCI MESSAGE END **********************\n"

int omci_msg_handler(omciPacket *pPacket, int len)
{
    int rv = 0, msgRet = 0;
    UINT8 msgType = OMCI_PACKET_MT(pPacket);
    int txCount;
    UINT8 prio = OMCI_PACKET_PRIORITY(pPacket);
    static int msgCount = 0;
    DECLARE_PGPONOMCI();

    // capture every omci msg packet
    omci_msg_capture((UINT8 *)pPacket, len);

    SHORT_NOTICE("%s", OMCI_BANNER_BEGIN);

    if (OMCI_CHECK_DEV_ID_A(pPacket))
    {
        OMCID_STATS_INC(rxBaseLinePackets);
    }
    else if (OMCI_CHECK_DEV_ID_B(pPacket))
    {
        OMCID_STATS_INC(rxExtendedPackets);
    }
    if (len <= OMCI_PACKET_B_HDR_SIZE + OMCI_PACKET_MIC_SIZE ||
        (OMCI_CHECK_DEV_ID_A(pPacket) && len != OMCI_PACKET_A_SIZE) ||
        (OMCI_CHECK_DEV_ID_B(pPacket) && 
        (len > OMCI_PACKET_B_SIZE_MAX ||  
        OMCI_NTOHS(&pPacket->B.msgLen) > OMCI_PACKET_B_MSG_SIZE_MAX)))
    {
        cmsLog_error("Invalid OMCI packet length: %d\n", len);
        if (OMCI_CHECK_DEV_ID_B(pPacket) && len > OMCI_PACKET_B_HDR_SIZE + OMCI_PACKET_MIC_SIZE)
            cmsLog_error("Invalid OMCI message length: %d\n", OMCI_NTOHS(&pPacket->B.msgLen));

        OMCID_STATS_INC(rxLengthErrors);

        SHORT_NOTICE("%s", OMCI_BANNER_END);

        return -1;
    }

    /* Support Baseline and two Extended messages types DOWNLOADSECTION, SETTABLE. */
    if (!OMCI_CHECK_DEV_ID_A(pPacket) &&
       !(OMCI_CHECK_DEV_ID_B(pPacket) && (msgType == OMCI_MSG_TYPE_DOWNLOADSECTION)) &&
       !(OMCI_CHECK_DEV_ID_B(pPacket) && (msgType == OMCI_MSG_TYPE_SETTABLE)))
    {
        /* invalid devId, drop packet */
        cmsLog_error("Invalid OMCI packet Device ID or message type: %d, %d\n",
          pPacket->devId, msgType);

        OMCID_DUMP_PACKET(pPacket, OMCI_PACKET_A_SIZE);

        OMCI_PACKET_MSG(pPacket)[0] = OMCI_MSG_RESULT_PARM_ERROR;
        omci_msg_padding(pPacket, 1);
        omci_msg_send_response(pPacket);

        SHORT_NOTICE("%s", OMCI_BANNER_END);

        OMCID_STATS_INC(rxOtherErrors);

        return 0;
    }

    if (msgType != OMCI_MSG_TYPE_DOWNLOADSECTION)
    {
        //cmsLog_notice("CRC32 = 0x%08X", expectedCrc32);
        cmsLog_notice(" OMCI message #%d: '%s'\n", ++msgCount, omciMsgName[msgType]);
    }

    OMCID_STATS_INC(rxGoodPackets);

    /* process request packet if not a duplicate tcid */
    if(pGponOmci->prevTcId[prio] != OMCI_PACKET_TC_ID(pPacket))
    {
        // if flash child process for Software Donwload is still in progress
        // then send device busy message back
        if (SoftwareDownload.FlashPid != -1 &&
            isFlashImageInProgress(pPacket) == TRUE)
        {
            // send device busy response message
            return omci_msg_send_response(pPacket);
        }

        /* call handler of the command */
        msgRet = omciMsgHandler[msgType].pMsgHandler(pPacket, len);

        if (msgRet == OMCI_MSG_RESULT_SUCCESS)
        {
            /* increase mibDataSync, if needed */
            if(omciMsgHandler[msgType].incMibDataSync)
            {
                mib_data_sync_update(MIB_DATA_SYNC_OP_INC);
            }
        }
        else
        {
            UINT16 meClass = OMCI_PACKET_ME_CLASS(pPacket);
            UINT16 meInst = OMCI_PACKET_ME_INST(pPacket);
            UINT16 attrMask = OMCI_MSG_GET_ATTR_MASK(pPacket);
            UINT16 sequenceNum = OMCI_MSG_GET_SEQUENCE_NUM(pPacket);
            printf("\n========================================================================\n");
            printf("Errors occur during handle message:\n");
            printf("    Message type:                   %s\n", omciMsgHandler[msgType].name);
            printf("    Error message:                  %s\n", resultMessageTable[msgRet]);
            printf("    Managed entity class:           %d (0x%x)\n", meClass, meClass);
            printf("    Managed entity instance:        %d (0x%x)\n", meInst, meInst);
            printf("    Managed entity attribute mask:  0x%04x\n", attrMask);
            printf("    Sequence number:                %d (0x%x)\n", sequenceNum, sequenceNum);
            printf("========================================================================\n\n");
        }

        // do not return error even when msgRet != OMCI_MSG_RESULT_SUCCESS
        // to avoid omci stack exists pre-maturely
        rv = 0;
    }
    else
    {
        if (pGponOmci->prevTcId[prio] != OMCI_PACKET_TC_ID_INIT)
        {
            cmsLog_notice(" Response Message: *** Re-transmission (tcId = %d) ***\n", OMCI_PACKET_TC_ID(pPacket));

            OMCID_DUMP_PACKET(&pGponOmci->prevRespPacket[prio], OMCI_PACKET_A_SIZE);

            /* only send response packet to driver if requester is not HTTPD */
            if (pPacket->src_eid != EID_HTTPD)
            {
                int tx_size = OMCI_PACKET_SIZE(&pGponOmci->prevRespPacket[prio]) - OMCI_PACKET_MIC_SIZE;

                /* re-transmit previous response packet */
                txCount = gpon_omci_api_transmit(&pGponOmci->prevRespPacket[prio],
                    tx_size);
                if((txCount == -1) && (errno == ENOLINK))
                {
                    /* Return success: the OMCI Port has been deleted */
                    cmsLog_notice("OMCI Port has been deleted, continue");
                    rv = 0;
                }
                else if(txCount != tx_size)
                {
                    cmsLog_error("Failed to transmit Response Packet (count = %d)\n", txCount);
                    OMCID_STATS_INC(txErrors);
                    rv = -1;
                }
                else
                {
                    OMCID_STATS_INC(txRetransmissions);
                    rv = 0;
                }
            }
            /* if requester is HTTPD then copy previous packet to current packet
               so that omcid can send previous packet back to HTTPD */
            else
            {
                memcpy(pPacket, &pGponOmci->prevRespPacket[prio], len);
                rv = 0;
            }
        }
    }

//    omci_msg_dump_stats();

    if (msgType != OMCI_MSG_TYPE_DOWNLOADSECTION)
    {
        SHORT_NOTICE("%s", OMCI_BANNER_END);
    }

    return rv;
}

CmsRet omci_omcimsg_init(void)
{
    CmsRet ret = CMSRET_SUCCESS;

    /* generate the crc32 table */
    omciUtl_initCrc32Table();

    /* assgin value to OMCI object */
    ret = initOmciObject();

    /* initialize MIB Upload database */
    memset(mibUpload_mibCopy, 0, OMCI_MIB_UPLOAD_INST_MAX * sizeof(mibUpload_t));

    return ret;
}

static int omci_msg_send_notification(omciPacket *pPacket)
{
    int packetSize;
    int txBytes;

    packetSize = OMCI_PACKET_SIZE(pPacket) - OMCI_PACKET_MIC_SIZE;
    txBytes = gpon_omci_api_transmit(pPacket, packetSize);
    if ((txBytes == -1) && (errno == ENOLINK))
    {
        cmsLog_notice("OMCI Port has been deleted, continue");
    }
    else if (txBytes != packetSize)
    {
        cmsLog_error("Failed to transmit packet, size=%d, tx=%d",
          packetSize, txBytes);
        OMCID_STATS_INC(txErrors);
        return -1;
    }

    return 0;
}

static void omci_avc_sim(UINT16 meClass, UINT16 meInst, UINT16 attrMask)
{
    CmsRet ret = CMSRET_SUCCESS;
    omciMsgResult msgResult;
    omciMeInfo_t meInfo;
    void *mdmObj = NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    UINT16 unsupportMask = 0;
    omciPacket packet;

    if ((ret = _cmsLck_acquireLockWithTimeout(OMCID_LOCK_TIMEOUT))
      != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to get lock, ret=%d", ret);
        return;
    }

    msgResult = omci_msg_get_mdmObj(meClass, meInst, &meInfo, &mdmObj,
      &iidStack);
    if (msgResult != OMCI_MSG_RESULT_SUCCESS)
    {
        cmsLog_error("omci_msg_get_mdmObj() failed, "
          "meClass=%d, meInst=%d, ret=%d",
          meClass, meInst, msgResult);
        goto out;
    }

    unsupportMask = attrMask & (~meInfo.supportedMask);
    if (OMCID_PROMISC_MODE() && (unsupportMask != 0))
    {
        cmsLog_notice("ME masks: attr=0x%04X, support=0x%04X, option=0x%04X,"
          " unsupport=0x%04X",
          attrMask, meInfo.supportedMask, meInfo.optionalMask, unsupportMask);
        unsupportMask = 0;
    }

    memset(&packet, 0, sizeof(omciPacket));
    packet.msgType = OMCI_MSG_TYPE_ATTRIBUTEVALUECHANGE;
    packet.devId = OMCI_PACKET_DEV_ID_A;
    OMCI_HTONS(&packet.classNo, meClass);
    OMCI_HTONS(&packet.instId, meInst);
    OMCI_HTONS(&OMCI_PACKET_MSG(&packet)[0], attrMask);

    ret = omci_msg_copy_to_packet(&packet, OMCI_AVC_OVERHEAD, mdmObj,
      &attrMask, meClass);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("omci_msg_copy_to_packet() failed, "
          "meClass=%d, meInst=%d, attrMask=0x%04x, ret=%d",
          meClass, meInst, attrMask, ret);
    }
    else
    {
        OMCID_DUMP_PACKET(&packet, OMCI_PACKET_A_SIZE);

        if (omci_msg_send_notification(&packet) == 0)
        {
            OMCID_STATS_INC(txAvcPackets);
        }
    }
    _cmsObj_free(&mdmObj);

out:
    _cmsLck_releaseLock();
}

static void omci_alarm_sim(UINT16 meClass, UINT16 meId, UINT16 alarmNum,
  UBOOL8 setFlagB)
{
    omciPacket packet;
    UINT8 alarmMask[OMCI_PACKET_ALARM_BITMAP_SIZE];

    if (alarmNum > OMCI_PACKET_ALARM_NUM_MAX)
    {
        cmsLog_error("Invalid alarm number, valid range [0, %d]",
          OMCI_PACKET_ALARM_NUM_MAX);
        return;
    }

    memset(&packet, 0, sizeof(omciPacket));
    memset(&alarmMask, 0, sizeof(alarmMask));

    packet.msgType = OMCI_MSG_TYPE_ALARM;
    packet.devId = OMCI_PACKET_DEV_ID_A;
    OMCI_HTONS(&packet.classNo, meClass);
    OMCI_HTONS(&packet.instId, meId);

    if (setFlagB != 0)
    {
        alarmMask[alarmNum / 8] = (0x80 >> (alarmNum % 8));
        memcpy(&OMCI_PACKET_MSG(&packet)[0], &alarmMask, sizeof(alarmMask));
    }

    OMCI_PACKET_MSG(&packet)[OMCI_PACKET_A_MSG_SIZE - 1] =
      omci_alarm_getSequenceNumber();

    OMCID_DUMP_PACKET(&packet, (int)OMCI_PACKET_A_SIZE);

    if (omci_msg_send_notification(&packet) == 0)
    {
        OMCID_STATS_INC(txAlarmPackets);
    }
}

void omci_msg_gen_handler(OmciMsgGenCmd *cmdP)
{
    OmciMsgGenCmd *bodyP = cmdP;

    if (bodyP->msgType == OMCI_MSG_TYPE_ALARM)
    {
        omci_alarm_sim(bodyP->meClass, bodyP->meInst, bodyP->alarmNum,
          bodyP->flagB);
    }
    else if (bodyP->msgType == OMCI_MSG_TYPE_ATTRIBUTEVALUECHANGE)
    {
        omci_avc_sim(bodyP->meClass, bodyP->meInst, bodyP->attrMask);
    }
    else
    {
        cmsLog_error("Unsupported OMCI message type %d", bodyP->msgType);
    }
}
