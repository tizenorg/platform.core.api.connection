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

#ifndef __TIZEN_NETWORK_CONNECTION_PROFILE_H__
#define __TIZEN_NETWORK_CONNECTION_PROFILE_H__

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file connection_profile.h
 */

/**
* @addtogroup CAPI_NETWORK_CONNECTION_WIFI_PROFILE_MODULE
* @{
*/

/**
 * @brief Enumeration for security type of Wi-Fi.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef enum
{
    CONNECTION_WIFI_SECURITY_TYPE_NONE = 0,  /**< Security disabled */
    CONNECTION_WIFI_SECURITY_TYPE_WEP = 1,  /**< WEP */
    CONNECTION_WIFI_SECURITY_TYPE_WPA_PSK = 2,  /**< WPA-PSK */
    CONNECTION_WIFI_SECURITY_TYPE_WPA2_PSK = 3,  /**< WPA2-PSK */
    CONNECTION_WIFI_SECURITY_TYPE_EAP = 4,  /**< EAP */
} connection_wifi_security_type_e;

/**
 * @brief Enumeration for encryption modes.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef enum
{
    CONNECTION_WIFI_ENCRYPTION_TYPE_NONE = 0,  /**< Encryption disabled */
    CONNECTION_WIFI_ENCRYPTION_TYPE_WEP = 1,  /**< WEP */
    CONNECTION_WIFI_ENCRYPTION_TYPE_TKIP = 2,  /**< TKIP */
    CONNECTION_WIFI_ENCRYPTION_TYPE_AES = 3,  /**< AES */
    CONNECTION_WIFI_ENCRYPTION_TYPE_TKIP_AES_MIXED = 4,  /**< TKIP and AES are both supported */
} connection_wifi_encryption_type_e;

/**
* @}
*/


/**
* @addtogroup CAPI_NETWORK_CONNECTION_CELLULAR_PROFILE_MODULE
* @{
*/

/**
 * @brief Enumeration for cellular service type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef enum
{
    CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN = 0,  /**< Unknown */
    CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET = 1,  /**< Internet */
    CONNECTION_CELLULAR_SERVICE_TYPE_MMS = 2,  /**< MMS */
    CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_INTERNET = 3,  /**< Prepaid internet */
    CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_MMS = 4,  /**< Prepaid MMS */
    CONNECTION_CELLULAR_SERVICE_TYPE_TETHERING = 5,  /**< Tethering */
    CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION = 6,  /**< Specific application */
} connection_cellular_service_type_e;

/**
 * @brief Enumeration for cellular authentication type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef enum
{
    CONNECTION_CELLULAR_AUTH_TYPE_NONE = 0,  /**< No authentication */
    CONNECTION_CELLULAR_AUTH_TYPE_PAP  = 1,  /**< PAP authentication */
    CONNECTION_CELLULAR_AUTH_TYPE_CHAP = 2,  /**< CHAP authentication */
} connection_cellular_auth_type_e;

/**
* @}
*/


/**
* @addtogroup CAPI_NETWORK_CONNECTION_PROFILE_MODULE
* @{
*/

/**
 * @brief The profile handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef void* connection_profile_h;

/**
 * @brief Enumeration for profile state type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef enum
{
    CONNECTION_PROFILE_STATE_DISCONNECTED = 0,  /**< Disconnected state */
    CONNECTION_PROFILE_STATE_ASSOCIATION = 1,  /**< Association state */
    CONNECTION_PROFILE_STATE_CONFIGURATION = 2,  /**< Configuration state */
    CONNECTION_PROFILE_STATE_CONNECTED = 3,  /**< Connected state */
} connection_profile_state_e;

/**
 * @brief Enumeration for address family.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef enum
{
    CONNECTION_ADDRESS_FAMILY_IPV4 = 0,  /**< IPV4 Address type */
    CONNECTION_ADDRESS_FAMILY_IPV6 = 1,  /**< IPV6 Address type */
} connection_address_family_e;

/**
 * @brief Enumeration for IP configuration type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef enum
{
    CONNECTION_IP_CONFIG_TYPE_NONE = 0,  /**< Not defined */
    CONNECTION_IP_CONFIG_TYPE_STATIC  = 1,  /**< Manual IP configuration */
    CONNECTION_IP_CONFIG_TYPE_DYNAMIC = 2,  /**< Config IP using DHCP client*/
    CONNECTION_IP_CONFIG_TYPE_AUTO = 3,  /**< Config IP from Auto IP pool (169.254/16). Later with DHCP client, if available */
    CONNECTION_IP_CONFIG_TYPE_FIXED = 4,  /**< Indicates an IP address that can not be modified */
} connection_ip_config_type_e;

/**
 * @brief Enumeration for proxy method type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef enum
{
    CONNECTION_PROXY_TYPE_DIRECT = 0,  /**< Direct connection */
    CONNECTION_PROXY_TYPE_AUTO = 1,  /**< Auto configuration(Use PAC file). If URL property is not set, DHCP/WPAD auto-discover will be tried */
    CONNECTION_PROXY_TYPE_MANUAL  = 2,  /**< Manual configuration */
} connection_proxy_type_e;

/**
 * @brief Enumeration for network connection type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
*/
typedef enum{
    CONNECTION_PROFILE_TYPE_CELLULAR = 0,  /**< Cellular type */
    CONNECTION_PROFILE_TYPE_WIFI = 1,  /**< Wi-Fi type */
    CONNECTION_PROFILE_TYPE_ETHERNET = 2,  /**< Ethernet type */
    CONNECTION_PROFILE_TYPE_BT = 3,  /**< Bluetooth type */
} connection_profile_type_e;

/**
 * @brief Creates a profile handle.
 * @details The profile name, which you get from connection_profile_get_name(), will include the keyword you set.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.get
 * @remarks You must release @a profile using connection_profile_destroy().
 * @param[in] type  The type of profile\n
 *		 #CONNECTION_PROFILE_TYPE_CELLULAR and #CONNECTION_PROFILE_TYPE_WIFI are supported.
 * @param[in] keyword  The keyword included in profile name
 * @param[out] profile  The handle of the profile
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
 * @see connection_profile_destroy()
 * @see connection_profile_get_name()
*/
int connection_profile_create(connection_profile_type_e type, const char* keyword, connection_profile_h* profile);

/**
 * @brief Destroys a profile handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[out] profile  The handle to the profile
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @see connection_profile_create()
*/
int connection_profile_destroy(connection_profile_h profile);

/**
 * @brief Clones a profile handle.
 * @since_tizen 2.3
 * @remarks You must release @a cloned_profile using connection_profile_destroy().
 * @param[out] cloned_profile  The handle of the cloned profile
 * @param[in] origin_profile  The handle of the origin profile
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @see connection_profile_destroy()
*/
int connection_profile_clone(connection_profile_h* cloned_profile, connection_profile_h origin_profile);

/**
 * @brief Gets the profile ID.
 * @details The separate profiles can have the same name.
 * So, you must use this API instead of connection_profile_get_name() if you want to get the unique identification.
 * In case you create a profile, this value will be determined when you add the profile.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a profile_id using free().
 * @param[in] profile  The profile handle
 * @param[out] profile_id  The ID of the profile
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @see connection_profile_get_name()
 * @see connection_add_profile()
*/
int connection_profile_get_id(connection_profile_h profile, char** profile_id);

/**
 * @brief Gets the profile name.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a profile_name using free().
 * @param[in] profile  The profile handle
 * @param[out] profile_name  The name of the profile
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @see connection_profile_get_id()
*/
int connection_profile_get_name(connection_profile_h profile, char** profile_name);

/**
 * @brief Gets the network type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] type  The type of the profile
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_type(connection_profile_h profile, connection_profile_type_e* type);

/**
 * @brief Gets the name of the network interface, e.g. eth0 and pdp0.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a interface_name using free().
 * @param[in] profile  The profile handle
 * @param[out] interface_name  The name of the network interface
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
*/
int connection_profile_get_network_interface_name(connection_profile_h profile, char** interface_name);

/**
 * @brief Refreshes the profile information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/network.get
 * @remarks You should call this function in order to get the current information because the profile information can be changed.
 * @param[in] profile  The profile handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
*/
int connection_profile_refresh(connection_profile_h profile);

/**
 * @brief Gets the network type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] state  The state of the profile
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_state(connection_profile_h profile, connection_profile_state_e* state);

/**
 * @brief Gets the IP config type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] address_family  The address family
 * @param[out] type  The type of the IP config
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_ip_config_type(connection_profile_h profile, connection_address_family_e address_family, connection_ip_config_type_e* type);

/**
 * @brief Gets the IP address.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a ip_address using free().
 * @param[in] profile  The profile handle
 * @param[in] address_family  The address family
 * @param[out] ip_address  The IP address
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_ip_address(connection_profile_h profile, connection_address_family_e address_family, char** ip_address);

/**
 * @brief Gets the Subnet Mask.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a subnet_mask using free().
 * @param[in] profile  The profile handle
 * @param[in] address_family  The address family
 * @param[out] subnet_mask  The subnet mask
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_subnet_mask(connection_profile_h profile, connection_address_family_e address_family, char** subnet_mask);

/**
 * @brief Gets the Gateway address.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a gateway_address using free().
 * @param[in] profile  The profile handle
 * @param[in] address_family  The address family
 * @param[out] gateway_address  The gateway address
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_gateway_address(connection_profile_h profile, connection_address_family_e address_family, char** gateway_address);

/**
 * @brief Gets the DNS address.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks The allowance of the DNS address is @c 2. You must release @a dns_address using free().
 * @param[in] profile  The profile handle
 * @param[in] order  The order of DNS address \n
 *		it starts from 1, which means first DNS address.
 * @param[in] address_family  The address family
 * @param[out] dns_address  The DNS address
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_dns_address(connection_profile_h profile, int order, connection_address_family_e address_family, char** dns_address);

/**
 * @brief Gets the Proxy type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] type  The type of the proxy
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_proxy_type(connection_profile_h profile, connection_proxy_type_e* type);

/**
 * @brief Gets the Proxy address.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a proxy_address using free().
 * @param[in] profile  The profile handle
 * @param[in] address_family  The address family
 * @param[out] proxy_address  The proxy address
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_proxy_address(connection_profile_h profile, connection_address_family_e address_family, char** proxy_address);

/**
 * @brief Sets the IP config type.
 * @details If you set IP config type to #CONNECTION_IP_CONFIG_TYPE_STATIC,
 * then IP address, Gateway and Subnet mask will be set to the initial value "0.0.0.0".
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] address_family  The address family
 * @param[in] type  The type of the IP config
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_set_ip_config_type(connection_profile_h profile, connection_address_family_e address_family, connection_ip_config_type_e type);

/**
 * @brief Sets the IP address.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] address_family  The address family
 * @param[in] ip_address  The IP address.\n
 *			If you set this value to @c NULL, then the existing value will be deleted.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @see connection_update_profile()
*/
int connection_profile_set_ip_address(connection_profile_h profile, connection_address_family_e address_family, const char* ip_address);

/**
 * @brief Sets the Subnet Mask.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] address_family  The address family
 * @param[in] subnet_mask  The subnet mask. \n
 *			If you set this value to @c NULL, then the existing value will be deleted.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @see connection_update_profile()
*/
int connection_profile_set_subnet_mask(connection_profile_h profile, connection_address_family_e address_family, const char* subnet_mask);

/**
 * @brief Sets the Gateway address.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] address_family  The address family
 * @param[in] gateway_address  The gateway address. \n
 *			If you set this value to @c NULL, then the existing value will be deleted.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @see connection_update_profile()
*/
int connection_profile_set_gateway_address(connection_profile_h profile, connection_address_family_e address_family, const char* gateway_address);

/**
 * @brief Sets the DNS address.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks The allowance of the DNS address is @c 2.
 * @param[in] profile  The profile handle
 * @param[in] order  The order of the DNS address. \n
 *		It starts from @c 1, which means first DNS address.
 * @param[in] address_family  The address family
 * @param[in] dns_address  The DNS address; if you set this value to NULL, then the existing value will be deleted
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @see connection_update_profile()
*/
int connection_profile_set_dns_address(connection_profile_h profile, int order, connection_address_family_e address_family, const char* dns_address);

/**
 * @brief Sets the Proxy type.
 * @details If you set the Proxy type to #CONNECTION_PROXY_TYPE_AUTO or #CONNECTION_PROXY_TYPE_MANUAL, then Proxy will be restored.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] type  The type of the proxy
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @see connection_update_profile()
*/
int connection_profile_set_proxy_type(connection_profile_h profile, connection_proxy_type_e type);

/**
 * @brief Sets the Proxy address.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] address_family  The address family
 * @param[in] proxy_address  The proxy address. \n
 *			if you set this value to @c NULL, then the existing value will be deleted.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @see connection_update_profile()
*/
int connection_profile_set_proxy_address(connection_profile_h profile, connection_address_family_e address_family, const char* proxy_address);

/**
 * @brief Called when the state of the profile is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] state  The state
 * @param[in] user_data The user data passed from the callback registration function
 * @see connection_profile_set_state_changed_cb()
 * @see connection_profile_unset_state_changed_cb()
*/
typedef void(*connection_profile_state_changed_cb)(connection_profile_state_e state, void* user_data);

/**
 * @brief Registers the callback that is called when the state of profile is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] callback  The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @post connection_opened_cb() is invoked when the state of profile is changed.
 * @see connection_profile_state_changed_cb()
 * @see connection_profile_unset_state_changed_cb()
*/
int connection_profile_set_state_changed_cb(connection_profile_h profile, connection_profile_state_changed_cb callback, void* user_data);

/**
 * @brief Unregisters the callback that is called when the state of profile is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @see connection_profile_state_changed_cb()
 * @see connection_profile_set_state_changed_cb()
*/
int connection_profile_unset_state_changed_cb(connection_profile_h profile);

/**
* @}
*/


/**
* @addtogroup CAPI_NETWORK_CONNECTION_WIFI_PROFILE_MODULE
* @{
*/

/**
 * @brief Gets the ESSID (Extended Service Set Identifier).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a essid using free().
 * @param[in] profile  The profile handle
 * @param[out] essid  The ESSID
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_get_wifi_essid(connection_profile_h profile, char** essid);

/**
 * @brief Gets the BSSID (Basic Service Set Identifier).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a bssid using free().
 * @param[in] profile  The profile handle
 * @param[out] bssid  The BSSID
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_get_wifi_bssid(connection_profile_h profile, char** bssid);

/**
 * @brief Gets the RSSI.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] rssi  The RSSI
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_get_wifi_rssi(connection_profile_h profile, int* rssi);

/**
 * @brief Gets the frequency (MHz).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] frequency  The frequency
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_get_wifi_frequency(connection_profile_h profile, int* frequency);

/**
 * @brief Gets the max speed (Mbps).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] max_speed  The max speed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_get_wifi_max_speed(connection_profile_h profile, int* max_speed);

/**
 * @brief Gets the security mode of Wi-Fi.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] type  The type of Wi-Fi security
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_get_wifi_security_type(connection_profile_h profile, connection_wifi_security_type_e* type);

/**
 * @brief Gets the security mode of Wi-Fi.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] type  The type of Wi-Fi security
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_get_wifi_encryption_type(connection_profile_h profile, connection_wifi_encryption_type_e* type);

/**
 * @brief Checks whether passphrase is required.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks This function is not valid if security type is #CONNECTION_WIFI_SECURITY_TYPE_EAP.
 * @param[in] profile  The profile handle
 * @param[out] required  @c true if a passphrase is required, otherwise @c false if a passphrase is not required.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_is_wifi_passphrase_required(connection_profile_h profile, bool* required);

/**
 * @brief Sets the passphrase of the Wi-Fi WPA.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] passphrase  The passphrase of Wi-Fi security
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
 * @see connection_update_profile()
*/
int connection_profile_set_wifi_passphrase(connection_profile_h profile, const char* passphrase);

/**
 * @brief Checks whether the WPS (Wi-Fi Protected Setup) is supported.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks If WPS is supported, you can connect the access point with WPS by wifi_connect_with_wps().
 * @param[in] profile  The profile handle
 * @param[out] supported  @c true if WPS is supported, otherwise @c false if WPS is not supported.
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_is_wifi_wps_supported(connection_profile_h profile, bool* supported);

/**
* @}
*/


/**
* @addtogroup CAPI_NETWORK_CONNECTION_CELLULAR_PROFILE_MODULE
* @{
*/

/**
 * @brief Gets the service type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] type  The type of the cellular service
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_get_cellular_service_type(connection_profile_h profile, connection_cellular_service_type_e* type);

/**
 * @brief Gets the APN (access point name).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a apn using free().
 * @param[in] profile  The profile handle
 * @param[out] apn  The name of the APN
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_get_cellular_apn(connection_profile_h profile, char** apn);

/**
 * @brief Gets the authentication information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a user_name and @a password using free().
 * @param[in] profile  The profile handle
 * @param[out] type  The type of the authentication
 * @param[out] user_name  The user name
 * @param[out] password  The password
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_get_cellular_auth_info(connection_profile_h profile, connection_cellular_auth_type_e* type, char** user_name, char** password);

/**
 * @brief Gets the home URL.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a home_url using free().
 * @param[in] profile  The profile handle
 * @param[out] home_url  The home URL
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_get_cellular_home_url(connection_profile_h profile, char** home_url);

/**
 * @brief Gets the cellular pdn type.
 * @since_tizen 3.0
 * @param[in] profile The profile handle
 * @param[in] type The cellular pdn type
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED     Not Supported
*/
int connection_profile_get_cellular_pdn_type(connection_profile_h profile, connection_cellular_pdn_type_e* type);

/**
 * @brief Gets the cellular roam pdn type.
 * @since_tizen 3.0
 * @param[in] profile The profile handle
 * @param[in] type The cellular pdn type
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED     Not Supported
*/
int connection_profile_get_cellular_roam_pdn_type(connection_profile_h profile, connection_cellular_pdn_type_e* type);

/**
 * @brief Checks wheter the connection is in roaming state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] is_roaming  @c true if the cellular is roaming, otherwise @c false if it is not roaming.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_is_cellular_roaming(connection_profile_h profile, bool* is_roaming);

/**
 * @brief Checks whether the profile is hidden.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] is_hidden @c ture if the profile is in hidden, otherwise @c false if the profile is not hidden.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_is_cellular_hidden(connection_profile_h profile, bool* is_hidden);

/**
 * @brief Checks whether the profile is editable.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] is_editable  @c true if the profile is editable, otherwise @c false if the profile is not editable.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_is_cellular_editable(connection_profile_h profile, bool* is_editable);

/**
 * @brief Checks whether the profile is default.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[out] is_default  @c true if the profile is default, otherwise @c false if the profile is not default.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
*/
int connection_profile_is_cellular_default(connection_profile_h profile, bool* is_default);

/**
 * @brief Sets the service type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] service_type  The type of cellular service
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
 * @see connection_update_profile()
*/
int connection_profile_set_cellular_service_type(connection_profile_h profile, connection_cellular_service_type_e service_type);

/**
 * @brief Sets the APN (Access Point Name).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] apn  The name of APN
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
 * @see connection_update_profile()
*/
int connection_profile_set_cellular_apn(connection_profile_h profile, const char* apn);

/**
 * @brief Sets the Authentication information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] type  The type of the authentication
 * @param[in] user_name  The user name
 * @param[in] password  The password
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
 * @see connection_update_profile()
*/
int connection_profile_set_cellular_auth_info(connection_profile_h profile, connection_cellular_auth_type_e type, const char* user_name, const char* password);

/**
 * @brief Sets the home URL.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] profile  The profile handle
 * @param[in] home_url  The home URL
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED	Not supported
 * @see connection_update_profile()
*/
int connection_profile_set_cellular_home_url(connection_profile_h profile, const char* home_url);

/**
 * @brief Sets the cellular pdn type.
 * @since_tizen 3.0
 * @param[in] profile The profile handle
 * @param[in] type The cellular pdn type
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED     Not Supported
*/
int connection_profile_set_cellular_pdn_type(connection_profile_h profile, connection_cellular_pdn_type_e type);

/**
 * @brief Sets the cellular roam pdn type.
 * @since_tizen 3.0
 * @param[in] profile The profile handle
 * @param[in] type The cellular pdn type
 * @return @c 0 on success, otherwise negative error value
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
 * @retval #CONNECTION_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #CONNECTION_ERROR_NOT_SUPPORTED     Not Supported
*/
int connection_profile_set_cellular_roam_pdn_type(connection_profile_h profile, connection_cellular_pdn_type_e type);

/**
* @}
*/


#ifdef __cplusplus
}

#endif

#endif /* __TIZEN_NETWORK_CONNECTION_PROFILE_H__ */
