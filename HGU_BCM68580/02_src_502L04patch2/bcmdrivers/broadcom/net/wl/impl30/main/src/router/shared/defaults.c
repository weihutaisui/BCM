/*
 * Router default NVRAM values
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
 * $Id: defaults.c 710030 2017-07-11 13:07:17Z $
 */

#include <epivers.h>
#include "router_version.h"
#include <typedefs.h>
#include <string.h>
#include <ctype.h>
#include <bcmnvram.h>
#include <wlioctl.h>
#include <stdio.h>
#include <ezc.h>
#include <bcmconfig.h>
#include <shutils.h>
#include <bcmparams.h>

#define XSTR(s) STR(s)
#define STR(s) #s

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#endif /* ARRAYSIZE */

struct nvram_tuple router_defaults[] = {
#ifndef DSLCPE
	/* OS parameters */
	{ "os_name", "", 0 },			 /* OS name string */
	{ "os_version", ROUTER_VERSION_STR, 0 }, /* OS revision */
	{ "os_date", __DATE__, 0 },		 /* OS date */
	{ "wl_version", EPI_VERSION_STR, 0 },	 /* OS revision */

	/* Version */
	{ "nvram_version", NVRAM_SOFTWARE_VERSION, 0 },

	/* Miscellaneous parameters */
	{ "timer_interval", "3600", 0 },	/* Timer interval in seconds */
	{ "ntp_server", "192.5.41.40 192.5.41.41 133.100.9.2", 0 },		/* NTP server */
	{ "time_zone", "PST8PDT", 0 },		/* Time zone (GNU TZ format) */
	{ "log_level", "0", 0 },		/* Bitmask 0:off 1:denied 2:accepted */
	{ "upnp_enable", "1", 0 },		/* Start UPnP */
#ifdef __CONFIG_DLNA_DMR__
	{ "dlna_dmr_enable", "1", 0 },		/* Start DLNA Renderer */
#endif // endif
#ifdef __CONFIG_DLNA_DMS__
	{ "dlna_dms_enable", "1", 0 },		/* Start DLNA Server */
#endif // endif
	{ "ezc_enable", "1", 0 },		/* Enable EZConfig updates */
	{ "ezc_version", EZC_VERSION_STR, 0 },	/* EZConfig version */
	{ "is_default", "1", 0 },		/* is it default setting: 1:yes 0:no */
	{ "os_server", "", 0 },			/* URL for getting upgrades */
	{ "stats_server", "", 0 },		/* URL for posting stats */
	{ "console_loglevel", "1", 0 },		/* Kernel panics only */
#endif /* #ifndef DSLCPE */
	/* Big switches */
	{ "router_disable", "0", 0 },		/* lan_proto=static lan_stp=0 wan_proto=disabled */
	{ "ure_disable", "1", 0 },		/* sets APSTA for radio and puts wirelesss
						 * interfaces in correct lan
						 */
#ifndef DSLCPE
	{ "fw_disable", "0", 0 },		/* Disable firewall (allow new connections from the
						 * WAN)
						 */

	{ "log_ipaddr", "", 0 },		/* syslog recipient */
#ifdef BCMQOS
	{ "wan_mtu",			"1500"			},
#endif // endif
	/* LAN H/W parameters */
	{ "lan_ifname", "", 0 },		/* LAN interface name */
	{ "lan_ifnames", "", 0 },		/* Enslaved LAN interfaces */
	{ "lan_hwnames", "", 0 },		/* LAN driver names (e.g. et0) */
	{ "lan_hwaddr", "", 0 },		/* LAN interface MAC address */

	/* LAN TCP/IP parameters */
	{ "lan_dhcp", "0", 0 },			/* DHCP client [static|dhcp] */
	{ "lan_ipaddr", "192.168.1.1", 0 },	/* LAN IP address */
	{ "lan_netmask", "255.255.255.0", 0 },	/* LAN netmask */
	{ "lan_gateway", "192.168.1.1", 0 },	/* LAN gateway */
	{ "lan_proto", "dhcp", 0 },		/* DHCP server [static|dhcp] */
	{ "lan_wins", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "lan_domain", "", 0 },		/* LAN domain name */
	{ "lan_lease", "86400", 0 },		/* LAN lease time in seconds */
	{ "lan_stp", "1", 0 },			/* LAN spanning tree protocol */
	{ "lan_route", "", 0 },			/* Static routes
						 * (ipaddr:netmask:gateway:metric:ifname ...)
						 */
	/* Guest H/W parameters */
	{ "lan1_ifname", "", 0 },		/* LAN interface name */
	{ "lan1_ifnames", "", 0 },		/* Enslaved LAN interfaces */
	{ "lan1_hwnames", "", 0 },		/* LAN driver names (e.g. et0) */
	{ "lan1_hwaddr", "", 0 },		/* LAN interface MAC address */

	/* Guest TCP/IP parameters */
	{ "lan1_dhcp", "0", 0 },			/* DHCP client [static|dhcp] */
	{ "lan1_ipaddr", "192.168.2.1", 0 },	/* LAN IP address */
	{ "lan1_netmask", "255.255.255.0", 0 },	/* LAN netmask */
	{ "lan1_gateway", "192.168.2.1", 0 },	/* LAN gateway */
	{ "lan1_proto", "dhcp", 0 },		/* DHCP server [static|dhcp] */
	{ "lan1_wins", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "lan1_domain", "", 0 },		/* LAN domain name */
	{ "lan1_lease", "86400", 0 },		/* LAN lease time in seconds */
	{ "lan1_stp", "1", 0 },			/* LAN spanning tree protocol */
	{ "lan1_route", "", 0 },			/* Static routes
						 * (ipaddr:netmask:gateway:metric:ifname ...)
						 */
#ifdef __CONFIG_NAT__
	/* WAN H/W parameters */
	{ "wan_ifname", "", 0 },		/* WAN interface name */
	{ "wan_ifnames", "", 0 },		/* WAN interface names */
	{ "wan_hwname", "", 0 },		/* WAN driver name (e.g. et1) */
	{ "wan_hwaddr", "", 0 },		/* WAN interface MAC address */

	/* WAN TCP/IP parameters */
	{ "wan_proto", "dhcp", 0 },		/* [static|dhcp|pppoe|disabled] */
	{ "wan_ipaddr", "0.0.0.0", 0 },		/* WAN IP address */
	{ "wan_netmask", "0.0.0.0", 0 },	/* WAN netmask */
	{ "wan_gateway", "0.0.0.0", 0 },	/* WAN gateway */
	{ "wan_dns", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "wan_wins", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "wan_hostname", "", 0 },		/* WAN hostname */
	{ "wan_domain", "", 0 },		/* WAN domain name */
	{ "wan_lease", "86400", 0 },		/* WAN lease time in seconds */

	/* PPPoE parameters */
	{ "wan_pppoe_ifname", "", 0 },		/* PPPoE enslaved interface */
	{ "wan_pppoe_username", "", 0 },	/* PPP username */
	{ "wan_pppoe_passwd", "", 0 },		/* PPP password */
	{ "wan_pppoe_idletime", "60", 0 },	/* Dial on demand max idle time (seconds) */
	{ "wan_pppoe_keepalive", "0", 0 },	/* Restore link automatically */
	{ "wan_pppoe_demand", "0", 0 },		/* Dial on demand */
	{ "wan_pppoe_mru", "1492", 0 },		/* Negotiate MRU to this value */
	{ "wan_pppoe_mtu", "1492", 0 },		/* Negotiate MTU to the smaller of this value or
						 * the peer MRU
						 */
	{ "wan_pppoe_service", "", 0 },		/* PPPoE service name */
	{ "wan_pppoe_ac", "", 0 },		/* PPPoE access concentrator name */
	/* Misc WAN parameters */
	{ "wan_desc", "", 0 },			/* WAN connection description */
	{ "wan_route", "", 0 },			/* Static routes
						 * (ipaddr:netmask:gateway:metric:ifname ...)
						 */
	{ "wan_primary", "0", 0 },		/* Primary wan connection */

	{ "wan_unit", "0", 0 },			/* Last configured connection */

	/* Filters */
	{ "filter_maclist", "", 0 },		/* xx:xx:xx:xx:xx:xx ... */
	{ "filter_macmode", "deny", 0 },	/* "allow" only, "deny" only, or "disabled"
						 * (allow all)
						 */
	{ "filter_client0", "", 0 },		/* [lan_ipaddr0-lan_ipaddr1|*]:lan_port0-lan_port1,
						 * proto,enable,day_start-day_end,sec_start-sec_end,
						 * desc
						 */
	{ "nat_type", "sym", 0 },               /* sym: Symmetric NAT, cone: Cone NAT */
	/* Port forwards */
	{ "dmz_ipaddr", "", 0 },		/* x.x.x.x (equivalent to 0-60999>dmz_ipaddr:
						 * 0-60999)
						 */
	{ "forward_port0", "", 0 },		/* wan_port0-wan_port1>lan_ipaddr:
						 * lan_port0-lan_port1[:,]proto[:,]enable[:,]desc
						 */
	{ "autofw_port0", "", 0 },		/* out_proto:out_port,in_proto:in_port0-in_port1>
						 * to_port0-to_port1,enable,desc
						 */
#ifdef BCMQOS
	{ "qos_orates",	"80-100,10-100,5-100,3-100,2-95,0-0,0-0,0-0,0-0,0-0", 0 },
	{ "qos_irates",	"0,0,0,0,0,0,0,0,0,0", 0 },
	{ "qos_enable",			"0"				},
	{ "qos_method",			"0"				},
	{ "qos_sticky",			"1"				},
	{ "qos_ack",			"1"				},
	{ "qos_icmp",			"0"				},
	{ "qos_reset",			"0"				},
	{ "qos_obw",			"384"			},
	{ "qos_ibw",			"1500"			},
	{ "qos_orules",			"" },
	{ "qos_burst0",			""				},
	{ "qos_burst1",			""				},
	{ "qos_default",		"3"				},
#endif /* BCMQOS */
	/* DHCP server parameters */
	{ "dhcp_start", "192.168.1.100", 0 },	/* First assignable DHCP address */
	{ "dhcp_end", "192.168.1.150", 0 },	/* Last assignable DHCP address */
	{ "dhcp1_start", "192.168.2.100", 0 },	/* First assignable DHCP address */
	{ "dhcp1_end", "192.168.2.150", 0 },	/* Last assignable DHCP address */
	{ "dhcp_domain", "wan", 0 },		/* Use WAN domain name first if available (wan|lan)
						 */
	{ "dhcp_wins", "wan", 0 },		/* Use WAN WINS first if available (wan|lan) */
#endif	/* __CONFIG_NAT__ */

	/* Web server parameters */
	{ "http_username", "", 0 },		/* Username */
	{ "http_passwd", "admin", 0 },		/* Password */
	{ "http_wanport", "", 0 },		/* WAN port to listen on */
	{ "http_lanport", "80", 0 },		/* LAN port to listen on */
#endif /* #ifndef DSLCPE */

	/* Wireless parameters */
	{ "wl_ifname", "", 0 },			/* Interface name */
	{ "wl_hwaddr", "", 0 },			/* MAC address */
	{ "wl_phytype", "b", 0 },		/* Current wireless band ("a" (5 GHz),
						 * "b" (2.4 GHz), or "g" (2.4 GHz))
						*/
	{ "wl_corerev", "", 0 },		/* Current core revision */
	{ "wl_phytypes", "", 0 },		/* List of supported wireless bands (e.g. "ga") */
	{ "wl_radioids", "", 0 },		/* List of radio IDs */
	{ "wl_ssid", "Broadcom", 0 },		/* Service set ID (network name) */
	{ "wl_bss_enabled", "1", 0 },		/* Service set Enable (1) or disable (0) radio */

#ifdef __CONFIG_HSPOT__
	{ "wl_hsflag",		"1aa0", 0 },	/* Passpoint Flags */
	{ "wl_hs2cap",		"1", 0 },	/* Passpoint Realese 2 (1), Realese 1 (0) radio */
	{ "wl_opercls",		"3", 0 },	/* Operating Class */
	{ "wl_anonai",		"anonymous.com", 0 },	/* Anonymous NAI */
	{ "wl_wanmetrics",	"1:0:0=2500>384=0>0=0", 0 }, /* WAN Metrics */

	{ "wl_oplist",		"Wi-Fi Alliance!eng|"
	"\x57\x69\x2d\x46\x69\xe8\x81\x94\xe7\x9b\x9f!chi", 0 }, /* Operator Friendly Name List */

	{ "wl_homeqlist",	"mail.example.com:rfc4282", 0 }, /* NAIHomeRealmQueryList */

	{ "wl_osu_ssid",	"OSU", 0}, /* OSU SSID */

	{ "wl_osu_frndname",	"SP Red Test Only!eng|"
	"\x53\x50\x20\xEB\xB9\xA8\xEA\xB0\x95\x20\xED\x85\x8C"
	"\xEC\x8A\xA4\xED\x8A\xB8\x20\xEC\xA0\x84\xEC\x9A\xA9!kor", 0}, /* OSU Friendly Name */

	{ "wl_osu_uri",
	"https://osu-server.r2-testbed.wi-fi.org/", 0}, /* OSU Server URI */

	{ "wl_osu_nai",		"", 0}, /* OSU NAI */

	{ "wl_osu_method",	"1", 0}, /* OSU Method */

	{ "wl_osu_icons",
	"icon_red_zxx.png+icon_red_eng.png", 0}, /* OSU Icons */

	{ "wl_osu_servdesc", "Free service for test purpose!eng|"
	"\xED\x85\x8C\xEC\x8A\xA4\xED\x8A\xB8\x20\xEB\xAA\xA9"
	"\xEC\xA0\x81\xEC\x9C\xBC\xEB\xA1\x9C\x20\xEB\xAC\xB4"
	"\xEB\xA3\x8C\x20\xEC\x84\x9C\xEB\xB9\x84\xEC\x8A\xA4!kor", 0}, /* OSU Serv Desc */

	{ "wl_concaplist",	"1:0:0;6:20:1;6:22:0;"
	"6:80:1;6:443:1;6:1723:0;6:5060:0;"
	"17:500:1;17:5060:0;17:4500:1;50:0:1", 0 }, /* Connection Capability List */

	{ "wl_qosmapie",
	"35021606+8,15;0,7;255,255;16,31;32,39;255,255;40,47;255,255", 0 },	/* QoS Map IE */

	/* ---- Passpoint Flags  ----------------------------------- */
	{ "wl_gascbdel",	"0", 0 },	/* GAS CB Delay */
	{ "hs2_debug_level",	"1", 0 },	/* HS2.0 debeug level default (1) ERROR */

/* #endif  __CONFIG_HSPOT__ */
/* #ifdef __CONFIG_802_11U__ */
	{ "wl_iwnettype",	"2", 0 },	/* Select Access Network Type */
	{ "wl_hessid",		"50:6F:9A:00:11:22",  0 },	/* Interworking HESSID */
	{ "wl_ipv4addr",	"3", 0 },	/* Select IPV4 Address Type Availability */
	{ "wl_ipv6addr",	"0", 0 },	/* Select IPV6 Address Type Availability */

	{ "wl_netauthlist", "accepttc=+"
	"httpred=https://tandc-server.wi-fi.org",  0 },	/* Network Authentication Type List */

	{ "wl_venuegrp",	"2", 0 },	/* Venue Group */
	{ "wl_venuetype",	"8", 0 },	/* Venue Type  */

	{ "wl_venuelist",
	"57692D466920416C6C69616E63650A"
	"3239383920436F7070657220526F61640A"
	"53616E746120436C6172612C2043412039"
	"353035312C2055534121656E677C"
	"57692D4669E88194E79B9FE5AE9EE9AA8CE5AEA40A"
	"E4BA8CE4B99DE585ABE4B99DE5B9B4E5BA93E69F8FE8B7AF0A"
	"E59CA3E5858BE68B89E68B892C20E58AA0E588A9E7A68FE5B0"
	"BCE4BA9A39353035312C20E7BE8EE59BBD21636869",  0 },	/* Venue Name List */

	{ "wl_ouilist",		"506F9A:1;001BC504BD:1", 0 },	/* Roaming Consortium List */
	{ "wl_3gpplist",	"",  0 },	/* 3GPP Cellular Network Information List */
	{ "wl_domainlist",	"",  0 },	/* Domain Name List */

	{ "wl_realmlist",
	"mail.example.com+0+21=2,4#5,7?"
	"cisco.com+0+21=2,4#5,7?"
	"wi-fi.org+0+21=2,4#5,7;13=5,6?"
	"example.com+0+13=5,6",  0 },	/* NAI Realm List */

#endif  /* __CONFIG_802_11U__ */

	{ "wl_country_code", "", 0 },		/* Country Code (default obtained from driver) */
	{ "wl_country_rev", "", 0 },	/* Regrev Code (default obtained from driver) */
	{ "wl_radio", "1", 0 },			/* Enable (1) or disable (0) radio */
	{ "wl_closed", "0", 0 },		/* Closed (hidden) network */
	{ "wl_ap_isolate", "0", 0 },            /* AP isolate mode */
#if defined(__CONFIG_PLC__)
	{ "wl_wmf_bss_enable", "1", 0 },	/* WMF Enable for IPTV Media or WiFi+PLC */
#else
	{ "wl_wmf_bss_enable", "1", 0 },	/* WMF Enable/Disable */
#endif	/* __CONFIG_PLC__ */
	{ "wl_mcast_regen_bss_enable", "1", 0 },	/* MCAST REGEN Enable/Disable */
	/* operational capabilities required for stations to associate to the BSS */
	{ "wl_bss_opmode_cap_reqd", "0", 0 },
	{ "wl_rxchain_pwrsave_enable", "1", 0 },	/* Rxchain powersave enable */
	{ "wl_rxchain_pwrsave_quiet_time", "1800", 0 },	/* Quiet time for power save */
	{ "wl_rxchain_pwrsave_pps", "10", 0 },	/* Packets per second threshold for power save */
	{ "wl_rxchain_pwrsave_stas_assoc_check", "0", 0 }, /* STAs associated before powersave */
	{ "wl_radio_pwrsave_enable", "0", 0 },	/* Radio powersave enable */
	{ "wl_radio_pwrsave_quiet_time", "1800", 0 },	/* Quiet time for power save */
	{ "wl_radio_pwrsave_pps", "10", 0 },	/* Packets per second threshold for power save */
	{ "wl_radio_pwrsave_level", "0", 0 },	/* Radio power save level */
	{ "wl_radio_pwrsave_stas_assoc_check", "0", 0 }, /* STAs associated before powersave */
	{ "wl_mode", "ap", 0 },			/* AP mode (ap|sta|wds) */
	{ "wl_lazywds", "0", 0 },		/* Enable "lazy" WDS mode (0|1) */
	{ "wl_wds", "", 0 },			/* xx:xx:xx:xx:xx:xx ... */
	{ "wl_wds_timeout", "1", 0 },		/* WDS link detection interval defualt 1 sec */
	{ "wl_wep", "disabled", 0 },		/* WEP data encryption (enabled|disabled) */
	{ "wl_auth", "0", 0 },			/* Shared key authentication optional (0) or
						 * required (1)
						 */
	{ "wl_key", "1", 0 },			/* Current WEP key */
	{ "wl_key1", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key2", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key3", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key4", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_maclist", "", 0 },		/* xx:xx:xx:xx:xx:xx ... */
	{ "wl_macmode", "disabled", 0 },	/* "allow" only, "deny" only, or "disabled"
						 * (allow all)
						 */
	{ "wl_assoc_retry_max", "3", 0 },	/* Non-zero limit for association retries */
	{ "wl_chanspec", "11", 0 },		/* Channel specification */
	{ "wl_reg_mode", "h", 0 },		/* Regulatory: 802.11H(h)/802.11D(d)/off(off) */
	{ "wl_rate", "0", 0 },			/* Rate (bps, 0 for auto) */
	{ "wl_mrate", "0", 0 },			/* Mcast Rate (bps, 0 for auto) */
	{ "wl_frameburst", "on", 0 },		/* BRCM Frambursting mode (off|on) */
	{ "wl_frameburst_override", "on", 0 },	/* BRCM frameburst override to enable/disable
						 * dynamic framebursting default on
						 */
	{ "wl_rateset", "default", 0 },		/* "default" or "all" or "12" */
	{ "wl_frag", "2346", 0 },		/* Fragmentation threshold */
	{ "wl_rts", "2347", 0 },		/* RTS threshold */
	{ "wl_dtim", "3", 0 },			/* DTIM period */
	{ "wl_bcn", "100", 0 },			/* Beacon interval */
	{ "wl_bcn_rotate", "1", 0 },		/* Beacon rotation */
	{ "wl_plcphdr", "long", 0 },		/* 802.11b PLCP preamble type */
	{ "wl_gmode", XSTR(GMODE_AUTO), 0 },	/* 54g mode */
	{ "wl_gmode_protection", "auto", 0 },	/* 802.11g RTS/CTS protection (off|auto) */
	{ "wl_wme", "auto", 0 },		/* WME mode (off|on|auto) */
	{ "wl_wme_bss_disable", "0", 0 },	/* WME BSS disable advertising (off|on) */
	{ "wl_antdiv", "-1", 0 },		/* Antenna Diversity (-1|0|1|3) */
	{ "wl_infra", "1", 0 },			/* Network Type (BSS/IBSS) */
	{ "wl_bw_cap", "1", 0},			/* BW Cap; 20 MHz */
	{ "wl_nband", "2", 0},			/* N-BAND */
	{ "wl_nmcsidx", "-1", 0},		/* MCS Index for N - rate */
	{ "wl_nmode", "-1", 0},			/* N-mode */
	{ "wl_rifs_advert", "auto", 0},		/* RIFS mode advertisement */
	{ "wl_vlan_prio_mode", "off", 0},	/* VLAN Priority support */
	{ "wl_leddc", "0x640000", 0},		/* 100% duty cycle for LED on router */
	{ "wl_rxstreams", "0", 0},              /* 802.11n Rx Streams, 0 is invalid, WLCONF will
						 * change it to a radio appropriate default
						 */
	{ "wl_txstreams", "0", 0},              /* 802.11n Tx Streams 0, 0 is invalid, WLCONF will
						 * change it to a radio appropriate default
						 */
	{ "wl_stbc_tx", "auto", 0 },		/* Default STBC TX setting */
	{ "wl_stbc_rx", "1", 0 },		/* Default STBC RX setting */
	{ "wl_ampdu", "auto", 0 },		/* Default AMPDU setting */
	/* Default AMPDU retry limit per-tid setting */
	{ "wl_ampdu_rtylimit_tid", "5 5 5 5 5 5 5 5", 0 },
	/* Default AMPDU regular rate retry limit per-tid setting */
	{ "wl_ampdu_rr_rtylimit_tid", "2 2 2 2 2 2 2 2", 0 },
	{ "wl_amsdu", "auto", 0 },		/* Default AMSDU setting */
	{ "wl_obss_coex", "1", 0 },		/* Default OBSS Coexistence setting - OFF */

	/* WPA parameters */
	{ "wl_auth_mode", "none", 0 },		/* Network authentication mode (radius|none) */
	{ "wl_wpa_psk", "", 0 },		/* WPA pre-shared key */
	{ "wl_wpa_gtk_rekey", "0", 0 },		/* GTK rotation interval */
	{ "wl_radius_ipaddr", "", 0 },		/* RADIUS server IP address */
	{ "wl_radius_key", "", 0 },		/* RADIUS shared secret */
	{ "wl_radius_port", "1812", 0 },	/* RADIUS server UDP port */
	{ "wl_crypto", "tkip+aes", 0 },		/* WPA data encryption */
	{ "wl_net_reauth", "36000", 0 },	/* Network Re-auth/PMK caching duration */
	{ "wl_akm", "", 0 },			/* WPA akm list */
#ifdef MFP
	{ "wl_mfp", "-1", 0 },			/* Protected Management Frame */
#endif // endif
	{ "wl_psr_mrpt", "0", 0 },		/* Default to one level repeating mode */

#ifdef __CONFIG_WPS__
	/* WSC parameters */
	{ "wps_version2", "enabled", 0 },	 /* Must specified before other wps variables */
	{ "wl_wps_mode", "enabled", 0 }, /* enabled wps */
	{ "wl_wps_config_state", "0", 0 },	/* config state unconfiged */
	{ "wps_device_pin", "12345670", 0 },
	{ "wps_modelname", "Broadcom", 0 },
	{ "wps_mfstring", "Broadcom", 0 },
	{ "wps_device_name", "BroadcomAP", 0 },
	{ "wl_wps_reg", "enabled", 0 },
	{ "wps_sta_pin", "00000000", 0 },
	{ "wps_modelnum", "123456", 0 },
	/* Allow or Deny Wireless External Registrar get or configure AP security settings */
	{ "wps_wer_mode", "allow", 0 },

	{ "lan_wps_oob", "enabled", 0 },	/* OOB state */
	{ "lan_wps_reg", "enabled", 0 },	/* For DTM 1.4 test */

	{ "lan1_wps_oob", "enabled", 0 },
	{ "lan1_wps_reg", "enabled", 0 },
#endif /* __CONFIG_WPS__ */
#ifdef __CONFIG_WFI__
	{ "wl_wfi_enable", "0", 0 },	/* 0: disable, 1: enable WifiInvite */
	{ "wl_wfi_pinmode", "0", 0 },	/* 0: auto pin, 1: manual pin */
#endif /* __CONFIG_WFI__ */
#ifdef __CONFIG_WAPI_IAS__
	/* WAPI parameters */
	{ "wl_wai_cert_name", "", 0 },		/* AP certificate name */
	{ "wl_wai_cert_index", "1", 0 },	/* AP certificate index. 1:X.509, 2:GBW */
	{ "wl_wai_cert_status", "0", 0 },	/* AP certificate status */
	{ "wl_wai_as_ip", "", 0 },		/* ASU server IP address */
	{ "wl_wai_as_port", "3810", 0 },	/* ASU server UDP port */
#endif /* __CONFIG_WAPI_IAS__ */
	/* WME parameters (cwmin cwmax aifsn txop_b txop_ag adm_control oldest_first) */
	/* EDCA parameters for STA */
	{ "wl_wme_sta_be", "15 1023 3 0 0 off off", 0 },	/* WME STA AC_BE parameters */
	{ "wl_wme_sta_bk", "15 1023 7 0 0 off off", 0 },	/* WME STA AC_BK parameters */
	{ "wl_wme_sta_vi", "7 15 2 6016 3008 off off", 0 },	/* WME STA AC_VI parameters */
	{ "wl_wme_sta_vo", "3 7 2 3264 1504 off off", 0 },	/* WME STA AC_VO parameters */

	/* EDCA parameters for AP */
	{ "wl_wme_ap_be", "15 63 3 0 0 off off", 0 },		/* WME AP AC_BE parameters */
	{ "wl_wme_ap_bk", "15 1023 7 0 0 off off", 0 },		/* WME AP AC_BK parameters */
	{ "wl_wme_ap_vi", "7 15 1 6016 3008 off off", 0 },	/* WME AP AC_VI parameters */
	{ "wl_wme_ap_vo", "3 7 1 3264 1504 off off", 0 },	/* WME AP AC_VO parameters */

	{ "wl_wme_no_ack", "off", 0},		/* WME No-Acknowledgment mode */
	{ "wl_wme_apsd", "on", 0},		/* WME APSD mode */

#ifdef __CONFIG_ROUTER_MINI__
	{ "wl_maxassoc", "64", 0},		/* Max associations driver could support */
	{ "wl_bss_maxassoc", "64", 0},		/* Max associations driver could support */
#else
	{ "wl_maxassoc", "128", 0},		/* Max associations driver could support */
	{ "wl_bss_maxassoc", "128", 0},		/* Max associations driver could support */
#endif /* __CONFIG_ROUTER_MINI__ */

	{ "wl_unit", "0", 0 },			/* Last configured interface */
	{ "wl_sta_retry_time", "5", 0 }, /* Seconds between association attempts */
#ifdef BCMDBG
	{ "wl_nas_dbg", "0", 0 }, /* Enable/Disable NAS Debugging messages */
#endif // endif

#ifdef __CONFIG_EMF__
	/* EMF defaults */
	{ "emf_entry", "", 0 },			/* Static MFDB entry (mgrp:if) */
	{ "emf_uffp_entry", "", 0 },		/* Unreg frames forwarding ports */
	{ "emf_rtport_entry", "", 0 },		/* IGMP frames forwarding ports */
	{ "emf_enable", "1", 0 },		/* Enable EMF by default */
#endif /* __CONFIG_EMF__ */
#ifndef DSLCPE
#ifdef __CONFIG_IPV6__
	{ "lan_ipv6_mode", "3", 0 },		/* 0=disable 1=6to4 2=native 3=6to4+native! */
	{ "lan_ipv6_dns", "", 0  },
	{ "lan_ipv6_6to4id", "0", 0  }, /* 0~65535 */
	{ "lan_ipv6_prefix", "2001:db6:1:0::/64", 0  },
	{ "wan_ipv6_prefix", "2001:db0:1:0::/64", 0  },
#endif /* __CONFIG_IPV6__ */
#ifdef __CONFIG_NETBOOT__
	{ "netboot_url", "", 0 },		/* netboot url */
	{ "netboot_username", "", 0 },	/* netboot username */
	{ "netboot_passwd", "", 0 },	/* netboor password */
#endif /* __CONFIG_NETBOOT__ */
	/* Restore defaults */
	{ "restore_defaults", "0", 0 },		/* Set to 0 to not restore defaults on boot */
#endif /* DSLCPE */
#if defined(__CONFIG_EXTACS__)
	{ "acs_ifnames", "", 0  },
#endif /* defined(__CONFIG_EXTACS__) */
#ifdef __CONFIG_SAMBA__
	{ "samba_mode", "", 0  },
	{ "samba_passwd", "", 0  },
#endif // endif

#ifdef __CONFIG_PLC__
	{ "igmp_enable", "1", 0 },              /* Enable igmp proxy in AP mode */
#else
	{ "igmp_enable", "0", 0 },              /* Disable igmp proxy in AP mode */
#endif  /* __CONFIG_PLC__ */

	{ "wl_wet_tunnel", "0", 0  },   /* Disable wet tunnel */

	{ "dpsta_ifnames", "", 0  },
	{ "dpsta_policy", "1", 0  },
	{ "dpsta_lan_uif", "1", 0  },
#ifdef TRAFFIC_MGMT_RSSI_POLICY
	{ "wl_trf_mgmt_rssi_policy", "0", 0 }, /* Disable RSSI (default) */
#endif /* TRAFFIC_MGMT_RSSI_POLICY */
#ifdef __CONFIG_EMF__
	{ "wl_wmf_ucigmp_query", "1", 0 },	/* Enable converting IGMP Query to ucast */
	{ "wl_wmf_mdata_sendup", "1", 0 },	/* Enable sending Multicast Data to host */
	{ "wl_wmf_ucast_upnp", "1", 0 },	/* Enable converting upnp to ucast */
	{ "wl_wmf_igmpq_filter", "1", 0 },	/* Enable igmp query filter */
#endif /* __CONFIG_EMF__ */

	/* Tx Beamforming */
	{ "wl_txbf_bfr_cap", "2", 0 },
	{ "wl_txbf_bfe_cap", "2", 0 },
	{ "wl_txbf_imp", "1", 0 },
	{ "wl_mu_features", "0x8000", 0},

	/* acsd setting */
	{ "wl_acs_fcs_mode", "1", 0 },		/* acsd disable FCS mode */
	{ "wl_dcs_csa_unicast", "1", 0 },	/* disable unicast csa */
	{ "wl_acs_excl_chans", "", 0 },		/* acsd exclude chanspec list */
	{ "wl_acs_dfs", "2", 0 },		/* acsd fcs disable init DFS chan */
	{ "wl_acs_dfsr_immediate", "300 3", 0 },   /* immediate if > 3 switches last 5 minutes */
	{ "wl_acs_dfsr_deferred", "604800 5", 0 }, /* deferred if > 5 switches in last 7 days */
	{ "wl_acs_dfsr_activity", "30 10240", 0 }, /* active: >10k I/O in the last 30 seconds */
	{ "wl_acs_cs_scan_timer", "900", 0 },	/* acsd fcs cs scan timeout */
	{ "wl_acs_ci_scan_timer", "4", 0 },	/* acsd fcs CI scan period */
	{ "wl_acs_ci_scan_timeout", "300", 0 },	/* acsd fcs CI scan timeout */
	{ "wl_acs_scan_entry_expire", "3600", 0 },	/* acsd fcs scan expier time */
	{ "wl_acs_tx_idle_cnt", "0", 0 },		/* acsd fcs tx idle thld */
	{ "wl_acs_chan_dwell_time", "70", 0 },	/* acsd fcs chan dwell time */
	{ "wl_acs_chan_flop_period", "70", 0 },	/* acsd fcs chan flip-flop time */
	{ "acs_no_restrict_align", "1", 0 },	/* acsd pri/ext ch OBSS alignment restriction */
	{ "wl_intfer_period", "1", 0 },		/* acsd fcs sample period */
	{ "wl_intfer_cnt", "3", 0 },		/* acsd fcs sample cnt */
	{ "wl_intfer_txfail", "5", 0 },		/* fcs non-TCP txfail threshold setting */
	{ "wl_intfer_tcptxfail", "5", 0 },	/* fcs TCP txfail threshold setting */
	{ "wl_acs_bgdfs_enab", "1", 0 },	/* acsd BGDFS toggle */
	{ "wl_acs_bgdfs_ahead", "1", 0 },	/* acsd BGDFS ahead toggle */
	{ "wl_acs_bgdfs_idle_interval", "3600", 0 },	 /* acsd traffic idle time before BGDFS */
	{ "wl_acs_bgdfs_idle_frames_thld", "36000", 0 }, /* acsd max frames for idle */
	{ "wl_acs_bgdfs_avoid_on_far_sta", "1", 0 },	/* bgdfs avoided by default on far sta */
	{ "wl_acs_far_sta_rssi", "-75", 0 },	/* rssi threshold for far sta */
	{ "wl_acs_bgdfs_fallback_blocking_cac", "1", 0},	/* full MIMO blocking CAC */
	{ "wl_acs_bgdfs_txblank_threshold", "50", 0},	/* tx blanking threshold for 3+1 */
	{ "wl_dyn160", "", 0 },			/* dyn160 toggle */
	{ "wl_phy_dyn_switch_th", "", 0 },	/* phy_dyn_switch_th (threshold values) */
	{ "wl_acsd_scs_dfs_scan", "0", 0},	/* acsd_scs_dfs_scan in scs mode */
	{ "wl_acs_start_on_nondfs", "0", 0},    /* acs_start_on_nondfs toggle */
	{ "wl_atf", "1", 0 },			/* Enable ATF */
	{ "wl_taf_enable", "0", 0 }, /* Disable TAF */

	{ "bsd_role", "0", 0 },              /* Disable Band Steer Daemon */
	{ "bsd_hport", "9877", 0 },          /* BSD helper port */
	{ "bsd_pport", "9878", 0 },          /* BSD Primary port */
	{ "bsd_helper", "192.168.1.2", 0 },  /* BSD primary ipaddr */
	{ "bsd_primary", "192.168.1.1", 0 }, /* BSD Helper ipaddr */
	{ "ssd_enable", "0", 0 },            /* Disable SSID Steer Daemon */
	{ "wl_ssd_type", "0", 0 },           /* default ssd_type "disabled" */
	{ "wl_dfs_pref", "" },               /* DFS Preferred channel value  */
	{ "wl_probresp_mf", "0", 0 },	/* MAC filter based probe response  */
	{ "wl_probresp_sw", "1", 0 },	/* SW probe response  */
	{ "wl_vhtmode", "-1", 0 },	/* VHT mode  */
	{ "wl_vht_features", "-1", 0 },	/* VHT features  */
	{ "wl_obss_dyn_bw", "0", 0}, /* Dynamic BWSW disable defaults */
#ifdef __CONFIG_VISUALIZATION__
	{ "vis_dcon_ipaddr", "127.0.0.1", 0 },	/* Visualization Dcon IP address  */
	{ "vis_do_remote_dcon", "0", 0 },	/* Visualization remote debugging enabled flag  */
	{ "vis_debug_level", "1", 0},		/* Visualization debug messages to be printed */
	{ "vis_disable_animate", "0", 0},	/* Visualization site survey animation on or off */
	{ "wl_rrm", "0", 0},			/* Set rrm caps per interface */
#endif /* __CONFIG_VISUALIZATION__ */
#ifdef __CONFIG_TREND_IQOS__
	{ "iqos_debug_level", "0", 0},		/* iQoS debug messages to be printed */
	{ "iqos_refresh_interval", "5", 0},	/* iQoS debug messages to be printed */
#endif /* __CONFIG_TREND_IQOS__ */
	{ "wl_acs_boot_only", "0", 0 },		/* enable acsd full operation */
#ifdef __CONFIG_LBR_AGGR__
	{ "wl_lbr_aggr_en_mask", "0", 0},	        /* per tid/ac mask disable by default */
	{ "wl_lbr_aggr_len", "16", 0},	                /* default aggregate len */
	{ "wl_lbr_aggr_release_timeout", "10", 0},	/* default release timeout in msec */
#endif /* __CONFIG_LBR_AGGR__ */
	{ "wl_acs_use_escan", "1", 0},		/* Enable acsd enhanced scan mode */
	{ "wl_flr_lfrag_max", "400", 0 },
	{ "wl_flr_lfrag_txpkts_min", "1", 0 },
	{ "wl_flr_lfrag_txpkts_adjust", "3", 0 },
	{ "wl_pspretend_retry_limit", "5", 0 }, /* Enable PsPretend */
	{ "wl_cal_period", "-1", 0 },            /* Disable periodic cal */
	{ "wl_psta_inact", "600", 0 },          /* PSTA inactivity timer */
	{ "wl_fbt_generate_local", "", 0},	/* Generate PMKs locally */
	{ "wl_atm_bssperc", "", 0},		/* ATM BSS percentage */
	{ 0, 0, 0 }
};

/* Translates from, for example, wl0_ (or wl0.1_) to wl_. */
/* Only single digits are currently supported */

static void
fix_name(const char *name, char *fixed_name)
{
	char *pSuffix = NULL;

	/* Translate prefix wlx_ and wlx.y_ to wl_ */
	/* Expected inputs are: wld_root, wld.d_root, wld.dd_root
	 * We accept: wld + '_' anywhere
	 */
	pSuffix = strchr(name, '_');

	if ((strncmp(name, "wl", 2) == 0) && isdigit(name[2]) && (pSuffix != NULL)) {
		strcpy(fixed_name, "wl");
		strcpy(&fixed_name[2], pSuffix);
		return;
	}

	/* No match with above rules: default to input name */
	strcpy(fixed_name, name);
}

/*
 * Find nvram param name; return pointer which should be treated as const
 * return NULL if not found.
 *
 * NOTE:  This routine special-cases the variable wl_bss_enabled.  It will
 * return the normal default value if asked for wl_ or wl0_.  But it will
 * return 0 if asked for a virtual BSS reference like wl0.1_.
 */
char *
nvram_default_get(const char *name)
{
	int idx;
	char fixed_name[NVRAM_MAX_VALUE_LEN];

	fix_name(name, fixed_name);
	if (strcmp(fixed_name, "wl_bss_enabled") == 0) {
		if (name[3] == '.' || name[4] == '.') { /* Virtual interface */
			return "0";
		}
	}

	for (idx = 0; router_defaults[idx].name != NULL; idx++) {
		if (strcmp(router_defaults[idx].name, fixed_name) == 0) {
			return router_defaults[idx].value;
		}
	}

	return NULL;
}
/* validate/restore all per-interface related variables */
void
nvram_validate_all(char *prefix, bool restore)
{
	struct nvram_tuple *t;
	char tmp[100];
	char *v;

	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3)) {
			strcat_r(prefix, &t->name[3], tmp);
			if (!restore && nvram_get(tmp))
				continue;
			v = nvram_get(t->name);
			nvram_set(tmp, v ? v : t->value);
		}
	}
}

/* Initialize all per-interface related variables */
void
nvram_initialize_all(char *prefix)
{
	struct nvram_tuple *t;
	char tmp[100];

	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3)) {
			strcat_r(prefix, &t->name[3], tmp);
			nvram_set(tmp, t->value);
		}
	}
}

/* restore specific per-interface variable */
void
nvram_restore_var(char *prefix, char *name)
{
	struct nvram_tuple *t;
	char tmp[100];

	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3) && !strcmp(&t->name[3], name)) {
			nvram_set(strcat_r(prefix, name, tmp), t->value);
			break;
		}
	}
}

#ifdef WLHOSTFBT

/* Clear FBT_APs NVRAMS based on prefix */
static void
fbt_aps_restore_defaults(char *prefix, int max_nvparse)
{
	char tmp_prefix[] = "wlXXXXXXXXXXXXXXXXXXXXXXXXXXXX_mssid_";
	char *fbt_aps, *next;
	char tmp[100], fbt_ap[100], tmp_fbt_ap[100];
	int iter_param, i, j;

	/* List of FBT_AP NVRAMs */
	char* fbt_ap_nvrams[] = {
		"addr",
		"r1kh_id",
		"r0kh_id",
		"r0kh_id_len",
		"br_addr",
		"r0kh_key",
		"r1kh_key",
	};

	/* Get fbt_all_gen_aps NVRAM */
	fbt_aps = nvram_safe_get(strcat_r(prefix, "fbt_all_gen_aps", tmp));
	/* If no values, no need to restore those */
	if (strlen(fbt_aps) <= 0) {
		goto fbt_all_aps;
	}

	/* For each fbt_all_gen_aps, clear the fbt_ap_nvrams */
	foreach(fbt_ap, fbt_aps, next) {
		/* Traverse through all FBT_AP NVRAMs */
		for (iter_param = 0; iter_param < ARRAYSIZE(fbt_ap_nvrams); iter_param++) {
			snprintf(tmp_fbt_ap, sizeof(tmp_fbt_ap), "%s_%s", fbt_ap,
				fbt_ap_nvrams[iter_param]);
			nvram_unset(tmp_fbt_ap);
		}
	}

fbt_all_aps:
	/* Get fbt_all_aps NVRAM to clear the fbt_bssid NVRAM */
	fbt_aps = nvram_safe_get(strcat_r(prefix, "fbt_all_aps", tmp));
	/* If no values, no need to restore those */
	if (strlen(fbt_aps) <= 0) {
		return;
	}

	/* For each fbt_aps, clear the fbt_bssid nvrams */
	foreach(fbt_ap, fbt_aps, next) {
		/* Traverse through all Primary Prefix to restore fbt_aps NVRAMs */
		for (i = 0; i < max_nvparse; i++) {
			sprintf(tmp_prefix, "wl%d_", i);
			snprintf(tmp_fbt_ap, sizeof(tmp_fbt_ap), "%s%s_fbt_bssid",
				tmp_prefix, fbt_ap);
			nvram_unset(tmp_fbt_ap);

			/* Traverse through all Virtual Prefix */
			for (j = 0; j < WL_MAXBSSCFG; j++) {
				sprintf(tmp_prefix, "wl%d.%d_", i, j);
				snprintf(tmp_fbt_ap, sizeof(tmp_fbt_ap), "%s%s_fbt_bssid",
					tmp_prefix, fbt_ap);
				nvram_unset(tmp_fbt_ap);
			}
		}
	}
}

/* Clear all the FBT NVRAMs */
void
fbt_restore_defaults(char *in_prefix, int max_nvparse)
{
	char tmp[100], prefix[] = "wlXXXXXXXXXXXXXXXXXXXXXXXXXXXX_mssid_";
	int i, j, iter_param;

	/* List of FBT NVRAMs */
	char* fbt_nvrams[] = {
		"fbt",
		"fbt_mdid",
		"fbtoverds",
		"fbt_reassoc_time",
		"fbt_ap",
		"r0kh_id",
		"r1kh_id",
		"r0kh_key",
		"fbt_aps",
		"fbt_all_aps",
		"fbt_all_gen_aps",
		"wbd_fbt"
	};

	/* If prefix is passed. Clear the NVRAMs with that prefix only */
	if (in_prefix != NULL) {
		fbt_aps_restore_defaults(in_prefix, max_nvparse);

		/* Traverse through all FBT NVRAMs */
		for (iter_param = 0; iter_param < ARRAYSIZE(fbt_nvrams); iter_param++) {
			/* Clear NVRAMs without Prefix */
			nvram_unset(fbt_nvrams[iter_param]);

			nvram_unset(strcat_r(in_prefix, fbt_nvrams[iter_param], tmp));
		}
		/* Cleared all the FBT NVRAMs with prefix passed. No need to move further */
		return;
	}

	/* Traverse through all Primary Prefix to restore fbt_aps NVRAMs */
	for (i = 0; i < max_nvparse; i++) {
		sprintf(prefix, "wl%d_", i);
		fbt_aps_restore_defaults(prefix, max_nvparse);

		/* Traverse through all Virtual Prefix */
		for (j = 0; j < WL_MAXBSSCFG; j++) {
			sprintf(prefix, "wl%d.%d_", i, j);
			fbt_aps_restore_defaults(prefix, max_nvparse);
		}
	}

	/* Traverse through all FBT NVRAMs */
	for (iter_param = 0; iter_param < ARRAYSIZE(fbt_nvrams); iter_param++) {
		/* Clear NVRAMs without Prefix */
		nvram_unset(fbt_nvrams[iter_param]);

		/* Traverse through all Primary Prefix */
		for (i = 0; i < max_nvparse; i++) {
			sprintf(prefix, "wl%d_", i);

			/* Clear NVRAMs with Primary Prefix */
			nvram_unset(strcat_r(prefix, fbt_nvrams[iter_param], tmp));

			/* Traverse through all Virtual Prefix */
			for (j = 0; j < WL_MAXBSSCFG; j++) {
				sprintf(prefix, "wl%d.%d_", i, j);

				/* Clear NVRAMs with Virtual Prefix */
				nvram_unset(strcat_r(prefix, fbt_nvrams[iter_param], tmp));
			}
		}
	}
}
#endif /* WLHOSTFBT */
