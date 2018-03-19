#
# Linux router libbcmcrypto Makefile
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
# $Id: Makefile 403251 2013-05-20 05:15:55Z $
#

include $(TOP)/.config

CFLAGS	+= -Wsign-compare -I. -I../httpd -I$(SRCBASE)/include ${WLAN_StdIncPathA} -I$(SRCBASE)/common/include -Wall -fPIC
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s
LDFLAGS += -L.

vpath %.c $(SRCBASE)/bcmcrypto
vpath %.o $(SRCBASE)/router/libbcmcrypto/prebuilt

OBJS := aes.o aeskeywrap.o rijndael-alg-fst.o dh.o bn.o sha1.o passhash.o prf.o md5.o hmac.o rc4.o random.o
# For customer backwords compatibility, for now we keep CONFIG_WSCCMD here and can be removed when
# all customers migrate to new SDK (no CONFIG_WSCCMD in configration files).
ifneq ($(CONFIG_WPS)$(CONFIG_WSCCMD)$(CONFIG_MFP),)
OBJS += sha256.o hmac_sha256.o
endif

# Find all ELF binaries (including extra libraries)
BINARIES := $(shell find $(TARGETDIR) -path $(TARGETDIR)/lib -prune -o -type f -print | file -f - | grep ELF | cut -d':' -f1)
MAP := .map
UNR := .unresolved
SYM := .symbols

all: libbcmcrypto.so

install: all
	install -d $(INSTALLDIR)/usr/lib
	install -m 755 libbcmcrypto.so $(INSTALLDIR)/usr/lib
	$(STRIP) $(INSTALLDIR)/usr/lib/libbcmcrypto.so

clean:
	rm -f *.o *.so *.a $(MAP) $(UNR) $(SYM)

libbcmcrypto.so: $(OBJS)
	$(LD) -shared -o $@ $^

# rule for removing unneeded symbols in the shared library
optimize: libbcmcrypto.so
	$(NM) -o --defined-only --no-sort libbcmcrypto.so | cut -d' ' -f3 > $(MAP)
ifeq ($(MKSYM),)
	$(NM) --dynamic -u --no-sort $(BINARIES) | sort -u > $(UNR)
	rm -rf $(SYM)
	for symbol in `cat $(UNR)` ; do \
	if grep -q "^$${symbol}$$" $(MAP) ; then echo "-u $${symbol}" >> $(SYM) ;  \
	fi ; done
else
	$(NM) --dynamic -u --no-sort $(BINARIES) | sort -u | $(MKSYM) $(MAP) > $(SYM)
endif
	# if no symbols are needed then delete the shared lib
	if ls $(SYM) ; then \
	$(AR) cr libbcmcrypto.a $(OBJS) ; \
	xargs -t $(LD) -shared -o libbcmcrypto.so libbcmcrypto.a < $(SYM) ; \
	install libbcmcrypto.so $(TARGETDIR)/usr/lib ; \
	$(STRIP) $(TARGETDIR)/usr/lib/libbcmcrypto.so ; \
	else \
	rm $(TARGETDIR)/usr/lib/libbcmcrypto.so ; fi
