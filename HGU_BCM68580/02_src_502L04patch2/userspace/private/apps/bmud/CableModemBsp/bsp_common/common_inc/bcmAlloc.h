//****************************************************************************
//
//  Copyright (c) 2002-2004  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id$
//
//  Filename:       bcmAlloc.h
//  Author:         Dannie Gay/David Pullen
//  Creation Date:  Feb 25, 2002
//
//****************************************************************************
//  Description:
//      Helper functions to handle low-level, efficient buffer allocation and
//      deallocation for packets, buffer descriptors, packet descriptors, etc.
//      Buffer allocated by these methods are intended to be used for DMA, not
//      for general data storage (use malloc for general-purpose data buffers).
//
//****************************************************************************

#ifndef BCMALLOC_H
#define BCMALLOC_H

#if defined __cplusplus
extern "C" {
#endif

// Visual SlickEdit gets confused by the open { above, and forces the code that
// follows to be indented.  This close } satisfies the editor and makes life
// easier.  Because it's inside the #if 0, the compiler will ignore it.
#if 0
}
#endif

//********************** Include Files ***************************************

// For size_t
#include <stddef.h>

// For BOOL
#include "bcmtypes.h"

#if defined (MMU_SYSTEM_ON)
// MMU Based Systems require a Physical to Virtual Address Translation Table
typedef struct
{
    uint32 physAddress;
    uint32 pBufNonCache;
    uint32 pBufCache;
    uint32 length;
    uint32 nodeIndex;
    uint32 bufferSizeBytes;
} PhysToVirtTable;

// define external declaration here, exclude for actual implementation (to allow for MACRO use)
#if !defined(PHYS_TO_VIRT_TABLE_IMPL)
//  For Macro Use.  Exclude extern in actual implementation (bcmAlloc.c)
extern PhysToVirtTable * pgPhysToVirtTable;
extern uint8 gNumberOfNodes;
#endif
#endif


//********************** Global Types ****************************************

// With the updated BcmAlloc, you can set not only the number of buffers that
// are available, but also the buffer sizes and the number of sizes.  Previously
// you could only set the number of buffers; the buffer sizes were fixed.  This
// new method allows the all of the DMA buffer characteristics to be tuned
// according to the system requirements.
//
// For example, a simple bridging cable modem may only need 4 types of buffers
// (256 buffers of 16 bytes, 256 buffers of 48 bytes, 144 buffers of 288 bytes,
// and 600 buffers of 1650 bytes); on the other hand, a residential gateway may
// need 8 types of buffers, and more of them.
//
// NOTE:  When selecting buffer sizes, it is best to choose values that are
// multiples of the DCache line size.  If you choose a value that is not a
// multiple of the cache line size, we will have to pad it out to fill the
// cache line anyway in order avoid coherency problems due to flushing and
// invalidating.
typedef struct
{
    // This sets the number of buffer types.
    uint8 numberOfBufferTypes;

    // Pointer to an array of buffer sizes, one for each type.  The array MUST
    // be in ascending order.
    const uint16 *bufferSizes;

    // Pointer to an array for the number of buffers of each size.  The array
    // MUST match the order of the buffer sizes.
    const uint16 *numberOfBuffers;

    // In supported silicon hardware platforms a hardware free pool manager is available for
    // data buffer management.  If supported the default settings will be passed in the members below:
#if defined (BCM_HARDWARE_FREE_POOL_MANAGER)
    // specifies whether or not FPM is enabled.
    BOOL   fpmEnabled;
    // total configured size of the FPM Sram (0=use default max Hardware supported size)
    uint16 fpmTotalMemSize;       
    // size of each buffer in pool  (0=use default max Hardware supported size)
    uint16 fpmBufferSize;    
    // number of buffers configured for FPM (i.e. FPM fifo unit size)
    uint16 fpmNumBuffers;       
#endif

} BcmAllocBufferConfig;


/* Here is a specific code snippet for the cable modem example listed above;
   note that the actual values used will typically be either compile time or
   read from nonvol:
{
    enum
    {
        kNumberOfSizes = 4
    };

    BcmAllocBufferConfig bufferConfig;
    uint16 bufferSizes[kNumberOfSizes] =     { 16,  48, 288, 1650};
    uint16 numberOfBuffers[kNumberOfSizes] = {256, 256, 144,  600};

    bufferConfig.numberOfBufferTypes = kNumberOfSizes;
    bufferConfig.bufferSizes = bufferSizes;
    bufferConfig.numberOfBuffers = numberOfBuffers;

    BcmAllocConfigure(&bufferConfig);
}
*/

//********************** Global Constants ************************************

// Maximum node size supported by BcmAlloc.  At one time, this was a constant,
// but now the client sets it during BcmAllocConfigure(), so we have to call
// a function to query it.
#define MAX_BCM_ALLOC_NODE_SIZE BcmAllocGetMaxBufferSize()

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************


// This function MUST be called by a client (prior to calling BcmAllocInit) to
// set the number of buffers of each size that will be allocated.  This allows
// the buffer allocations to be tuned for different platforms based on the
// number and type of interfaces that are enabled, application features, etc.
//
// PR1929
//
// Parameters:
//      bufferConfig - struct containing the buffer allocations to be used.
//
// Returns:  Nothing.
//
void BcmAllocConfigure(BcmAllocBufferConfig bufferConfig);

// Returns the maximum buffer size that was configured in BcmAllocConfigure(),
// and that has buffers assigned for that size (it is possible to configure a
// size with 0 buffers).
//
// This used to be a constant, but now it is configurable, so you must query
// it.
//
// Parameters:  None.
//
// Returns:
//      The maximum buffer size that can be allocated via BcmAlloc().
//
uint16 BcmAllocGetMaxBufferSize(void);

// Called by drivers to get the BcmAlloc driver to initialize its memory pool.
// It is safe to call this more than once; the buffers will only be allocated
// once.
//
// Parameters:  None.
//
// Returns:
//      0 if it initialized successfully.
//      -1 if there was a problem.
//
int BcmAllocInit(void);

// Allocates a buffer of the specified size, and returns a pointer to it.  The
// buffer will be at least the specified number of bytes, but may be larger.
// If the request can't be satisfied (size too big, or no more buffers
// available) then we will return NULL.
//
// The dcache associated with the buffer will be invalidated unless you pass in
// FALSE as the invalidate parameter.  If you are allocating the buffer for
// receive DMA purposes, then you MUST specify TRUE for this parameter.  If you
// are allocating the buffer so that s/w can write to it and then use it for
// transmit DMA, then you can specify FALSE, since you're going to overwrite
// whatever is in dcache anyway.
//
// NOTE:  All buffer sizes can now be used for DMA operations.
//
// Parameters:
//      bufferSize - the buffer size (in bytes) to allocate.
//      cached - TRUE to return the buffer in cached memory space, FALSE to
//               return the buffer in noncached memory space.
//      invalidate - TRUE if the dcache associated with the buffer should be
//                   invalidated, FALSE if we can skip invalidation.  You should
//                   only pass in FALSE if you are allocating a cached buffer,
//                   and if s/w will be writing to it (not for receive DMA).
//
// Returns:
//      A pointer to the buffer allocated, or NULL if none available.
//
void *BcmAllocInvalidate(size_t bufferSize, BOOL cached, BOOL invalidate);

// This is for the convenience of old-school code that hasn't been updated to
// use the new API.  This version simply calls BcmAllocInvalidate with TRUE
// as the invalidate parameter.
#define BcmAlloc(bufferSize, cached) BcmAllocInvalidate(bufferSize, cached, TRUE)

// This is like BcmAlloc, except that it does bulk allocation of multiple
// buffers of the same size.  This is useful (for example) when filling a
// receive DMA ring, where you need to allocate a buffer of the same size for
// each Buffer Descriptor.  Doing a bulk alloc can save a good deal of CPU time
// since the work of finding the appropriate buffer, handling interrupt locking,
// promoting buffers, etc, will only be done once.
//
// Parameters:
//      bufferSize - the buffer size (in bytes) to allocate.
//      numBuffers - the number of buffers of bufferSize bytes to allocate.
//      pBufferArray - pointer to an array of void *'s; the allocated buffers
//                     will be stored in this array.  The array must contain
//                     at least numBuffers entries.
//      cached - TRUE to return the buffers in cached memory space, FALSE to
//               return the buffers in noncached memory space.
//      invalidate - TRUE if the dcache associated with the buffers should be
//                   invalidated, FALSE if we can skip invalidation.  You should
//                   only pass in FALSE if you are allocating a cached buffer,
//                   and if s/w will be writing to it (not for receive DMA).
//
// Returns:
//      The number of buffers that were actually allocated.  This will be less
//      than or equal to numBuffers.
//
unsigned int BcmBulkAlloc(size_t bufferSize, unsigned int numBuffers,
                          unsigned int **pBufferArray, BOOL cached,
                          BOOL invalidate);

// Frees a buffer previously allocated via BcmAlloc or BcmBulkAlloc.
//
// You also need to indicate the number of bytes that were used in the buffer
// (-1 means all of them).  This is used to update the buffer statistics so that
// we know how many bytes to invalidate when it is allocated next time.  For
// example, if you allocated a 1600 byte buffer, but only used 64 bytes, then
// specifying 64 for bytesUsed will keep us from having to invalidate 1600 bytes
// when it is next allocated.
//
// However, if you play tricks with the buffer, such as carving structures out
// at the end of the buffer, then there will be a disjoint region of used data
// within the buffer, all of which needs to be invalidated.  In this case, your
// options are:
//
//   Safe - specify -1 for bytesUsed so that the whole buffer is invalidated.
//   Fast - invalidate the structure area at the end yourself, then specify the
//          number of bytes used from the rest of the buffer that were used.
//
// NOTE:  If this buffer was not allocated using BcmAlloc or BcmBulkAlloc, the
//        system can crash!
//
// Parameters:
//      pMemory - pointer to memory to be freed.
//      bytesUsed - the number of bytes that were used (-1 means all of them).
//    
// Return:  Nothing.
//
void BcmFreeBytesUsed(void *pMemory, size_t bytesUsed);

// This is for the convenience of old-school code that hasn't been updated to
// use the new API.  This version simply calls BcmFreeBytesUsed with -1 as the
// number of bytes used (which means that all bytes were used).
#define BcmFree(pMemory) BcmFreeBytesUsed(pMemory, (size_t) -1)

// Gets the BcmAlloc driver to print its internal state to the console (using
// printf).
//
// Parameters:
//      zap - if TRUE, then the counters will be cleared after printing.  If
//            FALSE, then they will be left alone.
//
// Returns:  Nothing.
//
void BcmAllocShow(BOOL zap);

// If BCM_ALLOC_TRACK is enabled (in bcmAlloc.c), this walks the lists of
// free and allocated buffers, displaying information about them (like the
// size, buffer address, and the return address of the function that called
// BcmAlloc/BcmFree).
//
void BcmAllocWalk(void);


#if defined __cplusplus
}
#endif

#endif


