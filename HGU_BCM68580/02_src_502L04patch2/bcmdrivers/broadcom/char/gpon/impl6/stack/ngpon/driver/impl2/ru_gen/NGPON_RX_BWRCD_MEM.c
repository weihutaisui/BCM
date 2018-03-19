/*
* <:copyright-BRCM:2016:proprietary:gpon
* 
*    Copyright (c) 2016 Broadcom 
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

#include "ru.h"
#include "NGPON_BLOCKS.h"


#if RU_INCLUDE_FIELD_DB
/******************************************************************************
 * Field: NGPON_RX_BWRCD_MEM_MEM_MSB_STARTTIME
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWRCD_MEM_MEM_MSB_STARTTIME_FIELD =
{
    "STARTTIME",
#if RU_INCLUDE_DESC
    "Start_time",
    "Start time",
#endif
    NGPON_RX_BWRCD_MEM_MEM_MSB_STARTTIME_FIELD_MASK,
    0,
    NGPON_RX_BWRCD_MEM_MEM_MSB_STARTTIME_FIELD_WIDTH,
    NGPON_RX_BWRCD_MEM_MEM_MSB_STARTTIME_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWRCD_MEM_MEM_MSB_ALLOCID
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWRCD_MEM_MEM_MSB_ALLOCID_FIELD =
{
    "ALLOCID",
#if RU_INCLUDE_DESC
    "Alloc_ID",
    "Alloc ID",
#endif
    NGPON_RX_BWRCD_MEM_MEM_MSB_ALLOCID_FIELD_MASK,
    0,
    NGPON_RX_BWRCD_MEM_MEM_MSB_ALLOCID_FIELD_WIDTH,
    NGPON_RX_BWRCD_MEM_MEM_MSB_ALLOCID_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWRCD_MEM_MEM_MSB_RESERVED0
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWRCD_MEM_MEM_MSB_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    NGPON_RX_BWRCD_MEM_MEM_MSB_RESERVED0_FIELD_MASK,
    0,
    NGPON_RX_BWRCD_MEM_MEM_MSB_RESERVED0_FIELD_WIDTH,
    NGPON_RX_BWRCD_MEM_MEM_MSB_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWRCD_MEM_MEM_LSB_SFC_LS
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWRCD_MEM_MEM_LSB_SFC_LS_FIELD =
{
    "SFC_LS",
#if RU_INCLUDE_DESC
    "Superframe_LSB",
    "Least significant bits of the superframe counter",
#endif
    NGPON_RX_BWRCD_MEM_MEM_LSB_SFC_LS_FIELD_MASK,
    0,
    NGPON_RX_BWRCD_MEM_MEM_LSB_SFC_LS_FIELD_WIDTH,
    NGPON_RX_BWRCD_MEM_MEM_LSB_SFC_LS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWRCD_MEM_MEM_LSB_HEC_OK
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWRCD_MEM_MEM_LSB_HEC_OK_FIELD =
{
    "HEC_OK",
#if RU_INCLUDE_DESC
    "HEC_correct",
    "HEC received with this allocation record was correct",
#endif
    NGPON_RX_BWRCD_MEM_MEM_LSB_HEC_OK_FIELD_MASK,
    0,
    NGPON_RX_BWRCD_MEM_MEM_LSB_HEC_OK_FIELD_WIDTH,
    NGPON_RX_BWRCD_MEM_MEM_LSB_HEC_OK_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWRCD_MEM_MEM_LSB_BPROFILE
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWRCD_MEM_MEM_LSB_BPROFILE_FIELD =
{
    "BPROFILE",
#if RU_INCLUDE_DESC
    "Burst_profile",
    "Burst profile",
#endif
    NGPON_RX_BWRCD_MEM_MEM_LSB_BPROFILE_FIELD_MASK,
    0,
    NGPON_RX_BWRCD_MEM_MEM_LSB_BPROFILE_FIELD_WIDTH,
    NGPON_RX_BWRCD_MEM_MEM_LSB_BPROFILE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWRCD_MEM_MEM_LSB_FWI
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWRCD_MEM_MEM_LSB_FWI_FIELD =
{
    "FWI",
#if RU_INCLUDE_DESC
    "Force_wakeup",
    "Force wakeup",
#endif
    NGPON_RX_BWRCD_MEM_MEM_LSB_FWI_FIELD_MASK,
    0,
    NGPON_RX_BWRCD_MEM_MEM_LSB_FWI_FIELD_WIDTH,
    NGPON_RX_BWRCD_MEM_MEM_LSB_FWI_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWRCD_MEM_MEM_LSB_PLOAMU
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWRCD_MEM_MEM_LSB_PLOAMU_FIELD =
{
    "PLOAMU",
#if RU_INCLUDE_DESC
    "PLOAMu",
    "PLOAMu",
#endif
    NGPON_RX_BWRCD_MEM_MEM_LSB_PLOAMU_FIELD_MASK,
    0,
    NGPON_RX_BWRCD_MEM_MEM_LSB_PLOAMU_FIELD_WIDTH,
    NGPON_RX_BWRCD_MEM_MEM_LSB_PLOAMU_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWRCD_MEM_MEM_LSB_DBRU
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWRCD_MEM_MEM_LSB_DBRU_FIELD =
{
    "DBRU",
#if RU_INCLUDE_DESC
    "DBRu",
    "DBRu",
#endif
    NGPON_RX_BWRCD_MEM_MEM_LSB_DBRU_FIELD_MASK,
    0,
    NGPON_RX_BWRCD_MEM_MEM_LSB_DBRU_FIELD_WIDTH,
    NGPON_RX_BWRCD_MEM_MEM_LSB_DBRU_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

/******************************************************************************
 * Field: NGPON_RX_BWRCD_MEM_MEM_LSB_GRANTSIZE
 ******************************************************************************/
const ru_field_rec NGPON_RX_BWRCD_MEM_MEM_LSB_GRANTSIZE_FIELD =
{
    "GRANTSIZE",
#if RU_INCLUDE_DESC
    "Grant_size",
    "Grant size",
#endif
    NGPON_RX_BWRCD_MEM_MEM_LSB_GRANTSIZE_FIELD_MASK,
    0,
    NGPON_RX_BWRCD_MEM_MEM_LSB_GRANTSIZE_FIELD_WIDTH,
    NGPON_RX_BWRCD_MEM_MEM_LSB_GRANTSIZE_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_read
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: NGPON_RX_BWRCD_MEM_MEM_MSB
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_BWRCD_MEM_MEM_MSB_FIELDS[] =
{
    &NGPON_RX_BWRCD_MEM_MEM_MSB_STARTTIME_FIELD,
    &NGPON_RX_BWRCD_MEM_MEM_MSB_ALLOCID_FIELD,
    &NGPON_RX_BWRCD_MEM_MEM_MSB_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_BWRCD_MEM_MEM_MSB_REG = 
{
    "MEM_MSB",
#if RU_INCLUDE_DESC
    "MSB Register",
    "Most significant half",
#endif
    NGPON_RX_BWRCD_MEM_MEM_MSB_REG_OFFSET,
    NGPON_RX_BWRCD_MEM_MEM_MSB_REG_RAM_CNT,
    8,
    131,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    3,
    NGPON_RX_BWRCD_MEM_MEM_MSB_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Register: NGPON_RX_BWRCD_MEM_MEM_LSB
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *NGPON_RX_BWRCD_MEM_MEM_LSB_FIELDS[] =
{
    &NGPON_RX_BWRCD_MEM_MEM_LSB_SFC_LS_FIELD,
    &NGPON_RX_BWRCD_MEM_MEM_LSB_HEC_OK_FIELD,
    &NGPON_RX_BWRCD_MEM_MEM_LSB_BPROFILE_FIELD,
    &NGPON_RX_BWRCD_MEM_MEM_LSB_FWI_FIELD,
    &NGPON_RX_BWRCD_MEM_MEM_LSB_PLOAMU_FIELD,
    &NGPON_RX_BWRCD_MEM_MEM_LSB_DBRU_FIELD,
    &NGPON_RX_BWRCD_MEM_MEM_LSB_GRANTSIZE_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec NGPON_RX_BWRCD_MEM_MEM_LSB_REG = 
{
    "MEM_LSB",
#if RU_INCLUDE_DESC
    "LSB Register",
    "Least significant half",
#endif
    NGPON_RX_BWRCD_MEM_MEM_LSB_REG_OFFSET,
    NGPON_RX_BWRCD_MEM_MEM_LSB_REG_RAM_CNT,
    8,
    132,
#if RU_INCLUDE_ACCESS
    ru_access_read,
#endif
#if RU_INCLUDE_FIELD_DB
    7,
    NGPON_RX_BWRCD_MEM_MEM_LSB_FIELDS,
#endif /* RU_INCLUDE_FIELD_DB */
    ru_reg_size_32
};

/******************************************************************************
 * Block: NGPON_RX_BWRCD_MEM
 ******************************************************************************/
static const ru_reg_rec *NGPON_RX_BWRCD_MEM_REGS[] =
{
    &NGPON_RX_BWRCD_MEM_MEM_MSB_REG,
    &NGPON_RX_BWRCD_MEM_MEM_LSB_REG,
};

unsigned long NGPON_RX_BWRCD_MEM_ADDRS[] =
{
#if defined(CONFIG_BCM96858)
    0x80162000,
#elif defined(CONFIG_BCM96856)
    0x82db6000,
#else
#error "Wrong CONFIG_BCM!!!"
#endif
};

const ru_block_rec NGPON_RX_BWRCD_MEM_BLOCK = 
{
    "NGPON_RX_BWRCD_MEM",
    NGPON_RX_BWRCD_MEM_ADDRS,
    1,
    2,
    NGPON_RX_BWRCD_MEM_REGS
};

/* End of file BCM6858_A0_NGPON_RX_BWRCD_MEM.c */
