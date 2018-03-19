/***********************************************************************
 *
 *  Copyright (c) 2008-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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

#ifndef __CGI_OMCI_H__
#define __CGI_OMCI_H__

#include <stdio.h>
#include <fcntl.h>
#include "cms.h"
#include "cms_msg.h"
#include "omci_api.h"
#include "omci_ipc.h"

/********************** Global Types ****************************************/

#if defined(__cplusplus)
extern "C" {
#endif

//#define OMCI_FROM_MDM
//#define OMCI_CFG_DEBUG

#define NUM_PACKETS_MAX 16
#define NUM_RETRIES_MAX 5
#define OMCI_CONF_NAME "omci.conf"
#define OMCI_DIR "/var/omci"
#define OMCI_MSG_MACRO_ON  "Record is ON: OMCI command is recorded to memory."
#define OMCI_MSG_MACRO_OFF "Record is OFF: OMCI command is NOT recorded to memory."
#define FN_UPLOAD          "filename="
#define OMCI_RAW_BUFFER_SIZE 256

typedef struct
{
   struct timeval timestamp;
   omciPacket packet;
} omci_raw_buffer_row_t;


typedef struct
{
   char *name;
   UINT16 oid;
   UINT16 parentOid;
   UINT16 classId;
   UINT16 setByCreateMask; /* Note that the Managed Entity identifier, which is
                               an attribute of each managed entity, has no
                               corresponding bit in the Set by Create attribute
                               mask. Thus, the attributes are counted starting
                               from the most significant bit of this mask, which
                               corresponds to the first attribute after the
                               Managed Entity identifier. */
} omciCreateObject_t;

/* macro state
 * OMCI_MARCO_OFF -- configuration command is executed right away
 * OMCI_MARCO_ON -- configuration command is saved to file and does not execute
 * OMCI_MARCO_RUN -- execute all commands that are currenly saved in file
 * OMCI_MARCO_SAVE -- backup curent configuration commands in file to PC
 * OMCI_MARCO_RESTORE -- load file from PC and execute configuration commands in this file
 */
typedef enum {
    OMCI_MARCO_OFF = 0,
    OMCI_MARCO_ON,
    OMCI_MARCO_RUN,
    OMCI_MARCO_SAVE,
    OMCI_MARCO_RESTORE
} omciMacroState;

typedef struct
{
    MdmObjectId _oid;
    UINT32 managedEntityId;
} OmciGenObject;

/* public functions */
CmsRet cgiOmci_handleResponse(const CmsMsgHeader *msgRes);
CmsRet cgiOmci_handlePmdDebug(const CmsMsgHeader *msgRes);
CmsRet cgiOmci_sendRequest(omciMsgType msgType);
void cgiOmciCfg(char *query, FILE *fs);
void cgiOmciRaw(char *query, FILE *fs);
void cgiOmci_queueRawPacket(omciPacket *packet);
void cgiOmciCreate(char *query, FILE *fs);
void cgiOmciGetNext(char *query, FILE *fs);
void cgiOmciMacro(char *query, FILE *fs);
int cgiOmciUpload( FILE *fs);
int cgiOmciDownloadPre(FILE *fs, int downloadLen);
void cgiOmciDownloadPost(void);
CmsRet cgiOmci_sendDownloadSection(omciPacket *packetRes);
void cgiOmci_getResultMessage(char *msg, UINT16 msgSize);
CmsRet cgiOmci_setParameterValue(omciPacket *packet, SINT32 *msgIndex,
   const OmciObjectInfo_t pathDesc, const SINT32 paramIndex, const char *paramValue);
void cgiOmci_makePathToOmci(char *buf, UINT32 bufLen, const char *fileName);
void cgiOmci_writeOmciResult(FILE *fs, char *message);
void cgiOmci_writeOmciEnd(FILE *fs);
void cgiOmci_addCrc(omciPacket *packet, CmsEntityId eid);
CmsRet cgiOmci_getParameterValue(const MdmObjectId oid, 
  const InstanceIdStack iidStack, const SINT32 paramIndex,
  char *paramValue, const UINT16 paramSize);
CmsRet cgiOmci_getMeId(const MdmObjectId oid,
  const InstanceIdStack iidStack, UINT16 *meIdP);


#if defined(__cplusplus)
}
#endif

#endif   // __CGI_CERT_H__
