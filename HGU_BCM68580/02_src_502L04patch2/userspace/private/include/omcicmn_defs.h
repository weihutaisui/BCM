/***********************************************************************
 *
 *  Copyright (c) 2017 Broadcom
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

/*****************************************************************************
*    Description:
*
*      OMCI common definitions.
*
*****************************************************************************/

#ifndef _OMCICMN_DEFS_H_
#define _OMCICMN_DEFS_H_


/* ---- Include Files ----------------------------------------------------- */

#include "os_defs.h"
#include "omci_msg.h"


/* ---- Constants and Types ----------------------------------------------- */

#define OMCID_DEBUG_ON  1
#define OMCID_DEBUG_OFF 0

/*
 * Ethernet port type definition in the GWO profile.
 * ONT: An ETH port set to ONT type may be configured by OMCI as if the port
 *      works in the SFU mode. The ETH port is reported as a PPTP ETH UNI
 *      interface in the OMCI MIB. The VLAN path is
 *      ethn.x->brx->gponx.x->gponx.
 * RG:  An ETH port set to RG type is owned by TR-69/WEB-UI. The ETH port is
 *      not reported in the OMCI MIB. The VLAN path is
 *      ethn->br0->veip0.1->veip0->gpon0.0->gpon0.
 */
typedef enum
{
    OMCI_ETH_PORT_TYPE_NONE = 0,
    OMCI_ETH_PORT_TYPE_NA, /* Deprecated. */
    OMCI_ETH_PORT_TYPE_ONT,
    OMCI_ETH_PORT_TYPE_RG
} OmciEthPortType;

typedef struct
{
    union
    {
        UINT32 all;
        struct
        {
            UINT32 eth0:2;
            UINT32 eth1:2;
            UINT32 eth2:2;
            UINT32 eth3:2;
            UINT32 eth4:2;
            UINT32 eth5:2;
            UINT32 eth6:2;
            UINT32 eth7:2;
            UINT32 unused:16;
        } ports;
    } types;
} OmciEthPortType_t;

typedef struct
{
    union
    {
        UINT32 all;
        struct
        {
            UINT32 omci:1;
            UINT32 model:1;
            UINT32 vlan:1;
            UINT32 cmf:1;
            UINT32 flow:1;
            UINT32 rule:1;
            UINT32 mcast:1;
            UINT32 voice:1;
            UINT32 file:1;
            UINT32 unused:23;
        } bits;
    } flags;
} omciDebug_t;

typedef enum
{
    OMCI_ACTION_NONE = 0,
    OMCI_ACTION_CREATE,
    OMCI_ACTION_EDIT,
    OMCI_ACTION_DELETE
} OmciObjectAction;

typedef enum
{
    OMCI_BP_TP_PPTP_ETH = 1,
    OMCI_BP_TP_VCC,
    OMCI_BP_TP_MAPPER_SERVICE,
    OMCI_BP_TP_IP_HOST,
    OMCI_BP_TP_GEM_INTERWORKING,
    OMCI_BP_TP_MCAST_GEM_INTERWORKING,
    OMCI_BP_TP_PPTP_XDSL,
    OMCI_BP_TP_PPTP_VDSL,
    OMCI_BP_TP_ETH_FLOW,
    OMCI_BP_TP_PPTP_802_11,
    OMCI_BP_TP_VEIP,
    OMCI_BP_TP_PPTP_MOCA
} OmciBridgePortTpType;

typedef enum
{
    OMCI_MS_TP_BRIDGE = 0,
    OMCI_MS_TP_PPTP_ETH,
    OMCI_MS_TP_IP_HOST,
    OMCI_MS_TP_ETH_FLOW,
    OMCI_MS_TP_PPTP_XDSL,
    OMCI_MS_TP_PPTP_802_11,
    OMCI_MS_TP_PPTP_MOCA,
    OMCI_MS_TP_VEIP,
    OMCI_MS_TP_IW_VCC
} OmciMapperSeriveTpType;

typedef enum
{
    OMCI_VLAN_TP_DEFAULT = 0,
    OMCI_VLAN_TP_IP_HOST,
    OMCI_VLAN_TP_MAPPER_SERVICE,
    OMCI_VLAN_TP_MAC_BRIDGE_PORT,
    OMCI_VLAN_TP_PPTP_XDSL,
    OMCI_VLAN_TP_GEM_INTERWORKING,
    OMCI_VLAN_TP_MCAST_GEM_INTERWORKING,
    OMCI_VLAN_TP_PPTP_MOCA,
    OMCI_VLAN_TP_PPTP_802_11,
    OMCI_VLAN_TP_ETH_FLOW,
    OMCI_VLAN_TP_PPTP_ETH,
    OMCI_VLAN_TP_VEIP,
    OMCI_VLAN_TP_MPLS_TP
} OmciVlanAssociatedType;

typedef enum
{
    OMCI_XVLAN_TP_MAC_BRIDGE_PORT = 0,
    OMCI_XVLAN_TP_MAPPER_SERVICE,
    OMCI_XVLAN_TP_PPTP_ETH,
    OMCI_XVLAN_TP_IP_HOST,
    OMCI_XVLAN_TP_PPTP_XDSL,
    OMCI_XVLAN_TP_GEM_INTERWORKING,
    OMCI_XVLAN_TP_MCAST_GEM_INTERWORKING,
    OMCI_XVLAN_TP_PPTP_MOCA,
    OMCI_XVLAN_TP_PPTP_802_11,
    OMCI_XVLAN_TP_ETH_FLOW,
    OMCI_XVLAN_TP_VEIP,
    OMCI_XVLAN_TP_MPLS_TP
} OmciXVlanAssociatedType;

typedef enum
{
    OMCI_MCAST_METYPE_MAC_BPCD = 0,
    OMCI_MCAST_METYPE_MAPPER_SVC,
} OmciMcastMeType;

typedef enum
{
    OMCI_DEBUG_MODULE_NONE = 0,
    OMCI_DEBUG_MODULE_OMCI,
    OMCI_DEBUG_MODULE_MODEL,
    OMCI_DEBUG_MODULE_VLAN,
    OMCI_DEBUG_MODULE_CMF,
    OMCI_DEBUG_MODULE_FLOW,
    OMCI_DEBUG_MODULE_RULE,
    OMCI_DEBUG_MODULE_MCAST,
    OMCI_DEBUG_MODULE_VOICE,
    OMCI_DEBUG_MODULE_FILE
} OmciDebugModule;

typedef enum
{
    OMCI_CONN_UNCONFIGURE = 0,
    OMCI_CONN_CONNECTING,
    OMCI_CONN_CONNECTED,
    OMCI_CONN_PENDING_DISCONNECT,
    OMCI_CONN_DISCONNECTING,
    OMCI_CONN_DISCONNECTED
} OmciConnectType;

typedef enum
{
    OMCI_PHY_NONE = 0,
    OMCI_PHY_ETHERNET,
    OMCI_PHY_MOCA,
    OMCI_PHY_WIRELESS,
    OMCI_PHY_POTS,
    OMCI_PHY_GPON
} OmciPhyType;

// OMCI_SCHEDULE_POLICY_HOL (Head of Line Queueing aka Strict Priority)
// OMCI_SCHEDULE_POLICY_WRR (Weighted Round Robin)
typedef enum
{
    OMCI_SCHEDULE_POLICY_NONE = 0,
    OMCI_SCHEDULE_POLICY_HOL,
    OMCI_SCHEDULE_POLICY_WRR
} OmciSchedulePolicy;

// OMCI_TRAFFIC_MANAGEMENT_PRIO (Priority controlled with flexibly scheduled)
// OMCI_TRAFFIC_MANAGEMENT_RATE (Rate controlled)
// OMCI_TRAFFIC_MANAGEMENT_PRIO_RATE (Priority with flexibly scheduled and rate controlled)
typedef enum
{
    OMCI_TRAFFIC_MANAGEMENT_PRIO = 0,
    OMCI_TRAFFIC_MANAGEMENT_RATE,
    OMCI_TRAFFIC_MANAGEMENT_PRIO_RATE
} OmciTrafficManagement;

// Options when the priority queue pointer for downstream
// in the GEM port network CTP ME is invalid
// 0: Skip vlanCtl_cmdSetSkbMarkQueue().
// 1. Set queue ID = packet pbit value. The queue setting is done in the VLAN
//    driver when the packet traverses the Linux kernel. This approach is
//    suitable for ONU/RG (HGU) mode.
// 2. Set queue ID = packet pbit value. The queue setting is done in OMCI by
//    multiplying VLANCtl rules with filter-pbits fields. This approach is
//    suitable for both SFU and ONU/RG. However, note the number of rules
//    may be multiplied by 8 (possible pbits values) in some cases, and may not
//    be the most optimized solution.
typedef enum
{
    OMCI_DS_INVALID_QUEUE_ACTION_NONE = 0,
    OMCI_DS_INVALID_QUEUE_ACTION_PBIT,
    OMCI_DS_INVALID_QUEUE_ACTION_PBIT_EXT
} OmciDsInvalidQueueACtion;

// Reference: [ITU-T G.988] 9.2.10 Priority queue definition.
typedef enum
{
    OMCI_COLOUR_MARK_NONE = 0,
    OMCI_COLOUR_MARK_INTERNAL,
    OMCI_COLOUR_MARK_DEI,
    OMCI_COLOUR_MARK_PCP8P0D,
    OMCI_COLOUR_MARK_PCP7P1D,
    OMCI_COLOUR_MARK_PCP6P2D,
    OMCI_COLOUR_MARK_PCP5P3D,
    OMCI_COLOUR_MARK_DSCPAF
} OmciColourMarkType;

// Reference: [ITU-T G.988] 6.4 Voice over IP management.
typedef enum
{
    OMCI_VOIP_PATH_OMCI = 0,
    OMCI_VOIP_PATH_IP
} OmciVoipPathType;

// Reference: [ITU-T G.988]. Certain ME and attribute definitions,
// such as the un-assigned Alloc-ID value, are different between G.984
// (0xff or 0xffff) and G.987 systems (0xffff).
typedef enum
{
    OMCI_PON_APP_TYPE_984 = 0,
    OMCI_PON_APP_TYPE_987
} OmciPonAppType;


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Variable Externs -------------------------------------------------- */


/* ---- Function Prototypes ----------------------------------------------- */


#endif /* _OMCICMN_DEF_H_ */
