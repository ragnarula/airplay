#include <libgupnp/gupnp.h>
#include <libgupnp-av/gupnp-av.h>
#include <stdlib.h>
#include <gmodule.h>

const char *ip =NULL;

void list_params (GUPnPServiceAction *action){
	GValue value = G_VALUE_INIT;
	g_value_init (&value, G_TYPE_UINT);
	gupnp_service_action_get_value (action, "ObjectID", &value);
	g_print("Object Id %s\n",g_strdup_value_contents(&value));

	g_value_unset(&value);
	g_value_init (&value, G_TYPE_STRING);
	gupnp_service_action_get_value (action, "BrowseFlag", &value);
	g_print("BrowseFlag %s\n",g_strdup_value_contents(&value));

	g_value_unset(&value);
	g_value_init (&value, G_TYPE_STRING);
	gupnp_service_action_get_value (action, "Filter", &value);
	g_print("Filter %s\n",g_strdup_value_contents(&value));

	g_value_unset(&value);
	g_value_init (&value, G_TYPE_UINT);
	gupnp_service_action_get_value (action, "StartingIndex", &value);
	g_print("StartingIndex %s\n",g_strdup_value_contents(&value));

	g_value_unset(&value);
	g_value_init (&value, G_TYPE_UINT);
	gupnp_service_action_get_value (action, "RequestedCount", &value);
	g_print("RequestedCount %s\n",g_strdup_value_contents(&value));

	g_value_unset(&value);
	g_value_init (&value, G_TYPE_STRING);
	gupnp_service_action_get_value (action, "SortCriteria", &value);
	g_print("SortCriteria %s\n",g_strdup_value_contents(&value));

}

void browse_cb (G_GNUC_UNUSED GUPnPService *service,
		G_GNUC_UNUSED GUPnPServiceAction         *action,
		G_GNUC_UNUSED gpointer      user_data){

	GUPnPDIDLLiteWriter *writer = NULL;	
	GUPnPDIDLLiteItem *item = NULL;
	GUPnPDIDLLiteContainer *container = NULL;
	GUPnPDIDLLiteResource *resource = NULL;
	GUPnPProtocolInfo *info = NULL;
	GError *error = NULL;
	guint id;

	GValue value = G_VALUE_INIT;
	g_value_init (&value, G_TYPE_UINT);
	gupnp_service_action_get_value (action, "ObjectID", &value);
	id = g_value_get_uint(&value);

	char *uri = NULL;
	uri = g_strconcat("http://", ip, "/stream.avi", NULL);

	if(error != NULL){
		g_print("Protocol Error: %s\n", error->message);
		g_error_free(error);
	}

	list_params(action);

	writer = gupnp_didl_lite_writer_new(NULL);

	if(id == 0){

		container = gupnp_didl_lite_writer_add_container(writer);

		gupnp_didl_lite_object_set_restricted((GUPnPDIDLLiteObject*)container, FALSE);
		gupnp_didl_lite_object_set_id((GUPnPDIDLLiteObject*)container, "1");
		gupnp_didl_lite_object_set_parent_id((GUPnPDIDLLiteObject*)container, "0");
		gupnp_didl_lite_object_set_title((GUPnPDIDLLiteObject*)container, "Video");
		gupnp_didl_lite_object_set_upnp_class((GUPnPDIDLLiteObject*)container, "object.container.storageFolder");
		gupnp_didl_lite_container_set_child_count(container, 1);
	}

	else if(id == 1){
		info = gupnp_protocol_info_new_from_string("http-get:*:video/mp4:*", &error);
		item = gupnp_didl_lite_writer_add_item(writer); 

		gupnp_didl_lite_object_set_id((GUPnPDIDLLiteObject*)item, "2");
		gupnp_didl_lite_object_set_parent_id((GUPnPDIDLLiteObject*)item, "1");
		gupnp_didl_lite_object_set_restricted((GUPnPDIDLLiteObject*)item, FALSE);
		gupnp_didl_lite_object_set_title((GUPnPDIDLLiteObject*)item, "Stream");
		gupnp_didl_lite_object_set_upnp_class((GUPnPDIDLLiteObject*)item, "object.item.videoItem.movie");

		resource = gupnp_didl_lite_object_add_resource((GUPnPDIDLLiteObject*)item);	

		gupnp_didl_lite_resource_set_uri(resource, uri);
		gupnp_didl_lite_resource_set_protocol_info(resource, info);

	}


	g_print("\nResponse :\n%s\n", gupnp_didl_lite_writer_get_string(writer));	
	gupnp_service_action_set(action, "Result", G_TYPE_STRING, gupnp_didl_lite_writer_get_string(writer),
			"NumberReturned", G_TYPE_UINT, 1, 
			"TotalMatches", G_TYPE_UINT, 1,
			"UpdateID", G_TYPE_UINT, 10, NULL);

	gupnp_service_action_return(action);

	g_object_unref(resource);
	g_object_unref(info);
	g_object_unref(item);
	g_object_unref(writer);
	g_free(uri);

}

void get_search_capabilities_cb (G_GNUC_UNUSED GUPnPService *service,
		GUPnPServiceAction         *action,
		G_GNUC_UNUSED gpointer      user_data) {

	gupnp_service_action_set (action, "SearchCaps", G_TYPE_STRING, "", NULL);
	gupnp_service_action_return(action);

}

void get_sort_capabilities_cb (G_GNUC_UNUSED GUPnPService *service,
		G_GNUC_UNUSED GUPnPServiceAction         *action,
		G_GNUC_UNUSED gpointer      user_data) {
	gupnp_service_action_set (action, "SortCaps", G_TYPE_STRING, "", NULL);
	gupnp_service_action_return(action);
}

void get_system_update_id_cb (G_GNUC_UNUSED GUPnPService *service,
		G_GNUC_UNUSED GUPnPServiceAction         *action,
		G_GNUC_UNUSED gpointer      user_data) {
	gupnp_service_action_set (action, "Id", G_TYPE_UINT, 1, NULL);
	gupnp_service_action_return(action);
}


int main () {
	GMainLoop *main_loop;
	GUPnPContext *context;
	GUPnPRootDevice *dev;
	GError *error = NULL;
	GUPnPServiceInfo *content_directory, *connection_manager, *av_transport;
#if !GLIB_CHECK_VERSION(2,35,0)
	g_type_init ();
#endif


	context = gupnp_context_new (NULL, NULL, 0, &error);
	if (error) {
		g_printerr ("Error creating the GUPnP context: %s\n",error->message);
		g_error_free (error);

		return EXIT_FAILURE;
	}

	ip = gssdp_client_get_host_ip((GSSDPClient*) context);
	g_print("ip: %s\n", ip);
	dev = gupnp_root_device_new (context, "airplay.xml", ".");
	gupnp_root_device_set_available (dev, TRUE);

	content_directory = gupnp_device_info_get_service
		(GUPNP_DEVICE_INFO (dev), "urn:schemas-upnp-org:service:ContentDirectory:1");
	if (!content_directory) {
		g_printerr ("Cannot get ContentDirectory service\n");

		return EXIT_FAILURE;
	}

	connection_manager = gupnp_device_info_get_service
		(GUPNP_DEVICE_INFO (dev), "urn:schemas-upnp-org:service:ConnectionManager:1");
	if (!connection_manager) {
		g_printerr ("Cannot get ConnectionManager service\n");

		return EXIT_FAILURE;
	}

	av_transport = gupnp_device_info_get_service
		(GUPNP_DEVICE_INFO (dev), "urn:schemas-upnp-org:service:AVTransport:1");
	if (!av_transport) {
		g_printerr ("Cannot get AVTransport service\n");
		g_error_free(error);
		return EXIT_FAILURE;
	}

	gupnp_service_signals_autoconnect (GUPNP_SERVICE (content_directory), NULL, &error);
	if(error) {
		g_printerr ("failed to autoconnect content directory signals: %s\n", error->message);
		g_error_free(error);
		return EXIT_FAILURE;
	}

	main_loop = g_main_loop_new (NULL, FALSE);
	g_main_loop_run (main_loop);

	g_main_loop_unref (main_loop);
	g_object_unref (dev);
	g_object_unref (context);
	return EXIT_SUCCESS;
}

