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

APP = ntfs-3g_ntfsprogs-2016.2.22
SO_VER = 87


.PHONY: check_untar_patch_configure conditional_build check_versions

ifneq ($(strip $(BUILD_NTFS_3G)),)
conditional_build: $(APP)

# Depend on tar file
# Use $(APP)/configure to mark tar progress is completed
$(APP)/configure: $(APP).tgz
	if [ ! -e $(APP) ]; then \
	    mkdir $(APP); cd $(APP); \
	    tar --strip-components=1 -xkf ../$(APP).tgz; \
	    cd ..; \
	    touch -c $(APP)/configure; \
	    echo "Applying patches to $(APP)"; \
	    patch -p1 -b -s -d$(APP) < $(APP).patch; \
	fi;
	@echo "$(APP) is untarred"

# Depend on $(APP)/configure so that tar progress should be done only once
$(APP)/Makefile: $(APP)/configure
	if [ -e $(APP) ]; then \
	    cd $(APP); \
	    ./configure --enable-static=no --host=$(TOOLCHAIN_PREFIX) --prefix=$(BCM_FSBUILD_DIR)/gpl/ \
	    --disable-ntfsprogs --disable-dependency-tracking --target=$(TOOLCHAIN_PREFIX) --sbindir=$(INSTALL_DIR)/bin; \
	    cd ..; \
	fi;
	@echo "$(APP) is configured"

# Depend on $(APP)/Makefile so that configure progress should be done only once
check_config: $(APP)/Makefile

# Depend on check_config so that make progress can continue where it is stopped before
$(APP): check_config
	$(MAKE) -C $(APP) ;
	mkdir -p $(INSTALL_DIR)/lib/gpl/;
	install -m 755 $(APP)/libntfs-3g/.libs/libntfs-3g.so $(INSTALL_DIR)/lib/gpl/libntfs-3g.so.$(SO_VER)
	$(STRIP) $(INSTALL_DIR)/lib/gpl/libntfs-3g.so.$(SO_VER)
	install -m 755 $(APP)/src/.libs/ntfs-3g  $(INSTALL_DIR)/bin/ntfs-3g	
	$(STRIP) $(INSTALL_DIR)/bin/ntfs-3g
	@echo "Done building $(APP)"
	
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif


# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean:
	rm -f $(INSTALL_DIR)/lib/gpl/libntfs-3g.so.$(SO_VER)
	rm -f $(INSTALL_DIR)/bin/ntfs-3g	
	rm -rf $(APP)

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

bcm_dorel_distclean: distclean

shell:
	bash -i
