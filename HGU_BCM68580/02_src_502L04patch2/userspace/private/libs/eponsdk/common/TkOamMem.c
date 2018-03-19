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
* \file TkOamMem.c
* \brief the OAM memory management
*
*/
#include <stdlib.h>
#ifdef LINUX
#include <malloc.h>
#endif
#include "TkSdkInitApi.h"
#include "TkOamMem.h"

EthernetFrame *TxFrame;
EthernetFrame *RxFrame;
U16 rxFrameLength;

/* block memory controler */
static TkOamMem TkOamBlkMem;
static TkOamMem *pTkOamBlkMem = &TkOamBlkMem;

static int TkOamMemCreate (void *addr, U32 nblks, U32 MemSize);

/* TkOamMemInit - init the OAM memory management
*
* Parameters: 
*  \blocks - number of memory blocks.
*  \size   - the size of each block in the memory partition.
*
*  \return: 
*    OK    - if init successfully or 
*    ERROR - if init fail.
*/
int TkOamMemInit (U32 Blocks, U32 Size)
{
    void *buf = NULL;

    buf = (void *) memalign (4, Size * Blocks);
    if (NULL == buf)
        return (ERROR);
    
    if (OK != TkOamMemCreate (buf, Blocks, Size))
   	{
        if (buf)
            free (buf);
        
        return (ERROR);
   	}
    else
        return (OK);
}


/* TkOamMemCreate - Create a fixed-sized memory partition.
*
* Parameters: 
*  \addr - the starting address of the memory partition
*  \nblks - number of memory blocks to create from the partition.
*  \MemSize - the size (in bytes and must be multipler 4 bytes) of each block 
*                        in the memory partition.
*
*  \return: 
*    OK    - if the partition was created or 
*    ERROR - if the partition was not created because of invalid arguments.
*/
static int TkOamMemCreate (void *addr, U32 nblks, U32 MemSize)
{
    U8 *pblk;
    void **plink;
    U32 i;

    if ((NULL == addr) ||   /* Must pass a valid address for the memory part. */
        (nblks < 2))        /* Must have at least 2 blocks per partition */
   	{
        return (ERROR);
   	}

    if ((MemSize < sizeof (void *)) ||  /* Must contain space for at least a pointer */
        (0 != MemSize % 4))             /* Must be multipler 4 bytes */
   	{
        return (ERROR);
   	}
    
    /*pTkOamBlkMem->BlkMemSem = TkOsSemCreate(1, "");*/

    if (TkOsSemCreate(&pTkOamBlkMem->BlkMemSem,TkSemFull) )
   	{
        printf ("Create the block memory semaphore FAIL!\n");
        return (ERROR);
   	}
    
    plink = (void **)addr;  /* Create linked list of free memory blocks */
    pblk  = (U8 *)((U32)addr + MemSize);
    for (i = 0; i < (nblks - 1); i++)
   	{
       *plink = (void *)pblk;                     /* Save pointer to NEXT block in CURRENT block */
        plink = (void **)pblk;                    /* Position to NEXT block */
        pblk  = (U8 *)((U32)pblk + MemSize);  /* Point to the FOLLOWING block */
   	}

    *plink               = (void *)NULL;          /* Last memory block points to NULL */
    pTkOamBlkMem->BlkMemAddr     = addr;          /* Store start address of memory partition */
    pTkOamBlkMem->BlkMemFreeList = addr;          /* Initialize pointer to pool of free blocks */
    pTkOamBlkMem->MemNFree       = nblks;         /* Store number of free blocks in MCB */
    pTkOamBlkMem->MemNBlks     = nblks;
    pTkOamBlkMem->BlkMemSize     = MemSize;       /* Store block size of each memory blocks */
#ifdef TKOAM_MEM_DEBUG
    pTkOamBlkMem->MemGetFailed   = 0;
#endif /* TKOAM_MEM_DEBUG */
    return (OK);
}


/* TkOamMemGet - Get a memory block from a partition
*
* Parameters: 
*    no
*
*  \return: 
*    A pointer to a memory block if no error is detected or
*    A pointer to NULL if an error is detected.
*/
void *TkOamMemGet (void)
{
    void *pblk;

    /* If the block memory can be operated? */
    if (!TkOsSemGet(&pTkOamBlkMem->BlkMemSem, TRUE))
   	{
        printf ("Get block memory semaphore FAIL!");
        return (NULL);
   	}
    
    if (pTkOamBlkMem->MemNFree > 0) /* See if there are any free memory blocks */
   	{
        pblk = pTkOamBlkMem->BlkMemFreeList;             /* Yes, point to next free memory block */
        pTkOamBlkMem->BlkMemFreeList = *(void **)pblk;   /* Adjust pointer to new free list */
        pTkOamBlkMem->MemNFree--;                        /* One less memory block in this partition */
        /* Give the block memory semaphore. */
        TkOsSemPut(&pTkOamBlkMem->BlkMemSem);
        return (pblk);      /* Return memory block to caller */
   	}
    else
   	{
        /* Give the block memory semaphore. */
        TkOsSemPut(&pTkOamBlkMem->BlkMemSem);
#ifdef BLOCK_MEM_DEBUG
        pTkOamBlkMem->MemGetFailed++;  
#endif /* BLOCK_MEM_DEBUG */
        return (NULL);
   	}
}


/* TkOamMemPut - Returns a memory block to a partition
*
* Parameters: 
*  \pblk - a pointer to the memory block being released.
*
*  \return: 
*    OK    - if the memory block was inserted into the partition
*    ERROR - returning a memory block to an already FULL memory partition or passed a NULL 
*            pointer for 'pblk'
*/
int TkOamMemPut (void *pblk)
{

    if (NULL == pblk)     /* Must release a valid block */
   	{
        return (ERROR);
   	}
    
    /* If the block memory can be operated? */
    if (!TkOsSemGet(&pTkOamBlkMem->BlkMemSem, WAIT_FOREVER))
   	{
        printf ("Get block memory semaphore FAIL!");
        return (ERROR);
   	}
    
    if (pTkOamBlkMem->MemNFree >= pTkOamBlkMem->MemNBlks) /* Make sure all blocks not already returned */
   	{
        /* Give the block memory semaphore. */
        TkOsSemPut(&pTkOamBlkMem->BlkMemSem);
        return (ERROR);
   	}
    
    *(void **)pblk = pTkOamBlkMem->BlkMemFreeList;   /* Insert released block into free block list */
    pTkOamBlkMem->BlkMemFreeList = pblk;
    pTkOamBlkMem->MemNFree++;                        /* One more memory block in this partition */
    /* Give the block memory semaphore. */
    TkOsSemPut(&pTkOamBlkMem->BlkMemSem);

    return (OK);
}


/* TkOamMemStatus - Display the number of free memory blocks and the number of
*                   used memory blocks from a memory partition.
*
* Parameters: 
*  no
*
*  \return: 
*    OK or
*    ERROR - if error
*/
int TkOamMemStatus (void)
{

    printf ("BlkMemSize = %u\n", pTkOamBlkMem->BlkMemSize);
    printf ("MemNBlks = %u\n", pTkOamBlkMem->MemNBlks);
    printf ("MemNFree = %u\n", pTkOamBlkMem->MemNFree);
#ifdef BLOCK_MEM_DEBUG
    printf ("MemGetFailed = 0x%08x\n", pTkOamBlkMem->MemGetFailed);  
#endif /* BLOCK_MEM_DEBUG */

    return (OK);
}


/* TkOamMemsFree - free the memory of one memory partition
*
* Parameters: 
*   no
*
*  \return: 
*   no
*/
void TkOamMemsFree (void)
{
    if (pTkOamBlkMem->BlkMemAddr)
   	{
        free (pTkOamBlkMem->BlkMemAddr);
   	}
}
