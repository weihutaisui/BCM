/*	$OpenBSD: cmac.h,v 1.1 2008/08/12 15:43:00 damien Exp $	*/

/*-
 * Copyright (c) 2008 Damien Bergamini <damien.bergamini@free.fr>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _CMAC_H_
#define _CMAC_H_

#include "aes.h"

#define AES_CMAC_KEY_LENGTH	16
#define AES_CMAC_DIGEST_LENGTH	16

typedef struct _AES_CMAC_CTX {
	AES_KEY	        key;
	uint8_t	        X[16];
	uint8_t	        M_last[16];
	uint32_t	M_n;
} AES_CMAC_CTX;


void AES_CMAC_Init(AES_CMAC_CTX *ctx) ;
void AES_CMAC_SetKey(AES_CMAC_CTX *ctx, const uint8_t key[AES_CMAC_KEY_LENGTH]) ;

void AES_CMAC_Update(AES_CMAC_CTX *ctx, const uint8_t *data, uint32_t len) ;

void AES_CMAC_Final(uint8_t digest[AES_CMAC_DIGEST_LENGTH], AES_CMAC_CTX *ctx) ;

void AES_CMAC (const uint8_t * key , const uint8_t * date_in ,  uint8_t * data_out , uint32_t len);

void AES_CMAC_generate_sub_keys(  const uint8_t * key ,uint8_t * k1 , uint8_t * k2 ) ;


#endif /* _CMAC_H_ */
