#***********************************************************************
#
#  Copyright (c) 2012  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/

APP = jamvm-1.5.4

all dynamic install: conditional_build

clean:
	rm -rf $(APP)

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

.PHONY: check_untar_patch_configure conditional_build

ifneq ($(strip $(BUILD_JVM_JAMVM)),)
ifeq ($(ARCH),mips)
check_untar_patch_configure: sanity_check
	@if [ ! -e $(APP)/README ]; then \
		echo "Untarring original $(APP) source"; \
		(tar xkfz $(APP).tar.gz 2> /dev/null || true); \
		echo "Applying patch to $(APP)"; \
		patch -p1 -b -d$(APP) <bcm_patches/configure_mips_linux_installdir.patch; \
		echo "Checking configure on $(APP)/configure for MIPS"; \
		if [ -e $(APP)/configure ]; then \
			echo "Running configure on $(APP)"; \
			echo "building to $(INSTALL_DIR)"; \
			cd $(APP); \
			./configure --host=mips-linux \
			--target=mips-linux-uclibc \
			--prefix=$(INSTALL_DIR)/usr/local/jamvm \
			--runtimeinstalldir=/usr/local/jamvm \
			--disable-dependency-tracking \
			--disable-zip \
			--with-classpath-install-dir=/usr/local/classpath; \
		fi \
	fi

conditional_build: check_untar_patch_configure
	cd $(APP)/src; (ln -s arch/mips.h arch.h 2>/dev/null || true)
	$(MAKE) -C $(APP) install

else
check_untar_patch_configure: sanity_check
	@if [ ! -e $(APP)/README ]; then \
		echo "Untarring original $(APP) source"; \
		(tar xkfz $(APP).tar.gz 2> /dev/null || true); \
		echo "Applying patch to $(APP)"; \
		patch -p1 -b -d$(APP) <bcm_patches/configure_mips_linux_installdir.patch; \
		echo "Checking configure on $(APP)/configure"; \
		if [ -e $(APP)/configure ]; then \
			echo "Running configure on $(APP) for ARM"; \
			echo "building to $(INSTALL_DIR)"; \
			cd $(APP); \
			./configure --host=arm-linux \
			--target=arm-unknown-linux-uclibcgnueabi \
			--prefix=$(INSTALL_DIR)/usr/local/jamvm \
			--runtimeinstalldir=/usr/local/jamvm \
			--disable-dependency-tracking \
			--disable-zip \
			--with-classpath-install-dir=/usr/local/classpath; \
		fi \
	fi

conditional_build: check_untar_patch_configure
	$(MAKE) -C $(APP) install

endif

else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif
