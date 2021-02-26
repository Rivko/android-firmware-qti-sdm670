/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QMI_RIL_UTF
#include <memory>
#include "gtest/gtest.h"
#include "framework/SolicitedSyncMessage.h"
#include "framework/legacy.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/PdcModemEndPoint.h"
#include "telephony/ril.h"


TEST(BasicFeatureSanity, TestEqualityOfPdcModemEndPoints) {
    std::shared_ptr<PdcModemEndPoint> endPoint1 =
        ModemEndPointFactory<PdcModemEndPoint>::getInstance().buildEndPoint();

    std::shared_ptr<PdcModemEndPoint> endPoint2 =
        ModemEndPointFactory<PdcModemEndPoint>::getInstance().buildEndPoint();

    EXPECT_EQ(endPoint1, endPoint2);
}
#endif
