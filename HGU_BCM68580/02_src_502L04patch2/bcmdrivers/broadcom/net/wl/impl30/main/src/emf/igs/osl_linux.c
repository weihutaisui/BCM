/*
 * Timer functions used by EMFL. These Functions can be moved to
 * shared/linux_osl.c, include/linux_osl.h
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
 * $Id: osl_linux.c 638546 2016-05-18 06:18:11Z $
 */

#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include "osl_linux.h"
#ifdef DSLCPE
static DECLARE_WAIT_QUEUE_HEAD(igs_clear_waiqueue);

#endif
static void
osl_timer(ulong data)
{
	osl_timer_t *t;
#ifdef DSLCPE
	int ret = 0;
#endif
	t = (osl_timer_t *)data;

	ASSERT(t->set);

	if (t->periodic) {
#if defined(BCMJTAG) || defined(BCMSLTGT)
		t->timer.expires = jiffies + t->ms*HZ/1000*htclkratio;
#else
		t->timer.expires = jiffies + t->ms*HZ/1000;
#endif /* defined(BCMJTAG) || defined(BCMSLTGT) */
		add_timer(&t->timer);
		t->set = TRUE;
		t->fn(t->arg);
	} else {
		t->set = FALSE;
#ifdef DSLCPE
		ret = t->fn(t->arg);
#else
		t->fn(t->arg);
#endif
#ifdef BCMDBG
		if (t->name) {
			MFREE(NULL, t->name, strlen(t->name) + 1);
		}
#endif
#ifdef DSLCPE
		if (ret == SUCCESS)
		MFREE(NULL, t, sizeof(osl_timer_t));
#else
		MFREE(NULL, t, sizeof(osl_timer_t));
#endif
	}

	return;
}

osl_timer_t *

#ifdef DSLCPE
osl_timer_init(const char *name, IGS_OSL_TIMER_FUNC fn, void *arg)
#else
osl_timer_init(const char *name, void (*fn)(void *arg), void *arg)
#endif
{
	osl_timer_t *t;

	if ((t = MALLOC(NULL, sizeof(osl_timer_t))) == NULL) {
		printk(KERN_ERR "osl_timer_init: out of memory, malloced %d bytes\n",
		       (int)sizeof(osl_timer_t));
		return (NULL);
	}

	bzero(t, sizeof(osl_timer_t));

	t->fn = fn;
	t->arg = arg;
	t->timer.data = (ulong)t;
	t->timer.function = osl_timer;
#ifdef BCMDBG
	if ((t->name = MALLOC(NULL, strlen(name) + 1)) != NULL) {
		strcpy(t->name, name);
	}
#endif // endif

	init_timer(&t->timer);

	return (t);
}

void
osl_timer_add(osl_timer_t *t, uint32 ms, bool periodic)
{
	ASSERT(!t->set);

	t->set = TRUE;
	t->ms = ms;
	t->periodic = periodic;
#if defined(BCMJTAG) || defined(BCMSLTGT)
	t->timer.expires = jiffies + ms*HZ/1000*htclkratio;
#else
	t->timer.expires = jiffies + ms*HZ/1000;
#endif /* defined(BCMJTAG) || defined(BCMSLTGT) */

	add_timer(&t->timer);

	return;
}

void
osl_timer_update(osl_timer_t *t, uint32 ms, bool periodic)
{
	ASSERT(t->set);

	t->ms = ms;
	t->periodic = periodic;
	t->set = TRUE;
#if defined(BCMJTAG) || defined(BCMSLTGT)
	t->timer.expires = jiffies + ms*HZ/1000*htclkratio;
#else
	t->timer.expires = jiffies + ms*HZ/1000;
#endif /* defined(BCMJTAG) || defined(BCMSLTGT) */

	mod_timer(&t->timer, t->timer.expires);

	return;
}

/*
 * Return TRUE if timer successfully deleted, FALSE if still pending
 */
bool
osl_timer_del(osl_timer_t *t)
{
	if (t->set) {
		t->set = FALSE;
#ifdef DSLCPE
		if(try_to_del_timer_sync(&t->timer)<0) {
#else
		if (!del_timer(&t->timer)) {
#endif
			printk(KERN_INFO "osl_timer_del: Failed to delete timer\n");
#ifdef DSLCPE
				return (FAILURE);
#else
			return (FALSE);
#endif
		}
#ifdef BCMDBG
		if (t->name) {
			MFREE(NULL, t->name, strlen(t->name) + 1);
		}
#endif // endif
		MFREE(NULL, t, sizeof(osl_timer_t));
	}
	return (SUCCESS);
}
#ifdef DSLCPE
bool
igs_osl_timer_pending(osl_timer_t *t)
{
	return  timer_pending(&t->timer);
}

void igs_osl_task_reschedule(void)
{
	DEFINE_WAIT(wait);
	prepare_to_wait(&igs_clear_waiqueue,&wait,TASK_INTERRUPTIBLE);
	schedule();
	finish_wait(&igs_clear_waiqueue,&wait);
	return;
}

void igs_osl_wakeup_clear(void) {
	wake_up_interruptible(&igs_clear_waiqueue);
}

#endif
