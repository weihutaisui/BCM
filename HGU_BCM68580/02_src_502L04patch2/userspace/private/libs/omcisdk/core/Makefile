#GLOBAL_RELEASE_SCRIPT_NOTOUCH

ifneq ($(wildcard Makefile.fullsrc),)

include Makefile.fullsrc

else

LIB = libomci_core.so

.PHONY: $(LIB)

default: dynamic

dynamic: generic_private_lib_install

libomci_core.so:
	if [ -a $(LIB)_$(PROFILE).save ]; then cp $(LIB)_$(PROFILE).save $(LIB); fi;

clean:
	rm -f $(LIB)

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
