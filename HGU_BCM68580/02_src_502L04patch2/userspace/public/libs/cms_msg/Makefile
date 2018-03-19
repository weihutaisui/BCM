#***********************************************************************
#
#  Copyright (c) 2006-2007  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************


LIB = libcms_msg.so

OBJS = msg.o oal_msg.o

all: conditional_build

clean: 
	rm -f $(INSTALL_DIR)/lib/public/$(LIB)
	rm -rf objs

# BRCM_SUPPORTS_MULTIARCH_BUILD



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

ifneq ($(strip $(BUILD_BRCM_CMS)),FIXME)
conditional_build: sanity_check $(LIB) generic_public_lib_install

else
conditional_build:
	@echo "cms_msg skipped (not configured)"

endif


ifneq ($(strip $(BRCM_VOICE_SUPPORT)),)
include $(BUILD_DIR)/make.voice
endif



#
# Public libs are only allowed to include header files from the
# public directory.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I.\
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR)


# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors

vpath %.c $(OALDIR) 

OBJS := $(addprefix objs/$(PROFILE_ARCH)/,$(OBJS))
LIB := $(addprefix objs/$(PROFILE_ARCH)/,$(LIB))


#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 

objs/$(PROFILE_ARCH):
	mkdir -p objs/$(PROFILE_ARCH)

objs/$(PROFILE_ARCH)/%.o: %.c | objs/$(PROFILE_ARCH)
	$(CC) -MMD $(CPPFLAGS) $(CFLAGS)  -c -o $@ $<

$(notdir $(LIB)) : $(LIB)

PHONY: $(notdir $(LIB)) 

$(LIB): $(OBJS) | objs/$(PROFILE_ARCH) 
	$(CC) -shared $(BCM_LD_FLAGS) -Wl,--whole-archive,-soname,$(notdir $@) -o $@ $(OBJS) -Wl,--no-whole-archive



#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)

shell: 
	bash 

