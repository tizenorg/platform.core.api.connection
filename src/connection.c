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

#include <net_connection.h>
#include <net_connection_private.h>
#include <vconf/vconf.h>
#include <dlog.h>
#include <string.h>
#include <stdio.h>

#define TIZEN_N_CONNECTION "CAPI_NETWORK_CONNECTION"

void connection_cb_net_config_change_cb(keynode_t *node, void *user_data);

// API to convert error codes back and forth
int convert_error_code(int dnet_error_code)
{
	switch(dnet_error_code)
	{
		case CONNECTION_ERROR_NONE:
			return CONNECTION_ERROR_NONE;
		case CONNECTION_ERROR_INVALID_PARAMETER:
			return CONNECTION_ERROR_INVALID_PARAMETER;
		default:
			return CONNECTION_ERROR_INVALID_PARAMETER;
	}
}

int connection_set_callbacks(connection_h handle, void *callback, void *user_data)
{
	if(handle!=NULL)
	{
		connection_handle_s *local_handle = (connection_handle_s *)handle;
		local_handle->user_data = user_data;
		local_handle->callback = callback;
        if(callback)
        {
            // This single vconf key will notify 
            // network status, ip and proxy changes.
		    vconf_notify_key_changed(VCONFKEY_NETWORK_CONFIGURATION_CHANGE_IND, 
						connection_cb_net_config_change_cb,
						local_handle);
        }
        else
        {
		    vconf_ignore_key_changed(VCONFKEY_NETWORK_CONFIGURATION_CHANGE_IND, 
						connection_cb_net_config_change_cb);
        }
		return (CONNECTION_ERROR_NONE);
	}
	else
	{
		return (CONNECTION_ERROR_INVALID_PARAMETER);
	}
}

int connection_set_cb(connection_h handle, connection_cb callback, void *user_data)
{
	int retval = CONNECTION_ERROR_NONE;

	retval = connection_set_callbacks(handle, callback, user_data);

	return convert_error_code(retval);
}

int connection_unset_cb(connection_h handle)
{
	int retval = CONNECTION_ERROR_NONE;

	retval = connection_set_callbacks(handle, NULL, NULL);

	return convert_error_code(retval);
}

void connection_cb_net_config_change_cb(keynode_t *node, void *user_data)
{
	LOGI(TIZEN_N_CONNECTION,"Net Status Indication\n");
	if((user_data!=NULL))
	{
		connection_handle_s *temp = user_data;
		if(temp->callback!=NULL)
		{
            if(!strcmp(vconf_keynode_get_name(node), VCONFKEY_NETWORK_STATUS))
                temp->callback(CONNECTION_NETWORK_STATUS, temp->user_data);
            if(!strcmp(vconf_keynode_get_name(node), VCONFKEY_NETWORK_IP))
                temp->callback(CONNECTION_IP_ADDRESS, temp->user_data);
            if(!strcmp(vconf_keynode_get_name(node), VCONFKEY_NETWORK_PROXY))
                temp->callback(CONNECTION_PROXY_ADDRESS, temp->user_data);
		}
	}
}

int connection_create(connection_h *handle)
{
	if(handle==NULL)
	{
		LOGI(TIZEN_N_CONNECTION,"Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}
	*handle = calloc(1, sizeof(connection_handle_s));
	if(*handle!=NULL)
	{
		LOGI(TIZEN_N_CONNECTION,"New Handle Created %p\n", *handle);
	}
	else
	{
		return CONNECTION_ERROR_OUT_OF_MEMORY;
	}
	return (CONNECTION_ERROR_NONE);
}

int connection_destroy(connection_h handle)
{
	if(handle==NULL)
	{
		LOGI(TIZEN_N_CONNECTION,"Wrong Parameter Passed\n");
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	LOGI(TIZEN_N_CONNECTION,"Destroy Handle : %p\n", handle);
	free(handle);

	return CONNECTION_ERROR_NONE;
}

int connection_get_network_status(connection_network_type_e network_type,
				connection_network_status_e* network_status)
{

	if (network_status==NULL ||
	    network_type > CONNECTION_WIFI_TYPE ||
	    network_type < CONNECTION_DEFAULT_TYPE)
	{
		return CONNECTION_ERROR_INVALID_PARAMETER;
	}

	int status = 0;

	if (vconf_get_int(VCONFKEY_NETWORK_STATUS, &status))
	{
		LOGI(TIZEN_N_CONNECTION,"First Step Failure = %d\n", status);
		return CONNECTION_ERROR_INVALID_OPERATION;
	}
	LOGI(TIZEN_N_CONNECTION,"Connected Network = %d\n", status);

	if (network_type==CONNECTION_DEFAULT_TYPE)
	{
		switch(status)
		{
			case VCONFKEY_NETWORK_CELLULAR:
			case VCONFKEY_NETWORK_WIFI:
				*network_status = CONNECTION_STATUS_AVAILABLE;
				break;
			default :
				*network_status = CONNECTION_STATUS_UNAVAILABLE;
				break;
		}

		return CONNECTION_ERROR_NONE;
	}

	if (network_type == CONNECTION_MOBILE_TYPE)
	{
		if (!vconf_get_int(VCONFKEY_NETWORK_CELLULAR_STATE,&status))
		{
			LOGI(TIZEN_N_CONNECTION,"Mobile = %d\n", status);
			if (status==VCONFKEY_NETWORK_CELLULAR_ON)
			{
				*network_status = CONNECTION_STATUS_AVAILABLE;
			}
			else
			{
				*network_status = CONNECTION_STATUS_UNAVAILABLE;
			}
			return CONNECTION_ERROR_NONE;
		}
		else
		{
			*network_status = CONNECTION_STATUS_UNKNOWN;
			LOGI(TIZEN_N_CONNECTION,"3G Failed = %d\n", status);
			return CONNECTION_ERROR_INVALID_OPERATION;
		}
	}

	if (network_type == CONNECTION_WIFI_TYPE)
	{
		if (!vconf_get_int(VCONFKEY_NETWORK_WIFI_STATE,&status))
		{
			LOGI(TIZEN_N_CONNECTION,"WiFi = %d\n", status);
			if (status==VCONFKEY_NETWORK_WIFI_CONNECTED)
			{
				*network_status = CONNECTION_STATUS_AVAILABLE;
			}
			else
			{
				*network_status = CONNECTION_STATUS_UNAVAILABLE;
			}
			return CONNECTION_ERROR_NONE;
		}
		else
		{
			*network_status = CONNECTION_STATUS_UNKNOWN;
			LOGI(TIZEN_N_CONNECTION,"WiFi Failed = %d\n", status);
			return CONNECTION_ERROR_INVALID_OPERATION;
		}
	}
	return CONNECTION_ERROR_INVALID_PARAMETER;
}

bool connection_is_connected(void)
{
	int network_status = 0;
	if (!vconf_get_int(VCONFKEY_NETWORK_STATUS, &network_status))
	{
		return (network_status > VCONFKEY_NETWORK_OFF) ? true : false;
	}
	else
		return false;
}

int connection_get_ip_address(connection_h handle, char **ip_address)
{
	if(handle)
	{
		*ip_address = vconf_get_str(VCONFKEY_NETWORK_IP);
		LOGI(TIZEN_N_CONNECTION,"IP Address %s\n", *ip_address);
		return CONNECTION_ERROR_NONE;
	}
	return (CONNECTION_ERROR_INVALID_PARAMETER);
}

int connection_get_proxy(connection_h handle, char **proxy)
{
	if(handle)
	{
		*proxy = vconf_get_str(VCONFKEY_NETWORK_PROXY);
		LOGI(TIZEN_N_CONNECTION,"Proxy Address %s\n", *proxy);
		return (CONNECTION_ERROR_NONE);
	}
	return (CONNECTION_ERROR_INVALID_PARAMETER);
}

int fill_call_statistic(connection_h handle, stat_request_e member, int *value)
{
	if(handle && value)
	{
        switch(member)
        {
            case LAST_DATACALL_DURATION:
		        *value = 0;
                break;
            case LAST_SENT_DATA_SIZE:
                if (vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_SNT, value))
                {
                    LOGI(TIZEN_N_CONNECTION,"Cannot Get LAST_SENT_DATA_SIZE = %d\n", *value);
		            *value = 0;
                    return CONNECTION_ERROR_INVALID_OPERATION;
                }
                LOGI(TIZEN_N_CONNECTION,"LAST_SENT_DATA_SIZE:%d bytes\n", *value);

                break;
            case LAST_RECEIVED_DATA_SIZE:
                if (vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_LAST_RCV, value))
                {
                    LOGI(TIZEN_N_CONNECTION,"Cannot Get LAST_RECEIVED_DATA_SIZE: = %d\n", *value);
		            *value = 0;
                    return CONNECTION_ERROR_INVALID_OPERATION;
                }
                LOGI(TIZEN_N_CONNECTION,"LAST_RECEIVED_DATA_SIZE:%d bytes\n", *value);
                break;
            case TOTAL_DATACALL_DURATION:
		        *value = 0;
                break;
            case TOTAL_SENT_DATA_SIZE:
                if (vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_TOTAL_SNT, value))
                {
                    LOGI(TIZEN_N_CONNECTION,"Cannot Get TOTAL_SENT_DATA_SIZE: = %d\n", *value);
		            *value = 0;
                    return CONNECTION_ERROR_INVALID_OPERATION;
                }
                LOGI(TIZEN_N_CONNECTION,"TOTAL_SENT_DATA_SIZE:%d bytes\n", *value);
                break;
            case TOTAL_RECEIVED_DATA_SIZE:
                if (vconf_get_int(VCONFKEY_NETWORK_CELLULAR_PKT_TOTAL_RCV, value))
                {
                    LOGI(TIZEN_N_CONNECTION,"Cannot Get TOTAL_RECEIVED_DATA_SIZE: = %d\n", *value);
		            *value = 0;
                    return CONNECTION_ERROR_INVALID_OPERATION;
                }
                LOGI(TIZEN_N_CONNECTION,"TOTAL_RECEIVED_DATA_SIZE:%d bytes\n", *value);
                break;
            case LAST_WIFI_DATACALL_DURATION:
		        *value = 0;
                break;
            case LAST_WIFI_SENT_DATA_SIZE:
                if (vconf_get_int(VCONFKEY_NETWORK_WIFI_PKT_LAST_SNT, value))
                {
                    LOGI(TIZEN_N_CONNECTION,"Cannot Get LAST_WIFI_SENT_DATA_SIZE: = %d\n", *value);
		            *value = 0;
                    return CONNECTION_ERROR_INVALID_OPERATION;
                }
                LOGI(TIZEN_N_CONNECTION,"LAST_WIFI_SENT_DATA_SIZE:%d bytes\n", *value);
                break;
            case LAST_WIFI_RECEIVED_DATA_SIZE:
                if (vconf_get_int(VCONFKEY_NETWORK_WIFI_PKT_LAST_RCV, value))
                {
                    LOGI(TIZEN_N_CONNECTION,"Cannot Get LAST_WIFI_RECEIVED_DATA_SIZE: = %d\n", *value);
		            *value = 0;
                    return CONNECTION_ERROR_INVALID_OPERATION;
                }
                LOGI(TIZEN_N_CONNECTION,"LAST_WIFI_RECEIVED_DATA_SIZE:%d bytes\n", *value);
                break;
            case TOTAL_WIFI_DATACALL_DURATION:
		        *value = 0;
                break;
            case TOTAL_WIFI_SENT_DATA_SIZE:
                if (vconf_get_int(VCONFKEY_NETWORK_WIFI_PKT_TOTAL_SNT, value))
                {
                    LOGI(TIZEN_N_CONNECTION,"Cannot Get TOTAL_WIFI_SENT_DATA_SIZE: = %d\n", *value);
		            *value = 0;
                    return CONNECTION_ERROR_INVALID_OPERATION;
                }
                LOGI(TIZEN_N_CONNECTION,"TOTAL_WIFI_SENT_DATA_SIZE:%d bytes\n", *value);
                break;
            case TOTAL_WIFI_RECEIVED_DATA_SIZE:
                if (vconf_get_int(VCONFKEY_NETWORK_WIFI_PKT_TOTAL_RCV, value))
                {
                    LOGI(TIZEN_N_CONNECTION,"Cannot Get TOTAL_WIFI_RECEIVED_DATA_SIZE: = %d\n", *value);
		            *value = 0;
                    return CONNECTION_ERROR_INVALID_OPERATION;
                }
                LOGI(TIZEN_N_CONNECTION,"TOTAL_WIFI_RECEIVED_DATA_SIZE:%d bytes\n", *value);
                break;
        }
		return (CONNECTION_ERROR_NONE);
	}
	return (CONNECTION_ERROR_INVALID_PARAMETER);
}

int connection_get_last_datacall_duration(connection_h handle, int *value)
{
	return fill_call_statistic(handle, LAST_DATACALL_DURATION, value);
}

int connection_get_last_received_data_size(connection_h handle, int *value)
{
	return fill_call_statistic(handle, LAST_RECEIVED_DATA_SIZE, value);
}

int connection_get_last_sent_data_size(connection_h handle, int *value)
{
	return fill_call_statistic(handle, LAST_SENT_DATA_SIZE, value);
}

int connection_get_total_datacall_duration(connection_h handle, int *value)
{
	return fill_call_statistic(handle, TOTAL_DATACALL_DURATION, value);
}

int connection_get_total_received_data_size (connection_h handle, int *value)
{
	return fill_call_statistic(handle, TOTAL_RECEIVED_DATA_SIZE, value);
}

int connection_get_total_sent_data_size (connection_h handle, int *value)
{
	return fill_call_statistic(handle, TOTAL_SENT_DATA_SIZE, value);
}

int connection_get_wifi_last_datacall_duration(connection_h handle, int *value)
{
	return fill_call_statistic(handle, LAST_WIFI_DATACALL_DURATION, value);
}

int connection_get_wifi_last_received_data_size(connection_h handle, int *value)
{
	return fill_call_statistic(handle, LAST_WIFI_RECEIVED_DATA_SIZE, value);
}

int connection_get_wifi_last_sent_data_size(connection_h handle, int *value)
{
	return fill_call_statistic(handle, LAST_WIFI_SENT_DATA_SIZE, value);
}

int connection_get_wifi_total_datacall_duration(connection_h handle, int *value)
{
	return fill_call_statistic(handle, TOTAL_WIFI_DATACALL_DURATION, value);
}

int connection_get_wifi_total_received_data_size (connection_h handle, int *value)
{
	return fill_call_statistic(handle, TOTAL_WIFI_RECEIVED_DATA_SIZE, value);
}

int connection_get_wifi_total_sent_data_size (connection_h handle, int *value)
{
	return fill_call_statistic(handle, TOTAL_WIFI_SENT_DATA_SIZE, value);
}
