#***********************************************************************
#
#  Copyright (c) 2007  Broadcom Corporation
#  All Rights Reserved
#
#  <:label-BRCM:2012:proprietary:standard
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
EXE = cellularapp
OBJS = cellularapp.o cellular_msg.o cellular_internal.o cms_access.o dc_tracker.o  \
       service_state_tracker.o uicc_controller.o call_tracker.o data_connection.o  \
       parcel.o sms_tracker.o ril_access.o timer_manager.o icc_file_handler.o \
       adn_record_loader.o misc_tracker.o

LIBS += -lcms_dal -lcms_msg $(CMS_COMMON_LIBS) -ldl $(CMS_CORE_LIBS)


# Define date and time for use if we're making a zip file
ZIP_FILE := $(shell date '+%Y%m%d_%H%M' 2>/dev/null)

all dynamic install : conditional_build

clean: generic_clean
	rm -f $(INSTALL_DIR)/bin/$(EXE)

ifneq ($(strip $(BUILD_CELLULAR)),)
conditional_build: $(EXE) generic_exe_install
else
conditional_build:
	@echo "skipping cellularapp (not configured)"
endif


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
ifneq ($(strip $(BUILD_VODSL)),)
include $(BUILD_DIR)/make.voice
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
                         -I$(BUILD_DIR)/userspace/public/apps/samos_ril/include \
                         -I$(BUILD_DIR)/userspace/public/apps/samos_ril/include/telephony \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/libs/cms_core/linux/device2 \
                         -I$(BUILD_DIR)/shared/broadcom/include/bcm963xx 

#
# Private apps and libs are allowed to link with libraries from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_LIB_DIRS := /lib:/lib/private:/lib/public


CFLAGS += -Werror
CFLAGS += -DCONFIG_BCM9$(BRCM_CHIP) -DFEA_TEMP_SAMOS_ROUTER_AUDIO_SUPPORT_PHASE_1


ifneq ($(strip $(BUILD_BRCM_CMS)),)
CFLAGS += -DBRCM_CMS_BUILD
endif



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


