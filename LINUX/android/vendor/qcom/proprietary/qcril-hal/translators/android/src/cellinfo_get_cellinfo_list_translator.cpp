/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <modules/cellinfo/GetCellInfoListMessage.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

class cellinfo_get_list_translator: public legacy_translator<RIL_REQUEST_GET_CELL_INFO_LIST>
{
    public:
        virtual std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request) override;
};
static cellinfo_get_list_translator the_translator;


QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_GET_CELL_INFO_LIST, void *, RIL_CellInfo_v12 *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_GET_CELL_INFO_LIST);

std::shared_ptr<SolicitedMessage<generic_callback_payload>> cellinfo_get_list_translator::translate_request(const ril_request_type &request)
{
    QCRIL_NOTUSED(request);
    qcril_request_params_type params { request.instance_id, QCRIL_DEFAULT_MODEM_ID, request.req_id_android, request.req_id, const_cast<void *>(request.payload), request.payload_len, request.t, nullptr  };
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> ret =
        std::make_shared<GetCellInfoListMessage>(params);
    return ret;
}
