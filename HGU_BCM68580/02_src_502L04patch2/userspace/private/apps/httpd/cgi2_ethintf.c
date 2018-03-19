/***********************************************************************
 *
 *  Copyright (c) 2008  Broadcom Corporation
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "cgi_cmd.h"
#include "cgi_ntwk.h"
#include "cgi_util.h"
#include "cgi_main.h"
#include "httpd.h"

#include "cms_util.h"
#include "cms_dal.h"

#include "cms_qdm.h"


#ifdef DMP_DEVICE2_ETHERNETINTERFACE_1

UBOOL8 cgiEthWanCfgViewBody_dev2(FILE *fs)
{
   Dev2EthernetInterfaceObject *ethIntfObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   UBOOL8 isEthWanExist = FALSE;
   
   while (!isEthWanExist &&
          cmsObj_getNextFlags(MDMOID_DEV2_ETHERNET_INTERFACE, &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &ethIntfObj) == CMSRET_SUCCESS)
   {
      if (ethIntfObj->upstream)
      {
         /* If enable, this eth port is configured as layer 2 WAN port */
         if (ethIntfObj->enable)
         {
            isEthWanExist = TRUE;
            fprintf(fs, "   <tr align='center'>\n");

            fprintf(fs, "<script language='javascript'>\n");
            fprintf(fs, "<!-- hide\n");
            fprintf(fs, "brdIntf = brdId + '|' + '%s';\n", ethIntfObj->name);
            fprintf(fs, "intfDisp = getUNameByLName(brdIntf);\n");
            fprintf(fs, "document.write('<td>%s/' + intfDisp + '</td>');\n", ethIntfObj->name);
            fprintf(fs, "// done hiding -->\n");
            fprintf(fs, "</script>\n");

            /* for now, in TR181, we are always in VLANMUX mode */
            fprintf(fs, "      <td>%s</td>\n", MDMVS_VLANMUXMODE);

            /* show remove checkbox */
            fprintf(fs, "      <td align='center'><input type='checkbox' name='rml' value='%s'></td>\n",
                         ethIntfObj->name);
         }

         fprintf(fs, "   </tr>\n");
      }
      cmsObj_free((void **) &ethIntfObj);
   }

   return isEthWanExist;
}


#endif /* DMP_DEVICE2_ETHERNETINTERFACE_1 */



