BSPOBJS += bcm63xx_potp_sotp_util.o

ifeq ($(findstring _$(strip $(BRCM_CHIP))_,_63148_63381_),)
BSPOBJS += bcm63xx_sotp_if.o
endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_63138_63148_63381_),)
BSPOBJS +=           \
	bcm63xx_potp_sotp_cmd_impl1.o
else
BSPOBJS +=           \
	bcm63xx_potp_sotp_cmd.o
endif
