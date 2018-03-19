/*
* <:copyright-BRCM:2012:proprietary:epon
* 
*    Copyright (c) 2012 Broadcom 
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

////////////////////////////////////////////////////////////////////////////////
/// \file    CtcOamFileTransfer.c
/// \brief  Ctc2.1 firmware upgrade based TFTP and Oam
/// \new  feature
///
/// Added by nking
////////////////////////////////////////////////////////////////////////////////

#include <string.h>     // memcpy
#include "Teknovus.h"
#include "PonManager.h"
#include "Oam.h"
#include "OamUtil.h"
#include "OamIeee.h"
#include "SysInfo.h"
#include "CtcOamFileTransfer.h"
#include "CtcOam.h"
#include "Oui.h"
#include "bcm_hwdefs.h"
#include "board.h"
#include "EponDevInfo.h"
#include "bcmTag.h"
#include <sys/sysinfo.h>

#ifdef BRCM_CMS_BUILD
#include "cms_image.h"
#include "cms_mem.h"
#include "cms_boardcmds.h"
#endif

char const CODE* CODE errMessage[ctcErrCodeNums] =
     {
     "Not defined, see error message (if any)",
     "File not found",
     "Access violation",
     "Disk full or allocation exceeded",
     "Illegal TFTP operation",
     "Unknown transfer ID",
     "File already exists",
     "No such user",
     };


static CtcFwUpgradeSessionInfo ctcSession;
static LoadSize CtcFileMaxSize = 25*1024*1024;/* default, may be updated later */


PhyLlid ctcTid;
static OamUpOamRate BULK original;

// DA(6)+SA(6)+VLAN(4)+Etype(2)+Subtype(1)+OUI(3)+PT(2)+CT(2)+LEN(2)+Link(2)+
// CODE(1)+OUI(3) = 34

#define CtcUpgradeStateSet(newState)\
    /*printf("C: %bu -> %bu\n", ctcSession.state, (newState));*/\
    (ctcSession.state = (newState))


////////////////////////////////////////////////////////////////////////////////
/// CtcFwUpgradeEnterIdle:Enter idle state
///
 // Parameters:
/// \param None
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CtcFwUpgradeEnterIdle (void)
    {
#ifdef BRCM_CMS_BUILD
    // note OamFileIdle is 0, so memset includes session.idle = OamFileIdle

    if (ctcSession.imgifHandle != NULL)
    {
        img_util_abort(ctcSession.imgifHandle);
    }
    memset (&ctcSession, 0, sizeof(ctcSession));
#endif
    } // CtcFwUpgradeEnterIdle

////////////////////////////////////////////////////////////////////////////////
//extern
void CtcOamFile100MsTimer(void)
    {
#ifdef BRCM_CMS_BUILD
    if (ctcSession.maxSize == 0) // upgrade not start
        {
        return;
        }

    ctcSession.timetick++;

    if (ctcSession.timetick > CtcFwUpgradeMaxWait) // timeout
        {
        //LoadMgmtRestoreBuffer("Download timeout. ");
        CtcFwUpgradeEnterIdle();
        }
#endif
    } // CtcOamFile100MsTimer


////////////////////////////////////////////////////////////////////////////////
/// \brief  fills in and sends ack message
///
/// \param blockNum     the block num received just
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CtcOamFileSendAck (U16 blockNum)
    {
    OamCtcPayloadHead BULK * payloadHead;
    CtcFwUpgradeFileAck BULK * ackPdu;
    OamCtcExt BULK *ctc  =  (OamCtcExt BULK *)oamParser.reply.cur;

    payloadHead =(OamCtcPayloadHead BULK * )(ctc + 1);
    ackPdu = (CtcFwUpgradeFileAck BULK *)(payloadHead + 1);
    ctc->opcode = OamCtcFileUpgrade;
    payloadHead->tid = OAM_HTONS(ctcTid);
    payloadHead->length = OAM_HTONS(sizeof(OamCtcPayloadHead) +
                                            sizeof(CtcFwUpgradeFileAck));
    payloadHead->dataType = CtcTftpProData;
    ackPdu->opcode = OAM_HTONS(CtcOpFileSendAck);
    ackPdu->num = OAM_HTONS(blockNum);

    StreamSkip(&oamParser.reply, OAM_NTOHS(payloadHead->length) + 1);
    OamAdjustCredits(oamParser.dest);    
    OamTransmit();
    } // CtcOamFileSendAck


////////////////////////////////////////////////////////////////////////////////
/// \brief  sends the error codemessage
///
/// \param errCode  the erroe code
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CtcOamFileSendErr (CtcFwUpgradeErr errCode)
    {
    OamCtcPayloadHead BULK * payloadHead;
    CtcFwUpgradeFileError BULK * errPdu;
    U8 BULK len;

    OamCtcExt BULK *ctc  =  (OamCtcExt BULK *)oamParser.reply.cur;

    payloadHead =(OamCtcPayloadHead BULK * )(ctc + 1);
    errPdu = (CtcFwUpgradeFileError BULK *)(payloadHead + 1);
    ctc->opcode = OamCtcFileUpgrade;
    payloadHead->tid = OAM_HTONS(ctcTid);
    payloadHead->length = OAM_HTONS(sizeof(OamCtcPayloadHead) +
                                      sizeof(CtcFwUpgradeFileAck));
    payloadHead->dataType = CtcTftpProData;
    errPdu->opcode = OAM_HTONS(CtcOpFileError);
    errPdu->errcode = OAM_HTONS(errCode);
    len = (U8)sprintf (errPdu->errMsg, "%s%c", errMessage[errCode],0x0);
    payloadHead->length += OAM_HTONS(len);

    StreamSkip(&oamParser.reply, OAM_NTOHS(payloadHead->length) + 1);
    OamTransmit();
    } // CtcOamFileSendErr


////////////////////////////////////////////////////////////////////////////////
/// \brief  handle OAM file data packet
///
/// \param msg      the poniter of File Transfer Data Header structure
/// \param length   the length of the message format
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CtcOamFileData (const CtcFwUpgradeFileData BULK * FAST msg,
                     U16 length)
    {
    U16 BULK blockDataSize;
    blockDataSize = (length - sizeof(CtcFwUpgradeFileData));

	// TODO: How to check the version replication?
    if (OAM_NTOHS(msg->num) == (ctcSession.blockNum + 1))
        {
        if ((S16)blockDataSize >= 0)
            {
                int byteCount;

                byteCount = img_util_write(ctcSession.imgifHandle, (UINT8*)(msg+1), blockDataSize);
                if ((byteCount < 0) || (byteCount != (int)blockDataSize))
                {
                    ctcSession.lastErr = CtcErrUndefined;
                    CtcOamFileSendErr (ctcSession.lastErr);
                    CtcFwUpgradeEnterIdle();
                    return;
                }
                ctcSession.size += (LoadSize)blockDataSize;
                ctcSession.blockNum++;
                ctcSession.timetick = 0;
            } // if non-zero size message

		ctcSession.timetick = 0;
        } // if expected blockNum
    else if(OAM_NTOHS(msg->num) < (ctcSession.blockNum + 1))
        {          
        // CTC3.0 support retransmit when upgrading, so the onu 
        //will ignore the reduplicate block .	         
		ctcSession.timetick = 0;
        CtcOamFileSendAck(OAM_NTOHS(msg->num));
        return;
        }
    else
        {
        ctcSession.lastErr = CtcErrUndefined;
        CtcOamFileSendErr (ctcSession.lastErr);
        CtcFwUpgradeEnterIdle();
        return;
        }
    CtcOamFileSendAck(ctcSession.blockNum);
    } // CtcOamFileData


////////////////////////////////////////////////////////////////////////////////
/// \brief  send the response package of End Download Request activating, and
/// committing message
///
/// \param opcode   the opcode of response
/// \param ackCode  the response Code
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void CtcFwUpgradeSendRes(CtcFwUpgradeOpcode opcode, U8 ackCode)
    {
    OamCtcPayloadHead BULK * payloadHead;
    CtcFwUpgradeComm BULK * resPdu;
    OamCtcExt BULK *ctc  =  (OamCtcExt BULK *)oamParser.reply.cur;

    payloadHead =(OamCtcPayloadHead BULK * )(ctc + 1);
    resPdu = (CtcFwUpgradeComm BULK *)(payloadHead + 1);
    ctc->opcode = OamCtcFileUpgrade;
    payloadHead->tid = OAM_HTONS(ctcTid);
    payloadHead->length = OAM_HTONS(sizeof(OamCtcPayloadHead) + 3);
    switch (opcode)
        {
        case CtcOpEndDnldRes:
            payloadHead->dataType = CtcCheckFileData;
            break;
        case CtcOpActImgRes:
            payloadHead->dataType = CtcLoadRunImageData;
            break;
        case CtcOpCmtImgRes:
            payloadHead->dataType = CtcCommitImageData;
            break;
        default:
            payloadHead->dataType = CtcNumDataTypes;
            payloadHead->length = OAM_HTONS(sizeof(OamCtcPayloadHead));
            break;
        }
    resPdu->opcode = OAM_HTONS(opcode);
    resPdu->para = ackCode;

    StreamSkip(&oamParser.reply, OAM_NTOHS(payloadHead->length) + 1);
    OamTransmit();
    }// CtcFwUpgradeSendRes


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle the file write request
///
/// \param pdu  Pointer to received CTC FileUpgrade PDU
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void CtcFwUpgradeWrReq (CtcFwUpgradeFileRequest *pdu)
    {
#ifdef BRCM_CMS_BUILD
    int maxSize = 0;
	OamUpOamRate BULK newRate ={0, 0};
	struct sysinfo si;

	CtcFwUpgradeEnterIdle();
	
	ctcSession.maxSize = cmsImg_getImageFlashSize();
	if (ctcSession.maxSize == 0)
		{
        ctcSession.lastErr = CtcErrUndefined;
        CtcOamFileSendErr (ctcSession.lastErr);
		return;
		}

	sysinfo(&si);
	if (si.totalram >= 200*1024*1024) /* Physical DDR size >= 256MB */
	{
		CtcFileMaxSize = 35*1024*1024;
	}
	else if ((si.totalram > 90*1024*1024) && (si.totalram < 200*1024*1024)) /* Physical DDR size: 128M */
	{
		CtcFileMaxSize = 25*1024*1024;
	}
	else
	{
		CtcFileMaxSize = 10*1024*1024;
	}
	maxSize = (ctcSession.maxSize > CtcFileMaxSize)? CtcFileMaxSize : ctcSession.maxSize;
	printf("MemTotal: %dMB, Reserve for upgrade: %dMB\n", si.totalram/(1024*1024), maxSize/(1024*1024));

	{
		int curBootImageId = -1;
		unsigned int opts = 0;
		imgutil_open_parms_t openParams;
 	   	UBOOL8 imgIfEnabled;

		imgIfEnabled = img_util_get_incflash_mode();
		curBootImageId = devCtl_getBootedImagePartition();

		if (curBootImageId == BOOTED_PART1_IMAGE)
			opts = CMS_IMAGE_FORMAT_PART2 | CMS_IMAGE_FORMAT_NO_REBOOT;
		else if (curBootImageId == BOOTED_PART2_IMAGE)
			opts = CMS_IMAGE_FORMAT_PART1 | CMS_IMAGE_FORMAT_NO_REBOOT;
		else
		{
			ctcSession.lastErr = CtcErrDiskFull;
			CtcOamFileSendErr (ctcSession.lastErr);
			return;
		}

		memset(&openParams, 0x0, sizeof(imgutil_open_parms_t));
		openParams.maxBufSize = maxSize;
		openParams.forceWholeFlashB = (imgIfEnabled == TRUE) ? 0 : 1;
		openParams.options = opts;
		openParams.calStdCrcB = TRUE;

		ctcSession.imgifHandle = img_util_open(&openParams);
		if (ctcSession.imgifHandle == NULL)
		{
			ctcSession.lastErr = CtcErrDiskFull;
			CtcOamFileSendErr (ctcSession.lastErr);
			return;
		}
	}

	ctcSession.maxSize = maxSize;
	GetOamRate(&original);
	newRate.minRate = original.minRate;
	newRate.maxRate = FwUpgradeOamCreditsRate;
	SetOamRate(&newRate);
	CtcOamFileSendAck(0);
    return;
#else
    UNUSED(pdu);
#endif
    } // CtcFwUpgradeWrReq



////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle Tftp Protocal messages.
///
/// \param msg      Pointer to Message format data
/// \param length   the length of Message format
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void CtcTftpProdataHandle(CtcFwUpgradeOpcode BULK *msg, U16 length)
    {
    switch (StreamReadU16(&oamParser.src))
        {
        case CtcOpFileWriteReq:
             CtcFwUpgradeWrReq ((CtcFwUpgradeFileRequest *)msg);
             break;
        case CtcOpFileSendData:
             CtcOamFileData ((CtcFwUpgradeFileData *)msg, length);
             break;
        case CtcOpFileSendAck:
        case CtcOpFileError:
        default:
             break;
        }
    } // CtcTftpProdataHandle


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle the End Download Request
///
/// \param msg  Pointer to Message format data
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void EndDnldHandle(const CtcFwUpgradeEndDnld BULK * FAST msg)
    {
#ifdef BRCM_CMS_BUILD
    CtcEndDnldRes BULK ret = CtcResEndDnldNoErr;
    LoadSize BULK fileSize = (LoadSize)OAM_NTOHL(msg->fileSize);
    imgutil_img_info_t imgInfo;
    U32 imageCrc = 0;
    int rc = 0;
    int curBootImageState = -1;


    if(ctcSession.maxSize == 0)
        { // response has been sent, but may be lost, so ONU receives multi
          // End Download Request when maxSize is 0.
        CtcFwUpgradeSendRes(CtcOpEndDnldRes, ret);
        return;
        }

    //restore the OamCreditsPerTick
    SetOamRate(&original);

    rc = img_util_get_imginfo(ctcSession.imgifHandle, IMGINFO_SRC_CALCOMBO,
      IMGUTIL_IMGINFO_BIT_SIZE, &imgInfo);
    if ((rc != 0) || (fileSize != imgInfo.size))
    {
    ret = CtcResEndDnldParaError;
    goto exit;
    }

    rc = img_util_get_imginfo(ctcSession.imgifHandle, IMGINFO_SRC_CALCOMBO,
      IMGUTIL_IMGINFO_BIT_CRC, &imgInfo);
    if (rc == 0)
    {
    imageCrc = imgInfo.crc;
    }

    EponDevSetImageCrcBeforeCommit(imageCrc);
    if (imageCrc == EponDevGetImageCrc())
    {
    goto exit;
    }
            
    curBootImageState = devCtl_getImageState();

    if (ret == CtcResEndDnldNoErr)
    {
    devCtl_setImageState(BOOT_SET_OLD_IMAGE);
    rc = img_util_close(ctcSession.imgifHandle, NULL, NULL);
    if (rc != 0)
    ret = CtcResEndDnldUnsupport;

    ctcSession.imgifHandle = NULL;
    devCtl_setImageState(curBootImageState);
    }

exit:
    CtcFwUpgradeSendRes(CtcOpEndDnldRes, ret);
    CtcFwUpgradeEnterIdle();
    printf("\nSoftware Image Upgrade finished with %s !\n", (CtcResEndDnldNoErr == ret) ? "SUCC" : "FAIL");
#else
    UNUSED(msg);
#endif
    }// EndDnldHandle


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle the Activate Image Request
///
/// \param msg  Pointer to Message format data
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void ActivateImg(const CtcFwUpgradeComm BULK * FAST msg)
    {
#ifdef BRCM_CMS_BUILD
    CtcActComAck BULK ret = CtcAckActComOk;
	int curBootImageId = -1;
	int newBootImageState = -1;

    if (msg->para != ActComFlags)
        {
        ret = CtcAckActComParaError;
        }
	else
		{
		// Put it here in order to meet the 1 second response time.
		CtcFwUpgradeSendRes(CtcOpActImgRes, ret);

		printf("Activate Image in Progress\n");
        if (EponDevGetImageCrcBeforeCommit() == EponDevGetImageCrc())
            {
            goto exit;
            }

		curBootImageId = devCtl_getBootedImagePartition();
		if (curBootImageId == BOOTED_PART1_IMAGE)
			newBootImageState = BOOT_SET_PART2_IMAGE_ONCE;
		else if (curBootImageId == BOOTED_PART2_IMAGE)
			newBootImageState = BOOT_SET_PART1_IMAGE_ONCE;
		else
			printf("%s: %d, strange boot image id\n", __FUNCTION__, __LINE__);

		if (ret == CtcAckActComOk)
			devCtl_setImageState(newBootImageState);
		}

exit:
	system("reboot \n");
	exit(0);
    return;
#endif
    }// ActivateImg


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle the Commit image Request
///
/// \param msg  Pointer to Message format data
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
static
void CommitImg(const CtcFwUpgradeComm BULK * FAST msg)
    {
#ifdef BRCM_CMS_BUILD
    CtcActComAck BULK ret = CtcAckActComOk;
	int curBootImageId = -1;
	int newBootImageState = -1;
	int curBootImageState = -1;

    if (msg->para != ActComFlags)
        {
        ret = CtcAckActComParaError;
        }
	else
		{
		// Put it here in order to meet the 1 second response time.
		// May have problem if there is continous OAM request right after it
		CtcFwUpgradeSendRes(CtcOpCmtImgRes, ret);

		printf("Commit Image in Progress\n");
        if (EponDevGetImageCrcBeforeCommit() == EponDevGetImageCrc())
            {
            goto exit;
            }
        EponDevSetImageCrc(EponDevGetImageCrcBeforeCommit());
		curBootImageId = devCtl_getBootedImagePartition();
		curBootImageState = devCtl_getImageState();
		// For U1/U2/U3/U4, please refer to Chapter 15.3 in "CTC EPON Equipment Technical Requirements V3.0 EN"
		// U1   ----->   U4
		if ((curBootImageId == BOOTED_PART1_IMAGE) && (curBootImageState == BOOT_SET_PART1_IMAGE))
			newBootImageState = BOOT_SET_PART2_IMAGE;
		// U4   ----->   U1
		else if ((curBootImageId == BOOTED_PART1_IMAGE) && (curBootImageState != BOOT_SET_PART1_IMAGE))
			newBootImageState = BOOT_SET_PART1_IMAGE;
		// U2   ----->   U3
		else if ((curBootImageId == BOOTED_PART2_IMAGE) && (curBootImageState == BOOT_SET_PART2_IMAGE))
			newBootImageState = BOOT_SET_PART1_IMAGE;
		// U3   ----->   U2
		else if ((curBootImageId == BOOTED_PART2_IMAGE) && (curBootImageState != BOOT_SET_PART2_IMAGE))
			newBootImageState = BOOT_SET_PART2_IMAGE;
		else
			printf("%s: %d, strange boot image id\n", __FUNCTION__, __LINE__);

		if (ret == CtcAckActComOk)
			devCtl_setImageState(newBootImageState);
		}

exit:
	printf("Commit Image finished\n");
    return;
#endif
    }// CommitImg


////////////////////////////////////////////////////////////////////////////////
/// \brief  Handle all Non-Tftp Protocal messages.
///
/// \param msg  Pointer to Message format data
///
/// \return
/// Nothing
////////////////////////////////////////////////////////////////////////////////
static
void CtcNonTftpDataHandle(const CtcFwUpgradeOpcode BULK *msg)
    {
    switch (StreamReadU16(&oamParser.src))
        {
        case CtcOpEndDnldReq:
            EndDnldHandle((CtcFwUpgradeEndDnld *)msg);
            break;
        case CtcOpActImgReq:
            ActivateImg((CtcFwUpgradeComm *)msg);
            break;
        case CtcOpCmtImgReq:
            CommitImg((CtcFwUpgradeComm *)msg);
            break;
        case CtcOpEndDnldRes:
        case CtcOpActImgRes:
        case CtcOpCmtImgRes:
        default:
            printf("Error, non-Illegal Opcode !!!\n");
            break;
        }
    }// CtcNonTftpDataHandle


////////////////////////////////////////////////////////////////////////////////
//extern
void OamCtcHandleFileUpgrade(OamCtcPayloadHead BULK *pdu)
    {
    U16 BULK length;
    length = OAM_NTOHS(pdu->length) - sizeof(OamCtcPayloadHead);
    ctcTid = OAM_NTOHS(pdu->tid);
    StreamSkip(&oamParser.src, sizeof(OamCtcPayloadHead));
    switch (pdu->dataType)
        {
        case CtcTftpProData:
            CtcTftpProdataHandle((CtcFwUpgradeOpcode BULK *)(pdu+1), length);
            break;
        case CtcCheckFileData:
        case CtcLoadRunImageData:
        case CtcCommitImageData:
            CtcNonTftpDataHandle((CtcFwUpgradeOpcode BULK *)(pdu+1));
            break;
        default:
            printf("Error, datatype %d!!!\n", pdu->dataType);
            break;
        }
    }// OamCtcHandleFileUpgrade


////////////////////////////////////////////////////////////////////////////////
//extern
void CtcFwUpgradePoll(void)
    {
    return;
    }


////////////////////////////////////////////////////////////////////////////////
//extern
void CtcFwUpgradeInit (void)
    {
    CtcFwUpgradeEnterIdle();
    } // CtcFwUpgradeInit


// end CtcOamFileTransfer.c

