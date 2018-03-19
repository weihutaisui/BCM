
all dynamic install: conditional_build


#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
#

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

clean:
	rm -f $(INSTALL_DIR)/etc/init.d/gencfg_init.sh
	rm -f $(INSTALL_DIR)/etc/rc3.d/S80gencfg_init.sh
ifeq ($(BCM_PON),y)
	rm -f $(INSTALL_DIR)/etc/rc3.d/S90gencfg_pon.sh
endif	

ifneq ($(strip $(BUILD_GENCFG_TOOLS)),)

conditional_build: 
	mkdir -p $(INSTALL_DIR)/etc/init.d
	mkdir -p $(INSTALL_DIR)/etc/rc3.d
	install scripts/gencfg_init.sh $(INSTALL_DIR)/etc/init.d
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S80gencfg_init.sh ; ln -s ../init.d/gencfg_init.sh S80gencfg_init.sh)
ifeq ($(BCM_PON),y)
	install scripts/gencfg_pon.sh $(INSTALL_DIR)/etc/init.d
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f S90gencfg_pon.sh ; ln -s ../init.d/gencfg_pon.sh S90gencfg_pon.sh)
endif	
else
conditional_build:
	@echo "Skipping GENCFG_TOOLS (not configured)"
endif

