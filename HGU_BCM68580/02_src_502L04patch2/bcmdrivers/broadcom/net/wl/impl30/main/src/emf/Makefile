WLEMF_LKM_NAME	:=wlemf
obj-m	+=$(WLEMF_LKM_NAME).o
include $(WLCFGDIR)/$(DEFAULT_CONFIG)
include $(WLCFGDIR)/wl.mk
COMMON_SHARED_DIR := ../../../../shared/impl1

ifeq ($(strip $(SOURCE_AVAIL)), 1)
include $(WLCFGDIR)/../../epivers
export DSLCPE_WLAN_VERSION = $(EPI_VERSION_STR)
EXTRA_CFLAGS += -DDSLCPE_WLAN_VERSION=\"$(DSLCPE_WLAN_VERSION)\" -DDSLCPE_EMF_BUILD  -DBCM_NBUFF -I$(COMMON_SHARED_DIR)
endif
EMF_BASE:=emf
IGS_BASE:=igs
SHARED_BASE:=../shared

WLEMF_PUB_FILES:=../router/emf/emf/emf_linux.c 
WLEMF_PUB_OBJS :=$(WLEMF_PUB_FILES:.c=.o)
WLEMF_SHARED_FILES := $(IGS_BASE)/osl_linux.c $(SHARED_BASE)/linux_osl.c $(SHARED_BASE)/nvramstubs.c $(SHARED_BASE)/bcmutils.c
WLEMF_SHARED_OBJS :=$(WLEMF_SHARED_FILES:.c=.o)

WLEMF_EMF_FILES := $(EMF_BASE)/emfc.c $(EMF_BASE)/emfc_ipv6.c 
WLEMF_EMF_OBJS :=$(WLEMF_EMF_FILES:.c=.o)
WLEMF_IGS_FILES := $(IGS_BASE)/igsc.c $(IGS_BASE)/igsc_ipv6.c $(IGS_BASE)/igsc_sdb.c $(IGS_BASE)/igsc_sdb_ipv6.c 
WLEMF_IGS_OBJS :=$(WLEMF_IGS_FILES:.c=.o)

$(WLEMF_LKM_NAME)-objs :=$(WLEMF_PUB_OBJS)
#$(WLEMF_LKM_NAME)-objs +=$(WLEMF_EMF_OBJS)
#$(WLEMF_LKM_NAME)-objs +=$(WLEMF_IGS_OBJS)
#$(WLEMF_LKM_NAME)-objs +=$(WLEMF_SHARED_OBJS)
$(WLEMF_LKM_NAME)-objs +=wlemflib.o

$(obj)/wlemflib.o:build_lib
ifeq ($(strip $(SOURCE_AVAIL)), 1)
	$(shell cp  $(obj)/wlemflib.o $(obj)/wlemflib-$(BRCM_CHIP)_$(BCMARCH).o_save)

build_lib: $(addprefix $(obj)/, $(WLEMF_EMF_OBJS)) $(addprefix $(obj)/,$(WLEMF_IGS_OBJS)) $(addprefix $(obj)/,$(WLEMF_SHARED_OBJS))
	$(LD) $(ld_flags) $(LDFLAGS_MODULE)  -r -o  $(obj)/wlemflib.o $^
else
build_lib:
	$(shell cp $(obj)/wlemflib-$(BRCM_CHIP)_$(BCMARCH).o_save $(obj)/wlemflib.o)
endif
