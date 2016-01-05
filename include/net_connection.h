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

#ifndef __NET_CONNECTION_INTF_H__
#define __NET_CONNECTION_INTF_H__

#include "connection_profile.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file net_connection.h
 */

/**
 * @addtogroup CAPI_NETWORK_CONNECTION_MANAGER_MODULE
 * @{
 */

/**
 * @brief The connection handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef void* connection_h;

/**
 * @brief The profiles iterator handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef void* connection_profile_iterator_h;

/**
 * @brief Enumeration for connection type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CONNECTION_TYPE_DISCONNECTED = 0,  /**< Disconnected */
    CONNECTION_TYPE_WIFI = 1,  /**< Wi-Fi type */
    CONNECTION_TYPE_CELLULAR = 2,  /**< Cellular type */
    CONNECTION_TYPE_ETHERNET = 3,  /**< Ethernet type */
    CONNECTION_TYPE_BT = 4,  /**< Bluetooth type */
    CONNECTION_TYPE_NET_PROXY, /**< Proxy type for internet connection (Since 3.0) */
} connection_type_e;

/**
 * @brief Enumeration for cellular network state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CONNECTION_CELLULAR_STATE_OUT_OF_SERVICE = 0,  /**< Out of service */
    CONNECTION_CELLULAR_STATE_FLIGHT_MODE = 1,  /**< Flight mode */
    CONNECTION_CELLULAR_STATE_ROAMING_OFF = 2,  /**< Roaming is turned off */
    CONNECTION_CELLULAR_STATE_CALL_ONLY_AVAILABLE = 3,  /**< Call is only available */
    CONNECTION_CELLULAR_STATE_AVAILABLE = 4,  /**< Available but not connected yet */
    CONNECTION_CELLULAR_STATE_CONNECTED = 5,  /**< Connected */
} connection_cellular_state_e;

/**
 * @brief Enumeration for Wi-Fi state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CONNECTION_WIFI_STATE_DEACTIVATED = 0,  /**< Wi-Fi is deactivated */
    CONNECTION_WIFI_STATE_DISCONNECTED = 1,  /**< Disconnected */
    CONNECTION_WIFI_STATE_CONNECTED = 2,  /**< Connected */
} connection_wifi_state_e;

/**
 * @brief Enumeration for ethernet state.
 * @since_tizen 2.4
 */
typedef enum
{
    CONNECTION_ETHERNET_STATE_DEACTIVATED = 0,  /**< There is no Ethernet profile to open */
    CONNECTION_ETHERNET_STATE_DISCONNECTED = 1,  /**< Disconnected */
    CONNECTION_ETHERNET_STATE_CONNECTED = 2,  /**< Connected */
} connection_ethernet_state_e;

/**
 * @brief Enumeration for Bluetooth state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CONNECTION_BT_STATE_DEACTIVATED = 0,  /**< There is no Bluetooth profile to open */
    CONNECTION_BT_STATE_DISCONNECTED = 1,  /**< Disconnected */
    CONNECTION_BT_STATE_CONNECTED = 2,  /**< Connected */
} connection_bt_state_e;

/**
 * @brief Enumeration for connection iterator type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CONNECTION_ITERATOR_TYPE_REGISTERED = 0,  /**< The iterator of the registered profile  */
    CONNECTION_ITERATOR_TYPE_CONNECTED = 1,  /**< The iterator of the connected profile  */
	CONNECTION_ITERATOR_TYPE_DEFAULT = 2,  	/**< The iterator of the default profile  */
} connection_iterator_type_e;

/**
 * @brief Enumeration for reset profile type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef enum
{
    CONNECTION_RESET_DEFAULT_PROFILE = 0,  /**< Initialized with the default profile defined by csc */
    CONNECTION_RESET_CLEAR_PROFILE = 1,  /**< Remove all profiles */
} connection_reset_option_e;

/**
 * @brief This enumeration defines the attached or detached state of ethernet cable.
 * @since_tizen 2.4
 */
typedef enum
{
    CONNECTION_ETHERNET_CABLE_DETACHED = 0,  /**< Ethernet cable is detached */
    CONNECTION_ETHERNET_CABLE_ATTACHED = 1,  /**< Ethernet cable is attached */
} connection_ethernet_cable_state_e;

/**
 * @brief Enumeration for connection errors.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CONNECTION_ERROR_NONE = TIZEN_ERROR_NONE, /**< Successful */
    CONNECTION_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
    CONNECTION_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY, /**< Out of memory error */
    CONNECTION_ERROR_INVALID_OPERATION = TIZEN_ERROR_INVALID_OPERATION, /**< Invalid Operation */
    CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED = TIZEN_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED, /**< Address family not supported */
    CONNECTION_ERROR_OPERATION_FAILED = TIZEN_ERROR_CONNECTION|0x0401, /**< Operation failed */
    CONNECTION_ERROR_ITERATOR_END = TIZEN_ERROR_CONNECTION|0x0402, /**< End of iteration */
    CONNECTION_ERROR_NO_CONNECTION = TIZEN_ERROR_CONNECTION|0x0403, /**< There is no connection */
    CONNECTION_ERROR_NOW_IN_PROGRESS = TIZEN_ERROR_NOW_IN_PROGRESS, /** Now in progress */
    CONNECTION_ERROR_ALREADY_EXISTS = TIZEN_ERROR_CONNECTION|0x0404, /**< Already exists */
    CONNECTION_ERROR_OPERATION_ABORTED = TIZEN_ERROR_CONNECTION|0x0405, /**< Operation is aborted */
    CONNECTION_ERROR_DHCP_FAILED = TIZEN_ERROR_CONNECTION|0x0406, /**< DHCP failed  */
    CONNECTION_ERROR_INVALID_KEY = TIZEN_ERROR_CONNECTION|0x0407, /**< Invalid key  */
    CONNECTION_ERROR_NO_REPLY = TIZEN_ERROR_CONNECTION|0x0408, /**< No reply */
    CONNECTION_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED, /**< Permission denied */
    CONNECTION_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED	/**< Not Supported */
} connection_error_e;

/**
 * @}
*/

/**
 * @addtogroup CAPI_NETWORK_CONNECTION_STATISTICS_MODULE
 * @{
*/

/**
 * @brief Enumeration for statistics type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
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
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.get
 * @remarks You must release @a handle using connection_destroy().
 * @param[out] connection  The connection handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @see connection_destroy()
 */
int connection_create(connection_h* connection);

/**
 * @brief Destroys the connection handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] connection  The connection handle
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @see connection_create()
 */
int connection_destroy(connection_h connection);

/**
 * @brief Called when the type of a connection is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] type  The type of the current network connection
 * @param[in] user_data The user data passed from the callback registration function
 * @see connection_set_type_changed_cb()
 * @see connection_unset_type_changed_cb()
 */
typedef void(*connection_type_changed_cb)(connection_type_e type, void* user_data);

/**
 * @brief Called when the address is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
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
 * @brief Called when connection_set_default_cellular_service_profile_async() finishes.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] result  The result
 * @param[in] user_data The user data passed from connection_open_profile()
 * @pre connection_set_default_cellular_service_profile_async() will invoke this callback function.
 * @see connection_set_default_cellular_service_profile_async()
*/
typedef void(*connection_set_default_cb)(connection_error_e result, void* user_data);

/**
 * @brief Gets the type of the current profile for data connection.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] connection  The connection handle
 * @param[out] type  The type of the network
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_get_type(connection_h connection, connection_type_e* type);

/**
 * @brief Gets the IP address of the current connection.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a ip_address using free().
 * @param[in] connection  The connection handle
 * @param[in] address_family  The address family
 * @param[out] ip_address  The pointer to the IP address string
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 */
int connection_get_ip_address(connection_h connection, connection_address_family_e address_family, char** ip_address);

/**
 * @brief Gets the proxy address of the current connection.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a proxy using free().
 * @param[in] connection  The connection handle
 * @param[in] address_family  The address family
 * @param[out] proxy  The proxy address
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 */
int connection_get_proxy(connection_h connection, connection_address_family_e address_family, char** proxy);

/**
 * @brief Gets the MAC address of the Wi-Fi or ethernet.
 * @since_tizen 2.4
 * @remarks @a mac_addr must be released with free() by you.
 * @param[in] connection  The handle of the connection
 * @param[in] type  The type of current network connection
 * @param[out] mac_addr  The MAC address
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Invalid operation
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED  Not supported
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 */
int connection_get_mac_address(connection_h connection, connection_type_e type, char** mac_addr);

/**
 * @brief Gets the state of cellular connection.
 * @details The returned state is for the cellular connection state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] connection  The connection handle
 * @param[out] state  The state of the cellular connection
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
 */
int connection_get_cellular_state(connection_h connection, connection_cellular_state_e* state);

/**
 * @brief Gets the state of the Wi-Fi.
 * @details The returned state is for the Wi-Fi connection state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.get
 * @param[in] connection  The connection handle
 * @param[out] state  The state of Wi-Fi connection
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
 */
int connection_get_wifi_state(connection_h connection, connection_wifi_state_e* state);

/**
 * @brief Gets the state of the Ethernet.
 * @details The returned state is for the Ethernet connection state.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.get
 * @param[in] connection  The connection handle
 * @param[out] state  The state of Ethernet connection
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
 */
int connection_get_ethernet_state(connection_h connection, connection_ethernet_state_e* state);

/**
* @brief Checks for ethernet cable is attached or not.
* @details The returned state is for the ethernet cable state.
* @since_tizen 2.4
* @privlevel public
* @privilege %http://tizen.org/privilege/network.get
* @param[in] connection  The handle of the connection
* @param[in] state - Enum connection_ethernet_cable_state_e
* @return 0 on success, otherwise negative error value
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED Operation failed
* @retval #CONNECTION_ERROR_NOT_SUPPORTED  Not supported
* @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
*/
int connection_get_ethernet_cable_state(connection_h connection, connection_ethernet_cable_state_e *state);

/**
 * @brief Called when ethernet cable is plugged [in/out].
 * @since_tizen 2.4
 * @param[in] state The ethernet cable state (connection_ethernet_cable_state_e)
 * @param[in] user_data The user data passed to callback registration function
 */
typedef void(*connection_ethernet_cable_state_chaged_cb)(
			connection_ethernet_cable_state_e state, void* user_data);

/**
 * @brief Registers callback for ethernet cable is plugged [in/out] event.
 * @since_tizen 2.4
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE   Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED   Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED  Not supported
 */
int connection_set_ethernet_cable_state_chaged_cb( connection_h connection,
		connection_ethernet_cable_state_chaged_cb callback, void *user_data);

/**
 * @brief Unregisters callback for ethernet cable is plugged [in/out] event.
 * @since_tizen 2.4
 * @param[in] connection  The handle of connection
 * @return 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED  Not supported
 */
int connection_unset_ethernet_cable_state_chaged_cb(connection_h connection);

/**
 * @brief Gets the state of the Bluetooth.
 * @details The returned state is for the Bluetooth connection state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.get
 * @param[in] connection  The connection handle
 * @param[out] state  The state of the Bluetooth connection
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
 */
int connection_get_bt_state(connection_h connection, connection_bt_state_e* state);

/**
 * @brief Registers the callback that is called when the type of the current connection is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] connection  The connection handle
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_set_type_changed_cb(connection_h connection, connection_type_changed_cb callback, void* user_data);

/**
 * @brief Unregisters the callback that is called when the type of current connection is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] connection  The connection handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_unset_type_changed_cb(connection_h connection);

/**
 * @brief Registers the callback that is called when the IP address is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] connection  The connection handle
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_set_ip_address_changed_cb(connection_h connection, connection_address_changed_cb callback, void* user_data);

/**
 * @brief Unregisters the callback that is called when the IP address is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] connection  The connection handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_unset_ip_address_changed_cb(connection_h connection);

/**
 * @brief Registers the callback that is called when the proxy address is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] connection  The connection handle
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_set_proxy_address_changed_cb(connection_h connection, connection_address_changed_cb callback, void* user_data);

/**
 * @brief Unregisters the callback that is called when the proxy address is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] connection  The connection handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 */
int connection_unset_proxy_address_changed_cb(connection_h connection);

/**
 * @brief Adds a new profile which is created by connection_profile_create().
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.profile
 * @remarks You can only add a profile of the cellular type.
 * @param[in] connection  The connection handle
 * @param[in] profile  The profile handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not Supported
 */
int connection_add_profile(connection_h connection, connection_profile_h profile);

/**
 * @brief Removes an existing profile.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.profile \n
 * 	      %http://tizen.org/privilege/network.get
 * @remarks This API needs both privileges.
 * @param[in] connection  The connection handle
 * @param[in] profile  The profile handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not Supported
 */
int connection_remove_profile(connection_h connection, connection_profile_h profile);

/**
 * @brief Updates an existing profile.
 * @details When a profile is changed, these changes will be not applied to the Connection Manager immediately.
 * When you call this function, your changes affect the Connection Manager and the existing profile is updated.
 * In addition, the existing profile will be updated if you call connection_open_profile().
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.profile \n
 * 	      %http://tizen.org/privilege/network.get
 * @remarks This API needs both privileges.
 * @param[in] connection  The connection handle
 * @param[in] profile  The profile handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not Supported
 * @see connection_open_profile()
 */
int connection_update_profile(connection_h connection, connection_profile_h profile);

/**
 * @brief Gets a profiles iterator.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.get
 * @remarks You must release @a profile_iterator using connection_destroy().
 * @param[in] connection  The connection handle
 * @param[in] type  The type of the connetion iterator
 * @param[out] profile_iterator  The iterator of profile
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 */
int connection_get_profile_iterator(connection_h connection, connection_iterator_type_e type, connection_profile_iterator_h* profile_iterator);

/**
 * @brief Moves the profile iterator to the next position and gets a profile handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile_iterator  The iterator of profile
 * @param[out] profile  The profile handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_ITERATOR_END  End of iteration
 */
int connection_profile_iterator_next(connection_profile_iterator_h profile_iterator, connection_profile_h* profile);

/**
 * @brief Checks whether the next element of a profile iterator exists or not.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] profile_iterator  The iterator of profile
 * @return @c true if next element exists, otherwise @c false if next element doesn't exist
 */
bool connection_profile_iterator_has_next(connection_profile_iterator_h profile_iterator);

/**
 * @brief Destroys a profiles iterator.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile_iterator  The iterator of the profile
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 */
int connection_destroy_profile_iterator(connection_profile_iterator_h profile_iterator);

/**
 * @brief Gets the name of the default profile.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.get
 * @remarks You must release @a profile using connection_profile_destroy().
 * @param[in] connection  The connection handle
 * @param[out] profile  The profile handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NO_CONNECTION  There is no connection
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 */
int connection_get_current_profile(connection_h connection, connection_profile_h* profile);

/**
 * @brief Gets the default profile which provides the given cellular service.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.get
 * @remarks You must release @a profile using connection_profile_destroy().
 * @param[in] connection  The connection handle
 * @param[in] type  The type of cellular service \n
 *		   #CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION is not permitted.
 * @param[out] profile  The profile handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denieda
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not Supported
 */
int connection_get_default_cellular_service_profile(connection_h connection, connection_cellular_service_type_e type, connection_profile_h* profile);

/**
 * @brief Sets the default profile which provides the given cellular service.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.profile \n
 *	      %http://tizen.org/privilege/network.get
 * @remarks This API needs both privileges.
 * @param[in] connection  The connection handle
 * @param[in] type  The type of cellular service \n
 *		   only #CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET and #CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_INTERNET are permitted.
 * @param[in] profile  The profile handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not Supported
 */
int connection_set_default_cellular_service_profile(connection_h connection, connection_cellular_service_type_e type, connection_profile_h profile);

/**
 * @brief Sets the default profile which provides the given cellular service, asynchronously.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.profile \n
 *	      %http://tizen.org/privilege/network.get
 * @remarks This API needs both privileges.
 * @param[in] connection  The connection handle
 * @param[in] type  The type of cellular service (only #CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET and #CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_INTERNET are permitted)
 * @param[in] profile  The profile handle
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not Supported
 */
int connection_set_default_cellular_service_profile_async(connection_h connection,
		connection_cellular_service_type_e type, connection_profile_h profile, connection_set_default_cb callback, void* user_data);

/**
 * @brief Called after connection_open_profile() is finished.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] result  The result
 * @param[in] user_data The user data passed from connection_open_profile()
 * @pre connection_open_profile() will invoke this callback function.
 * @see connection_open_profile()
*/
typedef void(*connection_opened_cb)(connection_error_e result, void* user_data);

/**
 * @brief Called after connection_close_profile() is finished.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] result  The result
 * @param[in] user_data The user data passed from connection_close_profile()
 * @pre connection_close_profile() will invoke this callback function.
 * @see connection_close_profile()
*/
typedef void(*connection_closed_cb)(connection_error_e result, void* user_data);

/**
 * @brief Called after connection_reset_profile() is finished.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] result  The result
 * @param[in] user_data The user data passed from connection_reset_profile()
 * @pre connection_reset_profile() will invoke this callback function.
 * @see connection_reset_profile()
*/
typedef void(*connection_reset_cb)(connection_error_e result, void* user_data);

/**
 * @brief Opens a connection of profile, asynchronously.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.set \n
 *	      %http://tizen.org/privilege/network.get
 * @remarks This API needs both privileges.
 * @param[in] connection  The connection handle
 * @param[in] profile  The profile handle
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @post connection_opened_cb() will be invoked.
 * @see connection_opened_cb()
 * @see connection_close_profile()
 * @see connection_profile_set_state_changed_cb()
 * @see connection_profile_unset_state_changed_cb()
 * @see connection_profile_state_changed_cb()
 */
int connection_open_profile(connection_h connection, connection_profile_h profile, connection_opened_cb callback, void* user_data);

/**
 * @brief Closes a connection of profile.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.set
 * @param[in] connection  The connection handle
 * @param[in] profile  The profile handle
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @post connection_closed_cb() will be invoked.
 * @see connection_closed_cb()
 * @see connection_open_profile()
 * @see connection_profile_set_state_changed_cb()
 * @see connection_profile_unset_state_changed_cb()
 * @see connection_profile_state_changed_cb()
 */
int connection_close_profile(connection_h connection, connection_profile_h profile, connection_closed_cb callback, void* user_data);

/**
 * @brief Resets the cellular profile.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.profile \n
 *	      %http://tizen.org/privilege/network.get
 * @remarks This API needs both privileges.
 * @param[in] connection  The connection handle
 * @param[in] type  The type of reset
 * @param[in] id  The subscriber identity module id to reset (The sim index starts from 0.)
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not Supported
 * @post connection_reset_cb() will be invoked.
*/
int connection_reset_profile(connection_h connection, connection_reset_option_e type, int id, connection_reset_cb callback, void *user_data);

/**
 * @brief Adds a IPv4 route to the routing table.
 * @details You can get the @a interface_name from connection_profile_get_network_interface_name() of opened profile.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.set
 * @param[in] connection  The connection handle
 * @param[in] interface_name  The name of network interface
 * @param[in] host_address  The IP address of the host
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_ALREADY_EXISTS  Already exists
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @see connection_profile_get_network_interface_name()
 */
int connection_add_route(connection_h connection, const char* interface_name, const char* host_address);

/**
 * @brief Removes a IPv4 route from the routing table.
 * @details You can get the @a interface_name from connection_profile_get_network_interface_name() of opened profile.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.set
 * @param[in] connection  The connection handle
 * @param[in] interface_name  The name of network interface
 * @param[in] host_address  The IP address of the host
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @see connection_profile_get_network_interface_name()
 */
int connection_remove_route(connection_h connection, const char* interface_name, const char* host_address);

/**
 * @brief Adds a IPv6 route to the routing table.
 * @details You can get the @a interface_name from connection_profile_get_network_interface_name() of opened profile.
 * @since_tizen 2.3.1
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.set
 * @param[in] connection  The connection handle
 * @param[in] interface_name  The name of network interface
 * @param[in] host_address  The IP address of the host
 * @param[in] gateway  The gateway address
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_ALREADY_EXISTS  Already exists
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @see connection_profile_get_network_interface_name()
 */
int connection_add_route_ipv6(connection_h connection, const char *interface_name, const char *host_address, const char * gateway);

/**
 * @brief Removes a IPV6 route from the routing table.
 * @details You can get the @a interface_name from connection_profile_get_network_interface_name() of opened profile.
 * @since_tizen 2.3.1
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.set
 * @param[in] connection  The connection handle
 * @param[in] interface_name  The name of network interface
 * @param[in] host_address  The IP address of the host
 * @param[in] gateway  The gateway address
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @see connection_profile_get_network_interface_name()
 */
int connection_remove_route_ipv6(connection_h connection, const char *interface_name, const char *host_address, const char * gateway);

/**
 * @}
*/

/**
 * @addtogroup CAPI_NETWORK_CONNECTION_STATISTICS_MODULE
 * @{
*/

/**
 * @brief Gets the statistics information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.get
 * @param[in] connection  The connection handle
 * @param[in] connection_type  The type of connection (only CONNECTION_TYPE_WIFI and CONNECTION_TYPE_CELLULAR are supported)
 * @param[in] statistics_type  The type of statistics
 * @param[out] size  The received data size of the last cellular packet data connection (bytes)
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not Supported
 */
int connection_get_statistics(connection_h connection, connection_type_e connection_type, connection_statistics_type_e statistics_type, long long* size);

/**
 * @brief Resets the statistics information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.set
 * @remarks This API needs both privileges.
 * @param[in] connection  The connection handle
 * @param[in] connection_type  The type of connection (only CONNECTION_TYPE_WIFI and CONNECTION_TYPE_CELLULAR are supported)
 * @param[in] statistics_type  The type of statistics
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not Supported
 */
int connection_reset_statistics(connection_h connection, connection_type_e connection_type, connection_statistics_type_e statistics_type);

/**
 * @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
