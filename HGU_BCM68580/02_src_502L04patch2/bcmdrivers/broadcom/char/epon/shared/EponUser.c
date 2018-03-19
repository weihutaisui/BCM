/*
*  Copyright 2011, Broadcom Corporation
*
* <:copyright-BRCM:2011:proprietary:standard
* 
*    Copyright (c) 2011 Broadcom 
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

//**************************************************************************
// File Name  : epon_usr.c
// This file mainly used to manager the resource for epon stack.
///1)Create Global timer for ALL EPON stack polling hanlde.
///2)Create MPCP queue and kernel thread.
///3)Create OAM queue and tasklet.
///4)Register PON MAC reset call back.
///5)Create netlink interface for event transmit to user space.
///6)Implement the MPCP and OAM packets transmit function for each link.
// Description: Broadcom EPON  Interface Driver
//               
//**************************************************************************
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <linux/skbuff.h>
#include <asm/uaccess.h>
#include <net/sock.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include "bcm_OS_Deps.h"
#include "bcm_map_part.h"
#include <linux/bcm_log.h>
#include "EponUser.h"
#include "OntDirector.h"
#include "board.h"
#include "boardparms.h"

#include "rdpa_api.h"
#include "autogen/rdpa_ag_epon.h"

#ifdef EPON_MPCP_SUPPORT
#include "EponTimer.h"
#include "PonMgrEncrypt.h"
#ifdef CONFIG_EPON_CLOCK_TRANSPORT
#include "ClockTransport.h"
#endif
#include "OptCtrl.h"
#include "OntmMpcp.h"
#include "OsAstMsgQ.h"
#include "CtcStats.h"
#include "CtcAlarms.h"
#include "EponFrame.h"
#include "EponCtrl.h"
#include "EponRogueDrv.h"
#endif

#include "epon_bdmf_interface.h"
#include "epon_drv.h"

static struct sock *epon_usr_nl_sk = NULL; //net link sock intercace
static int epon_usr_nl_pid = 0 ;
static int epon_mac_mode = EPON_MAC_UNCONFIGURED;
BOOL epon_mac_init_done = FALSE;
#define MAX_EPON_MSGLEN         640

BOOL eponUsrDebugEn[DebugModuleCount];

#ifdef EPON_MPCP_SUPPORT
extern int rdpa_epon_declare_and_register_to_bdmf(void);
extern void rdpa_epon_bdmf_unregister(void);
extern void EponDyingGaspIfInfo(void);
#endif
#ifdef _BYTE_ORDER_LITTLE_ENDIAN_       /* To limit the definition to ARM-based systems */
extern const ru_block_rec *RU_EPON_BLOCKS[];
void remap_ru_block_addrs(uint32_t block_index, const ru_block_rec *ru_blocks[]);
#endif


void DumpData (U8 level,U8 *name,U8 * addr, U16 size)
    {
    U16  i;
    eponUsrDebug (level,
    ("-----------%s DataSize:%d---------------\n",name,size));
    for (i = 0; i < size; ++i)
        {
        if ((i % 16) == 0)
            {
            eponUsrDebug (level,("\n"));
            }
        eponUsrDebug(level,("%02x ", (*addr++)));
        }

    eponUsrDebug(level,("\n"));	
    } // DisplayMemory


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Netlink between epon_stack and OAM_stack
static void EponUsrSendNLMsg(int type, const U8 *msgData, int msgLen)
    {
    struct sk_buff* skb = NULL;
    struct nlmsghdr* nl_msghdr = NULL;
    unsigned int payloadLen = sizeof(struct nlmsghdr);

    if(!epon_usr_nl_pid)
        {
        eponUsrDebug(DebugAlarm,(" epon_usr_nl_pid is not initialized\n"));
        return;
        }

    if(msgData && (msgLen > MAX_EPON_MSGLEN))
        {
        eponUsrDebug(DebugAlarm,("Invalid msgLen\n"));
        return;
        }

    payloadLen += msgLen;
    payloadLen = NLMSG_SPACE(payloadLen);

    if(in_atomic())
        {
        skb = alloc_skb(payloadLen, GFP_ATOMIC);
        }
    else
        {
        skb = alloc_skb(payloadLen, GFP_KERNEL);
        }
    if(!skb)
        {
        eponUsrDebug(DebugAlarm,("Failed to allocate skb in %s\n", __FUNCTION__));
        return;
        }

    nl_msghdr= (struct nlmsghdr*)skb->data;
    nl_msghdr->nlmsg_type   = type;
    nl_msghdr->nlmsg_pid    = epon_usr_nl_pid;
    nl_msghdr->nlmsg_len    =payloadLen;
    nl_msghdr->nlmsg_flags = 0;

    if(msgData)
        {
        DumpData(DebugAlarm,"EponUsrSendNLMsg",(U8 *)msgData,msgLen);
        memcpy(NLMSG_DATA(nl_msghdr), msgData, msgLen);
        }

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0)
    NETLINK_CB(skb).pid = 0;
#endif
    skb->len = payloadLen;

    netlink_unicast(epon_usr_nl_sk, skb, epon_usr_nl_pid, MSG_DONTWAIT);
    return;
    }


void EponNLSetPid(int pid)
    {
    epon_usr_nl_pid = pid;
    }

#ifdef EPON_MPCP_SUPPORT
static void EponRcvTask(struct sk_buff * skb)
    {
    kfree_skb(skb);
    return;
    }
static void EponInitNLSocket(void)
    {
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0)
    epon_usr_nl_sk = netlink_kernel_create(&init_net, 
        NETLINK_BRCM_EPON, 0, EponRcvTask, NULL, THIS_MODULE);
#else
    struct netlink_kernel_cfg cfg = {
        .input  = EponRcvTask,
        };
    epon_usr_nl_sk = netlink_kernel_create(&init_net, NETLINK_BRCM_EPON, &cfg);
#endif

    if(!epon_usr_nl_sk)
        {
        BCM_LOG_ERROR(BCM_LOG_ID_EPON, "Faild to Create Netlink Socket for EPON");
        }

    epon_usr_nl_pid = 0;
    return;
    }

static void EponCleanupNLSocket(void)
    {
    epon_usr_nl_pid = 0;
    sock_release(epon_usr_nl_sk->sk_socket);
    }
#endif

/////////////////////////////////////////////////////////////////////
// API for send Netlink message to OAM_stack
void EponAlarmMsgSend(const U8 *msg,int length)
    {
    EponUsrSendNLMsg(0,msg,length);
    }

#ifdef EPON_MPCP_SUPPORT
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void EponMacInit(void)
    {
    EponUserTimerReset();
    OsAstMsgQInit();

    //hardware init
    local_bh_disable();
    PonMgrInit();
    OptCtrlInit();
    HoldoverInit();
    PsInit();// must come after holdover init
    
    OntDirInit();
    OntmMpcpInit();
    EponRogueDetInit();
    local_bh_enable();

   //Local data init
    EncryptCfgInit();
    StatsInit();
    CtcStatsInit();
    OamCtcEvtInit();
    EponFrameTaskTimerReg();
#ifdef CONFIG_EPON_CLOCK_TRANSPORT
    ClkTransInit();
#endif
    EponDyingGaspIfInfo();
        
    epon_mac_init_done = TRUE;
    }
#endif

void EponUserConfigObjectRate(LaserRate dnRate, LaserRate upRate)
    {
    bdmf_object_handle epon_obj;
    rdpa_epon_rate currRate, newRate;
    U8 currDnRate, currUpRate;
    int rc;

    if ( rdpa_epon_get(&epon_obj) )
    return;

    rc = rdpa_epon_rate_get(epon_obj, &currRate);
    if (rc)
        return;

    if (currRate == rdpa_epon_rate_10g10g)
        currUpRate = LaserRate10G;
    else
        currUpRate = LaserRate1G;

    if (currRate == rdpa_epon_rate_1g1g)
        currDnRate = LaserRate1G;
    else if (currRate == rdpa_epon_rate_2g1g)
        currDnRate = LaserRate2G;
    else
        currDnRate = LaserRate10G;

    //check any valid change
    if ((currDnRate == dnRate) && (currUpRate == upRate))
        return;

    if (dnRate != 0xFF)
        currDnRate = dnRate;

    if (upRate != 0xFF)
        currUpRate = upRate;

    if (currDnRate == LaserRate1G)
        newRate = rdpa_epon_rate_1g1g;
    else if(currDnRate == LaserRate2G)
        newRate = rdpa_epon_rate_2g1g;
    else
        {
        if (currUpRate == LaserRate1G)
            newRate = rdpa_epon_rate_10g1g;
        else 
            newRate = rdpa_epon_rate_10g10g;
        }

    rdpa_epon_rate_set(epon_obj, newRate);
    bdmf_put(epon_obj);
    }

void EponTcToQueueTableInit(rdpa_traffic_dir dir)
    {
    int ret = 0;
    int rc  = 0;
    U8  i   = 0;
    bdmf_object_handle tc_to_q_obj = NULL;
    rdpa_tc_to_queue_key_t t2q_key;
    BDMF_MATTR(rdpa_tc_to_queue_attrs, rdpa_tc_to_queue_drv());

    t2q_key.dir = dir; 
    t2q_key.table = 0; 

    if ((ret = rdpa_tc_to_queue_get(&t2q_key, &tc_to_q_obj)))
        {
        rdpa_tc_to_queue_table_set(rdpa_tc_to_queue_attrs, t2q_key.table);
        rdpa_tc_to_queue_dir_set(rdpa_tc_to_queue_attrs, t2q_key.dir);
        rc = bdmf_new_and_set(rdpa_tc_to_queue_drv(), NULL, rdpa_tc_to_queue_attrs, &tc_to_q_obj);      
        }

    if (!rc)
        {
        for (i = 0; i < 8; i++)
            { 
            rdpa_tc_to_queue_tc_map_set(tc_to_q_obj, i, i);
            }
        }

    if (!ret)
        bdmf_put(tc_to_q_obj);
    }

epon_mac_mode_e EponGetMacMode()
    {
    int count= 0;
    char buf[PSP_BUFLEN_16];

    if(epon_mac_mode == EPON_MAC_UNCONFIGURED)
        {
        count = kerSysScratchPadGet(RDPA_WAN_TYPE_PSP_KEY, buf, sizeof(buf));
        if (count > 0)
            {
            buf[count] = '\0';
            if (!strcasecmp(buf ,RDPA_WAN_TYPE_VALUE_EPON))
                {
                    epon_mac_mode = EPON_MAC_NORMAL_MODE;
                }
            else if(!strcasecmp(buf ,RDPA_WAN_TYPE_VALUE_GBE))
                {
                count = kerSysScratchPadGet(RDPA_WAN_OEMAC_PSP_KEY, buf, sizeof(buf));
                if (count > 0)
                    {
                    buf[count] = '\0';
                    if (!strcasecmp(buf, "EPONMAC"))
                        {
                            epon_mac_mode = EPON_MAC_AE_MODE;
                        }
                    }
                }
            }
        }

        return epon_mac_mode;
    }

static rdpa_wan_type GetRdpaWanType(void)
    {
    rdpa_wan_type wanType = rdpa_wan_none;

    if(epon_mac_mode == EPON_MAC_NORMAL_MODE)
        wanType = (PonCfgDbGetUpRate() == LaserRate10G) ? rdpa_wan_xepon : rdpa_wan_epon;
    else if(epon_mac_mode == EPON_MAC_AE_MODE)
        {
        if((PonCfgDbGetWanRateMode() == WanRateFixMode) && (PonCfgDbGetUpRate() == LaserRate1G))
            wanType = rdpa_wan_epon;
        else
            wanType = rdpa_wan_xepon;
        }

    return wanType;
    }

////////////////////////////////////////////////////////////////////////////////
//create rdpa WAN port for epon object
int EponCreateWanPort(void)
    {
    BDMF_MATTR(port_attrs, rdpa_port_drv());
    BDMF_MATTR(rdpa_epon_attrs, rdpa_epon_drv());
    bdmf_object_handle eponObj = NULL;
    bdmf_object_handle wanPort = NULL;
    bdmf_object_handle cpu_obj = NULL;
    rdpa_wan_type wanType;
    rdpa_port_dp_cfg_t port_cfg = {0};
    int rc, macMode = EponGetMacMode();

    port_cfg.sal_enable = 1;
    port_cfg.dal_enable = 1;
    port_cfg.sal_miss_action = rdpa_forward_action_host;
    port_cfg.dal_miss_action = rdpa_forward_action_host;
    port_cfg.control_sid = rdpa_if_none;
    port_cfg.emac = rdpa_emac_none;
    port_cfg.physical_port = rdpa_physical_none;
    port_cfg.ls_fc_enable = 0; 
    /* Create logical ports object on top of the physical wan */
    wanType = GetRdpaWanType();
    if (wanType == rdpa_wan_none)
        {
        BCM_LOG_ERROR(BCM_LOG_ID_EPON, "Failed to get correct epon wan type");
        return -1;
        }

    rdpa_port_index_set(port_attrs, rdpa_wan_type_to_if(wanType));
    rdpa_port_wan_type_set(port_attrs, wanType);
   
    if (macMode == EPON_MAC_AE_MODE)
        {
        port_cfg.ae_enable = 1;     
        rdpa_port_cfg_set(port_attrs, &port_cfg);
        rdpa_port_speed_set(port_attrs, (PonCfgDbGetUpRate() == LaserRate10G) ? 
                rdpa_speed_10g : rdpa_speed_1g);
        }
        else
        {
        rdpa_port_cfg_set(port_attrs, &port_cfg);
        }

    rc = bdmf_new_and_set(rdpa_port_drv(), NULL, port_attrs, &wanPort);
    if (rc < 0)
        {
        BCM_LOG_ERROR(BCM_LOG_ID_EPON, "Failed to create wan port");
        return -1;
        }

    if(macMode == EPON_MAC_NORMAL_MODE)
        {
        rc = bdmf_new_and_set(rdpa_epon_drv(), wanPort, rdpa_epon_attrs, &eponObj);
        if (rc)
            {
            bdmf_put(wanPort);
            BCM_LOG_ERROR(BCM_LOG_ID_EPON, "Failed to create RDPA epon object rc(%d)", rc);
            return -1;
            }
        }

    rc = rdpa_cpu_get(rdpa_cpu_host, &cpu_obj);
    if (rc)
        {
        bdmf_put(wanPort);
        BCM_LOG_ERROR(BCM_LOG_ID_EPON, "Failed to get CPU object, rc(%d)", rc);
        return -1;
        }
#ifdef CONFIG_BCM_PON_XRDP
    rc = rdpa_port_cpu_obj_set(wanPort, cpu_obj);
    if (rc)
        {
        bdmf_put(wanPort);
        bdmf_put(cpu_obj);
        BCM_LOG_ERROR(BCM_LOG_ID_EPON, "Failed to set CPU object to WAN port, rc(%d)", rc);
        return -1;
        }
#endif
    bdmf_put(cpu_obj);

    return 0;
    }

static
void RemoveEponWanPort(void)
    {
    bdmf_object_handle wanPort = NULL;

    rdpa_epon_get(&wanPort);
    if (wanPort)
        bdmf_destroy(wanPort);
    }


void EponFuncRegister(void)
    {
#ifdef EPON_MPCP_SUPPORT
    global_shaper_cb = (void *)OntDirGlobalShaperSet;
    epon_link_shaper_cb = (void *)OntDirLinkShaperSet;
    p_OntmMpcpAssignMcast = OntmMpcpAssignMcast;
    p_OntDirFecModeSet = OntDirFecModeSet;
    p_OntDirFecModeGet = OntDirFecModeGet;
    p_OntDirNewLinkNumSet = OntDirNewLinkNumSet;
    p_OntDirRegLinkNumGet = OntDirRegLinkNumGet;    
    p_OntDirLaserTxModeSet = OntDirLaserTxModeSet;
    p_OntDirLaserTxModeGet= OntDirLaserTxModeGet;
    p_OntDirLaserRxPowerSet = OntDirLaserRxPowerSet;
    p_OntDirLaserRxPowerGet = OntDirLaserRxPowerGet;
    p_OntDirMpcpStateGet = OntDirMpcpStateGet;
    p_OntDirHoldoverGet = OntDirHoldoverGet;
    p_OntDirHoldoverSet = OntDirHoldoverSet;
    p_OntDirEponReset = OntDirEponReset;
    p_PonLosCheckTimeGet = PonLosCheckTimeGet;
    p_GateLosCheckTimeGet = GateLosCheckTimeGet;
    p_PonLosCheckTimeSet = PonLosCheckTimeSet;
    p_GateLosCheckTimeSet = GateLosCheckTimeSet;
    p_PonMgrActToWanState = PonMgrActToWanState;
    p_PonMgrUserTrafficGet = PonMgrUserTrafficGet;
    p_OntDirBurstCapSet = OntDirBurstCapSet;
    p_OntDirBurstCapGet = OntDirBurstCapGet;
    p_PonMgrLinkFlushSet = OntFlushLinkSet;
#endif    
    }


void EponFuncDeRegister(void)
    {
#ifdef EPON_MPCP_SUPPORT    
    global_shaper_cb = NULL;
    epon_link_shaper_cb = NULL;
    p_OntmMpcpAssignMcast = NULL;
    p_OntDirFecModeSet = NULL;
    p_OntDirFecModeGet = NULL;
    p_OntDirNewLinkNumSet = NULL;
    p_OntDirRegLinkNumGet = NULL;    
    p_OntDirLaserTxModeSet = NULL;
    p_OntDirLaserTxModeGet= NULL;
    p_OntDirLaserRxPowerSet = NULL;
    p_OntDirLaserRxPowerGet = NULL;
    p_OntDirMpcpStateGet = NULL;
    p_OntDirHoldoverGet = NULL;
    p_OntDirHoldoverSet = NULL;
    p_OntDirEponReset = NULL;
    p_PonLosCheckTimeGet = NULL;
    p_GateLosCheckTimeGet = NULL;
    p_PonLosCheckTimeSet = NULL;
    p_GateLosCheckTimeSet = NULL;
    p_PonMgrActToWanState = NULL;
    p_PonMgrUserTrafficGet = NULL;
    p_OntDirBurstCapSet = NULL;
    p_OntDirBurstCapGet = NULL;
    p_PonMgrLinkFlushSet = NULL;
#endif    
    }

////////////////////////////////////////////////////////////////////////////////
//init epon usr function
void EponDriverInit(void)
    {
    //Need read Pers from file system
    PonCfgDbInitPon();
#ifdef EPON_MPCP_SUPPORT    
    if (EponGetMacMode() == EPON_MAC_NORMAL_MODE)
        {
        EponInitNLSocket();
        EponFrameTaskInit();
        }
#endif
    //init timer
    EponStackTimerInit();
#ifdef EPON_MPCP_SUPPORT    
    rdpa_epon_declare_and_register_to_bdmf();
#endif
    //create rdpa wan port
    EponCreateWanPort();
    // register function from outside call
    EponFuncRegister();

#ifdef _BYTE_ORDER_LITTLE_ENDIAN_       /* To limit the definition to ARM-based systems */
#if defined(CONFIG_BCM96836) || defined(CONFIG_BCM96846) || defined(CONFIG_BCM96856) 
    remap_ru_block_addrs(XRDP_IDX, RU_EPON_BLOCKS);
#else
    remap_ru_block_addrs(EPON_IDX, RU_EPON_BLOCKS);
#endif
#endif //_BYTE_ORDER_LITTLE_ENDIAN_

#ifdef EPON_MPCP_SUPPORT    
#ifdef USE_BDMF_SHELL
    register_epon_drv_shell_commands();
#endif
#endif

    BCM_LOG_INFO(BCM_LOG_ID_EPON, "EPON driver init done");
    }

//exist epon usr function
void EponDriverExit(void)
    {
    EponStackTimerExit();
#ifdef EPON_MPCP_SUPPORT 
    if(EponGetMacMode() == EPON_MAC_NORMAL_MODE)
        {
        EponCleanupNLSocket();
        EponFrameTaskExit();
        }
    OsAstMsgQExit();
#endif
    RemoveEponWanPort();
#ifdef EPON_MPCP_SUPPORT
    rdpa_epon_bdmf_unregister();
#endif
    EponFuncDeRegister();

    BCM_LOG_INFO(BCM_LOG_ID_EPON, "EPON driver exit done");
    }

//end of EponUser.c

