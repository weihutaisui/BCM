/*
 * Broadcom Dongle Host Driver (DHD), kernel mode gdb server
 * This file contains routines that are specific to Linux
 *
 * Copyright 2017 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: dhd_linux.c 674170 2016-12-07 05:24:28Z $
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/file.h>
#include <linux/socket.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#include <typedefs.h>
#include <osl.h>
#include <debugger.h>

struct listener_params {
	int (*main)(void *handle);
	int port;
	struct task_struct *task;
};

static int sock_send(struct socket *sock, const char *buf, const size_t length, unsigned long flags) {
	struct iovec iov;
	struct msghdr msg; // a linux kernel type defined in <socket.h>
	int size;
	int written = 0;
	int remaining = length;
	mm_segment_t old_mm;

	memset(&msg, 0, sizeof(msg));

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	old_mm = get_fs();
	set_fs(KERNEL_DS);

	for (;;) {
		msg.msg_iov->iov_len = remaining;
		msg.msg_iov->iov_base = (char *) buf + written;

		size = sock_sendmsg(sock, &msg, remaining);

		if (size == -ERESTARTSYS)
			continue;

		if (!(flags & MSG_DONTWAIT) && (size == -EAGAIN))
			continue;

		if (size > 0) {
			written += size;
			remaining -= size;
			if (remaining)
				continue;
		}

		break;
	}

	set_fs(old_mm);

	return written ? written : size;
}

/**
 * This function may block.
 */
static int sock_read(struct socket *sock, char *buf, int buflen) {
	struct msghdr msg;
	struct iovec iov;
	int size;
	mm_segment_t old_mm;

	memset(&msg, 0, sizeof(msg));

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_iov->iov_base = buf;
	msg.msg_iov->iov_len = buflen;

	old_mm = get_fs();
	set_fs(KERNEL_DS);

	for (;;) {
		size = sock_recvmsg(sock, &msg, buflen, 0);
		if (size != -EAGAIN && size != -ERESTARTSYS) {
			break;
		}
	}

	set_fs(old_mm);

	return size;
}

/**
 * A kernel thread that listens on a certain server port. When a remote party initiates a TCP
 * connection, this thread will spawn a worker thread to service that particular TCP connection.
 */
static int server_socket_listener(void *vp) {
	int rval = -1;
	struct socket *new_socket;
	struct sockaddr_in srvaddr;
	struct socket *servsock = NULL;
	struct listener_params *lp = (struct listener_params *)vp;
	struct timeval tv = {0};
	bool received_stop_signal = FALSE;

	current->flags |= PF_NOFREEZE;
	allow_signal(SIGKILL | SIGSTOP);

	rval = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &servsock);
	memset(&srvaddr,0, sizeof(srvaddr));
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(lp->port);
	srvaddr.sin_addr.s_addr= htonl(INADDR_ANY);;
	rval = servsock->ops->bind(servsock, (struct sockaddr *)&srvaddr, sizeof (srvaddr));
	if (rval < 0)
		goto out;

	tv.tv_sec = 0; /* 1s Timeout */
	tv.tv_usec = 100000;

	kernel_setsockopt(servsock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
	kernel_setsockopt(servsock, SOL_TCP, TCP_NODELAY,  (char*)&rval, sizeof(rval));

	for (;;) {
		rval = servsock->ops->listen(servsock, 10); /* non blocking call, backlog=10 */
		if (rval < 0) {
			goto out;
		}
		rval = kernel_accept(servsock, &new_socket, 0); /* blocking call */
		if (kthread_should_stop()) {
			received_stop_signal = TRUE;
			goto out;
		}
		if (rval == -EAGAIN) {
			continue; /* timeout */
		}
		if (rval < 0) {
			goto out;
		}

		kthread_run(lp->main, (void *) new_socket, "debugger_ui");
	}

out:
	sock_release(servsock);

	if (received_stop_signal == FALSE) {
		/* for received_stop_signal == TRUE, kthread_stop() will handle the exit */
		do_exit(rval);
	}

	lp->task = NULL;

	return rval;
}

/**
 * A per-TCP connection kernel thread that is spawned when a remote party initiates a TCP
 * connection. The thread terminates when the TCP connection terminates, usually when the user
 * quits the gdb client.
 */
static int debugger_gdb_thread(void *handle) {
	struct socket *new_socket = handle;

	debugger_gdb_main(new_socket);
	sock_release(new_socket);

	return 0;
}

static void socket_fputc(char c, void *handle) {
	sock_send((struct socket *)handle, &c, 1, MSG_DONTWAIT);
}

/**
 * This function may block
 */
static int socket_fgetc(void *handle) {
	int len;
	char c;

	len = sock_read((struct socket *)handle, &c, 1);

	if (len)
		return c;

	return 0;
}

#ifndef DHD_DSCOPE
extern int debugger_ui_main(void *handle);
#endif /* DHD_DSCOPE */

struct listener_params listeners[] = {
#ifndef DHD_DSCOPE
	{ debugger_ui_main, 24, NULL },
#endif /* DHD_DSCOPE */
	{ debugger_gdb_thread, 2424, NULL },
	{ NULL }
};

/* Public functions */

void *g_bus_handle = NULL;

static struct dhd_gdb_bus_ops_s  *g_bus_ops; /**< contains functions provided by DHD bus layer */
static uint32 g_enum_base;

/**
 * First function called by the 'user' of this debugger module.
 *
 * @param bus_handle    Opaque handle, to be used when calling back into DHD bus layer.
 * @param bus_ops       Functions for reading and writing the chip backplane.
 * @param gdb_svr_port  TCP port to use for DHD_DSCOPE gdb server.
 * @param enum_base     Start of hardware register space on the backplane, usually 0x18000000.
 */
void debugger_init(void *bus_handle, struct dhd_gdb_bus_ops_s *bus_ops, int gdb_svr_port,
	uint32 enum_base) {
	struct listener_params *lp;

	/* at a maximum, only one wireless interface at a time can be debugged */
	debugger_close();

	g_bus_handle = bus_handle;
	g_bus_ops = bus_ops;
	g_enum_base = enum_base;

	for (lp = listeners; lp->main; lp++) {
#ifdef DHD_DSCOPE
		if (lp->main == debugger_gdb_thread) {
			lp->port = gdb_svr_port;
		}
#endif /* DHD_DSCOPE */
		lp->task = kthread_run( server_socket_listener, (void *) lp, "debugger_listener");
	}
}

/**
 * Function called by the 'user' of this debugger module
 */
void debugger_close() {
	struct listener_params *lp;

	if (g_bus_handle != NULL) {
		for (lp = listeners; lp->main; lp++) {
			if (lp->task != NULL) {
				kthread_stop(lp->task);
			}
		}
		g_bus_handle = NULL;
	}
}

void dbg_fputc(char c, void *handle) {
	sock_send((struct socket *)handle, &c, 1, MSG_DONTWAIT);
}

/**
 * This function may block
 *
 * @return  character read
 */
int dbg_fgetc(void *handle) {
	int len;
	char c;

	len = sock_read((struct socket *)handle, &c, 1);

	if (len)
		return c;

	return 0;
}

char *dbg_fgets(char *s, void *handle)
{
	int ch;
	char *p;

	p = s;
	while ((ch = socket_fgetc(handle)) != '\n')
		*p++ = ch;

	*p = 0;
	return s;
}

void dbg_fputs(char *s, void *handle) {
	while (*s)
		socket_fputc(*s++, handle);
}

void dbg_mdelay(int d) {
	mdelay(d);
}

void dbg_fclose(void *handle) {
	sock_release((struct socket *) handle);
}

/** Reads chip backplane */
unsigned short
dbg_bp_read16(uint32_t addr) {
	if (!g_bus_handle || addr >= g_enum_base)
		return 0xFFFF;

	return g_bus_ops->read_u16(g_bus_handle, addr);
}

/** Reads chip backplane */
uint32_t
dbg_bp_read32(uint32_t addr) {
	if (!g_bus_handle || addr >= g_enum_base)
		return 0xFFFFFFFF;

	return g_bus_ops->read_u32(g_bus_handle, addr);
}

/** Writes chip backplane */
void
dbg_bp_write32(uint32_t addr, uint32_t val) {
	if (!g_bus_handle || addr >= g_enum_base)
		return;

	return g_bus_ops->write_u32(g_bus_handle, addr, val);
}
