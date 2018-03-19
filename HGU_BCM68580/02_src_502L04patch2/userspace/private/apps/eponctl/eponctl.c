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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "os_defs.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/syscall.h>
#include <errno.h>

#include "eponctl_api.h"
#include "OamCmdsPro.h"

typedef struct command cmd_t;
typedef int (cmd_func_t)(int argc, char **argv);

struct command
{
    int         nargs;
    const char  *name;
    cmd_func_t  *func;
    const char  *help;
};

cmd_t *command_lookup(const char *cmd);
void command_help(const cmd_t *);
void command_helpall(void);

#define FIRST_ARG_OFFSET 2
extern void UDumpData (U8 * addr, U16 size);
extern EponCfgMoPara eponMoPara[] ;


static SINT32 InitConnWithEponApp(void)
{
    struct sockaddr_un serverAddr;
    SINT32 fd = -1;
    int rc = 0;

    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0)
        goto init_exit ;

    if ((rc = fcntl(fd, F_SETFD, FD_CLOEXEC)) != 0)
    {
        printf("set close-on-exec failed, rc=%d errno=%d", rc, errno);
        close(fd);
        fd = -1;
        goto init_exit ;
    }

    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_LOCAL;
    strncpy(serverAddr.sun_path, EPON_USER_MSG_ADDR, sizeof(serverAddr.sun_path));
  
    rc = connect(fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (rc != 0)
    {
        printf("connect to %s failed, rc=%d errno=%d", EPON_USER_MSG_ADDR, rc, errno);
        close(fd);
        fd = -1;
    }
    
init_exit:
    return fd;
}


static EponCfgItem GetCfgFieldId(char *name)
    {
    EponCfgItem i;

    for (i = 0;i < EponCfgItemCount;i++)
        {
        if (strcmp(name,eponMoPara[i].name) == 0)
            {
            return i;
            }
        }
    return i;
    }


static int DumpCfg(int argc, char **argv)
    {
    int ret = 0;
    EponCfgItem i;
    EponCfgParam value; 
    U8 j;        
    
    for (i = 0;i < EponCfgItemCount;i++)
        {
        printf("Field:%15s   Length:%d  ",eponMoPara[i].name,eponMoPara[i].len);
        memset(&value,0,sizeof(value));
        value.index = i;
        eponStack_CtlCfgEponCfg(EponGetOpe,&value); 
        if(i == EponCfgItemHoldover)
            {
            printf("Value: time %d\t,flags 0x%x\n", value.eponparm.holdoverval.time, 
                              value.eponparm.holdoverval.flags);
            }
        else if(i == EponCfgItemPsCfg)
            {
            printf("Value: type %d\t,mode %d\n", value.eponparm.pscfg.type, 
                              value.eponparm.pscfg.mode);
            }
        else
            {
            U8 *pramPtr;
			
            printf("value:");
            switch(eponMoPara[i].len)
                {
                case 1:
                    pramPtr = &(value.eponparm.u8);
                    break;
                case 2:
                    pramPtr = (U8*)&(value.eponparm.u16);
                    break;
                case 4:
                    pramPtr = (U8*)&(value.eponparm.u32);
                    break;                    
                default:
                    pramPtr = (U8*)&(value.eponparm.u48);
                    break;
                }
            
            for (j = 0;j< eponMoPara[i].len;j++)
                {
                printf("%02x ", pramPtr[j]);
                }
            printf("\n");
            }    
        }
    return ret;
    }


static int SetCfg(int argc, char **argv)
    {
    int ret = 0;
    EponCfgItem i;
    U8 j;
    EponCfgParam value; 

    argv += FIRST_ARG_OFFSET;
    if (strcmp(*argv,"help") == 0)
        {
        printf("Cfg field:\n");
        for (i = 0;i < EponCfgItemCount;i++)
            {
            printf("%s\n",eponMoPara[i].name);
            }
        }
    else
        {
        i = GetCfgFieldId(*argv);
        if (i  ==  EponCfgItemCount)
            {
            printf("error Cfg field\n");
            return ret;
            }
        argv++;
        memset(&value, 0, sizeof(value));
        if(i == EponCfgItemHoldover)
            {
            U16 oul16;
            oul16 = (U16)strtoul(*argv, NULL, 0);
            memcpy(&value.eponparm.u48[0], (U8*)&oul16, sizeof(U16));
            argv++;
            oul16 = (U16)strtoul(*argv, NULL, 0);
            memcpy(&value.eponparm.u48[2], (U8*)&oul16, sizeof(U16));
            }
        else if(i == EponCfgItemPsCfg)
            {
            value.eponparm.u16[0] = (U8)strtoul(*argv, NULL, 0);
            argv++;
            value.eponparm.u16[1] = (U8)strtoul(*argv, NULL, 0);
            }
        else
            {
            switch(eponMoPara[i].len)
                {
                case 1:
                    value.eponparm.u8 = strtoul(*argv, NULL, 0);
                    break;
                case 2:
                    {
                    U16 oul16 = (U16)strtoul(*argv, NULL, 0);
                    memcpy(&value.eponparm.u16[0], (U8*)&oul16, sizeof(U16));
                    break;
                    }
                case 4:
                    {
                    U32 oul32 = (U32)strtoul(*argv, NULL, 0);
                    memcpy(&value.eponparm.u32[0], (U8*)&oul32, sizeof(U32));
                    break;
                    }
                default:
                    for (j = 0;j < eponMoPara[i].len;j++)
                        {
                        value.eponparm.u48[j] = (U8)strtoul(*argv, NULL, 0);
                        argv++;
                        }
                    break;
                }
            }
        printf("i:%d\n",i);  
        value.index = i;
        ret = eponStack_CtlCfgEponCfg(EponSetOpe, &value);          
        }

    return ret;
    }

static int SetRegister(int argc, char **argv)
    {
    EponCtlOpe ope = EponSetOpe;
    U32 regStart = 0;
    U8 count = 0;
    U32 regVal = 0;
    int ret = 0;

    argv += FIRST_ARG_OFFSET;
    if (*argv) 
        {
        regStart = strtoul(*argv, NULL, 0);
        }

    argv++;

    if (*argv)
        {
        regVal = strtoul(*argv, NULL, 0);
        count = 1;
        }

    ret = eponStack_CtlCfgRegister(ope,regStart,count,&regVal);
    return ret;
    }


static int DumpRegister(int argc, char **argv)
    {
    EponCtlOpe ope = EponGetOpe;
    U32 regStart = 0;
    U8 count = 0;
    U32 regVal[100];
    int ret = 0;
    U8 i = 0;
    memset(regVal,0,sizeof(regVal));

    argv += FIRST_ARG_OFFSET;

    if (*argv)
        {
        regStart = strtoul(*argv, NULL, 0);
        }

    argv++;

    if (*argv)
        {
        count = strtoul(*argv, NULL, 0);
        }

    ret = eponStack_CtlCfgRegister(ope,regStart,count,regVal);

    for (i = 0;i < count;i++)
        {
        if (regStart > 0x12000000)
            {
            printf("%0x:0x%08x\n", regStart+i*4, regVal[i]);
            }
        else
            {
            printf("%0x:0x%08x\n", regStart+i, regVal[i]);
            }
        }
    return ret;
    }


static int DumpStats(int argc, char **argv)
    {
    U8 dumpid = 0;
    int ret = 0;

    argv += FIRST_ARG_OFFSET;

    if (*argv) 
        {
        dumpid = strtoul(*argv, NULL, 0);
        }  

    ret = eponStack_DumpStats(dumpid);


    return ret;
    }

static void dump_links_queue(U8 start_link, U8 end_link, epon_mac_q_cfg_t *cfg)
    {
    U8 start_q_id = 0;
    U8 end_q_id = 0;
    U8 link_id = 0; 
    U8 q_id = 0;
    
    for (link_id = 0; link_id < start_link; link_id++)
        {
        start_q_id += cfg->link_q_num[link_id];
        }
    
    for (link_id = start_link; link_id <= end_link; link_id++)
        {
        end_q_id = start_q_id + cfg->link_q_num[link_id];;
        
        printf("Link %d Queue Configuration:\n", link_id);
        for (q_id = start_q_id; q_id < end_q_id; q_id++)
            {
            printf("L1 :%u Level:%u Weight:%u l1 Size:%u \n",
                q_id, cfg->q_cfg[q_id].level, cfg->q_cfg[q_id].weight, 
                cfg->q_cfg[q_id].l1_size);
            }
        
        start_q_id = end_q_id;
        }  
    }

static int DumpEponMacCfg(int argc, char **argv)
    {
    U8 dumpid = 0;
    int ret = 0;
    epon_mac_q_cfg_t cfg;
    argv += FIRST_ARG_OFFSET;

    if (*argv)
        {
        dumpid = strtoul(*argv, NULL, 0);
        }  

    memset(&cfg, 0, sizeof(cfg));
    ret = eponStack_CtlCfgMacQueue(EponGetOpe, &cfg);
    
    if (!ret)
        {
        if (dumpid <= 7)
            {
            if (dumpid >= cfg.link_num)
                {
                goto exit;
                }
            dump_links_queue(dumpid, dumpid, &cfg);
            }
        else if (dumpid == 8)
            {
            dump_links_queue(0, (cfg.link_num-1), &cfg);
            }
        else if (dumpid == 9)
            {
            printf("Link number: %u \n", cfg.link_num);
            }
        else if (dumpid == 10)
            {
            printf("report mode: 0x%02x \n", cfg.rpt_mode);
            }
        else
            {
            printf("Link number: %u \n", cfg.link_num);
            printf("report mode: 0x%02x \n", cfg.rpt_mode);
            dump_links_queue(0, (cfg.link_num-1), &cfg);
            }
        }
    else
        {
        printf("get epon mac cfg fail n");
        }
exit:    
    return ret;
    }


static int Gather(int argc, char **argv)
    {
    U8 flag = 0;
    int ret = 0;

    argv += FIRST_ARG_OFFSET;

    if (*argv) 
        {
        flag = strtoul(*argv, NULL, 0);
        }  

    ret = eponStack_StatsGather((BOOL)flag);


    return ret;
    }


static int CfgDebug(int argc, char **argv)
    {
    EponCtlOpe ope = EponGetOpe;
    DebugPara value;
    int ret = 0;

    argv += FIRST_ARG_OFFSET;
    if (*argv) 
        {
        ope = strtoul(*argv, NULL, 0);
        }

    argv++;
    if (*argv) 
        {
        value.id = strtoul(*argv, NULL, 0);
        }

    argv++;

    if (*argv) 
        {
        value.flag = strtoul(*argv, NULL, 0);
        }

    ret = eponStack_CtlCfgDebug(ope,&value);

    return ret;
    }


static int DumpAlarmInfo(int argc, char **argv)
    {
    EponCtlOpe ope = EponSetOpe;
    DebugPara value;
    int ret = 0;

    value.id = 0x14;

    ret = eponStack_CtlCfgDebug(ope,&value);

    return ret;
    }

static int DumpMpcpInfo(int argc, char **argv)
    {
    EponCtlOpe ope = EponSetOpe;
    DebugPara value;
    int ret = 0;

    value.id = 0x13;

    ret = eponStack_CtlCfgDebug(ope,&value);

    return ret;
    }


static int CfgShaperAdd(int argc, char **argv)
    {
    int ret = 0;
    U32 l1 = 0;
    U32 rate = 0;
    U16 size = 0;
    U8 shp = 0;

    argv += FIRST_ARG_OFFSET;
    if (*argv)
    {
    l1 = strtoul(*argv, NULL, 0);
    }

    argv++;
    if (*argv) {
    rate = strtoul(*argv, NULL, 0);
    }
    argv++;
    if (*argv) {
    size = strtoul(*argv, NULL, 0);
    }
    if (!eponStack_CtlCfgShaper(EponSetOpe,&l1,&rate,&size,&shp))
    {
    printf("Profile Number:%d\n",shp);
    }
    return ret;
    }

static int CfgShaperDel(int argc, char **argv)
    {
    int ret = 0;
    U8 shp = 0;

    argv += FIRST_ARG_OFFSET;
    if (*argv) {
    shp = strtoul(*argv, NULL, 0);
    }


    eponStack_CtlDelShaper(shp);
    return ret;
    }

static int CfgShaperDump(int argc, char **argv)
    {
    int ret = 0;
    U32 l1 = 0;
    U32 rate = 0;
    U16 size = 0;
    U8 shp = 0;

    argv += FIRST_ARG_OFFSET;
    if (*argv) {
    shp = strtoul(*argv, NULL, 0);
    }

    if (shp < 8)
        {
        if (!eponStack_CtlCfgShaper(EponGetOpe,&l1,&rate,&size,&shp))
            {
            printf("Profile:%d l1map:%0x rate:%d size;%d\n",
            shp,l1,rate,size);
            }
        }
    else
        {
        for (shp = 0;shp < 8;shp++)
            {
            if (!eponStack_CtlCfgShaper(EponGetOpe,&l1,&rate,&size,&shp))
                {
                printf("Profile Number:%d l1map:%0x rate:%d size;%d\n",
                shp,l1,rate,size);
                }
            }
        }
    return ret;
    }


static int Cfgsilence(int argc, char **argv)
    {
    int ret = 0;
    BOOL flag = FALSE;
    U16 time = 0;

    argv += FIRST_ARG_OFFSET;
    if (*argv) {
    flag = strtoul(*argv, NULL, 0);
    }

    argv++;
    if (*argv) {
    time = strtoul(*argv, NULL, 0);
    }
    eponStack_CtlSetSilence(flag,time);
    return ret;
    }

static int CfgByteLimit(int argc, char **argv)
    {
    int ret = 0;
    U8 queue = 0;
    U8 limit = 0;
    EponCtlOpe ope = EponSetOpe;
    argv += FIRST_ARG_OFFSET;

    if (*argv) {
    ope = strtoul(*argv, NULL, 0);
    }
    argv++;
    if (*argv) {
    queue = strtoul(*argv, NULL, 0);
    }
    argv++;
    if (*argv) {
    limit = strtoul(*argv, NULL, 0);
    }
    if(eponStack_CtlCfgByteLimit(ope,&queue,&limit))
    	{
        printf("cfg fail \n");
        }
    else
    	{
        if (ope == EponSetOpe)
           {
           eponStack_CtlCfgByteLimit(EponGetOpe,&queue,&limit);
           }
        printf("queue:%d limit:%d\n",queue,limit);
        }
    
    return ret;
    }


static int CfgTxpower(int argc, char **argv)
    {
    int ret = 0;
    BOOL actOpt = FALSE;
    U16 enableTime=0;
    argv += FIRST_ARG_OFFSET;

    if (*argv) {
    actOpt = strtoul(*argv, NULL, 0);
    }
    argv++;
    if (*argv) {
    enableTime = strtoul(*argv, NULL, 0);
    }
    eponStack_CtlSetTxPower(actOpt,enableTime);
    return ret;
    }


static int CfgGetLLID(int argc, char **argv)
    {
    int ret = 0;
    U8 link = 0;
    U16 llid;

    argv += FIRST_ARG_OFFSET;
    if (*argv) {
    link = strtoul(*argv, NULL, 0);
    }
    eponStack_CtlGetLLID(link,&llid);
    printf("link:%d llid:%0x\n",link,llid);
    return ret;
    }

static int CfgGetLinkIndex(int argc, char **argv)
    {
    int ret = 0;
    U8 link = 0;
    U16 llid = 0;

    argv += FIRST_ARG_OFFSET;
    if (*argv) {
    llid = strtoul(*argv, NULL, 0);
    }
    ret = eponStack_CtlGetLinkIndex(llid,&link);
    printf("ret: %d llid:0x%x link:%d\n",ret, llid, link);
    return ret;
    }

static int CfgEncMode(int argc, char **argv)
    {
    /* eponctl enc [link] [mode: aes/zoh/ctc/8021ae/dis] [direction: dn/bi] [opts1: im/ex] [opts2: authonly/authenc] */
    U8 link = 0, mode = EncryptModeDisable;
    U16 opts = EncryptOptNone; 

    argv += FIRST_ARG_OFFSET;
    if (*argv) 
        {
        link = (U8)strtoul(*argv, NULL, 0);
        }
    argv++;
    if (*argv) 
	{
	if (!strcmp(*argv, "aes")) 
	    {
	    mode = EncryptModeAes;
	    }
	else if (!strcmp(*argv, "zoh")) 
	    {
	    mode = EncryptModeZoh;
	    }
	else if (!strcmp(*argv, "ctc")) 
	    {
	    mode = EncryptModeTripleChurn;
	    }
	else if (!strcmp(*argv, "8021ae")) 
	    {
	    mode = EncryptMode8021AE;
	    }
	else if (!strcmp(*argv, "dis")) 
	    {
	    mode = EncryptModeDisable;
	    }
	else
	    {
	    goto exit;
	    }
        }
    argv++;
    if (*argv) 
	{
	if (!strcmp(*argv, "bi")) 
            {
            opts |= EncryptOptBiDir;
            }
    	}
    printf("Configure Link %d, encMode %d, dir: %s", link, mode,  
		(opts&EncryptOptBiDir)?"Bi":"Dn");

    if (mode == EncryptMode8021AE)
    	{// more options for 802.1AE encryption
        argv++;
        if (*argv) 
            {
            if (!strcmp(*argv, "im")) 
                {
                opts |= EncryptOptImplicitSci;
                }
    	    }
        argv++;
        if (*argv) 
            {
            if (!strcmp(*argv, "authonly")) 
                {
                opts |= EncryptOptAuthOnly;
                }
    	    }
        printf(", %s, %s", (opts&EncryptOptImplicitSci)?"Implicit":"Explicit", 
			(opts&EncryptOptAuthOnly)?"AuthOnly":"AuthEnc");
    	}
    printf("\n");
    eponStack_CtlSetKeyMode(link, mode, opts);
    return 0;
	
exit:
    printf("Wrong parameters\n");
    return -1;
    }


#define CTC_1G_KEY_STR_LEN	6
#define CTC_10G_KEY_STR_LEN	18
#define AES_ONE_KEY_STR_LEN	8
#define AES_KEY_STR_LEN	32
#define SCI_KEY_STR_LEN	16
static int CfgEncKey(int argc, char **argv)
    {
    /* eponctl enc [link] [index: 0/1] [mode: ctc/aes/zoh/ae] [dir: dn/up][key data] [sci data]*/
    U8 link = 0, index = 0, mode = 1, keyLen;
    Direction dir = Dnstream;

    argv += FIRST_ARG_OFFSET;
    if (*argv) 
        {
        link = (U8)strtoul(*argv, NULL, 0);
        }
    argv++;
    if (*argv) 
        {
        index = (U8)strtoul(*argv, NULL, 0);
        }
    argv++;
    if (*argv) 
        {
        if (!strcmp(*argv, "ctc")) 
            {
            mode = 1;
            }
        else
            {
            mode = 0;
            }
        }
    argv++;
    if (*argv) 
        {
        if (!strcmp(*argv, "dn")) 
            {
            dir = Dnstream;
            }
        else
            {
            dir = Upstream;
            }
        }

    //key data
    argv++;
    if (*argv)
        {
        U8 i, strLen;
        U32 key[4], sci[2];
        char strBuf[20];
        char *strPtr;
        memset(key, 0, sizeof(key));
        memset(sci, 0, sizeof(sci));

        strPtr = *argv;
        strLen = strlen(strPtr);
        printf("Configure Link %d, index %d, dir: %s\n", link, index, 
			(dir == Dnstream)?"Dn":"Up");
        if (mode == 1)
            {//ctc mode
            if ((strLen >= CTC_1G_KEY_STR_LEN) && (strLen < CTC_10G_KEY_STR_LEN))
                {
                strncpy(strBuf, strPtr, CTC_1G_KEY_STR_LEN);
                strBuf[CTC_1G_KEY_STR_LEN] = '\0';
                key[0] = strtoul(strBuf, NULL, 16);
                keyLen = 4;
                printf("Key: %06x , keyLen: %d\n", key[0], keyLen);
                }
            else if (strLen >= CTC_10G_KEY_STR_LEN)
                {
                printf("Key: ");
                for (i = 0; i < 3; i ++)
                    {
                    strncpy(strBuf, &(strPtr[i*CTC_1G_KEY_STR_LEN]), CTC_1G_KEY_STR_LEN);
                    strBuf[CTC_1G_KEY_STR_LEN] = '\0';
                    key[i] = strtoul(strBuf, NULL, 16);
                    printf(" %06x", key[i]);
                    }
                keyLen = 12;
                printf(", keyLen: %d\n", keyLen);
                }
            else
                goto exit;
            }
        else 
            {
            if (strLen >= AES_KEY_STR_LEN)
                {
                printf("Key: ");
                for (i = 0; i < 4; i++)
                    {
                    strncpy(strBuf, &(strPtr[i*AES_ONE_KEY_STR_LEN]), AES_ONE_KEY_STR_LEN);
                    strBuf[AES_ONE_KEY_STR_LEN] = '\0';
                    key[i] = strtoul(strBuf, NULL, 16);
                    printf(" %08x", key[i]);
                    }
                keyLen = 16;
                printf(", keyLen: %d", keyLen);
                
                argv++;
                if (*argv)
                    {
                    strPtr = *argv;
                    if(strlen(strPtr) >= SCI_KEY_STR_LEN)
                        {
                        printf(", Sci: ");
                        for (i = 0; i < 2; i++)
                            {
                            strncpy(strBuf, &(strPtr[i*AES_ONE_KEY_STR_LEN]), AES_ONE_KEY_STR_LEN);
                            strBuf[AES_ONE_KEY_STR_LEN] = '\0';
                            sci[i] = strtoul(strBuf, NULL, 16);
                            printf(" %08x", sci[i]);
                            }
                        }
                    }
                printf("\n");
                }
            else
                goto exit;
            }
        eponStack_CtlSetKeyData(link, dir, index, key, keyLen, sci);
        return 0;
        }
	
exit:
    return -1;
    }


static int CfgSetFec(int argc, char **argv)
    {
    int ret = 0;
    FecPara val;
    EponCtlOpe ope = EponSetOpe;

    memset(&val,0,sizeof(val));
    argv += FIRST_ARG_OFFSET;
    if (*argv) {
    ope = strtoul(*argv, NULL, 0);
    }
    argv++;
    if (*argv) {
    val.link = strtoul(*argv, NULL, 0);
    }
    argv++;
    if (*argv) {
    val.tx = strtoul(*argv, NULL, 0);
    }
    argv++;
    if (*argv) {
    val.rx = strtoul(*argv, NULL, 0);
    }	
    eponStack_CtlCfgFec(ope,&val);
    printf("Link:%d TxFec:%s RxFec:%s\n",
    val.link,(val.tx ? "True":"False"),
    (val.rx ? "True":"False"));
    return ret;
    }
	
static int CfgGetMcast(int argc, char **argv)
    {
    int ret = 0;
    U8 link = 0;
    U16 phyLlid, mcastIdx;

    argv += FIRST_ARG_OFFSET;
    if (*argv) {
    link = strtoul(*argv, NULL, 0);
    }
    ret = eponStack_CtlGetMcast(link, &phyLlid, &mcastIdx, ActGetMcastLlidAndIdxByUcastIdx);
    if (ret != 0 || phyLlid == 0)
        printf("Invalid link:%d\n", link);
    else
        printf("Link %d: Mcast LLID 0x%04x, Idx %d\n", link, phyLlid, mcastIdx);
    return ret;
    }

#ifdef CLOCK_TRANSPORT
static int CfgClkTransport(int argc, char **argv)
    {
    int ret = 0;
    ClkTransPara para;

    memset(&para, 0x0, sizeof(ClkTransPara));
    para.flag = ClkTransOptionsShow;

    argv += FIRST_ARG_OFFSET;
    if (*argv)
    {
        if (strcmp(*argv, "show") == 0)
            para.flag = ClkTransOptionsShow;
        if (strcmp(*argv, "todout") == 0)
            para.flag = ClkTransOptionsTodFmt;
        if (strcmp(*argv, "todCmccAscii") == 0)
            para.flag = ClkTransOptionsTodCmccAscii;
        if (strcmp(*argv, "timeadjust") == 0)
            para.flag = ClkTransOptionsTimeAdj;
        if (strcmp(*argv, "cmccpara") == 0)
            para.flag = ClkTransOptionsCmccPara;
        if (strcmp(*argv, "halfPeriod") == 0)
            para.flag = ClkTransOptionsHalfperiod;
        if (strcmp(*argv, "ignoreMsgCnt") == 0)
            para.flag = ClkTransOptionsIgnoreMsg;
    }

    argv++;
    if (*argv) 
        para.data  = strtoul(*argv, NULL, 0);

    ret = eponStack_CtlSetClkTrans(&para);

    return ret;
    }
#endif

static int CfgSetEponRogueOnuDet(int argc, char **argv)
    {
    int ret = 0;
    Bool enable = FALSE;
    U32 threshold = 0;
    U8 times = 0;
    EponCtlOpe ope = EponSetOpe;

    argv += FIRST_ARG_OFFSET;
    if (*argv) {
        ope = strtoul(*argv, NULL, 0);
        }
    argv++;
    if (*argv) {
        enable = strtoul(*argv, NULL, 0);
        }
    argv++;
    if (*argv) {
        threshold = strtoul(*argv, NULL, 0);
        }
    argv++;
    if (*argv) {
        times = strtoul(*argv, NULL, 0);
        }
    
    eponStack_CtlCfgRogueOnuDet(ope, &enable, &threshold, &times);
        
    printf("Epon Rogue Onu Detection, enable:%u threshold:%08x times:%u \n",
        (U8)enable, threshold, times);
       
    return ret;
    }

static int CfgFailSafe(int argc, char **argv)
    {
    int ret = 0;
    Bool enable = FALSE;
    EponCtlOpe ope = EponSetOpe;

    argv += FIRST_ARG_OFFSET;
    if (*argv) {
        ope = strtoul(*argv, NULL, 0);
        }
    argv++;
    if (*argv) {
        enable = strtoul(*argv, NULL, 0);
        }
    
    eponStack_CtlCfgFailSafe(ope, &enable);
        
    printf("Epon Fail Safe Configuration, enable:%u \n", (U8)enable);
       
    return ret;
    }

#ifdef CONFIG_EPON_10G_SUPPORT
static int Cfg10gFecAutoDet(int argc, char **argv)
    {
    int ret = 0;
    Bool enable = FALSE;
    EponCtlOpe ope = EponSetOpe;

    argv += FIRST_ARG_OFFSET;
    if (*argv) {
        ope = strtoul(*argv, NULL, 0);
        }
    argv++;
    if (*argv) {
        enable = strtoul(*argv, NULL, 0);
        }
    
    eponStack_CtlCfg10gFecAutoDet(ope, &enable);
        
    printf("Epon 10G FEC auto_detection, enable:%u \n", (U8)enable);
       
    return ret;
    }

static int Cfg10gFecSwitchOnce(int argc, char **argv)
    {
    int ret = 0;

    ret = eponStack_CtlCfg10gFecSwitchOnce();

    return ret;
    }

#endif

static int ApiTesting(int argc, char **argv)
    {
    int ret = 0;
    argv += FIRST_ARG_OFFSET;
    printf("api:%s testing\n",*argv);
    sleep(1);
    
    if (strcmp(*argv,"eponStack_CtlCfgMacQueue") == 0)
        {
        epon_mac_q_cfg_t cfg;
        memset(&cfg, 0, sizeof(cfg));
        eponStack_CtlCfgMacQueue(EponGetOpe, &cfg);
        UDumpData((U8 *)&cfg, sizeof(cfg));
        }
    if (strcmp(*argv,"eponStack_CtlCfgCtcAlarmThreshold") == 0)
        {
        U8 count = 3;
        U16 alarmid[3];
        U32 setthreshold[3];
        U32 clearthreshold[3];
        memset(alarmid,0,sizeof(alarmid));
        memset(setthreshold,0,sizeof(setthreshold));
        memset(clearthreshold,0,sizeof(clearthreshold));
        for (count=0;count<3;count++)
        {
        alarmid[count] = (0x0115+count);
        }
        eponStack_CtlCfgCtcAlarmThreshold(EponGetOpe,alarmid,count,setthreshold,clearthreshold);
        sleep(1);
        UDumpData((U8 *)alarmid,sizeof(alarmid));
        UDumpData((U8 *)setthreshold,sizeof(setthreshold));
        UDumpData((U8 *)clearthreshold,sizeof(clearthreshold));

        for (count=0;count<3;count++)
        {
        setthreshold[count] = (0x3e8+count);
        clearthreshold[count] = (2+count);
        }
        sleep(1);
        eponStack_CtlCfgCtcAlarmThreshold(EponSetOpe,alarmid,count,setthreshold,clearthreshold);
        memset(setthreshold,0,sizeof(setthreshold));
        memset(clearthreshold,0,sizeof(clearthreshold));

        eponStack_CtlCfgCtcAlarmThreshold(EponGetOpe,alarmid,count,setthreshold,clearthreshold);
        sleep(1);
        UDumpData((U8 *)alarmid,sizeof(alarmid));
        UDumpData((U8 *)setthreshold,sizeof(setthreshold));
        UDumpData((U8 *)clearthreshold,sizeof(clearthreshold));
        }
    if (strcmp(*argv,"eponStack_CtlCfgCtcAlarmState") == 0)
        {
        U8 count = 3;
        U16 alarmid[3];
        BOOL setthreshold[3];

        memset(alarmid,0,sizeof(alarmid));
        memset(setthreshold,0,sizeof(setthreshold));

        for (count=0;count<3;count++)
        {
        alarmid[count] = (0x0115+count);
        }
        eponStack_CtlCfgCtcAlarmState(EponGetOpe,alarmid,count,setthreshold);
        sleep(1);
        UDumpData((U8 *)alarmid,sizeof(alarmid));
        UDumpData((U8 *)setthreshold,sizeof(setthreshold));


        for (count=0;count<3;count++)
        {
        setthreshold[count] = TRUE;
        }
        sleep(1);
        eponStack_CtlCfgCtcAlarmState(EponSetOpe,alarmid,count,setthreshold);
        memset(setthreshold,0,sizeof(setthreshold));


        eponStack_CtlCfgCtcAlarmState(EponGetOpe,alarmid,count,setthreshold);
        sleep(1);
        UDumpData((U8 *)alarmid,sizeof(alarmid));
        UDumpData((U8 *)setthreshold,sizeof(setthreshold));

        }
    if (strcmp(*argv,"eponStack_CtlCfgCtcAlarmState") == 0)
        {
        U8 count = 3;
        U16 alarmid[3];
        BOOL setthreshold[3];

        memset(alarmid,0,sizeof(alarmid));
        memset(setthreshold,0,sizeof(setthreshold));

        for (count=0;count<3;count++)
        {
        alarmid[count] = (0x0115+count);
        }
        eponStack_CtlCfgCtcAlarmState(EponGetOpe,alarmid,count,setthreshold);
        sleep(1);
        UDumpData((U8 *)alarmid,sizeof(alarmid));
        UDumpData((U8 *)setthreshold,sizeof(setthreshold));


        for (count=0;count<3;count++)
        {
        setthreshold[count] = TRUE;
        }
        sleep(1);
        eponStack_CtlCfgCtcAlarmState(EponSetOpe,alarmid,count,setthreshold);
        memset(setthreshold,0,sizeof(setthreshold));


        eponStack_CtlCfgCtcAlarmState(EponGetOpe,alarmid,count,setthreshold);
        sleep(1);
        UDumpData((U8 *)alarmid,sizeof(alarmid));
        UDumpData((U8 *)setthreshold,sizeof(setthreshold));

        }
     if (strcmp(*argv,"eponStack_CtlLinkStatsGet") == 0)
        {
        U8 count = 0;
        StatsCntOne linkstat[MaxStatsCount];
        U8 id;


        memset(linkstat,0,sizeof(linkstat));
        for (id = 0;id < 60;id++)
            {
            linkstat[id].statsId = id;
            count++;
            }
        eponStack_CtlLinkStatsGet(0, linkstat,count);
        sleep(1);
        UDumpData((U8 *)linkstat,sizeof(linkstat));
        }
     
     if (strcmp(*argv,"eponStack_CtlPonStatsGet") == 0)
        {
        StatsCntOne ponstat[MaxStatsCount];
        U8 id;

        memset(ponstat,0,sizeof(ponstat));
        
        for (id=0; id<MaxStatsCount; id++)
            {
            ponstat[id].statsId = id;
            }

        eponStack_CtlPonStatsGet(ponstat, MaxStatsCount-1);
        UDumpData((U8 *)ponstat,sizeof(ponstat));
        }
    return ret;
    }

static int CfgMaxFrameSize(int argc, char **argv)
{
    int ret = 0;
    U16 maxFrameSize= 0;

    argv += FIRST_ARG_OFFSET;
    if (*argv) {
        maxFrameSize = strtoul(*argv, NULL, 0);
        eponStack_CtlSetMaxFrameSize(maxFrameSize);
        printf("new epon mac mtu %d\n", maxFrameSize);
    }

    return ret; 
}

static int EpnInfo(int argc, char **argv)
{
    U32 epnSetVal = 0;
    
    argv += FIRST_ARG_OFFSET;
    if (*argv)
    {
        if (strcmp(*argv, "selcnt") == 0)
        {
            argv ++;
            if (*argv)
            {   
                epnSetVal = strtoul(*argv, NULL, 0);
                eponStack_CtlDumpEpnInfo(EponSetOpe,EpnInfoL1Cnt,epnSetVal);

                return 0;
            }    
            else
            {
               printf("No L1 index select\n");
               return -1;
            }
        }
        else if (strcmp(*argv, "eninrp") == 0)
        {
            argv ++;
            if (*argv)
            {   
                epnSetVal = strtoul(*argv, NULL, 0);
                if(epnSetVal < 0 || epnSetVal > 1)
                {
                    printf("Invalid input value:%s\n", *argv);
                    return -1;
                }
                eponStack_CtlDumpEpnInfo(EponSetOpe,EpnInfoInterrupt,epnSetVal);

                return 0;
            }    
            else
            {
               printf("No L1 index select\n");
               return -1;
            }
        }
        else if (strcmp(*argv, "show") == 0)
        {
            argv ++;
            if (strcmp(*argv, "cnt") == 0)
            {   
                eponStack_CtlDumpEpnInfo(EponGetOpe,EpnInfoL1Cnt,epnSetVal);
                return 0;
            }
            else if(strcmp(*argv, "inrp") == 0)
            {
                eponStack_CtlDumpEpnInfo(EponGetOpe,EpnInfoInterrupt,epnSetVal);
                return 0;
            }
            else
            {
               printf("Invalid input value:%s\n", *argv);
               return -1;
            }
        }
        
        else
        {
             printf("Invalid input value:%s\n", *argv);
             return -1; 
        }
    }
    else    
        return -1; 
}

static int SendCmdsToOamStack(OamCmdsMsg *msg)
{
    SINT32 rc = 0;
    int ret = 0;
    SINT32 eponCtlFd = -1;

    eponCtlFd = InitConnWithEponApp();
    
    if (eponCtlFd > 0)
    {
        rc = write(eponCtlFd, msg, sizeof(OamCmdsMsg));
        
        if (rc < 0)
        {
            if (errno == EPIPE)
                printf("got EPIPE, epon app is dead");
            else
                printf("write failed, errno=%d", errno);

            ret = -1;
        }
        else if (rc != sizeof(OamCmdsMsg))
        {
            printf("unexpected rc %d, expected %u", rc, sizeof(OamCmdsMsg));
            ret = -1;
        }
       
        close(eponCtlFd);
    }
    else
    {
        ret = -1;
    }
   
    return ret;
}

static int DumpOamInfo(int argc, char **argv)
{
    OamCmdsMsg msg = {0};

    msg.cmdType = EponCmdDumpOamState;

    argv += FIRST_ARG_OFFSET;
    if (*argv) 
    {
        msg.paraData[0] = strtoul(*argv, NULL, 0);
        msg.paraLength = 1;
    }
    
    return SendCmdsToOamStack(&msg);
}

static int DumpOamAlarm(int argc, char **argv)
{
    OamCmdsMsg msg = {0};

    msg.cmdType = EponCmdDumpOamAlm;

    argv += FIRST_ARG_OFFSET;
    if (*argv) 
    {
        msg.paraData[0] = strtoul(*argv, NULL, 0);
        msg.paraLength = 0;
    }
    
    return SendCmdsToOamStack(&msg);
}

static int DumpOamVars(int argc, char **argv)
{
    OamCmdsMsg msg = {0};

    msg.cmdType = EponCmdDumpOamVars;

    argv += FIRST_ARG_OFFSET;
    if (*argv) 
    {
        if (strcmp(*argv, "queueshaper") == 0)
        {
            msg.paraData[0] = OAM_VARS_QUEUE_RL;
            msg.paraLength = 1;
        }
    }
    
    return SendCmdsToOamStack(&msg);
}

static int DumpOamMsg(int argc, char **argv)
{

    OamCmdsMsg msg = {0};
    msg.cmdType = EponCmdDumpOamMsg;

    argv += FIRST_ARG_OFFSET;
    if (*argv)
    {
        if ((strcmp(*argv, "show") == 0)  && (argc == 3))
        {
            msg.paraData[0] = OAM_DUMP_CMD_SHOW;
            msg.paraLength = 1;
            SendCmdsToOamStack(&msg);
            return 0;
        }
        else if ((strcmp(*argv, "dump") == 0) && (argc == 5))
        {
            msg.paraData[0] = OAM_DUMP_CMD_DUMP;
            argv ++;
            msg.paraData[1] = strtoul(*argv, NULL, 0);
            argv ++;
            if (strcmp(*argv, "off") == 0)
            {
                 msg.paraData[2] = OAM_DUMP_OFF;
            }
            else if (strcmp(*argv, "alarm") == 0)
            {
                 msg.paraData[2] = OAM_DUMP_ALARM;
            }
            else if (strcmp(*argv, "normal") == 0)
            {
                 msg.paraData[2] = OAM_DUMP_NORMAL;
            }
            else if (strcmp(*argv, "all") == 0)
            {
                 msg.paraData[2] = OAM_DUMP_ALL;
            }
            else
            {
                 printf("Invalid input value:%s\n", *argv);
                 return -1;
            }
            msg.paraLength = 3;
            SendCmdsToOamStack(&msg);
            return 0;
        }
        else if ((strcmp(*argv, "setdst") == 0) && (argc >= 4) && (argc <= 5))
        {
            argv ++;
            if((strcmp(*argv, "console") == 0) && (argc == 4))
            {
                 msg.paraData[0] = OAM_DUMP_CMD_CONSOLE;
                 msg.paraLength = 1;
                 SendCmdsToOamStack(&msg);
                 return 0;
            }
            else if(strcmp(*argv, "file") == 0)
            {
                 msg.paraData[0] = OAM_DUMP_CMD_FILE;
                 msg.paraLength = 1;
                 if(argc == 5) 
                 {
                     argv++;
                     if(strcmp (*argv, "overwrite") == 0)
                     {
                         msg.paraData[0] = OAM_DUMP_CMD_FILE_OVERWRITE;
                     }
                     else
                     {
                         printf("Invalid input arg:%s\n", *argv);
                         return -1;
                     }
                 }
                 SendCmdsToOamStack(&msg);
                 return 0;
            }
            printf("Invalid input arg:%s\n", *argv);
            return -1;
        }
    }
    printf("Invalid input arg!\n");
    return -1;
}

static const struct command commands[] = {
    /* This part commands are for kernel epon stak */
    { 2, "setcfg", SetCfg,
      ": setcfgToEponModule\n"
      "  eponctl SetCfg [Field Name][Value]\n"
      "  eponctl SetCfg help 0 \n"
    },
    { 0, "dumpcfg",DumpCfg,
      ": Dump the epon cfg data \n"
      "  eponctl dumpcfg\n"
    },
    { 2, "setreg", SetRegister,
      ": Set the register \n"
      "  eponctl SetReg [reg][value]\n"
    },
    { 2, "dumpreg",DumpRegister,
      ": Dump the register \n"
      "  eponctl DumpReg [reg][length]\n"
    },
    { 0, "alarminfo",DumpAlarmInfo,
      ": Dump the alarm information \n"
      "  eponctl alarminfo \n"
    },
    { 0, "mpcpinfo",DumpMpcpInfo,
      ": Dump the mpcp information \n"
      "  eponctl mpcpinfo \n"
    },
    { 1, "dumpstats",DumpStats,
      ": dump the stats \n"
#if defined(CONFIG_BCM96858) || defined(CONFIG_BCM96856)
      "  eponctl DumpStats [id 0:lif,1:epon,2-33:link,34:xif,35:xpcs32,36:xpcs40]\n"
#else
      "  eponctl DumpStats [id 0:lif,1:epon,2-9:uni-link,18-25:mul-link]\n"
#endif
    },
    { 1, "dumpeponmac",DumpEponMacCfg,
      ": dump epon mac queue, report, link num configuration\n"
      "  eponctl dumpeponmac [id (0-7):link q,8:all links q,9:total link num,10:report mode,11:all]\n"
    },
    { 1, "gather",Gather,
      ": Enalbe or Disable stats gather \n"
      "  eponctl Gather [0:disable,1:enable]\n"
    },
    { 3, "debug", CfgDebug,
      ": Configure debug \n"
      "  eponctl Debug [set(1)/get(2)][Module Id][Enable(1)/Disable(0)] \n" 	
      "Ioctl=1,Mpcp,Nco,Epon,Stats,Alarm,HoldOver,ProtectSwitch,OptCtrl,OntDir,EponUser,EponRogue\n"
    },
    { 3, "shaperadd", CfgShaperAdd,
      ": Add shaperadd profile\n"
      "  eponctl shaperadd [L1bitmap][rate] [size]\n" 	
    },
    { 1, "shaperdel", CfgShaperDel,
      ": Delete shaperadd profile\n"
      "  eponctl shaperdel [number]]\n" 	
    },
    { 1, "shaperdump", CfgShaperDump,
      ": Dump shaperadd profile\n"
      "  eponctl shaperdump [number]]\n" 	
    },
    { 2, "silence", Cfgsilence,
      ": Configure silence \n"
      "  eponctl silence [delloc flag][time]\n" 	
    },
    { 2, "txpower", CfgTxpower,
      ": Configure Txpower \n"
      "  eponctl txpower [time]\n" 	
    },
    { 1, "llid", CfgGetLLID,
      ": Get physic LLID according to the actual link\n"
      "  eponctl llid [link]\n" 	
    },
    { 1, "getLinkIndex", CfgGetLinkIndex,
      ": Get link index according to physical llid value\n"
      "  eponctl getLinkIndex [llid]\n" 	
    },
    { 4, "fec", CfgSetFec,
      ": Configure FEC \n"
      "  eponctl fec [set(1)/get(2)] [link] [Tx 0:disable,1:enable] [Rx 0:disable,1:enable]\n" 	
    },
    { 2, "encmode", CfgEncMode,
      ": Configure encryption mode \n"
      "  eponctl enc [link] [mode: aes/zoh/ctc/8021ae/dis] [direction: dn/bi] [opts1: im/ex] [opts2: authonly/authen]\n" 	
    },
    { 5, "enckey", CfgEncKey,
      ": Configure encryption key\n"
      "  eponctl enc [link] [index: 0/1]  [mode: ctc/other] [dir: dn/up][key data] [sci data]\n" 	
    },
    { 3, "bytelimit", CfgByteLimit,
      ": Configure L1 Byte Limit \n"
      "  eponctl bytelimit [set(1)/get(2)] [queue] [limit]\n" 	
    },
    { 4, "rogueonu", CfgSetEponRogueOnuDet,
      ": Configure Rogue Onu Detection \n"
      "  eponctl rogueonu [set(1)/get(2)] [enable(1)/disable(0)] [threshold] [repeat times]\n" 
    },    
    { 1, "mcaststatus", CfgGetMcast,
      ": Get MPCP Multi LLID Status\n"
      "  eponctl mcaststatus [link]\n" 	
    },
#ifdef CLOCK_TRANSPORT
    { 2, "8021as", CfgClkTransport,
        ": Configure  Clock Transport\n"
        "  eponctl 8021as show <0|1> 0-none,1-clear stats \n"	
        "  eponctl 8021as todout <0|1|2|3> 0-hex,1-ascii,2-China standard, 3-disable\n"
        "  eponctl 8021as todCmccAscii <0|1> 0-disable,1-enable \n"
        "  eponctl 8021as timeadjust time (unit: TQ) \n"
        "  eponctl 8021as cmccpara para (leaps|secondPulse|Tacc) \n"
        "  eponctl 8021as halfPeriod time (uni:TQ)\n"
        "  eponctl 8021as ignoreMsgCnt number\n"
     },
#endif    
    {2, "failsafe", CfgFailSafe,
        ": Configure Faile Safe \n"
        "  eponctl failesafe [set(1)/get(2)] [enable(1)/disable(0)]\n"
    },
    { 1, "apitest",ApiTesting,
      ": Test Api \n"
      "  eponctl apitest [apiname]\n"
    },
    { 1, "maxframesize",CfgMaxFrameSize,
      ": Configure received max frame size of epon mac\n"
      "  eponctl maxframesize <maxframesize> 1-2000\n"
    },
    { 2, "epninfo",EpnInfo,
      ": Dump Epon Mac L1 Virtual Accumulator count(SVA,UVA) or Epon Mac main interrupt\n"
      "  eponctl epninfo selcnt [L1 index]:Selects which L1 Virtual Accumulator to report,default 0xffffffff\n"
      "  eponctl epninfo show cnt:show Virtual Accumulator count(SVA,UVA)\n"
      "  eponctl epninfo show inrp:show Epn main interrupt and xif/xpcsTx interrupt\n"
    },
#ifdef CONFIG_EPON_10G_SUPPORT
    {2, "10gfecautodet", Cfg10gFecAutoDet,
        ": enable/disable 10G FEC auto_detection \n"
        "  eponctl 10gfecautodet [set(1)/get(2)] [enable(1)/disable(0)]\n"
    },
    { 0, "10gfecswonce",Cfg10gFecSwitchOnce,
      ": force switch 10G fec mode once \n"
      "  eponctl 10gfecswonce\n"
    },
#endif

    /* This part commands for userspace oam stack */
    {1, "dumpoamstate", DumpOamInfo,
        ": Dump oam data structure of the link \n"
        "  eponctl dumpoamstate [link]\n"
    },
    {0, "dumpoamalm", DumpOamAlarm,
        ": Dump oam alarm state \n"
        "  eponctl dumpoamalm \n"
    },
    {1, "oammsg", DumpOamMsg,
        ": OAM message dump config\n"
        "  eponctl oammsg show\n"
        "              -- Show current configuration\n"
        "  eponctl oammsg dump <link> <off|alarm|normal|all>\n"
        "              -- Configure OAM message dump level\n"
        "                     Link:     <0-31> -- link #\n"
        "                               <0xff> -- all links\n"
        "                     off:      No OAM message dump\n"
        "                     normal:   Dump all OAM messages except for info messages\n"
        "                     alarm:    Dump alarm OAM messages only\n"
        "                     all:      Dump all OAM messages\n"
        "  eponctl oammsg setdst <console|file> [overwrite]\n"
        "              -- Dump OAM message to console or file, default to console\n"
        "              -- logfile: \"/tmp/oammsg.log\", maxsize: 2M bytes \n"
    },
    {0, "dumpoamvars", DumpOamVars,
        ": Dump oam internal variables\n"
        "  eponctl dumpoamvars queueshaper\n"
    },
};

cmd_t *command_lookup(const char *cmd)
{
    int i;

    for (i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) {
        if (!strcmp(cmd, commands[i].name))
            return (cmd_t *)&commands[i];
    }

    return NULL;
}

void command_help(const cmd_t *cmd)
{
    fprintf(stderr, "  %s %s\n\n", cmd->name, cmd->help);
}

void command_helpall(void)
{
    int i;

    fprintf(stderr, "eponctl commands:\n");
    fprintf(stderr, "Usage: eponctl <command> [arguments...]\n\n");
    for (i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) 
        command_help(commands+i);
}

/*
 * Global Funtions
 */
int main(int argc, char **argv)
{
    cmd_t *cmd;
    int rc = 0;

    if (argc < 2) {
        command_helpall();
        return -1;
    }

    cmd = command_lookup(argv[1]);
    if (cmd == NULL) {
        fprintf(stderr, "invalid command [%s]\n", argv[1]);
        command_helpall();
        return -1;
    }

    if (argc < cmd->nargs + 2) {
        fprintf(stderr, "incorrect number of arguments\n");
        command_help(cmd);
        return -1;
    }
 
    rc = cmd->func(argc, argv);

    return rc;
}

