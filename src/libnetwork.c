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
#include <system_info.h>
#include <arpa/inet.h>

#include "net_connection_private.h"

static GSList *prof_handle_list = NULL;
static GHashTable *profile_cb_table = NULL;

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
	connection_reset_cb reset_profile_cb;
	libnet_ethernet_cable_state_changed_cb ethernet_cable_state_changed_cb;
	void *opened_user_data;
	void *closed_user_data;
	void *set_default_user_data;
	void *reset_profile_user_data;
	bool registered;
	bool is_created;
};

struct managed_idle_data {
	GSourceFunc func;
	gpointer user_data;
	guint id;
};

static struct _profile_list_s profile_iterator = {0, 0, NULL};
static struct _libnet_s libnet = {NULL, NULL, NULL, NULL, NULL, NULL, false};
static __thread GSList *managed_idler_list = NULL;

bool _connection_is_created(void)
{
	return libnet.is_created;
}

static connection_error_e __libnet_convert_to_cp_error_type(net_err_t err_type)
{
	switch (err_type) {
	case NET_ERR_NONE:
		return CONNECTION_ERROR_NONE;
	case NET_ERR_APP_ALREADY_REGISTERED:
		return CONNECTION_ERROR_INVALID_OPERATION;
	case NET_ERR_APP_NOT_REGISTERED:
		return CONNECTION_ERROR_INVALID_OPERATION;
	case NET_ERR_NO_ACTIVE_CONNECTIONS:
		return CONNECTION_ERROR_NO_CONNECTION;
	case NET_ERR_ACTIVE_CONNECTION_EXISTS:
		return CONNECTION_ERROR_ALREADY_EXISTS;
	case NET_ERR_CONNECTION_DHCP_FAILED:
		return CONNECTION_ERROR_DHCP_FAILED;
	case NET_ERR_CONNECTION_INVALID_KEY:
		return CONNECTION_ERROR_INVALID_KEY;
	case NET_ERR_IN_PROGRESS:
		return CONNECTION_ERROR_NOW_IN_PROGRESS;
	case NET_ERR_OPERATION_ABORTED:
		return CONNECTION_ERROR_OPERATION_ABORTED;
	case NET_ERR_TIME_OUT:
		return CONNECTION_ERROR_NO_REPLY;
	case NET_ERR_ACCESS_DENIED:
		return CONNECTION_ERROR_PERMISSION_DENIED;
	default:
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
}

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
	case CONNECTION_ERROR_PERMISSION_DENIED:
		return "PERMISSION_DENIED";
	case CONNECTION_ERROR_NOT_SUPPORTED:
		return "NOT_SUPPORTED";
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

static void __libnet_set_reset_profile_cb(connection_opened_cb user_cb, void *user_data)
{
	if (user_cb != NULL) {
		libnet.reset_profile_cb = user_cb;
		libnet.reset_profile_user_data = user_data;
	}
}

static void __libnet_set_opened_cb(connection_opened_cb user_cb, void *user_data)
{
	if (user_cb) {
		libnet.opened_cb = user_cb;
		libnet.opened_user_data = user_data;
	}
}

static gboolean __libnet_reset_profile_cb_idle(gpointer data)
{
	connection_error_e result = (connection_error_e)data;

	if (libnet.reset_profile_cb != NULL)
		libnet.reset_profile_cb(result, libnet.reset_profile_user_data);

	libnet.reset_profile_cb = NULL;
	libnet.reset_profile_user_data = NULL;

	return FALSE;
}

static void __libnet_reset_profile_cb(connection_error_e result)
{
	if (_connection_is_created() != true) {
		CONNECTION_LOG(CONNECTION_ERROR, "Application is not registered"
				"If multi-threaded, thread integrity be broken.");
		return;
	}

	if (libnet.reset_profile_cb != NULL)
		_connection_callback_add(__libnet_reset_profile_cb_idle, (gpointer)result);
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

static void __libnet_default_cb(connection_error_e result)
{
	if (libnet.set_default_cb)
		libnet.set_default_cb(result, libnet.set_default_user_data);

	libnet.set_default_cb = NULL;
	libnet.set_default_user_data = NULL;
}

static void __libnet_set_ethernet_cable_state_changed_cb(
		libnet_ethernet_cable_state_changed_cb user_cb)
{
	libnet.ethernet_cable_state_changed_cb = user_cb;
}

static void __libnet_ethernet_cable_state_changed_cb(
		connection_ethernet_cable_state_e state)
{
	if (libnet.ethernet_cable_state_changed_cb)
		libnet.ethernet_cable_state_changed_cb(state);
}

static void __libnet_state_changed_cb(char *profile_name, connection_profile_state_e state)
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

static void __libnet_clear_profile_list(struct _profile_list_s *profile_list)
{
	if (profile_list->count > 0)
		g_free(profile_list->profiles);

	profile_list->count = 0;
	profile_list->next = 0;
	profile_list->profiles = NULL;
}

static void __libnet_evt_cb(net_event_info_t*  event_cb, void* user_data)
{
	bool is_requested = false;
	connection_error_e result = CONNECTION_ERROR_NONE;

	switch (event_cb->Event) {
	case NET_EVENT_OPEN_RSP:
		is_requested = true;
		/* fall through */
	case NET_EVENT_OPEN_IND:
		result = __libnet_convert_to_cp_error_type(event_cb->Error);
		CONNECTION_LOG(CONNECTION_INFO, "Got connection open %s : %s\n",
					(is_requested) ? "RSP":"IND",
					__libnet_convert_cp_error_type_to_string(result));

		if (is_requested)
			__libnet_opened_cb(result);

		switch (event_cb->Error) {
		case NET_ERR_NONE:
		case NET_ERR_ACTIVE_CONNECTION_EXISTS:
			CONNECTION_LOG(CONNECTION_INFO, "'Open connection' succeeded\n");

			__libnet_state_changed_cb(event_cb->ProfileName, CONNECTION_PROFILE_STATE_CONNECTED);
			return;
		default:
			CONNECTION_LOG(CONNECTION_ERROR, "'Open connection' failed!! [%s]\n",
						__libnet_convert_cp_error_type_to_string(result));
		}

		__libnet_state_changed_cb(event_cb->ProfileName, CONNECTION_PROFILE_STATE_DISCONNECTED);

		break;
	case NET_EVENT_CLOSE_RSP:
		is_requested = true;
		/* fall through */
	case NET_EVENT_CLOSE_IND:
		result = __libnet_convert_to_cp_error_type(event_cb->Error);
		CONNECTION_LOG(CONNECTION_INFO, "Got connection close %s : %s\n",
					(is_requested) ? "RSP":"IND",
					__libnet_convert_cp_error_type_to_string(result));

		if (is_requested)
			__libnet_closed_cb(result);

		switch (event_cb->Error) {
		case NET_ERR_NONE:
			CONNECTION_LOG(CONNECTION_INFO, "'Close connection' succeeded!\n");

			__libnet_state_changed_cb(event_cb->ProfileName, CONNECTION_PROFILE_STATE_DISCONNECTED);
			return;
		default:
			CONNECTION_LOG(CONNECTION_ERROR, "'Close connection' failed!! [%s]\n",
						__libnet_convert_cp_error_type_to_string(result));
		}

		break;
	case NET_EVENT_NET_STATE_IND:
		CONNECTION_LOG(CONNECTION_INFO, "Got State changed IND\n");

		if (event_cb->Datalength != sizeof(net_state_type_t))
			return;

		net_state_type_t *profile_state = (net_state_type_t*)event_cb->Data;
		connection_profile_state_e cp_state = _profile_convert_to_cp_state(*profile_state);

		CONNECTION_LOG(CONNECTION_INFO,
				"Profile State : %s, profile name : %s\n",
				__libnet_convert_cp_state_to_string(cp_state),
				event_cb->ProfileName);

		__libnet_state_changed_cb(event_cb->ProfileName, cp_state);

		break;
	case NET_EVENT_WIFI_SCAN_IND:
	case NET_EVENT_WIFI_SCAN_RSP:
		CONNECTION_LOG(CONNECTION_INFO, "Got wifi scan IND\n");
		break;
	case NET_EVENT_WIFI_POWER_IND:
	case NET_EVENT_WIFI_POWER_RSP:
		CONNECTION_LOG(CONNECTION_INFO, "Got wifi power IND\n");
		break;
	case NET_EVENT_CELLULAR_SET_DEFAULT_RSP:
		result = __libnet_convert_to_cp_error_type(event_cb->Error);
		CONNECTION_LOG(CONNECTION_INFO, "Got set default profile RSP %d\n", result);
		__libnet_default_cb(result);
		break;
	case NET_EVENT_WIFI_WPS_RSP:
		CONNECTION_LOG(CONNECTION_INFO, "Got wifi WPS RSP\n");
		/* fall through */
	case NET_EVENT_CELLULAR_RESET_DEFAULT_RSP:
		result = __libnet_convert_to_cp_error_type(event_cb->Error);
		CONNECTION_LOG(CONNECTION_INFO, "Got reset default profile RSP %d", result);
		__libnet_reset_profile_cb(result);

	case NET_EVENT_ETHERNET_CABLE_ATTACHED:
		CONNECTION_LOG(CONNECTION_INFO, "Got Ethernet cable Attached Indication\n");
		__libnet_ethernet_cable_state_changed_cb(CONNECTION_ETHERNET_CABLE_ATTACHED);
		break;
	case NET_EVENT_ETHERNET_CABLE_DETACHED:
		CONNECTION_LOG(CONNECTION_INFO, "Got Ethernet cable detached Indication\n");
		__libnet_ethernet_cable_state_changed_cb(CONNECTION_ETHERNET_CABLE_DETACHED);
		break;
	default :
		CONNECTION_LOG(CONNECTION_ERROR, "Error! Unknown Event\n\n");
		break;
	}
}

static int __libnet_check_address_type(int address_family, const char *address)
{
	struct in6_addr buf;
	int err = 0;

	err = inet_pton(address_family, address, &buf);
	if(err > 0)
		return 1;

	return 0;
}

int __libnet_get_connected_count(struct _profile_list_s *profile_list)
{
	int count = 0;
	int i = 0;

	for (;i < profile_list->count;i++) {
		if (profile_list->profiles[i].ProfileState == NET_STATE_TYPE_ONLINE ||
		    profile_list->profiles[i].ProfileState == NET_STATE_TYPE_READY)
			count++;
	}

	return count;
}

void __libnet_copy_connected_profile(net_profile_info_t **dest, struct _profile_list_s *source)
{
	int i = 0;

	for (;i < source->count;i++) {
		if (source->profiles[i].ProfileState == NET_STATE_TYPE_ONLINE ||
		    source->profiles[i].ProfileState == NET_STATE_TYPE_READY) {
			memcpy(*dest, &source->profiles[i], sizeof(net_profile_info_t));
			(*dest)++;
		}
	}
}

int _connection_libnet_init(void)
{
	int rv;

	if (!libnet.registered) {
		rv = net_register_client_ext((net_event_cb_t)__libnet_evt_cb, NET_DEVICE_DEFAULT, NULL);
		if (rv != NET_ERR_NONE)
			return false;

		libnet.registered = true;

		if (profile_cb_table == NULL)
			profile_cb_table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	}

	return NET_ERR_NONE;
}

bool _connection_libnet_deinit(void)
{
	if (libnet.registered) {
		if (net_deregister_client_ext(NET_DEVICE_DEFAULT) != NET_ERR_NONE)
			return false;

		libnet.registered = false;

		if (profile_cb_table) {
			g_hash_table_destroy(profile_cb_table);
			profile_cb_table = NULL;
		}

		__libnet_clear_profile_list(&profile_iterator);

		if (prof_handle_list) {
			g_slist_free_full(prof_handle_list, g_free);
			prof_handle_list = NULL;
		}
	}

	return true;
}

bool _connection_libnet_check_profile_validity(connection_profile_h profile)
{
	GSList *list;
	int i = 0;

	if (profile == NULL)
		return false;

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

	cb_info = g_hash_table_lookup(profile_cb_table, profile_info->ProfileName);
	if (cb_info != NULL)
		return true;

	return false;
}


int _connection_libnet_get_wifi_state(connection_wifi_state_e *state)
{
	int rv;
	net_wifi_state_t wlan_state;
	net_profile_name_t profile_name;

	rv = net_get_wifi_state(&wlan_state, &profile_name);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to get Wi-Fi state[%d]", rv);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	switch (wlan_state) {
	case WIFI_OFF:
		*state = CONNECTION_WIFI_STATE_DEACTIVATED;
		break;
	case WIFI_ON:
	case WIFI_CONNECTING:
		*state = CONNECTION_WIFI_STATE_DISCONNECTED;
		break;
	case WIFI_CONNECTED:
	case WIFI_DISCONNECTING:
		*state = CONNECTION_WIFI_STATE_CONNECTED;
		break;
	default :
		CONNECTION_LOG(CONNECTION_ERROR, "Error!! Unknown state\n");
		return CONNECTION_ERROR_INVALID_OPERATION;
	}

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_ethernet_state(connection_ethernet_state_e* state)
{
	int rv;
	struct _profile_list_s ethernet_profiles = {0, 0, NULL};
	rv = net_get_profile_list(NET_DEVICE_ETHERNET, &ethernet_profiles.profiles, &ethernet_profiles.count);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	}

	if (ethernet_profiles.count == 0) {
		state = CONNECTION_ETHERNET_STATE_DEACTIVATED;
		return CONNECTION_ERROR_NONE;
	}

	switch (ethernet_profiles.profiles->ProfileState) {
	case NET_STATE_TYPE_ONLINE:
	case NET_STATE_TYPE_READY:
		*state = CONNECTION_ETHERNET_STATE_CONNECTED;
		break;
	case NET_STATE_TYPE_IDLE:
	case NET_STATE_TYPE_FAILURE:
	case NET_STATE_TYPE_ASSOCIATION:
	case NET_STATE_TYPE_CONFIGURATION:
	case NET_STATE_TYPE_DISCONNECT:
		*state = CONNECTION_ETHERNET_STATE_DISCONNECTED;
		break;
	default:
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	__libnet_clear_profile_list(&ethernet_profiles);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_ethernet_cable_state(connection_ethernet_cable_state_e* state)
{
	int rv = 0;
	int status = 0;

	rv = net_get_ethernet_cable_state(&status);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to get ethernet cable state[%d]", rv);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	if(status == 1)
		*state = CONNECTION_ETHERNET_CABLE_ATTACHED;
	else
		*state = CONNECTION_ETHERNET_CABLE_DETACHED;
	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_set_ethernet_cable_state_changed_cb(
		libnet_ethernet_cable_state_changed_cb callback)
{
	__libnet_set_ethernet_cable_state_changed_cb(callback);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_bluetooth_state(connection_bt_state_e* state)
{
	int i = 0;
	int rv = 0;
	struct _profile_list_s bluetooth_profiles = {0, 0, NULL};
	rv = net_get_profile_list(NET_DEVICE_BLUETOOTH, &bluetooth_profiles.profiles, &bluetooth_profiles.count);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	}

	if (bluetooth_profiles.count == 0) {
		*state = CONNECTION_BT_STATE_DEACTIVATED;
		return CONNECTION_ERROR_NONE;
	}

	for (; i < bluetooth_profiles.count; i++) {
		switch (bluetooth_profiles.profiles[i].ProfileState) {
		case NET_STATE_TYPE_ONLINE:
		case NET_STATE_TYPE_READY:
			*state = CONNECTION_BT_STATE_CONNECTED;
			goto done;
		case NET_STATE_TYPE_IDLE:
		case NET_STATE_TYPE_FAILURE:
		case NET_STATE_TYPE_ASSOCIATION:
		case NET_STATE_TYPE_CONFIGURATION:
		case NET_STATE_TYPE_DISCONNECT:
			*state = CONNECTION_BT_STATE_DISCONNECTED;
			break;
		default:
			__libnet_clear_profile_list(&bluetooth_profiles);
			return CONNECTION_ERROR_OPERATION_FAILED;
		}
	}

done:
	__libnet_clear_profile_list(&bluetooth_profiles);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_profile_iterator(connection_iterator_type_e type, connection_profile_iterator_h* profile_iter_h)
{
	int count = 0;
	int rv;
	net_profile_info_t *profiles = NULL;

	struct _profile_list_s all_profiles = {0, 0, NULL};

	__libnet_clear_profile_list(&profile_iterator);

	rv = net_get_profile_list(NET_DEVICE_MAX, &all_profiles.profiles, &all_profiles.count);

	if (rv != NET_ERR_NONE) {
		if (rv == NET_ERR_NO_SERVICE) {
			*profile_iter_h = &profile_iterator;
			return CONNECTION_ERROR_NONE;
		} else
			return CONNECTION_ERROR_OPERATION_FAILED;
	}

	*profile_iter_h = &profile_iterator;

	switch (type) {
	case CONNECTION_ITERATOR_TYPE_REGISTERED:
		count = all_profiles.count;
		CONNECTION_LOG(CONNECTION_INFO, "Total profile count : %d\n", count);

		if (count == 0)
			return CONNECTION_ERROR_NONE;

		profile_iterator.profiles = all_profiles.profiles;

		break;
	case CONNECTION_ITERATOR_TYPE_CONNECTED:
		count = __libnet_get_connected_count(&all_profiles);
		CONNECTION_LOG(CONNECTION_INFO, "Total connected profile count : %d\n", count);

		if (count == 0)
			return CONNECTION_ERROR_NONE;

		profiles = g_try_new0(net_profile_info_t, count);
		if (profiles == NULL) {
			__libnet_clear_profile_list(&all_profiles);
			return CONNECTION_ERROR_OUT_OF_MEMORY;
		break;
	case CONNECTION_ITERATOR_TYPE_DEFAULT:
			/* To do : Not supported yet */
		break;
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

	rv = net_get_active_net_info(&active_profile);
	if (rv == NET_ERR_NO_SERVICE)
		return CONNECTION_ERROR_NO_CONNECTION;
	else if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	*profile = g_try_malloc0(sizeof(net_profile_info_t));
	if (*profile == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	memcpy(*profile, &active_profile, sizeof(net_profile_info_t));
	prof_handle_list = g_slist_append(prof_handle_list, *profile);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_reset_profile(connection_reset_option_e type,
		connection_cellular_subscriber_id_e id, connection_reset_cb callback, void *user_data)
{
	int rv;

	rv = net_reset_profile(type, id);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to add profile[%d]", rv);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	__libnet_set_reset_profile_cb(callback, user_data);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_open_profile(connection_profile_h profile, connection_opened_cb callback, void* user_data)
{
	int rv;

	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid parameter");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	net_profile_info_t *profile_info = profile;

	rv = net_open_connection_with_profile(profile_info->ProfileName);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	__libnet_set_opened_cb(callback, user_data);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_cellular_service_profile(connection_cellular_service_type_e type, connection_profile_h *profile)
{
	int i = 0;
	int j = 0;
	int rv = NET_ERR_NONE;
	net_service_type_t service_type = _connection_profile_convert_to_libnet_cellular_service_type(type);

	struct _profile_list_s cellular_profiles = {0, 0, NULL};

	rv = net_get_profile_list(NET_DEVICE_CELLULAR, &cellular_profiles.profiles, &cellular_profiles.count);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to get profile list (%d)", rv);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

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

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_set_cellular_service_profile_sync(connection_cellular_service_type_e type, connection_profile_h profile)
{
	int rv;

	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	net_profile_info_t *profile_info = profile;
	connection_cellular_service_type_e service_type;

	service_type = _profile_convert_to_connection_cellular_service_type(profile_info->ProfileInfo.Pdp.ServiceType);

	if (service_type != type)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	rv = net_set_default_cellular_service_profile(profile_info->ProfileName);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_set_cellular_service_profile_async(connection_cellular_service_type_e type,
			connection_profile_h profile, connection_set_default_cb callback, void* user_data)
{
	int rv;

	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	net_profile_info_t *profile_info = profile;
	connection_cellular_service_type_e service_type;

	service_type = _profile_convert_to_connection_cellular_service_type(profile_info->ProfileInfo.Pdp.ServiceType);

	if (service_type != type)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	rv = net_set_default_cellular_service_profile_async(profile_info->ProfileName);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	__libnet_set_default_cb(callback, user_data);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_close_profile(connection_profile_h profile, connection_closed_cb callback, void *user_data)
{
	int rv;

	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	net_profile_info_t *profile_info = profile;

	rv = net_close_connection(profile_info->ProfileName);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (net_close_connection(profile_info->ProfileName) != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	__libnet_set_closed_cb(callback, user_data);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_add_route(const char *interface_name, const char *host_address)
{
	int rv;
	char *endstr = NULL;
	int address_family = 0;

	if(__libnet_check_address_type(AF_INET, host_address))
		address_family = AF_INET;
	else
		return CONNECTION_ERROR_INVALID_PARAMETER;

	switch(address_family) {
		case AF_INET:
			endstr = strrchr(host_address, '.');
			if (endstr == NULL ||
					strcmp(endstr, ".0") == 0 ||
					strncmp(host_address, "0.", 2) == 0 ||
					strstr(host_address, "255") != NULL) {
				CONNECTION_LOG(CONNECTION_ERROR, "Invalid IP address Passed\n");
				return CONNECTION_ERROR_INVALID_PARAMETER;
			}
			break;
		default:
			return CONNECTION_ERROR_OPERATION_FAILED;
	}

	rv = net_add_route(host_address, interface_name, address_family);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_remove_route(const char *interface_name, const char *host_address)
{
	int rv;
	char *endstr = strrchr(host_address, '.');
	int address_family = 0;

	if (__libnet_check_address_type(AF_INET, host_address))
		address_family = AF_INET;
	else
		return CONNECTION_ERROR_INVALID_PARAMETER;

	switch(address_family) {
		case AF_INET:
			endstr = strrchr(host_address, '.');
			if (endstr == NULL ||
				strcmp(endstr, ".0") == 0 ||
				strncmp(host_address, "0.", 2) == 0 ||
				strstr(host_address, ".0.") != NULL ||strstr(host_address, "255") != NULL) {
				CONNECTION_LOG(CONNECTION_ERROR, "Invalid IP address Passed");
				return CONNECTION_ERROR_INVALID_PARAMETER;
			}
			break;
		default:
			return CONNECTION_ERROR_OPERATION_FAILED;
	}

	rv = net_remove_route(host_address, interface_name, address_family);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_add_route_ipv6(const char *interface_name, const char *host_address, const char *gateway)
{
	int rv;
	int address_family = 0;

	address_family = AF_INET6;
/*	if(__libnet_check_address_type(AF_INET6, host_address))
		address_family = AF_INET6;
	else
		return CONNECTION_ERROR_INVALID_PARAMETER;*/

	switch(address_family) {
		case AF_INET6:
			if (strncmp(host_address, "fe80:", 5) == 0 ||
				strncmp(host_address, "ff00:", 5) == 0 ||
				strncmp(host_address, "::", 2) == 0) {
				CONNECTION_LOG(CONNECTION_ERROR, "Invalid IP address Passed\n");
				return CONNECTION_ERROR_INVALID_PARAMETER;
			}
			break;
		default:
			return CONNECTION_ERROR_OPERATION_FAILED;
	}

	rv = net_add_route_ipv6(host_address, interface_name, address_family, gateway);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_remove_route_ipv6(const char *interface_name, const char *host_address, const char *gateway)
{
	int rv;
	int address_family = 0;

	address_family = AF_INET6;
/*	if (__libnet_check_address_type(AF_INET6, host_address))
		address_family = AF_INET6;
	else
		return CONNECTION_ERROR_INVALID_PARAMETER;*/

	switch(address_family) {
		case AF_INET6:
			if (strncmp(host_address, "fe80:", 5) == 0 ||
				strncmp(host_address, "ff00:", 5) == 0 ||
				strncmp(host_address, "::", 2) == 0) {
				CONNECTION_LOG(CONNECTION_ERROR, "Invalid IP address Passed\n");
				return CONNECTION_ERROR_INVALID_PARAMETER;
			}
			break;
		default:
			return CONNECTION_ERROR_OPERATION_FAILED;
	}

	rv = net_remove_route_ipv6(host_address, interface_name, address_family, gateway);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

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
	char *profile_name = g_strdup(profile_info->ProfileName);

	struct _profile_cb_s *profile_cb_info = g_try_malloc0(sizeof(struct _profile_cb_s));
	if (profile_cb_info == NULL) {
		g_free(profile_name);
		return false;
	}

	profile_cb_info->callback = callback;
	profile_cb_info->user_data = user_data;

	g_hash_table_insert(profile_cb_table, profile_name, profile_cb_info);

	return true;
}

bool _connection_libnet_remove_from_profile_cb_list(connection_profile_h profile)
{
	net_profile_info_t *profile_info = profile;
	if (g_hash_table_remove(profile_cb_table, profile_info->ProfileName) == TRUE)
		return true;

	return false;
}

int _connection_libnet_set_statistics(net_device_t device_type, net_statistics_type_e statistics_type)
{
	int rv;
	rv = net_set_statistics(device_type, statistics_type);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_statistics(net_statistics_type_e statistics_type, unsigned long long *size)
{
	int rv;
	rv = net_get_statistics(NET_DEVICE_WIFI, statistics_type, size);
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	}else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_set_cellular_subscriber_id(connection_profile_h profile,
		connection_cellular_subscriber_id_e sim_id)
{
	char *modem_path = NULL;
	net_profile_info_t *profile_info = (net_profile_info_t *)profile;

	if (net_get_cellular_modem_object_path(&modem_path, sim_id) != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Failed to get subscriber[%d]", sim_id);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	if (!modem_path) {
		CONNECTION_LOG(CONNECTION_ERROR, "NULL modem object path");
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	g_strlcpy(profile_info->ProfileInfo.Pdp.PSModemPath, modem_path,
				NET_PROFILE_NAME_LEN_MAX);
	g_free(modem_path);

	return CONNECTION_ERROR_NONE;
}

static void __connection_idle_destroy_cb(gpointer data)
{
	if (!data)
		return;

	managed_idler_list = g_slist_remove(managed_idler_list, data);
	g_free(data);
}

static gboolean __connection_idle_cb(gpointer user_data)
{
	struct managed_idle_data *data = (struct managed_idle_data *)user_data;

	if (!data)
		return FALSE;

	return data->func(data->user_data);
}

guint _connection_callback_add(GSourceFunc func, gpointer user_data)
{
	guint id;
	struct managed_idle_data *data;

	if (!func)
		return 0;

	data = g_try_new0(struct managed_idle_data, 1);
	if (!data)
		return 0;

	data->func = func;
	data->user_data = user_data;

	id = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, __connection_idle_cb, data,
			__connection_idle_destroy_cb);
	if (!id) {
		g_free(data);
		return id;
	}

	data->id = id;

	managed_idler_list = g_slist_append(managed_idler_list, data);

	return id;
}

void _connection_callback_cleanup(void)
{
	GSList *cur = managed_idler_list;
	GSource *src;
	struct managed_idle_data *data;

	while (cur) {
		GSList *next = cur->next;
		data = (struct managed_idle_data *)cur->data;

		src = g_main_context_find_source_by_id(g_main_context_default(), data->id);
		if (src) {
			g_source_destroy(src);
			cur = managed_idler_list;
		} else
			cur = next;
	}

	g_slist_free(managed_idler_list);
	managed_idler_list = NULL;
}

int _connection_libnet_check_get_privilege()
{
	int rv;

	rv = net_check_get_privilege();
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_check_profile_privilege()
{
	int rv;

	rv = net_check_profile_privilege();
	if (rv == NET_ERR_ACCESS_DENIED) {
		CONNECTION_LOG(CONNECTION_ERROR, "Access denied");
		return CONNECTION_ERROR_PERMISSION_DENIED;
	} else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

int _connection_check_feature_supported(const char *feature_name, ...)
{
	va_list list;
	const char *key;
	bool value, feature_supported = false;

	va_start(list, feature_name);
	key = feature_name;
	while(1) {
		if(system_info_get_platform_bool(key, &value) < 0) {
			CONNECTION_LOG(CONNECTION_ERROR, "Error - Feature getting from System Info");
			set_last_result(CONNECTION_ERROR_OPERATION_FAILED);
			return CONNECTION_ERROR_OPERATION_FAILED;
		}
		SECURE_CONNECTION_LOG(CONNECTION_INFO, "%s feature is %s", key, (value?"true":"false"));
		feature_supported |= value;
		key = va_arg(list, const char *);
		if (!key) break;
	}
	if (!feature_supported) {
		CONNECTION_LOG(CONNECTION_ERROR, "Error - Feature is not supported");
		set_last_result(CONNECTION_ERROR_NOT_SUPPORTED);
		return CONNECTION_ERROR_NOT_SUPPORTED;
	}
	va_end(list);

	set_last_result(CONNECTION_ERROR_NONE);
	return CONNECTION_ERROR_NONE;
}
