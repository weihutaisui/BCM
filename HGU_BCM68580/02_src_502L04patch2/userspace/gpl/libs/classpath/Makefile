#***********************************************************************
#
#  Copyright (c) 2012  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/

LIB = classpath-0.98

all dynamic install: conditional_build

clean:
	rm -rf $(LIB)
	rm -rf $(INSTALL_DIR)/usr/local/classpath
	rm -f $(INSTALL_DIR)/lib/libjavanet.so


# Since we do not have any checked in files below classpath-0.98, we
# can rm -rf the whole directory before release.  The next line is
# a hint to to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean
	rm -rf $(LIB)


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


ifneq ($(strip $(BUILD_JVM_CLASSPATH)),)
check_untar_configure_patch: sanity_check
	@if [ ! -e $(LIB)/README ]; then \
	echo "Untarring original $(LIB) source and configure"; \
	(tar xkfz $(LIB).tar.gz 2> /dev/null || true); \
	(cd $(LIB); ./configure --host=mips-linux-uclibc \
        --target=mips-linux-uclibc \
        --enable-local-sockets --disable-gconf-peer --disable-gtk-peer \
        --disable-plugin --enable-jni --disable-alsa --disable-dssi \
        --disable-gjdoc --disable-libtool-lock --disable-default_toolkit \
        --disable-gmp --disable-examples --without-x \
        --prefix=$(INSTALL_DIR)/usr/local/classpath \
        CFLAGS=-g\ -O2\ -Wno-strict-aliasing\ -Wno-error=unused-parameter ); \
	echo "Applying patches to $(LIB)"; \
	patch -p1 -b -d$(LIB) < bcm_patches/still_accept_on_eintr.patch; \
    fi

conditional_build: check_untar_configure_patch
	mkdir -p $(INSTALL_DIR)/usr/local/classpath/share/classpath
	mkdir -p $(INSTALL_DIR)/usr/local/classpath/lib/classpath
	mkdir -p $(INSTALL_DIR)/lib
	cd $(LIB); $(MAKE) install
	cd $(INSTALL_DIR)/usr/local/classpath/share/classpath; unzip -o glibj.zip
	cd $(INSTALL_DIR)/usr/local/classpath/share/classpath; rm -rf glibj.zip
	cp -f $(INSTALL_DIR)/usr/local/classpath/lib/classpath/libjavanet.so $(INSTALL_DIR)/lib/libjavanet.so
else
conditional_build: sanity_check
	@echo "skipping $(LIB) (not configured)"
endif
