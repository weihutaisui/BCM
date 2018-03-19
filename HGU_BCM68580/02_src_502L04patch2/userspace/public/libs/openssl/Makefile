
openssl: conditional_build 

.PHONY: all distclean clean conditional_build openssl

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

PATCH_FILE = bcm_patches/lib_config.patch
TOP_MAKE_PATCH_FILE :=bcm_patches/top-makefile.patch
TOP_MAKE_SHARED_PATCH_FILE :=bcm_patches/top-makefile.shared.patch
CRYPTO_MAKE_PATCH_FILE :=bcm_patches/crypto-makefile.patch
CRYPTO_OBJECTS_MAKE_PATCH_FILE :=bcm_patches/crypto-objects-makefile.patch
ENGINES_MAKE_PATCH_FILE :=bcm_patches/engines-makefile.patch
ARMCAP_PATCH_FILE :=bcm_patches/armcap.patch

PATCH_COMPLETE_FILE = .patch_complete
UNTAR_COMPLETE_FILE = .untar_complete
CONFIG_ID_FILE = BRCM_CONFIG_ID_$(TOOLCHAIN_PREFIX)_$(LINUX_VER_STR)

ifeq ($(strip $(DO_BUILD_OPENSSL)),y)
conditional_build: all
else
conditional_build:
	@echo "skipping openssl 1.0.2a (not configured)"
endif

#clear CC to avoid conflict with openssl configuration
CC=

# APP must match the openssl folder name
APP=openssl-1.0.2a

SHLIBCMD= grep -Po '(?<=SHLIB_VERSION_NUMBER=).*' $(APP)/Makefile

# the options are ordered for the configure script
ifeq ($(findstring mips,$(PROFILE_ARCH)),mips)
SSLOPTIONS := linux-bcmmips
else 
ifeq ($(strip $(PROFILE_ARCH)),aarch64)
SSLOPTIONS := linux-aarch64
else
SSLOPTIONS := linux-bcmarm
endif
endif
ifeq ($(strip $(DESKTOP_LINUX)),y)
ifeq ($(strip $(ARCH)),x86_64)
SSLOPTIONS := linux-x86_64
else
SSLOPTIONS := $(BCM_LD_FLAGS) linux-generic32
endif
endif

SSLOPTIONS += --prefix=/usr/local/ssl --openssldir=/etc/ssl
SSLOPTIONS += no-hw shared no-md2 no-md4 no-mdc2 no-ripemd no-rc2 no-krb5 no-idea no-unit-test no-cast no-bf

ifeq ($(strip $(BUILD_TR69C_SSL)$(BUILD_TR69C_BASIC_SSL)$(BUILD_HTTPD_SSL)$(BUILD_HTTPD_BASIC_SSL)),)
SSLOPTIONS += no-rc4
endif

ifneq ($(strip $(BENCHMARKS_SMP)),y)
SSLOPTIONS += no-threads no-zlib no-speed
endif

ifeq ($(strip $(BUILD_HTTPD_SSL)$(BUILD_HTTPD_BASIC_SSL)),)
SSLOPTIONS += no-rc5 no-err no-ssl2 -DOPENSSL_NO_HEARTBEATS
endif

ifeq ($(strip $(ARCH_ENDIAN)),little)
SSLOPTIONS += -DL_ENDIAN
else
SSLOPTIONS += -DB_ENDIAN 
endif

ifneq ($(strip $(BUILD_CRYPTODEV_LINUX)),)
SSLOPTIONS += -DHAVE_CRYPTODEV -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)
else
SSLOPTIONS += no-engine
endif

SSLOPTIONS += $(BRCM_COMMON_CFLAGS) $(SSP_TYP_COMPILER_OPTS) $(SSP_TYP_LIBS) $(SSP_LIB_INC)

$(APP)/$(UNTAR_COMPLETE_FILE): $(PATCH_FILE) $(TOP_MAKE_PATCH_FILE) \
	$(TOP_MAKE_SHARED_PATCH_FILE) $(CRYPTO_MAKE_PATCH_FILE) \
	$(CRYPTO_OBJECTS_MAKE_PATCH_FILE) $(ENGINES_MAKE_PATCH_FILE) $(APP).tar.gz
	rm -rf $(APP)
	tar xzf $(APP).tar.gz
	-ln -s ../../crypto/rc5/rc5.h $(APP)/include/openssl/rc5.h
	@echo "$(APP) is untarred"
	-[ -e $(APP)/Makefile ] && $(MAKE) -j 1 -C $(APP) dclean
	patch -p1 -d$(APP) < $(PATCH_FILE)
	patch -p1 -d$(APP) < $(TOP_MAKE_PATCH_FILE)
	patch -p1 -d$(APP) < $(TOP_MAKE_SHARED_PATCH_FILE)
	patch -p1 -d$(APP) < $(CRYPTO_MAKE_PATCH_FILE) 
	patch -p1 -d$(APP) < $(CRYPTO_OBJECTS_MAKE_PATCH_FILE)
	patch -p1 -d$(APP) < $(ENGINES_MAKE_PATCH_FILE)
ifeq ($(strip $(BUILD_OPENSSL_NEON)),)
	patch -p1 -d$(APP) < $(ARMCAP_PATCH_FILE)
endif
	@echo "$(APP) is patched"
	touch $(APP)/$(UNTAR_COMPLETE_FILE)

$(APP)/$(CONFIG_ID_FILE): $(APP)/$(UNTAR_COMPLETE_FILE)
	@echo "$(APP) is being configured"
	#beware: Configure modifies the Makefile
	(cd $(APP); ./Configure $(SSLOPTIONS);)
	$(MAKE) -j1 -C $(APP) depend
	rm -f $(APP)/BRCM_CONFIG_ID_*
	touch $(APP)/$(CONFIG_ID_FILE)

ifdef NO_SSL_PARABUILD
NOPARABUILD :=-j1
endif

all: $(APP)/$(CONFIG_ID_FILE)
	$(MAKE) $(NOPARABUILD) -C $(APP) all
ifneq ($(strip $(BUILD_IPSEC_TOOLS)$(BUILD_CERT)),)
	mkdir -p $(INSTALL_DIR)/bin
	mkdir -p $(INSTALL_DIR)/etc/ssl
	install -m 755 $(APP)/apps/openssl $(INSTALL_DIR)/bin
	install -m 755 $(APP)/apps/openssl.cnf $(INSTALL_DIR)/etc/ssl
endif
	mkdir -p $(INSTALL_DIR)/lib
	mkdir -p $(BCM_FSBUILD_DIR)/lib
	@SHLIB_VERSION_NUMBER=$$(grep -Po '(?<=SHLIB_VERSION_NUMBER=).*' ./$(APP)/Makefile ); \
	echo "install -m 755 $(APP)/libssl.so.$$SHLIB_VERSION_NUMBER $(INSTALL_DIR)/lib"; \
	install -m 755 $(APP)/libssl.so.$$SHLIB_VERSION_NUMBER $(INSTALL_DIR)/lib; \
	cd $(INSTALL_DIR)/lib; rm -f libssl.so; ln -s libssl.so.$$SHLIB_VERSION_NUMBER libssl.so; cd - > /dev/null; \
	echo "install -m 755 $(APP)/libcrypto.so.$$SHLIB_VERSION_NUMBER $(INSTALL_DIR)/lib"; \
	install -m 755 $(APP)/libcrypto.so.$$SHLIB_VERSION_NUMBER $(INSTALL_DIR)/lib; \
	cd $(INSTALL_DIR)/lib; rm -f libcrypto.so; ln -s libcrypto.so.$$SHLIB_VERSION_NUMBER libcrypto.so; cd - > /dev/null; \
	echo "cp -u $(APP)/libssl.so.$$SHLIB_VERSION_NUMBER $(BCM_FSBUILD_DIR)/lib"; \
	cp -u $(APP)/libssl.so.$$SHLIB_VERSION_NUMBER $(BCM_FSBUILD_DIR)/lib; \
	cd $(BCM_FSBUILD_DIR)/lib; rm -f libssl.so; ln -s libssl.so.$$SHLIB_VERSION_NUMBER libssl.so; cd - > /dev/null; \
	echo "cp -u $(APP)/libcrypto.so.$$SHLIB_VERSION_NUMBER $(BCM_FSBUILD_DIR)/lib"; \
	cp -u $(APP)/libcrypto.so.$$SHLIB_VERSION_NUMBER $(BCM_FSBUILD_DIR)/lib; \
	cd $(BCM_FSBUILD_DIR)/lib; rm -f libcrypto.so; ln -s libcrypto.so.$$SHLIB_VERSION_NUMBER libcrypto.so;
	mkdir -p $(BCM_FSBUILD_DIR)/public/include/openssl
	cp -u $(APP)/include/openssl/* $(BCM_FSBUILD_DIR)/public/include/openssl

clean:
	rm -rf $(APP)
	rm -f $(INSTALL_DIR)/lib/libssl.so*
	rm -f $(INSTALL_DIR)/lib/libcrypto.so*
	rm -f $(INSTALL_DIR)/bin/openssl
	rm -rf $(INSTALL_DIR)/etc/ssl
	rm -f $(BCM_FSBUILD_DIR)/lib/libssl.so*
	rm -f $(BCM_FSBUILD_DIR)/lib/libcrypto.so*
	rm -f $(BCM_FSBUILD_DIR)/public/include/openssl/*

distclean: clean

shell:
	bash -i

