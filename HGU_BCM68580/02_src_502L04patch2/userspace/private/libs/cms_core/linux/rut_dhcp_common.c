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
 * rut_dhcp_common.c
 *
 *  Created on:  Sep. 2017
 *      Author: Fuguo Xu <fuguo.xu@broadcom.com>
 */
 

/*
 * the function in this file is used for both DHCPv4 and DHCPv6
 */


#include "cms_core.h"
#include "cms_util.h"
#include "rut_util.h"
#include "rut_dhcp_common.h"
#include "dhcp_config_key.h"


/* For debug */
static void show_Option(const char *option, int len) 
{
    int i;

    for (i = 0; i < len; i++) 
    {
        printf("%02x ", option[i]);
        if ((i + 1) % 16 == 0) 
        {
            printf("\n");
        }
    }
    printf("\n\n");
}

CmsRet rutDhcp_mkCfgDir(IN DhcpVersion dhcpVer, const char *ifName)
{
    char cmd[BRCM_UDHCPC_CONFIG_FILE_NAME_LEN + 16];
    
    snprintf(cmd, sizeof(cmd), "mkdir -p %s/%s", 
        (DHCP_V4 == dhcpVer) ? BRCM_UDHCPC_CONFIG_DIR : BRCM_UDHCP6C_CONFIG_DIR, ifName);
    rut_doSystemAction("rutDhcp_mkCfgDir", (cmd));
    
    return CMSRET_SUCCESS;
}

CmsRet rutDhcp_readOption(IN DhcpVersion dhcpVer, const char *ifName, int code, OUT char *option, INOUT int *len)
{
    char file[BRCM_UDHCPC_CONFIG_FILE_NAME_LEN] = {0};
    char buffer[MAX_DHCP_OPTION_LEN] = {0}, *buffPtr = NULL;
    FILE *in;
    int rlen, cpLen, vlen = 0;
    CmsRet ret = CMSRET_SUCCESS;

    if (NULL == ifName || NULL == option || NULL == len)
    {
        cmsLog_error("param invalid!");
        return CMSRET_INVALID_ARGUMENTS;
    }

    /* Full name example: /var/udhcpc/veip0.1/option122.out */
    snprintf(file, sizeof(file), "%s/%s/%s%d%s", 
                   (DHCP_V4 == dhcpVer) ? BRCM_UDHCPC_CONFIG_DIR : BRCM_UDHCP6C_CONFIG_DIR, ifName, 
                    BRCM_UDHCPC_CONFIG_FILE_BASE_NAME, code,
                    BRCM_UDHCPC_CONFIG_OUT_FILE_SUFFIX); /* output file from dhcp */
    if (!(in = fopen(file, "r")))
    {
        cmsLog_error("unable to open option file: %s", file);
        return CMSRET_INTERNAL_ERROR;
    }

    rlen = fread(buffer, 1, MAX_DHCP_OPTION_LEN, in);

    /* decrypt */
    dhcpEncryptCfgFile(buffer, rlen, BRCM_DHCP_CONFIG_KEY);

    if (DHCP_V4 == dhcpVer)
        vlen = buffer[VDR_OPTION_LEN_OFFSET] + VDR_OPTION_SUBCODE_OFFSET;
    else if (DHCP_V6 == dhcpVer)
    {
        buffPtr = &buffer[VDR_OPTION_V6_LEN_OFFSET];
        vlen = ntohs(*((uint16_t *)buffPtr)) + VDR_OPTION_V6_SUBCODE_OFFSET;
    }

    /* verify */
    if (rlen != vlen)
    {
        cmsLog_error("Invalid option file: %s", file);
        ret = CMSRET_INTERNAL_ERROR;
        goto exit;
    }

    if(cmsLog_getLevel() == LOG_LEVEL_DEBUG) 
    {
        cmsLog_debug("Option%d:\n", code);
        show_Option(buffer, rlen);
    }

    cpLen = rlen < (*len) ? rlen : (*len);
    memcpy(option, buffer, cpLen);
    *len = cpLen;

exit:
    fclose(in);
    return ret;
}

CmsRet rutDhcp_saveOption(IN DhcpVersion dhcpVer, const char *ifName, int code, const char *option, int len)
{
    char file[BRCM_UDHCPC_CONFIG_FILE_NAME_LEN] = {0};
    char buffer[MAX_DHCP_OPTION_LEN] = {0};
    FILE *out;
    CmsRet ret = CMSRET_SUCCESS;
    unsigned int cnt = 1;
    int wLen = len < MAX_DHCP_OPTION_LEN ? len : MAX_DHCP_OPTION_LEN;

    if(cmsLog_getLevel() == LOG_LEVEL_DEBUG) 
    {
        cmsLog_debug("Option%d:\n", code);
        show_Option(option, wLen);
    }

    if(cmsLog_getLevel() == LOG_LEVEL_DEBUG)
    {
        cmsLog_debug("Option%d:\n", code);
        show_Option(option, len);
    }

    /* encrypt */
    memcpy(buffer, option, wLen);
    dhcpEncryptCfgFile(buffer, wLen, BRCM_DHCP_CONFIG_KEY);

    /* Full name example: /var/udhcpc/veip0.1/option43.in */
    snprintf(file, sizeof(file), "%s/%s/%s%d%s", 
                   (DHCP_V4 == dhcpVer) ? BRCM_UDHCPC_CONFIG_DIR : BRCM_UDHCP6C_CONFIG_DIR, ifName, 
                    BRCM_UDHCPC_CONFIG_FILE_BASE_NAME, code,
                    BRCM_UDHCPC_CONFIG_IN_FILE_SUFFIX); /* input file for dhcp */
    if (!(out = fopen(file, "w"))) 
    {
        cmsLog_error("unable to open option file: %s", file);
        return CMSRET_INTERNAL_ERROR;
    }

    if (fwrite(buffer, wLen, cnt, out) != cnt)
    {
        cmsLog_error("write option file: %s fail", file);
        ret = CMSRET_INTERNAL_ERROR;
        goto exit;
    }

exit:
    fclose(out);
    return ret;
}

CmsRet rutDhcp_encapsulateSubOption(uint16_t code, IN DhcpSubOptionTable *subOptTable, 
                  int subOptTableLen, const void *generalParm, OUT char* optData, OUT int *dataLen,
                  IN const DhcpOptionCodeLen codeLen, IN const DhcpOptionSizeLen sizeLen)
{
    char *dataPtr, *subCodeLenPtr, *valPtr;
    uint16_t sub_code;
    int subLen = 0, totalLen = 0, cnt = 0;
    int loop, value, i;
    char type;
    char valNew[MAX_DHCP_SUB_OPTION_LEN] = {0};
    CmsRet ret = CMSRET_SUCCESS;

    dataPtr = optData;
    for(loop = 0; loop < subOptTableLen; loop++)
    {
        sub_code = subOptTable[loop].subCode;
        type = subOptTable[loop].type; 
        if (OPTION_CODE_LEN1  == codeLen)
            *dataPtr++ = sub_code;
        else if (OPTION_CODE_LEN2  == codeLen)
        {
            *((uint16_t *)dataPtr) = htons(sub_code);
            dataPtr += 2;
        }
        else
        {
            cmsLog_error("Unsupported codeLen=%d\n", codeLen);
            ret = CMSRET_INTERNAL_ERROR;
            goto exit;
        }

        if (OPTION_SIZE_LEN1 == sizeLen)
            subCodeLenPtr = dataPtr++;
        else if (OPTION_SIZE_LEN2 == sizeLen)
        {
            subCodeLenPtr = dataPtr;
            dataPtr += 2;
        }
        else
        {
            cmsLog_error("Unsupported sizeLen=%d\n", sizeLen);
            ret = CMSRET_INTERNAL_ERROR;
            goto exit;
        } 

        if (NULL == subOptTable[loop].valFn)
            valPtr = subOptTable[loop].valDef;
        else
        {
            int len = sizeof(valNew);
            memset(valNew, 0, len);
            if (!subOptTable[loop].valFn(generalParm, valNew, &len))
            {
                valPtr = valNew;
            }
            else
            {
                cmsLog_error("option %d, subOption %d: update value fail!", code, sub_code);
                valPtr = subOptTable[loop].valDef;
            }
        }         
        
        if (OPTION_CHAR_STRING == type)
        {
            subLen = strlen(valPtr);
            strncpy(dataPtr, valPtr, subLen);
            dataPtr += subLen;
        }
        else if(OPTION_HEX_STRING == type) 
        {
            cnt = 0;
            while ( sscanf(valPtr, "%2x%n", &value, &i) == 1 )
            {
                *dataPtr++ = value;
                valPtr += i;
                cnt++;
            }
            subLen = cnt;
        }
        else
        {
            cmsLog_error("Unsupported type %d\n", type);
            ret = CMSRET_INTERNAL_ERROR;
            goto exit;
        }

        if (OPTION_SIZE_LEN1 == sizeLen)            
            *subCodeLenPtr = subLen;
        else if (OPTION_SIZE_LEN2 == sizeLen)
            *(uint16_t *)subCodeLenPtr = htons(subLen);        
        
        totalLen += codeLen + sizeLen + subLen;
    }

    *dataLen = totalLen;

exit:
    if (ret)
    {
        cmsLog_error("Encapsulate fail! code = %d\n", code);
    }
    return ret;    
}


