

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

APP = felix_framework


.PHONY: check_untar_patch_configure conditional_build check_versions

ifneq ($(strip $(BUILD_OSGI_FELIX)),)
conditional_build: $(APP)

else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"

endif

felix_framework: sanity_check
	cd felix_framework; (tar xkfz ../felix.tar.gz 2> /dev/null || true)
	$(MAKE) -C ./felix_framework
#this bundle is used for debugging only.  Felix will run in the background, so this needs to be removed.
#       rm -f $(INSTALL_DIR)/usr/local/felix/bundle/org.apache.felix.gogo.shell-0.7.0-SNAPSHOT.jar
#

clean:
	-make -C felix_framework clean

