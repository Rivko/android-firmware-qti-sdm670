/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ-CELLINFO"
#include <framework/Module.h>
#include <modules/cellinfo/GetCellInfoListMessage.h>

extern "C" void qcril_qmi_nas_get_cell_info_list_ncl
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr // Output parameter
);

class cellinfo_module: public Module
{
    private:
    public:
        void handle_get_cell_info_list(std::shared_ptr<GetCellInfoListMessage> msg);
        cellinfo_module();
};

static load_module<cellinfo_module> the_module;


cellinfo_module::cellinfo_module()
{
    mName = "cellinfo_module";
    using std::placeholders::_1;

    mMessageHandler = {
        HANDLER(GetCellInfoListMessage, cellinfo_module::handle_get_cell_info_list),
    };
}

void cellinfo_module::handle_get_cell_info_list(std::shared_ptr<GetCellInfoListMessage> msg)
{

    QCRIL_LOG_FUNC_ENTRY();
    std::shared_ptr<GetCellInfoListMessage> getmsg = std::static_pointer_cast<GetCellInfoListMessage>(msg);

    qcril_request_params_type &params = getmsg->get_params();
    qcril_qmi_nas_get_cell_info_list_ncl(&params, nullptr);
    //msg.sendResponse();
    QCRIL_LOG_FUNC_RETURN();
}
