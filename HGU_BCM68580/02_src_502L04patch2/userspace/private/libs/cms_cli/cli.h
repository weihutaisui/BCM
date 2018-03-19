/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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

#ifndef __CLI_H__
#define __CLI_H__


/*!\file cli.h
 * \brief internal header file for CLI library.
 */

#include "cms.h"
#include "cms_dal.h"


/** Max length of a single line of input */
#define CLI_MAX_BUF_SZ   4096

/** Number of seconds to sleep when max auth failures is reached. */
#define AUTH_FAIL_SLEEP  3

/** Permission bit for admin user */
#define PERM_ADMIN     0x80

/** Permission bit for support user */
#define PERM_SUPPORT   0x40

/** Permission bit for normal user (user) */
#define PERM_USER      0x01

/** Permission mask to indicate something that can be done by Admin only */
#define PERM1 (PERM_ADMIN)

/** Permission mask to indicate something that can be done by Admin or Support */
#define PERM2 (PERM_ADMIN|PERM_SUPPORT)

/** Permission mask to indicate something that can be done by Admin or Support or User */
#define PERM3 (PERM_ADMIN|PERM_SUPPORT|PERM_USER)

/** The amount of time, in seconds, of idle-ness before timing out the CLI session. */
extern UINT32 exitOnIdleTimeout;

/** Max length of menuPathBuf */
#define MAX_MENU_PATH_LENGTH   1024

/** Global string buffer that displays where we are in the menu hierarchy. */
extern char menuPathBuf[MAX_MENU_PATH_LENGTH];

/** Global boolean to decide whether CLI should keep running. */
extern UBOOL8 cli_keepLooping;

/** Global var indicating who is logged in right now. Max length 63. */
extern char currUser[];

/** Global var indicating current permission level associated with current user. */
extern UINT8 currPerm;

/** Global pointer to message handle passed in by caller */
extern void *cliPrvtMsgHandle;

/** Global pointer to config data from MDM via DAL. */
extern WEB_NTWK_VAR glbWebVar;

/** Global var indicating current port */
extern UINT16 currAppPort;

/** Global var indicating current app */
extern char currAppName[];

/** Global var indicating current IP connection */
extern char currIpAddr[];

/** used in PARAMS_SETTING in cli_menu */
typedef UBOOL8 (*CLI_VALIDATE_FUNC) (const char* inputParam);

/** this is only used in cli_menu */
typedef struct {
   char* prompt;
   char  param[CLI_MAX_BUF_SZ];
   CLI_VALIDATE_FUNC validateFnc;
} PARAMS_SETTING;

/** Global var used in cli_menu to tell post-func to save config */
extern UBOOL8 cliMenuSaveNeeded;

/** Global var used in cli_cmd to save config */
extern UBOOL8 cliCmdSaveNeeded;

/** Display the current menu.
 *
 */
extern void cli_displayMenu(void);


/** Check cmdLine against the menu driven table.
 *
 * @param cmdLine (IN) command line from user.
 *
 * @return TRUE if cmdLine was a menu driven item that was processed by
 * this function.
 */
extern UBOOL8 cli_processMenuItem(const char *cmdLine);


/** Check cmdLine against the CLI cmd table.
 *
 * @param cmdLine (IN) command line from user.
 *
 * @return TRUE if cmdLine was a CLI command that was processed by
 * this function.
 */
extern UBOOL8 cli_processCliCmd(const char *cmdLine);



/** Check cmdLine against the hidden cmd table.
 *
 * @param cmdLine (IN) command line from user.
 *
 * @return TRUE if cmdLine was a hidden command that was processed by
 * this function.
 */
extern UBOOL8 cli_processHiddenCmd(const char *cmdLine);


/** Wait for user input before continuing.
 *  This is used when menu driven CLI mode is enabled.
 */
void cli_waitForContinue(void);


/** Wait for the specified amount of time for input to be available on stdin.
 *  This function will also return if a signal was received.
 *
 *
 * @return CMSRET_SUCCESS if input becomes available.
 *         CMSRET_TIMED_OUT if user stops typing for the exit-on-idle number of seconds.
 *         CMSRET_OP_INTR if input was interrupted by a signal.
 */
CmsRet cli_waitForInputAvailable(void);

/** Read a line from standard input.
 *
 * Note this function calls cli_waitForInputAvailable() prior to calling the
 * read, which blocks.
 * 
 * @param buf (OUT) buffer to hold the text read.
 * @param size (IN) Size of the buffer.
 *
 * @return CMSRET_SUCCESS if input was read.
 *         CMSRET_TIMED_OUT if user stops typing for the exit-on-idle number of seconds.
 *         CMSRET_OP_INTR if input was interrupted by a signal.
 */
CmsRet cli_readString(char *buf, int size);


/** Return true if the specified state is valid.
 *  For menu cli, valid state is 1 for on/enabled, and 2 for off/disabled.
 *
 * @param state (IN) state string.
 *
 * @return TRUE if the state string is valid.
 */
UBOOL8 cli_isValidState(const char *state);


#ifdef BRCM_VOICE_SUPPORT
void processVoiceCtlCmd(char* cmdLine);
#  ifdef DMP_X_BROADCOM_COM_DECTENDPOINT_1 
void processDectCmd(char *cmdLine);
#  endif
#endif


/* functions in cli_cmd.c */
void processSwVersionCmd(char *cmdLine);

void cmdSyslogHelp(void);
void processSyslogCmd(char *cmdLine);
void processSyslogCmd_igd(char *cmdLine);

/* functions defined in cli_cmdwan.c */
void processWanCmd(char *cmdLine);
void processVirtualServerCmd(char *cmdLine);
void virtualServerShow(void);
void virtualServerShow_igd(void);
void virtualServerEnableDisable(UBOOL8 doEnable, UINT32 entryNum);
void virtualServerEnableDisable_igd(UBOOL8 doEnable, UINT32 entryNum);

/* functions defined in cli2_cmd.c */
void processSyslogCmd_dev2(char *cmdLine);
void virtualServerShow_dev2(void);
void virtualServerEnableDisable_dev2(UBOOL8 doEnable, UINT32 entryNum);

/* functions defined in cli_cmdcmf.c */
void processCmfCmd(char *cmdLine);

/* functions defined in cli_cmdomci.c */
void processOmciCmd(char *cmdLine);
void processOmcipmCmd(char *cmdLine);

/* functions defined in cli_cmdlaser.c */
void processLaserCmd(char *cmdLine);


/* functions in cli_cmddebug.c */
void processLogLevelCmd(char *cmdLine);
void processLogDestCmd(char *cmdLine);
void processDumpCfgCmd(char *cmdLine);
void processDumpMdmCmd(char *cmdLine);
void processDumpEidInfoCmd(char *cmdLine);
void processMeminfoCmd(char *cmdLine);
void processMdmCmd(char *cmdLine);
void processDataModelCmd(char *cmdLine);
void processDumpSysInfoCmd(char *cmdLine);
void processDnsproxyCmd(char *cmdLine);
void processExitOnIdleCmd(char *cmdLine);


/* functions defined in cli_osgid.c */
void processOsgidCmd(char *cmdLine);

/* functions defined in cli_menu.c */
CmsRet cli_getMenuParameters(PARAMS_SETTING *inParam, UBOOL8 showInstruction);

/* functions defined in cli_menuwan.c */
CmsRet menuWanCfg(void);
CmsRet menuWanDelete(void);
CmsRet menuWanShow(void);

/* functions defined in cli_util.c */
UBOOL8 cli_isMacAddress(char *addr);
UBOOL8 cli_isIpAddress(const char *addr);
UBOOL8 cli_isNumber(const char *buf);
UBOOL8 cli_isValidVpi(const char *vpi);
UBOOL8 cli_isValidVci(const char *vci);
UBOOL8 cli_isValidWanId(const char *id);
UBOOL8 cli_isValidVlanId(const char *id);
UBOOL8 cli_isValidIdleTimeout(const char *timeout);
UBOOL8 cli_isValidWanServiceName(const char *service);
UBOOL8 cli_isValidPppUserName(const char *username);
UBOOL8 cli_isValidPppPassword(const char *password);
UBOOL8 cli_isValidService(const char *service);
UBOOL8 cli_atmAddrToNum(const char *atmAddr, int *port, int *vpi, int *vci);


/** Check if there is enough queues for QoS.
 *
 * @param pInfo (IN) Pointer to the WEB_NTWK_VAR containing current settings.
 *
 * @return CMSRET_SUCCESS if there is enough resources.  CMSRET_RESOURCE_EXCEEDED otherwise.
 */
CmsRet cli_checkQosQueueResources(const PWEB_NTWK_VAR pInfo);


/** Check the existence of the given layer2 interface name.
 *
 * @param l2Name (IN) Layer2 interface name.
 *
 * @return TRUE if layer2 interface name exists.
 */
UBOOL8 cli_isValidL2IfName(const char *l2Name);
UBOOL8 cli_isValidL2IfName_igd(const char *l2Name);
UBOOL8 cli_isValidL2IfName_dev2(const char *l2Name);

#if defined(SUPPORT_DM_LEGACY98)
#define cli_isValidL2IfName(i)          cli_isValidL2IfName_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define cli_isValidL2IfName(i)          cli_isValidL2IfName_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define cli_isValidL2IfName(i)          cli_isValidL2IfName_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define cli_isValidL2IfName(i)   (cmsMdm_isDataModelDevice2() ? \
                                        cli_isValidL2IfName_dev2((i)) : \
                                        cli_isValidL2IfName_igd((i)))
#endif


/** Check if the portid/vpi/vci specified in the webVar exists in the data model.
 *
 * @param webVar (IN) Pointer to the WEB_NTWK_VAR containing current settings.
 *
 * @return TRUE if the portid/vpi/vci is found.
 */
UBOOL8 cli_isVccAddrExist(const WEB_NTWK_VAR *webVar);
UBOOL8 cli_isVccAddrExist_igd(const WEB_NTWK_VAR *webVar);
UBOOL8 cli_isVccAddrExist_dev2(const WEB_NTWK_VAR *webVar);

#if defined(SUPPORT_DM_LEGACY98)
#define cli_isVccAddrExist(i)          cli_isVccAddrExist_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define cli_isVccAddrExist(i)          cli_isVccAddrExist_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define cli_isVccAddrExist(i)          cli_isVccAddrExist_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define cli_isVccAddrExist(i)   (cmsMdm_isDataModelDevice2() ? \
                                       cli_isVccAddrExist_dev2((i)) : \
                                       cli_isVccAddrExist_igd((i)))
#endif


/** Show all the ATM and PTM interfaces configured on the system.
 *
 */
void cli_wanShowInterfaces(void);
void cli_wanShowInterfaces_igd(void);
void cli_wanShowInterfaces_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define cli_wanShowInterfaces()          cli_wanShowInterfaces_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define cli_wanShowInterfaces()          cli_wanShowInterfaces_igd()
#elif defined(SUPPORT_DM_PURE181)
#define cli_wanShowInterfaces()          cli_wanShowInterfaces_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define cli_wanShowInterfaces()   (cmsMdm_isDataModelDevice2() ? \
                                       cli_wanShowInterfaces_dev2() : \
                                       cli_wanShowInterfaces_igd())
#endif

/** Show the WAN services configured for all VC's or for a particular VC.
 *
 * @param specificVcc (IN)  If this param is NULL, than all WAN services for all VCC's will be
 *                          displayed.  If this param is not NULL, then only the wan services
 *                          defined for this VCC will be displayed.
 */
CmsRet cli_wanShowServices(const char *specificVcc);



/***************************************************************************
// Function Name: cli_wanIfaceInfo
// Description  : get wan interface information, it is modified from cgiGetWanInterfaceInfo
//Parameters   :
//    type : the type of wan interface
//    iface : the list of interface info
// Returns      :  none
****************************************************************************/
void cli_wanIfaceInfo(char *type, char *iface) ;


/* functions defined in cli_moca.c */

/** Process MoCA command line string.
 *
 * This function parses "moca" CLI commands and executes the appropriate 
 * MoCA functions as a result. 
 *
 * @param cmdLine (IN) String pointer to the command line arguments.
 * @return None.
 */
void processMocaCmd(char *cmdLine);

/** Process Software Version command line string.
 *
 * This function parses software version CLI commands and executes the appropriate 
 * functions as a result. 
 *
 * @param cmdLine (IN) String pointer to the command line arguments.
 * @return None.
 */
void processSwVersionCmd_igd(char *cmdLine);

void processSwVersionCmd_dev2(char *cmdLine);

/** Process Uptime command line string.
 *
 * This function Read the DeviceInfo object to retrieve upTime,
 * convert to string, and print its value to console. 
 *
 * @param None.
 * @return None.
 */
void processUptimeCmd_igd(void);

void processUptimeCmd_dev2(void);

#if defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1) 

#define STNP_SERVER_MAX         5

void processSntpCmd_igd(char *cmdLine);

void processSntpCmd_dev2(char *cmdLine);

#endif // defined(DMP_TIME_1) || defined(DMP_DEVICE2_TIME_1)


void processLanHostsCmd_igd(char *cmdLine);

void processLanHostsCmd_dev2(char *cmdLine);

void DslCfgProfileUpdate_igd(unsigned char lineId);
void DslCfgProfileUpdate_dev2(unsigned char lineId);
#if defined(SUPPORT_DM_LEGACY98)
#define DslCfgProfileUpdate(l)          DslCfgProfileUpdate_igd((l))
#elif defined(SUPPORT_DM_HYBRID)
#define DslCfgProfileUpdate(l)          DslCfgProfileUpdate_igd((l))
#elif defined(SUPPORT_DM_PURE181)
#define DslCfgProfileUpdate(l)          DslCfgProfileUpdate_dev2((l))
#elif defined(SUPPORT_DM_DETECT)
#define DslCfgProfileUpdate(l)          (cmsMdm_isDataModelDevice2() ? \
                                         DslCfgProfileUpdate_dev2((l)) :     \
                                         DslCfgProfileUpdate_igd((l)))
#endif

void DslIntfCfgMdmUpdate_igd(unsigned char lineId);
void DslIntfCfgMdmUpdate_dev2(unsigned char lineId);
#if defined(SUPPORT_DM_LEGACY98)
#define DslIntfCfgMdmUpdate(l)          DslIntfCfgMdmUpdate_igd((l))
#elif defined(SUPPORT_DM_HYBRID)
#define DslIntfCfgMdmUpdate(l)          DslIntfCfgMdmUpdate_igd((l))
#elif defined(SUPPORT_DM_PURE181)
#define DslIntfCfgMdmUpdate(l)          DslIntfCfgMdmUpdate_dev2((l))
#elif defined(SUPPORT_DM_DETECT)
#define DslIntfCfgMdmUpdate(l)          (cmsMdm_isDataModelDevice2() ? \
                                         DslIntfCfgMdmUpdate_dev2((l)) :     \
                                         DslIntfCfgMdmUpdate_igd((l)))
#endif




/** Configure the primary or secondary LAN side IP addr.
 *  Currently supports br0 only.
 *
 *
 */
CmsRet cliCmd_configLanIp(UBOOL8 isPrimary, const char *ip, const char *mask);
CmsRet cliCmd_configLanIp_igd(UBOOL8 isPrimary, const char *ip, const char *mask);
CmsRet cliCmd_configLanIp_dev2(UBOOL8 isPrimary, const char *ip, const char *mask);

#if defined(SUPPORT_DM_LEGACY98)
#define cliCmd_configLanIp(v, i, a)   cliCmd_configLanIp_igd((v), (i), (a))
#elif defined(SUPPORT_DM_HYBRID)
#define cliCmd_configLanIp(v, i, a)   cliCmd_configLanIp_igd((v), (i), (a))
#elif defined(SUPPORT_DM_PURE181)
#define cliCmd_configLanIp(v, i, a)   cliCmd_configLanIp_dev2((v), (i), (a))
#elif defined(SUPPORT_DM_DETECT)
#define cliCmd_configLanIp(v, i, a)   (cmsMdm_isDataModelDevice2() ? \
                 cliCmd_configLanIp_dev2((v), (i), (a)) : \
                 cliCmd_configLanIp_igd((v), (i), (a)))
#endif


/** Delete secondary IP addr (on br0 only).
 *
 */
CmsRet cliCmd_deleteLanIp(UBOOL8 isPrimary);
CmsRet cliCmd_deleteLanIp_igd(UBOOL8 isPrimary);
CmsRet cliCmd_deleteLanIp_dev2(UBOOL8 isPrimary);

#if defined(SUPPORT_DM_LEGACY98)
#define cliCmd_deleteLanIp(i)          cliCmd_deleteLanIp_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define cliCmd_deleteLanIp(i)          cliCmd_deleteLanIp_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define cliCmd_deleteLanIp(i)          cliCmd_deleteLanIp_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define cliCmd_deleteLanIp(i)   (cmsMdm_isDataModelDevice2() ? \
                                       cliCmd_deleteLanIp_dev2((i)) : \
                                       cliCmd_deleteLanIp_igd((i)))
#endif


/** Update the start address, end address,
 *  and leased time for the dhcp server
 *
 *
 */
CmsRet cliCmd_updateDhcpServer(const char *minAddress, const char *maxAddress, UINT32 leaseTime);
CmsRet cliCmd_updateDhcpServer_igd(const char *minAddress, const char *maxAddress, UINT32 leaseTime);
CmsRet cliCmd_updateDhcpServer_dev2(const char *minAddress, const char *maxAddress, UINT32 leaseTime);

#if defined(SUPPORT_DM_LEGACY98)
#define cliCmd_updateDhcpServer(v, i, a)   cliCmd_updateDhcpServer_igd((v), (i), (a))
#elif defined(SUPPORT_DM_HYBRID)
#define cliCmd_updateDhcpServer(v, i, a)   cliCmd_updateDhcpServer_igd((v), (i), (a))
#elif defined(SUPPORT_DM_PURE181)
#define cliCmd_updateDhcpServer(v, i, a)   cliCmd_updateDhcpServer_dev2((v), (i), (a))
#elif defined(SUPPORT_DM_DETECT)
#define cliCmd_updateDhcpServer(v, i, a)   (cmsMdm_isDataModelDevice2() ? \
                                            cliCmd_updateDhcpServer_dev2((v), (i), (a)) : \
                                            cliCmd_updateDhcpServer_igd((v), (i), (a)))
#endif


/** Show the dhcp server information.
 *
 */
CmsRet cliCmd_showDhcpServer(void);
CmsRet cliCmd_showDhcpServer_igd(void);
CmsRet cliCmd_showDhcpServer_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define cliCmd_showDhcpServer()          cliCmd_showDhcpServer_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define cliCmd_showDhcpServer()          cliCmd_showDhcpServer_igd()
#elif defined(SUPPORT_DM_PURE181)
#define cliCmd_showDhcpServer()          cliCmd_showDhcpServer_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define cliCmd_showDhcpServer()   (cmsMdm_isDataModelDevice2() ? \
                                    cliCmd_showDhcpServer_dev2() : \
                                    cliCmd_showDhcpServer_igd())
#endif


/** Enable or disable the dhcp server (on br0 only).
 *
 */
CmsRet cliCmd_configDhcpServer(UBOOL8 enable);
CmsRet cliCmd_configDhcpServer_igd(UBOOL8 enable);
CmsRet cliCmd_configDhcpServer_dev2(UBOOL8 enable);

#if defined(SUPPORT_DM_LEGACY98)
#define cliCmd_configDhcpServer(i)          cliCmd_configDhcpServer_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define cliCmd_configDhcpServer(i)          cliCmd_configDhcpServer_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define cliCmd_configDhcpServer(i)          cliCmd_configDhcpServer_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define cliCmd_configDhcpServer(i)   (cmsMdm_isDataModelDevice2() ? \
                                       cliCmd_configDhcpServer_dev2((i)) : \
                                       cliCmd_configDhcpServer_igd((i)))
#endif


/** Enable or disable the dhcp client (on br0 only).
 *
 */
CmsRet cliCmd_configDhcpClient(UBOOL8 enable);
CmsRet cliCmd_configDhcpClient_igd(UBOOL8 enable);
CmsRet cliCmd_configDhcpClient_dev2(UBOOL8 enable);

#if defined(SUPPORT_DM_LEGACY98)
#define cliCmd_configDhcpClient(i)          cliCmd_configDhcpClient_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define cliCmd_configDhcpClient(i)          cliCmd_configDhcpClient_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define cliCmd_configDhcpClient(i)          cliCmd_configDhcpClient_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define cliCmd_configDhcpClient(i)   (cmsMdm_isDataModelDevice2() ? \
                                       cliCmd_configDhcpClient_dev2((i)) : \
                                       cliCmd_configDhcpClient_igd((i)))
#endif



/** Find out if the ppp exist in the system
 *
 */
UBOOL8 isPPPExist(const char *pppName);
UBOOL8 isPPPExist_igd(const char *pppName);
UBOOL8 isPPPExist_dev2(const char *pppName);

#if defined(SUPPORT_DM_LEGACY98)
#define isPPPExist(i)          isPPPExist_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define isPPPExist(i)          isPPPExist_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define isPPPExist(i)          isPPPExist_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define isPPPExist(i)   (cmsMdm_isDataModelDevice2() ? \
                                     isPPPExist_dev2((i)) : \
                                     isPPPExist_igd((i)))
#endif

CmsRet cli_wanShowServicesSub(const char * specificVcc);
CmsRet cli_wanShowServicesSub_igd(const char * specificVcc);
CmsRet cli_wanShowServicesSub_dev2(const char * specificVcc);

#if defined(SUPPORT_DM_LEGACY98)
#define cli_wanShowServicesSub(i)          cli_wanShowServicesSub_igd((i))
#elif defined(SUPPORT_DM_HYBRID)
#define cli_wanShowServicesSub(i)          cli_wanShowServicesSub_igd((i))
#elif defined(SUPPORT_DM_PURE181)
#define cli_wanShowServicesSub(i)          cli_wanShowServicesSub_dev2((i))
#elif defined(SUPPORT_DM_DETECT)
#define cli_wanShowServicesSub(i)   (cmsMdm_isDataModelDevice2() ? \
                                       cli_wanShowServicesSub_dev2((i)) : \
                                       cli_wanShowServicesSub_igd((i)))
#endif


/* only for TR69 */
void cli_wanShowDslServices(InstanceIdStack * parentIidStack, const char * vccString, const char * linkType);

#ifdef DMP_X_BROADCOM_COM_TM_1
/* cli_qos.c. */
void processQoSCmd(char *cmdLine);
#endif


#endif  /* __CLI_H__ */
