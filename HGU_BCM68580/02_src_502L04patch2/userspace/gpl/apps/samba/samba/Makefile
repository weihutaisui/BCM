
all:	build

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common


ifneq ($(strip $(BUILD_BEEP_SAMBA)),)
INSTALL_DIR=../samba_beep
endif

ifeq ($(strip $(BUILD_BCM_RECVFILE)),y)
CFLAGS := -DHAVE_BCM_RECVFILE
SMBCONFFILE:=smb_rcvfile.conf
else
CFLAGS :=
SMBCONFFILE:=smb.conf
endif


ifeq ($(strip $(BRCM_USER_SSP)),y)
CFLAGS+=$(SSP_MIN_COMPILER_OPTS)
LDFLAGS=-L$(INSTALL_DIR)/lib $(SSP_LIBS) 
export CFLAGS
export LDFLAGS
endif

build:
	if [ ! -e ./source/Makefile ];then \
	cd ./source; ./configure --host=$(TOOLCHAIN_PREFIX) --target=$(TOOLCHAIN_PREFIX) --disable-developer --enable-cups=no --enable-fam=no --enable-shared=yes --enable-iprint=no --enable-pie=no --without-ldap --without-cifsupcall --without-cifsmount --without-cluster-support --without-acl-support --with-privatedir=/var/samba --with-lockdir=/var/samba/locks --with-configdir=/var/samba --with-piddir=/var/samba --with-logfilebase=/var/samba SMB_BUILD_CC_NEGATIVE_ENUM_VALUES=no linux_getgrouplist_ok=no samba_cv_have_longlong=yes samba_cv_HAVE_IFACE_IFCONF=yes ; \
	fi

	echo $(CFLAGS)
	$(warning $(CFLAGS))
	$(warning $(LDFLAGS))
	$(MAKE) -C ./source srcdir=./ builddir=./
	install -m 755 ./source/bin/smbd $(INSTALL_DIR)/bin/smbd
	install -m 755 ./source/bin/smbpasswd $(INSTALL_DIR)/bin/smbpasswd
	install -m 444 ./source/conf/$(SMBCONFFILE) $(INSTALL_DIR)/etc/smb.conf


clean:
	if [ -f ./source/Makefile ];then $(MAKE) -C ./source clean distclean srcdir=./ builddir=./;fi;

shell:
	bash
