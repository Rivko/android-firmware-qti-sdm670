/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include "modules/data/DataCommon.h"

namespace rildata {

class ProcessScreenStateChangeMessage : public SolicitedMessage<int>,
                                        public add_message_id<ProcessScreenStateChangeMessage> {
    private:
    public:
	boolean screenState;
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_PROCESS_SCREEN_STATE_CHANGE";
    ProcessScreenStateChangeMessage() = delete;

    inline ProcessScreenStateChangeMessage( boolean screen_state ):
                     SolicitedMessage<int>(get_class_message_id()) {
        mName = MESSAGE_NAME;
	screenState = screen_state;
  }
  ~ProcessScreenStateChangeMessage();
  string dump();
};
}
