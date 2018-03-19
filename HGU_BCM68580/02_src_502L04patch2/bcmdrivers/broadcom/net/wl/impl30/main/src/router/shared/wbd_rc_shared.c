/*
 * Broadcom Home Gateway Reference Design
 * Broadcom Wi-Fi Blanket shared functions
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
 * $Id: wbd_rc_shared.c 720827 2017-09-12 09:20:47Z $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <typedefs.h>
#include <shutils.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <bcmendian.h>
#include <bcmparams.h>
#include <proto/ethernet.h>
#include <wlioctl.h>
#include <wlutils.h>
#include <bcmendian.h>
#include <bcmutils.h>
#include <common_utils.h>

#include "wbd_rc_shared.h"

/* Wi-Fi Blanket Buffer Lengths */
#define WBD_MAX_BUF_256			256
#define WBD_MIN_PSK_LEN			8
#define WBD_MAX_PSK_LEN			63

/* WBD Backhaul Types
 * TODO : Needs to add entry for PLC and MOCA interfaces
 */
typedef enum {
	WBD_BACKHAUL_TYPE_UNDEFINED = -1,
	WBD_BACKHAUL_TYPE_MASTER = 0,
	WBD_BACKHAUL_TYPE_ETHERNET,
	WBD_BACKHAUL_TYPE_DWDS,
	WBD_BACKHAUL_TYPE_PLC
} wbd_backhaul_type_t;

#define WBD_BACKHAUL_TYPE_STR_ETH	"eth"
#define WBD_BACKHAUL_TYPE_STR_DWDS	"dwds"

/* Convert Backhaul String to Backhaul enum type */
#define WBD_BKHL_STR_TO_TYPE(str)	 \
	((strcasecmp((str), WBD_BACKHAUL_TYPE_STR_ETH) == 0) ? WBD_BACKHAUL_TYPE_ETHERNET : \
	((strcasecmp((str), WBD_BACKHAUL_TYPE_STR_DWDS) == 0) ? WBD_BACKHAUL_TYPE_DWDS : \
	WBD_BACKHAUL_TYPE_UNDEFINED))

/* Wi-Fi Blanket Bridge Types of Interfaces */
#define	WBD_BRIDGE_INVALID	-1	/* -1 Bridge Type is Invalid */
#define	WBD_BRIDGE_LAN		0	/*  0 Bridge Type is LAN */
#define	WBD_BRIDGE_GUEST	1	/*  1 Bridge Type is GUEST */
#define WBD_BRIDGE_VALID(bridge) ((((bridge) < WBD_BRIDGE_LAN) || \
					((bridge) > WBD_BRIDGE_GUEST)) ? (0) : (1))

/* Wi-Fi Blanket Band Types */
#define	WBD_BAND_LAN_INVALID	0	/* 0 - auto-select */
#define	WBD_BAND_LAN_2G		1	/* 1 - 2.4 Ghz */
#define	WBD_BAND_LAN_5GL	2	/* 2 - 5 Ghz LOW */
#define	WBD_BAND_LAN_5GH	3	/* 3 - 5 Ghz HIGH */
#define	WBD_BAND_LAN_ALL	4	/* 4 - all bands */

/* Validate Wi-Fi Blanket Band LAN Type Digit */
#define WBD_BAND_LAN_VALID(band) ((((band) < WBD_BAND_LAN_2G) || \
				((band) > WBD_BAND_LAN_5GH)) ? (0) : (1))

/* Validate Wi-Fi Blanket Band Digit */
#define WBD_BAND_VALID(band)	(WBD_BAND_LAN_VALID((band)))

/* Print Wi-Fi Blanket Band Digit */
#define WBD_BAND_DIGIT(band) (((band) == WBD_BAND_LAN_2G) ? (2) : (5))

/* General NVRAMs */
#define NVRAM_BR0_IFNAMES		"br0_ifnames"
#define NVRAM_LAN_IFNAMES		"lan_ifnames"
#define NVRAM_LAN1_IFNAMES		"lan1_ifnames"
#define NVRAM_WPS_CUSTOM_IFNAMES	"wps_custom_ifnames"
#define NVRAM_MODE			"mode"
#define NVRAM_DWDS			"dwds"
#define NVRAM_ROUTER_DISABLE		"router_disable"
#define NVRAM_LAN_DHCP			"lan_dhcp"
#define NVRAM_UNIT			"unit"
#define NVRAM_BSS_ENABLED		"bss_enabled"
#define NVRAM_HWADDR			"hwaddr"
#define NVRAM_IFNAME			"ifname"
#define NVRAM_VIFS			"vifs"
#define NVRAM_AKM			"akm"
#define NVRAM_SSID			"ssid"
#define NVRAM_CLOSED			"closed"
#define NVRAM_CRYPTO			"crypto"
#define NVRAM_WPA_PSK			"wpa_psk"

/* WBD NVRAM variable names */
#define WBD_NVRAM_MODE			"wbd_mode"
#define WBD_NVRAM_IFNAMES		"wbd_ifnames"
#define WBD_NVRAM_FIXED_IFNAMES		"wbd_fixed_ifnames"
#define WBD_NVRAM_REPEAT_BACKHAUL	"wbd_repeat_backhaul"
#define WBD_NVRAM_BACKHAUL_BAND		"wbd_backhaul_band"
#define WBD_NVRAM_FBT			"wbd_fbt"
#define WBD_NVRAM_AUTO_CONFIG		"wbd_auto_config"
#define WBD_BACKHAUL_SSID		"wbd_backhaul_ssid"
#define WBD_BACKHAUL_PWD		"wbd_backhaul_pwd"
#define WBD_NVRAM_BACKHAUL_TYPE		"wbd_backhaul_type"

/* FBT related NVRAMs */
#define NVRAM_FBT_MDID			"fbt_mdid"
#define NVRAM_FBT_R1KH_ID		"r1kh_id"
#define NVRAM_FBT_OVERDS		"fbtoverds"
#define NVRAM_FBT_REASSOC_TIME		"fbt_reassoc_time"
#define NVRAM_FBT_AP			"fbt_ap"

/* WBD NVRAM variables - Default values */
#define WBD_NV_DEF_FIXED_IFNAMES	0
#define WBD_NV_DEF_REPEAT_BACKHAUL	0
#define WBD_NV_DEF_BACKHAUL_BAND	WBD_BAND_LAN_5GH
#define WBD_NV_DEF_AUTO_CONFIG		1
#define WBD_NV_DEF_DWDS			"1"
#define WBD_NV_DEF_REP_MODE		"sta"
#define WBD_NV_DEF_REP_ROUTER_DISABLE	"1"
#define WBD_NV_DEF_REP_LAN_DHCP		"1"
#define WBD_NV_DEF_MODE_AP		"ap"
#define WBD_NV_DEF_BACKHAUL_SSID	"Bcm_Wbd_Hidden"
#define WBD_NV_DEF_CLOSED		"1"
#define WBD_NV_DEF_AKM			"psk2"
#define WBD_NV_DEF_CRYPTO		"aes"
#define WBD_NV_DEF_WPA_PSK		"Pv!tGcM0K&#>^mRk"
#define WBD_NV_DEF_BACKHAUL_TYPE	WBD_BACKHAUL_TYPE_DWDS

/* FBT NVRAMs default values */
#define WBD_FBT_NOT_DEFINED		-1
#define WBD_FBT_DEF_FBT_ENABLED		1
#define WBD_FBT_DEF_OVERDS		0
#define WBD_FBT_DEF_REASSOC_TIME	1000
#define WBD_FBT_DEF_AP			1

/* WBD Errors */
#define WBDE_OK			0
#define WBDE_INV_ARG		-3	/* Invalid arguments */
#define WBDE_WL_ERROR		-11	/* WL IOVAR error */
#define WBDE_INV_MODE		-20	/* Invalid Blanket Mode */
#define WBDE_DWDS_AP_VIF_EXST	-30	/* Virtual interface already up */
#define WBDE_DWDS_STA_PIF_NEXST	-31	/* No DWDS primary Ifr with mode STA */
#define WBDE_WBD_IFNAMES_FULL	-32	/* wbd_ifnames have 2G & 5G ifnames */
#define WBDE_WBD_IFNAMES_NEXST	-33	/* wbd_ifnames NVRAM not defined */
#define WBDE_CLI_INV_BAND	-75	/* Valid Band required */
#define WBDE_INV_IFNAME		-81	/* Invalid interface name */

#ifndef WBDSTRNCPY
#define WBDSTRNCPY(dst, src, len)	 \
	do { \
		strncpy(dst, src, len -1); \
		dst[len - 1] = '\0'; \
	} while (0)
#endif // endif

#ifdef WBD_RC_PRINT_ENB
#define WBD_RC_PRINT(fmt, arg...) \
	printf("WBD-RC-SHARED >> %s(%d) : "fmt, __FUNCTION__, __LINE__, ##arg)
#else
#define WBD_RC_PRINT(fmt, arg...)
#endif /* WBD_RC_PRINT_ENB */

/* Check if device in MASTER mode */
#define IS_WBD_MASTER(mode) (((mode) == WBD_MODE_MASTER) || \
	((mode) == WBD_MODE_DAP_MASTER))

/* Check if device in SLAVE mode */
#define IS_WBD_SLAVE(mode) (((mode) == WBD_MODE_SLAVE) || \
	((mode) == WBD_MODE_UAP_SLAVE))

/* Check if device is UAP */
#define IS_WBD_DEV_UAP(mode) (((mode) == WBD_MODE_MASTER) || \
	((mode) == WBD_MODE_UAP_SLAVE))

/* Check if device is DAP */
#define IS_WBD_DEV_DAP(mode) (((mode) == WBD_MODE_SLAVE) || \
	((mode) == WBD_MODE_DAP_MASTER))

/* Wi-Fi Blanket Mode */
typedef enum {
	WBD_MODE_UNDEFINED = -1,
	WBD_MODE_DISABLED = 0,
	WBD_MODE_MASTER,
	WBD_MODE_SLAVE,
	WBD_MODE_DAP_MASTER,	/* Downstream AP running master */
	WBD_MODE_UAP_SLAVE	/* Upstream AP running only slave */
} wbd_mode_t;

/* Validate Wi-Fi Blanket Mode */
#define WBD_MODE_VALID(mode)	((((mode) < WBD_MODE_MASTER) || \
				((mode) > WBD_MODE_UAP_SLAVE)) ? (0) : (1))

/* IOCTL swapping mode for Big Endian host with Little Endian dongle.  Default to off */
/* The below macros handle endian mis-matches between wl utility and wl driver. */
extern bool gg_swap;
#define htod32(i) (gg_swap ? bcmswap32(i) : (uint32)(i))
#define dtoh32(i) (gg_swap ? bcmswap32(i) : (uint32)(i))

#ifdef DSLCPE
static int wbd_is_intf_virtual(char *ifname) {
	int unit = -1, subunit = -1;

	if (get_ifname_unit(ifname, &unit, &subunit) < 0) {
		return FALSE;
	}
	if (unit < 0) {
		return FALSE;
	}
	if (subunit > 0) {
		return TRUE;
	}

	return FALSE;
}
#endif

/* Extern Declarations */
extern char* strncpy_n(char *destination, const char *source, size_t num);
extern void nvram_initialize_all(char *prefix);
static inline void sys_reboot(void)
{
	eval("wl", "reboot");
	kill(1, SIGTERM);
}

#if defined(BCA_CPEROUTER)
#define wbd_sys_restart() do { system("nvram commit restart"); raise(SIGTERM); } while (0)
#elif defined(STB)
#define wbd_sys_restart() do { sleep(1); eval("rc", "restart"); } while (0)
#else
#define wbd_sys_restart() kill(1, SIGHUP)
#endif /* BCA_CPEROUTER */

#if defined(BCA_CPEROUTER)
#define wbd_sys_reboot() do { system("nvram commit reboot"); raise(SIGTERM); } while (0)
#else
#define wbd_sys_reboot() do { sys_reboot(); } while (0)
#endif /* BCA_CPEROUTER */

#if defined(BCA_CPEROUTER)
#define wbd_nvram_commit() do {} while (0)
#else
#define wbd_nvram_commit() do { nvram_commit(); } while (0)
#endif /* BCA_CPEROUTER */

/* WBD version to get the NVRAM value. If we run this in PC it will just return NULL */
static char*
wbd_nvram_safe_get(const char *nvram)
{
	return nvram_safe_get(nvram);
}

/* WBD version to get the NVRAM value for specific BSS prefix */
static char*
wbd_nvram_prefix_safe_get(const char *prefix, const char *nvram)
{
	char data[WBD_MAX_BUF_256] = {0};

	if (prefix) {
		return nvram_safe_get(strcat_r(prefix, nvram, data));
	} else {
		return nvram_safe_get(nvram);
	}
}

/* Gets the unsigned integer config val from NVARM, if not found applies the default value */
static uint16
wbd_nvram_safe_get_uint(char* prefix, const char *c, uint16 def)
{
	char *val = NULL;
	uint16 ret = def;

	if (prefix) {
		val = wbd_nvram_prefix_safe_get(prefix, c);
	} else {
		val = wbd_nvram_safe_get(c);
	}

	if (val[0] != '\0') {
		ret = strtoul(val, NULL, 0);
	} else {
		WBD_RC_PRINT("NVRAM is not defined: %s%s \n", (prefix ? prefix : ""), c);
	}

	return ret;
}

/* Gets the integer config val from NVARM, if not found applies the default value */
static int
wbd_nvram_safe_get_int(char* prefix, const char *c, int def)
{
	char *val = NULL;
	int ret = def;

	if (prefix) {
		val = wbd_nvram_prefix_safe_get(prefix, c);
	} else {
		val = wbd_nvram_safe_get(c);
	}

	if (val[0] != '\0') {
		ret = atoi(val);
	} else {
		WBD_RC_PRINT("NVRAM is not defined: %s%s \n", (prefix ? prefix : ""), c);
	}

	return ret;
}

/* WBD version to set the NVRAM value for specific BSS prefix */
static int
wbd_nvram_prefix_set(const char *prefix, const char *nvram, const char *nvramval)
{
	char data[WBD_MAX_BUF_256] = {0};

	if (prefix) {
		return nvram_set(strcat_r(prefix, nvram, data), nvramval);
	} else {
		return nvram_set(nvram, nvramval);
	}
}

/* Match NVRAM and ARG value, and if mismatch, Set new value in NVRAM */
static uint32
wbd_nvram_prefix_match_set(const char* prefix, char* nvram, char* new_value,
	bool matchcase)
{
	char *nvram_value = NULL;
	int mismatch = 0;

	/* Get NVRAM Value */
	nvram_value = wbd_nvram_prefix_safe_get(prefix, nvram);

	/* Compare NVRAM and New value, and if mismatch, set New value in NVRAM */
	if (matchcase) {
		mismatch = strcmp(nvram_value, new_value);
	} else {
		mismatch = strcasecmp(nvram_value, new_value);
	}

	if (mismatch) {

		WBD_RC_PRINT("Prefix[%s] NVRAM[%s] NVRAMVal[%s] != NewVal[%s]."
			" Needs Rc Restart.\n", prefix, nvram, nvram_value, new_value);

		/* Set New Value in NVRAM */
		wbd_nvram_prefix_set(prefix, nvram, new_value);

		/* Value is Overidden, indicate NVRAM commit & RC Restart */
		return WBD_FLG_NV_COMMIT|WBD_FLG_RC_RESTART;
	}
	return 0;
}

/* Executes nvram commit/rc restart/reboot commands */
void
wbd_do_rc_restart_reboot(uint32 rc_flags)
{
	if (rc_flags & WBD_FLG_NV_COMMIT) {
		WBD_RC_PRINT("rc_flags[0x%x], Going to do nvram_commit()...\n", rc_flags);
		wbd_nvram_commit();
	}

	if (rc_flags & WBD_FLG_REBOOT) {
		WBD_RC_PRINT("rc_flags[0x%x], Going to do Reboot...\n", rc_flags);
		wbd_sys_reboot();
		return;
	}

	if (rc_flags & WBD_FLG_RC_RESTART) {
		WBD_RC_PRINT("rc_flags[0x%x], Going to do RC Restart...\n", rc_flags);
		wbd_sys_restart();
		return;
	}
}

/* Check if Interface is Virtual or not */
static bool
wbd_is_ifr_virtual(char *ifname)
{
	int unit = -1, subunit = -1;

	if (get_ifname_unit(ifname, &unit, &subunit) < 0) {
		return FALSE;
	}
	if (unit < 0) {
		return FALSE;
	}
	if (subunit > 0) {
		return TRUE;
	}

	return FALSE;
}

/* Check if Interface and its Primary Radio is enabled or not */
static int
wbd_is_ifr_enabled(char *ifname, bool validate_vif, int *error)
{
#ifdef DSLCPE_ENDIAN
	int i, j;
#endif
	int ret = WBDE_OK, unit = 0, ifr_enabled = 0;
	char buf[WBD_MAX_BUF_256] = {0};

	/* Check interface (fail for non-wl interfaces) */
	if ((ret = wl_probe(ifname))) {
		ret = WBDE_INV_IFNAME;
		goto end;
	}

	wl_endian_probe(ifname);

	/* Get instance */
	ret = wl_ioctl(ifname, WLC_GET_INSTANCE, &unit, sizeof(unit));
	if (ret < 0) {
		/* printf("%s : Failed to %s, WL error : %d\n", ifname, "get instance", ret); */
		ret = WBDE_WL_ERROR;
		goto end;
	}
	unit = dtoh32(unit);

#ifdef DSLCPE_ENDIAN
	unit = etoh32(unit);
#endif
	/* Check if Primary Radio of given Interface is enabled or not */
	snprintf(buf, sizeof(buf), "wl%d_radio", unit);
	ifr_enabled = atoi(nvram_safe_get(buf));

	if (!ifr_enabled) {
		goto end;
	}

	/* Check if interface is vifs */
	if ((validate_vif) && (wbd_is_ifr_virtual(ifname))) {
		/* Check if vifs is enabled or not */
		snprintf(buf, sizeof(buf), "%s_bss_enabled", ifname);
		ifr_enabled = (atoi(nvram_safe_get(buf)));
	}
end:
	if (error) {
		*error = ret;
	}
	return ifr_enabled;
}

/* Get wlX_ or wlX.y_ Prefix from OS specific interface name */
static int
wbd_get_prefix(char *ifname, char *prefix, int prefix_len)
{
	int ret = WBDE_OK;
	char wl_name[IFNAMSIZ] = {0};

	/* Convert eth name to wl name - returns 0 if success */
	ret = osifname_to_nvifname(ifname, wl_name, sizeof(wl_name));
	if (ret != WBDE_OK) {
		ret = WBDE_INV_IFNAME;
		goto end;
	}

	/* Get prefix of the interface from Driver */
	make_wl_prefix(prefix, prefix_len, 1, wl_name);
	/* printf("Interface: %s, wl_name: %s, Prefix: %s\n", ifname, wl_name, prefix); */

end:
	return ret;
}

/* Gets WBD Band Enumeration from ifname's Chanspec & Bridge Type */
static int
wbd_identify_wbd_band_type(int bridge_dgt, chanspec_t ifr_chanspec, int *out_band)
{
	int ret = WBDE_CLI_INV_BAND, channel = 0;

	if (!out_band) {
		ret = WBDE_INV_ARG;
		goto end;
	}
	*out_band = WBD_BAND_LAN_INVALID;

	if (!WBD_BRIDGE_VALID(bridge_dgt)) {
		goto end;
	}

	channel = CHSPEC_CHANNEL(ifr_chanspec);

	if (bridge_dgt == WBD_BRIDGE_LAN) {
		if (CHSPEC_IS2G(ifr_chanspec)) {
			*out_band = WBD_BAND_LAN_2G;
		} else if (CHSPEC_IS5G(ifr_chanspec)) {
			if (channel < 100) {
				*out_band = WBD_BAND_LAN_5GL;
			} else {
				*out_band = WBD_BAND_LAN_5GH;
			}
		}
		ret = WBDE_OK;
	}

end:
	WBD_RC_PRINT("Bridge[br%d] Chanspec[0x%x] Channel[%d] WBD_BAND[%d]\n",
		bridge_dgt, ifr_chanspec, channel,
		((out_band) ? *out_band : WBD_BAND_LAN_INVALID));

	return ret;
}

/* Find Primary Interface Name configured on a specific WBD Band Type */
static int
wbd_find_prim_ifname_fm_wbd_band(int wbd_band, char *ifname, int len, bool check_ap_mode)
{
	int ret, ifr_band, ifr_band_digit = 0, cur_chspec, wbd_band_digit = 0;
	char lan_ifnames[NVRAM_MAX_VALUE_LEN] = {0};
	char var_intf[IFNAMSIZ] = {0}, *next_intf, tmp[NVRAM_MAX_VALUE_LEN];
	char prefix[IFNAMSIZ] = {0}, name[IFNAMSIZ] = {0}, nbr_ifname[IFNAMSIZ] = {0};
	chanspec_t ifr_chanspec;

	memset(ifname, 0, len);

	/* Validate Arg, WBD Band */
	if (!WBD_BAND_VALID((wbd_band))) {
		return WBDE_CLI_INV_BAND;
	}

	/* Get input Band Digit */
	wbd_band_digit = WBD_BAND_DIGIT((wbd_band));

	/* Get NVRAM value from "lan_ifnames" and save it */
	WBDSTRNCPY(lan_ifnames, wbd_nvram_safe_get(NVRAM_LAN_IFNAMES), sizeof(lan_ifnames) - 1);

	/* Traverse lan_ifnames for each ifname */
	foreach(var_intf, lan_ifnames, next_intf) {

		/* Copy Interface name temporarily */
		WBDSTRNCPY(name, var_intf, sizeof(name) - 1);

		/* Check if valid Wireless Interface and its Primary Radio is enabled or not */
		if (!wbd_is_ifr_enabled(name, FALSE, &ret)) {
			continue; /* Skip non-Wireless & disabled Wireless Interface */
		}

		/* Check if Interface is Virtual Interface */
		if (wbd_is_ifr_virtual(name)) {
			continue; /* skip Virtual Interface, as we are looking for Primif */
		}

		/* If Interface's mode is required as "ap" */
		if (check_ap_mode) {

			/* Get Prefix from OS specific interface name */
			wbd_get_prefix(name, prefix, sizeof(prefix));

			/* Check if Interface's NVRAM wlX_mode = "ap" or not */
			if (!nvram_match(strcat_r(prefix, NVRAM_MODE, tmp),
				WBD_NV_DEF_MODE_AP)) {
				continue; /* Skip non-ap Interface */
			}
		}

		/* Get Interface's Chanspec */
		wl_endian_probe(name);
		(void)wl_iovar_getint(name, "chanspec", &cur_chspec);
		ifr_chanspec = (chanspec_t)dtoh32(cur_chspec);

		/* Gets WBD Band Enumeration from ifname's Chanspec & Bridge Type */
		ret = wbd_identify_wbd_band_type(WBD_BRIDGE_LAN, ifr_chanspec, &ifr_band);
		if (ret != WBDE_OK) {
			continue;
		}

		/* Get Interface's Band Digit */
		ifr_band_digit = WBD_BAND_DIGIT((ifr_band));

		/* Compare This Interface's WBD Band Digit with Input Band Digit */
		if (ifr_band_digit == wbd_band_digit) {
			/* Copy Neighbor ifname, if matching ifr not found, return this ifname */
			WBDSTRNCPY(nbr_ifname, name, sizeof(nbr_ifname) - 1);
		}

		/* Compare This Interface's WBD Band Type with Dedicated Band */
		if (ifr_band == wbd_band) {
			/* Copy Interface name to return it */
			WBDSTRNCPY(ifname, name, len - 1);
			return WBDE_OK; /* if WBD Band Types matches, exit loop with Success code */
		}
	}

	/* Copy Neighbor Interface name to return it, as matching ifr not found */
	WBDSTRNCPY(ifname, nbr_ifname, len - 1);
	return WBDE_DWDS_STA_PIF_NEXST;
}

/* Check if Interface is Virtual Interface, if Disabled, Enable it */
uint32
wbd_enable_vif(char *ifname)
{
	int vif_enabled, unit, subunit;
	char prefix[IFNAMSIZ] = {0};
	uint32 rc_flags = 0;

	/* Check if interface is Primary Interface */
	if (!wbd_is_ifr_virtual(ifname)) {
		goto end; /* get out */
	}

	/* Get Prefix from OS specific interface name */
	wbd_get_prefix(ifname, prefix, sizeof(prefix));

	/* Check if Virtual AP Interface is Enabled */
	vif_enabled = atoi(wbd_nvram_prefix_safe_get(prefix, NVRAM_BSS_ENABLED));

	if (!vif_enabled) {
		/* Get Unit & Subunit of Virtual Interface */
		sscanf(ifname, "wl%d.%d", &unit, &subunit);

		/* Create & Configure Virtual AP Interface, for DWDS Slave */
		wbd_create_vif(unit, subunit);
		/* Commit NVRAM & RC Restart */
		rc_flags |= WBD_FLG_NV_COMMIT|WBD_FLG_RC_RESTART;
	}

end:
	return rc_flags;
}

/* Check if Interface is DWDS Primary Interface, with mode = STA, Change name1 */
int
wbd_check_dwds_sta_primif(char *ifname, char *ifname1, int len1)
{
	int ret = WBDE_DWDS_STA_PIF_NEXST, dwds, sta_mode, unit, subunit = 1;
	char prefix[IFNAMSIZ] = {0}, tmp[NVRAM_MAX_VALUE_LEN];

	/* Check if interface is Virtual Interface */
	if (wbd_is_ifr_virtual(ifname)) {
		goto end; /* get out */
	}

	/* Get Prefix from OS specific interface name */
	wbd_get_prefix(ifname, prefix, sizeof(prefix));

	/* Check if on Primary Interface DWDS in ON */
	dwds = atoi(wbd_nvram_prefix_safe_get(prefix, NVRAM_DWDS));

	/* Check if on Primary Interface Mode is STA */
	sta_mode = nvram_match(strcat_r(prefix, NVRAM_MODE, tmp), WBD_NV_DEF_REP_MODE) ? 1 : 0;

	/* printf("%sdwds = %d, sta_mode = %d\n", prefix, dwds, sta_mode); */
	/* if DWDS in ON, mode is STA */
	if (dwds && sta_mode) {

		/* Get Unit of Primary Interface */
		unit = atoi(wbd_nvram_prefix_safe_get(prefix, NVRAM_UNIT));

		/* Get next available Virtual AP Interface for DWDS Slave */
		subunit = wbd_get_next_vif_subunit(unit, &ret);

		/* Change Virtual AP Interface ifname */
		snprintf(ifname1, len1, "wl%d.%d", unit, subunit);

		/* DWDS Primary Interface, with mode = STA found */
		ret = WBDE_OK;
	} else {
		/* Set Primary Interface ifname as actual name */
#ifdef DSLCPE
		if (ifname1 != ifname)
#endif
		WBDSTRNCPY(ifname1, ifname, len1);
	}

end:
	return ret;
}

/* Find First DWDS Primary Interface, with mode = STA */
int
wbd_find_dwds_sta_primif(char *ifname, int len, char *ifname1, int len1)
{
	int ret = WBDE_DWDS_STA_PIF_NEXST;
	char lan_ifnames[NVRAM_MAX_VALUE_LEN] = {0};
	char var_intf[IFNAMSIZ] = {0}, *next_intf;

	/* Get NVRAM value from "lan_ifnames" and save it */
	WBDSTRNCPY(lan_ifnames, wbd_nvram_safe_get(NVRAM_LAN_IFNAMES), sizeof(lan_ifnames) - 1);

	/* Traverse lan_ifnames for each ifname */
	foreach(var_intf, lan_ifnames, next_intf) {

		/* Copy interface name temporarily */
		WBDSTRNCPY(ifname, var_intf, len - 1);

		/* Find DWDS Primary Interface, with mode = STA */
		ret = wbd_check_dwds_sta_primif(ifname, ifname1, len1);
		if (ret == WBDE_OK) {
			break; /* if found, exit loop, get updated ifname, ifname1 */
		}
	}

	return ret;
}

/* Find Backhaul Prim Ifr Configured on this Device (RootAP/Repeater), Check if its Dedicated */
int
wbd_find_backhaul_primif_on_device(char *backhaul_if, int backhaul_if_len, int *repeat_backhaul)
{
	int ret = WBDE_DWDS_STA_PIF_NEXST, backhaul_band, wbd_mode = WBD_MODE_DISABLED;
	char ifname[IFNAMSIZ] = {0};

	/* Get the NVRAM : Wi-Fi Blanket Application Mode */
	wbd_mode = wbd_nvram_safe_get_int(NULL, WBD_NVRAM_MODE, WBD_MODE_DISABLED);

	/* Get the NVRAM from "wbd_backhaul_band" and save it */
	backhaul_band = wbd_nvram_safe_get_int(NULL,
		WBD_NVRAM_BACKHAUL_BAND, WBD_NV_DEF_BACKHAUL_BAND);

	/* Find Primary Interface Name configured on RootAP/Repeater for Backhaul Band */
	ret = wbd_find_prim_ifname_fm_wbd_band(backhaul_band, ifname, sizeof(ifname),
		IS_WBD_DEV_UAP(wbd_mode) ? TRUE : FALSE);
	WBD_RC_PRINT("ifname for Backhaul Band[%d] is [%s]\n", backhaul_band, ifname);

	/* Remember Backhaul Link */
	WBDSTRNCPY(backhaul_if, ifname, backhaul_if_len - 1);

	if (repeat_backhaul) {
		/* Get the NVRAM which tells whether to have dedicated backhaul link or not */
		*repeat_backhaul = wbd_nvram_safe_get_int(NULL, WBD_NVRAM_REPEAT_BACKHAUL,
			WBD_NV_DEF_REPEAT_BACKHAUL);

		/* If the number of interfaces is not more than two(not triband),
		 * disable dedicated backhaul
		 */
		if (wbd_count_interfaces() <= 2) {
			*repeat_backhaul = 1;
		}

		WBD_RC_PRINT("repeat_backhaul[%d].\n", *repeat_backhaul);
	}

	return ret;
}

/* Add valid interfaces to "wbd_ifnames" and "wbd_ifnames1" arrays for 2G & 5G */
static int
add_ifr_to_wbd_ifnames(char *wbd_ifnames, int len, char *wbd_ifnames1, int len1,
	char *ifname, char *ifname1)
{
	int ret = WBDE_OK;

	/* Add ifname to wbd_ifnames */
	add_to_list(ifname, wbd_ifnames, len);

	/* Add ifname1 to wbd_ifnames1 */
	add_to_list(ifname1, wbd_ifnames1, len1);

	return ret;
}

/* Get "wbd_ifnames" from "lan_ifnames" */
/* "wbd_ifnames" : returns list of Primary Interfaces, used to set "wbd_ifnames" NVRAM */
/* "wbd_ifnames1" : returns list of Ifs, used by app to fetch WL infromation, can be Prim / VIFs */
int
wbd_ifnames_fm_lan_ifnames(char *wbd_ifnames, int len, char *wbd_ifnames1, int len1)
{
	int ret = WBDE_OK, repeat_backhaul = 0;
	char lan_ifnames[NVRAM_MAX_VALUE_LEN] = {0};
	char backhaul_if[IFNAMSIZ] = {0}, name[IFNAMSIZ] = {0};
	char *next_intf, var_intf[IFNAMSIZ] = {0};

	memset(wbd_ifnames, 0, len);
	memset(wbd_ifnames1, 0, len1);

	/* Find Backhaul Prim Ifr on this Device (RootAP/Repeater), Check if its Dedicated */
	wbd_find_backhaul_primif_on_device(backhaul_if, sizeof(backhaul_if), &repeat_backhaul);

	/* Get NVRAM value from "lan_ifnames" and save it */
	WBDSTRNCPY(lan_ifnames, wbd_nvram_safe_get(NVRAM_LAN_IFNAMES), sizeof(lan_ifnames) - 1);

	/* Traverse lan_ifnames for non-DWDS Primary ifnames */
	foreach(var_intf, lan_ifnames, next_intf) {

		/* Copy interface name temporarily */
		WBDSTRNCPY(name, var_intf, sizeof(name) - 1);

		/* Don't add this ifname to "wbd_ifnames", if it's a Virtual Interface or
		 * if it's a Dedicated Backhaul Primary Interface and we don't want to Repeat it
		 */
		if ((wbd_is_ifr_virtual(name)) ||
			((!repeat_backhaul) && (strcmp(name, backhaul_if) == 0))) {

			WBD_RC_PRINT("name[%s] backhaul_if[%s] repeat_backhaul[%d].\n",
				name, backhaul_if, repeat_backhaul);
			continue; /* Skip it */
		}

		/* Check if valid Wireless Interface and its Primary Radio is enabled or not */
		if (!wbd_is_ifr_enabled(name, FALSE, &ret)) {
			continue; /* Skip non-Wireless & disabled Wireless Interface */
		}

		/* Try to add this interface to "wbd_ifnames" */
		ret = add_ifr_to_wbd_ifnames(wbd_ifnames, len, wbd_ifnames1, len1, name, name);
	}

	return ret;
}

/* Get next available Virtual Interface Subunit */
int
wbd_get_next_vif_subunit(int in_unit, int *error)
{
	int ret = WBDE_OK, unit, subunit, ret_subunit = 1;
	char lan_ifnames[NVRAM_MAX_VALUE_LEN] = {0};
	char name[IFNAMSIZ] = {0}, var_intf[IFNAMSIZ] = {0}, *next_intf;

	/* Get NVRAM value from "lan_ifnames" and save it */
	WBDSTRNCPY(lan_ifnames, wbd_nvram_safe_get(NVRAM_LAN_IFNAMES), sizeof(lan_ifnames) - 1);

	/* Traverse lan_ifnames for each ifname */
	foreach(var_intf, lan_ifnames, next_intf) {

		/* Re-Intialize flags */
		unit = 0, subunit = 0, ret_subunit = 1;

		/* Copy interface name temporarily */
		WBDSTRNCPY(name, var_intf, sizeof(name) - 1);

		/* Check if interface is Primary Interface */
		if (!wbd_is_ifr_virtual(name)) {
			continue; /* skip Primary Interface, as we are looking for vif */
		}

		/* Check if Virtual Interface is of our interest, if so, return this subunit */
		sscanf(name, "wl%d.%d", &unit, &subunit);
		if (unit == in_unit) {
			ret_subunit = subunit;
			/* printf("Interface : %s is enabled, don't create again\n", name); */
			break;
		}
	}

	if (error) {
		*error = ret;
	}
	return ret_subunit;
}

/* Create & Configure Virtual Interface (MBSS) in Network Type LAN */
/* Returns : WBDE_OK : if vif required to enable, gets enabled successfully */
/* Returns : WBDE_DWDS_AP_VIF_EXST : if vif required to enable, is already enabled */
int
wbd_create_vif(int unit, int subunit)
{
	int ret = WBDE_OK, vif_enabled;
	char prim_prefix[IFNAMSIZ] = {0};
	char vif_prefix[IFNAMSIZ] = {0}, vif_ifname[IFNAMSIZ] = {0};
	char vif_unit[IFNAMSIZ] = {0};
	char interface_list[NVRAM_MAX_VALUE_LEN] = {0}, *str = NULL;
	int interface_list_size = sizeof(interface_list);

	/* Create prim_prefix[wlX_] */
	snprintf(prim_prefix, sizeof(prim_prefix), "wl%d_", unit);

	/* Create vif_prefix[wlX.1_] , vif_ifname[wlX.1],  vif_unit[X.1] */
	snprintf(vif_ifname, sizeof(vif_ifname), "wl%d.%d", unit, subunit);
	snprintf(vif_prefix, sizeof(vif_prefix), "wl%d.%d_", unit, subunit);
	snprintf(vif_unit, sizeof(vif_unit), "%d.%d", unit, subunit);

	/* Check if AP vif is already up or not */
	vif_enabled = atoi(wbd_nvram_prefix_safe_get(vif_prefix, NVRAM_BSS_ENABLED));
	str = wbd_nvram_prefix_safe_get(vif_prefix, NVRAM_IFNAME);

	/* if AP vif is already up, return */
	if ((vif_enabled) && (str) && (str[0] != '\0')) {
		ret = WBDE_DWDS_AP_VIF_EXST;
		goto end;
	}

	/* -- if AP vif is not up already, enable it  -- */
	/* Add interface to br0_ifnames, if not already */
	str = wbd_nvram_safe_get(NVRAM_BR0_IFNAMES);
	strncpy_n(interface_list, str, interface_list_size);
	ret = add_to_list(vif_ifname, interface_list, interface_list_size);
	if (ret == 0) {
		nvram_set(NVRAM_BR0_IFNAMES, interface_list);
	}

	/* Add interface to lan_ifnames, if not already */
	str = wbd_nvram_safe_get(NVRAM_LAN_IFNAMES);
	strncpy_n(interface_list, str, interface_list_size);
	ret = add_to_list(vif_ifname, interface_list, interface_list_size);
	if (ret == 0) {
		nvram_set(NVRAM_LAN_IFNAMES, interface_list);
	}

	/* Add interface to wlX_vifs, if not already */
	str = wbd_nvram_prefix_safe_get(prim_prefix, NVRAM_VIFS);
	strncpy_n(interface_list, str, interface_list_size);
	ret = add_to_list(vif_ifname, interface_list, interface_list_size);
	if (ret == 0) {
		wbd_nvram_prefix_set(prim_prefix, NVRAM_VIFS, interface_list);
	}

	/* Remove interface fm lan1_ifnames, if not already */
	str = wbd_nvram_safe_get(NVRAM_LAN1_IFNAMES);
	strncpy_n(interface_list, str, interface_list_size);
	ret = remove_from_list(vif_ifname, interface_list, interface_list_size);
	if (ret == 0) {
		nvram_set(NVRAM_LAN1_IFNAMES, interface_list);
	}

	/* Initialize other required NVRAMs to enable AP vif */
	nvram_initialize_all(vif_prefix);
	wbd_nvram_prefix_set(vif_prefix, NVRAM_IFNAME, vif_ifname);
	wbd_nvram_prefix_set(vif_prefix, NVRAM_UNIT, vif_unit);

	/* Execute nvram commit & rc restart commands, skipping as Easy Setup covers this */
	/* wbd_do_rc_restart_reboot(WBD_FLG_NV_COMMIT|WBD_FLG_RC_RESTART); */

#ifdef DSLCPE
	system("nvram commit restart");
	raise(SIGTERM);
#endif

end:
	return ret;
}

#if defined(__linux__)

#ifndef	RANDOM_READ_TRY_MAX
#define RANDOM_READ_TRY_MAX	10
#endif // endif
static void linux_random(uint8 *rand, int len)
{
	static int dev_random_fd = -1;
	int status;
	int i;

	if (dev_random_fd == -1) {
		if ((dev_random_fd = open("/dev/urandom", O_RDONLY|O_NONBLOCK)) == -1) {
			WBD_RC_PRINT("Error opening /dev/urandom : %s", strerror(errno));
			return;
		}
	}

	for (i = 0; i < RANDOM_READ_TRY_MAX; i++) {
		status = read(dev_random_fd, rand, len);
		if (status == -1) {
			if (errno == EINTR)
				continue;

			assert(status != -1);
		}

		return;
	}

	assert(i != RANDOM_READ_TRY_MAX);
}

/* Get Random Bytes */
static void RAND_bytes(unsigned char *buf, int num)
{
	linux_random(buf, num);
}
#endif /* __linux__ */

#ifdef WLHOSTFBT

/* Get MDID from NVRAM, if not present Generate the MDID */
uint16
wbd_get_mdid(char *prefix)
{
	uint16 mdid = 0;

	mdid = wbd_nvram_safe_get_uint(prefix, NVRAM_FBT_MDID, 0);

	/* If MDID not found in NVRAM */
	if (mdid == 0) {
		/* Generate MDID */
		RAND_bytes((uint8 *)&mdid, sizeof(mdid));
		mdid |= 1;
	}

	return mdid;
}

/* Enable FBT */
static int
wbd_enable_fbt(char *prefix)
{
	int fbt = WBD_FBT_DEF_FBT_ENABLED;
	char strnvval[WBD_MAX_BUF_256] = {0};
	char *nvval = NULL;

	/* Set wbd_fbt NVRAM */
	memset(strnvval, 0, sizeof(strnvval));
	snprintf(strnvval, sizeof(strnvval), "%d", fbt);
	wbd_nvram_prefix_set(prefix, WBD_NVRAM_FBT, strnvval);
	WBD_RC_PRINT("%swbd_fbt NVRAM not defined, Seting it[%s]\n", prefix, strnvval);

	/* If psk2ft is already defined in akm NVRAM, do not add it */
	nvval = wbd_nvram_prefix_safe_get(prefix, NVRAM_AKM);
	if (find_in_list(nvval, "psk2ft")) {
		goto end;
	}

	/* Else Add psk2ft to akm */
	memset(strnvval, 0, sizeof(strnvval));
	WBDSTRNCPY(strnvval, nvval, sizeof(strnvval));
	add_to_list("psk2ft", strnvval, sizeof(strnvval));
	wbd_nvram_prefix_set(prefix, NVRAM_AKM, strnvval);
	WBD_RC_PRINT("psk2ft not defined in %sakm NVRAM, Seting it[%s]\n", prefix, strnvval);

end:
	return fbt;
}

/* Check whether FBT enabling is possible or not. First it checks for psk2 and then wbd_fbt */
static int
wbd_is_fbt_possible(char *prefix)
{
	int fbt = 0;
	char *nvval;

	/* Check if the akm contains psk2 or not */
	nvval = wbd_nvram_prefix_safe_get(prefix, NVRAM_AKM);
	if (find_in_list(nvval, "psk2") == NULL) {
		WBD_RC_PRINT("%s%s[%s]. Not psk2\n", prefix, NVRAM_AKM, nvval);
		goto end;
	}

	/* Get the wlxy_wbd_ft NVRAM value, which tells whether FBT is enabled from WBD or not
	 * If the NVRAM is not defined it returns not defined(-1)
	 */
	fbt = wbd_nvram_safe_get_int(prefix, WBD_NVRAM_FBT, WBD_FBT_NOT_DEFINED);

end:
	return fbt;
}

/* If Device is Upstream AP, Initialize the FBT NVRAMs */
static uint32
wbd_uap_init_fbt_nvram_config(char *ifname)
{
	char *nvval = NULL;
	char strnvval[WBD_MAX_BUF_256] = {0};
	char prefix[IFNAMSIZ] = {0}, new_value[WBD_MAX_BUF_256] = {0};
	char data[WBD_MAX_BUF_256];
	int fbt = 0, invval = 0, mdid = 0;
	uint32 rc_flags = 0;

	/* Get Prefix from OS specific interface name */
	wbd_get_prefix(ifname, prefix, sizeof(prefix));
	WBD_RC_PRINT("ifname[%s] Prefix[%s]\n", ifname, prefix);

	memset(data, 0, sizeof(data));

	/* If wbd_fbt NVRAM is not defined. Enable the FBT by setting the wbd_fbt */
	fbt = wbd_is_fbt_possible(prefix);
	if (fbt == WBD_FBT_NOT_DEFINED) {
		WBD_RC_PRINT("ifname[%s] Prefix[%s]\n Enabling FBT...", ifname, prefix);
		fbt = wbd_enable_fbt(prefix);
		rc_flags |= WBD_FLG_NV_COMMIT|WBD_FLG_RC_RESTART;
	}

	/* If wbd_fbt is disabled, no need to continue further */
	if (fbt <= 0) {
		WBD_RC_PRINT("Prefix[%s] WBD_FBT[%d] is disabled\n", prefix, fbt);
		goto end;
	}

	/* If FBT OVERDS NVRAM is not defined. set it to default */
	nvval = nvram_get(strcat_r(prefix, NVRAM_FBT_OVERDS, data));
	if (!nvval) {
		memset(strnvval, 0, sizeof(strnvval));
		snprintf(strnvval, sizeof(strnvval), "%d", WBD_FBT_DEF_OVERDS);
		wbd_nvram_prefix_set(prefix, NVRAM_FBT_OVERDS, strnvval);
		rc_flags |= WBD_FLG_NV_COMMIT|WBD_FLG_RC_RESTART;
	}

	/* Read fbt reassoc time NVRAM and set default */
	invval = wbd_nvram_safe_get_int(prefix, NVRAM_FBT_REASSOC_TIME, 0);
	if (invval <= 0) {
		memset(strnvval, 0, sizeof(strnvval));
		snprintf(strnvval, sizeof(strnvval), "%d", WBD_FBT_DEF_REASSOC_TIME);
		wbd_nvram_prefix_set(prefix, NVRAM_FBT_REASSOC_TIME, strnvval);
		rc_flags |= WBD_FLG_NV_COMMIT|WBD_FLG_RC_RESTART;
	}

	/* If FBT AP NVRAM is not defined. set it to default */
	nvval = nvram_get(strcat_r(prefix, NVRAM_FBT_AP, data));
	if (!nvval) {
		memset(strnvval, 0, sizeof(strnvval));
		snprintf(strnvval, sizeof(strnvval), "%d", WBD_FBT_DEF_AP);
		wbd_nvram_prefix_set(prefix, NVRAM_FBT_AP, strnvval);
		rc_flags |= WBD_FLG_NV_COMMIT|WBD_FLG_RC_RESTART;
	}

	/* Get MDID from NVRAM, if not present generate the mdid */
	mdid = wbd_get_mdid(NULL);
	memset(new_value, 0, sizeof(new_value));
	snprintf(new_value, sizeof(new_value), "%d", mdid);

	/* Match MDID from NVRAM, and argument, and if mismatch, Set new MDID in NVRAM */
	rc_flags |= wbd_nvram_prefix_match_set(NULL,  NVRAM_FBT_MDID, new_value, FALSE);

	/* Get NVRAM value for "wlX_hwaddr" */
	memset(new_value, 0, sizeof(new_value));
	WBDSTRNCPY(new_value, wbd_nvram_prefix_safe_get(prefix, NVRAM_HWADDR),
		sizeof(new_value) - 1);

	/* Match MDID from NVRAM, and argument, and if mismatch, Set new MDID in NVRAM */
	rc_flags |= wbd_nvram_prefix_match_set(prefix, NVRAM_FBT_R1KH_ID, new_value, FALSE);

	/* If any NVRAM is updated, do NVRAM commit */
	if (rc_flags & WBD_FLG_NV_COMMIT) {
		nvram_commit();
		WBD_RC_PRINT("Prefix[%s] FBT NVRAMs Modified. Needs Rc Restart.\n", prefix);
	}

end:
	return rc_flags;

}

#endif /* WLHOSTFBT */

/* Set "wps_custom_ifnames" from "lan_ifnames" */
/* "wps_custom_ifnames" : returns list of Wireless Ifrs, used to set "wps_custom_ifnames" NVRAM */
static uint32
wbd_set_wps_custom_ifnames()
{
	int ret = WBDE_OK;
	char lan_ifnames[NVRAM_MAX_VALUE_LEN] = {0};
	char wps_custom_ifnames[NVRAM_MAX_VALUE_LEN] = {0};
	char wps_new_ifnames[NVRAM_MAX_VALUE_LEN] = {0};
	char name[IFNAMSIZ] = {0};
	char *next_intf, var_intf[IFNAMSIZ] = {0};
	uint32 rc_flags = 0;

	/* Get NVRAM value from "wps_custom_ifnames" and save it */
	WBDSTRNCPY(wps_custom_ifnames, wbd_nvram_safe_get(NVRAM_WPS_CUSTOM_IFNAMES),
		sizeof(wps_custom_ifnames) - 1);
	WBD_RC_PRINT("NVRAM value : wps_custom_ifnames[%s]\n", wps_custom_ifnames);

	/* Get NVRAM value from "lan_ifnames" and save it */
	WBDSTRNCPY(lan_ifnames, wbd_nvram_safe_get(NVRAM_LAN_IFNAMES),
		sizeof(lan_ifnames) - 1);

	/* Traverse lan_ifnames for each ifname, and create "wps_new_ifnames" */
	foreach(var_intf, lan_ifnames, next_intf) {

		/* Copy interface name temporarily */
		WBDSTRNCPY(name, var_intf, sizeof(name) - 1);

		/* Check if valid Wireless Interface & its Primary Radio is enabled */
		if (!wbd_is_ifr_enabled(name, TRUE, &ret)) {
			continue; /* Skip non-Wireless & disabled Wireless Interface */
		}

		/* Add this interface to "wps_new_ifnames" */
		add_to_list(name, wps_new_ifnames, sizeof(wps_new_ifnames));
		WBD_RC_PRINT("Adding %s in wps_new_ifnames[%s]\n", name, wps_new_ifnames);
	}

	/* If NVRAM "wps_custom_ifnames" is not set, or changed, set it again. */
	if ((!(strlen(wps_custom_ifnames) > 0)) ||
		(strcmp(wps_new_ifnames, wps_custom_ifnames) != 0)) {

		WBD_RC_PRINT("Setting wps_custom_ifnames[%s]\n", wps_new_ifnames);
		/* Set new NVRAM "wps_custom_ifnames" value */
		nvram_set(NVRAM_WPS_CUSTOM_IFNAMES, wps_new_ifnames);
		/* Commit NVRAM & RC Restart */
		rc_flags |= WBD_FLG_NV_COMMIT|WBD_FLG_RC_RESTART;
	}

	return rc_flags;
}

/* Do default configuration settings for backhaul interface. */
static uint32
wbd_configure_dwds_backhaul_interface(char *prefix, int wbd_mode)
{
	uint32 rc_flags = 0;
	char* backhaul_ssid = wbd_nvram_prefix_safe_get(prefix, WBD_BACKHAUL_SSID);
	char* backhaul_pwd = wbd_nvram_prefix_safe_get(prefix, WBD_BACKHAUL_PWD);
	int len = strlen(backhaul_pwd);

	backhaul_ssid = backhaul_ssid[0] != '\0' ? backhaul_ssid : WBD_NV_DEF_BACKHAUL_SSID;

	/* Backhaul pwd validation. */
	if ((len < WBD_MIN_PSK_LEN) || (len > WBD_MAX_PSK_LEN)) {
		backhaul_pwd = WBD_NV_DEF_WPA_PSK;
	}

	rc_flags |= wbd_nvram_prefix_match_set(prefix, NVRAM_DWDS, WBD_NV_DEF_DWDS, TRUE);
	rc_flags |= wbd_nvram_prefix_match_set(prefix, NVRAM_SSID, backhaul_ssid, TRUE);
	rc_flags |= wbd_nvram_prefix_match_set(prefix, NVRAM_AKM, WBD_NV_DEF_AKM, TRUE);
	rc_flags |= wbd_nvram_prefix_match_set(prefix, NVRAM_CRYPTO, WBD_NV_DEF_CRYPTO, TRUE);
	rc_flags |= wbd_nvram_prefix_match_set(prefix, NVRAM_WPA_PSK, backhaul_pwd, TRUE);

	/* For RootAP set n/w type as closed. */
	if (IS_WBD_DEV_UAP(wbd_mode)) {
		rc_flags |= wbd_nvram_prefix_match_set(prefix, NVRAM_CLOSED,
			WBD_NV_DEF_CLOSED, TRUE);
	}

	return rc_flags;
}

/* Create a Virtual AP Interface for DWDS Backhaul */
static uint32
wbd_uap_enable_dwds_ap_vif(char *ifname, int len)
{
	int unit = -1, subunit = -1;
	char wl_name[IFNAMSIZ] = {0};
	uint32 rc_flags = 0;

	/* Convert eth name to wl name - returns 0 if success */
	if (osifname_to_nvifname(ifname, wl_name, sizeof(wl_name))) {
		return 0;
	}

	/* Parse the unit from an interface string such as wlXX or wlXX.YY */
	if (!get_ifname_unit(wl_name, &unit, NULL)) {

		/* Get next available Virtual AP Interface to create DWDS Backhaul */
		subunit = wbd_get_next_vif_subunit(unit, NULL);

		/* Create Virtual AP Interface as DWDS Backhaul */
		if (wbd_create_vif(unit, subunit) != WBDE_DWDS_AP_VIF_EXST) {

			/* Commit NVRAM & RC Restart */
			rc_flags |= WBD_FLG_NV_COMMIT|WBD_FLG_RC_RESTART;
			WBD_RC_PRINT("UAP Backhaul DWDS VAP IF created on [wl%d.%d]\n",
				unit, subunit);
		}
		/* Change Virtual AP Interface ifname */
		snprintf(ifname, len, "wl%d.%d", unit, subunit);
	}

	return rc_flags;
}

/* Do Initial Auto Configuration for Easy Setup for RootAP OR Repeater */
static uint32
wbd_do_init_auto_config(int wbd_mode, wbd_backhaul_type_t backhaul_type)
{
	int ret = WBDE_OK, router_disable = 0, lan_dhcp = 0;
	char prefix[IFNAMSIZ] = {0}, ifname[IFNAMSIZ] = {0};
	uint32 rc_flags = 0;
	int repeat_backhaul = 0;

	/* Validate Arg, WBD Mode */
	if (!WBD_MODE_VALID((wbd_mode))) {
		return 0;
	}

	/* Reset NVRAM "wbd_auto_config", as fm next boot this sequence is not desired */
	nvram_set(WBD_NVRAM_AUTO_CONFIG, "0");
	nvram_commit();

	/* Find Backhaul Prim Ifr on this Device (RootAP/Repeater), Check if its Dedicated */
	ret = wbd_find_backhaul_primif_on_device(ifname, sizeof(ifname), &repeat_backhaul);

	/* if matching interface not found for wbd_band, look for Neighbor Interface */
	if (ret == WBDE_DWDS_STA_PIF_NEXST) {

		/* if Interface name is present, it's Neighbor Interface name */
		if (strlen(ifname) > 0) {
			/* Configure Neighbor Interface as Backhaul Link */
			WBD_RC_PRINT("Neighbor ifr[%s] found for Backhaul Band", ifname);

		/* if Neighbor Interface is also not found, nothing can be done! */
		} else {
			WBD_RC_PRINT("Error : ifname for Backhaul Band not found\n");
			return 0;
		}
	}

	/* If Device is Upstream AP, and repeat backhaul is set */
	if (IS_WBD_DEV_UAP(wbd_mode) && repeat_backhaul) {

		/* Create a Virtual AP Interface for DWDS Backhaul, "ifname" can change here */
		rc_flags |= wbd_uap_enable_dwds_ap_vif(ifname, sizeof(ifname));
		WBD_RC_PRINT("UAP Configured ifr[%s] for Backhaul Band", ifname);
	}

	/* Get Prefix from OS specific Primary Interface name for Backhaul Band */
	ret = wbd_get_prefix(ifname, prefix, sizeof(prefix));
	WBD_RC_PRINT("prefix for Backhaul Band is [%s]\n", prefix);
	if ((ret != WBDE_OK) || (!(strlen(prefix) > 0))) {
		WBD_RC_PRINT("Error : prefix for Backhaul Band not found\n");
		return 0;
	}

	/* Either Device is Upstream AP or Downstream AP with Backhaul Type = DWDS */
	if (IS_WBD_DEV_UAP(wbd_mode) ||
		(IS_WBD_DEV_DAP(wbd_mode) && (backhaul_type == WBD_BACKHAUL_TYPE_DWDS))) {

		/* Configure backhaul interface with default nvrams (ssid, pwd etc.) */
		rc_flags |= wbd_configure_dwds_backhaul_interface(prefix, wbd_mode);

	}

	/* if Device is Downstream AP, Set STA mode, router_disable, lan_dhcp NVRAMs */
	if (IS_WBD_DEV_DAP(wbd_mode)) {

		/* Check for Backhaul Type, if its DWDS, Set mode = STA */
		if (backhaul_type == WBD_BACKHAUL_TYPE_DWDS) {

			/* Match MODE from NVRAM, and "sta", if mismatch, Set NVRAM mode = STA */
			rc_flags |= wbd_nvram_prefix_match_set(prefix,
				NVRAM_MODE, WBD_NV_DEF_REP_MODE, FALSE);
		}

		/* Check for Repeater, router_disable = 1 or not */
		router_disable = atoi(wbd_nvram_safe_get(NVRAM_ROUTER_DISABLE));
		WBD_RC_PRINT("router_disable[%d]\n", router_disable);

		/* Check for Repeater, lan_dhcp = 1 or not */
		lan_dhcp = atoi(wbd_nvram_safe_get(NVRAM_LAN_DHCP));
		WBD_RC_PRINT("lan_dhcp[%d]\n", lan_dhcp);

		/* if any of above 2 NVRAMs is not set with correct value, set them correctly */
		if (!router_disable || !lan_dhcp) {

			/* Set router_disable = 1 */
			nvram_set(NVRAM_ROUTER_DISABLE, WBD_NV_DEF_REP_ROUTER_DISABLE);
			/* Set lan_dhcp = 1 */
			nvram_set(NVRAM_LAN_DHCP, WBD_NV_DEF_REP_LAN_DHCP);

			/* Commit NVRAM & RC Restart or Reboot */
			rc_flags |= (router_disable ? (WBD_FLG_NV_COMMIT|WBD_FLG_RC_RESTART) :
				(WBD_FLG_NV_COMMIT|WBD_FLG_REBOOT));
			WBD_RC_PRINT("New Values : %smode[%s] router_disable[%d] "
				"lan_dhcp[%d] rc_flags[0x%x]\n",
				prefix, wbd_nvram_prefix_safe_get(prefix, NVRAM_MODE),
				atoi(wbd_nvram_safe_get(NVRAM_ROUTER_DISABLE)),
				atoi(wbd_nvram_safe_get(NVRAM_LAN_DHCP)), rc_flags);
		}
	}

	/* If any NVRAM is updated, do NVRAM commit */
	if (rc_flags & WBD_FLG_NV_COMMIT) {
		nvram_commit();
	}

	return rc_flags;
}

/* Read "wbd_ifnames" NVRAM and get actual ifnames */
int
wbd_read_actual_ifnames(char *wbd_ifnames1, int len1, bool create)
{
	int ret = WBDE_OK, nvram_exists = 0, dwds_sta_pif = 0, is_fixed_ifnames = 0;
	wbd_backhaul_type_t backhaul_type;
	char wbd_ifnames[NVRAM_MAX_VALUE_LEN] = {0};
	char wbd_if_new[NVRAM_MAX_VALUE_LEN] = {0}, temp[NVRAM_MAX_VALUE_LEN] = {0};
	char name[IFNAMSIZ] = {0}, var_intf[IFNAMSIZ] = {0}, outname[IFNAMSIZ];
	char *next_intf, *val;
	int auto_config_enab = 1, wbd_mode = WBD_MODE_DISABLED;
	uint32 rc_flags = 0;

	memset(wbd_ifnames1, 0, len1);

	/* Get the NVRAM : Wi-Fi Blanket Application Mode */
	wbd_mode = wbd_nvram_safe_get_int(NULL, WBD_NVRAM_MODE, WBD_MODE_DISABLED);

	/* Get the NVRAM : Backhaul Type, and convert it to enum */
	val = wbd_nvram_safe_get(WBD_NVRAM_BACKHAUL_TYPE);
	backhaul_type = (val[0] != '\0') ? WBD_BKHL_STR_TO_TYPE(val) : WBD_NV_DEF_BACKHAUL_TYPE;
	WBD_RC_PRINT("wbd_mode[%d] backhaul_type[%d] \n", wbd_mode, backhaul_type);

	/* Get NVRAM value for "wbd_ifnames" */
	val = wbd_nvram_safe_get(WBD_NVRAM_IFNAMES);
	if (val[0] != '\0') {
		WBDSTRNCPY(wbd_ifnames, val, sizeof(wbd_ifnames) - 1);
		nvram_exists = 1;
	}

	/* if app other wbd (wlconf, bsd etc ) is calling & NVRAM not exist, or if no ifr's up */
	if (((!create) && (!nvram_exists)) ||
		(!(wbd_count_interfaces() > 0)))
	{
		/* wbd app will create it, so just get out */
		ret = WBDE_WBD_IFNAMES_NEXST;
		WBD_RC_PRINT("create[%d] nvram_exists[%d]. NVRAM not exists\n",
			create, nvram_exists);
		goto end;
	}

	/* Get NVRAM value which tells whether to recreate the NVRAM or not */
	is_fixed_ifnames = wbd_nvram_safe_get_int(NULL, WBD_NVRAM_FIXED_IFNAMES,
		WBD_NV_DEF_FIXED_IFNAMES);

	/* if wbd app is calling this */
	if (create) {

		/* Get NVRAM value which tells whether to do Initial Auto Configuration or not */
		auto_config_enab = wbd_nvram_safe_get_int(NULL, WBD_NVRAM_AUTO_CONFIG,
			WBD_NV_DEF_AUTO_CONFIG);

		if (auto_config_enab) {
			/* Do Initial Auto Configuration for Easy Setup for RootAP OR Repeater */
			rc_flags |= wbd_do_init_auto_config(wbd_mode, backhaul_type);
		}

		/* Find First DWDS Primary Interface, with mode = STA */
		ret = wbd_find_dwds_sta_primif(name, sizeof(name), name, sizeof(name));
		dwds_sta_pif = (ret == WBDE_DWDS_STA_PIF_NEXST) ? 0 : 1;
		ret = WBDE_OK;

		/* if NVRAM not exist OR, NVRAM exists but any DWDS STA Primary Ifr found
		 * && if wbd_ifnames are not fixed
		 */
		if ((!nvram_exists) || (nvram_exists && dwds_sta_pif && (!is_fixed_ifnames))) {
			/* prepare "wbd_ifnames"  fm "lan_ifnames" */
			wbd_ifnames_fm_lan_ifnames(wbd_if_new, sizeof(wbd_if_new),
				temp, sizeof(temp));
			WBD_RC_PRINT("nvram_exists[%d] dwds_sta_pif[%d] is_fixed_ifnames[%d] "
				"wbd_if_new[%s] temp[%s]\n",
				nvram_exists, dwds_sta_pif, is_fixed_ifnames, wbd_if_new, temp);
		}

		/* if NVRAM not exist OR, NVRAM exists && any DWDS STA Primary Ifr found
		 * && if wbd_ifnames are not fixed && Old NVRAM value and newly prepared
		 * wbd_ifnames don't match
		 */
		if ((!nvram_exists) || (nvram_exists && dwds_sta_pif && (!is_fixed_ifnames) &&
			(strcmp(wbd_if_new, wbd_ifnames) != 0))) {
			/* Set NVRAM value for "wbd_ifnames", commit NVRAM, rc restart */
			nvram_set(WBD_NVRAM_IFNAMES, wbd_if_new);
			/* commit NVRAM */
			nvram_commit();
			WBD_RC_PRINT("nvram_exists[%d] dwds_sta_pif[%d] is_fixed_ifnames[%d] "
				"wbd_ifnames[%s] wbd_if_new[%s]. Needs Rc Restart.\n",
				nvram_exists, dwds_sta_pif, is_fixed_ifnames, wbd_ifnames,
				wbd_if_new);

			/* Copy new value of "wbd_ifnames", and set flag to do "rc restart" */
			WBDSTRNCPY(wbd_ifnames, wbd_if_new, sizeof(wbd_ifnames) - 1);
			/* Commit NVRAM & RC Restart */
			rc_flags |= WBD_FLG_NV_COMMIT|WBD_FLG_RC_RESTART;
		}
	}

	/* Traverse wbd_ifnames for each ifname */
	foreach(var_intf, wbd_ifnames, next_intf) {

		memset(outname, 0, sizeof(outname));
		/* Copy interface name temporarily */
		WBDSTRNCPY(name, var_intf, sizeof(name) - 1);

		/* Check if Interface is DWDS Primary Interface, with mode = STA, Change name */
		wbd_check_dwds_sta_primif(name, outname, sizeof(outname));

		/* Check if Interface is DWDS Virtual Interface, if Disabled, Enable it */
		if (create) {
			rc_flags |= wbd_enable_vif(outname);
		}

		/* Add this interface to "wbd_ifnames1" */
		add_to_list(outname, wbd_ifnames1, len1);
		WBD_RC_PRINT("wbd_ifnames1[%s] name[%s] outname[%s]\n", wbd_ifnames1,
			name, outname);

#ifdef WLHOSTFBT
		/* If Device is Upstream AP, Initialize the FBT NVRAMs */
		if (create && (IS_WBD_DEV_UAP(wbd_mode))) {
			rc_flags |= wbd_uap_init_fbt_nvram_config(outname);
		}
#endif /* WLHOSTFBT */

	}
	WBD_RC_PRINT("nvram_exists[%d] dwds_sta_pif[%d] is_fixed_ifnames[%d] wbd_ifnames[%s] "
		"wbd_ifnames1[%s]\n", nvram_exists, dwds_sta_pif, is_fixed_ifnames,
		wbd_ifnames, wbd_ifnames1);

	/* WBD is calling & If Auto Config Enabled &&
	 * (Either its Upstream AP || Downstream AP with Backhaul Type = DWDS)
	 */
	if (create && auto_config_enab &&
		(IS_WBD_DEV_UAP(wbd_mode) ||
		(IS_WBD_DEV_DAP(wbd_mode) && (backhaul_type == WBD_BACKHAUL_TYPE_DWDS)))) {

		/* Set "wps_custom_ifnames" from "lan_ifnames" */
		rc_flags |= wbd_set_wps_custom_ifnames();
	}
end:
	/* If required, Execute nvram commit/rc restart/reboot commands */
	wbd_do_rc_restart_reboot(rc_flags);

	return ret;
}

/* Find Number of valid interfaces */
int
wbd_count_interfaces(void)
{
	char nvram_name[32], ifname[32];
	int ret, index, total = 0;

	/* Find out the wl interface index for the specified interface. */
	for (index = 0; index < DEV_NUMIFS; ++index) {

		snprintf(nvram_name, sizeof(nvram_name), "wl%d_ifname", index);
		WBDSTRNCPY(ifname, nvram_safe_get(nvram_name), sizeof(ifname));

		WBD_RC_PRINT("nvram_name=%s, ifname=%s\n", nvram_name, ifname);

		/* Check if valid Wireless Interface and its Primary Radio is enabled or not */
		if (!wbd_is_ifr_enabled(ifname, FALSE, &ret)) {
			continue; /* Skip non-Wireless & disabled Wireless Interface */
		}

		/* Valid Wireless Interface, Add it to count */
		total ++;
		WBD_RC_PRINT("unit=%d, total=%d\n", index, total);
	}

	return total;
}
