/*
 *
 * Copyright (c) 2003-2012  Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
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
 *
 * $Id: wanppp.h,v 1.11.32.2 2003/10/31 21:31:35 mthawani Exp $
 */

#ifndef _wanppp_h_
#define _wanppp_h_

/*
  Definitions and declarations to support the WAN IPConnection service.
*/

#define VAR_ConnectionType		0
#define VAR_PossibleConnectionTypes		1
#define VAR_ConnectionStatus		2
#define VAR_Uptime		3
#define VAR_UpstreamMaxBitRate		4
#define VAR_DownstreamMaxBitRate	5
#define VAR_LastConnectionError		6
#define VAR_RSIPAvailable		7
#define VAR_NATEnabled		8
#define VAR_ExternalIPAddress		9
#define VAR_PortMappingNumberOfEntries		10
#define VAR_PortMappingEnabled		11
#define VAR_PortMappingLeaseDuration		12
#define VAR_RemoteHost		13
#define VAR_ExternalPort		14
#define VAR_InternalPort		15
#define VAR_PortMappingProtocol		16
#define VAR_InternalClient		17
#define VAR_PortMappingDescription		18


typedef enum { 
    PPP_UNCONFIGURED, 
    PPP_CONNECTING, 
    PPP_AUTHENTICATING,
    PPP_CONNECTED, 
    PPP_PENDINGDISCONNECT, 
    PPP_DISCONNECTING,
    PPP_DISCONNECTED	  
} ppp_conn_t;


#define CONNECTION_TIMEOUT   20

struct WANPPPConnectionData {
    ppp_conn_t	connection_status;
    int		connection_timeout;  /* timeout in seconds */
    time_t    connected_time;
    struct in_addr external_ipaddr;
    int igd_generation;
    int nportmappings;
    int upstrmaxbitrate;
    int dnstrmaxbitrate;
    timer_t eventhandle;
};

typedef struct WANPPPConnectionData WANPPPConnectionData, *PWANPPPConnectionData;

#endif // _wanppp_h_
