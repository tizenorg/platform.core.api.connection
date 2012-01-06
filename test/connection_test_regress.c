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
int test_get_network_status(int);
int test_set_default_profile_id(void);
void test_print_device_info(void);
connection_h handle = NULL;
int state_machine = 0;
char a[2];
void net_callback(const connection_network_param_e param, void *user_data)
{
	char *ptr = NULL;
	connection_get_ip_address(handle, &ptr);
	printf("Param Name : %d IP Address = %s\n", param,  ptr);
	free(ptr);
}

int test_register_client(void){

	int err = connection_create(&handle);

	if(err==CONNECTION_ERROR_NONE) //set callbacks
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
	a[0]='1';
	mainloop = g_main_loop_new (NULL, FALSE);
	g_idle_add((GSourceFunc)test_thread,NULL);
	printf("Test Thread created...\n");
	g_main_loop_run (mainloop);
	return 0;
}

gboolean test_thread(GIOChannel *source, GIOCondition condition, gpointer data)
{
	int rv=0;
	
	switch (a[0])
	{
		case '1':{
			rv = test_register_client();
			a[0] = '4';
		}break;
		case '2':{
			rv = test_deregister_client();
			a[0] = '1';
		}break;
		case '4':{
			if(state_machine==2)
			{
				state_machine = 3;
				a[0] = '7';
			}
			else if(state_machine==0)
			{
				rv = test_open_connection();
				if(rv<CONNECTION_ERROR_NONE)
				{
					state_machine = 0;
					a[0] = '2';
				}
				else
				{
					state_machine = 1;
				}
			}
		}break;
		case '6':{
			if(state_machine==5){
				state_machine = 0;
				a[0] = '2';
			}
			else if(state_machine==3)
			{
				rv = test_close_connection();
				if(rv<CONNECTION_ERROR_NONE)
				{
					state_machine = 0;
					a[0] = '2';
				}
				else
				{
					state_machine = 4;
				}
			}
		}break;		
		case '7':{
			rv = test_get_current_proxy();
			a[0] = '8';
		}break;
		case '8':{
			rv = test_is_connected();
			a[0] = '9';
		}break;
		case '9':{
			rv = test_get_current_device_info();
			a[0] = 'e';
		}break;
		case 'e':{
			rv = test_get_call_statistics_info();
			a[0] = 'f';
		}break;
		case 'f':{
			rv = test_get_network_status(CONNECTION_MOBILE_TYPE);
			a[0] = 'g';
		}break;
		case 'g':{
			rv = test_get_network_status(CONNECTION_WIFI_TYPE);
			a[0] = '6';
		}break;
	}
	return TRUE;
}
