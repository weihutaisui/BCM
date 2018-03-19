/*
 * Common code for DHD command-line utility
 *
 * Copyright (C) 2017, Broadcom. All Rights Reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id: dhdu.c 696803 2017-04-28 08:22:49Z $
 */

/* For backwards compatibility, the absence of the define 'BWL_NO_FILESYSTEM_SUPPORT'
 * implies that a filesystem is supported.
 */
#if !defined(BWL_NO_FILESYSTEM_SUPPORT)
#define BWL_FILESYSTEM_SUPPORT
#endif // endif

#ifndef PROP_TXSTATUS
#define PROP_TXSTATUS
#endif // endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <typedefs.h>
#include <epivers.h>
#include <proto/ethernet.h>
#include <dhdioctl.h>
#include <sdiovar.h>
#include <bcmutils.h>
#include <bcmendian.h>
#include "dhdu.h"
#include "miniopt.h"
#include <usbrdl.h>
#include <proto/bcmip.h>
#include <hnd_debug.h>
#include <hnd_armtrap.h>
#include <hnd_cons.h>
#define IPV4_ADDR_LEN 4
#ifdef BWL_FILESYSTEM_SUPPORT
#include <sys/stat.h>
#endif // endif

#include <errno.h>

#include <trxhdr.h>
#include "ucode_download.h"
#include <d11reglist_proto.h>

#if defined(__NetBSD__) || defined(__FreeBSD__) || defined(linux) || defined(MACOSX)
#define stricmp strcasecmp
#define strnicmp strncasecmp
#elif defined(_WIN32)
#define bzero(a, b) memset(a, 0, b);
#define __FUNCTION__ "not available"
#elif	defined(BWL_STRICMP)
#define stricmp bcmstricmp
#define strnicmp bcmstrnicmp
#else /* defined (_WIN32) */
#error Needs implementation
#endif /* defined (_WIN32) */

static cmd_func_t dhd_var_void;
static cmd_func_t dhd_varint, dhd_varstr;
static cmd_func_t dhd_var_getandprintstr, dhd_var_getint, dhd_var_get;
static cmd_func_t dhd_var_setint;

static cmd_func_t dhd_version, dhd_list, dhd_msglevel;

#ifdef SDTEST
static cmd_func_t dhd_pktgen;
#endif // endif
static cmd_func_t dhd_sprom;
static cmd_func_t dhd_sdreg;
static cmd_func_t dhd_reg2args;
static cmd_func_t dhd_pcie_dmaxfer;
static cmd_func_t dhd_pcie_serdes_reg;
static cmd_func_t dhd_sd_msglevel, dhd_sd_blocksize, dhd_sd_mode, dhd_sd_reg;
static cmd_func_t dhd_dma_mode;
static cmd_func_t dhd_usb_loopback_txfer;
static cmd_func_t dhd_membytes, dhd_download, dhd_dldn,
	dhd_upload, dhd_coredump, dhd_consoledump, dhd_vars, dhd_idleclock, dhd_idletime;
static cmd_func_t dhd_logstamp, dhd_pmac, dhd_dump_mac, dhd_psvmp, dhd_dump_svmp;
#ifdef BCMSPI
static cmd_func_t dhd_spierrstats;
#endif /* BCMSPI */

static cmd_func_t dhd_hostreorder_flows;

#ifdef QMONITOR
static int dhd_qtime_thres(void *dhd, cmd_t *cmd, char **argv);
static int dhd_qtime_percent(void *dhd, cmd_t *cmd, char **argv);
#endif // endif

#ifdef PROP_TXSTATUS
static cmd_func_t dhd_proptxstatusenable;
static cmd_func_t dhd_proptxstatusmode;
static cmd_func_t dhd_proptxopt;
#endif // endif

#if defined(BCM_DHD_RUNNER)
static cmd_func_t dhd_runner_varstr;
#endif /* BCM_DHD_RUNNER */

static cmd_func_t dhd_bsscfg_int;
static int dhd_bssiovar_mkbuf(const char *iovar, int bssidx, void *param,
	int paramlen, void *bufptr, int buflen, int *perr);
int dhdu_bssiovar_setbuf(void* dhd, const char *iovar, int bssidx,
	void *param, int paramlen, void *bufptr, int buflen);
static int dhd_bssiovar_getbuf(void* dhd, const char *iovar, int bssidx,
	void *param, int paramlen, void *bufptr, int buflen);
int dhdu_bssiovar_get(void *dhd, const char *iovar, int bssidx, void *outbuf, int len);
static int dhd_bssiovar_set(void *dhd, const char *iovar, int bssidx, void *param, int paramlen);
static int dhd_bssiovar_getint(void *dhd, const char *iovar, int bssidx, int *pval);
static int dhd_bssiovar_setint(void *dhd, const char *iovar, int bssidx, int val);

static int dhd_var_getbuf(void *dhd, char *iovar, void *param, int param_len, void **bufptr);
static int dhd_var_setbuf(void *dhd, char *iovar, void *param, int param_len);

static uint dhd_iovar_mkbuf(char *name, char *data, uint datalen,
                            char *buf, uint buflen, int *perr);
static int dhd_iovar_getint(void *dhd, char *name, int *var);
static int dhd_iovar_setint(void *dhd, char *name, int var);

#if defined(BWL_FILESYSTEM_SUPPORT)
static int file_size(char *fname);
static int read_vars(char *fname, char *buf, int buf_maxlen);
#endif // endif

/* dword align allocation */
static union {
	char bufdata[DHD_IOCTL_MAXLEN];
	uint32 alignme;
} bufstruct_dhd;
static char *buf = (char*) &bufstruct_dhd.bufdata;

/* integer output format, default to signed integer */
static uint8 int_fmt;

const uint32 dump_info_ptr_ptr[] = {DUMP_INFO_PTR_PTR_LIST};

typedef struct {
	uint value;
	char *string;
} dbg_msg_t;

static int dhd_do_msglevel(void *dhd, cmd_t *cmd, char **argv, dbg_msg_t *dbg_msg);

/* Actual command table */
cmd_t dhd_cmds[] = {
	{ "cmds", dhd_list, -1, -1,
	"generate a short list of available commands"},
	{ "version", dhd_version, DHD_GET_VAR, -1,
	"get version information" },
	{ "lmtest", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set low memory test flag" },
	{ "msglevel", dhd_msglevel, DHD_GET_VAR, DHD_SET_VAR,
	"get/set message bits" },
	{ "bcmerrorstr", dhd_var_getandprintstr, DHD_GET_VAR, -1,
	"errorstring"},
	{ "wdtick", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"watchdog tick time (ms units)"},
	{ "msi_sim", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"simulate MSI interrupts for the PCIE full dongle operation"},
	{ "pcie_lpbk", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"send a loopback message on the bus"},
	{ "pcie_dmaxfer", dhd_pcie_dmaxfer, -1, DHD_SET_VAR,
	"send a dma transfer message on the bus"},
	{ "pcie_tx_lpbk", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable loopback of tx packets on the bus"},
	{ "pcie_suspend", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"suspend/resume pcie bus"},
	{ "oob_bt_reg_on", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set level of BT_REG_ON pin"},
	{ "oob_enable", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"Enable OOB DEVICE_WAKE functionality"},
	{ "pciereg", dhd_reg2args, DHD_GET_VAR, DHD_SET_VAR,
	"get/set a pciereg from the device"},
	{ "bar0secwinreg", dhd_sdreg, DHD_GET_VAR, DHD_SET_VAR,
	"g/set any backplane address  across BAR0"},
	{ "pciecorereg", dhd_reg2args, DHD_GET_VAR, DHD_SET_VAR,
	"get/set a pciecorereg from the device"},
	{ "pcieserdesreg", dhd_pcie_serdes_reg, DHD_GET_VAR, DHD_SET_VAR,
	"get/set a pcieserdesreg from the device.\n"
	"first param is the block adress. second is the register offset, third is value of reg"},
	{ "pciecfgreg", dhd_reg2args, DHD_GET_VAR, DHD_SET_VAR,
	"get/set a pciecfgreg from the device"},
	{ "aspm", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set ASPM values" },
	{ "sleep_allowed", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"boolean to indicate whether the device is allowed to go to deep sleep"},
	{ "cc_nvmshadow", dhd_varstr, DHD_GET_VAR, -1,
	"dump OTP/SPROM shadow in chip common enum space"},
	{ "dump_ringupdblk", dhd_varstr, DHD_GET_VAR, -1,
	"dump ring update block in dhd memory"},
	{"dma_ring_indices", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"allow dmaing write/read indices from dongle to host memory"},
	{ "intr", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"use interrupts on the bus"},
	{ "pollrate", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"number of ticks between bus polls (0 means no polling)"},
	{ "idletime", dhd_idletime, DHD_GET_VAR, DHD_SET_VAR,
	"number of ticks for activity timeout (-1: immediate, 0: never)"},
	{ "idleclock", dhd_idleclock, DHD_GET_VAR, DHD_SET_VAR,
	"idleclock active | stopped | <N>\n"
	"\tactive (0)   - do not request any change to the SD clock\n"
	"\tstopped (-1) - request SD clock be stopped on activity timeout\n"
	"\t<N> (other)  - an sd_divisor value to request on activity timeout\n"},
	{ "sd1idle", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"change mode to SD1 when turning off clock at idle"},
	{ "forceeven", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"force SD tx/rx buffers to be even"},
	{ "readahead", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"enable readahead feature (look for next frame len in headers)"},
	{ "sdrxchain", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"enable packet chains to SDIO stack for glom receive"},
	{ "alignctl", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"align control frames"},
	{ "sdalign", dhd_varint, DHD_GET_VAR, -1,
	"display the (compiled in) alignment target for sd requests"},
	{ "txbound", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set maximum number of tx frames per scheduling"},
	{ "rxbound", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set maximum number of rx frames per scheduling"},
	{ "txminmax", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set maximum number of tx frames per scheduling while rx frames outstanding"},
	{ "dconpoll", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"g/set dongle console polling interval (ms)"},
	{ "dump", dhd_varstr, DHD_GET_VAR, -1,
	"dump information"},
	{ "cons", dhd_varstr, -1, DHD_SET_VAR,
	"send string to device console (sd only)"},
	{ "clearcounts", dhd_var_void, -1, DHD_SET_VAR,
	"reset the bus stats shown in the dhd dump"},
	{ "logdump", dhd_varstr, DHD_GET_VAR, -1,
	"dump the timestamp logging buffer"},
	{ "logcal", dhd_varint, -1, DHD_SET_VAR,
	"logcal <n>  -- log around an osl_delay of <n> usecs"},
	{ "logstamp", dhd_logstamp, -1, DHD_SET_VAR,
	"logstamp [<n1>] [<n2>]  -- add a message to the log"},
	{ "ramstart", dhd_varint, DHD_GET_VAR, -1,
	"display start address of onchip SOCRAM"},
	{ "ramsize", dhd_varint, DHD_GET_VAR, -1,
	"display size of onchip SOCRAM"},
	{ "usb_lb_txfer", dhd_usb_loopback_txfer, -1, DHD_SET_VAR,
	"usb_lb_txfer <cnt> <size> send cnt usb looback packets of size equal to len on the bus"},
	{ "membytes", dhd_membytes, DHD_GET_VAR, DHD_SET_VAR,
	"membytes [-h | -r | -i] <address> <length> [<data>]\n"
	"\tread or write data in the dongle ram\n"
	"\t-h   <data> is a sequence of hex digits rather than a char string\n"
	"\t-r   output binary to stdout rather hex\n"},
	{ "buzzz_dump", dhd_varint, DHD_GET_VAR, -1, "buzzz_dump\n"},
	{ "download", dhd_download, -1, DHD_SET_VAR,
	"download [-a <address>] [--noreset] [--norun] [--verify] <binfile> [<varsfile>]\n"
	"\tdownload file to specified dongle ram address and start CPU\n"
	"\toptional vars file will replace vars parsed from the CIS\n"
	"\t--noreset    do not reset SOCRAM core before download\n"
	"\t--norun      do not start dongle CPU after download\n"
	"\t--verify     do readback verify \n"
	"\tdefault <address> is 0\n"},
	{ "dldn", dhd_dldn, -1, DHD_SET_VAR,
	"download <binfile>\n"
	"\tdownload file to specified dongle ram address 0\n"},
	{ "vars", dhd_vars, DHD_GET_VAR, DHD_SET_VAR,
	"vars [<file>]\n"
	"\toverride SPROM vars with <file> (before download)\n"},
	{ "coredump", dhd_coredump, -1, -1,
	"coredump <file>\n"
	"\tdump dongle RAM content into a file in dumpfile format\n"
	"\tfor use with ELF core generator"},
	{ "consoledump", dhd_consoledump, -1, -1,
	"consoledump\n"
	"\tdump dongle debug console buffer"},
	{ "upload", dhd_upload, -1, -1,
	"upload [-a <address> ] [-s <size> ] [-r <idx>] <file>\n"
	"\tupload dongle RAM and optionally register contents (-r) into file(s)\n"
	"\tdefault <address> is 0, default <size> is RAM size"},
	{ "srdump", dhd_sprom, DHD_GET_VAR, -1,
	"display SPROM content" },
	{ "srwrite", dhd_sprom, -1, DHD_SET_VAR,
	"write data or file content to SPROM\n"
	"\tsrwrite <word-offset> <word-value> ...\n"
	"\tsrwrite [-c] <srom-file-path>\n"
	"\t  -c means write regardless of crc"},
	{ "sleep", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"enter/exit simulated host sleep (bus powerdown w/OOB wakeup)"},
	{ "kso", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"keep sdio on"},
	{ "devcap", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"brcm device capabilities"},
	{ "devsleep", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"Sleep CMD14"},
#ifdef SDTEST
	{ "extloop", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"external loopback: convert all tx data to echo test frames"},
	{ "pktgen", dhd_pktgen, DHD_GET_VAR, DHD_SET_VAR,
	"configure/report pktgen status (SDIO)\n"
	"\t-f N     frequency: send/recv a burst every N ticks\n"
	"\t-c N     count: send/recv N packets each burst\n"
	"\t-t N     total: stop after a total of N packets\n"
	"\t-p N     print: display counts on console every N bursts\n"
	"\t-m N     min: set minimum length of packet data\n"
	"\t-M N     Max: set maximum length of packet data\n"
	"\t-l N     len: set fixed length of packet data\n"
	"\t-s N     stop after N tx failures\n"
	"\t-d dir   test direction/type:\n"
	"\t            send -- send packets discarded by dongle\n"
	"\t            echo -- send packets to be echoed by dongle\n"
	"\t            burst -- request bursts (of size <-c>) from dongle\n"
	"\t              one every <-f> ticks, until <-t> total requests\n"
	"\t            recv -- request dongle enter continuous send mode,\n"
	"\t              read up to <-c> pkts every <-f> ticks until <-t>\n"
	"\t              total reads\n"},
#endif /* SDTEST */
	{ "dngl_isolation", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"g/set dongle isolation, so the dev could be disabled with out effecting the dongle state"},
	{ "ltrsleep_on_unload", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"g/set sending ltr sleep message on unload config"},
	{ "sdreg", dhd_sdreg, DHD_GET_VAR, DHD_SET_VAR,
	"g/set sdpcmdev core register (f1) across SDIO (CMD53)"},
	{ "sbreg", dhd_sdreg, DHD_GET_VAR, DHD_SET_VAR,
	"g/set any backplane core register (f1) across SDIO (CMD53)"},
	{ "sd_cis", dhd_var_getandprintstr, DHD_GET_VAR, -1,
	"dump sdio CIS"},
	{ "sd_devreg", dhd_sd_reg, DHD_GET_VAR, DHD_SET_VAR,
	"g/set device register across SDIO bus (CMD52)"},
	{ "sd_hostreg", dhd_sd_reg, DHD_GET_VAR, DHD_SET_VAR,
	"g/set local controller register"},
	{ "sd_blocksize", dhd_sd_blocksize, DHD_GET_VAR, DHD_SET_VAR,
	"g/set block size for a function"},
	{ "sd_blockmode", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"g/set blockmode"},
	{ "sd_ints", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"g/set client ints"},
	{ "sd_dma", dhd_dma_mode, DHD_GET_VAR, DHD_SET_VAR,
	"g/set dma usage: [PIO | SDMA | ADMA1 | ADMA2]"},
	{ "sd_yieldcpu", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"allow blocking (yield of CPU) on data xfer"},
	{ "sd_minyield", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"minimum xfer size to allow CPU yield"},
	{ "sd_forcerb", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"force readback when changing local interrupt settings"},
	{ "sd_numints", dhd_varint, DHD_GET_VAR, -1,
	"number of device interrupts"},
	{ "sd_numlocalints", dhd_varint, DHD_GET_VAR, -1,
	"number of non-device interrupts"},
	{ "sd_divisor", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"set the divisor for SDIO clock generation"},
	{ "sd_power", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"set the SD Card slot power"},
	{ "sd_power_save", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"set the SDIO3.0 power save value"},
	{ "sd_clock", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"turn on/off the SD Clock"},
	{ "sd_crc", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"turn on/off CRC checking in SPI mode"},
	{ "sd_mode", dhd_sd_mode, DHD_GET_VAR, DHD_SET_VAR,
	"g/set SDIO bus mode (spi, sd1, sd4)"},
	{ "sd_highspeed", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"set the high-speed clocking mode"},
	{ "sd_msglevel", dhd_sd_msglevel, DHD_GET_VAR, DHD_SET_VAR,
	"g/set debug message level"},
	{ "sd_hciregs", dhd_varstr, DHD_GET_VAR, -1,
	"display host-controller interrupt registers"},
	{ "sdiod_drive", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"SDIO Device drive strength in milliamps. (0=tri-state, 1-12mA)"},
#ifdef BCMSPI
	{ "spi_errstats", dhd_spierrstats, DHD_GET_VAR, DHD_SET_VAR,
	"SPI device status error statistics."},
	{ "spi_respdelay", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"g/set response delay flag."},
#endif /* BCMSPI */
	{ "devreset", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"Move device into or out of reset state (1/reset, or 0/operational)"},
	{ "pcie_device_trap", dhd_var_void, -1, DHD_SET_VAR,
	"trap the pcie device via mailbox mechanism"},
	{ "ioctl_timeout", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"IOCTL response timeout (milliseconds)."},
#ifdef QMONITOR
	{ "qtime_percent", dhd_qtime_percent, DHD_GET_VAR, -1,
	"qtime_percent gives the percentage of time where DHD TX queue\n"
	"has a number of packets queued above a threshold set by\n"
	"the iovar qtimer_thres"},
	{ "qtime_thres", dhd_qtime_thres, DHD_GET_VAR, DHD_SET_VAR,
	"qtime_thres sets the thresold of number of packets used\n"
	"by the iovar qtime_percent"},
#endif // endif
#ifdef PROP_TXSTATUS
	{ "proptx", dhd_proptxstatusenable, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable the proptxtstatus feature\n"
	"0 - disabled\n"
	"1 - enabled\n"},
	{ "ptxmode", dhd_proptxstatusmode, DHD_GET_VAR, DHD_SET_VAR,
	"set the proptxtstatus operation mode:\n"
	"0 - Unsupported\n"
	"1 - Use implied credit from a packet status\n"
	"2 - Use explicit credit\n" },
	{ "proptx_opt", dhd_proptxopt, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable proptxtstatus optimizations to increase throughput:\n"
	"0 - Unsupported\n"
	"1 - Enable proptxstatus optimizations to increase throughput\n" },
	{ "pmodule_ignore", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"use/bypass the proptxtstatus module\n"
	"0 - apply\n"
	"1 - bypass\n"},
	{ "pcredit_ignore", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"apply/ignore the proptxtstatus credit control\n"
	"0 - apply\n"
	"1 - ignore\n"},
	{ "ptxstatus_ignore", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"process/ignore the proptxtstatus txstaus\n"
	"0 - process\n"
	"1 - ignore\n"},
	{ "rxpkt_chk", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"check/ignore rx packet for multiple AC detection\n"
	"0 - ignore\n"
	"1 - check\n"},
#endif /* PROP_TXSTATUS */
	{ "sd_uhsimode", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"g/set UHSI Mode"},
	{ "host_reorder_flows", dhd_hostreorder_flows, DHD_GET_VAR, -1,
	"get host reorder flows "},
	{ "txglomsize", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"max glom size for sdio tx\n"},
	{ "txp_thresh", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"max txp glom to flush to device on PCIE Tx\n"},
	{ "txglommode", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"glom mode for sdio tx 0- copy, 1- multidescriptor\n"},
	{ "fw_hang_report", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable report firmware hangs for firmware reload\n"
	"0 - disabled (for testing)\n"
	"1 - enabled (default)\n"},
	{ "tuning_mode", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set tuning mode \n"
	"1 - Disabled periodic tuning \n"
	"0 - Enable periodic tuning \n"},
	{ "tcpack_suppress", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"Turn tcpack_suppress on/off\n"
	"0 - off\n"
	"1 - on\n"},
	{ "txinrx_thres", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set threshold for num of in-queued pkts to tx in rx process\n"
	"0 - always send out in rx process\n"
	"num - amount of in-queued pkts\n"},
	{"db1_for_mb", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set doorbell1 option for Mail box interrupt, for PCIE FD"
	"0 - use default config write for mb interrupt\n"
	"1 - use enumeration doorbell1 write for mb interrupt\n"
	},
	{"flow_prio_map", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"0 - Default AC priority map\n"
	"1 - TID based priority map\n"
	},
	{ "op_mode", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"\nget/set operation mode when cfg80211 is used\n"
	"1- STA Only\n"
	"2- SOFT AP Only\n"
	"3- P2P Only\n"
	"4- STA + P2P\n"
	},
	{"assert_type", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"set/get the asset_bypass flag\n"
	"assert_type <1/0> (On/Off)\n"
	},
	{"wowl_wakeind", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get wowl wake reason\n"
	"WAKE_EVENT_NLO_DISCOVERY_BIT\t\t\t0x0001\n"
	"WAKE_EVENT_AP_ASSOCIATION_LOST_BIT\t\t0x0002\n"
	"WAKE_EVENT_GTK_HANDSHAKE_ERROR_BIT\t\t0x0004\n"
	"WAKE_EVENT_4WAY_HANDSHAKE_REQUEST_BIT\t\t0x0008\n"
	"WAKE_EVENT_NET_PAKCET_BIT\t\t\t0x0010\n"
	"UNSPECIFIED (dhd not the wake reason)\t\t0x0000\n"
	},
	{ "wmf_bss_enable", dhd_bsscfg_int, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable wmf features" },
	{ "wmf_psta_disable", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"disable/enable wmf for proxy sta" },
	{ "wmf_ucast_igmp", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable wmf unicast igmp features" },
	{ "wmf_mcast_data_sendup", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable wmf multicast data sendup" },
	{ "wmf_ucast_igmp_query", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable wmf unicast igmp query" },
	{ "wmf_ucast_upnp", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable wmf unicast upnp features" },
	{ "ap_isolate", dhd_bsscfg_int, DHD_GET_VAR, DHD_SET_VAR,
	"set/get AP isolation" },
	{ "proxy_arp", dhd_bsscfg_int, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable proxy arp features" },
	{ "dhcp_unicast", dhd_bsscfg_int, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable dhcp unicast features" },
	{ "block_ping", dhd_bsscfg_int, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable block ping features" },
	{ "grat_arp", dhd_bsscfg_int, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable grat arp features" },
	{ "block_tdls", dhd_bsscfg_int, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable block_tdls feature" },
	{ "dev_def", dhd_bsscfg_int, -1, DHD_SET_VAR,
	"set/get dev_def" },
	{ "pmac", dhd_pmac, -1, -1,
	"Get mac obj values such as of SHM and IHR\n"
	"\tusage: dhd -i IF pmac <type> <address> [-s step_size] [-n num] [-b bitmap] [-v]\n"
	"type: register type"
	"address: register address"
	"step_size: 0 or 2 or 4\n"
	"num : how many to print\n"
	"bitmap : 32-bit value\n"
	"-v: verbose mode"},
	{ "dump_mac", dhd_dump_mac, DHD_GET_VAR, -1,
	"dump mac registers in the list\n"
	"\tusage: dhd -i IF dump_mac [-x] [-v] [-k]\n"
	"-x: Dump psmx regs"
	"-v: verbose mode"
	"-k: Kill ARM processor before start dumping"},
	{ "psvmp", dhd_psvmp, -1, -1,
	"Get svmp mem values\n"
	"\tusage: dhd -i IF psvmp [-n num]\n"
	"num : how many to print\n"
	"-v: verbose mode"},
	{ "dump_svmp", dhd_dump_svmp, DHD_GET_VAR, -1,
	"dump svmp mems in the list\n"
	"\tusage: dhd -i IF dump_svmp [-v] [-k]\n"
	"-v: verbose mode"
	"-k: Kill ARM processor before start dumping"},
	{ "mcast_regen_bss_enable", dhd_bsscfg_int, DHD_GET_VAR, DHD_SET_VAR,
	"enable/disable per bss unicast to multicast pkt conversion feature" },
	{"lbr_aggr_en_mask", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set tid bit mask for low bit rate aggregation b'0: disable b'1: enable " },
	{"lbr_aggr_len", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set number of packets to aggregate" },
	{"lbr_aggr_release_timeout", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"get/set low bit rate aggregation time window to release aggregates" },
#ifdef DSLCPE
#if defined(PKTC)
	{ "pktc", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"Enable/Disable PKTC\n"
	"0 - disable\n"
	"1 - enable\n"},
	{ "pktcbnd", dhd_varint, DHD_GET_VAR, DHD_SET_VAR,
	"RX PKTC bound\n"},
#endif /* PKTC */
#endif /* DSLCPE */
#if defined(BCM_DHD_RUNNER)
	{ "rnr_flowring_profile", dhd_runner_varstr, DHD_GET_VAR, DHD_SET_VAR,
	"Get/Set DHD Runner Flowring profile\n"
	"\tUsage: dhd -i IF rnr_flowring_profile -id <id> -ac_bk <weight:size> -ac_be <weight:size> -ac_vi <weight:size> -ac_vo <weight:size> -bc_mc <weight:size>\n"
	"\t<id>            0-2 user profile per radio\n"
	"\t                3-7 Built-in profiles\n"
	"\t<weight>        priority to allocate reserved memory\n"
	"\t                -1: Allocate all ring items\n"
	"\t                0: do not allocate ring items\n"
	"\t                1-n: allocate n items (<size) per round robin allocation\n"
	"\t<size>          numer of items in the flow ring\n"
	"\n"
	"NOTE: Set needs DHD driver unload or system reboot\n"},
	{ "rnr_flowring_policy", dhd_runner_varstr, DHD_GET_VAR, DHD_SET_VAR,
	"Get/Set DHD Runner Flowring selection policy for HW or SW\n"
	"\tUsage: dhd -i IF rnr_flowring_policy -<policy> <setting>\n"
	"\t<policy>        policy name (global, intfidx, clients, aclist, maclist, dot11ac)\n"
	"\t<setting>       setting based on the policy\n"
	"\t                <policy>    <setting>\n"
	"\t                 global     <0/1>\n"
	"\t                 intfidx    <interface index>\n"
	"\t                 clients    <number of clients>\n"
	"\t                 aclist     ac_bk:<0/1> ac_be:<0/1> ac_vi:<0/1> ac_vo:<0/1> bc_mc:<0/1>\n"
	"\t                 maclist    <xx-xx-xx-xx-xx-xx>\n"
	"\t                 dot11ac    <0/1>\n"
	"\n"
	"NOTE: Set needs DHD driver unload or system reboot\n"},
	{ "rnr_rxoffload", dhd_runner_varstr, DHD_GET_VAR, DHD_SET_VAR,
	"Get/Set DHD Runner Rx Offload\n"
	"\tUsage: dhd -i IF rnr_rxoffload <1/0>\n"
	"\t  0    disable Rx Offload acceleration\n"
	"\t  1    Enable Rx Offload acceleration\n"
	"\n"
	"NOTE: Set needs DHD driver unload or system reboot\n"},
#endif /* BCM_DHD_RUNNER */
	{ "halt_dongle", dhd_varint, 0, DHD_SET_VAR,
	"halts ARM and PSMs." },
	{ "cap", dhd_varstr, DHD_GET_VAR, -1, "Gets dongle capabilities" },
	{ NULL, NULL, 0, 0, NULL }
};

cmd_t dhd_varcmd = {"var", dhd_varint, -1, -1, "unrecognized name, type -h for help"};
char *dhdu_av0;

#if defined(BWL_FILESYSTEM_SUPPORT)
static int
file_size(char *fname)
{
	FILE *fp;
	long size = -1;

	/* Can't use stat() because of Win CE */

	if ((fp = fopen(fname, "rb")) == NULL ||
	    fseek(fp, 0, SEEK_END) < 0 ||
	    (size = ftell(fp)) < 0)
		fprintf(stderr, "Could not determine size of %s: %s\n",
		        fname, strerror(errno));

	if (fp != NULL)
		fclose(fp);

	return (int)size;
}
#endif   /* BWL_FILESYSTEM_SUPPORT */

/* parse/validate the command line arguments */
/*
* pargv is updated upon return if the first argument is an option.
 * It remains intact otherwise.
 */
int
dhd_option(char ***pargv, char **pifname, int *phelp)
{
	char *ifname = NULL;
	int help = FALSE;
	int status = CMD_OPT;
	char **argv = *pargv;

	int_fmt = INT_FMT_DEC;

	while (*argv) {
		/* select different adapter */
		if (!strcmp(*argv, "-a") || !strcmp(*argv, "-i")) {
			char *opt = *argv++;
			ifname = *argv;
			if (!ifname) {
				fprintf(stderr,
					"error: expected interface name after option %s\n", opt);
				status = CMD_ERR;
				break;
			}
		}

		/* integer output format */
		else if (!strcmp(*argv, "-d"))
			int_fmt = INT_FMT_DEC;
		else if (!strcmp(*argv, "-u"))
			int_fmt = INT_FMT_UINT;
		else if (!strcmp(*argv, "-x"))
			int_fmt = INT_FMT_HEX;

		/* command usage */
		else if (!strcmp(*argv, "-h"))
			help = TRUE;

		/* done with generic options */
		else {
			status = CMD_DHD;
			break;
		}

		/* consume the argument */
		argv ++;
		break;
	}

	*phelp = help;
	*pifname = ifname;
	*pargv = argv;

	return status;
}

void
dhd_cmd_usage(cmd_t *cmd)
{
	if (strlen(cmd->name) >= 8)
		fprintf(stderr, "%s\n\t%s\n\n", cmd->name, cmd->help);
	else
		fprintf(stderr, "%s\t%s\n\n", cmd->name, cmd->help);
}

/* Dump out short list of commands */
static int
dhd_list(void *dhd, cmd_t *garb, char **argv)
{
	cmd_t *cmd;
	int nrows, i, len;
	char *buf;
	int letter, col, row, pad;

	UNUSED_PARAMETER(dhd);
	UNUSED_PARAMETER(garb);
	UNUSED_PARAMETER(argv);

	for (cmd = dhd_cmds, nrows = 0; cmd->name; cmd++)
		    nrows++;

	nrows /= 4;
	nrows++;

	len = nrows * 80 + 2;
	buf = malloc(len);
	if (buf == NULL) {
		fprintf(stderr, "Failed to allocate buffer of %d bytes\n", len);
		return BCME_NOMEM;
	}
	for (i = 0; i < len; i++)
		*(buf+i) = 0;

	row = col = 0;
	for (letter = 'a'; letter < 'z'; letter++) {
		for (cmd = dhd_cmds; cmd->name; cmd++) {
			if (cmd->name[0] == letter || cmd->name[0] == letter - 0x20) {
				strcat(buf+row*80, cmd->name);
				pad = 18 * (col + 1) - (int)strlen(buf+row*80);
				if (pad < 1)
					pad = 1;
				for (; pad; pad--)
					strcat(buf+row*80, " ");
				row++;
				if (row == nrows) {
					col++; row = 0;
				}
			}
		}
	}
	for (row = 0; row < nrows; row++)
		printf("%s\n", buf+row*80);

	printf("\n");
	free(buf);
	return (0);
}

void
dhd_cmds_usage(cmd_t *port_cmds)
{
	cmd_t *port_cmd;
	cmd_t *cmd;

	/* print usage of port commands */
	for (port_cmd = port_cmds; port_cmd && port_cmd->name; port_cmd++)
		/* Check for wc_cmd */
		dhd_cmd_usage(port_cmd);

	/* print usage of common commands without port counterparts */
	for (cmd = dhd_cmds; cmd->name; cmd++) {
		/* search if port counterpart exists */
		for (port_cmd = port_cmds; port_cmd && port_cmd->name; port_cmd++)
			if (!strcmp(port_cmd->name, cmd->name))
				break;
		if (!port_cmd || !port_cmd->name)
			dhd_cmd_usage(cmd);
	}
}

void
dhd_usage(cmd_t *port_cmds)
{
	fprintf(stderr,
	        "Usage: %s [-a|i <adapter>] [-h] [-d|u|x] <command> [arguments]\n",
		dhdu_av0);

	fprintf(stderr, "\n");
	fprintf(stderr, "  -h		this message\n");
	fprintf(stderr, "  -a, -i	adapter name or number\n");
	fprintf(stderr, "  -d		display values as signed integer\n");
	fprintf(stderr, "  -u		display values as unsigned integer\n");
	fprintf(stderr, "  -x		display values as hexdecimal\n");
	fprintf(stderr, "\n");

	dhd_cmds_usage(port_cmds);
}

int
dhd_check(void *dhd)
{
	int ret;
	int val;

	if ((ret = dhd_get(dhd, DHD_GET_MAGIC, &val, sizeof(int))) < 0)
		return ret;
	if (val != DHD_IOCTL_MAGIC)
		return -1;
	if ((ret = dhd_get(dhd, DHD_GET_VERSION, &val, sizeof(int))) < 0)
		return ret;
	if (val > DHD_IOCTL_VERSION) {
		fprintf(stderr, "Version mismatch, please upgrade\n");
		return -1;
	}
	return 0;
}

void
dhd_printint(int val)
{
	switch (int_fmt) {
	case INT_FMT_UINT:
		printf("%u\n", val);
		break;
	case INT_FMT_HEX:
		printf("0x%x\n", val);
		break;
	case INT_FMT_DEC:
	default:
		printf("%d\n", val);
		break;
	}
}

/* pretty hex print a contiguous buffer (tweaked from wlu) */
void
dhd_hexdump(uchar *buf, uint nbytes, uint saddr)
{
	char line[256];
	char* p;
	uint i;

	if (nbytes == 0) {
		printf("\n");
		return;
	}

	p = line;
	for (i = 0; i < nbytes; i++) {
		if (i % 16 == 0) {
			p += sprintf(p, "%08x: ", saddr + i);	/* line prefix */
		}
		p += sprintf(p, "%02x ", buf[i]);
		if (i % 16 == 15) {
			uint j;
			p += sprintf(p, "  ");
			for (j = i-15; j <= i; j++)
				p += sprintf(p, "%c",
				             ((buf[j] >= 0x20 && buf[j] <= 0x7f) ? buf[j] : '.'));
			printf("%s\n", line);		/* flush line */
			p = line;
		}
	}

	/* flush last partial line */
	if (p != line)
		printf("%s\n", line);
}

/*
*
* Set or Get a BSS Configuration idexed integer
*/
static int
dhd_bsscfg_int(void *dhd, cmd_t *cmd, char **argv)
{
	char *endptr = NULL;
	char *val_name;
	int bsscfg_idx = 0;
	int val = 0;
	int ret;

	val_name = *argv++;

	if (GET_BSSCFG_IDX(dhd, &bsscfg_idx) != BCME_OK) {
		bsscfg_idx = 0;
	}

	if (!*argv) {
		/* This is a GET */
		if (cmd->get == -1) {
			return -1;
		}

		ret = dhd_bssiovar_getint(dhd, val_name, bsscfg_idx, &val);

		if (ret < 0)
			return ret;

		dhd_printint(val);
	} else {
		/* This is a SET */
		if (cmd->set == -1) {
			return -1;
		}

		if (!stricmp(*argv, "on")) {
			val = 1;
		} else if (!stricmp(*argv, "off")) {
			val = 0;
		} else {
			val = strtol(*argv, &endptr, 0);
			if (*endptr != '\0') {
				/* not all the value string was parsed by strtol */
				return BCME_USAGE_ERROR;
			}
		}

		ret = dhd_bssiovar_setint(dhd, val_name, bsscfg_idx, val);
	}

	return ret;
}

#ifdef SDTEST
static int
dhd_pktgen(void *dhd, cmd_t *cmd, char **argv)
{
	int ret = 0;
	void *ptr = NULL;
	dhd_pktgen_t pktgen;
	char *str;

	UNUSED_PARAMETER(dhd);
	UNUSED_PARAMETER(cmd);

	/* Get current settings */
	if ((ret = dhd_var_getbuf(dhd, "pktgen", NULL, 0, &ptr)) != 0)
		return ret;
	memcpy(&pktgen, ptr, sizeof(pktgen));

	if (pktgen.version != DHD_PKTGEN_VERSION) {
		fprintf(stderr, "pktgen version mismatch (module %d app %d)\n",
		        pktgen.version, DHD_PKTGEN_VERSION);
		return BCME_ERROR;
	}

	/* Presence of args implies a set, else a get */
	if (*++argv) {
		miniopt_t opts;
		int opt_err;

		/* Initialize option parser */
		miniopt_init(&opts, "pktgen", "", FALSE);

		while ((opt_err = miniopt(&opts, argv)) != -1) {
			if (opt_err == 1) {
				fprintf(stderr, "pktgen options error\n");
				ret = -1;
				goto exit;
			}
			argv += opts.consumed;

			if (!opts.good_int && opts.opt != 'd') {
				fprintf(stderr, "invalid integer %s\n", opts.valstr);
				ret = -1;
				goto exit;
			}

			switch (opts.opt) {
			case 'f':
				pktgen.freq = opts.uval;
				break;
			case 'c':
				pktgen.count = opts.uval;
				break;
			case 'p':
				pktgen.print = opts.uval;
				break;
			case 't':
				pktgen.total = opts.uval;
				break;
			case 's':
				pktgen.stop = opts.uval;
				break;
			case 'm':
				pktgen.minlen = opts.uval;
				break;
			case 'M':
				pktgen.maxlen = opts.uval;
				break;
			case 'l': case 'L':
				pktgen.minlen = pktgen.maxlen = opts.uval;
				break;
			case 'd':
				if (!strcmp(opts.valstr, "send"))
					pktgen.mode = DHD_PKTGEN_SEND;
				else if (!strcmp(opts.valstr, "echo"))
					pktgen.mode = DHD_PKTGEN_ECHO;
				else if (!strcmp(opts.valstr, "burst"))
					pktgen.mode = DHD_PKTGEN_RXBURST;
				else if (!strcmp(opts.valstr, "recv"))
					pktgen.mode = DHD_PKTGEN_RECV;
				else {
					fprintf(stderr, "unrecognized dir mode %s\n",
					        opts.valstr);
					return BCME_USAGE_ERROR;
				}
				break;

			default:
				fprintf(stderr, "option parsing error (key %s valstr %s)\n",
				        opts.key, opts.valstr);
				ret = BCME_USAGE_ERROR;
				goto exit;
			}
		}

		if (pktgen.maxlen < pktgen.minlen) {
			fprintf(stderr, "min/max error (%d/%d)\n", pktgen.minlen, pktgen.maxlen);
			ret = -1;
			goto exit;
		}

		/* Set the new values */
		ret = dhd_var_setbuf(dhd, "pktgen", &pktgen, sizeof(pktgen));
	} else {
		printf("Counts: %d send attempts, %d received, %d tx failures\n",
		       pktgen.numsent, pktgen.numrcvd, pktgen.numfail);
	}

	/* Show configuration in either case */
	switch (pktgen.mode) {
	case DHD_PKTGEN_ECHO: str = "echo"; break;
	case DHD_PKTGEN_SEND: str = "send"; break;
	case DHD_PKTGEN_RECV: str = "recv"; break;
	case DHD_PKTGEN_RXBURST: str = "burst"; break;
	default: str = "UNKNOWN"; break;
	}

	printf("Config: mode %s %d pkts (len %d-%d) each %d ticks\n",
	       str, pktgen.count, pktgen.minlen, pktgen.maxlen, pktgen.freq);

	/* Second config line for optional items */
	str = "        ";
	if (pktgen.total) {
		printf("%slimit %d", str, pktgen.total);
		str = ", ";
	}
	if (pktgen.print) {
		printf("%sprint every %d ticks", str, (pktgen.freq * pktgen.print));
		str = ", ";
	}
	if (pktgen.stop) {
		printf("%sstop after %d tx failures", str, pktgen.stop);
		str = ", ";
	}
	if (str[0] == ',')
		printf("\n");

exit:
	return ret;
}
#endif /* SDTEST */

static dbg_msg_t dhd_sd_msgs[] = {
	{SDH_ERROR_VAL,	"error"},
	{SDH_TRACE_VAL,	"trace"},
	{SDH_INFO_VAL,	"info"},
	{SDH_DATA_VAL,	"data"},
	{SDH_CTRL_VAL,	"control"},
	{SDH_LOG_VAL,	"log"},
	{SDH_DMA_VAL,	"dma"},
	{0,		NULL}
};

static int
dhd_sd_msglevel(void *dhd, cmd_t *cmd, char **argv)
{
	return dhd_do_msglevel(dhd, cmd, argv, dhd_sd_msgs);
}

static int
dhd_sd_blocksize(void *dhd, cmd_t *cmd, char **argv)
{
	int ret;
	int argc;
	char *endptr = NULL;
	void *ptr = NULL;
	int func, size;

	/* arg count */
	for (argc = 0; argv[argc]; argc++);
	argc--;

	if (argc < 1 || argc > 2) {
		printf("required args: function [size] (size 0 means max)\n");
		return BCME_USAGE_ERROR;
	}

	func = strtol(argv[1], &endptr, 0);
	if (*endptr != '\0') {
		printf("Invalid function: %s\n", argv[1]);
		return BCME_USAGE_ERROR;
	}

	if (argc > 1) {
		size = strtol(argv[2], &endptr, 0);
		if (*endptr != '\0') {
			printf("Invalid size: %s\n", argv[1]);
			return BCME_USAGE_ERROR;
		}
	}

	if (argc == 1) {
		if ((ret = dhd_var_getbuf(dhd, cmd->name, &func, sizeof(func), &ptr)) >= 0)
			printf("Function %d block size: %d\n", func, *(int*)ptr);
	} else {
		printf("Setting function %d block size to %d\n", func, size);
		size &= 0x0000ffff; size |= (func << 16);
		ret = dhd_var_setbuf(dhd, cmd->name, &size, sizeof(size));
	}

	return (ret);
}

static int
dhd_sd_mode(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	int argc;
	int sdmode;

	/* arg count */
	for (argc = 0; argv[argc]; argc++);
	argc--;

	if (argv[1]) {
		if (!strcmp(argv[1], "spi")) {
			strcpy(argv[1], "0");
		} else if (!strcmp(argv[1], "sd1")) {
			strcpy(argv[1], "1");
		} else if (!strcmp(argv[1], "sd4")) {
			strcpy(argv[1], "2");
		} else {
			return BCME_USAGE_ERROR;
		}

		ret = dhd_var_setint(wl, cmd, argv);

	} else {
		if ((ret = dhd_var_get(wl, cmd, argv))) {
			return (ret);
		} else {
			sdmode = *(int32*)buf;

			printf("SD Mode is: %s\n",
			       sdmode == 0 ? "SPI"
			       : sdmode == 1 ? "SD1"
				   : sdmode == 2 ? "SD4" : "Unknown");
		}
	}

	return (ret);
}

static int
dhd_dma_mode(void *wl, cmd_t *cmd, char **argv)
{
	int ret;
	int argc;
	int dmamode;

	/* arg count */
	for (argc = 0; argv[argc]; argc++);
	argc--;

	if (argv[1]) {
		if (!stricmp(argv[1], "pio")) {
			strcpy(argv[1], "0");
		} else if (!strcmp(argv[1], "0")) {
		} else if (!stricmp(argv[1], "dma")) {
			strcpy(argv[1], "1");
		} else if (!stricmp(argv[1], "sdma")) {
			strcpy(argv[1], "1");
		} else if (!strcmp(argv[1], "1")) {
		} else if (!stricmp(argv[1], "adma1")) {
			strcpy(argv[1], "2");
		} else if (!stricmp(argv[1], "adma")) {
			strcpy(argv[1], "3");
		} else if (!stricmp(argv[1], "adma2")) {
			strcpy(argv[1], "3");
		} else {
			return BCME_USAGE_ERROR;
		}

		ret = dhd_var_setint(wl, cmd, argv);

	} else {
		if ((ret = dhd_var_get(wl, cmd, argv))) {
			return (ret);
		} else {
			dmamode = *(int32*)buf;

			printf("DMA Mode is: %s\n",
			       dmamode == 0 ? "PIO"
			       : dmamode == 1 ? "SDMA"
			       : dmamode == 2 ? "ADMA1"
			       : dmamode == 3 ? "ADMA2"
			       : "Unknown");
		}
	}

	return (ret);
}

/*
 * dhd_regs2args is a generic function that is used for setting/getting
 * DHDIOVAR variables that require address for read, and
 * address + data for write.
 */
static int
dhd_reg2args(void *dhd, cmd_t *cmd, char **argv)
{
	char var[256];
	uint32 int_val;
	bool get = TRUE;
	uint32 len;
	void *ptr = NULL;
	char *endptr;
	int ret = 0;

	if (argv[1]) {
		len = sizeof(int_val);
		int_val = strtoul(argv[1], &endptr, 0);
		memcpy(var, (char *)&int_val, sizeof(int_val));
	}
	else
		return BCME_USAGE_ERROR;

	if (argv[2]) {
		get = FALSE;
		int_val = strtoul(argv[2], &endptr, 0);
		memcpy(&var[len], (char *)&int_val, sizeof(int_val));
		len += sizeof(int_val);
	}
	if (get) {
		if ((ret = dhd_var_getbuf(dhd, cmd->name, var, sizeof(var), &ptr)) < 0)
			return ret;

		printf("0x%x\n", (*(int *)ptr));
	}
	else
		ret = dhd_var_setbuf(dhd, cmd->name, &var, sizeof(var));
	return ret;
}

static int
dhd_pcie_serdes_reg(void *dhd, cmd_t *cmd, char **argv)
{
	char var[256];
	uint32 int_val;
	uint32 len;
	char *endptr;
	int ret = 0;
	void *ptr = NULL;

	bzero(var, sizeof(var));
	if (argv[1]) {
		len = sizeof(int_val);
		int_val = strtoul(argv[1], &endptr, 0);
		memcpy(var, (char *)&int_val, sizeof(int_val));
	}
	else
		return BCME_USAGE_ERROR;

	if (argv[2]) {
		int_val = strtoul(argv[2], &endptr, 0);
		memcpy(&var[len], (char *)&int_val, sizeof(int_val));
		len += sizeof(int_val);
	}
	else
		return BCME_USAGE_ERROR;

	if (argv[3]) {
		int_val = strtoul(argv[3], &endptr, 0);
		memcpy(&var[len], (char *)&int_val, sizeof(int_val));
		len += sizeof(int_val);
		ret = dhd_var_setbuf(dhd, cmd->name, &var, sizeof(var));
	} else {
		ret = dhd_var_getbuf(dhd, cmd->name, &var, sizeof(var), &ptr);
		if (ret < 0)
			return ret;
		printf("0x%x\n", (*(int *)ptr));
	}
	return ret;
}

static int
dhd_pcie_dmaxfer(void *dhd, cmd_t *cmd, char **argv)
{
	char var[256];
	uint32 int_val;
	uint32 len;
	char *endptr;
	int ret = 0;

	bzero(var, sizeof(var));
	if (argv[1]) {
		len = sizeof(int_val);
		int_val = strtoul(argv[1], &endptr, 0);
		memcpy(var, (char *)&int_val, sizeof(int_val));
	}
	else
		return BCME_USAGE_ERROR;

	if (argv[2]) {
		int_val = strtoul(argv[2], &endptr, 0);
		memcpy(&var[len], (char *)&int_val, sizeof(int_val));
		len += sizeof(int_val);
	}

	if (argv[3]) {
		int_val = strtoul(argv[3], &endptr, 0);
		memcpy(&var[len], (char *)&int_val, sizeof(int_val));
		len += sizeof(int_val);
	}
	ret = dhd_var_setbuf(dhd, cmd->name, &var, sizeof(var));
	return ret;
}

static int
dhd_usb_loopback_txfer(void *dhd, cmd_t *cmd, char **argv)
{
	char var[256];
	uint32 int_val;
	uint32 len;
	char *endptr;
	int ret = 0;

	bzero(var, sizeof(var));
	if (argv[1]) {
		len = sizeof(int_val);
		int_val = strtoul(argv[1], &endptr, 0);
		memcpy(var, (char *)&int_val, sizeof(int_val));
	}
	else
		return BCME_USAGE_ERROR;

	if (argv[2]) {
		int_val = strtoul(argv[2], &endptr, 0);
		memcpy(&var[len], (char *)&int_val, sizeof(int_val));
		len += sizeof(int_val);
	}
	ret = dhd_var_setbuf(dhd, cmd->name, &var, sizeof(var));
	if (ret != BCME_OK) {
		printf("Looopback test failed.\n");
	}
	printf("Looopback test passed.\n");
	return ret;
}

static int
dhd_sdreg(void *dhd, cmd_t *cmd, char **argv)
{
	int ret;
	sdreg_t sdreg;
	uint argc;
	char *ptr = NULL;

	UNUSED_PARAMETER(cmd);

	bzero(&sdreg, sizeof(sdreg));

	/* arg count */
	for (argc = 0; argv[argc]; argc++);
	argc--;

	/* required args: offset (will default size) */
	if (argc < 1) {
		printf("required args: offset[/size] [value]\n");
		return BCME_USAGE_ERROR;
	}

	sdreg.offset = strtoul(argv[1], &ptr, 0);
	if (*ptr && *ptr != '/') {
		printf("Bad arg: %s\n", argv[1]);
		return BCME_USAGE_ERROR;
	}

	/* read optional /size */
	if (*ptr == '/') {
		sdreg.func = strtol((ptr+1), &ptr, 0);
		if (*ptr || ((sdreg.func != 1) && (sdreg.func != 2) &&
			(sdreg.func != 4) && (sdreg.func != 8))) {
			printf("Bad size option?\n");
			return BCME_USAGE_ERROR;
		}
	}
	else {
		sdreg.func = 4;
		printf("Defaulting to register size 4\n");
	}

	if (argc > 1) {
		sdreg.value = strtoul(argv[2], &ptr, 0);
		if (*ptr) {
			printf("Bad value: %s\n", argv[2]);
			return BCME_USAGE_ERROR;
		}
	}

	if (argc <= 1) {
		ret = dhd_var_getbuf(dhd, argv[0], &sdreg, sizeof(sdreg), (void**)&ptr);
		if (ret >= 0)
			printf("0x%0*x\n", (2 * sdreg.func), *(int *)ptr);
	} else {
		ret = dhd_var_setbuf(dhd, argv[0], &sdreg, sizeof(sdreg));
	}

	return (ret);
}

static int
dhd_membytes(void *dhd, cmd_t *cmd, char **argv)
{
	int ret = -1;
	uint argc;
	char *ptr;
	int params[2];
	uint addr;
	uint len;
	int align;

	int rawout, hexin;

	miniopt_t opts;
	int opt_err;

	/* Parse command-line options */
	miniopt_init(&opts, "membytes", "rh", FALSE);

	rawout = hexin = 0;

	argv++;
	while ((opt_err = miniopt(&opts, argv)) != -1) {
		if (opt_err == 1) {
			fprintf(stderr, "membytes options error\n");
			ret = -1;
			goto exit;
		}

		if (opts.positional)
			break;

		argv += opts.consumed;

		if (opts.opt == 'h') {
			hexin = 1;
		} else if (opts.opt == 'r') {
			rawout = 1;
		} else {
			fprintf(stderr, "membytes command error\n");
			ret = -1;
			goto exit;
		}
	}

	/* arg count */
	for (argc = 0; argv[argc]; argc++);

	/* required args: address size [<data>]] */
	if (argc < 2) {
		fprintf(stderr, "required args: address size [<data>]\n");
		return BCME_USAGE_ERROR;
	}

	if (argc < 3 && hexin) {
		fprintf(stderr, "missing <data> required by -h\n");
		return BCME_USAGE_ERROR;
	}
	if ((argc > 2) && (rawout)) {
		fprintf(stderr, "can't have <data> arg with -r\n");
		return BCME_USAGE_ERROR;
	}

	/* read address */
	addr = strtoul(argv[0], &ptr, 0);
	if (*ptr) {
		fprintf(stderr, "Bad arg: %s\n", argv[0]);
		return BCME_USAGE_ERROR;
	}

	/* read size */
	len = strtoul(argv[1], &ptr, 0);
	if (*ptr) {
		fprintf(stderr, "Bad value: %s\n", argv[1]);
		return BCME_USAGE_ERROR;
	}

	align = addr & 0x03;
	if (align && argc > 2) {
		fprintf(stderr, "Can only write starting at long-aligned addresses.\n");
		return BCME_USAGE_ERROR;
	}

	/* get can just use utility function, set must copy custom buffer */
	if (argc == 2) {
		/* Read */
		uint chunk = DHD_IOCTL_MAXLEN;
		for (addr -= align, len += align; len; addr += chunk, len -= chunk, align = 0) {
			chunk = MIN(chunk, len);
			params[0] = addr;
			params[1] = ROUNDUP(chunk, 4);
			ret = dhd_var_getbuf(dhd, "membytes",
			                     params, (2 * sizeof(int)), (void**)&ptr);
			if (ret < 0)
				goto exit;

			if (rawout) {
				fwrite(ptr + align, sizeof(char), chunk - align, stdout);
			} else {
				dhd_hexdump((uchar*)ptr + align, chunk - align, addr + align);
			}
		}
	} else {
		/* Write */
		uint patlen = (uint)strlen(argv[2]);
		uint chunk, maxchunk;
		char *sptr;

		if (hexin) {
			char *inptr, *outptr;
			if (patlen & 1) {
				fprintf(stderr, "Hex (-h) must consist of whole bytes\n");
				ret = BCME_USAGE_ERROR;
				goto exit;
			}

			for (inptr = outptr = argv[2]; patlen; patlen -= 2) {
				int n1, n2;

				n1 = (int)((unsigned char)*inptr++);
				n2 = (int)((unsigned char)*inptr++);
				if (!isxdigit(n1) || !isxdigit(n2)) {
					fprintf(stderr, "invalid hex digit %c\n",
					        (isxdigit(n1) ? n2 : n1));
					ret = BCME_USAGE_ERROR;
					goto exit;
				}
				n1 = isdigit(n1) ? (n1 - '0')
				        : ((islower(n1) ? (toupper(n1)) : n1) - 'A' + 10);
				n2 = isdigit(n2) ? (n2 - '0')
				        : ((islower(n2) ? (toupper(n2)) : n2) - 'A' + 10);
				*outptr++ = (n1 * 16) + n2;
			}

			patlen = (uint)(outptr - argv[2]);
		}

		sptr = argv[2];
		maxchunk = DHD_IOCTL_MAXLEN - ((uint)strlen(cmd->name) + 1 + (2 * sizeof(int)));

		while (len) {
			chunk = (len > maxchunk) ? (maxchunk & ~0x3) : len;

			/* build the iovar command */
			memset(buf, 0, DHD_IOCTL_MAXLEN);
			strcpy(buf, cmd->name);
			ptr = buf + strlen(buf) + 1;
			params[0] = addr; params[1] = chunk;
			memcpy(ptr, params, (2 * sizeof(int)));
			ptr += (2 * sizeof(int));
			addr += chunk; len -= chunk;

			while (chunk--) {
				*ptr++ = *sptr++;
				if (sptr >= (argv[2] + patlen))
					sptr = argv[2];
			}

			ret = dhd_set(dhd, DHD_SET_VAR, &buf[0], (int)(ptr - buf));
			if (ret < 0)
				goto exit;
		}
	}

exit:
	return ret;
}

static int
dhd_idletime(void *dhd, cmd_t *cmd, char **argv)
{
	int32 idletime;
	char *endptr = NULL;
	int err = 0;

	if (argv[1]) {
		if (!strcmp(argv[1], "never")) {
			idletime = 0;
		} else if (!strcmp(argv[1], "immediate") || !strcmp(argv[1], "immed")) {
			idletime = DHD_IDLE_IMMEDIATE;
		} else {
			idletime = strtol(argv[1], &endptr, 0);
			if (*endptr != '\0') {
				fprintf(stderr, "invalid number %s\n", argv[1]);
				err = BCME_USAGE_ERROR;
			}
		}
		if ((idletime < 0) && (idletime != DHD_IDLE_IMMEDIATE)) {
			fprintf(stderr, "invalid value %s\n", argv[1]);
			err = -1;
		}

		if (!err) {
			strcpy(buf, "idletime");
			endptr = buf + strlen(buf) + 1;
			memcpy(endptr, &idletime, sizeof(uint32));
			endptr += sizeof(uint32);
			err = dhd_set(dhd, DHD_SET_VAR, &buf[0], (int)(endptr - buf));
		}
	} else {
		if ((err = dhd_var_get(dhd, cmd, argv))) {
			return err;
		} else {
			idletime = *(int32*)buf;

			if (idletime == 0) {
				printf("0 (never)\n");
			} else if (idletime == DHD_IDLE_IMMEDIATE) {
				printf("-1 (immediate)\n");
			} else if (idletime > 0) {
				printf("%d\n", idletime);
			} else printf("%d (invalid)\n", idletime);
		}
	}
	return err;
}

static int
dhd_idleclock(void *dhd, cmd_t *cmd, char **argv)
{
	int32 idleclock;
	char *endptr = NULL;
	int err = 0;

	if (argv[1]) {
		if (!strcmp(argv[1], "active")) {
			idleclock = DHD_IDLE_ACTIVE;
		} else if (!strcmp(argv[1], "stopped")) {
			idleclock = DHD_IDLE_STOP;
		} else {
			idleclock = strtol(argv[1], &endptr, 0);
			if (*endptr != '\0') {
				fprintf(stderr, "invalid number %s\n", argv[1]);
				err = BCME_USAGE_ERROR;
			}
		}

		if (!err) {
			strcpy(buf, "idleclock");
			endptr = buf + strlen(buf) + 1;
			memcpy(endptr, &idleclock, sizeof(int32));
			endptr += sizeof(int32);
			err = dhd_set(dhd, DHD_SET_VAR, &buf[0], (int)(endptr - buf));
		}
	} else {
		if ((err = dhd_var_get(dhd, cmd, argv))) {
			return err;
		} else {
			idleclock = *(int32*)buf;

			if (idleclock == DHD_IDLE_ACTIVE)
				printf("Idleclock %d (active)\n", idleclock);
			else if (idleclock == DHD_IDLE_STOP)
				printf("Idleclock %d (stopped)\n", idleclock);
			else
				printf("Idleclock divisor %d\n", idleclock);
		}
	}
	return err;
}

/* Word count for a 4kb SPROM */
#define SPROM_WORDS 256

static int
dhd_sprom(void *dhd, cmd_t *cmd, char **argv)
{
#if !defined(BWL_FILESYSTEM_SUPPORT)
	return (-1);
#else
	int ret, i;
	uint argc;
	char *endptr;
	char *bufp, *countptr;
	uint16 *wordptr;
	uint offset, words, bytes;
	bool nocrc = FALSE;

	char *fname;
	FILE *fp;

	UNUSED_PARAMETER(cmd);

	/* arg count */
	for (argc = 0; argv[argc]; argc++);
	argc--;

	/* init buffer */
	bufp = buf;
	memset(bufp, 0, DHD_IOCTL_MAXLEN);
	strcpy(bufp, "sprom");
	bufp += strlen("sprom") + 1;

	if (strcmp(argv[0], "srdump") == 0) {
		if (argc) {
			fprintf(stderr, "Command srdump doesn't take args\n");
			return BCME_USAGE_ERROR;
		}
		offset = 0;
		words = SPROM_WORDS;
		bytes = 2 * words;

		memcpy(bufp, &offset, sizeof(int));
		bufp += sizeof(int);
		memcpy(bufp, &bytes, sizeof(int));
		bufp += sizeof(int);

		if (!ISALIGNED((uintptr)bufp, sizeof(uint16))) {
			fprintf(stderr, "Internal error: unaligned word buffer\n");
			return BCME_ERROR;
		}
	} else {
		if (strcmp(argv[0], "srwrite") != 0) {
			fprintf(stderr, "Unimplemented sprom command: %s\n", argv[0]);
			return BCME_USAGE_ERROR;
		}

		if (argc == 0) {
			return BCME_USAGE_ERROR;
		} else if ((argc == 1) ||
		           ((argc == 2) && ((nocrc = !strcmp(argv[1], "-c"))))) {

			fname = nocrc ? argv[2] : argv[1];

			/* determine and validate file size */
			if ((ret = file_size(fname)) < 0)
				return BCME_ERROR;

			bytes = ret;
			offset = 0;
			words = bytes / 2;

			if (bytes != 2 * SPROM_WORDS) {
				fprintf(stderr, "Bad file size\n");
				return BCME_ERROR;
			}

			memcpy(bufp, &offset, sizeof(int));
			bufp += sizeof(int);
			memcpy(bufp, &bytes, sizeof(int));
			bufp += sizeof(int);

			if (!ISALIGNED((uintptr)bufp, sizeof(uint16))) {
				fprintf(stderr, "Internal error: unaligned word buffer\n");
				return BCME_ERROR;
			}

			if ((fp = fopen(fname, "rb")) == NULL) {
				fprintf(stderr, "Could not open %s: %s\n",
				        fname, strerror(errno));
				return BCME_ERROR;
			}

			if (fread((uint16*)bufp, sizeof(uint16), words, fp) != words) {
				fprintf(stderr, "Could not read %d bytes from %s\n",
				        words * 2, fname);
				fclose(fp);
				return BCME_ERROR;
			}

			fclose(fp);

			if (!nocrc &&
			    hndcrc8((uint8*)bufp, bytes, CRC8_INIT_VALUE) != CRC8_GOOD_VALUE) {
				fprintf(stderr, "CRC check failed: 0x%02x, should be 0x%02x.\n",
				        ((uint8*)bufp)[bytes-1],
				        ~hndcrc8((uint8*)bufp, bytes - 1, CRC8_INIT_VALUE) & 0xff);
				return BCME_ERROR;
			}

			ltoh16_buf(bufp, bytes);
		} else {
			offset = strtoul(*++argv, &endptr, 0) * 2;
			if (*endptr != '\0') {
				fprintf(stderr, "offset %s is not an integer\n", *argv);
				return BCME_USAGE_ERROR;
			}

			memcpy(bufp, &offset, sizeof(int));
			bufp += sizeof(int);
			countptr = bufp;
			bufp += sizeof(int);

			if (!ISALIGNED((uintptr)bufp, sizeof(uint16))) {
				fprintf(stderr, "Internal error: unaligned word buffer\n");
				return BCME_ERROR;
			}

			for (words = 0, wordptr = (uint16*)bufp; *++argv; words++) {
				*wordptr++ = (uint16)strtoul(*argv, &endptr, 0);
				if (*endptr != '\0') {
					fprintf(stderr, "value %s is not an integer\n", *argv);
					return BCME_USAGE_ERROR;
				}
				if (words > SPROM_WORDS) {
					fprintf(stderr, "max of %d words\n", SPROM_WORDS);
					return BCME_USAGE_ERROR;
				}
			}

			bytes = 2 * words;
			memcpy(countptr, &bytes, sizeof(int));
		}
	}

	if (argc) {
		ret = dhd_set(dhd, DHD_SET_VAR, buf,
		              ((int)strlen("sprom") + 1) + (2 * sizeof(int)) + bytes);
		return (ret);
	} else {
		ret = dhd_get(dhd, DHD_GET_VAR, buf,
		              ((int)strlen("sprom") + 1) + (2 * sizeof(int)) + bytes);
		if (ret < 0) {
			return ret;
		}

		for (i = 0; i < (int)words; i++) {
			if ((i % 8) == 0)
				printf("\n  srom[%03d]:  ", i);
			printf("0x%04x  ", ((uint16*)buf)[i]);
		}
		printf("\n");
	}

	return 0;
#endif /* BWL_FILESYSTEM_SUPPORT */
}

/*
 * read_vars: reads an environment variables file into a buffer,
 * reformatting them and returning the length (-1 on error).
 *
 * The input text file consists of lines of the form "<var>=<value>\n".
 * CRs are ignored, as are blank lines and comments beginning with '#'.
 *
 * The output buffer consists of blocks of the form "<var>=<value>\0"
 * (the newlines have been replaced by NULs)
 *
 * Todo: allow quoted variable names and quoted values.
*/

#if defined(BWL_FILESYSTEM_SUPPORT)
static int
read_vars(char *fname, char *buf, int buf_maxlen)
{
	FILE *fp;
	int buf_len;
	struct stat finfo;
	uint file_size;

	if (stat(fname, &finfo)) {
		perror(fname);
		exit(1);
	}
	file_size = finfo.st_size;
	if (file_size > buf_maxlen) {
		fprintf(stderr, "NVRAM file %s (%d bytes) too long for %d bytes buffer\n",
			fname, file_size, buf_maxlen);
		return -1;
	}
	if ((fp = fopen(fname, "rb")) == NULL) {
		fprintf(stderr, "Cannot open NVRAM file %s: %s\n",
		        fname, strerror(errno));
		exit(1);
	}

	/* read the file and push blocks down to memory */
	if (fread(buf, 1, file_size, fp) != file_size) {
		perror(fname);
		fclose(fp);
		exit(1);
	}

	fclose(fp);
	fp = NULL;

	/* convert linefeeds to nuls */
	buf_len = process_nvram_vars(buf, file_size);

	return buf_len;
}
#endif   /* BWL_FILESYSTEM_SUPPORT */

static int
dhd_vars(void *dhd, cmd_t *cmd, char **argv)
{
	int ret;
	uint argc;
	char *bufp;

	UNUSED_PARAMETER(cmd);

	/* arg count */
	for (argc = 0; argv[argc]; argc++);
	argc--;

	switch (argc) {
	case 0: /* get */
	{
		if ((ret = dhd_var_getbuf(dhd, "vars", NULL, 0, (void**)&bufp)))
			break;
		while (*bufp) {
			printf("%s\n", bufp);
			bufp += strlen(bufp) + 1;
		}
	}
	break;

#if defined(BWL_FILESYSTEM_SUPPORT)
	case 1: /* set */
	{
		char *vname;
		uint nvram_len;

		vname = argv[1];

		bufp = buf;
		strcpy(bufp, "vars");
		bufp += strlen("vars") + 1;

		if ((ret = read_vars(vname, bufp,
		                           DHD_IOCTL_MAXLEN - ((int)strlen("vars") + 3))) < 0) {
			ret = -1;
			break;
		}

		nvram_len = ret;
		bufp += nvram_len;
		*bufp++ = 0;

		ret = dhd_set(dhd, DHD_SET_VAR, buf, (int)(bufp - buf));
	}
	break;
#endif   /* BWL_FILESYSTEM_SUPPORT */

	default:
		ret = -1;
		break;
	}

	return ret;
}

#define MEMBLOCK 2048

/* Check that strlen("membytes")+1 + 2*sizeof(int32) + MEMBLOCK <= DHD_IOCTL_MAXLEN */
#if (MEMBLOCK + 17 > DHD_IOCTL_MAXLEN)
#error MEMBLOCK/DHD_IOCTL_MAXLEN sizing
#endif // endif

#if defined(BWL_FILESYSTEM_SUPPORT)
static int
dhd_verify_file_bytes(void *dhd, uint8 *memblock, int start, uint len)
{
	int ret = 0;
	uint i = 0;
	char *ptr;
	int params[2];
	uint8 *src, *dst;

	params[0] = start;
	params[1] = len;
	ret = dhd_var_getbuf(dhd, "membytes", params, 2 * sizeof(int), (void**)&ptr);
	if (ret) {
		fprintf(stderr, "%s: failed reading %d membytes from 0x%08x\n",
		__FUNCTION__, len, start);
		return ret;
	}

	src = (uint8 *)memblock;
	dst = (uint8 *)ptr;
	while (i < len) {
		if (src[i] != dst[i]) {
			fprintf(stderr, "   0x%x: exp[0x%02X] != got[0x%02X]\n",
				start+i, src[i], dst[i]);
			ret = -1;
		}
		i++;
	}

	return ret;
}

static int
dhd_load_file_bytes(void *dhd, cmd_t *cmd, FILE *fp, int fsize, int start, uint blk_sz, bool verify)
{
	int tot_len = 0;
	uint read_len;
	char *bufp;
	uint len;
	uint8 memblock[MEMBLOCK];
	int ret;
	int retry;

	UNUSED_PARAMETER(cmd);

	if (!fsize || !fp)
		return -1;

	assert(blk_sz <= MEMBLOCK);

	while (tot_len < fsize) {
		read_len = fsize - tot_len;
		if (read_len >= blk_sz) {
			read_len = blk_sz;

			if (!ISALIGNED(start, MEMBLOCK))
				read_len = ROUNDUP(start, MEMBLOCK) - start;
		}

		len = (uint)fread(memblock, sizeof(uint8), read_len, fp);
		if ((len < read_len) && !feof(fp)) {
			fprintf(stderr, "%s: error reading file\n", __FUNCTION__);
			return -1;

		}
		retry = 0;
failed_retry:

		bufp = buf;
		memset(bufp, 0, DHD_IOCTL_MAXLEN);
		strcpy(bufp, "membytes");
		bufp += strlen("membytes") + 1;
		memcpy(bufp, &start, sizeof(int));
		bufp += sizeof(int);
		memcpy(bufp, &len, sizeof(int));
		bufp += sizeof(int);
		memcpy(bufp, memblock, len);

		if ((ret = dhd_set(dhd, DHD_SET_VAR, &buf[0], (int)(bufp - buf + len))) < 0) {
			fprintf(stderr, "%s: error %d on writing %d membytes at 0x%08x\n",
			        __FUNCTION__, ret, len, start);
			return ret;
		}

		if (verify == TRUE) {
			if (dhd_verify_file_bytes(dhd, memblock, start, len) != 0) {
				if (retry++ < 5000)
				{
					fprintf(stderr, "%s: verify failed %d membytes "
						"from 0x%08x\n", __FUNCTION__, len, start);
					goto failed_retry;
				}
			}
		}

		start += len;
		tot_len += len;
	}
	return 0;
}
#endif   /* BWL_FILESYSTEM_SUPPORT */

#ifdef QMONITOR
static int
dhd_qtime_thres(void *dhd, cmd_t *cmd, char **argv)
{
	int flag = 0xdead;

	if (argv[1]) {
		flag = atoi(argv[1]);
		dhd_iovar_setint(dhd, cmd->name, flag);
	}
	else {
		dhd_iovar_getint(dhd, cmd->name, &flag);
		printf("qtime_thres: %d\n", flag);
	}
	return 0;
}

static int
dhd_qtime_percent(void *dhd, cmd_t *cmd, char **argv)
{
	int flag = 0xdead;

	if (argv[1]) {
		flag = atoi(argv[1]);
		dhd_iovar_setint(dhd, cmd->name, flag);
	}
	else {
		dhd_iovar_getint(dhd, cmd->name, &flag);
		printf("qtime_percent: %d %%\n", flag);
	}
	return 0;
}
#endif /* QMONITOR */

#ifdef PROP_TXSTATUS
static int
dhd_proptxstatusenable(void *dhd, cmd_t *cmd, char **argv)
{
	int flag = 0xdead;
	int ret;

	if (argv[1]) {
		flag = atoi(argv[1]);
		ret = dhd_iovar_setint(dhd, cmd->name, flag);
	}
	else {
		ret = dhd_iovar_getint(dhd, cmd->name, &flag);
		if (ret >= 0)
			printf("proptxstatus: %d\n", flag);
	}
	return ret;
}

static int
dhd_proptxstatusmode(void *dhd, cmd_t *cmd, char **argv)
{
	int mode = 0xdead;
	int ret;

	if (argv[1]) {
		mode = atoi(argv[1]);
		ret = dhd_iovar_setint(dhd, cmd->name, mode);
	}
	else {
		ret = dhd_iovar_getint(dhd, cmd->name, &mode);
		if (ret >= 0)
			printf("proptxstatusmode: %d\n", mode);
	}
	return ret;
}

static int
dhd_proptxopt(void *dhd, cmd_t *cmd, char **argv)
{
	int flag = 0xdead;
	int ret;

	if (argv[1]) {
		flag = atoi(argv[1]);
		ret = dhd_iovar_setint(dhd, cmd->name, flag);
	}
	else {
		ret = dhd_iovar_getint(dhd, cmd->name, &flag);
		if (ret >= 0)
			printf("proptx_opt: %d\n", flag);
	}
	return ret;
}

#endif /* PROP_TXSTATUS */

#if defined(BCM_DHD_RUNNER)
static int
dhd_runner_varstr(void *dhd, cmd_t *cmd, char **argv)
{
	char buff[128];
	uint argc;
	char *p  = NULL;
	char *endp = NULL;
	char *argvp = NULL;
	int ret;

	/* Check if these are part of dhd_runner supported iovar commands */
	if ((strcmp(cmd->name, "rnr_flowring_profile")) &&
	    (strcmp(cmd->name, "rnr_flowring_policy")) &&
	    (strcmp(cmd->name, "rnr_rxoffload"))) {

	    return BCME_USAGE_ERROR;
	}

	if (argv[1]) {
	    /* iovar set command */
	    p = buff;
	    endp = p+sizeof(buff);

	    /* Copy the arguments to a single buffer */
	    for (argc = 1; argv[argc]; argc++) {
	        argvp = argv[argc];
	        while (p != endp && (*p++ = *argvp++) != '\0');
	        if (p == endp)
	            return BCME_BUFTOOLONG;
	        /* replace string delimiter with parameter delimiter */
	        *(p-1) = ' ';
	    }
	    /* replace parameter delimiter with final string delimiter */
	    *(p-1) = '\0';

	    /* call the iovar set function */
	    ret = dhd_var_setbuf(dhd, cmd->name, buff, (p-buff));
	}
	else
	{
	    /* call the iovar get function */
	    ret = dhd_varstr(dhd, cmd, argv);
	}

	return (ret);
}

#endif /* BCM_DHD_RUNNER */

static int
dhd_get_ramstart(void *dhd, uint32 *ramstart)
{
	int ret;
	char *ramstart_args[] = {"ramstart", NULL};

	/* Read the bus type the DHD driver is associated to */
	if ((ret = dhd_var_get(dhd, NULL, ramstart_args)) != BCME_OK) {
		fprintf(stderr, "%s: error obtaining ramstart\n", __FUNCTION__);

		return ret;
	}

	*ramstart = *(uint32 *)buf;

	return BCME_OK;
}

static void
dhd_ramsize_adj(void *dhd, uint32 bustype, uint32 ramsize_org, FILE *fp)
{
	int i, search_len = 0;
	uint8 *memptr = NULL;
	uint32 ramsize_ptr_ptr[] = {RAMSIZE_PTR_PTR_LIST};
	uint32 read_len;
	hnd_ramsize_ptr_t ramsize_info;

	/* Pcie only for now */
	if (bustype != BUS_TYPE_PCIE) {
		fprintf(stderr, "%s: unsupported bus type %d to adjust ram size.\n",
			__FUNCTION__, bustype);
		return;
	}

	/* Get maximum RAMSIZE info search length */
	for (i = 0; ; i++) {
		if (ramsize_ptr_ptr[i] == RAMSIZE_PTR_PTR_END)
			break;

		if (search_len < ramsize_ptr_ptr[i])
			search_len = ramsize_ptr_ptr[i];
	}

	if (!search_len)
		return;

	search_len += sizeof(hnd_ramsize_ptr_t);

	memptr = malloc(search_len);
	if (memptr == NULL) {
		fprintf(stderr, "%s: Failed to allocate memory %d bytes\n",
			__FUNCTION__, search_len);
		goto err;
	}

	/* Read image file */
	read_len = (uint32)fread(memptr, sizeof(uint8), search_len, fp);
	fseek(fp, 0, SEEK_SET);
	if (read_len != search_len) {
		fprintf(stderr, "%s: Failed to allocate memory %d bytes\n",
			__FUNCTION__, search_len);
		goto err;
	}

	/* Check Magic */
	for (i = 0; ; i++) {
		if (ramsize_ptr_ptr[i] == RAMSIZE_PTR_PTR_END)
			break;

		if (ramsize_ptr_ptr[i] + sizeof(hnd_ramsize_ptr_t) > search_len)
			continue;

		memcpy((char *)&ramsize_info, memptr + ramsize_ptr_ptr[i],
			sizeof(hnd_ramsize_ptr_t));

		if (ramsize_info.magic == HND_RAMSIZE_PTR_MAGIC &&
			ramsize_org != ramsize_info.ram_size) {
			if (dhd_var_setbuf(dhd, "ramsize", &ramsize_info.ram_size,
				sizeof(ramsize_info.ram_size))) {
				fprintf(stderr, "%s: error setting ramsize\n", __FUNCTION__);
				goto err;
			}
			fprintf(stderr, "Adjust dongle ram size to 0x%x\n", ramsize_info.ram_size);
			break;
		}
	}

err:
	if (memptr)
		free(memptr);

	return;
}

static int
dhd_download(void *dhd, cmd_t *cmd, char **argv)
{
#if !defined(BWL_FILESYSTEM_SUPPORT)
	return (-1);
#else
	bool reset = TRUE;
	bool run = TRUE;
	bool verify = FALSE;
	char *fname = NULL;
	char *vname = NULL;
	uint32 start;
	int ret = 0;
	int fsize;
	uint32 bustype;
	long filepos;

	FILE *fp = NULL;
	uint32 ramsize;
	char *memszargs[] = { "ramsize", NULL };

	char *bufp;

	miniopt_t opts;
	int opt_err;
	uint nvram_len;
	struct trx_header trx_hdr;
	uint32 trx_hdr_len;
	bool trx_file = FALSE;
	uint memblock_sz = MEMBLOCK;
	bool embedded_ucode = FALSE;

	UNUSED_PARAMETER(cmd);

	if ((ret = dhd_get_ramstart(dhd, &start)) != BCME_OK)
		goto exit;

	/* Parse command-line options */
	miniopt_init(&opts, "download", "", TRUE);

	argv++;
	while ((opt_err = miniopt(&opts, argv)) != -1) {
		if (opt_err == 1) {
			fprintf(stderr, "download options error\n");
			ret = -1;
			goto exit;
		}
		argv += opts.consumed;

		if (opts.opt == 'a') {
			if (!opts.good_int) {
				fprintf(stderr, "invalid address %s\n", opts.valstr);
				ret = -1;
				goto exit;
			}
			start = (uint32)opts.uval;
		} else if (opts.positional) {
			if (fname && vname) {
				fprintf(stderr, "extra positional arg, %s\n",
				        opts.valstr);
				ret = -1;
				goto exit;
			}
			if (fname)
				vname = opts.valstr;
			else
				fname = opts.valstr;
		} else if (!opts.opt) {
			if (!strcmp(opts.key, "noreset")) {
				reset = FALSE;
			} else if (!strcmp(opts.key, "norun")) {
				run = FALSE;
			} else if (!strcmp(opts.key, "verify")) {
				verify = TRUE;
			} else {
				fprintf(stderr, "unrecognized option %s\n", opts.valstr);
				ret = -1;
				goto exit;
			}
		} else {
			fprintf(stderr, "unrecognized option %c\n", opts.opt);
			ret = -1;
			goto exit;
		}
	}

	/* validate arguments */
	if (!fname) {
		fprintf(stderr, "filename required\n");
		ret = -1;
		goto exit;
	}

	/* validate file size compared to memory size */
	if ((fsize = file_size(fname)) < 0) {
		ret = -1;
		goto exit;
	}
	/* read the file and push blocks down to memory */
	if ((fp = fopen(fname, "rb")) == NULL) {
		fprintf(stderr, "%s: unable to open %s: %s\n",
		        __FUNCTION__, fname, strerror(errno));
		ret = -1;
		goto exit;
	}
	/* Verify the file is a regular bin file or trx file */
	{
		uint32 tmp_len;
		trx_hdr_len = sizeof(struct trx_header);
		tmp_len = (uint32)fread(&trx_hdr, sizeof(uint8), trx_hdr_len, fp);
		if (tmp_len == trx_hdr_len) {
			if (trx_hdr.magic == TRX_MAGIC) {
				trx_file = TRUE;
				if (trx_hdr.flag_version & TRX_EMBED_UCODE)
					embedded_ucode = TRUE;
			}
			else
				fseek(fp, 0, SEEK_SET);
		}
		else
			fseek(fp, 0, SEEK_SET);
	}

	/* Check on which bus the dhd driver is sitting. Downloading methodology differs from
	 * USB to SDIO.
	 */
	{
		char* bustype_args[] = {"bustype", NULL};

		/* Read the bus type the DHD driver is associated to */
		if ((ret = dhd_var_get(dhd, NULL, bustype_args))) {
			fprintf(stderr, "%s: error obtaining bustype\n", __FUNCTION__);
			goto exit;
		}

		bustype = *(uint32*)buf;
	}

	if (trx_file)
		fsize = (int)(trx_hdr.offsets[0]);
	if (bustype == BUS_TYPE_PCIE) {
		if ((ret = dhd_var_get(dhd, NULL, memszargs))) {
			fprintf(stderr, "%s: error obtaining memsize\n", __FUNCTION__);
			goto exit;
		}
		ramsize = *(uint32*)buf;

		/* Adjust the ramsize if ram image has specified, for now pcie only */
		dhd_ramsize_adj(dhd, bustype, ramsize, fp);
	}

	if (bustype == BUS_TYPE_SDIO) {
		if ((ret = dhd_var_get(dhd, NULL, memszargs))) {
			fprintf(stderr, "%s: error obtaining ramsize\n", __FUNCTION__);
			goto exit;
		}
		ramsize = *(uint32*)buf;
	}

	BCM_REFERENCE(ramsize);

	/* do the download reset if not suppressed */
	if (reset) {
		if ((ret = dhd_iovar_setint(dhd, "dwnldstate", TRUE))) {
			fprintf(stderr, "%s: failed to put dongle in download mode\n",
			        __FUNCTION__);
			goto exit;
		}
	}

	if (BUS_TYPE_USB == bustype) {
		/* store the cur pos pointing to base image which should be written */
		filepos = ftell(fp);
		if (filepos == -1) {
			fprintf(stderr, "%s: ftell failed.\n", __FUNCTION__);
		}

		/* In case of USB, we need to write header information also to dongle. */
		fseek(fp, 0, SEEK_SET);

		/* The file size is "base_image + TRX_Header_size" */
		fsize = (int)(trx_hdr.offsets[0] + sizeof(struct trx_header));

		memblock_sz = RDL_CHUNK;
	}

	/* Load the ram image */
	if ((ret = dhd_load_file_bytes(dhd, cmd, fp, fsize, start, memblock_sz, verify))) {
		fprintf(stderr, "%s: error loading the ramimage at addr 0x%x\n",
		        __FUNCTION__, start);
		goto exit;
	}

	if (trx_file) {

		filepos = ftell(fp);
		if (filepos == -1) {
			fprintf(stderr, "%s: ftell failed.\n", __FUNCTION__);
		}

		if (BUS_TYPE_SDIO == bustype) {

		}
	}

	fclose(fp);
	fp = NULL;

	/* download the vars file if specified */
	if (vname) {
		bufp = buf;
		strcpy(bufp, "vars");
		bufp += strlen("vars") + 1;

		if ((ret = read_vars(vname, bufp,
		                           DHD_IOCTL_MAXLEN - ((int)strlen("vars") + 3))) < 0) {
			ret = -1;
			goto exit;
		}

		nvram_len = ret;
		bufp += nvram_len;
		*bufp++ = 0;

		ret = dhd_set(dhd, DHD_SET_VAR, buf, (int)(bufp - buf));
		if (ret) {
			fprintf(stderr, "%s: error %d on delivering vars\n",
			        __FUNCTION__, ret);
			goto exit;
		}
	}

	/* start running the downloaded code if not suppressed */
	if (run) {
		if ((ret = dhd_iovar_setint(dhd, "dwnldstate", FALSE))) {

			fprintf(stderr, "%s: failed to take dongle out of download mode\n",
			        __FUNCTION__);
			/* USB Error return values */
			if (BUS_TYPE_USB == bustype) {
				if (ret == -1)
					fprintf(stderr, "%s: CPU is not in RUNNABLE State\n",
						__FUNCTION__);
				else
					fprintf(stderr, "%s: Error in setting CPU to RUN mode.\n",
						__FUNCTION__);
			}
			goto exit;
		}
	}
	if (embedded_ucode) {
	}

exit:
	if (fp)
		fclose(fp);

	return ret;
#endif /* BWL_FILESYSTEM_SUPPORT */
}

static int
dhd_dldn(void *dhd, cmd_t *cmd, char **argv)
{
#if !defined(BWL_FILESYSTEM_SUPPORT)
	return (-1);
#else
	char *fname = NULL;
	uint32 start;
	int ret = 0;
	int fsize;
	int fd = 0;

	FILE *fp = NULL;
	uint32 ramsize;

	uint len;
	uint8 memblock[MEMBLOCK];

	miniopt_t opts;
	int opt_err;

	UNUSED_PARAMETER(cmd);

	/* Parse command-line options */
	miniopt_init(&opts, "download", "", TRUE);
	argv++;

	while ((opt_err = miniopt(&opts, argv)) != -1) {
		if (opt_err == 1) {
			fprintf(stderr, "download options error\n");
			ret = -1;
			goto exit;
		}
		argv += opts.consumed;

		if (opts.positional) {
			if (fname) {
				fprintf(stderr, "extra positional arg, %s\n",
				        opts.valstr);
				ret = -1;
				goto exit;
			}
			if (!fname)
				fname = opts.valstr;
		} else {
			fprintf(stderr, "unrecognized option %c\n", opts.opt);
			ret = -1;
			goto exit;
		}
	}

	fd = dhd_set(dhd, DHD_DLDN_ST, NULL, 0);
	if (fd < 0) {
		ret = -1;
		goto exit;
	}

	/* validate arguments */
	if (!fname) {
		fprintf(stderr, "filename required\n");
		ret = -1;
		goto exit;
	}

	/* validate file size compared to memory size */
	if ((fsize = file_size(fname)) < 0) {
		ret = -1;
		goto exit;
	}

	ramsize = 393216;

	if (ramsize && ((uint32)fsize > ramsize)) {
		fprintf(stderr, "%s: file %s too large (%d > %d)\n",
		        __FUNCTION__, fname, fsize, ramsize);
		ret = -1;
		goto exit;
	}

	/* read the file and push blocks down to memory */
	if ((fp = fopen(fname, "rb")) == NULL) {
		fprintf(stderr, "%s: unable to open %s: %s\n",
		        __FUNCTION__, fname, strerror(errno));
		ret = -1;
		goto exit;
	}

	if ((ret = dhd_get_ramstart(dhd, &start)) != BCME_OK)
		goto exit;

	while ((len = (uint)fread(memblock, sizeof(uint8), MEMBLOCK, fp))) {
		if (len < MEMBLOCK && !feof(fp)) {
			fprintf(stderr, "%s: error reading file %s\n", __FUNCTION__, fname);
			ret = -1;
			goto exit;
		}

		ret = dhd_set(dhd, DHD_DLDN_WRITE, memblock, len);
		if (ret) {
			fprintf(stderr, "%s: error %d on writing %d membytes at 0x%08x\n",
			        __FUNCTION__, ret, len, start);
			goto exit;
		}

		start += len;
	}

	if (!feof(fp)) {
		fprintf(stderr, "%s: error reading file %s\n", __FUNCTION__, fname);
		ret = -1;
		goto exit;
	}
	fclose(fp);
	fp = NULL;

exit:
	if (fp)
		fclose(fp);

	if (fd)
		ret = dhd_set(dhd, DHD_DLDN_END, NULL, 0);

	return ret;
#endif /* BWL_FILESYSTEM_SUPPORT */
}

/** keep the 'proprietary 16 bits' entries in this enum in sync with array d11ft2str[] */
enum file_type_e {
	FILE_TYPE_IHR,       /** BRCM proprietary 16 bits regdump format */
	FILE_TYPE_SCR,       /** BRCM proprietary 16 bits regdump format */
	FILE_TYPE_SHM,       /** BRCM proprietary 16 bits regdump format */
	FILE_TYPE_TPL,       /** BRCM proprietary 16 bits regdump format */
	FILE_TYPE_KEYTB,     /** BRCM proprietary 16 bits regdump format */
	FILE_TYPE_IHRX,      /** BRCM proprietary 16 bits regdump format */
	FILE_TYPE_SCRX,      /** BRCM proprietary 16 bits regdump format */
	FILE_TYPE_SHMX,      /** BRCM proprietary 16 bits regdump format */
	FILE_TYPE_SREC,      /** Motorola S-record, to be read by e.g. a gdb client */
	FILE_TYPE_ACPHY_DMP  /** BRCM proprietary phy reg dump format */
};

/** keep this array in sync with enum file_type_e */
char *d11ft2str[] = {
	"ihr",    /**< FILE_TYPE_IHR */
	"scr",    /**< FILE_TYPE_SCR */
	"shm",    /**< FILE_TYPE_SHM */
	"tpl",    /**< FILE_TYPE_TPL */
	"keytb",  /**< FILE_TYPE_KEYTB */
	"ihrx",   /**< FILE_TYPE_IHRX */
	"scrx",   /**< FILE_TYPE_SCRX */
	"shmx"    /**< FILE_TYPE_SHMX */
};

/**
 * Writes register or memory contents of e.g. ChipCommon core into a Motorola S-record file for
 * offline firmware debugging using gdb.
 *
 * @param file_path   File to create
 * @param typ         DUMP_DONGLE_COREREG or DUMP_DONGLE_D11MEM_SHM or ...
 * @param ft          e.g. FILE_TYPE_SREC
 * @param idx_to_dump for regs: 0:dump all cores, >0: dump specific core. For mem: 0: first mem.
 */
static int
dhd_upload_core(void *dhd, char *file_path, enum dump_dongle_e typ, enum file_type_e ft,
	int idx_to_dump)
{
#if !defined(BWL_FILESYSTEM_SUPPORT)
	return (-1);
#else
	int ret = 0;
	dump_dongle_in_t ddi;    /**< from utility to driver */
	dump_dongle_out_t *ddo;  /**< from driver to utility */
	FILE *fp;
	uint8 bbuf[256];         /**< contains one Motorola S-record line */
	int n_srec_bytes;        /**< number of bytes in Motorola S-record line following header */
	int srec_len;
	uint8 srec_checksum;     /**< Motorola S-record */
	int idx = 0;

	switch (typ) {
		case DUMP_DONGLE_COREREG:
		case DUMP_DONGLE_D11MEM_SHM:
		case DUMP_DONGLE_D11MEM_SCR:
		case DUMP_DONGLE_D11IHR:
		case DUMP_DONGLE_D11MEM_AMT:
		case DUMP_DONGLE_PHYREGS:
			break;
		default:
			printf("%s unknown type %d\n", __FUNCTION__, typ);
			return BCME_ERROR;
	}

	ddi.type = typ;
	ddi.index = idx_to_dump;
	fp = fopen(file_path, "w");
	if (fp == NULL) {
		fprintf(stderr, "could not open file %s\n", file_path);
		return BCME_ERROR;
	}

	do { // iterate over cores
		ddi.byte_offset = 0;
		do { // iterate over single register set or single memory
			ret = dhd_var_getbuf(dhd, "dump_dongle", &ddi, sizeof(ddi), (void**)&ddo);
			if (ret != BCME_OK) {
				break;
			}
			if (typ == DUMP_DONGLE_COREREG &&
				(ddo->address & 0xFFFF0000) != 0x18000000) {
				ddo->n_bytes = 0; // 43602 contains cores with unexpected addresses
			}
			if (ddo->n_bytes > 0) {
				int i, j;
				srec_len = 4 + 1 + ddo->element_width; /* 4 bytes addr, 1 crc */
				/* the for loop prints one output file line per array element */
				for (i = 0; i < ddo->n_bytes / ddo->element_width; i++) {
					n_srec_bytes = 0;
					bbuf[n_srec_bytes++] = srec_len;
					*((uint32*)&bbuf[n_srec_bytes]) = BCMSWAP32(ddo->address);
					n_srec_bytes += 4; /* 4 address bytes */
					switch (ddo->element_width) {
					case sizeof(uint16):
						*((uint16*)&bbuf[n_srec_bytes]) = ddo->u.ui16[i];
						break;
					case sizeof(uint32):
						*((uint32*)&bbuf[n_srec_bytes]) = ddo->u.ui32[i];
						break;
					case sizeof(uint64):
						*((uint64*)&bbuf[n_srec_bytes]) = ddo->u.ui64[i];
						break;
					}
					n_srec_bytes += ddo->element_width;
					srec_checksum = 0;
					switch (ft) { // different output file formats
					case FILE_TYPE_SREC:
						fprintf(fp, "S3"); // Motorola S3 record
						for (j = 0; j < n_srec_bytes; j++) {
							fprintf(fp, "%02X", bbuf[j]);
							srec_checksum += bbuf[j];
						}
						srec_checksum = ~srec_checksum;
						fprintf(fp, "%02X\n", srec_checksum);
						break;
					case FILE_TYPE_ACPHY_DMP:
						fprintf(fp, "%s: 0x%03x 0x%04x\n", "aphy",
							ddo->address, ddo->u.ui16[i]);
						break;
					case FILE_TYPE_IHR:
					case FILE_TYPE_SCR:
					case FILE_TYPE_SHM:
					case FILE_TYPE_SHMX:
					case FILE_TYPE_TPL:
					case FILE_TYPE_KEYTB:
					case FILE_TYPE_IHRX:
					case FILE_TYPE_SCRX:
						fprintf(fp, "%-3d %s 0x%-4x = 0x%-4x\n",
							idx, d11ft2str[ft], ddo->address,
							ddo->u.ui16[i]);
						break;
					}
					ddo->address += ddo->element_width;
					idx++;
				}
			}
			ddi.byte_offset += ddo->n_bytes;
		} while (ddo->n_bytes > 0);
		ddi.index++;
	} while (ret == BCME_OK && (
		(typ == DUMP_DONGLE_COREREG && (ddo->id == 0 || ddi.byte_offset > 0)) ||
		(typ != DUMP_DONGLE_COREREG && ddo->n_bytes > 0)));
	fclose(fp);

	return ret;
#endif /* BWL_FILESYSTEM_SUPPORT */
} /* dhd_upload_core */

/**
 * if the option '-r 0' is used, dongle register contents is dumped, and the ARM on the dongle is
 * reset.
 */
static int
dhd_upload(void *dhd, cmd_t *cmd, char **argv)
{
#if !defined(BWL_FILESYSTEM_SUPPORT)
	return (-1);
#else
	char *fname = NULL;
	uint32 start;
	uint32 size = 0;
	int ret = 0;
	FILE *fp;
	uint32 ramsize;
	char *memszargs[] = { "ramsize", NULL };
	char file_path[80];
	uint len;
	miniopt_t opts;
	int opt_err;
	int coreidx_to_dump = -1; /**< -1:no upload, 0:upload all, >0: upload specific core */

	UNUSED_PARAMETER(cmd);
	UNUSED_PARAMETER(argv);

	if ((ret = dhd_get_ramstart(dhd, &start)) != BCME_OK)
		goto exit;

	/* Parse command-line options */
	miniopt_init(&opts, "upload", "", TRUE);

	argv++;
	while ((opt_err = miniopt(&opts, argv)) != -1) {
		if (opt_err == 1) {
			fprintf(stderr, "upload options error\n");
			ret = -1;
			goto exit;
		}
		argv += opts.consumed;

		if (opts.opt == 'a') {
			if (!opts.good_int) {
				fprintf(stderr, "invalid address %s\n", opts.valstr);
				ret = -1;
				goto exit;
			}
			start = (uint32)opts.uval;
		} else if (opts.opt == 's') {
			if (!opts.good_int) {
				fprintf(stderr, "invalid RAM size %s\n", opts.valstr);
				ret = -1;
				goto exit;
			}
			size = (uint32)opts.uval;
		} else if (opts.opt == 'r') {
			if (!opts.good_int) {
				fprintf(stderr, "invalid core index %s\n", opts.valstr);
				ret = -1;
				goto exit;
			}
			coreidx_to_dump = (uint32)opts.uval;
		} else if (opts.positional) {
			if (!fname) {
				fname = opts.valstr;
			} else {
				fprintf(stderr, "upload options error\n");
				ret = -1;
				goto exit;
			}
		} else if (!opts.opt) {
			fprintf(stderr, "unrecognized option %s\n", opts.valstr);
			ret = -1;
			goto exit;
		} else {
			fprintf(stderr, "unrecognized option %c\n", opts.opt);
			ret = -1;
			goto exit;
		}
	}

	/* validate arguments */
	if (!fname) {
		fprintf(stderr, "filename required\n");
		ret = -1;
		goto exit;
	}

	if ((ret = dhd_var_get(dhd, NULL, memszargs))) {
		fprintf(stderr, "%s: error obtaining ramsize\n", __FUNCTION__);
		goto exit;
	}
	ramsize = *(uint32*)buf;

	if (!ramsize)
		ramsize = size;

	strncpy(file_path, fname, sizeof(file_path));
	if ((fp = fopen(file_path, "wb")) == NULL) {
		fprintf(stderr, "%s: Could not open %s: %s\n",
		        __FUNCTION__, file_path, strerror(errno));
		ret = -1;
		goto exit;
	}

	/* default size to full RAM */
	if (!size)
		size = ramsize;

	if (coreidx_to_dump != -1) {
		dhd_iovar_setint(dhd, "halt_dongle", 1); // for consistent memory / regs dumps
	}

	/* read memory and write to file */
	while (size) {
		char *ptr;
		int params[2];

		len = MIN(MEMBLOCK, size);

		params[0] = start;
		params[1] = len;
		ret = dhd_var_getbuf(dhd, "membytes", params, 2 * sizeof(int), (void**)&ptr);
		if (ret) {
			fprintf(stderr, "%s: failed reading %d membytes from 0x%08x\n",
			        __FUNCTION__, len, start);
			break;
		}

		if (fwrite(ptr, sizeof(char), len, fp) != len) {
			fprintf(stderr, "%s: error writing to file %s\n", __FUNCTION__, fname);
			ret = -1;
			break;
		}

		start += len;
		size -= len;
	}

	fclose(fp);

	if (ret == BCME_OK && coreidx_to_dump != -1) {
		int i;
		const struct {
			char *ext;               /**< file_extension */
			enum dump_dongle_e typ;  /**< what type of dump */
			enum file_type_e ft;     /**< output file type */
			int idx;                 /**< index of entity to dump */
		} files_to_generate[] = {
			{".core_regs", DUMP_DONGLE_COREREG, FILE_TYPE_SREC, 0},
			{".shm", DUMP_DONGLE_D11MEM_SHM, FILE_TYPE_SHM, 0},
			{".scr", DUMP_DONGLE_D11MEM_SCR, FILE_TYPE_SCR, 0},
			{".ihr", DUMP_DONGLE_D11IHR, FILE_TYPE_IHR, 0},
			{".amt", DUMP_DONGLE_D11MEM_AMT, FILE_TYPE_SREC, 0},
			{".phy_regs", DUMP_DONGLE_PHYREGS, FILE_TYPE_ACPHY_DMP, 0},
			{".shmx", DUMP_DONGLE_D11MEM_SHM, FILE_TYPE_SHMX, 1},
			{".scrx", DUMP_DONGLE_D11MEM_SCR, FILE_TYPE_SCRX, 1},
			{".ihrx", DUMP_DONGLE_D11IHR, FILE_TYPE_IHRX, 1},
		};
		for (i = 0; i < ARRAYSIZE(files_to_generate); i++) {
			strncpy(file_path, fname, sizeof(file_path));
			strncat(file_path, files_to_generate[i].ext, sizeof(file_path));
			ret = dhd_upload_core(dhd, file_path, files_to_generate[i].typ,
				files_to_generate[i].ft, files_to_generate[i].idx);
			if (ret != BCME_OK) {
				printf("failed to dump %s\n", file_path);
				break;
			}
		}
	}

exit:
	return ret;
#endif /* BWL_FILESYSTEM_SUPPORT */
} /* dhd_upload */

#ifdef BWL_FILESYSTEM_SUPPORT
static int
dhd_get_debug_info(void *dhd, hnd_debug_t *debug_info)
{
	int i;
	int ret;
	int params[2];

	uint32 *buffer;
	uint32 ramstart;
	hnd_debug_ptr_t *debugPtr;

	if ((ret = dhd_get_ramstart(dhd, &ramstart)) != BCME_OK)
		return ret;

	/*
	 * Different chips have different fixed debug_info_ptrs
	 * because of different ROM locations/uses.  Try them all looking
	 * for the magic number.
	 */
	for (i = 0; ; i++) {
		if (dump_info_ptr_ptr[i] == DUMP_INFO_PTR_PTR_END) {
			fprintf(stderr, "Error: cannot find pointer to debug_info\n");
			return -1;
		}

		params[0] = dump_info_ptr_ptr[i] + ramstart;
		params[1] = sizeof(hnd_debug_ptr_t);
		ret = dhd_var_getbuf(dhd, "membytes", params, 2 * sizeof(int), (void**)&debugPtr);
		if ((ret == 0) &&
		    (debugPtr->magic == HND_DEBUG_PTR_PTR_MAGIC))
			break;
	}

	if (debugPtr->hnd_debug_addr == 0) {
		fprintf(stderr, "Error: Debug info pointer is zero\n");
		return -1;
	}

	/* Read the area the debuginfoptr points at */
	params[0] = debugPtr->hnd_debug_addr;
	params[1] = sizeof(hnd_debug_t);
	ret = dhd_var_getbuf(dhd, "membytes", params, 2 * sizeof(int), (void**)&buffer);
	if (ret) {
		fprintf(stderr, "%s: failed reading %lu membytes from 0x%08lx\n",
			__FUNCTION__, (long unsigned) params[1], (long unsigned) params[0]);
		return ret;
	}

	memcpy((char *) debug_info, buffer, sizeof(hnd_debug_t));

	/* Sanity check the area */
	if ((debug_info->magic != HND_DEBUG_MAGIC) ||
	    (debug_info->version != HND_DEBUG_VERSION)) {
		fprintf(stderr, "Error: Invalid debug info area\n");
		return -1;
	}

	return 0;
}
#endif /* BWL_FILESYSTEM_SUPPORT */

static int
dhd_coredump(void *dhd, cmd_t *cmd, char **argv)
{
#if !defined(BWL_FILESYSTEM_SUPPORT)
	return (-1);
#else
	char *fname = NULL;
	int ret;

	FILE *fp;

	hnd_debug_t debugInfo;

	miniopt_t opts;
	int opt_err;

	int params[2];
	char *ptr;

	unsigned int start;
	unsigned int size;

	prstatus_t prstatus;

	UNUSED_PARAMETER(cmd);
	UNUSED_PARAMETER(argv);

	/* Parse command-line options */
	miniopt_init(&opts, "dump", "", TRUE);

	argv++;
	while ((opt_err = miniopt(&opts, argv)) != -1) {
		if (opt_err == 1) {
			fprintf(stderr, "dump options error\n");
			ret = -1;
			goto exit;
		}
		argv += opts.consumed;

		if (opts.positional) {
			if (!fname) {
				fname = opts.valstr;
			} else {
				fprintf(stderr, "dump options error\n");
				ret = -1;
				goto exit;
			}
		} else if (!opts.opt) {
			fprintf(stderr, "unrecognized option %s\n", opts.valstr);
			ret = -1;
			goto exit;
		} else {
			fprintf(stderr, "unrecognized option %c\n", opts.opt);
			ret = -1;
			goto exit;
		}
	}

	/* validate arguments */
	if (!fname) {
		fprintf(stderr, "filename required\n");
		ret = -1;
		goto exit;
	}

	if ((ret = dhd_get_debug_info(dhd, &debugInfo)) < 0)
		goto exit;

	/* Get the base and size to dump */
	start = debugInfo.ram_base;
	size = debugInfo.ram_size;

	/* Get the arm trap area */
	bzero(&prstatus, sizeof(prstatus_t));
	if (debugInfo.trap_ptr != 0) {
		int i;
		trap_t armtrap;
		uint32 *reg;

		params[0] = debugInfo.trap_ptr;
		params[1] = sizeof(trap_t);
		ret = dhd_var_getbuf(dhd, "membytes", params, 2 * sizeof(int), (void**)&ptr);
		if (ret) {
			fprintf(stderr, "%s: failed reading %lu membytes from 0x%08lx\n",
				__FUNCTION__, (long unsigned) params[1], (long unsigned) params[0]);
			goto exit;
		}

		memcpy((char *) &armtrap, ptr, sizeof(trap_t));

		/* Populate the prstatus */
		prstatus.si_signo = armtrap.type;
		reg = &armtrap.r0;
		for (i = 0; i < 15; i++, reg++) {
			prstatus.uregs[i] = *reg;
		}
		prstatus.uregs[15] = armtrap.epc;
	}

	if ((fp = fopen(fname, "wb")) == NULL) {
		fprintf(stderr, "%s: Could not open %s: %s\n",
		        __FUNCTION__, fname, strerror(errno));
		ret = -1;
		goto exit;
	}

	/* Write the preamble and debug header */
	fprintf(fp, "Dump starts for version %s FWID 01-%x\n", debugInfo.epivers, debugInfo.fwid);
	fprintf(fp, "XXXXXXXXXXXXXXXXXXXX");
	fprintf(fp, "%8.8lX", (long unsigned) sizeof(debugInfo));
	if (fwrite(&debugInfo, sizeof(unsigned char), sizeof(debugInfo), fp) != sizeof(debugInfo)) {
		fprintf(stderr, "%s: error writing to file %s\n", __FUNCTION__, fname);
		ret = -1;
		fclose(fp);
		goto exit;
	}

	/* Write the prstatus */
	if (fwrite(&prstatus, sizeof(unsigned char), sizeof(prstatus), fp) != sizeof(prstatus)) {
		fprintf(stderr, "%s: error writing to file %s\n", __FUNCTION__, fname);
		ret = -1;
		fclose(fp);
		goto exit;
	}

	/* Write the ram size as another sanity check */
	fprintf(fp, "%8.8X", size);

	/* read memory and write to file */
	while (size) {
		int len;
		len = MIN(MEMBLOCK, size);

		params[0] = start;
		params[1] = len;
		ret = dhd_var_getbuf(dhd, "membytes", params, 2 * sizeof(int), (void**)&ptr);
		if (ret) {
			fprintf(stderr, "%s: failed reading %d membytes from 0x%08x\n",
			        __FUNCTION__, len, start);
			break;
		}

		if (fwrite(ptr, sizeof(char), len, fp) != (uint) len) {
			fprintf(stderr, "%s: error writing to file %s\n", __FUNCTION__, fname);
			ret = -1;
			break;
		}

		start += len;
		size -= len;
	}

	fclose(fp);
exit:
	return ret;
#endif /* BWL_FILESYSTEM_SUPPORT */
}

static int
dhd_consoledump(void *dhd, cmd_t *cmd, char **argv)
{
#if !defined(BWL_FILESYSTEM_SUPPORT)
	return (-1);
#else
	int ret;

	hnd_debug_t debugInfo;

	miniopt_t opts;
	int opt_err;

	int params[2];
	char *ptr;

	unsigned int start;
	unsigned int size;
	int len;

	UNUSED_PARAMETER(cmd);
	UNUSED_PARAMETER(argv);

	/* Parse command-line options */
	miniopt_init(&opts, "consoledump", "", TRUE);

	argv++;
	while ((opt_err = miniopt(&opts, argv)) != -1) {
		if (opt_err == 1) {
			fprintf(stderr, "dump options error\n");
			ret = -1;
			goto exit;
		}
		argv += opts.consumed;

		if (!opts.opt) {
			fprintf(stderr, "unrecognized option %s\n", opts.valstr);
			ret = -1;
			goto exit;
		} else {
			fprintf(stderr, "unrecognized option %c\n", opts.opt);
			ret = -1;
			goto exit;
		}
	}

	if ((ret = dhd_get_debug_info(dhd, &debugInfo)) < 0)
		goto exit;

	if (debugInfo.console <= debugInfo.ram_base) {
		fprintf(stderr, "%s: console not found\n", __FUNCTION__);
		ret = -1;
		goto exit;
	}

	/* Get the debug console area */
	params[0] = debugInfo.console;
	params[1] = sizeof(hnd_cons_t);
	ret = dhd_var_getbuf(dhd, "membytes", params, 2 * sizeof(int), (void**)&ptr);
	if (ret) {
		fprintf(stderr, "%s: failed reading %lu membytes from 0x%08lx\n",
			__FUNCTION__, (long unsigned) params[1], (long unsigned) params[0]);
		goto exit;
	}

	if (ptr == NULL) {
		fprintf(stderr, "%s: console not initialised\n", __FUNCTION__);
		ret = -1;
		goto exit;
	}

	start = (unsigned int)((hnd_cons_t *)ptr)->log.buf;
	size = ((hnd_cons_t *)ptr)->log.buf_size;

	if (start <= debugInfo.ram_base) {
		fprintf(stderr, "%s: console buffer not initialised\n", __FUNCTION__);
		ret = -1;
		goto exit;
	}

	/* read memory and write to file */
	while (size > 0) {
		len = MIN(MEMBLOCK, size);

		params[0] = start;
		params[1] = len;
		ret = dhd_var_getbuf(dhd, "membytes", params, 2 * sizeof(int), (void**)&ptr);
		if (ret) {
			fprintf(stderr, "%s: failed reading %d membytes from 0x%08x\n",
			        __FUNCTION__, len, start);
			break;
		}

		printf("%s", ptr);

		start += len;
		size -= len;
	}

exit:
	return ret;
#endif /* BWL_FILESYSTEM_SUPPORT */
}

static int
dhd_logstamp(void *dhd, cmd_t *cmd, char **argv)
{
	int ret;
	char *endptr = NULL;
	uint argc;
	int valn[2] = {0, 0};

	/* arg count */
	for (argc = 0; argv[argc]; argc++);
	argc--; argv++;

	if (argc > 2)
		return BCME_USAGE_ERROR;

	if (argc) {
		valn[0] = strtol(argv[0], &endptr, 0);
		if (*endptr != '\0') {
			printf("bad val1: %s\n", argv[0]);
			return BCME_USAGE_ERROR;
		}
	}

	if (argc > 1) {
		valn[1] = strtol(argv[1], &endptr, 0);
		if (*endptr != '\0') {
			printf("bad val2: %s\n", argv[1]);
			return BCME_USAGE_ERROR;
		}
	}

	ret = dhd_var_setbuf(dhd, cmd->name, valn, argc * sizeof(int));

	return (ret);
}

#define MAXPD11REGSCNT 32
static int
dhd_pd11regs_bylist(void *dhd, d11regs_list_t *reglist, uint16 reglist_sz, bool verbose)
{
	uint i;
	int err = BCME_OK;
	dhd_pd11regs_buf *pd11regsbuf;
	uint16 param_sz = OFFSETOF(dhd_pd11regs_param, plist) + sizeof(*reglist);
	char param_buf[param_sz];
	dhd_pd11regs_param *param = (void *)param_buf;

	param->start_idx = 0;
	param->verbose = verbose;

	if (reglist != NULL && reglist_sz > 0) {
		for (i = 0; i < reglist_sz; i++) {
			if (reglist[i].cnt == 0) {
				memcpy(param->plist, &reglist[i], sizeof(*reglist));
				err = dhd_var_getbuf(dhd, "pd11regs", param, param_sz,
					(void **)&pd11regsbuf);
				if (err != BCME_OK) {
					printf("Error %d %d\n", err, pd11regsbuf->idx);
					goto exit;
				}
				param->start_idx += pd11regsbuf->idx;
				printf("%s", pd11regsbuf->pbuf);
			} else {
				uint16 rem_cnt;
				rem_cnt = reglist[i].cnt;
				while (rem_cnt > 0) {
					reglist[i].cnt = MIN(MAXPD11REGSCNT, rem_cnt);
					memcpy(param->plist, &reglist[i], sizeof(*reglist));
					err = dhd_var_getbuf(dhd, "pd11regs", param, param_sz,
						(void **)&pd11regsbuf);
					if (err != BCME_OK) {
						printf("Error %d\n", err);
						goto exit;
					}
					if ((pd11regsbuf->idx == 0) ||
						(pd11regsbuf->idx != reglist[i].cnt)) {
						err = BCME_ERROR;
						goto exit;
					}
					printf("%s", pd11regsbuf->pbuf);
					reglist[i].addr += reglist[i].step * reglist[i].cnt;
					rem_cnt -= reglist[i].cnt;
					param->start_idx += reglist[i].cnt;
				}
			}
		}
	}
exit:
	return err;
}

static int
dhd_pmac(void *dhd, cmd_t *cmd, char **argv)
{
	int err = BCME_OK;
	d11regs_list_t reglist = {0, };
	char *p, opt;
	bool verbose = FALSE;

	/* Skip the iovar name and get the type */
	if ((p = *(++argv)) == NULL) {
		err = BCME_USAGE_ERROR;
		goto exit;
	}

	if (!strncmp(p, "scrx", 4)) {
		reglist.type = D11REG_TYPE_SCRX;
		reglist.step = 2;
	} else if (!strncmp(p, "shmx", 4)) {
		reglist.type = D11REG_TYPE_SHMX;
		reglist.step = 2;
	} else if (!strncmp(p, "ihrx32", 6)) {
		/* ihrx32 reads the same space as ihr32 */
		reglist.type = D11REG_TYPE_IHR32;
		reglist.step = 4;
	} else if (!strncmp(p, "ihrx", 4)) {
		reglist.type = D11REG_TYPE_IHRX16;
		reglist.step = 2;
	} else if (!strncmp(p, "scr", 3)) {
		reglist.type = D11REG_TYPE_SCR;
		reglist.step = 1;
	} else if (!strncmp(p, "shm", 3)) {
		reglist.type = D11REG_TYPE_SHM;
		reglist.step = 2;
	} else if (!strncmp(p, "ihr32", 5)) {
		reglist.type = D11REG_TYPE_IHR32;
		reglist.step = 4;
	} else if (!strncmp(p, "ihr", 3)) {
		reglist.type = D11REG_TYPE_IHR16;
		reglist.step = 2;
	} else {
		printf("Invalid selection!!\n");
		err = BCME_USAGE_ERROR;
		goto exit;
	}

	/* skip the type and get the address */
	if ((p = *(++argv)) == NULL) {
		err = BCME_USAGE_ERROR;
		goto exit;
	}
	reglist.addr = strtol(p, NULL, 0);
	/* default cnt 1 in case there is no argument given */
	reglist.cnt = 1;

	/* skip the address */
	argv++;

	/* skip the address and process additional arguments */
	while ((p = *argv)) {
		argv++;
		if (!strncmp(p, "-", 1)) {
			if (strlen(p) > 2 || (p[1] != 'v' && *argv == NULL)) {
				err = BCME_USAGE_ERROR;
				goto exit;
			}
			opt = p[1];

			switch (opt) {
				case 's':
					reglist.step = strtol(*argv, NULL, 0);
					argv++;
					break;
				case 'n':
					reglist.cnt = strtol(*argv, NULL, 0);
					argv++;
					break;
				case 'b':
					reglist.bitmap = strtoul(*argv, NULL, 0);
					argv++;
					break;
				case 'v':
					verbose = TRUE;
					break;
				default:
					printf("Invalid option!!\n");
					err = BCME_USAGE_ERROR;
					goto exit;
			}
		} else {
			printf("Invalid argument %s!!\n", p);
			err = BCME_USAGE_ERROR;
			goto exit;
		}
	}

	err = dhd_pd11regs_bylist(dhd, &reglist, 1, verbose);
exit:
	return err;
}

static int
dhd_dump_mac(void *dhd, cmd_t *cmd, char **argv)
{
	int err = BCME_OK;
	char *p, opt;
	dhd_maclist_t *macreglist;
	d11regs_list_t *reglist;
	void *reglists = NULL;
	uint16 reglists_blen, reglist_sz;
	bool verbose = FALSE, killarm = FALSE;
	bool dump_psmx = FALSE;

	/* skip the iovar name */
	argv++;

	/* process additional arguments */
	while ((p = *argv)) {
		argv++;
		if (!strncmp(p, "-", 1)) {
			if (strlen(p) > 2) {
				err = BCME_USAGE_ERROR;
				goto exit;
			}
			opt = p[1];

			switch (opt) {
				case 'x':
					dump_psmx = TRUE;
					break;
				case 'v':
					verbose = TRUE;
					break;
				case 'k':
					killarm = TRUE;
					break;
				default:
					printf("Invalid option!!\n");
					err = BCME_USAGE_ERROR;
					goto exit;
			}
		} else {
			printf("Invalid argument %s!!\n", p);
			err = BCME_USAGE_ERROR;
			goto exit;
		}
	}

	if (killarm) {
		dhd_var_setbuf(dhd, "pcie_device_trap", NULL, 0);
	}

	err = dhd_var_getbuf(dhd, "mreglist", NULL, 0, (void **)&macreglist);
	if (err != BCME_OK) {
		printf("mreglist error %d\n", err);
		goto exit;
	}

	reglists_blen = macreglist->bytes_len;
	reglists = malloc(reglists_blen);
	if (reglists == NULL) {
		printf("NOMEM for size %d\n", reglists_blen);
		err = BCME_NOMEM;
		goto exit;
	}
	memcpy(reglists, macreglist->plist, reglists_blen);

	if (!dump_psmx) {
		/* Dump R */
		reglist = bcm_get_data_from_xtlv_buf(reglists, reglists_blen,
			DHD_MACLIST_XTLV_R, &reglist_sz, BCM_XTLV_OPTION_ALIGN32);
		reglist_sz /= sizeof(*reglist);
	} else {
		/* Dump X */
		reglist = bcm_get_data_from_xtlv_buf(reglists, reglists_blen,
			DHD_MACLIST_XTLV_X, &reglist_sz, BCM_XTLV_OPTION_ALIGN32);
		reglist_sz /= sizeof(*reglist);
	}

	err = dhd_pd11regs_bylist(dhd, reglist, reglist_sz, verbose);
exit:
	if (reglists) {
		free(reglists);
	}
	return err;
}

#define MAXPSVMPSCNT 32
static int
dhd_psvmps_bylist(void *dhd, svmp_list_t *reglist, uint16 reglist_sz, bool verbose)
{
	uint i;
	int err = BCME_OK;
	dhd_pd11regs_buf *psvmpssbuf;
	uint16 param_sz = OFFSETOF(dhd_pd11regs_param, plist) + sizeof(*reglist);
	char param_buf[param_sz];
	dhd_pd11regs_param *param = (void *)param_buf;

	param->start_idx = 0;
	param->verbose = verbose;

	if (reglist != NULL && reglist_sz > 0) {
		for (i = 0; i < reglist_sz; i++) {
			uint16 rem_cnt;
			rem_cnt = reglist[i].cnt;
			while (rem_cnt > 0) {
				reglist[i].cnt = MIN(MAXPSVMPSCNT, rem_cnt);
				memcpy(param->plist, &reglist[i], sizeof(*reglist));
				err = dhd_var_getbuf(dhd, "psvmpmems", param, param_sz,
					(void **)&psvmpssbuf);
				if (err != BCME_OK) {
					printf("Error %d\n", err);
					goto exit;
				}
				if ((psvmpssbuf->idx == 0) ||
					(psvmpssbuf->idx != reglist[i].cnt)) {
					err = BCME_ERROR;
					goto exit;
				}
				printf("%s", psvmpssbuf->pbuf);
				reglist[i].addr += reglist[i].cnt;
				rem_cnt -= reglist[i].cnt;
				param->start_idx += reglist[i].cnt;
			}
		}
	}
exit:
	return err;
}

static int
dhd_psvmp(void *dhd, cmd_t *cmd, char **argv)
{
	int err = BCME_OK;
	char *p, opt;
	bool verbose = FALSE;
	svmp_list_t svmplist;

	/* skip the iovar name and get the address */
	if ((p = *(++argv)) == NULL) {
		err = BCME_USAGE_ERROR;
		goto exit;
	}
	svmplist.addr = strtol(p, NULL, 0);
	/* default cnt 1 in case there is no argument given */
	svmplist.cnt = 1;

	/* skip the address */
	argv++;

	/* process additional arguments */
	while ((p = *argv)) {
		argv++;
		if (!strncmp(p, "-", 1)) {
			if (strlen(p) > 2) {
				err = BCME_USAGE_ERROR;
				goto exit;
			}
			opt = p[1];

			switch (opt) {
				case 'n':
					svmplist.cnt = strtol(*argv, NULL, 0);
					argv++;
					break;
				case 'v':
					verbose = TRUE;
					break;
				default:
					printf("Invalid option!!\n");
					err = BCME_USAGE_ERROR;
					goto exit;
			}
		} else {
			printf("Invalid argument %s!!\n", p);
			err = BCME_USAGE_ERROR;
			goto exit;
		}
	}

	err = dhd_psvmps_bylist(dhd, &svmplist, 1, verbose);
exit:
	return err;
}

static int
dhd_dump_svmp(void *dhd, cmd_t *cmd, char **argv)
{
	int err = BCME_OK;
	char *p, opt;
	dhd_maclist_t *macreglist;
	svmp_list_t *svmplist;
	void *lists = NULL;
	uint16 lists_blen, svmplist_sz;
	bool verbose = FALSE, killarm = FALSE;

	/* skip the iovar name */
	argv++;

	/* process additional arguments */
	while ((p = *argv)) {
		argv++;
		if (!strncmp(p, "-", 1)) {
			if (strlen(p) > 2) {
				err = BCME_USAGE_ERROR;
				goto exit;
			}
			opt = p[1];

			switch (opt) {
				case 'v':
					verbose = TRUE;
					break;
				case 'k':
					killarm = TRUE;
					break;
				default:
					printf("Invalid option!!\n");
					err = BCME_USAGE_ERROR;
					goto exit;
			}
		} else {
			printf("Invalid argument %s!!\n", p);
			err = BCME_USAGE_ERROR;
			goto exit;
		}
	}

	if (killarm) {
		dhd_var_setbuf(dhd, "pcie_device_trap", NULL, 0);
	}

	err = dhd_var_getbuf(dhd, "mreglist", NULL, 0, (void **)&macreglist);
	if (err != BCME_OK) {
		printf("mreglist error %d\n", err);
		goto exit;
	}

	lists_blen = macreglist->bytes_len;
	lists = malloc(lists_blen);
	if (lists == NULL) {
		printf("NOMEM for size %d\n", lists_blen);
		err = BCME_NOMEM;
		goto exit;
	}
	memcpy(lists, macreglist->plist, lists_blen);

	svmplist = bcm_get_data_from_xtlv_buf(lists, lists_blen,
		DHD_SVMPLIST_XTLV, &svmplist_sz, BCM_XTLV_OPTION_ALIGN32);
	svmplist_sz /= sizeof(*svmplist);

	err = dhd_psvmps_bylist(dhd, svmplist, svmplist_sz, verbose);
exit:
	if (lists) {
		free(lists);
	}
	return err;
}

static int
dhd_sd_reg(void *dhd, cmd_t *cmd, char **argv)
{
	int ret;
	sdreg_t sdreg;
	char *endptr = NULL;
	uint argc;
	void *ptr = NULL;

	bzero(&sdreg, sizeof(sdreg));

	/* arg count */
	for (argc = 0; argv[argc]; argc++);
	argc--;

	/* hostreg: offset [value]; devreg: func offset [value] */
	if (!strcmp(cmd->name, "sd_hostreg")) {
		argv++;
		if (argc < 1) {
			printf("required args: offset [value]\n");
			return BCME_USAGE_ERROR;
		}

	} else if (!strcmp(cmd->name, "sd_devreg")) {
		argv++;
		if (argc < 2) {
			printf("required args: func offset [value]\n");
			return BCME_USAGE_ERROR;
		}

		sdreg.func = strtoul(*argv++, &endptr, 0);
		if (*endptr != '\0') {
			printf("Invalid function number\n");
			return BCME_USAGE_ERROR;
		}
	} else {
		return BCME_USAGE_ERROR;
	}

	sdreg.offset = strtoul(*argv++, &endptr, 0);
	if (*endptr != '\0') {
		printf("Invalid offset value\n");
		return BCME_USAGE_ERROR;
	}

	/* third arg: value */
	if (*argv) {
		sdreg.value = strtoul(*argv, &endptr, 0);
		if (*endptr != '\0') {
			printf("Invalid value\n");
			return BCME_USAGE_ERROR;
		}
	}

	/* no third arg means get, otherwise set */
	if (!*argv) {
		if ((ret = dhd_var_getbuf(dhd, cmd->name, &sdreg, sizeof(sdreg), &ptr)) >= 0)
			printf("0x%x\n", *(int *)ptr);
	} else {
		ret = dhd_var_setbuf(dhd, cmd->name, &sdreg, sizeof(sdreg));
	}

	return (ret);
}

static dbg_msg_t dhd_msgs[] = {
	{DHD_ERROR_VAL,	"error"},
	{DHD_ERROR_VAL, "err"},
	{DHD_TRACE_VAL, "trace"},
	{DHD_INFO_VAL,	"inform"},
	{DHD_INFO_VAL,	"info"},
	{DHD_INFO_VAL,	"inf"},
	{DHD_DATA_VAL,	"data"},
	{DHD_CTL_VAL,	"ctl"},
	{DHD_TIMER_VAL,	"timer"},
	{DHD_HDRS_VAL,	"hdrs"},
	{DHD_BYTES_VAL,	"bytes"},
	{DHD_INTR_VAL,	"intr"},
	{DHD_LOG_VAL,	"log"},
	{DHD_GLOM_VAL,	"glom"},
	{DHD_EVENT_VAL,	"event"},
	{DHD_BTA_VAL,	"bta"},
	{DHD_ISCAN_VAL,	"iscan"},
	{DHD_ARPOE_VAL,	"arpoe"},
	{DHD_REORDER_VAL, "reorder"},
	{DHD_DNGL_IOVAR_GET_VAL, "g_iov"},
	{DHD_DNGL_IOVAR_SET_VAL, "s_iov"},
	{0,		NULL}
};

static int
dhd_msglevel(void *dhd, cmd_t *cmd, char **argv)
{
	return dhd_do_msglevel(dhd, cmd, argv, dhd_msgs);
}

static int
dhd_do_msglevel(void *dhd, cmd_t *cmd, char **argv, dbg_msg_t *dbg_msg)
{
	int ret, i;
	uint val, last_val = 0, msglevel = 0, msglevel_add = 0, msglevel_del = 0;
	char *endptr = NULL;

	if ((ret = dhd_iovar_getint(dhd, cmd->name, (int*)&msglevel)) < 0)
		return (ret);

	if (!*++argv) {
		printf("0x%x ", msglevel);
		for (i = 0; (val = dbg_msg[i].value); i++) {
			if ((msglevel & val) && (val != last_val))
				printf(" %s", dbg_msg[i].string);
			last_val = val;
		}
		printf("\n");
		return (0);
	}

	while (*argv) {
		char *s = *argv;
		if (*s == '+' || *s == '-')
			s++;
		else
			msglevel_del = ~0;	/* make the whole list absolute */
		val = strtoul(s, &endptr, 0);
		/* not a plain integer if not all the string was parsed by strtoul */
		if (*endptr != '\0') {
			for (i = 0; (val = dbg_msg[i].value); i++)
				if (stricmp(dbg_msg[i].string, s) == 0)
					break;
			if (!val)
				goto usage;
		}
		if (**argv == '-')
			msglevel_del |= val;
		else
			msglevel_add |= val;
		++argv;
	}

	msglevel &= ~msglevel_del;
	msglevel |= msglevel_add;

	return (dhd_iovar_setint(dhd, cmd->name, msglevel));

usage:
	fprintf(stderr, "msg values may be a list of numbers or names from the following set.\n");
	fprintf(stderr, "Use a + or - prefix to make an incremental change.");

	for (i = 0; (val = dbg_msg[i].value); i++) {
		if (val != last_val)
			fprintf(stderr, "\n0x%04x %s", val, dbg_msg[i].string);
		else
			fprintf(stderr, ", %s", dbg_msg[i].string);
		last_val = val;
	}
	fprintf(stderr, "\n");

	return 0;
}

static char *
ver2str(unsigned int vms, unsigned int vls)
{
	static char verstr[100];
	unsigned int maj, year, month, day, build;

	maj = (vms >> 16) & 0xFFFF;
	if (maj > 1000) {
		/* it is probably a date... */
		year = (vms >> 16) & 0xFFFF;
		month = vms & 0xFFFF;
		day = (vls >> 16) & 0xFFFF;
		build = vls & 0xFFFF;
		sprintf(verstr, "%d/%d/%d build %d",
			month, day, year, build);
	} else {
		/* it is a tagged release. */
		sprintf(verstr, "%d.%d RC%d.%d",
			(vms>>16)&0xFFFF, vms&0xFFFF,
			(vls>>16)&0xFFFF, vls&0xFFFF);
	}
	return verstr;
}

static int
dhd_version(void *dhd, cmd_t *cmd, char **argv)
{
	int ret;
	char *ptr;

	UNUSED_PARAMETER(cmd);
	UNUSED_PARAMETER(argv);

	/* Display the application version info */
	printf("%s: %s\n", dhdu_av0,
		ver2str((EPI_MAJOR_VERSION << 16)| EPI_MINOR_VERSION,
		(EPI_RC_NUMBER << 16) | EPI_INCREMENTAL_NUMBER));

	if ((ret = dhd_var_getbuf(dhd, cmd->name, NULL, 0, (void**)&ptr)) < 0)
		return ret;

	/* Display the returned string */
	printf("%s\n", ptr);

	return 0;
}

static int
dhd_var_setint(void *dhd, cmd_t *cmd, char **argv)
{
	int32 val;
	int len;
	char *varname;
	char *endptr = NULL;
	char *p;

	if (cmd->set == -1) {
		printf("set not defined for %s\n", cmd->name);
		return BCME_ERROR;
	}

	if (!*argv) {
		printf("set: missing arguments\n");
		return BCME_USAGE_ERROR;
	}

	varname = *argv++;

	if (!*argv) {
		printf("set: missing value argument for set of \"%s\"\n", varname);
		return BCME_USAGE_ERROR;
	}

	val = strtol(*argv, &endptr, 0);
	if (*endptr != '\0') {
		/* not all the value string was parsed by strtol */
		printf("set: error parsing value \"%s\" as an integer for set of \"%s\"\n",
			*argv, varname);
		return BCME_USAGE_ERROR;
	}

	strcpy(buf, varname);
	p = buf;
	while (*p != '\0') {
		*p = tolower(*p);
		p++;
	}

	/* skip the NUL */
	p++;

	memcpy(p, &val, sizeof(uint));
	len = (int)(p - buf) +  sizeof(uint);

	return (dhd_set(dhd, DHD_SET_VAR, &buf[0], len));
}

static int
dhd_var_get(void *dhd, cmd_t *cmd, char **argv)
{
	char *varname;
	char *p;

	UNUSED_PARAMETER(cmd);

	if (!*argv) {
		printf("get: missing arguments\n");
		return BCME_USAGE_ERROR;
	}

	varname = *argv++;

	if (*argv) {
		printf("get: error, extra arg \"%s\"\n", *argv);
		return BCME_USAGE_ERROR;
	}

	strcpy(buf, varname);
	p = buf;
	while (*p != '\0') {
		*p = tolower(*p);
		p++;
	}
	return (dhd_get(dhd, DHD_GET_VAR, &buf[0], DHD_IOCTL_MAXLEN));
}

static int
dhd_var_getint(void *dhd, cmd_t *cmd, char **argv)
{
	int err;
	int32 val;
	if (cmd->get == -1) {
		printf("get not defined for %s\n", cmd->name);
		return BCME_ERROR;
	}

	if ((err = dhd_var_get(dhd, cmd, argv)))
		return (err);

	val = *(int32*)buf;

	if (val < 10)
		printf("%d\n", val);
	else
		printf("%d (0x%x)\n", val, val);

	return (0);
}

static int
dhd_var_getandprintstr(void *dhd, cmd_t *cmd, char **argv)
{
	int err;

	if ((err = dhd_var_get(dhd, cmd, argv)))
		return (err);

	printf("%s\n", buf);
	return (0);
}

void
dhd_printlasterror(void *dhd)
{
	char *cmd[2] = {"bcmerrorstr"};

	if (dhd_var_get(dhd, NULL, cmd) != 0) {
		fprintf(stderr, "%s: \nError getting the last error\n", dhdu_av0);
	} else {
		fprintf(stderr, "%s: %s\n", dhdu_av0, buf);
	}
}

static int
dhd_varint(void *dhd, cmd_t *cmd, char *argv[])
{
	if (argv[1])
		return (dhd_var_setint(dhd, cmd, argv));
	else
		return (dhd_var_getint(dhd, cmd, argv));
}

static int
dhd_var_getbuf(void *dhd, char *iovar, void *param, int param_len, void **bufptr)
{
	int len;

	memset(buf, 0, DHD_IOCTL_MAXLEN);
	strcpy(buf, iovar);

	/* include the NUL */
	len = (int)strlen(iovar) + 1;

	if (param_len)
		memcpy(&buf[len], param, param_len);

	*bufptr = buf;

	return dhd_get(dhd, DHD_GET_VAR, &buf[0], DHD_IOCTL_MAXLEN);
}

static int
dhd_var_setbuf(void *dhd, char *iovar, void *param, int param_len)
{
	int len;

	memset(buf, 0, DHD_IOCTL_MAXLEN);
	strcpy(buf, iovar);

	/* include the NUL */
	len = (int)strlen(iovar) + 1;

	if (param_len)
		memcpy(&buf[len], param, param_len);

	len += param_len;

	return dhd_set(dhd, DHD_SET_VAR, &buf[0], len);
}

static int
dhd_var_void(void *dhd, cmd_t *cmd, char **argv)
{
	UNUSED_PARAMETER(argv);

	if (cmd->set < 0)
		return BCME_ERROR;

	return dhd_var_setbuf(dhd, cmd->name, NULL, 0);
}

/*
 * format an iovar buffer
 */
static uint
dhd_iovar_mkbuf(char *name, char *data, uint datalen, char *buf, uint buflen, int *perr)
{
	uint len;

	len = (uint)strlen(name) + 1;

	/* check for overflow */
	if ((len + datalen) > buflen) {
		*perr = BCME_BUFTOOSHORT;
		return 0;
	}

	strcpy(buf, name);

	/* append data onto the end of the name string */
	if (datalen > 0)
		memcpy(&buf[len], data, datalen);

	len += datalen;

	*perr = 0;
	return len;
}

static int
dhd_iovar_getint(void *dhd, char *name, int *var)
{
	char ibuf[DHD_IOCTL_SMLEN];
	int error;

	dhd_iovar_mkbuf(name, NULL, 0, ibuf, sizeof(ibuf), &error);
	if (error)
		return error;

	if ((error = dhd_get(dhd, DHD_GET_VAR, &ibuf, sizeof(ibuf))) < 0)
		return error;

	memcpy(var, ibuf, sizeof(int));

	return 0;
}

static int
dhd_iovar_setint(void *dhd, char *name, int var)
{
	int len;
	char ibuf[DHD_IOCTL_SMLEN];
	int error;

	len = dhd_iovar_mkbuf(name, (char *)&var, sizeof(var), ibuf, sizeof(ibuf), &error);
	if (error)
		return error;

	if ((error = dhd_set(dhd, DHD_SET_VAR, &ibuf, len)) < 0)
		return error;

	return 0;
}

/*
* format a bsscfg indexed iovar buffer
*/
static int
dhd_bssiovar_mkbuf(const char *iovar, int bssidx, void *param,
	int paramlen, void *bufptr, int buflen, int *perr)
{
	const char *prefix = "bsscfg:";
	int8* p;
	uint prefixlen;
	uint namelen;
	uint iolen;

	prefixlen = strlen(prefix);	/* length of bsscfg prefix */
	namelen = strlen(iovar) + 1;	/* length of iovar name + null */
	iolen = prefixlen + namelen + sizeof(int) + paramlen;

	/* check for overflow */
	if (buflen < 0 || iolen > (uint)buflen) {
		*perr = BCME_BUFTOOSHORT;
		return 0;
	}

	p = (int8*)bufptr;

	/* copy prefix, no null */
	memcpy(p, prefix, prefixlen);
	p += prefixlen;

	/* copy iovar name including null */
	memcpy(p, iovar, namelen);
	p += namelen;

	/* bss config index as first param */
	bssidx = bssidx;
	memcpy(p, &bssidx, sizeof(int32));
	p += sizeof(int32);

	/* parameter buffer follows */
	if (paramlen) {
		memcpy(p, param, paramlen);
	}

	*perr = 0;
	return iolen;
}

/*
* set named & bss indexed driver iovar providing both parameter and i/o buffers
*/
int
dhdu_bssiovar_setbuf(void* dhd, const char *iovar, int bssidx,
	void *param, int paramlen, void *bufptr, int buflen)
{
	int err;
	int iolen;

	iolen = dhd_bssiovar_mkbuf(iovar, bssidx, param, paramlen, bufptr, buflen, &err);
	if (err) {
		return err;
	}

	return dhd_set(dhd, DHD_SET_VAR, bufptr, iolen);
}

/*
* get named & bss indexed driver iovar providing both parameter and i/o buffers
*/
static int
dhd_bssiovar_getbuf(void* dhd, const char *iovar, int bssidx,
	void *param, int paramlen, void *bufptr, int buflen)
{
	int err;

	dhd_bssiovar_mkbuf(iovar, bssidx, param, paramlen, bufptr, buflen, &err);
	if (err) {
		return err;
	}

	return dhd_get(dhd, DHD_GET_VAR, bufptr, buflen);
}

/*
* get named & bss indexed driver variable to buffer value
*/
int
dhdu_bssiovar_get(void *dhd, const char *iovar, int bssidx, void *outbuf, int len)
{
	char smbuf[DHD_IOCTL_SMLEN] = {0};
	int err;

	/* use the return buffer if it is bigger than what we have on the stack */
	if (len > (int)sizeof(smbuf)) {
		err = dhd_bssiovar_getbuf(dhd, iovar, bssidx, NULL, 0, outbuf, len);
	} else {
		err = dhd_bssiovar_getbuf(dhd, iovar, bssidx, NULL, 0, smbuf, sizeof(smbuf));
		if (err == 0) {
			memcpy(outbuf, smbuf, len);
		}
	}

	return err;
}

/*
* set named & bss indexed driver variable to buffer value
*/
static int
dhd_bssiovar_set(void *dhd, const char *iovar, int bssidx, void *param, int paramlen)
{
	char smbuf[DHD_IOCTL_SMLEN] = {0};

	return dhdu_bssiovar_setbuf(dhd, iovar, bssidx, param, paramlen, smbuf, sizeof(smbuf));
}

/*
* get named & bsscfg indexed driver variable as an int value
*/
static int
dhd_bssiovar_getint(void *dhd, const char *iovar, int bssidx, int *pval)
{
	int ret;

	ret = dhdu_bssiovar_get(dhd, iovar, bssidx, pval, sizeof(int));
	return ret;
}

/*
* set named & bsscfg indexed driver variable to int value
*/
static int
dhd_bssiovar_setint(void *dhd, const char *iovar, int bssidx, int val)
{
	return dhd_bssiovar_set(dhd, iovar, bssidx, &val, sizeof(int));
}

static int
dhd_varstr(void *dhd, cmd_t *cmd, char **argv)
{
	int error;
	char *str;

	if (!*++argv) {
		void *ptr;

		if ((error = dhd_var_getbuf(dhd, cmd->name, NULL, 0, &ptr)) < 0)
			return (error);

		str = (char *)ptr;
		printf("%s\n", str);
		return (0);
	} else {
		str = *argv;
		/* iovar buffer length includes NUL */
		return dhd_var_setbuf(dhd, cmd->name, str, (int)strlen(str) + 1);
	}
}

static int
dhd_hostreorder_flows(void *dhd, cmd_t *cmd, char **argv)
{
	int ret, count, i = 0;
	void *ptr;
	uint8 *flow_id;

	if ((ret = dhd_var_getbuf(dhd, cmd->name, NULL, 0, &ptr)) < 0) {
		printf("error getting reorder flows from the host\n");
		return ret;
	}
	flow_id = (uint8 *)ptr;
	count = *flow_id;
	if (!count)
		printf("there are no active flows\n");
	else {
		printf("flows(%d): \t", count);
		while (i++ < count)
			printf("%d  ", *flow_id++);
		printf("\n");
	}
	return 0;
}

#ifdef BCMSPI
static int
dhd_spierrstats(void *dhd, cmd_t *cmd, char **argv)
{
	int ret, i;
	struct spierrstats_t spierrstats, *p;
	uint argc;
	void *ptr = NULL;

	bzero(&spierrstats, sizeof(spierrstats));

	/* arg count */
	for (argc = 0; argv[argc]; argc++);
	argc--;

	if (strcmp(cmd->name, "spi_errstats")) {
		printf("Supported only for SPI. Required cmd name missing\n");
		return BCME_USAGE_ERROR;
	}

	/* Check 3rd argument */
	argv++;

	/* no third arg means get, otherwise set */
	if (!*argv) {
		if ((ret = dhd_var_getbuf(dhd, cmd->name, NULL, 0, &ptr)) < 0)
			return ret;

		p = (struct spierrstats_t *)ptr;

		printf("Last %d device-status-bits\n", NUM_PREV_TRANSACTIONS);

		for (i = 0; i < NUM_PREV_TRANSACTIONS; i++)
			printf("dstatus[%d] = 0x%x, spicmd[%d] = 0x%x\n", i, p->dstatus[i],
			        i, p->spicmd[i]);

		printf("The requested data was not available  = %d\n",
			p->dna);
		printf("FIFO underflow happened due to F2 rd command = %d\n",
			p->rdunderflow);
		printf("FIFO overflow happened due to F1/F2 wr command = %d\n",
			p->wroverflow);
		printf("F2 interrupt (OR of all F2 related intr status bits) = %d\n",
			p->f2interrupt);
		printf("F2 FIFO is not ready to receive data = %d\n",
			p->f2rxnotready);
		printf("Error in command or host data, detected by checksum = %d\n",
			p->hostcmddataerr);
		printf("Packet is available in F2 TX FIFO = %d\n",
			p->f2pktavailable);

	} else {
		ret = dhd_var_setbuf(dhd, cmd->name, &spierrstats, sizeof(spierrstats));
	}

	return (ret);
}
#endif /* BCMSPI */

/* These two utility functions are used by dhdu_linux.c
 * The code is taken from wlu.c.
 */
int
dhd_atoip(const char *a, struct ipv4_addr *n)
{
	char *c;
	int i = 0;

	for (;;) {
	        n->addr[i++] = (uint8)strtoul(a, &c, 0);
	        if (*c++ != '.' || i == IPV4_ADDR_LEN)
	                break;
	        a = c;
	}
	return (i == IPV4_ADDR_LEN);
}

int
dhd_ether_atoe(const char *a, struct ether_addr *n)
{
	char *c;
	int i = 0;

	memset(n, 0, ETHER_ADDR_LEN);
	for (;;) {
	        n->octet[i++] = (uint8)strtoul(a, &c, 16);
	        if (!*c++ || i == ETHER_ADDR_LEN)
	                break;
	        a = c;
	}
	return (i == ETHER_ADDR_LEN);
}
