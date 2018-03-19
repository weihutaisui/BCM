#
# In most cases, you only need to modify this first section.
#

all: build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

CROSS := $(CROSS_COMPILE)
export CROSS

CFLAGS += -Wno-error=unused-parameter

ifneq ($(strip $(BUILD_LMBENCH)),)
build: 
	-tar xkzf lmbench-3.0-a9.tgz 2>/dev/null
	cd lmbench-3.0-a9/src ; make CC=$(CC) OS=$(TOOLCHAIN_PREFIX)
	mkdir -p $(INSTALL_DIR)/opt/lmbench/bin
	cp -f lmbench-3.0-a9/bin/$(TOOLCHAIN_PREFIX)/* $(INSTALL_DIR)/opt/lmbench/bin

else
build: 
	echo "skipping lmbench (not configured)"

endif

clean: 
	rm -rf lmbench-3.0-a9

# Shell target permits creating an interacive shell for debugging
shell:
	bash -i


