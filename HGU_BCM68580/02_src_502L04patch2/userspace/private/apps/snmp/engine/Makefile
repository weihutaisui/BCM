
OBJS = agtEngine.o agtMib.o asn1.o snmpCommunity.o agtTransport.o
LIBS = $(CMS_COMMON_LIBS) -ldl $(CMS_CORE_LIBS)

engine.o: $(OBJS)
	$(LD) $(EXTRA_LDFLAGS) -r -o $@ $(OBJS)

%.o: %.c Makefile
	$(CC) $(BCM_LD_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c -o $@ $<

clean:
	rm -f $(OBJS) engine.o

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

ifneq ($(strip $(BUILD_XDSLCTL)),)
ifeq ($(strip $(BUILD_SNMP_CHINA_TELECOM_CPE_MIB)),y)
CFLAGS += -DCPE_DSL_MIB
endif
endif

ifeq ($(strip $(BUILD_SNMP_UDP)), y)
CFLAGS += -DBUILD_SNMP_UDP
endif

ifeq ($(strip $(BUILD_SNMP_EOC)), y)
CFLAGS += -DBUILD_SNMP_EOC
endif

ifeq ($(strip $(BUILD_SNMP_AAL5)), y)
CFLAGS += -DBUILD_SNMP_AAL5
endif

ifeq ($(strip $(BUILD_SNMP_AUTO)), y)
CFLAGS += -DBUILD_SNMP_AUTO
endif

ifeq ($(strip $(BUILD_SNMP_DEBUG)), y)
CFLAGS += -DBUILD_SNMP_DEBUG
endif

ifeq ($(strip $(BUILD_SNMP_TRANSPORT_DEBUG)), y)
CFLAGS += -DBUILD_SNMP_TRANSPORT_DEBUG
endif

ifeq ($(strip $(BUILD_SNMP_LAYER_DEBUG)), y)
CFLAGS += -DBUILD_SNMP_LAYER_DEBUG
endif

ALLOWED_INCLUDE_PATHS := -I.\
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR)

INC_PATH = -I$(BUILD_DIR)/userspace/private/apps/snmp/engine/ -I$(BUILD_DIR)/userspace/private/apps/snmp/agent/include 
CFLAGS += $(INC_PATH) $(ALLOWED_INCLUDE_PATHS) $(CMS_LOG_FLAGS)

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)
 
