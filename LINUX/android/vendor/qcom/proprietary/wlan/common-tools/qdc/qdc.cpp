/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * QDC : qti wificond Daemon Communication tool.
 *
 * This is a sample tool which is used to call QSAP APIs via
 * wificond daemon.
 */

#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <android-base/logging.h>
#include <android-base/stringprintf.h>
#include <android-base/strings.h>
#include <binder/IBinder.h>
#include <binder/IServiceManager.h>
#include <utils/String16.h>
#include <utils/StrongPointer.h>

#include "android/net/wifi/IWificond.h"
#include "wificond/ipc_constants.h"

using android::String16;
using android::base::StringPrintf;
using android::base::Trim;
using android::net::wifi::IWificond;
using android::sp;
using android::IBinder;
using android::wificond::ipc_constants::kServiceName;
using std::string;
using std::vector;

namespace android {
namespace wificond {
namespace qdc {
namespace {

sp<IWificond> getWificondService() {
    sp<IWificond> service;
    status_t status = getService(String16(kServiceName), &service);
    if (status != NO_ERROR || service.get() == nullptr) {
        LOG(ERROR) << "Failed to get wificond service";
    }
    return service;
}

} // namespace
} // qdc
} // wificond
} // android


int main(int argc, char** argv) {

    int retry = 0;
    bool status = false;
    sp<IWificond> service;
    std::stringstream ss;

    // get wificond service.
    LOG(INFO) << "get wificond service";
    while (retry < 10) {
        service = android::wificond::qdc::getWificondService();
        if (service.get() == nullptr) {
            LOG(ERROR) << "get wificond service failed. Retry = " << retry + 1;
	    sleep(1); // wait for 1 sec to get wificond ready
	    retry++;
	    continue;
        }
	break;
    }
    if (retry == 10) {
        LOG(ERROR) << "Failed to get wificond service. Make sure wificond is running or not";
	return -1;
    }

    LOG(INFO) << "wificond service received.";

    if (argc > 1) {
        // create a byte string to be sent to setHostapdParam()
        for (int i = 0; i < argc - 1; i++) {
	    ss << argv[i+1] << " "; /* This will add extra space at end of arguements.
                                     * wificond will handle while tokenizing the string */
	}
        std::string cstr = ss.str();
        LOG(INFO) << "Sending " << cstr ;
        service->setHostapdParam(vector<uint8_t>(cstr.begin(), cstr.end()), &status);
        LOG(INFO) << "Received " << (status ? "OK" : "FAIL");
    }

    // display result or return value.
    if (status)
        LOG(INFO) << "QDC Command Success.";
    else
        LOG(ERROR) << "QDC Command Failed.";
    return 0;
}
