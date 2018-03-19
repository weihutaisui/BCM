#***********************************************************************
#
#  Copyright (c) 2007  Broadcom Corporation
#  All Rights Reserved
#
#  <:label-BRCM:2012:proprietary:epon
#  
#   This program is the proprietary software of Broadcom and/or its
#   licensors, and may only be used, duplicated, modified or distributed pursuant
#   to the terms and conditions of a separate, written license agreement executed
#   between you and Broadcom (an "Authorized License").  Except as set forth in
#   an Authorized License, Broadcom grants no license (express or implied), right
#   to use, or waiver of any kind with respect to the Software, and Broadcom
#   expressly reserves all rights in and to the Software and all intellectual
#   property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
#   NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
#   BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#  
#   Except as expressly set forth in the Authorized License,
#  
#   1. This program, including its structure, sequence and organization,
#      constitutes the valuable trade secrets of Broadcom, and you shall use
#      all reasonable efforts to protect the confidentiality thereof, and to
#      use this information only in connection with your use of Broadcom
#      integrated circuit products.
#  
#   2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#      AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#      WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
#      RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
#      ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
#      FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
#      COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
#      TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
#      PERFORMANCE OF THE SOFTWARE.
#  
#   3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
#      ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
#      INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
#      WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
#      IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
#      OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
#      SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
#      SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
#      LIMITED REMEDY.
#  :>
#
#***********************************************************************/


#
# In most cases, you only need to modify this first section.
#
EXE = portLoopDetect
OBJS = portLoopDetect.o 
LIBS = -lethswctl
LIBS += -lcms_msg $(CMS_COMMON_LIBS) -ldl $(CMS_CORE_LIBS)

BMD_TARGETS = shell api pkgsrc shared
BMD_LIBNAMES = $(addprefix -lbmd,$(BMD_TARGETS))
CDK_TARGETS = shell main pkgsrc shared sym libc
CDK_LIBNAMES = $(addprefix -lcdk,$(CDK_TARGETS))
PHY_TARGETS = pkgsrc generic util sym
PHY_LIBNAMES = $(addprefix -lphy,$(PHY_TARGETS))

#LIBS += $(BMD_LIBNAMES)
#LIBS += $(PHY_LIBNAMES)
#LIBS += $(CDK_LIBNAMES)

# Define date and time for use if we're making a zip file
ZIP_FILE := $(shell date '+%Y%m%d_%H%M' 2>/dev/null)


all dynamic install : conditional_build

clean: generic_clean
	rm -f $(INSTALL_DIR)/bin/$(EXE)

# Dummy dependency FORCE to force stuff to happen
FORCE:

# Archive source and make files into a .ZIP file.  Use the dummy FORCE dependency to force running
zip:  FORCE
	@-mkdir -p ./_archive
	zip -R ./_archive/$(ZIP_FILE) "*.c*" "*.h*" "Mak*" "run*"
	
	@echo
	@echo Archive name: $(ZIP_FILE)
	@echo	
#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

ifneq ($(strip $(BUILD_BRCM_CMS)),)
ifneq ($(strip $(BUILD_EPON_SDK)),)
conditional_build: $(EXE) generic_exe_install
else
conditional_build:
	@echo "Skipping EPON SDK APP"
endif
else
conditional_build:
	@echo "Skipping EPON SDK APP (CMS not configured)"
endif



#
# Private apps and libs are allowed to include header files from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.

ALLOWED_INCLUDE_PATHS := -I.\
						 -I$(BUILD_DIR)/userspace/public/include  \
						 -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
						 -I$(BUILD_DIR)/userspace/private/include  \
						 -I$(BUILD_DIR)/userspace/private/include/$(OALDIR) \
						 -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)  \
						 -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
						 -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD) \
						 -I$(BUILD_DIR)/shared/broadcom/include/bcm963xx \
                         $(INC_RDP_FLAGS)

#
# Private apps and libs are allowed to link with libraries from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_LIB_DIRS := /lib:/lib/private:/lib/public

CFLAGS += -Werror
CFLAGS += -DCONFIG_BCM9$(BRCM_CHIP)

MDK_BASE = $(CURR_DIR)/../../libs/mdk212
CFLAGS +=-I$(MDK_BASE)/cdk/include
CFLAGS +=-I$(MDK_BASE)/bmd/include
CFLAGS +=-I$(MDK_BASE)/phy/include
ifndef SYS_NO_TTY
CFLAGS += -DSYS_HAS_TTY
endif
ifneq ($(strip $(BUILD_BRCM_CMS)),)
CFLAGS += -DBRCM_CMS_BUILD
endif
CFLAGS += -DCDK_INCLUDE_CUSTOM_CONFIG -DBMD_INCLUDE_CUSTOM_CONFIG -DUSE_SYSTEM_LIBC -DBMD_CONFIG_INCLUDE_DMA=0 -DBMD_SYS_USLEEP=_usleep -DPHY_SYS_USLEEP=_usleep
CFLAGS += -DBCM_MDK_OS_DEP=1


SDK_BASE = $(CURR_DIR)/../../libs/eponsdk
CFLAGS +=-I$(SDK_BASE)/api/h
CFLAGS +=-I$(SDK_BASE)/config
CFLAGS +=-I$(SDK_BASE)/common/h
CFLAGS +=-I$(SDK_BASE)/msgprocess/h
CFLAGS +=-I$(SDK_BASE)/oamprocess/h
CFLAGS +=-I$(SDK_BASE)/oamprocess/Type/Generic

CFLAGS +=-I$(SDK_BASE)/platform/h
CFLAGS +=-I$(SDK_BASE)/platform/h/linux

CFLAGS += $(SSP_TYP_COMPILER_OPTS)
LIBS += $(SSP_TYP_LIBS)

# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 
$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS) -Wl,-rpath,$(CMS_LIB_RPATH) $(CMS_LIB_PATH) $(LIBS) -lpthread

#$(EXE): $(BOBJS) $(BMD_BLIBS) $(PHY_BLIBS) $(CDK_BLIBS)
#	$(CC) $(BOBJS) $(BMD_BLIBS) $(PHY_BLIBS) $(CDK_BLIBS)  $(OSLIBS) $(LDFLAGS) -o $@  $(CMS_LIB_PATH) $(LIBS)


#app:$(OBJS_LIB) $(OBJ_APP)
#	$(CC) -g $(OBJS_LIB) $(OBJ_APP) -o sdkApp -lpthread

# Define date and time for use if we're making a zip file
ZIP_FILE := $(shell date '+%Y%m%d_%H%M' 2>/dev/null)

# Dummy dependency FORCE to force stuff to happen
FORCE:

# Archive source and make files into a .ZIP file.  Use the dummy FORCE dependency to force running
zip:  FORCE
	@-mkdir -p ./_archive
	zip -R ./_archive/$(ZIP_FILE) "*.c*" "*.h*" "Mak*" "run*"
	
	@echo
	@echo Archive name: $(ZIP_FILE)
	@echo	
	
#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)


