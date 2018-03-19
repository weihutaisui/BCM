
dynamic install: conditional_build

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

APP = e2fsprogs-1.43.4


.PHONY: check_untar check_configure conditional_build check_versions $(APP)

ifneq ($(strip $(BUILD_E2FSPROGS)),)
conditional_build: all

else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"

endif

check_configure: $(PROFILE_ARCH)/Makefile 

$(PROFILE_ARCH)/Makefile: check_untar
	if [ ! -e $(PROFILE_ARCH)/Makefile ]; then \
		mkdir -p $(PROFILE_ARCH) ;\
		cd $(PROFILE_ARCH) && ../$(APP)/configure --host=$(TOOLCHAIN_PREFIX) --prefix=$(BCM_FSBUILD_DIR)/gpl/ ;\
		\rm -rf $(PROFILE_ARCH)/doc ;\
	fi

check_untar: $(APP)/README

$(APP)/README: $(APP).tar.gz
	if [ ! -e $(APP)/Makefile.in ]; then \
		mkdir -p $(APP); \
		(cd $(APP) && tar --strip-components=1 -xkzf ../$(APP).tar.gz  || true) ; \
		echo "Applying patches to $(APP)" ; \
		patch -p1 -b -N -s -d$(APP) < $(APP).patch ; \
	fi

all: check_configure
	$(MAKE) -C $(PROFILE_ARCH)
	$(MAKE) -C $(PROFILE_ARCH) install
	mkdir -p $(INSTALL_DIR)/bin
	-cp $(BCM_FSBUILD_DIR)/gpl/sbin/e2fsck $(INSTALL_DIR)/bin/
	-cp $(BCM_FSBUILD_DIR)/gpl/sbin/mke2fs $(INSTALL_DIR)/bin/
	-cp $(BCM_FSBUILD_DIR)/gpl/sbin/resize2fs $(INSTALL_DIR)/bin/
	-cp $(BCM_FSBUILD_DIR)/gpl/sbin/dumpe2fs $(INSTALL_DIR)/bin/

clean:
	if [ -e $(PROFILE_ARCH) ]; then \
		$(MAKE) -C $(PROFILE_ARCH) uninstall; \
		$(MAKE) -C $(PROFILE_ARCH) clean; \
		rm -rf $(PROFILE_ARCH); \
	fi
	rm -f $(INSTALL_DIR)/bin/mke2fs
	rm -f $(INSTALL_DIR)/bin/e2fsck
	rm -f $(INSTALL_DIR)/bin/resize2fs
	rm -f $(INSTALL_DIR)/bin/dumpe2fs
	rm -rf $(APP)

bcm_dorel_distclean:
	rm -rf $(PROFILE_ARCH) $(APP)

shell:
	bash -i

