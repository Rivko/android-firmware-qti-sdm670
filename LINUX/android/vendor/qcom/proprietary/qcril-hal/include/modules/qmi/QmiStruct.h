/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#ifndef __QMI_STRUCT_H
#define __QMI_STRUCT_H

#include "framework/Message.h"
#include "qmi_client.h"
#include <memory>

#define QMI_NOMEM_ERR           -32
// for QMI send sync/async
typedef struct QmiSync {
    unsigned int                msgId;
    void                        *reqData;
    unsigned int                reqSize;
    void                        *respData;
    unsigned int                respSize;
    int                         timerValue;
} QmiSyncStruct;

typedef void (*qmiAsyncCbType)
(
    unsigned int                msgId,
    std::shared_ptr<void>       respData,
    unsigned int                respSize,
    void                        *cbData,
    qmi_client_error_type       transpErr
);

typedef struct QmiReqAsync {
    unsigned int                msgId;
    void                        *reqData;
    unsigned int                reqSize;
    unsigned int                respSize;
    qmiAsyncCbType              cb;
    void                        *cbData;
} QmiReqAsyncStruct;

typedef struct QmiResp {
    std::shared_ptr<void>       respData;
    unsigned int                respSize;
    qmi_client_error_type       transpErr;
} QmiRespStruct;

typedef struct QmiAsyncRespData {
  unsigned int                 msgId;
  std::shared_ptr<void>        respData;
  unsigned int                 respDataSize;
  qmi_client_error_type        traspErr;
  qmiAsyncCbType               cb;
  void                        *cbData;
} QmiAsyncRespData;

class ModemEndPointModule;
typedef struct QmiAsyncCbData {
    ModemEndPointModule             *pModule;
    std::shared_ptr<Message>        msg;
    std::shared_ptr<unsigned char>  respData;
} QmiAsyncCbDataStruct;

typedef struct QmiIndMsgData {
    unsigned int                msgId;
    unsigned char               *indData;
    unsigned int                indSize;
    void                        *cbData;
} QmiIndMsgDataStruct;

#endif
