/******************************************************************************
 *
 * <:copyright-BRCM:2017:proprietary:standard
 * 
 *    Copyright (c) 2017 Broadcom 
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
 *
 *****************************************************************************/
#ifndef __DPICTL_API_H
#define __DPICTL_API_H

#include <stdint.h>
#include <bcmdpi.h>

#define DPICTL_TUPLE_LENGTH     46

struct dc_traffic {
   unsigned long      pkts;
   unsigned long long bytes;
};

struct dc_app {
   /* contains app id, appcat id, and behaviour id (see macros in bcmdpi.h) */
   uint32_t app_id;
};

struct dc_app_stat {
   struct dc_app     app;
   struct dc_traffic us;
   struct dc_traffic ds;
};

struct dc_dev {
   uint32_t dev_id;
   uint8_t  mac[6];
};

struct dc_dev_stat {
   struct dc_dev     dev;
   struct dc_traffic us;
   struct dc_traffic ds;
};

struct dc_appinst {
   struct dc_app     app;
   struct dc_dev     dev;
   struct dc_traffic us;
   struct dc_traffic ds;
};

struct dc_flow_info {
   char               tuple[DPICTL_TUPLE_LENGTH];
   unsigned long      pkts;
   unsigned long long bytes;
};

struct dc_flow {
   struct dc_app       app;
   struct dc_dev       dev;
   uint32_t            status;
   struct dc_flow_info ds;
   struct dc_flow_info us;
};

struct dc_qos_appinst {
   struct dc_app app;
   struct dc_dev dev;
   int32_t       ds_queue;
   uint32_t      kbps;
   int32_t       us_queue;
   uint32_t      flow_count;
};

struct dc_qos_appinst_stat {
   struct dc_qos_appinst appinst;
   struct dc_traffic     us;
   struct dc_traffic     ds;
};

struct dc_qos_queue {
   int32_t  id;
   uint32_t valid;
   uint32_t min_kbps;
   uint32_t max_kbps;
};

struct dc_qos_info_dir {
   int num_queues;
   int default_queue;
   int bypass_queue;
   int overall_bw;
};

struct dc_qos_info {
   struct dc_qos_info_dir us;
   struct dc_qos_info_dir ds;
};

struct dc_id_name_map {
   uint32_t  id;
   char     *name;
};

struct dc_url {
   char *url;
};

struct dc_table {
   int   entries;
   char  data[0];
};


/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_update_db
 * Description  : Update signature database
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int dpictl_update_db(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_update_flows
 * Description  : Update flows list
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int dpictl_update_flows(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_update_appinsts
 * Description  : Update appinsts list
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int dpictl_update_appinsts(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_update_qos_info
 * Description  : Update qos & bw info
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int dpictl_update_qos_info(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_update_qos_queues
 * Description  : Update qos queue info
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int dpictl_update_qos_queues(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_update_qos
 * Description  : Update qos appinst info
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int dpictl_update_qos(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_update_urls
 * Description  : Update urls list
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int dpictl_update_urls(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_enable
 * Description  : Enable/disable DPI feature.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int dpictl_enable(int enable);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_status
 * Description  : Show DPI enabled status.
 * Returns      : status on success, -1 on error
 *------------------------------------------------------------------------------
 */
int dpictl_status(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_reset_stats
 * Description  : Reset all appinst stats. Note that this does not reset current
 *                flow stats.
 * Returns      : 0 on success, -1 on error
 *------------------------------------------------------------------------------
 */
int dpictl_reset_stats(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_set_max_pkt
 * Description  : Set max number of packets used for DPI classification.
 * Returns      : 0 - success, non-0 - error
 *------------------------------------------------------------------------------
 */
int dpictl_set_max_pkt(int pkt);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_app_name
 * Description  : Lookup the name of the given app_id
 *                (combination of 8-bit cat_id, 16-bit app_id, and 8-bit beh_id)
 * Returns      : name - success, non-0 - error
 * Note         : string returned must be freed by caller
 *------------------------------------------------------------------------------
 */
char *dpictl_app_name(uint32_t app_id);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_dev_name
 * Description  : Lookup the name of the given device id
 * Returns      : name - success, non-0 - error
 * Note         : string returned must be freed by caller
 *------------------------------------------------------------------------------
 */
char *dpictl_dev_name(uint32_t dev_id);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_appcat_name
 * Description  : Lookup the name of the given category id
 * Returns      : name - success, non-0 - error
 * Note         : string returned must be freed by caller
 *------------------------------------------------------------------------------
 */
char *dpictl_appcat_name(uint32_t cat_id);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_app_db
 * Description  : Get list of all apps in database
 * Returns      : struct - success, NULL - error
 * Note         : dc_table data contains array of struct dc_id_name_map.
 *                struct returned must be freed by caller, along with each
 *                entry's name string.
 *------------------------------------------------------------------------------
 */
struct dc_table *dpictl_get_app_db(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_appcat_db
 * Description  : Get list of all application classes in database
 * Returns      : struct - success, NULL - error
 * Note         : dc_table data contains array of struct dc_id_name_map.
 *                struct returned must be freed by caller, along with each
 *                entry's name string.
 *------------------------------------------------------------------------------
 */
struct dc_table *dpictl_get_appcat_db(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_dev_db
 * Description  : Get list of all devices in database
 * Returns      : struct - success, NULL - error
 * Note         : dc_table data contains array of struct dc_id_name_map.
 *                struct returned must be freed by caller, along with each
 *                entry's name string.
 *------------------------------------------------------------------------------
 */
struct dc_table *dpictl_get_dev_db(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_appinst
 * Description  : Get application instance stats for given appinst.
 * Returns      : struct - success, NULL - error
 * Note         : struct returned must be freed by caller
 *------------------------------------------------------------------------------
 */
struct dc_appinst *dpictl_get_appinst(uint32_t app_id, uint8_t *mac,
                                      int include_flow_stats);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_appinsts
 * Description  : Get table of all appinsts in the system
 * Arguments    : include_flow_stats - 1 to update with current flow stats
 * Returns      : struct - success, NULL - error
 * Note         : dc_table data contains array of struct dc_appinst.
 *                struct returned must be freed by caller.
 *------------------------------------------------------------------------------
 */
struct dc_table *dpictl_get_appinsts(int include_flow_stats);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_dev
 * Description  : Get device statistics for given mac
 * Returns      : struct - success, NULL - error
 * Note         : struct returned must be freed by caller
 *------------------------------------------------------------------------------
 */
struct dc_dev_stat *dpictl_get_dev(uint8_t *mac, int include_flow_stats);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_devs
 * Description  : Get table of all devices in the system
 * Arguments    : include_flow_stats - 1 to update with current flow stats
 * Returns      : struct - success, NULL - error
 * Note         : dc_table data contains array of struct dc_dev_stat.
 *                struct returned must be freed by caller.
 *------------------------------------------------------------------------------
 */
struct dc_table *dpictl_get_devs(int include_flow_stats);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_app
 * Description  : Get application statistics for given app_id
 * Returns      : struct - success, NULL - error
 * Note         : struct returned must be freed by caller
 *------------------------------------------------------------------------------
 */
struct dc_app_stat *dpictl_get_app(uint32_t app_id, int include_flow_stats);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_apps
 * Description  : Get table of all apps in the system
 * Arguments    : include_flow_stats - 1 to update with current flow stats
 * Returns      : struct - success, NULL - error
 * Note         : dc_table data contains array of struct dc_app_stat.
 *                struct returned must be freed by caller.
 *------------------------------------------------------------------------------
 */
struct dc_table *dpictl_get_apps(int include_flow_stats);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_flows
 * Description  : Get table of all flows in the system
 * Returns      : struct - success, NULL - error
 * Note         : dc_table data contains array of struct dc_flow.
 *                struct returned must be freed by caller.
 *------------------------------------------------------------------------------
 */
struct dc_table *dpictl_get_flows(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_urls
 * Description  : Get table of all encountered urls
 * Returns      : struct - success, NULL - error
 * Note         : dc_table data contains array of struct dc_url.
 *                struct returned must be freed by caller, along with each
 *                entry's name string.
 *------------------------------------------------------------------------------
 */
struct dc_table *dpictl_get_urls(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_qos_appinst
 * Description  : Get qos application instance info for given appinst. Includes
 *                statistics from current flows.
 * Returns      : struct - success, NULL - error
 * Note         : struct returned must be freed by caller
 *------------------------------------------------------------------------------
 */
struct dc_qos_appinst_stat *dpictl_get_qos_appinst(uint32_t app_id,
                                                   uint8_t *mac,
                                                   int include_flow_stats);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_qos_appinsts
 * Description  : Get table of all qos appinsts in the system
 * Arguments    : include_flow_stats - 1 to update with current flow stats
 * Returns      : struct - success, NULL - error
 * Note         : dc_table data contains array of struct dc_qos_appinst_stat.
 *                struct returned must be freed by caller.
 *------------------------------------------------------------------------------
 */
struct dc_table *dpictl_get_qos_appinsts(int include_flow_stats);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_qos_info
 * Description  : Get info about qos and bandwidth
 * Returns      : struct - success, NULL - error
 * Note         : struct returned must be freed by caller.
 *------------------------------------------------------------------------------
 */
struct dc_qos_info *dpictl_get_qos_info(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_qos_queues
 * Description  : Get table of all qos queues in the system
 * Returns      : struct - success, NULL - error
 * Note         : dc_table data contains array of struct dc_qos_queue.
 *                struct returned must be freed by caller.
 *------------------------------------------------------------------------------
 */
struct dc_table *dpictl_get_qos_queues(void);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_table
 * Description  : Program a new QoS table
 * Returns      : 0 - success, -1 - error
 *------------------------------------------------------------------------------
 */
int dpictl_table(DpictlTableType_t type, char *file);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_set_avail_bw
 * Description  : Set the available bandwidth for the system
 * Returns      : 0 - success, -1 - error
 *------------------------------------------------------------------------------
 */
int dpictl_set_avail_bw(unsigned int kbps);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_set_default_bw
 * Description  : Set the default bandwidth for the system
 * Returns      : 0 - success, -1 - error
 *------------------------------------------------------------------------------
 */
int dpictl_set_default_bw(unsigned int kbps);

/*
 *------------------------------------------------------------------------------
 * Function Name: dpictl_get_bw_cfg
 * Description  : Retrieve the current bandwidth configs
 * Returns      : struct - success, NULL - error
 * Note         : struct returned must be freed by caller
 *------------------------------------------------------------------------------
 */
DpictlBwConfig_t *dpictl_get_bw_cfg(void);

#endif  /* defined(__DPICTL_API_H) */
