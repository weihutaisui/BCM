#***********************************************************************
#
#  Copyright (c) 2016  Broadcom Corporation
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

APP = treetable
# http://github.com/ludo/jquery-treetable/zipball/3.2.0
TREE_TABLE_TAR_FILE := treetable-3.2.0.tar.gz
TREE_TABLE_JS_FILE := jquery.treetable.js

.PHONY: conditional_build clean distclean

ifneq ($(strip $(BUILD_JQPLOT)),)
conditional_build: $(APP)

$(APP)/$(TREE_TABLE_JS_FILE): $(TREE_TABLE_TAR_FILE)
	if [ ! -e $(APP) ]; then \
	   tar xkzf $(TREE_TABLE_TAR_FILE); \
	   touch -c $(APP)/$(TREE_TABLE_JS_FILE); \
	fi;
	@echo "$(APP) is untarred"

$(APP): $(APP)/$(TREE_TABLE_JS_FILE)
	$(MAKE) -f Makefile.bcm;

else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif

clean:
	if [ -e $(APP) ]; then \
	    $(MAKE) -f Makefile.bcm clean; \
	fi;

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

bcm_dorel_distclean: distclean

shell:
	bash -i

