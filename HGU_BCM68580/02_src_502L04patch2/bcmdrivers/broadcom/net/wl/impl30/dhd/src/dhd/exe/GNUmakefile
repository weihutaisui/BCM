#
# GNUmakefile for dhd/exe
#
# Copyright 2017 Broadcom
#
# This program is the proprietary software of Broadcom and/or
# its licensors, and may only be used, duplicated, modified or distributed
# pursuant to the terms and conditions of a separate, written license
# agreement executed between you and Broadcom (an "Authorized License").
# Except as set forth in an Authorized License, Broadcom grants no license
# (express or implied), right to use, or waiver of any kind with respect to
# the Software, and Broadcom expressly reserves all rights in and to the
# Software and all intellectual property rights therein.  IF YOU HAVE NO
# AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
# WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
# THE SOFTWARE.
#
# Except as expressly set forth in the Authorized License,
#
# 1. This program, including its structure, sequence and organization,
# constitutes the valuable trade secrets of Broadcom, and you shall use
# all reasonable efforts to protect the confidentiality thereof, and to
# use this information only in connection with your use of Broadcom
# integrated circuit products.
#
# 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
# "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
# REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
# OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
# DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
# NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
# ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
# CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
# OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
#
# 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
# BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
# SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
# IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
# IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
# ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
# OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
# NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
#
# <<Broadcom-WL-IPTag/Proprietary:>>
#
# $Id: GNUmakefile,v 1.27 2010-09-13 14:20:34 $

WLAN_ComponentsInUse := bcmwifi
ifeq ($(CONFIG_DHDAP),y)
    export WLAN_TreeBaseA=$(abspath ../../..)
endif
include ../../makefiles/WLAN_Common.mk
SRCBASE := $(WLAN_SrcBaseR)

UNAME = $(shell uname)

#-----------------------------------------------------------------
# Windows build
# 1) windows, don't include Makerules due to all: conflict

ifeq ($(findstring CYGWIN,$(UNAME)),CYGWIN)

include ../../GNUmakefile.inc

else # UNAME

# 2) not windows, need to include first to pick up TARGETENV dependent vars
include $(SRCBASE)/Makerules

# use nl80211 for driver access (ioctl replacement)
ifdef NL80211
CFLAGS += -DNL80211
endif

#ifdef BCMWPA2
CFLAGS += -DBCMWPA2
#endif // endif
# Always include pktgen commands in the application
CFLAGS += -DSDTEST
#ifndef OEM_ANDROID
CFLAGS += -DBCMSPI
#endif // endif

#ifdef WLCNT
CFLAGS += -DWLCNT
#endif // endif

CFLAGS += -DBCMDONGLEHOST
CFLAGS += -DBCM_DHD_RUNNER

CFLAGS += -DWLPFN -DLINUX -g -I$(SRCBASE)/wl/exe

ifneq (,$(findstring freebsd, $(TARGETENV)))
    CC := $(CROSS_COMPILE)gcc
    # CFLAGS += -D__FreeBSD__
    CFLAGS += -I$(FREEBSD_DIR)
    ifneq (,$(findstring freebsd_be, $(TARGETENV)))
        CFLAGS += -DIL_BIGENDIAN
    endif
endif

vpath %.c $(SRCBASE)/shared $(SRCBASE)/wl/exe $(WLAN_StdSrcDirsR) $(WLAN_ComponentSrcDirsR)

#-----------------------------------------------------------------
# Linux build
#
# This should be one of values recognized in src/Makerules

# Change to the new dhd driver trunk build structure as discussed
# by the build team
ifeq ($(BUILD_TAG),TRUNK)

# $(TARGETARCH) is set based on TARGETENV in src/Makerules.* files
DHD_OBJS := $(TARGETARCH)/dhdu.o $(TARGETARCH)/dhdu_linux.o $(TARGETARCH)/bcmutils.o $(TARGETARCH)/bcmxtlv.o $(TARGETARCH)/bcmwifi_channels.o $(TARGETARCH)/wlu_client_shared.o $(TARGETARCH)/wlu_pipe_linux.o $(TARGETARCH)/wlu_pipe.o $(TARGETARCH)/miniopt.o $(TARGETARCH)/bcm_app_utils.o $(TARGETARCH)/ucode_download.o $(TARGETARCH)/wlu_common.o
DHD_EXE  := $(TARGETARCH)/dhd

include netlink.inc

all: $(DHD_EXE)

clean:
	 rm -fr $(DHD_EXE) $(DHD_OBJS) $(TARGETARCH)

$(DHD_EXE): $(DHD_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(TARGETARCH)/%.o: %.c
	@mkdir -p $(TARGETARCH)
	$(CC) -c $(CFLAGS) -DRWL_SOCKET -DRWL_WIFI -DRWL_DONGLE -o $@ $^

else # BUILD_TAG

# Preserve the old build structure for other components that still haven't not
# changed to the new build structure, once everyone moved to the new structure
# , we can remove the whole else BUILD_TAG code

ifneq ($(findstring $(TARGETENV), "linux linuxmips linuxmips_be linuxarm linuxarm_le android android_ndk_r6b freebsd freebsd_be"),)
# $(TARGETARCH) is set based on TARGETENV in src/Makerules.* files
DHD_OBJS := dhdu.o dhdu_linux.o bcmutils.o bcmxtlv.o bcmwifi_channels.o wlu_client_shared.o wlu_pipe_linux.o wlu_pipe.o miniopt.o bcm_app_utils.o ucode_download.o wlu_common.o
DHD_EXE  := dhd
DHD_OBJS := $(DHD_OBJS:%.o=%.o)

include netlink.inc

ifeq ($(findstring $(TARGETARCH), "x86 x86_mmx"),)
  DHD_EXE  := $(DHD_EXE)$(TARGETARCH)
  DHD_OBJS := $(DHD_OBJS:%.o=$(TARGETARCH)/%.o)
endif

all: $(DHD_EXE)

clean:
	 rm -fr $(DHD_EXE) $(DHD_OBJS) $(TARGETARCH)

$(DHD_EXE): $(DHD_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

# net_priv target is for building the dhd with NET_ADMIN privilege (i.e. to avoid running as root)
# note: The 'setcap' command is availble from the libcap2-bin package
# note: linux kernel < 2.6.33 doesn't support Security File Capabilities by default
net_priv: $(DHD_EXE)
	sudo setcap CAP_NET_ADMIN=eip $^

%.o: %.c
	$(CC) -c $(CFLAGS) -DRWL_SOCKET -DRWL_WIFI -DRWL_DONGLE -o $@ $^

ifeq ($(findstring $(TARGETARCH), "x86 x86_mmx"),)
$(TARGETARCH)/%.o: %.c
	@mkdir -p $(TARGETARCH)
	$(CC) -c $(CFLAGS) -DRWL_SOCKET -DRWL_WIFI -DRWL_DONGLE -o $@ $^
endif

endif # TARGETENV linux

endif # BUILD_TAG

.PHONY: all clean net_priv

endif # UNAME
