/*
 * 
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 * 
 * This software is the confidential and proprietary information of SAMSUNG 
 * ELECTRONICS ("Confidential Information"). You agree and acknowledge that 
 * this software is owned by Samsung and you shall not disclose such 
 * Confidential Information and shall use it only in accordance with the terms 
 * of the license agreement you entered into with SAMSUNG ELECTRONICS. SAMSUNG 
 * make no representations or warranties about the suitability of the software, 
 * either express or implied, including but not limited to the implied 
 * warranties of merchantability, fitness for a particular purpose, or 
 * non-infringement. SAMSUNG shall not be liable for any damages suffered by 
 * licensee arising out of or related to this software.
 * 
 */


#include <net_connection_interface.h>

void foo(void)
{
    //..
    // open connection	 
    //..
    char *ip_address = NULL;
    connection_get_ip_address(handle,&ip_address);
    if(err != CONNECTION_ERROR_NONE)
    {
    // error handling
    }
    char *proxy = NULL;
    connection_get_proxy(handle,&proxy);
    if(err != CONNECTION_ERROR_NONE)
    {
    // error handling
    }
}
