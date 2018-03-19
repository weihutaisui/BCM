/***********************************************************************
 * <:copyright-BRCM:2013:proprietary:standard
 * 
 *    Copyright (c) 2013 Broadcom 
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
 * :>
 *
 * $Change: 160447 $
 ***********************************************************************/

/*
 * IEEE1905 Glue
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#if defined(WANDEV_SUPPORT)
#include <bcm_local_kernel_include/linux/sockios.h>
#else
#include <linux/sockios.h>
#endif
#include <linux/if.h>
#include "ieee1905_glue.h"
#include "ieee1905_plc.h"
#include "ieee1905_wlcfg.h"
#include "ieee1905_ethstat.h"
#include "ieee1905_trace.h"
#include "ieee1905_datamodel.h"
#include "i5api.h"
#include "ieee1905_brutil.h"
#include "ieee1905_tlv.h"
#if defined(SUPPORT_ETHSWCTL)
#include "ethswctl_api.h"
#endif
#if defined(BRCM_CMS_BUILD)
#include "ieee1905_cmsutil.h"
#include "ieee1905_cmsmdm.h"
#endif

#define I5_TRACE_MODULE i5TraceGlue

typedef struct _i5_glue_interface_name_info {
  char             *nameString;
  unsigned short   mediaType;
  unsigned short   (*getMediaType)(const char *ifname, unsigned char *pMediaInfo, int *pMediaLen, unsigned char *netTechOui, unsigned char *netTechVariant, unsigned char *netTechName, unsigned char *url, int sizeUrl);
  int              flags;
} i5_glue_interface_name_info;

#define I5_GLUE_INTERFACE_EXACT_MATCH   (1 << 0)

/* I5_GLUE_INTERFACE_EXACT_MATCH can be set in the flags field to require exact name match
   anything requiring an exact match should be at the start of the list */
i5_glue_interface_name_info i5GlueInterfaceList[] = {
  {"eth",  I5_MEDIA_TYPE_UNKNOWN,  i5EthStatFetchIfInfo,       0 },
#if defined(SUPPORT_HOMEPLUG)
  {"plc",  I5_MEDIA_TYPE_UNKNOWN,  i5PlcFetchIfInfo,           0 },
#endif
  {"moca", I5_MEDIA_TYPE_MOCA_V11, NULL,                       0 },
#if defined(WIRELESS)
  {"wl",   I5_MEDIA_TYPE_UNKNOWN,  i5WlCfgFetchWirelessIfInfo, 0 },
  {"wds",  I5_MEDIA_TYPE_UNKNOWN,  i5WlCfgFetchWirelessIfInfo, 0 },
#endif
  {NULL,   0 }
};

static int i5GlueAssign1905AlMac( )
{
#if defined(CMS_BOARD_UTIL_SUPPORT)
#if defined(SUPPORT_HOMEPLUG)
  unsigned char tmpMacAddr[MAC_ADDR_LEN];
  /* TDB: 1905 daemon and homeplugd start at roughly the same time and
     both request a MAC address. MAC addresses are reassigned every time 
     the board boots. This can result in 1905 and homeplugd
     swapping MAC addresses after a reboot. To avoid this,
     1905 will request the PLC MAC before requesting its own MAC */
  i5CmsutilGet1901MacAddress(tmpMacAddr);
#endif
  if ( i5CmsutilGet1905MacAddress(i5_config.i5_mac_address) < 0 ) {
    return -1;
  }
#else
  /* assign a random MAC */
  srand((unsigned)time(NULL));
  i5_config.i5_mac_address[0] = 0x02;
  i5_config.i5_mac_address[1] = 0x10;
  i5_config.i5_mac_address[2] = 0x18;
  i5_config.i5_mac_address[3] = rand() & 0xFF;
  i5_config.i5_mac_address[4] = rand() & 0xFF;
  i5_config.i5_mac_address[5] = rand() & 0xFF;
#endif

  printf("1905 MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           i5_config.i5_mac_address[0], i5_config.i5_mac_address[1], i5_config.i5_mac_address[2],
           i5_config.i5_mac_address[3], i5_config.i5_mac_address[4], i5_config.i5_mac_address[5]);
  return 0;
}

/* Once a data model entry is created for an interface 1905 will create a socket
   to send and receive packets. This interface can be used to prevent a socket
   from being created. Right now if the interface is not oeprational it is 
   excluded */
int i5GlueInterfaceIsSocketRequired(char const *ifname)
{
  char path[MAX_PATH_NAME];
  char operStatus[32];
  FILE *f;

  /* don't create socket if interface is not operational */
  snprintf(path, MAX_PATH_NAME, "/sys/class/net/%s/operstate", ifname);
  f = fopen(path, "r");
  if ( f ) {
    if ((fscanf(f, "%31s", &operStatus[0]) == 1) && 
        ((0 == strcmp("up", &operStatus[0])) || (0 == strcmp("unknown", &operStatus[0])) )
       ) {
      fclose(f);
      return 1;
    }
    fclose(f);
  }
  else {
    i5TraceError("cannot read operstate for interface %s\n", ifname);
  }

  return 0;
}

/* This function is used to exclude interfaces prior to creating a data model entry.
 *  - excludes interfaces flagged as WAN 
 *  - if a bridge exists, exclude interfaces that are not members of a bridge
 *
 * NOTE: This function assumes the bridge interfaces are learned before all other interfaces
 */
int i5GlueInterfaceFilter(char const *ifname)
{
#if defined(WANDEV_SUPPORT)
  struct ifreq ifr;
  int sockfd, err;

  sockfd = socket(PF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    printf("unable to create socket\n");
    return 0;
  }

  strncpy(ifr.ifr_name, ifname, I5_MAX_IFNAME);
  err = ioctl(sockfd, SIOCDEVISWANDEV, (void*)&ifr);
  close(sockfd);
  if((err != -1) && (ifr.ifr_flags != 0))
  {
    return 0;
  }
#endif

  if ( !i5BrUtilDevIsBridge (ifname) ) {
    i5_dm_device_type *selfDevice = i5DmDeviceFind(i5_config.i5_mac_address);

    if (NULL == selfDevice) {
      i5TraceInfo("No device yet\n");
      return 0;
    }
    else if ( selfDevice->bridging_tuple_list.ll.next != NULL ) {
      char path[MAX_PATH_NAME];
      struct stat s;
      snprintf(path, MAX_PATH_NAME, "/sys/class/net/%s/brport", ifname);

      i5TraceInfo("Bridge Detected, checking for %s\n",path);

      if ( (stat(path, &s) != 0) || (!S_ISDIR(s.st_mode)) ) {
        i5TraceInfo("Not in bridge: %s\n",ifname);
        return 0;
      }
    }
  }

  return 1;
}

unsigned short i5GlueInterfaceGetMediaInfoFromName( char const *ifname, unsigned char *pMediaInfo, int *pMediaLen,  
                                                    unsigned char *pNetTechOui,  unsigned char *pNetTechVariant,  unsigned char *pNetTechName,
                                                    unsigned char *url, int sizeUrl)
{
  i5_glue_interface_name_info * pNameInfo = &i5GlueInterfaceList[0];
  int ifNameLen = strlen(ifname);
  int mediaType = I5_MEDIA_TYPE_UNKNOWN;

  i5TraceInfo("\n");

  if ( 0 == i5GlueInterfaceFilter(ifname) ) {
    return I5_MEDIA_TYPE_UNKNOWN;
  }

  if ( i5BrUtilDevIsBridge(ifname) ) {
    return I5_MEDIA_TYPE_BRIDGE;
  }

  while( pNameInfo->nameString != NULL ) {
    int nameLen = strlen(pNameInfo->nameString);
    if ( ifNameLen >= strlen(pNameInfo->nameString) ) {
      if ( pNameInfo->flags & I5_GLUE_INTERFACE_EXACT_MATCH ) {
        if (0 == strcmp(ifname, pNameInfo->nameString)) {
          break;
        }
      }
      else {
        if (0 == strncmp(ifname, pNameInfo->nameString, nameLen) ) {
          break;
        }
      }
    }
    pNameInfo++;
  }

  if ( pNameInfo->nameString != NULL ) {
    if ( pNameInfo->getMediaType != NULL ) {
      mediaType = (pNameInfo->getMediaType)(ifname, pMediaInfo, pMediaLen, pNetTechOui, pNetTechVariant, pNetTechName, url, sizeUrl);
    }
    else {
      mediaType = pNameInfo->mediaType;
    }
  }
  return mediaType;
}

int i5GlueMainInit( )
{
  if ( i5GlueAssign1905AlMac() < 0 ) {
    return -1;
  }

#if defined(SUPPORT_ETHSWCTL)
  /* Program the integrated switch to not flood the 1905 MAC address on all ports */
  bcm_multiport_set(0, I5_MULTICAST_MAC);
#endif
  return 0;
}

void i5GlueSaveConfig()
{
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  t_I5_API_CONFIG_BASE cfg;
  t_I5_API_CONFIG_SET_NETWORK_TOPOLOGY cfgNT;
   
  i5TraceInfo("\n");

  cfg.isEnabled = i5_config.running;
  strncpy(cfg.deviceFriendlyName, i5_config.friendlyName, I5_DEVICE_FRIENDLY_NAME_LEN-1);
  cfg.isRegistrar = i5_config.isRegistrar;
  cfg.apFreqBand24En = i5_config.freqBandEnable[i5MessageFreqBand_802_11_2_4Ghz];
  cfg.apFreqBand5En = i5_config.freqBandEnable[i5MessageFreqBand_802_11_5Ghz];
  i5CmsMdmSaveConfig(&cfg);

  cfgNT.isEnabled = i5_config.networkTopEnabled;
  i5CmsMdmSaveNetTopConfig(&cfgNT);
#endif
}

void i5GlueLoadConfig()
{
  i5_dm_device_type *pdevice = i5DmDeviceFind(i5_config.i5_mac_address);
#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  t_I5_API_CONFIG_BASE cfg;
  t_I5_API_CONFIG_SET_NETWORK_TOPOLOGY cfgNT;
#endif

  i5TraceInfo("\n");

#if defined(DMP_DEVICE2_IEEE1905BASELINE_1)
  i5CmsMdmLoadConfig(&cfg);
  i5_config.running = cfg.isEnabled;
  /* cfg.deviceFriendlyName is NULL terminated */
  strncpy(i5_config.friendlyName, cfg.deviceFriendlyName, I5_DEVICE_FRIENDLY_NAME_LEN-1);
  i5_config.isRegistrar = cfg.isRegistrar;
  i5_config.freqBandEnable[i5MessageFreqBand_802_11_2_4Ghz] = cfg.apFreqBand24En;
  i5_config.freqBandEnable[i5MessageFreqBand_802_11_5Ghz] = cfg.apFreqBand5En;

  i5CmsMdmLoadNetTopConfig(&cfgNT);
  i5_config.networkTopEnabled = cfgNT.isEnabled;
#else
  i5_config.running = 1;
  snprintf(i5_config.friendlyName, sizeof(i5_config.friendlyName), "%02X%02X%02X%02X%02X%02X", 
           i5_config.i5_mac_address[0], i5_config.i5_mac_address[1], i5_config.i5_mac_address[2],
           i5_config.i5_mac_address[3], i5_config.i5_mac_address[4], i5_config.i5_mac_address[5]);
  i5_config.isRegistrar = 0;
  i5_config.freqBandEnable[i5MessageFreqBand_802_11_2_4Ghz] = 1;
  i5_config.freqBandEnable[i5MessageFreqBand_802_11_5Ghz] = 1;
  i5_config.networkTopEnabled = 0;
#endif
  if ( pdevice )
  {
     /* cfg.deviceFriendlyName is NULL terminated */
     strncpy(pdevice->friendlyName, i5_config.friendlyName, I5_DEVICE_FRIENDLY_NAME_LEN);
  }
}

int i5GlueAssignFriendlyName(unsigned char *deviceId, char *pFriendlyName, int maxLen)
{
  if ( (NULL == deviceId) || (NULL == pFriendlyName) ) {
    i5TraceError("Invalid input\n");
    return -1;
  }

#if defined(BRCM_CMS_BUILD)
  if ( i5DmDeviceIsSelf(deviceId) )
  {
     i5CmsUtilGetFriendlyName(deviceId, pFriendlyName, maxLen);
     return 0;
  }
#endif
  snprintf(pFriendlyName, I5_DEVICE_FRIENDLY_NAME_LEN, "%02X%02X%02X%02X%02X%02X", deviceId[0], deviceId[1], deviceId[2], deviceId[3], deviceId[4], deviceId[5]);
  return 0;
}

