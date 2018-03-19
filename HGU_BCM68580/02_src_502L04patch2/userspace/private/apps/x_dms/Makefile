
dynamic default: conditional_build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
export BUILD_DIR

include $(BUILD_DIR)/make.common

ifeq ($(strip $(DESKTOP_LINUX)),y)
BCM_BLT32_FLAGS := $(BCM_LD_FLAGS)
export BCM_BLT32_FLAGS
endif

ifneq ($(strip $(BUILD_DLNA)),)

conditional_build:
	(tar xkfj x_dms.tar.bz2 2> /dev/null || true)
	$(MAKE) -j 1 -C x_dms dynamic

else

conditional_build:
	@echo "x_dms skipped (not configured)"

endif

clean:
	$(MAKE) -j 1 -C x_dms clean

