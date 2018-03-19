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

#include "usl.h"

/*****************************************************************************
 * Generic FSM operation
 *****************************************************************************/

void usl_fsm_handle_event(struct usl_fsm_instance *fsmi,
			  int event, void *arg1, void *arg2, void *arg3)
{
	struct usl_fsm_table const *fsm = fsmi->table; 
	struct usl_fsm_entry const *fsme = &fsm->fsm[0];

	if (event < 0) {
		goto out;
	}

	if ((fsmi->state >= fsm->num_states) || (event >= fsm->num_events)) {
		USL_SYSLOG(LOG_ERR, "Internal FSM error: %s: state=%d event=%d",
			   fsm->name, fsmi->state, event);
		return;
	}

	(*fsm->log)(fsmi, LOG_INFO, "FSM: %s(%s) event %s in state %s",
		    fsm->name, fsmi->name, fsm->event_names[event], fsm->state_names[fsmi->state]);

	while (fsme->action != NULL) {
		if ((event == fsme->event) && (fsmi->state == fsme->state)) {
			int prev_state = fsmi->state;
			fsmi->state = fsme->new_state;
			(*fsme->action)(arg1, arg2, arg3);
			if (fsme->new_state != prev_state) {
				(*fsm->log)(fsmi, LOG_INFO, "FSM: %s(%s) state change: %s --> %s",
					    fsm->name, fsmi->name, fsm->state_names[prev_state], 
					    fsm->state_names[fsme->new_state]);
			}
			return;
		}
		fsme++;
	}

out:
	(*fsm->log)(fsmi, LOG_ERR, "FSM: %s: %s(%s) unhandled event %d in state %d",
		    __func__, fsm->name, fsmi->name, event, fsmi->state);
}

void usl_fsm_new_state(const struct usl_fsm_table *fsm, struct usl_fsm_instance *fsmi, int new_state)
{
	if (new_state >= fsm->num_states) {
		USL_SYSLOG(LOG_ERR, "FSM: Internal error: %s: new state=%d out of range",
			   fsm->name, new_state);
		return;
	}

	if (new_state != fsmi->state) {
		(*fsm->log)(fsmi, LOG_INFO, "FSM: %s(%s) state change: %s --> %s",
			    fsm->name, fsmi->name, fsm->state_names[fsmi->state], 
			    fsm->state_names[new_state]);
		fsmi->state = new_state;
	}
}

const char *usl_fsm_state_name(struct usl_fsm_instance *fsmi)
{
	if (fsmi->state > fsmi->table->num_states) {
		return "???";
	}
	return fsmi->table->state_names[fsmi->state];
}

const char *usl_fsm_event_name(struct usl_fsm_instance *fsmi, int event)
{
	if (event > fsmi->table->num_events) {
		return "???";
	}
	return fsmi->table->event_names[event];
}
