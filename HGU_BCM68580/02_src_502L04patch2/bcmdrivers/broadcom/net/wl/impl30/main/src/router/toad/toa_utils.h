/*
 * toa_utils.h - traffic ordering agent shared utility functions header file
 *
 * This is the traffic ordering agent, which takes care of assigning a scheduler
 * and priority level to associated stations.
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
 * $Id: toa_utils.h 462558 2014-03-18 01:10:07Z $
 *
 */
#if !defined(__toa_utils_h__)
#define __toa_utils_h__

typedef struct toa_attribute {
	struct toa_attribute *next;
	const char *name;
	const char *value;
} toa_attribute;

typedef struct toa_record {
	struct toa_record *next;
	char		*key;
	toa_attribute	*attributes;
} toa_record;

typedef toa_record toa_station;
typedef toa_record toa_ssid;

typedef struct {
	toa_station	*station_settings;	/* per-station settings */
	toa_ssid	*bss_settings;		/* per-BSS settings */
	toa_record	*default_settings;	/* default settings */
} toa_settings;

typedef struct {
	int		id;
	const char	*name;
} toa_value_map;

typedef enum {
	ATTR_ERROR = 0,
	ATTR_TYPE,
	ATTR_PRIO,
	ATTR_NAME
} toa_attribute_id;

typedef enum {
	STA_TYPE_ERROR = 0,
	STA_TYPE_VIDEO,
	STA_TYPE_DATA,
	STA_TYPE_PUBLIC
} toa_station_type;

typedef enum {
	STA_PRIO_ERROR   = 0,
	STA_PRIO_HIGHEST = 1,
	STA_PRIO_HIGHER  = 2,
	STA_PRIO_HIGH    = 3,
	STA_PRIO_LOW     = 13,
	STA_PRIO_DEFAULT = STA_PRIO_LOW,
	STA_PRIO_LOWER   = 14,
	STA_PRIO_LOWEST  = 15
} toa_station_priority;

extern const toa_value_map attribute_map[];
extern const toa_value_map sta_type_map[];
extern const toa_value_map sta_prio_map[];

extern toa_settings *toa_load_settings();
extern int toa_save_settings(toa_settings *);
extern void toa_free_settings(toa_settings *);
extern toa_record *toa_match_record(toa_record **list_head, const char *key, int create);

extern int toa_add_attribute(toa_attribute **headp, char *token);
extern void toa_move_attributes(toa_attribute **dst, toa_attribute **src);
extern toa_attribute *toa_get_attribute_by_id(toa_attribute *head, toa_attribute_id aid);
extern void toa_flush_attributes(toa_attribute **headp);

extern int toa_keyword_to_id(const toa_value_map *map, const char *kw);
extern const char *toa_id_to_keyword(const toa_value_map *map, int id);
extern void toa_log_error(const char *fmtstr, ...);

#endif /* __toa__utils_h__ */
