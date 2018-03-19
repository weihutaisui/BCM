#***********************************************************************
#
#  Copyright (c) 2015  Broadcom Corporation
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

APP = qrcode
WEBS_DIR = $(INSTALL_DIR)/webs

.PHONY: conditional_build clean distclean

ifneq ($(strip $(BUILD_QRCODE)),)
conditional_build:
	@echo "done $@"

#--------------------------------------------------------------------------------
# eval call CREATE_WEB_TRG creates the makefile target install_web_files.  This
# target is used to install and minimize web files.   See make.common for details
all dynamic install: install_web_files
$(eval $(call CREATE_WEB_TRG, install_web_files, $(APP), $(WEBS_DIR)/qrcode, staging, minimized_webs.tar))
#--------------------------------------------------------------------------------

else
conditional_build: sanity_check
	@echo "skipping $(APP) ($(APP) not configured)"
endif

# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean:

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

