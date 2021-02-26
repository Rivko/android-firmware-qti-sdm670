/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_qdi.h
@brief: Header file containing all definitions required to implement QDI
        remoting.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/inc/mpd/adsppm_qdi.h#1 $
*/

#ifndef ADSPPM_QDI_H_
#define ADSPPM_QDI_H_

#include "qurt_qdi_constants.h"
#include "qurt_rmutex.h"
#include "qurt_qdi_driver.h"
#include "asyncmgr.h"
#include "mmpm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "qurt.h"


#define ADSPPM_QDI_DRV_NAME "/power/adsppm"
#define ADSPPM_REGISTER    (QDI_PRIVATE+0)
#define ADSPPM_REQUEST     (QDI_PRIVATE+1)
#define ADSPPM_RELEASE     (QDI_PRIVATE+2)
#define ADSPPM_DEREGISTER  (QDI_PRIVATE+3)
#define ADSPPM_GET_INFO    (QDI_PRIVATE+4)
#define ADSPPM_SET_PARAM   (QDI_PRIVATE+5)
#define ADSPPM_GET_CB      (QDI_PRIVATE+7)


typedef struct QDI_Adsppm_Ctx_t
{
    qurt_qdi_obj_t qdiobj;
    struct QDI_Adsppm_CbqElem_t *cbqueue;
    qurt_mutex_t mtx;
    qurt_signal_t sigLocal[QDSPPM_NUM_PDS];
    int log_buffer_size;
    ULogHandle hLog;
} QDI_Adsppm_Ctx_t;

typedef struct
{
    QDI_Adsppm_Ctx_t *object;
    uint32 (*pfn)(MmpmCallbackParamType *pCbParam); // Callback function for
                                                    // event notification
    uint32 clientId;
    uint32 pdNum;
} QDI_Adsppm_cbinfo_t;

typedef struct QDI_Adsppm_CbqElem_t
{
    struct QDI_Adsppm_CbqElem_t *next;
    QDI_Adsppm_cbinfo_t *cbinfo;
    MmpmCallbackParamType callbackParam;
    MmpmCompletionCallbackDataType callbackValue;
} QDI_Adsppm_CbqElem_t;

typedef struct
{
    uint32 (*pfn)(MmpmCallbackParamType *pCbParam); // Callback function for
                                                    // event notification
    MmpmCallbackParamType callbackParam;
    MmpmCompletionCallbackDataType callbackValue;
} Adsppm_cbinfo_Client_t;

typedef struct
{
    qurt_qdi_obj_t qdiobj;
} QDI_Adsppm_Opener_t;


void QDI_adsppm_init(void);


#endif /* ADSPPM_QDI_H_ */

