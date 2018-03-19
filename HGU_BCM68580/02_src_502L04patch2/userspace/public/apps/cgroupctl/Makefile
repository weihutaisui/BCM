#***********************************************************************
#
#  <:copyright-BRCM:2012:DUAL/GPL:standard
#  
#     Copyright (c) 2012 Broadcom 
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

#
# In most cases, you only need to modify this first section.
#
EXE  = 
OBJS = 
LIBS = 

all dynamic install: conditional_build


clean:
	rm -f $(INSTALL_DIR)/etc/init.d/cgroups.sh
	rm -f $(INSTALL_DIR)/etc/rc3.d/S27cgroups



#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common



ifneq ($(strip $(BCM_CGROUPS)),)
conditional_build:
	mkdir -p $(INSTALL_DIR)/cgroups/cputree
	mkdir -p $(INSTALL_DIR)/etc/init.d
	mkdir -p $(INSTALL_DIR)/etc/rc3.d
	install scripts/cgroups.sh $(INSTALL_DIR)/etc/init.d
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S27cgroups; ln -s ../init.d/cgroups.sh S27cgroups)
	@if [ -e scripts/cgroups.conf.custom ]; then \
	  echo Using custom cgroups.conf; \
	  install scripts/cgroups.conf.custom $(INSTALL_DIR)/etc/cgroups.conf; \
	else \
	  install scripts/cgroups.conf $(INSTALL_DIR)/etc/cgroups.conf; \
	fi
else
conditional_build:
	@echo "Skipping cgroupctl (not configured)"
endif


#
# Private apps and libs are allowed to include header files from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I.\
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR)



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


#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 
$(EXE): $(OBJS) generic_oaldir
	$(CC) -o $@ $(OBJS) $(OALDIR)/oal.a -Wl,-rpath,$(CMS_LIB_RPATH) $(CMS_LIB_PATH) $(LIBS)



#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)


