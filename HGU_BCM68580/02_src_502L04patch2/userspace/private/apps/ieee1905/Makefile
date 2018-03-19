#
# <:copyright-BRCM:2011:proprietary:standard
# 
#    Copyright (c) 2011 Broadcom 
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
#

all dynamic install: conditional_build

EXE := ieee1905
SRCS := $(shell find . -name "ieee*.c")
O_SAVED := $(shell find . -name "ieee*.$(PROFILE_ARCH).o_saved")
OBJS := $(sort $(SRCS:.c=.o) $(O_SAVED:.$(PROFILE_ARCH).o_saved=.o))
LINK_OBJS := $(OBJS)

EID_FILE = eid_bcm_1905.txt

ifeq ($(strip $(BRCM_1905_FM)),)
#$(info REMOVING FM FROM LINK)
LINK_OBJS := $(filter-out ./ieee1905_flowmanager.o,$(LINK_OBJS))
endif

#$(info LINK_OBJS $(LINK_OBJS))
#$(info OBJS $(OBJS))

EXE2 = i5ctl
LN2  = i5
SRCS2 = $(shell find . -name "i5ctl*.c")
OBJS2 = $(SRCS2:.c=.o)

EXE3 = ncap
SRCS3 = $(shell find . -name "i5ncap*.c")
OBJS3 = $(SRCS3:.c=.o)

LIBS = -li5api
CUSTOM_CFLAGS += -L$(INSTALL_DIR)/lib/

CUSTOM_CFLAGS += -Werror=return-type -Werror=uninitialized -Wall -Werror -Wfatal-errors
CUSTOM_CFLAGS += $(CMS_COMPILE_FLAGS)

ifeq ($(strip $(BUILD_DIR)),)
CC := gcc
CFLAGS := $(CUSTOM_CFLAGS) -g -DDESKTOP=1
all:
i5api.o: i5api.c i5api.h
	$(CC) -fPIC $(CFLAGS) -c $<

libi5api.so: i5api.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $?

$(EXE) $(EXE2) $(EXE3): libi5api.so

LIBS += -L.

LIBS += -lrt
BUILD_IEEE1905 := y
else
LIBS += -lcms_util -lbcm_flashutil -lbcm_crc -lcms_msg -lbcm_boardctl
ifneq ($(strip $(BUILD_ETHSWCTL)),)
LIBS +=  -lethswctl
endif 
ifneq ($(strip $(BUILD_FBCTL)),)
LIBS += -lfbctl
endif
ifneq ($(strip $(BUILD_FCCTL)),)
LIBS += -lfcctl
endif

CUSTOM_CFLAGS += -DWANDEV_SUPPORT -DIEEE1905_KERNEL_MODULE_PB_SUPPORT -DSUPPORT_BRIDGE_DEDICATED_PORT
ifneq ($(strip $(BUILD_BRCM_CMS)),)
CUSTOM_CFLAGS += -DCMS_BOARD_UTIL_SUPPORT
endif
endif

ifeq ($(WIRELESS),1)
DSLCPE_WLCSM_EXT=0
ifeq ($(DSLCPE_WLCSM_EXT),1)
CUSTOM_CFLAGS += -DDSLCPE_WLCSM_EXT
CFLAGS += -DDSLCPE_WLCSM_EXT
LIBS += -lwlcsm -lnvram 
CFLAGS += -L../wlan/wlcsm  -L$(INSTALL_DIR)/lib
endif
endif

all dynamic install: conditional_build

#
# make.common provides definitions to control what gets built in this code
# E.g. -DWIRELESS, -DSUPPORT_HOMEPLUG and -DSUPPORT_FBCTL
#
ifneq ($(strip $(BUILD_DIR)),)
include $(BUILD_DIR)/make.common
all dynamic install: generic_eid_file_install
clean: generic_eid_file_clean
endif

ifneq ($(strip $(BUILD_BRCM_CMS)),)
LIBS += -lcms_msg $(CMS_COMMON_LIBS) -ldl $(CMS_CORE_LIBS) -lcms_dal
endif

CFLAGS += $(SSP_TYP_COMPILER_OPTS)
LIBS += $(SSP_TYP_LIBS)

ifneq ($(strip $(BUILD_IEEE1905)),)
conditional_build: $(EXE) $(EXE2) $(EXE3)
	mkdir -p $(INSTALL_DIR)/bin
	install -m 755 $(EXE) $(INSTALL_DIR)/bin
	install -m 755 $(EXE2) $(INSTALL_DIR)/bin
	install -m 755 $(EXE3) $(INSTALL_DIR)/bin
ifeq ($(strip $(BUILD_BRCM_CMS)),)
	@echo "Setting 1905 to start via init.d"
	mkdir -p $(INSTALL_DIR)/etc/init.d
	mkdir -p $(INSTALL_DIR)/etc/rc3.d
	install scripts/ieee1905.sh $(INSTALL_DIR)/etc/init.d
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S70ieee1905; ln -s ../init.d/ieee1905.sh S70ieee1905)
endif
	ln -sf $(EXE2) $(INSTALL_DIR)/bin/$(LN2)

ifneq ($(strip $(BRCM_1905_TOPOLOGY_WEB_PAGE)),)
#--------------------------------------------------------------------------------
# eval call CREATE_WEB_TRG creates the makefile target install_web_files.  This
# target is used to install and minimize web files.   See make.common for details
all dynamic install: install_web_files
install_web_files: conditional_build
$(eval $(call CREATE_WEB_TRG, install_web_files, html, $(INSTALL_DIR)/webs, staging, minimized_webs.tar))
#--------------------------------------------------------------------------------
endif

else
conditional_build:
	@echo "Skipping IEEE1905 (not configured)"
endif

clean:
	rm -f *.[od] *.d.* $(EXE) $(EXE2) $(EXE3) *.so
	rm -f $(INSTALL_DIR)/bin/$(EXE)
	rm -f $(INSTALL_DIR)/bin/$(EXE2)
	rm -f $(INSTALL_DIR)/bin/$(EXE3)
	rm -f $(INSTALL_DIR)/bin/$(LN2)
	rm -f $(INSTALL_DIR)/etc/init.d/ieee1905.sh
	rm -f $(INSTALL_DIR)/etc/rc3.d/S70ieee1905

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
#
BUILD_DIR:=$(subst /targets, /targets, $(INSTALL_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

#
# Private apps and libs are allowed to include header files from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I.
ALLOWED_INCLUDE_PATHS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) \
                         -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD) \
                         -I$(BUILD_DIR)/userspace/private/apps/wlan/wlmngr \
                         -I$(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/wps/brcm_apps/include \
                         -I$(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/main/src/wps/brcm_apps/include \
                         -I$(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/common/include \
                         -I$(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/main/src/common/include \
                         -I$(KERNEL_LINKS_DIR)

ifneq ($(BRCM_1905_TOPOLOGY_WEB_PAGE),)
CFLAGS += -DSUPPORT_1905_TOPOLOGY_WEB_PAGE
ALLOWED_INCLUDE_PATHS += -I$(BCM_FSBUILD_DIR)/public/include
ALLOWED_INCLUDE_PATHS += -I$(BCM_FSBUILD_DIR)/public/include/websockets
ALLOWED_INCLUDE_PATHS += -I$(BUILD_DIR)/userspace/public/libs/httpdshared
LIBS += -lwebsockets -lssl -lcrypto -lhttpdshared
endif

ifeq ($(DSLCPE_WLCSM_EXT),1)
ALLOWED_INCLUDE_PATHS += -I$(BUILD_DIR)/userspace/private/apps/wlan/wlcsm/include 
endif

#
# Private apps and libs are allowed to link with libraries from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_LIB_DIRS := /lib:/lib/private:/lib/public


#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
#
$(EXE): $(OBJS)
	$(CC) -o $@ $(LINK_OBJS) -Wl,-rpath,$(CMS_LIB_RPATH) $(CMS_LIB_PATH) $(LIBS)

$(EXE2): $(OBJS2)
	$(CC) -o $@ $(OBJS2) -Wl,-rpath,$(CMS_LIB_RPATH) $(CMS_LIB_PATH) $(LIBS)

$(EXE3): $(OBJS3)
	$(CC) -o $@ $(OBJS3) -Wl,-rpath,$(CMS_LIB_RPATH) $(CMS_LIB_PATH) $(LIBS)

ifneq ($(strip $(BUILD_IEEE1905)),)
#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#
ifneq ($(strip $(BUILD_DIR)),)
include $(BUILD_DIR)/make.deprules
endif

-include $(OBJS:.o=.d) $(OBJS2:.o=.d) $(LIBOBJS:.o=.d)
endif


#
# we do not want to produce debug symbols for flowmanager.o
#
ieee1905_flowmanager.o: CFLAGS += -g0

#
# Creating an implicit rule to create a .o file 
# from a .o_saved file.  Note: because this is 
# the last pattern rule mentioned, it will have 
# lowest precedence
#
%.o : %.$(PROFILE_ARCH).o_saved
	@cp $< $@






