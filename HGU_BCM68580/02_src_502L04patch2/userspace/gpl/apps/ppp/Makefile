#***********************************************************************
#
#  Copyright (c) 2012  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/

all dynamic install: conditional_build

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
# You do not need to modify this part.
#

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common
APPSOURCE = ppp-2.4.5.tar.gz
APPDIR = ppp-2.4.5

CXX := $(CC)

export CXX CFLAGS CXXFLAGS LDFLAGS CC USERSPACE_PUBLIC_LIBS_DIR

clean:
	@if [ -e ./$(APPDIR)/pppd/Makefile ]; then \
	$(MAKE) -C ./$(APPDIR)/pppd clean ; \
	rm -rf install ; \
	rm -rf $(APPDIR) ; \
	fi

#.PHONY: check_untar_patch_configure conditional_build

ifneq ($(strip $(BUILD_ACCEL_PPTP)),)

check_untar_patch_configure: sanity_check
	@if [ ! -e $(APPDIR)/Makefile ]; then \
    	echo "Untarring original $(APPDIR) source"; \
	    (tar zxvf $(APPSOURCE)> /dev/null || true); \
	    if [ ! -e ./$(APPDIR)/pppd/Makefile ];then \
	    cd ./$(APPDIR); ./configure --prefix=$(CURR_DIR)/install --host=$(TOOLCHAIN_PREFIX) --target=$(TOOLCHAIN_PREFIX);\
	    echo "configure is done" \
	    echo "Applying patches to $(APPDIR)"; \
	    cd ../; patch -p1 -b -s -d $(APPDIR) <$(APPDIR).patch; \
	    if [ $$? -ne 0 ]; then \
	         echo "patch returned error (ignoring)"; \
	    fi; \
	    fi; \
	 fi   

conditional_build: check_untar_patch_configure
	$(MAKE) -C $(CURR_DIR)/$(APPDIR)/pppd install
else
conditional_build: sanity_check
	@echo "skipping $(APPDIR) (not configured)"
endif

