CFG_64BIT    ?= 0       # cpu in 64 bit mode
CFG_MLONG64  ?= 0       # 64 bit long integer
CFG_LITTLE   ?= 0       # Mindbender
CFG_RELOC    ?= 0
CFG_UNCACHED ?= 0       # Mindbender
CFG_NEWRELOC ?= 0
CFG_VAPI     ?= 0 
CFG_BOOTRAM  ?= 0
CFG_RAMAPP   ?= 1
CFG_BOARDNAME = "BCM9$(BRCM_CHIP)"
CFG_PCI       = 0
CFG_ZLIB      = 0		# enable zlib
CFG_COPY_PSRAM ?= 0
CFG_BOOT_PSRAM ?= 0
CFG_DT         ?= 1
CFG_ATAG       ?= 0
CFG_NONSEC_BOOT ?= 0

COMPRESS ?= cmplzma
ENCRYPTRAM ?= SecureBootUtils/makeEncryptedCfeRamForNor

TOP   = ../../../cfe
ARCH  ?= mips
CPU   ?= bcmcore
BOARD = bcm63xx

# configuration for chips: 63138, 63148
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_63138_63148_47189_6846_),)
CFG_LDR_SREC := 0

ARCH = arm
CPU = armv7
CFG_LITTLE = 1

# enable nonsecure boot only for 6846 for now
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6846__),)
CFG_NONSEC_BOOT = 1
endif

endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6838_6848_),)
CFG_KER_LZ4 = 1
else
CFG_KER_LZ4 = 0
endif

ifeq ($(BRCM_CHIP), 60333)
CFG_LITTLE = 1
endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_4908_6858_6836_63158_6856_),)
CFG_MLONG64 = 1
CFG_ARMV8_AARCH64 = 0
CFG_ARMV8_AARCH32 = 0
CFG_64BIT = 1     # in ARM AArch64 mode
ARCH = arm

ifeq ($(strip $(CFG_64BIT)),1)
CPU = armv8
CFG_ARMV8_AARCH64 = 1

# device tree support. Both mips and arm support device tree in linux
# but currently only used in ARMv8 based chip
else
CPU = armv7       # ARM AArch32 mode use armv7 code.   
CFG_ARMV8_AARCH32 = 1
endif
CFG_LITTLE = 1
CFG_ROM_PRINTF = 0
IKOS_NO_DDRINIT ?= 0
CFG_KER_LZ4 = 0
endif

ifeq ($(strip ${CFG_LDR_SREC}),1)
CFLAGS += -DCFG_LDR_SREC=1
endif

# Configurable cfe ram link address based on the chip and total memory on board
# considering both linux kernel space below this address and image staging buffer
# above this address

# default link address for mips, up to 10MB kernel size
RAMAPP_TEXT = 0x80a01000
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_63138_63148_47189_6846_),)
# ARM based chip with 128MB memory on board, up to 15MB kernel size
RAMAPP_TEXT = 0x00f00000
endif
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_60333_),)
# 6318, 60333 with 8MB memory on board, up to 6MB kernel size
RAMAPP_TEXT = 0x80601000
endif
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_4908_6858_6836_63158_6856_),)
RAMAPP_TEXT = 0x01000000
endif

HOSTTOOLS = ../../../../hostTools
SHARED_DIR = ../../../../shared
BCMDRIVERS_DIR = ../../../../bcmdrivers
RDP_DIR = ../../../../rdp
BOARDPARMS_DIR = $(SHARED_DIR)/opensource/boardparms/bcm963xx
FLASH_DIR = $(SHARED_DIR)/opensource/flash
UTILS_DIR = $(SHARED_DIR)/opensource/utils
SHARED_DRIVERS_DIR = $(SHARED_DIR)/opensource/drivers
BCMSPI_DIR = $(SHARED_DIR)/opensource/spi
INC_SHARED_PUB_PATH = $(SHARED_DIR)/opensource/include/bcm963xx
INC_SHARED_PRIV_PATH = $(SHARED_DIR)/broadcom/include/bcm963xx
ROM_BUILD = ../$(BOARD)_rom
ASM_FILE =$(ROM_BUILD)/flashimg.S

ifeq ($(strip $(CFG_SBP)),1)
	CFLAGS += -DUSE_SINGLE_BOARD_PARAM 
endif

include $(TOP)/board/$(BOARD)_ram/src/ram_cfe.mk

ifneq ($(HND),)
VERSION_MAKE_FILE := version.hnd
else 
ifneq ($(wildcard ../../../../version.make),)
VERSION_MAKE_FILE := version.make
else
VERSION_MAKE_FILE := version.hnd
endif
endif

include ../../../../$(VERSION_MAKE_FILE)
CFLAGS += -DBRCM_VERSION=$(BRCM_VERSION) -DBRCM_RELEASE=$(BRCM_RELEASE) -DBRCM_EXTRAVERSION="\"$(BRCM_EXTRAVERSION)\""

ifeq ($(strip $(CFG_DT)),1)
CFLAGS += -DCFG_DTB_IMAGE="\"9$(BRCM_CHIP).dtb\"" 
endif

# Use VPATH for all source dirs within cfe/, vpath %.filetype for all external source dirs
VPATH += ${TOP}/board/$(BOARD)_ram/src/html 

vpath %.c ${BOARDPARMS_DIR} $(BCMSPI_DIR) ${FLASH_DIR} ${UTILS_DIR} ${SHARED_DRIVERS_DIR} 
INCDIRS += -I${INC_SHARED_PUB_PATH} -I${INC_SHARED_PRIV_PATH}
ifeq ($(strip $(INC_PMC_DRIVER)),1)
INCDIRS += -I$(INC_SHARED_PUB_PATH)/../pmc
ifeq ($(strip $(BRCM_CHIP)),6838)
vpath %.c $(SHARED_DIR)/opensource/pmc/impl2
else
vpath %.c $(SHARED_DIR)/opensource/pmc/impl1
endif
endif

SHA_DIR = ../../../cfe/sha256
AES_DIR = ../../../cfe/aes128
OTP_DIR = ../../../cfe/otp
INCDIRS += -I${SHA_DIR} -I${AES_DIR} -I${OTP_DIR}

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_63158_),)
CFLAGS += -DCONFIG_BP_PHYS_INTF
endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6838_6848_),)
RDP_PROJECT=PON_6838
ifeq ($(strip $(BRCM_CHIP)),6838)
CFLAGS += -DCONFIG_BCM96838 
else
CFLAGS += -DCONFIG_BCM96848
endif

CFLAGS += -D__MIPS_C
ifeq ($(strip $(NONETWORK)),0)
CFLAGS += -DRDD_BASIC -DOREN -D__OREN__ -DLEGACY_RDP --include lib_types.h
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdd
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/bdmf/framework
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdp_subsystem
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdpa
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdpa_gpl
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/dpi
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/phys
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/mdio
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/unimac
INCDIRS += -I$(SHARED_DIR)/opensource/include/rdp
vpath %.c $(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdp_subsystem
vpath %.c $(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdd
vpath %.c $(SHARED_DIR)/opensource/drivers
vpath %.c $(SHARED_DIR)/opensource/drv/dpi
vpath %.c $(SHARED_DIR)/opensource/drv/phys
vpath %.c $(SHARED_DIR)/opensource/drv/mdio
vpath %.c $(SHARED_DIR)/opensource/drv/unimac
endif
endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6838_),)
ifeq ($(strip $(INC_NAND_FLASH_DRIVER)),1)
include ${SHA_DIR}/Makefile
# Single build supports both unsecure and secure boot (no sotp)
VPATH += ${SHA_DIR}
endif
endif


ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_4908_6858_6836_63158_6846_6856_),)
ifeq ($(strip $(INC_NAND_FLASH_DRIVER)),1)
VPATH += ${SHA_DIR} ${OTP_DIR}
include ${OTP_DIR}/Makefile
include ${SHA_DIR}/Makefile
# Single build supports both unsecure and secure boot (has sotp)
else
ifeq ($(strip $(INC_EMMC_FLASH_DRIVER)),1)
# Single build supports both unsecure and secure boot (has sotp)
VPATH += ${SHA_DIR} ${OTP_DIR}
include ${OTP_DIR}/Makefile
include ${SHA_DIR}/Makefile
endif
endif
endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_63138_63148_63381_),)
VPATH += ${SHA_DIR}
VPATH += ${OTP_DIR}
ifneq ($(strip $(INC_BTRM_BOOT)),1)
# nonsecure boot build
SHA_AUTH_ONLY=1
endif
include ${OTP_DIR}/Makefile
include ${SHA_DIR}/Makefile
endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_63268_),)
VPATH += ${SHA_DIR}
ifneq ($(strip $(INC_BTRM_BOOT)),1)
# secure boot build
SHA_AUTH_ONLY=1
# nonsecure boot build
endif
include ${SHA_DIR}/Makefile
endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6838_6848_6858_6836_6846_6856_),)
CFLAGS += -DCONFIG_BCM_PON
export BCM_PON=y
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6858_6836_6846_6856_),)
CFLAGS += -DCONFIG_BCM_PON_XRDP
export BCM_XRDP=y
endif
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6838_6848_),)
CFLAGS += -DCONFIG_BCM_PON_RDP
export BCM_RDP=y
endif
endif

ifeq ($(strip $(NONETWORK)),0)
XRDPDIR = ../../../cfe/xrdp
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6858_6836_6846_6856_),)
ifeq ($(strip $(BRCM_CHIP)),6858)
CFLAGS += -DBCM6858B0
CFLAGS += -DCPU_TX_RING -DCPU_TX_RING_COPY
RDP_PROJECT=XRDP_CFE
RDP_SUBSYS_PROJECT=BCM6858
CFLAGS += -DCONFIG_BCM96858
CFLAGS += -D_CFE_REDUCED_XRDP_
vpath %.c $(SHARED_DIR)/opensource/drv/lport
vpath %.c $(SHARED_DIR)/opensource/drv/lport/ag
INCDIRS += -I$(SHARED_DIR)/opensource/drv/lport
INCDIRS += -I$(SHARED_DIR)/opensource/drv/lport/ag
CFLAGS += -D$(RDP_SUBSYS_PROJECT) -DFIRMWARE_LITTLE_ENDIAN -D_BYTE_ORDER_LITTLE_ENDIAN_ -DPHYS_ADDR_64BIT
ifneq ($(strip $(CONFIG_GPL_RDP_GEN)),1)
# Use GPL RDP by default
CONFIG_GPL_RDP ?= 1
else
CONFIG_GPL_RDP := 0
endif
export CONFIG_GPL_RDP
export CONFIG_GPL_RDP_GEN
endif
ifeq ($(strip $(BRCM_CHIP)),6836)
RDP_PROJECT=PON_6836
CFLAGS += -DCONFIG_BCM96836
RDP_SUBSYS_PROJECT=BCM6836
vpath %.c $(SHARED_DIR)/opensource/drv/mdio
vpath %.c $(SHARED_DIR)/opensource/drv/unimac
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/mdio
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/unimac
CFLAGS += -D$(RDP_SUBSYS_PROJECT) -DFIRMWARE_LITTLE_ENDIAN -D_BYTE_ORDER_LITTLE_ENDIAN_ -DPHYS_ADDR_64BIT
endif
ifeq ($(strip $(BRCM_CHIP)),6846)
RDP_PROJECT=PON_6846
CFLAGS += -DCONFIG_BCM96846
RDP_SUBSYS_PROJECT=BCM6846
vpath %.c $(SHARED_DIR)/opensource/drv/mdio
vpath %.c $(SHARED_DIR)/opensource/drv/unimac
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/mdio
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/unimac
CFLAGS += -D$(RDP_SUBSYS_PROJECT) -DFIRMWARE_LITTLE_ENDIAN -D_BYTE_ORDER_LITTLE_ENDIAN_
endif
ifeq ($(strip $(BRCM_CHIP)),6856)
RDP_PROJECT=PON_6856
CFLAGS += -DCONFIG_BCM96856
RDP_SUBSYS_PROJECT=BCM6856
vpath %.c $(SHARED_DIR)/opensource/drv/mdio
vpath %.c $(SHARED_DIR)/opensource/drv/unimac
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/mdio
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/unimac
CFLAGS += -D$(RDP_SUBSYS_PROJECT) -DFIRMWARE_LITTLE_ENDIAN -D_BYTE_ORDER_LITTLE_ENDIAN_ -DPHYS_ADDR_64BIT
endif

ifeq ("$(CONFIG_GPL_RDP_GEN)", "1")
CFLAGS += -DCONFIG_GPL_RDP_GEN
endif

ifeq ("$(CONFIG_GPL_RDP)", "1")
CFLAGS += -DCONFIG_GPL_RDP
endif

export RDP_PROJECT
export RDP_SUBSYS_PROJECT

vpath %.c $(SHARED_DIR)/opensource/drv/phys
INCDIRS += -I$(SHARED_DIR)/opensource/include/rdp
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/phys
ifneq ("$(CONFIG_GPL_RDP)", "1")
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdpa_gpl
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/bdmf/framework
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/bdmf/system
endif
INCDIRS += -I$(XRDPDIR)
XRDP_TARGET = libxrdp
XRDP_LINK = -L$(XRDPDIR) -lxrdp

endif
endif
# configuration for chips: 63138, 63148
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_63138_63148_),)
CFLAGS += -DCONFIG_BCM9$(BRCM_CHIP)
ifeq ($(strip $(NONETWORK)),0)
CFLAGS += -DLEGACY_RDP --include lib_types.h
ifeq ($(strip $(BRCM_CHIP)),63148)
CFLAGS += -DRDD_BASIC -DDSL_63148
RDP_PROJECT=DSL_63148
else
CFLAGS += -DRDD_BASIC -DDSL_63138 
RDP_PROJECT=DSL_63138
endif
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdd
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/bdmf/framework
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/bdmf/system
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdp_subsystem
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdpa
INCDIRS += -I$(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdpa_gpl
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/dpi
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/phys
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/mdio
INCDIRS += -I$(SHARED_DIR)/opensource/include/drv/unimac
INCDIRS += -I$(SHARED_DIR)/opensource/include/rdp
vpath %.c $(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdp_subsystem
vpath %.c $(RDP_DIR)/projects/$(RDP_PROJECT)/target/rdd
vpath %.c $(SHARED_DIR)/opensource/drv/dpi
vpath %.c $(SHARED_DIR)/opensource/drv/phys
vpath %.c $(SHARED_DIR)/opensource/drv/mdio
vpath %.c $(SHARED_DIR)/opensource/drv/unimac
endif
endif

ifeq ($(strip $(NONETWORK)),0)
rdp_link:
ifneq ($(strip $(RDP_PROJECT)),)
	$(MAKE) -C $(RDP_DIR) PROJECT=$(RDP_PROJECT) rdp_link
endif

rdp_clean: libxrdp_clean
ifneq ($(strip $(RDP_PROJECT)),)
	$(MAKE) -C $(RDP_DIR) PROJECT=$(RDP_PROJECT) clean
else
	$(MAKE) -C $(RDP_DIR) ALL=clean
endif
endif

cmplzma_tool:
	$(MAKE) -C $(HOSTTOOLS) build_cmplzma

secbt_tools:
ifeq ($(strip $(INC_BTRM_BOOT)),1)
	$(MAKE) -C $(HOSTTOOLS) build_secbtutils
endif

# support rdp target except the chips in the list
ifeq ($(findstring _$(strip $(BRCM_CHIP))_,_60333_63268_63381_47189_),)
ifeq ($(strip $(NONETWORK)),0)
ALL: sanity_check cmplzma_tool secbt_tools rdp_link $(XRDP_TARGET)
else
ALL: sanity_check cmplzma_tool secbt_tools
endif
else
ALL: sanity_check cmplzma_tool secbt_tools
endif
	find $(SHARED_DIR) -name "*.o" -exec rm -f "{}" ";"
	find $(BOARDPARMS_DIR) -name "*.o" -exec rm -f "{}" ";"
	${MAKE} cfe$(BRCM_CHIP)
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_63138_63148_4908_6858_47189_6836_63158_6846_6856_),)
ifeq ($(strip $(INC_BTRM_BOOT)),1)
	make -C $(TOP)/board/$(BOARD)_btrm/tools/src
	$(HOSTTOOLS)/$(COMPRESS) -c -2 -lzma cfe$(BRCM_CHIP) cfe$(BRCM_CHIP).bin $(ASM_FILE) -e "$(HOSTTOOLS)/$(ENCRYPTRAM) $(BRCM_CHIP) cfe$(BRCM_CHIP).bin.lzma cfe$(BRCM_CHIP).bin.lzma $(TOP)/board/$(BOARD)_btrm"
else
	$(HOSTTOOLS)/$(COMPRESS) -c -2 -lzma cfe$(BRCM_CHIP) cfe$(BRCM_CHIP).bin $(ASM_FILE)
endif
else
ifeq ($(strip $(INC_BTRM_BOOT)),1)
	make -C $(TOP)/board/$(BOARD)_btrm/tools/src
	$(HOSTTOOLS)/$(COMPRESS) -c -2 -lzma cfe$(BRCM_CHIP) cfe$(BRCM_CHIP).bin $(ASM_FILE) -e "$(HOSTTOOLS)/$(ENCRYPTRAM) $(BRCM_CHIP) cfe$(BRCM_CHIP).bin.lzma cfe$(BRCM_CHIP).bin.lzma $(TOP)/board/$(BOARD)_btrm"
else
	$(HOSTTOOLS)/$(COMPRESS) -s -c -2 -lzma cfe$(BRCM_CHIP) cfe$(BRCM_CHIP).bin $(ASM_FILE)
endif
endif

sanity_check:
	@if [ "$(BRCM_CHIP)" = "" ]; then \
		echo You must specify chip ID using 'gmake BRCM_CHIP=[60333|63268|63138|6838|63148|6848|4908|6858|6836|63158|6846|6856]'; exit 1; \
	fi

export GCC
BASEFLAGS := $(CFLAGS)
export BASEFLAGS

libxrdp:
	$(MAKE) -C $(XRDPDIR) cfe

libxrdp_clean:
	$(MAKE) -C $(XRDPDIR) clean

$(ASM_FILE): cfe$(BRCM_CHIP) cfe$(BRCM_CHIP).bin

cfe$(BRCM_CHIP) cfe$(BRCM_CHIP).bin: $(CRT0OBJS) $(BSPOBJS) $(LIBCFE) 
	$(GLD) -o cfe$(BRCM_CHIP) -Map cfe$(BRCM_CHIP).map $(LDFLAGS) $(CRT0OBJS) $(BSPOBJS) $(XRDP_LINK) -L. -lcfe $(LDLIBS) 
	$(OBJDUMP) -d cfe$(BRCM_CHIP) > cfe$(BRCM_CHIP).dis
	$(OBJCOPY) --output-target=binary cfe$(BRCM_CHIP) cfe$(BRCM_CHIP).bin
	$(OBJCOPY) --output-target=srec --srec-forceS3 -R .reginfo -R .note -R .comment -R .mdebug -S cfe$(BRCM_CHIP) cfe$(BRCM_CHIP).srec

ifeq ($(strip $(INC_NAND_FLASH_DRIVER)),1)
	$(OBJDUMP) -x cfe$(BRCM_CHIP) | $(HOSTTOOLS)/mkCfeHeader  cferamhdr.bin
	cat cferamhdr.bin cfe$(BRCM_CHIP).bin > cfe$(BRCM_CHIP)ram.bin
	rm cferamhdr.bin
else
ifeq ($(strip $(INC_EMMC_FLASH_DRIVER)),1)
	$(OBJDUMP) -x cfe$(BRCM_CHIP) | $(HOSTTOOLS)/mkCfeHeader  cferamhdr.bin
	cat cferamhdr.bin cfe$(BRCM_CHIP).bin > cfe$(BRCM_CHIP)ram.bin
	rm cferamhdr.bin
endif
endif

ifeq ($(strip $(NONETWORK)),0)
clean : rdp_clean
else
clean :
endif
	rm -f *.o *~ cfe*
	#rm -f build_date.c
	rm -f libcfe.a
	rm -f $(ASM_FILE)
distclean : clean




