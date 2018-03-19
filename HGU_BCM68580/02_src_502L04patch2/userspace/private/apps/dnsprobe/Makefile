#
# Simple build file for now
#
# dnsprobe: kick off secondary dns server relay should primary dns server die
#
CFLAGS = -s -Os -fomit-frame-pointer -I$(BUILD_DIR)/userspace/public/include
LDFLAGS = -Wl,-allow-shlib-undefined
ifeq ($(strip $(BUILD_DNSPROBE)), static)
CFLAGS += -DBUILD_STATIC 
endif

CFLAGS+=-Wall

OBJS = dns.o

all: dnsprobe

install: dnsprobe
	install -m 755 dnsprobe $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/dnsprobe

dynamic: all install

static: dns.a

.c.o:
	$(CC) $(CFLAGS) -c $<

dnsprobe: dns.o
	$(CC) $(LDFLAGS) -o dnsprobe dns.o

dns.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

clean:
	-rm -f dnsprobe *.o  dns.a
