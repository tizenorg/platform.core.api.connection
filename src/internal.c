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

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "net_connection_private.h"


static pthread_mutex_t connection_mutex;
static int connection_flag = 0;


static void __inter_mutex_init(void)
{
	if (g_atomic_int_get(&connection_flag) == 0)
		if (pthread_mutex_init(&connection_mutex, NULL) != 0)
			CONNECTION_LOG(CONNECTION_ERROR, "Mutex initialization failed!\n");

	g_atomic_int_inc(&connection_flag);
}

void __inter_mutex_destroy(void)
{
	if (g_atomic_int_dec_and_test(&connection_flag))
		pthread_mutex_destroy(&connection_mutex);
}

void _connection_inter_mutex_lock(void)
{
	__inter_mutex_init();
	pthread_mutex_lock(&connection_mutex);
}

void _connection_inter_mutex_unlock(void)
{
	pthread_mutex_unlock(&connection_mutex);
	__inter_mutex_destroy();
}

