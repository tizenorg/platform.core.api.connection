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

#ifndef __NET_CONNECTION_PRIVATE_H__
#define __NET_CONNECTION_PRIVATE_H__

#include <dlog.h>
#include <network-cm-intf.h>
#include <network-wifi-intf.h>

#include "net_connection.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#undef LOG_TAG
#define LOG_TAG "CAPI_NETWORK_CONNECTION"

#define CONNECTION_INFO		1
#define CONNECTION_ERROR	2
#define CONNECTION_WARN		3

typedef enum
{
	FEATURE_TYPE_TELEPHONY = 0,
	FEATURE_TYPE_WIFI = 1,
	FEATURE_TYPE_TETHERING_BLUETOOTH = 2
} enable_feature_type_e;

typedef enum
{
	CONNECTION_CELLULAR_SUBSCRIBER_1 = 0x00,
	CONNECTION_CELLULAR_SUBSCRIBER_2 = 0x01,
} connection_cellular_subscriber_id_e;

#define CONNECTION_LOG(log_level, format, args...) \
	do { \
		switch (log_level) { \
		case CONNECTION_ERROR: \
			LOGE(format, ## args); \
			break; \
		case CONNECTION_WARN: \
			LOGW(format, ## args); \
			break; \
		default: \
			LOGI(format, ## args); \
		} \
	} while(0)

#define CONNECTION_MUTEX_LOCK _connection_inter_mutex_lock()
#define CONNECTION_MUTEX_UNLOCK _connection_inter_mutex_unlock()

#define SECURE_CONNECTION_LOG(log_level, format, args...) \
	do { \
		switch (log_level) { \
		case CONNECTION_ERROR: \
			SECURE_LOGE(format, ## args); \
			break; \
		case CONNECTION_WARN: \
			SECURE_LOGW(format, ## args); \
			break; \
		default: \
			SECURE_LOGI(format, ## args); \
		} \
	} while(0)

#define VCONF_TELEPHONY_DEFAULT_DATA_SERVICE \
			"db/telephony/dualsim/default_data_service"

bool _connection_is_created(void);

typedef struct _connection_handle_s
{
	connection_type_changed_cb type_changed_callback;
	connection_address_changed_cb ip_changed_callback;
	connection_address_changed_cb proxy_changed_callback;
	void *state_changed_user_data;
	void *ip_changed_user_data;
	void *proxy_changed_user_data;
} connection_handle_s;


bool _connection_libnet_init(void);
bool _connection_libnet_deinit(void);
int _connection_libnet_get_wifi_state(connection_wifi_state_e *state);
int _connection_libnet_get_ethernet_state(connection_ethernet_state_e *state);
int _connection_libnet_get_bluetooth_state(connection_bt_state_e* state);
bool _connection_libnet_check_profile_validity(connection_profile_h profile);
bool _connection_libnet_check_profile_cb_validity(connection_profile_h profile);
int _connection_libnet_get_profile_iterator(connection_iterator_type_e type,
				connection_profile_iterator_h *profile_iterator);
bool _connection_libnet_iterator_has_next(connection_profile_iterator_h profile_iterator);
int _connection_libnet_get_iterator_next(connection_profile_iterator_h profile_iter_h, connection_profile_h *profile);
int _connection_libnet_destroy_iterator(connection_profile_iterator_h profile_iter_h);
int _connection_libnet_get_current_profile(connection_profile_h *profile);
int _connection_libnet_reset_profile(connection_reset_option_e type, connection_cellular_subscriber_id_e id, connection_reset_cb callback, void *user_data);
int _connection_libnet_open_profile(connection_profile_h profile, connection_opened_cb callback, void *user_data);
int _connection_libnet_get_cellular_service_profile(connection_cellular_service_type_e type, connection_profile_h *profile);
int _connection_libnet_set_cellular_service_profile_sync(connection_cellular_service_type_e type, connection_profile_h profile);
int _connection_libnet_set_cellular_service_profile_async(connection_cellular_service_type_e type,
			connection_profile_h profile, connection_set_default_cb callback, void* user_data);
int _connection_libnet_close_profile(connection_profile_h profile, connection_closed_cb callback, void *user_data);
int _connection_libnet_add_route(const char *interface_name, const char *host_address);
int _connection_libnet_remove_route(const char *interface_name, const char *host_address);
void _connection_libnet_add_to_profile_list(connection_profile_h profile);
void _connection_libnet_remove_from_profile_list(connection_profile_h profile);
bool _connection_libnet_add_to_profile_cb_list(connection_profile_h profile,
		connection_profile_state_changed_cb callback, void *user_data);
bool _connection_libnet_remove_from_profile_cb_list(connection_profile_h profile);
int _connection_libnet_set_statistics(net_device_t device_type, net_statistics_type_e statistics_type);
int _connection_libnet_get_statistics(net_statistics_type_e statistics_type, unsigned long long *size);
int _connection_libnet_check_get_privilege();
int _connection_libnet_check_profile_privilege();

bool _connection_libnet_get_is_check_enable_feature();
bool _connection_libnet_get_enable_feature_state(enable_feature_type_e feature_type);
int _connection_libnet_check_enable_feature();

guint _connection_callback_add(GSourceFunc func, gpointer user_data);
void _connection_callback_cleanup(void);

connection_cellular_service_type_e _profile_convert_to_connection_cellular_service_type(net_service_type_t svc_type);
connection_profile_state_e _profile_convert_to_cp_state(net_state_type_t state);
net_service_type_t _connection_profile_convert_to_libnet_cellular_service_type(connection_cellular_service_type_e svc_type);
net_state_type_t _connection_profile_convert_to_net_state(connection_profile_state_e state);

int _connection_libnet_set_cellular_subscriber_id(connection_profile_h profile, connection_cellular_subscriber_id_e sim_id);
void _connection_inter_mutex_lock(void);
void _connection_inter_mutex_unlock(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
