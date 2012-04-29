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
#include <net_connection.h>
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

static void test_state_changed_callback(connection_network_state_e state, void* user_data)
{
	printf("State changed callback, state : %d\n", state);
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

int test_register_client(void)
{

	int err = connection_create(&connection);

	if (CONNECTION_ERROR_NONE == err) {
		connection_set_network_state_changed_cb(connection, test_state_changed_callback, NULL);
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
	connection_network_state_e net_state;

	rv = connection_get_network_state(connection, &net_state);

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
	int rv = 0;

	connection_get_last_received_data_size(connection, &rv);
	printf("last recv data size [%d]\n", rv);
	connection_get_last_sent_data_size(connection, &rv);
	printf("last sent data size [%d]\n",rv );
	connection_get_total_received_data_size (connection, &rv);
	printf("total received data size [%d]\n",rv );
	connection_get_total_sent_data_size (connection, &rv);
	printf("total sent data size [%d]\n", rv);

	return 1;
}

int test_get_wifi_call_statistics_info(void)
{
	int rv = 0;

	connection_get_wifi_last_received_data_size(connection, &rv);
	printf("WiFi last recv data size [%d]\n", rv);
	connection_get_wifi_last_sent_data_size(connection, &rv);
	printf("WiFi last sent data size [%d]\n",rv );
	connection_get_wifi_total_received_data_size (connection, &rv);
	printf("WiFi total received data size [%d]\n",rv );
	connection_get_wifi_total_sent_data_size (connection, &rv);
	printf("WiFi total sent data size [%d]\n", rv);

	return 1;
}

int main(int argc, char **argv){
	
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
	}
	return TRUE;
}
