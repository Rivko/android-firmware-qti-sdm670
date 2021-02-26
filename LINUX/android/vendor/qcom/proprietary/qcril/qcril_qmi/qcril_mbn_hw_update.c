/******************************************************************************

  @file    qcril_mbn_reader.c
  @brief   Provides interface to read mbn file

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  ---------------------------------------------------------------------------
******************************************************************************/

#include "qcril_file_utils.h"
#include "qcril_mbn_hw_update.h"
#include "qcril_mbn_sw_update.h"
#include "qcril_mbn_db.h"
#include "qcril_mbn_meta.h"
#include "qcril_log.h"
#include "qcril_dir_utils.h"
#include "qcrili.h"
#include "persistent_device_configuration_v01.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_pdc.h"
#include "qcril_other.h"
#include "qcril_db.h"
#include "qcril_mbn_sw_update.h"

#define QCRIL_MBN_HW_FEATURE_ENABLED "persist.vendor.radio.hw_mbn_update"
#define QCRIL_MBN_SW_OTA_FOR_MODEM_CENTRIC "persist.vendor.radio.start_ota_daemon"

typedef struct {
    uint32_t                 sub_id;
    pdc_config_type_enum_v01 config_type;
} qcril_mbn_hw_config_req_type;

typedef struct {
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
    uint32_t config_id_len;
} qcril_mbn_hw_config_id_info;

typedef struct {
    uint32_t sub_id;
    char    *file_name;
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
    uint32_t config_id_len;
} qcril_mbn_hw_load_config_info;

typedef struct {
    uint32_t sub_id;
    pdc_config_type_enum_v01 config_type;
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
    uint32_t config_id_len;
} qcril_mbn_hw_select_config_info;

typedef struct {
    uint32_t cur_stat;
    void (*on_success)(void *);
    void (*on_failure)(void *);
} qcril_mbn_hw_state_machine_type;

typedef enum {
    QCRIL_MBN_HW_STATE_NONE                         = 0,
    QCRIL_MBN_HW_STATE_INITIALIZING                 = 1,
    QCRIL_MBN_HW_STATE_MBN_LOADING_TO_DB            = 2,
    QCRIL_MBN_HW_STATE_MBN_LOADING_FAILED           = 3,
    QCRIL_MBN_HW_STATE_GETTING_CURRENT_CONFIG       = 4,
    QCRIL_MBN_HW_STATE_DEACTIVATING_CURRENT_CONFIG  = 5,
    QCRIL_MBN_HW_STATE_DEACTIVATION_FAILED          = 6,
    QCRIL_MBN_HW_STATE_DELETING_CURRENT_CONFIG      = 7,
    QCRIL_MBN_HW_STATE_LOADING_CONFIG               = 8,
    QCRIL_MBN_HW_STATE_LOAD_FAILED                  = 9,
    QCRIL_MBN_HW_STATE_SELECTING_CONFIG             = 10,
    QCRIL_MBN_HW_STATE_SELECTION_FAILED             = 11,
    QCRIL_MBN_HW_STATE_ACTIVATING_CONFIG            = 12,
    QCRIL_MBN_HW_STATE_ACTIVATION_COMPLETED         = 13,
    QCRIL_MBN_HW_STATE_ACTIVATION_FAILED            = 14,
    QCRIL_MBN_HW_STATE_ACTIVATION_NOT_NEEDED        = 15
} qcril_mbn_hw_state_type;

qcril_mbn_call_back_on_drop_type qcril_mbn_hw_call_back_on_drop;

qcril_mbn_hw_state_type qcril_mbn_hw_cur_state = QCRIL_MBN_HW_STATE_NONE;
int qcril_mbn_max_apss_instance_id = 1;
char     hw_config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
uint32_t hw_config_id_len;

char     active_hw_config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
uint32_t active_hw_config_id_len;
pthread_mutex_t        qcril_mbn_hw_config_mutex;
pthread_mutexattr_t    qcril_mbn_hw_config_mutex_attr;

#define MBN_HW_CONFIG_LOCK()    pthread_mutex_lock(&qcril_mbn_hw_config_mutex)
#define MBN_HW_CONFIG_UNLOCK()  pthread_mutex_unlock(&qcril_mbn_hw_config_mutex)

IxErrnoType qcril_mbn_hw_load_config
(
    char *file_name
);

IxErrnoType qcril_mbn_hw_select_config
(
    uint32_t  sub_id,
    pdc_config_type_enum_v01 config_type,
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01],
    uint32_t config_id_len
);

IxErrnoType qcril_mbn_hw_activate_selected_config
(
    uint32_t  sub_id,
    pdc_config_type_enum_v01 config_type
);

char *qcril_mbn_hw_get_hw_name_to_look_for
(
    int max_subscriptions
);

IxErrnoType qcril_mbn_hw_get_hw_config_from_db
(
    char   **mbn_file_name,
    char   *device_type
);

IxErrnoType qcril_mbn_hw_delete_config
(
    char      *config_id,
    uint32_t  config_id_len
);

IxErrnoType qcril_mbn_hw_deactivate_config
(
    uint32_t                 sub_id,
    pdc_config_type_enum_v01 config_type
);

#ifndef QMI_RIL_UTF
// external function: MBN OTA dedicated thread
void* qmi_ril_mbn_ota_thread_proc(void* empty_param);
#else
void* qmi_ril_mbn_ota_thread_proc(void* empty_param)
{
    return (void*)0;
}
#endif

/*===========================================================================

  FUNCTION qcril_mbn_hw_get_cur_state

===========================================================================*/
/*!
    @brief
    Get cur state for hw mbn

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
static qcril_mbn_hw_state_type qcril_mbn_hw_get_cur_state
(
    void
)
{
    return qcril_mbn_hw_cur_state;
}

/*===========================================================================

  FUNCTION qcril_mbn_hw_set_cur_state

===========================================================================*/
/*!
    @brief
    Set cur state for hw mbn

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
static void qcril_mbn_hw_set_cur_state
(
    qcril_mbn_hw_state_type state
)
{
    QCRIL_LOG_DEBUG("new_state: %d", state);
    qcril_mbn_hw_cur_state = state;
}

/*===========================================================================

  FUNCTION qcril_mbn_hw_is_completed

===========================================================================*/
/*!
    @brief
    Check if mbn hw is completed

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
boolean qcril_mbn_hw_is_completed
(
    void
)
{
    boolean result = FALSE;

    /*check hw_mbn_update property.if it is disabled there no need to check
      hw_mbn current state and return TRUE to process PDC OEM requests.
    */
    int is_hw_enabled = 1;
    qmi_ril_get_property_value_from_integer(QCRIL_MBN_HW_FEATURE_ENABLED,
                                            &is_hw_enabled, 0);
    if(!is_hw_enabled) {
        return TRUE;
    }

    switch (qcril_mbn_hw_cur_state)
    {
        case QCRIL_MBN_HW_STATE_NONE:
        case QCRIL_MBN_HW_STATE_ACTIVATION_NOT_NEEDED:
        case QCRIL_MBN_HW_STATE_ACTIVATION_FAILED:
        case QCRIL_MBN_HW_STATE_SELECTION_FAILED:
        case QCRIL_MBN_HW_STATE_LOAD_FAILED:
        case QCRIL_MBN_HW_STATE_MBN_LOADING_FAILED:
        case QCRIL_MBN_HW_STATE_ACTIVATION_COMPLETED:
        {
            result = TRUE;
            break;
        }

        default:
            break;
    }

    return result;
}

/*===========================================================================

  FUNCTION qcril_mbn_hw_send_get_selected_resp

===========================================================================*/
/*!
    @brief
    Send get_selected response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_send_get_selected_resp
(
    RIL_Errno result,
    uint8_t     *config_id,
    uint32_t  config_id_len
)
{
    char *file_name = NULL;
    char *hw_name = NULL;
    uint8_t tmp_config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
    IxErrnoType ret = E_SUCCESS;

    unsigned int len         = 0;
    unsigned int start_pos   = 0;

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        QCRIL_LOG_DEBUG("result: %d",
                        result);

        qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                           (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);

        hw_name = qcril_mbn_hw_get_hw_name_to_look_for(
                                qcril_mbn_max_apss_instance_id);

        if(NULL == hw_name)
        {
            QCRIL_LOG_DEBUG("hw_name is null");
            ret = E_FAILURE;
            break;
        }
        QCRIL_LOG_DEBUG("Retrieve file name %s", hw_name);

        /* retrieve mbn file for apss device type*/
        qcril_mbn_hw_get_hw_config_from_db(&file_name, hw_name);
        if(NULL == file_name)
        {
            QCRIL_LOG_DEBUG("file_name is null");
            ret = E_FAILURE;
            break;
        }
        QCRIL_LOG_DEBUG("file name %s", file_name);

        if ((result != RIL_E_SUCCESS) || (!config_id) || (!config_id_len))
        {
            qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_LOADING_CONFIG);
            qcril_mbn_hw_load_config(file_name);
        }
        else
        {
            memcpy(active_hw_config_id, config_id, config_id_len);
            active_hw_config_id_len = config_id_len;

            if (strlen(file_name) >= sizeof(tmp_config_id))
            {
                start_pos = strlen(file_name) - len;
            }

            len = strlcpy((char *)tmp_config_id, file_name + start_pos,
                               PDC_CONFIG_ID_SIZE_MAX_V01);

            if ((len == config_id_len) &&
                !memcmp(tmp_config_id, active_hw_config_id,
                                        config_id_len))
            {
                qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_ACTIVATION_NOT_NEEDED);
                if (qcril_mbn_hw_call_back_on_drop)
                {
                    qcril_mbn_hw_call_back_on_drop();
                }
                else
                {
                    qcril_mbn_sw_update_init(TRUE);
                }
            }
            else
            {
                qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_DEACTIVATING_CURRENT_CONFIG);
                qcril_mbn_hw_deactivate_config(0, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);
            }
        }
    } while(FALSE);

    QCRIL_LOG_FUNC_RETURN();

    return ret;
}

/*===========================================================================

  FUNCTION qcril_mbn_hw_query_selected_config_hndlr

===========================================================================*/
/*!
    @brief
    Query selected confuration handler

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_hw_query_selected_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno                     result = RIL_E_GENERIC_FAILURE;
    qcril_reqlist_public_type     qcril_req_info;
    qcril_mbn_hw_config_req_type *req = NULL;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_hw_config_req_type *)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_HW_GET_SELECTED_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            result = qcril_qmi_pdc_get_selected_mbn_config(req->sub_id,
                                                           req->config_type);
        }
        else
        {
            QCRIL_LOG_ERROR("No memory to allocate reqlist");
        }

    } while (0);

    QCRIL_LOG_DEBUG("result: %d", result);
    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_hw_send_get_selected_resp(result, NULL, 0);
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_mbn_hw_retrieve_selected_config

===========================================================================*/
/*!
    @brief
    Get selected config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_retrieve_selected_config
(
    void
)
{
    qcril_mbn_hw_config_req_type req;
    QCRIL_LOG_FUNC_ENTRY();

    memset(&req, 0, sizeof(req));
    req.sub_id      = 0;
    req.config_type = PDC_CONFIG_TYPE_MODEM_PLATFORM_V01;

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_HW_GET_SELECTED_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);
    QCRIL_LOG_FUNC_RETURN();

    return E_SUCCESS;
}

/*===========================================================================

  FUNCTION qcril_mbn_hw_send_deactivate_config_resp

===========================================================================*/
/*!
    @brief
    Send deactivate config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_send_deactivate_config_resp
(
    RIL_Errno result
)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                       (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_DEACTIVATION_FAILED);
        if (qcril_mbn_hw_call_back_on_drop)
        {
            qcril_mbn_hw_call_back_on_drop();
        }
        else
        {
            qcril_mbn_sw_update_init(TRUE);
        }
    }
    else
    {
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_DELETING_CURRENT_CONFIG);
        qcril_mbn_hw_delete_config(active_hw_config_id, active_hw_config_id_len);
    }

    QCRIL_LOG_FUNC_RETURN();
    return E_SUCCESS;
}

/*=========================================================================

  FUNCTION:  qcril_mbn_hw_deactivate_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_HW_DEACTIVATE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_hw_deactivate_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno                       result = RIL_E_GENERIC_FAILURE;
    qcril_mbn_hw_config_req_type   *req = NULL;
    qcril_reqlist_public_type qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_hw_config_req_type *)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_HW_DEACTIVATE_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            result = qcril_qmi_pdc_deactivate_configuration_hdlr(req->sub_id,
                                                                 req->config_type);
        }
        else
        {
            QCRIL_LOG_ERROR("No memory to allocate reqlist");
        }

    } while (0);

    QCRIL_LOG_DEBUG("result: %d", result);
    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_hw_send_deactivate_config_resp(result);
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_mbn_hw_deactivate_config

===========================================================================*/
/*!
    @brief
    Deactivate config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_deactivate_config
(
    uint32_t                 sub_id,
    pdc_config_type_enum_v01 config_type
)
{
    qcril_mbn_hw_config_req_type req;

    QCRIL_NOTUSED(sub_id);
    QCRIL_NOTUSED(config_type);

    memset(&req, 0, sizeof(req));
    req.sub_id      = sub_id;
    req.config_type = PDC_CONFIG_TYPE_MODEM_PLATFORM_V01;

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_HW_DEACTIVATE_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}
/*===========================================================================

  FUNCTION qcril_mbn_hw_send_delete_config_resp

===========================================================================*/
/*!
    @brief
    Send delete config resp

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_send_delete_config_resp
(
    RIL_Errno result
)
{
    char *file_name = NULL;
    char *hw_name = NULL;

    QCRIL_NOTUSED(result);

    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                       (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
    qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_LOADING_CONFIG);

    hw_name = qcril_mbn_hw_get_hw_name_to_look_for(
                            qcril_mbn_max_apss_instance_id);

    if(hw_name)
    {
        /* retrieve mbn file for apss device type*/
        qcril_mbn_hw_get_hw_config_from_db(&file_name, hw_name);

        if(file_name)
        {
            qcril_mbn_hw_load_config(file_name);
        }
    }
    return E_SUCCESS;
}

/*=========================================================================

  FUNCTION:  qcril_mbn_hw_delete_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_HW_DELETE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_hw_delete_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno result = RIL_E_GENERIC_FAILURE;
    qcril_mbn_hw_config_id_info *req = NULL;
    qcril_reqlist_public_type qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_hw_config_id_info *)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_HW_DELETE_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            result = qcril_qmi_pdc_delete_configuration_hdlr(req->config_id,
                                                     req->config_id_len,
                                                     PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);
        }
        else
        {
            QCRIL_LOG_ERROR("No memory to allocate reqlist");
        }

    } while (0);

    QCRIL_LOG_DEBUG("result: %d", result);
    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_hw_send_delete_config_resp(result);
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_mbn_hw_delete_config

===========================================================================*/
/*!
    @brief
    Delete config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_delete_config
(
    char      *config_id,
    uint32_t  config_id_len
)
{
    qcril_mbn_hw_config_id_info req = {{0}, 0};

    memcpy(req.config_id, config_id, config_id_len);
    req.config_id_len = config_id_len;

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_HW_DELETE_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}
/*===========================================================================

  FUNCTION qcril_mbn_hw_send_load_config_resp

===========================================================================*/
/*!
    @brief
    Send load_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_send_load_config_resp
(
    uint32_t  error
)
{
    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                       (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
    if ((error != QMI_ERR_NONE_V01) && (error != QMI_ERR_INVALID_ID_V01))
    {
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_LOAD_FAILED);
        if (qcril_mbn_hw_call_back_on_drop)
        {
            qcril_mbn_hw_call_back_on_drop();
        }
        else
        {
            qcril_mbn_sw_update_init(TRUE);
        }
    }
    else
    {
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_SELECTING_CONFIG);
        qcril_mbn_hw_select_config(0, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01,
                                   (unsigned char *)hw_config_id, hw_config_id_len);
    }

    return E_SUCCESS;
}
/*=========================================================================

  FUNCTION:  qcril_mbn_hw_load_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_HW_LOAD_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_hw_load_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno result = RIL_E_GENERIC_FAILURE;
    qcril_mbn_hw_load_config_info *req = NULL;
    qcril_reqlist_public_type qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_hw_load_config_info *)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_HW_LOAD_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            result = qcril_qmi_pdc_load_config_file(req->sub_id,
                                                    req->file_name,
                                                    (char *)req->config_id,
                                                    PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);
        }
        else
        {
            QCRIL_LOG_ERROR("No memory to allocate reqlist");
        }

    } while (0);

    QCRIL_LOG_DEBUG("result: %d", result);
    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_hw_send_load_config_resp(result);
    }

    if (req && req->file_name)
    {
        free(req->file_name);
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_mbn_hw_load_config

===========================================================================*/
/*!
    @brief
    Load config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_load_config
(
    char *file_name
)
{
    qcril_mbn_hw_load_config_info req;

    req.sub_id      = 0;
    req.file_name   = file_name;

    int len         = 0;
    int start_pos   = 0;

    if (strlen(file_name) >= sizeof(req.config_id))
    {
        start_pos = strlen(file_name) - len;
    }

    len = strlcpy((char *)req.config_id, file_name + start_pos,
                       PDC_CONFIG_ID_SIZE_MAX_V01);
    hw_config_id_len = strlcpy(hw_config_id, file_name + start_pos,
                       PDC_CONFIG_ID_SIZE_MAX_V01);
    req.config_id_len = len;
    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_HW_LOAD_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}
/*===========================================================================

  FUNCTION qcril_mbn_hw_send_select_config_resp

===========================================================================*/
/*!
    @brief
    Send select_config response

    @return
    None
*/
/*=========================================================================*/
void qcril_mbn_hw_send_select_config_resp
(
    RIL_Errno result
)
{
    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                       (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_SELECTION_FAILED);
        if (qcril_mbn_hw_call_back_on_drop)
        {
            qcril_mbn_hw_call_back_on_drop();
        }
        else
        {
            qcril_mbn_sw_update_init(TRUE);
        }
    }
    else
    {
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_ACTIVATING_CONFIG);
        qcril_mbn_hw_activate_selected_config(0, PDC_CONFIG_TYPE_MODEM_PLATFORM_V01);
    }
}

/*=========================================================================

  FUNCTION:  qcril_mbn_hw_select_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_HW_SELECT_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_hw_select_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno result = RIL_E_GENERIC_FAILURE;
    qcril_mbn_hw_select_config_info *req = NULL;
    qcril_reqlist_public_type qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_hw_select_config_info *)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_HW_SELECT_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            result = qcril_qmi_pdc_set_select_configuration(req->sub_id,
                                                            req->config_type,
                                                            req->config_id,
                                                            req->config_id_len);
        }
        else
        {
            QCRIL_LOG_ERROR("No memory to allocate reqlist");
        }

    } while (0);

    QCRIL_LOG_DEBUG("result: %d", result);
    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_hw_send_select_config_resp(result);
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_mbn_hw_select_config

===========================================================================*/
/*!
    @brief
    Select config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_select_config
(
    uint32_t  sub_id,
    pdc_config_type_enum_v01 config_type,
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01],
    uint32_t config_id_len
)
{
    qcril_mbn_hw_select_config_info req;

    memset(&req, 0, sizeof(req));
    req.sub_id          = sub_id;
    req.config_type     = config_type;
    memcpy(req.config_id, config_id, config_id_len);
    req.config_id_len   = config_id_len;

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_HW_SELECT_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}
/*===========================================================================

  FUNCTION qcril_mbn_hw_send_activate_config_resp

===========================================================================*/
/*!
    @brief
    Send activate_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_send_activate_config_resp
(
    RIL_Errno result
)
{
    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                       (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_ACTIVATION_FAILED);
        if (qcril_mbn_hw_call_back_on_drop)
        {
            qcril_mbn_hw_call_back_on_drop();
        }
        else
        {
            qcril_mbn_sw_update_init(TRUE);
        }
    }
    else
    {
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_ACTIVATION_COMPLETED);
        qcril_mbn_sw_update_init(TRUE);
    }
    return E_SUCCESS;
}

/*=========================================================================

  FUNCTION:  qcril_mbn_hw_activate_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_HW_ACTIVATE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_hw_activate_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno result = RIL_E_GENERIC_FAILURE;
    qcril_mbn_hw_config_req_type *req = NULL;
    qcril_reqlist_public_type qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_hw_config_req_type *)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_HW_ACTIVATE_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            result = qcril_qmi_pdc_activate_configuration_hdlr(req->sub_id,
                                                            req->config_type);
        }
        else
        {
            QCRIL_LOG_ERROR("No memory to allocate reqlist");
        }

    } while (0);

    QCRIL_LOG_DEBUG("result: %d", result);
    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_hw_send_activate_config_resp(result);
    }

    QCRIL_LOG_FUNC_RETURN();
}
/*===========================================================================

  FUNCTION  qcril_mbn_hw_activate_selected_config

===========================================================================*/
/*!
    @brief
    Activate selected_config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_activate_selected_config
(
    uint32_t  sub_id,
    pdc_config_type_enum_v01 config_type
)
{
    qcril_mbn_hw_select_config_info req;

    memset(&req, 0, sizeof(req));
    req.sub_id      = sub_id;
    req.config_type = config_type;

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_HW_ACTIVATE_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}

/*===========================================================================

  FUNCTION qcril_mbn_kick_ota_update_in_dedicated_thread

===========================================================================*/
/*!
    @brief
    Start a dedicated thread for SW MBN OTA update

    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
static IxErrnoType qcril_mbn_kick_ota_update_in_dedicated_thread(void)
{
    pthread_attr_t attr;
    pthread_t qmi_ril_mbn_ota_pid;
    IxErrnoType res;
    int conf;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    conf = pthread_create(&qmi_ril_mbn_ota_pid, &attr, qmi_ril_mbn_ota_thread_proc, NULL);
    qmi_ril_set_thread_name(qmi_ril_mbn_ota_pid, "MBN_OTA");
    pthread_attr_destroy(&attr);

    res =  (conf < 0 ) ? E_FAILURE : E_SUCCESS;
    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);

    return res;
}


/*===========================================================================

  FUNCTION qcril_mbn_hw_update_init

===========================================================================*/
/*!
    @brief
    Hardware update initilization

    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_update_init
(
    int apss_max_subscriptions,
    int modem_max_subscriptions,
    int is_modem_state_valid,
    boolean is_on_request
)
{
    qcril_mbn_hw_init_req hw_init_req;

    int          is_hw_enabled     = 1;
    int          is_sw_ota_enabled = 0;
    IxErrnoType  ret               = E_SUCCESS;
    int          is_copy_completed = 0;

    memset(&hw_init_req, 0, sizeof(hw_init_req));
    hw_init_req.modem_max_sub = modem_max_subscriptions;
    hw_init_req.apss_max_sub  = apss_max_subscriptions;

    qmi_ril_get_property_value_from_integer(QCRIL_MBN_HW_FEATURE_ENABLED,
                                            &is_hw_enabled, 0);
    qmi_ril_get_property_value_from_integer(QCRIL_MBN_SW_OTA_FOR_MODEM_CENTRIC,
                                            &is_sw_ota_enabled, 0);

    is_copy_completed = qcril_mbn_is_copy_completed();
    MBN_HW_CONFIG_LOCK();
    if (!is_on_request)
    {
        qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                           (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
    }

    if (!is_modem_state_valid)
    {
        QCRIL_LOG_INFO("Invalid modem state");
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_ACTIVATION_NOT_NEEDED);
    }
    else if (!is_copy_completed)
    {
        QCRIL_LOG_INFO("No modem config directory");
        ret = E_NO_ENTRY;
    }
    else if (qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID)
    {
        QCRIL_LOG_INFO("HW update will be done by primary rild");
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_ACTIVATION_NOT_NEEDED);
    }
    else if (is_sw_ota_enabled)
    {
        qcril_mbn_kick_ota_update_in_dedicated_thread();
    }
    else if (!is_hw_enabled)
    {
        QCRIL_LOG_INFO("HW update feature disabled");
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_ACTIVATION_NOT_NEEDED);

        if (qmi_ril_get_process_instance_id() == QCRIL_DEFAULT_INSTANCE_ID)
        {
            (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_DATA_ON_STACK,
                                    QCRIL_EVT_REQUEST_MBN_HW_DISABLED_INIT,
                                    (void *) &hw_init_req,
                                    sizeof(hw_init_req),
                                    (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);
        }
        else
        {
            QCRIL_LOG_INFO("HW for SW update will be done by primary rild");
        }
    }
    else if (is_on_request &&
            (QCRIL_MBN_HW_STATE_NONE != qcril_mbn_hw_get_cur_state()))
    {
        QCRIL_LOG_INFO("Hardware init already called");
    }
    else
    {
        qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_INITIALIZING);

        (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                                QCRIL_DEFAULT_MODEM_ID,
                                QCRIL_DATA_ON_STACK,
                                QCRIL_EVT_REQUEST_MBN_HW_INIT,
                                (void *) &hw_init_req,
                                sizeof(hw_init_req),
                                (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);
    }
    MBN_HW_CONFIG_UNLOCK();
    return ret;
}

/*===========================================================================

  FUNCTION qcril_mbn_hw_get_hw_name_to_look_for

===========================================================================*/
/*!
    @brief
    Get hardware name to look for

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
char *qcril_mbn_hw_get_hw_name_to_look_for
(
    int max_subscriptions
)
{
    char *hw_name =  NULL;

    QCRIL_LOG_DEBUG("max_sub %d", max_subscriptions);
    if (max_subscriptions == QMI_RIL_NUM_TRIPLE_SUB)
    {
        hw_name = "LA_TS";
    }
    else if (max_subscriptions == QMI_RIL_NUM_DUAL_SUB)
    {
        hw_name = "LA_DS";
    }
    else
    {
        hw_name = "LA_SS";
    }

    QCRIL_LOG_DEBUG("hw_name: %s", hw_name);
    return hw_name;
}

/*===========================================================================

  FUNCTION qcril_mbn_hw_update_init_hdlr

===========================================================================*/
/*!
    @brief
    Hardware update init handler

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_hw_update_init_hdlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
)
{
    qcril_mbn_hw_init_req *hw_init_req  = NULL;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();
    do {

        if (params_ptr == NULL)
        {
            QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
            break;
        }

        hw_init_req = (qcril_mbn_hw_init_req*)params_ptr->data;
        if (hw_init_req == NULL)
        {
            QCRIL_LOG_ERROR("%s", "null hw_init_req");
            break;
        }

        qcril_mbn_max_apss_instance_id = hw_init_req->apss_max_sub;

        if (hw_init_req->apss_max_sub != hw_init_req->modem_max_sub)
        {
            /* load custom mbn files to db */
            if (qcril_mbn_hw_load_to_db() != E_SUCCESS)
            {
                qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_ACTIVATION_NOT_NEEDED);
                if (qcril_mbn_hw_call_back_on_drop)
                {
                    qcril_mbn_hw_call_back_on_drop();
                }
                else
                {
                    qcril_mbn_sw_update_init(TRUE);
                }

                break;
            }

            /* get current active hw config */
            qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_GETTING_CURRENT_CONFIG);
            qcril_mbn_hw_retrieve_selected_config();
        }
        else
        {
            qcril_mbn_hw_set_cur_state(QCRIL_MBN_HW_STATE_ACTIVATION_NOT_NEEDED);
            if (qcril_mbn_hw_call_back_on_drop)
            {
                qcril_mbn_hw_call_back_on_drop();
            }
            else
            {
                qcril_mbn_sw_update_init(TRUE);
            }
        }

    } while(0);

    QCRIL_LOG_FUNC_RETURN();
    return;
}

/*===========================================================================

  FUNCTION qcril_mbn_hw_update_disabled_init_hdlr

===========================================================================*/
/*!
    @brief
    Hardware update disabled init handler

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_hw_update_disabled_init_hdlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
)
{
    qcril_mbn_hw_init_req *hw_init_req  = NULL;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();
    do {

        if (params_ptr == NULL)
        {
            QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
            break;
        }

        hw_init_req = (qcril_mbn_hw_init_req*)params_ptr->data;
        if (hw_init_req == NULL)
        {
            QCRIL_LOG_ERROR("%s", "null hw_init_req");
            break;
        }

        qcril_mbn_max_apss_instance_id = hw_init_req->apss_max_sub;

        qcril_mbn_sw_update_init(TRUE);

    } while(0);

    QCRIL_LOG_FUNC_RETURN();
    return;
}

/*===========================================================================

  FUNCTION qcril_mbn_hw_inform_wait_drop

===========================================================================*/
/*!
    @brief
    Inform if process is dropped

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_hw_inform_wait_drop
(
    qcril_mbn_call_back_on_drop_type call_back_on_drop
)
{
    qcril_mbn_hw_call_back_on_drop = call_back_on_drop;
}

#ifdef QMI_RIL_UTF
#define HW_CFG_PATH "data/modem_config/mcfg_hw/"
#else
#define HW_CFG_PATH DATA_PARTITION_ROOT "radio/modem_config/mcfg_hw/"
#endif
/*===========================================================================

  FUNCTION qcril_mbn_hw_load_to_db

===========================================================================*/
/*!
    @brief
    Load hw mbn to db

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_load_to_db
(
    void
)
{
#define QCRIL_MBN_HW_MAX_FILES (50)
    char *hw_cfg_path     = HW_CFG_PATH;
    char *file_names[QCRIL_MBN_HW_MAX_FILES] = {NULL};
    int   number_of_files = 0;
    int   index           = 0;
    int   int_value       = 0;
    char  *str_value      = NULL;
    unsigned char *meta_data = NULL;
    unsigned int meta_data_len;
    IxErrnoType ret = E_FAILURE;
    int i;
    static   int mbn_loaded = 0;

    QCRIL_LOG_FUNC_ENTRY();

    qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);

    qcril_db_begin();
    if (access (hw_cfg_path, F_OK))
    {
        QCRIL_LOG_ESSENTIAL("QCRIL_ERROR:IO: No hw mbn config directory");
    }
    else if (mbn_loaded || qcril_qmi_hw_mbn_loaded_to_db())
    {
      QCRIL_LOG_INFO("HW mbn already loaded to DB");
      ret = E_SUCCESS;
    }
    else
    {
        qcril_dir_retrieve_all_files_recursively(hw_cfg_path,
                                                 file_names,
                                                 &number_of_files,
                                                 QCRIL_MBN_HW_MAX_FILES);

        QCRIL_LOG_INFO("Total number of files retrieved: %d", number_of_files);

        while (index < number_of_files)
        {
            QCRIL_LOG_INFO("File%d: %s", index+1, file_names[index]);
            ret = qcril_mbn_db_add_hw_mbn_file(file_names[index]);
            if (ret != E_SUCCESS)
            {
                QCRIL_LOG_DEBUG("db add hw mbn file failed");
            }

            ret = qcril_mbn_meta_retrieve_mbn_meta_data(file_names[index],
                                                  &meta_data,
                                                  &meta_data_len);
            if (ret != E_SUCCESS || meta_data == NULL )
            {
                QCRIL_LOG_DEBUG("retrieve mbn meta from file failed");
                continue;
            }

            ret = qcril_mbn_meta_retrieve_hw_qc_version(meta_data,
                                                  meta_data_len,
                                                  &int_value);
            if (ret != E_SUCCESS)
            {
                QCRIL_LOG_DEBUG("retrieve hw qc_version failed");
            }

            if (ret == E_SUCCESS)
            {
                ret = qcril_mbn_db_add_hw_mbn_qc_version(file_names[index],
                                                   int_value);
                if (ret != E_SUCCESS)
                {
                    QCRIL_LOG_DEBUG("db add hw qc_version failed");
                    break;
                }
            }

            ret = qcril_mbn_meta_retrieve_hw_oem_version(meta_data,
                                                  meta_data_len,
                                                  &int_value);
            if (ret != E_SUCCESS)
            {
                QCRIL_LOG_DEBUG("retrieve hw oem_version failed");
                break;
            }

            if (ret == E_SUCCESS)
            {
                ret = qcril_mbn_db_add_hw_mbn_oem_version(file_names[index],
                                                   int_value);
                if (ret != E_SUCCESS)
                {
                    QCRIL_LOG_DEBUG("db add hw mbn oem_version fails");
                    break;
                }
            }

            ret= qcril_mbn_meta_retrieve_hw_name(meta_data,
                                            meta_data_len,
                                            &str_value);
            if (ret != E_SUCCESS)
            {
                QCRIL_LOG_DEBUG("retrieve hw name failed");
            }

            if (ret == E_SUCCESS)
            {
                if(str_value)
                {
                    ret = qcril_mbn_db_add_hw_mbn_hw_name(file_names[index],
                                                       str_value);
                    if (ret != E_SUCCESS)
                    {
                        QCRIL_LOG_DEBUG("db add hw name failed");
                        break;
                    }
                }
                else
                {
                    QCRIL_LOG_DEBUG("str_value is null");
                }
            }

            if (str_value)
            {
                free(str_value);
                str_value = NULL;
            }

            if (meta_data)
            {
                free(meta_data);
                meta_data = NULL;
            }

            index++;
        }

        for (i = 0; i < QCRIL_MBN_HW_MAX_FILES; ++i)
        {
            if (file_names[i])
            {
                free(file_names[i]);
                file_names[i] = NULL;
            }
        }
    }

    if (ret == E_SUCCESS)
    {
        mbn_loaded = 1;
        qmi_ril_set_property_value_to_integer(QCRIL_MBN_HW_MBN_LOADED,
                                              mbn_loaded);
        qcril_db_update_cur_modem_version();
        qcril_db_commit();
        qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);
    }
    else
    {
        qcril_db_abort();
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

/*===========================================================================

  FUNCTION qcril_mbn_hw_get_hw_config_from_db

===========================================================================*/
/*!
    @brief
    Get hw config from db

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_get_hw_config_from_db
(
    char   **mbn_file_name,
    char   *device_type
)
{
    QCRIL_LOG_FUNC_ENTRY();
    return qcril_mbn_db_retrieve_hw_mbn_file_for_hw_type(mbn_file_name, device_type);
    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION:  qcril_qmi_mbn_hw_init
===========================================================================*/
/*!
    @brief
    Initialize the PDC subsystem of the RIL.

    @return
    None.
*/
 /*=========================================================================*/

void qcril_qmi_mbn_hw_init
(
  void
)
{
  QCRIL_LOG_FUNC_ENTRY();

  pthread_mutexattr_init(&qcril_mbn_hw_config_mutex_attr);
  pthread_mutex_init(&qcril_mbn_hw_config_mutex, &qcril_mbn_hw_config_mutex_attr);

  QCRIL_LOG_FUNC_RETURN();

  return;
}

/*===========================================================================

  FUNCTION qcril_qmi_hw_mbn_reboot_cleanup

===========================================================================*/
/*!
    @brief
    Clean up hw mbn state

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_hw_mbn_reboot_cleanup (void)
{
  qcril_mbn_hw_cur_state = QCRIL_MBN_HW_STATE_NONE;
  qcril_mbn_max_apss_instance_id = 1;

  memset(hw_config_id, 0, PDC_CONFIG_ID_SIZE_MAX_V01);
  hw_config_id_len = 0;

  memset(active_hw_config_id, 0, PDC_CONFIG_ID_SIZE_MAX_V01);
  active_hw_config_id_len = 0;
} // qcril_qmi_hw_mbn_reboot_cleanup

/*===========================================================================

  FUNCTION qcril_qmi_hw_mbn_db_loaded

===========================================================================*/
/*!
    @brief
    Check if hw db is loaded or not

    @return
    1 if db is loaded, 0 otherwise
*/
/*=========================================================================*/
int qcril_qmi_hw_mbn_loaded_to_db (void)
{
    return !qcril_db_is_hw_mbn_reevaluation_needed();
} // qcril_qmi_hw_mbn_db_loaded
