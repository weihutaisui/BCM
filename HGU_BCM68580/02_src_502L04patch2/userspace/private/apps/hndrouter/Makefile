# Toploevel makefile to build HND router applications
#
#
#
#

default : all

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

export ALLSRCBASE= $(BUILD_DIR)/bcmdrivers/broadcom/net/wl/bcm9$(BRCM_CHIP)
export HNDAPPS_DIR = $(ALLSRCBASE)/$(HNDAPPS_BASE)
export WLCFGDIR = $(ALLSRCBASE)/main/src/wl/config
export WLMAIN = main/src

export PLT = arm
export LINUX_VERSION = 4_1_0
export LINUXDIR = $(KERNEL_DIR)
SUBMAKE_SETTINGS = SRCBASE=$(SRCBASE) BASEDIR=$(BASEDIR)

USR_CFLAGS = -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)
USR_LDFLAGS = -L$(INSTALL_DIR)/lib -lnvram -lwlcsm -lbcm_flashutil -lbcm_boardctl -lbcm_crc -L$(TOOLCHAIN)/../lib

USR_CFLAGS += -DCHIP_$(BRCM_CHIP)

ifeq ($(strip $(BRCM_USER_SSP)),y)
USR_CFLAGS  += $(SSP_MIN_COMPILER_OPTS)
USR_LDFLAGS += $(SSP_LIBS)
endif
ifneq ($(strip $(BCA_CPEROUTER)),)
export USE_EXTERNAL_HTTPD=y
USR_CFLAGS += -DBCA_CPEROUTER  -DNO_NVRAM_GUI -DUSE_EXTERNAL_HTTPD -I$(BUILD_DIR)/userspace/private/libs/wlcsm/include
endif
export USR_CFLAGS
export USR_LDFLAGS

.PHONY: all oldconfig clean
all: version
	$(MAKE) -C $(HNDAPPS_DIR) all install

version: oldconfig
	$(MAKE) -C $(HNDAPPS_DIR) version

oldconfig:
	$(MAKE) -C $(HNDAPPS_DIR) oldconfig

ifneq ($(strip $(BCA_HNDROUTER)),)
clean:
	if [ -d $(HNDAPPS_DIR) ]; then \
	$(MAKE) -C $(HNDAPPS_DIR) clean; \
	fi
else
clean:
	@true
endif
