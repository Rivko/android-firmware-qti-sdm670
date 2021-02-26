/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "latency_hidl_test"

#include <log/log.h>
#include <vendor/qti/hardware/data/latency/1.0/ILinkLatency.h>
#include <vendor/qti/hardware/data/latency/1.0/ILinkLatencyService.h>
#include <VtsHalHidlTargetTestBase.h>

using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::vendor::qti::hardware::data::latency::V1_0::ILinkLatency;
using ::vendor::qti::hardware::data::latency::V1_0::IClientToken;
using ::vendor::qti::hardware::data::latency::V1_0::ILinkLatencyService;
using ::vendor::qti::hardware::data::latency::V1_0::Level;
using ::vendor::qti::hardware::data::latency::V1_0::Radio;
using ::vendor::qti::hardware::data::latency::V1_0::StatusCode;

class ClientToken : public IClientToken {};

#define L1_CB [this](StatusCode status, Level eUplink, Level eDownlink) { \
    ASSERT_EQ(StatusCode::OK, status); \
    ASSERT_EQ(Level::L1, eUplink); \
    ASSERT_EQ(Level::L1, eDownlink); \
}
#define L2_CB [this](StatusCode status, Level eUplink, Level eDownlink) { \
    ASSERT_EQ(StatusCode::OK, status); \
    ASSERT_EQ(Level::L2, eUplink); \
    ASSERT_EQ(Level::L2, eDownlink); \
}
#define L3_CB [this](StatusCode status, Level eUplink, Level eDownlink) { \
    ASSERT_EQ(StatusCode::OK, status); \
    ASSERT_EQ(Level::L3, eUplink); \
    ASSERT_EQ(Level::L3, eDownlink); \
}
#define L4_CB [this](StatusCode status, Level eUplink, Level eDownlink) { \
    ASSERT_EQ(StatusCode::OK, status); \
    ASSERT_EQ(Level::L4, eUplink); \
    ASSERT_EQ(Level::L4, eDownlink); \
}
#define INVALID_ARGS_CB [this](StatusCode status, Level eUplink, Level eDownlink) { \
    ASSERT_EQ(StatusCode::INVALID_ARGUMENTS, status); \
}

class LatencyHidlTestBase : public ::testing::VtsHalHidlTargetTestBase {
public:
    virtual void SetUp() override {
        setUpFactory();
    }

    void setUpFactory() {
        linkLatency = ::testing::VtsHalHidlTargetTestBase::getService<ILinkLatency>();
        ASSERT_NE(nullptr, linkLatency.get()) << "Could not get HIDL instance";
    }

    sp<ILinkLatencyService> requestService(const StatusCode expected_result) {
        sp<ILinkLatencyService> linkLatencyService;

        auto cb = [&](StatusCode status, sp<ILinkLatencyService> service) {
            ASSERT_EQ(expected_result, status);

            linkLatencyService = service;
            ASSERT_NE(nullptr, linkLatencyService.get()) << "Could not get HIDL instance";
        };

        sp<ClientToken> token = new ClientToken();
        const Return<void> ret = linkLatency->createLatencyService(token, cb);
        return linkLatencyService;
    }

    sp<ILinkLatency> linkLatency;
};

// positive test. Ensure linkLatency can request an ILinkLatencyService and release the same.
TEST_F(LatencyHidlTestBase, TestCreateLinkLatencyService) {
    sp<ILinkLatencyService> service = requestService(StatusCode::OK);
    ASSERT_NE(nullptr, service.get()) << "Could not get Service instance";
}

class LatencyHidlTestService : public LatencyHidlTestBase {
public:
    virtual void SetUp() override {
        setUpFactory();
        linkLatencyService = requestService(StatusCode::OK);
        linkLatencyService2 = requestService(StatusCode::OK);
    }

    sp<ILinkLatencyService> linkLatencyService;
    sp<ILinkLatencyService> linkLatencyService2;
};

// positive test. Ensure setLevel updates the effective levels for uplink and dowlink.
TEST_F(LatencyHidlTestService, TestSetLevel) {
    Return<void> ret = linkLatencyService->setLevel(Radio::WWAN, Level::L1, Level::L1, L1_CB);
    ASSERT_TRUE(ret.isOk());
}

// negative test. Invalid Level returns INVALID_ARGUMENTS
TEST_F(LatencyHidlTestService, TestSetLevelInvalidArgs) {
    Return<void> ret = linkLatencyService->setLevel(Radio::WWAN, (Level) 8, Level::L1, INVALID_ARGS_CB);
    ASSERT_TRUE(ret.isOk());
}

// negative test. Invalid Level returns INVALID_ARGUMENTS
TEST_F(LatencyHidlTestService, TestSetLevelInvalidArgs2) {
    Return<void> ret = linkLatencyService->setLevel(Radio::WWAN, Level::L1, (Level) 0, INVALID_ARGS_CB);
    ASSERT_TRUE(ret.isOk());
}

// negative test. Invalid Level returns INVALID_ARGUMENTS
TEST_F(LatencyHidlTestService, TestSetLevelInvalidArgs3) {
    Return<void> ret = linkLatencyService->setLevel(Radio::WWAN, (Level) 0, (Level) 5, INVALID_ARGS_CB);
    ASSERT_TRUE(ret.isOk());
}

// positive test. Ensure setLevel works on multiple clients
TEST_F(LatencyHidlTestService, TestSetLevel2Clients) {
    Return<void> ret;

    ret = linkLatencyService->setLevel(Radio::WWAN, Level::L1, Level::L1, L1_CB);
    ASSERT_TRUE(ret.isOk());

    ret = linkLatencyService2->setLevel(Radio::WWAN, Level::L1, Level::L1, L1_CB);
    ASSERT_TRUE(ret.isOk());
}

// positive test. Ensure setLevel works on multiple clients
TEST_F(LatencyHidlTestService, TestSetLevel2Clients2) {
    Return<void> ret;

    ret = linkLatencyService->setLevel(Radio::WWAN, Level::L2, Level::L2, L2_CB);
    ASSERT_TRUE(ret.isOk());

    ret = linkLatencyService2->setLevel(Radio::WWAN, Level::L1, Level::L1, L2_CB);
    ASSERT_TRUE(ret.isOk());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int status = RUN_ALL_TESTS();
    ALOGE("Test result with status=%d", status);
    return status;
}
