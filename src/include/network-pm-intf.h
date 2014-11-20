/*
 * Network Client Library
 *
 * Copyright 2011-2013 Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __NETWORK_PM_INTF_H__
#define __NETWORK_PM_INTF_H__

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef DEPRECATED
#define DEPRECATED __attribute__((deprecated))
#endif

/**
 * @file network-pm-intf.h
 * @brief This file defines the interface of Profile Manager with the application/Connection Manager.
*/

/**
 * \addtogroup  profile_managing
 * \{
*/

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/**
 * @enum net_pdp_type_t
 * This enumeration defines the pdp protocol type.
 */
typedef enum
{
	/** Not defined */
	NET_PDP_TYPE_NONE	= 0x00,
	/** PDP-GPRS type */
	NET_PDP_TYPE_GPRS,
	/** PDP-EDGE type */
	NET_PDP_TYPE_EDGE,
	/** PDP-UMTS type */
	NET_PDP_TYPE_UMTS,
} net_pdp_type_t;

/**
 * @enum net_state_type_t
 * This enumeration defines the service state type.
 */

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/


/**
 * Profile data structures: Used between Application and PM Plug-in Interface
 */
typedef struct
{
	/** Specifies a protocol type */
	net_pdp_type_t  ProtocolType;
	/** Specifies a service type(Internet, MMS, WAP, etc...) */
	net_service_type_t ServiceType;
	/** Network Access Point Name */
	char            Apn[NET_PDP_APN_LEN_MAX+1];
	/** Authentication info of the PDP profile */
	net_auth_info_t	AuthInfo;
	/** Browser Home URL or MMS server URL */
	char            HomeURL[NET_HOME_URL_LEN_MAX+1];
	/** Sim Info Mcc */
	char Mcc[NET_SIM_INFO_LEN+1];
	/** Sim Info Mnc */
	char Mnc[NET_SIM_INFO_LEN+1];
	/** Indicates whether the use of static IP or not */
	char IsStatic;

	/** Indicates Roaming mode */
	char Roaming;
	/** This will be deprecated */
	char SetupRequired;

	char Keyword[NET_PDP_APN_LEN_MAX+1];
	char Hidden;
	char Editable;
	char DefaultConn;

	/** network information */
	net_dev_info_t net_info;
} net_pdp_profile_info_t;

/**
 * AP Profile information
 */
typedef struct
{
	/** ESSID */
	char essid[NET_WLAN_ESSID_LEN+1];
	/** Basic service set identification */
	char bssid[NET_MAX_MAC_ADDR_LEN+1];
	/**  Strength : between 0 and 100 */
	unsigned char Strength;
	/** Frequency band(MHz) */
	unsigned int frequency;
	/** Maximum speed of the line(bps) */
	unsigned int max_rate;
	/** If a passphrase has been set already or if no
		passphrase is needed, then this property will
		be set to FALSE. */
	char PassphraseRequired;
	/** Infrastucture / ad-hoc / auto mode */
	wlan_connection_mode_type_t wlan_mode;
	/** Security mode and authentication info */
	wlan_security_info_t security_info;

	/** network information */
	net_dev_info_t net_info;
} net_wifi_profile_info_t;

/**
 * Profile data structures: Ethernet Interface
 */
typedef struct
{
	/** network information */
	net_dev_info_t net_info;
} net_eth_profile_info_t;

/**
 * Profile data structures: Bluetooth Interface
 */
typedef struct
{
	/** network information */
	net_dev_info_t net_info;
} net_bt_profile_info_t;

/**
 * Specific profile information related to each technology type
 */
typedef union
{
	/** PDP Profile Information */
	net_pdp_profile_info_t       Pdp;
	/** Wifi Profile Information */
	net_wifi_profile_info_t      Wlan;
	/** Ethernet Profile Information */
	net_eth_profile_info_t       Ethernet;
	/** Bluetooth Profile Information */
	net_bt_profile_info_t        Bluetooth;
} net_specific_profile_info_t;

/**
 * This is the profile structure exposed to applications.
 */
typedef struct
{
	/** Device Type of the profile */
	net_device_t  	profile_type;
	/** Profile name */
	char	ProfileName[NET_PROFILE_NAME_LEN_MAX+1];
	/** Specific profile information */
	net_specific_profile_info_t ProfileInfo;
	/** Service state */
	net_state_type_t        ProfileState;
	/** Favourite flag */
	char Favourite;
} net_profile_info_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
