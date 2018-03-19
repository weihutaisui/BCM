
#ifndef _TC_TO_Q_H
#define _TC_TO_Q_H

#include "rdd.h"
#include "rdd_defs.h"

#define RDPA_CS_TC_TO_QUEUE_TABLE_SIZE 32
#define RDPA_BS_TC_TO_QUEUE_TABLE_SIZE 8
/* TODO: maybe ifdef on not G9991 RDD_DS_QOS_MAPPER_ID_MAX_TABLES = 8*/
#define RDD_DS_QOS_MAPPER_ID_MAX_TABLES   32    /* Max number of TC-to-QM_QUEUE DS mapping tables. */
#define RDD_US_QOS_MAPPER_ID_MAX_TABLES   32    /* Max number of TC-to-QM_QUEUE US mapping tables. */
/* The last allocated TC2Q table is used as an invalid table and cannot be used */
#define RDD_QOS_MAPPER_ID_MAX_TABLES      (RDD_TC_TO_QUEUE_TABLE_SIZE - 1)
#define RDD_QOS_MAPPER_INVALID_TABLE       RDD_TC_TO_QUEUE_TABLE_SIZE

/* API to RDPA level */
bdmf_error_t rdd_tc_to_queue_entry_set(uint16_t port_or_wan_flow, rdpa_traffic_dir dir, uint8_t *size, 
    uint8_t tc, uint16_t qm_queue_index);
bdmf_error_t rdd_realloc_tc_to_queue_table(uint16_t port, rdpa_traffic_dir dir, uint8_t *size);
void rdd_qos_mapper_invalidate_table(uint16_t port_or_wan_flow, rdpa_traffic_dir dir, uint8_t size);
/* Internal RDD functions */
void rdd_qos_mapper_init(void);
bdmf_error_t rdd_pbit_to_queue_entry_set(uint16_t port_or_wan_flow, rdpa_traffic_dir dir, uint8_t pbit, 
    uint16_t qm_queue_index);
bdmf_error_t rdd_us_pbits_to_wan_flow_entry_cfg(uint8_t gem_mapping_table, uint8_t pbit, uint8_t gem);
void rdd_qos_mapper_set_table_id_to_tx_flow(uint16_t src_tx_flow, uint16_t dst_tx_flow);

#endif




