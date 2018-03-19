# eapd makefile

BUILD_DIR=$(word 1, $(subst /bcmdrivers, /bcmdrivers,$(shell pwd)))
include $(BUILD_DIR)/make.common

CFLAGS += -I.
CFLAGS += -I../../include
CFLAGS += -I../../common/include
CFLAGS += -I../../shared
CFLAGS += -I../../router/shared
CFLAGS += -I../../router/nas
CFLAGS += -I../../shared/bcmwifi/include
CFLAGS += -DBCMWPA2 
CFLAGS += -DEAPD_WKSP_AUTO_CONFIG
CFLAGS += -DDSLCPE -DDSLCPE_ENDIAN
CFLAGS += -DBCM_MEVENT
CFLAGS += -Wall

LDFLAGS = -Os 
LDFLAGS += -L$(EXTRALIBDIR)
LDFLAGS += -lc
LDFLAGS += -lgcc_s
LDFLAGS += -L$(INSTALL_DIR)/lib
LDFLAGS += -lnvram
ifeq ($(DSLCPE_WLCSM_EXT),1)
LDFLAGS += -lwlcsm
endif
LDFLAGS += -lwlbcmcrypto
LDFLAGS += -lwlbcmshared

-include files

OBJS = $(foreach x, $(FILES), $(shell find . -name "$x" -exec echo $(x:.c=.o) ";"))

ifneq ($(strip $(OBJS)),)
ifeq ($(BRCM_WAPI),y)
OBJS += wai_eap.o
endif
endif

ifneq ($(strip $(OBJS)),)
ifeq ($(CONFIG_BCMDCS),y)
OBJS += dcs_eap.o
CFLAGS += -DBCM_DCS
endif
endif

ifneq ($(strip $(OBJS)),)
ifeq ($(CONFIG_BCMBSD),y)
OBJS += bsd_eap.o
CFLAGS += -DBCM_BSD
endif
endif

ifneq ($(strip $(OBJS)),)
ifeq ($(CONFIG_BCMSSD),y)
OBJS += ssd_eap.o
CFLAGS += -DBCM_SSD
endif
endif

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

eapd: $(OBJS)
ifneq ($(strip $(OBJS)),)
ifeq ($(BRCM_WAPI),y)
	$(CC) -o $@_wapi $^ $(LDFLAGS)
	cp -f $@_wapi $@_wapi-$(EXT_CPU_ARCH_NAME)
else
	$(CC) -o $@ $^ $(LDFLAGS)
	cp -f $@ $@-$(EXT_CPU_ARCH_NAME)
endif
else
ifeq ($(BRCM_WAPI),y)
	cp -f $@_wapi-$(EXT_CPU_ARCH_NAME) $@_wapi
else
	cp -f $@-$(EXT_CPU_ARCH_NAME) $@
endif
endif

install: eapd
ifeq ($(BRCM_WAPI),y)
	install -m 755 $<_wapi $(INSTALL_DIR)/bin/$<
else
	install -m 755 $< $(INSTALL_DIR)/bin/$<
endif

clean:
	rm -f $(OBJS) *.d
	rm -f eapd_wapi eapd

dynamic static: eapd

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)
