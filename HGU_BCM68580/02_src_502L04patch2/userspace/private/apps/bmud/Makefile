#***********************************************************************
# <:copyright-BRCM:2010:proprietary:standard
# 
#    Copyright (c) 2010 Broadcom 
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
#:>
#***********************************************************************/

EXE  = bmud

BFC_TARGET_OS = Linux

all dynamic install: conditional_build

clean:
	find . -name "*.[od]" -exec rm -f {} \;
	find . -name "*.d.[0-9]*" -exec rm -f {} \;
	rm -f $(EXE)
	rm -f $(INSTALL_DIR)/bin/$(EXE)
	rm -f $(INSTALL_DIR)/etc/init.d/bmud.sh
	rm -f $(INSTALL_DIR)/etc/rc3.d/S70bmud


ifeq ($(strip $(BRCM_CHIP)),6816)
BRCM_BMU_CHIP = 3382
endif
ifeq ($(strip $(BRCM_CHIP)),6818)
BRCM_BMU_CHIP = 3382
endif
ifeq ($(strip $(BRCM_CHIP)),6828)
BRCM_BMU_CHIP = 3382
endif
ifeq ($(strip $(BRCM_CHIP)),63138)
BRCM_BMU_CHIP = 3385
endif
ifeq ($(strip $(BRCM_CHIP)),63148)
BRCM_BMU_CHIP = 3385
endif

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
#

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

BMU_DIR = $(CURR_DIR)

OS_DIR = $(BMU_DIR)/Bfc/OperatingSystem
OS_OBJS = $(OS_DIR)/CountingSemaphore.o $(OS_DIR)/Event.o $(OS_DIR)/EventSet.o $(OS_DIR)/LnxCountingSemaphore.o \
          $(OS_DIR)/LnxEvent.o $(OS_DIR)/LnxEventSet.o $(OS_DIR)/LnxInterface.o $(OS_DIR)/LnxMessageQueue.o \
		  $(OS_DIR)/LnxOperatingSystem.o $(OS_DIR)/LnxTimeout.o $(OS_DIR)/LnxTimer.o $(OS_DIR)/LnxMutexEvent.o \
		  $(OS_DIR)/LnxMutexSemaphore.o $(OS_DIR)/MessageQueue.o $(OS_DIR)/MutexSemaphore.o $(OS_DIR)/OperatingSystemFactory.o  \
		  $(OS_DIR)/OperatingSystem.o $(OS_DIR)/SoftTimer.o $(OS_DIR)/SystemTime.o \
		  $(OS_DIR)/Thread.o $(OS_DIR)/Timer.o

BF_DIR = $(BMU_DIR)/Bfc
BF_OBJS = $(BF_DIR)/BcmBasicString.o $(BF_DIR)/Utilities.o $(BF_DIR)/EventPublisher.o $(BF_DIR)/IpAddress.o	$(BF_DIR)/MacAddress.o \
          $(BF_DIR)/IpV4Address.o $(BF_DIR)/IpV6Address.o $(BF_DIR)/NonVolSettings.o $(BF_DIR)/OctetBuffer.o $(BF_DIR)/CompletionHandlerACT.o \
          $(BF_DIR)/NonVolDevice.o $(BF_DIR)/NonVolSettingsAccessLock.o $(BF_DIR)/CompositeNonVolSettings.o	$(BF_DIR)/UnrecognizedNonVolSettings.o

BT_DIR = $(BMU_DIR)/Bfc/Battery
BT_OBJS = $(BT_DIR)/BatteryController.o $(BT_DIR)/BatteryNonVolSettings.o $(BT_DIR)/BatterySoftTimer.o $(BT_DIR)/BatteryThread.o \
          $(BT_DIR)/TemperatureCalibrator.o $(BT_DIR)/BatteryCommandTable.o

BU_DIR = $(BMU_DIR)/Bfc/UserInterface
BU_OBJS = $(BU_DIR)/MessageLogSettings.o $(BU_DIR)/MessageLog.o $(BU_DIR)/CommandTable.o  \
          $(BU_DIR)/CommandParameterList.o $(BU_DIR)/StringSetCommandParameter.o $(BU_DIR)/Command.o $(BU_DIR)/CommandParameter.o \
		  $(BU_DIR)/BoolCommandParameter.o $(BU_DIR)/StringCommandParameter.o $(BU_DIR)/UnsignedIntCommandParameter.o $(BU_DIR)/BcmBfcFuncCallCommand.o \
		  $(BU_DIR)/FlagCommandParameter.o $(BU_DIR)/SignedIntCommandParameter.o $(BU_DIR)/FloatCommandParameter.o

ifeq ($(strip $(BRCM_BMU_CHIP)),3382)
BB_DIR = $(BMU_DIR)/CableModemBsp/bsp_bcm93382/src
endif
ifeq ($(strip $(BRCM_BMU_CHIP)),3385)
BB_DIR = $(BMU_DIR)/CableModemBsp/bsp_bcm93385
endif

BB_OBJS = $(BB_DIR)/battery.o

BC_DIR = $(BMU_DIR)/CableModemBsp/bsp_common/common_src
BC_OBJS = $(BC_DIR)/batteryCommon.o	$(BC_DIR)/crc.o

BM_OBJS = $(BM_DIR)/VendorSpecificBattery.o $(BM_DIR)/alarmBatteryACT.o

ifeq ($(strip $(BRCM_BMU_CHIP)),3382)
# Note - the 6816 APM register definition is not exactly like the 3382, but close enough
# for the single OTP register that is being read through these included files
BR_DIR = $(BMU_DIR)/CableModemBsp/bsp_bcm93382/inc/Registers
endif
ifeq ($(strip $(BRCM_BMU_CHIP)),3385)
BR_DIR = $(BMU_DIR)/CableModemBsp/bsp_bcm93385
endif

BI_DIR = $(BMU_DIR)/CableModemBsp/bsp_common/common_inc
BX_DIR = $(BMU_DIR)/CableModemBsp/bsp_common/os/linux
BM_DIR = $(BMU_DIR)/Custom

OBJS = bmud.o $(OS_OBJS) $(BF_OBJS) $(BT_OBJS) $(BU_OBJS) $(BB_OBJS) $(BC_OBJS) $(BM_OBJS)

include $(BUILD_DIR)/make.common
LIBS_CMS-$(BUILD_BRCM_CMS) = $(CMS_COMMON_LIBS) $(CMS_CORE_LIBS)
LIBS = -lgcc_s -lm -lstlport -lpthread -lcms_util -lbcm_flashutil -lbcm_crc -lcms_msg -lbcm_boardctl -lrt $(LIBS_CMS-y)

CFLAGS += -DBCM_REDUCED_IMAGE_SIZE=0 -DLINEAR_CHARGER=1 -DCONFIG_NO_KERNEL_THREAD_INFO -DPROCESSOR_TYPE=BCMMIPS -DTARGETOS_Linux -D__linux__ -DDISABLE_STARTUP_LOGGING -DBFC_CONFIG_MESSAGE_LOGGING_OSTREAM_DEFAULT=cout -DBCM$(BRCM_BMU_CHIP)

# Uncomment to enable low-level debug prints
# CFLAGS += -DBCM_DEBUG_MSG=1

# Chips require a calibration procedure to allow accurate measurements of
# battery voltage and current. When in production, all products supporting batteries
# must use calibrated chips. Therefore, the battery code is made to stop running if
# loaded on a chip that is not calibrated. If you want to override this behavior for
# development or testing purposes, you can change the setting below from FALSE to TRUE.
# This will allow uncalibrated chips to charge batteries to 75% of their capacity only.
# WARNING: Production code MUST NOT have this setting set to TRUE
# WARNING: Production code MUST have ALLOW_UNTRIMMED_PARTS set to FALSE
# WARNING: Setting ALLOW_UNTRIMMED_PARTS to TRUE is done at your own risk
CFLAGS += -DALLOW_UNTRIMMED_PARTS=FALSE

#important to use signed-char, the code assumed it was as such
CFLAGS += -fsigned-char

# Check if the number of cells is specified in the target profile
ifeq ($(strip $(BUILD_BMU_0CELL)),y)
check_battery:
	echo ERROR!!! You must select the number of cells for the batteries through make menuconfig. Typically, 12V designs use 2-Cell batteries and 15V designs use 3-Cell batteries. Selecting the wrong number of cells can cause physical damage, so make sure you choose the right number of cells for your design.
	false
else
check_battery:
endif

# Apply setting for 2-cell
ifeq ($(strip $(BUILD_BMU_2CELL)),y)
CFLAGS += -DBFC_USEVIN12V
endif

CFLAGS +=$(SSP_MIN_COMPILER_OPTS)
LIBS += $(SSP_MIN_LIBS)


# These must match the board if the defaults in battery.c are not appropriate. Uncomment out or edit as necessary.
# The defaults for 2-Cell packs are 14.7kOhms and 3.24kOhms and for 3-Cell packs, 30.1kOhms and 4.12kOhms
# These are for a charge voltage of 8.6V with a 2-Cell pack
# CFLAGS += -DVBAT_ATTEN_UPPER_KOHMS=15.4 -DVBAT_ATTEN_LOWER_KOHMS=3.3
# These are for a charge voltage of 8.7V with a 2-Cell pack
# CFLAGS += -DVBAT_ATTEN_UPPER_KOHMS=11.5 -DVBAT_ATTEN_LOWER_KOHMS=2.43

#
# Private apps and libs are allowed to include header files from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I. -I$(BF_DIR) -I$(BX_DIR) -I$(BI_DIR) -I$(BM_DIR)

ALLOWED_INCLUDE_PATHS += -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR)

# obviously, this app needs to access kernel headers
ALLOWED_INCLUDE_PATHS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)  \
                         -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
                         -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)  \
                         -I$(INC_BRCMSHARED_PRIV_PATH)/$(BRCM_BOARD)

ALLOWED_INCLUDE_PATHS += -I$(BT_DIR) -I$(BU_DIR) -I$(OS_DIR) -I$(BR_DIR) -I$(BB_DIR)

ALLOWED_INCLUDE_PATHS += -I$(BUILD_DIR)/userspace/public/libs/stlport/include
LIBS += -L$(BUILD_DIR)/userspace/public/libs/stlport/lib
                      
#
# Private apps and libs are allowed to link with libraries from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_LIB_DIRS := /lib:/lib/private:/lib/public


# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors

ifneq ($(strip $(BUILD_BMU)),)
conditional_build: check_battery $(EXE)
	install -m 755 $(EXE) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/bmud
	mkdir -p $(INSTALL_DIR)/etc/init.d
	mkdir -p $(INSTALL_DIR)/etc/rc3.d
	install scripts/bmud.sh $(INSTALL_DIR)/etc/init.d
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S70bmud; ln -s ../init.d/bmud.sh S70bmud)
else
conditional_build:
	@echo "Skipping BMUD (not configured)"
endif

bmud: $(OBJS)
	$(CC) -o $@ $(OBJS) -Wl,-rpath,$(CMS_LIB_RPATH) $(CMS_LIB_PATH) $(LIBS)

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)

%.o: %.cpp
	$(CXX) -c -g -fPIC -Wall $(CXXFLAGS)  $(CFLAGS) $(CPPFLAGS) $< -o $@
