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


#ifndef __NET_CONNECTION_INTF_H__        /* To prevent inclusion of a header file twice */
#define __NET_CONNECTION_INTF_H__

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @addtogroup CAPI_NETWORK_CONNECTION_MODULE
 * @{
 */


/**
 * @brief Enumerations of connection errors.
 */
typedef enum
{
    CONNECTION_ERROR_NONE = TIZEN_ERROR_NONE, /**< Successful */
    CONNECTION_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
    CONNECTION_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY, /**< Out of memory error */
    CONNECTION_ERROR_INVALID_OPERATION = TIZEN_ERROR_INVALID_OPERATION, /**< Invalid Operation */
    CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED = TIZEN_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED, /**< Address family not supported */
}connection_error_e;

/**
 * @brief Enumerations of network status.
 */
typedef enum
{
    CONNECTION_NETWORK_STATE_DISCONNECTED = 0,  /**< Disconnected */
    CONNECTION_NETWORK_STATE_WIFI = 1,  /**< Wi-Fi is used for default connection */
    CONNECTION_NETWORK_STATE_CELLULAR = 2,  /**< Cellular is used for default connection */
}connection_network_state_e;

/**
 * @brief Enumerations of Address type
 */
typedef enum
{
    CONNECTION_ADDRESS_FAMILY_IPV4 = 0,  /**< IPV4 Address type */
    CONNECTION_ADDRESS_FAMILY_IPV6 = 1,  /**< IPV6 Address type */
} connection_address_family_e;

/**
 * @brief Enumerations of cellular network state.
 */
typedef enum
{
    CONNECTION_CELLULAR_STATE_OUT_OF_SERVICE = 0,  /**< Out of service */
    CONNECTION_CELLULAR_STATE_FLIGHT_MODE = 1,  /**< Flight mode */
    CONNECTION_CELLULAR_STATE_ROAMING_OFF = 2,  /**< Roaming is turned off */
    CONNECTION_CELLULAR_STATE_CALL_ONLY_AVAILABLE = 3,  /**< Call is only available. */
    CONNECTION_CELLULAR_STATE_AVAILABLE = 4,  /**< Available */
}connection_cellular_state_e;

/**
 * @brief This enumeration defines the Wi-Fi state.
 */
typedef enum
{
    CONNECTION_WIFI_STATE_DEACTIVATED = 0,  /**< Deactivated state */
    CONNECTION_WIFI_STATE_DISCONNECTED = 1,  /**< disconnected state */
    CONNECTION_WIFI_STATE_CONNECTED = 2,  /**< Connected state */
} connection_wifi_state_e;

/**
 * @brief  The connection handle for all connection functions.
*/
typedef struct connection_handle_s  * connection_h;

/**
 * @brief Called when the state of network is changed.
 * @param[in] connection_network_state_e  The state of current network connection
 * @param[in] user_data The user data passed from the callback registration function
 * @see connection_set_network_status_changed_cb()
 * @see connection_unset_network_status_changed_cb()
 */
typedef void(*connection_network_state_changed_cb)(connection_network_state_e state, void* user_data);

/**
 * @brief Called when the address is changed.
 * @param[in] ipv4_address  The ipv4 address of current network connection
 * @param[in] ipv6_address  The ipv6 address of current network connection
 * @param[in] user_data The user data passed from the callback registration function
 * @see connection_set_ip_address_changed_cb()
 * @see connection_unset_ip_address_changed_cb()
 * @see connection_set_proxy_address_changed_cb()
 * @see connection_unset_proxy_address_changed_cb()
 */
typedef void(*connection_address_changed_cb)(const char* ipv4_address, const char* ipv6_address, void* user_data);

/**
 * @brief Creates a handle for managing data connections.
 * @remarks @a handle must be released with connection_destroy().
 * @param[out] connection  The handle to the connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @see connection_destroy()
 */
int connection_create(connection_h* connection);

/**
 * @brief Destroys the connection handle.
 * @param[in] connection  The handle to the connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @see connection_create()
 */
int connection_destroy(connection_h connection);

/**
 * @brief Gets the network state.
 * @details The returned status is for the current connection.
 * @param[in] connection  The handle to the connection
 * @param[out] state  The state of network
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION  Cannot retrieve the state due to internal error
 */
int connection_get_network_state(connection_h connection, connection_network_state_e* state);

/**
 * @brief Gets the IP address of the current connection.
 * @remarks @a ip_address must be released with free() by you.
 * @param[in] connection  The handle to the connection
 * @param[in] address_family  The address family
 * @param[out] ip_address  The pointer to IP address string.
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Cannot retrieve the ip address due to internal error
 */
int connection_get_ip_address(connection_h connection, connection_address_family_e address_family, char** ip_address);

/**
 * @brief Gets the proxy address of the current connection.
 * @remarks @a proxy must be released with free() by you.
 * @param[in] connection  The handle to the connection
 * @param[in] address_family  The address family
 * @param[out] proxy  The proxy address
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Cannot retrieve the proxy due to internal error
 */
int connection_get_proxy(connection_h connection, connection_address_family_e address_family, char** proxy);

/**
 * @brief  Gets the state of celluar connection.
 * @details The returned state is for the cellular connection state.
 * @param[in] connection  The handle of connection
 * @param[out] state  The state of cellular connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Cannot retrieve the state due to internal error
 */
int connection_get_cellular_state(connection_h connection, connection_cellular_state_e* state);

/**
 * @brief  Gets the state of Wi-Fi.
 * @details The returned state is for the Wi-Fi connection state.
 * @param[in] connection  The handle of connection
 * @param[out] state  The state of Wi-Fi connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Cannot retrieve the state due to internal error
 */
int connection_get_wifi_state(connection_h connection, connection_wifi_state_e* state);

/**
 * @brief Registers the callback called when the state of current connection is changed.
 * @param[in] connection  The handle of connection
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 */
int connection_set_network_state_changed_cb(connection_h connection, connection_network_state_changed_cb callback, void* user_data);

/**
 * @brief Unregisters the callback called when the state of current connection is changed.
 * @param[in] connection  The handle of connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 */
int connection_unset_network_state_changed_cb(connection_h connection);

/**
 * @brief Registers the callback called when the ip address is changed.
 * @param[in] connection  The handle of connection
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 */
int connection_set_ip_address_changed_cb(connection_h connection, connection_address_changed_cb callback, void* user_data);

/**
 * @brief Unregisters the callback called when the ip address is changed.
 * @param[in] connection  The handle of connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 */
int connection_unset_ip_address_changed_cb(connection_h connection);

/**
 * @brief Registers the callback called when the proxy address is changed.
 * @param[in] connection  The handle of connection
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 */
int connection_set_proxy_address_changed_cb(connection_h connection, connection_address_changed_cb callback, void* user_data);

/**
 * @brief Unregisters the callback called when the proxy address is changed.
 * @param[in] connection  The handle of connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 */
int connection_unset_proxy_address_changed_cb(connection_h connection);

/**
 * @brief Gets the received data size of the last cellular packet data connection.
 * @param[in] connection  The handle to the connection
 * @param[out] size  The received data size of the last cellular packet data connection ( bytes )
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION  Cannot retrieve the size due to internal error
 */
int connection_get_last_received_data_size(connection_h connection, int* size);

/**
 * @brief Gets the sent data size of the last cellular packet data connection.
 * @param[in] connection  The handle to the connection
 * @param[out] size  The sent data size of the last cellular packet data connection.
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION  Cannot retrieve the size due to internal error
 */
int connection_get_last_sent_data_size(connection_h connection, int* size);

/**
 * @brief Gets the total received data size of all cellular packet data connections.
 * @param[in] connection  The handle to the connection
 * @param[out] size  The total received data size of all cellular packet data connections (bytes)
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Cannot retrieve the size due to internal error
 */
int connection_get_total_received_data_size (connection_h connection, int* size);

/**
 * @brief Gets the total sent data size of all cellular packet data connections.
 * @param[in] connection  The handle to the connection
 * @param[out] size  The total sent data size of all cellular packet data connections (bytes)
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Cannot retrieve the size due to internal error
 */
int connection_get_total_sent_data_size (connection_h connection, int* size);

/**
 * @brief Gets the received data size of the last Wi-Fi packet data connection.
 * @param[in] connection  The handle to the connection
 * @param[out] size  The received data size of the last Wi-Fi packet data connection ( bytes )
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Cannot retrieve the size due to internal error
 */
int connection_get_wifi_last_received_data_size(connection_h connection, int* size);

/**
 * @brief Gets the sent data size of the last Wi-Fi packet data connection.
 * @param[in] connection  The handle to the connection
 * @param[out]  size  The sent data size of the last Wi-Fi packet data connection.
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Cannot retrieve the size due to internal error
 */
int connection_get_wifi_last_sent_data_size(connection_h connection, int* size);

/**
 * @brief Gets the total received data size of all Wi-Fi packet data connections.
 * @param[in] connection  The handle to the connection
 * @param[out] size  The total received data size of all Wi-Fi packet data connections (bytes)
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Cannot retrieve the size due to internal error
 */
int connection_get_wifi_total_received_data_size (connection_h connection, int* size);

/**
 * @brief Gets the total sent data size of all Wi-Fi packet data connections.
 * @param[in] connection  The handle to the connection
 * @param[out] size  The total sent data size of all Wi-Fi packet data connections (bytes)
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Cannot retrieve the size due to internal error
 */
int connection_get_wifi_total_sent_data_size (connection_h connection, int* size);


/**
 * @}
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
