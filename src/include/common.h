/*
* Network Wi-Fi library
*
* Copyright (c) 2014-2015 Intel Corporation. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*              http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#ifndef __NETWORK_PM_WLAN_H__
#define __NETWORK_PM_WLAN_H__

#include "network-pm-config.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**  \file network-pm-wlan.h
     \brief This file contains constants, enums, tructs, and function
     prototypes that are used by Wlan related sources internally.
     This File defines the WLAN exported Data Structures.

*/

/**
 * \addtogroup  profile_managing
 * \{
*/

/*
==================================================================================================
                                           CONSTANTS
==================================================================================================
*/
/** MAX number of DNS Address */
#define	NET_DNS_ADDR_MAX 2

/** Length of essid */
#define NET_WLAN_ESSID_LEN  32 /* As per  IEEE802.11 standard , SSID Length should be 32 Char*/

/**
 * Length of WPS PIN code
 * WPS PIN code should be 4 or 8 digits
 */
#define NET_WLAN_MAX_WPSPIN_LEN		8

/**
 * Passphrase length should be between 8..63,
 * If we plan to use encrypted key(hex value generated by wpa_passphrase),
 * then we have to set this value to some higher number
 *
 */
#define NETPM_WLAN_MAX_PSK_PASSPHRASE_LEN 65

/**
 * Length of WEP Key
 * Max of 10 Hex digits allowed in case of 64 bit encryption
 * Max of 26 Hex digits allowed in case of 128 bit encryption
 */
#define NETPM_WLAN_MAX_WEP_KEY_LEN        26

/**
 * These lengths depends on authentication server being used,
 * In case of freeradius server Max allowed length for username/password is 255
 * Let us restrict this value to some optimal value say 50.
 * Used by EAP-TLS, optional for EAP-TTLS and EAP-PEAP
 */
#define NETPM_WLAN_USERNAME_LEN               50

/**
 * These lengths depends on authentication server being used,
 * In case of freeradius server Max allowed length for username/password is 255
 * Let us restrict this value to some optimal value say 50.
 * Used by EAP-TLS, optional for EAP-TTLS and EAP-PEAP
 */
#define NETPM_WLAN_PASSWORD_LEN               50

/**
 * length of CA Cert file name
 * Used by EAP-TLS, optional for EAP-TTLS and EAP-PEAP
 */
#define NETPM_WLAN_CA_CERT_FILENAME_LEN       50

/**
 * length of Client Cert file name
 * Used by EAP-TLS, optional for EAP-TTLS and EAP-PEAP
 */
#define NETPM_WLAN_CLIENT_CERT_FILENAME_LEN   50

/**
 * length of private key file name
 * Used by EAP-TLS, optional for EAP-TTLS and EAP-PEAP
 */
#define NETPM_WLAN_PRIVATE_KEY_FILENAME_LEN   50

/**
 * length of Private key password
 * Used by EAP-TLS, optional for EAP-TTLS and EAP-PEAP
 */
#define NETPM_WLAN_PRIVATE_KEY_PASSWD_LEN     50

/*===========================================================================
                                             ENUMS
=============================================================================*/
/*
 * Added:Begin
 */

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
/**
 * \addtogroup  common_info
 * \{
*/

/**
* @enum net_cm_network_status_t
* This enum indicates network status
*/
typedef enum
{
	/** Service unknown */
	NET_STATUS_UNKNOWN,
	/** Not connected / Suspended / Idle / Connecting/ Disconnecting/ Killing*/
	NET_STATUS_UNAVAILABLE,
	/** Active */
	NET_STATUS_AVAILABLE,
} net_cm_network_status_t;

/**
 * @brief Enumerations of statistics type.
 */
typedef enum
{
	NET_STATISTICS_TYPE_LAST_RECEIVED_DATA = 0,  /**< Last received data */
	NET_STATISTICS_TYPE_LAST_SENT_DATA = 1,  /**< Last sent data */
	NET_STATISTICS_TYPE_TOTAL_RECEIVED_DATA = 2,  /**< Total received data */
	NET_STATISTICS_TYPE_TOTAL_SENT_DATA = 3,  /**< Total sent data */
} net_statistics_type_e;

/**
 * \}
 */

typedef enum {
	/** No error */
	NET_ERR_NONE = 0x00,

	/* Common Error value */

	/** Error unknown */
	NET_ERR_UNKNOWN = -999,

	/* Client Register related Errors used in API return */

	/** Application is already registered */
	NET_ERR_APP_ALREADY_REGISTERED = -990,
	/** Application is not registered */
	NET_ERR_APP_NOT_REGISTERED = -989,

	/* Connection Related Error */

	/** No active connection exists for the given profile name */
	NET_ERR_NO_ACTIVE_CONNECTIONS = -899,
	/** Active connection already exists for the given profile name  */
	NET_ERR_ACTIVE_CONNECTION_EXISTS = -898,

	/** Connection failure : out of range */
	NET_ERR_CONNECTION_OUT_OF_RANGE = -897,
	/** Connection failure : pin missing */
	NET_ERR_CONNECTION_PIN_MISSING = -896,
	/** Connection failure : dhcp failed */
	NET_ERR_CONNECTION_DHCP_FAILED = -895,
	/** Connection failure */
	NET_ERR_CONNECTION_CONNECT_FAILED = -894,
	/** Connection failure : login failed */
	NET_ERR_CONNECTION_LOGIN_FAILED = -893,
	/** Connection failure : authentication failed */
	NET_ERR_CONNECTION_AUTH_FAILED = -892,
	/** Connection failure : invalid key */
	NET_ERR_CONNECTION_INVALID_KEY = -891,

	/* Other Error */

	/** Access(permission) denied */
	NET_ERR_ACCESS_DENIED = -799,
	/** Operation is in progress */
	NET_ERR_IN_PROGRESS = -798,
	/** Operation was aborted by client or network*/
	NET_ERR_OPERATION_ABORTED = -797,
	/** Invalid value of API parameter */
	NET_ERR_INVALID_PARAM = -796,
	/** Invalid operation depending on current state */
	NET_ERR_INVALID_OPERATION = -795,

	/** Feature not supported */
	NET_ERR_NOT_SUPPORTED = -794,
	/** TimeOut Error */
	NET_ERR_TIME_OUT = -793,
	/** Network service is not available */
	NET_ERR_NO_SERVICE = -792,
	/** DBus can't find appropriate method */
	NET_ERR_UNKNOWN_METHOD = -791,
	/** Operation is restricted */
	NET_ERR_SECURITY_RESTRICTED = -790,
	/** Already exists */
	NET_ERR_ALREADY_EXISTS = -789,

	/** WiFi driver on/off failed */
	NET_ERR_WIFI_DRIVER_FAILURE = -699,
	NET_ERR_WIFI_DRIVER_LOAD_INPROGRESS = -698,
} net_err_t;

/**
* @enum net_wifi_state_t
* This enum indicates wifi state
*/
typedef enum {
	/** Unknown state */
	WIFI_UNKNOWN = 0x00,
	/** Wi-Fi is Off */
	WIFI_OFF,
	/** Wi-Fi is On(idle/failure) */
	WIFI_ON,
	/** Trying to connect(association/configuration) */
	WIFI_CONNECTING,
	/** Wi-Fi is connected to an AP(ready/online) */
	WIFI_CONNECTED,
	/** Trying to disconnect(connected,
	 * but disconnecting process is on going)
	 */
	WIFI_DISCONNECTING,
} net_wifi_state_t;

/**
*@enum net_wifi_background_scan_mode_t
* This enum indicates background scanning mode.
*/
typedef enum {
	/** scan cycle : 4, 8, 16, ...128s */
	WIFI_BGSCAN_MODE_EXPONENTIAL = 0x00,
	/** scan cycle : 10s */
	WIFI_BGSCAN_MODE_PERIODIC,
} net_wifi_background_scan_mode_t;

/**
*@enum net_wifi_wps_type_t
* This enum indicates WPS type.
*/
typedef enum
{
	/** WPS type is PBC */
	WIFI_WPS_PBC = 0x00,
	/** WPS type is PIN */
	WIFI_WPS_PIN
} net_wifi_wps_type_t;

/**
 * @enum net_state_type_t
 * This enumeration defines the service state type.
 */
typedef enum
{
	/** Not defined */
	NET_STATE_TYPE_UNKNOWN	= 0x00,
	/** Idle state */
	NET_STATE_TYPE_IDLE,
	/** Failure state */
	NET_STATE_TYPE_FAILURE,
	/** Association state */
	NET_STATE_TYPE_ASSOCIATION,
	/** Configuration state */
	NET_STATE_TYPE_CONFIGURATION,
	/** Ready state */
	NET_STATE_TYPE_READY,
	/** Online state */
	NET_STATE_TYPE_ONLINE,
	/** Login state */
	NET_STATE_TYPE_DISCONNECT,
} net_state_type_t;

/*
 * Added:End
 */

/**
 * @enum wlan_security_mode_type_t
 * Below security modes are used in infrastructure and ad-hoc mode
 * For now all EAP security mechanisms are provided only in infrastructure mode
 */
typedef enum
{
	/** Security disabled */
	WLAN_SEC_MODE_NONE = 0x01,
	/** WEP */
	WLAN_SEC_MODE_WEP,
	/** EAP */
	WLAN_SEC_MODE_IEEE8021X,
	/** WPA-PSK */
	WLAN_SEC_MODE_WPA_PSK,
	/** WPA2-PSK */
	WLAN_SEC_MODE_WPA2_PSK,
} wlan_security_mode_type_t;

/**
 * @enum wlan_encryption_mode_type_t
 * Below encryption modes are used in infrastructure and ad-hoc mode
 */
typedef enum
{
	/** Not defined */
	WLAN_ENC_MODE_UNKNOWN = 0x00,
	/** Encryption disabled */
	WLAN_ENC_MODE_NONE,
	/** WEP */
	WLAN_ENC_MODE_WEP,
	/** TKIP */
	WLAN_ENC_MODE_TKIP,
	/** AES */
	WLAN_ENC_MODE_AES,
	/** TKIP and AES are both supported */
	WLAN_ENC_MODE_TKIP_AES_MIXED,
} wlan_encryption_mode_type_t;

/**
 * @enum wlan_connection_mode_type_t
 * WLAN Operation Mode
 * @see net_pm_wlan_profile_info_t
 */
typedef enum
{
	/** auto connection mode */
	NETPM_WLAN_CONNMODE_AUTO = 0x01,
	/** Connection mode Adhoc  */
	NETPM_WLAN_CONNMODE_ADHOC,
	/** Infra connection mode */
	NETPM_WLAN_CONNMODE_INFRA,
} wlan_connection_mode_type_t;


/**
 * @enum wlan_eap_type_t
 * EAP type
 * @see wlan_eap_info_t
 */
typedef enum {
	/** EAP PEAP type */
	WLAN_SEC_EAP_TYPE_PEAP = 0x01,
	/** EAP TLS type */
	WLAN_SEC_EAP_TYPE_TLS,
	/** EAP TTLS type */
	WLAN_SEC_EAP_TYPE_TTLS,
	/** EAP SIM type */
	WLAN_SEC_EAP_TYPE_SIM,
	/** EAP AKA type */
	WLAN_SEC_EAP_TYPE_AKA,
} wlan_eap_type_t;

/**
 * @enum wlan_eap_auth_type_t
 * EAP phase2 authentication type
 * @see wlan_eap_info_t
 */
typedef enum {
	/** EAP phase2 authentication none */
	WLAN_SEC_EAP_AUTH_NONE = 0x01,
	/** EAP phase2 authentication PAP */
	WLAN_SEC_EAP_AUTH_PAP,
	/** EAP phase2 authentication MSCHAP */
	WLAN_SEC_EAP_AUTH_MSCHAP,
	/** EAP phase2 authentication MSCHAPv2 */
	WLAN_SEC_EAP_AUTH_MSCHAPV2,
	/** EAP phase2 authentication GTC */
	WLAN_SEC_EAP_AUTH_GTC,
	/** EAP phase2 authentication MD5 */
	WLAN_SEC_EAP_AUTH_MD5,
} wlan_eap_auth_type_t;

/*======================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
=====================================================++================*/

/**
 * Below structure is used to export essid
 */
typedef struct
{
	/** ESSID */
	char essid[NET_WLAN_ESSID_LEN+1];
} net_essid_t;

/**
 * Below structure is used by WPA-PSK or WPA2-PSK
 * @remark To see the maximum length of PSK passphrase key.
 * @see wlan_auth_info_t
 */
typedef struct
{
	/** key value for WPA-PSK or WPA2-PSK */
	char pskKey[NETPM_WLAN_MAX_PSK_PASSPHRASE_LEN + 1];
} wlan_psk_info_t;

/**
 * Below structure is used by WEP
 * @remark To see the maximum length of WEP key.
 * @see wlan_auth_info_t
 */
typedef struct
{
	/** key value for WEP */
	char wepKey[NETPM_WLAN_MAX_WEP_KEY_LEN + 1];
} wlan_wep_info_t;

/**
 * Below structure is used by EAP
 * @see wlan_auth_info_t
 */
typedef struct
{
	/** User name */
	char username[NETPM_WLAN_USERNAME_LEN+1];
	/** Password */
	char password[NETPM_WLAN_PASSWORD_LEN+1];

	/**
	 * Following fields are mandatory for EAP-TLS,
	 * Optional for EAP-TTLS and EAP-PEAP
	 */
	/**
	 * For EAP-TTLS and EAP-PEAP only ca_cert_filename[]
	 * can also be provided
	 */
	/* Used to authenticate server */
	char ca_cert_filename[NETPM_WLAN_CA_CERT_FILENAME_LEN+1];
	/** client certificate file name */
	char client_cert_filename[NETPM_WLAN_CLIENT_CERT_FILENAME_LEN+1];
	/** private key file name */
	char private_key_filename[NETPM_WLAN_PRIVATE_KEY_FILENAME_LEN+1];
	/** private key password */
	char private_key_passwd[NETPM_WLAN_PRIVATE_KEY_PASSWD_LEN+1];

	/** eap type */
	wlan_eap_type_t eap_type;
	/** eap phase2 authentication type */
	wlan_eap_auth_type_t eap_auth;
} wlan_eap_info_t;

/**
 * At any point of time only one security mechanism is supported
 * @see wlan_security_info_t
 */
typedef union
{
	/** Wep Authentication */
	wlan_wep_info_t wep;
	/** psk Authentication */
	wlan_psk_info_t psk;
	/** eap Authentication */
	wlan_eap_info_t eap;
} wlan_auth_info_t;

/**
 * This is main security information structure
 * @see net_pm_wlan_profile_info_t
 */
typedef struct
{
	/** security mode type */
	wlan_security_mode_type_t sec_mode;
	/** encryption mode type */
	wlan_encryption_mode_type_t enc_mode;
	/** authentication information */
	wlan_auth_info_t authentication;
	/** If WPS is supported, then this property will be set to TRUE */
	char wps_support;
} wlan_security_info_t;

/**
 * This is the structure to connect with WPS network.
 */
typedef struct {
	/** PBC / PIN */
	net_wifi_wps_type_t type;

	/** Optional. This pin is needed when the user input PIN code */
	char pin[NET_WLAN_MAX_WPSPIN_LEN + 1];
} net_wifi_wps_info_t;

/**
 * This is the profile structure to connect hidden WiFi network.
 */
typedef struct {
	/** Basic feature */
	char essid[NET_WLAN_ESSID_LEN + 1];

	/** Infrastructure / ad-hoc / auto mode */
	wlan_connection_mode_type_t wlan_mode;

	/** Security mode and authentication info */
	wlan_security_info_t security_info;
} net_wifi_connection_info_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NETPM_WLAN_H__ */
