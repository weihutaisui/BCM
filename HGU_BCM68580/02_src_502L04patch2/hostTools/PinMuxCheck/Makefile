
vpath %.c ../../shared/opensource/boardparms/bcm963xx
vpath %.h ../../shared/opensource/boardparms/bcm963xx

CFLAGS +=  -I ../../shared/opensource/include/bcm963xx/  -I ../../bcmdrivers/opensource/include/bcm963xx/ -DCONFIG_BCM9$(BRCM_CHIP) -D_BCM9$(BRCM_CHIP)_ -DPINMUXCHECK -DCONFIG_BOARDPARMS_VOICE -D_CFE_ -DPRINT_ERRORS

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_63158_),)
CFLAGS += -DCONFIG_BP_PHYS_INTF
endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6858_4908_6836_63158_6846_6856_),)
CFLAGS += -DCONFIG_CHECK_EXT_INT_GPIO
endif

bptest: bptest.c boardparms.c boardparms_voice.c bp_funcs.c

clean:
	rm -f *.o bptest

