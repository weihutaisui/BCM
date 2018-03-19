#***********************************************************************
#
#  Copyright (c) 2012  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/

default: conditional_build 

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

APP = miniupnpc
LIB = libminiupnpc.so
INCLUDE_DIR= $(BCM_FSBUILD_DIR)/public/include/miniupnpc

ifneq ($(strip $(BUILD_LIBMINIUPNPC)),)
conditional_build: $(APP)
else
conditional_build:
	@echo "skipping $(APP) (not configured)"
endif

check_untar:
ifeq ($(wildcard miniupnpc/Makefile),)
	mkdir -p $(APP)
	cd $(APP) ; tar -xzf ../miniupnpc-1.9.20150206.tar.gz
endif
	@echo "$(APP) is untarred"

$(APP): check_untar	
	$(MAKE) -C $(APP);
	mkdir -p $(INSTALL_DIR)/lib/public/;
	cp -T $(APP)/$(LIB) $(INSTALL_DIR)/lib/public/$(LIB);
	$(STRIP) $(INSTALL_DIR)/lib/public/$(LIB);
	if [ ! -e $(INCLUDE_DIR) ]; then \
	    mkdir $(INCLUDE_DIR); \
	fi;
	cp $(APP)/*.h $(INCLUDE_DIR)/.;
	@echo "Done building $(APP)"

# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean:
	rm -f $(INSTALL_DIR)/lib/public/$(LIB);
	rm -rf $(APP) $(INCLUDE_DIR);

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

bcm_dorel_distclean: distclean

shell:
	bash -i
