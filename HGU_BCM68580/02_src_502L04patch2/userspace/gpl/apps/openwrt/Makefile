#***********************************************************************
#
#  Copyright (c) 2014  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/

all: conditional_build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

unexport CC CPP CPPFLAGS LDFLAGS CFLAGS CXX AR AS LD NM STRIP SSTRIP OBJCOPY OBJDUMP RANLIB READELF IMAGES_DIR PROFILE
MAKEFLAGS=

.PHONY: conditional_build 

OPENWRT_INSTALL_DIR=$(INSTALL_DIR)/opt/openwrt
OPENWRT_INSTALL_DIR_ROOTFS=$(OPENWRT_INSTALL_DIR)/rootfs
OPENWRT_INSTALL_DIR_CONFIG=$(OPENWRT_INSTALL_DIR)/config
OPENWRT_ROOTFS=openwrt-rootfs.tar.gz

BUSGATE_SHARE_DIR = $(INSTALL_DIR)/share/busgate
BUSGATE_RW_SHARE_DIR = /local/busgate

ifneq ($(strip $(BUILD_MODSW_OPENWRTEE)),)
conditional_build: $(OPENWRT_ROOTFS) conf
	@echo "untar openwrt filesystem..."
	tar xfz $(OPENWRT_ROOTFS) -C $(OPENWRT_INSTALL_DIR_ROOTFS) 
else
conditional_build:
	@echo "skipping OpenWRT_EE (not configured)"
endif

# With a read-only FS, make sure to make all our host-side LXC required files
# and directories first.
clean:
	@echo "Cleaning openwrt filesystem..."
	rm -rf $(OPENWRT_INSTALL_DIR)
	rm -rf $(BUSGATE_SHARE_DIR)/openwrt

mkdirs:
	@mkdir -p $(OPENWRT_INSTALL_DIR)
	@mkdir -p $(OPENWRT_INSTALL_DIR_ROOTFS)
	@mkdir -p $(OPENWRT_INSTALL_DIR_CONFIG)
	@mkdir -p $(BUSGATE_SHARE_DIR)
	(cd $(BUSGATE_SHARE_DIR); rm -f openwrt; ln -s $(BUSGATE_RW_SHARE_DIR)/openwrt openwrt)

conf: mkdirs
	@cp -rf config/* $(OPENWRT_INSTALL_DIR_CONFIG)/
	@chmod +x $(OPENWRT_INSTALL_DIR_CONFIG)/pre-init-obox.sh
	@cp -rf scripts/* $(OPENWRT_INSTALL_DIR)
	@chmod 755 $(OPENWRT_INSTALL_DIR)/start_obox.sh

$(OPENWRT_ROOTFS):
	@echo "Preparing source code for OpenWRT..."
	@./getsource.sh
	@echo "Untar patches..."
	tar xvf openwrt-patches.tar.gz
	cd openwrt; git checkout -b master; git am --keep-cr ../patches/00*
	@echo "Building from openwrt source..."
	cd openwrt && env MAKEFLAGS= ./build.sh; 
	@cp openwrt/bin/bcm53xx/openwrt-bcm53xx-Generic-rootfs.tar.gz ./openwrt-rootfs.tar.gz

distclean: clean
