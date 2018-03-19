#***********************************************************************
#
#  Copyright (c) 2012  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/

all dynamic install: conditional_build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

CXX := $(CC)
APPSOURCE = accel-pptp-0.8.5.tar.bz2
APPDIR = accel-pptp-0.8.5

export CXX CFLAGS CXXFLAGS LDFLAGS CC USERSPACE_PUBLIC_LIBS_DIR

clean:
	if [ -f ./$(APPDIR)/pppd_plugin/Makefile ];then $(MAKE) -C ./$(APPDIR)/pppd_plugin clean ; rm -rf install; rm -rf $(APPDIR); fi;

ifneq ($(strip $(BUILD_ACCEL_PPTP)),)

check_untar_patch_configure: sanity_check
	if [ ! -e $(APPDIR)/Makefile ]; then \
	(tar xvf $(APPSOURCE)> /dev/null || true);\
	fi; \
	if [ ! -e ./$(APPDIR)/pppd_plugin/Makefile ];then \
	patch -p1 -b -s -d $(APPDIR) <$(APPDIR).patch;\
	cd ./$(APPDIR)/pppd_plugin; ./configure --prefix=$(CURR_DIR)/install --host=$(TOOLCHAIN_PREFIX) --target=$(TOOLCHAIN_PREFIX);\
	fi
	echo "configure is done"
conditional_build: check_untar_patch_configure
	echo "configure is done"
	$(MAKE) -C $(CURR_DIR)/$(APPDIR)/pppd_plugin/src install
	mkdir -p $(INSTALL_DIR)/lib
	install -m 755 $(CURR_DIR)/install/lib/pptp.so.0.0.0 $(INSTALL_DIR)/lib
	cd $(INSTALL_DIR)/lib ; rm -f pptp.so ; rm -f pptp.so.0 ; ln -s pptp.so.0.0.0 pptp.so ; ln -s pptp.so.0.0.0 pptp.so.0
else
conditional_build: sanity_check
	@echo "skipping $(APPDIR) (not configured)"
endif

