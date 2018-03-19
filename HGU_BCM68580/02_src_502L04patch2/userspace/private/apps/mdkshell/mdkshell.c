/******************************************************************************
 * <:copyright-BRCM:2012:proprietary:standard
 * 
 *    Copyright (c) 2012 Broadcom 
 *    All Rights Reserved
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
 ******************************************************************************
 *
 * Linux User mode CDK/BMD Application
 *
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

/* CDK Package Headers */
#include <cdk/cdk_readline.h>
#include <cdk/cdk_shell.h>

#include <mdkshell_ipc.h>

#define TRUE 1
#define FALSE 0
static int current_unit = 0;
int mdksh_sock_conn = -1;
static char sock_buffer[1024];

/*******************************************************************************
 *
 * Terminal support
 *
 * The functions below are used by the readline terminal
 * interface, and should work on most POSIX systems.
 *
 ******************************************************************************/
#ifdef SYS_HAS_TTY
#include <termios.h>

/*
 * This function ensures that the TTY returns typed characters
 * immediately and has character echo disabled.
 */
static int
_tty_set(int reset)
{
    static struct termios old, new;
    static int tty_initialized;

    if (reset) {
        /* Restore TTY settings */
        if (tty_initialized) {
            tcsetattr(0, TCSADRAIN, &old);
        }
        return 0;
    }

    if (tcgetattr(0, &old) < 0) {
        perror("tcgetattr");
    } else {
        /* Save terminal settings */
        new = old;
        /* Disable echo and buffering */
        new.c_lflag &= ~(ECHO | ICANON | ISIG);
        new.c_iflag &= ~(ISTRIP | INPCK);
        new.c_cc[VMIN] = 1;
        new.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSADRAIN, &new) < 0) {
            perror("tcsetattr");
        } else {
            tty_initialized = 1;
        }
    }
    return 0;
}

/* Read character from TTY */
static int
_tty_getchar(void)
{
    return getchar();
}

/* Send character to TTY */
static int
_tty_putchar(int ch)
{
    return putchar(ch);
}
#endif

int
_usleep(uint32_t usecs)
{
    return usleep(usecs);
}

/*
 * Function:
 *      _readline
 * Purpose:
 *      Retrieve an input line from the user.
 * Returns:
 *      Pointer to the readline buffer
 * Notes:
 *
 *      If SYS_HAS_TTY is defined:
 *          The built-in readline library is used.
 *
 *      If SYS_HAS_TTY is not defined:
 *          A normal read on stdio is used.
 *
 */

char *
_readline(const char *prompt, char *buf, int max)
{
#ifdef SYS_HAS_TTY
    _tty_set(0);
    cdk_readline(_tty_getchar, _tty_putchar, prompt, buf, max);
    _tty_set(1);
#else
    int len;

    write(0, prompt, strlen(prompt));
    if ((len = read(0, buf, max)) <= 0) {
        buf[0] = 0;
    } else {
        buf[len-1] = 0;
    }
#endif
    return buf;
}
static int
mdksh_shell_custom(const char *sh_prompt, cdk_shell_gets_t sh_gets)
{
    static char line[CDK_CONFIG_SHELL_IO_MAX_LINE];
    static char prompt[CDK_CONFIG_SHELL_IO_MAX_PROMPT];
    int len;    

    CDK_ASSERT(sh_prompt);
    CDK_ASSERT(sh_gets);

    // Effectively flushing what is there

    len = mdksh_read_from_socket(mdksh_sock_conn, sock_buffer, 1024);

    /* Generate the prompt with unit suffix */
    CDK_SPRINTF(prompt, "%s%d> ", sh_prompt, current_unit);
    while (TRUE) {

        /* Read a command */
        if (sh_gets(prompt, line, CDK_CONFIG_SHELL_IO_MAX_LINE) == NULL) {
            break;
        }

       // send line buffer to peer mdkshell running as pthread of 'swmdk'
       if (mdksh_write_to_socket(mdksh_sock_conn, line, strlen(line) + 1) < 0) {
           fprintf(stderr, "%s Error \n", __FUNCTION__); 
           return -1;     
       }
       while (TRUE) {
          len = mdksh_read_from_socket(mdksh_sock_conn, sock_buffer, 1024);
          if (len == 0) {
              /*
               * CDK_SHELL_CMD_EXIT is returned by command dispatcher
               * at the other end.
               * When shell exit command is detected, peer closes the 
               * socket and socket read returns 0. This is mdkshell exit point.
               */
              return 0;
          } else if (len > 0) {
              if (!strncmp(PROMPT_STR, sock_buffer, 4)) {
                  // This is end of command output
                  strncpy(prompt, sock_buffer, CDK_CONFIG_SHELL_IO_MAX_PROMPT);
                  break;
              } else {
                  // dump on to tty
                  CDK_PRINTF(sock_buffer);
              }
          } else {
              return -1; // socket read error.
          }
       } // cmd output loop
    } // while next command
    return 0;
}

/* TBD: to use the defines in config.h files.  */
#define MAX_SWITCH_PORTS 8
int thread_lock = 1;
int main(int argc __attribute__ ((unused)), char* argv[] __attribute__ ((unused)))
{

    if ((mdksh_sock_conn = mdksh_connect_to_socket(MDKSH_SOCK_NAME)) < 0) {
        return -1;
    }

    mdksh_shell_custom(PROMPT_STR, _readline);
    /* shell is exiting */

    return 0;
}
