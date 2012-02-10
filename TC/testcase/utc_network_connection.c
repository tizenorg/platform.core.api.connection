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



#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>
#include "utc_network_connection.h"

connection_h handle = NULL;
static void startup(), cleanup();
void (*tet_startup) () = startup;
void (*tet_cleanup) () = cleanup;
int rv = 0;
static connection_network_status_e conn_status = CONNECTION_STATUS_UNAVAILABLE;

struct tet_testlist tet_testlist[] = { 
					{utc_connection_create_positive,1},
					{utc_connection_create_negative,2},
					{utc_connection_destroy_positive,3},
					{utc_connection_destroy_negative,4},
					{utc_connection_set_cb_positive,5},
					{utc_connection_set_cb_negative,6},
					{utc_connection_get_ip_address_positive,7},
					{utc_connection_get_ip_address_negative,8},
					{utc_connection_get_proxy_positive,9},
					{utc_connection_get_proxy_negative,10},
					{utc_connection_get_network_status_positive_1,11},
					{utc_connection_get_network_status_positive_2,12},
					{utc_connection_get_network_status_positive_3,13},
					{utc_connection_get_network_status_negative_1,14},
					{utc_connection_get_network_status_negative_2,15},
					{utc_connection_is_connected_positive,16},
					{utc_connection_unset_cb_positive,17},
					{utc_connection_unset_cb_negative,18},
					{utc_connection_get_last_sent_positive,19},
					{utc_connection_get_last_sent_negative,20},
					{utc_connection_get_last_recv_positive,21},
					{utc_connection_get_last_recv_negative,22},
					{utc_connection_get_total_sent_positive,23},
					{utc_connection_get_total_sent_negative,24},
					{utc_connection_get_total_recv_positive,25},
					{utc_connection_get_total_recv_negative,26},
					{utc_connection_get_total_duration_positive,27},
					{utc_connection_get_total_duration_negative,28},
					{utc_connection_get_wifi_last_datacall_duration_positive,29},
					{utc_connection_get_wifi_last_datacall_duration_negative,30},
					{utc_connection_get_wifi_last_received_data_size_positive,31},
					{utc_connection_get_wifi_last_received_data_size_negative,32},
					{utc_connection_get_wifi_last_sent_data_size_positive,33},
					{utc_connection_get_wifi_last_sent_data_size_negative,34},
					{utc_connection_get_wifi_total_datacall_duration_positive,35},
					{utc_connection_get_wifi_total_datacall_duration_negative,36},
					{utc_connection_get_wifi_total_received_data_size_positive,37},
					{utc_connection_get_wifi_total_received_data_size_negative,38},
					{utc_connection_get_wifi_total_sent_data_size_positive,39},
					{utc_connection_get_wifi_total_sent_data_size_negative,40},
					{utc_connection_get_last_datacall_duration_positive,41},
					{utc_connection_get_last_datacall_duration_negative,42},
					{TET_NULLFP, 0} 
				     };



//callbacks
void net_callback(const connection_network_param_e param, void *user_data)
{
	tet_printf("== CM callback == %d\n", param);
}

int create_handle()
{
	int rv=0;
	rv = connection_create(&handle);
	if(rv != CONNECTION_ERROR_NONE){
		tet_printf("Connection Create  Error [%d]\n", rv);
		return 1;
	}
	tet_printf("Connection Create Success [%d]\n", rv);
	return 0;

}

int set_callbacks()
{
	int rv = 0;
	rv = connection_set_cb(handle, net_callback, NULL);
	if(rv != CONNECTION_ERROR_NONE){
		tet_printf("Connection Set Callback is not called! Error [%d]\n", rv);
		return 1;
	}
	tet_printf("Connection Set Callback! Success [%d]\n", rv);
	return 0;
}

int get_api_preconditions()
{
	if(create_handle()){
		tet_result(TET_FAIL);
		return 1;
	}
	if(set_callbacks()){
		tet_result(TET_FAIL);
		return 1;
	}
	return 0;
}

void startup()
{
	tet_printf(" TC startup");
	return;
}

void cleanup()
{
	tet_printf(" TC cleanup");
	return;
}

void utc_connection_create_positive(void)
{
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}
	tet_result(TET_PASS);
	return;
}

void utc_connection_create_negative(void)
{
	int rv=0;
	rv = connection_create(NULL);
	if(rv != CONNECTION_ERROR_NONE){
		tet_printf("Connection Create Must Fail! Success [%d]\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Create Must Fail ! Error [%d]\n", rv);
	tet_result(TET_FAIL);
	return;
}


void utc_connection_set_cb_positive(void)
{
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}
	if(set_callbacks()){
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}

void utc_connection_set_cb_negative(void)
{
	int rv=0;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}
	rv = connection_set_cb(NULL, NULL, NULL);
	if(rv != CONNECTION_ERROR_NONE){
		tet_printf("Connection Set Callback! Must Fail : Success [%d]\n", rv);
		tet_result(TET_PASS);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Set Callback Must Fail! Error [%d]\n", rv);
	connection_destroy(handle);
	tet_result(TET_FAIL);
	return;
}

void utc_connection_get_ip_address_positive(void)
{
	int rv=0;

	if(get_api_preconditions())
	{
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}

	char *ip_address=NULL;
	rv = connection_get_ip_address(handle, &ip_address);
	if(rv != CONNECTION_ERROR_NONE){
		tet_printf("Connection Get IP Address Failed [%d]\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	if(ip_address !=NULL)
		tet_printf("Connection Get IP Address Success : %s\n", ip_address);
	else
		tet_printf("Connection Get IP Address  Success : No IP Address from Network\n");
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}

void utc_connection_get_ip_address_negative(void)
{
	if(get_api_preconditions())
	{
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}
	char *ip_address=NULL;
	rv = connection_get_ip_address(NULL, &ip_address);
	if(rv != CONNECTION_ERROR_NONE){
		tet_printf("Connection Get IP Address Must Fail [%d]\n", rv);
		tet_result(TET_PASS);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get IP Address Must Fail Success : %d\n",rv); 
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}

void utc_connection_get_proxy_positive(void)
{
	int rv=0;
	if(get_api_preconditions())
	{
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}

	char *proxy=NULL;
	rv = connection_get_proxy(handle, &proxy);
	if(rv != CONNECTION_ERROR_NONE){
		tet_printf("Connection Get Proxy Failed [%d]\n", rv);
		tet_result(TET_FAIL);
		return;
	}
	if(proxy!=NULL)
		tet_printf("Connection Get Proxy Success : %s\n", proxy);
	else
		tet_printf("Connection Get Proxy Success : Network Did Not Give Proxy\n");

	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_proxy_negative(void)
{
	int rv=0;
	if(get_api_preconditions())
	{
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}

	char *proxy=NULL;
	rv = connection_get_proxy(NULL, &proxy);
	if(rv != CONNECTION_ERROR_NONE){
		tet_printf("Connection Get Proxy Must Fail [%d]\n", rv);
		tet_result(TET_PASS);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get Proxy Must Fail : %d\n", rv);
	tet_result(TET_FAIL);
	connection_destroy(handle);
	return;
}

void utc_connection_get_network_status_positive_1(void)
{
	int rv=0;
	if(get_api_preconditions())
	{
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}

	connection_network_status_e  status;

	rv = connection_get_network_status(CONNECTION_WIFI_TYPE, &status);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get Status Fail : %d\n", rv);
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}
	tet_printf("Connection Get Status Success : %d Status = %d\n", rv, status);
	connection_destroy(handle);
	tet_result(TET_PASS);

	if(status == CONNECTION_STATUS_AVAILABLE)
		conn_status = status;

	return;

}

void utc_connection_get_network_status_positive_2(void)
{
	int rv=0;
	if(get_api_preconditions())
	{
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}

	connection_network_status_e  status;

	rv = connection_get_network_status(CONNECTION_MOBILE_TYPE, &status);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get Status Fail : %d\n", rv);
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}
	tet_printf("Connection Get Status Success : %d Status = %d\n", rv, status);
	connection_destroy(handle);
	tet_result(TET_PASS);

	if(status == CONNECTION_STATUS_AVAILABLE)
		conn_status = status;

	return;
}

void utc_connection_get_network_status_positive_3(void)
{
	int rv=0;
	if(get_api_preconditions())
	{
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}

	connection_network_status_e  status;

	rv = connection_get_network_status(CONNECTION_DEFAULT_TYPE, &status);

	if(rv!=CONNECTION_ERROR_NONE || status!=conn_status)
	{
		tet_printf("Connection Get Status Fail : %d\n", rv);
		connection_destroy(handle);
		tet_result(TET_FAIL);
		status = CONNECTION_STATUS_UNAVAILABLE;
		return;
	}
	tet_printf("Connection Get Status Success : %d Status = %d\n", rv, status);
	connection_destroy(handle);
	tet_result(TET_PASS);
	conn_status = CONNECTION_STATUS_UNAVAILABLE;
	return;
}

void utc_connection_get_network_status_negative_1(void)
{
	int rv=0;
	if(get_api_preconditions())
	{
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}
	rv = connection_get_network_status(CONNECTION_MOBILE_TYPE, NULL);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get Status MUST Fail : Success %d\n", rv);
		tet_result(TET_PASS);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get Status MUST Fail : Failure %d\n", rv);
	tet_result(TET_FAIL);
	connection_destroy(handle);
	return;
}

void utc_connection_get_network_status_negative_2(void)
{
	int rv=0;
	if(get_api_preconditions())
	{
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}
	connection_network_status_e  status;
	rv = connection_get_network_status(999, &status);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get Status MUST Fail : Success %d\n", rv);
		tet_result(TET_PASS);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get Status MUST Fail : Failure %d\n", rv);
	tet_result(TET_FAIL);
	connection_destroy(handle);
	return;
}

void utc_connection_get_last_datacall_duration_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}

	rv = connection_get_last_datacall_duration(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get last_datacall_duration : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get last_datacall_duration: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}

void utc_connection_get_last_datacall_duration_negative(void)
{
	int stat;

	rv = connection_get_last_datacall_duration(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get last_datacall_duration : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get last_datacall_duration: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}

void utc_connection_get_last_sent_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}

	rv = connection_get_last_sent_data_size(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get last_sent : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get last_sent: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}

void utc_connection_get_last_sent_negative(void)
{
	int stat;

	rv = connection_get_last_sent_data_size(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get last_sent : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get last_sent: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}

void utc_connection_get_last_recv_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}

	rv = connection_get_last_received_data_size(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get last_recv : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get last_recv: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_last_recv_negative(void)
{
	int stat;

	rv = connection_get_last_received_data_size(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get last_recv : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get last_recv: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_get_total_sent_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}

	rv = connection_get_total_sent_data_size(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get total_sent : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get total_sent: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_total_sent_negative(void)
{
	int stat;

	rv = connection_get_total_sent_data_size(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get total_sent : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get total_sent: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_get_total_recv_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}

	rv = connection_get_total_received_data_size(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get total_recv : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get total_recv: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_total_recv_negative(void)
{
	int stat;

	rv = connection_get_total_received_data_size(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get total_recv : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get total_recv : MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_get_last_duration_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}

	rv = connection_get_last_datacall_duration(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get last_duration : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get last_duration: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_last_duration_negative(void)
{
	int stat;

	rv = connection_get_last_datacall_duration(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get last_duration : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get last_duration: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_get_total_duration_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}

	rv = connection_get_total_datacall_duration(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get total_datacall_duration : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get total_datacall_duration: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_total_duration_negative(void)
{
	int stat;

	rv = connection_get_total_datacall_duration(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get total_datacall_duration : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get total_datacall_duration: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_get_wifi_last_datacall_duration_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}
	rv = connection_get_wifi_last_datacall_duration(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_last_datacall_duration : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get wifi_last_datacall_duration: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_wifi_last_datacall_duration_negative(void)
{
	int stat;

	rv = connection_get_wifi_last_datacall_duration(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_last_datacall_duration : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get wifi_last_datacall_duration: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_get_wifi_last_received_data_size_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}
	rv = connection_get_wifi_last_received_data_size(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_last_received_data_size : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get wifi_last_received_data_size: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_wifi_last_received_data_size_negative(void)
{
	int stat;

	rv = connection_get_wifi_last_received_data_size(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_last_received_data_size : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get wifi_last_received_data_size: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_get_wifi_last_sent_data_size_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}
	rv = connection_get_wifi_last_sent_data_size(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_last_sent_data_size : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get wifi_last_sent_data_size: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_wifi_last_sent_data_size_negative(void)
{
	int stat;

	rv = connection_get_wifi_last_sent_data_size(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_last_sent_data_size : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get wifi_last_sent_data_size: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_get_wifi_total_datacall_duration_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}
	rv = connection_get_wifi_total_datacall_duration(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_total_datacall_duration : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get wifi_total_datacall_duration: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_wifi_total_datacall_duration_negative(void)
{
	int stat;

	rv = connection_get_wifi_total_datacall_duration(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_total_datacall_duration : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get wifi_total_datacall_duration: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_get_wifi_total_received_data_size_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}
	rv = connection_get_wifi_total_received_data_size(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_total_received_data_size : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get wifi_total_received_data_size: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_wifi_total_received_data_size_negative(void)
{
	int stat;

	rv = connection_get_wifi_total_received_data_size(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_total_received_data_size : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get wifi_total_received_data_size: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_get_wifi_total_sent_data_size_positive(void)
{
	int stat;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}
	rv = connection_get_wifi_total_sent_data_size(handle, &stat);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_total_sent_data_size : Failure %d\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection Get wifi_total_sent_data_size: Success %d Value = %d\n", rv, stat);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_get_wifi_total_sent_data_size_negative(void)
{
	int stat;

	rv = connection_get_wifi_total_sent_data_size(NULL, &stat);
	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection Get wifi_total_sent_data_size : MUST FAIL %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Connection Get wifi_total_sent_data_size: MUST Fail %d \n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_unset_cb_positive(void)
{
	int rv=0;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}
	if(set_callbacks()){
		tet_result(TET_FAIL);
		return;
	}
	rv = connection_unset_cb(handle);
	if(rv != CONNECTION_ERROR_NONE){
		tet_printf("Connection UnSet Callback Failed! Error [%d]\n", rv);
		tet_result(TET_FAIL);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection UnSet Callback! Success [%d]\n", rv);
	tet_result(TET_PASS);
	connection_destroy(handle);
	return;
}
void utc_connection_unset_cb_negative(void)
{
	int rv=0;
	if(create_handle()){
		tet_result(TET_FAIL);
		return;
	}
	rv = connection_unset_cb(NULL);
	if(rv != CONNECTION_ERROR_NONE){
		tet_printf("Connection UnSet Callback! Must Fail : Success [%d]\n", rv);
		tet_result(TET_PASS);
		connection_destroy(handle);
		return;
	}
	tet_printf("Connection UnSet Callback Must Fail! Error [%d]\n", rv);
	connection_destroy(handle);
	tet_result(TET_FAIL);
	return;
}

void utc_connection_destroy_positive(void)
{
	int rv = connection_destroy(handle);

	if(rv==CONNECTION_ERROR_NONE)
	{
		tet_printf("Destroy connection : Success %d\n", rv);
		tet_result(TET_PASS);
		return;
	}
	tet_printf("Destroy connection : Failure %d\n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_destroy_negative(void)
{
	int rv = connection_destroy(NULL);

	if(rv!=CONNECTION_ERROR_NONE)
	{
		tet_printf("Connection destroy Must Fail : Success %d\n", rv);
		tet_result(TET_PASS);
		return;
	}

	tet_printf("Connection destroy Must Fail : Failure %d\n", rv);
	tet_result(TET_FAIL);
	return;
}
void utc_connection_is_connected_positive(void)
{
	bool status = FALSE;
	if(get_api_preconditions())
	{
		connection_destroy(handle);
		tet_result(TET_FAIL);
		return;
	}

	if((connection_is_connected()))
	{
		tet_result(TET_PASS);
		tet_printf("Is Connected:Expected %d vs Obtained %d\n", TRUE, status);
	}
	else
	{
		tet_result(TET_FAIL);
		tet_printf("Is Connected:Expected %d vs Obtained %d\n", FALSE, status);
	}
	connection_destroy(handle);

}
