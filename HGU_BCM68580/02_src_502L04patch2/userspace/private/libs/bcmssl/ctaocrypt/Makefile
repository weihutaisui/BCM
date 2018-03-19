#
# Add all the objects that make up the CYSSL_core lib here.
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 
OBJS = src/random.o src/rsa.o src/md5.o src/aes.o src/coding.o src/sha.o src/integer.o \
		src/asn.o src/hmac.o src/arc4.o src/des3.o

LIB = libbcmcrypto.so

all: install

$(LIB): $(OBJS)
	$(CC) -shared -Wl,-soname,$@ -o $@ $(OBJS)

install: $(LIB)
	install -m 755 $(LIB) $(INSTALL_DIR)/lib/private
	$(STRIP) $(INSTALL_DIR)/lib/private/$(LIB)

#
# Cleaning rules
#
clean:
	rm -f src/*.o src/*.d src/*.d.* $(LIB) $(INSTALL_DIR)/lib/private/$(LIB)

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)

