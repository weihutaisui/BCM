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

#ifdef SUPPORT_STUN

/** Includes. **/

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h> /* ntohs */
#include "stun_packet.h"

/** Defines. **/

#define STUN_MIN_MESSAGE_SIZE 20
#define STUN_TID_SIZE 16

/** Typedefs. **/


/** Prototypes. **/


/** Globals. **/


void stun_pkt_set_field(UINT8* data, UINT16 value)
{
  data[0] = (value & 0xff00) >> 8;
  data[1] = value & 0xff;
}

stun_pkt* stun_pkt_new(UINT16 type)
{
  stun_pkt* pkt = (stun_pkt*) calloc (1,sizeof(stun_pkt));
  if (!pkt)
  {
    return 0;
  }

  pkt->data = (UINT8*) calloc(STUN_MIN_MESSAGE_SIZE, sizeof(UINT8));
  if (!pkt->data)
  {
    return 0;
  }
  pkt->len = STUN_MIN_MESSAGE_SIZE;
  pkt->attr_len = 0;

  /* attribute */
  stun_pkt_set_field(&pkt->data[0], type);
  /* length */
  stun_pkt_set_field(&pkt->data[2], 0);
  /* Transaction ID */
  UINT8 i;
  for (i = 0; i < STUN_TID_SIZE; i++)
  {
    pkt->data[4+i] = rand() % 256;
  }

  return pkt;
}

int stun_pkt_add_str_attr(stun_pkt* pkt, UINT16 attr, char* value)
{
  return(stun_pkt_add_byte_attr(pkt, attr, (UINT8*)value, strlen(value)));
}

int stun_pkt_add_byte_attr(stun_pkt* pkt, UINT16 attr, UINT8* value, UINT16 len)
{
  /* length MUST be a multiple of 4 (measured in bytes) in order to guarantee 
   * alignment of attributes on word boundaries. */
  UINT16 pad = 0;
  UINT8 mod = len % 4;
  if (mod)
  {
    pad = 4 - mod;
  }

  /* attr original len + paddig */
  UINT16 attr_len = len + pad;

  /* attr type + attr length = 4 then + attr len */
  UINT16 data_to_add_len = 4 + attr_len;   

  UINT16 new_len = data_to_add_len + pkt->len;
  UINT8* new_data = (UINT8*) calloc(new_len, sizeof(UINT8));
  if (!new_data)
  {
    return 0;
  }

  memcpy(new_data, pkt->data, pkt->len);
  free(pkt->data);
  pkt->data = 0;

  pkt->attr_len += data_to_add_len;
  stun_pkt_set_field(&new_data[2], pkt->attr_len);

  UINT8* attr_data = &new_data[pkt->len];
  stun_pkt_set_field(&attr_data[0], attr);
  stun_pkt_set_field(&attr_data[2], attr_len);
  memcpy(&attr_data[4], value, attr_len-pad);

  pkt->data = new_data;
  pkt->len = new_len;

  return 1;
}

int stun_pkt_add_binding_change(stun_pkt* pkt)
{
  /*This attribute contains no value. Its Length element MUST be equal to zero */
  UINT16 data_to_add_len = 4;

  UINT16 new_len = data_to_add_len + pkt->len;
  UINT8* new_data = (UINT8*) calloc(new_len, sizeof(UINT8));
  if (!new_data)
  {
    return 0;
  }

  memcpy(new_data, pkt->data, pkt->len);
  free(pkt->data);
  pkt->data = 0;

  pkt->attr_len += data_to_add_len;
  stun_pkt_set_field(&new_data[2], pkt->attr_len);

  UINT8* attr_data = &new_data[pkt->len];
  stun_pkt_set_field(&attr_data[0], STUN_AT_BINCHG);
  stun_pkt_set_field(&attr_data[2], 0x0); 

  pkt->data = new_data;
  pkt->len = new_len;

  return 1;
}

void stun_pkt_delete(stun_pkt* pkt)
{
  if (pkt)
  {
    if (pkt->data)
    {
      free(pkt->data);
      pkt->data = 0;
    }
    free(pkt);
    pkt = 0;
  }
}

stun_pkt_resp* stun_pkt_parse_resp(UINT8* response, int size)
{
  short type, len;
  if (size < STUN_MIN_MESSAGE_SIZE)
  {
    return 0;		/* Packet too short */
  }

  memcpy(&type, response, 2);
  type = ntohs(type);
  if (type == STUN_MT_BINREQ)
  {
    return 0;   
  }

  memcpy(&len, response + 2, 2);
  len = ntohs(len);
  if (len != (size - STUN_MIN_MESSAGE_SIZE))
  {
    return 0;		/* Packet too short */
  }

  stun_pkt_resp* pkt_resp = (stun_pkt_resp *) malloc(sizeof(stun_pkt_resp));
  if (!pkt_resp)
  {
    return 0;
  }
  
  UINT8 *a = response + 20;
  int i=0;
  while (i < len)
  {
    UINT16 t, l;

    memcpy(&t, a, 2);
    t = ntohs(t);

    memcpy(&l, a + 2, 2);
    l = ntohs(l);
    
    UINT8 *buf = a + 4;

    if (t == STUN_AT_MAPADD && l == 8)
    {      
      pkt_resp->errcode = 0;
      pkt_resp->family = *(buf + 1);
      pkt_resp->port = (*(buf + 2) << 8) + *(buf + 3);
      pkt_resp->address = (*(buf + 4) << 24) + (*(buf + 5) << 16) + (*(buf + 6) << 8) + *(buf + 7);            
      return pkt_resp;          
    }
    else if (t == STUN_AT_ERRCOD && l == 4)
    {
      pkt_resp->errcode = (*(buf) << 24) + (*(buf + 1) << 16) + (*(buf + 2) << 8) + *(buf + 3);        
      return pkt_resp;
    }

    a += 4+l;
    i += 4+l;
  }
  
  free(pkt_resp);
  return 0;
}
 
#endif // SUPPORT_STUN
