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
int test_open_connection(void);
int test_close_connection(void);
int test_get_current_proxy(void);
int test_get_current_device_info(void);
int test_get_call_statistics_info(void);
int test_get_wifi_call_statistics_info(void);
int test_get_network_status(int);
int test_set_default_profile_id(void);
void test_print_device_info(void);
connection_h handle = NULL;

void net_callback(const connection_network_param_e param, void *user_data)
{
	char *ptr = NULL;
	connection_get_ip_address(handle, &ptr);
	printf("Param Name : %d IP Address = %s\n", param,  ptr);
	free(ptr);
}

int test_register_client(void){

	int err = connection_create(&handle);

	if(CONNECTION_ERROR_NONE==err) //set callbacks
	{
		if(handle==NULL)
			printf("OOOOOPPPPSSS\n");
		connection_set_cb(handle, net_callback, NULL);
	}
	else
	{
		printf("Client registration failed %d\n", err);
		return -1;
	}

	printf("Client registration success\n");
	return 1;
}
 
int  test_deregister_client(void){
	int rv = 0;

	if(handle!=NULL)
		rv = connection_destroy(handle);
	else
		printf("Cannot deregister : Handle is NULL\n");

	if (rv != CONNECTION_ERROR_NONE){
		printf("Client deregistration fail [%d]\n", rv);
		return -1;
	}

	printf("Client deregistration success\n");
	return 1;
}

int test_open_connection(void){
	int rv =0;

	//rv = connection_open(handle, CONNECTION_DEFAULT_TYPE);

	if (rv != CONNECTION_ERROR_NONE){
		printf("Fail to call open connection [%d]\n", rv);
		return -1;
	}

	printf("open connection api is called [%d]\n", rv);
	return 1;
}

int test_close_connection(void){
	int rv =0;

	//rv = connection_close(handle);

	if (rv  != CONNECTION_ERROR_NONE){
		printf("Fail to call close connection [%d]\n", rv);
		return -1;
	}

	printf("close connection api is called\n");
	return 1;
}


int test_is_connected(void){
	int rv =0;

	rv = connection_is_connected();

	if(rv != 1){
		printf("There is no active connection\n");
		return -1;
	}

	printf("Connected\n");
	printf("is connected api is called\n");
	return 1;
}

int test_get_current_proxy(void){
	int rv=0;
	char *proxy_addr=NULL;

	rv = connection_is_connected();

	if(rv != 1){
		printf("There is no active connection\n");
		return -1;
	}

	rv = connection_get_proxy(handle, &proxy_addr);

	if(proxy_addr == NULL){
		printf("Proxy address does not exist\n");
		return -1;
	}

	printf("Current Proxy [%s]\n", proxy_addr);
	free(proxy_addr);
	
	printf("get current proxy api is called\n");
	return 1;
}

int test_get_current_device_info(void){
	int rv=0;
	rv = connection_is_connected();

	if(rv != 1){
		printf("There is no active connection\n");
		return -1;
	}

	test_print_device_info();

	printf("get current device information api is called\n");
	
	return 1;	
}

int test_get_call_statistics_info(void){
	int rv=0;

	connection_get_last_datacall_duration(handle, &rv);
	printf("last received data pkg size [%d]\n", rv);
	connection_get_last_received_data_size(handle, &rv);
	printf("last recv data pkg size [%d]\n", rv);
	connection_get_last_sent_data_size(handle, &rv);
	printf("last sent data pkg size [%d]\n",rv );
	connection_get_total_datacall_duration(handle, &rv);
	printf("total data pkg size [%d]\n", rv);
	connection_get_total_received_data_size (handle, &rv);
	printf("total received data pkg size [%d]\n",rv );
	connection_get_total_sent_data_size (handle, &rv);
	printf("total sent data pkg size [%d]\n", rv);


	printf("get call statistics information is called\n");

	return 1;
}

int test_get_wifi_call_statistics_info(void){
	int rv=0;

	connection_get_wifi_last_datacall_duration(handle, &rv);
	printf("WiFi last received data pkg size [%d]\n", rv);
	connection_get_wifi_last_received_data_size(handle, &rv);
	printf("WiFi last recv data pkg size [%d]\n", rv);
	connection_get_wifi_last_sent_data_size(handle, &rv);
	printf("WiFi last sent data pkg size [%d]\n",rv );
	connection_get_wifi_total_datacall_duration(handle, &rv);
	printf("WiFi total data pkg size [%d]\n", rv);
	connection_get_wifi_total_received_data_size (handle, &rv);
	printf("WiFi total received data pkg size [%d]\n",rv );
	connection_get_wifi_total_sent_data_size (handle, &rv);
	printf("WiFi total sent data pkg size [%d]\n", rv);


	printf("get WiFi call statistics information is called\n");

	return 1;
}

int test_get_network_status(int type){
	int rv=0;
	connection_network_status_e net_status;

	memset(&net_status, 0 , sizeof(connection_network_status_e) );

    if(type == 1)
	    rv = connection_get_network_status(CONNECTION_MOBILE_TYPE, &net_status);
    if(type==2)
	    rv = connection_get_network_status(CONNECTION_WIFI_TYPE, &net_status);

	if(rv != CONNECTION_ERROR_NONE){
		printf("Fail to get network status [%d]\n",rv );
		return -1;
	}

	printf("Retval = %d network connection status  [%d]\n", rv, net_status);

	
	return 1;		
}


void test_print_device_info(void){
	char *temp=NULL;
	if(!connection_get_ip_address(handle, &temp))
	{
		if(temp!=NULL)
		{
			printf("IPv4 address : %s\n", temp);
			free(temp);
		}
		else
			printf("IPv4 address Not Provided by Network\n");
	}
	
	return;
}

int main(int argc, char **argv){
	
	GMainLoop *mainloop;
	mainloop = g_main_loop_new (NULL, FALSE);

	GIOChannel *channel = g_io_channel_unix_new(0);
	g_io_add_watch(channel, (G_IO_IN|G_IO_ERR|G_IO_HUP|G_IO_NVAL), test_thread,NULL );

	printf("Test Thread created...\n");

	test_register_client();
	
	g_main_loop_run (mainloop);

	return 0;
}

gboolean test_thread(GIOChannel *source, GIOCondition condition, gpointer data)
{
	int rv=0;
	char a[100];
	
	memset(a, '\0', 100);
	printf("Event received from stdin \n");
	
	rv = read(0, a, 100);
	
	if (rv < 0 || a[0] == '0') exit(1);

	if (*a == '\n' || *a == '\r'){
		printf("\n\n Network Framework Test App\n\n");
		printf("Options..\n");
		printf("1 	- Create Handle\n");
		printf("2 	- Destroy Handle\n");
		printf("3	- Get current proxy address \n");		
		printf("4	- Is connected\n");
		printf("5 	- Get current network device information\n");

		printf("6 	- Get cellular data call statistics\n");
		printf("7 	- Get WiFi data call statistics\n");
		printf("8 	- Get cellular status (please insert SIM Card)\n");
		printf("9 	- Get wifi status (please turn on WiFi)\n");

		printf("0 	- Exit \n");

		printf("ENTER  - Show options menu.......\n");
	}

	switch (a[0])
	{
		case '1':{
			rv = test_register_client();
		}break;
		case '2':{
			rv = test_deregister_client();
		}break;
		case '3':{
			rv = test_get_current_proxy();
		}break;
		case '4':{
			rv = test_is_connected();
		}break;
		case '5':{
			rv = test_get_current_device_info();
		}break;
		case '6':{
			rv = test_get_call_statistics_info();
		}break;
		case '7':{
			rv = test_get_wifi_call_statistics_info();
		}break;
		case '8':{
			rv = test_get_network_status(CONNECTION_MOBILE_TYPE);
		}break;
		case '9':{
			rv = test_get_network_status(CONNECTION_WIFI_TYPE);
		}break;
	}
	return TRUE;
}
