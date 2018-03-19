
libjsonc: conditional_build 

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

ifeq ($(strip $(DESKTOP_LINUX)),y)
BCM_BLT32_FLAGS := "CFLAGS=$(BCM_LD_FLAGS)" "CXXFLAGS=$(BCM_LD_FLAGS)" "LDFLAGS=$(BCM_LD_FLAGS)"
endif

ifeq ($(strip $(BRCM_USER_SSP)),y)
CFLAGS+=$(SSP_TYP_COMPILER_OPTS)
LDFLAGS+=-L$(INSTALL_DIR)/lib $(SSP_LIBS)
endif


export LINUX_VER_STR TOOLCHAIN_PREFIX


ifneq ($(strip $(BUILD_LIBJSONC)),)
conditional_build: all
else
conditional_build:
	@echo "skipping libjsonc (not configured)"
endif



jsonc/configure: json-c-0.11.tar.gz 
	mkdir -p jsonc
	(cd jsonc ; tar --strip-components=1  -xkzf ../json-c-0.11.tar.gz ; true)
	patch -p0 < jsonc_config.patch
	touch -c jsonc/configure
	@echo "jsonc is untarred"

check_config: jsonc/Makefile

jsonc/Makefile: jsonc/configure
	cd jsonc; ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes \
		./configure --host=$(TOOLCHAIN_PREFIX) \
		--prefix=$(BCM_FSBUILD_DIR)/public/ --enable-static=no $(BCM_BLT32_FLAGS)

all: check_config
	mkdir -p $(INSTALL_DIR)/lib/
	cd jsonc ; make
	cd jsonc ; make install
	cp -d $(BCM_FSBUILD_DIR)/public/lib/libjson*.so* $(INSTALL_DIR)/lib/
	$(STRIP) $(INSTALL_DIR)/lib/libjson*.so*

clean:
	rm -f $(INSTALL_DIR)/lib/libjson*.so*
	-[ ! -e jsonc/Makefile ] || (make -C jsonc clean; make -C jsonc uninstall)
	-rm -rf jsonc


bcm_dorel_distclean: distclean

distclean: 
	rm -rf jsonc

shell:
	bash -i

