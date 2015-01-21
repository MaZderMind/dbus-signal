all: dbus-signal dbus-signal-ownsrv

clean:
	rm -f dbus-signal

dbus-signal: dbus-signal.c
	gcc dbus-signal.c -o dbus-signal `pkg-config --cflags --libs gtk+-3.0`

dbus-signal-ownsrv: dbus-signal-ownsrv.c dbus-internal-server.h dbus-internal-server.c
	gcc dbus-signal-ownsrv.c dbus-internal-server.c `pkg-config --cflags --libs gtk+-3.0` -o dbus-signal-ownsrv
