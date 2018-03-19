/***********************************************************************
 *
 *  Copyright (c) 2007-2010  Broadcom Corporation
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
#ifdef DMP_DEVICE2_BASELINE_1

#include "cms.h"
#include "cms_util.h"
#include "cms_obj.h"
#include "cms_dal.h"


void dalPMirror_getPMirrorList_dev2(char *lst)
{
#if defined (DMP_DEVICE2_ATMLINK_1) || defined (DMP_DEVICE2_X_BROADCOM_COM_PTMLINK_1)
   char name[CMS_IFNAME_LENGTH+BUFLEN_32]={0};
#endif
   char pMirrorList[BUFLEN_1024]={0};
   SINT32 lstLen=0;


   /* Look for DSL (ATM and PTM) WAN connection devices */
#ifdef DMP_DEVICE2_ATMLINK_1
      {
         Dev2AtmLinkObject *atmLink = NULL;
         InstanceIdStack atmLinkIid = EMPTY_INSTANCE_ID_STACK;
         SINT32 vpi = 0;
         SINT32 vci = 0;
         while (cmsObj_getNext(MDMOID_DEV2_ATM_LINK, &atmLinkIid, (void **)&atmLink) == CMSRET_SUCCESS)
         {
             cmsLog_debug("Found %s/%s ATM link",
                           atmLink->name, atmLink->destinationAddress);

             cmsUtl_atmVpiVciStrToNum_dev2(atmLink->destinationAddress, &vpi, &vci);
             sprintf(name, "%s", atmLink->name);

             dalPMirror_fillPMirrorEntry(name, pMirrorList, sizeof(pMirrorList));

             cmsObj_free((void **) &atmLink);
         }
      }
#endif /* DMP_DEVICE2_ATMLINK_1 */

#ifdef DMP_DEVICE2_X_BROADCOM_COM_PTMLINK_1
      {
         Dev2PtmLinkObject *ptmLink = NULL;
         InstanceIdStack ptmLinkIid = EMPTY_INSTANCE_ID_STACK;
         while (cmsObj_getNext(MDMOID_DEV2_PTM_LINK, &ptmLinkIid, (void **)&ptmLink) == CMSRET_SUCCESS)
          {
             cmsLog_debug("Found %s PTM link", ptmLink->name);

             /*
              * TR98 code has some code here which skipped the interface
              * if it is non-VLANMUX mode and already had one WAN service
              * defined on it.  But in TR181, all connections are
              * VLANMUX mode, so don't worry about it.
              */

            sprintf(name, "%s", ptmLink->name);

            dalPMirror_fillPMirrorEntry(name, pMirrorList, sizeof(pMirrorList));

            cmsObj_free((void **) &ptmLink);
         }
      }
#endif /* DMP_DEVICE2_X_BROADCOM_COM_PTMLINK_1 */    

   if (pMirrorList[0] != '\0')
   {
      strcpy(lst, pMirrorList);
   }  

   lstLen = strlen(lst);
   
   if (lstLen > 0)
   {
      lst[lstLen-1] = '\0';   /* remove the last '|' character   */
   }
   
   cmsLog_debug("Port Mirror list: %s", lst);
}

#endif
   
