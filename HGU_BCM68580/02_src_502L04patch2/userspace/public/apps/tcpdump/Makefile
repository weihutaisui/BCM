
tcpdump: conditional_build 

# BRCM_SUPPORTS_MULTIARCH_BUILD

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

export LINUX_VER_STR TOOLCHAIN_PREFIX
export CFLAGS LDFLAGS
CFLAGS=-I$(BCM_FSBUILD_DIR)/public/include -I$(TOOLCHAIN_INCLUDE_DIR) -isystem $(TOOLCHAIN_INCLUDE_DIR) -I$(CURR_DIR)/tcpdump
# -DNDEBUG=1 -O0
LDFLAGS=-L$(BCM_FSBUILD_DIR)/public/lib 



ifneq ($(strip $(BUILD_TCPDUMP)),)
conditional_build: all
else
conditional_build:
	@echo "skipping tcpdump (not configured)"
endif


tcpdump/configure: tcpdump.tar.gz
	mkdir -p tcpdump
	cd tcpdump ; tar --strip-components=1 -xzf ../tcpdump.tar.gz
	patch -p0 < patch.tcpdump_cross
	touch -c tcpdump/configure
	@echo "tcpdump is untarred"

check_config: objs/$(PROFILE_ARCH)/Makefile

objs/$(PROFILE_ARCH)/Makefile: tcpdump/configure
	mkdir -p objs/$(PROFILE_ARCH)
	cd objs/$(PROFILE_ARCH) ; ac_cv_linux_vers=$(LINUX_VER_STR)  ac_cv_path_PCAP_CONFIG=$(BCM_FSBUILD_DIR)/public/bin/pcap-config ../../tcpdump/configure --host=$(TOOLCHAIN_PREFIX) --with-pcap=linux --prefix=$(BCM_FSBUILD_DIR)/public

all: check_config
	$(MAKE) -C objs/$(PROFILE_ARCH)
	$(MAKE) -C objs/$(PROFILE_ARCH) install
	mkdir -p $(INSTALL_DIR)/bin$(BCM_INSTALL_SUFFIX_DIR)
	cp -d $(BCM_FSBUILD_DIR)/public/sbin/tcpdump $(INSTALL_DIR)/bin$(BCM_INSTALL_SUFFIX_DIR)

clean:
	rm -f $(INSTALL_DIR)/bin/tcpdump
	rm -f $(INSTALL_DIR)/*/bin/tcpdump
	-[ ! -e objs/$(PROFILE_ARCH) ] || $(MAKE) -C objs/$(PROFILE_ARCH) clean uninstall distclean
	rm -rf objs

bcm_dorel_distclean: distclean

distclean: clean

shell:
	bash -i

