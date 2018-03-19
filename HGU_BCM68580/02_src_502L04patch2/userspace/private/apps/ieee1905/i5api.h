/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2013:proprietary:standard
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
 * $Change: 160447 $
 ***********************************************************************/

#ifndef _I5API_H_
#define _I5API_H_

#define I5_MESSAGE_BACKLOG 3
#define I5_SSID_MAX_LENGTH 32
#define I5_PASSWORD_MAX_LENGTH 32
#define I5_DEVICE_FRIENDLY_NAME_LEN 32
#define I5_MAX_IFNAME 16

/* ieee1905 api messages */
typedef enum t_i5_api_cmd_name
{
  I5_API_CMD_NONE = 0,
  I5_API_CMD_RETRIEVE_DM,
  I5_API_CMD_TRACE,
  I5_API_CMD_WLCFG,
  I5_API_CMD_PLC,
  I5_API_CMD_FLOWSHOW,
  I5_API_CMD_STOP,
  I5_API_CMD_START,
  I5_API_CMD_LINKUPDATE,
  I5_API_CMD_JSON_LEG,
  I5_API_CMD_PUSH_BUTTON,
  I5_API_CMD_SHOW_AL_MAC,
  I5_API_CMD_SEND_MESSAGE,
  I5_API_CMD_SET_LQ_INTERVAL,
  I5_API_CMD_SEND_BYTES,
  I5_API_CMD_SET_WIFI_PASS_SSID,
  I5_API_CMD_SET_WIFI_OVERRIDE_BW,
  I5_API_CMD_SET_WIFI_RELEASE_BW,
  I5_API_CMD_SET_WIFI_BOUNCE_BW,
  I5_API_CMD_SET_PLC_PASS_NMK,
  I5_API_CMD_SET_PLC_OVERRIDE_BW,
  I5_API_CMD_SET_PLC_RELEASE_BW,
  I5_API_CMD_SET_PLC_BOUNCE_BW,
  I5_API_CMD_SET_MOCA_PASS,
  I5_API_CMD_SHOW_MSGS,
  I5_API_CMD_SHOW_SOCKETS,
  I5_API_CMD_TRACE_TIME,
  I5_API_CMD_GET_CONFIG,
  I5_API_CMD_SET_CONFIG,
} t_I5_API_CMD_NAME;

typedef struct t_i5_api_msg
{
  t_I5_API_CMD_NAME  cmd;
  unsigned int       len;
} t_I5_API_MSG;

typedef struct t_i5_api_trace_msg
{
  int                module_id;
  int                depth;
  unsigned int       ifindex;
  unsigned char      interfaceMac[6];
} t_I5_API_TRACE_MSG;

typedef struct t_i5_api_wlcfg_msg
{
  char               ifname[I5_MAX_IFNAME];
  int                subcmd;
  int                status;
} t_I5_API_WLCFG_MSG;

typedef enum t_i5_api_plc_subcmd
{
  I5_API_PLC_UKE_START     = 0,
  I5_API_PLC_UKE_RANDOMIZE,
} t_I5_API_PLC_SUBCMD;

typedef struct t_i5_api_plc_msg
{
  t_I5_API_PLC_SUBCMD subcmd;
} t_I5_API_PLC_MSG;

typedef enum t_i5_api_json_leg_subcmd
{
  I5_API_JSON_LEG_OFF     = 0,
  I5_API_JSON_LEG_ON,
} t_I5_API_JSON_LEG_SUBCMD;

typedef struct t_i5_api_json_leg_msg
{
  t_I5_API_JSON_LEG_SUBCMD subcmd;
} t_I5_API_JSON_LEG_MSG;

typedef struct t_i5_api_golden_node_send_msg
{
  unsigned char       macAddr[6];
  unsigned int        messageId;   
} t_I5_API_GOLDEN_NODE_SEND_MSG;

typedef struct t_i5_api_link_metric_interval
{
  unsigned int        intervalMsec;
} t_I5_API_LINK_METRIC_INTERVAL;

typedef struct t_i5_api_golden_node_send_msg_bytes
{
  unsigned char       macAddr[6];
  unsigned char       message[1];   
} t_I5_API_GOLDEN_NODE_SEND_MSG_BYTES;

typedef struct t_i5_api_password_ssid_msg
{
  unsigned char       password[I5_PASSWORD_MAX_LENGTH+1];
  unsigned char       ssid[I5_SSID_MAX_LENGTH+1];
  char                ifname[I5_MAX_IFNAME];
} t_I5_API_PASSWORD_SSID_MSG;

typedef struct t_i5_api_override_bw_msg
{
  unsigned char       overrideCount;
  unsigned int        availBwMbps;
  unsigned int        macThroughBwMbps;  
} t_I5_API_OVERRIDE_BW_MSG;

typedef enum t_i5_api_config_subcmd
{
  I5_API_CONFIG_BASE     = 0,

  /* the following commands are applicable
     when DMP_DEVICE2_IEEE1905BASELINE_1 is defined */
  I5_API_CONFIG_SET_NETWORK_TOPOLOGY,
  I5_API_CONFIG_GET_LINK_METRICS,  
} t_I5_API_CONFIG_SUBCMD;

typedef struct 
{
    t_I5_API_CONFIG_SUBCMD  subcmd;
    unsigned int            isEnabled;
    char                    deviceFriendlyName[I5_DEVICE_FRIENDLY_NAME_LEN];
    unsigned int            isRegistrar;
    unsigned int            apFreqBand24En;
    unsigned int            apFreqBand5En;
} t_I5_API_CONFIG_BASE;

#if defined(BRCM_CMS_BUILD) && defined(DMP_DEVICE2_IEEE1905BASELINE_1)
typedef struct 
{
    t_I5_API_CONFIG_SUBCMD  subcmd;
    unsigned int            isEnabled;
} t_I5_API_CONFIG_SET_NETWORK_TOPOLOGY;

typedef struct 
{
    t_I5_API_CONFIG_SUBCMD  subcmd;
    unsigned char           ieee1905Id[6];
    unsigned char           remoteInterfaceId[6];
} t_I5_API_CONFIG_GET_LINK_METRICS;

typedef struct 
{
    unsigned int    packetErrors;
    unsigned int    packetErrorsReceived;
    unsigned int    transmittedPackets;
    unsigned int    packetsReceived;
    unsigned int    MacThroughputCapacity;
    unsigned int    linkAvailability;
    unsigned int    phyRate;
    unsigned int    rssi;
} t_I5_API_CONFIG_GET_LINK_METRICS_REPLY;
#endif

/* create session to ieee1905 daemon -- return socket */
int i5apiOpen(void);

/* prepend header to user cmd/data and send message -- return amount sent */
int i5apiSendMessage(int sd, int cmd, void *data, size_t datalen);

/* wait until response ready */
int i5apiWait(int sd, int waitsec);

/* receive response -- returns -1 on error, else payload length */
int i5apiRecvResponse(int sd, void **data, size_t datalen);

/* close session to ieee1905 daemon */
int i5apiClose(int sd);

/* perform transaction -- return -1 on error, else length of reply */
int i5apiTransaction(int cmd, void *reqdata, size_t reqlen, void **repdata, size_t replen);

#endif /* _I5API_H_ */
