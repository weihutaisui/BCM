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

UNTAR_COMPLETE_FILE = .untar_complete
CONFIG_COMPLETE_FILE = config_complete

APP = redsocks

ifneq ($(strip $(BUILD_MPTCP)),)

$(APP)/$(UNTAR_COMPLETE_FILE): $(APP).tar.gz
	rm -rf $(APP)
	mkdir $(APP) && tar --strip-components=1 -xf $(APP).tar.gz -C $(APP)
	echo "Applying patches to $(APP)"
	patch -p1 -b -s -d$(APP) < $(APP).patch
	@echo "$(APP) is untarred"
	touch $(APP)/$(UNTAR_COMPLETE_FILE)

conditional_build: $(APP)/$(UNTAR_COMPLETE_FILE) sanity_check
	$(MAKE) -C $(APP)
	install -m 755 $(APP)/redsocks $(INSTALL_DIR)/bin/redsocks
	echo "Done building $(APP)"
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif

# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean: 
	rm -f $(INSTALL_DIR)/bin/redsocks
	rm -rf $(APP)

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

shell:
	bash

