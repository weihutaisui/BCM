/*
* <:copyright-BRCM:2013:proprietary:epon
* 
*    Copyright (c) 2013 Broadcom 
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
// \file TkOamApiCommon.c
// \brief the TK extension OAM common functions  
//
////////////////////////////////////////////////////////////////////////////////
#include "TkPlatform.h"
#include "TkOamCommon.h"
#include "config.h"
#include "TkOamMem.h"
#include "cms_log.h"

////////////////////////////////////////////////////////////////////////////////
/// GetSourceForFlag:  Gets the source associated with the provided OAM flag
///
/// Parameters:
/// \param flag	OAM flags
/// 
/// \return 
/// The source (interface + link) associated with the given flag
////////////////////////////////////////////////////////////////////////////////

U8 GetSourceForFlag(U16 flags)
	{
	U8  source;
	source = (U8)((flags & OamReservedFlagMask) >> OamReservedFlagShift);
    source = ((source & OamFlagSrcIfMask) << OamFlagSrcIfShift) | 
             (source >> OamFlagLinkShift);
	return source;
	} // GetSourceForFlag


void AttachOamFlag(U8 source, U16 *flags)
    {
    *flags = ((*flags) & OamFlagMask) | 
             ((source &  OnuHostIfLinkMask) << OamFlagLinkShift  | 
              (source >> OnuHostIfPhyIfSft) << OamFlagSrcIfShift) 
               << OamReservedFlagShift;
    }




////////////////////////////////////////////////////////////////////////////////
/// TkOamEthSend:  Sends an ethernet frame to a specified link
///
// Parameters:
///	\param linkId: the link to send the ethernet/oam frame;
///	\param dstAddr: the mac address of the ethernet destination
/// \param pDataBuf: the buffer pointer to the ethernet frame
/// \param dataLen: valid data lenth of the ethernet frame
/// 
/// \return 
/// STATUS.  ERROR on failure and OK on success.
////////////////////////////////////////////////////////////////////////////////

STATUS TkOamEthSend (U8 linkId, MacAddr *dstAddr, U8 *pDataBuf, U32 dataLen)
{
    MacAddr  srcAddress;
    MacAddr *srcAddr = &srcAddress;
    U16 flags ;
    OamMsg *msg = (OamMsg *)((U8 *)pDataBuf + sizeof(EthernetFrame));
    
    if ((linkId > SDK_MAX_NUM_OF_LINK) || (NULL == pDataBuf))
        {
        return (ERROR);
        } 

    if (dataLen > MAX_MSG_LEN)
    {
        cmsLog_error("The packet(Len: %d) to be sent is too long!\n", (int)dataLen);
    	return (ERROR);
    }

    if(OAM_NTOHS(((EthernetFrame *)pDataBuf)->type) != EthertypeOam)
    {
    	cmsLog_error("TkOamEthSend handles OAM frames only! \n");
    	return (ERROR);
    }
    
    /* convert linkId to reserved flag bits in OAM frames */
    flags = OAM_NTOHS(msg->flags) & OamFlagMask;	
    AttachOamFlag(linkId, &flags);
    msg->flags = htons(flags);  

    TkOsGetIfMac(linkId, (U8 *)srcAddr);
    bcopy ((U8 *)dstAddr, (U8 *)&(((EthernetFrame *)pDataBuf)->da), sizeof(MacAddr));
    bcopy ((U8 *)srcAddr, (U8 *)&(((EthernetFrame *)pDataBuf)->sa), sizeof(MacAddr));
    
    if( !TkOsDataTx(pDataBuf, (dataLen<60)?60:dataLen) )
    {
        return ERROR;
    }
	return OK;
}


