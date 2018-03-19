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
 * $Id: wbd_rc_shared.h 714901 2017-08-08 14:32:18Z $
 */

#ifndef _WBD_RC_SHARED_H_
#define _WBD_RC_SHARED_H_

/* RC & NVRAM operation Flags */
#define WBD_FLG_RC_RESTART			0X0010
#define WBD_FLG_REBOOT				0X0020
#define WBD_FLG_NV_COMMIT			0X0040

/* ----------------------------- WBD shared Routines --------------------------------- */

/* Check if Interface is Virtual Interface, if Disabled, Enable it */
extern uint32 wbd_enable_vif(char *ifname);

/* Check if Interface is DWDS Primary Interface, with mode = STA, Change name1 */
extern int wbd_check_dwds_sta_primif(char *ifname, char *ifname1, int len1);

/* Find First DWDS Primary Interface, with mode = STA */
extern int wbd_find_dwds_sta_primif(char *ifname, int len, char *ifname1, int len1);

/* Get "wbd_ifnames" from "lan_ifnames" */
extern int wbd_ifnames_fm_lan_ifnames(char *wbd_ifnames, int len, char *wbd_ifnames1, int len1);

/* Get next available Virtual Interface Subunit */
extern int wbd_get_next_vif_subunit(int in_unit, int *error);

/* Create & Configure Virtual AP Interface, if WBD is ON and AP is WBD DWDS Slave */
extern int wbd_create_vif(int unit, int subunit);

/* Read "wbd_ifnames" NVRAM and get actual ifnames */
extern int wbd_read_actual_ifnames(char *wbd_ifnames1, int len1, bool create);

/* Find Number of valid interfaces */
extern int wbd_count_interfaces(void);

/* Get MDID from NVRAM, if not present generate and set the mdid */
#ifdef WLHOSTFBT
extern uint16 wbd_get_mdid(char *prefix);
#endif /* WLHOSTFBT */

/* Executes nvram commit/rc restart/reboot commands */
extern void wbd_do_rc_restart_reboot(uint32 rc_flags);

/* Find Backhaul Prim Ifr Configured on this Device (RootAP/Repeater), Check if its Dedicated */
extern int wbd_find_backhaul_primif_on_device(char *backhaul_if,
	int backhaul_if_len, int *repeat_backhaul);
/* ----------------------------- WBD shared Routines --------------------------------- */

#endif /* _WBD_RC_SHARED_H_ */
