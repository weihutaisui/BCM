#***********************************************************************
#
#  Copyright (c) 2006  Broadcom Corporation
#  All Rights Reserved
#  No portions of this material may be reproduced in any form without the
#  written permission of:
#          Broadcom Corporation
#          16215 Alton Parkway
#          Irvine, California 92619
#  All information contained in this document is Broadcom Corporation
#  company private, proprietary, and trade secret.
#
#***********************************************************************/

#
# We need to declare these directory names as phony targets,
# otherwise make will think the directories themselves are
# the targets and will never execute the cmds.
# See GNU Make, 4.6 
#
CYASSL_LIBS = src ctaocrypt
.PHONY : $(CYASSL_LIBS)
all    : $(CYASSL_LIBS)


CYASSL_COMPILE_FLAGS := -DHAVE_CONFIG_H -Wall -DNDEBUG -Os -fPIC -fomit-frame-pointer \
		-DNO_AES -DNO_DH -DNO_DSA -DNO_SHA256

ifneq ($(strip $(BRCM_PTHREADS)),y)
CYASSL_COMPILE_FLAGS += -DSINGLE_THREADED
endif

### -DDEBUG_CYASSL
CYASSL_INCLUDE_PATHS := -I. -I../ctaocrypt/include -I../include

ifneq ($(strip $(BUILD_DIR)),)
# make.common overwrites cflags (and CFLAGS are inherited in here by parent
# makefile, so create a backup, and then readd it after inclusion
CFLAGS_OLD:=$(CFLAGS)
include $(BUILD_DIR)/make.common
CFLAGS += $(CFLAGS_OLD)
CFLAGS += $(SSP_TYP_COMPILER_OPTS) $(SSP_TYP_LIBS)  $(SSP_LIB_INC)
ifeq ($(strip $(ARCH_ENDIAN)),little)
SSLOPTIONS = -DWORDS_LITTLEENDIAN
else
SSLOPTIONS = -DWORDS_BIGENDIAN 
endif
else
$(info BUILD_DIR not set!)
SSLOPTIONS = -DWORDS_BIGENDIAN 
endif

CFLAGS += $(CYASSL_COMPILE_FLAGS) $(CYASSL_INCLUDE_PATHS) $(SSLOPTIONS)
CFLAGS += -I$(TOOLCHAIN)/include 
CFLAGS += $(BRCM_WERROR_CFLAGS)

##############################################################
# 
# Rules for inidividual libs
#
##############################################################


$(CYASSL_LIBS): 
	$(MAKE) -C $@

#
# See example in GNU Make, 4.6
#
lib_clean:
	for dir in $(CYASSL_LIBS); do \
		$(MAKE) -C $$dir clean; \
	done

clean: lib_clean

export CFLAGS
