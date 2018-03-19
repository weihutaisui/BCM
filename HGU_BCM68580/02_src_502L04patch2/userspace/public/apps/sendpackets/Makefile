
EXE := sendpackets

all install: conditional_build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))


include $(BUILD_DIR)/make.common

ifneq ($(strip $(BUILD_SENDPACKETS)),)
conditional_build: sanity_check generic_exe_install

else
conditional_build: sanity_check
	@echo "Skipping $(EXE) (not configured)"
endif


sendpackets: sendpackets.c
	$(CC) -L$(BCM_FSBUILD_DIR)/public/lib -I$(BCM_FSBUILD_DIR)/public/include  sendpackets.c -lpcap  -o sendpackets


clean: generic_clean
	rm -f $(INSTALL_DIR)/bin/$(EXE)



