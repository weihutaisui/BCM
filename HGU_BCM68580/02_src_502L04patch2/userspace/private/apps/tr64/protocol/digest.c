/*----------------------------------------------------------------------*
 *
 * Copyright (c) 2005-2012 Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2012:proprietary:standard
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
 * :>
 *
 *----------------------------------------------------------------------*/
/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include "md5.h"
#include "upnp.h"
#include "tr64defs.h"
#include "shutils.h"

extern const char* rfc1123_fmt;
extern tr64PersistentData *pTr64Data;

unsigned char bcm_ctype[] = {
	_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,			/* 0-7 */
	_BCM_C,_BCM_C|_BCM_S,_BCM_C|_BCM_S,_BCM_C|_BCM_S,_BCM_C|_BCM_S,_BCM_C|_BCM_S,_BCM_C,_BCM_C,		/* 8-15 */
	_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,			/* 16-23 */
	_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,_BCM_C,			/* 24-31 */
	_BCM_S|_BCM_SP,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,			/* 32-39 */
	_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,			/* 40-47 */
	_BCM_D,_BCM_D,_BCM_D,_BCM_D,_BCM_D,_BCM_D,_BCM_D,_BCM_D,			/* 48-55 */
	_BCM_D,_BCM_D,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,			/* 56-63 */
	_BCM_P,_BCM_U|_BCM_X,_BCM_U|_BCM_X,_BCM_U|_BCM_X,_BCM_U|_BCM_X,_BCM_U|_BCM_X,_BCM_U|_BCM_X,_BCM_U,	/* 64-71 */
	_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,			/* 72-79 */
	_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,			/* 80-87 */
	_BCM_U,_BCM_U,_BCM_U,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,			/* 88-95 */
	_BCM_P,_BCM_L|_BCM_X,_BCM_L|_BCM_X,_BCM_L|_BCM_X,_BCM_L|_BCM_X,_BCM_L|_BCM_X,_BCM_L|_BCM_X,_BCM_L,	/* 96-103 */
	_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,			/* 104-111 */
	_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,			/* 112-119 */
	_BCM_L,_BCM_L,_BCM_L,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_C,			/* 120-127 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 128-143 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 144-159 */
	_BCM_S|_BCM_SP,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,   /* 160-175 */
	_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,_BCM_P,       /* 176-191 */
	_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,       /* 192-207 */
	_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_P,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_U,_BCM_L,       /* 208-223 */
	_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,       /* 224-239 */
	_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_P,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L,_BCM_L        /* 240-255 */
};

/** 
 *   YEN_TO_DO: share code with TR69 in tr69c/main/utils.c
 * size_t b64_encode(const char *inp, size_t insize, char **outptr);
 * static void  resetSessionAuth( sessionAuth *s) 
 * void MD5Init(struct MD5Context *ctx) 
 * void MD5Update(struct MD5Context *ctx, u_char const *buf, u_int len) 
 * void MD5Final(u_char digest[16], struct MD5Context *ctx) 
 * static void  MD5Transform(u_int32_t buf[4], u_int32_t const in[16]) 
 * void tr69_md5it(unsigned char *out, const unsigned char *in)--- tr64_md5it
 * static char *generateWWWAuthenticateHdr(sessionAuth *sa, char *realm, char *domain, char *method ) 
 * table64[] should also be shared -- used to do digest 
 * md5.h can be removed as well 
 **/

/* ---- Base64 Encoding --- */
static const char table64[]=
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static size_t b64_encode(const char *inp, size_t insize, char **outptr)
{
	unsigned char ibuf[3];
	unsigned char obuf[4];
	int i;
	int inputparts;
	char *output;
	char *base64data;

	char *indata = (char *)inp;

	*outptr = NULL;	/* set to NULL in case of failure before we reach the end */

	if (0 == insize)
		insize = strlen(indata);

	base64data = output = (char*)malloc(insize*4/3+4);
	if (NULL == output)
		return 0;

	while (insize > 0) {
		for (i = inputparts = 0; i < 3; i++) {
			if (insize > 0) {
				inputparts++;
				ibuf[i] = *indata;
				indata++;
				insize--;
			} else
				ibuf[i]	= 0;
		}

		obuf [0] = (ibuf [0] & 0xFC) >> 2;
		obuf [1] = ((ibuf [0] & 0x03) << 4) | ((ibuf [1] & 0xF0) >> 4);
		obuf [2] = ((ibuf [1] & 0x0F) << 2) | ((ibuf [2] & 0xC0) >> 6);
		obuf [3] = ibuf [2] & 0x3F;

		switch (inputparts) {
		case 1:	/* only one byte read */
			snprintf(output, 5, "%c%c==",
					 table64[obuf[0]],
					 table64[obuf[1]]);
			break;
		case 2:	/* two bytes read */
			snprintf(output, 5, "%c%c%c=",
					 table64[obuf[0]],
					 table64[obuf[1]],
					 table64[obuf[2]]);
			break;
		default:
			snprintf(output, 5, "%c%c%c%c",
					 table64[obuf[0]],
					 table64[obuf[1]],
					 table64[obuf[2]],
					 table64[obuf[3]] );
			break;
		}
		output += 4;
	}
	*output=0;
	*outptr = base64data; /* make it return the actual data memory */

	return strlen(base64data); /* return the length of the new data */
}
/* ---- End of Base64 Encoding ---- */

static void  resetSessionAuth( sessionAuth *s)
{
	free(s->nc);
	free(s->nonce);
	free(s->orignonce);
	free(s->realm);
	free(s->domain);
	free(s->method);
	free(s->cnonce);
	free(s->opaque);
	free(s->qop);
	free(s->user);
	free(s->uri);
	free(s->algorithm);
	free(s->response);
	free(s->basic);
	memset(s,0,sizeof(struct sessionAuth));
	s->nonceCnt = 1;
}
static void MD5Transform(u_int32_t buf[4], u_int32_t const in[16]);

/*
 * Note: this code is harmless on little-endian machines.
 */
static void
byteReverse(u_char *buf, u_int longs)
{
        u_int32_t t;
        do {
                t = (u_int32_t) ((u_int) buf[3] << 8 | buf[2]) << 16 |
                    ((u_int) buf[1] << 8 | buf[0]);
                *(u_int32_t *) buf = t;
                buf += 4;
        } while (--longs);
}

/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
static void
MD5Init(struct MD5Context *ctx)
{
        ctx->buf[0] = 0x67452301;
        ctx->buf[1] = 0xefcdab89;
        ctx->buf[2] = 0x98badcfe;
        ctx->buf[3] = 0x10325476;

        ctx->bits[0] = 0;
        ctx->bits[1] = 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
static void
MD5Update(struct MD5Context *ctx, u_char const *buf, u_int len)
{
        register u_int32_t t;

        /* Update bitcount */

        t = ctx->bits[0];
        if ((ctx->bits[0] = t + ((u_int32_t) len << 3)) < t)
                ctx->bits[1]++;        /* Carry from low to high */
        ctx->bits[1] += len >> 29;

        t = (t >> 3) & 0x3f;        /* Bytes already in shsInfo->data */

        /* Handle any leading odd-sized chunks */

        if (t) {
                u_char *p = (u_char *) ctx->in + t;

                t = 64 - t;
                if (len < t) {
                        memmove(p, buf, len);
                        return;
                }
                memmove(p, buf, t);
                byteReverse(ctx->in, 16);
                MD5Transform(ctx->buf, (u_int32_t *) ctx->in);
                buf += t;
                len -= t;
        }
        /* Process data in 64-byte chunks */

        while (len >= 64) {
                memmove(ctx->in, buf, 64);
                byteReverse(ctx->in, 16);
                MD5Transform(ctx->buf, (u_int32_t *) ctx->in);
                buf += 64;
                len -= 64;
        }

        /* Handle any remaining bytes of data. */

        memmove(ctx->in, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
static void
MD5Final(u_char digest[16], struct MD5Context *ctx)
{
        u_int count;
        u_char *p;

        /* Compute number of bytes mod 64 */
        count = (ctx->bits[0] >> 3) & 0x3F;

        /* Set the first char of padding to 0x80.  This is safe since there is
           always at least one byte free */
        p = ctx->in + count;
        *p++ = 0x80;

        /* Bytes of padding needed to make 64 bytes */
        count = 64 - 1 - count;

        /* Pad out to 56 mod 64 */
        if (count < 8) {
                /* Two lots of padding:  Pad the first block to 64 bytes */
                memset(p, 0, count);
                byteReverse(ctx->in, 16);
                MD5Transform(ctx->buf, (u_int32_t *) ctx->in);

                /* Now fill the next block with 56 bytes */
                memset(ctx->in, 0, 56);
        } else {
                /* Pad block to 56 bytes */
                memset(p, 0, count - 8);
        }
        byteReverse(ctx->in, 14);

        /* Append length in bits and transform */
        ((u_int32_t *) ctx->in)[14] = ctx->bits[0];
        ((u_int32_t *) ctx->in)[15] = ctx->bits[1];

        MD5Transform(ctx->buf, (u_int32_t *) ctx->in);
        byteReverse((u_char *) ctx->buf, 4);
        memmove(digest, ctx->buf, 16);
        memset(ctx, 0, sizeof (ctx));        /* In case it's sensitive */
}

/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
        ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void
MD5Transform(u_int32_t buf[4], u_int32_t const in[16])
{
        register u_int32_t a, b, c, d;

        a = buf[0];
        b = buf[1];
        c = buf[2];
        d = buf[3];

        MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
        MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
        MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
        MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
        MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
        MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
        MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
        MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
        MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
        MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
        MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
        MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
        MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
        MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
        MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
        MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

        MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
        MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
        MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
        MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
        MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
        MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
        MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
        MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
        MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
        MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
        MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
        MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
        MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
        MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
        MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
        MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

        MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
        MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
        MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
        MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
        MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
        MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
        MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
        MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
        MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
        MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
        MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
        MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
        MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
        MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
        MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
        MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

        MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
        MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
        MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
        MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
        MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
        MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
        MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
        MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
        MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
        MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
        MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
        MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
        MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
        MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
        MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
        MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

        buf[0] += a;
        buf[1] += b;
        buf[2] += c;
        buf[3] += d;
}

void tr64_md5it(unsigned char *out, const unsigned char *in)
{
    MD5Context ctx;
    MD5Init( &ctx );
    MD5Update(&ctx, in, strlen((char*)in));
    MD5Final(out, &ctx);
} /* tr64_md5it */


/*
 * return dynamic memory buffer containing a nonce
 */
static char *makeNonce(void)
{
   struct timeval tv;
   char  buf[100];
   char  *np;

   gettimeofday( &tv, NULL);
   /* start with something odd but hardly random */
   srand(tv.tv_usec*17);
   snprintf(buf, sizeof(buf), "%8x:%8x:%8x",
            (unsigned)tv.tv_usec*rand(), (unsigned)tv.tv_usec*rand(), (unsigned)tv.tv_usec*1551*rand());
   b64_encode(buf, 0, &np);
   return np;
} /* makeNonce */

static char *generateWWWAuthenticateHdr(sessionAuth *sa, char *realm, char *domain, char *method )
{
   char buf[256];

   resetSessionAuth(sa);

   sa->nonce = makeNonce();
   sa->orignonce = strdup(sa->nonce); /* make copy for later test */
   sa->realm = strdup(realm);
   sa->domain = strdup(domain);
   sa->method = strdup(method);
   sa->qopType = eAuth;
   snprintf(buf, sizeof(buf),
            "WWW-Authenticate: Digest realm=\"%s\", domain=\"%s\", nonce=\"%s\", qop=\"auth\","
            " algorithm=MD5",
            sa->realm, sa->domain, sa->nonce);
   return strdup(buf);
} /* generateWWWAuthenticateHdr */


void sendChallenge(struct http_connection *c)
{
   time_t now;
   char date[500];
   char    *h;


   /* Prepare a GMT date string for use in the http header. */
   now = time( (time_t*) 0 );
   (void) strftime( date, sizeof(date), rfc1123_fmt, gmtime( &now ) );
    
   /* Send the HTTP response header */
   uprintf(c->up, "HTTP/1.1 401 Unauthorized\r\n");
   uprintf(c->up, "Content-Length: 0\r\n");
   h = generateWWWAuthenticateHdr( &c->auth, TR64_REALM, TR64_DOMAIN, c->method);
   uprintf(c->up, "%s\r\n\r\n", h);

   uflush(c->up);

   free(h);

} /* sendChallenge */

void sendOK(struct http_connection *c)
{
   uprintf(c->up, "HTTP/1.1 200 OK\r\n");
   uprintf(c->up, "Content-Length: 0\r\n");
   uflush(c->up);
} /* sendOK */


void sendAuthFailed(struct http_connection *c)
{
   uprintf(c->up, "HTTP/1.1 401 Unauthorized\r\n");
   uprintf(c->up, "Content-Length: 0\r\n");

   uflush(c->up);
} /* sendAuthFailed */

/*
 *  Scan for "argname=arval"
 * and return strdup pointer to argval;
 * Return NULL is not found or form error;
 */

static char *getArg(char *p, char *argname, char **argval)
{
   int      nameLth = strlen(argname);
   int      lth;
   char    *s = p;

   *argval = NULL;
   do {
      if ((s=strcasestr(s, argname)) ) {
         if ( isalpha(*(s-1))) {
            s += nameLth;
            continue;
         }
         s += nameLth;
         while (*s && isblank(*s)) ++s;
         if ( *s == '='){
            ++s;
            while (*s && isblank(*s)) ++s;
            if (*s!='\"') {
               /* no quotes around value assume blank delimited or trailing , */
               char *e;
               if ( (e=strchr(s, ',')) || (e=strchr(s,' ')))
                  lth = e-s;
               else /* assume hit \0 at end */
                  lth = strlen(s);
            } else { /* s at opening quote of string enclosed in quotes */
               char *e;
               ++s;
               if ((e=strchr(s,'\"')))
                  lth = e-s;
               else
                  lth = 0;    /* no closing quote-- ignore */
            }
            if (lth)
               *argval = strndup(s,lth);
            return *argval;
         }
      }  else   /* no char sequence found -- return */
         return NULL;

   } while (*s);
   return NULL;
}

static void md5ToAscii( unsigned char *s /*16bytes */, unsigned char *d /*33bytes*/)
{
   int i;

   for (i = 0; i < 16; i++)
      snprintf((char *)&d[i*2],3,"%02x", s[i]);
}
static void generateRequestDigest( sessionAuth *sa, char *user, char* pwd)
{
   char md5inbuf[512];
   unsigned char md5buf[16];
   unsigned char HA1[33];
   unsigned char HA2[33];

   snprintf(md5inbuf, sizeof(md5inbuf), "%s:%s:%s", user, sa->realm, pwd);
   tr64_md5it(md5buf, (unsigned char*)md5inbuf);
   md5ToAscii(md5buf,HA1);
   /*if ( sa->algorithm && strcmp(sa->algorithm, "MD5-sess"))
     snprintf(tmpbuf, sizeof(tmpbuf), "%s:%s:%s", HA1, sa->nonce, cnonceBuf); */
   /* don't know how to do auth-int */
   snprintf(md5inbuf, sizeof(md5inbuf),"%s:%s", sa->method, sa->uri);
   tr64_md5it(md5buf,(unsigned char *)md5inbuf);
   md5ToAscii(md5buf,HA2);

   if (sa->qopType == eNoQop )
      snprintf(md5inbuf, sizeof(md5inbuf), "%s:%s:%s", HA1, sa->nonce, HA2);
   else
      snprintf(md5inbuf, sizeof(md5inbuf), "%s:%s:%08x:%s:%s:%s", HA1, sa->nonce,
               sa->nonceCnt, sa->cnonce, sa->qop, HA2);
   tr64_md5it(md5buf, (unsigned char*)md5inbuf);
   md5ToAscii(md5buf, sa->requestDigest);
}


/*
 * parse Authorization: header and test response against  requestDigest
 * returns : 1 passed authorization
 *         : 0 failed authorization
 */
static int parseAuthorizationHdr(char *ahdr, sessionAuth *sa, char *username, char *password)
{
   char    *p;

   if ( ahdr && (p=strcasestr(ahdr, "Digest"))) {
      free(sa->realm);
      getArg(p,"realm", &sa->realm);
      free(sa->user);
      getArg(p,"username", &sa->user);
      free(sa->opaque);
      getArg(p,"opaque", &sa->opaque);
      free(sa->nonce);
      getArg(p,"nonce", &sa->nonce);
      free(sa->uri);
      getArg(p,"uri", &sa->uri);
      free(sa->response);
      getArg(p,"response", &sa->response);
      free(sa->cnonce);
      getArg(p,"cnonce", &sa->cnonce);
      free(sa->qop);
      getArg(p,"qop", &sa->qop);
      free(sa->nc);
      getArg(p,"nc", &sa->nc);
      if (sa->nc)
         sa->nonceCnt = atoi(sa->nc);

      if ( (strcmp(sa->user, username) == 0) && (strcmp(sa->nonce, sa->orignonce) == 0)) {
         generateRequestDigest(sa, sa->user, password);
         if (!memcmp(sa->response, sa->requestDigest, 32))
            return 1;
      }
   }
   /* else "basic" is not allowed on connectionRequest" */
   return 0;
}

int testChallenge(struct http_connection *c, char *msg)
{
   char *authorizationHdr = NULL;
   int ret;
   char *line, *p;

   /* get the authorization header part */
   p = msg;
   while (p != NULL)
   {
      line = strsep(&p, "\r\n");
      if (IMATCH_PREFIX(msg,"Authorization:"))
      {
         /* "skipping the Authorization: part" */
         authorizationHdr = &line[15];
         break;
      }      
   } /* while */
   
   if (authorizationHdr == NULL)
   {
      /* fail */
      return 0;
   }

#if 0
   printf("authorizationHdr %s\n",authorizationHdr);
#endif

   ret = parseAuthorizationHdr(authorizationHdr, &c->auth,
                               (char*)TR64_DSLF_CONFIG,pTr64Data->password);
   if ((ret == 0)  && (strcmp(c->auth.user,TR64_DSLF_RESET) == 0) && (c->setPasswordFlag))
   {
      /* dslf-reset: redo the test with TR64_DSLF_RESET and TR64_DSLF_RESET_PWD */
         ret = parseAuthorizationHdr(authorizationHdr, &c->auth,
                                     (char*)TR64_DSLF_RESET,(char*)TR64_DSLF_RESET_PWD);
   } /* special for dslf-reset */

   return (ret);
}


