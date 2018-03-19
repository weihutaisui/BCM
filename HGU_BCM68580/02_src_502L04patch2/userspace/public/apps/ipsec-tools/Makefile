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

UNTAR_COMPLETE_FILE = .untar_complete
CONFIG_COMPLETE_FILE = config_complete

APP = ipsec-tools-0.8.0
# ignore BRCM_WERROR_CFLAGS for this APP
APPCFLAGS:=$(filter-out $(BRCM_WERROR_CFLAGS),$(BRCM_COMMON_CFLAGS)) 
APPCFLAGS+= -Wno-cpp -Wno-implicit-int

clean:
	rm -f $(INSTALL_DIR)/bin/setkey
	rm -f $(INSTALL_DIR)/bin/racoon
	rm -rf $(APP)
	rm -rf objs

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

ifneq ($(strip $(BUILD_IPSEC_TOOLS)),)
$(APP)/$(UNTAR_COMPLETE_FILE): Makefile $(APP).tar.bz2
	rm -rf $(APP)
	(tar xkf $(APP).tar.bz2 2> /dev/null || true); 
	patch -p1 -b -s -d$(APP) < bcm_patches/$(APP)-$(PROFILE_KERNEL_VER).patch; \
	if [ $$? -ne 0 ]; then exit 1; fi;
	touch $(APP)/$(UNTAR_COMPLETE_FILE)
	@echo "$(APP) is untarred"

objs/$(PROFILE_ARCH)/$(CONFIG_COMPLETE_FILE): $(APP)/$(UNTAR_COMPLETE_FILE)
	mkdir -p objs/$(PROFILE_ARCH)
	(cd objs/$(PROFILE_ARCH); ../../$(APP)/configure \
        --host=$(TOOLCHAIN_PREFIX) \
        --enable-natt --enable-hybrid --enable-dpd \
        --enable-ipv6 --enable-security-context=no \
        --prefix=$(INSTALL_DIR)/usr/local \
        --with-openssl=$(BCM_FSBUILD_DIR)/public \
        --with-kernel-headers=$(TOOLCHAIN_INCLUDE_DIR) \
        LDFLAGS=-L$(INSTALL_DIR)/lib\ -L$(INSTALL_DIR)/lib\
        CFLAGS="$(APPCFLAGS)");
	@echo "$(APP) is configured"
	touch objs/$(PROFILE_ARCH)/$(CONFIG_COMPLETE_FILE)

conditional_build: objs/$(PROFILE_ARCH)/$(CONFIG_COMPLETE_FILE) sanity_check
	$(MAKE) -C objs/$(PROFILE_ARCH) -j1
	install -m 755 objs/$(PROFILE_ARCH)/src/setkey/setkey $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/setkey
	install -m 755 objs/$(PROFILE_ARCH)/src/racoon/racoon $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/racoon
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif
