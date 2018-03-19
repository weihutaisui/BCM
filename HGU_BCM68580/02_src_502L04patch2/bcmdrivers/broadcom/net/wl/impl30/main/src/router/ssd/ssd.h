/*
 * SSD shared include file
 *
 * Copyright (C) 2014, Broadcom Corporation
 * All Rights Reserved.
 *
 * $Id: ssd.h $
 */

#define NVRAM_SSD_ENABLE "ssd_enable"
#define NVRAM_SSD_SSID_TYPE "ssd_type"

#define BCM_EVENT_HEADER_LEN	(sizeof(bcm_event_t))
#define MAX_EVENT_BUFFER_LEN	1400
#define MAX_LINE_LEN			16

/* wait for security check */
#define INTERVAL_ASSOC_CONFIRM 5
#define DISCONNECT_EVENT(e) ((e == WLC_E_DISASSOC_IND) ||\
							(e == WLC_E_DEAUTH) ||\
							(e == WLC_E_DEAUTH_IND))

typedef enum {
	SSD_TYPE_DISABLE = 0,
	SSD_TYPE_PRIVATE = 1,
	SSD_TYPE_PUBLIC = 2
} ssd_type_t;

typedef struct ssd_maclist {
	struct ether_addr addr;
	time_t timestamp;		/* assoc timestamp */
	uint8  ifidx;			/* destination OS i/f index */
	uint8  bsscfgidx;		/* source bsscfg index */
	char   ssid[MAX_SSID_LEN + 1];
	uint8  security;        /* open, WEP or WPA/WPA2 */
	struct ssd_maclist *next;
} ssd_maclist_t;

#define SSD_DEBUG_ERROR		0x000001
#define SSD_DEBUG_WARNING	0x000002
#define SSD_DEBUG_INFO		0x000004

#define SSD_PRINT_ERROR(fmt, arg...) \
		do { if (ssd_msglevel & SSD_DEBUG_ERROR) \
			printf("SSD >>%s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)

#define SSD_PRINT_WARNING(fmt, arg...) \
		do { if (ssd_msglevel & SSD_DEBUG_WARNING) \
			printf("SSD >>%s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)

#define SSD_PRINT_INFO(fmt, arg...) \
		do { if (ssd_msglevel & SSD_DEBUG_INFO) \
			printf("SSD >>%s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)
