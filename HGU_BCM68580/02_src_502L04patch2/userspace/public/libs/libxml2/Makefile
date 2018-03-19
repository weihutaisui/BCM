
libxml2: conditional_build 

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

export LINUX_VER_STR TOOLCHAIN_PREFIX


ifneq ($(strip $(BUILD_LIBXML2)),)
conditional_build: all
else
conditional_build:
	@echo "skipping libxml2 (not configured)"
endif


libxml2/configure: libxml2-sources-2.9.0.tar.gz
	mkdir -p libxml2
	cd libxml2 ; tar --strip-components=1 -xzf ../libxml2-sources-2.9.0.tar.gz
	patch -p0 < libxml_config.patch
	touch -c libxml2/configure
	@echo "libxml2 is untarred"

check_config: libxml2/Makefile

libxml2/Makefile: libxml2/configure
	cd libxml2 ; ./configure --host=$(TOOLCHAIN_PREFIX) --with-python=no --enable-static=no --without-history --without-legacy --without-zlib --without-http --without-ftp --without-debug --without-docbook --prefix=$(BCM_FSBUILD_DIR)/public/

all: check_config
	mkdir -p $(INSTALL_DIR)/lib/
	cd libxml2 ; $(MAKE) 
	cd libxml2 ; $(MAKE) install
	cp -d $(BCM_FSBUILD_DIR)/public/lib/libxml2*.so* $(INSTALL_DIR)/lib/

clean:
	-rm -f $(INSTALL_DIR)/lib/libxml2*.so*
	-rm -rf libxml2

bcm_dorel_distclean: distclean

distclean:
	-rm -rf libxml2

shell:
	bash -i

