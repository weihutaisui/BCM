#***********************************************************************
#
#  Copyright (c) 2017  Broadcom Corporation
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

KBUILD_OUTPUT:=$(TOOLCHAIN_INCLUDE_DIR)/../../
export KBUILD_OUTPUT

APP = linuxptp-1.8

.PHONY: check_untar_patch_configure conditional_build 

check_untar_patch_configure: 
	@if [ ! -e $(APP)/Makefile ]; then \
	    echo "Untarring original $(APP) source"; \
	    (tar xkzf $(APP).tgz 2> /dev/null || true); \
	    fi

conditional_build: check_untar_patch_configure
	$(MAKE) -C $(APP)/ ptp4l
	install -m 755 $(APP)/ptp4l $(INSTALL_DIR)/bin
	install -m 755 brcm.cfg $(INSTALL_DIR)/bin


# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean:
	rm -rf $(APP)

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

