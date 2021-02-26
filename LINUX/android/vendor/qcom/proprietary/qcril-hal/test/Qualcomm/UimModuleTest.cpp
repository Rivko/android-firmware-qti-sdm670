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
#include "modules/qmi/UimModemEndPoint.h"
#define RIL_SHLIB
#include "telephony/ril.h"

using namespace std;
extern bool callback_executed;

extern RIL_RadioFunctions *funcs;

TEST (UimModuleTest, VerifyGetSimStatus)
{
  /* Step#2: Inject the desired telephony request */
  int dummy = 1;
  RIL_Token  libril_token = &dummy;
  funcs->onRequest(RIL_REQUEST_GET_SIM_STATUS, nullptr, 0, libril_token);
  /* Step#3: onRequest would dispatch a msg, give enough time to get it
   * handled by our module */
  threadSleep(1);
  /* Step#4: Verify that our local callbacks were executed*/
  ASSERT_TRUE(callback_executed);
}

TEST (UimModuleTest, VerifyEnterSimPin)
{
  char *sim_pin[2] = {0};
  char pin[5] = {'1','2','3','4'};

  sim_pin[0] = pin;
  callback_executed = false;
  /* Step#2: Inject the desired telephony request */
  int dummy = 1;
  RIL_Token  libril_token = &dummy;
  funcs->onRequest(RIL_REQUEST_ENTER_SIM_PIN, sim_pin, sizeof(sim_pin), libril_token);
  /* Step#3: onRequest would dispatch a msg, give enough time to get it
   * handled by our module */
  threadSleep(1);
  /* Step#4: Verify that our local callbacks were executed*/
  ASSERT_TRUE(callback_executed);
}

TEST(UimModuleTest, VerifyThatModemEndPointAreUnique) {
    std::shared_ptr<UimModemEndPoint> endPoint1 =
        ModemEndPointFactory<UimModemEndPoint>::getInstance().buildEndPoint();
    ASSERT_FALSE(endPoint1 == nullptr);

    ASSERT_EQ(2, endPoint1.use_count());

    /* Removal of existing refrences should result in decrement of ref-count*/
    endPoint1 = nullptr;
    ASSERT_EQ(1, ModemEndPointFactory<UimModemEndPoint>::getInstance().getRefCount());
}
#endif