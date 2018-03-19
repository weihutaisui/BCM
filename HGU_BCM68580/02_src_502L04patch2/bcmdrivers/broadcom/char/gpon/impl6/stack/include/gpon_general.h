/*
* <:copyright-BRCM:2013:proprietary:gpon
* 
*    Copyright (c) 2013 Broadcom 
*    All Rights Reserved
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
* :> 
*/

#ifndef __GPON_GENERAL_H
#define __GPON_GENERAL_H

#include <packing.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************/
/**                                                                        */
/** GPON General definitions.                                              */
/**                                                                        */
/** Title:                                                                 */ 
/**    GPON software general definitions   */
/**                                                                        */
/** Abstract:                                                              */
/**                                                                        */
/**                                                                        */
/****************************************************************************/

#define CE_DS_FRAMES_IN_MS 8 
#define MS_DS_FRAMES_TO_MILISEC(frames) (frames/CE_DS_FRAMES_IN_MS)
#define GPON_MILISEC_TO_TICKS(m) ((m * (gs_gpon_database.os_resources.os_frequency)) / 1000)

/* Low */
#define LOW BDMF_FALSE
/* High */
#define HIGH BDMF_TRUE

/******************************************************************************/
/* 3 bits describing the downstream and upstream rate of the link.            */
/******************************************************************************/
typedef uint8_t GPON_PLOAM_TRANSMISSION_NUMBER;

/* Number of availiable transmition times */
#define GPON_ONE_TRANSMISSION_NUMBER    1 
#define GPON_TWO_TRANSMISSION_NUMBER    2 
#define GPON_THREE_TRANSMISSION_NUMBER  3 

/* Number of password messages to send ( 3 transmission ) */
#define GPON_PASSWORD_TRANSMISSION_NUMBER  GPON_THREE_TRANSMISSION_NUMBER 

/* Number of vendor specific messages to send ( 1 transmission ) */
#define GPON_VENDOR_SPECIFIC_TRANSMISSION_NUMBER  GPON_ONE_TRANSMISSION_NUMBER 

/* Number of pee alarm messages to send ( 1 transmission ) */
#define GPON_PEE_ALARM_TRANSMISSION_NUMBER  GPON_ONE_TRANSMISSION_NUMBER 

/* Number of pst alarm messages to send ( 1 transmission ) */
#define GPON_PST_ALARM_TRANSMISSION_NUMBER  GPON_ONE_TRANSMISSION_NUMBER

/* Number of Dying gasp messages to send ( 3 transmission ) */
#define GPON_DYING_GASP_TRANSMISSION_NUMBER  GPON_THREE_TRANSMISSION_NUMBER 

/* Number of rei messages to send ( 1 transmission ) */
#define GPON_REI_TRANSMISSION_NUMBER  GPON_ONE_TRANSMISSION_NUMBER 

/* Number of ack messages to send ( 1 transmission ) */
#define GPON_ACK_TRANSMISSION_NUMBER  GPON_ONE_TRANSMISSION_NUMBER 

/* Number of Sleep Request messages to send ( 1 transmission ) */
#define GPON_SLEEP_REQUEST_TRANSMISSION_NUMBER  GPON_ONE_TRANSMISSION_NUMBER 


/* BW recording Bit fields */
#ifndef _BYTE_ORDER_LITTLE_ENDIAN_
typedef struct
{
    uint32_t upper;
    uint32_t lower;
}
bwmap_record_rdata;

typedef struct
{
    uint32_t alloc_id: 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t flag_pls: 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t flag_ploam: 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t flag_fec: 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t flag_dbru: 2  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t flag_reserved: 6  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t crc: 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t sstart: 16 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t sstop: 16 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t sf_counter: 8  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}__PACKING_ATTRIBUTE_STRUCT_END__ bw_map_fields;
#else
typedef struct
{
    uint32_t lower;
    uint32_t upper;
}
bwmap_record_rdata;

typedef struct
{
    uint32_t sf_counter: 8  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t sstop: 16 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t sstart: 16 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t crc: 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t flag_reserved: 6  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t flag_dbru: 2  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t flag_fec: 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t flag_ploam: 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t flag_pls: 1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t alloc_id: 12 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
}__PACKING_ATTRIBUTE_STRUCT_END__ bw_map_fields;
#endif 

typedef union
{
    bwmap_record_rdata data;
    bw_map_fields map_fields;
}
GPON_ACCESS;

/* GPON Port range */
#define GPON_PORT_ID_LOW ((PON_PORT_ID_DTE) 0)
#define GPON_PORT_ID_HIGH ((PON_PORT_ID_DTE) 4096)
#define GPON_PORT_ID_UNASSIGNED ((PON_PORT_ID_DTE) 0xffff)


#ifdef __cplusplus
}
#endif

#endif



