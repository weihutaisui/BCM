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
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "cli_api.h"
#include "cli_private.h"

static struct cli_node **cli_rl_nodes;
static int cli_rl_nodes_num;
static int cli_rl_display_help;

static char *cli_rl_prompt = NULL;
static int cli_rl_history_lines_this_session = 0;
static int cli_rl_history_lines_in_file = 0;

/* Generator function for command completion.  STATE lets us
   know whether to start from scratch; without any state
   (i.e. STATE == 0), then we start at the top of the list. 
 */
static char *cli_rl_next_completion_word(const char *text, int state)
{
	static int list_index;
	char *name;
	struct cli_node *node;

	/* If this is a new word to complete, initialize now.  This
	   includes initializing the index variable to 0. 
	 */
	if (!state) {
		list_index = 0;
	}

	/* Return the next name which partially matches from the
	   prebuilt completion list. 
	 */
	if ((cli_rl_nodes == NULL) || (list_index == cli_rl_nodes_num)) {
		name = NULL;
		goto out;
	}
	node = cli_rl_nodes[list_index++];
	name = strdup(node->keyword);
	if (name == NULL) {
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}

out:
	return name;
}

static char *cli_rl_completion_entry_function(const char *text, int state)
{
	return NULL;
}

static int cli_rl_is_quote_char(char ch)
{
	const char *wbc = rl_completer_quote_characters;

	while (*wbc) {
		if (ch == *wbc) {
			return 1;
		}
		wbc++;
	}
	return 0;
}

static inline int skip_single_quoted (char *string, size_t slen, int sind)
{
	register int c;

	c = sind;
	while (string[c] && string[c] != '\'')
		c++;

	if (string[c])
		c++;
	return c;
}

static int skip_double_quoted (char *string, size_t slen, int sind)
{
	int c, i;
	int pass_next, backquote;

	pass_next = backquote = 0;
	i = sind;
	while ((c = string[i])) {
		if (pass_next) {
			pass_next = 0;
			i++;
			continue;
		}
		else if (c == '\\') {
			pass_next++;
			i++;
			continue;
		}
		else if (backquote) {
			if (c == '`')
				backquote = 0;
			i++;
			continue;
		}
		else if (c == '`') {
			backquote++;
			i++;
			continue;
		} else if (c != '"') {
			i++;
			continue;
		}
		else
			break;
	}

	if (c)
		i++;

	return (i);
}


/* Return 1 if the portion of STRING ending at EINDEX is quoted (there
   is an unclosed quoted string), or if the character at EINDEX is
   quoted by a backslash. The characters that this recognizes need to
   be the same as the contents of rl_completer_quote_characters. 
*/
static int cli_rl_char_is_quoted(char *string, int eindex)
{
	int i, pass_next, c;
	size_t slen;

	slen = strlen (string);
	i = pass_next = 0;
	while (i <= eindex) {
		c = string[i];

		if (pass_next) { 
			pass_next = 0;
			if (i >= eindex)	/* XXX was if (i >= eindex - 1) */
				return 1;
			i++;
			continue;
		}
		else if (c == '\\') {
			pass_next = 1;
			i++;
			continue;
		}
		else if (c == '\'' || c == '"') {
			i = (c == '\'') ? skip_single_quoted (string, slen, ++i)
				: skip_double_quoted (string, slen, ++i);
			if (i > eindex)
				return 1;
			/* no increment, the skip_xxx functions go one past end */
		}
		else
			i++;
	}

	return 0;
}

int cli_rl_buffer_to_argv(char *start, char *end, char *argv[], int argv_len)
{
	char *c;
	char buf[100];
	int index = 0;
	int argc = 0;
	int arg;
	int in_break = 0;
	int close_char = '\0';

	if (start == NULL) {
		goto out;
	}
	if ((start[0] == ' ') || (start[0] == '\t')) {
		in_break = 1;
	}
	if (start[0] == '#') {
		goto out;
	}

	for (c = start; c < end; c++) {
		if (cli_rl_is_quote_char(*c)) {
			if (close_char == *c) {
				close_char = '\0';
			} else {
				close_char = *c;
			}
		}
		if (((*c == ' ') || (*c == '\t')) && (close_char == '\0')) {
			if (!in_break) {
				buf[index] = '\0';
				argv[argc++] = strdup(buf);
				index = 0;
			}
			in_break = 1;
		} else {
			buf[index++] = *c;
			in_break = 0;
			if (index == (sizeof(buf) - 1)) {
				break;
			}
		}
	}
	buf[index] = '\0';
	argv[argc++] = strdup(buf);

out:
	if (close_char != '\0') {
		for (arg = 0; arg < argc; arg++) {
			free(argv[arg]);
		}
		return -EAGAIN;
	}

	return argc;
}

static void cli_rl_display_wrapped_text(int left_margin, int right_margin, const char *text1, const char *text)
{
	int index;
	int text_len = strlen(text);
	char text_buf[120];
	int word_start;
	int pos;
	int in_ws;
	int i;

	if (left_margin == 0) {
		left_margin = 3;
	}
	if (right_margin == 0) {
		right_margin = 78;;
	}

	word_start = 0;
	in_ws = 1;

	/* First copy the text heading to the buffer and add a "-", accounting for
	 * the specified left margin.
	 */
	strncpy(&text_buf[0], text1, left_margin - 3);
	for (pos = strlen(text1); pos < left_margin - 3; pos++) {
		text_buf[pos] = ' ';
	}
	text_buf[pos++] = ' ';
	text_buf[pos++] = '-';
	text_buf[pos++] = ' ';

	/* Now loop over each character in the caller's text string, remembering 
	 * where we saw the first character of a word when the text buffer was
	 * empty.
	 */
	word_start = 0;
	for (index = 0; index < text_len; index++) {
		if (text[index] == '\n') {
			if ((pos + (index - word_start)) >= right_margin) {
				text_buf[pos++] = '\0';
				puts(text_buf);
				for (pos = 0; pos < left_margin; pos++) {
					text_buf[pos] = ' ';
				}
			}
			for (i = 0; i < (index - word_start); i++) {
				text_buf[pos++] = text[word_start + i];
			}
			text_buf[pos++] = '\0';
			puts(text_buf);
			for (pos = 0; pos < left_margin; pos++) {
				text_buf[pos] = ' ';
			}
			word_start = index + 1;
			in_ws = 1;
			continue;
		} else if (whitespace(text[index])) {
			if (!in_ws) {
				if ((pos + (index - word_start)) >= right_margin) {
					text_buf[pos++] = '\0';
					puts(text_buf);
					for (pos = 0; pos < left_margin; pos++) {
						text_buf[pos] = ' ';
					}
				}
				for (i = 0; i < (index - word_start); i++) {
					if (text[word_start + i] == '\n') {
						continue;
					}
					text_buf[pos++] = text[word_start + i];
				}
			}
			in_ws = 1;
			text_buf[pos++] = text[index];
		} else {
			if (in_ws) {
				word_start = index;
			}
			in_ws = 0;
		}
	}
	for (i = 0; i < (index - word_start); i++) {
		text_buf[pos++] = text[word_start + i];
	}
	text_buf[pos++] = '\0';
	puts(text_buf);
}

static void cli_rl_display_formatted_help(struct cli_node *nodes[], int num_nodes)
{
	int node;
	int max_field_len = 0;
	int field_len;
	int descr_column;

	for (node = 0; node < num_nodes; node++) {
		field_len = strlen(nodes[node]->keyword);
		if (field_len > max_field_len) {
			max_field_len = field_len;
		}
	}

	descr_column = max_field_len + strlen(" - ");

	for (node = 0; node < num_nodes; node++) {
		cli_rl_display_wrapped_text(descr_column, 0, nodes[node]->keyword, 
					    nodes[node]->help ? nodes[node]->help : "");
	}
}

static char **cli_rl_attempted_completion_function(const char *text, int start, int end)
{
	char **matches = NULL;
	struct cli_node *nodes[CLI_MAX_KEYWORDS];
	int num_nodes = CLI_MAX_KEYWORDS;
	char *argv[CLI_MAX_ARGC];
	int argc;
	struct cli_node *node;
	int arg;

	argc = cli_rl_buffer_to_argv(&rl_line_buffer[0], &rl_line_buffer[end], &argv[0], CLI_MAX_ARGC);
	if (argc > 0) {
		node = cli_find_command(argc, &argv[0], &nodes[0], &num_nodes);

		if (!cli_rl_display_help) {
			if (node != NULL) {
				cli_rl_nodes = &nodes[0];
				cli_rl_nodes_num = num_nodes;
			} else {
				cli_rl_nodes = NULL;
				cli_rl_nodes_num = 0;
			}

			matches = rl_completion_matches(text, cli_rl_next_completion_word);

			if (matches) {
				if (matches[1] == NULL) {
					if (node->type == CLI_NODE_TYPE_ARG) {
						rl_stuff_char((node->arg->flags & CLI_ARG_FLAG_NO_VALUE) ? ' ' : '=');
					} else {
						rl_stuff_char(' ');
					}
				}
			}
		} else {
			puts("");
			if (num_nodes == 0) {
				if (node != NULL) {
					cli_rl_display_formatted_help(&node, 1);
				}
			} else {
				cli_rl_display_formatted_help(&nodes[0], num_nodes);
			}
			rl_forced_update_display();
		}

		for (arg = 0; arg < argc; arg++) {
			free(argv[arg]);
		}
	}

	return matches;
}

/* Strip whitespace from the start and end of STRING.  Return a pointer
   into STRING. */
static char *stripwhite(char *string)
{
	char *s, *t;
     
	for (s = string; whitespace(*s); s++)
		;
     
	if (*s == 0)
		return s;
     
	t = s + strlen(s) - 1;
	while ((t > s) && whitespace(*t))
		t--;
	*++t = '\0';
     
	return s;
}

static void cli_rl_execute_line(char *str)
{
	char *argv[CLI_MAX_ARGC];
	int argc;

	if (str[0] == '#') {
		return;
	}

	argc = cli_rl_buffer_to_argv(str, str + strlen(str), &argv[0], CLI_MAX_ARGC);
	if (argc > 0) {
		(void) cli_execute(argc, &argv[0]);
		rl_on_new_line();
	}
}

/* Read a string and execute it */
void cli_rl_wait_then_execute_command(void)
{
	char *line;
	char *str;

	/* Get a line from the user. */
	line = readline(cli_rl_prompt);
	if (line == NULL) {
		goto out;
	}
     
	/* Remove leading and trailing whitespace from the line.
	   Then, if there is anything left, add it to the history list
	   and execute it. */
	str = stripwhite(line);
     
	if (*str) {
		add_history(str);
		cli_rl_history_lines_this_session++;
		cli_rl_execute_line(str);
	}

out:     
	free(line);
}

int cli_rl_stuff_char(int ch)
{
	return rl_stuff_char(ch);
}

static int cli_rl_describe(int x, int y)
{
	cli_rl_display_help = 1;
	rl_complete_internal('?');
	rl_on_new_line();
	cli_rl_display_help = 0;
	return 0;
}

void cli_rl_show_help(void)
{
	if (rl_end != 0) {
		rl_delete_text(0, rl_end);
	}
	cli_rl_describe(0, 0);
}

/* Set the prompt. Take care not to free the current prompt string
 * while it is still being referenced by cli_rl_prompt.
 */
int cli_rl_set_prompt(const char *prompt)
{
	int result = 0;
	char *new_prompt = malloc(strlen(prompt) + 3);
	char *prev_prompt;

	if (new_prompt == NULL) {
		result = -ENOMEM;
		goto out;
	}

	sprintf(new_prompt, "%s> ", prompt);
	prev_prompt = cli_rl_prompt;
	cli_rl_prompt = new_prompt;
	if (prev_prompt != NULL) {
		free(prev_prompt);
	}
	rl_reset_line_state();
	rl_on_new_line();
	rl_clear_message();
	rl_forced_update_display();
	rl_message("\n");

out:
	return result;
}

/*****************************************************************************
 * History file manipulation
 *****************************************************************************/

int cli_rl_write_history_file(char *fname, int max_size)
{
	int result = 0;
	struct stat buf;
	char *filename;

	if (fname && *fname) {
		filename = tilde_expand_word(fname);

		/* If the file doesn't exist, then create it. */
		if (stat(filename, &buf) == -1) {
			int file;
			file = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			if (file != -1) {
				close(file);
			}
		}

		/* Now actually append the lines if the history hasn't been
		   stifled.  If the history has been stifled, rewrite the
		   history file. */
		using_history();
		if (cli_rl_history_lines_this_session <= where_history()) {
			result = append_history(cli_rl_history_lines_this_session, filename);
			cli_rl_history_lines_in_file += cli_rl_history_lines_this_session;
		} else {
			result = write_history(filename);
			cli_rl_history_lines_in_file = cli_rl_history_lines_this_session;
		}
		cli_rl_history_lines_this_session = 0;

		if ((filename != NULL) && (max_size > 0)) {
			history_truncate_file (filename, max_size);
			if (max_size <= cli_rl_history_lines_in_file) {
				cli_rl_history_lines_in_file = max_size;
			}
		}
	}

	return result;
}

int cli_rl_read_history_file(char *fname)
{
	int result = 0;
	char *filename;

	if (fname && *fname) {
		filename = tilde_expand_word(fname);
		result = read_history(filename);
	}
	using_history();
	cli_rl_history_lines_in_file = where_history();

	return result;
}

void cli_rl_clear_history(void)
{
	clear_history();
	stifle_history(0);
}

/*****************************************************************************
 * Init and cleanup
 *****************************************************************************/

static void cli_rl_handle_signal(int sig)
{
	switch (sig) {
	case SIGINT:
		rl_clear_pending_input();
		rl_reset_line_state();
		rl_crlf();
		rl_on_new_line();
		if (rl_end != 0) {
			rl_delete_text(0, rl_end);
		}
		rl_forced_update_display();
		break;
	default:
		break;
	}
}

static void cli_rl_uninstall_signal_handlers(void)
{
	(void) signal(SIGALRM, SIG_DFL);
	(void) signal(SIGTSTP, SIG_DFL);
	(void) signal(SIGTTIN, SIG_DFL);
	(void) signal(SIGTTOU, SIG_DFL);
	(void) signal(SIGINT, SIG_DFL);

	rl_catch_signals = 1;
	rl_free_line_state();
	rl_cleanup_after_signal();
}

static int cli_rl_install_signal_handlers(void)
{
	__sighandler_t handler;

	rl_catch_signals = 0;
	rl_clear_signals();

	handler = signal(SIGALRM, SIG_IGN);
	if (handler == SIG_ERR) {
		goto err;
	}
	handler = signal(SIGTSTP, SIG_IGN);
	if (handler == SIG_ERR) {
		goto err;
	}
	handler = signal(SIGTTIN, SIG_IGN);
	if (handler == SIG_ERR) {
		goto err;
	}
	handler = signal(SIGTTOU, SIG_IGN);
	if (handler == SIG_ERR) {
		goto err;
	}
	handler = signal(SIGINT, cli_rl_handle_signal);
	if (handler == SIG_ERR) {
		goto err;
	}

	return 0;

err:
	fprintf(stderr, "Failed to install signal handlers\n");

	return -EFAULT;
}

static int cli_rl_startup_hook(void)
{
	/* We use long command lines, so make sure the line buffer is large enough */
	rl_extend_line_buffer(4000);

	cli_rl_install_signal_handlers();

	return 0;
}

void cli_rl_init(const char *app_name)
{
	/* Allow conditional parsing of the ~/.inputrc file. */
	rl_readline_name = app_name;
	rl_instream = stdin;
	rl_outstream = stderr;

	/* Readline related settings. */
	rl_startup_hook = cli_rl_startup_hook;
	rl_bind_key ('?', cli_rl_describe);
	rl_completion_entry_function = cli_rl_completion_entry_function;
	rl_attempted_completion_function = cli_rl_attempted_completion_function;

	/* Don't append space after completion. It will be appended in
	 * the new_completion() function explicitly
	 */
	rl_completion_append_character = '\0';

	rl_completer_quote_characters = "'\"";
	rl_char_is_quoted_p = cli_rl_char_is_quoted;

	cli_rl_set_prompt(app_name);

	cli_rl_nodes = NULL;
	cli_rl_nodes_num = 0;

	cli_rl_display_help = 0;
}

void cli_rl_cleanup(void)
{
	cli_rl_uninstall_signal_handlers();
}
