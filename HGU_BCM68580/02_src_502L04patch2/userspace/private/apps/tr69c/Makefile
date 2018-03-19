
#GLOBAL_RELEASE_SCRIPT_NOTOUCH


ifneq ($(wildcard Makefile.fullsrc),)

include Makefile.fullsrc

else

EXE = tr69c

all dynamic install: conditional_build


#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
#

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

ifneq ($(strip $(BUILD_TR69C)),)
conditional_build: tr69c generic_exe_install

clean:
	rm -f $(EXE)

else 
ifneq ($(strip $(BUILD_TR69C_BEEP)),)

PACKAGE_NAME = $(EXE)
BEEPPKG_INFO = $(PACKAGE_NAME)$(BEEP_PKT_INFO_SUFIX)
BEEP_TAR_GZ =  $(BEEP_TAR_GZ_PREFIX)*$(PACKAGE_NAME)*$(BEEP_TAR_GZ_SUFIX)

conditional_build: tr69c_beep $(BEEPPKG_INFO)
	mkdir -p ./pkg/bin
	install -m 755 ./$(EXE) ./pkg/bin/$(EXE)
	mkdir -p ./pkg/lib
	cp -f $(INSTALL_DIR)/lib/libnanoxml.so ./pkg/lib
	cp -f $(INSTALL_DIR)/lib/libssl.so.1.0.0 ./pkg/lib
	cp -f $(INSTALL_DIR)/lib/libcrypto.so.1.0.0 ./pkg/lib
	cp -f $(INSTALL_DIR)/lib/libbcm_sslconf.so ./pkg/lib
	if [ -a $(INSTALL_DIR)/lib/libstrophe.so ] ; then \
		cp -f $(INSTALL_DIR)/lib/libstrophe.so ./pkg/lib ; \
	fi;
	mkdir -p ./pkg/etc/openssl
	cp -f $(INSTALL_DIR)/etc/openssl/sslconf.xml ./pkg/etc/openssl
	mkdir -p ./pkg/etc/ssl
	cp -f $(INSTALL_DIR)/etc/ssl/openssl.cnf ./pkg/etc/ssl/openssl.cnf
	tar zcf $(PACKAGE_NAME)$(BEEP_TAR_GZ_SUFIX) -C ./pkg .
	mkdir -p $(USERSPACE_DL_MODULES_BEEP_DIR)
	$(HOSTTOOLS_DIR)/beep/beepPkgBuilder -f $(BEEPPKG_INFO)
	mv -f $(BEEP_TAR_GZ) $(USERSPACE_DL_MODULES_BEEP_DIR)	
	rm -rf ./pkg
	rm -f $(PACKAGE_NAME)$(BEEP_TAR_GZ_SUFIX)

clean:
	rm -f $(EXE)
	rm -rf $(USERSPACE_DL_MODULES_BEEP_DIR)/$(BEEP_TAR_GZ)

else
conditional_build:
	@echo "skipping  $(EXE) (not configured)"

clean:

endif # BUILD_TR69C_BEEP
endif # BUILD_TR69C

tr69c:
	cp $(EXE)_$(PROFILE_ARCH).save $(EXE)

tr69c_beep:
	cp $(EXE)_$(PROFILE_ARCH)_BEEP.save $(EXE)

endif # NO_Makefile.fullsrc

