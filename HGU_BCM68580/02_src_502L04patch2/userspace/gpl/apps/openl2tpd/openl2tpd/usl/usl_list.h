/*****************************************************************************
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 *
 *****************************************************************************/

/*
 * This code is lifted from the Linux kernel include/linux/list.h.
 * It's been slightly modified to remove the dependency on prefetch.h and
 * to comply with local naming conventions.
 */

#ifndef USL_LIST_H
#define USL_LIST_H

#include <stddef.h>

#ifndef barrier
#define barrier() __asm__ __volatile__("": : :"memory")
#endif

/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define USL_LIST_POISON1  ((void *) 0x00100100)
#define USL_LIST_POISON2  ((void *) 0x00200200)

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct usl_list_head {
	struct usl_list_head *next;
	struct usl_list_head *prev;
};

#define USL_LIST_HEAD_NODE(name) { &(name), &(name) }

#define USL_LIST_HEAD(name) \
	struct usl_list_head name = USL_LIST_HEAD_NODE(name)

#define USL_LIST_HEAD_INIT(ptr) do { 			\
	(ptr)->next = (ptr); (ptr)->prev = (ptr); 	\
} while (0)

/*
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __usl_list_add(struct usl_list_head *new,
			      struct usl_list_head *prev,
			      struct usl_list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
	barrier();
}

/**
 * usl_list_add - add a new entry
 * @new: new entry to be added
 * @head: usl_list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void usl_list_add(struct usl_list_head *new, struct usl_list_head *head)
{
	__usl_list_add(new, head, head->next);
}

/**
 * usl_list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void usl_list_add_tail(struct usl_list_head *new, struct usl_list_head *head)
{
	__usl_list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __usl_list_del(struct usl_list_head *prev, struct usl_list_head *next)
{
	next->prev = prev;
	prev->next = next;
	barrier();
}

/**
 * usl_list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is in an undefined state.
 */
static inline void usl_list_del(struct usl_list_head *entry)
{
	__usl_list_del(entry->prev, entry->next);
	entry->next = USL_LIST_POISON1;
	entry->prev = USL_LIST_POISON2;
}

/**
 * usl_list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void usl_list_del_init(struct usl_list_head *entry)
{
	__usl_list_del(entry->prev, entry->next);
	USL_LIST_HEAD_INIT(entry); 
}

/**
 * usl_list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void usl_list_move(struct usl_list_head *list, struct usl_list_head *head)
{
        __usl_list_del(list->prev, list->next);
        usl_list_add(list, head);
}

/**
 * usl_list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void usl_list_move_tail(struct usl_list_head *list,
				  struct usl_list_head *head)
{
        __usl_list_del(list->prev, list->next);
        usl_list_add_tail(list, head);
}

/**
 * usl_list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int usl_list_empty(struct usl_list_head *head)
{
	return head->next == head;
}

static inline void __usl_list_splice(struct usl_list_head *list,
				 struct usl_list_head *head)
{
	struct usl_list_head *first = list->next;
	struct usl_list_head *last = list->prev;
	struct usl_list_head *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}

/**
 * usl_list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void usl_list_splice(struct usl_list_head *list, struct usl_list_head *head)
{
	if (!usl_list_empty(list))
		__usl_list_splice(list, head);
}

/**
 * usl_list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void usl_list_splice_init(struct usl_list_head *list,
				    struct usl_list_head *head)
{
	if (!usl_list_empty(list)) {
		__usl_list_splice(list, head);
		USL_LIST_HEAD_INIT(list);
	}
}

/**
 * usl_list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define usl_list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * usl_list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define usl_list_for_each(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
	     pos = n, n = pos->next)

/*****************************************************************************
 * Ordered lists - extends the regular usl_list_head features.
 *****************************************************************************/

typedef int (*usl_list_compare_fn_t)(void *key1, void *key2, int key_len);

struct usl_ord_list_head {
  struct usl_ord_list_head 	*next, *prev; /* must be first */
  usl_list_compare_fn_t		compare;
  int 				key_len;
  void 				*key;
};

#define USL_ORD_LIST_HEAD_NODE(name, cmp, key, len) { &(name), &(name), cmp, len, key }

#define USL_ORD_LIST_HEAD(name, cmp, key, len) \
        struct usl_ord_list_head name = USL_ORD_LIST_HEAD_NODE(name, cmp, key, len)

#define USL_ORD_LIST_HEAD_INIT(_ptr, _cmp, _key, _len) do { \
        (_ptr)->next = (_ptr); (_ptr)->prev = (_ptr); \
        (_ptr)->compare = _cmp; (_ptr)->key = _key; (_ptr)->key_len = _len; \
} while (0)

/**
 * usl_ord_list_add - add a new entry to a keyed list
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry into the specified list head.
 * This is good for implementing ordered lists.
 */
static __inline__ void usl_ord_list_add(struct usl_ord_list_head *new, struct usl_ord_list_head *head)
{
	struct usl_ord_list_head *entry;
	struct usl_ord_list_head *tmp;

	for (entry = head->next, tmp = entry->next; 
	     entry != head; 
	     entry = tmp, tmp = entry->next) {
		if (head->compare(entry->key, new->key, new->key_len) > 0) {
			usl_list_add_tail((struct usl_list_head *)new, (struct usl_list_head *)entry);
			return;
		}
	}
	usl_list_add_tail((struct usl_list_head *)new, (struct usl_list_head *)head);
}

/**
 * usl_ord_list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: ord_list_empty on entry does not return true after this, the entry is in an undefined state.
 */
static __inline__ void usl_ord_list_del(struct usl_ord_list_head *entry)
{
	usl_list_del((struct usl_list_head *) entry);
}

/**
 * usl_ord_list_del_init - deletes entry from list and reinits it.
 * @entry: the element to delete from the list.
 * Note: ord_list_empty on entry does not return true after this, the entry is in an undefined state.
 */
static __inline__ void usl_ord_list_del_init(struct usl_ord_list_head *entry)
{
	usl_list_del_init((struct usl_list_head *) entry);
}

/**
 * usl_ord_list_empty - tests whether an ordered list is empty
 * @head: the list to test.
 */
static __inline__ int usl_ord_list_empty(struct usl_ord_list_head *head)
{
	return head->next == head;
}

/**
 * usl_ord_list_find - find an entry with the specified key in a keyed list
 * @head: the list to search
 * @key: the key to find
 * @len: the key length (or max length)
 *
 */
static __inline__ struct usl_ord_list_head *usl_ord_list_find(struct usl_ord_list_head *head, void *key, int key_len)
{
	struct usl_ord_list_head *entry;
	struct usl_ord_list_head *tmp;

	for (entry = head->next, tmp = entry->next; 
	     entry != head; 
	     entry = tmp, tmp = entry->next) {
		if (head->compare(entry->key, key, key_len) == 0) {
			return entry;
		}
	}
	return NULL;
}

/**
 * usl_ord_list_find_next - find an entry with the specified key in a keyed list
 * @head: the list to search
 * @key: the key to find
 * @len: the key length (or max length)
 *
 */
static __inline__ struct usl_ord_list_head *usl_ord_list_find_next(struct usl_ord_list_head *head, void *key, int key_len)
{
	struct usl_ord_list_head *entry;
	struct usl_ord_list_head *tmp;

	for (entry = head->next, tmp = entry->next; 
	     entry != head; 
	     entry = tmp, tmp = entry->next) {
		if (head->compare(entry->key, key, key_len) > 0) {
			return entry;
		}
	}
	return NULL;
}

/*****************************************************************************
 * Hash lists - Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 *****************************************************************************/

struct usl_hlist_head {
	struct usl_hlist_node *first;
};

struct usl_hlist_node {
	struct usl_hlist_node *next, **pprev;
};

#define USL_HLIST_HEAD(name) struct usl_hlist_head name = {  .first = NULL }
#define USL_HLIST_HEAD_INIT(ptr) ((ptr)->first = NULL)
#define USL_HLIST_NODE_INIT(ptr) ((ptr)->next = NULL, (ptr)->pprev = NULL)

static inline int usl_hlist_unhashed(const struct usl_hlist_node *h)
{
	return !h->pprev;
}

static inline int usl_hlist_empty(const struct usl_hlist_head *h)
{
	return !h->first;
}

static inline void __usl_hlist_del(struct usl_hlist_node *n)
{
	struct usl_hlist_node *next = n->next;
	struct usl_hlist_node **pprev = n->pprev;
	*pprev = next;
	if (next)
		next->pprev = pprev;
}

static inline void usl_hlist_del(struct usl_hlist_node *n)
{
	__usl_hlist_del(n);
	n->next = USL_LIST_POISON1;
	n->pprev = USL_LIST_POISON2;
}

static inline void usl_hlist_del_init(struct usl_hlist_node *n)
{
	if (n->pprev)  {
		__usl_hlist_del(n);
		USL_HLIST_NODE_INIT(n);
	}
}

static inline void usl_hlist_add_head(struct usl_hlist_node *n, struct usl_hlist_head *h)
{
	struct usl_hlist_node *first = h->first;
	n->next = first;
	if (first)
		first->pprev = &n->next;
	h->first = n;
	n->pprev = &h->first;
}

static inline void usl_hlist_add_before(struct usl_hlist_node *n, struct usl_hlist_node *next)
{
	n->pprev = next->pprev;
	n->next = next;
	next->pprev = &n->next;
	*(n->pprev) = n;
}

static inline void usl_hlist_add_after(struct usl_hlist_node *n, struct usl_hlist_node *next)
{
	next->next = n->next;
	n->next = next;
	next->pprev = &n->next;

	if(next->next)
		next->next->pprev  = &next->next;
}

#define usl_hlist_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define usl_hlist_for_each(pos, n, head) \
	for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
	     pos = n)

#endif /* USL_LIST_H */
