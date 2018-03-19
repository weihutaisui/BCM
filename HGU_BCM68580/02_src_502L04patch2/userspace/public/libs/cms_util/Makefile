#***********************************************************************
#
#  Copyright (c) 2007  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/


#
# In most cases, you only need to modify this first section.
#
LIB  = libcms_util.so

OBJS := eid.o logging.o memory.o bget.o psp.o timestamp.o timer.o \
       eid_parser.o strconv.o prctl.o cal_log.o \
       assert.o network.o file.o base64decode.o base64encode.o \
       lzw_encode.o lzw_decode.o hexbinary.o xml.o led.o passwd.o \
       image.o image_modsw_linux.o \
       tokenizer.o errorcodes.o \
       ftrace.o linklist.o random.o unicode.o \
       pcp.o data_model_selector.o \
       strconv2.o math.o md5.o
       
# if you want to use CMS_MEM_LEAK_TRACING feature on ARM based platforms,
# uncomment out the following line.  But to avoid complicated license
# issues, do NOT compile this file into your production code.
# OBJS += backtrace_arm.o

all: sanity_check generic_public_lib_install scripts/symbol_table.txt
	mkdir -p $(INSTALL_DIR)/etc/cms_entity_info.d
	install scripts/eid*.txt scripts/symbol_table.txt $(INSTALL_DIR)/etc/cms_entity_info.d

clean: generic_oalclean generic_clean 
	rm -f $(INSTALL_DIR)/lib/public/$(LIB)
	rm -f $(INSTALL_DIR)/etc/cms_entity_info.d/eid*.txt
	rm -f $(INSTALL_DIR)/etc/cms_entity_info.d/symbol_table.txt
	rm -f scripts/symbol_table.txt



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



# grab some #defines from cms_eid.h and cms_params.h so that symbols can
# be used in the eid files
scripts/symbol_table.txt: $(BUILD_DIR)/userspace/public/include/cms_eid.h $(BUILD_DIR)/userspace/public/include/cms_params.h $(KERNEL_DIR)/include/linux/bcm_realtime.h
	@echo Generating $@
	@egrep 'EID_[[:alnum:]_-]+=' $(BUILD_DIR)/userspace/public/include/cms_eid.h | sed -e 's/,//g' | sed -e 's/=/ /g' > $@
	@egrep 'NDA_ACCESS_[[:alnum:]_\t\ ]+0x' $(BUILD_DIR)/userspace/public/include/cms_eid.h | sed -e 's/#define//g' >> $@
	@grep '#define EIF_' $(BUILD_DIR)/userspace/public/include/cms_eid.h | sed -e 's/#define//g' >> $@
	@grep '#define FTPD_PORT' $(BUILD_DIR)/userspace/public/include/cms_params.h | sed -e 's/#define//g' >> $@
	@grep '#define TFTPD_PORT' $(BUILD_DIR)/userspace/public/include/cms_params.h | sed -e 's/#define//g' >> $@
	@grep '#define SSHD_PORT' $(BUILD_DIR)/userspace/public/include/cms_params.h | sed -e 's/#define//g' >> $@
	@grep '#define TELNETD_PORT' $(BUILD_DIR)/userspace/public/include/cms_params.h | sed -e 's/#define//g' >> $@
	@grep '#define HTTPD_PORT' $(BUILD_DIR)/userspace/public/include/cms_params.h | sed -e 's/#define//g' >> $@
	@grep '#define SNMPD_PORT' $(BUILD_DIR)/userspace/public/include/cms_params.h | sed -e 's/#define//g' >> $@
	@grep '#define TR69C_CONN_REQ_PORT' $(BUILD_DIR)/userspace/public/include/cms_params.h | sed -e 's/#define//g' >> $@
	@grep '#define TR64C_HTTP_CONN_PORT' $(BUILD_DIR)/userspace/public/include/cms_params.h | sed -e 's/#define//g' >> $@
	@grep '#define TR69C_CONN_REQ_PORT' $(BUILD_DIR)/userspace/public/include/cms_params.h | sed -e 's/#define//g' >> $@
	@egrep '#define SCHED_[NFR]' $(KERNEL_DIR)/include/linux/sched.h | sed -e 's/#define//g' >> $@
ifneq ($(PROFILE_KERNEL_VER),LINUX_3_4_0_RT)
	@egrep '#define SCHED_[NFR]' $(KERNEL_DIR)/include/uapi/linux/sched.h | sed -e 's/#define//g' >> $@
endif
	@grep '#define BCM_RTPRIO_' $(KERNEL_DIR)/include/linux/bcm_realtime.h | sed -e 's/#define//g' >> $@


#
# Public libs are only allowed to include header files from the
# public directory.
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I.\
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BCM_FSBUILD_DIR)/public/include \
                         -I$(CURR_DIR)/../bcm_util \
                         -I$(CURR_DIR)/../bcm_flashutil

# psp.c needs to do direct ioctl to the board to write to flash
ALLOWED_INCLUDE_PATHS += -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)  \
                         -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)  \
                         -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) 


# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors

#needed by image.c
CFLAGS += -DBRCM_CHIP_HEX=0x$(BRCM_CHIP)


#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 
$(LIB): generic_oaldir $(OBJS)
	$(CC) -shared $(BCM_LD_FLAGS) -Wl,--whole-archive,-soname,$(notdir $@) -o $@ \
	      $(OBJS) $(OALDIR)/oal.a $(MODSW_CONTRIB_LIBS) \
	      -Wl,-lcrypt -lm -Wl,--no-whole-archive
	@echo "cms_util: done building $@"



#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)
