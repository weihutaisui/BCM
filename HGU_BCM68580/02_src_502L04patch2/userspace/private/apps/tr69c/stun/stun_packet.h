/***********************************************************************
 * <:copyright-BRCM:2007-2013:proprietary:standard
 * 
 *    Copyright (c) 2007-2013 Broadcom 
 *    All Rights Reserved
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
 * :> *
 * $Change: 109793 $
 ***********************************************************************/

#ifndef _STUN_PACKET_H
#define _STUN_PACKET_H

#include "cms.h"

#define STUN_MT_BINREQ 0x0001
#define STUN_MT_BINRES 0x0101
#define STUN_MT_BINERR 0x0111

#define STUN_AT_MAPADD 0x0001
#define STUN_AT_RESADD 0x0002
#define STUN_AT_CHGREQ 0x0003
#define STUN_AT_SRCADD 0x0004
#define STUN_AT_CHGADD 0x0005
#define STUN_AT_USRNAM 0x0006
#define STUN_AT_PASSWD 0x0007
#define STUN_AT_MSGINT 0x0008
#define STUN_AT_ERRCOD 0x0009
#define STUN_AT_UNKATT 0x000a
#define STUN_AT_REFFRM 0x000b

#define STUN_EC_UNAUTH 0x401

/* Optional STUN attribs */
#define STUN_AT_CONNREQBIN 0xc001
#define STUN_AT_BINCHG     0xc002

typedef struct stun_pkt
{
  UINT8 *data;
  UINT16 len;
  UINT16 attr_len;
} stun_pkt;

typedef struct stun_pkt_resp
{
  UINT32 errcode;
  UINT8 family;
  UINT16 port;
  UINT32 address;
} stun_pkt_resp;

stun_pkt* stun_pkt_new(UINT16 attr);
int stun_pkt_add_str_attr(stun_pkt* pkt, UINT16 attr, char* value);
int stun_pkt_add_byte_attr(stun_pkt* pkt, UINT16 attr, UINT8* value, UINT16 len);
int stun_pkt_add_binding_change(stun_pkt* pkt);
void stun_pkt_delete(stun_pkt* pkt);
stun_pkt_resp* stun_pkt_parse_resp(UINT8* response, int size);
void stun_pkt_set_field(UINT8* data, UINT16 value);

#endif  /* _STUN_PACKET_H */
