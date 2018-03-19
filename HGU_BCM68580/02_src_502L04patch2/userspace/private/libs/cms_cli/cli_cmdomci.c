/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
 *  <:label-BRCM:2011:proprietary:standard
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
 *
 ************************************************************************/

/** command driven CLI code goes into this file */

#ifdef SUPPORT_CLI_CMD

#ifdef DMP_X_ITU_ORG_GPON_1 /* aka SUPPORT_OMCI */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "cms.h"
#include "cms_log.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"

#include "cli.h"
#include "omci_api.h"
#include "omciutl_cmn.h"
#include "rut_omci.h"


#define CLIAPP_LOCK_TIMEOUT 6000 /* ms */

#define MSG_RECV_TIMEOUT_MS 60000

#define MAX_OPTS 12

#define GRAB_END_STR "[GRAB:END]\n"

#define GRAB_LINE_SIZE_MAX 1024

#define SEND_FILE_OVHD 64

#define ETH_PORT_TYPE_RG    "rg"
#define ETH_PORT_TYPE_ONT    "ont"

typedef struct {
    union {
        UINT32 all;
        struct {
            UINT8  state;
            UINT8  pmId;
            UINT16 portId;
        } info;
    } omcipmDebug;
} CLI_OMCI_PM_DEBUG;

static const char omciusage[] = "\nUsage:\n";

static const char omcigrab[] =
"        omci grab --filetype <txt|bin> --filename <full_dest_file_name>\n";

static const char omcicapture[] =
"        omci capture control --state <on [--overwrite <y|n>] | off>\n\
        omci capture replay --state <on [--filename <full_src_file_name>] | off>\n\
        omci capture view [--filename <full_src_file_name>] \n\
        omci capture save [--filename <partial_dest_file_name>]\n\
        omci capture restore [--filename <partial_src_file_name>]\n";

static const char omcisend[] =
"        omci send --filename <file_name>\n";

static const char omcitcont[] =
"        omci tcont --portmax <0..32> --startid <0..65535>\n\
        omci tcont --port <0..31> --policy <sp|wrr>\n";

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
static const char omcieth[] =
"        omci eth --portmax <0..8> --startid <0..65535> --veip <0|1>\n\
        omci eth --port <0..7> --type <rg|ont>\n\
        omci eth --show\n";
#else   /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */
static const char omcieth[] =
"        omci eth --portmax <0..8> --startid <0..65535>\n";
#endif   /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

static const char omcimibsync[] =
    "        omci mibsync --help\n\
        omci mibsync --get\n";

static const char omciunipathmode[] =
"        omci unipathmode --help\n\
        omci unipathmode --status\n\
        omci unipathmode --mode <on|off>\n";

static const char omcipromiscmode[] =
"        omci promiscmode --help\n\
        omci promiscmode --status\n\
        omci promiscmode --mode <on|off>\n";

static const char omcirawmode[] =
"        omci rawmode --help\n\
        omci rawmode --status\n\
        omci rawmode --mode <on|off>\n";

static const char omcitmoption[] =
"        omci tmoption --help\n\
        omci tmoption --status\n\
        omci tmoption --usmode <0(priority)|1(rate)|2(priority and rate)>\n\
        omci tmoption --dsqueue <0(skip)|1(pbit)|2(extended pbit)>\n";

static const char omcibridge[] =
"        omci bridge --help\n\
        omci bridge --status\n\
        omci bridge --fwdmask <0..65535>\n";

static const char omcimisc[] =
"        omci misc --help\n\
        omci misc --status\n\
        omci misc --extvlandefault <on|off>\n";

#if defined(DMP_X_ITU_ORG_VOICE_1)
static const char omcidebug[] =
"        omci debug --help\n\
        omci debug --status\n\
        omci debug --dump <class ID>\n\
        omci debug --mkerr <swdlerr1(Window hole error)|swdlerr2(Download section msg rsp error)|swdlerr3(SW image CRC error)>\n\
        omci debug --module <all|omci|model|vlan|flow|rule|mcast|voice|file> --state <on|off>\n\
        omci debug --info <flag>\n";
static const char omcivoice[] =
"        omci voice --help\n\
        omci voice --status\n\
        omci voice --model <0(omci path) | 1(ip path)\n";
#else    // DMP_X_ITU_ORG_VOICE_1
static const char omcidebug[] =
"        omci debug --help\n\
        omci debug --status\n\
        omci debug --dump <class ID>\n\
        omci debug --mkerr <swdlerr1(Window hole error)|swdlerr2(Download section msg rsp error)|swdlerr3(SW image CRC error)>\n\
        omci debug --module <all|omci|model|vlan|flow|rule|mcast|file> --state <on|off>\n\
        omci debug --info <flag>\n";
#endif    // DMP_X_ITU_ORG_VOICE_1

static const char omcipmusage[] = "\nUsage:\n";

static const char omcipmdebug[] =
"        omcipm debug --help\n\
        omcipm debug --pmid <1..9> --portid <0..31> --state <on|off>\n\
        omcipm --getAlarmSeq\n\
        omcipm --setAlarmSeq <1..255>\n";

static const char omcimcast[] =
"        omci mcast --help\n\
        omci mcast --status\n\
        omci mcast --hostctrl <on|off>\n\
        omci mcast --joinfwd <on|off>\n";

static const char omcitest[] =
"        omci test --genalarm --meclass <class ID> --meid <meid> --num <alarmnum> --state <0|1>\n\
        omci test --genavc --meclass <class ID> --meid <meid> --attrmask <attrmask>\n";

static const char omcisettable[] =
"        omci settable --tcid <tcid> --meclass <class ID> --meid <meid> --index <0-based attrindex> --val <entry>\n";

char    FullFilename[256]   = "";
char    UsbDir1[]           = "/mnt/usb1_1/";
char    UsbDir2[]           = "/mnt/usb2_2/";

/***************************************************************************
 * Function Name: cmdOmciHelp
 * Description  : Prints help information about the OMCI commands
 ***************************************************************************/
static void cmdOmciHelp(char *help)
{
    if(help == NULL || strcasecmp(help, "--help") == 0)
    {
#if defined(DMP_X_ITU_ORG_VOICE_1)
       printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", 
              omciusage, omcigrab, omcisend, omcitcont,
              omcieth, omcimibsync, omciunipathmode, omcipromiscmode, omcirawmode,
              omcitmoption, omcidebug, omcicapture, omcimcast, omcitest,
              omcivoice, omcisettable, omcibridge, omcimisc
              );
#else
       printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", 
              omciusage, omcigrab, omcisend, omcitcont,
              omcieth, omcimibsync, omciunipathmode, omcipromiscmode, omcirawmode,
              omcitmoption, omcidebug, omcicapture, omcimcast, omcitest,
              omcisettable, omcibridge, omcimisc
              );
#endif /* DMP_X_ITU_ORG_VOICE_1 */
    }
    else if(strcasecmp(help, "grab") == 0)
    {
        printf("%s%s", omciusage, omcigrab);
    }
    else if(strcasecmp(help, "send") == 0)
    {
        printf("%s%s", omciusage, omcisend);
    }
    else if(strcasecmp(help, "tcont") == 0)
    {
        printf("%s%s", omciusage, omcitcont);
    }
    else if(strcasecmp(help, "eth") == 0)
    {
        printf("%s%s", omciusage, omcieth);
    }
   else if(strcasecmp(help, "mibsync") == 0)
   {
       printf("%s%s", omciusage, omcimibsync);
   }
    else if(strcasecmp(help, "unipathmode") == 0)
    {
        printf("%s%s", omciusage, omciunipathmode);
    }
    else if(strcasecmp(help, "promiscmode") == 0)
    {
        printf("%s%s", omciusage, omcipromiscmode);
    }
    else if(strcasecmp(help, "rawmode") == 0)
    {
        printf("%s%s", omciusage, omcirawmode);
    }
    else if(strcasecmp(help, "tmoption") == 0)
    {
        printf("%s%s", omciusage, omcitmoption);
    }
    else if(strcasecmp(help, "debug") == 0)
    {
        printf("%s%s", omciusage, omcidebug);
    }
    else if(strcasecmp(help, "capture") == 0)
    {
        printf("%s%s", omciusage, omcicapture);
    }
    else if(strcasecmp(help, "mcast") == 0)
    {
	    printf("%s%s", omciusage, omcimcast);
    }
    else if(strcasecmp(help, "test") == 0)
    {
	    printf("%s%s", omciusage, omcitest);
    }
    else if(strcasecmp(help, "settable") == 0)
    {
	    printf("%s%s", omciusage, omcisettable);
    }
    else if(strcasecmp(help, "bridge") == 0)
    {
	    printf("%s%s", omciusage, omcibridge);
    }
    else if(strcasecmp(help, "misc") == 0)
    {
	    printf("%s%s", omciusage, omcimisc);
    }
#if defined(DMP_X_ITU_ORG_VOICE_1)
    else if(strcasecmp(help, "voice") == 0)
    {
	    printf("%s%s", omciusage, omcivoice);
    }
#endif
}


/***************************************************************************
 * Function Name: cmdOmciCaptureControl
 *
 * Description  : Processes the OMCI msg capture state command from the CLI
                  that controls the capturing of OMCI msgs from the OLT.

                  Capture can only be performed when other features that
                  access the internal capture file are not active.  This
                  includes playback, display, download and upload.
 ***************************************************************************/
static CmsRet cmdOmciCaptureControl (char *pStateOpt, char *pStateArg, char *pOvrOpt, char *pOvrArg)
{

    CmsMsgHeader
        MsgHdr;

    CmsRet Ret = CMSRET_SUCCESS;

    MsgHdr.dst                  = EID_OMCID;
    MsgHdr.src                  = EID_CONSOLED;
    MsgHdr.dataLength           = 0;
    MsgHdr.flags.all            = 0;
    MsgHdr.flags.bits.request   = TRUE;
    MsgHdr.next                 = 0;
    MsgHdr.sequenceNumber       = 0;
    MsgHdr.wordData             = 0;

    if ( (NULL != pStateOpt) && (NULL != pStateArg))
    {
        if (strcasecmp(pStateOpt, "--state") == 0)
        {
            if (strcasecmp(pStateArg, "on") == 0)
            {
                // send capture ON msg via CMS
                MsgHdr.type = CMS_MSG_OMCI_CAPTURE_STATE_ON;

                if (NULL != pOvrOpt)
                {
                    if (strcasecmp(pOvrOpt, "--overwrite") == 0)
                    {
                        if (NULL != pOvrArg)
                        {
                            if (strcasecmp(pOvrArg, "y") == 0)
                            {
                                MsgHdr.wordData=TRUE;
                            }
                            else if (strcasecmp(pOvrArg, "n") == 0)
                            {
                                MsgHdr.wordData=FALSE;
                            }
                            else
                            {
                                // Invalid argument
                                cmsLog_error("omci capture control: Invalid argument\n");
                                cmdOmciHelp("capture");
                                Ret = CMSRET_INVALID_ARGUMENTS;
                            }
                        }
                        else
                        {
                            // Invalid argument
                            cmsLog_error("omci capture control: Invalid argument\n");
                            cmdOmciHelp("capture");
                            Ret = CMSRET_INVALID_ARGUMENTS;
                        }
                    }
                    else
                    {
                        // Invalid argument
                        cmsLog_error("omci capture control: Invalid argument\n");
                        cmdOmciHelp("capture");
                        Ret = CMSRET_INVALID_ARGUMENTS;
                    }
                }
            }
            else if (strcasecmp(pStateArg, "off") == 0)
            {
                // send capture OFF msg via CMS
                MsgHdr.type = CMS_MSG_OMCI_CAPTURE_STATE_OFF;
            }
            else
            {
                // Invalid argument
                cmsLog_error("omci capture control: Invalid argument\n");
                cmdOmciHelp("capture");
                Ret = CMSRET_INVALID_ARGUMENTS;
            }
        }
        else
        {
            // Missing argument
            cmsLog_error("omci capture control: Missing argument\n");
            cmdOmciHelp("capture");
            Ret = CMSRET_INVALID_ARGUMENTS;
        }
    }
    else
    {
        // Invalid argument
        cmsLog_error("omci capture control: Invalid argument\n");
        cmdOmciHelp("capture");
        Ret = CMSRET_INVALID_ARGUMENTS;
    }
    // send msg if processed valid
    if (CMSRET_SUCCESS == Ret)
    {
        Ret = cmsMsg_send(cliPrvtMsgHandle, &MsgHdr);
    }

    return Ret;

}

/***************************************************************************
 * Function Name: cmdOmciCaptureReplay
 *
 * Description  : Processes the OMCI msg playback command from the CLI that
                  controls the playback of OMCI msgs from a file.

                  Playback can only be performed when other features that
                  access the internal capture file are not active.  This
                  includes Capture, display, download and upload.
 ***************************************************************************/
static CmsRet cmdOmciCaptureReplay (char *pStateOpt, char *pStateArg, char *pFilenameOpt, char *pFilenameArg)
{

    CmsRet Ret = CMSRET_SUCCESS;

    UINT16 MsgSize = sizeof(CmsMsgHeader) + 128;

    char Buf[MsgSize],
        * pData = Buf + sizeof(CmsMsgHeader);

    CmsMsgHeader *  pMsgHdr = (CmsMsgHeader *)Buf;

    static FILE
        *pFileHandle = NULL;

    pMsgHdr->dst                    = EID_OMCID;
    pMsgHdr->src                    = EID_CONSOLED;
    pMsgHdr->dataLength             = 128;
    pMsgHdr->flags.all              = 0;
    pMsgHdr->flags.bits.request     = TRUE;
    pMsgHdr->next                   = 0;
    pMsgHdr->sequenceNumber         = 0;
    pMsgHdr->wordData               = 0;

    if ( (NULL != pStateOpt) && (NULL != pStateArg))
    {
        if (strcasecmp(pStateOpt, "--state") == 0)
        {
            if (strcasecmp(pStateArg, "on") == 0)
            {
                // send capture replay ON msg via CMS
                pMsgHdr->type = CMS_MSG_OMCI_CAPTURE_REPLAY_ON;

                // check for user provided file name
                if (NULL != pFilenameOpt)
                {
                    if (strcasecmp(pFilenameOpt, "--filename") == 0)
                    {
                        if (NULL != pFilenameArg)
                        {
                            if (strlen(pFilenameArg) > 128)//check string length
                            {
                                printf("omci capture replay: Invalid filename argument %s too long\n", pFilenameArg);
                                Ret = CMSRET_INVALID_ARGUMENTS;
                            }
                            else
                            {
                                pFileHandle = fopen(pFilenameArg, "r");

                                // file is valid if we opened it so go ahead and process it
                                if (NULL != pFileHandle)
                                {
                                    pMsgHdr->wordData = TRUE;
                                    fclose (pFileHandle);
                                    strcat (pData, pFilenameArg);
                                }
                                else
                                {
                                    printf("omci capture replay: Invalid filename argument, can't open file %s\n", pFilenameArg);
                                    Ret = CMSRET_INVALID_ARGUMENTS;
                                }
                            }
                        }
                        else
                        {
                            // Invalid argument
                            cmsLog_error("omci capture replay: Missing filename argument\n");
                            cmdOmciHelp("capture");
                            Ret = CMSRET_INVALID_ARGUMENTS;
                        }
                    }
                    else
                    {
                        // Invalid argument
                        cmsLog_error("omci capture replay: Invalid filename option\n");
                        cmdOmciHelp("capture");
                        Ret = CMSRET_INVALID_ARGUMENTS;
                    }
                }
            }
            else if (strcasecmp(pStateArg, "off") == 0)
            {
                // send capture replay OFF msg via CMS
                pMsgHdr->type = CMS_MSG_OMCI_CAPTURE_REPLAY_OFF;
                pFileHandle = NULL;
            }
            else
            {
                // Invalid argument
                cmsLog_error("omci capture replay: Invalid argument\n");
                cmdOmciHelp("capture");
                Ret = CMSRET_INVALID_ARGUMENTS;
                pFileHandle = NULL;
            }
        }
        else
        {
            // Missing argument
            cmsLog_error("omci capture replay: Missing argument\n");
            cmdOmciHelp("capture");
            Ret = CMSRET_INVALID_ARGUMENTS;
        }
    }
    else
    {
        // Invalid argument
        cmsLog_error("omci capture replay: Invalid argument\n");
        cmdOmciHelp("capture");
        Ret = CMSRET_INVALID_ARGUMENTS;
    }
    // send msg if processed valid
    if (CMSRET_SUCCESS == Ret)
    {
        Ret = cmsMsg_send(cliPrvtMsgHandle, pMsgHdr);
    }

    return Ret;

}

/***************************************************************************
 * Function Name: cmdOmciCaptureView
 *
 * Description  : Processes the OMCI msg show command from the CLI that
                  controls the display of OMCI msgs from a file.

                  Display can only be performed when other features that
                  access the internal capture file are not active.  This
                  includes Capture, Playback, download and upload.

                  this command supports an optional filename option and arg
                  that allows the user to view from a filename different from
                  the internal file.
 ***************************************************************************/
static CmsRet cmdOmciCaptureView (char *pFilenameOpt, char *pFilenameArg)
{
    CmsRet Ret = CMSRET_SUCCESS;

    UINT16 MsgSize = sizeof(CmsMsgHeader) + 128;

    char Buf[MsgSize],
        * pData = Buf + sizeof(CmsMsgHeader);

    CmsMsgHeader *  pMsgHdr = (CmsMsgHeader *)Buf;

    static FILE
        *pFileHandle = NULL;

    pMsgHdr->dst                    = EID_OMCID;
    pMsgHdr->src                    = EID_CONSOLED;
    pMsgHdr->dataLength             = 128;
    pMsgHdr->flags.all              = 0;
    pMsgHdr->flags.bits.request     = TRUE;
    pMsgHdr->next                   = 0;
    pMsgHdr->sequenceNumber         = 0;
    pMsgHdr->wordData               = 0;

    // send capture ON msg via CMS
    pMsgHdr->type = CMS_MSG_OMCI_CAPTURE_VIEW;

    if (NULL != pFilenameOpt)
    {
        if (strcasecmp(pFilenameOpt, "--filename") == 0)
        {
            if (NULL != pFilenameArg)
            {
                if (strlen(pFilenameArg) > 128)//check string length
                {
                    printf("omci capture view: Invalid filename argument %s too long\n", pFilenameArg);
                    Ret = CMSRET_INVALID_ARGUMENTS;
                }
                else
                {
                    pFileHandle = fopen(pFilenameArg, "r");

                    // file is valid if we opened it so go ahead and process it
                    if (NULL != pFileHandle)
                    {
                        pMsgHdr->wordData = TRUE;
                        fclose (pFileHandle);
                        strcat (pData, pFilenameArg);
                    }
                    else
                    {
                        printf("omci capture view: Invalid filename argument, can't open file %s\n", pFilenameArg);
                        Ret = CMSRET_INVALID_ARGUMENTS;
                    }
                }
            }
            else
            {
                // Missing argument
                printf("omci capture view: Missing filename argument\n");
                Ret = CMSRET_INVALID_ARGUMENTS;
                cmdOmciHelp("capture");
            }
        }
        else
        {
            // Invalid argument
            printf("omci capture view: Invalid argument\n");
            cmdOmciHelp("capture");
            Ret = CMSRET_INVALID_ARGUMENTS;
        }
    }

    // send msg if processed valid
    if (CMSRET_SUCCESS == Ret)
    {
        Ret = cmsMsg_send(cliPrvtMsgHandle, pMsgHdr);
    }

    return Ret;
}


/***************************************************************************
 * Function Name: cmdOmciCaptureSave
 *
 * Description  : Processes the OMCI download command from the CLI that
                  controls the downloading of the OMCI msg capture file from
                  the host.

                 Save can only be performed when other features that
                 access the internal capture file are not active.  This
                 includes Capture, Playback, display and upload.
 ***************************************************************************/
static CmsRet cmdOmciCaptureSave (char *pFilenameOpt, char * pFilenameArg)
{
    CmsRet Ret = CMSRET_SUCCESS;

    UINT16 MsgSize = sizeof(CmsMsgHeader) + 128;

    char Buf[MsgSize],
        * pData = Buf + sizeof(CmsMsgHeader);

    CmsMsgHeader *  pMsgHdr = (CmsMsgHeader *)Buf;

    pMsgHdr->dst                 = EID_OMCID;
    pMsgHdr->src                 = EID_CONSOLED;
    pMsgHdr->dataLength          = 128;
    pMsgHdr->flags.all           = 0;
    pMsgHdr->flags.bits.request  = TRUE;
    pMsgHdr->next                = 0;
    pMsgHdr->sequenceNumber      = 0;
    pMsgHdr->wordData            = 0;

    // send download msg via CMS
    pMsgHdr->type = CMS_MSG_OMCI_CAPTURE_DOWNLOAD;

    if (NULL != pFilenameOpt)
    {
        if (strcasecmp(pFilenameOpt, "--filename") == 0)
        {
            if (NULL != pFilenameArg)
            {
                pMsgHdr->wordData = TRUE;
                strcat (pData, pFilenameArg);
            }
            else
            {
                // Missing argument
                cmsLog_error("omci capture save: Missing filename argument\n");
                Ret = CMSRET_INVALID_ARGUMENTS;
                cmdOmciHelp("capture");
            }
        }
        else
        {
            // Invalid argument
            cmsLog_error("omci capture save: Invalid argument\n");
            cmdOmciHelp("capture");
            Ret = CMSRET_INVALID_ARGUMENTS;
        }
    }

    if (CMSRET_SUCCESS == Ret)
    {
        Ret = cmsMsg_send(cliPrvtMsgHandle, pMsgHdr);
    }

    return Ret;
}

/***************************************************************************
 * Function Name: cmdOmciRestore
 *
 * Description  : Processes the OMCI upload command from the CLI that
                  controls the downloading of the OMCI msg capture file from
                  the host.

                  Restore can only be performed when other features that
                  access the internal capture file are not active.  This
                  includes Capture, Playback, display and Save.
 ***************************************************************************/
static CmsRet cmdOmciCaptureRestore (char *pFilenameOpt, char * pFilenameArg)
{
    CmsRet Ret = CMSRET_SUCCESS;

    UINT16 MsgSize = sizeof(CmsMsgHeader) + 128;

    char Buf[MsgSize],
        * pData = Buf + sizeof(CmsMsgHeader);

    CmsMsgHeader *  pMsgHdr = (CmsMsgHeader *)Buf;

    pMsgHdr->dst                    = EID_OMCID;
    pMsgHdr->src                    = EID_CONSOLED;
    pMsgHdr->dataLength             = 128;
    pMsgHdr->flags.all              = 0;
    pMsgHdr->flags.bits.request     = TRUE;
    pMsgHdr->next                   = 0;
    pMsgHdr->sequenceNumber         = 0;
    pMsgHdr->wordData               = 0;

    // send upload msg via CMS
    pMsgHdr->type = CMS_MSG_OMCI_CAPTURE_UPLOAD;

    if (NULL != pFilenameOpt)
    {

        if (strcasecmp(pFilenameOpt, "--filename") == 0)
        {

            if (NULL != pFilenameArg)
            {

                pMsgHdr->wordData = TRUE;
                strcat (pData, pFilenameArg);

            }
            else
            {
                // Missing argument
                cmsLog_error("omci capture restore: Missing filename argument\n");
                Ret = CMSRET_INVALID_ARGUMENTS;
                cmdOmciHelp("capture");
            }
        }
        else
        {
            // Invalid argument
            cmsLog_error("omci capture restore: Invalid argument\n");
            cmdOmciHelp("capture");
            Ret = CMSRET_INVALID_ARGUMENTS;
        }
    }

    if (CMSRET_SUCCESS == Ret)
    {
        Ret = cmsMsg_send(cliPrvtMsgHandle, pMsgHdr);
    }

    return Ret;
}


/***************************************************************************
 * Function Name: cmdOmciGrab
 * Description  : Grabs the contents of a binary file from CONSOLE Daemon
 ***************************************************************************/
static CmsRet cmdOmciGrab(char *pFileTypeOpt, char *pFileType, char *pFileNameOpt,char *pFileName)
{

    CmsRet ret = CMSRET_SUCCESS;

    if ((NULL != pFileTypeOpt) && (NULL != pFileType) && (NULL != pFileNameOpt) && (NULL != pFileName))
    {
        // filename opt and arg apply to both filetype options so eval up front
        if ( (NULL != pFileName) && (strcasecmp(pFileNameOpt, "--filename") == 0 ))
        {
            // file options are good so process if file type are good
            if (strcasecmp(pFileType, "txt") == 0)
            {
                FILE *outputFile;
                char *lineBuf;

                if(pFileName == NULL)
                {
                    cmdOmciHelp("grab");
                    ret = CMSRET_INVALID_FILENAME;
                }
                else
                {
                    outputFile = fopen(pFileName, "w");
                    if(outputFile == NULL)
                    {
                        cmsLog_error("Failed to Open %s: %s\n", pFileName, strerror(errno));
                        ret = CMSRET_INVALID_ARGUMENTS;
                    }
                    else
                    {
                        lineBuf = cmsMem_alloc(GRAB_LINE_SIZE_MAX, ALLOC_ZEROIZE);

                        if(lineBuf != NULL)
                        {
                            while(TRUE)
                            {
                                if ((fgets(lineBuf, GRAB_LINE_SIZE_MAX, stdin) == NULL) && (!feof(stdin)))
                                {
                                    cmsLog_error("ERROR: fgets\n");
                                    fprintf(outputFile, "ERROR: fgets\n");
                                    ret = CMSRET_INTERNAL_ERROR;
                                    break;
                                }

                                if(0==strcmp(GRAB_END_STR, lineBuf))
                                {
                                    printf ("breaking out\n");

                                    break;
                                }

                                if(strlen(lineBuf) == GRAB_LINE_SIZE_MAX-1)
                                {
                                    cmsLog_error("ERROR: Line Is Too Long! (%d)\n", strlen(lineBuf));
                                    fprintf(outputFile, "ERROR: Line Is Too Long! (%d)\n", strlen(lineBuf));
                                    ret = CMSRET_INTERNAL_ERROR;
                                    break;
                                }

                                if(fputs(lineBuf, outputFile) == EOF)
                                {
                                    cmsLog_error("ERROR: fputs\n");
                                    ret = CMSRET_INTERNAL_ERROR;
                                    break;
                                }
                            }

                            cmsMem_free(lineBuf);
                        }
                        else
                        {
                            cmsLog_error("Could not Allocate Memory");
                            fprintf(outputFile, "Failed to Allocate Memory!\n");
                            ret = CMSRET_RESOURCE_EXCEEDED;
                        }

                        fclose(outputFile);
                    }
                }
            }
            else if (strcasecmp(pFileType, "bin") == 0)
            {

                int fdWrite = -1;
                UINT8 *buf = NULL;
                UINT16 msgSize = sizeof(CmsMsgHeader) + OMCI_PACKET_A_SIZE;
                UINT32 bufSize = 0;
                // OMCI message is stored in file as hex string that has
                // double size of OMCI message
                UINT32 strSize = (msgSize * 2) + 1;
                char hexStr[strSize];

                if ((fdWrite = open(pFileName,
                    O_CREAT | O_WRONLY,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1)
                {
                    printf("Failed to open '%s' for write: %s\n", pFileName, strerror(errno));
                    ret = CMSRET_INVALID_ARGUMENTS;
                }

                // initialize hexStr
                memset(hexStr, 0, strSize);

                // read the OMCI commands from stdin and write it to fileName
                while (fgets(hexStr, strSize, stdin) != NULL)
                {
                    // length of line in OMCI script should be double of msgSize
                    // so that after calling cmsUtl_hexStringToBinaryBuf
                    // bufSize should be equal msgSize
                    if (strlen(hexStr) == (msgSize * 2))
                    {
                        ret = cmsUtl_hexStringToBinaryBuf(hexStr, &buf, &bufSize);
                        if (ret == CMSRET_SUCCESS)
                        {
                            write(fdWrite, (void *)buf, bufSize);
                            CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
                        }
                    }
                    else
                        break;
                    // read pass newline character
                    fgets(hexStr, strSize, stdin);
                    // initialize hexStr
                    memset(hexStr, 0, strSize);
                }

                close(fdWrite);
            }
            else
            {
                ret = CMSRET_INVALID_ARGUMENTS;
                cmdOmciHelp("grab");
            }
        }
        else
        {
            ret = CMSRET_INVALID_ARGUMENTS;
            cmdOmciHelp("grab");
        }
    }
    else
    {
        cmdOmciHelp("grab");
        ret = CMSRET_INVALID_ARGUMENTS;
    }

    return ret;
}


/***************************************************************************
 * Function Name: cmdOmciCapture
 *
 * Description  : Processes the OMCI msg capture command from the CLI that
                  controls the processing of options to the command.

                  Capture can only be performed when other features that
                  access the internal capture file are not active.  This
                  includes playback, display, download and upload.
 ***************************************************************************/
static CmsRet cmdOmciCapture (char *pCmd, char *pOpt1, char *pArg1, char *pOpt2, char *pArg2)
{

    CmsRet Ret = CMSRET_SUCCESS;

    if (NULL == pCmd)
    {
        // Missing arguments
        cmsLog_error("omci capture: Missing arguments\n");
        cmdOmciHelp("capture");
    }
    else if (strcasecmp(pCmd, "control") == 0)
    {
        Ret = cmdOmciCaptureControl(pOpt1, pArg1, pOpt2, pArg2);
    }
    else if(strcasecmp(pCmd, "replay") == 0)
    {
        Ret = cmdOmciCaptureReplay(pOpt1, pArg1, pOpt2, pArg2);
    }
    else if (strcasecmp(pCmd, "view") == 0)
    {
        Ret = cmdOmciCaptureView(pOpt1, pArg1);
    }
    else if(strcasecmp(pCmd, "save") == 0)
    {
        Ret = cmdOmciCaptureSave(pOpt1, pArg1);
    }
    else if (strcasecmp(pCmd, "restore") == 0)
    {
        Ret = cmdOmciCaptureRestore(pOpt1, pArg1);
    }
    else
    {
        // Invalid argument
        cmsLog_error("omci capture: Invalid argument\n");
        cmdOmciHelp("capture");
    }

    return Ret;
}

/***************************************************************************
* Function Name: cmdOmciDgbMkErr
*
* Description  : Processes the OMCI dbg mkerr command from the CLI that
*                controls the processing of options to the command.
*
*omci debug --mkerr <swdlerr1(Window hole error)|swdlerr2(Download section msg rsp error)|swdlerr3(SW image CRC error)>\n
*
***************************************************************************/
static CmsRet cmdOmciDgbMkErr (char *pCmd)
{

CmsRet
    Ret = CMSRET_SUCCESS;

CmsMsgHeader
    MsgHdr;

    MsgHdr.dst                  = EID_OMCID;
    MsgHdr.src                  = EID_CONSOLED;
    MsgHdr.dataLength           = 0;
    MsgHdr.flags.all            = 0;
    MsgHdr.flags.bits.request   = TRUE;
    MsgHdr.next                 = 0;
    MsgHdr.sequenceNumber       = 0;
    MsgHdr.wordData             = 0;

    if (NULL == pCmd)
    {
        // Missing argument
        cmsLog_error("omci dbg mkerr: Missing argument\n");
        cmdOmciHelp("debug");
        Ret = CMSRET_INVALID_ARGUMENTS;
    }
    else if (strcasecmp(pCmd, "swdlerr1") == 0)
    {
        printf("Option is set to 1\n");
        // send msg via CMS
        MsgHdr.type = CMS_MSG_OMCI_DEBUG_MKERR_SWDLERR1;
    }
    else if(strcasecmp(pCmd, "swdlerr2") == 0)
    {
        printf("Option is set to 2\n");
        // send msg via CMS
        MsgHdr.type = CMS_MSG_OMCI_DEBUG_MKERR_SWDLERR2;
    }
    else if (strcasecmp(pCmd, "swdlerr3") == 0)
    {
        printf("Option is set to 3\n");
        // send msg via CMS
        MsgHdr.type = CMS_MSG_OMCI_DEBUG_MKERR_SWDLERR3;
    }
    else
    {
        // Missing argument
        cmsLog_error("omci dbg mkerr: invalid argument\n");
        cmdOmciHelp("debug");
        Ret = CMSRET_INVALID_ARGUMENTS;
    }

    // send msg if processed valid
    if (CMSRET_SUCCESS == Ret)
    {
        Ret = cmsMsg_send(cliPrvtMsgHandle, &MsgHdr);
    }

    return Ret;
}


/***************************************************************************
 * Function Name: getFileSize
 * Description  : Returns the size of a file
 ***************************************************************************/
static int getFileSize(char *fileName)
{
    struct stat fileStatus;

    if(stat(fileName, &fileStatus) != 0)
    {
        return -1;
    }

    return (int)(fileStatus.st_size);
}

/***************************************************************************
 * Function Name: cmdOmciSend
 * Description  : Sends the contents of a binary file to the OMCI Daemon
 ***************************************************************************/
static CmsRet cmdOmciSend(char *pFileNameOpt, char *pFileNameArg)
{
    int fileSize = 0;
    UINT16 i = 0, j = 0;
    UINT16 msgSize = sizeof(CmsMsgHeader) + OMCI_PACKET_A_SIZE;
    char buf[msgSize];
    FILE  *fs = NULL;
    CmsRet ret = CMSRET_SUCCESS;

    if (NULL != pFileNameOpt)
    {
        if (strcasecmp(pFileNameOpt, "--filename") == 0)
        {
            if (NULL != pFileNameArg)
            {
                if ((fs = fopen(pFileNameArg, "rb")) != NULL)
                {
                    if ((fileSize = getFileSize(pFileNameArg)) > 0)
                    {
                        while (!feof(fs))
                        {
                            buf[i++] = fgetc(fs);
                            if (i == msgSize)
                            {
                                j++;
                                if ((ret = cmsMsg_send(cliPrvtMsgHandle, (CmsMsgHeader *) buf)) != CMSRET_SUCCESS)
                                {
                                    cmsLog_error("Could not send out CMS_MSG_OMCI_COMMAND_REQUEST, ret=%d, id = %d from CONSOLED to OMCID", ret, j);
                                }
                                cmsLog_notice("Send CMS_MSG_OMCI_COMMAND_REQUEST with id = %d from CONSOLED to OMCID", j);
                                i = 0;
                            }
                        }
                    }
                    else
                    {
                        cmsLog_error("File %s is empty or could not stat", pFileNameArg);
                        ret = CMSRET_INTERNAL_ERROR;
                    }

                }
                else
                {
                    cmsLog_error("Failed to open %s for read", pFileNameArg);
                    return CMSRET_INVALID_ARGUMENTS;
                }
            }
            else
            {
                // Missing argument
                cmsLog_error("omci send: Missing filename argument\n");
                cmdOmciHelp("send");
            }
        }
        else
        {
            // Missing argument
            cmsLog_error("omci send: invalid filename option\n");
            cmdOmciHelp("send");
        }
    }
    else
    {
        // Missing argument
        cmsLog_error("omci send: Missing argument option\n");
        cmdOmciHelp("send");
    }

    if (NULL != fs)
    {
        fclose(fs);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciTcont
 * Description  : configure maximum number of TConts and the first TCont
 *                managed entity ID.
 ***************************************************************************/
static CmsRet cmdOmciTcont(char *tcontmax, char *startid)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void *) &obj)) == CMSRET_SUCCESS)
        {
            obj->numberOfTConts = strtoul(tcontmax, (char **)NULL, 0);
            obj->tcontManagedEntityId = strtoul(startid, (char **)NULL, 0);

            cmsObj_set(obj, &iidStack);
            cmsObj_free((void **)&obj);

            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciTcontPolicy
 * Description  : configure policy of  TCont.
 ***************************************************************************/
static CmsRet cmdOmciTcontPolicy(char *port, char *policy)
{
    CmsRet ret;
    UINT32 portNum = 0;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void *) &obj)) == CMSRET_SUCCESS)
        {
            portNum = strtoul(port, (char **)NULL, 0);
            // tcontPolicies is 32-bit bitmask that represents policies for 32 TCont.
            // If bit is 0 then policy is stricted priority (SP),
            // otherwise policy is weighted round robin (WRR)
            if (strcasecmp(policy, "wrr") == 0)
                obj->tcontPolicies  |= (1 << portNum);
            else
                obj->tcontPolicies &= ~(1 << portNum);

            cmsObj_set(obj, &iidStack);
            cmsObj_free((void **)&obj);

            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciEth
 * Description  : configure maximum number of Ethernet ports and the first
 *                Ethernet port managed entity ID.
 ***************************************************************************/
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
static CmsRet cmdOmciEth(char *ethmax, char *startid, char*veip)
#else
static CmsRet cmdOmciEth(char *ethmax, char *startid)
#endif
{
    UINT32 portMax = 0, startId = 0, i = 0;
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void *) &obj)) == CMSRET_SUCCESS)
        {
            portMax = strtoul(ethmax, (char **)NULL, 0);
            startId = strtoul(startid, (char **)NULL, 0);

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
            if (!strcmp(veip, "1"))
            {
                //1st eth port as veip
                obj->veipPptpUni0 = 1;
                obj->veipManagedEntityId1 = startId;
                portMax -= 1;
                startId += 1;
            }
            else
#endif
            {
                obj->veipPptpUni0 = 0;
                obj->veipManagedEntityId1 = GPON_FIRST_VEIP_MEID;
            }

            for (i = 0; i < portMax; i++)
            {
                switch (i)
                {
                case 0:
                    obj->ethernetManagedEntityId1 = startId + i;
                    break;
                case 1:
                    obj->ethernetManagedEntityId2 = startId + i;
                    break;
                case 2:
                    obj->ethernetManagedEntityId3 = startId + i;
                    break;
                case 3:
                    obj->ethernetManagedEntityId4 = startId + i;
                    break;
                case 4:
                    obj->ethernetManagedEntityId5 = startId + i;
                    break;
                case 5:
                    obj->ethernetManagedEntityId6 = startId + i;
                    break;
                case 6:
                    obj->ethernetManagedEntityId7 = startId + i;
                    break;
                case 7:
                    obj->ethernetManagedEntityId8 = startId + i;
                    break;
                case 8:
                    obj->ethernetManagedEntityId9 = startId + i;
                    break;
                case 9:
                    obj->ethernetManagedEntityId10 = startId + i;
                    break;
                case 10:
                    obj->ethernetManagedEntityId11 = startId + i;
                    break;
                case 11:
                    obj->ethernetManagedEntityId12 = startId + i;
                    break;
                case 12:
                    obj->ethernetManagedEntityId13 = startId + i;
                    break;
                case 13:
                    obj->ethernetManagedEntityId14 = startId + i;
                    break;
                case 14:
                    obj->ethernetManagedEntityId15 = startId + i;
                    break;
                case 15:
                    obj->ethernetManagedEntityId16 = startId + i;
                    break;
                case 16:
                    obj->ethernetManagedEntityId17 = startId + i;
                    break;
                case 17:
                    obj->ethernetManagedEntityId18 = startId + i;
                    break;
                case 18:
                    obj->ethernetManagedEntityId19 = startId + i;
                    break;
                case 19:
                    obj->ethernetManagedEntityId20 = startId + i;
                    break;
                case 20:
                    obj->ethernetManagedEntityId21 = startId + i;
                    break;
                case 21:
                    obj->ethernetManagedEntityId22 = startId + i;
                    break;
                case 22:
                    obj->ethernetManagedEntityId23 = startId + i;
                    break;
                case 23:
                    obj->ethernetManagedEntityId24 = startId + i;
                    break;
                default:
                    break;
                }
            }
            obj->numberOfEthernetPorts = portMax;
            cmsObj_set(obj, &iidStack);
            cmsObj_free((void **)&obj);

            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1

/***************************************************************************
 * Function Name: cmdEthPortTypeShow
 * Description  : show port type (rg only, or ont only) for all ethernet ports.
 ***************************************************************************/
static CmsRet cmdEthPortTypeShow(void)
{
    CmsRet ret = CMSRET_INVALID_ARGUMENTS;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        rutOmci_printPorts();
        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdEthPortTypeConfig
 * Description  : configure Gpon type (rg only, or ont only) for the given port (0..7).
 ***************************************************************************/
static CmsRet cmdEthPortTypeConfig(char *ethPort, char *type)
{
    UINT32 port = 0;
    OmciEthPortType portType = OMCI_ETH_PORT_TYPE_NONE;
    OmciEthPortType_t eth;
    CmsRet ret = CMSRET_INVALID_ARGUMENTS;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    port = strtoul(ethPort, (char **)NULL, 0);
    if (port > 8)
    {
        cmsLog_error("Ethernet port (%d) is out of range <0..8>", port);
        return ret;
    }

    if (strcmp(type, ETH_PORT_TYPE_RG) == 0)
        portType = OMCI_ETH_PORT_TYPE_RG;
    else
        portType = OMCI_ETH_PORT_TYPE_ONT;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void *) &obj)) == CMSRET_SUCCESS)
        {
            eth.types.all = obj->ethernetTypes;

            switch (port)
            {
                case 0:
                    eth.types.ports.eth0 = portType;
                    break;
                case 1:
                    eth.types.ports.eth1 = portType;
                    break;
                case 2:
                    eth.types.ports.eth2 = portType;
                    break;
                case 3:
                    eth.types.ports.eth3 = portType;
                    break;
                case 4:
                    eth.types.ports.eth4 = portType;
                    break;
                case 5:
                    eth.types.ports.eth5 = portType;
                    break;
                case 6:
                    eth.types.ports.eth6 = portType;
                    break;
                case 7:
                    eth.types.ports.eth7 = portType;
                    break;
                default:
                    break;
            }

            obj->ethernetTypes = eth.types.all;

            cmsObj_set(obj, &iidStack);
            cmsObj_free((void **)&obj);

            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();

        cmdEthPortTypeShow();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

#endif   /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */

/***************************************************************************
 * Function Name: cmdOmciMibSyncGet
 * Description  : get value of MibDataSync attribute in OntData object
 ***************************************************************************/
static CmsRet cmdOmciMibSyncGet(void)
{
    CmsMsgHeader cmdMsg;
    CmsRet ret = CMSRET_SUCCESS;

    memset(&cmdMsg, 0, sizeof(CmsMsgHeader));
    cmdMsg.type = CMS_MSG_OMCI_DUMP_INFO_REQ;
    cmdMsg.src = EID_CONSOLED;
    cmdMsg.dst = EID_OMCID;
    cmdMsg.flags_request = 1;
    cmdMsg.dataLength = 0;
    cmdMsg.wordData = (UINT32)MDMOID_ONT_DATA;

    ret = cmsMsg_send(cliPrvtMsgHandle, (CmsMsgHeader*)&cmdMsg);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Send CMS_MSG_OMCI_DUMP_INFO_REQ failed, ret=%d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciUniPathModeStatus
 * Description  : show current uni path mode configuration
 ***************************************************************************/
static CmsRet cmdOmciUniPathModeStatus(void)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0,
          (void*)&obj)) == CMSRET_SUCCESS)
        {
            switch (obj->uniDataPathMode)
            {
            case 1:
                printf("   UNI data path mode is configured to on\n\n");
                break;
            case 0:
                printf("   UNI data path mode is configured to off\n\n");
                break;
            default:
                printf("   UNI data path mode %d is invalid \n\n",
                  obj->uniDataPathMode);
                break;
            }
            cmsObj_free((void **)&obj);
        }
        else
            cmsLog_error("Could not cmsObj_get OmciSystemObject, ret=%d", ret);

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciUniPathModeConfig
 * Description  : configure UNI data path mode:
 *                TRUE - check the UNI ports that MEs on their ANI-UNI path
 *                       have been updated.
 *                FALSE - check the whole OMCI MIB.
 ***************************************************************************/
static CmsRet cmdOmciPathModeConfig(UINT32 uniDataPathMode)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, 
          (void*)&obj)) == CMSRET_SUCCESS)
        {
            obj->uniDataPathMode = uniDataPathMode;
            cmsObj_set(obj, &iidStack);
            cmsObj_free((void**)&obj);
            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    cmdOmciUniPathModeStatus();

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciPromiscModeStatus
 * Description  : show current Promisc mode configuration
 ***************************************************************************/
static CmsRet cmdOmciPromiscModeStatus(void)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void *) &obj)) == CMSRET_SUCCESS)
        {
            switch (obj->promiscMode)
            {
            case 0:
                printf("   Promisc mode is configured to off\n\n");
                break;
            case 1:
                printf("   Promisc mode is configured to on\n\n");
                break;
            default:
                printf("   Promisc mode is not configured yet\n\n");
                break;
            }
            cmsObj_free((void **)&obj);
        }
        else
            cmsLog_error("Could not cmsObj_get OmciSystemObject, ret=%d", ret);

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciPromiscModeConfig
 * Description  : configure omci Promisc mode:
 *                on, off
 ***************************************************************************/
static CmsRet cmdOmciPromiscModeConfig(UINT32 promiscMode)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader reqMsg;
    CmsMsgHeader *respMsg;

    memset(&reqMsg, 0, sizeof(CmsMsgHeader));
    reqMsg.type = CMS_MSG_OMCI_PROMISC_SET_REQUEST;
    reqMsg.src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
    reqMsg.dst = EID_OMCID;
    reqMsg.flags_request = 1;
    reqMsg.dataLength = 0;
    reqMsg.sequenceNumber = 1;
    reqMsg.wordData = promiscMode;

    if ((ret = cmsMsg_send(cliPrvtMsgHandle, &reqMsg)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Could not send out CMS_MSG_OMCI_PROMISC_SET_REQUEST, ret=%d from CONSOLED to OMCID", ret);
        goto out;
    }
    cmsLog_notice("Send CMS_MSG_OMCI_PROMISC_SET_REQUEST from CONSOLED to OMCID");

    if ((ret = cmsMsg_receiveWithTimeout(cliPrvtMsgHandle, &respMsg, MSG_RECV_TIMEOUT_MS)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to receive CMS_MSG_OMCI_PROMISC_SET_RESPONSE, ret=%d from OMCID to CONSOLED", ret);
        goto out;
    }
    if (respMsg->type != CMS_MSG_OMCI_PROMISC_SET_RESPONSE)
    {
        cmsLog_error("Failed to receive CMS_MSG_OMCI_PROMISC_SET_RESPONSE, type=%d", respMsg->type);
        goto out;
    }
    cmsLog_notice("Receive CMS_MSG_OMCI_PROMISC_SET_RESPONSE from OMCID to CONSOLED");

    CMSMEM_FREE_BUF_AND_NULL_PTR(respMsg);

    cmdOmciPromiscModeStatus();

out:
    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciRawModeStatus
 * Description  : show current raw mode configuration
 ***************************************************************************/
static CmsRet cmdOmciRawModeStatus(void)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void *) &obj)) == CMSRET_SUCCESS)
        {
            if (obj->omciRawEnable == TRUE)
                printf("   OMCI raw is enabled - HTTPD should accept OMCI raw messages.\n\n");
            else
                printf("   OMCI raw is disabled - HTTPD should NOT accept OMCI raw messages.\n\n");
            cmsObj_free((void **)&obj);
        }
        else
            cmsLog_error("Could not cmsObj_get OmciSystemObject, ret=%d", ret);

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciRawModeConfig
 * Description  : configure raw mode:
 *                TRUE - allow OMCI raw messages be accepted by HTTPD.
 *                FALSE - disallow OMCI raw messages be accpeted by HTTPD.
 ***************************************************************************/
static CmsRet cmdOmciRawModeConfig(UBOOL8 rawMode)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void *) &obj)) == CMSRET_SUCCESS)
        {
            obj->omciRawEnable = rawMode;

            cmsObj_set(obj, &iidStack);
            cmsObj_free((void **)&obj);

            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    cmdOmciRawModeStatus();

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciTmOptionStatus
 * Description  : show current OMCI upstream traffic management option.
 ***************************************************************************/
static CmsRet cmdOmciTmOptionStatus(void)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void*)&obj))
          == CMSRET_SUCCESS)
        {
            printf("   OMCI upstream traffic management option:\n");
            if (obj->trafficManagementOption == OMCI_TRAFFIC_MANAGEMENT_PRIO)
            {
                printf("    (0) Priority controlled upstream traffic.\n\n");
            }
            else if (obj->trafficManagementOption == OMCI_TRAFFIC_MANAGEMENT_RATE)
            {
                printf("    (1) Rate controlled upstream traffic.\n\n");
            }
            else if (obj->trafficManagementOption == OMCI_TRAFFIC_MANAGEMENT_PRIO_RATE)
            {
                printf("    (2) Priority and rate controlled.\n\n");
            }
            else
            {
                printf("    Invalid option.\n\n");
            }

            printf("   QoS action when the downstream queue in "
              "GEM port network CTP ME is invalid:\n");
            if (obj->dsInvalidQueueAction == OMCI_DS_INVALID_QUEUE_ACTION_NONE)
            {
                printf("    (0) Do not set queue.\n\n");
            }
            else if (obj->dsInvalidQueueAction == OMCI_DS_INVALID_QUEUE_ACTION_PBIT)
            {
                printf("    (1) Set queue ID = packet pbit (HGU).\n\n");
            }
            else if (obj->dsInvalidQueueAction == OMCI_DS_INVALID_QUEUE_ACTION_PBIT_EXT)
            {
                printf("    (2) Set queue ID = packet pbit (SFU and HGU).\n\n");
            }
            else
            {
                printf("    Invalid option.\n\n");
            }
            cmsObj_free((void **)&obj);
        }
        else
        {
            cmsLog_error("Could not cmsObj_get OmciSystemObject, ret=%d", ret);
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciTmOptionConfig
 * Description  : configure OMCI upstream traffic management option.
 ***************************************************************************/
static CmsRet cmdOmciTmOptionConfig(UINT32 tmOption)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void*)&obj))
          == CMSRET_SUCCESS)
        {
            obj->trafficManagementOption = tmOption;
            cmsObj_set(obj, &iidStack);
            cmsObj_free((void**)&obj);
            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    cmdOmciTmOptionStatus();

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciDsQosConfig
 * Description  : configure OMCI downstream queue setting option.
 ***************************************************************************/
static CmsRet cmdOmciDsQosConfig(UINT32 qosAction)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void*)&obj))
          == CMSRET_SUCCESS)
        {
            obj->dsInvalidQueueAction = qosAction;
            cmsObj_set(obj, &iidStack);
            cmsObj_free((void**)&obj);
            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    cmdOmciTmOptionStatus();

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciDebugStatus
 * Description  : show debug status for all modules
 ***************************************************************************/
static CmsRet cmdOmciDebugStatus(void)
{
    omciDebug_t debug;
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void *) &obj)) == CMSRET_SUCCESS)
        {
            debug.flags.all = obj->debugFlags;
            cmsObj_free((void **)&obj);
            printf("\n=========== OMCI Debug Status ===========\n\n");
            if (debug.flags.bits.omci == OMCID_DEBUG_ON)
                printf("   OMCI messages:          ON\n");
            else
                printf("   OMCI messages:          OFF\n");
            if (debug.flags.bits.model == OMCID_DEBUG_ON)
                printf("   MODEL messages:         ON\n");
            else
                printf("   MODEL messages:         OFF\n");
            if (debug.flags.bits.vlan == OMCID_DEBUG_ON)
                printf("   VLAN messages:          ON\n");
            else
                printf("   VLAN messages:          OFF\n");
            if (debug.flags.bits.cmf == OMCID_DEBUG_ON)
                printf("   CMF messages:           ON\n");
            else
                printf("   CMF messages:           OFF\n");
            if (debug.flags.bits.flow == OMCID_DEBUG_ON)
                printf("   FLOW messages:          ON\n");
            else
                printf("   FLOW messages:          OFF\n");
            if (debug.flags.bits.rule == OMCID_DEBUG_ON)
                printf("   RULE messages:          ON\n");
            else
                printf("   RULE messages:          OFF\n");
            if (debug.flags.bits.mcast == OMCID_DEBUG_ON)
                printf("   MULTICAST messages:     ON\n");
            else
                printf("   MULTICAST messages:     OFF\n");
#if defined(DMP_X_ITU_ORG_VOICE_1)
            if (debug.flags.bits.voice == OMCID_DEBUG_ON)
                printf("   VOICE messages:         ON\n");
            else
                printf("   VOICE messages:         OFF\n");
#endif    // DMP_X_ITU_ORG_VOICE_1
            if (debug.flags.bits.file == OMCID_DEBUG_ON)
                printf("   Messages are logged to: FILE\n");
            else
                printf("   Messages are logged to: CONSOLE\n");
            printf("\n=========================================\n\n");
        }
        else
            cmsLog_error("Could not cmsObj_get OmciSystemObject, ret=%d", ret);

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciDebugDump
 * Description  : Write objects in MDM that has the given class ID to console
 ***************************************************************************/
static CmsRet cmdOmciDebugDump(const char *id)
{
    OmciMdmOidClassId_t info;
    CmsMsgHeader cmdMsg;
    CmsRet ret = CMSRET_SUCCESS;

    memset(&info, 0, sizeof(OmciMdmOidClassId_t));
    info.classId = (UINT16)strtoul(id, (char**)NULL, 0);
    if (info.classId >= OMCI_ME_CLASS_MAX)
    {
        cmsLog_notice("Invalid ME Class: %d (max %d)",
          info.classId, OMCI_ME_CLASS_MAX);
        return CMSRET_INVALID_ARGUMENTS;
    }

    omciUtl_classIdToOid(&info);

    if (info.mdmOid == 0)
    {
        cmsLog_notice("ME Class %d is not supported", info.classId);
        return CMSRET_INVALID_ARGUMENTS;
    }

    memset(&cmdMsg, 0, sizeof(CmsMsgHeader));
    cmdMsg.type = CMS_MSG_OMCI_DUMP_INFO_REQ;
    cmdMsg.src = EID_CONSOLED;
    cmdMsg.dst = EID_OMCID;
    cmdMsg.flags_request = 1;
    cmdMsg.dataLength = 0;
    cmdMsg.wordData = (UINT32)info.mdmOid;

    ret = cmsMsg_send(cliPrvtMsgHandle, (CmsMsgHeader*)&cmdMsg);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Send CMS_MSG_OMCI_DUMP_INFO_REQ failed, ret=%d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciDebugInfo
 * Description  : Dump OMCI internal data
 ***************************************************************************/
static CmsRet cmdOmciDebugInfo(char *numValStr)
{
    CmsMsgHeader cmdMsg;
    UINT32 flag = 0;
    CmsRet ret = CMSRET_SUCCESS;

    if (numValStr != NULL)
    {
        flag = atoi(numValStr);
    }

    memset(&cmdMsg, 0, sizeof(CmsMsgHeader));
    cmdMsg.type = CMS_MSG_OMCI_DUMP_INFO_REQ;
    cmdMsg.src = EID_CONSOLED;
    cmdMsg.dst = EID_OMCID;
    cmdMsg.flags_request = 1;
    cmdMsg.dataLength = 0;
    cmdMsg.wordData = (UINT32)flag;

    ret = cmsMsg_send(cliPrvtMsgHandle, (CmsMsgHeader*)&cmdMsg);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Send CMS_MSG_OMCI_DUMP_INFO_REQ failed, ret=%d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciDebug
 * Description  : show/hide OMCI, CMF, MODEL, or VLAN debug messages
 ***************************************************************************/
static CmsRet cmdOmciDebug(char *module, char *state)
{
    omciDebug_t debug;
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader reqMsg;
    CmsMsgHeader *respMsg;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    memset(&reqMsg, 0, sizeof(CmsMsgHeader));
    reqMsg.type = CMS_MSG_OMCI_DEBUG_GET_REQUEST;
    reqMsg.src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
    reqMsg.dst = EID_OMCID;
    reqMsg.flags_request = 1;
    reqMsg.dataLength = 0;
    reqMsg.sequenceNumber = 1;

    if ((ret = cmsMsg_send(cliPrvtMsgHandle, &reqMsg)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Could not send out CMS_MSG_OMCI_DEBUG_GET_REQUEST, ret=%d from CONSOLED to OMCID", ret);
        goto out;
    }
    cmsLog_notice("Send CMS_MSG_OMCI_DEBUG_GET_REQUEST from CONSOLED to OMCID");

    if ((ret = cmsMsg_receiveWithTimeout(cliPrvtMsgHandle, &respMsg, MSG_RECV_TIMEOUT_MS)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to receive CMS_MSG_OMCI_DEBUG_GET_RESPONSE, ret=%d from OMCID to CONSOLED", ret);
        goto out;
    }
    if (respMsg->type != CMS_MSG_OMCI_DEBUG_GET_RESPONSE)
    {
        cmsLog_error("Failed to receive CMS_MSG_OMCI_DEBUG_GET_RESPONSE, type=%d", respMsg->type);
        goto out;
    }
    cmsLog_notice("Receive CMS_MSG_OMCI_DEBUG_GET_RESPONSE from OMCID to CONSOLED");

    debug.flags.all = respMsg->wordData;
    CMSMEM_FREE_BUF_AND_NULL_PTR(respMsg);

    if (strcasecmp(module, "all") == 0)
    {
        if (strcasecmp(state, "on") == 0)
            debug.flags.bits.omci = debug.flags.bits.model =
                debug.flags.bits.vlan = debug.flags.bits.cmf =
                debug.flags.bits.flow = debug.flags.bits.rule =
                debug.flags.bits.mcast = debug.flags.bits.voice =
                debug.flags.bits.file = OMCID_DEBUG_ON;
        else
            debug.flags.bits.omci = debug.flags.bits.model =
                debug.flags.bits.vlan = debug.flags.bits.cmf =
                debug.flags.bits.flow = debug.flags.bits.rule =
                debug.flags.bits.mcast = debug.flags.bits.voice =
                debug.flags.bits.file = OMCID_DEBUG_OFF;
    }
    else if (strcasecmp(module, "omci") == 0)
    {
        if (strcasecmp(state, "on") == 0)
            debug.flags.bits.omci = OMCID_DEBUG_ON;
        else
            debug.flags.bits.omci = OMCID_DEBUG_OFF;
    }
    else if (strcasecmp(module, "model") == 0)
    {
        if (strcasecmp(state, "on") == 0)
            debug.flags.bits.model = OMCID_DEBUG_ON;
        else
            debug.flags.bits.model = OMCID_DEBUG_OFF;
    }
    else if (strcasecmp(module, "vlan") == 0)
    {
        if (strcasecmp(state, "on") == 0)
            debug.flags.bits.vlan = OMCID_DEBUG_ON;
        else
            debug.flags.bits.vlan = OMCID_DEBUG_OFF;
    }
    else if (strcasecmp(module, "cmf") == 0)
    {
        if (strcasecmp(state, "on") == 0)
            debug.flags.bits.cmf = OMCID_DEBUG_ON;
        else
            debug.flags.bits.cmf = OMCID_DEBUG_OFF;
    }
    else if (strcasecmp(module, "flow") == 0)
    {
        if (strcasecmp(state, "on") == 0)
            debug.flags.bits.flow = OMCID_DEBUG_ON;
        else
            debug.flags.bits.flow = OMCID_DEBUG_OFF;
    }
    else if (strcasecmp(module, "rule") == 0)
    {
        if (strcasecmp(state, "on") == 0)
            debug.flags.bits.rule = OMCID_DEBUG_ON;
        else
            debug.flags.bits.rule = OMCID_DEBUG_OFF;
    }
    else if (strcasecmp(module, "mcast") == 0)
    {
        if (strcasecmp(state, "on") == 0)
            debug.flags.bits.mcast = OMCID_DEBUG_ON;
        else
            debug.flags.bits.mcast = OMCID_DEBUG_OFF;
    }
#if defined(DMP_X_ITU_ORG_VOICE_1)
    else if (strcasecmp(module, "voice") == 0)
    {
        if (strcasecmp(state, "on") == 0)
            debug.flags.bits.voice = OMCID_DEBUG_ON;
        else
            debug.flags.bits.voice = OMCID_DEBUG_OFF;
    }
#endif    // DMP_X_ITU_ORG_VOICE_1
    else if (strcasecmp(module, "file") == 0)
    {
        if (strcasecmp(state, "on") == 0)
            debug.flags.bits.file = OMCID_DEBUG_ON;
        else
            debug.flags.bits.file = OMCID_DEBUG_OFF;
    }

    memset(&reqMsg, 0, sizeof(CmsMsgHeader));
    reqMsg.type = CMS_MSG_OMCI_DEBUG_SET_REQUEST;
    reqMsg.src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
    reqMsg.dst = EID_OMCID;
    reqMsg.flags_request = 1;
    reqMsg.dataLength = 0;
    reqMsg.sequenceNumber = 1;
    reqMsg.wordData = debug.flags.all;

    if ((ret = cmsMsg_send(cliPrvtMsgHandle, &reqMsg)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Could not send out CMS_MSG_OMCI_DEBUG_SET_REQUEST, ret=%d from CONSOLED to OMCID", ret);
        goto out;
    }
    cmsLog_notice("Send CMS_MSG_OMCI_DEBUG_SET_REQUEST from CONSOLED to OMCID");

    if ((ret = cmsMsg_receiveWithTimeout(cliPrvtMsgHandle, &respMsg, MSG_RECV_TIMEOUT_MS)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Failed to receive CMS_MSG_OMCI_DEBUG_SET_RESPONSE, ret=%d from OMCID to CONSOLED", ret);
        goto out;
    }
    if (respMsg->type != CMS_MSG_OMCI_DEBUG_SET_RESPONSE)
    {
        cmsLog_error("Failed to receive CMS_MSG_OMCI_DEBUG_SET_RESPONSE, type=%d", respMsg->type);
        goto out;
    }
    cmsLog_notice("Receive CMS_MSG_OMCI_DEBUG_SET_RESPONSE from OMCID to CONSOLED");

    debug.flags.all = respMsg->wordData;
    CMSMEM_FREE_BUF_AND_NULL_PTR(respMsg);

    printf("\n=========== OMCI Debug Status ===========\n\n");
    if (debug.flags.bits.omci == OMCID_DEBUG_ON)
        printf("   OMCI messages:          ON\n");
    else
        printf("   OMCI messages:          OFF\n");
    if (debug.flags.bits.model == OMCID_DEBUG_ON)
        printf("   MODEL messages:         ON\n");
    else
        printf("   MODEL messages:         OFF\n");
    if (debug.flags.bits.vlan == OMCID_DEBUG_ON)
        printf("   VLAN messages:          ON\n");
    else
        printf("   VLAN messages:          OFF\n");
    if (debug.flags.bits.cmf == OMCID_DEBUG_ON)
        printf("   CMF messages:           ON\n");
    else
        printf("   CMF messages:           OFF\n");
    if (debug.flags.bits.flow == OMCID_DEBUG_ON)
        printf("   FLOW messages:          ON\n");
    else
        printf("   FLOW messages:          OFF\n");
    if (debug.flags.bits.rule == OMCID_DEBUG_ON)
        printf("   RULE messages:          ON\n");
    else
        printf("   RULE messages:          OFF\n");
    if (debug.flags.bits.mcast == OMCID_DEBUG_ON)
        printf("   MULTICAST messages:     ON\n");
    else
        printf("   MULTICAST messages:     OFF\n");
#if defined(DMP_X_ITU_ORG_VOICE_1)
    if (debug.flags.bits.voice == OMCID_DEBUG_ON)
        printf("   VOICE messages:         ON\n");
    else
        printf("   VOICE messages:         OFF\n");
#endif    // DMP_X_ITU_ORG_VOICE_1
    if (debug.flags.bits.file == OMCID_DEBUG_ON)
        printf("   Messages are logged to: FILE\n");
    else
        printf("   Messages are logged to: CONSOLE\n");
    printf("\n=========================================\n\n");

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {    
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack,
          0, (void*)&obj)) == CMSRET_SUCCESS)
        {
            obj->debugFlags = debug.flags.all;

            cmsObj_set(obj, &iidStack);
            cmsObj_free((void **)&obj);
        }    

        cmsLck_releaseLock();
    }    
    else 
    {    
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }    

out:
    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciMcastHostctrlStatus
 * Description  : show current mcast host control mode configuration
 ***************************************************************************/
static CmsRet cmdOmciMcastHostctrlStatus(void)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    McastCfgObject *obj = NULL;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_MCAST_CFG, &iidStack, 0, (void *) &obj)) == CMSRET_SUCCESS)
        {
            if (obj->mcastHostControl== TRUE)
                printf("   OMCI host controlled multicast is enabled.\n\n");
            else
                printf("   OMCI host controlled multicast is disabled.\n\n");
            cmsObj_free((void **)&obj);
        }
        else
            cmsLog_error("Could not cmsObj_get McastCfgObject, ret=%d", ret);

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}


/***************************************************************************
 * Function Name: cmdOmciMcastHostctrlConfig
 * Description  : configure mcast mode:
 *                TRUE - Enable Host controlled multicast.
 *                FALSE - Disable Host controlled multicast.
 ***************************************************************************/
static CmsRet cmdOmciMcastHostctrlConfig(UBOOL8 mcastMode)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    McastCfgObject *obj = NULL;

    // Attempt to lock MDM.
    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_MCAST_CFG, &iidStack, 0, (void *) &obj)) == CMSRET_SUCCESS)
        {
            obj->mcastHostControl = mcastMode;

            cmsObj_set(obj, &iidStack);
            cmsObj_free((void **)&obj);

            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    cmdOmciMcastHostctrlStatus();

    return ret;
}


/***************************************************************************
 * Function Name: cmdOmciTestAvc
 * Description  : Request OMCID to generate debug AVC message.
 ***************************************************************************/
static CmsRet cmdOmciTestAvc(UINT16 meClass, UINT16 meId, UINT16 attrMask)
{
    CmsRet ret = CMSRET_SUCCESS;
    char buf[sizeof(CmsMsgHeader) + sizeof(OmciMsgGenCmd)];
    CmsMsgHeader *msgHdrP = (CmsMsgHeader*)buf;
    OmciMsgGenCmd *bodyP = (OmciMsgGenCmd*)(msgHdrP + 1);
    UINT32 dataLength = sizeof(OmciMsgGenCmd);

    memset(buf, 0, sizeof(CmsMsgHeader) + dataLength);
    msgHdrP->type = CMS_MSG_OMCI_DEBUG_OMCI_MSG_GEN;
    msgHdrP->dst = EID_OMCID;
    msgHdrP->src = EID_CONSOLED;
    msgHdrP->dataLength = dataLength;
    msgHdrP->flags_request = TRUE;
    bodyP->msgType = OMCI_MSG_TYPE_ATTRIBUTEVALUECHANGE;
    bodyP->meClass = meClass;
    bodyP->meInst = meId;
    bodyP->attrMask = attrMask;

    ret = cmsMsg_send(cliPrvtMsgHandle, msgHdrP);
    return ret;
}


/***************************************************************************
 * Function Name: cmdOmciTestAlarm
 * Description  : Request OMCID to generate debug alarm message.
 ***************************************************************************/
static CmsRet cmdOmciTestAlarm(UINT16 meClass, UINT16 meId, UINT16 alarmNum,
  UBOOL8 setFlagB)
{
    CmsRet ret = CMSRET_SUCCESS;
    char buf[sizeof(CmsMsgHeader) + sizeof(OmciMsgGenCmd)];
    CmsMsgHeader *msgHdrP = (CmsMsgHeader*)buf;
    OmciMsgGenCmd *bodyP = (OmciMsgGenCmd*)(msgHdrP + 1);
    UINT32 dataLength = sizeof(OmciMsgGenCmd);

    memset(buf, 0, sizeof(CmsMsgHeader) + dataLength);
    msgHdrP->type = CMS_MSG_OMCI_DEBUG_OMCI_MSG_GEN;
    msgHdrP->dst = EID_OMCID;
    msgHdrP->src = EID_CONSOLED;
    msgHdrP->dataLength = dataLength;
    msgHdrP->flags_request = TRUE;
    bodyP->msgType = OMCI_MSG_TYPE_ALARM;
    bodyP->meClass = meClass;
    bodyP->meInst = meId;
    bodyP->alarmNum = alarmNum;
    bodyP->flagB = setFlagB;

    ret = cmsMsg_send(cliPrvtMsgHandle, msgHdrP);
    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciVoiceModelSet
 * Description  : Set voice model (OMCI path or IP path).
 ***************************************************************************/
static CmsRet cmdOmciVoiceModelSet(UINT8 model)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT))
      == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void*)&obj))
          == CMSRET_SUCCESS)
        {
            obj->voiceModelOption = model;
            cmsObj_set(obj, &iidStack);
            cmsObj_free((void**)&obj);
            cmsMgm_saveConfigToFlash();
        }
        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret=%d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciVoiceModelGet
 * Description  : show current voice model configuration
 ***************************************************************************/
static CmsRet cmdOmciVoiceModelGet(void)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT))
      == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void*)&obj))
          == CMSRET_SUCCESS)
        {
            (obj->voiceModelOption == 0) ? \
              printf("   OMCI path.\n\n") : printf("   IP path.\n\n");
            cmsObj_free((void **)&obj);
        }
        else
        {
            cmsLog_error("cmsObj_get(OmciSystem) failed, ret=%d", ret);
        }
        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout() failed, ret=%d", ret);
    }

    return ret;
}


/***************************************************************************
 * Function Name: cmdOmciTmOptionHandler
 * Description  : TM option command handler.
 ***************************************************************************/
void cmdOmciTmOptionHandler(SINT32 argc __attribute__((unused)), char *argv[])
{
    if (argv[1] == NULL)
    {
        cmdOmciHelp("tmoption");
    }
    else if (strcasecmp(argv[1], "--help") == 0)
    {
        cmdOmciHelp("tmoption");
    }
    else if (strcasecmp(argv[1], "--status") == 0)
    {
        cmdOmciTmOptionStatus();
    }
    else if (strcasecmp(argv[1], "--usmode") == 0)
    {
        if (argv[2] == NULL)
        {
            cmsLog_error("TM option: Missing arguments\n");
            cmdOmciHelp("tmoption");
        }
        else
        {
            if (strcasecmp(argv[2], "0") == 0)
            {
                cmdOmciTmOptionConfig(0);
            }
            else if(strcasecmp(argv[2], "1") == 0)
            {
                cmdOmciTmOptionConfig(1);
            }
            else if(strcasecmp(argv[2], "2") == 0)
            {
                cmdOmciTmOptionConfig(2);
            }
            else
            {
                cmsLog_error("TM option: Invalid argument '%s'\n", argv[2]);
                cmdOmciHelp("tmoption");
            }
        }
    }
    else if (strcasecmp(argv[1], "--dsqueue") == 0)
    {
        if (argv[2] == NULL)
        {
            cmsLog_error("TM option: Missing arguments\n");
            cmdOmciHelp("tmoption");
        }
        else
        {
            if (strcasecmp(argv[2], "0") == 0)
            {
                cmdOmciDsQosConfig(0);
            }
            else if(strcasecmp(argv[2], "1") == 0)
            {
                cmdOmciDsQosConfig(1);
            }
            else if(strcasecmp(argv[2], "2") == 0)
            {
                cmdOmciDsQosConfig(2);
            }
            else
            {
                cmsLog_error("TM option: Invalid argument '%s'\n", argv[2]);
                cmdOmciHelp("tmoption");
            }
        }
    }
}

/***************************************************************************
 * Function Name: cmdOmciTestHandler
 * Description  : OMCI avc/alarm command handler.
 ***************************************************************************/
void cmdOmciTestHandler(SINT32 argc, char *argv[])
{
    UINT16 val0;
    UINT16 val1;
    UINT16 val2;
    UINT16 val3;

    if (argv[1] == NULL)
    {
        cmdOmciHelp("test");
    }
    else if (strcasecmp(argv[1], "--genavc") == 0)
    {
        if ((argc == 8) &&
          (strcasecmp(argv[2], "--meclass") == 0) &&
          (strcasecmp(argv[4], "--meid") == 0) &&
          (strcasecmp(argv[6], "--attrmask") == 0))
        {
            val0 = strtoul(argv[3], (char**)NULL, 0);
            val1 = strtoul(argv[5], (char**)NULL, 0);
            val2 = strtoul(argv[7], (char**)NULL, 0);
            cmdOmciTestAvc(val0, val1, val2);
        }
        else
        {
            cmdOmciHelp("test");
        }
    }
    else if (strcasecmp(argv[1], "--genalarm") == 0)
    {
        if ((argc == 10) &&
          (strcasecmp(argv[2], "--meclass") == 0) &&
          (strcasecmp(argv[4], "--meid") == 0) &&
          (strcasecmp(argv[6], "--num") == 0) &&
          (strcasecmp(argv[8], "--state") == 0))
        {
            val0 = strtoul(argv[3], (char**)NULL, 0);
            val1 = strtoul(argv[5], (char**)NULL, 0);
            val2 = strtoul(argv[7], (char**)NULL, 0);
            val3 = strtoul(argv[9], (char**)NULL, 0);
            cmdOmciTestAlarm(val0, val1, val2, val3);
        }
        else
        {
            cmdOmciHelp("test");
        }
    }
    else
    {
        cmdOmciHelp("test");
    }
}

/***************************************************************************
 * Function Name: cmdOmciBridgeStatus
 * Description  : show current bridge setting.
 ***************************************************************************/
static CmsRet cmdOmciBridgeStatus(void)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT))
      == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void*)&obj))
          == CMSRET_SUCCESS)
        {
            printf("   Bridge forwarding mask: 0x%08x\n",
              obj->bridgeGroupFwdMask);
            cmsObj_free((void**)&obj);
        }
        else
        {
            cmsLog_error("Could not cmsObj_get OmciSystemObject, ret=%d", ret);
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciBridgeConfig
 * Description  : configure bridge forwarding mask.
 ***************************************************************************/
static CmsRet cmdOmciBridgeConfig(UINT16 mask)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) ==
      CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void*)&obj))
          == CMSRET_SUCCESS)
        {
            obj->bridgeGroupFwdMask = mask;
            cmsObj_set(obj, &iidStack);
            cmsObj_free((void**)&obj);
            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    cmdOmciBridgeStatus();

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciBridgeHandler
 * Description  : OMCI bridge command handler.
 ***************************************************************************/
void cmdOmciBridgeHandler(SINT32 argc, char *argv[])
{
    UINT16 val0;

    if (argv[1] == NULL)
    {
        cmdOmciHelp("bridge");
    }
    else if (strcasecmp(argv[1], "--help") == 0)
    {
        cmdOmciHelp("bridge");
    }
    else if (strcasecmp(argv[1], "--status") == 0)
    {
        cmdOmciBridgeStatus();
    }
    else if (strcasecmp(argv[1], "--fwdmask") == 0)
    {
        if (argv[2] == NULL)
        {
            cmsLog_error("bridge: Missing arguments\n");
            cmdOmciHelp("bridge");
        }
        else
        {
            val0 = strtoul(argv[2], (char**)NULL, 0);
            cmdOmciBridgeConfig(val0);
        }
    }
}

/***************************************************************************
 * Function Name: cmdOmciMiscStatus
 * Description  : show current miscellaneous setting.
 ***************************************************************************/
static CmsRet cmdOmciMiscStatus(void)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT))
      == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0,
          (void*)&obj)) == CMSRET_SUCCESS)
        {
            printf("   Extended VLAN default rule mode: ");
            if (obj->extVlanDefaultRuleEnable == TRUE)
            {
                printf("enabled\n");
            }
            else
            {
                printf("disabled\n");
            }
            cmsObj_free((void**)&obj);
        }
        else
        {
            cmsLog_error("Could not cmsObj_get OmciSystemObject, ret=%d", ret);
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciMiscConfig
 * Description  : configure Extended VLAN default rule enable mode.
 ***************************************************************************/
static CmsRet cmdOmciMiscConfig(UBOOL8 mode)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) ==
      CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack,
          0, (void*)&obj)) == CMSRET_SUCCESS)
        {
            obj->extVlanDefaultRuleEnable = mode;
            cmsObj_set(obj, &iidStack);
            cmsObj_free((void**)&obj);
            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    cmdOmciMiscStatus();

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciMiscHandler
 * Description  : OMCI miscellaneous command handler.
 ***************************************************************************/
void cmdOmciMiscHandler(SINT32 argc, char *argv[])
{
    if (argv[1] == NULL)
    {
        cmdOmciHelp("misc");
    }
    else if (strcasecmp(argv[1], "--help") == 0)
    {
        cmdOmciHelp("misc");
    }
    else if (strcasecmp(argv[1], "--status") == 0)
    {
        cmdOmciMiscStatus();
    }
    else if (strcasecmp(argv[1], "--extvlandefault") == 0)
    {
        if (argv[2] == NULL)
        {
            cmsLog_error("bridge: Missing arguments\n");
            cmdOmciHelp("misc");
        }
        else
        {
            if(strcasecmp(argv[2], "on") != 0 &&
              strcasecmp(argv[2], "off") != 0)
            {
                cmsLog_error("Misc: Invalid argument '%s'\n", argv[2]);
                cmdOmciHelp("misc");
            }
            else
            {
                if(strcasecmp(argv[2], "on") == 0)
                {
                    cmdOmciMiscConfig(TRUE);
                }
                else if(strcasecmp(argv[2], "off") == 0)
                {
                    cmdOmciMiscConfig(FALSE);
                }
            }
        }
    }
}

/***************************************************************************
 * Function Name: cmdOmciVoiceHandler
 * Description  : Voice model command handler.
 ***************************************************************************/
void cmdOmciVoiceHandler(SINT32 argc, char **argv)
{
    UINT16 val0;

    if ((argc == 3) && (argv[1] != NULL) &&
      (strcasecmp(argv[1], "--model") == 0))
    {
        val0 = strtoul(argv[2], (char**)NULL, 0);
        if (val0 <= 1)
        {
            cmdOmciVoiceModelSet(val0);
        }
    }
    else if ((argc == 2) && (argv[1] != NULL) &&
      (strcasecmp(argv[1], "--status") == 0))
    {
        cmdOmciVoiceModelGet();
    }
    else
    {
        cmdOmciHelp("voice");
    }
}

/***************************************************************************
 * Function Name: cmdOmciJoinForceForwardStatus
 * Description  : show current upstream join force forwarding option.
 ***************************************************************************/
static CmsRet cmdOmciJoinForceForwardStatus(void)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT))
      == CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void*)&obj))
          == CMSRET_SUCCESS)
        {
            printf("   Upstream join force forwarding option:\n");
            if (obj->joinForceForward == FALSE)
            {
                printf("    (0) Forwarding based on OMCI "
                       "'unauthorized join request behaviour' attribute.\n\n");
            }
            else
            {
                printf("    (1) Force forwarding.\n\n");
            }

            cmsObj_free((void**)&obj);
        }
        else
        {
            cmsLog_error("Could not cmsObj_get OmciSystemObject, ret=%d", ret);
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciJoinForceForwardConfig
 * Description  : configure upstream join mode:
 *                TRUE - force forwarding.
 *                FALSE - forwarding based on OMCI "unauthorized join
 *                  request behaviour" attribute.
 ***************************************************************************/
static CmsRet cmdOmciJoinForceForwardConfig(UBOOL8 mode)
{
    CmsRet ret;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BcmOmciConfigSystemObject *obj = NULL;

    if ((ret = cmsLck_acquireLockWithTimeout(CLIAPP_LOCK_TIMEOUT)) ==
      CMSRET_SUCCESS)
    {
        if ((ret = cmsObj_get(MDMOID_BCM_OMCI_CONFIG_SYSTEM, &iidStack, 0, (void*)&obj))
          == CMSRET_SUCCESS)
        {
            obj->joinForceForward = mode;
            cmsObj_set(obj, &iidStack);
            cmsObj_free((void**)&obj);
            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    cmdOmciJoinForceForwardStatus();

    return ret;
}

/***************************************************************************
 * Function Name: cmdOmciMcastHandler
 * Description  : OMCI multicast command handler.
 ***************************************************************************/
void cmdOmciMcastHandler(SINT32 argc, char *argv[])
{
    if (argv[1] == NULL)
    {
        cmdOmciHelp("mcast");
    }
    else if (strcasecmp(argv[1], "--help") == 0)
    {
        cmdOmciHelp("mcast");
    }
    else if (strcasecmp(argv[1], "--status") == 0)
    {
        cmdOmciMcastHostctrlStatus();
        cmdOmciJoinForceForwardStatus();
    }
    else if (strcasecmp(argv[1], "--hostctrl") == 0)
    {
        if (argv[2] == NULL)
        {
            cmsLog_error("Multicast: Missing arguments\n");
            cmdOmciHelp("mcast");
        }
        else
        {
            if(strcasecmp(argv[2], "on") != 0 &&
              strcasecmp(argv[2], "off") != 0)
            {
                cmsLog_error("Multicast: Invalid argument '%s'\n", argv[2]);
                cmdOmciHelp("mcast");
            }
            else
            {
                if(strcasecmp(argv[2], "on") == 0)
                {
                    cmdOmciMcastHostctrlConfig(TRUE);
                }
                else if(strcasecmp(argv[2], "off") == 0)
                {
                    cmdOmciMcastHostctrlConfig(FALSE);
                }
            }
        }
    }
    else if (strcasecmp(argv[1], "--joinfwd") == 0)
    {
        if (argv[2] == NULL)
        {
            cmsLog_error("Multicast: Missing arguments\n");
            cmdOmciHelp("mcast");
        }
        else
        {
            if(strcasecmp(argv[2], "on") != 0 &&
              strcasecmp(argv[2], "off") != 0)
            {
                cmsLog_error("Multicast: Invalid argument '%s'\n", argv[2]);
                cmdOmciHelp("mcast");
            }
            else
            {
                if(strcasecmp(argv[2], "on") == 0)
                {
                    cmdOmciJoinForceForwardConfig(TRUE);
                }
                else if(strcasecmp(argv[2], "off") == 0)
                {
                    cmdOmciJoinForceForwardConfig(FALSE);
                }
            }
        }
    }
}

/***************************************************************************
 * Function Name: cmdOmciSetTable
 * Description  : Generate OMCI Set table request message.
 ***************************************************************************/
static void cmdOmciSetTable(UINT16 tcId, UINT16 meClass, UINT16 meInst,
  UINT16 index, UINT8 *bufP, UINT32 bufSize)
{
    UINT16 msgSize = sizeof(CmsMsgHeader) + sizeof(omciPacket);
    UINT16 meAttrMask = 0;
    char buf[msgSize];
    CmsMsgHeader *msgP = (CmsMsgHeader*)buf;
    CmsMsgHeader *respMsg;
    omciPacket *packetP = (omciPacket*)(msgP + 1);
    UINT32 crc32;
    CmsRet ret = CMSRET_SUCCESS;

    memset(buf, 0x0, msgSize);
    msgP->type = CMS_MSG_OMCI_COMMAND_REQUEST;
    msgP->src = EID_CONSOLED;
    msgP->dst = EID_OMCID;
    msgP->dataLength = sizeof(omciPacket);
    msgP->flags_event = 0;
    msgP->flags_request = 0;
    msgP->sequenceNumber = 0;

    OMCI_HTONS(&packetP->tcId, tcId);
    packetP->msgType = OMCI_MSG_TYPE_AR(OMCI_MSG_TYPE_SETTABLE);
    packetP->devId = OMCI_PACKET_DEV_ID_B;
    OMCI_HTONS(&packetP->classNo, meClass);
    OMCI_HTONS(&packetP->instId, meInst);

    OMCI_HTONS(&packetP->B.msgLen, bufSize + sizeof(meAttrMask));
    meAttrMask = 0x8000 >> index;
    OMCI_HTONS(&packetP->B.msg[0], meAttrMask);
    memcpy(&packetP->B.msg[OMCI_SET_OVERHEAD], bufP, bufSize);

    packetP->src_eid = EID_CONSOLED;
    crc32 = omciUtl_getCrc32(-1, (char*)packetP,
      OMCI_PACKET_SIZE(packetP) - OMCI_PACKET_MIC_SIZE);
    OMCI_HTONL(OMCI_PACKET_CRC(packetP), crc32);
    ret = cmsMsg_send(cliPrvtMsgHandle, msgP);
    ret = (ret == CMSRET_SUCCESS) ? cmsMsg_receiveWithTimeout(cliPrvtMsgHandle,
      &respMsg, MSG_RECV_TIMEOUT_MS) : ret;
    if (ret == CMSRET_SUCCESS)
    {
        if (respMsg->type != CMS_MSG_OMCI_COMMAND_RESPONSE)
        {
            cmsLog_error("Failed to receive rsp, type=0x%x", respMsg->type);
        }
        CMSMEM_FREE_BUF_AND_NULL_PTR(respMsg);
    }
    else
    {
        cmsLog_error("cmsMsg_send() or cmsMsg_receiveWithTimeout() failure, "
          "ret=%d", ret);
    }
}

/***************************************************************************
 * Function Name: cmdOmciSetTableHandler
 * Description  : OMCI Set table command handler.
 ***************************************************************************/
void cmdOmciSetTableHandler(SINT32 argc, char *argv[])
{
    UINT16 val0;
    UINT16 val1;
    UINT16 val2;
    UINT16 val3;
    UINT8 *buf;
    UINT32 bufSize;
    CmsRet ret = CMSRET_SUCCESS;

    if ((argc == 11) &&
      (strcasecmp(argv[1], "--tcId") == 0) &&
      (strcasecmp(argv[3], "--meclass") == 0) &&
      (strcasecmp(argv[5], "--meid") == 0) &&
      (strcasecmp(argv[7], "--index") == 0) &&
      (strcasecmp(argv[9], "--val") == 0))
    {
        val0 = strtoul(argv[2], (char**)NULL, 0);
        val1 = strtoul(argv[4], (char**)NULL, 0);
        val2 = strtoul(argv[6], (char**)NULL, 0);
        val3 = strtoul(argv[8], (char**)NULL, 0);
        ret = cmsUtl_hexStringToBinaryBuf(argv[10], &buf, &bufSize);
        if (ret == CMSRET_SUCCESS)
        {
            cmdOmciSetTable(val0, val1, val2, val3, buf, bufSize);
            CMSMEM_FREE_BUF_AND_NULL_PTR(buf);
        }
        else
        {
            cmsLog_error("Invalid hex string, ret=%d", ret);
        }
    }
    else
    {
        cmdOmciHelp("settable");
    }
}

/***************************************************************************
 * Function Name: processOmciCmd
 * Description  : Parses OMCI commands
 ***************************************************************************/
void processOmciCmd(char *cmdLine)
{
    SINT32 argc = 0;
    char *argv[MAX_OPTS]={NULL};
    char *last = NULL;

    /* parse the command line and build the argument vector */
    argv[0] = strtok_r(cmdLine, " ", &last);

    if(argv[0] != NULL)
    {
        for(argc=1; argc<MAX_OPTS; ++argc)
        {
            argv[argc] = strtok_r(NULL, " ", &last);

            if (argv[argc] == NULL)
            {
                break;
            }

            cmsLog_debug("arg[%d]=%s", argc, argv[argc]);
        }
    }

    if (NULL == argv[0])
    {
        cmdOmciHelp(NULL);
    }
    else if(strcasecmp(argv[0], "capture") == 0)
    {
        cmdOmciCapture(argv[1], argv[2], argv[3], argv[4], argv[5]);
    }
    else if(strcasecmp(argv[0], "--help") == 0)
    {
        cmdOmciHelp(argv[0]);
    }
    else if(strcasecmp(argv[0], "grab") == 0)
    {
        cmdOmciGrab(argv[1], argv[2], argv[3], argv[4]);
    }
    else if(strcasecmp(argv[0], "send") == 0)
    {
        cmdOmciSend(argv[1], argv[2]);
    }
    else if(strcasecmp(argv[0], "tcont") == 0)
    {
        if (argv[1] == NULL)
            cmdOmciHelp("tcont");
        else if (strcasecmp(argv[1], "--portmax") == 0)
        {
            if (argc < 5 ||
                argv[2] == NULL ||
                argv[3] == NULL ||
                argv[4] == NULL)
            {
                cmsLog_error("Tcont: Missing arguments\n");
                cmdOmciHelp("tcont");
            }
            else if (strcasecmp(argv[3], "--startid") != 0)
            {
                cmsLog_error("Tcont: Invalid argument '%s'\n", argv[3]);
                cmdOmciHelp("tcont");
            }
            else
                cmdOmciTcont(argv[2], argv[4]);
        }
        else if (strcasecmp(argv[1], "--port") == 0)
        {
            if (argc < 5 ||
                argv[2] == NULL ||
                argv[3] == NULL ||
                argv[4] == NULL)
            {
                cmsLog_error("Tcont: Missing arguments\n");
                cmdOmciHelp("tcont");
            }
            else if (strcasecmp(argv[3], "--policy") != 0)
            {
                cmsLog_error("Tcont: Invalid argument '%s'\n", argv[3]);
                cmdOmciHelp("tcont");
            }
            else if (strcasecmp(argv[4], "sp") != 0 && strcasecmp(argv[4], "wrr") != 0)
            {
                cmsLog_error("Tcont: Invalid argument '%s'\n", argv[4]);
                cmdOmciHelp("tcont");
            }
            else
                cmdOmciTcontPolicy(argv[2], argv[4]);
        }
    }
    else if(strcasecmp(argv[0], "eth") == 0)
    {
        if (argv[1] == NULL)
            cmdOmciHelp("eth");
        else if (strcasecmp(argv[1], "--portmax") == 0)
        {
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
            if (argc < 7 ||
#else
            if (argc < 5 ||
#endif
                argv[2] == NULL ||
                argv[3] == NULL ||
                argv[4] == NULL)
            {
                cmsLog_error("Ethernet: Missing arguments\n");
                cmdOmciHelp("eth");
            }
            else if (strcasecmp(argv[3], "--startid") != 0)
            {
                cmsLog_error("Ethernet: Invalid argument '%s'\n", argv[3]);
                cmdOmciHelp("eth");
            }
            else
            {
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
                if (strcasecmp(argv[5], "--veip") != 0)
                {
                    cmsLog_error("Ethernet: Invalid argument '%s'\n", argv[5]);
                    cmdOmciHelp("eth");
                }
                else
                    cmdOmciEth(argv[2], argv[4], argv[6]);
#else
                cmdOmciEth(argv[2], argv[4]);
#endif
            }
        }
#ifdef DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1
        else if (strcasecmp(argv[1], "--port") == 0)
        {
            if (argc < 5 ||
                argv[2] == NULL ||
                argv[3] == NULL ||
                argv[4] == NULL)
            {
                cmsLog_error("Ethernet: Missing arguments\n");
                cmdOmciHelp("eth");
            }
            else if (strcasecmp(argv[3], "--type") != 0)
            {
                cmsLog_error("Ethernet: Invalid argument '%s'\n", argv[3]);
                cmdOmciHelp("eth");
            }
            else if (strcasecmp(argv[4], ETH_PORT_TYPE_RG) != 0 &&
                strcasecmp(argv[4], ETH_PORT_TYPE_ONT) != 0)
            {
                cmsLog_error("Debug: Invalid argument '%s'\n", argv[4]);
                cmdOmciHelp("eth");
            }
            else
                cmdEthPortTypeConfig(argv[2], argv[4]);
        }
        else if (strcasecmp(argv[1], "--show") == 0)
        {
            cmdEthPortTypeShow();
        }
#endif   /* DMP_X_BROADCOM_COM_GPONRG_OMCI_FULL_1 */
    }
   else if(strcasecmp(argv[0], "mibsync") == 0)
   {
       if (argv[1] == NULL)
           cmdOmciHelp("mibsync");
       else if (strcasecmp(argv[1], "--help") == 0)
           cmdOmciHelp("mibsync");
       else if (strcasecmp(argv[1], "--get") == 0)
           cmdOmciMibSyncGet();
       /* To set MIB data cync: use WEB GUI OMCI configuration page. */
   }
    else if(strcasecmp(argv[0], "unipathmode") == 0)
    {
        if (argv[1] == NULL)
            cmdOmciHelp("unipathmode");
        else if (strcasecmp(argv[1], "--help") == 0)
            cmdOmciHelp("unipathmode");
        else if (strcasecmp(argv[1], "--status") == 0)
            cmdOmciUniPathModeStatus();
        else if (strcasecmp(argv[1], "--mode") == 0)
        {
            if (argv[2] == NULL)
            {
                cmsLog_error("UNI data path mode: Missing arguments\n");
                cmdOmciHelp("unipathmode");
            }
            else
            {
                if (strcasecmp(argv[2], "on") != 0 &&
                    strcasecmp(argv[2], "off") != 0)
                {
                    cmsLog_error("UNI data path mode: Invalid argument '%s'\n", argv[2]);
                    cmdOmciHelp("unipathmode");
                }
                else
                {
                    if (strcasecmp(argv[2], "on") == 0)
                    {
                        cmdOmciPathModeConfig(1);
                    }
                    else if (strcasecmp(argv[2], "off") == 0)
                    {
                        cmdOmciPathModeConfig(0);
                    }
                }
            }
        }
    }
    else if(strcasecmp(argv[0], "promiscmode") == 0)
    {
        if (argv[1] == NULL)
            cmdOmciHelp("promiscmode");
        else if (strcasecmp(argv[1], "--help") == 0)
            cmdOmciHelp("promiscmode");
        else if (strcasecmp(argv[1], "--status") == 0)
            cmdOmciPromiscModeStatus();
        else if (strcasecmp(argv[1], "--mode") == 0)
        {
            if (argv[2] == NULL)
            {
                cmsLog_error("Promisc mode: Missing arguments\n");
                cmdOmciHelp("promiscmode");
            }
            else
            {
                if(strcasecmp(argv[2], "on") != 0 &&
                    strcasecmp(argv[2], "off") != 0)
                {
                    cmsLog_error("Promisc mode: Invalid argument '%s'\n", argv[2]);
                    cmdOmciHelp("promiscmode");
                }
                else
                {
                    if(strcasecmp(argv[2], "on") == 0)
                    {
                        cmdOmciPromiscModeConfig(1);
                    }
                    else if(strcasecmp(argv[2], "off") == 0)
                    {
                        cmdOmciPromiscModeConfig(0);
                    }
                }
            }
        }
    }
    else if(strcasecmp(argv[0], "rawmode") == 0)
    {
        if (argv[1] == NULL)
            cmdOmciHelp("rawmode");
        else if (strcasecmp(argv[1], "--help") == 0)
            cmdOmciHelp("rawmode");
        else if (strcasecmp(argv[1], "--status") == 0)
            cmdOmciRawModeStatus();
        else if (strcasecmp(argv[1], "--mode") == 0)
        {
            if (argv[2] == NULL)
            {
                cmsLog_error("Raw mode: Missing arguments\n");
                cmdOmciHelp("rawmode");
            }
            else
            {
                if(strcasecmp(argv[2], "on") != 0 &&
                    strcasecmp(argv[2], "off") != 0)
                {
                    cmsLog_error("Raw mode: Invalid argument '%s'\n", argv[2]);
                    cmdOmciHelp("rawmode");
                }
                else
                {
                    if(strcasecmp(argv[2], "on") == 0)
                    {
                        cmdOmciRawModeConfig(TRUE);
                    }
                    else if(strcasecmp(argv[2], "off") == 0)
                    {
                        cmdOmciRawModeConfig(FALSE);
                    }
                }
            }
        }
    }
    else if (strcasecmp(argv[0], "tmoption") == 0)
    {
        cmdOmciTmOptionHandler(argc, argv);
    }
    else if (strcasecmp(argv[0], "debug") == 0)
    {
        if (argv[1] == NULL)
            cmdOmciHelp("debug");
        else if (strcasecmp(argv[1], "--mkerr") == 0)
            cmdOmciDgbMkErr(argv[2]);
        else if (strcasecmp(argv[1], "--help") == 0)
            cmdOmciHelp("debug");
        else if (strcasecmp(argv[1], "--status") == 0)
            cmdOmciDebugStatus();
        else if (strcasecmp(argv[1], "--dump") == 0)
        {
            if (argv[2] == NULL)
            {
                cmsLog_error("Debug: Missing arguments\n");
                cmdOmciHelp("debug");
            }
            else
                cmdOmciDebugDump(argv[2]);
        }
        else if (strcasecmp(argv[1], "--module") == 0)
        {
            if (argc < 5 ||
                argv[2] == NULL ||
                argv[3] == NULL ||
                argv[4] == NULL)
            {
                cmsLog_error("Debug: Missing arguments\n");
                cmdOmciHelp("debug");
            }
            else if(strcasecmp(argv[2], "all") != 0 &&
                strcasecmp(argv[2], "omci") != 0 &&
                strcasecmp(argv[2], "model") != 0 &&
                strcasecmp(argv[2], "vlan") != 0 &&
                strcasecmp(argv[2], "cmf") != 0 &&
                strcasecmp(argv[2], "flow") != 0 &&
                strcasecmp(argv[2], "rule") != 0 &&
                strcasecmp(argv[2], "mcast") != 0 &&
                strcasecmp(argv[2], "voice") != 0 &&
                strcasecmp(argv[2], "file") != 0)
            {
                cmsLog_error("Debug: Invalid argument '%s'\n", argv[2]);
                cmdOmciHelp("debug");
            }
            else if (strcasecmp(argv[3], "--state") != 0)
            {
                cmsLog_error("Debug: Invalid argument '%s'\n", argv[3]);
                cmdOmciHelp("debug");
            }
            else if(strcasecmp(argv[4], "on") != 0 &&
                strcasecmp(argv[4], "off") != 0)
            {
                cmsLog_error("Debug: Invalid argument '%s'\n", argv[4]);
                cmdOmciHelp("debug");
            }
            else
                cmdOmciDebug(argv[2], argv[4]);
        }
        else if (strcasecmp(argv[1], "--info") == 0)
        {
            if (argc < 3)
            {
                cmsLog_error("Debug: Missing arguments\n");
                cmdOmciHelp("debug");
            }
            else
            {
                cmdOmciDebugInfo(argv[2]);
            }
        }
        else
        {
            cmsLog_error("Debug: Invalid argument '%s'\n", argv[1]);
            cmdOmciHelp("debug");
        }
    }
    else if(strcasecmp(argv[0], "mcast") == 0)
    {
        cmdOmciMcastHandler(argc, argv);
    }
    else if (strcasecmp(argv[0], "settable") == 0)
    {
        cmdOmciSetTableHandler(argc, argv);
    }
    else if (strcasecmp(argv[0], "test") == 0)
    {
        cmdOmciTestHandler(argc, argv);
    }
    else if (strcasecmp(argv[0], "bridge") == 0)
    {
        cmdOmciBridgeHandler(argc, argv);
    }
    else if (strcasecmp(argv[0], "misc") == 0)
    {
        cmdOmciMiscHandler(argc, argv);
    }
#if defined(DMP_X_ITU_ORG_VOICE_1)
    else if (strcasecmp(argv[0], "voice") == 0)
    {
        cmdOmciVoiceHandler(argc, argv);
    }
#endif /* DMP_X_ITU_ORG_VOICE_1 */
    else
    {
        printf("Invalid OMCI CLI Command\n");
        cmdOmciHelp(NULL);
    }
}

/***************************************************************************
 * Function Name: cmdOmcipmHelp
 * Description  : Prints help information about the OMCIPM commands
 ***************************************************************************/
static void cmdOmcipmHelp(char *help)
{
    if(help == NULL || strcasecmp(help, "--help") == 0)
    {
        printf("%s%s", omcipmusage, omcipmdebug);
    }
    else if(strcasecmp(help, "debug") == 0)
    {
        printf("%s%s", omcipmusage, omcipmdebug);
    }
}

/***************************************************************************
 * Function Name: cmdOmcipmDebug
 * Description  : send debug message from CONSOLED to OMCIPMD
 ***************************************************************************/
static CmsRet cmdOmcipmDebug(char *pmid, char *portid, char *state)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader reqMsg;
    CLI_OMCI_PM_DEBUG debug;

    memset(&debug, 0, sizeof(debug));

    if (strcmp(state, "on") == 0)
        debug.omcipmDebug.info.state = 1;
    debug.omcipmDebug.info.pmId = (UINT8)strtoul(pmid, (char **)NULL, 0);
    debug.omcipmDebug.info.portId = (UINT16)strtoul(portid, (char **)NULL, 0);

    memset(&reqMsg, 0, sizeof(CmsMsgHeader));
    reqMsg.type = CMS_MSG_OMCIPMD_DEBUG;
    reqMsg.src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
    reqMsg.dst = EID_OMCIPMD;
    reqMsg.flags_request = 1;
    reqMsg.dataLength = 0;
    reqMsg.sequenceNumber = 1;

    reqMsg.wordData = debug.omcipmDebug.all;

    if ((ret = cmsMsg_send(cliPrvtMsgHandle, &reqMsg)) != CMSRET_SUCCESS)
        cmsLog_error("Could not send out CMS_MSG_OMCIPMD_DEBUG, ret=%d from CONSOLED to OMCIPMD", ret);
    else
        cmsLog_notice("Send CMS_MSG_OMCIPMD_DEBUG from CONSOLED to OMCIPMD");

    return ret;
}


/***************************************************************************
 * Function Name: cmdOmciAlarmSeq_Get
 * Description  : Prints OMCIPMD current Alarm Sequence Number.
 ***************************************************************************/
static void cmdOmciAlarmSeq_Get()
{
  CmsRet cmsReturn = CMSRET_SUCCESS;
  CmsMsgHeader cmdMsgBuffer;

  // Setup CMS_MSG_OMCIPMD_ALARM_SEQ_GET command.
  memset(&cmdMsgBuffer, 0x0, sizeof(CmsMsgHeader));
  cmdMsgBuffer.type = CMS_MSG_OMCIPMD_ALARM_SEQ_GET;
  cmdMsgBuffer.src = EID_CONSOLED;
  cmdMsgBuffer.dst = EID_OMCID;
  cmdMsgBuffer.flags_request = 1;
  cmdMsgBuffer.dataLength = 0;

  // Attempt to send CMS message & test result.
  cmsReturn = cmsMsg_send(cliPrvtMsgHandle, (CmsMsgHeader*)&cmdMsgBuffer);
  if (cmsReturn != CMSRET_SUCCESS)
  {
    // Log error.
    cmsLog_error("Send message failure, cmsReturn: %d", cmsReturn);
  }
}


/***************************************************************************
 * Function Name: cmdOmciAlarmSeq_Get
 * Description  : Sets OMCIPMD new Alarm Sequence Number from input arg.
 ***************************************************************************/
static void cmdOmciAlarmSeq_Set(char* numValStr)
{
  CmsRet cmsReturn = CMSRET_SUCCESS;
  CmsMsgHeader cmdMsgBuffer;
  int seqVal = 0;

  // Test for valid pointer.
  if (numValStr != NULL)
  {
    // Convert input string to int.
    seqVal = atoi(numValStr);
  }

  // Test for valid range.
  if ((seqVal > 0) && (seqVal < 256))
  {
    // Setup CMS_MSG_OMCIPMD_ALARM_SEQ_SET command.
    memset(&cmdMsgBuffer, 0x0, sizeof(CmsMsgHeader));
    cmdMsgBuffer.type = CMS_MSG_OMCIPMD_ALARM_SEQ_SET;
    cmdMsgBuffer.src = EID_CONSOLED;
    cmdMsgBuffer.dst = EID_OMCID;
    cmdMsgBuffer.flags_request = 1;
    cmdMsgBuffer.dataLength = 0;
    cmdMsgBuffer.wordData = (UINT32)seqVal;

    // Attempt to send CMS message & test result.
    cmsReturn = cmsMsg_send(cliPrvtMsgHandle, (CmsMsgHeader*)&cmdMsgBuffer);
    if (cmsReturn != CMSRET_SUCCESS)
    {
      // Log error.
      cmsLog_error("Send message failure, cmsReturn: %d", cmsReturn);
    }
  }
  else
  {
    printf("*** cmdOmciAlarmSeq_Set error valid range <1...255> ***\n");
  }
}


/***************************************************************************
 * Function Name: processOmcipmCmd
 * Description  : Parses OMCIPM commands
 ***************************************************************************/
void processOmcipmCmd(char *cmdLine)
{
    SINT32 argc = 0;
    char *argv[MAX_OPTS]={NULL};
    char *last = NULL;

    /* parse the command line and build the argument vector */
    argv[0] = strtok_r(cmdLine, " ", &last);

    if(argv[0] != NULL)
    {
        for(argc=1; argc<MAX_OPTS; ++argc)
        {
            argv[argc] = strtok_r(NULL, " ", &last);

            if (argv[argc] == NULL)
            {
                break;
            }

            cmsLog_debug("arg[%d]=%s", argc, argv[argc]);
        }
    }

    if (argv[0] == NULL)
    {
        cmdOmcipmHelp(NULL);
    }
    else if(strcasecmp(argv[0], "--getAlarmSeq") == 0)
    {
        cmdOmciAlarmSeq_Get();
    }
    else if(strcasecmp(argv[0], "--setAlarmSeq") == 0)
    {
        cmdOmciAlarmSeq_Set(argv[1]);
    }
    else if(strcasecmp(argv[0], "--help") == 0)
    {
        cmdOmcipmHelp(argv[0]);
    }
    else if(strcasecmp(argv[0], "debug") == 0)
    {
        if (argv[1] == NULL)
            cmdOmcipmHelp("debug");
        else if (strcasecmp(argv[1], "--pmid") == 0)
        {
            if (argc < 7 ||
                argv[2] == NULL || argv[3] == NULL ||
                argv[4] == NULL || argv[5] == NULL ||
                argv[6] == NULL)
            {
                cmsLog_error("Debug: Missing arguments\n");
                cmdOmcipmHelp("debug");
            }
            else if (strcasecmp(argv[3], "--portid") != 0)
            {
                cmsLog_error("Debug: Invalid argument '%s'\n", argv[3]);
                cmdOmcipmHelp("debug");
            }
            else if (strcasecmp(argv[5], "--state") != 0)
            {
                cmsLog_error("Debug: Invalid argument '%s'\n", argv[5]);
                cmdOmcipmHelp("debug");
            }
            else if(strcasecmp(argv[6], "on") != 0 &&
                strcasecmp(argv[6], "off") != 0)
            {
                cmsLog_error("Debug: Invalid argument '%s'\n", argv[6]);
                cmdOmciHelp("debug");
            }
            else
                cmdOmcipmDebug(argv[2], argv[4], argv[6]);
        }
    }
    else
    {
        printf("Invalid OMCIPM CLI Command\n");
        cmdOmcipmHelp(NULL);
    }
}

#endif /* SUPPORT_OMCI */

#endif /* SUPPORT_CLI_CMD */
