/*
* <:copyright-BRCM:2006:proprietary:standard
* 
*    Copyright (c) 2006 Broadcom 
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
:>
*/

#ifndef __DAL2_WAN_H__
#define __DAL2_WAN_H__


/* The following functions are in dal2_ppp.c */

UBOOL8 dalWan_isPPPWanConnection_dev2(const WEB_NTWK_VAR *webVar);
CmsRet dalPpp_addIntfObject_dev2(const WEB_NTWK_VAR *webVar,  
                                 UBOOL8 supportIpv4 __attribute((unused)),
                                 UBOOL8 supportIpv6 __attribute((unused)),
                                 const char *lowLayerStr,
                                 char *myPathRef,
                                 UINT32 bufLen);

CmsRet dalPpp_deleteIntfObject_dev2(char *pppLowerlayerFullPath);

                                       
/* The following functions are in dal2_ethernet.c */

CmsRet dalEth_addEthernetLink_dev2(const char *lowerLayer, char *myPathRef, UINT32 bufLen);

CmsRet dalEth_deleteEthernetLinkByName_dev2(const char *ethLinkName);

CmsRet dalEth_addEthernetVlanTermination_dev2(const WEB_NTWK_VAR *webVar,
                                              const char *lowerLayer,
                                              char *myPathRef,
                                              UINT32 bufLen);
                                              
CmsRet dalEth_deleteEthernetVlanTermination_dev2(char *ethLinkFullPath);

CmsRet dalWan_removeFailedWanServiceByL2IfName_dev2(const char * ifName);




/** Add IP.Interface object
 *
 * @param intfGroupName (IN) If this IP.Interface is on the LAN side, the
 *                           interface group name.
 * @param isBridgeService (IN) TRUE if this is a WAN side bridge service
 * @param bridgeName      (IN) If this is a WAN side bridge service, this is
 *                             the layer 2 bridge interface name on the LAN
 *                             side, e.g. br0
 *
 * @param lowerLayerFullPath (IN) This is the lowerlayer of the IP.Interface.
 *                                e.g. fullpath of the Ethernet.Link obj or
 *                                PPP obj that is under the IP.Interface
 *
 * @param myPathRef (OUT) This is a buffer which contains the fullpath to the
 *                        newly created IP.Interface object.
 * @param pathLen   (IN)  Length of the myPathRef buf
 * @param pathDesc  (OUT) This will be filled in with the path descriptor of
 *                        the newly created IP.Interface object.
 */
CmsRet dalIp_addIntfObject_dev2(UBOOL8 supportIpv4,
                                UBOOL8 supportIpv6,
                                const char *intfGroupName,
                                UBOOL8 isBridgeService,
                                const char *bridgeName,
                                UBOOL8 firewallEnabled,
                                UBOOL8 igmpEnabled,
                                UBOOL8 igmpSourceEnabled,
                                UBOOL8 mldEnabled,
                                UBOOL8 mldSourceEnabled,
                                const char *lowerLayerFullPath,
                                char *myPathRef,
                                UINT32 pathLen,
                                MdmPathDescriptor *ipIntfPathDesc);


CmsRet dalIp_deleteIntfObject_dev2(const char *ipLowerlayerFullPath, char *ipFullPath);


/** Add an IPV4_ADDRESS object under the specified iidStack of the IP.Interface.
 *
 */
CmsRet dalIp_addIpIntfIpv4Address_dev2(const InstanceIdStack *ipIntfIidStack, 
                                       const char *ipAddr,
                                       const char *subnetMask);




/* The following functions are in dal2_route.c */  
CmsRet dalRt_addStaticIpv4DefaultGateway_dev2(const char *gwIpAddr, const char * ipIntfFullPath);
void dalRt_deleteStaticIpv4DefaultGateway_dev2(const char * ipIntfFullPath);

/* The following functions are in dal2_nat.c */  
CmsRet dalNat_set_dev2(const char *ipIntfFullPath, UBOOL8 isNATEnabled, UBOOL8 isFullcone);
CmsRet dalNat_delete_dev2(const char *ipIntfFullPath);

/* The following functions are in dal2_pcp.c */  
CmsRet dalPcp_set_dev2(const char *ipIntfFullPath, UINT32 mode, const char *pcpServer);


#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
                                       
CmsRet dalIp_addIpIntfDhcpv6Client_dev2(const WEB_NTWK_VAR *webVar, const char * ipIntfPathRef);

CmsRet dalIp_deleteDhcpv6Client_dev2(const char * ipIntfFullPath);


CmsRet dalIp_addIpIntfDnsClientServerIpv6_dev2(const WEB_NTWK_VAR *webVar, const char *ipIntfPathRef);

                                       
CmsRet dalIp_addIpIntfIPv6Prefix_dev2(const InstanceIdStack *ipIntfIidStack,
                                      const WEB_NTWK_VAR *webVar,
                                      char *myPathRef, 
                                      UINT32 pathLen);


CmsRet dalIp_addIPv6DelegatedPrefix_dev2(const InstanceIdStack *ipIntfIidStack,
                                              char *myPathRef, UINT32 pathLen);


CmsRet dalIp_addIPv6ChildPrefix_dev2(const char *ifname,
                           const char *parentRef, const char *childPrefixBits);


CmsRet dalIp_addIpIntfIpv6Address_dev2(const InstanceIdStack *ipIntfIidStack, 
                                       const WEB_NTWK_VAR *webVar,
                                       char * prefixPathRef);
                                  
#endif /* DMP_X_BROADCOM_COM_DEV2_IPV6_1 */


#endif  /* __DAL2_WAN_H__ */

