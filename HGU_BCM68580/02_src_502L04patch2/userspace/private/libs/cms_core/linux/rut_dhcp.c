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
 * rut_dhcp.c
 *
 *  Created on:  Sep. 2017
 *      Author: Fuguo Xu <fuguo.xu@broadcom.com>
 */



#include "cms_core.h"
#include "cms_util.h"
#include "rut_util.h"
#include "rut_dhcp.h"


#if defined(BRCM_PKTCBL_SUPPORT)
typedef struct {
    char *duid;    
}Option43InParms;

static int option43_getMacAddr(const void *parm, OUT char *string, INOUT int *len)
{
    int macOffset = 4;
    Option43InParms *InParm;

    /* TODO: tmp get from duid if it is not NULL */
    InParm = (Option43InParms *)parm;    
    if (NULL == InParm->duid)
        return -1;

    if ((strlen(InParm->duid) - macOffset) >= (*len))
    {
        cmsLog_error("str too long!");
        return -1;
    }

    *len = sprintf(string, &InParm->duid[macOffset]);
    return 0;
}                 

static DhcpSubOptionTable option43_subOptions[] = 
{
    { 2,   OPTION_CHAR_STRING, "dev_type",        "EMTA",               NULL },
    { 4,   OPTION_CHAR_STRING, "sn",              "001018b0ff00",       option_getSN },
    { 5,   OPTION_CHAR_STRING, "hw_ver",          "V1.0",               option_getHwV },
    { 6,   OPTION_CHAR_STRING, "sw_ver",          "5.02L.04",           option_getSwV },
    { 7,   OPTION_CHAR_STRING, "bootroom_ver",    "CFE=1.0",            option_getBtV },
    { 8,   OPTION_CHAR_STRING, "OUI",             "001018",             option_getOUI },
    { 9,   OPTION_CHAR_STRING, "model_num",       "968580XREF",         option_getModelNum },
    { 10,  OPTION_CHAR_STRING, "vendor_name",     "Broadcom",           option_getVdrName },
    { 31,  OPTION_HEX_STRING,  "mac",             "001018b0ff00",       option43_getMacAddr },
    { 32,  OPTION_HEX_STRING,  "id_correlation",  "1DA82FF9",           NULL }
};

CmsRet create_dhcpOption43(const char *ifName, const char *duid)
{
    char code = DHCP_VDR_SPECIFIC_INFO;
    char optionFrame[MAX_DHCP_OPTION_LEN] = {0};
    int totalLen = 0, dataLen = 0;
    Option43InParms parm;

    parm.duid = (char *)duid;
    if (CMSRET_SUCCESS != rutDhcp_encapsulateSubOption(code, option43_subOptions, 
        sizeof(option43_subOptions)/sizeof(DhcpSubOptionTable), (void *)&parm, 
        &optionFrame[VDR_OPTION_SUBCODE_OFFSET], &dataLen,
        OPTION_CODE_LEN1, OPTION_SIZE_LEN1))
    {
        return CMSRET_INTERNAL_ERROR;
    }

    optionFrame[VDR_OPTION_CODE_OFFSET] = code;
    optionFrame[VDR_OPTION_LEN_OFFSET] = dataLen;
    totalLen = VDR_OPTION_SUBCODE_OFFSET + dataLen;
    return rutDhcp_saveOption(DHCP_V4, ifName, code, optionFrame, totalLen);
}  

static int option60_getEndPointNum(const void * parm, OUT char* string, INOUT int * len)
{
    /* TODO:  hard code tmp */
    int cpyLen = 2;

    strncpy(string, "02", cpyLen);
    *len = cpyLen;

    return 0;
}

static int option60_getIfIndex(const void * parm, OUT char* string, INOUT int * len)
{
    /* TODO:  hard code tmp */
    int cpyLen = 2;

    strncpy(string, "09", cpyLen);
    *len = cpyLen;

    return 0;
}

/* For SubOption descripton, Pls refer to "PacketCable� 1.5 Specification":  "PKT-SP-PROV1.5-I04-090624.doc" */
static DhcpSubOptionTable option60_subOptions[] = {
    { 1,    OPTION_HEX_STRING,     "",      "02",             NULL },
    { 2,    OPTION_HEX_STRING,     "",      "02",             option60_getEndPointNum },
    { 3,    OPTION_HEX_STRING,     "",      "00",             NULL },
    { 4,    OPTION_HEX_STRING,     "",      "00",             NULL },
    { 9,    OPTION_HEX_STRING,     "",      "01" ,            NULL },
    { 11,   OPTION_HEX_STRING,     "",      "06090f",         NULL },
    { 12,   OPTION_HEX_STRING,     "",      "01",             NULL },
    { 13,   OPTION_HEX_STRING,     "",      "01",             NULL },
    { 15,   OPTION_HEX_STRING,     "",      "01",             NULL },
    { 16,   OPTION_HEX_STRING,     "",      "09",             option60_getIfIndex },
    { 18,   OPTION_HEX_STRING,     "",      "0007",           NULL },
    { 19,   OPTION_HEX_STRING,     "",      "01",             NULL },
    { 20,   OPTION_HEX_STRING,     "",      "01",             NULL },
    { 21,   OPTION_HEX_STRING,     "",      "01",             NULL },
    { 22,   OPTION_HEX_STRING,     "",      "01",             NULL },
    { 23,   OPTION_HEX_STRING,     "",      "02003f",         NULL },
    { 24,   OPTION_HEX_STRING,     "",      "00",             NULL },
    { 25,   OPTION_HEX_STRING,     "",      "00",             NULL },
    { 26,   OPTION_HEX_STRING,     "",      "00",             NULL },
    { 38,   OPTION_HEX_STRING,     "",      "01",             NULL }
};

/* to share the descripter with DHCPv6 option17->subOption35 */
int get_dhcpV4Option60DesPtr(OUT DhcpSubOptionTable **desPtr, OUT int *TableLen)
{
    *desPtr = option60_subOptions;
    *TableLen = sizeof(option60_subOptions)/sizeof(DhcpSubOptionTable);
    return 0;
}

CmsRet create_dhcpOption60(const char *ifName)
{
#define OPTION60_SUB_HEADER "pktc2.0:05"
#define OPTION60_SUB_LEN_SIZE  2
    char code = DHCP_VDR;
    char optionFrame[MAX_DHCP_OPTION_LEN] = {0};
    char dataFrame[MAX_DHCP_OPTION_LEN] = {0};
    int dataOffset, subLenOffset, dataLen = 0;
    int subHeaderLen = strlen(OPTION60_SUB_HEADER);
    int i, totalLen = 0;

    subLenOffset = VDR_OPTION_SUBCODE_OFFSET + subHeaderLen;
    dataOffset = subLenOffset + OPTION60_SUB_LEN_SIZE;
    if (CMSRET_SUCCESS != rutDhcp_encapsulateSubOption(code, option60_subOptions, 
        sizeof(option60_subOptions)/sizeof(DhcpSubOptionTable), NULL, 
        dataFrame, &dataLen,
        OPTION_CODE_LEN1, OPTION_SIZE_LEN1))
    {
        return CMSRET_INTERNAL_ERROR;
    }

    optionFrame[VDR_OPTION_CODE_OFFSET] = code;
    optionFrame[VDR_OPTION_LEN_OFFSET] = subHeaderLen + OPTION60_SUB_LEN_SIZE + dataLen * 2;
    strncpy(&optionFrame[VDR_OPTION_SUBCODE_OFFSET], OPTION60_SUB_HEADER, subHeaderLen);

    /* Hex to Hex ASCII string */
    sprintf(&optionFrame[subLenOffset], "%02x", (char)dataLen);
    for (i = 0; i < dataLen && i < MAX_DHCP_OPTION_LEN; i++)
    {
        sprintf(&optionFrame[dataOffset], "%02x", dataFrame[i]);
        dataOffset += 2;        
    }

    totalLen = VDR_OPTION_SUBCODE_OFFSET + optionFrame[VDR_OPTION_LEN_OFFSET];

    return rutDhcp_saveOption(DHCP_V4, ifName, code, optionFrame, totalLen);
}


#endif // BRCM_PKTCBL_SUPPORT

