#
# Makefile for wlctl utility.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /bcmdriver, /bcmdriver,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common
PROGRAM_NAME:= wlctl

RWL=1
ifeq ($(RWL),1)
SERVER_SOCKET:= wl_server_socket
endif

CFLAGS +=-s -Os -fomit-frame-pointer -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMSHARED_PRIV_PATH)/$(BRCM_BOARD)
CFLAGS += -I$(TOOLCHAIN)/include  -lgcc_s

CFLAGS += -DLINUX
ifeq ($(RWL),1)
CFLAGS += -DRWL_SOCKET  #-DRWL_DONGLE -DRWL_WIFI
endif
CFLAGS += -fno-strict-aliasing


LDFLAGS=-Os -Wl,-allow-shlib-undefined
CFLAGS_EXTRA =

ifeq ($(ARCH), arm)
EXTRA_LDFLAGS += -z muldefs
endif

# DSLCPE
CFLAGS += $(SSP_TYP_COMPILER_OPTS) $(SSP_TYP_LIBS) $(SSP_LIB_INC)
EXTRA_LDFLAGS += $(SSP_TYP_LIBS)  $(SSP_LIB_INC)
# end DSLCPE

WIRELESS_DRIVER_PATH=$(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/dhd/src
INC_WIRELESS_DRIVER_PATH=$(WIRELESS_DRIVER_PATH)/include
SHARED_WIRELESS_DRIVER_PATH=$(WIRELESS_DRIVER_PATH)/shared
SHARED_WIRELESS_ROUTER_PATH=$(WIRELESS_DRIVER_PATH)/../../main/src/router/shared
BCMWIFI_WIRELESS_ROUTER_PATH=$(WIRELESS_DRIVER_PATH)/shared/bcmwifi/src
WLPPR_WIRELESS_DRIVER_PATH=$(WIRELESS_DRIVER_PATH)/wl/ppr/src
COMM_WIRELESS_DRIVER_PATH=$(WIRELESS_DRIVER_PATH)/../components/shared

CFLAGS += -Wall -I$(INC_WIRELESS_DRIVER_PATH) -I$(COMM_WIRELESS_DRIVER_PATH) -I$(COMM_WIRELESS_DRIVER_PATH)/proto -DDSLCPE -DD11AC_IOTYPES -I$(WIRELESS_DRIVER_PATH)/shared/bcmwifi/include -I$(WIRELESS_DRIVER_PATH)/wl/ppr/include -DPPR_API -DDSLCPE_ENDIAN
CFLAGS += -I$(SHARED_WIRELESS_ROUTER_PATH)


# Append DSLCPE_WLAN_VERSION to EPI_VERSION_STR
include $(WIRELESS_DRIVER_PATH)/dslcpe_wlan_minor_version
DSLCPE_WLAN_VERSION=cpe$(BRCM_VERSION).$(BRCM_VERSION)$(BRCM_RELEASE).$(DSLCPE_WLAN_MINOR_VERSION)
CFLAGS += -DDSLCPE_WLAN_VERSION=\"$(DSLCPE_WLAN_VERSION)\"

include $(WIRELESS_DRIVER_PATH)/../../main/src/epivers
CFLAGS += -DDSLCPE_EPI_VERSION_STR=\"$(EPI_VERSION_STR)\"

ifeq ($(strip $(BUILD_WLCTL)), static)
CFLAGS += -DBUILD_STATIC 
endif
CFLAGS += -DBCMWPA2

ifeq ($(DSLCPE_DT_BUILD),1)
CFLAGS	+= -DRADIUS_RESTRICTION
endif

ifeq ($(strip $(BUILD_WLCTL_SHLIB)),1) 
CFLAGS += -DDSLCPE_SHLIB
CFLAGS_EXTRA = -fpic
endif

CFLAGS += -DDSLCPE -DDSLCPE_SCBLIST

#wlctl cmd verbose to show cmd in console
#CFLAGS += -DDSLCPE_VERBOSE

### Packet Chaining ####
CFLAGS += -DPKTC

CFLAGS += -DWLTEST

WLCTLOBJS = wlu.o wlu_cmd.o wlu_iov.o wlu_client_shared.o wlu_pipe_linux.o wlu_pipe.o wlu_common.o wlu_rates_matrix.o bcmxtlv.o bcmbloom.o
ifeq ($(RWL),1)
SERVER_OBJS = wlu_server_linux.o wlu_pipe_linux.o wlu_pipe.o wlu_server_shared.o shellproc_linux.o wlu_common.o
endif
SHAREDOBJS = bcmutils.o bcmwifi_channels.o miniopt.o bcm_app_utils.o
WLPPROBJS = wlc_ppr.o
ROUTERSHAREDOBJS = 

WLCTLOBJS += wluc_phy.o
WLCTLOBJS += wluc_wnm.o
WLCTLOBJS += wluc_cac.o
WLCTLOBJS += wluc_relmcast.o
WLCTLOBJS += wluc_rrm.o
WLCTLOBJS += wluc_wowl.o
WLCTLOBJS += wluc_pkt_filter.o
WLCTLOBJS += wluc_mfp.o
WLCTLOBJS += wluc_ota_test.o
WLCTLOBJS += wluc_bssload.o
WLCTLOBJS += wluc_stf.o
WLCTLOBJS += wluc_offloads.o
WLCTLOBJS += wluc_tpc.o
WLCTLOBJS += wluc_toe.o
WLCTLOBJS += wluc_arpoe.o
WLCTLOBJS += wluc_keep_alive.o
WLCTLOBJS += wluc_ap.o
WLCTLOBJS += wluc_ampdu.o
WLCTLOBJS += wluc_ampdu_cmn.o
WLCTLOBJS += wluc_bmac.o
WLCTLOBJS += wluc_ht.o
WLCTLOBJS += wluc_wds.o
WLCTLOBJS += wluc_keymgmt.o
WLCTLOBJS += wluc_scan.o
WLCTLOBJS += wluc_obss.o
WLCTLOBJS += wluc_prot_obss.o
WLCTLOBJS += wluc_lq.o
WLCTLOBJS += wluc_seq_cmds.o
WLCTLOBJS += wluc_btcx.o
WLCTLOBJS += wluc_led.o
WLCTLOBJS += wluc_interfere.o
WLCTLOBJS += wluc_ltecx.o
WLCTLOBJS += wlu_endian_handler.o


ifneq ($(findstring WL_BTCDYN, $(CFLAGS)),)
WLCTLOBJS += wluc_btcdyn.o
endif

ifneq ($(findstring WL_NAN, $(CFLAGS)),)
WLCTLOBJS += wluc_nan.o
endif

ifneq ($(findstring WLEXTLOG, $(CFLAGS)),)
WLCTLOBJS += wluc_extlog.o
endif

ifneq ($(findstring WLBTAMP, $(CFLAGS)),)
WLCTLOBJS += wluc_bta.o
endif

ifneq ($(findstring BCMSDIO, $(CFLAGS)),)
WLCTLOBJS += wluc_sdio.o
endif

ifneq ($(findstring WLNDOE, $(CFLAGS)),)
WLCTLOBJS += wluc_ndoe.o
endif

ifneq ($(filter -DWLP2PO, $(CFLAGS)),)
WLCTLOBJS += wluc_p2po.o
endif

ifneq ($(findstring WLANQPO, $(CFLAGS)),)
WLCTLOBJS += wluc_anqpo.o
endif

ifneq ($(findstring WLPFN, $(CFLAGS)),)
WLCTLOBJS += wluc_pfn.o
endif

ifneq ($(findstring BT_WIFI_HANDOVER, $(CFLAGS)),)
WLCTLOBJS += wluc_tbow.o
endif

ifneq ($(filter -DWLP2P, $(CFLAGS)),)
WLCTLOBJS += wluc_p2p.o
endif

ifneq ($(findstring WLTDLS, $(CFLAGS)),)
WLCTLOBJS += wluc_tdls.o
endif

ifneq ($(findstring TRAFFIC_MGMT, $(CFLAGS)),)
WLCTLOBJS += wluc_traffic_mgmt.o
endif

ifneq ($(findstring WL_PROXDETECT, $(CFLAGS)),)
WLCTLOBJS += wluc_proxd.o
endif

ifeq ($(strip $(BUILD_WLCTL_SHLIB)),1) 
MAIN = wlu_linux_dslcpe
WLCTLOBJS += wlu_linux.o
else
MAIN = wlu_linux
endif
ifeq ($(RWL),1)
MAIN_SERVER = wlu_server_linux
MAINSERVEROBJ = $(MAIN_SERVER).o
endif

MAINOBJ = $(MAIN).o
OBJS = $(WLCTLOBJS) $(SHAREDOBJS) $(ROUTERSHAREDOBJS) $(WLPPROBJS)
ifeq ($(RWL),1)
SOCKET_OBJS = $(SERVER_OBJS)
endif

ifneq ($(wildcard $(SHARED_WIRELESS_DRIVER_PATH) $(BCMWIFI_WIRELESS_DRIVER_PATH)),)
vpath %.c $(SHARED_WIRELESS_DRIVER_PATH) $(CRYPTO_WIRELESS_DRIVER_PATH) $(SHARED_WIRELESS_ROUTER_PATH) $(BCMWIFI_WIRELESS_ROUTER_PATH) $(WLPPR_WIRELESS_DRIVER_PATH)
else
OBJS := $(foreach obj,$(OBJS),$(SHARED_WIRELESS_DRIVER_PATH)/$(OBJS)),$(CRYPTO_WIRELESS_DRIVER_PATH)/$(OBJS),$(SHARED_WIRELESS_ROUTER_PATH)/$(OBJS),$(BCMWIFI_WIRELESS_ROUTER_PATH)/$(OBJS),$(WLPPR_WIRELESS_DRIVER_PATH)/$(OBJS)
endif

ifeq ($(RWL),1)
all: lib$(PROGRAM_NAME) $(PROGRAM_NAME)	lib$(SERVER_SOCKET) $(SERVER_SOCKET)
else
all: lib$(PROGRAM_NAME) $(PROGRAM_NAME)
endif

ifeq ($(RWL),1)
install: $(PROGRAM_NAME) $(SERVER_SOCKET)
else
install: $(PROGRAM_NAME)
endif
	install -m 755 $(PROGRAM_NAME) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(PROGRAM_NAME)
	if [ -e lib$(PROGRAM_NAME).so ]; then cp -av lib$(PROGRAM_NAME).so $(INSTALL_DIR)/lib; fi
	mkdir -p $(INSTALL_DIR)/sbin
	ln -sf $(PROGRAM_NAME) $(INSTALL_DIR)/bin/wl
ifeq ($(RWL),1)
	install -m 755 $(SERVER_SOCKET) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(SERVER_SOCKET)
	if [ -e lib$(SERVER_SOCKET).so ]; then cp -av lib$(SERVER_SOCKET).so $(INSTALL_DIR)/lib; fi
	mkdir -p $(INSTALL_DIR)/sbin
	ln -sf $(SERVER_SOCKET) $(INSTALL_DIR)/bin/wl_server
endif

dynamic: all install

.c.o:
	$(CC) -DPROTOTYPES $(CFLAGS) $(CFLAGS_EXTRA) -c $<

$(MAINOBJ): $(MAIN).c
	$(CC) $(CFLAGS) -o $@ -c $<

ifeq ($(RWL),1)
$(MAINSERVEROBJ): $(MAIN_SERVER).c
	$(CC) $(CFLAGS) -o $@ -c $<
endif

ifeq ($(strip $(BUILD_WLCTL_SHLIB)),1) 

lib$(PROGRAM_NAME): $(OBJS) 
	$(LD) -shared -soname lib$(PROGRAM_NAME).so -o lib$(PROGRAM_NAME).so $(OBJS) -L$(LIBDIR) -lc $(EXTRA_LDFLAGS)

$(PROGRAM_NAME): lib$(PROGRAM_NAME) $(MAINOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -L. -o $(PROGRAM_NAME) $(MAINOBJ) -l$(PROGRAM_NAME)

ifeq ($(RWL),1)
lib$(SERVER_SOCKET): $(SOCKET_OBJS)
	$(LD) -shared -soname lib$(SERVER_SOCKET).so -o lib$(SERVER_SOCKET).so $(SOCKET_OBJS) -L$(LIBDIR) -lc $(EXTRA_LDFLAGS)

$(SERVER_SOCKET): lib$(SERVER_SOCKET) $(MAINSERVEROBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -L. -o $(SERVER_SOCKET) $(MAINSERVEROBJ) -l$(SERVER_SOCKET)
endif

else
$(PROGRAM_NAME): $(MAINOBJ) $(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PROGRAM_NAME) $(MAINOBJ) $(OBJS)

lib$(PROGRAM_NAME):

ifeq ($(RWL),1)
$(SERVER_SOCKET): $(MAINSERVEROBJ) $(SOCKET_OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(SERVER_SOCKET) $(MAINSERVEROBJ) $(SOCKET_OBJS)
lib$(SERVER_SOCKET):
endif
endif

clean:
	-rm -f $(PROGRAM_NAME) *.o  $(PROGRAM_NAME).a lib$(PROGRAM_NAME).so $(MAINOBJ)
ifeq ($(RWL),1)
	-rm -f $(SERVER_SOCKET) *.o  $(SERVER_SOCKET).a lib$(SERVER_SOCKET).so $(MAINSERVEROBJ)
endif
