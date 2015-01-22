LIBS = gtk+-3.0
CFLAGS = -O0 -ggdb `pkg-config --cflags $(LIBS)`
LDFLAGS = `pkg-config --libs $(LIBS)`
PROGRAMS = dbus-signal dbus-signal-ownsrv gdbus-example-peer

all: $(PROGRAMS)

clean:
	rm -f $(PROGRAMS)

dbus-signal: dbus-signal.c
	clang-3.5 $(CFLAGS) dbus-signal.c $(LDFLAGS) -o dbus-signal

dbus-signal-ownsrv: dbus-signal-ownsrv.c dbus-internal-server.h dbus-internal-server.c
	clang-3.5 $(CFLAGS) dbus-signal-ownsrv.c dbus-internal-server.c $(LDFLAGS) -o dbus-signal-ownsrv

gdbus-example-peer: gdbus-example-peer.c
	clang-3.5 $(CFLAGS) gdbus-example-peer.c $(LDFLAGS) -o gdbus-example-peer
