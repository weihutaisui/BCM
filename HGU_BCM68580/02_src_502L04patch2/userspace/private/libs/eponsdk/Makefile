#***********************************************************************
#
# <:copyright-BRCM:2006:proprietary:epon
# 
#    Copyright (c) 2006 Broadcom 
#    All Rights Reserved
# 
#  This program is the proprietary software of Broadcom and/or its
#  licensors, and may only be used, duplicated, modified or distributed pursuant
#  to the terms and conditions of a separate, written license agreement executed
#  between you and Broadcom (an "Authorized License").  Except as set forth in
#  an Authorized License, Broadcom grants no license (express or implied), right
#  to use, or waiver of any kind with respect to the Software, and Broadcom
#  expressly reserves all rights in and to the Software and all intellectual
#  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
#  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
#  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
# 
#  Except as expressly set forth in the Authorized License,
# 
#  1. This program, including its structure, sequence and organization,
#     constitutes the valuable trade secrets of Broadcom, and you shall use
#     all reasonable efforts to protect the confidentiality thereof, and to
#     use this information only in connection with your use of Broadcom
#     integrated circuit products.
# 
#  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
#     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
#     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
#     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
#     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
#     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
#     PERFORMANCE OF THE SOFTWARE.
# 
#  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
#     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
#     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
#     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
#     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
#     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
#     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
#     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
#     LIMITED REMEDY.
# :>
#
#
#
#***********************************************************************


#
# In most cases, you only need to modify this first section.
#
LIB = libeponsdk.so

# Build library and copy hornet formware as needed
all install: generic_private_lib_install 
.PHONY: $(LIB)

    
#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
# You do not need to modify this part.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

SDK_BASE = sdk

include $(BUILD_DIR)/make.common

clean: target_clean $(SDK_BASE)_clean

target_clean: generic_clean
	rm -f $(INSTALL_DIR)/lib/private/$(LIB)
	$(MAKE) -C build clean


#
# Private apps and libs are allowed to include header files from the
# private and public directories
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#

# Note: name "ALLOWED_INCLUDE_PATHS" has been used in $(BUILD_DIR)/make.common!!
ALLOWED_INCLUDE_PATHS_2 := -I.\
						 -I$(BUILD_DIR)/userspace/public/include  \
						 -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
						 -I$(BUILD_DIR)/userspace/private/include  \
						 -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)  \
						 -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
						 -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD) \
						 -I$(BUILD_DIR)/bcmdrivers/broadcom/char/epon/impl$(BCM_EPON_STACK_IMPL)/include/platform \
						 -I$(PROJECT_DIR)/target/rdpa_gpl\
						 -I$(PROJECT_DIR)/target/bdmf/framework \
						 -I$(BRCMDRIVERS_DIR)/opensource/char/rdpa_gpl_ext/impl1/include

ifneq ($(strip $(BUILD_BRCM_CMS)),)
ALLOWED_INCLUDE_PATHS_2 += -I$(BUILD_DIR)/userspace/private/libs/cms_core/linux  \
						   -I$(BUILD_DIR)/userspace/private/libs/cms_core
endif
						 

#CFLAGS += -DBMD_CONFIG_INCLUDE_DMA=0 -DBMD_SYS_USLEEP=_usleep -DPHY_SYS_USLEEP=_usleep
CFLAGS += -DCDK_INCLUDE_CUSTOM_CONFIG -DBMD_INCLUDE_CUSTOM_CONFIG -DUSE_SYSTEM_LIBC -DBMD_CONFIG_INCLUDE_DMA=0 -DBMD_SYS_USLEEP=_usleep -DPHY_SYS_USLEEP=_usleep
CFLAGS += -DBCM_MDK_OS_DEP=1

ifneq ($(strip $(BUILD_BRCM_CMS)),)
CFLAGS += -DBRCM_CMS_BUILD
endif

ifneq ($(strip $(BRCM_VOICE_SUPPORT)),)
include $(BUILD_DIR)/make.voice
endif

#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 
#$(LIB):
#	$(CC) -shared -Wl,--whole-archive,-soname,$(notdir $@) -o $@ $(OBJS) -Wl,--no-whole-archive

$(LIB): conditional_build $(SDK_BASE)
	$(MAKE) -C build all

ifneq ($(findstring DPOE_OAM_BUILD, $(CMS_COMPILE_FLAGS)),)
conditional_build:
	mkdir -p $(INSTALL_DIR)/etc/X509
	install ./oamprocess/App/DpoeAuth/X509/certificate.default $(INSTALL_DIR)/etc/X509/certificate
	install ./oamprocess/App/DpoeAuth/X509/certificatehost.default $(INSTALL_DIR)/etc/X509/certificatehost
	install ./oamprocess/App/DpoeAuth/X509/rsakey.default $(INSTALL_DIR)/etc/X509/rsakey
else
conditional_build:
	@echo "Skipping DPoE Authentication (not configured)"
endif

# Create a symbol link to SDK_BASE Dir, aim to reduce the length of compile option lines(a large mount of -I(SDK_BASE)/xxx)
$(SDK_BASE):
	@ln -s $(CURR_DIR) $(SDK_BASE)
	@ln -s $(CURR_DIR) build/$(SDK_BASE)

$(SDK_BASE)_clean: target_clean
	rm -f $(SDK_BASE)
	rm -f build/$(SDK_BASE)

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

#include $(BUILD_DIR)/make.deprules

#-include $(OBJS:.o=.d)

export SDK_BASE BUILD_DIR CPPFLAGS CFLAGS LIB ALLOWED_INCLUDE_PATHS_2
