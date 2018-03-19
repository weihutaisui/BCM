
libpcap: conditional_build 

# BRCM_SUPPORTS_MULTIARCH_BUILD

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

export LINUX_VER_STR TOOLCHAIN_PREFIX


ifneq ($(strip $(BUILD_LIBPCAP)),)
conditional_build: all
else
conditional_build:
	@echo "skipping libpcap (not configured)"
endif


libpcap/configure: libpcap.tar.gz
	mkdir -p libpcap
	cd libpcap ; tar --strip-components=1 -xzf ../libpcap.tar.gz
	touch -c libpcap/configure
	@echo "libpcap is untarred"

check_config: objs/$(PROFILE_ARCH)/Makefile

objs/$(PROFILE_ARCH)/Makefile: libpcap/configure
	mkdir -p objs/$(PROFILE_ARCH)
	cd objs/$(PROFILE_ARCH) ; ac_cv_linux_vers=$(LINUX_VER_STR)  ../../libpcap/configure --host=$(TOOLCHAIN_PREFIX) --with-pcap=linux --prefix=$(BCM_FSBUILD_DIR)/public/

all: check_config
	mkdir -p $(INSTALL_DIR)/lib/public/
	cd objs/$(PROFILE_ARCH) ; make
	cd objs/$(PROFILE_ARCH) ; make install
	mkdir -p  $(INSTALL_DIR)/lib$(BCM_INSTALL_SUFFIX_DIR)
	cp -d $(BCM_FSBUILD_DIR)/public/lib/libpcap.so* $(INSTALL_DIR)/lib$(BCM_INSTALL_SUFFIX_DIR)

clean:
	rm -f $(INSTALL_DIR)/lib/public/libpcap.so*
	-[ ! -e objs/$(PROFILE_ARCH)/Makefile ] || make objs/$(PROFILE_ARCH) uninstall 
	rm -rf objs

bcm_dorel_distclean: distclean

distclean: clean

shell:
	bash -i

