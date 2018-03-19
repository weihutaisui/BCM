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

APP = openvpn-2.3.12
# ignore BRCM_WERROR_CFLAGS for this APP
APPCFLAGS:=$(filter-out $(BRCM_WERROR_CFLAGS),$(BRCM_COMMON_CFLAGS)) 
APPCFLAGS+= -I$(BCM_FSBUILD_DIR)/gpl/include -I$(BCM_FSBUILD_DIR)/public/include

clean:
	rm -f $(BCM_FSBUILD_DIR)/gpl/openvpn
	rm -f $(INSTALL_DIR)/bin/openvpn
	rm -f $(INSTALL_DIR)/bin/*/openvpn
	rm -rf objs
	rm -rf $(APP)

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

ifneq ($(strip $(BUILD_OPENVPN)),)
$(APP)/$(UNTAR_COMPLETE_FILE): $(APP).tar.gz Makefile
	rm -rf $(APP)
	(tar xkf $(APP).tar.gz 2> /dev/null || true); 
	@echo "$(APP) is untarred"
	touch $(APP)/$(UNTAR_COMPLETE_FILE)

objs/$(PROFILE_ARCH)/$(CONFIG_COMPLETE_FILE): $(APP)/$(UNTAR_COMPLETE_FILE) 
	mkdir -p objs/$(PROFILE_ARCH)
	(cd objs/$(PROFILE_ARCH); ../../$(APP)/configure \
        --host=$(TOOLCHAIN_PREFIX) \
        --disable-plugins --disable-shared \
        --prefix=$(BCM_FSBUILD_DIR)/gpl \
        ROUTE=/sbin/route IPROUTE=/bin/ip GREP=/bin/grep \
        IFCONFIG=/sbin/ifconfig NETSTAT=/bin/netstat \
        OPENSSL_SSL_LIBS="-lssl -lcrypto" OPENSSL_SSL_CFLAGS="" \
        OPENSSL_CRYPTO_LIBS="-lcrypto" OPENSSL_CRYPTO_CFLAGS="" \
        LDFLAGS=-L$(BCM_FSBUILD_DIR)/lib\ -L$(BCM_FSBUILD_DIR)/gpl/lib\
        CFLAGS="$(APPCFLAGS)");
	@echo "$(APP) is configured"
	touch objs/$(PROFILE_ARCH)/$(CONFIG_COMPLETE_FILE)

conditional_build: objs/$(PROFILE_ARCH)/$(CONFIG_COMPLETE_FILE) sanity_check
	$(MAKE) -C objs/$(PROFILE_ARCH)
	$(MAKE) -C objs/$(PROFILE_ARCH)/src/openvpn install
	mkdir -p $(INSTALL_DIR)/bin$(BCM_INSTALL_SUFFIX_DIR)
	cp -d $(BCM_FSBUILD_DIR)/gpl/sbin/openvpn $(INSTALL_DIR)/bin$(BCM_INSTALL_SUFFIX_DIR)
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif