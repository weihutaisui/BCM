.PHONY:			all clean distclean install subdirs-all

all:			pppd_dummy

pppd_dummy:		pppd_dummy.c
			$(CC) -o $@ $<

clean:
			-$(RM) pppd_dummy

distclean:		clean

TAGS:
			@for d in $(SUBDIRS); do $(MAKE) -C $$d $(MFLAGS) $@; done
			etags $(wildcard *.c) $(wildcard *.h)

install:		all
			@for d in $(SUBDIRS); do $(MAKE) -C $$d $(MFLAGS) $@; if [ $$? -ne 0 ]; then exit 1; fi; done

BANNER=			@echo "----------------------------------------------------------------------------"
TCLSH=			tclsh all.tcl -preservecore 3 -verbose bps -tmpdir ./results -outfile $@.result

.PHONY:			test-setup test-postprocess test-l2tpd test-thirdparty

test:			test-setup test-l2tpd test-postprocess

test-cfg:		test-setup test-configfile test-postprocess

test-3rdparty:		test-setup test-3rdpartylns test-postprocess

test-setup:
			@if [ -d ./results ]; then rm -fr ./results; fi
			@mkdir ./results

test-l2tpd:	
			$(BANNER)
			@$(TCLSH) -constraints "l2tpdRunning peerProfile tunnelProfile sessionProfile pppProfile system tunnel session"

test-configfile:	
			$(BANNER)
			-@killall openl2tpd
#			@$(TCLSH) -constraints "peerProfile tunnelProfile sessionProfile pppProfile system tunnel session"
			@$(TCLSH) -constraints "peerProfile tunnelProfile sessionProfile pppProfile system"

test-3rdpartylns:	
			$(BANNER)
			@$(TCLSH) -constraints "l2tpdRunning thirdpartylns kernelModuleLoaded interactive" -match 'thirdpartylns-*'

test-postprocess:	
			$(BANNER)
			@(failed=`grep FAILED results/*.result | wc -l`; \
			let failed2=failed/2 ;\
			passed=`grep PASSED results/*.result | wc -l`; \
			echo "TEST SUMMARY: $$passed tests PASSED, $$failed2 tests FAILED" ;\
			exit $$failed2)

