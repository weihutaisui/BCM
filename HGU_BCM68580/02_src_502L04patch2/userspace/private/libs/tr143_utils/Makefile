#***********************************************************************
#
#  Copyright (c) 2013  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/

#
# In most cases, you only need to modify this first section.
#

LIB = libtr143_utils.so

OBJS := common.o protocol.o ftp.o http.o  

all dynamic install: conditional_build 

clean: generic_clean
	rm -f $(INSTALL_DIR)/lib/private/$(LIB)
	rm -f $(BCM_FSBUILD_DIR)/private/include/tr143_defs.h


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


ifeq ($(strip $(BCM_SPEEDYGET)),y)
	CFLAGS += -DCONFIG_BCM_SPEEDYGET
endif

ifneq ($(strip $(BUILD_BRCM_CMS)),)
ifneq ($(strip $(BUILD_TR69_TR143)),)
conditional_build: sanity_check generic_private_lib_install
	mkdir -p $(BCM_FSBUILD_DIR)/private/include
	install -m 444 tr143_defs.h $(BCM_FSBUILD_DIR)/private/include
else
conditional_build: sanity_check
	@echo "Skipping $(LIB) (not configured)"
endif
else
conditional_build: sanity_check
	@echo "Skipping $(LIB) (CMS not configured)"
endif



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



#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 
$(LIB): $(OBJS)
	$(CC) -shared $(BCM_LD_FLAGS) -Wl,--whole-archive,-soname,$(notdir $@) -o $@ $(OBJS) -Wl,--no-whole-archive



#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
# no objects at this level, so don't need dependencies

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)

