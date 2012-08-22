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

#include "connection_profile.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef DEPRECATED
#define DEPRECATED __attribute__((deprecated))
#endif

/**
 * @addtogroup CAPI_NETWORK_CONNECTION_MANAGER_MODULE
 * @{
 */

/**
 * @brief  The connection handle for all connection functions.
*/
typedef void* connection_h;

/**
 * @brief  The iterator handle for profiles.
*/
typedef void* connection_profile_iterator_h;

/**
 * @brief Enumerations of connection type.
 */
typedef enum
{
    CONNECTION_TYPE_DISCONNECTED = 0,  /**< Disconnected */
    CONNECTION_TYPE_WIFI = 1,  /**< Wi-Fi is used for default connection */
    CONNECTION_TYPE_CELLULAR = 2,  /**< Cellular is used for default connection */
    CONNECTION_TYPE_ETHERNET = 3,  /**< Ethernet is used for default connection */
} connection_type_e;

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
} connection_cellular_state_e;

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
 * @brief This enumeration defines the ethernet state.
 */
typedef enum
{
    CONNECTION_ETHERNET_STATE_DEACTIVATED = 0,  /**< Deactivated state */
    CONNECTION_ETHERNET_STATE_DISCONNECTED = 1,  /**< disconnected state */
    CONNECTION_ETHERNET_STATE_CONNECTED = 2,  /**< Connected state */
} connection_ethernet_state_e;

/**
 * @brief This enumeration defines the type of connection iterator.
 */
typedef enum
{
    CONNECTION_ITERATOR_TYPE_REGISTERED = 0,  /**< The iterator of registered profile  */
    CONNECTION_ITERATOR_TYPE_CONNECTED = 1,  /**< The iterator of connected profile  */
} connection_iterator_type_e;

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
    CONNECTION_ERROR_OPERATION_FAILED = TIZEN_ERROR_NETWORK_CLASS|0x0401, /**< Operation failed */
    CONNECTION_ERROR_ITERATOR_END = TIZEN_ERROR_NETWORK_CLASS|0x0402, /**< End of iteration */
    CONNECTION_ERROR_NO_CONNECTION = TIZEN_ERROR_NETWORK_CLASS|0x0403, /**< There is no connection */
} connection_error_e;

/**
 * @}
*/

/**
 * @addtogroup CAPI_NETWORK_CONNECTION_STATISTICS_MODULE
 * @{
*/

/**
 * @brief Enumerations of statistics type.
 */
typedef enum
{
    CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA = 0,  /**< Last received data */
    CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA = 1,  /**< Last sent data */
    CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA = 2,  /**< Total received data */
    CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA = 3,  /**< Total sent data */
} connection_statistics_type_e;

/**
 * @}
*/

/**
 * @addtogroup CAPI_NETWORK_CONNECTION_MANAGER_MODULE
 * @{
*/


/**
 * @brief Creates a handle for managing data connections.
 * @remarks @a handle must be released with connection_destroy().
 * @param[out] connection  The handle of the connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @see connection_destroy()
 */
int connection_create(connection_h* connection);

/**
 * @brief Destroys the connection handle.
 * @param[in] connection  The handle of the connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @see connection_create()
 */
int connection_destroy(connection_h connection);

/**
 * @brief Called when the type of connection is changed.
 * @param[in] type  The type of current network connection
 * @param[in] user_data The user data passed from the callback registration function
 * @see connection_set_network_type_changed_cb()
 * @see connection_unset_network_type_changed_cb()
 */
typedef void(*connection_type_changed_cb)(connection_type_e type, void* user_data);

/**
 * @brief Called when the address is changed.
 * @param[in] ipv4_address  The IP address for IPv4
 * @param[in] ipv6_address  The IP address for IPv6
 * @param[in] user_data The user data passed from the callback registration function
 * @see connection_set_ip_address_changed_cb()
 * @see connection_unset_ip_address_changed_cb()
 * @see connection_set_proxy_address_changed_cb()
 * @see connection_unset_proxy_address_changed_cb()
 */
typedef void(*connection_address_changed_cb)(const char* ipv4_address, const char* ipv6_address, void* user_data);


/**
 * @brief Gets the type.
 * @details The returned type is for the current connection.
 * @param[in] connection  The handle of the connection
 * @param[out] state  The state of network
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_get_type(connection_h connection, connection_type_e* type);

/**
 * @brief Gets the IP address of the current connection.
 * @remarks @a ip_address must be released with free() by you.
 * @param[in] connection  The handle of the connection
 * @param[in] address_family  The address family
 * @param[out] ip_address  The pointer to IP address string.
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 */
int connection_get_ip_address(connection_h connection, connection_address_family_e address_family, char** ip_address);

/**
 * @brief Gets the proxy address of the current connection.
 * @remarks @a proxy must be released with free() by you.
 * @param[in] connection  The handle of the connection
 * @param[in] address_family  The address family
 * @param[out] proxy  The proxy address
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
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
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
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
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_get_wifi_state(connection_h connection, connection_wifi_state_e* state);

/**
 * @brief  Gets the state of ethernet.
 * @details The returned state is for the ethernet connection state.
 * @param[in] connection  The handle of connection
 * @param[out] state  The state of Ethernet connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_get_ethernet_state(connection_h connection, connection_ethernet_state_e* state);

/**
 * @brief Registers the callback called when the type of current connection is changed.
 * @param[in] connection  The handle of connection
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_set_type_changed_cb(connection_h connection, connection_type_changed_cb callback, void* user_data);

/**
 * @brief Unregisters the callback called when the type of current connection is changed.
 * @param[in] connection  The handle of connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_unset_type_changed_cb(connection_h connection);

/**
 * @brief Registers the callback called when the IP address is changed.
 * @param[in] connection  The handle of connection
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_set_ip_address_changed_cb(connection_h connection, connection_address_changed_cb callback, void* user_data);

/**
 * @brief Unregisters the callback called when the IP address is changed.
 * @param[in] connection  The handle of connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
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
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_set_proxy_address_changed_cb(connection_h connection, connection_address_changed_cb callback, void* user_data);

/**
 * @brief Unregisters the callback called when the proxy address is changed.
 * @param[in] connection  The handle of connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_unset_proxy_address_changed_cb(connection_h connection);

/**
 * @brief Adds new profile which is created by connection_profile_created().
 * @param[in] connection  The handle of connection
 * @param[in] profile  The handle of profile
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_add_profile(connection_h connection, connection_profile_h profile);

/**
 * @brief Removes existing profile.
 * @param[in] connection  The handle of connection
 * @param[in] profile  The handle of profile
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_remove_profile(connection_h connection, connection_profile_h profile);

/**
 * @brief Updates existing profile.
 * @param[in] connection  The handle of connection
 * @param[in] profile  The handle of profile
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_update_profile(connection_h connection, connection_profile_h profile);

/**
 * @brief Gets a iterator of the profiles.
 * @remarks @a profile_iterator must be released with connection_destroy().
 * @param[in] connection  The handle of connection
 * @param[in] type  The type of connetion iterator
 * @param[out] profile_iterator  The iterator of profile
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_get_profile_iterator(connection_h connection, connection_iterator_type_e type, connection_profile_iterator_h* profile_iterator);

/**
 * @brief Moves the profile iterator to the next position and gets a profile handle.
 * @param[in] profile_iterator  The iterator of profile
 * @param[out] profile  The handle of profile
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_ITERATOR_END  End of iteration
 */
int connection_profile_iterator_next(connection_profile_iterator_h profile_iterator, connection_profile_h* profile);

/**
 * @brief Checks whether the next element of profile iterator exists or not.
 * @param[in] profile_iterator  The iterator of profile
 * @return @c true if next element exists, \n @c false if next element doesn't exist
 */
bool connection_profile_iterator_has_next(connection_profile_iterator_h profile_iterator);

/**
 * @brief Destroys a iterator of the profiles.
 * @param[in] profile_iterator  The iterator of profile
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 */
int connection_destroy_profile_iterator(connection_profile_iterator_h profile_iterator);

/**
 * @brief Gets the name of default profile.
 * @remarks @a profile must be released with connection_profile_destroy().
 * @param[in] connection  The handle of connection
 * @param[out] profile  The handle of profile
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NO_CONNECTION  There is no connection
 */
int connection_get_current_profile(connection_h connection, connection_profile_h* profile);

/**
 * @brief Opens the connection with the profile, asynchronously.
 * @param[in] connection  The handle of connection
 * @param[in] profile  The handle of profile
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_open_profile(connection_h connection, connection_profile_h profile);

/**
 * @brief Opens the connection with service type, asynchronously.
 * @remarks @a profile must be released with connection_profile_destroy().
 * @param[in] connection  The handle of connection
 * @param[in] type  The type of cellular service
 * @param[out] profile  The handle of profile
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_open_cellular_service_type(connection_h connection, connection_cellular_service_type_e type, connection_profile_h* profile);

/**
 * @brief Closes the connection with the profile.
 * @param[in] connection  The handle of connection
 * @param[in] profile  The handle of profile
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_close_profile(connection_h connection, connection_profile_h profile);

/**
 * @}
*/

/**
 * @addtogroup CAPI_NETWORK_CONNECTION_STATISTICS_MODULE
 * @{
*/

/**
 * @brief Gets the statistics information.
 * @param[in] connection_type  The type of connection. CONNECTION_TYPE_WIFI and CONNECTION_TYPE_CELLULAR are only supported.
 * @param[in] statistics_type  The type of statistics
 * @param[out] size  The received data size of the last cellular packet data connection (bytes)
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_get_statistics(connection_type_e connection_type, connection_statistics_type_e statistics_type, long long* size);

/**
 * @brief Resets the statistics information
 * @param[in] connection_type  The type of connection. CONNECTION_TYPE_WIFI and CONNECTION_TYPE_CELLULAR are only supported.
 * @param[in] statistics_type  The type of statistics
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_reset_statistics(connection_type_e connection_type, connection_statistics_type_e statistics_type);

/**
 * @}
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
