
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

#ifdef DMP_STORAGESERVICE_1

#include<stdio.h>
#include<sys/vfs.h>
#include<dirent.h>
#include<linux/unistd.h>
#include <errno.h>

#include "cms_util.h"
#include "cms_core.h"
#include "ssk.h"


#define STORAGE_MOUNT_POINT "/mnt/"

#define FS_TYPE_VFAT 0x4D44
#define FS_TYPE_NTFS 0x65735546 
#define FS_TYPE_EXT3 0xEF53
#define FS_TYPE_EXT2 0xEF51


#define TR140_AUTH_NONE 0x00

#ifdef SUPPORT_SAMBA
static UBOOL8 isFolderPresent(char *dirpath)
{
   DIR *dfd;
   UBOOL8 ret;

   dfd = opendir(dirpath);

   if((dfd ==NULL) && (errno == ENOENT))
   {
      ret = FALSE;
   }
   else
   {
      ret = TRUE;
   }

   if(dfd != NULL)
	closedir(dfd);

   return ret;
}
/*caller should free the memory allocated for folder list */
static CmsRet getToplevelFolders(char *volumeName, char ***FolderList, int *nFolders)
{
   struct dirent **dlist;
   int n = 0,count =0;
   char **tmpFolderList=NULL;

   if(!volumeName)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   n = scandir(volumeName, &dlist, 0, 0);

   if (n < 0)
   {
      cmsLog_error("Logical volume scan failed");
      return CMSRET_INTERNAL_ERROR;
   }
   else if (n > 0)
   {
      tmpFolderList = cmsMem_alloc( n * sizeof(char *), 0);
      while (n--) 
      {
         if((dlist[n]->d_type == DT_DIR) && (dlist[n]->d_name[0] != '.'))
         {
            tmpFolderList[count++] = cmsMem_strdup(dlist[n]->d_name);
         }
         free(dlist[n]);
      }
      free(dlist);
   }
   *FolderList = tmpFolderList;
   *nFolders = count;
   return CMSRET_SUCCESS;
}

#ifdef not_used_for_now
static CmsRet getMountedPartitions(char *baseMntPoint, char ***partitionList, int *nPartitions)
{

   struct dirent **namelist;
   int n = 0,count =0;
   char **tmpPartitionList;

   if(!baseMntPoint)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   n = scandir(baseMntPoint, &namelist, 0, 0);
   if (n < 0)
   {
      return CMSRET_INTERNAL_ERROR;
   }
   else if (n > 0)
   {
      tmpPartitionList = cmsMem_alloc( n * sizeof(char *), 0);
      while (n--) 
      {
         if(namelist[n]->d_name[0] != '.')
         {
         tmpPartitionList[count++] = cmsMem_strdup(namelist[n]->d_name);
         }
         free(namelist[n]);
      }
      free(namelist);
   }
   *partitionList = tmpPartitionList;
   *nPartitions = count;
   return CMSRET_SUCCESS;
}
#endif /* not_used_for_now */

#endif  /* SUPPORT_SAMBA */

static CmsRet setLogicalVolumeInfo(char *path, LogicalVolumeObject *logicalVolumeObj)
{
   struct statfs fsStats;

   if(!path || !logicalVolumeObj)
   {
      return CMSRET_INVALID_ARGUMENTS;
   }

   if(statfs(path,&fsStats) == 0)
   {
      logicalVolumeObj->capacity  = cmsFil_scaleSizetoKB(fsStats.f_blocks, fsStats.f_bsize)/KILOBYTE ;/*Mega Bytes*/
      logicalVolumeObj->usedSpace = logicalVolumeObj->capacity - (cmsFil_scaleSizetoKB(fsStats.f_bfree, fsStats.f_bsize)/KILOBYTE);/*Mega Bytes*/

      if(fsStats.f_type == FS_TYPE_VFAT)
      {
         CMSMEM_REPLACE_STRING(logicalVolumeObj->fileSystem,"fat");
      }
      else if(fsStats.f_type == FS_TYPE_NTFS)
      {
         CMSMEM_REPLACE_STRING(logicalVolumeObj->fileSystem,"ntfs");
      }
      else if(fsStats.f_type == FS_TYPE_EXT2)
      {
         CMSMEM_REPLACE_STRING(logicalVolumeObj->fileSystem,"ext2");
      }
      else if(fsStats.f_type ==FS_TYPE_EXT3)
      {
         CMSMEM_REPLACE_STRING(logicalVolumeObj->fileSystem,"ext3");
      }
      else 
      {
         CMSMEM_REPLACE_STRING(logicalVolumeObj->fileSystem,"unknown");
      }


      return CMSRET_SUCCESS;
   }
   else
   {
      return CMSRET_INTERNAL_ERROR;
   }

}


static CmsRet addPhysicalMedium(char *name, char *connectionType,InstanceIdStack iidStack)
{
   PhysicalMediumObject *physicalMediumObj =NULL;
   CmsRet ret;

   if((ret=cmsObj_addInstance(MDMOID_PHYSICAL_MEDIUM, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not add PhysicalMedium object, ret=%d", ret);
      return CMSRET_INTERNAL_ERROR;
   }

   if(( ret =cmsObj_get(MDMOID_PHYSICAL_MEDIUM, &iidStack, OGF_NO_VALUE_UPDATE,(void **) &physicalMediumObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get PhysicalMedium object, ret=%d", ret);
      return CMSRET_INTERNAL_ERROR;
   } 

   CMSMEM_REPLACE_STRING(physicalMediumObj->name,name);
   CMSMEM_REPLACE_STRING(physicalMediumObj->connectionType,connectionType);
   physicalMediumObj->removable = TRUE;
   physicalMediumObj->capacity = 0;/*TODO update with proper value*/
   //CMSMEM_REPLACE_STRING(physicalmedium->status,"online");


   if(( ret =cmsObj_set(physicalMediumObj,&iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set PhysicalMedium object, ret=%d", ret);
      cmsObj_free((void **) &physicalMediumObj);
      return CMSRET_INTERNAL_ERROR;
   } 

   cmsObj_free((void **) &physicalMediumObj);

   return CMSRET_SUCCESS;

}

static void removePhysicalMedium(char *phyMediumName, char *phyReference)
{
   StorageServiceObject *storageServiceObj=NULL;
   PhysicalMediumObject *physicalMediumObj =NULL;
   LogicalVolumeObject *logicalVolumeObj = NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack tmpIidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack childIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   int tobeDeleted=0;

   if(( ret =cmsObj_getNext(MDMOID_STORAGE_SERVICE, &iidStack, (void **) &storageServiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get StorageService object, ret=%d", ret);
      return ;
   } 


   while(( ret = cmsObj_getNextInSubTree(MDMOID_PHYSICAL_MEDIUM, &iidStack, &tmpIidStack, (void **) &physicalMediumObj)) == CMSRET_SUCCESS)
   {
      if(cmsUtl_strcmp(physicalMediumObj->name,phyMediumName) == 0)
      {

         if((ret = cmsObj_deleteInstance(MDMOID_PHYSICAL_MEDIUM, &tmpIidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not delete PhysicalMedium entry, ret=%d", ret);
         }
         else
         {
            storageServiceObj->physicalMediumNumberOfEntries--;
         } 
         cmsObj_free((void **) &physicalMediumObj);
         break;
      }
      cmsObj_free((void **) &physicalMediumObj);

   }


   /*remove any logical volumes(if not removed explicitly by hotplug already) on this physical medium*/
   INIT_INSTANCE_ID_STACK(&tmpIidStack);
   INIT_INSTANCE_ID_STACK(&childIidStack);

   tobeDeleted = 0;

   while(( ret =cmsObj_getNextInSubTreeFlags(MDMOID_LOGICAL_VOLUME, &iidStack, &childIidStack, OGF_NO_VALUE_UPDATE,(void **) &logicalVolumeObj)) == CMSRET_SUCCESS)
   {

      if(tobeDeleted)
      {
         if((ret = cmsObj_deleteInstance(MDMOID_LOGICAL_VOLUME, &tmpIidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not delete Logical Volume entry, ret=%d", ret);
         }
         else
         {
            storageServiceObj->logicalVolumeNumberOfEntries--;
         } 
         tobeDeleted = 0;
      }

      if(cmsUtl_strcmp(logicalVolumeObj->physicalReference,phyReference) == 0)
      {
         /* entry matched, mark for removal
          * we cant delete this entry immediately as we need to get
          * next entry based on childIidstack,so we just mark here and it will
          * be removed in next iteration
          */

         tmpIidStack = childIidStack;
         tobeDeleted = 1;
      }

      cmsObj_free((void **) &logicalVolumeObj);
   }

   if(tobeDeleted)
   {
      if((ret = cmsObj_deleteInstance(MDMOID_LOGICAL_VOLUME, &tmpIidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("could not delete Logical Volume entry, ret=%d", ret);
      }
      else
      {
         storageServiceObj->logicalVolumeNumberOfEntries--;
      } 
      tobeDeleted = 0;
   }

   if(( ret =cmsObj_set(storageServiceObj,&iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set StorageServic object, ret=%d", ret);
   } 

   cmsObj_free((void **) &storageServiceObj);
   return;

}

#ifdef SUPPORT_SAMBA
static CmsRet addFolderObj(char *name, UINT32 accessPerm, InstanceIdStack iidStack)
{
   FolderObject *folderObj;
   CmsRet ret;

   if((ret=cmsObj_addInstance(MDMOID_FOLDER, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not add Folder object, ret=%d", ret);
      return ret;
   }

   if((ret =cmsObj_get(MDMOID_FOLDER, &iidStack, OGF_NO_VALUE_UPDATE,(void **) &folderObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get Folder object, ret=%d", ret);
      return ret;
   } 

   CMSMEM_REPLACE_STRING(folderObj->name,name);
   folderObj->userAccountAccess = accessPerm;
   folderObj->enable = TRUE;
   
   if(( ret =cmsObj_set(folderObj,&iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set folderObj object, ret=%d", ret);
      cmsObj_free((void **) &folderObj);
      return ret;
   } 

   cmsObj_free((void **) &folderObj);
   return CMSRET_SUCCESS;
}

#ifdef not_used_for_now
static CmsRet deleteFolder(char *name, InstanceIdStack iidStack)
{
   return CMSRET_SUCCESS;
}
#endif

#endif  /* SUPPORT_SAMBA */

static CmsRet addLogicalVolume(char *volumeName, InstanceIdStack iidStack, char *physicalReference)
{
   LogicalVolumeObject *logicalVolumeObj = NULL;
   char tmpBuf[BUFLEN_128];
   CmsRet ret;

   if((ret=cmsObj_addInstance(MDMOID_LOGICAL_VOLUME, &iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not add LogicalVolume object, ret=%d", ret);
      return ret;
   }

   if((ret =cmsObj_get(MDMOID_LOGICAL_VOLUME, &iidStack, OGF_NO_VALUE_UPDATE,(void **) &logicalVolumeObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get LogicalVolume object, ret=%d", ret);
      return ret;
   } 

   CMSMEM_REPLACE_STRING(logicalVolumeObj->name,volumeName);
   CMSMEM_REPLACE_STRING(logicalVolumeObj->physicalReference, physicalReference);

   sprintf(tmpBuf,"%s%s",STORAGE_MOUNT_POINT,logicalVolumeObj->name);
   setLogicalVolumeInfo(tmpBuf, logicalVolumeObj); 

#ifdef SUPPORT_SAMBA
   {
      char **FolderList=NULL;
      int nFolders=0;
      int i;

      /*get the top level folders information */
      getToplevelFolders(tmpBuf,&FolderList, &nFolders);
      for(i=0; i<nFolders; i++)
      {
         if((ret=addFolderObj(FolderList[i],TR140_AUTH_NONE,iidStack)) == CMSRET_SUCCESS)
         {
            logicalVolumeObj->folderNumberOfEntries++;

         }
         else
         {
            cmsLog_error(" addFolderobj Failed ret=%d", ret);
         }

         cmsMem_free(FolderList[i]);
      }
      cmsMem_free(FolderList);
   }
#endif

   CMSMEM_REPLACE_STRING(logicalVolumeObj->status,"online");
   logicalVolumeObj->enable = TRUE;

   if(( ret =cmsObj_set(logicalVolumeObj,&iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set LogicalVolume object, ret=%d", ret);
      cmsObj_free((void **) &logicalVolumeObj);
      return ret;
   } 

   cmsObj_free((void **) &logicalVolumeObj);
   return CMSRET_SUCCESS;

}

static CmsRet removeLogicalVolume(char *volumeName, InstanceIdStack iidStack,
                                  char *physicalReference __attribute__((unused)))
{
   LogicalVolumeObject *logicalVolumeObj = NULL;
   InstanceIdStack childIidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;

   while(( ret =cmsObj_getNextInSubTreeFlags(MDMOID_LOGICAL_VOLUME, &iidStack, &childIidStack, OGF_NO_VALUE_UPDATE,(void **) &logicalVolumeObj)) == CMSRET_SUCCESS)
   {
      if(cmsUtl_strcmp(logicalVolumeObj->name,volumeName) == 0)
      {
         if((ret = cmsObj_deleteInstance(MDMOID_LOGICAL_VOLUME, &childIidStack)) != CMSRET_SUCCESS)
         {
            cmsLog_error("could not delete Logical Volume entry, ret=%d", ret);
         } 
         cmsObj_free((void **) &logicalVolumeObj);
         break;
      }
      cmsObj_free((void **) &logicalVolumeObj);
   } 
   return ret ;
}

void processAddPhysicalMediumMsg(CmsMsgHeader *msg)
{
   char phyMediumName[BUFLEN_64];
   StorageServiceObject *storageServiceObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   int disk=0;
   char *data;

   data = (char*) (msg +1);
   if(!data)
   {
      cmsLog_error("Invalid Msg data");
      return;
   }
   disk = *((int *)data);
   sprintf(phyMediumName,"disk%d",disk);

   if( (ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   if(( ret =cmsObj_getNext(MDMOID_STORAGE_SERVICE, &iidStack, (void **) &storageServiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get StorageService object, ret=%d", ret);
      cmsLck_releaseLock();
      return ;
   } 

   if(addPhysicalMedium(phyMediumName, "disk", iidStack) != CMSRET_SUCCESS)
   {
      cmsLog_error("Failed to add physicalMedium %s",phyMediumName);
      cmsLck_releaseLock();
      return;
   }
   else
   {
      storageServiceObj->physicalMediumNumberOfEntries++;
   }


   if(( ret =cmsObj_set(storageServiceObj,&iidStack)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not set StorageServic object, ret=%d", ret);
   } 

   cmsObj_free((void **) &storageServiceObj);
   cmsLck_releaseLock();
   return;
}

void processRemovePhysicalMediumMsg(CmsMsgHeader *msg)
{
   char phyMediumName[BUFLEN_64];
   char phyReference[BUFLEN_64];
   int disk;
   char *data;
   CmsRet ret;

   data = (char*) (msg +1);
   if(!data)
   {
      cmsLog_error("Invalid Msg data");
      return;
   }
   
   disk = *((int *)data);
   sprintf(phyMediumName,"disk%d",disk);
   sprintf(phyReference,"PhysicalMedium.%d",disk);

   if( (ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   removePhysicalMedium(phyMediumName, phyReference);

   cmsLck_releaseLock();
}

void processAddLogicalVolumeMsg(CmsMsgHeader *msg)
{
   StorageServiceObject *storageServiceObj=NULL;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   CmsRet ret;
   char volumeName[BUFLEN_64];
   char phyReference[BUFLEN_64];
   int disk, partition;
   char *data;

   data = (char*) (msg +1);
   if(!data)
   {
      cmsLog_error("Invalid Msg data");
      return;
   }
   
   //sscanf(data,"%d,%d",&disk,&partition);
   disk = *((int *)data);
   partition = *((int *)data+1);
   sprintf(volumeName,"disk%d_%d",disk,partition);
   sprintf(phyReference,"PhysicalMedium.%d",disk);

   
   if( (ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
   {
      cmsLog_error("could not get lock, ret=%d", ret);
      return;
   }

   if(( ret =cmsObj_getNext(MDMOID_STORAGE_SERVICE, &iidStack, (void **) &storageServiceObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get StorageService object, ret=%d", ret);
      cmsLck_releaseLock();
      return ;
   } 

   if(addLogicalVolume(volumeName, iidStack, phyReference) == CMSRET_SUCCESS)
   {
      storageServiceObj->logicalVolumeNumberOfEntries++;

      if((ret =cmsObj_set(storageServiceObj,&iidStack)) != CMSRET_SUCCESS)
      {
         cmsLog_error("Could not set StorageServic object, ret=%d", ret);
      } 
      
#ifdef SUPPORT_SAMBA
      {
         char cmd[BUFLEN_256];
         char folderName[BUFLEN_128];

         /*check if shared folder is present on logical volume if not create one */
         sprintf(folderName,"/mnt/%s/share",volumeName);

         if(isFolderPresent(folderName) == FALSE)
         {
            sprintf(cmd,"mkdir %s ",folderName);
            if (system(cmd) == -1)
            {
               cmsLog_error("mkdir command failed");
            }
         }
      }
#endif
   }

   cmsObj_free((void **) &storageServiceObj);
   cmsLck_releaseLock();
   return;
}

void processRemoveLogicalVolumeMsg(CmsMsgHeader *msg)
{
    StorageServiceObject *storageServiceObj=NULL;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    CmsRet ret;
    char volumeName[BUFLEN_64];
    char phyReference[BUFLEN_64];
    int disk, partition;
    char *data;

    data = (char*) (msg +1);
    if(!data)
    {
        cmsLog_error("Invalid Msg data");
        return;
    }
    
    //sscanf(data,"%d,%d",&disk,&partition);
    disk = *((int *)data);
    partition = *((int *)data+1);
    sprintf(volumeName,"disk%d_%d",disk,partition);
    sprintf(phyReference,"PhysicalMedium.%d",disk);


    if( (ret = cmsLck_acquireLockWithTimeout(SSK_LOCK_TIMEOUT)) != CMSRET_SUCCESS )
    {
        cmsLog_error("could not get lock, ret=%d", ret);
        return;
    }

    if(( ret =cmsObj_getNext(MDMOID_STORAGE_SERVICE, &iidStack, (void **) &storageServiceObj)) != CMSRET_SUCCESS)
    {
        cmsLog_error("Could not get StorageService object, ret=%d", ret);
        cmsLck_releaseLock();
        return ;
    } 

    if(removeLogicalVolume(volumeName, iidStack, phyReference) == CMSRET_SUCCESS)
    {
        storageServiceObj->logicalVolumeNumberOfEntries--;

        if((ret =cmsObj_set(storageServiceObj,&iidStack)) != CMSRET_SUCCESS)
        {
            cmsLog_error("Could not set StorageServic object, ret=%d", ret);
        } 

    }

    cmsObj_free((void **) &storageServiceObj);
    cmsLck_releaseLock();
    return;
}

void initStorageService(void)
{

#ifdef SUPPORT_SAMBA

   /*TODO get admin password from login cfg object*/
   /* add admin to smbpasswd */
   
#endif
   return;
}


#endif /* DMP_STORAGESERVICE_1 */
