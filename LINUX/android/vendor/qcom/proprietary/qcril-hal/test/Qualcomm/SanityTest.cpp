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
#include "modules/voice/DialMessageCallback.h"
#include "modules/voice/RilRequestDialSyncMessage.h"
#include "modules/voice/VoiceModule.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/VoiceModemEndPoint.h"
#include "modules/qmi/DmsModemEndPoint.h"
#include "modules/qmi/PdcModemEndPoint.h"
#include "modules/android/LegacyHandlerSubModule.h"
#include "modules/android/LegacyHandlerMessage.h"

#include "framework/PolicyManager.h"
#include "framework/ThreadPoolManager.h"
#include "framework/SingleDispatchAndFamilyPairRestriction.h"

#include "telephony/ril.h"

using namespace std;

static char* convertString(string str) {
    int strlen = str.length();
    char *myPtr = new char[strlen + 1];
    strncpy(myPtr, str.c_str(), strlen);
    myPtr[strlen] = '\0';

    return myPtr;
}

static std::shared_ptr<RilRequestDialSyncMessage> createTestMsg() {
    RIL_Dial dial;
    RIL_UUS_Info uus;

    auto addressPtr = convertString("8589005271-");
    //string msgStr = "RIL_REQUEST_DIAL";
    dial.uusInfo = &uus;
    dial.address = addressPtr;
    dial.clir = 0;
    dial.uusInfo->uusType = RIL_UUS_TYPE1_IMPLICIT;
    dial.uusInfo->uusDcs = RIL_UUS_DCS_USP;
    dial.uusInfo->uusLength = 0;
    dial.uusInfo->uusData = nullptr;

    std::shared_ptr<RilRequestDialSyncMessage> shared_msg = make_shared<RilRequestDialSyncMessage>(sizeof dial, &dial);


    delete[] addressPtr;

    return shared_msg;
}

TEST(BasicFeatureSanity, TestRegisteredMessageProcessing) {
    std::shared_ptr<RilRequestDialSyncMessage> msgPtr = createTestMsg();
    std::shared_ptr<string> sync_shared_response;
    Message::Callback::Status status = msgPtr->dispatchSync(sync_shared_response);
    ASSERT_TRUE(status == Message::Callback::Status::SUCCESS);
}

TEST(BasicFeatureSanity, TestEqualityOfModemEndPoints) {
    std::shared_ptr<DmsModemEndPoint> endPoint1 =
        ModemEndPointFactory<DmsModemEndPoint>::getInstance().buildEndPoint();

    std::shared_ptr<DmsModemEndPoint> endPoint2 =
        ModemEndPointFactory<DmsModemEndPoint>::getInstance().buildEndPoint();

    EXPECT_EQ(endPoint1, endPoint2);
}

/* ToDo: update the test case with actual async requestSetup procedure.
 */

/* ToDo: update the test case with actual async requestSetup procedure.
 */

TEST(BasicFeatureSanity, TestUniquenessOfModemEndPoints) {
    std::shared_ptr<VoiceModemEndPoint> voiceModemPoint1 =
        ModemEndPointFactory<VoiceModemEndPoint>::getInstance().buildEndPoint();

    int initial_count = voiceModemPoint1.use_count();

    std::shared_ptr<VoiceModemEndPoint> voiceModemPoint2 =
        ModemEndPointFactory<VoiceModemEndPoint>::getInstance().buildEndPoint();
    /* Verify that local references to ModemEndPoint correctly increments and
     * refcount*/
    ASSERT_EQ(voiceModemPoint2.use_count(), initial_count+1);
}

TEST(BasicFeatureSanity, TestSingleDispatchAndFamilyPairRestriction) {
    class TestFamilyPair1Message : public SolicitedMessage<string> {
    public:
      inline TestFamilyPair1Message():
            SolicitedMessage<string>(REG_MSG("TEST_PAIR1_MESSAGE"))
      {
        mName = "TEST_PAIR1_MESSAGE";
      }

      inline ~TestFamilyPair1Message() {}
      inline string dump() override {
        return mName + "{}";
      }
    };

    class TestFamilyPair2Message : public SolicitedMessage<string> {
    public:
      inline TestFamilyPair2Message():
            SolicitedMessage<string>(REG_MSG("TEST_PAIR2_MESSAGE"))
      {
        mName = "TEST_PAIR2_MESSAGE";
      }

      inline ~TestFamilyPair2Message() {}
      inline string dump() override {
        return mName + "{}";
      }
    };

    class TestOtherMessage : public SolicitedMessage<string> {
    public:
      inline TestOtherMessage():
            SolicitedMessage<string>(REG_MSG("TEST_OTHER_MESSAGE"))
      {
        mName = "TEST_OTHER_MESSAGE";
      }

      inline ~TestOtherMessage() {}
      inline string dump() override {
        return mName + "{}";
      }
    };

    class SampleModule : public Module {
    public:
      inline SampleModule() {
        mName = "SampleModule";
        mNumPair1MsgCame = 0;
        mNumPair2MsgCame = 0;
        mNumOtherMsgsCame = 0;
        mNumNewMessagesUnRead = 0;
        mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);
        using std::placeholders::_1;
        mMessageHandler = {
          { REG_MSG("TEST_PAIR1_MESSAGE"),
            std::bind(&SampleModule::handlePair1Message, this, _1) },
          { REG_MSG("TEST_PAIR2_MESSAGE"),
            std::bind(&SampleModule::handlePair2Message, this, _1) },
          { REG_MSG("TEST_OTHER_MESSAGE"),
            std::bind(&SampleModule::handleOtherMessage, this, _1) },
        };
      }
      inline ~SampleModule() {}

      inline void init() {
        Module::init();
        std::vector<std::string> ring {
          "TEST_PAIR1_MESSAGE",
          "TEST_PAIR2_MESSAGE",
          "TEST_OTHER_MESSAGE",
        };
        auto rest = std::make_shared<SingleDispatchAndFamilyPairRestriction>();
        if (rest) {
          rest->setMessagePair("TEST_PAIR1_MESSAGE", "TEST_PAIR2_MESSAGE");
        }
        PolicyManager::getInstance().setMessageRestriction(ring, rest);
      }

#define HANLE_MESSAGE(msgCount) do { \
    { \
      usleep(5000); \
      std::unique_lock<std::mutex> lock(mLock); \
      mCv.wait_for(lock, std::chrono::seconds(2), [=] { return mNumNewMessagesUnRead == 0; }); \
      mNumNewMessagesUnRead++; \
      msgCount++; \
    } \
    mCv.notify_all(); \
  } while (0)

#define CONSUME_MESSAGE() do { \
    { \
      std::unique_lock<std::mutex> lock(mLock); \
      mCv.wait_for(lock, std::chrono::seconds(2), [=] {return mNumNewMessagesUnRead > 0; }); \
      mNumNewMessagesUnRead--; \
    } \
    mCv.notify_all(); \
  } while (0)

      inline void handlePair1Message(std::shared_ptr<Message> msg) {
        (void)msg;
        HANLE_MESSAGE(mNumPair1MsgCame);
      }
      inline void handlePair2Message(std::shared_ptr<Message> msg) {
        (void)msg;
        HANLE_MESSAGE(mNumPair2MsgCame);
      }
      inline void handleOtherMessage(std::shared_ptr<Message> msg) {
        (void)msg;
        HANLE_MESSAGE(mNumOtherMsgsCame);
      }

      inline int WaitNextAndGetNumbersOfPair1Message() {
        CONSUME_MESSAGE();
        return mNumPair1MsgCame;
      }
      inline int WaitNextAndGetNumbersOfPair2Message() {
        CONSUME_MESSAGE();
        return mNumPair2MsgCame;
      }
      inline int WaitNextAndGetNumbersOfOtherMessages() {
        CONSUME_MESSAGE();
        return mNumOtherMsgsCame;
      }
    private:
      std::condition_variable mCv;
      std::mutex              mLock;
      int                     mNumPair1MsgCame;
      int                     mNumPair2MsgCame;
      int                     mNumOtherMsgsCame;
      int                     mNumNewMessagesUnRead;
    }; // end of class definition

    SampleModule *module = new SampleModule();
    module->init();

    auto pair1Msg = std::make_shared<TestFamilyPair1Message>();
    auto pair2Msg = std::make_shared<TestFamilyPair2Message>();
    auto otherMsg = std::make_shared<TestOtherMessage>();

    // no callbacks, the informMessageFinished will be called after handler completed
    // dispatcher order: pair1, other, pair1, pair2, other, pair2
    pair1Msg->dispatch();
    otherMsg->dispatch();
    pair1Msg->dispatch();
    pair2Msg->dispatch();
    otherMsg->dispatch();
    pair2Msg->dispatch();

    // executed order should be: pair1, pair2, pair1, pair2, other, other
    ASSERT_EQ(1, module->WaitNextAndGetNumbersOfPair1Message());
    ASSERT_EQ(1, module->WaitNextAndGetNumbersOfPair2Message());
    ASSERT_EQ(2, module->WaitNextAndGetNumbersOfPair1Message());
    ASSERT_EQ(2, module->WaitNextAndGetNumbersOfPair2Message());
    ASSERT_EQ(1, module->WaitNextAndGetNumbersOfOtherMessages());
    ASSERT_EQ(2, module->WaitNextAndGetNumbersOfOtherMessages());

    delete module;
    module = nullptr;
}

TEST(BasicFeatureSanity, TestLegacyHandlerSubModule) {
    std::thread::id payload1 = std::this_thread::get_id();
    std::thread::id payload2 = std::this_thread::get_id();
    qcril_request_params_type params1 = {QCRIL_DEFAULT_INSTANCE_ID,
                                        QCRIL_DEFAULT_MODEM_ID,
                                        0, nullptr, &payload1, sizeof(payload1),
                                        0, nullptr};
    qcril_request_params_type params2 = {QCRIL_DEFAULT_INSTANCE_ID,
                                        QCRIL_DEFAULT_MODEM_ID,
                                        0, nullptr, &payload2, sizeof(payload2),
                                        0, nullptr};

    // could not define function inside GTEST... use functor without capture list
    void (*test_legacy_handler)(const qcril_request_params_type *const, qcril_request_return_type *const) =
        [](const qcril_request_params_type *const params_ptr, qcril_request_return_type *const ret_ptr) {
            (void)ret_ptr;
            if (params_ptr && params_ptr->data)
              *((std::thread::id*)params_ptr->data) = std::this_thread::get_id();
        };

    // message1 with dedicated thread
    LegacyHandlerSubModule *subModule1= new LegacyHandlerSubModule();
    subModule1->init();
    // true means clean up the subModule after complete
    auto subMessage1 = std::make_shared<LegacyHandlerMessage>(&params1,
        test_legacy_handler, subModule1, true);
    subModule1->dispatch(subMessage1);

    // message2 with dedicated thread
    LegacyHandlerSubModule *subModule2= new LegacyHandlerSubModule();
    subModule2->init();
    // true means clean up the subModule after complete
    auto subMessage2 = std::make_shared<LegacyHandlerMessage>(&params2,
        test_legacy_handler, subModule2, true);
    subModule2->dispatch(subMessage2);

    threadSleep(2);

    // handler got executed
    ASSERT_NE(payload1, std::this_thread::get_id());
    ASSERT_NE(payload2, std::this_thread::get_id());
    // the handlers are in different thread
    ASSERT_NE(payload1, payload2);
    // dedicated thread
    //ASSERT_NE(payload1, Dispatcher::getInstance().getDispatcherLooperThreadId());
    //ASSERT_NE(payload2, Dispatcher::getInstance().getDispatcherLooperThreadId());
}

TEST(BasicFeatureSanity, TestThreadPool) {
    std::thread::id payload1 = std::this_thread::get_id();
    std::thread::id payload2 = std::this_thread::get_id();
    // define the callback
    void (*func_cb1)(void* cb_data) = [](void* cb_data) {
        if (cb_data)
            *((std::thread::id*)cb_data) = std::this_thread::get_id();
        threadSleep(2);
    };
    void (*func_cb2)(void* cb_data) = [](void* cb_data) {
        if (cb_data)
            *((std::thread::id*)cb_data) = std::this_thread::get_id();
        threadSleep(4);
    };

    (void)ThreadPoolManager::getInstance().scheduleExecution(func_cb1, &payload1);
    (void)ThreadPoolManager::getInstance().scheduleExecution(func_cb2, &payload2);
    threadSleep(1);

    // handlers get executed and they are different threads for cb1 and cb2
    ASSERT_NE(payload1, std::this_thread::get_id());
    ASSERT_NE(payload2, std::this_thread::get_id());
    ASSERT_NE(payload1, payload2);
    // should be 2 threads totally, and no idle thread
    ASSERT_EQ(ThreadPoolManager::getInstance().getIdleModule(), nullptr);
    ASSERT_EQ(ThreadPoolManager::getInstance().getTotalThreads(), 2);

    threadSleep(2);
    // should be one idle thread and not freed
    ASSERT_NE(ThreadPoolManager::getInstance().getIdleModule(), nullptr);
    ASSERT_EQ(ThreadPoolManager::getInstance().getTotalThreads(), 2);

    // No task active. Only Idle thread left
    threadSleep(2);
    ASSERT_NE(ThreadPoolManager::getInstance().getIdleModule(), nullptr);
    ASSERT_EQ(ThreadPoolManager::getInstance().getTotalThreads(), 1);

    // When enqueue one new task, the idle thread should be used directly
    (void)ThreadPoolManager::getInstance().scheduleExecution(func_cb1, &payload1);
    threadSleep(1);
    ASSERT_EQ(ThreadPoolManager::getInstance().getIdleModule(), nullptr);
    ASSERT_EQ(ThreadPoolManager::getInstance().getTotalThreads(), 1);

    // test clean up threads
    (void)ThreadPoolManager::getInstance().scheduleExecution(func_cb2, &payload2);
    ThreadPoolManager::getInstance().cleanupThreads();
    ASSERT_EQ(ThreadPoolManager::getInstance().getIdleModule(), nullptr);
    ASSERT_EQ(ThreadPoolManager::getInstance().getTotalThreads(), 0);
}

extern "C" void *qcril_malloc_adv ( size_t size, const char* func_name, int line_num);

TEST(LegacyQcril, TestQcrilMalloc) {
    char *buffer = (char *)qcril_malloc(10);
    ASSERT_TRUE(buffer != nullptr);
}
#endif
