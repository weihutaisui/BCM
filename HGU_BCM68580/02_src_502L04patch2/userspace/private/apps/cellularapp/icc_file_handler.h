/*
 <:copyright-BRCM:2015:NONE/NONE:standard
 
    Copyright (c) 2015 Broadcom 
    All Rights Reserved
 
 :>

 *****************************************************************************
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
 * ****************************************************************************

*/

#ifndef _ICC_FILE_HANDLER_H
#define _ICC_FILE_HANDLER_H

/* GSM SIM file ids from TS 51.011 */
typedef enum { 
    EF_ICCID =   0x2fe2,
    EF_MBI   = 0x6fc9,
    EF_MSISDN =   0x6f40,
    EF_AD   = 0x6FAD,
    EF_MWIS  =  0x6FCA,
    EF_VOICE_MAIL_INDICATOR_CPHS =   0x6F11,
    EF_CFIS  =  0x6FCB,
    EF_CFF_CPHS  =  0x6F13,
    EF_SPDI  =  0x6fcd,
    EF_PNN   = 0x6fc5,
    EF_SST   = 0x6f38,
    EF_INFO_CPHS  =  0x6f16,
    EF_CSP_CPHS  =  0x6f15,
    EF_GID1   = 0x6F3E,
    EF_SMS    = 0x6F3C,
    EF_CBMI   = 0x6f45,
    EF_EXT1 = 0x6f4a
}EfFileId;

/* From TS 11.11 9.1 or elsewhere */
typedef enum {
    COMMAND_READ_BINARY = 0xb0,
    COMMAND_UPDATE_BINARY = 0xd6,
    COMMAND_READ_RECORD = 0xb2,
    COMMAND_UPDATE_RECORD = 0xdc,
    COMMAND_SEEK = 0xa2,
    COMMAND_GET_RESPONSE = 0xc0
}EfCommandId;

/* Size of GET_RESPONSE for EF's */
typedef enum {
    GET_RESPONSE_EF_SIZE_BYTES = 15,
    GET_RESPONSE_EF_IMG_SIZE_BYTES = 10
}EfGetResponseSize;

/* Byte order received in response to COMMAND_GET_RESPONSE 
 * Refer TS 51.011 Section 9.2.1
 */
typedef enum {
    RESPONSE_DATA_RFU_1 = 0,
    RESPONSE_DATA_RFU_2 = 1,
    RESPONSE_DATA_FILE_SIZE_1 = 2,
    RESPONSE_DATA_FILE_SIZE_2 = 3,
    RESPONSE_DATA_FILE_ID_1 = 4,
    RESPONSE_DATA_FILE_ID_2 = 5,
    RESPONSE_DATA_FILE_TYPE = 6,
    RESPONSE_DATA_RFU_3 = 7,
    RESPONSE_DATA_ACCESS_CONDITION_1 = 8,
    RESPONSE_DATA_ACCESS_CONDITION_2 = 9,
    RESPONSE_DATA_ACCESS_CONDITION_3 = 10,
    RESPONSE_DATA_FILE_STATUS = 11,
    RESPONSE_DATA_LENGTH = 12, 
    RESPONSE_DATA_STRUCTURE = 13,
    RESPONSE_DATA_RECORD_LENGTH = 14
}CommandGetResponseByteOrder;

/* Type of files TS 11.11 9.3 */
typedef enum {
    TYPE_RFU = 0,
    TYPE_MF = 1,
    TYPE_DF = 2,
    TYPE_EF = 4
}EfFileType_0;

/* Type of files TS 11.11 9.3 */
typedef enum {
    EF_TYPE_TRANSPARENT = 0,
    EF_TYPE_LINEAR_FIXED = 1,
    EF_TYPE_CYCLE = 3
}EfFileType_1;

/* From TS 11.11 9.2.5 */
typedef enum {
    READ_RECORD_MODE_ABSOLUTE = 4
}ReadRecordMode;

typedef struct icc_io_result
{
    int sw1;
    int sw2;
    int efid;
    int index;
    int length;
    UINT8 *payload;
}IccIoResult;

typedef struct load_linear_fixed_context
{
    int mEfId;
    int mRecordNum;
    int mRecordSize;
    int mCountRecords;
    UBOOL8 mLoadAll;
    struct internal_message* onLoaded;

}LoadLinearFixedContext;

void icc_file_handler_loadEFLinearFixed(int fileid, int recordNum, 
                                 const char *aid, struct internal_message* msg);
void icc_file_handler_init();
void icc_file_handler_process(struct internal_message *msg);
UINT8* icc_util_hexStringToBytes(const char *s, int* pdu_len);
#endif
