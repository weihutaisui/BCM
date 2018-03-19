/***********************************************************************
 *
 *  Copyright (c) 2017  Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:standard

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

/*
 * rut_dhcp_common.h
 *
 *  Created on:  Sep. 2017
 *      Author: Fuguo Xu <fuguo.xu@broadcom.com>
 */


/*
 * the defines in this file is used for both DHCPv4 and DHCPv6
 */


#ifndef __RUT_DHCP_COMMON_H__
#define __RUT_DHCP_COMMON_H__


/*
 * Parameter Tags indicating whether the parameter is an input, output, or input/output argument
 */
#ifndef IN
#define IN
#endif /*IN*/

#ifndef OUT
#define OUT
#endif /*OUT*/

#ifndef INOUT
#define INOUT
#endif /*INOUT*/


/* -------------------------------- define A begin   -------------------------------- */

/* Note: the define A should be consistent with the one in files: 
** userspace/gpl/apps/udhcp/x.h, and userspace/public/apps/dhcpv6/dhcpv6/common.c
** For private/gpls licens concern, cannot include same file directly.
*/

#define BRCM_UDHCPC_CONFIG_DIR       "/var/udhcpc"
#define BRCM_UDHCP6C_CONFIG_DIR       "/var/udhcp6c"
#define BRCM_UDHCPC_CONFIG_FILE_BASE_NAME "option"
#define BRCM_UDHCPC_CONFIG_IN_FILE_SUFFIX ".in"
#define BRCM_UDHCPC_CONFIG_OUT_FILE_SUFFIX ".out"
#define BRCM_UDHCPC_CONFIG_FILE_NAME_LEN 64


#define MAX_DHCP_OPTION_LEN      384  /* is enough for actual DHCPv6 case??  For DHCPv4, max len is 255  */
#define MAX_DHCP_SUB_OPTION_LEN  256

/* use differenct name */
#define VDR_OPTION_CODE_OFFSET         0
#define VDR_OPTION_LEN_OFFSET          1 /* DHCPv4 */
#define VDR_OPTION_V6_LEN_OFFSET       2 /* DHCPv6 */
#define VDR_OPTION_SUBCODE_OFFSET      2 /* DHCPv4 */
#define VDR_OPTION_V6_SUBCODE_OFFSET   4 /* DHCPv6 */

/* -------------------------------- define A  end  -------------------------------- */


typedef enum {
    OPTION_CHAR_STRING = 1,
    OPTION_HEX_STRING
} DhcpSubOptionValType;

typedef enum {
    DHCP_V4 = 1,
    DHCP_V6
} DhcpVersion;


/* DHCPv4 option codeLen=1 byte.
    DHCPv6 option codeLen=2 bytes, but some suboption codeLen=1 byte (i.e. option17->suboption35) */
typedef enum {
    OPTION_CODE_LEN1 = 1, /* 1 byte */
    OPTION_CODE_LEN2 = 2  /* 2 bytes */
} DhcpOptionCodeLen;

/* DHCPv4 option sizeLen=1 byte, DHCPv6 option sizeLen=2 bytes */
typedef enum {
    OPTION_SIZE_LEN1 = 1, /* 1 byte */
    OPTION_SIZE_LEN2 = 2  /* 2 bytes */
} DhcpOptionSizeLen;


typedef struct {
    uint16_t subCode;  /* type for both DHCPv4 and DHCPv6*/     
    char type;  
    char *name;   
    char *valDef; /* default value */
    int (*valFn)(IN const void * parm, OUT char* string, INOUT int * len); /* callback function, to update default value */
}DhcpSubOptionTable;


/** This function make the dir for saving config files. 
 *
 * @param dhcpVer  (IN) DHCPv4 or DHCPv6.
 * @param ifName  (IN)  interface name on which dhcpc is launched.
 *
 * @return CmsRet enum.
 */
CmsRet rutDhcp_mkCfgDir(IN DhcpVersion dhcpVer, const char *ifName);


/** This function read DHCP optionX string from optionX file. 
 *  optionX file example: /var/udhcpc/veip0.1/option122.out
 *
 * @param ifName  (IN)  interface name on which dhcpc is launched.
 * @param code  (IN)  option code
 * @param option  (OUT)  entire option string
 * @param len  (INOUT)  IN: option buffer(char *option) len, OUT: output option string len
 *
 * @return CmsRet enum.
 */
CmsRet rutDhcp_readOption(IN DhcpVersion dhcpVer, const char *ifName, int code, OUT char *option, INOUT int *len);


/** This function save DHCP optionX string to file, so that other application(dhcpc) can get it. 
 *  optionX file example: /var/udhcpc/veip0.1/option43.in, /var/udhcpc/veip0.1/option60.in
 *
 * @param ifName  (IN)  interface name on which dhcpc is launched.
 * @param code  (IN)  option code
 * @param option  (IN)  entire option string
 * @param len  (IN)  entire option string len
 *
 * @return CmsRet enum.
 */
CmsRet rutDhcp_saveOption(IN DhcpVersion dhcpVer, const char *ifName, int code, const char *option, int len);


/** This function encapsulate DHCP sub-options into packet based on TLV(type/length/value)
 *  Note: the function will use defaule sub-option value in TLV table if callback "valFn == NULL", 
 *  otherwise, the function will call valFn to update the sub-option value.
 *
 * @param code  (IN)  option code
 * @param subOptTable  (IN)  sub option TLV table
 * @param subOptTableLen  (IN)  sub option TLV table len
 * @param generalParm  (IN)  general params passeding to callback function "valFn",
 *                                          which can be a structure for multi parms.
 * @param optData  (OUT)  encapsulated sub-option string.
 * @param dataLen  (OUT)  encapsulated sub-option string len.
 * @param codeLen  (IN)  option code len.
 * @param sizeLen  (IN)   option size len.
 *
 * @return CmsRet enum.
 */
CmsRet rutDhcp_encapsulateSubOption(uint16_t code, IN DhcpSubOptionTable *subOptTable, 
                  int subOptTableLen, const void *generalParm, OUT char* optData, OUT int *dataLen,
                  IN const DhcpOptionCodeLen codeLen, IN const DhcpOptionSizeLen sizeLen);



#if defined(BRCM_PKTCBL_SUPPORT)

#define option_getDevInfoX(X, outStr, len)               \
do                                                       \
{                                                        \
    IGDDeviceInfoObject *deviceInfoObj=NULL;             \
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;  \
    CmsRet ret = 0;                                      \
                                                         \
    if ((ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **) &deviceInfoObj)) != CMSRET_SUCCESS) \
    {                                                                        \
        cmsLog_error("could not get device info object!, ret=%d", ret);      \
        return -1;                                                           \
    }                                                                        \
                                                                             \
    if (deviceInfoObj->X) {                                                  \
        if (strlen(deviceInfoObj->X) < (*len)) {                             \
            *len = sprintf(outStr, "%s", deviceInfoObj->X);                  \
        }                                                                    \
        else {                                                               \
            cmsLog_error("str too long!");                                   \
            ret = -1;                                                        \
        }                                                                    \
    }                                                                        \
                                                                             \
    cmsObj_free((void**)&deviceInfoObj);                                     \
    return ret;                                                              \
} while(0)  

static inline int option_getSN(const void *parm, OUT char *string, INOUT int *len)
{
    option_getDevInfoX(serialNumber, string, len);
}

static inline int option_getHwV(const void *parm, OUT char *string, INOUT int *len)
{
    option_getDevInfoX(hardwareVersion, string, len);
}

static inline int option_getSwV(const void *parm, OUT char *string, INOUT int *len)
{
    option_getDevInfoX(softwareVersion, string, len);
}

static inline int option_getBtV(const void *parm, OUT char *string, INOUT int *len)
{
    option_getDevInfoX(additionalSoftwareVersion, string, len);
}

static inline int option_getOUI(const void *parm, OUT char *string, INOUT int *len)
{
    option_getDevInfoX(manufacturerOUI, string, len);
}

static inline int option_getModelNum(const void *parm, OUT char *string, INOUT int *len)
{
    option_getDevInfoX(modelName, string, len);
}

static inline int option_getVdrName(const void *parm, OUT char *string, INOUT int *len)
{
    option_getDevInfoX(manufacturer, string, len);
}

#endif // BRCM_PKTCBL_SUPPORT

#endif // __RUT_DHCP_COMMON_H__

