/***********************************************************************
 *
 *  Copyright (c) 2000-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2011:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
************************************************************************/

#include <httpd.h>
#ifdef BRCM_WLAN 
struct cpe_mime_handler cpe_mime_handlers[] = {
#else
struct mime_handler mime_handlers[] = {
#endif
	{ "**.xml", "text/xml", do_file, do_auth, NULL },
	{ "**.htm", "text/html", do_file, do_auth, NULL },
	{ "**.html", "text/html", do_ej, do_auth, NULL },
#ifdef BRCM_WLAN
#ifdef SUPPORT_WLAN_VISUALIZATION

	{ "json.cgi**", "application/json", do_wl_json_get, do_auth, do_wl_json_set },
	{ "wlvis/visdata.db**", NULL, do_wl_vis_dbdownload, do_auth, NULL },
#endif
	{ "wlstatus.cgi**", "text/plain", do_wl_status_get, do_auth, NULL},
#endif
	{ "**.cgi", "text/html", do_cgi, do_auth, NULL },
#ifdef BRCM_WLAN
	{ "**.wl", "text/html", do_wl_cgi, do_auth, NULL },
#ifdef SES
	{ "ezconfig.asp", "text/html", do_ezconfig_asp, NULL, NULL },
#endif
#endif

#ifdef SUPPORT_TOD
	{ "**.tod", "text/html", do_tod_cgi, do_auth, NULL },
#endif
#ifdef UNAUTHENICATED_SELT
	{ "**.selt", "text/html", do_selt_cgi, NULL, NULL },
#endif
	{ "**.tst", "text/html", do_test_cgi, do_auth, NULL },
	{ "**.cmd", "text/html", do_cmd_cgi, do_auth, NULL },
	{ "**.conf", "config/conf", do_cmd_cgi, do_auth, NULL },
	{ "**.txt", "text/plain", do_cmd_cgi, do_auth, NULL },
#ifdef BCMWAPI_WAI
	{ "**.cer", "config/conf", do_cmd_cgi, do_auth, NULL },
#endif /* BCMWAPI_WAI */
	{ "**.js", "text/js", do_file, NULL, NULL },
	{ "**.gif", "image/gif", do_file, do_auth, NULL },
	{ "**.jpg", "image/jpeg", do_file, do_auth, NULL },
	{ "**.jpeg", "image/gif", do_file, do_auth, NULL },
	{ "**.png", "image/png", do_file, do_auth, NULL },
	{ "**.css", "text/css", do_file, NULL, NULL },
	{ "**.au", "audio/basic", do_file, do_auth, NULL },
	{ "**.wav", "audio/wav", do_file, do_auth, NULL },
	{ "**.avi", "video/x-msvideo", do_file, do_auth, NULL },
	{ "**.mov", "video/quicktime", do_file, do_auth, NULL },
	{ "**.mpeg", "video/mpeg", do_file, do_auth, NULL },
	{ "**.vrml", "model/vrml", do_file, do_auth, NULL },
	{ "**.midi", "audio/midi", do_file, do_auth, NULL },
	{ "**.mp3", "audio/mpeg", do_file, do_auth, NULL },
	{ "**.pac", "application/x-ns-proxy-autoconfig", do_file, do_auth, NULL },
	{ NULL, NULL, NULL, NULL, NULL }
};
