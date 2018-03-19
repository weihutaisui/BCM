/*
 <:copyright-BRCM:2015:NONE/NONE:standard
 
    Copyright (c) 2015 Broadcom 
    All Rights Reserved
 
 :>
 ****************************************************************************
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


#include <stdlib.h>
#include <assert.h>

#include "cms_log.h"
#include "cellular_msg.h"


struct internal_message* internal_message_obtain(int event, int dest)
{
    struct internal_message* msg;
    msg = (struct internal_message*)malloc(sizeof(struct internal_message));
    if(msg != NULL)
    {
        msg->what = event;
        msg->dest = dest;
        msg->object = NULL;
    }
    return msg;
}

struct internal_message * internal_message_obtain_with_data(int event, int dest, const void* data, int size)
{
    struct internal_message *msg = internal_message_obtain(event, dest);
    Object *o = object_obtain();
	
    assert(msg);
    assert(o);
	
    object_setData(o, data, size);
    internal_message_setObject(msg, o);

    return msg;	
}

void internal_message_freeObject(struct internal_message *msg)
{
    assert(msg);
    if(msg->object != NULL)
    {
        object_freeData(msg->object);
        free(msg->object);
        msg->object = NULL;
    }
}

void internal_message_free_and_null_ptr(struct internal_message **msg)
{
   if (*msg != NULL)
   { 
       internal_message_freeObject(*msg);
       free(*msg);
       *msg = NULL;
   }
}

/* the "object" of internal message is always a copy of the content */
void internal_message_setObject(struct internal_message *msg, Object *object)
{
    assert(msg);

    msg->object = object;
}

struct object* internal_message_getObject(struct internal_message *msg)
{
    assert(msg);
    return msg->object;
}

struct object * object_obtain()
{
    struct object *new_object;
    new_object = (struct object *)malloc(sizeof(struct object));
    if(new_object != NULL)
    {
        new_object->data_len = 0;
        new_object->data = NULL;
    }
    return new_object;
}

UBOOL8 object_setData(struct object* t, const void* data, int size)
{
    assert(t);
    if(t->data != NULL)
    {
        cmsLog_notice("there's valid data attached to the object! free it first..\n");
        object_freeData(t);
    }
    
    /*
     * If size is 0, some malloc implementation would happily allocate 
     * zero-sized buffer.We would check the size here to make sure it is 
     * greater than 0.
     */
    if(size <= 0)
    {
        return FALSE;
    }

    t->data = (void *)malloc(size);
    if(t->data != NULL)
    {
        memcpy(t->data, data, size);
        t->data_len = size;
        return TRUE;
    }
    return FALSE;
}

struct object * object_obtain_with_data(const void* data, int size)
{
    struct object *t;
	
    assert(size > 0);
	
    t = (struct object *)malloc(sizeof(struct object));	
    assert(t);
    
    t->data = (void *)malloc(size);
    assert(t->data);
	
    memcpy(t->data, data, size);
    t->data_len = size;
	
    return t;
}

void object_freeData(struct object *t)
{
    assert(t);
    if(t->data_len != 0)
    {
        assert(t->data);
        free(t->data);
        t->data = NULL;
        t->data_len = 0;
    }
}

UBOOL8 object_appendData(struct object *t, void *data, size_t data_len)
{
    if(t->data_len == 0)
    {
        /* 
         * We don't have any data attached to the target object. Just setData
         * to it
         */
        object_setData(t, data, data_len);
        return TRUE;
    }
    else if(data_len > 0)
    {
        /* 
         * There is some data attached to the object. We need to append the new
         * data to the original data.
         */
        void *old_data = t->data;
        int old_data_len = t->data_len;
        t->data = (void *)malloc(old_data_len + data_len);
        if(t->data != NULL)
        {
            memcpy(t->data, old_data, old_data_len);
            memcpy(t->data+old_data_len, data, data_len);
            t->data_len = old_data_len + data_len;
            free(old_data);
            return TRUE;
        }
        else
        {
            t->data = old_data;
            t->data_len = old_data_len;
            return FALSE;
        }
    }
    return FALSE;
}

void* object_getData(struct object *t)
{
    assert(t);
    return (t->data);
}

int object_dataLength(struct object *t)
{ 
    assert(t);
    return t->data_len;
}

UBOOL8 object_containsData(struct object *t)
{
    assert(t);
    return (t->data_len>0? TRUE : FALSE );
}
