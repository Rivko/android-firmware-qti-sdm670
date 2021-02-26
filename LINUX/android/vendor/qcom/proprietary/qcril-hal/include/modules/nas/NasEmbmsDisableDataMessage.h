/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "framework/legacy.h"

/******************************************************************************
#   Message Class: NasEmbmsDisableDataMessage
#   Sender: DataModule
#   Receiver: NasModule
#   Data transferred: qcril_embms_disable_indication_payload_type
                      1. cause: int
                      2. call_id: uint8_t
                      3. interface_name: char array
                      4. dbg_trace_id: int32_t
                      5. resp_code: int32_t
#   Usage: Data Sends us this message when Embms call is disabled/disconnected.
           Nas updates its embms state machine accordingly and sends response if
           there has been a oem hook req to disable embms.
#******************************************************************************/

class NasEmbmsDisableDataMessage : public SolicitedMessage<RIL_Errno>,
    public add_message_id<NasEmbmsDisableDataMessage>
{
private:
    qcril_embms_disable_indication_payload_type payload;

public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.data_embms_disable_data";

    NasEmbmsDisableDataMessage() = delete;
    inline ~NasEmbmsDisableDataMessage() {};

    inline NasEmbmsDisableDataMessage(int cause, uint8_t call_id,
        char* interface_name, int interface_name_len, int32_t dbg_trace_id, int32_t resp_code) :
        SolicitedMessage<RIL_Errno>(get_class_message_id())
    {
        mName = MESSAGE_NAME;
        payload.cause = cause;
        payload.call_id = call_id;
        if (interface_name != NULL && interface_name_len > 0)
        {
            strlcpy(payload.interafce_name, interface_name, sizeof(payload.interafce_name));
        }
        payload.dbg_trace_id = dbg_trace_id;
        payload.resp_code = resp_code;
    }
    inline NasEmbmsDisableDataMessage(qcril_embms_disable_indication_payload_type &p) :
        SolicitedMessage<RIL_Errno>(get_class_message_id())
    {
        payload = p;
    }

    inline const qcril_embms_disable_indication_payload_type &get_payload() {
        return payload;
    }
    string dump()
    {
      return mName;
    }
};
