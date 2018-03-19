default:all

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
WLAN_DIR:=$(BUILD_DIR)/userspace/private/apps/wlan
include $(BUILD_DIR)/make.common

all install:
	$(MAKE) -C $(WLAN_DIR)/nvram all 

distclean clean:
	$(MAKE) -C $(WLAN_DIR)/nvram clean 
