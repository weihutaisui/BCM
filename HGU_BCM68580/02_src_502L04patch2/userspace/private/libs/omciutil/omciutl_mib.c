/***********************************************************************
 *
 *  Copyright (c) 2007 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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
*      OMCI MIB/ME utility APIs.
*      APIs in this file may be used by an external process.
*
*****************************************************************************/

/* ---- Include Files ----------------------------------------------------- */

#include "omci_objectid.h"
#include "omciutl_cmn.h"


/* ---- Private Constants and Types --------------------------------------- */


/* ---- Macro API definitions --------------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */

static const OmciMdmOidClassId_t omciMdmOidClassIdTable[] =
{
    {MDMOID_ONT_DATA, 2},
    {MDMOID_CARD_HOLDER, 5},
    {MDMOID_CIRCUIT_PACK, 6},
    {MDMOID_SOFTWARE_IMAGE, 7},
    {MDMOID_PPTP_ETHERNET_UNI, 11},
    {MDMOID_ETHERNET_PM_HISTORY_DATA, 24},
    {MDMOID_MAC_BRIDGE_SERVICE_PROFILE, 45},
    {MDMOID_MAC_BRIDGE_CONFIG_DATA, 46},
    {MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA, 47},
    {MDMOID_MAC_BRIDGE_PORT_DESIGNATION_DATA, 48},
    {MDMOID_MAC_BRIDGE_PORT_FILTER_TABLE_DATA, 49},
    {MDMOID_MAC_BRIDGE_PORT_BRIDGE_TABLE_DATA, 50},
    {MDMOID_MAC_BRIDGE_PM_HISTORY_DATA, 51},
    {MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA, 52},
    {MDMOID_PPTP_POTS_UNI, 53},
    {MDMOID_VOICE_SERVICE, 58},
    {MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA, 78},
    {MDMOID_MAC_BRIDGE_PORT_FILTER_PRE_ASSIGN_TABLE, 79},
    {MDMOID_VLAN_TAGGING_FILTER_DATA, 84},
    {MDMOID_ETHERNET_PM_HISTORY_DATA2, 89},
    {MDMOID_MAPPER_SERVICE_PROFILE, 130},
    {MDMOID_OLT_G, 131},
    {MDMOID_POWER_SHEDDING, 133},
    {MDMOID_IP_HOST_CONFIG_DATA, 134},
    {MDMOID_IP_HOST_PM_HISTORY_DATA, 135},
    {MDMOID_TCP_UDP_CONFIG_DATA, 136},
    {MDMOID_NETWORK_ADDRESS, 137},
    {MDMOID_VO_IP_CONFIG_DATA, 138},
    {MDMOID_VO_IP_VOICE_CTP, 139},
    {MDMOID_CALL_CONTROL_PM_HISTORY_DATA, 140},
    {MDMOID_VO_IP_LINE_STATUS, 141},
    {MDMOID_VO_IP_MEDIA_PROFILE, 142},
    {MDMOID_RTP_PROFILE_DATA, 143},
    {MDMOID_RTP_PM_HISTORY_DATA, 144},
    {MDMOID_NETWORK_DIAL_PLAN_TABLE, 145},
    {MDMOID_VO_IP_APP_SERVICE_PROFILE, 146},
    {MDMOID_VOICE_FEATURE_ACCESS_CODES, 147},
    {MDMOID_AUTHENTICATION_SECURITY_METHOD, 148},
    {MDMOID_SIP_CONFIG_PORTAL, 149},
    {MDMOID_SIP_AGENT_CONFIG_DATA, 150},
    {MDMOID_SIP_AGENT_PM_HISTORY_DATA, 151},
    {MDMOID_SIP_CALL_INIT_PM_HISTORY_DATA, 152},
    {MDMOID_SIP_USER_DATA, 153},
    {MDMOID_MGC_CONFIG_PORTAL, 154},
    {MDMOID_MGC_CONFIG_DATA, 155},
    {MDMOID_MGC_PM_HISTORY_DATA, 156},
    {MDMOID_LARGE_STRING, 157},
    {MDMOID_ONU_REMOTE_DEBUG, 158},
    {MDMOID_EQPT_PROTECT_PROFILE, 159},
    {MDMOID_PPTP_MOCA_UNI, 162},
    {MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA, 163},
    {MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA, 164},
    {MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA, 171},
    {MDMOID_BRCM_PPTP_MOCA_UNI, 240},
    {MDMOID_MOCA_STATUS, 241},
    {MDMOID_MOCA_STATS, 242},
    {MDMOID_ONT_G, 256},
    {MDMOID_ONT2_G, 257},
    {MDMOID_T_CONT, 262},
    {MDMOID_ANI_G, 263},
    {MDMOID_UNI_G, 264},
    {MDMOID_GEM_INTERWORKING_TP, 266},
    {MDMOID_GEM_PORT_NETWORK_CTP, 268},
    {MDMOID_GAL_ETHERNET_PROFILE, 272},
    {MDMOID_THRESHOLD_DATA1, 273},
    {MDMOID_THRESHOLD_DATA2, 274},
    {MDMOID_GAL_ETHERNET_PM_HISTORY_DATA, 276},
    {MDMOID_PRIORITY_QUEUE_G, 277},
    {MDMOID_TRAFFIC_SCHEDULER_G, 278},
    {MDMOID_PROTECTION_DATA, 279},
    {MDMOID_GEM_TRAFFIC_DESCRIPTOR, 280},
    {MDMOID_MULTICAST_GEM_INTERWORKING_TP, 281},
    {MDMOID_OMCI, 287},
    {MDMOID_ETHERNET_PM_HISTORY_DATA3, 296},
    {MDMOID_PORT_MAPPING_PACKAGE_G, 297},
    {MDMOID_DOT1_RATE_LIMITER, 298},
    {MDMOID_OCTET_STRING, 307},
    {MDMOID_GENERAL_PURPOSE_BUFFER, 308},
    {MDMOID_MULTICAST_OPERATIONS_PROFILE, 309},
    {MDMOID_MULTICAST_SUBSCRIBER_CONFIG_INFO, 310},
    {MDMOID_MULTICAST_SUBSCRIBER_MONITOR, 311},
    {MDMOID_FEC_PM_HISTORY_DATA, 312},
    {MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA, 321},
    {MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA, 322},
    {MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT, 329},
    {MDMOID_GENERIC_STATUS_PORTAL, 330},
    {MDMOID_ESC, 332},
    {MDMOID_ETHERNET_FRAME_EXTENDED_P_M, 334},
    {MDMOID_DYNAMIC_POWER_MGMT, 336},
    {MDMOID_TR069_MANAGEMENT_SERVER, 340},
    {MDMOID_GEM_PORT_PM_HISTORY_DATA, 341},
    {MDMOID_TCP_UDP_PM_HISTORY_DATA, 342},
    {MDMOID_ENERGY_CONSUMPTION_PM_HISTORY_DATA, 343},
    {MDMOID_IPV6_HOST_CONFIG_DATA, 347},
    {MDMOID_MAC_BP_ICMPV6_PROCESS_PREASSIGN_TABLE, 348},
    {MDMOID_ETHERNET_FRAME_EXTENDED_P_M64_BIT, 425},
    {MDMOID_THRESHOLD_DATA64_BIT, 426}
};

static UINT32 omciClassIdNum = (sizeof(omciMdmOidClassIdTable) /
  sizeof(OmciMdmOidClassId_t));


/* ---- Functions --------------------------------------------------------- */

/*****************************************************************************
*  FUNCTION:  omciUtl_classIdToOid
*  PURPOSE:   Map OMCI class ID to OID.
*  PARAMETERS:
*      obj - pointer to OmciMdmOidClassId_t.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omciUtl_classIdToOid(OmciMdmOidClassId_t *obj)
{
    UINT32 classI = 0;

    for (classI = 0; classI < omciClassIdNum; classI++)
    {
        if (omciMdmOidClassIdTable[classI].classId == obj->classId)
            break;
    }

    if (classI < omciClassIdNum)
    {
        obj->mdmOid = omciMdmOidClassIdTable[classI].mdmOid;
    }
    else
    {
        obj->mdmOid = 0;
    }
}

/*****************************************************************************
*  FUNCTION:  omciUtl_oidToClassId
*  PURPOSE:   Map OID to OMCI class ID.
*  PARAMETERS:
*      obj - pointer to OmciMdmOidClassId_t.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void omciUtl_oidToClassId(OmciMdmOidClassId_t *obj)
{
    UINT32 classI = 0;

    for (classI = 0; classI < omciClassIdNum; classI++)
    {
        if (omciMdmOidClassIdTable[classI].mdmOid == obj->mdmOid)
          break;
    }

    if (classI < omciClassIdNum)
    {
        obj->classId = omciMdmOidClassIdTable[classI].classId;
    }
    else
    {
        obj->classId = 0;
    }
}

/*****************************************************************************
*  FUNCTION:  omciUtl_getParamSize
*  PURPOSE:   Get OMCI object parameter size based on type and maximum value.
*  PARAMETERS:
*      type - parameter type.
*      maxVal - maximum parameter value defined in schema.
*  RETURNS:
*      Parameter size.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 omciUtl_getParamSize(_MdmParamTypes type, UINT32 maxVal)
{
    UINT32 size;

    switch (type)
    {
        case _MPT_STRING:
        case _MPT_BASE64:
            size = maxVal;
            break;

        case _MPT_HEX_BINARY:
            size = maxVal/2;
            break;

        case _MPT_INTEGER:
            if(maxVal < 128)
                size = 1;
            else if(maxVal < 32768)
                size = 2;
            else if(maxVal < 8388608)
                size = 3;
            else
                size = 4;
            break;

        case _MPT_UNSIGNED_INTEGER:
            if(maxVal < 256)
                size = 1;
            else if(maxVal < 65536)
                size = 2;
            else if(maxVal < 16777216)
                size = 3;
            else
                size = 4;
            break;

        case _MPT_LONG64:
        case _MPT_UNSIGNED_LONG64:
            size = 8;
            break;

        case _MPT_BOOLEAN:
            size = 1;
            break;

        default:
            size = 0;
            break;
    }

    return size;
}
