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

# Download from https://bitbucket.org/cleonello/jqplot/downloads/
APP_DIR := $(abspath jqplot)
TAR_FILE := jquery.jqplot.1.0.8r1250.tar.bz2

WEBS_DIR := $(INSTALL_DIR)/webs

APP_PLUGINS_DIR := $(APP_DIR)/plugins
WEBS_PLUGINS_DIR := $(WEBS_DIR)/plugins

APP_JQUERY_UI_DIR := $(APP_DIR)/examples/jquery-ui
WEBS_JQUERY_UI_DIR := $(WEBS_DIR)/jquery-ui

.PHONY: check_untar_patch_configure conditional_build check_versions

ifneq ($(strip $(BUILD_JQPLOT)),)

conditional_build: install_web_files_pi install_web_files_jqui install_web_files_jqui2
	@echo "done $@"

#--------------------------------------------------------------------------------
# eval call CREATE_WEB_TRG creates the makefile target install_web_files.  This
# target is used to install and minimize web files.   See make.common for details
# install_web_files: conditional_build
$(eval $(call CREATE_WEB_TRG, install_web_files_pi, $(APP_DIR), $(WEBS_DIR), staging, minimized_webs.tar, *.js *.css plugins/*))
$(eval $(call CREATE_WEB_TRG, install_web_files_jqui, $(APP_JQUERY_UI_DIR)/js, $(WEBS_JQUERY_UI_DIR), staging, minimized_webs_jqui.tar))
$(eval $(call CREATE_WEB_TRG, install_web_files_jqui2, $(APP_JQUERY_UI_DIR)/css/ui-lightness, $(WEBS_JQUERY_UI_DIR), staging, minimized_webs_jqui2.tar, jquery-ui.min.css))
#--------------------------------------------------------------------------------

else
conditional_build: sanity_check
	echo "skipping $(APP) ($(APP) not configured)"
endif

# NOTE: clean is taken care of by the WEB_CLEAN rules in CREATE_WEB_TRG macros.
clean:

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

