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

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <vconf/vconf.h>
#include "net_connection_private.h"

static GSList *conn_handle_list = NULL;

static int __connection_convert_net_type(net_device_t net_type)
{
	switch (net_type) {
	case NET_DEVICE_CELLULAR:
		return CONNECTION_TYPE_CELLULAR;
	case NET_DEVICE_WIFI:
		return CONNECTION_TYPE_WIFI;
	case NET_DEVICE_ETHERNET:
		return CONNECTION_TYPE_ETHERNET;
	case NET_DEVICE_BLUETOOTH:
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

static int __connection_get_type_changed_callback_count(void)
{
	GSList *list;
	int count = 0;

	for (list = conn_handle_list; list; list = list->next) {
		connection_handle_s *local_handle = (connection_handle_s *)list->data;
		if (local_handle->type_changed_callback) count++;
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

static int __connection_set_type_changed_callback(connection_h connection,
							void *callback, void *user_data)
{
	connection_handle_s *local_handle = (connection_handle_s *)connection;

	if (callback) {
		if (__connection_get_type_changed_callback_count() == 0)
			_connection_libnet_set_type_changed_cb();

		local_handle->state_changed_user_data = user_data;
	} else {
		if (local_handle->type_changed_callback &&
		    __connection_get_type_changed_callback_count() == 1)
			_connection_libnet_unset_type_changed_cb();
	}

	local_handle->type_changed_callback = callback;
	return CONNECTION_ERROR_NONE;
}

static int __connection_set_ip_changed_callback(connection_h connection,
							void *callback, void *user_data)
{
	connection_handle_s *local_handle = (connection_handle_s *)connection;

	if (callback) {
		if (__connection_get_ip_changed_callback_count() == 0)
			_connection_libnet_set_ip_changed_cb();

		local_handle->ip_changed_user_data = user_data;
	} else {
		if (local_handle->ip_changed_callback &&
		    __connection_get_ip_changed_callback_count() == 1)
			_connection_libnet_unset_ip_changed_cb();
	}

	local_handle->ip_changed_callback = callback;
	return CONNECTION_ERROR_NONE;
}

static int __connection_set_proxy_changed_callback(connection_h connection,
							void *callback, void *user_data)
{
	connection_handle_s *local_handle = (connection_handle_s *)connection;

	if (callback) {
		if (__connection_get_proxy_changed_callback_count() == 0)
			_connection_libnet_set_proxy_changed_cb();

		local_handle->proxy_changed_user_data = user_data;
	} else {
		if (local_handle->proxy_changed_callback &&
		    __connection_get_proxy_changed_callback_count() == 1)
			_connection_libnet_unset_proxy_changed_cb();
	}

	local_handle->proxy_changed_callback = callback;
	return CONNECTION_ERROR_NONE;
}

void _connection_cb_type_change_cb(net_device_t device_type, void *user_data)
{
	CONNECTION_LOG(CONNECTION_INFO, "Net Status Changed Indication\n");

	GSList *list;

	for (list = conn_handle_list; list; list = list->next) {
		connection_handle_s *local_handle = (connection_handle_s *)list->data;
		if (local_handle->type_changed_callback)
			local_handle->type_changed_callback(
					__connection_convert_net_type(
								device_type),
					local_handle->state_changed_user_data);
	}
}

void _connection_cb_ip_change_cb(char *ip_addr, void *user_data)
{
	CONNECTION_LOG(CONNECTION_INFO, "Net IP Changed Indication\n");

	if (ip_addr == NULL)
		return;

	GSList *list;

	for (list = conn_handle_list; list; list = list->next) {
		connection_handle_s *local_handle = (connection_handle_s *)list->data;
		if (local_handle->ip_changed_callback)
			local_handle->ip_changed_callback(
					ip_addr, NULL,
					local_handle->ip_changed_user_data);
	}
}

void _connection_cb_proxy_change_cb(char *proxy, void *user_data)
{
	CONNECTION_LOG(CONNECTION_INFO, "Net IP Changed Indication\n");

	if (proxy == NULL)
		return;

	GSList *list;

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

static int __connection_get_handle_count(void)
{
	GSList *list;
	int count = 0;

	if (!conn_handle_list)
		return count;

	for (list = conn_handle_list; list; list = list->next) count++;

	return count;
}

/* Connection Manager ********************************************************/
EXPORT_API int connection_create(connection_h* connection)
{
	CONNECTION_MUTEX_LOCK;

	if (connection == NULL || __connection_check_handle_validity(*connection)) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		CONNECTION_MUTEX_UNLOCK;
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (_connection_libnet_init() == false) {
		CONNECTION_LOG(CONNECTION_ERROR, "Creation failed!\n");
		CONNECTION_MUTEX_UNLOCK;
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	CONNECTION_LOG(CONNECTION_ERROR, "Connection successfully created!\n");

	*connection = g_try_malloc0(sizeof(connection_handle_s));
	if (*connection != NULL) {
		CONNECTION_LOG(CONNECTION_INFO, "New Handle Created %p\n", *connection);
	} else {
		CONNECTION_MUTEX_UNLOCK;
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	}

	conn_handle_list = g_slist_append(conn_handle_list, *connection);

	CONNECTION_MUTEX_UNLOCK;
	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_destroy(connection_h connection)
{
	CONNECTION_MUTEX_LOCK;

	if (connection == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		CONNECTION_MUTEX_UNLOCK;
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	CONNECTION_LOG(CONNECTION_INFO, "Destroy Handle : %p\n", connection);

	__connection_set_type_changed_callback(connection, NULL, NULL);
	__connection_set_ip_changed_callback(connection, NULL, NULL);
	__connection_set_proxy_changed_callback(connection, NULL, NULL);

	conn_handle_list = g_slist_remove(conn_handle_list, connection);

	g_free(connection);

	if (__connection_get_handle_count() == 0)
		_connection_libnet_deinit();

	CONNECTION_MUTEX_UNLOCK;
	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_get_type(connection_h connection, connection_type_e* type)
{
	net_device_t device_type;

	if (type == NULL || !(__connection_check_handle_validity(connection))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (_connection_libnet_get_default_device_type(&device_type) ==
						CONNECTION_ERROR_NO_CONNECTION)
		*type = CONNECTION_TYPE_DISCONNECTED;
	else
		*type = __connection_convert_net_type(device_type);

	CONNECTION_LOG(CONNECTION_INFO, "Connected Network = %d\n",
								device_type);

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
		if (_connection_libnet_get_default_ip_address(ip_address) !=
							CONNECTION_ERROR_NONE)
			return CONNECTION_ERROR_OPERATION_FAILED;
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
		CONNECTION_LOG(CONNECTION_ERROR, "Get IP Address Failed\n");
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
		if (_connection_libnet_get_default_proxy(proxy) !=
							CONNECTION_ERROR_NONE)
			return CONNECTION_ERROR_OPERATION_FAILED;
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
		CONNECTION_LOG(CONNECTION_ERROR, "Get Proxy Failed\n");
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

	/*
	int rv = 0;

	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}
	TODO:
	rv = net_add_profile(profile_info->ProfileInfo.Pdp.ServiceType, (net_profile_info_t*)profile);

	if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "net_add_profile Failed = %d\n", rv);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	*/
	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_remove_profile(connection_h connection, connection_profile_h profile)
{
	if (!(__connection_check_handle_validity(connection)) ||
	    !(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	int rv = 0;
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR &&
	    profile_info->profile_type != NET_DEVICE_WIFI) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}
	TODO:
	rv = net_delete_profile(profile_info->profile_name);
	if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "net_delete_profile Failed = %d\n", rv);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_update_profile(connection_h connection, connection_profile_h profile)
{
	if (!(__connection_check_handle_validity(connection)) ||
	    !(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

/*
	int rv = 0;
	net_profile_info_t *profile_info = profile;

	TODO:
	rv = net_modify_profile(profile_info->profile_name, (net_profile_info_t *)profile);
	if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "net_modify_profile Failed = %d\n", rv);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
*/

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


/* Connection Statistics module ******************************************************************/

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

		CONNECTION_LOG(CONNECTION_INFO,"%d bytes\n", ull_size);
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
	if(rv != CONNECTION_ERROR_NONE)
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
