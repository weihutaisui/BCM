# File: bcmdrivers/opensource/char/tzdrv/bcm_astra/impl1/Makefile
#
# Makefile for the bcm_astra driver
#
obj-m += bcm_astra.o

astra_subdir:=$(BUILD_DIR)/secureos/astra

bcm_astra-objs += \
	tzioc_drv.o \
	tzioc_api.o \
	tzioc_ioctl.o \
	tzioc_msg.o \
	tzioc_mem.o \
	tzioc_client.o \
	tzioc_peer.o \
	tzioc_file.o \
	tzioc_coredump.o \
	astra_drv.o \
	astra_api.o \
	astra_ioctl.o

bcm_astra-objs += \
	../../../../../../secureos/astra/common/src/tzioc_common/tzioc_common.o \
	../../../../../../secureos/astra/common/src/tzioc_common/tzioc_common_msg.o \
	../../../../../../secureos/astra/common/src/tzioc_common/tzioc_common_ring.o \
	../../../../../../secureos/astra/common/src/tzioc_common/tzioc_common_mem.o \
	../../../../../../secureos/astra/common/src/tzioc_common/tzioc_common_client.o

#bcm_astra-objs += \
#	tracelog/tracelog.o

#bcm_astra-objs += \
#	vuart/vuart.o

EXTRA_CFLAGS += \
	-I$(src) \
	-I$(src)/tracelog \
	-I$(src)/vuart \
	-I$(astra_subdir)/common/include \
	-I$(astra_subdir)/common/src/tzioc_common

ifneq ($(VIRTUAL_UART_USE_TTY),n)
EXTRA_CFLAGS += -DVUART_USE_TTY
endif
