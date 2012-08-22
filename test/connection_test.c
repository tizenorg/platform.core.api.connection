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

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "assert.h"
#include "glib.h"
#include "net_connection.h"
#include <tizen_error.h>

  
gboolean test_thread(GIOChannel *source, GIOCondition condition, gpointer data);
int test_register_client(void);
int test_deregister_client(void);
int test_get_network_state(void);
int test_get_cellular_state(void);
int test_get_wifi_state(void);
int test_get_current_proxy(void);
int test_get_current_ip(void);
int test_get_call_statistics_info(void);
int test_get_wifi_call_statistics_info(void);


connection_h connection = NULL;

static void test_state_changed_callback(connection_type_e type, void* user_data)
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

static void test_profile_state_callback(connection_profile_h profile, bool is_requested, void* user_data)
{
	connection_profile_state_e state;
	char *profile_name;

	if (connection_profile_get_state(profile, &state) != CONNECTION_ERROR_NONE)
		return;

	if (connection_profile_get_name(profile, &profile_name) != CONNECTION_ERROR_NONE)
		return;

	switch (state) {
	case CONNECTION_PROFILE_STATE_DISCONNECTED:
		printf("[Disconnected] : %s\n", profile_name);
		break;
	case CONNECTION_PROFILE_STATE_ASSOCIATION:
		printf("[Association] : %s\n", profile_name);
		break;
	case CONNECTION_PROFILE_STATE_CONFIGURATION:
		printf("[Configuration] : %s\n", profile_name);
		break;
	case CONNECTION_PROFILE_STATE_CONNECTED:
		printf("[Connected] : %s\n", profile_name);
	}

	g_free(profile_name);
}

static bool test_get_user_selected_profile(connection_profile_h *profile)
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

		connection_profile_get_state(profile_h, &profile_state);

		if (profile_type == CONNECTION_PROFILE_TYPE_WIFI) {
			char *essid;
			connection_profile_get_wifi_essid(profile_h, &essid);
			printf("%d. state:%d, profile name:%s, essid:%s\n",
					profile_count, profile_state, profile_name, (essid)? essid : "");
			g_free(essid);
		} else
			printf("%d. state:%d, profile name : %s\n",
					profile_count, profile_state, profile_name);

		g_free(profile_name);

		if (profile_count >= 100)
			break;

		profile_list[profile_count] = profile_h;
		profile_count++;
	}

	printf("\nInput profile number : \n");
	rv = scanf("%d", &input);

	if (input >= profile_count || input < 0) {
		printf("Wrong number!!\n");
		return false;
	}

	*profile = profile_list[input];

	return true;
}

int test_register_client(void)
{

	int err = connection_create(&connection);

	if (CONNECTION_ERROR_NONE == err) {
		connection_set_type_changed_cb(connection, test_state_changed_callback, NULL);
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

	if (connection != NULL)
		rv = connection_destroy(connection);
	else
		printf("Cannot deregister : Handle is NULL\n");

	if (rv != CONNECTION_ERROR_NONE){
		printf("Client deregistration fail [%d]\n", rv);
		return -1;
	}

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

	connection_get_statistics(CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA, &rv);
	printf("last recv data size [%lld]\n", rv);
	connection_get_statistics(CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA, &rv);
	printf("last sent data size [%lld]\n",rv );
	connection_get_statistics(CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA, &rv);
	printf("total received data size [%lld]\n",rv );
	connection_get_statistics(CONNECTION_TYPE_CELLULAR, CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA, &rv);
	printf("total sent data size [%lld]\n", rv);

	return 1;
}

int test_get_wifi_call_statistics_info(void)
{
	long long rv = 0;

	connection_get_statistics(CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_LAST_RECEIVED_DATA, &rv);
	printf("WiFi last recv data size [%lld]\n", rv);
	connection_get_statistics(CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_LAST_SENT_DATA, &rv);
	printf("WiFi last sent data size [%lld]\n",rv );
	connection_get_statistics(CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_TOTAL_RECEIVED_DATA, &rv);
	printf("WiFi total received data size [%lld]\n",rv );
	connection_get_statistics(CONNECTION_TYPE_WIFI, CONNECTION_STATISTICS_TYPE_TOTAL_SENT_DATA, &rv);
	printf("WiFi total sent data size [%lld]\n", rv);

	return 1;
}

int test_get_profile_list(void)
{
	int rv = 0;
	char *profile_name;
	connection_profile_iterator_h profile_iter;
	connection_profile_h profile_h;

	rv = connection_get_profile_iterator(connection, CONNECTION_ITERATOR_TYPE_REGISTERED, &profile_iter);
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

int test_get_connected_profile_list(void)
{
	int rv = 0;
	char *profile_name;
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
	char *profile_name;
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
	if (test_get_user_selected_profile(&profile) == false)
		return -1;

	if (connection_open_profile(connection, profile) != CONNECTION_ERROR_NONE) {
		printf("Connection open Failed!!\n");
		return -1;
	}

	if (connection_profile_set_state_changed_cb(profile,
			test_profile_state_callback, NULL) != CONNECTION_ERROR_NONE) {
		printf("Set profile callback Failed!!\n");
		return -1;
	}

	return 1;
}

int test_open_cellular_service_type(void)
{
	int input;
	int rv;
	int service_type;
	connection_profile_h profile;
	printf("\nInput profile type(1:Internet, 2:MMS, 3:WAP, 4:Prepaid internet, 5:Prepaid MMS : \n");
	rv = scanf("%d", &input);

	switch (input) {
	case 1:
		service_type = CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET;
		break;
	case 2:
		service_type = CONNECTION_CELLULAR_SERVICE_TYPE_MMS;
		break;
	case 3:
		service_type = CONNECTION_CELLULAR_SERVICE_TYPE_WAP;
		break;
	case 4:
		service_type = CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_INTERNET;
		break;
	case 5:
		service_type = CONNECTION_CELLULAR_SERVICE_TYPE_PREPAID_MMS;
		break;
	default:
		printf("Wrong number!!\n");
		return -1;
	}

	if (connection_open_cellular_service_type(connection, service_type, &profile) != CONNECTION_ERROR_NONE) {
		printf("Connection open Failed!!\n");
		return -1;
	}

	if (connection_profile_set_state_changed_cb(profile,
			test_profile_state_callback, NULL) != CONNECTION_ERROR_NONE) {
		printf("Set profile callback Failed!!\n");
		connection_profile_destroy(profile);
		return -1;
	}

	connection_profile_destroy(profile);

	return 1;
}

int test_close_profile(void)
{
	connection_profile_h profile;
	if (test_get_user_selected_profile(&profile) == false)
		return -1;

	if (connection_close_profile(connection, profile) != CONNECTION_ERROR_NONE) {
		printf("Connection close Failed!!\n");
		return -1;
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
	
	if (rv < 0 || a[0] == '0') exit(1);

	if (*a == '\n' || *a == '\r'){
		printf("\n\n Network Connection API Test App\n\n");
		printf("Options..\n");
		printf("1 	- Create Handle and set callbacks\n");
		printf("2 	- Destroy Handle(unset callbacks automatically)\n");
		printf("3	- Get network state\n");
		printf("4 	- Get cellular state (please insert SIM Card)\n");
		printf("5 	- Get wifi state (please turn on WiFi)\n");
		printf("6	- Get current proxy address \n");
		printf("7 	- Get current Ip address\n");
		printf("8 	- Get cellular data call statistics\n");
		printf("9 	- Get WiFi data call statistics\n");
		printf("a 	- Get Profile list\n");
		printf("b 	- Get Connected Profile list\n");
		printf("c 	- Get Current profile\n");
		printf("d 	- Open connection with profile\n");
		printf("e 	- Open cellular service type\n");
		printf("f 	- Close connection with profile\n");
		printf("0 	- Exit \n");

		printf("ENTER  - Show options menu.......\n");
	}

	switch (a[0]) {
		case '1': {
			rv = test_register_client();
		} break;
		case '2': {
			rv = test_deregister_client();
		} break;
		case '3': {
			rv = test_get_network_state();
		} break;
		case '4': {
			rv = test_get_cellular_state();
		} break;
		case '5': {
			rv = test_get_wifi_state();
		} break;
		case '6': {
			rv = test_get_current_proxy();
		} break;
		case '7': {
			rv = test_get_current_ip();
		} break;
		case '8': {
			rv = test_get_call_statistics_info();
		} break;
		case '9': {
			rv = test_get_wifi_call_statistics_info();
		} break;
		case 'a': {
			rv = test_get_profile_list();
		} break;
		case 'b': {
			rv = test_get_connected_profile_list();
		} break;
		case 'c': {
			rv = test_get_current_profile();
		} break;
		case 'd': {
			rv = test_open_profile();
		} break;
		case 'e': {
			rv = test_open_cellular_service_type();
		} break;
		case 'f': {
			rv = test_close_profile();
		} break;
	}
	return TRUE;
}
