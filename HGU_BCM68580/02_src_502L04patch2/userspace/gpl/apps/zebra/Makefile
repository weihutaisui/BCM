all:    conditional_build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common


ifneq ($(strip $(BUILD_ZEBRA)),)
conditional_build:
	(tar xkfj zebra.tar.bz2 2> /dev/null || true)
	$(MAKE) -C zebra $(BUILD_ZEBRA)
else
conditional_build: 
	@echo "skipping zebra (not configured)"
endif

clean:
	$(MAKE) -C zebra clean

