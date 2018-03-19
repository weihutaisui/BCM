#***********************************************************************
#
#  Copyright (c) 2012  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/

all dynamic install: conditional_build


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

LDFLAGS=-L$(INSTALL_DIR)/lib

ifeq ($(strip $(BRCM_USER_SSP)),y)
CFLAGS+=$(SSP_MIN_COMPILER_OPTS)
LDFLAGS+=-L$(INSTALL_DIR)/lib $(SSP_LIBS) 
endif

export CFLAGS
export LDFLAGS

# Download from http://www.ijg.org/files/jpegsrc.v9a.tar.gz
APP = jpeg-9a
LIB = libjpeg.so.9.1.0
INS_LIB = libjpeg.so.9

.PHONY: check_untar_patch_configure conditional_build check_versions

ifneq ($(strip $(BUILD_DLNA)),)
conditional_build: 
	@if [ ! -e $(APP)/Makefile ]; then \
	   tar zxvf $(APP).tar.gz; \
	   cd $(APP); \
	   ./configure --host=$(TOOLCHAIN_PREFIX) --prefix=$(INSTALL_DIR) --sbindir=$(INSTALL_DIR)/bin --libdir=$(INSTALL_DIR)/lib; \
	   cd ..; \
	fi; \
	$(MAKE) -C $(APP) ; \
	cp -a $(APP)/.libs/$(LIB) $(INSTALL_DIR)/lib/$(INS_LIB); \
	echo "Done building $(APP)"
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif

# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean:
	rm -f $(INSTALL_DIR)/lib/$(INS_LIB)
	-[ ! -e $(APP)/Makefile ] || $(MAKE) -C $(APP) distclean

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean
	rm -rf $(APP)
	
bcm_dorel_distclean: distclean

