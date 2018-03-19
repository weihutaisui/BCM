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

# Download from https://github.com/strophe/libstrophe
APP = libstrophe
LIB = libstrophe.so
INCLUDE_DIR = $(BCM_FSBUILD_DIR)/public/include/strophe

.PHONY: check_untar_patch_configure conditional_build check_versions

ifneq ($(strip $(BUILD_TR69C_SSL)),)
ifneq ($(strip $(BUILD_XMPP)),)
conditional_build: $(APP)

# Depend on tar file
# Use $(APP)/src/Makefile to mark tar progress is completed
$(APP)/src/Makefile: $(APP).tar.gz
	if [ ! -e $(APP) ]; then \
	    mkdir $(APP); cd $(APP); \
	    tar --strip-components=1 -xzf ../$(APP).tar.gz; \
	    cp -d ../Makefile.bcm src/Makefile; \
	    cd ..; \
	    touch -c $(APP)/src/Makefile; \
	fi;
	@echo "$(APP) is untarred"

# Depend on $(APP)/src/Makefile so that tar progress should be done only once
check_config: $(APP)/src/Makefile

# Depend on check_config so that make progress can continue where it is stopped before
$(APP): check_config
	$(MAKE) -C $(APP)/src/;
	mkdir -p $(INSTALL_DIR)/lib/
	cp -T $(APP)/src/$(LIB) $(INSTALL_DIR)/lib/$(LIB);
	$(STRIP) $(INSTALL_DIR)/lib/$(LIB);
	if [ ! -e $(INCLUDE_DIR) ]; then \
	    mkdir $(INCLUDE_DIR); \
	fi;
	cp $(APP)/strophe.h $(INCLUDE_DIR)/.;
	@echo "Done building $(APP)"

else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif
else
conditional_build: sanity_check
	@echo "skipping $(APP) (OpenSSL not configured)"
endif

# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean:
	rm -f $(INSTALL_DIR)/lib/$(LIB);
	rm -rf $(APP) $(INCLUDE_DIR);

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

bcm_dorel_distclean: distclean

shell:
	bash -i
