/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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
#if defined(DMP_X_BROADCOM_COM_RDPA_1)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "httpd.h"
#include "cgi_main.h"
#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_dal.h"
#include "cgi_ingress_filters.h"
#include "rdpactl_api.h"
#include "ethswctl_api.h"

#define MAX_PORTS 64
#define FILTER_PORTS_STR_SIZE MAX_PORTS+1
#define MAX_PORTS_MASK_INDEX MAX_PORTS-1
#define PORTS_OUTPUT_STR_SIZE 1000
#define OUTPUT_STR_SIZE 5000

#define STRNCMP(src, dst) cmsUtl_strncmp(src, dst, strlen(dst))

int firstEntry;
int init; 
typedef struct {
	char ifname[CMS_IFNAME_LENGTH];
}protTranslation;

protTranslation allValidPorts[MAX_PORTS];

CmsRet getRpdaIfByIfname(const char* ifname, int* rdpaIf_p)
{
   int rc;


   rc = bcm_enet_get_rdpa_if_from_if_name(ifname, rdpaIf_p);
   if (rc)
      return CMSRET_INTERNAL_ERROR;

   return CMSRET_SUCCESS;

}



char* itoa(unsigned n, char *buf, unsigned buflen)
{
    unsigned i, out, res;

    if (buflen) {
        out = 0;
        if (sizeof(n) == 4)
            i = 1000000000;
#if UINT_MAX > 4294967295 /* prevents warning about "const too large" */
        else
        if (sizeof(n) == 8)
            i = 10000000000000000000;
#endif
        for (; i; i /= 10) {
            res = n / i;
            n = n % i;
            if (res || out || i == 1) {
                if (--buflen == 0)
                    break;
                out++;
                *buf++ = '0' + res;
            }
        }
    }
    return buf;
}

static void initValidInterfaces()
{
	char *ifNames=NULL;
	int rdpaIf;
	char ifNameBuf[CMS_IFNAME_LENGTH];
	int end,idx,c=0;
	CmsRet ret;

	memset(&allValidPorts, 0, sizeof(allValidPorts));
	cmsNet_getIfNameList(&ifNames);
	end = strlen(ifNames);

	while (c < end)
	{
		idx = 0;
		while (c < end && ifNames[c] != ',')
		{
			ifNameBuf[idx] = ifNames[c];
			c++;
			idx++;
		}
		ifNameBuf[idx] = 0;
		c++;

        if (!STRNCMP(ifNameBuf, "wl"))
        {
            cmsUtl_strncpy(allValidPorts[rdpa_if_wlan0].ifname,"wlan",5);
            continue; 
        }

		ret = getRpdaIfByIfname(ifNameBuf, &rdpaIf);
		if (ret == CMSRET_SUCCESS)
		{
			if (rdpaIf < 1 || rdpaIf > MAX_PORTS)
				continue; 
			cmsUtl_strncpy(allValidPorts[rdpaIf].ifname,ifNameBuf,(idx-1));
		}
	}
	init = 1;
	return;
}
static void mdmPortBitmask2Ifnames(char *bitmask, char *output)
{
	int i, first = 1;
    
	if (!init)
        initValidInterfaces();

    for (i = 0; i < MAX_PORTS; i++)
    {
        if (bitmask[MAX_PORTS_MASK_INDEX-i] == '0')
            continue;

		if (i==0)
		{
			cmsUtl_strcat(output, "wan0");
			first = 0;
			continue;
		}

		if (!first)
		{
			cmsUtl_strcat(output, ",");
		}
		cmsUtl_strcat(output, allValidPorts[i].ifname);
		first = 0;
	}
}

static void GetValidInterfaces(char *output)
{
	int i, not_first = 0;

	if (!init)
		initValidInterfaces();

	for (i = 0; i < MAX_PORTS; i++)
    {
        if ((!cmsUtl_strncmp(allValidPorts[i].ifname, "eth", 3)) || (!cmsUtl_strncmp(allValidPorts[i].ifname, "wl", 2)))
        {
            if (not_first++)
                strcat(output, ",");
            strcat(output, allValidPorts[i].ifname);
        }
        else
            continue;
    }
}

static int checkEntry(const char *type, const char *action, UINT32 val, int *index, InstanceIdStack *iisout)
{
    CmsRet ret=CMSRET_SUCCESS;
	InstanceIdStack iidStack;
    IngressFiltersDataObject *filterData = NULL;
	INIT_INSTANCE_ID_STACK(&iidStack);
	PUSH_INSTANCE_ID(&iidStack,1); 
	if (firstEntry)
	{
		cmsLck_releaseLock();
		firstEntry = 0;
	}
	cmsLck_acquireLock();


    /* Check if filters alredy configured or the first filter equal to type */
	if ((ret = cmsObj_get(MDMOID_INGRESS_FILTERS_DATA, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &filterData)) != CMSRET_SUCCESS)
	{
		cmsLck_releaseLock();
		return 0;
	}
    if ((!cmsUtl_strcmp(filterData->type, type) && val == 0 && !cmsUtl_strcmp(filterData->action, action)) ||
        (!cmsUtl_strcmp(filterData->type, type) && filterData->val == val && !cmsUtl_strcmp(filterData->action, action)))
    {
        *index = 1;
		memcpy(iisout, &iidStack, sizeof(InstanceIdStack));
        cmsObj_free((void **) &filterData);
		cmsLck_releaseLock();
        return 1;
    }

    *index = *index+1;

    /* Search the filter type */
    while ((ret = cmsObj_getNext(MDMOID_INGRESS_FILTERS_DATA, &iidStack, (void **) &filterData)) == CMSRET_SUCCESS)
    {
        if((!cmsUtl_strcmp(filterData->type, type) && val == 0 && !cmsUtl_strcmp(filterData->action, action)) ||
           (!cmsUtl_strcmp(filterData->type, type) && filterData->val == val && !cmsUtl_strcmp(filterData->action, action)))
        {
            cmsObj_free((void **) &filterData);
			cmsLck_releaseLock();
			memcpy(iisout, &iidStack, sizeof(InstanceIdStack));
            return 1;
        }
		cmsObj_free((void **) &filterData);
        *index = *index+1;
    }

    cmsObj_free((void **) &filterData);
	cmsLck_releaseLock();
    return 0;
}

static CmsRet filterEntryConfig(const char *type, UINT32 val, char *action, char *ports )
{
    CmsRet ret = CMSRET_SUCCESS;
    int index=1;
    int i=0;
	UINT64 ports_u = 0;
    IngressFiltersDataObject *filterData = NULL;

	cmsUtl_strtoul64(ports, NULL, 2, &ports_u);
	InstanceIdStack iidStack2;
    /* Get the valid index for this wan filter */
    i = checkEntry(type, action, val, &index, &iidStack2);
	InstanceIdStack iidStack;
	INIT_INSTANCE_ID_STACK(&iidStack);
	cmsLck_acquireLock();

	if (i)
	{
		PUSH_INSTANCE_ID(&iidStack, index); 
		if ((ret = cmsObj_get(MDMOID_INGRESS_FILTERS_DATA, &iidStack2, OGF_NORMAL_UPDATE, (void **) &filterData)) != CMSRET_SUCCESS)
		{
			cmsLck_releaseLock();
			return 0;
		}

		if (!ports_u)
		{
			if ((ret = cmsObj_deleteInstance(MDMOID_INGRESS_FILTERS_DATA, &iidStack2)) != CMSRET_SUCCESS)
			{
				cmsLog_error("Could not delete data object, ret=%d", ret);
			}
			goto exit;
		}
	}
    else
	{
		if (!ports_u)
				goto exit;

		if ((ret = cmsObj_addInstance(MDMOID_INGRESS_FILTERS_DATA, &iidStack)) != CMSRET_SUCCESS)
		{
			printf("%s@%d Add Obj Error=%d\n", __FUNCTION__, __LINE__, ret );
			goto exit;
		}
		if ((ret = cmsObj_get(MDMOID_INGRESS_FILTERS_DATA, &iidStack, OGF_NORMAL_UPDATE, (void **) &filterData)) != CMSRET_SUCCESS)
		{
            cmsLog_error("Could not get filter data object, ret=%d", ret);
			cmsLck_releaseLock();
			return 0;
		}
        filterData->type = cmsMem_strdup(type);
    }

        filterData->ports = cmsMem_strdup(ports);
        filterData->action = cmsMem_strdup(action);
        filterData->val = val;

		if (i)
		{
			if ((ret = cmsObj_set((void *)filterData, &iidStack2)) != CMSRET_SUCCESS)
				cmsLog_error("Could not set filter data object, ret=%d", ret);
		}
		else
		{
			if ((ret = cmsObj_set((void *)filterData, &iidStack)) != CMSRET_SUCCESS)
				cmsLog_error("Could not set filter data object, ret=%d", ret);
		}
exit:
	if(filterData)
    {
        cmsMgm_saveConfigToFlash();
		cmsObj_free((void **) &filterData);
    }
	cmsLck_releaseLock();
    return ret;
}

int portsActionSplit(char *line, int max_len)
{
    int i;

    if (!line[0])
        return 0;

    for (i = 0; i < max_len; i++)
    {
        if (line[i] == '%' && line[i+1] == '2' && line[i+2] == 'C')
            return i;
    
		if (line[i] == '&')
			return i;
    }
    return max_len;
}

void parsePortsActionStr(char *str,int len, char *ports)
{
   int start = 0, i, k;

   do
   {
	   k = portsActionSplit(str + start, len);
	   if (k <= 0)
		   break;
	   if (!STRNCMP((str+start),"wan0"))
		   ports[MAX_PORTS_MASK_INDEX] = '1';
	   for (i = 0; i < MAX_PORTS; i++)
	   {
		   if (!cmsUtl_strncmp(allValidPorts[i].ifname, (str+start), k))
			   ports[MAX_PORTS_MASK_INDEX - i] = '1';
	   }
       start = start + k + 3;
   } while (start < len);
}

static int isDropActionValid(char *action)
{
	if (!STRNCMP(action,MDMVS_FILTER_ICMPV6) || 
			!STRNCMP(action,MDMVS_FILTER_MLD) ||
			!STRNCMP(action,MDMVS_FILTER_DHCP) ||
			!STRNCMP(action,MDMVS_FILTER_IGMP))
		return 0;
	else
		return 1;
}

static int serializedTableLineHandler(char *lineStr, int len)
{
	long c = 0, value = 0, namestart, namelen, valstart, vallen;
	CmsRet ret = CMSRET_SUCCESS, dropRet = CMSRET_SUCCESS;
	char *action = NULL, *filterType = NULL, ports[MAX_PORTS + 1];
	UINT64 ports_u = 0;

	ports[MAX_PORTS] = '\0';

	while (c < len)
	{
        namestart = c;
		while (c < len && lineStr[c] != '=')
			c++;

		if (lineStr[c] == '\0')
			continue;

        namelen = c - namestart;
        c++;
        valstart = c;
		while (c < len && lineStr[c] != '&')
			c++;

		if (lineStr[c] == '\0')
			continue;

        vallen = c - valstart;
		c++; /* skip & */

		memset(ports, '0', MAX_PORTS);
		if (!STRNCMP(&lineStr[namestart], "filterType"))
			filterType = cmsMem_strndup(&lineStr[valstart], vallen);
		else if (!STRNCMP(&lineStr[namestart], "value"))
		{
			if (!STRNCMP(&lineStr[valstart], "none"))
				value = 0; 
			else
				value = strtol(&lineStr[valstart], NULL, 16); /* base 16 */
		}
		else if (!STRNCMP(&lineStr[namestart],MDMVS_FILTER_CPU))
		{
			action = cmsMem_strndup(MDMVS_FILTER_CPU,strlen(MDMVS_FILTER_CPU));
			parsePortsActionStr(&lineStr[valstart], vallen, ports);
			
			ret = filterEntryConfig(filterType, (UINT32)value, action, ports);		
		}
		else if (!STRNCMP(&lineStr[namestart],MDMVS_FILTER_DROP))
		{
			action = cmsMem_strndup(MDMVS_FILTER_DROP,strlen(MDMVS_FILTER_DROP));
			parsePortsActionStr(&lineStr[valstart], vallen, ports);
			
			cmsUtl_strtoul64(ports, NULL, 2, &ports_u);
			if (!isDropActionValid(filterType) && ports_u)
			{
				dropRet = CMSRET_INVALID_ARGUMENTS; 
				continue;
			}
			ret = filterEntryConfig(filterType, (UINT32)value, action, ports);
		}
		else if (!STRNCMP(&lineStr[namestart],MDMVS_FILTER_FORWARD))
		{
			action = cmsMem_strndup(MDMVS_FILTER_FORWARD,strlen(MDMVS_FILTER_FORWARD));
			parsePortsActionStr(&lineStr[valstart], vallen, ports);
			
			ret = filterEntryConfig(filterType, (UINT32)value, action, ports);
		}
		else
		{
			cmsLog_error("unknown name/val: %.s %.s", namelen, &lineStr[namestart], vallen, &lineStr[valstart]);
			ret = -1;
			goto Exit;
		}
	}

Exit:
	cmsMem_free(filterType);
	cmsMem_free(action);
	if(dropRet)
		ret = dropRet;
	if (ret)
		cmsLog_error("Could not parse/set ingressfilter query ret=%d", ret);

    return ret;
}

void filterEntryPortMaskBuilder(char *inputStr, char *outputPortMaskStr)
{
	int end,idx,i;
	int c = 0;
	char ifNameBuf[CMS_IFNAME_LENGTH];

	memset(outputPortMaskStr, '0', FILTER_PORTS_STR_SIZE);
	outputPortMaskStr[MAX_PORTS] = 0;
	end = strlen(inputStr);

	while (c < end)
	{
		idx = 0;
		while (c < end && inputStr[c] != ',')
		{
			ifNameBuf[idx] = inputStr[c];
			c++;
			idx++;
		}
		ifNameBuf[idx] = 0;
		c++;

		for (i = 0; i < MAX_PORTS; i++)
		{
			if (cmsUtl_strcmp(allValidPorts[i].ifname, ifNameBuf))
				continue;
			outputPortMaskStr[MAX_PORTS_MASK_INDEX-i] = '1';
		}
	}
}

static void strLineBuilder(char *outputStr, char *entryIndex, IngressFiltersDataObject *filterData )
{
	char valueBuf[16]={};

	/* Get the entry params  */
	cmsUtl_strcat(outputStr,"filterType" );
	cmsUtl_strcat(outputStr,entryIndex);
	cmsUtl_strcat(outputStr,"=" );
	cmsUtl_strcat(outputStr,filterData->type);
	cmsUtl_strcat(outputStr,"&" );
	cmsUtl_strcat(outputStr,"value" );
	cmsUtl_strcat(outputStr,entryIndex);
	cmsUtl_strcat(outputStr,"=" );
	if (filterData->val)
	{
		sprintf( valueBuf, "%x",filterData->val );
		cmsUtl_strcat(outputStr,valueBuf);
	}
	else
	{
		cmsUtl_strcat(outputStr,"none" );
	}
	cmsUtl_strcat(outputStr,"&" );
	cmsUtl_strcat(outputStr,filterData->action);
	cmsUtl_strcat(outputStr,entryIndex);
	cmsUtl_strcat(outputStr,"=" );
	mdmPortBitmask2Ifnames(filterData->ports,outputStr);
	cmsUtl_strcat(outputStr,"&" );
}

static int isValueFilter(char *filterType)
{
	if (!STRNCMP(filterType,MDMVS_FILTER_ETYPE_UDEF) || 
			!STRNCMP(filterType,MDMVS_FILTER_MAC_ADDR_OUI) ||
			!STRNCMP(filterType,MDMVS_FILTER_TPID))
		return 1;
	else
		return 0;
}

static CmsRet getEntriesStr(char* entriesStr)
{
    CmsRet ret = CMSRET_SUCCESS;
    int index=1, absoluteIndex=1, tempVal = 0;
	char *f, *r;
    char indexBuf[8]={};
    InstanceIdStack iidStack;
    IngressFiltersDataObject *filterData = NULL;
    INIT_INSTANCE_ID_STACK(&iidStack);
    PUSH_INSTANCE_ID(&iidStack,1);

    /* Check if filters alredy configured or the first filter equal to type */
    if ((ret = cmsObj_get(MDMOID_INGRESS_FILTERS_DATA, &iidStack, OGF_NO_VALUE_UPDATE, (void **) &filterData)) != CMSRET_SUCCESS)
    {
        return ret;
    }

	itoa(absoluteIndex,indexBuf, 2);
	strLineBuilder(entriesStr, indexBuf, filterData);    
	absoluteIndex++;

    /* Search the filter type */
    while ((ret = cmsObj_getNext(MDMOID_INGRESS_FILTERS_DATA, &iidStack, (void **) &filterData)) == CMSRET_SUCCESS)
	{
		/* In case that another entry with the same type exist only add the action and the ports */		
		f = strstr(entriesStr,filterData->type);

		/* Check if one of the multiple filters - ethrtype, oui and tpid  */
		if (f)
		{
			if (isValueFilter(filterData->type))
			{
				while (f)
				{
					f = f + strlen(filterData->type) + strlen("&value");
					r = f;
					f = strchr(f,'=');
					f++;
					tempVal = strtol(f, NULL, 10);
					if (filterData->val == (unsigned int)tempVal)
					{
						f = r;
						goto sameline;
					}
					else
						f = strstr(f,filterData->type);
				}
				goto newline;
			}

			f = f + strlen(filterData->type) + strlen("&value");
sameline:
			index = strtol(f, NULL, 10); /* base 10 */

			memset(indexBuf, 0, sizeof(indexBuf));
			itoa(index,indexBuf, 8);

			cmsUtl_strcat(entriesStr,filterData->action);
			cmsUtl_strcat(entriesStr,indexBuf);
			cmsUtl_strcat(entriesStr,"=");
			mdmPortBitmask2Ifnames(filterData->ports,entriesStr);
			cmsUtl_strcat(entriesStr,"&");
			continue;
		}
newline:
		memset(indexBuf, 0, sizeof(indexBuf));
		sprintf (indexBuf, "%d",absoluteIndex);
		strLineBuilder(entriesStr, indexBuf, filterData);        
        absoluteIndex++;
	}

	if(filterData)
		cmsObj_free((void **) &filterData);
	return CMSRET_SUCCESS;
}

int splitquery(char *query)
{
	char *end = query + strlen(query);
	char *f, *s = query;
    int ret = 0;
	
	while (s < end)
	{
		f = s;
		f = strstr(f, "filterType");
		if (!f)
			return 0;

		s = strstr(f + 1, "filterType");
		if (!s)
			s = end;

		if ((ret = serializedTableLineHandler(f, s - f)))
            return ret;
	}
	cmsLck_acquireLock();
	return 0;
}

void cgiSetIngressFiltersCfg(char *query, FILE *fs)
{
	firstEntry = 1;
    int ret = splitquery(query);
//	printf("cgiSetIngressFiltersCfg - %s \n", query);
	fprintf(fs, "%d", ret);
}

char ifNameBuf[CMS_IFNAME_LENGTH];
void cgiIngressFiltersSysPortsGet(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue)
{
	GetValidInterfaces(varValue);
	return ;
}

void cgiGetIngressFiltersCfg(int argc __attribute__((unused)), char **argv __attribute__((unused)), char *varValue)
{
	char outputStr[OUTPUT_STR_SIZE]={};
	getEntriesStr(outputStr);
//	printf("cgiGetIngressFiltersCfg - %s \n", outputStr);
	cmsLog_debug("Enter %s \n", outputStr);
	strcpy(varValue, outputStr);

	return;
}
#endif

