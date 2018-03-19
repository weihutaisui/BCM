#***********************************************************************
# 
#    Copyright (c) 2012 Broadcom Corporation
#    All Rights Reserved
#
#***********************************************************************/

#
# In most cases, you only need to modify this first section.
#
LIB = libcms_qdm.so

OBJS = qdm_modsw_ee.o \
       qdm_intf.o qdm2_intf.o \
       qdm_ipintf.o qdm2_ipintf.o \
       qdm_lan.o qdm2_lan.o \
       qdm2_dhcpv4.o \
       qdm_route.o qdm2_route.o \
       qdm_dns.o qdm2_dns.o \
       qdm_multicast.o qdm2_multicast.o \
       qdm_dsl.o qdm2_dsl.o \
       qdm_qos.o qdm2_qos.o \
       qdm_vlan.o qdm2_vlan.o \
       qdm_ethernet.o qdm2_ethernet.o \
       qdm_tr69c.o qdm2_tr69c.o \
       qdm_ipv6.o qdm2_ipv6.o \
       qdm_xtm.o qdm2_xtm.o \
       qdm_diag.o qdm2_diag.o \
       qdm2_xmppc.o \
       qdm_system.o qdm2_system.o \
       qdm_wifi.o qdm2_wifi.o
       
all install: generic_private_lib_install

clean: generic_clean
	rm -f $(INSTALL_DIR)/lib/private/$(LIB)



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

ifneq ($(strip $(BRCM_VOICE_SUPPORT)),)
include $(BUILD_DIR)/make.voice
endif

CFLAGS += $(SSP_TYP_COMPILER_OPTS)
BCM_LD_FLAGS += -L$(INSTALL_DIR)/lib $(SSP_TYP_LIBS)

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
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR) \
                         -I$(INC_BRCMSHARED_PUB_PATH)/bcm963xx
 
# Need to include xdsl header for qdm2_dsl.c
ALLOWED_INCLUDE_PATHS += -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD)

# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors


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
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)
