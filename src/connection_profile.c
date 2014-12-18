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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <glib.h>
#include <vconf/vconf.h>
#include "net_connection_private.h"

#define HTTP_PROXY "http_proxy"

/*
static net_dev_info_t* __profile_get_net_info(net_profile_info_t *profile_info)
{
	switch (profile_info->profile_type) {
	case NET_DEVICE_CELLULAR:
		return &profile_info->ProfileInfo.Pdp.net_info;
	case NET_DEVICE_WIFI:
		return &profile_info->ProfileInfo.Wlan.net_info;
	case NET_DEVICE_ETHERNET:
		return &profile_info->ProfileInfo.Ethernet.net_info;
	case NET_DEVICE_BLUETOOTH:
		return &profile_info->ProfileInfo.Bluetooth.net_info;
	case NET_DEVICE_DEFAULT:
	case NET_DEVICE_USB:
	case NET_DEVICE_UNKNOWN:
	case NET_DEVICE_MAX:
	default:
		return NULL;
	}

	return NULL;
}

static char* __profile_convert_ip_to_string(net_addr_t *ip_addr)
{
	unsigned char *ipaddr = (unsigned char *)&ip_addr->Data.Ipv4.s_addr;

	char *ipstr = g_try_malloc0(16);
	if (ipstr == NULL)
		return NULL;

	snprintf(ipstr, 16, "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);

	return ipstr;
}

static void __profile_init_cellular_profile(net_profile_info_t *profile_info, const char *keyword)
{
	profile_info->profile_type = NET_DEVICE_CELLULAR;
	profile_info->profile_state = NET_STATE_TYPE_IDLE;
	profile_info->ProfileInfo.Pdp.net_info.IpConfigType = NET_IP_CONFIG_TYPE_OFF;
	profile_info->ProfileInfo.Pdp.net_info.ProxyMethod = NET_PROXY_TYPE_DIRECT;
	g_strlcpy(profile_info->ProfileInfo.Pdp.Keyword, keyword, NET_PDP_APN_LEN_MAX);
}

static void __profile_init_wifi_profile(net_profile_info_t *profile_info)
{
	profile_info->profile_type = NET_DEVICE_WIFI;
	profile_info->profile_state = NET_STATE_TYPE_IDLE;
	profile_info->ProfileInfo.Wlan.net_info.IpConfigType = NET_IP_CONFIG_TYPE_OFF;
	profile_info->ProfileInfo.Wlan.net_info.ProxyMethod = NET_PROXY_TYPE_DIRECT;
	profile_info->ProfileInfo.Wlan.wlan_mode = NETPM_WLAN_CONNMODE_AUTO;
	profile_info->ProfileInfo.Wlan.security_info.sec_mode = WLAN_SEC_MODE_NONE;
	profile_info->ProfileInfo.Wlan.security_info.enc_mode = WLAN_ENC_MODE_NONE;
}

static const char* __profile_get_ethernet_proxy(void)
{
	char *proxy;

	proxy = getenv(HTTP_PROXY);

	if(proxy == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Fail to get system proxy");
		return NULL;
	}

	return proxy;
}
*/

connection_cellular_service_type_e _profile_convert_to_connection_cellular_service_type(net_service_type_t svc_type)
{
	switch (svc_type) {
	case NET_SERVICE_INTERNET:
		return CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET;
	case NET_SERVICE_MMS:
		return CONNECTION_CELLULAR_SERVICE_TYPE_MMS;
	case NET_SERVICE_PREPAID_INTERNET:
		return CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_INTERNET;
	case NET_SERVICE_PREPAID_MMS:
		return CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_MMS;
	case NET_SERVICE_TETHERING:
		return CONNECTION_CELLULAR_SERVICE_TYPE_TETHERING;
	case NET_SERVICE_APPLICATION:
		return CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION;
	default:
		return CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;
	}
}

connection_profile_state_e _profile_convert_to_cp_state(net_state_type_t state)
{
	connection_profile_state_e cp_state;

	switch (state) {
	case NET_STATE_TYPE_ONLINE:
	case NET_STATE_TYPE_READY:
		cp_state = CONNECTION_PROFILE_STATE_CONNECTED;
		break;
	case NET_STATE_TYPE_IDLE:
	case NET_STATE_TYPE_FAILURE:
	case NET_STATE_TYPE_DISCONNECT:
		cp_state = CONNECTION_PROFILE_STATE_DISCONNECTED;
		break;
	case NET_STATE_TYPE_ASSOCIATION:
		cp_state = CONNECTION_PROFILE_STATE_ASSOCIATION;
		break;
	case NET_STATE_TYPE_CONFIGURATION:
		cp_state = CONNECTION_PROFILE_STATE_CONFIGURATION;
		break;
	default:
		cp_state = -1;
	}

	return cp_state;
}

net_service_type_t _connection_profile_convert_to_libnet_cellular_service_type(connection_cellular_service_type_e svc_type)
{
	switch (svc_type) {
	case CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET:
		return NET_SERVICE_INTERNET;
	case CONNECTION_CELLULAR_SERVICE_TYPE_MMS:
		return NET_SERVICE_MMS;
	case CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_INTERNET:
		return NET_SERVICE_PREPAID_INTERNET;
	case CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_MMS:
		return NET_SERVICE_PREPAID_MMS;
	case CONNECTION_CELLULAR_SERVICE_TYPE_TETHERING:
		return NET_SERVICE_TETHERING;
	case CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION:
		return NET_SERVICE_APPLICATION;
	default:
		return NET_SERVICE_UNKNOWN;
	}
}

net_state_type_t _connection_profile_convert_to_net_state(connection_profile_state_e state)
{
	net_state_type_t libnet_state = NET_STATE_TYPE_UNKNOWN;

	switch (state) {
	case CONNECTION_PROFILE_STATE_CONNECTED:
		libnet_state = NET_STATE_TYPE_ONLINE;
		break;
	case CONNECTION_PROFILE_STATE_DISCONNECTED:
		libnet_state = NET_STATE_TYPE_IDLE;
		break;
	case CONNECTION_PROFILE_STATE_ASSOCIATION:
		libnet_state = NET_STATE_TYPE_ASSOCIATION;
		break;
	case CONNECTION_PROFILE_STATE_CONFIGURATION:
		libnet_state = NET_STATE_TYPE_CONFIGURATION;
		break;
	}

	return libnet_state;
}


/* Connection profile ********************************************************/
EXPORT_API int connection_profile_create(connection_profile_type_e type, const char* keyword, connection_profile_h* profile)
{
	if ((type != CONNECTION_PROFILE_TYPE_CELLULAR &&
	     type != CONNECTION_PROFILE_TYPE_WIFI) || profile == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = g_try_malloc0(sizeof(net_profile_info_t));
	if (profile_info == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	switch (type) {
	case CONNECTION_PROFILE_TYPE_CELLULAR:
		if (keyword == NULL) {
			CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
			g_free(profile_info);
			return CONNECTION_ERROR_INVALID_PARAMETER;
		}
		__profile_init_cellular_profile(profile_info, keyword);
		break;
	case CONNECTION_PROFILE_TYPE_WIFI:
		__profile_init_wifi_profile(profile_info);
		break;
	default:
		break;
	}

	*profile = (connection_profile_h)profile_info;
	_connection_libnet_add_to_profile_list(*profile);
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_destroy(connection_profile_h profile)
{
	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	_connection_libnet_remove_from_profile_list(profile);

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_clone(connection_profile_h* cloned_profile, connection_profile_h origin_profile)
{
	if (!(_connection_libnet_check_profile_validity(origin_profile)) || cloned_profile == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	*cloned_profile = g_try_malloc0(sizeof(net_profile_info_t));
	if (*cloned_profile == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	memcpy(*cloned_profile, origin_profile, sizeof(net_profile_info_t));
	_connection_libnet_add_to_profile_list(*cloned_profile);
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_id(connection_profile_h profile, char** profile_id)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || profile_id == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	char *prof_id = strrchr(profile_info->profile_name, '/');
	if (prof_id == NULL)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	prof_id++;
	*profile_id = g_strdup(prof_id);

	if (*profile_id == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_name(connection_profile_h profile, char** profile_name)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || profile_name == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	struct connman_service *service =
				_connection_libnet_get_service_h(profile);
	if (service == NULL)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	net_profile_info_t *profile_info = profile;

	switch (profile_info->profile_type) {
	case NET_DEVICE_CELLULAR: /*TODO */
	case NET_DEVICE_WIFI:
		*profile_name = g_strdup(connman_service_get_name(service));
		break;
	case NET_DEVICE_ETHERNET: {
		const struct service_ethernet *ethernet =
					connman_service_get_ethernet(service);
		*profile_name = g_strdup(ethernet->interface);
	} break;
	case NET_DEVICE_BLUETOOTH: {
		char *bt_name = strrchr(profile_info->profile_name, '/');
		if (bt_name == NULL)
			return CONNECTION_ERROR_INVALID_PARAMETER;

		bt_name++;
		*profile_name = g_strdup(bt_name);
	} break;
	default:
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (*profile_name == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_type(connection_profile_h profile, connection_profile_type_e* type)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || type == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	net_profile_info_t *profile_info = profile;

	switch (profile_info->profile_type) {
	case NET_DEVICE_CELLULAR:
		*type = CONNECTION_PROFILE_TYPE_CELLULAR;
		break;
	case NET_DEVICE_WIFI:
		*type = CONNECTION_PROFILE_TYPE_WIFI;
		break;
	case NET_DEVICE_ETHERNET:
		*type = CONNECTION_PROFILE_TYPE_ETHERNET;
		break;
	case NET_DEVICE_BLUETOOTH:
		*type = CONNECTION_PROFILE_TYPE_BT;
		break;
	default:
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid profile type\n");
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_network_interface_name(connection_profile_h profile, char** interface_name)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || interface_name == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	*interface_name = g_strdup(net_info->DevName);
	if (*interface_name == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_refresh(connection_profile_h profile)
{
	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	net_profile_info_t profile_info_local;
	/*
	net_profile_info_t *profile_info = profile;

	TODO:
	if (net_get_profile_info(profile_info->profile_name, &profile_info_local) != NET_ERR_NONE) {
		CONNECTION_LOG(CONNECTION_ERROR, "Error!!! net_get_profile_info() failed\n");
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	 */

	memcpy(profile, &profile_info_local, sizeof(net_profile_info_t));

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_state(connection_profile_h profile, connection_profile_state_e* state)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || state == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	net_profile_info_t *profile_info = profile;
	*state = _profile_convert_to_cp_state(profile_info->profile_state);
	if (*state < 0)
		return CONNECTION_ERROR_OPERATION_FAILED;

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_ip_config_type(connection_profile_h profile,
		connection_address_family_e address_family, connection_ip_config_type_e* type)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6) ||
	    type == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	switch (net_info->IpConfigType) {
	case NET_IP_CONFIG_TYPE_STATIC:
		*type = CONNECTION_IP_CONFIG_TYPE_STATIC;
		break;
	case NET_IP_CONFIG_TYPE_DYNAMIC:
		*type = CONNECTION_IP_CONFIG_TYPE_DYNAMIC;
		break;
	case NET_IP_CONFIG_TYPE_AUTO_IP:
		*type = CONNECTION_IP_CONFIG_TYPE_AUTO;
		break;
	case NET_IP_CONFIG_TYPE_FIXED:
		*type = CONNECTION_IP_CONFIG_TYPE_FIXED;
		break;
	case NET_IP_CONFIG_TYPE_OFF:
		*type = CONNECTION_IP_CONFIG_TYPE_NONE;
		break;
	default:
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_ip_address(connection_profile_h profile,
		connection_address_family_e address_family, char** ip_address)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6) ||
	    ip_address == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	*ip_address = __profile_convert_ip_to_string(&net_info->IpAddr);
	if (*ip_address == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_subnet_mask(connection_profile_h profile,
		connection_address_family_e address_family, char** subnet_mask)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6) ||
	    subnet_mask == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	*subnet_mask = __profile_convert_ip_to_string(&net_info->SubnetMask);
	if (*subnet_mask == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_gateway_address(connection_profile_h profile,
		connection_address_family_e address_family, char** gateway_address)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6) ||
	    gateway_address == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	*gateway_address = __profile_convert_ip_to_string(&net_info->GatewayAddr);
	if (*gateway_address == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_dns_address(connection_profile_h profile, int order,
		connection_address_family_e address_family, char** dns_address)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6) ||
	    dns_address == NULL ||
	    order <= 0 ||
	    order > NET_DNS_ADDR_MAX) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	*dns_address = __profile_convert_ip_to_string(&net_info->DnsAddr[order-1]);
	if (*dns_address == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_proxy_type(connection_profile_h profile, connection_proxy_type_e* type)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || type == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	const char *proxy;
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (profile_info->profile_type == NET_DEVICE_ETHERNET) {
		proxy = __profile_get_ethernet_proxy();
		if (proxy == NULL)
			*type = CONNECTION_PROXY_TYPE_DIRECT;
		else
			*type = CONNECTION_PROXY_TYPE_MANUAL;

		return CONNECTION_ERROR_NONE;
	}

	switch (net_info->ProxyMethod) {
	case NET_PROXY_TYPE_DIRECT:
		*type = CONNECTION_PROXY_TYPE_DIRECT;
		break;
	case NET_PROXY_TYPE_AUTO:
		*type = CONNECTION_PROXY_TYPE_AUTO;
		break;
	case NET_PROXY_TYPE_MANUAL:
		*type = CONNECTION_PROXY_TYPE_MANUAL;
		break;
	case NET_PROXY_TYPE_UNKNOWN:
	default:
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_proxy_address(connection_profile_h profile,
		connection_address_family_e address_family, char** proxy_address)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6) ||
	     proxy_address == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	const char *proxy;
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	if (profile_info->profile_type == NET_DEVICE_ETHERNET) {
		proxy = __profile_get_ethernet_proxy();
		if (proxy == NULL)
			return CONNECTION_ERROR_OPERATION_FAILED;

		*proxy_address = g_strdup(proxy);
	} else
		*proxy_address = g_strdup(net_info->ProxyAddr);

	if (*proxy_address == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_ip_config_type(connection_profile_h profile,
		connection_address_family_e address_family, connection_ip_config_type_e type)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6)) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	switch (type) {
	case CONNECTION_IP_CONFIG_TYPE_STATIC:
		net_info->IpConfigType = NET_IP_CONFIG_TYPE_STATIC;
		net_info->IpAddr.Data.Ipv4.s_addr = 0;
		net_info->SubnetMask.Data.Ipv4.s_addr = 0;
		net_info->GatewayAddr.Data.Ipv4.s_addr = 0;
		break;
	case CONNECTION_IP_CONFIG_TYPE_DYNAMIC:
		net_info->IpConfigType = NET_IP_CONFIG_TYPE_DYNAMIC;
		break;
	case CONNECTION_IP_CONFIG_TYPE_AUTO:
		net_info->IpConfigType = NET_IP_CONFIG_TYPE_AUTO_IP;
		break;
	case CONNECTION_IP_CONFIG_TYPE_FIXED:
		net_info->IpConfigType = NET_IP_CONFIG_TYPE_FIXED;
		break;
	case CONNECTION_IP_CONFIG_TYPE_NONE:
		net_info->IpConfigType = NET_IP_CONFIG_TYPE_OFF;
		break;
	default:
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_ip_address(connection_profile_h profile,
		connection_address_family_e address_family, const char* ip_address)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6)) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	if (ip_address == NULL)
		net_info->IpAddr.Data.Ipv4.s_addr = 0;
	else if (inet_aton(ip_address, &(net_info->IpAddr.Data.Ipv4)) == 0)
		return CONNECTION_ERROR_INVALID_PARAMETER;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_subnet_mask(connection_profile_h profile,
		connection_address_family_e address_family, const char* subnet_mask)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6)) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	if (subnet_mask == NULL)
		net_info->SubnetMask.Data.Ipv4.s_addr = 0;
	else if (inet_aton(subnet_mask, &(net_info->SubnetMask.Data.Ipv4)) == 0)
		return CONNECTION_ERROR_INVALID_PARAMETER;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_gateway_address(connection_profile_h profile,
		connection_address_family_e address_family, const char* gateway_address)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6)) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	if (gateway_address == NULL)
		net_info->GatewayAddr.Data.Ipv4.s_addr = 0;
	else if (inet_aton(gateway_address, &(net_info->GatewayAddr.Data.Ipv4)) == 0)
		return CONNECTION_ERROR_INVALID_PARAMETER;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_dns_address(connection_profile_h profile, int order,
		connection_address_family_e address_family, const char* dns_address)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6) ||
	    order <= 0 ||
	    order > NET_DNS_ADDR_MAX) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	if (dns_address == NULL)
		net_info->DnsAddr[order-1].Data.Ipv4.s_addr = 0;
	else if (inet_aton(dns_address, &(net_info->DnsAddr[order-1].Data.Ipv4)) == 0)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	if (net_info->DnsCount < order)
		net_info->DnsCount = order;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_proxy_type(connection_profile_h profile, connection_proxy_type_e type)
{
	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	switch (type) {
	case CONNECTION_PROXY_TYPE_DIRECT:
		net_info->ProxyMethod = NET_PROXY_TYPE_DIRECT;
		break;
	case CONNECTION_PROXY_TYPE_AUTO:
		net_info->ProxyMethod = NET_PROXY_TYPE_AUTO;
		break;
	case CONNECTION_PROXY_TYPE_MANUAL:
		net_info->ProxyMethod = NET_PROXY_TYPE_MANUAL;
		break;
	default:
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_proxy_address(connection_profile_h profile,
		connection_address_family_e address_family, const char* proxy_address)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    (address_family != CONNECTION_ADDRESS_FAMILY_IPV4 &&
	     address_family != CONNECTION_ADDRESS_FAMILY_IPV6)) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;
	net_dev_info_t *net_info = __profile_get_net_info(profile_info);
	if (net_info == NULL)
		return CONNECTION_ERROR_OPERATION_FAILED;

	if (address_family == CONNECTION_ADDRESS_FAMILY_IPV6)
		return CONNECTION_ERROR_ADDRESS_FAMILY_NOT_SUPPORTED;

	if (proxy_address == NULL)
		net_info->ProxyAddr[0] = '\0';
	else
		g_strlcpy(net_info->ProxyAddr, proxy_address, NET_PROXY_LEN_MAX);
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_state_changed_cb(connection_profile_h profile,
		connection_profile_state_changed_cb callback, void* user_data)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || callback == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	if (_connection_libnet_add_to_profile_cb_list(profile, callback, user_data))
		return CONNECTION_ERROR_NONE;

	return CONNECTION_ERROR_OPERATION_FAILED;
}

EXPORT_API int connection_profile_unset_state_changed_cb(connection_profile_h profile)
{
	if (!(_connection_libnet_check_profile_cb_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	_connection_libnet_remove_from_profile_cb_list(profile);

	return CONNECTION_ERROR_NONE;
}


/* Wi-Fi profile *************************************************************/
EXPORT_API int connection_profile_get_wifi_essid(connection_profile_h profile, char** essid)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || essid == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_WIFI)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	*essid = g_strdup(profile_info->ProfileInfo.Wlan.essid);
	if (*essid == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_wifi_bssid(connection_profile_h profile, char** bssid)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || bssid == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_WIFI)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	*bssid = g_strdup(profile_info->ProfileInfo.Wlan.bssid);
	if (*bssid == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_wifi_rssi(connection_profile_h profile, int* rssi)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || rssi == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_WIFI)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	*rssi = (int)profile_info->ProfileInfo.Wlan.Strength;
	*/

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_wifi_frequency(connection_profile_h profile, int* frequency)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || frequency == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_WIFI)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	*frequency = (int)profile_info->ProfileInfo.Wlan.frequency;
	*/

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_wifi_max_speed(connection_profile_h profile, int* max_speed)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || max_speed == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_WIFI)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	*max_speed = (int)profile_info->ProfileInfo.Wlan.max_rate;
	*/

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_wifi_security_type(connection_profile_h profile, connection_wifi_security_type_e* type)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || type == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_WIFI)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	switch (profile_info->ProfileInfo.Wlan.security_info.sec_mode) {
	case WLAN_SEC_MODE_NONE:
		*type = CONNECTION_WIFI_SECURITY_TYPE_NONE;
		break;
	case WLAN_SEC_MODE_WEP:
		*type = CONNECTION_WIFI_SECURITY_TYPE_WEP;
		break;
	case WLAN_SEC_MODE_IEEE8021X:
		*type = CONNECTION_WIFI_SECURITY_TYPE_EAP;
		break;
	case WLAN_SEC_MODE_WPA_PSK:
		*type = CONNECTION_WIFI_SECURITY_TYPE_WPA_PSK;
		break;
	case WLAN_SEC_MODE_WPA2_PSK:
		*type = CONNECTION_WIFI_SECURITY_TYPE_WPA2_PSK;
		break;
	default:
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_wifi_encryption_type(connection_profile_h profile, connection_wifi_encryption_type_e* type)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || type == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_WIFI)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	switch (profile_info->ProfileInfo.Wlan.security_info.enc_mode) {
	case WLAN_ENC_MODE_NONE:
		*type = CONNECTION_WIFI_ENCRYPTION_TYPE_NONE;
		break;
	case WLAN_ENC_MODE_WEP:
		*type = CONNECTION_WIFI_ENCRYPTION_TYPE_WEP;
		break;
	case WLAN_ENC_MODE_TKIP:
		*type = CONNECTION_WIFI_ENCRYPTION_TYPE_TKIP;
		break;
	case WLAN_ENC_MODE_AES:
		*type = CONNECTION_WIFI_ENCRYPTION_TYPE_AES;
		break;
	case WLAN_ENC_MODE_TKIP_AES_MIXED:
		*type = CONNECTION_WIFI_ENCRYPTION_TYPE_TKIP_AES_MIXED;
		break;
	default:
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_is_wifi_passphrase_required(connection_profile_h profile, bool* required)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || required == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_WIFI)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	if (profile_info->favourite) {
		*required = false;
		return CONNECTION_ERROR_NONE;
	}

	switch (profile_info->ProfileInfo.Wlan.security_info.sec_mode) {
	case WLAN_SEC_MODE_NONE:
		*required = false;
		break;
	case WLAN_SEC_MODE_WEP:
	case WLAN_SEC_MODE_IEEE8021X:
	case WLAN_SEC_MODE_WPA_PSK:
	case WLAN_SEC_MODE_WPA2_PSK:
		*required = true;
		break;
	default:
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_wifi_passphrase(connection_profile_h profile, const char* passphrase)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || passphrase == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_WIFI)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	g_strlcpy(profile_info->ProfileInfo.Wlan.security_info.authentication.psk.pskKey,
						passphrase, NETPM_WLAN_MAX_PSK_PASSPHRASE_LEN);
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_is_wifi_wps_supported(connection_profile_h profile, bool* supported)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || supported == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_WIFI)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	if (profile_info->ProfileInfo.Wlan.security_info.wps_support)
		*supported = true;
	else
		*supported = false;
	*/

	return CONNECTION_ERROR_NONE;
}


/* Cellular profile **********************************************************/
EXPORT_API int connection_profile_get_cellular_network_type(connection_profile_h profile, connection_cellular_network_type_e* type)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || type == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	int network_type;
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	if (vconf_get_int(VCONFKEY_TELEPHONY_SVC_ACT, &network_type)) {
		CONNECTION_LOG(CONNECTION_ERROR, "vconf_get_int Failed\n");
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	CONNECTION_LOG(CONNECTION_INFO, "Cellular network type = %d\n", network_type);

	switch (network_type) {
	case VCONFKEY_TELEPHONY_SVC_ACT_NONE:
		*type = CONNECTION_CELLULAR_NETWORK_TYPE_UNKNOWN;
		break;
	case VCONFKEY_TELEPHONY_SVC_ACT_GPRS:
		*type = CONNECTION_CELLULAR_NETWORK_TYPE_GPRS;
		break;
	case VCONFKEY_TELEPHONY_SVC_ACT_EGPRS:
		*type = CONNECTION_CELLULAR_NETWORK_TYPE_EDGE;
		break;
	case VCONFKEY_TELEPHONY_SVC_ACT_UMTS:
		*type = CONNECTION_CELLULAR_NETWORK_TYPE_UMTS;
		break;
	default:
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_cellular_service_type(connection_profile_h profile,
						connection_cellular_service_type_e* type)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || type == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid profile type Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	*type = _profile_convert_to_connection_cellular_service_type(profile_info->ProfileInfo.Pdp.ServiceType);

	if (*type == CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN) {
		CONNECTION_LOG(CONNECTION_ERROR, "Invalid service type Passed\n");
		return CONNECTION_ERROR_OPERATION_FAILED;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_cellular_apn(connection_profile_h profile, char** apn)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || apn == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	*apn = g_strdup(profile_info->ProfileInfo.Pdp.Apn);
	if (*apn == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_cellular_auth_info(connection_profile_h profile,
		connection_cellular_auth_type_e* type, char** user_name, char** password)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    type == NULL || user_name == NULL || password == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	switch (profile_info->ProfileInfo.Pdp.AuthInfo.AuthType) {
	case NET_PDP_AUTH_NONE:
		*type = CONNECTION_CELLULAR_AUTH_TYPE_NONE;
		break;
	case NET_PDP_AUTH_PAP:
		*type = CONNECTION_CELLULAR_AUTH_TYPE_PAP;
		break;
	case NET_PDP_AUTH_CHAP:
		*type = CONNECTION_CELLULAR_AUTH_TYPE_CHAP;
		break;
	default:
		return CONNECTION_ERROR_OPERATION_FAILED;
	}

	*user_name = g_strdup(profile_info->ProfileInfo.Pdp.AuthInfo.UserName);
	if (*user_name == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;

	*password = g_strdup(profile_info->ProfileInfo.Pdp.AuthInfo.Password);
	if (*password == NULL) {
		g_free(*user_name);
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_get_cellular_home_url(connection_profile_h profile, char** home_url)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || home_url == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	*home_url = g_strdup(profile_info->ProfileInfo.Pdp.HomeURL);
	if (*home_url == NULL)
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_is_cellular_roaming(connection_profile_h profile, bool* is_roaming)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || is_roaming == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	if (profile_info->ProfileInfo.Pdp.Roaming)
		*is_roaming = true;
	else
		*is_roaming = false;
	*/

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_cellular_service_type(connection_profile_h profile,
		connection_cellular_service_type_e service_type)
{
	if (!(_connection_libnet_check_profile_validity(profile))) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	switch (service_type) {
	case CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET:
		profile_info->ProfileInfo.Pdp.ServiceType = NET_SERVICE_INTERNET;
		break;
	case CONNECTION_CELLULAR_SERVICE_TYPE_MMS:
		profile_info->ProfileInfo.Pdp.ServiceType = NET_SERVICE_MMS;
		break;
	case CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_INTERNET:
		profile_info->ProfileInfo.Pdp.ServiceType = NET_SERVICE_PREPAID_INTERNET;
		break;
	case CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_MMS:
		profile_info->ProfileInfo.Pdp.ServiceType = NET_SERVICE_PREPAID_MMS;
		break;
	case CONNECTION_CELLULAR_SERVICE_TYPE_TETHERING:
		profile_info->ProfileInfo.Pdp.ServiceType = NET_SERVICE_TETHERING;
		break;
	case CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION:
		profile_info->ProfileInfo.Pdp.ServiceType = NET_SERVICE_APPLICATION;
		break;
	case CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN:
	default:
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_cellular_apn(connection_profile_h profile, const char* apn)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || apn == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	g_strlcpy(profile_info->ProfileInfo.Pdp.Apn, apn, NET_PDP_APN_LEN_MAX+1);
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_cellular_auth_info(connection_profile_h profile,
		connection_cellular_auth_type_e type, const char* user_name, const char* password)
{
	if (!(_connection_libnet_check_profile_validity(profile)) ||
	    user_name == NULL || password == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	switch (type) {
	case CONNECTION_CELLULAR_AUTH_TYPE_NONE:
		profile_info->ProfileInfo.Pdp.AuthInfo.AuthType = NET_PDP_AUTH_NONE;
		break;
	case CONNECTION_CELLULAR_AUTH_TYPE_PAP:
		profile_info->ProfileInfo.Pdp.AuthInfo.AuthType = NET_PDP_AUTH_PAP;
		break;
	case CONNECTION_CELLULAR_AUTH_TYPE_CHAP:
		profile_info->ProfileInfo.Pdp.AuthInfo.AuthType = NET_PDP_AUTH_CHAP;
		break;
	default:
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	g_strlcpy(profile_info->ProfileInfo.Pdp.AuthInfo.UserName, user_name, NET_PDP_AUTH_USERNAME_LEN_MAX+1);
	g_strlcpy(profile_info->ProfileInfo.Pdp.AuthInfo.Password, password, NET_PDP_AUTH_PASSWORD_LEN_MAX+1);
	 */

	return CONNECTION_ERROR_NONE;
}

EXPORT_API int connection_profile_set_cellular_home_url(connection_profile_h profile, const char* home_url)
{
	if (!(_connection_libnet_check_profile_validity(profile)) || home_url == NULL) {
		CONNECTION_LOG(CONNECTION_ERROR, "Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	/*
	net_profile_info_t *profile_info = profile;

	if (profile_info->profile_type != NET_DEVICE_CELLULAR)
		return CONNECTION_ERROR_INVALID_PARAMETER;

	g_strlcpy(profile_info->ProfileInfo.Pdp.HomeURL, home_url, NET_HOME_URL_LEN_MAX);
	 */

	return CONNECTION_ERROR_NONE;
}
