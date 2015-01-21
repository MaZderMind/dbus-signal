#include <stdio.h>
#include <gio/gio.h>
#include "dbus-signal-ownsrv.c"

static void
gst_switch_controller_on_connection_closed (GDBusConnection * connection,
    gboolean vanished, GError * error, gpointer user_data)
{
  g_object_unref (connection);
}

static gboolean
gst_switch_controller_on_new_connection (GDBusServer * server,
    GDBusConnection * connection, gpointer user_data)
{
  printf("new-connection...\n");

  g_signal_connect (connection, "closed",
      G_CALLBACK (gst_switch_controller_on_connection_closed), NULL);

  g_object_ref (connection);
  return TRUE;
}



static GDBusServer * bus_server;
void dbus_internal_server_setup()
{
    GError *error = NULL;

    gchar *guid = g_dbus_generate_guid ();
    //GDBusAuthObserver *auth_observer;

    GDBusServerFlags flags = G_DBUS_SERVER_FLAGS_NONE;
    flags |= G_DBUS_SERVER_FLAGS_RUN_IN_THREAD;
    flags |= G_DBUS_SERVER_FLAGS_AUTHENTICATION_ALLOW_ANONYMOUS;

    //auth_observer = g_dbus_auth_observer_new ();
    printf("g_dbus_server_new_sync\n");
    bus_server = g_dbus_server_new_sync ("tcp:host=0.0.0.0,port=6000",
                                         flags,
                                         guid,
                                         NULL /*auth_observer*/,
                                         NULL,
                                         &error);

    g_assert_no_error (error);
    g_assert (bus_server != NULL);

    printf("Controller is listening at: %s\n",
      g_dbus_server_get_client_address (bus_server));

    g_signal_connect (bus_server, "new-connection",
      G_CALLBACK (gst_switch_controller_on_new_connection), NULL);

    g_dbus_server_start (bus_server);
    //g_object_unref (auth_observer);

    g_free (guid);
}

void dbus_internal_server_teardown()
{
    g_dbus_server_stop (bus_server);
    g_object_unref (bus_server);
}
