#
# In most cases, you only need to modify this first section.
#
LIB = libomciutil.so

OBJS = omciutl_crc.o omciutl_mib.o

all install: conditional_build

clean: conditional_clean

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

ifneq ($(strip $(BUILD_BCMIPC)),)
OBJS += omci_ipc_client.o
endif


ifneq ($(strip $(BUILD_OMCI)),)
conditional_build: generic_private_lib_install

conditional_clean: generic_clean
	rm -f $(INSTALL_DIR)/lib/private/$(LIB)

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
                         -I$(BUILD_DIR)/userspace/private/libs/omcisdk/core/inc \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR) 

# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors

#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 
$(LIB): $(OBJS)
	$(CC) -shared $(BCM_LD_FLAGS) -Wl,--whole-archive,-soname,$@ -o $@ $(OBJS) -Wl,--no-whole-archive


#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)

else
conditional_build:
	@echo "omciutil skipped (BUILD_OMCI not configured)"
conditional_clean:
	@echo "omciutil skipped (BUILD_OMCI not configured)"
endif

