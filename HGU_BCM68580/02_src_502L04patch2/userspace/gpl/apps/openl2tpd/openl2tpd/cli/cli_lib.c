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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <asm/types.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "cli_api.h"
#include "cli_private.h"


static struct cli_node *cli_nodes;

static void cli_link_buddy_node(struct cli_node *node, struct cli_node *to)
{
	if (to == node) {
		if (node != cli_nodes) {
			printf("Trying to add first entry to cli_nodes twice\n");
			exit(1);
		}
	} else {
		node->next = to->next;
		to->next = node;
	}
}

static void cli_link_child_node(struct cli_node *node, struct cli_node *to)
{
	if (to->child != NULL) {
		printf("%s: to child list not empty!\n", __func__);
	}
	to->child = node;
}

static int cli_add_node(struct cli_node_entry *entry, struct cli_node **new_node)
{
	struct cli_node *node;
	int result = 0;
	
	if (entry->type == CLI_NODE_TYPE_END) {
		node = NULL;
		result = -EMLINK;
		goto out;
	}

	node = calloc(1, sizeof(*node));
	if (node == NULL) {
		result = -ENOMEM;
		goto out;
	}

	node->type = entry->type;
	node->keyword = entry->keyword;
	node->level = entry->level;
	node->next = NULL;
	node->child = NULL;
	node->first = NULL;
	node->parent = NULL;

	switch (node->type) {
	case CLI_NODE_TYPE_END:
	case CLI_NODE_TYPE_KEYWORD:
	case CLI_NODE_TYPE_COMMAND:
	case CLI_NODE_TYPE_ARG:
		node->help = entry->help;
		node->action = entry->action;
		break;
	default:
		result = -EINVAL;
	}

	*new_node = node;

out:
	return result;
}

int cli_add_commands(struct cli_node_entry *table)
{
	int result = 0;
	struct cli_node_entry *entry;
	struct cli_node *node;
	struct cli_node *child = NULL;
	struct cli_node *parent = NULL;
	int prev_level = 0;
	struct cli_node *first = NULL;

	for (entry = table; entry->type != CLI_NODE_TYPE_END; entry++) {
		result = cli_add_node(entry, &node);
		if (result < 0) {
			goto out;
		}
		if (cli_nodes == NULL) {
			cli_nodes = node;
		}

		if (node->level == 0) {
			first = cli_nodes;
		} else if (node->level < prev_level) {
			first = node;
		}
		if ((node->level == 0) || (node->level == prev_level)) {
			cli_link_buddy_node(node, first);
		} else if (node->level == (prev_level + 1)) {
			parent = child;
			cli_link_child_node(node, child);
			first = node;
		} else {
			result = -EINVAL;
			fprintf(stderr, "Command table error at node '%s', level %d\n", 
				node->keyword, node->level);
			goto out;
		}
		node->first = first;
		node->parent = parent;
		child = node;
		prev_level = node->level;

		{
			struct cli_arg_entry *arg;
			struct cli_node *arg_head = NULL;
			struct cli_node *prev_arg = NULL;

			for (arg = entry->args; arg != NULL; arg++) {
				struct cli_node_entry arg_entry;
				struct cli_node *arg_node;

				if (arg->name == NULL) {
					break;
				}
				arg_entry.level = node->level + 1;
				arg_entry.type = CLI_NODE_TYPE_ARG;
				arg_entry.keyword = arg->name;
				arg_entry.help = arg->data.help;
				arg_entry.action = NULL;
				result = cli_add_node(&arg_entry, &arg_node);
				if (result < 0) {
					goto out;
				}
				if (arg_head == NULL) {
					arg_head = arg_node;
					node->child = arg_node;
				}
				arg_node->arg = &arg->data;
				arg_node->first = arg_head;
				arg_node->parent = node;
				if (prev_arg != NULL) {
					cli_link_buddy_node(arg_node, prev_arg);
				}
				prev_arg = arg_node;
			}
		}
	}
out:
	return result;
}

int cli_remove_commands(struct cli_node_entry *table)
{
	return -EOPNOTSUPP;
}

void cli_run(void)
{
	for (;;) {
		cli_rl_wait_then_execute_command();
	}
}

int cli_stuff_args(int argc, char *argv[])
{
	int arg;
	int result = 0;

	for (arg = 0; arg < argc; arg++) {
		int pos;
		int end = strlen(argv[arg]);
		for (pos = 0; pos < end; pos++) {
			cli_rl_stuff_char(argv[arg][pos]);
		}
		result = cli_rl_stuff_char(' ');
		if (result < 0) {
			goto out;
		}
	}
	if (argc > 0) {
		result = cli_rl_stuff_char('\n');
	}

out:
	return result;
}

void cli_redirect_io(FILE *in_stream, FILE *out_stream)
{
	rl_instream = in_stream;
	rl_outstream = out_stream;
}

int cli_set_prompt(const char *prompt)
{
	return cli_rl_set_prompt(prompt);
}

void cli_show_help(void)
{
	cli_rl_show_help();
}

int cli_write_history_file(char *filename, int max_size)
{
	return cli_rl_write_history_file(filename, max_size);
}

int cli_read_history_file(char *filename)
{
	return cli_rl_read_history_file(filename);
}

void cli_clear_history(void)
{
	cli_rl_clear_history();
}

static int cli_walk_commands_1(struct cli_node *root,
			       int (*callback)(struct cli_node *node, void *arg),
			       void *arg)
{
	struct cli_node *node = root;
	int result = 0;

	for (node = root; node != NULL; node = node->next) {
		result = (*callback)(node, arg);
		if (result != 0) {
			goto out;
		}
		if (node->child != NULL) {
			/* recurse */
			cli_walk_commands_1(node->child, callback, arg);
		}
	}
out:
	return result;
}

int cli_walk_commands(int (*callback)(struct cli_node *node, void *arg),
		      void *arg)
{
	return cli_walk_commands_1(cli_nodes, callback, arg);
}

static int cli_dump_node(struct cli_node *node, void *arg)
{
	int result = 0;

	switch (node->type) {
	case CLI_NODE_TYPE_END:
		result = -ENOENT;
		break;
	case CLI_NODE_TYPE_KEYWORD:
		printf("[KEY/%d] + %s %p/%p\n", node->level, node->keyword, node, node->first);
		break;
	case CLI_NODE_TYPE_COMMAND:
		printf("[CMD/%d] x %s %p/%p\n", node->level, node->keyword, node, node->first);
		break;
	case CLI_NODE_TYPE_ARG:
		printf("[ARG/%d] = %s %p/%p\n", node->level, node->keyword, node, node->first);
		break;
	}

	return result;
}

static int cli_find_node(struct cli_node *node, void *arg)
{
	int result = -EINVAL;
        int max_len = strlen(arg);
	char *equals;

	switch (node->type) {
	case CLI_NODE_TYPE_END:
		result = -ENOENT;
		break;
	case CLI_NODE_TYPE_ARG:
		equals = index(arg, '=');
		if (equals != NULL) {
			max_len = equals - (char *) arg;
		}
	case CLI_NODE_TYPE_KEYWORD:
	case CLI_NODE_TYPE_COMMAND:
		if (strncasecmp(arg, node->keyword, max_len) == 0) {
			result = 1; /* non-zero => terminate walk */
		} else {
			result = 0;
		}
		break;
	}

	return result;
}

void cli_dump_commands(void)
{
	(void) cli_walk_commands(cli_dump_node, NULL);
}

static struct cli_node *cli_find_command_1(struct cli_node *root, char *keyword, 
					   struct cli_node *matches[], int *num_matches)
{
	int result = -EINVAL;
	int match = 0;
	struct cli_node *node;

	for (node = root; node != NULL; node = node->next) {
		result = cli_find_node(node, keyword);
		if (result < 0) {
			break;
		}
		if (result > 0) {
			matches[match++] = node;
			if (match == *num_matches) {
				break;
			}
		}
	}

	if (match > 0) {
		*num_matches = match;
		return matches[0];
	}

	return NULL;
}

struct cli_node *cli_find_command(int argc, char *argv[], struct cli_node *matches[], int *num_matches)
{
	struct cli_node *node = NULL;
	struct cli_node *child_node = cli_nodes;
	int match_count = 0;
	int arg;

	for (arg = 0; arg < argc; arg++) {
		if (child_node == NULL) {
			match_count = 0;
			node = NULL;
			break;
		}

		match_count = *num_matches;
		node = cli_find_command_1(child_node->first, argv[arg], matches, &match_count);
		if (node == NULL) {
			match_count = arg; /* tell where error occurred */
			break;
		}
		if (match_count > 1) {
			/* More than one keyword matches */
			break;
		}
		switch (node->type) {
		case CLI_NODE_TYPE_COMMAND:
		case CLI_NODE_TYPE_KEYWORD:
			child_node = node->child;
			break;
		case CLI_NODE_TYPE_ARG:
			break;
		case CLI_NODE_TYPE_END:
			break;
		}
	}

	*num_matches = match_count;

	return node;
}

int cli_find_args(int argc, char *argv[], struct cli_node *cmd, struct cli_node *args[], char *values[], int *num_args)
{
	struct cli_node *node = cmd;
	int arg;
	int arg_count = 0;
	int result = 0;
	char *arg_string;

	for (arg = 0; arg < argc; arg++) {
		arg_count = *num_args;
		node = cli_find_command_1(cmd->child->first, argv[arg], &args[arg], &arg_count);
		if (node == NULL) {
			result = -ENOENT;
			break;
		}
		if (arg_count > 1) {
			/* More than one keyword matches */
			break;
		}
		args[arg] = node;
		if ((node->arg->flags & CLI_ARG_FLAG_NO_VALUE) == 0) {
			arg_string = index(argv[arg], '=');
			if (arg_string == NULL) {
				result = -EINVAL;
				break;
			}
			if (arg_string[1] == '\0') {
				/* no arg value - only allowed for string args */
				if (node->arg->parser == cli_arg_parse_string) {
					values[arg] = '\0';
				} else {
					result = -EINVAL;
					break;
				}
			} else {
				values[arg] = &arg_string[1];
			}
		} else {
			values[arg] = NULL;
		}
	}

	*num_args = arg;
	return result;
}

int cli_execute(int arg_count, char *arg_values[])
{
	struct cli_node *node;
	struct cli_node *nodes[CLI_MAX_KEYWORDS];
	int num_nodes = CLI_MAX_KEYWORDS;
	int result = 0;
	int argc = arg_count;
	char *argv[CLI_MAX_ARGC];
	int arg;

	if (argc > 0) {
		if (argc == 1) {
			argc = cli_rl_buffer_to_argv(arg_values[0], arg_values[0] + strlen(arg_values[0]), &argv[0], CLI_MAX_ARGC);
		} else {
			for (arg = 0; arg < argc; arg++) {
				argv[arg] = arg_values[arg];
			}
			argv[argc] = NULL;
		}
		node = cli_find_command(argc, &argv[0], &nodes[0], &num_nodes);
		if (node != NULL) {
			int arg_num = 0;
			switch (node->type) {
			case CLI_NODE_TYPE_COMMAND:
				result = (*node->action)(node, 0, NULL, &arg_num);
				if (result < 0) {
					printf("Error at or near '%s'\n", argv[node->level + arg_num]);
				}
				break;
			case CLI_NODE_TYPE_KEYWORD:
				printf("Ambiguous command\n");
				break;
			case CLI_NODE_TYPE_ARG:
				result = (*node->parent->action)(node->parent, argc - node->parent->level - 1, &argv[node->parent->level + 1], &arg_num);
				if (result < 0) {
					printf("Error at or near '%s'\n", argv[node->parent->level + arg_num]);
				}
				break;
			case CLI_NODE_TYPE_END:
				break;
			}
		} else {
			printf("Error at or near '%s'\n", argv[num_nodes]);
		}
	} else if (argc < 0) {
		printf("Incomplete command\n");
	}

	return result;
}

/*****************************************************************************
 * Argument parsers
 *****************************************************************************/

int cli_arg_parse_string(struct cli_node *arg, const char *val, void *result)
{
	/* The caller should use val directly. Nothing to do here. */
	return 0;
}

int cli_arg_parse_int32(struct cli_node *arg, const char *val, void *result)
{
	int32_t *intval = result;
	long tmp;
	char *endp;
	int ret = 0;

	tmp = strtol(val, &endp, 0);
	if (*endp == '\0') {
		*intval = tmp;
	} else {
		ret = -EINVAL;
	}

	return ret;
}

int cli_arg_parse_int16(struct cli_node *arg, const char *val, void *result)
{
	int16_t *intval = result;
	unsigned long tmp;
	char *endp;
	int ret = 0;

	tmp = strtol(val, &endp, 0);
	if ((*endp == '\0') && (labs(tmp) < 32768)) {
		*intval = tmp;
	} else {
		ret = -EINVAL;
	}

	return ret;
}

int cli_arg_parse_int8(struct cli_node *arg, const char *val, void *result)
{
	int8_t *intval = result;
	unsigned long tmp;
	char *endp;
	int ret = 0;

	tmp = strtol(val, &endp, 0);
	if ((*endp == '\0') && (labs(tmp) < 128)) {
		*intval = tmp;
	} else {
		ret = -EINVAL;
	}

	return ret;
}

int cli_arg_parse_uint32(struct cli_node *arg, const char *val, void *result)
{
	uint32_t *intval = result;
	unsigned long tmp;
	char *endp;
	int ret = 0;

	tmp = strtoul(val, &endp, 0);
	if (*endp == '\0') {
		*intval = tmp;
	} else {
		ret = -EINVAL;
	}

	return ret;
}

int cli_arg_parse_uint16(struct cli_node *arg, const char *val, void *result)
{
	uint16_t *intval = result;
	unsigned long tmp;
	char *endp;
	int ret = 0;

	tmp = strtoul(val, &endp, 0);
	if ((*endp == '\0') && (labs(tmp) < 65536)) {
		*intval = tmp;
	} else {
		ret = -EINVAL;
	}

	return ret;
}

int cli_arg_parse_uint8(struct cli_node *arg, const char *val, void *result)
{
	uint8_t *intval = result;
	unsigned long tmp;
	char *endp;
	int ret = 0;

	tmp = strtoul(val, &endp, 0);
	if ((*endp == '\0') && (labs(tmp) < 255)) {
		*intval = tmp;
	} else {
		ret = -EINVAL;
	}

	return ret;
}

int cli_arg_parse_bool(struct cli_node *arg, const char *val, void *result)
{
	cli_bool_t *yes_no = result;

	if ((strcasecmp(val, "yes") == 0) ||
	    (strcasecmp(val, "enable") == 0) ||
	    (strcasecmp(val, "true") == 0) ||
	    (strcasecmp(val, "on") == 0) ||
	    (strcasecmp(val, "1") == 0)) {
		*yes_no = 1;
	} else if ((strcasecmp(val, "no") == 0) ||
		   (strcasecmp(val, "disable") == 0) ||
		   (strcasecmp(val, "false") == 0) ||
		   (strcasecmp(val, "off") == 0) ||
		   (strcasecmp(val, "0") == 0)) {
		*yes_no = 0;
	} else {
		return -EINVAL;
	}

	return 0;
}

int cli_arg_parse_macaddr(struct cli_node *arg, const char *val, void *result)
{
	unsigned char *mac = result;
	unsigned short tmp[6];
	int count;
	int ret = 0;
	int i;

	count = sscanf(val, "%02hx:%02hx:%02hx:%02hx:%02hx:%02hx", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]);
	if (count != 6) {
		ret = -EINVAL;
	}

	for (i = 0; i < 6; i++) {
		mac[i] = (unsigned char) (tmp[i] & 0xff);
	}

	return ret;
}

int cli_arg_parse_ipaddr(struct cli_node *arg, const char *val, void *result)
{
	struct in_addr *ip = result;
	int ret = 0;

	ret = inet_aton(val, ip);
	if (ret != 1) {
		ret = -EINVAL;
	} else {
		ret = 0;
	}

	return ret;
}

int cli_arg_parse_hex(struct cli_node *arg, const char *val, void *result)
{
	unsigned char *buf = result;
	int ret = 0;
	const char *pos = val;
	int slen;
	int count = 0;
	int tmpval;

	/* Drop leading 0x, if present */
	if (strncasecmp(val, "0x", 2) == 0) {
		val += 2;
	}

	slen = strlen(val);
	if (slen & 1) {
		return -EINVAL;
	}

	for (count = 0; count < slen / 2; count++) {
		ret = sscanf(pos, "%02x", &tmpval);
		if (ret != 1) {
			ret = -EINVAL;
			goto out;
		}
		*buf++ = tmpval;
		pos += 2;
	} 

	ret = 0;
out:
	return ret;
}

int cli_init(const char *app_name)
{
	cli_nodes = NULL;
	cli_rl_init(app_name);

	return 0;
}

void cli_cleanup(void)
{
}
