/******************************************************************************
 *
 * Copyright (c) 2009   Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2009:proprietary:standard
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
 ************************************************************************/

/***************************************************************************
 *
 *    Copyright (c) 2008-2009, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Description: MoCA userland daemon
 *
 ***************************************************************************/

#include "moca_os.h"

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>

#if !defined(STANDALONE) && !defined(WIN32)
#include <error.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#endif


#if defined(WIN32)
#include <io.h>

#ifndef O_SYNC
#define O_SYNC 0
#endif

#define open _open
#define read _read
#define write _write
#define close _close


#else
#include <unistd.h>
#endif

#include "mocad.h"


#if defined(DSL_MOCA)
#include <cms_msg.h>
#include <cms_psp.h>
#include <ethswctl_api.h>
#endif

#include <mocalib.h>
#include "mocad-timer.h"
#include "mocaint.h"
#include "mocad-mps.h"

//#include "devctl_moca.h"

#include "mocad-gen.h"
#include "mocad-int.h"


#define RTT_PAYLOAD_LENGTH 200

/* Copied from bmoca.h */
#define MOCA_BAND_HIGHRF   0
#define MOCA_BAND_MIDRF    1
#define MOCA_BAND_WANRF    2 
#define MOCA_BAND_EXT_D    3
#define MOCA_BAND_D_LOW    4
#define MOCA_BAND_D_HIGH   5
#define MOCA_BAND_E        6
#define MOCA_BAND_F        7
#define MOCA_BAND_UNUSED   8  /* This used to be band G.  Do not use */
#define MOCA_BAND_H        9

#define SNR_ELEMENTS       10
#define PACKETRAM_ADDR_REINIT       0x8000
#define MOCAD_FIRMWAREDATA_REINIT_LEN  (96 * 1024)

#define PACKETRAM_M1       0x01234567
#define PACKETRAM_M2       0x89ABCDEF

//#define MOCAD_DEFTRAP_LEN  (8*1024)

#define MAX_STRING_LENGTH 256
#define MAX_RTT_FILES     50

#ifndef MIN
#define MIN(a, b)    (((a) < (b)) ? (a) : (b))
#endif

#ifdef STANDALONE
#include "hw_board.h"
#include "hw_eport.h"
#include "nvram.h"
#include "printf.h"
#define RESTART(x)      do { xprintf("Restarting.  Reason: %d\n",x); MoCAOS_MSleep(2000); BOARD_Reset(); } while (0)       
#else
#define RESTART(x)      ctx->restart=x;
#endif

typedef struct 
{
#if (__BYTE_ORDER == __LITTLE_ENDIAN) && (!defined(__EMU_HOST_20__))
   uint32_t length       : 16; // Length of buffer in bytes (not including the control word)
   uint32_t reserved     : 14; // Reserved
   uint32_t ownership    : 1;  // Indicates ownership of the buffer
   uint32_t last         : 1;  // Indicates that this the last buffer of the RTT passing
#else
   uint32_t last         : 1;  // Indicates that this the last buffer of the RTT passing
   uint32_t ownership    : 1;  // Indicates ownership of the buffer
   uint32_t reserved     : 14; // Reserved
   uint32_t length       : 16; // Length of buffer in bytes (not including the control word)
#endif
   uint32_t rtt_buffer_size;
   uint32_t rtt_last_write_index;
} rtt_control_t;


int mocad_keygen(uint8_t mmk[8], uint8_t pmki[8], const unsigned char *in);


struct mocad_egr_mc_addr_filter
{
   uint32_t valid;
   uint32_t addr_hi;
   uint32_t addr_lo;  
};



MOCA_DATA2 struct mocad_ctx g_ctx;


#define MOCA_REQ_TIMEOUT      2
#define MOCA_STAT_INTERVAL    20
#define MOCAD_PQOS_TIMEOUT    5
#define MOCAD_FMR_TIMEOUT     5
#define MOCAD_MR_TIMEOUT      5
#define MOCAD_PS_CMD_TIMEOUT  5
#define MOCA_MAX_FMR20_NODES  2
#define MOCAD_ACA_TIMEOUT     20

#define MOCA_TIME_EVENT_CORE_UP   0
#define MOCA_TIME_EVENT_CORE_DOWN 1
#define MOCA_TIME_EVENT_LINK_UP   2
#define MOCA_TIME_EVENT_LINK_DOWN 3

#define CPU_CHECK_INTERVAL        5   //every 5 seconds
#define CPU_CHECK_MARGIN_ERR      1   // Allow plus or minus 1 second difference


#define MOCA_INVALID_NODE_ID 0xFF

#define MOCAD_PQOS_VLAN_FILTER_HASH "801:"
#define MOCAD_PQOS_DSCP_FILTER_HASH "802:"

#define MAX(x, y)    (((x) > (y)) ? (x) : (y))

typedef enum {
   MMP_TASK_POOL_EMPTY_ERROR      = 1001,
   MMP_ISR_POOL_EMPTY_ERROR       = 1002,
   MMP_TRAP_QUEUE_FULL_ERROR      = 1003,
   MMP_CPU0_TRAP_QUEUE_FULL_ERROR = 1004,
} MMP_ERRORS_E;

typedef enum {
   MOCAD_PQOS_SWITCH_ADD,
   MOCAD_PQOS_SWITCH_DELETE
} MOCAD_PQOS_SWITCH_ACTIONS_E;

/* The following flags are used with the do_flags context field */
typedef enum {
   MOCAD_PQOS_CREATE_FLOW = (1 << 0),
   MOCAD_PQOS_UPDATE_FLOW = (1 << 1),
   MOCAD_PQOS_DELETE_FLOW = (1 << 2),
   MOCAD_PQOS_QUERY       = (1 << 3),
   MOCAD_PQOS_LIST        = (1 << 4),
   MOCAD_PQOS_STATUS      = (1 << 5),
   MOCAD_DD               = (1 << 6),
   MOCAD_PS_CMD           = (1 << 7),
   MOCAD_MOCA_RESET       = (1 << 8),
   MOCAD_HOSTLESS_MODE    = (1 << 9),
   MOCAD_WAKEUP_NODE      = (1 << 10),
   MOCAD_ACA              = (1 << 11),
} MOCAD_DO_FLAGS_E;

typedef enum {
   MOCAD_DEVICE_ATTRS_PQOS = 0,
   MOCAD_DEVICE_ATTRS_AGGR,
   MOCAD_DEVICE_ATTRS_MAP_PROC,
   MOCAD_DEVICE_ATTRS_BONDED_AGGR,
} MOCAD_DEVICE_ATTRS_E;

struct mocad_node_attrs_hdr {
   uint8_t  node_id;
   uint8_t  num_attrs;
   uint16_t reserved;
};

struct mocad_attr_ie_hdr {
   uint8_t  attr_id;
   uint8_t  length;
   uint8_t  data[2];
};

static struct moca_snr_margin_table_rs snr_def_table_rs;
static struct moca_snr_margin_table_ldpc snr_def_table_ldpc;
static struct moca_snr_margin_table_ldpc_pri_ch snr_def_table_ldpc_pri_ch;
static struct moca_snr_margin_table_ldpc_sec_ch snr_def_table_ldpc_sec_ch;
static struct moca_snr_margin_table_ldpc_pre5 snr_def_table_ldpc_pre5;
static struct moca_snr_margin_table_ofdma snr_def_table_ofdma;
static struct moca_snr_margin_table_pre5_pri_ch snr_def_table_pre5_pri_ch;
static struct moca_snr_margin_table_pre5_sec_ch snr_def_table_pre5_sec_ch;

int mocad_cmd(void *ctx, uint32_t msg_type, uint32_t ie_type,
   const void *wr, int wr_len, void *rd, int rd_len, int flags);
int mocad_get_table(void * vctx, uint32_t ie_type, uint32_t * in,
   void * out, int struct_len, int max_out_len, int flags);
static void mocad_remove_pqos_flows(struct mocad_ctx *ctx);
int mocad_write_lof(struct mocad_ctx *ctx, int lof);
void mocad_init_forwarding(struct mocad_ctx *ctx);
int mocad_clear_mcfilter(struct mocad_ctx * ctx);
//////////////////////////////////////////////////////////////////////////
//                         RTT Module                                   //
//////////////////////////////////////////////////////////////////////////

int32_t  mocad_read_rtt_control(struct mocad_ctx *ctx, rtt_control_t * a_pRttControl, uint32_t a_rttBuffer);
int32_t  mocad_read_rtt_payload(struct mocad_ctx *ctx, unsigned char * a_pRttPayload, uint32_t a_rttBuffer);
int32_t  mocad_write_rtt_control(struct mocad_ctx *ctx, rtt_control_t * a_pRttControl, uint32_t a_rttBuffer);
uint32_t mocad_print_rtt(struct mocad_ctx *ctx, uint32_t rtt_buffer, uint32_t cpu_id);

//////////////////////////////////////////////////////////////////////////

/*
 * CLI / LOGGING
 */

MOCAD_S void mocad_usage(struct mocad_ctx *ctx)
{
   MoCAOS_Printf(ctx->os_handle, "usage: mocad [ -d <device> ] [ -w ] [ -f <firmware> ] [ -D ] "
      "[ -F <freq> ]\n"
         "         [ -q ] [ -v[v[v]] ] [-l <directory>] [-p <filename>] [-i <interface>] [ -P ]\n"
         "\n"
         "  -d <device>   Use <device> (default: /dev/bmoca0)\n"
         "  -w              Wait for START command (default: start MoCA immediately)\n"
         "  -f <firmware>   Use alternate firmware image (default: MOCACORE_PATH_0 )\n"
         "  -D              Run as a daemon (default: run in foreground)\n"
         "  -F <freq>       Force RF channel to <freq> Mhz (default: autoscan)\n"
         "  -l <directory>  Place lof and other persistent files into <directory>\n"
         "  -p <filename>   Write pid to <filename>\n"
         "  -i              Specify MoCA interface name\n"
         "\n"
         "Log levels:\n"
         "  (default)     Warnings, errors, major events\n"
         "  -q         Quiet: warnings and errors only\n"
         "  -v         Verbose: add minor socket hiccups\n"
         "  -vv           Debug: add debug messages\n"
         "  -vvv          Dump: add MMP hex dumps\n"
         "  -P         Enable lab_printf from core\n");
}

// #define die(fmt, ...) do { MoCAOS_Printf(g_ctx.os_handle,fmt, ##__VA_ARGS__); } while (1)
#define die(fmt, ...) fprintf(stderr,fmt,##__VA_ARGS__);

MOCAD_S int mocad_set_moca_core_trace_enable(void *vctx, uint32_t bool_val)
{
   int ret = 0;
   struct mocad_ctx *ctx = (struct mocad_ctx *) vctx;

   if (ctx->any_time.moca_core_trace_enable != bool_val)
   {
      ret = moca_set_moca_core_trace_enable(vctx, bool_val);

      if (ret == 0)
      {
         ctx->any_time.moca_core_trace_enable = bool_val;
      }
   }

   return ret;
}

MOCAD_S int mocad_set_lof(void * vctx, uint32_t val)
{
   int ret = 0;
   uint32_t orig_lof_update;
   struct mocad_ctx *ctx = (struct mocad_ctx *) vctx;

   // If the user specifies the LOF, we want it to
   // be saved regardless of the lof_update parameter
   if (ctx->cof == 0)
   {
      orig_lof_update = ctx->any_time.lof_update;
      ctx->any_time.lof_update = 1;
      mocad_write_lof(ctx, val);
      ctx->any_time.lof_update = orig_lof_update;

      ret = moca_set_lof(vctx, val/25);
   }
   else
   {
      ret = moca_set_lof(vctx, ctx->cof/25);
   }

   return(ret);
}

// These strings are put in ram as an optimization for 6802 standalone
const static char fours_fmt[] = "%4s: ";
const static char nl_fmt[] = "\n";
const static char s_fmt[] = "%s";
const static char s_nl_fmt[] = "%s\n";
const static char hex_fmt[] = "%02x%02x%02x%02x ";

MOCAD_S void mocad_dump_buf(struct mocad_ctx *ctx, const char *pfx,
   unsigned char *buf, int len)
{
   int i, j;

   if(ctx->in_lab_printf) {
      mocad_log(ctx, L_ALWAYS, nl_fmt);
      ctx->in_lab_printf = 0;
   }
   if(len < 0) {
      mocad_log(ctx, L_ALWAYS, "%s < %d bytes truncated >\n", pfx, -len);
      return;
   }
   for(i = 0; i < len; i += 0x10) {
      mocad_log(ctx, L_ALWAYS, s_fmt, pfx); 
      for(j = 0; (j < 0x10) && ((i + j) < len); j += 4)
         mocad_log(ctx, L_ALWAYS_NO_PFX, hex_fmt,
            buf[i + j], buf[i + j + 1],
            buf[i + j + 2], buf[i + j + 3]);
      mocad_log(ctx, L_ALWAYS_NO_PFX, nl_fmt); 
   }
}

MOCAD_S void mocad_lab_printf(struct mocad_ctx *ctx, char *msg)
{
   char *eol;
//   const char *pfx;

   if(! ctx->show_lab_printf)
   {
      mocad_set_moca_core_trace_enable(ctx,0);
      return;
   }
   
   while(msg && *msg) {

      eol = strchr(msg, '\n');
      if(eol) {
         *eol = 0;
         if(ctx->in_lab_printf || msg != eol)
            mocad_log(ctx, L_CORE_TRACE, s_nl_fmt, msg);
         ctx->in_lab_printf = 0;
         msg = eol + 1;
      } else {
         mocad_log(ctx, L_CORE_TRACE, s_fmt, msg);
         ctx->in_lab_printf = 1;
         msg = NULL;
      }
   }
}

MOCAD_S void mocad_log_trap(struct mocad_ctx *ctx, char *msgbuf)
{
   struct mmp_msg_hdr *mh;
   int n=0;

   if (ctx->verbose & L_PRINT_TRAP) 
   {
      mh = (struct mmp_msg_hdr *)msgbuf;
      mh->type = BE32(MOCA_MSG_TRAP);
      mh->ie_type = BE32(IE_MOCAD_PRINTF);

      n = strlen(msgbuf+sizeof(struct mmp_msg_hdr)) + 1; // +1 for \0

      n = ((n+3) & 0xFFFFFFFC) + MOCA_EXTRA_MMP_HDR_LEN; // pad to 4 bytes
      mh->length = BE32(n);

      MoCAOS_SendMMP((MoCAOS_Handle)ctx->os_handle, MoCAOS_CLIENT_BROADCAST, (unsigned char *)msgbuf, n - MOCA_EXTRA_MMP_HDR_LEN+ sizeof(struct mmp_msg_hdr));
   }
}

MOCAD_S void mocad_log(struct mocad_ctx *ctx, int lev, const char *fmt,...)
{
   va_list ap;
   char msgbuf[sizeof(struct moca_mocad_printf_out) + sizeof(struct mmp_msg_hdr) + sizeof(uint32_t)];
   char *buf = (char *)msgbuf + sizeof(struct mmp_msg_hdr); 
   char tmp_fmt[256];

   if (((ctx->verbose & lev) == 0) &&
       (lev != L_ALWAYS)           &&
       (lev != L_ALWAYS_NO_PFX)    &&
       (lev != L_CORE_TRACE))
      return;

   if (ctx->in_lab_printf && ((lev & L_CORE_TRACE) == 0)) {
      msgbuf[sizeof(struct mmp_msg_hdr)] = '\n';
      mocad_log_trap(ctx, msgbuf);
      MoCAOS_Printf(ctx->os_handle, nl_fmt);
      ctx->in_lab_printf = 0;
   }

   if (lev != L_ALWAYS_NO_PFX)
   {
      if ((strlen(ctx->ifname) > 6) || !ctx->ifname[0]) {
         MoCAOS_Printf(ctx->os_handle,"MOCA: ");
      }
      else if (!(ctx->in_lab_printf && (lev == L_CORE_TRACE))) {
         MoCAOS_Printf(ctx->os_handle, fours_fmt, ctx->ifname);
      }
   }
   strncpy(tmp_fmt, fmt, sizeof(tmp_fmt)-1); // why copy?  fmt may be in flash in standalone mode, and hence can't be byte-accessed.  vsnprintf will crash in that case.
   tmp_fmt[sizeof(tmp_fmt)-1] = '\0';

   va_start(ap, fmt);
   vsnprintf(buf, sizeof(struct moca_mocad_printf_out)-1, tmp_fmt, ap);
   va_end(ap);

   buf[sizeof(struct moca_mocad_printf_out)-1] = '\0';

   mocad_log_trap(ctx, msgbuf);
   MoCAOS_Printf(ctx->os_handle, s_fmt, buf);
}

MOCAD_S int mocad_is64bitformat(char *x)
{
   x++;  // skip past %

   // skip past numbers
   while ((*x >= '0') && (*x <= '9')) x++;

   if ((*x == 'l') && (*(x+1) == 'l'))
      return(1);
   else
      return(0);
}

MOCAD_S int checkPacketRam(struct mocad_ctx *ctx)
{
   int rc = 0;
   unsigned int M1, M2;

   if (0 != MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)&M1, 4, (unsigned char *)PACKETRAM_ADDR_REINIT))
      mocad_log(ctx, L_ERR, "====== WARNING: Cannot check packet ram ======\n"); 

   if (0 != MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)&M2, 4, (unsigned char *)PACKETRAM_ADDR_REINIT+ MOCAD_FIRMWAREDATA_REINIT_LEN-4))
      mocad_log(ctx, L_ERR, "====== WARNING: Cannot check packet ram ======\n"); 


   if ((BE32(M1) != PACKETRAM_M1) || 
       (BE32(M2) != PACKETRAM_M2))
      mocad_log(ctx, L_ERR, "WARNING: Invalid Packet Ram M1(%08X) or M2(%08X) %d !!!\n", BE32(M1),
         BE32(M2), PACKETRAM_ADDR_REINIT+ MOCAD_FIRMWAREDATA_REINIT_LEN-4);

   return(rc);
}

static const char hex8_fmt[] = "%08x ";

/* 
 * This function takes the parameters from vars_array[] and lists them in string, up to 
 * a length of max_len bytes. 
 */
MOCAD_S void mocad_param_string(struct mocad_ctx *ctx, char* string, unsigned int max_len, 
   unsigned int numVars, unsigned int vars_array[])
{
   unsigned int   i;
   
   if (string == NULL)
      return;

   string[0] = '\0';

   if (numVars == 0)
   {
      return;
   }
   
   /* We print the variables in hex format with one space in between, so each
      32-bit variable takes up 9 bytes. Print as many as will fit. */
   if (numVars * 9 > max_len)
      numVars = max_len / 9;
      
   for (i = 0; i < numVars; i++)
   {
      string += sprintf(string, hex8_fmt, (unsigned int) BE32(vars_array[i]));
   }
}

int mocad_key_compare(const void *pkey, const void *pelem)
{
   // check to see if we are in between two keys to find the closest match.
   // The table is preceded with an entry that has a key of 0, so it's 
   // safe to access it
   unsigned int key = *(unsigned int *)pkey;
   unsigned int p1 = BE32(*(unsigned int *)pelem);
   unsigned int p2 = BE32(*((unsigned int *)pelem+2));

   if ((key > p1) &&
       (key < p2))
       return 0;
   
   if (key < p1) return -1;
   if (key > p1) return 1;

   return 0;
}

#define MAX_OFFSET_CACHE 128

void mocad_find_offset(struct mocad_ctx *ctx, unsigned int cpuid, unsigned int key, unsigned int *value, unsigned int after)
{
   // Look up the value based on the key from the sorted lookup table
   // The lookup table contains an array of two 32-bit values.  The first is the key, the second is the value
   // If after==1, then we are looking for the item after the match.  If after==0, we are looking for the item before
   //   the match.  Asserts use the item before the FW_Assert call, Logs use the item after the Host_Send function

   unsigned int *entry = NULL;

#ifdef STANDALONE
   // First we check a cache of values.  This speeds things up when the lookup table is in flash (6802 standalone).
   static unsigned int cache_key_cpu0[MAX_OFFSET_CACHE] = {0};
   static unsigned int *cache_offset_cpu0[MAX_OFFSET_CACHE];
   static unsigned int cache_wr_index_cpu0=0;
   static unsigned int cache_key_cpu1[MAX_OFFSET_CACHE] = {0};
   static unsigned int *cache_offset_cpu1[MAX_OFFSET_CACHE];
   static unsigned int cache_wr_index_cpu1=0;
   unsigned int i;
   int index;
#endif

   // The key is actually an address of code in firmware.  The address bit 0 will be set if it's mips16 code,
   // so ignore bit 0.  The return address will also be after the branch, which is sometimes another trace/assert,
   // so subtract 1 to make sure we don't get that address instead:
   key = (key & 0xFFFFFFFE) -1;

   if (cpuid == 0)
   {
#ifdef STANDALONE
      index = cache_wr_index_cpu0;
      for (i=0;i<MAX_OFFSET_CACHE;i++)
      {
         if (cache_key_cpu0[index] == key)
            entry = cache_offset_cpu0[index];
         index--;
         if (index < 0)
            index = MAX_OFFSET_CACHE-1;
      }

      if (entry == NULL)
      {
#endif
      entry = (unsigned int *)bsearch(&key, ctx->cpu0lookup, ctx->sizeofcpu0lookup/8, 2*sizeof(int), mocad_key_compare);

#ifdef STANDALONE
         if (entry != NULL)
         {
            cache_key_cpu0[cache_wr_index_cpu0] = key;
            cache_offset_cpu0[cache_wr_index_cpu0] = entry;
            cache_wr_index_cpu0 = cache_wr_index_cpu0 == MAX_OFFSET_CACHE-1?0:cache_wr_index_cpu0+1;
         }
      }
#endif
   }
   else if (cpuid == 1)
   {
#ifdef STANDALONE
      index = cache_wr_index_cpu1;
      for (i=0;i<MAX_OFFSET_CACHE;i++)
      {
         if (cache_key_cpu1[index] == key)
            entry = cache_offset_cpu1[index];
         index--;
         if (index < 0)
            index = MAX_OFFSET_CACHE-1;
      }

      if (entry == NULL)
      { 
#endif
      entry = (unsigned int *)bsearch(&key, ctx->cpu1lookup, ctx->sizeofcpu1lookup/8, 2*sizeof(int), mocad_key_compare);

#ifdef STANDALONE
         if (entry != NULL)
         {
            cache_key_cpu1[cache_wr_index_cpu1] = key;
            cache_offset_cpu1[cache_wr_index_cpu1] = entry;
            cache_wr_index_cpu1 = cache_wr_index_cpu1 == MAX_OFFSET_CACHE-1?0:cache_wr_index_cpu1+1;
         }
      }
#endif
   }

   if (!entry)
      *value = 0xFFFFFFFF;
   else
      *value = BE32(*(entry+1+after*2));
}

MOCAD_S void mocad_string_formatting(struct mocad_ctx *ctx, unsigned int lookup_required, char* string, unsigned int stringID, 
   unsigned int numVars, unsigned int vars_array[])
{
   int i;
// int paramInString;
   char *strPtr = NULL;
   char tmpStr[400];
   unsigned long long val;
   unsigned char *fmtstring;
   unsigned int sectlen = 0;
   unsigned int cpuid;
   unsigned int offset; // offset into the string table

   if ( stringID == 0xffffffff )
      return;

   if (lookup_required)
   {
      // New style prints find the offset of the string from
      // a second table.  This table is a sorted array of Key,value pairs (u32x2)
      // The key is stringID, the value is the offset into the string table

      cpuid = numVars >> 24; // top 8 bits are used for CPU ID
      numVars = numVars & 0xFFFFFF;
      mocad_find_offset(ctx, cpuid, stringID, &offset, 1);
   }
   else
   {
      // Top bit is used to identify new style vs old style mmp.  Not using old style in moca 2.0
      stringID = stringID & 0x7fffffff;
      cpuid = stringID >> 24;      // top 8 bits are the CPU ID of the string
      offset= stringID & 0xFFFFFF; // bottom 24 bits are the offset of the string into the table
   }

   if (cpuid == 0) 
   {
      if (!ctx->cpu0strings)
         fmtstring = NULL;
      else
         fmtstring = ctx->cpu0strings + offset; 
      sectlen = ctx->sizeofcpu0strings;
   }
   else if (cpuid == 1)
   {
      if (!ctx->cpu1strings)
         fmtstring = NULL;
      else
         fmtstring = ctx->cpu1strings + offset;
      sectlen = ctx->sizeofcpu1strings;
   }
   else
   {
      fmtstring = NULL;
      sectlen = 0;
   }
   if ((!fmtstring) || (offset >= sectlen))
   {
      sprintf(string, "Invalid string ID (%8x).\n", 
         stringID);
      return;
   }

   strncpy(string, (char *)fmtstring, MAX_STRING_LENGTH-100);

   string[MAX_STRING_LENGTH-100] = 0;

#if (0)
   /* Sanity 2: Verify enough % in string. If there are too few, add some.
     Ignore cases that the string does not contain any parameter and there is a single 
     input parameter. 
     This is because the printout from the ISRs always pass single parameter, even if 
     the string has none. */
   strPtr = string;
   paramInString = 0;
   do 
   {
      strPtr = strchr(strPtr, '%');
      if (strPtr)
      {
         paramInString++;
         strPtr++;
      }
   } while (strPtr);

   if (paramInString && (numVars > paramInString))
   {
      int i;

      /* Clear the \n. */ 
      strPtr = strchr(string, '\n');
      *strPtr = 0;

      /* Too few parameter in the original string. Add some %. */
      for (i=paramInString; i<numVars; i++)
      {
         strcat(string, " [%d]");
      }
      strcat(string, "==> Warning: string modified\n");
   }
   /* Sanity 2: End */
#endif
   if (numVars)
   {
      for (i=numVars-1; i>=0; i--)
      {
         strPtr = strrchr(string, '%');
         if (strPtr)
         {
            if (mocad_is64bitformat(strPtr))
            {
               if (i == 0)
               {
                  mocad_log(ctx, L_ERR, 
                     "Invalid 64-bit printf\n");
               }
               else
               {
                  i--;
                  val = *((unsigned long long *)&vars_array[i]);
                  val = BE64( val );
                  sprintf(tmpStr, strPtr, val);
               }
            }
            else
            {
               //take care of %s
               char *strTmp = NULL;             
               unsigned char *fmtstring1 = NULL;
               // unsigned int strStringId =0; no %s support for now

               strTmp = strPtr;
               strTmp++; //skip past %

               // flag exist
               if ( strTmp[0] == '-' )
               {
                  strTmp++;
               }              

               while( (strTmp[0] >='0') && (strTmp[0] <='9') )
               {// handle %8s ...
                  strTmp++;
               }
               switch (*strTmp)
               {
                  case 's':           /* string */
                  /* Remove string support for now:

                     strStringId = BE32(vars_array[i]);
                     strStringId = strStringId & 0x7fffffff;
                     if ((strStringId >> 24) == 0)  // top 8 bits are the CPU ID of the string
                     {
                        if (!ctx->cpu0strings)
                           fmtstring1 = NULL;
                        else
                           fmtstring1 = ctx->cpu0strings + (strStringId & 0xFFFFFF); // bottom 24 bits are the offset of the string into the table
                     }
                     else if ((strStringId >> 24) == 1)
                     {
                        if (!ctx->cpu1strings)
                           fmtstring1 = NULL;
                        else
                           fmtstring1 = ctx->cpu1strings + (strStringId & 0xFFFFFF);
                     }
                     if (!fmtstring1)
                     {
                        mocad_log(ctx, L_ERR, "Invalid String string ID (%8x).\n", strStringId);
                        return;
                     }
                     */
                     sprintf(tmpStr, "<string %X>", (unsigned int)fmtstring1);
                     break;
                  default:
                     sprintf(tmpStr, strPtr, BE32(vars_array[i]));
                     break;
               }
            }
            strcpy(strPtr, tmpStr);
         }
      }
   }
}

#define MOCAD_CHUNKSIZE         4096
#define MOCAD_PKTRAM_BASE       0x80008000
#define MOCAD_PKTRAM_CODE_BASE  0x8000E000
 
// extract packet ram from mocacore.bin section
MOCAD_S void mocad_get_packetram(struct mocad_ctx *ctx, unsigned char *fw_img,
   unsigned int img_length)
{
   unsigned int i,j;
   unsigned int *addr = (unsigned int *) fw_img;
   int in_packetram_section = 0;
   unsigned int sectionAddr=0;
   unsigned int sectionLen=0;   

   // Each CPU's image follows this format:
   // <ADDR> <LEN> <DATA> [PADDING].  PADDING is 0. 
   // A special rule is that the word at the end of each
   // 4096 byte block is ignored
   // no record will span beyond a 4096 byte block
   enum
   {
      STATE_ADDR,
      STATE_LENGTH,
      STATE_DATA
   } state = STATE_ADDR;

   addr += MOCAD_CHUNKSIZE/4; // skip the bootloader
   
   for (i=MOCAD_CHUNKSIZE;i<img_length;i+=MOCAD_CHUNKSIZE)
   {
      for (j=0;j<(MOCAD_CHUNKSIZE-4)/4;j++)
      {
         switch (state)
         { 
         case STATE_DATA:
            if (in_packetram_section)
            {
               if (sectionAddr - MOCAD_PKTRAM_BASE > MOCAD_FIRMWAREDATA_REINIT_LEN)
               {
                  mocad_log(ctx, L_ERR, "ERROR: firmware address out of range\n");
                  return;
               }

               if (0 != MoCAOS_WriteMem(ctx->os_handle, (unsigned char *)addr, sectionLen, (unsigned char *)sectionAddr-MOCAD_PKTRAM_BASE + PACKETRAM_ADDR_REINIT))
               {
                  mocad_log(ctx, L_ERR, "ERROR: Unable to update packet ram (%x)\n",sectionAddr);
                  return;
               }
            }

            addr += sectionLen/4;
            j += sectionLen/4-1;
            state = STATE_ADDR;

            break;

         case STATE_ADDR:
            sectionAddr = BE32(*(addr));

            if ((sectionAddr&3) != 0)
            {
               mocad_log(ctx, L_ERR, "ERROR: firmware contains an unaligned address 0x%08X\n", sectionAddr);
               return;
            }

            if (sectionAddr)
            {
               in_packetram_section = 
                  ((sectionAddr >= MOCAD_PKTRAM_BASE) && (sectionAddr < MOCAD_PKTRAM_BASE+MOCAD_FIRMWAREDATA_REINIT_LEN)?1:0);
               
               state = STATE_LENGTH;
            }
            addr++;
            break;
            
         case STATE_LENGTH:
            sectionLen = BE32(*(addr));

            if ((sectionLen&3) != 0)
            {
               mocad_log(ctx, L_ERR, "ERROR: firmware contains an unaligned length 0x%08X\n", sectionLen);
               return;
            }

            if (sectionLen)
               state = STATE_DATA;

            addr++;
            break;
         }
      }

      addr++; // The last word of each chunk isn't used
   }
}


MOCAD_S void mocad_update_times(struct mocad_ctx *ctx, int event)
{
   unsigned int now = MoCAOS_GetTimeSec();

   switch (event)
   {
      case MOCA_TIME_EVENT_CORE_UP:
         if (ctx->moca_running == 0)
            ctx->core_uptime = MoCAOS_GetTimeSec();
         break;
      case MOCA_TIME_EVENT_CORE_DOWN:
         if (ctx->moca_running == 1)
         {
            ctx->core_downtime = MoCAOS_GetTimeSec();
            if (ctx->link_state == LINK_STATE_UP)
               ctx->link_downtime = MoCAOS_GetTimeSec();
         }
         break;
      case MOCA_TIME_EVENT_LINK_UP:
         ctx->link_uptime = now;
         break;
      case MOCA_TIME_EVENT_LINK_DOWN:
         ctx->link_downtime = now;
         break;
      default:
         mocad_log(ctx, L_WARN, "unknown time event %d\n", event);
         break;
   }
}


MOCAD_S void mocad_get_core_times(struct mocad_ctx * ctx, 
   uint32_t * core_uptime, uint32_t * link_uptime)
{
   unsigned int last_time;
   unsigned int now = MoCAOS_GetTimeSec();

   mocad_log(ctx, L_DEBUG, "Get Times:\n");
   mocad_log(ctx, L_DEBUG, "  Now    : %u s\n", now);
   mocad_log(ctx, L_DEBUG, "  Core Up  : %u s\n", ctx->core_uptime);
   mocad_log(ctx, L_DEBUG, "  Core Down: %u s\n", ctx->core_downtime);
   mocad_log(ctx, L_DEBUG, "  Link Up  : %u s\n", ctx->link_uptime);
   mocad_log(ctx, L_DEBUG, "  Link Down: %u s\n", ctx->link_downtime);

   if ((ctx->moca_running == 1) &&
       (ctx->link_state == LINK_STATE_UP))
      last_time = now;
   else
      last_time = ctx->link_downtime;

   if (last_time > ctx->link_uptime)
      *link_uptime = (uint32_t) BE32(last_time - ctx->link_uptime);
   else
      *link_uptime = (uint32_t)BE32((0xFFFFFFFF - ctx->link_uptime + 1) + last_time);

   if (ctx->moca_running == 1)
      last_time = now;
   else
      last_time = ctx->core_downtime;

   if (last_time > ctx->core_uptime)
      *core_uptime = (uint32_t)BE32(last_time - ctx->core_uptime);
   else
      *core_uptime = (uint32_t)BE32((0xFFFFFFFF - ctx->core_uptime + 1) + last_time);

}

MOCAD_S void mocad_clear_base_stats(struct mocad_ctx *ctx)
{
   memset(&ctx->gen_stats_base, 0, sizeof(ctx->gen_stats_base));
   memset(&ctx->node_stats_base, 0, sizeof(ctx->node_stats_base));
   memset(&ctx->node_stats_ext_base, 0, sizeof(ctx->node_stats_ext_base));
   memset(&ctx->last_node_stats_ext, 0, sizeof(ctx->last_node_stats_ext));
}

MOCAD_S void mocad_update_link_state(struct mocad_ctx *ctx, int new_state)
{
   struct moca_network_status ns;
   struct moca_interface_status is;
   int ret;
   int prev_link_state;

   prev_link_state = ctx->link_state;
   ctx->link_state = new_state;
   switch(new_state) {
      case LINK_STATE_UP:
         mocad_update_times(ctx, MOCA_TIME_EVENT_LINK_UP);

         ret = moca_get_network_status(ctx, &ns);
         ret |= moca_get_interface_status(ctx, &is);
         if (ret == 0) 
         {
            ctx->active_nodes = ns.connected_nodes;

            // Check for a change in the LOF in the small chance that 
            // we missed the LOF trap
            if ((is.rf_channel*25) != (uint32_t) ctx->disk_lof)
            {
               mocad_write_lof(ctx, (is.rf_channel*25));
            }
         }
         else 
         {
            mocad_log(ctx, L_ERR, 
               "moca_get_network_status/moca_get_interface_status ret %d\n", ret);
         }

         if (prev_link_state != new_state)
         {
            mocad_log(ctx, L_INFO, "MoCA link is up after %ds of downtime\n", 
               MoCAOS_GetTimeSec() - ctx->link_downtime );
            MoCAOS_EnableDataIf(ctx->os_handle, ctx->ifname, 0);

            if (ctx->any_time.host_qos)
            {
               mocad_init_forwarding(ctx);
            }

            mocad_mps_link_state(ctx, new_state);
         }

         if (!ctx->show_lab_printf)
         {
            mocad_set_moca_core_trace_enable(ctx, ctx->show_lab_printf);
         }

         ctx->link_up_count++;
         ctx->topology_change_count = 0;
         break;

      case LINK_STATE_DOWN:
         mocad_update_times(ctx, MOCA_TIME_EVENT_LINK_DOWN);
         mocad_remove_pqos_flows(ctx);
         mocad_clear_mcfilter(ctx);

         if (ctx->moca_running)
         {
            if ((prev_link_state != new_state) && (!ctx->hostless_mode))
            {
               mocad_log(ctx, L_INFO, "MoCA link is down after %ds of uptime\n", 
                  MoCAOS_GetTimeSec() - ctx->link_uptime );
               // ifdown
               MoCAOS_EnableDataIf(ctx->os_handle, ctx->ifname, 2);
               
               // To release all the data buffers that already exist in the Packet RAM wait 20 Msec..
               MoCAOS_MSleep(20);                    
                    

               // copy data to packet ram
               mocad_get_packetram(ctx, ctx->cpu_img[1], ctx->cpu_img_len[1]);

               checkPacketRam(ctx);

               mocad_mps_link_state(ctx, new_state);
            }

            // send download done

            if (!ctx->hostless_mode)
               __moca_set_download_to_packet_ram_done(ctx);

            if (!ctx->show_lab_printf)
            {
               mocad_set_moca_core_trace_enable(ctx,1);
            }
         }

         ctx->active_nodes = 0;

         ctx->link_down_count++;
         break;

      case LINK_STATE_DEBUG:
         mocad_log(ctx, L_INFO,
            "MoCA is in debug mode\n");
         break;
   }
}

MOCAD_S void mocad_check_link_state(struct mocad_ctx *ctx)
{
   struct moca_interface_status is;
   uint32_t res;
   
   res = moca_get_interface_status(ctx, &is);
   if (res == 0)
   {
      if (is.link_status != (uint32_t) ctx->link_state)
      {
         mocad_update_link_state(ctx, is.link_status);
      }

      if ((is.link_status == 1) &&
          ((is.rf_channel*25) != (uint32_t) ctx->disk_lof))
      {
         mocad_log(ctx, L_WARN, 
            "Operating frequency changed\n");
         mocad_write_lof(ctx, (is.rf_channel*25));
      }
   }
}

/*
 * FILESYSTEM OPERATIONS
 */
#if defined(DSL_MOCA) /* DSL Code */
int mocad_read_reg(struct mocad_ctx * ctx, uint32_t addr)
{
   int fd, len;
   char buffer[64];

   fd = open("/proc/bcmlog", O_RDWR | O_NONBLOCK);

   if (fd < 0)
   {
      mocad_log(ctx, L_ERR, "%s: couldn't open bcmlog (%d)\n",
         __FUNCTION__, fd);
      return (fd);
   }

   len = sprintf(buffer, "m 0x%x 1 w", addr);

   if(write(fd, buffer, len) != len)
   {
      close(fd);
      mocad_log(ctx, L_ERR, "%s: couldn't write to bcmlog\n",
         __FUNCTION__);
      return (-1);
   }

   close(fd);

   return(0);
}

int mocad_write_reg(struct mocad_ctx * ctx, uint32_t addr, uint32_t val)
{
   int fd, len, wrlen;
   char buffer[64];

   fd = open("/proc/bcmlog", O_RDWR | O_NONBLOCK);

   if (fd < 0)
   {
      mocad_log(ctx, L_ERR, "%s: couldn't open bcmlog (%d)\n",
         __FUNCTION__, fd);
      return (fd);
   }

   len = sprintf(buffer, "w 0x%x 0x%x 1 w", addr, val);

   wrlen = write(fd, buffer, len);
   if( wrlen != len)
   {
      close(fd);
      mocad_log(ctx, L_ERR, "%s: couldn't write to bcmlog (%d vs. %d)\n",
         __FUNCTION__, wrlen, len);
      return (-1);
   }

   close(fd);

   return(0);
}

#if (0)
int mocad_read_rtt_from_ctrl_ram(struct mocad_ctx *ctx)
{
   uint32_t start_addr;
   uint32_t end_addr;
   void     *pMem;
   struct   moca_xfer x;
   uint32_t *temp_ptr;
   uint32_t *temp_ptr_kernel;

   /* Print the RTT buffer */
   start_addr = BE32(0x4C2CC);
   end_addr = start_addr + BE32(1500 * sizeof(uint32_t));

   mocad_log(ctx, L_ERR, "FOUND RTT DATA @ 0x%X\n", start_addr); 

   /* the memory transfer at the kernel layer might require 
   * 64-bit alignment */
   if (MoCAOS_MemAlign(&pMem, 64, (end_addr - (start_addr & 0xFFFFFFF8))) < 0)
     return;

   ret = MoCAOS_ReadMem((unsigned char *)pMem, (end_addr - (start_addr & 0xFFFFFFF8)),
      (start_addr & 0xFFFFFFF8) );

   if(ret != 0)
   {
     mocad_log(ctx, L_ERR, "Unable to read mem from kernel\n");
   }
   else
   {
     temp_ptr_kernel = pMem + (start_addr & 0x7);

//          mocad_log(ctx, L_ERR, "%08x %08x %08x\n", 
//            BE32(*(temp_ptr_kernel + 1500)),
//            BE32(*(temp_ptr_kernel + 1501)),
//            BE32(*(temp_ptr_kernel + 1502)));

     for (temp_ptr = (uint32_t*)start_addr; 
       (uint32_t)temp_ptr < end_addr-12; 
       temp_ptr += 10, temp_ptr_kernel += 10)
     {
       mocad_log(ctx, L_ERR, 
         "%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x\n",
         BE32(*(temp_ptr_kernel + 0)), BE32(*(temp_ptr_kernel + 1)), 
         BE32(*(temp_ptr_kernel + 2)), BE32(*(temp_ptr_kernel + 3)), 
         BE32(*(temp_ptr_kernel + 4)), BE32(*(temp_ptr_kernel + 5)), 
         BE32(*(temp_ptr_kernel + 6)), BE32(*(temp_ptr_kernel + 7)),
         BE32(*(temp_ptr_kernel + 8)), BE32(*(temp_ptr_kernel + 9)));
     }
   }
   MoCAOS_FreeMemAlign(pMem);
}
#endif

int mocad_read_lof(struct mocad_ctx * ctx)
{
/*
   int ret;
   uint8_t    buf[sizeof(CmsMsgHeader) + CMS_IFNAME_LENGTH];
   CmsMsgHeader *pMsg = (CmsMsgHeader *)&buf[0];

   pMsg->type        = CMS_MSG_MOCA_READ_LOF;
   pMsg->src         = MAKE_SPECIFIC_EID(getpid(), EID_MOCAD);
   pMsg->dst         = EID_SSK;
   pMsg->flags_request = 1;          
   pMsg->dataLength  = CMS_IFNAME_LENGTH;
   strcpy((char*)(pMsg + 1), ctx->ifname);    

   ret = cmsMsg_sendAndGetReplyBuf(ctx->cmsMsgHandle, pMsg, &pMsg);
   if ( CMSRET_SUCCESS != ret )
   {
      mocad_log(ctx, L_WARN, "can't read LOF from CMS: error %d\n", ret);
      ctx->disk_lof = 0 ;
      return -1;
   }

   ctx->disk_lof = pMsg->wordData;
*/
   /* Don't do anything. CMS isn't ready for this message when we
    * send it. CMS will initialize MoCA with the proper parameters
    * prior to starting.
    */
   return(0);
}

int mocad_write_lof(struct mocad_ctx *ctx, int lof)
{
   int        ret;
   uint8_t    buf[sizeof(CmsMsgHeader) + CMS_IFNAME_LENGTH];
   CmsMsgHeader *pMsg = (CmsMsgHeader *)&buf[0];
   
   ctx->cof = 0;

   if(ctx->any_time.lof_update == 0)
      return(0);

   ctx->any_time.lof = lof;

   memset(&buf[0], 0x0, sizeof(buf));

   pMsg->type       = CMS_MSG_MOCA_WRITE_LOF;
   pMsg->src        = MAKE_SPECIFIC_EID(getpid(), EID_MOCAD);
   pMsg->dst        = EID_SSK;
   pMsg->flags_event = 1;
   pMsg->dataLength  = CMS_IFNAME_LENGTH;
   strcpy((char*)(pMsg + 1), ctx->ifname);
   pMsg->wordData   = lof;

   ret = cmsMsg_send(ctx->cmsMsgHandle, pMsg);
   if ( CMSRET_SUCCESS != ret )
   {
      mocad_log(ctx, L_WARN, "can't write LOF in CMS: error %d\n", ret);
      return -1;
   }

   ctx->disk_lof = lof;
   return 0;
}

#if 1
int mocad_read_nondefseqnum (struct mocad_ctx * ctx)
{
/*
   CmsRet        ret;
   uint8_t    buf[sizeof(CmsMsgHeader) + CMS_IFNAME_LENGTH];
   CmsMsgHeader *pMsg = (CmsMsgHeader *)&buf[0];

   pMsg->type        = CMS_MSG_MOCA_READ_MRNONDEFSEQNUM;
   pMsg->src         = MAKE_SPECIFIC_EID(getpid(), EID_MOCAD);
   pMsg->dst         = EID_SSK;
   pMsg->flags_request = 1;          
   pMsg->dataLength  = CMS_IFNAME_LENGTH;
   strcpy((char*)(pMsg + 1), ctx->ifname);    

   ret = cmsMsg_sendAndGetReplyBuf(ctx->cmsMsgHandle, pMsg, &pMsg);
   if ( CMSRET_SUCCESS != ret )
   {
      mocad_log(ctx, L_WARN, 
         "can't read NONDEFSEQNUM from CMS: error %d\n", ret);
      ctx->disk_nondefseqnum = 0 ;
      return -1;
   }

   ctx->disk_nondefseqnum = pMsg->wordData;
*/
   /* Don't do anything. CMS isn't ready for this message when we
    * send it. CMS will initialize MoCA with the proper parameters
    * prior to starting.
    */
   return(0);

}

int mocad_write_nondefseqnum (struct mocad_ctx * ctx, int nondefseqnum)
{
   CmsRet ret;
   uint8_t    buf[sizeof(CmsMsgHeader) + CMS_IFNAME_LENGTH];
   CmsMsgHeader *pMsg = (CmsMsgHeader *)&buf[0];

   if (ctx->disk_nondefseqnum == nondefseqnum)
      return(0);
         
   memset(&buf[0], 0x0, sizeof(buf));

   pMsg->type       = CMS_MSG_MOCA_WRITE_MRNONDEFSEQNUM;
   pMsg->src        = MAKE_SPECIFIC_EID(getpid(), EID_MOCAD);
   pMsg->dst        = EID_SSK;
   pMsg->flags_event = 1;
   pMsg->dataLength  = CMS_IFNAME_LENGTH;
   strcpy((char*)(pMsg + 1), ctx->ifname);
   pMsg->wordData   = nondefseqnum;

   ret = cmsMsg_send(ctx->cmsMsgHandle, pMsg);
   if ( CMSRET_SUCCESS != ret )
   {
      mocad_log(ctx, L_WARN, 
         "can't write NONDEFSEQNUM in CMS: error %d\n", ret);
      return -1;
   }   

   ctx->disk_nondefseqnum = nondefseqnum;
   return 0;
}
#endif

int mocad_read_e2m(struct mocad_ctx *ctx)
{
   int tmp[3] = {0, 0, 0};
   char filename[MOCA_FILENAME_LEN], buf[36];

   sprintf(filename, "E2M%s", ctx->ifname);
   if (cmsPsp_get(filename, buf, sizeof(buf)) <= 0)
      goto bad;

   if(sscanf(buf, "%d %d %d", &tmp[0], &tmp[1], &tmp[2]) != 3)
      goto bad;

   mocad_log(ctx, L_DEBUG, "E2M data: %d %d %d\n", tmp[0], tmp[1], tmp[2]);
   memcpy(&ctx->error_to_mask, tmp, sizeof(ctx->error_to_mask));
   return(0);

bad:
   /* not a serious failure; it will be missing on the first run */
   mocad_log(ctx, L_VERBOSE, "can't read E2M file '%s'\n", filename);
   return(-1);
}

int mocad_write_e2m(struct mocad_ctx *ctx, struct moca_error_to_mask * e2m)
{
   int len;
   char filename[MOCA_FILENAME_LEN], buf[36];

   mocad_log(ctx, L_VERBOSE, "Saving new E2M %d %d %d\n", 
      e2m->error1, e2m->error2, e2m->error3);

   sprintf(filename, "E2M%s", ctx->ifname);
   len = sprintf(buf, "%d %d %d\n", e2m->error1, e2m->error2, e2m->error3);

   if (cmsPsp_set(filename, buf, len) != CMSRET_SUCCESS)
      goto bad;

   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write E2M file '%s'\n", filename);
   return(-1);
}

int mocad_read_rf_calib_data(struct mocad_ctx * ctx)
{
   char filename[MOCA_FILENAME_LEN];
   int len;

   sprintf(filename, "RFCAL%s", ctx->ifname);
   len = sizeof(ctx->rf_calib_data);
   if (cmsPsp_get(filename, &ctx->rf_calib_data, len) <= 0)
      goto bad;

   mocad_log(ctx, L_INFO, "Found valid RF Calib Data file\n");

   return(0);

bad:
   /* not a serious failure; it will be missing on the first run */
   mocad_log(ctx, L_VERBOSE, "can't read RF Calib Data file '%s'\n", filename);
   return(-1);
}

int mocad_write_rf_calib_data(struct mocad_ctx * ctx)
{
   int len;
   char filename[MOCA_FILENAME_LEN];

   mocad_log(ctx, L_VERBOSE, "Saving new RFCAL\n");

   sprintf(filename, "RFCAL%s", ctx->ifname);
   len = sizeof(ctx->rf_calib_data);

   if (cmsPsp_set(filename, &ctx->rf_calib_data, len) != CMSRET_SUCCESS)
      goto bad;

   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write RFCAL file '%s'\n", filename);
   return(-1);
}

int mocad_read_probe_2_results(struct mocad_ctx * ctx)
{
   char filename[MOCA_FILENAME_LEN];
   int len;

   sprintf(filename, "PROBE2%s", ctx->ifname);
   len = sizeof(ctx->probe_2_results);
   if (cmsPsp_get(filename, &ctx->probe_2_results, len) <= 0)
      goto bad;

   mocad_log(ctx, L_INFO, "Found valid Probe 2 Results file\n");

   return(0);

bad:
   /* not a serious failure; it will be missing on the first run */
   mocad_log(ctx, L_VERBOSE, "can't read Probe 2 Results file '%s'\n", filename);
   return(-1);
}

int mocad_write_probe_2_results(struct mocad_ctx * ctx)
{
   int len;
   char filename[MOCA_FILENAME_LEN];

   mocad_log(ctx, L_VERBOSE, "Saving new Probe 2 Results\n");

   sprintf(filename, "PROBE2%s", ctx->ifname);
   len = sizeof(ctx->probe_2_results);

   if (cmsPsp_set(filename, &ctx->probe_2_results, len) != CMSRET_SUCCESS)
      goto bad;

   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write Probe 2 Results file '%s'\n", filename);
   return(-1);
}

int mocad_read_preconfig_seed_bonding(struct mocad_ctx *ctx) 
{
   // Not supported for DSL_MOCA at this time
   return(0);
}

int mocad_write_preconfig_seed_bonding(struct mocad_ctx *ctx, uint32_t preconfig_seed_bonding) 
{ 
   // Not supported for DSL_MOCA at this time
   return 0;
}



int mocad_send_notification(struct mocad_ctx * ctx, uint32_t id)
{
   CmsRet        ret;
   uint8_t       buf[sizeof(CmsMsgHeader) + CMS_IFNAME_LENGTH];
   CmsMsgHeader *pMsg = (CmsMsgHeader *)&buf[0];

   memset(&buf[0], 0x0, sizeof(buf));

   pMsg->type        = CMS_MSG_MOCA_NOTIFICATION;
   pMsg->src         = MAKE_SPECIFIC_EID(getpid(), EID_MOCAD);
   pMsg->dst         = EID_SSK;
   pMsg->flags_event = 1;
   pMsg->dataLength  = CMS_IFNAME_LENGTH;
   strcpy((char*)(pMsg + 1), ctx->ifname);
   pMsg->wordData    = id;

   ret = cmsMsg_send(ctx->cmsMsgHandle, pMsg);
   if ( CMSRET_SUCCESS != ret )
   {
      mocad_log(ctx, L_WARN, 
         "can't send notification %lu to CMS: error %d\n", id, ret);
      return -1;
   }

   return 0;
}

/* This function updates the ebtables so that PQoS flows are sent
 * to the MoCA core on the right queue. */
int mocad_update_pqos_flow_in_switch(struct mocad_ctx * ctx, 
   struct moca_pqos_table * r, MOCAD_PQOS_SWITCH_ACTIONS_E action,
   int index)
{
   static char command[256];
   int ret = 0;
   char add_del;

   switch (action)
   {
      case MOCAD_PQOS_SWITCH_ADD:
         add_del = 'I';
         break;
      case MOCAD_PQOS_SWITCH_DELETE:
         add_del = 'D';
         break;
      default:
         mocad_log(ctx, L_ERR, "%s: Unknown action %d\n", 
            __FUNCTION__, action);
         return(-1);
   }
   
   ret = sprintf(command, 
                 "ebtables -%c FORWARD %s-p 802_1Q "
                 "-d %02x:%02x:%02x:%02x:%02x:%02x/FF:FF:FF:FF:FF:FF "
                 "-j mark --mark-set 0x3 -o %s",
                 add_del, 
                 (action == MOCAD_PQOS_SWITCH_ADD ? "1 " : ""),
                 r->packet_da.addr[0], r->packet_da.addr[1], r->packet_da.addr[2], 
                 r->packet_da.addr[3], r->packet_da.addr[4], r->packet_da.addr[5],
                 ctx->ifname);

   if (r->vlan_id != MOCA_PQOS_CREATE_FLOW_VLAN_ID_DEF)
   {
      ret += sprintf(&command[ret],
                     " --vlan-id %u",
                     r->vlan_id);
   }
   if (r->vlan_prio != 0xFF)
   {
      ret += sprintf(&command[ret],
                     " --vlan-prio %u",
                     r->vlan_prio);
   }
   
   ret = system(command);

   mocad_log(ctx, L_DEBUG, "Command: %s\n", command);
   mocad_log(ctx, L_DEBUG, "Return value: %d\n", ret);

   return(ret);
}


/* Set up the forwarding tables in the switch so that traffic is routed to the
 * proper moca queues.
 */
void mocad_init_dsl(struct mocad_ctx *ctx)
{
   int i;
   char command[128];
   uint32_t queue_map[8] = {0, 0, 0, 0, 1, 1, 2, 2};

   for (i = 0; i < 8; i++)
   {
      sprintf(command, "ebtables -A FORWARD -p 802_1Q --vlan-prio %u -j mark --mark-set 0x%x -o %s",
              i, queue_map[i], ctx->ifname);

      system(command);
   }
}

void mocad_init_forwarding(struct mocad_ctx *ctx)
{
   return;
}

// end Filesystem operations for #if defined(DSL_MOCA) /* DSL Code */
#elif defined(STANDALONE)

int mocad_read_lof(struct mocad_ctx *ctx) 
{
   unsigned int x;
   unsigned int * tmp;
   unsigned int len;

   ctx->disk_lof = 0;

   x=NVRAM_Lock();

   tmp = NVRAM_GetItem(NVRAM_LOF, &len);     
   if (!tmp)
   {
      NVRAM_Unlock(x);
      /* not a serious failure; it will be missing on the first run */
      mocad_log(ctx, L_VERBOSE, "can't read LOF from NVRAM\n");
      return(-1);
   }
   ctx->disk_lof = *tmp;
   NVRAM_Unlock(x);

   mocad_log(ctx, L_INFO, "Using %d Mhz for LOF\n", ctx->disk_lof);

   return(0);
}

int mocad_write_lof(struct mocad_ctx *ctx, int lof) 
{ 
   unsigned int x;

   ctx->cof = 0;

   if(ctx->disk_lof == lof)
      return(0);

   if(ctx->any_time.lof_update == 0)
      return(0);
   
   mocad_log(ctx, L_VERBOSE, "Saving new LOF %d\n", lof);
   
   ctx->any_time.lof = lof;

   x=NVRAM_Lock();
   NVRAM_SetItem(NVRAM_LOF, &lof, sizeof(lof));
   NVRAM_Unlock(x);

   ctx->disk_lof = lof;

   return 0;
}

int mocad_read_nondefseqnum(struct mocad_ctx *ctx)
{
   unsigned int x;
   unsigned int * tmp;
   unsigned int len;

   ctx->disk_nondefseqnum = 0;

   x=NVRAM_Lock();

   tmp = NVRAM_GetItem(NVRAM_MR_SEQ_NUM, &len);     
   if (!tmp)
   {
      NVRAM_Unlock(x);
      /* not a serious failure; it will be missing on the first run */
      mocad_log(ctx, L_VERBOSE, "can't read MR Sequence Number from NVRAM\n");
      return(-1);
   }
   ctx->disk_nondefseqnum = *tmp;
   NVRAM_Unlock(x);

   return(0);
}

int mocad_write_nondefseqnum (struct mocad_ctx * ctx, int nondefseqnum) 
{ 
   unsigned int x;

   if (ctx->disk_nondefseqnum == nondefseqnum)
      return(0);

   mocad_log(ctx, L_VERBOSE, "Saving new MR Sequence Number %d\n", nondefseqnum);
   
   x=NVRAM_Lock();
   NVRAM_SetItem(NVRAM_MR_SEQ_NUM, &nondefseqnum, sizeof(nondefseqnum));
   NVRAM_Unlock(x);

   ctx->disk_nondefseqnum = nondefseqnum;

   return 0;
}

int mocad_read_e2m(struct mocad_ctx *ctx) { return 0; }
int mocad_write_e2m(struct mocad_ctx *ctx, struct moca_error_to_mask * e2m) { return 0; }

int mocad_read_rf_calib_data(struct mocad_ctx * ctx)
{
   unsigned int x;
   unsigned int * tmp;
   unsigned int len;

   x=NVRAM_Lock();

   tmp = NVRAM_GetItem(NVRAM_RF_CALIB_DATA, &len);     

   if (!tmp || (len != sizeof(ctx->rf_calib_data)))
   {
      NVRAM_Unlock(x);
      /* not a serious failure; it will be missing on the first run */
      mocad_log(ctx, L_VERBOSE, "No RF Calibration Data in NVRAM\n");
      return(-1);
   }
   ctx->rf_calib_data = *(struct moca_rf_calib_data *)tmp;
   NVRAM_Unlock(x);
 
   return 0; 
}

int mocad_write_rf_calib_data(struct mocad_ctx * ctx)
{ 
   unsigned int x;

   mocad_log(ctx, L_VERBOSE, "Saving new RF Calibration value\n");
   
   x=NVRAM_Lock();
   NVRAM_SetItem(NVRAM_RF_CALIB_DATA, (unsigned int *)&ctx->rf_calib_data, sizeof(ctx->rf_calib_data));
   NVRAM_Unlock(x);
   
   return 0;
}

int mocad_read_probe_2_results(struct mocad_ctx * ctx)
{ 
   unsigned int x;
   unsigned int * tmp;
   unsigned int len;
  
   x=NVRAM_Lock();

   tmp = NVRAM_GetItem(NVRAM_PROBE_2_RESULTS, &len);     

   if (!tmp || (len != sizeof(ctx->probe_2_results)))
   {
      NVRAM_Unlock(x);
      /* not a serious failure; it will be missing on the first run */
      mocad_log(ctx, L_VERBOSE, "No Probe 2 Data in NVRAM\n");
      return(-1);
   }
   
   ctx->probe_2_results = *(struct moca_probe_2_results *)tmp;
   NVRAM_Unlock(x);
   
   return 0;
}

int mocad_write_probe_2_results(struct mocad_ctx * ctx)
{ 
   unsigned int x;

   mocad_log(ctx, L_VERBOSE, "Saving new Probe 2 Data\n");
   
   x=NVRAM_Lock();
   NVRAM_SetItem(NVRAM_PROBE_2_RESULTS, (unsigned int *)&ctx->probe_2_results, sizeof(ctx->probe_2_results));
   NVRAM_Unlock(x);
   
   return 0;
}

int mocad_read_preconfig_seed_bonding(struct mocad_ctx *ctx) 
{
   unsigned int x;
   unsigned int * tmp;
   unsigned int len;

   x=NVRAM_Lock();

   tmp = NVRAM_GetItem(NVRAM_PRECONFIG_SEED_BONDING, &len);     
   if (!tmp)
   {
      NVRAM_Unlock(x);
      /* not a serious failure; it will be missing on the first run */
      mocad_log(ctx, L_VERBOSE, "can't read bonding seed from NVRAM\n");
      return(-1);
   }
   ctx->preconfig_seed_bonding = *tmp;
   NVRAM_Unlock(x);

   mocad_log(ctx, L_DEBUG, "Using %d bonding preconfig seed\n", ctx->preconfig_seed_bonding);

   return(0);
}

int mocad_write_preconfig_seed_bonding(struct mocad_ctx *ctx, uint32_t preconfig_seed_bonding) 
{ 
   unsigned int x;
   
   mocad_log(ctx, L_VERBOSE, "Saving new bonding preconfig seed %d\n", preconfig_seed_bonding);
   
   ctx->preconfig_seed_bonding = preconfig_seed_bonding;

   x=NVRAM_Lock();
   NVRAM_SetItem(NVRAM_PRECONFIG_SEED_BONDING, &preconfig_seed_bonding, sizeof(preconfig_seed_bonding));
   NVRAM_Unlock(x);

   return 0;
}


int mocad_send_notification(struct mocad_ctx * ctx, uint32_t id) { return 0; }
int mocad_update_pqos_flow_in_switch(struct mocad_ctx * ctx, 
   struct moca_pqos_table * r, MOCAD_PQOS_SWITCH_ACTIONS_E action,
   int index) { return 0; }
void mocad_init_forwarding(struct mocad_ctx *ctx) {}

// end Filesystem operations for #elif defined(STANDALONE)
// start Filesystem operations for general POSIX open/read/write/close() based systems
#else

int mocad_read_lof(struct mocad_ctx *ctx)
{
   int fd, tmp;
   char filename[MOCA_FILENAME_LEN], buf[16];

   ctx->disk_lof = 0;

   sprintf(filename, LOF_PATH_FMT, ctx->ifname);
   fd = open(filename, O_RDONLY);
   if(fd < 0)
      goto bad;
   if(read(fd, buf, sizeof(buf)) <= 0)
      goto bad;
   buf[15] = '\0';
   if(sscanf(buf, "%d", &tmp) != 1)
      goto bad;
   mocad_log(ctx, L_INFO, "Using %d Mhz for LOF\n", tmp);
   close(fd);
   ctx->disk_lof = tmp;
   return(0);

bad:
   /* not a serious failure; it will be missing on the first run */
   mocad_log(ctx, L_VERBOSE, "can't read LOF file '%s'\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}

int mocad_write_lof(struct mocad_ctx *ctx, int lof)
{
   int fd, len;
   char filename[MOCA_FILENAME_LEN], buf[16];

   ctx->cof = 0;

   if(ctx->disk_lof == lof)
      return(0);

   if(ctx->any_time.lof_update == 0)
      return(0);
   
   mocad_log(ctx, L_VERBOSE, "Saving new LOF %d\n", lof);

   ctx->any_time.lof = lof;

   sprintf(filename, LOF_PATH_FMT, ctx->ifname);
   fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0644);
   if(fd < 0)
      goto bad;
   len = sprintf(buf, "%d\n", lof);
   if(write(fd, buf, len) != len)
      goto bad;
   close(fd);

   ctx->disk_lof = lof;
   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write LOF file '%s'\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}


int mocad_read_nondefseqnum(struct mocad_ctx *ctx)
{
   int fd, tmp;
   char filename[MOCA_FILENAME_LEN], buf[16];

   sprintf(filename, NONDEFSEQNUM_PATH_FMT, ctx->ifname);
   fd = open(filename, O_RDONLY);
   if(fd < 0)
      goto bad;
   if(read(fd, buf, sizeof(buf)) <= 0)
      goto bad;
  buf[15] = '\0';
   if(sscanf(buf, "%d", &tmp) != 1)
      goto bad;
   close(fd);
   ctx->disk_nondefseqnum = tmp;
   return(0);

bad:
        mocad_log(ctx, L_VERBOSE, "can't read nondefseqnum file '%s'.  Using default.\n", filename);
   /* not a serious failure; it will be missing on the first run */
   if(fd >= 0)
      close(fd);
   return(-1);
}

int mocad_write_nondefseqnum (struct mocad_ctx * ctx, int nondefseqnum)
{
   int fd, len;
   char filename[MOCA_FILENAME_LEN], buf[16];

   if (ctx->disk_nondefseqnum == nondefseqnum)
      return(0);

   sprintf(filename, NONDEFSEQNUM_PATH_FMT, ctx->ifname);
   fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0644);
   if(fd < 0)
      goto bad;
   len = sprintf(buf, "%d\n", nondefseqnum);
   if(write(fd, buf, len) != len)
      goto bad;
   close(fd);

   ctx->disk_nondefseqnum = nondefseqnum;
   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write NONDEFSEQNUM file '%s'\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}

int mocad_read_e2m(struct mocad_ctx *ctx)
{
   int fd, tmp[3];
   char filename[MOCA_FILENAME_LEN], buf[36];

   sprintf(filename, E2M_PATH_FMT, ctx->ifname);
   fd = open(filename, O_RDONLY);
   if(fd < 0)
      goto bad;
   if(read(fd, buf, sizeof(buf)) <= 0)
      goto bad;
  buf[15] = '\0';
   if(sscanf(buf, "%d %d %d", &tmp[0], &tmp[1], &tmp[2]) != 3)
      goto bad;
   mocad_log(ctx, L_DEBUG, "E2M data: %d %d %d\n", tmp[0], tmp[1], tmp[2]);
   memcpy(&ctx->error_to_mask, tmp, sizeof(ctx->error_to_mask));
   close(fd);
   return(0);

bad:
   /* not a serious failure; it will be missing on the first run */
   mocad_log(ctx, L_VERBOSE, "can't read E2M file '%s'.  Using default.\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}

int mocad_write_e2m(struct mocad_ctx *ctx, struct moca_error_to_mask * e2m)
{
   int fd, len;
   char filename[MOCA_FILENAME_LEN], buf[36];

   mocad_log(ctx, L_VERBOSE, "Saving new E2M %d %d %d\n", 
      e2m->error1, e2m->error2, e2m->error3);

   sprintf(filename, E2M_PATH_FMT, ctx->ifname);
   fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0644);
   if(fd < 0)
      goto bad;
   len = sprintf(buf, "%i %i %i\n", (int)e2m->error1, (int)e2m->error2, (int)e2m->error3);
   if(write(fd, buf, len) != len)
      goto bad;
   close(fd);

   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write E2M file '%s'\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}

int mocad_read_rf_calib_data(struct mocad_ctx * ctx)
{
   int fd, len;
   char filename[MOCA_FILENAME_LEN];

   sprintf(filename, RF_CALIB_DATA_PATH_FMT, ctx->ifname);
   fd = open(filename, O_RDONLY);
   if(fd < 0)
      goto bad;

   len = sizeof(ctx->rf_calib_data);
   if(read(fd, &ctx->rf_calib_data, len) <= 0)
      goto bad;

   mocad_log(ctx, L_INFO, "Found valid RF Calib Data file\n");

   close(fd);
   
   return(0);

bad:
   /* not a serious failure; it will be missing on the first run */
   mocad_log(ctx, L_VERBOSE, "can't read RF Calib Data file '%s'\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}

int mocad_write_rf_calib_data(struct mocad_ctx * ctx)
{
   int fd, len;
   char filename[MOCA_FILENAME_LEN];

   sprintf(filename, RF_CALIB_DATA_PATH_FMT, ctx->ifname);
   fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0644);
   if(fd < 0)
      goto bad;

   len = sizeof(ctx->rf_calib_data);

   mocad_log(ctx, L_VERBOSE, "Saving new RF Calibration Data %d (%d bytes)\n", ctx->rf_calib_data.valid, len);

   if(write(fd, &ctx->rf_calib_data, len) != len)
      goto bad;
   close(fd);

   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write RF Cal Data file '%s'\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}

int mocad_read_probe_2_results(struct mocad_ctx * ctx)
{
   int fd, len;
   char filename[MOCA_FILENAME_LEN];

   sprintf(filename, PROBE_2_RESULTS_PATH_FMT, ctx->ifname);
   fd = open(filename, O_RDONLY);
   if(fd < 0)
      goto bad;

   len = sizeof(ctx->probe_2_results);
   if(read(fd, &ctx->probe_2_results, len) <= 0)
      goto bad;

   mocad_log(ctx, L_INFO, "Found valid Probe 2 Results file\n");

   close(fd);
   
   return(0);

bad:
   /* not a serious failure; it will be missing on the first run */
   mocad_log(ctx, L_VERBOSE, "can't read Probe 2 Results file '%s'\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}

int mocad_write_probe_2_results(struct mocad_ctx * ctx)
{
   int fd, len;
   char filename[MOCA_FILENAME_LEN];

   mocad_log(ctx, L_VERBOSE, "Saving new Probe 2 Data\n");

   sprintf(filename, PROBE_2_RESULTS_PATH_FMT, ctx->ifname);
   fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0644);
   if(fd < 0)
      goto bad;

   len = sizeof(ctx->probe_2_results);
   if(write(fd, &ctx->probe_2_results, len) != len)
      goto bad;
   close(fd);

   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write Probe 2 Results file '%s'\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}


int mocad_read_preconfig_seed_bonding(struct mocad_ctx *ctx) 
{
   int fd, tmp;
   char filename[MOCA_FILENAME_LEN], buf[16];

   sprintf(filename, PRECFG_SEED_BND_PATH_FMT, ctx->ifname);
   fd = open(filename, O_RDONLY);
   if(fd < 0)
      goto bad;
   if(read(fd, buf, sizeof(buf)) <= 0)
      goto bad;
   buf[15] = '\0';
   if(sscanf(buf, "%d", &tmp) != 1)
      goto bad;
   close(fd);
   ctx->preconfig_seed_bonding = tmp;
   return(0);

bad:
   mocad_log(ctx, L_VERBOSE, "can't read bonding preconfig seed file '%s'.  Using default.\n", filename);
   /* not a serious failure; it will be missing on the first run */
   if(fd >= 0)
      close(fd);
   return(-1);
}

int mocad_write_preconfig_seed_bonding(struct mocad_ctx *ctx, uint32_t preconfig_seed_bonding) 
{
   int fd, len;
   char filename[MOCA_FILENAME_LEN], buf[16];

   if (ctx->preconfig_seed_bonding == preconfig_seed_bonding)
      return(0);

   sprintf(filename, PRECFG_SEED_BND_PATH_FMT, ctx->ifname);
   fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0644);
   if(fd < 0)
      goto bad;
   len = sprintf(buf, "%d\n", preconfig_seed_bonding);
   if(write(fd, buf, len) != len)
      goto bad;
   close(fd);

   ctx->preconfig_seed_bonding = preconfig_seed_bonding;
   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write bonding preconfig seed file '%s'\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}


\

int mocad_send_notification(struct mocad_ctx * ctx, uint32_t id)
{
   /* Do nothing in STB case */
   return(0);
}

int mocad_update_pqos_flow_in_switch(struct mocad_ctx * ctx, 
   struct moca_pqos_table * r, MOCAD_PQOS_SWITCH_ACTIONS_E action,
   int index)
{
   int ret = 0;
#if defined(__gnu_linux__)
   static char command[256];
   static char prio_match[2][32];
   static char protocol[2][8];
   static char hash[2][8];
   uint32_t pref[2];
   uint32_t num_rules = 1;
   uint32_t i;

   switch (r->ingr_class_rule)
   {
      case MOCA_PQOS_INGR_CLASS_RULE_DA_VLAN:
         sprintf(protocol[0], "802.1q");
         sprintf(prio_match[0], "match u16 0x%04X 0xE000 at 0", (r->vlan_prio << 13));
         sprintf(hash[0], MOCAD_PQOS_VLAN_FILTER_HASH);
         pref[0] = 1;
         num_rules = 1;
         break;

      case MOCA_PQOS_INGR_CLASS_RULE_DA_VLAN_4_5:
         sprintf(protocol[0], "802.1q");
         sprintf(prio_match[0], "match u16 0x8000 0xC000 at 0");
         sprintf(hash[0], MOCAD_PQOS_VLAN_FILTER_HASH);
         pref[0] = 1;
         num_rules = 1;
         break;

      case MOCA_PQOS_INGR_CLASS_RULE_DA_DSCP:
         sprintf(protocol[0], "ip");
         sprintf(prio_match[0], "match ip tos 0x%02X 0xE0", (r->dscp_moca << 5));
         sprintf(hash[0], MOCAD_PQOS_DSCP_FILTER_HASH);
         pref[0] = 3;
         num_rules = 1;
         break;

      case MOCA_PQOS_INGR_CLASS_RULE_DA_VLAN_DSCP:
         // Need to add/delete two rules for this 
         sprintf(protocol[0], "802.1q");
         sprintf(prio_match[0], "match u16 0x%04X 0xE000 at 0", (r->vlan_prio << 13));
         sprintf(hash[0], MOCAD_PQOS_VLAN_FILTER_HASH);
         pref[0] = 1;

         sprintf(protocol[1], "ip");
         sprintf(prio_match[1], "match ip tos 0x%02X 0xE0", (r->dscp_moca << 5));
         sprintf(hash[1], MOCAD_PQOS_DSCP_FILTER_HASH);
         pref[1] = 3;

         num_rules = 2;
         break;

      case MOCA_PQOS_INGR_CLASS_RULE_DA_ONLY:
      default:
         sprintf(protocol[0], "802.1q");
         sprintf(hash[0], MOCAD_PQOS_VLAN_FILTER_HASH);
         pref[0] = 1;
         prio_match[0][0] = 0x0;
         num_rules = 1;
         break;
   }

   for (i = 0; i < num_rules; i++)
   {
      switch (action)
      {
         case MOCAD_PQOS_SWITCH_ADD:
            ret = sprintf(command,
                 "tc filter add dev %s parent 1: protocol %s handle 0x%x "
                 "pref %u u32 ht %s: "
                 "match u16 0x%02X%02X 0xFFFF at -10 "
                 "match u16 0x%02X%02X 0xFFFF at -12 "
                 "match u16 0x%02X%02X 0xFFFF at -14 "
                 "%s action skbedit queue_mapping 1 ", //2> /dev/null",
                 ctx->ifname, protocol[i], (index + 1), pref[i], hash[i],
                 r->packet_da.addr[4], r->packet_da.addr[5],
                 r->packet_da.addr[2], r->packet_da.addr[3],
                 r->packet_da.addr[0], r->packet_da.addr[1],
                 prio_match[i]);
             break;
         case MOCAD_PQOS_SWITCH_DELETE:
             // Need to specify the handle otherwise all rules for the protocol
             // will be deleted.
             ret = sprintf(command,
                 "tc filter del dev %s parent 1: protocol %s handle %s:%x "
                 "pref %u u32 2> /dev/null",
                 ctx->ifname, protocol[i], hash[i], (index + 1), pref[i]);
             break;
         default:
             mocad_log(ctx, L_ERR, "%s: Unknown action %d\n",
            __FUNCTION__, action);
             return(-1);
      }
   
      ret = system(command);
   
      mocad_log(ctx, L_DEBUG, "Command: %s\n", command);
      mocad_log(ctx, L_DEBUG, "Return value: %d\n", ret);
   
      if (ret != 0)
      {
         mocad_log(ctx, L_WARN, "'%s' returned %d\n", command, ret);
         return(ret);
      }
   }
#endif
   return(ret);
}

void mocad_init_forwarding(struct mocad_ctx *ctx)
{
#if defined(__gnu_linux__)
   int i;
   char command[256];
   uint32_t queue_map[8] = {4, 0, 0, 4, 3, 3, 2, 2};  // genet driver maps queue 0 to ring 16
   int ret;
   static int once = 1;

   // delete any qos associated with the interface
   sprintf(command, "tc qdisc del dev %s root 2> /dev/null", ctx->ifname);
   ret = system(command);

   mocad_log(ctx, L_DEBUG, "Command: %s\n", command);
   mocad_log(ctx, L_DEBUG, "Return value: %d\n", ret);

   // root queueing discipline is PRIO queueing
   sprintf(command, "tc qdisc add dev %s root handle 1: multiq 2> /dev/null",
      ctx->ifname);
   ret = system(command);
   mocad_log(ctx, L_DEBUG, "Command: %s\n", command);
   mocad_log(ctx, L_DEBUG, "Return value: %d\n", ret);

   if ((ret) && (once))
   {
      mocad_log(ctx, L_WARN, 
         "Warning:  MoCA cannot configure host-side transmit queues for QOS.\n"
         "Continuing with non-compliant queuing behaviour.  Some CTPs may fail.\n"
         "The kernel must have QOS and MULTIQ configured, and the tc utility \n"
         "must be in the path for spec-compliant operation.\n"
         "See MoCA_Diagnostics.pdf for more details\n");
      once = 0;
      return;
   }

   // set up vlan priority filters
   for (i = 0; i < 8; i++)
   {
      sprintf(command, "tc filter add dev %s parent 1: protocol 802.1q pref 2 u32 match u8 0x%02X 0xe0 at 0 action skbedit queue_mapping %d",
        ctx->ifname, i << 5, (int)queue_map[i]);
      ret = system(command);
      mocad_log(ctx, L_DEBUG, "Command: %s\n", command);
      mocad_log(ctx, L_DEBUG, "Return value: %d\n", ret);
   }

   // Add a place holder entry so that the hash value is predictable (801)
   // Use our own MAC address as we should never have to send to it.
   // We need the hash value in order to be able to delete specific rules
   // only when PQOS flows are deleted.
   sprintf(command,
      "tc filter add dev %s parent 1: protocol 802.1q handle 0x%x "
      "pref 1 u32 ht %s: "
      "match u16 0x%02X%02X 0xFFFF at -10 "
      "match u16 0x%02X%02X 0xFFFF at -12 "
      "match u16 0x%02X%02X 0xFFFF at -14 action skbedit queue_mapping 1",
      ctx->ifname, (MOCA_MAX_PQOS_ENTRIES+1), MOCAD_PQOS_VLAN_FILTER_HASH,
      (ctx->kdrv_info.macaddr_lo >> 24) & 0xff,
      (ctx->kdrv_info.macaddr_lo >> 16) & 0xff,
      (ctx->kdrv_info.macaddr_hi >>  8) & 0xff,
      (ctx->kdrv_info.macaddr_hi      ) & 0xff,
      (ctx->kdrv_info.macaddr_hi >> 24) & 0xff,
      (ctx->kdrv_info.macaddr_hi >> 16) & 0xff);

   mocad_log(ctx, L_DEBUG, "Command: %s\n", command);
   ret = system(command);
   mocad_log(ctx, L_DEBUG, "Return value: %d\n", ret);

   // set up pQoS priority filters
   for (i=0;i<MOCA_MAX_PQOS_ENTRIES;i++)
   {
      if (!MOCA_MACADDR_IS_NULL(&ctx->pqos_table[i].flow_id))
      {
         if (ctx->any_time.host_qos)
            mocad_update_pqos_flow_in_switch(ctx, &ctx->pqos_table[i], MOCAD_PQOS_SWITCH_ADD, i);
      }
   }

   ////////////////////////////////////////////////////////
   // Set up DSCP priority filters
   ////////////////////////////////////////////////////////
   sprintf(command,
      "tc filter add dev %s parent 1: protocol ip pref 4 u32",
      ctx->ifname);

   for (i = 0; i < 8; i++)
   {
      sprintf(command, "tc filter add dev %s parent 1: protocol ip pref 3 u32 match u8 0x%02X 0xe0 at 1 action skbedit queue_mapping %d",
        ctx->ifname, i << 5, (int)queue_map[i]);
      ret = system(command);

      mocad_log(ctx, L_DEBUG, "Command: %s\n", command);
      mocad_log(ctx, L_DEBUG, "Return value: %d\n", ret);
   }

   sprintf(command,
      "tc filter add dev %s parent 1: protocol ip handle 0x%x "
      "pref 3 u32 ht %s: "
      "match u16 0x%02X%02X 0xFFFF at -10 "
      "match u16 0x%02X%02X 0xFFFF at -12 "
      "match u16 0x%02X%02X 0xFFFF at -14 action skbedit queue_mapping 1",
      ctx->ifname, (MOCA_MAX_PQOS_ENTRIES+1), MOCAD_PQOS_DSCP_FILTER_HASH,
      (ctx->kdrv_info.macaddr_lo >> 24) & 0xff,
      (ctx->kdrv_info.macaddr_lo >> 16) & 0xff,
      (ctx->kdrv_info.macaddr_hi >>  8) & 0xff,
      (ctx->kdrv_info.macaddr_hi      ) & 0xff,
      (ctx->kdrv_info.macaddr_hi >> 24) & 0xff,
      (ctx->kdrv_info.macaddr_hi >> 16) & 0xff);

   mocad_log(ctx, L_DEBUG, "Command: %s\n", command);
   ret = system(command);
   mocad_log(ctx, L_DEBUG, "Return value: %d\n", ret);

   // By default, packets without a vlan tag or dscp field should be treated as
   // low priority. Add a catch-all rule to map to queue 4 at the end.
   sprintf(command,
      "tc filter add dev %s parent 1: protocol all u32 "
      "match u8 0x00 0x00 at 0 "
      "action skbedit queue_mapping 4",
      ctx->ifname);

   mocad_log(ctx, L_DEBUG, "Command: %s\n", command);
   ret = system(command);
   mocad_log(ctx, L_DEBUG, "Return value: %d\n", ret);

#endif    
}

// end blocks of Filesystem operations for each platform
#endif

int mocad_write_pidfile(struct mocad_ctx *ctx)
{
#if defined(__gnu_linux__)

#if defined(DSL_MOCA) /* DSL Code */
   return(0);
#else
   int fd, len, pid = getpid();
   char filename[MOCA_FILENAME_LEN], buf[16];

   sprintf(filename, ctx->pidfile, ctx->ifname);
   fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
   if(fd < 0)
      goto bad;
   len = sprintf(buf, "%d\n", pid);
   if(write(fd, buf, len) != len)
      goto bad;
   close(fd);

   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write PID file '%s': %s\n", filename,
      strerror(errno));
   if(fd >= 0)
      close(fd);
   return(-1);
#endif

#else
   return(0);
#endif
}

MOCAD_S static void printNodeErrorInfo(struct mocad_ctx *ctx, int node, 
   struct moca_node_stats_ext * node_stats_ext)
{

   if ((ctx->last_node_stats_ext[node].rx_beacon_crc_error   < node_stats_ext->rx_beacon_crc_error) ||
       (ctx->last_node_stats_ext[node].rx_map_crc_error      < node_stats_ext->rx_map_crc_error)    ||
       (ctx->last_node_stats_ext[node].rx_rr_crc_error       < node_stats_ext->rx_rr_crc_error)     ||
       (ctx->last_node_stats_ext[node].rx_lc_uc_crc_error    < node_stats_ext->rx_lc_uc_crc_error)  ||
       (ctx->last_node_stats_ext[node].rx_lc_bc_crc_error    < node_stats_ext->rx_lc_bc_crc_error)  ||
       (ctx->last_node_stats_ext[node].rx_uc_crc_error       < node_stats_ext->rx_uc_crc_error)     ||
       (ctx->last_node_stats_ext[node].rx_uc_crc_error_sec_ch  < node_stats_ext->rx_uc_crc_error_sec_ch)  ||
       (ctx->last_node_stats_ext[node].rx_bc_crc_error       < node_stats_ext->rx_bc_crc_error)     ||
       (ctx->last_node_stats_ext[node].rx_plp_crc_error      < node_stats_ext->rx_plp_crc_error)    ||
       (ctx->last_node_stats_ext[node].rx_ofdma_rr_crc_error < node_stats_ext->rx_ofdma_rr_crc_error))
   {
      if (ctx->in_lab_printf)
      {
         mocad_log(ctx, L_ALWAYS, "\n");
         ctx->in_lab_printf = 0;
      }
         
      mocad_log(ctx, L_ALWAYS, "MOCA: @ CRC:  Node=%02d,Bcn=%02d,MAP=%02d,RR=%02d,LC_UC=%02d,LC_BC=%02d,UC_Eth=%04d,CB_1=%04d,BC_Eth=%04d,PLP=%04d,OfdRR=%04d\n",
         node, 
         node_stats_ext->rx_beacon_crc_error - ctx->last_node_stats_ext[node].rx_beacon_crc_error, 
         node_stats_ext->rx_map_crc_error - ctx->last_node_stats_ext[node].rx_map_crc_error,
         node_stats_ext->rx_rr_crc_error - ctx->last_node_stats_ext[node].rx_rr_crc_error,
         node_stats_ext->rx_lc_uc_crc_error - ctx->last_node_stats_ext[node].rx_lc_uc_crc_error,
         node_stats_ext->rx_lc_bc_crc_error - ctx->last_node_stats_ext[node].rx_lc_bc_crc_error,
         node_stats_ext->rx_uc_crc_error - ctx->last_node_stats_ext[node].rx_uc_crc_error,
         node_stats_ext->rx_uc_crc_error_sec_ch - ctx->last_node_stats_ext[node].rx_uc_crc_error_sec_ch,
         node_stats_ext->rx_bc_crc_error - ctx->last_node_stats_ext[node].rx_bc_crc_error,
         node_stats_ext->rx_plp_crc_error - ctx->last_node_stats_ext[node].rx_plp_crc_error,
         node_stats_ext->rx_ofdma_rr_crc_error - ctx->last_node_stats_ext[node].rx_ofdma_rr_crc_error);
   }
      
   if ((ctx->last_node_stats_ext[node].rx_beacon_timeout_error < node_stats_ext->rx_beacon_timeout_error)  ||
       (ctx->last_node_stats_ext[node].rx_map_timeout_error    < node_stats_ext->rx_map_timeout_error)     ||
       (ctx->last_node_stats_ext[node].rx_rr_timeout_error     < node_stats_ext->rx_rr_timeout_error)      ||
       (ctx->last_node_stats_ext[node].rx_lc_uc_timeout_error  < node_stats_ext->rx_lc_uc_timeout_error)   ||
       (ctx->last_node_stats_ext[node].rx_lc_bc_timeout_error  < node_stats_ext->rx_lc_bc_timeout_error)   ||
       (ctx->last_node_stats_ext[node].rx_uc_timeout_error     < node_stats_ext->rx_uc_timeout_error)      ||
       (ctx->last_node_stats_ext[node].rx_uc_timeout_error_sec_ch    < node_stats_ext->rx_uc_timeout_error_sec_ch)   ||
       (ctx->last_node_stats_ext[node].rx_bc_timeout_error     < node_stats_ext->rx_bc_timeout_error)      ||
       (ctx->last_node_stats_ext[node].rx_probe1_error         < node_stats_ext->rx_probe1_error)          ||
       (ctx->last_node_stats_ext[node].rx_probe1_error_sec_ch        < node_stats_ext->rx_probe1_error_sec_ch)       ||
       (ctx->last_node_stats_ext[node].rx_probe2_error         < node_stats_ext->rx_probe2_error)          ||
       (ctx->last_node_stats_ext[node].rx_probe3_error         < node_stats_ext->rx_probe3_error)          ||
       (ctx->last_node_stats_ext[node].rx_probe1_gcd_error     < node_stats_ext->rx_probe1_gcd_error)      ||
       (ctx->last_node_stats_ext[node].rx_plp_timeout_error    < node_stats_ext->rx_plp_timeout_error))
   {
      if (ctx->in_lab_printf)
      {
         mocad_log(ctx, L_ALWAYS, "\n");
         ctx->in_lab_printf = 0;
      }
      mocad_log(ctx, L_ALWAYS, "MOCA: @ TIMEOUT: Node=%02d, Bcn=%02d, MAP=%02d, RR=%02d, LC_UC=%02d, LC_BC=%02d, UC_Eth=%04d, CB_0=%04d, CB_1=%04d, BC_Eth=%04d, P1_0=%02d, P1_1=%02d, P2=%02d, P3=%02d, P1g=%02d, PLP=%04d\n",
         node, 
         node_stats_ext->rx_beacon_timeout_error - ctx->last_node_stats_ext[node].rx_beacon_timeout_error,
         node_stats_ext->rx_map_timeout_error - ctx->last_node_stats_ext[node].rx_map_timeout_error,
         node_stats_ext->rx_rr_timeout_error - ctx->last_node_stats_ext[node].rx_rr_timeout_error,
         node_stats_ext->rx_lc_uc_timeout_error - ctx->last_node_stats_ext[node].rx_lc_uc_timeout_error,
         node_stats_ext->rx_lc_bc_timeout_error - ctx->last_node_stats_ext[node].rx_lc_bc_timeout_error,
         node_stats_ext->rx_uc_timeout_error - ctx->last_node_stats_ext[node].rx_uc_timeout_error,
         node_stats_ext->rx_uc_timeout_error_sec_ch - ctx->last_node_stats_ext[node].rx_uc_timeout_error_sec_ch,
         node_stats_ext->rx_bc_timeout_error - ctx->last_node_stats_ext[node].rx_bc_timeout_error,
         node_stats_ext->rx_probe1_error - ctx->last_node_stats_ext[node].rx_probe1_error,      
         node_stats_ext->rx_probe1_error_sec_ch - ctx->last_node_stats_ext[node].rx_probe1_error_sec_ch,      
         node_stats_ext->rx_probe2_error - ctx->last_node_stats_ext[node].rx_probe2_error,
         node_stats_ext->rx_probe3_error - ctx->last_node_stats_ext[node].rx_probe3_error,
         node_stats_ext->rx_probe1_gcd_error - ctx->last_node_stats_ext[node].rx_probe1_gcd_error,
         node_stats_ext->rx_plp_timeout_error - ctx->last_node_stats_ext[node].rx_plp_timeout_error);
   }
}


MOCAD_S void mocad_check_node_stats_ext(struct mocad_ctx *ctx, int altnodemask)
{
   struct moca_node_stats_ext_in in;
   struct moca_node_stats_ext node_stats_ext;

   memset(&in, 0, sizeof(in));

   while (in.index < MOCA_MAX_NODES)
   {
      if ((altnodemask & (1<<in.index)) != 0) 
      {
         moca_get_node_stats_ext(ctx, &in, &node_stats_ext);

         if (ctx->show_lab_printf)
            printNodeErrorInfo(ctx, in.index, &node_stats_ext);

         memcpy(&ctx->last_node_stats_ext[in.index], &node_stats_ext, 
            sizeof(ctx->last_node_stats_ext[in.index]));
      }

      in.index++;
   }
}


MOCAD_S void mocad_nv_cal_clear(struct mocad_ctx *ctx)
{
   /* Clear the RF Calibration data and the Probe II result data */
   memset(&ctx->rf_calib_data, 0, sizeof(ctx->rf_calib_data));
   mocad_write_rf_calib_data(ctx);

   memset(&ctx->probe_2_results, 0, sizeof(ctx->probe_2_results));
   mocad_write_probe_2_results(ctx);
}

MOCAD_S void mocad_reset_stats(struct mocad_ctx *ctx)
{
   struct moca_node_stats_in node_stats_in;
   struct moca_node_stats_ext_in node_stats_ext_in;
   
   ctx->in_octets = 0;
   ctx->out_octets = 0;
   
#ifndef STANDALONE
   {
      int i;

      for (i=0;i<MOCA_MAX_NODES;i++)
         memset(ctx->cir_data[i], 0, CIR_DATA_SIZE);
   }
#endif

   /* Reset the our base stats */
   if (ctx->moca_running)
   {
      moca_get_gen_stats(ctx, 0, &ctx->gen_stats_base);

      node_stats_in.reset_stats = 0;
      for (node_stats_in.index = 0; 
           node_stats_in.index < MOCA_MAX_NODES; 
           node_stats_in.index++)
      {
         moca_get_node_stats(ctx, &node_stats_in, 
            &ctx->node_stats_base[node_stats_in.index]);
      }

      node_stats_ext_in.reset_stats = 0;
      for (node_stats_ext_in.index = 0; 
           node_stats_ext_in.index < MOCA_MAX_NODES; 
           node_stats_ext_in.index++)
      {
         moca_get_node_stats_ext(ctx, &node_stats_ext_in, 
            &ctx->node_stats_ext_base[node_stats_ext_in.index]);
         memcpy(&ctx->last_node_stats_ext[node_stats_ext_in.index],
                &ctx->node_stats_ext_base[node_stats_ext_in.index],
                sizeof(ctx->node_stats_ext_base[node_stats_ext_in.index]));
}
   }
   else
   {
      mocad_clear_base_stats(ctx);
   }
}

/*
 * MMP
 */
MOCAD_S int mocad_handle_trap(struct mocad_ctx *ctx, int trap_len);

MOCAD_S int mocad_add_trap(struct mocad_ctx *ctx, uint8_t * trap, int len)
{
   struct mmp_msg_hdr *mh = (struct mmp_msg_hdr *)trap;

   /* Handle traps that will reset the core immediately */
   switch (BE32(mh->ie_type))
   {
      case IE_ASSERT:
      case IE_MIPS_EXCEPTION:
      case IE_WDT:
         MoCAOS_EnableDataIf(ctx->os_handle, ctx->ifname, 2);
         memcpy(ctx->trap_buf, trap, len);
         mocad_log(ctx, L_DEBUG, "\n\nHandling trap 0x%x immediately\n\n\n",
            BE32(mh->ie_type));
         return(mocad_handle_trap(ctx, len));
      default:
         break;
   }
   
   if (len > (int)sizeof(ctx->drv_in))
   {
      mocad_log(ctx, L_WARN, "%s: called with len %u\n",
         __FUNCTION__, len);
      return(-1);
   }

   if (ctx->deferred_end >= ctx->deferred_start) 
   {
      if (ctx->deferred_end+4+len >= &ctx->deferred_traps[MOCAD_DEFTRAP_LEN])
      {
         int remaining;
         //wrap
         remaining = &ctx->deferred_traps[MOCAD_DEFTRAP_LEN] - ctx->deferred_end - 4;
  
         if (ctx->deferred_traps + len - remaining >= ctx->deferred_start)
         {
            if (!ctx->silentlyDropTraps)
               mocad_log(ctx, L_WARN, "Dropping trap 0x%x (len %u)\n",
                  BE32(mh->ie_type), len);
            return(-1);
         }
 
         *(unsigned int *)ctx->deferred_end = len;
         ctx->deferred_end += 4;
         if (remaining)
            memcpy(ctx->deferred_end, trap, remaining);
         memcpy(ctx->deferred_traps, trap+remaining, len - remaining);
         ctx->deferred_end = ctx->deferred_traps + len - remaining;
      }
      else
      {
         *(unsigned int *)ctx->deferred_end = len;
         memcpy(ctx->deferred_end+4, trap, len);
         ctx->deferred_end += 4 + len;
      }
   }
   else
   {  // end < start
      if (ctx->deferred_end + 4 + len < ctx->deferred_start)
      {
         *(unsigned int *)ctx->deferred_end = len;
         memcpy(ctx->deferred_end+4, trap, len);
         ctx->deferred_end += 4 + len;
      }
      else
      {
         if (!ctx->silentlyDropTraps)
            mocad_log(ctx, L_WARN, "Dropping trap 0x%x (len %u base=%x star=%x end=%x).\n",
               BE32(mh->ie_type), len, ctx->deferred_traps, ctx->deferred_start, ctx->deferred_end);
         return(-1);
      }
   }
   
   ctx->trap_count++;

   if (ctx->trap_count > ctx->trap_watermark)
      ctx->trap_watermark = ctx->trap_count;

   mocad_log(ctx, L_DEBUG, 
      "Deferring trap 0x%x (len %d) - #%u (%u)\n",
      BE32(mh->ie_type), 
      len, ctx->trap_count,
      ctx->trap_watermark);

   return(0);
}

MOCAD_S int mocad_get_trap(struct mocad_ctx *ctx, uint8_t *trap, unsigned int buffer_len)
{
   unsigned int len;

   if (ctx->deferred_end == ctx->deferred_start)
      return(-1);

   len = *(unsigned int *) ctx->deferred_start;
   if (len > buffer_len)
   {
      mocad_log(ctx, L_WARN, "Large trap encountered.  Flushing list (len = %u).\n",
         len);
      ctx->deferred_start = ctx->deferred_end = ctx->deferred_traps;
      ctx->trap_count = 0;
      return(-1);
   }

   ctx->deferred_start += 4;
   if (ctx->deferred_start + len < &ctx->deferred_traps[MOCAD_DEFTRAP_LEN])
   {
       memcpy(trap, ctx->deferred_start, len);
       ctx->deferred_start += len;
   }
   else
   {
      //wrap
      unsigned int remaining;
      remaining = &ctx->deferred_traps[MOCAD_DEFTRAP_LEN] - ctx->deferred_start;
      memcpy(trap, ctx->deferred_start, remaining);
      trap += remaining;
      memcpy(trap, ctx->deferred_traps, len-remaining);
      ctx->deferred_start = ctx->deferred_traps + len - remaining;
   }
   return(len);
}

MOCAD_S static int mocad_do_hostless_mode(void * vctx, uint32_t enable)
{
   struct mocad_ctx *ctx = (struct mocad_ctx *)vctx;
   int ret = 0;

   if ((ctx->do_flags & MOCAD_HOSTLESS_MODE) != 0)
   {
      mocad_log(ctx, L_WARN,
          "Hostless mode setting already in progress.\n");
      return(ctx->do_flags);
   }

   ctx->do_flags |= MOCAD_HOSTLESS_MODE;

   mocad_log(ctx, L_INFO, "Hostless-mode set to %d\n", enable);

   // reset watchdog
   ctx->lab_printf_wdog_count = 1;
   ctx->hostless_mode = enable;
   ret = moca_set_hostless_mode_request(vctx, enable);

   if (!enable)
   {
      struct moca_interface_status intf;

      if (MOCA_API_SUCCESS == moca_get_interface_status(ctx, &intf))
      {
      if ((int)intf.link_status != ctx->link_state )
         mocad_update_link_state(ctx, intf.link_status);
   }
   }

   return ret;
}

MOCAD_S static int mocad_do_wakeup_node(void * vctx, void *data)
{
   struct mocad_ctx *ctx = (struct mocad_ctx *)vctx;
   int ret = 0;
   uint32_t node = BE32(*(uint32_t *)data);

   if ((ctx->do_flags & MOCAD_WAKEUP_NODE) != 0)
   {
      mocad_log(ctx, L_WARN,
          "Wakeup_node already in progress.\n");
      return(ctx->do_flags);
   }

   ctx->do_flags |= MOCAD_WAKEUP_NODE;

   mocad_log(ctx, L_INFO, "Waking up node %d\n", node);

   ret = moca_set_wakeup_node_request(vctx, node);

   return ret;
}

MOCAD_S int mocad_set_loopback_en(void * vctx, uint32_t enable)
{
   int ret = 0;
   struct mocad_ctx *ctx = (struct mocad_ctx *)vctx;

   ret = moca_set_loopback_en(vctx, enable);

   MoCAOS_SetLoopback(ctx->os_handle, enable, ctx->kdrv_info.device_id);

   return ret;
}

MOCAD_S int mocad_set_brcmtag_enable(void * vctx, uint32_t enable)
{
   int ret = 0;
   struct moca_stag_priority prio;
   struct moca_stag_removal rem;

   memset(&prio, 0, sizeof(prio));
   memset(&rem, 0, sizeof(rem));

   if (!enable)
   {
      prio.tag_mask = 0;
      ret = moca_set_stag_priority(vctx, &prio);
      if (ret) return ret;
      ret = moca_set_stag_removal(vctx, &rem);
      if (ret) return ret;
   }
   else
   {
      prio.enable = 1;

      prio.tag_mask = 0xE;
      prio.tag_priority_0  = 0;
      prio.moca_priority_0 = 4;
      prio.tag_priority_1  = 1;
      prio.moca_priority_1 = 0;
      prio.tag_priority_2  = 2;
      prio.moca_priority_2 = 1;
      prio.tag_priority_3  = 3;
      prio.moca_priority_3 = 1;
      prio.tag_priority_4  = 4;
      prio.moca_priority_4 = 2;
      prio.tag_priority_5  = 5;
      prio.moca_priority_5 = 2;
      prio.tag_priority_6  = 6;
      prio.moca_priority_6 = 3;
      prio.tag_priority_7  = 7;
      prio.moca_priority_7 = 3;

      ret = moca_set_stag_priority(vctx, &prio);
      if (ret) return ret;

      rem.enable = 1;
      rem.valid_0 = 1;
      rem.value_0 = 0;
      rem.mask_0 = 0xFFFFFF;

      ret = moca_set_stag_removal(vctx, &rem);
      if (ret) return ret;
   }

   return (0);
}

MOCAD_S int mocad_set_mr_seq_num(void * vctx, uint32_t mr_seq_num)
{
   int ret = 0;
   struct mocad_ctx *ctx = (struct mocad_ctx *)vctx;

   ret = __moca_non_def_seq_num_check(BE32(mr_seq_num), ctx->config_flags);
   if (ret != 0)
   {
      mocad_log(ctx, L_WARN, "MR Sequence Number check failed (0x%x)\n", mr_seq_num);
      return ret;
   }
    
   mocad_write_nondefseqnum(ctx, (int) mr_seq_num);

   if (ctx->moca_running)   
      ret = __moca_set_non_def_seq_num(vctx, mr_seq_num);

   return ret;
}

MOCAD_S int mocad_set_rlapm_table_50(void * vctx, struct moca_rlapm_table_50 * in)
{
   struct mocad_ctx *ctx = (struct mocad_ctx *)vctx;
   struct moca_rlapm_table_50 tbl;
   int ret = 0;
   int i;

   moca_set_rlapm_table_50_defaults(&tbl, ctx->config_flags);

   if (ctx->any_time.rlapm_en)
   {
      ret = moca_set_rlapm_table_50(vctx, in);
   }
   else
   {
      for (i = 0; 
           i < (int)(sizeof(tbl.rlapmtable)/sizeof(tbl.rlapmtable[0])); 
           i++)
      {
         tbl.rlapmtable[i] = MIN(in->rlapmtable[i], ctx->any_time.rlapm_cap_50);
      }

      ret = moca_set_rlapm_table_50(vctx, &tbl);
   }

   return ret;
}


MOCAD_S int mocad_set_rlapm_table_100(void * vctx, struct moca_rlapm_table_100 * in)
{
   struct mocad_ctx *ctx = (struct mocad_ctx *)vctx;
   struct moca_rlapm_table_100 tbl;
   int ret = 0;
   int i;

   moca_set_rlapm_table_100_defaults(&tbl, ctx->config_flags);

   if (ctx->any_time.rlapm_en)
   {
      ret = moca_set_rlapm_table_100(vctx, in);
   }
   else
   {
      for (i = 0; 
           i < (int)(sizeof(tbl.rlapmtable)/sizeof(tbl.rlapmtable[0])); 
           i++)
      {
         tbl.rlapmtable[i] = MIN(in->rlapmtable[i], ctx->any_time.rlapm_cap_100);
      }

      ret = moca_set_rlapm_table_100(vctx, &tbl);
   }

   return ret;
}


MOCAD_S int mocad_set_rlapm_en(void * vctx, uint32_t bool_val)
{
   struct mocad_ctx *ctx = (struct mocad_ctx *)vctx;
   int ret = 0;

   ret = moca_set_rlapm_en(vctx, 1);

   if (ret == 0)
   {
      ret = mocad_set_rlapm_table_50(vctx, (struct moca_rlapm_table_50 *)&ctx->any_time.rlapm_table_50);
   }
   if (ret == 0)
   {
      ret = mocad_set_rlapm_table_100(vctx, (struct moca_rlapm_table_100 *)&ctx->any_time.rlapm_table_100);
   }

   return ret;
}

MOCAD_S int mocad_set_rlapm_cap_50(void * vctx, uint32_t cap)
{
   struct mocad_ctx *ctx = (struct mocad_ctx *)vctx;
   int ret = 0;

   if (ctx->any_time.rlapm_en == 0)
   {
      ret = mocad_set_rlapm_table_50(vctx, (struct moca_rlapm_table_50 *)&ctx->any_time.rlapm_table_50);
   }

   return ret;
}

MOCAD_S int mocad_set_rlapm_cap_100(void * vctx, uint32_t cap)
{
   struct mocad_ctx *ctx = (struct mocad_ctx *)vctx;
   int ret = 0;

   if (ctx->any_time.rlapm_en == 0)
   {
      ret = mocad_set_rlapm_table_100(vctx, (struct moca_rlapm_table_100 *)&ctx->any_time.rlapm_table_100);
   }

   return ret;
}

MOCAD_S int mocad_set_snr_margin_rs(void * vctx, struct moca_snr_margin_rs * in)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
   int i;

   for (i=0;i < SNR_ELEMENTS-1; i++)	 
   {	 
      if ((int)in->offsets[i] + (int)snr_def_table_rs.mgntable[i] >	 
          (int)in->offsets[i+1] + (int)snr_def_table_rs.mgntable[i+1])	 
      {	 
         mocad_log(ctx, L_ERR, "SNR_TABLE_RS: Table does not contain increasing values\n");	 
         return -1;	 
      }	 
   }

   ctx->any_time.snr_margin_rs.base_margin = in->base_margin;
   ctx->any_time.snr_margin_table_rs.mgntable[SNR_ELEMENTS] =
      ctx->any_time.snr_margin_rs.base_margin;

   for (i = 0; i < SNR_ELEMENTS; i++)
   {
      ctx->any_time.snr_margin_rs.offsets[i] = in->offsets[i];
      ctx->any_time.snr_margin_table_rs.mgntable[i] = (int16_t)
         ((int)in->base_margin + (int)ctx->any_time.snr_margin_rs.offsets[i] + 
         (int)snr_def_table_rs.mgntable[i]);
      ctx->any_time.snr_margin_table_rs.mgntable[i + SNR_ELEMENTS + 1] = 
         ctx->any_time.snr_margin_rs.offsets[i];
   }

   return __moca_set_snr_margin_table_rs(vctx, &ctx->any_time.snr_margin_table_rs);
}

MOCAD_S int mocad_set_snr_margin_ldpc(void * vctx, struct moca_snr_margin_ldpc * in)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
   int i;

   for (i=0;i < SNR_ELEMENTS-1; i++)
   {
      if ((int)in->offsets[i] + (int)snr_def_table_ldpc.mgntable[i] >
          (int)in->offsets[i+1] + (int)snr_def_table_ldpc.mgntable[i+1])
      {
         mocad_log(ctx, L_ERR, "SNR_TABLE_LDPC: Table does not contain increasing values\n");
         return -1;
      }
   }

   ctx->any_time.snr_margin_ldpc.base_margin = in->base_margin;

   ctx->any_time.snr_margin_table_ldpc.mgntable[SNR_ELEMENTS] =
      ctx->any_time.snr_margin_ldpc.base_margin;

   for (i = 0; i < SNR_ELEMENTS; i++)
   {
      ctx->any_time.snr_margin_ldpc.offsets[i] = in->offsets[i];
      ctx->any_time.snr_margin_table_ldpc.mgntable[i] = (int16_t)
         ((int)in->base_margin + (int)ctx->any_time.snr_margin_ldpc.offsets[i] + 
         (int)snr_def_table_ldpc.mgntable[i]);
      ctx->any_time.snr_margin_table_ldpc.mgntable[i + SNR_ELEMENTS + 1] = 
         ctx->any_time.snr_margin_ldpc.offsets[i];
   }

   return __moca_set_snr_margin_table_ldpc(vctx, &ctx->any_time.snr_margin_table_ldpc);
}

MOCAD_S int mocad_set_snr_margin_ldpc_pri_ch(void * vctx, struct moca_snr_margin_ldpc_pri_ch * in)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
   int i;

   for (i=0;i < SNR_ELEMENTS-1; i++)	 
   {	 
      if ((int)in->offsets[i] + (int)snr_def_table_ldpc_pri_ch.mgntable[i] >	 
         (int)in->offsets[i+1] + (int)snr_def_table_ldpc_pri_ch.mgntable[i+1])	 
      {	 
         mocad_log(ctx, L_ERR, "SNR_TABLE_LDPC_PRI_CH: Table does not contain increasing values\n");	 
         return -1;	 
      }	 
   }

   ctx->any_time.snr_margin_ldpc_pri_ch.base_margin = in->base_margin;

   ctx->any_time.snr_margin_table_ldpc_pri_ch.mgntable[SNR_ELEMENTS] =
      ctx->any_time.snr_margin_ldpc_pri_ch.base_margin;

   for (i = 0; i < SNR_ELEMENTS; i++)
   {
      ctx->any_time.snr_margin_ldpc_pri_ch.offsets[i] = in->offsets[i];
      ctx->any_time.snr_margin_table_ldpc_pri_ch.mgntable[i] = (int16_t)
         ((int)in->base_margin + (int)ctx->any_time.snr_margin_ldpc_pri_ch.offsets[i] + 
         (int)snr_def_table_ldpc_pri_ch.mgntable[i]);
      ctx->any_time.snr_margin_table_ldpc_pri_ch.mgntable[i + SNR_ELEMENTS + 1] = 
         ctx->any_time.snr_margin_ldpc_pri_ch.offsets[i];
   }

   return __moca_set_snr_margin_table_ldpc_pri_ch(vctx, &ctx->any_time.snr_margin_table_ldpc_pri_ch);
}


MOCAD_S int mocad_set_snr_margin_ldpc_sec_ch(void * vctx, struct moca_snr_margin_ldpc_sec_ch * in)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
   int i;

   for (i=0;i < SNR_ELEMENTS-1; i++)	 
   {	 
      if ((int)in->offsets[i] + (int)snr_def_table_ldpc_sec_ch.mgntable[i] >	 
         (int)in->offsets[i+1] + (int)snr_def_table_ldpc_sec_ch.mgntable[i+1])	 
      {	 
         mocad_log(ctx, L_ERR, "SNR_TABLE_LDPC_SEC_CH: Table does not contain increasing values\n");	 
         return -1;	 
      }	 
   }

   ctx->any_time.snr_margin_ldpc_sec_ch.base_margin = in->base_margin;

   ctx->any_time.snr_margin_table_ldpc_sec_ch.mgntable[SNR_ELEMENTS] =
      ctx->any_time.snr_margin_ldpc_sec_ch.base_margin;

   for (i = 0; i < SNR_ELEMENTS; i++)
   {
      ctx->any_time.snr_margin_ldpc_sec_ch.offsets[i] = in->offsets[i];
      ctx->any_time.snr_margin_table_ldpc_sec_ch.mgntable[i] = (int16_t)
         ((int)in->base_margin + (int)ctx->any_time.snr_margin_ldpc_sec_ch.offsets[i] + 
         (int)snr_def_table_ldpc_sec_ch.mgntable[i]);
      ctx->any_time.snr_margin_table_ldpc_sec_ch.mgntable[i + SNR_ELEMENTS + 1] = 
         ctx->any_time.snr_margin_ldpc_sec_ch.offsets[i];
   }

   return __moca_set_snr_margin_table_ldpc_sec_ch(vctx, &ctx->any_time.snr_margin_table_ldpc_sec_ch);
}

MOCAD_S int mocad_set_snr_margin_ldpc_pre5(void * vctx, struct moca_snr_margin_ldpc_pre5 * in)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
   int i;

   for (i=0;i < SNR_ELEMENTS-1; i++)	 
   {	 
      if ((int)in->offsets[i] + (int)snr_def_table_ldpc_pre5.mgntable[i] >	 
          (int)in->offsets[i+1] + (int)snr_def_table_ldpc_pre5.mgntable[i+1])	 
      {	 
         mocad_log(ctx, L_ERR, "SNR_TABLE_LDPC_PRE5: Table does not contain increasing values\n");	 
         return -1;	 
      }	 
   }

   ctx->any_time.snr_margin_ldpc_pre5.base_margin = in->base_margin;
   ctx->any_time.snr_margin_table_ldpc_pre5.mgntable[SNR_ELEMENTS] =
      ctx->any_time.snr_margin_ldpc_pre5.base_margin;

   for (i = 0; i < SNR_ELEMENTS; i++)
   {
      ctx->any_time.snr_margin_ldpc_pre5.offsets[i] = in->offsets[i];
      ctx->any_time.snr_margin_table_ldpc_pre5.mgntable[i] = (int16_t)
         ((int)in->base_margin + (int)ctx->any_time.snr_margin_ldpc_pre5.offsets[i] + 
         (int)snr_def_table_ldpc_pre5.mgntable[i]);
      ctx->any_time.snr_margin_table_ldpc_pre5.mgntable[i + SNR_ELEMENTS + 1] = 
         ctx->any_time.snr_margin_ldpc_pre5.offsets[i];
   }

   return __moca_set_snr_margin_table_ldpc_pre5(vctx, &ctx->any_time.snr_margin_table_ldpc_pre5);
}


MOCAD_S int mocad_set_snr_margin_pre5_pri_ch(void * vctx, struct moca_snr_margin_pre5_pri_ch * in)
{
	struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
	int i;

	for (i=0;i < SNR_ELEMENTS-1; i++)	 
	{	 
		if ((int)in->offsets[i] + (int)snr_def_table_pre5_pri_ch.mgntable[i] >	 
			(int)in->offsets[i+1] + (int)snr_def_table_pre5_pri_ch.mgntable[i+1])	 
		{	 
			mocad_log(ctx, L_ERR, "SNR_TABLE_PRE5_PRI_CH: Table does not contain increasing values\n");	 
			return -1;	 
		}	 
	}

	ctx->any_time.snr_margin_pre5_pri_ch.base_margin = in->base_margin;
	ctx->any_time.snr_margin_table_pre5_pri_ch.mgntable[SNR_ELEMENTS] =
		ctx->any_time.snr_margin_pre5_pri_ch.base_margin;

	for (i = 0; i < SNR_ELEMENTS; i++)
	{
		ctx->any_time.snr_margin_pre5_pri_ch.offsets[i] = in->offsets[i];
		ctx->any_time.snr_margin_table_pre5_pri_ch.mgntable[i] = (int16_t)
			((int)in->base_margin + (int)ctx->any_time.snr_margin_pre5_pri_ch.offsets[i] + 
			(int)snr_def_table_pre5_pri_ch.mgntable[i]);
		ctx->any_time.snr_margin_table_pre5_pri_ch.mgntable[i + SNR_ELEMENTS + 1] = 
			ctx->any_time.snr_margin_pre5_pri_ch.offsets[i];
	}

	return __moca_set_snr_margin_table_pre5_pri_ch(vctx, &ctx->any_time.snr_margin_table_pre5_pri_ch);

}
MOCAD_S int mocad_set_snr_margin_pre5_sec_ch(void * vctx, struct moca_snr_margin_pre5_sec_ch * in)
{
	struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
	int i;

	for (i=0;i < SNR_ELEMENTS-1; i++)	 
	{	 
		if ((int)in->offsets[i] + (int)snr_def_table_pre5_sec_ch.mgntable[i] >	 
			(int)in->offsets[i+1] + (int)snr_def_table_pre5_sec_ch.mgntable[i+1])	 
		{	 
			mocad_log(ctx, L_ERR, "SNR_TABLE_PRE5_PRI_CH: Table does not contain increasing values\n");	 
			return -1;	 
		}	 
	}

	ctx->any_time.snr_margin_pre5_sec_ch.base_margin = in->base_margin;
	ctx->any_time.snr_margin_table_pre5_sec_ch.mgntable[SNR_ELEMENTS] =
		ctx->any_time.snr_margin_pre5_sec_ch.base_margin;

	for (i = 0; i < SNR_ELEMENTS; i++)
	{
		ctx->any_time.snr_margin_pre5_sec_ch.offsets[i] = in->offsets[i];
		ctx->any_time.snr_margin_table_pre5_sec_ch.mgntable[i] = (int16_t)
			((int)in->base_margin + (int)ctx->any_time.snr_margin_pre5_sec_ch.offsets[i] + 
			(int)snr_def_table_pre5_sec_ch.mgntable[i]);
		ctx->any_time.snr_margin_table_pre5_sec_ch.mgntable[i + SNR_ELEMENTS + 1] = 
			ctx->any_time.snr_margin_pre5_sec_ch.offsets[i];
	}

	return __moca_set_snr_margin_table_pre5_sec_ch(vctx, &ctx->any_time.snr_margin_table_pre5_sec_ch);
}





MOCAD_S int mocad_set_snr_margin_ofdma(void * vctx, struct moca_snr_margin_ofdma * in)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
   int i;

   for (i=0;i < SNR_ELEMENTS-1; i++)	 
   {	 
      if ((int)in->offsets[i] + (int)snr_def_table_ofdma.mgntable[i] >	 
          (int)in->offsets[i+1] + (int)snr_def_table_ofdma.mgntable[i+1])	 
      {	 
         mocad_log(ctx, L_ERR, "SNR_TABLE_OFDMA: Table does not contain increasing values\n");	 
         return -1;	 
      }	 
   }

   ctx->any_time.snr_margin_ofdma.base_margin = in->base_margin;
   ctx->any_time.snr_margin_table_ofdma.mgntable[SNR_ELEMENTS] =
      ctx->any_time.snr_margin_ofdma.base_margin;

   for (i = 0; i < SNR_ELEMENTS; i++)
   {
      ctx->any_time.snr_margin_ofdma.offsets[i] = in->offsets[i];
      ctx->any_time.snr_margin_table_ofdma.mgntable[i] = (int16_t)
         ((int)in->base_margin + (int)ctx->any_time.snr_margin_ofdma.offsets[i] + 
         (int)snr_def_table_ofdma.mgntable[i]);
      ctx->any_time.snr_margin_table_ofdma.mgntable[i + SNR_ELEMENTS + 1] = 
         ctx->any_time.snr_margin_ofdma.offsets[i];
   }

   return __moca_set_snr_margin_table_ofdma(vctx, &ctx->any_time.snr_margin_table_ofdma);
}

MOCAD_S int mocad_set_primary_ch_offset(void * vctx, int32_t offset)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *)vctx;
   uint32_t core_offset = MOCA_PRIM_CH_OFFS_DEF;

   switch (offset)
   {
      case -25:
         core_offset = 2;
         break;
      case 25:
         core_offset = 1;
         break;
      case 0:
      case 1:
         core_offset = 0;
         break;
      default:
         mocad_log(ctx, L_WARN, "Unrecognized value %d for primary_ch_offset\n", offset);
         break;
   }

   return __moca_set_prim_ch_offs(vctx, core_offset);
}

MOCAD_S int mocad_set_secondary_ch_offset(void * vctx, int32_t offset)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *)vctx;
   uint32_t core_offset = MOCA_SEC_CH_OFFS_DEF;

   switch (offset)
   {
      case -125:
      case 1:
         core_offset = 1;
         break;
      case 125:
         core_offset = 2;
         break;
      case 0:
         core_offset = 0;
         break;
      default:
         mocad_log(ctx, L_WARN, "Unrecognized value %d for secondary_ch_offset\n", offset);
         break;
   }

   return __moca_set_sec_ch_offs(vctx, core_offset);
}

MOCAD_S int mocad_set_sapm_table_50(void * vctx, struct moca_sapm_table_50 * table)
{
   int i;
   int fw_index;
   struct moca_sapm_table_fw_50 fw_table;

   /* Copy the incoming table into the firmware version */
   fw_index = 0;
   for (i = MOCA_SAPM_TABLE_FW_50_LOW_START; 
        i <= MOCA_SAPM_TABLE_FW_50_LOW_END;
        i++ )
   {
      fw_table.sapmtablelo[fw_index++] = table->val[i];
   }

   fw_index = 0;

   for (i = MOCA_SAPM_TABLE_FW_50_HI_START; 
        i <= MOCA_SAPM_TABLE_FW_50_HI_END;
        i++ )
   {
      fw_table.sapmtablehi[fw_index++] = table->val[i];
   }
        
   return __moca_set_sapm_table_fw_50(vctx, &fw_table);
}

MOCAD_S int mocad_set_sapm_table_100(void * vctx, struct moca_sapm_table_100 * table)
{
   int i;
   int fw_index;
   struct moca_sapm_table_fw_100 fw_table;

   /* Copy the incoming table into the firmware version */
   fw_index = 0;
   for (i = MOCA_SAPM_TABLE_FW_100_LOW_START; 
        i <= MOCA_SAPM_TABLE_FW_100_LOW_END;
        i++ )
   {
      fw_table.sapmtablelo[fw_index++] = table->val[i];
   }

   fw_index = 0;

   for (i = MOCA_SAPM_TABLE_FW_100_HI_START; 
        i <= MOCA_SAPM_TABLE_FW_100_HI_END;
        i++ )
   {
      fw_table.sapmtablehi[fw_index++] = table->val[i];
   }
        
   return __moca_set_sapm_table_fw_100(vctx, &fw_table);
}

MOCAD_S int mocad_set_sapm_table_sec(void * vctx, struct moca_sapm_table_sec * table)
{
   int i;
   int fw_index;
   struct moca_sapm_table_fw_sec fw_table;

   /* Copy the incoming table into the firmware version */
   fw_index = 0;
   for (i = MOCA_SAPM_TABLE_FW_SEC_LOW_START; 
        i <= MOCA_SAPM_TABLE_FW_SEC_LOW_END;
        i++ )
   {
      fw_table.sapmtablelo[fw_index++] = table->val[i];
   }

   fw_index = 0;

   for (i = MOCA_SAPM_TABLE_FW_SEC_HI_START; 
        i <= MOCA_SAPM_TABLE_FW_SEC_HI_END;
        i++ )
   {
      fw_table.sapmtablehi[fw_index++] = table->val[i];
   }
        
   return __moca_set_sapm_table_fw_sec(vctx, &fw_table);
}

MOCAD_S int mocad_set_keys(void * vctx, struct moca_network_password * password)
{
   int ret;
   uint8_t mmk[8], pmki[8];
   struct moca_mmk_key_set mmk_key;
   struct moca_pmk_initial_key_set pmk_initial_key;
   
   ret = mocad_keygen(mmk, pmki,
      (unsigned char *)password->password);
      
   if (ret != 0)
      return(ret);

   mmk_key.mmk_key_hi = (mmk[0]  << 24) | (mmk[1]  << 16) |
                        (mmk[2]  <<  8) | (mmk[3]  <<  0);
   mmk_key.mmk_key_lo = (mmk[4]  << 24) | (mmk[5]  << 16) |
                        (mmk[6]  <<  8) | (mmk[7]  <<  0);
   pmk_initial_key.pmk_initial_key_hi = (pmki[0] << 24) | (pmki[1] << 16) |
                                        (pmki[2] <<  8) | (pmki[3] <<  0);
   pmk_initial_key.pmk_initial_key_lo = (pmki[4] << 24) | (pmki[5] << 16) |
                                        (pmki[6] <<  8) | (pmki[7] <<  0);
   
   __moca_set_mmk_key(vctx, &mmk_key);
   __moca_set_pmk_initial_key(vctx, &pmk_initial_key);
   return 0;
}

MOCAD_S int mocad_set_network_password(void * vctx, struct moca_network_password * password)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
   int ret = mocad_set_keys(vctx, password);
   if (ret != 0)
      return(ret);
   
   return __moca_set_network_password(vctx, &ctx->any_time.network_password);
}

MOCAD_S void mocad_set_anytime_network_password(struct mocad_ctx *ctx, const unsigned char *password)
{
   memset(&ctx->any_time.network_password, 0, sizeof(ctx->any_time.network_password));
   strcpy(ctx->any_time.network_password.password, (char*)password);
   memset(&ctx->any_time.password, 0, sizeof(ctx->any_time.password));
   strcpy(ctx->any_time.password.password, (char*)password);
}

MOCAD_S int mocad_set_const_tx_params(void * vctx, struct moca_const_tx_params * params)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;

   /* If const TX mode is band and the bandwidth is 50MHz, then move sub-carriers
         128-255 into positions 384-511 for firmware. */
   if ((params->const_tx_submode == MOCA_CONST_TX_SUBMODE_BAND) &&
       (ctx->any_time.bandwidth  == MOCA_BANDWIDTH_50MHZ))
   {
      memcpy(&params->const_tx_band[12], &params->const_tx_band[4], (sizeof(uint32_t) * 4));
      memset(&params->const_tx_band[4], 0, (sizeof(uint32_t) * 8));
   }

   return moca_set_const_tx_params(vctx, (const struct moca_const_tx_params *) params);
}

MOCAD_S int mocad_set_nv_cal_enable(void * vctx, uint32_t nv_cal_enable)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
   static uint32_t previous_value = 0;

   if ((previous_value != nv_cal_enable) &&
       (nv_cal_enable))
   {
      /* NV Calibration is being enabled, commit the most recent calibration data to flash if it's valid */
      if (ctx->rf_calib_data.valid)
      {
         mocad_write_rf_calib_data(ctx);
      }

      if (ctx->probe_2_results.theta)
      {
         mocad_write_probe_2_results(ctx);
      }
   }

   previous_value = nv_cal_enable;

   return(0);
}

MOCAD_S int mocad_set_led_mode(void * vctx, uint32_t led_mode)
{
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
   int ret = 0;

   ret = MoCAOS_SetLedMode(ctx->os_handle, led_mode);

   if (ret != 0)
      return(ret);

   ret = moca_set_led_mode(ctx, led_mode);

   return(ret);
}


MOCAD_S int mocad_set_eport_link_state(void * vctx)
{
#ifdef STANDALONE
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
   int ret = 0;
   uint32_t state = EPORT_GetLinkState();

   ret = __moca_set_eport_link_state(ctx , state);

   return(ret);
#elif defined(DSL_MOCA)
   struct mocad_ctx * ctx = (struct mocad_ctx *) vctx;
   int ret = 0;

   ret = __moca_set_eport_link_state(ctx , 1);

   return(ret);
#else
   return(0);
#endif
}


MOCAD_S static void mocad_update_config_flags(struct mocad_ctx *ctx)
{
   ctx->config_flags = 0;

   if (ctx->any_time.rf_band == MOCA_RF_BAND_D_LOW)
   {
      ctx->config_flags |= MOCA_BAND_D_LOW_FLAG;
   }
   if (ctx->any_time.rf_band == MOCA_RF_BAND_D_HIGH)
   {
      ctx->config_flags |= MOCA_BAND_D_HIGH_FLAG;
   }
   if (ctx->any_time.rf_band == MOCA_RF_BAND_EX_D)
   {
      ctx->config_flags |= MOCA_BAND_EX_D_FLAG;
   }
   
   if (ctx->any_time.rf_band == MOCA_RF_BAND_E)
   {
      ctx->config_flags |= MOCA_BAND_E_FLAG;
   }

   if (ctx->any_time.rf_band == MOCA_RF_BAND_C4)
   {
      ctx->config_flags |= MOCA_BAND_C4_FLAG;
   }

   if (ctx->any_time.rf_band == MOCA_RF_BAND_F)
   {
      ctx->config_flags |= MOCA_BAND_F_FLAG;
   }

   if (ctx->any_time.rf_band == MOCA_RF_BAND_H)
   {
      ctx->config_flags |= MOCA_BAND_H_FLAG;
   }

   if (ctx->any_time.rf_band == MOCA_RF_BAND_GENERIC)
   {
      ctx->config_flags |= MOCA_BAND_GENERIC_FLAG;
   }

   if (
       (
        ((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x68020000) ||
        ((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x339000B0)
       ) &&
       (ctx->any_time.rf_band != MOCA_RF_BAND_D_LOW) &&
       (ctx->any_time.rf_band != MOCA_RF_BAND_C4) &&
       (ctx->any_time.rf_band != MOCA_RF_BAND_H))
   {
      ctx->config_flags |= MOCA_BONDING_SUPPORTED_FLAG;
   }

   if ((ctx->kdrv_info.chip_id & 0xFFFE00F0) == 0x680200C0)
   {
      ctx->config_flags |= MOCA_6802C0_FLAG;
   }

   if ((ctx->kdrv_info.chip_id & 0xFFFE00F0) == 0x680200B0)
   {
      ctx->config_flags |= MOCA_6802B0_FLAG;
   }

   if (((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x742800B0) ||
       ((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x742900B0) ||
       ((ctx->kdrv_info.chip_id & 0xFFFFFF0) == 0x74295A0))
   {
      ctx->config_flags |= MOCA_7428B0_FLAG;
   }

   
   if ((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x74250000)
   {
      ctx->config_flags |= MOCA_7425_FLAG;
   }

   if ((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x744500D0)
   {
      ctx->config_flags |= MOCA_7445D0_FLAG | MOCA_SWITCH_FLAG;
   }

   if ((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x742500B0)
   {
      ctx->config_flags |= MOCA_7425B0_FLAG;
   }
  
   
   if ((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x71450000)
   {
      ctx->config_flags |= MOCA_SWITCH_FLAG;
   }

   if (((ctx->kdrv_info.chip_id & 0x0FFFFF00) == 0x7437100) ||
       ((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x43710000) || 
       ((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x74370000))
   {
      ctx->config_flags |= MOCA_74371B0_FLAG;
   }

   if (ctx->kdrv_info.hw_rev >= MOCA_CHIP_20_3)
   {
      ctx->config_flags |= MOCA_28NM_FLAG;
   }
   
   if ((ctx->kdrv_info.chip_id  & 0xFFFF00F0) == 0x339000B0)
   { 
      ctx->config_flags |= MOCA_3390B0_FLAG;
   }
#if defined(DSL_MOCA)
   ctx->config_flags |= MOCA_6816_FLAG;
//#if defined(CHIP_63268)
//   ctx->config_flags |= MOCA_BHR_FLAG;
//#endif
#else
   ctx->config_flags |= MOCA_7xxx_FLAG;
#endif

#ifdef STANDALONE
   ctx->config_flags |= MOCA_STANDALONE_FLAG;
#endif

   if (((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x74250000) || 
       ((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x74350000) || 
       ((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x74280000) || 
       ((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x74290000) || 
       ((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x73660000) || 
       ((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x73640000) || 
       ((ctx->kdrv_info.chip_id & 0xFFFF0000) == 0x74390000))
       ctx->config_flags |= MOCA_FC_CAPABLE_CHIP_FLAG;
}


// restore init and anytime values to defaults, except turboEn and rfType
MOCAD_S int mocad_restore_defaults(struct mocad_ctx *ctx, unsigned int freq, unsigned int waitmode)
{
   int i;
   struct moca_mac_addr_int mac;
   uint32_t impedance_mode_bonding;
   uint32_t rework_6802;
   char pad_ctrl_deg_6802[86];

   mocad_update_config_flags(ctx);

   // preserve any MAC address the user may have set:
   memcpy(&mac, &ctx->any_time.mac_addr, sizeof(mac));
   // preserve any impedance mode the user may have set:
   memcpy(&impedance_mode_bonding, &ctx->any_time.impedance_mode_bonding, sizeof(impedance_mode_bonding));
   // preserve any rework 6802 the user may have set:
   memcpy(&rework_6802, &ctx->any_time.rework_6802, sizeof(rework_6802));
   memcpy(pad_ctrl_deg_6802, ctx->any_time.prof_pad_ctrl_deg_6802c0_single.offset, sizeof(pad_ctrl_deg_6802));

   mocad_set_defaults(ctx, ctx->config_flags);
   mocad_mps_restore_defaults(ctx);

   memcpy(&ctx->any_time.mac_addr, &mac, sizeof(mac));
   memcpy(&ctx->any_time.impedance_mode_bonding, &impedance_mode_bonding, sizeof(impedance_mode_bonding));
   memcpy(&ctx->any_time.rework_6802, &rework_6802, sizeof(rework_6802));
   memcpy(ctx->any_time.prof_pad_ctrl_deg_6802c0_single.offset, pad_ctrl_deg_6802, sizeof(pad_ctrl_deg_6802));

   // Some startup scripts will start mocad with the -w option to wait
   // for an IE_START message before starting MoCA. This allows the
   // script to configure MoCA parameters, which may include a restore
   // defaults operation. In this case we don't want to use the default
   // operating frequency, use the the disk_lof parameter instead. 
   if((freq == 0) || (waitmode == MOCA_DONT_START_MOCA_WAIT)) {
      ctx->any_time.lof = ctx->disk_lof;
   } else {
      ctx->any_time.lof = freq;
   }

   ctx->any_time.dont_start_moca = waitmode;

   /* Max constellation default values are handled differently than others. Keep 
      the initialization here. */
   for (i = 0; i < MOCA_MAX_NODES; i++)
   {
      moca_set_max_constellation_defaults (&ctx->max_constellation[i], ctx->config_flags);
      ctx->max_constellation[i].node_id = i;
   }

   return 0;
}

MOCAD_S int mocad_req(struct mocad_ctx *ctx, void *wr, int wr_len)
{
   int len;
   int ret;
   unsigned int start;   
   struct mmp_msg_hdr *mh = (struct mmp_msg_hdr *)ctx->drv_in;

   if(ctx->verbose & L_MMP_MSG)
         mocad_dump_buf(ctx, "REQ <", (unsigned char *)wr, wr_len);

   MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_CORE, 
      (unsigned char *)wr, (unsigned int)wr_len);

   start = MoCAOS_GetTimeSec();

   while (1)
   {
      len = sizeof(ctx->drv_in);
      ret = MoCAOS_ReadMMP(ctx->os_handle, MoCAOS_CLIENT_CORE, 
         MOCA_REQ_TIMEOUT, ctx->drv_in, &len);  // return 0 timeout, <0 failure, >0 success

      if (ret == 0)
      {
         // Timeout
         mocad_update_times(ctx, MOCA_TIME_EVENT_CORE_DOWN);
         ctx->moca_running = 0;
         RESTART(13);
         return(-1);
      }
      else if (ret < 0)
      {
         return(-2);
      }

      ctx->lab_printf_wdog_count++;


      if(BE32(mh->type) & MOCA_MMP_MSG_ACK_BIT)
      {
         if(ctx->verbose & L_MMP_MSG)
            mocad_dump_buf(ctx, "RESP>",
               ctx->drv_in, len);
         return(len);
      }
      else if (((BE32(mh->type)&0xF) == MOCA_MSG_TRAP) ||
               ((BE32(mh->type)&0xF) == MOCA_MSG_CONCAT_TRAP))
      {
         /* While expecting a RSP from core, 
          * defer trap processing */
         mocad_add_trap(ctx, ctx->drv_in, len);
      }
      else
      {
         mocad_log(ctx, L_WARN,
            "warning: unrecognized reply 0x%x\n",
            BE32(mh->type));
         return(-3);
      }

      if (MoCAOS_GetTimeSec()-start > MOCA_REQ_TIMEOUT+1)
      {
         // overall loop timeout
         mocad_update_times(ctx, MOCA_TIME_EVENT_CORE_DOWN);
         ctx->moca_running = 0;
         RESTART(14);
         return(-4);
      }        
   }
}


MOCAD_S int mocad_cmd(void *vctx, uint32_t msg_type, uint32_t ie_type,
   const void *wr, int wr_len, void *rd, int rd_len, int flags)
{
   struct mocad_ctx *ctx = (struct mocad_ctx *)vctx;
   struct mmp_msg_hdr *mh;
   int ret;

   mh = (struct mmp_msg_hdr *)ctx->drv_out;
   mh->type = BE32(msg_type);
   mh->length = BE32(wr_len + MOCA_EXTRA_MMP_HDR_LEN);
   mh->ie_type = BE32(ie_type);

   if(wr_len) {
      if(flags & FL_SWAP_WR)
         __moca_copy_be32((void *)(mh + 1), wr, wr_len);
      else
         memcpy((void *)(mh + 1), wr, wr_len);
   }

   ret = mocad_req(ctx, ctx->drv_out, wr_len + sizeof(*mh));
   if(ret < 0)
      return(ret);

   mh = (struct mmp_msg_hdr *)ctx->drv_in;

   if(ret <= (int)(sizeof(*mh))) {
      mocad_log(ctx, L_WARN, 
         "warning: short response for %s (recv: %d vs expected: %d)\n",
         moca_ie_name(ie_type), ret, (int)(sizeof(*mh)));
      return(-1);
   }

   ret -= sizeof(*mh);

   if(ret > rd_len) {
      mocad_log(ctx, L_WARN,
         "warning: response size exceeds buffer len IE %04X, (%d %d)\n", ie_type, ret, rd_len);
      ret = rd_len;
   }
   if(ie_type != BE32(mh->ie_type)) {
      mocad_log(ctx, L_WARN,
         "warning: response IE mismatch, %x != %x\n",
         ie_type, BE32(mh->ie_type));
   }

   if(rd && ret) {
      if(flags & FL_SWAP_RD)
         __moca_copy_be32(rd, (void *)(mh + 1), ret);
      else
         memcpy(rd, (void *)(mh + 1), ret);
      }
   return(0);
}


MOCAD_S int mocad_get_table(void * vctx, uint32_t ie_type, uint32_t * in,
   void * out, int struct_len, int max_out_len, int flags)
{
   uint32_t            i = 0;
   int                 out_len_left = max_out_len;
   void *              wr = out;
   int                 ret = 0;
   int                 req_len = 0;
   struct mmp_msg_hdr *mh;
   struct mocad_ctx   *ctx = (struct mocad_ctx *)vctx;
   uint32_t *          wr_msg;

   mh = (struct mmp_msg_hdr *)ctx->drv_out;
   mh->type = BE32(MOCA_MSG_GET);
   mh->length = BE32(sizeof(i) + MOCA_EXTRA_MMP_HDR_LEN);
   mh->ie_type = BE32(ie_type);

   while (1)
   {
      mh = (struct mmp_msg_hdr *)ctx->drv_out;

      wr_msg = (uint32_t *)(mh + 1);
      __moca_copy_be32((void *)wr_msg, &i, sizeof(i));

      req_len = sizeof(i) + sizeof(*mh);

      if (in != NULL)
      {
         wr_msg++;
         __moca_copy_be32((void *)wr_msg, in, sizeof(*in));
         req_len += sizeof(*in);
      }

      ret = mocad_req(ctx, ctx->drv_out, req_len);
      if(ret < 0)
         return(ret);

      mh = (struct mmp_msg_hdr *)ctx->drv_in;

      /* sanity checks on reply */
      if(ret <= (int)(sizeof(*mh))) {
         mocad_log(ctx, L_WARN, 
            "warning: short response for %s (recv %d vs expected %d)\n",
            moca_ie_name(ie_type), ret, (int)(sizeof(*mh)));
         return(-1);
      }

      ret -= sizeof(*mh);

      if(ret > out_len_left) {
         mocad_log(ctx, L_WARN,
            "warning: response size exceeds buffer len IE %04X, (%d %d %d %d)\n", 
            ie_type, ret, out_len_left, i, max_out_len);
         return(-2);
      }

      if((MOCA_MSG_GET | MOCA_MMP_MSG_ACK_BIT) != BE32(mh->type)) {
         return(-3);
      }

      if(ie_type != BE32(mh->ie_type)) {
         mocad_log(ctx, L_WARN,
            "warning: response IE mismatch, %x != %x\n",
            ie_type, BE32(mh->ie_type));
      }

      if ((ret % struct_len) != 0) {
         mocad_log(ctx, L_WARN,
            "warning: IE %s size % dnot multiple of struct_len %d\n",
            moca_ie_name(ie_type), ret, struct_len);
         return(-4);
      }

      if(out && ret) {
         if (flags & FL_SWAP_RD)
            __moca_copy_be32(wr, (void *)(mh + 1), ret);
         else
            memcpy(wr, (void *)(mh + 1), ret);
      }

      i += (ret / struct_len);
      out_len_left -= ret;
      wr = (void *)((uint8_t *)wr + ret);

      if (ret < (int)(MOCA_MAX_CORE_RSP - sizeof(*mh) - struct_len))
      {
         /* We have retrieved all of the data */
         ret = i;
         break;
      }
   }

   return(ret);
}


MOCAD_S int mocad_handle_add_mcfilter( struct mocad_ctx * ctx,  macaddr_t * pMacAddr)
{
    int i;
    int ret = 0;
    int first_empty = -1;
    struct moca_mcfilter_addentry mcadd;
   
    //Verify if mc is enabled
    if (ctx->any_time.mcfilter_enable == 0 )
    {
        mocad_log(ctx, L_WARN, "mcfilter mode is disabled\n");
         return(1);
    }
    /* search for an empty entry in the table and add it */
    for (i = 0; i < MOCA_MAX_ECL_MCFILTERS; i++)
    {
       if ((first_empty < 0) &&
           (MOCA_MACADDR_IS_NULL(&ctx->ecl_mcfilter[i])) )
       {
          first_empty = i;
       }

       /* make sure that this entry doesn't already exist */
       if (MOCA_MACADDR_COMPARE(pMacAddr, &ctx->ecl_mcfilter[i]))
       {
           mocad_log(ctx, L_WARN, "Entry already exists %02x:%02x:%02x:%02x:%02x:%02x\n",
                      MOCA_DISPLAY_MAC(*pMacAddr));
          return(1);
       }
   }

   if (first_empty < 0)
   {
      mocad_log(ctx, L_WARN, "Could not find free entry\n");
      return(2);
   }
   MOCA_MACADDR_COPY(&mcadd.addr, pMacAddr);
   ret  = moca_set_mcfilter_addentry(ctx, &mcadd);
   if (ret == 0)
   {
       MOCA_MACADDR_COPY(&ctx->ecl_mcfilter[first_empty], pMacAddr);
       return(0);
   }
   return(ret);
}

MOCAD_S int mocad_handle_del_mcfilter( struct mocad_ctx * ctx,  macaddr_t * pMacAddr)
{
   int i;
   
   int entry_found = 0;
   struct moca_mcfilter_delentry mcdel;
   int ret = 0;

   /* search for the matching entry in the table and delete it */
   for (i = 0; i < MOCA_MAX_ECL_MCFILTERS; i++)
   {
      if (MOCA_MACADDR_COMPARE(&ctx->ecl_mcfilter[i], pMacAddr) )
      {
         entry_found = 1;
         break;
      }
   }

   if (entry_found)
   {
      MOCA_MACADDR_COPY(&mcdel.addr, pMacAddr);
      ret = moca_set_mcfilter_delentry(ctx, &mcdel);
      if (ret == 0)
      {
         memset(&ctx->ecl_mcfilter[i], 0x0, sizeof(macaddr_t));
         return(0);
      }
   }
   else
   {
      mocad_log(ctx, L_WARN, "entry not found %02x:%02x:%02x:%02x:%02x:%02x\n", 
          MOCA_DISPLAY_MAC(*pMacAddr));
      return(1);
   }
   return(ret);
}

MOCAD_S int mocad_reset_mcfilter(struct mocad_ctx * ctx)
{
   int i;
   int ret =0;

   for (i = 0; i < MOCA_MAX_ECL_MCFILTERS; i++)
   {
      if ( !( MOCA_MACADDR_IS_NULL(&ctx->ecl_mcfilter[i])) )
      {
          ret = mocad_handle_del_mcfilter(ctx, &ctx->ecl_mcfilter[i]);
      }
   }
   return(ret);
}

MOCAD_S int mocad_clear_mcfilter(struct mocad_ctx * ctx)
{

  if ( ctx->any_time.mcfilter_enable != 0)
  {
     ctx->any_time.mcfilter_enable =0;
     moca_set_mcfilter_enable(ctx, 0);
  } 
  memset(ctx->ecl_mcfilter, 0x0, sizeof(ctx->ecl_mcfilter));
  return(0);
}

MOCAD_S static void mocad_set_pqos_flow_create_time (struct mocad_ctx * ctx, int index)
{
   uint64_t expire_time;
   struct moca_pqos_table * p_pqos = &ctx->pqos_table[index];
   
   p_pqos->create_time = (uint32_t)MoCAOS_GetTimeSec();

   expire_time = (uint64_t)p_pqos->create_time + (uint64_t)p_pqos->lease_time;

   if (expire_time >= 0x100000000LL)
      ctx->pqos_time_wrap_flags |= (1 << index);
   else
      ctx->pqos_time_wrap_flags &= ~(1 << index);
}

MOCAD_S static int mocad_add_pqos_entry(
   struct mocad_ctx * ctx, struct moca_pqos_table * r, int * index)
{
   int i;
   int first_empty = -1;

   /* search for an empty entry in the table and add it */
   for (i = 0; i < MOCA_MAX_PQOS_ENTRIES; i++)
   {
      if ((first_empty < 0) &&
           MOCA_MACADDR_IS_NULL(&ctx->pqos_table[i].flow_id))
      {
         first_empty = i;
      }

      /* make sure that this entry doesn't already exist */
      if (MOCA_MACADDR_COMPARE(&r->flow_id, &ctx->pqos_table[i].flow_id))
      {
         mocad_log(ctx, L_WARN, "%s: entry already exists %02x:%02x:%02x:%02x:%02x:%02x\n",
            __FUNCTION__, MOCA_DISPLAY_MAC(r->flow_id));
         return(1);
      }
   }

   if (first_empty < 0)
   {
      mocad_log(ctx, L_WARN, "%s: Could not find free entry\n",
         __FUNCTION__);
      return(1);
   }

   if (index != NULL)
      memcpy(index, &first_empty, sizeof(*index));

   if (ctx->any_time.host_qos)
   {
      mocad_update_pqos_flow_in_switch(ctx, r, MOCAD_PQOS_SWITCH_ADD, first_empty);
   }
   memcpy(&ctx->pqos_table[first_empty], r, sizeof(*r));
   mocad_set_pqos_flow_create_time(ctx, first_empty);
   
   return(0);
}

MOCAD_S static int mocad_del_pqos_entry(
   struct mocad_ctx * ctx, struct moca_pqos_table * r)
{
   int i;
   int entry_found = 0;

   /* search for the matching entry in the table and delete it */
   for (i = 0; i < MOCA_MAX_PQOS_ENTRIES; i++)
   {
      if (MOCA_MACADDR_COMPARE(&ctx->pqos_table[i].flow_id, &r->flow_id))
      {
         entry_found = 1;
         if (ctx->any_time.host_qos)
         {
            mocad_update_pqos_flow_in_switch(ctx, &ctx->pqos_table[i],
               MOCAD_PQOS_SWITCH_DELETE, i);
         }
         memset(&ctx->pqos_table[i], 0x0, sizeof(struct moca_pqos_table));
         
         break;
      }
   }

   if (entry_found)
   {
      return(0);
   }
   else
   {
      mocad_log(ctx, L_DEBUG, "%s: entry not found %02x:%02x:%02x:%02x:%02x:%02x\n", 
         __FUNCTION__, MOCA_DISPLAY_MAC(r->flow_id));
      return(1);
   }
}

MOCAD_S static void mocad_handle_pqos_create_rsp(struct mocad_ctx *ctx, 
   struct moca_pqos_create_response * pqosc_rsp)
{
   struct moca_network_status ns;
   struct moca_pqos_ingr_add_flow req;
   uint32_t nodeId = 0;
   uint32_t egressnodebitmask;
   int ret, i;
   struct moca_pqos_table * p_table = NULL;
   struct moca_pqos_table pqos_table;
   struct moca_pqos_create_flow_out * p_trap;
   struct moca_pqos_status_out * p_status_trap;


   if (ctx->do_flags & MOCAD_PQOS_CREATE_FLOW)
   {
      ctx->do_flags &= ~MOCAD_PQOS_CREATE_FLOW;

      p_trap = &ctx->mocad_trap.payload.pqosc_rsp;

      MOCA_MACADDR_COPY(&p_trap->flow_id, &pqosc_rsp->flowid);
      p_trap->response_code = pqosc_rsp->responsecode;
      p_trap->decision = pqosc_rsp->decision;
      p_trap->flow_tag = pqosc_rsp->flowtag;
      MOCA_MACADDR_COPY(&p_trap->flowda, &pqosc_rsp->flowda);
      p_trap->peak_data_rate = pqosc_rsp->maxpeakdatarate;
      p_trap->packet_size = pqosc_rsp->tpacketsize;
      p_trap->burst_size = pqosc_rsp->maxburstsize;
      p_trap->lease_time = pqosc_rsp->tleasetime;
      p_trap->total_stps = pqosc_rsp->totalstps;
      p_trap->total_txps = pqosc_rsp->totaltxps;
      p_trap->flow_stps = pqosc_rsp->flowstps;
      p_trap->flow_txps = pqosc_rsp->flowtxps;
      p_trap->dest_flow_id = pqosc_rsp->dest_flow_id;
      p_trap->maximum_latency = pqosc_rsp->maximum_latency;
      p_trap->short_term_avg_ratio = pqosc_rsp->short_term_avg_ratio;
      p_trap->max_number_retry = pqosc_rsp->max_number_retry;
      p_trap->flow_per = pqosc_rsp->flow_per;
      p_trap->in_order_delivery = pqosc_rsp->in_order_delivery;
      p_trap->ingr_class_rule = pqosc_rsp->ingr_class_rule;
      p_trap->traffic_protocol = pqosc_rsp->traffic_protocol;
      p_trap->vlan_tag = pqosc_rsp->vlan_tag;
      p_trap->dscp_moca = pqosc_rsp->dscp_moca;
      p_trap->bw_limit_info = pqosc_rsp->bw_limit_info;
      p_trap->max_short_term_avg_ratio = pqosc_rsp->max_short_term_avg_ratio;

      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(*p_trap) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_PQOS_CREATE_FLOW);

      ret = MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
         (const unsigned char *)&ctx->mocad_trap, 
         (sizeof(*p_trap) + sizeof(struct mmp_msg_hdr)));
   }
   else if (ctx->do_flags & MOCAD_PQOS_STATUS)
   {
      ctx->do_flags &= ~MOCAD_PQOS_STATUS;

      p_status_trap = &ctx->mocad_trap.payload.pqoss_rsp;
      p_status_trap->total_stps = pqosc_rsp->totalstps;
      p_status_trap->total_txps = pqosc_rsp->totaltxps;
      p_status_trap->flow_stps = pqosc_rsp->flowstps;
      p_status_trap->flow_txps = pqosc_rsp->flowtxps;

      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(*p_status_trap) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_PQOS_STATUS);

      ret = MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
         (const unsigned char *)&ctx->mocad_trap, 
         (sizeof(*p_status_trap) + sizeof(struct mmp_msg_hdr)));
   }

   ret = mocad_cmd(ctx, MOCA_MSG_GET, IE_NETWORK_STATUS, NULL, 0,
      &ns, sizeof(ns), 0);
   if (ret != 0)
   {
      mocad_log(ctx, L_ERR, "%s: get_network_status failed %d\n",
         __FUNCTION__, ret);
      return;
   }

   mocad_log(ctx, L_DEBUG, "pqos create rsp node id %u (ours %u)\n",
      BE32(pqosc_rsp->ingressnodeid), BE32(ns.node_id));

   if (BE32(ns.node_id) == BE32(pqosc_rsp->ingressnodeid)) {
      if ((BE32(pqosc_rsp->responsecode) == MOCA_L2_SUCCESS) &&
          (BE32(pqosc_rsp->decision) == MOCA_PQOS_DECISION_SUCCESS))
      {
         /* There should be an entry for this flow in the context */
         for (i = 0; i < MOCA_MAX_PQOS_ENTRIES; i++)
         {
            if (MOCA_MACADDR_COMPARE(&ctx->pqos_table[i].flow_id, &pqosc_rsp->flowid)) 
            {
               p_table = &ctx->pqos_table[i];
               break;
            }
         }

         if (p_table == NULL)
         {
            mocad_log(ctx, L_DEBUG, "%s: Couldn't find entry in table\n",
               __FUNCTION__);
            return;
         }

         MOCA_MACADDR_COPY(&req.flowid, &pqosc_rsp->flowid);
         req.flowtag = BE32(pqosc_rsp->flowtag);
         req.qtag = i;
         req.tpeakdatarate = BE32(pqosc_rsp->maxpeakdatarate);
         req.tpacketsize = BE32(pqosc_rsp->tpacketsize);
         req.tburstsize = BE32(pqosc_rsp->maxburstsize);
         req.tleasetime = BE32(pqosc_rsp->tleasetime);
         egressnodebitmask = BE32(pqosc_rsp->egressnodebitmask);
         while (egressnodebitmask && (nodeId < MOCA_MAX_NODES))
         {
            if (egressnodebitmask & 0x01)
            {
               egressnodebitmask >>= 1;
               if (egressnodebitmask)
                  nodeId = 0x3f;
               break;
            }
            egressnodebitmask >>= 1;
            nodeId++;
         }
         req.egressnodeid = nodeId;
         MOCA_MACADDR_COPY(&req.flowsa, &p_table->talker);
         MOCA_MACADDR_COPY(&req.flowda, &pqosc_rsp->flowda);
         req.flowvlanid = p_table->vlan_id;
         req.committedstps = BE32(pqosc_rsp->flowstps);
         req.committedtxps = BE32(pqosc_rsp->flowtxps);
         req.dest_flow_id = BE32(pqosc_rsp->dest_flow_id);
         req.maximum_latency = BE32(pqosc_rsp->maximum_latency);
         req.short_term_avg_ratio = BE32(pqosc_rsp->short_term_avg_ratio);
         req.max_number_retry = BE32(pqosc_rsp->max_number_retry);
         req.flow_per = BE32(pqosc_rsp->flow_per);
         req.in_order_delivery = BE32(pqosc_rsp->in_order_delivery);
         req.ingr_class_rule = BE32(pqosc_rsp->ingr_class_rule);
         req.traffic_protocol = BE32(pqosc_rsp->traffic_protocol);
         req.vlan_tag = BE32(pqosc_rsp->vlan_tag);
         req.dscp_moca = BE32(pqosc_rsp->dscp_moca);

         ret = __moca_set_pqos_ingr_add_flow(ctx, &req);
         if (ret != 0)
            mocad_log(ctx, L_ERR, 
               "Creating MoCA Ingress flow error! %d\n", ret);
      }
      else
      {
         MOCA_MACADDR_COPY(&pqos_table.flow_id, &pqosc_rsp->flowid);
         mocad_del_pqos_entry(ctx, &pqos_table);
      }
   }
}


MOCAD_S static void mocad_handle_pqos_create_cmp(struct mocad_ctx *ctx, 
   struct moca_pqos_create_complete * pqosc_cmp)
{
   struct moca_network_status ns;
   struct moca_pqos_ingr_add_flow req;
   uint32_t nodeId = 0;
   uint32_t egressnodebitmask;
   int ret;
   struct moca_pqos_table p_table;

   if ((BE32(pqosc_cmp->responsecode) == MOCA_L2_SUCCESS) &&
       (BE32(pqosc_cmp->decision) == MOCA_PQOS_DECISION_SUCCESS))
   {
      ret = mocad_cmd(ctx, MOCA_MSG_GET, IE_NETWORK_STATUS, NULL, 0,
         &ns, sizeof(ns), 0);

      if (ret != 0)
      {
         mocad_log(ctx, L_ERR, "%s: get_network_status failed %d\n",
            __FUNCTION__, ret);
         return;
      }

      mocad_log(ctx, L_DEBUG, "pqos create cmp node id %u (ours %u)\n",
         BE32(pqosc_cmp->ingressnodeid), BE32(ns.node_id)); 

      if (BE32(ns.node_id) == BE32(pqosc_cmp->ingressnodeid)) {
         /* Need to add an entry for this flow in the context */
         memset(&p_table, 0x0, sizeof(p_table));
         MOCA_MACADDR_COPY(&p_table.flow_id, &pqosc_cmp->flowid);
         MOCA_MACADDR_COPY(&p_table.packet_da, &pqosc_cmp->flowda);
         p_table.lease_time = BE32(pqosc_cmp->tleasetime);
         p_table.egress_node_mask = BE32(pqosc_cmp->egressnodebitmask);
         p_table.vlan_id = 0xFFF;
         p_table.vlan_prio = BE32(pqosc_cmp->vlan_tag);
         p_table.ingr_class_rule = BE32(pqosc_cmp->ingr_class_rule);
         p_table.dscp_moca = BE32(pqosc_cmp->dscp_moca);

         ret = mocad_add_pqos_entry(ctx, &p_table, (int *)&req.qtag);
         if (ret != 0)
         {
            mocad_log(ctx, L_ERR, "%s: unable to add entry\n",
               __FUNCTION__);
            return;
         }

         MOCA_MACADDR_COPY(&req.flowid, &pqosc_cmp->flowid);
         req.flowtag = BE32(pqosc_cmp->flowtag);
         req.tpeakdatarate = BE32(pqosc_cmp->maxpeakdatarate);
         req.tpacketsize = BE32(pqosc_cmp->tpacketsize);
         req.tburstsize = BE32(pqosc_cmp->maxburstsize);
         req.tleasetime = BE32(pqosc_cmp->tleasetime);
         egressnodebitmask = BE32(pqosc_cmp->egressnodebitmask);
         while (egressnodebitmask && (nodeId < MOCA_MAX_NODES)) {
            if (egressnodebitmask & 0x01)
            {
               egressnodebitmask >>= 1;
               if (egressnodebitmask)
                  nodeId = 0x3f;
               break;
            }
            egressnodebitmask >>= 1;
            nodeId++;
         }
         req.egressnodeid = nodeId;
         MOCA_MACADDR_COPY(&req.flowsa, &p_table.talker);
         MOCA_MACADDR_COPY(&req.flowda, &pqosc_cmp->flowda);
         req.flowvlanid = p_table.vlan_id;
         req.committedstps = BE32(pqosc_cmp->flowstps);
         req.committedtxps = BE32(pqosc_cmp->flowtxps);
         req.dest_flow_id = BE32(pqosc_cmp->dest_flow_id);
         req.maximum_latency = BE32(pqosc_cmp->maximum_latency);
         req.short_term_avg_ratio = BE32(pqosc_cmp->short_term_avg_ratio);
         req.max_number_retry = BE32(pqosc_cmp->max_number_retry);
         req.flow_per = BE32(pqosc_cmp->flow_per);
         req.in_order_delivery = BE32(pqosc_cmp->in_order_delivery);
         req.ingr_class_rule = BE32(pqosc_cmp->ingr_class_rule);
         req.traffic_protocol = BE32(pqosc_cmp->traffic_protocol);
         req.vlan_tag = BE32(pqosc_cmp->vlan_tag);
         req.dscp_moca = BE32(pqosc_cmp->dscp_moca);

         ret = __moca_set_pqos_ingr_add_flow(ctx, &req);
         if (ret != 0)
            mocad_log(ctx, L_ERR, 
               "Creating MoCA Ingress flow error! %d\n", ret);
      }
   }
}

MOCAD_S static void mocad_handle_pqos_update_rsp(struct mocad_ctx *ctx, 
   struct moca_pqos_update_response * pqosu_rsp)
{
   struct moca_pqos_ingr_update req;
   int ret;
   int index;
   struct moca_pqos_table * p_table = NULL;
   struct moca_pqos_update_flow_out * p_trap;

   if (ctx->do_flags & MOCAD_PQOS_UPDATE_FLOW)
   {
      ctx->do_flags &= ~MOCAD_PQOS_UPDATE_FLOW;

      p_trap = &ctx->mocad_trap.payload.pqosu_rsp;

      MOCA_MACADDR_COPY(&p_trap->flowid, &pqosu_rsp->flowid);
      p_trap->response_code = pqosu_rsp->responsecode;
      p_trap->decision = pqosu_rsp->decision;
      p_trap->flow_tag = pqosu_rsp->flowtag;
      MOCA_MACADDR_COPY(&p_trap->flowda, &pqosu_rsp->flowda);
      p_trap->peak_data_rate = pqosu_rsp->maxpeakdatarate;
      p_trap->packet_size = pqosu_rsp->tpacketsize;
      p_trap->burst_size = pqosu_rsp->maxburstsize;
      p_trap->lease_time = pqosu_rsp->tleasetime;
      p_trap->total_stps = pqosu_rsp->totalstps;
      p_trap->total_txps = pqosu_rsp->totaltxps;
      p_trap->flow_stps = pqosu_rsp->flowstps;
      p_trap->flow_txps = pqosu_rsp->flowtxps;
      p_trap->maximum_latency = pqosu_rsp->maximum_latency;
      p_trap->short_term_avg_ratio = pqosu_rsp->short_term_avg_ratio;
      p_trap->max_number_retry = pqosu_rsp->max_number_retry;
      p_trap->flow_per = pqosu_rsp->flow_per;
      p_trap->in_order_delivery = pqosu_rsp->in_order_delivery;
      p_trap->ingr_class_rule = pqosu_rsp->ingr_class_rule;
      p_trap->traffic_protocol = pqosu_rsp->traffic_protocol;
      p_trap->vlan_tag = pqosu_rsp->vlan_tag;
      p_trap->dscp_moca = pqosu_rsp->dscp_moca;
      p_trap->bw_limit_info = pqosu_rsp->bw_limit_info;
      p_trap->max_short_term_avg_ratio = pqosu_rsp->max_short_term_avg_ratio;

      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(*p_trap) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_PQOS_UPDATE_FLOW);

      ret = MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
         (const unsigned char *)&ctx->mocad_trap, 
         (sizeof(*p_trap) + sizeof(struct mmp_msg_hdr)));
   }

   if ((BE32(pqosu_rsp->responsecode) == MOCA_L2_SUCCESS) &&
       (BE32(pqosu_rsp->decision) == MOCA_PQOS_DECISION_SUCCESS))
   {
      /* There should be an entry for this flow in the context */
      for (index = 0; index < MOCA_MAX_PQOS_ENTRIES; index++)
      {
         if (MOCA_MACADDR_COMPARE(&ctx->pqos_table[index].flow_id, &pqosu_rsp->flowid))
         {
            p_table = &ctx->pqos_table[index];
            break;
         }
      }

      /* This update is not for us, nothing to do */
      if (p_table == NULL)
      {
         mocad_log(ctx, L_DEBUG, "%s: Couldn't find entry in table\n",
            __FUNCTION__);
         return;
      }

      MOCA_MACADDR_COPY(&req.flowid, &pqosu_rsp->flowid);
      req.flowtag = BE32(pqosu_rsp->flowtag);
      req.tpeakdatarate = BE32(pqosu_rsp->maxpeakdatarate);
      req.tpacketsize = BE32(pqosu_rsp->tpacketsize);
      req.tburstsize = BE32(pqosu_rsp->maxburstsize);
      req.tleasetime = BE32(pqosu_rsp->tleasetime);
      MOCA_MACADDR_COPY(&req.flowda, &pqosu_rsp->flowda);
      req.committedstps = BE32(pqosu_rsp->flowstps);
      req.committedtxps = BE32(pqosu_rsp->flowtxps);
      req.maximum_latency = BE32(pqosu_rsp->maximum_latency);
      req.short_term_avg_ratio = BE32(pqosu_rsp->short_term_avg_ratio);
      req.max_number_retry = BE32(pqosu_rsp->max_number_retry);
      req.flow_per = BE32(pqosu_rsp->flow_per);
      req.in_order_delivery = BE32(pqosu_rsp->in_order_delivery);
      req.ingr_class_rule = BE32(pqosu_rsp->ingr_class_rule);
      req.traffic_protocol = BE32(pqosu_rsp->traffic_protocol);
      req.vlan_tag = BE32(pqosu_rsp->vlan_tag);
      req.dscp_moca = BE32(pqosu_rsp->dscp_moca);

      /* update the relevant local table parameters */
      p_table->lease_time = BE32(pqosu_rsp->tleasetime);
      mocad_set_pqos_flow_create_time(ctx, index);
      
      ret = __moca_set_pqos_ingr_update(ctx, &req);
      if (ret != 0)
         mocad_log(ctx, L_ERR, "Update MoCA Ingress flow error! %d\n", ret);
   }
}


MOCAD_S static void mocad_handle_pqos_update_cmp(struct mocad_ctx *ctx, 
   struct moca_pqos_update_complete * pqosu_cmp)
{
   struct moca_pqos_ingr_update req;
   int ret;
   int index;
   struct moca_pqos_table * p_table = NULL;

   if ((BE32(pqosu_cmp->responsecode) == MOCA_L2_SUCCESS) &&
       (BE32(pqosu_cmp->decision) == MOCA_PQOS_DECISION_SUCCESS))
   {
      /* There should be an entry for this flow in the context */
      for (index = 0; index < MOCA_MAX_PQOS_ENTRIES; index++)
      {
         if (MOCA_MACADDR_COMPARE(&ctx->pqos_table[index].flow_id, &pqosu_cmp->flowid))
         {
            p_table = &ctx->pqos_table[index];
            break;
         }
      }

      if (p_table == NULL)
      {
         mocad_log(ctx, L_DEBUG, "%s: Couldn't find entry in table\n",
            __FUNCTION__);
         return;
      }

      MOCA_MACADDR_COPY(&req.flowid, &pqosu_cmp->flowid);
      req.flowtag = BE32(pqosu_cmp->flowtag);
      req.tpeakdatarate = BE32(pqosu_cmp->maxpeakdatarate);
      req.tpacketsize = BE32(pqosu_cmp->tpacketsize);
      req.tburstsize = BE32(pqosu_cmp->maxburstsize);
      req.tleasetime = BE32(pqosu_cmp->tleasetime);
      MOCA_MACADDR_COPY(&req.flowda, &pqosu_cmp->flowda);
      req.committedstps = BE32(pqosu_cmp->flowstps);
      req.committedtxps = BE32(pqosu_cmp->flowtxps);
      req.maximum_latency = BE32(pqosu_cmp->maximum_latency);
      req.short_term_avg_ratio = BE32(pqosu_cmp->short_term_avg_ratio);
      req.max_number_retry = BE32(pqosu_cmp->max_number_retry);
      req.flow_per = BE32(pqosu_cmp->flow_per);
      req.in_order_delivery = BE32(pqosu_cmp->in_order_delivery);
      req.ingr_class_rule = BE32(pqosu_cmp->ingr_class_rule);
      req.traffic_protocol = BE32(pqosu_cmp->traffic_protocol);
      req.vlan_tag = BE32(pqosu_cmp->vlan_tag);
      req.dscp_moca = BE32(pqosu_cmp->dscp_moca);

      /* update the relevant local table parameters */
      p_table->lease_time = BE32(pqosu_cmp->tleasetime);
      mocad_set_pqos_flow_create_time(ctx, index);

      ret = __moca_set_pqos_ingr_update(ctx, &req);
      if (ret != 0)
         mocad_log(ctx, L_ERR, 
            "Update MoCA Ingress flow error! %d\n", ret);
   }
}

MOCAD_S static void mocad_handle_pqos_delete_rsp(
   struct mocad_ctx *ctx, 
   struct moca_pqos_delete_response_int * pqosd_rsp)
{
   struct moca_pqos_ingr_delete req;
   struct moca_pqos_table p_table;
   struct moca_pqos_delete_flow_out * p_trap;
   int ret;

   if (ctx->do_flags & MOCAD_PQOS_DELETE_FLOW)
   {
      ctx->do_flags &= ~MOCAD_PQOS_DELETE_FLOW;

      p_trap = &ctx->mocad_trap.payload.pqosd_rsp;

      MOCA_MACADDR_COPY(&p_trap->flowid, &pqosd_rsp->flowid);
      p_trap->response_code = pqosd_rsp->responsecode;

      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(*p_trap) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_PQOS_DELETE_FLOW);

      ret = MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
         (const unsigned char *)&ctx->mocad_trap, 
         (sizeof(*p_trap) + sizeof(struct mmp_msg_hdr)));
   }


   if (BE32(pqosd_rsp->responsecode) == MOCA_L2_SUCCESS)
   {
      /* delete the flow from our table if it exists */
      p_table.flow_id = pqosd_rsp->flowid;
      ret = mocad_del_pqos_entry(ctx, &p_table);
      if (ret == 0)
      {
         /* found the flow in our table */
         MOCA_MACADDR_COPY(&req.flowid, &pqosd_rsp->flowid);
         __moca_set_pqos_ingr_delete(ctx, &req);
      }
   }
}

MOCAD_S static void mocad_handle_pqos_delete_cmp(
   struct mocad_ctx *ctx, 
   struct moca_pqos_delete_complete_int * pqosd_cmp)
{
   struct moca_pqos_ingr_delete req;
   struct moca_pqos_table p_table;
   int ret;

   if (BE32(pqosd_cmp->responsecode) == MOCA_L2_SUCCESS)
   {
      /* delete the flow from our table if it exists */
      p_table.flow_id = pqosd_cmp->flowid;
      ret = mocad_del_pqos_entry(ctx, &p_table);
      if (ret == 0)
      {
         /* found the flow in our table */
         MOCA_MACADDR_COPY(&req.flowid, &pqosd_cmp->flowid);
         __moca_set_pqos_ingr_delete(ctx, &req);
      }
   }
}


MOCAD_S static void mocad_handle_pqos_list_rsp(struct mocad_ctx *ctx, 
   struct moca_pqos_list_response * pqosl_rsp)
{
   uint32_t i;
   uint32_t num_flows = 0;
   struct moca_pqos_list_out * p_trap;

   if (ctx->do_flags & MOCAD_PQOS_LIST)
   {
      ctx->do_flags &= ~MOCAD_PQOS_LIST;

      p_trap = &ctx->mocad_trap.payload.pqosl_rsp;

      p_trap->response_code = pqosl_rsp->responsecode;
      p_trap->flow_update_count = pqosl_rsp->flow_update_count;
      p_trap->total_flow_id_count = pqosl_rsp->total_flow_id_count;

      for (i = 0; i < (sizeof(p_trap->flowid)/sizeof(p_trap->flowid[0])); i++)
      {
         MOCA_MACADDR_COPY(&p_trap->flowid[i], &pqosl_rsp->flowid[i]);
         if (!MOCA_MACADDR_IS_NULL(&p_trap->flowid[i]))
         {
            num_flows++;
         }
      }
      p_trap->num_ret_flow_ids = BE32(num_flows);
      
      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(*p_trap) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_PQOS_LIST);

      MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
         (const unsigned char *)&ctx->mocad_trap, 
         (sizeof(*p_trap) + sizeof(struct mmp_msg_hdr)));
   }
}


MOCAD_S static int mocad_pqos_create_error_trap(struct mocad_ctx *ctx, 
   uint32_t l2_status, uint32_t decision)
{
   struct moca_pqos_create_flow_out * p_trap;
   int ret;

   p_trap = &ctx->mocad_trap.payload.pqosc_rsp;

   memset(p_trap, 0, sizeof(*p_trap));

   p_trap->response_code = BE32(l2_status);
   p_trap->decision = BE32(decision);

   ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
   ctx->mocad_trap.hdr.length = BE32(sizeof(*p_trap) + MOCA_EXTRA_MMP_HDR_LEN);
   ctx->mocad_trap.hdr.ie_type = BE32(IE_PQOS_CREATE_FLOW);

   ret = MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
      (const unsigned char *)&ctx->mocad_trap, 
      (sizeof(*p_trap) + sizeof(struct mmp_msg_hdr)));

   return(ret);
}

MOCAD_S static int mocad_pqos_update_error_trap(struct mocad_ctx *ctx, 
   uint32_t l2_status, uint32_t decision)
{
   struct moca_pqos_update_flow_out * p_trap;
   int ret;

   p_trap = &ctx->mocad_trap.payload.pqosu_rsp;

   memset(p_trap, 0, sizeof(*p_trap));

   p_trap->response_code = BE32(l2_status);
   p_trap->decision = BE32(decision);

   ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
   ctx->mocad_trap.hdr.length = BE32(sizeof(*p_trap) + MOCA_EXTRA_MMP_HDR_LEN);
   ctx->mocad_trap.hdr.ie_type = BE32(IE_PQOS_UPDATE_FLOW);

   ret = MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
      (const unsigned char *)&ctx->mocad_trap, 
      (sizeof(*p_trap) + sizeof(struct mmp_msg_hdr)));

   return(ret);
}


MOCAD_S int mocad_pqos_update_flow_check(struct mocad_ctx * ctx, struct moca_pqos_update_flow_in * in, uint32_t flags)
{
   if ((uint32_t) BE32(in->packet_size) > ctx->any_time.pqos_max_packet_size)
      return MOCA_PQOS_UPDATE_FLOW_PACKET_SIZE_ERR;

   return 0;
}

MOCAD_S int mocad_pqos_create_flow_check(struct mocad_ctx * ctx, struct moca_pqos_create_flow_in * in, uint32_t flags)
{
   if ((uint32_t) BE32(in->packet_size) > ctx->any_time.pqos_max_packet_size)
      return MOCA_PQOS_CREATE_FLOW_PACKET_SIZE_ERR;

   return 0;
}

MOCAD_S static int mocad_do_pqos_flow_create(
   struct mocad_ctx * ctx, 
   void * data)
{
   struct moca_pqos_create_flow_in * pIn;
   struct moca_pqos_create_request req;
   struct moca_gen_node_status gns;
   struct moca_network_status nws;
   struct moca_pqos_table pqos;
   int ret = 0;
   int i;
   int ingress_node_found = 0;
   int egress_node_found = 0;
   unsigned int now = MoCAOS_GetTimeSec();
   
   pIn = (struct moca_pqos_create_flow_in *)data;

   ret = mocad_pqos_create_flow_check(ctx, pIn, ctx->config_flags);

   if (ret == 0) {
      ret = moca_pqos_create_flow_check(pIn, ctx->config_flags);
   }
   
   if (ret != 0) {
      mocad_log(ctx, L_ERR, "PQOS Flow Create parameter error.\n");
      if (ret == MOCA_PQOS_CREATE_FLOW_MAX_RETRY_ERR)
         ret = mocad_pqos_create_error_trap(ctx, MOCA_L2_TRANSACTION_CANT_START, MOCA_PQOS_DECISION_RETRY_NOTOK);
      else
         ret = mocad_pqos_create_error_trap(ctx, MOCA_L2_TRANSACTION_CANT_START, MOCA_PQOS_DECISION_INVALID_TSPEC);
      return(ret);
   }

   /* The FLOW_ID must be a group MAC address */
   if ((pIn->flow_id.addr[0] & 0x1) == 0)
   {
      ret = mocad_pqos_create_error_trap(ctx, MOCA_L2_TRANSACTION_CANT_START, MOCA_PQOS_DECISION_INVALID_TSPEC);
      return(ret);
   }

   if ((ctx->do_flags & MOCAD_PQOS_CREATE_FLOW) != 0)
   {
      /* Check to see if the flag can be expired */
      if (now - ctx->pqosc_time < MOCAD_PQOS_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "Another PQOS flow create is in progress.\n");
         return(ctx->do_flags);
      }
   }

   if ((ctx->do_flags & MOCAD_PQOS_STATUS) != 0)
   {
      /* Check to see if the flag can be expired */
      if (now - ctx->pqoss_time < MOCAD_PQOS_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "A PQOS flow status is in progress.\n");
         return(ctx->do_flags);
      }
   }

   memset(&req, 0, sizeof(req));
   memset(&pqos, 0, sizeof(pqos));

   /* get node bitmask */
   moca_get_network_status(ctx, &nws);

   MOCA_MACADDR_COPY(&req.flowid, &pIn->flow_id);
   req.tPacketSize = (uint16_t) BE32(pIn->packet_size);

   /* find ingress and egress node id, set wave0Nodemask */
   for(i = 0; i < MOCA_MAX_NODES; i++)
   {
      if(!(nws.connected_nodes & (1 << i)))
         continue;
      ret = moca_get_gen_node_status(ctx, i, &gns);

      if (ret == 0)
      {
         if ((gns.protocol_support >> 24) >= MoCA_VERSION_1_1)
            req.wave0Nodemask |= (1 << i);

         if (MOCA_MACADDR_COMPARE(&gns.eui, &pIn->ingress_node))
         {
            req.ingressNodeId = (uint8_t) i;
            ingress_node_found = 1;
         }
         else if (MOCA_MACADDR_COMPARE(&gns.eui, &pIn->egress_node))
         {
            req.egressNodeId = (uint8_t) i;
            egress_node_found = 1;
         }
      }
   }

   if (MOCA_MACADDR_IS_BCAST(&pIn->egress_node))
   {
      req.egressNodeId = 0x3F;
      egress_node_found = 1;
   }
   
   if (ingress_node_found == 0)
   {
      mocad_log(ctx, L_ERR, "PQOS Create: Unknown ingress node.\n");
      ret = mocad_pqos_create_error_trap(ctx, MOCA_L2_TRANSACTION_CANT_START, MOCA_PQOS_DECISION_INVALID_TSPEC);
      return(ret);
   }

   if (egress_node_found == 0)
   {
      mocad_log(ctx, L_ERR, "PQOS Create: Unknown egress node.\n");
      ret = mocad_pqos_create_error_trap(ctx, MOCA_L2_TRANSACTION_CANT_START, MOCA_PQOS_DECISION_INVALID_TSPEC);
      return(ret);
   }

   // If a unicast packet DA is specified, make sure it's in the forwarding
   // table for the associated egress node. 
   if ((pIn->packet_da.addr[0] & 0x1) == 0)
   {
      int da_match_found = 0;

      if (req.egressNodeId < MOCA_MAX_NODES)
      {
         int num_entries;

         // If we are the egress node, check the src_addr table
         if (nws.node_id == req.egressNodeId)
         {
            struct moca_src_addr src_addr_table[128];
            
            num_entries = moca_get_src_addr(ctx, &src_addr_table[0], sizeof(src_addr_table));
            for (i = 0; i < num_entries; i++)
            {
               if ((src_addr_table[i].moca_node_id == req.egressNodeId) &&
                   (MOCA_MACADDR_COMPARE(&src_addr_table[i].mac_addr, &pIn->packet_da)))
               {
                  da_match_found = 1;
                  break;
               }
            }
         }
         // The egress node is not us, check the uc_fwd table
         else
         {  
            struct moca_uc_fwd uc_fwd_table[128];

            num_entries = moca_get_uc_fwd(ctx, &uc_fwd_table[0], sizeof(uc_fwd_table));

            for (i = 0; i < num_entries; i++)
            {
               if ((uc_fwd_table[i].moca_dest_node_id == req.egressNodeId) &&
                   (MOCA_MACADDR_COMPARE(&uc_fwd_table[i].mac_addr, &pIn->packet_da)))
               {
                  da_match_found = 1;
                  break;
               }
            }
         }
      }

      if (da_match_found == 0)
      {
         mocad_log(ctx, L_ERR, "PQOS Create: Unknown packet DA.\n");
         ret = mocad_pqos_create_error_trap(ctx, MOCA_L2_TRANSACTION_CANT_START, MOCA_PQOS_DECISION_INVALID_TSPEC);
         return(ret);
      }
   }

   req.flowTag = BE32(pIn->flow_tag);
   MOCA_MACADDR_COPY(&req.packetda, &pIn->packet_da);
   req.tPeakDataRate = BE32(pIn->peak_data_rate);
   req.tLeaseTime = BE32(pIn->lease_time);
   req.tBurstSize = (uint8_t) BE32(pIn->burst_size);

   req.max_latency = (uint8_t) BE32(pIn->max_latency);
   req.short_term_avg_ratio = (uint8_t) BE32(pIn->short_term_avg_ratio);
   req.retry_per_delivery = ((uint8_t) BE32(pIn->max_retry) << 6) |
                            ((uint8_t) BE32(pIn->flow_per)  << 4) |
                            ((uint8_t) BE32(pIn->in_order_delivery));
   req.proto_rule_vlan_dscp = ((uint16_t) BE32(pIn->traffic_protocol) << 13) |
                              ((uint16_t) BE32(pIn->ingr_class_rule)  << 10) |
                              ((uint16_t) BE32(pIn->vlan_tag) << 7) |
                              ((uint16_t) BE32(pIn->dscp_moca) << 4);

   /* If we are the ingress node, add to the table */
   if (req.ingressNodeId == nws.node_id)
   {
      MOCA_MACADDR_COPY(&pqos.flow_id, &req.flowid);
      MOCA_MACADDR_COPY(&pqos.packet_da, &req.packetda);
      pqos.lease_time = req.tLeaseTime;
      pqos.vlan_prio = BE32(pIn->vlan_tag);
      pqos.vlan_id = BE32(pIn->vlan_id);
      pqos.ingr_class_rule = BE32(pIn->ingr_class_rule);
      pqos.dscp_moca = BE32(pIn->dscp_moca);

      if (req.egressNodeId == 0x3F)
         pqos.egress_node_mask = nws.connected_nodes;
      else
         pqos.egress_node_mask = (1 << req.egressNodeId);

      ret = mocad_add_pqos_entry(ctx, &pqos, NULL);
      if (ret != 0)
      {
         return (ret);
      }
   }

   ret = __moca_set_pqos_create_request(ctx, &req);
   if (ret != 0)
   {
      mocad_log(ctx, L_ERR, "__moca_set_pqos_create_request returned %d\n", ret);
      mocad_del_pqos_entry(ctx, &pqos);
      return (ret);
   }

   ctx->do_flags |= MOCAD_PQOS_CREATE_FLOW;
   ctx->pqosc_time = now;

   return(ret);
}

MOCAD_S static int mocad_do_pqos_flow_update(
   struct mocad_ctx * ctx, 
   void * data)
{
   struct moca_pqos_update_flow_in * pIn;
   struct moca_pqos_update_request req;
   struct moca_gen_node_status gns;
   struct moca_network_status nws;
   int ret = 0;
   int i;
   unsigned int now = MoCAOS_GetTimeSec();
   
   pIn = (struct moca_pqos_update_flow_in *)data;

   ret = mocad_pqos_update_flow_check(ctx, pIn, ctx->config_flags);

   if (ret == 0) {
      ret = moca_pqos_update_flow_check(pIn, ctx->config_flags);
   }

   if (ret != 0) {
      mocad_log(ctx, L_VERBOSE, "PQOS Flow Update parameter error.\n");
      ret = mocad_pqos_update_error_trap(ctx, MOCA_L2_TRANSACTION_CANT_START, MOCA_PQOS_DECISION_INVALID_TSPEC);
      return(ret);
   }

   if ((ctx->do_flags & MOCAD_PQOS_UPDATE_FLOW) != 0)
   {
      /* Check to see if the flag can be expired */
      if (now - ctx->pqosu_time < MOCAD_PQOS_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "Another PQOS flow update is in progress.\n");
         return(ctx->do_flags);
      }
   }

   memset(&req, 0, sizeof(req));

   /* get node bitmask */
   moca_get_network_status(ctx, &nws);

   MOCA_MACADDR_COPY(&req.flowid, &pIn->flow_id);
   req.tPacketSize = (uint16_t) BE32(pIn->packet_size);

   /* set wave0Nodemask to all 1.1+ nodes and find ingr and egr node IDs */
   req.ingressNodeId = MOCA_MAX_NODES;
   req.egressNodeId  = MOCA_MAX_NODES;
   for(i = 0; i < MOCA_MAX_NODES; i++)
   {
      if(!(nws.connected_nodes & (1 << i)))
         continue;
      ret = moca_get_gen_node_status(ctx, i, &gns);

      if (ret == 0)
      {
         if ((gns.protocol_support >> 24) >= MoCA_VERSION_1_1)
            req.wave0Nodemask |= (1 << i);
         if (MOCA_MACADDR_COMPARE(&pIn->ingress_mac, &gns.eui))
         {
            req.ingressNodeId = i;
         }
         if (MOCA_MACADDR_COMPARE(&pIn->egress_mac, &gns.eui))
         {
            req.egressNodeId = i;
         }
      }
   }

   req.flowTag = BE32(pIn->flow_tag);
   MOCA_MACADDR_COPY(&req.packetda, &pIn->flow_id);
   req.tPeakDataRate = BE32(pIn->peak_data_rate);
   req.tLeaseTime = BE32(pIn->lease_time);
   req.tBurstSize = (uint8_t) BE32(pIn->burst_size);

   req.max_latency = (uint8_t) BE32(pIn->max_latency);
   req.short_term_avg_ratio = (uint8_t) BE32(pIn->short_term_avg_ratio);
   req.retry_per_delivery = ((uint8_t) BE32(pIn->max_retry) << 6) |
                            ((uint8_t) BE32(pIn->flow_per)  << 4) |
                            ((uint8_t) BE32(pIn->in_order_delivery));
   req.proto_rule_vlan_dscp = ((uint16_t) BE32(pIn->traffic_protocol) << 13);

   ret = __moca_set_pqos_update_request(ctx, &req);
   if (ret != 0)
   {
      mocad_log(ctx, L_ERR, "__moca_set_pqos_update_request returned %d\n", ret);
      return (ret);
   }

   ctx->do_flags |= MOCAD_PQOS_UPDATE_FLOW;
   ctx->pqosu_time = now;

   return(ret);
}

MOCAD_S static int mocad_do_pqos_flow_delete(
   struct mocad_ctx * ctx, 
   void * data)
{
   macaddr_t * pIn;
   struct moca_pqos_delete_request req;
   struct moca_gen_node_status gns;
   struct moca_network_status nws;
   int ret = 0;
   int i;
   unsigned int now = MoCAOS_GetTimeSec();
   
   pIn = (macaddr_t *)data;

   if ((ctx->do_flags & MOCAD_PQOS_DELETE_FLOW) != 0)
   {
      /* Check to see if the flag can be expired */
      if (now - ctx->pqosd_time < MOCAD_PQOS_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "Another PQOS flow delete is in progress.\n");
         return(ctx->do_flags);
      }
   }

   memset(&req, 0, sizeof(req));

   /* get node bitmask */
   moca_get_network_status(ctx, &nws);

   MOCA_MACADDR_COPY(&req.flowid, pIn);

   /* Set wave0Nodemask to all 1.1+ nodes */
   for(i = 0; i < MOCA_MAX_NODES; i++)
   {
      if(!(nws.connected_nodes & (1 << i)))
         continue;
      ret = moca_get_gen_node_status(ctx, i, &gns);

      if (ret == 0)
      {
         if ((gns.protocol_support >> 24) >= MoCA_VERSION_1_1)
            req.wave0Nodemask |= (1 << i);
      }
   }

   ret = __moca_set_pqos_delete_request(ctx, &req);
   if (ret != 0)
   {
      mocad_log(ctx, L_ERR, "__moca_set_pqos_delete_request returned %d\n", ret);
      return (ret);
   }

   ctx->do_flags |= MOCAD_PQOS_DELETE_FLOW;
   ctx->pqosd_time = now;

   return(ret);
}


MOCAD_S static int mocad_do_pqos_flow_query(
   struct mocad_ctx * ctx, 
   void * data)
{
   macaddr_t * pIn;
   struct moca_pqos_query_request req;
   struct moca_gen_node_status gns;
   struct moca_network_status nws;
   int ret = 0;
   int i;
   unsigned int now = MoCAOS_GetTimeSec();
   
   pIn = (macaddr_t *)data;

   if ((ctx->do_flags & MOCAD_PQOS_QUERY) != 0)
   {
      /* Check to see if the flag can be expired */
      if (now - ctx->pqosq_time < MOCAD_PQOS_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "Another PQOS flow query is in progress.\n");
         return(ctx->do_flags);
      }
   }

   memset(&req, 0, sizeof(req));

   /* get node bitmask */
   moca_get_network_status(ctx, &nws);

   MOCA_MACADDR_COPY(&req.flowid, pIn);

   /* Set wave0Nodemask to all 1.1+ nodes */
   for(i = 0; i < MOCA_MAX_NODES; i++)
   {
      if(!(nws.connected_nodes & (1 << i)))
         continue;
      ret = moca_get_gen_node_status(ctx, i, &gns);

      if (ret == 0)
      {
         if ((gns.protocol_support >> 24) >= MoCA_VERSION_1_1)
            req.wave0Nodemask |= (1 << i);
      }
   }

   ret = __moca_set_pqos_query_request(ctx, &req);
   if (ret != 0)
   {
      mocad_log(ctx, L_ERR, "__moca_set_pqos_query_request returned %d\n", ret);
      return (ret);
   }

   ctx->do_flags |= MOCAD_PQOS_QUERY;
   ctx->pqosq_time = now;

   return(ret);
}


MOCAD_S static int mocad_do_pqos_status(
   struct mocad_ctx * ctx, 
   void * data)
{
   struct moca_pqos_create_request req;
   struct moca_gen_node_status gns;
   struct moca_network_status nws;
   int ret = 0;
   int i;
   unsigned int now = MoCAOS_GetTimeSec();
   
   if ((ctx->do_flags & MOCAD_PQOS_CREATE_FLOW) != 0)
   {
      /* Check to see if the flag can be expired */
      if (now - ctx->pqosc_time < MOCAD_PQOS_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "A PQOS flow create is in progress.\n");
         return(ctx->do_flags);
      }
   }

   if ((ctx->do_flags & MOCAD_PQOS_STATUS) != 0)
   {
      /* Check to see if the flag can be expired */
      if (now - ctx->pqoss_time < MOCAD_PQOS_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "Another PQOS flow status is in progress.\n");
         return(ctx->do_flags);
      }
   }

   memset(&req, 0, sizeof(req));

   /* get node bitmask */
   moca_get_network_status(ctx, &nws);

   /* Set wave0Nodemask to all 1.1+ nodes */
   for(i = 0; i < MOCA_MAX_NODES; i++)
   {
      if(!(nws.connected_nodes & (1 << i)))
         continue;
      ret = moca_get_gen_node_status(ctx, i, &gns);

      if (ret == 0)
      {
         if ((gns.protocol_support >> 24) >= MoCA_VERSION_1_1)
            req.wave0Nodemask |= (1 << i);
      }
   }

   srand((unsigned int)MoCAOS_GetTimeSec());
   req.flowid.addr[0] = rand() | 0x01;
   req.flowid.addr[1] = rand();
   req.flowid.addr[2] = rand();
   req.flowid.addr[3] = rand();
   req.flowid.addr[4] = rand();
   req.flowid.addr[5] = rand();
   req.tPacketSize = 1518;
   req.ingressNodeId = nws.node_id;
   req.egressNodeId = 0x3F;
   req.flowTag = MOCA_PQOS_CREATE_FLOW_FLOW_TAG_DEF;
   MOCA_MACADDR_COPY(&req.packetda, &req.flowid);
   req.tPeakDataRate = 0xFFFFFF;
   req.tLeaseTime = MOCA_PQOS_CREATE_FLOW_LEASE_TIME_DEF;
   req.tBurstSize = MOCA_PQOS_CREATE_FLOW_BURST_SIZE_DEF;
   req.max_latency = (uint8_t) MOCA_PQOS_CREATE_FLOW_MAX_LATENCY_DEF;
   req.short_term_avg_ratio = MOCA_PQOS_CREATE_FLOW_SHORT_TERM_AVG_RATIO_DEF;
   req.retry_per_delivery = (MOCA_PQOS_CREATE_FLOW_MAX_RETRY_DEF << 6) |
                            (MOCA_PQOS_CREATE_FLOW_FLOW_PER_DEF  << 4) |
                            (MOCA_PQOS_CREATE_FLOW_IN_ORDER_DELIVERY_DEF);
   req.proto_rule_vlan_dscp = (MOCA_PQOS_CREATE_FLOW_TRAFFIC_PROTOCOL_DEF << 13) |
                              (MOCA_PQOS_CREATE_FLOW_INGR_CLASS_RULE_DEF << 10) |
                              (MOCA_PQOS_CREATE_FLOW_VLAN_TAG_DEF << 7) |
                              (MOCA_PQOS_CREATE_FLOW_DSCP_MOCA_DEF << 4);

   ret = __moca_set_pqos_create_request(ctx, &req);
   if (ret != 0)
   {
      mocad_log(ctx,L_ERR, "__moca_set_pqos_create_request returned %d\n", ret);
      return (ret);
   }

   ctx->do_flags |= MOCAD_PQOS_STATUS;
   ctx->pqoss_time = now;

   return(ret);
}

MOCAD_S static int mocad_do_pqos_list(
   struct mocad_ctx * ctx, 
   void * data)
{
   struct moca_pqos_list_in * pIn;
   struct moca_pqos_list_request req;
   struct moca_gen_node_status gns;
   struct moca_network_status nws;
   unsigned int now = MoCAOS_GetTimeSec();
   int ret;
   int i;
   
   pIn = (struct moca_pqos_list_in *)data;

   if ((ctx->do_flags & MOCAD_PQOS_LIST) != 0)
   {
      /* Check to see if the flag can be expired */
      if (now - ctx->pqosl_time < MOCAD_PQOS_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "A PQOS flow list is in progress.\n");
         return(ctx->do_flags);
      }
   }

   memset(&req, 0, sizeof(req));

   req.flowStartIndex = BE32(pIn->flow_start_index);
   req.flowMaxReturn  = (uint8_t)BE16(pIn->flow_max_return);

   if (MOCA_MACADDR_IS_NULL(&pIn->ingr_node_mac))
   {
      /* use the node id from params */
      ret = moca_get_gen_node_status(ctx, BE32(pIn->ingr_node_id), &gns);
      if (ret == 0)
      {
         if ((gns.protocol_support >> 24) >= MoCA_VERSION_1_1)
            req.wave0Nodemask = 0x1 << BE32(pIn->ingr_node_id);
      }
   }
   else
   {  /* need to find the node id for this mac address */

      /* get node bitmask */
      moca_get_network_status(ctx, &nws);

      for(i = 0; i < MOCA_MAX_NODES; i++)
      {
         if(! (nws.connected_nodes & (1 << i)))
            continue;
         
         ret = moca_get_gen_node_status(ctx, i, &gns);
         if (ret == 0)
         {
            if (MOCA_MACADDR_COMPARE(&gns.eui, &pIn->ingr_node_mac))
            {
               if ((gns.protocol_support >> 24) >= MoCA_VERSION_1_1)
                  req.wave0Nodemask = 0x1 << i;
               break;
            }
         }
      }
   }

   if (req.wave0Nodemask != 0)
   {
      ret = __moca_set_pqos_list_request(ctx, &req);

      if (ret == 0)
      {
         ctx->do_flags |= MOCAD_PQOS_LIST;
         ctx->pqosl_time = now;
      }
   }
   else
   {
      ret = -1;
   }

   return(ret);
}


MOCAD_S static int mocad_do_dd_init(
   struct mocad_ctx * ctx, 
   void * data)
{
   struct moca_dd_request req;
   struct moca_gen_node_status gns;
   struct moca_network_status nws;
   unsigned int now = MoCAOS_GetTimeSec();
   int ret;
   int i;
   
   if ((ctx->do_flags & MOCAD_DD) != 0)
   {
      /* Check to see if the flag can be expired */
      if (now - ctx->dd_time < MOCAD_PQOS_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "A Device Discovery is in progress.\n");
         return(ctx->do_flags);
      }
   }

   memset(&req, 0, sizeof(req));

   /* get node bitmask */
   moca_get_network_status(ctx, &nws);

   mocad_log(ctx, L_DEBUG, "%s: connected nodes = 0x%x, wave0Nodemask = 0x%x\n", 
      __FUNCTION__, nws.connected_nodes, BE32(*(uint32_t *)data));
   for(i = 0; i < MOCA_MAX_NODES; i++)
   {
      if(! (nws.connected_nodes & (1 << i)))
         continue;
      
      ret = moca_get_gen_node_status(ctx, i, &gns);
      if (ret == 0)
      {
         mocad_log(ctx, L_DEBUG, "%s: node %d  protocol support = %x\n", __FUNCTION__, i, gns.protocol_support);
         if ((gns.protocol_support >> 24) >= MoCA_VERSION_2_0)
            req.wave0Nodemask |= (0x1 << i);
      }
   }

   req.wave0Nodemask &= BE32(*(uint32_t *)data);

   mocad_log(ctx, L_DEBUG, "%s: wave0Nodemask = %x\n", __FUNCTION__, req.wave0Nodemask);
   
   if (req.wave0Nodemask != 0)
   {
      ret = __moca_set_dd_request(ctx, &req);

      if (ret == 0)
      {
         ctx->do_flags |= MOCAD_DD;
         ctx->dd_time = now;
      }
   }
   else
   {
      ret = -1;
   }

   return(ret);
}


MOCAD_S int mocad_do_fmr_init(
   struct mocad_ctx *ctx,
   uint32_t          node_mask)
{
   unsigned int now = MoCAOS_GetTimeSec();
   struct moca_fmr_request in;
   struct moca_network_status ns;
   struct moca_gen_node_status gns;
   uint32_t i;
   int ret;

   /* Only allow one FMR request at a time, if there is a 
      pending one, check to see if the flag can be expired,
      otherwise reply with an error */
   if (ctx->fmr_count != 0)
   {
      if (now - ctx->fmr_time < MOCAD_PQOS_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "An FMR transaction is in progress.\n");
         return(ctx->fmr_count);
      }
      else
      {
         ctx->fmr_count = 0;
      }
   }
   
   ctx->fmr_time = now;
   
   /* Pre-fill the response structure with illegal
      values to indicate that it's unset */
   memset(&ctx->fmr_trap.fmr_rsp, 0xFF, sizeof(ctx->fmr_trap.fmr_rsp));

   if (node_mask == 0)
   {
      moca_get_network_status(ctx, &ns);

      /* Set nodemask to all 1.1+ nodes */
      for(i = 0; i < MOCA_MAX_NODES; i++) 
      {        
         if(! (ns.connected_nodes & (1 << i)))
             continue;

         ret = moca_get_gen_node_status(ctx, i, &gns);
         if ((ret == 0) &&           
             ((gns.protocol_support >> 24) >= MoCA_VERSION_1_1))
         {           
             node_mask |= (1 << i);
         }
      }  

      mocad_log(ctx, L_DEBUG, "FMR INIT: Forcing node mask to 0x%x\n", node_mask);
   }

   /* The firmware will only provide up to 9 nodes of info 
      at a time, so we might need to request two seperate
      times */
   if (node_mask & 0xFF)
   {
      in.wave0Nodemask = (node_mask & 0xFF);
      mocad_log(ctx, L_DEBUG, "FMR INIT: Send req to FW for mask 0x%x\n", in.wave0Nodemask);
      __moca_set_fmr_request(ctx, &in);
      ctx->fmr_count++;
   }
   if (node_mask & 0xFF00)
   {
      ctx->fmr_count++;
      ctx->fmr_rem_mask = node_mask & 0xFF00;
      mocad_log(ctx, L_DEBUG, "FMR INIT: Set remaining mask to 0x%x\n", ctx->fmr_rem_mask);
   }

   return(0);
}

MOCAD_S static int mocad_do_moca_reset(
   struct mocad_ctx * ctx, 
   void * data)
{
   struct moca_mr_request req;
   struct moca_gen_node_status gns;
   struct moca_network_status nws;
   struct moca_moca_reset_in * pIn;
   unsigned int now = MoCAOS_GetTimeSec();
   int ret;
   uint32_t i;

   pIn = (struct moca_moca_reset_in *)data;
   
   if ((ctx->do_flags & MOCAD_MOCA_RESET) != 0)
   {
      /* Check to see if the flag can be expired */
      if (now - ctx->mr_time < MOCAD_MR_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "A MoCA Reset operation is in progress.\n");
         return(ctx->do_flags);
      }
   }

   memset(&req, 0, sizeof(req));

   /* get node bitmask */
   moca_get_network_status(ctx, &nws);

   mocad_log(ctx, L_DEBUG, "%s: connected nodes = 0x%x, wave0Nodemask = 0x%x\n", 
      __FUNCTION__, nws.connected_nodes, BE32(*(uint32_t *)data));
   for(i = 0; i < MOCA_MAX_NODES; i++)
   {
      if( (! (nws.connected_nodes & (1 << i))) ||
          (i == nws.node_id) )
         continue;
      
      ret = moca_get_gen_node_status(ctx, i, &gns);
      if (ret == 0)
      {
         mocad_log(ctx, L_DEBUG, "%s: node %u  protocol support = %x\n", __FUNCTION__, i, gns.protocol_support);
         if ((gns.protocol_support >> 24) >= MoCA_VERSION_1_1)
            req.wave0Nodemask |= (0x1 << i);
      }
   }

   req.wave0Nodemask &= BE32(pIn->node_mask);
   req.resetTimer = (uint8_t) BE32(pIn->reset_timer);

   if (BE32(pIn->non_def_seq_num) == 0x10000)
   {
      req.nonDefSeqNum = (uint16_t)(ctx->disk_nondefseqnum);
   }
   else
   {
      req.nonDefSeqNum = (uint16_t) BE32(pIn->non_def_seq_num);
   }

   mocad_log(ctx, L_DEBUG, "%s: wave0Nodemask = %x  resetTimer = %d  nonDef = %d\n", 
   __FUNCTION__, req.wave0Nodemask, req.resetTimer, req.nonDefSeqNum);
   
   if (req.wave0Nodemask != 0)
   {
      ret = __moca_set_mr_request(ctx, &req);

      if (ret == 0)
      {
         ctx->do_flags |= MOCAD_MOCA_RESET;
         ctx->mr_time = now;
      }
   }
   else
   {
      ret = -1;
   }

   return(ret);
}

MOCAD_S uint32_t mocad_get_num_lsb(uint32_t value, uint32_t num_lsbs)
{
   uint32_t i = 0;
   uint32_t j = 0;
   uint32_t ret = 0;
   
   while((i < 32) && (j < num_lsbs))
   {
      if (value & (1 << i))
      {
         ret |= (1 << i);
         j++;
      }

      i++;
   }

   return(ret);
}

MOCAD_S int mocad_do_fmr_20(
   struct mocad_ctx *ctx,
   uint32_t          node_mask)
{
   unsigned int now = MoCAOS_GetTimeSec();
   struct moca_fmr20_request in;
   struct moca_network_status ns;
   struct moca_gen_node_status gns;
   uint32_t i;
   int ret;

   /* Only allow one FMR 2.0 request at a time, if there is a 
      pending one, check to see if the flag can be expired,
      otherwise reply with an error */
   if (ctx->fmr20_count != 0)
   {
      if (now - ctx->fmr20_time < MOCAD_PQOS_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "An FMR 2.0 transaction is in progress.\n");
         return(ctx->fmr20_count);
      }
      else
      {
         ctx->fmr20_count = 0;
      }
   }
   
   ctx->fmr20_time = now;
   
   /* Pre-fill the response structure with illegal
      values to indicate that it's unset */
   memset(&ctx->fmr20_trap.fmr20_rsp, 0xFF, sizeof(ctx->fmr20_trap.fmr20_rsp));

   if (node_mask == 0)
   {
      moca_get_network_status(ctx, &ns);

      /* Set nodemask to all 2.0+ nodes */
      for(i = 0; i < MOCA_MAX_NODES; i++) 
      {        
         if(! (ns.connected_nodes & (1 << i)))
             continue;

         ret = moca_get_gen_node_status(ctx, i, &gns);
         if ((ret == 0) &&           
             ((gns.protocol_support >> 24) >= MoCA_VERSION_2_0))
         {           
             node_mask |= (1 << i);
         }
      }  
      mocad_log(ctx, L_DEBUG, "FMR 2.0: Forcing node mask to 0x%x\n", node_mask);
   }

   /* The firmware will only provide up to 2 nodes of info 
      at a time, so we might need to request 6 seperate
      times */
   in.wave0Nodemask = mocad_get_num_lsb(node_mask, MOCA_MAX_FMR20_NODES);
   node_mask &= ~in.wave0Nodemask;
   
   if (in.wave0Nodemask)
   {
      mocad_log(ctx, L_DEBUG, "FMR 2.0: Send req to FW for mask 0x%x\n", in.wave0Nodemask);
      __moca_set_fmr20_request(ctx, &in);
      ctx->fmr20_count++;
   }

   if (node_mask)
   {
      ctx->fmr20_rem_mask = node_mask;
      mocad_log(ctx, L_DEBUG, "FMR 2.0: Set remaining mask to 0x%x\n", ctx->fmr20_rem_mask);
   }

   return(0);
}

MOCAD_S int mocad_do_aca(
   struct mocad_ctx *ctx,
   void * data)
{
   struct moca_aca_in * pAcaReq = (struct moca_aca_in *) data;
   unsigned int now = MoCAOS_GetTimeSec();
   struct moca_start_aca in;
   int ret;

   /* Only allow one ACA operation at a time, if there is a 
      pending one, check to see if the flag can be expired,
      otherwise reply with an error */
   if (ctx->aca_count != 0)
   {
      if (now - ctx->aca_time < MOCAD_ACA_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "An ACA transaction is in progress.\n");
         return(ctx->aca_count);
      }
      else
      {
         ctx->aca_count = 0;
      }
   }
   
   ctx->aca_time = now;
   
   /* Pre-fill the response structure with illegal
      values to indicate that it's unset */
   memset(&ctx->aca_trap.aca_rsp, 0, sizeof(ctx->aca_trap.aca_rsp));

   in.src_node      = BE32(pAcaReq->src_node);
   in.dest_nodemask = BE32(pAcaReq->dest_nodemask);
   in.type          = BE32(pAcaReq->type);
   in.channel       = BE32(pAcaReq->channel);
   in.num_probes    = BE32(pAcaReq->num_probes);
   
   ret = __moca_set_start_aca(ctx, &in);

   if (ret == MOCA_API_SUCCESS)
      ctx->aca_count = 2;

   return(ret);
}



MOCAD_S static void mocad_handle_pqos_query_rsp(
   struct mocad_ctx *ctx,
   struct moca_pqos_query_response * pqosq_rsp)
{
   struct moca_pqos_query_out * p_trap;
   struct moca_gen_node_status  gns;
   int ret;
   
   /* tpeakdatarate is a 24-bit field trapped in a u32 */
   pqosq_rsp->tpeakdatarate &= BE32(0xFFFFFF);

   if (ctx->do_flags & MOCAD_PQOS_QUERY)
   {
      ctx->do_flags &= ~MOCAD_PQOS_QUERY;

      p_trap = &ctx->mocad_trap.payload.pqosq_rsp;
      memset(p_trap, 0x0, sizeof(*p_trap));

      memcpy(&p_trap->flow_id.addr[0], &pqosq_rsp->flowid[0], sizeof(p_trap->flow_id.addr));
      memcpy(&p_trap->packet_da.addr[0], &pqosq_rsp->packetda[0], sizeof(p_trap->packet_da.addr));

      ret = moca_get_gen_node_status(ctx, pqosq_rsp->ingressnodeid, &gns);
      if (ret == 0)
         MOCA_MACADDR_COPY(&p_trap->ingress_node, &gns.eui);

      if ((pqosq_rsp->egressnodeid == 0x3F) ||
          (pqosq_rsp->egressnodeid == 0x3E))
      {
         memset(&p_trap->egress_node.addr[0], 0xFF, sizeof(p_trap->egress_node.addr));
      }
      else
      {
         ret = moca_get_gen_node_status(ctx, pqosq_rsp->egressnodeid, &gns);
         if (ret == 0)
            MOCA_MACADDR_COPY(&p_trap->egress_node, &gns.eui);
      }
      p_trap->response_code = pqosq_rsp->responsecode;
      p_trap->flow_tag = pqosq_rsp->flowtag;
      p_trap->peak_data_rate = pqosq_rsp->tpeakdatarate;
      p_trap->packet_size = BE32((uint32_t) BE16(pqosq_rsp->tpacketsize));
      p_trap->burst_size = BE32(pqosq_rsp->tburstsize);
      p_trap->lease_time = pqosq_rsp->tleasetime;
      p_trap->lease_time_left = pqosq_rsp->leasetimeleft;
      p_trap->dest_flow_id = BE32(pqosq_rsp->dest_flow_id);
      p_trap->max_latency = BE32(pqosq_rsp->max_latency);
      p_trap->short_term_avg_ratio = BE32(pqosq_rsp->short_term_avg_ratio);
      p_trap->max_retry = BE32((pqosq_rsp->max_retry_flow_per >> 6) & 0x3);
      p_trap->flow_per = BE32((pqosq_rsp->max_retry_flow_per >> 4) & 0x3);
      p_trap->ingr_class_rule = BE32((BE16(pqosq_rsp->icr_vlan_dscp_iod_prot) >> 13) & 0x7);
      p_trap->vlan_tag = BE32((BE16(pqosq_rsp->icr_vlan_dscp_iod_prot) >> 10) & 0x7);
      p_trap->dscp_moca = BE32((BE16(pqosq_rsp->icr_vlan_dscp_iod_prot) >> 7) & 0x7);
      p_trap->in_order_delivery = BE32((BE16(pqosq_rsp->icr_vlan_dscp_iod_prot) >> 3) & 0x3);
      p_trap->traffic_protocol = BE32(BE16(pqosq_rsp->icr_vlan_dscp_iod_prot) & 0x7);

      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(*p_trap) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_PQOS_QUERY);

      ret = MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
         (const unsigned char *)&ctx->mocad_trap, 
         (sizeof(*p_trap) + sizeof(struct mmp_msg_hdr)));
   }

}

MOCAD_S static void mocad_handle_fmr_response(
   struct mocad_ctx *ctx,
   struct moca_fmr_response * fmr_resp)
{
   int i;
   uint32_t * in_node;
   uint32_t * out_node;
   struct moca_fmr_request in;

   mocad_log(ctx, L_DEBUG, "%s: fmr_count %d\n", __FUNCTION__, ctx->fmr_count);

   if (ctx->fmr_count)
   {
      ctx->fmr_count--;

      mocad_log(ctx, L_DEBUG, "%s: response code %d\n", __FUNCTION__, ctx->fmr_trap.fmr_rsp.responsecode);
      if ((BE32(ctx->fmr_trap.fmr_rsp.responsecode) == 0xFFFFFFFF) ||
          (BE32(ctx->fmr_trap.fmr_rsp.responsecode) == MOCA_L2_SUCCESS))
      {
         /* Make sure to return an error if there is one */
         ctx->fmr_trap.fmr_rsp.responsecode = fmr_resp->responsecode;
      }

      /* Loop through the response trap and fill in the
         context response */
      for (i = 0; i < 9; i++)
      {
         in_node = (uint32_t *)((uint32_t)&fmr_resp->responded_node_0 + 
                                 (i * (sizeof(fmr_resp->responded_node_0) + 
                                       sizeof(fmr_resp->fmrinfo_node_0))));

         mocad_log(ctx, L_DEBUG, "%s: in_node %d\n", __FUNCTION__, BE32(*in_node));

         if (BE32(*in_node) != 0xFF)
         {
            if (BE32(*in_node) < MOCA_MAX_NODES)
            {
               out_node = (uint32_t *)((uint32_t)&ctx->fmr_trap.fmr_rsp.responded_node_0 + 
                  (BE32(*in_node) * (sizeof(ctx->fmr_trap.fmr_rsp.responded_node_0) + 
                                      sizeof(ctx->fmr_trap.fmr_rsp.fmrinfo_node_0))));

               *out_node = *in_node;

               memcpy((out_node + 1), (in_node + 1), sizeof(ctx->fmr_trap.fmr_rsp.fmrinfo_node_0));
            }
            else
            {
               mocad_log(ctx, L_ERR, "FMR responded with node ID %d\n", BE32(*in_node));
               return;
            }
         }
      }

      /* May need to perform the second half of the FMR operation */
      if (ctx->fmr_count > 0)
      {
         mocad_log(ctx, L_DEBUG, "%s: Send second FMR request for node mask 0x%x\n", __FUNCTION__, ctx->fmr_rem_mask);
         in.wave0Nodemask = ctx->fmr_rem_mask & 0xFF00;
         __moca_set_fmr_request(ctx, &in);
      }

      /* May need to send reply trap out to client */
      if (ctx->fmr_count == 0)
      {
         /* Convert any non-responded nodes from 0xFFFFFFFF to 0xFF */
         for (i = 0; i < MOCA_MAX_NODES; i++)
         {
            out_node = (uint32_t *)((uint32_t)&ctx->fmr_trap.fmr_rsp.responded_node_0 + 
               (i * (sizeof(ctx->fmr_trap.fmr_rsp.responded_node_0) + 
                     sizeof(ctx->fmr_trap.fmr_rsp.fmrinfo_node_0))));

            if (*out_node == 0xFFFFFFFF)
            {
               mocad_log(ctx, L_DEBUG, "%s: No response from node %d\n", __FUNCTION__, i);
               *out_node = BE32(0xFF);
               memset((out_node + 1), 0x0, sizeof(ctx->fmr_trap.fmr_rsp.fmrinfo_node_0));
            }
         }

         ctx->fmr_trap.hdr.type = BE32(MOCA_MSG_TRAP);
         ctx->fmr_trap.hdr.length = BE32(sizeof(struct moca_fmr_init_out) + MOCA_EXTRA_MMP_HDR_LEN);
         ctx->fmr_trap.hdr.ie_type = BE32(IE_FMR_INIT);

         MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
            (const unsigned char *)&ctx->fmr_trap, 
            (sizeof(struct moca_fmr_init_out) + sizeof(struct mmp_msg_hdr)));
      }
   }
}


MOCAD_S static void mocad_handle_fmr20_response(
   struct mocad_ctx *ctx,
   struct moca_fmr20_response * fmr20_resp)
{
   int i, j;
   uint32_t * in_node;
   struct moca_fmr_20_out * out_data;
   struct moca_fmr20_request in;
   struct moca_fmr20_node_data * node_data;
   uint32_t   fmr_data_len = ((uint32_t)&(((struct moca_fmr20_response *)0)->responded_node_1)) - 
                             ((uint32_t)&(((struct moca_fmr20_response *)0)->responded_node_0));
   uint32_t   trap_node_data_len = ((uint32_t)&(((struct moca_fmr_20_out *)0)->node1_gap_nper[0])) -
                                   ((uint32_t)&(((struct moca_fmr_20_out *)0)->node0_gap_nper[0]));

   mocad_log(ctx, L_DEBUG, "%s: fmr20_count %d\n", __FUNCTION__, ctx->fmr20_count);

   if (ctx->fmr20_count)
   {
      ctx->fmr20_count--;

      if ((BE32(ctx->fmr20_trap.fmr20_rsp.responsecode) == 0xFFFFFFFF) ||
          (BE32(ctx->fmr20_trap.fmr20_rsp.responsecode) == MOCA_L2_SUCCESS))
      {
         /* Make sure to return an error if there is one */
         ctx->fmr20_trap.fmr20_rsp.responsecode = fmr20_resp->responsecode;
         mocad_log(ctx, L_DEBUG, "%s: response code %d\n", __FUNCTION__, BE32(ctx->fmr20_trap.fmr20_rsp.responsecode));
      }

      /* Loop through the response trap and fill in the
         context response */
      for (i = 0; i < MOCA_MAX_FMR20_NODES; i++)
      {
         in_node = (uint32_t *)((uint32_t)&fmr20_resp->responded_node_0 + 
                                (i * fmr_data_len));

         mocad_log(ctx, L_DEBUG, "%s: in_node %d\n", __FUNCTION__, BE32(*in_node));

         if (BE32(*in_node) != 0xFF)
         {
            if (BE32(*in_node) < MOCA_MAX_NODES)
            {
               out_data = (struct moca_fmr_20_out *)((uint32_t)&ctx->fmr20_trap.fmr20_rsp.responsecode + 
                           (BE32(*in_node) * trap_node_data_len));

               node_data = (struct moca_fmr20_node_data *) (in_node + 1);

               /* Copy data */
               for (j = 0; j < MOCA_MAX_NODES; j++)
               {
                  out_data->node0_gap_nper[j] = node_data->tx_node_info[j].gap_nper;
                  out_data->node0_gap_vlper[j] = node_data->tx_node_info[j].gap_vlper;
                  out_data->node0_ofdmb_nper[j] = node_data->tx_node_info[j].ofdmb_nper;
                  out_data->node0_ofdmb_vlper[j] = node_data->tx_node_info[j].ofdmb_vlper;
               }

               out_data->node0_ofdmb_gcd = node_data->ofdmb_gcd;
               out_data->node0_gap_gcd = node_data->gap_gcd;
               out_data->node0_ofdma_def_tab_num = node_data->ofdma_def_tab_num;

               if (node_data->ofdma_def_tab_num > 4)
                  mocad_log(ctx, L_ERR, "Illegal OFDMA def tab num %d\n", node_data->ofdma_def_tab_num);
               
               for (j = 0; j < node_data->ofdma_def_tab_num; j++)
               {
                  out_data->node0_ofdma_tab_node_bitmask[j] = node_data->ofdma_tab_info[j].node_bitmask;
                  out_data->node0_ofdma_tab_num_subchan[j] = node_data->ofdma_tab_info[j].ofdma_subchan;
                  out_data->node0_ofdma_tab_gap[j] = node_data->ofdma_tab_info[j].ofdma_gap;
                  out_data->node0_ofdma_tab_bps[j] = node_data->ofdma_tab_info[j].ofdmab;
               }
               for (; j < 4; j++)
               {
                  out_data->node0_ofdma_tab_node_bitmask[j] = 0;
                  out_data->node0_ofdma_tab_num_subchan[j] = 0;
                  out_data->node0_ofdma_tab_gap[j] = 0;
                  out_data->node0_ofdma_tab_bps[j] = 0;
               }
            }
            else
            {
               mocad_log(ctx, L_ERR, "FMR 2.0 responded with node ID %d\n", BE32(*in_node));
               return;
            }
         }
      }

      /* May need to perform more waves of FMR 2.0 operation */
      if (ctx->fmr20_rem_mask)
      {
         mocad_log(ctx, L_DEBUG, "%s: Send another FMR 2.0 request for node mask 0x%x\n", __FUNCTION__, ctx->fmr20_rem_mask);
         in.wave0Nodemask = mocad_get_num_lsb(ctx->fmr20_rem_mask, MOCA_MAX_FMR20_NODES);
         ctx->fmr20_count++;
         ctx->fmr20_rem_mask &= ~in.wave0Nodemask;

         __moca_set_fmr20_request(ctx, &in);
      }

      /* May need to send reply trap out to client */
      if (ctx->fmr20_count == 0)
      {
         /* Convert any non-responded nodes from 0xFFFFFFFF to 0xFF */
         for (i = 0; i < MOCA_MAX_NODES; i++)
         {
            out_data = (struct moca_fmr_20_out *)((uint32_t)&ctx->fmr20_trap.fmr20_rsp.node0_gap_nper[0] + 
                        (i * trap_node_data_len));

            if (out_data->node0_ofdma_tab_node_bitmask[0] == 0xFFFFFFFF)
            {
               mocad_log(ctx, L_DEBUG, "%s: No response from node %d\n", __FUNCTION__, i);
               memset(out_data, 0x0, trap_node_data_len);
            }
         }

         ctx->fmr20_trap.hdr.type = BE32(MOCA_MSG_TRAP);
         ctx->fmr20_trap.hdr.length = BE32(sizeof(struct moca_fmr_20_out) + MOCA_EXTRA_MMP_HDR_LEN);
         ctx->fmr20_trap.hdr.ie_type = BE32(IE_FMR_20);

         MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
            (const unsigned char *)&ctx->fmr20_trap, 
            (sizeof(struct moca_fmr_20_out) + sizeof(struct mmp_msg_hdr)));
      }
   }
}


MOCAD_S static void mocad_handle_mr_response(
   struct mocad_ctx *ctx,
   struct moca_mr_response * mr_resp)
{
   struct moca_moca_reset_out * p_trap;

   if (ctx->do_flags & MOCAD_MOCA_RESET)
   {
      ctx->do_flags &= ~MOCAD_MOCA_RESET;

      p_trap = &ctx->mocad_trap.payload.mr_rsp;
      memset(p_trap, 0x0, sizeof(*p_trap));

      p_trap->response_code = mr_resp->ResponseCode;
      p_trap->reset_status = mr_resp->ResetStatus;
      p_trap->non_def_seq_num = mr_resp->NonDefSeqNum;
      p_trap->n00ResetStatus = mr_resp->n00ResetStatus;
      p_trap->n00RspCode = mr_resp->n00RspCode;
      p_trap->n01ResetStatus = mr_resp->n01ResetStatus;
      p_trap->n01RspCode = mr_resp->n01RspCode;
      p_trap->n02ResetStatus = mr_resp->n02ResetStatus;
      p_trap->n02RspCode = mr_resp->n02RspCode;
      p_trap->n03ResetStatus = mr_resp->n03ResetStatus;
      p_trap->n03RspCode = mr_resp->n03RspCode;
      p_trap->n04ResetStatus = mr_resp->n04ResetStatus;
      p_trap->n04RspCode = mr_resp->n04RspCode;
      p_trap->n05ResetStatus = mr_resp->n05ResetStatus;
      p_trap->n05RspCode = mr_resp->n05RspCode;
      p_trap->n06ResetStatus = mr_resp->n06ResetStatus;
      p_trap->n06RspCode = mr_resp->n06RspCode;
      p_trap->n07ResetStatus = mr_resp->n07ResetStatus;
      p_trap->n07RspCode = mr_resp->n07RspCode;
      p_trap->n08ResetStatus = mr_resp->n08ResetStatus;
      p_trap->n08RspCode = mr_resp->n08RspCode;
      p_trap->n09ResetStatus = mr_resp->n09ResetStatus;
      p_trap->n09RspCode = mr_resp->n09RspCode;
      p_trap->n10ResetStatus = mr_resp->n10ResetStatus;
      p_trap->n10RspCode = mr_resp->n10RspCode;
      p_trap->n11ResetStatus = mr_resp->n11ResetStatus;
      p_trap->n11RspCode = mr_resp->n11RspCode;
      p_trap->n12ResetStatus = mr_resp->n12ResetStatus;
      p_trap->n12RspCode = mr_resp->n12RspCode;
      p_trap->n13ResetStatus = mr_resp->n13ResetStatus;
      p_trap->n13RspCode = mr_resp->n13RspCode;
      p_trap->n14ResetStatus = mr_resp->n14ResetStatus;
      p_trap->n14RspCode = mr_resp->n14RspCode;
      p_trap->n15ResetStatus = mr_resp->n15ResetStatus;
      p_trap->n15RspCode = mr_resp->n15RspCode;

      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(*p_trap) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_MOCA_RESET);

      MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
         (const unsigned char *)&ctx->mocad_trap, 
         (sizeof(*p_trap) + sizeof(struct mmp_msg_hdr)));
   }
}

MOCAD_S static void mocad_handle_hostless_mode_response(
   struct mocad_ctx *ctx)
{
   uint32_t * p_trap;

   if (ctx->do_flags & MOCAD_HOSTLESS_MODE)
   {
      int ret;
      ctx->do_flags &= ~MOCAD_HOSTLESS_MODE;

      p_trap = &ctx->mocad_trap.payload.hostless_mode_rsp;
      memset(p_trap, 0x0, sizeof(*p_trap));
     
      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(*p_trap) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_HOSTLESS_MODE);

      MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST,
         (const unsigned char *)&ctx->mocad_trap,
         (sizeof(*p_trap) + sizeof(struct mmp_msg_hdr)));

      // Acknowledge to bmoca that we've entered hostless mode now
      // Do this last so there is nothing more to do before we get frozen
      if (ctx->hostless_mode && (ret = MoCAOS_PMDone(ctx->os_handle)) != 0)
         mocad_log(ctx, L_ALWAYS, "MoCAOS_PMDone failed %d\n", ret);
   }
}

MOCAD_S static void mocad_handle_wakeup_node_response(
   struct mocad_ctx *ctx)
{
   uint32_t * p_trap;

   if (ctx->do_flags & MOCAD_WAKEUP_NODE)
   {
      ctx->do_flags &= ~MOCAD_WAKEUP_NODE;

      p_trap = &ctx->mocad_trap.payload.wakeup_node_rsp;
      memset(p_trap, 0x0, sizeof(*p_trap));
     
      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(*p_trap) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_WAKEUP_NODE);

      MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST,
         (const unsigned char *)&ctx->mocad_trap,
         (sizeof(*p_trap) + sizeof(struct mmp_msg_hdr)));
   }
}

MOCAD_S static void mocad_handle_dd_response(
   struct mocad_ctx *ctx,
   struct moca_dd_response * dd_resp)
{
   struct moca_dd_init_out * p_trap;
   struct mocad_node_attrs_hdr * p_attr_hdr;
   struct mocad_attr_ie_hdr * p_attr_ie_hdr;
   uint32_t num_nodes;
   uint32_t num_attrs;

   if (ctx->do_flags & MOCAD_DD)
   {
      ctx->do_flags &= ~MOCAD_DD;

      p_trap = &ctx->mocad_trap.payload.dd_rsp;
      memset(p_trap, 0x0, sizeof(*p_trap));

      p_trap->responsecode = dd_resp->responsecode;

      num_nodes = dd_resp->num_nodes;
      
      /* Walk through the trap message from the firmware to decipher
         the attributes of each responded node */
      p_attr_hdr = (struct mocad_node_attrs_hdr *)((uint32_t) &dd_resp->data[0]);
      while (num_nodes > 0)
      {
         if (p_attr_hdr->node_id >= MOCA_MAX_NODES)
         {
            mocad_log(ctx, L_ERR, "Invalid node ID 0x%x in DD response\n", p_attr_hdr->node_id);
            return;
         }
         num_attrs = p_attr_hdr->num_attrs;
         mocad_log(ctx, L_DEBUG, "%d attrs reported for node %d\n", p_attr_hdr->num_attrs, p_attr_hdr->node_id);

         p_trap->responded_nodemask |= BE32(1 << p_attr_hdr->node_id);
         p_attr_ie_hdr = (struct mocad_attr_ie_hdr *) ((uint32_t)(p_attr_hdr + 1));
         while (num_attrs > 0)
         {
            switch (p_attr_ie_hdr->attr_id)
            {
               case MOCAD_DEVICE_ATTRS_PQOS:
                  mocad_log(ctx, L_DEBUG, "PQOS Attr = 0x%x 0x%x\n", p_attr_ie_hdr->data[0], p_attr_ie_hdr->data[1]);
                  p_trap->ingress_pqos_flows[p_attr_hdr->node_id] = BE32((uint32_t)p_attr_ie_hdr->data[0]);
                  p_trap->egress_pqos_flows[p_attr_hdr->node_id] = BE32((uint32_t)p_attr_ie_hdr->data[1]);
                  break;
               case MOCAD_DEVICE_ATTRS_AGGR:
                  mocad_log(ctx, L_DEBUG, "AGGR Attr = 0x%x 0x%x\n", p_attr_ie_hdr->data[0], p_attr_ie_hdr->data[1]);
                  p_trap->aggr_pdus[p_attr_hdr->node_id] = BE32((uint32_t)p_attr_ie_hdr->data[0]);

                  switch(p_attr_ie_hdr->data[1])
                  {
                     case 0: // 12 KB
                        p_trap->aggr_size[p_attr_hdr->node_id] = BE32(12 << 10);
                        break;
                     case 1: // 16 KB
                        p_trap->aggr_size[p_attr_hdr->node_id] = BE32(16 << 10);
                        break;
                     case 2: // 24 KB
                        p_trap->aggr_size[p_attr_hdr->node_id] = BE32(24 << 10);
                        break;
                     case 3: // 32 KB
                        p_trap->aggr_size[p_attr_hdr->node_id] = BE32(32 << 10);
                        break;
                     case 4: // 48 KB
                        p_trap->aggr_size[p_attr_hdr->node_id] = BE32(48 << 10);
                        break;
                     case 5: // 64 KB
                        p_trap->aggr_size[p_attr_hdr->node_id] = BE32(64 << 10);
                        break;
                     default:
                        mocad_log(ctx, L_WARN, "Unrecognized Aggregation Size (%d) from node %d\n",
                           p_attr_ie_hdr->data[1], p_attr_hdr->node_id);
                  }
                  break;
               case MOCAD_DEVICE_ATTRS_MAP_PROC:
                  mocad_log(ctx, L_DEBUG, "AENUM Attr = 0x%x\n", p_attr_ie_hdr->data[0]);
                  p_trap->ae_number[p_attr_hdr->node_id] = BE32((uint32_t)p_attr_ie_hdr->data[0]);
                  break;
               case MOCAD_DEVICE_ATTRS_BONDED_AGGR:
                  mocad_log(ctx, L_DEBUG, "Bonded AGGR Attr = 0x%x 0x%x\n", p_attr_ie_hdr->data[0], p_attr_ie_hdr->data[1]);
                  p_trap->aggr_pdus_bonded[p_attr_hdr->node_id] = BE32((uint32_t)p_attr_ie_hdr->data[0]);

                  switch(p_attr_ie_hdr->data[1])
                  {
                     case 1: // 18 KB
                        p_trap->aggr_size_bonded[p_attr_hdr->node_id] = BE32(18 << 10);
                        break;
                     case 2: // 24 KB
                        p_trap->aggr_size_bonded[p_attr_hdr->node_id] = BE32(24 << 10);
                        break;
                     case 3: // 32 KB
                        p_trap->aggr_size_bonded[p_attr_hdr->node_id] = BE32(32 << 10);
                        break;
                     case 4: // 48 KB
                        p_trap->aggr_size_bonded[p_attr_hdr->node_id] = BE32(48 << 10);
                        break;
                     case 5: // 64 KB
                        p_trap->aggr_size_bonded[p_attr_hdr->node_id] = BE32(64 << 10);
                        break;
                     default:
                        mocad_log(ctx, L_WARN, "Unrecognized Bonded Aggregation Size (%d) from node %d\n",
                           p_attr_ie_hdr->data[1], p_attr_hdr->node_id);
                  }
                  break;
               default:
                  mocad_log(ctx, L_VERBOSE, "Unrecognized attribute type %d from node %d\n", p_attr_ie_hdr->attr_id, p_attr_hdr->node_id);
                  break;
            }

            p_attr_ie_hdr += p_attr_ie_hdr->length + 1;
            num_attrs--;
         }
         
         p_attr_hdr = (struct mocad_node_attrs_hdr *) ((uint32_t) p_attr_ie_hdr);
         num_nodes--;
      }
      
      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(*p_trap) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_DD_INIT);

      MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
         (const unsigned char *)&ctx->mocad_trap, 
         (sizeof(*p_trap) + sizeof(struct mmp_msg_hdr)));
   }
}


MOCAD_S static void mocad_handle_aca_response(
   struct mocad_ctx *ctx,
   void * aca_resp,
   uint32_t ie)
{
   struct moca_aca_out * pTrap = &ctx->aca_trap.aca_rsp;
   struct moca_aca_result_1 * pResult1 = NULL;
   struct moca_aca_result_2 * pResult2 = NULL;

   mocad_log(ctx, L_DEBUG, "%s: aca_count %d\n", __FUNCTION__, ctx->aca_count);

   if (ctx->aca_count)
   {
      switch (ie)
      {
         case IE_ACA_RESULT_1:
            pResult1 = (struct moca_aca_result_1 *)aca_resp;
            pTrap->aca_status = pResult1->aca_status;
            pTrap->aca_type = pResult1->aca_type;
            pTrap->tx_status = pResult1->tx_status;
            pTrap->rx_status = pResult1->rx_status;
            pTrap->total_power = pResult1->total_power;
            pTrap->relative_power = pResult1->relative_power;
            pTrap->num_elements = pResult1->num_elements;
            memcpy(&pTrap->power_profile[0], &pResult1->power_profile[0], sizeof(pResult1->power_profile));
            break;
         case IE_ACA_RESULT_2:
            pResult2 = (struct moca_aca_result_2 *)aca_resp;
            memcpy(&pTrap->power_profile[sizeof(pResult1->power_profile)], &pResult2->power_profile[0], sizeof(pResult2->power_profile));
            break;
         default:
            mocad_log(ctx, L_ERR, "Unknown IE for ACA trap 0x%x\n", ie);
            return;
      }

      ctx->aca_count--;

      /* May need to send reply trap out to client */
      if (ctx->aca_count == 0)
      {
         ctx->aca_trap.hdr.type = BE32(MOCA_MSG_TRAP);
         ctx->aca_trap.hdr.length = BE32(sizeof(struct moca_aca_out) + MOCA_EXTRA_MMP_HDR_LEN);
         ctx->aca_trap.hdr.ie_type = BE32(IE_ACA);

         MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
            (const unsigned char *)&ctx->aca_trap, 
            (sizeof(struct moca_aca_out) + sizeof(struct mmp_msg_hdr)));
      }
   }
   else
   {
      mocad_log(ctx, L_WARN, "Unexpected ACA trap.\n");
   }
}

MOCAD_S static void mocad_bcast_link_down(struct mocad_ctx *ctx)
{
   struct {
      struct mmp_msg_hdr mh;
      uint32_t state;
   } __attribute__((packed)) msg;

   msg.state = 0;
   msg.mh.type = BE32(MOCA_MSG_TRAP);
   msg.mh.length = BE32(sizeof(msg.state) + MOCA_EXTRA_MMP_HDR_LEN);
   msg.mh.ie_type = BE32(IE_LINK_UP_STATE);

   MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
                  (const unsigned char *)&msg, 
                  sizeof(msg));
}

MOCAD_S static int mocad_do_ps_cmd(
   struct mocad_ctx * ctx, 
   void * data)
{
   uint32_t req;
   unsigned int now = MoCAOS_GetTimeSec();
   int ret;

   if ((ctx->do_flags & MOCAD_PS_CMD) != 0)
   {
      /* Check to see if the flag can be expired */
      if (now - ctx->ps_cmd_time < MOCAD_PS_CMD_TIMEOUT)
      {
         mocad_log(ctx, L_WARN, 
            "A Power State Command is in progress.\n");
         return(ctx->do_flags);
      }
   }

   // Pass the WoM mode to FW first
   if ((ret = __moca_set_wom_mode_internal(ctx, ctx->any_time.wom_mode)) != MOCA_API_SUCCESS) {
      mocad_log(ctx, L_WARN, "mocad_do_ps_cmd: Failed to set WoM mode (%d)\n", ret);
   }
   
   req = BE32(*(uint32_t *)data);

   ret = __moca_set_power_state(ctx, req);

   if (ret == 0)
   {
      ctx->do_flags |= MOCAD_PS_CMD;
      ctx->ps_cmd_time = now;
   }
   else
   {
      ret = -1;
   }

   return(ret);
}

MOCAD_S static void mocad_handle_power_state_rsp(
   struct mocad_ctx *ctx,
   uint32_t power_state_rsp)
{
   
   if (ctx->do_flags & MOCAD_PS_CMD)
   {
      ctx->do_flags &= ~MOCAD_PS_CMD;

      ctx->mocad_trap.payload.ps_cmd_rsp = power_state_rsp;
      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(power_state_rsp) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_PS_CMD);

      MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
         (const unsigned char *)&ctx->mocad_trap, 
         (sizeof(power_state_rsp) + sizeof(struct mmp_msg_hdr)));
   }
}


#if 0 
WAS USED FOR ASSERTION PRINTING, may be reintroduced later
static void mocad_print_stats(struct mocad_ctx *ctx)
{
   int i;
   
   // print ctx->gen_stats and ctx->ext_stats
   printf("in_uc_packets          : %u\n", (unsigned int)ctx->gen_stats.in_uc_packets);
   printf("in_discard_packets_ecl : %u\n", (unsigned int)ctx->gen_stats.in_discard_packets_ecl);
   printf("in_discard_packets_mac : %u\n", (unsigned int)ctx->gen_stats.in_discard_packets_mac);
   printf("in_unknown_packets     : %u\n", (unsigned int)ctx->gen_stats.in_unknown_packets);
   printf("in_mc_packets          : %u\n", (unsigned int)ctx->gen_stats.in_mc_packets);
   printf("in_bc_packets          : %u\n", (unsigned int)ctx->gen_stats.in_bc_packets);
   printf("in_octets              : %u\n", (unsigned int)ctx->gen_stats.in_octets);
   printf("out_uc_packets         : %u\n", (unsigned int)ctx->gen_stats.out_uc_packets);
   printf("out_discard_packets    : %u\n", (unsigned int)ctx->gen_stats.out_discard_packets);
   printf("out_bc_packets         : %u\n", (unsigned int)ctx->gen_stats.out_bc_packets);
   printf("out_octets             : %u\n", (unsigned int)ctx->gen_stats.out_octets);
   printf("nc_handoff_counter     : %u\n", (unsigned int)ctx->gen_stats.nc_handoff_counter);
   printf("nc_backup_counter      : %u\n", (unsigned int)ctx->gen_stats.nc_backup_counter);
   
   for (i=0;i<10;i++)
   {
      printf("aggr_pkt_stats_tx[%d]   : %-8u   ", i, (unsigned int)ctx->gen_stats.aggr_pkt_stats_tx[i]);
      printf("aggr_pkt_stats_rx[%d]   : %-8u\n", i, (unsigned int)ctx->gen_stats.aggr_pkt_stats_rx[i]);
   }
   printf("received_data_filtered : %u\n", (unsigned int)ctx->gen_stats.received_data_filtered);

   printf("\n");

   printf("received_map_packets        : %u\n", (unsigned int)ctx->gen_stats.received_map_packets);
   printf("received_rr_packets         : %u\n", (unsigned int)ctx->gen_stats.received_rr_packets);
   printf("received_beacons            : %u\n", (unsigned int)ctx->gen_stats.received_beacons);
   printf("received_control_packets    : %u\n", (unsigned int)ctx->gen_stats.received_control_packets);
   printf("transmitted_beacons         : %u\n", (unsigned int)ctx->gen_stats.transmitted_beacons);
   printf("transmitted_maps            : %u\n", (unsigned int)ctx->gen_stats.transmitted_maps);
   printf("transmitted_lcs             : %u\n", (unsigned int)ctx->gen_stats.transmitted_lcs);
   printf("transmitted_rr_packets      : %u\n", (unsigned int)ctx->gen_stats.transmitted_rr_packets);
   printf("resync_attempts_to_network  : %u\n", (unsigned int)ctx->gen_stats.resync_attempts_to_network);
   printf("gmii_tx_buffer_full_counter : %u\n", (unsigned int)ctx->gen_stats.gmii_tx_buffer_full_counter);
   printf("moca_rx_buffer_full_counter : %u\n", (unsigned int)ctx->gen_stats.moca_rx_buffer_full_counter);
   printf("this_handoff_counter        : %u\n", (unsigned int)ctx->gen_stats.this_handoff_counter);
   printf("this_backup_counter         : %u\n", (unsigned int)ctx->gen_stats.this_backup_counter);

   for(i=0;i<3;i++)
      printf("fc_counter[%d]               : %u\n", i, (unsigned int)ctx->gen_stats.fc_counter[i]);
   
   printf("transmitted_protocol_ie     : %u\n", (unsigned int)ctx->gen_stats.transmitted_protocol_ie);
   printf("received_protocol_ie        : %u\n", (unsigned int)ctx->gen_stats.received_protocol_ie);
   printf("transmitted_time_ie         : %u\n", (unsigned int)ctx->gen_stats.transmitted_time_ie);
   printf("received_time_ie            : %u\n", (unsigned int)ctx->gen_stats.received_time_ie);
   printf("rx_lc_adm_req_crc_error     : %u\n", (unsigned int)ctx->gen_stats.rx_lc_adm_req_crc_error);
}

#endif

/*
      The RTT Buffer looks like this:

      <-       64       ->
      <- 12 -><-   52   ->
        ctrl     payload
*/
enum 
{
   RTT_CONTROL_OWNERSHIP_FW = 0,
   RTT_CONTROL_OWNERSHIP_HOST,
   RTT_CONTROL_OWNERSHIP_LAST
};
      

// a_pRttControl is already allocated
// a_rttBuffer is the address of the RTT buffer
MOCAD_S int32_t mocad_read_rtt_payload(struct mocad_ctx *ctx, unsigned char * a_pRttPayload, uint32_t a_rttBuffer)
{
   void        * pMem;
   void        * pMemBigEnd;
   int           ret;
   rtt_control_t rttControl;

   mocad_read_rtt_control(ctx, &rttControl, a_rttBuffer);

   if (rttControl.length > RTT_PAYLOAD_LENGTH)
   {
      mocad_log(ctx, L_ERR, "RTT payload length (%d) is larger than what is supported! Please enlarge RTT_PAYLOAD_LENGTH!\n", rttControl.length);
      return -1;
   }

   /* the memory transfer at the kernel layer might require 64-bit alignment */
   if ((ret = MoCAOS_MemAlign(&pMem, 64, rttControl.length)) < 0)
   {
      mocad_log(ctx, L_ERR, "Unable to allocate memory from kernel (err %d)\n", ret);
      return -1;
   }

   /* the memory transfer at the kernel layer might require 64-bit alignment */
   if ((ret = MoCAOS_MemAlign(&pMemBigEnd, 64, rttControl.length)) < 0)
   {
      mocad_log(ctx, L_ERR, "Unable to allocate memory from kernel (err %d)\n", ret);
      MoCAOS_FreeMemAlign(pMem);
      return -1;
   }

   if ((ret = MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)pMem, rttControl.length, (unsigned char *)((a_rttBuffer+sizeof(rtt_control_t))))) != 0)
   {
      mocad_log(ctx, L_ERR, "Unable to read memory from kernel (err %d, addr=%0x%08X, len=%d)\n", ret,a_rttBuffer+sizeof(rtt_control_t), rttControl.length);
      MoCAOS_FreeMemAlign(pMem);
      MoCAOS_FreeMemAlign(pMemBigEnd);
      return -1;
   }

   __moca_copy_be32(pMemBigEnd, pMem, rttControl.length);

   memcpy(a_pRttPayload, pMemBigEnd, rttControl.length);

   MoCAOS_FreeMemAlign(pMem);
   MoCAOS_FreeMemAlign(pMemBigEnd);

   return rttControl.length;
}

// a_pRttControl is already allocated
// a_rttBuffer is the address of the RTT buffer
MOCAD_S int32_t mocad_read_rtt_control(struct mocad_ctx *ctx, rtt_control_t * a_pRttControl, uint32_t a_rttBuffer)
{
   void * pMem;
   void * pMemBigEnd;
   int    ret;

   /* the memory transfer at the kernel layer might require 64-bit alignment */
   if ((ret = MoCAOS_MemAlign(&pMem, 64, sizeof(*a_pRttControl))) < 0)
   {
      mocad_log(ctx, L_ERR, "Unable to allocate memory from kernel (err %d)\n", ret);
      return -1;
   }

   /* the memory transfer at the kernel layer might require 64-bit alignment */
   if ((ret = MoCAOS_MemAlign(&pMemBigEnd, 64, sizeof(*a_pRttControl))) < 0)
   {
      mocad_log(ctx, L_ERR, "Unable to allocate memory from kernel (err %d)\n", ret);
      MoCAOS_FreeMemAlign(pMem);
      return -1;
   }

   if ((ret = MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)pMem, sizeof(*a_pRttControl), (unsigned char *)(a_rttBuffer & 0xFFFFFFF8))) != 0)
   {
      mocad_log(ctx, L_ERR, "Unable to read memory from kernel (err %d)\n", ret);
   }

   __moca_copy_be32(pMemBigEnd, pMem, sizeof(*a_pRttControl));

   memcpy(a_pRttControl, pMemBigEnd, sizeof(*a_pRttControl));

   MoCAOS_FreeMemAlign(pMem);
   MoCAOS_FreeMemAlign(pMemBigEnd);

   return sizeof(*a_pRttControl);
}

MOCAD_S void mocad_read_rtt_control_timeout(struct mocad_ctx *ctx, rtt_control_t * a_pRttControl, uint32_t a_rttBuffer, uint32_t timeout_s)
{
   unsigned int start_time = MoCAOS_GetTimeSec();
   int32_t      bytes_read;

   do 
   {
      if ((MoCAOS_GetTimeSec() - start_time) > timeout_s)
      {
         a_pRttControl->rtt_buffer_size = 0;
         return;
      }

      bytes_read = mocad_read_rtt_control(ctx, a_pRttControl, a_rttBuffer);
   } while ((bytes_read > 0) && (a_pRttControl->ownership != RTT_CONTROL_OWNERSHIP_HOST));
}

MOCAD_S int32_t mocad_write_rtt_control(struct mocad_ctx *ctx, rtt_control_t * a_pRttControl, uint32_t a_rttBuffer)
{
   void * pMemBigEnd;
   int    ret;

   /* the memory transfer at the kernel layer might require 64-bit alignment */
   if ((ret = MoCAOS_MemAlign(&pMemBigEnd, 64, sizeof(*a_pRttControl))) < 0)
   {
      mocad_log(ctx, L_ERR, "Unable to allocate memory from kernel (err %d)\n", ret);
      return -1;
   }

   __moca_copy_be32(pMemBigEnd, a_pRttControl, sizeof(*a_pRttControl));

   MoCAOS_WriteMem(ctx->os_handle, (unsigned char *)pMemBigEnd, sizeof(*a_pRttControl), (unsigned char *)((a_rttBuffer) & 0xFFFFFFF8));

   MoCAOS_FreeMemAlign(pMemBigEnd);

   return 0;
}

#if defined(__gnu_linux__)
#define mocad_rtt_log(ctx,fp,fmt,...) \
do { \
   if (ctx->verbose & L_RTT) \
      mocad_log(ctx, L_ERR, fmt, ##__VA_ARGS__);\
   if ((ctx->verbose & L_RTT_FILE) && (fp != 0))\
      fprintf((FILE *)fp, fmt, ##__VA_ARGS__);\
} while (0)
#else
#define mocad_rtt_log(ctx,fp,fmt,...) \
do { \
   if (ctx->verbose & L_RTT) \
      mocad_log(ctx, L_ERR, fmt, ##__VA_ARGS__); \
} while(0)
#endif
MOCAD_S uint32_t mocad_print_rtt(struct mocad_ctx *ctx, uint32_t rtt_buffer, uint32_t cpu_id)
{
   rtt_control_t  rtt_control;
   char           last;
   int32_t        bytes_read;
#if defined(__gnu_linux__)
   uintptr_t      fp __attribute__((unused)) =0;
#endif

#if defined(__gnu_linux__)
   char           rtt_filename[20];

   if (ctx->verbose & L_RTT_FILE)
   {
      int            i;

      for (i=0;i<MAX_RTT_FILES;i++)
      {
         sprintf(rtt_filename, "/tmp/moca_rtt_%d.log",i);
         if (access(rtt_filename, F_OK) == -1)
            break; 
      }
      if (i==MAX_RTT_FILES)
      {
         i = 0;
         sprintf(rtt_filename, "/tmp/moca_rtt_%d.log",i);
      }

      fp = (uintptr_t)fopen(rtt_filename, "w");

      if (fp == 0)
         mocad_log(ctx, L_ERR, "Unable to open RTT file for write: %s (errno=%d)\n", rtt_filename, errno);
      else
      {
         mocad_log(ctx, L_ERR, "RTT dump file: %s\n", rtt_filename);
         sprintf(rtt_filename, "/tmp/moca_rtt_%d.log",(i+1)%MAX_RTT_FILES);
         remove(rtt_filename);
      }
   }
#endif   
   rtt_buffer = (BE32(rtt_buffer) & 0xFFFFF);
   
   mocad_read_rtt_control_timeout(ctx, &rtt_control, rtt_buffer, 3);

   if (rtt_control.rtt_buffer_size == 0)
   {
      mocad_log(ctx, L_ERR, "No RTT was collected for CPU %d\n", cpu_id);
   }
   else
   {

      // first time that data is valid
      if (ctx->verbose & L_RTT) MoCAOS_MSleep(1000);
      mocad_rtt_log(ctx, fp, "FOUND RTT DATA for CPU %d\n", cpu_id); 
      mocad_rtt_log(ctx, fp, "%08x %08x\n", rtt_control.rtt_buffer_size, rtt_control.rtt_last_write_index);

      do
      {
         unsigned char  rtt_payload[RTT_PAYLOAD_LENGTH];
         uint32_t       num_words;
         uint32_t       i;
         char           str[200];
         char         * pStr;
         
         pStr = str;

         mocad_read_rtt_control_timeout(ctx, &rtt_control, rtt_buffer, 3);
         last       = rtt_control.last;
         bytes_read = mocad_read_rtt_payload(ctx, rtt_payload, rtt_buffer);

         if (bytes_read == -1)
         {
            break;
         }

         num_words  = bytes_read / sizeof(uint32_t);

         for ( i = 0 ; i < num_words ; i++ )
         {
            pStr += sprintf(pStr, "%08x ", (unsigned int)(*((uint32_t*)((void*)rtt_payload) + i)));
         }

         sprintf(pStr, "\n");

       //  LOG_I(ALWAYS_PRINT_TRACE, pStr);

         mocad_rtt_log(ctx, fp, "%s", str);

         /* There can be a lot of RTT data. Give the printf thread a chance to process it all */
#define SLEEP_MS 15
#define ITER_PRINT 4 
         {
            static int cnt=0;
            if ( (cnt% ITER_PRINT) == 0)
            {
               if (ctx->verbose & L_RTT) MoCAOS_MSleep(SLEEP_MS*ITER_PRINT);
            }
            cnt++;
         }

         rtt_control.ownership = RTT_CONTROL_OWNERSHIP_FW;
         mocad_write_rtt_control(ctx, &rtt_control, rtt_buffer);
      }
      while (last != 1);

      mocad_rtt_log(ctx, fp, "RTT Dump End\n");
   }

#if defined(__gnu_linux__)
   if (fp)
      fclose((FILE *)fp);
#endif

   return 0;
}

MOCAD_S static void mocad_print_assert(struct mocad_ctx *ctx, uint32_t ie_len, struct moca_assert * p_assert)
{
   uint32_t num_params = 0;
   char str[MAX_STRING_LENGTH];
   uint32_t error_code;
   uint32_t i;

#if defined(__gnu_linux__)
   char printkstr[64];
   int printkstrvalid = 0;
   FILE *fp;

   // stop prints from corrupting output
   fp = fopen("/proc/sys/kernel/printk","r");
   if (fp)
   {
      fgets(printkstr, 64, fp);
      printkstrvalid = 1;
      fclose(fp);
      
      fp = fopen("/proc/sys/kernel/printk","w");
      if (fp)
      {
         fprintf(fp,"0");
         fclose(fp);
      }
   }
#endif

   mocad_log(ctx, L_ERR, "-----------| MoCA Core Assertion !!! |----------\n");

   // error_code = BE32(p_assert->error_code);
   mocad_find_offset(ctx, BE32(p_assert->cpu_id), BE32(p_assert->error_code), (unsigned int*)&error_code,0 );

   if (ctx->link_state == LINK_STATE_DOWN)
      checkPacketRam(ctx);

   ctx->assert_count++;
   ctx->last_assert_num = ctx->moca_running ? error_code : -(int)error_code;

   mocad_log(ctx, L_ERR, "CPU ID                       : %d\n", BE32(p_assert->cpu_id));
   mocad_log(ctx, L_ERR, "Error Code                   : %d\n", ctx->moca_running ? error_code : -(int)error_code);
   mocad_log(ctx, L_ERR, "MacClock                     : 0x%08X\n", BE32(p_assert->macClock));
   mocad_log(ctx, L_ERR, "Len                          : %d \n", ie_len);  

   num_params = BE32(p_assert->num_params);

   ie_len -= offsetof(struct moca_assert, args);

   if (num_params > (ie_len >> 2))
   {
      mocad_log(ctx, L_ERR, "Invalid length (%d) for number of parameters (%d)\n", ie_len, num_params);
      num_params = ie_len >> 2;
   }

   for (i = 0; i < num_params; i++)
   {
      mocad_log(ctx, L_ERR, "Param %3d                    : 0x%08X\n", 
         (i + 1), BE32(p_assert->args[i]));
   }

   if ( ((BE32(p_assert->string_id)) != 0xffffffff) && (ctx->any_time.assertText != 0) )
   {
      mocad_string_formatting(ctx, 0, str, BE32(p_assert->string_id), BE32(p_assert->num_params),(unsigned int*)(&p_assert->args[0]));
      mocad_log(ctx, L_ERR, "Assert string                : ");
   }
   if (p_assert->return_address != 0)
      mocad_log(ctx, L_ERR, "Return Address               : 0x%x \n", BE32(p_assert->return_address));
   mocad_log(ctx, L_ERR, "Core SW                      : %d.%d.%d \n",
      ctx->moca_sw_version_major, 
      ctx->moca_sw_version_minor,
      ctx->moca_sw_version_rev);
   mocad_log(ctx, L_ERR, "Firmware                     : %u.%u.%u.%u\n",
         ctx->fw_version.version_moca,
         ctx->fw_version.version_major,
         ctx->fw_version.version_minor,
         ctx->fw_version.version_patch); 
   mocad_log(ctx, L_ERR, "Chip type:                   : 0x%x\n", ctx->kdrv_info.chip_id);
   mocad_log(ctx, L_ERR, "------------------------------------------------------\n");

   if ((BE32(p_assert->flags) & 0x4) && (ctx->verbose & (L_RTT | L_RTT_FILE)))
   {
      mocad_print_rtt(ctx, p_assert->rtt_buffer_ptr_0, 0);
      mocad_print_rtt(ctx, p_assert->rtt_buffer_ptr_1, 1);
      mocad_log(ctx, L_ERR, "Assert Code                   : %d\n", error_code);
   }

#if defined(__gnu_linux__)
   MoCAOS_MSleep(1000);

   // Restore printk settings
   if (printkstrvalid)
   {
      fp = fopen("/proc/sys/kernel/printk","w");
      if (fp)
      {
         fputs(printkstr, fp);
         fclose(fp);
      }
   }
#endif

}

MOCAD_S static void mocad_remove_pqos_flows(struct mocad_ctx *ctx)
{
   int i;
   struct moca_pqos_table * ptable;
   struct moca_pqos_ingr_delete req;
   
   for (i = 0; i < MOCA_MAX_PQOS_ENTRIES; i++)
   {
      ptable = &ctx->pqos_table[i];
      if (!MOCA_MACADDR_IS_NULL(&ptable->flow_id))
      {
         mocad_log(ctx, L_DEBUG, "%s: deleting pqos flow %02x:%02x:%02x:%02x:%02x:%02x\n",
            __FUNCTION__, MOCA_DISPLAY_MAC(ptable->flow_id));
         /* delete this entry */
         if (ctx->moca_running)
         {
            MOCA_MACADDR_COPY(&req.flowid, &ptable->flow_id);
            __moca_set_pqos_ingr_delete(ctx, &req);
         }
         mocad_del_pqos_entry(ctx, ptable);
      }
   }
}

MOCAD_S static void mocad_update_pqos_node(struct mocad_ctx *ctx, 
   uint32_t node_id, uint32_t node_dropped)
{
   int i;
   struct moca_pqos_table * ptable;
   struct moca_pqos_ingr_delete req;

   if (node_dropped)
   {
      for (i = 0; i < MOCA_MAX_PQOS_ENTRIES; i++)
      {
         ptable = &ctx->pqos_table[i];
         if ((!MOCA_MACADDR_IS_NULL(&ptable->flow_id)) &&
             (ptable->egress_node_mask & (1 << node_id)))
         {
            ptable->egress_node_mask &= ~(1 << node_id);

            if (ptable->egress_node_mask == 0)
            {
               mocad_log(ctx, L_DEBUG, 
                  "%s: deleting pqos flow %02x:%02x:%02x:%02x:%02x:%02x\n",
                  __FUNCTION__, MOCA_DISPLAY_MAC(ptable->flow_id));
               /* delete this entry */
               MOCA_MACADDR_COPY(&req.flowid, &ptable->flow_id);
               __moca_set_pqos_ingr_delete(ctx, &req);
               mocad_del_pqos_entry(ctx, ptable);
            }
         }
      }
   }
}

unsigned int mocad_expire_pqos_flows(struct mocad_ctx *ctx, unsigned int now)
{
   int i;
   struct moca_pqos_table * ptable;
   struct moca_pqos_ingr_delete req;
   uint32_t low_lease_time = 0xFFFFFFFF;
   
   for (i = 0; i < MOCA_MAX_PQOS_ENTRIES; i++)
   {
      ptable = &ctx->pqos_table[i];
      if ((!MOCA_MACADDR_IS_NULL(&ptable->flow_id)) &&
          (ptable->lease_time != 0))
      {
         if (((unsigned int )(ptable->lease_time + ptable->create_time) <= now) &&
              ((ctx->pqos_time_wrap_flags & (1 << i)) == 0))
         {
            mocad_log(ctx, L_DEBUG, "%s: expiring pqos flow %02x:%02x:%02x:%02x:%02x:%02x\n",
               __FUNCTION__, MOCA_DISPLAY_MAC(ptable->flow_id));
            /* delete this entry */
            MOCA_MACADDR_COPY(&req.flowid, &ptable->flow_id);
            __moca_set_pqos_ingr_delete(ctx, &req);
            mocad_del_pqos_entry(ctx, ptable);
         }

         if ((ptable->lease_time != 0) &&
             (((ptable->lease_time + ptable->create_time) - now)
                < low_lease_time)) {
            low_lease_time = 
               (ptable->lease_time + ptable->create_time) - (uint32_t)now;
         }
      }
   }

   if (low_lease_time == 0xFFFFFFFF)
      return 0;
   else
      return(low_lease_time);
}

MOCAD_S unsigned int mocad_get_default_freq(struct mocad_ctx *ctx)
{
   if (ctx->any_time.rf_band == MOCA_RF_BAND_D_LOW)
      return(MOCA_LOF_BAND_D_LOW_DEF);
   else if (ctx->any_time.rf_band == MOCA_RF_BAND_D_HIGH)
      return(MOCA_LOF_BAND_D_HIGH_DEF);
   else if (ctx->any_time.rf_band == MOCA_RF_BAND_EX_D)
      return(MOCA_LOF_BAND_EX_D_DEF);
   else if (ctx->any_time.rf_band == MOCA_RF_BAND_E)
      return(MOCA_LOF_BAND_E_DEF);
   else if (ctx->any_time.rf_band == MOCA_RF_BAND_F) 
      return(MOCA_LOF_BAND_F_DEF);
   else if (ctx->any_time.rf_band == MOCA_RF_BAND_C4) 
      return(MOCA_LOF_BAND_C4_DEF);
   else if (ctx->any_time.rf_band == MOCA_RF_BAND_H) 
      return(MOCA_LOF_BAND_H_DEF);
   else
      return(MOCA_LOF_BAND_EX_D_DEF);
}

MOCAD_S int mocad_freq_in_band(struct mocad_ctx *ctx)
{
   uint32_t oper_freq;

   if (ctx->cof == 0)
      oper_freq = ctx->any_time.lof;
   else
      oper_freq = ctx->cof;

   if (oper_freq == 0)
      return 1;
   
   if (ctx->any_time.rf_band == MOCA_RF_BAND_GENERIC)
   {
      if ( (oper_freq >= 150 ) && (oper_freq <= 2125) )
      {
         return 1;
      }
      else
      {
         return 0;
      }
   }

   if (ctx->any_time.rf_band == MOCA_RF_BAND_D_LOW)
   {
      switch ( oper_freq ) {
         /* LAN */
         case 1125 :
         case 1150 :
         case 1175 :
         case 1200 :
         case 1225 :
           return 1 ;
      
         default :
           return 0 ;
      }
   }
   if (ctx->any_time.rf_band == MOCA_RF_BAND_D_HIGH)
   {
      switch ( oper_freq ) {
         /* LAN */
         case 1350 :
         case 1375 :
         case 1400 :
         case 1425 :
         case 1450 :
         case 1475 :
         case 1500 :
         case 1525 :
         case 1550 :
         case 1575 :
         case 1600 :
         case 1625 :
           return 1 ;
      
         default :
           return 0 ;
      }
   }
   if (ctx->any_time.rf_band == MOCA_RF_BAND_EX_D)
   {
      switch ( oper_freq ) {
         /* LAN */
         case 1125 :
         case 1150 :
         case 1175 :
         case 1200 :
         case 1225 :
         case 1250 :
         case 1275 :
         case 1300 :
         case 1325 :
         case 1350 :
         case 1375 :
         case 1400 :
         case 1425 :
         case 1450 :
         case 1475 :
         case 1500 :
         case 1525 :
         case 1550 :
         case 1575 :
         case 1600 :
         case 1625 :
           return 1 ;
      
         default :
           return 0 ;
      }
   }
   else if (ctx->any_time.rf_band == MOCA_RF_BAND_E)
   {
      switch ( oper_freq ) {
         /* MidRF */
         case 500 :
         case 525 :
         case 550 :
         case 575 :
         case 600 :
           return 1 ;
         default :
           return 0 ;
      }
   }
   else if (ctx->any_time.rf_band == MOCA_RF_BAND_F)
   {
      switch ( oper_freq ) {
         case 675:
         case 700:
         case 725:
         case 750:
         case 775:
         case 800:
         case 825:
         case 850:
            return 1 ;
         default : 
            return 0;
      }
   }
   else if (ctx->any_time.rf_band == MOCA_RF_BAND_C4)
   {
      switch ( oper_freq ) {
         case 1000:
            return 1 ;
         default : 
            return 0;
      }
   }
   else if (ctx->any_time.rf_band == MOCA_RF_BAND_H)
   {
      switch ( oper_freq ) {
         case 975 :
         case 1000 :
         case 1025 :
            return 1 ;
         default :
            return 0 ;
      }
   }
   else // unknown band
   {
      return 0;
   }
}

MOCAD_S unsigned get_svn_version(const char* string)
{
   const char* str = string; 
   unsigned int res=0;

   while(*str && ( *str < '0' || *str > '9' ))
      str++;

   while(*str && ( *str >= '0' && *str <= '9' ))
   {
      res=10*res + ( *str-'0' );
      str++;
   }
   return (res);
}

MOCAD_S int mocad_set_rf_band(void * vctx, uint32_t band)
{
   struct mocad_ctx *ctx = (struct mocad_ctx *)vctx;
   int               ret = 0;
   int               old_lof;

   if (band != ctx->any_time.rf_band)
   {
      mocad_log(ctx, L_WARN, "Rf_band changed to %u, switching to default settings for the new band\n", band);
      ctx->any_time.rf_band = band;

      if (mocad_freq_in_band(ctx))
         old_lof = ctx->any_time.lof;
      else
         old_lof = 0;

      mocad_update_config_flags(ctx);
      mocad_set_band_defaults(ctx, ctx->config_flags);

      if (old_lof)
         ctx->any_time.lof = old_lof;

      // Clear out the RF Cal and Probe II data
      mocad_nv_cal_clear(ctx);

      // Send out a trap indicating that the RF band has changed
      ctx->mocad_trap.hdr.type = BE32(MOCA_MSG_TRAP);
      ctx->mocad_trap.hdr.length = BE32(sizeof(ctx->mocad_trap.payload.new_rf_band) + MOCA_EXTRA_MMP_HDR_LEN);
      ctx->mocad_trap.hdr.ie_type = BE32(IE_NEW_RF_BAND);
      ctx->mocad_trap.payload.new_rf_band = BE32(band);
      
      ret = MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
         (const unsigned char *)&ctx->mocad_trap, 
         (sizeof(ctx->mocad_trap.payload.new_rf_band) + sizeof(struct mmp_msg_hdr)));
   }
   return ret;
}

MOCAD_S void mocad_set_random(uint32_t *buf, unsigned int intsz)
{
   unsigned int i;
   for (i = 0; i < intsz; i++) {
      buf[i] = MoCAOS_GetRandomValue();
   }
}

MOCAD_S static int mocad_handle_core_ready(struct mocad_ctx *ctx, void *data)
{
   struct moca_core_ready *a = (struct moca_core_ready *)data;
    
   struct moca_max_constellation mc;
   struct moca_node_status ns;
   struct moca_fw_version fw_version;
   struct moca_mocad_version mocad_version;
   uint32_t mmp_version = 0;
   uint32_t res;
   int i;

   ctx->reset_count++;
      
   if (BE32(*(uint32_t *)data) == 0)
   {
      mocad_log(ctx, L_WARN,
                     "Core is NOT ready\n");
      return(0);
   }

#if defined(DSL_MOCA) && !defined(CONFIG_BCM_6802_MoCA)
   if (a->chip_type != 0x02)  // bits 0-3 are the chip id firmware is expecting
#else
   if ( ( (a->chip_type != 0x04) && 
         ((ctx->kdrv_info.hw_rev == MOCA_CHIP_11) || (ctx->kdrv_info.hw_rev == MOCA_CHIP_11_LITE))) ||
       ( (a->chip_type != 0x08) && (ctx->kdrv_info.hw_rev == MOCA_CHIP_11_PLUS) ) ||
       ( (a->chip_type != 0x20) && ((ctx->kdrv_info.hw_rev&0xFF00) == (MOCA_CHIP_20&0xFF00)) ))
#endif
   {
      mocad_log(ctx, L_WARN,
         "WARNING: MoCA firmware not appropriate for chip.  Firmware compiled for %s (0x%08x).\n", 
         a->chip_type==0x02?"6816":
         a->chip_type==0x04?"7xxx-gen1":
         a->chip_type==0x08?"7xxx-gen2":
         a->chip_type==0x20?"7xxx-moca20":
         "UNKNOWN chip", a->chip_type);
   }

   if (__moca_get_mmp_version(ctx, &mmp_version) != 0)
      mocad_log(ctx, L_WARN, "Unable to confirm MoCA firmware MMP version.\n");
   else if (mmp_version != MOCAD_MMP_VERSION)
   {
      mocad_log(ctx, L_ERR, "MMP version mismatch! %u vs. %u\n", mmp_version, MOCAD_MMP_VERSION);
      if (ctx->exit_on_mmp_mismatch)
         exit(4);
   }

   mocad_log(ctx, L_DEBUG,
      "Core is ready, sending INIT_TIME args\n");
 
   mocad_log(ctx, L_DEBUG, 
      "Identified chip ID: 0x%08X", ctx->kdrv_info.chip_id );
 
   moca_set_rf_band(ctx, ctx->any_time.rf_band);

   if (ctx->any_time.lof == 1)
   {
      ctx->any_time.lof = ctx->disk_lof;
   }
   else if (ctx->cof == 0)
   {
      mocad_write_lof(ctx, ctx->any_time.lof);
   }

   if ((ctx->any_time.continuous_power_tx_mode != 0) && (ctx->any_time.lof == 0))
   {
      ctx->any_time.lof = mocad_get_default_freq(ctx);
   }

   if (
         (ctx->any_time.continuous_power_tx_mode != 0) && 
         ((ctx->any_time.primary_ch_offset != 0) || (ctx->any_time.secondary_ch_offset != 0)) && 
         (ctx->any_time.bandwidth == 0)
      )
   {
      mocad_log(ctx, L_WARN, "WARNING: Primary/Secondary channel offset are set, but BW is set to 50MHz. Tx might be on wrong channel. Please check --bw configuration.\n");
   }

   if ((ctx->any_time.continuous_power_tx_mode == 0) && !mocad_freq_in_band(ctx))
   {
      mocad_log(ctx, L_WARN, "WARNING: LOF is out of RF band.  Resetting to NULL.\n");
      ctx->any_time.lof = 0;
   }

   if ( ctx->any_time.lof == 0 ) 
   {
      ctx->any_time.lof = mocad_get_default_freq(ctx);

      if (ctx->any_time.rf_band == MOCA_RF_BAND_F) {
         ctx->any_time.tpc_en = 1;
      }
   }

   /* Force single channel operation for WAN channel */
   if ((ctx->any_time.rf_band == MOCA_RF_BAND_C4) && (ctx->cof == 0))
   {
      ctx->any_time.single_channel_operation = MOCA_SINGLE_CH_OP_SINGLE;
   }

   // Determine flags
   mocad_update_config_flags(ctx);

   /* set any_time parameters */
   mocad_log(ctx, L_DEBUG,
      "Sending ANY_TIME args\n");

   mocad_set_all_anytime(ctx);

   // send EGR_MC_FILTER table
   for (i=0; i< MOCA_MAX_EGR_MC_FILTERS;i++)
   {
      struct moca_egr_mc_addr_filter_set filter;
      if (ctx->egr_mc_addr_filter[i].valid)
      {
         MOCA_MACADDR_COPY(&filter.addr, &ctx->egr_mc_addr_filter[i].addr);
         filter.valid = ctx->egr_mc_addr_filter[i].valid;
         filter.entryid = i;
         moca_set_egr_mc_addr_filter(ctx, &filter);
      }
   }

   for (i = 0; i < MOCA_MAX_NODES; i++ ) {
      mc.node_id = i;
      mc.p2p_limit_50  = ctx->max_constellation[i].p2p_limit_50;
      mc.gcd_limit_50  = ctx->max_constellation[i].gcd_limit_50;
      mc.p2p_limit_100 = ctx->max_constellation[i].p2p_limit_100;
      mc.gcd_limit_100 = ctx->max_constellation[i].gcd_limit_100;
      res = moca_set_max_constellation(ctx, &mc);
      if(res != 0)
         mocad_log(ctx, L_WARN,
            "warning: %s[%i] reply %x\n", 
            moca_ie_name(IE_MAX_CONSTELLATION), i, res);
   }

   __moca_set_non_def_seq_num(ctx, ctx->disk_nondefseqnum);

   if (ctx->any_time.nv_cal_enable)
   {
      __moca_set_rf_calib_data(ctx, &ctx->rf_calib_data);

      if (!ctx->any_time.bonding)
         __moca_set_probe_2_results(ctx, &ctx->probe_2_results);
   }

   mocad_set_eport_link_state(ctx);
   
   if (MoCAOS_GetDriverInfo(ctx->os_handle, &ctx->kdrv_info) != 0)
   {
      die("can't read MoCA driver info");
      MoCAOS_MSleep(4000);
      RESTART(2);
      return(0);
   }

   if ((ctx->any_time.continuous_power_tx_mode == MMP_CONTINUOUS_TX_PROBE_I_MODE) &&
       (
        (ctx->any_time.const_tx_params.const_tx_submode == MOCA_CONST_TX_SUBMODE_BAND) ||
        (ctx->any_time.const_tx_params.const_tx_submode == MOCA_CONST_TX_SUBMODE_PROBE_I)
       ) &&
       (
        (ctx->any_time.rf_band == MOCA_RF_BAND_EX_D) ||
        (ctx->any_time.rf_band == MOCA_RF_BAND_D_LOW) ||
        (ctx->any_time.rf_band == MOCA_RF_BAND_D_HIGH)
       )
      )
      MoCAOS_SetSSC(ctx->os_handle, 1);
   else
      MoCAOS_SetSSC(ctx->os_handle, 0);

   /* for watchdog purposes, always enable core traces at start */
   mocad_set_moca_core_trace_enable(ctx, 1);
#if defined(DSL_MOCA) && defined(CONFIG_BCM_6802_MoCA)
   res = __moca_set_start_moca_core(ctx, 300);
#else

   if (ctx->phy_clk_rate)
   {
      mocad_log(ctx, L_VERBOSE, "Using %d phy clock\n", ctx->phy_clk_rate/1000000);
      res = __moca_set_start_moca_core(ctx, ctx->phy_clk_rate/1000000);
   }
   else
   {
      mocad_log(ctx, L_VERBOSE, "Using %d phy clock\n", 300);
      res = __moca_set_start_moca_core(ctx, 300);
   }
#endif

   mocad_clear_base_stats(ctx);

   /* Clear the wait flag if we've been given an order to start */
   if (ctx->any_time.dont_start_moca == MOCA_DONT_START_MOCA_WAIT)
      ctx->any_time.dont_start_moca = MOCA_DONT_START_MOCA_OFF;

   if(ctx->link_state == LINK_STATE_DEBUG) {
      mocad_update_times(ctx, MOCA_TIME_EVENT_CORE_UP);
      ctx->moca_running = 1;
      /* Check for special Const Tx Mode options */
      return(0);
   }

   if(res != 0)
      mocad_log(ctx, L_WARN,
         "warning: START_MOCA_CORE reply %x\n",
         res);
   else {
      mocad_update_times(ctx, MOCA_TIME_EVENT_CORE_UP);
      ctx->moca_running = 1;

      // read back pre-config data from firmware
      if (MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)&ctx->preconfig, 
                         sizeof(ctx->preconfig), 
                         (unsigned char *)(ctx->kdrv_info.gp1 & 0x1fffffff)) != 0)
      {
         mocad_log(ctx, L_ERR, "====== WARNING: Unable to read preconfig ======\n"); 
      }
   }

   mocad_mps_init(ctx);

   mocad_log(ctx, L_DEBUG, "GP1 is %08x\n",
      ctx->kdrv_info.gp1);
   mocad_log(ctx, L_INFO, 
      "THIS Node MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
      ctx->any_time.mac_addr.val.addr[0],
      ctx->any_time.mac_addr.val.addr[1],
      ctx->any_time.mac_addr.val.addr[2],
      ctx->any_time.mac_addr.val.addr[3],
      ctx->any_time.mac_addr.val.addr[4],
      ctx->any_time.mac_addr.val.addr[5]);
   
   mocad_log(ctx, L_INFO, "Last Operational Frequency = %u Mhz\n",
      ctx->any_time.lof);

   if (ctx->cof)
      mocad_log(ctx, L_INFO, "Current Operating Frequency = %u Mhz\n",
         ctx->cof);

   moca_get_node_status(ctx, &ns);
   moca_get_fw_version(ctx, &fw_version);
   ctx->fw_version = fw_version;
   moca_get_version(
      &mocad_version.mocad_version_moca,
      &mocad_version.mocad_version_major, 
      &mocad_version.mocad_version_minor, 
       &mocad_version.mocad_version_patch);
   mocad_log(ctx, L_INFO, "MoCA Startup Successful.\n");
   mocad_log(ctx, L_INFO, "MoCA Version\n");
   mocad_log(ctx, L_INFO, "-----------------------\n");
   if (!fw_version.version_patch)
      mocad_log(ctx, L_INFO, "firmware version  : %u.%u.%u\n",
         fw_version.version_moca,
         fw_version.version_major,
         fw_version.version_minor);
   else
      mocad_log(ctx, L_INFO, "firmware version  : %u.%u.%u.%u\n",
         fw_version.version_moca,
         fw_version.version_major,
         fw_version.version_minor,
         fw_version.version_patch); 
   ctx->moca_sw_version_major = ns.moca_sw_version_major;
   ctx->moca_sw_version_minor = ns.moca_sw_version_minor;
   ctx->moca_sw_version_rev = ns.moca_sw_version_rev;

   if (!mocad_version.mocad_version_patch)
      mocad_log(ctx, L_INFO, "mocad version     : %u.%u.%u\n",
         mocad_version.mocad_version_moca,
         mocad_version.mocad_version_major,
         mocad_version.mocad_version_minor);
   else
      mocad_log(ctx, L_INFO, "mocad version     : %u.%u.%u.%u\n",
         mocad_version.mocad_version_moca,
         mocad_version.mocad_version_major,
         mocad_version.mocad_version_minor,
         mocad_version.mocad_version_patch);

   mocad_log(ctx, L_INFO, "HW version        : 0x%x\n", 
      /* ns.moca_hw_version */
      ctx->kdrv_info.chip_id);
   mocad_log(ctx, L_INFO, "bmoca version     : %u.%u.%x\n",
      (ctx->kdrv_info.version >> 16),
      (ctx->kdrv_info.version & 0xffff),
      ctx->kdrv_info.build_number);   
   mocad_log(ctx, L_INFO, "MoCA self version : 0x%x\n",
      ns.self_moca_version);

   mocad_log(ctx, L_INFO, "-----------------------\n");

   return(0);
}

MOCAD_S int mocad_handle_hostless_mode_wakeup(struct mocad_ctx *ctx)
{
   if (ctx->link_state != LINK_STATE_DEBUG)
      mocad_check_link_state(ctx);

   return 0;
}

MOCAD_S int mocad_handle_new_rf_calib_data(struct mocad_ctx * ctx, struct moca_new_rf_calib_data * data)
{
   memcpy(&ctx->rf_calib_data.data[0], &data->data[0], sizeof(*data));
   ctx->rf_calib_data.valid = 1;

   if (ctx->any_time.nv_cal_enable)
   {
      mocad_write_rf_calib_data(ctx);
   }   
   return 0;
}

MOCAD_S int mocad_handle_new_probe_2_results(struct mocad_ctx * ctx, struct moca_new_probe_2_results * data)
{
   __moca_copy_be32(&ctx->probe_2_results, data, sizeof(*data));

   if (ctx->any_time.nv_cal_enable && !ctx->any_time.bonding)
   {
      mocad_write_probe_2_results(ctx);
   }   
   return 0;
}


// On suspend set core to desired power state (via M0 if necessary) and enable WoM and hostless mode
// On resume set core to M0 Active and disable WoM and hostless mode
MOCAD_S int mocad_handle_pm_notification(struct mocad_ctx * ctx, uint32_t state)
{
   int ret;
   uint32_t standby = (state == 1) ? 1 : 0; // MOCA_SUSPENDING=1 see bmoca.h
   uint32_t cur_state = 0xffffffff;
   uint32_t desired_standby_state = ctx->any_time.standby_power_state;
   uint32_t new_wom_mode = (standby) ? ctx->any_time.wom_mode : 0;

   // Pass the WoM mode to FW first
   if ((ret = __moca_set_wom_mode_internal(ctx, new_wom_mode)) != MOCA_API_SUCCESS) {
      mocad_log(ctx, L_WARN, "mocad_handle_pm_notification: Failed to set WoM mode (%d)\n", ret);
   }

   // We choose to ignore updating and verifying ps_capabilities in order to save time
   if ((ret = moca_get_power_state(ctx, &cur_state)) != MOCA_API_SUCCESS) {
      mocad_log(ctx, L_WARN, "mocad_handle_pm_notification: Failed to retrieve power state (%d)\n", ret);
   }

   if ((standby && cur_state != 0 && desired_standby_state != 0 && cur_state != desired_standby_state)
       || (!standby)) {
      // Note that transition to new state must go via M0 Active
      if ((ret = __moca_set_power_state(ctx, 0)) != MOCA_API_SUCCESS)
         mocad_log(ctx, L_WARN, "mocad_handle_pm_notification: Failed to start Active (M0) power state (%d)\n", ret);
   }

   if (standby
       && (ret = __moca_set_power_state(ctx, desired_standby_state)) != MOCA_API_SUCCESS)
      mocad_log(ctx, L_WARN, "mocad_handle_pm_notification: Failed to start %d power state (%d)\n", desired_standby_state, ret);

   if ((ret = mocad_do_hostless_mode(ctx, standby)) != 0)
      mocad_log(ctx, L_WARN, "mocad_handle_pm_notification: do hostless mode failed %d\n", ret);

   return 0;
}


MOCAD_S void  mocad_clear_bonded_seed_recovery_counts(struct mocad_ctx *ctx)
{
   int i;

   for (i = 0; i < MOCAD_NUM_BONDED_SEED_RECOVERY_ERRORS; i++)
   {
      ctx->bonded_seed_recovery_table[i].error_count = 0;
   }
}

MOCAD_S void  mocad_bonded_seed_check(struct mocad_ctx *ctx, int32_t error)
{
   int      i;
   uint32_t reset_counts = 0;
   uint32_t now;

   now = MoCAOS_GetTimeSec();

   mocad_log(ctx, L_DEBUG, "Seed recovery (%d): error %d, seed %d\n", now, error, ctx->any_time.prop_bonding_compatibility_mode);
   
   if (ctx->any_time.prop_bonding_compatibility_mode == 1)
   {
      for (i = 0; i < MOCAD_NUM_BONDED_SEED_RECOVERY_ERRORS; i++)
      {
         if (error == ctx->bonded_seed_recovery_table[i].error_code)
         {
            if ((now - ctx->last_bonded_seed_error_time) > MOCAD_BONDED_SEED_RECOVERY_INTERVAL)
            {
               mocad_log(ctx, L_DEBUG, "Resetting recovery counts (now %d, last %d)\n",
                  now, ctx->last_bonded_seed_error_time);
               mocad_clear_bonded_seed_recovery_counts(ctx);
            }

            ctx->bonded_seed_recovery_table[i].error_count++;

            mocad_log(ctx, L_DEBUG, "Recovery match error %d. Curr %d Limit %d (now %d, last %d)\n",
               error, ctx->bonded_seed_recovery_table[i].error_count, 
               ctx->bonded_seed_recovery_table[i].error_limit,
               now, ctx->last_bonded_seed_error_time);

            ctx->last_bonded_seed_error_time = now;
            
            if (ctx->bonded_seed_recovery_table[i].error_count >= 
                ctx->bonded_seed_recovery_table[i].error_limit)
            {
               // Reset error_count fields in bonded_seed_recovery_table[]
               reset_counts = 1;
   
               // Print warning
               mocad_log(ctx, L_INFO, "Switching MoCA bonded seed from %d to %d\n", 
                  ctx->preconfig_seed_bonding, !ctx->preconfig_seed_bonding );

               // Toggle preconfig seed for bonding
               mocad_write_preconfig_seed_bonding(ctx, ctx->preconfig_seed_bonding ? 0 : 1);
   
               // Restart the firmware, not using RESTART() so that we don't reboot the 
               // board in standalone mode
               ctx->restart = 23;
   
               break;
            }
         }
      }

      // Reset the counts if either we're about to restart the firmware
      if (reset_counts)
      {
         mocad_log(ctx, L_DEBUG, "Resetting recovery counts\n");
         mocad_clear_bonded_seed_recovery_counts(ctx);
      }
   }
}

MOCAD_S int mocad_handle_trap(struct mocad_ctx *ctx, int trap_len)
{
   struct mmp_msg_hdr *mh;
   void *data;
   uint32_t ie_type, ie_len, msg_type;
   int i;
   int delete_ie = 0;
   char str[MAX_STRING_LENGTH];
#if defined(DSL_MOCA) && !defined(CONFIG_BCM_6802_MoCA)
   unsigned char c;
#endif
   mh = (struct mmp_msg_hdr *)ctx->trap_buf;

   if (BE32(mh->type) == MOCA_MSG_CONCAT_TRAP)
   {
      mocad_log(ctx, L_TRAP, "Got concat trap, len %u bytes\n", BE32(mh->length));
      trap_len -= (sizeof(mh->type) + sizeof(mh->length));
      mh = (struct mmp_msg_hdr *)&mh->ie_type;
   }

   while (trap_len > 0)
   {
      const char *ie_name;

      msg_type = BE32(mh->type);
      data = (void *)(mh + 1);

      delete_ie = 0;
      if (msg_type == 0)
      {
         break;
      }

      ie_type = BE32(mh->ie_type);
      ie_len = BE32(mh->length) - MOCA_EXTRA_MMP_HDR_LEN;

      if (ie_len > (unsigned) trap_len)
      {
         mocad_log(ctx, L_TRAP, "Got Invalid trap %04x length %d\n", ie_type, ie_len);
         return(-1);
      }

      if(ctx->verbose & L_TRAP)
      {
         ie_name = moca_ie_name(ie_type);
         if (ie_name)
            mocad_log(ctx, L_TRAP, "Got trap %04x (%s)\n", ie_type, ie_name);
         else 
            mocad_log(ctx, L_TRAP, "Got Unknown trap %04x\n", ie_type);

         mocad_dump_buf(ctx, "TRAP:", (unsigned char *)data, MIN(ie_len, (unsigned) trap_len));
      }

      if (!ctx->moca_running)
      {
         switch (ie_type) {
            case IE_CORE_READY:
            case IE_MIPS_EXCEPTION:
            case IE_WDT:
            case IE_ASSERT:
               break;

            default:
               // ignore traps if firmware isn't running (i.e. handle race condition between moca stop and traps)
               mocad_log(ctx, L_TRAP, "Ignore trap %04x when FW not running\n", ie_type);
               trap_len -= ie_len + sizeof(struct mmp_msg_hdr);
               mh = (struct mmp_msg_hdr *)((int)mh + ie_len + sizeof(struct mmp_msg_hdr));
               continue;
         }
      }

      if (ctx->hostless_mode)
      {
          if ((ie_type != IE_MIPS_EXCEPTION) &&
              (ie_type != IE_WDT) &&
              (ie_type != IE_ASSERT))
          {
             if (mocad_handle_hostless_mode_wakeup(ctx) != 0)
               break;
          }
      }

      ctx->lab_printf_wdog_count++;

#if !defined(__EMU_HOST_20__) && defined(STANDALONE)
      if (ie_type == IE_MOCAD_FORWARDING_RX_PACKET
          || ie_type == IE_MOCAD_FORWARDING_TX_ACK) {
         /* Shortcut the processing of these because they are of no concern to mocad and 
          * on Standalone this becomes a bottleneck for datapath e.g. receiving and discarding
          * flood of broadcast traffic that we're not interested in.
          */
      }
      else
#endif
      switch(ie_type) {
         case IE_ADMISSION_STATUS:
         {
            unsigned int now;
            unsigned int then;
            char timestr[50];
            uint32_t *a = (uint32_t *)data;

            if ((BE32(*a) != 0) && (ctx->show_lab_printf))
            {
               now = MoCAOS_GetTimeSec();

               if (ctx->core_uptime > ctx->link_downtime)
                  then = ctx->core_uptime;
               else
                  then = ctx->link_downtime;
               
               sprintf(timestr, "Network search completed after %ds\n", (int)(now - then));
               mocad_lab_printf(ctx, timestr);
            }
            break;
         }
         case IE_TOPOLOGY_CHANGED: {
            uint32_t *a = (uint32_t *)data;

            for (i = 0; i < MOCA_MAX_NODES; i++)
            {
               if ((ctx->active_nodes & (1 << i)) !=
                  (BE32(*a) & (1 << i)))
               {
                  mocad_log(ctx, L_DEBUG,
                     "Topology changed node=%d  add/del=%d\n",
                     i,((BE32(*a) >> i) & 0x1));

                  mocad_update_pqos_node(ctx, i, !((BE32(*a) >> i) & 0x1));

                  memset(&ctx->node_stats_base[i], 0, sizeof(ctx->node_stats_base[i]));
                  memset(&ctx->node_stats_ext_base[i], 0, sizeof(ctx->node_stats_ext_base[i]));
               }
            }
            ctx->active_nodes = BE32(*a);
            ctx->topology_change_count++;
            break;
         }
#if defined(__EMU_HOST_20__)
         case IE_MOCAD_FORWARDING_RX_PACKET:
         {
            char buf[1518];
            struct moca_mocad_forwarding_rx_packet *rx = (struct moca_mocad_forwarding_rx_packet *) data;
            struct moca_mocad_forwarding_rx_ack ack;
//             printf("RX PACKET:  offset=%X, len=%u\n", (unsigned int)BE32(rx->offset), (unsigned int)BE32(rx->length));
            ack.offset = BE32(rx->offset);
            ack.size = BE32(rx->length);

//             if (0 != MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)buf, BE32(rx->length), (unsigned char *)BE32(rx->offset)))
//                mocad_log(ctx, L_WARN, "Unable to read mem\n");

            if (0) // display ?
            {
               int k;
               for(k=0;k<16;k++)
                  printf("%02X ",buf[k]);
               printf("\n");
            }

            moca_set_mocad_forwarding_rx_ack(ctx, &ack);

            if (0) // forward to MoCA ?
            {
               char tempMac[6];
               struct moca_mocad_forwarding_tx_alloc txalloc;
               struct moca_mocad_forwarding_tx_send send;

               moca_get_mocad_forwarding_tx_alloc(ctx, &txalloc);

               printf("OUT: offset=%X size=%X, count=%X\n", (unsigned int)txalloc.offset, (unsigned int)txalloc.size, (unsigned int)txalloc.count);

               send.offset = txalloc.offset;
               send.size = 64;
               send.dest_if = 0;

               // swap DA and SA
               memcpy(tempMac, &buf[0], 6);
               memcpy(&buf[0], &buf[6], 6);
               memcpy(&buf[6], tempMac, 6);

               MoCAOS_WriteMem(ctx->os_handle, (unsigned char *)&buf, 64, (unsigned char *)txalloc.offset);

               moca_set_mocad_forwarding_tx_send(ctx, &send);
            }
            break;
          }
          case IE_MOCAD_FORWARDING_TX_ACK:
          {
            struct moca_mocad_forwarding_tx_ack *rx = (struct moca_mocad_forwarding_tx_ack *) data;
            printf("TX Ack, offset: %x\n", (unsigned int)rx);
            break;
         }
#endif
         case IE_KEY_CHANGED: {
            uint32_t *a = (uint32_t *)data;
            switch(BE32(*a))
            {
               case MOCA_KEY_CHANGED_TEK:
               {
                  ctx->key_times.tek_even_odd = BE32(*(a+1));
                  if (ctx->key_times.tek_time != 0)
                     ctx->key_times.tek_last_interval = MoCAOS_GetTimeSec() - ctx->key_times.tek_time;
                  ctx->key_times.tek_time = MoCAOS_GetTimeSec();
                  break;
               }
               case MOCA_KEY_CHANGED_PMK:
               {
                  ctx->key_times.pmk_even_odd = BE32(*(a+1));
                  if (ctx->key_times.pmk_time != 0)
                     ctx->key_times.pmk_last_interval = MoCAOS_GetTimeSec() - ctx->key_times.pmk_time;
                  ctx->key_times.pmk_time = MoCAOS_GetTimeSec();
                  break;
               }
               case MOCA_KEY_CHANGED_APMK:
               {
                  ctx->key_times.aes_pmk_even_odd = (BE32(*(a+1)) & MOCA_KEY_CHANGED_APMK_EVEN_ODD_BIT) ? 1 : 0;
                  if (ctx->key_times.aes_pmk_time != 0)
                     ctx->key_times.aes_pmk_last_interval = MoCAOS_GetTimeSec() - ctx->key_times.aes_pmk_time;
                  ctx->key_times.aes_pmk_time = MoCAOS_GetTimeSec();
                  break;
               }
               case MOCA_KEY_CHANGED_ATEK:
               {
                  ctx->key_times.aes_tek_even_odd = (BE32(*(a+1)) & MOCA_KEY_CHANGED_ATEK_EVEN_ODD_BIT) ? 1 : 0;
                  if (ctx->key_times.aes_tek_time != 0)
                     ctx->key_times.aes_tek_last_interval = MoCAOS_GetTimeSec() - ctx->key_times.aes_tek_time;
                  ctx->key_times.aes_tek_time = MoCAOS_GetTimeSec();
                  break;
               }
            }
            break;
         }
         case IE_CORE_READY: {
            mocad_handle_core_ready(ctx, data);
            break;
         }
         case IE_LAB_PRINTF_CODES:
         case IE_LAB_PRINTF_CODES_LOOKUP:
         {
            struct moca_lab_printf_codes * pPrint = (struct moca_lab_printf_codes *) data;
            struct moca_lab_printf_codes_lookup * pPrintLookup = (struct moca_lab_printf_codes_lookup *) data;
            unsigned int string_id;
            unsigned int num_params;
            unsigned int arg_offset;

            if (ie_type == IE_LAB_PRINTF_CODES) // legacy
            {
               string_id = BE32(pPrint->string_id);
               num_params = BE32(pPrint->num_params);
               arg_offset = 3;
            }
            else // IE_LAB_PRINTF_CODES_LOOKUP
            {
               string_id = BE32(pPrintLookup->string_id);
               num_params = BE32(pPrintLookup->num_params);
               arg_offset = 3;
            }


            if ( ctx->show_lab_printf ){
               mocad_string_formatting(ctx, ie_type == IE_LAB_PRINTF_CODES_LOOKUP, str, string_id, num_params, 
                  &(((unsigned int*)data)[arg_offset]));

               mocad_lab_printf(ctx, str);
            }
            else
            {
               mocad_set_moca_core_trace_enable(ctx,0);
            }
            
               delete_ie = 1;

            break;
         }

         case IE_LAB_PRINTF:
         case IE_DRV_PRINTF:
            mocad_lab_printf(ctx, (char *)data);

               delete_ie = 1;

            break;

         case IE_ASSERT: {
            struct moca_assert *a = (struct moca_assert *)data;
#if defined(DSL_MOCA) && !defined(CONFIG_BCM_6802_MoCA)
            bcm_reg_read(0x0000005d, (char *)&c, 1);

            mocad_log(ctx, L_ERR, "GENET if state reg=%X\n", c);
            system("/bin/ethswctl -c pagedump -P 0");
            system("dw 0xb0da1420");
            system("fh 0xb0e00a00 5 1");
            system("dh 0xb0e00a90 16");                
#endif
            
            MoCAOS_MSleep(1000);

            mocad_print_assert(ctx, ie_len, a);                  
            mocad_update_times(ctx, MOCA_TIME_EVENT_CORE_DOWN);
            ctx->moca_running = 0;

            if (ctx->any_time.assert_restart)
            {
               RESTART(4);
            }
            else
               mocad_log(ctx, L_WARN, "Not restarting MoCA after assert since assert_restart parameter is set\n");

            return(0);
         }
         case IE_WARNING: 
         case IE_WARNING_LOOKUP: {
            struct moca_warning * pWarn = (struct moca_warning *) data;
            struct moca_warning_lookup * pWarnLookup = (struct moca_warning_lookup *) data;
            unsigned int string_id;
            unsigned int num_params;
            unsigned int arg_offset;

            if (ie_type == IE_WARNING) // legacy
            {
               string_id = BE32(pWarn->string_id);
               num_params = BE32(pWarn->num_params);
               msg_type = BE32(pWarn->msg_type);
               arg_offset = 3;
            }
            else // IE_WARNING_LOOKUP 
            {
               string_id = BE32(pWarnLookup->string_id);
               num_params = BE32(pWarnLookup->num_params);
               msg_type = BE32(pWarnLookup->msg_type);
               arg_offset = 3;
            }

            mocad_bonded_seed_check(ctx, msg_type);

            if (ctx->any_time.continuous_power_tx_mode ||
                (
                 !(
                   ((msg_type == 6529) && (ctx->any_time.primary_ch_offset == 1)) ||
                   ((msg_type == 6530) && (ctx->any_time.primary_ch_offset == 1)) ||
                   ((msg_type == 6528) && (ctx->any_time.secondary_ch_offset == 1))
                  )
                ) 
               )
            {
               if ((ctx->verbose & L_CORE_WARN) || ctx->show_lab_printf || (msg_type == 6529) ||
                   (msg_type == 6528) || (msg_type == 6530))
               {
                  mocad_string_formatting(ctx, ie_type == IE_WARNING_LOOKUP, str, string_id, num_params, 
                     &(((unsigned int*)data)[arg_offset]));
   
                  if (BE32(pWarn->string_id) == 0xffffffff)
                     mocad_log(ctx, L_WARN, "MoCA_W: %d\n", BE32(pWarn->msg_type)) ;
                  else
                     mocad_log(ctx, L_WARN, "MoCA_W: %d  W_INFO: %s", BE32(pWarn->msg_type), str) ;
               }
               else
               {
                  mocad_param_string(ctx, str, MAX_STRING_LENGTH, num_params & 0xFFFFFF, 
                     &(((unsigned int*)data)[arg_offset]));
                  mocad_log(ctx, L_WARN, "MoCA_W: %d [%s]\n", BE32(pWarn->msg_type), str);
               }
            }

               delete_ie = 1;

            break;
         }
         case IE_ERROR:
         case IE_ERROR_LOOKUP: {
            struct moca_error * pErr = (struct moca_error *) data;
            struct moca_error_lookup * pErrLookup = (struct moca_error_lookup *) data;
            int32_t error;
            unsigned int string_id;
            unsigned int num_params;
            unsigned int arg_offset;

            if (ie_type == IE_ERROR) // legacy
            {
               string_id = BE32(pErr->string_id);
               num_params = BE32(pErr->num_params);
               arg_offset = 3;
               error = BE32(pErr->err_id);
            }
            else // IE_ERROR_LOOKUP 
            {
               string_id = BE32(pErrLookup->string_id);
               num_params = BE32(pErrLookup->num_params);
               arg_offset = 3;
               error = BE32(pErrLookup->err_id);
            }

            mocad_bonded_seed_check(ctx, error);


            /* Special case for errors to be masked. */
            if ((error == ctx->error_to_mask.error1) ||
               (error == ctx->error_to_mask.error2) ||
               (error == ctx->error_to_mask.error3))
            {
               /* Do nothing. The user has requested that this error
                  not be printed. */
               mocad_log(ctx, L_DEBUG, "Skipping error %u\n", error);
            }
            else if ((error >= MMP_TASK_POOL_EMPTY_ERROR) &&
                   (error <= MMP_CPU0_TRAP_QUEUE_FULL_ERROR)) {   
               mocad_log(ctx, L_ERR, "MoCA_E: %s \n",
                  ((error == MMP_TASK_POOL_EMPTY_ERROR) ? "TASKPOOLEMPTY" :
                  ((error == MMP_ISR_POOL_EMPTY_ERROR) ? "ISRPOOLEMPTY" :
                  ((error == MMP_TRAP_QUEUE_FULL_ERROR) ? "TRAPQFULL - CPU1" :
                  ((error == MMP_CPU0_TRAP_QUEUE_FULL_ERROR) ? "TRAPQFULL - CPU0" :
                  "UNKNOWNERROR"))))) ;


               /* If a TRAPQFULL message was received, there's a chance that
                * we've missed a LINK UP/DOWN trap. Check the status to update
                * the link up/down times and LOF if necessary. */
               if ((ctx->link_state != LINK_STATE_DEBUG) &&
                  (error == MMP_TRAP_QUEUE_FULL_ERROR))
               {
                  mocad_check_link_state(ctx);
               }
            }
            else 
            {
               if ( ctx->show_lab_printf ){
                  mocad_string_formatting(ctx, ie_type == IE_ERROR_LOOKUP, str, string_id, num_params, 
                        &(((unsigned int*)data)[arg_offset]));
                  mocad_log(ctx, L_ERR, "MoCA_E: %d  E_INFO: %s", error, str) ;
               }
               else 
               {
                  mocad_param_string(ctx, str, MAX_STRING_LENGTH, num_params & 0xFFFFFF, 
                     &(((unsigned int*)data)[arg_offset]));
                  mocad_log(ctx, L_ERR, "MoCA_E: %d [%s]\n", error, str) ;
               }
            }

            delete_ie = 1;

            break;
         }
         case IE_MIPS_EXCEPTION: {
            struct moca_mips_exception *e = (struct moca_mips_exception *)data;

            mocad_log(ctx, L_ERR, "Assertion from MoCA Core \n");
            mocad_log(ctx, L_ERR, 
               "Assertion ID (in decimals)  : %d \n", 
               BE32(e->err_code));
            mocad_log(ctx, L_ERR, 
               "\t RESERVED    (in hex)  : 0x%x \n", 
               BE32(e->zero));
            mocad_log(ctx, L_ERR, 
               "\t CP0_EPC     (in hex)  : 0x%x \n", 
               BE32(e->cp0_epc));
            mocad_log(ctx, L_ERR, 
               "\t CP0_CAUSE   (in hex)  : 0x%x \n", 
               BE32(e->cp0_cause));
            mocad_log(ctx, L_ERR, 
               "\t CP0_STATUS  (in hex)  : 0x%x \n", 
               BE32(e->cp0_status));
            mocad_log(ctx, L_ERR, 
               "\t CP0_ERROREPC (in hex)  : 0x%x \n", 
               BE32(e->cp0_errorepc));
            mocad_log(ctx, L_ERR, 
               "Assertion Info - Core SW     : %d.%d.%d \n",
                ctx->moca_sw_version_major, 
                ctx->moca_sw_version_minor,
                ctx->moca_sw_version_rev);
            mocad_log(ctx, L_ERR, 
               "Assertion Info - Drv SW      : %d.%d.%x \n",
               (ctx->kdrv_info.version >> 16),
               (ctx->kdrv_info.version & 0xffff),
               ctx->kdrv_info.build_number) ;
            mocad_update_times(ctx, MOCA_TIME_EVENT_CORE_DOWN);
            ctx->moca_running = 0;
            RESTART(5);

#if defined(DSL_MOCA) && !defined(CONFIG_BCM_6802_MoCA)
            bcm_reg_read(0x0000005d, (char *)&c, 1);
            mocad_log(ctx, L_ERR, "GENET if state reg=%X\n", c);
            system("/bin/ethswctl -c pagedump -P 0");
            system("dw 0xb0da1420");
            system("fh 0xb0e00a00 5 1");
            system("dh 0xb0e00a90 16");
#endif
            return(0);
         }
         case IE_WDT:{
            void *pMem;

            mocad_log(ctx, L_ERR, "MoCA WDT timeout\n");
            if (MoCAOS_MemAlign(&pMem, 64, (129 * sizeof(uint32_t))) < 0)
               return(0);
       
            if ( MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)pMem, (129 * sizeof(uint32_t)), 0) == 0 )
            {
               if ((BE32(*(int *)pMem)) == 0x12345678 )
               {
                  mocad_print_assert(ctx,  BE32(*(int *)((unsigned char *)pMem + 8)), (struct moca_assert *)((unsigned char *)pMem + 16));
               }
            }
            MoCAOS_FreeMemAlign(pMem); 
            mocad_update_times(ctx, MOCA_TIME_EVENT_CORE_DOWN);
            ctx->moca_running = 0;
            RESTART(6);
            return(0);
         }
         case IE_NEW_LOF: {
            uint32_t *lof = (uint32_t *)data;
            mocad_write_lof(ctx, BE32(*lof) * 25);

            /* NOTE: LOF is also broadcast to the clients */
            break;
         }
         case IE_PKTRAM_FILL: {
               // copy data to packet ram           
               mocad_get_packetram(ctx, ctx->cpu_img[1], ctx->cpu_img_len[1]);

               checkPacketRam(ctx);

               // send download done
               __moca_set_download_to_packet_ram_done(ctx);               
               break;
            }
         case IE_LINK_UP_STATE: {
            uint32_t *st = (uint32_t *)data;

            mocad_update_link_state(ctx, BE32(*st) ? 1 : 0);
            /* NOTE: link state is broadcast to the clients */
            break;
         }

         case IE_CPU_CHECK: 
         {
            if (ctx->cpu_check_sec == 0)
            {
               ctx->cpu_check_sec = MoCAOS_GetTimeSec();
            }
            else
            {
               unsigned int timeSec;
    
               timeSec  = ctx->cpu_check_sec;
               ctx->cpu_check_sec = MoCAOS_GetTimeSec();
                  
               if ((abs((ctx->cpu_check_sec - timeSec) - CPU_CHECK_INTERVAL)) > CPU_CHECK_MARGIN_ERR )               
               {                   
                  mocad_log(ctx, L_WARN, "warning: CPU_CHECK time difference of %d sec\n", (ctx->cpu_check_sec - timeSec));
               }
            }
            break;
         }

         case IE_LMO_INFO: {
            struct moca_lmo_info * lmo = (struct moca_lmo_info *)data;
            mocad_check_node_stats_ext(ctx, BE32(lmo->lmo_anb));
#ifndef STANDALONE
            /* retrieve the CIR data from memory */
            if((ctx->preconfig.ciroffset != 0) && ( BE32(lmo->lmo_node_id) < MOCA_MAX_NODES))
            {
               int ret;

               ret = MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)ctx->cir_data[ BE32(lmo->lmo_node_id) ], 
                     CIR_DATA_SIZE,
                     (unsigned char *)((uintptr_t)ctx->preconfig.ciroffset & 0x1fffffff));
               
               if (ret != 0)
               {
                  mocad_log(ctx, L_ERR, "Failure to read CIR data for node %lu (%d)\n",
                     BE32(lmo->lmo_node_id), ret);
               } 
            }
#endif
            break;
         }

         case IE_PQOS_CREATE_RESPONSE: {
            /* Transaction initiated by this node */
            struct moca_pqos_create_response * pqosc_rsp = (struct moca_pqos_create_response *)data;
            mocad_handle_pqos_create_rsp(ctx, pqosc_rsp);
            break;
         }
         case IE_PQOS_CREATE_COMPLETE: {
            /* Transaction initiated by other node */
            struct moca_pqos_create_complete * pqosc_cmp = (struct moca_pqos_create_complete *) data;
            mocad_handle_pqos_create_cmp(ctx, pqosc_cmp);
            break;
         }
         case IE_PQOS_UPDATE_RESPONSE: {
            struct moca_pqos_update_response * pqosu_rsp = (struct moca_pqos_update_response *)data;
            mocad_handle_pqos_update_rsp(ctx, pqosu_rsp);
            break;
         }
         case IE_PQOS_UPDATE_COMPLETE: {
            struct moca_pqos_update_complete * pqosu_cmp = (struct moca_pqos_update_complete *)data;
            mocad_handle_pqos_update_cmp(ctx, pqosu_cmp);
            break;
         }
         case IE_PQOS_DELETE_RESPONSE: {
            /* Transaction initiated by this node */
            struct moca_pqos_delete_response_int * pqosd_rsp = (struct moca_pqos_delete_response_int *)data;
            mocad_handle_pqos_delete_rsp(ctx, pqosd_rsp);
            break;
         }
         case IE_PQOS_DELETE_COMPLETE: {
            /* Transaction initiated by other node */
            struct moca_pqos_delete_complete_int * pqosd_cmp = (struct moca_pqos_delete_complete_int *)data;
            mocad_handle_pqos_delete_cmp(ctx, pqosd_cmp);
            break;
         }
         case IE_PQOS_MAINTENANCE_COMPLETE: {
            struct moca_pqos_maintenance_complete * pqos_maint = (struct moca_pqos_maintenance_complete *)data;

            mocad_log(ctx, L_TRAP, "PQoS Maintenance complete.\n") ;
            mocad_log(ctx, L_TRAP, "      iocOvercommit:   %d\n", 
               pqos_maint->iocovercommit) ;
            mocad_log(ctx, L_TRAP, "      allocatedSTPS:   %d\n", 
               pqos_maint->allocatedstps) ;
            mocad_log(ctx, L_TRAP, "      allocatedTXPS:   %d\n", 
               pqos_maint->allocatedtxps) ;
            break;
         }
         case IE_PQOS_QUERY_RESPONSE: {
            struct moca_pqos_query_response * pqos_query = 
               (struct moca_pqos_query_response *)data;
            mocad_handle_pqos_query_rsp(ctx, pqos_query);
            break;
         }
         case IE_PQOS_LIST_RESPONSE: {
            struct moca_pqos_list_response * pqosl_rsp = (struct moca_pqos_list_response *)data;
            mocad_handle_pqos_list_rsp(ctx, pqosl_rsp);
            break;
         }

         case IE_FMR_RESPONSE: {
            struct moca_fmr_response * fmr_resp =
               (struct moca_fmr_response *)data;
            mocad_handle_fmr_response(ctx, fmr_resp);
            delete_ie = 1;
            break;
         }

         case IE_FMR20_RESPONSE: {
            struct moca_fmr20_response * fmr20_resp =
               (struct moca_fmr20_response *)data;
            mocad_handle_fmr20_response(ctx, fmr20_resp);
            delete_ie = 1;
            break;
         }

         case IE_DD_RESPONSE: {
            struct moca_dd_response * dd_resp =
               (struct moca_dd_response *)data;
            mocad_handle_dd_response(ctx, dd_resp);
            delete_ie = 1;
            break;
         }

         case IE_ACA_RESULT_1:
         case IE_ACA_RESULT_2: {
            mocad_handle_aca_response(ctx, data, ie_type);
            delete_ie = 1;
            break;
         }

         case IE_HOSTLESS_MODE_RESPONSE: {
/*            struct moca_hostless_mode_response_int * hostless_mode_resp =
               (struct moca_hostless_mode_response_int *)data; */
            mocad_handle_hostless_mode_response(ctx);
            delete_ie = 1;
            break;
         }

#if (MPS_EN == 1)
         case IE_INIT_SCAN_REC: {
            mocad_handle_mps_init_scan_rec(ctx);
            break;
         }

         case IE_MPS_PRIVACY_CHANGED: {
            mocad_handle_mps_privacy_changed(ctx);
            break;
         }

         case IE_MPS_TRIGGER: {
            mocad_handle_mps_trigger(ctx);
            // XX if not locally triggered: delete_ie = 1;
            break;
         }

         case IE_MPS_ADMISSION_NOCHANGE: {
            mocad_handle_mps_admission_nochange(ctx);
            break;
         }

         case IE_MPS_PAIR_FAIL: {
            mocad_handle_mps_pair_failed(ctx);
            break;
         }

         case IE_MPS_REQUEST_MPSKEY: {
            struct moca_mps_request_mpskey *pubkey = (struct moca_mps_request_mpskey *)data;
            mocad_handle_mps_request_mpskey(ctx, pubkey);
            delete_ie = 1;
            break;
         }
#endif

         case IE_WAKEUP_NODE_RESPONSE: {
            mocad_handle_wakeup_node_response(ctx);
            delete_ie = 1;
            break;
         }

         case IE_POWER_STATE_RSP: {
            uint32_t * power_state_rsp = (uint32_t *)data;
            mocad_handle_power_state_rsp(ctx, *power_state_rsp);
            //delete_ie = 1; I don't think we want this. Apps may want the response
            break;
         }

         case IE_POWER_STATE_EVENT: {
            uint32_t * power_state_event = (uint32_t *)data;
            mocad_log(ctx, L_POWER_STATE, "Power State event %d occurred\n", BE32(*power_state_event));
            ctx->last_ps_event_code = (int32_t) BE32(*power_state_event);
            break;
         }

         case IE_POWER_STATE_CAP: {
            uint32_t * power_state_cap = (uint32_t *)data;
            ctx->ps_capabilities = BE32(*power_state_cap);
            break;
         }

         case IE_MOCA_RESET_REQUEST:
         {
            struct moca_moca_reset_request * mr_req = (struct moca_moca_reset_request *) data;
            ctx->last_mr_events.last_cause = (int32_t) BE32(mr_req->cause);
            ctx->last_mr_events.last_seq_num = BE32(mr_req->mr_seq_num);
            mocad_log(ctx, L_ALWAYS, "MoCA reset request.  Cause: 0x%08X\n", (unsigned int)BE32(mr_req->cause));
            ctx->restart = 7;
            break;
         }

         case IE_MR_EVENT:
         {
            uint32_t * status = (uint32_t *) data;
            ctx->last_mr_events.last_mr_result = (int32_t)BE32(*status);
            break;
         }

         case IE_MR_COMPLETE:
         case IE_MR_RESPONSE:
         {
            mocad_handle_mr_response(ctx, (struct moca_mr_response *) data);
            break;
         }

         case IE_UCFWD_UPDATE:
         {
            mocad_log(ctx, L_DEBUG, "UCFWD table update\n");
            break;
         }

         case IE_NEW_RF_CALIB_DATA:
         {
            mocad_handle_new_rf_calib_data(ctx, (struct moca_new_rf_calib_data *) data);
            delete_ie = 1;
            break;
         }

         case IE_NEW_PROBE_2_RESULTS:
         {
            mocad_handle_new_probe_2_results(ctx, (struct moca_new_probe_2_results *) data);
            delete_ie = 1;
            break;
         }

         case IE_PM_NOTIFICATION:
         {
            uint32_t *state = (uint32_t *)data;
            mocad_handle_pm_notification(ctx, BE32(*state));
            delete_ie = 1;
            break;
         }
      }

      if (!delete_ie)
      {
         /* broadcast event to all evt clients */
         MoCAOS_SendMMP(ctx->os_handle, MoCAOS_CLIENT_BROADCAST, 
            (const unsigned char *)mh, 
            (ie_len + sizeof(struct mmp_msg_hdr)));
      }

      trap_len -= ie_len + sizeof(struct mmp_msg_hdr);
      mh = (struct mmp_msg_hdr *)((int)mh + ie_len + sizeof(struct mmp_msg_hdr));
   }
   return(0);
}

MOCAD_S static void mocad_handle_deferred_traps(struct mocad_ctx *ctx)
{
   int len;

   while (1)
   {
      len = mocad_get_trap(ctx, ctx->trap_buf, sizeof(ctx->trap_buf));
      if (len > 0)
         mocad_handle_trap(ctx, len);
      else
         break;
   }

   ctx->trap_count = 0;
}

MOCAD_S void mocad_enable_data_if(struct mocad_ctx *ctx)
{
   MoCAOS_EnableDataIf(ctx->os_handle, ctx->ifname, ctx->hostless_mode);
}

int mocad_wr_get_response(void *wr, uint32_t msg_type, uint32_t ie_type, uint32_t ie_len)
{
   struct mmp_msg_hdr *mh;

   mh = (struct mmp_msg_hdr *)wr;
   mh->type = BE32(msg_type | MOCA_MMP_MSG_ACK_BIT);
   mh->length = BE32(ie_len + MOCA_EXTRA_MMP_HDR_LEN);
   mh->ie_type = BE32(ie_type);

   return(sizeof(*mh));
}

MOCAD_S int mocad_wr_result(void *wr, uint32_t msg_type, uint32_t ie_type,
   uint32_t retcode)
{
   struct mmp_msg_hdr *mh;
   uint32_t *data;

   mh = (struct mmp_msg_hdr *)wr;
   mh->type = BE32(msg_type | MOCA_MMP_MSG_ACK_BIT);
   mh->length = BE32(sizeof(retcode) + MOCA_EXTRA_MMP_HDR_LEN);
   mh->ie_type = BE32(ie_type);

   data = (uint32_t *)(mh + 1);
   *data = BE32(retcode);

   return(sizeof(*mh) + sizeof(uint32_t));
}

MOCAD_S int mocad_wr_abort(void *wr)
{
   return(mocad_wr_result(wr, MOCA_MSG_TRAP, IE_ABORT, 1));
}

MOCAD_S int mocad_core_wdog(struct mocad_ctx *ctx)
{
   struct moca_node_status ns;
   int ret;

   ret = mocad_cmd(ctx, MOCA_MSG_GET, IE_NODE_STATUS, NULL, 0,
      &ns, sizeof(ns), 0);
   mocad_log(ctx, L_DEBUG, "MoCA WDOG ret %d, version %d.%d.%d count %u\n", 
      ret, BE32(ns.moca_sw_version_major), BE32(ns.moca_sw_version_minor),
      BE32(ns.moca_sw_version_rev), ctx->lab_printf_wdog_count);
#if !defined(__EMU_HOST_20__)
   if ((ctx->lab_printf_wdog_count == 0) && (ctx->any_time.wdog_enable) &&
      (!ctx->hostless_mode))
   {
      RESTART(8);
      ctx->wdog_count++;
   }
#endif

   ctx->lab_printf_wdog_count = 0;

   if (!ctx->show_lab_printf)
   {
      mocad_set_moca_core_trace_enable(ctx,1);
   }
   
   return(ret);
}

MOCAD_S int mocad_lof_check(uint32_t val, uint32_t flags)
{
      do {
         if (flags & MOCA_BAND_D_HIGH_FLAG)
         {
            switch (val)
            {
               case    0:
               case 1400:
               case 1450:
               case 1500:
               case 1550:
               case 1600:
                  continue;
            }
         }
         else if (flags & MOCA_BAND_F_FLAG)
         {
            switch (val)
            {
               case   0:
               case 675:
               case 700:
               case 725:
               case 750:
               case 775:
               case 800:
               case 825:
               case 850:
                  continue;
            }
         }
         else if (flags & MOCA_BAND_D_LOW_FLAG)
         {
            switch (val)
            {
               case    0:
               case 1150:
               case 1200:
                  continue;
            }
         }
         else if (flags & MOCA_BAND_EX_D_FLAG)
         {
            switch (val)
            {
               case    0:
               case 1150:
               case 1200:
               case 1250:
               case 1300:
               case 1350:
               case 1400:
               case 1450:
               case 1500:
               case 1550:
               case 1600:
                  continue;
            }
         }
         else if (flags & MOCA_BAND_E_FLAG)
         {
            switch (val)
            {
               case   0:
               case 500:
               case 525:
               case 550:
               case 575:
               case 600:
                  continue;
            }
         }
         else if (flags & MOCA_BAND_C4_FLAG)
         {
            switch (val)
            {
               case 1000:
                  continue;
            }
         }
         else if (flags & MOCA_BAND_H_FLAG)
         {
            switch (val)
            {
            case    0:
            case  975:
            case 1000:
            case 1025:
               continue;
            }
         }
         else if (flags & MOCA_BAND_GENERIC_FLAG)
         {
            if ((val >= 150) && (val <= 2125))
            {
               continue;
            }
         }
         return(1);
      } while (0);

   return 0;
}

/* This function checks that the configuration is valid */
MOCAD_S int mocad_parameter_check(struct mocad_ctx *ctx)
{
   int ret= 0;
   uint32_t oper_freq;

   if (ctx->cof == 0)
      oper_freq = ctx->any_time.lof;
   else
      oper_freq = ctx->cof;
   
       ret = mocad_lof_check(oper_freq, ctx->config_flags) ;
       if ( ret != 0 )
       { 
            mocad_log(ctx, L_WARN, 
                        "Invalid LOF %d. Not restarting firmware\n", oper_freq);
            return (ret);
       }


   if ((oper_freq == 1000) && (ctx->any_time.secondary_ch_offset != 0) && (ctx->any_time.secondary_ch_offset != 1))
   {
      mocad_log(ctx, L_WARN, 
         "Invalid secondary_ch_offset(%d)/lof(%d) combination.\n",
         ctx->any_time.secondary_ch_offset, oper_freq);
   }

   if (ctx->any_time.rf_band == MOCA_RF_BAND_GENERIC)
   {
      if ((ctx->any_time.primary_ch_offset != 0) && (ctx->any_time.primary_ch_offset != 1))
      {
         mocad_log(ctx, L_WARN, 
            "Invalid primary_ch_offset(%d) for GENERIC RF-band. Not restarting firmware.\n",
            ctx->any_time.primary_ch_offset);
         return(-1);
      }
      else if (ctx->any_time.secondary_ch_offset < 0)
      {
         mocad_log(ctx, L_WARN, 
            "Invalid secondary_ch_offset(%d) for GENERIC RF-band. Not restarting firmware.\n",
            ctx->any_time.secondary_ch_offset);
         return(-1);
      }
      else if (ctx->any_time.single_channel_operation == MOCA_SINGLE_CH_OP_NETWORK_SEARCH)
      {
         mocad_log(ctx, L_WARN, 
            "Invalid single_channel_operation(%d) for GENERIC RF-band. Not restarting firmware.\n",
            ctx->any_time.single_channel_operation);
         return(-1);
      }
   }


   return(0);
}

MOCAD_S int mocad_wom_pattern_check(struct mocad_ctx *ctx, struct moca_wom_pattern_set * pdata)
{
   int i;

   /* There is a requirement that bytes 0-13 of the mask field be either 0x00 or 0xFF */
   for (i = 0; i < 14; i++)
   {
      if ((pdata->mask[i] != 0x00) && (pdata->mask[i] != 0xFF))
      {
         mocad_log(ctx, L_WARN, "WOM Pattern mask bytes 0-13 must be 0x00 or 0xFF\n");
         return(-1);
      }
   }

   return(0);
}

#define DO_DIFF(res, a, b, field, count) {res->field = a->field - b->field; count += sizeof(res->field);}
MOCAD_S void mocad_diff_gen_stats(struct mocad_ctx * ctx,
                                struct moca_gen_stats * p_result, 
                                struct moca_gen_stats * p_new, 
                                struct moca_gen_stats * p_old)
{
   uint32_t count = 0;

   DO_DIFF(p_result, p_new, p_old, ecl_tx_total_pkts, count);
   DO_DIFF(p_result, p_new, p_old, ecl_tx_ucast_pkts, count);
   DO_DIFF(p_result, p_new, p_old, ecl_tx_bcast_pkts, count);
   DO_DIFF(p_result, p_new, p_old, ecl_tx_mcast_pkts, count);
   DO_DIFF(p_result, p_new, p_old, ecl_tx_ucast_unknown, count);
   DO_DIFF(p_result, p_new, p_old, ecl_tx_mcast_unknown, count);
   DO_DIFF(p_result, p_new, p_old, ecl_tx_ucast_drops, count);
   DO_DIFF(p_result, p_new, p_old, ecl_tx_mcast_drops, count);
   DO_DIFF(p_result, p_new, p_old, ecl_tx_total_bytes, count);
   DO_DIFF(p_result, p_new, p_old, ecl_tx_buff_drop_pkts, count);
   DO_DIFF(p_result, p_new, p_old, ecl_tx_error_drop_pkts, count);
   DO_DIFF(p_result, p_new, p_old, ecl_rx_total_pkts, count);
   DO_DIFF(p_result, p_new, p_old, ecl_rx_ucast_pkts, count);
   DO_DIFF(p_result, p_new, p_old, ecl_rx_bcast_pkts, count);
   DO_DIFF(p_result, p_new, p_old, ecl_rx_mcast_pkts, count);
   DO_DIFF(p_result, p_new, p_old, ecl_rx_ucast_drops, count);
   DO_DIFF(p_result, p_new, p_old, ecl_rx_mcast_filter_pkts, count);
   DO_DIFF(p_result, p_new, p_old, ecl_rx_total_bytes, count);
   DO_DIFF(p_result, p_new, p_old, ecl_fc_bg, count);
   DO_DIFF(p_result, p_new, p_old, ecl_fc_low, count);
   DO_DIFF(p_result, p_new, p_old, ecl_fc_medium, count);
   DO_DIFF(p_result, p_new, p_old, ecl_fc_high, count);
   DO_DIFF(p_result, p_new, p_old, ecl_fc_pqos, count);
   DO_DIFF(p_result, p_new, p_old, ecl_fc_bp_all, count);
   DO_DIFF(p_result, p_new, p_old, mac_tx_low_drop_pkts, count);
   DO_DIFF(p_result, p_new, p_old, mac_rx_buff_drop_pkts, count);
   DO_DIFF(p_result, p_new, p_old, mac_channel_usable_drop, count);
   DO_DIFF(p_result, p_new, p_old, mac_remove_node_drop, count);
   DO_DIFF(p_result, p_new, p_old, mac_loopback_pkts, count);
   DO_DIFF(p_result, p_new, p_old, mac_loopback_drop_pkts, count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_rx_max, count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_rx_count, count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[0], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[1], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[2], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[3], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[4], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[5], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[6], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[7], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[8], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[9], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[10], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[11], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[12], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[13], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[14], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[15], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[16], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[17], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[18], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[19], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[20], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[21], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[22], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[23], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[24], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[25], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[26], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[27], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[28], count);
   DO_DIFF(p_result, p_new, p_old, aggr_pkt_stats_tx[29], count);
   DO_DIFF(p_result, p_new, p_old, link_down_count, count);
   DO_DIFF(p_result, p_new, p_old, link_up_count, count);
   DO_DIFF(p_result, p_new, p_old, nc_handoff_counter, count);
   DO_DIFF(p_result, p_new, p_old, nc_backup_counter, count);
   DO_DIFF(p_result, p_new, p_old, resync_attempts_to_network, count);
   DO_DIFF(p_result, p_new, p_old, tx_beacons, count);
   DO_DIFF(p_result, p_new, p_old, tx_map_packets, count);
   DO_DIFF(p_result, p_new, p_old, tx_rr_packets, count);
   DO_DIFF(p_result, p_new, p_old, tx_ofdma_rr_packets, count);
   DO_DIFF(p_result, p_new, p_old, tx_control_uc_packets, count);
   DO_DIFF(p_result, p_new, p_old, tx_control_bc_packets, count);
   DO_DIFF(p_result, p_new, p_old, tx_protocol_ie, count);
   DO_DIFF(p_result, p_new, p_old, rx_beacons, count);
   DO_DIFF(p_result, p_new, p_old, rx_map_packets, count);
   DO_DIFF(p_result, p_new, p_old, rx_rr_packets, count);
   DO_DIFF(p_result, p_new, p_old, rx_ofdma_rr_packets, count);
   DO_DIFF(p_result, p_new, p_old, rx_control_uc_packets, count);
   DO_DIFF(p_result, p_new, p_old, rx_control_bc_packets, count);
   DO_DIFF(p_result, p_new, p_old, rx_protocol_ie, count);
   DO_DIFF(p_result, p_new, p_old, mac_frag_mpdu_tx, count);
   DO_DIFF(p_result, p_new, p_old, mac_frag_mpdu_rx, count);
   DO_DIFF(p_result, p_new, p_old, mac_pqos_policing_tx, count);
   DO_DIFF(p_result, p_new, p_old, mac_pqos_policing_drop, count);
   DO_DIFF(p_result, p_new, p_old, nc_became_nc_counter, count);
   DO_DIFF(p_result, p_new, p_old, nc_became_backup_nc_counter, count);
   DO_DIFF(p_result, p_new, p_old, rx_buffer_full_counter, count);

   if (count != sizeof(struct moca_gen_stats))
   {
      mocad_log(ctx, L_ERR, 
         "Missing field in mocad_diff_gen_stats. count = %d, should be %d\n",
         count, sizeof(struct moca_gen_stats));
   }
}

MOCAD_S void mocad_diff_node_stats(struct mocad_ctx * ctx,
                                struct moca_node_stats * p_result, 
                                struct moca_node_stats * p_new, 
                                struct moca_node_stats * p_old)
{
   uint32_t count = 0;

   DO_DIFF(p_result, p_new, p_old, tx_packets, count);
   DO_DIFF(p_result, p_new, p_old, rx_packets, count);
   DO_DIFF(p_result, p_new, p_old, primary_ch_rx_cw_unerror, count);
   DO_DIFF(p_result, p_new, p_old, primary_ch_rx_cw_corrected, count);
   DO_DIFF(p_result, p_new, p_old, primary_ch_rx_cw_uncorrected, count);
   DO_DIFF(p_result, p_new, p_old, primary_ch_rx_no_sync, count);
   DO_DIFF(p_result, p_new, p_old, secondary_ch_rx_cw_unerror, count);
   DO_DIFF(p_result, p_new, p_old, secondary_ch_rx_cw_corrected, count);
   DO_DIFF(p_result, p_new, p_old, secondary_ch_rx_cw_uncorrected, count);
   DO_DIFF(p_result, p_new, p_old, secondary_ch_rx_no_sync, count);

   if (count != sizeof(struct moca_node_stats))
   {
      mocad_log(ctx, L_ERR, 
         "Missing field in mocad_diff_node_stats. count = %d, should be %d\n",
         count, sizeof(struct moca_node_stats));
   }
}

MOCAD_S void mocad_diff_node_stats_ext(struct mocad_ctx * ctx,
                                struct moca_node_stats_ext * p_result, 
                                struct moca_node_stats_ext * p_new, 
                                struct moca_node_stats_ext * p_old)
{
   uint32_t count = 0;

   DO_DIFF(p_result, p_new, p_old, rx_uc_crc_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_uc_crc_error_sec_ch, count);
   DO_DIFF(p_result, p_new, p_old, rx_uc_timeout_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_uc_timeout_error_sec_ch, count);
   DO_DIFF(p_result, p_new, p_old, rx_bc_crc_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_bc_timeout_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_map_crc_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_map_timeout_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_beacon_crc_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_beacon_timeout_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_rr_crc_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_ofdma_rr_crc_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_rr_timeout_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_lc_uc_crc_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_lc_bc_crc_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_lc_uc_timeout_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_lc_bc_timeout_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_probe1_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_probe1_error_sec_ch, count);
   DO_DIFF(p_result, p_new, p_old, rx_probe2_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_probe3_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_probe1_gcd_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_plp_crc_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_plp_timeout_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_broken_packet_error, count);
   DO_DIFF(p_result, p_new, p_old, rx_broken_packet_error_sec_ch, count);
   DO_DIFF(p_result, p_new, p_old, rx_acf_crc_error, count);
   DO_DIFF(p_result, p_new, p_old, reserved_0, count); // Odd number of real fields

   if (count != sizeof(struct moca_node_stats_ext))
   {
      mocad_log(ctx, L_ERR, 
         "Missing field in mocad_diff_node_stats_ext. count = %d, should be %d\n",
         count, sizeof(struct moca_node_stats_ext));
   }
}

MOCAD_S int mocad_change_privacy_defaults(struct mocad_ctx *ctx, uint32_t new_val)
{
   if (ctx->privacy_defaults == new_val)
      return 0;

   if (MOCA_PRIVACY_DEFAULTS_MIN <= new_val &&
       new_val <= MOCA_PRIVACY_DEFAULTS_MAX)
      ctx->privacy_defaults = new_val ? 1 : 0;
   else
      return -1;

   // If privacy settings have changed manually or through MPS then they should be kept
   if (ctx->privacy_changed || ctx->mps_state == 1)
      return 0;

   // Otherwise they should be updated to the new defaults
   return mocad_set_privacy_defaults(ctx);
}

MOCAD_S int mocad_set_privacy_defaults(struct mocad_ctx *ctx)
{
   struct moca_password password;
   unsigned char *passwordp = (unsigned char *) &(password.password[0]);

   ctx->privacy_changed = 0;

   switch (ctx->privacy_defaults) {

   case 0: // Legacy defaults
      mocad_log(ctx, L_ALWAYS, "Set to new Privacy settings to Legacy Defaults\n");
      moca_set_privacy_en_defaults(&ctx->any_time.privacy_en, ctx->config_flags);
      moca_set_password_defaults(&password, ctx->config_flags);
      break;

   case 1: // MPS defaults
      mocad_log(ctx, L_ALWAYS, "Set to new Privacy settings to MPS Defaults\n");
      ctx->any_time.privacy_en = 1;
      mocad_random_password(passwordp);
      break;
   }

   if (mocad_validate_password(passwordp))
      mocad_log(ctx, L_ERR, "Invalid random password generated\n");
   else
      mocad_set_anytime_network_password(ctx, passwordp);

   // Note a restart is required for the defaults to take effect in FW

   return 0;
}


MOCAD_S int mocad_local_req(struct mocad_ctx *ctx, void *rd, int rd_len,
   void *wr, int wr_len)
{
   struct mmp_msg_hdr *mh;
   void  *data;
   int ret;
   int temp_ret;
   uint32_t ie_len, msg_type, ie_type;


   mh = (struct mmp_msg_hdr *) rd;
   msg_type = BE32(mh->type);
   ie_len = BE32(mh->length) - MOCA_EXTRA_MMP_HDR_LEN;
   ie_type = BE32(mh->ie_type);
   data = (void *)((uintptr_t)rd + sizeof(*mh));

   if(rd_len < (int)sizeof(*mh))
   {
      ret = -1;
      goto bad;
   }

   if((ie_len + sizeof(*mh)) > (uint32_t)rd_len)
   {
      ret = -2;
      goto bad;
   }
  
   ret = 0;

   /* Shortcut the processing of these because they are of no concern to mocad and 
    * on Standalone the anytime array is in Flash and so becomes a bottleneck for datapath
    */
   switch (ie_type) {
   case IE_MOCAD_FORWARDING_RX_ACK:
   case IE_MOCAD_FORWARDING_TX_SEND:
      return ret;
   }

   if((msg_type&0xF) == MOCA_MSG_GET)
   {
      if ((ret=mocad_get_anytime(ctx, ie_type, (uint32_t *)data, 
         (unsigned char *)(wr))) < 0)
         goto bad;
   }
   else
   {
      if ((ret=mocad_set_anytime(ctx, ie_type, (uint32_t *)data, 
         rd_len - sizeof(*mh), wr)) < 0)
         goto bad;
   }

   switch(ie_type) {

      case IE_MPS_BUTTON_PRESS: 
         {
            int rc = mocad_mps_button_press(ctx);
            ret = mocad_wr_result(wr, msg_type, ie_type, rc);
            break;
         }

      case IE_MPS_RESET: 
         {
            mocad_mps_reset(ctx);
            ret = mocad_wr_result(wr, msg_type, ie_type, 0);
            break;
         }

#if (MPS_EN == 0)
      case IE_MPS_EN:
         {
            if ((msg_type&0xF) == MOCA_MSG_SET)
               mocad_log(ctx, L_ERR, "MPS unsupported in this build!\n");
            ret = mocad_wr_result(wr, msg_type, ie_type, 0);
            break;
         }
#endif

#ifdef STANDALONE
      case IE_MPS_DATA_READY:
         {
            if (ctx->moca_running)
               mocad_handle_mps_data_ready(ctx);
            else
               mocad_log(ctx, L_ERR, "MPS_DATA_READY while not running!\n");
            ret = mocad_wr_result(wr, msg_type, ie_type, 0);
            break;
         }
#endif

      case IE_PRIVACY_DEFAULTS:
         if((msg_type&0xF) == MOCA_MSG_GET) {
            uint32_t *r;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
                  sizeof(*r));

            r = (uint32_t *)((uintptr_t)wr + ret);
            *r = BE32(ctx->privacy_defaults);
            ret += sizeof(*r);
            break;
         }
         if((msg_type&0xF) == MOCA_MSG_SET) {
            const int len = sizeof(uint32_t);
            uint32_t newv;
            int rc;

            if(rd_len != (sizeof(*mh) + len))
               goto bad;

            newv = BE32(*(uint32_t *)data);
            rc = mocad_change_privacy_defaults(ctx, newv);

            ret = mocad_wr_result(wr, msg_type, ie_type, rc);
            break;
         }
         goto bad;

      case IE_PRIVACY_EN:

         if ((msg_type&0xF) == MOCA_MSG_SET) {
            // use the latest persisted privacy settings
            if (ctx->any_time.dont_start_moca == MOCA_DONT_START_MOCA_WAIT
                && ctx->mps_state) {
               // Ignore since we have either:
               // Paired MPS data (mps_state 1) which should take precedence, or
               // MPS Reset (mps_state 2) in which case Defaults should be maintained
               // Therefore revert what anytime did above
               mocad_reread_mps_data(ctx);
               ret = mocad_wr_result(wr, msg_type,
                                     ie_type, 0);
            }
            else {
               ctx->privacy_changed = PRIVACY_CHANGED_MANUAL;
               ctx->mps_state = 0;
               ctx->any_time.mps_state = 0;
               // No need to inform FW of Unpaired state, it'll be cleared on restart
               mocad_clear_mps_data(ctx);
               mocad_mps_stop_unpaired_timer(ctx);
            }
         }
        break;

      case IE_TABOO_CHANNELS:
         if((msg_type&0xF) == MOCA_MSG_GET) {
            // Force the values from the firmware to be used in the reply
            if (ctx->moca_running)
               ret = 0;
         }
         break;

      case IE_WOM_MODE:
         if((msg_type&0xF) == MOCA_MSG_SET) {
            int rc;
            if ((rc = MoCAOS_WolCtrl(ctx->os_handle, BE32(*(int*)data)) != 0)) {
               mocad_log(ctx, L_ERR, "Failed to change WOM mode\n");
               goto bad;
            }
         }
         break;
      case IE_RESET_STATS: /* WO */
         mocad_reset_stats(ctx);
         ret = mocad_wr_result(wr, msg_type, ie_type, 0);
         break;
      case IE_EXT_OCTET_COUNT: { /* RO */
         struct moca_ext_octet_count *r;

         /* This doesn't produce real values yet. Need firmware support */
         ret = mocad_wr_get_response(wr, msg_type, ie_type,
            sizeof(*r));
         r = (struct moca_ext_octet_count *)((uintptr_t)wr + ret);

         r->in_octets_hi = (uint32_t)BE32(ctx->in_octets >> 32);
         r->in_octets_lo = (uint32_t)BE32(ctx->in_octets & 0xffffffff);
         r->out_octets_hi = (uint32_t)BE32(ctx->out_octets >> 32);
         r->out_octets_lo = (uint32_t)BE32(ctx->out_octets & 0xffffffff);

         ret += sizeof(*r);
         break;
      }
      case IE_START: /* WO */
         if(ctx->moca_running) {
            /* already running */
            ret = mocad_wr_result(wr, msg_type,
               ie_type, 1);
            break;
         }

         ctx->restart = 9;
         ret = mocad_wr_result(wr, msg_type, ie_type, 0);
         mocad_log(ctx, L_VERBOSE, "%sStarting MoCA interface\n", 
            (ctx->any_time.dont_start_moca == MOCA_DONT_START_MOCA_ON ? "Not " : ""));
         break;
      case IE_STOP: /* WO */
         if(! ctx->moca_running) {
            /* already stopped */
            ret = mocad_wr_result(wr, msg_type,
               ie_type, 1);
            break;
         }
         mocad_bcast_link_down(ctx);
         mocad_mps_stop(ctx);
         /* update running flag before sending shutdown in order to distinguish  
          * asserts happening after shutdown.
          */
         mocad_update_times(ctx, MOCA_TIME_EVENT_CORE_DOWN);
         ctx->moca_running = 0;

         __moca_set_shutdown(ctx);

         MoCAOS_EnableDataIf(ctx->os_handle, ctx->ifname, 2);
         // In order release all the data buffers that already exist in the Packet RAM wait 20 Msec..
         MoCAOS_MSleep(20);

         if (MoCAOS_StopCore(ctx->os_handle) != 0)
         {
            die("MoCAOS_StopCore failed");
            MoCAOS_MSleep(4000);
            RESTART(10);
            ret = mocad_wr_result(wr, msg_type, ie_type, 1);
         }
         else
         {
            ret = mocad_wr_result(wr, msg_type, ie_type, 0);
            mocad_log(ctx, L_INFO, "Stopping MoCA interface\n");
            /* Ensure we do not restart following stop command by clearing the restart flag.
             * It could have been set in case one of the operations here failed
             * and consequently set it. 
             */
            ctx->restart = 0;
         }

         break;
      case IE_MOCAD_VERSION: { /* RO */
         struct moca_mocad_version *a;
         if((msg_type&0xF) == MOCA_MSG_GET) {
         
            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(*a));
         
            a = (struct moca_mocad_version *)((uintptr_t)wr + ret);

            if (ctx->moca_running)
            {
               moca_get_version(
                  &a->mocad_version_moca, 
                  &a->mocad_version_major, 
                  &a->mocad_version_minor, 
                  &a->mocad_version_patch);

               a->mocad_version_moca = BE32(a->mocad_version_moca);
               a->mocad_version_major = BE32(a->mocad_version_major);
               a->mocad_version_minor = BE32(a->mocad_version_minor);
               a->mocad_version_patch = BE32(a->mocad_version_patch);
            }
            else
               memset(a, 0, sizeof(*a));

            ret += sizeof(*a);
         }
         break;
      }
      case IE_RESTART: /* WO */
         if(ctx->moca_running) 
         {
            mocad_update_times(ctx, MOCA_TIME_EVENT_CORE_DOWN);
            ctx->moca_running = 0;

            MoCAOS_EnableDataIf(ctx->os_handle, ctx->ifname, 2);
            // In order release all the data buffers that already exist in the Packet RAM wait 20 Msec..
            MoCAOS_MSleep(20);

            mocad_bcast_link_down(ctx);
            mocad_mps_stop(ctx);
            if (MoCAOS_StopCore(ctx->os_handle) != 0)
            {
               die("MoCAOS_StopCore failed");
               MoCAOS_MSleep(4000);
               RESTART(20);
               ret = mocad_wr_result(wr, msg_type, ie_type, 1);
            }
            else
            {
               ret = mocad_wr_result(wr, msg_type, ie_type, 0);
               mocad_log(ctx, L_INFO, "Stopping MoCA interface\n");
            }
         }
         ctx->restart = 15;
         ret = mocad_wr_result(wr, msg_type, ie_type, 0);
         mocad_log(ctx, L_INFO, "%sStarting MoCA interface\n", 
            (ctx->any_time.dont_start_moca == MOCA_DONT_START_MOCA_ON ? "Not " : ""));
         break;   
      case IE_PASSWORD: { /* RW */
         struct moca_password *a;
         if((msg_type&0xF) == MOCA_MSG_SET) {

            // use the latest persisted privacy settings
            if (ctx->any_time.dont_start_moca == MOCA_DONT_START_MOCA_WAIT
                && ctx->mps_state) {
               // Ignore since we have either:
               // Paired MPS data (mps_state 1) which should take precedence, or
               // MPS Reset (mps_state 2) in which case Defaults should be maintained
               // Therefore revert what anytime did above
               mocad_reread_mps_data(ctx);
               ret = mocad_wr_result(wr, msg_type,
                                     ie_type, 0);
               break;
            }

            a = ( struct moca_password *)data;

            ret = mocad_validate_password((unsigned char *)a->password);
         
            if (ret != 0)
            {
               mocad_log(ctx, L_WARN, "Error! Invalid password provided.\n"
                                      "Password must be composed of numeric digits 0-9, "
                                      "with length from 12 to 17 characters.\n");
               memset(&ctx->any_time.password, 0, sizeof(ctx->any_time.password));
               strcpy(ctx->any_time.password.password, ctx->any_time.network_password.password);
            }
            else
            {
               memset(&ctx->any_time.network_password, 0, sizeof(ctx->any_time.network_password));
               strcpy(ctx->any_time.network_password.password, a->password);
               ctx->privacy_changed = PRIVACY_CHANGED_MANUAL;
               ctx->mps_state = 0;
               ctx->any_time.mps_state = 0;
               // No need to inform FW of Unpaired state, it'll be cleared on restart
               mocad_clear_mps_data(ctx);
               mocad_mps_stop_unpaired_timer(ctx);
            }

            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
         }
         break;
      }
      case IE_DRV_INFO: { /* RO */
         struct moca_drv_info *r;

         if (MoCAOS_GetDriverInfo(ctx->os_handle, &ctx->kdrv_info) != 0)
         {
            mocad_log(ctx, L_ERR, "BAD: no driver info\n");
            goto bad;
         }

         ret = mocad_wr_get_response(wr, msg_type, ie_type,
            sizeof(*r));
         r = (struct moca_drv_info *)((uintptr_t)wr + ret);
         r->version = BE32(ctx->kdrv_info.version);
         r->build_number = BE32(ctx->kdrv_info.build_number);
         r->uptime = BE32(ctx->kdrv_info.uptime);
         r->hw_rev = BE32(ctx->kdrv_info.hw_rev);
         r->chip_id = BE32(ctx->kdrv_info.chip_id);
         r->rf_band = BE32(ctx->kdrv_info.rf_band);
         strncpy((char *)r->ifname, ctx->ifname, 16);
         r->ifname[15] = '\0';
         mocad_get_core_times(ctx, &r->core_uptime, &r->link_uptime);

         r->reset_count = BE32(ctx->reset_count);
         r->link_up_count = BE32(ctx->link_up_count);
         r->link_down_count = BE32(ctx->link_down_count);
         r->topology_change_count = BE32(ctx->topology_change_count);
         r->assert_count = BE32(ctx->assert_count);
         r->last_assert_num = BE32(ctx->last_assert_num);
         r->wdog_count = BE32(ctx->wdog_count);
         r->restart_history = BE32(ctx->restart_history);

         if (BE32(*(uint32_t *)data))
         {
            // Reset the counters
            ctx->reset_count           = 0;
            ctx->link_up_count         = 0;
            ctx->link_down_count       = 0;
            ctx->topology_change_count = 0;
            ctx->assert_count          = 0;
            ctx->wdog_count            = 0;
         }

         ret += sizeof(*r);
         break;
      }
      case IE_SNR_DATA: {
         void * pMem;
         int    retVal = 0;

         if(ctx->preconfig.snroffset == 0)
            goto bad;

         ret = mocad_wr_get_response(wr, msg_type, ie_type, SNR_DATA_SIZE);

         /* the memory transfer at the kernel layer might require 
          * 64-bit alignment */
         if (MoCAOS_MemAlign(&pMem, 64, SNR_DATA_SIZE) < 0)
            goto bad;

         retVal = MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)pMem, 
               SNR_DATA_SIZE,
               (unsigned char *)(ctx->preconfig.snroffset & 0x1fffffff));

         if (retVal != 0)
         {
            mocad_log(ctx, L_ERR, "Failure to read SNR data (%d)\n", retVal);
            MoCAOS_FreeMemAlign(pMem);
            goto bad;
         }
         memcpy((unsigned char *)((uintptr_t)wr + ret), pMem, SNR_DATA_SIZE);
         MoCAOS_FreeMemAlign(pMem);
         ret += SNR_DATA_SIZE;
         break;
      }
      case IE_IQ_DATA: {
         void * pMem;
         int    retVal = 0;

         if(ctx->preconfig.iqoffset == 0)
            goto bad;

         ret = mocad_wr_get_response(wr, msg_type, ie_type, IQ_DATA_SIZE);

         /* the memory transfer at the kernel layer might require 
          * 64-bit alignment */
         if (MoCAOS_MemAlign(&pMem, 64, IQ_DATA_SIZE) < 0)
            goto bad;

         retVal = MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)pMem, 
               IQ_DATA_SIZE,
               (unsigned char *)(ctx->preconfig.iqoffset & 0x1fffffff));

         if (retVal != 0)
         {
            mocad_log(ctx, L_ERR, "Failure to read IQ data (%d)\n", retVal);
            MoCAOS_FreeMemAlign(pMem);
            goto bad;
         }
         memcpy((void *)(((uintptr_t)wr + ret)), pMem, IQ_DATA_SIZE);
         MoCAOS_FreeMemAlign(pMem);
         ret += IQ_DATA_SIZE;
         break;
      }
      case IE_CIR_DATA: {
         uint32_t node;

         node = BE32(*(uint32_t *)data);

         ret = mocad_wr_get_response(wr, msg_type, ie_type, CIR_DATA_SIZE);
#ifndef STANDALONE
         memcpy((void *)(((uintptr_t)wr + ret)), ctx->cir_data[node], CIR_DATA_SIZE);
#endif
         ret += CIR_DATA_SIZE;
         break;
      }
      case IE_RESTORE_DEFAULTS: /* WO */
         if (ctx->any_time.dont_start_moca != MOCA_DONT_START_MOCA_WAIT)
            mocad_mps_reset(ctx);
         mocad_restore_defaults(ctx, mocad_get_default_freq(ctx), ctx->any_time.dont_start_moca);
         ret = mocad_wr_result(wr, msg_type, ie_type, 0);
         break;

      case IE_NV_CAL_CLEAR: /* WO */
         /* Clear the RF Calibration data and the Probe II result data */
         mocad_nv_cal_clear(ctx);

         ret = mocad_wr_result(wr, msg_type, ie_type, 0);
         break;

      case IE_MCFILTER_DELENTRY:
         if((msg_type&0xF) == MOCA_MSG_SET) 
         {
           struct moca_mcfilter_addentry  *pData = 
              (struct moca_mcfilter_addentry  *) data;
           const int len = sizeof(struct moca_mcfilter_addentry  );

           if(rd_len != (sizeof(*mh) + len))
               goto bad;
 
           ret = mocad_handle_del_mcfilter(ctx, &pData->addr);

           ret = mocad_wr_result(wr, msg_type, ie_type, ret);
         }
         break;  
      case IE_MCFILTER_ADDENTRY:
         if((msg_type&0xF) == MOCA_MSG_SET) 
         {
           const int len = sizeof(struct moca_mcfilter_addentry );   
           struct moca_mcfilter_addentry  *pData = 
                  (struct moca_mcfilter_addentry  *) data;

           if(rd_len != (sizeof(*mh) + len))
              goto bad;

           ret = mocad_handle_add_mcfilter(ctx, &pData->addr);
 
           ret = mocad_wr_result(wr, msg_type, ie_type, ret);
         }
         break;
      case IE_MCFILTER_CLEAR_TABLE: 
         
         ret = mocad_reset_mcfilter(ctx);
         ret = mocad_wr_result(wr, msg_type, ie_type, 0);
         break;        
      case  IE_MCFILTER_TABLE:{ 
         struct moca_mcfilter_table *r = 
            (struct moca_mcfilter_table *)data; 
         int i;

         ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(*r));
         r = (struct moca_mcfilter_table *)((uintptr_t)wr + ret);
         
         for (i = 0; i < MOCA_MAX_ECL_MCFILTERS; i++)
         {
           MOCA_MACADDR_COPY(&r->addr, &ctx->ecl_mcfilter[i]);
           ret += sizeof(ctx->ecl_mcfilter[i]);     
           r = (struct moca_mcfilter_table *)((uintptr_t)wr + ret);
         }
         break; 
      }
      case IE_EGR_MC_ADDR_FILTER:
         if((msg_type&0xF) == MOCA_MSG_SET) 
         {
            struct moca_egr_mc_addr_filter_set *filter = 
               (struct moca_egr_mc_addr_filter_set *)data; 
            const int len = sizeof(struct moca_egr_mc_addr_filter);

            if(rd_len != (sizeof(*mh) + len))
               goto bad;

            filter->valid = BE32(filter->valid);
            filter->entryid = BE32(filter->entryid);
                
            if (filter->entryid >= MOCA_MAX_EGR_MC_FILTERS)
               goto bad;

            if ((ctx->egr_mc_addr_filter[filter->entryid].valid != filter->valid) ||
                !MOCA_MACADDR_COMPARE(&ctx->egr_mc_addr_filter[filter->entryid].addr, &filter->addr))
            {
               ctx->egr_mc_addr_filter[filter->entryid].valid = filter->valid;
               MOCA_MACADDR_COPY(&ctx->egr_mc_addr_filter[filter->entryid].addr, &filter->addr);

               
               if (ctx->moca_running)
                  moca_set_egr_mc_addr_filter(ctx, filter);
            }

            ret = mocad_wr_result(wr, msg_type,
               ie_type, 0);
         }
         else
         {
            struct moca_egr_mc_addr_filter *r;
            struct moca_egr_mc_addr_filter *filter = 
               (struct moca_egr_mc_addr_filter *)data; 

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(*r));

            r = (struct moca_egr_mc_addr_filter *)((uintptr_t)wr + ret);

            if (BE32(filter->entryid) >= MOCA_MAX_EGR_MC_FILTERS)
            {
               ret = -1;
               break;
            }

            r->valid = BE32(ctx->egr_mc_addr_filter[BE32(filter->entryid)].valid);
            r->entryid = filter->entryid; // double BE32 cancels
            MOCA_MACADDR_COPY(&r->addr, &ctx->egr_mc_addr_filter[BE32(filter->entryid)].addr);

            ret += sizeof(*r);      
         }
         break;
      case IE_RF_BAND:
         if((msg_type&0xF) == MOCA_MSG_SET) {
            mocad_set_rf_band(ctx, BE32(*(unsigned int *)data));
            ret = mocad_wr_result(wr, msg_type,
               ie_type, 0);
         } else {
            uint32_t *r;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
                sizeof(*r)); 

            r = (uint32_t *)((uintptr_t)wr + ret);
            *r = BE32(ctx->any_time.rf_band);

            ret += sizeof(*r);
         }
         break;
      case IE_MAX_CONSTELLATION:
         /* store set values in context */
         if((msg_type&0xF) == MOCA_MSG_SET) {
            const int len = sizeof(struct moca_max_constellation);
            struct moca_max_constellation *mc = (struct moca_max_constellation *)data;

            if(rd_len != (sizeof(*mh) + len))
               goto bad;

            if (moca_max_constellation_check(mc, ctx->config_flags))
               goto bad;

            if (BE32(mc->node_id) >= MOCA_MAX_NODES)
               goto bad;

            ctx->max_constellation[BE32(mc->node_id)].p2p_limit_50  = BE32(mc->p2p_limit_50);
            ctx->max_constellation[BE32(mc->node_id)].gcd_limit_50  = BE32(mc->gcd_limit_50);
            ctx->max_constellation[BE32(mc->node_id)].p2p_limit_100 = BE32(mc->p2p_limit_100);
            ctx->max_constellation[BE32(mc->node_id)].gcd_limit_100 = BE32(mc->gcd_limit_100);

            // moca_set_max_constellation will byte swap these values but they are already byte swapped, so we byte swap them
            mc->p2p_limit_50  = BE32(mc->p2p_limit_50);
            mc->gcd_limit_50  = BE32(mc->gcd_limit_50);
            mc->p2p_limit_100 = BE32(mc->p2p_limit_100);
            mc->gcd_limit_100 = BE32(mc->gcd_limit_100);
            mc->node_id = BE32(mc->node_id);
            if (ctx->moca_running)
               moca_set_max_constellation(ctx, mc);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, 0);
         }
         else
         {
            struct moca_max_constellation *r;
            uint32_t node_id = *((uint32_t *)data);

            if (BE32(node_id) >= MOCA_MAX_NODES)
               goto bad;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(*r));

            r = (struct moca_max_constellation *)((uintptr_t)wr + ret);

            r->p2p_limit_50  = BE32(ctx->max_constellation[BE32(node_id)].p2p_limit_50);
            r->gcd_limit_50  = BE32(ctx->max_constellation[BE32(node_id)].gcd_limit_50);
            r->p2p_limit_100 = BE32(ctx->max_constellation[BE32(node_id)].p2p_limit_100);
            r->gcd_limit_100 = BE32(ctx->max_constellation[BE32(node_id)].gcd_limit_100);
            r->node_id       = node_id;

            ret += sizeof(*r); 
         }
         break;
      case IE_MOCA_CORE_TRACE_ENABLE: { /* RW */
         if((msg_type&0xF) == MOCA_MSG_GET) {
            uint32_t *r;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(*r));

            r = (uint32_t *)((uintptr_t)wr + ret);
            *r = BE32(ctx->show_lab_printf);
            ret += sizeof(*r);
            break;
         }
         if((msg_type&0xF) == MOCA_MSG_SET) {
            const int len = sizeof(uint32_t);
            uint32_t *val = (uint32_t *)data;

            if(rd_len != (sizeof(*mh) + len))
               goto bad;

            ctx->show_lab_printf = !!(*val);
            /* if the link is down, keep traces on for wdog */
            if (ctx->moca_running && ctx->lab_printf_wdog_count &&
               (ctx->show_lab_printf ||
                (ctx->link_state == LINK_STATE_UP)))
            {
               mocad_set_moca_core_trace_enable(ctx,
                  ctx->show_lab_printf);
            }
            ret = mocad_wr_result(wr, msg_type,
               ie_type, 0);

            break;
         }
         goto bad;
      }
      case IE_FW_FILE: { /* WO */
         struct moca_fw_file *r = (struct moca_fw_file *)data;

         ctx->fw_file = (char *)r->fw_file;
         mocad_log(ctx, L_VERBOSE, "FW_FILE %s\n", ctx->fw_file);
         ctx->fw_img = MoCAOS_GetFw(ctx->os_handle, (unsigned char *)ctx->fw_file, 
            (int *)&ctx->fw_len);

         ret = mocad_wr_result(wr, msg_type,
            ie_type, 0);

         break;
      }
      case IE_BRCMTAG_ENABLE: { /* RW */
         if((msg_type&0xF) == MOCA_MSG_GET) {
            uint32_t *r;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
                  sizeof(*r));

            r = (uint32_t *)((uintptr_t)wr + ret);
            *r = BE32(ctx->any_time.brcmtag_enable);
            ret += sizeof(*r);
            break;
         }
         if((msg_type&0xF) == MOCA_MSG_SET) {
            const int len = sizeof(uint32_t);

            if(rd_len != (sizeof(*mh) + len))
               goto bad;

            __moca_copy_be32(&ctx->any_time.brcmtag_enable, data, len);
            mocad_set_brcmtag_enable(ctx, ctx->any_time.brcmtag_enable);

            ret = mocad_wr_result(wr, msg_type,
                  ie_type, 0);
            break;
         }
         goto bad;
      }
      case IE_HOST_QOS: { /* RW */
         if((msg_type&0xF) == MOCA_MSG_GET) {
            uint32_t *r;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
                  sizeof(*r));

            r = (uint32_t *)((uintptr_t)wr + ret);
            *r = BE32(ctx->any_time.host_qos);
            ret += sizeof(*r);
            break;
         }
         if((msg_type&0xF) == MOCA_MSG_SET) {
            const int len = sizeof(uint32_t);

            if(rd_len != (sizeof(*mh) + len))
               goto bad;

            __moca_copy_be32(&ctx->any_time.host_qos, data, len);

            ret = mocad_wr_result(wr, msg_type,
                  ie_type, 0);
            break;
         }
         goto bad;
      }
      case IE_VERBOSE: { /* RW */
         if((msg_type&0xF) == MOCA_MSG_GET) {
            uint32_t *r;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(*r));

            r = (uint32_t *)((uintptr_t)wr + ret);
            *r = BE32(ctx->verbose);
            ret += sizeof(*r);
            break;
         }
         if((msg_type&0xF) == MOCA_MSG_SET) {
            const int len = sizeof(uint32_t);

            if(rd_len != (sizeof(*mh) + len))
               goto bad;

            __moca_copy_be32(&ctx->verbose, data, len);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, 0);
            break;
         }
         goto bad;
      }

      case IE_NO_RTT: /* WO */
         ctx->verbose &= ~L_RTT;
         ret = mocad_wr_result(wr, msg_type, ie_type, 0);
         break;

      case IE_KEY_TIMES: {
         struct moca_key_times *r = (struct moca_key_times *)data;

         if((msg_type&0xF) == MOCA_MSG_GET) {
            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(*r));

            r = (struct moca_key_times *)((uintptr_t)wr + ret);
            __moca_copy_be32(r, &ctx->key_times, sizeof(*r));            
            
            ret += sizeof(*r);
         }
         break;
      }       
         
      case IE_ERROR_TO_MASK: { /* RW */
         if((msg_type&0xF) == MOCA_MSG_GET) {
            struct moca_error_to_mask *r;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(*r));

            r = (struct moca_error_to_mask *)((uintptr_t)wr + ret);
            __moca_copy_be32(r, &ctx->error_to_mask, sizeof(*r));

            ret += sizeof(*r);
            break;
         }
         if((msg_type&0xF) == MOCA_MSG_SET) {
            const int len = sizeof(ctx->error_to_mask);

            if(rd_len != (sizeof(*mh) + len))
               goto bad;

            __moca_copy_be32(&ctx->error_to_mask, data, len);
            mocad_write_e2m(ctx, &ctx->error_to_mask);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, 0);
            break;
         }
         goto bad;
      }
      case IE_MESSAGE: { /* WO */
         uint32_t *r = (uint32_t *)data;
         
         ret = mocad_send_notification(ctx, BE32(*r));
         if (ret == 0) {
            ret = mocad_wr_result(wr, msg_type,
               ie_type, 0);
         } else {
            ret = mocad_wr_result(wr, msg_type,
               ie_type, 1);
         }
         break;
      }
      case IE_FMR_INIT: {
         /* This is a "do" command. mocad is responsible for
            sending the request to the core and then sending
            the resulting trap back to the client */
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->any_time.en_capable && ctx->moca_running) {
            uint32_t node_mask = BE32(*(uint32_t *)data);

            ret = mocad_do_fmr_init(ctx, node_mask);

            ret = mocad_wr_result(wr, msg_type, ie_type, ret);

            break;            
         }
         goto bad;
      }
      case IE_FMR_20: {
         /* This is a "do" command. mocad is responsible for
            sending the request to the core and then sending
            the resulting trap back to the client */
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->any_time.en_capable && ctx->moca_running) {
            uint32_t node_mask = BE32(*(uint32_t *)data);

            ret = mocad_do_fmr_20(ctx, node_mask);

            ret = mocad_wr_result(wr, msg_type, ie_type, ret);

            break;            
         }
         goto bad;
      }
      case IE_ACA: {
         /* This is a "do" command. mocad is responsible for
            sending the request to the core and then sending
            the resulting trap back to the client */
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->moca_running) {
            ret = mocad_do_aca(ctx, data);

            ret = mocad_wr_result(wr, msg_type, ie_type, ret);

            break;            
         }
         goto bad;
      }
      case IE_PQOS_CREATE_FLOW: {
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->any_time.en_capable && ctx->moca_running) {
            ret = mocad_do_pqos_flow_create(ctx, data);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
            break;
         }
         goto bad;
      }
      case IE_PQOS_UPDATE_FLOW: {
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->any_time.en_capable && ctx->moca_running) {
            ret = mocad_do_pqos_flow_update(ctx, data);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
            break;
         }
         goto bad;
      }
      case IE_PQOS_DELETE_FLOW: {
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->any_time.en_capable && ctx->moca_running) {
            ret = mocad_do_pqos_flow_delete(ctx, data);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
            break;
         }
         goto bad;
      }
      case IE_PQOS_QUERY: {
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->any_time.en_capable && ctx->moca_running) {
            ret = mocad_do_pqos_flow_query(ctx, data);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
            break;
         }
         goto bad;
      }

      case IE_PQOS_STATUS: {
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->any_time.en_capable && ctx->moca_running) {
            ret = mocad_do_pqos_status(ctx, data);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
            break;
         }
         goto bad;
      }

      case IE_PQOS_LIST: {
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->any_time.en_capable && ctx->moca_running) {
            ret = mocad_do_pqos_list(ctx, data);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
            break;
         }
      }
      case IE_MOCA_RESET: {
         /* This is a "do" command. mocad is responsible for
            sending the request to the core and then sending
            the resulting trap back to the client */
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->any_time.en_capable && ctx->moca_running) {
            ret = mocad_do_moca_reset(ctx, data);

            ret = mocad_wr_result(wr, msg_type, ie_type, ret);

            break;            
         }
         goto bad;
      }
      case IE_DD_INIT: {
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->any_time.en_capable && ctx->moca_running) {
            ret = mocad_do_dd_init(ctx, data);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
            break;
         }
         goto bad;
      }
      case IE_HOSTLESS_MODE: {
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->moca_running) {
            uint32_t enable = BE32(*(uint32_t *)data);
            ret = mocad_do_hostless_mode(ctx, enable);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
            break;
         }
         goto bad;
      }
      case IE_WAKEUP_NODE: {
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->moca_running) {
            ret = mocad_do_wakeup_node(ctx, data);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
            break;
         }
         goto bad;
      }
      case IE_PS_CMD: {
         if(((msg_type&0xF) == MOCA_MSG_SET) && ctx->moca_running) {
            ret = mocad_do_ps_cmd(ctx, data);
      
            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
            break;
         }
         goto bad;
      }
      case IE_POWER_STATE_CAPABILITIES: {
         uint32_t *r;
         if((msg_type&0xF) == MOCA_MSG_GET) {
            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(uint32_t));
      
            r = (uint32_t *)((uintptr_t)wr + ret);
            *r = BE32(ctx->ps_capabilities);
            ret += sizeof(*r);
            break;
         }
         goto bad;
      }
      case IE_CONFIG_FLAGS: {
         uint32_t *r;
         if((msg_type&0xF) == MOCA_MSG_GET) {
            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(uint32_t));
            r = (uint32_t *)((uintptr_t)wr + ret);
            *r = BE32(ctx->config_flags);
            ret += sizeof(*r);
            break;
         }
         goto bad;
      }
      case IE_ERROR_STATS: {
         struct moca_error_stats *r;
         struct moca_node_stats_ext node_stats_ext;
         struct moca_node_stats_ext node_stats_ext_temp;
         struct moca_network_status ns;
         struct moca_node_stats_ext_in in;
         
         if(((msg_type&0xF) == MOCA_MSG_GET) && ctx->moca_running) {

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(*r));

            r = (struct moca_error_stats *)((uintptr_t)wr + ret);
            memset(r, 0, sizeof(*r));

            if (ctx->moca_running)
            {

               temp_ret = moca_get_network_status(ctx, &ns);
               if (temp_ret != 0)
                  goto bad;

               memset(&in, 0, sizeof(in));
               for (in.index = 0; in.index < MOCA_MAX_NODES; in.index++) 
               {
                  if (ns.nodes_usable_bitmask & (1 << in.index))
                  {
                     temp_ret = moca_get_node_stats_ext(ctx, &in, &node_stats_ext_temp);
   
                     if (temp_ret != 0)
                        goto bad;

                     mocad_diff_node_stats_ext(ctx, &node_stats_ext, &node_stats_ext_temp, 
                        &ctx->node_stats_ext_base[in.index]);

                     r->rx_uc_crc_error += BE32((uint32_t) node_stats_ext.rx_uc_crc_error);
                     r->rx_uc_crc_error_sec_ch += BE32((uint32_t) node_stats_ext.rx_uc_crc_error_sec_ch);
                     r->rx_uc_timeout_error += BE32((uint32_t) node_stats_ext.rx_uc_timeout_error);
                     r->rx_uc_timeout_error_sec_ch += BE32((uint32_t) node_stats_ext.rx_uc_timeout_error_sec_ch);
                     r->rx_bc_crc_error += BE32((uint32_t) node_stats_ext.rx_bc_crc_error);
                     r->rx_bc_timeout_error += BE32((uint32_t) node_stats_ext.rx_bc_timeout_error);
                     r->rx_map_crc_error += BE32((uint32_t) node_stats_ext.rx_map_crc_error);
                     r->rx_map_timeout_error += BE32((uint32_t) node_stats_ext.rx_map_timeout_error);
                     r->rx_beacon_crc_error += BE32((uint32_t) node_stats_ext.rx_beacon_crc_error);
                     r->rx_beacon_timeout_error += BE32((uint32_t) node_stats_ext.rx_beacon_timeout_error);
                     r->rx_rr_crc_error += BE32((uint32_t) node_stats_ext.rx_rr_crc_error);
                     r->rx_ofdma_rr_crc_error += BE32((uint32_t) node_stats_ext.rx_ofdma_rr_crc_error);
                     r->rx_rr_timeout_error += BE32((uint32_t) node_stats_ext.rx_rr_timeout_error);
                     r->rx_lc_uc_crc_error += BE32((uint32_t) node_stats_ext.rx_lc_uc_crc_error);
                     r->rx_lc_bc_crc_error += BE32((uint32_t) node_stats_ext.rx_lc_bc_crc_error);
                     r->rx_lc_uc_timeout_error += BE32((uint32_t) node_stats_ext.rx_lc_uc_timeout_error);
                     r->rx_lc_bc_timeout_error += BE32((uint32_t) node_stats_ext.rx_lc_bc_timeout_error);
                     r->rx_probe1_error += BE32((uint32_t) node_stats_ext.rx_probe1_error);
                     r->rx_probe1_error_sec_ch += BE32((uint32_t) node_stats_ext.rx_probe1_error_sec_ch);
                     r->rx_probe2_error += BE32((uint32_t) node_stats_ext.rx_probe2_error);
                     r->rx_probe3_error += BE32((uint32_t) node_stats_ext.rx_probe3_error);
                     r->rx_probe1_gcd_error += BE32((uint32_t) node_stats_ext.rx_probe1_gcd_error);
                     r->rx_plp_crc_error += BE32((uint32_t) node_stats_ext.rx_plp_crc_error);
                     r->rx_plp_timeout_error += BE32((uint32_t) node_stats_ext.rx_plp_timeout_error);
                     r->rx_acf_crc_error += BE32((uint32_t) node_stats_ext.rx_acf_crc_error);
                  }                  
               }
            }

            ret += sizeof(*r);
         }
         break;
      }       

      case IE_MR_SEQ_NUM: {
         uint32_t *r;

         if((msg_type&0xF) == MOCA_MSG_GET) {

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(uint32_t));

            r = (uint32_t *)((uintptr_t)wr + ret);
            __moca_copy_be32(r, &ctx->disk_nondefseqnum, 
               sizeof(ctx->disk_nondefseqnum));
            ret += sizeof(ctx->disk_nondefseqnum);
            break;
         }
         if((msg_type&0xF) == MOCA_MSG_SET) {
            uint32_t seq_num = BE32(*((uint32_t *)data));

            ret = mocad_set_mr_seq_num(ctx, seq_num);
      
            ret = mocad_wr_result(wr, msg_type,
               ie_type, ret);
            break;
         }
         goto bad;
      }

      case IE_COF: {
         if((msg_type&0xF) == MOCA_MSG_SET) {
            ctx->cof = BE32(*(uint32_t *)data);

            ret = mocad_wr_result(wr, msg_type,
               ie_type, 0);
            break;
         }
         goto bad;
      }
      case IE_WOM_PATTERN: {
         if((msg_type&0xF) == MOCA_MSG_SET) {
            ctx->cof = BE32(*(uint32_t *)data);

            ret = mocad_wom_pattern_check(ctx, 
               (struct moca_wom_pattern_set *) data);
            if (ret == 0)
               break;
         }
         else if((msg_type&0xF) == MOCA_MSG_GET) {
            break;
         }
         goto bad;
      }

      case IE_GEN_STATS: {
         if((msg_type&0xF) == MOCA_MSG_GET) {
            struct moca_gen_stats gen_stats;
            struct moca_gen_stats * rsp;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
                     sizeof(gen_stats));
 
            rsp = (struct moca_gen_stats *)((uintptr_t)wr + ret);

            if (ctx->moca_running)
            {
               temp_ret = moca_get_gen_stats(ctx, 0, &gen_stats);

               if (temp_ret == MOCA_API_SUCCESS)
               {
                  mocad_diff_gen_stats(ctx, rsp, &gen_stats, 
                     &ctx->gen_stats_base);
                  moca_gen_stats_swap(rsp);
                  ret += sizeof(struct moca_gen_stats);
               }

               if (BE32(*(uint32_t *)data))
               {
                  memcpy(&ctx->gen_stats_base, &gen_stats, 
                     sizeof(gen_stats));
               }
            }

            break;
         }

         goto bad;
      }

      case IE_NODE_STATS: {
         if((msg_type&0xF) == MOCA_MSG_GET) {
            struct moca_node_stats node_stats;
            struct moca_node_stats * rsp;
            struct moca_node_stats_in * p_node_stats_in = (struct moca_node_stats_in *) data;
            struct moca_node_stats_in node_stats_in;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
                     sizeof(node_stats));
 
            rsp = (struct moca_node_stats *)((uintptr_t)wr + ret);

            if (ctx->moca_running)
            {
               node_stats_in.index = BE32(p_node_stats_in->index);
               node_stats_in.reset_stats = 0;
               
               temp_ret = moca_get_node_stats(ctx, &node_stats_in, &node_stats);

               if (temp_ret == MOCA_API_SUCCESS)
               {
                  mocad_diff_node_stats(ctx, rsp, &node_stats, 
                     &ctx->node_stats_base[node_stats_in.index]);

                  __moca_copy_be32(rsp, rsp, sizeof(*rsp));

                  ret += sizeof(struct moca_node_stats);
               }

               if (BE32(p_node_stats_in->reset_stats))
               {
                  memcpy(&ctx->node_stats_base[node_stats_in.index], 
                     &node_stats, sizeof(node_stats));
               }
            }

            break;
         }

         goto bad;
      }

      case IE_NODE_STATS_EXT: {
         if((msg_type&0xF) == MOCA_MSG_GET) {
            struct moca_node_stats_ext node_stats_ext;
            struct moca_node_stats_ext * rsp;
            struct moca_node_stats_ext_in * p_node_stats_ext_in = (struct moca_node_stats_ext_in *) data;
            struct moca_node_stats_ext_in node_stats_ext_in;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
                     sizeof(node_stats_ext));
 
            rsp = (struct moca_node_stats_ext *)((uintptr_t)wr + ret);

            if (ctx->moca_running)
            {
               node_stats_ext_in.index = BE32(p_node_stats_ext_in->index);
               node_stats_ext_in.reset_stats = 0;
               
               temp_ret = moca_get_node_stats_ext(ctx, &node_stats_ext_in, &node_stats_ext);

               if (temp_ret == MOCA_API_SUCCESS)
               {
                  mocad_diff_node_stats_ext(ctx, rsp, &node_stats_ext, 
                     &ctx->node_stats_ext_base[node_stats_ext_in.index]);

                  moca_node_stats_ext_swap(rsp);

                  ret += sizeof(struct moca_node_stats_ext);
               }

               if (BE32(p_node_stats_ext_in->reset_stats))
               {
                  memcpy(&ctx->node_stats_ext_base[node_stats_ext_in.index], 
                     &node_stats_ext, sizeof(node_stats_ext));
                  memcpy(&ctx->last_node_stats_ext[node_stats_ext_in.index], 
                     &node_stats_ext, sizeof(node_stats_ext));
               }
            }

            break;
         }

         goto bad;
      }

      case IE_AMP_REG: { /* RW */
         struct moca_amp_reg * amp_reg = (struct moca_amp_reg *) data;
         int                   ret_val;

         if((msg_type&0xF) == MOCA_MSG_GET) {
            struct moca_amp_reg * r;

            ret = mocad_wr_get_response(wr, msg_type, ie_type,
                     sizeof(*amp_reg));

            r = (struct moca_amp_reg *)((uintptr_t)wr + ret);
            r->addr = amp_reg->addr;

            ret_val = MoCAOS_Get3450Reg(ctx->os_handle, BE32(r->addr), (unsigned int*)(&r->value));
            r->value = BE32(r->value);

            if (ret_val != 0)
               goto bad;

            ret += sizeof(*r);
            break;
         }
         if((msg_type&0xF) == MOCA_MSG_SET) {
            const int len = sizeof(struct moca_amp_reg);

            if(rd_len != (sizeof(*mh) + len))
               goto bad;

            ret_val = MoCAOS_Set3450Reg(ctx->os_handle, BE32(amp_reg->addr), BE32(amp_reg->value));

            if (ret_val != 0)
               goto bad;

            ret = mocad_wr_result(wr, msg_type,
                  ie_type, 0);

            break;
         }
         goto bad;
      }

      case IE_LAST_PS_EVENT_CODE: {
         int32_t *r;
         if((msg_type&0xF) == MOCA_MSG_GET) {
            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(int32_t));
      
            r = (int32_t *)((uintptr_t)wr + ret);
            *r = BE32(ctx->last_ps_event_code);
            ret += sizeof(*r);
            break;
         }
         goto bad;
      }

      case IE_LAST_MR_EVENTS: {
         struct moca_last_mr_events *r;
         if((msg_type&0xF) == MOCA_MSG_GET) {
            ret = mocad_wr_get_response(wr, msg_type, ie_type,
               sizeof(struct moca_last_mr_events));
      
            r = (struct moca_last_mr_events *)((uintptr_t)wr + ret);
            r->last_cause     = BE32(ctx->last_mr_events.last_cause);
            r->last_seq_num   = BE32(ctx->last_mr_events.last_seq_num);
            r->last_mr_result = BE32(ctx->last_mr_events.last_mr_result);
            ret += sizeof(*r);
            break;
         }
         goto bad;
      }

   }

#define LOCL_MSG_LIMIT     0x80

   if(ret && (ctx->verbose & L_MMP_MSG)) {
      mocad_dump_buf(ctx, "LOCL<", (unsigned char *)rd, rd_len);
      if(ret > LOCL_MSG_LIMIT) {
         mocad_dump_buf(ctx, "LOCL>", (unsigned char *)wr, LOCL_MSG_LIMIT);
         mocad_dump_buf(ctx, "LOCL>", (unsigned char *)wr, LOCL_MSG_LIMIT - ret);
      } else {
         mocad_dump_buf(ctx, "LOCL>", (unsigned char *)wr, ret);
      }
   }

   return(ret);

bad:
   return(mocad_wr_abort(wr));
}

/* we want to allow some MMP communication in debug mode */
MOCAD_S int mocad_allow_msg_in_dbg(void * msg)
{
   struct mmp_msg_hdr *mh;

   mh = (struct mmp_msg_hdr *)msg;

   if (((BE32(mh->type)&0xF) == MOCA_MSG_GET) ||
       ((BE32(mh->type)&0xF) == MOCA_MSG_SET))
   {
      switch (BE32(mh->ie_type)) {
         case IE_LAB_REGISTER:
         case IE_LAB_CMD:
            return(1);
         default:
            return(0);
      }
   }
   return(0);
}

MOCAD_S int mocad_filter_requests(struct mocad_ctx *ctx)
{
   if (!ctx->any_time.en_capable)
   {
      struct mmp_msg_hdr *mh;

      mh = (struct mmp_msg_hdr *)ctx->sock_in;

      if (((BE32(mh->type)&0xF) == MOCA_MSG_GET) ||
          ((BE32(mh->type)&0xF) == MOCA_MSG_SET))
      {
         switch (BE32(mh->ie_type)) {
            case IE_FMR_REQUEST:
            case IE_FMR20_REQUEST:
            case IE_DD_REQUEST:
            case IE_PQOS_CREATE_REQUEST:
            case IE_PQOS_UPDATE_REQUEST:
            case IE_PQOS_DELETE_REQUEST:
            case IE_PQOS_LIST_REQUEST:
            case IE_PQOS_QUERY_REQUEST:
            case IE_PQOS_MAINTENANCE_START:
            case IE_MR_REQUEST:            
            return(1);
         default:
            return(0);
         }
      }
   }    
   return(0);
}

MOCAD_S int mocad_handle_client_req(struct mocad_ctx *ctx, MoCAOS_ClientHandle client)
{
   int ret, len;
   len = sizeof(ctx->sock_in);
   ret = MoCAOS_ReadMMP(ctx->os_handle, client, MoCAOS_TIMEOUT_INFINITE, ctx->sock_in, &len);

   if (ret <= 0)
      return(-1);
      
   /* handle requests to the daemon */
   ret = mocad_local_req(ctx, ctx->sock_in, len, ctx->sock_out,
      sizeof(ctx->sock_out));
   if (ret > 0) 
   {
      ret = MoCAOS_SendMMP(ctx->os_handle, client, ctx->sock_out, ret);

      return(ret);
   }
   /* else, send the request up to the driver */
   if(ctx->moca_running) {
      if(((ctx->link_state == LINK_STATE_DEBUG) &&
         !mocad_allow_msg_in_dbg(ctx->sock_in)) ||
         mocad_filter_requests(ctx)) 
      {
         ret = mocad_wr_abort(ctx->drv_in);
         if(ctx->verbose & L_VERBOSE)
            mocad_dump_buf(ctx, "DROP<", ctx->sock_in, len);
      } 
      else 
      {
         ret = mocad_req(ctx, ctx->sock_in, len);
         if(ret < 0) 
         {
            mocad_log(ctx, L_VERBOSE,
               "bad request from client\n");
            ret = mocad_wr_abort(ctx->drv_in);
         }
      }
   } else {
      struct mmp_msg_hdr *mh;
      uint32_t ie_type;

      mh = (struct mmp_msg_hdr *)ctx->sock_in;

      ie_type = BE32(mh->ie_type);
      mocad_log(ctx, L_VERBOSE,
         "request from client while !moca_running (IE=%04X)\n", ie_type);
      ret = mocad_wr_abort(ctx->drv_in);
   }

   ret = MoCAOS_SendMMP(ctx->os_handle, client, ctx->drv_in, ret);

   return(ret);
}

/*
 * CHAR DEVICE OPERATIONS
 */

 
#define BCM3450_PACNTL 0x18
#define BCM3450_LNACNTL  0x14

MOCAD_S void mocad_set_pm_lna(struct mocad_ctx *ctx)
{
   uint32_t val;
   int ret=0;

   if (
       (ctx->any_time.bonding == 0) &&  (ctx->any_time.tpc_en ==1) && 
       (((((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x680200B0) || ((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x680300B0)  )) ||
       (((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x742800B0) || ((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x742900B0) ||  
       ((ctx->kdrv_info.chip_id & 0xFFFFFF0) == 0x74295A0) || 
       ((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x743500B0)))
      )
   {
      ret = MoCAOS_Get3450Reg(ctx->os_handle, BCM3450_LNACNTL, (unsigned int*)&val);
      if (ret == -2) // 3450 ioctl not available
      {
         mocad_log(ctx, L_WARN, "Unable to set pm_reduce_pa, kernel ioctl not available (old bmoca version)\n");
      }
      else
      {
         val = 0x00004509;
         MoCAOS_Set3450Reg(ctx->os_handle, BCM3450_LNACNTL, val);
      }
   } 
}

MOCAD_S void mocad_set_pm_pa(struct mocad_ctx *ctx)
{
   uint32_t val;
   int ret=0;
   
   if (
       ((((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x680200B0) || ((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x680300B0)  )) ||
       (((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x742800B0) || ((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x742900B0) ||  
       ((ctx->kdrv_info.chip_id & 0xFFFFFF0) == 0x74295A0) || 
       ((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x743500B0))
      )
   {
      ret = MoCAOS_Get3450Reg(ctx->os_handle, BCM3450_PACNTL, (unsigned int*)&val);
      if (ret == -2) // 3450 ioctl not available
      {
         mocad_log(ctx, L_WARN, "Unable to set pm_reduce_pa, kernel ioctl not available (old bmoca version)\n");
      }
      else
      {
         val = (val & ~(0xF << 11)) | (0x0E << 11);
         MoCAOS_Set3450Reg(ctx->os_handle, BCM3450_PACNTL, val); 
      }
   }
}
 
#if !defined(CHIP_6818) && !defined(CHIP_63268) && !defined(STANDALONE) && !defined(WIN32)

MOCAD_S unsigned int mocad_mapphys(unsigned int addr)
{
   static unsigned int base = 0xffffffff;
   static int fd = -1;
   static int pagesize = 0, pagemask;
   static void *mapping;

   if(fd == -1)
   {
      fd = open("/dev/mem", O_RDWR | O_SYNC);

      if(fd < 0)
      {
         printf("can't open /dev/mem: %s\n", strerror(errno));
         exit(1);
      }

      pagesize = getpagesize();
      pagemask = ~(pagesize - 1);
   }

   if((addr & pagemask) == (base & pagemask))
      return((unsigned int)mapping + (addr & ~pagemask));

   if(mapping)
      munmap(mapping, pagesize);

   base = addr & pagemask;

   mapping = mmap(NULL, pagesize, (PROT_READ | PROT_WRITE),
                  MAP_SHARED, fd, base);

   if(mapping == (void *) - 1)
   {
      printf("mmap failed: %s\n", strerror(errno));
      exit(1);
   }

   return((unsigned int)mapping + (addr & ~pagemask));
}

MOCAD_S  unsigned int mocad_regr(unsigned int addr)
{
   unsigned int *x = (unsigned int *)mocad_mapphys(addr);
   return(*x);
}

MOCAD_S void mocad_regw(unsigned int addr, unsigned int val)
{
   unsigned int *x = (unsigned int *)mocad_mapphys(addr);
   *x = val;
}
#else
MOCAD_S unsigned int mocad_regr(unsigned int addr) {return 0;}
MOCAD_S void mocad_regw(unsigned int addr, unsigned int val){}



#endif	
	
#define PHY_CLOCKREG_ADDR 0xf0480170
MOCAD_S unsigned int  mocad_set_phy_clk(struct mocad_ctx *ctx, unsigned int force_phy_clk)
{
  unsigned int reg, div;
  
  div = (((3600)+((force_phy_clk)/2))/(force_phy_clk)); 
  
  reg = mocad_regr(PHY_CLOCKREG_ADDR);
  mocad_log(ctx, L_INFO, "Forced PHY clock to %d, reg=%x, div =%d\n", force_phy_clk, reg,div);
  
  reg =  (reg & ~(0xFF << 1)) |  ((div & 0xFF) << 1);  
  mocad_regw(PHY_CLOCKREG_ADDR,reg);
  
  return force_phy_clk * 1000000;  
}

MOCAD_S void mocad_set_pm(struct mocad_ctx *ctx)
{
   mocad_set_pm_pa(ctx);
   mocad_set_pm_lna(ctx);
}
 
 
MOCAD_S void mocad_start_moca(struct mocad_ctx *ctx, int fakecoreready)
{
   int restart_reason = ctx->restart;
   void *pMem;
   int i;
   int rc;
   unsigned int boot_flags = 0;
   unsigned int bcm3450rev = 0;
   unsigned int rmon_hz,rmon_vt;  
   

   ctx->restart = 0;
   ctx->moca_running = 0;
   ctx->cpu_check_sec  = 0;
   ctx->do_flags = 0;
   ctx->hostless_mode = 0;

   mocad_update_config_flags(ctx);
   if (mocad_parameter_check(ctx) != 0) {
      return;
   } 
   mocad_log(ctx, L_INFO, "Loading Moca Core image...(%d)\n", restart_reason);
   mocad_update_link_state(ctx, ctx->any_time.continuous_power_tx_mode ?
      LINK_STATE_DEBUG : LINK_STATE_DOWN);
   mocad_reset_stats(ctx); 
   mocad_remove_pqos_flows(ctx);    
   mocad_clear_mcfilter(ctx);

   MoCAOS_EnableDataIf(ctx->os_handle, ctx->ifname, 2);

   if (MoCAOS_StopCore(ctx->os_handle) != 0)
   {
      die("MoCAOS_StopCore failed");
      MoCAOS_MSleep(4000);
      RESTART(13);
   }

   ctx->any_time.moca_core_trace_enable = 0;

   ctx->cpu_clk_rate = MoCAOS_SetCpuClk(ctx->os_handle, (ctx->any_time.moca_cpu_freq * 1000000));

   if (ctx->any_time.bonding)
   {
      boot_flags |= MOCAOS_BOOT_FLAGS_BONDING_EN;
   }

   if (ctx->any_time.prop_bonding_compatibility_mode == 0)
      ctx->preconfig_seed_bonding = 0;
   else if (ctx->any_time.prop_bonding_compatibility_mode == 2)
      ctx->preconfig_seed_bonding = 1;

#ifdef STANDALONE
extern unsigned int exdebug[16];
   exdebug[0] = 0x1;
   exdebug[1] = (unsigned int)ctx->cpu_img[0];
   exdebug[2] = (unsigned int)ctx;
#endif

   if (MoCAOS_StartCore(ctx->os_handle, (unsigned char *)ctx->cpu_img[0], ctx->cpu_img_len[0], boot_flags) != 0)
   {
      die("MoCAOS_StartCore 0 failed");
      MoCAOS_MSleep(4000);
      RESTART(11);
      return;
   }

#ifdef STANDALONE
   exdebug[0] = 0x2;
   exdebug[1] = (unsigned int)ctx->cpu_img[1];
   exdebug[2] = (unsigned int)ctx;
#endif

   if (MoCAOS_StartCore(ctx->os_handle, (unsigned char *)ctx->cpu_img[1], ctx->cpu_img_len[1], boot_flags) != 0)
   {
      die("MoCAOS_StartCore 1 failed");
      MoCAOS_MSleep(4000);
      RESTART(22);
      return;
   }

   ctx->phy_clk_rate = MoCAOS_SetPhyClk(ctx->os_handle, ctx->cpu_clk_rate);

   if (((ctx->kdrv_info.chip_id & 0xFFFF00F0) == 0x339000B0) && (ctx->any_time.phy_clock!=0))
   {
		ctx->phy_clk_rate = mocad_set_phy_clk(ctx, ctx->any_time.phy_clock);
   }

   if ((ctx->phy_clk_rate == 0) || (ctx->cpu_clk_rate == 0))
   {
      mocad_log(ctx, L_WARN, "WARNING: cannot set MoCA clocks.\n");
      mocad_log(ctx, L_WARN, "  Most likely cause is an out-of-date kernel.\n");
      mocad_log(ctx, L_WARN, "  Please apply the moca_clk.patch file found in\n");
      mocad_log(ctx, L_WARN, "  the kernelpatch directory of the MoCA release.\n");
      mocad_log(ctx, L_WARN, "  Continuing...\n");
   }

   mocad_log(ctx, L_INFO, "Loading Moca Core image done.\n");

   checkPacketRam(ctx);
  
#if defined(__EMU_HOST_20__)
   /*
      if (fakecoreready)
    {
        struct moca_core_ready mcr;

        mcr.chip_type = 32;
        mcr.compatibility = 1;
        mcr.phy_freq_mhz  = 0;  // Meaningless field. The host does not need this field.   
        mcr.reserved      = 0;
        mcr.syncVersion   = 0;
        
        // The emulator doesn't send the core ready.  Assume it's ready
        mocad_handle_core_ready(ctx, &mcr);
    } */
#endif    
    
   // firmware needs some configuration before IE_CORE_READY
   for (i=0;i<500;i++)
   {
      MoCAOS_GetDriverInfo(ctx->os_handle, &ctx->kdrv_info);
      if (ctx->kdrv_info.gp1)
         break;
      MoCAOS_MSleep(1);
   }

   if (i==500)
   {
      unsigned int buf[129];

      mocad_log(ctx, L_ERR, "Timeout waiting for Firmware.\n");
      if (MoCAOS_MemAlign(&pMem, 64, (129 * sizeof(uint32_t))) < 0)
         pMem = buf;

      if ( MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)pMem, (129 * sizeof(uint32_t)), 0) == 0 )
      {
         if ((BE32(*(int *)pMem)) == 0x12345678 )
         {
            mocad_print_assert(ctx,  BE32(*(int *)((unsigned char *)pMem + 8)), (struct moca_assert *)((unsigned char *)pMem + 16));
         }
      }
      if (pMem != buf)
         MoCAOS_FreeMemAlign(pMem);
      RESTART(17);
      MoCAOS_MSleep(10000);
      return;
   }

#define BCM3450_CHIP_REV  0x04

   rc = MoCAOS_Get3450Reg(ctx->os_handle, BCM3450_CHIP_REV, &bcm3450rev);

   if (rc < 0)
      bcm3450rev = ctx->any_time.amp_type;
   else
      bcm3450rev >>= 8;

   if ((rc < 0) && (ctx->kdrv_info.hw_rev < 0x2003)) // gen3 and later chips don't have a 3450 connected to the host
   {
      mocad_log(ctx, L_WARN, "Unable to retrieve 3450 revision.\n");
   }
   
   mocad_log(ctx, L_VERBOSE, "Chip_id=%x\n", ctx->kdrv_info.chip_id);
   
   mocad_log(ctx, L_VERBOSE, "%s configured\n", (bcm3450rev==0?"3450":"3451"));
   
   mocad_set_pm(ctx);
   
   memset(&ctx->preconfig, 0, sizeof(ctx->preconfig));

   ctx->preconfig.phyclock = BE32(ctx->phy_clk_rate/1000000);
   ctx->preconfig.cpuclock = BE32(ctx->cpu_clk_rate/1000000);
   ctx->preconfig.capabilities.remote_management = ctx->any_time.remote_man;

   ctx->preconfig.capabilities.bonded = ctx->any_time.bonding ? 1 : 0; 
   ctx->preconfig.capabilities.limit_traffic = ctx->any_time.limit_traffic ? 1 : 0;
   ctx->preconfig.capabilities.bcm3450rev = bcm3450rev ? 1 : 0;
   ctx->preconfig.capabilities.useExtPram = ctx->any_time.use_ext_data_mem ? 1 : 0;
   ctx->preconfig.capabilities.brcm_seed_bonding = ctx->preconfig_seed_bonding;
   ctx->preconfig.capabilities.turbo_en = ctx->any_time.turbo_en ? 1 : 0; 
   
   ctx->preconfig.capabilities.psm_3451 = (((ctx->any_time.psm_config)) & 0x1) ? 1 : 0;
   ctx->preconfig.capabilities.psm_pll =  (((ctx->any_time.psm_config)) & 0x2) ? 1 : 0;
   ctx->preconfig.capabilities.psm_analog = (((ctx->any_time.psm_config)) & 0x4) ? 1 : 0;

   ctx->preconfig.hostMacAddress[0] = BE32(ctx->kdrv_info.macaddr_hi);
   ctx->preconfig.hostMacAddress[1] = BE32(ctx->kdrv_info.macaddr_lo);
   ctx->preconfig.chipId = BE32(ctx->kdrv_info.chip_id);
   ctx->preconfig.capabilities.reserved = ctx->any_time.init1;

   ctx->preconfig.capabilities.val32 = BE32(ctx->preconfig.capabilities.val32);

   if (MoCAOS_GetRMON(&rmon_hz, &rmon_vt)){ 
      mocad_log(ctx, L_VERBOSE, "Unable to get RMON\n");
   }
   else{
    ctx->preconfig.rmon_hz = BE32(rmon_hz); 
	ctx->preconfig.rmon_vt = BE32(rmon_vt); 
   }   
   mocad_set_random(&ctx->preconfig.random_seed[0], sizeof(ctx->preconfig.random_seed)/sizeof(ctx->preconfig.random_seed[0]));
   ctx->preconfig.valid = BE32(1);

#if !defined(__EMU_HOST_20__)
   if (0 != MoCAOS_WriteMem(ctx->os_handle, (unsigned char *)&ctx->preconfig, sizeof(ctx->preconfig), (unsigned char *)(ctx->kdrv_info.gp1 & 0x1fffffff)))
      mocad_log(ctx, L_ERR, "ERROR: Unable to update pre-config data\n");
#endif

   while (1)
   {
      MoCAOS_ClientHandle client;
  
      client = MoCAOS_WaitForRequest(ctx->os_handle, 3);

      if (client == MoCAOS_CLIENT_CORE)
         break;

      if (client == MoCAOS_CLIENT_TIMEOUT)
      {
         unsigned int buf[129];
         die("Timeout 11111 waiting for MOCA_CORE_READY trap.\n");
         if (MoCAOS_MemAlign(&pMem, 64, (129 * sizeof(uint32_t))) < 0)
            pMem = buf; 

         if ( MoCAOS_ReadMem(ctx->os_handle, (unsigned char *)pMem, (129 * sizeof(uint32_t)), 0) == 0 )
         {
            if ((BE32(*(int *)pMem)) == 0x12345678 )
            {
               mocad_print_assert(ctx,  BE32(*(int *)((unsigned char *)pMem + 8)), (struct moca_assert *)((unsigned char *)pMem + 16));
            }
         }
         if (pMem != buf)
            MoCAOS_FreeMemAlign(pMem);             
         MoCAOS_MSleep(4000);
         RESTART(12);
         return;
      }
      else if (client == MoCAOS_CLIENT_NL)
      {
         continue;
      }
      else if (client == MoCAOS_CLIENT_GMII)
      {
         /* handle these in the main loop */
         continue;
      }
      else if (client != MoCAOS_CLIENT_NULL)
      {
         mocad_handle_client_req(ctx, client);
      }
   }
}

MOCAD_S static void mocad_set_core_print_prefix(struct mocad_ctx *ctx)
{
   int i = 0;
   int len = 0;
   char * pifnum;

   // strpbrk(ctx->ifname, "0123456789");
   pifnum = ctx->ifname;

   while (*pifnum != '\0')
   {
      if ((*pifnum >= '0') && (*pifnum <= '9')) 
         break;
      else
         pifnum++;
   }
   if (!*pifnum)
      pifnum = NULL;

   if (pifnum != NULL) {
      /* Assuming that the interface name includes a number. Find the number
       * and use it to indentify which moca core is generating the traces. */
      strcpy(ctx->core_print_prefix, "CORE");

      /* Subtract 3 from length for colon, space and null character */
      len = (int)MIN(sizeof(ctx->core_print_prefix) - strlen("CORE") - 3, 
                strlen(pifnum));
      strncpy(&ctx->core_print_prefix[strlen("CORE")], pifnum, len);
      i = (int)(strlen("CORE") + len);
   }
   else {
      /* If there's no number in the interface name, just use the
       * interface name in uppercase to identify the moca core. */
      strcpy(ctx->core_print_prefix, ctx->ifname);

      for (i = 0; i < (int)strlen(ctx->core_print_prefix); i++) {
         ctx->core_print_prefix[i] = toupper((int)ctx->core_print_prefix[i]);
      }
   }

   ctx->core_print_prefix[i] = ':';
   ctx->core_print_prefix[i+1] = ' ';
   ctx->core_print_prefix[i+2] = 0x0;
}

MOCAD_S void mocad_handle_time_wrap(struct mocad_ctx * ctx)
{
   ctx->pqos_time_wrap_flags = 0;
}

// extract all images, strings, and packet ram from mocacore.bin
MOCAD_S int mocad_parse_mocacore(struct mocad_ctx *ctx,
   unsigned char *fw_img, unsigned int fw_len)
{
   unsigned int img_length;
   int core_id;
   ctx->cpu_img[0] = NULL;
   ctx->cpu_img[1] = NULL;
   ctx->cpu0strings = NULL;
   ctx->cpu1strings = NULL;

   if (fw_len < 64)
   {
       mocad_log(ctx, L_ERR, "Error, firmware image too short\n");
       return(-1);
   }

   // check the "hw_ver" field to determine the file type
   if (( BE32(*((unsigned int *)fw_img+5)) == 0x02 ) ||
       ( BE32(*((unsigned int *)fw_img)) == 0xAAAAAAAA))
   {
       // new style multi-core image, loop through the images
       while (fw_len > 0)
       {
           if (fw_len < 8)
           {
               mocad_log(ctx, L_ERR, "Error, invalid firmware image\n");
               return(-1);
           }
   
           if ((BE32(*((unsigned int *)fw_img)) == 0xAAAAAAAA))
           {
               // Strings section
               fw_img += 4;
               fw_len -= 4;
               img_length =  BE32(*((unsigned int *)fw_img));
               fw_img += 4;
               fw_len -= 4;
               core_id = BE32(*((unsigned int *)fw_img));
               fw_img += 4;
               fw_len -= 4;
               if (fw_len < img_length)
               {
                  mocad_log(ctx, L_ERR, "ERROR: strings section length (%u) exceeds file size\n", img_length);
                  return(-1);
               }
               
               if (core_id == 0)
               {
                  ctx->sizeofcpu0strings = img_length;
                  ctx->cpu0strings = fw_img;
               }
               else if (core_id == 1)
               {
                  ctx->sizeofcpu1strings = img_length;
                  ctx->cpu1strings = fw_img;
               }
               else
               {
                  mocad_log(ctx, L_ERR, "ERROR: invalid CPU id: %X\n", core_id);
                  return(-1);
               }
   
               fw_len -= img_length;
               fw_img += img_length;
           }
           else if ((BE32(*((unsigned int *)fw_img)) == 0xBBBBBBBB))
           {
               unsigned int section_length;

               // lookup table section
               fw_img += 4;
               fw_len -= 4;
               img_length =  BE32(*((unsigned int *)fw_img));
               fw_img += 4;
               fw_len -= 4;
               core_id = BE32(*((unsigned int *)fw_img));
               fw_img += 4;
               fw_len -= 4;
               section_length = BE32(*((unsigned int *)fw_img));
               fw_img += 4;
               fw_len -= 4;

               if (fw_len < img_length)
               {
                  mocad_log(ctx, L_ERR, "ERROR: lookup section length (%u) exceeds file size\n", img_length);
                  return(-1);
               }

               if (core_id == 0)
               {
                  ctx->sizeofcpu0lookup = section_length;
                  ctx->cpu0lookup = (uint32_t *)fw_img;
               }
               else if (core_id == 1)
               {
                  ctx->sizeofcpu1lookup = section_length;
                  ctx->cpu1lookup = (uint32_t *)fw_img;
               }
               else
               {
                  mocad_log(ctx, L_ERR, "ERROR: invalid CPU id: %X\n", core_id);
                  return(-1);
               }
   
               fw_len -= img_length;
               fw_img += img_length;
           }
           else if ((BE32(*((unsigned int *)fw_img)) == 0x11111111))  
           {
              unsigned int compat = 0;
              // firmware chip compatibility check
              fw_img += 4;
              fw_len -= 4;
              img_length =  BE32(*((unsigned int *)fw_img));
              fw_img += 4;
              fw_len -= 4;

              if (img_length < 4)
              {
                 mocad_log(ctx, L_ERR, "ERROR: firmware chip compatibility check failed...corrupted firmware image\n");
                 return(-1);
              }
              compat = BE32(*((unsigned int *)fw_img));

              if (((compat & 0xFFFF) != (ctx->kdrv_info.hw_rev & 0xFFFF)) && ((ctx->kdrv_info.hw_rev & 0xFFFF) != 0x2000))
              {
                 mocad_log(ctx, L_ERR, "WARNING: MoCA firmware file not compatible with current chip.\nFirmware: %X, Chip %X\n", 
                    compat, ctx->kdrv_info.hw_rev);
              }

              fw_len -= img_length; 
              fw_img += img_length;
           }
           else
           {
               // Firmware section
               if (fw_len < 64)
               {
                   mocad_log(ctx, L_ERR, "Error, invalid firmware image\n");
                   return(-1);
               }
               
               img_length = BE32(*((unsigned int *)fw_img+2));
               core_id = BE32(*((unsigned int *)fw_img+3));
   
               if ((core_id != 0) && (core_id != 1))
               {
                   mocad_log(ctx, L_ERR, "Error, invalid core_id in firmware image: %d\n", core_id);
                   return(-1);
               }
   
               if (fw_len < img_length)
               {
                   mocad_log(ctx, L_ERR, "Error, invalid firmware image, core image too short\n");
                   return(-1);
               }

               ctx->cpu_img[core_id] = fw_img;
               ctx->cpu_img_len[core_id] = (img_length + 3) & ~3;               
   
               fw_img += img_length;
               fw_len -= img_length;
           }
       }
   }
   else
   {
      mocad_log(ctx, L_ERR, "mocacore.bin is a MoCA 1.1 image, not 2.0.  Refusing to run.\n");
      return(-1);
   }

#ifdef STANDALONE
extern unsigned int exdebug[16];
   exdebug[3] = (unsigned int)ctx->cpu_img[0];
   exdebug[4] = (unsigned int)ctx->cpu_img[1];
#endif

   return(0);
}

#if defined (__EMU_HOST_20__)
void mocad_set_args(struct mocad_ctx *ctx, struct mocad_args *args)
{
   ctx->any_time.single_channel_operation = args->sc;    // Single channel
   ctx->any_time.taboo_channels.taboo_left_mask = args->tlb;   // Taboo left bitmask
   ctx->any_time.taboo_channels.taboo_right_mask = args->trb;   // Taboo tight bitmask
   ctx->any_time.listening_freq_mask = args->pfm;   // Preliminary Network Search
   ctx->any_time.lof = args->lof;   // LOF parameter - Last Operating Frequency
   ctx->any_time.tpc_en = args->tpc;   // TPC enable

   // RF type
   if (args->rf == -1)
      ctx->any_time.rf_band = MOCA_RF_BAND_EX_D;
   else
      ctx->any_time.rf_band = args->rf;

   ctx->any_time.taboo_channels.taboo_fixed_mask_start = args->tsc;   // Taboo fixed mask start
   ctx->any_time.taboo_channels.taboo_fixed_channel_mask = args->tbm;   // Taboo fixed channel bitmask 
   ctx->any_time.loopback_en = args->lpbk;  // Loopback mode enable
   ctx->any_time.nc_mode = args->nc_mode; // NC mode
   ctx->any_time.ofdma_en = args->ofdma; // ofdma enale

   /* Set the cont TX mode based on NC mode if necessary */
   switch(ctx->any_time.nc_mode)
   {
      case NODE_MODE_CONTINUOUS_TX:
         ctx->any_time.continuous_power_tx_mode = MMP_CONTINUOUS_TX_PROBE_I_MODE;

         //ctx->any_time.const_tx_params.const_tx_submode =       TCM_NORM_PROBE1;//TCM_SINGLE_TONE;
         ctx->any_time.const_tx_params.const_tx_submode = TCM_SINGLE_TONE;
         //ctx->any_time.const_tx_params.const_tx_submode =       TCM_CW;
         //ctx->any_time.const_tx_params.const_tx_submode =       TCM_BAND;

         ctx->any_time.const_tx_params.const_tx_sc1      = 11;          // The first SC tone for single tone only
         ctx->any_time.const_tx_params.const_tx_sc2      = 36;          // The second SC tone for single tone only
         ctx->any_time.bandwidth                         = BW_50;       // The second SC tone for single tone only
         ctx->any_time.const_tx_params.const_tx_band[0]  = 0 ;          // Bitmask of the valid 0  - 31  SCs 
         ctx->any_time.const_tx_params.const_tx_band[1]  = 1 ;          // Bitmask of the valid 32 - 63  SCs 
         ctx->any_time.const_tx_params.const_tx_band[2]  = 2 ;          // Bitmask of the valid 64 - 95  SCs  
         ctx->any_time.const_tx_params.const_tx_band[3]  = 3 ;          // Bitmask of the valid 96 - 127 SCs 
         ctx->any_time.const_tx_params.const_tx_band[4]  = 4 ;          // Bitmask of the valid 128- 159 SCs 
         ctx->any_time.const_tx_params.const_tx_band[5]  = 5 ;          // Bitmask of the valid 160- 191 SCs 
         ctx->any_time.const_tx_params.const_tx_band[6]  = 6 ;          // Bitmask of the valid 192- 223 SCs 
         ctx->any_time.const_tx_params.const_tx_band[7]  = 7 ;          // Bitmask of the valid 224- 255 SCs 
         ctx->any_time.const_tx_params.const_tx_band[8]  = 8 ;          // Bitmask of the valid 256- 287 SCs 
         ctx->any_time.const_tx_params.const_tx_band[9]  = 9 ;          // Bitmask of the valid 288- 319 SCs 
         ctx->any_time.const_tx_params.const_tx_band[10] = 10;          // Bitmask of the valid 320- 351 SCs  
         ctx->any_time.const_tx_params.const_tx_band[11] = 11;          // Bitmask of the valid 352- 383 SCs 
         ctx->any_time.const_tx_params.const_tx_band[12] = 12;          // Bitmask of the valid 384- 415 SCs 
         ctx->any_time.const_tx_params.const_tx_band[13] = 13;          // Bitmask of the valid 416- 447 SCs 
         ctx->any_time.const_tx_params.const_tx_band[14] = 14;          // Bitmask of the valid 448- 479 SCs 
         ctx->any_time.const_tx_params.const_tx_band[15] = 15;          // Bitmask of the valid 480- 511 SCs 
         break;
      case NODE_MODE_CONTINUOUS_RX:
         ctx->any_time.continuous_power_tx_mode = MMP_CONTINUOUS_RX_MODE;
         ctx->any_time.lof = 1150;
         ctx->any_time.continuous_rx_mode_attn = MOCA_CONTINUOUS_RX_MODE_ATTN_MIN;
         break;
      default:
         // Do nothing
         break;
   }

/***************************************/
/***************************************/
//#define TCM_NORM_PROBE1_50
//#define TCM_NORM_PROBE1_100
//#define TCM_SINGLE_TONE_50_SC1_11_SC2_36
//#define TCM_SINGLE_TONE_100_SC1_11_SC2_411
//#define TCM_BAND_50
//#define TCM_BAND_100
//#define TCM_CW_50
//#define TCM_CW_100

#ifdef TCM_NORM_PROBE1_50
   ctx->any_time.lof                                 = 1150;
   ctx->any_time.nc_mode                             = NODE_MODE_CONTINUOUS_TX;
   ctx->any_time.continuous_power_tx_mode            = MMP_CONTINUOUS_TX_PROBE_I_MODE; 
   ctx->any_time.const_tx_params.const_tx_submode    = TCM_NORM_PROBE1;
   ctx->any_time.bandwidth                           = BW_50;
#endif

#ifdef TCM_NORM_PROBE1_100
   ctx->any_time.lof                                 = 1150;
   ctx->any_time.nc_mode                             = NODE_MODE_CONTINUOUS_TX;
   ctx->any_time.continuous_power_tx_mode            = MMP_CONTINUOUS_TX_PROBE_I_MODE; 
   ctx->any_time.const_tx_params.const_tx_submode    = TCM_NORM_PROBE1;
   ctx->any_time.bandwidth                           = BW_100;
#endif

#ifdef TCM_SINGLE_TONE_50_SC1_11_SC2_36
   ctx->any_time.lof                                 = 1150;
   ctx->any_time.nc_mode                             = NODE_MODE_CONTINUOUS_TX;
   ctx->any_time.continuous_power_tx_mode            = MMP_CONTINUOUS_TX_PROBE_I_MODE; 
   ctx->any_time.const_tx_params.const_tx_submode    = TCM_SINGLE_TONE;
   ctx->any_time.bandwidth                           = BW_50;    

   ctx->any_time.const_tx_params.const_tx_sc1        = 11; // The first SC tone for single tone only
   ctx->any_time.const_tx_params.const_tx_sc2        = 36; // The second SC tone for single tone only
#endif

#ifdef TCM_SINGLE_TONE_100_SC1_11_SC2_411
   ctx->any_time.lof                                 = 1150;
   ctx->any_time.nc_mode                             = NODE_MODE_CONTINUOUS_TX;
   ctx->any_time.continuous_power_tx_mode            = MMP_CONTINUOUS_TX_PROBE_I_MODE; 
   ctx->any_time.const_tx_params.const_tx_submode    = TCM_SINGLE_TONE;
   ctx->any_time.bandwidth                           = BW_100;

   ctx->any_time.const_tx_params.const_tx_sc1        = 11;  // The first SC tone for single tone only
   ctx->any_time.const_tx_params.const_tx_sc2        = 411; // The second SC tone for single tone only
#endif

#ifdef TCM_BAND_50
   ctx->any_time.lof                                 = 1150;
   ctx->any_time.nc_mode                             = NODE_MODE_CONTINUOUS_TX;
   ctx->any_time.continuous_power_tx_mode            = MMP_CONTINUOUS_TX_PROBE_I_MODE; 
   ctx->any_time.const_tx_params.const_tx_submode    = TCM_BAND;
   ctx->any_time.bandwidth                           = BW_50;        

   ctx->any_time.const_tx_params.const_tx_band[0]      = 0 ;  // Bitmask of the valid 0  - 31  SCs 
   ctx->any_time.const_tx_params.const_tx_band[1]      = 1 ;  // Bitmask of the valid 32 - 63  SCs 
   ctx->any_time.const_tx_params.const_tx_band[2]      = 2 ;  // Bitmask of the valid 64 - 95  SCs  
   ctx->any_time.const_tx_params.const_tx_band[3]      = 3 ;  // Bitmask of the valid 96 - 127 SCs 
   ctx->any_time.const_tx_params.const_tx_band[4]      = 4 ;  // Bitmask of the valid 128- 159 SCs 
   ctx->any_time.const_tx_params.const_tx_band[5]      = 5 ;  // Bitmask of the valid 160- 191 SCs 
   ctx->any_time.const_tx_params.const_tx_band[6]      = 6 ;  // Bitmask of the valid 192- 223 SCs 
   ctx->any_time.const_tx_params.const_tx_band[7]      = 7 ;  // Bitmask of the valid 224- 255 SCs 
   ctx->any_time.const_tx_params.const_tx_band[8]      = 8 ;  // Bitmask of the valid 256- 287 SCs 
   ctx->any_time.const_tx_params.const_tx_band[9]      = 9 ;  // Bitmask of the valid 288- 319 SCs 
   ctx->any_time.const_tx_params.const_tx_band[10]     = 10;  // Bitmask of the valid 320- 351 SCs  
   ctx->any_time.const_tx_params.const_tx_band[11]     = 11;  // Bitmask of the valid 352- 383 SCs 
   ctx->any_time.const_tx_params.const_tx_band[12]     = 12;  // Bitmask of the valid 384- 415 SCs 
   ctx->any_time.const_tx_params.const_tx_band[13]     = 13;  // Bitmask of the valid 416- 447 SCs 
   ctx->any_time.const_tx_params.const_tx_band[14]     = 14;  // Bitmask of the valid 448- 479 SCs 
   ctx->any_time.const_tx_params.const_tx_band[15]     = 15;  // Bitmask of the valid 480- 511 SCs 
#endif

#ifdef TCM_BAND_100
   ctx->any_time.lof                                 = 1150;
   ctx->any_time.nc_mode                             = NODE_MODE_CONTINUOUS_TX;
   ctx->any_time.continuous_power_tx_mode            = MMP_CONTINUOUS_TX_PROBE_I_MODE; 
   ctx->any_time.const_tx_params.const_tx_submode    = TCM_BAND;
   ctx->any_time.bandwidth                           = BW_100;

   ctx->any_time.const_tx_params.const_tx_band[0]      = 0 ; // Bitmask of the valid 0  - 31  SCs 
   ctx->any_time.const_tx_params.const_tx_band[1]      = 1 ; // Bitmask of the valid 32 - 63  SCs 
   ctx->any_time.const_tx_params.const_tx_band[2]      = 2 ; // Bitmask of the valid 64 - 95  SCs  
   ctx->any_time.const_tx_params.const_tx_band[3]      = 3 ; // Bitmask of the valid 96 - 127 SCs 
   ctx->any_time.const_tx_params.const_tx_band[4]      = 4 ; // Bitmask of the valid 128- 159 SCs 
   ctx->any_time.const_tx_params.const_tx_band[5]      = 5 ; // Bitmask of the valid 160- 191 SCs 
   ctx->any_time.const_tx_params.const_tx_band[6]      = 6 ; // Bitmask of the valid 192- 223 SCs 
   ctx->any_time.const_tx_params.const_tx_band[7]      = 7 ; // Bitmask of the valid 224- 255 SCs 
   ctx->any_time.const_tx_params.const_tx_band[8]      = 8 ; // Bitmask of the valid 256- 287 SCs 
   ctx->any_time.const_tx_params.const_tx_band[9]      = 9 ; // Bitmask of the valid 288- 319 SCs 
   ctx->any_time.const_tx_params.const_tx_band[10]     = 10; // Bitmask of the valid 320- 351 SCs  
   ctx->any_time.const_tx_params.const_tx_band[11]     = 11; // Bitmask of the valid 352- 383 SCs 
   ctx->any_time.const_tx_params.const_tx_band[12]     = 12; // Bitmask of the valid 384- 415 SCs 
   ctx->any_time.const_tx_params.const_tx_band[13]     = 13; // Bitmask of the valid 416- 447 SCs 
   ctx->any_time.const_tx_params.const_tx_band[14]     = 14; // Bitmask of the valid 448- 479 SCs 
   ctx->any_time.const_tx_params.const_tx_band[15]     = 15; // Bitmask of the valid 480- 511 SCs 
#endif

#ifdef TCM_CW_50
   ctx->any_time.lof                                 = 1150;
   ctx->any_time.nc_mode                             = NODE_MODE_CONTINUOUS_TX;
   ctx->any_time.continuous_power_tx_mode            = MMP_CONTINUOUS_TX_PROBE_I_MODE; 
   ctx->any_time.const_tx_params.const_tx_submode    = TCM_CW;
   ctx->any_time.bandwidth                           = BW_50;        
#endif

#ifdef TCM_CW_100
   ctx->any_time.lof                                 = 1150;
   ctx->any_time.nc_mode                             = NODE_MODE_CONTINUOUS_TX;
   ctx->any_time.continuous_power_tx_mode            = MMP_CONTINUOUS_TX_PROBE_I_MODE; 
   ctx->any_time.const_tx_params.const_tx_submode    = TCM_CW;
   ctx->any_time.bandwidth                           = BW_100;
#endif

}
#endif

#if defined (__EMU_HOST_20__)
int mocad_main_loop(struct mocad_ctx *ctx, struct mocad_args *args)
#else
void mocad_main_loop(struct mocad_ctx *ctx)
#endif
{
   /* main loop */

   unsigned int next_pqos_expiry = 0;
   unsigned int last_stat_update = MoCAOS_GetTimeSec() + MOCA_STAT_INTERVAL;
   int len;

   while(1) {
      MoCAOS_ClientHandle client;
      unsigned int timeout_sec;
      unsigned int now;
      unsigned int timer_next;

      if(ctx->restart && (ctx->any_time.dont_start_moca != MOCA_DONT_START_MOCA_ON))
      {
         ctx->restart_history = (ctx->restart_history << 8) | (ctx->restart & 0xFF);
#if defined (__EMU_HOST_20__)
//       mocad_start_moca(ctx, args->wait);
         args->wait = 0;
#else
         mocad_start_moca(ctx, 0);
#endif
      }

      now=MoCAOS_GetTimeSec(); 

      next_pqos_expiry = mocad_expire_pqos_flows(ctx, now);
      if ((now >= (last_stat_update + MOCA_STAT_INTERVAL)) ||
          ((now <= MOCA_STAT_INTERVAL) && (last_stat_update >= (0xFFFFFFFF - MOCA_STAT_INTERVAL))))
      {
         if ((now <= MOCA_STAT_INTERVAL) && (last_stat_update >= (0xFFFFFFFF - MOCA_STAT_INTERVAL)))
         {
            mocad_handle_time_wrap(ctx);
         }

         last_stat_update = now;

         if ((ctx->show_lab_printf) && (ctx->any_time.nc_mode != 0))
         {
            mocad_log(ctx, L_WARN, "WARNING: Using non-standard nc_mode %u\n", ctx->any_time.nc_mode);
         }

         if(ctx->moca_running) {
            if (ctx->link_state != LINK_STATE_DEBUG) {
               mocad_core_wdog(ctx);
            }
            if(ctx->restart)
               continue;
         }
      }

#if defined(__EMU_HOST_20__)
      if ((ctx->moca_running) || (args->wait == 0))
#endif
      mocad_handle_deferred_traps(ctx); 

      now = MoCAOS_GetTimeSec();

      timeout_sec = (last_stat_update + MOCA_STAT_INTERVAL) - now;

      if (next_pqos_expiry &&
         (next_pqos_expiry < timeout_sec)) {
         timeout_sec = next_pqos_expiry;
      }

      mocad_timer_process(ctx, &ctx->timer_head);
      timer_next = mocad_timer_next(ctx, &ctx->timer_head);
      if (timer_next < timeout_sec)
         timeout_sec = timer_next;

      client = MoCAOS_WaitForRequest(ctx->os_handle, (unsigned int)timeout_sec);
      if ((client == MoCAOS_CLIENT_CORE) || (client == MoCAOS_CLIENT_GMII))
      {
         len = sizeof(ctx->trap_buf);
         MoCAOS_ReadMMP(ctx->os_handle, client, MoCAOS_TIMEOUT_INFINITE, ctx->trap_buf, &len);

#if defined (__EMU_HOST_20__)
         // in the emulator, we can get the CORE_READY trap before we even start the core.  Process this trap after we've restarted
         if ((!ctx->moca_running) && (args->wait))
         {
            mocad_add_trap(ctx, ctx->trap_buf, len);
         }
         else
#endif
         mocad_handle_trap(ctx, len);
      }
      else if (client == MoCAOS_CLIENT_NL)
      {
      }
      else if ((client != MoCAOS_CLIENT_NULL) && (client != MoCAOS_CLIENT_TIMEOUT))
      {         
         mocad_handle_client_req(ctx, client);         
      }

#if defined(__EMU_HOST_20__)
      if ((ctx->moca_running) || (args->wait == 0))
#endif
      mocad_handle_deferred_traps(ctx);
   }
}

#if defined (__EMU_HOST_20__)
int mocad_main(struct mocad_args *args)
#elif defined (STANDALONE)
MOCAD_S int mocad_main()
#else
int main(int argc, char **argv)
#endif
{
   const char *chardev = MOCA_DEFAULT_DEV;
   struct mocad_ctx *ctx;
   int daemon = 0, freq = 0;
   ctx = &g_ctx;
   memset(ctx, 0, sizeof(*ctx));
   ctx->deferred_start = ctx->deferred_end = ctx->deferred_traps;
   ctx->useMocadHook = 1;
   ctx->last_ps_event_code = -1;
   ctx->last_mr_events.last_cause = -1;
   ctx->last_mr_events.last_mr_result = -1;

   /* the memory transfer at the kernel layer might require 
    * 64-bit alignment */     
#ifndef STANDALONE
   {
      int i;
      for (i=0;i<MOCA_MAX_NODES;i++)
      {
         if (MoCAOS_MemAlign((void**)&ctx->cir_data[i], 64, CIR_DATA_SIZE) < 0)
         {
            mocad_log(ctx, L_ERR, "Error, failure allocating memory\n");
            return(-1);
         }
         memset(ctx->cir_data[i], 0, CIR_DATA_SIZE);
      }
   }
   ctx->exit_on_mmp_mismatch = 1;
#else
   ctx->exit_on_mmp_mismatch = 0;
#endif
 
   ctx->restart = 1;
   ctx->verbose = L_ERR | L_WARN | L_INFO;

   // Workaround MSVC 2005 "error C2059: syntax error: '{'"
#define MOCAD_INIT_STRUCT(s, t, ...) do { t __val = __VA_ARGS__; s = __val;  } while (0)
                                                                                            //   Code      Limit     Count
   MOCAD_INIT_STRUCT(ctx->bonded_seed_recovery_table[0], struct mocad_bonded_seed_recovery, {    6526,         3,        0 });
   MOCAD_INIT_STRUCT(ctx->bonded_seed_recovery_table[1], struct mocad_bonded_seed_recovery, {    6527,         3,        0 });
   MOCAD_INIT_STRUCT(ctx->bonded_seed_recovery_table[2], struct mocad_bonded_seed_recovery, {   19211,        12,        0 });
   MOCAD_INIT_STRUCT(ctx->bonded_seed_recovery_table[3], struct mocad_bonded_seed_recovery, {    9011,        12,        0 });
   MOCAD_INIT_STRUCT(ctx->bonded_seed_recovery_table[4], struct mocad_bonded_seed_recovery, {   18209,        12,        0 });
   MOCAD_INIT_STRUCT(ctx->bonded_seed_recovery_table[5], struct mocad_bonded_seed_recovery, {    6518,         5,        0 });
   MOCAD_INIT_STRUCT(ctx->bonded_seed_recovery_table[6], struct mocad_bonded_seed_recovery, {    6519,         5,        0 });
   MOCAD_INIT_STRUCT(ctx->bonded_seed_recovery_table[7], struct mocad_bonded_seed_recovery, {    6535,         5,        0 });
   MOCAD_INIT_STRUCT(ctx->bonded_seed_recovery_table[8], struct mocad_bonded_seed_recovery, {    9508,         5,        0 });
   MOCAD_INIT_STRUCT(ctx->bonded_seed_recovery_table[9], struct mocad_bonded_seed_recovery, {   15100,         3,        0 });
   // This table contains MOCAD_NUM_BONDED_SEED_RECOVERY_ERRORS entries

   moca_set_snr_margin_table_rs_defaults(&snr_def_table_rs, 0);
   moca_set_snr_margin_table_ldpc_defaults(&snr_def_table_ldpc, 0);
   moca_set_snr_margin_table_ldpc_pri_ch_defaults(&snr_def_table_ldpc_pri_ch, 0);
   moca_set_snr_margin_table_ldpc_sec_ch_defaults(&snr_def_table_ldpc_sec_ch, 0);
   moca_set_snr_margin_table_ldpc_pre5_defaults(&snr_def_table_ldpc_pre5, 0);
   moca_set_snr_margin_table_pre5_pri_ch_defaults(&snr_def_table_pre5_pri_ch, 0);
   moca_set_snr_margin_table_pre5_sec_ch_defaults(&snr_def_table_pre5_sec_ch, 0);
   
   moca_set_snr_margin_table_ofdma_defaults(&snr_def_table_ofdma, 0);


   __mocad_cmd_hook = &mocad_cmd;
   __mocad_table_cmd_hook = &mocad_get_table;

#if !defined (__EMU_HOST_20__) && !defined(STANDALONE)
{
   int vcount = 0;
   int ret;
   ctx->workdir = "/etc/moca";
   ctx->pidfile = PIDFILE_FMT;
   /* parse command line */

   while((ret = getopt(argc, argv, "xhd:wDvqPF:f:i:l:p:")) != -1) {
      switch(ret) {
         case 'x':
            ctx->exit_on_mmp_mismatch = 0;
            break;
         case 'd':
            chardev = optarg;
            break;
         case 'w':
            ctx->restart = 0;
            break;
         case 'v':
            vcount++;
            switch (vcount) {
               case 1:
                  ctx->verbose = L_ERR | L_WARN | L_INFO | L_VERBOSE;
                  break;
               case 2:
                  ctx->verbose = L_ERR | L_WARN | L_INFO | L_VERBOSE | 
                                 L_DEBUG | L_POWER_STATE;
                  break;
               case 3:
               default:
                  ctx->verbose = L_ERR | L_WARN | L_INFO | L_VERBOSE |
                                 L_DEBUG | 
                                 L_TRAP | L_MMP_MSG | L_POWER_STATE;
                  break;
            }
            break;
         case 'q':
            ctx->verbose = L_ERR | L_WARN;
            break;
         case 'P':
            ctx->show_lab_printf = 1;
            break;
         case 'f':
            ctx->fw_file = optarg;
            break;
         case 'F':
            freq = atoi(optarg);
            // keep this LOF in face of -w option unless mocad_read_lof() gives us something else
            // or in case, on this platform, that function is mute and we expect LOF only provided through -F.
            ctx->disk_lof = freq;
            break;
         case 'D':
            daemon = 1;
            break;
         case 'i':
            if (strlen(optarg) < MoCAOS_IFNAMSIZE)
               strcpy(ctx->ifname, optarg);
            break;
         case 'p':
            ctx->pidfile = optarg;
            break;
         case 'l':
            ctx->workdir = optarg;
            break;
         case 'h':
         default:
            mocad_usage(ctx);
            return(1);
      }
   }
}
#endif

#if defined (JENKINS_BUILD) || defined (DSL_MOCA)
   ctx->verbose |= L_RTT;
#endif

#if defined (__gnu_linux__)
   if (!(ctx->verbose&L_RTT))
      ctx->verbose |= L_RTT_FILE;
#endif

#if defined (__EMU_HOST_20__)
   {
      static char workdir[32];
      strcpy(workdir, (const char *)args->chip);
      ctx->workdir = workdir;
      if (args->wait)
         ctx->restart = 0;

      sprintf(ctx->ifname,"%u", args->sn+(args->cn<<12));
   }
#endif

#if defined(STANDALONE)
   {
      ctx->restart = 0;
      strcpy(ctx->ifname, "CORE1");
   }
#endif
   // This line enables core traces by default. Commented out for customer releases.
//   ctx->show_lab_printf = 1;

   ctx->os_handle = MoCAOS_Init(chardev, ctx->ifname, ctx->workdir, daemon, ctx);
   MoCAOS_GetDriverInfo(ctx->os_handle, &ctx->kdrv_info);

   moca_u32_to_mac(&ctx->any_time.mac_addr.val.addr[0], ctx->kdrv_info.macaddr_hi, ctx->kdrv_info.macaddr_lo);
   
#if !defined (__EMU_HOST_20__)
   ctx->fw_img = MoCAOS_GetFw(ctx->os_handle, (unsigned char *)ctx->fw_file,
      (int *)&ctx->fw_len);

   if ((ctx->fw_len==0) || mocad_parse_mocacore(ctx, (unsigned char *)ctx->fw_img, ctx->fw_len))
   {
      die("could not parse firmware image");
      exit(3);
   }
#endif

#if defined(DSL_MOCA)
{ 
   int ret;
   /* establish communication with CMS */
   if ((ret = cmsMsg_init(EID_MOCAD, &ctx->cmsMsgHandle)) != CMSRET_SUCCESS)
   {
      die("could not initialize msg, ret=%d", ret);
      exit(2);
   }
}
#endif
   mocad_set_core_print_prefix(ctx);
   mocad_read_lof(ctx);
   mocad_read_nondefseqnum(ctx);
   mocad_read_preconfig_seed_bonding(ctx);

   /* set default params */
   switch (ctx->kdrv_info.rf_band)
   {
      case MOCA_BAND_MIDRF:
      case MOCA_BAND_E:
         ctx->any_time.rf_band = MOCA_RF_BAND_E;
         mocad_log(ctx, L_INFO, "MidRF board detected\n");
         break;

      case MOCA_BAND_F:
         ctx->any_time.rf_band = MOCA_RF_BAND_F;
         break;

      case MOCA_BAND_D_LOW:
         ctx->any_time.rf_band = MOCA_RF_BAND_D_LOW;
         break;

      case MOCA_BAND_D_HIGH:
         ctx->any_time.rf_band = MOCA_RF_BAND_D_HIGH;
         break;

      case MOCA_BAND_WANRF:
         ctx->any_time.rf_band = MOCA_RF_BAND_C4;
         break;

      case MOCA_BAND_H:
         ctx->any_time.rf_band = MOCA_RF_BAND_H;
         break;

      case MOCA_BAND_HIGHRF:
      case MOCA_BAND_EXT_D:
      default:
#if !defined(DSL_MOCA)  
         mocad_log(ctx, L_INFO, "HiRF board detected\n");      
#endif
         ctx->any_time.rf_band = MOCA_RF_BAND_EX_D; // hi rf
         break;
   }

   ctx->any_time.turbo_en = 0;    
#if defined (__EMU_HOST_20__)
   // do this before and after mocad_restore_defaults, since there's a circular 
   // dependency on the defaults of some of the args
   mocad_set_args (ctx, args);
#endif

   if (ctx->restart)
      mocad_restore_defaults(ctx, freq, 0);
   else
      mocad_restore_defaults(ctx, freq, MOCA_DONT_START_MOCA_WAIT); // wait = 2 means mocad is initialized with -w option

   if (ctx->config_flags & MOCA_BONDING_SUPPORTED_FLAG)
      ctx->any_time.impedance_mode_bonding = MOCA_IMPEDANCE_MODE_BONDING_BONDING_SUPPORTED_DEF;
   else
      ctx->any_time.impedance_mode_bonding = MOCA_IMPEDANCE_MODE_BONDING_DEF;

   moca_set_prof_pad_ctrl_deg_6802c0_single_defaults (&ctx->any_time.prof_pad_ctrl_deg_6802c0_single, ctx->config_flags);

   ctx->any_time.rework_6802 = MOCA_REWORK_6802_DEF;

#if defined (__EMU_HOST_20__)
   mocad_set_args (ctx, args);
#endif

   /* create pidfile */
   mocad_write_pidfile(ctx);

   mocad_read_e2m(ctx);
   mocad_read_probe_2_results(ctx);
   mocad_read_rf_calib_data(ctx);

#if defined(DSL_MOCA) /* DSL Code */
   mocad_init_dsl(ctx);
#endif

   mocad_update_times(ctx, MOCA_TIME_EVENT_CORE_DOWN);
   mocad_update_times(ctx, MOCA_TIME_EVENT_LINK_DOWN);

   if (!ctx->restart || (ctx->any_time.dont_start_moca == MOCA_DONT_START_MOCA_ON))
      MoCAOS_StopCore(ctx->os_handle);
   
#ifdef MOCAD_TIMER_UNITTEST
   mocad_timer_unittest(ctx, &ctx->timer_head);
#endif
      
#if defined (__EMU_HOST_20__)
   mocad_main_loop(ctx, args);
#else
   mocad_main_loop(ctx);
#endif

   return(0);
}

// if (0)
// {
// 
//    // debug
// 
//    {
//       rtt_control_t  rtt_control_debug;
//       unsigned char  rtt_payload_debug[RTT_PAYLOAD_LENGTH];
//       uint32_t     * ptr;
// 
//       mocad_read_rtt_control(ctx, &rtt_control_debug, rtt_buffer);
//       mocad_read_rtt_payload(ctx, rtt_payload_debug , rtt_buffer);
// 
//       ptr = (uint32_t*)(&rtt_control_debug);
// 
//       mocad_log(ctx, L_ERR, "rtt_buffer[0] = 0x%x\n", (ptr[0]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[1] = 0x%x\n", (ptr[1]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[2] = 0x%x\n", (ptr[2]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[3] = 0x%x\n", (ptr[3]));
// 
//       ptr = (uint32_t*)((void*)rtt_payload_debug);
// 
//       mocad_log(ctx, L_ERR, "rtt_buffer[4] = 0x%x\n", (ptr[0]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[5] = 0x%x\n", (ptr[1]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[6] = 0x%x\n", (ptr[2]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[7] = 0x%x\n", (ptr[3]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[8] = 0x%x\n", (ptr[4]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[9] = 0x%x\n", (ptr[5]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[10] = 0x%x\n", (ptr[6]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[11] = 0x%x\n", (ptr[7]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[12] = 0x%x\n", (ptr[8]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[13] = 0x%x\n", (ptr[9]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[14] = 0x%x\n", (ptr[10]));
//       mocad_log(ctx, L_ERR, "rtt_buffer[15] = 0x%x\n", (ptr[11]));
//    }
// 
// 
//    {
//       uint32_t * pAssertData;
//       uint32_t   i;
// 
//       pAssertData = (uint32_t *)p_assert;
// 
//       for ( i = 0 ; i < sizeof(*p_assert) / sizeof(uint32_t) ; i++ )
//       { 
//          mocad_log(ctx, L_ERR, "p_assert[%d] = 0x%x\n", i, BE32(pAssertData[i]));
//       }
//    }
// 
//    {
//       mocad_read_rtt_control(ctx, &rtt_control, rtt_buffer);
// 
//       mocad_log(ctx, L_ERR, "before while\n");
//       mocad_log(ctx, L_ERR, "ownership=%d\n",(rtt_control.ownership));
//       mocad_log(ctx, L_ERR, "last=%d\n", (rtt_control.last));
//       mocad_log(ctx, L_ERR, "reserved=%d\n", (rtt_control.reserved));
//       mocad_log(ctx, L_ERR, "length=%d\n", (rtt_control.length));
//    }
// 
//    // debug
// }

// New line
