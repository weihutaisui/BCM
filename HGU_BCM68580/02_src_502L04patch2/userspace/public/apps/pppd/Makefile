
default: conditional_build 

# BRCM_SUPPORTS_MULTIARCH_BUILD

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common


ifneq ($(strip $(BUILD_PPPD)),)
conditional_build: all
else
conditional_build:
	@echo "skipping pppd (not configured)"
endif


all: 
ifneq ($(strip $(BUILD_BRCM_CMS)),)
	$(MAKE) -C ppp/pppoe $(BUILD_PPPD)
else
	$(MAKE) -C ppp/pppoe
endif

clean:
	$(MAKE) -C ppp/pppoe clean


shell:
	bash -i

