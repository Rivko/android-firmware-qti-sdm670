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
#include "framework/TimeKeeper.h"
#include "framework/ModuleLooper.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "framework/add_message_id.h"
#include "telephony/ril.h"

using namespace std;

/* Verify that dispatcher properly removes all the references to the module upon module
  destructor..
*/

TEST(FrameworkUnitTest, VerifyThatSingleModuleUnregistrationIsClean) {

    class SampleModule : public Module {
    public:
        inline SampleModule() {
            mName = "SampleModule";
            using std::placeholders::_1;
            mMessageHandler = {
                { REG_MSG("TEST_MSG_1"),
                std::bind(&SampleModule::handleSampleMessage, this, _1) },
            };

        }
        inline ~SampleModule() {}
        inline void handleSampleMessage(std::shared_ptr<Message> msg) {}
    };

    int initialRegisteredModulesCount = Dispatcher::getInstance().getRegisteredModulesCount();

    SampleModule *myModule = new SampleModule();
    myModule->init();

    int registeredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
    int registeredModulesCount = Dispatcher::getInstance().getRegisteredModulesCount();

    ASSERT_EQ(initialRegisteredModulesCount+1, registeredModulesCount);

    /* Clean up the module and verify the dispatcher book keeping*/
    delete myModule;
    myModule = nullptr;

    registeredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
    registeredModulesCount = Dispatcher::getInstance().getRegisteredModulesCount();

    ASSERT_EQ(initialRegisteredModulesCount, registeredModulesCount);

}

/* Verify that dispatcher does not records duplicate messages from
   multiple subscribers for same message
*/
TEST(FrameworkUnitTest, VerifyThatDuplicateMessageRegistrationIsClean) {
    class SampleModule1 : public Module {
    public:
        inline SampleModule1() {
            mName = "SampleModule";
            using std::placeholders::_1;
            mMessageHandler = {
                { REG_MSG("BROADCAST_MSG_1"),
                std::bind(&SampleModule1::handleSampleMessage, this, _1) },
            };

        }
        inline ~SampleModule1() {}
        inline void handleSampleMessage(std::shared_ptr<Message> msg) {}
    };

    class SampleModule2 : public Module {
    public:
        inline SampleModule2() {
            mName = "SampleModule";
            using std::placeholders::_1;
            mMessageHandler = {
                { REG_MSG("BROADCAST_MSG_1"),
                std::bind(&SampleModule2::handleSampleMessage, this, _1) },
            };

        }
        inline ~SampleModule2() {}
        inline void handleSampleMessage(std::shared_ptr<Message> msg) {}
    };
    int initialRegisteredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
    int initialRegisteredModulesCount = Dispatcher::getInstance ().getRegisteredModulesCount ();

    SampleModule1 myModule1;
    myModule1.init();

    SampleModule2 myModule2;
    myModule2.init();


    int registeredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
    int registeredModulesCount = Dispatcher::getInstance().getRegisteredModulesCount();

    ASSERT_EQ(registeredMsgCount, initialRegisteredMsgCount+1);
    ASSERT_EQ(registeredModulesCount, initialRegisteredModulesCount+2);
}

//TODO_REARCH: change module_list from vector to a set for below test case to pass.
///* Verify that dispatcher does not records duplicate messages and modules for same message and same module*/
//TEST(FrameworkUnitTest, VerifyThatDoubleInitByModuleIsClean) {
//    class SampleModule1 : public Module {
//    public:
//        inline SampleModule1() {
//            mName = "SampleModule";
//            using std::placeholders::_1;
//            mMessageHandler = {
//                { REG_MSG("BROADCAST_MSG_2"),
//                std::bind(&SampleModule1::handleSampleMessage, this, _1) },
//            };
//
//        }
//        inline ~SampleModule1() {}
//        inline void handleSampleMessage(std::shared_ptr<Message> msg) {}
//    };
//
//    int initialRegisteredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
//    int initialRegisteredModulesCount = Dispatcher::getInstance ().getRegisteredModulesCount ();
//
//    SampleModule1 myModule1;
//    myModule1.init();
//
//    myModule1.init();
//
//    int registeredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
//    int registeredModulesCount = Dispatcher::getInstance().getRegisteredModulesCount();
//
//    ASSERT_EQ(initialRegisteredMsgCount+1, registeredMsgCount);
//    ASSERT_EQ(initialRegisteredModulesCount+1, registeredModulesCount);
//
//}

TEST(FrameworkUnitTest, VerifyThatBroadcastReceiverModulesUnregistrationIsClean) {

    class SampleModule1 : public Module {
    public:
        inline SampleModule1() {
            mName = "SampleModule";
            using std::placeholders::_1;
            mMessageHandler = {
                { REG_MSG("BROADCAST_MSG_3"),
                std::bind(&SampleModule1::handleSampleMessage, this, _1) },
            };

        }
        inline ~SampleModule1() {}
        inline void handleSampleMessage(std::shared_ptr<Message> msg) {}
    };

    class SampleModule2 : public Module {
    public:
        inline SampleModule2() {
            mName = "SampleModule";
            using std::placeholders::_1;
            mMessageHandler = {
                { REG_MSG("BROADCAST_MSG_3"),
                std::bind(&SampleModule2::handleSampleMessage, this, _1) },
            };

        }
        inline ~SampleModule2() {}
        inline void handleSampleMessage(std::shared_ptr<Message> msg) {}
    };

    int initialRegisteredMsgCount = Dispatcher::getInstance ().getRegisteredMessagesCount ();
    int initialRegisteredModulesCount = Dispatcher::getInstance ().getRegisteredModulesCount ();

    SampleModule1* myModule1 = new SampleModule1();
    myModule1->init();

    SampleModule2* myModule2 = new SampleModule2();
    myModule2->init();

    int registeredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
    int registeredModulesCount = Dispatcher::getInstance().getRegisteredModulesCount();

    /* Verify that a duplicate message results in 2 records for modules but 1 for message.*/
    ASSERT_EQ(initialRegisteredMsgCount+1, registeredMsgCount );
    ASSERT_EQ(initialRegisteredModulesCount +2, registeredModulesCount);

    delete myModule1;
    myModule1 = nullptr;
    registeredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
    registeredModulesCount = Dispatcher::getInstance().getRegisteredModulesCount();

    ASSERT_EQ(initialRegisteredMsgCount+1, registeredMsgCount);
    ASSERT_EQ(initialRegisteredModulesCount+1, registeredModulesCount);

    delete myModule2;
    myModule2 = nullptr;
    registeredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
    registeredModulesCount = Dispatcher::getInstance().getRegisteredModulesCount();

    ASSERT_EQ(initialRegisteredMsgCount+1, registeredMsgCount);
    ASSERT_EQ(initialRegisteredModulesCount, registeredModulesCount);
}

TEST(FrameworkUnitTest, VerifyThatNonBroadcastReceiverModulesUnregistrationIsClean) {

    class SampleModule1 : public Module {
    public:
        inline SampleModule1() {
            mName = "SampleModule";
            using std::placeholders::_1;
            mMessageHandler = {
                { REG_MSG("TEST_MSG_2"),
                std::bind(&SampleModule1::handleSampleMessage, this, _1) },
            };

        }
        inline ~SampleModule1() {}
        inline void handleSampleMessage(std::shared_ptr<Message> msg) {}
    };

    class SampleModule2 : public Module {
    public:
        inline SampleModule2() {
            mName = "SampleModule";
            using std::placeholders::_1;
            mMessageHandler = {
                { REG_MSG("TEST_MSG_3"),
                std::bind(&SampleModule2::handleSampleMessage, this, _1) },
            };

        }
        inline ~SampleModule2() {}
        inline void handleSampleMessage(std::shared_ptr<Message> msg) {}
    };

    int initialRegisteredMsgCount = Dispatcher::getInstance ().getRegisteredMessagesCount ();
    int initialRegisteredModulesCount = Dispatcher::getInstance ().getRegisteredModulesCount ();

    SampleModule1* myModule1 = new SampleModule1();
    myModule1->init();

    SampleModule2* myModule2 = new SampleModule2();
    myModule2->init();

    int registeredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
    int registeredModulesCount = Dispatcher::getInstance().getRegisteredModulesCount();

    /* Verify that 2 unique message results in 2 records in dispatcher*/
    ASSERT_EQ(initialRegisteredMsgCount+2, registeredMsgCount);
    ASSERT_EQ(initialRegisteredModulesCount+2, registeredModulesCount);

    delete myModule1;
    myModule1 = nullptr;
    registeredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
    registeredModulesCount = Dispatcher::getInstance().getRegisteredModulesCount();

    ASSERT_EQ(initialRegisteredMsgCount+2, registeredMsgCount);
    ASSERT_EQ(initialRegisteredModulesCount+1, registeredModulesCount);

    delete myModule2;
    myModule2 = nullptr;
    registeredMsgCount = Dispatcher::getInstance().getRegisteredMessagesCount();
    registeredModulesCount = Dispatcher::getInstance().getRegisteredModulesCount();

    ASSERT_EQ(initialRegisteredMsgCount+2, registeredMsgCount);
    ASSERT_EQ(initialRegisteredModulesCount, registeredModulesCount);
}

TEST(FrameworkUnitTest, VerifyThatModemEndPointRefCoutingIsCorrect) {
    class SampleModemEndPoint : public ModemEndPoint {
    public:
        SampleModemEndPoint() :ModemEndPoint("Sample") {
        }
        ~SampleModemEndPoint() {
        }

    };

    std::shared_ptr<SampleModemEndPoint> sampleEndPoint =
        ModemEndPointFactory<SampleModemEndPoint>::getInstance().buildEndPoint();
    ASSERT_FALSE(sampleEndPoint == nullptr);

    ASSERT_EQ(2, sampleEndPoint.use_count());
    ASSERT_EQ(ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount(), sampleEndPoint.use_count());

    sampleEndPoint = nullptr;
    ASSERT_EQ(1, ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount());

    ModemEndPointFactory<SampleModemEndPoint>::getInstance().reset();
    ASSERT_EQ(0, ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount());

}

TEST(FrameworkUnitTest, VerifyThatModemEndPointAreUnique) {
    class SampleModemEndPoint : public ModemEndPoint {
    public:
        SampleModemEndPoint() :ModemEndPoint("Sample") {
        }
        ~SampleModemEndPoint() {
        }

    };

    std::shared_ptr<SampleModemEndPoint> sampleEndPoint1 =
        ModemEndPointFactory<SampleModemEndPoint>::getInstance().buildEndPoint();
    ASSERT_FALSE(sampleEndPoint1 == nullptr);

    ASSERT_EQ(2, sampleEndPoint1.use_count());
    ASSERT_EQ(ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount(), sampleEndPoint1.use_count());

    /* A new instance/reference should result in ref-count increment*/
    std::shared_ptr<SampleModemEndPoint> sampleEndPoint2 =
        ModemEndPointFactory<SampleModemEndPoint>::getInstance().buildEndPoint();
    ASSERT_FALSE(sampleEndPoint2 == nullptr);
    ASSERT_EQ(3, sampleEndPoint2.use_count());
    ASSERT_EQ(sampleEndPoint1.use_count(), sampleEndPoint2.use_count());
    ASSERT_EQ(ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount(), sampleEndPoint2.use_count());


    /* Removal of existing refrences should result in decrement of ref-count*/
    sampleEndPoint1 = nullptr;
    ASSERT_EQ(2, ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount());

    sampleEndPoint2 = nullptr;
    ASSERT_EQ(1, ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount());


    ModemEndPointFactory<SampleModemEndPoint>::getInstance().reset();
    ASSERT_EQ(0, ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount());
}

TEST(FrameworkUnitTest, DISABLED_VerifySolicitedMessageDispatch) {
  class SampleSolicitedMessage : public SolicitedMessage<string> {
  public:
    int mRandomData;
    SampleSolicitedMessage() :
      mRandomData{ 911 },
      SolicitedMessage<string>(Dispatcher::getInstance().registerMessage("TEST_SOLICITED_MSG_1")) {
          mName = "TEST_SOLICITED_MSG_1";
    }

    inline string dump() override {
      return mName + "{}";
    }
  };

  class SampleCallback : public GenericCallback<string> {
  private:
  public:
    inline SampleCallback(string clientToken)
      : GenericCallback(clientToken) {};
    ~SampleCallback() {}
    inline void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
      std::shared_ptr<string> responseDataPtr) override {
      std::shared_ptr<SampleSolicitedMessage> shared_msg(
        std::static_pointer_cast<SampleSolicitedMessage>(solicitedMsg));

      ASSERT_EQ(911, shared_msg->mRandomData);
      ASSERT_EQ(status, Message::Callback::Status::SUCCESS);
      ASSERT_TRUE(*responseDataPtr == "SAMPLE-RESPONSE");
    }
    inline Message::Callback *clone() override {
      SampleCallback *clone = new SampleCallback(mClientToken);
      return clone;
    }
  };

  class SampleModule : public Module {
  public:
    inline SampleModule() {
      mName = "SampleModule";
      using std::placeholders::_1;
      mMessageHandler = {
        { REG_MSG("TEST_SOLICITED_MSG_1"),
        std::bind(&SampleModule::handleSampleMessage, this, _1) },
      };

    }
    inline ~SampleModule() {}
    inline void handleSampleMessage(std::shared_ptr<Message> msg) {
      std::shared_ptr<SampleSolicitedMessage> shared_msg(
        std::static_pointer_cast<SampleSolicitedMessage>(msg));

      ASSERT_EQ(911, shared_msg->mRandomData);

      string *sampleResponse = new string("SAMPLE-RESPONSE");
      std::shared_ptr<string> shared_pResponse(sampleResponse);
      shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
        shared_pResponse);
    }
  };



  SampleModule *myModule = new SampleModule();
  myModule->init();

  auto sampleMsgPtr = std::make_shared<SampleSolicitedMessage>();
  SampleCallback sampleCb("MyToken");
  sampleMsgPtr->setCallback(&sampleCb);
  sampleMsgPtr->dispatch();

  /* Ofcourse, client has to wait...!*/
  threadSleep(1);

  delete myModule;
  myModule = nullptr;
}

//TEST(FrameworkUnitTest, VerifyUnSolicitedMessageDispatch) {
//  class SampleUnSolicitedMessage : public UnSolicitedMessage {
//  public:
//    int mRandomData;
//    SampleUnSolicitedMessage() :
//      UnSolicitedMessage(REG_MSG("TEST_UNSOLICITED_MSG_1")),
//      mRandomData{ 911 } {
//    }
//
//    std::shared_ptr<UnSolicitedMessage> clone() {
//        return (std::make_shared<SampleUnSolicitedMessage>());
//    }
//
//    inline string dump() override {
//      return mName + "{}";
//    }
//  };
//
//  class SampleModule : public Module {
//  public:
//    inline SampleModule() {
//      mName = "SampleModule";
//      using std::placeholders::_1;
//      mMessageHandler = {
//        { REG_MSG("TEST_UNSOLICITED_MSG_1"),
//        std::bind(&SampleModule::handleSampleMessage, this, _1) },
//      };
//
//    }
//    inline ~SampleModule() {}
//    inline void handleSampleMessage(std::shared_ptr<Message> msg) {
//      std::shared_ptr<SampleUnSolicitedMessage> shared_msg(
//        std::static_pointer_cast<SampleUnSolicitedMessage>(msg));
//      ASSERT_EQ(shared_msg->mRandomData, 911);
//    }
//  };
//
//
//
//  SampleModule *myModule = new SampleModule();
//  myModule->init();
//
//  std::shared_ptr<SampleUnSolicitedMessage> sampleMsgPtr = std::make_shared<SampleUnSolicitedMessage>();
//  sampleMsgPtr->broadcast();
//
//  /* Ofcourse, client has to wait...!*/
//  threadSleep(1);
//
//  delete myModule;
//  myModule = nullptr;
//}

TEST(FrameworkUnitTest, VerifySolicitedSyncMessageDispatch) {
  class SampleSolicitedSyncMessage : public SolicitedSyncMessage<string>,
                                  public add_message_id<SampleSolicitedSyncMessage> {
  public:
    int mRandomData;
    SampleSolicitedSyncMessage()
      :mRandomData{ 911 },
      SolicitedSyncMessage<string>(Dispatcher::getInstance().registerMessage("TEST_SOLICITED_SYNC_MSG_1")) {
          mName = "TEST_SOLICITED_SYNC_MSG_1";
      }

    inline string dump() override {
      return mName + "{}";
    }
  };

  class SampleModule : public Module {
  public:
    inline SampleModule() {
      mName = "SampleModule";
      using std::placeholders::_1;
      mMessageHandler = {
        { REG_MSG("TEST_SOLICITED_SYNC_MSG_1"),
        std::bind(&SampleModule::handleSampleMessage, this, _1) },
      };

    }

    inline ~SampleModule() {}

    inline void handleSampleMessage(std::shared_ptr<Message> msg) {
      std::shared_ptr<SampleSolicitedSyncMessage> shared_msg(
        std::static_pointer_cast<SampleSolicitedSyncMessage>(msg));

      string *sampleResponse = new string("SAMPLE-RESPONSE");
      std::shared_ptr<string> shared_pResponse(sampleResponse);
      shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
        shared_pResponse);
    }
  };

  SampleModule *myModule = new SampleModule();
  myModule->init();

  std::shared_ptr<SampleSolicitedSyncMessage> sampleMsgPtr = std::make_shared<SampleSolicitedSyncMessage>();

  std::shared_ptr<string> sync_shared_response;
  Message::Callback::Status status = sampleMsgPtr->dispatchSync(sync_shared_response);
  ASSERT_TRUE(status == Message::Callback::Status::SUCCESS);
  ASSERT_TRUE(*sync_shared_response == "SAMPLE-RESPONSE");

  delete myModule;
  myModule = nullptr;
}

TEST(FrameworkUnitTest, VerifySyncMessageHandlingWhenHandlerDidNotCallSendResponse) {
  class SampleSolicitedSyncMessage : public SolicitedSyncMessage<string>,
                                  public add_message_id<SampleSolicitedSyncMessage> {
  public:
    int mRandomData;
    SampleSolicitedSyncMessage()
      :mRandomData{ 911 },
      SolicitedSyncMessage<string>(REG_MSG("TEST_SOLICITED_SYNC_MSG_2")) {
          mName = "TEST_SOLICITED_SYNC_MSG_2";
      }

    inline string dump() override {
      return mName + "{}";
    }
  };

  class SampleModule : public Module {
  public:
    inline SampleModule() {
      mName = "SampleModule";
      using std::placeholders::_1;
      mMessageHandler = {
        { REG_MSG("TEST_SOLICITED_SYNC_MSG_2"),
        std::bind(&SampleModule::handleSampleMessage, this, _1) },
      };

    }

    inline ~SampleModule() {}

    inline void handleSampleMessage(std::shared_ptr<Message> msg) {
      std::shared_ptr<SampleSolicitedSyncMessage> shared_msg(
        std::static_pointer_cast<SampleSolicitedSyncMessage>(msg));

      /* This handler deliberately does not call sendResponse() */
      //string *sampleResponse = new string("SAMPLE-RESPONSE");
      //std::shared_ptr<string> shared_pResponse(sampleResponse);
      //shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
      //  shared_pResponse);
    }
  };

  SampleModule *myModule = new SampleModule();
  myModule->init();

  std::shared_ptr<SampleSolicitedSyncMessage> sampleMsgPtr = std::make_shared<SampleSolicitedSyncMessage>();

  std::shared_ptr<string> sync_shared_response;
  Message::Callback::Status status = sampleMsgPtr->dispatchSync(sync_shared_response);
  ASSERT_EQ(status, Message::Callback::Status::TIMEOUT);

  delete myModule;
  myModule = nullptr;
}

TEST(FrameworkUnitTest, VerifySyncMessageHandlingWhenModuleDelegationTookLongTime) {
  /* This is the syncMesg1 sent by test to Module1.*/
  class SampleSolicitedSyncMessage : public SolicitedSyncMessage<string>,
                                  public add_message_id<SampleSolicitedSyncMessage> {
  public:
    int mRandomData;
    SampleSolicitedSyncMessage()
      :mRandomData{ 100 },
      SolicitedSyncMessage<string>(REG_MSG("TEST_SOLICITED_SYNC_MSG_3")) {
          mName = "TEST_SOLICITED_SYNC_MSG_3";
      }

    inline string dump() override {
      return mName + "{}";
    }
  };

  /* Module1 would delegate and send below msg(asyncMesg2) to Module2. This msg is async.
     Note that Module1 wants to call sendResponse() only after it gets to heard
     from Module2.*/
  class SampleSolicitedMessage : public SolicitedMessage<string>,
                                  public add_message_id<SampleSolicitedMessage> {
  public:
    int mRandomData;
    SampleSolicitedMessage()
      :mRandomData{ 200 },
      SolicitedMessage<string>(REG_MSG("TEST_SOLICITED_MSG_2")) {
          mName = "TEST_SOLICITED_MSG_2";
      }

    inline string dump() override {
      return mName + "{}";
    }
  };

  /* This is the callback associated with asyncMsg2. Note that we call sendResponse for syncMsg1
     here.*/
  class SampleCallback : public GenericCallback<string> {
  private:
    std::shared_ptr<SampleSolicitedSyncMessage> mClientMsg;
  public:
    inline SampleCallback(string clientToken, std::shared_ptr<SampleSolicitedSyncMessage> originalMsg)
      : GenericCallback(clientToken) {
      mClientMsg = originalMsg;
    };
    ~SampleCallback() {}
    inline void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
      std::shared_ptr<string> responseDataPtr) override {
      std::shared_ptr<SampleSolicitedMessage> shared_msg(
        std::static_pointer_cast<SampleSolicitedMessage>(solicitedMsg));

      ASSERT_EQ(shared_msg->mRandomData, 200);
      ASSERT_EQ(Message::Callback::Status::SUCCESS, status);
      ASSERT_EQ("SAMPLE-RESPONSE-FROM-MODULE-2", *responseDataPtr);

      /* Responding back to client now. Delegation completed.*/
      string *sampleResponse = new string("SAMPLE-RESPONSE");
      std::shared_ptr<string> shared_pResponse(sampleResponse);
      Log::getInstance().d("Responding to msg = " + mClientMsg->to_string());
      mClientMsg->sendResponse(mClientMsg, Message::Callback::Status::SUCCESS,
        shared_pResponse);

    }
    inline Message::Callback *clone() override {
      SampleCallback *clone = new SampleCallback(mClientToken, mClientMsg);
      return clone;
    }
  };

  /* Module1 delegating to module2*/
  class SampleModule1 : public Module {
  public:
    inline SampleModule1() {
      mName = "SampleModule";
      using std::placeholders::_1;
      mMessageHandler = {
        { REG_MSG("TEST_SOLICITED_SYNC_MSG_3"),
        std::bind(&SampleModule1::handleSampleMessage, this, _1) },
      };

    }

    inline ~SampleModule1() {}

    inline void handleSampleMessage(std::shared_ptr<Message> msg) {
      std::shared_ptr<SampleSolicitedSyncMessage> shared_msg(
        std::static_pointer_cast<SampleSolicitedSyncMessage>(msg));

      std::shared_ptr<SampleSolicitedMessage> delegateMsgPtr = std::make_shared<SampleSolicitedMessage>();
      SampleCallback cb("Delegate-token", shared_msg);
      delegateMsgPtr->setCallback(&cb);
      delegateMsgPtr->dispatch();
      //shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS, std::make_shared<string>("solicited-response"));
    }
  };

  /* Module2 responding back to Module1*/
  class SampleModule2 : public Module {
  public:
    inline SampleModule2() {
      mName = "SampleModule2";
      using std::placeholders::_1;
      mMessageHandler = {
        { REG_MSG("TEST_SOLICITED_MSG_2"),
        std::bind(&SampleModule2::handleSampleMessage, this, _1) },
      };

    }
    inline ~SampleModule2() {}
    inline void handleSampleMessage(std::shared_ptr<Message> msg) {
      std::shared_ptr<SampleSolicitedMessage> shared_msg(
        std::static_pointer_cast<SampleSolicitedMessage>(msg));

      ASSERT_EQ(shared_msg->mRandomData, 200);

      string *sampleResponse = new string("SAMPLE-RESPONSE-FROM-MODULE-2");
      std::shared_ptr<string> shared_pResponse(sampleResponse);
      shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
        shared_pResponse);
    }
  };

  SampleModule1 *myModule1 = new SampleModule1();
  myModule1->init();
  SampleModule2 *myModule2 = new SampleModule2();
  myModule2->init();

  std::shared_ptr<SampleSolicitedSyncMessage> sampleMsgPtr = std::make_shared<SampleSolicitedSyncMessage>();

  std::shared_ptr<string> sync_shared_response;
  Message::Callback::Status status = sampleMsgPtr->dispatchSync(sync_shared_response);
  ASSERT_EQ(Message::Callback::Status::SUCCESS, status);

  Log::getInstance().d("Waiting 40 seconds...!");
  threadSleep(40);
  delete myModule1;
  myModule1 = nullptr;

  delete myModule2;
  myModule2 = nullptr;

}

TEST(FrameworkUnitTest, VerifySetTimer) {

    bool cb_executed = false;
    int delay_sec = 1;
    long delay_ms = delay_sec * 1000;
    auto my_cb = [&cb_executed] (void* user_data) {Log::getInstance().d("lambda executed"); cb_executed = true;};
    TimeKeeper::timer_id timer1 = TimeKeeper::getInstance ().set_timer(my_cb, nullptr, delay_ms);

    threadSleep(delay_sec +1);
    ASSERT_TRUE(cb_executed);
}

TEST(FrameworkUnitTest, VerifyShortestTimerExpiresFirst) {

    bool shortest_cb_executed = false;
    bool longest_cb_executed = false;
    auto my_cb = [] (void* user_data) {Log::getInstance().d("normal lambda executed");};
    auto my_shortest_timer_cb = [&shortest_cb_executed] (void* user_data) {Log::getInstance().d("shortest lambda executed"); shortest_cb_executed = true;};
    auto my_longest_timer_cb = [&longest_cb_executed] (void* user_data) {Log::getInstance().d("longest lambda executed"); longest_cb_executed = true;};


    TimeKeeper::timer_id timer1 = TimeKeeper::getInstance ().set_timer(my_longest_timer_cb, nullptr, 5 * 1000);
    TimeKeeper::timer_id timer2 = TimeKeeper::getInstance ().set_timer(my_cb, nullptr, 4 * 1000);
    TimeKeeper::timer_id timer3 = TimeKeeper::getInstance ().set_timer(my_cb, nullptr, 3 * 1000);
    TimeKeeper::timer_id timer4 = TimeKeeper::getInstance ().set_timer(my_cb, nullptr, 2 * 1000);
    TimeKeeper::timer_id timer5 = TimeKeeper::getInstance ().set_timer(my_shortest_timer_cb, nullptr, 1 * 1000);


    /* sleep for the shortest delay */
    threadSleep(1 + 1);
    /* Verify if shortest delay callback is executed */
    ASSERT_TRUE(shortest_cb_executed);

    /* Sleep for longest delay - delay already slept before */
    threadSleep( 5 - 2 + 1);

    /* Verify if longest delay callback is executed */
    ASSERT_TRUE(longest_cb_executed);
}

TEST(FrameworkUnitTest, VerifyCancellationOfTimer) {

    bool shortest_cb_executed = false;
    bool longest_cb_executed = false;
    bool cancellable_cb_executed = false;
    auto my_cb = [] (void* user_data) {Log::getInstance().d("normal lambda executed");};
    auto my_shortest_timer_cb = [&shortest_cb_executed] (void* user_data) {Log::getInstance().d("shortest lambda executed"); shortest_cb_executed = true;};
    auto my_longest_timer_cb = [&longest_cb_executed] (void* user_data) {Log::getInstance().d("longest lambda executed"); longest_cb_executed = true;};
    auto cancellable_cb = [&cancellable_cb_executed] (void* user_data) {Log::getInstance().d("cancellable lambda executed"); cancellable_cb_executed = true;};


    TimeKeeper::timer_id timer1 = TimeKeeper::getInstance ().set_timer(my_longest_timer_cb, nullptr, 5 * 1000);
    TimeKeeper::timer_id timer2 = TimeKeeper::getInstance ().set_timer(my_cb, nullptr, 4 * 1000);
    TimeKeeper::timer_id timer3 = TimeKeeper::getInstance ().set_timer(cancellable_cb, nullptr, 3 * 1000);
    TimeKeeper::timer_id timer4 = TimeKeeper::getInstance ().set_timer(my_cb, nullptr, 2 * 1000);
    TimeKeeper::timer_id timer5 = TimeKeeper::getInstance ().set_timer(my_shortest_timer_cb, nullptr, 1 * 1000);


    /* sleep for the shortest delay */
    threadSleep(1 + 1);
    /* Verify that shortest delay callback is executed */
    ASSERT_TRUE(shortest_cb_executed);

    /* Cancel one of the timer */
    TimeKeeper::getInstance().clear_timer(timer3);

    /* Sleep for longest delay - delay already slept before */
    threadSleep( 5 - 2);

    /* Verify that longest delay callback is executed */
    ASSERT_TRUE(longest_cb_executed);

    /* Verify that cancelled timer's callback was not executed  */
    ASSERT_FALSE(cancellable_cb_executed);
}

TEST(FrameworkUnitTest, VerifyNonLooperModuleDispatchIsHandledInDispatcherContext) {
    class P2PMessage : public SolicitedMessage<string> {
    public:
      int mRandomData;
      P2PMessage() :
        mRandomData{ 911 },
        SolicitedMessage<string>(Dispatcher::getInstance().registerMessage("TEST_PEER_2_PEER_MSG_1")) {
            mName = "TEST_PEER_2_PEER_MSG_1";
      }

      inline string dump() override {
        return mName + "{}";
      }
    };

    enum class HandlerExecutionStatus {
      NOT_INVOKED,
      INVOKED_IN_DISPATCHER_CONTEXT,
      NOT_INVOKED_IN_DISPATCHER_CONTEXT
    };

    class NonLooperSampleModule : public Module {
    private:
      HandlerExecutionStatus mFlag;
    public:
      inline NonLooperSampleModule() {
        mFlag = HandlerExecutionStatus::NOT_INVOKED;
        mName = "NonLooperSampleModule";
        using std::placeholders::_1;
        mMessageHandler = {
          { REG_MSG("TEST_PEER_2_PEER_MSG_1"),
          std::bind(&NonLooperSampleModule::handleP2PMessage, this, _1) },
        };

      }

      inline ~NonLooperSampleModule() {}

      inline HandlerExecutionStatus getHandlerExecutionStatus() {
        return mFlag;
      }

      inline void handleP2PMessage(std::shared_ptr<Message> msg) {
        std::shared_ptr<P2PMessage> shared_msg(
          std::static_pointer_cast<P2PMessage>(msg));

        std::thread::id myThreadId = std::this_thread::get_id();
        std::thread::id dispatcherLooperThreadId = Dispatcher::getInstance().getDispatcherLooperThreadId();

        if (myThreadId == dispatcherLooperThreadId) {
          mFlag = HandlerExecutionStatus::INVOKED_IN_DISPATCHER_CONTEXT;
        } else {
          mFlag = HandlerExecutionStatus::NOT_INVOKED_IN_DISPATCHER_CONTEXT;
        }

        shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS, std::make_shared<string>("solicited-response"));
      }
    };

    NonLooperSampleModule *myModule = new NonLooperSampleModule();
    myModule->init();

    std::shared_ptr<P2PMessage> p2pMsgPtr = std::make_shared<P2PMessage>();
    myModule->dispatch(p2pMsgPtr);

    if (myModule->getHandlerExecutionStatus() == HandlerExecutionStatus::NOT_INVOKED) {
      Log::getInstance().d("Handler not yet executed, Waiting 30 seconds...!");
      threadSleep(30);
    }

    threadSleep(1);

    ASSERT_EQ(HandlerExecutionStatus::INVOKED_IN_DISPATCHER_CONTEXT, myModule->getHandlerExecutionStatus());

    delete myModule;
    myModule = nullptr;
}

TEST(FrameworkUnitTest, VerifyDedicatedLooperModuleDispatchIsHandledInNonDispatcherContext) {

    class P2PMessage : public SolicitedMessage<string> {
    public:
      int mRandomData;
      P2PMessage() :
        mRandomData{ 911 },
        SolicitedMessage<string>(Dispatcher::getInstance().registerMessage("TEST_PEER_2_PEER_MSG_2")) {
            mName = "TEST_PEER_2_PEER_MSG_2";
      }

      inline string dump() override {
        return mName + "{}";
      }
    };

    enum class HandlerExecutionStatus {
      NOT_INVOKED,
      INVOKED_IN_DISPATCHER_CONTEXT,
      NOT_INVOKED_IN_DISPATCHER_CONTEXT
    };

    class LooperSampleModule : public Module {
    private:
      HandlerExecutionStatus mFlag;
    public:
      inline LooperSampleModule() {
        mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);
        mFlag = HandlerExecutionStatus::NOT_INVOKED;
        mName = "LooperSampleModule";
        using std::placeholders::_1;
        mMessageHandler = {
          { REG_MSG("TEST_PEER_2_PEER_MSG_2"),
          std::bind(&LooperSampleModule::handleP2PMessage, this, _1) },
        };

      }

      inline ~LooperSampleModule() {}

      inline HandlerExecutionStatus getHandlerExecutionStatus() {
        return mFlag;
      }

      inline void handleP2PMessage(std::shared_ptr<Message> msg) {
        std::shared_ptr<P2PMessage> shared_msg(
          std::static_pointer_cast<P2PMessage>(msg));

        std::thread::id myThreadId = std::this_thread::get_id();
        std::thread::id dispatcherLooperThreadId = Dispatcher::getInstance().getDispatcherLooperThreadId();

        if (myThreadId == dispatcherLooperThreadId) {
          mFlag = HandlerExecutionStatus::INVOKED_IN_DISPATCHER_CONTEXT;
        } else {
          mFlag = HandlerExecutionStatus::NOT_INVOKED_IN_DISPATCHER_CONTEXT;
        }

        shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS, std::make_shared<string>("solicited-response"));
      }
    };

    LooperSampleModule *myModule = new LooperSampleModule();
    myModule->init();

    std::shared_ptr<P2PMessage> p2pMsgPtr = std::make_shared<P2PMessage>();
    myModule->dispatch(p2pMsgPtr);

    if (myModule->getHandlerExecutionStatus() == HandlerExecutionStatus::NOT_INVOKED) {
      Log::getInstance().d("Handler not yet executed, Waiting 30 seconds...!");
      threadSleep(30);
    }

    threadSleep(1);

    ASSERT_EQ(HandlerExecutionStatus::NOT_INVOKED_IN_DISPATCHER_CONTEXT, myModule->getHandlerExecutionStatus());

    delete myModule;
    myModule = nullptr;
}
#endif
