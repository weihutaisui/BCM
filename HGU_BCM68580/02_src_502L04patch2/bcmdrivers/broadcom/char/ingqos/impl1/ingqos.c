
/*
<:copyright-BRCM:2009:proprietary:standard

   Copyright (c) 2009 Broadcom 
   All Rights Reserved

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
*/


/*
 *******************************************************************************
 * File Name  : ingqos.c
 *
 * Description: This file implements the Ingress QoS.
 *
 *******************************************************************************
 */

/* -----------------------------------------------------------------------------
 *                      Ingress QoS (IQ)
 * -----------------------------------------------------------------------------
 * Ingress QoS feature defines a flow has low or high ingress priority at an 
 * ingress interface based on a pre-defined criterion or the layer4 (TCP, UDP)
 * destination port. Under normal load conditions all received packets are
 * accepted and forwarded. But under CPU congestion high ingress priority are
 * accepted whereas low ingress priority packets are dropped.
 *
 * CPU congestion detection:
 * -------------------------
 * Ingress QoS constantly monitors the queue depth for an interface to detect
 * CPU congestion. If the queue depth is greater than the IQ high threshold,
 * CPU congestion has set in. When the queue depth is less than IQ low
 * threshold, then CPU congestion has abated.  
 *
 * CPU Congestion Set:
 * -------------------
 * When the CPU is congested only high ingress priority are accepted 
 * whereas low ingress priority packets are dropped.
 *
 * These are some of the pre-defined criterion for a flow to be 
 * high ingress priority:
 * a) High Priority Interface
 *    - Any packet received from XTM 
 *    - Any packet received from or sent to WLAN 
 * b) Multicast
 * c) Flows configured by default through the following ALGs
 *    - SIP (default SIP UDP port = 5060), RTSP ports for data
 *    - RTSP (default UDP port = 554), RTSP ports for data
 *    - H.323 (default UDP port = 1719, 1720)
 *    - MGCP (default UDP ports 2427 and 2727)
 *    Note:- The above ALGs are not invoked for bridging mode. Therefore 
 *           in bridging mode, user code needs to call APIs such as
 *           iqos_add_L4port() to classify a L4 port as high.
 *      
 * d) Other flows configured through Ingress QoS APIs:
 *    - DNS  UDP port = 53  
 *    - DHCP UDP port = 67  
 *    - DHCP UDP port = 68  
 *    - HTTP TCP port = 80, and 8080  
 *    Note:- If required user code can add more flows in the iq_init() or
 *           use the iqos_add_L4port().
 *
 * Note: API prototypes are given in iqos.h file.
 */


#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/ip.h>
#include "pktHdr.h"
#include <linux/bcm_log_mod.h>
#include <linux/bcm_log.h>
#include <linux/iqos.h>
#include "iqctl_common.h"
#include "ingqos.h"
#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
#include "rdpa_types.h"
#include "bdmf_system_common.h"
#include "bdmf_interface.h"
#include "rdpa_cpu.h"
#include "rdpa_api.h"
#endif

/*----- Defines -----*/
#define IQ_L4PROTO_IX(ipproto)                                     \
    ((ipproto == IQOS_IPPROTO_UDP) ? IQCTL_PROTO_UDP : IQCTL_PROTO_TCP)

#define IQ_IPPROTO(protoix)                                     \
    ((protoix == IQCTL_PROTO_UDP) ? IQOS_IPPROTO_UDP : IQOS_IPPROTO_TCP)

#define IQ_ISSTAT(ent)      \
    ((ent == IQOS_ENT_STAT) ? IQOS_ENT_STAT : IQOS_ENT_DYN)


/*----- Globals -----*/
#undef IQ_DECL
#define IQ_DECL(x) #x,

const char *iqctl_ioctl_name[] =
{
    IQ_DECL(IQCTL_IOCTL_SYS)
    IQ_DECL(IQCTL_IOCTL_MAX)
};

const char *iqctl_subsys_name[] =
{
    IQ_DECL(IQCTL_SUBSYS_STATUS)
    IQ_DECL(IQCTL_SUBSYS_DPORT)
    IQ_DECL(IQCTL_SUBSYS_DPORTTBL)
    IQ_DECL(IQCTL_SUBSYS_QUE)
    IQ_DECL(IQCTL_SUBSYS_PROTO)
    IQ_DECL(IQCTL_SUBSYS_MAX)
};

const char *iqctl_op_name[] =
{   
    IQ_DECL(IQCTL_OP_GET)
    IQ_DECL(IQCTL_OP_SET)
    IQ_DECL(IQCTL_OP_ADD)
    IQ_DECL(IQCTL_OP_REM)
    IQ_DECL(IQCTL_OP_DUMP)
    IQ_DECL(IQCTL_OP_FLUSH)
    IQ_DECL(IQCTL_OP_MAX)
};

extern uint32_t iqos_enable_g;
extern uint32_t iqos_debug_g;
extern uint32_t iqos_cpu_cong_g;
extern spinlock_t iqos_lock_g;

/* Hooks for getting/dumping the Ingress QoS status */
extern iqos_status_hook_t iqos_enet_status_hook_g;

#if (defined(CONFIG_BCM_FAP) || defined(CONFIG_BCM_FAP_MODULE))
extern iqos_status_hook_t iqos_fap_status_hook_g;
#endif

#if defined(CONFIG_BCM_XTMCFG) || defined(CONFIG_BCM_XTMCFG_MODULE)
extern iqos_status_hook_t iqos_xtm_status_hook_g;
#endif


#if (defined(CONFIG_BCM_FAP) || defined(CONFIG_BCM_FAP_MODULE))
/* Hooks for getting the current RX DQM queue depth */
extern iqos_fap_ethRxDqmQueue_hook_t iqos_fap_ethRxDqmQueue_hook_g;
#if defined(CONFIG_BCM_XTMCFG) || defined(CONFIG_BCM_XTMCFG_MODULE)
extern iqos_fap_xtmRxDqmQueue_hook_t iqos_fap_xtmRxDqmQueue_hook_g;
#endif

extern iqos_fap_set_status_hook_t     iqos_fap_set_status_hook_g;
extern iqos_fap_add_L4port_hook_t     iqos_fap_add_L4port_hook_g;
extern iqos_fap_rem_L4port_hook_t     iqos_fap_rem_L4port_hook_g;
extern iqos_fap_dump_porttbl_hook_t   iqos_fap_dump_porttbl_hook_g;
extern iqos_fap_set_proto_prio_hook_t iqos_fap_set_proto_prio_hook_g;
extern iqos_fap_rem_proto_prio_hook_t iqos_fap_rem_proto_prio_hook_g;
#endif

#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
extern iqos_runner_get_hook_t iqos_runner_stat_hook_g;
extern iqos_runner_get_hook_t iqos_runner_get_L4port_hook_g;
extern iqos_runner_L4port_hook_t iqos_runner_add_L4port_hook_g;
extern iqos_runner_rem_L4port_hook_t iqos_runner_rem_L4port_hook_g;
extern iqos_runner_L4port_hook_t iqos_runner_find_L4port_hook_g;

bdmf_object_handle rdpa_iq_cpu_obj=NULL;
bdmf_object_handle rdpa_iq_wlan0_obj=NULL;

#define INGQOS_RUNNER_MAX_QOS_RULES  255
#endif

typedef struct {
    uint16_t port;          /* dest port */
    uint8_t  ent    :1;     /* static entry */
    uint8_t  unused :4;     /* unused */
    uint8_t  prio   :3;     /* prio */
    uint8_t  nextIx;        /* overflow bucket index */
    uint16_t unused2;       /* unused */
    uint16_t refcnt ;       /* refcnt */
} qos_hent_t;

/* Main Hash Table(s): for UDP and TCP */
qos_hent_t  qos_htbl[IQCTL_PROTO_MAX][IQ_HASHTBL_SIZE];

/* Overflow Table(s): for UDP and TCP */
qos_hent_t  ovfl_tbl[IQCTL_PROTO_MAX][IQ_OVFLTBL_SIZE];

#define IQ_MAX_PROTO_ENTRIES 4

typedef struct {
    uint8_t protoval;
    uint8_t unused : 5;
    uint8_t prio   : 3;
} qos_protoprio_t;

typedef struct {
    uint8_t         entryCount[IQCTL_PROTOTYPE_MAX];
    qos_protoprio_t entry[IQCTL_PROTOTYPE_MAX][IQ_MAX_PROTO_ENTRIES];
} qos_protoprio_tbl_t;

/* This table is used to set the default priority of protocol */
qos_protoprio_tbl_t proto_prio_tbl;

typedef struct {
    uint8_t count;
    uint8_t nextIx;
} free_ovfl_ent_t;

free_ovfl_ent_t free_ovfl_list[IQCTL_PROTO_MAX];

static uint8_t iq_rem_dyn_L4port( iqos_ipproto_t ipProto, uint16_t destPort );


static uint8_t iq_hash( uint32_t port )
{
    uint8_t hashIx = (uint8_t) _hash(port) % IQ_HASHTBL_SIZE;

    /* if hash happens to be 0, make it 1 */
    if (hashIx == IQOS_INVALID_NEXT_IX) 
        hashIx = 1;

    return hashIx;
}

static int iq_get_status( void )
{
    BCM_LOG_NOTICE( BCM_LOG_ID_IQ, "Ingress QoS status : %s \n", 
                            (iqos_enable_g == 1) ? "enabled": "disabled" );

    if ( likely(iqos_enet_status_hook_g != (iqos_status_hook_t)NULL)
#if defined(CONFIG_BCM_XTMCFG) || defined(CONFIG_BCM_XTMCFG_MODULE)
         || (likely(iqos_xtm_status_hook_g != (iqos_status_hook_t)NULL))
#endif
#if (defined(CONFIG_BCM_FAP) || defined(CONFIG_BCM_FAP_MODULE))
         || (likely(iqos_fap_status_hook_g != (iqos_status_hook_t)NULL))
#endif
       )
    {
        printk(  "\n------------------------IQ Status---------------------\n");
        printk(  "        dev chnl loThr hiThr  used    dropped     cong\n");
        printk(  "------ ---- ---- ----- ----- ----- ---------- --------\n");
    }

    /* Dump the Ingress QoS status for various interfaces */
    if ( likely(iqos_enet_status_hook_g != (iqos_status_hook_t)NULL) )
    {
        iqos_enet_status_hook_g(); 
    }

#if defined(CONFIG_BCM_XTMCFG) || defined(CONFIG_BCM_XTMCFG_MODULE)
    if ( likely(iqos_xtm_status_hook_g != (iqos_status_hook_t)NULL) )
    {
        iqos_xtm_status_hook_g(); 
    }
#endif

#if (defined(CONFIG_BCM_FAP) || defined(CONFIG_BCM_FAP_MODULE))
    if ( likely(iqos_fap_status_hook_g != (iqos_status_hook_t)NULL) )
        iqos_fap_status_hook_g(); 
#endif

#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
    if ( likely(iqos_runner_stat_hook_g != (iqos_runner_get_hook_t)NULL))
    {
        rdpa_cpu_rx_stat_t rxstat;
        bdmf_index idx;

        printk(  "\n----------------IQ Status------------------\n");
        printk(  " qidx\trxpkts\tqueued\tdropped\tinterrupts\n");
        printk(  "-------------------------------------------\n");
        for (idx = 0; idx < RDPA_CPU_MAX_QUEUES; idx++) 
        {
            if (iqos_runner_stat_hook_g(rdpa_iq_cpu_obj, idx, &rxstat) == 0) 
            {
                printk(" %d\t%u\t%u\t%u\t%u\n", 
                       (int)idx, rxstat.received, rxstat.queued, rxstat.dropped, rxstat.interrupts);
            }
        }
        for (idx = RDPA_CPU_MAX_QUEUES; idx < (RDPA_CPU_MAX_QUEUES + RDPA_WLAN_MAX_QUEUES); idx++) 
        {
            if (iqos_runner_stat_hook_g(rdpa_iq_wlan0_obj, idx, &rxstat) == 0) 
            {
                printk(" %d\t%u\t%u\t%u\t%u\n", 
                       (int)idx, rxstat.received, rxstat.queued, rxstat.dropped, rxstat.interrupts);
            }
        }
    }
#endif
    return iqos_enable_g;
}

#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
static int iq_add_ports_to_runner_from_hosttbl(int maxentries, qos_hent_t pTable[][maxentries])
{
    rdpa_l4_dst_port_to_reason_cfg_t entry;
    bdmf_index idx;
    iqctl_proto_t proto;
    int rc, ix;

    for (ix = 0; ix < maxentries; ix++)
    {
        for (proto = 0; proto < IQCTL_PROTO_MAX; proto++) 
        {
            if (pTable[proto][ix].port != IQOS_INVALID_PORT)
            {
                entry.is_static = pTable[proto][ix].ent;
                entry.is_tcp = (proto == IQCTL_PROTO_TCP);
                entry.l4_dst_port = pTable[proto][ix].port;
                if (pTable[proto][ix].prio == IQOS_PRIO_HIGH) 
                {
                    entry.reason = rdpa_cpu_rx_reason_l4_udef_0;

                    // IPv4 DHCP (port 67, 68), IPv6 DHCP (port 546, 547)
                    if ((proto == IQCTL_PROTO_UDP) && 
                        (entry.l4_dst_port == 67 || entry.l4_dst_port == 68 || 
                         entry.l4_dst_port == 546 || entry.l4_dst_port == 547))
                    {
                        entry.reason = rdpa_cpu_rx_reason_dhcp;
                    }
                }
                rc = iqos_runner_add_L4port_hook_g(rdpa_iq_cpu_obj, &idx, &entry); 
            }
        }
    }
    return 0;
}
#endif

static int iq_set_status( uint32_t status )
{
#if (defined(CONFIG_BCM_FAP) || defined(CONFIG_BCM_FAP_MODULE))
    if (likely(iqos_fap_set_status_hook_g != (iqos_fap_set_status_hook_t)NULL))
        iqos_fap_set_status_hook_g(status);
#endif

#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
    if (status != iqos_enable_g)
    {
        if (status) 
        {
            if (likely(iqos_runner_add_L4port_hook_g != (iqos_runner_L4port_hook_t)NULL))
            {
                iq_add_ports_to_runner_from_hosttbl(IQ_HASHTBL_SIZE, qos_htbl);
                iq_add_ports_to_runner_from_hosttbl(IQ_OVFLTBL_SIZE, ovfl_tbl);
            }
        }
        else if (likely(iqos_runner_rem_L4port_hook_g != (iqos_runner_rem_L4port_hook_t)NULL) )
        {
             bdmf_index idx;
             for (idx = 0; idx < INGQOS_RUNNER_MAX_QOS_RULES; idx++) 
             {
                 iqos_runner_rem_L4port_hook_g(rdpa_iq_cpu_obj, idx); 
             }
        }
    }
#endif

    iqos_enable_g = status;
    iqos_cpu_cong_g = 0;        /* clear the CPU cong status */

    return status;
}


static inline int iq_alloc_ovfl_ent( uint8_t protoIx )
{
    uint8_t allocIx;

    if ( free_ovfl_list[protoIx].nextIx == IQOS_INVALID_NEXT_IX )
        return IQOS_INVALID_NEXT_IX;
    else
    {
        allocIx = free_ovfl_list[protoIx].nextIx;
        free_ovfl_list[protoIx].nextIx = ovfl_tbl[protoIx][allocIx].nextIx;
        free_ovfl_list[protoIx].count--;

        ovfl_tbl[protoIx][allocIx].port = IQOS_INVALID_PORT;
        ovfl_tbl[protoIx][allocIx].ent = IQOS_ENT_DYN;
        ovfl_tbl[protoIx][allocIx].prio = IQOS_PRIO_LOW;
        ovfl_tbl[protoIx][allocIx].nextIx = IQOS_INVALID_NEXT_IX;
        ovfl_tbl[protoIx][allocIx].refcnt = 0;
    }
    return allocIx;
}

static inline int iq_free_ovfl_ent( uint8_t protoIx, uint8_t freeIx )
{
    ovfl_tbl[protoIx][freeIx].port = IQOS_INVALID_PORT;
    ovfl_tbl[protoIx][freeIx].ent = IQOS_ENT_DYN;
    ovfl_tbl[protoIx][freeIx].prio = IQOS_PRIO_LOW;
    ovfl_tbl[protoIx][freeIx].nextIx = free_ovfl_list[protoIx].nextIx;
    ovfl_tbl[protoIx][freeIx].refcnt = 0;
    free_ovfl_list[protoIx].nextIx = freeIx;
    free_ovfl_list[protoIx].count++;
    return freeIx;
}

static qos_hent_t  *iq_find_port( iqos_ipproto_t ipProto, uint16_t destPort )
{
    uint8_t hashIx;
    uint8_t protoIx;
    uint8_t curIx;

    hashIx = iq_hash(destPort);
    protoIx = IQ_L4PROTO_IX(ipProto);

    if ( qos_htbl[protoIx][hashIx].port != IQOS_INVALID_PORT )
    {
        if ( qos_htbl[protoIx][hashIx].port == destPort )
            return &qos_htbl[protoIx][hashIx];
        else
        {
            curIx = qos_htbl[protoIx][hashIx].nextIx;

            while (curIx != IQOS_INVALID_NEXT_IX)
            {
                if ( ovfl_tbl[protoIx][curIx].port == destPort )
                    return &ovfl_tbl[protoIx][curIx];
                else
                    curIx = ovfl_tbl[protoIx][curIx].nextIx;
            }
        }
    }

    return NULL;
}


static int iq_get_L4port( iqos_ipproto_t ipProto, uint16_t destPort,
        iqos_ent_t *ent_p, iqos_prio_t *prio_p, uint8_t *refcnt_p )
{
    qos_hent_t *qos_ent_p;

    printk( "GetPort ipProto<%d> dport<%d>\n", ipProto, destPort );  

    if ((ipProto != IQOS_IPPROTO_UDP) && (ipProto != IQOS_IPPROTO_TCP))
    {
        qos_ent_p = NULL;
    } 
    else 
        qos_ent_p = iq_find_port( ipProto, destPort );

    if ( qos_ent_p != NULL )
    {
        *ent_p = qos_ent_p->ent;
        *prio_p = qos_ent_p->prio;
        *refcnt_p = qos_ent_p->refcnt;
    }
    else
    {
        *ent_p = IQOS_ENT_DYN;
        *prio_p = IQOS_PRIO_LOW;
        *refcnt_p = 0;
    }
    printk( "ent<%d> prio<%d> refcnt<%d>\n", *ent_p, *prio_p, *refcnt_p );  

    return IQOS_SUCCESS;
}

int iq_host_rem_proto_prio( iqctl_prototype_t protoType, uint8_t protoval )
{
    uint8_t ptIdx = 0;
    int ptValIdx = 0;

#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
    {
        /* Not supported in Runner platforms */
        return IQOS_ERROR;
    }
#endif

    if (protoType >= IQCTL_PROTOTYPE_MAX) 
    {
        return IQOS_ERROR;
    }

    if (proto_prio_tbl.entryCount[protoType] == 0) 
    {
        return IQOS_ERROR;
    }

    for (ptIdx = 0; ptIdx < IQCTL_PROTOTYPE_MAX; ptIdx++) 
    {
        for (ptValIdx = 0; ptValIdx < proto_prio_tbl.entryCount[ptIdx]; ptValIdx++) 
        {
            if (proto_prio_tbl.entry[ptIdx][ptValIdx].protoval == protoval) 
            {
                int shiftIdx;
                for (shiftIdx = ptValIdx; shiftIdx < proto_prio_tbl.entryCount[protoType]; shiftIdx++) 
                {
                    proto_prio_tbl.entry[ptIdx][shiftIdx].protoval = 
                        proto_prio_tbl.entry[ptIdx][shiftIdx + 1].protoval; 
                    proto_prio_tbl.entry[ptIdx][shiftIdx].prio = 
                        proto_prio_tbl.entry[ptIdx][shiftIdx + 1].prio; 
                }
                proto_prio_tbl.entryCount[ptIdx]--;
                break;
            }
        }
    }

    return IQOS_SUCCESS;
}

int iq_host_get_proto_prio( iqctl_prototype_t protoType, 
    uint8_t protoval, iqctl_prio_t *prio, uint8_t *entryIdx )
{
    uint8_t ptIdx = 0;
    uint8_t ptValIdx = 0;

#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
    {
        /* Not supported in Runner platforms */
        return IQOS_ERROR;
    }
#endif

    if (protoType >= IQCTL_PROTOTYPE_MAX) 
    {
        return IQOS_ERROR;
    }

    for (ptIdx = 0; ptIdx < IQCTL_PROTOTYPE_MAX; ptIdx++) 
    {
        for (ptValIdx = 0; ptValIdx < proto_prio_tbl.entryCount[ptIdx]; ptValIdx++) 
        {
            if (proto_prio_tbl.entry[ptIdx][ptValIdx].protoval == protoval) 
            {
                if (prio) 
                {
                    *prio = proto_prio_tbl.entry[ptIdx][ptValIdx].prio;
                }
                if (entryIdx) 
                {
                    *entryIdx = ptValIdx;
                }
                return IQOS_SUCCESS;
            }
        }
    }
    return IQOS_ERROR;
}

static uint8_t iq_host_set_proto_prio( iqctl_prototype_t protoType, 
    uint8_t protoVal, iqctl_prio_t prio )
{
    uint8_t ptValIdx = 0;
    iqctl_prio_t oldPrio = IQOS_PRIO_MAX;

#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
    {
        /* Not supported in Runner platforms */
        return IQOS_ERROR;
    }
#endif

    if (protoType >= IQCTL_PROTOTYPE_MAX) 
    {
        return IQOS_ERROR;
    }

    if (prio >= IQCTL_PRIO_MAX) 
    {
        return IQOS_ERROR;
    }

    /* Check if the protocol already exists */
    if (iq_host_get_proto_prio(protoType, protoVal, &oldPrio, &ptValIdx) == IQOS_SUCCESS)
    {
        /* Check if prio has changed. If not nothing to do */
        if (prio != oldPrio) 
        {
            proto_prio_tbl.entry[protoType][ptValIdx].prio = prio;
        }
        return IQOS_SUCCESS;
    }

    /* Check if there is room for more entries */
    if (proto_prio_tbl.entryCount[protoType] >= IQ_MAX_PROTO_ENTRIES) 
    {
        return IQOS_ERROR;
    }

    /* protocol does not exist. Add entry */
    proto_prio_tbl.entry[protoType][proto_prio_tbl.entryCount[protoType]].protoval = protoVal;
    proto_prio_tbl.entry[protoType][proto_prio_tbl.entryCount[protoType]].prio = prio;
    proto_prio_tbl.entryCount[protoType]++;

    return IQOS_SUCCESS;
}

static int iq_prio_L4port( iqos_ipproto_t ipProto, uint16_t destPort )
{
    qos_hent_t *qos_ent_p;
    iqos_prio_t retPrio = IQOS_PRIO_HIGH;

    if ((ipProto == IQOS_IPPROTO_UDP) || (ipProto == IQOS_IPPROTO_TCP))
    {
        qos_ent_p = iq_find_port( ipProto, destPort );
        if ( qos_ent_p != NULL )
            return qos_ent_p->prio;
        retPrio = IQOS_PRIO_LOW;
    }

    /* check the defaultprio table. If there is an entry retPrio will be
       populated with appropriate priority. If not, retPrio will be untouched
       and keep the default value (FAP_IQ_PRIO_HIGH) */
    iq_host_get_proto_prio(IQCTL_PROTOTYPE_IP, (uint8_t)ipProto, (iqctl_prio_t *)(&retPrio), NULL);

    return retPrio;
}

static uint8_t iq_host_add_L4port( iqos_ipproto_t ipProto, uint16_t destPort, 
        iqos_ent_t ent, iqos_prio_t prio )
{
    uint8_t hashIx;
    uint8_t protoIx;
    uint8_t nextIx = IQOS_INVALID_NEXT_IX;
    uint8_t addIx = IQOS_INVALID_NEXT_IX;

    BCM_LOG_DEBUG( BCM_LOG_ID_IQ, "AddPort ent<%d> ipProto<%d> dport<%d> prio<%d> ", 
            ent, ipProto, destPort, prio);  

    if ((ipProto != IQOS_IPPROTO_UDP) && (ipProto != IQOS_IPPROTO_TCP))
    {
        addIx = IQOS_INVALID_NEXT_IX;
        goto iq_host_add_L4port_exit;
    }

    hashIx = iq_hash(destPort);
    protoIx = IQ_L4PROTO_IX(ipProto);

    if ( qos_htbl[protoIx][hashIx].port == destPort )
    {
        if ( ( qos_htbl[protoIx][hashIx].ent == IQOS_ENT_STAT ) ||
             ( ent == IQOS_ENT_STAT ) )
        {
            qos_htbl[protoIx][hashIx].ent = IQOS_ENT_STAT;
            qos_htbl[protoIx][hashIx].refcnt = 1;
        }
        else
        {
            qos_htbl[protoIx][hashIx].ent = ent;
                qos_htbl[protoIx][hashIx].refcnt++;
        }

        addIx = hashIx;
        goto iq_host_add_L4port_exit;
    }
    else if ( qos_htbl[protoIx][hashIx].port == IQOS_INVALID_PORT )
    {
        /* Hash bucket is unused */
        qos_htbl[protoIx][hashIx].nextIx = IQOS_INVALID_NEXT_IX;
        qos_htbl[protoIx][hashIx].prio = prio;
        qos_htbl[protoIx][hashIx].port = destPort;
        qos_htbl[protoIx][hashIx].ent = ent;
        qos_htbl[protoIx][hashIx].refcnt = 1;
        addIx = hashIx;
        goto iq_host_add_L4port_exit;
    }
    else
    {  /* Hash bucket is already used (i.e. there is a collision) */
        uint8_t ovflIx;

        nextIx = qos_htbl[protoIx][hashIx].nextIx;

        /* search for matching ovfl entry for the dest port */
        while (nextIx != IQOS_INVALID_NEXT_IX)
        {
            addIx = nextIx;
            nextIx = ovfl_tbl[protoIx][addIx].nextIx;
            if ( ( ovfl_tbl[protoIx][addIx].port == destPort ) &&
                   ovfl_tbl[protoIx][addIx].refcnt )
            {   

                if ( ( ovfl_tbl[protoIx][addIx].ent == IQOS_ENT_STAT ) ||
                     ( ent == IQOS_ENT_STAT ) )
                {
                    ovfl_tbl[protoIx][addIx].ent = IQOS_ENT_STAT;
                    ovfl_tbl[protoIx][addIx].refcnt = 1;
                }
                else
                {
                    ovfl_tbl[protoIx][addIx].ent = ent;
                    ovfl_tbl[protoIx][addIx].refcnt++;
                }

                goto iq_host_add_L4port_exit;
            }
        }

        /* allocate an overflow/collision entry */
        if ( (ovflIx = iq_alloc_ovfl_ent(protoIx) ) == IQOS_INVALID_NEXT_IX )
        {
            addIx = IQOS_INVALID_NEXT_IX;
            goto iq_host_add_L4port_exit;
        }

        ovfl_tbl[protoIx][ovflIx].nextIx = IQOS_INVALID_NEXT_IX;
        ovfl_tbl[protoIx][ovflIx].prio = prio;
        ovfl_tbl[protoIx][ovflIx].port = destPort;
        ovfl_tbl[protoIx][ovflIx].ent = ent;
        ovfl_tbl[protoIx][ovflIx].refcnt = 1;

        nextIx = qos_htbl[protoIx][hashIx].nextIx;

        if ( nextIx == IQOS_INVALID_NEXT_IX )
        {
            /* First overflow entry */
            qos_htbl[protoIx][hashIx].nextIx = ovflIx;
            addIx = ovflIx;
            goto iq_host_add_L4port_exit;
        }
        else 
        {   /* not First overflow entry */
            uint8_t curIx;

            curIx = nextIx;

            /* Find the last overflow entry for the current hash bucket */
            while (nextIx != IQOS_INVALID_NEXT_IX)
            {
                curIx = nextIx;
                nextIx = ovfl_tbl[protoIx][curIx].nextIx;
            }

            ovfl_tbl[protoIx][curIx].nextIx = ovflIx;
            addIx = ovflIx;
            goto iq_host_add_L4port_exit;
        }
    }

iq_host_add_L4port_exit:
    BCM_LOG_DEBUG( BCM_LOG_ID_IQ, "addIx<%d>", addIx);  
    return addIx;
}


static uint8_t iq_host_rem_L4port( iqos_ipproto_t ipProto, uint16_t destPort, 
        iqos_ent_t ent )
{
    uint8_t hashIx; 
    uint8_t nextIx;
    uint8_t protoIx;
    uint8_t remIx;

    BCM_LOG_DEBUG( BCM_LOG_ID_IQ, "RemPort ent<%d> ipProto<%d> dport<%d> ", 
                ent, ipProto, destPort);  

    if ((ipProto != IQOS_IPPROTO_UDP) && (ipProto != IQOS_IPPROTO_TCP))
    {
        remIx = IQOS_INVALID_NEXT_IX;
        goto iq_host_rem_L4port_exit;
    }

    hashIx = iq_hash(destPort);
    protoIx = IQ_L4PROTO_IX(ipProto);

    if ( qos_htbl[protoIx][hashIx].port == IQOS_INVALID_PORT )
    {
        /* Hash bucket is unused */
        remIx = IQOS_INVALID_NEXT_IX;
        goto iq_host_rem_L4port_exit;
    }
    else
    {   /* Hash bucket is used */
        nextIx = qos_htbl[protoIx][hashIx].nextIx;

        if ( ( qos_htbl[protoIx][hashIx].port == destPort ) &&
             ( qos_htbl[protoIx][hashIx].ent == ent ) &&
               qos_htbl[protoIx][hashIx].refcnt )
        {   /* Hash bucket matches the desired entry */
            qos_htbl[protoIx][hashIx].refcnt--;

            if ( qos_htbl[protoIx][hashIx].refcnt )
            {
                remIx = IQOS_INVALID_NEXT_IX;
                goto iq_host_rem_L4port_exit;
            }

            if ( nextIx == IQOS_INVALID_NEXT_IX )
            {  /* There are no overflow entries */
                qos_htbl[protoIx][hashIx].port = IQOS_INVALID_PORT;
                qos_htbl[protoIx][hashIx].ent = IQOS_ENT_DYN;
                qos_htbl[protoIx][hashIx].prio = IQOS_PRIO_LOW;
                qos_htbl[protoIx][hashIx].refcnt = 0;
            }
            else
            {  /* There are overflow entries */
                /* copy the next overlow entry to hash bucket */
                qos_htbl[protoIx][hashIx].port = 
                                            ovfl_tbl[protoIx][nextIx].port;
                qos_htbl[protoIx][hashIx].ent = 
                                            ovfl_tbl[protoIx][nextIx].ent;
                qos_htbl[protoIx][hashIx].prio = 
                                            ovfl_tbl[protoIx][nextIx].prio;
                qos_htbl[protoIx][hashIx].nextIx = 
                                           ovfl_tbl[protoIx][nextIx].nextIx;
                qos_htbl[protoIx][hashIx].refcnt = 
                                       ovfl_tbl[protoIx][nextIx].refcnt;
                remIx = nextIx;
                goto iq_free_ovfl_ent_exit;
            }
            remIx = hashIx;
            goto iq_host_rem_L4port_exit;
        }
        else
        {  /* Hash bucket is used (but does not match the desired entry) */

            if ( nextIx == IQOS_INVALID_NEXT_IX )
            {   /* No overflow entry */
                remIx = hashIx;
                goto iq_host_rem_L4port_exit;
            }
            else 
            {   /* First overflow entry */
                remIx = nextIx;
                nextIx = ovfl_tbl[protoIx][remIx].nextIx;

                if ( ( ovfl_tbl[protoIx][remIx].port == destPort ) &&
                     ( ovfl_tbl[protoIx][remIx].ent == ent ) && 
                       ovfl_tbl[protoIx][remIx].refcnt )
                {
                    ovfl_tbl[protoIx][remIx].refcnt--;

                    if ( ovfl_tbl[protoIx][remIx].refcnt )
                    {
                        remIx = IQOS_INVALID_NEXT_IX;
                        goto iq_host_rem_L4port_exit;
                    }

                    /* First overflow entry is a match */
                    qos_htbl[protoIx][hashIx].nextIx = nextIx;
                    goto iq_free_ovfl_ent_exit;
                }
                else
                {
                    uint8_t prevIx;

                    prevIx = remIx;

                    /* Find the matching entry for the dest port */
                    while (nextIx != IQOS_INVALID_NEXT_IX)
                    {
                        prevIx = remIx;
                        remIx = nextIx;
                        nextIx = ovfl_tbl[protoIx][remIx].nextIx;

                        if ( ( ovfl_tbl[protoIx][remIx].port == destPort ) &&
                             ( ovfl_tbl[protoIx][remIx].ent == ent ) && 
                               ovfl_tbl[protoIx][remIx].refcnt )
                        {   
                            ovfl_tbl[protoIx][remIx].refcnt--;

                            if ( ovfl_tbl[protoIx][remIx].refcnt )
                            {
                                remIx = IQOS_INVALID_NEXT_IX;
                                goto iq_host_rem_L4port_exit;
                            }

                            ovfl_tbl[protoIx][prevIx].nextIx = nextIx;
                            goto iq_free_ovfl_ent_exit;
                        }
                    }

                    remIx = IQOS_INVALID_NEXT_IX;
                    goto iq_host_rem_L4port_exit;
                }
            }
        }
    }

iq_free_ovfl_ent_exit:
    /* free the overflow/collision entry */
    iq_free_ovfl_ent( protoIx, remIx );

iq_host_rem_L4port_exit:
    BCM_LOG_DEBUG( BCM_LOG_ID_IQ, "remIx<%d> ", remIx);  
    return remIx;
}

static void iq_host_dump_porttbl( iqos_ipproto_t ipProto )
{
    uint8_t proto;
    int ix;

    printk( "Dump tables ipProto<%d>\n", ipProto );  

    proto = IQ_L4PROTO_IX(ipProto);

    /* dump Hash Table */
    if (proto == IQCTL_PROTO_UDP)
        printk( "\n ----------- UDP Proto ----------- \n" );
    else
        printk( "\n ----------- TCP Proto ----------- \n" );

    printk( " Hash Table \n" );
    printk( " Ix   Port  Ent  Prio  Next  RefCnt\n" );
    for( ix=0; ix<IQ_HASHTBL_SIZE; ix++)
    {
        if ( qos_htbl[proto][ix].port != IQOS_INVALID_PORT )
            printk( "%3d  %5d  %3d  %4d  %4d   %5d\n", ix, 
                    qos_htbl[proto][ix].port, qos_htbl[proto][ix].ent,
                    qos_htbl[proto][ix].prio, qos_htbl[proto][ix].nextIx,
                    qos_htbl[proto][ix].refcnt );
    }

    /* dump Overflow Table */
    printk( "\n Overflow Table \n" );
    printk( " Ix   Port  Ent  Prio  Next  RefCnt\n" );
    for( ix=0; ix<IQ_OVFLTBL_SIZE; ix++)
    {
        if ( ovfl_tbl[proto][ix].port != IQOS_INVALID_PORT )
            printk( "%3d  %5d  %3d  %4d  %4d   %5d\n", ix, 
                    ovfl_tbl[proto][ix].port, ovfl_tbl[proto][ix].ent,
                    ovfl_tbl[proto][ix].prio, ovfl_tbl[proto][ix].nextIx,
                    ovfl_tbl[proto][ix].refcnt );
    }
    printk( "\n Free entries count = %d\n", free_ovfl_list[proto].count); 
}


static void iq_host_flush_porttbl( iqos_ipproto_t ipProto )
{
    uint8_t proto;
    int ix;

    printk( "Flush Tables ipProto<%d>\n", ipProto );  

    proto = IQ_L4PROTO_IX(ipProto);

    /* flush Hash Table */
    if (proto == IQCTL_PROTO_UDP)
    {
        printk( "\n --------- UDP Proto --------- \n" );
    }
    else
    {
        printk( "\n --------- TCP Proto --------- \n" );
    }

    printk( " Overflow Table \n" );
    printk( " Ix   Port  Ent  Prio  Next  RefCnt\n" );

    /* flush Overflow Table */
    for ( ix=IQ_OVFLTBL_SIZE-1; ix>=0; ix-- )
    {
        if ( ( ovfl_tbl[proto][ix].port != IQOS_INVALID_PORT ) && 
             ( ovfl_tbl[proto][ix].ent == IQOS_ENT_DYN ) )
        {
            printk( "%3d  %5d  %3d  %4d  %4d    %5d\n", ix, 
                    ovfl_tbl[proto][ix].port, ovfl_tbl[proto][ix].ent,
                    ovfl_tbl[proto][ix].prio, ovfl_tbl[proto][ix].nextIx,
                    ovfl_tbl[proto][ix].refcnt );

            iq_rem_dyn_L4port( ipProto, ovfl_tbl[proto][ix].port );
        }
    }

    printk( "\n Hash Table \n" );
    printk( " Ix   Port  Ent  Prio  Next\n" );
    for ( ix=IQ_HASHTBL_SIZE-1; ix > 0; ix-- )
    {
        if ( ( qos_htbl[proto][ix].port != IQOS_INVALID_PORT ) && 
             ( qos_htbl[proto][ix].ent == IQOS_ENT_DYN ) )
        {
            printk( "%3d  %5d  %3d  %4d  %4d    %5d\n", ix, 
                    qos_htbl[proto][ix].port, qos_htbl[proto][ix].ent,
                    qos_htbl[proto][ix].prio, qos_htbl[proto][ix].nextIx,
                    qos_htbl[proto][ix].refcnt );

            iq_rem_dyn_L4port( ipProto, qos_htbl[proto][ix].port );
        }
    }
}


static uint8_t iq_add_L4port( iqos_ipproto_t ipProto, uint16_t destPort, 
        iqos_ent_t ent, iqos_prio_t prio )
{
    uint8_t ix = IQOS_INVALID_NEXT_IX;

    ix = iq_host_add_L4port( ipProto, destPort, ent, prio );

    if ( ix == IQOS_INVALID_NEXT_IX )
        return ix;

#if (defined(CONFIG_BCM_FAP) || defined(CONFIG_BCM_FAP_MODULE))
    if (likely(iqos_fap_add_L4port_hook_g != (iqos_fap_add_L4port_hook_t)NULL) )
        iqos_fap_add_L4port_hook_g( ipProto, destPort, ent, prio ); 
#endif

#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
    if (likely(iqos_runner_add_L4port_hook_g != (iqos_runner_L4port_hook_t)NULL) )
    {
        rdpa_l4_dst_port_to_reason_cfg_t entry;
        bdmf_index idx;

        entry.is_static = (ent == IQOS_ENT_STAT);
        entry.is_tcp = (ipProto == IQOS_IPPROTO_TCP);
        entry.l4_dst_port = destPort;
        if (prio == IQOS_PRIO_HIGH) 
        {
            entry.reason = rdpa_cpu_rx_reason_l4_udef_0;

            // IPv4 DHCP (port 67, 68), IPv6 DHCP (port 546, 547)
            if ((ipProto == IQOS_IPPROTO_UDP) && 
                (destPort == 67 || destPort == 68 || destPort == 546 || destPort == 547))
            {
                entry.reason = rdpa_cpu_rx_reason_dhcp;
            }
        }
        iqos_runner_add_L4port_hook_g(rdpa_iq_cpu_obj, &idx, &entry);
    }
#endif

    return ix;
}


static uint8_t iq_rem_L4port( iqos_ipproto_t ipProto, uint16_t destPort, 
        iqos_ent_t ent )
{
    uint8_t ret = iq_host_rem_L4port( ipProto, destPort, ent );

#if (defined(CONFIG_BCM_FAP) || defined(CONFIG_BCM_FAP_MODULE))
    if (ret && (likely(iqos_fap_rem_L4port_hook_g != (iqos_fap_rem_L4port_hook_t)NULL)) )
        iqos_fap_rem_L4port_hook_g( ipProto, destPort, ent ); 
#endif

#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
    if (likely(iqos_runner_rem_L4port_hook_g != (iqos_runner_rem_L4port_hook_t)NULL) &&
        likely(iqos_runner_find_L4port_hook_g != (iqos_runner_L4port_hook_t)NULL))
    {
        rdpa_l4_dst_port_to_reason_cfg_t entry;
        bdmf_index idx;

        entry.is_static = (ent == IQOS_ENT_STAT);
        entry.is_tcp = (ipProto == IQOS_IPPROTO_TCP);
        entry.l4_dst_port = destPort;

        if (iqos_runner_find_L4port_hook_g(rdpa_iq_cpu_obj, &idx, &entry) == 0)
        {
            iqos_runner_rem_L4port_hook_g(rdpa_iq_cpu_obj, idx); 
        }
        else
        {
            printk("%s unable to find dstport %d\n", __FUNCTION__, destPort);
        }
    }
#endif

    return ret; 
}


static void iq_flush_porttbl( iqos_ipproto_t ipProto )
{
    /* Flush not required for FAP because it is done as part of host flush */ 
    iq_host_flush_porttbl( ipProto ); 
}

static void iq_dump_porttbl( iqos_ipproto_t ipProto )
{
#if !defined(CONFIG_BCM_RDPA) && !defined(CONFIG_BCM_RDPA_MODULE)
    uint8_t ptIdx = 0;
    uint8_t ptValIdx = 0;

    /* dump default protoprio table */
    printk("\n --------------------------------------------");
    printk("\n Dumping Default Protocol Prio table\n");
    for (ptIdx = 0; ptIdx < IQCTL_PROTOTYPE_MAX; ptIdx++) 
    {
        if (ptIdx == IQCTL_PROTOTYPE_IP) 
        {
            printk("\n Prototype: IP PROTO, ProtoVal:prio ");
        }
        if (proto_prio_tbl.entryCount[ptIdx] > 0) 
        {
            for (ptValIdx = 0; ptValIdx < proto_prio_tbl.entryCount[ptIdx]; ptValIdx++) 
            {
                printk("0x%x:%d ", proto_prio_tbl.entry[ptIdx][ptValIdx].protoval,
                       proto_prio_tbl.entry[ptIdx][ptValIdx].prio); 
            }
        }
        else
        {
            printk("NULL:NULL");
        }
    }
    printk("\n --------------------------------------------\n\n");
#endif

    iq_host_dump_porttbl( ipProto ); 
    mdelay(100);

#if (defined(CONFIG_BCM_FAP) || defined(CONFIG_BCM_FAP_MODULE))
    if ( likely(iqos_fap_dump_porttbl_hook_g !=
            (iqos_fap_dump_porttbl_hook_t)NULL) )
        iqos_fap_dump_porttbl_hook_g( ipProto ); 
#endif

#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
    if (likely(iqos_runner_get_L4port_hook_g != (iqos_runner_get_hook_t)NULL) )
    {
        bdmf_index idx;
        rdpa_l4_dst_port_to_reason_cfg_t entry;
        int is_tcp = (ipProto == IQOS_IPPROTO_TCP);

        printk(  "\n----------------IQ Status---------------------\n");
        printk(  " Ix\tl4port\tis_tcp\tstatic\treason\trefcnt\n");
        printk(  "----------------------------------------------\n");
        for (idx = 0; idx < INGQOS_RUNNER_MAX_QOS_RULES; idx++) 
        {
            if (iqos_runner_get_L4port_hook_g(rdpa_iq_cpu_obj, idx, &entry) == 0) 
            {
                if (entry.refcnt > 0 && (entry.is_tcp == is_tcp)) 
                {
                    printk(" %d\t%d\t%d\t%d\t%d\t%d\n", (int)idx, entry.l4_dst_port, entry.is_tcp, entry.is_static, entry.reason, entry.refcnt);
                }
            }
        }
    }
#endif
}


static uint8_t iq_rem_dyn_L4port( iqos_ipproto_t ipProto, uint16_t destPort )
{
    return iq_rem_L4port( ipProto, destPort, IQOS_ENT_DYN );
}

#if 0
static uint8_t iq_rem_stat_L4port( iqos_ipproto_t ipProto, uint16_t destPort )
{
    return iq_rem_L4port( ipProto, destPort, IQOS_ENT_STAT );
}

static uint8_t iq_add_dyn_L4port( iqos_ipproto_t ipProto, uint16_t destPort, 
        iqos_prio_t prio )
{
    return iq_add_L4port( ipProto, destPort, IQOS_ENT_DYN, prio );
}

static uint8_t iq_add_stat_L4port( iqos_ipproto_t ipProto, uint16_t destPort, 
        iqos_prio_t prio )
{
    return iq_add_L4port( ipProto, destPort, IQOS_ENT_STAT, prio );
}
#endif

static uint8_t iq_set_proto_prio( iqctl_prototype_t prototype, uint16_t protoval, 
                                  iqctl_prio_t prio )
{
#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
    {
        /* Not supported in Runner platforms */
        return IQOS_ERROR;
    }
#endif

    if (iq_host_set_proto_prio( prototype, protoval, prio ) == IQOS_ERROR)
    {
        return IQOS_ERROR;
    }

#if (defined(CONFIG_BCM_FAP) || defined(CONFIG_BCM_FAP_MODULE))
    if (likely(iqos_fap_set_proto_prio_hook_g != (iqos_fap_set_proto_prio_hook_t)NULL) )
        iqos_fap_set_proto_prio_hook_g( (int)prototype, protoval, prio ); 
#endif
    return IQOS_SUCCESS;
}

static uint8_t iq_rem_proto_prio( iqctl_prototype_t prototype, uint16_t protoval )
{
#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
    {
        /* Not supported in Runner platforms */
        return IQOS_ERROR;
    }
#endif

    if (iq_host_rem_proto_prio( prototype, protoval ) == IQOS_ERROR)
    {
        return IQOS_ERROR;
    }

#if (defined(CONFIG_BCM_FAP) || defined(CONFIG_BCM_FAP_MODULE))
    if (likely(iqos_fap_rem_proto_prio_hook_g != (iqos_fap_rem_proto_prio_hook_t)NULL) )
        iqos_fap_rem_proto_prio_hook_g( (int)prototype, protoval ); 
#endif
    return IQOS_SUCCESS;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: iq_drv_ioctl
 * Description  : Main entry point to handle user applications IOCTL requests
 *                Ingress QoS Utility.
 * Returns      : 0 - success or error
 *------------------------------------------------------------------------------
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33)
static long iq_drv_ioctl(struct file *filep, unsigned int command, 
                        unsigned long arg)
#else
int iq_drv_ioctl(struct inode *inode, struct file *filep,
                       unsigned int command, unsigned long arg)
#endif
{
    iqctl_ioctl_t cmd;
    iqctl_data_t iq;
    iqctl_data_t *iq_p = &iq;
    int ret = IQOS_SUCCESS;

    if ( command > IQCTL_IOCTL_MAX )
        cmd = IQCTL_IOCTL_MAX;
    else
        cmd = (iqctl_ioctl_t)command;

    copy_from_user( iq_p, (uint8_t *) arg, sizeof(iq) );

    BCM_LOG_DEBUG( BCM_LOG_ID_IQ, "cmd<%d>%s subsys<%d>%s op<%d>%s arg<0x%lx>",
        command, iqctl_ioctl_name[command], 
        iq_p->subsys, iqctl_subsys_name[iq_p->subsys],
        iq_p->op, iqctl_op_name[iq_p->op], arg );

    IQOS_LOCK_BH();

    if (cmd == IQCTL_IOCTL_SYS)
    {
        switch (iq_p->subsys)
        {
            case IQCTL_SUBSYS_DPORT:
            {
                switch (iq_p->op)
                {
                    case IQCTL_OP_ADD:
                    {
                        iqos_ipproto_t ipproto = IQ_IPPROTO(iq_p->proto);
                        iq_add_L4port( ipproto, iq_p->dport, iq_p->ent,
                                iq_p->prio);
                        break;
                    }

                    case IQCTL_OP_REM:
                    {
                        iqos_ipproto_t ipproto = IQ_IPPROTO(iq_p->proto);
                        iq_rem_L4port(ipproto, iq_p->dport, iq_p->ent);
                        break;
                    }

                    case IQCTL_OP_GET:
                    {
                        iqos_ipproto_t ipproto = IQ_IPPROTO(iq_p->proto);
                        iq_get_L4port(ipproto, iq_p->dport,
                          (iqos_ent_t *)&iq_p->ent, (iqos_prio_t *)&iq_p->prio, (uint8_t *)&iq_p->refcnt);
                        copy_to_user( (uint8_t *)arg, iq_p, sizeof(iq) );
                        break;
                    }

                    default:
                        BCM_LOG_ERROR(BCM_LOG_ID_IQ,"Invalid op[%u]", iq_p->op);
                }
                break;
            }

            case IQCTL_SUBSYS_DPORTTBL:
            {
                switch (iq_p->op)
                {
                    case IQCTL_OP_DUMP:
                    {
                        iqos_ipproto_t ipproto = IQ_IPPROTO(iq_p->proto);
                        iq_dump_porttbl(ipproto);
                        break;
                    }

                    case IQCTL_OP_FLUSH:
                    {
                        iqos_ipproto_t ipproto = IQ_IPPROTO(iq_p->proto);
                        iq_flush_porttbl(ipproto);
                        break;
                    }

                    default:
                        BCM_LOG_ERROR(BCM_LOG_ID_IQ,"Invalid op[%u]", iq_p->op);
                }
                break;
            }

            case IQCTL_SUBSYS_STATUS:
            {
                switch (iq_p->op)
                {
                    case IQCTL_OP_GET:
                        iq_get_status();
                        iq_p->status = iqos_enable_g;
                        copy_to_user( (uint8_t *)arg, iq_p, sizeof(iq) );
                        break;

                    case IQCTL_OP_SET:
                        iq_set_status(iq_p->status);
                        break;

                    default:
                        BCM_LOG_ERROR(BCM_LOG_ID_IQ,"Invalid op[%u]", iq_p->op);
                }
                break;
            }

            case IQCTL_SUBSYS_PROTO:
            {
                switch (iq_p->op)
                {
                    case IQCTL_OP_SET:
                        iq_set_proto_prio(iq_p->prototype, iq_p->protoval, 
                                iq_p->prio);
                        break;

                    case IQCTL_OP_REM:
                        iq_rem_proto_prio(iq_p->prototype, iq_p->protoval);
                        break;

                    default:
                        BCM_LOG_ERROR(BCM_LOG_ID_IQ,"Invalid op[%u]", iq_p->op);
                }
                break;
            }
            default:
                BCM_LOG_ERROR(BCM_LOG_ID_IQ,"Invalid subsys[%u]", iq_p->subsys);
        }
    }
    else
    {
        BCM_LOG_ERROR(BCM_LOG_ID_IQ, "Invalid cmd[%u]", command );
        ret = IQOS_ERROR;
    }

    IQOS_UNLOCK_BH();

    return ret;

} /* iq_drv_ioctl */

/*
 *------------------------------------------------------------------------------
 * Function Name: iq_drv_open
 * Description  : Called when a user application opens this device.
 * Returns      : 0 - success
 *------------------------------------------------------------------------------
 */
static int iq_drv_open(struct inode *inode, struct file *filp)
{
    BCM_LOG_DEBUG( BCM_LOG_ID_IQ, "Access Ingress QoS Char Device" );
    return IQOS_SUCCESS;
} /* iq_drv_open */

/* Global file ops */
static struct file_operations iq_fops =
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33)
    .unlocked_ioctl = iq_drv_ioctl,
#if defined(CONFIG_COMPAT)
    .compat_ioctl = iq_drv_ioctl,
#endif
#else
    .ioctl  = iq_drv_ioctl,
#endif

    .open   = iq_drv_open,
};

/*
 *------------------------------------------------------------------------------
 * Function Name: iq_drv_construct
 * Description  : Initial function that is called at system startup that
 *                registers this device.
 * Returns      : None.
 *------------------------------------------------------------------------------
 */

static int iq_drv_construct(void)
{
    bcmLog_setLogLevel( BCM_LOG_ID_IQ, BCM_LOG_LEVEL_NOTICE );

    if ( register_chrdev( IQ_DRV_MAJOR, IQ_DRV_NAME, &iq_fops ) )
    {
        BCM_LOG_ERROR( BCM_LOG_ID_IQ, 
                "%s Unable to get major number <%d>" CLRnl,
                  __FUNCTION__, IQ_DRV_MAJOR);
        return IQOS_ERROR;
    }

    printk( IQ_MODNAME " Char Driver " IQ_VER_STR " Registered<%d>" 
                                                        CLRnl, IQ_DRV_MAJOR );

#if defined(CONFIG_BCM963138) || defined(CONFIG_BCM963148) || defined(CONFIG_BCM94908)
    if (rdpa_cpu_get(rdpa_cpu_host, &rdpa_iq_cpu_obj))
    {
        printk("%s rdpa_cpu_get returned error\n", __FUNCTION__);
        return -ESRCH;
    }

    if (rdpa_cpu_get(rdpa_cpu_wlan0, &rdpa_iq_wlan0_obj))
    {
        printk("%s rdpa_cpu_get returned error\n", __FUNCTION__);
        return -ESRCH;
    }
#endif

    return IQ_DRV_MAJOR;
}

/*
 *------------------------------------------------------------------------------
 * Function Name: iq_drv_destruct
 * Description  : Final function that is called when the module is unloaded.
 * Returns      : None.
 *------------------------------------------------------------------------------
 */
static void iq_drv_destruct(void)
{
    unregister_chrdev( IQ_DRV_MAJOR, IQ_DRV_NAME );

    printk( IQ_MODNAME " Char Driver " IQ_VER_STR " Unregistered<%d>" 
                                                        CLRnl, IQ_DRV_MAJOR );
}


/*
 *------------------------------------------------------------------------------
 * Function     : iq_init
 * Description  : Static construction of ingress QoS subsystem.
 *------------------------------------------------------------------------------
 */
int __init iq_init(void)
{
    int proto, ix;

    if (iq_drv_construct() == IQOS_ERROR)
        return IQOS_ERROR;

    BCM_LOG_DEBUG( BCM_LOG_ID_IQ, "iqos_cpu_cong_g<0x%p>\n", &iqos_cpu_cong_g );

    /* Init Hash Tables */
    for( proto=0; proto<IQCTL_PROTO_MAX; proto++)
        for( ix=0; ix<IQ_HASHTBL_SIZE; ix++)
        {
            qos_htbl[proto][ix].port = IQOS_INVALID_PORT;
            qos_htbl[proto][ix].ent = IQOS_ENT_DYN;
            qos_htbl[proto][ix].prio = IQOS_PRIO_LOW;
            qos_htbl[proto][ix].nextIx = IQOS_INVALID_NEXT_IX;
            qos_htbl[proto][ix].refcnt = 0;
        }

    /* Init Overflow Tables */
    for( proto=0; proto<IQCTL_PROTO_MAX; proto++)
        for( ix=0; ix<IQ_OVFLTBL_SIZE; ix++)
         {
            ovfl_tbl[proto][ix].port = IQOS_INVALID_PORT;
            ovfl_tbl[proto][ix].ent = IQOS_ENT_DYN;
            ovfl_tbl[proto][ix].prio = IQOS_PRIO_LOW;
            ovfl_tbl[proto][ix].nextIx = IQOS_INVALID_NEXT_IX;
            ovfl_tbl[proto][ix].refcnt = 0;
        }

    /* setup the free linked list Tables */
    for( proto=0; proto<IQCTL_PROTO_MAX; proto++)
    {
        for( ix=1; ix<(IQ_OVFLTBL_SIZE-1); ix++)
            ovfl_tbl[proto][ix].nextIx = ix+1;

        ovfl_tbl[proto][IQ_OVFLTBL_SIZE-1].nextIx = 0;
        free_ovfl_list[proto].nextIx = 1; /* 0 is reserved */
        free_ovfl_list[proto].count = (IQ_OVFLTBL_SIZE-1); 
     }

    for (ix = 0; ix < IQCTL_PROTOTYPE_MAX; ix++)
    {
        proto_prio_tbl.entryCount[ix] = 0; 
    }

    iqos_bind( iq_add_L4port, iq_rem_L4port, iq_prio_L4port );

    /* NOTE: SIP and RTSP ports have been added in their modules */

    /* Init UDP port # */
    iq_add_L4port(IQOS_IPPROTO_UDP, 53, 1, 1);    /* DNS UDP port 53     */
    iq_add_L4port(IQOS_IPPROTO_UDP, 67, 1, 1);    /* DHCP UDP port 67    */
    iq_add_L4port(IQOS_IPPROTO_UDP, 68, 1, 1);    /* DHCP UDP port 68    */
    iq_add_L4port(IQOS_IPPROTO_UDP, 546, 1, 1);   /* DHCP UDP port 546   */
    iq_add_L4port(IQOS_IPPROTO_UDP, 547, 1, 1);   /* DHCP UDP port 547   */
    iq_add_L4port(IQOS_IPPROTO_UDP, 2427, 1, 1);  /* MGCP UDP port 2427  */
    iq_add_L4port(IQOS_IPPROTO_UDP, 2727, 1, 1);  /* MGCP UDP port 2727  */

    /* Init TCP port # */
    iq_add_L4port(IQOS_IPPROTO_TCP, 53, 1, 1);    /* DNS TCP port 53     */
    iq_add_L4port(IQOS_IPPROTO_TCP, 80, 1, 1);    /* HTTP TCP port       */
    iq_add_L4port(IQOS_IPPROTO_TCP, 8080, 1, 1);  /* HTTP TCP port       */

    iqos_enable_g = 1;
    BCM_LOG_DEBUG( BCM_LOG_ID_IQ, 
        "iqos_enable_g<0x%p> qos_htbl<0x%p> ovfl_tbl<0x%p> "
        "iqos_debug_g<0x%p>\n", 
        &iqos_enable_g, &qos_htbl[0], &ovfl_tbl[0], &iqos_debug_g );

    printk( "\nBroadcom Ingress QoS ver 0.1 initialized\n" );
    return 0;
}

/*
 *------------------------------------------------------------------------------
 * Function     : iq_exit
 * Description  : Destruction of Ingress QoS subsystem.
 *------------------------------------------------------------------------------
 */
void iq_exit(void)
{
    iq_drv_destruct();
    iqos_enable_g = 0;
    printk( "\nBroadcom Ingress QoS uninitialized\n" );
}



module_init(iq_init);
module_exit(iq_exit);

MODULE_DESCRIPTION(IQ_MODNAME);
MODULE_VERSION(IQ_VERSION);

MODULE_LICENSE("Proprietary");




