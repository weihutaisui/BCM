/***********************************************************************
 *
 *  Copyright (c) 2006  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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


#ifndef __DAL_NETWORK_H__
#define __DAL_NETWORK_H__


#include "cms_core.h"
#include "cms_dal.h"
#include "cms_util.h"
#include "dal.h"


/** Set the system NetworkCofigObj for default gateway interface and dns
 *  information from Web or CLI inputs.
 *  if it is empty string, the action for that variable will be skipped.
 * 
 * @param webVarGwIfcs     (IN) The gateway interface list.
 * @param webVarDnsIfcs    (IN) The dns interface list
 * @param webVarDns1       (IN) The primary static dns ip.
 * @param webVarDns2       (IN) The secondary static dns ip (can be null)
 *
 * @return CmsRet enum.
 */
CmsRet dalWan_setDefaultGatewayAndDns(char *webVarGwIfcs, char *webVarDnsIfcs, char *webVarDns1, char *webVarDns2);

/** Create a new lan device (br1) by setting up the enablAdvancedDMZ flag (which is done at rcl_networkCfg handler)
 *  and add non DMZ LAN IP and Mask to IPInterface of this new device if they are different from the default
 * @param nonDmzIp       (IN) Non DMZ ip address.
 * @param nonDmzMask     (IN) Non DMZ ip address mask.
 *
 * @return CmsRet enum.
 */
CmsRet dalWan_addNonAdvancedDmzLan(const char *nonDmzIp,  const char *nonDmzMask);


/** Delete the non advanced DMZ LAN 
 *
 */
CmsRet dalWan_deleteNonAdvancedDmzLan(void);

#ifdef DMP_X_BROADCOM_COM_IPV6_1 /* aka SUPPORT_IPV6 */
/**  If the wan interface is removed, and it is used for default gateway, dns server or
  *  site prefix delegation, clean up the related parameters in the data model.
  *  
  * @param oid      (IN) The object id of the deleted WAN connection.
  * @param iidStackDel (IN) The iidStack o the deleted WAN connection.
  */
void removeIfcFromNetworkCfg6(MdmObjectId oid, InstanceIdStack *iidStackDel);

#endif

#endif /* __DAL_NETWORK_H__ */
