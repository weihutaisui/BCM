# Makefile to build DHCP Client

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

APP = udhcp-0.9.6

ifneq ($(strip $(BUILD_BEEP)),)
check_untar_patch_configure: sanity_check
	if [ ! -e $(APP)/Makefile ]; then \
		echo "Untarring source and overrides..." ; \
		(tar xfz $(APP).orig.tar.gz 2> /dev/null || true) ; \
		echo "Applying patches to $(APP)" ; \
		patch -p1 -b -N -s -d$(APP) < $(APP).patch ; \
	fi

conditional_build: check_untar_patch_configure
	@echo "Making $(APP)"
	@cd $(APP); \
	make; \
	install -m 755 udhcpc $(INSTALL_DIR)/bin/dhcpclient
	$(STRIP) $(INSTALL_DIR)/bin/dhcpclient


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

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean
	
bcm_dorel_distclean: distclean

