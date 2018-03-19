/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

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
 *
 ************************************************************************/

#ifndef __CGI_UTIL_H__
#define __CGI_UTIL_H__

#include "cms.h"

/** Read the current LAN IP address from MDM and compare it to the 
 *  given IP address.
 *
 * @param newIpAddr     (IN) New IP address that the user wants to change to.
 * @param intfGroupName (IN) The interface group name
 *

 * @return TRUE if the LAN IP address in the MDM is different from
 *              the given IP address.  FALSE otherwise.
 */

UBOOL8 cgiWillLanIpChange(const char *newIpAddr, const char *intfGroupName);
UBOOL8 cgiWillLanIpChange_igd(const char *newIpAddr, const char *intfGroupName);
UBOOL8 cgiWillLanIpChange_dev2(const char *newIpAddr, const char *intfGroupName);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiWillLanIpChange(a, b)   cgiWillLanIpChange_igd((a), (b))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiWillLanIpChange(a, b)   cgiWillLanIpChange_igd((a), (b))
#elif defined(SUPPORT_DM_PURE181)
#define cgiWillLanIpChange(a, b)   cgiWillLanIpChange_dev2((a), (b))
#elif defined(SUPPORT_DM_DETECT)
#define cgiWillLanIpChange(a, b)   (cmsMdm_isDataModelDevice2() ? \
                                    cgiWillLanIpChange_dev2((a), (b)) : \
                                    cgiWillLanIpChange_igd((a), (b)))
#endif


/** It is to check whether the INPUT IP and Subnet is used by  other interface group.
 *
 * Each interface group corresponds to a bridge interface.
 *
 * @param intfGroupName (IN) The interface group name
 * @param ipAddr        (IN) New IP address that the user wants to change to.
 * @param subnetMask    (IN) New subnet mask that the user wants to change to.
 *
 * @return TRUE if the specified ipAddr/subnetMask is in use by another interface group.
 */
UBOOL8 isUsedByOtherBridge(const char *intfGroupName, const char *ipAddr, const char *subnetMask);
UBOOL8 isUsedByOtherBridge_igd(const char *intfGroupName, const char *ipAddr, const char *subnetMask);
UBOOL8 isUsedByOtherBridge_dev2(const char *intfGroupName, const char *ipAddr, const char *subnetMask);

#if defined(SUPPORT_DM_LEGACY98)
#define isUsedByOtherBridge(a, b, c)   isUsedByOtherBridge_igd((a), (b), (c))
#elif defined(SUPPORT_DM_HYBRID)
#define isUsedByOtherBridge(a, b, c)   isUsedByOtherBridge_igd((a), (b), (c))
#elif defined(SUPPORT_DM_PURE181)
#define isUsedByOtherBridge(a, b, c)   isUsedByOtherBridge_dev2((a), (b), (c))
#elif defined(SUPPORT_DM_DETECT)
#define isUsedByOtherBridge(a, b, c)   (cmsMdm_isDataModelDevice2() ? \
                                 isUsedByOtherBridge_dev2((a), (b), (c)) : \
                                 isUsedByOtherBridge_igd((a), (b), (c)))
#endif


/* It is to read the current DHCP static IP tables 
 *
 */  
void cgiGetStaticIpLease(int argc, char **argv, char *varValue);


/** return TRUE if IPoA or static MER protocol
  *
  * @return TRUE if IPoA or static MER protocol
  */
UBOOL8 isStaticPVC(void);


/** return TRUE if there is a VDSL PTM WAN link on the modem.
 *  The link must be up, otherwise, we don't know if this link is actually
 *  PTM or not.
 *
 * @return TRUE if there is a VDSL PTM WAN link on the modem.
 */
UBOOL8 isVdslPtmMode(void);


/** Determine modem has DSL bonding enabled.
 *
 * This function is used by the ejGetOther cgi function.
 * 
 * @param argc (IN) number of args passed in. (not used)
 * @param argv (IN) array of args. (not used)
 * @param varValue (OUT) print a string 1 if supported, 0 if not.
 */
void cgiGetIsDslBondingEnabled (int argc, char **argv, char *varValue);


/** return TRUE if there is a xDSL link up.
 *
 * @return TRUE if there is a xDSL link up.
 */
UBOOL8 isXdslLinkUp(void);


/** Determine if we are currently in VDSL mode.
 *
 * Not clear if this really means determine if the modem is capable of supporting
 * VDSL, even if we are currently in ADSL or if the link is not up.
 * 
 * @param argc (IN) number of args passed in. (not used)
 * @param argv (IN) array of args. (not used)
 * @param varValue (OUT) print a string 1 if supported, 0 if not.
 */
void cgiIsVdslSupported (int argc, char **argv, char *varValue);

/** Convert seconds to "XXXXX days, XX hours, XX minutes, XX seconds" format
 * 
 * @return string with the above format
 */
char *cgiRelTime(UINT32 seconds);

struct lease_t {
	unsigned char chaddr[16];
	u_int32_t yiaddr;
	u_int32_t expires;
	char hostname[64];
};

/** Print out the dhcpd info
 *
 * @return void.
 */
void cgiPrintDHCPLeases(FILE *fs);

/** Print out the group list
 *
 * @return void.
 */
void cgiPrintGroupList(char *print);
void cgiPrintGroupList_igd(char *print);
void cgiPrintGroupList_dev2(char *print);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiPrintGroupList(p)   cgiPrintGroupList_igd((p))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiPrintGroupList(p)   cgiPrintGroupList_igd((p))
#elif defined(SUPPORT_DM_PURE181)
#define cgiPrintGroupList(p)   cgiPrintGroupList_dev2((p))
#elif defined(SUPPORT_DM_DETECT)
#define cgiPrintGroupList(p)   (cmsMdm_isDataModelDevice2() ? \
                                cgiPrintGroupList_dev2((p)) : \
                                cgiPrintGroupList_igd((p)))
#endif

/** Read the DeviceInfo object to retrieve upTime, convert to string, and return its value back.
 *
 *@param (IN) pointer to char
 */
void getUptimeString(char *value);
void getUptimeString_igd(char *value);
void getUptimeString_dev2(char *value);

#if defined(SUPPORT_DM_LEGACY98)
#define getUptimeString(value)   getUptimeString_igd((value))
#elif defined(SUPPORT_DM_HYBRID)
#define getUptimeString(value)   getUptimeString_igd((value))
#elif defined(SUPPORT_DM_PURE181)
#define getUptimeString(value)   getUptimeString_dev2((value))
#elif defined(SUPPORT_DM_DETECT)
#define getUptimeString(value)   (cmsMdm_isDataModelDevice2() ? \
                                  getUptimeString_dev2((value)) : \
                                  getUptimeString_igd((value)))
#endif

/** Read the Dev2HostObject to retrieve MAC address, and return its value back.
 *
 *@param (IN) pointer to char
 */
void getPcMacAddr(char *value);
void getPcMacAddr_igd(char *value);
void getPcMacAddr_dev2(char *value);

#if defined(SUPPORT_DM_LEGACY98)
#define getPcMacAddr(value)   getPcMacAddr_igd((value))
#elif defined(SUPPORT_DM_HYBRID)
#define getPcMacAddr(value)   getPcMacAddr_igd((value))
#elif defined(SUPPORT_DM_PURE181)
#define getPcMacAddr(value)   getPcMacAddr_dev2((value))
#elif defined(SUPPORT_DM_DETECT)
#define getPcMacAddr(value)   (cmsMdm_isDataModelDevice2() ? \
                                  getPcMacAddr_dev2((value)) : \
                                  getPcMacAddr_igd((value)))
#endif

/** Get DSL line rate path(0/1) and downstream/upstream specified in argument and store in varValue
 *
 *@param (IN) argc count
 *@param (IN) argv count       
 *@param (OUT)char varValue
 */
void cgiGetDslLineRate(int argc, char **argv, char *varValue);

void cgiGetDslLineRate_igd(int argc, char **argv, char *varValue);

void cgiGetDslLineRate_dev2(int argc, char **argv, char *varValue);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiGetDslLineRate(a,b,c)   cgiGetDslLineRate_igd((a),(b),(c))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiGetDslLineRate(a,b,c)   cgiGetDslLineRate_igd((a),(b),(c))
#elif defined(SUPPORT_DM_PURE181)
#define cgiGetDslLineRate(a,b,c)   cgiGetDslLineRate_dev2((a),(b),(c))
#elif defined(SUPPORT_DM_DETECT)
#define cgiGetDslLineRate(a,b,c)   (cmsMdm_isDataModelDevice2() ? \
                                    cgiGetDslLineRate_dev2((a),(b),(c)) : \
                                    cgiGetDslLineRate_igd((a),(b),(c)))
#endif

CmsRet cgiAutoAtmAdd(char *query, FILE *fs);
CmsRet cgiAutoAtmAdd_dev2(char *query, FILE *fs);
CmsRet cgiAutoAtmAdd_igd(char *query, FILE *fs);

#if defined(SUPPORT_DM_LEGACY98)
#define cgiAutoAtmAdd(a,b)         cgiAutoAtmAdd_igd((a),(b))
#elif defined(SUPPORT_DM_HYBRID)
#define cgiAutoAtmAdd(a,b)         cgiAutoAtmAdd_igd((a),(b))
#elif defined(SUPPORT_DM_PURE181)
#define cgiAutoAtmAdd(a,b)         cgiAutoAtmAdd_dev2((a),(b))
#elif defined(SUPPORT_DM_DETECT)
#define cgiAutoAtmAdd(a,b)         (cmsMdm_isDataModelDevice2() ? \
                                    cgiAutoAtmAdd_dev2((a),(b)) : \
                                    cgiAutoAtmAdd_igd((a),(b)))
#endif

#endif // __CGI_UTIL_H__
