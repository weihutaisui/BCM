#***********************************************************************
#
#  Copyright (c) 2006-20012  Broadcom Corporation
#  All Rights Reserved
#
#***********************************************************************/

#
# In most cases, you only need to modify this first section.
#
LIB = libcms_dal.so

OBJS = dal_main.o dal2_main.o \
       dal_wan.o dal_cellular.o dal2_cellular.o \
       dal_lan.o dal2_lan.o dal2_bridging.o \
       dal_auth.o dal_route.o \
       dal_virtualserver.o dal_parentalcntrl.o dal_urlfilter.o \
       dal_sec.o dal2_sec.o  \
       dal_qos.o dal2_qos.o \
       dal_pmap.o  dal_cert.o dal_ethswitch.o dal_ddns.o dal_network.o dal_iptunnel.o \
       dal_pmirror.o dal2_pmirror.o \
       dal_diag.o dal_dnsproxy.o \
       dal_dns.o dal2_dns.o dal2_dns6.o \
       dal_eth.o dal2_eth.o dal_dsl.o dal2_dsl.o \
       dal_moca.o dal2_moca.o \
       dal_porttriggering.o dal_ipsec.o dal_pwrmngt.o dal_standby.o \
       dal_multicast.o dal_l2tpac.o dal_storageservice.o dal_pptpac.o dal_autodetection.o \
       dal_modsw_ee.o \
       dal_gponwan.o dal_eponwan.o dal_bmu.o \
       dal_wifiwan.o dal_homeplug.o dal2_wifiwan.o \
       dal2_wan.o dal2_wan6.o dal_lan6.o dal2_lan6.o \
       dal2_ip.o dal2_ip6.o dal2_ppp.o dal2_ethernet.o dal2_nat.o dal2_route.o dal2_ipsec.o \
       dal2_dhcp.o dal2_iptunnel.o dal2_diag.o dal2_virtualserver.o dal2_ieee1905.o dal2_porttriggering.o \
       dal2_pcp.o dal_nfc.o dal_stun.o

all install: generic_private_lib_install

clean: generic_clean
	rm -f $(INSTALL_DIR)/lib/private/$(LIB)



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

ifneq ($(strip $(BRCM_VOICE_SUPPORT)),)
include $(BUILD_DIR)/make.voice
endif

ifeq ($(strip $(BUILD_SPDSVC)), y)
OBJS += dal_spdsvc.o
endif

ifneq ($(strip $(BUILD_OPENVSWITCH)),)
OBJS += dal_openVS.o
endif

ifneq ($(strip $(BUILD_DM_TR104_V2)),)
    OBJS += dal_voice.o dal2_voice.o dal2_dect.o
else
    OBJS += dal_voice.o dal2_voice.o dal_dect.o
endif



CFLAGS += $(SSP_TYP_COMPILER_OPTS)
BCM_LD_FLAGS += -L$(INSTALL_DIR)/lib $(SSP_TYP_LIBS)


#
# Private apps and libs are allowed to include header files from the
# private and public directories
#
# WARNING: Do not modify this section unless you understand the
# license implications of what you are doing.
#
ALLOWED_INCLUDE_PATHS := -I.\
                         -I$(BUILD_DIR)/userspace/public/include  \
                         -I$(BUILD_DIR)/userspace/public/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/include  \
                         -I$(BUILD_DIR)/userspace/private/include/$(OALDIR) \
                         -I$(BUILD_DIR)/userspace/private/libs/cms_core \
                         -I$(BUILD_DIR)/userspace/private/libs/cms_core/linux  \
                         -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)  \
                         -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
                         -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)


# dal_wan.c includes adslctlapi.h, which in turn needs kernel header files
ALLOWED_INCLUDE_PATHS += -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD)

# treat all warnings as errors
CUSTOM_CFLAGS += -Werror -Wfatal-errors # NEW_FORBID_WARNINGS

#
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
#
$(LIB): $(OBJS)
	$(CC) -shared $(BCM_LD_FLAGS) -Wl,-soname,$(notdir $@) -o $@ $(OBJS)



#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)
