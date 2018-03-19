#
# Add all the objects that make up the CYSSL_core lib here.
# Implicit rule will make the .c into a .o
# Implicit rule is $(CC) -c $(CPPFLAGS) $(CFLAGS)
# See Section 10.2 of Gnu Make manual
# 

OBJS = ssl.o cyassl_int.o keys.o tls.o
LIB = libbcmssl.so

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
	rm -f *.o *.d *.d.* *.core $(LIB) $(INSTALL_DIR)/lib/private/$(LIB)

include $(BUILD_DIR)/make.deprules

-include $(OBJS:.o=.d)

