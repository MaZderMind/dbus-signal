all: dbus-signal dbus-signal-ownsrv

clean:
	rm -f dbus-signal

dbus-signal: dbus-signal.c
	clang-3.5 dbus-signal.c -o dbus-signal `pkg-config --cflags --libs gtk+-3.0`

dbus-signal-ownsrv: dbus-signal-ownsrv.c
	clang-3.5 dbus-signal-ownsrv.c -o dbus-signal-ownsrv `pkg-config --cflags --libs gtk+-3.0`
