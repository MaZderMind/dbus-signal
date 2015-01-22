#include <stdio.h>
#include <gio/gio.h>
#include "dbus-internal-server.h"

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
    printf("emitting_signal\n");

    // this may fail with multiple invocation
    GVariant * params;
    params = g_variant_new ("(iii)", 10, 20, 50);
    dbus_internal_server_emitsignal ("foo_signal",
                                     GSTSWITCH_OBJECT_PATH,
                                     GSTSWITCH_INTERFACE_NAME,
                                     params);

    return TRUE;
}


void
connection_cb (GDBusConnection *connection)
{
    printf("connection_cb\n");

    GError *error = NULL;
    guint registration_id;

    registration_id = g_dbus_connection_register_object (connection,
                                                         GSTSWITCH_OBJECT_PATH,
                                                         introspection_data->interfaces[0],
                                                         &interface_vtable,
                                                         NULL,  /* user_data */
                                                         NULL,  /* user_data_free_func */
                                                         &error); /* GError** */

    g_assert_no_error (error);
    g_assert (registration_id > 0);
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


int main(int argc, char * argv[])
{
    guint owner_id;
    GMainLoop *loop;

    printf("dbus_internal_server_setup\n");
    dbus_internal_server_setup(connection_cb);

    g_timeout_add_seconds (2, timeout_cb, NULL);

    printf("g_main_loop_new & g_main_loop_run\n");
    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);


    printf("dbus_internal_server_teardown\n");
    dbus_internal_server_teardown();

    g_dbus_node_info_unref (introspection_data);

    return 0;
}
