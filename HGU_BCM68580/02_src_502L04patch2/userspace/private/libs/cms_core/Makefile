#***********************************************************************
#
#  Copyright (c) 2006-2007  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************


#
# In most cases, you only need to modify this first section.
#
LIB = libcms_core.so

FULL_SOURCE_OBJS = object.o phl.o odl.o management.o locks.o\
                   securitylog.o \
                   mdm_config.o mdm_configin.o \
                   mdm_binaryHelper.o mdm_dataModelHelper.o \
                   mdm_oidInfoArray.o mdm2_oidInfoArray.o \
                   mdm_init.o \
                   mdm_initwlan.o \
                   mdm_initdsl.o mdm_initmoca.o mdm_initlan.o mdm_initvpn.o \
                   mdm_initstorageservice.o \
                   mdm_initponwan.o mdm_initepon.o mdm_initoptical.o \
                   mdm_initcellular.o mdm2_initcellular.o \
                   mdm_initmodsw.o \
                   mdm_initmodsw_osgiee.o \
                   mdm_initmodsw_linuxee.o \
                   mdm_initmodsw_openwrtee.o \
                   mdm_initmodsw_dockeree.o \
                   mdm_initbmu.o \
                   mdm_inithomeplug.o \
                   mdm2_init.o mdm2_initlan.o mdm2_initlan6.o \
                   mdm2_initwifi.o \
                   mdm2_initusb.o \
                   mdm2_initdsl.o


BINARY_ONLY_OBJS = mdm.o

OBJS = $(FULL_SOURCE_OBJS) $(BINARY_ONLY_OBJS)

all install: conditional_build

clean: generic_oalclean generic_clean device2_clean
	rm -f $(INSTALL_DIR)/lib/private/$(LIB)

HAVESRC := $(wildcard mdm.c)


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

export BUILD_EPON_SDK_VOICE_OAM

ifneq ($(strip $(BRCM_VOICE_SUPPORT)),)
include $(BUILD_DIR)/make.voice
endif

ifeq ($(BCM_PON),y)
FULL_SOURCE_OBJS += mdm_initwanfilters.o 
endif                   

ifneq ($(strip $(BUILD_DM_TR104_V2)),)
    FULL_SOURCE_OBJS += mdm2_initvoice.o
else
    FULL_SOURCE_OBJS += mdm_initvoice.o
endif

CFLAGS += $(SSP_TYP_COMPILER_OPTS) $(SSP_TYP_LIBS)  $(SSP_LIB_INC)


#
# Private apps and libs are allowed to include header files from the
# private and public directories
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I.\
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR)

# management.c needs bcm_crc.h
ALLOWED_INCLUDE_PATHS += -I$(BUILD_DIR)/userspace/public/libs/bcm_util

ifneq ($(strip $(WIRELESS)),)
STATIC_LIBS = ../wlcsm/libwlcsm.a
CFLAGS += -DSUPPORT_WIRELESS
ALLOWED_INCLUDE_PATHS += -I$(BUILD_DIR)/userspace/private/libs/wlcsm/include
endif

ifeq ($(strip $(BUILD_6802_MOCA)),y)
CFLAGS += -DCONFIG_BCM_6802_MoCA
endif

ifeq ($(strip $(BRCM_XDSL_DISTPOINT_USE_G9991)),y)
CFLAGS += -DG9991
endif

ifneq ($(strip $(BUILD_BRCM_FTTDP)),)
CFLAGS += "-DBRCM_FTTDP"
endif

#management.c needs cms_boardioctl.h, so we have to include these kernel header files
ALLOWED_INCLUDE_PATHS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)  \
                         -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
                         -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD) \
                         -I$(BCM_FSBUILD_DIR)/public/include

ifeq ($(BUILD_RDPA),y)
ALLOWED_INCLUDE_PATHS += $(INC_RDP_FLAGS)
endif

# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors # NEW_FORBID_WARNINGS


device2:
	$(MAKE) -C $(OALDIR)/device2
	
device2_clean:
	$(MAKE) -C $(OALDIR)/device2 clean

#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 
ifneq ($(strip $(BUILD_BRCM_CMS)),)

conditional_build: generic_private_lib_install

$(LIB): generic_oaldir $(OBJS) device2 $(STATIC_LIBS)
	$(CC) -shared $(BCM_LD_FLAGS) -Wl,--whole-archive,-soname,$(notdir $@) -o $@ $(OBJS) \
          $(OALDIR)/oal.a $(OALDIR)/device2/device2.a $(STATIC_LIBS) -Wl,--no-whole-archive -ldl -lpthread 


#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

ifeq ($(strip $(HAVESRC)),)

mdm.o: mdm.o.$(PROFILE_ARCH).save
	cat $< > $@

-include $(FULL_SOURCE_OBJS:.o=.d)

else
-include $(OBJS:.o=.d)
endif

else
conditional_build:
	@echo "skipping $(LIB) (not configured)"
endif
