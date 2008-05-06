CC=gcc

PLATFORM=$(shell uname -s)

PURPLE_CFLAGS=`pkg-config --cflags purple`
PURPLE_LIBS=`pkg-config --libs purple`
PURPLE_PREFIX=`pkg-config --variable=prefix purple`

GOBJECT_CFLAGS=`pkg-config --cflags gobject-2.0`
GOBJECT_LIBS=`pkg-config --libs gobject-2.0`

EXTRA_WARNINGS=-Wall -W -Wformat-nonliteral -Wcast-align -Wpointer-arith \
	       -Wbad-function-cast -Wmissing-prototypes -Wstrict-prototypes \
	       -Wmissing-declarations -Winline -Wundef -Wnested-externs -Wcast-qual \
	       -Wshadow -Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic -ansi -std=c99

OTHER_WARNINGS=-D_FORTIFY_SOURCE=2 -fstack-protector -g3 -pedantic -W -Wall \
	       -Wbad-function-cast -Wcast-align -Wcast-qual -Wdisabled-optimization \
	       -Wendif-labels -Wfloat-equal -Wformat=2 -Wformat-nonliteral -Winline \
	       -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wno-unused-parameter \
	       -Wpointer-arith -Wshadow -Wstack-protector -Wstrict-prototypes \
	       -Wswitch -Wundef -Wwrite-strings

CFLAGS=-Wall -ggdb $(EXTRA_WARNINGS)

override CFLAGS += -I. -DHAVE_LIBPURPLE

purpledir=$(DESTDIR)/$(PURPLE_PREFIX)/lib/purple-2

objects = \
	directconn.o \
	error.o \
	msn.o \
	nexus.o \
	notification.o \
	object.o \
	page.o \
	session.o \
	state.o \
	switchboard.o \
	sync.o \
	pecan_log.o \
	pecan_printf.o \
	pecan_util.o \
	cmd/cmdproc.o \
	cmd/command.o \
	cmd/history.o \
	cmd/msg.o \
	cmd/table.o \
	cmd/transaction.o \
	io/pecan_buffer.o \
	ab/pecan_group.o \
	ab/pecan_contact.o \
	ab/pecan_contactlist.o \
	io/pecan_stream.o \
	io/pecan_node.o \
	io/pecan_cmd_server.o \
	io/pecan_http_server.o \
	cvr/slp.o \
	cvr/slpcall.o \
	cvr/slplink.o \
	cvr/slpmsg.o \
	cvr/slpsession.o \
	fix_purple.o

sources = $(patsubst %.o,%.c,$(objects))

ifeq ($(PLATFORM),Darwin)
	DYNFLAG=-dynamiclib
else
	DYNFLAG=-shared
endif

all: libmsn-pecan.so

libmsn-pecan.so: $(objects)
	$(CC) $(LDFLAGS) $(PURPLE_LIBS) $(GOBJECT_LIBS) $+ $(DYNFLAG) -o $@

%.o: %.c
	$(CC) -fPIC $(CFLAGS) $(PURPLE_CFLAGS) $(GOBJECT_CFLAGS) $< -c -o $@

clean:
	rm -f libmsn-pecan.so $(objects)

depend:
	makedepend -Y -- $(CFLAGS) -- $(sources)

dist:
	git archive --format=tar --prefix=msn-pecan-$(version)/ $(version) > /tmp/msn-pecan-$(version).tar
	mkdir -p msn-pecan-$(version)
	git-changelog > msn-pecan-$(version)/ChangeLog
	chmod 664 msn-pecan-$(version)/ChangeLog
	tar --append -f /tmp/msn-pecan-$(version).tar --owner root --group root msn-pecan-$(version)/ChangeLog
	rm -r msn-pecan-$(version)
	bzip2 /tmp/msn-pecan-$(version).tar

install: libmsn-pecan.so
	mkdir -p $(purpledir)
	install libmsn-pecan.so $(purpledir)
	# chcon -t textrel_shlib_t /usr/lib/purple-2/libmsn-pecan.so
