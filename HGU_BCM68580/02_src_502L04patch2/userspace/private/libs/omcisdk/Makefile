#
# In most cases, you only need to modify this first section.
#

#GLOBAL_RELEASE_SCRIPT_NOTOUCH

.PHONY: all

DIR_LIST = core rutcms pm pm_drv

all install: conditional_build
clean: conditional_clean

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

ifneq ($(strip $(BUILD_OMCI)),)
conditional_build: omcisdk_all

omcisdk_all:
	set -e; for dir in ${DIR_LIST}; do ${MAKE} -C $$dir; done
conditional_clean:
	set -e; for dir in ${DIR_LIST}; do ${MAKE} -C $$dir clean; done
else
conditional_build:
	@echo "omcisdk skipped (BUILD_OMCI not configured)"
conditional_clean:
	@echo "omcisdk skipped (BUILD_OMCI not configured)"
endif

binaryonly_dist_clean:
	set -e; for dir in ${DIR_LIST}; do ${MAKE} -C $$dir binaryonly_dist_cleanx; done
