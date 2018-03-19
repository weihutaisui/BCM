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

#include <stdio.h>

#include "cli_api.h"

static void banner(const char *str)
{
	printf("----------------------------------------------------------------------------\n");
	printf("%s\n", str);	
	printf("----------------------------------------------------------------------------\n");
}

#define ARG(id, name, flag, type, doc) \
	{ name, { CLI_TEST_ARGID_##id, flag, cli_arg_parse_##type, doc } }

#define FLG(id, name, doc) \
	{ name, { CLI_TEST_ARGID_##id, CLI_ARG_FLAG_NO_VALUE, NULL, doc } }

#define CLI_TEST_ARGID_TRACE_FLAGS 	0
#define CLI_TEST_ARGID_DEBUG	 	1
#define CLI_TEST_ARGID_MORE_FLAGS 	2
#define CLI_TEST_ARGID_A_STRING 	3
#define CLI_TEST_ARGID_A_LONG_STRING 	4

static struct cli_arg_entry cli_args_system_modify[] = {
#if 0
	{ "trace_flags", 	{ 0, 0, 			cli_arg_parse_int32, 	"description on trace flags" } },
	{ "debug", 		{ 1, CLI_ARG_FLAG_NO_VALUE, 	NULL, 			"description on debug"} },
	{ "more_flags", 	{ 2, 0, 			cli_arg_parse_int32, 	"more flags" } },
	{ "a_string", 		{ 3, 0, 			cli_arg_parse_string, 	"a test string" } },
#else
	ARG(TRACE_FLAGS, 	"trace_flags", 		0, 	int32, "description on trace flags"),
	FLG(DEBUG, 		"debug", 				"description on debug"),
	ARG(MORE_FLAGS, 	"more_flags", 		0, 	int32, 	"description on more flags"),
	ARG(A_STRING, 		"a_string", 		0, 	string, "a test string"),
	ARG(A_LONG_STRING,	"a_long_string",	0,	string, ("Wibble is a word that I first heard a long "
									 "time ago on a British comedy show called "
									 "Black Adder. Rowan Akinson, the actor, put "
									 "on a funny face when he said it.\n"
									 "a) to make people laugh\n"
									 "b) to be stupid\n"
									 "c) wibble\n"
									 "British comedy rocks!\n")),
#endif
	{ NULL, },
};

static int cli_test_act_system_modify(struct cli_node *node, int argc, char *argv[], int *arg_num)
{
	int result;
	struct cli_node *args[100];
	char *arg_values[100];
	int num_args = 100;
	int arg;
	void *result_data;
	int result_len;

	printf("%s: node='%s', argc=%d\n", __func__, node->keyword, argc);

	result = cli_find_args(argc, argv, node, &args[0], &arg_values[0], &num_args);
	if (result == 0) {
		for (arg = 0; arg < num_args; arg++) {
			if (args[arg] && args[arg]->arg && args[arg]->arg->parser) {
				printf("Parsing arg %d, id=%d\n", arg, args[arg]->arg->id);
				result = (*args[arg]->arg->parser)(args[arg], arg_values[arg], &result_data, &result_len);
				if (result < 0) {
					/* tell caller which arg failed */
					*arg_num = arg;
					break;
				}
			}
		}
	} else {
		/* tell caller which arg failed */
		*arg_num = num_args;
	}
	return result;
}

/*
 * SYSTEM MODIFY ...
 * SYSTEM SHOW ...
 * TUNNEL CREATE ...
 * TUNNEL DELETE ...
 * TUNNEL MODIFY
 * TUNNEL SHOW
 * TUNNEL LIST
 * TUNNEL PROFILE CREATE ...
 * TUNNEL PROFILE DELETE ...
 * TUNNEL PROFILE MODIFY
 * TUNNEL PROFILE SHOW
 * TUNNEL PROFILE LIST
 * 
 */
static struct cli_node_entry cmds[] = {
	{ 0, CLI_NODE_TYPE_KEYWORD, "system", },
	// { 1, CLI_NODE_TYPE_COMMAND, "modify", },
	{ 1, CLI_NODE_TYPE_COMMAND, "modify", "modify system parameters", cli_test_act_system_modify, &cli_args_system_modify[0], },
	{ 1, CLI_NODE_TYPE_COMMAND, "show", "show system bollox" },
	{ 0, CLI_NODE_TYPE_KEYWORD, "test", "test some stuff" },
	{ 1, CLI_NODE_TYPE_KEYWORD, "one", },
	{ 2, CLI_NODE_TYPE_KEYWORD, "two", },
	{ 3, CLI_NODE_TYPE_COMMAND, "run", },
	{ 0, CLI_NODE_TYPE_KEYWORD, "tunnel", "tunnel commands" },
	{ 1, CLI_NODE_TYPE_COMMAND, "create", },
	{ 1, CLI_NODE_TYPE_COMMAND, "delete", },
	{ 1, CLI_NODE_TYPE_COMMAND, "modify", },
	{ 1, CLI_NODE_TYPE_COMMAND, "show", },
	{ 1, CLI_NODE_TYPE_COMMAND, "list", },
	{ 1, CLI_NODE_TYPE_KEYWORD, "profile", },
	{ 2, CLI_NODE_TYPE_COMMAND, "create", },
	{ 2, CLI_NODE_TYPE_COMMAND, "delete", },
	{ 2, CLI_NODE_TYPE_COMMAND, "modify", },
	{ 2, CLI_NODE_TYPE_COMMAND, "show", },
	{ 2, CLI_NODE_TYPE_COMMAND, "list", },
	{ 2, CLI_NODE_TYPE_COMMAND, "wibble", },
	{ 2, CLI_NODE_TYPE_COMMAND, "wibble2", },
	{ 0, CLI_NODE_TYPE_KEYWORD, "session", "session commands" },
	{ 1, CLI_NODE_TYPE_COMMAND, "create", },
	{ 1, CLI_NODE_TYPE_COMMAND, "delete", },
	{ 1, CLI_NODE_TYPE_COMMAND, "modify", },
	{ 1, CLI_NODE_TYPE_COMMAND, "show", },
	{ 1, CLI_NODE_TYPE_COMMAND, "list", },
	{ 1, CLI_NODE_TYPE_KEYWORD, "profile", "session profile commands" },
	{ 2, CLI_NODE_TYPE_COMMAND, "create", },
	{ 2, CLI_NODE_TYPE_COMMAND, "delete", },
	{ 2, CLI_NODE_TYPE_COMMAND, "modify", },
	{ 2, CLI_NODE_TYPE_COMMAND, "show", },
	{ 2, CLI_NODE_TYPE_COMMAND, "list", },
	{ 0, CLI_NODE_TYPE_KEYWORD, "ppp", "ppp bollox" },
	{ 1, CLI_NODE_TYPE_KEYWORD, "profile", "ppp profile commands" },
	{ 2, CLI_NODE_TYPE_COMMAND, "create", },
	{ 2, CLI_NODE_TYPE_COMMAND, "delete", },
	{ 2, CLI_NODE_TYPE_COMMAND, "modify", },
	{ 2, CLI_NODE_TYPE_COMMAND, "show", },
	{ 2, CLI_NODE_TYPE_COMMAND, "list", },
	{ 0, CLI_NODE_TYPE_KEYWORD, "peer", "peer commands" },
	{ 1, CLI_NODE_TYPE_KEYWORD, "profile", "peer profile commands" },
	{ 2, CLI_NODE_TYPE_COMMAND, "create", },
	{ 2, CLI_NODE_TYPE_COMMAND, "delete", },
	{ 2, CLI_NODE_TYPE_COMMAND, "modify", },
	{ 2, CLI_NODE_TYPE_COMMAND, "show", },
	{ 2, CLI_NODE_TYPE_COMMAND, "list", },
	{ 0, CLI_NODE_TYPE_END, NULL, },
};

static void dump_buddies(struct cli_node *my_node, int quiet)
{
	struct cli_node *node = my_node;
	int level;

	if (!quiet) {
		printf("Buddies of node: '%s', level %d\n", node->keyword, node->level);
	}
	level = node->level;
	for (node = my_node->first; node != NULL; node = node->next) {
		if (node == my_node) {
			continue;
		}
		printf("  Node: '%s', level %d\n", node->keyword, node->level);
		if (node->level != level) {
			printf("      at wrong level!\n");
		}
	}
}

static void dump_kids(struct cli_node *node)
{
	printf("Child of node: '%s', level %d\n", node->keyword, node->level);

	if (node->child != NULL) {
		node = node->child;
		printf("  Node: '%s', level %d\n", node->keyword, node->level);
		if (node->first != NULL) {
			dump_buddies(node, 1);
		}
	}
}

int main(int argc, char *argv[])
{
	int result;
	struct cli_node *node;
	struct cli_node *matches[30];
	int num_matches = 30;
	int match;

	cli_init("test");
	result = cli_add_commands(&cmds[0]);
	cli_dump_commands();
	printf("dump: result = %d\n", result);
	if (argc > 1) {
		node = cli_find_command(argc - 1, &argv[1], &matches[0], &num_matches);
		if (node != NULL) {
			for (match = 0; match < num_matches; match++) {
				node = matches[match];
				printf("node[%d] = %p (%s:%s)\n", match, node, 
				       node->type == CLI_NODE_TYPE_KEYWORD ? "KEYWORD" : "COMMAND",
				       node->keyword);
			}
		} else {
			printf("find: invalid command sequence\n");
		}
	} else {
		char *argv[10];

		banner("test");
		argv[0] = "test";
		argv[1] = NULL;
		node = cli_find_command(1, &argv[0], &matches[0], &num_matches);
		if (node != NULL) {
			dump_buddies(node, 0);
			dump_kids(node);
		} else {
			printf("UNMATCHED\n");
		}

		banner("tunnel");
		argv[0] = "tunnel";
		argv[1] = NULL;
		node = cli_find_command(1, &argv[0], &matches[0], &num_matches);
		if (node != NULL) {
			dump_buddies(node, 0);
			dump_kids(node);
		} else {
			printf("UNMATCHED\n");
		}

		banner("system");
		argv[0] = "system";
		argv[1] = NULL;
		node = cli_find_command(1, &argv[0], &matches[0], &num_matches);
		if (node != NULL) {
			dump_buddies(node, 0);
			dump_kids(node);
		} else {
			printf("UNMATCHED\n");
		}

		banner("system modify");
		argv[0] = "system";
		argv[1] = "modify";
		argv[2] = NULL;
		node = cli_find_command(2, &argv[0], &matches[0], &num_matches);
		if (node != NULL) {
			dump_buddies(node, 0);
			dump_kids(node);
		} else {
			printf("UNMATCHED\n");
		}

		banner("test one");
		argv[0] = "test";
		argv[1] = "one";
		argv[2] = NULL;
		node = cli_find_command(2, &argv[0], &matches[0], &num_matches);	
		if (node != NULL) {
			printf("matches node '%s', level %d\n", node->keyword, node->level);
		} else {
			printf("UNMATCHED\n");
		}

		banner("test one two run");
		argv[0] = "test";
		argv[1] = "one";
		argv[2] = "two";
		argv[3] = "run";
		argv[4] = NULL;
		node = cli_find_command(4, &argv[0], &matches[0], &num_matches);	
		printf("test one two run");
		if (node != NULL) {
			printf("matches node '%s', level %d\n", node->keyword, node->level);
		} else {
			printf("UNMATCHED\n");
		}

		banner("tunnel profile create");
		argv[0] = "tunnel";
		argv[1] = "profile";
		argv[2] = "create";
		argv[3] = NULL;
		node = cli_find_command(3, &argv[0], &matches[0], &num_matches);	
		printf("tunnel profile create ");
		if (node != NULL) {
			printf("matches node '%s', level %d\n", node->keyword, node->level);
		} else {
			printf("UNMATCHED\n");
		}

		banner("system modify trace_flags=1 more_flags=2 debug");
		argv[0] = "system";
		argv[1] = "modify";
		argv[2] = "trace_flags";
		argv[3] = "1";
		argv[4] = "more_flags";
		argv[5] = "2";
		argv[6] = "debug";
		argv[7] = NULL;
		node = cli_find_command(7, &argv[0], &matches[0], &num_matches);
		if (node != NULL) {
			printf("matches node '%s', level %d\n", node->keyword, node->level);
		} else {
			printf("UNMATCHED\n");
		}

		// exit(0);
		cli_run();
	}

	return 0;
}
