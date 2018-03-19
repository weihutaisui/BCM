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
*      GPON utility: interface related.
*
*****************************************************************************/

#ifdef DMP_X_ITU_ORG_GPON_1


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

SINT32 rutGpon_getInterfaceIndex(const char *interfaceName)
{
    struct ifreq ifr;
    SINT32 s = 0, ret = -1;

    if (interfaceName == NULL)
        return ret;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        return ret;

    strcpy(ifr.ifr_name, interfaceName);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0)
        ret = 0;
    else
        ret = ifr.ifr_ifindex;

    close(s);

    return ret;
}

UBOOL8 rutGpon_isInterfaceExisted(const char *interfaceName)
{
    SINT32 ret = 0;

#ifdef DESKTOP_LINUX
    ret = rutGpon_vlanIf_search( (const char *) interfaceName);
#else /* DESKTOP_LINUX */
    ret = rutGpon_getInterfaceIndex(interfaceName);
#endif /* DESKTOP_LINUX */
    return (ret > 0);
}

CmsRet rutGpon_addBridgeInterface
    (const char   *ifcName)
{
    char cmd[BUFLEN_1024];
    CmsRet ret = CMSRET_SUCCESS;
#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1)
    int ifi;
#endif
    UINT32 brgFwdMask = 0;

    // do nothing if bridge interface is already existed
    if (rutGpon_isInterfaceExisted(ifcName) == TRUE)
        return ret;

    snprintf(cmd, sizeof(cmd), "brctl addbr %s", ifcName);
    _owapi_rut_doSystemAction("rcl_gpon", cmd);
    if (_owapi_rut_getBrgFwdMask(&brgFwdMask) == CMSRET_SUCCESS)
    {
        snprintf(cmd, sizeof(cmd),
          " echo %d > /sys/class/net/%s/bridge/group_fwd_mask",
          brgFwdMask, ifcName);
        _owapi_rut_doSystemAction("rcl_gpon", cmd);
    }
    snprintf(cmd, sizeof(cmd), "brctl stp %s off", ifcName);
    _owapi_rut_doSystemAction("rcl_gpon", cmd);
    snprintf(cmd, sizeof(cmd), "brctl setfd %s 0", ifcName);
    _owapi_rut_doSystemAction("rcl_gpon", cmd);
    snprintf(cmd, sizeof(cmd), "sendarp -s br0 -d %s", ifcName);
    _owapi_rut_doSystemAction("rcl_gpon", cmd);
#if defined(DMP_X_BROADCOM_COM_IGMPSNOOP_1)
    ifi = cmsNet_getIfindexByIfname(ifcName);
    bcm_mcast_api_set_snooping_cfg(-1, ifi, BCM_MCAST_PROTO_IPV4, 2, 0);
#endif
    rutGpon_ifUp(ifcName);

    // Seems like in GPON, it is possible for the bridge exists in Linux but
    // not in the MDM, so skip RCL and action of the snooping object, and
    // just update the bridge snooping list
    _owapi_rut_updateIgmpMldSnoopingIntfList();

    return ret;
}

CmsRet rutGpon_deleteBridgeInterface
    (const char   *ifcName)
{
    char cmd[BUFLEN_1024];
    CmsRet ret = CMSRET_SUCCESS;

    // do nothing if bridge interface does not exist yet
    if (rutGpon_isInterfaceExisted(ifcName) == FALSE)
        return ret;

    rutGpon_ifDown(ifcName);
    snprintf(cmd, sizeof(cmd), "brctl delbr %s", ifcName);
    _owapi_rut_doSystemAction("rcl_gpon", cmd);

    // Seems like in GPON, it is possible for the bridge exists in Linux but
    // not in the MDM, so skip RCL and action of the snooping object, and
    // just update the bridge snooping list
    _owapi_rut_updateIgmpMldSnoopingIntfList();

    return ret;
}

CmsRet rutGpon_addInterfaceToBridge
    (const char   *ifcName,
     const char   *toBridge)
{
    char cmd[BUFLEN_1024];
    CmsRet ret = CMSRET_SUCCESS;

    if (ifcName == NULL)
    {
        cmsLog_error("Invalid device interface name: %s\n", ifcName);
        ret = CMSRET_INVALID_ARGUMENTS;
    }
    else if (toBridge == NULL)
    {
        cmsLog_error("Invalid bridge interface name: %s\n", toBridge);
        ret = CMSRET_INVALID_ARGUMENTS;
    }
    else
    {
        // add interface to the current bridge
        snprintf(cmd, sizeof(cmd), "brctl addif %s %s 2>/dev/null", toBridge, ifcName);
        _owapi_rut_doSystemAction("rut_gpon", cmd);
        // re-launch MCPD everytime interface is added to the bridge
        rutGpon_reloadMcpd();
        //snprintf(cmd, sizeof(cmd), "sendarp -s %s -d %s", toBridge, ifcName);
        //_owapi_rut_doSystemAction("rut_gpon", cmd);
    }

    return ret;
}

CmsRet rutGpon_ifUp(const char *ifName)
{
    int sockfd = 0;
    struct ifreq ifr;
    CmsRet ret = CMSRET_SUCCESS;

    if (ifName == NULL)
    {
        cmsLog_error("Cannot bring up NULL interface");
        ret = CMSRET_INTERNAL_ERROR;
    }
    else
    {
        /* Create a channel to the NET kernel. */
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            cmsLog_error("Cannot create socket to the NET kernel");
            ret = CMSRET_INTERNAL_ERROR;
        }
        else
        {
            cmsUtl_strncpy(ifr.ifr_name, ifName, IFNAMSIZ);
            ifr.ifr_flags = (IFF_UP | IFF_RUNNING | IFF_MULTICAST);
            if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0)
            {
                cmsLog_error("Cannot ioctl SIOCSIFFLAGS on the socket");
                ret = CMSRET_INTERNAL_ERROR;
            }
            close(sockfd);
        }
    }

    return ret;
}

CmsRet rutGpon_ifDown(const char *ifName)
{
    int sockfd = 0;
    struct ifreq ifr;
    CmsRet ret = CMSRET_SUCCESS;

    if (ifName == NULL)
    {
        cmsLog_error("Cannot bring down NULL interface");
        ret = CMSRET_INTERNAL_ERROR;
    }
    else
    {
        /* Create a channel to the NET kernel. */
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            cmsLog_error("Cannot create socket to the NET kernel");
            ret = CMSRET_INTERNAL_ERROR;
        }
        else
        {
            cmsUtl_strncpy(ifr.ifr_name, ifName, IFNAMSIZ);
            ifr.ifr_flags = ~IFF_UP;
            if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0)
            {
                cmsLog_error("Cannot ioctl SIOCSIFFLAGS on the socket");
                perror("SIOCSIFFLAGS");
                ret = CMSRET_INTERNAL_ERROR;
            }
            close(sockfd);
        }
    }

    return ret;
}

UBOOL8 rutGpon_checkInterfaceUp(const char *devname)
{

   int  skfd;
   int  ret = FALSE;
   struct ifreq intf;

   if (devname == NULL || (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
   {
      return ret;
   }

   strcpy(intf.ifr_name, devname);

   /*    if interface is br0:0 and
    * there is no binding IP address then return down
    */

   if (strchr(devname, ':') != NULL)
   {
      if (ioctl(skfd, SIOCGIFADDR, &intf) < 0)
      {
         close(skfd);
         return ret;
      }
   }

   // if interface flag is down then return down
   if (ioctl(skfd, SIOCGIFFLAGS, &intf) != -1)
   {
      if ((intf.ifr_flags & IFF_UP) != 0)
         ret = TRUE;
   }

   close(skfd);

   return ret;
}

CmsRet rutGpon_disablePptpEthernetUni(UINT32 port)
{
    int unit = 0;

    if (BCM_E_NONE == bcm_port_traffic_control_set(unit,
      bcm_enet_map_oam_idx_to_phys_port(port), PORT_RXDISABLE | PORT_TXDISABLE))
    {
       return CMSRET_SUCCESS;
    }
    else
    {
       cmsLog_error("bcm_port_traffic_control_set failed, unit=%d, port=%d\n",
         unit, port);
       return CMSRET_INTERNAL_ERROR;
    }
}

CmsRet rutGpon_enablePptpEthernetUni(UINT32 port)
{
    int unit = 0;

    if (BCM_E_NONE == bcm_port_traffic_control_set(unit,
      bcm_enet_map_oam_idx_to_phys_port(port), 0))
    {
       return CMSRET_SUCCESS;
    }
    else
    {
       cmsLog_error("bcm_port_traffic_control_set failed, unit=%d, port=%d\n",
         unit, port);
       return CMSRET_INTERNAL_ERROR;
    }
}

CmsRet rutGpon_getEnetInfo(UINT32 portIndex, UINT32 *pPortInfo)
{
    CmsRet ret = CMSRET_INTERNAL_ERROR;
    int skFd = 0;
    int linkState = 0;
    struct ifreq ifReq;
    struct ethswctl_data ifData;
    struct ethswctl_data *ifDataPtr = &ifData;
    int* tempIntPtr = (int*)&ifData;
    UINT32 line_Speed = 0;
    UINT32 duplex_State = 0;
    IOCTL_MIB_INFO* portMibInfoPtr;

    // Open a basic socket.
    skFd = socket(AF_INET, SOCK_DGRAM, 0);

    if (skFd < 0)
    {
        cmsLog_error("rutGpon_getEnetInfo:  socket open error");
        return CMSRET_SOCKET_ERROR;
    }

    // Clear IOCTL structure.
    memset(&ifReq, 0, sizeof(ifReq));

    // Setup data pointer.
    ifReq.ifr_data = (char *)ifDataPtr;

    // Setup "eth0-3" string.
    sprintf(ifReq.ifr_name, "eth%d", portIndex);

    // Get link-state through IOCTL & test for successful result.
    if (ioctl(skFd, SIOCGLINKSTATE, &ifReq) == 0)
    {
        // Read port's current link-state.
        linkState = *tempIntPtr;

        // Test for valid ENET connection, do not test sensed-type unless link state is up.
        if (linkState != 0)
        {
            // Get link's sensed type through IOCTL.
            if (ioctl(skFd, SIOCMIBINFO, &ifReq) == 0)
            {
                // Setup MIB info pointer
                portMibInfoPtr = (IOCTL_MIB_INFO*)&ifData;

                // Setup sensed line speed on active port.
                line_Speed = (UINT32)portMibInfoPtr->ulIfSpeed;

                switch (line_Speed)
                {
                    // Setup 10BaseT line speed attribute.
                    case SPEED_10MBIT:
                        *pPortInfo = OMCI_LINE_SENSE_10;
                        break;
                    // Setup 100BaseT line speed attribute.
                    case SPEED_100MBIT:
                        *pPortInfo = OMCI_LINE_SENSE_100;
                        break;
                    // Set configuration field to 1gbit.
                    case SPEED_1000MBIT:
                        *pPortInfo = OMCI_LINE_SENSE_1000;
                        break;
                }


                // Setup sensed line speed on active port.
                duplex_State = (UINT32)portMibInfoPtr->ulIfDuplex;

                // Test for FULL_DUPLEX.
                if (duplex_State == 0)
                {
                    // Set configuration field to HALF_DUPLEX.
                    *pPortInfo |= OMCI_LINE_HALF_DUPLEX;
                }
                else
                {
                    // Set configuration field to FULL_DUPLEX.
                    *pPortInfo |= OMCI_LINE_FULL_DUPLEX;
                }

                ret = CMSRET_SUCCESS;
            }
        }
    }

    // Close socket's FD.
    close(skFd);

    return ret;
}

#endif /* DMP_X_ITU_ORG_GPON_1 */
