#
# Copyright 2014  Broadcom Corporation
#

build dynamic install: bin-install init-install rc3-install scripts-install
clean:                 bin-clean   init-clean   rc3-clean   scripts-clean

CURR_DIR  := $(shell pwd)
BUILD_DIR := $(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR := $(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

SCRIPTS:=


ifeq ($(strip $(BRCM_DEFAULTS_VOLSIZE))$(strip $(BRCM_LOCAL_VOLSIZE)),) 
RC3:=
else
RC3:= S30defaults
$(shell mkdir -p $(INSTALL_DIR)/local )
endif

BIN:=
INIT:=

$(info BRCM_DRIVER_LOG: $(BRCM_DRIVER_LOG))

ifeq ($(strip $(BRCM_DRIVER_LOG)),y) # true
SCRIPTS += procbcmlog/bcmlog procbcmlog/spitr 
ifeq ($(filter-out 63268,${BRCM_CHIP}),)
SCRIPTS += procbcmlog/ssrw
endif
endif

ifneq ($(strip $(BRCM_DRIVER_SPU)),)
ifneq ($(strip $(BRCM_DRIVER_PKTFLOW)),)
ifeq ($(filter-out 4908 63158,${BRCM_CHIP}),)
BIN += spu_blog_enable
endif
endif
endif

ifneq ($(strip $(BRCM_DRIVER_WIRELESS)),)
ifeq ($(strip $(BUILD_BRCM_HNDROUTER)),)
RC3 += S46wl-setup-threads
endif
endif

init-install: ${INIT}
	@echo "init-install: ${INIT}"    
	@test -z "${INIT}" || install -Dp ${INIT} ${INSTALL_DIR}/etc/init.d
init-clean:
	for f in ${INIT}; do rm -f ${INSTALL_DIR}/etc/init.d/$$f; done

# RC3 names must match /^[KS][0-9][0-9]/
rc3-install:
	mkdir -p ${INSTALL_DIR}/etc/init.d/
	mkdir -p ${INSTALL_DIR}/etc/rc3.d/
	@echo "rc3-install: ${RC3}"    
	@for f in ${RC3}; do \
	  cat init/$${f:3} | \
	    sed -e 's/___BRCM_DEFAULTS_VOLSIZE___/$(BRCM_DEFAULTS_VOLSIZE)/'  \
	    | sed -e 's/___BRCM_LOCAL_VOLSIZE___/$(BRCM_LOCAL_VOLSIZE)/'  \
	    > ${INSTALL_DIR}/etc/init.d/$${f:3} ; \
	  chmod +x ${INSTALL_DIR}/etc/init.d/$${f:3} ;  ln -fs ../init.d/$${f:3} ${INSTALL_DIR}/etc/rc3.d/$$f; \
	done
rc3-clean:
	for f in ${RC3}; do rm -f ${INSTALL_DIR}/etc/rc3.d/$$f; done

bin-install: ${BIN}
	@echo "bin-install: ${BIN}"    
	@test -z "${BIN}" || install -Dp ${BIN} ${INSTALL_DIR}/bin
bin-clean: ${BIN}
	for f in ${BIN}; do rm -f ${INSTALL_DIR}/bin/$$f; done

scripts-install: ${SCRIPTS}
	@echo "scripts-install: ${SCRIPTS}"    
	@test -z "${SCRIPTS}" || install -Dp ${SCRIPTS} ${INSTALL_DIR}/opt/scripts
scripts-clean: 
	for f in ${SCRIPTS}; do rm -f ${INSTALL_DIR}/opt/scripts/$$f; done

.PHONY: build dynamic install clean bin-install init-install rc3-install scripts-install bin-clean init-clean rc3-clean scripts-clean
