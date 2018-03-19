

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

APP = samba
APP_BEEP = samba_beep
BEEP_PATCH = $(APP_BEEP).patch

.PHONY: check_untar_patch_configure conditional_build check_versions

ifneq ($(strip $(BUILD_SAMBA)),)
conditional_build: $(APP)
else 
ifneq ($(strip $(BUILD_BEEP_SAMBA)),)
conditional_build: $(APP_BEEP)
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif
endif

samba: sanity_check
	(tar xkfz samba.tar.gz 2> /dev/null || true)
	$(MAKE) -C ./samba -j 1

check_patch_file: $(APP_BEEP).patch

samba_beep: sanity_check check_patch_file
	test -s $(APP_BEEP)/lib || patch -s -p0 < $(BEEP_PATCH); chmod 755 $(APP_BEEP)/bin/*.sh;
	(tar xkfz samba.tar.gz 2> /dev/null || true)
	$(MAKE) -C ./samba -j 1
	$(MAKE) -C ./$(APP_BEEP)


clean:
	make -C samba clean
	if [ -f ./$(APP_BEEP)/Makefile ];then $(MAKE) -C ./$(APP_BEEP) clean;fi;
