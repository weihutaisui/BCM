
#***********************************************************************
#
#  <:copyright-BRCM:2015:DUAL/GPL:standard
#  
#     Copyright (c) 2015 Broadcom 
#     All Rights Reserved
#  
#  Unless you and Broadcom execute a separate written software license
#  agreement governing use of this software, this software is licensed
#  to you under the terms of the GNU General Public License version 2
#  (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
#  with the following added to such license:
#  
#     As a special exception, the copyright holders of this software give
#     you permission to link this software with independent modules, and
#     to copy and distribute the resulting executable under terms of your
#     choice, provided that you also meet, for each linked independent
#     module, the terms and conditions of the license of that module.
#     An independent module is a module which is not derived from this
#     software.  The special exception does not apply to any modifications
#     of the software.
#  
#  Not withstanding the above, under no circumstances may you combine
#  this software in any way with any other Broadcom software provided
#  under a license other than the GPL, without Broadcom's express prior
#  written consent.
#  
#  :>
#
#***********************************************************************/

EXE = wanconf
OBJS = wanconf.o

LIBS = -lcms_util -lbcm_boardctl -lbcm_crc -lbcm_flashutil

all dynamic install: conditional_build

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
LIBS += -lcms_msg
endif

clean: generic_clean
	rm -f $(INSTALL_DIR)/bin/$(EXE)
	rm -f $(INSTALL_DIR)/etc/init.d/wanconf.sh
	rm -f $(INSTALL_DIR)/etc/rc3.d/S85wanconf

ifeq ($(strip $(RELEASE_BUILD)),)
	rm -f $(INSTALL_DIR)/etc/init.d/xrnd_init.sh
	rm -f $(INSTALL_DIR)/etc/rc3.d/S95xrndinit
endif
	rm -f $(EXE)
	rm -f $(OBJS)
	rm -f *.d

ifneq ($(strip $(WAN_AUTODETECT)),)	
conditional_build: $(EXE) generic_exe_install
	mkdir -p $(INSTALL_DIR)/etc/init.d
	mkdir -p $(INSTALL_DIR)/etc/rc3.d
	install scripts/wanconf.sh $(INSTALL_DIR)/etc/init.d
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S85wanconf; ln -s ../init.d/wanconf.sh S85wanconf)

ifeq ($(strip $(RELEASE_BUILD)),)
	install scripts/xrnd_init.sh $(INSTALL_DIR)/etc/init.d
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S95xrndinit; ln -s ../init.d/xrnd_init.sh S95xrndinit)
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
                         -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD) \
                         $(INC_RDP_FLAGS) \

#
# Private apps and libs are allowed to link with libraries from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_LIB_DIRS := /lib:/lib/private:/lib/public


CFLAGS += -Werror -Wfatal-errors
LIBS += -lrt -lrdpactl

#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 
$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS) -Wl,-rpath,$(CMS_LIB_RPATH) $(CMS_LIB_PATH) $(LIBS)


#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

$(OBJS): wanconf.c
	$(CC) -c $(CFLAGS) wanconf.c -DKERNELVER=\"${LINUX_VER_STR}\"
else
conditional_build: 
	@echo "skip wanconf"
endif

-include $(OBJS:.o=.d)

