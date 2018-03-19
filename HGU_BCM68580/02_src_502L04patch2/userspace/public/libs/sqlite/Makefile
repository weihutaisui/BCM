
sqlite: conditional_build 

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

export LINUX_VER_STR TOOLCHAIN_PREFIX

ifeq ($(strip $(BRCM_USER_SSP)),y)
CFLAGS+=$(SSP_TYP_COMPILER_OPTS)
LDFLAGS+=-L$(INSTALL_DIR)/lib $(SSP_LIBS)
endif

export CFLAGS
export LDFLAGS

# CFLAGS += -Os



ifneq ($(strip $(BUILD_LIBSQLITE)),)
conditional_build: all
else
conditional_build:
	@echo "skipping libsqlite (not configured)"
endif


check_untar:
ifeq ($(wildcard sqlite/configure),)
	mkdir -p sqlite
	cd sqlite ; tar --strip-components=1 -xzf ../sqlite-autoconf-3080600.tar.gz;\
	patch -p1 -b -N -s -d . < ../sqlite.patch;
	patch -p0 < sqlite_config.patch
endif
	@echo "sqlite is untarred"

check_config: check_untar
ifeq ($(wildcard objs/$(PROFILE_ARCH)/Makefile),)
	mkdir -p objs/$(PROFILE_ARCH)
	cd objs/$(PROFILE_ARCH) ; ../../sqlite/configure --host=$(TOOLCHAIN_PREFIX) \
	 --prefix=$(BCM_FSBUILD_DIR)/public/ --enable-static=no
endif

all: check_config
	mkdir -p $(INSTALL_DIR)/lib/
	cd objs/$(PROFILE_ARCH) ; $(MAKE)
	cd objs/$(PROFILE_ARCH) ; $(MAKE) install
	cp -d $(BCM_FSBUILD_DIR)/public/lib/libsqlite*.so* $(INSTALL_DIR)/lib/
	$(STRIP) $(INSTALL_DIR)/lib/libsqlite*.so*
	cp -d $(BCM_FSBUILD_DIR)/public/bin/sqlite* $(INSTALL_DIR)/bin/
	$(STRIP) $(INSTALL_DIR)/bin/sqlite*

clean:
	rm -f $(INSTALL_DIR)/lib/libsqlite*.so*
	rm -rf objs

bcm_dorel_distclean: distclean

distclean: 
	rm -rf objs

shell:
	bash -i

