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

#ifndef __TIZEN_NETWORK_CONNECTION_PROFILE_H__
#define __TIZEN_NETWORK_CONNECTION_PROFILE_H__

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @addtogroup CAPI_NETWORK_CONNECTION_WIFI_PROFILE_MODULE
* @{
*/

/**
* @brief Security type of Wi-Fi
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
* @brief Below encryption modes are used in infrastructure and ad-hoc mode
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
* @brief This enumeration defines the cellular protocol type.
*/
typedef enum
{
    CONNECTION_CELLULAR_NETWORK_TYPE_UNKNOWN = 0,  /**< Not defined */
    CONNECTION_CELLULAR_NETWORK_TYPE_GPRS = 1,  /**< GPRS type */
    CONNECTION_CELLULAR_NETWORK_TYPE_EDGE = 2,  /**< EDGE type */
    CONNECTION_CELLULAR_NETWORK_TYPE_UMTS = 3,  /**< UMTS type */
} connection_cellular_network_type_e;

/**
* @breif This enum indicates cellular service type
*/
typedef enum
{
    CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN = 0,  /**< Unknown */
    CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET = 1,  /**< Established for Internet */
    CONNECTION_CELLULAR_SERVICE_TYPE_MMS = 2,  /**< Established for MMS */
    CONNECTION_CELLULAR_SERVICE_TYPE_WAP = 3,  /**< Established for WAP */
    CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_INTERNET = 4,  /**< Established for prepaid internet service */
    CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_MMS = 5,  /**< Established for prepaid MMS service */
} connection_cellular_service_type_e;

/**
* @brief Cellular Authentication Type
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
* @brief The handle of profile
*/
typedef void* connection_profile_h;

/**
* @brief This enumeration defines the profile state type.
*/
typedef enum
{
    CONNECTION_PROFILE_STATE_DISCONNECTED = 0,  /**< Disconnected state */
    CONNECTION_PROFILE_STATE_ASSOCIATION = 1,  /**< Association state */
    CONNECTION_PROFILE_STATE_CONFIGURATION = 2,  /**< Configuration state */
    CONNECTION_PROFILE_STATE_CONNECTED = 3,  /**< Connected state */
} connection_profile_state_e;

/**
* @brief Enumerations of Address family
*/
typedef enum
{
    CONNECTION_ADDRESS_FAMILY_IPV4 = 0,  /**< IPV4 Address type */
    CONNECTION_ADDRESS_FAMILY_IPV6 = 1,  /**< IPV6 Address type */
} connection_address_family_e;

/**
* @brief Net IP configuration Type
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
* @brief This enumeration defines the proxy method type.
*/
typedef enum
{
    CONNECTION_PROXY_TYPE_DIRECT = 0,  /**< Direct connection */
    CONNECTION_PROXY_TYPE_AUTO = 1,  /**< Auto configuration(Use PAC file). If URL property is not set, DHCP/WPAD auto-discover will be tried */
    CONNECTION_PROXY_TYPE_MANUAL  = 2,  /**< Manual configuration */
} connection_proxy_type_e;

/**
* @enum connection_profile_type_e
* @brief Enumerations of network connection type.
*/
typedef enum{
    CONNECTION_PROFILE_TYPE_CELLULAR = 0,  /**< Cellular type */
    CONNECTION_PROFILE_TYPE_WIFI = 1,  /**< Wi-Fi type */
    CONNECTION_PROFILE_TYPE_ETHERNET = 2,  /**< Ethernet type */
} connection_profile_type_e;

/**
* @brief Creates the profile handle.
* @remarks @a profile must be released with connection_profile_destroy().
* @param[in] type  The type of profile
* @param[out] profile  The handle of the profile
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
* @see connection_profile_destroy()
*/
int connection_profile_create(connection_profile_type_e type, connection_profile_h* profile);

/**
* @brief Destroys the profile handle.
* @param[out] connection  The handle to the connection
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @see connection_profile_create()
*/
int connection_profile_destroy(connection_profile_h profile);

/**
* @brief Clons the profile handle.
* @remarks @a cloned_profile must be released with connection_profile_destroy().
* @param[in] origin_profile  The handle of origin profile
* @param[out] cloned_profile  The handle of cloned profile
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
* @see connection_profile_destroy()
*/
int connection_profile_clone(connection_profile_h* cloned_profile, connection_profile_h origin_profile);

/**
* @brief Gets the profile name.
* @remarks @a profile_name must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[out] profile_name  The name of profile
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
*/
int connection_profile_get_name(connection_profile_h profile, char** profile_name);

/**
* @brief Gets the network type.
* @param[in] profile  The handle of profile
* @param[out] type  The type of profile
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_type(connection_profile_h profile, connection_profile_type_e* type);

/**
* @brief Gets the name of network interface. For example, eth0 and pdp0.
* @remarks @a interface_name must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[out] interface_name  The name of network interface
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
*/
int connection_profile_get_network_interface_name(connection_profile_h profile, char** interface_name);

/**
* @brief Gets the network type.
* @param[in] profile  The handle of profile
* @param[out] state  The state of profile
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_state(connection_profile_h profile, connection_profile_state_e* state);

/**
* @brief Gets the IP config type.
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[out] type  The type of IP config
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_ip_config_type(connection_profile_h profile, connection_address_family_e address_family, connection_ip_config_type_e* type);

/**
* @brief Gets the IP address.
* @remarks @a ip_address must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[out] ip_address  The IP address
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
* @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_ip_address(connection_profile_h profile, connection_address_family_e address_family, char** ip_address);

/**
* @brief Gets the Subnet Mask.
* @remarks @a subnet_mask must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[out] subnet_mask  The subnet mask
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
* @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_subnet_mask(connection_profile_h profile, connection_address_family_e address_family, char** subnet_mask);

/**
* @brief Gets the Gateway address.
* @remarks @a gateway_address must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[out] gateway_address  The gateway address
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
* @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_gateway_address(connection_profile_h profile, connection_address_family_e address_family, char** gateway_address);

/**
* @brief Gets the DNS address.
* @remarks The allowance of DNS address is 2. @a dns_address must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[in] order  The order of DNS address. It starts from 1, which means first DNS address.
* @param[in] address_family  The address family
* @param[out] dns_address  The DNS address
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
* @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_dns_address(connection_profile_h profile, int order, connection_address_family_e address_family, char** dns_address);

/**
* @brief Gets the Proxy type.
* @param[in] profile  The handle of profile
* @param[out] type  The type of proxy
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_proxy_type(connection_profile_h profile, connection_proxy_type_e* type);

/**
* @brief Gets the Proxy address.
* @remarks @a proxy_address must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[out] proxy_address  The proxy address
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
* @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_proxy_address(connection_profile_h profile, connection_address_family_e address_family, char** proxy_address);

/**
* @brief Sets the IP config type.
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[in] type  The type of IP config
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_set_ip_config_type(connection_profile_h profile, connection_address_family_e address_family, connection_ip_config_type_e type);

/**
* @brief Sets the IP address.
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[in] ip_address  The IP address
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_set_ip_address(connection_profile_h profile, connection_address_family_e address_family, const char* ip_address);

/**
* @brief Sets the Subnet Mask.
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[in] subnet_mask  The subnet mask
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_set_subnet_mask(connection_profile_h profile, connection_address_family_e address_family, const char* subnet_mask);

/**
* @brief Sets the Gateway address.
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[in] gateway_address  The gateway address
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_set_gateway_address(connection_profile_h profile, connection_address_family_e address_family, const char* gateway_address);

/**
* @brief Sets the DNS address.
* @remarks The allowance of DNS address is 2.
* @param[in] profile  The handle of profile
* @param[in] order  The order of DNS address. It starts from 1, which means first DNS address.
* @param[in] address_family  The address family
* @param[in] dns_address  The DNS address
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_set_dns_address(connection_profile_h profile, int order, connection_address_family_e address_family, const char* dns_address);

/**
* @brief Sets the Proxy type.
* @param[in] profile  The handle of profile
* @param[in] type  The type of proxy
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_set_proxy_type(connection_profile_h profile, connection_proxy_type_e type);

/**
* @brief Sets the Proxy address.
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[in] gateway_address  The gateway address
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED  Not supported address family
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_set_proxy_address(connection_profile_h profile, connection_address_family_e address_family, const char* proxy_address);

/**
* @brief Called when the state of profile is changed.
* @param[in] profile  The handle of profile
* @param[in] is_requested  Indicates whether this change is requested or not
* @param[in] user_data The user data passed from the callback registration function
* @see connection_profile_set_state_changed_cb()
* @see connection_profile_unset_state_changed_cb()
*/
typedef void(*connection_profile_state_changed_cb)(connection_profile_h profile, bool is_requested, void* user_data);

/**
* @brief Registers the callback called when the state of profile is changed.
* @param[in] profile  The handle of profile
* @param[in] callback  The callback function to be called
* @param[in] user_data The user data passed to the callback function
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
* @see connection_profile_state_changed_cb()
* @see connection_profile_unset_state_changed_cb()
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_set_state_changed_cb(connection_profile_h profile, connection_profile_state_changed_cb callback, void* user_data);

/**
* @brief Unregisters the callback called when the state of profile is changed.
* @param[in] profile  The handle of profile
* @return 0 on success, otherwise negative error value.
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
* @brief Gets the ESSID(Extended Service Set Identifier).
* @remarks @a essid must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[out] essid  The ESSID
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
*/
int connection_profile_get_wifi_essid(connection_profile_h profile, char** essid);

/**
* @brief Gets the BSSID(Basic Service Set Identifier).
* @remarks @a bssid must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[out] bssid  The BSSID
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
*/
int connection_profile_get_wifi_bssid(connection_profile_h profile, char** bssid);

/**
* @brief Gets the RSSI.
* @param[in] profile  The handle of profile
* @param[out] rssi  The RSSI
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
*/
int connection_profile_get_wifi_rssi(connection_profile_h profile, int* rssi);

/**
* @brief Gets the frequency (MHz).
* @param[in] profile  The handle of profile
* @param[out] frequency  The frequency
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
*/
int connection_profile_get_wifi_frequency(connection_profile_h profile, int* frequency);

/**
* @brief Gets the max speed (Mbps).
* @param[in] profile  The handle of profile
* @param[out] max_speed  The max speed
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
*/
int connection_profile_get_wifi_max_speed(connection_profile_h profile, int* max_speed);

/**
* @brief Gets the security mode of Wi-Fi.
* @param[in] profile  The handle of profile
* @param[out] type  The type of Wi-Fi security
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_wifi_security_type(connection_profile_h profile, connection_wifi_security_type_e* type);

/**
* @brief Gets the security mode of Wi-Fi.
* @param[in] profile  The handle of profile
* @param[out] type  The type of Wi-Fi security
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_wifi_encryption_type(connection_profile_h profile, connection_wifi_encryption_type_e* type);

/**
* @brief Checks whether passphrase is required.
* @param[in] profile  The handle of profile
* @param[out] required  Indicates whether passphrase is required or not
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
*/
int connection_profile_is_wifi_passphrase_required(connection_profile_h profile, bool* required);

/**
* @brief Sets the passphrase of Wi-Fi WPA.
* @param[in] profile  The handle of profile
* @param[in] passphrase  The passphrase of Wi-Fi security
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
*/
int connection_profile_set_wifi_passphrase(connection_profile_h profile, const char* passphrase);

/**
* @brief Checks whether WPS(Wi-Fi Protected Setup) is supported.
* @remarks If WPS is supported, you can connect access point with WPS by wifi_connect_with_wps().
* @param[in] profile  The handle of profile
* @param[out] supported  Indicates whether WPS is supported or not
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
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
* @brief Gets the cellular network type.
* @param[in] profile  The handle of profile
* @param[out] type  The type of cellular
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_cellular_network_type(connection_profile_h profile, connection_cellular_network_type_e* type);

/**
* @brief Gets the service type.
* @param[in] profile  The handle of profile
* @param[out] type  The type of cellular service
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_cellular_service_type(connection_profile_h profile, connection_cellular_service_type_e* type);

/**
* @brief Gets the APN(access point name).
* @remarks @a apn must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[out] apn  The name of APN
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
*/
int connection_profile_get_cellular_apn(connection_profile_h profile, char** apn);

/**
* @brief Gets the authentication information.
* @remarks @a user_name and @a password must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[out] type  The type of authentication
* @param[out] user_name  The user name
* @param[out] password  The password
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
*/
int connection_profile_get_cellular_auth_info(connection_profile_h profile, connection_cellular_auth_type_e* type, char** user_name, char** password);

/**
* @brief Gets the home URL.
* @remarks @a home_url must be released with free() by you.
* @param[in] profile  The handle of profile
* @param[out] home_url  The home URL
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory
*/
int connection_profile_get_cellular_home_url(connection_profile_h profile, char** home_url);

/**
* @brief Gets the state of roaming.
* @param[in] profile  The handle of profile
* @param[out] is_roaming  Indicates whether cellular is in roaming or not
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
*/
int connection_profile_is_cellular_roaming(connection_profile_h profile, bool* is_roaming);

/**
* @brief Sets the service type.
* @param[in] profile  The handle of profile
* @param[out] type  The type of cellular service
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
*/
int connection_profile_set_cellular_service_type(connection_profile_h profile, connection_cellular_service_type_e service_type);

/**
* @brief Sets the APN(Access Point Name).
* @param[in] profile  The handle of profile
* @param[out] apn  The name of APN
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
*/
int connection_profile_set_cellular_apn(connection_profile_h profile, const char* apn);

/**
* @brief Sets the Athentication information.
* @param[in] profile  The handle of profile
* @param[out] type  The type of authentication
* @param[out] user_name  The user name
* @param[out] password  The password
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
*/
int connection_profile_set_cellular_auth_info(connection_profile_h profile, connection_cellular_auth_type_e type, const char* user_name, const char* password);

/**
* @brief Sets the home URL.
* @param[in] profile  The handle of profile
* @param[out] home_url  The home URL
* @return 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
*/
int connection_profile_set_cellular_home_url(connection_profile_h profile, const char* home_url);

/**
* @}
*/


#ifdef __cplusplus
}

#endif

#endif /* __TIZEN_NETWORK_CONNECTION_PROFILE_H__ */
