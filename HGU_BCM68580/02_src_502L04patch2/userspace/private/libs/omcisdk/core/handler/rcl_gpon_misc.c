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
*      RCL handlers for Miscellaneous OMCI MEs.
*
*****************************************************************************/

#ifdef DMP_X_ITU_ORG_GPON_1

/* ---- Include Files ----------------------------------------------------- */

#include "owsvc_api.h"
#include "mdmlite_api.h"
#include "me_handlers.h"
#include "ctlutils_api.h"
#include "owrut_api.h"


/* ---- Private Constants and Types --------------------------------------- */


/* ---- Private Function Prototypes --------------------------------------- */


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */


/* ---- Functions --------------------------------------------------------- */

CmsRet rcl_miscellaneousServicesObject(
  _MiscellaneousServicesObject *newObj __attribute__((unused)),
  const _MiscellaneousServicesObject *currObj __attribute__((unused)),
  const InstanceIdStack *iidStack __attribute__((unused)),
  char **errorParam __attribute__((unused)),
  CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_eSCObject(
  _ESCObject *newObj __attribute__((unused)),
  const _ESCObject *currObj __attribute__((unused)),
  const InstanceIdStack *iidStack __attribute__((unused)),
  char **errorParam __attribute__((unused)),
  CmsRet *errorCode __attribute__((unused)))
{
   CmsRet ret = CMSRET_SUCCESS;

   return ret;
}

CmsRet rcl_broadcastKeyTableObject(
  _BroadcastKeyTableObject *newObj __attribute__((unused)),
  const _BroadcastKeyTableObject *currObj __attribute__((unused)),
  const InstanceIdStack *iidStack __attribute__((unused)),
  char **errorParam __attribute__((unused)),
  CmsRet *errorCode __attribute__((unused)))
{
    CmsRet ret = CMSRET_SUCCESS;
    UINT8 *addr = NULL, key_frg, key_frg_len, key_idx;
    UINT32 addrSize = 0;
    BCM_Ploam_McastEncryptionKeysInfo info;

    if (SET_EXISTING(newObj, currObj))
    {
        ret = cmsUtl_hexStringToBinaryBuf(newObj->broadcastKey, &addr, &addrSize);
        if (addrSize != (OMCI_ENTRY_SIZE_36/2))
        {
            ret = CMSRET_INVALID_ARGUMENTS;
            goto Exit;
        }

        key_idx = addr[1] >> 6;
        key_frg = addr[1] & 0xf;
        key_frg_len = addr[0] >> 4;

        /* Support only key length of 128 bits, which is len == 0 */
        if (key_frg_len != 0 || key_frg != 0)
        {
            cmsLog_error("Support only XGPON MCAST encryption key with single"
              "fragment 128 bits long (key len %d, frag id: %d)",
              key_frg_len, key_frg);
            ret = CMSRET_INVALID_ARGUMENTS;
            goto Exit;
        }

        info.key_idx = key_idx;
        memcpy(info.key, &addr[2], sizeof(info.key));
        ret = gponCtl_setMcastEncryptionKeys(&info);
        if (ret)
        {
            cmsLog_error("Failed to set XGPON MCAST encryption keys "
              "(keyidx %d) Error: %d", key_idx, ret);
        }
    }
    else if (DELETE_EXISTING(newObj, currObj))
    {
        /* TODO: invalidate key */
    }

Exit:
    cmsMem_free(addr);

    return ret;
}

#endif /* DMP_X_ITU_ORG_GPON_1 */
