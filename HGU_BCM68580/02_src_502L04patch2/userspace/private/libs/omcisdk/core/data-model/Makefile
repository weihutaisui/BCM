default_target: all

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /data-model and taking the first word only).
# Then include the common defines under CommEngine.
# You do not need to modify this part.
#
CURR_DIR := $(shell pwd)
SDK_DIR := $(CURR_DIR)/..
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
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
DMFILE := data-model-merged.xml

REPORTS := OMCI-supported-profiles-report.txt OMCI-supported-parameters-report.txt

MERGE_IGD_DIR := merge-igd.d

# The DMFILE must be regenerated anytime one of the data model files changes.
# For this detection mechanism to work correctly, all data model files
# must match the pattern of cms-dm*.xml
DM_FILES := $(shell ls -w 5000 -x cms-dm*.xml)

#
# Generate all the files from the data model file.
#
all: $(DMFILE) \
     $(SDK_DIR)/inc/omci_objectid.h  \
     $(SDK_DIR)/inc/omci_object.h \
     $(SDK_DIR)/inc/omci_validstrings.h \
     $(SDK_DIR)/inc/omci_params.h \
     $(SDK_DIR)/schema/Baseline_1.c \
     $(SDK_DIR)/schema/omci_oidInfoArray.c \
     $(SDK_DIR)/inc/omci_rcl.h \
     $(SDK_DIR)/inc/omci_stl.h

$(DMFILE): $(GENERATE) $(PERL_MODULES) $(MERGE_IGD_DIR) $(DM_FILES)
	$(GENERATE) merge $(SDK_DIR) $(MERGE_IGD_DIR) $(DMFILE)

$(SDK_DIR)/schema/Baseline_1.c: $(DMFILE)
	$(GENERATE) schema $(SDK_DIR) $(DMFILE)

$(SDK_DIR)/inc/omci_params.h: $(DMFILE)
	$(GENERATE) mdmparams $(SDK_DIR) $(DMFILE)

$(SDK_DIR)/inc/omci_objectid.h: $(DMFILE)
	$(GENERATE) objectid $(SDK_DIR) $(DMFILE)

$(SDK_DIR)/inc/omci_object.h: $(DMFILE)
	$(GENERATE) object $(SDK_DIR) $(DMFILE)

$(SDK_DIR)/inc/omci_validstrings.h: $(DMFILE)
	$(GENERATE) validstrings $(SDK_DIR) $(DMFILE)

$(SDK_DIR)/inc/omci_rcl.h $(SDK_DIR)/inc/omci_stl.h: $(DMFILE)
	$(GENERATE) prototypes $(SDK_DIR) $(DMFILE)

$(SDK_DIR)/schema/omci_oidInfoArray.c: $(DMFILE)
	$(GENERATE) oidinfo $(SDK_DIR) $(DMFILE)

$(BBF_DMFILE): $(DMFILE)
	@echo "Done building $(BBF_DMFILE)"

report: $(DMFILE)
	$(GENERATE) report $(SDK_DIR) $(DMFILE)

clean:
	rm -f $(DMFILE)
	rm -f $(REPORTS)
	rm -f $(SDK_DIR)/inc/omci_objectid.h
	rm -f $(SDK_DIR)/inc/omci_object.h
	rm -f $(SDK_DIR)/inc/omci_params.h
	rm -f $(SDK_DIR)/inc/omci_validstrings.h
	rm -f $(SDK_DIR)/inc/omci_rcl.h
	rm -f $(SDK_DIR)/inc/omci_stl.h
	rm -f $(SDK_DIR)/schema/omci_oidInfoArray.c
	rm -f $(SDK_DIR)/schema/validstrings*.c
	rm -f $(SDK_DIR)/schema/Baseline*.c
	rm -f $(SDK_DIR)/schema/X_ITU_ORG*.c
	rm -f $(SDK_DIR)/schema/X_BROADCOM_COM*.c
