/*
 * ACS deamon (Linux)
 *
 * Copyright 2017 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 * $Id: acsd_main.c 708250 2017-06-30 10:30:52Z $
 */

#include <proto/ethernet.h>
#include <proto/bcmeth.h>
#include <proto/bcmevent.h>
#include <proto/802.11.h>

#include "acsd_svr.h"
#include "acs_dfsr.h"

acsd_wksp_t *d_info;

/* open a UDP packet to event dispatcher for receiving/sending data */
static int
acsd_open_eventfd()
{
	int reuse = 1;
	struct sockaddr_in sockaddr;
	int fd = ACSD_DFLT_FD;

	/* open loopback socket to communicate with event dispatcher */
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sockaddr.sin_port = htons(EAPD_WKSP_DCS_UDP_SPORT);

	if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		ACSD_ERROR("Unable to create loopback socket\n");
		goto exit1;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
		ACSD_ERROR("Unable to setsockopt to loopback socket %d.\n", fd);
		goto exit1;
	}

	if (bind(fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
		ACSD_ERROR("Unable to bind to loopback socket %d\n", fd);
		goto exit1;
	}

	ACSD_INFO("opened loopback socket %d\n", fd);
	d_info->event_fd = fd;

	return ACSD_OK;

	/* error handling */
exit1:
	if (fd != ACSD_DFLT_FD) {
		close(fd);
	}
	return errno;
}

static int
acsd_svr_socket_init(unsigned int port)
{
	int reuse = 1;
	struct sockaddr_in s_sock;

	d_info->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (d_info->listen_fd < 0) {
		ACSD_ERROR("Socket open failed: %s\n", strerror(errno));
		return ACSD_FAIL;
	}

	if (setsockopt(d_info->listen_fd, SOL_SOCKET, SO_REUSEADDR,
		(char*)&reuse, sizeof(reuse)) < 0) {
		ACSD_ERROR("Unable to setsockopt to server socket %d.\n", d_info->listen_fd);
		return ACSD_FAIL;
	}

	memset(&s_sock, 0, sizeof(struct sockaddr_in));
	s_sock.sin_family = AF_INET;
	s_sock.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	s_sock.sin_port = htons(port);

	if (bind(d_info->listen_fd, (struct sockaddr *)&s_sock,
		sizeof(struct sockaddr_in)) < 0) {
		ACSD_ERROR("Socket bind failed: %s\n", strerror(errno));
		return ACSD_FAIL;
	}

	if (listen(d_info->listen_fd, 5) < 0) {
		ACSD_ERROR("Socket listen failed: %s\n", strerror(errno));
		return ACSD_FAIL;
	}

	return ACSD_OK;
}

static void
acsd_close_listenfd()
{
	/* close event dispatcher socket */
	if (d_info->listen_fd != ACSD_DFLT_FD) {
		ACSD_INFO("listenfd: close  socket %d\n", d_info->listen_fd);
		close(d_info->listen_fd);
		d_info->event_fd = ACSD_DFLT_FD;
	}
	return;
}

static void
acsd_close_eventfd()
{
	/* close event dispatcher socket */
	if (d_info->event_fd != ACSD_DFLT_FD) {
		ACSD_INFO("eventfd: close loopback socket %d\n", d_info->event_fd);
		close(d_info->event_fd);
		d_info->event_fd = ACSD_DFLT_FD;
	}
	return;
}

static int
acsd_validate_wlpvt_message(int bytes, uint8 *dpkt)
{
	bcm_event_t *pvt_data;

	/* the message should be at least the header to even look at it */
	if (bytes < sizeof(bcm_event_t) + 2) {
		ACSD_ERROR("Invalid length of message\n");
		goto error_exit;
	}
	pvt_data  = (bcm_event_t *)dpkt;
	if (ntohs(pvt_data->bcm_hdr.subtype) != BCMILCP_SUBTYPE_VENDOR_LONG) {
		ACSD_ERROR("%s: not vendor specifictype\n",
		       pvt_data->event.ifname);
		goto error_exit;
	}
	if (pvt_data->bcm_hdr.version != BCMILCP_BCM_SUBTYPEHDR_VERSION) {
		ACSD_ERROR("%s: subtype header version mismatch\n",
			pvt_data->event.ifname);
		goto error_exit;
	}
	if (ntohs(pvt_data->bcm_hdr.length) < BCMILCP_BCM_SUBTYPEHDR_MINLENGTH) {
		ACSD_ERROR("%s: subtype hdr length not even minimum\n",
			pvt_data->event.ifname);
		goto error_exit;
	}
	if (bcmp(&pvt_data->bcm_hdr.oui[0], BRCM_OUI, DOT11_OUI_LEN) != 0) {
		ACSD_ERROR("%s: acsd_validate_wlpvt_message: not BRCM OUI\n",
			pvt_data->event.ifname);
		goto error_exit;
	}
	/* check for wl dcs message types */
	switch (ntohs(pvt_data->bcm_hdr.usr_subtype)) {
		case BCMILCP_BCM_SUBTYPE_EVENT:

			ACSD_INFO("subtype: event\n");
			break;
		default:
			goto error_exit;
			break;
	}
	return ACSD_OK; /* good packet may be this is destined to us */
error_exit:
	return ACSD_FAIL;
}

/*
 * Receives and processes the commands from client
 * o Wait for connection from client
 * o Process the command and respond back to client
 * o close connection with client
 */
static int
acsd_proc_client_req(void)
{
	uint resp_size = 0;
	int rcount = 0;
	int fd = -1;
	struct sockaddr_in cliaddr;
	uint len = 0; /* need initialize here to avoid EINVAL */
	char* buf;
	int ret = 0;

	fd = accept(d_info->listen_fd, (struct sockaddr*)&cliaddr,
		&len);
	if (fd < 0) {
		if (errno == EINTR) return 0;
		else {
			ACSD_ERROR("accept failed: errno: %d - %s\n", errno, strerror(errno));
			return -1;
		}
	}
	d_info->conn_fd = fd;

	if (!d_info->cmd_buf)
		d_info->cmd_buf = acsd_malloc(ACSD_BUFSIZE_4K);

	buf = d_info->cmd_buf;

	/* get command from client */
	if ((rcount = sread(d_info->conn_fd, buf, ACSD_BUFSIZE_4K)) < 0) {
		ACSD_ERROR("Failed reading message from client: %s\n", strerror(errno));
		ret = -1;
		goto done;
	}

	/* reqeust is small string. */
	if (rcount == ACSD_BUFSIZE_4K) {
		ACSD_ERROR("Client Req too large\n");
		ret = -1;
		goto done;
	}
	buf[rcount] = '\0';

	acsd_proc_cmd(d_info, buf, rcount, &resp_size);

	if (swrite(d_info->conn_fd, buf, resp_size) < 0) {
		ACSD_ERROR("Failed sending message: %s\n", strerror(errno));
		ret = -1;
		goto done;
	}

done:
	close(d_info->conn_fd);
	d_info->conn_fd = -1;
	return ret;
}

/* Check if stay in current channel long enough */
static bool
chanim_record_chan_dwell(acs_chaninfo_t *c_info, chanim_info_t *ch_info)
{
	acs_fcs_t *dcs_info = &c_info->acs_fcs;
	uint8 cur_idx = chanim_mark(ch_info).record_idx;
	uint8 start_idx;
	chanim_acs_record_t *start_record;
	time_t now = time(NULL);

	/* negative value implies infinite dwell time; negative implies infinity */
	if (dcs_info->acs_chan_dwell_time < 0) {
		return FALSE;
	}

	start_idx = MODSUB(cur_idx, 1, CHANIM_ACS_RECORD);
	start_record = &ch_info->record[start_idx];
	if (now - start_record->timestamp > dcs_info->acs_chan_dwell_time)
		return TRUE;
	return FALSE;
}

/* listen to sockets and call handlers to process packets */
void
acsd_main_loop(struct timeval *tv)
{
	fd_set fdset;
	int width, status = 0, bytes, len;
	uint8 *pkt;
	bcm_event_t *pvt_data;
	bool chan_least_dwell = FALSE;
	wl_bcmdcs_data_t dcs_data;
	acs_chaninfo_t *c_info;
	int idx = 0;
	int err;
	uint32 escan_event_status;
	wl_escan_result_t *escan_data = NULL;
	struct escan_bss *result;

#ifdef DEBUG
	char test_cswitch_ifname[32];

	strncpy(test_cswitch_ifname, nvram_safe_get("acs_test_cs"), sizeof(test_cswitch_ifname));
	test_cswitch_ifname[sizeof(test_cswitch_ifname) - 1] = '\0';
	nvram_unset("acs_test_cs");

	if (test_cswitch_ifname[0]) {
		if ((idx = acs_idx_from_map(test_cswitch_ifname)) < 0) {
			ACSD_ERROR("cannot find the mapped entry for ifname: %s\n",
				test_cswitch_ifname);
			return;
		}
		c_info = d_info->acs_info->chan_info[idx];

		ACSD_FCS("trigger Fake cswitch from:0x%x:...\n", c_info->cur_chspec);

		if (acsd_trigger_dfsr_check(c_info)) {
			ACSD_DFSR("trigger DFS reentry...\n");
			acs_dfsr_set(ACS_DFSR_CTX(c_info), c_info->cur_chspec, __FUNCTION__);
		}
		else {
			if (acsd_hi_chan_check(c_info)) {
				acs_get_best_dfs_forced_chspec(c_info);
				c_info->selected_chspec = acs_adjust_ctrl_chan(c_info,
					c_info->dfs_forced_chspec);
				ACSD_FCS("Select 0x%x:...\n", c_info->selected_chspec);
			}
			else {
				c_info->switch_reason = APCS_TXFAIL;
				acs_select_chspec(c_info);
			}
			dcs_data.reason = 1;
			dcs_data.chspec = c_info->selected_chspec;

			if ((err = dcs_handle_request(test_cswitch_ifname, &dcs_data,
							DOT11_CSA_MODE_ADVISORY, FCS_CSA_COUNT,
#ifdef DSLCPE_ENDIAN
							c_info->acs_fcs.acs_dcs_csa, c_info))) {
#else
							c_info->acs_fcs.acs_dcs_csa))) {
#endif
				ACSD_ERROR("err from dcs_handle_request: %d\n", err);
			}
			else {
				acs_intfer_config(c_info);
				chanim_upd_acs_record(c_info->chanim_info,
						c_info->selected_chspec, APCS_TXFAIL);
			}
		}
	}
#endif /* DEBUG */

	/* init file descriptor set */
	FD_ZERO(&d_info->fdset);
	d_info->fdmax = -1;

	/* build file descriptor set now to save time later */
	if (d_info->event_fd != ACSD_DFLT_FD) {
		FD_SET(d_info->event_fd, &d_info->fdset);
		d_info->fdmax = d_info->event_fd;
	}

	if (d_info->listen_fd != ACSD_DFLT_FD) {
		FD_SET(d_info->listen_fd, &d_info->fdset);
		if (d_info->listen_fd > d_info->fdmax)
			d_info->fdmax = d_info->listen_fd;
	}

	pkt = d_info->packet;
	len = sizeof(d_info->packet);
	width = d_info->fdmax + 1;
	fdset = d_info->fdset;

	/* listen to data availible on all sockets */
	status = select(width, &fdset, NULL, NULL, tv);

	if ((status == -1 && errno == EINTR) || (status == 0))
		return;

	if (status <= 0) {
		ACSD_ERROR("err from select: %s", strerror(errno));
		return;
	}

	if (d_info->listen_fd != ACSD_DFLT_FD && FD_ISSET(d_info->listen_fd, &fdset)) {
		ACSD_INFO("recved command from a client\n");

		d_info->stats.num_cmds++;
		acsd_proc_client_req();
	}

	/* handle brcm event */
	if (d_info->event_fd !=  ACSD_DFLT_FD && FD_ISSET(d_info->event_fd, &fdset)) {
		char *ifname = (char *)pkt;
		struct ether_header *eth_hdr = (struct ether_header *)(ifname + IFNAMSIZ);
		uint16 ether_type = 0;
		uint32 evt_type;

		ACSD_INFO("recved event from eventfd\n");

		d_info->stats.num_events++;

		if ((bytes = recv(d_info->event_fd, pkt, len, 0)) <= 0)
			return;

		ACSD_INFO("recved %d bytes from eventfd, ifname: %s\n",
				bytes, ifname);
		bytes -= IFNAMSIZ;

		if ((ether_type = ntohs(eth_hdr->ether_type) != ETHER_TYPE_BRCM)) {
			ACSD_INFO("recved ether type %x\n", ether_type);
			return;
		}

		if ((err = acsd_validate_wlpvt_message(bytes, (uint8 *)eth_hdr)))
			return;

		pvt_data = (bcm_event_t *)(ifname + IFNAMSIZ);
		evt_type = ntoh32(pvt_data->event.event_type);
		ACSD_INFO("recved brcm event, event_type: %d\n", evt_type);

		if ((idx = acs_idx_from_map(ifname)) < 0) {
			ACSD_ERROR("cannot find the mapped entry for ifname: %s\n", ifname);
			return;
		}

		c_info = d_info->acs_info->chan_info[idx];

		if (c_info->mode == ACS_MODE_DISABLE && c_info->acs_boot_only) {
			ACSD_INFO("No event handling enabled. Only boot selection \n");
			return;
		}

		if (!AUTOCHANNEL(c_info) && !COEXCHECK(c_info)) {
			ACSD_INFO("Event fail ACSD not in autochannel/coex modes \n");
			return;
		}

		/* If on Fixed chspec and Obss Coex mode, allow only escan events */
		if (COEXCHECK(c_info) &&
			(evt_type != WLC_E_SCAN_COMPLETE && evt_type != WLC_E_ESCAN_RESULT)) {
			ACSD_INFO("In coex mode, discard events other than escan %d\n", evt_type);
			return;
		}

		if (c_info->acs_bgdfs != NULL && c_info->acs_bgdfs->state != BGDFS_STATE_IDLE) {
			return;
		}

		/* Wi-Fi Blanket Repeater needs to run in Fixed Chanspec mode, to implement this
		 * behaviour, WBD needs ACSD not to change channel, if not told to do so through CLI
		 */
		if (FIXCHSPEC(c_info) && (evt_type == WLC_E_TXFAIL_THRESH)) {
			ACSD_INFO("ACSD mode is FIXCHSPEC, avoid channel switch\n");
			return;
		}

		d_info->stats.valid_events++;

		switch (evt_type) {
			case WLC_E_DCS_REQUEST:
				{
					dot11_action_wifi_vendor_specific_t * actfrm;
					actfrm = (dot11_action_wifi_vendor_specific_t *)(pvt_data + 1);

#ifdef DSLCPE_ENDIAN
					acs_chaninfo_t *c_info;
					int index = 0;

					if ((index = acs_idx_from_map(ifname)) < 0) {
						ACSD_ERROR("cannot find the mapped entry for ifname: %s\n", ifname);
						break;
					}
					c_info = d_info->acs_info->chan_info[index];
#endif
					if ((err = dcs_parse_actframe(actfrm, &dcs_data))) {
						ACSD_ERROR("err from dcs_parse_request: %d\n", err);
						break;
					}

					if ((err = dcs_handle_request(ifname, &dcs_data, DOT11_CSA_MODE_ADVISORY,
#ifdef DSLCPE_ENDIAN
						DCS_CSA_COUNT, CSA_BROADCAST_ACTION_FRAME, c_info)))
#else
						DCS_CSA_COUNT, CSA_BROADCAST_ACTION_FRAME)))
#endif
						ACSD_ERROR("err from dcs_handle_request: %d\n", err);

					break;
				}
			case WLC_E_SCAN_COMPLETE:
				{
					ACSD_INFO("recved brcm event: scan complete\n");
					break;
				}
			case WLC_E_PKTDELAY_IND:
				{
					txdelay_event_t pktdelay;

					memcpy(&pktdelay, (txdelay_event_t *)(pvt_data + 1),
							sizeof(txdelay_event_t));
					/* stay in current channel more than acs_chan_dwell_time */
					chan_least_dwell = chanim_record_chan_dwell(c_info, c_info->chanim_info);

					if (chan_least_dwell &&
							(pktdelay.chanim_stats.chan_idle <
							 c_info->acs_fcs.acs_fcs_chanim_stats)) {

						c_info->switch_reason = APCS_TXDLY;
						acs_select_chspec(c_info);
						dcs_data.reason = 0;
						dcs_data.chspec = c_info->selected_chspec;

						if ((err = dcs_handle_request(ifname, &dcs_data,
										DOT11_CSA_MODE_ADVISORY, FCS_CSA_COUNT,
#ifdef DSLCPE_ENDIAN
										c_info->acs_fcs.acs_dcs_csa,c_info)))
#else
										c_info->acs_fcs.acs_dcs_csa)))
#endif
							ACSD_ERROR("err from dcs_handle_request: %d\n", err);
						else
							chanim_upd_acs_record(c_info->chanim_info,
									c_info->selected_chspec, APCS_TXDLY);
					}
					break;
				}
			case WLC_E_TXFAIL_THRESH:
				{
					wl_intfer_event_t *event;
					unsigned char *addr;

					if (!ACS_FCS_MODE(c_info)) {
						ACSD_FCS("Cannot handle event ACSD not in FCS mode \n");
						break;
					}

					/* ensure we have the latest channel information and dwell time etc */
					acs_update_status(c_info);

					event = (wl_intfer_event_t *)(pvt_data + 1);
					chan_least_dwell = chanim_record_chan_dwell(c_info, c_info->chanim_info);
					addr = (unsigned char *)(&(pvt_data->event.addr));

					ACSD_FCS("Intfer:%s Mac:%02x:%02x:%02x:%02x:%02x:%02x status=0x%x\n",
							ifname, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5],
							event->status);

					for (idx = 0; idx < WLINTFER_STATS_NSMPLS; idx++) {
						ACSD_FCS("0x%x\t", event->txfail_histo[idx]);
					}

					ACSD_FCS("\n time:%u", (uint32)time(NULL));

					if (!chan_least_dwell) {
						ACSD_FCS("chan_least_dwell is FALSE\n");
						break;
					}

					if (acsd_trigger_dfsr_check(c_info)) {
						ACSD_DFSR("trigger DFS reentry...\n");
						acs_dfsr_set(ACS_DFSR_CTX(c_info),
								c_info->cur_chspec, __FUNCTION__);
						break;
					}

					if (!acsd_need_chan_switch(c_info)) {
						ACSD_FCS("No channel switch...\n");
						break;
					}

					if (acsd_hi_chan_check(c_info)) {
						acs_get_best_dfs_forced_chspec(c_info);
						c_info->selected_chspec = acs_adjust_ctrl_chan(c_info,
							c_info->dfs_forced_chspec);
						ACSD_FCS("Select 0x%x\n", c_info->selected_chspec);
					} else {
						c_info->switch_reason = APCS_TXFAIL;
						acs_select_chspec(c_info);
					}

					if (!acs_bgdfs_attempt_on_txfail(c_info)) {
						dcs_data.reason = 0;
						dcs_data.chspec = c_info->selected_chspec;

						if (c_info->selected_chspec == c_info->cur_chspec) {
							ACSD_INFO("Seleted = cur 0x%x avoid CSA\n",
								c_info->cur_chspec);
							break;
						}

						ACSD_INFO("%s Performing CSA on chspec 0x%x\n", c_info->name,
								dcs_data.chspec);
						if ((err = dcs_handle_request(ifname, &dcs_data,
										DOT11_CSA_MODE_ADVISORY, FCS_CSA_COUNT,
#ifdef DSLCPE_ENDIAN
      				        			c_info->acs_fcs.acs_dcs_csa, c_info))) {
#else
										c_info->acs_fcs.acs_dcs_csa))) {
#endif
							ACSD_ERROR("err from dcs_handle_request: %d\n", err);
						} else {
							c_info->recent_prev_chspec = c_info->cur_chspec;
							c_info->acs_prev_chan_at = time(NULL);
							acs_intfer_config(c_info);
							chanim_upd_acs_record(c_info->chanim_info,
									c_info->selected_chspec, APCS_TXFAIL);
						}
					}
					break;
				}
			case WLC_E_ESCAN_RESULT:
				{
					if (!c_info->acs_escan->acs_escan_inprogress ||
							!c_info->acs_escan->acs_use_escan) {
						ACSD_INFO("ACSD Escan not triggered from ACSD\n");
						return;
					}

					escan_event_status = ntoh32(pvt_data->event.status);
					escan_data = (wl_escan_result_t*)(pvt_data + 1);

					if (escan_event_status == WLC_E_STATUS_PARTIAL) {
						wl_bss_info_t *bi = &escan_data->bss_info[0];
						wl_bss_info_t *bss;
#ifdef DSLCPE
						endian_convert_bss_info(bi,c_info);
#endif
						/* check if we've received info of same BSSID */
						for (result = c_info->acs_escan->escan_bss_head; result;
								result = result->next) {
							bss = result->bss;

							if (!memcmp(bi->BSSID.octet, bss->BSSID.octet,
									ETHER_ADDR_LEN) && CHSPEC_BAND(bi->chanspec) ==
									CHSPEC_BAND(bss->chanspec) && bi->SSID_len ==
									bss->SSID_len && !memcmp(bi->SSID, bss->SSID,
									bi->SSID_len)) {
								break;
							}
						}

						if (!result) {
							/* New BSS. Allocate memory and save it */
							struct escan_bss *ebss = (struct escan_bss *)acsd_malloc(
									OFFSETOF(struct escan_bss, bss) + bi->length);

							if (!ebss) {
								ACSD_ERROR("can't allocate memory for escan bss");
								break;
							}

							ebss->next = NULL;
							memcpy(&ebss->bss, bi, bi->length);
							if (c_info->acs_escan->escan_bss_tail) {
								c_info->acs_escan->escan_bss_tail->next = ebss;
							} else {
								c_info->acs_escan->escan_bss_head = ebss;
							}

							c_info->acs_escan->escan_bss_tail = ebss;
						} else if (bi->RSSI != WLC_RSSI_INVALID) {
							/* We've got this BSS. Update RSSI if necessary */
							bool preserve_maxrssi = FALSE;
							if (((bss->flags & WL_BSS_FLAGS_RSSI_ONCHANNEL) ==
									(bi->flags & WL_BSS_FLAGS_RSSI_ONCHANNEL)) &&
									((bss->RSSI == WLC_RSSI_INVALID) || (bss->RSSI <
									bi->RSSI))) {
								/* Preserve max RSSI if the measurements are both
								 * on-channel or both off-channel */
								preserve_maxrssi = TRUE;
							} else if ((bi->flags & WL_BSS_FLAGS_RSSI_ONCHANNEL) &&
									(bss->flags & WL_BSS_FLAGS_RSSI_ONCHANNEL) == 0) {
								/* Preserve the on-channel RSSI measurement if the
								 * new measurement is off channel */
								preserve_maxrssi = TRUE;
								bss->flags |= WL_BSS_FLAGS_RSSI_ONCHANNEL;
							}

							if (preserve_maxrssi) {
								bss->RSSI = bi->RSSI;
								bss->SNR = bi->SNR;
								bss->phy_noise = bi->phy_noise;
							}
						}
					} else if (escan_event_status == WLC_E_STATUS_SUCCESS) {
						/* Escan finished. Lets dump results */
						if (ACS_FCS_MODE(c_info)) {
							c_info->acs_fcs.timestamp_acs_scan = time(NULL);
							if (c_info->acs_escan->scan_type == ACS_SCAN_TYPE_CS) {
								c_info->acs_fcs.timestamp_tx_idle =
									c_info->acs_fcs.timestamp_acs_scan;
							}
						}
#ifdef ACS_DEBUG
						/* print scan results */
						for (result = c_info->acs_escan->escan_bss_head; result;
								result = result->next) {
							dump_bss_info(result->bss);
						}
#endif // endif
						c_info->acs_escan->acs_escan_inprogress = FALSE;
						ACSD_INFO("Escan success!\n");
					} else {
						ACSD_ERROR("sync_id: %d, status:%d, misc. error/abort\n",
								escan_data->sync_id, status);

						acs_escan_free(c_info->acs_escan->escan_bss_head);
						c_info->acs_escan->escan_bss_head = NULL;
						c_info->acs_escan->escan_bss_tail = NULL;
						c_info->acs_escan->acs_escan_inprogress = FALSE;
					}
					break;
				}
			case WLC_E_MODE_SWITCH:
				{
					wl_event_mode_switch_t *ev_ms =
						(wl_event_mode_switch_t *) (pvt_data + 1);
					wl_event_mode_switch_dyn160 *data_dyn160;
#ifdef DSLCPE_ENDIAN
					ev_ms->version=dtoh16(ev_ms->version);
					ev_ms->length=dtoh16(ev_ms->length);
					ev_ms->reason=dtoh16(ev_ms->reason);
					ev_ms->data_offset=dtoh16(ev_ms->data_offset);
					ev_ms->opmode_from=dtoh16(ev_ms->opmode_from);
					ev_ms->opmode_to=dtoh16(ev_ms->opmode_to);
					ev_ms->flags=dtoh32(ev_ms->flags);
#endif

					if (ev_ms->version != WL_EVENT_MODESW_VER_1) {
						ACSD_ERROR("%s: Unsupported modesw event ver %d",
							c_info->name, ev_ms->version);
						break;
					}
					if (ev_ms->reason != WL_E_MODESW_REASON_DYN160) {
						break;
					}
					if (bytes < ev_ms->data_offset + sizeof(*data_dyn160)) {
						ACSD_ERROR("%s: invalid event data offset %d",
							c_info->name, ev_ms->data_offset);
						break;
					}
					data_dyn160 = (wl_event_mode_switch_dyn160 *)
						((uint8 *)ev_ms) + ev_ms->data_offset;

#ifdef DSLCPE_ENDIAN
					data_dyn160->trigger=dtoh16(data_dyn160->trigger);
					data_dyn160->metric_160_80=dtoh16(data_dyn160->metric_160_80);
#endif
					if (data_dyn160->trigger == 0) {
						c_info->is_mu_active = FALSE;
					} else if (data_dyn160->trigger == 1) {
						c_info->is_mu_active = TRUE;
					}
					break;
				}
			default:
				ACSD_INFO("recved event type %x\n", evt_type);
				break;
		}
	}
}

/*
 * acs_upgrade_downgrade_opermode - upgrade/downgrade by oper_mode
 *
 * c_info - pointer to acs_chaninfo_t for an interface
 *
 * Return void.
 */
static void
acs_upgrade_downgrade_opermode(acs_chaninfo_t * c_info)
{
	acs_rsi_t *rsi = &c_info->rs_info;

	if (!WL_BW_CAP_160MHZ(rsi->bw_cap)) {
		return;
	}

	if (ACS_CHINFO_IS_UNCLEAR(acs_channel_info(c_info, c_info->cur_chspec))) {
		ACSD_DEBUG("%s Chanspec 0x%x NOT cleared or CAC in Progress\n",
			c_info->name, c_info->cur_chspec);
		return;
	}

	/* Upgrade to 160Mhz by Oper_mode */
	if (ACS_11H(c_info) && c_info->is160_upgradable &&
		c_info->dyn160_enabled && ACS_OP_BW_IS_80(c_info->oper_mode)) {
		chanspec_t upgrade_chspc;

		upgrade_chspc = wf_channel2chspec(wf_chspec_ctlchan(c_info->cur_chspec),
			WL_CHANSPEC_BW_160);

		if (CHSPEC_IS160(upgrade_chspc) && !wf_chspec_malformed(upgrade_chspc) &&
			ACS_CHINFO_IS_CLEARED(acs_channel_info(c_info, upgrade_chspc))) {
			ACSD_INFO("%s Upgrading to 160 Mhz chanspec 0x%x by oper_mode \n",
				c_info->name, upgrade_chspc);
			/* Upgrade to 160Mhz done in two steps, 0x112 and then to 0x116 */
			if (c_info->oper_mode == ACS_OP_4NSS_80) {
				if (acs_set_oper_mode(c_info, ACS_OP_2NSS_80) != BCME_OK) {
					ACSD_ERROR("%s Upgrade to 160 Mhz Step 1 Failed\n",
						c_info->name);
				}
			} else if (c_info->oper_mode == ACS_OP_2NSS_80) {
				if (acs_set_oper_mode(c_info, ACS_OP_2NSS_160) != BCME_OK) {
					ACSD_ERROR("%s Upgrade to 160 Mhz Step 2 Failed\n",
						c_info->name);
				}
			}
		}
	}

	/* Downgrade to 80Mhz by Oper_mode */
	if (ACS_11H(c_info) && c_info->is160_downgradable &&
		ACS_OP_BW_IS_160_80p80(c_info->oper_mode)) {
		ACSD_INFO("%s Downgrade to 80 Mhz by oper_mode \n", c_info->name);
		/* from 160MHz to 80MHz */
		if (acs_set_oper_mode(c_info, ACS_OP_2NSS_80) != BCME_OK) {
			ACSD_ERROR("%s Downgrade to 80Mhz Failed\n", c_info->name);
		}
	}
}

static int
acsd_init(void)
{
	int err = ACSD_OK;
	uint  port = ACSD_DFLT_CLI_PORT;

	d_info = (acsd_wksp_t*)acsd_malloc(sizeof(acsd_wksp_t));

	d_info->version = ACSD_VERSION;
	d_info->fdmax = ACSD_DFLT_FD;
	d_info->event_fd = ACSD_DFLT_FD;
	d_info->listen_fd = ACSD_DFLT_FD;
	d_info->conn_fd = ACSD_DFLT_FD;
	d_info->poll_interval = ACSD_DFLT_POLL_INTERVAL;
	d_info->ticks = 0;
	d_info->cmd_buf = NULL;

	err = acsd_open_eventfd();

	if (err == ACSD_OK)
		err = acsd_svr_socket_init(port);

	return err;
}

static void
acsd_cleanup(void)
{
	if (d_info) {
		if (d_info->acs_info)
			acs_cleanup(&d_info->acs_info);
		ACS_FREE(d_info->cmd_buf);
		free(d_info);
	}
}

static void
acsd_watchdog(uint ticks)
{
	int i, ret;
	acs_chaninfo_t* c_info;

	for (i = 0; i < ACS_MAX_IF_NUM; i++) {
		c_info = d_info->acs_info->chan_info[i];

		if ((!c_info) || (c_info->mode == ACS_MODE_DISABLE)) {
			continue;
		}

		if (ticks % ACS_TICK_DISPLAY_INTERVAL == 0) {
			ACSD_INFO("tick:%u\n", ticks);
		}

		if (ticks % ACS_ASSOCLIST_POLL == 0)  {
			acs_update_assoc_info(c_info);
			if (c_info->dyn160_cap) {
				acs_update_dyn160_status(c_info);
			}
		}

		/* BGDFS is not enabled/triggered if 160Mhz BW capable */
		if (ACS_11H_AND_BGDFS(c_info) && !c_info->is160_bwcap &&
				c_info->acs_bgdfs->state != BGDFS_STATE_IDLE) {
			acs_bgdfs_info_t * bgdfs = c_info->acs_bgdfs;
			time_t now = time(NULL);
			bool bgdfs_scan_done = FALSE;
			if ((ticks % ACS_BGDFS_SCAN_STATUS_CHECK_INTERVAL) == 0) {
				if ((ret = acs_bgdfs_get(c_info)) != BGDFS_CAP_TYPE0) {
					ACSD_ERROR("acs bgdfs get failed with %d\n", ret);
				}
				if (bgdfs->status.move_status != DFS_SCAN_S_INPROGESS) {
					bgdfs_scan_done = TRUE;
				}
			}
			if (bgdfs_scan_done || bgdfs->timeout < now) {
				bgdfs->state = BGDFS_STATE_IDLE;
				if (!bgdfs_scan_done &&
						(ret = acs_bgdfs_get(c_info)) != BGDFS_CAP_TYPE0) {
					ACSD_ERROR("acs bgdfs get failed with %d\n", ret);
				}
				if (bgdfs->fallback_blocking_cac &&
						bgdfs->acs_bgdfs_on_txfail &&
						((ret = acs_bgdfs_check_status(c_info, TRUE))
						 != BCME_OK)) {
					wl_bcmdcs_data_t dcs_data;
					int err;
					ACSD_INFO("%s####BGDFS Failed. Do Full MIMO CAC#####\n",
							c_info->name);

					dcs_data.reason = 0;
					dcs_data.chspec = c_info->selected_chspec;
					if ((err = dcs_handle_request(c_info->name,
							&dcs_data,
							DOT11_CSA_MODE_ADVISORY,
							FCS_CSA_COUNT,
#ifdef DSLCPE_ENDIAN
      				        c_info->acs_fcs.acs_dcs_csa, c_info))) {
#else
							c_info->acs_fcs.acs_dcs_csa))) {
#endif
						ACSD_ERROR("%s Error dcs_handle_request: %d\n",
								c_info->name, err);
					}
					bgdfs->acs_bgdfs_on_txfail = FALSE;

				} else if (bgdfs->next_scan_chan != 0) {
					if ((ret = acs_bgdfs_check_status(c_info, FALSE))
							== BCME_OK) {
						ACSD_INFO("acs bgdfs ch 0x%x is radar free\n",
								bgdfs->next_scan_chan);
					} else if (bgdfs->next_scan_chan != 0) {
						ACSD_INFO("acs bgdfs chan 0x%x is not radar free "
								"(err: %d)\n",
								bgdfs->next_scan_chan, ret);
					}
					/* reset for next try; let it recompute channel */
					bgdfs->next_scan_chan = 0;
				}
			} else {
				continue;
			}
		}

		if (ACS_FCS_MODE(c_info)) { /* Update channel idle times */

			if (ACS_11H_AND_BGDFS(c_info) &&
				(ticks % ACS_TRAFFIC_INFO_UPDATE_INTERVAL((c_info)->acs_bgdfs)) == 0) {

				if ((ret = acs_activity_update(c_info)) != BCME_OK) {
					ACSD_ERROR("activity update failed");
				}

				(void) acs_bgdfs_idle_check(c_info);

				/* BGDFS is not enabled/triggered if 160Mhz BW capable */
				if (ACS_11H_AND_BGDFS(c_info) && !c_info->is160_bwcap &&
						c_info->acs_bgdfs->idle &&
						c_info->acs_bgdfs->state == BGDFS_STATE_IDLE) {
					if (c_info->acs_bgdfs->ahead &&
							acs_bgdfs_ahead_trigger_scan(c_info) !=
							BCME_OK) {
						ACSD_ERROR("BGDFS ahead trigger scan "
								"failed\n");
					}
				}
				/* Upgrade to 160Mhz by full MIMO CAC */
				if (ACS_11H_AND_BGDFS(c_info) && c_info->is160_upgradable &&
						!c_info->dyn160_enabled &&
						c_info->acs_bgdfs->idle) {
					if (acs_upgrade_to160(c_info) == BCME_OK) {
						wl_bcmdcs_data_t dcs_data;
						int err;
						ACSD_INFO("%s acs_upgrade_to160 picked 0x%x\n",
								c_info->name,
								c_info->selected_chspec);

						dcs_data.reason = 0;
						dcs_data.chspec = c_info->selected_chspec;
						if ((err = dcs_handle_request(c_info->name,
								&dcs_data,
								DOT11_CSA_MODE_ADVISORY,
								FCS_CSA_COUNT,
#ifdef DSLCPE_ENDIAN
								c_info->acs_fcs.acs_dcs_csa, c_info))) {
#else								
								c_info->acs_fcs.acs_dcs_csa))) {
#endif
							ACSD_ERROR("%sErr dcs_handle_request:%d\n",
									c_info->name, err);
						}
					} else {
						ACSD_ERROR("%s acs_upgrade_to160 Failed\n",
								c_info->name);
					}
				}
			}

			if (ticks % ACS_ASSOCLIST_POLL == 0)  {
				acs_upgrade_downgrade_opermode(c_info);
			}
#ifdef DSLCPE_ENDIAN
		  	acs_dfsr_activity_update(ACS_DFSR_CTX(c_info), c_info->name, c_info);
#else
			acs_dfsr_activity_update(ACS_DFSR_CTX(c_info), c_info->name);
#endif
		}

		if (ticks % ACS_STATUS_POLL == 0)
			acs_update_status(c_info);

		acsd_chanim_check(ticks, c_info);

		acs_scan_timer_or_dfsr_check(c_info); /* AUTOCHANNEL/DFSR is checked in called fn */

		if (AUTOCHANNEL(c_info) &&
			(acs_fcs_ci_scan_check(c_info) || CI_SCAN(c_info))) {
			acs_do_ci_update(ticks, c_info);
		}

	}
}

/* service main entry */
int
main(int argc, char *argv[])
{
	int err = ACSD_OK;
	struct timeval tv;
	char *val;
	int daemonize = 1;

	val = nvram_safe_get("acsd_debug_level");
	if (strcmp(val, ""))
		acsd_debug_level = strtoul(val, NULL, 0);

	ACSD_INFO("acsd start...\n");

	if (argc > 1) {
	    if (strcmp(argv[1], "-F") == 0) {
		daemonize = 0;
	    } else {
		ACSD_ERROR("Unknown argument\n");
		goto cleanup;
	    }
	}

	val = nvram_safe_get("acs_ifnames");
	if (!strcmp(val, "")) {
		ACSD_ERROR("No interface specified, exiting...");
		return err;
	}

	if ((err = acsd_init()))
		goto cleanup;

	acs_init_run(&d_info->acs_info);

#if !defined(DEBUG)
	if (daemonize) {
		if (daemon(1, 1) == -1) {
			ACSD_ERROR("err from daemonize.\n");
			goto cleanup;
		}
	}
#endif // endif
	tv.tv_sec = d_info->poll_interval;
	tv.tv_usec = 0;

	while (1) {
		/* Don't change channel when WPS is in the processing,
		 * to avoid WPS fails
		 */
		if (ACS_WPS_RUNNING) {
			sleep_ms(1000);
			continue;
		}

		if (tv.tv_sec == 0 && tv.tv_usec == 0) {
			d_info->ticks ++;
			tv.tv_sec = d_info->poll_interval;
			tv.tv_usec = 0;
			ACSD_DEBUG("ticks: %d\n", d_info->ticks);
			acsd_watchdog(d_info->ticks);
		}
		acsd_main_loop(&tv);
	}
cleanup:
	acsd_close_eventfd();
	acsd_close_listenfd();
	acsd_cleanup();
	return err;
}
