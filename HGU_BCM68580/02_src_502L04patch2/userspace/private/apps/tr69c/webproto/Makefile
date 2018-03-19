#***********************************************************************
#
#  Copyright (c) 2007  Broadcom Corporation
#  All Rights Reserved
#
#  <:label-BRCM:2011:proprietary:standard
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
:>
#
#***********************************************************************/

#
# In most cases, you only need to modify this first section.
#
LIB = webproto.a
OBJS = protocol.o wget.o www.o

all: $(LIB)

clean: generic_clean

$(LIB): $(OBJS)
	$(AR) -rcs $@ $(OBJS)


#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
# You do not need to modify this part.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, ,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

ifneq ($(strip $(BRCM_VOICE_SUPPORT)),)
include $(BUILD_DIR)/make.voice
endif

ifeq ($(strip $(BRCM_USER_SSP)),y)
CFLAGS += $(SSP_TYP_COMPILER_OPTS)
endif



#
# Private apps and libs are allowed to include from private and public.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I. -I.. \
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR)



# Set flags if user wants to link with OpenSSL
ifneq ($(strip $(BUILD_TR69C_SSL)),)
CFLAGS += -DUSE_SSL -DORIGINAL_OPENSSL -I$(BCM_FSBUILD_DIR)/public/include
endif

# Set flags if user wants to link with BCM SSL
ifneq ($(strip $(BUILD_TR69C_BCM_SSL)),)
CFLAGS += -DUSE_SSL -I$(BUILD_DIR)/userspace/private/libs/bcmssl/include
endif

ifneq ($(strip $(BUILD_TR69C_BEEP)),)
CFLAGS += -I$(BCM_FSBUILD_DIR)/public/lib/glib-2.0/include -I$(BCM_FSBUILD_DIR)/public/include/glib-2.0
CFLAGS += -I$(BCM_FSBUILD_DIR)/public/include/gio-unix-2.0
CFLAGS += -DBUILD_TR69C_BEEP
endif


ifeq ($(strip $(DEBUG)),true)
CFLAGS+=-DDEBUGSSL
endif

# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors

#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 


#
# generic rule makes the library oal.a
# see make.common
#


#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)


