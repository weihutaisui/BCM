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


/*
* \file TkOamMem.h
* \brief the OAM memory management
*
*/

#ifndef TKOAMMEM_H
#define TKOAMMEM_H

#include "TkPlatform.h"
#include "TkSdkInitApi.h"

#define TKOAM_MEM_DEBUG

#ifndef ROUND_UP
#define ROUND_UP(x, align)	    (((int) (x) + (align - 1)) & ~(align - 1))
#endif /* ROUND_UP */

#ifndef ROUND_DOWN
#define ROUND_DOWN(x, align)	((int)(x) & ~(align - 1))
#endif /* ROUND_DOWN */

typedef struct TkOamMem_s       /* memory control block */
{
    void *BlkMemAddr;           /* Pointer to beginning of memory partition */
    void *BlkMemFreeList;       /* Pointer to list of free memory blocks */
    U32 BlkMemSize;           /* Size (in bytes) of each block of memory */
    U32 MemNBlks;             /* Total number of blocks in this partition */
    U32 MemNFree;             /* Number of memory blocks remaining in this partition */
    TkOsSem BlkMemSem;           /* semphore for block memory safe operation. */
#ifdef TKOAM_MEM_DEBUG
    U32 MemGetFailed;         /* Get failed */
#endif /* TKOAM_MEM_DEBUG */
} TkOamMem;


#ifdef __cplusplus
extern "C" {
#endif

/*
 * to be used by oam parsing state machine for OAM source buffer address and
 * OAM reply buffer address. These pointers must be initialised by the actual 
 * memory address before any actual parsing starts.
 */    

extern EthernetFrame *TxFrame;
extern EthernetFrame *RxFrame;
extern U16 rxFrameLength;

/* TkOamMemInit - init the OAM memory management */
extern int TkOamMemInit (U32 Blocks, U32 Size);

/* TkOamMemGet - Get a memory block from a partition */
extern void *TkOamMemGet (void);

/* TkOamMemPut - Returns a memory block to a partition */
extern int TkOamMemPut (void *pblk);

/* TkOamMemStatus - Display the number of free memory blocks and the number of
                    used memory blocks from a memory partition. */
extern int TkOamMemStatus (void);

/* TkOamMemsFree - free the memory of one memory partition */
extern void TkOamMemsFree (void);


#ifdef __cplusplus
}
#endif

#endif /* TKOAMMEM_H */
