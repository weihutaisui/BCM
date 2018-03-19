/***********************************************************************
 *
 *  Copyright (c) 2017 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:omcid

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


/*****************************************************************************
*    Description:
*
*      GPON utility. 
*
*****************************************************************************/


/* ---- Include Files ----------------------------------------------------- */

#include "owsvc_api.h"
#include "ctlutils_api.h"
#include "owrut_api.h"
#include "me_handlers.h"
#include "omci_pm.h"


/* ---- Private Constants and Types --------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */


/* ---- Functions --------------------------------------------------------- */

CmsRet rutcmn_getBCastFromIpSubnetMask(char* inIpStr, char* inSubnetMaskStr, char *outBcastStr)
{
   struct in_addr ip;
   struct in_addr subnetMask;
   struct in_addr bCast;
   CmsRet ret = CMSRET_SUCCESS;

   if (inIpStr == NULL || inSubnetMaskStr == NULL || outBcastStr == NULL)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   ip.s_addr = inet_addr(inIpStr);
   subnetMask.s_addr = inet_addr(inSubnetMaskStr);
   bCast.s_addr = ip.s_addr | ~subnetMask.s_addr;
   strcpy(outBcastStr, inet_ntoa(bCast));

   return ret;
}

CmsRet rutcmn_getBcastStrAndSubnetFromIpAndMask(char *InIpAddressStr,
                                                char *InMaskStr,
                                                char *outBCastStr,
                                                char *outSubnetStr)
{
   struct in_addr ip, mask, subnet;

   if (rutcmn_getBCastFromIpSubnetMask(InIpAddressStr, InMaskStr, outBCastStr) != CMSRET_SUCCESS)
   {
      cmsLog_error("bad address %s/%s", InIpAddressStr, InMaskStr);
      return CMSRET_INVALID_ARGUMENTS;
   }

   if (!inet_aton(InIpAddressStr, &ip) || !inet_aton(InMaskStr, &mask) || outSubnetStr == NULL)
   {
      cmsLog_error("Invalid strings:  ipAddress %s, mask %s or subnet %s", InIpAddressStr, InMaskStr, outSubnetStr);
      return CMSRET_INVALID_ARGUMENTS;
   }

   subnet.s_addr = ip.s_addr & mask.s_addr;
   strcpy(outSubnetStr, inet_ntoa(subnet));

   return CMSRET_SUCCESS;

}

