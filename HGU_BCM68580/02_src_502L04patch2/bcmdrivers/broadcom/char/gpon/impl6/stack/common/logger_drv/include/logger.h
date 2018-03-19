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

#ifndef __INClogger
#define __INClogger

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************/
/*                                                                          */
/* Software unit logger.                                                    */
/*                                                                          */
/* Title:                                                                   */
/*                                                                          */
/*   Logger mechanism interface.                                            */
/*                                                                          */
/* Abstract:                                                                */
/*                                                                          */
/*   The logger is a run-time multi-level logging mechanism.                */
/*                                                                          */
/* Requirements allocated to this software unit:                            */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/* Other software units include files                                       */
/*                                                                          */
/****************************************************************************/

#define LOGGER_SUCCESS 0
#define LOGGER_ERROR_FUNC_NOT_IMPLEMENTED 1

#define CE_FALSE 0
#define CE_TRUE	1

typedef struct
{
    void* (*log_alloc)(long size);
    void (*log_free)(void *buffer); 
    long (*log_queue_create)(bdmf_queue_t *queue, long number_of_messages, long max_message_length); 
    long (*log_queue_send)(bdmf_queue_t *queue, char *buffer, long length); 
    long (*log_queue_receive_not_blocking)(bdmf_queue_t *queue, char *buffer, long *length); 
    void (*log_get_task_name)(char *buffer); 
} LOGGER_CALLBACKS; 

/* From vxworks_emulate.h */
#define LOGGER_BUFFER_SIZE 1024
#define NR_LOGGER_BUFFERS   300
long logger_create_queue(void);
int logger_get_from_queue(void *buffer, unsigned long *nBytes);
void logger_put_in_queue(char *buffer);

/*
 * Macros to help debugging
 */
#undef LOGGER_DRV_DEBUG
#ifdef LOGGER_DRV_DEBUG
#define LDEBUG(fmt, args...) printk("LOGGER_DRV: " fmt, ## args)
#else
#define LDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef  PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */


/****************************************************************************/
/*                                                                          */
/* Definitions                                                              */
/*                                                                          */
/****************************************************************************/

/* Error codes */
typedef enum
{
    /* General error indication */
    LOGGER_GEN_ERROR = -7000,
    
    /* Object not found */
    LOGGER_OBJECT_NOT_FOUND = -7001,

    /* Successful indication */
    LOGGER_OK = 0
} 
LOGGER_ERROR;

/* Various IDs */
typedef unsigned long LOGGER_LOG_ID;
typedef unsigned long LOGGER_SECTION_ID;
typedef unsigned long LOGGER_GROUP_ID;
typedef unsigned long LOGGER_ITEM_ID;
typedef unsigned long LOGGER_DEVICE_ID;

/* notification */
typedef enum
{
    /* None */
    LOGGER_NOTIFICATION_NONE,

    /* Watermark */
    LOGGER_NOTIFICATION_WATERMARK,

    /* Full */
    LOGGER_NOTIFICATION_FULL,

    /* Wrap */
    LOGGER_NOTIFICATION_WRAP
}
LOGGER_NOTIFICATION;

/* Log time-base type */
typedef enum
{
    /* None */
    LOGGER_TIME_BASE_NONE,

    /* Tick */
    LOGGER_TIME_BASE_TICK,

    /* Time */
    LOGGER_TIME_BASE_TIME,

    /* Time based microsec */
    LOGGER_TIME_BASE_MICROSEC,

    /* User-defined */
    LOGGER_TIME_BASE_USER_DEFINED
}
LOGGER_TIME_BASE_TYPE;

/* Termination mode */
typedef enum
{
    /* Graceful */
    LOGGER_TERMINATION_MODE_GRACEFUL,

    /* Brutal */
    LOGGER_TERMINATION_MODE_BRUTAL
}
LOGGER_TERMINATION_MODE;

/* Clear trigger parameters */
typedef enum
{
    /* State */
    LOGGER_TRIGGER_CLEAR_STATE,

    /* Mode */
    LOGGER_TRIGGER_CLEAR_MODE
}
LOGGER_TRIGGER_MODES;

/* Device logger modes */
typedef enum
{
    /* Normal mode - on, send to device */
    LOGGER_NORMAL_MODE,

    /* Ram mode - put text in RAM */
    LOGGER_RAM_MODE,

    /* Trigger mode - send to device or put in ram when first trigger 'on' accounted */
    /* and suspend it when the first 'off' accounted */
    LOGGER_TRIGGER_MODE,
}
LOGGER_MODES;

/* Buffer mode */
typedef enum
{
    /* Normal */
    LOGGER_BUFFER_MODE_NORMAL,

    /* Cyclic */
    LOGGER_BUFFER_MODE_CYCLIC
}
LOGGER_BUFFER_MODE;

/* Item types */
typedef enum
{
    /* Normal */
    LOGGER_ITEM_TYPE_NORMAL,

    /* Format */
    LOGGER_ITEM_TYPE_FORMAT,
}
LOGGER_ITEM_TYPE;

typedef enum
{
    /* No valid log */
    CE_LOG_NULL,
     
    /* external file already exists */
    CE_FILE_EXIST,

    /* file was opened successfully */
    CE_FILE_OPEN 
}
LOGGER_EXT_FILE_STATUS;

typedef enum
{
    /* change state*/
    CE_CHANGE_STATE ,

    /* change writing to ram status */
    CE_CHANGE_RAM ,

    /* change state and writing to ram status */
    CE_CHANGE_BOTH 
}
LOGGER_CHANGE_REQUEST;

/* Item printing attributes */
typedef enum
{
    LOGGER_ATTRIBUTE_NORMAL        = 0x00,
    LOGGER_ATTRIBUTE_BOLD          = 0x01,
    LOGGER_ATTRIBUTE_UNDERLINE     = 0x02,
    LOGGER_ATTRIBUTE_BLINK         = 0x04,
    LOGGER_ATTRIBUTE_REVERSE_VIDEO = 0x08,
    LOGGER_ATTRIBUTE_BEEP          = 0x10,
    LOGGER_ATTRIBUTE_DOUBLE_HEIGHT = 0x20,
    LOGGER_ATTRIBUTE_DOUBLE_WIDTH  = 0x40
}
LOGGER_ATTRIBUTES;

/* VT102 terminal escape sequences */
#define LOGGER_NORMAL        "\033[0m\033#5"
#define LOGGER_BOLD          "\033[1m"
#define LOGGER_UNDERLINE     "\033[4m"
#define LOGGER_BLINK         "\033[5m"
#define LOGGER_REVERSE_VIDEO "\033[7m"
#define LOGGER_BEEP          "\007"

/* Trigger actions */
typedef enum
{
    /* None */
    LOGGER_TRIGGER_ACTION_NONE,

    /* Srart */
    LOGGER_TRIGGER_ACTION_START,

    /* Stop */
    LOGGER_TRIGGER_ACTION_STOP
}
LOGGER_TRIGGER_ACTION;


/* Notification callback */
typedef void (* LOGGER_NOTIFICATION_CALLBACK)(LOGGER_NOTIFICATION xi_notification, unsigned long xi_parameter);

/* Timeofday callback */
typedef void (* LOGGER_TIMEOFDAY_CALLBACK)(unsigned char *xo_buffer);

/* Timeofday formatting callback */
typedef void (* LOGGER_TIMEOFDAY_FORMATTING_CALLBACK)(unsigned char *xi_buffer, char *xo_buffer); 

/* Timestamping callback */
typedef uint64_t (* LOGGER_TIMESTAMPING_CALLBACK)( void);

/* Timestamp formatting callback */
typedef void (* LOGGER_TIMESTAMP_FORMATTING_CALLBACK)(uint64_t xi_timestamp, char *xo_buffer);

/* Log information formatting callback */
typedef void (* LOGGER_LOG_INFORMATION_FORMATTING_CALLBACK)(char *xi_log, char *xi_timeofday, char *xi_timestamp, char * xi_file, unsigned long xi_line, LOGGER_SECTION_ID xi_section_id, LOGGER_GROUP_ID xi_group_id, LOGGER_ITEM_ID xi_item_id, char * xi_context, char * xo_log_information);

/* Log item formatting callback */
typedef void (* LOGGER_LOG_ITEM_FORMATTING_CALLBACK)(char *xi_log_information, char *xi_item, char *xo_log_item);

/* Binary item formatting callback */
typedef void (* LOGGER_BINARY_ITEM_FORMATTING_CALLBACK)(unsigned long xi_parameter, char *xi_binary_data, char *xo_binary);

#define LOGGER_NAME_LENGTH           (64)
#define LOGGER_DESCRIPTION_LENGTH    (64)
#define LOGGER_FORMAT_LENGTH         (256)
#define LOGGER_LOG_INFORMATION_LENGTH (512)
#define LOGGER_BINARY_LENGTH         (512)


/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_initialize.                                                     */
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
/*  xi_time_base - Log time-base type: none | tick | time | user-defined    */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_initialize(char *xi_name, char *xi_description, 
    unsigned long xi_priority, unsigned long xi_stack_size, 
    unsigned long xi_queue_size, LOGGER_TIME_BASE_TYPE xi_time_base, 
    LOGGER_LOG_ID *xo_log_id, LOGGER_CALLBACKS *xi_logger_callbacks, long console); 

LOGGER_ERROR fi_linux_logger_init(char *xi_name, char *xi_description, 
    unsigned long xi_priority, unsigned long xi_stack_size, unsigned long xi_queue_size, 
    LOGGER_TIME_BASE_TYPE xi_time_base, unsigned long *xo_log_id, 
    LOGGER_CALLBACKS *xi_logger_callbacks, long console);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_terminate.                                                      */  
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
/*    LOGGER_TERMINATION_MODE_GRACEFUL                                      */
/*    LOGGER_TERMINATION_MODE_BRUTAL                                        */
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_terminate(LOGGER_LOG_ID xi_log_id, unsigned long xi_priority, LOGGER_TERMINATION_MODE xi_mode);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_device.                                                     */  
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
    char *xi_description, long xi_device, unsigned long xi_type, 
    long xi_state, LOGGER_DEVICE_ID *xo_device_id);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_clear_device                                                    */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - Remove device from internal data structure of the logger      */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The function removes device from internal data structure of the logger */  
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_device - The device FD                                               */  
/*                                                                          */
/* Output:                                                                  */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_clear_device(unsigned long xi_device_id);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_time_base.                                                  */  
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
/*  xi_timestamping_callback - Polonger to timestamping callback            */
/*  xi_timestamp_formatting_callback - Polonger to timestamp formatting     */
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
    LOGGER_TIMESTAMP_FORMATTING_CALLBACK xi_timestamp_formatting_callback);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_day_base.                                                   */  
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
/*  xi_timeofday_callback - Polonger to time of day callback                */
/*  xi_timeofday_formatting_callback - Polonger to time of day formatting   */
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
    LOGGER_TIMEOFDAY_FORMATTING_CALLBACK xi_timeofday_formatting_callback);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_format.                                                     */  
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
    LOGGER_LOG_ITEM_FORMATTING_CALLBACK xi_log_item_formatting_callback);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_notification.                                               */  
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
    LOGGER_NOTIFICATION_CALLBACK xi_notification_callback);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_section.                                                    */  
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
    char *xi_description, long xi_state, LOGGER_SECTION_ID *xo_section_id);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_group.                                                      */  
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
    char *xi_description, long xi_state, LOGGER_GROUP_ID *xo_group_id);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_item.                                                       */  
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
    LOGGER_ITEM_ID *xo_item_id);

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
LOGGER_ERROR logger_set_item_throttle(LOGGER_ITEM_ID xi_item_id, 
    unsigned long xi_throttle_threshold);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_section_device.                                             */  
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
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_section_device(LOGGER_SECTION_ID xi_section_id, 
    char *xi_name, char *xi_description, long xi_device, long xi_state, 
    unsigned long xi_throttle_depth, unsigned long xi_throttle_threshold, 
    long xi_flush, long xi_close);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_group_device.                                               */  
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
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_group_device(LOGGER_GROUP_ID xi_group_id, char *xi_name, 
    char *xi_description, long xi_device, long xi_state, 
    unsigned long xi_throttle_depth, unsigned long xi_throttle_threshold, 
    long xi_flush, long xi_close);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_set_item_device.                                                */  
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
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_set_item_device(LOGGER_ITEM_ID xi_item_id, char *xi_name, 
    char *xi_description, long xi_device, long xi_state, 
    unsigned long xi_throttle_depth, unsigned long xi_throttle_threshold, 
    long xi_flush, long xi_close);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_get_item_state.                                                 */  
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
/*   long  - Item state.                                                    */
/*     CE_TRUE  - Item os on.                                               */
/*     CE_FALSE - Item is off.                                              */
/*                                                                          */
/****************************************************************************/
long logger_get_item_state(LOGGER_ITEM_ID xi_item_id);

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
void pi_logger_log(char * xi_file, unsigned long xi_line, const char *xi_function, 
    LOGGER_ITEM_ID xi_item_id, ...);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_on                                                              */  
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
/*  xi_logger_ptr - current logger instance                                 */  
/*  xi_object - current task context                                        */  
/*  xi_in_ram - which field goes 'on' : state, ram or both                  */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*     LOGGER_OBECT_NOT_FOUND - Object is not in the data structure         */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_on(unsigned long xi_device_id, unsigned long xi_log_ptr, 
    char *xi_object, LOGGER_CHANGE_REQUEST xi_in_ram);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_off                                                             */  
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
/*  xi_logger_ptr - current logger instance                                 */  
/*  xi_object - current task context                                        */    
/*  xi_in_ram - which field goes 'off' : state, ram or both                 */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*     LOGGER_OBECT_NOT_FOUND - Object is not in the data structure         */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_off(unsigned long xi_device_id, unsigned long xi_log_ptr, 
    char *xi_object, LOGGER_CHANGE_REQUEST xi_in_ram);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_off_all                                                         */  
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
/*  xi_logger_ptr - current logger instance                                 */  
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*     LOGGER_OBECT_NOT_FOUND - Object is not in the data structure         */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_off_all(unsigned long xi_device_id, unsigned long xi_log_ptr);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_log_to_file                                                     */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Activate the logger to an external file                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Activate the logger(as "lh" command)but the output is into             */
/*   an external file                                                       */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_fd_log_file - file descriptor of the file                            */  
/*  xi_file_name - file name                                                */  
/*  xi_logger_ptr - current logger instance                                 */  
/*  xi_object - the current task context                                    */
/*                                                                          */  
/* Output:                                                                  */
/*                                                                          */
/*  LOGGER_EXT_FILE_STATUS -                                                */
/*  CE_LOG_NULL                                                             */
/*  CE_FILE_EXIST                                                           */
/*  CE_FILE_OPEN                                                            */
/*                                                                          */
/****************************************************************************/
LOGGER_EXT_FILE_STATUS logger_log_to_file(unsigned long xi_fd_log_file, 
    unsigned long xi_log_ptr, char *xi_object, char *xi_file_name);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger_close_log_file                                                  */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Activate the logger to an external file                                */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   Close log file                                                         */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_object - the current task context                                    */
/*  xi_logger_ptr - current logger instance                                 */  
/*  xo_filename - closed file name                                          */
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/*   long - Error code.                                                     */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*     LOGGER_OBECT_NOT_FOUND - Object is not in the data structure         */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_close_log_file(unsigned long xi_deviceid, 
    unsigned long xi_log_ptr, char *xo_filename);

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
LOGGER_ERROR logger_get_object_list(unsigned long xi_log_ptr, char *xi_object, 
    char *object_list);

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
/*  xi_logger_ptr - current logger instance                                 */  
/*  xi_flush  - clear the ram buffer after printing                         */
/*                                                                          */  
/* Output:                                                                  */
/*   LOGGER_ERROR - Error code.                                             */
/*     LOGGER_OK  - Success indication.                                     */
/*      LOGGER_GEN_ERROR - Error indication.                                */
/*                                                                          */
/****************************************************************************/
LOGGER_ERROR logger_print_ram(unsigned long xi_device, unsigned long xi_device_type, 
    unsigned long xi_log_ptr, long  xi_flush);

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
/*  xi_logger_ptr - current logger instance                                 */  
/*  xi_action - CE_ENABLE or CE_DISABLE                                     */
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
    unsigned long xi_log_ptr, long  xi_action, LOGGER_MODES xi_which_parameter, 
    unsigned long xi_parameter_value, unsigned long xi_ram_size);

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
/*  xi_logger_ptr - current logger instance                                 */  
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
    unsigned long xi_count, char * xi_object);

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
unsigned long fi_get_log_ptr(char * xi_current_path);

LOGGER_ERROR logger_unset_item(LOGGER_ITEM_ID xi_item_id); 
LOGGER_ERROR logger_unset_group(LOGGER_GROUP_ID xi_group_id);
LOGGER_ERROR logger_unset_section(LOGGER_SECTION_ID xi_section_id);

/****************************************************************************/
/*                                                                          */
/* Name:                                                                    */
/*                                                                          */
/*   logger.                                                                */  
/*                                                                          */  
/* Title:                                                                   */  
/*                                                                          */  
/*   Logger - The logging macro.                                            */  
/*                                                                          */  
/* Abstract:                                                                */  
/*                                                                          */  
/*   The macro calls the logging function and passes the file and line.     */
/*                                                                          */  
/* Input:                                                                   */  
/*                                                                          */  
/*  xi_item_id - The item ID                                                */  
/*  ... - Log arguments                                                     */
/*                                                                          */
/* Output:                                                                  */
/*                                                                          */
/****************************************************************************/
#define logger(item_id, parameters...)pi_logger_log(__FILE__, __LINE__, item_id, ##parameters)

int logger_drv_init(void);
void logger_drv_exit(void);

#ifdef __cplusplus
}
#endif
#endif

