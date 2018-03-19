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

APP = tablesorter
# http://tablesorter.com/__jquery.tablesorter.zip
TABLE_SORTER_TAR_FILE := tablesorter.tar.bz2
TABLE_SORTER_JS_FILE := jquery.tablesorter.min.js

.PHONY: conditional_build clean distclean

ifneq ($(strip $(BUILD_JQPLOT)),)
conditional_build: $(APP)

$(APP)/$(TABLE_SORTER_JS_FILE): $(TABLE_SORTER_TAR_FILE)
	if [ ! -e $(APP) ]; then \
	   tar xkjf $(TABLE_SORTER_TAR_FILE); \
	   touch -c $(APP)/$(TABLE_SORTER_JS_FILE); \
	fi;
	@echo "$(APP) is untarred"

$(APP): $(APP)/$(TABLE_SORTER_JS_FILE)
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

