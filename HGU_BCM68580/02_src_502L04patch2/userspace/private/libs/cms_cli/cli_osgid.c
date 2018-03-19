/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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

/** command driven CLI code goes into this file */

#ifdef SUPPORT_CLI_CMD

#ifdef SUPPORT_MODSW_CLI

#ifdef SUPPORT_OSGI_FELIX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/stat.h>

#include "cms.h"
#include "cms_log.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "cms_msg_modsw.h"

#include "cli.h"

static const char osgidUsage[] = "\nUsage:\n";

static const char osgidInstall[] =
    "        osgid install url <http://> uuid <1234> username <test>  password <test>\n";
static const char osgidUninstall[] =
    "        osgid uninstall uuid <1234> version <1>\n";
static const char osgidUpdate[] =
    "        osgid update url <ftp://> uuid <1234> username <test> password <test> version <1>\n";
static const char osgidLB[] =
    "        osgid lb\n";
static const char osgidPrint[] =
    "        osgid print\n";

#define OSGID_MAX_OPTS 11

/***************************************************************************
 * Function Name: cmdOsgidHelp
 * Description  : Prints help information about basic osgid commands
 ***************************************************************************/
static void cmdOsgidHelp(void)
{
   printf("%s%s%s%s%s%s", osgidUsage, osgidInstall, osgidUninstall, osgidUpdate, osgidLB, osgidPrint);
}

/***************************************************************************
 * Function Name: cmdOsgidInstall
 * Description  : Sends message to osgid to do install operation
 ***************************************************************************/
static CmsRet cmdOsgidInstall(char *url, char *uuid, char *username, char *password)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader *reqMsg;
    unsigned char *body;
    DUrequestStateChangedMsgBody *msgPayload;

    reqMsg = cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(DUrequestStateChangedMsgBody),
                          ALLOC_ZEROIZE);
    if(reqMsg == NULL)
    {
        cmsLog_error("Could not allocate memory");
        ret = CMSRET_RESOURCE_EXCEEDED;
    }

    /* initialize header fields */
    reqMsg->type = CMS_MSG_REQUEST_DU_STATE_CHANGE;
    reqMsg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
    reqMsg->dst = EID_OSGID;
    reqMsg->flags_request = 1;
    reqMsg->dataLength = sizeof(DUrequestStateChangedMsgBody);

    /* copy file into the payload and send message */
    body = (unsigned char *)(reqMsg + 1);
    msgPayload = (DUrequestStateChangedMsgBody*)body;

    strncpy(msgPayload->operation,SW_MODULES_OPERATION_INSTALL,strlen(SW_MODULES_OPERATION_INSTALL));
    strncpy(msgPayload->URL,url,strlen(url));
    strncpy(msgPayload->UUID,uuid,strlen(uuid));
    strncpy(msgPayload->username,username,strlen(username));
    strncpy(msgPayload->password,password,strlen(password));

    if((ret = cmsMsg_send(cliPrvtMsgHandle, reqMsg)) != CMSRET_SUCCESS)
    {
       cmsLog_error("Failed to send message (ret=%d)", ret);
    }
    else
    {
       printf("%s: Request message (0x%08X), size %d\n", __FUNCTION__, (unsigned int)(reqMsg), reqMsg->dataLength);
    }
    CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);
    return ret;
}

/***************************************************************************
 * Function Name: cmdOsgidUninstall
 * Description  : Sends message to osgid to do uninstall operation
 ***************************************************************************/
static CmsRet cmdOsgidUninstall(char *version, char *uuid)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader *reqMsg;
    unsigned char *body;
    DUrequestStateChangedMsgBody *msgPayload;

    reqMsg = cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(DUrequestStateChangedMsgBody),
                          ALLOC_ZEROIZE);
    if(reqMsg == NULL)
    {
        cmsLog_error("Could not allocate memory");
        ret = CMSRET_RESOURCE_EXCEEDED;
    }

    /* initialize header fields */
    reqMsg->type = CMS_MSG_REQUEST_DU_STATE_CHANGE;
    reqMsg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
    reqMsg->dst = EID_OSGID;
    reqMsg->flags_request = 1;
    reqMsg->dataLength = sizeof(DUrequestStateChangedMsgBody);

    /* copy file into the payload and send message */
    body = (unsigned char *)(reqMsg + 1);
    msgPayload = (DUrequestStateChangedMsgBody*)body;

    strncpy(msgPayload->operation,SW_MODULES_OPERATION_UNINSTALL,strlen(SW_MODULES_OPERATION_UNINSTALL));
    strncpy(msgPayload->version,version,strlen(version));
    strncpy(msgPayload->UUID,uuid,strlen(uuid));
    if((ret = cmsMsg_send(cliPrvtMsgHandle, reqMsg)) != CMSRET_SUCCESS)
    {
       cmsLog_error("Failed to send message (ret=%d)", ret);
    }
    else
    {
       printf("%s: Request message (0x%08X), size %d\n", __FUNCTION__, (unsigned int)(reqMsg), reqMsg->dataLength);
    }
    CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);
    return ret;
}

/***************************************************************************
 * Function Name: cmdOsgidUninstall
 * Description  : Sends message to osgid to do uninstall operation
 ***************************************************************************/
static CmsRet cmdOsgidUpdate(char *url, char *uuid, char *username, char *password, char *version)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader *reqMsg;
    unsigned char *body;
    DUrequestStateChangedMsgBody *msgPayload;

    reqMsg = cmsMem_alloc(sizeof(CmsMsgHeader) + sizeof(DUrequestStateChangedMsgBody),
                          ALLOC_ZEROIZE);
    if(reqMsg == NULL)
    {
        cmsLog_error("Could not allocate memory");
        ret = CMSRET_RESOURCE_EXCEEDED;
    }

    /* initialize header fields */
    reqMsg->type = CMS_MSG_REQUEST_DU_STATE_CHANGE;
    reqMsg->src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
    reqMsg->dst = EID_OSGID;
    reqMsg->flags_request = 1;
    reqMsg->dataLength = sizeof(DUrequestStateChangedMsgBody);

    /* copy file into the payload and send message */
    body = (unsigned char *)(reqMsg + 1);
    msgPayload = (DUrequestStateChangedMsgBody*)body;

    strncpy(msgPayload->operation,SW_MODULES_OPERATION_UPDATE,strlen(SW_MODULES_OPERATION_UPDATE));
    strncpy(msgPayload->URL,url,strlen(url));
    strncpy(msgPayload->UUID,uuid,strlen(uuid));
    strncpy(msgPayload->username,username,strlen(username));
    strncpy(msgPayload->password,password,strlen(password));
    strncpy(msgPayload->version,version,strlen(version));
    if((ret = cmsMsg_send(cliPrvtMsgHandle, reqMsg)) != CMSRET_SUCCESS)
    {
       cmsLog_error("Failed to send message (ret=%d)", ret);
    }
    else
    {
       printf("%s: Request message (0x%08X), size %d\n", __FUNCTION__, (unsigned int)(reqMsg), reqMsg->dataLength);
    }
    CMSMEM_FREE_BUF_AND_NULL_PTR(reqMsg);
    return ret;
}

static CmsRet cmdOsgidLB(void)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader msg = EMPTY_MSG_HEADER;

    /* initialize header fields */
    msg.type = CMS_MSG_REQUEST_BUNDLE_LIST;
    msg.src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
    msg.dst = EID_OSGID;
    msg.flags_request = 1;

    if((ret = cmsMsg_send(cliPrvtMsgHandle, &msg)) != CMSRET_SUCCESS)
    {
       cmsLog_error("Failed to send message (ret=%d)", ret);
    }
    return ret;
}

static CmsRet cmdOsgidPrint(void)
{
    CmsRet ret = CMSRET_SUCCESS;
    CmsMsgHeader msg = EMPTY_MSG_HEADER;

    /* initialize header fields */
    msg.type = CMS_MSG_OSGID_PRINT;
    msg.src = cmsMsg_getHandleEid(cliPrvtMsgHandle);
    msg.dst = EID_OSGID;
    msg.flags_request = 1;

    if((ret = cmsMsg_send(cliPrvtMsgHandle, &msg)) != CMSRET_SUCCESS)
    {
       cmsLog_error("Failed to send message (ret=%d)", ret);
    }
    return ret;
}

/***************************************************************************
 * Function Name: processOsgidCmd
 * Description  : Parses Osgid commands
 ***************************************************************************/
void processOsgidCmd(char *cmdLine)
{
   SINT32 argc = 0;
   char *argv[OSGID_MAX_OPTS]={NULL};
   char *last = NULL;
   CmsRet ret = CMSRET_SUCCESS;

   /* parse the command line and build the argument vector */
   argv[0] = strtok_r(cmdLine, " ", &last);

   if(argv[0] != NULL)
   {
       for(argc=1; argc<OSGID_MAX_OPTS; ++argc)
       {
           argv[argc] = strtok_r(NULL, " ", &last);

           if (argv[argc] == NULL)
           {
               break;
           }

           cmsLog_debug("arg[%d]=%s", argc, argv[argc]);
       }
   }

   if (argv[0] == NULL)
   {
       cmdOsgidHelp();
   }
   else if(strcasecmp(argv[0], "install") == 0)
   {
       char *url = NULL;
       char *uuid = NULL;
       char *username = NULL;
       char *password = NULL;
       int argIndex = 1;

       while(1)
       {
           if(argv[argIndex] == NULL)
           {
               printf("install: Missing arguments\n");
               cmdOsgidHelp();
               break;
           }

           if(strcasecmp(argv[argIndex], "url") == 0)
           {
               if(argv[++argIndex] != NULL)
               {
                   url = argv[argIndex++];
               }
               else
               {
                   printf("install: Missing 'url' argument\n");
                   cmdOsgidHelp();
                   break;
               }
           }
           else if(strcasecmp(argv[argIndex], "username") == 0)
           {
               if(argv[++argIndex] != NULL)
               {
                   username = argv[argIndex++];
               }
               else
               {
                   printf("install: Missing 'username' argument\n");
                   cmdOsgidHelp();
                   break;
               }
           }
           else if(strcasecmp(argv[argIndex], "password") == 0)
           {
               if(argv[++argIndex] != NULL)
               {
                   password = argv[argIndex++];
               }
               else
               {
                   printf("install: Missing 'password' argument\n");
                   cmdOsgidHelp();
                   break;
               }
           }
           else if(strcasecmp(argv[argIndex], "uuid") == 0)
           {
               if(argv[++argIndex] != NULL)
               {
                   uuid = argv[argIndex++];
               }
               else
               {
                   printf("install: Missing 'uuid' argument\n");
                   cmdOsgidHelp();
                   break;
               }
           }
           else
           {
               printf("Install: Invalid argument '%s'\n", argv[argIndex]);
               cmdOsgidHelp();
               break;
           }

           if(url && uuid && username && password)
           {
              ret = cmdOsgidInstall(url,uuid,username,password);
              break;
           }
       } /* while 1 */
   } /* install */
   else if(strcasecmp(argv[0], "update") == 0)
   {
       char *url = NULL;
       char *uuid = NULL;
       char *username = NULL;
       char *password = NULL;
       char *version = NULL;
       int argIndex = 1;

       while(1)
       {
           if(argv[argIndex] == NULL)
           {
               printf("update: Missing arguments\n");
               cmdOsgidHelp();
               break;
           }

           if(strcasecmp(argv[argIndex], "url") == 0)
           {
               if(argv[++argIndex] != NULL)
               {
                   url = argv[argIndex++];
               }
               else
               {
                   printf("update: Missing 'url' argument\n");
                   cmdOsgidHelp();
                   break;
               }
           }
           else if(strcasecmp(argv[argIndex], "username") == 0)
           {
               if(argv[++argIndex] != NULL)
               {
                   username = argv[argIndex++];
               }
               else
               {
                   printf("update: Missing 'username' argument\n");
                   cmdOsgidHelp();
                   break;
               }
           }
           else if(strcasecmp(argv[argIndex], "password") == 0)
           {
               if(argv[++argIndex] != NULL)
               {
                   password = argv[argIndex++];
               }
               else
               {
                   printf("update: Missing 'password' argument\n");
                   cmdOsgidHelp();
                   break;
               }
           }
           else if(strcasecmp(argv[argIndex], "uuid") == 0)
           {
               if(argv[++argIndex] != NULL)
               {
                   uuid = argv[argIndex++];
               }
               else
               {
                   printf("update: Missing 'uuid' argument\n");
                   cmdOsgidHelp();
                   break;
               }
           }
           else if(strcasecmp(argv[argIndex], "version") == 0)
           {
               if(argv[++argIndex] != NULL)
               {
                   version = argv[argIndex++];
               }
               else
               {
                   printf("update: Missing 'version' argument\n");
                   cmdOsgidHelp();
                   break;
               }
           }
           else
           {
               printf("Update: Invalid argument '%s'\n", argv[argIndex]);
               cmdOsgidHelp();
               break;
           }

           if(url && uuid && username && password && version)
           {
              ret = cmdOsgidUpdate(url,uuid,username,password,version);
              break;
           }
       } /* while 1 */
   } /* update */
   else if(strcasecmp(argv[0], "uninstall") == 0)
   {
       char *version = NULL;
       char *uuid = NULL;
       int argIndex = 1;

       while(1)
       {
           if(argv[argIndex] == NULL)
           {
               printf("uninstall: Missing arguments\n");
               cmdOsgidHelp();
               break;
           }
           if(strcasecmp(argv[argIndex], "version") == 0)
           {
               if(argv[++argIndex] != NULL)
               {
                   version = argv[argIndex++];
               }
               else
               {
                   printf("update: Missing 'version' argument\n");
                   cmdOsgidHelp();
                   break;
               }
           }
           else if(strcasecmp(argv[argIndex], "uuid") == 0)
           {
               if(argv[++argIndex] != NULL)
               {
                   uuid = argv[argIndex++];
               }
               else
               {
                   printf("uninstall: Missing 'uuid' argument\n");
                   cmdOsgidHelp();
                   break;
               }
           }
           else
           {
               printf("Uninstall: Invalid argument '%s'\n", argv[argIndex]);
               cmdOsgidHelp();
               break;
           }

           if(version && uuid)
           {
              ret = cmdOsgidUninstall(version,uuid);
              break;
           }
       } /* while 1 */
   } /* uninstall */
   else if(strcasecmp(argv[0], "LB") == 0)
   {
      ret = cmdOsgidLB();
   } /* LB */
   else if(strcasecmp(argv[0], "print") == 0)
   {
      ret = cmdOsgidPrint();
   } /* LB */
   else
   {
       printf("Invalid osgid Command\n");
       cmdOsgidHelp();
   }
}

#endif /* SUPPORT_OSGI_FELIX */

#endif /* SUPPORT_MODSW_CLI */

#endif /* SUPPORT_CLI_CMD */
