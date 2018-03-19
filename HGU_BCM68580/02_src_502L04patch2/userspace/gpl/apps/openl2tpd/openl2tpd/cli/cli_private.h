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

#ifndef CLI_PRIVATE_H
#define CLI_PRIVATE_H

#define CLI_MAX_ARGC		200
#define CLI_MAX_KEYWORDS	100


extern int cli_rl_buffer_to_argv(char *start, char *end, char *argv[], int argv_len);
extern int cli_rl_set_prompt(const char *prompt);
extern int cli_rl_stuff_char(int ch);
extern void cli_rl_show_help(void);
extern void cli_rl_wait_then_execute_command(void);
extern int cli_rl_write_history_file(char *filename, int max_size);
extern int cli_rl_read_history_file(char *filename);
extern void cli_rl_clear_history(void);
extern void cli_rl_init(const char *app_name);
extern void cli_rl_cleanup(void);


#endif /* CLI_PRIVATE_H */
