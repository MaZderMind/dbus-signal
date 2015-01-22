#ifndef _DBUS_SIGNAL_OWNSRV_h
#define _DBUS_SIGNAL_OWNSRV_h

typedef void (* dbus_internal_server_connection_cb) (GDBusConnection *connection);

void
dbus_internal_server_setup(dbus_internal_server_connection_cb cb);

void
dbus_internal_server_teardown();

void
dbus_internal_server_emitsignal(const gchar * name,
                                const gchar * object_path,
                                const gchar * interface_name,
                                GVariant * parameters);

#endif
