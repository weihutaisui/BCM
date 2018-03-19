.PHONY:		all clean install

MANSRC.1=	l2tpconfig.1
MANSRC.4=	openl2tp_rpc.4
MANSRC.5=	openl2tpd.conf.5
MANSRC.7=	openl2tp.7
MANSRC.8=	openl2tpd.8

MANSRC=		$(MANSRC.1) $(MANSRC.4) $(MANSRC.7) $(MANSRC.8) 
ifeq ($(L2TP_FEATURE_LOCAL_CONF_FILE),y)
MANSRC+=	$(MANSRC.5)
endif

all:		$(MANSRC:%=%.gz)

%.gz: %
		gzip --best -c $< > $@

clean:
		-$(RM) $(MANSRC:%=%.gz)

install:	all
		$(INSTALL) -d $(DESTDIR)/usr/share/man/man1
ifeq ($(L2TP_FEATURE_RPC_MANAGEMENT),y)
		$(INSTALL) $(MANSRC.1:%=%.gz) $(DESTDIR)/usr/share/man/man1
endif
		$(INSTALL) -d $(DESTDIR)/usr/share/man/man4
		$(INSTALL) $(MANSRC.4:%=%.gz) $(DESTDIR)/usr/share/man/man4
ifeq ($(L2TP_FEATURE_LOCAL_CONF_FILE),y)
		$(INSTALL) -d $(DESTDIR)/usr/share/man/man5
		$(INSTALL) $(MANSRC.5:%=%.gz) $(DESTDIR)/usr/share/man/man5
endif
		$(INSTALL) -d $(DESTDIR)/usr/share/man/man7
		$(INSTALL) $(MANSRC.7:%=%.gz) $(DESTDIR)/usr/share/man/man7
		$(INSTALL) -d $(DESTDIR)/usr/share/man/man8
		$(INSTALL) $(MANSRC.8:%=%.gz) $(DESTDIR)/usr/share/man/man8

TAGS:

