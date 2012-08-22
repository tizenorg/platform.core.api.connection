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



#ifndef __NET_CONNECTION_PRIVATE_H__        /* To prevent inclusion of a header file twice */
#define __NET_CONNECTION_PRIVATE_H__

#include <dlog.h>
#include <network-cm-intf.h>
#include <network-wifi-intf.h>
#include "net_connection.h"

#define TIZEN_NET_CONNECTION "net_connection"

#define CONNECTION_INFO		LOG_VERBOSE
#define CONNECTION_ERROR	LOG_ERROR
#define CONNECTION_WARN		LOG_WARN

#define CONNECTION_LOG(log_level, format, args...) \
	SLOG(log_level,TIZEN_NET_CONNECTION, "[%s][Ln: %d] " format, __FILE__, __LINE__, ##args)

#define CONNECTION_MUTEX_LOCK _connection_inter_mutex_lock()

#define CONNECTION_MUTEX_UNLOCK _connection_inter_mutex_unlock()

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _connection_handle_s
{
	connection_type_changed_cb state_changed_callback;
	connection_address_changed_cb ip_changed_callback;
	connection_address_changed_cb proxy_changed_callback;
	void *state_changed_user_data;
	void *ip_changed_user_data;
	void *proxy_changed_user_data;
} connection_handle_s;


bool _connection_libnet_init(void);
bool _connection_libnet_deinit(void);
bool _connection_libnet_get_ethernet_state(connection_ethernet_state_e* state);
bool _connection_libnet_check_profile_validity(connection_profile_h profile);
int _connection_libnet_get_profile_iterator(connection_iterator_type_e type,
				connection_profile_iterator_h* profile_iterator);
bool _connection_libnet_iterator_has_next(connection_profile_iterator_h profile_iterator);
int _connection_libnet_get_iterator_next(connection_profile_iterator_h profile_iter_h, connection_profile_h *profile);
int _connection_libnet_destroy_iterator(connection_profile_iterator_h profile_iter_h);
int _connection_libnet_get_current_profile(connection_profile_h *profile);
int _connection_libnet_open_profile(connection_profile_h profile);
int _connection_libnet_open_cellular_service_type(connection_cellular_service_type_e type, connection_profile_h *profile);
int _connection_libnet_close_profile(connection_profile_h profile);
void _connection_libnet_add_to_profile_list(connection_profile_h profile);
void _connection_libnet_remove_from_profile_list(connection_profile_h profile);
bool _connection_libnet_add_to_profile_cb_list(connection_profile_h profile,
		connection_profile_state_changed_cb callback, void *user_data);
void _connection_libnet_remove_from_profile_cb_list(connection_profile_h profile);
int _connection_libnet_set_statistics(net_device_t device_type, net_statistics_type_e statistics_type);
int _connection_libnet_get_statistics(net_statistics_type_e statistics_type, unsigned long long *size);

net_service_type_t _connection_profile_convert_to_libnet_cellular_service_type(connection_cellular_service_type_e svc_type);
net_state_type_t _connection_profile_convert_to_net_state(connection_profile_state_e state);

void _connection_inter_mutex_lock(void);
void _connection_inter_mutex_unlock(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
