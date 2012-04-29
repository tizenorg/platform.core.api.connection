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

#include <net_connection.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _connection_handle_s
{
	connection_network_state_changed_cb state_changed_callback;
	connection_address_changed_cb ip_changed_callback;
	connection_address_changed_cb proxy_changed_callback;
	void *state_changed_user_data;
	void *ip_changed_user_data;
	void *proxy_changed_user_data;
} connection_handle_s;

/**
 * @internal
 * @brief same as number of callbacks in connection_event_callbacks_s
 */
typedef enum
{
	LAST_SENT_DATA_SIZE,
	LAST_RECEIVED_DATA_SIZE,
	TOTAL_SENT_DATA_SIZE,
	TOTAL_RECEIVED_DATA_SIZE,
	LAST_WIFI_SENT_DATA_SIZE,
	LAST_WIFI_RECEIVED_DATA_SIZE,
	TOTAL_WIFI_SENT_DATA_SIZE,
	TOTAL_WIFI_RECEIVED_DATA_SIZE,
} stat_request_e;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
