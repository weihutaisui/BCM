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

#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>

typedef enum {
	CLI_NODE_TYPE_END,
	CLI_NODE_TYPE_KEYWORD,
	CLI_NODE_TYPE_COMMAND,
	CLI_NODE_TYPE_ARG
} cli_node_type_t;

struct cli_node;
struct cli_arg;

typedef int cli_bool_t;

typedef int (*cli_action_fn_t)(struct cli_node *node, int argc, char *argv[], int *arg_num);
typedef int (*cli_arg_parse_fn_t)(struct cli_node *arg, const char *val, void *result);

struct cli_arg {
	int			id;
	int			flags;
#define CLI_ARG_FLAG_MANDATORY		(1 << 0)
#define CLI_ARG_FLAG_NO_VALUE		(1 << 1)
	cli_arg_parse_fn_t	parser;
	const char		*help;
};

struct cli_node {
	int			level;
	cli_node_type_t		type;
	const char		*keyword;
	const char		*help;
	cli_action_fn_t		action;		/* for command nodes only */
	struct cli_arg		*arg;		/* for argument nodes only */
	struct cli_node		*first;		/* ptr back to first node of this list */
	struct cli_node		*next;		/* next node at same level */
	struct cli_node		*child;		/* node below this node */
	struct cli_node		*parent; 	/* ptr back to parent node, or NULL if root node */
};

struct cli_node_entry {
	int			level;
	cli_node_type_t		type;
	const char		*keyword;
	const char		*help;
	cli_action_fn_t		action;		/* for command nodes only */
	struct cli_arg_entry	*args; 		/* for command nodes only */
};

struct cli_arg_entry {
	const char		*name;
	struct cli_arg		data;
};

extern int cli_add_commands(struct cli_node_entry *table);
extern int cli_remove_commands(struct cli_node_entry *table);
extern void cli_dump_commands(void);
extern struct cli_node *cli_find_command(int argc, char *argv[], struct cli_node *matches[], int *num_matches);
extern int cli_find_args(int argc, char *argv[], struct cli_node *cmd, struct cli_node *args[], char *values[], int *num_args);
extern void cli_run(void);
extern int cli_execute(int argc, char *argv[]);
extern int cli_stuff_args(int argc, char *argv[]);
extern void cli_redirect_io(FILE *in_stream, FILE *out_stream);
extern int cli_set_prompt(const char *prompt);
extern int cli_write_history_file(char *filename, int max_size);
extern int cli_read_history_file(char *filename);
extern void cli_clear_history(void);
extern void cli_show_help(void);

extern int cli_arg_parse_string(struct cli_node *arg, const char *val, void *result);
extern int cli_arg_parse_int32(struct cli_node *arg, const char *val, void *result);
extern int cli_arg_parse_int16(struct cli_node *arg, const char *val, void *result);
extern int cli_arg_parse_int8(struct cli_node *arg, const char *val, void *result);
extern int cli_arg_parse_uint32(struct cli_node *arg, const char *val, void *result);
extern int cli_arg_parse_uint16(struct cli_node *arg, const char *val, void *result);
extern int cli_arg_parse_uint8(struct cli_node *arg, const char *val, void *result);
extern int cli_arg_parse_bool(struct cli_node *arg, const char *val, void *result);
extern int cli_arg_parse_macaddr(struct cli_node *arg, const char *val, void *result);
extern int cli_arg_parse_ipaddr(struct cli_node *arg, const char *val, void *result);
extern int cli_arg_parse_hex(struct cli_node *arg, const char *val, void *result);

extern int cli_init(const char *prompt);
extern void cli_cleanup(void);
