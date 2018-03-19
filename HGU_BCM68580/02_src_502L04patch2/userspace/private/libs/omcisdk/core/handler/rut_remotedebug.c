/***********************************************************************
 *
 *  Copyright (c) 2016 Broadcom
 *  All Rights Reserved
 *
<:label-BRCM:2016:proprietary:omcid

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
*      ONU Remote Debug ME utility functions.
*
*****************************************************************************/

#ifdef DMP_X_ITU_ORG_GPON_1


/* ---- Include Files ----------------------------------------------------- */

#include "owsvc_api.h"
#include "ctlutils_api.h"
#include "me_handlers.h"
#include "omci_api.h"


/* ---- Private Constants and Types --------------------------------------- */

#define CMD_LEN_MAX OMCI_ENTRY_SIZE_25
#define RSP_LEN_MAX OMCI_RMTDBG_RSP_LEN_MAX

typedef struct
{
    pthread_mutex_t cmdReqRspMutex;
    pthread_cond_t cmdReqCond;
    pthread_t threadId;
    UBOOL8 initDoneB;
    UINT32 cmdLen;
    UINT32 rspLen;
    char cmd[CMD_LEN_MAX + 1];
    char rsp[RSP_LEN_MAX + 1];
} debugCB_t;


/* ---- Private Function Prototypes --------------------------------------- */

static int debugCmdExec(char *cmdP);
static int createPipe(void);
static void flushPipe(void);
static void readResult(void);
static int debugCmdReqWait(char *cmdP);
static void remotedebugThread(void *data);


/* ---- Public Variables -------------------------------------------------- */


/* ---- Private Variables ------------------------------------------------- */

static debugCB_t debugCB;

/* fd0 is for reading, fd1 is for writing. */
static int pipeFds[2];
static int oldStdout = 0;


/* ---- Functions --------------------------------------------------------- */


/*****************************************************************************
*  FUNCTION:  debugCbInit
*  PURPOSE:   Initialize Remote Debug control block and create the thread.
*  PARAMETERS:
*      None.
*  RETURNS:
*      int.
*  NOTES:
*      None.
*****************************************************************************/
int debugCbInit(void)
{
    struct sched_param debugThreadSched;
    pthread_attr_t debugThreadAttr;
    int rc = 0;

    memset(&debugCB, 0x0, sizeof(debugCB));
    pthread_mutex_init(&debugCB.cmdReqRspMutex, NULL);
    pthread_cond_init(&debugCB.cmdReqCond, NULL);

    pthread_attr_init(&debugThreadAttr);
    debugThreadSched.sched_priority = 1;  /* Low priority. */
    pthread_attr_setschedparam(&debugThreadAttr, &debugThreadSched);
    pthread_attr_setdetachstate(&debugThreadAttr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&debugCB.threadId, &debugThreadAttr,
      (void*)&remotedebugThread, (void*)NULL) != 0)
    {
        cmsLog_error("Failed to create remote debug thread");
        rc = -1;
    }
    else
    {
        debugCB.initDoneB = TRUE;
    }

    pthread_attr_destroy(&debugThreadAttr);
    return rc;
}

/*****************************************************************************
*  FUNCTION:  debugCmdReqNotify
*  PURPOSE:   Notify the Remote Debug thread a new command is available.
*  PARAMETERS:
*      cmdP - pointer to the command.
*  RETURNS:
*      None.
*  NOTES:
*      None.
*****************************************************************************/
void debugCmdReqNotify(char *cmdP)
{
    int len;

    if (debugCB.initDoneB != TRUE)
    {
        cmsLog_error("Invalid debugCB state");
        return;
    }

    len = strlen(cmdP);
    if (len == 0)
    {
        cmsLog_error("Invalid command len %d", len);
        return;
    }

    pthread_mutex_lock(&debugCB.cmdReqRspMutex);
    strcpy(debugCB.cmd, cmdP);
    debugCB.rspLen = 0;
    debugCB.cmdLen = len;
    pthread_cond_signal(&debugCB.cmdReqCond);
    pthread_mutex_unlock(&debugCB.cmdReqRspMutex);
}

/*****************************************************************************
*  FUNCTION:  debugRspGet
*  PURPOSE:   Get response from the debug control block.
*  PARAMETERS:
*      bufP - pointer to the response buffer (provided by user).
*  RETURNS:
*      Response buffer size with valid information.
*  NOTES:
*      None.
*****************************************************************************/
UINT32 debugRspGet(char *bufP)
{
    int len = 0;

    if (bufP != NULL)
    {
        pthread_mutex_lock(&debugCB.cmdReqRspMutex);
        if (debugCB.rspLen != 0)
        {
            memcpy(bufP, &debugCB.rsp, debugCB.rspLen);
            len = debugCB.rspLen;
        }
        pthread_mutex_unlock(&debugCB.cmdReqRspMutex);
    }

    return len;
}

/* Create pipe, use the pipe for command output. */
static int createPipe(void)
{
    int fd;

    if (pipe(pipeFds) == -1)
    {
        cmsLog_error("pipe() failed\n");
        return -1;
    }

    /* Set to non-blocking mode. */
    fcntl(pipeFds[0], F_SETFL, fcntl(pipeFds[0], F_GETFL) | O_NONBLOCK);
    fcntl(pipeFds[1], F_SETFL, fcntl(pipeFds[1], F_GETFL) | O_NONBLOCK);

    fd = dup2(STDOUT_FILENO, oldStdout);
    fd = (fd == -1) ? fd : dup2(pipeFds[1], STDOUT_FILENO);
    if (fd == -1)
    {
        cmsLog_error("dup2() failed\n");
        close(pipeFds[0]);
        close(pipeFds[1]);
        return -1;
    }

    return 0;
}

/* Execute the command. */
static int debugCmdExec(char *cmdP)
{
    int rc = -1;

    if (cmdP == NULL)
    {
        cmsLog_error("Invalid command");
        return -1;
    }

    rc = createPipe();
    if (rc == 0)
    {
        flushPipe();
        if (system(cmdP) == -1)
        {
            cmsLog_error("system(%s) failed", cmdP);
            rc = -1;
        }
    }

    dup2(oldStdout, STDOUT_FILENO);
    oldStdout = 0;

    close(pipeFds[1]);
    readResult();
    close(pipeFds[0]);
    return rc;
}

/* Flush the pipe. */
static void flushPipe(void)
{
    int r = 0;

    while (1)
    {
        r = read(pipeFds[0], &debugCB.rsp, RSP_LEN_MAX);
        if (r <= 0)
        {
            break;
        }
    }
}

/* Copy the response to debugCB. */
static void readResult(void)
{
    int r = 0;
    int total = 0;

    pthread_mutex_lock(&debugCB.cmdReqRspMutex);
    while (1)
    {
        r = read(pipeFds[0], &debugCB.rsp[total], (RSP_LEN_MAX - total));
        if (r > 0)
        {
            total += r;
            if (total > RSP_LEN_MAX)
            {
                total = RSP_LEN_MAX;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if ((total == 0) || (r < 0))
    {
        cmsLog_error("read() failed");
        total = 0;
    }

    debugCB.rsp[total] = '\0';
    debugCB.rspLen = total;
    pthread_mutex_unlock(&debugCB.cmdReqRspMutex);
    cmsLog_debug("===================\n");
    cmsLog_debug("\n%s\n", debugCB.rsp);
    cmsLog_debug("===================\n");
}

/* Wait for the command. */
static int debugCmdReqWait(char *cmdP)
{
    int len;

    pthread_mutex_lock(&debugCB.cmdReqRspMutex);
    while (debugCB.cmdLen == 0)
    {
        pthread_cond_wait(&debugCB.cmdReqCond, &debugCB.cmdReqRspMutex);
    }
    strncpy(cmdP, debugCB.cmd, CMD_LEN_MAX);
    len = debugCB.cmdLen;
    debugCB.cmdLen = 0;
    pthread_mutex_unlock(&debugCB.cmdReqRspMutex);
    return len;
}

/* The thread that executes remote debug commands. */
static void remotedebugThread(void *data __attribute__((unused)))
{
    char cmd[CMD_LEN_MAX + 1];
    char *cmdP = &cmd[0];
    int len;

    while (1)
    {
        len = debugCmdReqWait(cmdP);
        if (len != 0)
        {
            debugCmdExec(cmdP);
        }
    }
}

#endif /* DMP_X_ITU_ORG_GPON_1 */
