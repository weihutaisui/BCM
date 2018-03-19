
all:	conditional_build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

APP = iperf-2.0.9
CXX := $(CC)
CFLAGS := -lstlport
LDFLAGS := -lstlport -lm -L$(USERSPACE_PUBLIC_LIBS_DIR)/stlport/lib

PACKAGE_NAME = iPerf2
BEEPPKG_INFO = $(PACKAGE_NAME)$(BEEP_PKT_INFO_SUFIX)
BEEP_TAR_GZ = $(BEEP_TAR_GZ_PREFIX)$(PACKAGE_NAME)$(BEEP_TAR_GZ_SUFIX)

ifneq ($(strip $(BUILD_BEEP_IPERFV2)),)
ALLOWED_INCLUDE_PATHS := -I$(BUILD_DIR)/userspace/public/include
CFLAGS += $(ALLOWED_INCLUDE_PATHS)
CFLAGS += -I$(BCM_FSBUILD_DIR)/public/lib/glib-2.0/include -I$(BCM_FSBUILD_DIR)/public/include/glib-2.0
CFLAGS += -I$(BCM_FSBUILD_DIR)/public/include/gio-unix-2.0
CFLAGS += -I$(TOOLCHAIN)/include

LIBS += -L$(INSTALL_DIR)/lib/public -lexpat
LIBS += -L$(INSTALL_DIR)/lib -lgio-2.0 -lglib-2.0 -lgmodule-2.0 -lgobject-2.0 -lgthread-2.0 -lz -lffi -ldbus-1
endif

CXXFLAGS := $(CFLAGS)
export CXX CFLAGS CXXFLAGS LDFLAGS CC USERSPACE_PUBLIC_LIBS_DIR LIBS

ifneq ($(strip $(BUILD_BEEP_IPERFV2)),)
conditional_build: $(BEEP_TAR_GZ)
else
ifneq ($(strip $(BUILD_IPERF)),)
conditional_build: build
else
conditional_build: 
	@echo "skipping iperf (not configured)"
endif
endif

build:
	(tar xkfz $(APP).tar.gz 2> /dev/null || true)
	if [ ! -e ./$(APP)/Makefile ];then \
	patch -p1 -b -s -d $(APP) <$(APP).patch; \
	cd ./$(APP); ./configure --host=$(TOOLCHAIN_PREFIX) --target=$(TOOLCHAIN_PREFIX) ac_cv_func_malloc_0_nonnull=yes; \
	fi
	echo "configure is done"
	$(MAKE) -j 1 -C ./$(APP) srcdir=./ builddir=./
	install -m 755 ./$(APP)/src/iperf $(INSTALL_DIR)/bin/iperf

check_patch_file: $(APP)_BEEP.patch

build_beep: check_patch_file
	if [ ! -e $(APP)_beep ]; then \
            mkdir $(APP)_beep; cd $(APP)_beep; \
            tar --strip-components=1 -xkf ../$(APP).tar.gz; \
            cd ..; \
        fi
	if [ ! -e ./$(APP)_beep/Makefile ];then \
	patch -p1 -b -s -d $(APP)_beep <$(APP).patch; \
	patch -p1 -b -s -d $(APP)_beep <$(APP)_BEEP.patch; \
	cd ./$(APP)_beep; ./configure --host=$(TOOLCHAIN_PREFIX) --target=$(TOOLCHAIN_PREFIX) ac_cv_func_malloc_0_nonnull=yes; \
	fi
	echo "configure is done"
	$(MAKE) -j 1 -C ./$(APP)_beep srcdir=./ builddir=./

$(BEEP_TAR_GZ): build_beep $(BEEPPKG_INFO)
	mkdir -p ./pkg/bin
	install -m 755 ./$(APP)_beep/src/iperf ./pkg//bin/$(PACKAGE_NAME)
	$(STRIP) ./pkg//bin/$(PACKAGE_NAME)
	mkdir -p ./pkg/lib
	cp -f $(INSTALL_DIR)/lib/libstlport.so.5.2 ./pkg/lib
	$(STRIP) ./pkg/lib/libstlport.so.5.2
	tar zcf $(PACKAGE_NAME)$(BEEP_TAR_GZ_SUFIX) -C ./pkg .
	mkdir -p $(USERSPACE_DL_MODULES_BEEP_DIR)
	$(HOSTTOOLS_DIR)/beep/beepPkgBuilder -f $(BEEPPKG_INFO) -a sha256
	mv -f  $(BEEP_TAR_GZ_PREFIX)*$(PACKAGE_NAME)*$(BEEP_TAR_GZ_SUFIX) $(USERSPACE_DL_MODULES_BEEP_DIR)
	rm -rf ./pkg
	rm -f $(PACKAGE_NAME)$(BEEP_TAR_GZ_SUFIX)	
	
clean:
	if [ -f ./$(APP)/Makefile ];then $(MAKE) -C ./$(APP) distclean srcdir=./ builddir=./;fi;
	if [ -f ./$(APP)_beep/Makefile ];then $(MAKE) -C ./$(APP)_beep distclean srcdir=./ builddir=./;fi;
	rm -f $(INSTALL_DIR)/bin/iperf 
	rm -rf $(APP)
	rm -f $(PACKAGE_NAME)
	rm -rf $(APP)_beep
	rm -f $(USERSPACE_DL_MODULES_BEEP_DIR)/$(BEEP_TAR_GZ_PREFIX)*$(PACKAGE_NAME)*$(BEEP_TAR_GZ_SUFIX) 
	
shell:
	bash

