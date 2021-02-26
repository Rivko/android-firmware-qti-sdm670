/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QMI_RIL_UTF
#include <memory>
#include "gtest/gtest.h"
#include "framework/Dispatcher.h"
#include "framework/Module.h"
#include "framework/Util.h"
#include "framework/SolicitedMessage.h"
#include "framework/SolicitedSyncMessage.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/Util.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "telephony/ril.h"

using namespace std;
extern bool callback_executed;
int token_val = 1;
extern RIL_RadioFunctions *funcs;

int off = 0; // OFF
int on  = 1; // ON
int invalid = INT_MAX; // Max Integer
int negative = -1;

// Roam Preference
int roam_off = 0;
int roam_on = 1;
int roam_any = 2;

int enable = 1 ;
int disable = 0;

int nw_type1 = INT_MAX;
int nw_type2 = UINT_MAX;
int nw_type3 = 1;
int nw_type4 = 22;

RIL_RadioBandMode band = BAND_MODE_UNSPECIFIED;

RIL_CdmaSubscriptionSource source_ruim = CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM;
RIL_CdmaSubscriptionSource source_nv   = CDMA_SUBSCRIPTION_SOURCE_NV;

RIL_InitialAttachApn apn_req_null = {
    .apn = nullptr,
    .protocol = nullptr,
    .authtype = 0,
    .username = nullptr,
    .password = nullptr,
};

RIL_InitialAttachApn apn_req = {
    .apn = "jionet",
    .protocol = "IPV4V6",
    .authtype = 0,
    .username = nullptr,
    .password = nullptr,
};


RIL_RadioCapability radio_cap_start = {1, 123, RC_PHASE_START, RAF_LTE, "1", 0};
RIL_RadioCapability radio_cap_apply = {1, 123, RC_PHASE_APPLY, RAF_LTE, "1", 0};
RIL_RadioCapability radio_cap_finish = {1, 123, RC_PHASE_FINISH, RAF_LTE, "1", 0};


/* Below is structure of the parameterized test data*/
typedef struct test_data {
    int android_req_id;
    void* data;
    size_t data_len;
    int token;
} test_data_t;

/* Below is our fixture class for these parameterized test data*/
class NasParameterizedTest : public ::testing::TestWithParam<test_data_t> {
};

/* Below is our fixture class for these parameterized test data*/
class NasComplexParameterizedTest : public ::testing::TestWithParam<test_data_t> {
};

/* This is the table for our test data*/
test_data_t test_data_1[] = {
  {RIL_REQUEST_VOICE_REGISTRATION_STATE, nullptr, 0, token_val++},
  {RIL_REQUEST_DATA_REGISTRATION_STATE, nullptr, 0, token_val++},
  {RIL_REQUEST_OPERATOR, nullptr, 0, token_val++},
  {RIL_REQUEST_CDMA_SUBSCRIPTION, nullptr, 0, token_val++},
  {RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, &nw_type1, sizeof(nw_type1), token_val++},
  {RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, &nw_type2, sizeof(nw_type2), token_val++},
  {RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, &nw_type3, sizeof(nw_type3), token_val++},
  {RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, &nw_type4, sizeof(nw_type4), token_val++},
  {RIL_REQUEST_SCREEN_STATE, &invalid, sizeof(invalid), token_val++},
  {RIL_REQUEST_SCREEN_STATE, &off, sizeof(off), token_val++},
  {RIL_REQUEST_SCREEN_STATE, &on, sizeof(on), token_val++},
  {RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE, &invalid, sizeof(invalid), token_val++},
  {RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE, &negative, sizeof(negative), token_val++},
  {RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE, &roam_off, sizeof(roam_off), token_val++},
  {RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE, &roam_on, sizeof(roam_on), token_val++},
  {RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE, &roam_any, sizeof(roam_any), token_val++},
  {RIL_REQUEST_SET_LOCATION_UPDATES, &invalid, sizeof(invalid), token_val++},
  {RIL_REQUEST_SET_LOCATION_UPDATES, &negative, sizeof(negative), token_val++},
  {RIL_REQUEST_SET_LOCATION_UPDATES, &enable, sizeof(enable), token_val++},
  {RIL_REQUEST_SET_LOCATION_UPDATES, &disable, sizeof(disable), token_val++},
  {RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE, nullptr, 0, token_val++},
  {RIL_REQUEST_GET_DC_RT_INFO, nullptr, 0, token_val++},
  {RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE, nullptr, 0, token_val++},
  {RIL_REQUEST_VOICE_RADIO_TECH, nullptr, 0, token_val++},
  {RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE, nullptr, 0, token_val++},
  {RIL_REQUEST_SET_BAND_MODE, &band, sizeof(band), token_val++},
  {RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE, &source_ruim, sizeof(source_ruim), token_val++},
  {RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE, &source_nv, sizeof(source_nv), token_val++},
  {RIL_REQUEST_SIGNAL_STRENGTH, nullptr, 0, token_val++},
};


/* This is the table for our test data*/
test_data_t test_data_2[] = {
  {RIL_REQUEST_ALLOW_DATA, &off, sizeof(off), token_val++},
  {RIL_REQUEST_ALLOW_DATA, &on, sizeof(on), token_val++},
  {RIL_REQUEST_ALLOW_DATA, &invalid, sizeof(invalid), token_val++},
  {RIL_REQUEST_ALLOW_DATA, &on, sizeof(on), token_val++},
  {RIL_REQUEST_SET_RADIO_CAPABILITY, &radio_cap_start, sizeof(radio_cap_start), token_val++},
  {RIL_REQUEST_SET_RADIO_CAPABILITY, &radio_cap_apply, sizeof(radio_cap_apply), token_val++},
  {RIL_REQUEST_SET_RADIO_CAPABILITY, &radio_cap_finish, sizeof(radio_cap_finish), token_val++},
  {RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC, nullptr, 0, token_val++},
  {RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL, nullptr, 0, token_val++},
  {RIL_REQUEST_SET_INITIAL_ATTACH_APN, &apn_req_null, sizeof(apn_req_null), token_val++},
  {RIL_REQUEST_SET_INITIAL_ATTACH_APN, &apn_req, sizeof(apn_req), token_val++},
  {RIL_REQUEST_QUERY_AVAILABLE_NETWORKS, nullptr, 0, token_val++},
};


/* Individual test with parameterized test data are being created here*/
INSTANTIATE_TEST_CASE_P(InstantiationName,
                        NasParameterizedTest,
                        ::testing::ValuesIn(test_data_1));

/* Executing the test here*/
TEST_P(NasParameterizedTest, NasBasicSanity) {
  callback_executed = false;
  test_data_t param = GetParam();
  funcs->onRequest(param.android_req_id, param.data, param.data_len, (RIL_Token)&(param.token));
  threadSleep(5);
  ASSERT_TRUE(callback_executed);
}


/* Individual test with parameterized test data are being created here*/
INSTANTIATE_TEST_CASE_P(InstantiationName,
                        NasComplexParameterizedTest,
                        ::testing::ValuesIn(test_data_2));

/* Executing the test here*/
TEST_P(NasComplexParameterizedTest, NasComplexSanity) {
  callback_executed = false;
  test_data_t param = GetParam();
  funcs->onRequest(param.android_req_id, param.data, param.data_len, (RIL_Token)&(param.token));
  threadSleep(30);
  ASSERT_TRUE(callback_executed);
}
#endif

