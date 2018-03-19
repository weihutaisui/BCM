/*****************************************************************************
 * Copyright (C) 2004,2005,2006,2007,2008 Katalix Systems Ltd
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
 * Simple tests for USL code
 */

#include <stdio.h>

#include "usl_list.h"

static USL_LIST_HEAD(list_one);
static USL_LIST_HEAD(list_two);

struct usl_item {
	char name[16];
	struct usl_list_head list;
};

static struct usl_item item_one, item_two, item_three;

struct usl_test_desc {
	const char *name;
	int (*func)(int arg);
	int arg;
};

static int test_list_walk(int arg)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct usl_item *item;

	usl_list_add(&item_one.list, &list_one);

	usl_list_for_each(walk, tmp, &list_one) {
		item = usl_list_entry(walk, struct usl_item, list);
		printf("Got %s\n", item->name); 
	}

	usl_list_del(&item_one.list);

	usl_list_for_each(walk, tmp, &list_one) {
		item = usl_list_entry(walk, struct usl_item, list);
		printf("Got %s\n", item->name); 
	}

	return 0;
}

static int test_list_walk_2(int arg)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct usl_item *item;

	usl_list_add_tail(&item_one.list, &list_one);
	usl_list_add_tail(&item_two.list, &list_one);
	usl_list_add_tail(&item_three.list, &list_one);

	printf("After item one, two, three added......\n");

	usl_list_for_each(walk, tmp, &list_one) {
		item = usl_list_entry(walk, struct usl_item, list);
		printf("Got %s\n", item->name); 
	}

	usl_list_del(&item_one.list);

	printf("After item one deleted......\n");
	usl_list_for_each(walk, tmp, &list_one) {
		item = usl_list_entry(walk, struct usl_item, list);
		printf("Got %s\n", item->name); 
	}

	usl_list_del(&item_three.list);
	usl_list_del(&item_two.list);

	printf("List should now be empty...\n");
	usl_list_for_each(walk, tmp, &list_one) {
		item = usl_list_entry(walk, struct usl_item, list);
		printf("Got %s\n", item->name); 
	}

	return 0;
}

static int test_list_queue(int arg)
{
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct usl_item *item;

	usl_list_add_tail(&item_one.list, &list_one);
	usl_list_add_tail(&item_two.list, &list_one);
	usl_list_add_tail(&item_three.list, &list_one);

	printf("After item one, two, three added......\n");

	usl_list_for_each(walk, tmp, &list_one) {
		item = usl_list_entry(walk, struct usl_item, list);
		printf("Got %s\n", item->name); 
	}

	printf("Read items off queue...\n");
	for (;;) {
		if (usl_list_empty(&list_one)) {
			break;
		}	
		item = usl_list_entry(list_one.next, struct usl_item, list);
		usl_list_del(&item->list);
		printf("Got %s\n", item->name); 
	}

	printf("List should now be empty...\n");
	usl_list_for_each(walk, tmp, &list_one) {
		item = usl_list_entry(walk, struct usl_item, list);
		printf("Got %s\n", item->name); 
	}

	return 0;
}

static struct usl_test_desc tests[] = {
	{ "list walk", test_list_walk, 0 },
	{ "list walk repeat", test_list_walk, 1 },
	{ "list walk 2", test_list_walk_2, 0 },
	{ "queue", test_list_queue, 0 },
	{ NULL, NULL, 0 }
};

int main(int argc, char **argv)
{
	struct usl_test_desc *test;
	int result;

	strcpy(&item_one.name[0], "item_one");
	USL_LIST_HEAD_INIT(&item_one.list);

	strcpy(&item_two.name[0], "item_two");
	USL_LIST_HEAD_INIT(&item_two.list);

	strcpy(&item_three.name[0], "item_three");
	USL_LIST_HEAD_INIT(&item_three.list);

	for (test = &tests[0]; test->func != NULL; test++) {
		printf("TEST: %s\n", test->name);
		result = test->func(test->arg);
		printf("TEST: %s: %s\n", test->name, result == 0 ? "PASS" : "FAIL");
	}
}
