/***********************************************************************
 *
 *  Copyright (c) 2008  Broadcom Corporation
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


#include <errno.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <bridgeutil.h>
#include <net/if.h>

#include "cms.h"
#include "cms_util.h"
#include "../oal.h"

#define MAX_FDB_READ_PER_ITER 128

CmsRet oal_getPortNameFromMacAddr(const char *ifName, const unsigned char *macAddr, char *portName)
{
   int                portnum = -1;
   int                i;
   CmsRet             ret=CMSRET_INTERNAL_ERROR;
   struct __fdb_entry fe[MAX_FDB_READ_PER_ITER];
   int                cnt = 0;

   do 
   {
      /* search through all fdb entries */
      cnt = br_util_read_fdb(ifName, fe, cnt, MAX_FDB_READ_PER_ITER);
      if ( cnt )
      {
         for ( i = 0; i < cnt; i++ )
         {
            if (memcmp(macAddr, fe[i].mac_addr, MAC_ADDR_LEN) == 0)
            {
               portnum = fe[i].port_no;
               cmsLog_debug("found port id = %d", portnum);
               break;
            }
         }
      }
   } while ((cnt == MAX_FDB_READ_PER_ITER) && (portnum == -1));

   if ( portnum != -1 )
   {
      if ( 0 == br_util_get_port_name(ifName, portnum, portName, CMS_IFNAME_LENGTH) )
      {
         cmsLog_debug("PortName=%s", portName);
         ret = CMSRET_SUCCESS;
      }
      else
      {
         cmsLog_error("unable to find name for portnum %d", portnum);
      }
   }
   else
   {
      cmsLog_error("port not found for mac");
   }

   return ret;
}

