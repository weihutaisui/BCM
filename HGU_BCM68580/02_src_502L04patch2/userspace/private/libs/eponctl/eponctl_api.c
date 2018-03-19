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


#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "os_defs.h"

#include "eponctl_api.h"
#include "eponctl_types.h"

/* Defines the supported funtionality */
#define EPONCTL_LOG_COLOR_SUPPORTED
#define EPONCTL_ASSERT_SUPPORTED
#define EPONCTL_LOG_SUPPORTED
#define EPONCTL_ERROR_SUPPORTED

#define EPONCTL_DONT_CARE ~0

#if defined(EPONCTL_LOG_COLOR_SUPPORTED)
#define COLOR(clr_code)     clr_code
#else
#define COLOR(clr_code)
#endif

#ifndef MIN
#define	MIN(a, b)		(((a)<(b))?(a):(b))
#endif


/* White background */
#define CLRr                COLOR("\e[0;31m")       /* red              */
#define CLRg                COLOR("\e[0;32m")       /* green            */
#define CLRy                COLOR("\e[0;33m")       /* yellow           */
#define CLRb                COLOR("\e[0;34m")       /* blue             */
#define CLRm                COLOR("\e[0;35m")       /* magenta          */
#define CLRc                COLOR("\e[0;36m")       /* cyan             */

/* blacK "inverted" background */
#define CLRrk               COLOR("\e[0;31;40m")    /* red     on blacK */
#define CLRgk               COLOR("\e[0;32;40m")    /* green   on blacK */
#define CLRyk               COLOR("\e[0;33;40m")    /* yellow  on blacK */
#define CLRmk               COLOR("\e[0;35;40m")    /* magenta on blacK */
#define CLRck               COLOR("\e[0;36;40m")    /* cyan    on blacK */
#define CLRwk               COLOR("\e[0;37;40m")    /* white   on blacK */

/* Colored background */
#define CLRcb               COLOR("\e[0;36;44m")    /* cyan    on blue  */
#define CLRyr               COLOR("\e[0;33;41m")    /* yellow  on red   */
#define CLRym               COLOR("\e[0;33;45m")    /* yellow  on magen */

/* Generic foreground colors */
#define CLRhigh             CLRm                    /* Highlight color  */
#define CLRbold             CLRcb                   /* Bold      color  */
#define CLRbold2            CLRym                   /* Bold2     color  */
#define CLRerr              CLRwk                   /* Error     color  */
#define CLRnorm             COLOR("\e[0m")          /* Normal    color  */
#define CLRnl               CLRnorm "\n"            /* Normal + newline */

#if defined(EPONCTL_LOG_SUPPORTED)
#define EPONCTL_LOGCODE(code)    code
#else
#define EPONCTL_LOGCODE(code)
#endif /*defined(EPONCTL_LOG_SUPPORTED)*/

#if defined(EPONCTL_ERROR_SUPPORTED)
#define EPONCTL_ERRORCODE(code)    code
#else
#define EPONCTL_ERRORCODE(code)
#endif /*defined(EPONCTL_ERROR_SUPPORTED)*/

#if defined(EPONCTL_ASSERT_SUPPORTED)
#define EPONCTL_ASSERTCODE(code)    code
#else
#define EPONCTL_ASSERTCODE(code)
#endif /*defined(EPONCTL_ASSERT_SUPPORTED)*/

typedef enum {
    EPONCTL_LOG_LEVEL_ERROR=0,
    EPONCTL_LOG_LEVEL_NOTICE,
    EPONCTL_LOG_LEVEL_INFO,
    EPONCTL_LOG_LEVEL_DEBUG,
    EPONCTL_LOG_LEVEL_MAX
} eponCtl_logLevel_t;

/**
 * Logging API: Activate by #defining EPONCTL_LOG_SUPPORTED
 **/
#define EPONCTL_LOG_NAME "eponctl"


#define EPONCTL_LOG_FUNC() EPONCTL_LOG_DEBUG(" ")

#define EPONCTL_LOG_DEBUG(fmt, arg...)                                     \
    EPONCTL_LOGCODE( if(eponCtl_logLevelIsEnabled(EPONCTL_LOG_LEVEL_DEBUG)) \
                         printf(CLRg "[DBG " "%s" "] %-10s: " fmt CLRnl, \
                             EPONCTL_LOG_NAME, __FUNCTION__, ##arg); )

#define EPONCTL_LOG_INFO(fmt, arg...)                                      \
    EPONCTL_LOGCODE( if(eponCtl_logLevelIsEnabled(EPONCTL_LOG_LEVEL_INFO)) \
                         printf(CLRm "[INF " "%s" "] %-10s: " fmt CLRnl, \
                                EPONCTL_LOG_NAME, __FUNCTION__, ##arg); )

#define EPONCTL_LOG_NOTICE(fmt, arg...)                                    \
    EPONCTL_LOGCODE( if(eponCtl_logLevelIsEnabled(EPONCTL_LOG_LEVEL_NOTICE)) \
                         printf(CLRb "[NTC " "%s" "] %-10s: " fmt CLRnl, \
                                EPONCTL_LOG_NAME, __FUNCTION__, ##arg); )

/**
 * Error Reporting API: Activate by #defining EPONCTL_ERROR_SUPPORTED
 **/

#define EPONCTL_LOG_ERROR(fmt, arg...)                                     \
    EPONCTL_ERRORCODE( if(eponCtl_logLevelIsEnabled(EPONCTL_LOG_LEVEL_ERROR)) \
                           printf(CLRerr "[ERROR " "%s" "] %-10s, %d: " fmt CLRnl, \
                                  EPONCTL_LOG_NAME, __FUNCTION__, __LINE__, ##arg); )

/**
 * Assert API: Activate by #defining EPONCTL_ASSERT_SUPPORTED
 **/

#define EPONCTL_ASSERT(_cond)                                              \
    EPONCTL_ASSERTCODE( if(!(_cond))                                       \
                        { printf(CLRerr "[ASSERT " "%s" "] %-10s,%d: " #_cond CLRnl, \
                                 __FILE__, __FUNCTION__, __LINE__);     \
                        } )


static eponCtl_logLevel_t eponCtl_logLevel = EPONCTL_LOG_LEVEL_ERROR;
#ifdef DESKTOP_LINUX
#define  ioctl(fd, cmd, param) 0
#else
static int eponStackCtlfileDesc = 0;
#endif

int eponCtl_logLevelIsEnabled(eponCtl_logLevel_t logLevel)
{
    EPONCTL_ASSERT(logLevel >= 0 && logLevel < EPONCTL_LOG_LEVEL_MAX);

    if(eponCtl_logLevel >= logLevel)
    {
        return 1;
    }

    return 0;
}

int eponCtl_setLogLevel(eponCtl_logLevel_t logLevel)
{
    if(logLevel >= 0 && logLevel < EPONCTL_LOG_LEVEL_MAX)
    {
        EPONCTL_LOG_ERROR("Invalid Log Level: %d (allowed values are 0 to %d)", logLevel, EPONCTL_LOG_LEVEL_MAX-1);

        return -EINVAL;
    }

    eponCtl_logLevel = logLevel;

    return 0;
}

eponCtl_logLevel_t eponCtl_getLogLevel(void)
{
    return eponCtl_logLevel;
}

void UDumpData (U8 * addr, U16 size)
	{
	U16  i;
	printf("size:%d\n",size);
	for (i = 0; i < size; ++i)
		{
		if ((i % 16) == 0)
			{
			printf("\n");
			}
		printf("%02x ", (*addr++));
		}
	printf("\n");
	//sleep(1);
	} // DisplayMemory

#define PRINT_IOCTL_ERROR(x) if(x < 0) {EPONCTL_LOG_ERROR("%s", strerror(errno));}


EponCfgMoPara eponMoPara[] = 
    {
    {EponCfgItemPortUpRate,      "upRateMap",    2},
    {EponCfgItemPortDnRate,      "dnRateMap",    2},
    {EponCfgItemPortLaserOn,        "laserOn",      2},
    {EponCfgItemPortLaserOff,       "laserOff",     2},
    {EponCfgItemPortTxPolarity,     "txPolarity",   1},
    {EponCfgItemPortOffTimeOffset,  "offTimeOffset",1},
    {EponCfgItemPortMaxFrameSize,   "maxFrameSize", 2},
    {EponCfgItemPortPreDraft2dot1,  "preDraft2dot1",1},
    {EponCfgItemPortNttReporting,   "nttReporting", 1},
    {EponCfgItemPortPowerupTime,    "powerupTime",  2},
    {EponCfgItemPortTxOffIdle,      "txOffIdle",    1},
    {EponCfgItemShaperipg,          "shaperipg",    1},
    {EponCfgItemPonMac,             "ponMac",       6},
    {EponCfgItemHoldover,           "holdover",     3},
    {EponCfgItemActiveTime,         "activeTime",   2},
    {EponCfgItemStandbyTime,        "standbyTime",  2},
    {EponCfgItemDefaultLinkNum,     "defaultlinknum",1},
    {EponCfgItemPsCfg,              "psCfg",        2},
    {EponCfgItemOamSel,             "oamSel",       1},
    {EponCfgItemSchMode,            "schMode",      1},
    {EponCfgItemIdileTimOffset,     "idleTimeOffset",1},
    };

int EponStatckCtlInit(void)
{
    int ret = 0;
#ifndef DESKTOP_LINUX     
    if(eponStackCtlfileDesc <= 0)
    {    
        eponStackCtlfileDesc = open(BCM_EPON_DRIVER_PATH, O_RDWR);
        if(eponStackCtlfileDesc < 0)
        {
            EPONCTL_LOG_ERROR("%s: %s", BCM_EPON_DRIVER_PATH, strerror(errno));

            ret = -EINVAL;
        }
    }
#endif
    return ret;
}


int eponStack_CtlCfgEponCfg(EponCtlOpe ope,EponCfgParam *value)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!value)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;
    eponParms.eponparm.poncfg = value;
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgPers, &eponParms);
	PRINT_IOCTL_ERROR(ret)		
    else
        {
        if (ope == EponGetOpe)
            {
            memcpy(value,eponParms.eponparm.poncfg,sizeof(EponCfgParam));
            }
        }

	return ret;
	}

int eponStack_CtlLoadEponCfg(U8 flag)
    {
    int ret = 0;
    EponCtlParamt eponParms;
	
    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponSetOpe;
    eponParms.eponparm.eponact = flag;
    ret = ioctl(eponStackCtlfileDesc, BCMEPONLoadPers, &eponParms);
    PRINT_IOCTL_ERROR(ret)

    return ret;
    }


int eponStack_CtlCfgDebug(EponCtlOpe ope,DebugPara *value)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!value)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	
	eponParms.ope = ope;
	memcpy(&eponParms.eponparm.debug,value,sizeof(DebugPara));	
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgDebug, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		if (ope == EponGetOpe)
			{
			memcpy(value,&eponParms.eponparm.debug,sizeof(DebugPara));
			}
		}
	
	//UDumpData(value,sizeof(DebugPara));
	return ret;
	}


int eponStack_CtlCfgRegister(EponCtlOpe ope,U32 regStart ,U8 count,U32 *regVal)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if ((!regVal) && (count == 0))
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;
	eponParms.eponparm.reg.regStart = regStart;
	eponParms.eponparm.reg.regCount = count;
	if ((count == 1) && (ope == EponSetOpe))
		{		
		eponParms.eponparm.reg.regval = *regVal;		
		}
	else if (count < MaxRegCount)
		{
		eponParms.eponparm.reg.regval = (U32)regVal;
		}
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgReg, &eponParms);
	PRINT_IOCTL_ERROR(ret)

	return ret;
	}



int eponStack_DumpStats(U8 dumpid)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	eponParms.eponparm.statsdumpid = dumpid;
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONDumpStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	
	return ret;
	}

int eponStack_StatsGather(BOOL flag)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	eponParms.eponparm.gather = flag;
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONGather, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	
	return ret;
	}



int eponStack_CtlLifStatsGet(StatsCntOne *stats,U8 count)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!stats)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	eponParms.eponparm.stats.port = EponStatsLif;
	eponParms.eponparm.stats.statsCount = count;
	
	if ((count!= 0)&&(count < MaxRegCount))
		{
		eponParms.eponparm.stats.StatsCnt.statsVal2 = stats;
		}
		
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)		

	return ret;
	}

#ifdef CONFIG_EPON_10G_SUPPORT
int eponStack_CtlXifStatsGet(StatsCntOne *stats,U8 count)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!stats)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	eponParms.eponparm.stats.port = EponStatsXif;
	eponParms.eponparm.stats.statsCount = count;
	
	if ((count!= 0)&&(count < MaxRegCount))
		{
		eponParms.eponparm.stats.StatsCnt.statsVal2 = stats;
		}
		
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)		

	return ret;
	}


int eponStack_CtlXpcs32StatsGet(StatsCntOne *stats,U8 count)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!stats)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	eponParms.eponparm.stats.port = EponStatsXpcs32;
	eponParms.eponparm.stats.statsCount = count;
	
	if ((count!= 0)&&(count < MaxRegCount))
		{
		eponParms.eponparm.stats.StatsCnt.statsVal2 = stats;
		}
		
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)		

	return ret;
	}

int eponStack_CtlXpcs40StatsGet(StatsCntOne *stats,U8 count)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!stats)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	eponParms.eponparm.stats.port = EponStatsXpcs40;
	eponParms.eponparm.stats.statsCount = count;
	
	if ((count!= 0)&&(count < MaxRegCount))
		{
		eponParms.eponparm.stats.StatsCnt.statsVal2 = stats;
		}
		
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)		

	return ret;
	}

int eponStack_CtlCfg10gFecAutoDet(EponCtlOpe ope, Bool* enable)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    if (!enable)
        {
        return -EINVAL;
        }
    
    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = ope;
    
    if (ope == EponSetOpe)
        {
        eponParms.eponparm.fec10gAutoDet = *enable;
        }

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfg10gFecAutoDet, &eponParms);

    if (ope == EponGetOpe)
        {
        *enable = eponParms.eponparm.fec10gAutoDet;
        }
    
    PRINT_IOCTL_ERROR(ret)

    return ret;
    }

int eponStack_CtlCfg10gFecSwitchOnce(void)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    
    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    
    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfg10gFecSWOnce, &eponParms);
    
    PRINT_IOCTL_ERROR(ret)

    return ret;
    }

#endif

int eponStack_CtlLinkStatsGet(U8 link, StatsCntOne *stats,U8 count)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!stats)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	eponParms.eponparm.stats.port = EponStatsLink;
    eponParms.eponparm.stats.instance = link;
	eponParms.eponparm.stats.statsCount = count;
	
	if ((count!= 0)&&(count < MaxRegCount))
		{
		eponParms.eponparm.stats.StatsCnt.statsVal2 = stats;
		}
		
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)	

	return ret;
	}

int eponStack_CtlPonStatsGet(StatsCntOne *stats,U8 count)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!stats)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	eponParms.eponparm.stats.port = EponStatsPon;
	eponParms.eponparm.stats.statsCount = count;
	
	if ((count!= 0)&&(count < MaxRegCount))
		{
		eponParms.eponparm.stats.StatsCnt.statsVal2 = stats;
		}
		
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)		

	return ret;
	}

int eponStack_CtlPonStatsClear(void)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	eponParms.eponparm.stats.port = EponStatsPon;
	eponParms.eponparm.stats.statsCount = 0;
			
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)		

	return ret;
	}


int eponStack_CtlL2cpStatsGet(U8 port , L2cpStats *l2cpStats)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	eponParms.eponparm.portL2cpStats.port = port;
    eponParms.eponparm.portL2cpStats.l2cpStats = l2cpStats;
		
	ret = ioctl(eponStackCtlfileDesc, BCMEPONPortL2cpStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)		

	return ret;
	}


int eponStack_CtlL2cpStatsClear(U8 port)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
		
	ret = ioctl(eponStackCtlfileDesc, BCMEPONPortL2cpStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)		

	return ret;
	}


int eponStack_CtlLinkStatsClear(U8 link)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	eponParms.eponparm.stats.port = EponStatsLink;
    eponParms.eponparm.stats.instance = link;
	eponParms.eponparm.stats.statsCount = 0;
			
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)		

	return ret;
	}


int eponStack_CfgLinkNum(EponCtlOpe ope,U8 *num, U8 *onlinelinks)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!num)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;
	if (ope == EponSetOpe)
		{
		eponParms.eponparm.linksInfo.maxLinks = *num;
		}
		
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgLink, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		if (ope == EponGetOpe)
			{
			*num = eponParms.eponparm.linksInfo.maxLinks;
            *onlinelinks = eponParms.eponparm.linksInfo.onlineLinks;
			}
		}		

	return ret;
	}


int eponStack_CtlGetLinkMpcpStatus (U8 link , rdpa_epon_link_mpcp_state *status)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!status)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;

	eponParms.eponparm.linkstatus.link = link;
	ret = ioctl(eponStackCtlfileDesc, BCMEPONGetMpcpStatus, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		*status = eponParms.eponparm.linkstatus.state;
		}		

	return ret;
	}

int eponStack_CtlCfgMacQueue(EponCtlOpe ope, epon_mac_q_cfg_t *cfg)
{
    int ret = 0;
    EponCtlParamt eponParms;

    if (!cfg)
    {
        return -EINVAL;
    }

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));

    eponParms.ope = ope;
    eponParms.eponparm.epon_mac_q.cfg = cfg;
   
    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgQueue, &eponParms);
    PRINT_IOCTL_ERROR(ret)

    return ret;
}

int eponStack_CfgL2PonState(EponCtlOpe ope,WanStatePara *value)
{
    int ret = 0;
    EponCtlParamt eponParms;

    if (!value)
    {
        return -EINVAL;
    }

    EponStatckCtlInit();

    eponParms.ope = ope;
    memcpy(&eponParms.eponparm.wanstate, value,sizeof(WanStatePara));

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgL2PonState, &eponParms);
    PRINT_IOCTL_ERROR(ret);

    if (ope == EponGetOpe)
    {
        memcpy(value, &eponParms.eponparm.wanstate,
            sizeof(WanStatePara));
    }

    return ret;
}

int eponStack_CtlCfgFec(EponCtlOpe ope,FecPara *value)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!value)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;
	if (ope == EponSetOpe)
		{
		memcpy(&eponParms.eponparm.fec,value,sizeof(FecPara));
		}
	else
		{
		eponParms.eponparm.fec.link = value->link;
		}
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgFec, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		if (ope == EponGetOpe)
			{
			memcpy(value,&eponParms.eponparm.fec,sizeof(FecPara));
			}
		}		

	return ret;
	}


int eponStack_CtlCfgHoldover (EponCtlOpe ope, U16 *time, 
    rdpa_epon_holdover_flags *flag)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if ((!time) || (!flag))
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;
	if (ope == EponSetOpe)
		{
		eponParms.eponparm.holdover.time = *time;
		eponParms.eponparm.holdover.flags = *flag;
		}
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONHoldover, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		if (ope == EponGetOpe)
			{
			*time = eponParms.eponparm.holdover.time;
			*flag = eponParms.eponparm.holdover.flags;
			}
		}		

	return ret;
	}


int eponStack_CtlCfgByteLimit (EponCtlOpe ope, U8* queue, U8* limit)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    if ((!queue) || (!limit))
        {
        return -EINVAL;
        }
	
    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = ope;

    eponParms.eponparm.bytelimit.queue = *queue;
    if (ope == EponSetOpe)
        {
        eponParms.eponparm.bytelimit.limit = *limit;
        }
	
    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgByteLimit, &eponParms);
    PRINT_IOCTL_ERROR(ret)
    else
        {
        if (ope == EponGetOpe)
            {
            *limit = eponParms.eponparm.bytelimit.limit;
            }
        }		

    return ret;
    }


int eponStack_CtlCfgProtectSwitch (EponCtlOpe ope, PsOnuState *state)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!state)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;
	if (ope == EponSetOpe)
		{
		eponParms.eponparm.psstate = *state;
		}
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgProtParm, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		if (ope == EponGetOpe)
			{
			*state = eponParms.eponparm.psstate;
			}
		}		

	return ret;
	}


int eponStack_CtlCfgLosCheckTime (EponCtlOpe ope, LosCheckPara *para)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!para) 
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;
	if (ope == EponSetOpe)
		{
		eponParms.eponparm.lostime.losopttime = para->losopttime;
		eponParms.eponparm.lostime.losmpcptime = para->losmpcptime;
		}
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONLosTime, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		if (ope == EponGetOpe)
			{
			para->losopttime = eponParms.eponparm.lostime.losopttime;
			para->losmpcptime = eponParms.eponparm.lostime.losmpcptime;
			}
		}		

	return ret;
	}

int eponStack_CtlGetLLID(U8 link ,U16 *llid)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!llid)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	eponParms.eponparm.llid.link = link;
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONGetLLID, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		*llid = eponParms.eponparm.llid.LLID;
		}		

	return ret;
	}

int eponStack_CtlGetLinkIndex(U16 llid ,U8 *linkIndex)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!linkIndex)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	eponParms.eponparm.llid.LLID = llid;
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONGetLinkIndex, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		*linkIndex = eponParms.eponparm.llid.link;
		}		

	return ret;
	}

int eponStack_CtlGetMaxPhyLlidNum(U16 *num)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!num)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONGetMaxPhyLlidNum, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		*num = eponParms.eponparm.maxPhyLlidNum;
		}

	return ret;
	}

int eponStack_CtlCfgCtcAlarmState (EponCtlOpe ope, U16 *alarmid,U8 count, BOOL *enable)
	{
	int ret = 0;
	U8 i = 0;
	EponCtlParamt eponParms;
	CtcStatsAlarm ctcalm[100];
	
	if ((!alarmid) || (!enable) || (count > 99))
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;
	eponParms.eponparm.ctcstatsalm.statsCount = count;
	eponParms.eponparm.ctcstatsalm.statsAlmVal = ctcalm;
	memset(ctcalm,0,sizeof(ctcalm));
			
	for (i = 0;i < count;i++)
		{
		ctcalm[i].alarmId = alarmid[i];
		if (ope == EponSetOpe)
			{
			ctcalm[i].enable = enable[i];
			}
		}
		
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCtcAlm, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		if (ope == EponGetOpe)
			{
			for (i = 0;i < count;i++)
				{
				enable[i] = ctcalm[i].enable;					
				}
			}
		}		

	return ret;
	}

int eponStack_CtlCfgCtcAlarmThreshold (EponCtlOpe ope,
											U16 *alarmid,
											U8 count ,
											U32 *setthreshold, 
											U32 *clearthreshold)
	{
	int ret = 0;
	U8 i = 0;
	EponCtlParamt eponParms;
	CtcStatsAlmThe ctcalmthe[100];
	
	if ((!alarmid) || (!setthreshold) || 
		(!clearthreshold)|| (count > 99))
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;
	eponParms.eponparm.ctcstatsthe.statsCount = count;
	eponParms.eponparm.ctcstatsthe.statsTheVal = ctcalmthe;
	memset(ctcalmthe,0,sizeof(ctcalmthe));

	for (i = 0;i < count;i++)
		{
		ctcalmthe[i].alarmId = alarmid[i];
		if (ope == EponSetOpe)
			{
			ctcalmthe[i].setThe  = setthreshold[i];
			ctcalmthe[i].clearThe = clearthreshold[i];
			}
		}

	ret = ioctl(eponStackCtlfileDesc, BCMEPONCtcThe, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		if (ope == EponGetOpe)
			{
			for (i = 0;i < count;i++)
				{
				setthreshold[i] = ctcalmthe[i].setThe;
				clearthreshold[i] = ctcalmthe[i].clearThe;					
				}
			}
		}		

	return ret;
	}

int eponStack_CtlCfgCtcStatsPeriod (EponCtlOpe ope, BOOL *enable,U32 *period)

	{
	int ret = 0;
	EponCtlParamt eponParms;

	if ((!period) || (!enable))
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;	

	if (ope == EponSetOpe)
		{
		if (*enable == FALSE)
			{
			eponParms.eponparm.ctcstatsperiod = 0;
			}
		else
			{
			eponParms.eponparm.ctcstatsperiod = *period;
			}
		}
		

	ret = ioctl(eponStackCtlfileDesc, BCMEPONCtcPer, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		if (ope == EponGetOpe)
			{
			if (eponParms.eponparm.ctcstatsperiod == 0)
				{
				*enable = FALSE;			
				}
			else
				{
				*enable = TRUE;
				*period = eponParms.eponparm.ctcstatsperiod;
				}
			}
		}		

	return ret;
	}	

int eponStack_CtlGetCtcStats (BOOL history, U8 *statsid, U8 count, U64 *dst)
	{
	int ret = 0;
	U8 i = 0;
	EponCtlParamt eponParms;
	StatsCntOne cnt[100];

	if ((!statsid) || (!dst) || (count > 99))
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;	

	eponParms.eponparm.ctcstats.history = history;
	eponParms.eponparm.ctcstats.statsCount = count;
	memset(cnt,0,sizeof(cnt));
	for (i = 0;i < count;i++)
		{
		cnt[i].statsId = statsid[i];
		}
	eponParms.eponparm.ctcstats.statsVal = cnt;
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCtcStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		for (i = 0;i < count;i++)
			{
			dst[i] = cnt[i].statsVal;
			}
		}		

	return ret;
	}

int eponStack_CtlClearCtcStats (void)
	{
	int ret = 0;
	EponCtlParamt eponParms;

	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCtcStats, &eponParms);
	PRINT_IOCTL_ERROR(ret)	

	return ret;
	}

int eponStack_CtCfgBurstCap (EponCtlOpe ope ,U8 link, U16 *size,U8 *count)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	U16 burstsize[8];
	
	if ((!size) || (!count))
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;
	
	memset(burstsize,0,sizeof(burstsize));
	eponParms.eponparm.bcapval.link = link;
	eponParms.eponparm.bcapval.bcapsize = burstsize;
	
	if ((ope == EponSetOpe) && (*count <= 8))
		{
		eponParms.eponparm.bcapval.count = *count;
		memcpy(burstsize,size,eponParms.eponparm.bcapval.count*sizeof(U16));			
		}

	ret = ioctl(eponStackCtlfileDesc, BCMEPONBCap, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
                {
                if (ope == EponGetOpe)
                	{
                	*count = eponParms.eponparm.bcapval.count;
                	memcpy(size,burstsize,eponParms.eponparm.bcapval.count*sizeof(U16));
                	}
                }		

	return ret;
	}

int eponStack_CtlCfgShaper (EponCtlOpe ope ,U32 *shaperL1Map,	U32 *rate,
						  U16 *size,U8 *shp)
	{
	int ret = 0;
	EponCtlParamt eponParms;

	if ((!shaperL1Map) || (!rate) || (!size) || (!shp))
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = ope;

	if (ope == EponSetOpe)
		{
		eponParms.eponparm.shpval.add = TRUE;
		eponParms.eponparm.shpval.l1map = *shaperL1Map;
		eponParms.eponparm.shpval.rate = *rate;
		eponParms.eponparm.shpval.size = *size;
		}
	else
		{
		eponParms.eponparm.shpval.shpid = *shp;
		}

	ret = ioctl(eponStackCtlfileDesc, BCMEPONShaper, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
	        {
		if (ope == EponSetOpe)
			{
			*shp = eponParms.eponparm.shpval.shpid;
			}
		else
			{
			*shaperL1Map = eponParms.eponparm.shpval.l1map;
			*rate = eponParms.eponparm.shpval.rate;
			*size = eponParms.eponparm.shpval.size;
			}
	        }		

	return ret;
	}

int eponStack_CtlDelShaper (U8 shp) 
	{
	int ret = 0;
	EponCtlParamt eponParms;

	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	eponParms.eponparm.shpval.add = FALSE;
	eponParms.eponparm.shpval.shpid = shp;
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONShaper, &eponParms);
	PRINT_IOCTL_ERROR(ret)	

	return ret;
	}

int eponStack_CtlSetTxPower (BOOL actOpt, U16 enableTime)
	{
	int ret = 0;
	EponCtlParamt eponParms;

	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	eponParms.eponparm.txpower.actOpt = actOpt;
	eponParms.eponparm.txpower.enabletime = enableTime;

	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgTxPower, &eponParms);
	PRINT_IOCTL_ERROR(ret)	

	return ret;
	}

int eponStack_CtlSetLaserEnable (Direction dir, BOOL enable)
	{
	int ret = 0;
	EponCtlParamt eponParms;

	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	eponParms.eponparm.laserpara.dir = dir;
	eponParms.eponparm.laserpara.enable = enable;

	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgLaserEn, &eponParms);
	PRINT_IOCTL_ERROR(ret)	

	return ret;
	}

int eponStack_CtlSetSilence (BOOL flag, U8 silence)
	{
	int ret = 0;
	EponCtlParamt eponParms;

	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	eponParms.eponparm.silence.flag = flag;
	eponParms.eponparm.silence.time = silence;

	ret = ioctl(eponStackCtlfileDesc, BCMEPONSilence, &eponParms);
	PRINT_IOCTL_ERROR(ret)	

	return ret;
	}

int eponStack_CtlGetKeyInuse (U8 link, U8 *keyindex)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	if (!keyindex)
		{
		return -EINVAL;
		}
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	eponParms.eponparm.keyinuse.link = link;
	ret = ioctl(eponStackCtlfileDesc, BCMEPONKeyInuse, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		*keyindex = eponParms.eponparm.keyinuse.keyinUse;
		}		

	return ret;
	}


int eponStack_CtlSetKeyMode (U8 link, EncryptMode mode, EncryptOptions opts)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	eponParms.eponparm.keymode.link = link;
	eponParms.eponparm.keymode.mode = mode;
	eponParms.eponparm.keymode.opts = opts;
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONKeyMode, &eponParms);
	PRINT_IOCTL_ERROR(ret)		

	return ret;
	}


int eponStack_CtlSetKeyData (U8 link, Direction dir,U8 keyindex,U32 *key,U8 length, U32 *sci)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!key)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	eponParms.eponparm.keydata.link = link;
	eponParms.eponparm.keydata.dir = dir;
	eponParms.eponparm.keydata.keyindex = keyindex;
	eponParms.eponparm.keydata.length = length;
	eponParms.eponparm.keydata.key = key;
	eponParms.eponparm.keydata.sci = sci;
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONKeyData, &eponParms);
	PRINT_IOCTL_ERROR(ret)	

	return ret;
	}


int eponStack_CtlSetPid (int pid)
	{
	int ret = 0;
	EponCtlParamt eponParms;

	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	eponParms.eponparm.nlpid = pid;
	ret = ioctl(eponStackCtlfileDesc, BCMEPONSetPid, &eponParms);
	PRINT_IOCTL_ERROR(ret)		

	return ret;
	}

int eponStack_CtlAssignMcast (U8 link, U16 phyLlid, U8 flags, BOOL isStandalone)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponSetOpe;
    eponParms.eponparm.assignMcast.link = link;
    eponParms.eponparm.assignMcast.assignedPort = phyLlid;
    eponParms.eponparm.assignMcast.flags = flags;
    eponParms.eponparm.assignMcast.idxMcast = isStandalone ? 0 : EPON_ASSOCIATE_LINK_IDX;

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgAssignMcast, &eponParms);
    PRINT_IOCTL_ERROR(ret)

    return ret;
    }

int eponStack_CtlGetMcast (U8 link, U16 *phyLlid, U16 *idxMcast, U8 flags)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponGetOpe;
    eponParms.eponparm.assignMcast.link = link;
    eponParms.eponparm.assignMcast.flags = flags;
    eponParms.eponparm.assignMcast.assignedPort = *phyLlid;
    eponParms.eponparm.assignMcast.idxMcast = *idxMcast;
    
    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgAssignMcast, &eponParms);
    PRINT_IOCTL_ERROR(ret)
    else
        {
        *phyLlid = eponParms.eponparm.assignMcast.assignedPort;
        *idxMcast = eponParms.eponparm.assignMcast.idxMcast;
        }       

    return ret;
    }


int eponStack_CtlLinkLoopback (EponCtlOpe ope, U8 link, BOOL* loopback)
    {
    int ret = 0;
    EponCtlParamt eponParms;
	
    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = ope;
    eponParms.eponparm.loopback.link = link;
    if (ope == EponSetOpe)
        {
        eponParms.eponparm.loopback.isOn = (*loopback)?1:0;
        }
		
    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgLoopback, &eponParms);
		
    if (ope == EponGetOpe)
        {
        *loopback = (eponParms.eponparm.loopback.isOn == 1)?TRUE:FALSE;
        }
		
    PRINT_IOCTL_ERROR(ret)
    return ret;
    }


#ifdef CLOCK_TRANSPORT
int eponStack_CtlSetClkTrans (ClkTransPara * para)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponSetOpe;
    memcpy(&eponParms.eponparm.clkTrans, para,sizeof(ClkTransPara));

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgClktrans, &eponParms);
    PRINT_IOCTL_ERROR(ret)		

    return ret;
    }

int eponStack_CtlSetClk1ppsTickTrans(OamExtMpcpClock * para)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponSetOpe;
    memcpy(&eponParms.eponparm.extmpcpClk, para,sizeof(OamExtMpcpClock));

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgClk1ppsTickTrans, &eponParms);
    PRINT_IOCTL_ERROR(ret)		

    return ret;
    }

int eponStack_CtlGetClk1ppsCompTrans(TkOamOnuClkTransConfig * para)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponGetOpe;
	
    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgClk1ppsCompTrans, &eponParms);
    PRINT_IOCTL_ERROR(ret)
    else
        {
        memcpy(para, &(eponParms.eponparm.clkTransCfg), sizeof(TkOamOnuClkTransConfig));
        }       

    return ret;
    }

int eponStack_CtlSetClk1ppsCompTrans(TkOamOnuClkTransConfig * para)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponSetOpe;
    memcpy(&eponParms.eponparm.clkTransCfg, para,sizeof(TkOamOnuClkTransConfig));

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgClk1ppsCompTrans, &eponParms);
    PRINT_IOCTL_ERROR(ret)		

    return ret;
    }

int eponStack_CtlSetClkTodTrans(OamExtTimeOfDay * para)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponSetOpe;
    memcpy(&eponParms.eponparm.extTod, para,sizeof(OamExtTimeOfDay));

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgClkTodTrans, &eponParms);
    PRINT_IOCTL_ERROR(ret)		

    return ret;
    }

int eponStack_CtlGetClockTransferTime(DpoeClockTransferTime * para)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponGetOpe;
	
    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgClkTransferTime, &eponParms);
    PRINT_IOCTL_ERROR(ret)
    else
        {
        para->length = eponParms.eponparm.clkTransportTime.todLen + sizeof(U32);
        para->mpcpRefClock = eponParms.eponparm.clkTransportTime.mpcpRefClock;
        memcpy(para->tod, eponParms.eponparm.clkTransportTime.strTod, eponParms.eponparm.clkTransportTime.todLen);
        }       

    return ret;
    }

int eponStack_CtlSetClockTransferTime(DpoeClockTransferTime * para)
    {
    int ret = 0;
    EponCtlParamt eponParms;
    int i;

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponSetOpe;

    eponParms.eponparm.clkTransportTime.mpcpRefClock = para->mpcpRefClock;
    eponParms.eponparm.clkTransportTime.todLen = para->length - sizeof(U32);
    memcpy(eponParms.eponparm.clkTransportTime.strTod, para->tod, MIN((para->length - sizeof(U32)), TodStringMaxLength));

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgClkTransferTime, &eponParms);
    PRINT_IOCTL_ERROR(ret)		

    return ret;
    }

int eponStack_CtlGetClockPropagationParameter(ClockPropagationParam * para)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponGetOpe;
	
    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgClkPropagationParameter, &eponParms);
    PRINT_IOCTL_ERROR(ret)
    else
        {
        memcpy(para, &eponParms.eponparm.clkPropagationParameter, sizeof(ClockPropagationParam));
        }       

    return ret;
    }

int eponStack_CtlSetClockPropagationParameter(ClockPropagationParam * para)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = EponSetOpe;
    memcpy(&eponParms.eponparm.clkPropagationParameter, para, sizeof(ClockPropagationParam));

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgClkPropagationParameter, &eponParms);
    PRINT_IOCTL_ERROR(ret)		

    return ret;
    }

#endif

int eponStack_CtlGetReportMode(PonMgrRptMode *mode)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!mode)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponGetOpe;
	
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgReport, &eponParms);
	PRINT_IOCTL_ERROR(ret)
	else
		{
		*mode = eponParms.eponparm.reportmod;
		}		

	return ret;
	}

int eponStack_CtlSetReportMode(PonMgrRptMode mode)
	{
	int ret = 0;
	EponCtlParamt eponParms;
	
	if (!mode)
		{
		return -EINVAL;
		}
	
	EponStatckCtlInit();
	memset(&eponParms,0,sizeof(EponCtlParamt));
	eponParms.ope = EponSetOpe;
	eponParms.eponparm.reportmod = mode;
    
	ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgReport, &eponParms);
	PRINT_IOCTL_ERROR(ret)			

	return ret;
	}


int eponStack_CtlCfgRogueOnuDet(
        EponCtlOpe ope, Bool* enable, U32* threshold, U8* times)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    if ((!enable) || (!threshold) || (!times))
        {
        return -EINVAL;
        }
    
    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = ope;
    
    if (ope == EponSetOpe)
        {
        eponParms.eponparm.rogueonudetect.enable = *enable;
        eponParms.eponparm.rogueonudetect.threshold = *threshold;
        eponParms.eponparm.rogueonudetect.times = *times;
        }

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgRogueOnuDet, &eponParms);

    if (ope == EponGetOpe)
        {
        *enable = eponParms.eponparm.rogueonudetect.enable;
        *threshold = eponParms.eponparm.rogueonudetect.threshold;
        *times = eponParms.eponparm.rogueonudetect.times;
        }
    
    PRINT_IOCTL_ERROR(ret)

    return ret;
    }

int eponStack_CtlCfgFailSafe(EponCtlOpe ope, Bool* enable)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    if (!enable)
        {
        return -EINVAL;
        }
    
    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = ope;
    
    if (ope == EponSetOpe)
        {
        eponParms.eponparm.failsafe = *enable;
        }

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgFailSafe, &eponParms);

    if (ope == EponGetOpe)
        {
        *enable = eponParms.eponparm.failsafe;
        }
    
    PRINT_IOCTL_ERROR(ret)

    return ret;
    }
    
int eponStack_CtlSetMaxFrameSize(U16 maxFrameSize)
	{
    int ret = 0;
    EponCtlParamt eponParms;
    EponCfgParam  ponCfg;
    
    EponStatckCtlInit();
    memset(&ponCfg,0,sizeof(EponCfgParam));
    memset(&eponParms,0,sizeof(EponCtlParamt));
        
    ponCfg.eponparm.maxFrameSize = maxFrameSize;
    eponParms.ope = EponSetOpe;
    eponParms.eponparm.poncfg = &ponCfg;

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgMaxFrameSize, &eponParms);
    
    PRINT_IOCTL_ERROR(ret)

    return ret;
    }

int eponStack_CtlDumpEpnInfo(EponCtlOpe ope,EpnInfoId_e eponInfoId,U32 epnSetVal)
	{
    int ret = 0;
    EponCtlParamt eponParms;
    
    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
        
    eponParms.ope = ope;
    eponParms.eponparm.epninfo.epnInfoId = eponInfoId;
    eponParms.eponparm.epninfo.epnSetVal = epnSetVal;

    ret = ioctl(eponStackCtlfileDesc, BCMEPONDumpEpnInfo, &eponParms);
    
    PRINT_IOCTL_ERROR(ret)

    return ret;
    }

int eponStack_CtlMpcpRegister(EponCtlOpe ope, BOOL* enable)
    {
    int ret = 0;
    EponCtlParamt eponParms;

    if (!enable)
        {
        return -EINVAL;
        }
    
    EponStatckCtlInit();
    memset(&eponParms,0,sizeof(EponCtlParamt));
    eponParms.ope = ope;
    
    if (ope == EponSetOpe)
        {
        eponParms.eponparm.MpcpRegisterEn = *enable;
        }

    ret = ioctl(eponStackCtlfileDesc, BCMEPONCfgMpcpRegisterEn, &eponParms);

    if (ope == EponGetOpe)
        {
        *enable = eponParms.eponparm.MpcpRegisterEn;
        }
    
    PRINT_IOCTL_ERROR(ret)

    return ret;
    }

// end of Eponctl_api.c

