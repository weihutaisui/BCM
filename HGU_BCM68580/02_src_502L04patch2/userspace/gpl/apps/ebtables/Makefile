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

APP = ebtables-v2.0.10-4

clean:
	rm -rf $(APP)
	rm -f $(INSTALL_DIR)/bin/ebtables
	rm -f $(INSTALL_DIR)/lib/gpl/libebt*.so
	rm -f $(INSTALL_DIR)/etc/ethertypes

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean
	rm -rf $(APP)

.PHONY: check_untar_patch_configure conditional_build

ifneq ($(strip $(BUILD_EBTABLES)),)
check_untar_patch_configure: sanity_check
	@if [ ! -e $(APP)/INSTALL ]; then \
	    echo "Untarring original $(APP) source"; \
	    (tar xkf $(APP).tar.bz2 2> /dev/null || true); \
	    echo "Applying patches to $(APP)"; \
	    patch -p1 -b -s -d$(APP) < $(APP).patch; \
	    if [ $$? -ne 0 ]; then \
	         echo "patch returned error (ignoring)"; \
	    fi; \
	    echo "Checking configure on $(APP)/configure"; \
	    if [ -e $(APP)/configure ]; then \
	        echo "Running configure on $(APP)"; \
	        echo "building to $(INSTALL_DIR)/usr/local"; \
	         cd $(APP); \
	        ./configure --host=mips-linux-uclibc \
	        --target=mips-linux-uclibc \
	        --prefix=$(INSTALL_DIR)/usr/local \
	        LDFLAGS=-L$(INSTALL_DIR)/lib \
	        CFLAGS=-g\ -O2\ -Wno-strict-aliasing; \
	    fi \
	fi
	@if [ $(APP)==ebtables-v2.0.10-4 -a ! -e $(APP)/extensions/ebt_ip.c ]; then \
		echo "Untarring original $(APP) source"; \
		(tar xkf $(APP).tar.bz2 2> /dev/null || true); \
	fi

conditional_build: check_untar_patch_configure
	cd $(APP); \
	$(MAKE) -j1 DESTDIR=$(INSTALL_DIR) CC=$(CC) LIBDIR=/lib/gpl KERNEL_INCLUDES="$(TOOLCHAIN_INCLUDE_DIR) -I$(KERNEL_LINKS_DIR)";
	mkdir -p $(INSTALL_DIR)/lib/$(BCM_INSTALL_SUFFIX_DIR)/gpl
	install -m 0755 $(APP)/ebtables $(INSTALL_DIR)/bin
	install -m 0755 $(APP)/extensions/libebt*.so $(INSTALL_DIR)/lib/$(BCM_INSTALL_SUFFIX_DIR)/gpl
	install -m 0755 $(APP)/libebt*.so $(INSTALL_DIR)/lib/$(BCM_INSTALL_SUFFIX_DIR)/gpl
	mkdir -p $(INSTALL_DIR)/etc
	install -m 0644 $(APP)/ethertypes $(INSTALL_DIR)/etc
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif
