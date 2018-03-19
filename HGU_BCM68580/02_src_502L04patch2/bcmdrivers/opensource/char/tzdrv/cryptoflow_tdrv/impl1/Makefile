# File: bcmdrivers/opensource/char/tzdrv/cryptoflow_tdrv/impl1/Makefile
#
# Makefile for the cryptoflow_tdrv driver
#

obj-m := cryptoflow_tdrv.o

astra_subdir:=$(BUILD_DIR)/secureos/astra

EXTRA_CFLAGS += \
	-I$(src)/../../bcm_astra/bcm9$(BRCM_CHIP) \
	-I$(astra_subdir)/common/include \
	-I$(astra_subdir)/user/cryptoflow_app


