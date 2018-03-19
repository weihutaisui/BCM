ifndef BUILD_WL_PARALLEL
	MAKEFLAGS =+ -j1
endif

current_dir:=$(lastword $(MAKEFILE_LIST))
override obj := $(abspath $(obj))
ifneq ($(strip $(BRCMDRIVERS_DIR)),)
ifeq ($(findstring wlobj, $(current_dir)),)
ifneq ($(strip $(BRCM_DRIVER_DHD)),)
include $(BRCMDRIVERS_DIR)/broadcom/net/wl/makefile.dhd.src
endif
endif
ifeq ($(findstring dhdobj, $(current_dir)),)
OPEN_OBJS = ../shared/impl1/wl_linux_dslcpe.o ../shared/impl1/wl_blog.o ../shared/impl1/wl_wfd.o ../shared/impl1/wl_pktc.o ../shared/impl1/wl_thread.o main/src/wl/sys/wl_linux.o 
include $(BRCMDRIVERS_DIR)/broadcom/net/wl/makefile.wlan.src
endif
else
include ../makefile.release
endif
