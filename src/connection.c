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

static __thread GSList *conn_handle_list = NULL;

//LCOV_EXCL_START
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
	case VCONFKEY_NETWORK_DEFAULT_PROXY:
		return CONNECTION_TYPE_NET_PROXY;
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

	if (g_slist_find(conn_handle_list, connection) != NULL)
		ret = true;

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
				"If multi-threaded, thread integrity be broken.");
		return;
	}

	for (list = conn_handle_list; list; list = list->next) {
		handle = (connection_h)list->data;
		_connection_callback_add(__connection_cb_type_changed_cb_idle, (gpointer)handle);
	}
}

static void __connection_cb_ethernet_cable_state_changed_cb(connection_ethernet_cable_state_e state)
{
	CONNECTION_LOG(CONNECTION_INFO, "Ethernet Cable state Indication");

	GSList *list;

	for (list = conn_handle_list; list; list = list->next) {
		connection_handle_s *local_handle = (connection_handle_s *)list->data;
		if (local_handle->ethernet_cable_state_changed_callback)
			local_handle->ethernet_cable_state_changed_callback(state,
					local_handle->ethernet_cable_state_changed_user_data);
	}
}

static int __connection_get_ethernet_cable_state_changed_callback_count(void)
{
	GSList *list;
	int count = 0;

	for (list = conn_handle_list; list; list = list->next) {
		connection_handle_s *local_handle = (connection_handle_s *)list->data;
		if (local_handle->ethernet_cable_state_changed_callback) count++;
	}

	return count;
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
		CONNECTION_LOG(CONNECTION_INFO, "Successfully registered(%d)", refcount);
	} else {
		if (refcount > 0 &&
				__connection_get_type_changed_callback(local_handle) != NULL) {
			if (--refcount == 0) {
				if (vconf_ignore_key_changed(VCONFKEY_NETWORK_STATUS,
						__connection_cb_type_change_cb) < 0) {
					CONNECTION_LOG(CONNECTION_ERROR, //LCOV_EXCL_LINE
							"Error to de-register vconf callback(%d)", refcount);
				} else {
					CONNECTION_LOG(CONNECTION_INFO,
							"Successfully de-registered(%d)", refcount);
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
	if (ip_addr == NULL)
		CONNECTION_LOG(CONNECTION_ERROR, //LCOV_EXCL_LINE
			"vconf_get_str(VCONFKEY_NETWORK_IP) is Failed");

	callback = __connection_get_ip_changed_callback(local_handle);
	data = __connection_get_ip_changed_userdata(local_handle);
	/* TODO: IPv6 should be supported */
	if (callback)
		callback(ip_addr, NULL, data);

	g_free(ip_addr);

	return FALSE;
}

static void __connection_cb_ip_change_cb(keynode_t *node, void *user_data)
{
	GSList *list;
	connection_h handle;

	if (_connection_is_created() != true) {
		CONNECTION_LOG(CONNECTION_ERROR, "Application is not registered"
				"If multi-threaded, thread integrity be broken.");
		return;
	}

	for (list = conn_handle_list; list; list = list->next) {
		handle = (connection_h)list->data;
		_connection_callback_add(__connection_cb_ip_changed_cb_idle, (gpointer)handle);
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
		CONNECTION_LOG(CONNECTION_INFO, "Successfully registered(%d)", refcount);
	} else {
		if (refcount > 0 &&
				__connection_get_ip_changed_callback(local_handle) != NULL) {
			if (--refcount == 0) {
				if (vconf_ignore_key_changed(VCONFKEY_NETWORK_IP,
						__connection_cb_ip_change_cb) < 0) {
					CONNECTION_LOG(CONNECTION_ERROR, //LCOV_EXCL_LINE
							"Error to de-register vconf callback(%d)", refcount);
				} else {
					CONNECTION_LOG(CONNECTION_INFO,
							"Successfully de-registered(%d)", refcount);
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
	if (ip_addr == NULL)
		CONNECTION_LOG(CONNECTION_ERROR, //LCOV_EXCL_LINE
			"vconf_get_str(VCONFKEY_NETWORK_PROXY) is Failed");

	callback = __connection_get_proxy_changed_callback(local_handle);
	data = __connection_get_proxy_changed_userdata(local_handle);
	/* TODO: IPv6 should be supported */
	if (callback)
		callback(proxy, NULL, data);

	g_free(proxy);

	return FALSE;
}

static void __connection_cb_proxy_change_cb(keynode_t *node, void *user_data)
{
	GSList *list;
	connection_h handle;

	if (_connection_is_created() != true) {
		CONNECTION_LOG(CONNECTION_ERROR, "Application is not registered"
				"If multi-threaded, thread integrity be broken.");
		return;
	}

	for (list = conn_handle_list; list; list = list->next) {
		handle = (connection_h)list->data;
		_connection_callback_add(__connection_cb_proxy_changed_cb_idle, (gpointer)handle);
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
		CONNECTION_LOG(CONNECTION_INFO, "Successfully registered(%d)", refcount);
	} else {
		if (refcount > 0 &&
				__connection_get_proxy_changed_callback(local_handle) != NULL) {
			if (--refcount == 0) {
				if (vconf_ignore_key_changed(VCONFKEY_NETWORK_PROXY,
						__connection_cb_proxy_change_cb) < 0) {
					CONNECTION_LOG(CONNECTION_ERROR, //LCOV_EXCL_LINE
							"Error to de-register vconf callback(%d)", refcount);
				} else {
					CONNECTION_LOG(CONNECTION_INFO,
							"Successfully de-registered(%d)", refcount);
				}
			}
		}
	}

	local_handle->proxy_changed_user_data = user_data;
	local_handle->proxy_changed_callback = callback;

	return CONNECTION_ERROR_NONE;
}

static int __connection_set_ethernet_cable_state_changed_cb(connection_h connection,
		connection_ethernet_cable_state_chaged_cb callback, void *user_data)
{
	connection_handle_s *local_handle = (connection_handle_s *)connection;

	if (callback) {
		if (__connection_get_ethernet_cable_state_changed_callback_count() == 0)
			_connection_libnet_set_ethernet_cable_state_changed_cb(
					__connection_cb_ethernet_cable_state_changed_cb);

	} else {
		if (__connection_get_ethernet_cable_state_changed_callback_count() == 1)
			_connection_libnet_set_ethernet_cable_state_changed_cb(NULL);
	}

	local_handle->ethernet_cable_state_changed_callback = callback;
	local_handle->ethernet_cable_state_changed_user_data = user_data;
	return CONNECTION_ERROR_NONE;
}
//LCOV_EXCL_STOP

static int __connection_get_handle_count(void)
{
	return ((int)g_slist_length(conn_handle_list));
}

/* Connection Manager ********************************************************/
EXPORT_API int connection_create(connection_h *connection)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (connection == NULL || __connection_check_handle_validity(*connection)) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	int rv = _connection_libnet_init();
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_PERMISSION_DENIED; //LCOV_EXCL_LINE
	} else if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to create connection[%d]", rv); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_OPERATION_FAILED; //LCOV_EXCL_LINE
	}

	*connection = g_try_malloc0(sizeof(connection_handle_s));
	if (*connection != NULL)
		CONNECTION_LOG(CONNECTION_INFO, "New handle created[%p]", *connection);
	else
		return CONNECTION_ERROR_OUT_OF_MEMORY; //LCOV_EXCL_LINE

	conn_handle_list = g_slist_prepend(conn_handle_list, *connection);

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_destroy(connection_h connection)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	CONNECTION_LOG(CONNECTION_INFO, "Destroy handle: %p", connection);

	__connection_set_type_changed_callback(connection, NULL, NULL);
	__connection_set_ip_changed_callback(connection, NULL, NULL);
	__connection_set_proxy_changed_callback(connection, NULL, NULL);
	__connection_set_ethernet_cable_state_changed_cb(connection, NULL, NULL);

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
	int rv = 0;
	int status = 0;

	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (type == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	rv = vconf_get_int(VCONFKEY_NETWORK_STATUS, &status);
	if (rv != VCONF_OK) {
		CONNECTION_LOG(CONNECTION_ERROR, "vconf_get_int Failed = %d", status); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_OPERATION_FAILED; //LCOV_EXCL_LINE
	}

	CONNECTION_LOG(CONNECTION_INFO, "Connected Network = %d", status);

	*type = __connection_convert_net_state(status);

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_ip_address(connection_h connection,
				connection_address_family_e address_family, char** ip_address)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (ip_address == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	switch (address_family) {
	case CONNECTION_ADDRESS_FAMILY_IPV4:
	case CONNECTION_ADDRESS_FAMILY_IPV6:
		*ip_address = vconf_get_str(VCONFKEY_NETWORK_IP);
		break;
	default:
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (*ip_address == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "vconf_get_str Failed"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_OPERATION_FAILED;//LCOV_EXCL_LINE
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_proxy(connection_h connection,
				connection_address_family_e address_family, char** proxy)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (proxy == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	switch (address_family) {
	case CONNECTION_ADDRESS_FAMILY_IPV4:
	case CONNECTION_ADDRESS_FAMILY_IPV6:
		*proxy = vconf_get_str(VCONFKEY_NETWORK_PROXY);
		break;
	default:
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (*proxy == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "vconf_get_str Failed"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_OPERATION_FAILED; //LCOV_EXCL_LINE
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_mac_address(connection_h connection, connection_type_e type, char** mac_addr)
{
	FILE *fp;
	char buf[CONNECTION_MAC_INFO_LENGTH + 1];

	CHECK_FEATURE_SUPPORTED(WIFI_FEATURE, ETHERNET_FEATURE);

	if (type == CONNECTION_TYPE_WIFI)
		CHECK_FEATURE_SUPPORTED(WIFI_FEATURE);
	else if (type == CONNECTION_TYPE_ETHERNET) //LCOV_EXCL_LINE
		CHECK_FEATURE_SUPPORTED(ETHERNET_FEATURE); //LCOV_EXCL_LINE

	if (mac_addr == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	switch (type) {
	case CONNECTION_TYPE_WIFI:
#if defined TIZEN_TV
		fp = fopen(WIFI_MAC_INFO_FILE, "r");
		if (fp == NULL) {
			CONNECTION_LOG(CONNECTION_ERROR, "Failed to open file %s", WIFI_MAC_INFO_FILE); //LCOV_EXCL_LINE
			return CONNECTION_ERROR_OUT_OF_MEMORY; //LCOV_EXCL_LINE
		}

		if (fgets(buf, sizeof(buf), fp) == NULL) {
			CONNECTION_LOG(CONNECTION_ERROR, "Failed to get MAC info from %s", WIFI_MAC_INFO_FILE); //LCOV_EXCL_LINE
			fclose(fp); //LCOV_EXCL_LINE
			return CONNECTION_ERROR_OPERATION_FAILED;
		}

		CONNECTION_LOG(CONNECTION_INFO, "%s : %s", WIFI_MAC_INFO_FILE, buf);

		*mac_addr = (char *)malloc(CONNECTION_MAC_INFO_LENGTH + 1);
		if (*mac_addr == NULL) {
			CONNECTION_LOG(CONNECTION_ERROR, "malloc() failed"); //LCOV_EXCL_LINE
			fclose(fp); //LCOV_EXCL_LINE
			return CONNECTION_ERROR_OUT_OF_MEMORY; //LCOV_EXCL_LINE
		}
		g_strlcpy(*mac_addr, buf, CONNECTION_MAC_INFO_LENGTH + 1);
		fclose(fp);
#else
		*mac_addr = vconf_get_str(VCONFKEY_WIFI_BSSID_ADDRESS);

		if (*mac_addr == NULL) {
			CONNECTION_LOG(CONNECTION_ERROR, "Failed to get vconf from %s", VCONFKEY_WIFI_BSSID_ADDRESS); //LCOV_EXCL_LINE
			return CONNECTION_ERROR_OPERATION_FAILED; //LCOV_EXCL_LINE
		}
#endif
		break;
	//LCOV_EXCL_START
	case CONNECTION_TYPE_ETHERNET:
		fp = fopen(ETHERNET_MAC_INFO_FILE, "r");
		if (fp == NULL) {
			CONNECTION_LOG(CONNECTION_ERROR, "Failed to open file %s", ETHERNET_MAC_INFO_FILE);
			return CONNECTION_ERROR_OUT_OF_MEMORY;
		}

		if (fgets(buf, sizeof(buf), fp) == NULL) {
			CONNECTION_LOG(CONNECTION_ERROR, "Failed to get MAC info from %s", ETHERNET_MAC_INFO_FILE);
			fclose(fp);
			return CONNECTION_ERROR_OPERATION_FAILED;
		}

		CONNECTION_LOG(CONNECTION_INFO, "%s : %s", ETHERNET_MAC_INFO_FILE, buf);

		*mac_addr = (char *)malloc(CONNECTION_MAC_INFO_LENGTH + 1);
		if (*mac_addr == NULL) {
			CONNECTION_LOG(CONNECTION_ERROR, "malloc() failed");
			fclose(fp);
			return CONNECTION_ERROR_OUT_OF_MEMORY;
		}

		g_strlcpy(*mac_addr, buf, CONNECTION_MAC_INFO_LENGTH + 1);
		fclose(fp);

		break;
	//LCOV_EXCL_STOP
	default :
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_INVALID_PARAMETER; //LCOV_EXCL_LINE
	}

	/* Checking Invalid MAC Address */
	if ((strcmp(*mac_addr, "00:00:00:00:00:00") == 0) ||
			(strcmp(*mac_addr, "ff:ff:ff:ff:ff:ff") == 0)) {
		CONNECTION_LOG(CONNECTION_ERROR, "MAC Address(%s) is invalid", *mac_addr); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_INVALID_OPERATION; //LCOV_EXCL_LINE
	}

	CONNECTION_LOG(CONNECTION_INFO, "MAC Address %s", *mac_addr);

	return CONNECTION_ERROR_NONE;
}


EXPORT_API int connection_get_cellular_state(connection_h connection, connection_cellular_state_e* state)
{
	int rv = 0;
	int status = 0;
	int cellular_state = 0;
#if defined TIZEN_DUALSIM_ENABLE
	int sim_id = 0;
#endif

	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE);

	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	rv = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_STATE, &status);
	if (rv != VCONF_OK) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to get cellular state"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_OPERATION_FAILED; //LCOV_EXCL_LINE
	}

	CONNECTION_LOG(CONNECTION_INFO, "Cellular: %d", status);
	*state = __connection_convert_cellular_state(status);

	if (*state == CONNECTION_CELLULAR_STATE_AVAILABLE) {
#if defined TIZEN_DUALSIM_ENABLE
		rv = vconf_get_int(VCONF_TELEPHONY_DEFAULT_DATA_SERVICE, &sim_id);
		if (rv != VCONF_OK) {
			CONNECTION_LOG(CONNECTION_ERROR,
					"Failed to get default subscriber id", sim_id);
			return CONNECTION_ERROR_OPERATION_FAILED;
		}

		switch (sim_id) {
		case CONNECTION_CELLULAR_SUBSCRIBER_1:
#endif
			rv = vconf_get_int(VCONFKEY_DNET_STATE, &cellular_state);
#if defined TIZEN_DUALSIM_ENABLE
			break;

		case CONNECTION_CELLULAR_SUBSCRIBER_2:
			rv = vconf_get_int(VCONFKEY_DNET_STATE2, &cellular_state);
			break;

		default:
			CONNECTION_LOG(CONNECTION_ERROR, "Invalid subscriber id:%d", sim_id);
			return CONNECTION_ERROR_OPERATION_FAILED;
		}
#endif
		if (rv != VCONF_OK) {
			CONNECTION_LOG(CONNECTION_ERROR, "Failed to get cellular state"); //LCOV_EXCL_LINE
			return CONNECTION_ERROR_OPERATION_FAILED; //LCOV_EXCL_LINE
		}
	}

	CONNECTION_LOG(CONNECTION_INFO, "Cellular state: %d", cellular_state);

	if (cellular_state == VCONFKEY_DNET_NORMAL_CONNECTED ||
			cellular_state == VCONFKEY_DNET_SECURE_CONNECTED ||
			cellular_state == VCONFKEY_DNET_TRANSFER)
		*state = CONNECTION_CELLULAR_STATE_CONNECTED;

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_wifi_state(connection_h connection, connection_wifi_state_e* state)
{
	CHECK_FEATURE_SUPPORTED(WIFI_FEATURE);

	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	int rv = _connection_libnet_get_wifi_state(state);
	if (rv != CONNECTION_ERROR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Fail to get Wi-Fi state[%d]", rv); //LCOV_EXCL_LINE
		return rv; //LCOV_EXCL_LINE
	}

	CONNECTION_LOG(CONNECTION_INFO, "Wi-Fi state: %d", *state);

	return CONNECTION_ERROR_NONE;
}

//LCOV_EXCL_START
EXPORT_API int connection_get_ethernet_state(connection_h connection, connection_ethernet_state_e *state)
{
	CHECK_FEATURE_SUPPORTED(ETHERNET_FEATURE);

	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_get_ethernet_state(state);
}

EXPORT_API int connection_get_ethernet_cable_state(connection_h connection, connection_ethernet_cable_state_e *state)
{
	CHECK_FEATURE_SUPPORTED(ETHERNET_FEATURE);

	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_get_ethernet_cable_state(state);
}

EXPORT_API int connection_set_ethernet_cable_state_chaged_cb(connection_h connection,
			  connection_ethernet_cable_state_chaged_cb callback, void *user_data)
{
	CHECK_FEATURE_SUPPORTED(ETHERNET_FEATURE);

	if (callback == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_ethernet_cable_state_changed_cb(connection,
							callback, user_data);
}

EXPORT_API int connection_unset_ethernet_cable_state_chaged_cb(connection_h connection)
{
	CHECK_FEATURE_SUPPORTED(ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_ethernet_cable_state_changed_cb(connection,
							NULL, NULL);
}
//LCOV_EXCL_STOP

EXPORT_API int connection_get_bt_state(connection_h connection, connection_bt_state_e *state)
{
	CHECK_FEATURE_SUPPORTED(TETHERING_BLUETOOTH_FEATURE);

	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_get_bluetooth_state(state);
}

EXPORT_API int connection_set_type_changed_cb(connection_h connection,
					connection_type_changed_cb callback, void* user_data)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (callback == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_type_changed_callback(connection, callback, user_data);
}

EXPORT_API int connection_unset_type_changed_cb(connection_h connection)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_type_changed_callback(connection, NULL, NULL);
}

EXPORT_API int connection_set_ip_address_changed_cb(connection_h connection,
				connection_address_changed_cb callback, void* user_data)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (callback == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_ip_changed_callback(connection, callback, user_data);
}

EXPORT_API int connection_unset_ip_address_changed_cb(connection_h connection)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_ip_changed_callback(connection, NULL, NULL);
}

EXPORT_API int connection_set_proxy_address_changed_cb(connection_h connection,
				connection_address_changed_cb callback, void* user_data)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (callback == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_proxy_changed_callback(connection, callback, user_data);
}

EXPORT_API int connection_unset_proxy_address_changed_cb(connection_h connection)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_proxy_changed_callback(connection, NULL, NULL);
}

EXPORT_API int connection_add_profile(connection_h connection, connection_profile_h profile)
{
	int rv = 0;
	net_profile_info_t *profile_info = profile;

	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE);

	if (!(__connection_check_handle_validity(connection)) ||
	    !(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (profile_info->profile_type != NET_DEVICE_CELLULAR) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_INVALID_PARAMETER; //LCOV_EXCL_LINE
	}

	if (profile_info->ProfileInfo.Pdp.PSModemPath[0] != '/' ||
			strlen(profile_info->ProfileInfo.Pdp.PSModemPath) < 2) {
		CONNECTION_LOG(CONNECTION_ERROR, "Modem object path is NULL"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_INVALID_PARAMETER; //LCOV_EXCL_LINE
	}

	rv = net_add_profile(profile_info->ProfileInfo.Pdp.ServiceType,
							(net_profile_info_t*)profile);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_PERMISSION_DENIED; //LCOV_EXCL_LINE
	} else if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to add profile[%d]", rv); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_OPERATION_FAILED; //LCOV_EXCL_LINE
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_remove_profile(connection_h connection, connection_profile_h profile)
{
	int rv = 0;
	net_profile_info_t *profile_info = profile;

	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE);

	if (!(__connection_check_handle_validity(connection)) ||
			!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (profile_info->profile_type != NET_DEVICE_CELLULAR &&
	    profile_info->profile_type != NET_DEVICE_WIFI) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_INVALID_PARAMETER; //LCOV_EXCL_LINE
	}

	rv = net_delete_profile(profile_info->ProfileName);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_PERMISSION_DENIED; //LCOV_EXCL_LINE
	} else if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to delete profile[%d]", rv); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_OPERATION_FAILED; //LCOV_EXCL_LINE
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_update_profile(connection_h connection, connection_profile_h profile)
{
	int rv = 0;
	net_profile_info_t *profile_info = profile;

	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection)) ||
	    !(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	rv = net_modify_profile(profile_info->ProfileName, (net_profile_info_t*)profile);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_PERMISSION_DENIED; //LCOV_EXCL_LINE
	} else if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to modify profile[%d]", rv); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_OPERATION_FAILED; //LCOV_EXCL_LINE
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_profile_iterator(connection_h connection,
		connection_iterator_type_e type, connection_profile_iterator_h* profile_iterator)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection)) ||
	    (type != CONNECTION_ITERATOR_TYPE_REGISTERED &&
	     type != CONNECTION_ITERATOR_TYPE_CONNECTED &&
	     type != CONNECTION_ITERATOR_TYPE_DEFAULT)) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_get_profile_iterator(type, profile_iterator);
}

EXPORT_API int connection_profile_iterator_next(connection_profile_iterator_h profile_iterator,
							connection_profile_h* profile)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	return _connection_libnet_get_iterator_next(profile_iterator, profile);
}

EXPORT_API bool connection_profile_iterator_has_next(connection_profile_iterator_h profile_iterator)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	return _connection_libnet_iterator_has_next(profile_iterator);
}

EXPORT_API int connection_destroy_profile_iterator(connection_profile_iterator_h profile_iterator)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	return _connection_libnet_destroy_iterator(profile_iterator);
}

EXPORT_API int connection_get_current_profile(connection_h connection, connection_profile_h* profile)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection)) || profile == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_get_current_profile(profile);
}

EXPORT_API int connection_get_default_cellular_service_profile(
		connection_h connection, connection_cellular_service_type_e type,
		connection_profile_h *profile)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE);

	if (!(__connection_check_handle_validity(connection)) || profile == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_get_cellular_service_profile(type, profile);
}

EXPORT_API int connection_set_default_cellular_service_profile(connection_h connection,
		connection_cellular_service_type_e type, connection_profile_h profile)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE);

	if (!(__connection_check_handle_validity(connection)) || profile == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_set_cellular_service_profile_sync(type, profile);
}

EXPORT_API int connection_set_default_cellular_service_profile_async(connection_h connection,
		connection_cellular_service_type_e type, connection_profile_h profile,
		connection_set_default_cb callback, void* user_data)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE);

	if (!(__connection_check_handle_validity(connection)) ||
	    profile == NULL || callback == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_set_cellular_service_profile_async(type, profile, callback, user_data);
}

EXPORT_API int connection_open_profile(connection_h connection, connection_profile_h profile,
					connection_opened_cb callback, void* user_data)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE);

	if (!(__connection_check_handle_validity(connection)) ||
	    profile == NULL || callback == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_open_profile(profile, callback, user_data);
}

EXPORT_API int connection_close_profile(connection_h connection, connection_profile_h profile,
					connection_closed_cb callback, void* user_data)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE);

	if (!(__connection_check_handle_validity(connection)) ||
	    profile == NULL || callback == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_close_profile(profile, callback, user_data);
}

EXPORT_API int connection_reset_profile(connection_h connection,
				connection_reset_option_e type, int id, connection_reset_cb callback, void *user_data)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE);

	if (!(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (id < 0 || id > 1) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_INVALID_PARAMETER; //LCOV_EXCL_LINE
	}

	return _connection_libnet_reset_profile(type, id, callback, user_data);
}

EXPORT_API int connection_add_route(connection_h connection, const char* interface_name, const char* host_address)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection)) ||
	    interface_name == NULL || host_address == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_add_route(interface_name, host_address);
}

EXPORT_API int connection_remove_route(connection_h connection, const char* interface_name, const char* host_address)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, TETHERING_BLUETOOTH_FEATURE, ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection)) ||
	    interface_name == NULL || host_address == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_remove_route(interface_name, host_address);
}

EXPORT_API int connection_add_route_ipv6(connection_h connection, const char *interface_name, const char *host_address, const char * gateway)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection)) ||
	    interface_name == NULL || host_address == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_add_route_ipv6(interface_name, host_address, gateway);
}

EXPORT_API int connection_remove_route_ipv6(connection_h connection, const char *interface_name, const char *host_address, const char * gateway)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE, ETHERNET_FEATURE);

	if (!(__connection_check_handle_validity(connection)) ||
	    interface_name == NULL || host_address == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return _connection_libnet_remove_route_ipv6(interface_name, host_address, gateway);
}

static int __get_cellular_statistic(connection_statistics_type_e statistics_type, long long *llsize)
{
	int rv = VCONF_OK, rv1 = VCONF_OK;
	int last_size = 0, size = 0;
#if defined TIZEN_DUALSIM_ENABLE
	int sim_id = 0;
#endif

	if (llsize == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	switch (statistics_type) {
	case CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA:
	case CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA:
	case CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA:
	case CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA:
		break;
	default:
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

#if defined TIZEN_DUALSIM_ENABLE
	rv = vconf_get_int(VCONF_TELEPHONY_DEFAULT_DATA_SERVICE, &sim_id);
	if (rv != VCONF_OK) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to get default subscriber id");
		*llsize = 0;
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	switch (sim_id) {
	case 0:
#endif
		switch (statistics_type) {
		case CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA:
			rv = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_SNT, &last_size);
			break;
		case CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA:
			rv = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_RCV, &last_size);
			break;
		case CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA:
			rv = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_SNT, &last_size);
			rv1 = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_TOTAL_SNT, &size);
			break;
		case CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA:
			rv = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_RCV, &last_size);
			rv1 = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_TOTAL_RCV, &size);
			break;
		}
#if defined TIZEN_DUALSIM_ENABLE
		break;
	case 1:
		switch (statistics_type) {
		case CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA:
			rv = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_SNT2, &last_size);
			break;
		case CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA:
			rv = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_RCV2, &last_size);
			break;
		case CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA:
			rv = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_SNT2, &last_size);
			rv1 = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_TOTAL_SNT2, &size);
			break;
		case CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA:
			rv = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_RCV2, &last_size);
			rv1 = vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_TOTAL_RCV2, &size);
			break;
		}
		break;
	default:
		*llsize = 0;
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid subscriber id:%d", sim_id);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
#endif

	if (rv != VCONF_OK || rv1 != VCONF_OK) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to get cellular statistics"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_OPERATION_FAILED; //LCOV_EXCL_LINE
	}

	*llsize = (long long)(last_size * 1000) + (long long)(size * 1000);
	CONNECTION_LOG(CONNECTION_INFO, "%lld bytes", *llsize);

	return CONNECTION_ERROR_NONE;
}

static int __get_statistic(connection_type_e connection_type,
		connection_statistics_type_e statistics_type, long long *llsize)
{
	int rv, stat_type;
	unsigned long long ull_size;

	if (llsize == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	rv  = _connection_libnet_check_get_privilege();
	if (rv == CONNECTION_ERROR_PERMISSION_DENIED)
		return rv;
	else if (rv != CONNECTION_ERROR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to get statistics"); //LCOV_EXCL_LINE
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	if (connection_type == CONNECTION_TYPE_CELLULAR)
		return __get_cellular_statistic(statistics_type, llsize);
	else if (connection_type == CONNECTION_TYPE_WIFI) {
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

		rv  = _connection_libnet_get_statistics(stat_type, &ull_size);
		if (rv == CONNECTION_ERROR_PERMISSION_DENIED)
			return rv;
		else if (rv != CONNECTION_ERROR_NONE) {
			CONNECTION_LOG(CONNECTION_ERROR, "Failed to get Wi-Fi statistics"); //LCOV_EXCL_LINE
			*llsize = 0; //LCOV_EXCL_LINE
			return CONNECTION_ERROR_OPERATION_FAILED; //LCOV_EXCL_LINE
		}

		CONNECTION_LOG(CONNECTION_INFO, "%lld bytes", ull_size);
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

	CONNECTION_LOG(CONNECTION_INFO, "connection_reset_statistics success");

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_statistics(connection_h connection,
				connection_type_e connection_type,
				connection_statistics_type_e statistics_type, long long* size)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE);

	if (connection_type == CONNECTION_TYPE_CELLULAR)
		CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE);
	else if (connection_type == CONNECTION_TYPE_WIFI)
		CHECK_FEATURE_SUPPORTED(WIFI_FEATURE);

	if (!(__connection_check_handle_validity(connection)) || size == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __get_statistic(connection_type, statistics_type, size);
}

EXPORT_API int connection_reset_statistics(connection_h connection,
				connection_type_e connection_type,
				connection_statistics_type_e statistics_type)
{
	CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE, WIFI_FEATURE);

	if (connection_type == CONNECTION_TYPE_CELLULAR)
		CHECK_FEATURE_SUPPORTED(TELEPHONY_FEATURE);
	else if (connection_type == CONNECTION_TYPE_WIFI)
		CHECK_FEATURE_SUPPORTED(WIFI_FEATURE);

	if (!__connection_check_handle_validity(connection)) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __reset_statistic(connection_type, statistics_type);
}

