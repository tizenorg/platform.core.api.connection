/*
 * Copyright (c) 2011-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __TIZEN_NETWORK_CONNECTION_EXTENSION_H__
#define __TIZEN_NETWORK_CONNECTION_EXTENSION_H__

#include <tizen.h>
#include "connection_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file connection_extension.h
 */
 
/**
 * @brief Enumeration for DNS configuration type.
 * @since_tizen 3.0
*/
typedef enum
{
    CONNECTION_DNS_CONFIG_TYPE_NONE = 0,  /**< Not defined */
    CONNECTION_DNS_CONFIG_TYPE_STATIC  = 1,  /**< Manual DNS configuration */
    CONNECTION_DNS_CONFIG_TYPE_DYNAMIC = 2,  /**< Config DNS using DHCP client*/
} connection_dns_config_type_e;

/**
* @addtogroup CAPI_NETWORK_CONNECTION_PROFILE_MODULE
* @{
*/

/**
* @brief Gets the DNS config type.
* @since_tizen @if TV 3.0 @endif
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[out] type  The type of DNS config
* @return @c 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
* @retval #CONNECTION_ERROR_NOT_SUPPORTED     Not Supported
*/
int connection_profile_get_dns_config_type(connection_profile_h profile,
		connection_address_family_e address_family,
		connection_dns_config_type_e* type);

/**
* @brief Sets the DNS config type.
* @since_tizen @if TV 3.0 @endif
* @param[in] profile  The handle of profile
* @param[in] address_family  The address family
* @param[in] type  The type of DNS config
* @return @c 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
* @retval #CONNECTION_ERROR_NOT_SUPPORTED     Not Supported
* @see connection_update_profile()
*/
int connection_profile_set_dns_config_type(connection_profile_h profile,
		connection_address_family_e address_family,
		connection_dns_config_type_e type);

/**
* @brief Gets the Prefix Length of IPv6.
* @since_tizen @if TV 3.0 @endif
* @param[in] profile  The handle of profile
* @param[out] prefix_len  The prefix length
* @return @c 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
* @retval #CONNECTION_ERROR_NOT_SUPPORTED     Not Supported
*/
int connection_profile_get_prefix_length(connection_profile_h profile,
					 unsigned char* prefix_length);

/**
* @brief Sets the Prefix Length of IPv6.
* @since_tizen @if TV 3.0 @endif
* @param[in] profile  The handle of profile
* @param[in] prefix_length  The prefix len. If you set this value to NULL, then the existing value will be deleted.
* @return @c 0 on success, otherwise negative error value.
* @retval #CONNECTION_ERROR_NONE  Successful
* @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter
* @retval #CONNECTION_ERROR_OPERATION_FAILED  Operation failed
* @retval #CONNECTION_ERROR_NOT_SUPPORTED     Not Supported
* @see connection_update_profile()
*/
int connection_profile_set_prefix_length(connection_profile_h profile,
					 unsigned char prefix_length);

/**
* @}
*/


#ifdef __cplusplus
}

#endif

#endif /* __TIZEN_NETWORK_CONNECTION_EXTENSION_H__ */
