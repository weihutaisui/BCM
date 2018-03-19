EXE = bcmmserver


LIB= libbmedia.so libb_playback_ip.so libbdlna.so libbdlna-dms-aal.so libbdlna-dms.so


dynamic all install: generic_exe_install generic_private_lib_install
	if ! [ -d $(INSTALL_DIR)/webs/html_root ]; then mkdir -p $(INSTALL_DIR)/webs/html_root/ ; fi
	cp -rf html_root $(INSTALL_DIR)/webs/. 


clean:
	rm -f bcmmserver libbmedia.so libb_playback_ip.so libbdlna.so libbdlna-dms-aal.so libbdlna-dms.so


CURR_DIR := $(shell pwd)

BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))

BUILD_DIR := $(word 1,$(BUILD_DIR))


include $(BUILD_DIR)/make.common



% : %.$(PROFILE_ARCH).save
	cp $< $@


