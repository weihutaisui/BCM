# <<Broadcom-WL-IPTag/Proprietary:>>

AUTOMAKE_OPTIONS = foreign
CURR_DIR := $(shell pwd)
BUILD_DIR=$(word 1, $(subst /bcmdrivers, /bcmdrivers,$(shell pwd)))

VISSHARED=../shared

CFLAGS +=-Wall -Werror -DDSLCPE -DDSLCPE_ENDIAN -I$(SRCDIR)/common/include/ -I$(SRCDIR)/include/ -I../include/ \
		-L$(INSTALL_DIR)/lib \
		-I$(BCM_FSBUILD_DIR)/public/include/libxml2 \
		-I$(BCM_FSBUILD_DIR)/public/include/json-c \
		-I$(BCM_FSBUILD_DIR)/public/include \
		-I$(BUILD_DIR)/userspace/private/apps/wlan/wlcsm/include \
		-I$(BUILD_DIR)/userspace/private/libs/wlcsm/include \
		-I$(SRCDIR)/../components/opensource/jsonc/ \
		-I$(SRCDIR)/../components/opensource/libxml2/include/ \
		-I$(SRCDIR)/../components/opensource/sqlite3/ \
		-I$(SRCDIR)/../components/shared/ \
		-DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""


LDFLAGS = -L$(COMPDIR)/opensource/sqlite3/installbin/lib \
       	-L$(COMPDIR)/opensource/jsonc/installbin/lib \
	-L$(COMPDIR)/opensource/libxml2/installbin/lib \
	-L$(INSTALL_DIR)/lib -L$(INSTALL_DIR)/lib \
	-lsqlite3 -lpthread -lxml2 -ljson-c -L$(SRCBASE_ROUTER)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram -lwlcsm -lm
CFLAGS  += -DBCMDBG

bin_PROGRAMS = vis-dcon
vis_dcon_SOURCES = $(VISSHARED)/vis_xmlshared.c $(VISSHARED)/vis_sock_util.c \
			$(VISSHARED)/vis_shared_util.c vis_synchdb.c \
			vis_utility.c database.c vis_xmlutility.c vis_jsonutility.c \
			vis_sockserver.c vis_dcon_main.c

clean-local:
	rm -f Makefile
	rm -rf autom4te.cache
	rm -f config.h
	rm -f config.log
	rm -f config.status
	rm -f stamp-h1
	rm -rf installbin
