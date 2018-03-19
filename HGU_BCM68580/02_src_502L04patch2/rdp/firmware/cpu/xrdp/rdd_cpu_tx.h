/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

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

#ifndef _RDD_CPU_TX_H_
#define _RDD_CPU_TX_H_

typedef struct
{
    void *sysb;             /**< Buffer pointer */
    void *data;             /**< Buffer pointer */
    uint32_t fpm_bn;        /**< Buffer number */
    uint16_t offset;        /**< Buffer offset */
    uint16_t length;        /**< Buffer length */
    uint8_t abs_flag;       /**< ABS/FPM */
    uint8_t sbpm_copy;      /**< copy to SBPM/FPM */
    uint8_t fpm_fallback;   /**< if no SBPM copy to FPM */
} pbuf_t;


typedef union
{
    uint32_t bn1_or_abs2_or_1588         :18  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    uint32_t data_1588                   :18  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
#ifndef FIRMWARE_LITTLE_ENDIAN
    struct
    {
        uint32_t ssid                    :4  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint32_t fpm_fallback            :1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint32_t sbpm_copy               :1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint32_t bn1_or_abs2             :12 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    };
#else
    struct
    {
        uint32_t bn1_or_abs2             :12 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint32_t sbpm_copy               :1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint32_t fpm_fallback            :1  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint32_t ssid                    :4  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    };
#endif
#ifndef FIRMWARE_LITTLE_ENDIAN
    struct
    {
        uint32_t lag_index               :2  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint32_t reserved2               :16 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    };
#else
    struct
    {
        uint32_t reserved2               :16 __PACKING_ATTRIBUTE_FIELD_LEVEL__;
        uint32_t lag_index               :2  __PACKING_ATTRIBUTE_FIELD_LEVEL__;
    };
#endif
} cpu_tx_bn1_or_abs2_or_1588;



static inline void rdd_cpu_tx_set_packet_descriptor(const rdpa_cpu_tx_info_t *info, pbuf_t *pbuf, RDD_CPU_TX_DESCRIPTOR_DTS *cpu_tx_descriptor)
{
    uintptr_t data_phys_addr = RDD_RSV_VIRT_TO_PHYS(pbuf->data);
    uintptr_t sysb_phys_addr = RDD_RSV_VIRT_TO_PHYS(pbuf->sysb);
    cpu_tx_bn1_or_abs2_or_1588 bn1_or_abs2_or_1588;
    bn1_or_abs2_or_1588.bn1_or_abs2_or_1588 = cpu_tx_descriptor->bn1_or_abs2_or_1588;
    
    cpu_tx_descriptor->abs = pbuf->abs_flag;
    bn1_or_abs2_or_1588.fpm_fallback = pbuf->fpm_fallback;
    bn1_or_abs2_or_1588.sbpm_copy = pbuf->sbpm_copy;
    cpu_tx_descriptor->packet_length = pbuf->length;
    cpu_tx_descriptor->valid = 1;
    cpu_tx_descriptor->agg_pd = 0;
    cpu_tx_descriptor->target_mem_0 = 0;
    if (!cpu_tx_descriptor->abs)
    {
        cpu_tx_descriptor->payload_offset_or_abs_1 = pbuf->offset;
        cpu_tx_descriptor->buffer_number_0_or_abs_0 = pbuf->fpm_bn;
        if (unlikely(info->ptp_info))
        {
            cpu_tx_descriptor->flag_1588 = 1;
            bn1_or_abs2_or_1588.data_1588 = info->ptp_info; /*only 18 bits will be copied */
        }
    }
    else
    {
        cpu_tx_descriptor->abs_data0 = (data_phys_addr) & 0x3FFFF;
        cpu_tx_descriptor->abs_data1 = ((data_phys_addr) >> 18) & 0x3FFFFF;
        cpu_tx_descriptor->buffer_number_0_or_abs_0 = (sysb_phys_addr) & 0x3FFFF;
        cpu_tx_descriptor->payload_offset_or_abs_1 = ((sysb_phys_addr) >> 18) & 0x7FF; 
        bn1_or_abs2_or_1588.bn1_or_abs2 = ((sysb_phys_addr) >> 29) & 0xFFF;
    }

    cpu_tx_descriptor->bn1_or_abs2_or_1588 = bn1_or_abs2_or_1588.bn1_or_abs2_or_1588;
}
#endif /* _RDD_CPU_TX_H_ */
