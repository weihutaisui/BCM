#
# Broadcom web pages
#
# Copyright (C) 2017, Broadcom. All Rights Reserved.
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# $Id: Makefile 701506 2017-05-25 09:01:59Z $
#

all: pages.mk
VISWWW = $(SRCBASE)/../components/apps/visualization/www
IQOSWWW = $(SRCBASE)/../components/apps/iqos/www
WBDWWW = $(WBDSOURCE_DIR)/www
IQOSICONS = $(IQOSWWW)/icons
JQUERYLIB = $(SRCBASE)/../components/opensource/jquerylib

ifneq ($(WIND_HOST_TYPE),x86-win32)
ifdef BCM_NETBSD
# NetBSD
CONFIG_NAT=y
else
# Linux
$(SRCBASE)/router/.config:
	cp $(SRCBASE)/router/config/defconfig $@

include $(SRCBASE)/router/.config
include $(SRCBASE)/router/.config.plt
endif
else
# VxWorks compiled on Windows host
CONFIG_WPS=y
endif

include $(SRCBASE)/makefiles/WLAN_Common.mk

PAGES := blur_new.jpg logo_new.gif repeat_edge_new.gif overlib.js style.css favicon.ico
PAGES += index.asp basic.gif
PAGES += lan.asp lan.gif
ifeq ($(CONFIG_NAT),y)
PAGES += wan.asp wan.gif
endif
PAGES += status.asp status.gif
ifeq ($(CONFIG_NAT),y)
PAGES += filter.asp filter.gif
PAGES += forward.asp forward.gif
endif
ifneq ($(CONFIG_SAMBA)$(CONFIG_CIFS)$(CONFIG_DLNA_DMS)$(CONFIG_DLNA_DMR),)
PAGES += storage.asp storage.gif
endif
PAGES += ssid.asp ssid.gif
PAGES += radio.asp radio.gif
PAGES += security.asp security.gif
ifeq ($(CONFIG_WPS),y)
PAGES += wps.asp wps.gif
endif
ifeq ($(CONFIG_HSPOT),y)
PAGES += passpoint.asp passpoint.gif
endif
PAGES += firmware.asp firmware.gif
ifeq ($(CONFIG_BCMQOS),y)
PAGES += qos.asp qos.gif
endif

ifeq ($(CONFIG_WAPI_IAS),y)
PAGES += as.asp as.gif
endif

ifeq ($(CONFIG_WFI),y)
PAGES += cancelinvite.asp
endif

ifeq ($(CONFIG_PLC),y)
PAGES += plc.asp plc.gif
endif

PAGES += media.asp media.gif

ifeq ($(PLT),arm)
ifeq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.6.36),TRUE)
ifeq ($(CONFIG_VISUALIZATION),y)
PAGES += visindex.asp channelcapacity.asp channels.asp configure.asp metrics.asp
PAGES += curvedLines.js jquery.cookie.js jquery.flot.axislabels.js jquery.flot.min.js jquery.flot.navigate.min.js jquery.flot.orderBars.js jquery.flot.time.min.js jquery-2.0.3.min.js jquery.tablesorter.min.js jquery.flot.pie.js
PAGES += channelcapacity.js channels.js common.js configure.js main.js matrics.js
PAGES += tabheading.jpg down-arrow.gif up-arrow.gif ap-image.jpg download.jpg
PAGES += channelCapacity.css configure.css matrics.css visstyle.css tablestyle.css jquery-ui.min.css
PAGES += highlightseries.js jquery.flot.js jquery.flot.stack.js jquery-ui.js
PAGES += visualization.gif
PAGES += zerowaitdfs.asp zerowaitdfs.css zerowaitdfs.js

VISPAGES += $(VISWWW)/visindex.asp $(VISWWW)/channelcapacity.asp $(VISWWW)/channels.asp $(VISWWW)/configure.asp $(VISWWW)/metrics.asp
VISPAGES += $(VISWWW)/channelcapacity.js $(VISWWW)/channels.js $(VISWWW)/common.js $(VISWWW)/configure.js $(VISWWW)/main.js $(VISWWW)/matrics.js
VISPAGES += $(VISWWW)/tabheading.jpg $(VISWWW)/down-arrow.gif $(VISWWW)/up-arrow.gif $(VISWWW)/ap-image.jpg $(VISWWW)/download.jpg
VISPAGES += $(VISWWW)/channelCapacity.css $(VISWWW)/configure.css $(VISWWW)/matrics.css $(VISWWW)/visstyle.css $(VISWWW)/tablestyle.css
VISPAGES += $(VISWWW)/visualization.gif
VISPAGES += $(VISWWW)/zerowaitdfs.asp $(VISWWW)/zerowaitdfs.css $(VISWWW)/zerowaitdfs.js

VISPAGES += $(JQUERYLIB)/curvedLines.js $(JQUERYLIB)/jquery.flot.axislabels.js $(JQUERYLIB)/jquery.flot.navigate.min.js
VISPAGES += $(JQUERYLIB)/jquery.flot.stack.js $(JQUERYLIB)/highlightseries.js $(JQUERYLIB)/jquery.flot.js $(JQUERYLIB)/jquery.flot.orderBars.js
VISPAGES += $(JQUERYLIB)/jquery.flot.time.min.js $(JQUERYLIB)/jquery.tablesorter.min.js $(JQUERYLIB)/jquery-2.0.3.min.js
VISPAGES += $(JQUERYLIB)/jquery.cookie.js $(JQUERYLIB)/jquery.flot.min.js $(JQUERYLIB)/jquery.flot.pie.js
VISPAGES += $(JQUERYLIB)/jquery-ui.js $(JQUERYLIB)/jquery-ui.min.css
endif # CONFIG_VISUALIZATION
endif
endif # PLT

ifeq ($(PLT),arm)
ifeq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.6.36),TRUE)
ifeq ($(CONFIG_TREND_IQOS),y)
PAGES += iQoS.asp iQoS.css iQoS.js iqoscommon.js iqosstyle.css iQoSNetworkSummary.js iQoSNetworkSummary.asp iQoSNetworkSummary.css router.png www.png iQoSSettings.js iQoSSettings.css iQoSSettings.asp
PAGES += jquery.flot.axislabels.js jquery.flot.min.js jquery.flot.navigate.min.js jquery.flot.time.min.js jquery-2.0.3.min.js jquery.flot.tooltip.js jquery.flot.pie.js
PAGES += jquery.flot.js jquery.flot.stack.js jquery-ui.js jquery.min.js jquery-ui.min.js
PAGES += iqos.gif

IQOSPAGES += $(IQOSWWW)/iQoS.asp $(IQOSWWW)/iQoS.css $(IQOSWWW)/iQoS.js $(IQOSWWW)/iQoSNetworkSummary.js $(IQOSWWW)/iQoSNetworkSummary.asp $(IQOSWWW)/iQoSNetworkSummary.css $(IQOSWWW)/iqoscommon.js $(IQOSWWW)/router.png $(IQOSWWW)/www.png $(IQOSWWW)/iqosstyle.css $(IQOSWWW)/iQoSSettings.js $(IQOSWWW)/iQoSSettings.css $(IQOSWWW)/iQoSSettings.asp
IQOSPAGES += $(JQUERYLIB)/jquery.flot.axislabels.js $(JQUERYLIB)/jquery.flot.min.js $(JQUERYLIB)/jquery.flot.navigate.min.js $(JQUERYLIB)/jquery.flot.time.min.js $(JQUERYLIB)/jquery-2.0.3.min.js
IQOSPAGES += $(JQUERYLIB)/jquery.flot.js $(JQUERYLIB)/jquery.flot.stack.js $(JQUERYLIB)/jquery.flot.tooltip.js $(JQUERYLIB)/jquery.flot.pie.js $(JQUERYLIB)/jquery-ui.js $(JQUERYLIB)/jquery.min.js  $(JQUERYLIB)/jquery-ui.min.js $(IQOSWWW)/iqos.gif
endif # CONFIG_TREND_IQOS

ifeq ($(CONFIG_WBD),y)
PAGES += wbd_demo.asp wbd_demo.css wbd_demo.js
WBDPAGES += $(WBDWWW)/wbd_demo.asp $(WBDWWW)/wbd_demo.css $(WBDWWW)/wbd_demo.js
endif # CONFIG_WBD
endif # wlan_version_ge
endif # PLT

ifneq ($(WIND_HOST_TYPE),x86-win32)
ifdef BCM_NETBSD
pages.mk:
	echo PAGES := $(PAGES) > $@
else
# Linux
pages.mk: $(SRCBASE)/router/.config
	echo "PAGES := $(PAGES)" > $@
endif
else
# VxWorks
pages.mk:
	echo PAGES := $(PAGES) > $@
endif

install:
	install -d $(INSTALLDIR)/www
ifeq ($(PLT),arm)
ifeq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.6.36),TRUE)
ifeq ($(CONFIG_VISUALIZATION),y)
	install -m 644 $(VISPAGES) .
endif # CONFIG_VISUALIZATION

ifeq ($(CONFIG_WBD),y)
	install -m 644 $(WBDPAGES) .
endif
endif # wlan_version_ge

ifeq ($(LINUX_VERSION)$(PLT),2_6_36arm)
ifeq ($(CONFIG_TREND_IQOS),y)
	install -m 644 $(IQOSPAGES) .
	install -m 644 $(IQOSICONS)/* $(INSTALLDIR)/www
endif # CONFIG_TREND_IQOS
endif # LINUX_VERSION
endif # PLT
	install -C -m 644 $(PAGES) $(INSTALLDIR)/www

clean:
	rm -f pages.mk
