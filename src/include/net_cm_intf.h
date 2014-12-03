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

#ifndef __NET_CM_INTF_H__
#define __NET_CM_INTF_H__

#include "net_pm_wlan.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * \addtogroup  common_info
 * \{
*/

/**
* @enum net_cm_network_status_t
* This enum indicates network status
*/
typedef enum {
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
typedef enum {
	NET_STATISTICS_TYPE_LAST_RECEIVED_DATA = 0,  /**< Last received data */
	NET_STATISTICS_TYPE_LAST_SENT_DATA = 1,  /**< Last sent data */
	NET_STATISTICS_TYPE_TOTAL_RECEIVED_DATA = 2,  /**< Total received data */
	NET_STATISTICS_TYPE_TOTAL_SENT_DATA = 3,  /**< Total sent data */
} net_statistics_type_e;

/**
 * \}
 */

/**
 * @enum net_state_type_t
 * This enumeration defines the service state type.
 */
typedef enum {
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

/**
 * This is the profile structure.
 */
typedef struct {
	/** Device Type of the profile */
	net_device_t profile_type;
	/** Profile name */
	char profile_name[NET_PROFILE_NAME_LEN_MAX+1];
	/** Service state */
	net_state_type_t profile_state;
} net_profile_info_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NET_CM_INTF_H__ */
