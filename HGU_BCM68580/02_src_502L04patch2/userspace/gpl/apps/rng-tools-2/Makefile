

all dynamic install: conditional_build

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
# You do not need to modify this part.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

APP = rng-tools-2


.PHONY: check_untar_patch_configure conditional_build check_versions

ifneq ($(strip $(BUILD_RNGD)),)
conditional_build: $(APP)

else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"

endif

rng-tools-2: sanity_check
	mkdir -p $(INSTALL_DIR)/bin
	(tar xkfz rng-tools-2.tar.gz 2> /dev/null || true)
	($(MAKE) -C rng-tools-2 $(BUILD_RNGD))


clean:
	-make -C rng-tools-2 clean

