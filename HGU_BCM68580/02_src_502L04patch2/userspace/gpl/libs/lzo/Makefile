all dynamic install: conditional_build

LIB = lzo-2.09

clean:
	rm -rf objs
	rm -rf $(LIB)
	rm -f $(INSTALL_DIR)/lib/liblzo2.*
	rm -f $(BCM_FSBUILD_DIR)/gpl/lib/liblzo2.*
	rm -f $(BCM_FSBUILD_DIR)/gpl/include/lzo/*

# Since we do not have any checked in files below lzo directory, we
# can rm -rf the whole directory before release.  The next line is
# a hint to to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

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

SHLIB_VERSION_NUMBER := 2.0.0

ifneq ($(strip $(BUILD_OPENVPN)),)
$(LIB)/$(UNTAR_COMPLETE_FILE): $(LIB).tar.gz Makefile
	rm -rf $(LIB)
	(tar xkf $(LIB).tar.gz 2> /dev/null || true);
	@echo "$(LIB) is untarred"
	touch $(LIB)/$(UNTAR_COMPLETE_FILE)

objs/$(PROFILE_ARCH)/$(CONFIG_COMPLETE_FILE): $(LIB)/$(UNTAR_COMPLETE_FILE)
	mkdir -p objs/$(PROFILE_ARCH)
	(cd objs/$(PROFILE_ARCH); ../../$(LIB)/configure --enable-shared \
                --prefix=$(BCM_FSBUILD_DIR)/gpl \
                --host=$(TOOLCHAIN_PREFIX)\
        CFLAGS="$(BRCM_COMMON_CFLAGS)");
	rm -f $(APP)/BRCM_CONFIG_ID_*
	@echo "$(LIB) is configured"
	touch objs/$(PROFILE_ARCH)/$(CONFIG_COMPLETE_FILE)

conditional_build: objs/$(PROFILE_ARCH)/$(CONFIG_COMPLETE_FILE) sanity_check
	@echo "$(LIB) (enabled)"
	$(MAKE) -C objs/$(PROFILE_ARCH)
	$(MAKE) -C objs/$(PROFILE_ARCH) install-libLTLIBRARIES
	mkdir -p $(INSTALL_DIR)/lib
	mkdir -p $(BCM_FSBUILD_DIR)/gpl/lib
	mkdir -p $(BCM_FSBUILD_DIR)/gpl/include/lzo
	install -m 755 objs/$(PROFILE_ARCH)/src/.libs/liblzo2.so.$(SHLIB_VERSION_NUMBER) $(INSTALL_DIR)/lib;
	cd $(INSTALL_DIR)/lib; rm -f liblzo2.so; ln -s liblzo2.so.$(SHLIB_VERSION_NUMBER) liblzo2.so; cd - > /dev/null;
	cd $(INSTALL_DIR)/lib; rm -f liblzo2.so.2; ln -s liblzo2.so.$(SHLIB_VERSION_NUMBER) liblzo2.so.2; cd - > /dev/null;
	cp -u $(LIB)/include/lzo/* $(BCM_FSBUILD_DIR)/gpl/include/lzo

else
conditional_build: sanity_check
	@echo "skipping $(LIB) (not configured)"
endif
