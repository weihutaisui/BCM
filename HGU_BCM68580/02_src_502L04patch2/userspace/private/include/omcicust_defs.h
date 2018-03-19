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
*      OMCI user-customizable definitions.
*
*****************************************************************************/

#ifndef _OMCICUST_DEFS_H
#define _OMCICUST_DEFS_H

/* ---- Include Files ----------------------------------------------------- */

#include "os_defs.h"


/* ---- Constants and Types ----------------------------------------------- */

/* OMCI MIB related definitions. */

#define GPON_CARD_HOLDER_NUM_MAX       5
/* Value of MS byte is 1 for virtual circuit pack. */
#define GPON_CARD_HOLDER_ETH_MEID      0x0101
#define GPON_CARD_HOLDER_MOCA_MEID     0x0102
#define GPON_CARD_HOLDER_POTS_MEID     0x0103
#define GPON_CARD_HOLDER_VEIP_MEID     0x0104
#define GPON_CARD_HOLDER_GPON_MEID     0x0180
#ifdef BRCM_FTTDP
#define GPON_PHY_ETH_PORT_MAX          24
#else
#define GPON_PHY_ETH_PORT_MAX          8
#endif
#define GPON_PHY_MOCA_PORT_MAX         1
#define GPON_PHY_POTS_PORT_MAX         2
#define GPON_PHY_VEIP_PORT_MAX         1
#define GPON_PHY_GPON_PORT_MAX         1
#define GPON_TCONT_MAX                 BCM_PLOAM_NUM_DATA_TCONTS
#define GPON_IP_HOST_MAX               1
#define GPON_IPV6_HOST_MAX             1
#define GPON_PHY_US_TS_MAX             BCM_PLOAM_NUM_GTCUS_QUEUES_PER_TCONT
#define GPON_PHY_US_PQ_MAX             BCM_PLOAM_NUM_GTCUS_QUEUES_PER_TCONT
#ifdef BRCM_FTTDP
#define GPON_PHY_DS_PQ_MAX             4
#else
#define GPON_PHY_DS_PQ_MAX             8
#endif
#define OMCI_PRIO_MAX                  7
#define OMCI_QID_NOT_EXIST             0xff

#define OMCI_DEFAULT_TCONT_MEID        0

#define GPON_PHY_PQ_TOTAL              \
  ((GPON_TCONT_MAX * GPON_PHY_US_PQ_MAX) + \
  ((GPON_PHY_ETH_PORT_MAX + GPON_PHY_MOCA_PORT_MAX + \
  GPON_PHY_VEIP_PORT_MAX) * GPON_PHY_DS_PQ_MAX))

#define GPON_FIRST_TCONT_MEID          ((GPON_CARD_HOLDER_GPON_MEID & 0xFF) << 8)
#define GPON_FIRST_ETH_MEID            (((GPON_CARD_HOLDER_ETH_MEID & 0xFF) << 8) + 1)
#define GPON_FIRST_MOCA_MEID           (((GPON_CARD_HOLDER_MOCA_MEID & 0xFF) << 8) + 1)
#define GPON_FIRST_POTS_MEID           (((GPON_CARD_HOLDER_POTS_MEID & 0xFF) << 8) + 1)
#define GPON_FIRST_VEIP_MEID           (((GPON_CARD_HOLDER_VEIP_MEID & 0xFF) << 8) + 1)

#define OMCI_DIRECTORY                 "/var/omci/"
#define OMCI_DEBUG_FILE_NAME           "/var/omci/omci.dbg"
#define BCMGPON_FILE                   "/dev/bcm_omci"

#define DEFAULT_RG_BRIDGE_NAME         "br0"
#define GPON_DEF_IFNAME                "gpondef"
#define GPON_WAN_IF_NAME               "veip0"

#define MAX_LARGE_STRING_PARTS         15

#define SUPPORT_DOUBLE_TAGS

/* Software image related definitions. */

#define OMCI_SW_IMAGE_0_VERSION "534F465457415245494D41474530" /* SOFTWAREIMAGE0 */
#define OMCI_SW_IMAGE_1_VERSION "534F465457415245494D41474531" /* SOFTWAREIMAGE1 */
#define OMCI_SW_IMAGE_0_VERDFLT "SOFTWAREIMAGE0" /* SOFTWAREIMAGE0 */
#define OMCI_SW_IMAGE_1_VERDFLT "SOFTWAREIMAGE1" /* SOFTWAREIMAGE1 */
#define OMCI_SOFTWARE_DOWNLOAD_SECTION_SIZE (31)

/*
 * Maximum ME class ID can be supported should be updated when more ME
 * are added in omcid_me.h and omcimib_util.c.
 */
#define OMCI_ME_CLASS_MAX       427

#define OMCI_MIB_UPLOAD_INST_MAX  256 /* maximum number of ME instances to upload */
#define OMCI_MIB_UPLOAD_FRAG_MAX  16

#define OMCI_RMTDBG_RSP_LEN_MAX (32 * 1024)

/*
 * MAX number of supported ANI VIDs defined in Multicast operations profile 
 * ME Dynamic ACL. 
 */
#define MCAST_VID_LIST_MAX       64


/* ---- Macro API definitions --------------------------------------------- */

#define rutGpon_getVeipId()            GPON_FIRST_VEIP_MEID


/* ---- Variable Externs -------------------------------------------------- */


/* ---- Function Prototypes ----------------------------------------------- */


#endif /* _OMCICUST_DEFS_H */
