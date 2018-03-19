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


#include <stdint.h>


#define ALOGV(...) 
//((void) printf(__VA_ARGS__))
#define ALOGE(...) 
//((void) printf(__VA_ARGS__))

#define size_t int


#ifdef SAMOS_BCECPE_INTG
namespace samos_bcecpe {
#endif


#define NO_ERROR 0
#define NO_MEMORY 1
#define BAD_VALUE 2
#define NOT_ENOUGH_DATA 3

typedef  int status_t;
typedef struct parcel{
    status_t            mError;
    uint8_t*            mData;
    size_t              mDataSize;
    size_t              mDataCapacity;
    size_t              mDataPos;
}Parcel;


const uint8_t*  parcel_data(struct parcel* p);
size_t          parcel_dataSize(struct parcel* p);
size_t          parcel_dataAvail(struct parcel* p);
size_t          parcel_dataPosition(struct parcel* p);
size_t          parcel_dataCapacity(struct parcel* p);

status_t        parcel_setDataSize(struct parcel* p, size_t size);
void            parcel_setDataPosition(struct parcel *p, size_t pos);
status_t        parcel_setDataCapacity(struct parcel *p, size_t size);

status_t        parcel_setData(struct parcel *p, const uint8_t* buffer, size_t len);
status_t parcel_appendFrom(struct parcel *p, 
                           const struct parcel *that, size_t offset, size_t len);

void            parcel_freeData(struct parcel *p);
status_t        parcel_write(struct parcel *p, const void* data, size_t len);
void*           parcel_writeInplace(struct parcel *p, size_t len);
status_t        parcel_writeInt32(struct parcel *p, int32_t val);
status_t        parcel_writeInt64(struct parcel *p, int64_t val);

status_t        parcel_writeCString(struct parcel *p, const char* str);
status_t        parcel_read(struct parcel *p, void* outData, size_t len);
const void*     parcel_readInplace(struct parcel *p, size_t len);
status_t        parcel_readInt32(struct parcel *p, int32_t *pArg);
status_t        parcel_readInt64(struct parcel *p, int64_t *pArg);
const char*     parcel_readCString(struct parcel *p);

struct parcel* parcel_obtain();
void parcel_free(struct parcel *p);


