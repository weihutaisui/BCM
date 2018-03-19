
all dynamic install: conditional_build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

UNTAR_COMPLETE_FILE = .bcm_untar_complete
CONFIG_COMPLETE_FILE = bcm_config_complete

APP = iperf-3.1.4

ifneq ($(strip $(BUILD_IPERF)),)
$(APP)/$(UNTAR_COMPLETE_FILE): $(APP).tar.gz Makefile
	rm -rf $(APP)
	tar xkf $(APP).tar.gz 2> /dev/null || true
	@echo "Applying patches to $(APP)"
	patch -p1 -b -s -d$(APP) < $(APP).patch
	@echo "$(APP) is untarred"
	touch $(APP)/$(UNTAR_COMPLETE_FILE)

$(APP)/$(CONFIG_COMPLETE_FILE): $(APP)/$(UNTAR_COMPLETE_FILE) 
	(cd $(APP); ./configure \
        --host=$(TOOLCHAIN_PREFIX) \
        --target=$(TOOLCHAIN_PREFIX) \
        --enable-shared=no --enable-static=yes);
	@echo "$(APP) is configured"
	touch $(APP)/$(CONFIG_COMPLETE_FILE)

conditional_build: $(APP)/$(CONFIG_COMPLETE_FILE) sanity_check
	$(MAKE) -C $(APP)
	install -m 755 $(APP)/src/iperf3 $(INSTALL_DIR)/bin/iperf3

else
conditional_build: sanity_check
	@echo "skipping iperf (not configured)"
endif

clean:
	rm -f $(INSTALL_DIR)/bin/iperf3
	rm -rf $(APP)

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

shell:
	bash

