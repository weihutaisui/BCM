/*
* <:copyright-BRCM:2013:proprietary:epon
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
*/

////////////////////////////////////////////////////////////////////////////////
/// \file EponDevInfo.c
/// \brief Contains configuration access for "personality" info
///
/// The "personality" information configures various options of the oam stack 
/// that allow the same library to behave in different manners.  
/// Personality information is calculated by user space process with data from
/// board parameter, or simply put a default value for the OAM stack usage.
/// 
////////////////////////////////////////////////////////////////////////////////


#include "Build.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "board.h"
#include "EponDevInfo.h"
#include "eponctl_api.h"
#include "TkMsgProcess.h"

// Broadcom specific libraries
#include "cms_boardioctl.h"
#include "cms_boardcmds.h"
#include "cms_log.h"
#include "cms.h"
#ifdef BRCM_CMS_BUILD
#include "cms_eid.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_log.h"
#include "cms_image.h"
#else
#include "EponDevInfoUtil.h"
#endif

#include "bcm_epon_cfg.h"
#include "OamProcessInit.h"
#include "Laser.h"

EponDevInfo eponDevInfo;  

static void fill_dev_extendedId(EponDevInfo *devInfo)
{
    char vendor_id[4] = {'B', 'R', 'C', 'M'};
#if defined(CONFIG_BCM96858)
    char model_index[4] = {'6', '8', '5', '8'};
#elif defined(CONFIG_BCM96848)
    char model_index[4] = {'6', '8', '4', '8'};
#elif defined(CONFIG_BCM96836)
    char model_index[4] = {'6', '8', '3', '6'};
#elif defined(CONFIG_BCM96846)
    char model_index[4] = {'6', '8', '4', '6'};
#elif defined(CONFIG_BCM96856)
    char model_index[4] = {'6', '8', '5', '6'};
#else
    char model_index[4] = {'6', '8', '3', '8'};
#endif
    char hardware_version[8];
    char software_version[16];
    char ext_onu_model[16];
    unsigned int chip_id;
    int boot_partition, partition;
    unsigned int firmware_version = 0x0;
    CmsRet ret = CMSRET_SUCCESS;

    U8 * vendor = (U8 *)&(devInfo->extendedId);

    memcpy(&vendor[CtcOamVendorIdIndex], vendor_id, sizeof(vendor_id));
    memcpy(&vendor[CtcOamOnuModelIndex], model_index, sizeof(model_index));
    
    if ((ret = devCtl_boardIoctl(BOARD_IOCTL_GET_CHIP_ID, 0, NULL, 0, 0, 
                                 &chip_id)) != CMSRET_SUCCESS)
    {
         cmsLog_error("Could not get Chip Id");
    }
    else
    {
        snprintf(hardware_version, sizeof(hardware_version), 
                                              "%x", chip_id);

        memcpy(&vendor[CtcOamHardwareVerIndex], hardware_version, 
                                            sizeof(hardware_version));
    }
    
    if ((ret = devCtl_boardIoctl(BOARD_IOCTL_GET_ID, 0, ext_onu_model, 
                    sizeof(ext_onu_model), 0, NULL)) != CMSRET_SUCCESS)
    {
         cmsLog_error("Could not get Board Id");
    }
    else
    {
         memcpy(&vendor[CtcOamOnuExtOnuModel], ext_onu_model, 
                                               sizeof(ext_onu_model));
    }

 
    boot_partition = devCtl_getBootedImagePartition();
    switch(boot_partition)
    {
        case BOOTED_PART1_IMAGE:
            partition = 1;
            break;

        case BOOTED_PART2_IMAGE:
            partition = 2;
            break;

        default:
            partition = 0;
            break;
    }
    
    if(partition > 0)
    {
        
        if(0 != devCtl_getImageVersion(partition, software_version, 
                                                   sizeof(software_version)))
        {
             memcpy(&vendor[CtcOamSoftwareVerIndex], software_version, 
                                            sizeof(software_version));   
                   
             firmware_version = strtol(software_version, NULL, 10);
             devInfo->firmwareVer = firmware_version & 0xFFFF;
        }  
        else
        {
             cmsLog_error("failed to get sw version using devCtl");
        } 
    }
   
}

static void fill_dev_chipId(EponDevInfo *devInfo)
{
    devInfo->chipId.vendorId = 0x00BF;
    unsigned int chip_id, rev_id;
    CmsRet ret = CMSRET_SUCCESS;
    
    if ((ret = devCtl_boardIoctl(BOARD_IOCTL_GET_CHIP_ID, 0, NULL, 0, 0, 
                                 &chip_id)) != CMSRET_SUCCESS)
    {
         cmsLog_error("Could not get Chip Id");
    }
    else
    {
        devInfo->chipId.chipModel = chip_id & 0xFFFF;
    } 
    
    if ((ret = devCtl_boardIoctl(BOARD_IOCTL_GET_CHIP_REV, 0, NULL, 0, 
                                 0, &rev_id)) != CMSRET_SUCCESS)
    {
         cmsLog_error("Could not get Rev Id");
    }
    else
    {
         devInfo->chipId.revision = (rev_id & 0xf) << 24;
    }
    devInfo->chipId.revision |= 0x0D040F;
}

static void fill_dev_base_mac(EponDevInfo *devInfo)
{
    unsigned char i, pMACAddr[6];
    CmsRet ret;

    if ((OAM_DPOE_SUPPORT == (devInfo->oamsel & OAM_DPOE_SUPPORT))
        || (OAM_BCM_SUPPORT == (devInfo->oamsel & OAM_BCM_SUPPORT)))
    {
        /* Get MAC addresses for all 8 links*/
#if defined(CONFIG_BCM96858) || defined(CONFIG_BCM96856)
        devInfo->eponMacNum = TkOnuMaxBiDirLlids;
#else
        devInfo->eponMacNum = 8;
#endif
    }
    else
    {
        /* Get the MAC address for one link*/
        devInfo->eponMacNum = 1;
    }

    ret = devCtl_getMacAddresses(
        pMACAddr, MAC_ADDRESS_EPONONU, devInfo->eponMacNum);

    if(ret != CMSRET_SUCCESS)
    {
        cmsLog_error("Unable to allocate MAC address for EPON MAC");
        devInfo->resbaseEponMac.u8[0] = 0x00;
        devInfo->resbaseEponMac.u8[1] = 0x10;
        devInfo->resbaseEponMac.u8[2] = 0x18;
        devInfo->resbaseEponMac.u8[3] = 0x00;
        devInfo->resbaseEponMac.u8[4] = 0x00;
        devInfo->resbaseEponMac.u8[5] = 0x00;
        devInfo->eponMacNum = 1;
    }
    else
    {
        for (i=0; i<6; i++)
        {
            devInfo->resbaseEponMac.u8[i] = pMACAddr[i];
        }
    }

    cmsLog_notice("baseEponMac is %x:%x:%x:%x:%x:%x",
                        devInfo->resbaseEponMac.u8[0],
                        devInfo->resbaseEponMac.u8[1],
                        devInfo->resbaseEponMac.u8[2],
                        devInfo->resbaseEponMac.u8[3],
                        devInfo->resbaseEponMac.u8[4],
                        devInfo->resbaseEponMac.u8[5]);
}

#if !defined(DESKTOP_LINUX) && defined(BRCM_CMS_BUILD)
static INT32 get_uni_port_info_from_kernel(U8 *pUniPortNum)
{
    char *ifNames=NULL;    
    UINT32 end, c=0;
    UINT32 idx;    
    char ifNameBuf[CMS_IFNAME_LENGTH];
    UINT8 uniPortNum = 0;
    
    cmsNet_getIfNameList(&ifNames);
    if (ifNames == NULL)
    {
       cmsLog_error("no interfaces found during initialization!");
       return -1;
    }
    else
    {
        uniPortNum = 0;
        end = strlen(ifNames);
        
        while (c < end)
        {
            U8 isSubIf = 0;  
            idx = 0;
            while (c < end && ifNames[c] != ',')
            {
               ifNameBuf[idx] = ifNames[c];
               c++;
               idx++;
            }
            ifNameBuf[idx] = 0;
            c++;

            if (0 == cmsUtl_strncmp(ifNameBuf, "eth", 3))
            {       
                U8 curPos = 0, length = strlen(ifNameBuf);

                isSubIf = 0;                                
                while (curPos < length)
                {
                    if (ifNameBuf[curPos] == '.')
                        {                        
                        isSubIf = 1;
                        break;
                        }
                    curPos++;
                }
                if (isSubIf == 0)
                    uniPortNum++;                
            }
        }
        /* Free the the LAN interface name list */
        cmsMem_free(ifNames);
    }
    
    *pUniPortNum = uniPortNum;
    return 0;
}
#endif
   
static void fill_dev_uni_port_param(EponDevInfo *devInfo)
{
#if !defined(DESKTOP_LINUX) && defined(BRCM_CMS_BUILD)
    CmsRet ret;
    int i;
    unsigned int fe_ports, ge_ports, port_map, mac_type;
    unsigned int voip_ports = 0, port_num = 0;
    U8 * vendor = (U8 *)&(devInfo->extendedId);
    U8 uniPortNum = 0;
    
#ifdef DMP_TAENDPOINT_1
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    VoiceCapObject* voiceCapObj;
#endif
    
    ret = devCtl_getNumFePorts(&fe_ports);
    if (ret != CMSRET_SUCCESS) 
    {
        cmsLog_error("eponapp not able to get the Number of EPON FE ports. Assuming 4");
        fe_ports = 4;
    }
    devInfo->fePortNum = fe_ports;
    
    ret = devCtl_getNumGePorts(&ge_ports);
    if (ret != CMSRET_SUCCESS) 
    {
        cmsLog_error("eponapp not able to get the Number of EPON GE ports. Assuming 0");
        ge_ports = 0;
    }
    cmsLog_debug("Total FE ports %d, GE ports %d\n", fe_ports, ge_ports);
    devInfo->gePortNum = ge_ports;
    devInfo->uniPortNum = fe_ports + ge_ports;

    //TODO: Note here uniPortNum may be different from ge_ports + fe_ports
    //ge_ports and fe_ports are only used for CTC ONU capability report by now
    if((ret = get_uni_port_info_from_kernel(&uniPortNum)) == CMSRET_SUCCESS)
    {
        devInfo->uniPortNum = uniPortNum;
    }
    
#ifdef DMP_TAENDPOINT_1
    if( (ret = cmsLck_acquireLockWithTimeout(50000)) != CMSRET_SUCCESS )
    {
        cmsLog_error("could not acquire lock, ret=%d", ret);
    }
    else
    {    
        if((ret = cmsObj_getNext(MDMOID_VOICE_CAP, &iidStack, 
                                 (void **)&voiceCapObj)) != CMSRET_SUCCESS)
        {
            cmsLog_error("could not get MDMOID_VOICE_CAP, ret=%d", ret);
        }
        else
        {        
            voip_ports = voiceCapObj->maxLineCount;   
            cmsObj_free((void **)&voiceCapObj);
        }    
        cmsLck_releaseLock(); 
    }
#endif

    devInfo->voipPortNum = voip_ports;
    
    /*vendor extended ID needs information of ports, so put it here*/
    vendor[CtcOamE1PortCountIndex] = 0x00;
    vendor[CtcOamPotsPortCountIndex] = voip_ports;
 
    ret = devCtl_getPortMap(&port_map);

    for (i=0; i<MAX_NUM_UNI_PORTS; i++) 
    {      
        devInfo->uniToPhysicalPortMap[i] = 0;
    }


    for (i=0; ((i < devInfo->uniPortNum) && (port_map != 0)); 
              i++, port_map/= 2, port_num++) 
    {

        while ((port_map % 2) == 0) 
        {
             port_map /= 2;
             port_num++;
        }
    
        devInfo->uniToPhysicalPortMap[i] = port_num;
        devInfo->portCfg[port_num].portConfig |= TkOnuEthPortCfgEnable;
        if(devCtl_getPortMacType(port_num, &mac_type) == CMSRET_SUCCESS)
        {
             switch(mac_type)
            {
                case MAC_IF_GMII:
                case MAC_IF_RGMII:
                case MAC_IF_RGMII_3P3V:
                case MAC_IF_QSGMII:
                case MAC_IF_SGMII:
                case MAC_IF_HSGMII:
                     devInfo->portcapability[port_num] |= EthPort1GSpeed;
                     break;
                case MAC_IF_XFI:
                     devInfo->portcapability[port_num] |= EthPort1GSpeed|EthPort10GSpeed;
                     break;
                default:
                     break;
            }
            
        }
        else
        {
            cmsLog_error("failed to get port[%d] mac type", port_num);
        }
    }
#else
	int i=0;

	devInfo->fePortNum   = 0;
    devInfo->gePortNum   = 4;
    devInfo->uniPortNum  = 4;
	devInfo->voipPortNum = 1;

	for(i=0; i<4; i++)
	{
		devInfo->uniToPhysicalPortMap[i]  = i;
		devInfo->portCfg[i].portConfig |= TkOnuEthPortCfgEnable;
		devInfo->portcapability[i]|= EthPort1GSpeed;
	}
#endif
    
}

static void fill_dev_onu_type(EponDevInfo *devInfo)
{
#if defined(EPON_SFU)
    devInfo->onuType = OamCtcOnuSfu;
#elif defined(EPON_HGU)
    devInfo->onuType = OamCtcOnuHgu;
#else
#error
#endif

}

/*
1, When system does not support pspctl to read dpoe port type configuration or
2, When this is a SFU device and dpoe port type is not written to pspctl pad,
DPoE port can be initialized according to uni port information.
But for HGU, pspctl stored dpoe port type is mandantory for dpoe support.
3, In SFU DPOE, if voice macro enabled, there is an virtual interface for eMTAs
*/
static void fill_dpoe_port_according_to_uni_port(EponDevInfo *devInfo)
{
    U8 loop;

    devInfo->dpoePortNum = ((devInfo->uniPortNum) < MAX_NUM_DPOE_PORTS)?(devInfo->uniPortNum):MAX_NUM_DPOE_PORTS;
    for (loop = 0; loop < devInfo->dpoePortNum; loop++)
        devInfo->dpoePort[loop].portType = DpoePortTypeUnspecified;

#ifdef BRCM_PKTCBL_SUPPORT
    if((devInfo->dpoePortNum + 1) < MAX_NUM_DPOE_PORTS)
    {
        devInfo->dpoePortNum ++;
        devInfo->dpoePort[devInfo->dpoePortNum - 1].portType = DpoePortTypeMTA;
    }
#endif
}

#if !defined(DESKTOP_LINUX) && defined(BRCM_CMS_BUILD)
static void update_epon_object_ifname(void)
{
#ifdef EPON_HGU
    InstanceIdStack eponLinkIid = EMPTY_INSTANCE_ID_STACK;   
    WanEponLinkCfgObject *eponLinkCfg = NULL;
    CmsRet ret;
    InstanceIdStack eponWanIid = EMPTY_INSTANCE_ID_STACK;

    if ((ret = cmsLck_acquireLock()) != CMSRET_SUCCESS)
    {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
    }
    
    /* Get Epon WanDevice iidStack first */      
    if (dalEpon_getEponWanIidStatck(&eponWanIid) != CMSRET_SUCCESS)
    {    
       cmsLck_releaseLock();
       return;
    }
    
    /* Get WANEponLinkConfig object and fill the fixed layer 2 interface name */
    cmsLog_notice("Get EponLinkcfg");

    if (cmsObj_getNextInSubTree(MDMOID_WAN_EPON_LINK_CFG, &eponWanIid, &eponLinkIid, (void **)&eponLinkCfg) == CMSRET_SUCCESS)
    {
        CMSMEM_REPLACE_STRING_FLAGS(eponLinkCfg->ifName, EPON_WAN_IF_NAME, ALLOC_ZEROIZE); 
        if ((ret = cmsObj_set(eponLinkCfg, &eponLinkIid)) != CMSRET_SUCCESS)
        {
            cmsLog_error("could not set eponLinkCfg, ret=%d", ret);
        }
        else
        {
            cmsLog_debug("Done updating wanConnectionDevice.");
        }
        mdm_freeObject((void **) &eponLinkCfg);
    }

    cmsLck_releaseLock();
                  
#endif
}
#endif

/*
According to DPoE OAM 2.0 9.1.15.
The DPoE port information is stored in scratch pad and can be accessed by pspctl.
# pspctl dump DpoePortType
0406010602
The format is defined like: 0406010602, which means there are 4 DPoE ports on current profile.
The port type is respectively 0x06(eRouter), 0x01(eMTA), 0x06(eRouter) and 0x02(eSTB-IP).
*/
static void fill_dpoe_port(EponDevInfo *devInfo)
{

#if !defined(DESKTOP_LINUX) && defined(BRCM_CMS_BUILD)
#define DPOE_PORT_TYPE_PSP_KEY         "DpoePortType"
#define DPOE_PORT_TYPE_BUF_LEN 64


    U8 loop;
    char *p = NULL;
    char dpoePortTypeBuf[DPOE_PORT_TYPE_BUF_LEN] = {0};
    char dpoePortNumBuf[DPOE_PORT_NUM_LEN+1] = {0};
    char dpoeSinglePortTypeBuf[DPOE_PORT_TYPE_LEN+1] = {0};
    U8 dpoePortNum = 0xFF;
    U8 dpoePortType = 0xFF;
    Bool firsteRouter = FALSE;

    SINT32 count;

    count = cmsPsp_get(DPOE_PORT_TYPE_PSP_KEY, dpoePortTypeBuf, DPOE_PORT_TYPE_BUF_LEN);
    cmsLog_debug("read buffer count %d, buffer %s\n", count, dpoePortTypeBuf);

#if defined(EPON_SFU)
    if (count == 0)
    {
        cmsLog_error("SFU read DpoePortType failed, fill dpoe port type as unspecified\n\r");
        fill_dpoe_port_according_to_uni_port(devInfo);
        return;
    }
#endif
    //check if port number has been read
    if (count >= DPOE_PORT_NUM_LEN)
    {
       strncpy(dpoePortNumBuf, dpoePortTypeBuf, DPOE_PORT_NUM_LEN);
       dpoePortNum = atoi(dpoePortNumBuf);
    }

    //check if all port types are read 
    if ((count < DPOE_PORT_NUM_LEN) ||
        (dpoePortNum > MAX_NUM_DPOE_PORTS) ||
        (count < (DPOE_PORT_NUM_LEN + dpoePortNum*DPOE_PORT_TYPE_LEN)))
    {
    
        cmsLog_error("read %s fail(count=%d) or dpoePortNum(%d) not in range\n", DPOE_PORT_TYPE_PSP_KEY, count, dpoePortNum);
        return;
    }

    devInfo->dpoePortNum = dpoePortNum;
    cmsLog_debug("dpoePortNum %d\n", dpoePortNum);
    p = dpoePortTypeBuf + DPOE_PORT_NUM_LEN;
    for (loop = 0; loop < dpoePortNum; loop++)
    {
        char cmdStr[BUFLEN_64] = {};
    
        strncpy(dpoeSinglePortTypeBuf, p, DPOE_PORT_TYPE_LEN);
        cmsLog_debug("port %d dpoePortType %s\n", loop, dpoeSinglePortTypeBuf);
        
        dpoePortType = atoi(dpoeSinglePortTypeBuf);
        devInfo->dpoePort[loop].portType = (DpoePortType)dpoePortType;
        p += DPOE_PORT_TYPE_LEN; 
        if (!firsteRouter && dpoePortType == DpoePortTypeRouter)
            {
            eponVeipCmsIfIdx = loop;
            firsteRouter = TRUE;
            snprintf(cmdStr, sizeof(cmdStr), "echo veip%d>%s", eponVeipCmsIfIdx, CMS_EPON_IF_NAME_CFG);
            system(cmdStr);
            update_epon_object_ifname();
            }
    }

#else
    fill_dpoe_port_according_to_uni_port(devInfo);
#endif

    return;
}

static void fill_dev_info_def_value(EponDevInfo *devInfo)
{
    strncpy((char *)devInfo->vendorName, "BRCM", DevVendorNameLen-1);

    strncpy((char *)devInfo->modelName, "68x8SFU", DevModelNameLen-1);

    strncpy((char *)devInfo->ctcAuthId, "Admin", EponDevCtcAuthIdLen);

    strncpy((char *)devInfo->ctcAuthPass, "Admin", EponDevCtcAuthPassLen);

    devInfo->numLinks = 1; 
    devInfo->oamsel = OAM_CTC_SUPPORT;   // CTC         
    devInfo->eponMacNum = 1;
}

#ifdef BRCM_CMS_BUILD
static void read_mdm_hexstr_2_hex(char *hexStr, U8 *dst, U8 maxLen)
{
    U8* binBufPtr;
    UINT32 binBufLen;
    CmsRet cmsResult = CMSRET_SUCCESS;

    if(hexStr == NULL)
        {
        printf("NULL str\n");
        return;
        }
    cmsResult = cmsUtl_hexStringToBinaryBuf(hexStr, &binBufPtr, &binBufLen);
    if (cmsResult != CMSRET_SUCCESS)
    {
        cmsLog_error("Could not convert HEXBINARY data from string, cmsResult = %d", cmsResult);
    }
    else
    {
        // Test for NULL entry.
        printf("get len %d\n", binBufLen);
        if (binBufLen != 0)
        {
            memcpy(dst, binBufPtr, (binBufLen>maxLen)? maxLen:binBufLen);
            CMSMEM_FREE_BUF_AND_NULL_PTR(binBufPtr);        
        }       
    }
}

static void write_mdm_hex_2_hexstr(char **hexStr, U8 *src, U8 maxLen)
{
    char *str = NULL;
    CmsRet cmsResult = CMSRET_SUCCESS;

    if(*hexStr != NULL)
        {
        cmsMem_free(*hexStr);
        }

    *hexStr = cmsMem_alloc((maxLen*2)+1, ALLOC_SHARED_MEM | ALLOC_ZEROIZE);
    if(*hexStr == NULL)
        {
        printf("mem is NULL\n");
        }
    cmsResult = cmsUtl_binaryBufToHexString(src, maxLen, &str);
    if (cmsResult != CMSRET_SUCCESS)
    {
        cmsLog_error("Could not convert HEXBINARY data to string, cmsResult = %d", cmsResult);
    }
    else
    {
        memcpy(*hexStr, str, maxLen*2);
        CMSMEM_FREE_BUF_AND_NULL_PTR(str);        
    }
}
#endif

#ifdef BRCM_CMS_BUILD
#if defined(DMP_BASELINE_1)
static void fill_dev_info_from_mdm_igd(EponDevInfo *devInfo)
{
    XponObject *obj=NULL;
    IGDDeviceInfoObject *infoObj = NULL;

    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;

    if ((ret = cmsLck_acquireLock()) != CMSRET_SUCCESS)
    {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
    }

    /* Get from IGD DEVICE INFO */   
    if ((ret = cmsObj_get(MDMOID_IGD_DEVICE_INFO, &iidStack, 0, (void **) &infoObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("get of MDMOID_IGD_DEVICE_INFO object failed, ret=%d", ret);
        cmsLck_releaseLock();
        return;
    }

   if (infoObj->manufacturer != NULL)
   {
      strncpy((char *)devInfo->vendorName, infoObj->manufacturer, DevVendorNameLen-1);
   }

   if (infoObj->modelName != NULL)
   {
      strncpy((char *)devInfo->modelName, infoObj->modelName, DevModelNameLen-1);
   }

   if (infoObj->serialNumber != NULL)
   {
      strncpy((char *)devInfo->dpoeVendor, infoObj->serialNumber, DpoeVendorInfoLen);
   }

   if (infoObj->hardwareVersion != NULL)
   {
      strncpy((char *)devInfo->devHwVer, infoObj->hardwareVersion, DevHwVerLen-1);
   }
   cmsObj_free((void **) &infoObj);

   /* Get from XPON */ 
   memset(&iidStack, 0, sizeof(iidStack));   
   if ((ret = cmsObj_get(MDMOID_XPON, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("get of MDMOID_XPON object failed, ret=%d", ret);
      cmsLck_releaseLock();
      return;
   }

   if(obj->authenticationId != NULL)
   {
      strncpy((char *)devInfo->ctcAuthId,
            obj->authenticationId, EponDevCtcAuthIdLen);
   }

   if(obj->authenticationPassword != NULL)
   {
      strncpy((char *)devInfo->ctcAuthPass,
            obj->authenticationPassword, EponDevCtcAuthPassLen);
   }

   if(obj->dpoeFileName != NULL)
   {
      strncpy((char *)devInfo->dpoeFileName,
            obj->dpoeFileName, DpoeFileNameLen);
   }

   devInfo->numLinks = obj->maxLinkNum;
   devInfo->oamsel = obj->oamSelection;
   devInfo->schMode = obj->eponSchMode;
   devInfo->idleTimeOffset = obj->eponIdleTimeOffset;
   devInfo->imageCrc = obj->imageCrc;
   devInfo->imageCrcBeforeCommit = obj->imageCrcBeforeCommit;
   devInfo->txLaserPowerOff = obj->eponTxLaserPowerOff;
   devInfo->failSafe = obj->failSafe;

   read_mdm_hexstr_2_hex(obj->date, devInfo->date, DateInfoLen);

   read_mdm_hexstr_2_hex(obj->dpoeMfgTime, devInfo->dpoeMfgTime, DpoeMfgTimeLen);

   if (obj->extendedId != NULL)
   {
      strncpy((char *)devInfo->extendedId,
            obj->extendedId, EponDevExtendedIdLen);
   }

   cmsObj_free((void **) &obj);
   cmsLck_releaseLock();
}
#endif

#if defined(DMP_DEVICE2_BASELINE_2)
static void fill_dev_info_from_mdm_dev2(EponDevInfo *devInfo)
{
   XponObject *obj=NULL;
   IGDDeviceInfoObject *infoObj = NULL;

   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   if ((ret = cmsLck_acquireLock()) != CMSRET_SUCCESS)
   {
      cmsLog_error("failed to get lock, ret=%d", ret);
      return;
   }

    /* Get from DEV2 DEVICE INFO */   
    if ((ret = cmsObj_get(MDMOID_DEV2_DEVICE_INFO, &iidStack, 0, (void **) &infoObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("get of MDMOID_DEV2_DEVICE_INFO object failed, ret=%d", ret);
        cmsLck_releaseLock();
        return;
    }

    if (infoObj->manufacturer != NULL)
    {
        strncpy((char *)devInfo->vendorName, infoObj->manufacturer, DevVendorNameLen-1);
    }

    if (infoObj->modelName != NULL)
    {
        strncpy((char *)devInfo->modelName, infoObj->modelName, DevModelNameLen-1);
    }

    if (infoObj->serialNumber != NULL)
    {
        strncpy((char *)devInfo->dpoeVendor, infoObj->serialNumber, DpoeVendorInfoLen);
    }

    if (infoObj->hardwareVersion != NULL)
    {
        strncpy((char *)devInfo->devHwVer, infoObj->hardwareVersion, DevHwVerLen-1);
    }
    cmsObj_free((void **) &infoObj);

    /* Get from XPON */ 
    memset(&iidStack, 0, sizeof(iidStack));   
    if ((ret = cmsObj_get(MDMOID_XPON, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
    {
       cmsLog_error("get of MDMOID_XPON object failed, ret=%d", ret);
       cmsLck_releaseLock();
       return;
    }

    if(obj->authenticationId != NULL)
    {
        strncpy((char *)devInfo->ctcAuthId,
                 obj->authenticationId, EponDevCtcAuthIdLen);
    }

    if(obj->authenticationPassword != NULL)
    {
        strncpy((char *)devInfo->ctcAuthPass,
                obj->authenticationPassword, EponDevCtcAuthPassLen);
    }

    if(obj->dpoeFileName != NULL)
    {
        strncpy((char *)devInfo->dpoeFileName,
                obj->dpoeFileName, DpoeFileNameLen);
    }

    devInfo->numLinks = obj->maxLinkNum;
    devInfo->oamsel = obj->oamSelection;
    devInfo->schMode = obj->eponSchMode;
    devInfo->idleTimeOffset = obj->eponIdleTimeOffset;
   devInfo->imageCrc = obj->imageCrc;
   devInfo->imageCrcBeforeCommit = obj->imageCrcBeforeCommit;
    devInfo->txLaserPowerOff = obj->eponTxLaserPowerOff;
    devInfo->failSafe = obj->failSafe;

    read_mdm_hexstr_2_hex(obj->date, devInfo->date, DateInfoLen);

    read_mdm_hexstr_2_hex(obj->dpoeMfgTime, devInfo->dpoeMfgTime, DpoeMfgTimeLen);

    if (obj->extendedId != NULL)
    {
        strncpy((char *)devInfo->extendedId,
            obj->extendedId, EponDevExtendedIdLen);
    }
         
    cmsObj_free((void **) &obj);
    cmsLck_releaseLock();
}
#endif
#endif


static void fill_dev_info_from_mdm(EponDevInfo *devInfo)
{
#ifdef BRCM_CMS_BUILD
#if defined(SUPPORT_DM_LEGACY98)
   fill_dev_info_from_mdm_igd(devInfo);
#elif defined(SUPPORT_DM_HYBRID)
   fill_dev_info_from_mdm_igd(devInfo);
#elif defined(SUPPORT_DM_PURE181)
   fill_dev_info_from_mdm_dev2(devInfo);
#elif defined(SUPPORT_DM_DETECT)
   if (cmsMdm_isDataModelDevice2())
   {
      fill_dev_info_from_mdm_dev2(devInfo);
   }
   else
   {
      fill_dev_info_from_mdm_igd(devInfo);
   }
#endif
#endif    
}

#if !defined(BCM_OAM_BUILD)
static int epon_invalidate(int action)
{
   char buf[1024];
   BOARD_IOCTL_PARMS ioctlParms;
   int fd;
   int rv;

   /*
    * CMS code will do a cmsImg_getRealConfigFlashSize and malloc a
    * buffer of that length and fill with zero's and write.  Here, we just
    * write 1KB of zeros.  Maybe later we can have a "full invalidate" option.
    */
   memset(buf, 0, sizeof(buf));

   cmsLog_debug("writing %d bytes of zeros to %s",
                sizeof(buf),
                ((action == PERSISTENT) ? "PRIMARY_PSI" : "BACKUP_PSI"));

   fd = open(BOARD_DEVICE_NAME, O_RDWR);
   if (fd < 0)
   {
      fprintf(stderr, "could not open %s, errno=%d", BOARD_DEVICE_NAME, errno);
   }

   memset(&ioctlParms, 0, sizeof(ioctlParms));
   ioctlParms.string = buf;
   ioctlParms.strLen = sizeof(buf);
   ioctlParms.action = action;

   rv = ioctl(fd, BOARD_IOCTL_FLASH_WRITE, &ioctlParms);

   cmsLog_debug("rv=%d result=%d", rv, ioctlParms.result);
   if (rv != 0)
   {
      fprintf(stderr, "invalidate failed, rv=%d result=%d errno=%d\n",
              rv, ioctlParms.result, errno);
      if (action == BACKUP_PSI)
      {
         fprintf(stderr, "check that BACKUP_PSI is enabled in CFE\n");
      }
   }

   close(fd);

   return rv;
}

static int epon_getPsiSize(int ioc)
{
   BOARD_IOCTL_PARMS ioctlParms;
   int fd;
   int rv;

   fd = open(BOARD_DEVICE_NAME, O_RDWR);
   if (fd < 0)
   {
      fprintf(stderr, "could not open %s, errno=%d", BOARD_DEVICE_NAME, errno);
   }

   memset(&ioctlParms, 0, sizeof(ioctlParms));

   rv = ioctl(fd, ioc, &ioctlParms);

   cmsLog_debug("rv=%d result=%d", rv, ioctlParms.result);

   close(fd);

   if (rv < 0)
   {
      fprintf(stderr, "BOARD_IOCTL_GET_PSI_SIZE failed, rc=%d errno=%d\n", rv, errno);
      if (ioc == BOARD_IOCTL_GET_BACKUP_PSI_SIZE)
      {
         fprintf(stderr, "check that BACKUP_PSI is enabled in CFE\n");
      }

      return rv;
   }

   return ioctlParms.result;
}
static int epon_copyprimary2backup(void)
{
   int primarysize, backupsize;
   char *buf;
   BOARD_IOCTL_PARMS ioctlParms;
   int fd;
   int rv;

   primarysize = epon_getPsiSize(BOARD_IOCTL_GET_PSI_SIZE);
   backupsize = epon_getPsiSize(BOARD_IOCTL_GET_BACKUP_PSI_SIZE);

   cmsLog_debug("primarysize=%d backupsize=%d", primarysize, backupsize);

   if (backupsize <= 0)
   {
      fprintf(stderr, "backup PSI size is %d!!\n", backupsize);
      fprintf(stderr, "check that BACKUP_PSI is enabled in CFE\n");
      return -1;
   }

   if (primarysize > backupsize)
   {
      /* these sizes are the sizes of the storage area, not the actual
       * amount of data in them.  The device driver does not report the
       * actual number of valid data read because it does not know what is
       * valid or not.
       */
      fprintf(stderr, "primary psi (%d) is bigger than backup psi (%d)!\n",
              primarysize, backupsize);
      return -1;
   }

   buf = malloc(primarysize);
   if (buf == NULL)
   {
      fprintf(stderr, "malloc of %d bytes failed\n", primarysize);
      return -1;
   }
   memset(buf, 0, primarysize);

   cmsLog_debug("reading %d bytes from primary", primarysize);

   /* read primary */
   fd = open(BOARD_DEVICE_NAME, O_RDWR);
   if (fd < 0)
   {
      fprintf(stderr, "could not open %s, errno=%d", BOARD_DEVICE_NAME, errno);
      free(buf);
      return -1;
   }

   memset(&ioctlParms, 0, sizeof(ioctlParms));
   ioctlParms.string = (char *) buf;
   ioctlParms.strLen = primarysize;
   ioctlParms.action = PERSISTENT;

   rv = ioctl(fd, BOARD_IOCTL_FLASH_READ, &ioctlParms);

   cmsLog_debug("read rv=%d result=%d", rv, ioctlParms.result);
   if (rv < 0)
   {
      fprintf(stderr, "read failed, rv=%d errno=%d\n", rv, errno);
      free(buf);
      close(fd);
      return -1;
   }

   /* write backup */
   ioctlParms.string = (char *) buf;
   ioctlParms.strLen = primarysize;
   ioctlParms.action = BACKUP_PSI;

   rv = ioctl(fd, BOARD_IOCTL_FLASH_WRITE, &ioctlParms);

   cmsLog_debug("write rv=%d result=%d", rv, ioctlParms.result);

   free(buf);
   close(fd);

   return rv;
}
#endif

static void save_dev_info_to_mdm(Bool noSaveInNonTek)
    {
#ifdef BRCM_CMS_BUILD
    XponObject *obj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;

    if ((ret = cmsLck_acquireLock()) != CMSRET_SUCCESS)
    {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
    }
 
    if ((ret = cmsObj_get(MDMOID_XPON, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
    {
       cmsLog_error("get of MDMOID_XPON object failed, ret=%d", ret);
       cmsLck_releaseLock();
       return;
    }

    obj->maxLinkNum = eponDevInfo.numLinks;
    obj->oamSelection = eponDevInfo.oamsel;
    obj->eponSchMode = eponDevInfo.schMode;
    obj->eponIdleTimeOffset = eponDevInfo.idleTimeOffset;
    obj->eponTxLaserPowerOff = eponDevInfo.txLaserPowerOff;
    obj->failSafe = eponDevInfo.failSafe;


    write_mdm_hex_2_hexstr(&obj->date, eponDevInfo.date, DateInfoLen);

    write_mdm_hex_2_hexstr(&obj->dpoeMfgTime, eponDevInfo.dpoeMfgTime, DpoeMfgTimeLen);

    if(strlen((char*)eponDevInfo.dpoeFileName) != 0)
    {
        if(obj->dpoeFileName != NULL)
        {
            CMSMEM_FREE_BUF_AND_NULL_PTR(obj->dpoeFileName);
        }
        obj->dpoeFileName = cmsMem_alloc(DpoeFileNameLen, ALLOC_ZEROIZE);        
        memcpy(obj->dpoeFileName, eponDevInfo.dpoeFileName, DpoeFileNameLen);
    }

    if(strlen((char*)eponDevInfo.ctcAuthId) != 0)
    {
        if(obj->authenticationId != NULL)
        {
            CMSMEM_FREE_BUF_AND_NULL_PTR(obj->authenticationId);
        }
        obj->authenticationId = cmsMem_alloc(EponDevCtcAuthIdLen, ALLOC_ZEROIZE);        
        memcpy(obj->authenticationId, eponDevInfo.ctcAuthId, EponDevCtcAuthIdLen);
    }

    if(strlen((char*)eponDevInfo.ctcAuthPass) != 0)
    {
        if(obj->authenticationPassword != NULL)
        {
            CMSMEM_FREE_BUF_AND_NULL_PTR(obj->authenticationPassword);
        }
        obj->authenticationPassword = cmsMem_alloc(EponDevCtcAuthPassLen, ALLOC_ZEROIZE);        
        memcpy(obj->authenticationPassword, eponDevInfo.ctcAuthPass, EponDevCtcAuthPassLen);
    }
    
    if(strlen((char*)eponDevInfo.extendedId) != 0)
    {
        if(obj->extendedId != NULL)
        {
            CMSMEM_FREE_BUF_AND_NULL_PTR(obj->extendedId);
        }
        obj->extendedId = cmsMem_alloc(EponDevExtendedIdLen, ALLOC_ZEROIZE);        
        memcpy(obj->extendedId, eponDevInfo.extendedId, EponDevExtendedIdLen);
    }

    ret = cmsObj_set(obj, &iidStack);
    if( ret != CMSRET_SUCCESS)
        printf("set cms error (%d)\n", ret);

    ret = cmsMgm_saveConfigToFlash();
    if (ret != CMSRET_SUCCESS)
    {
       printf("Could not save config to flash, ret=%d\n", ret);
    }

    cmsObj_free((void **) &obj);
    cmsLck_releaseLock();  
    
#if !defined(BCM_OAM_BUILD)
    if (noSaveInNonTek)
    {
        /* copy to backup PSI */
        (void)epon_copyprimary2backup();
        /* invalidate primary PSI */
        (void)epon_invalidate(PERSISTENT);
    }
#endif
#endif
}

void printfDevInfo(void)
{
    int i;

    printf("DevInfo:\n");
    printf("OamSelection:\t%d\n", eponDevInfo.oamsel);
    printf("SchMode:\t%d\n", eponDevInfo.schMode); 
    printf("IdleTimeOffset:\t%d\n", eponDevInfo.idleTimeOffset);   
    printf("MaxLinks:\t%d\n", eponDevInfo.numLinks);
    printf("CtcAuthId:\t%s\n", eponDevInfo.ctcAuthId);
    printf("CtcAuthPass:\t%s\n", eponDevInfo.ctcAuthPass);
    printf("DpoeFileName:\t%s\n", eponDevInfo.dpoeFileName);
    
    printf("Date:\t\t%02x %02x %02x %02x\n", 
        eponDevInfo.date[0], eponDevInfo.date[1],
        eponDevInfo.date[2], eponDevInfo.date[3]);

    printf("DpoeMfgTime:\n");

    for (i = 0; i < DpoeMfgTimeLen; ++i)
    {
        if ((i % 13) == 0)
        {
            printf("\n");
        }
        printf("%02x ", eponDevInfo.dpoeMfgTime[i]);
    }
    printf("\n");         

    printf("Vendor:\n");

    for (i = 0; i < DpoeVendorInfoLen; ++i)
    {
        if ((i % 16) == 0)
        {
            printf("\n");
        }
        printf("%02x ", eponDevInfo.dpoeVendor[i]);
    }
    printf("\n");         
}

BOOL EponDevSetMaxLinks(U16 maxLinks)
{
    if(maxLinks > TkOnuNumTxLlids)
    {
        return FALSE;
    }

    eponDevInfo.numLinks = maxLinks;

    save_dev_info_to_mdm(TRUE);
    return TRUE;
}

BOOL EponDevSetMfgTime(U8 *data, U16 len)
{
    if(len > DpoeMfgTimeLen)
    {
        return FALSE;
    }

    memcpy(eponDevInfo.dpoeMfgTime, data, len);

    save_dev_info_to_mdm(TRUE);
    return TRUE;
}

U16 EponDevGetMfgTime(U8 *data, U16 maxLen)
{
    U16 len;

    len = (maxLen > DpoeMfgTimeLen)?DpoeMfgTimeLen:maxLen;

    memcpy(data, eponDevInfo.dpoeMfgTime, len);
    return len;
}

void EponDevSetFileName(U8 *data, U16 len)
{
    U16 maxLen;

    maxLen = (len > DpoeFileNameLen)?DpoeFileNameLen:len;
    memset(eponDevInfo.dpoeFileName, 0, DpoeFileNameLen+1);
    memcpy(eponDevInfo.dpoeFileName, data, maxLen);
    
    save_dev_info_to_mdm(TRUE);
    return ;
}

U16 EponDevGetFileName(U8 *data, U16 maxLen)
{
    U16 len = strlen((char *)eponDevInfo.dpoeFileName);

    if(len != 0)
    {
        len = (maxLen > len)?len:maxLen;
        strncpy((char *)data, (char *)eponDevInfo.dpoeFileName, len);
    }

    return len;
}

U8 EponDevGetOamSel(void)
{
    return eponDevInfo.oamsel;
}

U8 EponDevGetEponMacNum(void)
{
    return eponDevInfo.eponMacNum;
}

void EponDevSetSchMode(U8 mode)
{
    eponDevInfo.schMode = mode;
    save_dev_info_to_mdm(TRUE);
    return;
}

U8 EponDevGetSchMode(void)
{
    return eponDevInfo.schMode;
}

void EponDevSetIdleTimeOffset(U8 offset)
{
    eponDevInfo.idleTimeOffset = offset;
    save_dev_info_to_mdm(TRUE);
    return;
}

U8 EponDevGetIdleTimeOffset(void)
{
    return eponDevInfo.idleTimeOffset;
}

void EponDevSetTxLaserPowerOff(Bool powerOff)
{
    eponDevInfo.txLaserPowerOff = powerOff;
    save_dev_info_to_mdm(FALSE);
    return;
}

Bool EponDevGetTxLaserPowerOff(void)
{
    return eponDevInfo.txLaserPowerOff;
}

void EponDevSetFailSafe(Bool enable)
{
    eponDevInfo.failSafe = enable;
    save_dev_info_to_mdm(FALSE);
    return;
}

U8 EponDevGetDnPonRate(void)
{
    return eponDevInfo.dnPonRate;
}

U8 EponDevGetUpPonRate(void)
{
    return eponDevInfo.upPonRate;
}

Bool EponDevGetFailSafe(void)
{
    return eponDevInfo.failSafe;
}

static void save_image_crc_to_mdm(void)
    {
#ifdef BRCM_CMS_BUILD
    XponObject *obj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;

    if ((ret = cmsLck_acquireLock()) != CMSRET_SUCCESS)
    {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
    }
 
    if ((ret = cmsObj_get(MDMOID_XPON, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
    {
       cmsLog_error("get of MDMOID_XPON object failed, ret=%d", ret);
       cmsLck_releaseLock();
       return;
    }

    obj->imageCrc = eponDevInfo.imageCrc;

    ret = cmsObj_set(obj, &iidStack);
    if( ret != CMSRET_SUCCESS)
        printf("set cms error (%d)\n", ret);

    ret = cmsMgm_saveConfigToFlash();
    if (ret != CMSRET_SUCCESS)
    {
       printf("Could not save config to flash, ret=%d\n", ret);
    }

    cmsObj_free((void **) &obj);
    cmsLck_releaseLock();  
#endif
}

void EponDevSetImageCrc(U32 imageCrc)
{
    eponDevInfo.imageCrc = imageCrc;
    save_image_crc_to_mdm();
    return ;
}

U32 EponDevGetImageCrc(void)
{
    return eponDevInfo.imageCrc;
}

static void save_image_crc_before_commit_to_mdm(void)
    {
#ifdef BRCM_CMS_BUILD
    XponObject *obj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;

    if ((ret = cmsLck_acquireLock()) != CMSRET_SUCCESS)
    {
       cmsLog_error("failed to get lock, ret=%d", ret);
       return;
    }
 
    if ((ret = cmsObj_get(MDMOID_XPON, &iidStack, 0, (void **) &obj)) != CMSRET_SUCCESS)
    {
       cmsLog_error("get of MDMOID_XPON object failed, ret=%d", ret);
       cmsLck_releaseLock();
       return;
    }

    obj->imageCrcBeforeCommit = eponDevInfo.imageCrcBeforeCommit;

    ret = cmsObj_set(obj, &iidStack);
    if( ret != CMSRET_SUCCESS)
        printf("set cms error (%d)\n", ret);

    ret = cmsMgm_saveConfigToFlash();
    if (ret != CMSRET_SUCCESS)
    {
       printf("Could not save config to flash, ret=%d\n", ret);
    }

    cmsObj_free((void **) &obj);
    cmsLck_releaseLock(); 
#endif
}

void EponDevSetImageCrcBeforeCommit(U32 imageCrc)
{
    eponDevInfo.imageCrcBeforeCommit = imageCrc;
    save_image_crc_before_commit_to_mdm();
    return ;
}

U32 EponDevGetImageCrcBeforeCommit(void)
{
    return eponDevInfo.imageCrcBeforeCommit;
}

EponDevInfo *EponDevInfoInit(void)
{
    memset(&eponDevInfo, 0, sizeof(eponDevInfo));
    fill_dev_info_def_value(&eponDevInfo);
    fill_dev_info_from_mdm(&eponDevInfo);
    fill_dev_extendedId(&eponDevInfo);
    fill_dev_chipId(&eponDevInfo);
    fill_dev_base_mac(&eponDevInfo);
    fill_dev_uni_port_param(&eponDevInfo);
    fill_dev_onu_type(&eponDevInfo);
    
    if((EponDevGetOamSel() == OAM_DPOE_SUPPORT))    
        fill_dpoe_port(&eponDevInfo);

    return &eponDevInfo;
}


////////////////////////////////////////////////////////////////////////////////
/// PersInit:       Initialize MPCP stack for OAM stack 
///
/// Parameters:
///
/// \return 
/// TRUE on sucess
/// FALSE on failure
//
////////////////////////////////////////////////////////////////////////////////
//extern
BOOL EponDevInit(void)
{    
    EponCfgParam eponcfgparm;
    char eponRateBuf[PSP_BUFLEN_16]={0};
    U8 dnRate = 0xFF;
    U8 upRate = 0xFF;
    SINT32 count;;

    memset(&eponcfgparm,0,sizeof(eponcfgparm));
    eponcfgparm.index = EponCfgItemPonMac;
    memcpy(&eponcfgparm.eponparm.ponMac, &eponDevInfo.resbaseEponMac, sizeof(MacAddr));
    eponStack_CtlCfgEponCfg(EponSetOpe, &eponcfgparm);

    memset(&eponcfgparm,0,sizeof(eponcfgparm));
    eponcfgparm.index = EponCfgItemOamSel;
    memcpy(&eponcfgparm.eponparm.oamsel, &eponDevInfo.oamsel, sizeof(eponDevInfo.oamsel));
    eponStack_CtlCfgEponCfg(EponSetOpe, &eponcfgparm);
    
#ifdef BRCM_CMS_BUILD
#define RATE_STR_LEN 2
#define PSP_RATE_STR_LEN 4
    count = cmsPsp_get(RDPA_WAN_RATE_PSP_KEY, eponRateBuf, sizeof(eponRateBuf));
    if (count >= PSP_RATE_STR_LEN)
    {
        if (!strncasecmp(eponRateBuf, RDPA_WAN_RATE_10G, RATE_STR_LEN))
        {
            dnRate = LaserRate10G;
        }
        else if(!strncasecmp(eponRateBuf, RDPA_WAN_RATE_2_5G, RATE_STR_LEN))
        {
            dnRate = LaserRate2G;
        }
        else if(!strncasecmp(eponRateBuf, RDPA_WAN_RATE_1G, RATE_STR_LEN))
        {
            dnRate = LaserRate1G;
        }
    
        if (!strncasecmp(&eponRateBuf[RATE_STR_LEN], RDPA_WAN_RATE_10G, RATE_STR_LEN))
        {
            upRate = LaserRate10G;
        }
        else if (!strncasecmp(&eponRateBuf[RATE_STR_LEN], RDPA_WAN_RATE_1G, RATE_STR_LEN))
        {
            upRate = LaserRate1G;
        }
    }
    else
    {   
        count = cmsPsp_get(RDPA_EPON_SPEED_PSP_KEY, eponRateBuf, sizeof(eponRateBuf));
        if (count > 0)
        {
            upRate = 1;
            if (!strcasecmp(eponRateBuf, EPON_SPEED_TURBO))
            {
                dnRate = LaserRate2G;
            }
            else
            {
                dnRate = LaserRate1G;
            }
    	}
    }
#endif    
    /* if now additional set, pass 0xFF to driver to use default rate that get from chip type. */
    memset(&eponcfgparm,0,sizeof(eponcfgparm));
    eponcfgparm.index = EponCfgItemPortDnRate;
    memcpy(&eponcfgparm.eponparm.dnRate, &dnRate, sizeof(dnRate));
    eponStack_CtlCfgEponCfg(EponSetOpe, &eponcfgparm);
    eponDevInfo.dnPonRate = (dnRate != 0xFF) ? dnRate : LaserRate1G;

    memset(&eponcfgparm,0,sizeof(eponcfgparm));
    eponcfgparm.index = EponCfgItemPortUpRate;
    memcpy(&eponcfgparm.eponparm.upRate, &upRate, sizeof(upRate));
    eponStack_CtlCfgEponCfg(EponSetOpe, &eponcfgparm);
    eponDevInfo.upPonRate = (upRate != 0xFF) ? upRate : LaserRate1G;

    memset(&eponcfgparm,0,sizeof(eponcfgparm));
    eponcfgparm.index = EponCfgItemSchMode;
    memcpy(&eponcfgparm.eponparm.schMode, 
           &eponDevInfo.schMode, sizeof(eponDevInfo.schMode));
    eponStack_CtlCfgEponCfg(EponSetOpe, &eponcfgparm);

    memset(&eponcfgparm,0,sizeof(eponcfgparm));
    eponcfgparm.index = EponCfgItemIdileTimOffset;
    memcpy(&eponcfgparm.eponparm.idleTimeOffset, 
           &eponDevInfo.idleTimeOffset, sizeof(eponDevInfo.idleTimeOffset));
    eponStack_CtlCfgEponCfg(EponSetOpe, &eponcfgparm);
    
    eponStack_CtlCfgFailSafe(EponSetOpe, &eponDevInfo.failSafe);
    
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
/// EponDevInfoExtendedId: get extended ID information
///
/// The extended ID is a 64-byte section of user-defined memory intended
/// to hold information such as a serial number, product code, date of
/// manufacture, etc.
///
 // Parameters:
/// \param None
///
/// \return
/// Pointer to where the extended ID information is stored
////////////////////////////////////////////////////////////////////////////////
//extern
U8 const* EponDevInfoExtendedId (void)
{
    return eponDevInfo.extendedId;
} 


// end of Personality.c
