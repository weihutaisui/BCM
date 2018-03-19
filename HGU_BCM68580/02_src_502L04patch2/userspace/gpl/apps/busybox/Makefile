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

export INC_KERNEL_BASE

APP = busybox-1.27.2


.PHONY: check_untar_patch_configure conditional_build check_versions

ifneq ($(strip $(BUILD_BUSYBOX)),)
conditional_build: $(APP) $(APP)_vanilla

CMS_BUILD=0
BUILD_BUSYBOX_STATIC_FLAG=n
ifneq ($(strip $(BUILD_BRCM_CMS)),)
CMS_BUILD=1
BUILD_BUSYBOX_STATIC_FLAG=y
endif


# Depend on tar file
# Use $(APP)/Makefile to mark tar progress is completed
$(APP)/.config: $(APP).tar.bz2
	if [ ! -e $(APP) ]; then \
	    mkdir $(APP); cd $(APP); \
	    tar --strip-components=1 -xkf ../$(APP).tar.bz2; \
	    cd ..; \
	if [ $(CMS_BUILD) -eq 1 ]; then \
		echo "*********************Applying patches to $(APP)"; \
		patch -p1 -b -s -d$(APP) < $(APP).patch; \
	fi; \
	fi;
	@echo "$(APP) is untarred"

# Depend on $(APP)/configure so that tar progress should be done only once
$(APP)/Makefile: $(APP)/.config
	if [ -e $(APP) ]; then \
	    cat brcm.config > $(APP)/.config; \
	fi;
	@echo "$(APP) is configured"

# Depend on $(APP)/Makefile so that configure progress should be done only once
check_config: $(APP)/Makefile

# Depend on check_config so that make progress can continue where it is stopped before
$(APP): check_config
	for i in config.d/[0-9]*.conf ; do echo "config script $$i" ; BUSYBOX_VANILLA_BUILD=n BRCM_EXTRA_PATH=/opt perl $$i $(APP)/.config; done;
	$(MAKE) -C $(APP) include/autoconf.h
	cd $(APP) && rm -f Config.h && ln -sf include/autoconf.h Config.h && touch Config.h
	BRCM_LIBS="cms_msg cms_util bcm_flashutil bcm_crc dl bcm_boardctl resolv" $(MAKE) -C $(APP) prepare
	BRCM_LIBS="cms_msg cms_util bcm_flashutil bcm_crc dl bcm_boardctl resolv" $(MAKE) -C $(APP) install

	@echo "Done building $(APP)"

# Depend on tar file
# Use $(APP)/Makefile to mark tar progress is completed
$(APP)_vanilla/.config: $(APP).tar.bz2
	if [ ! -e $(APP)_vanilla ]; then \
	    mkdir $(APP)_vanilla; cd $(APP)_vanilla; \
	    tar --strip-components=1 -xkf ../$(APP).tar.bz2; \
	    cd ..; \
	fi;
	@echo "$(APP)_vanilla is untarred"

# Depend on $(APP)_vanilla/configure so that tar progress should be done only once
$(APP)_vanilla/Makefile: $(APP)_vanilla/.config
	if [ -e $(APP)_vanilla ]; then \
	    cat brcm.config > $(APP)_vanilla/.config; \
	fi;
	@echo "$(APP)_vanilla is configured"

# Depend on $(APP)/Makefile so that configure progress should be done only once
check_config_vanilla: $(APP)_vanilla/Makefile

# Depend on check_config so that make progress can continue where it is stopped before
$(APP)_vanilla: check_config_vanilla
	@echo "Building vanilla"
	for i in config.d/[0-9]*.conf ; do echo "config script $$i" ; BUSYBOX_VANILLA_BUILD=y BUILD_BUSYBOX_STATIC=$(BUILD_BUSYBOX_STATIC_FLAG) perl $$i $(APP)_vanilla/.config; done;
	$(MAKE) -C $(APP)_vanilla include/autoconf.h
	cd $(APP)_vanilla && rm -f Config.h && ln -sf include/autoconf.h Config.h && touch Config.h
	$(MAKE) -C $(APP)_vanilla prepare
	$(MAKE) -C $(APP)_vanilla install
	@echo "Done building $(APP)_vanilla"
	
	
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif

# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean:
	rm -rf $(APP) $(APP)_vanilla

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

bcm_dorel_distclean: distclean

shell:
	bash -i
