# <<Broadcom-WL-IPTag/Proprietary:>>

AUTOMAKE_OPTIONS = foreign
include $(BASEDIR)/src/router/.config
CURR_DIR := $(shell pwd)
BUILD_DIR=$(word 1, $(subst /bcmdrivers, /bcmdrivers,$(shell pwd)))

SRCDIR = $(BASEDIR)/src
VISSHARED=../shared
BCMWIFIDIR=$(SRCDIR)/shared/bcmwifi/src
SHAREDDIR=$(SRCDIR)/shared

CFLAGS=-Wall -Werror -Wno-strict-aliasing -DDSLCPE -DDSLCPE_ENDIAN -DDSLCPE_VIS_WL_IO -I$(SRCDIR)/shared/bcmwifi/include/ \
	-I$(SRCDIR)/common/include/ \
	-I$(SRCDIR)/include/ -I../include/ \
	-I$(BCM_FSBUILD_DIR)/public/include/libxml2 \
	-I$(BCM_FSBUILD_DIR)/public/include/json-c \
	-I$(BUILD_DIR)/userspace/private/libs/wlcsm/include \
	-I$(BUILD_DIR)/userspace/private/apps/wlan/wlcsm/include \
	-I$(BUILD_DIR)/userspace/private/apps/wlan/src/router/shared \
	-I$(SRCDIR)/../components/opensource/libxml2/include/ -I$(SRCDIR)/../components/shared/ -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""


LDFLAGS=-L$(COMPDIR)/opensource/libxml2/installbin/lib \
		-L$(INSTALL_DIR)/lib   \
		-lxml2 -lpthread $(USRLIBDIR)/libm.a -L$(SRCBASE_ROUTER)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram -L$(EXTRALIBDIR) -L$(INSTALL_DIR)/lib -lwlcsm -lwlbcmshared -lwlbcmcrypto -lm

if IS_BCA_HNDROUTER
LDFLAGS += $(LIBDIR)/../usr/lib/libm.a
else
LDFLAGS += $(USRLIBDIR)/libm.a
endif

LDFLAGS	+= $(EXTRA_LDFLAGS)

CFLAGS  += -DBCMDBG
bin_PROGRAMS = vis-datacollector
vis_datacollector_SOURCES = $(SHAREDDIR)/bcmutils.c \
			$(SHAREDDIR)/bcmxtlv.c $(SHAREDDIR)/bcm_app_utils.c \
			$(BCMWIFIDIR)/bcmwifi_channels.c vis_common.c \
			$(VISSHARED)/vis_xmlshared.c $(VISSHARED)/vis_sock_util.c \
			$(VISSHARED)/vis_shared_util.c \
			vis_socketclient.c vis_xmlutility.c vis_savedata.c\
			vis_wl.c vis_wlmetrics.c vis_wlcongestion.c  vis_rrmprocess.c \
			vis_linux_main.c

clean-local:
	rm -f Makefile
	rm -rf autom4te.cache
	rm -f config.h
	rm -f config.log
	rm -f config.status
	rm -f stamp-h1
	rm -rf installbin
