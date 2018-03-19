#
# In most cases, you only need to modify this first section.
#
EXE  = spictl
OBJS = spictl.o

all dynamic install: build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

ALLOWED_INCLUDE_PATHS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)  \
                         -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
                         -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)  \
                         -I$(BUILD_DIR)/shared/broadcom/include/$(BRCM_BOARD) \
                         -I$(BUILD_DIR)/shared/opensource/include/$(BRCM_BOARD) \
		                 -I$(BUILD_DIR)/userspace/public/include     \
			             -I$(BUILD_DIR)/userspace/private/include  			 
ifneq ($(strip $(BUILD_SPICTL)),)
build: $(EXE) generic_exe_install
else
ifeq ($(strip $(BUILD_6802_MOCA)),y)
build: $(EXE) generic_exe_install
else
build:
	@echo "Skipping spictl application (not configured)"
endif	
endif				     

clean: generic_clean
	rm -f $(INSTALL_DIR)/bin/$(EXE)

# Shell target permits creating an interactive shell for debugging
shell:
	bash -i

$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS)
