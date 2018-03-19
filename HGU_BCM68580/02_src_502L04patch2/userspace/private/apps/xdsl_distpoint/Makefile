#**************************************************************************
#                Copyright C 1998-2009 Broadcom Corporation
#
#This material is the confidential trade secret and proprietary information
#of Broadcom Corporation.  It may not be reproduced, used, sold or
#transferred to any third party without the prior written consent of
#Broadcom Corporation.   All rights reserved.
#**************************************************************************
#  FileName        : Makefile
#  Purpose         : Top level makefile for DSL SDK integration
#  Limitations     : None
#  Creation Date   : 20-Jan-2009
#  Current Version : 1.1
#  Last Modified   : 2011/12/21 11:19:26
#  History         : Compiled by CVS
#**************************************************************************

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(word 1, $(subst /userspace, /userspace,$(CURR_DIR)))
default: conditional_build
include $(BUILD_DIR)/make.common

BUILD_BRCM_XDSL_DISTPOINT:=$(subst ",,$(BUILD_BRCM_XDSL_DISTPOINT)) # remove " to make it a list
ifneq ($(strip $(BUILD_BRCM_XDSL_DISTPOINT)),)
conditional_build: all
else
conditional_build:
	@echo “xdsl_distpoint not selected… skipping”
endif

SRC_DIR := $(CURR_DIR)/src
BIN_DIR := $(CURR_DIR)/Binaries

TARGETS = telnetd4cli
TARGETS += $(if $(filter fcope% 965200dpf2% fttdp2,$(BUILD_BRCM_XDSL_DISTPOINT)),max9611,)

# BCM965200F boards
ifneq (,$(filter fcope%,$(BUILD_BRCM_XDSL_DISTPOINT)))
TARGETS           += $(if $(filter fcope_co  fcope,$(BUILD_BRCM_XDSL_DISTPOINT)),xdslctl_fcope xdslctl_fcope_vdsl,)
TARGETS           += $(if $(filter fcope_cpe fcope,$(BUILD_BRCM_XDSL_DISTPOINT)),xdslctl_fcope_cpe,)
BOOT_FCOPE_CFG     = $(if $(BRCM_XDSL_DISTPOINT_USE_G9991),SGMII_1G_RGMII_FCOPE_g9991_cfg,SGMII_1G_RGMII_FCOPE_cfg)
BOOTER_SOURCE     += $(wildcard $(BIN_DIR)/hmibooter_$(BOOT_FCOPE_CFG)_*v36kco.bin)
BOOTER_OLD_SOURCE += $(wildcard /projects/hardware/BCM965200F/booters/hmibooter_$(BOOT_FCOPE_CFG)_6_4_0_v36kco.bin)
FW_EXT            += $(if $(filter fcope_co  fcope,$(BUILD_BRCM_XDSL_DISTPOINT)),f6kco v24kco)
FW_EXT            += $(if $(filter fcope_cpe fcope,$(BUILD_BRCM_XDSL_DISTPOINT)),f4kcpe f4ccpe v12kcpe)
endif

# BCM965200DPF2 boards
ifneq (,$(filter 965200dpf2%,$(BUILD_BRCM_XDSL_DISTPOINT)))
TARGETS           += $(if $(filter 965200dpf2_co  965200dpf2,$(BUILD_BRCM_XDSL_DISTPOINT)),xdslctl_dpf2 xdslctl_dpf2_vdsl,)
TARGETS           += $(if $(filter 965200dpf2_cpe 965200dpf2,$(BUILD_BRCM_XDSL_DISTPOINT)),xdslctl_dpf2_cpe,)
BOOT_DPF2_CFG      = $(if $(BRCM_XDSL_DISTPOINT_USE_G9991),SGMII_1G_RGMII_DPF2_g9991_cfg,SGMII_1G_RGMII_DPF2_cfg)
BOOTER_SOURCE     += $(wildcard $(BIN_DIR)/hmibooter_$(BOOT_DPF2_CFG)_*v36kco.bin)
BOOTER_OLD_SOURCE += $(wildcard /projects/hardware/BCM965200DPF2/booters/hmibooter_$(BOOT_DPF2_CFG)_6_4_0_v36kco.bin)
FW_EXT            += $(if $(filter 965200dpf2_co  965200dpf2,$(BUILD_BRCM_XDSL_DISTPOINT)),f6kco v24kco)
FW_EXT            += $(if $(filter 965200dpf2_cpe 965200dpf2,$(BUILD_BRCM_XDSL_DISTPOINT)),f4ccpe v12kcpe)
endif 

# BCM965200DPF boards
ifneq (,$(filter 965200dpf,$(BUILD_BRCM_XDSL_DISTPOINT)))
TARGETS           += xdslctl_dpf xdslctl_dpf_evce 965200dpf_traffic
BOOT_DPF_CFG1      = SGMII_1G_x4_DPF_INBAND_SGMII2_cfg
BOOT_DPF_CFG2      = SGMII_1G_g9991_x3_RGMII_DPF_SGMII2_cfg
BOOT_DPF_CFG       = $(if $(BRCM_XDSL_DISTPOINT_USE_G9991),$(BOOT_DPF_CFG2),$(BOOT_DPF_CFG1))
BOOTER_SOURCE     += $(wildcard $(BIN_DIR)/hmibooter_$(BOOT_DPF_CFG)_*v36kco.bin)
BOOTER_OLD_SOURCE += $(wildcard /projects/hardware/BCM965200DPF/booters/hmibooter_$(BOOT_DPF_CFG)_6_4_0_v36kco.bin)
FW_EXT            += f6kco
# add inband booter as alternate source for external vce case (always without g999.1)
BOOTER_ALT_SOURCE     = $(wildcard $(BIN_DIR)/hmibooter_$(BOOT_DPF_CFG1)_*v36kco.bin)
BOOTER_ALT_OLD_SOURCE = $(wildcard /projects/hardware/BCM965200DPF/booters/hmibooter_$(BOOT_DPF_CFG1)_6_4_0_v36kco.bin)
BOOTER_ALT_DEST       = inband_v36kco.bin
BOOTER_ALT_OLD_DEST   = inband_old.bin
endif

# BCM965400X boards
ifneq (,$(filter 965400x%,$(BUILD_BRCM_XDSL_DISTPOINT)))
TARGETS       += $(if $(filter 965400x_co  965400x,$(BUILD_BRCM_XDSL_DISTPOINT)),xdslctl_400x xdslctl_400x_vdsl,)
TARGETS       += $(if $(filter 965400x_cpe 965400x,$(BUILD_BRCM_XDSL_DISTPOINT)),xdslctl_400x_cpe,)
BOOTER_SOURCE += $(wildcard $(BIN_DIR)/hmibooter_*_400X_*_f8?co.bin)
FW_EXT        += $(if $(filter 965400x_co  965400x,$(BUILD_BRCM_XDSL_DISTPOINT)),f4yco f8yco f4zco f8zco)
FW_EXT        += $(if $(filter 965400x_cpe 965400x,$(BUILD_BRCM_XDSL_DISTPOINT)),f4ycpe f8ycpe f4zcpe f8zcpe)
endif

# FTTDP1/2 boards
ifneq (,$(filter fttdp%,$(BUILD_BRCM_XDSL_DISTPOINT)))
TARGETS       += $(if $(filter fttdp1,$(BUILD_BRCM_XDSL_DISTPOINT)), xdslctl_fttdp1)
TARGETS       += $(if $(filter fttdp2,$(BUILD_BRCM_XDSL_DISTPOINT)), xdslctl_fttdp2)
RGMII          = $(if $(BRCM_XDSL_DISTPOINT_USE_RGMII1),rgmii1,rgmii0)
G9991          = $(if $(BRCM_XDSL_DISTPOINT_USE_G9991),g9991_boelen,ethernet)
BOOTER_SOURCE += $(wildcard $(BIN_DIR)/hmibooter_$(RGMII)_$(G9991)_inband*v16sco.sprom)
FW_EXT        += v8sco v16sco
endif

ifneq (,$(filter 955045dpu%,$(BUILD_BRCM_XDSL_DISTPOINT)))
TARGETS       += xdslctl_955045dpu
BOOTER_SOURCE += $(wildcard $(BIN_DIR)/hmibooter_7_*_f8zco.bin)
FW_EXT        += f4zco
endif

BLD_DIR := $(addprefix $(CURR_DIR)/,$(TARGETS))

# target directory for xdsl related files
XDSL_DIR:=/etc/xdsl
XDSL_INSTALL_DIR:=$(INSTALL_DIR)$(XDSL_DIR)
XDSL_COMPILE_DIR:=example2
XDSL_TARGETS=$(filter xdsl%, $(TARGETS))

BOOTER_OLD_DEST = old.bin
INBAND_ADDRESS = $(if $(BRCM_XDSL_DISTPOINT_INBAND_ADDRESS),$(BRCM_XDSL_DISTPOINT_INBAND_ADDRESS),4)

# firmware binaries for installation
FIRMWARE_BIN = $(wildcard $(foreach ext,$(sort $(FW_EXT)),$(BIN_DIR)/VE_*$(ext).bin))

xdslctl_dpf_evce: BUILD_BRCM_XDSL_DISTPOINT_VECTORING=vce_fast_internal
export BUILD_BRCM_XDSL_DISTPOINT
export BUILD_BRCM_XDSL_DISTPOINT_VECTORING
export BRCM_XDSL_DISTPOINT_USE_G9991

# 1 booter source, 2 (optional) booter dest extension
define CMD_INSTALL_AND_SOFTLINK_BOOTER
  if [ -n "$(1)" ]; then \
    boot_src=$(1); \
    install $(1) $(XDSL_INSTALL_DIR);\
    [ ! -n "$${boot_src#*.sprom}" ] || echo ok; \
    if [ ! -n "$${boot_src#*.sprom}" ]; then \
      boot=booter.sprom; \
    elif [ -n "$(2)" ]; then \
      boot=`echo $(notdir $(1)) | sed -e 's/hmibooter_\(.\+\)_cfg_[0-9]\+_[0-9]\+_[0-9]\+_\([a-z0-9]\+\.bin\)/booter_\1_$(2)/'`; \
    else \
      boot=`echo $(notdir $(1)) | sed -e 's/hmibooter_\(.\+\)_cfg_[0-9]\+_[0-9]\+_[0-9]\+_\([a-z0-9]\+\.bin\)/booter_\1_\2/'`; \
    fi;\
	boot=booter.bin;\
    (cd $(XDSL_INSTALL_DIR); ln -sf $(notdir $(1)) $$boot);\
  fi;
endef

all: $(TARGETS)
	@tools/patch_config_booters $(BIN_DIR) $(INBAND_ADDRESS)
	$(RM) -rf $(XDSL_INSTALL_DIR)
	mkdir -p $(XDSL_INSTALL_DIR)
	$(foreach boot,$(BOOTER_SOURCE),$(call CMD_INSTALL_AND_SOFTLINK_BOOTER,$(boot),))
        # install alt booter if present
	$(call CMD_INSTALL_AND_SOFTLINK_BOOTER,$(BOOTER_ALT_SOURCE),$(BOOTER_ALT_DEST))
        # only install old booter when source boot major rev > 6
	if [ -n "$(BOOTER_OLD_SOURCE)" -a X`echo $(firstword $(BOOTER_SOURCE)) | perl -ne '/(\d+)_\d+_\d+_[a-z0-9]+\.bin/x && do {print 1 if $$1 > 6;}'` == X1 ]; then \
	  $(foreach boot,$(BOOTER_OLD_SOURCE),$(call CMD_INSTALL_AND_SOFTLINK_BOOTER,$(boot),$(BOOTER_OLD_DEST))) \
	  $(call CMD_INSTALL_AND_SOFTLINK_BOOTER,$(BOOTER_ALT_OLD_SOURCE),$(BOOTER_ALT_OLD_DEST)) \
	fi
	install $(FIRMWARE_BIN) $(XDSL_INSTALL_DIR)
	(cd $(XDSL_INSTALL_DIR); $(foreach fw,$(FIRMWARE_BIN),\
          ln -s $(notdir $(fw)) modem_$(word 5,$(subst _, ,$(notdir $(fw))));))
	install tools/xdsl_cmd_*.txt $(XDSL_INSTALL_DIR)
	install tools/cpld_*.txt $(XDSL_INSTALL_DIR)
	install tools/howto.txt $(XDSL_INSTALL_DIR)
	install tools/xdsl.sh $(INSTALL_DIR)/etc/init.d/
	install -m 755 tools/reset_device $(INSTALL_DIR)/etc/xdsl/reset_device
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S82xdsl; ln -s ../init.d/xdsl.sh S82xdsl)
	install -m 755 tools/xdslcli $(INSTALL_DIR)/bin/xdslcli

$(XDSL_TARGETS):
	$(MAKE) $(CURR_DIR)/$@
	$(MAKE) -C $(CURR_DIR)/$@/$(XDSL_COMPILE_DIR) all PHY=$(if $(findstring fttdp,$@),ethernet,ioctl683xx) TARGET=$@
	install -m 755 $(CURR_DIR)/$@/$(XDSL_COMPILE_DIR)/bcm_modem $(INSTALL_DIR)/bin/$@
	$(STRIP) $(INSTALL_DIR)/bin/$@

telnetd4cli: 
	$(MAKE) $(CURR_DIR)/$@
	$(MAKE) -C $(CURR_DIR)/$@/support $@
	install -m 755 $(CURR_DIR)/$@/support/$@ $(INSTALL_DIR)/bin/$@

max9611: $(CURR_DIR)/tools/max9611
	install -m 755 $< $(INSTALL_DIR)/bin/$@

965200dpf_traffic:
	install tools/fttdp_gbe.sh $(INSTALL_DIR)/etc/init.d/
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S83gbe; ln -s ../init.d/fttdp_gbe.sh S83gbe)
	install tools/gpon_tm.sh $(INSTALL_DIR)/etc/init.d/
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S84gpon; ln -s ../init.d/gpon_tm.sh S84gpon)

# build dir for DSL SDK
$(BLD_DIR): $(SRC_DIR)
	$(MAKE) -C $(SRC_DIR) BLD_ROOT=$(CURR_DIR) BLD_DIR=$@ $@
	mkdir -p $@/build
	@[ -f $@/build/top_flags.mak ] || (echo sinclude $(SRC_DIR)/top_flags/6838.mak; echo sinclude $(CURR_DIR)/libfix.mak) >> $@/build/top_flags.mak
	@[ ! -f $(BIN_DIR)/bcm_logAndDebugSupport.c ] || cp -a $(BIN_DIR)/bcm_logAndDebugSupport.c $@/logAndDebug

ifeq (,$(wildcard $(SRC_DIR)))
# no src extracted yet => first thing to do
$(SRC_DIR): extract
	touch src
	echo fooo
else ifneq (,$(wildcard $(CURR_DIR)/dsl-release))
$(SRC_DIR): $(CURR_DIR)/dsl-release/*_delivery_cs*.tgz
	@echo 'A DSL DSK release, more recent compared to last build is available.'
	@echo 'In case you want to use it do a [make distclean] in [$(CURR_DIR)].'
	@read -p "Press ENTER to continue and ignore the new release, or CTRL-C to abort. "
endif

extract:
	@tools/extract_dsl $(SRC_DIR) $(BIN_DIR)
ifneq (,$(filter 955045dpu%,$(BUILD_BRCM_XDSL_DISTPOINT)))
	@echo patching not-up-to-date tarball
	patch -p2 -d src < dsl-release/55045dpu.diff
endif

clean:
	$(RM) -rf $(BLD_DIR) xdslctl_*

distclean: clean
	$(RM) -rf $(BIN_DIR) $(SRC_DIR)

print-%:
	@echo '$*=$($*)'
	@echo '  origin = $(origin $*)'
	@echo '  flavor = $(flavor $*)'
	@echo '   value = $(value $*)'

.PHONY: all extract clean distclean $(TARGETS) $(BLD_DIR)
.NOTPARALLEL:
