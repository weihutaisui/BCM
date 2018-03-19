# ias makefile

BUILD_DIR=$(word 1, $(subst /bcmdrivers, /bcmdrivers,$(shell pwd)))
include $(BUILD_DIR)/make.common

CFLAGS += -I.
CFLAGS += -I./as/include
CFLAGS += -I../router/shared
CFLAGS += -I../router/wapi/openssl/openssl-0.9.8e/include/openssl
CFLAGS += -I../router/wapi/openssl/openssl-0.9.8e/include
#CFLAGS += -I$(USERSPACE_PUBLIC_LIBS_DIR)/openssl
#CFLAGS += -I$(USERSPACE_PUBLIC_LIBS_DIR)/openssl/include
CFLAGS += -I../include
CFLAGS += -DDSLCPE
#CFLAGS += -DBCMDBG

LDFLAGS = -Os
LDFLAGS += -L$(EXTRALIBDIR)
LDFLAGS += -lc
LDFLAGS += -lgcc_s
LDFLAGS += -L../router/wapi/openssl/openssl-0.9.8e
#LDFLAGS += -L$(INSTALL_DIR)/lib
LDFLAGS += -lcrypto
LDFLAGS += -L$(INSTALL_DIR)/lib
LDFLAGS += -lnvram

-include files

OBJS = $(foreach x, $(FILES), $(shell find . -name "$x" -exec echo $(x:.c=.o) ";"))

vpath %.c ./as/src

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

openssl:
ifneq ($(strip $(OBJS)),)
	make -C ../router/wapi/openssl install
endif

ias: $(OBJS)
ifneq ($(strip $(OBJS)),)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
	cp -f $@ $@-$(EXT_CPU_ARCH_NAME)
else
	cp -f $@-$(EXT_CPU_ARCH_NAME) $@ 
endif

install: openssl ias
	install ./as/conf/AS.conf $(INSTALL_DIR)/etc/AS.conf
	install -m 755 ias $(INSTALL_DIR)/bin

clean:
	rm -f $(OBJS)
	rm -f ias

all dynamic static: ias 
.PHONY: openssl