

#GLOBAL_RELEASE_SCRIPT_NOTOUCH


ifneq ($(wildcard Makefile.fullsrc),)

include Makefile.fullsrc

else

EXE = eponctl
dynamic: generic_exe_install

clean:
	rm -f $(EXE)

eponctl:
	cp $(EXE)_$(PROFILE).save $(EXE)

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
#

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common
endif

ALLOWED_INCLUDE_PATHS := -I.\
						 -I$(BUILD_DIR)/userspace/public/include  \
						 -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
						 -I$(BUILD_DIR)/userspace/private/include  \
						 -I$(BUILD_DIR)/userspace/private/include/$(OALDIR) \
						 -I$(BUILD_DIR)/userspace/private/libs/eponsdk/msgprocess/h \
						 -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
						 -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) \
						 $(INC_RDP_FLAGS) 


