/***********************************************************************
 *
 *  Copyright (c) 2016  Broadcom Corporation
 *  All Rights Reserved
 *
 *  <:label-BRCM:2016:proprietary:standard
 *
 *   This program is the proprietary software of Broadcom and/or its
 *   licensors, and may only be used, duplicated, modified or distributed pursuant
 *   to the terms and conditions of a separate, written license agreement executed
 *   between you and Broadcom (an "Authorized License").  Except as set forth in
 *   an Authorized License, Broadcom grants no license (express or implied), right
 *   to use, or waiver of any kind with respect to the Software, and Broadcom
 *   expressly reserves all rights in and to the Software and all intellectual
 *   property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *   NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *   BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *   Except as expressly set forth in the Authorized License,
 *
 *   1. This program, including its structure, sequence and organization,
 *      constitutes the valuable trade secrets of Broadcom, and you shall use
 *      all reasonable efforts to protect the confidentiality thereof, and to
 *      use this information only in connection with your use of Broadcom
 *      integrated circuit products.
 *
 *   2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *      AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *      WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *      RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *      ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *      FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *      COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *      TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *      PERFORMANCE OF THE SOFTWARE.
 *
 *   3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *      ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *      INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *      WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *      IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *      OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *      SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *      SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *      LIMITED REMEDY.
 *  :>
 *
 ************************************************************************/

/*****************************************************************************
*    Description:
*
*      CLI commands for QoS.
*
*****************************************************************************/

#ifdef SUPPORT_CLI_CMD
#ifdef DMP_X_BROADCOM_COM_TM_1

/* ---- Include Files ----------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "cms.h"
#include "cms_log.h"
#include "cms_util.h"
#include "cms_core.h"
#include "cms_msg.h"
#include "../cms_core/linux/rut_tmctl_wrap.h"

#include "cli.h"


/* ---- Private Constants and Types --------------------------------------- */

#define MAX_OPTS 8


/* ---- Private Function Prototypes --------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */

static const char qosUsage[] = "\nUsage:\n";

#if (defined(GPON_HGU) || defined(EPON_HGU))
static const char qosScheme[] =
"        qos scheme --help\n\
        qos scheme --status\n\
        qos scheme --owner <(1)frontend)|2(backhaul) "
        "--qidpriomap <(1)Q7P7)|2(Q0P7)>\n";
static char *qosOwnerStr[] = \
  {"(0)default", "(1)frontend", "(2)backhaul", "invalid option"};
#else
static const char qosScheme[] =
"        qos scheme --help\n\
        qos scheme --status\n\
        qos scheme --qidpriomap <(1)Q7P7)|2(Q0P7)>\n";
#endif /* (defined(GPON_HGU) || defined(EPON_HGU)) */
static char *qidPrioMapStr[] = \
  {"(0)default", "(1)Q7P7", "(2)Q0P7", "invalid option"};


/* ---- Functions --------------------------------------------------------- */


/*****************************************************************************
*  FUNCTION:  cmdQoSHelp
*  PURPOSE:   Prints help information about the CLI commands.
*  PARAMETERS:
*      argc - number of command line arguments.
*      argv - array of command line argument strings.
*  RETURNS:
*      0 - success, others - error.
*  NOTES:
*      None.
*****************************************************************************/

static void cmdQoSHelp(char *help)
{
    if ((help == NULL) || (strcasecmp(help, "--help") == 0) ||
      (strcasecmp(help, "scheme") == 0))
    {
        printf("%s%s", qosUsage, qosScheme);
    }
}

/*****************************************************************************
*  FUNCTION:  cmdQoSSchemeStatus
*  PURPOSE:   show curent QoS scheme status.
*  PARAMETERS:
*      None.
*  RETURNS:
*      0 - success, others - error.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet cmdQoSSchemeStatus(void)
{
    CmsRet ret;
    UINT32 owner;
    UINT32 qidPrioMap;

    ret = rut_tmctl_getQueueScheme(&owner, &qidPrioMap);
    if (ret != CMSRET_SUCCESS)
    {
        cmsLog_error("rut_tmctl_getQueueScheme() failed, ret=%d", ret);
    }

#if (defined(GPON_HGU) || defined(EPON_HGU))
    printf("   Owner: ");
    if (owner >= TMCTL_OWNER_MAX)
    {
        printf("%s\n", qosOwnerStr[TMCTL_OWNER_MAX]);
    }
    else
    {
        printf("%s\n", qosOwnerStr[owner]);
    }
#endif /* (defined(GPON_HGU) || defined(EPON_HGU)) */
    printf("   QID to priority mapping: ");
    if (qidPrioMap >= QID_PRIO_MAP_MAX)
    {
        printf("%s\n\n", qidPrioMapStr[QID_PRIO_MAP_MAX]);
    }
    else
    {
        printf("%s\n\n", qidPrioMapStr[qidPrioMap]);
    }

    return ret;
}

/*****************************************************************************
*  FUNCTION:  cmdQoSSchemeConfig
*  PURPOSE:   Configure curent QoS scheme status.
*  PARAMETERS:
*      None.
*  RETURNS:
*      0 - success, others - error.
*  NOTES:
*      None.
*****************************************************************************/
static CmsRet cmdQoSSchemeConfig(UINT32 owner, UINT32 qidPrioMap)
{
    CmsRet ret = CMSRET_INVALID_PARAM_VALUE;
    InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
    BCMTrafficManagementObject *tmObj = NULL;

    if ((owner >= TMCTL_OWNER_MAX) || (qidPrioMap>= QID_PRIO_MAP_MAX))
    {
        cmsLog_error("Invalid parameters, owner %d, qidPrioMap %d\n",
          owner, qidPrioMap);
        return ret;
    }

    if ((ret = cmsLck_acquireLockWithTimeout(6 * MSECS_IN_SEC))
      == CMSRET_SUCCESS)
    {
        INIT_INSTANCE_ID_STACK(&iidStack);
        if ((ret = cmsObj_get(MDMOID_BCM_TRAFFIC_MANAGEMENT, &iidStack, 0,
          (void*)&tmObj)) == CMSRET_SUCCESS)
        {
            tmObj->owner = owner;
            tmObj->qidPrioMap = qidPrioMap;
            cmsObj_set(tmObj, &iidStack);
            cmsObj_free((void**)&tmObj);
            cmsMgm_saveConfigToFlash();
        }

        cmsLck_releaseLock();
    }
    else
    {
        cmsLog_notice("cmsLck_acquireLockWithTimeout failed ret: %d", ret);
    }

    cmdQoSSchemeStatus();

    return ret;
}

/*****************************************************************************
*  FUNCTION:  processQoSCmd
*  PURPOSE:   Processes QoS CLI commands.
*  PARAMETERS:
*      cmdLine - command string.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void processQoSCmd(char *cmdLine)
{
    SINT32 argc = 0;
    char *argv[MAX_OPTS] = {NULL};
    char *last = NULL;
    UINT32 val0;
#if (defined(GPON_HGU) || defined(EPON_HGU))
    UINT32 val1;
#endif

    argv[0] = strtok_r(cmdLine, " ", &last);

    if (argv[0] != NULL)
    {
        for (argc = 1; argc < MAX_OPTS; ++argc)
        {
            argv[argc] = strtok_r(NULL, " ", &last);

            if (argv[argc] == NULL)
            {
                break;
            }
        }
    }

    if (argv[0] == NULL)
    {
        cmdQoSHelp(NULL);
    }
    else if (strcasecmp(argv[0], "--help") == 0)
    {
        cmdQoSHelp(argv[0]);
    }
    else if (strcasecmp(argv[0], "scheme") == 0)
    {
        if ((argv[1] == NULL) || (strcasecmp(argv[1], "--help") == 0))
        {
            cmdQoSHelp("scheme");
        }
        else if (strcasecmp(argv[1], "--status") == 0)
        {
            cmdQoSSchemeStatus();
        }
#if (defined(GPON_HGU) || defined(EPON_HGU))
        else if ((argc == 5) && 
          (strcasecmp(argv[1], "--owner") == 0) &&
          (strcasecmp(argv[3], "--qidpriomap") == 0))
        {
            val0 = strtoul(argv[2], (char**)NULL, 0);
            val1 = strtoul(argv[4], (char**)NULL, 0);
            cmdQoSSchemeConfig(val0, val1);
        }
#else
        else if ((argc == 3) && 
          (strcasecmp(argv[1], "--qidpriomap") == 0))
        {
            val0 = strtoul(argv[2], (char**)NULL, 0);
            cmdQoSSchemeConfig(0, val0);
        }
#endif /* (defined(GPON_HGU) || defined(EPON_HGU)) */
        else
        {
            cmdQoSHelp("scheme");
        }
    }
}
#endif /* DMP_X_BROADCOM_COM_TM_1 */
#endif /* SUPPORT_CLI_CMD */
