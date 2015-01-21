#include <stdio.h>
#include <gio/gio.h>

#define GSTSWITCH_BUS_NAME       "us.timvideos.gst-switch.controller"
#define GSTSWITCH_OBJECT_PATH    "/us/timvideos/GstSwitch/Controller"
#define GSTSWITCH_INTERFACE_NAME "us.timvideos.GstSwitch.ControllerInterface"

static GDBusNodeInfo *introspection_data = NULL;

/* Introspection data for the service we are exporting */
static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='" GSTSWITCH_INTERFACE_NAME "'>"
  "    <method name='get_compose_port'>"
  "      <arg type='i' name='port' direction='out'/>"
  "    </method>"
  "    <signal name='preview_port'>"
  "      <arg type='i' name='port'/>"
  "      <arg type='i' name='serve'/>"
  "      <arg type='i' name='type'/>"
  "    </signal>"
  "  </interface>"
  "</node>";



static void
handle_method_call (GDBusConnection       *connection,
                    const gchar           *sender,
                    const gchar           *object_path,
                    const gchar           *interface_name,
                    const gchar           *method_name,
                    GVariant              *parameters,
                    GDBusMethodInvocation *invocation,
                    gpointer               user_data)
{
  if (g_strcmp0 (method_name, "get_compose_port") == 0)
  {
    printf("get_compose_port called\n");
    g_dbus_method_invocation_return_value (invocation, g_variant_new ("(i)", 3000));
  }
}

static GVariant *
handle_get_property (GDBusConnection  *connection,
                     const gchar      *sender,
                     const gchar      *object_path,
                     const gchar      *interface_name,
                     const gchar      *property_name,
                     GError          **error,
                     gpointer          user_data)
{
  return NULL;
}

static gboolean
handle_set_property (GDBusConnection  *connection,
                     const gchar      *sender,
                     const gchar      *object_path,
                     const gchar      *interface_name,
                     const gchar      *property_name,
                     GVariant         *value,
                     GError          **error,
                     gpointer          user_data)
{
  return TRUE;
}

static const GDBusInterfaceVTable interface_vtable =
{
  handle_method_call,
  handle_get_property,
  handle_set_property
};



static gboolean
timeout_cb (gpointer user_data)
{
  GDBusConnection *connection = G_DBUS_CONNECTION (user_data);

  printf("emitting_signal\n");
  guint port = 4000, serve = 123, type = 2;
  GError *error;
  g_dbus_connection_emit_signal (connection,
                                 NULL,
                                 GSTSWITCH_OBJECT_PATH,
                                 GSTSWITCH_INTERFACE_NAME,
                                 "preview_port",
                                 g_variant_new ("(iii)",
                                                port, serve, type),
                                 &error);

  g_assert_no_error (error);
  return TRUE;
}



void bus_name_acquired_cb (GDBusConnection *connection,
                                 const gchar *name,
                                 gpointer user_data)
{
    printf("bus_name_acquired_cb\n");
}

void bus_name_lost_cb (GDBusConnection *connection,
                             const gchar *name,
                             gpointer user_data)
{
    printf ("bus_name_lost_cb\n");
}


static gboolean
gst_switch_controller_on_new_connection (GDBusServer * server,
    GDBusConnection * connection, gpointer user_data)
{
  printf("new-connection...\n");
  g_object_ref (connection);
  return TRUE;
}

int main(int argc, char * argv[])
{
    guint owner_id;
    GMainLoop *loop;
    GError *error = NULL;

    introspection_data = g_dbus_node_info_new_for_xml (introspection_xml, NULL);
    g_assert (introspection_data != NULL);



    gchar *guid = g_dbus_generate_guid ();
    GDBusServer * bus_server;
    GDBusAuthObserver *auth_observer;

    GDBusServerFlags flags = G_DBUS_SERVER_FLAGS_NONE;
    flags |= G_DBUS_SERVER_FLAGS_RUN_IN_THREAD;
    flags |= G_DBUS_SERVER_FLAGS_AUTHENTICATION_ALLOW_ANONYMOUS;

    auth_observer = g_dbus_auth_observer_new ();
    printf("g_dbus_server_new_sync\n");
    bus_server = g_dbus_server_new_sync ("tcp:host=0.0.0.0,port=5000",
                                         flags,
                                         guid,
                                         auth_observer,
                                         NULL,
                                         &error);

    g_assert_no_error (error);
    g_assert (bus_server != NULL);

    printf("Controller is listening at: %s\n",
      g_dbus_server_get_client_address (bus_server));

    g_signal_connect (bus_server, "new-connection",
      G_CALLBACK (gst_switch_controller_on_new_connection), NULL);

    g_dbus_server_start (bus_server);
    g_object_unref (auth_observer);

    g_free (guid);



    printf("g_dbus_connection_new_for_address_sync\n");
    GDBusConnection * con = g_dbus_connection_new_for_address_sync("tcp:host=127.0.0.1,port=5000",
                                                                    G_DBUS_CONNECTION_FLAGS_NONE,
                                                                    NULL,
                                                                    NULL,
                                                                    &error);

    g_assert_no_error (error);



    guint registration_id;

    printf("g_dbus_connection_register_object\n");
    registration_id = g_dbus_connection_register_object (con,
                                                         GSTSWITCH_OBJECT_PATH,
                                                         introspection_data->interfaces[0],
                                                         &interface_vtable,
                                                         NULL,  /* user_data */
                                                         NULL,  /* user_data_free_func */
                                                         NULL); /* GError** */

    g_assert (registration_id > 0);



    printf("g_timeout_add_seconds\n");
    g_timeout_add_seconds (2,
                           timeout_cb,
                           con);



    printf("g_bus_own_name_on_connection\n");
    owner_id = g_bus_own_name_on_connection (con,
                                             GSTSWITCH_BUS_NAME,
                                             G_BUS_NAME_OWNER_FLAGS_NONE,
                                             &bus_name_acquired_cb,
                                             &bus_name_lost_cb,
                                             /*user_data*/ NULL,
                                             /*user_data_free_func*/ NULL);

    printf("g_bus_own_name done, entering mainloop\n");

    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);


    g_dbus_connection_unregister_object(con, registration_id);

    g_bus_unown_name (owner_id);

    g_dbus_node_info_unref (introspection_data);

    g_dbus_server_stop (bus_server);
    g_object_unref (bus_server);

    return 0;
}
