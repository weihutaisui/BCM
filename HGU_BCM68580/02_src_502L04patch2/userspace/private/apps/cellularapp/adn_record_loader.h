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

#ifndef _ADN_RECORD_H
#define _ADN_RECORD_H

typedef struct adn_record
{
    char mAlphaTag[BUFLEN_64];
    char mNumber[BUFLEN_32];
    int mExtRecord;
    int mEfid;
    int mRecordNumber;
}AdnRecord;

typedef struct adn_record_loader_context
{
    int mEf;
    int mExtensionEf;
    int mRecordNumber;
    int mPendingExtLoads;
    struct adn_record adn;
    struct internal_message *callback_msg;
}AdnRecordLoaderContext;


void adn_record_loader_process(struct internal_message *msg);

void adn_record_LoadFromEF(int ef, int extensionEF, int recordNumber, 
                           const char *aid, struct internal_message* msg);

#endif
