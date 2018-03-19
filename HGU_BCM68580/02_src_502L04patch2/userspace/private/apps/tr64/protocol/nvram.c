/*
 * NVRAM variable manipulation (common)
 *
 * Copyright (c) 2003-2012 Broadcom Corporation
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
 * $Id: nvram.c,v 1.33.8.1 2003/10/16 21:26:03 mthawani Exp $
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <typedefs.h>
#include <linux_osl.h>
#include <bcmendian.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <sbsdram.h>

#define MALLOC(x) malloc(x)
#define MFREE(buf, size) free(buf)

char * _nvram_get(const char *name);
int _nvram_set(const char *name, const char *value);
int _nvram_unset(const char *name);
int _nvram_getall(char *buf, int count);
int _nvram_commit(struct nvram_header *header);
int _nvram_init(void);
void _nvram_exit(void);

struct nvram_header nv_header = { 0x00d00d00, 0xabcdef00, 0xabcdef00, 0xabcdef00, 0xabcdef00 };
static struct nvram_tuple * nvram_hash[257] = { NULL };
static struct nvram_tuple * nvram_dead = NULL;

static int
_nvram_read(void *buf)
{
        uint32 *src, *dst;
        uint i;

        if (!nv_header.magic)
                return -19; /* -ENODEV */

        src = (uint32 *) &nv_header;
        dst = (uint32 *) buf;

        for (i = 0; i < sizeof(struct nvram_header); i += 4)
                *dst++ = *src++;

        /* PR2620 WAR: Read data bytes as words */
        for (; i < nv_header.len && i < NVRAM_SPACE; i += 4) {
                *dst++ = ltoh32(*src++);
	}

        return 0;
}

static struct nvram_tuple *
_nvram_realloc(struct nvram_tuple *t, const char *name, const char *value)
{
        if (!(t = MALLOC(sizeof(struct nvram_tuple) + strlen(name) + 1 + strlen(value) + 1))) {
		printf ( "malloc failed\n");
                return NULL;
	}

        /* Copy name */
        t->name = (char *) &t[1];
        strcpy(t->name, name);

        /* Copy value */
        t->value = t->name + strlen(name) + 1;
        strcpy(t->value, value);

        return t;
}

static void _nvram_free(struct nvram_tuple *t)
{
    if (t) {
        MFREE(t, sizeof(struct nvram_tuple) + strlen(t->name) + 1 + strlen(t->value) + 1);
    }
}

/* Free all tuples. Should be locked. */
static void
nvram_free(void)
{
	uint i;
	struct nvram_tuple *t, *next;

	/* Free hash table */
	for (i = 0; i < ARRAYSIZE(nvram_hash); i++) {
		for (t = nvram_hash[i]; t; t = next) {
			next = t->next;
			_nvram_free(t);
		}
		nvram_hash[i] = NULL;
	}

	/* Free dead table */
	for (t = nvram_dead; t; t = next) {
		next = t->next;
		_nvram_free(t);
	}
	nvram_dead = NULL;

	/* Indicate to per-port code that all tuples have been freed */
	_nvram_free(NULL);
}

/* String hash */
static INLINE uint
hash(const char *s)
{
	uint hash = 0;

	while (*s) {
		hash = 31 * hash + *s++;
	}

	return hash;
}

/* (Re)initialize the hash table. Should be locked. */
static int
nvram_rehash(struct nvram_header *header)
{
	char buf[] = "0xXXXXXXXX", *name, *value, *end, *eq;

	/* (Re)initialize hash table */
	nvram_free();

	/* Parse and set "name=value\0 ... \0\0" */
	name = (char *) &header[1];
	end = (char *) header + NVRAM_SPACE - 2;
	end[0] = end[1] = '\0';
	for (; *name; name = value + strlen(value) + 1) {
		if (!(eq = strchr(name, '=')))
			break;
		*eq = '\0';
		value = eq + 1;
		_nvram_set(name, value);
		*eq = '=';
	}

	/* Set special SDRAM parameters */
	if (!_nvram_get("sdram_init")) {
		sprintf(buf, "0x%04X", (uint16)(header->crc_ver_init >> 16));
		_nvram_set("sdram_init", buf);
	}
	if (!_nvram_get("sdram_config")) {
		sprintf(buf, "0x%04X", (uint16)(header->config_refresh & 0xffff));
		_nvram_set("sdram_config", buf);
	}
	if (!_nvram_get("sdram_refresh")) {
		sprintf(buf, "0x%04X", (uint16)((header->config_refresh >> 16) & 0xffff));
		_nvram_set("sdram_refresh", buf);
	}
	if (!_nvram_get("sdram_ncdl")) {
		sprintf(buf, "0x%08X", header->config_ncdl);
		_nvram_set("sdram_ncdl", buf);
	}

	return 0;
}

/* Get the value of an NVRAM variable. Should be locked. */
char *
_nvram_get(const char *name)
{
	uint i;
	struct nvram_tuple *t;
	char *value;

	if (!name)
		return NULL;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(nvram_hash);

	/* Find the associated tuple in the hash table */
	for (t = nvram_hash[i]; t && strcmp(t->name, name); t = t->next) {
		;
	}

	value = t ? t->value : NULL;

	return value;
}

/* Get the value of an NVRAM variable. Should be locked. */
int
_nvram_set(const char *name, const char *value)
{
	uint i;
	struct nvram_tuple *t, *u, **prev;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(nvram_hash);

	/* Find the associated tuple in the hash table */
	for (prev = &nvram_hash[i], t = *prev; t && strcmp(t->name, name); prev = &t->next, t = *prev) {
		;
	}

	/* (Re)allocate tuple */
	if (!(u = _nvram_realloc(t, name, value))) {
		return -12; /* -ENOMEM */
	}

	/* Value reallocated */
	if (t && t == u) {
		return 0;
        }

	/* Move old tuple to the dead table */
	if (t) {
		*prev = t->next;
		t->next = nvram_dead;
		nvram_dead = t;
	}

	/* Add new tuple to the hash table */
	u->next = nvram_hash[i];
	nvram_hash[i] = u;

	return 0;
}

/* Unset the value of an NVRAM variable. Should be locked. */
int
_nvram_unset(const char *name)
{
	uint i;
	struct nvram_tuple *t, **prev;

	if (!name)
		return 0;

	/* Hash the name */
	i = hash(name) % ARRAYSIZE(nvram_hash);

	/* Find the associated tuple in the hash table */
	for (prev = &nvram_hash[i], t = *prev; t && strcmp(t->name, name); prev = &t->next, t = *prev) {
		;
	}

	/* Move it to the dead table */
	if (t) {
		*prev = t->next;
		t->next = nvram_dead;
		nvram_dead = t;
	}

	return 0;
}

/* Get all NVRAM variables. Should be locked. */
int
_nvram_getall(char *buf, int count)
{
	uint i;
	struct nvram_tuple *t;
	int len = 0;

	bzero(buf, count);

	/* Write name=value\0 ... \0\0 */
	for (i = 0; i < ARRAYSIZE(nvram_hash); i++) {
		for (t = nvram_hash[i]; t; t = t->next) {
			if ((count - len) > (strlen(t->name) + 1 + strlen(t->value) + 1))
				len += sprintf(buf + len, "%s=%s", t->name, t->value) + 1;
			else
				break;
		}
	}

	return 0;
}

/* Regenerate NVRAM. Should be locked. */
int
_nvram_commit(struct nvram_header *header)
{
	char *ptr, *end;
	int i;
	struct nvram_tuple *t;

	/* Regenerate header */
	header->magic = NVRAM_MAGIC;

	/* Clear data area */
	ptr = (char *) header + sizeof(struct nvram_header);
	bzero(ptr, NVRAM_SPACE - sizeof(struct nvram_header));

	/* Leave space for a double NUL at the end */
	end = (char *) header + NVRAM_SPACE - 2;

	/* Write out all tuples */
	for (i = 0; i < ARRAYSIZE(nvram_hash); i++) {
		for (t = nvram_hash[i]; t; t = t->next) {
			if ((ptr + strlen(t->name) + 1 + strlen(t->value) + 1) > end)
				break;
			ptr += sprintf(ptr, "%s=%s", t->name, t->value) + 1;
		}
	}

	/* End with a double NUL */
	ptr += 2;

	/* Set new length */
	header->len = ROUNDUP(ptr - (char *) header, 4);
	/* Reinitialize hash table */
	return nvram_rehash(header);
}

/* Initialize hash table. Should be locked. */
int
_nvram_init(void)
{
	struct nvram_header *header;
	int ret;

	if (!(header = (struct nvram_header *) MALLOC(NVRAM_SPACE))) {
		printf("nvram_init: out of memory\n");
		return -12; /* -ENOMEM */
	}

	if ((ret = _nvram_read(header)) == 0 &&
	    header->magic == NVRAM_MAGIC)
		nvram_rehash(header);

	MFREE(header, NVRAM_SPACE);
	return ret;
}

/* Free hash table. Should be locked. */
void
_nvram_exit(void)
{
	nvram_free();
}

int
nvram_commit(void)
{
        struct nvram_header *header;
        int ret;
        uint32 *src, *dst;
        uint i;

        if (!(header = (struct nvram_header *) MALLOC(NVRAM_SPACE))) {
                printf("nvram_commit: out of memory\n");
                return -12; /* -ENOMEM */
        }

        /* Regenerate NVRAM */
        ret = _nvram_commit(header);
        if (ret)
                goto done;

        src = (uint32 *) &header[1];
        dst = src;

        /* PR2620 WAR: Write data bytes as words */
        for (i = sizeof(struct nvram_header); i < header->len && i < NVRAM_SPACE; i += 4)
                *dst++ = htol32(*src++);

 done:
        MFREE(header, NVRAM_SPACE);
        return ret;
}

