/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <framework/GenericCallback.h>
#include <modules/android/RilRequestMessage.h>
#include <framework/legacy.h>

/******************************************************************************
#   Message Class: NasEmbmsEnableDataConMessage
#   Sender: DataModule
#   Receiver: NasModule
#   Data transferred: qcril_embms_enable_response_payload_type
                      1. cause: int
                      2. call_id: uint8_t
                      3. interface_name: char array
                      4. if_index: unsigned int
                      5. dbg_trace_id: int32_t
                      6. resp_code: int32_t
#   Usage: Data Sends us this message when Embms call is setup.
           Nas updates its embms state machine accordingly and and sends response if
           there has been a oem hook req to enable embms.
#******************************************************************************/

class NasEmbmsEnableDataConMessage : public SolicitedMessage<RIL_Errno>,
    public add_message_id<NasEmbmsEnableDataConMessage>
{
private:
    qcril_embms_enable_response_payload_type payload;

public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.data_embms_enable_data_con";

    NasEmbmsEnableDataConMessage() = delete;
    inline ~NasEmbmsEnableDataConMessage() {};

    inline NasEmbmsEnableDataConMessage(int cause, uint8_t call_id,
        char* interface_name,int interface_name_len, unsigned int if_index, int32_t dbg_trace_id, int32_t resp_code) :
        SolicitedMessage<RIL_Errno>(get_class_message_id())
    {
        mName = MESSAGE_NAME;
        payload.cause = cause;
        payload.call_id = call_id;
        if (interface_name != NULL && interface_name_len > 0)
        {
            strlcpy(payload.interafce_name, interface_name, sizeof(payload.interafce_name));
        }
        payload.if_index = if_index;
        payload.dbg_trace_id = dbg_trace_id;
        payload.resp_code = resp_code;
    }
    inline NasEmbmsEnableDataConMessage(qcril_embms_enable_response_payload_type& p) :
        SolicitedMessage<RIL_Errno>(get_class_message_id())
    {
        payload = p;
    }
    inline const qcril_embms_enable_response_payload_type &get_payload()
    {
        return payload;
    }

    string dump()
    {
      return mName;
    }
};
