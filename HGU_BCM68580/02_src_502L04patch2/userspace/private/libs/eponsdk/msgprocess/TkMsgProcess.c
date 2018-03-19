/*
* <:copyright-BRCM:2013:proprietary:epon
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


#include "TkMsgProcess.h"
#include "TkOamCommon.h"
#include "TkPlatform.h"
#include "config.h"
#include "cms_msg.h"
#include "cms_log.h"
#include "cms_lck.h"
#include "TkOamMem.h"
#include "OamIeee.h"
#include "OsAstMsgQ.h"
#include "OnuOs.h"
#include "TkDebug.h"
#include "eponctl_api.h"
#include <linux/rtnetlink.h>
#include <linux/if_link.h>
#include <linux/if_bridge.h>
#include <linux/netlink.h>
#include <sys/socket.h>
#include "OamProcessInit.h"
#ifdef CLOCK_TRANSPORT
#include "ClockTransport.h"
#endif
#ifdef BRCM_CMS_BUILD
#include "cms.h"
#include "cms_util.h"
#include "cms_core.h"
#include "mdm.h"
#include "mdm_private.h"
#endif

extern OamExtCallback OamReqHandle;
extern LinkConfig gTagLinkCfg;
extern U8 gTagCount;
extern void *msgHandle;

extern OamHandlerCB *oamVenderHandlerHead;

const OnuAssertId nlMsgMapToAsserts[NlMsgNums] =
{
    ////////////////////////////////////////////////////////////////////////////
    /// Os Alarm Assert Id
    ////////////////////////////////////////////////////////////////////////////
    // per-ONT alarms  
    OsAstAlmOnt1GDnRegistered,
    OsAstAlmOnt1GDnActivity,
    OsAstAlmOnt10GDnRegistered,
    OsAstAlmOnt10GDnActivity,
    OsAstAlmOnt1GUpRegistered,
    OsAstAlmOnt1GUpActivity,
    OsAstAlmOnt10GUpRegistered,
    OsAstAlmOnt10GUpActivity,
    OsAstAlmOntSingleLinkReg,
    OsAstAlmOntMultiLinkReg,

    OsAstAlmOntFecUp,
    OsAstAlmOntFecDn,
    OsAstAlmOntProtSwitch,
    OsAstAlmOntRogueOnu,

    // EPON port alarms             
    OsAstAlmEponLos,
    OsAstAlmEponLostSync,
    OsAstAlmEponStatThreshold,
    OsAstAlmEponActLed,
    OsAstAlmEponStandbyLos,
    OsAstAlmEponLaserShutdownTemp,
    OsAstAlmEponLaserShutdownPerm,
    OsAstAlmEponLaserOn,

    // per-LLID alarms
    OsAstAlmLinkNoGates,
    OsAstAlmLinkUnregistered,
    OsAstAlmLinkOamTimeout,
    OsAstAlmLinkOamDiscComplete,
    OsAstAlmLinkStatThreshold,
    OsAstAlmLinkKeyExchange,
    OsAstAlmLinkLoopback,
    OsAstAlmLinkAllDown,
    OsAstAlmLinkDisabled,
    OsAstAlmLinkRegStart,
    OsAstAlmLinkRegSucc,
    OsAstAlmLinkRegFail,
    OsAstAlmLinkRegTimeOut,
};

BOOL send_notification_to_cms(LinkIndex link, Bool in_service)
{
#ifdef BRCM_CMS_BUILD
    char buf[sizeof(CmsMsgHeader) + sizeof(EponLinkStatusMsgBody)] = {0};
    CmsRet cmsReturn;
    CmsMsgHeader *msgHdr = (CmsMsgHeader *)buf;
    EponLinkStatusMsgBody *msgBody = (EponLinkStatusMsgBody *)(msgHdr + 1);

    msgHdr->dst = EID_SSK;
    msgHdr->src = EID_EPON_APP;
    msgHdr->type = CMS_MSG_EPON_LINK_STATUS_CHANGED;
    msgHdr->flags_event = 0;
    msgHdr->flags_request = 1;
    msgHdr->dataLength = sizeof(EponLinkStatusMsgBody);

    strncpy(msgBody->l2Ifname, EPON_WAN_IF_NAME, IFNAMESIZ);

    msgBody->linkStatus = in_service? 1 : 2;
    msgBody->link = link;
    msgBody->vlanId = -1;
    msgBody->pbits = -1;


    cmsLog_notice("l2Ifname=%s, linkStatus=%d", msgBody->l2Ifname, 
                                               msgBody->linkStatus);

    cmsReturn = cmsMsg_sendAndGetReplyWithTimeout(msgHandle, msgHdr, CMSLCK_MAX_HOLDTIME);

    if (cmsReturn != CMSRET_SUCCESS)
    {
        if (cmsReturn == CMSRET_TIMED_OUT)
        {
            cmsLog_notice("Send cms link status %d timeout: linkId=%d",  
                      msgBody->linkStatus,
                      link);
        }
        else
        {
            cmsLog_error("Send cms link status %d fail: %d",  
                      msgBody->linkStatus,
                      cmsReturn);
        }
        return FALSE;
    }
    else
    {
        cmsLog_notice("send cms link status %d successful", 
                      msgBody->linkStatus);
        return TRUE;
    }   
#else
    return TRUE;
#endif
}

BOOL send_bcm_hgu_notification_to_cms(LinkIndex link, Bool in_service, S32 vlan_id, S32 pbits)
{
#ifdef BRCM_CMS_BUILD
    char buf[sizeof(CmsMsgHeader) + sizeof(EponLinkStatusMsgBody)] = {0};
    CmsRet cmsReturn;
    CmsMsgHeader *msgHdr = (CmsMsgHeader *)buf;
    EponLinkStatusMsgBody *msgBody = (EponLinkStatusMsgBody *)(msgHdr + 1);

    msgHdr->dst = EID_SSK;
    msgHdr->src = EID_EPON_APP;
    msgHdr->type = CMS_MSG_EPON_LINK_STATUS_CHANGED;
    msgHdr->flags_event = 0;
    msgHdr->flags_request = 1;
    msgHdr->dataLength = sizeof(EponLinkStatusMsgBody);

    strncpy(msgBody->l2Ifname, EPON_WAN_IF_NAME, IFNAMESIZ);

    msgBody->linkStatus = in_service? 1 : 2;
    msgBody->link = link;
    msgBody->vlanId = (vlan_id == 0x0fff ? -1 : vlan_id);
    msgBody->pbits = pbits;

    cmsLog_notice("l2Ifname=%s, linkStatus=%d, vlanid=%d, pbits=%d\n",
        msgBody->l2Ifname, msgBody->linkStatus, msgBody->vlanId, msgBody->pbits);

    cmsReturn = cmsMsg_send(msgHandle, msgHdr);

    if (cmsReturn != CMSRET_SUCCESS)
    {
        cmsLog_error("Send cms link status %d fail: %d",  
                      msgBody->linkStatus,
                      cmsReturn);
        return FALSE;
    }
    else
    {
        cmsLog_notice("send cms link status %d successful", 
                      msgBody->linkStatus);
        return TRUE;
    }   
#else
    return TRUE;
#endif
}

static BOOL epon_netlink_handle_brcm(NlMsgData *data)
{
    if (data->almid < NlMsgNums)
    {
        if (data->raise)
        {
            OsAstMsgQSet (nlMsgMapToAsserts[data->almid], data->inst, 0);
        }
        else
        {
            OsAstMsgQClr (nlMsgMapToAsserts[data->almid], data->inst, 0);
        }
    }
    
    switch(data->almid)
    {
        case NlMsgLinkRegSucc:
            cmsLog_notice("link[%d] reg event: %s", data->inst, data->raise?
                                                    "raised":"cleared");
            if(data->raise)
            {
                OamLinkStart(data->inst);
            }
            else
            {
                WanStatePara para;

                OamLinkInit(data->inst);
                para.index = data->inst;
                para.enable = FALSE;

                if(eponStack_CfgL2PonState(EponSetOpe, &para) != 0)
                {
                    cmsLog_error("failed to set L2 Pon state in epon stack");    
                }
            }
            break;
            
        case NlMsgLinkFailSafeReset:
            system("reboot");
            break;
            
        default:
            cmsLog_debug("unknown almid: 0x%x", data->almid);
            break;
    }

    return TRUE;
}

static BOOL epon_netlink_handle(NlMsgData *data)
{
   switch(data->type)
   {
       case nl_type_brcm:
           epon_netlink_handle_brcm(data); 
           break;
#ifdef CTC_OAM_BUILD
       case nl_type_ctc:
           break;
#endif
       default:
           cmsLog_error("unknown netlink message: %d", data->type);
           break;

   }
   return TRUE; 
}

BOOL TkDataProcessHandle_NetlinkEponMsg(struct msghdr *msg, U16 len)
{
   struct nlmsghdr* nlh = NULL;
   U8 alarm_count;
   NlMsg *ptr = NULL; 
   NlMsgData *data = NULL;

   nlh = (struct nlmsghdr*)msg->msg_iov->iov_base;
   ptr = (NlMsg *)NLMSG_DATA(nlh);
   data = (NlMsgData *)&(ptr->data);
  
   cmsLog_debug("received nl msg:");
   if(cmsLog_getLevel() == LOG_LEVEL_DEBUG) 
   {
       DumpEthFrame((void *)ptr, 64);    
   }

   alarm_count = ptr->hd.count;

   while(alarm_count > 0)
   {
       epon_netlink_handle(data);
       data += sizeof(NlMsgData);
       alarm_count--;
   } 
   return TRUE;
}


#ifdef BRCM_CMS_BUILD
void TkDataProcessHandle_CmsMsg(CmsMsgHeader *pMsg )
{
   CmsRet ret;
   
   switch(pMsg->type)
   {
       case CMS_MSG_SET_LOG_LEVEL:
           cmsLog_debug("got set log level to %d", pMsg->wordData);
           cmsLog_setLevel(pMsg->wordData);
           if ((ret = cmsMsg_sendReply(msgHandle, pMsg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
           {
              cmsLog_error("send response for msg 0x%x failed, ret=%d", pMsg->type, ret);
           }
           break;
   
       case CMS_MSG_SET_LOG_DESTINATION:
           cmsLog_debug("got set log destination to %d", pMsg->wordData);
           cmsLog_setDestination(pMsg->wordData);
           if ((ret = cmsMsg_sendReply(msgHandle, pMsg, CMSRET_SUCCESS)) != CMSRET_SUCCESS)
           {
              cmsLog_error("send response for msg 0x%x failed, ret=%d", pMsg->type, ret);
           }
           break;

        case CMS_MSG_ETH_LINK_UP:
        case CMS_MSG_ETH_LINK_DOWN:
        {
            int len;
            char ifName[IFNAMESIZ];

            len = pMsg->dataLength;
            if(len >IFNAMESIZ || len < 4)
            {
                cmsLog_error("Wrong len msg");
            }
            else
            {
                char *dataPtr = (char *) (pMsg+1);
                U8 inst = 0;
                strncpy(ifName, dataPtr, len);
                inst = ifName[3] - '0';
                if(inst >= UniCfgDbGetActivePortCount())
                {
                    cmsLog_error("Wrong inst in msg");
                }
                else
                {
                    CMS_ETH_LINK_ST_HANDLE(pMsg->type, inst);
                }
            }
        }
            break;

       default:
          cmsLog_debug("ingore msg 0x%x", pMsg->type);
           break;
   }
}
#endif


BOOL TkDataProcessHandle_NetlinkRouteMsg(struct msghdr *msg, U16 len)
{
    struct nlmsghdr* nlh = NULL;
    NlMsg *ptr = NULL; 
    struct nlmsghdr  *nl_msgHdr;
    struct ifinfomsg *ifi;
    struct rtattr    *tb[IFLA_MAX + 1];
    struct rtattr    *rta;
    char             *name;
    U8              state;
    U8              port = 0;
 
    nlh = (struct nlmsghdr*)msg->msg_iov->iov_base;
    ptr = (NlMsg *)NLMSG_DATA(nlh);
 
    if(cmsLog_getLevel() == LOG_LEVEL_NOTICE) 
    {
        DumpEthFrame((void *)ptr, 128);    
    }
 
    for(nl_msgHdr = (struct nlmsghdr *) nlh; NLMSG_OK (nl_msgHdr, (unsigned int)len);
        nl_msgHdr = NLMSG_NEXT (nl_msgHdr, len))
    {
        cmsLog_debug("length:%d,type:%d,flag:%d,seq:%d,pid:%d\n",
            nl_msgHdr->nlmsg_len, nl_msgHdr->nlmsg_type, nl_msgHdr->nlmsg_flags,
            nl_msgHdr->nlmsg_seq, nl_msgHdr->nlmsg_pid);

        DumpEthFrame((void *)nl_msgHdr, 128);    
        /* Finish reading */
        if (nl_msgHdr->nlmsg_type == NLMSG_DONE)
            return TRUE;

        /* Message is some kind of error */
        if (nl_msgHdr->nlmsg_type == NLMSG_ERROR)
        {
            continue;
        }

        /* make sure the message is from kernel */
        if(nl_msgHdr->nlmsg_pid !=0)
        {
            return 0;
        }

        switch (nl_msgHdr->nlmsg_type)
        {
            case RTM_NEWLINK:
                ifi = NLMSG_DATA (nl_msgHdr);
                cmsLog_debug("ifi_family:%d,type:%d,index:%d,flags:%d,changes:%d",
                    ifi->ifi_family,ifi->ifi_type,ifi->ifi_index,ifi->ifi_flags,ifi->ifi_change);

                len = nl_msgHdr->nlmsg_len - NLMSG_LENGTH (sizeof (struct ifinfomsg));

                if (len < 0)
                    return FALSE;

                /* if it is not from the bridge we don't care */
                if ( AF_BRIDGE != ifi->ifi_family )
                    continue;

                /* If the message does not include the interface name or port state  then ignore */
                memset (tb, 0, sizeof tb);
                rta = IFLA_RTA(ifi);
                while( RTA_OK(rta, len) )
                {
                    if (rta->rta_type <= IFLA_MAX)
                        tb[rta->rta_type] = rta;
                    rta = RTA_NEXT(rta,len);
                }

                if ((tb[IFLA_IFNAME] == NULL) ||(tb[IFLA_PROTINFO] == NULL) )
                    continue;          

                name = (char *)RTA_DATA(tb[IFLA_IFNAME]);
                state = *(U8 *)RTA_DATA(tb[IFLA_PROTINFO]);

                //assume LAN interface name in format of ethxxxx
                for(port=0;port<UniCfgDbGetActivePortCount();port ++)
                {
                    if(! strncmp(uniRealIfname[port],name,strlen(uniRealIfname[port])))
                        break;
                }

                if(port >= UniCfgDbGetActivePortCount())
                {
                    cmsLog_debug("Unsupported interface:%s",name);
                }
                else
                {
                    NL_NEW_LINK_HANDLE(state, port);
                }
                break;
				
            default:
                cmsLog_debug("ingore nlmsg 0x%x", nl_msgHdr->nlmsg_type);
                break;
        }
    }

   return TRUE;
}


BOOL TkDataProcessHandle( TkMsgBuff *pSrcMsg, TkMsgBuff *pResMsg, U16 len )
{   
    U8 link;

    if ((NULL == pSrcMsg) || (NULL == pResMsg))
    {
        return FALSE;
    }
    
    OamFrame *pOamFrame = (OamFrame *)pSrcMsg->buff;

    RxFrame = (EthernetFrame *)pSrcMsg->buff;
    TxFrame = (EthernetFrame *)pResMsg->buff;
	
#ifdef DESKTOP_LINUX
    printf("\nReceived oam from desktop PC\n", len);
    DumpEthFrame(RxFrame, len);
#endif

    if ( EthertypeOam == ntohs(pOamFrame->EthHeader.type) &&
             ( 0x3 == (pOamFrame->Data.OamHead.subtype)))
    {
        link = GetSourceForFlag(ntohs(pOamFrame->Data.OamHead.flags));
        rxFrameLength = len;
                
        OamProcess(link, (U8 *)(&(pOamFrame->Data.OamHead)));
    }
    
#ifdef CLOCK_TRANSPORT
    if (EthertypeOam == ntohs(pOamFrame->EthHeader.type) &&
        (Oam8021asProtocolSubtype == (pOamFrame->Data.OamHead.subtype)))
    {
        IeeeOui const  Clk8021AsOui = {0x00, 0x80, 0xC2};
        IeeeOui  *pOui = (IeeeOui  *)(&(pOamFrame->Data.OamHead.flags));    

        rxFrameLength = len;
         if (memcmp (pOui, &Clk8021AsOui, sizeof (IeeeOui)) == 0)
            Oam8021MsgRcv((U8 *)pSrcMsg->buff);
    }
#endif

    return TRUE;
}

//end of TkMsgProcess.c
