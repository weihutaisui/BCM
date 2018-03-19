/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
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

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include "cms_msg.h"
#include "cms_eid.h"
#include "cms_util.h"
#include "bcmtypes.h"
#include "bmu_api.h"

#define MAX_CMD_SIZE 512

/***************************************************************************
 * Function Name: main
 * Description  : This utility invokes the command line interface
 *                that is embedded within the Battery Controller code
 *                available in the Broadband Fondation Class (Bfc)
 ***************************************************************************/
int main(int argc, char **argv)
{
	uint8_t buf[sizeof(CmsMsgHeader)+MAX_CMD_SIZE];
	CmsMsgHeader *pMsg = (CmsMsgHeader *)&buf[0];
	CmsRet ret = CMSRET_SUCCESS;
    void *msgHandle = NULL;
    char *pcmd;
    int i;

    if ((ret = cmsMsg_init(EID_BMUCTL, &msgHandle)) != CMSRET_SUCCESS) {
        printf("cmsMsg_init failed, ret=%d", ret);
        goto done;
    }

    /* Create message to send to bmud */
	memset(&buf[0], 0x0, sizeof(buf));
	pMsg->type		  = CMS_MSG_BMU_CLI;
	pMsg->src		  = MAKE_SPECIFIC_EID(getpid(), EID_BMUCTL);
	pMsg->dst		  = EID_BMUD;
	pMsg->flags_event = 1;
	pcmd = (char*)(pMsg + 1);

    /* Copy the command line arguments over */
    if (argc > 1) {
        /* redirect bmud output to bmuctl user's terminal
           note: can't redirect to files, only ttys */
        char *tty = basename(ttyname(fileno(stdin)));
        if (strcmp(tty, "tty") != 0 && strcmp(tty, "console") != 0)
            pcmd += sprintf(pcmd, ">%s\n", tty);

        if(strlen(argv[1]) < MAX_CMD_SIZE) {
            sprintf(pcmd, "%s", argv[1]);

            i = 2;
            while (i < argc) {
                if (strlen(pcmd) + strlen(argv[i]) + 2 < MAX_CMD_SIZE) {
                    sprintf(&pcmd[strlen(pcmd)], " %s", argv[i]);
                    i++;
                }
                else {
                    ret = CMSRET_INVALID_ARGUMENTS;
                    goto done;
                }
            }
        }
        else {
            ret = CMSRET_INVALID_ARGUMENTS;
            goto done;
        }
    }
    pMsg->dataLength = strlen((char *)(pMsg + 1));

    /* And send it */
    ret = cmsMsg_send(msgHandle, pMsg);
    if ( CMSRET_SUCCESS != ret )
    {
        printf("cmsMsg_send failed, ret=%d", ret);
    }

done:
    cmsMsg_cleanup(&msgHandle);

    if (CMSRET_INVALID_ARGUMENTS == ret) {
        printf("Invalid arguments\n");
    }

    exit( ret );
}
