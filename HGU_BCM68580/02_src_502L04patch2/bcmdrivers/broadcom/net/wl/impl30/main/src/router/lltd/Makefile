# lltd makefile

BUILD_DIR=$(word 1, $(subst /bcmdrivers, /bcmdrivers,$(shell pwd)))
include $(BUILD_DIR)/make.common

CFLAGS += -I../../include
CFLAGS += -I../../common/include
CFLAGS += -I../../shared
CFLAGS += -I../../router/shared
CFLAGS += -I../../shared/bcmwifi/include
CFLAGS += -DDSLCPE -DDSLCPE_ENDIAN
CFLAGS += -DBCMWPA2
CFLAGS += -s
CFLAGS += -Os
CFLAGS += -Wall
CFLAGS += -fomit-frame-pointer

LDFLAGS = -Os 
LDFLAGS += -L$(INSTALL_DIR)/lib
LDFLAGS += -lnvram
ifeq ($(DSLCPE_WLCSM_EXT),1)
LDFLAGS += -lwlcsm
endif
LDFLAGS += -lwlbcmcrypto 
LDFLAGS += -lwlbcmshared

-include files

OBJS = $(foreach x, $(FILES), $(shell find . -name "$x" -exec echo $(x:.c=.o) ";"))

vpath %.c ./src

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

lld2d: $(OBJS)
ifneq ($(strip $(OBJS)),)
	$(CC) -o $@ $^ $(LDFLAGS)
	cp -f $@ $@-$(EXT_CPU_ARCH_NAME)
else
	cp -f $@-$(EXT_CPU_ARCH_NAME) $@
endif

install: lld2d
	install -m 755 $< $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$<
	install -m 444 ./src/lld2d_dslcpe.conf $(INSTALL_DIR)/etc/lld2d.conf
	install -D -m 444 ./src/bcm963xx.large.ico $(INSTALL_DIR)/etc/wlan
	install -D -m 444 ./src/bcm963xx.small.ico $(INSTALL_DIR)/etc/wlan

clean:
	rm -f $(OBJS) *.d
	rm -f lld2d

dynamic static: lld2d

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)
