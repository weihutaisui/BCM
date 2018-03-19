#
# In most cases, you only need to modify this first section.
#
LIB = schema.a

OBJS = omci_schema.o

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
ALLOWED_INCLUDE_PATHS := -I. -I../inc \
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR) \
                         -I$(BCM_FSBUILD_DIR)/public/include \
                         -I$(BCM_FSBUILD_DIR)/private/include

CUSTOM_CFLAGS += -DDMP_BASELINE_1

# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors # NEW_FORBID_WARNINGS


#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
#


#
# Generic rule makes the library oal.a
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
