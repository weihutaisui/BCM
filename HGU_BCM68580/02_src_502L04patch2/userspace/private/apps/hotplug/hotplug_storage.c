/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
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


#include "hotplug.h"

#ifdef DMP_STORAGESERVICE_1

extern void *g_msgHandle;
void notify_smd(int disk, int partition, int msgType)
{
   CmsMsgHeader *msg;
   char *data;
   void *msgBuf;
   UINT32 msgDataLen = 0;
   CmsRet ret;
   void *msgHandle = g_msgHandle ;
   int maxRetries = 5;

   if(msgHandle == NULL)
   {
      do
      {
         if ((ret = cmsMsg_initWithFlags(EID_HOTPLUG, EIF_MULTIPLE_INSTANCES, &g_msgHandle)) == CMSRET_SUCCESS)
         {
            msgHandle = g_msgHandle;
            break; 
         }
         else
         {
            sleep(30);
            g_msgHandle = NULL;
         }

      } while(maxRetries--);

   
      if(ret != CMSRET_SUCCESS)
      {
          cprintf("cmsMsg_init error ret=%d", ret);
          return;
      }
   }
    

   msgDataLen =  sizeof(disk) + sizeof(partition);

   msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader) + msgDataLen, ALLOC_ZEROIZE);

   if (msgBuf != NULL) {
      msg = (CmsMsgHeader *)msgBuf;

      msg->type = msgType;
      msg->src = MAKE_SPECIFIC_EID(getpid(), EID_HOTPLUG);
      msg->dst = EID_SMD;
      msg->flags_event = 1;
     
       
      data = (char *) (msg + 1);
      msg->dataLength = msgDataLen;
      *(int *)data = disk;
      *((int *)data + 1) = partition;
       
      ret = cmsMsg_send(msgHandle, msg);
      /*ignore if send fails */
      cmsMem_free(msgBuf);
   }

   cmsMsg_cleanup(&msgHandle);
}

#endif

int hotplug_storage_handler(void)
{

    char *action = NULL;
    char tmp[2];
    char cmd[80];
    char *devname = NULL;
    char *devpath = NULL;

    int disk=0,partition=0,retv= -1;

    action = getenv("ACTION");
    if(!action)
    {
        HOTPLUG_ERROR("getenv(ACTION) failed");
        return EINVAL;
    }

    HOTPLUG_DEBUG("action=%s",action);

    if(!strcmp(action,"add"))  
    {
        if(!(devpath = getenv("DEVPATH")))
        {
            HOTPLUG_ERROR("getenv(ACTION) failed");
            return EINVAL;
        }

        devname = strrchr(devpath,'/');
        devname++;

        HOTPLUG_DEBUG(":mount devname =%s, devpath=%s",devname, devpath);

        /* just need 'a' '1', 'a' '2', 'b' '1' , 'b' '2' ...from sda1, sda2, sdb1,sdb2......*/
        tmp[0] = devname[2];
        tmp[1] = devname[3];

        if(!isalpha(tmp[0])) {
            HOTPLUG_ERROR("add:- illegal devname=%s",devname);
            return 0;
        }				

        disk = tmp[0] - 'a' + 1; //disk1, disk2 .......

        if(tmp[1] == '\0')
        {
            /*device for entire disk ex: sda,sdb etc..*/
#ifdef DMP_STORAGESERVICE_1
            HOTPLUG_DEBUG("notifying ssk for add physical disk=%d\n", disk);
            notify_smd(disk,0,CMS_MSG_STORAGE_ADD_PHYSICAL_MEDIUM);
#endif
            return 0;
        }

        partition = tmp[1] - '0'; //disk1_1, disk1_2....

        sprintf(cmd, "/mnt/disk%d_%d", disk, partition);
        retv = mkdir(cmd, 0777); 
        if (retv  != 0 )
        {
            return 0;
        }


        /* VFAT filesystem*/
        sprintf(cmd, "mount -t auto /dev/sd%c%d /mnt/disk%d_%d",tmp[0],partition,disk,partition);
        retv = bcmSystem(cmd);

        /*NTFS filesystem*/
        if(retv != 0) 
        {
            sprintf(cmd, "ntfs-3g /dev/sd%c%d /mnt/disk%d_%d -o use_ino,direct_io,big_writes",tmp[0],partition,disk,partition);
            retv = bcmSystem(cmd); 
        }              
        if(retv != 0)
        {
            sprintf(cmd, "umount -l /mnt/disk%d_%d",disk,partition);
            bcmSystem(cmd);
            sprintf(cmd, "rm -r /mnt/disk%d_%d&", disk, partition);
            bcmSystem(cmd);
        }


#ifdef DMP_STORAGESERVICE_1
        if(retv == 0)
        {
            HOTPLUG_DEBUG("calling ssk for add disk=%d partition=%d\n", disk, partition);
            notify_smd(disk,partition,CMS_MSG_STORAGE_ADD_LOGICAL_VOLUME);
        }
#endif
    }
    else if (!strcmp(action,"remove"))
    {

        sleep(1);

        if(!(devpath = getenv("DEVPATH")))
            return EINVAL;

        devname = strrchr(devpath,'/');
        devname++;

        HOTPLUG_DEBUG(":unmount devname =%s, devpath=%s  \n",devname, devpath);

        /* just need 'a' '1', 'a' '2', 'b' '1' , 'b' '2' ...from sda1, sda2, sdb1,sdb2......*/
        tmp[0] = devname[2];
        tmp[1] = devname[3];

        if(!isalpha(tmp[0])) {
            HOTPLUG_ERROR("remove:- illegal devname=%s\n",devname);
            return 0;
        }				

        disk = tmp[0] - 'a' + 1; //disk1, disk2 .......

        if(tmp[1] == '\0')
        {
            /*device for entire disk ex: sda,sdb etc..*/
#ifdef DMP_STORAGESERVICE_1
            HOTPLUG_DEBUG("notifying ssk for removing physical disk=%d\n", disk);
            notify_smd(disk,0,CMS_MSG_STORAGE_REMOVE_PHYSICAL_MEDIUM);
#endif
            return 0;
        }
        partition = tmp[1] - '0'; //disk1_1, disk1_2....

        /*TODO why is this needed */
        bcmSystem("cd /var");

        sprintf(cmd, "umount -l /mnt/disk%d_%d",disk,partition);
        bcmSystem(cmd);
        sprintf(cmd, "rm -r /mnt/disk%d_%d", disk, partition);
        bcmSystem(cmd);				

#ifdef DMP_STORAGESERVICE_1
        HOTPLUG_DEBUG("calling ssk for remove disk=%d partition=%d\n", disk, partition);
        notify_smd(disk,partition,CMS_MSG_STORAGE_REMOVE_LOGICAL_VOLUME);
#endif

    }    

    return 0;
}
