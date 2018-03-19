#***********************************************************************
#
#  Copyright (c) 2007  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/

#
# In most cases, you only need to modify this first section.
#
LIB = SOAPParser.a
FULL_SOURCE_OBJS = vendorRpc.o xmlVendorRpcTables.o
BINARY_ONLY_OBJS = RPCState.o xmlParserSM.o xmlTables.o dmCommon.o


all: $(LIB)

clean: generic_clean

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
# You do not need to modify this part.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, ,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

ifneq ($(strip $(BRCM_VOICE_SUPPORT)),)
include $(BUILD_DIR)/make.voice
endif

ifeq ($(strip $(BRCM_USER_SSP)),y)
CFLAGS += $(SSP_TYP_COMPILER_OPTS)
endif

ifneq ($(strip $(BUILD_TR69C_BEEP)),)
LIBS += -L$(INSTALL_DIR)/lib -ldbus-1 -lpthread -lrt
LIBS += -L$(INSTALL_DIR)/lib/public -lexpat -lffi
ifneq ($(findstring uclibc, $(TOOLCHAIN_PREFIX)),)
LIBS += -lintl
endif
LIBS += -L$(INSTALL_DIR)/lib/public -lgio-2.0 -lglib-2.0 -lgmodule-2.0 -lgobject-2.0 -lgthread-2.0 -lz
CFLAGS += -I$(BCM_FSBUILD_DIR)/public/include/glib-2.0 -I$(BCM_FSBUILD_DIR)/public/lib/glib-2.0/include
CFLAGS += -I$(BCM_FSBUILD_DIR)/public/include/gio-unix-2.0 -I$(BCM_FSBUILD_DIR)/public/include/glib-2.0/gobject
CFLAGS += -DBUILD_TR69C_BEEP
BINARY_ONLY_OBJS += dmDbus.o
else
BINARY_ONLY_OBJS += dmCms.o
endif

OBJS = $(FULL_SOURCE_OBJS) $(BINARY_ONLY_OBJS)

$(LIB): $(OBJS)
	$(AR) -rcs $@ $(OBJS)

HAVESRC= $(wildcard RPCState.c)




#
# Private apps and libs are allowed to include from private and public.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I. -I.. \
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR)
#
#tr69c needs to call adsl driver to get DSL loop diagnostics data,
#so we need to include private driver path
#
ALLOWED_INCLUDE_PATHS += -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD)


# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors


#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 


#
# generic rule makes the library oal.a
# see make.common
#


#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

ifeq ($(strip $(HAVESRC)),)
RPCState.o: RPCState.o.$(PROFILE_ARCH).save
	cat $< >$@

xmlParserSM.o: xmlParserSM.o.$(PROFILE_ARCH).save
	cat $< >$@

xmlTables.o: xmlTables.o.$(PROFILE_ARCH).save
	cat $< >$@

dmDbus.o: dmDbus.o.$(PROFILE_ARCH).save
	cat $< >$@

dmCms.o: dmCms.o.$(PROFILE_ARCH).save
	cat $< >$@

dmCommon.o: dmCommon.o.$(PROFILE_ARCH).save
	cat $< >$@

-include $(FULL_SOURCE_OBJS:.o=.d)

else
-include $(OBJS:.o=.d)
endif


