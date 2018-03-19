
default: conditional_build 


CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common


ifneq ($(strip $(BUILD_IPV6)),)
conditional_build: all
else
conditional_build:
	@echo "skipping dhcpv6 (ipv6 not configured)"
endif



all: 
	(tar xkjf dhcpv6.tar.bz2 2> /dev/null || true)
	$(MAKE) -C dhcpv6 -j1


clean:
	-make -C dhcpv6 -j1 clean


shell:
	bash -i

