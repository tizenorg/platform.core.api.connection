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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>

#include "net_connection.h"
#include <tizen_error.h>

#define RETURN_FAIL_DESTROY(x) {connection_profile_destroy(x); return -1;}

gboolean test_thread(GIOChannel *source, GIOCondition condition, gpointer data);

connection_h connection = NULL;
static GSList *state_cb_list = NULL;


static bool test_get_user_string(const char *msg, char *buf, int buf_size)
{
	if (msg == NULL || buf == NULL || buf_size < 2)
		return false;

	int rv;
	printf("%s\n", msg);
	memset(buf, 0, buf_size);
	rv = read(0, buf, buf_size - 1);

	if (rv < 0 || buf[0] == '\0' || buf[0] == '\n' || buf[0] == '\r') {
		buf[0] = '\0';
		return false;
	}

	buf[strlen(buf) - 1] = '\0';

	return true;
}

static bool test_get_user_int(const char *msg, int *num)
{
	if (msg == NULL || num == NULL)
		return false;

	int rv;
	char buf[32] = {0,};
	printf("%s\n", msg);
	rv = read(0, buf, 32);

	if (rv < 0 || *buf == 0 || *buf == '\n' || *buf == '\r')
		return false;

	*num = atoi(buf);
	return true;
}

static const char *test_print_state(connection_profile_state_e state)
{
	switch (state) {
	case CONNECTION_PROFILE_STATE_DISCONNECTED:
		return "Disconnected";
	case CONNECTION_PROFILE_STATE_ASSOCIATION:
		return "Association";
	case CONNECTION_PROFILE_STATE_CONFIGURATION:
		return "Configuration";
	case CONNECTION_PROFILE_STATE_CONNECTED:
		return "Connected";
	default:
		return "Unknown";
	}
}

static void test_type_changed_callback(connection_type_e type, void* user_data)
{
	printf("Type changed callback, connection type : %d\n", type);
}

static void test_ip_changed_callback(const char* ipv4_address, const char* ipv6_address, void* user_data)
{
	printf("IP changed callback, IPv4 address : %s, IPv6 address : %s\n",
			ipv4_address, (ipv6_address ? ipv6_address : "NULL"));
}

static void test_proxy_changed_callback(const char* ipv4_address, const char* ipv6_address, void* user_data)
{
	printf("Proxy changed callback, IPv4 address : %s, IPv6 address : %s\n",
			ipv4_address, (ipv6_address ? ipv6_address : "NULL"));
}

static void test_profile_state_callback(connection_profile_state_e state, void* user_data)
{
	char *profile_name;
	connection_profile_h profile = user_data;

	if (profile == NULL)
		return;

	if (connection_profile_get_name(profile, &profile_name) != CONNECTION_ERROR_NONE)
		return;

	printf("[%s] : %s\n", test_print_state(state), profile_name);
	g_free(profile_name);
}

static void test_connection_opened_callback(connection_error_e result, void* user_data)
{
	if (result ==  CONNECTION_ERROR_NONE)
		printf("Connection open Succeeded\n");
	else
		printf("Connection open Failed, err : %d\n", result);
}

static void test_connection_closed_callback(connection_error_e result, void* user_data)
{
	if (result ==  CONNECTION_ERROR_NONE)
		printf("Connection close Succeeded\n");
	else
		printf("Connection close Failed, err : %d\n", result);
}

static void test_connection_set_default_callback(connection_error_e result, void* user_data)
{
	if (result ==  CONNECTION_ERROR_NONE)
		printf("Default profile setting Succeeded\n");
	else
		printf("Default profile setting Failed, err : %d\n", result);
}

static bool test_get_user_selected_profile(connection_profile_h *profile, bool select)
{
	int rv = 0;
	int input = 0;
	char *profile_name;
	connection_profile_type_e profile_type;
	connection_profile_state_e profile_state;
	connection_profile_iterator_h profile_iter;
	connection_profile_h profile_h;

	connection_profile_h profile_list[100] = {0,};
	int profile_count = 0;

	rv = connection_get_profile_iterator(connection, CONNECTION_ITERATOR_TYPE_REGISTERED, &profile_iter);
	if (rv != CONNECTION_ERROR_NONE) {
		printf("Fail to get profile iterator [%d]\n", rv);
		return false;
	}

	while (connection_profile_iterator_has_next(profile_iter)) {
		if (connection_profile_iterator_next(profile_iter, &profile_h) != CONNECTION_ERROR_NONE) {
			printf("Fail to get profile handle\n");
			return false;
		}

		if (connection_profile_get_name(profile_h, &profile_name) != CONNECTION_ERROR_NONE) {
			printf("Fail to get profile name\n");
			return false;
		}

		if (connection_profile_get_type(profile_h, &profile_type) != CONNECTION_ERROR_NONE) {
			printf("Fail to get profile type\n");
			g_free(profile_name);
			return false;
		}

		if (connection_profile_get_state(profile_h, &profile_state) != CONNECTION_ERROR_NONE) {
			printf("Fail to get profile state\n");
			g_free(profile_name);
			return false;
		}

		if (profile_type == CONNECTION_PROFILE_TYPE_WIFI) {
			char *essid;
			connection_profile_get_wifi_essid(profile_h, &essid);
			printf("%d. state:[%s], profile name:%s, essid:%s\n",
				profile_count, test_print_state(profile_state),
				profile_name, (essid)? essid : "");
			g_free(essid);

			profile_list[profile_count] = profile_h;
			profile_count++;
		} else {
			printf("%d. state:[%s], profile name : %s\n",
				profile_count, test_print_state(profile_state), profile_name);

			profile_list[profile_count] = profile_h;
			profile_count++;
		}

		g_free(profile_name);
		if (profile_count >= 100)
			break;
	}

	if (select == false)
		return true;

	if (test_get_user_int("Input profile number(Enter for cancel) :", &input) == false ||
	    input >= profile_count ||
	    input < 0) {
		printf("Wrong number!!\n");
		return false;
	}

	if (profile)
		*profile = profile_list[input];

	return true;
}

static int test_update_cellular_info(connection_profile_h profile)
{
	int rv = 0;
	char input_str1[100] = {0,};
	char input_str2[100] = {0,};
	int input_int = 0;
	int type_val = 0;

	if (test_get_user_int("Input Network Type (internet:1, MMS:2, Prepaid internet:3, "
			"Prepaid MMS:4, Tethering:5, Application:6)"
			" - (Enter for skip) :", &input_int)) {
		switch (input_int) {
		case 1:
			rv = connection_profile_set_cellular_service_type(profile,
					CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET);
			break;
		case 2:
			rv = connection_profile_set_cellular_service_type(profile,
					CONNECTION_CELLULAR_SERVICE_TYPE_MMS);
			break;
		case 3:
			rv = connection_profile_set_cellular_service_type(profile,
					CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_INTERNET);
			break;
		case 4:
			rv = connection_profile_set_cellular_service_type(profile,
					CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_MMS);
			break;
		case 5:
			rv = connection_profile_set_cellular_service_type(profile,
					CONNECTION_CELLULAR_SERVICE_TYPE_TETHERING);
			break;
		case 6:
			rv = connection_profile_set_cellular_service_type(profile,
					CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION);
			break;
		default:
			return -1;
		}

		if (rv != CONNECTION_ERROR_NONE)
			return -1;
	} else
		return -1;

	if (test_get_user_string("Input Apn - (Enter for skip) :", input_str1, 100)) {
		rv = connection_profile_set_cellular_apn(profile, input_str1);
		if (rv != CONNECTION_ERROR_NONE)
			return -1;
	}

	if (test_get_user_string("Input Proxy - (Enter for skip) :", input_str1, 100)) {
		rv = connection_profile_set_proxy_address(profile, CONNECTION_ADDRESS_FAMILY_IPV4, input_str1);
		if (rv != CONNECTION_ERROR_NONE)
			return -1;
	}

	if (test_get_user_string("Input HomeURL - (Enter for skip) :", input_str1, 100)) {
		rv = connection_profile_set_cellular_home_url(profile, input_str1);
		if (rv != CONNECTION_ERROR_NONE)
			return -1;
	}

	if (test_get_user_int("Input AuthType(0:NONE 1:PAP 2:CHAP) - (Enter for skip) :", &input_int)) {
		switch (input_int) {
		case 0:
			rv = connection_profile_set_cellular_auth_info(profile,
					CONNECTION_CELLULAR_AUTH_TYPE_NONE, "", "");
			if (rv != CONNECTION_ERROR_NONE)
				return -1;

			break;
		case 1:
			type_val = CONNECTION_CELLULAR_AUTH_TYPE_PAP;
			/* fall through */
		case 2:
			if (input_int == 2) type_val = CONNECTION_CELLULAR_AUTH_TYPE_CHAP;

			if (test_get_user_string("Input AuthId(Enter for skip) :", input_str1, 100) == false)
				input_str1[0] = 0;
			if (test_get_user_string("Input AuthPwd(Enter for skip) :", input_str2, 100) == false)
				input_str2[0] = 0;

			rv = connection_profile_set_cellular_auth_info(profile, type_val, input_str1, input_str2);
			if (rv != CONNECTION_ERROR_NONE)
				return -1;
		}
	}

	return 1;
}

static int test_update_wifi_info(connection_profile_h profile)
{
	int rv = 0;
	char input_str[100] = {0,};

	if (test_get_user_string("Input Passphrase - (Enter for skip) :", input_str, 100)) {
		rv = connection_profile_set_wifi_passphrase(profile, input_str);
		if (rv != CONNECTION_ERROR_NONE)
			return -1;
	}

	return 1;
}

static int test_update_ip_info(connection_profile_h profile)
{
	int rv = 0;
	char input_str[100] = {0,};

	if (test_get_user_string("Input IP Address - (Enter for skip) :", input_str, 100)) {
		rv = connection_profile_set_ip_address(profile,
							CONNECTION_ADDRESS_FAMILY_IPV4,
							input_str);
		if (rv != CONNECTION_ERROR_NONE)
			return -1;
	}

	if (test_get_user_string("Input Netmask - (Enter for skip) :", input_str, 100)) {
		rv = connection_profile_set_subnet_mask(profile,
							CONNECTION_ADDRESS_FAMILY_IPV4,
							input_str);
		if (rv != CONNECTION_ERROR_NONE)
			return -1;
	}

	if (test_get_user_string("Input Gateway - (Enter for skip) :", input_str, 100)) {
		rv = connection_profile_set_gateway_address(profile,
							CONNECTION_ADDRESS_FAMILY_IPV4,
							input_str);
		if (rv != CONNECTION_ERROR_NONE)
			return -1;
	}

	if (test_get_user_string("Input DNS 1 Address - (Enter for skip) :", input_str, 100)) {
		rv = connection_profile_set_dns_address(profile,
							1,
							CONNECTION_ADDRESS_FAMILY_IPV4,
							input_str);
		if (rv != CONNECTION_ERROR_NONE)
			return -1;

		if (test_get_user_string("Input DNS 2 Address - (Enter for skip) :", input_str, 100)) {
			rv = connection_profile_set_dns_address(profile,
								2,
								CONNECTION_ADDRESS_FAMILY_IPV4,
								input_str);
			if (rv != CONNECTION_ERROR_NONE)
				return -1;
		}
	}

	return 1;
}

static int test_update_proxy_info(connection_profile_h profile)
{
	int rv = 0;
	int input_int = 0;
	char input_str[100] = {0,};

	if (test_get_user_int("Input Proxy Type (1:direct, 2:auto, 3:manual)"
					" - (Enter for skip) :", &input_int)) {
		switch (input_int) {
		case 1:
			rv = connection_profile_set_proxy_type(profile,
					CONNECTION_PROXY_TYPE_DIRECT);

			if (rv != CONNECTION_ERROR_NONE)
				return -1;
			else
				return 1;
		case 2:
			rv = connection_profile_set_proxy_type(profile,
					CONNECTION_PROXY_TYPE_AUTO);
			break;
		case 3:
			rv = connection_profile_set_proxy_type(profile,
					CONNECTION_PROXY_TYPE_MANUAL);
			break;
		default:
			return -1;
		}

		if (rv != CONNECTION_ERROR_NONE)
			return -1;

		if (test_get_user_string("Input auto Proxy URL or Proxy address"
					" - (Enter for skip) :", input_str, 100)) {
			rv = connection_profile_set_proxy_address(profile,
								CONNECTION_ADDRESS_FAMILY_IPV4,
								input_str);
			if (rv != CONNECTION_ERROR_NONE)
				return -1;
		}

	} else
		return -1;

	return 1;
}

static int test_update_network_info(connection_profile_h profile)
{
	int rv = 0;
	int input_int = 0;

	if (test_get_user_int("Input IPv4 Address Type (DHCP:1, Static:2)"
				" - (Enter for skip) :", &input_int)) {
		switch (input_int) {
		case 1:
			rv = connection_profile_set_ip_config_type(profile,
								CONNECTION_ADDRESS_FAMILY_IPV4,
								CONNECTION_IP_CONFIG_TYPE_DYNAMIC);
			break;
		case 2:
			rv = connection_profile_set_ip_config_type(profile,
								CONNECTION_ADDRESS_FAMILY_IPV4,
								CONNECTION_IP_CONFIG_TYPE_STATIC);
			if (rv != CONNECTION_ERROR_NONE)
				return -1;

			if (test_update_ip_info(profile) == -1)
				return -1;

			if (test_update_proxy_info(profile) == -1)
				return -1;
			break;
		default:
			return -1;
		}

		if (rv != CONNECTION_ERROR_NONE)
			return -1;
	} else
		return -1;

	return 1;
}

static void test_print_cellular_info(connection_profile_h profile)
{
	connection_cellular_network_type_e network_type;
	connection_cellular_service_type_e service_type;
	char *apn = NULL;
	connection_cellular_auth_type_e auth_type;
	char *user_name = NULL;
	char *password = NULL;
	char *home_url = NULL;
	bool roaming = false;

	if (connection_profile_get_cellular_network_type(profile, &network_type) != CONNECTION_ERROR_NONE)
		printf("Fail to get cellular network type!\n");
	else
		printf("Cellular network type : %d\n", network_type);

	if (connection_profile_get_cellular_service_type(profile, &service_type) != CONNECTION_ERROR_NONE)
		printf("Fail to get cellular service type!\n");
	else
		printf("Cellular service type : %d\n", service_type);

	if (connection_profile_get_cellular_apn(profile, &apn) != CONNECTION_ERROR_NONE)
		printf("Fail to get cellular APN!\n");
	else {
		printf("Cellular APN : %s\n", apn);
		g_free(apn);
	}

	if (connection_profile_get_cellular_auth_info(profile, &auth_type, &user_name, &password) != CONNECTION_ERROR_NONE)
		printf("Fail to get auth info!\n");
	else {
		printf("Cellular auth type : %d\n", auth_type);
		printf("Cellular user_name : %s\n", user_name);
		printf("Cellular password : %s\n", password);
		g_free(user_name);
		g_free(password);
	}

	if (connection_profile_get_cellular_home_url(profile, &home_url) != CONNECTION_ERROR_NONE)
		printf("Fail to get cellular home url!\n");
	else {
		printf("Cellular home url : %s\n", home_url);
		g_free(home_url);
	}

	if (connection_profile_is_cellular_roaming(profile, &roaming) != CONNECTION_ERROR_NONE)
		printf("Fail to get cellular is roaming!\n");
	else
		printf("Cellular roaming : %s\n", roaming ? "true" : "false");
}

static void test_print_wifi_info(connection_profile_h profile)
{
	char *essid = NULL;
	char *bssid = NULL;
	int rssi = 0;
	int frequency = 0;
	int max_speed = 0;
	connection_wifi_security_type_e security_type;
	connection_wifi_encryption_type_e encryption_type;
	bool pass_required = false;
	bool wps_supported = false;

	if (connection_profile_get_wifi_essid(profile, &essid) != CONNECTION_ERROR_NONE)
		printf("Fail to get Wi-Fi essid!\n");
	else {
		printf("Wi-Fi essid : %s\n", essid);
		g_free(essid);
	}

	if (connection_profile_get_wifi_bssid(profile, &bssid) != CONNECTION_ERROR_NONE)
		printf("Fail to get Wi-Fi bssid!\n");
	else {
		printf("Wi-Fi bssid : %s\n", bssid);
		g_free(bssid);
	}

	if (connection_profile_get_wifi_rssi(profile, &rssi) != CONNECTION_ERROR_NONE)
		printf("Fail to get Wi-Fi rssi!\n");
	else
		printf("Wi-Fi rssi : %d\n", rssi);

	if (connection_profile_get_wifi_frequency(profile, &frequency) != CONNECTION_ERROR_NONE)
		printf("Fail to get Wi-Fi frequency!\n");
	else
		printf("Wi-Fi frequency : %d\n", frequency);

	if (connection_profile_get_wifi_max_speed(profile, &max_speed) != CONNECTION_ERROR_NONE)
		printf("Fail to get Wi-Fi max speed!\n");
	else
		printf("Wi-Fi max speed : %d\n", max_speed);

	if (connection_profile_get_wifi_security_type(profile, &security_type) != CONNECTION_ERROR_NONE)
		printf("Fail to get Wi-Fi security type!\n");
	else
		printf("Wi-Fi security type : %d\n", security_type);

	if (connection_profile_get_wifi_encryption_type(profile, &encryption_type) != CONNECTION_ERROR_NONE)
		printf("Fail to get Wi-Fi encryption type!\n");
	else
		printf("Wi-Fi encryption type : %d\n", encryption_type);

	if (connection_profile_is_wifi_passphrase_required(profile, &pass_required) != CONNECTION_ERROR_NONE)
		printf("Fail to get Wi-Fi passphrase required!\n");
	else
		printf("Wi-Fi passphrase required : %s\n", pass_required ? "true" : "false");

	if (connection_profile_is_wifi_wps_supported(profile, &wps_supported) != CONNECTION_ERROR_NONE)
		printf("Fail to get Wi-Fi wps info\n");
	else
		printf("Wi-Fi wps supported : %s\n", wps_supported ? "true" : "false");
}

static void test_print_network_info(connection_profile_h profile)
{
	char *interface_name = NULL;
	connection_ip_config_type_e ip_type;
	char *ip = NULL;
	char *subnet = NULL;
	char *gateway = NULL;
	char *dns1 = NULL;
	char *dns2 = NULL;
	connection_proxy_type_e proxy_type;
	char *proxy = NULL;

	if (connection_profile_get_network_interface_name(profile, &interface_name) != CONNECTION_ERROR_NONE)
		printf("Fail to get interface name!\n");
	else {
		printf("Interface name : %s\n", interface_name);
		g_free(interface_name);
	}

	if (connection_profile_get_ip_config_type(profile, CONNECTION_ADDRESS_FAMILY_IPV4, &ip_type) != CONNECTION_ERROR_NONE)
		printf("Fail to get ipconfig type!\n");
	else
		printf("Ipconfig type : %d\n", ip_type);

	if (connection_profile_get_ip_address(profile, CONNECTION_ADDRESS_FAMILY_IPV4, &ip) != CONNECTION_ERROR_NONE)
		printf("Fail to get IP address!\n");
	else {
		printf("IP address : %s\n", ip);
		g_free(ip);
	}

	if (connection_profile_get_subnet_mask(profile, CONNECTION_ADDRESS_FAMILY_IPV4, &subnet) != CONNECTION_ERROR_NONE)
		printf("Fail to get subnet mask!\n");
	else {
		printf("Subnet mask : %s\n", subnet);
		g_free(subnet);
	}

	if (connection_profile_get_gateway_address(profile, CONNECTION_ADDRESS_FAMILY_IPV4, &gateway) != CONNECTION_ERROR_NONE)
		printf("Fail to get gateway!\n");
	else {
		printf("Gateway : %s\n", gateway);
		g_free(gateway);
	}

	if (connection_profile_get_dns_address(profile, 1, CONNECTION_ADDRESS_FAMILY_IPV4, &dns1) != CONNECTION_ERROR_NONE)
		printf("Fail to get DNS1!\n");
	else {
		printf("DNS1 : %s\n", dns1);
		g_free(dns1);
	}

	if (connection_profile_get_dns_address(profile, 2, CONNECTION_ADDRESS_FAMILY_IPV4, &dns2) != CONNECTION_ERROR_NONE)
		printf("Fail to get DNS2!\n");
	else {
		printf("DNS2 : %s\n", dns2);
		g_free(dns2);
	}

	if (connection_profile_get_proxy_type(profile, &proxy_type) != CONNECTION_ERROR_NONE)
		printf("Fail to get proxy type!\n");
	else
		printf("Proxy type : %d\n", proxy_type);

	if (connection_profile_get_proxy_address(profile, CONNECTION_ADDRESS_FAMILY_IPV4, &proxy) != CONNECTION_ERROR_NONE)
		printf("Fail to get proxy!\n");
	else {
		printf("Proxy : %s\n", proxy);
		g_free(proxy);
	}
}

int test_register_client(void)
{

	int err = connection_create(&connection);

	if (CONNECTION_ERROR_NONE == err) {
		connection_set_type_changed_cb(connection, test_type_changed_callback, NULL);
		connection_set_ip_address_changed_cb(connection, test_ip_changed_callback, NULL);
		connection_set_proxy_address_changed_cb(connection, test_proxy_changed_callback, NULL);
	} else {
		printf("Client registration failed %d\n", err);
		return -1;
	}

	printf("Client registration success\n");
	return 1;
}

int  test_deregister_client(void)
{
	int rv = 0;
	GSList *list;
	connection_profile_h profile;

	if (connection != NULL)
		rv = connection_destroy(connection);
	else {
		printf("Cannot deregister : Handle is NULL\n");
		rv = CONNECTION_ERROR_INVALID_OPERATION;
	}

	if (rv != CONNECTION_ERROR_NONE){
		printf("Client deregistration fail [%d]\n", rv);
		return -1;
	}

	if (state_cb_list) {
		for (list = state_cb_list; list; list = list->next) {
			profile = list->data;
			connection_profile_destroy(profile);
		}

		g_slist_free(state_cb_list);
		state_cb_list = NULL;
	}

	connection = NULL;
	printf("Client deregistration success\n");

	return 1;
}

int test_get_network_state(void)
{
	int rv = 0;
	connection_type_e net_state;

	rv = connection_get_type(connection, &net_state);

	if (rv != CONNECTION_ERROR_NONE) {
		printf("Fail to get network state [%d]\n", rv);
		return -1;
	}

	printf("Retval = %d network connection state [%d]\n", rv, net_state);

	return 1;
}

int test_get_cellular_state(void)
{
	int rv = 0;
	connection_cellular_state_e cellular_state;

	rv = connection_get_cellular_state(connection, &cellular_state);

	if (rv != CONNECTION_ERROR_NONE) {
		printf("Fail to get Cellular state [%d]\n", rv);
		return -1;
	}

	printf("Retval = %d Cellular state [%d]\n", rv, cellular_state);

	return 1;
}

int test_get_wifi_state(void)
{
	int rv = 0;
	connection_wifi_state_e wifi_state;

	rv = connection_get_wifi_state(connection, &wifi_state);

	if (rv != CONNECTION_ERROR_NONE) {
		printf("Fail to get WiFi state [%d]\n", rv);
		return -1;
	}

	printf("Retval = %d WiFi state [%d]\n", rv, wifi_state);

	return 1;
}

int test_get_current_proxy(void)
{
	char *proxy_addr = NULL;

	connection_get_proxy(connection, CONNECTION_ADDRESS_FAMILY_IPV4, &proxy_addr);

	if (proxy_addr == NULL) {
		printf("Proxy address does not exist\n");
		return -1;
	}

	printf("Current Proxy [%s]\n", proxy_addr);
	g_free(proxy_addr);

	return 1;
}

int test_get_current_ip(void)
{
	char *ip_addr = NULL;

	connection_get_ip_address(connection, CONNECTION_ADDRESS_FAMILY_IPV4, &ip_addr);

	if (ip_addr == NULL) {
		printf("IP address does not exist\n");
		return -1;
	}

	printf("IPv4 address : %s\n", ip_addr);
	g_free(ip_addr);

	return 1;	
}

int test_get_call_statistics_info(void)
{
	long long rv = 0;

	connection_get_statistics(connection, CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA, &rv);
	printf("last recv data size [%lld]\n", rv);
	connection_get_statistics(connection, CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA, &rv);
	printf("last sent data size [%lld]\n",rv );
	connection_get_statistics(connection, CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA, &rv);
	printf("total received data size [%lld]\n",rv );
	connection_get_statistics(connection, CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA, &rv);
	printf("total sent data size [%lld]\n", rv);

	return 1;
}

int test_get_wifi_call_statistics_info(void)
{
	long long rv = 0;

	connection_get_statistics(connection, CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA, &rv);
	printf("WiFi last recv data size [%lld]\n", rv);
	connection_get_statistics(connection, CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA, &rv);
	printf("WiFi last sent data size [%lld]\n",rv );
	connection_get_statistics(connection, CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA, &rv);
	printf("WiFi total received data size [%lld]\n",rv );
	connection_get_statistics(connection, CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA, &rv);
	printf("WiFi total sent data size [%lld]\n", rv);

	return 1;
}

int test_get_profile_list(void)
{
	if (test_get_user_selected_profile(NULL, false) == false)
		return -1;

	return 1;
}

int test_get_connected_profile_list(void)
{
	int rv = 0;
	char *profile_name = NULL;
	connection_profile_iterator_h profile_iter;
	connection_profile_h profile_h;

	rv = connection_get_profile_iterator(connection, CONNECTION_ITERATOR_TYPE_CONNECTED, &profile_iter);
	if (rv != CONNECTION_ERROR_NONE) {
		printf("Fail to get profile iterator [%d]\n", rv);
		return -1;
	}

	while (connection_profile_iterator_has_next(profile_iter)) {
		if (connection_profile_iterator_next(profile_iter, &profile_h) != CONNECTION_ERROR_NONE) {
			printf("Fail to get profile handle\n");
			return -1;
		}

		if (connection_profile_get_name(profile_h, &profile_name) != CONNECTION_ERROR_NONE) {
			printf("Fail to get profile name\n");
			return -1;
		}
		printf("profile name : %s\n", profile_name);
		g_free(profile_name);
	}

	return 1;
}

int test_get_current_profile(void)
{
	int rv = 0;
	char *profile_name = NULL;
	connection_profile_h profile_h;

	rv = connection_get_current_profile(connection, &profile_h);
	if (rv != CONNECTION_ERROR_NONE) {
		printf("Fail to get profile iterator [%d]\n", rv);
		return -1;
	}

	if (connection_profile_get_name(profile_h, &profile_name) != CONNECTION_ERROR_NONE) {
		printf("Fail to get profile name\n");
		return -1;
	}
	printf("profile name : %s\n", profile_name);
	g_free(profile_name);

	connection_profile_destroy(profile_h);

	return 1;
}

int test_open_profile(void)
{
	connection_profile_h profile;

	printf("\n** Choose a profile to open. **\n");

	if (test_get_user_selected_profile(&profile, true) == false)
		return -1;

	if (connection_open_profile(connection, profile, test_connection_opened_callback, NULL) != CONNECTION_ERROR_NONE) {
		printf("Connection open Failed!!\n");
		return -1;
	}

	return 1;
}

int test_get_default_cellular_service_type(void)
{
	int input;
	int rv;
	int service_type;
	connection_profile_h profile;
	char *profile_name = NULL;

	rv = test_get_user_int("Input profile type to get"
			"(1:Internet, 2:MMS, 3:Prepaid internet, 4:Prepaid MMS, 5:Tethering):", &input);

	if (rv == false) {
		printf("Invalid input!!\n");
		return -1;
	}

	switch (input) {
	case 1:
		service_type = CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET;
		break;
	case 2:
		service_type = CONNECTION_CELLULAR_SERVICE_TYPE_MMS;
		break;
	case 3:
		service_type = CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_INTERNET;
		break;
	case 4:
		service_type = CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_MMS;
		break;
	case 5:
		service_type = CONNECTION_CELLULAR_SERVICE_TYPE_TETHERING;
		break;
	default:
		printf("Wrong number!!\n");
		return -1;
	}

	if (connection_get_default_cellular_service_profile(connection, service_type, &profile) != CONNECTION_ERROR_NONE)
		return -1;

	if (connection_profile_get_name(profile, &profile_name) != CONNECTION_ERROR_NONE) {
		printf("Fail to get profile name\n");
		connection_profile_destroy(profile);
		return -1;
	}
	printf("Default profile name : %s\n", profile_name);
	g_free(profile_name);

	connection_profile_destroy(profile);

	return 1;
}

int test_set_default_cellular_service_type(void)
{
	connection_profile_h profile;
	connection_cellular_service_type_e type;
	int input, rv;

	rv = test_get_user_int("Input API type (1:sync, 2:async)", &input);

	if (rv == false || (input != 1 && input != 2)) {
		printf("Invalid input!!\n");
		return -1;
	}

	printf("\n** Choose a profile to set default service(internet or prepaid internet type only). **\n");

	if (test_get_user_selected_profile(&profile, true) == false)
		return -1;

	if (connection_profile_get_cellular_service_type(profile, &type) != CONNECTION_ERROR_NONE) {
		printf("Fail to get cellular service type\n");
		return -1;
	}

	if (input == 1) {
		if (connection_set_default_cellular_service_profile(connection, type, profile) != CONNECTION_ERROR_NONE)
			return -1;
	} else {
		if (connection_set_default_cellular_service_profile_async(connection,
				type, profile, test_connection_set_default_callback, NULL) != CONNECTION_ERROR_NONE)
			return -1;
	}

	return 1;
}

int test_close_profile(void)
{
	connection_profile_h profile;

	printf("\n** Choose a profile to close. **\n");

	if (test_get_user_selected_profile(&profile, true) == false)
		return -1;

	if (connection_close_profile(connection, profile, test_connection_closed_callback, NULL) != CONNECTION_ERROR_NONE) {
		printf("Connection close Failed!!\n");
		return -1;
	}

	return 1;
}

int test_add_profile(void)
{
	int rv = 0;
	connection_profile_h profile;
	char input_str[100] = {0,};

	if (test_get_user_string("Input Keyword - (Enter for skip) :", input_str, 100) == false)
		return -1;

	rv = connection_profile_create(CONNECTION_PROFILE_TYPE_CELLULAR, input_str, &profile);
	if (rv != CONNECTION_ERROR_NONE)
		RETURN_FAIL_DESTROY(profile);

	if (test_update_cellular_info(profile) == -1)
		RETURN_FAIL_DESTROY(profile);

	rv = connection_add_profile(connection, profile);
	if (rv != CONNECTION_ERROR_NONE)
		RETURN_FAIL_DESTROY(profile);

	connection_profile_destroy(profile);
	return 1;
}

int test_remove_profile(void)
{
	connection_profile_h profile;

	printf("\n** Choose a profile to remove. **\n");
	if (test_get_user_selected_profile(&profile, true) == false)
		return -1;

	if (connection_remove_profile(connection, profile) != CONNECTION_ERROR_NONE) {
		printf("Remove profile Failed!!\n");
		return -1;
	}

	return 1;
}

int test_update_profile(void)
{
	int rv = 0;

	connection_profile_type_e prof_type;
	connection_profile_h profile;

	printf("\n** Choose a profile to update. **\n");
	if (test_get_user_selected_profile(&profile, true) == false)
		return -1;

	if (connection_profile_get_type(profile, &prof_type) != CONNECTION_ERROR_NONE)
		return -1;

	switch (prof_type) {
	case CONNECTION_PROFILE_TYPE_CELLULAR:
		if (test_update_cellular_info(profile) == -1)
			return -1;

		break;
	case CONNECTION_PROFILE_TYPE_WIFI:
		if (test_update_wifi_info(profile) == -1)
			return -1;

		if (test_update_network_info(profile) == -1)
			return -1;

		break;
	case CONNECTION_PROFILE_TYPE_ETHERNET:
	case CONNECTION_PROFILE_TYPE_BT:
		printf("Not supported!\n");
		/* fall through */
	default:
		return -1;
	}

	rv = connection_update_profile(connection, profile);
	if (rv != CONNECTION_ERROR_NONE)
		return -1;

	return 1;
}

int test_get_profile_info(void)
{
	connection_profile_type_e prof_type;
	connection_profile_state_e profile_state;
	connection_profile_h profile;
	char *profile_name = NULL;

	printf("\n** Choose a profile to print. **\n");
	if (test_get_user_selected_profile(&profile, true) == false)
		return -1;

	if (connection_profile_get_name(profile, &profile_name) != CONNECTION_ERROR_NONE) {
		printf("Fail to get profile name\n");
		return -1;
	} else {
		printf("Profile Name : %s\n", profile_name);
		g_free(profile_name);
	}

	if (connection_profile_get_state(profile, &profile_state) != CONNECTION_ERROR_NONE) {
		printf("Fail to get profile state\n");
		return -1;
	} else
		printf("Profile State : %s\n", test_print_state(profile_state));


	if (connection_profile_get_type(profile, &prof_type) != CONNECTION_ERROR_NONE)
		return -1;

	switch (prof_type) {
	case CONNECTION_PROFILE_TYPE_CELLULAR:
		printf("Profile Type : Cellular\n");
		test_print_cellular_info(profile);
		break;
	case CONNECTION_PROFILE_TYPE_WIFI:
		printf("Profile Type : Wi-Fi\n");
		test_print_wifi_info(profile);
		break;
	case CONNECTION_PROFILE_TYPE_ETHERNET:
		printf("Profile Type : Ethernet\n");
		break;
	case CONNECTION_PROFILE_TYPE_BT:
		printf("Profile Type : Bluetooth\n");
		break;
	default:
		return -1;
	}

	test_print_network_info(profile);

	return 1;
}

int test_refresh_profile_info(void)
{
	connection_profile_type_e prof_type;
	connection_profile_state_e profile_state;
	connection_profile_h profile;
	char *profile_name = NULL;

	printf("\n** Choose a profile to refresh. **\n");
	if (test_get_user_selected_profile(&profile, true) == false)
		return -1;

	if (connection_profile_refresh(profile) != CONNECTION_ERROR_NONE)
		return -1;

	if (connection_profile_get_name(profile, &profile_name) != CONNECTION_ERROR_NONE) {
		printf("Fail to get profile name\n");
		return -1;
	} else {
		printf("Profile Name : %s\n", profile_name);
		g_free(profile_name);
	}

	if (connection_profile_get_state(profile, &profile_state) != CONNECTION_ERROR_NONE) {
		printf("Fail to get profile state\n");
		return -1;
	} else
		printf("Profile State : %s\n", test_print_state(profile_state));


	if (connection_profile_get_type(profile, &prof_type) != CONNECTION_ERROR_NONE)
		return -1;

	switch (prof_type) {
	case CONNECTION_PROFILE_TYPE_CELLULAR:
		printf("Profile Type : Cellular\n");
		test_print_cellular_info(profile);
		break;
	case CONNECTION_PROFILE_TYPE_WIFI:
		printf("Profile Type : Wi-Fi\n");
		test_print_wifi_info(profile);
		break;
	case CONNECTION_PROFILE_TYPE_ETHERNET:
		printf("Profile Type : Ethernet\n");
		break;
	case CONNECTION_PROFILE_TYPE_BT:
		printf("Profile Type : Bluetooth\n");
		break;
	default:
		return -1;
	}

	test_print_network_info(profile);

	return 1;
}

int test_set_state_changed_callback()
{
	connection_profile_h profile;
	connection_profile_h profile_clone;

	printf("\n** Choose a profile to set callback. **\n");
	if (test_get_user_selected_profile(&profile, true) == false)
		return -1;

	if (connection_profile_clone(&profile_clone, profile) != CONNECTION_ERROR_NONE)
		return -1;

	if (connection_profile_set_state_changed_cb(profile,
			test_profile_state_callback, profile_clone) != CONNECTION_ERROR_NONE) {
		connection_profile_destroy(profile_clone);
		return -1;
	}

	state_cb_list = g_slist_append(state_cb_list, profile_clone);

	return 1;
}

int test_unset_state_changed_callback()
{
	connection_profile_h profile;
	GSList *list;
	char *profile_name = NULL;
	int count = 0;
	int input = 0;

	printf("\n** Choose a profile to unset callback. **\n");
	for (list = state_cb_list; list; list = list->next) {
		profile = list->data;
		if (connection_profile_get_name(profile, &profile_name) != CONNECTION_ERROR_NONE) {
			printf("Fail to get profile name!\n");
			return -1;
		} else {
			printf("%d. %s\n", count, profile_name);
			g_free(profile_name);
		}

		count++;
	}

	if (test_get_user_int("Input profile number(Enter for cancel) :", &input) == false ||
	    input >= count ||
	    input < 0) {
		printf("Wrong number!!\n");
		return -1;
	}

	count = 0;
	for (list = state_cb_list; list; list = list->next) {
		if (count == input) {
			profile = list->data;
			goto unset;
		}

		count++;
	}

	return -1;

unset:
	if (connection_profile_unset_state_changed_cb(profile) != CONNECTION_ERROR_NONE)
		return -1;

	state_cb_list = g_slist_remove(state_cb_list, profile);
	connection_profile_destroy(profile);

	return 1;
}

int test_reset_call_statistics_info(void)
{
	int ret = CONNECTION_ERROR_NONE;

	ret = connection_reset_statistics(connection, CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA);
	printf("reset last recv data size [%d]\n", ret);
	ret = connection_reset_statistics(connection, CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA);
	printf("last sent data size [%d]\n", ret);
	ret = connection_reset_statistics(connection, CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA);
	printf("total received data size [%d]\n", ret);
	ret = connection_reset_statistics(connection, CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA);
	printf("total sent data size [%d]\n", ret);

	return 1;
}

int test_reset_wifi_call_statistics_info(void)
{
	int ret = CONNECTION_ERROR_NONE;

	ret = connection_reset_statistics(connection, CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA);
	printf("WiFi last sent data size [%d]\n", ret);
	ret = connection_reset_statistics(connection, CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA);
	printf("WiFi last recv data size [%d]\n", ret);
	ret = connection_reset_statistics(connection, CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA);
	printf("WiFi total sent data size [%d]\n", ret);
	ret = connection_reset_statistics(connection, CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA);
	printf("WiFi total received data size [%d]\n", ret);

	return 1;
}

int test_add_route(void)
{
	int rv = 0;
	char ip_addr[30];
	char if_name[40];

	if (test_get_user_string("Input IP - (Enter for skip) :", ip_addr, 30) == false)
		return -1;

	if (test_get_user_string("Input Interface name - (Enter for skip) :", if_name, 40) == false)
		return -1;

	rv = connection_add_route(connection, if_name, ip_addr);
	if (rv != CONNECTION_ERROR_NONE) {
		printf("Fail to get add new route [%d]\n", rv);
		return -1;
	}

	return 1;
}

int test_get_bt_state(void)
{
	int rv = 0;
	connection_bt_state_e bt_state;

	rv = connection_get_bt_state(connection, &bt_state);

	if (rv != CONNECTION_ERROR_NONE) {
		printf("Fail to get Bluetooth state [%d]\n", rv);
		return -1;
	}

	printf("Retval = %d, Bluetooth state [%d]\n", rv, bt_state);

	return 1;
}

int test_get_profile_id(void)
{
	connection_profile_h profile;
	char *profile_id;

	printf("\n** Choose a profile to see profile id. **\n");
	if (test_get_user_selected_profile(&profile, true) == false)
		return -1;

	if (connection_profile_get_id(profile, &profile_id) != CONNECTION_ERROR_NONE) {
		printf("Fail to get profile name\n");
		return -1;
	} else {
		printf("Profile id : %s\n", profile_id);
		g_free(profile_id);
	}

	return 1;
}

int main(int argc, char **argv)
{
	
	GMainLoop *mainloop;
	mainloop = g_main_loop_new (NULL, FALSE);

	GIOChannel *channel = g_io_channel_unix_new(0);
	g_io_add_watch(channel, (G_IO_IN|G_IO_ERR|G_IO_HUP|G_IO_NVAL), test_thread,NULL );

	printf("Test Thread created...\n");

	g_main_loop_run (mainloop);

	return 0;
}

gboolean test_thread(GIOChannel *source, GIOCondition condition, gpointer data)
{
	int rv = 0;
	char a[100];
	
	memset(a, '\0', 100);
	printf("Event received from stdin\n");
	
	rv = read(0, a, 100);
	
	if (rv < 0 || a[0] == '0') {
		if (connection != NULL)
			test_deregister_client();

		exit(1);
	}

	if (*a == '\n' || *a == '\r'){
		printf("\n\n Network Connection API Test App\n\n");
		printf("Options..\n");
		printf("1 	- Create Handle and set callbacks\n");
		printf("2 	- Destroy Handle(unset callbacks automatically)\n");
		printf("3 	- Get network state\n");
		printf("4 	- Get cellular state (please insert SIM Card)\n");
		printf("5 	- Get wifi state (please turn on WiFi)\n");
		printf("6 	- Get current proxy address \n");
		printf("7 	- Get current Ip address\n");
		printf("8 	- Get cellular data call statistics\n");
		printf("9 	- Get WiFi data call statistics\n");
		printf("a 	- Get Profile list\n");
		printf("b 	- Get Connected Profile list\n");
		printf("c 	- Get Current profile\n");
		printf("d 	- Open connection with profile\n");
		printf("e 	- Get default cellular service by type\n");
		printf("f 	- Set default cellular service by type\n");
		printf("g 	- Close connection with profile\n");
		printf("h 	- Add profile(Cellular only)\n");
		printf("i 	- Remove profile(Cellular:delete, WiFi:forgot)\n");
		printf("j 	- Update profile\n");
		printf("k 	- Get profile info\n");
		printf("l 	- Refresh profile info\n");
		printf("m 	- Set state changed callback\n");
		printf("n 	- Unset state changed callback\n");
		printf("o 	- Reset cellular data call statistics\n");
		printf("p 	- Reset WiFi data call statistics\n");
		printf("q 	- Add new route\n");
		printf("r 	- Get Bluetooth state\n");
		printf("s 	- Get profile id\n");
		printf("0 	- Exit \n");
		printf("ENTER  - Show options menu.......\n");
	}

	switch (a[0]) {
	case '1':
		rv = test_register_client();
		break;
	case '2':
		rv = test_deregister_client();
		break;
	case '3':
		rv = test_get_network_state();
		break;
	case '4':
		rv = test_get_cellular_state();
		break;
	case '5':
		rv = test_get_wifi_state();
		break;
	case '6':
		rv = test_get_current_proxy();
		break;
	case '7':
		rv = test_get_current_ip();
		break;
	case '8':
		rv = test_get_call_statistics_info();
		break;
	case '9':
		rv = test_get_wifi_call_statistics_info();
		break;
	case 'a':
		rv = test_get_profile_list();
		break;
	case 'b':
		rv = test_get_connected_profile_list();
		break;
	case 'c':
		rv = test_get_current_profile();
		break;
	case 'd':
		rv = test_open_profile();
		break;
	case 'e':
		rv = test_get_default_cellular_service_type();
		break;
	case 'f':
		rv = test_set_default_cellular_service_type();
		break;
	case 'g':
		rv = test_close_profile();
		break;
	case 'h':
		rv = test_add_profile();
		break;
	case 'i':
		rv = test_remove_profile();
		break;
	case 'j':
		rv = test_update_profile();
		break;
	case 'k':
		rv = test_get_profile_info();
		break;
	case 'l':
		rv = test_refresh_profile_info();
		break;
	case 'm':
		rv = test_set_state_changed_callback();
		break;
	case 'n':
		rv = test_unset_state_changed_callback();
		break;
	case 'o':
		rv = test_reset_call_statistics_info();
		break;
	case 'p':
		rv = test_reset_wifi_call_statistics_info();
		break;
	case 'q':
		rv = test_add_route();
		break;
	case 'r':
		rv = test_get_bt_state();
		break;
	case 's':
		rv = test_get_profile_id();
		break;
	}

	if (rv == 1)
		printf("Operation succeeded!\n");
	else
		printf("Operation failed!\n");

	return TRUE;
}
