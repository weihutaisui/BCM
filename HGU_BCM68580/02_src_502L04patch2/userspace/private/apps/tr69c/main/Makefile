#***********************************************************************
#
#  Copyright (c) 2007  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/

#
# In most cases, you only need to modify this first section.
#
LIB = main.a


all: $(LIB)

clean: generic_clean

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

FULL_SOURCE_OBJS = main.o event.o
BINARY_ONLY_OBJS = acsListener.o informer.o md5.o utils.o httpDownload.o

ifneq ($(strip $(BUILD_TR69C_BEEP)),)
BINARY_ONLY_OBJS += mainDbus.o eventDbus.o informerDbus.o httpDownloadDbus.o httpUploadDbus.o
else
FULL_SOURCE_OBJS += mainCms.o eventCms.o
BINARY_ONLY_OBJS += informerCms.o httpDownloadCms.o httpUploadCms.o
endif

OBJS = $(FULL_SOURCE_OBJS) $(BINARY_ONLY_OBJS)

$(LIB): $(OBJS)
	$(AR) -rcs $@ $(OBJS)

HAVESRC= $(wildcard informer.c)

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
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR) \
                         -I$(BCM_FSBUILD_DIR)/public/include \
                         -I$(BCM_FSBUILD_DIR)/private/include



# Set flags if user wants to link with OpenSSL
ifneq ($(strip $(BUILD_TR69C_SSL)),)
CFLAGS += -DUSE_SSL -DORIGINAL_OPENSSL -I$(BCM_FSBUILD_DIR)/public/include
endif

# Set flags if user wants to link with BCM SSL
ifneq ($(strip $(BUILD_TR69C_BCM_SSL)),)
CFLAGS += -DUSE_SSL -I$(BUILD_DIR)/userspace/private/libs/bcmssl/include
endif

ifeq ($(strip $(DEBUG)),true)
CFLAGS+=-DDEBUGSSL
endif

# add strophe include directory to support XMPP
ifneq ($(strip $(BUILD_XMPP)),)
CFLAGS += -I$(BCM_FSBUILD_DIR)/public/include/strophe
endif

ifeq ($(strip $(BRCM_USER_SSP)),y)
CFLAGS += $(SSP_TYP_COMPILER_OPTS)
endif

ifneq ($(strip $(BUILD_TR69C_BEEP)),)
ALLOWED_LIB_DIRS := /lib:/lib/private:/lib/public
LIBS += -L$(INSTALL_DIR)/lib -ldbus-1 -lpthread -lrt
LIBS += -L$(INSTALL_DIR)/lib/public -lexpat -lffi

ifneq ($(findstring uclibc, $(TOOLCHAIN_PREFIX)),)
LIBS += -lintl
endif

LIBS += -L$(INSTALL_DIR)/lib/public -lgio-2.0 -lglib-2.0 -lgmodule-2.0 -lgobject-2.0 -lgthread-2.0 -lz
CFLAGS += -I$(BCM_FSBUILD_DIR)/public/lib/glib-2.0/include -I$(BCM_FSBUILD_DIR)/public/include/glib-2.0
CFLAGS += -I$(BCM_FSBUILD_DIR)/public/include/gio-unix-2.0
CFLAGS += -DBUILD_TR69C_BEEP
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

ifeq ($(strip $(HAVESRC)),)
acsListener.o: acsListener.o.$(PROFILE_ARCH).save
	cat $< >$@

httpDownload.o: httpDownload.o.$(PROFILE_ARCH).save
	cat $< >$@

httpDownloadCms.o: httpDownloadCms.o.$(PROFILE_ARCH).save
	cat $< >$@

httpDownloadDbus.o: httpDownloadDbus.o.$(PROFILE_ARCH).save
	cat $< >$@

httpUpload.o: httpUpload.o.$(PROFILE_ARCH).save
	cat $< >$@
	
httpUploadCms.o: httpUploadCms.o.$(PROFILE_ARCH).save
	cat $< >$@

httpUploadDbus.o: httpUploadDbus.o.$(PROFILE_ARCH).save
	cat $< >$@

informer.o: informer.o.$(PROFILE_ARCH).save
	cat $< >$@

informerCms.o: informerCms.o.$(PROFILE_ARCH).save
	cat $< >$@

informerDbus.o: informerDbus.o.$(PROFILE_ARCH).save
	cat $< >$@

mainDbus.o: mainDbus.o.$(PROFILE_ARCH).save
	cat $< >$@

eventDbus.o: eventDbus.o.$(PROFILE_ARCH).save
	cat $< >$@

md5.o: md5.o.$(PROFILE_ARCH).save
	cat $< >$@

utils.o: utils.o.$(PROFILE_ARCH).save
	cat $< >$@

-include $(FULL_SOURCE_OBJS:.o=.d)

else
-include $(OBJS:.o=.d)
endif


