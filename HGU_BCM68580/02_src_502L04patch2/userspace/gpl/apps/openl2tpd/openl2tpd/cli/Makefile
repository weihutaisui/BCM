CLI_SRCS.c=		cli_lib.c cli_readline.c
CLI_SRCS.h=		cli_api.h cli_private.h
CLI_SRCS_TEST.c=	cli_test.c

CLI_SRCS.o=		$(CLI_SRCS.c:%.c=%.o)
CLI_SRCS_TEST.o=	$(CLI_SRCS_TEST.c:%.c=%.o)

LDFLAGS.cli_test=	-L.. -L. $(READLINE_LDFLAGS) -lcli -lusl -lreadline -lcurses -lc

CFLAGS=			$(CFLAGS.optimize) -MMD -Wall -Werror -I.. $(READLINE_CFLAGS)

.PHONY:			all test clean

all:			libcli.a

test:			libcli.a cli_test

libcli.a:		$(CLI_SRCS.o)
			$(AR) rus $@ $^

cli_test:		$(CLI_SRCS_TEST.o) libcli.a
			$(CC) -o $@ $(CLI_SRCS_TEST.o) $(LDFLAGS.cli_test)

clean:
	$(RM) $(CLI_SRCS.o) $(CLI_SRCS_TEST.o) libcli.a cli_test $(wildcard *.d)

distclean:
			$(RM) TAGS

%.o:	%.c
			$(CC) -c $(CFLAGS) $< -o $@

install:		all
#			$(INSTALL) -d $(DESTDIR)$(SYS_LIBDIR)
#			$(INSTALL) libcli.a $(DESTDIR)$(SYS_LIBDIR)

TAGS:			$(CLI_SRCS.c) $(CLI_SRCS.h)
			etags $^

include $(wildcard *.d /dev/null)
