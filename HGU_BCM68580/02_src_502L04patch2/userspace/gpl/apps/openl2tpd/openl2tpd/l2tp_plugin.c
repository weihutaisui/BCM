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
 * Plugin support
 */

#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <pwd.h>
#include <dlfcn.h>

#include "usl.h"
#include "l2tp_private.h"

#define OPENL2TP_DEFAULT_PLUGIN	"ppp_unix.so"

struct l2tp_plugin {
	struct usl_list_head list;
	char *path;
	void *handle;
};

static USL_LIST_HEAD(l2tp_plugin_list);


static int l2tp_plugin_load(struct l2tp_plugin *plugin)
{
	const char *err;
	void (*init)(void);
	const char *vers;

	plugin->handle = dlopen(plugin->path, RTLD_GLOBAL | RTLD_NOW);
	if (plugin->handle == 0) {
		err = dlerror();
		if (err != 0)
			l2tp_log(LOG_ERR, "%s", err);
		l2tp_log(LOG_ERR, "Couldn't load plugin %s", plugin->path);
		goto err;
	}
	init = (void (*)(void)) dlsym(plugin->handle, "openl2tp_plugin_init");
	if (init == 0) {
		l2tp_log(LOG_ERR, "%s has no initialization entry point", plugin->path);
		goto err_close;
	}
	vers = (const char *) dlsym(plugin->handle, "openl2tp_plugin_version");
	if (vers == NULL) {
		l2tp_log(LOG_ERR, "%s has no version information", plugin->path);
		goto err_close;
	}

	l2tp_log(LOG_INFO, "Loading plugin %s, version %s", plugin->path, vers);
	(*init)();

	return 0;

err_close:
	dlclose(plugin->handle);
err:
	return -EINVAL;
}

int l2tp_plugin_add(char *name)
{
	struct l2tp_plugin *plugin;
	char *path;
	int result = 0;

	if (strchr(name, '/') == 0) {
		const char *base = L2TP_PLUGIN_DIR;
		int len = strlen(base) + strlen(name) + 2;
		path = malloc(len);
		if (path == NULL) {
			l2tp_log(LOG_ERR, "OOM: plugin file path");
			return -ENOMEM;
		}

		strncpy(path, base, len);
		strncat(path, "/", len);
		strncat(path, name, len);
	} else {
		path = strdup(name);
		if (path == NULL) {
			l2tp_log(LOG_ERR, "OOM: plugin file path");
			return -ENOMEM;
		}
	}

	plugin = calloc(1, sizeof(*plugin));
	if (plugin == NULL) {
		goto err_free_path;
	}
	plugin->path = path;
	USL_LIST_HEAD_INIT(&plugin->list);

	usl_list_add_tail(&plugin->list, &l2tp_plugin_list);

	/* If it is the default plugin, load it now */
	if (strcmp(name, OPENL2TP_DEFAULT_PLUGIN) == 0) {
		result = l2tp_plugin_load(plugin);
	}

	return result;

err_free_path:
	if (path != name)
		free(path);

	return -ENOMEM;
}

int l2tp_plugin_init(void)
{
	struct l2tp_plugin *plugin;
	struct usl_list_head *tmp;
	struct usl_list_head *walk;

	/* Load all registered plugins */
	usl_list_for_each(walk, tmp, &l2tp_plugin_list) {
		plugin = usl_list_entry(walk, struct l2tp_plugin, list);
		l2tp_plugin_load(plugin);
	}

	/* Load ppp_unix plugin if default plugin has not been inhibited */
	if (!l2tp_inhibit_default_plugin) {
		if (l2tp_plugin_add(OPENL2TP_DEFAULT_PLUGIN) < 0) {
			exit(1);
		}
	}

	return 0;
}

void l2tp_plugin_cleanup(void)
{
	struct l2tp_plugin *plugin;
	struct usl_list_head *tmp;
	struct usl_list_head *walk;
	void (*cleanup)(void);

	/* Call cleanup functions of every loaded plugin */
	usl_list_for_each(walk, tmp, &l2tp_plugin_list) {
		plugin = usl_list_entry(walk, struct l2tp_plugin, list);
		l2tp_log(LOG_INFO, "Unloading plugin %s", plugin->path);
		if (plugin->handle != NULL) {
			cleanup = (void (*)(void))dlsym(plugin->handle, "openl2tp_plugin_cleanup");
			if (cleanup != NULL) {
				(*cleanup)();
			}
			dlclose(plugin->handle);
		}
		free(plugin->path);
		usl_list_del(&plugin->list);
		free(plugin);
	}
}
