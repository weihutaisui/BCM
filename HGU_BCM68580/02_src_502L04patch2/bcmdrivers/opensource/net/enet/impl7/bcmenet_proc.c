/*
   <:copyright-BRCM:2015:DUAL/GPL:standard
   
      Copyright (c) 2015 Broadcom 
      All Rights Reserved
   
   Unless you and Broadcom execute a separate written software license
   agreement governing use of this software, this software is licensed
   to you under the terms of the GNU General Public License version 2
   (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
   with the following added to such license:
   
      As a special exception, the copyright holders of this software give
      you permission to link this software with independent modules, and
      to copy and distribute the resulting executable under terms of your
      choice, provided that you also meet, for each linked independent
      module, the terms and conditions of the license of that module.
      An independent module is a module which is not derived from this
      software.  The special exception does not apply to any modifications
      of the software.
   
   Not withstanding the above, under no circumstances may you combine
   this software in any way with any other Broadcom software provided
   under a license other than the GPL, without Broadcom's express prior
   written consent.
   
   :>
 */

/*
 *  Created on: Nov/2015
 *      Author: dima.mamut@broadcom.com
 */
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "proc_cmd.h"
#include "bcmenet_proc.h"

#define PROC_DIR                    "driver/debug_mode"
#define DEBUG_MODE_PROC_FILE        "debug_mode"

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *cmd_proc_file;

#ifdef CONFIG_ARM64
uint64_t g_debug_mode_pckt_rx;
#else
uint32_t g_debug_mode_pckt_rx;
#endif

uint32_t g_debug_mode;
struct timeval g_start_time;
struct timeval g_end_time;

static struct proc_cmd_ops debug_mode_command_entries[] = 
{
    { .name = "set_debug_mode",             .do_command	= debug_mode_set },
    { .name = "get_pps_measure",            .do_command	= debug_mode_get_pps},
};

static struct proc_cmd_table debug_mode_command_table = 
{
    .module_name = "debug_mode_masters",
    .size = sizeof(debug_mode_command_entries) / sizeof(debug_mode_command_entries[0]),
    .ops = debug_mode_command_entries
};


int debug_mode_set(int argc, char *argv[])
{
    char *usage = "Usage: set_debug_mode <[<mode 0|1>]";
    unsigned int debug_mode = 0xffffffff;
    int rc = 0;

    if (argc < 2)
        goto wrong_params;

    if (kstrtos32(argv[1], 10, &debug_mode))
        goto wrong_params;


    if (debug_mode > 1)
        goto wrong_params; 
    
    
    g_debug_mode = debug_mode;

    printk("\x1b[32m DEBUG_MODE[%d]: \x1b[0m\n",g_debug_mode);

    return rc;


wrong_params:
    pr_info("%s\n", usage);
    return rc;

}

int debug_mode_get_pps(int argc, char *argv[])
{
    char *usage = "Usage: get_pps";
    int rc = 0;
#ifdef CONFIG_ARM64
    uint64_t time_diff;
#else
    uint32_t time_diff;
#endif
    
    if (argc < 1)
        goto wrong_params;
    
    do_gettimeofday(&g_end_time);
    
    time_diff = (g_end_time.tv_sec - g_start_time.tv_sec) + (g_end_time.tv_usec - g_start_time.tv_usec) / 1000000;

#ifdef CONFIG_ARM64
    printk("\x1b[32m DEBUG_MODE: pckt_rx[%lld] end_time[%ldsec] start_time[%ldsec] PPS[%lld]\x1b[0m\n", 
#else
    printk("\x1b[32m DEBUG_MODE: pckt_rx[%d] end_time[%ldsec] start_time[%ldsec] PPS[%d]\x1b[0m\n", 
#endif
           g_debug_mode_pckt_rx,
           g_end_time.tv_sec,
           g_start_time.tv_sec,
           g_debug_mode_pckt_rx / time_diff);

    memcpy(&g_start_time, &g_end_time, sizeof(struct timespec)); 
    g_debug_mode_pckt_rx = 0;
    
    return rc;                      


wrong_params:
    pr_info("%s\n", usage);
    return rc;
}

int create_debug_mode_proc(void)
{
    int rc = 0;

    proc_dir = proc_mkdir(PROC_DIR, NULL);
    if (!proc_dir) 
    {
        pr_err("Failed to create PROC directory %s.\n",
            PROC_DIR);
        goto error;
    }


    cmd_proc_file = proc_create_cmd(DEBUG_MODE_PROC_FILE, proc_dir, &debug_mode_command_table);
    if (!cmd_proc_file) 
    {
        pr_err("Failed to create %s\n", DEBUG_MODE_PROC_FILE);
        goto error;
    }

    g_debug_mode = 0;
    g_debug_mode_pckt_rx = 0;
    do_gettimeofday(&g_start_time);

    return rc;

error:
    if (cmd_proc_file) 
    {
        remove_proc_entry(DEBUG_MODE_PROC_FILE, proc_dir);
        cmd_proc_file = NULL;
    }	
    if (proc_dir)
    {
        remove_proc_entry(PROC_DIR, NULL);
        proc_dir = NULL;
    }
    
    return -1;	
}


