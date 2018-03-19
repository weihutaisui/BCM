# Makefile to build LXC

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
# You do not need to modify this part.
#
all dynamic install: conditional_build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

all dynamic install:

include $(BUILD_DIR)/make.common


LDFLAGS = -L$(BCM_FSBUILD_DIR)/public/lib
LIBS = -lcms_msg\ -lcms_util\ -lbcm_boardctl\ -lbcm_flashutil\ -lbcm_crc

CFLAGS = -I$(BUILD_DIR)/userspace/public/include -I$(BUILD_DIR)/userspace/public/include/linux
ifeq ($(strip $(DESKTOP_LINUX)),y)
CFLAGS += $(BCM_LD_FLAGS)
ifeq ($(strip $(BUILD_DESKTOP_BEEP)), y)
CFLAGS += -DBUILD_DESKTOP_BEEP
endif
endif



APP = lxc-2.0.4
START_LXC = S64lxc

ifneq ($(strip $(BUILD_LXC)),)
check_untar_patch_configure: sanity_check
	if [ ! -e $(APP)/Makefile.in ]; then \
		echo "Untarring source and overrides..." ; \
		(tar xfz $(APP).tar.gz 2> /dev/null || true) ; \
		echo "Applying patches to $(APP)" ; \
		patch -p1 -b -N -s -d$(APP) < $(APP).patch ; \
		cd $(APP); \
		./autogen.sh ; \
		./configure --host=${TOOLCHAIN_PREFIX} --prefix= \
		--enable-doc=no --enable-api-docs=no --enable-examples=no \
		--enable-bash=no --enable-python=no \
		CFLAGS="$(CFLAGS)" \
		LDFLAGS=$(LDFLAGS) \
		LIBS=$(LIBS); \
	fi

conditional_build: check_untar_patch_configure
	@echo "Making $(APP)"
	@cd $(APP); \
	$(MAKE) &&  \
	$(MAKE) install DESTDIR=$(INSTALL_DIR)
	mkdir -p $(INSTALL_DIR)/etc/init.d
	mkdir -p $(INSTALL_DIR)/etc/rc3.d
	rm -f $(INSTALL_DIR)/run
	ln -s /var/run $(INSTALL_DIR)/run
	install -m 0755 lxc.sh $(INSTALL_DIR)/etc/init.d
	(cd $(INSTALL_DIR)/etc/rc3.d; rm -f $(START_LXC); ln -s ../init.d/lxc.sh $(START_LXC))


else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"

endif

# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean:
	@if [ -e $(APP)/Makefile ]; then \
		cd $(APP); $(MAKE) clean ; \
	fi
	rm -rf $(APP)
	@rm -f $(INSTALL_DIR)/etc/init.d/lxc.sh
	@rm -f $(INSTALL_DIR)/etc/rc3.d/$(START_LXC)
	@rm -f $(INSTALL_DIR)/run

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean
	
bcm_dorel_distclean: distclean

