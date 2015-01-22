#include <stdio.h>
#include <gio/gio.h>
#include "dbus-internal-server.h"

static GDBusServer * bus_server;
static GList *clients;
static dbus_internal_server_connection_cb connection_cb;

static void
connection_closed_cb (GDBusConnection * connection,
    gboolean vanished, GError * error, gpointer user_data)
{
    printf("connection_closed_cb\n");
    clients = g_list_remove (clients, connection);

    g_object_unref (connection);
}

static gboolean
new_connection_cb (GDBusServer * server,
    GDBusConnection * connection, gpointer user_data)
{
    printf("new_connection_cb\n");
    clients = g_list_append (clients, connection);

    g_signal_connect (connection, "closed",
        G_CALLBACK (connection_closed_cb), NULL);

    (*connection_cb) (connection);

    g_object_ref (connection);
    return TRUE;
}

void
dbus_internal_server_emitsignal(const gchar * name,
                                const gchar * object_path,
                                const gchar * interface_name,
                                GVariant * parameters)
{
    printf("dbus_internal_server_emitsignal\n");
    GList *it;
    GDBusConnection *connection;


    for (it = clients; it != NULL; it = it->next)
    {
        connection = G_DBUS_CONNECTION (it->data);

        GError *error = NULL;
        printf("g_dbus_connection_emit_signal for connection=%p\n", connection);
        g_dbus_connection_emit_signal (connection,
                                       NULL,
                                       object_path,
                                       interface_name,
                                       name,
                                       parameters,
                                       &error);

        g_assert_no_error (error);
    }
}



void
dbus_internal_server_setup(dbus_internal_server_connection_cb cb)
{
    GError *error = NULL;
    gchar *guid = g_dbus_generate_guid ();
    //GDBusAuthObserver *auth_observer;

    printf("connection_cb=%p, cb=%p\n", connection_cb, cb);
    connection_cb = cb;

    GDBusServerFlags flags = G_DBUS_SERVER_FLAGS_NONE;
    //flags |= G_DBUS_SERVER_FLAGS_RUN_IN_THREAD;
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
    g_object_ref(bus_server);

    printf("Controller is listening at: %s\n",
        g_dbus_server_get_client_address (bus_server));

    g_signal_connect (bus_server, "new-connection",
        G_CALLBACK (new_connection_cb), NULL);

    g_dbus_server_start (bus_server);
    //g_object_unref (auth_observer);

    g_free (guid);
}

void dbus_internal_server_teardown()
{
    g_dbus_server_stop (bus_server);
    g_object_unref (bus_server);
}
