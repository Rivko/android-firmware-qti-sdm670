/******************************************************************************
#  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "modules/uim/qcril_uim_srvc.h"


typedef struct
{
  char       mcc[4];
  char       mnc[4];
  RIL_Errno  err_code;
}qcril_mcc_mnc_info_type;


typedef enum
{
  QCRIL_UIM_APP_UNKNOWN                       = 0,
  QCRIL_UIM_APP_SIM                           = 1,
  QCRIL_UIM_APP_USIM                          = 2,
  QCRIL_UIM_APP_RUIM                          = 3,
  QCRIL_UIM_APP_CSIM                          = 4,
  QCRIL_UIM_APP_ISIM                          = 5
} qcril_uim_app_type;


class UimGetMccMncRequestMsg : public SolicitedMessage<qcril_mcc_mnc_info_type>,
                               public add_message_id<UimGetMccMncRequestMsg>
{
  private:
    char                mAidBuffer[QMI_UIM_MAX_AID_LEN+1];
    qcril_uim_app_type  mAppType;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.get_mcc_mnc";
    UimGetMccMncRequestMsg() = delete;
    inline ~UimGetMccMncRequestMsg() {}

    inline UimGetMccMncRequestMsg(char * aid, qcril_uim_app_type app_type,
                                  GenericCallback<qcril_mcc_mnc_info_type> *callback) :
                                  SolicitedMessage<qcril_mcc_mnc_info_type>(get_class_message_id())
    {
      mName = MESSAGE_NAME;
      mAppType = app_type;
      memset(mAidBuffer, 0x00, sizeof(mAidBuffer));

      /* check for AID len + null termination */
      if (aid != nullptr && strlen(aid) <= QMI_UIM_MAX_AID_LEN )
      {
        strlcpy(mAidBuffer, aid, sizeof(mAidBuffer));
      }

      setCallback(callback);
    }

    inline char * get_aid()
    {
      return mAidBuffer;
    }

    inline qcril_uim_app_type get_app_type()
    {
      return mAppType;
    }

    inline string dump()
    {
      return mName + "{ " + mAidBuffer + "}";
    }
};
