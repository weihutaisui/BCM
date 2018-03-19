/*
 * SVID hsearch (with GNU extensions) for VxWorks (implemented with hashLib)
 *
 * Copyright (C) 2016, Broadcom. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: vxsearch.h 241182 2011-02-17 21:50:03Z $
 */

#ifndef _vxsearch_h_
#define _vxsearch_h_

#include <hashLib.h>

typedef enum {
	FIND,
	ENTER
} ACTION;

typedef struct entry {
	H_NODE_STRING node;
	char *key;
	void *data;
} ENTRY;

struct hsearch_data {
	ENTRY *table;
	unsigned int size;
	unsigned int filled;
	HASH_ID id;
};

extern ENTRY *hsearch(ENTRY item, ACTION action);
extern int hcreate(size_t nel);
extern void hdestroy(void);
extern int hsearch_r(ENTRY item, ACTION action, ENTRY **retval, struct hsearch_data *htab);
extern int hcreate_r(size_t nel, struct hsearch_data *htab);
extern void hdestroy_r(struct hsearch_data *htab);

#endif
