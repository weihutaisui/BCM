/***********************************************************************
 *
 *  Copyright (c) 2017  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2017:proprietary:standard

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

#ifdef DMP_X_BROADCOM_COM_CONTAINER_1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>

#include "cms_util.h"
#include "cms_dal.h"
#include "cms_qdm.h"


extern WEB_NTWK_VAR glbWebVar; // this global var is defined in cms_dal.h


void cgiGetContainerTable(int argc __attribute__((unused)),
                          char **argv __attribute__((unused)),
                          char *varValue)
{
   CmsRet ret = CMSRET_SUCCESS;
   char eeName[BUFLEN_64];
   UBOOL8 found = FALSE;
   UINT32 rootId = 0, containerId = 0;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   InstanceIdStack iidStackEE = EMPTY_INSTANCE_ID_STACK;
   ExecEnvObject   *eeObj   = NULL;
   ContainerObject *containerObj = NULL;
   ContainerInfoObject *containerInfoObj = NULL;
   ContainerStateObject *containerStateObj = NULL;
   char *p = varValue;


   /* Get container object to call stl_containerObject() implicitly
      for retrieving current container information  */
   ret = cmsObj_get(MDMOID_CONTAINER,
                    &iidStack,
                    0,
                    (void **) &containerObj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get container object, returned %d", ret);
      return;
   }

   cmsObj_free((void **) &containerObj);

   INIT_INSTANCE_ID_STACK(&iidStackEE);

   /* Loop through EE objects to add EE containers */
   while (cmsObj_getNext(MDMOID_EXEC_ENV, &iidStackEE, (void **)&eeObj) == CMSRET_SUCCESS)
   {
      INIT_INSTANCE_ID_STACK(&iidStack);
      found = FALSE;
      rootId = containerId;

      /* Add EE Container */
      /* Look for containerStateObj->name that matches with eeObj->X_BROADCOM_COM_ContainerName */
      while (found == FALSE &&
             cmsObj_getNextFlags(MDMOID_CONTAINER_INFO,
                                 &iidStack,
                                 OGF_NO_VALUE_UPDATE,
                                 (void **) &containerInfoObj) == CMSRET_SUCCESS)
      {
         if (cmsObj_get(MDMOID_CONTAINER_STATE,
                        &iidStack,
                        0,
                        (void **) &containerStateObj) == CMSRET_SUCCESS)
         {
            /* Look for only EE container */
            if (cmsUtl_strcmp(containerStateObj->name, eeObj->X_BROADCOM_COM_ContainerName) == 0)
            {
               rootId++;
               p += sprintf(p, "        <tr data-tt-id='%u' data-tt-parent-id='0'>\n", rootId);
               p += sprintf(p, "          <td>%s</td>\n", (containerStateObj->name? containerStateObj->name:""));
               p += sprintf(p, "          <td>%s</td>\n", (containerStateObj->ID? containerStateObj->ID:""));
               p += sprintf(p, "          <td>%s</td>\n", (containerStateObj->status? containerStateObj->status:""));
               p += sprintf(p, "          <td><input type='button' onClick='detailClick(\"%s\")' value='Show'></td>\n",
                            (containerStateObj->name? containerStateObj->name:""));
               p += sprintf(p, "          <td>%s</td>\n", (containerInfoObj->standard? containerInfoObj->standard:""));
               p += sprintf(p, "          <td></td>\n");
               p += sprintf(p, "        </tr>\n");

               found = TRUE;
            }
         
            cmsObj_free((void **) &containerStateObj);
         }

         cmsObj_free((void **) &containerInfoObj);
      }

      if (found == TRUE)
      {
         INIT_INSTANCE_ID_STACK(&iidStack);
         containerId = rootId;
   
         /* Add EU Containers */
         /* Look for each container info object that has its eeName match with eeObj->name */
         while (cmsObj_getNextFlags(MDMOID_CONTAINER_INFO,
                                    &iidStack,
                                    OGF_NO_VALUE_UPDATE,
                                    (void **) &containerInfoObj) == CMSRET_SUCCESS)
         {
            memset(eeName, 0, sizeof(eeName));

            if (cmsObj_get(MDMOID_CONTAINER_STATE,
                           &iidStack,
                           0,
                           (void **) &containerStateObj) == CMSRET_SUCCESS)
            {
               /* Look for only non EE container */
               if (cmsUtl_strcmp(containerStateObj->name, eeObj->X_BROADCOM_COM_ContainerName) != 0 &&
                   containerInfoObj->execEnvRef != NULL)
               {
                  /* Get execution environment name from its full path */
                  qdmModsw_getExecEnvNameByFullPathLocked(containerInfoObj->execEnvRef,
                                                          eeName, sizeof(eeName));
                  /* Only add EUs containers that has its eeName match with eeObj->name */
                  if (cmsUtl_strcmp(eeObj->name, eeName) == 0)
                  {
                     containerId++;
                     p += sprintf(p, "        <tr data-tt-id='%u' data-tt-parent-id='%u'>\n", containerId, rootId);
                     p += sprintf(p, "          <td>%s</td>\n", (containerStateObj->name? containerStateObj->name:""));
                     p += sprintf(p, "          <td>%s</td>\n", (containerStateObj->ID? containerStateObj->ID:""));
                     p += sprintf(p, "          <td>%s</td>\n", (containerStateObj->status? containerStateObj->status:""));
                     p += sprintf(p, "          <td><input type='button' onClick='detailClick(\"%s\")' value='Show'></td>\n",
                                  (containerStateObj->name? containerStateObj->name:""));
                     p += sprintf(p, "          <td>%s</td>\n", (containerInfoObj->standard? containerInfoObj->standard:""));
                     p += sprintf(p, "          <td>%s</td>\n", eeName);
                     p += sprintf(p, "        </tr>\n");
                  }
               }

               cmsObj_free((void **) &containerStateObj);
            }

            cmsObj_free((void **) &containerInfoObj);
         }
      }

      cmsObj_free((void **)&eeObj);
   }
   
   *p = 0;

   /* Buffer is WEB_BUF_SIZE_MAX bytes */
   cmsLog_debug("total bytes written=%d", p-varValue+1);
}


void cgiGetContainerDetail(int argc __attribute__((unused)),
                           char **argv __attribute__((unused)),
                           char *varValue)
{
   UBOOL8 found = FALSE;
   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   ContainerInfoObject *containerInfoObj = NULL;
   ContainerStateObject *containerStateObj = NULL;
   ContainerNetworkObject *containerNetworkObj = NULL;
   ContainerStatsObject *containerStatsObj = NULL;
   char *p = varValue;

   /* Look for each container info object */
   while (found == FALSE &&
          cmsObj_getNextFlags(MDMOID_CONTAINER_INFO,
                              &iidStack,
                              OGF_NO_VALUE_UPDATE,
                              (void **) &containerInfoObj) == CMSRET_SUCCESS)
   {
      /* Get container state object */
      if (cmsObj_get(MDMOID_CONTAINER_STATE,
                     &iidStack,
                     0,
                     (void **) &containerStateObj) == CMSRET_SUCCESS)
      {
         /* Find container state object that has its name matched with container name */
         if (cmsUtl_strcmp(containerStateObj->name, glbWebVar.containerName) == 0)
         {
            found = TRUE;

            /* Get container network object */
            if (cmsObj_get(MDMOID_CONTAINER_NETWORK,
                           &iidStack,
                           0,
                           (void **) &containerNetworkObj) == CMSRET_SUCCESS)
            {
               /* Get container stats object */
               if (cmsObj_get(MDMOID_CONTAINER_STATS,
                              &iidStack,
                              0,
                              (void **) &containerStatsObj) == CMSRET_SUCCESS)
               {
                  p += sprintf(p, "      <div id='state' class='tabcontent'><center>\n");
                  p += sprintf(p, "        <table border='1' cellpadding='4' cellspacing='0'>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center' width='150'>Name</td>\n");
                  p += sprintf(p, "            <td align='center' width='200'>%s</td>\n", (containerStateObj->name? containerStateObj->name:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center'>ID</td>\n");
                  p += sprintf(p, "            <td align='center'>%s</td>\n", (containerStateObj->ID? containerStateObj->ID:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center'>Status</td>\n");
                  p += sprintf(p, "            <td align='center'>%s</td>\n", (containerStateObj->status? containerStateObj->status:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center'>PID</td>\n");
                  p += sprintf(p, "            <td align='center'>%s</td>\n", (containerStateObj->PID? containerStateObj->PID:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center'>CPU Use</td>\n");
                  p += sprintf(p, "            <td align='center'>%s</td>\n", (containerStateObj->cpuUse? containerStateObj->cpuUse:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center'>Memory Use</td>\n");
                  p += sprintf(p, "            <td align='center'>%s</td>\n", (containerStateObj->memoryUse? containerStateObj->memoryUse:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "        </table>\n");
                  p += sprintf(p, "      </div>\n");

                  p += sprintf(p, "      <div id='network' class='tabcontent'><center>\n");
                  p += sprintf(p, "        <table border='1' cellpadding='4' cellspacing='0'>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center' width='150'>Interface</td>\n");
                  p += sprintf(p, "            <td align='center' width='200'>%s</td>\n", (containerNetworkObj->interface? containerNetworkObj->interface:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center'>IPv4 Addresses</td>\n");
                  p += sprintf(p, "            <td align='center'>%s</td>\n", (containerNetworkObj->IPv4Addresses? containerNetworkObj->IPv4Addresses:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center'>IPv6 Addresses</td>\n");
                  p += sprintf(p, "            <td align='center'>%s</td>\n", (containerNetworkObj->IPv6Addresses? containerNetworkObj->IPv6Addresses:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center'>Ports</td>\n");
                  p += sprintf(p, "            <td align='center'>%s</td>\n", (containerNetworkObj->ports? containerNetworkObj->ports:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "        </table>\n");
                  p += sprintf(p, "      </div>\n");

                  p += sprintf(p, "      <div id='stats' class='tabcontent'><center>\n");
                  p += sprintf(p, "        <table border='1' cellpadding='4' cellspacing='0'>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center' width='150'>Transmitted Bytes</td>\n");
                  p += sprintf(p, "            <td align='center' width='200'>%s</td>\n", (containerStatsObj->bytesSent? containerStatsObj->bytesSent:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "          <tr align='center'>\n");
                  p += sprintf(p, "            <td class='hd' align='center'>Received Bytes</td>\n");
                  p += sprintf(p, "            <td align='center'>%s</td>\n", (containerStatsObj->bytesReceived? containerStatsObj->bytesReceived:""));
                  p += sprintf(p, "          </tr>\n");
                  p += sprintf(p, "        </table>\n");
                  p += sprintf(p, "      </div>\n");

                  /* Get container stats object */
                  cmsObj_free((void **) &containerStatsObj);
               }

               /* Free container network object */
               cmsObj_free((void **) &containerNetworkObj);
            }
         }

         /* Free container state object */
         cmsObj_free((void **) &containerStateObj);
      }

      /* Free container info object */
      cmsObj_free((void **) &containerInfoObj);
   }
   
   *p = 0;

   /* Buffer is WEB_BUF_SIZE_MAX bytes */
   cmsLog_debug("total bytes written=%d", p-varValue+1);
}


#endif /* DMP_X_BROADCOM_COM_CONTAINER_1 */
