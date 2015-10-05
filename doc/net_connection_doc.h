/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __TIZEN_NETWORK_CONNECTION_DOC_H__
#define __TIZEN_NETWORK_CONNECTION_DOC_H__

/**
 * @defgroup CAPI_NETWORK_CONNECTION_MODULE Connection
 * @brief The Connection API provides functions for managing modem data connections.
 * @ingroup CAPI_NETWORK_FRAMEWORK
 *
 * @section CAPI_NETWORK_CONNECTION_MODULE_HEADER Required Header
 *   \#include <net_connection.h>
 *
 * @section CAPI_NETWORK_CONNECTION_MODULE_OVERVIEW Overview
 * The Connection API provides functions used to create a network connection and to perform other operations on the
 * connection. With these functions, the application can get details such as the IP address, proxy information, gateway information, and connection
 * statistics.
 *
 */

/**
 * @defgroup CAPI_NETWORK_CONNECTION_MANAGER_MODULE Connection Manager
 * @brief The Connection Manager API provides functions for managing data connections.
 * @ingroup CAPI_NETWORK_CONNECTION_MODULE
 *
 * @section CAPI_NETWORK_CONNECTION_MANAGER_MODULE_HEADER Required Header
 *   \#include <net_connection.h>
 *
 * @section CAPI_NETWORK_CONNECTION_MANAGER_MODULE_OVERVIEW Overview
 * To use Connection Manager API, first create a connection handle using connection_create(). After that, you can obtain network information.
 * You should destroy the created connection handle if you do not need it anymore.
 * This API is related with libsoup and sockets. It allows you to create a socket on the kernel Linux stack, which can be used directly or by libsoup
 * or any other network library.
 * @section CAPI_NETWORK_CONNECTION_MANAGER_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 * - http://tizen.org/feature/network.bluetooth\n
 * - http://tizen.org/feature/network.wifi\n
 * - http://tizen.org/feature/network.telephony\n
 * - http://tizen.org/feature/network.tethering.bluetooth\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature List</b>.</a>
 *
 */

/**
 * @defgroup CAPI_NETWORK_CONNECTION_PROFILE_MODULE Connection Profile
 * @brief The Connection Profile API provides functions for managing the connection profile.
 * @ingroup CAPI_NETWORK_CONNECTION_MANAGER_MODULE
 *
 * @section CAPI_NETWORK_CONNECTION_PROFILE_MODULE_HEADER Required Header
 *   \#include <net_connection.h>
 *
 * @section CAPI_NETWORK_CONNECTION_PROFILE_MODULE_OVERVIEW Overview
 * The Connection Profile provides functions for mapping connection profile.
 * It allows you to use a handle for dealing with a connection profile. You can get details about connection using ‘connection_profile_h’ handle.
 * @section CAPI_NETWORK_CONNECTION_PROFILE_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 * - http://tizen.org/feature/network.bluetooth\n
 * - http://tizen.org/feature/network.wifi\n
 * - http://tizen.org/feature/network.telephony\n
 * - http://tizen.org/feature/network.tethering.bluetooth\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature List</b>.</a>
 *
 */

/**
 * @defgroup CAPI_NETWORK_CONNECTION_CELLULAR_PROFILE_MODULE Cellular Profile
 * @brief The Connection Cellular Profile API provides functions for managing the cellular profile.
 * @ingroup CAPI_NETWORK_CONNECTION_PROFILE_MODULE
 *
 * @section CAPI_NETWORK_CONNECTION_CELLULAR_PROFILE_MODULE_HEADER Required Header
 *   \#include <net_connection.h>
 * @section CAPI_NETWORK_CONNECTION_CELLULAR_PROFILE_MODULE_OVERVIEW Overview
 * The Connection Cellular Profile API provides functions for managing the cellular profile. You can manage the cellular profile using the functions.
 * @section CAPI_NETWORK_CONNECTION_CELLULAR_PROFILE_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 * - http://tizen.org/feature/network.telephony\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature List</b>.</a>
 *
 */

/**
 * @defgroup CAPI_NETWORK_CONNECTION_WIFI_PROFILE_MODULE Wi-Fi Profile
 * @brief The Connection Wi-Fi Profile API provides functions for managing the WiFi profile.
 * @ingroup CAPI_NETWORK_CONNECTION_PROFILE_MODULE
 *
 * @section CAPI_NETWORK_CONNECTION_WIFI_PROFILE_MODULE_HEADER Required Header
 *   \#include <net_connection.h>
 * @section CAPI_NETWORK_CONNECTION_WIFI_PROFILE_MODULE_OVERVIEW Overview
 * The Connection Wi-Fi Profile API provides functions for managing the wi-fi profile. You can manage the wi-fi profile using the functions.
 * @section CAPI_NETWORK_CONNECTION_WIFI_PROFILE_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 * - http://tizen.org/feature/network.wifi\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature List</b>.</a>
 *
 */

/**
 * @defgroup CAPI_NETWORK_CONNECTION_STATISTICS_MODULE Connection Statistics
 * @brief The Connection Statistics API provides functions for getting the statistical information.
 * @ingroup CAPI_NETWORK_CONNECTION_MODULE
 *
 * @section CAPI_NETWORK_CONNECTION_STATISTICS_MODULE_HEADER Required Header
 *   \#include <net_connection.h>
 *
 * @section CAPI_NETWORK_CONNECTION_STATISTICS_MODULE_HEADER_OVERVIEW Overview
 * The Connection allows you to track the data transfer information.
 * Use the Connection statistics to gather and reset statistics on network usage, such as the size of the sent or received data, in bytes.
 * It also provides methods for getting the cumulative size of packets sent or received.
 * @section CAPI_NETWORK_CONNECTION_STATISTICS_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 * - http://tizen.org/feature/network.wifi\n
 * - http://tizen.org/feature/network.telephony\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature List</b>.</a>
 *
 */




#endif /* __TIZEN_NETWORK_CONNECTION_DOC_H__ */
