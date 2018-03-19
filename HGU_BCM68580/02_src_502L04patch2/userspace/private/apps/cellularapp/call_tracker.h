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

#ifndef _CALL_TRACKER_H
#define _CALL_TRACKER_H 

#include "cms.h"
#include "ril.h"
#include "cellular_msg.h"


void call_tracker_process(struct internal_message *msg);
void call_tracker_init();
Object* call_tracker_getCurrentCalls();

#endif
