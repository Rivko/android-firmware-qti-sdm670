/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/
#include "NetPerfTuner.h"
#include "ResponseCode.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace wigig {
namespace netperftuner {
namespace V1_0 {
namespace implementation {

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MAX_NPT_RESPONSE_SIZE 256

NetPerfTuner::NetPerfTuner(CommandListener *commandListener)
: mCommandListener(commandListener) {
    // nothing else to do
}

// Methods from ::vendor::qti::hardware::wigig::netperftuner::V1_0::INetPerfTuner follow.
Return<void> NetPerfTuner::setTuningParameter(const hidl_string& param, const hidl_string& client, const hidl_string& value, setTuningParameter_cb _hidl_cb) {
	NetPerfTunerStatus status = {NetPerfTunerStatusCode::FAILURE_UNKNOWN, ""};
	char response[MAX_NPT_RESPONSE_SIZE];
	const char *argv[] = { "perftuner", "set" , param.c_str(), client.c_str(), value.c_str() };

	response[0] = '\0';
	int code = mCommandListener->runNptCommand(ARRAY_SIZE(argv), argv, response, sizeof(response));
	status = { responseCodeToStatusCode(code), response };
	_hidl_cb(status);
	return Void();
}

Return<void> NetPerfTuner::unsetTuningParameter(const hidl_string& param, const hidl_string& client, unsetTuningParameter_cb _hidl_cb) {
	NetPerfTunerStatus status = {NetPerfTunerStatusCode::FAILURE_UNKNOWN, ""};
	char response[MAX_NPT_RESPONSE_SIZE];
	const char *argv[] = { "perftuner", "unset" , param.c_str(), client.c_str() };

	response[0] = '\0';
	int code = mCommandListener->runNptCommand(ARRAY_SIZE(argv), argv, response, sizeof(response));
	status = { responseCodeToStatusCode(code), response };
	_hidl_cb(status);
	return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

INetPerfTuner* HIDL_FETCH_INetPerfTuner(const char* /* name */) {
    return new NetPerfTuner();
}

NetPerfTunerStatusCode NetPerfTuner::responseCodeToStatusCode(int code)
{
    switch (code) {
    case ResponseCode::CommandOkay:
        return NetPerfTunerStatusCode::SUCCESS;
    case ResponseCode::CommandParameterError:
        return NetPerfTunerStatusCode::FAILURE_INVALID_PARAM;
    case ResponseCode::CommandParameterValueError:
        return NetPerfTunerStatusCode::FAILURE_INVALID_VALUE;
    default:
        return NetPerfTunerStatusCode::FAILURE_UNKNOWN;
    }
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace netperftuner
}  // namespace wigig
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
