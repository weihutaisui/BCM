#
# In most cases, you only need to modify this first section.
#
LIB = omcihandler.a

OBJS = mdm_initgpon.o \
       common_ellist.o omci_res_dm.o \
       rut_gpon_service_llist.o rut_gpon_rule_llist.o \
       rut_gpon_flow.o rut_gpon_model.o rut_gpon_rule.o rut_gpon_vlan.o \
       stl_gpon.o rcl_gpon.o rut_gpon.o rut_gpon_mcast.o \
       rut_gponrg_full.o rut_gpon_if.o rut_common.o\
       rcl_gpon_voice.o stl_gpon_voice.o rut_gpon_voice.o \
       rut_remotedebug.o \
       rcl_gpon_eqpt.o stl_gpon_eqpt.o \
       rcl_gpon_l2.o stl_gpon_l2.o \
       rcl_gpon_l3.o stl_gpon_l3.o \
       rcl_gpon_gen.o stl_gpon_gen.o \
       rcl_gpon_ani.o stl_gpon_ani.o \
       rcl_gpon_eth.o stl_gpon_eth.o \
       rcl_gpon_misc.o stl_gpon_misc.o

all: $(LIB)

clean: generic_clean


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

# The standalone OMCI needs flags from CMS_COMPILE_FLAGS
ifeq ($(strip $(BUILD_BRCM_CMS)),)
CFLAGS += $(CMS_COMPILE_FLAGS)
endif

#
# Private apps and libs are allowed to include header files from the
# private and public directories
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I. -I.. -I../inc \
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR) \
                         -I$(BCM_FSBUILD_DIR)/public/include \
                         -I$(BCM_FSBUILD_DIR)/private/include

ALLOWED_INCLUDE_PATHS += -I$(BUILD_DIR)/userspace/public/libs/bcm_util

ALLOWED_INCLUDE_PATHS += -I$(BUILD_DIR)/userspace/public/libs/bcm_flashutil

ALLOWED_INCLUDE_PATHS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)  \
                         -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
                         -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)

ALLOWED_INCLUDE_PATHS += -I$(KERNEL_LINKS_DIR)

ifeq ($(strip $(BRCM_XDSL_DISTPOINT_USE_G9991)),y)
CFLAGS += -DG9991
endif

ifneq ($(strip $(BUILD_BRCM_FTTDP)),)
CFLAGS += -DBRCM_FTTDP
endif


# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors # NEW_FORBID_WARNINGS


#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
#


#
# Generic rule makes the library
# see make.common
#

$(LIB): $(OBJS)
	$(AR) -rcs $@ $(OBJS)


#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)
