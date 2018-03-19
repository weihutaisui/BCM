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
 * $Id: vxsearch.c 241182 2011-02-17 21:50:03Z $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vxsearch.h>

int
hcreate_r(size_t nel, struct hsearch_data *htab)
{
	int sizeLog2 = 0;

	if (!htab)
		return 0;

	/* Allocate enough nodes */
	if (!(htab->table = malloc(sizeof(ENTRY) * nel)))
		return 0;
	htab->size = nel;
	htab->filled = 0;

	/* Calculate nel log 2 */
	while (nel) {
		sizeLog2++;
		nel >>= 1;
	}

	/* Create hash table */
	htab->id = hashTblCreate(sizeLog2, hashKeyStrCmp, hashFuncIterScale, 0);

	return htab->id ? 1 : 0;
}

void
hdestroy_r(struct hsearch_data *htab)
{
	if (!htab)
		return;
	if (htab->table)
		free(htab->table);
	if (htab->id)
		hashTblDelete(htab->id);
	htab->table = NULL;
	htab->id = (HASH_ID) NULL;
}

int
hsearch_r(ENTRY item, ACTION action, ENTRY **retval, struct hsearch_data *htab)
{
	/* Fill in H_NODE_STRING */
	item.node.string = item.key;
	item.node.data = (int) item.key;

	if (action == ENTER) {
		/* Allocate a node */
		if (htab->filled == htab->size)
			return 0;
		*retval = &htab->table[htab->filled++];
		memcpy(*retval, &item, sizeof(ENTRY));
		/* Put node in the hash */
		if (hashTblPut(htab->id, (HASH_NODE *) *retval) == OK)
			return 1;
	} else if (action == FIND) {
		/* Find node */
		*retval = (ENTRY *) hashTblFind(htab->id, (HASH_NODE *) &item, 0);
		return 1;
	}

	return 0;
}

/* The non-reentrant version use a global space for storing the table.  */
static struct hsearch_data global_htab;

/* Define the non-reentrant function using the reentrant counterparts.  */
ENTRY *
hsearch(ENTRY item, ACTION action)
{
	ENTRY *result;

	(void) hsearch_r(item, action, &result, &global_htab);

	return result;
}

int
hcreate(size_t nel)
{
	return hcreate_r(nel, &global_htab);
}

void
hdestroy(void)
{
	hdestroy_r(&global_htab);
}
