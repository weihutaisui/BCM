/*
   Copyright (c) 2015 Broadcom
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



/* This is an automated file. Do not edit its contents. */


#include "bdmf_shell.h"
#ifdef RDP_SIM
#include "rdd_platform.h"
#endif
#include "access_macros.h"
#include "rdd_runner_reg_dump.h"

void dump_RDD_IPTV_DDR_CONTEXT_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register IPTV_DDR_CONTEXT_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\tvalid                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 6, 1, r);
	bdmf_session_print(session, "\tip_ver                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 5, 1, r);
	bdmf_session_print(session, "\tany_vid                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 4, 1, r);
	bdmf_session_print(session, "\tany_src_ip               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p, 0, 12, r);
	bdmf_session_print(session, "\tvid                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tnext_entry_idx           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tssid_vector_0_or_flooding_vport= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 6, r);
	bdmf_session_print(session, "\tssid_vector_1            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tssid_vector_2            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 10, 5, 3, r);
	bdmf_session_print(session, "\tradio_dhd_vector         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 10, 0, 5, r);
	bdmf_session_print(session, "\treserved3                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 11, 7, 1, r);
	bdmf_session_print(session, "\twlan_mcast_proxy_enabled = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 11, 0, 7, r);
	bdmf_session_print(session, "\twlan_mcast_index         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 12, 7, 1, r);
	bdmf_session_print(session, "\treserved2                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 12, 4, 3, r);
	bdmf_session_print(session, "\twlan_mcast_tx_prio       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 12, 0, 4, r);
	bdmf_session_print(session, "\treserved0                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 13, 6, 2, r);
	bdmf_session_print(session, "\tpool_num                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 13, 0, 6, r);
	bdmf_session_print(session, "\treplications             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 14, 2, 6, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 14, 0, 10, r);
	bdmf_session_print(session, "\tcntr_id                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 16, r);
	bdmf_session_print(session, "\tegress_ports_vector      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 20, r);
	bdmf_session_print(session, "\tsrc_ip                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	bdmf_session_print(session, "\tsrc_ipv6_addr            =\n\t");
	for (i=0,j=0; i<16; i++)
	{
		MREAD_I_8((uint8_t *)p + 24, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tdst_ipv6_addr            =\n\t");
	for (i=0,j=0; i<16; i++)
	{
		MREAD_I_8((uint8_t *)p + 40, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tresult                   =\n\t");
	for (i=0,j=0; i<32; i++)
	{
		MREAD_I_8((uint8_t *)p + 56, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_WLAN_MCAST_DHD_LIST_ENTRY_ARRAY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register WLAN_MCAST_DHD_LIST_ENTRY_ARRAY\n");

	bdmf_session_print(session, "\tdhd_station              =\n\t");
	for (i=0,j=0; i<64; i++)
	{
		MREAD_I_8((uint8_t *)p, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_WLAN_MCAST_DHD_LIST_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register WLAN_MCAST_DHD_LIST_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\tvalid                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 7, r);
	bdmf_session_print(session, "\tindex                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_PROCESSING_TX_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register PROCESSING_TX_DESCRIPTOR\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\tvalid                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 6, 1, r);
	bdmf_session_print(session, "\theadroom                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 5, 1, r);
	bdmf_session_print(session, "\tdont_agg                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 4, 1, r);
	bdmf_session_print(session, "\tmc_copy                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 3, 1, r);
	bdmf_session_print(session, "\treprocess                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 2, 1, r);
	bdmf_session_print(session, "\tcolor                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 1, 1, r);
	bdmf_session_print(session, "\tforce_copy               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p, 0, 9, r);
	bdmf_session_print(session, "\tsecond_level_q           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 2, 7, 9, r);
	bdmf_session_print(session, "\tfirst_level_q            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 6, 1, r);
	bdmf_session_print(session, "\tflag_1588                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 5, 1, r);
	bdmf_session_print(session, "\tcoherent                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 0, 5, r);
	bdmf_session_print(session, "\thn                       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 4, 6, 10, r);
	bdmf_session_print(session, "\tserial_num               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 5, 5, 1, r);
	bdmf_session_print(session, "\tpriority                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 4, 14, 7, r);
	bdmf_session_print(session, "\tbn_num                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 6, 0, 14, r);
	bdmf_session_print(session, "\tpacket_length            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 8, 7, 1, r);
	bdmf_session_print(session, "\tdrop                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 8, 6, 1, r);
	bdmf_session_print(session, "\ttarget_mem_1             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 8, 4, 2, r);
	bdmf_session_print(session, "\tmcst_bcst_union          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 8, 3, 1, r);
	bdmf_session_print(session, "\tingress_cong             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 8, 2, 1, r);
	bdmf_session_print(session, "\tlan                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 8, 2, 8, r);
	bdmf_session_print(session, "\tingress_port             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 8, 0, 18, r);
	bdmf_session_print(session, "\tunion3                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 12, 7, 1, r);
	bdmf_session_print(session, "\tagg_pd                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 12, 6, 1, r);
	bdmf_session_print(session, "\ttarget_mem_0             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 12, 5, 1, r);
	bdmf_session_print(session, "\tabs                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 12, 2, 11, r);
	bdmf_session_print(session, "\tpayload_offset_or_abs_1  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 12, 0, 18, r);
	bdmf_session_print(session, "\tbuffer_number_0_or_abs_0 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_COMPLEX_SCHEDULER_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register COMPLEX_SCHEDULER_DESCRIPTOR\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\trate_limit_enable        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 6, 1, r);
	bdmf_session_print(session, "\tis_positive_budget       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 5, 1, r);
	bdmf_session_print(session, "\tbasic_scheduler_exists   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p, 7, 6, r);
	bdmf_session_print(session, "\tbbh_queue                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 0, 7, r);
	bdmf_session_print(session, "\trate_limiter_index       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 5, 3, r);
	bdmf_session_print(session, "\tdwrr_offset_pir          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 2, 3, r);
	bdmf_session_print(session, "\tdwrr_offset_sir          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 2, 5, 5, r);
	bdmf_session_print(session, "\tlast_served_block_pir    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 0, 5, r);
	bdmf_session_print(session, "\tlast_served_block_sir    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tstatus_bit_vector        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tslot_budget_bit_vector_0 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\tslot_budget_bit_vector_1 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 16, 7, 1, r);
	bdmf_session_print(session, "\tovl_rl_en                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 16, 0, 31, r);
	bdmf_session_print(session, "\treserved3                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	bdmf_session_print(session, "\treserved1                =\n\t");
	for (i=0,j=0; i<12; i++)
	{
		MREAD_I_8((uint8_t *)p + 20, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tblock_index              =\n\t");
	for (i=0,j=0; i<32; i++)
	{
		MREAD_I_8((uint8_t *)p + 32, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_SCHEDULING_QUEUE_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register SCHEDULING_QUEUE_DESCRIPTOR\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\tprefetch_pd              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 7, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 7, 1, r);
	bdmf_session_print(session, "\tblock_type               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 6, 1, r);
	bdmf_session_print(session, "\tenable                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 0, 6, r);
	bdmf_session_print(session, "\tbbh_queue_index          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 7, 1, r);
	bdmf_session_print(session, "\trate_limit_enable        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 0, 7, r);
	bdmf_session_print(session, "\tscheduler_index          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 3, r);
	bdmf_session_print(session, "\tqueue_bit_mask           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\trate_limiter_index       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 5, r);
	bdmf_session_print(session, "\tquantum_number           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 6, r);
	bdmf_session_print(session, "\tdeficit_counter          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_TM_FLOW_CNTR_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register TM_FLOW_CNTR_ENTRY\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\tcntr_id                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register BBH_TX_EGRESS_COUNTER_ENTRY\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\tcounter                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 0, 0, 24, r);
	bdmf_session_print(session, "\treserved0                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_BYTES_4(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register BYTES_4\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tbits                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_BASIC_SCHEDULER_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register BASIC_SCHEDULER_DESCRIPTOR\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\trate_limit_enable        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 6, 1, r);
	bdmf_session_print(session, "\tcomplex_scheduler_exists = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 4, 2, r);
	bdmf_session_print(session, "\tdwrr_offset              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 1, 3, r);
	bdmf_session_print(session, "\tlast_served_queue        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p, 0, 9, r);
	bdmf_session_print(session, "\tparent_index             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tslot_budget_bit_vector   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 3, r);
	bdmf_session_print(session, "\tstatus_bit_vector        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\trate_limiter_index       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 5, r);
	bdmf_session_print(session, "\tquantum_number           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 6, r);
	bdmf_session_print(session, "\tdeficit_counter          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	bdmf_session_print(session, "\tqueue_index              =\n\t");
	for (i=0,j=0; i<8; i++)
	{
		MREAD_I_8((uint8_t *)p + 8, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_BASIC_RATE_LIMITER_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register BASIC_RATE_LIMITER_DESCRIPTOR\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\trl_type                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p, 10, 21, r);
	bdmf_session_print(session, "\treserved2                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 0, 2, r);
	bdmf_session_print(session, "\tblock_type               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 3, r);
	bdmf_session_print(session, "\tblock_index              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tcurrent_budget           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 8, 2, 14, r);
	bdmf_session_print(session, "\talloc_mantissa           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 9, 0, 2, r);
	bdmf_session_print(session, "\talloc_exponent           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 10, 2, 14, r);
	bdmf_session_print(session, "\tlimit_mantissa           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 11, 0, 2, r);
	bdmf_session_print(session, "\tlimit_exponent           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_TRACE_EVENT(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register TRACE_EVENT\n");

	FIELD_MREAD_16((uint8_t *)p, 4, 12, r);
	bdmf_session_print(session, "\ttimestamp                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 2, 2, r);
	bdmf_session_print(session, "\tevent_id                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 0, 0, 18, r);
	bdmf_session_print(session, "\ttrace_event_info         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_BYTES_2(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register BYTES_2\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\tbits                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_BYTE_1(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register BYTE_1\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\tbits                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_BBH_TX_QUEUE_ID_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register BBH_TX_QUEUE_ID_ENTRY\n");

	FIELD_MREAD_32((uint8_t *)p, 8, 24, r);
	bdmf_session_print(session, "\tbytes_in_bbh             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 3, r);
	bdmf_session_print(session, "\tqueue                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_UPDATE_FIFO_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register UPDATE_FIFO_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\tvalid                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 6, 1, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p, 0, 14, r);
	bdmf_session_print(session, "\tpd_fifo_write_ptr        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 1, 7, r);
	bdmf_session_print(session, "\treserved0                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 2, 0, 9, r);
	bdmf_session_print(session, "\tqueue_number             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_NATC_TBL_CONFIGURATION(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register NATC_TBL_CONFIGURATION\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tkey_addr_low             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tkey_addr_high            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tres_addr_low             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\tres_addr_high            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 16, 1, 7, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 16, 0, 1, r);
	bdmf_session_print(session, "\tmiss_cache_enable        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 17, r);
	bdmf_session_print(session, "\tkey_size                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 18, r);
	bdmf_session_print(session, "\tcontext_size             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	bdmf_session_print(session, "\treserved1                =\n\t");
	for (i=0,j=0; i<5; i++)
	{
		MREAD_I_8((uint8_t *)p + 19, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_MIRRORING_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register MIRRORING_DESCRIPTOR\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\tmirror_en                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 1, 6, r);
	bdmf_session_print(session, "\tdst_vport                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p, 0, 9, r);
	bdmf_session_print(session, "\tdst_queue                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register QUEUE_DYNAMIC_MNG_ENTRY\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\tqm_queue_us_start        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 1, r);
	bdmf_session_print(session, "\tqm_queue_us_end          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tqm_queue_ds_start        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 3, r);
	bdmf_session_print(session, "\tqm_queue_ds_end          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tqm_queue_epon_start      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 4, 0, 24, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_BYTES_8(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register BYTES_8\n");

	bdmf_session_print(session, "\tbits                     =\n\t");
	for (i=0,j=0; i<2; i++)
	{
		MREAD_I_32((uint8_t *)p, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_FPM_GLOBAL_CFG(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register FPM_GLOBAL_CFG\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tfpm_base_low             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tfpm_base_high            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tfpm_token_size_asr_8     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 8, 0, 24, r);
	bdmf_session_print(session, "\treserved0                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_BBH_QUEUE_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register BBH_QUEUE_DESCRIPTOR\n");

	FIELD_MREAD_8((uint8_t *)p, 2, 6, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 1, 1, r);
	bdmf_session_print(session, "\tmirroring_en             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 1, r);
	bdmf_session_print(session, "\tpriority                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 1, 7, r);
	bdmf_session_print(session, "\tscheduler_index          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 0, 1, r);
	bdmf_session_print(session, "\tscheduler_type           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tbb_destination           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 3, r);
	bdmf_session_print(session, "\tingress_counter          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DHD_FLOW_RING_CACHE_CTX_ENTRY\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tflow_ring_base_low       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tflow_ring_base_high      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 5, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 6, r);
	bdmf_session_print(session, "\tshadow_wr_idx            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tsize                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 10, r);
	bdmf_session_print(session, "\tssid                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 11, r);
	bdmf_session_print(session, "\tflags                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\trd_idx                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 14, r);
	bdmf_session_print(session, "\twr_idx                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DHD_POST_COMMON_RADIO_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register DHD_POST_COMMON_RADIO_ENTRY\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\ttx_post_mgmt_fr_base_ptr_low= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\ttx_post_mgmt_fr_base_ptr_high= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\ttx_post_fr_wr_idx_base_ptr_low= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\ttx_post_fr_wr_idx_base_ptr_high= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 16, r);
	bdmf_session_print(session, "\ttx_post_fr_rd_idx_base_ptr_low= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 20, r);
	bdmf_session_print(session, "\ttx_post_fr_rd_idx_base_ptr_high= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 24, r);
	bdmf_session_print(session, "\tdhd_doorbell_low         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 28, r);
	bdmf_session_print(session, "\tdhd_doorbell_high        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 32, r);
	bdmf_session_print(session, "\ttx_post_wr_idx           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 34, r);
	bdmf_session_print(session, "\ttx_post_rd_idx           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 36, r);
	bdmf_session_print(session, "\tcam_rslt                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 40, 7, 1, r);
	bdmf_session_print(session, "\tadd_llcsnap_header       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 40, 6, 1, r);
	bdmf_session_print(session, "\taggregation_bypass_cpu_tx= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 40, 5, 1, r);
	bdmf_session_print(session, "\taggregation_bypass_non_udp_tcp= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 40, 2, 3, r);
	bdmf_session_print(session, "\tflow_ring_format         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 40, 0, 2, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 41, r);
	bdmf_session_print(session, "\taggregation_bypass_tcp_pktlen= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 42, r);
	bdmf_session_print(session, "\tcache_ctx_next_write_idx = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 43, r);
	bdmf_session_print(session, "\treserved2                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	bdmf_session_print(session, "\tper_ac_aggregation_thresholds=\n\t");
	for (i=0,j=0; i<4; i++)
	{
		MREAD_I_8((uint8_t *)p + 44, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tper_ac_aggregation_timeouts=\n\t");
	for (i=0,j=0; i<4; i++)
	{
		MREAD_I_8((uint8_t *)p + 48, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tper_ac_aggregation_timeout_cntrs=\n\t");
	for (i=0,j=0; i<4; i++)
	{
		MREAD_I_8((uint8_t *)p + 52, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tscratchpad               =\n\t");
	for (i=0,j=0; i<3; i++)
	{
		MREAD_I_32((uint8_t *)p + 56, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\treserved3                =\n\t");
	for (i=0,j=0; i<4; i++)
	{
		MREAD_I_8((uint8_t *)p + 68, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_PROCESSING_RX_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register PROCESSING_RX_DESCRIPTOR\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tpd_info                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 4, 6, 10, r);
	bdmf_session_print(session, "\tserial_num               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 5, 5, 1, r);
	bdmf_session_print(session, "\tploam                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 4, 14, 7, r);
	bdmf_session_print(session, "\tbn_num                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 6, 0, 14, r);
	bdmf_session_print(session, "\tpacket_length            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 8, 7, 1, r);
	bdmf_session_print(session, "\terror                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 8, 6, 1, r);
	bdmf_session_print(session, "\ttarget_mem_1             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 8, 4, 2, r);
	bdmf_session_print(session, "\tcong_state               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 8, 3, 1, r);
	bdmf_session_print(session, "\tingress_cong             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 8, 2, 1, r);
	bdmf_session_print(session, "\tlan                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 8, 2, 8, r);
	bdmf_session_print(session, "\tingress_port             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 8, 0, 18, r);
	bdmf_session_print(session, "\tbn1_last_or_abs1         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 12, 7, 1, r);
	bdmf_session_print(session, "\tagg_pd                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 12, 6, 1, r);
	bdmf_session_print(session, "\ttarget_mem_0             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 12, 5, 1, r);
	bdmf_session_print(session, "\tabs                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 12, 1, 4, r);
	bdmf_session_print(session, "\terror_type_or_cpu_tx     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 12, 2, 7, r);
	bdmf_session_print(session, "\tsop                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 12, 0, 18, r);
	bdmf_session_print(session, "\tbn0_first                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tmac_address_high         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tmac_address_low          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 6, r);
	bdmf_session_print(session, "\treference_count          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 7, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_CPU_RX_METER_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register CPU_RX_METER_ENTRY\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\tcurrent_budget           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tbudget_limit             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tallocated_budget         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 6, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_RX_FLOW_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register RX_FLOW_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 3, 5, r);
	bdmf_session_print(session, "\tvirtual_port             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 2, 1, r);
	bdmf_session_print(session, "\tflow_dest                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 1, 1, r);
	bdmf_session_print(session, "\texception                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 1, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 1, r);
	bdmf_session_print(session, "\tcntr_id                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register CPU_RING_INTERRUPT_COUNTER_ENTRY\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tcounter                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tmax_size                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_FPM_POOL_NUMBER(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register FPM_POOL_NUMBER\n");

	FIELD_MREAD_8((uint8_t *)p, 2, 6, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 2, r);
	bdmf_session_print(session, "\tpool_number              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DHD_TX_POST_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DHD_TX_POST_DESCRIPTOR\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\tmsg_type                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 1, r);
	bdmf_session_print(session, "\tif_id                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tcommon_hdr_flags         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 3, r);
	bdmf_session_print(session, "\tepoch                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\trequest_id               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\ttx_eth_hdr_0             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\ttx_eth_hdr_1             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 16, r);
	bdmf_session_print(session, "\ttx_eth_hdr_2             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 20, r);
	bdmf_session_print(session, "\ttx_eth_hdr_3             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 22, r);
	bdmf_session_print(session, "\tflags                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 23, r);
	bdmf_session_print(session, "\tseg_cnt                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 24, r);
	bdmf_session_print(session, "\tmetadata_buf_addr_low    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 28, r);
	bdmf_session_print(session, "\tmetadata_buf_addr_hi     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 32, r);
	bdmf_session_print(session, "\tdata_buf_addr_low        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 36, r);
	bdmf_session_print(session, "\tdata_buf_addr_hi         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 40, r);
	bdmf_session_print(session, "\tmeta_buf_len             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 42, r);
	bdmf_session_print(session, "\tdata_len                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 44, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register CPU_INTERRUPT_COALESCING_ENTRY\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\ttimer_period             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\ttimer_armed              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tscratch                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\treserved0                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_WLAN_MCAST_DHD_STATION_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register WLAN_MCAST_DHD_STATION_ENTRY\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\tmac_address_high         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tmac_address_mid          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tmac_address_low          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 6, 6, 2, r);
	bdmf_session_print(session, "\tradio_index              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 6, 5, 1, r);
	bdmf_session_print(session, "\tvalid                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 6, 2, 3, r);
	bdmf_session_print(session, "\ttx_priority              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 6, 0, 10, r);
	bdmf_session_print(session, "\tflowring_index           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DDR_ADDRESS(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DDR_ADDRESS\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tlow                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\thigh                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DHD_FLOW_RING_CACHE_LKP_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\tinvalid                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 2, 5, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p, 0, 10, r);
	bdmf_session_print(session, "\tflow_ring_id             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DHD_DOORBELL(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DHD_DOORBELL\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tvalue                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_CPU_RING_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register CPU_RING_DESCRIPTOR\n");

	FIELD_MREAD_8((uint8_t *)p, 3, 5, r);
	bdmf_session_print(session, "\tsize_of_entry            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p, 0, 11, r);
	bdmf_session_print(session, "\tnumber_of_entries        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tinterrupt_id             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tdrop_counter             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 6, r);
	bdmf_session_print(session, "\twrite_idx                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tbase_addr_low            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\tread_idx                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 14, r);
	bdmf_session_print(session, "\tlow_priority_threshold   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 15, r);
	bdmf_session_print(session, "\tbase_addr_high           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_PHYS_ADDR_64_PTR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register PHYS_ADDR_64_PTR\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tlow                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\thigh                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_MAC_TYPE_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register MAC_TYPE_ENTRY\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\ttype                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DHD_TX_POST_FLOW_RING_MGMT_DESCRIPTOR\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tflow_ring_base_low       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tflow_ring_base_high      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tsize_and_flags           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_WLAN_MCAST_DHD_STATION_CTX_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register WLAN_MCAST_DHD_STATION_CTX_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 4, 4, r);
	bdmf_session_print(session, "\treserved0                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 4, r);
	bdmf_session_print(session, "\tssid                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DHD_HW_CONFIGURATION(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DHD_HW_CONFIGURATION\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\taggr_timer_period        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 4, 3, 5, r);
	bdmf_session_print(session, "\treserved0                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 4, 0, 11, r);
	bdmf_session_print(session, "\tddr_sop_offset           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 6, r);
	bdmf_session_print(session, "\trx_post_fpm_pool         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 7, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tfpm_low_threshold        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 10, r);
	bdmf_session_print(session, "\tfpm_high_threshold       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_REPORTING_QUEUE_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register REPORTING_QUEUE_DESCRIPTOR\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\treported_bytes           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\treported_overhead        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tcounter_ptr              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	bdmf_session_print(session, "\treserved                 =\n\t");
	for (i=0,j=0; i<6; i++)
	{
		MREAD_I_8((uint8_t *)p + 10, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register DHD_COMPLETE_COMMON_RADIO_ENTRY\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\trx_complete_fr_base_ptr_low= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\trx_complete_fr_base_ptr_high= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\trx_complete_fr_rd_idx_ptr_low= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\trx_complete_fr_rd_idx_ptr_high= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 16, r);
	bdmf_session_print(session, "\trx_complete_fr_wr_idx_ptr_low= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 20, r);
	bdmf_session_print(session, "\trx_complete_fr_wr_idx_ptr_high= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 24, r);
	bdmf_session_print(session, "\ttx_complete_fr_base_ptr_low= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 28, r);
	bdmf_session_print(session, "\ttx_complete_fr_base_ptr_high= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 32, r);
	bdmf_session_print(session, "\ttx_complete_fr_rd_idx_ptr_low= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 36, r);
	bdmf_session_print(session, "\ttx_complete_fr_rd_idx_ptr_high= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 40, r);
	bdmf_session_print(session, "\ttx_complete_fr_wr_idx_ptr_low= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 44, r);
	bdmf_session_print(session, "\ttx_complete_fr_wr_idx_ptr_high= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 48, r);
	bdmf_session_print(session, "\trx_post_fr_base_ptr_low  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 52, r);
	bdmf_session_print(session, "\trx_post_fr_base_ptr_high = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 56, r);
	bdmf_session_print(session, "\trx_post_fr_rd_idx_ptr_low= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 60, r);
	bdmf_session_print(session, "\trx_post_fr_rd_idx_ptr_high= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 64, r);
	bdmf_session_print(session, "\trx_post_fr_wr_idx_ptr_low= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 68, r);
	bdmf_session_print(session, "\trx_post_fr_wr_idx_ptr_high= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 72, r);
	bdmf_session_print(session, "\tdhd_doorbell_low         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 76, r);
	bdmf_session_print(session, "\tdhd_doorbell_high        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 80, r);
	bdmf_session_print(session, "\trx_complete_rd_idx       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 82, r);
	bdmf_session_print(session, "\trx_complete_wr_idx       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 84, r);
	bdmf_session_print(session, "\ttx_complete_rd_idx       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 86, r);
	bdmf_session_print(session, "\ttx_complete_wr_idx       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 88, r);
	bdmf_session_print(session, "\trx_post_wr_idx           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 90, 5, 3, r);
	bdmf_session_print(session, "\tflow_ring_format         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 90, 0, 5, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	bdmf_session_print(session, "\treserved1                =\n\t");
	for (i=0,j=0; i<5; i++)
	{
		MREAD_I_8((uint8_t *)p + 91, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tscratchpad               =\n\t");
	for (i=0,j=0; i<3; i++)
	{
		MREAD_I_32((uint8_t *)p + 96, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\treserved2                =\n\t");
	for (i=0,j=0; i<4; i++)
	{
		MREAD_I_8((uint8_t *)p + 108, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_REPORTING_ACCUMULATED_DATA(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register REPORTING_ACCUMULATED_DATA\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\taccumulated_bytes        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\taccumulated_overhead     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_TX_FLOW_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register TX_FLOW_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\tvalid                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 7, r);
	bdmf_session_print(session, "\tqos_table_ptr            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DHD_TX_COMPLETE_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DHD_TX_COMPLETE_DESCRIPTOR\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\tmsg_type                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 1, r);
	bdmf_session_print(session, "\tif_id                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tcommon_hdr_flags         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 3, r);
	bdmf_session_print(session, "\tepoch                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\trequest_id               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tstatus                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 10, r);
	bdmf_session_print(session, "\tflow_ring_id             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\tdma_done_mark            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DHD_RX_COMPLETE_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DHD_RX_COMPLETE_DESCRIPTOR\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\tmsg_type                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 1, r);
	bdmf_session_print(session, "\tif_id                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tcommon_hdr_flags         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 3, r);
	bdmf_session_print(session, "\tepoch                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\trequest_id               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tcompl_msg_hdr_status     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 10, r);
	bdmf_session_print(session, "\tflow_ring_id             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\tmeta_buf_len             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 14, r);
	bdmf_session_print(session, "\tdata_len                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 16, r);
	bdmf_session_print(session, "\tdata_offset              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 18, r);
	bdmf_session_print(session, "\tflags                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 20, r);
	bdmf_session_print(session, "\trx_status_0              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 24, r);
	bdmf_session_print(session, "\trx_status_1              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 28, r);
	bdmf_session_print(session, "\tdma_done_mark            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_GHOST_REPORTING_GLOBAL_CFG_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register GHOST_REPORTING_GLOBAL_CFG_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 5, 3, r);
	bdmf_session_print(session, "\tmac_type                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p, 0, 13, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\ttimer_interval           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_SPDSVC_GEN_PARAMS(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register SPDSVC_GEN_PARAMS\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tprocessing_tx_pd_0       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tprocessing_tx_pd_1       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tprocessing_tx_pd_2       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\tprocessing_tx_pd_3       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 16, r);
	bdmf_session_print(session, "\ttotal_copies             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 20, r);
	bdmf_session_print(session, "\tbucket                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 24, r);
	bdmf_session_print(session, "\ttimer_period             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 26, r);
	bdmf_session_print(session, "\tbucket_size              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 28, r);
	bdmf_session_print(session, "\ttotal_length             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 30, r);
	bdmf_session_print(session, "\ttokens                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 32, r);
	bdmf_session_print(session, "\ttx_packets               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 36, r);
	bdmf_session_print(session, "\ttx_dropped               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_TX_ABS_RECYCLE_COUNTERS_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register TX_ABS_RECYCLE_COUNTERS_ENTRY\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\ttotal                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tspdsvc                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DHD_RX_POST_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DHD_RX_POST_DESCRIPTOR\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\tmsg_type                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 1, r);
	bdmf_session_print(session, "\tif_id                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tcommon_hdr_flags         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 3, r);
	bdmf_session_print(session, "\tepoch                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\trequest_id               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tmeta_buf_len             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 10, r);
	bdmf_session_print(session, "\tdata_len                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 16, r);
	bdmf_session_print(session, "\tmetadata_buf_addr_low    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 20, r);
	bdmf_session_print(session, "\tmetadata_buf_addr_hi     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 24, r);
	bdmf_session_print(session, "\tdata_buf_addr_low        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 28, r);
	bdmf_session_print(session, "\tdata_buf_addr_hi         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DHD_RING_SIZE(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DHD_RING_SIZE\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\tvalue                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_OVERALL_RATE_LIMITER_DESCRIPTOR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register OVERALL_RATE_LIMITER_DESCRIPTOR\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tbbh_queue_en_vec         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tcurrent_budget           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 8, 2, 14, r);
	bdmf_session_print(session, "\talloc_mantissa           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 9, 0, 2, r);
	bdmf_session_print(session, "\talloc_exponent           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 10, 2, 14, r);
	bdmf_session_print(session, "\tlimit_mantissa           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 11, 0, 2, r);
	bdmf_session_print(session, "\tlimit_exponent           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_PACKET_BUFFER(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register PACKET_BUFFER\n");

	bdmf_session_print(session, "\tpacket_header            =\n\t");
	for (i=0,j=0; i<128; i++)
	{
		MREAD_I_8((uint8_t *)p, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tparser_result            =\n\t");
	for (i=0,j=0; i<56; i++)
	{
		MREAD_I_8((uint8_t *)p + 128, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tscratch                  =\n\t");
	for (i=0,j=0; i<136; i++)
	{
		MREAD_I_8((uint8_t *)p + 184, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tpd                       =\n\t");
	for (i=0,j=0; i<16; i++)
	{
		MREAD_I_8((uint8_t *)p + 320, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tresolution_context       =\n\t");
	for (i=0,j=0; i<16; i++)
	{
		MREAD_I_8((uint8_t *)p + 336, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tclassification_results   =\n\t");
	for (i=0,j=0; i<8; i++)
	{
		MREAD_I_32((uint8_t *)p + 352, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tclassification_contexts_list=\n\t");
	for (i=0,j=0; i<128; i++)
	{
		MREAD_I_8((uint8_t *)p + 384, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_LAYER2_HEADER_COPY_MAPPING_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register LAYER2_HEADER_COPY_MAPPING_ENTRY\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\troutine                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tdst_buffer_offset        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_HW_IPTV_CONFIGURATION(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register HW_IPTV_CONFIGURATION\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\tfpm_base_token_size      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 7, 1, r);
	bdmf_session_print(session, "\treserved0                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 0, 7, r);
	bdmf_session_print(session, "\thn_size1                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 7, 1, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 0, 7, r);
	bdmf_session_print(session, "\thn_size0                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 4, 3, 5, r);
	bdmf_session_print(session, "\treserved2                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 4, 0, 11, r);
	bdmf_session_print(session, "\tddr_sop_offset0          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 6, 3, 5, r);
	bdmf_session_print(session, "\treserved3                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 6, 0, 11, r);
	bdmf_session_print(session, "\tddr_sop_offset1          = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_TC_TO_QUEUE_8(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register TC_TO_QUEUE_8\n");

	bdmf_session_print(session, "\tqueue_offset             =\n\t");
	for (i=0,j=0; i<8; i++)
	{
		MREAD_I_8((uint8_t *)p, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_POLICER_PARAMS_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register POLICER_PARAMS_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 5, 3, r);
	bdmf_session_print(session, "\tfactor_bytes             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 4, 1, r);
	bdmf_session_print(session, "\tdei_mode                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 4, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_INGRESS_FILTER_CTRL(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register INGRESS_FILTER_CTRL\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p, 0, 31, r);
	bdmf_session_print(session, "\tenable_vector            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 4, 7, 1, r);
	bdmf_session_print(session, "\tcpu_bypass               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_32((uint8_t *)p + 4, 0, 31, r);
	bdmf_session_print(session, "\taction_vector            = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_DUAL_STACK_LITE_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register DUAL_STACK_LITE_ENTRY\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tvesion                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tpayload_length_next_header_hop_limit= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	bdmf_session_print(session, "\tsrc_ip                   =\n\t");
	for (i=0,j=0; i<16; i++)
	{
		MREAD_I_8((uint8_t *)p + 8, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tdst_ip                   =\n\t");
	for (i=0,j=0; i<16; i++)
	{
		MREAD_I_8((uint8_t *)p + 24, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\treserved                 =\n\t");
	for (i=0,j=0; i<24; i++)
	{
		MREAD_I_8((uint8_t *)p + 40, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_PBIT_TO_QUEUE_8(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register PBIT_TO_QUEUE_8\n");

	bdmf_session_print(session, "\tqueue_offset             =\n\t");
	for (i=0,j=0; i<8; i++)
	{
		MREAD_I_8((uint8_t *)p, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
}

void dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register DEF_FLOW_CONTEXT_DDR_ADDR\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tlow                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\thigh                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_TCAM_IC_CFG(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register TCAM_IC_CFG\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\tres_offset               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tcontext_offset           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tcmd_table                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 6, r);
	bdmf_session_print(session, "\tscratch_offset           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tkey_size                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 10, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\tclassification_result    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_SYSTEM_CONFIGURATION_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register SYSTEM_CONFIGURATION_ENTRY\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\tds_drop_precedence_eligibility_vector= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tus_drop_precedence_eligibility_vector= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 4, 7, 1, r);
	bdmf_session_print(session, "\tfull_flow_cache_mode     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 4, 0, 7, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_VPORT_CFG_EX_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register VPORT_CFG_EX_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 6, 2, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 6, r);
	bdmf_session_print(session, "\tingress_filter_profile   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_TUNNELS_PARSING_CFG(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register TUNNELS_PARSING_CFG\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\treserved0                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 1, r);
	bdmf_session_print(session, "\ttunneling_enable         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tres_offset               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_IPTV_CFG(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register IPTV_CFG\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\tres_offset               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tcontext_offset           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tkey_offset               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 6, r);
	bdmf_session_print(session, "\thash_tbl_idx             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_8((uint8_t *)p + 7, r);
	bdmf_session_print(session, "\tl2_mcast                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register IPTV_DDR_CTX_TABLE_ADDRESS\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tlow                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\thigh                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_NAT_CACHE_CFG(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register NAT_CACHE_CFG\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\tres_offset               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tcontext_offset           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tkey_offset               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 6, 3, 5, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 6, 2, 1, r);
	bdmf_session_print(session, "\tthree_tuple_enable       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 6, 1, 1, r);
	bdmf_session_print(session, "\tesp_filter_action        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 6, 0, 1, r);
	bdmf_session_print(session, "\tesp_filter_enable        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_INGRESS_FILTER_CFG(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register INGRESS_FILTER_CFG\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\tres_offset               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_CPU_REDIRECT_MODE_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register CPU_REDIRECT_MODE_ENTRY\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\tmode                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register IPTV_CLASSIFICATION_CFG_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\tda_prefix_mode_mac       = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 6, 1, r);
	bdmf_session_print(session, "\tda_prefix_mode_ip        = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 5, 1, r);
	bdmf_session_print(session, "\tiptv_en                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 5, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_TCAM_IC_CMD(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register TCAM_IC_CMD\n");

	MREAD_8((uint8_t *)p, r);
	bdmf_session_print(session, "\tsrc_offset               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 4, 4, r);
	bdmf_session_print(session, "\tsize                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 0, 4, r);
	bdmf_session_print(session, "\tcmdid                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 7, 1, r);
	bdmf_session_print(session, "\tzero                     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 0, 7, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 7, 1, r);
	bdmf_session_print(session, "\tterminator               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 6, 1, r);
	bdmf_session_print(session, "\treserved2                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 0, 6, r);
	bdmf_session_print(session, "\tdst_offset               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_LAYER2_GRE_TUNNEL_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	int i,j;

	bdmf_session_print(session, "  Register LAYER2_GRE_TUNNEL_ENTRY\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tsrc_ip                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tdst_ip                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\treserved0                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 10, r);
	bdmf_session_print(session, "\tidentification           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	bdmf_session_print(session, "\tdst_mac                  =\n\t");
	for (i=0,j=0; i<6; i++)
	{
		MREAD_I_8((uint8_t *)p + 16, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	bdmf_session_print(session, "\tsrc_mac                  =\n\t");
	for (i=0,j=0; i<6; i++)
	{
		MREAD_I_8((uint8_t *)p + 22, i, r);
		bdmf_session_print(session, "0x%08x ", (unsigned int)r);
		j++;
		if (j >= 8)
		{
			j = 0;
			bdmf_session_print(session, "\n\t");
		}
	}
	bdmf_session_print(session, "\n");
	MREAD_32((uint8_t *)p + 32, r);
	bdmf_session_print(session, "\treserved2                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_TCAM_IC_HANDLER(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register TCAM_IC_HANDLER\n");

	MREAD_16((uint8_t *)p, r);
	bdmf_session_print(session, "\tptr                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_SPDSVC_ANALYZER_PARAMS(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register SPDSVC_ANALYZER_PARAMS\n");

	MREAD_32((uint8_t *)p, r);
	bdmf_session_print(session, "\tstream                   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tstream1                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 8, r);
	bdmf_session_print(session, "\tstream2                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 12, r);
	bdmf_session_print(session, "\tstream3                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 16, r);
	bdmf_session_print(session, "\tts_first                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 20, r);
	bdmf_session_print(session, "\tts_last                  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 24, r);
	bdmf_session_print(session, "\tis_on                    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_BRIDGE_CFG(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register BRIDGE_CFG\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\thit                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 6, 1, r);
	bdmf_session_print(session, "\tbridge_fw_failed         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 5, 1, r);
	bdmf_session_print(session, "\tvlan_aggregation         = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 5, r);
	bdmf_session_print(session, "\treserved1                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 1, 7, r);
	bdmf_session_print(session, "\treserved2                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 0, 1, r);
	bdmf_session_print(session, "\tbridge_results_available = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 2, r);
	bdmf_session_print(session, "\tres_offset               = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tcontext_offset           = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_16((uint8_t *)p + 6, r);
	bdmf_session_print(session, "\treserved                 = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

void dump_RDD_VPORT_CFG_ENTRY(bdmf_session_handle session, unsigned char *p)
{
	unsigned int r;
	bdmf_session_print(session, "  Register VPORT_CFG_ENTRY\n");

	FIELD_MREAD_8((uint8_t *)p, 7, 1, r);
	bdmf_session_print(session, "\tloopback_en              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 6, 1, r);
	bdmf_session_print(session, "\tmirroring_en             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 5, 1, r) /*defined by rdd_bridge_and_vlan_ctx_lookup_method enumeration*/;
	bdmf_session_print(session, "\tbridge_and_vlan_lookup_method= 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 4, 1, r);
	bdmf_session_print(session, "\tdiscard_prty             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 3, 1, r);
	bdmf_session_print(session, "\tegress_isolation_en      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 2, 1, r);
	bdmf_session_print(session, "\tingress_isolation_en     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 1, 1, r);
	bdmf_session_print(session, "\tsa_lookup_en             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p, 0, 1, r);
	bdmf_session_print(session, "\tda_lookup_en             = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 6, 2, r) /*defined by rdd_action enumeration*/;
	bdmf_session_print(session, "\tsa_lookup_miss_action    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 4, 2, r) /*defined by rdd_action enumeration*/;
	bdmf_session_print(session, "\tda_lookup_miss_action    = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 1, 0, 4, r);
	bdmf_session_print(session, "\tprotocol_filters_dis     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 7, 1, r);
	bdmf_session_print(session, "\tflow_control_all_traffic = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 6, 1, r);
	bdmf_session_print(session, "\tflow_control_broadcast   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 5, 1, r);
	bdmf_session_print(session, "\tflow_control_multicast   = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 2, 4, 1, r);
	bdmf_session_print(session, "\tflow_control_unknown_da  = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_16((uint8_t *)p + 2, 5, 7, r);
	bdmf_session_print(session, "\tpolicer_idx              = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 4, 1, r);
	bdmf_session_print(session, "\treserved2                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 1, 3, r);
	bdmf_session_print(session, "\tviq                      = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	FIELD_MREAD_8((uint8_t *)p + 3, 0, 1, r);
	bdmf_session_print(session, "\tls_fc_cfg                = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

	MREAD_32((uint8_t *)p + 4, r);
	bdmf_session_print(session, "\tegress_isolation_map     = 0x%08x", (unsigned int)r);
	bdmf_session_print(session, "\n");

}

