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
 * rut_dhcp6.c
 *
 *  Created on:  Sep. 2017
 *      Author: Fuguo Xu <fuguo.xu@broadcom.com>
 */

#ifdef SUPPORT_IPV6

#include "cms_core.h"
#include "cms_util.h"
#include "rut_util.h"
#include "rut_dhcp6.h"



#if defined(BRCM_PKTCBL_SUPPORT)

extern int get_dhcpV4Option60DesPtr(OUT DhcpSubOptionTable **desPtr, OUT int *TableLen);


/* For option17->subOption35: TLV5, CL_OPTION_MODEM_CAPABILITIES. Pls refer to [CL-SP-CANN-DHCP-Reg-I14-170111.pdf] section 5.2.15 
** which has same format as DHCPv4 option60_subOptions.
*/
static DhcpSubOptionTable *option17_35_subOptions = NULL;

static int option17_35getTLV5(const void *parm, OUT char *string, INOUT int *len)
{
    uint16_t SubCode = 35;
    int i, dataOffset;
    char dataFrame[MAX_DHCP_OPTION_LEN] = {0};
    int TableLen, dataLen = 0;

    get_dhcpV4Option60DesPtr(&option17_35_subOptions, &TableLen);
    if(NULL == option17_35_subOptions)
    {
        cmsLog_error("get_dhcpV4Option60DesPtr failed!");
        return -1;
    }

    if (CMSRET_SUCCESS != rutDhcp_encapsulateSubOption(SubCode, option17_35_subOptions, 
        TableLen, NULL, dataFrame, &dataLen, 
        OPTION_CODE_LEN1, OPTION_SIZE_LEN1))
    {
        return CMSRET_INTERNAL_ERROR;
    }

    /* Hex to Hex ASCII string */
    if ((dataLen * 2) >= (*len))
    {
        cmsLog_error("str too long!");
        return -1;
    }
    dataOffset = 0;
    for (i = 0; i < dataLen && i < MAX_DHCP_OPTION_LEN; i++)
    {
        sprintf(&string[dataOffset], "%02x", dataFrame[i]);
        dataOffset += 2;
    }
    *len = dataLen * 2;

    return 0;
}


/* Please refer to DHCPv4: option43_getMacAddr() */
static int option17_getMacAddr(const void *parm, OUT char *string, INOUT int *len)
{
    /* TODO:: here returen (-1) to use default */
    return -1;
}

static DhcpSubOptionTable option17_subOptions[] = 
{
    { 1,    OPTION_HEX_STRING,  "oro",             "002000210022087b",   NULL }, /* option 32, 33, 34, 2171 */
    { 2,    OPTION_CHAR_STRING, "dev_type",        "EDVA",               NULL },
    { 4,    OPTION_CHAR_STRING, "sn",              "001018b0ff00",       option_getSN },
    { 5,    OPTION_CHAR_STRING, "hw_ver",          "V1.0",               option_getHwV },
    { 6,    OPTION_CHAR_STRING, "sw_ver",          "5.02L.04",           option_getSwV },
    { 7,    OPTION_CHAR_STRING, "bootroom_ver",    "CFE=1.0",            option_getBtV },
    { 8,    OPTION_CHAR_STRING, "OUI",             "001018",             option_getOUI },
    { 9,    OPTION_CHAR_STRING, "model_num",       "968580XREF",         option_getModelNum },
    { 10,   OPTION_CHAR_STRING, "vendor_name",     "Broadcom",           option_getVdrName },
    { 35,   OPTION_HEX_STRING,  "TLV5",            "00",                 option17_35getTLV5 },
    { 36,   OPTION_HEX_STRING,  "mac",             "001018b0ff00",       option17_getMacAddr },
    { 2172, OPTION_HEX_STRING,  "id_correlation",  "1DA82FF9",           NULL }
};

CmsRet create_dhcp6Option17(const char *ifName)
{
    uint16_t code = DHCP_V6_VDR_SPECIFIC_INFO;
    char optionFrame[MAX_DHCP_OPTION_LEN] = {0}, *buffPtr = NULL;
    int totalLen = 0, dataLen = 0;
    uint32_t enterprise_num = 4491; //Cable Television Laboratories, Inc. 

    buffPtr = &optionFrame[VDR_OPTION_CODE_OFFSET];
    *(uint16_t *)buffPtr = htons(code);
    buffPtr += 2;
    totalLen += 2;
    
    /* skip option len first */
    buffPtr += 2;
    totalLen += 2;
    
    *(uint32_t *)buffPtr = htonl(enterprise_num);
    buffPtr += 4;
    totalLen += 4;

    if (CMSRET_SUCCESS != rutDhcp_encapsulateSubOption(code, option17_subOptions, 
            sizeof(option17_subOptions)/sizeof(DhcpSubOptionTable), NULL, 
            buffPtr, &dataLen, OPTION_CODE_LEN2, OPTION_SIZE_LEN2))
    {
        return CMSRET_INTERNAL_ERROR;
    }

    buffPtr = &optionFrame[VDR_OPTION_V6_LEN_OFFSET]; 
    *(uint16_t *)buffPtr = htons(dataLen + 4); /* 4 for enterprise_num */
    
    totalLen += dataLen;
    return rutDhcp_saveOption(DHCP_V6, ifName, code, optionFrame, totalLen);
}  

#endif // BRCM_PKTCBL_SUPPORT

#endif // SUPPORT_IPV6
