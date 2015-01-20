all: dbus-signal

clean:
	rm -f dbus-signal

dbus-signal: dbus-signal.c
	clang-3.5 dbus-signal.c -o dbus-signal `pkg-config --cflags --libs gtk+-3.0`
