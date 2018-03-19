#
# In most cases, you only need to modify this first section.
#
EXE = omcid
OBJS = omcid.o omcid_msg.o omci_api.o omcid_data.o omcid_capture.o \
       omcid_pm.o omcid_pm_timer.o omcid_helper.o omcid_cms.o

BCM_CTL_LIBS = -lgponctl -lgponif -lvlanctl -lethswctl -lethctl 
BCM_CTL_LIBS += -lbridgeutil -ltmctl -lrdpactl -lbcmmcast -lbdmf

LIBS = -lomci_core -lomci -lomciutil -lomcipm_drv -ldl $(BCM_CTL_LIBS) -lpthread
LIBS += $(CMS_COMMON_LIBS)
# OMCI uses cms_util in NOCMS profile
LIBS += -lcms_util
EID_FILE = eid_bcm_omcid.txt


all dynamic install : conditional_build

clean: generic_clean generic_eid_file_clean
	rm -f $(INSTALL_DIR)/bin/$(EXE)

binaryonly_dist_clean: clean generic_binaryonly_dist_clean


#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

ifneq ($(strip $(BUILD_BCMIPC)),)
LIBS += -lbcmipc
OBJS += omcid_ipc.o omcipm_ipc.o
CFLAGS += -DBUILD_BCMIPC
endif

ifneq ($(strip $(BUILD_BRCM_CMS)),)
LIBS += -lomci_cms -lcms_msg
else
# The standalone OMCI needs flags from CMS_COMPILE_FLAGS
CFLAGS += $(CMS_COMPILE_FLAGS)
endif

ifneq ($(strip $(BUILD_BRCM_FTTDP)),)
	CFLAGS += "-DBRCM_FTTDP"
endif

ifneq ($(strip $(BUILD_BRCM_CMS)),)
ifneq ($(strip $(BUILD_OMCI)),)
conditional_build: $(EXE) generic_exe_install generic_eid_file_install
else
conditional_build:
	@echo "Skipping $(EXE) (not configured)"
endif
else
ifneq ($(strip $(BUILD_OMCI)),)
conditional_build: $(EXE) generic_exe_install generic_eid_file_install
else
conditional_build:
	@echo "Skipping $(EXE) (CMS not configured)"
endif
endif


#
# Private apps and libs are allowed to include header files from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I.\
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/libs/omcisdk/core/inc \
                         -I$(BUILD_DIR)/userspace/private/libs/omcisdk/core/handler \
                         -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) \
                         -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
                         -I$(BUILD_DIR)/shared/opensource/include/bcm963xx  \
                         -I$(KERNEL_LINKS_DIR) \
                         $(INC_RDP_FLAGS)

#
# Private apps and libs are allowed to link with libraries from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_LIB_DIRS := /lib:/lib/private:/lib/public


CFLAGS += -DOMCID_ERROR_CHECKING
# CFLAGS += -DOMCID_MASK_INSTANCE_SET
# CFLAGS += -DOMCID_DISABLE_RESPONSE

# CFLAGS += -DOMCI_API_NOTICE
# CFLAGS += -DOMCI_API_DEBUG

ifneq ($(strip $(BRCM_MINIGW)),)
CFLAGS += -DCONFIG_BRCM_MINIGW
endif

ifneq ($(strip $(BUILD_BRCM_CMS)),)
CFLAGS += -DOMCI_API_CMS
LIBS += $(CMS_CORE_LIBS)
endif

CFLAGS += $(SSP_TYP_COMPILER_OPTS)
LIBS += $(SSP_TYP_LIBS)

#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# If a pre-compiled object is present, use it.  This extra logic allows
# the same Makefile to be used for source and binary only release.
# GLOBAL_RELEASE_SCRIPT_NOTOUCH
#
$(EXE): $(OBJS)
ifneq ($(wildcard omcid.c),)
	$(CC) -o $@ $(OBJS) $(BCM_LD_FLAGS) -Wl,-rpath,$(CMS_LIB_RPATH) $(CMS_LIB_PATH) $(LIBS)
else
	@echo "Using pre-compiled $(EXE)_$(PROFILE).save"
	cp $(EXE)_$(PROFILE).save $(EXE)
endif

# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors


#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)
