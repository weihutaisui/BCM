/***********************************************************************
 *
 *  Copyright (c) 2007-2015  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:DUAL/GPL:standard

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
 *
 ************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <netdb.h>
#include <resolv.h>
#include <asm/types.h>
#include <linux/pkt_sched.h>
#include <time.h>
#include <sys/time.h>
#include <endian.h>
#include <errno.h>

#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/xfrm.h>

#include "libnetlink.h"
#include "inc/libnetlinkApi.h"

#define	MAX_IPSEC_KEY_SIZE		256

static struct rtnl_handle	gRtnlHandle;				/**< NETLINK Socket Handle */
static struct sockaddr_un gLibnetlinkSocketAddr;
static int gLibnetlinkSocket;

eLibnetlinkStatus_t libnetlinkAppInit(void);
eLibnetlinkStatus_t libnetlinkAppDeinit(void);
void processLibnetlinkMsg(libnetlinkAppMsg_t *pLibnetlinkMsg);

eLibnetlinkStatus_t libnetlinkAddSA(libnetlinkAppMsg_t *pLibnetlinkMsg);
eLibnetlinkStatus_t libnetlinkDelSA(libnetlinkAppMsg_t *pLibnetlinkMsg);
eLibnetlinkStatus_t libnetlinkAddSP(libnetlinkAppMsg_t *pLibnetlinkMsg);
eLibnetlinkStatus_t libnetlinkDelSP(libnetlinkAppMsg_t *pLibnetlinkMsg);

eLibnetlinkStatus_t libnetlinkAppInit(void)
{
   int rc = 0;

   gLibnetlinkSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
   if (gLibnetlinkSocket == -1)
   {
      printf("%s, socket failed\n", __FUNCTION__);
      return LIBNETLINK_ERROR; 
   }

   if ((rc = unlink(LIBNETLINK_SERVER_SOCK_PATH)) < 0)
   {
      printf("%s, unlink failed\n", __FUNCTION__);
   }

   memset(&gLibnetlinkSocketAddr, 0, sizeof (gLibnetlinkSocketAddr));
   gLibnetlinkSocketAddr.sun_family = AF_UNIX;  /* local is declared before socket() ^ */
   strcpy(gLibnetlinkSocketAddr.sun_path, LIBNETLINK_SERVER_SOCK_PATH);

   if ((rc = bind(gLibnetlinkSocket, (struct sockaddr *) &gLibnetlinkSocketAddr, sizeof(struct sockaddr_un))) == -1)
   {
      printf("%s, bind failed, error %s\n", __FUNCTION__, strerror(errno));
      close(gLibnetlinkSocket);
      return LIBNETLINK_ERROR;
   }

   printf("%s: success\n", __FUNCTION__);
   return LIBNETLINK_SUCCESS;
}

eLibnetlinkStatus_t libnetlinkAppDeinit(void)
{
   close(gLibnetlinkSocket);
   return LIBNETLINK_SUCCESS;
}

void processLibnetlinkMsg(libnetlinkAppMsg_t *pLibnetlinkMsg)
{
   unsigned int bytesSent = 0;
   struct sockaddr_un imsdalSocketAddr;

   imsdalSocketAddr.sun_family = AF_UNIX;
   strcpy(imsdalSocketAddr.sun_path, LIBNETLINK_CLIENT_SOCK_PATH);

   /* At this point, we know we have received correct number of bytes from IMSD-AL but still double check */
   if (pLibnetlinkMsg == NULL)
   {
      return;
   }

   switch (pLibnetlinkMsg->msgType)
   {
      case LIBNETLINK_OPEN:
      {
         if (rtnl_open_byproto(&(gRtnlHandle), 0, NETLINK_XFRM) < 0)
         {
            pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_ERROR;
            bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
         }
         else
         {
            pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_OK;
            bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
         }
         break;
      }
      case LIBNETLINK_CLOSE:
      {
         rtnl_close(&gRtnlHandle);
         pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_OK;
         bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
         break;
      }
      case LIBNETLINK_ADD_SA:
      {
         if	(libnetlinkAddSA(pLibnetlinkMsg) == LIBNETLINK_SUCCESS)
         {
            pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_OK;
            bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
         }
         else
         {
            pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_ERROR;
            bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
         }
         break;
      }
      case LIBNETLINK_DEL_SA:
      {
         if	(libnetlinkDelSA(pLibnetlinkMsg) == LIBNETLINK_SUCCESS)
         {
            pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_OK;
            bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
         }
         else
         {
            pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_ERROR;
            bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
         }
         break;
      }
      case LIBNETLINK_ADD_SP:
      {
         if	(libnetlinkAddSP(pLibnetlinkMsg) == LIBNETLINK_SUCCESS)
         {
            pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_OK;
            bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
         }
         else
         {
            pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_ERROR;
            bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
         }
         break;
      }
      case LIBNETLINK_DEL_SP:
      {
         if	(libnetlinkDelSP(pLibnetlinkMsg) == LIBNETLINK_SUCCESS)
         {
            pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_OK;
            bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
         }
         else
         {
            pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_ERROR;
            bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
         }
         break;
      } 
      case LIBNETLINK_STATUS_OK:
      case LIBNETLINK_STATUS_ERROR:
      case LIBNETLINK_STATUS_REQUEST:
         break;     
   }
   return;
}

eLibnetlinkStatus_t libnetlinkAddSA(libnetlinkAppMsg_t *pLibnetlinkMsg)
{
	BrcmIPSecSktReq_t req;
	char buffer[MAX_IPSEC_KEY_SIZE];
	struct xfrm_algo *pXfrmAlgoInfo = (struct xfrm_algo*)buffer;
	struct inet_prefix stDestAddr;
	struct inet_prefix stSrcAddr;
	int iiksize_offset = 0;			// will be used in case DES algorithm case
	int icksize_offset = 0;			// 	''		''		''
	int sizeofIp = 0;
	int ret = 0;

	if(inet_pton( pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.usFamily, (const char*)pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.destnIP, (stDestAddr.data) ) <=0)
	{
	    printf("%s: %d, error with dest address conversion\n", __FUNCTION__, __LINE__);
		return LIBNETLINK_INVALIDPARMS;
	}

	if(ret = inet_pton( pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.usFamily, (const char*)pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.srcIP, (stSrcAddr.data) ) <= 0)
	{
	    printf("%s: %d, error with src address conversion. Family %d, src %s, ret %d \n", __FUNCTION__, __LINE__, pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.usFamily, 
	           pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.srcIP, ret);
		return LIBNETLINK_INVALIDPARMS;
	}

	sizeofIp = (AF_INET == pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.usFamily) ? sizeof(unsigned int) : sizeof(req.xsinfo.saddr);

	// fill request header fields
	memset(&req, 0, sizeof(req));
	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(req.xsinfo));
	req.n.nlmsg_flags = NLM_F_REQUEST;
	req.n.nlmsg_type = XFRM_MSG_NEWSA;
	req.xsinfo.reqid = 0;
	req.xsinfo.replay_window = 0;
	req.xsinfo.stats.replay = 1;
	req.xsinfo.stats.integrity_failed = 1;
	req.xsinfo.stats.replay_window = 1;
	req.xsinfo.lft.soft_byte_limit = XFRM_INF;
	req.xsinfo.lft.hard_byte_limit = XFRM_INF;
	req.xsinfo.lft.soft_packet_limit = XFRM_INF;
	req.xsinfo.lft.hard_packet_limit = XFRM_INF;

	req.xsinfo.mode = pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.ucMode; //XFRM_MODE_TRANSPORT;	//XFRM_MODE_TUNNEL;
	req.xsinfo.family = pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.usFamily;		//AF_INET or 	AF_INET6
	memcpy(&req.xsinfo.saddr, (stSrcAddr.data), sizeofIp);
	memcpy(&req.xsinfo.id.daddr, (stDestAddr.data), sizeofIp);
	req.xsinfo.id.proto = pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.ucProto;		// for esp
	req.xsinfo.id.spi = htonl(pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.uiSpi);

	// fill selector information
	req.xsinfo.sel.proto = 0;	// for upd -->17 tcp --> 6 all --> 0
	req.xsinfo.sel.family = pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.usFamily;
	memcpy( &req.xsinfo.sel.daddr, (stDestAddr.data), sizeofIp );
	req.xsinfo.sel.prefixlen_d = sizeofIp * BRCM_IPSEC_BITS;
	req.xsinfo.sel.dport = htons(pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.usDestnPort);
	req.xsinfo.sel.dport_mask = ~((__u16)0);
	memcpy( &req.xsinfo.sel.saddr, (stSrcAddr.data), sizeofIp );
	req.xsinfo.sel.prefixlen_s = sizeofIp * BRCM_IPSEC_BITS;
	req.xsinfo.sel.sport = htons(pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSa.usSrcPort);
	req.xsinfo.sel.sport_mask = ~((__u16)0);

	// fill Encryption algorithim information
	if( ( pLibnetlinkMsg->ipSecKeys.CK[0] != '\0' ) && ( pLibnetlinkMsg->ipSecKeys.encrypt[0] != '\0') )
	{
		memset( pXfrmAlgoInfo, 0, MAX_IPSEC_KEY_SIZE );
		pXfrmAlgoInfo->alg_key_len = pLibnetlinkMsg->ipSecKeys.uCKLength * BRCM_IPSEC_BITS;

		if (0 == strcmp( (const char*)pLibnetlinkMsg->ipSecKeys.encrypt, "des-ede3-cbc" ))
		{
			strcpy( pXfrmAlgoInfo->alg_name, "cbc(des3_ede)");

			// According to 3GPP TS 33.203 Annex I: Key expansion
			memcpy(&pXfrmAlgoInfo->alg_key[0], pLibnetlinkMsg->ipSecKeys.CK, pLibnetlinkMsg->ipSecKeys.uCKLength);
			memcpy(&pXfrmAlgoInfo->alg_key[pLibnetlinkMsg->ipSecKeys.uCKLength], pLibnetlinkMsg->ipSecKeys.CK, BRCM_IPSEC_DES_KEY_EXTRA_LEN);
			pXfrmAlgoInfo->alg_key_len += BRCM_IPSEC_DES_KEY_EXTRA_LEN * BRCM_IPSEC_BITS;
			icksize_offset = BRCM_IPSEC_DES_KEY_EXTRA_LEN;
		}
		else if (0 == strcmp((const char*)pLibnetlinkMsg->ipSecKeys.encrypt, "null" ))
		{
			pXfrmAlgoInfo->alg_key_len = 0;
			strcpy( pXfrmAlgoInfo->alg_name, "cipher_null" );
			memcpy(pXfrmAlgoInfo->alg_key, pLibnetlinkMsg->ipSecKeys.CK, pLibnetlinkMsg->ipSecKeys.uCKLength);
		}
		else if (0 == strcmp( (const char*)pLibnetlinkMsg->ipSecKeys.encrypt, "aes-cbc" ))
		{
			strcpy( pXfrmAlgoInfo->alg_name, "aes" );
			memcpy(pXfrmAlgoInfo->alg_key, pLibnetlinkMsg->ipSecKeys.CK, pLibnetlinkMsg->ipSecKeys.uCKLength);
		}
		else
		{
		    printf("%s: %d, unsupported parameter\n", __FUNCTION__, __LINE__);
			return LIBNETLINK_NOTSUPP;
		}

		//key_hex_dump("CK",pLibnetlinkMsg->ipSecKeys.uCKLength,pXfrmAlgoInfo->alg_key);

		if(addattr_l(&req.n, sizeof(req.buf), XFRMA_ALG_CRYPT, pXfrmAlgoInfo,
											sizeof (struct xfrm_algo) + pLibnetlinkMsg->ipSecKeys.uCKLength + icksize_offset) < 0)
		{
		    printf("%s: %d, internal error\n", __FUNCTION__, __LINE__);
			return LIBNETLINK_INTERR;
		}
	}

	// fill integrity algorithim information
	if( ( pLibnetlinkMsg->ipSecKeys.IK[0] != '\0' ) && ( pLibnetlinkMsg->ipSecKeys.auth[0] != '\0' ) )
	{
		memset (pXfrmAlgoInfo, 0, MAX_IPSEC_KEY_SIZE);

		pXfrmAlgoInfo->alg_key_len = pLibnetlinkMsg->ipSecKeys.uIKLength * BRCM_IPSEC_BITS;
		if( 0 == strcmp( (const char*)pLibnetlinkMsg->ipSecKeys.auth, "hmac-sha-1-96" ) )
		{
			strcpy( pXfrmAlgoInfo->alg_name, "hmac(sha1)" );
			memcpy( pXfrmAlgoInfo->alg_key, pLibnetlinkMsg->ipSecKeys.IK, pLibnetlinkMsg->ipSecKeys.uIKLength );
		}
		else if (0 == strcmp( (const char*)pLibnetlinkMsg->ipSecKeys.auth, "hmac-md5-96" ))
		{
			pXfrmAlgoInfo->alg_key_len += BRCM_IPSEC_BITS * BRCM_IPSEC_MD5_KEY_EXTRA_LEN ;
			iiksize_offset = BRCM_IPSEC_MD5_KEY_EXTRA_LEN ;
			strcpy(pXfrmAlgoInfo->alg_name, "hmac(md5)" );
			memcpy( pXfrmAlgoInfo->alg_key, pLibnetlinkMsg->ipSecKeys.IK, pLibnetlinkMsg->ipSecKeys.uIKLength );
		}
		else
		{
		    printf("%s: %d, unsupported parameter\n", __FUNCTION__, __LINE__);
			return LIBNETLINK_NOTSUPP;
		}

		//key_hex_dump("IK",pLibnetlinkMsg->ipSecKeys.uIKLength,pXfrmAlgoInfo->alg_key);

		if (addattr_l( &req.n, sizeof(req.buf), XFRMA_ALG_AUTH, (void *)pXfrmAlgoInfo,
												sizeof(struct xfrm_algo) + pLibnetlinkMsg->ipSecKeys.uIKLength + iiksize_offset) < 0)
		{
		    printf("%s: %d, internal error\n", __FUNCTION__, __LINE__);
			return LIBNETLINK_INTERR;
		}
	}

	ret = rtnl_talk((struct rtnl_handle *)&gRtnlHandle, &req.n, 0, 0, 0, 0, NULL);
	if( ret < LIBNETLINK_SUCCESS )
	{
	    printf("%s: %d, internal error with rtnl_talk()\n", __FUNCTION__, __LINE__);
		return LIBNETLINK_INTERR;
	}
	printf("%s: completed successfully\n", __FUNCTION__);
    return LIBNETLINK_SUCCESS;
}

eLibnetlinkStatus_t libnetlinkDelSA(libnetlinkAppMsg_t *pLibnetlinkMsg)
{
	BrcmIPSecDeleteSASktReq_t	req;		// structure type wll be sent over NETLINK Socket
	xfrm_address_t	stSrcAddr;
	struct inet_prefix 	stLocalAddr;
	struct inet_prefix 	stRemoteAddr;			// user for temp conversion
	int sizeofIp = 0;
	int ret = 0;

	if(inet_pton( pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSa.usFamily, (const char*)pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSa.srcIP, (stLocalAddr.data) ) <= 0)
	{
		return LIBNETLINK_INVALIDPARMS;
	}
	if(inet_pton( pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSa.usFamily, (const char*)pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSa.destnIP, (stRemoteAddr.data) ) <= 0)
	{
		return LIBNETLINK_INVALIDPARMS;
	}
	sizeofIp = (AF_INET == pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSa.usFamily) ? sizeof(unsigned int) : sizeof(req.xsid.daddr);

	// fill request header parameters
	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(req.xsid));
	req.n.nlmsg_flags = NLM_F_REQUEST;
	req.n.nlmsg_type = XFRM_MSG_DELSA;;
	req.xsid.family = pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSa.usFamily;
	memcpy(&req.xsid.daddr, (stRemoteAddr.data), sizeofIp);
	req.xsid.spi = htonl(pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSa.uiSpi);
	req.xsid.proto = pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSa.ucProto;	// for ESP
	memcpy(&stSrcAddr, &(stLocalAddr.data), sizeofIp);

	addattr_l(&req.n, sizeof(req.buf), XFRMA_SRCADDR,(void *)&stSrcAddr, sizeof(stSrcAddr));

	// send event over socket
	ret = rtnl_talk( (struct rtnl_handle *)&gRtnlHandle, &req.n, 0, 0,0, 0, NULL);
	if( ret < LIBNETLINK_SUCCESS )
	{
		return LIBNETLINK_INTERR;
	}

    return LIBNETLINK_SUCCESS;
}

eLibnetlinkStatus_t libnetlinkAddSP(libnetlinkAppMsg_t *pLibnetlinkMsg)
{
	char tmpls_buf[XFRM_TMPLS_BUF_SIZE];
	BrcmIPSecAddSPSktReq_t	sp_req;
	struct xfrm_user_tmpl *tmpl;
	struct inet_prefix 	stDestAddr;
	struct inet_prefix 	stSrcAddr;
	int sizeofIp = 0;
	int	ret = 0;

	if(inet_pton(pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.usFamily, (const char*)pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.srcIP, (stSrcAddr.data)) <=0 )
	{
		return LIBNETLINK_INVALIDPARMS;
	}

	if(inet_pton(pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.usFamily, (const char*)pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.destnIP, (stDestAddr.data)) <=0 )
	{
		return LIBNETLINK_INVALIDPARMS;
	}

	sizeofIp = (AF_INET == pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.usFamily) ? sizeof(unsigned int) : sizeof(sp_req.xpinfo.sel.saddr);

	memset( &sp_req, 0, sizeof(sp_req));
	sp_req.n.nlmsg_len = NLMSG_LENGTH(sizeof(sp_req.xpinfo));
	sp_req.n.nlmsg_flags = NLM_F_REQUEST;//flags;
	sp_req.n.nlmsg_type = XFRM_MSG_NEWPOLICY;
	sp_req.xpinfo.lft.soft_byte_limit = XFRM_INF;
	sp_req.xpinfo.lft.hard_byte_limit = XFRM_INF;
	sp_req.xpinfo.lft.soft_packet_limit = XFRM_INF;
	sp_req.xpinfo.lft.hard_packet_limit = XFRM_INF;

	// set policy direction
	sp_req.xpinfo.dir = pLibnetlinkMsg->ucDir;
	sp_req.xpinfo.index = 0;
	sp_req.xpinfo.sel.proto = 0;//UDP --> 17 TCP --> 6 All --> 0;
	sp_req.xpinfo.action = XFRM_POLICY_ALLOW;
	sp_req.xpinfo.sel.user = 0;
	sp_req.xpinfo.sel.family = pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.usFamily;
	sp_req.xpinfo.sel.prefixlen_d = sizeofIp * BRCM_IPSEC_BITS;
	memcpy( &sp_req.xpinfo.sel.daddr, (stDestAddr.data), sizeofIp );
	sp_req.xpinfo.sel.dport = htons (pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.usDestnPort);
	sp_req.xpinfo.sel.dport_mask = ~((__u16)0);
	sp_req.xpinfo.sel.prefixlen_s = sizeofIp * BRCM_IPSEC_BITS;
	memcpy( &sp_req.xpinfo.sel.saddr, (stSrcAddr.data), sizeofIp );
	sp_req.xpinfo.sel.sport = htons(pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.usSrcPort);
	sp_req.xpinfo.sel.sport_mask = ~((__u16)0);

	{
		tmpl = (struct xfrm_user_tmpl *)((char *)tmpls_buf);

		tmpl->family = pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.usFamily;
		tmpl->aalgos = (~(__u32)0);
		tmpl->ealgos = (~(__u32)0);
		tmpl->calgos = (~(__u32)0);
		memcpy(&tmpl->id.daddr, stDestAddr.data, sizeofIp);
		tmpl->id.proto = pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.ucProto;
		tmpl->id.spi = htonl(pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.uiSpi);
		tmpl->mode = pLibnetlinkMsg->libnetlinkMsgContent.ipSecAddSp.ucMode;
		tmpl->reqid = 0;
		memcpy(&tmpl->saddr, stSrcAddr.data, sizeofIp);

		addattr_l(&sp_req.n, sizeof(sp_req), XFRMA_TMPL,(void *)tmpls_buf, sizeof(*tmpl));
	}

	ret = rtnl_talk( (struct rtnl_handle *)&gRtnlHandle, &sp_req.n, 0, 0, 0, 0, NULL);
	if( ret < LIBNETLINK_SUCCESS )
	{
		return LIBNETLINK_INTERR;
	}

   return LIBNETLINK_SUCCESS;
}

eLibnetlinkStatus_t libnetlinkDelSP(libnetlinkAppMsg_t *pLibnetlinkMsg)
{

	struct inet_prefix 	stDestAddr;
	struct inet_prefix 	stSrcAddr;
	BrcmRtnlDeleteSP_t 	req;
	int	sizeofIp = 0;
	int	ret = 0;

	if(inet_pton(pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSp.usFamily, (const char*)pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSp.destnIP, stDestAddr.data) <= 0)
	{
		return LIBNETLINK_INVALIDPARMS;
	}

	if(inet_pton(pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSp.usFamily, (const char*)pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSp.srcIP, stSrcAddr.data) <= 0)
	{
		return LIBNETLINK_INVALIDPARMS;
	}
	sizeofIp = ( AF_INET == pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSp.usFamily ) ? sizeof(unsigned int): sizeof(req.xpid.sel.saddr);

	memset( &req, 0, sizeof(req) );
	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(req.xpid));
	req.n.nlmsg_flags = NLM_F_REQUEST;
	req.n.nlmsg_type = XFRM_MSG_DELPOLICY;//  XFRM_MSG_DELPOLICY : XFRM_MSG_GETPOLICY;
	req.xpid.dir = pLibnetlinkMsg->ucDir;
	req.xpid.sel.family = pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSp.usFamily;
	req.xpid.sel.proto = 0;		// fill selector information
	req.xpid.sel.family = pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSp.usFamily;
	req.xpid.sel.prefixlen_d = sizeofIp * BRCM_IPSEC_BITS;
	memcpy( &req.xpid.sel.daddr, &(stDestAddr.data), sizeofIp );
	req.xpid.sel.dport = htons (pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSp.usDestnPort);
	req.xpid.sel.dport_mask = ~((__u16)0);
	req.xpid.sel.prefixlen_s = sizeofIp * BRCM_IPSEC_BITS;
	memcpy( &req.xpid.sel.saddr, &(stSrcAddr.data),sizeofIp );
	req.xpid.sel.sport = htons(pLibnetlinkMsg->libnetlinkMsgContent.ipSecDelSp.usSrcPort);
	req.xpid.sel.sport_mask = ~((__u16)0);

	ret = rtnl_talk( (struct rtnl_handle *)&gRtnlHandle, &req.n, 0, 0, 0, 0, NULL);
	if( ret < LIBNETLINK_SUCCESS )
	{
		return LIBNETLINK_INTERR;
	}

   return LIBNETLINK_SUCCESS;
}

int main(int argc, char *argv[])
{
   unsigned int bytesRead = 0;
   unsigned int bytesSent = 0;
   struct sockaddr_un imsdalSocketAddr;

   imsdalSocketAddr.sun_family = AF_UNIX;
   strcpy(imsdalSocketAddr.sun_path, LIBNETLINK_CLIENT_SOCK_PATH);

   libnetlinkAppMsg_t *pLibnetlinkMsg = (libnetlinkAppMsg_t *)malloc(sizeof(libnetlinkAppMsg_t));
   memset(pLibnetlinkMsg, 0, sizeof(libnetlinkAppMsg_t));

   printf("Starting libnetlink...\n");

   if (libnetlinkAppInit() != LIBNETLINK_SUCCESS)
   {
      return -1;
   }

   /* Initial handshake to let the calling application know that we are awake */
   bytesRead = recv(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0);
   if (bytesRead != sizeof(libnetlinkAppMsg_t) || pLibnetlinkMsg->msgType != LIBNETLINK_STATUS_REQUEST)
   {   
      printf("Libnetlink error: incorrect bytesRead (%d), msg type (%d) \n", bytesRead, pLibnetlinkMsg->msgType);
      return -1;
   } 
   else
   {
      pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_OK;
	  printf("Libnetlink received status request from IMSDAL, sending OK back to IMSDAL\n");
      bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
      printf( "Libnetlink->IMSDAL: sendto %d bytes, errno %s\n", bytesSent, strerror(errno) );
   }

   while(1)
   {
      bytesRead = recv(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0);
      if (bytesRead == sizeof(libnetlinkAppMsg_t))
      { 
         processLibnetlinkMsg(pLibnetlinkMsg);
      }
      else
      {
         printf("Libnetlink: msg size incorrect, errno %s\n", strerror(errno)); 
         pLibnetlinkMsg->msgType = LIBNETLINK_STATUS_ERROR;
         bytesSent = sendto(gLibnetlinkSocket, pLibnetlinkMsg, sizeof(libnetlinkAppMsg_t), 0, (struct sockaddr *)&imsdalSocketAddr, sizeof(struct sockaddr_un));
      }
   }

   free(pLibnetlinkMsg);

   if (libnetlinkAppDeinit() != LIBNETLINK_SUCCESS)
   {
      return -1;
   }

   return 0;
}
