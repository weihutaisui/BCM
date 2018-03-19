/*
   Copyright (c) 2013 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2013:DUAL/GPL:standard
    
    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:
    
       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.
    
    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.
    
:>
*/

#include "rdd.h"


/******************************************************************************/
/*                                                                            */
/*                            Global Variables                                */
/*                                                                            */
/******************************************************************************/


void rdd_ipsec_sa_desc_table_address(uint32_t sa_table_addr, uint16_t sa_entry_size)
{
   uint32_t *ptr_32;
   uint16_t *ptr_16;
   int i;
   
   ptr_32 = (uint32_t *)(DEVICE_ADDRESS( RUNNER_PRIVATE_0_OFFSET ) + IPSEC_DS_DDR_SA_DESC_TABLE_PTR_ADDRESS);
   MWRITE_32(ptr_32, (uint32_t *)VIRT_TO_PHYS(sa_table_addr));

   ptr_16 = (uint16_t *)(DEVICE_ADDRESS( RUNNER_PRIVATE_0_OFFSET ) + IPSEC_DS_DDR_SA_DESC_SIZE_ADDRESS);
   MWRITE_16(ptr_16, sa_entry_size);

   ptr_16 = (uint16_t *)(DEVICE_ADDRESS( RUNNER_PRIVATE_0_OFFSET ) + IPSEC_DS_SA_DESC_CAM_TABLE_ADDRESS);
   for (i = 0; i < RDD_IPSEC_DS_SA_DESC_CAM_TABLE_SIZE; i++, ptr_16++)
      MWRITE_16(ptr_16, 0xFFFF);

   ptr_16 = (uint16_t *)(DEVICE_ADDRESS( RUNNER_PRIVATE_0_OFFSET ) + IPSEC_US_SA_DESC_CAM_TABLE_ADDRESS);
   for (i = 0; i < RDD_IPSEC_DS_SA_DESC_CAM_TABLE_SIZE; i++, ptr_16++)
      MWRITE_16(ptr_16, 0xFFFF);
}


BL_LILAC_RDD_ERROR_DTE rdd_ipsec_sa_desc_read(rdpa_traffic_dir dir, uint32_t index, void *val)
{
   uint8_t *sram_desc;

   if (dir == rdpa_dir_ds)
   {
      if (index >= RDD_IPSEC_DS_SA_DESC_TABLE_SIZE)
         return BL_LILAC_RDD_ERROR_ILLEGAL_IPSEC_SA_DESC_TABLE_INDEX;

      sram_desc = (uint8_t *)(DEVICE_ADDRESS( RUNNER_PRIVATE_0_OFFSET ) +
                              IPSEC_DS_SA_DESC_TABLE_ADDRESS +
                              (index * sizeof(RDD_IPSEC_SA_DESC_DTS)));
   }
   else
   {
      if (index >= RDD_IPSEC_US_SA_DESC_TABLE_SIZE)
         return BL_LILAC_RDD_ERROR_ILLEGAL_IPSEC_SA_DESC_TABLE_INDEX;

      sram_desc = (uint8_t *)(DEVICE_ADDRESS( RUNNER_PRIVATE_0_OFFSET ) +
                              IPSEC_US_SA_DESC_TABLE_ADDRESS +
                              (index * sizeof(RDD_IPSEC_SA_DESC_DTS)));
   }

   /* Read the descriptor from SRAM. */
   MREAD_BLK_8((uint8_t *)val, sram_desc, sizeof(RDD_IPSEC_SA_DESC_DTS));

   return BL_LILAC_RDD_OK;
}


BL_LILAC_RDD_ERROR_DTE rdd_ipsec_sa_desc_write(rdpa_traffic_dir dir, uint32_t index, const void *val)
{
   uint8_t *sram_desc;

   if (dir == rdpa_dir_ds)
   {
      if (index >= RDD_IPSEC_DS_SA_DESC_TABLE_SIZE)
         return BL_LILAC_RDD_ERROR_ILLEGAL_IPSEC_SA_DESC_TABLE_INDEX;

      sram_desc = (uint8_t *)(DEVICE_ADDRESS( RUNNER_PRIVATE_0_OFFSET ) +
                              IPSEC_DS_SA_DESC_TABLE_ADDRESS +
                              (index * sizeof(RDD_IPSEC_SA_DESC_DTS)));
   }
   else
   {
      if (index >= RDD_IPSEC_US_SA_DESC_TABLE_SIZE)
         return BL_LILAC_RDD_ERROR_ILLEGAL_IPSEC_SA_DESC_TABLE_INDEX;

      sram_desc = (uint8_t *)(DEVICE_ADDRESS( RUNNER_PRIVATE_0_OFFSET ) +
                              IPSEC_US_SA_DESC_TABLE_ADDRESS +
                              (index * sizeof(RDD_IPSEC_SA_DESC_DTS)));
   }

   if (*(uint32_t *)val)
      /* spi is not 0. Set the descriptor to SRAM. */
      MWRITE_BLK_8(sram_desc, (uint8_t *)val, sizeof(RDD_IPSEC_SA_DESC_DTS));
   else
      /* set spi to 0 to invalidate the descriptor entry */
      MWRITE_32(sram_desc, 0);

   return BL_LILAC_RDD_OK;
}


BL_LILAC_RDD_ERROR_DTE rdd_ipsec_sa_desc_cam_tbl_read(rdpa_traffic_dir dir, uint32_t index, void *val)
{
   uint16_t *ptr_16;

   if (dir == rdpa_dir_ds)
   {
      if (index >= RDD_IPSEC_DS_SA_DESC_CAM_TABLE_SIZE)
         return BL_LILAC_RDD_ERROR_ILLEGAL_IPSEC_SA_DESC_TABLE_INDEX;

      ptr_16 = (uint16_t *)(DEVICE_ADDRESS( RUNNER_PRIVATE_0_OFFSET ) +
                              IPSEC_DS_SA_DESC_CAM_TABLE_ADDRESS +
                              (index * sizeof(uint16_t)));
   }
   else
   {
      if (index >= RDD_IPSEC_US_SA_DESC_CAM_TABLE_SIZE)
         return BL_LILAC_RDD_ERROR_ILLEGAL_IPSEC_SA_DESC_TABLE_INDEX;

      ptr_16 = (uint16_t *)(DEVICE_ADDRESS( RUNNER_PRIVATE_0_OFFSET ) +
                              IPSEC_US_SA_DESC_CAM_TABLE_ADDRESS +
                              (index * sizeof(uint16_t)));
   }

   /* Read the descriptor from SRAM. */
   MREAD_16(ptr_16, *(uint16_t *)val);

   return BL_LILAC_RDD_OK;
}
