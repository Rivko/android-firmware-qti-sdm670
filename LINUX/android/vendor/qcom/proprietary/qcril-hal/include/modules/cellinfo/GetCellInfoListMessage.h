/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/SolicitedMessage.h>
#include <framework/add_message_id.h>
#include <modules/android/RilRequestMessage.h>

class GetCellInfoListMessage: public SolicitedMessage<generic_callback_payload>, public add_message_id<GetCellInfoListMessage>
{
    private:
        legacy_request_payload params;
    public:
        static constexpr const char * const MESSAGE_NAME = "com.qualcomm.qti.qcril.cellinfo.get_cell_info_list";
        GetCellInfoListMessage(const qcril_request_params_type &params):
            SolicitedMessage<generic_callback_payload>(get_class_message_id()),
            params(params) {}
        qcril_request_params_type &get_params() {
            return params.get_params();
        }
        string dump() {
            return mName;
        }
};
