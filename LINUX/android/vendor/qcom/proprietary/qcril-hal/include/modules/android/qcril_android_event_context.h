/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#ifdef __cplusplus
#include <memory>
#include <telephony/ril.h>
#include <framework/Message.h>
#include <framework/SolicitedMessage.h>
#include <framework/message_translator.h>
#endif


struct qcril_event_context;

#ifdef __cplusplus
struct qcril_event_context
{
    uint32_t token_id;
    qcril_instance_id_e_type instance_id;
    int req_id;
    bool is_internal;
    RIL_Token t;
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> msg;
    qcril_event_context(qcril_instance_id_e_type instance_id, int req_id, RIL_Token t):
        token_id(*static_cast<uint32_t *>(t)),
        instance_id(instance_id),
        req_id(req_id),
        is_internal(false),
        t(t)
    {
    }
};
#endif
