/***********************************************************************
 *
 *  Copyright (c) 2016  Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2016:proprietary:standard

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

#ifndef CONTAINER_H
#define CONTAINER_H

#include "beep_common.h"

#define CONT_BLKIO_MAX_ENTRIES  16
#define CONT_DEV_ACCESS_MAX_ENTRIES  16
#define CONT_DEVICES_MAX_ENTRIES  16
#define CONT_HOOK_POINT_MAX_ENTRIES  16
#define CONT_HOOK_POINT_ARGS_MAX_ENTRIES  16

typedef enum 
{
   CONT_TOOL_LXC = 0,
   CONT_TOOL_DOCKER,
   CONT_TOOL_MAX,
} ContContainerToolType_t;

typedef enum 
{
   CONT_BLKIO_READ_BPS_DEV = 0,
   CONT_BLKIO_WRITE_BPS_DEV,
   CONT_BLKIO_READ_IOPS_DEV,
   CONT_BLKIO_WRITE_IOPS_DEV
} ContBlockIoType_t;

typedef struct
{
   ContBlockIoType_t type;
   long long major;
   long long minor;
   unsigned long long rate;
} ContBlockIo_t;

typedef struct
{
   int allow;
   long long major;
   long long minor;
   char devType[8];
   char accessType[8];
} ContDevAccessList_t;

typedef struct
{
   char type[8];
   char path[32];
   long long major;
   long long minor;
} ContDevicesList_t;

typedef struct
{
   unsigned long long cpu;
   unsigned long long memory;
   unsigned long long flash;
   ContBlockIo_t blkio[CONT_BLKIO_MAX_ENTRIES];
   ContDevAccessList_t devAccess[CONT_DEV_ACCESS_MAX_ENTRIES];
} ContResource_t;

typedef struct
{
   char path[1024];
   char args[CONT_HOOK_POINT_ARGS_MAX_ENTRIES][1024];
   unsigned long long timeout;
} ContHookPoint_t;

typedef struct
{
   ContHookPoint_t presetup[CONT_HOOK_POINT_MAX_ENTRIES];
   ContHookPoint_t prestart[CONT_HOOK_POINT_MAX_ENTRIES];
   ContHookPoint_t poststart[CONT_HOOK_POINT_MAX_ENTRIES];
   ContHookPoint_t poststop[CONT_HOOK_POINT_MAX_ENTRIES];
   int presetupHookEntries;
   int prestartHookEntries;
   int poststartHookEntries;
   int poststopHookEntries;
} ContHooks_t;

typedef struct
{
   ContContainerToolType_t toolType;
   char containerName[64];
   char path[1024];
   char appName[64];
   char library[1024];
   char username[BEEP_USERNAME_LEN_MAX+1];
   int uid;
   int isPrivileged;
   char ntwkBridge[32];
   char ntwkDns[64];
   char ntwkMac[32];
   ContResource_t resource;
   ContDevicesList_t devices[CONT_DEVICES_MAX_ENTRIES];
   ContHooks_t hooks;
} ContainerSetup_t;

typedef enum
{
   CONT_MEDIA_TYPE_EXECUTABLE  = 0,   
   CONT_MEDIA_TYPE_TARBALL,
   CONT_MEDIA_TYPE_LAST,
} contMediaType;

typedef enum
{
   CONTRET_SUCCESS = 0,      /**<Success. */
   CONTRET_INTERNAL_ERROR ,  /**< Internal error. */
   CONTRET_SYSTEM_RESOURCE_EXCEEDED ,/**< System resources exceeded */
} ContRet;

int contCreateContainerDir(const char *path, ContContainerToolType_t tool,
                           contMediaType mediaType, char *unpackPath,
                           int unpackPathLen, int limitFlash);
ContRet contSetupContainer(const ContainerSetup_t *conf);
int contRestoreContainer(const char *container_name, const char *path,
                        int isPrivileged, const char *username, int limitFlash);
int contStopContainer(const char *container_name);
int contStartContainer(const char *path, const char *container_name,
                       int needCMS, int isRoot, int needNetwork);
int contDestroyContainer(const char *container_name, const char *path,
                         int limitFlash);
int contNetworkAddress(const char *container_name, char *addr, int addrLen);
int contGetContainerDataDir(const char *path, int limitFlash, int isEE,
                            char *dataDir, int dataDirLen);
int contGetEeContainerDuDir(const char *path, int limitFlash, int isEE,
                            char *duDir, int duDirLen);
int contCleanupContainerMountPoint(const char *container_name, const char *path,
                         int limitFlash);
#endif /* #ifndef CONTAINER_H */
