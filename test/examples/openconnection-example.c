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


#include <stdio.h>

#include <app.h>
#include <app_efl.h>
#include <i18n.h>
#include <Ecore_X.h>
#include <net_connection_interface.h>
#include <glib-2.0/glib.h>
#include <dbus/dbus-glib.h>

static GMutex *mutex= NULL; // mutex needed to synchronize connection callbacks

int main(int argc, char *argv[])
{

	if(!g_thread_supported())
	{
		g_thread_init(NULL);
	}

	dbus_g_thread_init();

	g_type_init();
	GMainLoop *main_loop = NULL;
	main_loop = g_main_loop_new(NULL, FALSE);

	mutex = g_mutex_new(); // create mutex
	g_thread_create(&connection_thread, NULL, false, NULL); // create new thread with our connection_thread function
	// it's important to create new thread, because callbacks are called from main loop context - if we don’t return control to the main loop, the callback will never be called
        g_main_loop_run(main_loop);
	return 0;
}

void connection_callback(const char *param_name, void *user_data) // this callback will be called when IP is changed.
{
	g_mutex_unlock(mutex);
}

static gpointer connection_thread(gpointer data)
{

	int result;
	void *user_data; // application specific or NULL
	connection_h handle;
	connection_error_e err = connection_create(&handle); // create handle
	if(err != CONNECTION_ERROR_NONE)
	{
		// error handling
	}

	if(handle!=NULL)
	{ 
		connection_error_e retval = connection_set_cb(handle, connection_callback, user_data); // set open connection callback, if connection is opened/closed our callback will be called
		if(retval != CONNECTION_ERROR_NONE)
		{
			// error handling
		}
		retval = connection_open(handle, CONNECTION_MOBILE_TYPE); // start opening connection
		if(retval != CONNECTION_ERROR_NONE)
		{
			// error handling
		}
		g_mutex_lock(mutex); // wait till open_callback is successfully called

	        // now you can perform network operations

		// now the connection should be closed, because it is no longer needed
		retval = connection_close(handle, CONNECTION_MOBILE_TYPE); // invoke connection closing
		if(retval != CONNECTION_ERROR_NONE)
		{
			// error handling
		}
		g_mutex_lock(mutex); // wait till close_callback is called

		connection_error_e err = connection_destroy(handle); // free resources used by handle
		if(err != CONNECTION_ERROR_NONE)
		{
			// error handling
		}
	}
}

