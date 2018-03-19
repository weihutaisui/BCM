#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
# You do not need to modify this part.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
APP := stress-1.0.4

stress: conditional_build

include $(BUILD_DIR)/make.common

ifneq ($(strip $(BUILD_POSIX_STRESS)),)
conditional_build: all
else
conditional_build:
	@echo "skipping POSIX stress utility (not configured)"
endif


all: objs/$(PROFILE_ARCH)/Makefile
	@echo "Making $(APP)"
	$(MAKE) -C objs/$(PROFILE_ARCH)
	mkdir -p $(INSTALL_DIR)/bin$(BCM_INSTALL_SUFFIX_DIR)
	install -m 755 objs/$(PROFILE_ARCH)/src/stress $(INSTALL_DIR)/bin$(BCM_INSTALL_SUFFIX_DIR)/posix-stress

objs/$(PROFILE_ARCH)/Makefile: $(APP)/configure
	mkdir -p objs/$(PROFILE_ARCH)
	cd objs/$(PROFILE_ARCH) ; ../../$(APP)/configure --host=$(TOOLCHAIN_PREFIX) --prefix=$(BCM_FSBUILD_DIR)/public

$(APP)/configure:
	@echo "Untarring source and overrides..."
	@(tar xfz $(APP).tar.gz 2> /dev/null || true)

distclean clean:
	@rm -rf $(APP) objs

