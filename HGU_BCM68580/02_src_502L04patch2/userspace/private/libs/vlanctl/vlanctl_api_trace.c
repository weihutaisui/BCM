/******************************************************************************
 *
<:copyright-BRCM:2015:proprietary:standard

   Copyright (c) 2015 Broadcom 
   All Rights Reserved

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
******************************************************************************/
#include <stdio.h>
#include "cms.h"
#include "vlanctl_api.h"

#include "vlanctl_api_trace.h"
#include "cms_actionlog.h"

#define __print(fmt, arg...)                                            \
    do {                                                                \
        calLog_library(fmt, ##arg);                                     \
    } while(0)

#define __trace(cmd, fmt, arg...)                                       \
    do {                                                                \
        if(cmsActionTraceEnable_g)                                      \
            __print(#cmd " " fmt, ##arg);                               \
        else                                                            \
            return CMSRET_SUCCESS;                                      \
    } while(0)

#ifdef CMS_ACTION_LOG
static int cmsActionTraceEnable_g = 1;
#else
static int cmsActionTraceEnable_g = 0;
#endif

int vlanCtl_initTrace(void)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_init");

    return CMSRET_SUCCESS;
}

int vlanCtl_cleanupTrace(void)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cleanup");

    return CMSRET_SUCCESS;
}

int vlanCtl_createVlanInterfaceTrace(const char *realDevName, unsigned int vlanDevId,
                                int isRouted, int isMulticast)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_createVlanInterface");
    __trace(==>vlanctl, "--if-create %s %d %s %s\n", 
       realDevName, vlanDevId, isRouted?"--routed":"", isMulticast?"--mcast":"");

    return CMSRET_SUCCESS;
}

int vlanCtl_createVlanInterfaceByNameTrace(char *realDevName, char *vlanDevName,
                                      int isRouted, int isMulticast)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_createVlanInterfaceByName");
    __trace(==>vlanctl, "--if-create-name %s %s %s %s\n", 
       realDevName, vlanDevName, isRouted?"--routed":"", isMulticast?"--mcast":"");

    return CMSRET_SUCCESS;
}

int vlanCtl_deleteVlanInterfaceTrace(char *vlanDevName)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_deleteVlanInterface");
    __trace(==>vlanctl, "--if-delete %s\n", vlanDevName);

    return CMSRET_SUCCESS;
}

int vlanCtl_initTagRuleTrace(void)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_initTagRule");

    return CMSRET_SUCCESS;
}

int vlanCtl_insertTagRuleTrace(const char *realDevName, vlanCtl_direction_t tableDir, unsigned int nbrOfTags,
                          vlanCtl_ruleInsertPosition_t position, unsigned int posTagRuleId)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_insertTagRule");
    __trace(==>vlanctl, "--if %s --%s --tags %d\n", 
       realDevName, tableDir == VLANCTL_DIRECTION_RX ? "rx":"tx",
       nbrOfTags);
    if (position == VLANCTL_POSITION_APPEND)
        __trace(--->, "--rule-append\n");
    else if (position == VLANCTL_POSITION_LAST)
        __trace(--->, "--rule-insert-last\n");
    else if (position == VLANCTL_POSITION_BEFORE)
        __trace(--->, "--rule-insert-before %d\n", posTagRuleId);
    else if (position == VLANCTL_POSITION_AFTER)
        __trace(--->, "--rule-insert-after %d\n", posTagRuleId);

    return CMSRET_SUCCESS;
}

int vlanCtl_removeTagRuleTrace(char *realDevName, vlanCtl_direction_t tableDir,
                          unsigned int nbrOfTags, unsigned int tagRuleId)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_removeTagRule");
    __trace(==>vlanctl, "--if %s --%s --tags %d --rule-remove %d\n", 
       realDevName, tableDir == VLANCTL_DIRECTION_RX ? "rx":"tx",
       nbrOfTags, tagRuleId);

    return CMSRET_SUCCESS;
}

int vlanCtl_removeAllTagRuleTrace(char *vlanDevName)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_removeAllTagRule");
    __trace(==>vlanctl, "--rule-remove-all %s\n", vlanDevName);

    return CMSRET_SUCCESS;
}

int vlanCtl_removeTagRuleByFilterTrace(char *realDevName, vlanCtl_direction_t tableDir,
                                  unsigned int nbrOfTags)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_removeTagRuleByFilter");
    __trace(==>vlanctl, "--if %s --%s --tags %d\n", 
       realDevName, tableDir == VLANCTL_DIRECTION_RX ? "rx":"tx",
       nbrOfTags);
    __trace(--->, "--rule-remove-by-filter\n");
    return CMSRET_SUCCESS;
}

int vlanCtl_dumpRuleTableTrace(char *realDevName, vlanCtl_direction_t tableDir, unsigned int nbrOfTags)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_dumpRuleTable");
    __trace(==>vlanctl, "--if %s --%s --tags %d --show-table\n", 
       realDevName, tableDir == VLANCTL_DIRECTION_RX ? "rx":"tx",
       nbrOfTags);

    return CMSRET_SUCCESS;
}

int vlanCtl_getNbrOfRulesInTableTrace(char *realDevName, vlanCtl_direction_t tableDir, unsigned int nbrOfTags)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_getNbrOfRulesInTable");
    __trace(==>vlanctl, "--if %s --%s --tags %d --<cmd_not_support>\n", 
       realDevName, tableDir == VLANCTL_DIRECTION_RX ? "rx":"tx",
       nbrOfTags);

    return CMSRET_SUCCESS;
}

int vlanCtl_setDefaultVlanTagTrace(char *realDevName, vlanCtl_direction_t tableDir, unsigned int nbrOfTags,
                              unsigned int defaultTpid, unsigned int defaultPbits, unsigned int defaultCfi,
                              unsigned int defaultVid)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_setDefaultVlanTag");
    __trace(==>vlanctl, "--if %s --%s --tags %d", 
       realDevName, tableDir == VLANCTL_DIRECTION_RX ? "rx":"tx",
       nbrOfTags);
	if(cmsActionTraceEnable_g)
	{
	    if (defaultTpid != VLANCTL_DONT_CARE)
	        __print(" --default-tpid %d", defaultTpid);
	    if (defaultPbits != VLANCTL_DONT_CARE)
	        __print(" --default-pbits %d", defaultPbits);
	    if (defaultCfi != VLANCTL_DONT_CARE)
	        __print(" --default-cfi %d", defaultCfi);
	    if (defaultVid != VLANCTL_DONT_CARE)
	        __print(" --default-vid %d", defaultVid);
	    __print("\n");
	}

    return CMSRET_SUCCESS;
}

int vlanCtl_setDscpToPbitsTrace(char *realDevName, unsigned int dscp, unsigned int pbits)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_setDscpToPbits");
    __trace(==>vlanctl, "--if %s --cfg-dscp2pbits %d %d\n", 
       realDevName, dscp, pbits);

    return CMSRET_SUCCESS;
}

int vlanCtl_dumpDscpToPbitsTrace(char *realDevName, unsigned int dscp)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_dumpDscpToPbits");
    if (dscp == VLANCTL_DONT_CARE)
        __trace(==>vlanctl, "--if %s --show-dscp2pbits\n", realDevName);
    else
        __trace(==>vlanctl, "--if %s --dscp %d --<cmd_not support>\n", realDevName, dscp);

    return CMSRET_SUCCESS;
}

int vlanCtl_setTpidTableTrace(char *realDevName, unsigned int *tpidTable)
{
    int i;

    __trace(vlanctl_api, "%s\n", "vlanCtl_setTpidTable");
    __trace(==>vlanctl, "--if %s --cfg-tpid", realDevName);

	if(cmsActionTraceEnable_g) 
	{
	    for(i=0; i<BCM_VLAN_MAX_TPID_VALUES; ++i)
	    {
	        __print(" %d", tpidTable[i]);
	    }
	    __print("\n");
	}
    return CMSRET_SUCCESS;
}

int vlanCtl_dumpTpidTableTrace(char *realDevName)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_dumpTpidTable");
    __trace(==>vlanctl, "--if %s --show-tpid\n", realDevName);

    return CMSRET_SUCCESS;
}

int vlanCtl_dumpLocalStatsTrace(char *realDevName)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_dumpLocalStats");
    __trace(==>vlanctl, "--local-stats %s\n", realDevName);

    return CMSRET_SUCCESS;
}

int vlanCtl_setIfSuffixTrace(char *ifSuffix)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_setIfSuffix");
    __trace(==>vlanctl, "--if-suffix %s\n", ifSuffix);

    return CMSRET_SUCCESS;
}

int vlanCtl_setDefaultActionTrace(const char *realDevName, vlanCtl_direction_t tableDir, unsigned int nbrOfTags,
                             vlanCtl_defaultAction_t defaultAction, char *defaultRxVlanDevName)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_setDefaultAction");
	if( (tableDir == VLANCTL_DIRECTION_RX) && (defaultAction == VLANCTL_ACTION_ACCEPT)) 
	    __trace(==>vlanctl, "--if %s --rx --tags %d --default-miss-accept %s\n", realDevName, nbrOfTags, defaultRxVlanDevName);
    else
	    __trace(==>vlanctl, "--if %s --%s --tags %d --%s\n", 
	        realDevName, tableDir == VLANCTL_DIRECTION_RX ? "rx":"tx", nbrOfTags, 
	        defaultAction == VLANCTL_ACTION_ACCEPT ? "default-miss-accept":"default-miss-drop");

    return CMSRET_SUCCESS;
}

int vlanCtl_setRealDevModeTrace(char *realDevName, bcmVlan_realDevMode_t mode)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_setRealDevMode");
	__trace(==>vlanctl, "--if %s --%s\n", realDevName, mode == BCM_VLAN_MODE_ONT ? "set-if-mode-ont":"set-if-mode-rg");

    return CMSRET_SUCCESS;
}

int vlanCtl_createVlanFlowsTrace(char *rxVlanDevName, char *txVlanDevName)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_createVlanFlows");
	__trace(==>vlanctl, "--create-flows %s %s\n", rxVlanDevName, txVlanDevName);

    return CMSRET_SUCCESS;
}

int vlanCtl_deleteVlanFlowsTrace(char *rxVlanDevName, char *txVlanDevName)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_deleteVlanFlows");
	__trace(==>vlanctl, "--delete-flows %s %s\n", rxVlanDevName, txVlanDevName);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnSkbPriorityTrace(unsigned int priority)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnSkbPriority");
	__trace(###>, "--filter-skb-prio %d\n", priority);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnSkbMarkFlowIdTrace(unsigned int flowId)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnSkbMarkFlowId");
	__trace(###>, "--filter-skb-mark-flowid %d\n", flowId);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnSkbMarkPortTrace(unsigned int port)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnSkbMarkPort");
	__trace(###>, "--filter-skb-mark-port %d\n", port);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnEthertypeTrace(unsigned int etherType)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnEthertype");
	__trace(###>, "--filter-ethertype %#04x\n", etherType);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnIpProtoTrace(unsigned int ipProto)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnIpProto");
	__trace(###>, "--filter-ipproto %d\n", ipProto);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnDscpTrace(unsigned int dscp)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnDscp");
	__trace(###>, "--filter-dscp %d\n", dscp);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnDscp2PbitsTrace(unsigned int dscp2pbits)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnDscp2Pbits");
	__trace(###>, "--filter-dscp2pbits %d\n", dscp2pbits);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnVlanDeviceMacAddrTrace(unsigned int acceptMulticast)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnVlanDeviceMacAddr");
	__trace(###>, "--filter-vlan-dev-mac-addr %d\n", acceptMulticast);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnFlagsTrace(unsigned int flags)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnFlags");
	if(flags&BCM_VLAN_FILTER_FLAGS_IS_UNICAST)
	    __trace(###>, "--filter-unicast\n");
	if(flags&BCM_VLAN_FILTER_FLAGS_IS_MULTICAST)
	    __trace(###>, "--filter-multicast\n");
	if(flags&BCM_VLAN_FILTER_FLAGS_IS_BROADCAST)
	    __trace(###>, "--filter-broadcast\n");

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnTagPbitsTrace(unsigned int pbits, unsigned int tagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnTagPbits");
	__trace(###>, "--filter-pbits %d %d\n", pbits, tagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnTagCfiTrace(unsigned int cfi, unsigned int tagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnTagCfi");
	__trace(###>, "--filter-cfi %d %d\n", cfi, tagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnTagVidTrace(unsigned int vid, unsigned int tagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnTagVid");
	__trace(###>, "--filter-vid %d %d\n", vid, tagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnTagEtherTypeTrace(unsigned int etherType, unsigned int tagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnTagEtherType");
	__trace(###>, "--filter-tag-ethertype %#04x %d\n", etherType, tagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnRxRealDeviceTrace(char *realDevName)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnRxRealDevice");
	__trace(###>, "--filter-rxif %s\n", realDevName);

    return CMSRET_SUCCESS;
}

int vlanCtl_filterOnTxVlanDeviceTrace(char *vlanDevName)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_filterOnTxVlanDevice");
	__trace(###>, "--filter-txif %s\n", vlanDevName);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdPopVlanTagTrace(void)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdPopVlanTag");
	__trace(###>, "--pop-tag\n");

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdPushVlanTagTrace(void)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdPushVlanTag");
	__trace(###>, "--push-tag\n");

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdSetEtherTypeTrace(unsigned int etherType)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdSetEtherType");
	__trace(###>, "--set-ethertype %#04x\n", etherType);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdSetTagPbitsTrace(unsigned int pbits, unsigned int tagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdSetTagPbits");
	__trace(###>, "--set-pbits %d %d\n", pbits, tagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdSetTagCfiTrace(unsigned int cfi, unsigned int tagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdSetTagCfi");
	__trace(###>, "--set-cfi %d %d\n", cfi, tagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdSetTagVidTrace(unsigned int vid, unsigned int tagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdSetTagVid");
	__trace(###>, "--set-vid %d %d\n", vid, tagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdSetTagEtherTypeTrace(unsigned int etherType, unsigned int tagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdSetTagEtherType");
	__trace(###>, "--set-tag-ethertype %#04x %d\n", etherType, tagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdCopyTagPbitsTrace(unsigned int sourceTagIndex, unsigned int targetTagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdCopyTagPbits");
	__trace(###>, "--copy-pbits %d %d\n", sourceTagIndex, targetTagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdCopyTagCfiTrace(unsigned int sourceTagIndex, unsigned int targetTagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdCopyTagCfi");
	__trace(###>, "--copy-cfi %d %d\n", sourceTagIndex, targetTagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdCopyTagVidTrace(unsigned int sourceTagIndex, unsigned int targetTagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdCopyTagVid");
	__trace(###>, "--copy-vid %d %d\n", sourceTagIndex, targetTagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdCopyTagEtherTypeTrace(unsigned int sourceTagIndex, unsigned int targetTagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdCopyTagEtherType");
	__trace(###>, "--copy-tag-ethertype %d %d\n", sourceTagIndex, targetTagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdDscpToPbitsTrace(unsigned int tagIndex)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdDscpToPbits");
	__trace(###>, "--dscp2pbits %d\n", tagIndex);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdSetDscpTrace(unsigned int dscp)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdSetDscp");
	__trace(###>, "--set-dscp %d\n", dscp);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdDropFrameTrace(void)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdDropFrame");
	__trace(###>, "--drop-frame\n");

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdSetSkbPriorityTrace(unsigned int priority)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdSetSkbPriority");
	__trace(###>, "--set-skb-prio %d\n", priority);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdSetSkbMarkPortTrace(unsigned int port)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdSetSkbMarkPort");
	__trace(###>, "--set-skb-mark-port %d\n", port);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdSetSkbMarkQueueTrace(unsigned int queue)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdSetSkbMarkQueue");
	__trace(###>, "--set-skb-mark-queue %d\n", queue);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdSetSkbMarkQueueByPbitsTrace(void)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdSetSkbMarkQueueByPbits");
	__trace(###>, "--set-skb-mark-queue-by-pbits\n");

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdSetSkbMarkFlowIdTrace(unsigned int flowId)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdSetSkbMarkFlowId");
	__trace(###>, "--set-skb-mark-flowid %d\n", flowId);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdOvrdLearningVidTrace(unsigned int vid)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdOvrdLearningVid");
	__trace(###>, "--ovrd-learn-vid %d\n", vid);

    return CMSRET_SUCCESS;
}

int vlanCtl_cmdContinueTrace(void)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_cmdContinue");
	__trace(###>, "--continue\n");

    return CMSRET_SUCCESS;
}

int vlanCtl_setReceiveVlanDeviceTrace(char *vlanDevName)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_setReceiveVlanDevice");
	__trace(###>, "--set-rxif %s\n", vlanDevName);

    return CMSRET_SUCCESS;
}

int vlanCtl_setVlanRuleTableTypeTrace(unsigned int type)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_setVlanRuleTableType");
	__trace(###>, "--rule-type %d\n", type);

    return CMSRET_SUCCESS;
}

int vlanCtl_setDropPrecedenceTrace(bcmVlan_flowDir_t dir,
                              bcmVlan_dpCode_t dpCode)
{
    __trace(vlanctl_api, "%s\n", "vlanCtl_setDropPrecedence");
	__trace(###>, "--dir %d, --dpcode %d\n", dir, dpCode);

    return CMSRET_SUCCESS;
}


