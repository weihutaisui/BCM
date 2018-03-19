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

/*
 * IEEE1905 Main
 */

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "ieee1905_glue.h"
#include "ieee1905_interface.h"
#include "ieee1905_socket.h"
#include "ieee1905_json.h"
#include "ieee1905_message.h"
#include "ieee1905_netlink.h"
#include "ieee1905_security.h"
#include "ieee1905_trace.h"
#include "ieee1905_plc.h"
#include "ieee1905_ethstat.h"
#include "ieee1905_brutil.h"
#include "ieee1905_udpsocket.h"
#include "ieee1905_wlcfg.h"
#include "ieee1905_flowmanager.h"
#include "ieee1905_control.h"
#include "ieee1905_cmsutil.h"
#include "i5ctl.h"


#define I5_TRACE_MODULE i5TraceMain

#if defined(SUPPORT_IEEE1905_FM)
#pragma message "1905 flow manager support enabled"
#else
#pragma message "1905 flow manager support disabled"
#endif

#if defined(SUPPORT_IEEE1905_AUTO_WDS)
#pragma message "1905 wds auto configuration enabled"
#else
#pragma message "1905 wds auto configuration disabled"
#endif

char *const traceTokens[] = {
  "m", /* module - int or string */
  "l", /* log level */
  "i", /* interface - if applicable */
  NULL
};

int main(int argc, char *argv[])
{
  int c;
  int rc = 0;

  //print command line options:
  if (1) {
      printf("STARTING 1905: command line: ");
      for (c = 0; c < argc; c++)
        printf("%s ", argv[c]);
      printf("\n");
  }  

  memset(&i5_config, 0, sizeof(i5_config_type));

  while ((c = getopt(argc, argv, "t:")) != -1) {
    switch (c) {
      case 't':
      {
        char *subopttok;
        char *value;
        char *opt;
        char *opttok;
        char *endptr;
        int   level   = 0;
        int   module  = 255;
        int   ifindex = 0;
        int   reqOptCnt = 0;
        
        value = strtok_r(optarg, ",", &subopttok);
        while ( value != NULL ) {
          opt = strtok_r(value, "=", &opttok);
          while ( 1 ) {
            if ( 0 == strcmp(opt, "m") ) {
              opt = strtok_r(NULL, "=", &opttok);
              errno = 0;
              module = strtol(opt, &endptr, 10);
              if ( (errno != 0) || (opt == endptr) ) {
                 break;
              }
              reqOptCnt++;
            }
            else if ( 0 == strcmp(opt, "l") ) { 
              opt = strtok_r(NULL, "=", &opttok);
              errno = 0;
              level = strtol(opt, &endptr, 10);
              if ( (errno != 0) || (opt == endptr) ) {
                 break;
              }
              reqOptCnt++;
            }
            else if ( 0 == strcmp(opt, "i") ) { 
              opt = strtok_r(NULL, "=", &opttok);
              errno = 0;
              ifindex = strtol(opt, &endptr, 10);
              if ( (errno != 0) || (opt == endptr) ) {
                 ifindex = if_nametoindex(opt);
                 break;
              }
            }
            else {
              printf("No match found for token: /%s/\n", opt);
            }
            break;
          }
          value = strtok_r(NULL, ",", &subopttok);
        }
        if ( reqOptCnt == 2 ) {
          i5TraceSet(module, level, ifindex, NULL);
        }
        else {
          printf("Ignoring invalid trace option\n");
        }
        break;
      }
      default:
        printf("Warning -- unknown option %c\n", c);
    }
  }

  do {
    if ( i5GlueMainInit() < 0 ) {
      i5TraceError("Main initialization error\n");
      break;
    }
    i5SocketInit();
#if defined(BRCM_CMS_BUILD)
    if ( i5CmsutilInit() < 0 ) {
      i5TraceError("CMS intialization error\n");
      rc = 1;
      break;
    }
#endif
    i5GlueLoadConfig();

    if ( i5DmInit() < 0 ) {
      i5TraceError("DM initialization error\n");
      rc = 1;
      break;;
    }

    i5JsonInit();
    i5NetlinkInit();
#if defined(SUPPORT_HOMEPLUG)
    i5PlcInitialize();
#endif
#if defined(IEEE1905_KERNEL_MODULE_PB_SUPPORT)
    i5UdpSocketInit();
#endif
    i5SecurityInit();
#if defined(WIRELESS)
    i5WlCfgInit();
#endif
    i5EthStatInit();
    i5BrUtilInit();
    i5InterfaceInit();

    /* enable state or other settings may have changed during init */
    i5_config.running = 1;
    i5GlueSaveConfig();
    i5SocketMain();
  } while (0);

  printf("Shutting down 1905\n");

  /* save configuration - specifically status */
  i5GlueSaveConfig();

#if defined(WIRELESS)
  i5WlCfgDeInit();
#endif
#if defined(SUPPORT_IEEE1905_FM)
  i5FlowManagerDeinit();
#endif
  i5BrUtilDeinit();
  i5MessageDeinit();
  i5DmDeinit();
  i5JsonDeinit();
#if defined(IEEE1905_KERNEL_MODULE_PB_SUPPORT)
  i5UdpSocketDeinit();
#endif
#if defined(BRCM_CMS_BUILD)
  i5CmsutilDeinit();
#endif
  i5SocketDeinit();
  return rc;
}
