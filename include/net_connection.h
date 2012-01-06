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


#ifndef __NET_CONNECTION_INTF_H__        /* To prevent inclusion of a header file twice */
#define __NET_CONNECTION_INTF_H__

#include <tizen_error.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef DEPRECATED
#define DEPRECATED __attribute__((deprecated))
#endif



/**
 * @addtogroup CAPI_NETWORK_FRAMEWORK
 * @{
*/

/**
 * @addtogroup CAPI_NETWORK_CONNECTION_MODULE
 * @{
*/

#include <arpa/inet.h>


/**
* @enum connection_error_e
* @brief Enumerations of network API errors.
*/
typedef enum
{
	CONNECTION_ERROR_NONE = TIZEN_ERROR_NONE, /**< Successful */

	CONNECTION_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */

    CONNECTION_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY, /**< Out of memory error */
    CONNECTION_ERROR_INVALID_OPERATION = TIZEN_ERROR_INVALID_OPERATION, /**< Invalid Operation */

}connection_error_e;

/**
* @enum connection_network_status_e
* @brief Enumerations of network status.
*/
typedef enum 
{

    /** Not connected / Suspended / Idle / Connecting / Disconnecting */
    CONNECTION_STATUS_UNAVAILABLE,

    /** Active */
    CONNECTION_STATUS_AVAILABLE,

    /** Service unknown */
    CONNECTION_STATUS_UNKNOWN,

}connection_network_status_e;

/**
* @enum connection_network_type_e
* @brief Enumerations of network connection type.
*/
typedef enum{
	/** Default type \n
	      The automatic connection is established.  \n
	      WiFi connection has higher priority than 3G connection. \n
	      If Wifi connection is established by user selection or auto joining,\n
	      3G connection will be automatically moved to WiFi connection in NET_DEFAULT_TYPE case. */
	CONNECTION_DEFAULT_TYPE = 0x00,

	/** Mobile Type \n
		Network connection is established in 3G network \n
	*/
	CONNECTION_MOBILE_TYPE	= 0x01,

	/** WiFi Type \n
		Network connection is established in WiFi network \n
	*/
	CONNECTION_WIFI_TYPE	= 0x02,

}connection_network_type_e;



/**
* @enum connection_network_param_e
* @brief Enumerations of changed network parameter.
* These are received as param in #connection_cb.
*/
typedef enum{
    /** Network Status has changed. \n
	*/
	CONNECTION_NETWORK_STATUS = 0x00,

	/** IP Address has changed. \n
	*/
	CONNECTION_IP_ADDRESS	= 0x01,

	/** Proxy Address has changed. \n
	*/
	CONNECTION_PROXY_ADDRESS	= 0x02,

}connection_network_param_e;


/**
 * @brief  The connection handle for all connection functions.
*/
typedef struct connection_handle_s  * connection_h;



/**
 * @brief Called when a connection event occurs. 
 * @param[in] param The enum of the parameter that changed. #connection_network_param_e
 * @param[in] user_data The user data passed from the callback registration function
 * @pre Register this function using connection_set_cb().
 * @see  connection_set_cb() 
 * @see connection_unset_cb()
 *
*/
typedef void (*connection_cb) (const connection_network_param_e param, void *user_data);


/**
 * @brief Creates a handle for managing data connections.
 *
 * @remarks @a handle must be released with connection_destroy(). \n
 *
 * @param[out] handle      The handle to the connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter 
 * @retval #CONNECTION_ERROR_OUT_OF_MEMORY  Out of memory error 
 * @see connection_destroy()
 *
 *
******************************************************************************************/
int connection_create(connection_h * handle);

/*****************************************************************************************/
/**
 * @brief  Destroys the connection handle.
 *
 * @param[in] handle      The handle to the connection
 *
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful 
 * @see connection_create()
 *
 *
******************************************************************************************/
int connection_destroy(connection_h handle);

/*****************************************************************************************/
/**
 * @brief  Registers a callback function to be invoked when connection event occurs.
 * @param[in] handle      The handle to the connection
 * @param[in] callback    The callback function to register
 * @param[in] user_data   The user data to be passed to the callback function 
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter 
 * @post  connection_cb() will be invoked.
 * @see connection_create()
 * @see connection_cb() 
 * @see connection_unset_cb()
 *
******************************************************************************************/
int connection_set_cb(connection_h handle, connection_cb callback, void *user_data);

/*****************************************************************************************/
/**
 * @brief  Unregisters the callback function.
 * @param[in] handle      The handle to the connection
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER  Invalid parameter 
 * @see connection_cb()
 * @see connection_set_cb()
******************************************************************************************/
int connection_unset_cb(connection_h handle);

/*****************************************************************************************/
/**
 * @brief 	Gets the IP address of an existing connection. 
 *
 * @remarks     @a ip_address must be released with free() by you. 
 * @param[in]	handle     The handle to the connection
 * @param[out]	ip_address   	    The pointer to IP address string.
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter 
 *
******************************************************************************************/
int connection_get_ip_address(connection_h handle, char **ip_address);


/*****************************************************************************************/
/**
 * @brief Gets the proxy address of the current connection. 
 *
 * @remarks  @a proxy must be released with free() by you.
 *
 * @param[in]	handle     The handle to the connection
 * @param[out]	proxy   The proxy address 
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter 
 *
 *
******************************************************************************************/
int connection_get_proxy(connection_h handle, char **proxy);

/*****************************************************************************************/
/**
 * @brief  Gets the network status. 
 *
 * @details The returned status is for the overall connection of the phone, not per application.
 *
 * @param[in] 	network_type    	The network type 
 * @param[out] 	network_status 		The status of network 
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter 
 * @retval #CONNECTION_ERROR_INVALID_OPERATION   Cannot retrieve the status due to internal error
 * 
 *
******************************************************************************************/
int connection_get_network_status(connection_network_type_e network_type, connection_network_status_e* network_status);


/*****************************************************************************************/
/**
 * @brief Gets the duration of the last cellular packet data connection.
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	last_datacall_duration    The time duration of the last cellular packet data connection ( seconds )
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 *
 ******************************************************************************************/
int connection_get_last_datacall_duration(connection_h handle, int *last_datacall_duration);

/*****************************************************************************************/
/**
 * @brief  Gets the received data size of the last cellular packet data connection. 
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	last_recv_data_size     The received data size of the last cellular packet data connection ( bytes )
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 *
 ******************************************************************************************/
int connection_get_last_received_data_size(connection_h handle, int * last_recv_data_size);

/*****************************************************************************************/
/**
 * @brief Gets the sent data size of the last cellular packet data connection. 
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	last_sent_data_size     the sent data size of the last cellular packet data connection.
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * 
 *
 ******************************************************************************************/
int connection_get_last_sent_data_size(connection_h handle, int * last_sent_data_size);

/*****************************************************************************************/
/**
 * @brief Gets the total time duration of all cellular packet data connections.
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	total_datacall_duration    The total time duration of all cellular packet data connections ( seconds )
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter 
 * 
 *
 ******************************************************************************************/
int connection_get_total_datacall_duration(connection_h handle, int * total_datacall_duration);

/*****************************************************************************************/
/**
 * @brief Gets the total received data size of all cellular packet data connections. 
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	total_recv_data_size    The total received data size of all cellular packet data connections (bytes)
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * 
 *
 ******************************************************************************************/
int connection_get_total_received_data_size (connection_h handle, int * total_recv_data_size);

/*****************************************************************************************/
/**
 * @brief Gets the total sent data size of all cellular packet data connections.
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	total_sent_data_size     The total sent data size of all cellular packet data connections (bytes)
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter 
 *
 *
 ******************************************************************************************/
int connection_get_total_sent_data_size (connection_h handle, int * total_sent_data_size);

/*****************************************************************************************/
/**
 * @brief Indicates connection status.
 *
 * @return Return @c true if connection is connected, otherwise @c false 
 *
 *
******************************************************************************************/
bool connection_is_connected(void);

/*****************************************************************************************/
/**
 * @brief Gets the duration of the last WiFi packet data connection.
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	wifi_last_datacall_duration    The time duration of the last WiFi packet data connection ( seconds )
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 *
 ******************************************************************************************/
int connection_get_wifi_last_datacall_duration(connection_h handle, int *wifi_last_datacall_duration);

/*****************************************************************************************/
/**
 * @brief  Gets the received data size of the last WiFi packet data connection. 
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	wifi_last_recv_data_size     The received data size of the last WiFi packet data connection ( bytes )
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 *
 ******************************************************************************************/
int connection_get_wifi_last_received_data_size(connection_h handle, int * wifi_last_recv_data_size);

/*****************************************************************************************/
/**
 * @brief Gets the sent data size of the last WiFi packet data connection. 
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	wifi_last_sent_data_size     the sent data size of the last WiFi packet data connection.
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * 
 *
 ******************************************************************************************/
int connection_get_wifi_last_sent_data_size(connection_h handle, int * wifi_last_sent_data_size);

/*****************************************************************************************/
/**
 * @brief Gets the total time duration of all WiFi packet data connections.
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	wifi_total_datacall_duration    The total time duration of all WiFi packet data connections ( seconds )
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter 
 * 
 *
 ******************************************************************************************/
int connection_get_wifi_total_datacall_duration(connection_h handle, int * wifi_total_datacall_duration);

/*****************************************************************************************/
/**
 * @brief Gets the total received data size of all WiFi packet data connections. 
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	wifi_total_recv_data_size    The total received data size of all WiFi packet data connections (bytes)
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter
 * 
 *
 ******************************************************************************************/
int connection_get_wifi_total_received_data_size (connection_h handle, int * wifi_total_recv_data_size);

/*****************************************************************************************/
/**
 * @brief Gets the total sent data size of all WiFi packet data connections.
 *
 * @param[in]   handle     The handle to the connection
 * @param[out]	wifi_total_sent_data_size     The total sent data size of all WiFi packet data connections (bytes)
 * @return 0 on success, otherwise negative error value.
 * @retval #CONNECTION_ERROR_NONE  Successful 
 * @retval #CONNECTION_ERROR_INVALID_PARAMETER   Invalid parameter 
 *
 *
 ******************************************************************************************/
int connection_get_wifi_total_sent_data_size (connection_h handle, int *wifi_total_sent_data_size);

/**
 * @}
*/
/** 
 * @}
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
