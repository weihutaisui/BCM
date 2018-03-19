#
# Makefile for the optical wan detection
#

obj-${CONFIG_BCM_OPTICALDET} += opticaldet.o
opticaldet-objs += detect_dev.o
opticaldet-objs += detect_dev_trx_data.o
opticaldet-objs += trx_activation.o

EXTRA_CFLAGS += -I$(INC_BRCMSHARED_PUB_PATH)/drv/mdio
EXTRA_CFLAGS += -I$(INC_BRCMSHARED_PUB_PATH)/drv/phys

EXTRA_CFLAGS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)
EXTRA_CFLAGS += -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD)
EXTRA_CFLAGS += -I$(INC_BRCMBOARDPARMS_PATH)/$(BRCM_BOARD)
EXTRA_CFLAGS += -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)
EXTRA_CFLAGS += -I$(INC_BRCMSHARED_PUB_PATH)/drv/mdio
EXTRA_CFLAGS += -I$(src)

ifeq ("$(CONFIG_BCM96838)" ,"y")
opticaldet-objs += detect_opt.o
CONFIG_PLATFORM=oren
EXTRA_CFLAGS += -I$(INC_BRCMSHARED_PUB_PATH)/drv/egphy
EXTRA_CFLAGS += -I$(INC_BRCMSHARED_PUB_PATH)/drv
endif

EXTRA_CFLAGS += $(INC_RDP_FLAGS) -Werror -Wall

-include $(TOPDIR)/Rules.make

clean:
	rm -f *.[ao] *.ko *.cmd

