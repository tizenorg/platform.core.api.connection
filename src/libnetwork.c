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
#include <glib.h>
#include <vconf/vconf.h>
#include "net_connection_private.h"

static GSList *prof_handle_list = NULL;
static GHashTable *profile_cb_table = NULL;
static bool registered = false;

struct _profile_cb_s {
	connection_profile_state_changed_cb callback;
	net_profile_info_t profile;
	void *user_data;
};

struct _profile_list_s {
	int count;
	int next;
	net_profile_info_t *profiles;
};

static struct _profile_list_s profile_iterator = {0, 0, NULL};


static void __libnet_state_changed_cb(char *profile_name, net_profile_info_t *profile_info,
				connection_profile_state_e state, bool is_requested)
{
	if (profile_name == NULL)
		return;

	struct _profile_cb_s *cb_info;
	cb_info = g_hash_table_lookup(profile_cb_table, profile_name);

	if (cb_info == NULL)
		return;

	if (profile_info)
		memcpy(&cb_info->profile, profile_info, sizeof(net_profile_info_t));
	else if (state >= 0)
		cb_info->profile.ProfileState = _connection_profile_convert_to_net_state(state);

	if (cb_info->callback)
		cb_info->callback((connection_profile_h)&cb_info->profile,
							is_requested, cb_info->user_data);
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

	switch (event_cb->Event) {
	case NET_EVENT_OPEN_RSP:
		is_requested = true;
	case NET_EVENT_OPEN_IND:
		CONNECTION_LOG(CONNECTION_INFO,
			"Received ACTIVATION(Open RSP/IND) response: %d \n", event_cb->Error);

		switch (event_cb->Error) {
		case NET_ERR_NONE:
		case NET_ERR_ACTIVE_CONNECTION_EXISTS:
			CONNECTION_LOG(CONNECTION_INFO, "Activation succeeded\n");

			net_profile_info_t *prof_info = NULL;

			if (event_cb->Datalength == sizeof(net_profile_info_t))
				prof_info = (net_profile_info_t*)event_cb->Data;

			__libnet_state_changed_cb(event_cb->ProfileName, prof_info,
					CONNECTION_PROFILE_STATE_CONNECTED, is_requested);
			return;
		case NET_ERR_TIME_OUT:
			CONNECTION_LOG(CONNECTION_ERROR, "Request time out!\n");
			break;
		case NET_ERR_OPERATION_ABORTED:
			CONNECTION_LOG(CONNECTION_ERROR, "Connction is aborted!\n");
			break;
		case NET_ERR_UNKNOWN_METHOD:
			CONNECTION_LOG(CONNECTION_ERROR, "Method not found!\n");
			break;
		case NET_ERR_UNKNOWN:
			CONNECTION_LOG(CONNECTION_ERROR, "Activation Failed!\n");
			break;
		default:
			CONNECTION_LOG(CONNECTION_ERROR, "Unknown Error!\n");
			break;
		}

		__libnet_state_changed_cb(event_cb->ProfileName, NULL,
				CONNECTION_PROFILE_STATE_DISCONNECTED, is_requested);

		break;
	case NET_EVENT_CLOSE_RSP:
		is_requested = true;
	case NET_EVENT_CLOSE_IND:
		CONNECTION_LOG(CONNECTION_INFO, "Got Close RSP/IND\n");

		switch (event_cb->Error) {
		case NET_ERR_NONE:
			/* Successful PDP Deactivation */
			CONNECTION_LOG(CONNECTION_INFO, "Deactivation succeeded!\n");

			net_profile_info_t prof_info;

			if (net_get_profile_info(event_cb->ProfileName, &prof_info) == NET_ERR_NONE)
				__libnet_state_changed_cb(event_cb->ProfileName, &prof_info,
					CONNECTION_PROFILE_STATE_DISCONNECTED, is_requested);
			else
				__libnet_state_changed_cb(event_cb->ProfileName, NULL,
					CONNECTION_PROFILE_STATE_DISCONNECTED, is_requested);
			return;
		case NET_ERR_TIME_OUT:
			CONNECTION_LOG(CONNECTION_ERROR, "Request time out!\n");
			break;
		case NET_ERR_IN_PROGRESS:
			CONNECTION_LOG(CONNECTION_ERROR, "Disconncting is in progress!\n");
			break;
		case NET_ERR_OPERATION_ABORTED:
			CONNECTION_LOG(CONNECTION_ERROR, "Disconnction is aborted!\n");
			break;
		case NET_ERR_UNKNOWN_METHOD:
			CONNECTION_LOG(CONNECTION_ERROR, "Service not found!\n");
			break;
		case NET_ERR_UNKNOWN:
			CONNECTION_LOG(CONNECTION_ERROR, "Deactivation Failed!\n");
			break;
		default:
			CONNECTION_LOG(CONNECTION_ERROR, "Unknown Error!\n");
			break;
		}

		__libnet_state_changed_cb(event_cb->ProfileName, NULL, -1, is_requested);

		break;
	case NET_EVENT_NET_STATE_IND:
		CONNECTION_LOG(CONNECTION_INFO, "Got State changed IND\n");
		net_state_type_t *profile_state = (net_state_type_t*)event_cb->Data;

		if (event_cb->Error == NET_ERR_NONE &&
		    event_cb->Datalength == sizeof(net_state_type_t)) {
			switch (*profile_state) {
			case NET_STATE_TYPE_ASSOCIATION:
				CONNECTION_LOG(CONNECTION_INFO,
					"Profile State : Association, profile name : %s\n",
					event_cb->ProfileName);

				__libnet_state_changed_cb(event_cb->ProfileName, NULL,
					CONNECTION_PROFILE_STATE_ASSOCIATION, is_requested);
				break;
			case NET_STATE_TYPE_CONFIGURATION:
				CONNECTION_LOG(CONNECTION_INFO,
					"Profile State : Configuration, profile name : %s\n",
					event_cb->ProfileName);

				__libnet_state_changed_cb(event_cb->ProfileName, NULL,
					CONNECTION_PROFILE_STATE_CONFIGURATION, is_requested);
				break;
			case NET_STATE_TYPE_IDLE:
			case NET_STATE_TYPE_FAILURE:
			case NET_STATE_TYPE_READY:
			case NET_STATE_TYPE_ONLINE:
			case NET_STATE_TYPE_DISCONNECT:
			case NET_STATE_TYPE_UNKNOWN:
			default:
				CONNECTION_LOG(CONNECTION_INFO,
					"Profile State : %d, profile name : %s\n", *profile_state,
					event_cb->ProfileName);
			}
		}

		break;
	case NET_EVENT_WIFI_SCAN_IND:
	case NET_EVENT_WIFI_SCAN_RSP:
		CONNECTION_LOG(CONNECTION_ERROR, "Got wifi scan IND\n");
		break;
	case NET_EVENT_WIFI_POWER_IND:
	case NET_EVENT_WIFI_POWER_RSP:
		CONNECTION_LOG(CONNECTION_ERROR, "Got wifi power IND\n");
		break;
	case NET_EVENT_WIFI_WPS_RSP:
		CONNECTION_LOG(CONNECTION_ERROR, "Got wifi WPS RSP\n");
	default :
		CONNECTION_LOG(CONNECTION_ERROR, "Error! Unknown Event\n\n");
		break;
	}
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

bool _connection_libnet_init(void)
{
	int rv;

	if (!registered) {
		rv = net_register_client_ext((net_event_cb_t)__libnet_evt_cb, NET_DEVICE_DEFAULT, NULL);
		if (rv != NET_ERR_NONE)
			return false;

		registered = true;

		if (profile_cb_table == NULL)
			profile_cb_table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	}

	return true;
}

bool _connection_libnet_deinit(void)
{
	if (registered) {
		if (net_deregister_client_ext(NET_DEVICE_DEFAULT) != NET_ERR_NONE)
			return false;

		registered = false;

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

	for (list = prof_handle_list; list; list = list->next)
		if (profile == list->data) return true;

	for (;i < profile_iterator.count;i++)
		if (profile == &profile_iterator.profiles[i]) return true;

	struct _profile_cb_s *cb_info;
	net_profile_info_t *profile_info = profile;

	cb_info = g_hash_table_lookup(profile_cb_table, profile_info->ProfileName);
	if (cb_info == NULL)
		return false;

	if (&cb_info->profile == profile)
		return true;

	return false;
}

bool _connection_libnet_get_ethernet_state(connection_ethernet_state_e* state)
{
	struct _profile_list_s ethernet_profiles = {0, 0, NULL};
	net_get_profile_list(NET_DEVICE_ETHERNET, &ethernet_profiles.profiles, &ethernet_profiles.count);

	if (ethernet_profiles.count == 0) {
		*state = CONNECTION_ETHERNET_STATE_DEACTIVATED;
		return true;
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
		return false;
	}

	__libnet_clear_profile_list(&ethernet_profiles);

	return true;
}

int _connection_libnet_get_profile_iterator(connection_iterator_type_e type, connection_profile_iterator_h* profile_iter_h)
{
	int count = 0;
	int rv1, rv2, rv3;
	net_profile_info_t *profiles = NULL;

	struct _profile_list_s wifi_profiles = {0, 0, NULL};
	struct _profile_list_s cellular_profiles = {0, 0, NULL};
	struct _profile_list_s ethernet_profiles = {0, 0, NULL};

	__libnet_clear_profile_list(&profile_iterator);

	rv1 = net_get_profile_list(NET_DEVICE_WIFI, &wifi_profiles.profiles, &wifi_profiles.count);
	if (rv1 != NET_ERR_NO_SERVICE && rv1 != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	CONNECTION_LOG(CONNECTION_INFO, "Wifi profile count : %d\n", wifi_profiles.count);

	rv2 = net_get_profile_list(NET_DEVICE_CELLULAR, &cellular_profiles.profiles, &cellular_profiles.count);
	if (rv2 != NET_ERR_NO_SERVICE && rv2 != NET_ERR_NONE) {
		__libnet_clear_profile_list(&wifi_profiles);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	CONNECTION_LOG(CONNECTION_INFO, "Cellular profile count : %d\n", cellular_profiles.count);

	rv3 = net_get_profile_list(NET_DEVICE_ETHERNET, &ethernet_profiles.profiles, &ethernet_profiles.count);
	if (rv3 != NET_ERR_NO_SERVICE && rv3 != NET_ERR_NONE) {
		__libnet_clear_profile_list(&wifi_profiles);
		__libnet_clear_profile_list(&cellular_profiles);
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	CONNECTION_LOG(CONNECTION_INFO, "Ethernet profile count : %d\n", ethernet_profiles.count);

	*profile_iter_h = &profile_iterator;

	switch (type) {
	case CONNECTION_ITERATOR_TYPE_REGISTERED:
		count = wifi_profiles.count + cellular_profiles.count + ethernet_profiles.count;
		CONNECTION_LOG(CONNECTION_INFO, "Total profile count : %d\n", count);
		if (count == 0)
			return CONNECTION_ERROR_NONE;

		profiles = g_try_new0(net_profile_info_t, count);
		if (profiles == NULL) {
			__libnet_clear_profile_list(&wifi_profiles);
			__libnet_clear_profile_list(&cellular_profiles);
			__libnet_clear_profile_list(&ethernet_profiles);
			return CONNECTION_ERROR_OUT_OF_MEMORY;
		}

		profile_iterator.profiles = profiles;

		if (wifi_profiles.count > 0) {
			memcpy(profiles, wifi_profiles.profiles,
					sizeof(net_profile_info_t) * wifi_profiles.count);
			profiles += wifi_profiles.count;
		}

		if (cellular_profiles.count > 0) {
			memcpy(profiles, cellular_profiles.profiles,
					sizeof(net_profile_info_t) * cellular_profiles.count);
			profiles += cellular_profiles.count;
		}

		if (ethernet_profiles.count > 0)
			memcpy(profiles, ethernet_profiles.profiles,
					sizeof(net_profile_info_t) * ethernet_profiles.count);

		break;
	case CONNECTION_ITERATOR_TYPE_CONNECTED:
		count = __libnet_get_connected_count(&wifi_profiles);
		count += __libnet_get_connected_count(&cellular_profiles);
		count += __libnet_get_connected_count(&ethernet_profiles);
		CONNECTION_LOG(CONNECTION_INFO, "Total connected profile count : %d\n", count);
		if (count == 0)
			return CONNECTION_ERROR_NONE;

		profiles = g_try_new0(net_profile_info_t, count);
		if (profiles == NULL) {
			__libnet_clear_profile_list(&wifi_profiles);
			__libnet_clear_profile_list(&cellular_profiles);
			__libnet_clear_profile_list(&ethernet_profiles);
			return CONNECTION_ERROR_OUT_OF_MEMORY;
		}

		profile_iterator.profiles = profiles;

		if (wifi_profiles.count > 0)
			__libnet_copy_connected_profile(&profiles, &wifi_profiles);

		if (cellular_profiles.count > 0)
			__libnet_copy_connected_profile(&profiles, &cellular_profiles);

		if (ethernet_profiles.count > 0)
			__libnet_copy_connected_profile(&profiles, &ethernet_profiles);

		break;
	}

	__libnet_clear_profile_list(&wifi_profiles);
	__libnet_clear_profile_list(&cellular_profiles);
	__libnet_clear_profile_list(&ethernet_profiles);

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
	else if (rv != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	*profile = g_try_malloc0(sizeof(net_profile_info_t));
	if (*profile == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	memcpy(*profile, &active_profile, sizeof(net_profile_info_t));
	prof_handle_list = g_slist_append(prof_handle_list, *profile);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_open_profile(connection_profile_h profile)
{
	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	net_profile_info_t *profile_info = profile;

	if (net_open_connection_with_profile(profile_info->ProfileName) != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_open_cellular_service_type(connection_cellular_service_type_e type, connection_profile_h *profile)
{
	if (profile == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	net_profile_name_t profile_name;
	net_profile_info_t profile_info;
	net_service_type_t service_type = _connection_profile_convert_to_libnet_cellular_service_type(type);

	if (net_open_connection_with_preference_ext(service_type, &profile_name) != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (net_get_profile_info(profile_name.ProfileName, &profile_info) != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	*profile = g_try_malloc0(sizeof(net_profile_info_t));
	if (*profile == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	memcpy(*profile, &profile_info, sizeof(net_profile_info_t));
	prof_handle_list = g_slist_append(prof_handle_list, *profile);

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_close_profile(connection_profile_h profile)
{
	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	net_profile_info_t *profile_info = profile;

	if (net_close_connection(profile_info->ProfileName) != NET_ERR_NONE)
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
	memcpy(&profile_cb_info->profile, profile_info, sizeof(net_profile_info_t));

	g_hash_table_insert(profile_cb_table, profile_name, profile_cb_info);

	return true;
}

void _connection_libnet_remove_from_profile_cb_list(connection_profile_h profile)
{
	net_profile_info_t *profile_info = profile;
	g_hash_table_remove(profile_cb_table, profile_info->ProfileName);
}

int _connection_libnet_set_statistics(net_device_t device_type, net_statistics_type_e statistics_type)
{
	if (net_set_statistics(device_type, statistics_type) != NET_ERR_NONE)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

int _connection_libnet_get_statistics(net_statistics_type_e statistics_type, unsigned long long *size)
{
	if (net_get_statistics(NET_DEVICE_WIFI, statistics_type, size) != NET_ERR_NONE)
			return CONNECTION_ERROR_OPERATION_FAILED;

		return CONNECTION_ERROR_NONE;
}

