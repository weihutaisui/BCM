# wapid makefile

BUILD_DIR=$(word 1, $(subst /bcmdrivers, /bcmdrivers,$(shell pwd)))
include $(BUILD_DIR)/make.common

CFLAGS += -Isms4
CFLAGS += -Iwapid
CFLAGS += -Iwapid/include
CFLAGS += -Iopenssl/openssl-0.9.8e
CFLAGS += -Iopenssl/openssl-0.9.8e/include
#CFLAGS += -I$(USERSPACE_PUBLIC_LIBS_DIR)/openssl
#CFLAGS += -I$(USERSPACE_PUBLIC_LIBS_DIR)/openssl/include
CFLAGS += -I../../include
CFLAGS += -I../../include/bcmcrypto
CFLAGS += -I../../shared
CFLAGS += -I../../router/eapd
CFLAGS += -I../../router/shared
CFLAGS += -DDSLCPE -DDSLCPE_ENDIAN
CFLAGS += -DBCMWAPI_WPI
CFLAGS += -DBCMWAPI_WAI
CFLAGS += -Wall
#CFLAGS += -DBCMDBG

LDFLAGS = -Os
LDFLAGS += -L$(EXTRALIBDIR)
LDFLAGS += -lc
LDFLAGS += -lgcc_s
LDFLAGS += -Lopenssl/openssl-0.9.8e
#LDFLAGS += -L$(INSTALL_DIR)/lib
LDFLAGS += -lcrypto
LDFLAGS += -L$(INSTALL_DIR)/lib
LDFLAGS += -lnvram
LDFLAGS += -lwlbcmshared
LDFLAGS += -lwlbcmcrypto

-include files
FILES = $(SMS4_FILES) $(WAPID_FILES)
OBJS = $(foreach x, $(FILES), $(shell find . -name "$x" -exec echo $(x:.c=.o) ";"))

vpath %.c sms4
vpath %.c wapid

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

openssl:
ifneq ($(strip $(OBJS)),)
	rm -rf  ./openssl/openssl-0.9.8e
	make -C ./openssl install
	cp -f ./openssl/openssl-0.9.8e/libcrypto.so.0.9.8 ./openssl/openssl-0.9.8e/libcrypto.so.0.9.8-$(EXT_CPU_ARCH_NAME)
else
	cp -f ./openssl/openssl-0.9.8e/libcrypto.so.0.9.8-$(EXT_CPU_ARCH_NAME) ./openssl/openssl-0.9.8e/libcrypto.so.0.9.8
endif

wapid: $(OBJS)
ifneq ($(strip $(OBJS)),)
	$(CC) -o wapid/$@ $(OBJS) $(LDFLAGS)
	cp -f wapid/$@ wapid/$@-$(EXT_CPU_ARCH_NAME) 
else
	cp -f wapid/$@-$(EXT_CPU_ARCH_NAME) wapid/$@
endif

install: openssl wapid
	install ./openssl/openssl-0.9.8e/libcrypto.so.0.9.8 $(INSTALL_DIR)/lib
	install -m 755 wapid/wapid $(INSTALL_DIR)/bin/wapid

clean:
	rm -f $(OBJS) *.d
ifneq ($(strip $(OBJS)),)
	mv ./openssl/openssl-0.9.8e/libcrypto.so.0.9.8 ./openssl
	rm -rf  ./openssl/openssl-0.9.8e
	install -d ./openssl/openssl-0.9.8e
	mv ./openssl/libcrypto.so.0.9.8 ./openssl/openssl-0.9.8e
endif
	rm -f ./openssl/openssl-0.9.8e/libcrypto.so.0.9.8
	rm -f wapid/wapid


.PHONY: openssl

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)