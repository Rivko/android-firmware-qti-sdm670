/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <memory>
#include <framework/SolicitedMessage.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/GenericCallback.h>

class generic_callback_payload
{
};

template <typename Rt, typename ... Args>
class generic_callback_payload_base: public generic_callback_payload
{

};

class message_translator
{
};

template <typename ReqT, typename RspT, typename UnsT>
class message_translator_base: public message_translator
{
    public:
        /**
          * translate_request: Invoked when a conversion is required. The source module will provide the input in the request parameter.
          * The Returned message must be a
          * @param request: The original data that needs translation
          * @return The message that will be dispatched, and that contains the converted payload.
          */
        virtual std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ReqT &request) = 0;

        /**
          * translate_response: Translate a response.
          * This method is invoked on receiving a callback for the messages created by the previous  method (translate_request)
          * @param
          */
        virtual std::shared_ptr<RspT> translate_response(std::shared_ptr<SolicitedMessage<generic_callback_payload>> msg,
                Message::Callback::Status status,
                std::shared_ptr<generic_callback_payload>) = 0;

        virtual std::shared_ptr<UnsT> translate_unsol(std::shared_ptr<UnSolicitedMessage> msg) = 0;

        virtual ~message_translator_base() {}
};
