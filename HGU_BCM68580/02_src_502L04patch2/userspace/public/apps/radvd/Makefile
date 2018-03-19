
default: conditional_build 


CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common


ifneq ($(strip $(BUILD_IPV6)),)
conditional_build: all
else
conditional_build:
	@echo "skipping radvd (ipv6 not configured)"
endif



all: 
	(tar xkjf radvd.tar.bz2 2>/dev/null  || true)
	$(MAKE) -C radvd


clean:
	-make -C radvd clean


shell:
	bash -i

