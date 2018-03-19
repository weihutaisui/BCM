/****************************************************************************
#
#  Copyright 2016, Broadcom Corporation
#
# <:label-BRCM:2016:proprietary:standard
# 
#  This program is the proprietary software of Broadcom and/or its
#  licensors, and may only be used, duplicated, modified or distributed pursuant
#  to the terms and conditions of a separate, written license agreement executed
#  between you and Broadcom (an "Authorized License").  Except as set forth in
#  an Authorized License, Broadcom grants no license (express or implied), right
#  to use, or waiver of any kind with respect to the Software, and Broadcom
#  expressly reserves all rights in and to the Software and all intellectual
#  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
#  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
#  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
# 
#  Except as expressly set forth in the Authorized License,
# 
#  1. This program, including its structure, sequence and organization,
#     constitutes the valuable trade secrets of Broadcom, and you shall use
#     all reasonable efforts to protect the confidentiality thereof, and to
#     use this information only in connection with your use of Broadcom
#     integrated circuit products.
# 
#  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
#     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
#     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
#     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
#     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
#     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
#     PERFORMANCE OF THE SOFTWARE.
# 
#  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
#     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
#     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
#     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
#     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
#     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
#     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
#     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
#     LIMITED REMEDY.
# :>
****************************************************************************
*
*  Filename: cli2_dect.c
*
****************************************************************************
*  Description:
*
*
****************************************************************************/

/****************************************************************************
*
*  cli2_dect.c
*
*  PURPOSE:
*
*  NOTES:
*
****************************************************************************/

#ifdef SUPPORT_CLI_CMD
#ifdef DMP_VOICE_SERVICE_2

#ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1

/* ---- Include Files ---------------------------------------------------- */

#include "cms_util.h"
#include "cms_core.h"
#include "cms_cli.h"
#include "cli.h"
#include "cms_msg.h"
#include "dal_voice.h"
#include "dect_msg.h"

/* ---- Private Define --------------------------------------------------- */

#define cmdDectGet         cmdDectFunc
#define cmdDectSet         cmdDectFunc
#define cmdDectAdd         cmdDectFunc
#define cmdDectDel         cmdDectFunc
#define cmdDectHandset     cmdDectFunc
#define cmdDectCall        cmdDectFunc
#define cmdDectTest        cmdDectFunc
#define cmdDectDebug       cmdDectFunc


/* ---- Public Variables ------------------------------------------------- */

/* ---- Constants and Types ---------------------------------------------- */

#define VOIP_PARMNAME_ARG_INDEX   1     /* Index of parameter name on the commandline string */
#define VOIP_FIRST_PARMARG_INDEX  2     /* Index of first argument after param name on the commandline string */

#define MAX_CLICMD_NAME_SIZE      30

#define MAX_OPTS                  10


const char dectusage[]            = "\nUsage:\n";
const char decths[]               = "       dect hs <reg|ping|del|info> [value] ...\n";
const char decthsreg[]            = "       dect hs reg <enable|disable>\n";
const char decthsping[]           = "       dect hs ping <all|handsetId>\n";
const char decthsdel[]            = "       dect hs del  <handsetId>\n";
const char decthsinfo[]           = "       dect hs info <handsetId>\n";
const char dectinfo[]             = "       dect info\n";
const char dectget[]              = "       dect get <ac|mode|prom|linesetting|systemsetting|contactlist> [value] ... \n";
const char dectgetlas[]           = "       dect get las [value] ... \n";
const char dectgetmode[]          = "       dect get mode\n";
const char dectgetprom[]          = "       dect get prom <address> <length>\n";
const char dectreghandsetlist[]   = "       dect get handsetlist\n";
const char decthandsetnumber[]    = "       dect get totalhandset\n";
const char dectsystemsetting[]    = "       dect get systemsetting\n";
const char dectset[]              = "       dect set <ac|mode|prom|default> [value] ...\n";
const char dectTest[]             = "       dect test <synctime|vmwi> [value] ...\n";
const char dectadd[]              = "       dect add <contact|call|handset> [value] ...\n";
const char dectaddcontact[]       = "       dect add contact <name> <first name> <number> <melody value> <line id>\n";
const char dectaddcall[]          = "       dect add call <name> <number> <date-time> <line name> <line id> <"MDMVS_IN"|"MDMVS_OUT"|"MDMVS_MISSED">\n";
const char dectaddhandset[]       = "       dect add handset <handset id> <handset name> <IPUI> \n";
const char dectdel[]              = "       dect del <contact|call> [value] ...\n";
const char dectdellist[]          = "       dect del <contact|call|handset> <index id>\n";
const char dectsetac[]            = "       dect set ac <accessCode>\n";
const char dectsethsetname[]      = "       dect set hsetname <handset id> <name>\n";
const char dectsetlas[]           = "       dect set las <parameters> ...\n";
const char dectsetprom[]          = "       dect set prom <address> <value>\n";
const char dectsetmode[]          = "       dect set mode <1|0>  \"0 - CAT-iq; 1 - DECT 6.0\"\n";
const char dectsetdefault[]       = "       dect set default\n";
const char decttestsynctime[]     = "       dect test synctime \n";
const char decttestvmwi[]         = "       dect test vmwi <multiplicity>\n";
const char dectstart[]            = "       dect start\n";
const char dectstop[]             = "       dect stop\n";
const char dectsave[]             = "       dect save\n";
const char decthelp[]             = "       dect help [cmd]\n";
const char decterror[]            = "       error\n";
const char dectlaslinesetting[]   = "       dect get linesetting sp line\n";
const char dectlascontactlist[]   = "       dect get contactlist\n";
const char dectlasinternalname[]  = "       dect get hsetname\n";
const char dectlascall[]          = "       dect get calllist\n";
const char dectlaslinename[]      = "       dect set las linename sp line name\n";
const char dectlaslineid[]        = "       dect set las lineid sp line <id>\n";
const char dectlasmelody[]        = "       dect set las melody sp line melody_number\n";
const char dectlascallmode[]      = "       dect set las callmode sp line <"MDMVS_SINGLE"|"MDMVS_MULTIPLE">\n";
const char dectlasintrusioncall[] = "       dect set las intrusioncall sp line <"MDMVS_ENABLED"|"MDMVS_DISABLED">\n";
const char dectlashandsetlist[]   = "       dect set las handsetlist sp line <handset1,[handset2,[handset3,[...]]]>\n";
const char dectlaspincode[]       = "       dect set las pincode <code>\n";
const char dectlasbasereset[]     = "       dect set las basereset <"MDMVS_YES">\n";
const char dectlasclockmaster[]   = "       dect set las clockmaster <"MDMVS_FP"|"MDMVS_PP">\n";
const char dectCall[]             = "       dect test call <pcalloutext|pcalloutint|pcallrel|cwaccept|cwreject|release|replace|hold|unhold> <handset ID> <Call-ID>\n";
const char dectCallpcalloutext[]  = "       dect test call pcalloutext <handset ID> <Call-ID>\n";
const char dectCallpcalloutint[]  = "       dect test call pcalloutint <handset ID> <Call-ID>\n";
const char dectCallpcallrel[]     = "       dect test call pcallrel <handset ID> <Call-ID>\n";
const char dectCallcwaccept[]     = "       dect test call cwaccept <handset ID> <Call-ID>\n";
const char dectCallcwreject[]     = "       dect test call cwreject <handset ID> <Call-ID>\n";
const char dectCallrelease[]      = "       dect test call release <handset ID> <Call-ID>\n";
const char dectCallreplace[]      = "       dect test call replace <handset ID> <Call-ID>\n";
const char dectCallintrude[]      = "       dect test call intrude <handset ID> <Call-ID>\n";
const char dectCallintercept[]    = "       dect test call intercept <handset ID> <Call-ID> <other handset ID>\n";
const char dectCallhold[]         = "       dect test call hold <handset ID> <Call-ID>\n";
const char dectCallunhold[]       = "       dect test call unhold <handset ID> <Call-ID>\n";
const char dectCalltoggle[]       = "       dect test call toggle <handset ID> <Call-ID>\n";
const char dectCallconf[]         = "       dect test call conf <handset ID> <Call-ID>\n";
const char dectdebug[]            = "       dect debug <start|stop>\n";

struct CMD_FUNC_MAP
{
   const char cmd[20];
   CmsRet (*func)( unsigned int argc, const char **value, struct CMD_FUNC_MAP *map );
   int numArgs;
   /*
    * aPos is the parameters start position in the argv[]
    * for example: "dect set ac 5555", "set" will be argv[0],
    *              "ac" will be argv[1], "5555" will be argv[2]
    *
    */
   int aPos;     /* the first parameter position in argv[] */
   struct CMD_FUNC_MAP *next;
   const char *help;
};

typedef struct CMD_FUNC_MAP CMD_FUNC_MAP;

typedef struct
{
   unsigned int cmd;    /* call control command */
   char cmdTxt[20];      /* command text */
} DECT_CTL_CMD_TXT_MAP;

/* ---- Private Function Prototypes -------------------------------------- */

/* generic help command */
static CmsRet cmdDectHelp(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);


/* Add commands */
static CmsRet cmdDectAddContact(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectAddCall(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectAddHandset(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);

/* Del commands */
static CmsRet cmdDectDelContactList(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectDelCall(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectDelHandset(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);


/* LAS related commands */
static CmsRet cmdDectSetLasLineId(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSetLasLineName(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSetLasMultiCallMode(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSetLasMelody(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSetLasIntrusionCall(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSetLasAttachedHandset(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSetLasPinCode(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSetLasClockMaster(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);

static CmsRet cmdDectGetLineSetting(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectGetContactList(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectGetInternalNameList(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectGetCall(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectGetTotalRegHandset(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectGetRegisteredHandset(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);

/* Call related commands */
static CmsRet cmdDectCallCmd(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);


/* Handset related commands */
static CmsRet cmdDectHandsetRegister(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectHandsetPing(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectHandsetDelete(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectHandsetInfo(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);

/* Set commands */
static CmsRet cmdDectFunc(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
#if 0
static CmsRet cmdDectHandset(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectGet(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectAdd(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectDel(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSet(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
#endif
static CmsRet cmdDectSetDefault(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSetAc(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSetMode(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSetEEProm(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectSetHsetName(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectTestVmwi(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);

/* Get commands */
static CmsRet cmdDectGetEEProm(unsigned int argc,const  char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectGetMode(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectGetSystemSetting(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);

/* Info command */
static CmsRet cmdDectInfo(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);

/* Start/Stop commands */
static CmsRet cmdDectStart(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectStop(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);

/* Save command */
static CmsRet cmdDectSave(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);

#if 0
/* Reset  to default*/
static CmsRet cmdDectReset(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
#endif

/* Date and time synchronization of the DECT FP and PP*/
static CmsRet cmdDectTestSyncTime(unsigned int argc,const char **argv, CMD_FUNC_MAP *map);

static unsigned int cmdLineParse(char ** argArray, char * argBuffer);
static CmsRet validHandsetId ( unsigned int hsIdx );

static CmsRet cmdDectDebug(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectDebugStart(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
static CmsRet cmdDectDebugStop(unsigned int argc, const char **argv, CMD_FUNC_MAP *map);
/* ---- Private Variables ------------------------------------------------ */

static DECT_CTL_CMD_TXT_MAP dectCtlCmdTextMap[] =
{
   { MSG_DECT_CALL_CTL_SETUP,      "callout"     },
   { MSG_DECT_CALL_CTL_ACCEPT,     "callacc"     },
   { MSG_DECT_CALL_CTL_RELEASE,    "release"     },
   { MSG_DECT_CALL_CTL_PARAL_INT,  "pcalloutint" },
   { MSG_DECT_CALL_CTL_PARAL_EXT,  "pcalloutext" },
   { MSG_DECT_CALL_CTL_TOGGLE,     "toggle"      },
   { MSG_DECT_CALL_CTL_3WAY_CONF,  "conf"        },
   { MSG_DECT_CALL_CTL_TRANSFER,   "transfer"    },
   { MSG_DECT_CALL_CTL_CW_ACCEPT,  "cwaccept"    },
   { MSG_DECT_CALL_CTL_CW_REJECT,  "cwreject"    },
   { MSG_DECT_CALL_CTL_REPLACE,    "replace"     },
   { MSG_DECT_CALL_CTL_INTRUSION,  "intrude"     },
   { MSG_DECT_CALL_CTL_HOLD,       "hold"        },
   { MSG_DECT_CALL_CTL_UNHOLD,     "unhold"      },
   { MSG_DECT_CALL_CTL_INTERCEPT,  "intercept"   },
   { MSG_DECT_CALL_CTL_REL_PARAL,  "pcallrel"    },
   { DECT_UNKNOWN_ID            ,  "unknown"     }
};

static CMD_FUNC_MAP cmdDectSetLasFuncMap[] =
{
   { "lineid",         cmdDectSetLasLineId,            3, 3, cmdDectSetLasFuncMap, dectlaslineid          },
   { "linename",       cmdDectSetLasLineName,          3, 3, cmdDectSetLasFuncMap, dectlaslinename        },
   { "melody",         cmdDectSetLasMelody,            3, 3, cmdDectSetLasFuncMap, dectlasmelody          },
   { "callmode",       cmdDectSetLasMultiCallMode,     3, 3, cmdDectSetLasFuncMap, dectlascallmode        },
   { "intrusioncall",  cmdDectSetLasIntrusionCall,     3, 3, cmdDectSetLasFuncMap, dectlasintrusioncall   },
   { "handsetlist",    cmdDectSetLasAttachedHandset,   3, 3, cmdDectSetLasFuncMap, dectlashandsetlist     },
   { "pincode",        cmdDectSetLasPinCode,           1, 3, cmdDectSetLasFuncMap, dectlaspincode         },
   { "clockmaster",    cmdDectSetLasClockMaster,       1, 3, cmdDectSetLasFuncMap, dectlasclockmaster     },
   { "NULL",           NULL,                           0, 0, NULL,                 decterror              }
};

#if 0
static CMD_FUNC_MAP cmdDectGetLasFuncMap[] =
{
   { "NULL", NULL, 0, 3, NULL, decterror }
};
#endif

static CMD_FUNC_MAP cmdDectSetFuncMap[] =
{
   { "ac",       cmdDectSetAc,       1, 2, cmdDectSetFuncMap,    dectsetac        },
   { "las",      cmdDectSet,         1, 2, cmdDectSetLasFuncMap, dectsetlas       },
   { "mode",     cmdDectSetMode,     1, 2, cmdDectSetFuncMap,    dectsetmode      },
   { "prom",     cmdDectSetEEProm,   2, 2, cmdDectSetFuncMap,    dectsetprom      },
   { "hsetname", cmdDectSetHsetName, 2, 2, cmdDectSetFuncMap,    dectsethsetname  },
   { "default",  cmdDectSetDefault,  0, 2, cmdDectSetFuncMap,    dectsetdefault   },
   { "NULL",     NULL,               0, 0, NULL,                 decterror        }
};

static CMD_FUNC_MAP cmdDectGetFuncMap[] =
{
   { "mode",           cmdDectGetMode,              0, 2, cmdDectGetFuncMap, dectgetmode          },
   { "prom",           cmdDectGetEEProm,            2, 2, cmdDectGetFuncMap, dectgetprom          },
   { "handsetlist",    cmdDectGetRegisteredHandset, 0, 2, cmdDectGetFuncMap, dectreghandsetlist   },
   { "totalhandset",   cmdDectGetTotalRegHandset,   0, 2, cmdDectGetFuncMap, decthandsetnumber    },
   { "systemsetting",  cmdDectGetSystemSetting,     0, 2, cmdDectGetFuncMap, dectsystemsetting    },
   { "linesetting",    cmdDectGetLineSetting,       2, 2, cmdDectGetFuncMap, dectlaslinesetting   },
   { "contactlist",    cmdDectGetContactList,       0, 2, cmdDectGetFuncMap, dectlascontactlist   },
   { "hsetname",       cmdDectGetInternalNameList,  0, 2, cmdDectGetFuncMap, dectlasinternalname  },
   { "calllist",       cmdDectGetCall,              0, 2, cmdDectGetFuncMap, dectlascall          },
   { "NULL",           NULL,                        0, 0, NULL,              decterror            }
};

static CMD_FUNC_MAP cmdDectDebugFuncMap[] =
{
   { "start",          cmdDectDebugStart,           0, 2, cmdDectDebugFuncMap, dectdebug            },
   { "stop",           cmdDectDebugStop,            0, 2, cmdDectDebugFuncMap, dectdebug            },
   { "NULL",           NULL,                        0, 0, NULL,                decterror            }
};

static CMD_FUNC_MAP cmdDectHsFuncMap[] =
{
   { "ping",  cmdDectHandsetPing,     0, 2, cmdDectHsFuncMap, decthsping  },
   { "reg",   cmdDectHandsetRegister, 1, 2, cmdDectHsFuncMap, decthsreg   },
   { "del",   cmdDectHandsetDelete,   1, 2, cmdDectHsFuncMap, decthsdel   },
   { "info",  cmdDectHandsetInfo,     1, 2, cmdDectHsFuncMap, decthsinfo  },
   { "NULL",  NULL,                   0, 0, NULL,             decterror   }
};

#if 0
static CMD_FUNC_MAP cmdDectHelpFuncMap[] =
{
   { "ping",  cmdDectHandsetPing,     0, 2, cmdDectHsFuncMap, decthsping  },
   { "reg",   cmdDectHandsetRegister, 1, 2, cmdDectHsFuncMap, decthsreg   },
   { "del",   cmdDectHandsetDelete,   1, 2, cmdDectHsFuncMap, decthsdel   },
   { "info",  cmdDectHandsetInfo,     1, 2, cmdDectHsFuncMap, decthsinfo  },
   { "NULL",  NULL,                   0, 0, NULL,             decterror   }
};
#endif

static CMD_FUNC_MAP cmdDectAddFuncMap[] =
{
   { "contact",   cmdDectAddContact,   5, 2, cmdDectAddFuncMap, dectaddcontact },
   { "call",      cmdDectAddCall,      6, 2, cmdDectAddFuncMap, dectaddcall    },
   { "handset",   cmdDectAddHandset,   3, 2, cmdDectAddFuncMap, dectaddhandset },
   { "NULL",      NULL,                0, 2, NULL,              decterror      }
};

static CMD_FUNC_MAP cmdDectDelFuncMap[] =
{
   { "contact",   cmdDectDelContactList,  1, 2, cmdDectDelFuncMap, dectdellist },
   { "call",      cmdDectDelCall,         1, 2, cmdDectDelFuncMap, dectdellist },
   { "handset",   cmdDectDelHandset,      1, 2, cmdDectDelFuncMap, dectdellist },
   { "NULL",      NULL,                   0, 2, NULL,              decterror   }
};

static CMD_FUNC_MAP cmdDectCallFuncMap[] =
{
   { "pcalloutext",  cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCallpcalloutext  },
   { "pcalloutint",  cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCallpcalloutint  },
   { "pcallrel",     cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCallpcallrel     },
   { "cwaccept",     cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCallcwaccept     },
   { "cwreject",     cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCallcwreject     },
   { "toggle",       cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCalltoggle       },
   { "hold",         cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCallhold         },
   { "unhold",       cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCallunhold       },
   { "release",      cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCallrelease      },
   { "pcallrel",     cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCallpcallrel     },
   { "replace",      cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCallreplace      },
   { "intrude",      cmdDectCallCmd,   2, 3, cmdDectCallFuncMap, dectCallintrude      },
   { "intercept",    cmdDectCallCmd,   3, 3, cmdDectCallFuncMap, dectCallintercept    },
   { "NULL",         NULL,             0, 0, NULL,               decterror            }
};

static CMD_FUNC_MAP cmdDectTestFuncMap[] =
{
   { "synctime",     cmdDectTestSyncTime, 0, 2, cmdDectTestFuncMap, decttestsynctime },
   { "vmwi",         cmdDectTestVmwi,     2, 2, cmdDectTestFuncMap, decttestvmwi     },  
   { "call",         cmdDectCall,         1, 2, cmdDectCallFuncMap, dectCall         },
   { "NULL",         NULL,                0, 0, NULL,               decterror        }
};


static CMD_FUNC_MAP cmdDectFuncMap[] =
{
/*  cmd name  cmd function    paramters   parm position, func_map,          help */
   { "get",   cmdDectGet,      1,          1,            cmdDectGetFuncMap, dectget },
   { "set",   cmdDectSet,      1,          1,            cmdDectSetFuncMap, dectset },
   { "add",   cmdDectAdd,      1,          1,            cmdDectAddFuncMap, dectadd },
   { "del",   cmdDectDel,      1,          1,            cmdDectDelFuncMap, dectdel },
//   { "call",  cmdDectCall,     1,          1,            cmdDectCallFuncMap,dectCall},
   { "hs",    cmdDectHandset,  1,          1,            cmdDectHsFuncMap,  decths  },
   { "test",  cmdDectTest,     1,          1,            cmdDectTestFuncMap,dectTest },
   { "start", cmdDectStart,    0,          1,            cmdDectFuncMap,    dectstart},
   { "stop",  cmdDectStop,     0,          1,            cmdDectFuncMap,    dectstop },
   { "save",  cmdDectSave,     0,          1,            cmdDectFuncMap,    dectsave },
   { "help",  cmdDectHelp,     0,          1,            cmdDectFuncMap,    decthelp },
   { "info",  cmdDectInfo,     0,          1,            cmdDectFuncMap,    dectinfo },
   { "debug", cmdDectDebug,    1,          1,            cmdDectDebugFuncMap, dectdebug },   
   { "NULL",  NULL,            0,          0,            NULL,              decterror}
};


/* ---- Function implementations ----------------------------------------- */

void processDectCmd(char *cmdLine)
{
   unsigned int argc = 0;
   unsigned int j=0;
   char *argv[MAX_OPTS]={NULL};

   cmsLog_debug("processDectCtlCmd called %s start\n", cmdLine);

   cliCmdSaveNeeded = FALSE;

   argc = cmdLineParse( argv, cmdLine );

   if(argc == 0 || argv[0] == NULL )
   {
      cmdDectHelp(argc, (const char **)argv, cmdDectFuncMap);
      return;
   }

   for(j=0; cmdDectFuncMap[j].func != NULL; j++)
   {
      if(!strcasecmp(argv[0], cmdDectFuncMap[j].cmd))
      {
         /* this first argument is cmd self, the second would be parameter */
         if( ( argc-1) >= cmdDectFuncMap[j].numArgs)
         {
           cmdDectFuncMap[j].func(argc, (const char **)argv, cmdDectFuncMap[j].next);
           return;
         }
         else
         {
            cmdDectHelp(argc, (const char **)argv, cmdDectFuncMap[j].next);
            return;
         }
         break;
      }
   }


   cmdDectHelp(argc, (const char **)argv, cmdDectFuncMap);
   cmsLog_debug("processDectCmd called %s end\n", cmdLine);
   return;
}

#if 0
static CmsRet cmdDectReset(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   char              value[20] = "yes\0";
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   parms.op[0] = 1;

   cmsLck_acquireLock();

   ret = dalVoice_SetDectResetBase( &parms, value );

   cmsLck_releaseLock();

   return ret;
}
#endif

static CmsRet cmdDectTestSyncTime(unsigned int argc,const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   void            *msgBuf;


   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader), ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   msg  = (CmsMsgHeader *) msgBuf;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_SYNC_DATE_TIME;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = 0;

   /* Send to dectctl application */
   if ((ret = cmsMsg_send( cliPrvtMsgHandle, msg )) != CMSRET_SUCCESS)
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
   return ret;
}

static CmsRet cmdDectSetHsetName(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet            ret;
   DAL_VOICE_PARMS   parms;
   unsigned int      i, total = 0, found = 0, position = 0, handsetId;
   char             *name = NULL;
   char             *tid = NULL;
   unsigned int      idlist[DAL_MAX_INSTANCE_IN_LIST];

   position = map[0].aPos;
   tid  = (char *)(argv[position]);
   name = (char *)(argv[position +1]);

   handsetId = atoi(tid);

   parms.op[0] = 1;
   cmsLck_acquireLock();
   ret = dalVoice_GetDectCurHsetList(&parms, (int *)&total, idlist);
   cmsLck_releaseLock();
   if(ret != CMSRET_SUCCESS )
   {
      return ret;
   }

   for(i=0; i< total ; i++)
   {
      if( handsetId == idlist[i] )
      {
         found = 1;
         break;
      }
   }

   if(found)
   {
      parms.op[1] = handsetId;
      cmsLck_acquireLock();
      ret = dalVoice_SetDectHsetName( &parms, name);
      cmsLck_releaseLock();
   }
   else
   {
      printf("handset id %d does not exist\n", handsetId);
   }

   return ret;
}

static CmsRet cmdDectTestVmwi(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   char            *data;
   void            *msgBuf;
   DectCMEventMsgBody* info;
   int              i;


   if( map == NULL )
   {
      cmsLog_error("%s(): NULL pointer", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      i = map[0].aPos;
   }

   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader) +
                                (sizeof(DectCMEventMsgBody)),
                                ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   /* prepared message header and body */
   msg  = (CmsMsgHeader *) msgBuf;
   data = (char *) (msg + 1);
   info = (DectCMEventMsgBody *)data;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_CM_EVENT;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = sizeof(DectCMEventMsgBody);

   /* initialize i to the first paramter */
   info->event = MSG_DECT_CM_SIG_MWI;
   info->lineId = atoi(argv[i]);
   info->handsetId = 1;
   info->connectionId = 1;
   info->dataLength = 1;
   info->data[0] = atoi(argv[i+1]);

   /* Send to dectctl application */
   if ((ret = cmsMsg_send( cliPrvtMsgHandle, msg )) != CMSRET_SUCCESS)
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
   return ret;
}


static CmsRet cmdDectSetAc(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   int  i;
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * DECT Interface Obj is per voice service instance
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   parms.op[0] = 1;
   parms.op[1] = OGF_NORMAL_UPDATE;
   parms.msgHandle = cliPrvtMsgHandle;
   i = map[0].aPos;

   cmsLck_acquireLock();

   ret = dalVoice_SetDectAc( &parms, (char *)argv[i] );

   cmsLck_releaseLock();

   return ret;
}

static CmsRet cmdDectSetMode(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   char            *data;
   void            *msgBuf;
   VoiceDiagMsgBody* info;
   int              i;


   if( map == NULL )
   {
      cmsLog_error("%s(): NULL pointer", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      i = map[0].aPos;
   }

   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader) +
                                (sizeof(VoiceDiagMsgBody)),
                                ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   /* prepared message header and body */
   msg  = (CmsMsgHeader *) msgBuf;
   data = (char *) (msg + 1);
   info = (VoiceDiagMsgBody *)data;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_MODE_SET;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = sizeof(VoiceDiagMsgBody);

   /* initialize i to the first paramter */
   info->type = VOICE_DIAG_DECT_MODE_SET;
   sprintf(info->cmdLine, "%s", argv[i]);

   /* Send to dectctl application */
   if ((ret = cmsMsg_send( cliPrvtMsgHandle, msg )) != CMSRET_SUCCESS)
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
   return ret;
}


static CmsRet cmdDectSetEEProm(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   char            *data;
   void            *msgBuf;
   VoiceDiagMsgBody* info;
   int              i;

   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader) +
                                (sizeof(VoiceDiagMsgBody)),
                                ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   msg  = (CmsMsgHeader *) msgBuf;
   data = (char *) (msg + 1);
   info = (VoiceDiagMsgBody *)data;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_MEM_SET;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = sizeof(VoiceDiagMsgBody);

   /* initialize i to the first paramter */
   i = map[0].aPos;

   info->type  =  VOICE_DIAG_DECT_MEM_SET;
   sprintf(info->cmdLine, "%s %s", argv[i], argv[i+1]);

   /* Send to dectctl application */
   if ((ret = cmsMsg_send( cliPrvtMsgHandle, msg )) != CMSRET_SUCCESS)
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
   return ret;
}

static CmsRet cmdDectSetDefault(unsigned int argc,const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet          ret = CMSRET_SUCCESS;

   return ret;
}

static CmsRet cmdDectGetEEProm(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg = NULL;
   char            *data;
   void            *msgBuf;
   VoiceDiagMsgBody* info;
   int              i;

   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader) +
                                (sizeof(VoiceDiagMsgBody)),
                                ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   msg  = (CmsMsgHeader *) msgBuf;

   data = (char *) (msg + 1);
   info = (VoiceDiagMsgBody *)data;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_MEM_GET;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = sizeof(VoiceDiagMsgBody);

   /* initialize i to the first paramter */
   i = map[0].aPos;

   info->type  =  VOICE_DIAG_DECT_MEM_SET;
   sprintf(info->cmdLine, "%s %s", argv[i], argv[i+1]);

   /* Send to dectctl application */
   if(( ret = cmsMsg_send(cliPrvtMsgHandle, msg )) != CMSRET_SUCCESS )
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   return ret;
}

static CmsRet cmdDectFunc(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   int i=0, j=0;

   cmsLog_debug("cmdDectFunc() start\n");
   /* check paramter list
    * first paramter in list should be "set"
    */
   if (argc == 0 || map == NULL )
   {
      cmsLog_debug("cmdDectFunc() invalid paramters\n");
      return CMSRET_INVALID_PARAM_VALUE;
   }

   /* Print all arguments */
   cmsLog_debug("Total arguments: %d\n", argc);
   for ( i = 0; i< argc; i++ )
   {
      cmsLog_debug("Arg%d:%s ", i, argv[i]);
      if ( i == argc - 1 )
      {
         cmsLog_debug("\n");
      }
   }

   for(j=0; map[j].func != NULL; j++ )
   {
      if(!strcasecmp(argv[map[j].aPos-1], map[j].cmd))
      {
         /* this first argument is cmd self, the second would be parameter */
         if( ( argc-map[j].aPos ) >= map[j].numArgs)
         {
            return( map[j].func(argc, (const char **)argv, map[j].next));
         }
         else
         {
            return(cmdDectHelp(argc, (const char **)&map[j].cmd, map[j].next));
         }
         break;
      }
   }

   cmsLog_debug("cmdDectFunc() end\n");
   return ( cmdDectHelp(argc, (const char **)argv, map ));
}

static CmsRet cmdDectGetMode(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   void            *msgBuf;

   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader), ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   /* prepared message header and body */
   msg  = (CmsMsgHeader *) msgBuf;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_MODE_GET;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = 0;

   /* Send to dectctl application */
   if ((ret = cmsMsg_send( cliPrvtMsgHandle, msg )) != CMSRET_SUCCESS)
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
   return ret;
}

static CmsRet cmdDectInfo(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   DAL_VOICE_DECT_INFO*           varObj = NULL;
   char *						  tempObj = NULL;
   DAL_VOICE_PARMS parms;
   CmsRet          ret = CMSRET_SUCCESS;

   /* Allocate memory for the object to be obtained through the DAL layer. */
   varObj = (DAL_VOICE_DECT_INFO *)cmsMem_alloc( sizeof(DAL_VOICE_DECT_INFO), ALLOC_ZEROIZE );
   if ( varObj == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object \n" );
      return CMSRET_INTERNAL_ERROR;
   }

   tempObj = (char *)cmsMem_alloc( CLI_MAX_BUF_SZ, ALLOC_ZEROIZE );
   if ( tempObj == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object \n" );
      return CMSRET_INTERNAL_ERROR;
   }

   /*
    * DECT Interface Obj is per voice service instance
    * Only vpIndex parm is need, so far the DECT interface
    * object will only support 1 voice service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   parms.op[0] = 1;
   parms.op[1] = OGF_NORMAL_UPDATE;

   cmsLck_acquireLock();

   if(( ret = dalVoice_GetDectInfo( &parms, varObj)) == CMSRET_SUCCESS)
   {
      printf("Dect Service:           %s\n", varObj->serviceEnabled ? "Enabled" : "Disabled");
	  printf("     Link Date:         %s\n", varObj->linkDate);
      printf("     Module type:       %s\n", (varObj->type == 0x00) ? "Euro" : ((varObj->type == 0x01) ? "US" : "Invalid") );
	  if(( ret = dalVoice_GetDectFwVer( &parms, tempObj, CLI_MAX_BUF_SZ )) == CMSRET_SUCCESS)
      {
         printf("     Firmware Ver:      %s\n", tempObj);
      }
      printf("     Module id:         %s\n", varObj->dectId);
      printf("     Manufacturer id:   0x%8X\n", varObj->manic);
      printf("     Modic:             0x%8X\n", varObj->modic);
      printf("     Register Window:   %s\n", varObj->waitingSubscription ? "Open" : "Closed");
      printf("     Access Code:       %s\n", varObj->accessCode);
   }

   /* Dect current registered hset */
   memset(tempObj, 0, CLI_MAX_BUF_SZ);
   if(( ret = dalVoice_GetDectMaxHset( &parms, tempObj, CLI_MAX_BUF_SZ )) == CMSRET_SUCCESS)
   {
      printf("     Max     handset:   %s\n", tempObj);
   }

   /* Dect current registered hset */
   memset(tempObj, 0, CLI_MAX_BUF_SZ);
   if(( ret = dalVoice_GetDectCurHset( &parms, tempObj, CLI_MAX_BUF_SZ )) == CMSRET_SUCCESS)
   {
      printf("     Active  handset:   %s\n", tempObj);
   }

   cmsLck_releaseLock();

   cmsMem_free( varObj );
   cmsMem_free( tempObj	);

   return ret;
}

/* Start/Stop commands */
static CmsRet cmdDectStart(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   void            *msgBuf;

   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader),
                                ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   /* prepared message header and body */
   msg  = (CmsMsgHeader *) msgBuf;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_START;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 0;
   msg->dataLength = 0;

   if(( ret = cmsMsg_send(cliPrvtMsgHandle, msg)) != CMSRET_SUCCESS )
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   return ret;
}
static CmsRet cmdDectStop(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   void            *msgBuf;

   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader),
                                ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   /* prepared message header and body */
   msg  = (CmsMsgHeader *) msgBuf;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_STOP;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = 0;

   if(( ret = cmsMsg_sendAndGetReplyWithTimeout(cliPrvtMsgHandle, msg, 1000)) != CMSRET_SUCCESS )
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   return ret;
}

/* Save command */
static CmsRet cmdDectSave(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet  ret = CMSRET_SUCCESS;

   cliCmdSaveNeeded = TRUE;

   return ret;
}


static CmsRet cmdDectCallCmd(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet               ret = CMSRET_SUCCESS;
   CmsMsgHeader         *msg;
   char                 *data;
   void                 *msgBuf;
   DectCallCtlCmdBody*  info;
   int                  i = 0;
   unsigned int         hsetId = 0;
   unsigned int         cid = 0;
   const char *               pCallCtrlCmdStr = NULL;
   MSG_DECT_CALL_CTL_CMD callCtlCmd;

   if( map == NULL )
   {
      cmsLog_error("%s(): NULL pointer", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }
   else
   {
      pCallCtrlCmdStr = argv[map[0].aPos - 1];
      hsetId = atoi(argv[map[0].aPos ]);
//       cid    = atoi(argv[map[0].aPos + 1]);
      cid    = strtoul(argv[map[0].aPos + 1], NULL, 16);
      cid |= 0xCAFE0000;


      if( pCallCtrlCmdStr != NULL )
      {
         /* Map the command text to actual command */
         while( dectCtlCmdTextMap[i].cmd != DECT_UNKNOWN_ID )
         {
            if( !strcmp( dectCtlCmdTextMap[i].cmdTxt, pCallCtrlCmdStr ) )
            {
               break;
            }
            i++;
         }

         if( dectCtlCmdTextMap[i].cmd != DECT_UNKNOWN_ID )
         {
            /* Found a matching command in the map */
            callCtlCmd = dectCtlCmdTextMap[i].cmd;
         }
         else
         {
            /* No matchin command found */
            cmsLog_error("%s(): Can not map Call Command ", __FUNCTION__);
            return CMSRET_INTERNAL_ERROR;
         }
      }
      else
      {
         cmsLog_error("%s(): Call Command not found", __FUNCTION__);
         return CMSRET_INTERNAL_ERROR;
      }

   }

   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader) +
                                (sizeof(DectCallCtlCmdBody)),
                                ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   /* prepared message header and body */
   msg  = (CmsMsgHeader *) msgBuf;
   data = (char *) (msg + 1);
   info = (DectCallCtlCmdBody *)data;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_CALL_CTL_CMD;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_VOICE;
   msg->flags_request = 0;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = sizeof(VoiceDiagMsgBody);

   /* Fill out the Call CTL command */
   info->cmd = callCtlCmd;
   info->lineId = DECT_UNKNOWN_ID;
   info->connectionId = cid;
   info->dstHandsetId = ( callCtlCmd == MSG_DECT_CALL_CTL_INTERCEPT ) ? atoi( argv[map[0].aPos + 2] ) : DECT_UNKNOWN_ID;
   info->srcHandsetId = hsetId;
   info->dataLength = 0;

   cmsLog_debug("%s() cid 0x%08x ",__FUNCTION__, cid);

   /* Send to voice application */
   if ((ret = cmsMsg_send( cliPrvtMsgHandle, msg )) != CMSRET_SUCCESS)
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
   return ret;
}

static CmsRet cmdDectHandsetRegister(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   unsigned int   open = 0;
   CmsRet  ret = CMSRET_SUCCESS;

   if (!strcasecmp(argv[map[0].aPos], "enable"))
   {
      open = 1;
   }
   else if(!strcasecmp(argv[map[0].aPos], "disable"))

   {
      open = 0;
   }
   else
   {
      return CMSRET_INVALID_PARAM_VALUE;
   }

   ret = dalVoice_dectCtlRegWnd(open, cliPrvtMsgHandle);

   return ret;
}

static CmsRet cmdDectHandsetPing(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   unsigned int hset = 0;
   CmsRet       ret = CMSRET_SUCCESS;

   if( strcasecmp(argv[map[0].aPos], "all") == 0)
   {
      hset = 0;
   }
   else
   {
      hset = atoi(argv[map[0].aPos]);
      ret = validHandsetId( hset );
      if(ret != CMSRET_SUCCESS )
      {
         return ret;
      }
   }

   ret = dalVoice_dectCtlPingHset(hset, cliPrvtMsgHandle); /* use 0 as "all handsets" id */

   return ret;
}

static CmsRet cmdDectHandsetDelete(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   unsigned int    hset = 0;
   CmsRet          ret = CMSRET_SUCCESS;

   hset = atoi(argv[map[0].aPos]);

   ret = validHandsetId( hset );
   if(ret != CMSRET_SUCCESS )
   {
      return ret;
   }

   /* Send command to delete the handset registration */
   ret = dalVoice_dectCtlDelHset(hset, cliPrvtMsgHandle);

   return ret;
}

static CmsRet cmdDectHandsetInfo(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   unsigned int    i = 0;
   char*           varObj = NULL;
   DAL_VOICE_PARMS parms;
   CmsRet          ret = CMSRET_SUCCESS;


   i = atoi(argv[map[0].aPos]);

   ret = validHandsetId( i );
   if(ret != CMSRET_SUCCESS )
   {
      return ret;
   }

   /* Allocate memory for the object to be obtained through the DAL layer. */
   varObj = (char *)cmsMem_alloc( CLI_MAX_BUF_SZ, ALLOC_ZEROIZE );
   if ( varObj == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object \n" );
      return CMSRET_INTERNAL_ERROR;
   }
   /*
    * DECT Interface Obj is per voice service instance
    * Only vpIndex parm is need, so far the DECT interface
    * object will only support 1 voice service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   parms.op[0] = 1;
   parms.op[1] = i;

   cmsLck_acquireLock();
   ret = dalVoice_GetDectHsId( &parms, varObj, CLI_MAX_BUF_SZ);
   cmsLck_releaseLock();

   if( ret == CMSRET_SUCCESS)
   {
      printf("Handset Id:               %s\n", varObj);
   }

   memset(varObj, 0, CLI_MAX_BUF_SZ);
   cmsLck_acquireLock();
   ret = dalVoice_GetDectHsStatus( &parms, varObj, CLI_MAX_BUF_SZ);
   cmsLck_releaseLock();

   if( ret == CMSRET_SUCCESS)
   {
      printf("Handset Status:           %s\n", varObj);
   }

   memset(varObj, 0, CLI_MAX_BUF_SZ);
   cmsLck_acquireLock();
   ret = dalVoice_GetDectHsSubTime( &parms, varObj, CLI_MAX_BUF_SZ);
   cmsLck_releaseLock();

   if( ret == CMSRET_SUCCESS)
   {
      printf("Subscription time:        %s\n", varObj);
   }

   memset(varObj, 0, CLI_MAX_BUF_SZ);
   cmsLck_acquireLock();
   ret = dalVoice_GetDectHsIpei( &parms, varObj, CLI_MAX_BUF_SZ);
   cmsLck_releaseLock();

   if( ret == CMSRET_SUCCESS)
   {
      printf("Handset IPEI:             %s\n", varObj);
   }

   memset(varObj, 0, CLI_MAX_BUF_SZ);
   cmsLck_acquireLock();
   ret = dalVoice_GetDectHsModic( &parms, varObj, CLI_MAX_BUF_SZ);
   cmsLck_releaseLock();

   if( ret == CMSRET_SUCCESS)
   {
      printf("Handset Module Id:        %s\n", varObj);
   }

   memset(varObj, 0, CLI_MAX_BUF_SZ);
   cmsLck_acquireLock();
   ret = dalVoice_GetDectHsManic( &parms, varObj, CLI_MAX_BUF_SZ);
   cmsLck_releaseLock();

   if( ret == CMSRET_SUCCESS)
   {
      printf("Manufacturer Id:          %s\n", varObj);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(varObj);

   return ret;
}


/***************************************************************************
 * Function Name: cmdDectHelp
 * Description  : Processes the dect help command.
 * Returns      : void
 ***************************************************************************/
static CmsRet cmdDectHelp(unsigned int argc,const char ** argv, CMD_FUNC_MAP *map)
{
   CmsRet ret = CMSRET_SUCCESS;
   int    i = 0;

   cmsLog_debug("argc=%d", argc);

   for(i=0; map[i].func != NULL && map[i].help != NULL ; i++)
   {
      printf("%s",map[i].help);
   }
#if 0
   if (argc == 0 || argv == NULL )
   {
      printf("%s%s%s%s%s%s%s%s%s", dectusage, decths, dectset, dectget, dectinfo, dectstart, dectstop, dectsave, decthelp);
   }
   else
   {
      if( strcasecmp(argv[0], "help") == 0)
      {
         cmdDectHelp(argc-1, &argv[1], map);
      }
      else if (strcasecmp(argv[0], "hs") == 0)
      {
         printf("%s%s%s%s%s", dectusage, decthsreg, decthsping, decthsdel, decthsinfo);
      }
      else if (strcasecmp(argv[0], "get") == 0)
      {
         printf("%s%s%s%s%s", dectusage, dectgetac, dectgetmode, dectgetprom, dectgetlas);
      }
      else if (strcasecmp(argv[0], "set") == 0)
      {
         printf("%s%s%s%s%s%s", dectusage, dectsetdefault, dectsetac, dectsetmode, dectsetprom, dectsetlas );
      }
      else if (strcasecmp(argv[0], "info") == 0)
      {
         printf("%s%s", dectusage, dectinfo);
      }
      else if (strcasecmp(argv[0], "start") == 0)
      {
         printf("%s%s", dectusage, dectstart);
      }
      else if (strcasecmp(argv[0], "stop") == 0)
      {
         printf("%s%s", dectusage, dectstop);
      }
      else if (strcasecmp(argv[0], "save") == 0)
      {
         printf("%s%s", dectusage, dectsave);
      }
      else if (strcasecmp(argv[0], "las") == 0)
      {
         printf("%s%s%s%s%s", dectlaslinename, dectlaslineid, dectlasmelody, dectlascallmode, dectlasintrusioncall);
      }
      else
      {
         cmdDectHelp( 0 , NULL, NULL );
      }
   }
#endif
   return ret;
}
/***********************************************************
 * This function takes a buffer full of space delimited    *
 * voip configuration parameters and populates an array of *
 * strings as follows:                                     *
 *          cmdLine = dect hs info 0                       *
 *                         ^  ^    ^  ^                    *
 *                         |  |    |  |                    *
 * argv[0][0] <------------+  |    |  |                    *
 * argv[1][0] <---------------+    |  |                    *
 * argv[2][0] <--------------------+  |                    *
 * argv[3][0] <-----------------------+                    *
 *                                                         *
 * Args: argv        - Array of strings to hold all args   *
 *       cmdLine     - cmdLine string                      *
 *                                                         *
 * Returns: Number of arguments parsed                     *
 ***********************************************************/
static unsigned int cmdLineParse( char ** argv, char * cmdLine )
{
   unsigned int argc = 0;
   char *last = NULL;

   /* parse the command line and build the argument vector */
   argv[0] = strtok_r(cmdLine, " ", &last);
   if (argv[0] != NULL)
   {
      for (argc = 1; argc < MAX_OPTS; argc++)
      {
         argv[argc] = strtok_r(NULL, " ", &last);
         if (argv[argc] == NULL)
            break;
         cmsLog_debug("arg[%d]=%s", argc, argv[argc]);
      }
   }

   /* return total number of parameters detected */
   return argc;
}


static CmsRet validHandsetId ( unsigned int hsIdx )
{
   int             i, maxHs;
   unsigned int    curHs, HsList[DAL_MAX_INSTANCE_IN_LIST];
   char*           varObj = NULL;
   DAL_VOICE_PARMS parms;
   CmsRet          ret = CMSRET_SUCCESS;

   /* Allocate memory for the object to be obtained through the DAL layer. */
   varObj = (char *)cmsMem_alloc( CLI_MAX_BUF_SZ, ALLOC_ZEROIZE );
   if ( varObj == NULL )
   {
      cmsLog_error( "Could not get allocate memory for object \n" );
      return CMSRET_INTERNAL_ERROR;
   }

   memset((void *)&parms, 0, sizeof(parms));
   parms.op[0] = 1;

   cmsLck_acquireLock();
   ret = dalVoice_GetDectMaxHset(&parms, varObj, CLI_MAX_BUF_SZ);
   cmsLck_releaseLock();

   if ( CMSRET_SUCCESS != ret )
   {
      cmsLog_error( "Could not get max handset count\n" );
      CMSMEM_FREE_BUF_AND_NULL_PTR(varObj);
      return ret;
   }
   else
   {
      maxHs = atoi(varObj);
      CMSMEM_FREE_BUF_AND_NULL_PTR(varObj);
   }

   cmsLck_acquireLock();
   ret = dalVoice_GetDectCurHsetList(&parms, &curHs, HsList);
   cmsLck_releaseLock();
   if ( CMSRET_SUCCESS != ret )
   {
      cmsLog_error( "Could not get registered handset list\n" );
      return ret;
   }

   if( hsIdx == 0 || hsIdx > maxHs)
   {
      printf( "Exceed handset range ( 1 - %d )\n", maxHs );
      return CMSRET_INVALID_PARAM_VALUE;
   }
   else if( curHs > 0)
   {
      for(i = 0; i < curHs; i++ )
      {
         if( hsIdx == HsList[i])
         {
            return CMSRET_SUCCESS;
         }
      }
   }

   if(curHs == 0 || i >= curHs)
   {
      printf("Unregistered handset %d\n", hsIdx);
      ret = CMSRET_INVALID_PARAM_VALUE;
   }

   return ret;
}

static CmsRet cmdDectGetSystemSetting(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   char value[20];
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */

   parms.op[0] = 1;

   printf("System setting\n");

   memset(value, 0, sizeof(value));
   cmsLck_acquireLock();
   ret = dalVoice_GetDectClockMaster( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        clockmaster: %s\n", value);
   }

   memset(value, 0, sizeof(value));
   cmsLck_acquireLock();
   ret = dalVoice_GetDectResetBase( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        base reset: %s\n", value);
   }

   memset(value, 0, sizeof(value));
   cmsLck_acquireLock();
   ret = dalVoice_GetDectPinCode( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        pin code: %s\n", value);
   }

   memset(value, 0, sizeof(value));
   cmsLck_acquireLock();
   ret = dalVoice_GetDectFwVersion( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        Fw version: %s\n", value);
   }

   memset(value, 0, sizeof(value));
   cmsLck_acquireLock();
   ret = dalVoice_GetDectHwVersion( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        Hw version: %s\n", value);
   }

   memset(value, 0, sizeof(value));
   cmsLck_acquireLock();
   ret = dalVoice_GetDectRomVersion( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        EEPROM version: %s\n", value);
   }

   return CMSRET_SUCCESS;
}

static CmsRet cmdDectAddHandset(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   char             *tid = NULL;
   char             *ipui = NULL;
   char             *name = NULL;
   CmsRet            ret;
   DAL_VOICE_PARMS   parms;
   int position, handsetId;

   parms.op[0]=1; /* voice service instance Id */
   position = map[0].aPos;

   cmsLck_acquireLock();

   tid = (char *)(argv[position]);
   name = (char *)(argv[position + 1]);
   ipui = (char *)(argv[position + 2]);

   handsetId = atoi(tid);
   ret = dalVoice_AddHandset(handsetId);
   if(ret != CMSRET_SUCCESS )
   {
      cmsLog_error("Can't add new handset instance ret = %d\n", ret);
      goto error_return;
   }

   parms.op[1] = handsetId;
   ret = dalVoice_SetDectHandsetName(&parms, name);
   if(ret != CMSRET_SUCCESS )
   {
      cmsLog_error("Can't set handset name ret = %d\n", ret);
      goto error_return;
   }

   ret = dalVoice_SetDectHsIpuiPersist(&parms, ipui);
   if(ret != CMSRET_SUCCESS )
   {
      cmsLog_error("Can't set handset IPUI ret = %d\n", ret);
      goto error_return;
   }

/*
   ret = dalVoice_SetDectHandsetDateTime(handsetId, datetime);
   if(ret != CMSRET_SUCCESS )
   {
      cmsLog_error("Can't add new handset datetime ret = %d\n", ret);
      goto error_return;
   }
*/
error_return:
   cmsLck_releaseLock();
   return ret;
}

static CmsRet cmdDectAddCall(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   char *name = NULL;
   char *number = NULL;
   char *datetime = NULL;
   char *linename = NULL;
   char *lineid = NULL;
   char *calltype = NULL;
   CmsRet            ret;
   unsigned  int     instId;
   DAL_VOICE_PARMS   parms;
   int position;

   parms.op[0]=1; /* voice service instance Id */

   cmsLck_acquireLock();
   ret = dalVoice_AddVoiceCallInstance(&parms, &instId);
   printf("add instance (%d)\n", instId);
   if(ret == CMSRET_SUCCESS )
   {
      parms.op[1] = instId;
      position = map[0].aPos;

      name = (char *)(argv[position]);
      ret = dalVoice_SetVoiceCallName(&parms, name);
      if(ret != CMSRET_SUCCESS )
      {
         cmsLog_error("Can't add name ret = %d\n", ret);
         goto error_return;
      }

      number = (char *)(argv[ position + 1]);
      ret = dalVoice_SetVoiceCallNumber(&parms, number);
      if(ret != CMSRET_SUCCESS )
      {
         cmsLog_error("Can't add call number ret = %d\n", ret);
         goto error_return;
      }

      datetime = (char *)(argv[ position + 2]);
      ret = dalVoice_SetVoiceCallDateTime(&parms, datetime);
      if(ret != CMSRET_SUCCESS )
      {
         cmsLog_error("Can't add new date-time ret = %d\n", ret);
         goto error_return;
      }

      linename = (char *)(argv[ position +3]);
      ret = dalVoice_SetVoiceCallLineName(&parms, linename);
      if(ret != CMSRET_SUCCESS )
      {
         cmsLog_error("Can't add new line name ret = %d\n", ret);
         goto error_return;
      }

      lineid = (char *)(argv[ position + 4]);
      ret = dalVoice_SetVoiceCallLineId(&parms, lineid);
      if(ret != CMSRET_SUCCESS )
      {
         cmsLog_error("Can't add line id ret = %d\n", ret);
         goto error_return;
      }

      calltype = (char *)(argv[ position + 5]);
      ret = dalVoice_SetVoiceCallType(&parms, calltype);
   }
   else
   {
      cmsLog_error("Can't add new instance in contact list ret = %d\n", ret);
   }

error_return:
   cmsLck_releaseLock();
   return ret;
}

static CmsRet cmdDectAddContact(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   char *name = NULL;
   char *firstname = NULL;
   char *number = NULL;
   char *melody = NULL;
   char *lineid = NULL;
   CmsRet            ret;
   unsigned  int     instId;
   DAL_VOICE_PARMS   parms;
   int position;

   parms.op[0]=1; /* voice service instance Id */


   cmsLck_acquireLock();
   ret = dalVoice_AddVoiceContactInstance(&parms, &instId);
   printf("add instance (%d)\n", instId);
   if(ret == CMSRET_SUCCESS )
   {
      parms.op[1] = instId;
      position = map[0].aPos;

      name = (char *)(argv[position]);
      ret = dalVoice_SetDectContactListName(&parms, name);
      if(ret != CMSRET_SUCCESS )
      {
         cmsLog_error("Can't add new name ret = %d\n", ret);
         goto error_return;
      }

      firstname = (char *)(argv[ position + 1]);
      ret = dalVoice_SetDectContactListFirstName(&parms, firstname);
      if(ret != CMSRET_SUCCESS )
      {
         cmsLog_error("Can't add new first name ret = %d\n", ret);
         goto error_return;
      }

      number = (char *)(argv[ position + 2]);
      ret = dalVoice_SetDectContactListNumber(&parms, number);
      if(ret != CMSRET_SUCCESS )
      {
         cmsLog_error("Can't add new number ret = %d\n", ret);
         goto error_return;
      }

      melody = (char *)(argv[ position +3]);
      ret = dalVoice_SetDectContactListMelody(&parms, melody);
      if(ret != CMSRET_SUCCESS )
      {
         cmsLog_error("Can't add new melody ret = %d\n", ret);
         goto error_return;
      }

      lineid = (char *)(argv[ position + 4]);
      ret = dalVoice_SetDectContactListLineId(&parms, lineid);
   }
   else
   {
      cmsLog_error("Can't add new instance in contact list ret = %d\n", ret);
   }

error_return:
   cmsLck_releaseLock();
   return ret;
}



static CmsRet cmdDectDelHandset(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   char             *tid = NULL;
   CmsRet            ret = CMSRET_INTERNAL_ERROR;
   int               handsetId, inst;
   int               position = map[0].aPos;

   tid = (char *)(argv[position]);
   handsetId = atoi(tid);

   cmsLck_acquireLock();
   if(dalVoice_mapHsetId2Instance(handsetId, &inst) == CMSRET_SUCCESS && inst != ANY_INSTANCE )
   {
      ret = dalVoice_dectCtlDelHset( handsetId, cliPrvtMsgHandle );
   }
   else
   {
      printf("handset (%d) isn't registered\n", handsetId);
   }
   cmsLck_releaseLock();

   return ret;
}


static CmsRet cmdDectDelContactList(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet            ret;
   DAL_VOICE_PARMS   parms;


   parms.op[0] = 1;
   parms.op[1] = atoi(argv[map[0].aPos]);

   cmsLck_acquireLock();
   ret = dalVoice_DeleteVoiceContactInstance( &parms );
   cmsLck_releaseLock();

   if(ret == CMSRET_SUCCESS)
   {
      printf("(%u) delete successful\n", parms.op[1]);
   }

   return ret;
}

static CmsRet cmdDectDelCall(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet            ret;
   DAL_VOICE_PARMS   parms;

   parms.op[0] = 1;
   parms.op[1] = atoi(argv[map[0].aPos]);

   cmsLck_acquireLock();
   ret = dalVoice_DeleteVoiceCallInstance(&parms);
   cmsLck_releaseLock();

   if(ret == CMSRET_SUCCESS)
   {
      printf("(%u) delete successful\n", parms.op[1]);
   }

   return ret;
}

static CmsRet cmdDectGetCall(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet            ret;
   DAL_VOICE_PARMS   parms;
   unsigned int      total, i;
   char              value[20];
   unsigned int      iidlist[DAL_MAX_INSTANCE_IN_LIST];

   parms.op[0] = 1;

   cmsLck_acquireLock();
   ret = dalVoice_GetTotalVoiceCalls(&parms, &total, iidlist);
   cmsLck_releaseLock();

   if(ret != CMSRET_SUCCESS )
   {
      return ret;
   }

   printf("Total (%d) calls\n", total);
   printf(" Idx Line Call Type Date                 Name                 Number\n");
   printf("+===+====+=========+====================+====================+===================+\n");

   for( i=0; i< total; i++ )
   {
      parms.op[1] = iidlist[i];
      printf(" %03u ", parms.op[1]);

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetVoiceCallLineId(&parms, value, sizeof(value));
      cmsLck_releaseLock();
      if(ret == CMSRET_SUCCESS)
      {
         printf("%4s ", value);
      }
      
      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetVoiceCallType(&parms, value, sizeof(value));
      cmsLck_releaseLock();
      if(ret == CMSRET_SUCCESS)
      {
         printf("%8s  ", value);
      }

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetVoiceCallDateTime(&parms, value, sizeof(value));
      cmsLck_releaseLock();
      if(ret == CMSRET_SUCCESS)
      {
         printf("%20s ", value);
      }

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetVoiceCallName(&parms, value, sizeof(value));
      cmsLck_releaseLock();
      if(ret == CMSRET_SUCCESS)
      {
         printf("%20s ", value);
      }

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetVoiceCallNumber(&parms, value, sizeof(value));
      cmsLck_releaseLock();
      if(ret == CMSRET_SUCCESS)
      {
         printf("%20s\n", value);
      }

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetVoiceCallLineName(&parms, value, sizeof(value));
      cmsLck_releaseLock();

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetVoiceCallNewFlag(&parms, value, sizeof(value));
      cmsLck_releaseLock();

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetVoiceNumberOfMissedCalls(&parms, value, sizeof(value));
      cmsLck_releaseLock();

   }

   return ret;
}

static CmsRet cmdDectGetInternalNameList(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet            ret;
   DAL_VOICE_PARMS   parms;
   unsigned int      i, total = 0;
   char              value[20];
   unsigned int      idlist[DAL_MAX_INSTANCE_IN_LIST];

   parms.op[0] = 1;

   cmsLck_acquireLock();
   ret = dalVoice_GetDectCurHsetList(&parms, &total, idlist);
   cmsLck_releaseLock();
   if(ret != CMSRET_SUCCESS )
   {
      return ret;
   }

   printf("Total (%d) handsets\n", total);
   printf(" hset Id  Name\n");
   printf("+=======+====================+\n");

   for( i=0; i< total; i++ )
   {
      parms.op[1] = idlist[i];
      printf("  %03u  ", parms.op[1]);

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetDectHsetName(&parms, value, sizeof(value));
      cmsLck_releaseLock();
      if(ret == CMSRET_SUCCESS)
      {
         printf(" %20s\n", value);
      }
   }

   return ret;
}


static CmsRet cmdDectGetContactList(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet            ret;
   DAL_VOICE_PARMS   parms;
   unsigned int      total, i;
   char              value[20];
   unsigned int      iidlist[DAL_MAX_INSTANCE_IN_LIST];

   parms.op[0] = 1;

   cmsLck_acquireLock();
   ret = dalVoice_GetDectTotalContacts(&parms, &total, iidlist);
   cmsLck_releaseLock();

   if(ret != CMSRET_SUCCESS )
   {
      return ret;
   }

   printf("Total (%d) contacts\n", total);
   printf(" Idx Contact Name        First Name           Number              Melody LineId\n");
   printf("+===+===================+====================+===================+======+======+\n");

   for( i=0; i< total; i++ )
   {
      parms.op[1] = iidlist[i];
      printf(" %03u ", parms.op[1]);

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetDectContactListName(&parms, value, sizeof(value));
      cmsLck_releaseLock();
      if(ret == CMSRET_SUCCESS)
      {
         printf("%20s", value);
      }

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetDectContactListFirstName(&parms, value, sizeof(value));
      cmsLck_releaseLock();
      if(ret == CMSRET_SUCCESS)
      {
         printf("%20s", value);
      }

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetDectContactListNumber(&parms, value, sizeof(value));
      cmsLck_releaseLock();
      if(ret == CMSRET_SUCCESS)
      {
         printf("%20s", value);
      }

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetDectContactListMelody(&parms, value, sizeof(value));
      cmsLck_releaseLock();
      if(ret == CMSRET_SUCCESS)
      {
         printf("  %3s ", value);
      }

      memset(value, 0, sizeof(value));
      cmsLck_acquireLock();
      ret = dalVoice_GetDectContactListLineId(&parms, value, sizeof(value));
      cmsLck_releaseLock();
      if(ret == CMSRET_SUCCESS)
      {
         printf(" %4s\n", value);
      }
   }

   return ret;
}

static CmsRet cmdDectGetLineSetting(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   int  vpInst, lineInst, spNum, accNum;
   char value[20];
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * DECT Interface Obj is per voice service instance
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   spNum = atoi(argv[map[0].aPos]);
   accNum = atoi(argv[map[0].aPos + 1]);

   cmsLck_acquireLock();
   /* Map to vpInst */
   ret = dalVoice_mapSpNumToSvcInst( spNum, &vpInst );
   /* Map to lineInst */
   if( ret == CMSRET_SUCCESS )
   {
      parms.op[0] = vpInst;
      parms.op[1] = accNum;
      ret = dalVoice_mapAcntNumToClientInst( &parms,  &lineInst );
   }
   cmsLck_releaseLock();

   if(ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Incorrect provider id or account number\n");
      return ret;
   }

   parms.op[0] = vpInst;
   parms.op[1] = lineInst;

   printf("Line (%02d) setting\n", lineInst);
   memset(value, 0, sizeof(value));

   cmsLck_acquireLock();
   ret = dalVoice_GetLineName( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        name: %s\n", value);
   }

   memset(value, 0, sizeof(value));
   cmsLck_acquireLock();
   ret = dalVoice_GetDectLineId( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        line Id: %s\n", value);
   }

   memset(value, 0, sizeof(value));
   cmsLck_acquireLock();
   ret = dalVoice_GetLineMelody( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        melody: %s\n", value);
   }

   memset(value, 0, sizeof(value));
   cmsLck_acquireLock();
   ret = dalVoice_GetLineMultiCallMode( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        multicall: %s\n", value);
   }

   memset(value, 0, sizeof(value));
   cmsLck_acquireLock();
   ret = dalVoice_GetLineIntrusionCallMode( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        intrusionCall: %s\n", value);
   }

   memset(value, 0, sizeof(value));
   cmsLck_acquireLock();
   ret = dalVoice_GetLineAttachedHandsetList( &parms, value, sizeof(value) );
   cmsLck_releaseLock();
   if(CMSRET_SUCCESS == ret )
   {
      printf("        attached handset list: %s\n", value);
   }

   return CMSRET_SUCCESS;
}

static CmsRet cmdDectGetTotalRegHandset(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   char value[20];
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * DECT Interface Obj is per voice service instance
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   memset(value, 0, sizeof(value));

   parms.op[0] = 1;

   cmsLck_acquireLock();

   ret = dalVoice_GetDectCurHset( &parms, value, sizeof(value) );

   cmsLck_releaseLock();

   if(CMSRET_SUCCESS == ret )
   {
      printf("Total registered handset number: %s\n",  value);
   }

   return ret;
}

static CmsRet cmdDectGetRegisteredHandset(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   char value[20];
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   memset(value, 0, sizeof(value));
   /* voice service instance, should be alway 1 */
   parms.op[0] = 1;

   cmsLck_acquireLock();

   ret = dalVoice_GetDectRegisteredHsList( &parms, value, sizeof(value) );

   cmsLck_releaseLock();

   if(CMSRET_SUCCESS == ret )
   {
      printf("Registered handset list: %s\n", value);
   }

   return ret;
}

static CmsRet cmdDectSetLasPinCode(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   char *value = NULL;
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * DECT Interface Obj is per voice service instance
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   value = (char *)(argv[map[0].aPos]);

   parms.op[0] = 1;

   cmsLck_acquireLock();

   ret = dalVoice_SetDectPinCode( &parms, value );

   cmsLck_releaseLock();

   return ret;
}

static CmsRet cmdDectSetLasClockMaster(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   char *value = NULL;
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * DECT Interface Obj is per voice service instance
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   value = (char *)(argv[map[0].aPos]);

   parms.op[0] = 1;

   cmsLck_acquireLock();

   ret = dalVoice_SetDectClockMaster( &parms, value );

   cmsLck_releaseLock();

   return ret;
}


static CmsRet cmdDectSetLasAttachedHandset(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   int  vpInst, lineInst, spNum, accNum;
   char *value = NULL;
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * DECT Interface Obj is per voice service instance
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   spNum = atoi(argv[map[0].aPos]);
   accNum = atoi(argv[map[0].aPos + 1]);
   value = (char *)(argv[map[0].aPos + 2]);

   cmsLck_acquireLock();
   /* Map to vpInst */
   ret = dalVoice_mapSpNumToSvcInst( spNum, &vpInst );
   /* Map to lineInst */
   if( ret == CMSRET_SUCCESS )
   {
      parms.op[0] = vpInst;
      parms.op[1] = accNum;
      ret = dalVoice_mapAcntNumToClientInst( &parms, &lineInst );
   }

   if(ret != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      cmsLog_error("Incorrect provider id or account number\n");
      return ret;
   }

   parms.op[0] = vpInst;
   parms.op[1] = lineInst;

   ret = dalVoice_SetLineAttachedHandsetList( &parms, value );

   cmsLck_releaseLock();

   return ret;
}

static CmsRet cmdDectSetLasIntrusionCall(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   int  vpInst, lineInst, spNum, accNum;
   char *value = NULL;
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * DECT Interface Obj is per voice service instance
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   spNum = atoi(argv[map[0].aPos]);
   accNum = atoi(argv[map[0].aPos + 1]);
   value = (char *)(argv[map[0].aPos + 2]);

   cmsLck_acquireLock();
   /* Map to vpInst */
   ret = dalVoice_mapSpNumToSvcInst( spNum, &vpInst );
   /* Map to lineInst */
   if( ret == CMSRET_SUCCESS )
   {
      parms.op[0] = vpInst;
      parms.op[1] = accNum;
      ret = dalVoice_mapAcntNumToClientInst( &parms, &lineInst );
   }

   if(ret != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      cmsLog_error("Incorrect provider id or account number\n");
      return ret;
   }

   parms.op[0] = vpInst;
   parms.op[1] = lineInst;

   ret = dalVoice_SetLineIntrusionCall( &parms, value );

   cmsLck_releaseLock();

   return ret;
}

static CmsRet cmdDectSetLasMultiCallMode(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   int  vpInst, lineInst, spNum, accNum;
   char *value = NULL;
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * DECT Interface Obj is per voice service instance
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   spNum = atoi(argv[map[0].aPos]);
   accNum = atoi(argv[map[0].aPos + 1]);
   value = (char *)(argv[map[0].aPos + 2]);

   cmsLck_acquireLock();
   /* Map to vpInst */
   ret = dalVoice_mapSpNumToSvcInst( spNum, &vpInst );
   /* Map to lineInst */
   if( ret == CMSRET_SUCCESS )
   {
      parms.op[0] = vpInst;
      parms.op[1] = accNum;
      ret = dalVoice_mapAcntNumToClientInst( &parms, &lineInst );
   }

   if(ret != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      cmsLog_error("Incorrect provider id or account number\n");
      return ret;
   }

   parms.op[0] = vpInst;
   parms.op[1] = lineInst;

   ret = dalVoice_SetLineMultiCallMode( &parms, value );

   cmsLck_releaseLock();

   return ret;
}

static CmsRet cmdDectSetLasMelody(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   int  vpInst, lineInst, spNum, accNum;
   char *value = NULL;
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * DECT Interface Obj is per voice service instance
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   spNum = atoi(argv[map[0].aPos]);
   accNum = atoi(argv[map[0].aPos + 1]);
   value = (char *)(argv[map[0].aPos + 2]);

   cmsLck_acquireLock();
   /* Map to vpInst */
   ret = dalVoice_mapSpNumToSvcInst( spNum, &vpInst );
   /* Map to lineInst */
   if( ret == CMSRET_SUCCESS )
   {
      parms.op[0] = vpInst;
      parms.op[1] = accNum;
      ret = dalVoice_mapAcntNumToClientInst( &parms, &lineInst );
   }

   if(ret != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      cmsLog_error("Incorrect provider id or account number\n");
      return ret;
   }

   parms.op[0] = vpInst;
   parms.op[1] = lineInst;

   ret = dalVoice_SetLineMelody( &parms, value );

   cmsLck_releaseLock();

   return ret;
}

static CmsRet cmdDectSetLasLineId(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   int  vpInst, lineInst, spNum, accNum;
   char *value = NULL;
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * DECT Interface Obj is per voice service instance
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   spNum = atoi(argv[map[0].aPos]);
   accNum = atoi(argv[map[0].aPos + 1]);
   value = (char *)(argv[map[0].aPos + 2]);

   cmsLck_acquireLock();
   /* Map to vpInst */
   ret = dalVoice_mapSpNumToSvcInst( spNum, &vpInst );
   /* Map to lineInst */
   if( ret == CMSRET_SUCCESS )
   {
      parms.op[0] = vpInst;
      parms.op[1] = accNum;
      ret = dalVoice_mapAcntNumToClientInst( &parms, &lineInst );
   }

   if(ret != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      cmsLog_error("Incorrect provider id or account number\n");
      return ret;
   }

   parms.op[0] = vpInst;
   parms.op[1] = lineInst;

   ret = dalVoice_SetDectLineId( &parms, value );

   cmsLck_releaseLock();

   return ret;
}

static CmsRet cmdDectSetLasLineName(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   int  vpInst, lineInst, spNum, accNum;
   char *value = NULL;
   DAL_VOICE_PARMS   parms;
   CmsRet ret = CMSRET_SUCCESS;

   /*
    * DECT Interface Obj is per voice service instance
    * the DECT interface object will only support 1 voice
    * service instance
    *    op[0] = 1;   fixed vpIndex
    *    op[1] = OGF_NORMAL_UPDATE; dectctl will queury realtime value from dect driver and update MDM accordingly
    *            OGF_NO_VALUE_UPDATE; from MDM directly, save some delay
    */
   spNum = atoi(argv[map[0].aPos]);
   accNum = atoi(argv[map[0].aPos + 1]);
   value = (char *)(argv[map[0].aPos + 2]);

   cmsLck_acquireLock();
   /* Map to vpInst */
   ret = dalVoice_mapSpNumToSvcInst( spNum, &vpInst );
   /* Map to lineInst */
   if( ret == CMSRET_SUCCESS )
   {
      parms.op[0] = vpInst;
      parms.op[1] = accNum;
      ret = dalVoice_mapAcntNumToClientInst( &parms, &lineInst );
   }

   if(ret != CMSRET_SUCCESS)
   {
      cmsLck_releaseLock();
      cmsLog_error("Incorrect provider id or account number\n");
      return ret;
   }

   parms.op[0] = vpInst;
   parms.op[1] = lineInst;

   ret = dalVoice_SetDectLineName( &parms, value );

   cmsLck_releaseLock();

   return ret;
}

static CmsRet cmdDectDebugStart(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   void            *msgBuf;

   cmsLog_error("%s\n", __FUNCTION__);

   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader), ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   /* prepared message header and body */
   msg  = (CmsMsgHeader *) msgBuf;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_DBG_START;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = 0;

   /* Send to dectctl application */
   if ((ret = cmsMsg_send( cliPrvtMsgHandle, msg )) != CMSRET_SUCCESS)
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
   return ret;
}

static CmsRet cmdDectDebugStop(unsigned int argc, const char **argv, CMD_FUNC_MAP *map)
{
   CmsRet          ret = CMSRET_SUCCESS;
   CmsMsgHeader    *msg;
   void            *msgBuf;

   cmsLog_error("%s\n", __FUNCTION__);
   
   if ( (msgBuf = cmsMem_alloc( sizeof(CmsMsgHeader), ALLOC_ZEROIZE)) == NULL)
   {
      cmsLog_error("%s(): msg send failed, can't allocate memory ", __FUNCTION__);
      return CMSRET_INTERNAL_ERROR;
   }

   /* prepared message header and body */
   msg  = (CmsMsgHeader *) msgBuf;

   /* Fill in request message */
   msg->type = CMS_MSG_VOICE_DECT_DBG_STOP;
   msg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
   msg->dst = EID_DECT;
   msg->flags_request = 1;
   msg->flags_bounceIfNotRunning = 1;
   msg->dataLength = 0;

   /* Send to dectctl application */
   if ((ret = cmsMsg_send( cliPrvtMsgHandle, msg )) != CMSRET_SUCCESS)
   {
      cmsLog_error("%s() msg send failed, ret=%d",__FUNCTION__, ret);
   }

   CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
   return ret;
}

#endif  /* DMP_X_BROADCOM_COM_DECTENDPOINT_1 */
#endif  /*  DMP_VOICE_SERVICE_2 */

#endif /* SUPPORT_CLI_CMD */

