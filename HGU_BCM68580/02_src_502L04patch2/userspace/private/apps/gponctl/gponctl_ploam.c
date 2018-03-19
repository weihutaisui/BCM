/*
* <:copyright-BRCM:2007-2010:proprietary:standard
* 
*    Copyright (c) 2007-2010 Broadcom 
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

/***************************************************************************
 * File Name  : gponctl_ploam.c
 * Description: Linux command line utility that controls the Broadcom
 *              BCM968xx PLOAM Driver.
 ***************************************************************************/

#include "bcm_OS_Deps_Usr.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "bcmtypes.h"

#include "gponctl.h"
#include "gponctl_api.h"

#define GPONCTL_MAC_SIZE             6
#define GPONCTL_SERIAL_NUMBER_SIZE   8
#define GPONCTL_PASSWORD_SIZE        10
#define GPONCTL_PLOAM_MSG_DATA_SIZE  10
#define GPONCTL_PLOAM_MSG_SIZE       13
#define GPONCTL_PLOAM_MSG_MAP_SIZE   8
#define MCAST_VLAN_ID_MAX            4095
#define GPONCTL_USER_PLOAM_FILE_NAME "/dev/bcm_user_ploam"
#define GPONCTL_OMCI_FILE_NAME       "/dev/bcm_omci"

#define MAX_NUM_DATA_ITEMS_IN_OMCI_INPUT_STRING 512

int  helpHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  startHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  stopHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getStateHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getSnPwdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  setSnPwdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getVersionHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getEventHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  maskEventHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getAlarmHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  maskAlarmHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getSfSdThresholdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  setSfSdThresholdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  transmitMsgHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getMsgCountersHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getGtcCountersHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getGemPortCountersHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  configGemPortHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  configDsGemPortEncryptionByIX(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  configDsGemPortEncryptionByID(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  configGemPortQosHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  enableGemPortHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getGemPortHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getOmciPortHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getAllocIdsHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getOnuIdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getFecModeHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getEncryptionKeyHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  generatePrbsHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  transmitOmciMsgHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getOmciCountersHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getOmciVersionHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  setTO1TO2Handler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getTO1TO2Handler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  getTcontCfgHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  cfgTcontAllocIdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  cfgGemBlkLenHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int  cfgTodInfoHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);
int deconfigGemPortHandler(PGPON_OPTION_INFO pOptions, int nNumOptions);

char g_gponctl_PgmName[128] = {0};

/*** File Scoped Globals. ***/
GPON_COMMAND_INFO g_gponctl_Cmds[] =
{
    {"--help",      {""}, helpHandler},
    {"start",       {"--oper", ""}, startHandler},
    {"stop",        {"--gasp", ""}, stopHandler},
    {"setTO1TO2",   {"--to1", "--to2", ""}, setTO1TO2Handler},
    {"getTO1TO2",   {""}, getTO1TO2Handler},
    {"getState",    {""}, getStateHandler},
    {"getSnPwd",    {""}, getSnPwdHandler},
    {"setSnPwd",    {"--sn", "--pwd",
                     ""}, setSnPwdHandler},
    {"getVersion",  {""}, getVersionHandler},
    {"getEvent",    {""}, getEventHandler},
    {"maskEvent",   {"--state", "--alarm",
                     "--port", "--rxo",
                     "--txo", "--gem", ""},               maskEventHandler},
    {"getAlarm",    {""}, getAlarmHandler},
    {"maskAlarm",   {"--los", "--lof", "--sf",
                     "--sd", "--lcdg", "--suf",
                     "--mem", "--dact", "--dis",
                     "--lol", "--apc", "--pee",
                     ""},                        maskAlarmHandler},
    {"getSfSdThreshold",   {""}, getSfSdThresholdHandler},
    {"setSfSdThreshold",   {"--sf", "--sd", ""}, setSfSdThresholdHandler},
    {"transmitMsg",   {"--id", "--data", "--block",
                       ""},                      transmitMsgHandler},
    {"getMsgCounters",     {"--reset", ""}, getMsgCountersHandler},
    {"getGtcCounters",     {"--reset", ""}, getGtcCountersHandler},
    {"getGemPortCounters", {"--reset", "--index", "--id",
                            ""},            getGemPortCountersHandler},
    {"configGemPort",      {"--index", "--id", "--alloc", "--usQueue", "--mcast",
                            ""},            configGemPortHandler},
    {"configDsGemPortEncryByIx",      {"--index", "--encryption",
                            ""},            configDsGemPortEncryptionByIX},
    {"configDsGemPortEncryById",      {"--gemPortId", "--encryption",
                            ""},            configDsGemPortEncryptionByID},

    {"deconfigGemPort",    {"--index", "--id",
                            ""},            deconfigGemPortHandler},
    {"enableGemPort",      {"--index", "--id", "--enable",
                            ""},            enableGemPortHandler},
    {"getGemPort",         {"--index", "--id",
                            ""},            getGemPortHandler},
    {"getOmciPort",        {""},            getOmciPortHandler},
    {"getAllocIds",        {""},            getAllocIdsHandler},
    {"getTcontCfg",   {"--tcontIdx", ""},  getTcontCfgHandler},
    {"configTcontAllocId",   {"--tcontIdx", "--allocId", ""},  cfgTcontAllocIdHandler},
    {"configGemBlkLen",   {"--len", ""},  cfgGemBlkLenHandler},
    {"configTod",   {"--enable", "--usrEvent","--pulseWidth","--sf", "--timeNs", 
                     "--timeSecMSB", "--timeSecLSB", ""},  cfgTodInfoHandler},
    {"getOnuId",           {""},            getOnuIdHandler},
    {"getFecMode",         {""},            getFecModeHandler},
    {"getEncryptionKey",   {""},            getEncryptionKeyHandler},
    {"transmitOmciMsg",    {"--len", "--data", "--block",
                            ""},            transmitOmciMsgHandler},
    {"getOmciCounters",    {"--reset", ""}, getOmciCountersHandler},
    {"getOmciVersion",     {""},            getOmciVersionHandler},
    {"generatePrbs",       {"--enable", "--mode",
                            ""},            generatePrbsHandler},
    {""}
};

/*
 *------------------------------------------------------------------------------
 * Function Name: usage
 * Description  : Displays the GPON control utility usage
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
void usage(void)
{
    printf("\n\nGPON Control Utility:\n\n");
    printf("Proc FileSystem: /proc/ploam /proc/omci\n\n");
    printf("::: Usage:\n\n");
    printf("       %s --help\n\n", g_gponctl_PgmName);

    printf(":::::: System ::::::\n");
    printf("       %s start\n", g_gponctl_PgmName);
    printf("                    [ --oper <1..8> ]\n");
    printf("       %s stop\n", g_gponctl_PgmName);
    printf("                    [ --gasp <0=FALSE|1=TRUE> ]\n");
    printf("       %s setTO1TO2 --to1 <ms> --to2 <ms>\n", g_gponctl_PgmName);
    printf("       %s getTO1TO2\n", g_gponctl_PgmName);
    printf("       %s getState\n", g_gponctl_PgmName);
    printf("       %s getSnPwd\n", g_gponctl_PgmName);
    printf("       %s setSnPwd\n", g_gponctl_PgmName);
    printf("                    [ --sn  <xx-xx-xx-xx-xx-xx-xx-xx>\n");
    printf("                      --pwd <xx-xx-xx-xx-xx-xx-xx-xx-xx-xx> ]\n");
    printf("       %s getVersion\n", g_gponctl_PgmName);

    printf(":::::: Event ::::::\n");
    printf("       %s getEvent\n", g_gponctl_PgmName);
    printf("       %s maskEvent\n", g_gponctl_PgmName);
    printf("                    [ --state <0=OFF|1=ON> --alarm <0=OFF|1=ON>\n");
    printf("                      --port <0=OFF|1=ON> --rxo <0=OFF|1=ON>\n");
    printf("                      --txo <0=OFF|1=ON> --gem <0=OFF|1=ON>]\n\n");

    printf(":::::: Alarm ::::::\n");
    printf("       %s getAlarm\n", g_gponctl_PgmName);
    printf("       %s maskAlarm\n", g_gponctl_PgmName);
    printf("                    [ --los <0=OFF|1=ON> --lof <0=OFF|1=ON>\n");
    printf("                      --sf <0=OFF|1=ON> --sd <0=OFF|1=ON>\n");
    printf("                      --lcdg <0=OFF|1=ON> --suf <0=OFF|1=ON>\n");
    printf("                      --mem <0=OFF|1=ON> --dact <0=OFF|1=ON>\n");
    printf("                      --dis <0=OFF|1=ON> --lol <0=OFF|1=ON>\n");
    printf("                      --apc <0=OFF|1=ON> --pee <0=OFF|1=ON> ]\n");
    printf("       %s getSfSdThreshold\n", g_gponctl_PgmName);
    printf("       %s setSfSdThreshold\n", g_gponctl_PgmName);
    printf("                    [ --sf <3..8> --sd <sf+1..9> ]\n");

    printf(":::::: Message ::::::\n");
    printf("       %s transmitMsg\n", g_gponctl_PgmName);
    printf("                    [ --id <0..255>\n");
    printf("                      --data <xx-xx-..-xx (up to 10)> ]\n");
    printf("                      --block (blocking bcm_dev_write) ]\n");

    printf(":::::: Counters ::::::\n");
    printf("       %s getMsgCounters\n", g_gponctl_PgmName);
    printf("                    [ --reset <0=FALSE|1=TRUE> ]\n");
    printf("       %s getGtcCounters\n", g_gponctl_PgmName);
    printf("                    [ --reset <0=FALSE|1=TRUE> ]\n");
    printf("       %s getGemPortCounters\n", g_gponctl_PgmName);
    printf("                    [ --reset <0=FALSE|1=TRUE>\n");
    printf("                      --index <0..255|BCM_PLOAM_GEM_PORT_IDX_ALL(0xfffe)|BCM_PLOAM_GEM_PORT_IDX_ETH_ALL(0xfffd)>\n");
    printf("                      --id <0..BCM_PLOAM_MAX_XGEM_ID(0x%x)> ]\n\n", BCM_PLOAM_MAX_XGEM_ID);

    printf(":::::: GEM Port Provisioning ::::::\n");
    printf("       %s configGemPort\n", g_gponctl_PgmName);
    printf("                    [ --index <0..31|BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED(0xffff)>\n");
    printf("                      --id <0..BCM_PLOAM_MAX_XGEM_ID(0x%x)>\n", BCM_PLOAM_MAX_XGEM_ID);
    printf("                      --alloc <0..BCM_PLOAM_MAX_XALLOC_ID(0x%x)|BCM_PLOAM_ALLOC_ID_UNASSIGNED(0xffff)> ]\n", BCM_PLOAM_MAX_XALLOC_ID);
    printf("       %s deconfigGemPort\n", g_gponctl_PgmName);
    printf("                    [ --index <0..31|BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED(0xffff)|\n");
    printf("                               BCM_PLOAM_GEM_PORT_IDX_ALL(0xfffe)>\n");
    printf("                      --id <0..BCM_PLOAM_MAX_XGEM_ID(0x%x)> ]\n", BCM_PLOAM_MAX_XGEM_ID);
    printf("       %s enableGemPort\n", g_gponctl_PgmName);
    printf("                    [ --index <0..31|BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED(0xffff)>\n");
    printf("                      --id <0..BCM_PLOAM_MAX_XGEM_ID(0x%x)>\n", BCM_PLOAM_MAX_XGEM_ID);
    printf("                      --enable <0=FALSE|1=TRUE> ]\n");
    printf("       %s getGemPort\n", g_gponctl_PgmName);
    printf("                    [ --index <0..31|BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED(0xffff)>\n");
    printf("                      --id <0..BCM_PLOAM_MAX_XGEM_ID(0x%x)> ]\n", BCM_PLOAM_MAX_XGEM_ID);
    printf("       %s getOmciPort\n", g_gponctl_PgmName);
    printf("       %s getAllocIds\n\n", g_gponctl_PgmName);
    printf("       %s configDsGemPortEncryByIx\n", g_gponctl_PgmName);
    printf("                    [ --index <0..(BCM_PLOAM_NUM_DATA_GEM_PORTS-1)|BCM_PLOAM_GEM_PORT_IDX_ALL(0xfffe)>\n");
    printf("                      --encryption <0..1> ]\n");
    printf("       %s configDsGemPortEncryById\n", g_gponctl_PgmName);
    printf("                    [ --gemPortId <0..BCM_PLOAM_MAX_XGEM_ID(0x%x)>\n", BCM_PLOAM_MAX_XGEM_ID);
    printf("                      --encryption <0..1> ]\n");
    printf("       %s configTcontAllocId\n", g_gponctl_PgmName);
    printf("                    [--tcontIdx <0..%d>]\n",BCM_PLOAM_NUM_DATA_TCONTS-1);
    printf("                     --allocId <0..BCM_PLOAM_MAX_XALLOC_ID(0x%x)|BCM_PLOAM_ALLOC_ID_UNASSIGNED(0xffff)> ]\n", BCM_PLOAM_MAX_XALLOC_ID);
    printf("       %s configGemBlkLen\n", g_gponctl_PgmName);
    printf("                    [--len <Bytes>]\n");
    printf("       %s configTod\n", g_gponctl_PgmName);
    printf("                    [--enable <0=FALSE|1=TRUE>]\n");
    printf("                    [--usrEvent <0=FALSE|1=TRUE>]\n");
    printf("                    [--pulseWidth <0...255>]\n");
    printf("                    [--sf <0...0x3FFFFFFF>]\n");
    printf("                    [--timeNs <0...0xFFFFFFFF>]\n");
    printf("                    [--timeSecMSB <0...0xFFFFF>]\n");
    printf("                    [--timeSecLSB <0...0xFFFFFFFF>]\n");
    printf("       %s getTcontCfg\n", g_gponctl_PgmName);
    printf("                    [--tcontIdx <0..%d>]\n",BCM_PLOAM_NUM_DATA_TCONTS-1);
    printf(":::::: GTC parameters ::::::\n");
    printf("       %s getOnuId\n", g_gponctl_PgmName);
    printf("       %s getFecMode\n", g_gponctl_PgmName);
    printf("       %s getEncryptionKey\n", g_gponctl_PgmName);
    printf("       %s generatePrbs\n", g_gponctl_PgmName);
    printf("                    [ --enable <0=FALSE|1=TRUE>\n");
    printf("                      --mode  <0=PRBS7|1=PRBS15|2=PRBS23|3=PRBS31|4=SERDES_WRAPPER_PSEUDORANDOM> ]\n\n");
    printf(":::::: OMCI ::::::\n");
    printf("       %s transmitOmciMsg\n", g_gponctl_PgmName);
    printf("                    [ --len <1..2040>\n");
    printf("                      --data <xx-xx-..-xx (up to len or 512, whichever comes first.\n");
    printf("                              If len>512, byte pattern 00-01-02-... is used for \n");
    printf("                              data items 512 and up.)>\n");
    printf("                      --block (blocking bcm_dev_write) ]\n");
    printf("       %s getOmciCounters\n", g_gponctl_PgmName);
    printf("                    [ --reset <0=FALSE|1=TRUE> ]\n");
    printf("       %s getOmciVersion\n\n", g_gponctl_PgmName);


} /* usage */

/***************************************************************************
// Function Name: isHexNumber
// Description  : validate hex number from string.
// Parameters   : buf - hex number.
// Returns      : 0 - invalid hex number.
//                1 - valid hex number.
****************************************************************************/
static UINT8 isHexNumber(char* buf) {
    UINT8 ret = 0, i = 0, size = 0;

    if (buf == NULL) return ret;

    size = strlen(buf);

    for (i = 0; i < size; i++)
        if (isxdigit(buf[i]) == 0)
            break;

    if (size > 0 && i == size)
        ret = 1;

    return ret;
}


/***************************************************************************
// Function Name: snStrToNum
// Description  : convert serial number string to the serial number.
// Parameters   : input - serial number string has the following format:
//                xx-xx-xx-xx-xx-xx-xx-xx where x is hex number.
//                sn - serial number is array 0f GPONCTL_SERIAL_NUMBER_SIZE UINT8.
// Returns      : 0 - success.
//                1 - failed.
****************************************************************************/
static UINT8 snStrToNum(char* input, UINT8 *sn)
{
    UINT8 ret = 1, i = 0;
    char *pToken = NULL;
    char *pLast = NULL;
    char buf[] = "xx-xx-xx-xx-xx-xx-xx-xx";

    /* need to copy since strtok_r updates string */
    strncpy(buf, input, sizeof(buf));

    /* serial number string has the following format
        xx-xx-xx-xx-xx-xx-xx-xx where x is hex number */
    pToken = strtok_r(buf, "-", &pLast);
    if (pToken != NULL &&
        isHexNumber(pToken) &&
        strlen(pToken) <= 2)
    {
        sn[0] = (UINT8) strtol(pToken, (char **)NULL, 16);
        for (i = 1; i < GPONCTL_SERIAL_NUMBER_SIZE; i++)
        {
            pToken = strtok_r(NULL, "-", &pLast);
            if (pToken == NULL ||
                isHexNumber(pToken) == 0 ||
                strlen(pToken) > 2)
                break;
            sn[i] = (UINT8) strtol(pToken, (char **)NULL, 16);
        }
        if (i == GPONCTL_SERIAL_NUMBER_SIZE)
            ret = 0;
    }

    return ret;
}

/***************************************************************************
// Function Name: pwdStrToNum
// Description  : convert password string to the password.
// Parameters   : input - password string has the following format:
//                xx-xx-xx-xx-xx-xx-xx-xx-xx-xx where x is hex number.
//                pwd - password is array 0f GPONCTL_PASSWORD_SIZE UINT8.
// Returns      : 0 - success.
//                1 - failed.
****************************************************************************/
static UINT8 pwdStrToNum(char* input, UINT8 *pwd)
{
    UINT8 ret = 1, i = 0;
    char *pToken = NULL;
    char *pLast = NULL;
    char buf[] = "xx-xx-xx-xx-xx-xx-xx-xx-xx-xx";

    /* need to copy since strtok_r updates string */
    strncpy(buf, input, sizeof(buf));

    /*  password string has the following format
        xx-xx-xx-xx-xx-xx-xx-xx-xx-xx where x is hex number */
    pToken = strtok_r(buf, "-", &pLast);
    if (pToken != NULL &&
        isHexNumber(pToken) &&
        strlen(pToken) <= 2)
    {
        pwd[0] = (UINT8) strtol(pToken, (char **)NULL, 16);
        for (i = 1; i < GPONCTL_PASSWORD_SIZE; i++)
        {
            pToken = strtok_r(NULL, "-", &pLast);
            if (pToken == NULL ||
                isHexNumber(pToken) == 0 ||
                strlen(pToken) > 2)
                break;
            pwd[i] = (UINT8) strtol(pToken, (char **)NULL, 16);
        }
        if (i == GPONCTL_PASSWORD_SIZE)
            ret = 0;
    }

    return ret;
}

/***************************************************************************
// Function Name: ploamDataStrToNum
// Description  : convert PLOAM data string to the PLOAM data number.
// Parameters   : input - data string has the following format:
//                xx-xx-..-xx (lesser or up to GPONCTL_PLOAM_MSG_DATA_SIZE hex number)
//                where x is hex number.
//                data - data number is array 0f GPONCTL_PLOAM_MSG_DATA_SIZE UINT8.
// Returns      : 0 - success.
//                1 - failed.
****************************************************************************/
static UINT8 ploamDataStrToNum(char* input, UINT8 *data)
{
    UINT8 ret = 1, i = 0;
    char *pToken = NULL;
    char *pLast = NULL;
    char buf[] = "xx-xx-xx-xx-xx-xx-xx-xx-xx-xx";

    /* need to copy since strtok_r updates string */
    strncpy(buf, input, sizeof(buf));

    /* data string has the following format
       xx-xx-..-xx (less or up to GPONCTL_PLOAM_MSG_DATA_SIZE hex number)
       where x is hex number */
    pToken = strtok_r(buf, "-", &pLast);
    if (pToken != NULL &&
        isHexNumber(pToken) &&
        strlen(pToken) <= 2)
    {
        data[0] = (UINT8) strtol(pToken, (char **)NULL, 16);
        for (i = 1; i < GPONCTL_PLOAM_MSG_DATA_SIZE; i++)
        {
            pToken = strtok_r(NULL, "-", &pLast);
            if (pToken == NULL ||
                isHexNumber(pToken) == 0 ||
                strlen(pToken) > 2)
                break;
            data[i] = (UINT8) strtol(pToken, (char **)NULL, 16);
        }
        if (i <= GPONCTL_PLOAM_MSG_DATA_SIZE)
            ret = 0;
    }

    return ret;
}

/***************************************************************************
// Function Name: omciDataStrToNum
// Description  : convert OMCI data string to the OMCI data number.
// Parameters   : input - data string has the following format:
//                xx-xx-..-xx (lesser or up to length hex number)
//                where x is hex number and length <= BCM_OMCI_TX_MSG_MAX_SIZE_BYTES.
//                data - data number is array 0f BCM_OMCI_TX_MSG_MAX_SIZE_BYTES UINT8.
//                length - size of UINT8 array data number
// Returns      : 0 - success.
//                1 - failed.
****************************************************************************/
static UINT8 omciDataStrToNum(char* input, UINT8 *data, int length)
{
    UINT8 ret = 1;
    int   i = 0;
    int   len = strlen(input) + 1;
    char *pToken = NULL;
    char *pLast = NULL;
    char buf[len];

    /* need to copy since strtok_r updates string */
    strncpy(buf, input, len);

    /* data string has the following format
       xx-xx-..-xx (less or up to BCM_OMCI_TX_MSG_MAX_SIZE_BYTES hex number)
       where x is hex number */
    pToken = strtok_r(buf, "-", &pLast);
    if (pToken != NULL &&
        isHexNumber(pToken) &&
        strlen(pToken) <= 2)
    {
        data[0] = (UINT8) strtol(pToken, (char **)NULL, 16);

        for (i = 1; (i < length) && (i < MAX_NUM_DATA_ITEMS_IN_OMCI_INPUT_STRING); i++)
        {
            pToken = strtok_r(NULL, "-", &pLast);
            if (pToken == NULL ||
                isHexNumber(pToken) == 0 ||
                strlen(pToken) > 2)
                break;
            data[i] = (UINT8) strtol(pToken, (char **)NULL, 16);
        }
        if (i <= length && i <= MAX_NUM_DATA_ITEMS_IN_OMCI_INPUT_STRING)
            ret = 0;
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: helpHandler
 * Description  : Displays the GPON control utility usage
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int helpHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    usage();
    return 0;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: startHandler
 * Description  : Processes the BCM_PLOAM_IOC_START command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int startHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_Ploam_StartInfo info;

    // initialize neccessary fields
    info.initOperState = BCM_PLOAM_OSTATE_INITIAL_O1;

    if (pOptions->pOptName == NULL)
        goto out;

    if (!strcasecmp(pOptions->pOptName, "--oper"))
    {
        if (pOptions->nNumParms != 1)
        {
            fprintf(stderr,
                    "%s: identify operational state <%d..%d>\n",
                    g_gponctl_PgmName, BCM_PLOAM_OSTATE_INITIAL_O1,
                    (BCM_PLOAM_OSTATE_MAX-1));
            return GPONCTL_INVALID_OPTION;
        }
        info.initOperState = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
        if (info.initOperState < BCM_PLOAM_OSTATE_INITIAL_O1 ||
            info.initOperState > BCM_PLOAM_OSTATE_MAX-1)
        {
            fprintf(stderr,
                    "%s: invalid operational state %d\n",
                    g_gponctl_PgmName, info.initOperState);
            return GPONCTL_INVALID_PARAMETER;
        }
#endif
    }
    else    /* should never reach here */
    {
        fprintf(stderr, "%s: invalid option [%s]\n",
                g_gponctl_PgmName, pOptions->pOptName);
        return GPONCTL_INVALID_OPTION;
    }

out:
    ret = gponCtl_startAdminState(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        switch (info.initOperState)
        {
            case BCM_PLOAM_OSTATE_INITIAL_O1:
                printf("\n   Start ONU with operational state: INIT (0%d)\n\n", info.initOperState);
                break;
            case BCM_PLOAM_OSTATE_STANDBY_O2:
                printf("\n   Start ONU with operational state: STANDBY (0%d)\n\n", info.initOperState);
                break;
            case BCM_PLOAM_OSTATE_SERIAL_NUMBER_O3:
                printf("\n   Start ONU with operational state: SERIAL NUMBER (0%d)\n\n", info.initOperState);
                break;
            case BCM_PLOAM_OSTATE_RANGING_O4:
                printf("\n   Start ONU with operational state: RANGING (0%d)\n\n", info.initOperState);
                break;
            case BCM_PLOAM_OSTATE_OPERATION_O5:
                printf("\n   Start ONU with operational state: OPERATION (0%d)\n\n", info.initOperState);
                break;
            case BCM_PLOAM_OSTATE_POPUP_O6:
                printf("\n   Start ONU with operational state: POPUP (0%d)\n\n", info.initOperState);
                break;
            case BCM_PLOAM_OSTATE_EMERGENCY_STOP_O7:
                printf("\n   Start ONU with operational state: EMERGENCY STOP (0%d)\n\n", info.initOperState);
                break;
            case BCM_PLOAM_OSTATE_DEACTIVATED_O8:
                printf("\n   Start ONU with operational state: DEACTIVATE (0%d)\n\n", info.initOperState);
                break;
#if defined(CONFIG_BCM_GPON_AE_AUTO_SWITCH)
            case BCM_PLOAM_OSTATE_AE_INITIAL_O1:
                printf("\n   Start ONU with operational state: AE INIT (0%d)\n\n", info.initOperState);
                break;
            case BCM_PLOAM_OSTATE_AE_OPERATION_O5:
                printf("\n   Start ONU with operational state: AE_OPERATION (0%d)\n\n", info.initOperState);
                break;
#endif /* CONFIG_BCM_GPON_AE_AUTO_SWITCH */
            default:
                printf("\n   INVALID State : (0%d)\n\n", info.initOperState);
                break;
        }
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: stopHandler
 * Description  : Processes the BCM_PLOAM_IOC_STOP command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int stopHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_Ploam_StopInfo info;

    // initialize neccessary fields
    info.sendDyingGasp = 0;

    if (pOptions->pOptName == NULL)
        goto out;

    if (!strcasecmp(pOptions->pOptName, "--gasp"))
    {
        if (pOptions->nNumParms != 1)
        {
            fprintf(stderr,
                    "%s: identify sending dying gasp <0..1>\n",
                    g_gponctl_PgmName);
            return GPONCTL_INVALID_OPTION;
        }
        info.sendDyingGasp = strtoul(pOptions->pParms[0], NULL, 0);
        if (info.sendDyingGasp > 1)
        {
            fprintf(stderr,
                    "%s: invalid sending dying gasp %d\n",
                    g_gponctl_PgmName, info.sendDyingGasp);
            return GPONCTL_INVALID_PARAMETER;
        }
    }
    else    /* should never reach here */
    {
        fprintf(stderr, "%s: invalid option [%s]\n",
                g_gponctl_PgmName, pOptions->pOptName);
        return GPONCTL_INVALID_OPTION;
    }

out:
    ret = gponCtl_stopAdminState(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        if (info.sendDyingGasp)
             printf("\n   Stop ONU with sending dying gasp messages\n\n");
        else
            printf("\n   Stop ONU without sending dying gasp messages\n\n");
    }

    return ret;
}

int setTO1TO2Handler(PGPON_OPTION_INFO pOptions, int nNumOptions) {
  int ret = 0;
  BCM_Ploam_TO1TO2Info info;

  // initialize neccessary fields
  info.to1 = 0;
  info.to2 = 0;

  if (nNumOptions < 2)
    return GPONCTL_INVALID_NUMBER_OF_OPTIONS;

  while (nNumOptions)
  {
    if (!strcasecmp(pOptions->pOptName, "--to1"))
    {
      if (pOptions->nNumParms != 1)
      {
          fprintf(stderr,
                  "%s: specify TO1 timeout value\n",
                  g_gponctl_PgmName);
          return GPONCTL_INVALID_OPTION;
      }
      info.to1 = strtoul(pOptions->pParms[0], NULL, 0);
    }
    else if (!strcasecmp(pOptions->pOptName, "--to2"))
    {
      if (pOptions->nNumParms != 1)
      {
          fprintf(stderr,
                  "%s: specify TO2 timeout value\n",
                  g_gponctl_PgmName);
          return GPONCTL_INVALID_OPTION;
      }
      info.to2 = strtoul(pOptions->pParms[0], NULL, 0);
    }
    else    /* should never reach here */
    {
      fprintf(stderr, "%s: invalid option [%s]\n",
              g_gponctl_PgmName, pOptions->pOptName);
      return GPONCTL_INVALID_OPTION;
    }

    nNumOptions--;
    pOptions++;
  }

  ret = gponCtl_setTO1TO2(&info);

  if (ret != GPONCTL_SUCCESS)
      goto out;

  printf("\n======== TO1 & TO2 ========\n\n");
  printf("   TO1: %u ms\n", info.to1);
  printf("   TO2: %u ms\n", info.to2);
  printf("\n===========================\n\n");

out:
  return ret;
}

int  getTO1TO2Handler(PGPON_OPTION_INFO pOptions, int nNumOptions) {
  int ret = 0;
  BCM_Ploam_TO1TO2Info info;

  // initialize neccessary fields
  info.to1 = 0;
  info.to2 = 0;

  ret = gponCtl_getTO1TO2(&info);

  if (ret != GPONCTL_SUCCESS)
      goto out;

  printf("\n======== TO1 & TO2 ========\n\n");
  printf("   TO1: %u ms\n", info.to1);
  printf("   TO2: %u ms\n", info.to2);
  printf("\n===========================\n\n");

out:
  return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getStateHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_ALARM_STATUS command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getStateHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_StateInfo info;

    int ret = gponCtl_getControlStates(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n======== Control State Status ========\n\n");
        switch (info.adminState)
        {
            case BCM_PLOAM_ASTATE_OFF:
                printf("   Administrative state: OFF\n");
                break;
            case BCM_PLOAM_ASTATE_ON:
                printf("   Administrative state: ON\n");
                break;
            default:
                break;
        }
        switch (info.operState)
        {
            case BCM_PLOAM_OSTATE_INITIAL_O1:
                printf("   Operational    state: INIT (O%d)\n", info.operState);
                break;
            case BCM_PLOAM_OSTATE_STANDBY_O2:
                printf("   Operational    state: STANDBY (O%d)\n", info.operState);
                break;
            case BCM_PLOAM_OSTATE_SERIAL_NUMBER_O3:
                printf("   Operational    state: SERIAL NUMBER (O%d)\n", info.operState);
                break;
            case BCM_PLOAM_OSTATE_RANGING_O4:
                printf("   Operational    state: RANGING (O%d)\n", info.operState);
                break;
            case BCM_PLOAM_OSTATE_OPERATION_O5:
                printf("   Operational    state: OPERATION (O%d)\n", info.operState);
                break;
            case BCM_PLOAM_OSTATE_POPUP_O6:
                printf("   Operational    state: POPUP (O%d)\n", info.operState);
                break;
            case BCM_PLOAM_OSTATE_EMERGENCY_STOP_O7:
                printf("   Operational    state: EMERGENCY STOP (O%d)\n", info.operState);
                break;
            case BCM_PLOAM_OSTATE_DEACTIVATED_O8:
                printf("   Operational    state: DEACTIVATE (O%d)\n", info.operState);
                break;
#if defined(CONFIG_BCM_GPON_AE_AUTO_SWITCH)
            case BCM_PLOAM_OSTATE_AE_INITIAL_O1:
                printf("   Operational    state: AE_INIT (O%d)\n", info.operState);
                break;
            case BCM_PLOAM_OSTATE_AE_OPERATION_O5:
                printf("   Operational    state: AE OPERATION (O%d)\n", info.operState);
                break;
#endif /* CONFIG_BCM_GPON_AE_AUTO_SWITCH */
            default:
                printf("\n   INVALID State : (0%d)\n\n", info.operState);
                break;
        }
        printf("\n======================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getSnPwdHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_SERIAL_PASSWD command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getSnPwdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_SerialPasswdInfo info;
    int i = 0;
    int ret = gponCtl_getSerialPasswd(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n======== Serial Number & Password ========\n\n");
        printf("   Serial Number: ");
        for (i = 0; i < 7; i++)
            printf("%02X-", info.serialNumber[i]);
        printf("%02X\n", info.serialNumber[7]);
        printf("   Password     : ");
        for (i = 0; i < 9; i++)
            printf("%02X-", info.password[i]);
        printf("%02X\n", info.password[9]);
        printf("\n==========================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: setSnPwdHandler
 * Description  : Processes the BCM_PLOAM_IOC_SET_SERIAL_PASSWD command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int setSnPwdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, i = 0;
    BCM_Ploam_SerialPasswdInfo info;

    ret = gponCtl_getSerialPasswd(&info);
    if (ret != GPONCTL_SUCCESS)
        goto out;

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--sn"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify serial number\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            if (snStrToNum(pOptions->pParms[0], info.serialNumber))
            {
                fprintf(stderr,
                        "%s: invalid serial number: %s\n",
                        g_gponctl_PgmName, pOptions->pParms[0]);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--pwd"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify password\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            if (pwdStrToNum(pOptions->pParms[0], info.password))
            {
                fprintf(stderr,
                        "%s: invalid password: %s\n",
                        g_gponctl_PgmName, pOptions->pParms[0]);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    ret = gponCtl_setSerialPasswd(&info);
    if (ret != GPONCTL_SUCCESS)
        goto out;

    printf("\n======== Serial Number & Password ========\n\n");
    printf("   Serial Number: ");
    for (i = 0; i < 7; i++)
        printf("%02X-", info.serialNumber[i]);
    printf("%02X\n", info.serialNumber[7]);
    printf("   Password     : ");
    for (i = 0; i < 9; i++)
        printf("%02X-", info.password[i]);
    printf("%02X\n", info.password[9]);
    printf("\n==========================================\n\n");

out:
    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getVersionHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_DRIVER_VERSION command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getVersionHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Gpon_DriverVersionInfo info;
    int ret = gponCtl_getPloamDriverVersion(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n======== PLOAM Driver Version ========\n\n");
        printf("   Driver major: %d\n", info.driverMajor);
        printf("   Driver minor: %d\n", info.driverMinor);
        printf("   Driver fix  : %d\n", info.driverFix);
        printf("   API    major: %d\n", info.apiMajor);
        printf("   API    minor: %d\n", info.apiMinor);
        printf("\n======================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getEventHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_EVENT_STATUS command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getEventHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_EventStatusInfo eventStatusInfo;
    int ret = gponCtl_getEventStatus(&eventStatusInfo);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n========= PLOAM Event Status ========\n\n");
        if (eventStatusInfo.eventBitmap & BCM_PLOAM_EVENT_ONU_STATE_CHANGE)
            printf("   ONU operational state change: YES\n");
        else
            printf("   ONU operational state change: NO\n");
        if (eventStatusInfo.eventBitmap & BCM_PLOAM_EVENT_ALARM)
            printf("   ONU alarm state change: YES\n");
        else
            printf("   ONU alarm state change: NO\n");
        if (eventStatusInfo.eventBitmap & BCM_PLOAM_EVENT_OMCI_PORT_ID)
            printf("   OMCI port ID state change: YES\n");
        else
            printf("   OMCI port ID state change: NO\n");
        if (eventStatusInfo.eventBitmap & BCM_PLOAM_EVENT_USER_PLOAM_RX_OVERFLOW)
            printf("   Received FIFO overflow: YES\n");
        else
            printf("   Received FIFO overflow: NO\n");
        if (eventStatusInfo.eventBitmap & BCM_PLOAM_EVENT_USER_PLOAM_TX_OVERFLOW)
            printf("   Transmitted FIFO overflow: YES\n");
        else
            printf("   Transmitted FIFO overflow: NO\n");
        if (eventStatusInfo.eventBitmap & BCM_PLOAM_EVENT_GEM_STATE_CHANGE)
            printf("   GEM Port Operational state change : YES\n");
        else
            printf("   GEM Port Operational state change : NO\n");
        printf("\n=====================================\n\n");

        printf("\n========= PLOAM Event Mask ========\n\n");
        if (eventStatusInfo.eventMask & BCM_PLOAM_EVENT_ONU_STATE_CHANGE)
            printf("   ONU operational state change: ON\n");
        else
            printf("   ONU operational state change: OFF\n");
        if (eventStatusInfo.eventMask & BCM_PLOAM_EVENT_ALARM)
            printf("   ONU alarm state change: ON\n");
        else
            printf("   ONU alarm state change: OFF\n");
        if (eventStatusInfo.eventMask & BCM_PLOAM_EVENT_OMCI_PORT_ID)
            printf("   OMCI port ID state change: ON\n");
        else
            printf("   OMCI port ID state change: OFF\n");
        if (eventStatusInfo.eventMask & BCM_PLOAM_EVENT_USER_PLOAM_RX_OVERFLOW)
            printf("   Received FIFO overflow: ON\n");
        else
            printf("   Received FIFO overflow: OFF\n");
        if (eventStatusInfo.eventMask & BCM_PLOAM_EVENT_USER_PLOAM_TX_OVERFLOW)
            printf("   Transmitted FIFO overflow: ON\n");
        else
            printf("   Transmitted FIFO overflow: OFF\n");
        if (eventStatusInfo.eventBitmap & BCM_PLOAM_EVENT_GEM_STATE_CHANGE)
            printf("   GEM Port Operational state change : YES\n");
        else
            printf("   GEM Port Operational state change : NO\n");
        printf("\n=====================================\n\n");

    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: maskEventHandler
 * Description  : Processes the BCM_PLOAM_IOC_MASK_EVENT command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int maskEventHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    UINT8 state = 0, alarm = 0, port = 0;
    UINT8 rxo = 0, txo = 0, gem = 0;
    BCM_Ploam_EventMaskInfo info;

    BCM_Ploam_EventStatusInfo eventStatusInfo;
    int ret = gponCtl_getEventStatus(&eventStatusInfo);
    if (ret == GPONCTL_SUCCESS)
    { 
        state = eventStatusInfo.eventMask & BCM_PLOAM_EVENT_ONU_STATE_CHANGE;
        alarm = eventStatusInfo.eventMask & BCM_PLOAM_EVENT_ALARM;
        port = eventStatusInfo.eventMask & BCM_PLOAM_EVENT_OMCI_PORT_ID;
        rxo = eventStatusInfo.eventMask & BCM_PLOAM_EVENT_USER_PLOAM_RX_OVERFLOW;
        txo = eventStatusInfo.eventMask & BCM_PLOAM_EVENT_USER_PLOAM_TX_OVERFLOW;
        gem = eventStatusInfo.eventMask & BCM_PLOAM_EVENT_GEM_STATE_CHANGE;
    }

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--state"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify ONU operational state mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            state = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if (state > 1)
            {
                fprintf(stderr,
                        "%s: invalid ONU operational state mask %d\n",
                        g_gponctl_PgmName, state);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
        }
        else if (!strcasecmp(pOptions->pOptName, "--alarm"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify alarm state mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            alarm = strtoul(pOptions->pParms[0], NULL, 0);
            if (alarm > 1)
            {
                fprintf(stderr,
                        "%s: invalid alarm state mask %d\n",
                        g_gponctl_PgmName, alarm);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--port"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify OMCI port ID mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            port = strtoul(pOptions->pParms[0], NULL, 0);
            if (port > 1)
            {
                fprintf(stderr,
                        "%s: invalid OMCI port ID mask %d\n",
                        g_gponctl_PgmName, port);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--rxo"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify received FIFO overflow mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            rxo = strtoul(pOptions->pParms[0], NULL, 0);
            if (rxo > 1)
            {
                fprintf(stderr,
                        "%s: invalid received FIFO overflow mask %d\n",
                        g_gponctl_PgmName, rxo);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--txo"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify transmitted FIFO overflow mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            txo = strtoul(pOptions->pParms[0], NULL, 0);
            if (txo > 1)
            {
                fprintf(stderr,
                        "%s: invalid transmitted FIFO overflow mask %d\n",
                        g_gponctl_PgmName, txo);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--gem"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify GEM state change mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            gem = strtoul(pOptions->pParms[0], NULL, 0);
            if (gem > 1)
            {
                fprintf(stderr,
                        "%s: invalid GEM state change mask %d\n",
                        g_gponctl_PgmName, txo);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    printf("\n========= PLOAM Event Mask ========\n\n");
    if (state)
        printf("   ONU operational state: ON\n");
    else
        printf("   ONU operational state: OFF\n");
    if (alarm)
        printf("   ONU alarm state: ON\n");
    else
        printf("   ONU alarm state: OFF\n");
    if (port)
        printf("   OMCI port ID state: ON\n");
    else
        printf("   OMCI port ID state: OFF\n");
    if (rxo)
        printf("   Received FIFO overflow: ON\n");
    else
        printf("   Received FIFO overflow: OFF\n");
    if (txo)
        printf("   Transmitted FIFO overflow: ON\n");
    else
        printf("   Transmitted FIFO overflow: OFF\n");
    if (gem)
        printf("   GEM Port Operational state change: ON\n");
    else
        printf("   GEM Port Operational state change: OFF\n");
    printf("\n===================================\n\n");

    info.eventMask = 0;
    if (state)
        info.eventMask |= BCM_PLOAM_EVENT_ONU_STATE_CHANGE;
    if (alarm)
        info.eventMask |= BCM_PLOAM_EVENT_ALARM;
    if (port)
        info.eventMask |= BCM_PLOAM_EVENT_OMCI_PORT_ID;
    if (rxo)
        info.eventMask |= BCM_PLOAM_EVENT_USER_PLOAM_RX_OVERFLOW;
    if (txo)
        info.eventMask |= BCM_PLOAM_EVENT_USER_PLOAM_TX_OVERFLOW;
    if (gem)
        info.eventMask |= BCM_PLOAM_EVENT_GEM_STATE_CHANGE;

    return gponCtl_maskEvent(&info);
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getAlarmHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_ALARM_STATUS command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getAlarmHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_AlarmStatusInfo info;

    int ret = gponCtl_getAlarmStatus(&info);

    // only display info if success
    if (ret != GPONCTL_SUCCESS)
        goto out;

    printf("\n========= PLOAM Alarm Status ========\n\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_APC_FAIL)
        printf("   Auto-power control failure: ON\n");
    else
        printf("   Auto-power control failure: OFF\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_LOS)
        printf("   Loss of signal: ON\n");
    else
        printf("   Loss of signal: OFF\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_LOL)
        printf("   Loss of link: ON\n");
    else
        printf("   Loss of link: OFF\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_LOF)
        printf("   Loss of frame: ON\n");
    else
        printf("   Loss of frame: OFF\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_LCDG)
        printf("   Loss of GEM channel delineation: ON\n");
    else
        printf("   Loss of GEM channel delineation: OFF\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_SF)
        printf("   Failed signal: ON\n");
    else
        printf("   Failed signal: OFF\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_SD)
        printf("   Degraded signal: ON\n");
    else
        printf("   Degraded signal: OFF\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_SUF)
        printf("   Start-up failure: ON\n");
    else
        printf("   Start-up failure: OFF\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_MEM)
        printf("   Message error message: ON\n");
    else
        printf("   Message error message: OFF\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_DACT)
        printf("   Deactivate ONU-ID: ON\n");
    else
        printf("   Deactivate ONU-ID: OFF\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_DIS)
        printf("   Disabled ONU: ON\n");
    else
        printf("   Disabled ONU: OFF\n");
    if (info.alarmStatusBitmap & BCM_PLOAM_ALARM_ID_PEE)
        printf("   Physical equipment error: ON\n");
    else
        printf("   Physical equipment error: OFF\n");
    printf("\n=====================================\n\n");

    printf("\n========= PLOAM Alarm Event ========\n\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_APC_FAIL)
        printf("   Auto-power control failure state change: YES\n");
    else
        printf("   Auto-power control failure state change: NO\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_LOS)
        printf("   Loss of signal state change: YES\n");
    else
        printf("   Loss of signal state change: NO\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_LOL)
        printf("   Loss of link state change: YES\n");
    else
        printf("   Loss of link state change: NO\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_LOF)
        printf("   Loss of frame state change: YES\n");
    else
        printf("   Loss of frame state change: NO\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_LCDG)
        printf("   Loss of GEM channel delineation state change: YES\n");
    else
        printf("   Loss of GEM channel delineation state change: NO\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_SF)
        printf("   Failed signal state change: YES\n");
    else
        printf("   Failed signal state change: NO\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_SD)
        printf("   Degraded signal state change: YES\n");
    else
        printf("   Degraded signal state change: NO\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_SUF)
        printf("   Start-up failure state change: YES\n");
    else
        printf("   Start-up failure state change: NO\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_MEM)
        printf("   Message error message state change: YES\n");
    else
        printf("   Message error message state change: NO\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_DACT)
        printf("   Deactivate ONU-ID state change: YES\n");
    else
        printf("   Deactivate ONU-ID state change: NO\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_DIS)
        printf("   Disabled ONU state change: YES\n");
    else
        printf("   Disabled ONU state change: NO\n");
    if (info.alarmEventBitmap & BCM_PLOAM_ALARM_ID_PEE)
        printf("   Physical equipment error state change: YES\n");
    else
        printf("   Physical equipment error state change: NO\n");
    printf("\n=====================================\n\n");

    printf("\n========= PLOAM Alarm Mask ========\n\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_APC_FAIL)
        printf("   Auto-power control failure: ON\n");
    else
        printf("   Auto-power control failure: OFF\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LOS)
        printf("   Loss of signal: ON\n");
    else
        printf("   Loss of signal: OFF\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LOL)
        printf("   Loss of link: ON\n");
    else
        printf("   Loss of link: OFF\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LOF)
        printf("   Loss of frame: ON\n");
    else
        printf("   Loss of frame: OFF\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LCDG)
        printf("   Loss of GEM channel delineation: ON\n");
    else
        printf("   Loss of GEM channel delineation: OFF\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_SF)
        printf("   Failed signal: ON\n");
    else
        printf("   Failed signal: OFF\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_SD)
        printf("   Degraded signal: ON\n");
    else
        printf("   Degraded signal: OFF\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_SUF)
        printf("   Start-up failure: ON\n");
    else
        printf("   Start-up failure: OFF\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_MEM)
        printf("   Message error message: ON\n");
    else
        printf("   Message error message: OFF\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_DACT)
        printf("   Deactivate ONU-ID: ON\n");
    else
        printf("   Deactivate ONU-ID: OFF\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_DIS)
        printf("   Disabled ONU: ON\n");
    else
        printf("   Disabled ONU: OFF\n");
    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_PEE)
        printf("   Physical equipment error: ON\n");
    else
        printf("   Physical equipment error: OFF\n");
    printf("\n=====================================\n\n");

out:
    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: maskAlarmHandler
 * Description  : Processes the BCM_PLOAM_IOC_MASK_ALARM command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int maskAlarmHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    UINT8 los = 0, lof = 0, sf = 0, sd = 0;
    UINT8 lcdg = 0, suf = 0, mem = 0, dact = 0;
    UINT8 dis = 0, lol = 0, apc = 0, pee = 0;
    BCM_Ploam_MaskAlarmInfo mask;
    BCM_Ploam_AlarmStatusInfo info;

    gponCtl_getAlarmStatus(&info);

    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_APC_FAIL)
      apc = 1;

    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LOS)
      los = 1;

    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LOL)
      lol = 1;

    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LOF)
      lof = 1;

    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LCDG)
      lcdg = 1;

    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_SF)
      sf = 1;

    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_SD)
      sd = 1;

    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_SUF)
      suf = 1;

    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_MEM)
      mem = 1;

    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_DACT)
      dact = 1;

    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_DIS)
      dis = 1;

    if (info.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_PEE)
      pee = 1;

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--los"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify loss of signal mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            los = strtoul(pOptions->pParms[0], NULL, 0);
            if (los > 1)
            {
                fprintf(stderr,
                        "%s: invalid loss of signal mask %d\n",
                        g_gponctl_PgmName, los);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--lof"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify loss of frame mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            lof = strtoul(pOptions->pParms[0], NULL, 0);
            if (lof > 1)
            {
                fprintf(stderr,
                        "%s: invalid loss of frame mask %d\n",
                        g_gponctl_PgmName, lof);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--sf"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify failed signal mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            sf = strtoul(pOptions->pParms[0], NULL, 0);
            if (sf > 1)
            {
                fprintf(stderr,
                        "%s: invalid failed signal mask %d\n",
                        g_gponctl_PgmName, sf);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--sd"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify degraded signal mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            sd = strtoul(pOptions->pParms[0], NULL, 0);
            if (sd > 1)
            {
                fprintf(stderr,
                        "%s: invalid degraded signal mask %d\n",
                        g_gponctl_PgmName, sd);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--lcdg"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify loss of channel delineation mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            lcdg = strtoul(pOptions->pParms[0], NULL, 0);
            if (lcdg > 1)
            {
                fprintf(stderr,
                        "%s: invalid loss of channel delineation mask %d\n",
                        g_gponctl_PgmName, lcdg);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--suf"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify start-up failure mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            suf = strtoul(pOptions->pParms[0], NULL, 0);
            if (suf > 1)
            {
                fprintf(stderr,
                        "%s: invalid start-up failure mask %d\n",
                        g_gponctl_PgmName, suf);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--mem"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify error message mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            mem = strtoul(pOptions->pParms[0], NULL, 0);
            if (mem > 1)
            {
                fprintf(stderr,
                        "%s: invalid error message mask %d\n",
                        g_gponctl_PgmName, mem);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--dact"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify deactivate ONU-ID mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            dact = strtoul(pOptions->pParms[0], NULL, 0);
            if (dact > 1)
            {
                fprintf(stderr,
                        "%s: invalid deactivate ONU-ID mask %d\n",
                        g_gponctl_PgmName, dact);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--dis"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify disabled ONU mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            dis = strtoul(pOptions->pParms[0], NULL, 0);
            if (dis > 1)
            {
                fprintf(stderr,
                        "%s: invalid disabled ONU mask %d\n",
                        g_gponctl_PgmName, dis);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--lol"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify loss of link mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            lol = strtoul(pOptions->pParms[0], NULL, 0);
            if (lol > 1)
            {
                fprintf(stderr,
                        "%s: invalid loss of link mask %d\n",
                        g_gponctl_PgmName, lol);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--apc"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify auto-power control failure mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            apc = strtoul(pOptions->pParms[0], NULL, 0);
            if (apc > 1)
            {
                fprintf(stderr,
                        "%s: invalid auto-power control failure mask %d\n",
                        g_gponctl_PgmName, apc);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--pee"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify physical equipment error mask <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            pee = strtoul(pOptions->pParms[0], NULL, 0);
            if (pee > 1)
            {
                fprintf(stderr,
                        "%s: invalid physical equipment error mask %d\n",
                        g_gponctl_PgmName, pee);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    mask.alarmMaskBitmap = 0;

    if (apc)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_APC_FAIL;
    if (los)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_LOS;
    if (lol)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_LOL;
    if (lof)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_LOF;
    if (lcdg)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_LCDG;
    if (sf)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_SF;
    if (sd)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_SD;
    if (suf)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_SUF;
    if (mem)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_MEM;
    if (dact)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_DACT;
    if (dis)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_DIS;
    if (pee)
        mask.alarmMaskBitmap |= BCM_PLOAM_ALARM_ID_PEE;

    printf("\n========= PLOAM Alarm Mask ========\n\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_APC_FAIL)
        printf("   Auto-power control failure: ON\n");
    else
        printf("   Auto-power control failure: OFF\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LOS)
        printf("   Loss of signal: ON\n");
    else
        printf("   Loss of signal: OFF\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LOL)
        printf("   Loss of link: ON\n");
    else
        printf("   Loss of link: OFF\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LOF)
        printf("   Loss of frame: ON\n");
    else
        printf("   Loss of frame: OFF\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_LCDG)
        printf("   Loss of GEM channel delineation: ON\n");
    else
        printf("   Loss of GEM channel delineation: OFF\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_SF)
        printf("   Failed signal: ON\n");
    else
        printf("   Failed signal: OFF\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_SD)
        printf("   Degraded signal: ON\n");
    else
        printf("   Degraded signal: OFF\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_SUF)
        printf("   Start-up failure: ON\n");
    else
        printf("   Start-up failure: OFF\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_MEM)
        printf("   Message error message: ON\n");
    else
        printf("   Message error message: OFF\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_DACT)
        printf("   Deactivate ONU-ID: ON\n");
    else
        printf("   Deactivate ONU-ID: OFF\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_DIS)
        printf("   Disabled ONU: ON\n");
    else
        printf("   Disabled ONU: OFF\n");
    if (mask.alarmMaskBitmap & BCM_PLOAM_ALARM_ID_PEE)
        printf("   Physical equipment error: ON\n");
    else
        printf("   Physical equipment error: OFF\n");
    printf("\n=====================================\n\n");

    return gponCtl_maskAlarm(&mask);
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getSfSdThresholdHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_SF_SD_THRESHOLD command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getSfSdThresholdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_SFSDthresholdInfo info;

    int ret = gponCtl_getSFSDThreshold(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n=========== Signal Thresholds ==========\n\n");
        printf("   Failed signal threshold: %d (exp)\n", info.sf_exp);
        printf("   Degraded signal threshold: %d (exp)\n", info.sd_exp);
        printf("\n=========== Signal Thresholds ==========\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: setSfSdThresholdHandler
 * Description  : Processes the BCM_PLOAM_IOC_SET_SF_SD_THRESHOLD command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int setSfSdThresholdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_SFSDthresholdInfo info;
    int ret = gponCtl_getSFSDThreshold(&info);

    if (ret != GPONCTL_SUCCESS)
        goto out;

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--sf"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify failed signal threshold <3..8> (exp)\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.sf_exp = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if (info.sf_exp < 3 || info.sf_exp > 8)
            {
                fprintf(stderr,
                        "%s: invalid failed signal threshold %d (exp)\n",
                        g_gponctl_PgmName, info.sf_exp);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
        }
        else if (!strcasecmp(pOptions->pOptName, "--sd"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify degraded signal threshold <1..9> (exp)\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.sd_exp = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if (info.sd_exp < 1 || info.sd_exp > 9)
            {
                fprintf(stderr,
                        "%s: invalid degraded signal threshold %d (exp)\n",
                        g_gponctl_PgmName, info.sd_exp);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    ret = gponCtl_setSFSDThreshold(&info);
    if (ret != GPONCTL_SUCCESS)
        goto out;

    printf("\n=========== Signal Thresholds ==========\n\n");
    printf("   Failed signal threshold: %d (exp)\n", info.sf_exp);
    printf("   Degraded signal threshold: %d (exp)\n", info.sd_exp);
    printf("\n=========== Signal Thresholds ==========\n\n");

out:
    return ret;
}




/*
 *------------------------------------------------------------------------------
 * Function Name: transmitMsgHandler
 * Description  : Write upstream PLOAM message to bcm_user_ploam device file.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int transmitMsgHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    UINT8 msg[GPONCTL_PLOAM_MSG_SIZE];
    UINT16 msgId = 0, i = 0;
    int fd = 0;
    int ret = GPONCTL_SUCCESS;
    int block = 0;

    memset(msg, 0, GPONCTL_PLOAM_MSG_SIZE);

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--block"))
        {
            block = 1;
        }
        else if (!strcasecmp(pOptions->pOptName, "--id"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify message ID <0..255>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            msgId = strtoul(pOptions->pParms[0], NULL, 0);
            if (msgId > 255)
            {
                fprintf(stderr,
                        "%s: invalid message ID %d\n",
                        g_gponctl_PgmName, msgId);
                return GPONCTL_INVALID_PARAMETER;
            }
            msg[0] = 0;   // onu_id is filled by driver
            msg[1] = msgId;
        }
        else if (!strcasecmp(pOptions->pOptName, "--data"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify message data <xx-xx-..-xx (up to 10)>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            if (ploamDataStrToNum(pOptions->pParms[0], &msg[2]))
            {
                fprintf(stderr,
                        "%s: invalid message data %s\n",
                        g_gponctl_PgmName, pOptions->pParms[0]);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    if ((fd = bcm_dev_open(GPONCTL_USER_PLOAM_FILE_NAME, block ? O_RDWR : O_RDWR|O_NONBLOCK)) < 0)
    {
        GPONCTL_LOG_ERROR("Cannot bcm_dev_open %s: %s",
                          GPONCTL_USER_PLOAM_FILE_NAME,
                          gponCtl_getMsgError(errno));
        return GPONCTL_GENERAL_ERROR;
    }

    if(bcm_dev_write(fd, (unsigned char *)msg, GPONCTL_PLOAM_MSG_SIZE) < 0)
    {
        GPONCTL_LOG_ERROR("Cannot bcm_dev_write to %s: %s",
                          GPONCTL_USER_PLOAM_FILE_NAME,
                          gponCtl_getMsgError(errno));
        ret = GPONCTL_GENERAL_ERROR;
    }

    bcm_dev_close(fd);

    if (ret >= 0)
    {
        printf("\n============= Transmit PLOAM Message =============\n\n");
        printf("   Message: ");
        for (i = 0; i < (GPONCTL_PLOAM_MSG_SIZE-1); i++)
            printf("%02X-", msg[i]);
        printf("%02X\n", msg[GPONCTL_PLOAM_MSG_SIZE-1]);
        printf("\n==================================================\n\n");
    }

    return ret;
}



/*
 *------------------------------------------------------------------------------
 * Function Name: getMsgCountersHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_MESSAGE_COUNTERS command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getMsgCountersHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_Ploam_MessageCounters counters;

    // initialize neccessary fields
    counters.reset = 0;

    if (pOptions->pOptName == NULL)
        goto out;

    if (!strcasecmp(pOptions->pOptName, "--reset"))
    {
        if (pOptions->nNumParms != 1)
        {
            fprintf(stderr,
                    "%s: identify reset <0..1>\n",
                    g_gponctl_PgmName);
            return GPONCTL_INVALID_OPTION;
        }
        counters.reset = strtoul(pOptions->pParms[0], NULL, 0);
        if (counters.reset > 1)
        {
            fprintf(stderr,
                    "%s: invalid reset %d\n",
                    g_gponctl_PgmName, counters.reset);
            return GPONCTL_INVALID_PARAMETER;
        }
    }
    else    /* should never reach here */
    {
        fprintf(stderr, "%s: invalid option [%s]\n",
                g_gponctl_PgmName, pOptions->pOptName);
        return GPONCTL_INVALID_OPTION;
    }

out:
    ret = gponCtl_getMessageCounters(&counters);

    if (ret == GPONCTL_SUCCESS)
    {
    printf("\n=========== Message Counters ===========\n\n");
    if (counters.reset)
        printf("The following counters are reset after retrieving them\n");
    else
        printf("The following counters are NOT reset after retrieving them\n");
    printf("   CRC error messages               : %u\n", counters.crcErrors);
    printf("   Total received messages          : %u\n", counters.rxPloamsTotal);
    printf("   Unicast received messages        : %u\n", counters.rxPloamsUcast);
    printf("   Broadcast received messages      : %u\n", counters.rxPloamsBcast);
    printf("   Discarded received messages      : %u\n", counters.rxPloamsDiscarded);
    printf("   Non-standard received messages   : %u\n", counters.rxPloamsNonStd);
    printf("   Total transmitted messages       : %u\n", counters.txPloams);
    printf("   Non-standard transmitted messages: %u\n", counters.txPloamsNonStd);
    printf("\n========================================\n\n");
  }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getGtcCountersHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_GTC_COUNTERS command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getGtcCountersHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_Ploam_GtcCounters gtc_counters;
    BCM_Ploam_fecCounters fec_counters;

    // initialize neccessary fields
    gtc_counters.reset = 0;
    fec_counters.reset = 0;

    if (pOptions->pOptName == NULL)
        goto out;

    if (!strcasecmp(pOptions->pOptName, "--reset"))
    {
        if (pOptions->nNumParms != 1)
        {
            fprintf(stderr,
                    "%s: identify reset <0..1>\n",
                    g_gponctl_PgmName);
            return GPONCTL_INVALID_OPTION;
        }
        gtc_counters.reset = strtoul(pOptions->pParms[0], NULL, 0);
        if (gtc_counters.reset > 1)
        {
            fprintf(stderr,
                    "%s: invalid reset %u\n",
                    g_gponctl_PgmName, gtc_counters.reset);
            return GPONCTL_INVALID_PARAMETER;
        }
        fec_counters.reset = gtc_counters.reset;
    }
    else    /* should never reach here */
    {
        fprintf(stderr, "%s: invalid option [%s]\n",
                g_gponctl_PgmName, pOptions->pOptName);
        return GPONCTL_INVALID_OPTION;
    }

out:
    ret = gponCtl_getGtcCounters(&gtc_counters);
    ret = ret ? ret : gponCtl_getFecCounters(&fec_counters);
    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n=============== GTC Counters ================\n\n");
        if (gtc_counters.reset)
            printf("The following counters are reset after retrieving them\n");
        else
            printf("The following counters are NOT reset after retrieving them\n");
        printf("   BIP error                           : %u\n", gtc_counters.bipErr);
        printf("   FEC corrected bytes                 : %u\n", fec_counters.fecByte);
        printf("   FEC corrected codewords             : %u\n", fec_counters.fecCerr);
        printf("   FEC uncorrectable codewords         : %u\n", fec_counters.fecUerr);
        printf("   Total received DS FEC codewords     : %u\n", fec_counters.fecCWs);
        printf("   FEC correction seconds              : %u\n", fec_counters.fecSecs);
        printf("\n=============================================\n\n");
    }

    return ret;
}

static int validate_opt_gemportid(char *opt_str, PGPON_OPTION_INFO pOptions, int nNumOptions, uint16_t *gem_port_id)
{
    if (!strcasecmp(pOptions->pOptName, opt_str))
    {
        if (pOptions->nNumParms != 1)
        {
            fprintf(stderr,
                    "%s: identify GEM port ID <0..BCM_PLOAM_MAX_XGEM_ID(0x%x)>\n",
                    g_gponctl_PgmName, BCM_PLOAM_MAX_XGEM_ID);
            return GPONCTL_INVALID_OPTION;
        }
        *gem_port_id = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
        if (*gem_port_id > BCM_PLOAM_MAX_XGEM_ID))
        {
            fprintf(stderr,
                    "%s: invalid GEM port ID %d\n",
                    g_gponctl_PgmName, *gem_port_id);
            return GPONCTL_INVALID_PARAMETER;
        }
#endif

        return -1; /* Found, ok */
    }

    return 0; /* Not found */
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getGemPortCountersHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_GEM_PORT_COUNTERS command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getGemPortCountersHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    UINT8 useIndex = 1;
    BCM_Ploam_GemPortCounters counters;

    // initialize neccessary fields
    counters.reset = 0;
    counters.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_ALL;
    counters.gemPortID = BCM_PLOAM_GEM_PORT_ID_UNASSIGNED;

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--reset"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify reset <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            counters.reset = strtoul(pOptions->pParms[0], NULL, 0);
            if (counters.reset > 1)
            {
                fprintf(stderr,
                        "%s: invalid reset %u\n",
                        g_gponctl_PgmName, counters.reset);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--index"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify GEM port index <0..(BCM_PLOAM_NUM_DATA_GEM_PORTS-1)|BCM_PLOAM_GEM_PORT_IDX_ALL(0xfffe)|BCM_PLOAM_GEM_PORT_IDX_ETH_ALL(0xfffd)>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            counters.gemPortIndex = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if (counters.gemPortIndex > (BCM_PLOAM_NUM_DATA_GEM_PORTS - 1) &&
                counters.gemPortIndex != BCM_PLOAM_GEM_PORT_IDX_ALL &&
                counters.gemPortIndex != BCM_PLOAM_GEM_PORT_IDX_ETH_ALL)
            {
                fprintf(stderr,
                        "%s: invalid GEM port index %d\n",
                        g_gponctl_PgmName, counters.gemPortIndex);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
            // set useIndex be true if index arg exists
            useIndex = 2;
        }
        else if ((ret = validate_opt_gemportid("--id", pOptions, nNumOptions, &counters.gemPortID)))
        {
            if (ret > 0)
                return ret;
            
            // if no index arg then set useIndex be false
            if (useIndex != 2)
                useIndex = 0;
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    if (useIndex)
        counters.gemPortID = BCM_PLOAM_GEM_PORT_ID_UNASSIGNED;
    else
        counters.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;

    ret = gponCtl_getGemPortCounters(&counters);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n============= GEM Port Counters ==============\n\n");
        if (counters.reset)
            printf("The following counters are reset after retrieving them\n");
        else
            printf("The following counters are NOT reset after retrieving them\n");
        if (useIndex)
        {
            if (counters.gemPortIndex == BCM_PLOAM_GEM_PORT_IDX_ALL)
                printf("Following counters retrieved for all GEM ports\n");
            else
                printf("Following counters retrieved for GEM port index %d\n", counters.gemPortIndex);
        }
        else
            printf("The following counters are retrieved for GEM port ID %d\n", counters.gemPortID);
        printf("   Received bytes               : %u\n", counters.rxBytes);
        printf("   Received fragments           : %u\n", counters.rxFragments);
        printf("   Received frames              : %u\n", counters.rxFrames);
        printf("   Received dropped frames      : %u\n", counters.rxDroppedFrames);
        printf("   Transmitted bytes            : %u\n", counters.txBytes);
        printf("   Transmitted fragments        : %u\n", counters.txFragments);
        printf("   Transmitted frames           : %u\n", counters.txFrames);
        printf("   Transmit dropped frames      : %u\n", counters.txDroppedFrames);
        printf("   Accepted Multicast Frames    : %u\n", counters.rxMcastAcceptedFrames);
        printf("   Dropped Multicast Frames     : %u\n", counters.rxMcastDroppedFrames);
        printf("\n==============================================\n\n");
    }

    return ret;
}
/*
 *------------------------------------------------------------------------------
 * Function Name: configGemPortHandler
 * Description  : Processes the BCM_PLOAM_IOC_CFG_GEM_PORT command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int configGemPortHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_Ploam_CfgGemPortInfo info;

    // initialize neccessary fields
    info.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
    info.gemPortID = BCM_PLOAM_GEM_PORT_ID_UNASSIGNED;
    info.allocID = BCM_PLOAM_ALLOC_ID_UNASSIGNED;
    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--index"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify GEM port index <0..(BCM_PLOAM_NUM_DATA_GEM_PORTS-1)|BCM_PLOAM_GEM_PORT_IDX_ALL(0xfffe)>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.gemPortIndex = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if (info.gemPortIndex > (BCM_PLOAM_NUM_DATA_GEM_PORTS - 1) &&
                info.gemPortIndex != BCM_PLOAM_GEM_PORT_IDX_ALL)
            {
                fprintf(stderr,
                        "%s: invalid GEM port index %d\n",
                        g_gponctl_PgmName, info.gemPortIndex);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
        }
        else if ((ret = validate_opt_gemportid("--id", pOptions, nNumOptions, &info.gemPortID)))
        {
            if (ret > 0)
                return ret;
        }
        else if (!strcasecmp(pOptions->pOptName, "--alloc"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify alloc ID <0..BCM_PLOAM_MAX_XALLOC_ID(0x%x)>\n",
                        g_gponctl_PgmName, BCM_PLOAM_MAX_XALLOC_ID);
                return GPONCTL_INVALID_OPTION;
            }
            info.allocID = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if ((info.allocID > BCM_PLOAM_MAX_XALLOC_ID) &&
                (info.allocID != BCM_PLOAM_ALLOC_ID_UNASSIGNED))
            {
                fprintf(stderr,
                        "%s: invalid alloc ID %d\n",
                        g_gponctl_PgmName, info.allocID);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
        }
        else if (!strcasecmp(pOptions->pOptName, "--mcast"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify multicast flag <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.isMcast = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if (info.isMast > 1)
            {
                fprintf(stderr,
                        "%s: invalid multicast flag %d\n",
                        g_gponctl_PgmName, info.isMcast);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
        }

        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    ret = gponCtl_configGemPort(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n====== GEM Port Configuration =======\n\n");
        printf("GEM port is configured with the following configuration\n");
        printf("   Port index  : %d\n", info.gemPortIndex);
        printf("   Port ID     : %d\n", info.gemPortID);
        printf("   Alloc ID    : %d\n", info.allocID);
        printf("\n=====================================\n\n");
    }

  return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: configDsGemPortEncryptionByIX
 * Description  : Processes the BCM_PLOAM_IOC_CFG_DS_GEM_PORT_IX_ENCRYPTION command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int configDsGemPortEncryptionByIX(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_Ploam_GemPortEncryption conf;

    // initialize neccessary fields
    conf.gemIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
    conf.encryption = 0;

    while (nNumOptions)
       {
           if (!strcasecmp(pOptions->pOptName, "--index"))
           {
               if (pOptions->nNumParms != 1)
               {
                   fprintf(stderr,
                           "%s: identify GEM port index <0..(BCM_PLOAM_NUM_DATA_GEM_PORTS-1)>\n",
                           g_gponctl_PgmName);
                   return GPONCTL_INVALID_OPTION;
               }
               conf.gemIndex = strtoul(pOptions->pParms[0], NULL, 0);
   #ifdef GPONCTL_VALIDATE_PARAMS
               if (conf.gemIndex > (BCM_PLOAM_NUM_DATA_GEM_PORTS - 1))
               {
                   fprintf(stderr,
                           "%s: invalid GEM port index %d\n",
                           g_gponctl_PgmName, conf.gemIndex);
                   return GPONCTL_INVALID_PARAMETER;
               }
   #endif
           }
           else if (!strcasecmp(pOptions->pOptName, "--encryption"))
           {
               if (pOptions->nNumParms != 1)
               {
                   fprintf(stderr,
                           "%s: identify DS encryption <0..1>\n",
                           g_gponctl_PgmName);
                   return GPONCTL_INVALID_OPTION;
               }
               conf.encryption = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
               if (conf.encryption > 1)
               {
                   fprintf(stderr,
                           "%s: invalid encryption option %d\n",
                           g_gponctl_PgmName, conf.encryption);
                   return GPONCTL_INVALID_PARAMETER;
               }
#endif
           }

           nNumOptions--;
           pOptions++;
       }

    if (conf.gemIndex != BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED)
        ret = gponCtl_configDsGemPortEncryptionByIX(&conf);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n====== DS GEM Port Encryption Configuration =======\n\n");
        printf("DS GEM Port Encryption is configured with the following configuration\n");
        printf("   Port index    : %d\n", conf.gemIndex);
        printf("   DS encryption : %d\n", conf.encryption);
        printf("\n=====================================\n\n");
    }

  return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: configDsGemPortEncryptionByID
 * Description  : Processes the BCM_PLOAM_IOC_CFG_DS_GEM_PORT_ID_ENCRYPTION command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int configDsGemPortEncryptionByID(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_Ploam_GemPortEncryption conf;

    // initialize neccessary fields
    conf.gemPortId = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
    conf.encryption = 0;

    while (nNumOptions)
    {
        if ((ret = validate_opt_gemportid("--gemPortId", pOptions, nNumOptions, &conf.gemPortId)))
        {
            if (ret > 0)
                return ret;
        }
        else if (!strcasecmp(pOptions->pOptName, "--encryption"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify DS encryption <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            conf.encryption = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if (conf.encryption > 1)
            {
                fprintf(stderr,
                        "%s: invalid encryption option %d\n",
                        g_gponctl_PgmName, conf.encryption);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
        }

        nNumOptions--;
        pOptions++;
    }

    if (conf.gemPortId != BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED)
        ret = gponCtl_configDsGemPortEncryptionByID(&conf);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n====== DS GEM Port Encryption Configuration =======\n\n");
        printf("DS GEM Port Encryption is configured with the following configuration\n");
        printf("   Port ID    : %d\n", conf.gemPortId);
        printf("   DS encryption : %d\n", conf.encryption);
        printf("\n=====================================\n\n");
    }

    return ret;
}





/*
 *------------------------------------------------------------------------------
 * Function Name: deconfigGemPortHandler
 * Description  : Processes the BCM_PLOAM_IOC_DECFG_GEM_PORT command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int deconfigGemPortHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret;
    BCM_Ploam_DecfgGemPortInfo info;

    // initialize neccessary fields
    info.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
    info.gemPortID = BCM_PLOAM_GEM_PORT_ID_UNASSIGNED;

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--index"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify GEM port index <0..(BCM_PLOAM_NUM_DATA_GEM_PORTS-1)|BCM_PLOAM_GEM_PORT_IDX_ALL(0xfffe)>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.gemPortIndex = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if (info.gemPortIndex > (BCM_PLOAM_NUM_DATA_GEM_PORTS - 1) &&
                info.gemPortIndex != BCM_PLOAM_GEM_PORT_IDX_ALL)
            {
                fprintf(stderr,
                        "%s: invalid GEM port index %d\n",
                        g_gponctl_PgmName, info.gemPortIndex);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
        }
        else if ((ret = validate_opt_gemportid("--id", pOptions, nNumOptions, &info.gemPortID)))
        {
            if (ret > 0)
                return ret;
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    printf("\n====== GEM Port Deconfiguration =======\n\n");
    printf("GEM port is deconfigured with the following configuration\n");
    printf("   Port index: %d\n", info.gemPortIndex);
    printf("   Port ID   : %d\n", info.gemPortID);
    printf("\n=====================================\n\n");

    return gponCtl_deconfigGemPort(&info);
}


/*
 *------------------------------------------------------------------------------
 * Function Name: enableGemPortHandler
 * Description  : Processes the BCM_PLOAM_IOC_ENABLE_GEM_PORT command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int enableGemPortHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret;
    BCM_Ploam_EnableGemPortInfo info;

    // initialize neccessary fields
    info.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
    info.gemPortID = BCM_PLOAM_GEM_PORT_ID_UNASSIGNED;
    info.enable = 1;

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--index"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify GEM port index <0..(BCM_PLOAM_NUM_DATA_GEM_PORTS-1)|BCM_PLOAM_GEM_PORT_IDX_ALL(0xfffe)>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.gemPortIndex = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if (info.gemPortIndex > (BCM_PLOAM_NUM_DATA_GEM_PORTS - 1) &&
                info.gemPortIndex != BCM_PLOAM_GEM_PORT_IDX_ALL)
            {
                fprintf(stderr,
                        "%s: invalid GEM port index %d\n",
                        g_gponctl_PgmName, info.gemPortIndex);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
        }
        else if ((ret = validate_opt_gemportid("--id", pOptions, nNumOptions, &info.gemPortID)))
        {
            if (ret > 0)
                return ret;
        }
        else if (!strcasecmp(pOptions->pOptName, "--enable"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify enable GEM port<0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.enable = strtoul(pOptions->pParms[0], NULL, 0);
            if (info.enable > 1)
            {
                fprintf(stderr,
                        "%s: invalid enable GEM port %d\n",
                        g_gponctl_PgmName, info.enable);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    printf("\n====== Enable/Disable GEM Port =======\n\n");
    if (info.enable)
        printf("GEM port is enabled with the following configuration\n");
    else
        printf("GEM port is disabled with the following configuration\n");
    printf("   Port index: %d\n", info.gemPortIndex);
    printf("   Port ID   : %d\n", info.gemPortID);
    printf("   Enable    : %d\n", info.enable);
    printf("\n======================================\n\n");

    return gponCtl_enableGemPort(&info);
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getGemPortHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_GEM_PORT_CFG command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getGemPortHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_Ploam_GemPortInfo info;

    // initialize neccessary fields
    info.gemPortIndex = BCM_PLOAM_GEM_PORT_IDX_UNASSIGNED;
    info.gemPortID = BCM_PLOAM_GEM_PORT_ID_UNASSIGNED;

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--index"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify GEM port index <0..(BCM_PLOAM_NUM_DATA_GEM_PORTS-1)|BCM_PLOAM_GEM_PORT_IDX_ALL(0xfffe)>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.gemPortIndex = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if (info.gemPortIndex > (BCM_PLOAM_NUM_DATA_GEM_PORTS - 1) &&
                info.gemPortIndex != BCM_PLOAM_GEM_PORT_IDX_ALL)
            {
                fprintf(stderr,
                        "%s: invalid GEM port index %d\n",
                        g_gponctl_PgmName, info.gemPortIndex);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
        }
        else if ((ret = validate_opt_gemportid("--id", pOptions, nNumOptions, &info.gemPortID)))
        {
            if (ret > 0)
                return ret;
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    ret = gponCtl_getGemPort(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n======= GEM Port Information ========\n\n");
        printf("   Port index           : %d\n", info.gemPortIndex);
        printf("   Port ID              : %d\n", info.gemPortID);
        printf("   Alloc ID             : %d\n", info.allocID);
        printf("   Flags                : %d", info.flags);
        printf(" (%s", (info.flags & BCM_PLOAM_GEM_PORT_FLAGS_PORT_ENABLED) ? " SW_EN" : " SW_DIS");
        printf("%s", (info.flags & BCM_PLOAM_GEM_PORT_FLAGS_PORT_OPERATIONAL) ? " HW_EN" : " HW_DIS");
        printf("%s", (info.flags & BCM_PLOAM_GEM_PORT_FLAGS_PORT_ENCRYPTED) ? " ENC" : "");
        printf("%s", (info.flags & BCM_PLOAM_GEM_PORT_FLAGS_DS_Q_PBIT_BASED) ? " DS_Q_PBIT" : "");
        printf("%s )\n", (info.flags & BCM_PLOAM_GEM_PORT_FLAGS_DS_Q_USE_TAG1_PBIT) ? " TAG1" : "");
        printf("   Mcast                : %d\n", info.isMcast);
        printf("   GTCUS queue          : %d\n", info.usQueueIdx);
        printf("   GTCDS queue          : %d\n", info.dsQueueIdx);
        printf("   Multicast filter mode: %d\n", info.mcastFilterMode);
        printf("\n=====================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getOmciPortHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_OMCI_PORT_INFO command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getOmciPortHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_OmciPortInfo info;

    int ret = gponCtl_getOmciPort(&info);

    // only display info if success
    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n========= OMCI Port Information ========\n\n");
        printf("   OMCI Port ID: %d\n", info.omciGemPortID);
        if (info.omciGemPortActivated)
            printf("   OMCI Port is activated\n");
        else
            printf("   OMCI Port is deactivated\n");
        if (info.encrypted)
            printf("   OMCI Port is encrypted\n");
        else
            printf("   OMCI Port is NOT encrypted\n");
        printf("\n========================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getAllocIdsHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_ALLOC_IDS command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getAllocIdsHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_AllocIDs info;
    int i = 0;
    int ret = gponCtl_getAllocIds(&info);

    // only display info if success
    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n========= Alloc IDs Information ========\n\n");
        printf("   Number of Alloc IDs: %d\n", info.numAllocIDs);
        for (i = 0;
             i < info.numAllocIDs && i < BCM_PLOAM_NUM_DATA_TCONTS;
             i++)
            printf("   Alloc ID #%d       : %d\n", i, info.allocIDs[i]);
        printf("\n========================================\n\n");
    }

    return ret;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: cfgGemBlkLenHandler
 * Description  : Processes the BCM_PLOAM_IOC_SET_GEM_BLOCK_LENGTH
 *                BCM_PLOAM_IOC_GET_GEM_BLOCK_LENGTH command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int cfgGemBlkLenHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_GemBlkLenInfo info;
    int ret, bSet = 0;

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--len"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify GEM Block Len <1..0xFFFF>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.gemBlkLen = strtoul(pOptions->pParms[0], NULL, 0);
            bSet = 1;
#ifdef GPONCTL_VALIDATE_PARAMS
        if (info.gemBlkLen <= 0 ||
            info.gemBlkLen > 0xFFFF)
        {
            fprintf(stderr,
                    "%s: Out of range %ld\n",
                    g_gponctl_PgmName, info.gemBlkLen);
            return GPONCTL_INVALID_PARAMETER;
        }
#endif
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    if (bSet)
    {
        ret = gponCtl_setGemBlockLength(&info);
    }
    else
    {
        ret = gponCtl_getGemBlockLength(&info);
    }
    // only display info if success
    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n======= GEM Block Len (DBRu Block Size) =======\n\n");
        printf("   Block Len : %u\n", info.gemBlkLen);
        printf("\n======================\n\n");
    }

    return ret;
}
/*
 *------------------------------------------------------------------------------
 * Function Name: cfgTodInfoHandler
 * Description  : Processes the BCM_PLOAM_IOC_SET_TIME_OF_DAY
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int cfgTodInfoHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_TimeOfDayInfo info;
    int ret, bSet = 0;

    memset(&info, 0, sizeof(info));

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--enable"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify enable <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.enable = strtoul(pOptions->pParms[0], NULL, 0);
            bSet = 1;
        }
        else if (!strcasecmp(pOptions->pOptName, "--usrEvent"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify enableUsrEvent <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.enableUsrEvent = strtoul(pOptions->pParms[0], NULL, 0);
        }
        else if (!strcasecmp(pOptions->pOptName, "--sf"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify superframe <0..0x3FFFFFFF>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.superframe = strtoul(pOptions->pParms[0], NULL, 0);
        }
        else if (!strcasecmp(pOptions->pOptName, "--pulseWidth"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify pulseWidth <0..255>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.pulseWidth = strtoul(pOptions->pParms[0], NULL, 0);
        }
        else if (!strcasecmp(pOptions->pOptName, "--timeNs"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify time nanosecond <0.0xFFFFFFFF>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.tStampN.nanoSeconds = strtoul(pOptions->pParms[0], NULL, 0);
        }
        else if (!strcasecmp(pOptions->pOptName, "--timeSecMSB"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify time Seconds MSB <0.0xFFFF>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.tStampN.secondsMSB = strtoul(pOptions->pParms[0], NULL, 0);
        }
        else if (!strcasecmp(pOptions->pOptName, "--timeSecLSB"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify time Seconds LSB <0.0xFFFFFFFF>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.tStampN.secondsLSB = strtoul(pOptions->pParms[0], NULL, 0);
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    if (bSet)
    {
        ret = gponCtl_setTodInfo(&info);
    }
    else
    {
        ret = gponCtl_getTodInfo(&info);
    }
    // only display info if success
    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n======= ToD Info =======\n\n");
        printf("   Enable      : %u\n", info.enable);
        printf("   UsrEvent    : %u\n", info.enableUsrEvent);
        printf("   Pulse Width : %u\n", info.pulseWidth);
        printf("   Superframe  : %u\n", info.superframe);
        printf("   SecondsMSB  : %u\n", info.tStampN.secondsMSB);
        printf("   SecondsLSB  : %u\n", info.tStampN.secondsLSB);
        printf("   NanoSeconds : %u\n", info.tStampN.nanoSeconds);
        printf("\n======================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getOnuIdHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_ONU_ID command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getOnuIdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_GetOnuIdInfo info;
    int ret = gponCtl_getOnuId(&info);

    // only display info if success
    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n======= ONU ID =======\n\n");
        printf("   ONU ID: %d\n", info.onuId);
        printf("\n======================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getFecModeHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_FEC_MODE command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getFecModeHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_GetFecModeInfo info;
    int ret = gponCtl_getFecMode(&info);

    // only display info if success
    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n=============== FEC Mode ==============\n\n");
        if (info.fecMode & BCM_PLOAM_FEC_MODE_FLAG_DS_FEC_ON)
            printf("   Downstream FEC is ON\n");
        else
            printf("   Downstream FEC is OFF\n");
        printf("\n=======================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getEncryptionKeyHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_ENCRYPTION_KEY command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getEncryptionKeyHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Ploam_GetEncryptionKeyInfo info;
    int i = 0;
    int ret = gponCtl_getEncryptionKey(&info);

    // only display info if success
    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n======= Encryption Key =======\n\n");
        printf("   Encryption key: ");
        for (i = 0; i < BCM_PLOAM_ENCRYPTION_KEY_SIZE_BYTES; i++)
            printf("%02X", info.key[i]);
        printf("\n\n==============================\n\n");
    }

    return ret;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: generatePrbsHandler
 * Description  : Processes the BCM_PLOAM_IOC_GEN_PRBS command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int generatePrbsHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_Ploam_GenPrbsInfo info;

    // initialize neccessary fields
    memset(&info, 0, sizeof(BCM_Ploam_GenPrbsInfo));

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--enable"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: disable/enable generation of PRBS sequence <0..1>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.enable = strtoul(pOptions->pParms[0], NULL, 0);
            if (info.enable > 1)
            {
                fprintf(stderr,
                        "%s: invalid enable generation of PRBS sequence %d\n",
                        g_gponctl_PgmName, info.enable);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--mode"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: specify PRBS mode <0..4>\n",
                        g_gponctl_PgmName);
                return GPONCTL_INVALID_OPTION;
            }
            info.mode = strtoul(pOptions->pParms[0], NULL, 0);
#ifdef GPONCTL_VALIDATE_PARAMS
            if (info.mode > BCM_GPON_SERDES_WRAPPER_PSEUDO_RANDOM)
            {
                fprintf(stderr,
                        "%s: invalid PRBS mode %d\n",
                        g_gponctl_PgmName, info.mode);
                return GPONCTL_INVALID_PARAMETER;
            }
#endif
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    ret = gponCtl_generatePrbsSequence(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n====== Enable/Disable Generation of PRBS Sequence =======\n\n");
        if (info.enable)
            printf("Generation of PRBS Sequence is enabled with the following configuration\n");
        else
            printf("Generation of PRBS Sequence is disabled with the following configuration\n");
        printf("   Enable: %d\n", info.enable);
        switch (info.mode)
        {
            case BCM_GPON_SERDES_PRBS7:
                printf("   Mode : PRBS7\n");
                break;
            case BCM_GPON_SERDES_PRBS15:
                printf("   Mode : PRBS15\n");
                break;
            case BCM_GPON_SERDES_PRBS23:
                printf("   Mode : PRBS23\n");
                break;
            case BCM_GPON_SERDES_PRBS31:
                printf("   Mode : PRBS31\n");
                break;
            case BCM_GPON_SERDES_WRAPPER_PSEUDO_RANDOM:
                printf("   Mode : SERDES_WRAPPER_PSEUDO_RANDOM\n");
                break;
            default:
                printf("   Mode : Unknown\n");
                break;
        }
        printf("\n======================================\n\n");
    }

    return ret;
}




/*
 *------------------------------------------------------------------------------
 * Function Name: transmitOmciMsgHandler
 * Description  : Write upstream OMCI message to bcm_omci device file.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int transmitOmciMsgHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    UINT8 msg[BCM_OMCI_TX_MSG_MAX_SIZE_BYTES];
    UINT16 length = 0, i = 0;
    int fd = 0;
    int ret = GPONCTL_SUCCESS;
    int block = 0;

    memset(msg, 0, BCM_OMCI_TX_MSG_MAX_SIZE_BYTES);

    for (i=MAX_NUM_DATA_ITEMS_IN_OMCI_INPUT_STRING; i<BCM_OMCI_TX_MSG_MAX_SIZE_BYTES; i++)
      msg[i] = (i-MAX_NUM_DATA_ITEMS_IN_OMCI_INPUT_STRING)&0xff;

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--block"))
        {
            block = 1;
        }
        else if (!strcasecmp(pOptions->pOptName, "--len"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify OMCI message length <0..%d>\n",
                        g_gponctl_PgmName, BCM_OMCI_TX_MSG_MAX_SIZE_BYTES);
                return GPONCTL_INVALID_OPTION;
            }
            length = strtoul(pOptions->pParms[0], NULL, 0);
            if (length > BCM_OMCI_TX_MSG_MAX_SIZE_BYTES)
            {
                fprintf(stderr,
                        "%s: invalid OMCI message length %d\n",
                        g_gponctl_PgmName, length);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else if (!strcasecmp(pOptions->pOptName, "--data"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify OMCI message data <xx-xx-..-xx (up to %d)>\n",
                        g_gponctl_PgmName, length);
                return GPONCTL_INVALID_OPTION;
            }
            if (omciDataStrToNum(pOptions->pParms[0], &msg[0], length))
            {
                fprintf(stderr,
                        "%s: invalid OMCI message data %s\n",
                        g_gponctl_PgmName, pOptions->pParms[0]);
                return GPONCTL_INVALID_PARAMETER;
            }
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }


    if ((fd = bcm_dev_open(GPONCTL_OMCI_FILE_NAME, block ? O_RDWR : O_RDWR|O_NONBLOCK)) < 0)
    {
        GPONCTL_LOG_ERROR("Cannot bcm_dev_open %s: %s",
                          GPONCTL_OMCI_FILE_NAME,
                          gponCtl_getMsgError(errno));
        return GPONCTL_GENERAL_ERROR;
    }

    if(bcm_dev_write(fd, (unsigned char *)msg, length) < 0)
    {
        GPONCTL_LOG_ERROR("Cannot bcm_dev_write to %s: %s",
                          GPONCTL_OMCI_FILE_NAME,
                          gponCtl_getMsgError(errno));
        ret = GPONCTL_GENERAL_ERROR;
    }

    bcm_dev_close(fd);

    if (ret == 0)
    {
        printf("\n============= Transmit OMCI Message =============\n\n");
        printf("   Message length: %d\n", length);
        printf("   Message: ");
        for (i = 0; i < (length - 1); i++)
            printf("%02X-", msg[i]);
        printf("%02X\n", msg[length-1]);
        printf("\n==================================================\n\n");
    }

    return ret;
}





/*
 *------------------------------------------------------------------------------
 * Function Name: getOmciCountersHandler
 * Description  : Processes the BCM_OMCI_IOC_GET_COUNTERS command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getOmciCountersHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0;
    BCM_Omci_Counters counters;

    // initialize neccessary fields
    counters.reset = 0;

    if (pOptions->pOptName == NULL)
        goto out;

    if (!strcasecmp(pOptions->pOptName, "--reset"))
    {
        if (pOptions->nNumParms != 1)
        {
            fprintf(stderr,
                    "%s: identify reset <0..1>\n",
                    g_gponctl_PgmName);
            return GPONCTL_INVALID_OPTION;
        }
        counters.reset = strtoul(pOptions->pParms[0], NULL, 0);
        if (counters.reset > 1)
        {
            fprintf(stderr,
                    "%s: invalid reset %u\n",
                    g_gponctl_PgmName, counters.reset);
            return GPONCTL_INVALID_PARAMETER;
        }
    }
    else    /* should never reach here */
    {
        fprintf(stderr, "%s: invalid option [%s]\n",
                g_gponctl_PgmName, pOptions->pOptName);
        return GPONCTL_INVALID_OPTION;
    }

out:
    ret = gponCtl_getOmciCounters(&counters);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n=========== OMCI Counters ===========\n\n");
        if (counters.reset)
            printf("The following counters are reset after retrieving them\n");
        else
            printf("The following counters are NOT reset after retrieving them\n");
        printf("   Received bytes             : %u\n", counters.rxBytes);
        printf("   Received fragments         : %u\n", counters.rxFragments);
        printf("   Dropped fragments          : %u\n", counters.rxFragmentsDropped);
        printf("   Total received messages    : %u\n", counters.rxMessagesTotal);
        printf("   Discarded received messages: %u\n", counters.rxMessagesDiscarded);
        printf("   Transmitted bytes          : %u\n", counters.txBytes);
        printf("   Transmitted fragments      : %u\n", counters.txFragments);
        printf("   Transmitted messages       : %u\n", counters.txMessages);
        printf("\n=====================================\n\n");
    }

    return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: getOmciVersionHandler
 * Description  : Processes the BCM_OMCI_IOC_GET_DRIVER_VERSION command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getOmciVersionHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    BCM_Gpon_DriverVersionInfo info;
    int ret = gponCtl_getOmciDriverVersion(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n======== OMCI Driver Version ========\n\n");
        printf("   Driver major: %d\n", info.driverMajor);
        printf("   Driver minor: %d\n", info.driverMinor);
        printf("   Driver fix  : %d\n", info.driverFix);
        printf("   API    major: %d\n", info.apiMajor);
        printf("   API    minor: %d\n", info.apiMinor);
        printf("\n======================================\n\n");
    }

    return ret;
}


/*
 *------------------------------------------------------------------------------
 * Function Name: getTcontCfgHandler
 * Description  : Processes the BCM_PLOAM_IOC_GET_TCONT_CFG command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int getTcontCfgHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, bIdx = 0;
    BCM_Ploam_TcontInfo info;

    memset(&info, 0, sizeof(info));

    // initialize neccessary fields

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--tcontIdx"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify TCONT index <0..%d>\n",
                        g_gponctl_PgmName,(BCM_PLOAM_NUM_DATA_TCONTS-1));
                return GPONCTL_INVALID_OPTION;
            }
            info.tcontIdx = strtoul(pOptions->pParms[0], NULL, 0);
            bIdx = 1;
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    /* Index is mandatory*/
    if (!bIdx)
    {
         fprintf(stderr, "%s: ERROR !! Missing TcontIdx mandatory parameter\n",
                g_gponctl_PgmName);
         return GPONCTL_INVALID_NUMBER_OF_OPTIONS;
    }

    ret = gponCtl_getTcontCfg(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n====== TCONT-Configuration =======\n\n");
        printf("   TCONT index             : %d\n", info.tcontIdx);
        printf("   AllocID                 : 0x%X\n",info.allocID);
        printf("   Enabled                 : %d\n",info.enabled);
        printf("   Ploam Created           : %d\n",info.ploamCreated);
        printf("   Ref Count               : 0x%X\n",info.refCount);
        printf("   OMCI  Created           : %d\n",info.omciCreated);
        printf("\n\n=====================================\n\n");
    }

  return ret;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: cfgTcontAllocIdHandler
 * Description  : Processes the BCM_PLOAM_IOC_CFG_TCONT_ALLOCID command.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int cfgTcontAllocIdHandler(PGPON_OPTION_INFO pOptions, int nNumOptions)
{
    int ret = 0, bIdx = 0, bSetValid = 0;
    BCM_Ploam_TcontAllocIdInfo info;

    memset(&info, 0, sizeof(info));

    // initialize neccessary fields

    while (nNumOptions)
    {
        if (!strcasecmp(pOptions->pOptName, "--tcontIdx"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify TCONT index <0..%d>\n",
                        g_gponctl_PgmName,(BCM_PLOAM_NUM_DATA_TCONTS-1));
                return GPONCTL_INVALID_OPTION;
            }
            info.tcontIdx = strtoul(pOptions->pParms[0], NULL, 0);
            bIdx = 1;
        }
        else if (!strcasecmp(pOptions->pOptName, "--allocId"))
        {
            if (pOptions->nNumParms != 1)
            {
                fprintf(stderr,
                        "%s: identify TCONT index <0..0x%X or 0x%X>\n",
                        g_gponctl_PgmName, BCM_PLOAM_MAX_XALLOC_ID, BCM_PLOAM_ALLOC_ID_UNASSIGNED);
                return GPONCTL_INVALID_OPTION;
            }
            info.allocID = strtoul(pOptions->pParms[0], NULL, 0);
            bSetValid = 1;
        }
        else    /* should never reach here */
        {
            fprintf(stderr, "%s: invalid option [%s]\n",
                    g_gponctl_PgmName, pOptions->pOptName);
            return GPONCTL_INVALID_OPTION;
        }

        nNumOptions--;
        pOptions++;
    }

    /* Index is mandatory*/
    if (!bIdx || !bSetValid)
    {
         fprintf(stderr, "%s: ERROR !! Missing mandatory parameters\n",
                g_gponctl_PgmName);
         return GPONCTL_INVALID_NUMBER_OF_OPTIONS;
    }

    ret = gponCtl_configTcontAllocId(&info);

    if (ret == GPONCTL_SUCCESS)
    {
        printf("\n====== TCONT-Configuration =======\n\n");
        printf("   TCONT index             : %d\n", info.tcontIdx);
        printf("   AllocID                 : 0x%X\n",info.allocID);
        printf("\n\n=====================================\n\n");
    }

  return ret;
}
