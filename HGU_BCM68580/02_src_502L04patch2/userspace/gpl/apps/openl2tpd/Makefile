

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

APP = openl2tpd


.PHONY: check_untar_patch_configure conditional_build check_versions

ifneq ($(strip $(BUILD_L2TPAC)),)
conditional_build: $(APP)

else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"

endif

openl2tpd: sanity_check
	(tar xkjf openl2tpd.tar.bz2 2> /dev/null || true)
	$(MAKE) -C openl2tpd -j1


clean:
	-make -C openl2tpd clean

