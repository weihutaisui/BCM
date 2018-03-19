#Linux Makefile

include $(SRCBASE)/router/.config

INCLUDE += -I. -I.. -I$(SRCBASE)/include ${WLAN_StdIncPathA} -I$(SRCBASE)/common/include -I$(SRCBASE)/shared -I$(SRCBASE)/router/shared -I$(SRCBASE)/router/nas -I$(SRCBASE)/router/nas/nas_fbt

CFLAGS += $(INCLUDE)
CFLAGS += $(WLAN_ComponentIncPath)
CFLAGS += -DEAPD_WKSP_AUTO_CONFIG
#CFLAGS += -DNAS_GTK_PER_STA
CFLAGS += -Wall -DEAPDDUMP

CFLAGS  += -s

ifneq ($(LINUXDIR),)
CFLAGS += -Dlinux
endif

LDFLAGS += -L$(TOP)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram
LDFLAGS += -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared
LDFLAGS += $(EXTRA_LDFLAGS)

vpath %.c $(SRCBASE)/router/eapd $(SRCBASE)/shared

OBJS = eapd.o eapd_linux.o
OBJS += nas_eap.o wps_eap.o ses_eap.o

#ifdef BCM_DCS
ifeq ($(CONFIG_BCMDCS),y)
OBJS += dcs_eap.o
CFLAGS += -DBCM_DCS
endif
#endif // endif

#ifdef BCM_MEVENT
ifeq ($(CONFIG_BCM_MEVENT),y)
OBJS += mevent_eap.o
OBJS += bcmevent.o
CFLAGS += -DBCM_MEVENT
endif
#endif // endif

#ifdef BCM_BSD
ifeq ($(CONFIG_BCMBSD),y)
OBJS += bsd_eap.o
CFLAGS += -DBCM_BSD
endif
#endif // endif

#ifdef BCM_SSD
ifeq ($(CONFIG_BCMSSD),y)
OBJS += ssd_eap.o
CFLAGS += -DBCM_SSD
endif
#endif // endif

#ifdef BCM_EVENTD
ifeq ($(CONFIG_BCMEVENTD),y)
OBJS += eventd_eap.o
CFLAGS += -DBCM_EVENTD
endif
#endif // endif

#ifdef BCM_ASPMD
ifneq (2_6,$(LINUX_VERSION))
ifeq ($(CONFIG_BCMASPMD),y)
OBJS += aspm_eap.o
CFLAGS += -DBCM_ASPMD
endif
endif
#endif // endif

#ifdef CONFIG_VISUALIZATION
ifeq ($(CONFIG_VISUALIZATION),y)
OBJS += visdcoll_eap.o
endif
#endif // endif

#ifdef BCM_WBD
ifeq ($(CONFIG_WBD),y)
OBJS += wbd_eap.o
endif
#endif // endif

all: eapd

eapd: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

install: all
	install -d $(INSTALLDIR)/bin
	install eapd $(INSTALLDIR)/bin
	$(STRIP) $(INSTALLDIR)/bin/eapd

clean:
	rm -f *.o eapd
