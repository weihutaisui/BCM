/***********************************************************************
 *
 *  Copyright (c) 2007 Broadcom
 *  All Rights Reserved
 *
 * <:label-BRCM:2007:proprietary:omcid
 *
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 *
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 *
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
:>
 *
 ************************************************************************/

#ifndef OMCID_ME_H
#define OMCID_ME_H


#include "os_defs.h"
#include "omcid.h"
#include "omcid_data.h"

#define OMCI_ME_FLAG_PRESENT    0x0001 /* One instance of the ME is always present */
#define OMCI_ME_FLAG_DYNAMIC    0x0002 /* Instances of the ME may or may not exist,
                                                                        and can be created dynamically */
#define OMCI_ME_FLAG_MIB_UPLOAD 0x0004 /* This ME must be included in MIB Update
                                                                         operations */
#define OMCI_ME_FLAG_STATIC     0x0008 /* Instances of the ME that can be created,
                                                                            statically (status, statistics ME)*/

#define OMCI_ME_SOFTWARE_IMAGE  0x0007 /* ME class ID for Software Image */

#define OMCI_ME_ATTRIB_ALL      0xFFFF /* Indicates all ME attributes should be returned in mibload */
#define OMCI_ME_ATTRIB_NONE     0x0000 /* Indicates no ME attributes should be returned in mibload */
#define OMCI_ME_ATTRIB_1        0x8000 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_2        0x4000 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_3        0x2000 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_4        0x1000 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_5        0x0800 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_6        0x0400 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_7        0x0200 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_8        0x0100 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_9        0x0080 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_10       0x0040 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_11       0x0020 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_12       0x0010 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_13       0x0008 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_14       0x0004 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_15       0x0002 /* Designates ME attribute for mibload */
#define OMCI_ME_ATTRIB_16       0x0001 /* Designates ME attribute for mibload */

#define OMCI_ME_PM_THRES_ATTRIB OMCI_ME_ATTRIB_2
#define OMCI_ME_PM_RO_ATTRIB    0xBFFF


typedef struct
{
    char  *name;
    UINT32 mdmOid;
    UINT32 parentMdmOid;
    UINT16 flags;
    SINT16 nbrInstancesUpload; /* The number of instances to upload of dynamic
                                  objects created by the OLT *MUST* always be -1,
                                  if the object is to be included in MIB Uploads */
    UINT16 setByCreateMask; /* Note that the Managed Entity identifier, which is
                               an attribute of each managed entity, has no
                               corresponding bit in the Set by Create attribute
                               mask. Thus, the attributes are counted starting
                               from the most significant bit of this mask, which
                               corresponds to the first attribute after the
                               Managed Entity identifier. */

    UINT16 MibUploadAttributeList;  /*
                                        16-bit mask of ME attributes to include in the mib upload .
                                        Mask shall be 0xffff in all MEs except those that do not
                                        upload all attributes by default.  For MEs that are flagged
                                        to be included in mib upload the mask shall be interpreted.
                                        0xffff shall mean "upload all attributes" while other non 0
                                        values shall be interpreted to mean "selectively upload
                                        nth attributes based on position (1-16) in mask.  A value of
                                        0x8000 shall cause only the first attribute to be uploaded,
                                        while a value of 0x8888 would result in the 1st, 5th, 9th
                                        and 13th attributes to be uploaded if all existed.
                                    */
    UINT16 tableAttrMask; /* Table attribute mask. */
    UINT16 roAttrMask;    /* Read-only attribute mask. */
    UINT16 supportedMask; /* supported attribute mask */
    UINT16 optionalMask; /* optional attribute mask */
    OMCI_CUSTOM_FNC_GET custom_get;
    OMCI_CUSTOM_FNC_SET custom_set;
} omciMeInfo_t;

/*
 * G.988 MIB upload: Latch a snapshot (i.e. copy) of the current MIB. Not every
 * managed entity or every attribute is included in a MIB upload:
 * - Performance monitoring history data MEs are excluded;
 * - Table attributes are excluded;
 * - Other MEs and attributes, such as the PPTP for the local craft terminal, are
 *   excluded as documented in their specific definitions;
 * - OMCI ME is excluded;
 * - Managed Entity ME is excluded;
 * - Attribute ME is excluded;
 */

/* OMCI_ME_CLASS_MAX that is defined in omcicust_defs.h specifies max number of
   class id in OMCID_ME_INFO struct */

/*
 * Supported Managed Entities
 */

#define OMCID_ME_INFO_GENERIC                                           \
        [2]   = {.name = "ONU Data",                                    \
                 .mdmOid = MDMOID_ONT_DATA,                             \
                 .parentMdmOid = MDMOID_EQUIPMENT_MANAGEMENT,           \
                 .flags = (OMCI_ME_FLAG_PRESENT|                        \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x8000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [5]   = {.name = "Cardholder",                                  \
                 .mdmOid = MDMOID_CARD_HOLDER,                          \
                 .parentMdmOid = MDMOID_EQUIPMENT_MANAGEMENT,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x8C00,                                  \
                 .supportedMask = 0xe000,                               \
                 .optionalMask = 0x3F80,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [6]   = {.name = "Circuit Pack",                                \
                 .mdmOid = MDMOID_CIRCUIT_PACK,                         \
                 .parentMdmOid = MDMOID_EQUIPMENT_MANAGEMENT,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x7AB8,                                  \
                 .supportedMask = 0xFEB8,                               \
                 .optionalMask = 0x4B84,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [7]   = {.name = "Software Image",                              \
                 .mdmOid = MDMOID_SOFTWARE_IMAGE,                       \
                 .parentMdmOid = MDMOID_EQUIPMENT_MANAGEMENT,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xFC00,                                  \
                 .supportedMask = 0xF000,                               \
                 .optionalMask = 0x0C00,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [11]  = {.name = "PPTP Ethernet UNI",                           \
                 .mdmOid = MDMOID_PPTP_ETHERNET_UNI,                    \
                 .parentMdmOid = MDMOID_ETHERNET_SERVICES,              \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x4600,                                  \
                 .supportedMask = 0xFF98,                               \
                 .optionalMask = 0x047E,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [24]  = {.name = "Ethernet PM History Data",                    \
                 .mdmOid = MDMOID_ETHERNET_PM_HISTORY_DATA,             \
                 .parentMdmOid = MDMOID_ETHERNET_SERVICES,              \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0xFFFF,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [45]  = {.name = "MAC Bridge Service Profile",                  \
                 .mdmOid = MDMOID_MAC_BRIDGE_SERVICE_PROFILE,           \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFFC0,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x5000,                               \
                 .optionalMask = 0x00C0,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [46]  = {.name = "MAC Bridge Configuration Data",               \
                 .mdmOid = MDMOID_MAC_BRIDGE_CONFIG_DATA,               \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xFF00,                                  \
                 .supportedMask = 0x4300,                               \
                 .optionalMask = 0x0300,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [47]  = {.name = "MAC Bridge Port Configuration Data",          \
                 .mdmOid = MDMOID_MAC_BRIDGE_PORT_CONFIG_DATA,          \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFF88,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0040,                                  \
                 .supportedMask = 0xF030,                               \
                 .optionalMask = 0x09F8,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [48]  = {.name = "MAC Bridge Port Designation Data",            \
                 .mdmOid = MDMOID_MAC_BRIDGE_PORT_DESIGNATION_DATA,     \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xC000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [49]  = {.name = "MAC Bridge Port Filter Table Data",           \
                 .mdmOid = MDMOID_MAC_BRIDGE_PORT_FILTER_TABLE_DATA,    \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0x8000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = getMacBridgePortFilterTableData,         \
                 .custom_set = setMacBridgePortFilterTableData},        \
                                                                        \
        [50]  = {.name = "MAC Bridge Port Bridge Table Data",           \
                 .mdmOid = MDMOID_MAC_BRIDGE_PORT_BRIDGE_TABLE_DATA,    \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0x8000,                               \
                 .roAttrMask = 0x8000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = getMacBridgePortBridgeTableData,         \
                 .custom_set = NULL},                                   \
                                                                        \
        [51]  = {.name = "MAC Bridge PM History Data",                  \
                 .mdmOid = MDMOID_MAC_BRIDGE_PM_HISTORY_DATA,           \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [52]  = {.name = "MAC Bridge Port PM History Data",             \
                 .mdmOid = MDMOID_MAC_BRIDGE_PORT_PM_HISTORY_DATA,      \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [78]  = {.name = "VLAN Tagging Operation Configuration Data",   \
                 .mdmOid = MDMOID_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA, \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xF800,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xF800,                               \
                 .optionalMask = 0x1800,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [79]  = {.name = "MAC Bridge Port Filter PreAssign Table",      \
                 .mdmOid = MDMOID_MAC_BRIDGE_PORT_FILTER_PRE_ASSIGN_TABLE,    \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [84]  = {.name = "VLAN Tagging Filter Data",                    \
                 .mdmOid = MDMOID_VLAN_TAGGING_FILTER_DATA,             \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xE000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xE000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [89]  = {.name = "Ethernet PM History Data 2",                  \
                 .mdmOid = MDMOID_ETHERNET_PM_HISTORY_DATA2,            \
                 .parentMdmOid = MDMOID_ETHERNET_SERVICES,              \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [130] = {.name = "802.1p Mapper Service Profile",               \
                 .mdmOid = MDMOID_MAPPER_SERVICE_PROFILE,               \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFFD8,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xFFF8,                               \
                 .optionalMask = 0x0008,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [131] = {.name = "OLT-G",                                       \
                 .mdmOid = MDMOID_OLT_G,                                \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = (OMCI_ME_FLAG_PRESENT |                       \
                          OMCI_ME_FLAG_MIB_UPLOAD),                     \
                 .nbrInstancesUpload = 1,                               \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xF000,                               \
                 .optionalMask = 0x1000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [133] = {.name = "ONU power shedding",                          \
                 .mdmOid = MDMOID_POWER_SHEDDING,                       \
                 .parentMdmOid = MDMOID_EQUIPMENT_MANAGEMENT,           \
                 .flags = (OMCI_ME_FLAG_PRESENT |                       \
                          OMCI_ME_FLAG_MIB_UPLOAD),                     \
                 .nbrInstancesUpload = 1,                               \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0020,                                  \
                 .supportedMask = 0xE000,                               \
                 .optionalMask = 0x0030,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [134] = {.name = "IP Host Config Data",                         \
                 .mdmOid = MDMOID_IP_HOST_CONFIG_DATA,                  \
                 .parentMdmOid = MDMOID_LAYER3_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x40FE,                                  \
                 .supportedMask = 0xFFFE,                               \
                 .optionalMask = 0x00F8,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [135] = {.name = "IP Host PM History Data",                     \
                 .mdmOid = MDMOID_IP_HOST_PM_HISTORY_DATA,              \
                 .parentMdmOid = MDMOID_LAYER3_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0xf000,                               \
                 .optionalMask = 0x0FC0,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [136] = {.name = "TCP/UDP Config Data",                         \
                 .mdmOid = MDMOID_TCP_UDP_CONFIG_DATA,                  \
                 .parentMdmOid = MDMOID_LAYER3_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xF000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xF000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [137] = {.name = "Network Address",                             \
                 .mdmOid = MDMOID_NETWORK_ADDRESS,                      \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xC000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xC000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [148] = {.name = "Authentication Security Method",              \
                 .mdmOid = MDMOID_AUTHENTICATION_SECURITY_METHOD,       \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xF000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [157] = {.name = "Large String",                                \
                 .mdmOid = MDMOID_LARGE_STRING,                         \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xFFFF,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [158] = {.name = "ONU Remote Debug",                            \
                 .mdmOid = MDMOID_ONU_REMOTE_DEBUG,                     \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = OMCI_ME_FLAG_DYNAMIC,                         \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0x2000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xE000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = getOnuRemoteDebug,                       \
                 .custom_set = NULL},                                   \
                                                                        \
        [159] = {.name = "Equipment Protection Profile",                \
                 .mdmOid = MDMOID_EQPT_PROTECT_PROFILE,                 \
                 .parentMdmOid = MDMOID_EQUIPMENT_MANAGEMENT,           \
                 .flags = OMCI_ME_FLAG_DYNAMIC,                         \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFFCC,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0030,                                  \
                 .supportedMask = 0xFFFC,                               \
                 .optionalMask = 0x5FCC,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [162] = {.name = "PPTP MOCA UNI",                               \
                 .mdmOid = MDMOID_PPTP_MOCA_UNI,                        \
                 .parentMdmOid = MDMOID_MOCA_SERVICES,                  \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xFFFC,                               \
                 .optionalMask = 0xAE30,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [163] = {.name = "MOCA Ethernet PM History Data",               \
                 .mdmOid = MDMOID_MOCA_ETHERNET_PM_HISTORY_DATA,        \
                 .parentMdmOid = MDMOID_MOCA_SERVICES,                  \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0xFFFF,                               \
                 .optionalMask = 0x3FFF,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [164] = {.name = "MOCA Interface PM History Data",              \
                 .mdmOid = MDMOID_MOCA_INTERFACE_PM_HISTORY_DATA,       \
                 .parentMdmOid = MDMOID_MOCA_SERVICES,                  \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0xF000,                               \
                 .optionalMask = 0x2000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [171] = {.name = "Extended VLAN Tagging Operation Configuration Data", \
                 .mdmOid = MDMOID_EXTENDED_VLAN_TAGGING_OPERATION_CONFIGURATION_DATA, \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x8200,                             \
                 .MibUploadAttributeList = (UINT16)~OMCI_ME_ATTRIB_6,   \
                 .tableAttrMask = 0x0400,                               \
                 .roAttrMask = 0x4000,                                  \
                 .supportedMask = 0xFF00,                               \
                 .optionalMask = 0x0100,                                \
                 .custom_get = getExtendedVlanTaggingOperationConfigurationData, \
                 .custom_set = setExtendedVlanTaggingOperationConfigurationData}, \
                                                                        \
        [240] = {.name = "BRCM PPTP MOCA UNI",                          \
                 .mdmOid = MDMOID_BRCM_PPTP_MOCA_UNI,                   \
                 .parentMdmOid = MDMOID_MOCA_SERVICES,                  \
                 .flags = OMCI_ME_FLAG_STATIC,                          \
                 .nbrInstancesUpload = 0,                               \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xFFFF,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [241] = {.name = "BRCM_MOCA STATUS",                            \
                 .mdmOid = MDMOID_MOCA_STATUS,                          \
                 .parentMdmOid = MDMOID_MOCA_SERVICES,                  \
                 .flags = OMCI_ME_FLAG_STATIC,                          \
                 .nbrInstancesUpload = 0,                               \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xFFFF,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [242] = {.name = "BRCM_MOCA STATS",                             \
                 .mdmOid = MDMOID_MOCA_STATS,                           \
                 .parentMdmOid = MDMOID_MOCA_SERVICES,                  \
                 .flags = OMCI_ME_FLAG_STATIC,                          \
                 .nbrInstancesUpload = 0,                               \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xFFFF,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [256] = {.name = "ONU-G",                                       \
                 .mdmOid = MDMOID_ONT_G,                                \
                 .parentMdmOid = MDMOID_EQUIPMENT_MANAGEMENT,           \
                 .flags = (OMCI_ME_FLAG_PRESENT |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xF9E8,                                  \
                 .supportedMask = 0xF788,                               \
                 .optionalMask = 0x09F8,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [257] = {.name = "ONU2-G",                                      \
                 .mdmOid = MDMOID_ONT2_G,                               \
                 .parentMdmOid = MDMOID_EQUIPMENT_MANAGEMENT,           \
                 .flags = (OMCI_ME_FLAG_PRESENT |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xF7E8,                                  \
                 .supportedMask = 0xFFC0,                               \
                 .optionalMask = 0xA0FC,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [262] = {.name = "T-CONT",                                      \
                 .mdmOid = MDMOID_T_CONT,                               \
                 .parentMdmOid = MDMOID_ANI_MANAGEMENT,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x4000,                                  \
                 .supportedMask = 0xE000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [263] = {.name = "ANI-G",                                       \
                 .mdmOid = MDMOID_ANI_G,                                \
                 .parentMdmOid = MDMOID_ANI_MANAGEMENT,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xD84C,                                  \
                 .supportedMask = 0xFFFF,                               \
                 .optionalMask = 0x01FF,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [264] = {.name = "UNI-G",                                       \
                 .mdmOid = MDMOID_UNI_G,                                \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xA000,                                  \
                 .supportedMask = 0x4000,                               \
                 .optionalMask = 0x3000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [266] = {.name = "GEM Interworking Termination Point",          \
                 .mdmOid = MDMOID_GEM_INTERWORKING_TP,                  \
                 .parentMdmOid = MDMOID_ANI_MANAGEMENT,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xF200,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0C00,                                  \
                 .supportedMask = 0xF600,                               \
                 .optionalMask = 0x0C00,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [268] = {.name = "GEM Port Network CTP",                        \
                 .mdmOid = MDMOID_GEM_PORT_NETWORK_CTP,                 \
                 .parentMdmOid = MDMOID_ANI_MANAGEMENT,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFAC0,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0500,                                  \
                 .supportedMask = 0xFAC0,                               \
                 .optionalMask = 0x0DC0,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [272] = {.name = "GAL Ethernet Profile",                        \
                 .mdmOid = MDMOID_GAL_ETHERNET_PROFILE,                 \
                 .parentMdmOid = MDMOID_ANI_MANAGEMENT,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x8000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [273] = {.name = "Threshold Data 1",                            \
                 .mdmOid = MDMOID_THRESHOLD_DATA1,                      \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFE00,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xFE00,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [274] = {.name = "Threshold Data 2",                            \
                 .mdmOid = MDMOID_THRESHOLD_DATA2,                      \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFE00,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xFE00,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [276] = {.name = "GAL Ethernet PM History Data",                \
                 .mdmOid = MDMOID_GAL_ETHERNET_PM_HISTORY_DATA,         \
                 .parentMdmOid = MDMOID_ANI_MANAGEMENT,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xE000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [277] = {.name = "Priority Queue-G",                            \
                 .mdmOid = MDMOID_PRIORITY_QUEUE_G,                     \
                 .parentMdmOid = MDMOID_TRAFFIC_MANAGEMENT,             \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xC000,                                  \
                 .supportedMask = 0xE709,                               \
                 .optionalMask = 0x180F,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [278] = {.name = "Traffic Scheduler-G",                         \
                 .mdmOid = MDMOID_TRAFFIC_SCHEDULER_G,                  \
                 .parentMdmOid = MDMOID_TRAFFIC_MANAGEMENT,             \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x4000,                                  \
                 .supportedMask = 0xF000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [279] = {.name = "Protection Data",                             \
                 .mdmOid = MDMOID_PROTECTION_DATA,                      \
                 .parentMdmOid = MDMOID_EQUIPMENT_MANAGEMENT,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFC00,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0400,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [280] = {.name = "Traffic Descriptor",                          \
                 .mdmOid = MDMOID_GEM_TRAFFIC_DESCRIPTOR,               \
                 .parentMdmOid = MDMOID_TRAFFIC_MANAGEMENT,             \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFF00,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xF000,                               \
                 .optionalMask = 0xFF00,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [281] = {.name = "Multicast GEM Interworking Termination Point",\
                 .mdmOid = MDMOID_MULTICAST_GEM_INTERWORKING_TP,        \
                 .parentMdmOid = MDMOID_ANI_MANAGEMENT,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xF300,                             \
                 .MibUploadAttributeList = (UINT16)~(OMCI_ME_ATTRIB_9 | OMCI_ME_ATTRIB_10), \
                 .tableAttrMask = 0x00C0,                               \
                 .roAttrMask = 0x0C00,                                  \
                 .supportedMask = 0xFF80,                               \
                 .optionalMask = 0x0C40,                                \
                 .custom_get = getMulticastGemInterworkingTp,           \
                 .custom_set = setMulticastGemInterworkingTp},          \
                                                                        \
        [287] = {.name = "OMCI",                                        \
                 .mdmOid = MDMOID_OMCI,                                 \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = OMCI_ME_FLAG_PRESENT,                         \
                 .nbrInstancesUpload = 0,                               \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0xC000,                               \
                 .roAttrMask = 0xC000,                                  \
                 .supportedMask = 0xC000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = getOmciMe,                               \
                 .custom_set = NULL},                                   \
                                                                        \
        [296] = {.name = "Ethernet PM History Data 3",                  \
                 .mdmOid = MDMOID_ETHERNET_PM_HISTORY_DATA3,            \
                 .parentMdmOid = MDMOID_ETHERNET_SERVICES,              \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0xFFFF,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [297] = {.name = "Port Mapping Package G",                      \
                 .mdmOid = MDMOID_PORT_MAPPING_PACKAGE_G,               \
                 .parentMdmOid = MDMOID_EQUIPMENT_MANAGEMENT,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = (UINT16)~(OMCI_ME_ATTRIB_10),\
                 .tableAttrMask = 0x0040,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x3FC0,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [298] = {.name = "Dot1 Rate Limiter",                           \
                 .mdmOid = MDMOID_DOT1_RATE_LIMITER,                    \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xF800,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x3800,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [307] = {.name = "Octet String",                                \
                 .mdmOid = MDMOID_OCTET_STRING,                         \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x3FFF,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [308] = {.name = "General Purpose Buffer",                      \
                 .mdmOid = MDMOID_GENERAL_PURPOSE_BUFFER,               \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = OMCI_ME_FLAG_DYNAMIC,                         \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x8000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x4000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x8000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [309] = {.name = "Multicast Operations Profile",                \
                 .mdmOid = MDMOID_MULTICAST_OPERATIONS_PROFILE,         \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFC79,                             \
                 .MibUploadAttributeList = (UINT16)~(OMCI_ME_ATTRIB_7 | OMCI_ME_ATTRIB_8 | OMCI_ME_ATTRIB_9),  \
                 .tableAttrMask = 0x0380,                               \
                 .roAttrMask = 0x0080,                                  \
                 .supportedMask = 0xFE03,                               \
                 .optionalMask = 0x1CFF,                                \
                 .custom_get = getMulticastOperationsProfile,           \
                 .custom_set = setMulticastOperationsProfile},          \
                                                                        \
        [310] = {.name = "Multicast Subscriber Config Info",            \
                 .mdmOid = MDMOID_MULTICAST_SUBSCRIBER_CONFIG_INFO,     \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xF800,                             \
                 .MibUploadAttributeList = (UINT16)~(OMCI_ME_ATTRIB_6 | OMCI_ME_ATTRIB_7),  \
                 .tableAttrMask = 0x0600,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xF000,                               \
                 .optionalMask = 0x3E00,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [311] = {.name = "Multicast Subscriber Monitor",                \
                 .mdmOid = MDMOID_MULTICAST_SUBSCRIBER_MONITOR,         \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x8000,                             \
                 .MibUploadAttributeList = (UINT16)~(OMCI_ME_ATTRIB_5 | OMCI_ME_ATTRIB_6), \
                 .tableAttrMask = 0x0C00,                               \
                 .roAttrMask = 0x7C00,                                  \
                 .supportedMask = 0xF800,                               \
                 .optionalMask = 0x7400,                                \
                 .custom_get = getMulticastSubscriberMonitor,           \
                 .custom_set = NULL},                                   \
                                                                        \
        [312] = {.name = "FEC PM History Data",                         \
                 .mdmOid = MDMOID_FEC_PM_HISTORY_DATA,                  \
                 .parentMdmOid = MDMOID_ANI_MANAGEMENT,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0xFE00,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [321] = {.name = "Ethernet frame performance monitoring history data downstream", \
                 .mdmOid = MDMOID_DOWNSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA, \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0xFFFF,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [322] = {.name = "Ethernet frame performance monitoring history data upstream", \
                 .mdmOid = MDMOID_UPSTREAM_ETHERNET_FRAME_PM_HISTORY_DATA, \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0xFFFF,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [329] = {.name = "Virtual Ethernet Interface Point",            \
                 .mdmOid = MDMOID_VIRTUAL_ETHERNET_INTERFACE_POINT,     \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x4800,                                  \
                 .supportedMask = 0xF800,                               \
                 .optionalMask = 0x7800,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [330] = {.name = "Generic Status Portal",                       \
                 .mdmOid = MDMOID_GENERIC_STATUS_PORTAL,                \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x2000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0xC000,                               \
                 .roAttrMask = 0xC000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x2000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [332] = {.name = "Enhanced Security Control",                   \
                 .mdmOid = MDMOID_ESC,                                  \
                 .parentMdmOid = MDMOID_MISCELLANEOUS_SERVICES,         \
                 .flags = (OMCI_ME_FLAG_PRESENT |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0x4E20,                               \
                 .roAttrMask = 0x1CD0,                                  \
                 .supportedMask = 0x0040,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = getBroadcastKeyTableData,                \
                 .custom_set = setBroadcastKeyTableData},               \
                                                                        \
        [334] = {.name = "Ethernet Frame Extended PM",                  \
                 .mdmOid = MDMOID_ETHERNET_FRAME_EXTENDED_P_M,          \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0xFFFF,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [336] = {.name = "ONU Dynamic Power Management Control",        \
                 .mdmOid = MDMOID_DYNAMIC_POWER_MGMT,                   \
                 .parentMdmOid = MDMOID_EQUIPMENT_MANAGEMENT,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xB060,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x00E0,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [340] = {.name = "TR-069 management server",                    \
                 .mdmOid = MDMOID_TR069_MANAGEMENT_SERVER,              \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xE000,                                  \
                 .supportedMask = 0xC000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [341] = {.name = "GEM Port PM History Data",                    \
                 .mdmOid = MDMOID_GEM_PORT_PM_HISTORY_DATA,             \
                 .parentMdmOid = MDMOID_ANI_MANAGEMENT,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0xFE00,                               \
                 .optionalMask = 0x0200,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [342] = {.name = "TCP/UDP PM History Data",                     \
                 .mdmOid = MDMOID_TCP_UDP_PM_HISTORY_DATA,              \
                 .parentMdmOid = MDMOID_LAYER3_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [343] = {.name = "Energy Consumption PM History Data",          \
                 .mdmOid = MDMOID_ENERGY_CONSUMPTION_PM_HISTORY_DATA,   \
                 .parentMdmOid = MDMOID_ANI_MANAGEMENT,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0400,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [347] = {.name = "IPv6 Host Config Data",                       \
                 .mdmOid = MDMOID_IPV6_HOST_CONFIG_DATA,                \
                 .parentMdmOid = MDMOID_LAYER3_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = ~(OMCI_ME_ATTRIB_9 | OMCI_ME_ATTRIB_10 | OMCI_ME_ATTRIB_11 | OMCI_ME_ATTRIB_14), \
                 .tableAttrMask = 0x00E4,                               \
                 .roAttrMask = 0x50F4,                                  \
                 .supportedMask = 0xFFFC,                               \
                 .optionalMask = 0x000E,                                \
                 .custom_get = getIpv6HostConfigData,                   \
                 .custom_set = NULL},                                   \
                                                                        \
        [348] = {.name = "MAC BP ICMPv6 Process PreAssign Table",       \
                 .mdmOid = MDMOID_MAC_BP_ICMPV6_PROCESS_PREASSIGN_TABLE,\
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [425] = {.name = "Ethernet Frame Extended PM 64bit",            \
                 .mdmOid = MDMOID_ETHERNET_FRAME_EXTENDED_P_M64_BIT,    \
                 .parentMdmOid = MDMOID_LAYER2_DATA_SERVICES,           \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [426] = {.name = "Threshold Data 64bit",                        \
                 .mdmOid = MDMOID_THRESHOLD_DATA64_BIT,                 \
                 .parentMdmOid = MDMOID_GENERAL,                        \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \

#ifdef DMP_X_ITU_ORG_VOICE_1
#define OMCID_ME_INFO_VOIP                                              \
        [53]  = {.name = "PPTP POTS UNI",                               \
                 .mdmOid = MDMOID_PPTP_POTS_UNI,                        \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x00C0,                                  \
                 .supportedMask = 0x8BC0,                               \
                 .optionalMask = 0x7FF8,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [58]  = {.name = "Voice Service Profile",                       \
                 .mdmOid = MDMOID_VOICE_SERVICE,                        \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFF84,                             \
                 .MibUploadAttributeList = ~(OMCI_ME_ATTRIB_10 | OMCI_ME_ATTRIB_11 | OMCI_ME_ATTRIB_12 | OMCI_ME_ATTRIB_13), \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xFFC0,                               \
                 .optionalMask = 0x6FFC,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [138] = {.name = "VoIP Config Data",                            \
                 .mdmOid = MDMOID_VO_IP_CONFIG_DATA,                    \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xA500,                                  \
                 .supportedMask = 0xF000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [139] = {.name = "VoIP Voice CTP",                              \
                 .mdmOid = MDMOID_VO_IP_VOICE_CTP,                      \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xF000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xF000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [140] = {.name = "Call Control PM History Data",                \
                 .mdmOid = MDMOID_CALL_CONTROL_PM_HISTORY_DATA,         \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [141] = {.name = "VoIP Line Status",                            \
                 .mdmOid = MDMOID_VO_IP_LINE_STATUS,                    \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0xFF80,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0180,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [142] = {.name = "VoIP Media Profile",                          \
                 .mdmOid = MDMOID_VO_IP_MEDIA_PROFILE,                  \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFFFF,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xFC93,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [143] = {.name = "RTP Profile Data",                            \
                 .mdmOid = MDMOID_RTP_PROFILE_DATA,                     \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFE00,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xE600,                               \
                 .optionalMask = 0x4100,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [144] = {.name = "RTP PM History Data",                         \
                 .mdmOid = MDMOID_RTP_PM_HISTORY_DATA,                  \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0xF000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [145] = {.name = "Network Dial Plan Table",                     \
                 .mdmOid = MDMOID_NETWORK_DIAL_PLAN_TABLE,              \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x7800,                             \
                 .MibUploadAttributeList = ~(OMCI_ME_ATTRIB_6),         \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xFC00,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [146] = {.name = "VoIP Application Service Profile",            \
                 .mdmOid = MDMOID_VO_IP_APP_SERVICE_PROFILE,            \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFF00,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x7000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [147] = {.name = "VoIP Feature Access Codes",                   \
                 .mdmOid = MDMOID_VOICE_FEATURE_ACCESS_CODES,           \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xC630,                               \
                 .optionalMask = 0xFFF0,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \

#if defined(DMP_X_ITU_ORG_VOICE_SIP_1)
#define OMCID_ME_INFO_SIP                                               \
        [149] = {.name = "SIP Config Portal",                           \
                 .mdmOid = MDMOID_SIP_CONFIG_PORTAL,                    \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = OMCI_ME_FLAG_DYNAMIC,                         \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0x8000,                               \
                 .roAttrMask = 0x8000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [150] = {.name = "SIP Agent Config Data",                       \
                 .mdmOid = MDMOID_SIP_AGENT_CONFIG_DATA,                \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xF16C,                             \
                 .MibUploadAttributeList = ~(OMCI_ME_ATTRIB_12),        \
                 .tableAttrMask = 0x0010,                               \
                 .roAttrMask = 0x0080,                                  \
                 .supportedMask = 0xCF64,                               \
                 .optionalMask = 0x001E,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [151] = {.name = "SIP Agent PM History Data",                   \
                 .mdmOid = MDMOID_SIP_AGENT_PM_HISTORY_DATA,            \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x3FFE,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [152] = {.name = "SIP Call Initialization PM History Data",     \
                 .mdmOid = MDMOID_SIP_CALL_INIT_PM_HISTORY_DATA,        \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [153] = {.name = "SIP User Data",                               \
                 .mdmOid = MDMOID_SIP_USER_DATA,                        \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xDFC0,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0xF1C0,                               \
                 .optionalMask = 0x0038,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \

#define OMCID_ME_INFO_MGCP

#elif defined(DMP_X_ITU_ORG_VOICE_MGC_1)
#define OMCID_ME_INFO_SIP
#define OMCID_ME_INFO_MGCP                                              \
        [154] = {.name = "MGC Config Portal",                           \
                 .mdmOid = MDMOID_MGC_CONFIG_PORTAL,                    \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = OMCI_ME_FLAG_DYNAMIC,                         \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x0000,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_NONE,         \
                 .tableAttrMask = 0x8000,                               \
                 .roAttrMask = 0x8000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [155] = {.name = "MGC Config Data",                             \
                 .mdmOid = MDMOID_MGC_CONFIG_DATA,                      \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0xFA60,                             \
                 .MibUploadAttributeList = OMCI_ME_ATTRIB_ALL,          \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = 0x0000,                                  \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x07A0,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \
                                                                        \
        [156] = {.name = "MGC PM History Data",                         \
                 .mdmOid = MDMOID_MGC_PM_HISTORY_DATA,                  \
                 .parentMdmOid = MDMOID_VOICE_SERVICES,                 \
                 .flags = (OMCI_ME_FLAG_DYNAMIC |                       \
                           OMCI_ME_FLAG_MIB_UPLOAD),                    \
                 .nbrInstancesUpload = -1,                              \
                 .setByCreateMask = 0x4000,                             \
                 .MibUploadAttributeList = OMCI_ME_PM_THRES_ATTRIB,     \
                 .tableAttrMask = 0x0000,                               \
                 .roAttrMask = OMCI_ME_PM_RO_ATTRIB,                    \
                 .supportedMask = 0x0000,                               \
                 .optionalMask = 0x0000,                                \
                 .custom_get = NULL,                                    \
                 .custom_set = NULL},                                   \

#else /* !DMP_X_ITU_ORG_VOICE_MGC_1 && !DMP_X_ITU_ORG_VOICE_MGC_1 */

#define OMCID_ME_INFO_SIP
#define OMCID_ME_INFO_MGCP
#endif /* defined(DMP_X_ITU_ORG_VOICE_SIP_1) */

#else
#define OMCID_ME_INFO_VOIP
#define OMCID_ME_INFO_SIP
#define OMCID_ME_INFO_MGCP
#endif /* DMP_X_ITU_ORG_VOICE_1 */

#endif /* OMCID_ME_H */

