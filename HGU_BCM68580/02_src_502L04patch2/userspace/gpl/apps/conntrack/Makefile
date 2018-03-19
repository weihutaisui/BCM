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

APP = conntrack-tools-1.4.1
REQUIRED_AUTOCONF_VERSION := 2.59
REQUIRED_AUTOMAKE_VERSION := 1.10.2

check_versions:
	$(HOSTTOOLS_DIR)/scripts/checkver.pl -e autoconf -r $(REQUIRED_AUTOCONF_VERSION)
	$(HOSTTOOLS_DIR)/scripts/checkver.pl -e automake -r $(REQUIRED_AUTOMAKE_VERSION)


.PHONY: check_untar_patch_configure conditional_build check_versions

ifneq ($(strip $(BUILD_CONNTRACK_TOOLS)),)

check_untar_patch_configure: sanity_check check_versions
	@if [ ! -e $(APP)/Makefile ]; then \
	    echo "Untarring original $(APP) source"; \
	    (tar xkjf $(APP).tar.bz2 2> /dev/null || true); \
	    if [ -e $(APP).patch ]; then \
	        echo "Applying patches to $(APP)"; \
	        patch -p1 -b -s -d$(APP) < $(APP).patch; \
	        if [ $$? -ne 0 ]; then \
	            exit 1; \
	        fi; \
	    fi; \
	    echo "Running configure on $(APP)"; \
	    echo "building to $(INSTALL_DIR)"; \
	    cd $(APP); \
	    export PKG_CONFIG_LIBDIR=$(INSTALL_DIR)/lib/gpl; \
	    export PKG_CONFIG_PATH=$(INSTALL_DIR)/lib/gpl/pkgconfig; \
	    ./configure --host=$(TOOLCHAIN_PREFIX) \
	    --prefix=$(INSTALL_DIR) \
	    --sbindir=$(INSTALL_DIR)/bin \
	    --libdir=$(INSTALL_DIR)/lib/gpl \
	    LDFLAGS=-L$(INSTALL_DIR)/lib \
	    CFLAGS=-s\ -Os\ -Wno-strict-aliasing; \
	fi

conditional_build: check_untar_patch_configure 
	cd $(APP); $(MAKE) -j1; $(MAKE) -j1 install
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif

# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean:
	rm -rf $(APP)

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean



