/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

enum class NasRequestDataShutdownResponse
{
    SUCCESS,
    FAILURE
};

class NasRequestDataShutdownMessage : public SolicitedMessage<NasRequestDataShutdownResponse>,
                                      public add_message_id<NasRequestDataShutdownMessage>
{
    public:
        static constexpr const char *MESSAGE_NAME =
            "com.qualcomm.qti.qcril.nas.request_data_shutdown";
        NasRequestDataShutdownMessage() = delete;

        inline ~NasRequestDataShutdownMessage()
        {
        }

        inline NasRequestDataShutdownMessage(TimeKeeper::millisec timeout):
            SolicitedMessage<NasRequestDataShutdownResponse>(get_class_message_id(), timeout)
        {
        }

        string dump()
        {
            return mName;
        }
};
