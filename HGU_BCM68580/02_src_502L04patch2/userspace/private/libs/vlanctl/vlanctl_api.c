/*
<:copyright-BRCM:2007:proprietary:standard

   Copyright (c) 2007 Broadcom 
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

#include "vlanctl_api.h"
#include "vlanctl_api_trace.h"


/*
 * Macros
 */
#define VLANCTL_IOCTL_FILE_NAME "/dev/bcmvlan"

#define VLANCTL_DEV_NAME_IS_VALID(_name) ( strlen(_name) < BCM_VLAN_IFNAMSIZ )

#ifdef DESKTOP_LINUX
#define _BCM_VLAN_CMD_SET_VAL(arg, val)                         \
{                                                               \
    do                                                          \
    {                                                           \
        VLANCTL_LOG_DEBUG("SET_VAL: 0x%x\n", val);              \
    } while (0);                                                \
    BCM_VLAN_CMD_SET_VAL(arg, val);                             \
}
#else /* DESKTOP_LINUX */
#define _BCM_VLAN_CMD_SET_VAL BCM_VLAN_CMD_SET_VAL
#endif /* DESKTOP_LINUX */

#ifdef DESKTOP_LINUX
static int vlanctl_dev_open(const char *name, int flags);
static int vlanctl_dev_ioctl(int fd, int request, void *argp, ...);
static int vlanctl_dev_close(int fd);
#else /* DESKTOP_LINUX */
#define vlanctl_dev_open  open
#define vlanctl_dev_ioctl ioctl
#define vlanctl_dev_close close
#endif /* DESKTOP_LINUX */

/*
 * Local variables
 */
static int vlanCtl_fileDesc;

#ifndef DESKTOP_LINUX
static vlanCtl_logLevel_t vlanCtl_logLevel = VLANCTL_LOG_LEVEL_ERROR;
#else /* DESKTOP_LINUX */
static vlanCtl_logLevel_t vlanCtl_logLevel = VLANCTL_LOG_LEVEL_DEBUG;
#endif /* DESKTOP_LINUX */

static bcmVlan_tagRule_t vlanCtl_tagRule;
static int vlanCtl_cmdIndex;

static unsigned int vlanCtl_cmdUsage;
#define VLANCTL_CMD_USAGE_PUSH    (1 << 0)
#define VLANCTL_CMD_USAGE_POP     (1 << 1)
#define VLANCTL_CMD_USAGE_OTHERS  (1 << 2)
#if defined(CONFIG_BCM_VLAN_AGGREGATION)
#define VLANCTL_CMD_USAGE_DEAGGR     (1 << 3)
#endif


/*
 * Private functions
 */

#ifdef DESKTOP_LINUX

static int vlanctl_dev_open(const char *name, int flags)
{
    VLANCTL_LOG_INFO("vlanctl_open(%s,0x%x)\n", name, flags);
    return 0;
}


static int vlanctl_dev_ioctl(int fd, int request, void *argp, ...)
{
    VLANCTL_LOG_INFO("vlanctl_ioctl(0x%x, 0x%x, 0x%p)", fd, request, argp);
    return 0;
}


static int vlanctl_dev_close(int fd)
{
    VLANCTL_LOG_INFO("vlanctl_close(0x%x)", fd);
    return 0;
}

#endif /* DESKTOP_LINUX */


static inline int tagIndexCheck(unsigned int tagIndex)
{
    if(tagIndex >= BCM_VLAN_MAX_TAGS)
    {
        VLANCTL_LOG_ERROR("Invalid tag number: %d (max = %d)", tagIndex, BCM_VLAN_MAX_TAGS);

        return -1;
    }

    return 0;
}

static inline int cmdIndexCheck(void)
{
    if(vlanCtl_cmdIndex >= BCM_VLAN_MAX_RULE_COMMANDS)
    {
        VLANCTL_LOG_ERROR("Maximum number of Commands reached (%d)", BCM_VLAN_MAX_RULE_COMMANDS);

        return -1;
    }

    return 0;
}


/*
 * Public functions
 */

int vlanCtl_init(void)
{
    int ret = 0;

    vlanCtl_initTrace();

    vlanCtl_fileDesc = vlanctl_dev_open(VLANCTL_IOCTL_FILE_NAME, O_RDWR);
    if(vlanCtl_fileDesc < 0)
    {
        VLANCTL_LOG_ERROR("%s: %s", VLANCTL_IOCTL_FILE_NAME, strerror(errno));

        ret = -EINVAL;
    }

    return ret;
}

void vlanCtl_cleanup(void)
{
    vlanCtl_cleanupTrace();

    vlanctl_dev_close(vlanCtl_fileDesc);
}

int vlanCtl_setLogLevel(vlanCtl_logLevel_t logLevel)
{
    if(logLevel > VLANCTL_LOG_LEVEL_MAX)
    {
        VLANCTL_LOG_ERROR("Invalid Log Level: %d (allowed values are 0 to %d)", logLevel, VLANCTL_LOG_LEVEL_MAX-1);

        return -EINVAL;
    }

    vlanCtl_logLevel = logLevel;

    return 0;
}

vlanCtl_logLevel_t vlanCtl_getLogLevel(void)
{
    return vlanCtl_logLevel;
}

int vlanCtl_logLevelIsEnabled(vlanCtl_logLevel_t logLevel)
{
    VLANCTL_ASSERT(logLevel >= 0 && logLevel < VLANCTL_LOG_LEVEL_MAX);

    if(vlanCtl_logLevel >= logLevel)
    {
        return 1;
    }

    return 0;
}

/**************************
 ** vlanCtl Commands API **
 **************************/

int vlanCtl_createVlanInterface(const char *realDevName, unsigned int vlanDevId,
                                int isRouted, int isMulticast)
{
    int ret;
    bcmVlan_iocCreateVlan_t iocCreateVlan;

    vlanCtl_createVlanInterfaceTrace(realDevName, vlanDevId, isRouted, isMulticast);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocCreateVlan.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);
    iocCreateVlan.vlanDevId = vlanDevId;
    iocCreateVlan.isRouted = isRouted;
    iocCreateVlan.isMulticast = isMulticast;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_CREATE_VLAN, &iocCreateVlan);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_createVlanInterfaceByName(char *realDevName, char *vlanDevName,
                                      int isRouted, int isMulticast)
{
    int ret;
    bcmVlan_iocCreateVlanByName_t iocCreateVlan;

    vlanCtl_createVlanInterfaceByNameTrace(realDevName, vlanDevName, isRouted, isMulticast);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }
    if(!VLANCTL_DEV_NAME_IS_VALID(vlanDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, vlanDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocCreateVlan.realDevName, realDevName, sizeof(iocCreateVlan.realDevName));
    strncpy(iocCreateVlan.vlanDevName, vlanDevName, sizeof(iocCreateVlan.vlanDevName));
    iocCreateVlan.isRouted = isRouted;
    iocCreateVlan.isMulticast = isMulticast;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_CREATE_VLAN_BY_NAME, &iocCreateVlan);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_deleteVlanInterface(char *vlanDevName)
{
    int ret;
    bcmVlan_iocDeleteVlan_t iocDeleteVlan;

    vlanCtl_deleteVlanInterfaceTrace(vlanDevName);

    if(!VLANCTL_DEV_NAME_IS_VALID(vlanDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, vlanDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocDeleteVlan.vlanDevName, vlanDevName, BCM_VLAN_IFNAMSIZ);

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_DELETE_VLAN, &iocDeleteVlan);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

void vlanCtl_initTagRule(void)
{
    vlanCtl_initTagRuleTrace();

    bcmVlan_initTagRule(&vlanCtl_tagRule);
    vlanCtl_cmdIndex = 0;
    vlanCtl_cmdUsage = 0;
}

int vlanCtl_insertTagRule(const char *realDevName, vlanCtl_direction_t tableDir, unsigned int nbrOfTags,
                          vlanCtl_ruleInsertPosition_t position, unsigned int posTagRuleId,
                          unsigned int *tagRuleId)
{
    int ret;
    bcmVlan_iocInsertTagRule_t iocInsertTagRule;

    vlanCtl_insertTagRuleTrace(realDevName, tableDir, nbrOfTags, position, posTagRuleId);

    VLANCTL_ASSERT(tagRuleId);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    if(tableDir == BCM_VLAN_TABLE_DIR_TX &&
       vlanCtl_tagRule.filter.vlanDevMacAddr != BCM_VLAN_MATCH_NO_VLANDEV_MACADDR)
    {
        VLANCTL_LOG_ERROR("--filter-vlan-dev-mac-addr only applies to rx tag rules");

        ret = -EINVAL;
        goto out;
    }
    
    if(tableDir == BCM_VLAN_TABLE_DIR_RX &&
       (vlanCtl_tagRule.filter.rxRealDevName[0] != '\0' ||
        strcmp(vlanCtl_tagRule.filter.txVlanDevName, BCM_VLAN_DEFAULT_DEV_NAME) != 0))
    {
        VLANCTL_LOG_ERROR("--filter-rxif and --filter-txif only apply to tx tag rules");

        ret = -EINVAL;
        goto out;
    }
    
    strncpy(iocInsertTagRule.ruleTableId.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);
    iocInsertTagRule.ruleTableId.tableDir = tableDir;
    iocInsertTagRule.ruleTableId.nbrOfTags = nbrOfTags;
    iocInsertTagRule.tagRule = vlanCtl_tagRule;
    iocInsertTagRule.position = position;
    iocInsertTagRule.posTagRuleId = posTagRuleId;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_INSERT_TAG_RULE, &iocInsertTagRule);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }
    else
    {
        *tagRuleId = iocInsertTagRule.tagRule.id;

        printf("Created new Tag Rule: dev=%s, dir=%d, tags=%d, id=%d\n\n",
               realDevName, tableDir, nbrOfTags, iocInsertTagRule.tagRule.id);
    }

out:
    return ret;
}
int vlanCtl_removeTagRule(char *realDevName, vlanCtl_direction_t tableDir,
                          unsigned int nbrOfTags, unsigned int tagRuleId)
{
    int ret;
    bcmVlan_iocRemoveTagRule_t iocRemoveTagRule;

    vlanCtl_removeTagRuleTrace(realDevName, tableDir, nbrOfTags, tagRuleId);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocRemoveTagRule.ruleTableId.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);
    iocRemoveTagRule.ruleTableId.tableDir  = tableDir;
    iocRemoveTagRule.ruleTableId.nbrOfTags = nbrOfTags;
    iocRemoveTagRule.tagRuleId = tagRuleId;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_REMOVE_TAG_RULE, &iocRemoveTagRule);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_removeAllTagRule(char *vlanDevName)
{
    int ret;
    bcmVlan_iocRemoveAllTagRule_t iocRemoveAllTagRule;

    vlanCtl_removeAllTagRuleTrace(vlanDevName);

    if(!VLANCTL_DEV_NAME_IS_VALID(vlanDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): <%s>", 
                          BCM_VLAN_IFNAMSIZ, vlanDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocRemoveAllTagRule.vlanDevName, vlanDevName, BCM_VLAN_IFNAMSIZ);

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_REMOVE_ALL_TAG_RULE, &iocRemoveAllTagRule);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_removeTagRuleByFilter(char *realDevName, vlanCtl_direction_t tableDir,
                                  unsigned int nbrOfTags)
{
    int ret;
    bcmVlan_iocRemoveTagRule_t iocRemoveTagRule;

    vlanCtl_removeTagRuleByFilterTrace(realDevName, tableDir, nbrOfTags);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocRemoveTagRule.ruleTableId.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);
    iocRemoveTagRule.ruleTableId.tableDir  = tableDir;
    iocRemoveTagRule.ruleTableId.nbrOfTags = nbrOfTags;
    iocRemoveTagRule.tagRuleId = BCM_VLAN_DONT_CARE;
    iocRemoveTagRule.tagRule = vlanCtl_tagRule;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_REMOVE_TAG_RULE, &iocRemoveTagRule);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_dumpRuleTable(char *realDevName, vlanCtl_direction_t tableDir, unsigned int nbrOfTags)
{
    int ret;
    bcmVlan_iocDumpRuleTable_t iocDumpRuleTable;

    vlanCtl_dumpRuleTableTrace(realDevName, tableDir, nbrOfTags);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocDumpRuleTable.ruleTableId.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);
    iocDumpRuleTable.ruleTableId.tableDir  = tableDir;
    iocDumpRuleTable.ruleTableId.nbrOfTags = nbrOfTags;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_DUMP_RULE_TABLE, &iocDumpRuleTable);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_getNbrOfRulesInTable(char *realDevName, vlanCtl_direction_t tableDir,
                                 unsigned int nbrOfTags, unsigned int *nbrOfRules)
{
    int ret;
    bcmVlan_iocGetNbrOfRulesInTable_t iocGetNbrOfRulesInTable;

    vlanCtl_getNbrOfRulesInTableTrace(realDevName, tableDir, nbrOfTags);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocGetNbrOfRulesInTable.ruleTableId.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);
    iocGetNbrOfRulesInTable.ruleTableId.tableDir  = tableDir;
    iocGetNbrOfRulesInTable.ruleTableId.nbrOfTags = nbrOfTags;
    iocGetNbrOfRulesInTable.nbrOfRules = 0;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_GET_NBR_OF_RULES_IN_TABLE, &iocGetNbrOfRulesInTable);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }
    else
        *nbrOfRules = iocGetNbrOfRulesInTable.nbrOfRules;

out:
    return ret;
}

int vlanCtl_setDefaultVlanTag(char *realDevName, vlanCtl_direction_t tableDir, unsigned int nbrOfTags,
                              unsigned int defaultTpid, unsigned int defaultPbits, unsigned int defaultCfi,
                              unsigned int defaultVid)
{
    int ret;
    bcmVlan_iocSetDefaultVlanTag_t iocSetDefaultVlanTag;

    vlanCtl_setDefaultVlanTagTrace(realDevName, tableDir, nbrOfTags, 
                  defaultTpid, defaultPbits, defaultCfi, defaultVid);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocSetDefaultVlanTag.ruleTableId.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);
    iocSetDefaultVlanTag.ruleTableId.tableDir = tableDir;
    iocSetDefaultVlanTag.ruleTableId.nbrOfTags = nbrOfTags;
    iocSetDefaultVlanTag.defaultTpid = defaultTpid;
    iocSetDefaultVlanTag.defaultPbits = defaultPbits;
    iocSetDefaultVlanTag.defaultCfi = defaultCfi;
    iocSetDefaultVlanTag.defaultVid = defaultVid;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_SET_DEFAULT_TAG, &iocSetDefaultVlanTag);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_setDscpToPbits(char *realDevName, unsigned int dscp, unsigned int pbits)
{
    int ret;
    bcmVlan_iocDscpToPbits_t iocDscpToPbits;

    vlanCtl_setDscpToPbitsTrace(realDevName, dscp, pbits);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocDscpToPbits.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);
    iocDscpToPbits.dscp = dscp;
    iocDscpToPbits.pbits = pbits;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_SET_DSCP_TO_PBITS, &iocDscpToPbits);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_dumpDscpToPbits(char *realDevName, unsigned int dscp)
{
    int ret;
    bcmVlan_iocDscpToPbits_t iocDscpToPbits;

    vlanCtl_dumpDscpToPbitsTrace(realDevName, dscp);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocDscpToPbits.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);
    iocDscpToPbits.dscp = dscp;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_DUMP_DSCP_TO_PBITS, &iocDscpToPbits);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_setTpidTable(char *realDevName, unsigned int *tpidTable)
{
    int ret;
    int i;
    bcmVlan_iocSetTpidTable_t iocSetTpidTable;

    vlanCtl_setTpidTableTrace(realDevName, tpidTable);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocSetTpidTable.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);

    for(i=0; i<BCM_VLAN_MAX_TPID_VALUES; ++i)
    {
        iocSetTpidTable.tpidTable[i] = tpidTable[i];
    }

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_SET_TPID_TABLE, &iocSetTpidTable);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_dumpTpidTable(char *realDevName)
{
    int ret;
    bcmVlan_iocDumpTpidTable_t iocDumpTpidTable;

    vlanCtl_dumpTpidTableTrace(realDevName);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocDumpTpidTable.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_DUMP_TPID_TABLE, &iocDumpTpidTable);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_dumpLocalStats(char *realDevName)
{
    int ret;
    bcmVlan_iocDumpLocalStats_t iocDumpLocalStats;

    vlanCtl_dumpLocalStatsTrace(realDevName);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocDumpLocalStats.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_DUMP_LOCAL_STATS, &iocDumpLocalStats);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_setIfSuffix(char *ifSuffix)
{
    int ret;
    bcmVlan_iocSetIfSuffix_t iocSetIfSuffix;

    vlanCtl_setIfSuffixTrace(ifSuffix);

    if(strlen(ifSuffix) >= BCM_VLAN_IF_SUFFIX_SIZE)
    {
        VLANCTL_LOG_ERROR("Interface suffix is too long (max %d characters): %s",
                          BCM_VLAN_IF_SUFFIX_SIZE-1, ifSuffix);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocSetIfSuffix.suffix, ifSuffix, BCM_VLAN_IF_SUFFIX_SIZE);

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_SET_IF_SUFFIX, &iocSetIfSuffix);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_setDefaultAction(const char *realDevName, vlanCtl_direction_t tableDir, unsigned int nbrOfTags,
                             vlanCtl_defaultAction_t defaultAction, char *defaultRxVlanDevName)
{
    int ret;
    bcmVlan_iocSetDefaultAction_t iocSetDefaultAction;

    vlanCtl_setDefaultActionTrace(realDevName, tableDir, nbrOfTags, defaultAction, defaultRxVlanDevName);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s",
                          BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    if(tableDir == VLANCTL_DIRECTION_RX && defaultAction == VLANCTL_ACTION_ACCEPT)
    {
        if(!VLANCTL_DEV_NAME_IS_VALID(defaultRxVlanDevName))
        {
            VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s",
                              BCM_VLAN_IFNAMSIZ, defaultRxVlanDevName);

            ret = -EINVAL;
            goto out;
        }

        strncpy(iocSetDefaultAction.defaultRxVlanDevName, defaultRxVlanDevName, BCM_VLAN_IFNAMSIZ);
    }
    else
    {
        /* defaultRxVlanDevName is not needed (may be NULL) */

        strncpy(iocSetDefaultAction.defaultRxVlanDevName, "DONTCARE", BCM_VLAN_IFNAMSIZ);
    }

    strncpy(iocSetDefaultAction.ruleTableId.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);
    iocSetDefaultAction.ruleTableId.tableDir  = tableDir;
    iocSetDefaultAction.ruleTableId.nbrOfTags = nbrOfTags;
    iocSetDefaultAction.defaultAction = defaultAction;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_SET_DEFAULT_ACTION, &iocSetDefaultAction);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_setRealDevMode(char *realDevName, bcmVlan_realDevMode_t mode)
{
    int ret;
    bcmVlan_iocSetRealDevMode_t iocSetRealDevMode;

    vlanCtl_setRealDevModeTrace(realDevName, mode);

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocSetRealDevMode.realDevName, realDevName, BCM_VLAN_IFNAMSIZ);
    iocSetRealDevMode.mode = mode;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_SET_REAL_DEV_MODE, &iocSetRealDevMode);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_createVlanFlows(char *rxVlanDevName, char *txVlanDevName)
{
    int ret;
    bcmVlan_iocVlanFlows_t iocVlanFlows;

    vlanCtl_createVlanFlowsTrace(rxVlanDevName, txVlanDevName);

    if(!VLANCTL_DEV_NAME_IS_VALID(rxVlanDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, rxVlanDevName);

        ret = -EINVAL;
        goto out;
    }
    if(!VLANCTL_DEV_NAME_IS_VALID(txVlanDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, txVlanDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(iocVlanFlows.rxVlanDevName, rxVlanDevName, sizeof(iocVlanFlows.rxVlanDevName));
    strncpy(iocVlanFlows.txVlanDevName, txVlanDevName, sizeof(iocVlanFlows.txVlanDevName));

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_CREATE_VLAN_FLOWS, &iocVlanFlows);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}

int vlanCtl_deleteVlanFlows(char *rxVlanDevName, char *txVlanDevName)
{
    int ret;
    bcmVlan_iocVlanFlows_t iocVlanFlows;

    vlanCtl_deleteVlanFlowsTrace(rxVlanDevName, txVlanDevName);

    if(!VLANCTL_DEV_NAME_IS_VALID(rxVlanDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, rxVlanDevName);
        ret = -EINVAL;
        goto out;
    }
    if(!VLANCTL_DEV_NAME_IS_VALID(txVlanDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, txVlanDevName);
        ret = -EINVAL;
        goto out;
    }
    strncpy(iocVlanFlows.rxVlanDevName, rxVlanDevName, sizeof(iocVlanFlows.rxVlanDevName));
    strncpy(iocVlanFlows.txVlanDevName, txVlanDevName, sizeof(iocVlanFlows.txVlanDevName));
    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_DELETE_VLAN_FLOWS, &iocVlanFlows);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }
out:
    return ret;
}
/*******************
 ** Unit Test     **
 *******************/

int vlanCtl_runTest(unsigned int testNbr, char *rxVlanDevName, char *txVlanDevName)
{
    int ret;
    bcmVlan_iocRunTest_t iocRunTest;

    if(!VLANCTL_DEV_NAME_IS_VALID(rxVlanDevName))
    {
        VLANCTL_LOG_ERROR("Rx VLAN If name is too long (max %d characters): %s",
                          BCM_VLAN_IFNAMSIZ, rxVlanDevName);

        ret = -EINVAL;
        goto out;
    }

    if(!VLANCTL_DEV_NAME_IS_VALID(txVlanDevName))
    {
        VLANCTL_LOG_ERROR("Tx VLAN If name is too long (max %d characters): %s",
                          BCM_VLAN_IFNAMSIZ, txVlanDevName);

        ret = -EINVAL;
        goto out;
    }

    iocRunTest.testNbr = testNbr;
    strncpy(iocRunTest.rxVlanDevName, rxVlanDevName, BCM_VLAN_IFNAMSIZ);
    strncpy(iocRunTest.txVlanDevName, txVlanDevName, BCM_VLAN_IFNAMSIZ);

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_RUN_TEST, &iocRunTest);
    if(ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}


/**************************
 ** Tag Rule Filters API **
 **************************/

#define VLANCTL_CHECK_CMD_USAGE_VOID()                                  \
    do {                                                                \
        if(vlanCtl_cmdUsage) {                                          \
            VLANCTL_LOG_ERROR("Filters MUST be specified before commands"); \
            return;                                                     \
        }                                                               \
    } while(0)

#define VLANCTL_CHECK_CMD_USAGE()                                       \
    do {                                                                \
        if(vlanCtl_cmdUsage) {                                          \
            VLANCTL_LOG_ERROR("Filters MUST be specified before commands"); \
            return -EINVAL;                                             \
        }                                                               \
    } while(0)

void vlanCtl_filterOnSkbPriority(unsigned int priority)
{
    vlanCtl_filterOnSkbPriorityTrace(priority);

    VLANCTL_CHECK_CMD_USAGE_VOID();

    vlanCtl_tagRule.filter.skbPrio = priority;
}

void vlanCtl_filterOnSkbMarkFlowId(unsigned int flowId)
{
    vlanCtl_filterOnSkbMarkFlowIdTrace(flowId);

    VLANCTL_CHECK_CMD_USAGE_VOID();

    vlanCtl_tagRule.filter.skbMarkFlowId = flowId;
}

void vlanCtl_filterOnSkbMarkPort(unsigned int port)
{
    vlanCtl_filterOnSkbMarkPortTrace(port);

    VLANCTL_CHECK_CMD_USAGE_VOID();

    vlanCtl_tagRule.filter.skbMarkPort = port;
}

void vlanCtl_filterOnEthertype(unsigned int etherType)
{
    vlanCtl_filterOnEthertypeTrace(etherType);

    VLANCTL_CHECK_CMD_USAGE_VOID();

    vlanCtl_tagRule.filter.etherType = etherType;
}

void vlanCtl_filterOnIpProto(unsigned int ipProto)
{
    vlanCtl_filterOnIpProtoTrace(ipProto);

    VLANCTL_CHECK_CMD_USAGE_VOID();

    vlanCtl_tagRule.filter.ipProto = ipProto;
}

void vlanCtl_filterOnDscp(unsigned int dscp)
{
    vlanCtl_filterOnDscpTrace(dscp);

    VLANCTL_CHECK_CMD_USAGE_VOID();

    vlanCtl_tagRule.filter.dscp = dscp;
}

void vlanCtl_filterOnDscp2Pbits(unsigned int dscp2pbits)
{
    vlanCtl_filterOnDscp2PbitsTrace(dscp2pbits);

    VLANCTL_CHECK_CMD_USAGE_VOID();

    vlanCtl_tagRule.filter.dscp2pbits = dscp2pbits;
}

void vlanCtl_filterOnVlanDeviceMacAddr(unsigned int acceptMulticast)
{
    vlanCtl_filterOnVlanDeviceMacAddrTrace(acceptMulticast);

    VLANCTL_CHECK_CMD_USAGE_VOID();

    if (acceptMulticast)
    {
        vlanCtl_tagRule.filter.vlanDevMacAddr = BCM_VLAN_MATCH_VLANDEV_MACADDR_OR_MULTICAST;
    }
    else
    {
        vlanCtl_tagRule.filter.vlanDevMacAddr = BCM_VLAN_MATCH_VLANDEV_MACADDR;
    }
}

int vlanCtl_filterOnFlags(unsigned int flags)
{
    int ret = 0;

    vlanCtl_filterOnFlagsTrace(flags);

    VLANCTL_CHECK_CMD_USAGE();

    if(BCM_VLAN_FILTER_FLAGS_ARE_INVALID(flags))
    {
        VLANCTL_LOG_ERROR("Invalid Filter flags: 0x%08X", flags);

        ret = -EINVAL;
        goto out;
    }

    vlanCtl_tagRule.filter.flags |= flags;

out:
    return ret;
}

int vlanCtl_filterOnTagPbits(unsigned int pbits, unsigned int tagIndex)
{
    int ret;

    vlanCtl_filterOnTagPbitsTrace(pbits, tagIndex);

    VLANCTL_CHECK_CMD_USAGE();

    ret = tagIndexCheck(tagIndex);
    if(!ret)
    {
        vlanCtl_tagRule.filter.vlanTag[tagIndex].pbits = pbits;
    }

    return ret;
}

int vlanCtl_filterOnTagCfi(unsigned int cfi, unsigned int tagIndex)
{
    int ret;

    vlanCtl_filterOnTagCfiTrace(cfi, tagIndex);

    VLANCTL_CHECK_CMD_USAGE();

    ret = tagIndexCheck(tagIndex);
    if(!ret)
    {
        vlanCtl_tagRule.filter.vlanTag[tagIndex].cfi = cfi;
    }

    return ret;
}

int vlanCtl_filterOnTagVid(unsigned int vid, unsigned int tagIndex)
{
    int ret;

    vlanCtl_filterOnTagVidTrace(vid, tagIndex);

    VLANCTL_CHECK_CMD_USAGE();

    ret = tagIndexCheck(tagIndex);
    if(!ret)
    {
        vlanCtl_tagRule.filter.vlanTag[tagIndex].vid = vid;
    }

    return ret;
}

int vlanCtl_filterOnTagEtherType(unsigned int etherType, unsigned int tagIndex)
{
    int ret;

    vlanCtl_filterOnTagEtherTypeTrace(etherType, tagIndex);

    VLANCTL_CHECK_CMD_USAGE();

    ret = tagIndexCheck(tagIndex);
    if(!ret)
    {
        vlanCtl_tagRule.filter.vlanTag[tagIndex].etherType = etherType;
    }

    return ret;
}

int vlanCtl_filterOnRxRealDevice(char *realDevName)
{
    int ret = 0;

    vlanCtl_filterOnRxRealDeviceTrace(realDevName);

    VLANCTL_CHECK_CMD_USAGE();

    if(!VLANCTL_DEV_NAME_IS_VALID(realDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, realDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(vlanCtl_tagRule.filter.rxRealDevName, realDevName, BCM_VLAN_IFNAMSIZ);

out:
    return ret;
}

int vlanCtl_filterOnTxVlanDevice(char *vlanDevName)
{
    int ret = 0;

    vlanCtl_filterOnTxVlanDeviceTrace(vlanDevName);

    VLANCTL_CHECK_CMD_USAGE();

    if(!VLANCTL_DEV_NAME_IS_VALID(vlanDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, vlanDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(vlanCtl_tagRule.filter.txVlanDevName, vlanDevName, BCM_VLAN_IFNAMSIZ);

out:
    return ret;
}

/***************************
 ** Tag Rule Commands API **
 ***************************/

#define VLANCTL_SET_CMD_USAGE(_type)                                    \
    do {                                                                \
        vlanCtl_cmdUsage |= (_type);                                    \
        VLANCTL_LOG_DEBUG("Command Usage : 0x%02X\n", (_type));         \
    } while(0)

int vlanCtl_cmdPopVlanTag(void)
{
    int ret;

    vlanCtl_cmdPopVlanTagTrace();

    ret = cmdIndexCheck();
    if(!ret)
    {
        if(vlanCtl_cmdUsage & VLANCTL_CMD_USAGE_OTHERS)
        {
            VLANCTL_LOG_ERROR("Pop commands MUST be specified before any other command types");

            ret = -EINVAL;
            goto out;
        }
        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_POP);

        vlanCtl_tagRule.cmd[vlanCtl_cmdIndex].opCode = BCM_VLAN_OPCODE_POP_TAG;

        vlanCtl_cmdIndex++;
    }

out:
    return ret;
}

int vlanCtl_cmdPushVlanTag(void)
{
    int ret;

    vlanCtl_cmdPushVlanTagTrace();

    ret = cmdIndexCheck();
    if(!ret)
    {
        if(vlanCtl_cmdUsage & VLANCTL_CMD_USAGE_OTHERS)
        {
            VLANCTL_LOG_ERROR("Push commands MUST be specified before any other command types");

            ret = -EINVAL;
            goto out;
        }
        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_PUSH);

        vlanCtl_tagRule.cmd[vlanCtl_cmdIndex].opCode = BCM_VLAN_OPCODE_PUSH_TAG;

        vlanCtl_cmdIndex++;
    }

out:
    return ret;
}

#if defined(CONFIG_BCM_VLAN_AGGREGATION)
int vlanCtl_cmdDeaggrVlanTag(void)
{
    int ret;
    ret = cmdIndexCheck();
    if(!ret)
    {
        if(vlanCtl_cmdUsage & VLANCTL_CMD_USAGE_OTHERS)
        {
            VLANCTL_LOG_ERROR("Pop commands MUST be specified before any other command types");
            ret = -EINVAL;
            goto out;
        }
        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_DEAGGR);
        vlanCtl_tagRule.cmd[vlanCtl_cmdIndex].opCode = BCM_VLAN_OPCODE_DEAGGR_TAG;
        vlanCtl_cmdIndex++;
    }
out:
    return ret;
}
#endif
int vlanCtl_cmdSetEtherType(unsigned int etherType)
{
    int ret;

    vlanCtl_cmdSetEtherTypeTrace(etherType);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_SET_ETHERTYPE;

        _BCM_VLAN_CMD_SET_VAL(cmd->arg, etherType);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdSetTagPbits(unsigned int pbits, unsigned int tagIndex)
{
    int ret;

    vlanCtl_cmdSetTagPbitsTrace(pbits, tagIndex);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_SET_PBITS;

        _BCM_VLAN_CMD_SET_VAL(cmd->arg, pbits);

        BCM_VLAN_CMD_SET_TARGET_TAG(cmd->arg, tagIndex);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdSetTagCfi(unsigned int cfi, unsigned int tagIndex)
{
    int ret;

    vlanCtl_cmdSetTagCfiTrace(cfi, tagIndex);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_SET_CFI;

        _BCM_VLAN_CMD_SET_VAL(cmd->arg, cfi);

        BCM_VLAN_CMD_SET_TARGET_TAG(cmd->arg, tagIndex);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdSetTagVid(unsigned int vid, unsigned int tagIndex)
{
    int ret;

    vlanCtl_cmdSetTagVidTrace(vid, tagIndex);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_SET_VID;

        _BCM_VLAN_CMD_SET_VAL(cmd->arg, vid);

        BCM_VLAN_CMD_SET_TARGET_TAG(cmd->arg, tagIndex);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdSetTagEtherType(unsigned int etherType, unsigned int tagIndex)
{
    int ret;

    vlanCtl_cmdSetTagEtherTypeTrace(etherType, tagIndex);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_SET_TAG_ETHERTYPE;

        _BCM_VLAN_CMD_SET_VAL(cmd->arg, etherType);

        BCM_VLAN_CMD_SET_TARGET_TAG(cmd->arg, tagIndex);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdCopyTagPbits(unsigned int sourceTagIndex, unsigned int targetTagIndex)
{
    int ret;

    vlanCtl_cmdCopyTagPbitsTrace(sourceTagIndex, targetTagIndex);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_COPY_PBITS;

        BCM_VLAN_CMD_SET_SOURCE_TAG(cmd->arg, sourceTagIndex);

        BCM_VLAN_CMD_SET_TARGET_TAG(cmd->arg, targetTagIndex);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdCopyTagCfi(unsigned int sourceTagIndex, unsigned int targetTagIndex)
{
    int ret;

    vlanCtl_cmdCopyTagCfiTrace(sourceTagIndex, targetTagIndex);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_COPY_CFI;

        BCM_VLAN_CMD_SET_SOURCE_TAG(cmd->arg, sourceTagIndex);

        BCM_VLAN_CMD_SET_TARGET_TAG(cmd->arg, targetTagIndex);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdCopyTagVid(unsigned int sourceTagIndex, unsigned int targetTagIndex)
{
    int ret;

    vlanCtl_cmdCopyTagVidTrace(sourceTagIndex, targetTagIndex);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_COPY_VID;

        BCM_VLAN_CMD_SET_SOURCE_TAG(cmd->arg, sourceTagIndex);

        BCM_VLAN_CMD_SET_TARGET_TAG(cmd->arg, targetTagIndex);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdCopyTagEtherType(unsigned int sourceTagIndex, unsigned int targetTagIndex)
{
    int ret;

    vlanCtl_cmdCopyTagEtherTypeTrace(sourceTagIndex, targetTagIndex);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_COPY_TAG_ETHERTYPE;

        BCM_VLAN_CMD_SET_SOURCE_TAG(cmd->arg, sourceTagIndex);

        BCM_VLAN_CMD_SET_TARGET_TAG(cmd->arg, targetTagIndex);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdDscpToPbits(unsigned int tagIndex)
{
    int ret;

    vlanCtl_cmdDscpToPbitsTrace(tagIndex);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_DSCP2PBITS;

        BCM_VLAN_CMD_SET_TARGET_TAG(cmd->arg, tagIndex);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdSetDscp(unsigned int dscp)
{
    int ret;

    vlanCtl_cmdSetDscpTrace(dscp);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_SET_DSCP;

        _BCM_VLAN_CMD_SET_VAL(cmd->arg, dscp);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdDropFrame(void)
{
    int ret;

    vlanCtl_cmdDropFrameTrace();

    ret = cmdIndexCheck();
    if(!ret)
    {
        vlanCtl_tagRule.cmd[vlanCtl_cmdIndex].opCode = BCM_VLAN_OPCODE_DROP_FRAME;

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdSetSkbPriority(unsigned int priority)
{
    int ret;

    vlanCtl_cmdSetSkbPriorityTrace(priority);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_SET_SKB_PRIO;

        _BCM_VLAN_CMD_SET_VAL(cmd->arg, priority);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdSetSkbMarkPort(unsigned int port)
{
    int ret;

    vlanCtl_cmdSetSkbMarkPortTrace(port);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_SET_SKB_MARK_PORT;

        _BCM_VLAN_CMD_SET_VAL(cmd->arg, port);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdSetSkbMarkQueue(unsigned int queue)
{
    int ret;

    vlanCtl_cmdSetSkbMarkQueueTrace(queue);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_SET_SKB_MARK_QUEUE;

        _BCM_VLAN_CMD_SET_VAL(cmd->arg, queue);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdSetSkbMarkQueueByPbits(void)
{
    int ret;

    vlanCtl_cmdSetSkbMarkQueueByPbitsTrace();

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_SET_SKB_MARK_QUEUE_BYPBITS;

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdSetSkbMarkFlowId(unsigned int flowId)
{
    int ret;

    vlanCtl_cmdSetSkbMarkFlowIdTrace(flowId);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_SET_SKB_MARK_FLOWID;

        _BCM_VLAN_CMD_SET_VAL(cmd->arg, flowId);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

int vlanCtl_cmdOvrdLearningVid(unsigned int vid)
{
    int ret;

    vlanCtl_cmdOvrdLearningVidTrace(vid);

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_OVRD_LEARNING_VID;

        _BCM_VLAN_CMD_SET_VAL(cmd->arg, vid);

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}


int vlanCtl_cmdContinue(void)
{
    int ret;

    vlanCtl_cmdContinueTrace();

    ret = cmdIndexCheck();
    if(!ret)
    {
        bcmVlan_tagRuleCommand_t *cmd = &vlanCtl_tagRule.cmd[vlanCtl_cmdIndex];

        cmd->opCode = BCM_VLAN_OPCODE_CONTINUE;

        vlanCtl_cmdIndex++;

        VLANCTL_SET_CMD_USAGE(VLANCTL_CMD_USAGE_OTHERS);
    }

    return ret;
}

/*******************
 ** Miscellaneous **
 *******************/

int vlanCtl_setReceiveVlanDevice(char *vlanDevName)
{
    int ret = 0;

    vlanCtl_setReceiveVlanDeviceTrace(vlanDevName);

    if(!VLANCTL_DEV_NAME_IS_VALID(vlanDevName))
    {
        VLANCTL_LOG_ERROR("Interface name is too long (max %d characters): %s", BCM_VLAN_IFNAMSIZ, vlanDevName);

        ret = -EINVAL;
        goto out;
    }

    strncpy(vlanCtl_tagRule.rxVlanDevName, vlanDevName, BCM_VLAN_IFNAMSIZ);

out:
    return ret;
}

int vlanCtl_setVlanRuleTableType(unsigned int type)
{
    int ret = 0;

    vlanCtl_setVlanRuleTableTypeTrace(type);

    vlanCtl_tagRule.type = type;
    return ret;
}

int vlanCtl_setDropPrecedence(bcmVlan_flowDir_t dir, bcmVlan_dpCode_t dpCode)
{
    bcmVlan_iocSetDropPrecedence_t iocDp;
    int ret = 0;

    vlanCtl_setDropPrecedenceTrace(dir, dpCode);

    iocDp.dir = dir;
    iocDp.dpCode = dpCode;

    ret = vlanctl_dev_ioctl(vlanCtl_fileDesc, BCM_VLAN_IOC_SET_DP, &iocDp);
    if (ret)
    {
        VLANCTL_LOG_ERROR("%s", strerror(errno));
    }

out:
    return ret;
}
