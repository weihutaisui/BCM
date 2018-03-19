# Override one or more of the following to build RPM source and binary packages
ARCH=	i686
VER=	1.6
REL=	1
PPPVER=	2.4.4
TARGZ=	/tmp/openl2tp-$(VER).tar.gz
SPEC=	openl2tp.spec
AUTHOR=	$(shell whoami)
DATE=	$(shell date '+%a %b %d %Y')
REDHAT=	/usr/src/redhat

.PHONY:	spec rpm

all:	spec rpm

clean:
	-$(RM) $(wildcard *.rpm) openl2tp-$(VER)-$(REL).spec

spec:
	@if [ ! -e $(TARGZ) ]; then \
		echo "$(TARGZ) not found. Aborting." ;\
		echo "Use make TARGZ=<file> to point to the OpenL2TP release tarball" ;\
	fi
	cp $(SPEC) openl2tp-1.spec
	cat openl2tp-1.spec | sed -e 's/%%VER%%/${VER}/g' > openl2tp-2.spec
	mv -f openl2tp-2.spec openl2tp-1.spec
	cat openl2tp-1.spec | sed -e 's/%%REL%%/${REL}/g' > openl2tp-2.spec
	mv -f openl2tp-2.spec openl2tp-1.spec
	cat openl2tp-1.spec | sed -e 's/%%PPPVER%%/${PPPVER}/g' > openl2tp-2.spec
	mv -f openl2tp-2.spec openl2tp-1.spec
	cat openl2tp-1.spec | sed -e 's/%%AUTHOR%%/$(AUTHOR)/g' > openl2tp-2.spec
	mv -f openl2tp-2.spec openl2tp-1.spec
	cat openl2tp-1.spec | sed -e 's/%%DATE%%/$(DATE)/g' > openl2tp-2.spec
	mv -f openl2tp-2.spec openl2tp-1.spec
	mv -f openl2tp-1.spec openl2tp-$(VER)-$(REL).spec

rpm:
	cp openl2tp-$(VER)-$(REL).spec $(REDHAT)/SPECS/openl2tp.spec
#	cp $(wildcard openl2tp-*.patch) $(REDHAT)/SOURCES
	cp $(TARGZ) $(REDHAT)/SOURCES
	cd $(REDHAT)/SPECS; rpmbuild -ba --target=$(ARCH)-linux openl2tp.spec
	cp -v $(REDHAT)/RPMS/$(ARCH)/openl2tp*-$(VER)*.rpm .
	cp -v $(REDHAT)/SRPMS/openl2tp*-$(VER)*.rpm .

