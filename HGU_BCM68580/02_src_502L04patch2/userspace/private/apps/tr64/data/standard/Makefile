#***********************************************************************
#
# Copyright (c) 2006-2012 Broadcom Corporation
# All Rights Reserved
#
# <:label-BRCM:2012:proprietary:standard
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
#***********************************************************************/
TR64STDLIB = libtr64std.o
all : $(TR64STDLIB)

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

ALLOWED_INCLUDE_PATHS := -I.\
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR)

INC_PATH = -I ../../inc 
CFLAGS += $(INC_PATH) $(ALLOWED_INCLUDE_PATHS)

ifneq ($(strip $(BUILD_TR64_LAYER3)),)
	CFLAGS+=-DINCLUDE_LAYER3
endif

ifneq ($(strip $(BUILD_TR64_DEVICEINFO)),)
	CFLAGS+=-DINCLUDE_DEVICEINFO
endif

ifneq ($(strip $(BUILD_TR64_DEVICECONFIG)),)
	CFLAGS+=-DINCLUDE_DEVICECONFIG
endif

ifneq ($(strip $(BUILD_TR64_LANCONFIGSECURITY)),)
	CFLAGS+=-DINCLUDE_LANCONFIGSECURITY
endif

ifneq ($(strip $(BUILD_TR64_MANAGEMENTSERVER)),)
	CFLAGS+=-DINCLUDE_MANAGEMENTSERVER
endif

ifneq ($(strip $(BUILD_TR64_TIME)),)
	CFLAGS+=-DINCLUDE_TIME
endif

ifneq ($(strip $(BUILD_TR64_USERINTERFACE)),)
	CFLAGS+=-DINCLUDE_USERINTERFACE
endif

ifneq ($(strip $(BUILD_TR64_LAYER2BRIDGE)),)
	CFLAGS+=-DINCLUDE_LAYER2BRIDGE
endif

ifneq ($(strip $(BUILD_TR64_QUEUEMANAGEMENT)),)
	CFLAGS+=-DINCLUDE_QUEUEMANAGEMENT
endif

ifneq ($(strip $(BUILD_TR64_LANHOSTCONFIGMGMT)),)
	CFLAGS+=-DINCLUDE_LANHOSTCONFIGMGMT
endif

ifneq ($(strip $(BUILD_TR64_LANETHINTERFACECONFIG)),)
	CFLAGS+=-DINCLUDE_LANETHINTERFACECONFIG
endif

ifneq ($(strip $(BUILD_TR64_WLANCONFIG)),)
	CFLAGS+=-DINCLUDE_WLANCONFIG
endif

ifneq ($(strip $(BUILD_TR64_LANUSBINTERFACECONFIG)),)
	CFLAGS+=-DINCLUDE_LANUSBINTERFACECONFIG
endif

ifneq ($(strip $(BUILD_TR64_LANHOSTS)),)
	CFLAGS+=-DINCLUDE_LANHOSTS
endif

ifneq ($(strip $(BUILD_TR64_WANCOMMONINTERFACE)),)
	CFLAGS+=-DINCLUDE_WANCOMMONINTERFACE
endif

ifneq ($(strip $(BUILD_TR64_WANDSLINTERFACE)),)
	CFLAGS+=-DINCLUDE_WANDSLINTERFACE
endif

ifneq ($(strip $(BUILD_TR64_WANETHERNETCONFIG)),)
	CFLAGS+=-DINCLUDE_WANETHERNETCONFIG
endif

ifneq ($(strip $(BUILD_TR64_WANDSLCONNECTIONMGMT)),)
	CFLAGS+=-DINCLUDE_WANDSLCONNECTIONMGMT
endif

ifneq ($(strip $(BUILD_TR64_WANDSLDIAGNOSTICS)),)
	CFLAGS+=-DINCLUDE_WANDSLDIAGNOSTICS
endif

ifneq ($(strip $(BUILD_TR64_WANPOTSLINKCONFIG)),)
	CFLAGS+=-DINCLUDE_WANPOTSLINKCONFIG
endif

ifneq ($(strip $(BUILD_TR64_WANDSLLINKCONFIG)),)
	CFLAGS+=-DINCLUDE_DSLLINKCONFIG
endif

ifneq ($(strip $(BUILD_TR64_WANCABLELINKCONFIG)),)
	CFLAGS+=-DINCLUDE_WANCABLELINKCONFIG
endif

ifneq ($(strip $(BUILD_TR64_WANETHERNETLINKCONFIG)),)
	CFLAGS+=-DINCLUDE_WANETHERNETLINKCONFIG
endif

ifneq ($(strip $(BUILD_TR64_WANIPCONNECTION)),)
	CFLAGS+=-DINCLUDE_IPCONNECTION
endif

ifneq ($(strip $(BUILD_TR64_WANPPPCONNECTION)),)
	CFLAGS+=-DINCLUDE_PPPCONNECTION
endif

ifneq ($(strip $(BUILD_TR64_IPPINGDIAG)),)
        CFLAGS+=-DINCLUDE_IPPINGDIAG       
endif

ifneq ($(strip $(BUILD_TR64_WANATMF5LOOPBACKDIAG)),)
        CFLAGS+=-DINCLUDE_ATMF5LOOPBACKDIAG
endif

#
# Private apps and libs are allowed to link with libraries from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_LIB_DIRS := /lib:/lib/private:/lib/public


CFLAGS += $(SSP_TYP_COMPILER_OPTS)


OBJS = igdhandlers.o igdparams.o 

ifneq ($(strip $(BUILD_TR64_LAYER3)),)
	OBJS +=layer3handlers.o layer3params.o 
endif

ifneq ($(strip $(BUILD_TR64_DEVICEINFO)),)
	OBJS +=deviceinfohandlers.o deviceinfoparams.o 
endif

ifneq ($(strip $(BUILD_TR64_DEVICECONFIG)),)
	OBJS +=deviceconfigparams.o deviceconfighandlers.o 
endif

ifneq ($(strip $(BUILD_TR64_LANCONFIGSECURITY)),)
	OBJS +=lancfgsecparams.o lancfgsechandlers.o 
endif

ifneq ($(strip $(BUILD_TR64_MANAGEMENTSERVER)),)
	OBJS +=mgtserverhandlers.o mgtserverparams.o 
endif

ifneq ($(strip $(BUILD_TR64_TIME)),)
	OBJS +=timeparams.o timehandlers.o 
endif

ifneq ($(strip $(BUILD_TR64_USERINTERFACE)),)
endif

ifneq ($(strip $(BUILD_TR64_LAYER2BRIDGE)),)
	OBJS +=layer2bridgeparams.o layer2bridgehandlers.o  
endif

ifneq ($(strip $(BUILD_TR64_QUEUEMANAGEMENT)),)
	OBJS +=queuemgtparams.o queuemgthandlers.o 
endif

ifneq ($(strip $(BUILD_TR64_LANHOSTCONFIGMGMT)),)
	OBJS +=lanhostcfgmgthandlers.o lanhostcfgmgtparams.o 
endif

ifneq ($(strip $(BUILD_TR64_LANETHINTERFACECONFIG)),)
	OBJS +=lanethifcfghandlers.o lanethifcfgparams.o 
endif

ifneq ($(strip $(BUILD_TR64_WLANCONFIG)),)
	OBJS +=lanwlancfghandlers.o lanwlancfgparams.o 
endif

ifneq ($(strip $(BUILD_TR64_LANUSBINTERFACECONFIG)),)
	OBJS +=lanusbifcfghandlers.o lanusbifcfgparams.o 
endif

ifneq ($(strip $(BUILD_TR64_LANHOSTS)),)
	OBJS +=lanhostsparams.o lanhostshandlers.o
endif

ifneq ($(strip $(BUILD_TR64_WANCOMMONINTERFACE)),)
	OBJS +=wancommonhandlers.o wancommonparams.o  
endif

ifneq ($(strip $(BUILD_TR64_WANDSLINTERFACE)),)
	OBJS +=wandslifcfgparams.o wandslifcfghandlers.o
endif

ifneq ($(strip $(BUILD_TR64_WANETHERNETCONFIG)),)
	OBJS +=wanethlinkconfigparams.o wanethlinkconfighandlers.o
endif

ifneq ($(strip $(BUILD_TR64_WANDSLCONNECTIONMGMT)),)
	OBJS +=wandslconnmgtparams.o wandslconnmgthandlers.o 
endif

ifneq ($(strip $(BUILD_TR64_WANDSLDIAGNOSTICS)),)
endif

ifneq ($(strip $(BUILD_TR64_WANPOTSLINKCONFIG)),)
endif

ifneq ($(strip $(BUILD_TR64_WANDSLLINKCONFIG)),)
	OBJS +=wandsllinkconfighandlers.o wandsllinkconfigparams.o 
endif

ifneq ($(strip $(BUILD_TR64_WANCABLELINKCONFIG)),)
endif

ifneq ($(strip $(BUILD_TR64_WANETHERNETLINKCONFIG)),)
	OBJS +=wanethifcfghandlers.o wanethifcfgparams.o 
endif

ifneq ($(strip $(BUILD_TR64_WANIPCONNECTION)),)
	OBJS +=wanipconnectionhandlers.o wanipconnectionparams.o 
endif

ifneq ($(strip $(BUILD_TR64_WANPPPCONNECTION)),)
	OBJS +=wanpppconnectionhandlers.o wanpppconnectionparams.o  
endif

ifneq ($(strip $(BUILD_TR64_IPPINGDIAG)),)
	OBJS +=ippingparams.o ippinghandlers.o 
endif

ifneq ($(strip $(BUILD_TR64_WANATMF5LOOPBACKDIAG)),)
	OBJS +=atmf5loopparams.o atmf5loophandlers.o 
endif

$(TR64STDLIB):$(OBJS) 
	$(LD) $(EXTRA_LDFLAGS) -r -o $(TR64STDLIB) $(OBJS) 

%.o: %.c Makefile
	$(CC) $(BCM_LD_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c -o $@ $<	
	
clean :
	rm -f *~ $(OBJS) $(TR64STDLIB)
	rm -f *.o *.d *.d.*
	
#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)	
