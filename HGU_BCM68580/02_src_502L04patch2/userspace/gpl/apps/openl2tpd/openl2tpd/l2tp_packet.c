/*****************************************************************************
 * Copyright (C) 2004,2005,2006,2007,2008 Katalix Systems Ltd
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 *
 *****************************************************************************/

/* Packet management functions.
 */

#include "usl.h"
#include "l2tp_private.h"


struct l2tp_packet *l2tp_pkt_alloc(int num_bufs)
{
	struct l2tp_packet *pkt;

	pkt = calloc(1, sizeof(*pkt) + (num_bufs * sizeof(struct iovec)));
	if (pkt == NULL) {
		goto out;
	}

	pkt->num_bufs = num_bufs;
	pkt->requeue_count = -1;
	USL_LIST_HEAD_INIT(&pkt->list);

out:
	return pkt;
}

struct l2tp_packet *l2tp_pkt_peek(struct usl_list_head *head)
{
	if (usl_list_empty(head)) {
		return NULL;
	}

	return usl_list_entry(head->next, struct l2tp_packet, list);
}

void l2tp_pkt_unlink(struct l2tp_packet *pkt)
{
	usl_list_del_init(&pkt->list);
}

int l2tp_pkt_queue_empty(struct usl_list_head *head)
{
	return usl_list_empty(head);
}

void l2tp_pkt_queue_add(struct usl_list_head *head, struct l2tp_packet *pkt)
{
	usl_list_add_tail(&pkt->list, head);
}

struct l2tp_packet *l2tp_pkt_dequeue(struct usl_list_head *head)
{
	struct l2tp_packet *pkt;

	if (usl_list_empty(head)) {
		return NULL;
	}
	pkt = usl_list_entry(head->next, struct l2tp_packet, list);
	usl_list_del_init(&pkt->list);
	return pkt;
}

void l2tp_pkt_free(struct l2tp_packet *pkt)
{
	int buf;

	L2TP_DEBUG(L2TP_DATA, "%s: pkt=%p, msg=%d tid=%hu sid=%hu ns=%hu nr=%hu", __func__,
		   pkt, pkt->msg_type, pkt->tunnel_id, pkt->session_id, pkt->nr, pkt->ns);

	if (pkt->num_bufs > 0) {
		for (buf = 0; buf < pkt->num_bufs; buf++) {
			if (pkt->iov[buf].iov_base != NULL) {
#ifdef DEBUG
				if (pkt->iov[buf].iov_len == 0) {
					l2tp_log(LOG_ERR, "Freeing zero length buffer %p, index %d?", 
						 pkt->iov[buf].iov_base, buf);
				}
				USL_POISON_MEMORY(pkt->iov[buf].iov_base, 0xe8, pkt->iov[buf].iov_len);
#endif
				free(pkt->iov[buf].iov_base);
			} else {
				break;
			}
		}
	}

	USL_POISON_MEMORY(pkt, 0xe7, sizeof(*pkt));
	free(pkt);
}

void l2tp_pkt_queue_purge(struct usl_list_head *head)
{	
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	struct l2tp_packet *pkt;

	usl_list_for_each(walk, tmp, head) {
		pkt = usl_list_entry(walk, struct l2tp_packet, list);
		usl_list_del(&pkt->list);
		l2tp_pkt_free(pkt);
	}
}

