CFLAGS = -s -Os -fomit-frame-pointer
CFLAGS += $(BRCM_WERROR_CFLAGS)

EXE = ivictl
OBJS = $(EXE).o

all dynamic install: conditional_build

clean:
	-rm -f $(EXE) *.o
	-rm -f $(INSTALL_DIR)/bin/$(EXE)

ifneq ($(strip $(BUILD_MAP)),)
conditional_build: $(EXE)
	install -m 755 $(EXE) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(EXE)

$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS)
else
conditional_build:
	@echo "skipping $(EXE) (not configured)"
endif

