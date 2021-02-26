/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppmcb.h
@brief: Header file for ADSPPM component that handles callbacks for clients in
        the user protection domain (user PD).

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/inc/mpd/adsppmcb.h#1 $
*/

#include "mmpm.h"
#include "adsppm_qdi.h"


/**
 * @struct Adsppmcb_Elem_Type_Srv
 * @brief Call back info queue for each registered client
 */
typedef struct Adsppmcb_Elem_Type_Srv
{
    struct Adsppmcb_Elem_Type_Srv *next;
    QDI_Adsppm_cbinfo_t arg;
} Adsppmcb_Elem_Type_Srv;


/**
 * @fn Adsppmcb_Init_Server
 * @brief Initialize ADSPPM QDI server
 */
void Adsppmcb_Init_Server(void);

/**
 * @fn Adsppmcb_register_callback_Srv
 * @brief Register call back function for QDI clients
 */
void Adsppmcb_register_callback_Srv(QDI_Adsppm_cbinfo_t *arg);

/**
 * @fn Adsppmcb_deRegister_callback_Srv
 * @brief De-register call back function for QDI clients
 */
void Adsppmcb_deRegister_callback_Srv(int clientId);

/**
 * @fn Adsppmcb_notify_callbacks_Srv
 * @brief Callback function from asynmgr
 */
uint32 Adsppmcb_notify_callbacks_Srv(MmpmCallbackParamType *pCallbackValue);

/**
 * @fn Adsppmcb_Deinit_Srv
 * @brief De-initialize ADSPPM QDI server
 */
void Adsppmcb_Deinit_Srv(void);

/**
 * @fn QDI_Adsppm_do_callback
 * @brief Add callback data to the callback queue and signal user to pick it up
 */
void QDI_Adsppm_do_callback(QDI_Adsppm_cbinfo_t *ptr,
    MmpmCallbackParamType *pValue, uint32 pd_num);

