OBJS = enterpriseMain.o

ifeq ($(strip $(BRCM_PKTCBL_SUPPORT)),y)
OBJS += cable/clPktc.o               \
        cable/clPktcEUEDevMib.o      \
        cable/clPktcEUEUserMib.o     \
        cable/clPktcEUEProvMgmtMib.o \
        cable/clPktcEUEEventMib.o    \
        cable/clPktcIetfMtaMib.o     \
        cable/clPktcUtils.o
endif

ifeq ($(strip $(BUILD_SNMP_CHINA_TELECOM_CPE_MIB)),y)
ifneq ($(strip $(BUILD_XDSLCTL)),)
OBJS += china/cpeDslMib.o china/cpeDslMibHelper.o 
endif
endif

ifeq ($(strip $(BUILD_SNMP_BRCM_CPE_MIB)),y)
OBJS += brcm/brcmCpeMib.o brcm/brcmCpeMibHelper.o
endif

enterprise.o: $(OBJS)
	$(LD) $(EXTRA_LDFLAGS) -r -o $@ $(OBJS)

%.o: %.c Makefile
	$(CC) $(BCM_LD_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c -o $@ $<

clean:
	rm -f $(OBJS) enterprise.o

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

ALLOWED_INCLUDE_PATHS := -I.\
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR)

LIBS = -lcms_msg -lcms_util -lbcm_flashutil -lbcm_crc -lcms_dal -lxdslctl -latmctl -lbcm_boardctl $(CMS_COMMON_LIBS) -ldl $(CMS_CORE_LIBS)

INC_PATH = -I$(BUILD_DIR)/userspace/private/apps/snmp/engine/ -I$(BUILD_DIR)/userspace/private/apps/snmp/agent/include 
CFLAGS += $(INC_PATH) $(ALLOWED_INCLUDE_PATHS) $(CMS_LOG_FLAGS)

ifneq ($(strip $(ADSL_PLN_TEST)),)
        CFLAGS += -DADSL_MIBOBJ_PLN
endif

ifeq ($(strip $(SNMP_SNMP_MIB)), 1)
CFLAGS+= -DBUILD_SNMP_MIB
endif

ifneq ($(strip $(BUILD_SNMP_SET)),)
CFLAGS+= -DBUILD_SNMP_SET
endif

ifneq ($(strip $(BUILD_XDSLCTL)),)
ifeq ($(strip $(SNMP_ADSL_MIB)), 1)
CFLAGS+= -DBUILD_SNMP_ADSL_MIB
endif
endif

ifeq ($(strip $(SNMP_ATM_MIB)), 1)
CFLAGS+= -DBUILD_SNMP_ATM_MIB
endif

ifeq ($(strip $(SNMP_BRIDGE_MIB)), 1)
CFLAGS+= -DBUILD_SNMP_BRIDGE_MIB
endif

ifneq ($(strip $(BUILD_XDSLCTL)),)
ifeq ($(strip $(BUILD_SNMP_CHINA_TELECOM_CPE_MIB)),y)
CFLAGS += -DCPE_DSL_MIB
endif
endif

ifeq ($(strip $(BUILD_SNMP_DEBUG)), y)
CFLAGS += -DBUILD_SNMP_DEBUG
endif

ifeq ($(strip $(BUILD_SNMP_CHINA_TELECOM_CPE_MIB)),y)
ifneq ($(strip $(BUILD_XDSLCTL)),)
CFLAGS += -DCPE_DSL_MIB
endif
ifeq ($(strip $(BUILD_SNMP_CHINA_TELECOM_CPE_MIB_V2)),y)
CFLAGS += -DCT_SNMP_MIB_VERSION_2
endif
endif

ifeq ($(strip $(BUILD_SNMP_BRCM_CPE_MIB)),y)
CFLAGS += -DBRCM_CPE_MIB
endif

ifeq ($(strip $(BRCM_PKTCBL_SUPPORT)),y)
CFLAGS += -DBRCM_PKTCBL_SUPPORT
endif

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)

