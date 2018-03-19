#
# In most cases, you only need to modify this first section.
#
EXE  = relayctl
OBJS = relayctl.o 

all dynamic install: build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

ifneq ($(strip $(BUILD_TST24_UTILS)),)
build: $(EXE) generic_exe_install
else
build:
	@echo "Skipping tst24 relayctlapplication (not configured)"
endif

clean: generic_clean
	rm -f $(INSTALL_DIR)/bin/$(EXE)

# Shell target permits creating an interacive shell for debugging
shell:
	bash -i



