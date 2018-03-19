
/***********************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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

#ifdef DMP_DEVICE2_USBHOSTSBASIC_1

#include<stdio.h>
#include<sys/vfs.h>
#include<dirent.h>
#include<linux/unistd.h>
#include <errno.h>

#include "cms.h"
#include "cms_obj.h"
#include "cms_util.h"
#include "cms_core.h"
#include "ssk.h"

static UBOOL8 getUsbHostObjByNameNum(UINT8 hostNum, Dev2UsbHostObject **usbHostObj, InstanceIdStack *usbHostIidStack);
static UBOOL8 getUsbHostDeviceObjByDeviceKey(char *deviceKey, Dev2UsbHostDeviceObject **usbHostDeviceObj, InstanceIdStack *usbHostDeviceIidStack);
static void setUsbHostDeviceObj(char *deviceKey, Dev2UsbHostDeviceObject *usbHostDeviceObj, InstanceIdStack *usbHostIidStack);
static CmsRet processAddUsbDeviceMsg(UINT8 hostNum, char *deviceKey);
static CmsRet processRemoveUsbDeviceMsg(char *deviceKey);

/* param (IN): hostNum
   param (OUT): usbHostObj 
   param (OUT): usbHostIidStack
   When return true, user should manually free usbHostObj     
*/
static UBOOL8 getUsbHostObjByNameNum(UINT8 hostNum, Dev2UsbHostObject **usbHostObj, InstanceIdStack *usbHostIidStack)
{
   InstanceIdStack usbIidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2UsbHostObject *usbObj=NULL;
   UBOOL8 found=FALSE;

   while (!found &&
          (cmsObj_getNext(MDMOID_DEV2_USB_HOST,
                         &usbIidStack,
                         (void **)&usbObj)) == CMSRET_SUCCESS)
   {
      if (usbObj->name && atoi(&usbObj->name[3]) == hostNum)
      {
         found = TRUE;
         if (usbHostObj)		 
         {
            *usbHostObj = usbObj;	 
         }
		 
         if (usbHostIidStack)		 
         {
            *usbHostIidStack = usbIidStack;
         }
      }
      else	  
      {
         cmsObj_free((void **) &usbObj);
      }
   }

   return found;
}

/* param (IN): deviceKey
   param (OUT): usbHostDeviceObj 
   param (OUT): usbHostDeviceIidStack
   When return true, user should manually free usbHostDeviceObj     
*/
static UBOOL8 getUsbHostDeviceObjByDeviceKey(char *deviceKey, Dev2UsbHostDeviceObject **usbHostDeviceObj, InstanceIdStack *usbHostDeviceIidStack)
{
   InstanceIdStack usbIidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2UsbHostDeviceObject *usbObj=NULL;
   UBOOL8 found=FALSE;

   while (!found &&
          (cmsObj_getNext(MDMOID_DEV2_USB_HOST_DEVICE,
                         &usbIidStack,
                         (void **)&usbObj)) == CMSRET_SUCCESS)
   {
      if (!cmsUtl_strcmp(usbObj->X_BROADCOM_COM_DeviceKey, deviceKey))
      {
         found = TRUE;
         if (usbHostDeviceObj)		 
         {
            *usbHostDeviceObj = usbObj;	 
         }
		 
         if (usbHostDeviceIidStack)		 
         {
            *usbHostDeviceIidStack = usbIidStack;
         }
      }
      else	  
      {
         cmsObj_free((void **) &usbObj);
      }
   }

   return found;
}


static void setUsbHostDeviceObj(char *deviceKey, Dev2UsbHostDeviceObject *usbHostDeviceObj, InstanceIdStack *usbHostIidStack)
{
   char usbDirName[BUFLEN_64];
   char buf[BUFLEN_64];
   char *p;
   char *usbRefFullPath=NULL;
   MdmPathDescriptor pathDesc = EMPTY_PATH_DESCRIPTOR;

   snprintf(usbDirName, sizeof(usbDirName), "/sys/bus/usb/devices/%s", deviceKey);
   cmsLog_debug("usbDirName = %s", usbDirName);
   
   CMSMEM_REPLACE_STRING(usbHostDeviceObj->X_BROADCOM_COM_DeviceKey, deviceKey);

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "devnum", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      usbHostDeviceObj->deviceNumber = atoi(buf);
   }
   
   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "version", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceObj->USBVersion, &buf[1]);
   }

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "bDeviceClass", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceObj->deviceClass, buf);
   }

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "bDeviceSubClass", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceObj->deviceSubClass, buf);
   }

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "bDeviceProtocol", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceObj->deviceProtocol, buf);
   }

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "idProduct", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      sscanf(buf, "%x", &usbHostDeviceObj->productID);   
   }

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "idVendor", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      sscanf(buf, "%x", &usbHostDeviceObj->vendorID);   
   }

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "manufacturer", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceObj->manufacturer, buf);
   }
   
   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "product", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceObj->productClass, buf);
   }

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "serial", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceObj->serialNumber, buf);
   }

   if ((p = strchr(deviceKey, '.')) || (p = strchr(deviceKey, '-')))
   {
      usbHostDeviceObj->port = atoi(p+1);
   }

   pathDesc.oid = MDMOID_DEV2_USB_HOST;
   pathDesc.iidStack = *usbHostIidStack;
   if (cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &usbRefFullPath) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not form fullpath to %d %s", pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack));
   }
   else   
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceObj->USBPort, usbRefFullPath);
      CMSMEM_FREE_BUF_AND_NULL_PTR(usbRefFullPath);
   }

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "speed", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceObj->rate, buf);
   }


   if ((p = strchr(deviceKey, '.')))
   {
      char parentDeviceKey[BUFLEN_32];
      Dev2UsbHostDeviceObject *parentUsbHostDeviceObj;
      InstanceIdStack parentUsbHostDeviceIidStack;

      strncpy(parentDeviceKey, deviceKey, sizeof(parentDeviceKey));
      p = strchr(parentDeviceKey, '.');
      *p = '\0';
      if (getUsbHostDeviceObjByDeviceKey(parentDeviceKey, &parentUsbHostDeviceObj, &parentUsbHostDeviceIidStack))
      {
         pathDesc.oid = MDMOID_DEV2_USB_HOST_DEVICE;
         pathDesc.iidStack = parentUsbHostDeviceIidStack;
         if (cmsMdm_pathDescriptorToFullPathNoEndDot(&pathDesc, &usbRefFullPath) != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not form fullpath to %d %s", pathDesc.oid, cmsMdm_dumpIidStack(&pathDesc.iidStack));
         }
         else   
         {
            CMSMEM_REPLACE_STRING(usbHostDeviceObj->parent, usbRefFullPath);
            CMSMEM_FREE_BUF_AND_NULL_PTR(usbRefFullPath);
         }
      }
   }
   
   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "maxchild", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      usbHostDeviceObj->maxChildren = atoi(buf);
   }

#ifdef DMP_DEVICE2_USBHOSTSADV_1
   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "bmAttributes", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      UINT32 bmAttributes;    
      sscanf(buf, "%x", &bmAttributes);   
      usbHostDeviceObj->isSelfPowered = (bmAttributes & 0x40) ? TRUE: FALSE;
   }
#endif
}


#ifdef DMP_DEVICE2_USBHOSTSADV_1
static void setUsbHostDeviceConfigIfcObj(char *deviceKey, UINT32 configNum, UINT32 interfaceNum, Dev2UsbHostDeviceConfigIfcObject *usbHostDeviceIfcObj)
{
   char usbDirName[BUFLEN_64];
   char buf[BUFLEN_64];

   snprintf(usbDirName, sizeof(usbDirName), "/sys/bus/usb/devices/%s:%d.%d", deviceKey, configNum, interfaceNum);
   cmsLog_debug("usbDirName = %s", usbDirName);
   
   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "bInterfaceNumber", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      usbHostDeviceIfcObj->interfaceNumber = atoi(buf);
   }
   
   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "bInterfaceClass", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceIfcObj->interfaceClass, buf);
   }

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "bInterfaceSubClass", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceIfcObj->interfaceSubClass, buf);
   }

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "bInterfaceProtocol", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      CMSMEM_REPLACE_STRING(usbHostDeviceIfcObj->interfaceProtocol, buf);
   }


}
#endif


static CmsRet processAddUsbDeviceMsg(UINT8 hostNum, char *deviceKey)
{
   Dev2UsbHostObject *usbHostObj;
   Dev2UsbHostDeviceObject *usbHostDeviceObj;
   InstanceIdStack usbHostIidStack;
   InstanceIdStack usbHostDeviceIidStack;
   CmsRet ret;
#ifdef DMP_DEVICE2_USBHOSTSADV_1
   UINT32 i, j; 
   UINT32 bNumConfigurations = 0, bNumInterfaces = 0;
   Dev2UsbHostDeviceConfigIfcObject *usbHostDeviceIfcObj;
   InstanceIdStack usbHostDeviceConfigIidStack;
   InstanceIdStack usbHostDeviceInterfaceIidStack;
   char usbDirName[BUFLEN_64];
   char buf[BUFLEN_64];
#endif
   
   if (!getUsbHostObjByNameNum(hostNum, &usbHostObj, &usbHostIidStack))
   {
      cmsLog_error("Failed to get USB_HOST_OBJ %d", hostNum);
      return CMSRET_INVALID_ARGUMENTS;
   }

   
   if (getUsbHostDeviceObjByDeviceKey(deviceKey, &usbHostDeviceObj, &usbHostDeviceIidStack))
   {
      cmsLog_notice("USB_HOST_DEVICE alreay exist, do remove then add");
      cmsObj_free((void **) &usbHostDeviceObj);
      processRemoveUsbDeviceMsg(deviceKey);
   } 

   usbHostDeviceIidStack = usbHostIidStack;
   
   if((ret=cmsObj_addInstance(MDMOID_DEV2_USB_HOST_DEVICE, &usbHostDeviceIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not add USB_HOST_DEVICE object, ret=%d", ret);
      cmsObj_free((void **) &usbHostObj);
      return ret;
   }
 
   if ((ret = cmsObj_get(MDMOID_DEV2_USB_HOST_DEVICE, &usbHostDeviceIidStack, 0, (void **) &usbHostDeviceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to get USB_HOST_DEVICE, ret = %d", ret);
      cmsObj_free((void **) &usbHostObj);
      return ret;
   }

   setUsbHostDeviceObj(deviceKey, usbHostDeviceObj, &usbHostIidStack);

   if(( ret =cmsObj_set(usbHostDeviceObj, &usbHostDeviceIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set USB_HOST_DEVICE object, ret=%d", ret);
   } 

   cmsObj_free((void **) &usbHostDeviceObj);
   cmsObj_free((void **) &usbHostObj);
   
#ifdef DMP_DEVICE2_USBHOSTSADV_1
   snprintf(usbDirName, sizeof(usbDirName), "/sys/bus/usb/devices/%s", deviceKey);

   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "bNumConfigurations", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      bNumConfigurations = atoi(buf);
   }
   if (cmsFil_readFirstlineFromFileWithBasedir(usbDirName, "bNumInterfaces", buf, sizeof(buf)) == CMSRET_SUCCESS)
   {
      bNumInterfaces = atoi(buf);
   }

   cmsLog_debug("bNumConfigurations = %d, bNumInterfaces = %d", bNumConfigurations, bNumInterfaces);
   
   for (i = 1; i <= bNumConfigurations; i++)   
   {
      usbHostDeviceConfigIidStack = usbHostDeviceIidStack;      
	  
      if((ret=cmsObj_addInstance(MDMOID_DEV2_USB_HOST_DEVICE_CONFIG, &usbHostDeviceConfigIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not add USB_HOST_DEVICE_CONFIG object, ret=%d", ret);
         return ret;
      }
    
      for (j = 0; j < bNumInterfaces; j++)   
      {
         usbHostDeviceInterfaceIidStack = usbHostDeviceConfigIidStack;      
		 
         if((ret=cmsObj_addInstance(MDMOID_DEV2_USB_HOST_DEVICE_CONFIG_IFC, &usbHostDeviceInterfaceIidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not add USB_HOST_DEVICE_CONFIG_IFC object, ret=%d", ret);
            return ret;
         }
       
         if ((ret = cmsObj_get(MDMOID_DEV2_USB_HOST_DEVICE_CONFIG_IFC, &usbHostDeviceInterfaceIidStack, 0, (void **) &usbHostDeviceIfcObj)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Failed to get USB_HOST_DEVICE_CONFIG_IFC, ret = %d", ret);
            return ret;
         }
      
         setUsbHostDeviceConfigIfcObj(deviceKey, i, j, usbHostDeviceIfcObj);
      
         if(( ret =cmsObj_set(usbHostDeviceIfcObj, &usbHostDeviceInterfaceIidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("Could not set USB_HOST_DEVICE_CONFIG_IFC object, ret=%d", ret);
         } 
      
         cmsObj_free((void **) &usbHostDeviceIfcObj);
      }
   }
#endif

   
   return ret;

}

static CmsRet processRemoveUsbDeviceMsg(char *deviceKey)
{
   Dev2UsbHostDeviceObject *usbHostDeviceObj;
   InstanceIdStack usbHostDeviceIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   cmsLog_debug("processRemoveUsbDeviceMsg:%s", deviceKey);
   
   if (!getUsbHostDeviceObjByDeviceKey(deviceKey, &usbHostDeviceObj, &usbHostDeviceIidStack))
   {
      cmsLog_notice("USB_HOST_DEVICE not found, ignore");
      return CMSRET_SUCCESS;
   } 

  if((ret = cmsObj_deleteInstance(MDMOID_DEV2_USB_HOST_DEVICE, &usbHostDeviceIidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("could not delete USB_HOST_DEVICE, ret=%d", ret);
   } 

   cmsObj_free((void **) &usbHostDeviceObj);
   
   return ret ;
}

void processUsbDeviceStateChange(CmsMsgHeader *msg)
{
   char *usbDevName, *p;
   UINT8 hostNum;   
   CmsRet ret;

#ifdef DMP_BASELINE_1
   if (!cmsMdm_isDataModelDevice2())
   {
      cmsLog_notice("no action for TR98 datamodel");
      return;
   }
#endif

   usbDevName = (char*) (msg +1);

   if ((p = strchr(usbDevName, ':')))
   {
      *p = '\0';
   }
      
   hostNum = atoi(usbDevName);


   cmsLog_debug("wordData=%d, usbDevName=%s, hostNum =%d", msg->wordData, usbDevName, hostNum);
   
   if( (ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   if (msg->wordData)
   {
      processAddUsbDeviceMsg(hostNum, usbDevName);
   }
   else
   {
      processRemoveUsbDeviceMsg(usbDevName);
   }
	
   cmsLck_releaseLock();
   
   return;
}

#endif /* DMP_DEVICE2_USBHOSTSBASIC_1 */
