/*
 * Copyright (c) 2011-2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <vconf/vconf.h>

#include "net_connection_private.h"

typedef struct _connection_handle_s
{
	connection_type_changed_cb type_changed_callback;
	connection_address_changed_cb ip_changed_callback;
	connection_address_changed_cb proxy_changed_callback;
	void *type_changed_user_data;
	void *ip_changed_user_data;
	void *proxy_changed_user_data;
} connection_handle_s;

static __thread GSList *conn_handle_list = NULL;

static int __connection_convert_net_state(int status)
{
	switch (status) {
	case VCONFKEY_NETWORK_CELLULAR:
		return CONNECTION_TYPE_CELLULAR;
	case VCONFKEY_NETWORK_WIFI:
		return CONNECTION_TYPE_WIFI;
	case VCONFKEY_NETWORK_ETHERNET:
		return CONNECTION_TYPE_ETHERNET;
	case VCONFKEY_NETWORK_BLUETOOTH:
		return CONNECTION_TYPE_BT;
	default:
		return CONNECTION_TYPE_DISCONNECTED;
	}
}

static int __connection_convert_cellular_state(int status)
{
	switch (status) {
	case VCONFKEY_NETWORK_CELLULAR_ON:
		return CONNECTION_CELLULAR_STATE_AVAILABLE;
	case VCONFKEY_NETWORK_CELLULAR_3G_OPTION_OFF:
		return CONNECTION_CELLULAR_STATE_CALL_ONLY_AVAILABLE;
	case VCONFKEY_NETWORK_CELLULAR_ROAMING_OFF:
		return CONNECTION_CELLULAR_STATE_ROAMING_OFF;
	case VCONFKEY_NETWORK_CELLULAR_FLIGHT_MODE:
		return CONNECTION_CELLULAR_STATE_FLIGHT_MODE;
	default:
		return CONNECTION_CELLULAR_STATE_OUT_OF_SERVICE;
	}
}

static bool __connection_check_handle_validity(connection_h connection)
{
	bool ret = false;

	if (connection == NULL)
		return false;

	if (g_slist_find(conn_handle_list, connection) != NULL) {
		CONNECTION_LOG(CONNECTION_INFO, "found (%p)\n", connection);
		ret = true;
	}

	return ret;
}

static connection_type_changed_cb
__connection_get_type_changed_callback(connection_handle_s *local_handle)
{
	return local_handle->type_changed_callback;
}

static void *__connection_get_type_changed_userdata(
					connection_handle_s *local_handle)
{
	return local_handle->type_changed_user_data;
}

static gboolean __connection_cb_type_changed_cb_idle(gpointer user_data)
{
	int state, status;
	void *data;
	connection_type_changed_cb callback;
	connection_handle_s *local_handle = (connection_handle_s *)user_data;

	if (__connection_check_handle_validity((connection_h)local_handle) != true)
		return FALSE;

	if (vconf_get_int(VCONFKEY_NETWORK_STATUS, &status) != 0)
		return FALSE;

	state = __connection_convert_net_state(status);
	CONNECTION_LOG(CONNECTION_INFO, "Network status changed (%d), %p\n",
			state, conn_handle_list);

	callback = __connection_get_type_changed_callback(local_handle);
	data = __connection_get_type_changed_userdata(local_handle);
	if (callback)
		callback(state, data);

	return FALSE;
}

static void __connection_cb_type_change_cb(keynode_t *node, void *user_data)
{
	GSList *list;
	connection_h handle;

	if (_connection_is_created() != true) {
		CONNECTION_LOG(CONNECTION_ERROR, "Application is not registered"
				"If multi-threaded, thread integrity be broken.\n");
		return;
	}

	for (list = conn_handle_list; list; list = list->next) {
		CONNECTION_LOG(CONNECTION_INFO, "list %p, conn %p\n", list, conn_handle_list);
		handle = (connection_h)list->data;
		_connectioin_callback_add(__connection_cb_type_changed_cb_idle, (gpointer)handle);
	}
}

static int __connection_set_type_changed_callback(connection_h connection,
							void *callback, void *user_data)
{
	static __thread gint refcount = 0;
	connection_handle_s *local_handle;

	local_handle = (connection_handle_s *)connection;

	if (callback) {
		if (refcount == 0)
			vconf_notify_key_changed(VCONFKEY_NETWORK_STATUS,
					__connection_cb_type_change_cb, NULL);

		refcount++;
		CONNECTION_LOG(CONNECTION_INFO, "Successfully registered(%d)\n", refcount);
	} else {
		if (refcount > 0 &&
				__connection_get_type_changed_callback(local_handle) != NULL) {
			if (--refcount == 0) {
				if (vconf_ignore_key_changed(VCONFKEY_NETWORK_STATUS,
						__connection_cb_type_change_cb) < 0) {
					CONNECTION_LOG(CONNECTION_ERROR,
							"Error to de-register vconf callback(%d)\n", refcount);
				} else {
					CONNECTION_LOG(CONNECTION_INFO,
							"Successfully de-registered(%d)\n", refcount);
				}
			}
		}
	}

	local_handle->type_changed_user_data = user_data;
	local_handle->type_changed_callback = callback;

	return CONNECTION_ERROR_NONE;
}

static connection_address_changed_cb
__connection_get_ip_changed_callback(connection_handle_s *local_handle)
{
	return local_handle->ip_changed_callback;
}

static void *__connection_get_ip_changed_userdata(
					connection_handle_s *local_handle)
{
	return local_handle->ip_changed_user_data;
}

static gboolean __connection_cb_ip_changed_cb_idle(gpointer user_data)
{
	char *ip_addr;
	void *data;
	connection_address_changed_cb callback;
	connection_handle_s *local_handle = (connection_handle_s *)user_data;

	if (__connection_check_handle_validity((connection_h)local_handle) != true)
		return FALSE;

	ip_addr = vconf_get_str(VCONFKEY_NETWORK_IP);
	CONNECTION_LOG(CONNECTION_INFO, "IP changed (%s)\n", ip_addr);

	callback = __connection_get_ip_changed_callback(local_handle);
	data = __connection_get_ip_changed_userdata(local_handle);
	/* TODO: IPv6 should be supported */
	if (callback)
		callback(ip_addr, NULL, data);

	return FALSE;
}

static void __connection_cb_ip_change_cb(keynode_t *node, void *user_data)
{
	GSList *list;
	connection_h handle;

	if (_connection_is_created() != true) {
		CONNECTION_LOG(CONNECTION_ERROR, "Application is not registered"
				"If multi-threaded, thread integrity be broken.\n");
		return;
	}

	for (list = conn_handle_list; list; list = list->next) {
		CONNECTION_LOG(CONNECTION_INFO, "list %p, conn %p\n", list, conn_handle_list);
		handle = (connection_h)list->data;
		_connectioin_callback_add(__connection_cb_ip_changed_cb_idle, (gpointer)handle);
	}
}

static int __connection_set_ip_changed_callback(connection_h connection,
							void *callback, void *user_data)
{
	static __thread gint refcount = 0;
	connection_handle_s *local_handle;

	local_handle = (connection_handle_s *)connection;

	if (callback) {
		if (refcount == 0)
			vconf_notify_key_changed(VCONFKEY_NETWORK_IP,
					__connection_cb_ip_change_cb, NULL);

		refcount++;
		CONNECTION_LOG(CONNECTION_INFO, "Successfully registered(%d)\n", refcount);
	} else {
		if (refcount > 0 &&
				__connection_get_ip_changed_callback(local_handle) != NULL) {
			if (--refcount == 0) {
				if (vconf_ignore_key_changed(VCONFKEY_NETWORK_IP,
						__connection_cb_ip_change_cb) < 0) {
					CONNECTION_LOG(CONNECTION_ERROR,
							"Error to de-register vconf callback(%d)\n", refcount);
				} else {
					CONNECTION_LOG(CONNECTION_INFO,
							"Successfully de-registered(%d)\n", refcount);
				}
			}
		}
	}

	local_handle->ip_changed_user_data = user_data;
	local_handle->ip_changed_callback = callback;

	return CONNECTION_ERROR_NONE;
}

static connection_address_changed_cb
__connection_get_proxy_changed_callback(connection_handle_s *local_handle)
{
	return local_handle->proxy_changed_callback;
}

static void *__connection_get_proxy_changed_userdata(
							connection_handle_s *local_handle)
{
	return local_handle->proxy_changed_user_data;
}

static gboolean __connection_cb_proxy_changed_cb_idle(gpointer user_data)
{
	char *proxy;
	void *data;
	connection_address_changed_cb callback;
	connection_handle_s *local_handle = (connection_handle_s *)user_data;

	if (__connection_check_handle_validity((connection_h)local_handle) != true)
		return FALSE;

	proxy = vconf_get_str(VCONFKEY_NETWORK_PROXY);
	CONNECTION_LOG(CONNECTION_INFO, "Proxy changed (%s)\n", proxy);

	callback = __connection_get_proxy_changed_callback(local_handle);
	data = __connection_get_proxy_changed_userdata(local_handle);
	/* TODO: IPv6 should be supported */
	if (callback)
		callback(proxy, NULL, data);

	return FALSE;
}

static void __connection_cb_proxy_change_cb(keynode_t *node, void *user_data)
{
	GSList *list;
	connection_h handle;

	if (_connection_is_created() != true) {
		CONNECTION_LOG(CONNECTION_ERROR, "Application is not registered"
				"If multi-threaded, thread integrity be broken.\n");
		return;
	}

	for (list = conn_handle_list; list; list = list->next) {
		CONNECTION_LOG(CONNECTION_INFO, "list %p, conn %p\n", list, conn_handle_list);
		handle = (connection_h)list->data;
		_connectioin_callback_add(__connection_cb_proxy_changed_cb_idle, (gpointer)handle);
	}
}

static int __connection_set_proxy_changed_callback(connection_h connection,
							void *callback, void *user_data)
{
	static __thread gint refcount = 0;
	connection_handle_s *local_handle;

	local_handle = (connection_handle_s *)connection;

	if (callback) {
		if (refcount == 0)
			vconf_notify_key_changed(VCONFKEY_NETWORK_PROXY,
					__connection_cb_proxy_change_cb, NULL);

		refcount++;
		CONNECTION_LOG(CONNECTION_INFO, "Successfully registered(%d)\n", refcount);
	} else {
		if (refcount > 0 &&
				__connection_get_proxy_changed_callback(local_handle) != NULL) {
			if (--refcount == 0) {
				if (vconf_ignore_key_changed(VCONFKEY_NETWORK_PROXY,
						__connection_cb_proxy_change_cb) < 0) {
					CONNECTION_LOG(CONNECTION_ERROR,
							"Error to de-register vconf callback(%d)\n", refcount);
				} else {
					CONNECTION_LOG(CONNECTION_INFO,
							"Successfully de-registered(%d)\n", refcount);
				}
			}
		}
	}

	local_handle->proxy_changed_user_data = user_data;
	local_handle->proxy_changed_callback = callback;

	return CONNECTION_ERROR_NONE;
}

static int __connection_get_handle_count(void)
{
	return ((int)g_slist_length(conn_handle_list));
}

/* Connection Manager ********************************************************/
EXPORT_API int connection_create(connection_h *connection)
{
	if (connection == NULL || __connection_check_handle_validity(*connection)) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (_connection_libnet_init() == false) {
		CONNECTION_LOG(CONNECTION_ERROR, "Creation failed!\n");
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	CONNECTION_LOG(CONNECTION_ERROR, "Connection successfully created!\n");

	*connection = g_try_malloc0(sizeof(connection_handle_s));
	if (*connection != NULL)
		CONNECTION_LOG(CONNECTION_INFO, "New Handle Created %p\n", *connection);
	else
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	conn_handle_list = g_slist_prepend(conn_handle_list, *connection);

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_destroy(connection_h connection)
{
	if (!(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	CONNECTION_LOG(CONNECTION_ERROR, "Destroy handle: %p\n", connection);

	__connection_set_type_changed_callback(connection, NULL, NULL);
	__connection_set_ip_changed_callback(connection, NULL, NULL);
	__connection_set_proxy_changed_callback(connection, NULL, NULL);

	conn_handle_list = g_slist_remove(conn_handle_list, connection);

	g_free(connection);
	connection = NULL;

	if (__connection_get_handle_count() == 0) {
		_connection_libnet_deinit();
		_connection_callback_cleanup();
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_type(connection_h connection, connection_type_e* type)
{
	int status = 0;

	if (type == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (vconf_get_int(VCONFKEY_NETWORK_STATUS, &status)) {
		CONNECTION_LOG(CONNECTION_ERROR, "vconf_get_int Failed = %d\n", status);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	CONNECTION_LOG(CONNECTION_INFO, "Connected Network = %d\n", status);

	*type = __connection_convert_net_state(status);

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_ip_address(connection_h connection,
				connection_address_family_e address_family, char** ip_address)
{
	if (ip_address == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	switch (address_family) {
	case CONNECTION_ADDRESS_FAMILY_IPV4:
		*ip_address = vconf_get_str(VCONFKEY_NETWORK_IP);
		break;
	case CONNECTION_ADDRESS_FAMILY_IPV6:
		CONNECTION_LOG(CONNECTION_ERROR, "Not supported yet\n");
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;
		break;
	default:
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (*ip_address == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "vconf_get_str Failed\n");
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_proxy(connection_h connection,
				connection_address_family_e address_family, char** proxy)
{
	if (proxy == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	switch (address_family) {
	case CONNECTION_ADDRESS_FAMILY_IPV4:
		*proxy = vconf_get_str(VCONFKEY_NETWORK_PROXY);
		break;
	case CONNECTION_ADDRESS_FAMILY_IPV6:
		CONNECTION_LOG(CONNECTION_ERROR, "Not supported yet\n");
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;
		break;
	default:
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (*proxy == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "vconf_get_str Failed\n");
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_cellular_state(connection_h connection, connection_cellular_state_e* state)
{
	int status = 0;
	int cellular_state = 0;

	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (!vconf_get_int(VCONFKEY_NETWORK_CELLULAR_STATE, &status)) {
		CONNECTION_LOG(CONNECTION_INFO, "Cellular = %d\n", status);
		*state = __connection_convert_cellular_state(status);

		if (*state == CONNECTION_CELLULAR_STATE_AVAILABLE) {
			if (vconf_get_int(VCONFKEY_DNET_STATE, &cellular_state)) {
				CONNECTION_LOG(CONNECTION_ERROR,
						"vconf_get_int Failed = %d\n", cellular_state);
				return CONNECTION_ERROR_OPERATION_FAILED;
			}
		}

		CONNECTION_LOG(CONNECTION_INFO, "Connection state = %d\n", cellular_state);

		if (cellular_state == VCONFKEY_DNET_NORMAL_CONNECTED ||
		    cellular_state == VCONFKEY_DNET_SECURE_CONNECTED ||
		    cellular_state == VCONFKEY_DNET_TRANSFER)
			*state = CONNECTION_CELLULAR_STATE_CONNECTED;

		return CONNECTION_ERROR_NONE;
	} else {
		CONNECTION_LOG(CONNECTION_ERROR, "vconf_get_int Failed = %d\n", status);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
}

EXPORT_API int connection_get_wifi_state(connection_h connection, connection_wifi_state_e* state)
{
	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (_connection_libnet_get_wifi_state(state) == false) {
		CONNECTION_LOG(CONNECTION_ERROR, "Fail to get wifi state\n");
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	CONNECTION_LOG(CONNECTION_INFO, "WiFi state = %d\n", *state);

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_ethernet_state(connection_h connection, connection_ethernet_state_e* state)
{
	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (_connection_libnet_get_ethernet_state(state) == false)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_bt_state(connection_h connection, connection_bt_state_e* state)
{
	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (_connection_libnet_get_bluetooth_state(state) == false)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_set_type_changed_cb(connection_h connection,
					connection_type_changed_cb callback, void* user_data)
{
	if (callback == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_type_changed_callback(connection, callback, user_data);
}

EXPORT_API int connection_unset_type_changed_cb(connection_h connection)
{
	if (!(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_type_changed_callback(connection, NULL, NULL);
}

EXPORT_API int connection_set_ip_address_changed_cb(connection_h connection,
				connection_address_changed_cb callback, void* user_data)
{
	if (callback == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_ip_changed_callback(connection, callback, user_data);
}

EXPORT_API int connection_unset_ip_address_changed_cb(connection_h connection)
{
	if (!(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_ip_changed_callback(connection, NULL, NULL);
}

EXPORT_API int connection_set_proxy_address_changed_cb(connection_h connection,
				connection_address_changed_cb callback, void* user_data)
{
	if (callback == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_proxy_changed_callback(connection, callback, user_data);
}

EXPORT_API int connection_unset_proxy_address_changed_cb(connection_h connection)
{
	if (!(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_proxy_changed_callback(connection, NULL, NULL);
}

EXPORT_API int connection_add_profile(connection_h connection, connection_profile_h profile)
{
	if (!(__connection_check_handle_validity(connection)) ||
	    !(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	int rv = 0;

	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	rv = net_add_profile(profile_info->ProfileInfo.Pdp.ServiceType, (net_profile_info_t*)profile);
	if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "net_add_profile Failed = %d\n", rv);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_remove_profile(connection_h connection, connection_profile_h profile)
{
	if (!(__connection_check_handle_validity(connection)) ||
	    !(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	int rv = 0;
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR &&
	    profile_info->profile_type != NET_DEVICE_WIFI) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	rv = net_delete_profile(profile_info->ProfileName);
	if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "net_delete_profile Failed = %d\n", rv);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_update_profile(connection_h connection, connection_profile_h profile)
{
	if (!(__connection_check_handle_validity(connection)) ||
	    !(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	int rv = 0;
	net_profile_info_t *profile_info = profile;

	rv = net_modify_profile(profile_info->ProfileName, (net_profile_info_t*)profile);
	if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "net_modify_profile Failed = %d\n", rv);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_profile_iterator(connection_h connection,
		connection_iterator_type_e type, connection_profile_iterator_h* profile_iterator)
{
	if (!(__connection_check_handle_validity(connection)) ||
	    (type != CONNECTION_ITERATOR_TYPE_REGISTERED &&
	     type != CONNECTION_ITERATOR_TYPE_CONNECTED)) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_get_profile_iterator(type, profile_iterator);
}

EXPORT_API int connection_profile_iterator_next(connection_profile_iterator_h profile_iterator,
							connection_profile_h* profile)
{
	return _connection_libnet_get_iterator_next(profile_iterator, profile);
}

EXPORT_API bool connection_profile_iterator_has_next(connection_profile_iterator_h profile_iterator)
{
	return _connection_libnet_iterator_has_next(profile_iterator);
}

EXPORT_API int connection_destroy_profile_iterator(connection_profile_iterator_h profile_iterator)
{
	return _connection_libnet_destroy_iterator(profile_iterator);
}

EXPORT_API int connection_get_current_profile(connection_h connection, connection_profile_h* profile)
{
	if (!(__connection_check_handle_validity(connection)) || profile == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_get_current_profile(profile);
}

EXPORT_API int connection_get_default_cellular_service_profile(connection_h connection,
		connection_cellular_service_type_e type, connection_profile_h* profile)
{
	if (!(__connection_check_handle_validity(connection)) || profile == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_get_cellular_service_profile(type, profile);
}

EXPORT_API int connection_set_default_cellular_service_profile(connection_h connection,
		connection_cellular_service_type_e type, connection_profile_h profile)
{
	if (!(__connection_check_handle_validity(connection)) || profile == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_set_cellular_service_profile_sync(type, profile);
}

EXPORT_API int connection_set_default_cellular_service_profile_async(connection_h connection,
		connection_cellular_service_type_e type, connection_profile_h profile,
		connection_set_default_cb callback, void* user_data)
{
	if (!(__connection_check_handle_validity(connection)) ||
	    profile == NULL || callback == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_set_cellular_service_profile_async(type, profile, callback, user_data);
}

EXPORT_API int connection_open_profile(connection_h connection, connection_profile_h profile,
					connection_opened_cb callback, void* user_data)
{
	if (!(__connection_check_handle_validity(connection)) ||
	    profile == NULL || callback == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_open_profile(profile, callback, user_data);
}

EXPORT_API int connection_close_profile(connection_h connection, connection_profile_h profile,
					connection_closed_cb callback, void* user_data)
{
	if (!(__connection_check_handle_validity(connection)) ||
	    profile == NULL || callback == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_close_profile(profile, callback, user_data);
}

EXPORT_API int connection_add_route(connection_h connection, const char* interface_name, const char* host_address)
{
	if (!(__connection_check_handle_validity(connection)) ||
	    interface_name == NULL || host_address == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_add_route(interface_name, host_address);
}

static int __get_statistic(connection_type_e connection_type,
			connection_statistics_type_e statistics_type, long long* llsize)
{
	int size;
	unsigned long long ull_size;
	int stat_type;
	char *key = NULL;

	if (llsize == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (connection_type == CONNECTION_TYPE_CELLULAR) {
		switch (statistics_type) {
		case CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA:
			key = VCONFKEY_NETWORK_CELLULAR_PKT_LAST_SNT;
			break;
		case CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA:
			key = VCONFKEY_NETWORK_CELLULAR_PKT_LAST_RCV;
			break;
		case CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA:
			key = VCONFKEY_NETWORK_CELLULAR_PKT_TOTAL_SNT;
			break;
		case CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA:
			key = VCONFKEY_NETWORK_CELLULAR_PKT_TOTAL_RCV;
			break;
		default:
			return CONNECTION_ERROR_INVALID_PARAMETER;
		}

		if (vconf_get_int(key, &size)) {
			CONNECTION_LOG(CONNECTION_ERROR, "Cannot Get %s = %d\n", key, size);
			*llsize = 0;
			return CONNECTION_ERROR_OPERATION_FAILED;
		}

		CONNECTION_LOG(CONNECTION_INFO,"%s:%d bytes\n", key, size);
		*llsize = (long long)size;
	} else if (connection_type == CONNECTION_TYPE_WIFI) {
		switch (statistics_type) {
		case CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA:
			stat_type = NET_STATISTICS_TYPE_LAST_SENT_DATA;
			break;
		case CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA:
			stat_type = NET_STATISTICS_TYPE_LAST_RECEIVED_DATA;
			break;
		case CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA:
			stat_type = NET_STATISTICS_TYPE_TOTAL_SENT_DATA;
			break;
		case CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA:
			stat_type = NET_STATISTICS_TYPE_TOTAL_RECEIVED_DATA;
			break;
		default:
			return CONNECTION_ERROR_INVALID_PARAMETER;
		}

		if (_connection_libnet_get_statistics(stat_type, &ull_size) != CONNECTION_ERROR_NONE) {
			CONNECTION_LOG(CONNECTION_ERROR, "Cannot Get Wi-Fi statistics : %d\n", ull_size);
			*llsize = 0;
			return CONNECTION_ERROR_OPERATION_FAILED;
		}

		CONNECTION_LOG(CONNECTION_INFO,"%lld bytes\n", ull_size);
		*llsize = (long long)ull_size;
	} else
		return CONNECTION_ERROR_INVALID_PARAMETER;

	return CONNECTION_ERROR_NONE;
}

static int __reset_statistic(connection_type_e connection_type,
			connection_statistics_type_e statistics_type)
{
	int conn_type;
	int stat_type;
	int rv;

	if (connection_type == CONNECTION_TYPE_CELLULAR)
		conn_type = NET_DEVICE_CELLULAR;
	else if (connection_type == CONNECTION_TYPE_WIFI)
		conn_type = NET_DEVICE_WIFI;
	else
		return CONNECTION_ERROR_INVALID_PARAMETER;

	switch (statistics_type) {
	case CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA:
		stat_type = NET_STATISTICS_TYPE_LAST_SENT_DATA;
		break;
	case CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA:
		stat_type = NET_STATISTICS_TYPE_LAST_RECEIVED_DATA;
		break;
	case CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA:
		stat_type = NET_STATISTICS_TYPE_TOTAL_SENT_DATA;
		break;
	case CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA:
		stat_type = NET_STATISTICS_TYPE_TOTAL_RECEIVED_DATA;
		break;
	default:
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	rv = _connection_libnet_set_statistics(conn_type, stat_type);
	if (rv != CONNECTION_ERROR_NONE)
		return rv;

	CONNECTION_LOG(CONNECTION_INFO,"connection_reset_statistics success\n");

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_statistics(connection_type_e connection_type,
				connection_statistics_type_e statistics_type, long long* size)
{
	return __get_statistic(connection_type, statistics_type, size);
}

EXPORT_API int connection_reset_statistics(connection_type_e connection_type,
				connection_statistics_type_e statistics_type)
{
	return __reset_statistic(connection_type, statistics_type);
}
