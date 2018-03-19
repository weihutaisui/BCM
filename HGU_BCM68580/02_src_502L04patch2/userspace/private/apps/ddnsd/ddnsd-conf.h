/***********************************************************************
 *
 *  Copyright (c) 2004-2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

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

#ifndef _DDNSD_CONF_H_
#define _DDNSD_CONF_H_

#define MAX_INTERFACES 16

// Defines for DDnsConfig

#define HOSTNAME 0
#define SERVICE  1
#define IFACE    2
#define USERNAME 3
#define PASSWORD 4

// Defines for DDnsCache
// #define HOSTNAME 0  Shared with DDnsConfig
#define CACHE_IP 1

#if 0
typedef struct ddns { char *vals[5];
                      struct ddns *next;
                    } BcmDDnsConfig;

typedef BcmDDnsConfig * pBcmDDnsConfig;

typedef struct ddns_cache { char *hostname;
                            char *address;
                            struct ddns_cache *next; } BcmDDnsCache;

typedef BcmDDnsCache * pBcmDDnsCache;
#endif

// Prototypes for DDnsConfig file functions

int    BcmDDnsConfig_init( char *filename );
char  *BcmDDnsConfig_get( char *iface, int var );
void   BcmDDnsConfig_iter( void (*func)(char *) );
char  *BcmDDnsConfig_getInterface( char *hostname );
char  *BcmDDnsConfig_getService( char *hostname );
char  *BcmDDnsConfig_getUsername( char *hostname );
char  *BcmDDnsConfig_getPassword( char *hostname );
void   BcmDDnsConfig_close(void);


int   BcmDDnsCache_init( char *filename );
char *BcmDDnsCache_get( char *hostname );
void  BcmDDnsCache_close(void);

#endif
