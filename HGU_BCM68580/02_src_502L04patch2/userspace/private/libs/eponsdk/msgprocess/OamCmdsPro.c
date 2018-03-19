/*
* <:copyright-BRCM:2017:proprietary:epon
* 
*    Copyright (c) 2017 Broadcom 
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TkMsgProcess.h"
#include "TkOamCommon.h"
#include "TkPlatform.h"
#include "config.h"
#include "OamCmdsPro.h"
#include "OnuOs.h"
#include "OamIeeeCommon.h"
#ifdef DPOE_OAM_BUILD
#include "OamDpoeSla.h"
#include "OamDpoeBridgeCommon.h"
#endif

typedef void (*OamCmdsFun) (OamCmdsMsg* cmds);

#define OAM_MSG_LOG_FILE_SIZE  0x200000      /* max size: 2M */
const char *OamMsgLogFile = "/tmp/oammsg.log";
static OamMsgLogLevel OamMsgDump[TkOnuNumTxLlids];
static U8 OamMsgToFile = OAM_DUMP_CMD_CONSOLE;

OamMsgLogLevel OamMsgLog_getLevel(U8 link)
{
    if (link < TkOnuNumTxLlids)
    {
        return OamMsgDump[link];
    }
    printf("OamMsgDumpCfg_get error! (link: %d)\n", link);
    return -1;
}

void OamMsgLog_setLevel(U8 link, OamMsgLogLevel level)
{
    if (link < TkOnuNumTxLlids)
    {
        OamMsgDump[link] = level;
    }
    else
    {
        printf("OamMsgDumpCfg_set error! (link: %d)\n", link);
    }
}

static void OamCmdsDumpOamState(OamCmdsMsg* cmds)
{
    U8 link = 0;
    OamPerLinkData *data = NULL;
    
    if (cmds->paraLength == 1)
        link = cmds->paraData[0];
    
    data = OamIeeeGetData(link);

    if (!data)
    {
        printf("Dump link-%u oam data error\n", link);
    }
    else
    {
        printf("//////////////////////////////// \n");
        printf("Dump link-%u oam data \n", link);
        printf("//////////////////////////////// \n");
        
        printf("discState            : %u \n", data->discState);
        printf("maxPduSize           : %u \n", data->maxPduSize);
        printf("minRate              : %u \n", data->minRate);
        printf("ticksSinceSend       : %u \n", data->ticksSinceSend);
        printf("ticksUntilLinkFail   : %u \n", data->ticksUntilLinkFail);
        printf("txCredits            : %u \n", data->txCredits);
        printf("framesRx             : %llu \n", data->framesRx);
        printf("framesTx             : %llu \n", data->framesTx);
        printf("orgSpecificTx        : %llu \n", data->orgSpecificTx);
        printf("orgSpecificRx        : %llu \n", data->orgSpecificRx);
        printf("informationRx        : %llu \n", data->informationRx);
        printf("informationTx        : %llu \n", data->informationTx);
        printf("localInfoRevision    : %u \n", data->localInfoRevision);
        printf("vendorNeg            : %u \n", data->vendorNeg);
    }
}


static void OamCmdsDumpAlarm(OamCmdsMsg* cmds)
{
    U32 id = 0;
    U64 value = 0;

    printf("//////////////////////////////// \n");
    printf("Dump oam stack alarm state \n");
    printf("//////////////////////////////// \n");

    for (id = 0; id < OsAstNums; id++)
    {
        value = OnuOsAssertDbAll(id);
        printf("id : %u, value : 0x%llx \n", id, value);
    }
}


static void OamCmdsDumpVars(OamCmdsMsg* cmds)
{
    U8   cmd;

    cmd = cmds->paraData[0];

    if (cmd == OAM_VARS_QUEUE_RL)
    {
#ifdef DPOE_OAM_BUILD
        U8 link, queueId;
        OamDpoeQueueShaper queueShaper;

        printf("OAM queue shaper dump:\n");
        for(link = 0; link < MAX_DPOE_10G_LINK_NUM; link++)
        {
            for (queueId = 0; queueId < MAX_DPOE_LINK_Q_NUM; queueId ++)
            {
                OamDpoeQueueShaperGet(link, queueId, &queueShaper);
                if ((queueShaper.CIR.queueRate != 0) || (queueShaper.EIR.queueRate != 0))
                {
                    printf("\nlink: %d, queueId %d\n", link, queueId);
                    printf("CIR (kbps): %10d ", queueShaper.CIR.queueRate);
                    printf("CBS (256bytes): %d\n", queueShaper.CIR.queueRate);
                    printf("EIR (kbps): %10d ", queueShaper.EIR.queueRate);
                    printf("EBS (256bytes): %d\n", queueShaper.EIR.queueRate);
                }
            }
        }
#endif        
    }
}

static void OamCmdsDumpOamMsg(OamCmdsMsg* cmds)
{
    U8             cmd, link, start, end;
    OamMsgLogLevel level;

    cmd = cmds->paraData[0];
    link = cmds->paraData[1];
    level = cmds->paraData[2];

    /* show oam msg dump config */
    if (cmd == OAM_DUMP_CMD_SHOW)
    {
        printf("OAM message dump switch\n");
        printf("Link       Stats\n");
        printf("====================\n");
        for(link = 0; link < TkOnuNumTxLlids; link++)
        {
            level = OamMsgLog_getLevel(link);
            printf(" %02d        %s\n", link, level == OAM_DUMP_ALL?    "all":
                                              level == OAM_DUMP_ALARM?  "alarm":
                                              level == OAM_DUMP_NORMAL? "normal":
                                                                        "off");
        }
        printf("====================\n");
    }
    /* dump oam msg to file */
    else if(cmd == OAM_DUMP_CMD_FILE)
    {
        OamMsgToFile = OAM_DUMP_CMD_FILE;
        printf("OAM message is now logging to file \"/tmp/oammsg.log\"!\n ");
    }
    else if(cmd == OAM_DUMP_CMD_FILE_OVERWRITE)
    {
        OamMsgToFile = OAM_DUMP_CMD_FILE_OVERWRITE;
        printf("OAM message is now logging to file \"/tmp/oammsg.log\"!\n ");
    }
    /* dump oam msg to console */
    else if(cmd == OAM_DUMP_CMD_CONSOLE)
    {
        OamMsgToFile = OAM_DUMP_CMD_CONSOLE;
        printf("OAM message dump to console!\n");
    }
    /* config oam msg dump level */
    else if(cmd == OAM_DUMP_CMD_DUMP)
    {
        if ( link == 0xff)
        {
            start = 0;
            end = TkOnuNumTxLlids - 1;
        }
        else if (( link >= 0 ) && ( link < TkOnuNumTxLlids))
        {
            start = end = link;
        }
        else
        {
            printf("Wrong link number: %u!!!\n", link);
            return;
        }
        for(link = start; link <= end; link++)
        {
            OamMsgLog_setLevel(link, level);
        }
    }
    else
    {
        printf("Unknow oam msg dump command!\n");
    }
}

static
OamCmdsFun OamCmds[EponCmdTotalCount] = 
{
    OamCmdsDumpOamState,  //EponCmdDumpOamState
    OamCmdsDumpAlarm,     //EponCmdDumpOamAlm
    OamCmdsDumpOamMsg,    //EponCmdDumpOamMsg
    OamCmdsDumpVars       //OamCmdsDumpVars
};


////////////////////////////////////////////////////////////////////////////////
/// OamCmdsProcess:  Hanle the detail commands for oam stack from eponctl CLI
///
// Parameters:
/// \param cmds     the detail command message
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
//extern
void OamCmdsProcess(OamCmdsMsg *cmds)
{
    if (cmds->cmdType < EponCmdTotalCount)
    {
        if (OamCmds[cmds->cmdType] != NULL)
            OamCmds[cmds->cmdType](cmds);
    }
}

static void get_timestamp(char *buffer, int max_len)
{
    struct timespec cur;

    clock_gettime(CLOCK_MONOTONIC, &cur);
    sprintf(buffer, "%02ld:%02ld:%02ld.%06ld",
                    cur.tv_sec / 3600,
                    (cur.tv_sec % 3600) / 60,
                    cur.tv_sec % 60,
                    cur.tv_nsec / 1000);
    return;
}

static void DumpOamFrame(FILE *fd, void *buf, U32 len )
{
    U32 i;
    U8 * p = (U8 *)buf;

    if( fd != NULL)
    {
        for (i = 0; i < len; i++)
        {
            fprintf (fd, "%02X ", *(p+i));
            if ( (i + 1) % 8 == 0 )
            {
                fprintf(fd, " " );
            }
            if ((i + 1) % 16 == 0 )
            {
                fprintf(fd, "\n");
            }
        }
        fprintf (fd, "\n");
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            printf ("%02X ", *(p + i));
            if ( (i + 1) % 8 == 0 )
                printf( " " );
            if ( (i + 1) % 16 == 0 )
                printf("\n");
        }
        printf ("\n");
    }
}

void DumpOamMessage(LinkIndex link, OamOpcode opcode, U8 * Frame, U16 size, Direction dir)
{
    char timestamp[64];
    int fsize;
    FILE *fd=NULL;
    static bool OamMsgLogOverflow = FALSE;

    if((OamMsgLog_getLevel(link) != OAM_DUMP_OFF) &&
       ((OamMsgLog_getLevel(link) == OAM_DUMP_ALL) ||
        ((OamMsgLog_getLevel(link) == OAM_DUMP_ALARM) && (opcode == OamOpEventNotification)) ||
        ((OamMsgLog_getLevel(link) == OAM_DUMP_NORMAL) && (opcode != OamOpInfo))))
    {
        get_timestamp(timestamp, sizeof(timestamp));

        if( (OamMsgToFile == OAM_DUMP_CMD_FILE) || (OamMsgToFile == OAM_DUMP_CMD_FILE_OVERWRITE) )
        {
            fd = fopen(OamMsgLogFile, "a");
            if (fd == NULL)
            {
                fprintf(stderr, "Open log file \"%s\" FAILED!\n", OamMsgLogFile);
                return;
            }
            else
            {
                fseek( fd, 0, SEEK_END );
                fsize=ftell(fd);
                if( fsize > OAM_MSG_LOG_FILE_SIZE)
                {
                    if(OamMsgToFile == OAM_DUMP_CMD_FILE)
                    {
                        if (OamMsgLogOverflow == FALSE)
                        {
                             printf("Warning: OAM log file \"%s\" overflow!!! %s\n"
                                    "max size: %d\n", OamMsgLogFile, timestamp, OAM_MSG_LOG_FILE_SIZE);
                        }
                        OamMsgLogOverflow = TRUE;
                        fclose(fd);
                        return;
                    }
                    else
                    {
                        printf("Warning: OAM log file \"%s\" out of size and overwitten !!! %s\n",
                                OamMsgLogFile, timestamp);
                        fclose(fd);
                        fd = fopen(OamMsgLogFile, "w");
                        if (fd == NULL)
                        {
                             fprintf(stderr, "Open log file \"%s\" FAILED!\n", OamMsgLogFile);
                             return;
                         }                        
                    }
                }
                OamMsgLogOverflow = 0;
                fprintf(fd, "\n%s link[%x], size = %d, opcode = 0x%x\n", (dir == Upstream)? "Tx": "Rx", link, size, opcode);
                fprintf(fd, "%s, timestamp:%s\n", (opcode == OamOpEventNotification)? "transmit oam Notification" :
                                                                   (dir == Upstream)? "transmit oam response" :
                                                                                      "received oam request",
                                                                                       timestamp);
                DumpOamFrame(fd, Frame, size);
                fclose(fd);
            }
        }
        else
        {
            printf("\n%s link[%x], size = %d, opcode = 0x%x\n", (dir == Upstream)? "Tx": "Rx", link, size, opcode);
            printf("%s, timestamp:%s\n", (opcode == OamOpEventNotification)? "transmit oam Notification" :
                                                          (dir == Upstream)? "transmit oam response" :
                                                                             "received oam request",
                                                                              timestamp);
            DumpOamFrame(NULL, Frame, size);
        }
    }
}

//end of OamCmdsPro.c
