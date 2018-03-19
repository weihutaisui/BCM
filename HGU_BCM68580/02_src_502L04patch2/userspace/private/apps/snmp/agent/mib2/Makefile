ifneq ($(strip $(BUILD_XDSLCTL)),)
OBJS = snmpv2Mib.o  ifMib.o RMON.o qBridge.o pBridge.o adsl/vdslMib.o adsl/vdslMibHelper.o
else
OBJS = snmpv2Mib.o  ifMib.o RMON.o qBridge.o pBridge.o 
endif

mib2.o: $(OBJS)
	$(LD) $(EXTRA_LDFLAGS) -r -o $@ $(OBJS)

%.o: %.c Makefile
	$(CC) $(BCM_LD_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c -o $@ $<

clean:
	rm -f $(OBJS) mib2.o
	rm -f *.o *.d *.d.* adsl/*.o adsl/*.d adsl/*.d.*

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
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR) \
                         -I$(BUILD_DIR)/bcmdrivers/opensource/include/bcm963xx \
                         -I$(BUILD_DIR)/bcmdrivers/broadcom/include/bcm963xx \
                         -I$(BUILD_DIR)/userspace/private/apps/httpd \
                         -I$(BUILD_DIR)/shared/opensource/include/bcm963xx  \
                         -I$(KERNEL_LINKS_DIR)

#LIBS = -lcms_msg -lcms_util -lcms_dal -lcms_qdm -lxdslctl -latmctl -lbcm_boardctl $(CMS_COMMON_LIBS) -ldl $(CMS_CORE_LIBS)

INC_PATH = -I$(BUILD_DIR)/userspace/private/apps/snmp/engine/ -I$(BUILD_DIR)/userspace/private/apps/snmp/agent/include 
CFLAGS += $(INC_PATH) $(ALLOWED_INCLUDE_PATHS) $(CMS_LOG_FLAGS) 
#$(CMS_COMPILE_FLAGS)

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

ifneq ($(strip $(BUILD_XDSLCTL)),)
ifeq ($(strip $(SNMP_ATM_MIB)), 1)
CFLAGS+= -DBUILD_SNMP_ATM_MIB
endif
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

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)
