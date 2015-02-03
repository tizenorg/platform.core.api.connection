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
#include <winet-wifi.h>
#include <connman-lib.h>
#include <connman-manager.h>
#include <connman-technology.h>
#include "net_connection_private.h"

static GSList *prof_handle_list = NULL;
static GHashTable *profile_cb_table = NULL;
static GHashTable *services_changed_cb_table;

struct _profile_cb_s {
	connection_profile_state_changed_cb callback;
	connection_profile_state_e state;
	void *user_data;
};

struct _profile_list_s {
	int count;
	int next;
	net_profile_info_t *profiles;
};

struct _libnet_s {
	connection_opened_cb opened_cb;
	connection_closed_cb closed_cb;
	connection_set_default_cb set_default_cb;
	void *opened_user_data;
	void *closed_user_data;
	void *set_default_user_data;
	bool registered;
};

struct _last_default_service_property_s {
	net_device_t device_type;
	char *service_path;
	char *ip_address;
	char *proxy_address;
};

static struct _last_default_service_property_s last_default_service_property = {
							NET_DEVICE_UNKNOWN,
							NULL,
							NULL,
							NULL};

static struct _profile_list_s profile_iterator = {0, 0, NULL};
static struct _libnet_s libnet = {NULL, NULL, NULL, NULL, NULL, NULL, false};

static connection_error_e __libnet_convert_to_cp_error_type(
						enum connman_lib_err err_type)
{
	switch (err_type) {
	case CONNMAN_LIB_ERR_NONE:
		return CONNECTION_ERROR_NONE;
	case CONNMAN_LIB_ERR_ALREADY_EXISTS:
		return CONNECTION_ERROR_INVALID_OPERATION;
	case CONNMAN_LIB_ERR_NOT_REGISTERED:
		return CONNECTION_ERROR_INVALID_OPERATION;
	case CONNMAN_LIB_ERR_NOT_CONNECTED:
		return CONNECTION_ERROR_NO_CONNECTION;
	case CONNMAN_LIB_ERR_ALREADY_CONNECTED:
		return CONNECTION_ERROR_ALREADY_EXISTS;
	case CONNMAN_LIB_ERR_IN_PROGRESS:
		return CONNECTION_ERROR_NOW_IN_PROGRESS;
	case CONNMAN_LIB_ERR_OPERATION_ABORTED:
		return CONNECTION_ERROR_OPERATION_ABORTED;
	case CONNMAN_LIB_ERR_OPERATION_TIMEOUT:
	case CONNMAN_LIB_ERR_TIMEOUT:
		return CONNECTION_ERROR_NO_REPLY;
	default:
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
}

/*
static const char *__libnet_convert_cp_error_type_to_string(connection_error_e err_type)
{
	switch (err_type) {
	case CONNECTION_ERROR_NONE:
		return "NONE";
	case CONNECTION_ERROR_INVALID_PARAMETER:
		return "INVALID_PARAMETER";
	case CONNECTION_ERROR_OUT_OF_MEMORY:
		return "OUT_OF_MEMORY";
	case CONNECTION_ERROR_INVALID_OPERATION:
		return "INVALID_OPERATION";
	case CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED:
		return "ADDRESS_FAMILY_NOT_SUPPORTED";
	case CONNECTION_ERROR_OPERATION_FAILED:
		return "OPERATION_FAILED";
	case CONNECTION_ERROR_ITERATOR_END:
		return "ITERATOR_END";
	case CONNECTION_ERROR_NO_CONNECTION:
		return "NO_CONNECTION";
	case CONNECTION_ERROR_NOW_IN_PROGRESS:
		return "NOW_IN_PROGRESS";
	case CONNECTION_ERROR_ALREADY_EXISTS:
		return "ALREADY_EXISTS";
	case CONNECTION_ERROR_OPERATION_ABORTED:
		return "OPERATION_ABORTED";
	case CONNECTION_ERROR_DHCP_FAILED:
		return "DHCP_FAILED";
	case CONNECTION_ERROR_INVALID_KEY:
		return "INVALID_KEY";
	case CONNECTION_ERROR_NO_REPLY:
		return "NO_REPLY";
	}

	return "UNKNOWN";
}

static const char *__libnet_convert_cp_state_to_string(connection_profile_state_e state)
{
	switch (state) {
	case CONNECTION_PROFILE_STATE_DISCONNECTED:
		return "DISCONNECTED";
	case CONNECTION_PROFILE_STATE_ASSOCIATION:
		return "ASSOCIATION";
	case CONNECTION_PROFILE_STATE_CONFIGURATION:
		return "CONFIGURATION";
	case CONNECTION_PROFILE_STATE_CONNECTED:
		return "CONNECTED";
	default:
		return "UNKNOWN";
	}
}
*/

static void __libnet_set_opened_cb(connection_opened_cb user_cb, void *user_data)
{
	if (user_cb) {
		libnet.opened_cb = user_cb;
		libnet.opened_user_data = user_data;
	}
}

static void __libnet_opened_cb(connection_error_e result)
{
	if (libnet.opened_cb)
		libnet.opened_cb(result, libnet.opened_user_data);

	libnet.opened_cb = NULL;
	libnet.opened_user_data = NULL;
}

static void __libnet_set_closed_cb(connection_closed_cb user_cb, void *user_data)
{
	if (user_cb) {
		libnet.closed_cb = user_cb;
		libnet.closed_user_data = user_data;
	}
}

static void __libnet_closed_cb(connection_error_e result)
{
	if (libnet.closed_cb)
		libnet.closed_cb(result, libnet.closed_user_data);

	libnet.closed_cb = NULL;
	libnet.closed_user_data = NULL;
}

static void __libnet_set_default_cb(connection_set_default_cb user_cb, void *user_data)
{
	if (user_cb) {
		libnet.set_default_cb = user_cb;
		libnet.set_default_user_data = user_data;
	}
}

/*
static void __libnet_default_cb(connection_error_e result)
{
	if (libnet.set_default_cb)
		libnet.set_default_cb(result, libnet.set_default_user_data);

	libnet.set_default_cb = NULL;
	libnet.set_default_user_data = NULL;
}
*/
static void __libnet_state_changed_cb(const char *profile_name,
					connection_profile_state_e state)
{
	if (profile_name == NULL)
		return;

	struct _profile_cb_s *cb_info;
	cb_info = g_hash_table_lookup(profile_cb_table, profile_name);

	if (cb_info == NULL)
		return;

	if (cb_info->state == state)
		return;

	cb_info->state = state;

	if (state >= 0 && cb_info->callback)
		cb_info->callback(state, cb_info->user_data);
}

static void __libnet_opened_connected_cb(enum connman_lib_err result,
							void *user_data)
{
	CONNECTION_LOG(CONNECTION_INFO, "callback: %d\n", result);

	__libnet_opened_cb(__libnet_convert_to_cp_error_type(result));
}

static void __libnet_closed_disconnected_cb(enum connman_lib_err result,
							void *user_data)
{
	CONNECTION_LOG(CONNECTION_INFO, "callback: %d\n", result);

	__libnet_closed_cb(__libnet_convert_to_cp_error_type(result));
}

static void __libnet_clear_profile_list(struct _profile_list_s *profile_list)
{
	if (profile_list->count > 0)
		g_free(profile_list->profiles);

	profile_list->count = 0;
	profile_list->next = 0;
	profile_list->profiles = NULL;
}

static net_device_t __libnet_service_type_string2type(const char *str)
{
	if (str == NULL)
		return NET_DEVICE_UNKNOWN;

	if (g_strcmp0(str, "ethernet") == 0)
		return NET_DEVICE_ETHERNET;
	if (g_strcmp0(str, "gadget") == 0)
		return NET_DEVICE_USB;
	if (g_strcmp0(str, "wifi") == 0)
		return NET_DEVICE_WIFI;
	if (g_strcmp0(str, "cellular") == 0)
		return NET_DEVICE_CELLULAR;
	if (g_strcmp0(str, "bluetooth") == 0)
		return NET_DEVICE_BLUETOOTH;

	return NET_DEVICE_UNKNOWN;
}

static net_state_type_t __libnet_service_state_string2type(const char *str)
{
	if (str == NULL)
		return NET_STATE_TYPE_UNKNOWN;

	if (g_strcmp0(str, "idle") == 0)
		return NET_STATE_TYPE_IDLE;
	if (g_strcmp0(str, "association") == 0)
		return NET_STATE_TYPE_ASSOCIATION;
	if (g_strcmp0(str, "configuration") == 0)
		return NET_STATE_TYPE_CONFIGURATION;
	if (g_strcmp0(str, "ready") == 0)
		return NET_STATE_TYPE_READY;
	if (g_strcmp0(str, "online") == 0)
		return NET_STATE_TYPE_ONLINE;
	if (g_strcmp0(str, "disconnect") == 0)
		return NET_STATE_TYPE_DISCONNECT;
	if (g_strcmp0(str, "failure") == 0)
		return NET_STATE_TYPE_FAILURE;

	return NET_STATE_TYPE_UNKNOWN;
}

static int __libnet_get_active_net_info(net_profile_info_t *active_profile_info)
{
	GList *services_list;
	struct connman_service *service;
	net_state_type_t profile_state;

	services_list = connman_get_services();
	if (services_list == NULL)
		return CONNECTION_ERROR_NO_CONNECTION;

	service = (struct connman_service *)services_list->data;
	profile_state = __libnet_service_state_string2type(
					connman_service_get_state(service));

	if ((profile_state == NET_STATE_TYPE_READY ||
				profile_state == NET_STATE_TYPE_ONLINE)) {
		active_profile_info->profile_type =
					__libnet_service_type_string2type(
					connman_service_get_type(service));
		g_strlcpy(active_profile_info->profile_name,
					connman_service_get_path(service),
					NET_PROFILE_NAME_LEN_MAX);
		active_profile_info->profile_state = profile_state;

		return CONNECTION_ERROR_NONE;
	}

	return CONNECTION_ERROR_NO_CONNECTION;
}

static int __libnet_extract_all_services(int *prof_count,
						net_profile_info_t **prof_ptr)
{
	int count = 0;
	GList *services_list;

	services_list = connman_get_services();
	if (services_list == NULL)
		return CONNECTION_ERROR_NO_CONNECTION;

	while (services_list) {
		net_profile_info_t prof_info;
		struct connman_service *service =
				(struct connman_service *)(services_list->data);
		net_device_t local_device_type =
					__libnet_service_type_string2type(
					connman_service_get_type(service));
		if (local_device_type == NET_DEVICE_UNKNOWN) {
			services_list = g_list_next(services_list);
			continue;
		}

		memset(&prof_info, 0, sizeof(net_profile_info_t));
		prof_info.profile_type = local_device_type;
		g_strlcpy(prof_info.profile_name,
					connman_service_get_path(service),
					NET_PROFILE_NAME_LEN_MAX);
		prof_info.profile_state = __libnet_service_state_string2type(
					connman_service_get_state(service));

		*prof_ptr = (net_profile_info_t *)g_realloc(*prof_ptr,
				(count + 1) * sizeof(net_profile_info_t));
		if (*prof_ptr == NULL) {
			CONNECTION_LOG(CONNECTION_ERROR,
						"Failed to allocate memory\n");
			*prof_count = 0;
			return CONNECTION_ERROR_OPERATION_FAILED;
		}

		memcpy(*prof_ptr + count, &prof_info,
						sizeof(net_profile_info_t));
		count++;

		services_list = g_list_next(services_list);
	}

	*prof_count = count;

	return CONNECTION_ERROR_NONE;
}

static int __libnet_get_profile_list(net_device_t device_type,
				net_profile_info_t **profile_list, int *count)
{
	int rv = CONNECTION_ERROR_NONE;
	int profile_count = 0;
	net_profile_info_t *profile_info = NULL;

	if (count == NULL || (device_type != NET_DEVICE_CELLULAR &&
					device_type != NET_DEVICE_WIFI &&
					device_type != NET_DEVICE_ETHERNET &&
					device_type != NET_DEVICE_BLUETOOTH &&
					device_type != NET_DEVICE_MAX))
		return CONNECTION_ERROR_INVALID_PARAMETER;

	rv = __libnet_extract_all_services(&profile_count, &profile_info);
	if (rv != CONNECTION_ERROR_NONE) {
		return rv;
	} else {
		*count = profile_count;
		*profile_list = profile_info;
	}

	return CONNECTION_ERROR_NONE;
}

static int __libnet_get_ipv4_ip_address(struct connman_service *service,
							char **ip_address)
{
	const struct service_ipv4 *ipv4;

	if (service == NULL)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	ipv4 = connman_service_get_ipv4_info(service);
	if (ipv4 == NULL || ipv4->address == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	*ip_address = g_strdup(ipv4->address);
	if (*ip_address == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	return CONNECTION_ERROR_NONE;
}

static void __libnet_update_network_type(net_device_t device_type)
{
	if (last_default_service_property.device_type == device_type)
		return;

	last_default_service_property.device_type = device_type;

	_connection_cb_type_change_cb(device_type, NULL);
}

static void __libnet_update_network_ipv4(struct connman_service *service,
								void *user_data)
{
	char *ipv4_addr;

	if (service == NULL) /*Disconnected the network*/
		ipv4_addr = g_strdup("");
	else {
		if (__libnet_get_ipv4_ip_address(service, &ipv4_addr) !=
							CONNECTION_ERROR_NONE)
			return;
	}

	if (!g_strcmp0(last_default_service_property.ip_address, ipv4_addr)) {
		g_free(ipv4_addr);
		return;
	}

	if (ipv4_addr) {
		g_free(last_default_service_property.ip_address);
		last_default_service_property.ip_address = g_strdup(ipv4_addr);
	} else if (g_strcmp0(last_default_service_property.ip_address, "")) {
		g_free(last_default_service_property.ip_address);
		last_default_service_property.ip_address = g_strdup("");
	}

	_connection_cb_ip_change_cb(ipv4_addr, NULL);

	g_free(ipv4_addr);
}

static void __libnet_update_network_proxy(struct connman_service *service,
								void *user_data)
{
	char *proxy_addr;

	if (service == NULL) /*Disconnected the network*/
		proxy_addr = g_strdup("");
	else {
		if (_connection_libnet_get_proxy_address(service, &proxy_addr)
			!= CONNECTION_ERROR_NONE)
			return;
	}

	if (!g_strcmp0(last_default_service_property.proxy_address,
								proxy_addr)) {
		g_free(proxy_addr);
		return;
	}

	if (proxy_addr) {
		g_free(last_default_service_property.proxy_address);
		last_default_service_property.proxy_address =
							g_strdup(proxy_addr);
	} else if (g_strcmp0(last_default_service_property.proxy_address, "")) {
		g_free(last_default_service_property.proxy_address);
		last_default_service_property.proxy_address = g_strdup("");
	}

	_connection_cb_proxy_change_cb(proxy_addr, NULL);

	g_free(proxy_addr);
}


static int __libnet_get_default_service(GList *services_list,
				struct connman_service **default_service)
{
	struct connman_service *service;
	net_state_type_t profile_state;

	service = (struct connman_service *)services_list->data;
	profile_state = __libnet_service_state_string2type(
					connman_service_get_state(service));

	if ((profile_state == NET_STATE_TYPE_READY ||
				profile_state == NET_STATE_TYPE_ONLINE)) {
		*default_service = service;

		return CONNECTION_ERROR_NONE;
	}

	return CONNECTION_ERROR_NO_CONNECTION;
}

static void __libnet_check_default_service_changed(GList *services_list)
{
	const gchar *service_path = NULL;
	struct connman_service *default_service = NULL;
	struct connman_service *last_default_service = NULL;
	net_device_t device_type = NET_DEVICE_UNKNOWN;

	if (services_list == NULL)
		return;

	last_default_service = connman_get_service_with_path(
				last_default_service_property.service_path);

	__libnet_get_default_service(services_list, &default_service);
	if (default_service) {
		device_type = __libnet_service_type_string2type(
				connman_service_get_type(default_service));
		service_path = connman_service_get_path(default_service);
	}

	if (!g_strcmp0(last_default_service_property.service_path,
								service_path))
		return;

	/*
	 * Update type/ipv4/proxy, if changed, it will revoke
	 * a valid callback function
	 */
	__libnet_update_network_type(device_type);
	__libnet_update_network_ipv4(default_service, NULL);
	__libnet_update_network_proxy(default_service, NULL);

	if (last_default_service) {
		/*
		 * Unregister to monitor the signal 'IPv4' and 'Proxy'
		 * of the default service
		 */
		connman_service_unset_property_changed_cb(last_default_service,
							SERVICE_PROP_IPV4);
		connman_service_unset_property_changed_cb(last_default_service,
							SERVICE_PROP_PROXY);
	}

	if (default_service) {
		/*
		 * Register to monitor the signal 'IPv4' and 'Proxy'
		 * of the default service
		 */
		connman_service_set_property_changed_cb(default_service,
						SERVICE_PROP_IPV4,
						__libnet_update_network_ipv4,
						NULL);
		connman_service_set_property_changed_cb(default_service,
						SERVICE_PROP_PROXY,
						__libnet_update_network_proxy,
						NULL);
	}

	g_free(last_default_service_property.service_path);
	last_default_service_property.service_path = g_strdup(service_path);
}

static void __libnet_service_state_changed_cb(struct connman_service *service,
								void *user_data)
{
	net_state_type_t net_state;
	connection_profile_state_e connection_state;

	net_state = __libnet_service_state_string2type(
					connman_service_get_state(service));
	connection_state = _profile_convert_to_cp_state(net_state);

	__libnet_state_changed_cb(connman_service_get_path(service),
							connection_state);
}

static void __libnet_set_services_state_changed_cb(
						struct connman_manager *manager,
						GList *added_service_list,
						GList *all_services_list,
						void *user_data)
{
	GList *iter;

	if (added_service_list == NULL)
		return;

	if (g_hash_table_size(profile_cb_table) == 0)
		return;

	for (iter = added_service_list; iter != NULL; iter = iter->next) {
		struct _profile_cb_s *cb_info;
		struct connman_service *service = iter->data;

		cb_info = g_hash_table_lookup(profile_cb_table,
					connman_service_get_path(service));
		if (cb_info != NULL) {
			connman_service_set_property_changed_cb(service,
					SERVICE_PROP_STATE,
					__libnet_service_state_changed_cb,
					NULL);
		}
	}
}

static void __libnet_set_default_service_changed_cb(
						struct connman_manager *manager,
						GList *added_service_list,
						GList *all_services_list,
						void *user_data)
{
	__libnet_check_default_service_changed(all_services_list);
}

static void __libnet_services_changed_callback(struct connman_manager *manager,
						GList *added_service_list,
						GList *all_services_list,
						void *user_data)
{
	CONNECTION_LOG(CONNECTION_INFO, "service changed");

	/*
	 * Execute all callback functions
	 */
	GHashTableIter iter;
	gpointer value;

	g_hash_table_iter_init(&iter, services_changed_cb_table);
	while (g_hash_table_iter_next(&iter, NULL, &value)) {
		connman_services_changed_cb cb = value;
		if (cb)
			cb(manager, added_service_list, all_services_list,
								user_data);
	}
}

static void __libnet_connman_manager_property_changed_cb(
						struct connman_manager *manager,
						void *user_data)
{
	__libnet_check_default_service_changed(connman_get_services());
}

int __libnet_get_connected_count(struct _profile_list_s *profile_list)
{
	int count = 0;
	int i = 0;

	for (;i < profile_list->count;i++) {
		if (profile_list->profiles[i].profile_state == NET_STATE_TYPE_ONLINE ||
		    profile_list->profiles[i].profile_state == NET_STATE_TYPE_READY)
			count++;
	}

	return count;
}

void __libnet_copy_connected_profile(net_profile_info_t **dest, struct _profile_list_s *source)
{
	int i = 0;

	for (;i < source->count;i++) {
		if (source->profiles[i].profile_state == NET_STATE_TYPE_ONLINE ||
		    source->profiles[i].profile_state == NET_STATE_TYPE_READY) {
			memcpy(*dest, &source->profiles[i], sizeof(net_profile_info_t));
			(*dest)++;
		}
	}
}

bool _connection_libnet_init(void)
{
	int rv;

	if (!libnet.registered) {
		rv = connman_lib_init();
		if (rv != CONNMAN_LIB_ERR_NONE)
			return false;

		libnet.registered = true;

		if (services_changed_cb_table == NULL)
			services_changed_cb_table = g_hash_table_new_full(
								g_direct_hash,
								g_direct_equal,
								NULL,
								NULL);

		if (profile_cb_table == NULL)
			profile_cb_table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	}

	return true;
}

bool _connection_libnet_deinit(void)
{
	if (libnet.registered) {
		connman_lib_deinit();

		libnet.registered = false;

		if (services_changed_cb_table) {
			g_hash_table_destroy(services_changed_cb_table);
			services_changed_cb_table = NULL;
		}

		if (profile_cb_table) {
			g_hash_table_destroy(profile_cb_table);
			profile_cb_table = NULL;
		}

		__libnet_clear_profile_list(&profile_iterator);

		if (prof_handle_list) {
			g_slist_free_full(prof_handle_list, g_free);
			prof_handle_list = NULL;
		}

		last_default_service_property.device_type = NET_DEVICE_UNKNOWN;

		g_free(last_default_service_property.service_path);
		last_default_service_property.service_path = NULL;

		g_free(last_default_service_property.ip_address);
		last_default_service_property.ip_address = NULL;

		g_free(last_default_service_property.proxy_address);
		last_default_service_property.proxy_address = NULL;
	}

	return true;
}

bool _connection_libnet_check_profile_validity(connection_profile_h profile)
{
	GSList *list;
	int i = 0;

	for (list = prof_handle_list; list; list = list->next)
		if (profile == list->data) return true;

	for (;i < profile_iterator.count;i++)
		if (profile == &profile_iterator.profiles[i]) return true;

	return false;
}

bool _connection_libnet_check_profile_cb_validity(connection_profile_h profile)
{
	struct _profile_cb_s *cb_info;
	net_profile_info_t *profile_info = profile;

	if (profile == NULL)
		return false;

	cb_info = g_hash_table_lookup(profile_cb_table, profile_info->profile_name);
	if (cb_info != NULL)
		return true;

	return false;
}

bool _connection_libnet_get_wifi_state(connection_wifi_state_e *state)
{
	struct connman_technology *technology;
	bool powered;
	bool tethering;
	bool connected;

	technology = connman_get_technology(TECH_TYPE_WIFI);
	if (technology == NULL)
		return false;

	*state = CONNECTION_WIFI_STATE_DEACTIVATED;
	powered = connman_get_technology_powered(technology);
	tethering = connman_get_technology_tethering(technology);

	if (powered) {
		if (!tethering) {
			connected = connman_get_technology_connected(
								technology);
			if (connected)
				*state = CONNECTION_WIFI_STATE_CONNECTED;
			else
				*state = CONNECTION_WIFI_STATE_DISCONNECTED;
		}
	}

	return true;
}

bool _connection_libnet_get_ethernet_state(connection_ethernet_state_e* state)
{
	struct connman_technology *technology;
	bool powered;
	bool connected;

	technology = connman_get_technology(TECH_TYPE_ETHERNET);
	if (technology == NULL)
		return false;

	*state = CONNECTION_ETHERNET_STATE_DEACTIVATED;
	powered = connman_get_technology_powered(technology);

	if (powered) {
		connected = connman_get_technology_connected(technology);
		if (connected)
			*state = CONNECTION_ETHERNET_STATE_CONNECTED;
		else
			*state = CONNECTION_ETHERNET_STATE_DISCONNECTED;
	}

	return true;
}

bool _connection_libnet_get_bluetooth_state(connection_bt_state_e* state)
{
	struct connman_technology *technology;
	bool powered;
	bool connected;

	technology = connman_get_technology(TECH_TYPE_BLUETOOTH);
	if (technology == NULL)
		return false;

	*state = CONNECTION_BT_STATE_DEACTIVATED;
	powered = connman_get_technology_powered(technology);

	if (powered) {
		connected = connman_get_technology_connected(technology);
		if (connected)
			*state = CONNECTION_BT_STATE_CONNECTED;
		else
			*state = CONNECTION_BT_STATE_DISCONNECTED;
	}

	return true;
}

int _connection_libnet_get_profile_iterator(connection_iterator_type_e type,
				connection_profile_iterator_h *profile_iter_h)
{
	int count = 0;
	int rv = CONNECTION_ERROR_NONE;
	net_profile_info_t *profiles = NULL;

	struct _profile_list_s all_profiles = {0, 0, NULL};

	__libnet_clear_profile_list(&profile_iterator);

	rv = __libnet_get_profile_list(NET_DEVICE_MAX, &all_profiles.profiles,
							&all_profiles.count);

	if (rv != CONNECTION_ERROR_NONE) {
		if (rv == CONNECTION_ERROR_NO_CONNECTION) {
			*profile_iter_h = &profile_iterator;
			return CONNECTION_ERROR_NONE;
		} else
			return CONNECTION_ERROR_OPERATION_FAILED;
	}

	*profile_iter_h = &profile_iterator;

	switch (type) {
	case CONNECTION_ITERATOR_TYPE_REGISTERED:
		count = all_profiles.count;
		CONNECTION_LOG(CONNECTION_INFO, "Total profile count : %d\n",
									count);

		if (count == 0)
			return CONNECTION_ERROR_NONE;

		profile_iterator.profiles = all_profiles.profiles;

		break;
	case CONNECTION_ITERATOR_TYPE_CONNECTED:
		count = __libnet_get_connected_count(&all_profiles);
		CONNECTION_LOG(CONNECTION_INFO,
				"Total connected profile count : %d\n", count);

		if (count == 0)
			return CONNECTION_ERROR_NONE;

		profiles = g_try_new0(net_profile_info_t, count);
		if (profiles == NULL) {
			__libnet_clear_profile_list(&all_profiles);
			return CONNECTION_ERROR_OUT_OF_MEMORY;
		}

		profile_iterator.profiles = profiles;

		__libnet_copy_connected_profile(&profiles, &all_profiles);

		__libnet_clear_profile_list(&all_profiles);
	}

	profile_iterator.count = count;

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_iterator_next(connection_profile_iterator_h profile_iter_h, connection_profile_h *profile)
{
	if (profile_iter_h != &profile_iterator)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	if (profile_iterator.count <= profile_iterator.next)
		return CONNECTION_ERROR_ITERATOR_END;

	*profile = &profile_iterator.profiles[profile_iterator.next];
	profile_iterator.next++;

	return CONNECTION_ERROR_NONE;
}

bool _connection_libnet_iterator_has_next(connection_profile_iterator_h profile_iter_h)
{
	if (profile_iter_h != &profile_iterator)
		return false;

	if (profile_iterator.count <= profile_iterator.next)
		return false;

	return true;
}

int _connection_libnet_destroy_iterator(connection_profile_iterator_h profile_iter_h)
{
	if (profile_iter_h != &profile_iterator)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	__libnet_clear_profile_list(&profile_iterator);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_current_profile(connection_profile_h *profile)
{
	net_profile_info_t active_profile;
	int rv;

	rv = __libnet_get_active_net_info(&active_profile);
	if (rv == CONNECTION_ERROR_NO_CONNECTION)
		return CONNECTION_ERROR_NO_CONNECTION;

	*profile = g_try_malloc0(sizeof(net_profile_info_t));
	if (*profile == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	memcpy(*profile, &active_profile, sizeof(net_profile_info_t));
	prof_handle_list = g_slist_append(prof_handle_list, *profile);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_open_profile(connection_profile_h profile, connection_opened_cb callback, void* user_data)
{
	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	struct connman_service *service =
				_connection_libnet_get_service_h(profile);
	if (service == NULL)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	if (connman_service_connect(service,
					__libnet_opened_connected_cb, NULL) !=
					CONNMAN_LIB_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	__libnet_set_opened_cb(callback, user_data);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_cellular_service_profile(connection_cellular_service_type_e type, connection_profile_h *profile)
{
/*
	int i = 0;
	int j = 0;
	int rv = NET_ERR_NONE;
	net_service_type_t service_type = _connection_profile_convert_to_libnet_cellular_service_type(type);

	struct _profile_list_s cellular_profiles = {0, 0, NULL};

	TODO:
	rv = net_get_profile_list(NET_DEVICE_CELLULAR, &cellular_profiles.profiles, &cellular_profiles.count);
	if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	for (;i < cellular_profiles.count;i++)
		if (cellular_profiles.profiles[i].ProfileInfo.Pdp.ServiceType == service_type)
			break;

	if (i >= cellular_profiles.count)
		return CONNECTION_ERROR_OPERATION_FAILED;

	*profile = g_try_malloc0(sizeof(net_profile_info_t));
	if (*profile == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	memcpy(*profile, &cellular_profiles.profiles[i], sizeof(net_profile_info_t));

	if (cellular_profiles.profiles[i].ProfileInfo.Pdp.DefaultConn)
		goto done;

	if (type != CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET &&
	    type != CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_INTERNET)
		goto done;

	for (;j < cellular_profiles.count;j++) {
		if (i == j)
			continue;

		if (cellular_profiles.profiles[j].ProfileInfo.Pdp.ServiceType != service_type)
			continue;

		if (cellular_profiles.profiles[j].ProfileInfo.Pdp.DefaultConn) {
			memcpy(*profile, &cellular_profiles.profiles[j], sizeof(net_profile_info_t));
			goto done;
		}
	}

done:
	prof_handle_list = g_slist_append(prof_handle_list, *profile);
*/

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_set_cellular_service_profile_sync(connection_cellular_service_type_e type, connection_profile_h profile)
{
	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	connection_cellular_service_type_e service_type;

	service_type = _profile_convert_to_connection_cellular_service_type(profile_info->ProfileInfo.Pdp.ServiceType);

	if (service_type != type)
		return CONNECTION_ERROR_INVALID_PARAMETER;
	TODO:
	if (net_set_default_cellular_service_profile(profile_info->profile_name) != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;
	 */

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_set_cellular_service_profile_async(connection_cellular_service_type_e type,
			connection_profile_h profile, connection_set_default_cb callback, void* user_data)
{
	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	connection_cellular_service_type_e service_type;

	service_type = _profile_convert_to_connection_cellular_service_type(profile_info->ProfileInfo.Pdp.ServiceType);

	if (service_type != type)
		return CONNECTION_ERROR_INVALID_PARAMETER;
	TODO:
	if (net_set_default_cellular_service_profile_async(profile_info->profile_name) != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;
	 */

	__libnet_set_default_cb(callback, user_data);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_close_profile(connection_profile_h profile,
				connection_closed_cb callback, void *user_data)
{
	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	struct connman_service *service =
				_connection_libnet_get_service_h(profile);
	if (service == NULL)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	if (connman_service_disconnect(service,
				__libnet_closed_disconnected_cb, NULL) !=
				CONNMAN_LIB_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	__libnet_set_closed_cb(callback, user_data);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_add_route(const char *interface_name, const char *host_address)
{
	char *endstr = strrchr(host_address, '.');

	if (endstr == NULL ||
	    strcmp(endstr, ".0") == 0 ||
	    strncmp(host_address, "0.", 2) == 0 ||
	    strstr(host_address, ".0.") != NULL ||
	    strstr(host_address, "255") != NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid IP address Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}
	/*
	TODO:
	if (net_add_route(host_address, interface_name) != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;
	 */

	return CONNECTION_ERROR_NONE;
}

void _connection_libnet_add_to_profile_list(connection_profile_h profile)
{
	prof_handle_list = g_slist_append(prof_handle_list, profile);
}

void _connection_libnet_remove_from_profile_list(connection_profile_h profile)
{
	prof_handle_list = g_slist_remove(prof_handle_list, profile);
	g_free(profile);
}

bool _connection_libnet_add_to_profile_cb_list(connection_profile_h profile,
		connection_profile_state_changed_cb callback, void *user_data)
{
	net_profile_info_t *profile_info = profile;
	char *profile_name;
	struct _profile_cb_s *profile_cb_info;
	struct connman_service *service;

	profile_name = g_strdup(profile_info->profile_name);

	profile_cb_info = g_try_malloc0(sizeof(struct _profile_cb_s));
	if (profile_cb_info == NULL) {
		g_free(profile_name);
		return false;
	}

	profile_cb_info->callback = callback;
	profile_cb_info->user_data = user_data;

	service = _connection_libnet_get_service_h(profile);
	if (service != NULL) {
		connman_service_set_property_changed_cb(service,
					SERVICE_PROP_STATE,
					__libnet_service_state_changed_cb,
					NULL);
	}

	if (g_hash_table_size(services_changed_cb_table) == 0)
		connman_set_services_changed_cb(
				__libnet_services_changed_callback, NULL);

	if (g_hash_table_size(profile_cb_table) == 0)
		g_hash_table_insert(services_changed_cb_table,
			GINT_TO_POINTER(SERVICE_PROP_STATE),
			(gpointer)__libnet_set_services_state_changed_cb);

	g_hash_table_insert(profile_cb_table, profile_name, profile_cb_info);

	return true;
}

void _connection_libnet_remove_from_profile_cb_list(connection_profile_h profile)
{
	net_profile_info_t *profile_info = profile;

	struct connman_service *service =
				_connection_libnet_get_service_h(profile);
	if (service != NULL)
		connman_service_unset_property_changed_cb(service,
							SERVICE_PROP_STATE);

	if (g_hash_table_size(services_changed_cb_table) == 1)
		connman_unset_services_changed_cb();

	if (g_hash_table_size(profile_cb_table) == 1)
		g_hash_table_remove(services_changed_cb_table,
					GINT_TO_POINTER(SERVICE_PROP_STATE));

	g_hash_table_remove(profile_cb_table, profile_info->profile_name);
}

int _connection_libnet_set_statistics(net_device_t device_type, net_statistics_type_e statistics_type)
{
	/*
	TODO:
	if (net_set_statistics(device_type, statistics_type) != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;
	 */

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_statistics(net_statistics_type_e statistics_type, unsigned long long *size)
{
	/*
	TODO:
	if (net_get_statistics(NET_DEVICE_WIFI, statistics_type, size) != NET_ERR_NONE)
			return CONNECTION_ERROR_OPERATION_FAILED;
	 */

		return CONNECTION_ERROR_NONE;
}

struct connman_service *_connection_libnet_get_service_h(
						connection_profile_h profile)
{
	struct connman_service *service =
		connman_get_service_with_path(
				((net_profile_info_t *) profile)->profile_name);

	return service;
}

int _connection_libnet_get_default_device_type(net_device_t *device_type)
{
	int rv;
	GList *services_list;
	struct connman_service *default_service;

	services_list = connman_get_services();
	if (services_list == NULL)
		return CONNECTION_ERROR_NO_CONNECTION;

	rv = __libnet_get_default_service(services_list, &default_service);
	if (rv == CONNECTION_ERROR_NO_CONNECTION)
		return CONNECTION_ERROR_NO_CONNECTION;

	*device_type = __libnet_service_type_string2type(
				connman_service_get_type(default_service));

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_default_ip_address(char **ip_address)
{
	int rv;
	GList *services_list;
	struct connman_service *default_service;

	services_list = connman_get_services();
	if (services_list == NULL)
		return CONNECTION_ERROR_NO_CONNECTION;

	rv = __libnet_get_default_service(services_list, &default_service);
	if (rv == CONNECTION_ERROR_NO_CONNECTION)
		return CONNECTION_ERROR_NO_CONNECTION;

	return __libnet_get_ipv4_ip_address(default_service, ip_address);
}

int _connection_libnet_get_default_proxy(char **proxy_address)
{
	int rv;
	GList *services_list;
	struct connman_service *default_service;

	services_list = connman_get_services();
	if (services_list == NULL)
		return CONNECTION_ERROR_NO_CONNECTION;

	rv = __libnet_get_default_service(services_list, &default_service);
	if (rv == CONNECTION_ERROR_NO_CONNECTION)
		return CONNECTION_ERROR_NO_CONNECTION;

	return _connection_libnet_get_proxy_address(
		default_service, proxy_address);
}

net_proxy_type_t _connection_libnet_proxy_type_string2type(const char *str)
{
	if (str == NULL)
		return NET_PROXY_TYPE_UNKNOWN;

	if (g_strcmp0(str, "direct") == 0)
		return NET_PROXY_TYPE_DIRECT;
	if (g_strcmp0(str, "manual") == 0)
		return NET_PROXY_TYPE_MANUAL;
	if (g_strcmp0(str, "auto") == 0)
		return NET_PROXY_TYPE_AUTO;

	return NET_PROXY_TYPE_UNKNOWN;
}

int _connection_libnet_get_proxy_address(struct connman_service *service,
							char **proxy_address)
{
	const struct service_proxy *proxy;
	net_proxy_type_t proxy_type;

	if (service == NULL)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	proxy = connman_service_get_proxy_info(service);
	if (proxy == NULL || proxy->method == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	proxy_type = _connection_libnet_proxy_type_string2type(proxy->method);

	if (proxy_type == NET_PROXY_TYPE_AUTO && proxy->url != NULL)
		*proxy_address = g_strdup(proxy->url);
	else if (proxy_type == NET_PROXY_TYPE_MANUAL && proxy->servers != NULL)
		*proxy_address = g_strdup(proxy->servers[0]);
	else
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (*proxy_address == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	return CONNECTION_ERROR_NONE;
}


void _connection_libnet_set_type_changed_cb()
{
	last_default_service_property.device_type = NET_DEVICE_UNKNOWN;
	_connection_libnet_get_default_device_type(
				&last_default_service_property.device_type);

	if (g_hash_table_size(services_changed_cb_table) == 0)
		connman_set_services_changed_cb(
				__libnet_services_changed_callback, NULL);

	/*
	 * Service has not the changed signal for the property 'Type',
	 * so uses the changed signal 'ServicesChanged' and 'State'
	 * of connman_manager
	 */
	g_hash_table_insert(services_changed_cb_table,
			GINT_TO_POINTER(SERVICE_PROP_TYPE),
			(gpointer)__libnet_set_default_service_changed_cb);

	connman_set_manager_property_changed_cb(
				__libnet_connman_manager_property_changed_cb,
				MANAGER_PROP_STATE,
				NULL);
}

void _connection_libnet_unset_type_changed_cb()
{
	if (g_hash_table_size(services_changed_cb_table) == 1)
		connman_unset_services_changed_cb();

	g_hash_table_remove(services_changed_cb_table,
					GINT_TO_POINTER(SERVICE_PROP_TYPE));

	connman_unset_manager_property_changed_cb(MANAGER_PROP_STATE);
}

void _connection_libnet_set_ip_changed_cb()
{
	g_free(last_default_service_property.ip_address);
	last_default_service_property.ip_address = NULL;
	_connection_libnet_get_default_ip_address(
				&last_default_service_property.ip_address);

	if (g_hash_table_size(services_changed_cb_table) == 0)
		connman_set_services_changed_cb(
				__libnet_services_changed_callback, NULL);

	/*
	 * Only supported IPV4
	 */
	g_hash_table_insert(services_changed_cb_table,
			GINT_TO_POINTER(SERVICE_PROP_IPV4),
			(gpointer)__libnet_set_default_service_changed_cb);
}

void _connection_libnet_unset_ip_changed_cb()
{
	if (g_hash_table_size(services_changed_cb_table) == 1)
		connman_unset_services_changed_cb();

	/*
	 * Only supported IPV4
	 */
	g_hash_table_remove(services_changed_cb_table,
					GINT_TO_POINTER(SERVICE_PROP_IPV4));

	g_free(last_default_service_property.ip_address);
	last_default_service_property.ip_address = NULL;
}

void _connection_libnet_set_proxy_changed_cb()
{
	g_free(last_default_service_property.proxy_address);
	last_default_service_property.proxy_address = NULL;
	_connection_libnet_get_default_proxy(
				&last_default_service_property.proxy_address);

	if (g_hash_table_size(services_changed_cb_table) == 0)
		connman_set_services_changed_cb(
				__libnet_services_changed_callback, NULL);

	g_hash_table_insert(services_changed_cb_table,
			GINT_TO_POINTER(SERVICE_PROP_PROXY),
			(gpointer)__libnet_set_default_service_changed_cb);
}

void _connection_libnet_unset_proxy_changed_cb()
{
	if (g_hash_table_size(services_changed_cb_table) == 1)
		connman_unset_services_changed_cb();

	g_hash_table_remove(services_changed_cb_table,
					GINT_TO_POINTER(SERVICE_PROP_PROXY));

	g_free(last_default_service_property.proxy_address);
	last_default_service_property.proxy_address = NULL;
}

int _connection_libnet_get_net_state_type(connection_profile_h profile,
						net_state_type_t *profile_state)
{
	struct connman_service *service =
				_connection_libnet_get_service_h(profile);
	if (service == NULL)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	*profile_state = __libnet_service_state_string2type(
					connman_service_get_state(service));

	return CONNECTION_ERROR_NONE;
}
