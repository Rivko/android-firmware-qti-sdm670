/*
* Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*
*/

#ifndef __WIFI_HAL_WIFI_CONFIG_TEST_HPP__
#define __WIFI_HAL_WIFI_CONFIG_TEST_HPP__

#include "wifi_hal.h"
#include <getopt.h>

namespace WIFI_CONFIG_TEST
{
    typedef struct{
        int dtim;
        u32 avgFactor;
        u32 guardTime;
        char countryCode[10];
    } cmdData;

    class WiFiConfigTestSuite
    {
    public:
        /* CLI cmd strings */
        static const char *WIFI_CONFIG_CMD;
        static const char *WIFI_CONFIG_SET_DTIM;
        static const char *WIFI_CONFIG_SET_AVG_FACTOR;
        static const char *WIFI_CONFIG_SET_GUARD_TIME;
        static const char *WIFI_CONFIG_SET_COUNTRY_CODE;

        /* Default service name */
        static const char *DEFAULT_SVC;

        /* Default service name */
        static const char *DEFAULT_SVC_INFO;

        WiFiConfigTestSuite(wifi_interface_handle ifaceHandle);

        /* execute the command line args */
        void executeCmd(int cmdId, cmdData data);

        void setRequestId(int reqId);
        int getRequestId();
        wifi_error processCmd(int argc, char **argv);

    private:
        wifi_request_id id;
        wifi_interface_handle ifaceHandle;

        /* Send a WiFiConfig command to Wi-Fi HAL layer */
        void wifiConfigSendSetDtimRequest(int dtim);
        void wifiConfigSendSetAvgFactorRequest(u32 avgFactor);
        void wifiConfigSendSetGuardTimeRequest(u32 guardTime);
        void wifiConfigSendSetCountryCodeRequest(char *countryCode);
    };
}

#endif
