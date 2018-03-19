#***********************************************************************
#
#  Copyright (c) 2006-2007  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************

LIB :=  libbcm_sslconf.so
OBJS := bcm_sslconf.o

INCLUDE_DIR = $(BCM_FSBUILD_DIR)/public/include
HEADER_FILE = bcm_sslconf.h
CONFIG_DIR = $(INSTALL_DIR)/etc/openssl
CONFIG_FILE = sslconf.xml

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
# You do not need to modify this part.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
#
# Public libs are only allowed to include header files from the
# public directory.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I.\
                         -I$(BCM_FSBUILD_DIR)/public/include

ALLOWED_LIB_DIRS := /lib:/lib/public

LIB_PATHS := $(patsubst %,-L$(INSTALL_DIR)%,$(subst :, ,$(ALLOWED_LIB_DIRS)))

LIBS := -lexpat

ifeq ($(strip $(BRCM_USER_SSP)),y)
LIBS   += -L$(INSTALL_DIR)/lib/public $(SSP_LIBS)
CFLAGS += $(SSP_TYP_COMPILER_OPTS)
endif

# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors

all: build_$(LIB)
	@echo "Done Building $@"

.PHONY: all

include $(BUILD_DIR)/make.common

ifeq ($(strip $(DO_BUILD_OPENSSL)),)

$(info "SKIPPING $(LIB)")
build_$(LIB):

else

$(info "BUILDING $(LIB)")

.PHONY: build_$(LIB)

build_$(LIB): sanity_check $(LIB) generic_public_lib_install fsbuild_install
	@echo "Done Building $@"

clean: generic_clean
	rm -f $(INSTALL_DIR)/lib/public/$(LIB)
	rm -f $(INCLUDE_DIR)/$(HEADER_FILE)
	rm -rf $(CONFIG_DIR)

fsbuild_install: generic_public_lib_install
	mkdir -p $(INCLUDE_DIR)
	install -m 444 $(HEADER_FILE) $(INCLUDE_DIR)
	mkdir -p $(CONFIG_DIR)
	install -m 444 $(CONFIG_FILE) $(CONFIG_DIR)

shell:
	bash -i


#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 
$(LIB): $(OBJS)
	$(CC) -shared $(BCM_LD_FLAGS) -Wl,--whole-archive,-soname,$@ -o $@ $(OBJS) -Wl,--no-whole-archive $(LIB_PATHS) $(LIBS)


#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)


endif
