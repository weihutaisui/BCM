#
#
# Copyright (c) 2003-2012  Broadcom Corporation
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
# $Id: Makefile,v 1.12.12.2 2003/10/31 21:31:36 mthawani Exp $
#

ifndef DEBUG
#export DEBUG=1
endif

SIZE = $(CROSS_COMPILE)size
RANLIB = $(CROSS_COMPILE)ranlib

VPATH.h=.:..:$(SRCBASE)/include:$(SRCBASE)/router/shared

vpath %.c .:..:$(SRCBASE)/router/shared
vpath %.h $(VPATH.h)

ifeq ($(DEBUG),1)
CFLAGS += -g -DDEBUG -UNDEBUG -DBCMDBG
else
CFLAGS += -s -Os -DNDEBUG
endif
CFLAGS += -I$(subst :, -I,$(VPATH.h))
CFLAGS += -Wall -Wunused 
#CFLAGS += --trace-includes
CFLAGS +=  $(CMS_COMPILE_FLAGS) -I$(BUILD_DIR)/userspace/public/include -I$(BUILD_DIR)/userspace/public/include/linux -I$(BUILD_DIR)/userspace/private/include

SOURCES = linux_osl.c upnp.c ssdp.c http.c gena.c soap.c service.c device.c uio.c

SOURCES.OBJ := $(SOURCES)
SOURCES.OBJ := $(patsubst %.c,%.o,$(SOURCES.OBJ))

vpath %.c .:..:
vpath %.h .:..:$(SRCBASE)/include

TARGET = libupnp.a
LIBS = 

all : $(TARGET)


$(SOURCES.OBJ): Makefile $(SRCBASE)/include $(SRCBASE)/include/proto 

$(TARGET) : $(SOURCES.OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<
	$(AR) -rcs $(TARGET) $(SOURCES.OBJ) 
	$(RANLIB) $(TARGET)
	$(SIZE)  $(TARGET)

clean: 
	rm -f *.o *.a



timertest: $(OBJDIR)/linux_timer.o $(OBJDIR)/timertest.o
	$(CC) $(OBJDIR)/linux_timer.o $(OBJDIR)/timertest.o -o timertest
