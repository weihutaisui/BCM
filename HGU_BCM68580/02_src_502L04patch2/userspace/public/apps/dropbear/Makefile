
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

ifneq ($(strip $(BUILD_BRCM_CMS)),)
LIBS += -lcms_cli -lcms_dal -lcms_msg $(CMS_CORE_LIBS) $(CMS_COMMON_LIBS)
CUSTOM_CFLAGS += -DBRCM_CMS_BUILD
CFLAGS +=$(ALLOWED_INCLUDE_PATHS)
endif

ifeq ($(strip $(DESKTOP_LINUX)),y)
BCM_BLT32_FLAGS := $(BCM_LD_FLAGS)
export BCM_BLT32_FLAGS
endif

#
# Public apps are only allowed to include header files from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I.\
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR)

APP = dropbear-2016.74

clean:
	rm -rf $(APP)

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean
	rm -rf $(APP)

#.PHONY: check_untar_patch_configure conditional_build

# Public apps are only allowed to link with libraries from the
# private and public directories.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_LIB_DIRS := /lib:/lib/private:/lib/public

ifneq ($(strip $(BUILD_SSHD)),)

check_untar_patch_configure: sanity_check
	@if [ ! -e $(APP)/Makefile.in ]; then \
    	echo "Untarring original $(APP) source"; \
	    (tar xkf $(APP).tar.bz2 2> /dev/null || true); \
	    patch -p1 -b -s -d$(APP) < $(APP).patch; \
	    if [ "x$(BUILD_BRCM_CMS)" != "x" ]; then \
		patch -p1 -b -s -d$(APP) < $(APP)_cms.patch; \
	    fi \
	fi; \
	    (cd $(APP); ./configure CFLAGS="$(CFLAGS)" LDFLAGS="$(BCM_BLT32_FLAGS) -L$(INSTALL_DIR)/lib" LIBS="$(LIBS)" --host=$(TOOLCHAIN_PREFIX) \
        --target=$(TOOLCHAIN_PREFIX) \
        --disable-largefile --disable-zlib  --disable-openpty --disable-syslog  --disable-shadow  --disable-lastlog  --disable-utmp --disable-utmpx --disable-wtmp --disable-wtmpx  \
        --prefix=$(INSTALL_DIR) \
        --sbindir=$(INSTALL_DIR)/bin); 

conditional_build: check_untar_patch_configure
	cd $(APP); $(MAKE)
	install -m 755 $(APP)/dropbear  $(INSTALL_DIR)/bin/sshd
	install -m 755 $(APP)/dropbearconvert $(INSTALL_DIR)/bin/dropbearconvert
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)



