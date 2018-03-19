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


#include <unistd.h>
#include "TkOsAlMsg.h"
#include "TkOsAlThread.h"

int TkOsMsgQCreate(
					TkOsMsgQId *pMsgQid,
					unsigned int key, 
					unsigned char *name,
					unsigned int flag,
					unsigned int maxMsgs,
					unsigned int maxLengthOfMsg,
					unsigned int mode)
	{
	TkOsMsgQId msgQid;

	(void)name;

	if(NULL == pMsgQid)
		{
		return RcFail;
		}
	msgQid = msgget(key, IPC_CREAT | 0666);
	
	msgctl(msgQid,IPC_RMID,NULL);

	msgQid = msgget(key, IPC_CREAT | 0666);

	*pMsgQid = msgQid;
	
	return RcOk;
	}


void TkOsMsgQDestroy(TkOsMsgQId id)
	{
	msgctl(id,IPC_RMID,NULL);
	}


unsigned char TkOsMsgQSnd(TkOsMsgQId id, unsigned char *buf, unsigned int len, int ms)
	{
	/*unsigned char msg[1600] = {0x0};*/

	if ( msgsnd( id, buf, len, IPC_NOWAIT ) < 0 )
		{
		return RcFail;
		}

	return RcOk;
	}

int TkOsMsgQRcv(TkOsMsgQId id, unsigned char *buf, unsigned int len, int ms)
	{
	int i, rv;

	if ( ms == WAIT_FOREVER )
		{
		for ( i=0; i<WAIT_MAX_CYCLES; ++i )
			{
			if ( (rv = msgrcv( id, buf, len, 0, MSG_NOERROR | IPC_NOWAIT )) >= 0 )
				{
				return rv;
				}

			usleep( 50 );
			}
			return -1;
		}
	else if ( ms > 0 )
		{
		unsigned long long expir = TkOsGetCurrentTimeMs() + 60*ms, curr;


		for ( i=0; i<WAIT_MAX_CYCLES; ++i )
			{
			if ( (rv = msgrcv( id, buf, len, 0, MSG_NOERROR | IPC_NOWAIT )) >= 0 )
				{
				return rv;
				}

			curr = TkOsGetCurrentTimeMs();

			if ( curr > expir )
				{
				return -1;
				}

			usleep( 50*1000 );
			}
		}
	else
		{
		if ( (rv = msgrcv( id, buf, len, 0, MSG_NOERROR | IPC_NOWAIT )) >= 0 )
			{
			return rv;
			}else
			return -1;
		}
	return -1;
	}

