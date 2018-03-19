/*
 <:copyright-BRCM:2015:NONE/NONE:standard
 
    Copyright (c) 2015 Broadcom 
    All Rights Reserved
 
 :>
 ******************************************************************************
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "parcel.h"



#ifndef INT32_MAX
#define INT32_MAX ((int32_t)(2147483647))
#endif

#define LOG_REFS(...)
//#define LOG_REFS(...) ALOG(LOG_DEBUG, "Parcel", __VA_ARGS__)

// ---------------------------------------------------------------------------

#define PAD_SIZE(s) (((s)+3)&~3)


static void parcel_initState(struct parcel *p);
static void parcel_freeDataNoInit(struct parcel *p);
static status_t parcel_restartWrite(struct parcel *p, size_t desired);
static status_t parcel_continueWrite(struct parcel *p, size_t desired);
static status_t parcel_growData(struct parcel *p, size_t len);
static status_t parcel_writeAligned(struct parcel *p, void* val, size_t size);

struct parcel* parcel_obtain()
{
    struct parcel *p = (struct parcel*) malloc(sizeof(struct parcel));
    if(p != NULL)
    {
        parcel_initState(p);
    }
    return p;
}


void parcel_free(struct parcel *p)
{
    parcel_freeDataNoInit(p);
}

const uint8_t* parcel_data(struct parcel *p)
{
    return p->mData;
}

size_t parcel_dataSize(struct parcel *p)
{
    return (p->mDataSize > p->mDataPos ? p->mDataSize : p->mDataPos);
}

size_t parcel_dataAvail(struct parcel *p)
{
    // TODO: decide what to do about the possibility that this can
    // report an available-data size that exceeds a Java int's max
    // positive value, causing havoc.  Fortunately this will only
    // happen if someone constructs a Parcel containing more than two
    // gigabytes of data, which on typical phone hardware is simply
    // not possible.
    return parcel_dataSize(p) - parcel_dataPosition(p);
}

size_t parcel_dataPosition(struct parcel *p)
{
    return p->mDataPos;
}

size_t parcel_dataCapacity(struct parcel *p)
{
    return p->mDataCapacity;
}


status_t parcel_setDataSize(struct parcel *p, size_t size)
{
    status_t err;
    err = parcel_continueWrite(p, size);
    if (err == NO_ERROR) {
        p->mDataSize = size;
        ALOGV("setDataSize Setting data size of %p to %d\n", p, p->mDataSize);
    }
    return err;
}

void parcel_setDataPosition(struct parcel *p, size_t pos)
{
    p->mDataPos = pos;
}

status_t parcel_setDataCapacity(struct parcel *p, size_t size)
{
    if (size > p->mDataCapacity) return parcel_continueWrite(p, size);
    return NO_ERROR;
}

status_t parcel_setData(struct parcel *p, const uint8_t* buffer, size_t len)
{
    status_t err = parcel_restartWrite(p, len);
    if (err == NO_ERROR) {
        memcpy(p->mData, buffer, len);
        p->mDataSize = len;
    }
    return err;
}

status_t parcel_appendFrom(struct parcel *p, 
                           const struct parcel *that, size_t offset, size_t len)
{
//    const sp<ProcessState> proc(ProcessState::self());
    status_t err;
    const uint8_t *data = that->mData;

    err = NO_ERROR;


    if (len == 0)
    {
        return NO_ERROR;
    }

    // range checks against the source parcel size
    if ((offset > that->mDataSize)
            || (len > that->mDataSize)
            || (offset + len > that->mDataSize)) 
    {
        return BAD_VALUE;
    }

    if ((p->mDataSize+len) > p->mDataCapacity) 
    {
        // grow data
        err = parcel_growData(p, len);
        if (err != NO_ERROR) 
        {
            return err;
        }
    }

    // append data
    memcpy(p->mData + p->mDataPos, data + offset, len);
    p->mDataPos += len;
    p->mDataSize += len;

    return err;
}

static status_t parcel_finishWrite(struct parcel *p, size_t len)
{
    //printf("Finish write of %d\n", len);
    p->mDataPos += len;
    ALOGV("finishWrite Setting data pos of %p to %d\n", p, p->mDataPos);
    if (p->mDataPos > p->mDataSize)
    {
        p->mDataSize = p->mDataPos;
        ALOGV("finishWrite Setting data size of %p to %d\n", p, p->mDataSize);
    }
    //printf("New pos=%d, size=%d\n", mDataPos, mDataSize);
    return NO_ERROR;
}

status_t parcel_write(struct parcel *p, const void* data, size_t len)
{
    void* const d = parcel_writeInplace(p, len);
    if (d) {
        memcpy(d, data, len);
        return NO_ERROR;
    }
    return p->mError;
}

void* parcel_writeInplace(struct parcel *p, size_t len)
{
    uint8_t* data;
    const size_t padded = PAD_SIZE(len);

    // sanity check for integer overflow
    if (p->mDataPos+padded < p->mDataPos) {
        return NULL;
    }

    if ((p->mDataPos+padded) <= p->mDataCapacity) {
restart_write:
        //printf("Writing %ld bytes, padded to %ld\n", len, padded);
        data = p->mData + p->mDataPos;

        // Need to pad at end?
        if (padded != len) {
#if BYTE_ORDER == BIG_ENDIAN
            static const uint32_t mask[4] = {
                0x00000000, 0xffffff00, 0xffff0000, 0xff000000
            };
#endif
#if BYTE_ORDER == LITTLE_ENDIAN
            static const uint32_t mask[4] = {
                0x00000000, 0x00ffffff, 0x0000ffff, 0x000000ff
            };
#endif
            //printf("Applying pad mask: %p to %p\n", (void*)mask[padded-len],
            //    *reinterpret_cast<void**>(data+padded-4));
            *(uint32_t*)(data+padded-4) &= mask[padded-len];
        }

        parcel_finishWrite(p, padded);
        return data;
    }

    status_t err = parcel_growData(p, padded);
    if (err == NO_ERROR) goto restart_write;
    return NULL;
}

status_t parcel_writeInt32(struct parcel* p, int32_t val)
{
    return parcel_writeAligned(p, &val, sizeof(int32_t));
}
status_t parcel_writeInt64(struct parcel *p, int64_t val)
{
    return parcel_writeAligned(p, &val, sizeof(int64_t));
}

status_t parcel_writeCString(struct parcel *p, const char* str)
{
    if(str == NULL)
    {
        return parcel_writeInt32(p, 0);
    }
    else
    {
        return parcel_write(p, str,strlen(str)+1);
    }
}

status_t parcel_read(struct parcel *p, void* outData, size_t len)
{
    if ((p->mDataPos + PAD_SIZE(len)) >= p->mDataPos && 
            (p->mDataPos + PAD_SIZE(len)) <= p->mDataSize && 
            len <= PAD_SIZE(len)) {
        memcpy(outData, p->mData + p->mDataPos, len);
        p->mDataPos += PAD_SIZE(len);
        ALOGV("read Setting data pos of %p to %d\n", p, p->mDataPos);
        return NO_ERROR;
    }
    return NOT_ENOUGH_DATA;
}

const void* parcel_readInplace(struct parcel *p, size_t len)
{
    if ((p->mDataPos + PAD_SIZE(len)) >= p->mDataPos && 
            (p->mDataPos + PAD_SIZE(len)) <= p->mDataSize && 
            len <= PAD_SIZE(len))
    {
        const void* data = p->mData + p->mDataPos;
        p->mDataPos += PAD_SIZE(len);
        ALOGV("readInplace Setting data pos of %p to %d\n", p, p->mDataPos);
        return data;
    }
    return NULL;
}

static status_t parcel_readAligned(struct parcel *p, void *pArg, size_t size)
{
//    COMPILE_TIME_ASSERT_FUNCTION_SCOPE(PAD_SIZE(sizeof(T)) == sizeof(T));

    if ((p->mDataPos + size) <= p->mDataSize)
    {
        const void* data = p->mData + p->mDataPos;
        p->mDataPos += size;
        memcpy(pArg, data, size);
        return NO_ERROR;
    }
    else 
    {
        return NOT_ENOUGH_DATA;
    }
}


static status_t parcel_writeAligned(struct parcel *p, void* val, size_t size) 
{
//    COMPILE_TIME_ASSERT_FUNCTION_SCOPE(PAD_SIZE(sizeof(T)) == sizeof(T));

    if ((p->mDataPos + size) <= p->mDataCapacity) 
    {
restart_write:
        memcpy((p->mData + p->mDataPos), val, size);
        return parcel_finishWrite(p, size);
    }

    status_t err = parcel_growData(p, size);
    if (err == NO_ERROR) goto restart_write;
    return err;
}

status_t parcel_readInt32(struct parcel *p, int32_t *pArg)
{
    return parcel_readAligned(p, pArg, sizeof(int32_t));
}


status_t parcel_readInt64(struct parcel *p, int64_t *pArg)
{
    return parcel_readAligned(p, pArg, sizeof(int64_t));
}

const char* parcel_readCString(struct parcel *p)
{
    const size_t avail = p->mDataSize - p->mDataPos;
    if (avail > 0) 
    {
        const char* str = (const char*) (p->mData + p->mDataPos);
        // is the string's trailing NUL within the parcel's valid bounds?
        const char* eos = (const char*) (memchr(str, 0, avail));
        if (eos) 
        {
            const size_t len = eos - str;
            p->mDataPos += PAD_SIZE(len+1);
            ALOGV("readCString Setting data pos of %p to %d\n", p, p->mDataPos);
            if (len)
            {
                return str;
            }
            else
            {
                return NULL;
            }
        }
    }
    return NULL;
}

void parcel_freeData(struct parcel *p)
{
    parcel_freeDataNoInit(p);
    parcel_initState(p);
}

static void parcel_freeDataNoInit(struct parcel *p)
{
    if (p->mData) free(p->mData);
}

static status_t parcel_growData(struct parcel *p, size_t len)
{
    size_t newSize = ((p->mDataSize + len)*3)/2;
    return (newSize <= p->mDataSize)
            ? (status_t) NO_MEMORY
            : parcel_continueWrite(p, newSize);
}

static status_t parcel_restartWrite(struct parcel *p, size_t desired)
{
    parcel_freeData(p);
    return parcel_continueWrite(p, desired);
}

static status_t parcel_continueWrite(struct parcel *p, size_t desired)
{
     if (p->mData) 
     {
      
        // We own the data, so we can just do a realloc().
        if (desired > p->mDataCapacity) 
        {
            uint8_t* data = (uint8_t*)realloc(p->mData, desired);
            if (data) 
            {
                p->mData = data;
                p->mDataCapacity = desired;
            }
            else if (desired > p->mDataCapacity) 
            {
                p->mError = NO_MEMORY;
                return NO_MEMORY;
            }
        }
        else 
        {
            if (p->mDataSize > desired) 
            {
                p->mDataSize = desired;
                ALOGV("continueWrite Setting data size of %p to %d\n", p, p->mDataSize);
            }
            if (p->mDataPos > desired) 
            {
                p->mDataPos = desired;
                ALOGV("continueWrite Setting data pos of %p to %d\n", p, p->mDataPos);
            }
        }
        
    } 
    else
    {
        // This is the first data.  Easy!
        uint8_t* data = (uint8_t*)malloc(desired);
        if (!data) 
        {
            p->mError = NO_MEMORY;
            return NO_MEMORY;
        }

        if(!(p->mDataCapacity == 0 )) 
        {
            ALOGE("continueWrite: %d %d", p->mDataCapacity, desired);
        }
        memset(data, 0 , desired);
        p->mData = data;
        p->mDataSize = p->mDataPos = 0;
        ALOGV("continueWrite Setting data size of %p to %d\n", p, p->mDataSize);
        ALOGV("continueWrite Setting data pos of %p to %d\n", p, p->mDataPos);
        p->mDataCapacity = desired;
    }

    return NO_ERROR;
}

static void parcel_initState(struct parcel *p)
{
    assert(p);

    p->mError = NO_ERROR;
    p->mData = 0;
    p->mDataSize = 0;
    p->mDataCapacity = 0;
    p->mDataPos = 0;
    ALOGV("initState Setting data size of %p to %d\n", p, p->mDataSize);
    ALOGV("initState Setting data pos of %p to %d\n", p, p->mDataPos);
}

