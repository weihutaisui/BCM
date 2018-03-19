/*
* <:copyright-BRCM:2015:proprietary:gpon
* 
*    Copyright (c) 2015 Broadcom 
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

/****************************************************************************/
/*                                                                          */
/* Software unit logger.                                                    */
/*                                                                          */
/* Title:                                                                   */
/*                                                                          */
/*   Logger mechanism.                                                      */
/*                                                                          */
/* Abstract:                                                                */
/*                                                                          */
/*   The logger is a run-time multi-level logging mechanism.                */
/*                                                                          */
/* Requirements allocated to this software unit:                            */
/*                                                                          */
/****************************************************************************/


#include "bdmf_system.h"
#include "logger.h"
#include <linux/string.h>
#include <linux/stddef.h>
#include <linux/ctype.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/sched.h>		
#include <linux/module.h>
#include <linux/fs.h>
#include "logger_dev.h"
#if defined CONFIG_BCM96838 || defined CONFIG_BCM96848
#include <rdp_ms1588.h>
#else
#include <wan_drv.h>
#endif


#define strtol  simple_strtol
#define strtoul simple_strtoul

#define open sys_open
#define close sys_close
#define read(a,b,c)	 vfs_read((struct file*)a, b, c, 0)
#define write(a,b,c) vfs_write((struct file*)a, b, c, 0)
#define creat sys_creat
#define stat vfs_stat

unsigned long tickGet ( void );
void ctime_r ( time_t * tm, char * xo_buffer, size_t* size);
time_t time ( void );
long fdprintf(long fd,const char *format,...);
static char* find_idx(const char *p, long ch);
 

/******************************************************************************/
/*                                                                            */
/* Declarations                                                               */
/*                                                                            */
/******************************************************************************/
/* Magic */
#define LOGGER_LOG_MAGIC     0xF45AC20F
#define LOGGER_SECTION_MAGIC 0xF45AC21F
#define LOGGER_GROUP_MAGIC   0xF45AC22F
#define LOGGER_ITEM_MAGIC    0xF45AC23F
#define LOGGER_DEVICE_MAGIC  0xF45AC24F
#define LOGGER_DEVICE_STATE_MAGIC 0xF45AC30F
/* Forward declaration to resolve cyclic reference */
struct _device;
struct _item;
struct _group;
struct _section;
struct _log;
struct _device_state;

/* Device control structure */
typedef struct _device
{
    /* Magic */
    unsigned long magic;

    /* Links to parents */
    struct _log *parent_log_ptr;

    /* Link list */
    struct _device *next_device_ptr;

    /* Name and description of the device */
    char name[LOGGER_NAME_LENGTH];

    /* Device control fields */
    unsigned long device;
    unsigned long type;
    long state;

    /* RAM buffer */
    char *ram_buffer_ptr;
    char *ram_buffer_write_ptr;
    char *oldest_ram_buffer_ptr;
    unsigned long ram_buffer_size;
    LOGGER_BUFFER_MODE ram_buffer_mode;
    long has_ram_buffer;
    long has_trigger;
    LOGGER_TRIGGER_ACTION trigger_state;
    long ram_wrapped;
    long device_ram_state;
}
LOGGER_DEVICE;

/* Device control structure */
typedef struct _device_state
{
    /* Magic */
    unsigned long magic;

    /* Link list */
    struct _device_state *next_device_ptr;

    /* Device control fields */
    unsigned long device;
    unsigned long type;
    long object_state;
}
LOGGER_DEVICE_STATE;

/* Item control structure */
typedef struct _item
{
    /* Magic */
    unsigned long magic;
    
    /* Devices */
    struct _device_state *devices_ptr;

    /* Links to parents */
    struct _log *parent_log_ptr;
    struct _group *parent_group_ptr;
    struct _section *parent_section_ptr;

    /* Link list */
    struct _item *next_item_ptr;
    
    /* Name and description of the item */
    char name[LOGGER_NAME_LENGTH];
    char description[LOGGER_DESCRIPTION_LENGTH];

    /* Item control fields */
    long default_state;
    LOGGER_ATTRIBUTES attributes;
    LOGGER_ITEM_TYPE type;
    unsigned long size;
    unsigned long parameter;
    unsigned long throttle_threshold;
    long item_ram_state;
    LOGGER_TRIGGER_ACTION item_trigger_state;
    unsigned long trigger_count;
    unsigned long trigger_current_count;
    unsigned long throttle_counter;
    char format[LOGGER_FORMAT_LENGTH];
}
LOGGER_ITEM;

/* Group control structure */
typedef struct _group
{
    /* Magic */
    unsigned long magic;

    /* Devices */
    struct _device_state *devices_ptr;

    /* Items */
    struct _item *items_ptr;
    
    /* Links to parent */
    struct _log *parent_log_ptr;
    struct _section *parent_section_ptr;

    /* Link list */
    struct _group *next_group_ptr;
    
    /* Name and description of the group */
    char name[LOGGER_NAME_LENGTH];
    char description[LOGGER_DESCRIPTION_LENGTH];

    long default_state;
}
LOGGER_GROUP;

/* Section control structure */
typedef struct _section
{
    /* Magic */
    unsigned long magic;

    /* Devices */
    struct _device_state *devices_ptr;

    /* Groups */
    struct _group *groups_ptr;
    
    /* Links to parent */
    struct _log *parent_log_ptr;

    /* Link list */
    struct _section *next_section_ptr;
    
    /* Name and description of the section */
    char name[LOGGER_NAME_LENGTH];
    char description[LOGGER_DESCRIPTION_LENGTH];

    long default_state;
}
LOGGER_SECTION;

/* Log control structure */
typedef struct _log
{
    /* Magic */
    unsigned long magic;

    /* Sections */
    struct _section *sections_ptr;

    /* Devices */
    struct _device *devices_ptr;

    /* Name and description of the log */
    char name[LOGGER_NAME_LENGTH];
    char description[LOGGER_DESCRIPTION_LENGTH];

    /* Log control fields */
    unsigned long priority;
    unsigned long queue_size;
    LOGGER_TIME_BASE_TYPE time_base;
    unsigned long lost_buffers;
	long current_output_id;

    /* Working areas */
    char *log_item;
    char *final_item;

    /* Partition ID */
    unsigned long partition_id;
    char *partition_buffer; 
    /* Queue ID */
    bdmf_queue_t queue; 

    /* Task ID */
    unsigned long task_id; 
    
    /* Time base */
    LOGGER_TIMESTAMPING_CALLBACK timestamping_callback;
    LOGGER_TIMESTAMP_FORMATTING_CALLBACK timestamp_formatting_callback;

    /* Time of day */
    LOGGER_TIMEOFDAY_CALLBACK timeofday_callback;
    LOGGER_TIMEOFDAY_FORMATTING_CALLBACK timeofday_formatting_callback;

    /* Log information formatting */
    LOGGER_LOG_INFORMATION_FORMATTING_CALLBACK log_information_formatting_callback;

    /* Log item formatting */
    LOGGER_LOG_ITEM_FORMATTING_CALLBACK log_item_formatting_callback;

    /* Notification callback */
    LOGGER_NOTIFICATION_CALLBACK notification_callback;

    /* Link list */
    struct _log *next_log_ptr;

    /* log to file parameters */
    char log_file_name[80];
    long fd_log_file;
}
LOGGER_LOG;

/* Log message */
typedef struct
{
    unsigned char daytime[3];

    /* Timestamp */
    uint64_t timestamp;

    /* Context */
    char context[LOGGER_NAME_LENGTH];

    /* Item */
    struct _item *item_ptr;

    /* File */
    char file[LOGGER_DESCRIPTION_LENGTH];

    /* Line */
    unsigned long line;

    /* Arguments */
    va_list arguments;

    /* Links to parent */
    struct _log *parent_log_ptr;
}
LOGGER_LOG_MESSAGE;

typedef struct
{
    LOGGER_LOG *log_ptr;
    LOGGER_LOG_MESSAGE *log_message_ptr;
}
LOGGER_ENTRY;

LOGGER_CALLBACKS logger_callbacks;

#define CS_NUMBER_OF_LOGGERS 2

LOGGER_ENTRY gs_logger_array[CS_NUMBER_OF_LOGGERS];
#define ALLOCATED_DEVICE_LENGTH       ((sizeof(LOGGER_DEVICE) + 3)& 0xFFFFFFFC)
#define ALLOCATED_DEVICE_STATE_LENGTH ((sizeof(LOGGER_DEVICE_STATE) + 3)& 0xFFFFFFFC)
#define ALLOCATED_ITEM_LENGTH         ((sizeof(LOGGER_ITEM) + 3)& 0xFFFFFFFC)
#define ALLOCATED_GROUP_LENGTH        ((sizeof(LOGGER_GROUP) + 3)& 0xFFFFFFFC)
#define ALLOCATED_SECTION_LENGTH      ((sizeof(LOGGER_SECTION) + 3) & 0xFFFFFFFC)
#define ALLOCATED_LOG_LENGTH          ((sizeof(LOGGER_LOG) + 3) & 0xFFFFFFFC)
#define ALLOCATED_LOG_MESSAGE_LENGTH  ((sizeof(LOGGER_LOG_MESSAGE) + 3)& 0xFFFFFFFC)

#define ALLOCATED_LOG_MESSAGE_RAM (8 *1024)

#define LOGGER_MAX_OBJS_IN_GROUP  100
#define LOGGER_OBJECT_MAX_SIZE 50
#define LOGGER_LIST_MAX_SIZE (LOGGER_OBJECT_MAX_SIZE * LOGGER_MAX_OBJS_IN_GROUP)     

/******************************************************************************/
/*                                                                            */
/* Static subroutines prototypes                                              */
/*                                                                            */
/******************************************************************************/

static long logger_queue_create(bdmf_queue_t *queue, long number_of_messages, long max_message_length);

static void p_default_none_timeofday(unsigned char *xi_timeofday);
static void p_default_none_timeofday_formatting(unsigned char *xi_timeofday, char *xo_buffer);

static void p_default_timeofday(unsigned char *xi_timeofday);
static void p_default_timeofday_formatting(unsigned char *xi_timeofday, char *xo_buffer);

static void p_logger(LOGGER_LOG_MESSAGE *xi_log_message_ptr);

static uint64_t f_default_none_timestamping(void);
static void p_default_none_timestamp_formatting(uint64_t xi_timestamp, char *xo_buffer);

static uint64_t f_default_tick_timestamping(void);
static void p_default_tick_timestamp_formatting(uint64_t xi_timestamp, char *xo_buffer);

static uint64_t f_default_time_timestamping(void);
static void p_default_time_timestamp_formatting(uint64_t xi_timestamp, char *xo_buffer);

static uint64_t f_default_microsec_timestamping(void);
static void p_default_microsec_timestamp_formatting(uint64_t xi_timestamp, char *xo_buffer);

static void p_default_log_information_formatting(char *xi_log, char *xi_time_of_day, 
    char *xi_timestamp, char *xi_file, unsigned long xi_line, LOGGER_SECTION_ID xi_section_id, 
    LOGGER_GROUP_ID xi_group_id, LOGGER_ITEM_ID xi_item_id, char *xi_context, char *xo_log_information);

static void p_default_log_item_formatting(char *xi_log_information, char *xi_item, char *xo_log_item);
static void p_logger_release_os_resources(LOGGER_LOG_ID xi_log_id);
static LOGGER_ERROR f_logger_allocate_device(char *xi_name, long xi_device, unsigned long xi_type, long xi_state, unsigned long *xo_device_ptr);
static LOGGER_ERROR f_logger_define_device(long xi_device, unsigned long xi_type, long xi_object_state, unsigned long *xo_device_state_ptr);
static void p_chain_a_device(LOGGER_DEVICE_STATE *xi_this_device_ptr, LOGGER_DEVICE_STATE *xi_device_ptr);
static void p_set_section_state(unsigned long xi_deviceid, LOGGER_SECTION *xi_section_ptr, long xi_state);
static void p_set_section_ram_state(unsigned long xi_deviceid, LOGGER_SECTION *xi_section_ptr, long xi_in_ram);
static void p_set_group_state(unsigned long xi_deviceid, LOGGER_SECTION *xi_section_ptr, LOGGER_GROUP *xi_group_ptr, long xi_state);
static void p_set_group_ram_state(unsigned long xi_deviceid, LOGGER_SECTION *xi_section_ptr, LOGGER_GROUP *xi_group_ptr, long xi_in_ram);
static void p_set_item_state(unsigned long xi_deviceid, LOGGER_ITEM *xi_item_ptr, long xi_state);
static void p_set_item_ram_state(LOGGER_ITEM *xi_item_ptr, long xi_in_ram);
static long f_put_in_queue(LOGGER_LOG *xi_log_ptr, LOGGER_SECTION *xi_section_ptr, LOGGER_GROUP *xi_group_ptr, LOGGER_ITEM *xi_item_ptr);
static void p_clear_sections(unsigned long xi_device, LOGGER_SECTION *xi_section_ptr);

static long f_match_name(char *xi_name, unsigned long xi_number, ...);

static char gs_logger_path[CS_NUMBER_OF_LOGGERS][ 126];

static char temp1[LOGGER_LIST_MAX_SIZE];


/* Logs counter */
static unsigned long gs_logs = 0;
static struct _log *gs_logs_ptr[CS_NUMBER_OF_LOGGERS] = 
{
        0,
        0
}; 

static void* logger_alloc(long size)
{
	if (logger_callbacks.log_alloc)
		return logger_callbacks.log_alloc(size); 
	return NULL;
}
static void logger_free(void *buffer)
{
	if (logger_callbacks.log_free)
		logger_callbacks.log_free(buffer); 
}

static long logger_queue_create(bdmf_queue_t *queue, long number_of_messages,  long max_message_length)
{
	if (logger_callbacks.log_queue_create)
		return logger_callbacks.log_queue_create(queue, number_of_messages, max_message_length);

	return LOGGER_ERROR_FUNC_NOT_IMPLEMENTED;
}

static long logger_queue_send(bdmf_queue_t *queue, char *buffer, long length)
{
	if (logger_callbacks.log_queue_send)
		return logger_callbacks.log_queue_send(queue, buffer, length); 

	return LOGGER_ERROR_FUNC_NOT_IMPLEMENTED;
}

static long logger_queue_receive_not_blocking(bdmf_queue_t *queue, char *buffer, long *length)
{
	if (logger_callbacks.log_queue_receive_not_blocking)
		return logger_callbacks.log_queue_receive_not_blocking(queue, buffer, length); 

	return LOGGER_ERROR_FUNC_NOT_IMPLEMENTED;
}
/****************************************************************************/
/*                                                                          */
/* Exported subroutines implementation                                      */
/*                                                                          */
/****************************************************************************/
extern struct logger_dev_t *logger_dev; /* allocated in logger_drv_init */

LOGGER_ERROR fi_get_logger_device(LOGGER_LOG_ID *xo_log_id)
{
    long log_error = LOGGER_OBJECT_NOT_FOUND;
    if (logger_dev != NULL)
    {
        *xo_log_id = (LOGGER_LOG_ID)logger_dev->log_id;
        log_error = LOGGER_OK;
    }
    return log_error;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_initialize.                                                  */
/*                                                                          */
/* Title:                                                                   */
/*                                                                          */
/*   Logger - Initialization.                                               */
/*                                                                          */
/* Abstract:                                                                */
/*                                                                          */
/*   The function initializes the logger.                                   */
/*                                                                          */
/* Input:                                                                   */
/*                                                                          */
/*  xi_name - Name of the log                                               */  
/*  xi_description - Description of the log                                 */  
/*  xi_priority - Log task priority                                         */  
/*  xi_stack_size - Log task stack size                                     */  
/*  xi_queue_size - Log queue size                                          */  
/*  xi_path -                                                               */  
/*  xi_time_base - Log time-base type: none | tick | time | user-defined    */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   xo_log_id - Output log ID                                              */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*     LOGGER_GEN_ERROR - Error indication.                                 */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_initialize(char *xi_name, char *xi_description,
    unsigned long xi_priority, unsigned long xi_stack_size, unsigned long xi_queue_size,
    LOGGER_TIME_BASE_TYPE xi_time_base, LOGGER_LOG_ID *xo_log_id, 
    LOGGER_CALLBACKS *xi_logger_callbacks, long console)
{
    if (logger_dev == NULL)
    {
        printk("ERROR Logger initialize\n");
        return LOGGER_OBJECT_NOT_FOUND;
    }
    *xo_log_id = (LOGGER_LOG_ID) logger_dev->log_id;
    return LOGGER_OK;
}

LOGGER_ERROR fi_linux_logger_init(char *xi_name, char *xi_description, 
    unsigned long xi_priority, unsigned long xi_stack_size, 
    unsigned long xi_queue_size, LOGGER_TIME_BASE_TYPE xi_time_base, 
    unsigned long *xo_log_id, LOGGER_CALLBACKS *xi_logger_callbacks, long console)
{
    LOGGER_LOG *log_ptr;
    LOGGER_LOG *this_log_ptr;

    memset(& logger_callbacks, 0, sizeof(LOGGER_CALLBACKS)); 
    if (xi_logger_callbacks)
    	memcpy(&logger_callbacks, xi_logger_callbacks, sizeof(LOGGER_CALLBACKS));
    else
        return LOGGER_GEN_ERROR; 
    /* Allocate area for the control structure */
    log_ptr = (LOGGER_LOG *)logger_alloc(ALLOCATED_LOG_LENGTH); 
    if (log_ptr == NULL)
        return LOGGER_GEN_ERROR+1;

    /* Allocate the working areas */
    log_ptr->log_item = (char *)logger_alloc(LOGGER_BINARY_LENGTH *3); 
    if (log_ptr->log_item == NULL)
    {
    	logger_free(log_ptr);
        return LOGGER_GEN_ERROR+2;
    }
    log_ptr->final_item = (char *)logger_alloc(LOGGER_BINARY_LENGTH *4); 
    if (log_ptr->final_item == NULL )
    {
        logger_free(log_ptr->log_item); 
        logger_free(log_ptr); 
        return LOGGER_GEN_ERROR+3;
    }
	
    /* Set the log control structure */
    log_ptr->magic = LOGGER_LOG_MAGIC;
    strcpy(log_ptr->name, xi_name);
    strcpy(log_ptr->description, xi_description);
    log_ptr->priority = xi_priority;
    log_ptr->queue_size = xi_queue_size;
    log_ptr->time_base = xi_time_base;
    log_ptr->lost_buffers = 0;
	log_ptr->current_output_id = console; 
    log_ptr->devices_ptr = NULL;
    log_ptr->sections_ptr = NULL;
    log_ptr->next_log_ptr = NULL;
    log_ptr->fd_log_file = -1;
    log_ptr->log_file_name[0] = '\0';

    switch(xi_time_base)
    {
    case LOGGER_TIME_BASE_NONE:
        log_ptr->timestamping_callback = f_default_none_timestamping;
        log_ptr->timestamp_formatting_callback = p_default_none_timestamp_formatting;
        log_ptr->timeofday_callback = p_default_none_timeofday;
        log_ptr->timeofday_formatting_callback = p_default_none_timeofday_formatting;
        break;

    case LOGGER_TIME_BASE_TICK:
        log_ptr->timestamping_callback = f_default_tick_timestamping;
        log_ptr->timestamp_formatting_callback = p_default_tick_timestamp_formatting;
        log_ptr->timeofday_callback = p_default_none_timeofday;
        log_ptr->timeofday_formatting_callback = p_default_none_timeofday_formatting;
        break;

    case LOGGER_TIME_BASE_TIME:
        log_ptr->timestamping_callback = f_default_time_timestamping;
        log_ptr->timestamp_formatting_callback = p_default_time_timestamp_formatting;
        log_ptr->timeofday_callback = p_default_timeofday;
        log_ptr->timeofday_formatting_callback = p_default_timeofday_formatting;
        break;

    case LOGGER_TIME_BASE_USER_DEFINED:
        log_ptr->timestamping_callback = NULL;
        log_ptr->timestamp_formatting_callback = NULL;
        log_ptr->timeofday_callback = NULL;
        log_ptr->timeofday_formatting_callback = NULL;
        break;

    case LOGGER_TIME_BASE_MICROSEC:
       log_ptr->timestamping_callback = f_default_microsec_timestamping;
       log_ptr->timestamp_formatting_callback = p_default_microsec_timestamp_formatting;
       log_ptr->timeofday_callback = p_default_none_timeofday;
       log_ptr->timeofday_formatting_callback = p_default_none_timeofday_formatting;
       break;

    default:
       break;
    }

    log_ptr->log_information_formatting_callback = p_default_log_information_formatting;
    log_ptr->log_item_formatting_callback = p_default_log_item_formatting;
    log_ptr->notification_callback = NULL;
	
    /* Link to the end of the logs link list */
    if (gs_logs_ptr [gs_logs] == NULL)
    {
        gs_logger_array[gs_logs].log_ptr = (LOGGER_LOG *)log_ptr;
        gs_logs_ptr[gs_logs] = log_ptr;
    }
    else
    {
        /* Find the end of the list */
        this_log_ptr = gs_logs_ptr[gs_logs]->next_log_ptr;
        while (this_log_ptr->next_log_ptr != NULL)
        {
        	this_log_ptr = this_log_ptr->next_log_ptr;
        }
        
        /* Link to the end of the list */
        this_log_ptr->next_log_ptr = log_ptr;
    }
    gs_logs ++;

    /* Return the log ID */
    *xo_log_id = (LOGGER_LOG_ID)log_ptr;

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_terminate.                                                   */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Termination.                                                  */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function terminates the logger.                                    */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_log_id - Log ID                                                      */  
/*  xi_priority - Log task priority while terminate                         */  
/*  xi_mode - Termination mode:                                             */  
/*    LOGGER_TERMINATION_MODE_GRACEFUL - change logger task priority        */
/*    LOGGER_TERMINATION_MODE_BRUTAL - release all logger resources         */
/*                                        and terminate                     */
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_terminate(LOGGER_LOG_ID xi_log_id, 
    unsigned long xi_priority, LOGGER_TERMINATION_MODE xi_mode)
{
    LOGGER_LOG *log_ptr;
    LOGGER_DEVICE *this_device_ptr;

    /* Get the log */
    log_ptr = (LOGGER_LOG *) xi_log_id;
    if (log_ptr == NULL )
    {
        return LOGGER_GEN_ERROR;
    }
    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC)
    {
        return LOGGER_GEN_ERROR;
    }

    /* Process all messages in queue */
    {
        /* According to the termination mode */
        switch(xi_mode )
        {
        case LOGGER_TERMINATION_MODE_GRACEFUL:
        case LOGGER_TERMINATION_MODE_BRUTAL:

           /* release all the devices resources */
            if (log_ptr->devices_ptr != NULL )
            {
                this_device_ptr = log_ptr->devices_ptr;
                while(this_device_ptr != NULL )
                {
                    logger_clear_device(this_device_ptr ->device);
                    this_device_ptr = this_device_ptr->next_device_ptr;
                }
            }

            /* release OS resources of the logger */
            p_logger_release_os_resources(xi_log_id);
        break;

        default:
            return LOGGER_GEN_ERROR;
        }
    }

    return LOGGER_OK;
}


/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_device.                                                  */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Configuration.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets the default device the logger will print to.         */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_log_id - Log ID                                                      */  
/*  xi_name - Name of the device                                            */  
/*  xi_description - Description of the device                              */  
/*  xi_device - The device FD                                               */  
/*  xi_state - Device state: off | on                                       */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  xo_device_id - The output device ID                                     */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_device(LOGGER_LOG_ID xi_log_id, char *xi_name, 
    char *xi_description, long xi_device, unsigned long xi_type, long xi_state, 
    LOGGER_DEVICE_ID *xo_device_id)
{
    LOGGER_LOG *log_ptr = (LOGGER_LOG *) xi_log_id;
    LOGGER_DEVICE *device_ptr = NULL;
    LOGGER_DEVICE *this_device_ptr = NULL;
    LOGGER_DEVICE_STATE *device_state_ptr = NULL;
    LOGGER_SECTION *this_section_ptr = NULL;
    LOGGER_GROUP *this_group_ptr = NULL;
    LOGGER_ITEM *this_item_ptr = NULL;
    unsigned long returnedptr;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC)
    {
        return LOGGER_GEN_ERROR;
    }

    if (f_logger_allocate_device(xi_name, xi_device, xi_type, xi_state, &returnedptr) != LOGGER_OK)
    {
    	return LOGGER_GEN_ERROR;
    }

    device_ptr = (LOGGER_DEVICE *) returnedptr;
    /* Link to the parent log */
    device_ptr->parent_log_ptr = log_ptr;
    /* Link to the end of the devices link list */
    if (log_ptr->devices_ptr == NULL )
    {
        log_ptr->devices_ptr = device_ptr;
    }
    else
    {
        /* Find the end of the list */
        this_device_ptr = log_ptr->devices_ptr;
        while(this_device_ptr->next_device_ptr != NULL )
        {
            this_device_ptr = this_device_ptr->next_device_ptr;
        }
        /* Link to the end of the list */
        this_device_ptr->next_device_ptr = device_ptr;
    }

    /* look for all defined sections */

    /* take the top of the sections chain */
    this_section_ptr = log_ptr->sections_ptr;
    while(this_section_ptr != NULL )
    {
        if (f_logger_define_device(xi_device, xi_type, this_section_ptr->default_state, &returnedptr) != LOGGER_OK)
        {
            return LOGGER_GEN_ERROR;
        }
        device_state_ptr = (LOGGER_DEVICE_STATE *)returnedptr;
        if (this_section_ptr->devices_ptr == NULL )
        {
            this_section_ptr->devices_ptr = device_state_ptr;
        }
        else
        {
            p_chain_a_device(this_section_ptr->devices_ptr, device_state_ptr);
        }
        /* take the top of the groups chain in this section */
        this_group_ptr = this_section_ptr->groups_ptr;
        while(this_group_ptr != NULL )
        {
            if (f_logger_define_device(xi_device, xi_type, this_group_ptr->default_state, & returnedptr) != LOGGER_OK)
            {
                return LOGGER_GEN_ERROR;
            }
            device_state_ptr = (LOGGER_DEVICE_STATE *) returnedptr;
            if (this_group_ptr->devices_ptr == NULL )
            {
                this_group_ptr->devices_ptr = device_state_ptr;
            }
            else
            {
                p_chain_a_device(this_group_ptr->devices_ptr, device_state_ptr);
            }
            /* take the top of the items chain in this section */
            this_item_ptr = this_group_ptr->items_ptr;
            while(this_item_ptr != NULL )
            {
                if (f_logger_define_device(xi_device, xi_type, this_item_ptr->default_state, & returnedptr) != LOGGER_OK)
                {
                    return LOGGER_GEN_ERROR;
                }
                device_state_ptr = (LOGGER_DEVICE_STATE *)returnedptr;
                /* set its trigger state to default state */
                this_item_ptr->item_trigger_state = LOGGER_TRIGGER_ACTION_NONE;
                this_item_ptr->trigger_count = 1;
                this_item_ptr->trigger_current_count = 0;
                this_item_ptr->throttle_counter = this_item_ptr->throttle_threshold;
                /* set its ram state to default state */
                this_item_ptr->item_ram_state = CE_FALSE;
                if (this_item_ptr->devices_ptr == NULL)
                {
                    this_item_ptr->devices_ptr = device_state_ptr;
                }
                else
                {
                    p_chain_a_device(this_item_ptr->devices_ptr, device_state_ptr);
                }
                /* take the next item on group */
                this_item_ptr = this_item_ptr->next_item_ptr;
            } /* while on defined items per group */
            /* take the next group on section */
            this_group_ptr = this_group_ptr->next_group_ptr;
        }/* while on defined groups per section */
        /* take the next section on logger */
        this_section_ptr = this_section_ptr->next_section_ptr;
    }  /* while on defined sections */
    
    /* Return the device ID */
	*xo_device_id = (LOGGER_DEVICE_ID)device_ptr;

    /* Create logger queue */
    if (logger_create_queue())
    {
        printk(KERN_ERR "logger queue create error \n");
        return LOGGER_GEN_ERROR;
    }

    return LOGGER_OK;
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_time_base.                                               */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Configuration.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets the time base the logger will use.                   */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_log_id - Log ID                                                      */  
/*  xi_timestamping_callback - Pointer to timestamping callback             */
/*  xi_timestamp_formatting_callback - Pointer to timestamp formatting      */
/*                                     callback                             */
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_time_base(LOGGER_LOG_ID xi_log_id, 
    LOGGER_TIMESTAMPING_CALLBACK xi_timestamping_callback, 
    LOGGER_TIMESTAMP_FORMATTING_CALLBACK xi_timestamp_formatting_callback) 
{
    LOGGER_LOG *log_ptr;

    /* Get the log */
    log_ptr = (LOGGER_LOG *) xi_log_id;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Set the time base */
    log_ptr->time_base = LOGGER_TIME_BASE_USER_DEFINED;
    log_ptr->timestamping_callback = xi_timestamping_callback;
    log_ptr->timestamp_formatting_callback = xi_timestamp_formatting_callback;

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_day_base.                                                */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Configuration.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets the day base the logger will use.                    */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_log_id - Log ID                                                      */  
/*  xi_timeofday_callback - Pointer to time of day callback                 */
/*  xi_timeofday_formatting_callback - Pointer to time of day formatting    */
/*                                     callback                             */
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_day_base(LOGGER_LOG_ID xi_log_id, 
    LOGGER_TIMEOFDAY_CALLBACK xi_timeofday_callback, 
    LOGGER_TIMEOFDAY_FORMATTING_CALLBACK xi_timeofday_formatting_callback) 
{

    LOGGER_LOG *log_ptr;

    /* Get the log */
    log_ptr = (LOGGER_LOG *)xi_log_id;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC)
    {
        return LOGGER_GEN_ERROR;
    }

    /* Set the time base */
    log_ptr->time_base = LOGGER_TIME_BASE_USER_DEFINED;
    log_ptr->timeofday_callback = xi_timeofday_callback;
    log_ptr->timeofday_formatting_callback = xi_timeofday_formatting_callback;

    return LOGGER_OK;
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_format.                                                  */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Configuration.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets the information format the logger will use.          */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_log_id - Log ID                                                      */  
/*  xi_log_information_formatting_callback - Log information formatting     */
/*                                           callback                       */
/*  xi_log_item_formatting_callback - Log item formatting callback          */
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_format(LOGGER_LOG_ID xi_log_id, 
    LOGGER_LOG_INFORMATION_FORMATTING_CALLBACK xi_log_information_formatting_callback, 
    LOGGER_LOG_ITEM_FORMATTING_CALLBACK xi_log_item_formatting_callback)
{
    LOGGER_LOG *log_ptr;

    /* Get the log */
    log_ptr = (LOGGER_LOG *)xi_log_id;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC)
    {
        return LOGGER_GEN_ERROR;
    }

    /* Set the log information formatting callback */
    if (xi_log_information_formatting_callback != NULL)
    {
        log_ptr->log_information_formatting_callback = xi_log_information_formatting_callback;
    }

    /* Set the log item formatting callback */
    if (xi_log_item_formatting_callback != NULL)
    {
        log_ptr->log_item_formatting_callback = xi_log_item_formatting_callback;
    }

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_notification.                                            */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Configuration.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets the notification callback the logger will use.       */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_log_id - Log ID                                                      */  
/*  xi_notification_callback - Notification callback                        */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_notification(LOGGER_LOG_ID xi_log_id, 
    LOGGER_NOTIFICATION_CALLBACK xi_notification_callback)
{
    LOGGER_LOG *log_ptr;

    /* Get the log */
    log_ptr = (LOGGER_LOG *)xi_log_id;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC)
    {
        return LOGGER_GEN_ERROR;
    }

    /* Set the notification callback */
    log_ptr->notification_callback = xi_notification_callback;

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_section.                                                 */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Log data base.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets a section in the data base.                          */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_log_id - Log ID                                                      */  
/*  xi_name - Name of the section                                           */  
/*  xi_description - The section description                                */  
/*  xi_state - Section state:                                               */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  xo_section_id - The output section ID                                   */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_section(LOGGER_LOG_ID xi_log_id, char *xi_name, 
    char *xi_description, long xi_state, LOGGER_SECTION_ID *xo_section_id)
{
    LOGGER_LOG *log_ptr;
    LOGGER_SECTION *section_ptr;
    LOGGER_SECTION *this_section_ptr;

    /* Get the log */
    log_ptr = (LOGGER_LOG *)xi_log_id;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC)
    {
        return LOGGER_GEN_ERROR;
    }

    /* Allocate area for the control structure */
    section_ptr = (LOGGER_SECTION *) logger_alloc(ALLOCATED_SECTION_LENGTH);
    if (section_ptr == NULL )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Set the section control structure */
    section_ptr->magic = LOGGER_SECTION_MAGIC;
    strcpy(section_ptr->name, xi_name);
    strcpy(section_ptr->description, xi_description);
    section_ptr->default_state = xi_state;
    section_ptr->devices_ptr = NULL;
    section_ptr->groups_ptr = NULL;
    section_ptr->next_section_ptr = NULL;
    /* Link to the parent log */
    section_ptr->parent_log_ptr = log_ptr;

    /* Link to the end of the sections link list */
    if (log_ptr->sections_ptr == NULL )
    {
        log_ptr->sections_ptr = section_ptr;
    }
    else
    {
        /* Find the end of the list */
        this_section_ptr = log_ptr->sections_ptr;
        while(this_section_ptr->next_section_ptr != NULL )
        {
            this_section_ptr = this_section_ptr->next_section_ptr;
        }

        /* Link to the end of the list */
        this_section_ptr->next_section_ptr = section_ptr;
    }
	
    /* Return the section ID */
    *xo_section_id = (LOGGER_SECTION_ID)section_ptr;

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_group.                                                   */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Log data base.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets a group in the data base.                            */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_section_id - Section ID                                              */  
/*  xi_name - Name of the section                                           */  
/*  xi_description - The section description                                */  
/*  xi_state - Section state:                                               */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  xo_group_id - The output group ID                                       */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_group(LOGGER_SECTION_ID xi_section_id, char *xi_name, 
    char *xi_description, long xi_state, LOGGER_GROUP_ID *xo_group_id)
{
    LOGGER_LOG *log_ptr;
    LOGGER_SECTION *section_ptr;
    LOGGER_GROUP *group_ptr;
    LOGGER_GROUP *this_group_ptr;

    /* Get the section */
    section_ptr = (LOGGER_SECTION *) xi_section_id;

    /* Is it a valid section? */
    if (section_ptr->magic != LOGGER_SECTION_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Get the log */
    log_ptr = section_ptr->parent_log_ptr;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Allocate area for the control structure */
    group_ptr = (LOGGER_GROUP *) logger_alloc(ALLOCATED_GROUP_LENGTH);
    if (group_ptr == NULL )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Set the group control structure */
    group_ptr->magic = LOGGER_GROUP_MAGIC;
    strcpy(group_ptr->name, xi_name);
    strcpy(group_ptr->description, xi_description);
    group_ptr->default_state = xi_state;
    group_ptr->devices_ptr = NULL;
    group_ptr->items_ptr = NULL;
    group_ptr->next_group_ptr = NULL;

    /* Link to the parent log and section */
    group_ptr->parent_log_ptr = log_ptr;
    group_ptr->parent_section_ptr = section_ptr;

    /* Link to the end of the groups link list */
    if (section_ptr->groups_ptr == NULL )
    {
        section_ptr->groups_ptr = group_ptr;
    }
    else
    {
        /* Find the end of the list */
        this_group_ptr = section_ptr->groups_ptr;
        while(this_group_ptr->next_group_ptr != NULL )
        {
            this_group_ptr = this_group_ptr->next_group_ptr;
        }

        /* Link to the end of the list */
        this_group_ptr->next_group_ptr = group_ptr;
    }

    /* Return the group ID */
    *xo_group_id = (LOGGER_GROUP_ID)group_ptr;

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_item.                                                    */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Log data base.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets an item in the data base.                            */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_group_id - Group ID                                                  */  
/*  xi_name - Name of the section                                           */  
/*  xi_description - The section description                                */  
/*  xi_state - Section state:                                               */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*  xi_attributes - Attributes:                                             */  
/*    LOGGER_ATTRIBUTE_NORMAL                                               */  
/*    LOGGER_ATTRIBUTE_BOLD                                                 */  
/*    LOGGER_ATTRIBUTE_UNDERLINE                                            */  
/*    LOGGER_ATTRIBUTE_BLINK                                                */  
/*    LOGGER_ATTRIBUTE_REVERSE_VIDEO                                        */  
/*    LOGGER_ATTRIBUTE_BEEP                                                 */  
/*    LOGGER_ATTRIBUTE_DOUBLE_HEIGHT                                        */  
/*    LOGGER_ATTRIBUTE_DOUBLE_WIDTH                                         */  
/*  xi_type - Item type:                                                    */
/*    LOGGER_ITEM_TYPE_NORMAL                                               */
/*    LOGGER_ITEM_TYPE_FORMAT                                               */
/*  xi_size - Size of binary item                                           */  
/*  xi_parameter - Parameter to send to the binary item formatting callback */  
/*  xi_format - Text item format string                                     */  
/*  xi_binary_item_formatting_callback - Binary item formatting callback    */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  xo_item_id - The output item ID                                         */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_item(LOGGER_GROUP_ID xi_group_id, char *xi_name, 
    char *xi_description, long xi_state, long xi_ram_state, long xi_synchronous, 
    LOGGER_ATTRIBUTES xi_attributes, LOGGER_ITEM_TYPE xi_type, 
    LOGGER_TRIGGER_ACTION xi_action, unsigned long xi_size, 
    unsigned long xi_parameter, char *xi_format, 
    LOGGER_ITEM_ID *xo_item_id) 
{
    LOGGER_LOG *log_ptr;
    LOGGER_SECTION *section_ptr;
    LOGGER_GROUP *group_ptr;
    LOGGER_ITEM *item_ptr;
	LOGGER_ITEM *this_item_ptr;

    /* Get the group */
    group_ptr = (LOGGER_GROUP *) xi_group_id;

    /* Is it a valid group? */
    if (group_ptr->magic != LOGGER_GROUP_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Get the section */
    section_ptr = group_ptr->parent_section_ptr;

    /* Is it a valid section? */
    if (section_ptr->magic != LOGGER_SECTION_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Get the log */
    log_ptr = group_ptr->parent_log_ptr;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC)
    {
        return LOGGER_GEN_ERROR;
    }

    /* Allocate area for the control structure */
    item_ptr = (LOGGER_ITEM *) logger_alloc(ALLOCATED_ITEM_LENGTH);
    if (item_ptr == NULL )
    {
        return LOGGER_GEN_ERROR;
    }
    /* Set the item control structure */
    item_ptr->magic = LOGGER_ITEM_MAGIC;
    strcpy(item_ptr->name, xi_name);
    strcpy(item_ptr->description, xi_description);
    item_ptr->default_state = xi_state;
    item_ptr->throttle_threshold = 0;
    item_ptr->attributes = xi_attributes;
    item_ptr->type = xi_type;
    item_ptr->size = xi_size;
    item_ptr->parameter = xi_parameter;
    strcpy(item_ptr->format, xi_format);
    item_ptr->devices_ptr = NULL;
    item_ptr->next_item_ptr = NULL;

    /* Link to the parent log */
    item_ptr->parent_log_ptr = log_ptr;

    /* Link to the parent section */
    item_ptr->parent_section_ptr = section_ptr;

    /* Link to the parent group */
    item_ptr->parent_group_ptr = group_ptr;

    /* Link to the end of the items link list */
    if (group_ptr->items_ptr == NULL )
    {
        group_ptr->items_ptr = item_ptr;
    }
    else
    {
        /* Find the end of the list */
        this_item_ptr = group_ptr->items_ptr;
        while(this_item_ptr->next_item_ptr != NULL )
        {
            this_item_ptr = this_item_ptr->next_item_ptr;
        }

        /* Link to the end of the list */
        this_item_ptr->next_item_ptr = item_ptr;
    }

    /* Return the item ID */
    *xo_item_id = (LOGGER_ITEM_ID) item_ptr;
    
    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_section_device.                                          */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Configuration.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets a device for section the logger will print to.       */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_section_id - The section ID                                          */  
/*  xi_name - Name of the device                                            */  
/*  xi_description - Description of the device                              */  
/*  xi_device - The device FD                                               */  
/*  xi_state - Device state:                                                */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*  xi_throttle_depth - Depth of throttling                                 */  
/*  xi_throttle_threshold - Threshold of throttling                         */  
/*  xi_flush - Automatic flush after write                                  */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*  xi_close - Automatic close while termination of log                     */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                                */
/*     LOGGER_OK  - Success indication.                                  */
/*      LOGGER_GEN_ERROR - Error indication.                                  */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_section_device(LOGGER_SECTION_ID xi_section_id, char *xi_name, char *xi_description, long xi_device, long xi_state, unsigned long xi_throttle_depth, unsigned long xi_throttle_threshold, long xi_flush, long xi_close)
{
    LOGGER_LOG *log_ptr;
    LOGGER_SECTION *section_ptr;

    /* Get the section */
    section_ptr = (LOGGER_SECTION *) xi_section_id;

    /* Is it a valid section? */
    if (section_ptr->magic != LOGGER_SECTION_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Get the log */
    log_ptr = section_ptr->parent_log_ptr;

    /* Is it a valid log? */
	if (log_ptr->magic != LOGGER_LOG_MAGIC)
	{
        return LOGGER_GEN_ERROR;
    }
	
    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_group_device.                                            */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Configuration.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets a device for group the logger will print to.         */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_group_id - The group ID                                              */  
/*  xi_name - Name of the device                                            */  
/*  xi_description - Description of the device                              */  
/*  xi_device - The device FD                                               */  
/*  xi_state - Device state:                                                */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*  xi_throttle_depth - Depth of throttling                                 */  
/*  xi_throttle_threshold - Threshold of throttling                         */  
/*  xi_flush - Automatic flush after write                                  */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*  xi_close - Automatic close while termination of log                     */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                                */
/*     LOGGER_OK  - Success indication.                                  */
/*      LOGGER_GEN_ERROR - Error indication.                                  */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_group_device(LOGGER_GROUP_ID xi_group_id, char *xi_name, char *xi_description, long xi_device, long xi_state, unsigned long xi_throttle_depth, unsigned long xi_throttle_threshold, long xi_flush, long xi_close)
{
    LOGGER_LOG *log_ptr;
    LOGGER_SECTION *section_ptr;
    LOGGER_GROUP *group_ptr;

    /* Get the group */
    group_ptr = (LOGGER_GROUP *) xi_group_id;

    /* Is it a valid group? */
    if (group_ptr->magic != LOGGER_GROUP_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Get the section */
    section_ptr = group_ptr->parent_section_ptr;

    /* Is it a valid section? */
    if (section_ptr->magic != LOGGER_SECTION_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Get the log */
    log_ptr = group_ptr->parent_log_ptr;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_item_device.                                             */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Configuration.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets a device for item the logger will print to.          */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_item_id - The item ID                                                */  
/*  xi_name - Name of the device                                            */  
/*  xi_description - Description of the device                              */  
/*  xi_device - The device FD                                               */  
/*  xi_state - Device state:                                                */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*  xi_throttle_depth - Depth of throttling                                 */  
/*  xi_throttle_threshold - Threshold of throttling                         */  
/*  xi_flush - Automatic flush after write                                  */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*  xi_close - Automatic close while termination of log                     */  
/*    CE_TRUE                                                               */  
/*    CE_FALSE                                                              */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                                */
/*     LOGGER_OK  - Success indication.                                  */
/*      LOGGER_GEN_ERROR - Error indication.                                  */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_item_device(LOGGER_ITEM_ID xi_item_id, char *xi_name, char *xi_description, long xi_device, long xi_state, unsigned long xi_throttle_depth, unsigned long xi_throttle_threshold, long xi_flush, long xi_close)
{
    LOGGER_LOG *log_ptr;
    LOGGER_SECTION *section_ptr;
    LOGGER_GROUP *group_ptr;
    LOGGER_ITEM *item_ptr;

    /* Get the item */
    item_ptr = (LOGGER_ITEM *) xi_item_id;

    /* Is it a valid item? */
    if (item_ptr->magic != LOGGER_ITEM_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Get the group */
    group_ptr = item_ptr->parent_group_ptr;

    /* Is it a valid group? */
    if (group_ptr->magic != LOGGER_GROUP_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Get the section */
    section_ptr = group_ptr->parent_section_ptr;

    /* Is it a valid section? */
    if (section_ptr->magic != LOGGER_SECTION_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Get the log */
    log_ptr = group_ptr->parent_log_ptr;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_get_item_state.                                              */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Get state.                                                    */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function returns the current state of the item.                    */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_item_id - The item ID                                                */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   long - Item state.                                          */
/*     CE_TRUE  - Item os on.                                               */
/*     CE_FALSE - Item is off.                                              */
/*                                                                          */
/****************************************************************************/
long logger_get_item_state(LOGGER_ITEM_ID xi_item_id)
{
    LOGGER_ITEM *item_ptr;

    /* Get the item */
    item_ptr = (LOGGER_ITEM *) xi_item_id;

    /* Is it a valid item? */
    if (item_ptr->magic != LOGGER_ITEM_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    /* Return the state */
    return(item_ptr->default_state);
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_log_to_file                                                  */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Activate the logger to an external file                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Activate the logger(as "lh" command ) but the output is into         */
/*   an external file                                                       */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_fd_log_file - file descriptor of the file                            */  
/*  xi_file_name - file name                                                */  
/*  xi_object - the current task context                                    */
/*                                                                          */  
/* Output:                                                                  */
/*                                                                          */
/*  LOGGER_EXT_FILE_STATUS -                                            */
/*  CE_LOG_NULL                                                             */
/*  CE_FILE_EXIST                                                           */
/*  CE_FILE_OPEN                                                            */
/*                                                                          */
/****************************************************************************/
LOGGER_EXT_FILE_STATUS logger_log_to_file(unsigned long xi_fd_log_file, unsigned long xi_log_ptr, char *xi_object, char *xi_file_name)
{
    LOGGER_LOG *log_ptr = (LOGGER_LOG *) xi_log_ptr;
    unsigned long device_id;

    if (log_ptr != 0 )
    {
        if (log_ptr->fd_log_file == -1 )
        {
            log_ptr->fd_log_file = xi_fd_log_file;
            if (xi_file_name != NULL )
            {
                strcpy(log_ptr->log_file_name, xi_file_name);
            }
            logger_set_device(( LOGGER_LOG_ID) log_ptr, xi_object, "log file", log_ptr->fd_log_file, 0, CE_TRUE, & device_id);
            return(CE_FILE_OPEN);
        }
        else
        {
            close(xi_fd_log_file);
            return(CE_FILE_EXIST);
        }
    }
    
    return(CE_LOG_NULL);
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_close_log_file                                               */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   close and save logger external file                                    */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Close external                                                         */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_deviceid - external file descriptor                                  */
/*                                                                          */
/*  xo_filename - saved file name                                           */
/*                                                                          */  
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                                */
/*     LOGGER_OK  - Success indication.                                  */
/*      LOGGER_GEN_ERROR - Error indication - logger doesn't exsits or        */
/*                       no external file                                   */                   
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_close_log_file(unsigned long xi_deviceid, unsigned long xi_log_ptr, char *xo_filename)
{
    LOGGER_LOG *log_ptr = (LOGGER_LOG *) xi_log_ptr;
    LOGGER_DEVICE *this_device_ptr;

    if (log_ptr != 0 )
    {
        logger_off(xi_deviceid, xi_log_ptr, NULL, CE_CHANGE_BOTH);
        if (log_ptr->fd_log_file != -1 )
        {
            close(log_ptr->fd_log_file);
            strcpy(xo_filename, log_ptr->log_file_name);
            /* release the devices */
            if (log_ptr->devices_ptr != NULL )
            {
                this_device_ptr = log_ptr->devices_ptr;
                while(this_device_ptr != NULL )
                {
                    if (this_device_ptr->device == (unsigned long )(log_ptr->fd_log_file) )
                    {
                        logger_clear_device(this_device_ptr ->device);
                        break;
                    }
                    this_device_ptr = this_device_ptr->next_device_ptr;
                }
            }
            log_ptr->fd_log_file = -1;
        }
        else
        {
            xo_filename[0] = '\0';
            return LOGGER_GEN_ERROR;
        }
    }
    else
    {
        xo_filename[0] = '\0';
        return LOGGER_GEN_ERROR;
    }

    return LOGGER_OK;
}


/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_get_object_list                                                 */
/*                                                                          */
/* Title:                                                                   */
/*                                                                          */
/*   Get a text list of all the objects                                     */
/*                                                                          */
/* Abstract:                                                                */
/*                                                                          */
/*   Build a list of all the descendens objects of a parent object          */
/*                                                                          */
/* Input:                                                                   */
/*                                                                          */
/*  xi_object - the parent object                                           */
/*  xi_logger_ptr - current logger instance                                 */
/*  object_list - text list of all the objects                              */
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   long - Error code.                                                     */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*     LOGGER_OBECT_NOT_FOUND - Object is not in the data structure         */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_get_object_list(unsigned long xi_log_ptr, char *xi_object, char *object_list)
{

    LOGGER_SECTION *section_ptr;
    LOGGER_GROUP *group_ptr;
    LOGGER_ITEM *item_ptr;
    LOGGER_LOG *log_ptr = (LOGGER_LOG *) xi_log_ptr;

    memset (temp1, 0, sizeof(temp1));

    while (log_ptr != 0)
    {
        section_ptr = log_ptr->sections_ptr;

        if (f_match_name(xi_object, 1, log_ptr->name) == CE_TRUE)
        {
            sprintf(temp1, "%s-->\n\n", log_ptr->name);

            while (section_ptr != NULL)
            {
                char temp2[LOGGER_OBJECT_MAX_SIZE];
                sprintf(temp2, "%30s %5ld\n", section_ptr->name, section_ptr->devices_ptr->object_state);
                strncat(temp1, temp2, sizeof(temp1)-strlen(temp1));

                section_ptr = section_ptr->next_section_ptr;
            }
            strcat (object_list, temp1);

            return (LOGGER_OK);
        }

        /* Traverse the sections link list */
        while (section_ptr != NULL)
        {
            group_ptr = section_ptr->groups_ptr;
            /* Match? */
            if (f_match_name(xi_object, 2, log_ptr->name, section_ptr->name) == CE_TRUE)
            {
                memset (temp1, 0, sizeof(temp1));
                sprintf(temp1, "%s-->%s(%ld)-->\n\n", log_ptr->name, section_ptr->name, section_ptr->devices_ptr->object_state);

                while (group_ptr != NULL)
                {
                    char temp2[LOGGER_OBJECT_MAX_SIZE];
                    sprintf(temp2, "%30s %5ld\n", group_ptr->name, group_ptr->devices_ptr->object_state);
                    strncat(temp1, temp2, sizeof(temp1)-strlen(temp1));

                    group_ptr = group_ptr->next_group_ptr;
                }

                strcat (object_list, temp1);
                /* Quit */
                return (LOGGER_OK);
            }

            /* Traverse the groups link list */
            while (group_ptr != NULL)
            {
                item_ptr = group_ptr->items_ptr;
                /* Match? */
                if (f_match_name(xi_object, 3, log_ptr->name, section_ptr->name, group_ptr->name) == CE_TRUE)
                {
                    memset (temp1, 0, sizeof(temp1));
                    sprintf(temp1, "%s-->%s(%ld)-->%s(%ld)-->\n\n", log_ptr->name, section_ptr->name, section_ptr->devices_ptr->object_state,
                        group_ptr->name, group_ptr->devices_ptr->object_state);

                    while (item_ptr != NULL)
                    {
                        char temp2[LOGGER_OBJECT_MAX_SIZE];
                        sprintf(temp2, "%30s %5ld\n", item_ptr->name, item_ptr->devices_ptr->object_state);
                        strncat(temp1, temp2, sizeof(temp1)-strlen(temp1));

                        item_ptr = item_ptr->next_item_ptr;
                    }

                    strcat(object_list, temp1);
                    /* Quit */
                    return (LOGGER_OK);
                }

                while (item_ptr != NULL)
                {
                    if (f_match_name(xi_object, 4, log_ptr->name, section_ptr->name, group_ptr->name, item_ptr->name) == CE_TRUE)
                    {
                        memset (temp1, 0, sizeof(temp1));

                        sprintf(temp1, "%s-->%s(%ld)-->%s(%ld)-->\n\n%30s %5ld\n", log_ptr->name, section_ptr->name, section_ptr->devices_ptr->object_state,
                            group_ptr->name, group_ptr->devices_ptr->object_state, item_ptr->name, item_ptr->devices_ptr->object_state);

                        strcat(object_list, temp1);

                        return (LOGGER_OK);
                    }

                    item_ptr = item_ptr->next_item_ptr;
                }

                group_ptr = group_ptr->next_group_ptr;

            }

            section_ptr = section_ptr->next_section_ptr;
        }

        log_ptr = log_ptr->next_log_ptr;
    }

    return (LOGGER_OBJECT_NOT_FOUND);
}




/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_on                                                           */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Enable logging                                                         */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set the device to be active for all already set items                  */  
/*   Set an item to be active                                               */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_device_id - current file descriptor                                  */  
/*  xi_object - logger object (section/group/item) to be turned on          */  
/*              NULL means entire device.                                   */
/*  xi_in_ram - which field goes 'on' : state, ram or both                  */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                                */
/*     LOGGER_OK  - Success indication.                                  */
/*      LOGGER_GEN_ERROR - Error indication.                                  */
/*     LOGGER_OBECT_NOT_FOUND - Object is not in the data structure      */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_on(unsigned long xi_device_id, unsigned long xi_log_ptr, char *xi_object, LOGGER_CHANGE_REQUEST xi_in_ram)
{
    LOGGER_DEVICE *device_ptr;
    LOGGER_SECTION *section_ptr;
    LOGGER_GROUP *group_ptr;
    LOGGER_ITEM *item_ptr;
    LOGGER_LOG *log_ptr = (LOGGER_LOG *) xi_log_ptr;

    /* Device should be enabled */
    if (xi_object == NULL )
    {
        while(log_ptr != 0 )
        {
            /* Traverse the devices link list */
            device_ptr = log_ptr->devices_ptr;
            while(device_ptr != NULL )
            {
                /* Match? */
                if (xi_device_id == device_ptr->device )
                {
                    /* Turn on */
                    switch(xi_in_ram )
                    {
                    case CE_CHANGE_STATE :
                        device_ptr->state = CE_TRUE;
                        break;

                    case CE_CHANGE_RAM :
                        device_ptr->device_ram_state = CE_TRUE;
                        break;

                    case CE_CHANGE_BOTH :
                        device_ptr->state = CE_TRUE;
                        device_ptr->device_ram_state = CE_TRUE;
                        break;
                    }
    
                    /* Quit */
                    return LOGGER_OK;
                }
    
                /* Get the next device */
                device_ptr = device_ptr->next_device_ptr;
            }

            /* Get the next log */
            log_ptr = log_ptr->next_log_ptr;
        }

        return(LOGGER_OBJECT_NOT_FOUND);
    }
    
    while(log_ptr != 0 )
    {
        /* Traverse the devices link list */
        device_ptr = log_ptr->devices_ptr;
        while(device_ptr != NULL )
        {
            /* Match? */
            if (f_match_name(xi_object, 2, log_ptr->name, device_ptr->name ) == CE_TRUE )
            {
                /* Turn on */
                switch(xi_in_ram )
                {
                case CE_CHANGE_STATE :
                    device_ptr->state = CE_TRUE;
                    break;

                case CE_CHANGE_RAM :
                    device_ptr->device_ram_state = CE_TRUE;
                    break;

                case CE_CHANGE_BOTH :
                    device_ptr->state = CE_TRUE;
                    device_ptr->device_ram_state = CE_TRUE;
                    break;
                }

                /* Quit */
                return LOGGER_OK;
            }

            /* Get the next device */
            device_ptr = device_ptr->next_device_ptr;
        }

        /* Traverse the sections link list */
        section_ptr = log_ptr->sections_ptr;
        while(section_ptr != NULL )
        {
            /* Match? */
            if (f_match_name(xi_object, 2, log_ptr->name, section_ptr->name ) == CE_TRUE )
            {
                /* Turn on */ 
                switch(xi_in_ram )
                {
                case CE_CHANGE_STATE :
                    p_set_section_state(xi_device_id, section_ptr, CE_TRUE);
                    break;

                case CE_CHANGE_RAM :
                    p_set_section_ram_state(xi_device_id, section_ptr, CE_TRUE);
                    break;

                case CE_CHANGE_BOTH :
                    p_set_section_state(xi_device_id, section_ptr, CE_TRUE);
                    p_set_section_ram_state(xi_device_id, section_ptr, CE_TRUE);
                    break;
                }

                /* Quit */
                return LOGGER_OK;
            }

            /* Traverse the groups link list */
            group_ptr = section_ptr->groups_ptr;
            while(group_ptr != NULL )
            {
                /* Match? */
                if (f_match_name(xi_object, 3, log_ptr->name, section_ptr->name, group_ptr->name ) == CE_TRUE )
                {
                    /* Turn on */
                    switch(xi_in_ram )
                    {
                    case CE_CHANGE_STATE :
                        p_set_section_state(xi_device_id, section_ptr, CE_TRUE);
                        p_set_group_state(xi_device_id, section_ptr, group_ptr, CE_TRUE);
                        break;

                    case CE_CHANGE_RAM :
                        p_set_section_state(xi_device_id, section_ptr, CE_TRUE);
                        p_set_group_ram_state(xi_device_id, section_ptr, group_ptr, CE_TRUE);
                        break;

                    case CE_CHANGE_BOTH :
                        p_set_section_state(xi_device_id, section_ptr, CE_TRUE);
                        p_set_group_state(xi_device_id, section_ptr, group_ptr, CE_TRUE);
                        p_set_group_ram_state(xi_device_id, section_ptr, group_ptr, CE_TRUE);
                        break;
                    }

                    /* Quit */
                    return LOGGER_OK;
                }
                /* Traverse the items link list */
                item_ptr = group_ptr->items_ptr;
                while(item_ptr != NULL )
                {
                    /* Match? */
                    if (f_match_name(xi_object, 4, log_ptr->name, section_ptr->name, group_ptr->name, item_ptr->name ) == CE_TRUE )
                    {
                        /* Turn on */
                        switch(xi_in_ram )
                        {
                        case CE_CHANGE_STATE :
                            p_set_section_state(xi_device_id, section_ptr, CE_TRUE);
                            p_set_group_state(xi_device_id, section_ptr, group_ptr, CE_TRUE);
                            p_set_item_state(xi_device_id, item_ptr, CE_TRUE);
                            break;

                        case CE_CHANGE_RAM :
                            p_set_section_state(xi_device_id, section_ptr, CE_TRUE);
                            p_set_group_state(xi_device_id, section_ptr, group_ptr, CE_TRUE);
                            p_set_item_ram_state(item_ptr, CE_TRUE);
                            break;

                        case CE_CHANGE_BOTH :
                            p_set_section_state(xi_device_id, section_ptr, CE_TRUE);
                            p_set_group_state(xi_device_id, section_ptr, group_ptr, CE_TRUE);
                            p_set_item_state(xi_device_id, item_ptr, CE_TRUE);
                            p_set_item_ram_state(item_ptr, CE_TRUE);
                            break;
                        }

                        /* Quit */
                        return LOGGER_OK;
                    }

                    /* Get the next item */
                    item_ptr = item_ptr->next_item_ptr;
                }
                group_ptr = group_ptr->next_group_ptr;
            }
            section_ptr = section_ptr->next_section_ptr;
        }

        /* Get the next log */
        log_ptr = log_ptr->next_log_ptr;
    }

    return(LOGGER_OBJECT_NOT_FOUND);
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_off                                                          */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Set the device or an item                                              */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set the device to be inactive for all already set items                */  
/*   Set an item to be inactive                                             */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_deviceid - current file descriptor                                   */  
/*  xi_object - logger object (section/group/item) to be turned off.        */  
/*              NULL means entire device.                                   */
/*  xi_in_ram - which field goes 'off' : state, ram or both                 */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                                */
/*     LOGGER_OK  - Success indication.                                  */
/*      LOGGER_GEN_ERROR - Error indication.                                  */
/*     LOGGER_OBECT_NOT_FOUND - Object is not in the data structure      */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_off(unsigned long xi_device_id, unsigned long xi_log_ptr, char *xi_object, LOGGER_CHANGE_REQUEST xi_in_ram)
{
    LOGGER_DEVICE *device_ptr;
    LOGGER_SECTION *section_ptr;
    LOGGER_GROUP *group_ptr;
    LOGGER_ITEM *item_ptr;
    LOGGER_LOG *log_ptr = (LOGGER_LOG *) xi_log_ptr;

    /* Device should be disabled */
    if (xi_object == NULL )
    {
        while(log_ptr != 0 )
        {
            /* Traverse the devices link list */
            device_ptr = log_ptr->devices_ptr;
            while(device_ptr != NULL )
            {
                /* Match? */
                if (xi_device_id == device_ptr->device )
                {
                    /* Turn off */
                    switch(xi_in_ram )
                    {
                    case CE_CHANGE_STATE :
                        device_ptr->state = CE_FALSE;
                        break;

                    case CE_CHANGE_RAM :
                        device_ptr->device_ram_state = CE_FALSE;
                        break;

                    case CE_CHANGE_BOTH :
                        device_ptr->state = CE_FALSE;
                        device_ptr->device_ram_state = CE_FALSE;
                        break;
                    }
    
                    /* Quit */
                    return LOGGER_OK;
                }
    
                /* Get the next device */
                device_ptr = device_ptr->next_device_ptr;
            }

            /* Get the next log */
            log_ptr = log_ptr->next_log_ptr;
        }

        return(LOGGER_OBJECT_NOT_FOUND);
    }
    
    /* Traverse the logs link list */
    while(log_ptr != 0 )
    {
        /* Traverse the devices link list */
        device_ptr = log_ptr->devices_ptr;
        while(device_ptr != NULL )
        {
            /* Match? */
            if (f_match_name(xi_object, 2, log_ptr->name, device_ptr->name ) == CE_TRUE )
            {
                /* Turn off */
                switch(xi_in_ram )
                {
                case CE_CHANGE_STATE :
                    device_ptr->state = CE_FALSE;
                    break;

                case CE_CHANGE_RAM :
                    device_ptr->device_ram_state = CE_FALSE;
                    break;

                case CE_CHANGE_BOTH :
                    device_ptr->state = CE_FALSE;
                    device_ptr->device_ram_state = CE_FALSE;
                    break;
                }

                /* Quit */
                return LOGGER_OK;
            }

            /* Get the next device */
            device_ptr = device_ptr->next_device_ptr;
        }

        /* Traverse the sections link list */
        section_ptr = log_ptr->sections_ptr;
        while(section_ptr != NULL )
        {
            /* Match? */
            if (f_match_name(xi_object, 2, log_ptr->name, section_ptr->name ) == CE_TRUE )
            {
                /* Turn off */
                switch(xi_in_ram )
                {
                case CE_CHANGE_STATE :
                    p_set_section_state(xi_device_id, section_ptr, CE_FALSE);
                    break;

                case CE_CHANGE_RAM :
                    p_set_section_ram_state(xi_device_id, section_ptr, CE_FALSE);
                    break;

                case CE_CHANGE_BOTH :
                    p_set_section_state(xi_device_id, section_ptr, CE_FALSE);
                    p_set_section_ram_state(xi_device_id, section_ptr, CE_FALSE);
                    break;
                }
                
                /* Quit */
                return LOGGER_OK;
            }

            /* Traverse the groups link list */
            group_ptr = section_ptr->groups_ptr;
            while(group_ptr != NULL )
            {
                /* Match? */
                if (f_match_name(xi_object, 3, log_ptr->name, section_ptr->name, group_ptr->name ) == CE_TRUE )
                {
                    /* Turn off */
                    switch(xi_in_ram )
                    {
                    case CE_CHANGE_STATE :
                        p_set_group_state(xi_device_id, section_ptr, group_ptr, CE_FALSE);
                        break;

                    case CE_CHANGE_RAM :
                        p_set_group_ram_state(xi_device_id, section_ptr, group_ptr, CE_FALSE);
                        break;

                    case CE_CHANGE_BOTH :
                        p_set_group_state(xi_device_id, section_ptr, group_ptr, CE_FALSE);
                        p_set_group_ram_state(xi_device_id, section_ptr, group_ptr, CE_FALSE);
                        break;
                    }

                    /* Quit */
                    return LOGGER_OK;
                }

                /* Traverse the items link list */
                item_ptr = group_ptr->items_ptr;
                while(item_ptr != NULL )
                {
                    /* Match? */
                    if (f_match_name(xi_object, 4, log_ptr->name, section_ptr->name, group_ptr->name, item_ptr->name ) == CE_TRUE )
                    {
                        /* Turn off */
                        switch(xi_in_ram )
                        {
                        case CE_CHANGE_STATE :
                            p_set_item_state(xi_device_id, item_ptr, CE_FALSE);
                            break;

                        case CE_CHANGE_RAM :
                            p_set_item_ram_state(item_ptr, CE_FALSE);
                            break;

                        case CE_CHANGE_BOTH :
                            p_set_item_state(xi_device_id, item_ptr, CE_FALSE);
                            p_set_item_ram_state(item_ptr, CE_FALSE);
                            break;
                        }

                        /* Quit */
                        return LOGGER_OK;
                    }

                    /* Get the next item */
                    item_ptr = item_ptr->next_item_ptr;
                }

                /* Get the next group */
                group_ptr = group_ptr->next_group_ptr;
            }

            /* Get the next section */
            section_ptr = section_ptr->next_section_ptr;
        }

        /* Get the next log */
        log_ptr = log_ptr->next_log_ptr;
    }
    
    return(LOGGER_OBJECT_NOT_FOUND);
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_off_all                                                      */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Deactivate all the items                                               */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set all items to be inactive                                           */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_deviceid - current file descriptor                                   */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                                */
/*     LOGGER_OK  - Success indication.                                  */
/*      LOGGER_GEN_ERROR - Error indication.                                  */
/*     LOGGER_OBECT_NOT_FOUND - Object is not in the data structure      */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_off_all(unsigned long xi_device_id, unsigned long xi_log_ptr)
{
    LOGGER_SECTION *section_ptr;
    LOGGER_GROUP *group_ptr;
    LOGGER_ITEM *item_ptr;
    LOGGER_LOG *log_ptr = (LOGGER_LOG *) xi_log_ptr;

    while(log_ptr != 0 )
    {
        /* Get the sections */
        section_ptr = log_ptr->sections_ptr;
        while(section_ptr != NULL ) 
        {
            group_ptr = section_ptr->groups_ptr;
            while(group_ptr != NULL ) 
            {
                item_ptr = group_ptr->items_ptr;
                while(item_ptr != NULL ) 
                {
                    p_set_item_state(xi_device_id, item_ptr, CE_FALSE);
                    p_set_item_ram_state(item_ptr, CE_FALSE);
                    item_ptr = item_ptr->next_item_ptr;
                }
                p_set_group_state(xi_device_id, section_ptr, group_ptr, CE_FALSE);
                group_ptr = group_ptr->next_group_ptr;
            }
            p_set_section_state(xi_device_id, section_ptr, CE_FALSE);
            section_ptr = section_ptr->next_section_ptr;
        }
        log_ptr = log_ptr->next_log_ptr;
    }

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   pi_logger_log.                                                         */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - The logging function.                                         */
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function logs a message.                                           */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_file - File                                                          */  
/*  xi_line - Line                                                          */  
/*  xi_item_id - The item ID                                                */  
/*  ... - Log arguments                                                     */
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/****************************************************************************/
void pi_logger_log(char *xi_file, unsigned long xi_line, 
    const char *xi_function, LOGGER_ITEM_ID xi_item_id, ...)
{
    LOGGER_LOG *log_ptr;
    LOGGER_ITEM *item_ptr;
    LOGGER_LOG_MESSAGE *log_message_ptr;
    LOGGER_TIMESTAMPING_CALLBACK timestamping_callback;
    LOGGER_TIMEOFDAY_CALLBACK timeofday_callback;
    va_list arguments;
    char *file_slash = find_idx(xi_file, '/');
    char local_buffer[900];

    /* Get the item */
    item_ptr = (LOGGER_ITEM *)xi_item_id;

    /* Is it a valid item? */
    if (item_ptr == NULL)
    {
        return;
    }

    if (item_ptr->magic != LOGGER_ITEM_MAGIC)
    {
        return;
    }
	
    if (f_put_in_queue(item_ptr->parent_log_ptr, item_ptr->parent_section_ptr, 
       item_ptr->parent_group_ptr, item_ptr) == CE_FALSE)
    {
        return;
    }

    /* Get the arguments */
    va_start(arguments, xi_item_id);

    /* Get the log */
    log_ptr = item_ptr->parent_log_ptr;

    /* Is there a registered user-defined timestamping? */
    if ((log_ptr->time_base == LOGGER_TIME_BASE_USER_DEFINED) &&
        (log_ptr->timestamping_callback == NULL ) )
    {
        /* Set the default timestamping */
        timestamping_callback = f_default_none_timestamping;
    }
    else
    {
        /* Get it */
        timestamping_callback = log_ptr->timestamping_callback;
    }

    /* Is there a registered user-defined timestamping? */
    if ((log_ptr->time_base == LOGGER_TIME_BASE_USER_DEFINED) &&
        (log_ptr->timeofday_callback == NULL ) )
    {
        /* Set the default timestamping */
        timeofday_callback = p_default_none_timeofday;
    }
    else
    {
        /* Get it */
        timeofday_callback = log_ptr->timeofday_callback;
    }
    
    log_message_ptr = (LOGGER_LOG_MESSAGE *)local_buffer;

    /* Get the daytime */
    if (timeofday_callback != NULL)
        timeofday_callback(log_message_ptr->daytime);
    /* Get the timestamp */
    if (timestamping_callback != NULL)
        log_message_ptr->timestamp = timestamping_callback ();

    /* Get the context */
    strncpy(log_message_ptr->context, xi_function, LOGGER_NAME_LENGTH);

    /* Set the log */
    log_message_ptr->parent_log_ptr = log_ptr;

    /* Set the log item */
    log_message_ptr->item_ptr = item_ptr;

    strncpy(log_message_ptr->file, file_slash ? file_slash + 1 : xi_file,
        LOGGER_DESCRIPTION_LENGTH - 1);

    log_message_ptr->line = xi_line;
    va_copy(log_message_ptr->arguments, arguments);
    va_end(arguments);

    /* Send the log message to the task */
    p_logger(log_message_ptr);
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_clear_device                                                    */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Release the memory of a device                                         */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Release the memory of a device                                         */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_device_id  -  current device id                                      */  
/*                                                                          */
/* Output:                                                                  */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_clear_device(unsigned long xi_device_id)
{
    LOGGER_LOG *log_ptr;
    LOGGER_DEVICE *this_device_ptr;
    LOGGER_DEVICE *temp_ptr = NULL;
    LOGGER_DEVICE *parent = NULL;
    long found = CE_FALSE;
    unsigned long i;

    for (i = 0; i < gs_logs; i ++)
    {
        found = CE_FALSE;
        /* Get the log */
        log_ptr = (LOGGER_LOG *)gs_logger_array[i].log_ptr;
        
        if (log_ptr == NULL ) 
        {
            return LOGGER_GEN_ERROR;
        }
        /* Is it a valid log? */
        if (log_ptr->magic == LOGGER_LOG_MAGIC)
        {
            if (log_ptr->devices_ptr != NULL)
            {
                /* Find the end of the list */
                this_device_ptr = log_ptr->devices_ptr;
                while ((this_device_ptr != NULL) && (found == CE_FALSE))
                {
                    if (this_device_ptr->device == xi_device_id )
                    {
                        found = CE_TRUE;
                    }
                    else
                    {
                        parent = this_device_ptr;
                        this_device_ptr = this_device_ptr->next_device_ptr;
                    }
                }
                if (found == CE_TRUE )
                {
                    temp_ptr = this_device_ptr;
                    this_device_ptr = this_device_ptr->next_device_ptr;
                    if (parent == NULL )
                    {
                        log_ptr->devices_ptr = NULL;
                    }
                    else
                    {
                        parent->next_device_ptr = this_device_ptr;
                    }
                    if (temp_ptr->ram_buffer_ptr != NULL )
                    {
						logger_free(temp_ptr->ram_buffer_ptr); 
                    }
					logger_free(temp_ptr); 
                }
            }
            p_clear_sections(xi_device_id, log_ptr->sections_ptr);
        }
        else
        {
            /* not a valid logger */
            return LOGGER_GEN_ERROR;
        }
    }
    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_print_ram                                                       */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Print the logger from ram                                              */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Print the logger from ram                                              */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_device - file descriptor of the printing device                      */
/*  xi_flush  - clear the ram buffer after printing                         */
/*                                                                          */  
/* Output:                                                                  */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_print_ram(unsigned long xi_device, 
    unsigned long xi_device_type, unsigned long xi_log_ptr, long xi_flush)
{
    LOGGER_DEVICE *this_device_ptr;
    char *temp_ptr = NULL;
    char *log_item = NULL;
    LOGGER_DEVICE *parent = NULL;
    long found = CE_FALSE;

    /* Get the log */
    LOGGER_LOG *log_ptr = (LOGGER_LOG *) xi_log_ptr;
    
    /* Is it a valid log? */
    if (log_ptr->magic == LOGGER_LOG_MAGIC )
    {
        if (log_ptr->devices_ptr != NULL )
        {
            /* Find the end of the list */
            this_device_ptr = log_ptr->devices_ptr;
            while(( this_device_ptr != NULL ) &&(found == CE_FALSE ) )
            {
                if (this_device_ptr->device == xi_device )
                {
                    found = CE_TRUE;
                }
                else
                {
                    parent = this_device_ptr;
                    this_device_ptr = this_device_ptr->next_device_ptr;
                }
            }
            if (found == CE_TRUE )
            {
                if (this_device_ptr->has_ram_buffer == CE_TRUE )
                {
                    if (this_device_ptr->oldest_ram_buffer_ptr != NULL )
                    {
                        if (this_device_ptr->oldest_ram_buffer_ptr != this_device_ptr->ram_buffer_ptr )
                        {
                            temp_ptr = this_device_ptr->oldest_ram_buffer_ptr;
                            log_item = strchr(temp_ptr, 0);
                            while(( log_item != NULL ) &&(temp_ptr <(this_device_ptr->ram_buffer_ptr + this_device_ptr->ram_buffer_size ) ) )
                            {
                                if (strlen(temp_ptr ) > 0 )
                                {
                                    fdprintf(xi_device, "%s\n\r", temp_ptr);
                                }
                                temp_ptr = log_item + 1;
                                log_item = strchr(temp_ptr, 0);
                            }
                        }
                        temp_ptr = this_device_ptr->ram_buffer_ptr;
                        log_item = strchr(temp_ptr, 0);
                        while(( log_item != NULL ) &&(temp_ptr < this_device_ptr->ram_buffer_write_ptr ) )
                        {
                            fdprintf(xi_device, "%s\n\r", temp_ptr);
                            temp_ptr = log_item + 1;
                            log_item = strchr(temp_ptr, 0);
                        }
                        if (xi_flush == CE_TRUE )
                        {
                            memset(this_device_ptr->ram_buffer_ptr, 0 , this_device_ptr->ram_buffer_size);
                            this_device_ptr->ram_buffer_write_ptr = this_device_ptr->ram_buffer_ptr;
                        }
                    }
                }
            }
        }
    }
    else
    {
        /* not a valid logger */
        return LOGGER_GEN_ERROR;
    }

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_item_throttle.                                              */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Set an item throttle threshold                                         */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set an item throttle threshold                                         */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_item_id - item_id                                                    */  
/*  xi_throttle_threshold - threshold                                       */  
/*                                                                          */  
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_item_throttle(LOGGER_ITEM_ID xi_item_id, unsigned long xi_throttle_threshold) 
{
    LOGGER_ITEM *item_ptr;

    /* Get the item */
    item_ptr = (LOGGER_ITEM *) xi_item_id;

    /* Is it a valid item? */
    if (item_ptr->magic == LOGGER_ITEM_MAGIC )
    {
        item_ptr->throttle_threshold = xi_throttle_threshold;
        item_ptr->throttle_counter = xi_throttle_threshold;
    }
    else
    {
        /* not a valid logger */
        return LOGGER_GEN_ERROR;
    }
    return LOGGER_OK;

}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_log_parameters                                              */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Set logger parameters after it was launched                            */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set logger parameters after it was launched                            */
/*    parameters may be : trigger or ram                                    */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_device - file descriptor of the printing device                      */
/*  xi_action - CE_TRUE or CE_FALSE                                         */
/*  xi_which_parameter - trigger or ram                                     */
/*                                                                          */
/*                  LOGGER_RAM_MODE                                         */
/*                  LOGGER_TRIGGER_MODE                                     */
/*                  LOGGER_NORMAL_MODE                                      */
/*                                                                          */
/*  xi_parameter - for trigger :                                            */
/*                  LOGGER_TRIGGER_CLEAR_STATE,                             */
/*                  LOGGER_TRIGGER_CLEAR_MODE                               */
/*                                                                          */
/*                 for ram :                                                */
/*                  LOGGER_BUFFER_MODE_NORMAL                               */
/*                  LOGGER_BUFFER_MODE_CYCLIC                               */
/*  xi_ram_size - ram size in bytes                                         */
/*                                                                          */  
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_log_parameters(unsigned long xi_device, 
    unsigned long xi_log_ptr, long xi_action, LOGGER_MODES xi_which_parameter, 
    unsigned long xi_parameter_value, unsigned long xi_ram_size) 
{
    LOGGER_DEVICE *device_ptr;
    LOGGER_LOG *log_ptr = (LOGGER_LOG *) xi_log_ptr;
    long found = CE_FALSE;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    while(log_ptr != 0 )
    {
         /* Traverse the item's devices link list */
         device_ptr = log_ptr->devices_ptr;
         while(( device_ptr != NULL ) &&(found == CE_FALSE ) )
         {
             /* Match? */
             if (xi_device == device_ptr->device )
             {
                 switch(xi_which_parameter )
                 {
                 case LOGGER_RAM_MODE :
                     switch(xi_action )
                     {
                     case CE_TRUE :
                         device_ptr->has_ram_buffer = CE_TRUE;
                         if (device_ptr->ram_buffer_ptr == NULL )
                         {
                             device_ptr->ram_buffer_ptr = ( char *)logger_alloc(xi_ram_size);
                             if (device_ptr->ram_buffer_ptr == NULL )
                             {
                                 device_ptr->has_ram_buffer = CE_FALSE;
                             }
                             else
                             {
                                 memset(device_ptr->ram_buffer_ptr, 0, xi_ram_size);
                             }
                         }
                         else
                         {
                             if (device_ptr->ram_buffer_size != xi_ram_size )
                             {
								 logger_free(device_ptr->ram_buffer_ptr); 
                                 device_ptr->ram_buffer_ptr = (char *)logger_alloc(xi_ram_size);
                             }
                             if (device_ptr->ram_buffer_ptr == NULL )
                             {
                                 device_ptr->has_ram_buffer = CE_FALSE;
                             }
                             else
                             {
                                 memset(device_ptr->ram_buffer_ptr, 0, xi_ram_size);
                             }
                         }
                         device_ptr->ram_buffer_size = xi_ram_size;
                         device_ptr->ram_buffer_mode = (LOGGER_BUFFER_MODE) xi_parameter_value;
                         device_ptr->oldest_ram_buffer_ptr = device_ptr->ram_buffer_ptr;
                         device_ptr->ram_buffer_write_ptr = device_ptr-> ram_buffer_ptr;

                         break;

                     case CE_FALSE :
                         device_ptr->has_ram_buffer = CE_FALSE;
                         break;
                     }
                     break;

                 case LOGGER_TRIGGER_MODE :
                     switch(xi_action )
                     {
                     case CE_TRUE :
                         device_ptr->has_trigger = CE_TRUE;
                         break;

                     case CE_FALSE :
                         if (( LOGGER_TRIGGER_MODES) xi_parameter_value == LOGGER_TRIGGER_CLEAR_MODE)
                         {
                             device_ptr->has_trigger = CE_FALSE;
                         }
                         device_ptr->trigger_state = LOGGER_TRIGGER_ACTION_NONE;
                         break;
                     }
                     break;

                 case LOGGER_NORMAL_MODE :
                     device_ptr->trigger_state = LOGGER_TRIGGER_ACTION_NONE;
                     device_ptr->has_trigger = CE_FALSE;
                     device_ptr->has_ram_buffer = CE_FALSE;
                     break;
                 }
                 found = CE_TRUE;
             }
            /* Get the next device */
            device_ptr = device_ptr->next_device_ptr;
         }
         /* Get the next log */
         log_ptr = log_ptr->next_log_ptr;
    }

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_trigger_action                                              */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Set item as trigger start | stop                                       */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set an item as start record logs or stop record logs                   */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_device - file descriptor of the printing device                      */
/*  xi_action - LOGGER_TRIGGER_ACTION_NONE                                  */
/*              LOGGER_TRIGGER_ACTION_START                                 */
/*              LOGGER_TRIGGER_ACTION_STOP                                  */
/*  xi_count - after which appearance of the trigger, the trigger is        */
/*             actually set                                                 */
/*  xi_object - pointer to string name of the item                          */
/*                                                                          */  
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_trigger_action(unsigned long xi_device, 
    unsigned long xi_log_ptr, LOGGER_TRIGGER_ACTION xi_action, 
    unsigned long xi_count, char *xi_object) 
{
    LOGGER_DEVICE *device_ptr;
    LOGGER_SECTION *section_ptr;
    LOGGER_GROUP *group_ptr;
    LOGGER_ITEM *item_ptr;
    LOGGER_LOG *log_ptr = (LOGGER_LOG *) xi_log_ptr;
    long found = CE_FALSE;

    /* Is it a valid log? */
    if (log_ptr->magic != LOGGER_LOG_MAGIC )
    {
        return LOGGER_GEN_ERROR;
    }

    while(log_ptr != 0 )
    {
         /* Traverse the item's devices link list */
         device_ptr = log_ptr->devices_ptr;
         while(( device_ptr != NULL ) &&(found == CE_FALSE ) )
         {
             /* Match? */
             if (xi_device == device_ptr->device )
             {
                 /* Traverse the sections link list */
                 section_ptr = log_ptr->sections_ptr;
                 while(( section_ptr != NULL ) &&(found == CE_FALSE ) )
                 {
                     /* Traverse the groups link list */
                     group_ptr = section_ptr->groups_ptr;
                     while(( group_ptr != NULL ) &&(found == CE_FALSE ) )
                     {
                         /* Traverse the items link list */
                         item_ptr = group_ptr->items_ptr;
                         while(( item_ptr != NULL ) &&(found == CE_FALSE ) )
                         {
                             /* Match? */
                             if (f_match_name(xi_object, 4, log_ptr->name, section_ptr->name, group_ptr->name, item_ptr->name ) == CE_TRUE )
                             {
                                 item_ptr->item_trigger_state = xi_action;
                                 item_ptr->trigger_count = xi_count;
                                 item_ptr->trigger_current_count = 0;
                                 if (xi_action != LOGGER_TRIGGER_ACTION_NONE)
                                 {
                                     p_set_section_state(xi_device, section_ptr, CE_TRUE);
                                     p_set_group_state(xi_device, section_ptr, group_ptr, CE_TRUE);
                                     p_set_item_state(xi_device, item_ptr, CE_TRUE);
                                 }
                                 else
                                 {
                                    device_ptr->trigger_state = LOGGER_TRIGGER_ACTION_NONE;
                                 }
                                 found = CE_TRUE;
                             }
                             /* Get the next item */
                             item_ptr = item_ptr->next_item_ptr;
                         }  /* item loop */
                         /* Get the next group */
                         group_ptr = group_ptr->next_group_ptr;
                     }  /* group loop */
                     /* Get the next section */
                     section_ptr = section_ptr->next_section_ptr;
                 }  /* section loop */
             } /* device current */
             /* Get the next device */
             device_ptr = device_ptr->next_device_ptr;
         } /* device loop */ 
         /* Get the next log */
         log_ptr = log_ptr->next_log_ptr;
    } /* log loop */

    return LOGGER_OK;
}

/****************************************************************************/
/*                                                                          */
/* Static routines implementation                                           */
/*                                                                          */
/****************************************************************************/

static void p_logger_release_os_resources(LOGGER_LOG_ID xi_log_id)
{
    LOGGER_LOG *log_ptr;
   
    log_ptr = (LOGGER_LOG *) xi_log_id;

    /* set global parameters */
    gs_logger_array[0].log_message_ptr = NULL;
    gs_logger_array[0].log_ptr = NULL;
    gs_logs_ptr[gs_logs --] = NULL;

    /* free logger structure */
    logger_free(log_ptr);
}

static long f_match_name(char *xi_name, unsigned long xi_number, ...)
{
    char *names[5];
    unsigned long i;
    unsigned long j;
    va_list arguments;
    char name[512];

    /* Validate parameters */
    if (xi_number > 5 )
    {
        return(CE_FALSE);
    }

    /* Get the arguments */
    va_start(arguments, xi_number);
    for(i = 0;i < xi_number;i ++ )
    {
        names[i] = va_arg(arguments, char *);
    }

    /* Iterate for all combinations */
    for(i = 0;i < xi_number;i ++ )
    {
        /* Build the combination */
        sprintf(name, "%s", names[i]);
        for(j = i + 1;j < xi_number;j ++ )
        {
            sprintf(name, "%s.%s", name, names[j]);
        }

        /* Match? */
        if (strcasecmp(xi_name, name ) == 0 )
        {
            return(CE_TRUE);
        }
    }
    
    return(CE_FALSE);
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   f_get_device_ptr                                                       */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Returns the section state pointer                                      */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Returns the section state pointer corresponding to given section       */  
/*    and given session                                                     */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_deviceid - current file descriptor                                   */  
/*  xi_device_ptr - the top of devices chain                                */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  device  state pointer                                                   */
/*                                                                          */
/****************************************************************************/
static LOGGER_DEVICE_STATE *f_get_device_ptr(unsigned long xi_deviceid, LOGGER_DEVICE_STATE *xi_device_top_ptr)
{
    LOGGER_DEVICE_STATE *this_device_ptr = xi_device_top_ptr;

    while(this_device_ptr != NULL )
    {
        if (this_device_ptr->device == xi_deviceid )
        {
            break;
        }
        this_device_ptr = this_device_ptr->next_device_ptr;
    }

    /* return the section ptr on current device */
    return(this_device_ptr);
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   p_set_section_state                                                    */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Set the section state                                                  */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set the section state to on or off according to given section          */  
/*    and given session                                                     */  
/*   If the device is not logged or filter is done before enqueue           */  
/*   - set the default state                                                */  
/*                                                                          */
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_deviceid - current file descriptor                                   */  
/*  xi_section_ptr - the current section                                    */  
/*  xi_state    - the state of section                                      */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  none                                                                    */
/*                                                                          */
/****************************************************************************/
static void p_set_section_state(unsigned long xi_deviceid, LOGGER_SECTION *xi_section_ptr, long xi_state)
{
    LOGGER_DEVICE_STATE *this_ptr = NULL;

    this_ptr = f_get_device_ptr(xi_deviceid, xi_section_ptr->devices_ptr);
    if (this_ptr != NULL )
    {
        this_ptr->object_state = xi_state;
    }
    else
    {
        xi_section_ptr->default_state = xi_state;
    }
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   p_set_item_ram_state                                                   */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Set the item ram state                                                 */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set the item ram state to on or off according to given item            */  
/*    and given session                                                     */  
/*   If the device is not logged or filter is done before enqueue           */  
/*   - set the default state                                                */  
/*                                                                          */
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_item_ptr - the current item                                          */  
/*  xi_in_ram - item will be put in ram                                     */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  none                                                                    */
/*                                                                          */
/****************************************************************************/
static void p_set_item_ram_state(LOGGER_ITEM *xi_item_ptr, long xi_in_ram)
{
    xi_item_ptr->item_ram_state = xi_in_ram;
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   p_set_group_ram_state                                                  */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Set the group ram state                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set the group ram state to on or off according to given group          */  
/*    and given session                                                     */  
/*   If the device is not logged or filter is done before enqueue           */  
/*   - set the default state(false )                                        */  
/*   All the items of the group are set to the group state                  */
/*                                                                          */
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_deviceid - current file descriptor                                   */  
/*  xi_section_ptr - the current section                                    */  
/*  xi_group_ptr - the current group                                        */  
/*  xi_in_ram   - the state of group                                        */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  none                                                                    */
/*                                                                          */
/****************************************************************************/
static void p_set_group_ram_state(unsigned long xi_deviceid, 
   LOGGER_SECTION *xi_section_ptr, LOGGER_GROUP *xi_group_ptr, long xi_in_ram)
{
    LOGGER_ITEM *item_ptr = xi_group_ptr->items_ptr;

    while(item_ptr != NULL ) 
    {
        p_set_item_ram_state(item_ptr, xi_in_ram);
        item_ptr = item_ptr->next_item_ptr;
    }
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   p_set_section_ram_state                                                */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Set the section ram state                                              */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set the section ram state to on or off according to given section      */  
/*    and given session                                                     */  
/*   If the device is not logged or filter is done before enqueue           */  
/*   - set the default state(false )                                        */  
/*   All the items of the group are set to the group state                  */
/*                                                                          */
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_deviceid - current file descriptor                                   */  
/*  xi_section_ptr - the current section                                    */  
/*  xi_in_ram   - the state of section                                      */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  none                                                                    */
/*                                                                          */
/****************************************************************************/
static void p_set_section_ram_state(unsigned long xi_deviceid, 
    LOGGER_SECTION *xi_section_ptr, long xi_in_ram)
{
    LOGGER_GROUP *group_ptr;

    group_ptr = xi_section_ptr->groups_ptr;
    while(group_ptr != NULL ) 
    {
        p_set_group_ram_state(xi_deviceid, xi_section_ptr, group_ptr, xi_in_ram);
        group_ptr = group_ptr->next_group_ptr;
    }
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   p_set_group_state                                                      */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Set the group state                                                    */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set the group  state to on or off according to given group             */  
/*    and given session                                                     */  
/*   If the device is not logged or filter is done before enqueue           */  
/*   - set the default state                                                */  
/*                                                                          */
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_deviceid - current file descriptor                                   */  
/*  xi_section_ptr - the current section                                    */  
/*  xi_group_ptr - the current group                                        */  
/*  xi_state    - the state of group                                        */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  none                                                                    */
/*                                                                          */
/****************************************************************************/
static void p_set_group_state(unsigned long xi_deviceid, 
   LOGGER_SECTION *xi_section_ptr, LOGGER_GROUP *xi_group_ptr, long xi_state)
{
    LOGGER_DEVICE_STATE *this_ptr = NULL;

    this_ptr = f_get_device_ptr(xi_deviceid, xi_group_ptr->devices_ptr);
    if (this_ptr != NULL )
    {
        this_ptr->object_state = xi_state;
    }
    else
    { 
        xi_group_ptr->default_state = xi_state;
    }
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   p_set_item_state                                                       */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Set the item    state                                                  */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set the item state to on or off according to given item                */  
/*    and given session                                                     */  
/*   If the device is not logged or filter is done before enqueue           */  
/*   - set the default state                                                */  
/*                                                                          */
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_deviceid - current file descriptor                                   */  
/*  xi_item_ptr - the current item                                          */  
/*  xi_state    - the state of item                                         */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  none                                                                    */
/*                                                                          */
/****************************************************************************/
static void p_set_item_state(unsigned long xi_deviceid, LOGGER_ITEM *xi_item_ptr, long xi_state)
{
    LOGGER_DEVICE_STATE *this_ptr = NULL;

    this_ptr = f_get_device_ptr(xi_deviceid, xi_item_ptr->devices_ptr);
    if (this_ptr != NULL )
    {
        this_ptr->object_state = xi_state;
    }
    else
    {
        xi_item_ptr->default_state = xi_state;
    }
    xi_item_ptr->throttle_counter = xi_item_ptr->throttle_threshold;
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   f_get_all_devices_state                                                */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Put in queue to be printed later                                       */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   In case the item is on into , at least one device, put it in queue     */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_device_state - top of the dvices chain                               */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  BOOLEAN - true must be put in queue                                     */
/*                                                                          */
/****************************************************************************/
static long f_get_all_devices_state(LOGGER_DEVICE_STATE *xi_device_ptr)
{
    long possible_state = CE_FALSE;
    LOGGER_DEVICE_STATE *this_device_ptr = xi_device_ptr;

    while ((this_device_ptr != NULL) &&(possible_state == CE_FALSE))
    {
        possible_state = this_device_ptr->object_state;
        if (possible_state == CE_FALSE)
        {
            this_device_ptr = this_device_ptr->next_device_ptr;
        }
    }

    return(possible_state);
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   f_check_trigger                                                        */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Check if the given item is a trigger                                   */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Set the item state corresponding to its trigger definitionin queue     */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_device_ptr - current device pointer                                  */  
/*  xi_item_state_ptr -  current item                                       */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  BOOLEAN - true must be put in queue                                     */
/*                                                                          */
/****************************************************************************/
static long f_check_trigger(LOGGER_DEVICE *xi_device_ptr, LOGGER_ITEM *xi_item_ptr)
{
    long retvalue = CE_TRUE;

    /* if the device has trigger - act according to its state */
    if (xi_device_ptr->has_trigger == CE_TRUE)
    {
        /* check if the current item is defined as trigger */
        switch(xi_item_ptr->item_trigger_state)
        {
        case LOGGER_TRIGGER_ACTION_START:
            /* it is the 'start' */
            switch(xi_device_ptr->trigger_state)
            {
                /* it is NOT the first time - it is not a real start */
            case LOGGER_TRIGGER_ACTION_START:
                break;
            case LOGGER_TRIGGER_ACTION_STOP:
                retvalue = CE_FALSE;
                break;
                /* it is the first time - it is may the real start */
                /* increase the counter - it reach the threshold => bingo, it is the start */
            case LOGGER_TRIGGER_ACTION_NONE:
                xi_item_ptr->trigger_current_count ++;
                if (xi_item_ptr->trigger_current_count == xi_item_ptr->trigger_count)
                {
                    xi_device_ptr->trigger_state = LOGGER_TRIGGER_ACTION_START;
                }
                else
                {
                    retvalue = CE_FALSE;
                }
                break;
            }
            break;
        case LOGGER_TRIGGER_ACTION_STOP:
            /* it is the 'stop' */
            switch(xi_device_ptr->trigger_state)
            {
            case LOGGER_TRIGGER_ACTION_START:
                /* it is the first time - it is may the real stop */
                /* increase the counter - it reach the threshold => bingo, it is the stop */
                xi_item_ptr->trigger_current_count++;
                if (xi_item_ptr->trigger_current_count == xi_item_ptr->trigger_count)
                {
                   xi_device_ptr->trigger_state = LOGGER_TRIGGER_ACTION_STOP;
                }
                else
                {
                    retvalue = CE_FALSE;
                }
                break;
            case LOGGER_TRIGGER_ACTION_STOP:
            case LOGGER_TRIGGER_ACTION_NONE:
                retvalue = CE_FALSE;
                break;
            }
            break;
        case LOGGER_TRIGGER_ACTION_NONE:
            /* a regular 'ON' item */
            switch(xi_device_ptr->trigger_state)
            {
                /* in between 'start' and 'stop' => put it in queue */
            case LOGGER_TRIGGER_ACTION_START:
                break;
            case LOGGER_TRIGGER_ACTION_STOP:
            case LOGGER_TRIGGER_ACTION_NONE:
                retvalue = CE_FALSE;
                break;
            }
            break;
        }
    }
    return(retvalue);
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   f_put_in_queue                                                         */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Put in queue to be printed later                                       */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   In case the item is on into , at least one device, put it in queue     */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_log_ptr -  current logger                                            */  
/*  xi_section_state_ptr -  current section                                 */  
/*  xi_group_state_ptr -  current group                                     */  
/*  xi_item_state_ptr -  current item                                       */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  BOOLEAN - true must be put in queue                                     */
/*                                                                          */
/****************************************************************************/
static long f_put_in_queue(LOGGER_LOG *xi_log_ptr, LOGGER_SECTION *xi_section_ptr, 
    LOGGER_GROUP *xi_group_ptr, LOGGER_ITEM *xi_item_ptr)
{
    LOGGER_LOG *log_ptr = xi_log_ptr;
    LOGGER_DEVICE *this_device_ptr = log_ptr->devices_ptr;
    long possible_state = CE_FALSE;

    /* while on all defined devices */
    while(( this_device_ptr != NULL ) &&(possible_state == CE_FALSE))
    {
        /* only for the devices on 'ON' state */
        if (this_device_ptr->state == CE_TRUE )
        {
            possible_state = f_get_all_devices_state(xi_section_ptr->devices_ptr);
			if (possible_state == CE_TRUE)
			{
                possible_state = f_get_all_devices_state(xi_group_ptr->devices_ptr);
				if (possible_state == CE_TRUE)
				{
                    possible_state = f_get_all_devices_state(xi_item_ptr->devices_ptr);
                }
             }
        }
        this_device_ptr = this_device_ptr->next_device_ptr;
    }
	
    return possible_state;
}


/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   p_put_in_ram                                                           */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Put the mesage into the RAM buffer                                     */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Put the mesage into the RAM buffer                                     */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_log_ptr   - current logger                                           */  
/*  xi_device_ptr- current device                                           */  
/*  xi_log_message_ptr - message to be printed                              */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/****************************************************************************/
static void p_put_in_ram(LOGGER_LOG *xi_log_ptr, LOGGER_DEVICE *xi_device_ptr, char *xi_log_message_ptr)
{
    char *temp_ptr;

    /* Handle the write pointer according to the RAM buffer mode */
    switch(xi_device_ptr-> ram_buffer_mode )
    {
    case LOGGER_BUFFER_MODE_NORMAL :
        /* End of RAM buffer reached? */
        if (xi_device_ptr->ram_buffer_write_ptr + strlen(xi_log_message_ptr ) + 1 > xi_device_ptr->ram_buffer_ptr + xi_device_ptr->ram_buffer_size) 
        {
            /* Call notofication callback */
            if (xi_log_ptr->notification_callback != NULL)
            {
                xi_log_ptr->notification_callback(LOGGER_NOTIFICATION_FULL, 0);
            }
            else
            {
                /* Set notification */
                fdprintf(xi_device_ptr->device, " " LOGGER_BOLD "RAM buffer is full!\n\r" LOGGER_NORMAL);
            }
        }
        else
        {
            /* Copy the log message to the RAM buffer */
            memcpy(xi_device_ptr->ram_buffer_write_ptr, xi_log_message_ptr, strlen(xi_log_message_ptr ));

            /* Advance the write pointer */
            xi_device_ptr->ram_buffer_write_ptr += strlen(xi_log_message_ptr);
            *xi_device_ptr->ram_buffer_write_ptr ++ = 0;
        }
        break;

    case LOGGER_BUFFER_MODE_CYCLIC :
        /* End of RAM buffer reached? */
        if (( xi_device_ptr->ram_buffer_write_ptr + strlen(xi_log_message_ptr ) + 1 ) >(xi_device_ptr->ram_buffer_ptr + xi_device_ptr->ram_buffer_size ) )
        {
            temp_ptr = xi_device_ptr->ram_buffer_ptr + strlen(xi_log_message_ptr);
            temp_ptr = strchr(temp_ptr, 0);
            xi_device_ptr->oldest_ram_buffer_ptr = temp_ptr + 1;
            /* Call notofication callback */
            if (xi_log_ptr->notification_callback != NULL )
            {
                xi_log_ptr->notification_callback(LOGGER_NOTIFICATION_WRAP, 0);
            }
            else
            {
                /* Set notification */
                fdprintf(xi_device_ptr->device, " " LOGGER_BOLD "RAM buffer wrapped!\n\r" LOGGER_NORMAL);
            }
            /* Wrap back to start of the RAM buffer */
            xi_device_ptr->ram_buffer_write_ptr = xi_device_ptr->ram_buffer_ptr;
            xi_device_ptr->ram_wrapped = CE_TRUE;
        }
        else
        {
            if (xi_device_ptr->ram_wrapped == CE_TRUE)
            {
                if (( xi_device_ptr->ram_buffer_write_ptr + strlen(xi_log_message_ptr ) + 1 ) >= xi_device_ptr->oldest_ram_buffer_ptr)
                {
                    temp_ptr = xi_device_ptr->ram_buffer_write_ptr + strlen(xi_log_message_ptr);
                    temp_ptr = strchr(temp_ptr, 0);
                    xi_device_ptr->oldest_ram_buffer_ptr = temp_ptr + 1;
                }
            }
        }
        /* Copy the log message to the RAM buffer */
        memcpy(xi_device_ptr->ram_buffer_write_ptr, xi_log_message_ptr, strlen(xi_log_message_ptr));
        /* Advance the write pointer */
        xi_device_ptr->ram_buffer_write_ptr += strlen(xi_log_message_ptr);
        *xi_device_ptr->ram_buffer_write_ptr ++ = 0;
        break;
    }
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   p_print_to_devices                                                     */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Print to the given device                                              */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   print the log to all devices on which the section/group and item are on*/  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_log_ptr -  current logger                                            */  
/*  xi_section_state_ptr -  current section                                 */  
/*  xi_group_state_ptr -  current group                                     */  
/*  xi_item_state_ptr -  current item                                       */  
/*  xi_log_item -  final item                                               */  
/*  xi_notification -  current notification                                 */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  none                                                                    */
/*                                                                          */
/****************************************************************************/
static void p_print_to_devices(LOGGER_LOG *xi_log_ptr, LOGGER_SECTION *xi_section_ptr, 
    LOGGER_GROUP *xi_group_ptr, LOGGER_ITEM *xi_item_ptr, 
    char *xi_log_item, char *xi_notification)
{
    LOGGER_LOG *log_ptr = xi_log_ptr;
    LOGGER_DEVICE *this_device_ptr = log_ptr->devices_ptr;
    LOGGER_DEVICE_STATE *section_device_state_ptr;
    LOGGER_DEVICE_STATE *group_device_state_ptr;
    LOGGER_DEVICE_STATE *item_device_state_ptr;
    
    /* while on all defined devices */
    while(this_device_ptr != NULL )
    {
        /* only for the devices on 'ON' state */
        if (this_device_ptr->state == CE_TRUE )
        {
            /* take the section's top */
            section_device_state_ptr = xi_section_ptr->devices_ptr;
            while(section_device_state_ptr != NULL )
            {
                /* check it is the section state of the required section */
                /* and it is in 'ON' state */
                if (( section_device_state_ptr->object_state == CE_TRUE ) &&(section_device_state_ptr->device == this_device_ptr->device ) )
                {
                    /* take the group's top */
                    group_device_state_ptr = xi_group_ptr->devices_ptr;
                    while(group_device_state_ptr != NULL ) 
                    {
                        /* check it is the group state of the required group */
                        /* and it is in 'ON' state */
                        if (( group_device_state_ptr->object_state == CE_TRUE ) &&(group_device_state_ptr->device == this_device_ptr->device ) )
                        {
                            /* take the item's top */
                            item_device_state_ptr = xi_item_ptr->devices_ptr;
                            while(item_device_state_ptr != NULL ) 
                            {
                                if (( item_device_state_ptr->object_state == CE_TRUE ) &&(item_device_state_ptr->device == this_device_ptr->device ) )
                                {
                                    /* and it is in 'ON' state */
                                    if (xi_item_ptr->throttle_counter > 0 )
                                    {
                                       xi_item_ptr->throttle_counter --;
                                    }
                                    else
                                    {
                                        xi_item_ptr->throttle_counter = xi_item_ptr->throttle_threshold;
                                        if (f_check_trigger(this_device_ptr, xi_item_ptr ) == CE_TRUE )
                                        {
                                            if (xi_item_ptr->item_ram_state == CE_TRUE )
                                            {
                                                if (( this_device_ptr->has_ram_buffer == CE_TRUE ) &&(this_device_ptr->ram_buffer_write_ptr != NULL ) )
                                                {
                                                    p_put_in_ram(log_ptr, this_device_ptr, xi_log_item);
                                                }
                                                else
                                                {
                                                     /* Print to the device */
                                                    fdprintf(item_device_state_ptr->device, "%s%s\n\r", xi_log_item, xi_notification);
                                                }
                                            }
                                            else
                                            {
                                                if (this_device_ptr->device_ram_state == CE_TRUE )
                                                {
                                                    if (( this_device_ptr->has_ram_buffer == CE_TRUE ) &&(this_device_ptr->ram_buffer_write_ptr != NULL ) )
                                                    {
                                                        p_put_in_ram(log_ptr, this_device_ptr, xi_log_item);
                                                    }
                                                    else
                                                    {
                                                        /* Print to the device */
                                                        fdprintf(item_device_state_ptr->device, "%s%s\n\r", xi_log_item, xi_notification);
                                                    }
                                                }
                                                else
                                                {
                                                     /* Print to the device */
                                                    fdprintf(item_device_state_ptr->device, "%s%s\n\r", xi_log_item, xi_notification);
                                                }
                                            }
                                        } /* my device */
                                    }
                                } /* item is 'on' */
                                item_device_state_ptr = item_device_state_ptr->next_device_ptr;
                            }
                        }
                        group_device_state_ptr = group_device_state_ptr->next_device_ptr;
                    } /* groups */
                }
                section_device_state_ptr = section_device_state_ptr->next_device_ptr;
            } /* section */
        } /* device on */
        this_device_ptr = this_device_ptr->next_device_ptr;
    }  /* while on devices */
}

static void p_logger(LOGGER_LOG_MESSAGE *xi_log_message_ptr)
{
    char timestamp[LOGGER_FORMAT_LENGTH];
    char time_of_day[LOGGER_NAME_LENGTH];
    char log_information[LOGGER_LOG_INFORMATION_LENGTH];
    LOGGER_LOG *log_ptr;
    LOGGER_SECTION *section_ptr;
    LOGGER_GROUP *group_ptr;
    LOGGER_ITEM *item_ptr;
    unsigned long section_id;
    unsigned long group_id;
    unsigned long item_id;
    /*va_list arguments;*/
    char notification[LOGGER_DESCRIPTION_LENGTH];
    char context[LOGGER_NAME_LENGTH];
    char *format;
    unsigned long local_lost_buffers;
    unsigned long length;

    /* Get the various control structures */
    log_ptr = xi_log_message_ptr->parent_log_ptr;
    item_ptr = xi_log_message_ptr->item_ptr;
    group_ptr = item_ptr->parent_group_ptr;
    section_ptr = group_ptr->parent_section_ptr;

    /* Set notification and context */
    memset(notification, 0, LOGGER_DESCRIPTION_LENGTH);
    memset(context, 0, LOGGER_NAME_LENGTH);

    /* Format the timestamp */
    if ((log_ptr->time_base == LOGGER_TIME_BASE_USER_DEFINED) &&
        (log_ptr->timestamp_formatting_callback == NULL ) )
    {
        p_default_none_timestamp_formatting(xi_log_message_ptr->timestamp, timestamp);
    }
    else
    {
        log_ptr->timestamp_formatting_callback(xi_log_message_ptr->timestamp, timestamp);
    }

    /* Format the timeofday */
    if ((log_ptr->time_base == LOGGER_TIME_BASE_USER_DEFINED) &&
        (log_ptr->timeofday_formatting_callback == NULL))
    {
        p_default_none_timeofday_formatting(xi_log_message_ptr->daytime, time_of_day);
    }
    else
    {
        log_ptr->timeofday_formatting_callback(xi_log_message_ptr->daytime, time_of_day);
    }
    /* Get the section, group, and item IDs */
    section_id = (unsigned long)section_ptr;
    group_id = (unsigned long)group_ptr;
    item_id = (unsigned long)item_ptr;

    /* Format the log information */
    xi_log_message_ptr->parent_log_ptr->log_information_formatting_callback(
        log_ptr->description, time_of_day, timestamp, xi_log_message_ptr->file, 
        xi_log_message_ptr->line, section_id, group_id, item_id, 
        xi_log_message_ptr->context, log_information);

    /* According to the type of the log item */
    if (item_ptr->type == LOGGER_ITEM_TYPE_NORMAL)
    {
        /* Format the 'NORMAL' item according to the predefined format */
        vsprintf(log_ptr->log_item, item_ptr->format, xi_log_message_ptr->arguments);
    }
    else /* format */ 
    {
        /* Format the 'FORMAT' item according to the provided format */
        /* Get the first argument which is the format string */
        format = va_arg(xi_log_message_ptr->arguments, char *);
        /* Format the log according to the fetched format */
        vsprintf(log_ptr->log_item, format, xi_log_message_ptr->arguments);
    }

    va_end(xi_log_message_ptr->arguments);

    /* Add the item attributes */
    if (item_ptr->attributes != LOGGER_ATTRIBUTE_NORMAL)
    {
        /* Turn on various attributes */
        {
            /* Start at begining of buffer */
            length = 0;

            /* Bold */
            if (item_ptr->attributes & LOGGER_ATTRIBUTE_BOLD)
            {
                length += sprintf(log_ptr->final_item, LOGGER_BOLD);
            }

            /* Underline */
            if (item_ptr->attributes &LOGGER_ATTRIBUTE_UNDERLINE)
            {
                length += sprintf(& log_ptr->final_item[length], LOGGER_UNDERLINE);
            }

            /* Blink */
            if (item_ptr->attributes &LOGGER_ATTRIBUTE_BLINK)
            {
                length += sprintf(& log_ptr->final_item[length], LOGGER_BLINK);
            }

            /* Reverse video */
            if (item_ptr->attributes &LOGGER_ATTRIBUTE_REVERSE_VIDEO)
            {
                length += sprintf(& log_ptr->final_item[length], LOGGER_REVERSE_VIDEO);
            }

            /* Reverse video */
            if (item_ptr->attributes & LOGGER_ATTRIBUTE_BEEP)
            {
                length += sprintf(& log_ptr->final_item[length], LOGGER_BEEP);
            }
        }
    
        /* Turn off all attributes */
        sprintf(& log_ptr->final_item[length], "%s" LOGGER_NORMAL, log_ptr->log_item);
        strcpy(log_ptr->log_item, log_ptr->final_item);
    }

    /* Format the final log item */
    xi_log_message_ptr->parent_log_ptr->log_item_formatting_callback(log_information, log_ptr->log_item, log_ptr->final_item);

    /* If no notification formatted yet, check the lost buffers counter */
    local_lost_buffers = log_ptr->lost_buffers;
    if ((strlen(notification) == 0) && (local_lost_buffers > 0))
    {
    	sprintf(notification, " " LOGGER_BOLD "(%ld message%s lost!)" LOGGER_NORMAL, local_lost_buffers, (local_lost_buffers > 1 ? "s" : ""));
    	log_ptr->lost_buffers -= local_lost_buffers;
    }
    
    p_print_to_devices(log_ptr, section_ptr, group_ptr, item_ptr, log_ptr->final_item, notification);
}


static uint64_t f_default_none_timestamping(void )
{
    return(0);
}

static void p_default_none_timestamp_formatting(uint64_t xi_timestamp, char *xo_buffer )
{
    sprintf(xo_buffer, "no timestamp");
}

static void p_default_none_timeofday(unsigned char *xi_timeofday ) 
{
    xi_timeofday[0] = 0;
    xi_timeofday[1] = 0;
    xi_timeofday[2] = 0;
}
static void p_default_none_timeofday_formatting(unsigned char *xi_timeofday, char *xo_buffer )
{
    sprintf(xo_buffer, "[]");
}

static void p_default_timeofday(unsigned char *xi_timeofday ) 
{
    p_default_none_timeofday(xi_timeofday);
}
static void p_default_timeofday_formatting(unsigned char *xi_timeofday, char *xo_buffer ) 
{
    sprintf(xo_buffer, "[%.2d:%.2d:%.2d] ", xi_timeofday[0], xi_timeofday[1], xi_timeofday[2]);
}


static uint64_t f_default_tick_timestamping(void )
{
    return(tickGet ());
}

static void p_default_tick_timestamp_formatting(uint64_t xi_timestamp, char *xo_buffer)
{
    /* Format timestamp */
    sprintf(xo_buffer, "%llu", xi_timestamp);
}
#if defined CONFIG_BCM96838 || defined CONFIG_BCM96848
static inline uint32_t tick2nanosec(uint32_t tick)
{
    /* Need to multiply tick by 6.430041 to convert from GPON frame ticks to nanosecs */
    uint32_t result = 0;
    uint32_t temp = 0;

    temp = tick *6;
    temp += (tick *4) / 10;
    temp += (tick *3) / 100;
    temp += (tick *4) / 100000;
    temp += (tick *1) / 1000000;
    result = temp;

    return result;
}

static inline uint32_t tick2usec(uint32_t tick)
{
    return tick2nanosec(tick)/1000;
}
#endif

static uint64_t f_default_microsec_timestamping(void)
{
#if defined CONFIG_BCM96838 || defined CONFIG_BCM96848
    uint64_t temp;

    union time_of_day
    {
        struct
        {
            uint64_t sec     : 34;        
            uint64_t nanosec : 30;
        };
        struct
        {
            uint32_t tod_high;
            uint32_t tod_low;
        };
    }hw_tod;
    
    MS1588_MASTER_REGS_M_TODRDCMD todrdcmd;

    /* set the command for reading the ToD.
       no need to read-modify-write, as it is a command register. */
    todrdcmd.todrdcmd = MS1588_MASTER_REGS_M_TODRDCMD_TODRDCMD_TODRDCMD_VALUE;
    todrdcmd.r1 = MS1588_MASTER_REGS_M_TODRDCMD_R1_RESERVED_VALUE;
    MS1588_MASTER_REGS_M_TODRDCMD_WRITE(todrdcmd);
    
    MS1588_MASTER_REGS_M_TODREGH_READ(hw_tod.tod_high);
    MS1588_MASTER_REGS_M_TODREGL_READ(hw_tod.tod_low);

    temp = hw_tod.sec;
    temp = (temp *(uint64_t)(1000000)) + tick2usec(hw_tod.nanosec);

    return temp;
#else
    uint16_t ts48_msb;
    uint32_t ts48_lsb; 
    uint64_t timestamp;

    wan_top_tod_ts48_get(&ts48_msb, &ts48_lsb);

    timestamp  = ts48_msb;
    timestamp = timestamp<<32 | ts48_lsb;
    do_div(timestamp, 1000);

    return timestamp;

#endif
}

static void p_default_microsec_timestamp_formatting(uint64_t xi_timestamp, char *xo_buffer)
{
    /* Format timestamp */
    sprintf(xo_buffer, "%llu", xi_timestamp);
}

static uint64_t f_default_time_timestamping(void)
{
    time_t tm;

    /* Get timestamp */
    tm = time();

    return(( unsigned long ) tm);
}

static void p_default_time_timestamp_formatting(uint64_t xi_timestamp, char *xo_buffer)
{
    time_t tm;
    size_t size;
    char *p;

    /* Format timestamp */
    tm = (time_t)xi_timestamp;
    size = LOGGER_FORMAT_LENGTH;

    ctime_r(&tm, xo_buffer, &size);

    p = xo_buffer;
    while (*p)
    {
        if (*p == '\n')
        {
            *p = '\0';
            break;
        }
        p++;
    }
}

static void p_default_log_information_formatting(char *xi_log, char *xi_time_of_day, 
    char *xi_timestamp, char *xi_file, unsigned long xi_line, LOGGER_SECTION_ID xi_section_id, 
    LOGGER_GROUP_ID xi_group_id, LOGGER_ITEM_ID xi_item_id, char *xi_context, char *xo_log_information)
{
    LOGGER_SECTION *section_ptr;
    LOGGER_GROUP *group_ptr;
    LOGGER_ITEM *item_ptr;
    char buffer[LOGGER_LOG_INFORMATION_LENGTH];
    unsigned long length = 0;

    /* Get the section */
    section_ptr = (LOGGER_SECTION *) xi_section_id;

    /* Is it a valid group? */
    if (section_ptr->magic != LOGGER_SECTION_MAGIC)
    {
        /* Set error */
        sprintf(xo_log_information, "error!");
        return;
    }

    /* Get the group */
    group_ptr = (LOGGER_GROUP *) xi_group_id;

    /* Is it a valid group? */
    if (group_ptr->magic != LOGGER_GROUP_MAGIC )
    {
        /* Set error */
        sprintf(xo_log_information, "error!");
        return;
    }

    /* Get the item */
    item_ptr = (LOGGER_ITEM *) xi_item_id;

    /* Is it a valid item? */
    if (item_ptr->magic != LOGGER_ITEM_MAGIC )
    {
        /* Set error */
        sprintf(xo_log_information, "error!");
        
        return;
    }

    /* Format the log information */
    memset(buffer, '_', LOGGER_LOG_INFORMATION_LENGTH);
    buffer[LOGGER_LOG_INFORMATION_LENGTH - 1] = '\0';

    /* Format the log information */
    length = sprintf (buffer, "[%-16.16s] [%-15.15s %5ld] [%-25.25s]",
                       xi_timestamp, xi_file, xi_line, xi_context);
    buffer[length] = ' ';

    /* Copy to caller's buffer */
    strcpy(xo_log_information, buffer);
}

static void p_default_log_item_formatting(char *xi_log_information, char *xi_item, char *xo_log_item )
{
    /* Format the log item */
    sprintf(xo_log_item, "%-70.70s %s", xi_log_information, xi_item);
}

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   fi_get_log_ptr                                                         */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Commands initialization.                                      */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function initializes the logger shell commands.                    */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_current_path - The path name                                         */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR_DTS - Pointer to logger                                   */
/*                                                                          */
/****************************************************************************/
unsigned long fi_get_log_ptr(char *xi_current_path ) 
{
    char tt[120];
    unsigned long k;
    long index = -1;

    for(k = 0; k < gs_logs; k ++ )
    {
		if (gs_logger_path[k][ 0] == '/')
			sprintf(tt, "%s%c", gs_logger_path[ k], gs_logger_path[k][ 0]);
		else
			sprintf(tt, "/%s", gs_logger_path[ k]);
        if (strcmp( xi_current_path, tt ) == 0 )
        {
            index = k;
            break;
        }
    }
    if (index >= 0 )
    {
        return(( unsigned long ) gs_logs_ptr[index]);
    }
    return 0;
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   p_clear_items                                                          */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Release the memory of an item state chain                              */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Release the memory of an item state chain                              */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_item_state_ptr -  current item                                       */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  none                                                                    */
/*                                                                          */
/****************************************************************************/
static void p_clear_items(unsigned long xi_device, LOGGER_ITEM *xi_item_ptr)
{
    LOGGER_DEVICE_STATE *temp_ptr = NULL;
    LOGGER_DEVICE_STATE *ptemp_ptr = NULL;
    LOGGER_ITEM *item_ptr = xi_item_ptr;

    while(item_ptr != NULL )
    {
        temp_ptr = item_ptr->devices_ptr;
        ptemp_ptr = NULL;
        /* go to the end of the sections chain */
        while(temp_ptr != NULL )
        {
            if (temp_ptr->device == xi_device )
            {
                if (ptemp_ptr == NULL )
                {
                    item_ptr->devices_ptr = NULL;
                }
                else
                {
                    ptemp_ptr->next_device_ptr = temp_ptr->next_device_ptr;
                }
                logger_free(temp_ptr);
                break;
            }
            else
            {
                ptemp_ptr = temp_ptr;
                temp_ptr = temp_ptr->next_device_ptr;
            }
        }
        item_ptr = item_ptr->next_item_ptr;
    }
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   p_clear_groups                                                         */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Release the memory of a group state chain                              */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Release the memory of a group state chain                              */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_group_state_ptr -  current group                                     */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  none                                                                    */
/*                                                                          */
/****************************************************************************/
static void p_clear_groups(unsigned long xi_device, LOGGER_GROUP *xi_group_ptr)
{
    LOGGER_DEVICE_STATE *temp_ptr = NULL;
    LOGGER_DEVICE_STATE *ptemp_ptr = NULL;
    LOGGER_GROUP *group_ptr = xi_group_ptr;

    while(group_ptr != NULL )
    {
        temp_ptr = group_ptr->devices_ptr;
        ptemp_ptr = NULL;
        /* go to the end of the sections chain */
        while(temp_ptr != NULL )
        {
            if (temp_ptr->device == xi_device )
            {
                if (ptemp_ptr == NULL )
                {
                    group_ptr->devices_ptr = NULL;
                }
                else
                {
                    ptemp_ptr->next_device_ptr = temp_ptr->next_device_ptr;
                }
                logger_free(temp_ptr);
                break;
            }
            else
            {
                ptemp_ptr = temp_ptr;
                temp_ptr = temp_ptr->next_device_ptr;
            }
        }
        p_clear_items(xi_device, group_ptr->items_ptr);
        group_ptr = group_ptr->next_group_ptr;
    }
}
/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   p_clear_sections                                                       */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Release the memory of a section state chain                            */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Release the memory of a section state chain                            */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_section_state_ptr -  current section                                 */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  none                                                                    */
/*                                                                          */
/****************************************************************************/
static void p_clear_sections(unsigned long xi_device, LOGGER_SECTION *xi_section_ptr)
{
    LOGGER_DEVICE_STATE *temp_ptr = NULL;
    LOGGER_DEVICE_STATE *ptemp_ptr = NULL;
    LOGGER_SECTION *section_ptr = xi_section_ptr;

    while(section_ptr != NULL )
    {
        temp_ptr = section_ptr->devices_ptr;
        ptemp_ptr = NULL;
        /* go to the end of the sections chain */
        while(temp_ptr != NULL )
        {
            if (temp_ptr->device == xi_device )
            {
                if (ptemp_ptr == NULL )
                {
                    section_ptr->devices_ptr = NULL;
                }
                else
                {
                    ptemp_ptr->next_device_ptr = temp_ptr->next_device_ptr;
                }
                logger_free(temp_ptr);
                break;
            }
            else
            {
                ptemp_ptr = temp_ptr;
                temp_ptr = temp_ptr->next_device_ptr;
            }
        }
        p_clear_groups(xi_device, section_ptr->groups_ptr);
        section_ptr = section_ptr->next_section_ptr;
    }
}


/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_device.                                                  */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Configuration.                                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function sets the default device the logger will print to.         */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_log_id - Log ID                                                      */  
/*  xi_name - Name of the device                                            */  
/*  xi_description - Description of the device                              */  
/*  xi_device - The device FD                                               */  
/*  xi_state - Device state: off | on                                       */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*  xo_device_id - The output device ID                                     */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                                */
/*     LOGGER_OK  - Success indication.                                  */
/*      LOGGER_GEN_ERROR - Error indication.                                  */
/*                                                                          */
/*                                                                          */
/*  --> next_device                                                         */
/*                     previous_section <--                                 */
/*                                        |                                 */
/*                                        |                                 */
/*  --> sections_state --> next_section -->                                 */
/*                    |                                                     */
/*                    |                                                     */
/*                    | previous_group <--                                  */
/*                    |                  |                                  */
/*                    |                  |                                  */
/*                    --> groups_state   --> next_group                     */
/*                                       |                                  */
/*                                       |  previous_item <--               */
/*                                       |                  |               */
/*                                       |                  |               */
/*                                       --> items_state    --> next_item   */
/*                                                                          */
/****************************************************************************/
static LOGGER_ERROR f_logger_allocate_device(char *xi_name, long xi_device, unsigned long xi_type, long xi_state, unsigned long *xo_device_ptr)
{
	LOGGER_DEVICE *device_ptr;

    /* Allocate area for the control structure */
    device_ptr = (LOGGER_DEVICE *) logger_alloc(ALLOCATED_DEVICE_LENGTH);
    if (device_ptr == NULL )
    {
        return LOGGER_GEN_ERROR;
    }
    /* Set the device control structure */
    device_ptr->magic = LOGGER_DEVICE_MAGIC;
    strcpy(device_ptr->name, xi_name);
    device_ptr->device = xi_device;
    device_ptr->type = xi_type;
    device_ptr->state = xi_state;
    device_ptr->next_device_ptr = NULL;
    device_ptr->ram_buffer_ptr = NULL;
    device_ptr->oldest_ram_buffer_ptr = NULL;
    device_ptr->ram_buffer_size = 0;
    device_ptr->ram_buffer_mode = LOGGER_BUFFER_MODE_NORMAL;
    device_ptr->device_ram_state = CE_FALSE;
    device_ptr->ram_wrapped = CE_FALSE;
    device_ptr->has_trigger = CE_FALSE;
    device_ptr->trigger_state = LOGGER_TRIGGER_ACTION_NONE;
    device_ptr->has_ram_buffer = CE_FALSE;
    device_ptr->oldest_ram_buffer_ptr = device_ptr->ram_buffer_ptr;
    device_ptr->ram_buffer_write_ptr = device_ptr-> ram_buffer_ptr;

    *xo_device_ptr = (unsigned long ) device_ptr;

    return LOGGER_OK;
}

static LOGGER_ERROR f_logger_define_device(long xi_device, unsigned long xi_type, long xi_object_state, unsigned long *xo_device_state_ptr)
{
    LOGGER_DEVICE_STATE *device_state_ptr;

    /* Allocate area for the control structure */
    device_state_ptr = (LOGGER_DEVICE_STATE *) logger_alloc(ALLOCATED_DEVICE_STATE_LENGTH);
    if (device_state_ptr == NULL )
    {
        return LOGGER_GEN_ERROR;
    }
    /* Set the device control structure */
    device_state_ptr->magic = LOGGER_DEVICE_STATE_MAGIC;
    device_state_ptr->next_device_ptr = NULL;
    device_state_ptr->device = xi_device;
    device_state_ptr->type = xi_type;
    device_state_ptr->object_state = xi_object_state;
    *xo_device_state_ptr = (unsigned long ) device_state_ptr;

    return LOGGER_OK;
}

static void p_chain_a_device(LOGGER_DEVICE_STATE *xi_this_device_ptr, LOGGER_DEVICE_STATE *xi_device_ptr)
{
    LOGGER_DEVICE_STATE *this_device_ptr = xi_this_device_ptr;

    while(this_device_ptr->next_device_ptr != NULL )
    {
        this_device_ptr = this_device_ptr->next_device_ptr;
    }
    /* Link to the end of the list */
    this_device_ptr->next_device_ptr = xi_device_ptr;
}

LOGGER_ERROR logger_unset_item(LOGGER_ITEM_ID xi_item_id) 
{
    LOGGER_ITEM *this_ptr;
    LOGGER_ITEM *prev_ptr;
    LOGGER_GROUP *group_ptr;
    LOGGER_ITEM *current_ptr = (LOGGER_ITEM *)xi_item_id;

    if (current_ptr != NULL )
    {    
        if (current_ptr->magic != LOGGER_ITEM_MAGIC) return (LOGGER_OBJECT_NOT_FOUND);
        group_ptr = current_ptr->parent_group_ptr;
        /* Find the item in the list */
        this_ptr = group_ptr->items_ptr;
        prev_ptr = this_ptr;
        while ((this_ptr != current_ptr) && (this_ptr != NULL))
        {  
            prev_ptr = this_ptr;
            this_ptr = this_ptr->next_item_ptr;
        }
        if (this_ptr != NULL)
        {
            prev_ptr->next_item_ptr = this_ptr->next_item_ptr;
            logger_free(this_ptr);
        }
        if (group_ptr->items_ptr == current_ptr )
        {
            group_ptr->items_ptr = NULL;
        }
    }
    return LOGGER_OK;
}
LOGGER_ERROR logger_unset_group(LOGGER_GROUP_ID xi_group_id) 
{
    LOGGER_GROUP *this_ptr;
    LOGGER_GROUP *prev_ptr;
    LOGGER_SECTION *section_ptr;
    LOGGER_ITEM *this_item_ptr;
    LOGGER_ITEM *prev_item_ptr;
    LOGGER_GROUP *current_ptr = (LOGGER_GROUP *) xi_group_id;
    if (current_ptr != NULL )
    {   
        if (current_ptr->magic != LOGGER_GROUP_MAGIC) return (LOGGER_OBJECT_NOT_FOUND);
        /* clear all items of the group */
        this_item_ptr = current_ptr->items_ptr;
        while(this_item_ptr != NULL )
        {
            prev_item_ptr = this_item_ptr;
            this_item_ptr = this_item_ptr->next_item_ptr;
            logger_free(prev_item_ptr);
        }
        section_ptr = current_ptr->parent_section_ptr;
        /* Find the group in the list */
        this_ptr = section_ptr->groups_ptr;
        prev_ptr = this_ptr;
        while ((this_ptr != current_ptr) && (this_ptr != NULL))
        {  
            prev_ptr = this_ptr;
            this_ptr = this_ptr->next_group_ptr;
        }
        if (this_ptr != NULL)
        {
            prev_ptr->next_group_ptr = this_ptr->next_group_ptr;
            logger_free(this_ptr);
        }
        if (section_ptr->groups_ptr == current_ptr )
        {
            section_ptr->groups_ptr = NULL;
        }
    }
    return LOGGER_OK;
}
LOGGER_ERROR logger_unset_section(LOGGER_SECTION_ID xi_section_id) 
{
    LOGGER_SECTION *this_ptr;
    LOGGER_SECTION *prev_ptr;
    LOGGER_LOG *log_ptr;
    LOGGER_ITEM *this_item_ptr;
    LOGGER_ITEM *prev_item_ptr;
    LOGGER_GROUP *this_group_ptr;
    LOGGER_GROUP *prev_group_ptr;
    LOGGER_SECTION *current_ptr = (LOGGER_SECTION *) xi_section_id;
    if (current_ptr != NULL)
    {   
        if (current_ptr->magic != LOGGER_SECTION_MAGIC) 
            return LOGGER_OBJECT_NOT_FOUND;
        /* clear all groups of the section */
        this_group_ptr = current_ptr->groups_ptr;
        while(this_group_ptr != NULL )
        {
            prev_group_ptr = this_group_ptr;
            /* clear all items of the group */
            this_item_ptr = this_group_ptr->items_ptr;
            while(this_item_ptr != NULL )
            {
                prev_item_ptr = this_item_ptr;
                this_item_ptr = this_item_ptr->next_item_ptr;
                logger_free(prev_item_ptr);
            }
            this_group_ptr = this_group_ptr->next_group_ptr;
            logger_free(prev_group_ptr);
        }

        log_ptr = current_ptr->parent_log_ptr;
        /* Find the group in the list */
        this_ptr = log_ptr->sections_ptr;
        prev_ptr = this_ptr;
        while ((this_ptr != current_ptr) && (this_ptr != NULL))
        {  
            prev_ptr = this_ptr;
            this_ptr = this_ptr->next_section_ptr;
        }
        if (this_ptr != NULL)
        {
            prev_ptr->next_section_ptr = this_ptr->next_section_ptr;
            logger_free(this_ptr);
        }
        if (log_ptr->sections_ptr == current_ptr )
        {
            log_ptr->sections_ptr = NULL;
        }
    }
    return LOGGER_OK;
}

#if defined(EXPORT_LOGGER_SYMBOLS)
EXPORT_SYMBOL(logger_initialize);
EXPORT_SYMBOL(fi_get_logger_device);
EXPORT_SYMBOL(logger_terminate);
EXPORT_SYMBOL(logger_set_device);
EXPORT_SYMBOL(logger_clear_device);
EXPORT_SYMBOL(logger_set_time_base);
EXPORT_SYMBOL(logger_set_day_base);
EXPORT_SYMBOL(logger_set_format);
EXPORT_SYMBOL(logger_set_notification);
EXPORT_SYMBOL(logger_set_section);
EXPORT_SYMBOL(logger_set_group);
EXPORT_SYMBOL(logger_set_item);
EXPORT_SYMBOL(logger_set_item_throttle);
EXPORT_SYMBOL(logger_set_section_device);
EXPORT_SYMBOL(logger_set_group_device);
EXPORT_SYMBOL(logger_set_item_device);
EXPORT_SYMBOL(logger_get_item_state);
EXPORT_SYMBOL(pi_logger_log);
EXPORT_SYMBOL(logger_on);
EXPORT_SYMBOL(logger_off);
EXPORT_SYMBOL(logger_off_all);
EXPORT_SYMBOL(logger_log_to_file);
EXPORT_SYMBOL(logger_close_log_file);
EXPORT_SYMBOL(logger_print_ram);
EXPORT_SYMBOL(logger_set_log_parameters);
EXPORT_SYMBOL(logger_set_trigger_action);
EXPORT_SYMBOL(logger_create_queue);
EXPORT_SYMBOL(fi_linux_logger_init); 
EXPORT_SYMBOL(fi_get_log_ptr);
EXPORT_SYMBOL(logger_get_version);
#endif

/******************************************************************/
/* All vxworks_emulate functions */
/******************************************************************/

#define PDEBUGG(fmt, args...)
unsigned long start_jiffies;
bdmf_queue_t queue, *queue_p;

static char log_buffer[LOGGER_BUFFER_SIZE+200];

unsigned long tickGet(void)
{
	unsigned long localj;

	localj = jiffies;
	if ((localj > start_jiffies) && (start_jiffies > 0xF000000))
		localj = localj - start_jiffies;
	else
		start_jiffies = 0;

	return localj;
}

void ctime_r(time_t *tm, char *xo_buffer, size_t *size)
{
	long t, h, m, s, ms;

	t = (unsigned long)*tm;
	h = t / 360000;
	t %= 360000;
	m = t / 6000;
	t %= 6000;
	s = t / 100;
	ms = t % 100;
	sprintf(xo_buffer, "%d:%2d:%2d:%d\n", (int)h, (int)m, (int)s, (int)ms);
}

time_t time(void)
{
	struct timeval tv;
	do_gettimeofday(&tv);
	return (time_t)((tv.tv_sec *100) + (tv.tv_usec / 10000));
	;
}
long fdprintf(long fd, const char *format, ...)
{
	va_list args;
	int count;

	va_start(args, format);
	count = vsprintf(log_buffer, format, args);
	va_end(args);

	if (count > LOGGER_BUFFER_SIZE)
	{
		PDEBUGG("\n(%s) log message to big\n%s\n", __FUNCTION__, log_buffer);
	}
	else
	{
		logger_put_in_queue(log_buffer);
	}

	return count;
}
#if defined(EXPORT_LOGGER_SYMBOLS)
EXPORT_SYMBOL(fdprintf); 
#endif


char* find_idx(const char *p, long ch)
{
	char *pp = (char *)p;
	char *save = NULL;

	if (pp == NULL)
		return save;

	while (1)
	{
		if (*pp == '\0')
			break;

		if (*pp == ch)
			save = pp;
		pp++;
	}

	return save;
}

long logger_create_queue(void)
{
	int	error;
	
	
	error = logger_queue_create(&queue, NR_LOGGER_BUFFERS, 
								  LOGGER_BUFFER_SIZE);
	if (error != LOGGER_SUCCESS)
	{
		PDEBUGG("(%s) Error: queue create failed : %d %d\n", __FUNCTION__, NR_LOGGER_BUFFERS, LOGGER_BUFFER_SIZE); 
		return -1;
	}
        queue_p = &queue;

	start_jiffies = jiffies;

	PDEBUGG("(%s) logger_create_queue: Create queue\n", __FUNCTION__);
	
	return 0;
}

void logger_put_in_queue(char *buffer)
{
	int		error;
	
	error = logger_queue_send (&queue, buffer, strlen(buffer));

	if (error != LOGGER_SUCCESS) 
	{
		PDEBUGG("(%s) Error: sending message (error: %d)\n",__FUNCTION__, (int)error);
	}
	else
	{
		PDEBUGG("(%s) Sent message\n",__FUNCTION__);
	}
}

int logger_get_from_queue(void *buffer, unsigned long *nBytes)
{
	int		error;

	error = logger_queue_receive_not_blocking(&queue, buffer, nBytes);

	if (error != LOGGER_SUCCESS)
	{
		PDEBUGG("(%s) Error: can't receive message (queue Id: %d, error: %d)\n",
				__FUNCTION__, &queue, (int)error);
		return 1;
	}
	else
	{
		PDEBUGG("(%s) received one message\n", __FUNCTION__);
	}

	return 0; 
}


