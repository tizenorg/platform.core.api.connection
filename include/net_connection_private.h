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
	connection_cb				callback;
	void					*user_data;
}connection_handle_s;


/**
 * @internal
 * @brief same as number of callbacks in connection_event_callbacks_s
 */
typedef enum
{
	LAST_DATACALL_DURATION,
	LAST_SENT_DATA_SIZE,
	LAST_RECEIVED_DATA_SIZE,
	TOTAL_DATACALL_DURATION,
	TOTAL_SENT_DATA_SIZE,
	TOTAL_RECEIVED_DATA_SIZE,
    LAST_WIFI_DATACALL_DURATION,
	LAST_WIFI_SENT_DATA_SIZE,
	LAST_WIFI_RECEIVED_DATA_SIZE,
	TOTAL_WIFI_DATACALL_DURATION,
	TOTAL_WIFI_SENT_DATA_SIZE,
	TOTAL_WIFI_RECEIVED_DATA_SIZE,

}stat_request_e;


/**
 * @brief Registers the callback to be invoked for Connection Open Response.
 * @details
 *
 *       Applications MUST call this API if they want to get the async response of connection_open() API.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @param[in] callback    Pointer to function of type (connection_cb).
 * @param[in] user_data   User data param provided by application. 
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed successfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create
 *
 * @par Example
 * @ref CONNECTION_MANAGER_EXAMPLE_1
 ******************************************************************************************/
int connection_set_open_response_cb(connection_h handle, connection_cb callback, void *user_data);

/**
 * @brief Deregisters callback for Connection Open Response.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed successfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create, #connection_set_open_response_cb
 *
******************************************************************************************/
int connection_unset_open_response_cb(connection_h handle);

/**
 * @brief Registers the callback to be invoked for Connection Close Response.
 *
 * @details
 *       Applications MUST call this API if they want to get the async response of connection_close() API.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @param[in] callback    Pointer to function of type (connection_cb).
 * @param[in] user_data   User data param provided by application.
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed successfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create
 * @par Example
 * @ref CONNECTION_MANAGER_EXAMPLE_1
 *
******************************************************************************************/
int connection_set_close_response_cb(connection_h handle, connection_cb callback, void *user_data);

/**
 * @brief Deregisters the callback for Connection Close Response.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed successfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create, #connection_set_close_response_cb
 *
******************************************************************************************/
int connection_unset_close_response_cb(connection_h handle);

/**
 * @brief Registers the callback to be invoked when the data connection get suspended by the network.
 *
 * @details
 *       Applications MUST call this API if they want to get notified of network suspend indications.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @param[in] callback    Pointer to function of type (connection_cb).
 * @param[in] user_data   User data param provided by application. 
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed successfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create
 *
******************************************************************************************/
int connection_set_suspend_indication_cb(connection_h handle, connection_cb callback, void *user_data);

/**
 * @brief Deregisters the callback for suspend indications.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed unccessfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create, #connection_set_suspend_indication_cb
 *
******************************************************************************************/
int connection_unset_suspend_indication_cb(connection_h handle);

/**
* @brief Registers the callback to be invoked when a suspended data connection gets resumed by the network.
 *
 * @details
 *       Applications MUST call this API if they want to get notified of network resume indications.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @param[in] callback    Pointer to function of type (connection_cb).
 * @param[in] user_data   User data param provided by application.
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed unccessfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create
 *
******************************************************************************************/
int connection_set_resume_indication_cb(connection_h handle, connection_cb callback, void *user_data);

/**
 * @brief Deregisters the callback for resume indications.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed successfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create, #connection_set_resume_indication_cb
 *
******************************************************************************************/
int connection_unset_resume_indication_cb(connection_h handle);

/**
 * @brief Registers the callback to be invoked network status changes.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @param[in] callback    Pointer to function of type (connection_cb).
 * @param[in] user_data   User data param provided by application. 
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed successfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create
 *
******************************************************************************************/
int connection_set_network_status_indication_cb(connection_h handle, connection_cb callback, void *user_data);

/**
 * @brief Deregisters the callback for network status change indications.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed successfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create, #connection_set_network_status_indication_cb
 *
******************************************************************************************/
int connection_unset_network_status_indication_cb(connection_h handle);

/**
 * @brief Registers the callback to be invoked IP Address changes.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @param[in] callback    Pointer to function of type (connection_cb).
 * @param[in] user_data   User data param provided by application. 
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed successfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create
 *
******************************************************************************************/
int connection_set_ip_changed_cb(connection_h handle, connection_cb callback, void *user_data);

/**
 * @brief Deregisters the callback for IP changed indications.
 *
 * @param[in] handle      Handle received as a result of connection_create() API.
 * @return 0 on success, otherwise negative error value.
 * @retval CONNECTION_ERROR_NONE - the operation has completed successfully.
 * @retval TIZEN_ERROR_INVALID_PARAMETER - handle is invalid or callback is null
 * @pre #connection_create, #connection_set_ip_changed_cb
 *
******************************************************************************************/
int connection_unset_ip_changed_cb(connection_h handle);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
