/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QMI_RIL_UTF
#include "gtest/gtest.h"
#define RIL_SHLIB
#include "telephony/ril.h"

#include <string.h>
#include <stdlib.h>
#include <utils/StrongPointer.h>
#include <framework/QcrilInitMessage.h>
#include <framework/legacy.h>
#include <framework/Util.h>
#include <dirent.h>

#include "modules/ims/ImsModule.h"
#include "qcril_qmi_ims_radio_utils.h"
#include "imsIF.pb.h"
#include <vendor/qti/hardware/radio/ims/1.0/IImsRadio.h>
#include <cutils/properties.h>

constexpr int SYS_PROP_LENGTH_MULTI_SIM = 4;

using namespace std;
using namespace vendor::qti::hardware::radio::ims::V1_0;
using namespace vendor::qti::hardware::radio::ims::V1_0::utils;

using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
static bool callback_executed;

extern "C" void
RIL_register (const RIL_RadioFunctions *callbacks);

static void RIL_onRequestComplete(RIL_Token t, RIL_Errno e,
        void *response, size_t responselen) {
    Log::getInstance().d("[RIL_onRequestComplete]");
    callback_executed = true;
}
static void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
        size_t datalen) {
    Log::getInstance().d("[RIL_onUnsolicitedResponse] unsol id = " + std::to_string(unsolResponse));
}
static void RIL_requestTimedCallback (RIL_TimedCallback callback,
        void *param, const struct timeval *relativeTime) {
    Log::getInstance().d("[RIL_requestTimedCallback]");
}
static void RIL_onRequestAck(RIL_Token t) {
    Log::getInstance().d("[RIL_onRequestAck]");
}

static static struct RIL_Env s_rilEnv = {
    RIL_onRequestComplete,
    RIL_onUnsolicitedResponse,
    RIL_requestTimedCallback,
    RIL_onRequestAck
};

extern "C" const RIL_RadioFunctions *RIL_Init
(
  const struct RIL_Env *env,
  int argc,
  char **argv
);

extern "C" void onRequest
(
  int request,
  void *data,
  size_t datalen,
  RIL_Token t
);

/* Verify that dispatcher properly removes all the references to the module upon module
  destructor..
*/
static const RIL_RadioFunctions *funcs = nullptr;

TEST(ImsFeatureSanity, TestRegisteredMessageProcessing) {
    std::unique_ptr<ImsModule> imsModule(new ImsModule());
    imsModule->init();

    ASSERT_FALSE(imsModule->isReady());

}

bool isMultiSim() {
    char prop_val[ SYS_PROP_LENGTH_MULTI_SIM + 1 ];

    property_get( QMI_RIL_SYS_PROP_NAME_MULTI_SIM, prop_val, "" );
    cout << "simConfig is" << prop_val << endl;
    return strncmp(prop_val, "ssss", PROPERTY_VALUE_MAX );
}

// IMS HIDL test - check for correct instances for HIDL service
// in single sim and msim
TEST(ImsFeatureSanity, TestHidlInstances) {
    //Check for running HIDL service
    qmi_ril_gen_operational_status_type cur_status;
    cur_status = qmi_ril_get_operational_status();
    cout << "ril init status ::" << (int)cur_status << endl;
    if(cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_UNKNOWN){
        //Ril Init
        char *argv[] = {
            "-c", "0"
        };
        funcs = RIL_Init(&s_rilEnv, sizeof(argv)/sizeof(*argv), argv);
    }

    //Sleep here is required, to wait for RIL_Init,
    //RIL_Init maybe initialized here or in AndroidModuleTest
    //There is no other way to get the Boot-up complete during gtests
    sleep(4);

    android::sp<IImsRadio> imsService0 = nullptr;
    android::sp<IImsRadio> imsService1 = nullptr;

    if (isMultiSim()) {
        imsService0 = IImsRadio::tryGetService("imsradio0");
        imsService1 = IImsRadio::tryGetService("imsradio1");
        ASSERT_TRUE(nullptr != imsService0.get());
        ASSERT_TRUE(nullptr != imsService1.get());
    } else {
        imsService0 = IImsRadio::tryGetService("imsradio0");
        imsService1 = IImsRadio::tryGetService("imsradio1");
        ASSERT_TRUE(nullptr != imsService0.get());
        ASSERT_TRUE(nullptr == imsService1.get());
    }
}

// IMS call details test for RTT
TEST(ImsFeatureSanity, TestRttCallDetails) {
    //Instantiate ims_CallDetails
    ims_CallDetails callDetails;
    memset(&callDetails, 0, sizeof(callDetails));

    callDetails.has_rttMode = TRUE;
    callDetails.rttMode = ims_RttMode_RTT_MODE_FULL;
    CallDetails  outCall;
    memset(&outCall, 0, sizeof(outCall));
    convertProtoToHidlCallDetails((const ims_CallDetails)callDetails, outCall);
    cout <<" rttMode in HIDL " << static_cast<int>(outCall.rttMode);
    ASSERT_TRUE(1 == static_cast<int>(outCall.rttMode));
}

// IMS call details test for RTT
TEST(ImsFeatureSanity, TestRttCallDetailsN) {
    //Instantiate ims_CallDetails
    ims_CallDetails callDetails;
    memset(&callDetails, 0, sizeof(callDetails));

    callDetails.has_rttMode = TRUE;
    callDetails.rttMode = ims_RttMode_RTT_MODE_DISABLED;
    CallDetails  outCall;
    memset(&outCall, 0, sizeof(outCall));
    convertProtoToHidlCallDetails((const ims_CallDetails)callDetails, outCall);
    cout <<" rttMode in HIDL " << static_cast<int>(outCall.rttMode);
    ASSERT_TRUE(0 == static_cast<int> (outCall.rttMode));
}

// IMS call details test for RTT
TEST(ImsFeatureSanity, TestRttCallDetailsNN) {
    //Instantiate ims_CallDetails
    ims_CallDetails callDetails;
    memset(&callDetails, 0, sizeof(callDetails));

    callDetails.has_rttMode = FALSE;
    callDetails.rttMode = ims_RttMode_RTT_MODE_DISABLED;
    CallDetails  outCall;
    memset(&outCall, 0, sizeof(outCall));
    convertProtoToHidlCallDetails((const ims_CallDetails)callDetails, outCall);
    cout <<" rttMode in HIDL " << static_cast<int>(outCall.rttMode);
    ASSERT_TRUE(2== static_cast<int> (outCall.rttMode));
}

TEST(ImsFeatureSanity, TestRttMsgConversion) {
    //Instantiate ims_CallDetails
    hidl_string inStr(" rttMessage in HIDL ");
    char * outStr;
    if (!inStr.empty()){
      size_t destSize = sizeof(char) * (inStr.size() + 1);
      outStr = (char*)qcril_malloc(destSize);
    } else {
      cout << "inStr is Null" << endl;
    }
    convertHidlToProtoString(inStr, outStr);
    cout <<" rttMessage in HIDL " << inStr.c_str() << "strcmp" << strncmp(" rttMessage in HIDL ", inStr.c_str() , inStr.size() + 1) << endl;
    cout <<" rttMessage in Proto " << outStr << "strcmp" << strncmp(" rttMessage in HIDL ", outStr, inStr.size() + 1) << endl;
    ASSERT_TRUE(0 == strncmp(" rttMessage in HIDL ", outStr, inStr.size() + 1));
    free(outStr);
}
#endif