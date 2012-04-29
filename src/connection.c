/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <stdio.h>
#include <string.h>
#include <dlog.h>
#include <glib.h>
#include <vconf/vconf.h>
#include <net_connection_private.h>

#define TIZEN_NET_CONNECTION "CAPI_NETWORK_CONNECTION"

static GSList *conn_handle_list = NULL;

static void __connection_cb_state_change_cb(keynode_t *node, void *user_data);
static void __connection_cb_ip_change_cb(keynode_t *node, void *user_data);
static void __connection_cb_proxy_change_cb(keynode_t *node, void *user_data);


static int __connection_convert_net_state(int status)
{
	switch (status) {
	case VCONFKEY_NETWORK_CELLULAR:
		return CONNECTION_NETWORK_STATE_CELLULAR;
	case VCONFKEY_NETWORK_WIFI:
		return CONNECTION_NETWORK_STATE_WIFI;
	default:
		return CONNECTION_NETWORK_STATE_DISCONNECTED;
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

static int __connection_convert_wifi_state(int status)
{
	switch (status) {
	case VCONFKEY_NETWORK_WIFI_CONNECTED:
		return CONNECTION_WIFI_STATE_CONNECTED;
	case VCONFKEY_NETWORK_WIFI_NOT_CONNECTED:
		return CONNECTION_WIFI_STATE_DISCONNECTED;
	default:
		return CONNECTION_WIFI_STATE_DEACTIVATED;
	}
}

static int __connection_get_state_changed_callback_count(void)
{
	GSList *list;
	int count = 0;

	for (list = conn_handle_list; list; list = list->next) {
		connection_handle_s *local_handle = (connection_handle_s *)list->data;
		if (local_handle->state_changed_callback) count++;
	}

	return count;
}

static int __connection_get_ip_changed_callback_count(void)
{
	GSList *list;
	int count = 0;

	for (list = conn_handle_list; list; list = list->next) {
		connection_handle_s *local_handle = (connection_handle_s *)list->data;
		if (local_handle->ip_changed_callback) count++;
	}

	return count;
}

static int __connection_get_proxy_changed_callback_count(void)
{
	GSList *list;
	int count = 0;

	for (list = conn_handle_list; list; list = list->next) {
		connection_handle_s *local_handle = (connection_handle_s *)list->data;
		if (local_handle->proxy_changed_callback) count++;
	}

	return count;
}

static int __connection_set_state_changed_callback(connection_h connection, void *callback, void *user_data)
{
	connection_handle_s *local_handle = (connection_handle_s *)connection;

	if (callback) {
		if (__connection_get_state_changed_callback_count() == 0)
			vconf_notify_key_changed(VCONFKEY_NETWORK_STATUS ,
						__connection_cb_state_change_cb,
						NULL);

		local_handle->state_changed_user_data = user_data;
	} else {
		if (local_handle->state_changed_callback &&
		    __connection_get_state_changed_callback_count() == 1)
			vconf_ignore_key_changed(VCONFKEY_NETWORK_STATUS,
						__connection_cb_state_change_cb);
	}

	local_handle->state_changed_callback = callback;
	return CONNECTION_ERROR_NONE;
}

static int __connection_set_ip_changed_callback(connection_h connection, void *callback, void *user_data)
{
	connection_handle_s *local_handle = (connection_handle_s *)connection;

	if (callback) {
		if (__connection_get_ip_changed_callback_count() == 0)
			vconf_notify_key_changed(VCONFKEY_NETWORK_IP,
						__connection_cb_ip_change_cb,
						NULL);

		local_handle->ip_changed_user_data = user_data;
	} else {
		if (local_handle->ip_changed_callback &&
		    __connection_get_ip_changed_callback_count() == 1)
			vconf_ignore_key_changed(VCONFKEY_NETWORK_IP,
						__connection_cb_ip_change_cb);
	}

	local_handle->ip_changed_callback = callback;
	return CONNECTION_ERROR_NONE;
}

static int __connection_set_proxy_changed_callback(connection_h connection, void *callback, void *user_data)
{
	connection_handle_s *local_handle = (connection_handle_s *)connection;

	if (callback) {
		if (__connection_get_proxy_changed_callback_count() == 0)
			vconf_notify_key_changed(VCONFKEY_NETWORK_PROXY,
						__connection_cb_proxy_change_cb,
						NULL);

		local_handle->proxy_changed_callback = user_data;
	} else {
		if (local_handle->proxy_changed_callback &&
		    __connection_get_proxy_changed_callback_count() == 1)
			vconf_ignore_key_changed(VCONFKEY_NETWORK_PROXY,
						__connection_cb_proxy_change_cb);
	}

	local_handle->proxy_changed_callback = callback;
	return CONNECTION_ERROR_NONE;
}

static void __connection_cb_state_change_cb(keynode_t *node, void *user_data)
{
	LOGI(TIZEN_NET_CONNECTION,"Net Status Changed Indication\n");

	GSList *list;
	int state = vconf_keynode_get_int(node);

	for (list = conn_handle_list; list; list = list->next) {
		connection_handle_s *local_handle = (connection_handle_s *)list->data;
		if (local_handle->state_changed_callback)
			local_handle->state_changed_callback(
					__connection_convert_net_state(state),
					local_handle->state_changed_user_data);
	}
}

static void __connection_cb_ip_change_cb(keynode_t *node, void *user_data)
{
	LOGI(TIZEN_NET_CONNECTION,"Net IP Changed Indication\n");

	GSList *list;
	char *ip_addr = vconf_keynode_get_str(node);

	for (list = conn_handle_list; list; list = list->next) {
		connection_handle_s *local_handle = (connection_handle_s *)list->data;
		if (local_handle->ip_changed_callback)
			local_handle->ip_changed_callback(
					ip_addr, NULL,
					local_handle->ip_changed_user_data);
	}
}

static void __connection_cb_proxy_change_cb(keynode_t *node, void *user_data)
{
	LOGI(TIZEN_NET_CONNECTION,"Net IP Changed Indication\n");

	GSList *list;
	char *proxy = vconf_keynode_get_str(node);

	for (list = conn_handle_list; list; list = list->next) {
		connection_handle_s *local_handle = (connection_handle_s *)list->data;
		if (local_handle->proxy_changed_callback)
			local_handle->proxy_changed_callback(
					proxy, NULL,
					local_handle->proxy_changed_user_data);
	}
}

static bool __connection_check_handle_validity(connection_h connection)
{
	GSList *list;

	for (list = conn_handle_list; list; list = list->next)
		if (connection == list->data) return true;

	return false;
}

int connection_create(connection_h* connection)
{
	if (connection == NULL || __connection_check_handle_validity(*connection)) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	*connection = g_try_malloc0(sizeof(connection_handle_s));
	if (*connection != NULL) {
		LOGI(TIZEN_NET_CONNECTION, "New Handle Created %p\n", *connection);
	} else {
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	}

	conn_handle_list = g_slist_append(conn_handle_list, *connection);

	return CONNECTION_ERROR_NONE;
}

int connection_destroy(connection_h connection)
{
	if (connection == NULL || !(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	conn_handle_list = g_slist_remove(conn_handle_list, connection);

	LOGI(TIZEN_NET_CONNECTION, "Destroy Handle : %p\n", connection);

	__connection_set_state_changed_callback(connection, NULL, NULL);
	__connection_set_ip_changed_callback(connection, NULL, NULL);
	__connection_set_proxy_changed_callback(connection, NULL, NULL);

	g_free(connection);

	return CONNECTION_ERROR_NONE;
}

int connection_get_network_state(connection_h connection, connection_network_state_e* state)
{
	int status = 0;

	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (vconf_get_int(VCONFKEY_NETWORK_STATUS, &status)) {
		LOGI(TIZEN_NET_CONNECTION,"vconf_get_int Failed = %d\n", status);
		return CONNECTION_ERROR_INVALID_OPERATION;
	}

	LOGI(TIZEN_NET_CONNECTION,"Connected Network = %d\n", status);

	*state = __connection_convert_net_state(status);

	return CONNECTION_ERROR_NONE;
}

int connection_get_ip_address(connection_h connection, connection_address_family_e address_family, char** ip_address)
{
	if (ip_address == NULL || !(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	switch (address_family) {
	case CONNECTION_ADDRESS_FAMILY_IPV4:
		*ip_address = vconf_get_str(VCONFKEY_NETWORK_IP);
		break;
	case CONNECTION_ADDRESS_FAMILY_IPV6:
		LOGI(TIZEN_NET_CONNECTION, "Not supported yet\n");
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;
		break;
	default:
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (*ip_address == NULL) {
		LOGI(TIZEN_NET_CONNECTION,"vconf_get_str Failed\n");
		return CONNECTION_ERROR_INVALID_OPERATION;
	}

	LOGI(TIZEN_NET_CONNECTION,"IP Address %s\n", *ip_address);

	return CONNECTION_ERROR_NONE;
}

int connection_get_proxy(connection_h connection, connection_address_family_e address_family, char** proxy)
{
	if (proxy == NULL || !(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	switch (address_family) {
	case CONNECTION_ADDRESS_FAMILY_IPV4:
		*proxy = vconf_get_str(VCONFKEY_NETWORK_PROXY);
		break;
	case CONNECTION_ADDRESS_FAMILY_IPV6:
		LOGI(TIZEN_NET_CONNECTION, "Not supported yet\n");
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;
		break;
	default:
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (*proxy == NULL) {
		LOGI(TIZEN_NET_CONNECTION,"vconf_get_str Failed\n");
		return CONNECTION_ERROR_INVALID_OPERATION;
	}

	LOGI(TIZEN_NET_CONNECTION,"Proxy Address %s\n", *proxy);

	return CONNECTION_ERROR_NONE;
}

int connection_get_cellular_state(connection_h connection, connection_cellular_state_e* state)
{
	int status = 0;

	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (!vconf_get_int(VCONFKEY_NETWORK_CELLULAR_STATE, &status)) {
		LOGI(TIZEN_NET_CONNECTION,"Cellular = %d\n", status);
		*state = __connection_convert_cellular_state(status);
		return CONNECTION_ERROR_NONE;
	} else {
		LOGI(TIZEN_NET_CONNECTION,"vconf_get_int Failed = %d\n", status);
		return CONNECTION_ERROR_INVALID_OPERATION;
	}
}

int connection_get_wifi_state(connection_h connection, connection_wifi_state_e* state)
{
	int status = 0;

	if (state == NULL || !(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (!vconf_get_int(VCONFKEY_NETWORK_WIFI_STATE, &status)) {
		LOGI(TIZEN_NET_CONNECTION,"WiFi = %d\n", status);
		*state = __connection_convert_wifi_state(status);
		return CONNECTION_ERROR_NONE;
	} else {
		LOGI(TIZEN_NET_CONNECTION,"vconf_get_int Failed = %d\n", status);
		return CONNECTION_ERROR_INVALID_OPERATION;
	}
}

int connection_set_network_state_changed_cb(connection_h connection,
				connection_network_state_changed_cb callback, void* user_data)
{
	if (callback == NULL || !(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_state_changed_callback(connection, callback, user_data);
}

int connection_unset_network_state_changed_cb(connection_h connection)
{
	if (!(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_state_changed_callback(connection, NULL, NULL);
}

int connection_set_ip_address_changed_cb(connection_h connection,
				connection_address_changed_cb callback, void* user_data)
{
	if (callback == NULL || !(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_ip_changed_callback(connection, callback, user_data);
}

int connection_unset_ip_address_changed_cb(connection_h connection)
{
	if (!(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_ip_changed_callback(connection, NULL, NULL);
}

int connection_set_proxy_address_changed_cb(connection_h connection,
				connection_address_changed_cb callback, void* user_data)
{
	if (callback == NULL || !(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_proxy_changed_callback(connection, callback, user_data);
}

int connection_unset_proxy_address_changed_cb(connection_h connection)
{
	if (!(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	return __connection_set_proxy_changed_callback(connection, NULL, NULL);
}

static int __fill_call_statistic(connection_h connection, stat_request_e member, int *size)
{
	if (size == NULL || !(__connection_check_handle_validity(connection))) {
		LOGI(TIZEN_NET_CONNECTION, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	switch (member) {
	case LAST_SENT_DATA_SIZE:
		if (vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_SNT, size)) {
			LOGI(TIZEN_NET_CONNECTION,
					"Cannot Get LAST_SENT_DATA_SIZE = %d\n",
					*size);
			*size = 0;
			return CONNECTION_ERROR_INVALID_OPERATION;
		}
		LOGI(TIZEN_NET_CONNECTION,"LAST_SENT_DATA_SIZE:%d bytes\n", *size);

		break;
	case LAST_RECEIVED_DATA_SIZE:
		if (vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_RCV, size)) {
			LOGI(TIZEN_NET_CONNECTION,
					"Cannot Get LAST_RECEIVED_DATA_SIZE: = %d\n",
					*size);
			*size = 0;
			return CONNECTION_ERROR_INVALID_OPERATION;
		}
		LOGI(TIZEN_NET_CONNECTION,"LAST_RECEIVED_DATA_SIZE:%d bytes\n", *size);
		break;
	case TOTAL_SENT_DATA_SIZE:
		if (vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_TOTAL_SNT, size)) {
			LOGI(TIZEN_NET_CONNECTION,
					"Cannot Get TOTAL_SENT_DATA_SIZE: = %d\n",
					*size);
			*size = 0;
			return CONNECTION_ERROR_INVALID_OPERATION;
		}
		LOGI(TIZEN_NET_CONNECTION,"TOTAL_SENT_DATA_SIZE:%d bytes\n", *size);
		break;
	case TOTAL_RECEIVED_DATA_SIZE:
		if (vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_TOTAL_RCV, size)) {
			LOGI(TIZEN_NET_CONNECTION,
					"Cannot Get TOTAL_RECEIVED_DATA_SIZE: = %d\n",
					*size);
			*size = 0;
			return CONNECTION_ERROR_INVALID_OPERATION;
		}
		LOGI(TIZEN_NET_CONNECTION,"TOTAL_RECEIVED_DATA_SIZE:%d bytes\n", *size);
		break;
	case LAST_WIFI_SENT_DATA_SIZE:
		if (vconf_get_int(VCONFKEY_NETWORK_WIFI_PKT_LAST_SNT, size)) {
			LOGI(TIZEN_NET_CONNECTION,
					"Cannot Get LAST_WIFI_SENT_DATA_SIZE: = %d\n",
					*size);
			*size = 0;
			return CONNECTION_ERROR_INVALID_OPERATION;
		}
		LOGI(TIZEN_NET_CONNECTION,"LAST_WIFI_SENT_DATA_SIZE:%d bytes\n", *size);
		break;
	case LAST_WIFI_RECEIVED_DATA_SIZE:
		if (vconf_get_int(VCONFKEY_NETWORK_WIFI_PKT_LAST_RCV, size)) {
			LOGI(TIZEN_NET_CONNECTION,
					"Cannot Get LAST_WIFI_RECEIVED_DATA_SIZE: = %d\n",
					*size);
			*size = 0;
			return CONNECTION_ERROR_INVALID_OPERATION;
		}
		LOGI(TIZEN_NET_CONNECTION,"LAST_WIFI_RECEIVED_DATA_SIZE:%d bytes\n", *size);
		break;
	case TOTAL_WIFI_SENT_DATA_SIZE:
		if (vconf_get_int(VCONFKEY_NETWORK_WIFI_PKT_TOTAL_SNT, size)) {
			LOGI(TIZEN_NET_CONNECTION,
					"Cannot Get TOTAL_WIFI_SENT_DATA_SIZE: = %d\n",
					*size);
			*size = 0;
			return CONNECTION_ERROR_INVALID_OPERATION;
		}
		LOGI(TIZEN_NET_CONNECTION,"TOTAL_WIFI_SENT_DATA_SIZE:%d bytes\n", *size);
		break;
	case TOTAL_WIFI_RECEIVED_DATA_SIZE:
		if (vconf_get_int(VCONFKEY_NETWORK_WIFI_PKT_TOTAL_RCV, size)) {
			LOGI(TIZEN_NET_CONNECTION,
					"Cannot Get TOTAL_WIFI_RECEIVED_DATA_SIZE: = %d\n",
					*size);
			*size = 0;
			return CONNECTION_ERROR_INVALID_OPERATION;
		}
		LOGI(TIZEN_NET_CONNECTION,"TOTAL_WIFI_RECEIVED_DATA_SIZE:%d bytes\n", *size);
		break;
	}
	return CONNECTION_ERROR_NONE;
}

int connection_get_last_received_data_size(connection_h connection, int *size)
{
	return __fill_call_statistic(connection, LAST_RECEIVED_DATA_SIZE, size);
}

int connection_get_last_sent_data_size(connection_h connection, int *size)
{
	return __fill_call_statistic(connection, LAST_SENT_DATA_SIZE, size);
}

int connection_get_total_received_data_size (connection_h connection, int *size)
{
	return __fill_call_statistic(connection, TOTAL_RECEIVED_DATA_SIZE, size);
}

int connection_get_total_sent_data_size (connection_h connection, int *size)
{
	return __fill_call_statistic(connection, TOTAL_SENT_DATA_SIZE, size);
}

int connection_get_wifi_last_received_data_size(connection_h connection, int *size)
{
	return __fill_call_statistic(connection, LAST_WIFI_RECEIVED_DATA_SIZE, size);
}

int connection_get_wifi_last_sent_data_size(connection_h connection, int *size)
{
	return __fill_call_statistic(connection, LAST_WIFI_SENT_DATA_SIZE, size);
}

int connection_get_wifi_total_received_data_size (connection_h connection, int *size)
{
	return __fill_call_statistic(connection, TOTAL_WIFI_RECEIVED_DATA_SIZE, size);
}

int connection_get_wifi_total_sent_data_size (connection_h connection, int *size)
{
	return __fill_call_statistic(connection, TOTAL_WIFI_SENT_DATA_SIZE, size);
}
