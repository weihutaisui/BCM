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

/********************************************************************************
* TkOsAlSem.c
*
* DESCRIPTION:
*       Semaphore related routines
*
* DEPENDENCIES:
*       OS Dependent.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "Teknovus.h"
#include "TkOsAlSem.h"

/*******************************************************************************
* TkOsSemCreate
*
* DESCRIPTION:
*       Create semaphore.
*
* INPUTS:
*       name   - semaphore Name
*       init   - init value of semaphore counter
*       count  - max counter (must be >= 1)
*
* OUTPUTS:
*       smid - semaphore Id
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
STATUS TkOsSemCreate(TkOsSem *pTkSem,TkSemBeginState state)
	{
	if(0 != sem_init(pTkSem, state, 1))
		return RcFail;
	else
		return RcOk;
	}

/*******************************************************************************
* TkOsSemDelete
*
* DESCRIPTION:
*       Delete semaphore.
*
* INPUTS:
*       smid - semaphore Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
STATUS TkOsSemDelete(TkOsSem *pTkSem)
	{
	sem_destroy(pTkSem);
	return RcOk;
	}
/*******************************************************************************
* TkOsSemWait
*
* DESCRIPTION:
*       Wait on semaphore.
*
* INPUTS:
*       smid    - semaphore Id
*       timeOut - time out in miliseconds or 0 to wait forever
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       OS_TIMEOUT - on time out
*
* COMMENTS:
*       None
*
*******************************************************************************/
STATUS TkOsSemWait(TkOsSem *pTkSem, U32 timeOut)
	{
	sem_wait(pTkSem) ; 
	return RcOk;
	}

/*******************************************************************************
* TkOsSemPut
*
* DESCRIPTION:
*       Signal a semaphore.
*
* INPUTS:
*       smid    - semaphore Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
STATUS TkOsSemPut(TkOsSem *pTkSem)
	{
	sem_post(pTkSem) ; 
	return RcOk;
	}

////////////////////////////////////////////////////////////////////////////////
/// TkOsSemGet: Get a counting semaphore
///
/// \parameter sem 		The semaphore to get
/// \parameter block 	Blocking call. FALSE = Return immediately
///                                    TRUE = Wait forever
///
/// \return		FALSE if block == FALSE and the semaphore was already locked,
///					TRUE otherwise
////////////////////////////////////////////////////////////////////////////////
BOOL TkOsSemGet(TkOsSem *pTkSem, BOOL block)
	{
	int i;

	if (block == TRUE)
		{
		if (sem_wait(pTkSem) != 0)
			{
			printf("Semaphore blocking get failed");
			}
		return TRUE;
		}
	else
		{
		i = sem_trywait(pTkSem);
		if (i == EAGAIN)
			{
			return FALSE;
			}
		if (i != 0)
			{
			printf("Semaphore non-blocking get failed");
			}
		return TRUE;
		}
	}

