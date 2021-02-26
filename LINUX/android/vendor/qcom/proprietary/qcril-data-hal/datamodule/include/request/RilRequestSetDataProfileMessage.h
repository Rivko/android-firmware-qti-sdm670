/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once

#include <framework/legacy.h>
#include "framework/Message.h"
#include "string.h"
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include <modules/android/RilRequestMessage.h>
#include "modules/data/DataCommon.h"

namespace rildata {

class RilRequestSetDataProfileMessage : public SolicitedMessage<generic_callback_payload>,
                                        public add_message_id<RilRequestSetDataProfileMessage>
{
  private:

    legacy_request_payload params;
    void deepCopy(qcril_request_params_type &p);

  public:

    static constexpr const char* MESSAGE_NAME = "RIL_REQUEST_SET_DATA_PROFILE";

    RilRequestSetDataProfileMessage() = delete;
    ~RilRequestSetDataProfileMessage();
    RilRequestSetDataProfileMessage(const qcril_request_params_type &p);

    qcril_request_params_type& get_params();
    string dump();
};

#if (QCRIL_RIL_VERSION >= 15)
class RilRequestSetDataProfileMessage_v15 : public SolicitedMessage<generic_callback_payload>,
                                            public add_message_id<RilRequestSetDataProfileMessage_v15>
{
  private:

    legacy_request_payload params;
    void deepCopy(qcril_request_params_type &p);

  public:

    static constexpr const char* MESSAGE_NAME = "RIL_REQUEST_SET_DATA_PROFILE_v15";

    RilRequestSetDataProfileMessage_v15() = delete;
    ~RilRequestSetDataProfileMessage_v15();
    RilRequestSetDataProfileMessage_v15(const qcril_request_params_type &p);

    qcril_request_params_type& get_params();
    string dump();
};
#endif /* QCRIL_RIL_VERSION >= 15 */

}//namespace
