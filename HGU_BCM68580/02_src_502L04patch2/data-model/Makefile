#***********************************************************************
#
#  Copyright (c) 2007  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/


default_target: all

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /data-model and taking the first word only).
# Then include the common defines under CommEngine.
# You do not need to modify this part.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /data-model, /data-model,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common


#
# Local defines for the data-model directory.
#
GENERATE = ./generate_from_dm.pl
PERL_MODULES = GenObjectNode.pm GenParamNode.pm Utils.pm

#
# Always generate both the IGD and Device based data models.
# Which one is compiled (in userspace/private/libs/{mdm, mdm2} depends
# on menuconfig settings for Data Model mode.
#
DMFILE_TR104 := cms-dm-tr104-voice.xml
DMFILE := cms-data-model-merged.xml
ifneq ($(strip $(BUILD_DM_WLAN_RETAIL)),)
DMFILE2 := cms-data-model-retail-merged2.xml
MERGE_DEV2_DIR := merge-retail2.d
else
DMFILE2 := cms-data-model-merged2.xml
MERGE_DEV2_DIR := merge-dev2.d
endif

ifneq ($(strip $(BUILD_DM_TR104_V2)),)
    $(shell cp -f cms-dm-tr104v2-voice.xml cms-dm-tr104-voice.xml)
else
    $(shell cp -f cms-dm-tr104v1-voice.xml cms-dm-tr104-voice.xml)
endif


MERGE_IGD_DIR := merge-igd.d

ifneq ($(strip $(BUILD_PROFILE_SUPPORTED_DATA_MODEL)),)
WEBS_DATA_MODEL_DIR := $(INSTALL_DIR)/webs/data-model
WEBS_DIR    := $(INSTALL_DIR)/webs
BBF_BUILD   := $(BUILD_DIR)/hostTools/cms2bbf/build.xml
BBF_DMFILE  := bbf-data-model-1.xml
ifneq ($(strip $(BUILD_DM_WLAN_RETAIL)),)
BBF_DMFILE2 := bbf-data-model-retail-2.xml
else
BBF_DMFILE2 := bbf-data-model-2.xml
endif
endif

# The DMFILE must be regenerated anytime one of the data model files changes.
# For this detection mechanism to work correctly, all data model files
# must match the pattern of cms-dm*.xml
DM_FILES := $(shell ls -w 5000 -x cms-dm*.xml)

#
# Generate all the files from the data model file.
#
all: $(DMFILE) $(DMFILE2) $(BBF_DMFILE) $(BBF_DMFILE2) \
     $(BUILD_DIR)/userspace/public/include/mdm_objectid.h  \
     $(BUILD_DIR)/userspace/public/include/mdm_object.h \
     $(BUILD_DIR)/userspace/public/include/mdm_validstrings.h \
     $(BUILD_DIR)/userspace/public/include/mdm_params.h \
     $(BUILD_DIR)/userspace/private/libs/mdm/Baseline_1.c \
     $(BUILD_DIR)/userspace/private/libs/mdm2/Device2_Baseline_1.c \
     $(BUILD_DIR)/userspace/private/libs/cms_core/mdm_oidInfoArray.c \
     $(BUILD_DIR)/userspace/private/libs/cms_core/mdm2_oidInfoArray.c \
     $(BUILD_DIR)/userspace/private/libs/cms_core/rcl.h \
     $(BUILD_DIR)/userspace/private/libs/cms_core/stl.h

$(DMFILE): $(GENERATE) $(PERL_MODULES) $(MERGE_IGD_DIR) $(DM_FILES)
	$(GENERATE) merge $(BUILD_DIR) $(MERGE_IGD_DIR) $(DMFILE)

$(DMFILE2): $(GENERATE) $(PERL_MODULES) $(MERGE_DEV2_DIR) $(DM_FILES)
	$(GENERATE) merge2 $(BUILD_DIR) $(MERGE_DEV2_DIR) $(DMFILE2)

$(BUILD_DIR)/userspace/private/libs/mdm/Baseline_1.c: $(DMFILE)
	$(GENERATE) mdm $(BUILD_DIR) $(DMFILE)

$(BUILD_DIR)/userspace/private/libs/mdm2/Device2_Baseline_1.c: $(DMFILE2)
	$(GENERATE) mdm2 $(BUILD_DIR) $(DMFILE2)

$(BUILD_DIR)/userspace/public/include/mdm_params.h: $(DMFILE) $(DMFILE2)
	$(GENERATE) mdmparams $(BUILD_DIR) $(DMFILE) $(DMFILE2)

$(BUILD_DIR)/userspace/public/include/mdm_objectid.h: $(DMFILE) $(DMFILE2)
	$(GENERATE) objectid $(BUILD_DIR) $(DMFILE) $(DMFILE2)

$(BUILD_DIR)/userspace/public/include/mdm_object.h: $(DMFILE) $(DMFILE2)
	$(GENERATE) object $(BUILD_DIR) $(DMFILE) $(DMFILE2)

$(BUILD_DIR)/userspace/public/include/mdm_validstrings.h: $(DMFILE) $(DMFILE2)
	$(GENERATE) validstrings $(BUILD_DIR) $(DMFILE) $(DMFILE2)

$(BUILD_DIR)/userspace/private/libs/cms_core/rcl.h $(BUILD_DIR)/userspace/private/libs/cms_core/stl.h: $(DMFILE) $(DMFILE2)
	$(GENERATE) prototypes $(BUILD_DIR) $(DMFILE) $(DMFILE2)

$(BUILD_DIR)/userspace/private/libs/cms_core/mdm_oidInfoArray.c: $(DMFILE)
	$(GENERATE) oidinfo $(BUILD_DIR) $(DMFILE)
	
$(BUILD_DIR)/userspace/private/libs/cms_core/mdm2_oidInfoArray.c: $(DMFILE2)
	$(GENERATE) oidinfo2 $(BUILD_DIR) $(DMFILE2)

ifneq ($(strip $(BUILD_PROFILE_SUPPORTED_DATA_MODEL)),)

$(BBF_DMFILE): $(DMFILE) bbf_dmfile_build bbf_dmfile_install
	@echo "Done building $(BBF_DMFILE)"

bbf_dmfile_build:
	ant -f $(BBF_BUILD) run -Darg0=$(CURR_DIR)/$(DMFILE) -Darg1=$(CURR_DIR)/$(BBF_DMFILE)

ifeq ($(strip $(BUILD_PURE181_PROFILES)),)
bbf_dmfile_install:
	if [ ! -e $(WEBS_DIR) ]; then \
	    mkdir $(WEBS_DIR); \
	fi; \
	if [ ! -e $(WEBS_DATA_MODEL_DIR) ]; then \
	    mkdir $(WEBS_DATA_MODEL_DIR); \
	fi; \
	cp -d $(BBF_DMFILE) $(WEBS_DATA_MODEL_DIR)/$(BBF_DMFILE);
else
bbf_dmfile_install:
endif

$(BBF_DMFILE2): $(DMFILE2) bbf_dmfile2_build bbf_dmfile2_install
	@echo "Done building $(BBF_DMFILE2)"

bbf_dmfile2_build:
	ant -f $(BBF_BUILD) run -Darg0=$(CURR_DIR)/$(DMFILE2) -Darg1=$(CURR_DIR)/$(BBF_DMFILE2)

ifneq ($(strip $(BUILD_PURE181_PROFILES)),)
bbf_dmfile2_install:
	if [ ! -e $(WEBS_DIR) ]; then \
	    mkdir $(WEBS_DIR); \
	fi; \
	if [ ! -e $(WEBS_DATA_MODEL_DIR) ]; then \
	    mkdir $(WEBS_DATA_MODEL_DIR); \
	fi; \
	cp -d $(BBF_DMFILE2) $(WEBS_DATA_MODEL_DIR)/$(BBF_DMFILE2);
else
bbf_dmfile2_install:
endif

else

$(BBF_DMFILE):
	@echo "skipping $(BBF_DMFILE) (not configured)"

$(BBF_DMFILE2):
	@echo "skipping $(BBF_DMFILE2) (not configured)"

endif

report: $(DMFILE) $(DMFILE2)
	$(GENERATE) report $(BUILD_DIR) $(DMFILE)
	$(GENERATE) report2 $(BUILD_DIR) $(DMFILE2)

clean: clean_bbf_files
	rm -f $(DMFILE) $(DMFILE2) $(DMFILE_TR104)
	rm -f $(BUILD_DIR)/userspace/public/include/mdm_objectid.h
	rm -f $(BUILD_DIR)/userspace/public/include/mdm_object.h
	rm -f $(BUILD_DIR)/userspace/public/include/mdm_params.h
	rm -f $(BUILD_DIR)/userspace/public/include/mdm_validstrings.h
	rm -f $(BUILD_DIR)/userspace/private/libs/cms_core/mdm_oidInfoArray.c
	rm -f $(BUILD_DIR)/userspace/private/libs/cms_core/mdm2_oidInfoArray.c
	rm -f $(BUILD_DIR)/userspace/private/libs/cms_core/rcl.h
	rm -f $(BUILD_DIR)/userspace/private/libs/cms_core/stl.h
	rm -f $(BUILD_DIR)/userspace/private/libs/mdm/*.c
	rm -f $(BUILD_DIR)/userspace/private/libs/mdm2/*.c

ifneq ($(strip $(BUILD_PROFILE_SUPPORTED_DATA_MODEL)),)
clean_bbf_files:
	rm -f $(BBF_DMFILE) $(BBF_DMFILE2)
	rm -rf $(WEBS_DATA_MODEL_DIR)
else
clean_bbf_files:
	@echo "skipping clean BBF data model files (not configured)"
endif

