import signal
from gi.repository import Gio, GLib, Gtk

def cb_signal(connection, sender_name, object_path, interface_name, signal_name, parameters, user_data):
	print("cb_signal: sender_name=%s object_path=%s interface_name=%s signal_name=%s, parameters=%s" % (sender_name, object_path, interface_name, signal_name, parameters))


print("Gio.bus_get_sync");
connection = Gio.bus_get_sync(
	Gio.BusType.SESSION,
	None)

print("connection.signal_subscribe");
connection.signal_subscribe(
	None,
	"us.timvideos.GstSwitch.ControllerInterface",
	None,
	None,
	None,
	0,
	cb_signal,
	None)

print("connection.call_sync(get_compose_port)");
port = connection.call_sync(
	"us.timvideos.gst-switch.controller",
	"/us/timvideos/GstSwitch/Controller",
	"us.timvideos.GstSwitch.ControllerInterface",
	"get_compose_port",
	None,
	GLib.VariantType.new("(i)"),
	Gio.DBusCallFlags.NONE,
	-1,
	None).unpack()[0];

print("get_compose_port returned %u" % port)

print("Gtk.main")
signal.signal(signal.SIGINT, signal.SIG_DFL)
Gtk.main()
