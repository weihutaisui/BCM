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

/*
 * Plugin to disable the default ppp_unix plugin.
 */

#include "l2tp_private.h"

/*****************************************************************************
 * L2TP plugin interface
 *****************************************************************************/

const char openl2tp_plugin_version[] = "V0.16";

int openl2tp_plugin_init(void)
{
	/* Disable loading default plugin */
	l2tp_inhibit_default_plugin = 1;
	return 0;
}

void openl2tp_plugin_cleanup(void)
{
}

