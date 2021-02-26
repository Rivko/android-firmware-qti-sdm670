/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include <framework/legacy.h>
#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include "modules/data/DataCommon.h"
#include <modules/android/RilRequestMessage.h>

namespace rildata {


class ProcessStackSwitchMessage : public SolicitedMessage<int>,
                                  public add_message_id<ProcessStackSwitchMessage>
{
  public:

    typedef struct
    {
      qcril_modem_stack_id_e_type oldStackId;
      qcril_modem_stack_id_e_type newStackId;
      qcril_instance_id_e_type instanceId;
    } StackSwitchReq;

  private:
    StackSwitchReq mData;

  public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_PROCESS_STACK_SWITCH";

    ProcessStackSwitchMessage()= delete;

    ProcessStackSwitchMessage(StackSwitchReq & params);

    ProcessStackSwitchMessage(
         qcril_modem_stack_id_e_type old_stack_id,
         qcril_modem_stack_id_e_type new_stack_id,
         qcril_instance_id_e_type instance_id);

    ~ProcessStackSwitchMessage();

    StackSwitchReq getParams();

    string dump();
};

}
