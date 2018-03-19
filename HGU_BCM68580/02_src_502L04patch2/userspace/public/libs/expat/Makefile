#***********************************************************************
#
#  Copyright (c) 2014  Broadcom Corporation
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

ifeq ($(strip $(DESKTOP_LINUX)),y)
BCM_BLT32_FLAGS := "CFLAGS=$(BCM_LD_FLAGS)" "CXXFLAGS=$(BCM_LD_FLAGS)" "LDFLAGS=$(BCM_LD_FLAGS)"
endif

# Download from http://sourceforge.net/projects/expat/files/expat/2.1.0/expat-2.1.0.tar.gz/download
APP = expat-2.1.0
LIB = libexpat
INCLUDE_DIR = $(BCM_FSBUILD_DIR)/public/include/expat

.PHONY: check_untar_patch_configure conditional_build check_versions

ifneq ($(strip $(DO_BUILD_EXPAT)),)
conditional_build: $(APP)

# Depend on tar file
# Use $(APP)/configure to mark tar progress is completed
$(APP)/configure: $(APP).tar.gz
	if [ ! -e $(APP) ]; then \
	    mkdir $(APP); cd $(APP); \
	    tar --strip-components=1 -xzf ../$(APP).tar.gz; \
	    cd ..; \
	    touch -c $(APP)/configure; \
	    patch -p1 -b -s -d $(APP) < $(APP).patch; \
	fi;
	@echo "$(APP) is untarred"

# Depend on $(APP)/configure so that tar progress should be done only once
$(APP)/Makefile: $(APP)/configure
	if [ -e $(APP) ]; then \
	    cd $(APP); \
	    ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes \
	    ./configure --enable-static=no --host=$(TOOLCHAIN_PREFIX) --prefix=$(BCM_FSBUILD_DIR)/public/ $(BCM_BLT32_FLAGS); \
	    cd ..; \
	fi;
	@echo "$(APP) is configured"

# Depend on $(APP)/Makefile so that configure progress should be done only once
check_config: $(APP)/Makefile

# Depend on check_config so that make progress can continue where it is stopped before
$(APP): check_config
	$(MAKE) -C $(APP) ;
	$(MAKE) -C $(APP) install;
	-mkdir -p $(INSTALL_DIR)/lib/public/;
	cp -d $(BCM_FSBUILD_DIR)/public/lib/$(LIB).so* $(INSTALL_DIR)/lib/public/;
	$(STRIP) $(INSTALL_DIR)/lib/public/$(LIB).so*;
	if [ ! -e $(INCLUDE_DIR) ]; then \
	    mkdir $(INCLUDE_DIR); \
	fi;
	cp -d $(BCM_FSBUILD_DIR)/public/include/expat*.* $(INCLUDE_DIR)/.;
	rm -f $(BCM_FSBUILD_DIR)/public/include/expat*.*;
	rm -f $(BCM_FSBUILD_DIR)/public/lib/$(LIB).*;
	@echo "Done building $(APP)"

else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif

# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean:
	rm -f $(INSTALL_DIR)/lib/public/$(LIB).so*;
	rm -rf $(APP) $(INCLUDE_DIR);

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

bcm_dorel_distclean: distclean

shell:
	bash -i
