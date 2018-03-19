/******************************************************************************
 *
 *  Copyright (c) 2009  Broadcom Corporation
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
/***************************************************************************
 *
 *     Copyright (c) 2008-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *  Description: SHA1-based key hash for MoCA 1.x Privacy
 *
 ***************************************************************************/

#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <moca_os.h>

////// Prototypes
static unsigned long int ft(int, unsigned long int, unsigned long int,
		     unsigned long int);
static unsigned long int k(int t);
static unsigned long int rotl(int, unsigned long int);

static void addParity(const unsigned char *in, unsigned char *result);
static int KeyGen(unsigned char *a, const unsigned char *b, int c);
static int sha1(unsigned char *, int, unsigned char *);

#define KEYTYPE_MMK		0
#define KEYTYPE_PMK		1
#define MIN_PASSWORD		12
#define MAX_PASSWORD		17

#define MOCA_HASH_SALT_STRING  ("MocaPasswordHash")
#define MOCA_HASH_SALT_STRLEN  ((int)strlen(MOCA_HASH_SALT_STRING))


/////////////////////////////////////////////////////
static unsigned long int ft(int t, unsigned long int x, unsigned long int y,
		     unsigned long int z)
{
	unsigned long int a, b, c;

	if (t < 20) {
		a = x & y;
		b = (~x) & z;
		c = a ^ b;
	} else if (t < 40) {
		c = x ^ y ^ z;
	} else if (t < 60) {
		a = x & y;
		b = a ^ (x & z);
		c = b ^ (y & z);
	} else if (t < 80) {
		c = (x ^ y) ^ z;
	} else {
		return 0;	// just for not having compile error
	}
	return c;
}

static unsigned long int k(int t)
{
	unsigned long int c;

	if (t < 20) {
		c = 0x5a827999;
	} else if (t < 40) {
		c = 0x6ed9eba1;
	} else if (t < 60) {
		c = 0x8f1bbcdc;
	} else if (t < 80) {
		c = 0xca62c1d6;
	} else {
		return 0;	// just for not having compile error
	}

	return c;
}

static unsigned long int rotl(int bits, unsigned long int a)
{

	unsigned long int c, d, e, f, g;

	c = (0x0001 << (32 - bits)) - 1;
	d = ~c;

	e = (a & c) << bits;
	f = (a & d) >> (32 - bits);

	g = e | f;

	return (g & 0xffffffff);

}

/* message must be a buffer of at least 64 bytes */
static int sha1(unsigned char *message, int message_length,
	unsigned char *digest)
{

	int i;
	int num_blocks;
	int block_remainder;
	int padded_length;

	unsigned long int l;
	unsigned long int t;
	unsigned long int h[5];
	unsigned long int a, b, c, d, e;
	unsigned long int w[80];
	unsigned long int temp;

	/* Calculate the number of 512 bit blocks */

	padded_length = message_length + 8;	/* Add length for l */
	padded_length = padded_length + 1;	/* Add the 0x01 bit postfix */

	l = message_length * 8;

	num_blocks = padded_length / 64;
	block_remainder = padded_length % 64;

	if (block_remainder > 0) {
		num_blocks++;
	}

	padded_length = padded_length + (64 - block_remainder);

	/* clear the padding field */
	for (i = message_length; i < (num_blocks * 64); i++) {
		message[i] = 0x00;
	}

	/* insert b1 padding bit */
	message[message_length] = 0x80;

	/* Insert l */
	message[(num_blocks * 64) - 1] = (unsigned char)(l & 0xff);
	message[(num_blocks * 64) - 2] = (unsigned char)((l >> 8) & 0xff);
	message[(num_blocks * 64) - 3] = (unsigned char)((l >> 16) & 0xff);
	message[(num_blocks * 64) - 4] = (unsigned char)((l >> 24) & 0xff);

	/* Set initial hash state */
	h[0] = 0x67452301;
	h[1] = 0xefcdab89;
	h[2] = 0x98badcfe;
	h[3] = 0x10325476;
	h[4] = 0xc3d2e1f0;

	for (i = 0; i < num_blocks; i++) {
		/* Prepare the message schedule */
		for (t = 0; t < 80; t++) {
			if (t < 16) {
				w[t] =
				    (256 * 256 * 256) * message[(i * 64) +
								(t * 4)];
				w[t] +=
				    (256 * 256) * message[(i * 64) + (t * 4) +
							  1];
				w[t] += (256) * message[(i * 64) + (t * 4) + 2];
				w[t] += message[(i * 64) + (t * 4) + 3];
			} else if (t < 80) {
				w[t] =
				    rotl(1,
					 (w[t - 3] ^ w[t - 8] ^ w[t - 14] ^
					  w[t - 16]));
			}
		}

		/* Initialize the five working variables */
		a = h[0];
		b = h[1];
		c = h[2];
		d = h[3];
		e = h[4];

		/* iterate a-e 80 times */

		for (t = 0; t < 80; t++) {
			temp = (rotl(5, a) + ft(t, b, c, d)) & 0xffffffff;
			temp = (temp + e) & 0xffffffff;
			temp = (temp + k(t)) & 0xffffffff;
			temp = (temp + w[t]) & 0xffffffff;
			e = d;
			d = c;
			c = rotl(30, b);
			b = a;
			a = temp;
		}

		/* compute the ith intermediate hash value */

		h[0] = (a + h[0]) & 0xffffffff;
		h[1] = (b + h[1]) & 0xffffffff;
		h[2] = (c + h[2]) & 0xffffffff;
		h[3] = (d + h[3]) & 0xffffffff;
		h[4] = (e + h[4]) & 0xffffffff;
	}

	digest[3] = (unsigned char)(h[0] & 0xff);
	digest[2] = (unsigned char)((h[0] >> 8) & 0xff);
	digest[1] = (unsigned char)((h[0] >> 16) & 0xff);
	digest[0] = (unsigned char)((h[0] >> 24) & 0xff);

	digest[7] = (unsigned char)(h[1] & 0xff);
	digest[6] = (unsigned char)((h[1] >> 8) & 0xff);
	digest[5] = (unsigned char)((h[1] >> 16) & 0xff);
	digest[4] = (unsigned char)((h[1] >> 24) & 0xff);

	digest[11] = (unsigned char)(h[2] & 0xff);
	digest[10] = (unsigned char)((h[2] >> 8) & 0xff);
	digest[9] = (unsigned char)((h[2] >> 16) & 0xff);
	digest[8] = (unsigned char)((h[2] >> 24) & 0xff);

	digest[15] = (unsigned char)(h[3] & 0xff);
	digest[14] = (unsigned char)((h[3] >> 8) & 0xff);
	digest[13] = (unsigned char)((h[3] >> 16) & 0xff);
	digest[12] = (unsigned char)((h[3] >> 24) & 0xff);

	digest[19] = (unsigned char)(h[4] & 0xff);
	digest[18] = (unsigned char)((h[4] >> 8) & 0xff);
	digest[17] = (unsigned char)((h[4] >> 16) & 0xff);
	digest[16] = (unsigned char)((h[4] >> 24) & 0xff);

	return 0;
}

int mocad_validate_password(const unsigned char *in)
{
	int i, len = (int)strlen((const char *)in);

	/* Convert the user password to 17-digit padded format */
	if((len > MAX_PASSWORD) || (len < MIN_PASSWORD))
		return(-EINVAL);

	for(i = 0; i < len; i++)
		if((in[i] < '0') || (in[i] > '9'))
			return(-EINVAL);

	return 0;
}

void mocad_random_password(unsigned char *out)
{
   unsigned int r;
   int i;
   
   for (i = 0; i < MAX_PASSWORD; i++) {
      r = MoCAOS_GetRandomValue();
      out[i] = (r % 10) + '0';
   }
   out[MAX_PASSWORD] = '\0';
}

int mocad_keygen(uint8_t mmk[8], uint8_t pmki[8], const unsigned char *in)
{
	unsigned char password[MAX_PASSWORD + 1];
	int len = (int)strlen((const char *)in);
	int ret = 0;

	ret = mocad_validate_password(in);
	if (ret != 0)
		return(ret);

	memset(password, '0', MAX_PASSWORD);
	password[MAX_PASSWORD] = 0;

	memcpy(&password[MAX_PASSWORD - len], in, len);

	KeyGen(mmk, password, KEYTYPE_MMK);
	KeyGen(pmki, password, KEYTYPE_PMK);
	return(0);
}

static int KeyGen(unsigned char *MKey, const unsigned char *PasswordString,
	   int KeyType)
{
	unsigned char PasswordSeed[20];
	unsigned char KeySeedString[128];
	unsigned char KeySeed[21];
	unsigned char l_psswdString[64] = { 0 };
	unsigned char key56[7];
	const char KS[][32] = { "MACManagementKey", "PrivacyManagementKey" };
	int len, i;

	//Compute PasswordSeed with SHA-1
	len = MAX_PASSWORD;
	memcpy(l_psswdString, PasswordString, len);	// Overrun Protection
	sha1(l_psswdString, len, PasswordSeed);

	// Concatenate the PasswordSeed with the Key String

	memset(KeySeedString, 0, sizeof(KeySeedString));
	memcpy(KeySeedString, PasswordSeed, sizeof(PasswordSeed));

	memcpy(&KeySeedString[sizeof(KeySeed) - 1], KS[KeyType],
	       strlen(KS[KeyType]));
	len = sizeof(PasswordSeed) + (int)strlen(KS[KeyType]);

	//Compute KeySeed with SHA1
	memset(KeySeed, 0, sizeof(KeySeed));
	sha1(KeySeedString, len, KeySeed);

	// Pad the KeySeed to length 168 (repeat last byte)
	KeySeed[20] = KeySeed[19];

	//Key56 = KeySeed[0:6] XOR KeySeed[7:13] XOR KeySeed[14:20]
	for(i = 0; i < 7; i++)
		key56[i] = KeySeed[0 + i] ^ KeySeed[7 + i] ^ KeySeed[14 + i];

	//Add parity bit to each 7_bit chunk to get a 64_bit key
	addParity(key56, MKey);

	return 0;
}

/*
* Add a parity bit to each 7_bit chunk of the 56 bits key
*
*/
static void addParity(const unsigned char *in, unsigned char *result)
{
	int index = 1;		// Keeps track of the bit position in the result
	int bitCount = 0;	// Keeps track of the number of 1 bits in each 7-bit chunk
	int i;
	char bit;

	// YBH - Clear result before start - a must
	memset(result, 0x0, 8 * sizeof(unsigned char));

	// Process each of the 56 bits        
	for (i = 0; i < 56; i++) {
		// Get the bit at bit position i
		bit = (in[6 - i / 8] & (1 << (i % 8))) > 0;

		// If set, set the corresponding bit in the result
		if (bit) {
			result[7 - index / 8] |= (1 << (index % 8)) & 0xFF;
			bitCount++;
		}
		// Set the parity bit after every 7 bits
		if ((i + 1) % 7 == 0) {
			if (bitCount % 2 == 0) {
				// Set low-order bit (parity bit) if bit count is even
				result[7 - index / 8] |= 0x01;
			}
			//                     else{
			//                             result[7-index/8] &= 0xFE;
			//                     }
			index++;
			bitCount = 0;
		}
		index++;
	}

}


/** Calculates the MoCA MIB password hash value.
 *
 * This function takes in a string password input and calculates the
 * mocaIfPasswordHash value. From the spec:
 * "MoCA password Hash is a 16-bit value determined by performing SHA-1 
 *  on a 0-extended password concatenated with a SALT value. First, if 
 *  the password is less than 17 digits, the password bytes are prepended 
 *  with ASCII '0' in order to create a 17 digit value. Second, the 
 *  17-digit result is concatenated with the static SALT string 
 *  'MocaPasswordHash' to create the SHA input. The SHA-1 algorithm is 
 *  performed on the SHA input to create a 160-bit output. The 16-LSBs of 
 *  the SHA-1 output are extracted as the password hash."
 *
 * @param password (IN) String pointer to MoCA password.
 * @return 2-byte hash value in binary.
 */
uint16_t moca_password_hash (char * password)
{
	uint8_t       shaInput[64] = { 0 }; /* must be 64-bytes for sha1() */
	uint16_t      retVal;
	unsigned char sha1_digest[20];
	int           password_len = 0;

	if (password == NULL)
		return 0;

	/* pre-fill with 0x30 */
	memset(shaInput, 0x30, MAX_PASSWORD);

	password_len = (int)strlen(password);
	if (password_len > MAX_PASSWORD)
		password_len = MAX_PASSWORD;

	/* stuff password into SHA input preserving 0x30 in MS bytes */
	memcpy( &shaInput[MAX_PASSWORD - password_len], 
	        password, password_len);

	/* copy salt into SHA input */
	memcpy( &shaInput[MAX_PASSWORD], MOCA_HASH_SALT_STRING,
	        MOCA_HASH_SALT_STRLEN);

	sha1(shaInput, (MAX_PASSWORD + MOCA_HASH_SALT_STRLEN), sha1_digest);

	/* return last 16 bits */
	retVal = ((uint16_t) sha1_digest[18] << 8) | ((uint16_t) sha1_digest[19]);

	return (retVal);
}

