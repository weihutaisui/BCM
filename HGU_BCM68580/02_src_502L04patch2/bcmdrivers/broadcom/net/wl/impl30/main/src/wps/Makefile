# wps makefile

BUILD_DIR=$(word 1, $(subst /bcmdrivers, /bcmdrivers,$(shell pwd)))
include $(BUILD_DIR)/make.common

CFLAGS += -g -I.
CFLAGS += -I./common/include
CFLAGS += -I./brcm_apps/apps
CFLAGS += -I./brcm_apps/arch/bcm947xx
CFLAGS += -I./brcm_apps/include
CFLAGS += -I../include
CFLAGS += -I../common/include
CFLAGS += -I../include/bcmcrypto
CFLAGS += -I../router/libupnp
CFLAGS += -I../router/libupnp/include
CFLAGS += -I../router/eapd
CFLAGS += -I../router/nas
CFLAGS += -I../router/shared
CFLAGS += -I../shared
CFLAGS += -I../shared/bcmwifi/include
CFLAGS += -I../wps/brcm_apps/upnp/WFADevice
#CFLAGS += -I../wps/brcm_apps/nfc
CFLAGS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)
CFLAGS += -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)
CFLAGS += -I$(BUILD_DIR)/userspace/private/include
CFLAGS += -I$(BUILD_DIR)/userspace/public/include
CFLAGS += -I$(BUILD_DIR)/userspace/public/include/linux
CFLAGS += -DBCMUPNP
CFLAGS += -DBCMWPA2
CFLAGS += -DBCMWPSAP
CFLAGS += -DDSLCPE -DDSLCPE_ENDIAN
CFLAGS += -DDSLCPE_1905
CFLAGS += -D_REENTRANT 
CFLAGS += -D__linux__
CFLAGS += -DUPNP_WFA_DEVICE
CFLAGS += -DWPS_UPNP_DEVICE
CFLAGS += -DWPS_AP_M2D
CFLAGS += -DWPS_ADDCLIENT_WWTP
CFLAGS += -DWPS_ROUTER
#CFLAGS += -D_TUDEBUGTRACE
CFLAGS += -DDSLCPE_NG

ifneq ($(strip $(DSLCPE_WPS_SEC_CLONE)),)
CFLAGS += -DBCMWPSAPSTA
CFLAGS += -DDSLCPE_WPS_SEC_CLONE
endif

#LDFLAGS = -Os
LDFLAGS += -L$(EXTRALIBDIR)
LDFLAGS += -lc
LDFLAGS += -lgcc_s
LDFLAGS += -L$(INSTALL_DIR)/lib
LDFLAGS += -lnvram
ifeq ($(DSLCPE_WLCSM_EXT),1)
LDFLAGS += -lwlcsm
CFLAGS += -DDSLCPE_WLCSM_EXT
CFLAGS += -I$(BUILD_DIR)/userspace/private/apps/wlan/wlcsm/include
endif
LDFLAGS += -lwlbcmcrypto
LDFLAGS += -lwlbcmshared
LDFLAGS += -lwlupnp
#LDFLAGS += -lwlctl
LDFALSG += -ldl
LDFLAGS += -L$(INSTALL_DIR)/lib
ifneq ($(strip $(BUILD_BRCM_CMS)),)
LDFLAGS += -lcms_msg
LDFLAGS += $(CMS_COMMON_LIBS)
else
CFLAGS += -DDSLCPE_NOCMS
endif
LDFLAGS += -L.
LDFLAGS += -lwps
ifeq ($(ARCH), arm)
LDFLAGS += -z muldefs
endif

# DSLCPE
CFLAGS += $(SSP_MIN_COMPILER_OPTS)
LDFLAGS += $(SSP_MIN_LIBS)  $(SSP_LIB_INC)
# end DSLCPE

-include files

LIBWPS_OBJS         = $(foreach x, $(LIBWPS_FILES), $(shell find . -name "$x" -exec echo $(x:.c=.o) ";"))
WPS_MONITOR_OBJS    = $(foreach x, $(WPS_MONITOR_FILES), $(shell find . -name "$x" -exec echo $(x:.c=.o) ";"))
WPS_AP_OBJS         = $(foreach x, $(WPS_AP_FILES), $(shell find . -name "$x" -exec echo $(x:.c=.o) ";"))
WPS_ENR_OBJS        = $(foreach x, $(WPS_ENR_FILES), $(shell find . -name "$x" -exec echo $(x:.c=.o) ";"))
WPS_WFA_OBJS        = $(foreach x, $(WPS_WFA_FILES), $(shell find . -name "$x" -exec echo $(x:.c=.o) ";"))
#WPS_NFC_OBJS        = $(foreach x, $(WPS_NFC_FILES), $(shell find . -name "$x" -exec echo $(x:.c=.o) ";"))

vpath %.c ./brcm_apps/apps
vpath %.c ./brcm_apps/arch/bcm947xx
vpath %.c ./brcm_apps/linux
vpath %.c ./brcm_apps/upnp/WFADevice
#vpath %.c ./brcm_apps/nfc
vpath %.c ./linux/ap
vpath %.c ./common/ap
vpath %.c ./common/enrollee
vpath %.c ./common/registrar
vpath %.c ./common/shared
vpath %.c ./common/sta

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

mkversion:
	sh ./mkversion.sh ./common/include/version.h.in ./common/include/wps_version.h "$(PROD_TAG)"

libwps.so: $(LIBWPS_OBJS)
ifneq ($(strip $(LIBWPS_OBJS)),)
	$(CC) -shared -O2 -o $@ $^
	cp -f $@ libwps.so-$(EXT_CPU_ARCH_NAME)
else
	cp -f libwps.so-$(EXT_CPU_ARCH_NAME) $@
endif

wps_monitor.lib: $(WPS_MONITOR_OBJS)
ifneq ($(strip $(WPS_MONITOR_OBJS)),)
	$(AR) -rcs $@ $^
	cp -f $@ wps_monitor.lib-$(EXT_CPU_ARCH_NAME)
else
	cp -f wps_monitor.lib-$(EXT_CPU_ARCH_NAME) $@
endif

wps_ap.lib : $(WPS_AP_OBJS)
ifneq ($(strip $(WPS_AP_OBJS)),)
	$(AR) -rcs $@ $^
	cp -f $@ wps_ap.lib-$(EXT_CPU_ARCH_NAME)
else
	cp -f wps_ap.lib-$(EXT_CPU_ARCH_NAME) $@
endif

wps_enr.lib : $(WPS_ENR_OBJS)
ifneq ($(strip $(WPS_ENR_OBJS)),)
	$(AR) -rcs $@ $^
	cp -f $@ wps_enr.lib-$(EXT_CPU_ARCH_NAME)
else
	cp -f wps_enr.lib-$(EXT_CPU_ARCH_NAME) $@
endif

wps_wfa.lib : $(WPS_WFA_OBJS)
ifneq ($(strip $(WPS_WFA_OBJS)),)
	$(AR) -rcs $@ $^
	cp -f $@ wps_wfa.lib-$(EXT_CPU_ARCH_NAME)
else
	cp -f wps_wfa.lib-$(EXT_CPU_ARCH_NAME) $@
endif


#wps_nfc.lib : $(WPS_NFC_OBJS)
#ifneq ($(strip $(WPS_NFC_OBJS)),)
#	$(AR) -rcs $@ $^
#	cp -f $@ wps_nfc-$(EXT_CPU_ARCH_NAME).lib
#else
#	cp -f wps_nfc-$(EXT_CPU_ARCH_NAME).lib $@
#endif

#wps_nfc.lib add to next line
wps_monitor: wps_gpio_dslcpe.o wps_monitor.lib wps_ap.lib wps_enr.lib xml_WFADevice.o wps_wfa.lib libwps.so
	$(CC) -o $@ $^ $(LDFLAGS)

install: dynamic
	install -m 755 libwps.so $(INSTALL_DIR)/lib
	install -m 755 wps_monitor $(INSTALL_DIR)/bin

clean:
	rm -f wps_monitor
	rm -f wps_gpio_dslcpe.o
	rm -f $(LIBWPS_OBJS)
	rm -f $(WPS_MONITOR_OBJS)
	rm -f $(WPS_AP_OBJS)
	rm -f $(WPS_ENR_OBJS)
	rm -f xml_WFADevice.o
	rm -f $(WPS_WFA_OBJS)
#	rm -f $(WPS_NFC_OBJS)
	rm -f *.o *.d
	rm -f libwps.so
	rm -f wps_monitor.lib
	rm -f wps_ap.lib
	rm -f wps_enr.lib
	rm -f wps_wfa.lib
ifneq ($(strip $(RELEASE_BUILD)), 1)
	if [ -e "files" ]; then \
		rm -f libwps.so-*; \
		rm -f wps_monitor.lib-*; \
		rm -f wps_ap.lib-*; \
		rm -f wps_enr.lib-*; \
		rm -f wps_wfa.lib-*; \
	fi
endif
#	rm -f wps_nfc.lib

dynamic: wps_monitor
static:

OBJS = $(LIBWPS_OBJS)
OBJS += $(WPS_MONITOR_OBJS)
OBJS += $(WPS_AP_OBJS)
OBJS += $(WPS_ENR_OBJS)
OBJS += $(WPS_WFA_OBJS)
#OBJS += $(WPS_NFC_OBJS)
#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)
