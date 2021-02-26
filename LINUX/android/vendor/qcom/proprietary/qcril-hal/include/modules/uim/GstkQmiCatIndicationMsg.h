/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "modules/uim/qcril_gstk_srvc.h"


class GstkQmiCatIndicationMsg : public UnSolicitedMessage,
                                public add_message_id<GstkQmiCatIndicationMsg>
{
  private:
    std::shared_ptr<qcril_gstk_qmi_ind_params_type>  mIndPtr;
    qmi_cat_indication_id_type                       mIndId;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.gstk.qmi_cat_indication";
    GstkQmiCatIndicationMsg() = delete;
    inline ~GstkQmiCatIndicationMsg()
    {
      mIndPtr = nullptr;
    }

    inline GstkQmiCatIndicationMsg(qmi_cat_indication_id_type                        ind_id,
                                   std::shared_ptr<qcril_gstk_qmi_ind_params_type>   ind_data_ptr):
                                                          UnSolicitedMessage(get_class_message_id())
    {
      mIndPtr = ind_data_ptr;
      mIndId  = ind_id;
    }

    inline std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<GstkQmiCatIndicationMsg>(mIndId, mIndPtr));
    }

    inline string dump()
    {
      return mName + "{" + std::to_string(mIndId) + "}";
    }

    inline qcril_gstk_qmi_ind_params_type *get_message(void)
    {
      return mIndPtr.get();
    }

    inline qmi_cat_indication_id_type get_ind_id(void)
    {
      return mIndId;
    }
};

