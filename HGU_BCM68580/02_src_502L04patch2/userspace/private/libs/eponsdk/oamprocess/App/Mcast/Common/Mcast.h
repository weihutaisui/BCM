/*
<:copyright-BRCM:2002-2003:proprietary:epon

   Copyright (c) 2002-2003 Broadcom 
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
:>*/
#if !defined(Mcast_h)
#define Mcast_h
////////////////////////////////////////////////////////////////////////////////
/// \file Mcast.h
////////////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus)
extern "C" {
#endif
#ifdef BRCM_CMS_BUILD
#include "cms_core.h"
#endif
#include "Stream.h"
#include "cms_util.h"
#include "bcm_mcast_api.h"
#include "vlanctl_api.h"
#include "EponDevInfo.h"
#include "rdpactl_api.h"
#include "bcm_common_llist.h"
#include "ethswctl_api.h"
#include "UniConfigDb.h"
#include <bcm_local_kernel_include/linux/sockios.h>
#include <bridgeutil.h>
#include <string.h>
#include "McastVlan.h"

#define MAX_MCASTS_PER_PORT 32
#define MAX_MCAST_SWITCHING_ENTRIES_PER_PORT 8
#define MAX_MCAST_FDB_ENTRIES (MAX_MCASTS_PER_PORT * MAX_UNI_PORTS)
#define MAX_GROUPS_DEF_VALUE 4  
#define IF_NAME_SIZE  16

#define INVALID_INDEX 0xFFFF
#define MAX_MC_GROUP_PORT_LIST_STR_LEN   128

#define McastUserVidToPort(vid)  ((U8)(((vid)&0xFFF)-1))     
#define McastPortToUserVid(port) ((U16)((port) + 1)) 


typedef enum
{
   IpFamilyIpV4,
   IpFamilyIpV6
} McastIpFamily;

typedef enum
{
    McastGrpDaMacOnly     = 0x00,
    McastGrpDaMacVid      = 0x01,
    McastGrpDaMacSaIpv4   = 0x02,
    McastGrpDaIpv4Vid     = 0x03,
    McastGrpDaIpv6Vid     = 0x04,
    McastGrpDaMacSaIpv6   = 0x05,
    McastGrpDaIpv4        = 0x06,
    McastGrpDaIpv6        = 0x07,
    McastGrpDaSaIpv4      = 0x08,
    McastGrpDaSaIpv6      = 0x09,
    McastGrpDaSaIpv4Vid   = 0x0a,
    McastGrpDaSaIpv6Vid   = 0x0b,
    
    McastGrpTypeNum       = 0x0c,
    McastGrpTypeInvalid   = 0xFF       
} McastGroupType;

typedef enum
{
    McastIgmpMode         = 0x00,
    McastHostMode         = 0x01,
    McastInvalidMode      = 0x7F
} McastMode;

typedef enum
{
    McastOamInvalidType   = 0x00,
    McastOamCtcType       = 0x01,
    McastOamDpoeType      = 0x02
} McastOamType;

typedef enum
{
    McastFdbOpAdd,
    McastFdbOpDel,
    McastFdbOpDelTmp,
    McastFdbOpNum
} McastFdbOp;

typedef struct 
{
    U16 	             mcastVlan;
    U16 	             userVlan;
} mcastVlanSwitchEntry; 

typedef struct
{
    U8                   op;
    U16                  vlan[1];
} McastVlanOpCfg;

typedef struct
{
    U8                   action;
    U16                  vlanId;
} McastVlanAction;

typedef struct
{
    U8                   op; 
    U8                   switchVlanNum;
    McastVlanEntry       entry[1];
} McastTagOpCfg;

typedef struct
{ 
    U8                   maxGroups;
    U8                   numVids;
    U16                  vids[MAX_MCASTS_PER_PORT];
    vlan_rule_data       vidRules[MAX_MCASTS_PER_PORT][MAX_VLANCTL_TAGS];
    U8                   tagStripped;             //0: keep; 1: detag; 2: translate
    U8                   numVlanSwitches;
    mcastVlanSwitchEntry entry[MAX_MCAST_SWITCHING_ENTRIES_PER_PORT];
} mcastPortConfig_t;

typedef struct
{
    U16                  userId;
    U16                  vlanTag;
    MacAddr              da;
    McastIpAddr          groupIp;
    McastIpAddr          sourceIp;
    U16                  ipFamily;       //0: ipv4; 1: ipv6
    U16                  index;          //iptv entry index
#ifdef DPOE_OAM_BUILD
    U16                  portUsed[MAX_UNI_PORTS];
#endif
    McastVlanAction      vlanAction;
    U32                  channelIndex;
} PACK  IptvEntry;

typedef struct
{
    U8                   mcastMode;
    mcastPortConfig_t    mcastPortCfg[MAX_UNI_PORTS];
    U8                   ctrlType;                        
    U8                   numEntries;
    IptvEntry            fdbEntry[MAX_MCAST_FDB_ENTRIES];     /*iptv entry*/
    vlan_rule_data       usMcastTagRules[MAX_UNI_PORTS][MAX_VLANCTL_TAGS];
} mcastConfig_t;

typedef struct
{
  BCM_COMMON_DECLARE_LL_ENTRY ();
  t_BCM_MCAST_PKT_INFO   pktInfo;
  union
  {
      struct in_addr     v4;
      struct in6_addr    v6;
  }grpIp;
  union
  {
      struct in_addr     v4;
      struct in6_addr    v6;
  }srcIp;
  U16 index;
} BCM_MULTICAST_RULE_ENTRY;

typedef struct
{
    U16                  userId;
    U16                  vlanTag;
    MacAddr              da;
} mcastEntry_1;

typedef struct
{
    U16                  userId;
    MacAddr              da;
    U8                   sourceIp[4];
} mcastEntry_2;

typedef struct
{
    U16                  userId;
    U16                  vlanTag;
    /*ip addr resides in low 4 bytes. the high two bytes are filled with 0*/
    U8                   groupIp[6];     
} mcastEntry_3;

typedef struct
{
    U16                  userId;
    U16                  vlanTag;
    U8                   groupIp[16];
} mcastEntry_4;

typedef struct
{
    U16                  userId;
    MacAddr              da;
    U8                   sourceIp[16];
} mcastEntry_5;

typedef struct
{
    U16                  userId;
    U16                  vlanTag;
    U8                   groupIp[16];
    U8                   sourceIp[16];
} mcastEntry_6;

#ifdef EPON_SFU
#ifdef CTC_MCAST_DEBUG
extern void McastDumpConfigInfo(U8 port);
#endif

extern int  McastAddOrRemoveFdbEntry(U8 ctrType, IptvEntry *pEntry, U16 *index, McastFdbOp fdbOp);

extern void McastSaveVlanRule(vlan_rule_data *pRule, char *devName, vlanCtl_direction_t direction, U32 numTags, U32 ruleIdx);

extern int  McastDeleteVlanRule(vlan_rule_data * pRule) ;

extern int  McastDelFdbEntriesOnPort(U8 port, McastFdbOp fdbOp);

extern int  McastFindVidOnPort(int port, U16 tag, U16 *foundIndex);

extern int  McastSetUpStreamVlanTag(void);

#if defined(DMP_X_BROADCOM_COM_IGMP_1) || defined(DMP_X_BROADCOM_COM_MLD_1)
extern int  McastEnableSnooping(int enable);
#endif

extern int  McastParseFdbEntry(U8 ctltype, U8 *pEntry, IptvEntry* fdbEntry);

extern int  McastCheckFdbEntry(U8 ctltype, U8 *pEntry);

extern U8   McastFdbEntrySizeGet(U8 ctrlType);

extern int  McastConfigClear(void);

extern BOOL McastSetMaxGroupsPortList(void);

extern int  McastConvertControlType(int ctlType);

extern void McastVlanDefaultUpdate(U8 port);

extern void McastVlanDefaultAdd(U8 port);

extern void McastVlanDefaultClear(U8 port);

extern int  McastVlanTagSet(U8 port, U8 numVids, const U16 *pVid, U8 vlanOp); 

extern int  McastAddReportToList(int type, struct in6_addr *grpIp, t_BCM_MCAST_PKT_INFO *pktInfo, void *srcAddr);

extern int  McastFastLeaveEnabled(void);

extern int  McastSetReport(int type, struct in6_addr *grpIp, struct in6_addr *srcIp, McastVlanAction *vlanAction, t_BCM_MCAST_PKT_INFO *pktInfo, U16 *index, McastFdbOp fdbOp);

extern int  McastIgmpProcessInput(t_BCM_MCAST_PKT_INFO *pktInfo);

extern int  McastMldProcessInput(t_BCM_MCAST_PKT_INFO *pktInfo);

extern U8   McastTransVlanOnPortGet(U8 vlan, U8 port);

extern U8   McastModeGet(void);

extern void McastModeSet(U8 mcastMode);

extern U16  McastFdbUserIdGet(int index);

extern U16  McastFdbVlanGet(int index);

extern U8   McastFdbEntryNumGet(void);

extern void McastFdbEntryNumSet(U8 num);

extern void McastFdbDaCpy(int index, U8 *daMac);

extern void McastFdbIpCpy(int index, U8 *ipAddr, U8 ipVer);

extern int  McastFdbDaCmp(int index, U8 *daMac);

extern int  McastFdbGroupIpCmp(int index, U8 *ipAddr, U8 ipVer);

extern int  McastFdbSourceIpCmp(int index, U8 *ipAddr, U8 ipVer);

extern U8   McastCtrlTypeGet(void);

extern void McastCtrlTypeSet(U8 ctrlType);

extern U8   McastPortVlanOpGet(U8 port);

extern void McastPortVlanOpSet(U8 port, U8 op);

extern U8   McastPortVlanSwitchNumGet(U8 port);

extern U16  McastPortEntryMcastVlanGet(U8 port,int i);

extern U16  McastPortEntryUserVlanGet(U8 port,int i);

extern U8   McastPortVidNumGet(U8 port);

extern U16  McastPortVidGet(U8 port, int i);

extern U8   McastGroupMaxGet(U8 port);

extern void McastGroupMaxSet(U8 port, U8 max);
 
extern int  McastTagOpSet(U8 port, U8 vlanNum, U8 tagOp, const McastVlanEntry *pEntry);

extern void McastVlanActionOnPortGet(IptvEntry *pEntry);

extern int McastHandleVlanRule(U16 port, U16 vlanTag, int shouldRemove);

#ifdef DPOE_OAM_BUILD
extern U16  McastFdbPortUsedGet(U8 port, int index);

extern void McastFdbPortUsedInc(U8 port, int index);

extern void McastFdbPortUsedDec(U8 port, int index);
#endif
extern void McastFdbClearByIndex(U8 index);

#endif
extern int McastIngressFiltersSet(rdpa_ports port, rdpa_filter filterType, rdpa_forward_action action);

extern void McastInit(void);

#if defined(__cplusplus)
}
#endif

#endif
