
default: conditional_build 


CURR_DIR  := $(shell pwd)
BUILD_DIR := $(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR := $(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

APP     := alljoyn
VERSION := 15.09.00a
SCONS   := $(HOSTTOOLS_DIR)/local_install/scons/bin/scons
APP_DIR := $(CURR_DIR)/$(APP)-$(VERSION)

BIN_DIR := $(INSTALL_DIR)/bin
LIB_DIR := $(INSTALL_DIR)/lib
INCLUDE_DIR := $(BCM_FSBUILD_DIR)/public/include

LIBCAP_DIR := $(CURR_DIR)/../libcap

ALLJOYN_BIN_DIR_C := $(APP_DIR)/build/broadcom/arm/debug/dist/c/bin
ALLJOYN_INC_DIR_C := $(APP_DIR)/build/broadcom/arm/debug/dist/c/inc
ALLJOYN_LIB_DIR_C := $(APP_DIR)/build/broadcom/arm/debug/dist/c/lib
ALLJOYN_BIN_DIR_CPP := $(APP_DIR)/build/broadcom/arm/debug/dist/cpp/bin
ALLJOYN_INC_DIR_CPP := $(APP_DIR)/build/broadcom/arm/debug/dist/cpp/inc
ALLJOYN_LIB_DIR_CPP := $(APP_DIR)/build/broadcom/arm/debug/dist/cpp/lib

ifneq ($(strip $(BUILD_ALLJOYN)),)
conditional_build: all
else
conditional_build:
	@echo "skipping alljoyn (not configured)"
endif

# Depend on libcap
check_libcap:
	if [ ! -e $(LIB_DIR)/libcap.so ]; then \
		cd $(LIBCAP_DIR); \
		make; \
		echo "libcap is built"; \
	fi;

$(APP_DIR)/SConstruct: $(APP)-$(VERSION)-src.tar.gz
	if [ ! -e $(APP_DIR) ]; then \
	    mkdir $(APP_DIR); \
	    cd $(APP_DIR); \
	    tar --strip-components=1 -xzf ../$(APP)-$(VERSION)-src.tar.gz; \
	    mkdir build_core/conf/broadcom; \
	    mkdir build_core/conf/broadcom/arm; \
	    cp -p ../SConscript build_core/SConscript; \
	    cp -p ../SConscript.bcm build_core/conf/broadcom/SConscript; \
	    cp -p ../SConscript.bcm.arm build_core/conf/broadcom/arm/SConscript; \
	    cd ..; \
	    touch -c $(APP_DIR)/SConstruct; \
	    echo "$(APP)-$(VERSION) is untarred"; \
	fi;

# Depend on $(APP_DIR)/SConstruct so that tar progress should be done only once
check_config: $(APP_DIR)/SConstruct

all: check_libcap check_config
	if [ ! -e $(ALLJOYN_LIB_DIR_CPP) ]; then \
		cd $(APP_DIR); \
		$(SCONS) OS=broadcom CPU=arm CROSS_COMPILE=$(CROSS_COMPILE) INCLUDE_PATH=$(INCLUDE_DIR) LIB_PATH=$(LIB_DIR) WS=off BT=off ICE=off BR=on BINDINGS=c,cpp SERVICES="about,notification,controlpanel,config,onboarding,sample_apps"; \
		cd ..; \
		echo "Done building $(APP)-$(VERSION)"; \
	fi;
#	cp -p $(ALLJOYN_BIN_DIR_C)/bbc* $(BIN_DIR);
	cp -p $(ALLJOYN_BIN_DIR_CPP)/bbclient $(BIN_DIR);
	cp -p $(ALLJOYN_BIN_DIR_CPP)/bbservice $(BIN_DIR);
	cp -p $(ALLJOYN_BIN_DIR_CPP)/bbsig $(BIN_DIR);
	cp -p $(ALLJOYN_LIB_DIR_C)/*.so $(LIB_DIR);
	cp -p $(ALLJOYN_LIB_DIR_CPP)/*.so $(LIB_DIR);
	cp -rf $(ALLJOYN_INC_DIR_C)/* $(INCLUDE_DIR);
	cp -rf $(ALLJOYN_INC_DIR_CPP)/* $(INCLUDE_DIR);
	cp -pf platform_types.h $(INCLUDE_DIR)/qcc/posix/platform_types.h;

clean:
	rm -f $(BIN_DIR)/bbclient
	rm -f $(BIN_DIR)/bbservice
	rm -f $(BIN_DIR)/bbsig
	rm -f $(LIB_DIR)/alljo*.so
	rm -rf $(INCLUDE_DIR)/alljoyn
	rm -rf $(INCLUDE_DIR)/alljoyn_c
	rm -rf $(INCLUDE_DIR)/qcc
	rm -rf $(APP_DIR)

shell:
	bash -i

