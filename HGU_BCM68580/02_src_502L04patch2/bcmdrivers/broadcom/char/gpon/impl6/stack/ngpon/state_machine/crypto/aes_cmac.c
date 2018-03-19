/*	$OpenBSD: cmac.c,v 1.2 2011/01/11 15:42:05 deraadt Exp $	*/

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

/*
 * This code implements the CMAC (Cipher-based Message Authentication)
 * algorithm described in FIPS SP800-38B using the AES-128 cipher.
 */

/*
#include <sys/param.h>
#include <sys/systm.h>
*/

#include "aes_cmac.h"
#ifndef SIM_ENV
#include <linux/string.h>
#else
#include <string.h>
#endif

#define LSHIFT(v, r) do {					\
	int i;							\
	for (i = 0; i < 15; i++)				\
		(r)[i] = (v)[i] << 1 | (v)[i + 1] >> 7;		\
	(r)[15] = (v)[15] << 1;					\
} while (0)

#define XOR(v, r) do {						\
	int i;							\
	for (i = 0; i < 16; i++)				\
		(r)[i] ^= (v)[i];				\
} while (0)

void AES_CMAC_Init(AES_CMAC_CTX *ctx)
{
	memset(ctx->X, 0, sizeof ctx->X);
	ctx->M_n = 0;
}



void AES_CMAC_SetKey( AES_CMAC_CTX *ctx , const uint8_t * key)
{
	AES_set_encrypt_key(key, 128 , &ctx->key);
}


void AES_CMAC_generate_sub_keys(  const uint8_t * key ,uint8_t * k1 , uint8_t * k2 )
{
	uint8_t K[16];
    AES_KEY aes_key ;
	AES_set_encrypt_key(key, 128 , &aes_key);
	/* generate subkey K1 */
	memset(K, 0, sizeof K);
	AES_encrypt( K, K , &aes_key);
	if (K[0] & 0x80)
    {
		LSHIFT(K, K);
		K[15] ^= 0x87;
	}
    else
		LSHIFT(K, K);

    memcpy (k1 , K , 16);

    /* generate subkey K2 */
    if (K[0] & 0x80)
    {
        LSHIFT(K, K);
        K[15] ^= 0x87;
    }
     else
        LSHIFT(K, K);

     memcpy (k2 , K , 16);
}

void AES_CMAC_Update(AES_CMAC_CTX *ctx, const uint8_t *data, uint32_t len)
{
	uint32_t mlen;

	if (ctx->M_n > 0) {
        if (16 - ctx->M_n > len) 
        {
            mlen = len;
        }
        else
        {
            mlen = 16 - ctx->M_n;

        }
		memcpy(ctx->M_last + ctx->M_n, data, mlen);
		ctx->M_n += mlen;
		if (ctx->M_n < 16 || len == mlen)
			return;
		XOR(ctx->M_last, ctx->X);
		AES_encrypt(ctx->X, ctx->X , &ctx->key);
		data += mlen;
		len -= mlen;
	}
	while (len > 16) {	/* not last block */
		XOR(data, ctx->X);
		AES_encrypt( ctx->X, ctx->X , &ctx->key);
		data += 16;
		len -= 16;
	}
	/* potential last block, save it */
	memcpy(ctx->M_last, data, len);
	ctx->M_n = len;
}

void
AES_CMAC_Final(uint8_t * digest , AES_CMAC_CTX *ctx)
{
	uint8_t K[16];

	/* generate subkey K1 */
	memset(K, 0, sizeof K);
	AES_encrypt( K, K , &ctx->key);

	if (K[0] & 0x80) {
		LSHIFT(K, K);
		K[15] ^= 0x87;
	} else
		LSHIFT(K, K);

	if (ctx->M_n == 16) {
		/* last block was a complete block */
		XOR(K, ctx->M_last);
	} else {
		/* generate subkey K2 */
		if (K[0] & 0x80) {
			LSHIFT(K, K);
			K[15] ^= 0x87;
		} else
			LSHIFT(K, K);

		/* padding(M_last) */
		ctx->M_last[ctx->M_n] = 0x80;
		while (++ctx->M_n < 16)
			ctx->M_last[ctx->M_n] = 0;

		XOR(K, ctx->M_last);
	}
	XOR(ctx->M_last, ctx->X);
	AES_encrypt( ctx->X, digest , &ctx->key);

	/*explicit_bzero(K, sizeof K);*/
}

void AES_CMAC (const uint8_t * key , const uint8_t * date_in ,  uint8_t * data_out , uint32_t len)
{
    AES_CMAC_CTX ctx ;
    AES_CMAC_Init(&ctx) ;
    AES_CMAC_SetKey(&ctx ,  key);
    AES_CMAC_Update(&ctx, date_in ,  len);
    AES_CMAC_Final(data_out , &ctx);
}
