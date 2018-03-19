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
 * $Id: igd.h,v 1.7.2.2 2003/10/31 21:31:35 mthawani Exp $
 */

#ifndef _igd_h_
#define _igd_h_

#include "upnp_osl.h"

typedef struct _if_stats {
    unsigned long rx_packets;	/* total packets received       */
    unsigned long tx_packets;	/* total packets transmitted    */
    unsigned long rx_bytes;	/* total bytes received         */
    unsigned long tx_bytes;	/* total bytes transmitted      */
    unsigned long rx_errors;	/* bad packets received         */
    unsigned long tx_errors;	/* packet transmit problems     */
    unsigned long rx_dropped;	/* no space in linux buffers    */
    unsigned long tx_dropped;	/* no space available in linux  */
    unsigned long rx_multicast;	/* multicast packets received   */
    unsigned long rx_compressed;
    unsigned long tx_compressed;
    unsigned long collisions;

    /* detailed rx_errors: */
    unsigned long rx_length_errors;
    unsigned long rx_over_errors;	/* receiver ring buff overflow  */
    unsigned long rx_crc_errors;	/* recved pkt with crc error    */
    unsigned long rx_frame_errors;	/* recv'd frame alignment error */
    unsigned long rx_fifo_errors;	/* recv'r fifo overrun          */
    unsigned long rx_missed_errors;	/* receiver missed packet     */
    /* detailed tx_errors */
    unsigned long tx_aborted_errors;
    unsigned long tx_carrier_errors;
    unsigned long tx_fifo_errors;
    unsigned long tx_heartbeat_errors;
    unsigned long tx_window_errors;

} if_stats_t;

typedef struct _WANDevicePrivateData {
    char ifname[CMS_IFNAME_LENGTH];
} WANDevicePrivateData, *PWANDevicePrivateData;

typedef struct _LANDevicePrivateData {
    char ifname[CMS_IFNAME_LENGTH];
} LANDevicePrivateData, *PLANDevicePrivateData;

typedef struct _WANCommonPrivateData {
    /* link status */
    osl_link_t if_up;

    /* interface speed (bits/sec) */
    unsigned long tx_bitrate;
    unsigned long rx_bitrate;

    /* network statistics, collected through osl_ifstats */
    if_stats_t stats;

    /* event handle for periodic update event,
       created in WANCommonInterfaceConfig_Init(). */
    timer_t eventhandle;

} WANCommonPrivateData, *PWANCommonPrivateData;

extern void osl_igd_disable();
extern void osl_igd_enable();
extern char *igd_pri_wan_var(char *prefix, int len, char *var);

#define SOAP_CONNECTIONNOTCONFIGURED	706 
#define SOAP_DISCONNECTINPROGRESS	707 
#define SOAP_INVALIDCONNECTIONTYPE	710 
#define SOAP_CONNECTIONALREADYTERMNATED	711 
#define SOAP_SPECIFIEDARRAYINDEXINVALID	713 
#define SOAP_NOSUCHENTRYINARRAY		714 
#define SOAP_CONFLICTINMAPPINGENTRY	718 
#define SOAP_ONLYPERMANENTLEASESSUPPORTED	725

#if defined(linux)
#if 0
/* Allow some time for the page to reload before killing ourselves */
static int
kill_after(pid_t pid, int sig, unsigned int after)
{
	if (fork() == 0) {
		sleep(after);
		return kill(pid, sig);
	}
	return 0;
}
#endif
////#define sys_restart() kill_after(1, SIGHUP, 3)
////#define sys_reboot() kill_after(1, SIGTERM, 3)

#endif /* linux */




/** Given the WAN intf name, return the associated instance numbers.
 *
 * @return CmsRet
 */
CmsRet getInstanceNumbersForWanIfname(const char *wan_ifname,
                                      int *wanDevInstanceNum,
                                      int *wanConnInstanceNum,
                                      int *pppipInstanceNum);

CmsRet getInstanceNumbersForWanIfname_igd(const char *wan_ifname,
                                      int *wanDevInstanceNum,
                                      int *wanConnInstanceNum,
                                      int *pppipInstanceNum);

CmsRet getInstanceNumbersForWanIfname_dev2(const char *wan_ifname,
                                      int *wanDevInstanceNum,
                                      int *wanConnInstanceNum,
                                      int *pppipInstanceNum);

#if defined(SUPPORT_DM_LEGACY98)
#define getInstanceNumbersForWanIfname(a, b, c, d)  getInstanceNumbersForWanIfname_igd((a), (b), (c), (d))
#elif defined(SUPPORT_DM_HYBRID)
#define getInstanceNumbersForWanIfname(a, b, c, d)  getInstanceNumbersForWanIfname_igd((a), (b), (c), (d))
#elif defined(SUPPORT_DM_PURE181)
#define getInstanceNumbersForWanIfname(a, b, c, d)  getInstanceNumbersForWanIfname_dev2((a), (b), (c), (d))
#elif defined(SUPPORT_DM_DETECT)
#define getInstanceNumbersForWanIfname(a, b, c, d)  (cmsMdm_isDataModelDevice2() ? \
                  getInstanceNumbersForWanIfname_dev2((a), (b), (c), (d)) : \
                  getInstanceNumbersForWanIfname_igd((a), (b), (c), (d)))
#endif




/* Update the variables associated with the WanCommonInterfaceConfig
 *
 */
void WANCommonInterface_UpdateVars(PService psvc);
void WANCommonInterface_UpdateVars_igd(PService psvc);
void WANCommonInterface_UpdateVars_dev2(PService psvc);

#if defined(SUPPORT_DM_LEGACY98)
#define WANCommonInterface_UpdateVars(a)  WANCommonInterface_UpdateVars_igd((a))
#elif defined(SUPPORT_DM_HYBRID)
#define WANCommonInterface_UpdateVars(a)  WANCommonInterface_UpdateVars_igd((a))
#elif defined(SUPPORT_DM_PURE181)
#define WANCommonInterface_UpdateVars(a)  WANCommonInterface_UpdateVars_dev2((a))
#elif defined(SUPPORT_DM_DETECT)
#define WANCommonInterface_UpdateVars(a)  (cmsMdm_isDataModelDevice2() ? \
                  WANCommonInterface_UpdateVars_dev2((a)) : \
                  WANCommonInterface_UpdateVars_igd((a)))
#endif




/** Update the variables associated with the WanIp or WanPpp connection.
 *
 */
void WANConnection_UpdateVars(struct Service *psvc, int isPpp);
void WANConnection_UpdateVars_igd(struct Service *psvc, int isPpp);
void WANConnection_UpdateVars_dev2(struct Service *psvc, int isPpp);

#if defined(SUPPORT_DM_LEGACY98)
#define WANConnection_UpdateVars(a, b)  WANConnection_UpdateVars_igd((a), (b))
#elif defined(SUPPORT_DM_HYBRID)
#define WANConnection_UpdateVars(a, b)  WANConnection_UpdateVars_igd((a), (b))
#elif defined(SUPPORT_DM_PURE181)
#define WANConnection_UpdateVars(a, b)  WANConnection_UpdateVars_dev2((a), (b))
#elif defined(SUPPORT_DM_DETECT)
#define WANConnection_UpdateVars(a, b)  (cmsMdm_isDataModelDevice2() ? \
                  WANConnection_UpdateVars_dev2((a), (b)) : \
                  WANConnection_UpdateVars_igd((a), (b)))
#endif




/** Get params from DeviceInfo object.
 *
 * @return errorInfo code
 */
int upnp_getDeviceInfo(void);
int upnp_getDeviceInfo_igd(void);
int upnp_getDeviceInfo_dev2(void);

#if defined(SUPPORT_DM_LEGACY98)
#define upnp_getDeviceInfo()               upnp_getDeviceInfo_igd()
#elif defined(SUPPORT_DM_HYBRID)
#define upnp_getDeviceInfo()               upnp_getDeviceInfo_igd()
#elif defined(SUPPORT_DM_PURE181)
#define upnp_getDeviceInfo()               upnp_getDeviceInfo_dev2()
#elif defined(SUPPORT_DM_DETECT)
#define upnp_getDeviceInfo()  (cmsMdm_isDataModelDevice2() ? \
                                       upnp_getDeviceInfo_dev2() : \
                                       upnp_getDeviceInfo_igd())
#endif




/** Set the WanIp or WanPpp connection type.  It is surprising that we
 *  allow UPnP to do this.  Maybe UPnP never calls this function.
 *
 *  @return errorInfo code
 */
int upnp_setConnectionType(UBOOL8 isPpp, const InstanceIdStack *iidStack,
                           const char *connectionType);
int upnp_setConnectionType_igd(UBOOL8 isPpp, const InstanceIdStack *iidStack,
                               const char *connectionType);
int upnp_setConnectionType_dev2(UBOOL8 isPpp, const InstanceIdStack *iidStack,
                               const char *connectionType);

#if defined(SUPPORT_DM_LEGACY98)
#define upnp_setConnectionType(a, b, c)  upnp_setConnectionType_igd((a), (b), (c))
#elif defined(SUPPORT_DM_HYBRID)
#define upnp_setConnectionType(a, b, c)  upnp_setConnectionType_igd((a), (b), (c))
#elif defined(SUPPORT_DM_PURE181)
#define upnp_setConnectionType(a, b, c)  upnp_setConnectionType_dev2((a), (b), (c))
#elif defined(SUPPORT_DM_DETECT)
#define upnp_setConnectionType(a, b, c)  (cmsMdm_isDataModelDevice2() ? \
                  upnp_setConnectionType_dev2((a), (b), (c)) : \
                  upnp_setConnectionType_igd((a), (b), (c)))
#endif




/** Update an existing or add a new port mapping entry under the WanIp or
 *  WanPpp connection object.
 *
 *  @return errorInfo code
 */
int upnp_updateOrAddPortMapping(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
              UBOOL8 portMappingEnabled, UINT32 leaseDuration,
              const char *protocol, UINT16 externalPort, UINT16 internalPort,
              const char *remoteHost, const char *internalClient, char *description);

int upnp_updateOrAddPortMapping_igd(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
              UBOOL8 portMappingEnabled, UINT32 leaseDuration,
              const char *protocol, UINT16 externalPort, UINT16 internalPort,
              const char *remoteHost, const char *internalClient, char *description);

int upnp_updateOrAddPortMapping_dev2(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
              UBOOL8 portMappingEnabled, UINT32 leaseDuration,
              const char *protocol, UINT16 externalPort, UINT16 internalPort,
              const char *remoteHost, const char *internalClient, char *description);

#if defined(SUPPORT_DM_LEGACY98)
#define upnp_updateOrAddPortMapping(a, b, c, d, e, f, g, h, i, j)  upnp_updateOrAddPortMapping_igd((a), (b), (c), (d), (e), (f), (g), (h), (i), (j))
#elif defined(SUPPORT_DM_HYBRID)
#define upnp_updateOrAddPortMapping(a, b, c, d, e, f, g, h, i, j)  upnp_updateOrAddPortMapping_igd((a), (b), (c), (d), (e), (f), (g), (h), (i), (j))
#elif defined(SUPPORT_DM_PURE181)
#define upnp_updateOrAddPortMapping(a, b, c, d, e, f, g, h, i, j)  upnp_updateOrAddPortMapping_dev2((a), (b), (c), (d), (e), (f), (g), (h), (i), (j))
#elif defined(SUPPORT_DM_DETECT)
#define upnp_updateOrAddPortMapping(a, b, c, d, e, f, g, h, i, j)  (cmsMdm_isDataModelDevice2() ? \
                  upnp_updateOrAddPortMapping_dev2((a), (b), (c), (d), (e), (f), (g), (h), (i), (j)) : \
                  upnp_updateOrAddPortMapping_igd((a), (b), (c), (d), (e), (f), (g), (h), (i), (j)))
#endif




/** Delete the specified portmapping entry.
 *
 * @return errorInfo code
 */
int upnp_deletePortMapping(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
            const char *protocol, UINT16 externalPort, const char *remoteHost,
            char *iaddr);

int upnp_deletePortMapping_igd(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
            const char *protocol, UINT16 externalPort, const char *remoteHost,
            char *iaddr);

int upnp_deletePortMapping_dev2(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
            const char *protocol, UINT16 externalPort, const char *remoteHost,
            char *iaddr);

#if defined(SUPPORT_DM_LEGACY98)
#define upnp_deletePortMapping(a, b, c, d, e, f)  upnp_deletePortMapping_igd((a), (b), (c), (d), (e), (f))
#elif defined(SUPPORT_DM_HYBRID)
#define upnp_deletePortMapping(a, b, c, d, e, f)  upnp_deletePortMapping_igd((a), (b), (c), (d), (e), (f))
#elif defined(SUPPORT_DM_PURE181)
#define upnp_deletePortMapping(a, b, c, d, e, f)  upnp_deletePortMapping_dev2((a), (b), (c), (d), (e), (f))
#elif defined(SUPPORT_DM_DETECT)
#define upnp_deletePortMapping(a, b, c, d, e, f)  (cmsMdm_isDataModelDevice2() ? \
                  upnp_deletePortMapping_dev2((a), (b), (c), (d), (e), (f)) : \
                  upnp_deletePortMapping_igd((a), (b), (c), (d), (e), (f)))
#endif




/** Get the portmapping by index
 *
 * @return errorInfo code
 */
int upnp_getGenericPortMapping(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
                               int portMappingIndex, PAction ac);
int upnp_getGenericPortMapping_igd(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
                                   int portMappingIndex, PAction ac);
int upnp_getGenericPortMapping_dev2(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
                                      int portMappingIndex, PAction ac);

#if defined(SUPPORT_DM_LEGACY98)
#define upnp_getGenericPortMapping(a, b, c, d)  upnp_getGenericPortMapping_igd((a), (b), (c), (d))
#elif defined(SUPPORT_DM_HYBRID)
#define upnp_getGenericPortMapping(a, b, c, d)  upnp_getGenericPortMapping_igd((a), (b), (c), (d))
#elif defined(SUPPORT_DM_PURE181)
#define upnp_getGenericPortMapping(a, b, c, d)  upnp_getGenericPortMapping_dev2((a), (b), (c), (d))
#elif defined(SUPPORT_DM_DETECT)
#define upnp_getGenericPortMapping(a, b, c, d)  (cmsMdm_isDataModelDevice2() ? \
                  upnp_getGenericPortMapping_dev2((a), (b), (c), (d)) : \
                  upnp_getGenericPortMapping_igd((a), (b), (c), (d)))
#endif




/** Get portmapping by the specified params
 *
 * @return errorInfo code
 */
int upnp_getSpecificPortMapping(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
            const char *protocol, UINT16 externalPort, const char *remoteHost,
            PAction ac);

int upnp_getSpecificPortMapping_igd(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
            const char *protocol, UINT16 externalPort, const char *remoteHost,
            PAction ac);

int upnp_getSpecificPortMapping_dev2(UBOOL8 isPpp, const InstanceIdStack *parentIidStack,
            const char *protocol, UINT16 externalPort, const char *remoteHost,
            PAction ac);

#if defined(SUPPORT_DM_LEGACY98)
#define upnp_getSpecificPortMapping(a, b, c, d, e, f)  upnp_getSpecificPortMapping_igd((a), (b), (c), (d), (e), (f))
#elif defined(SUPPORT_DM_HYBRID)
#define upnp_getSpecificPortMapping(a, b, c, d, e, f)  upnp_getSpecificPortMapping_igd((a), (b), (c), (d), (e), (f))
#elif defined(SUPPORT_DM_PURE181)
#define upnp_getSpecificPortMapping(a, b, c, d, e, f)  upnp_getSpecificPortMapping_dev2((a), (b), (c), (d), (e), (f))
#elif defined(SUPPORT_DM_DETECT)
#define upnp_getSpecificPortMapping(a, b, c, d, e, f)  (cmsMdm_isDataModelDevice2() ? \
                  upnp_getSpecificPortMapping_dev2((a), (b), (c), (d), (e), (f)) : \
                  upnp_getSpecificPortMapping_igd((a), (b), (c), (d), (e), (f)))
#endif



#endif /* _igd_h_ */
