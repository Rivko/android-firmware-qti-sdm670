/******************************************************************************

  @file    qcril_mbn_sw_update.c
  @brief   Provides interface to read mbn file

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/

#include "qcril_file_utils.h"
#include "qcril_mbn_sw_update.h"
#include "qcril_mbn_db.h"
#include "qcril_mbn_meta.h"
#include "qcril_log.h"
#include "qcril_dir_utils.h"
#include "qcrili.h"
#include "persistent_device_configuration_v01.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_pdc.h"
#include "qcril_qmi_nas.h"
#include "qcril_qmi_client.h"
#include "qcril_other.h"
#include "qcril_db.h"
#include "settings.h"
#include <cutils/properties.h>

#define QCRIL_MBN_SW_FEATURE_ENABLED "persist.vendor.radio.sw_mbn_update"
#define QCRIL_MBN_SW_VOLTE           "persist.vendor.radio.sw_mbn_volte"
#define QCRIL_MBN_SW_OPENMKT         "persist.vendor.radio.sw_mbn_openmkt"
#define QCRIL_MBN_SW_LONG_IIN        "persist.vendor.radio.long_iin_mbn"

typedef struct {
    uint32_t                 sub_id;
    pdc_config_type_enum_v01 config_type;
} qcril_mbn_sw_config_req_type;

typedef struct {
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
    uint32_t config_id_len;
} qcril_mbn_sw_config_id_info;

typedef struct {
    uint32_t sub_id;
    char    *file_name;
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
    uint32_t config_id_len;
} qcril_mbn_sw_load_config_info;

typedef struct {
    uint32_t sub_id;
    pdc_config_type_enum_v01 config_type;
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
    uint32_t config_id_len;
} qcril_mbn_sw_config_info;

typedef struct {
    uint32_t sub_id;
    pdc_config_type_enum_v01 config_type;
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
    uint32_t config_id_len;
} qcril_mbn_sw_select_config_info;

typedef struct {
    uint32_t cur_stat;
    void (*on_success)(void *);
    void (*on_failure)(void *);
} qcril_mbn_sw_state_machine_type;

typedef enum {
    QCRIL_MBN_SW_STATE_BOOTUP_NEEDED                 = 0,
    QCRIL_MBN_SW_STATE_NONE                          = 1,
    QCRIL_MBN_SW_STATE_INITIALIZING                  = 2,
    QCRIL_MBN_SW_STATE_MBN_LOADING_TO_DB             = 3,
    QCRIL_MBN_SW_STATE_MBN_LOADING_FAILED            = 4,
    QCRIL_MBN_SW_STATE_GETTING_CURRENT_CONFIG        = 5,
    QCRIL_MBN_SW_STATE_GETTING_CURRENT_CONFIG_INFO   = 6,
    QCRIL_MBN_SW_STATE_DEACTIVATING_CURRENT_CONFIG   = 7,
    QCRIL_MBN_SW_STATE_DEACTIVATION_FAILED           = 8,
    QCRIL_MBN_SW_STATE_DELETING_CURRENT_CONFIG       = 9,
    QCRIL_MBN_SW_STATE_DELETE_LOADING_CONFIG         = 10,
    QCRIL_MBN_SW_STATE_LOADING_CONFIG                = 11,
    QCRIL_MBN_SW_STATE_LOAD_FAILED                   = 12,
    QCRIL_MBN_SW_STATE_LOAD_INTERRUPTED              = 13,
    QCRIL_MBN_SW_STATE_SELECTING_CONFIG              = 14,
    QCRIL_MBN_SW_STATE_SELECTION_FAILED              = 15,
    QCRIL_MBN_SW_STATE_SELECTION_SUCCESS             = 16,
    QCRIL_MBN_SW_STATE_ACTIVATING_CONFIG             = 17,
    QCRIL_MBN_SW_STATE_ACTIVATION_COMPLETED          = 18,
    QCRIL_MBN_SW_STATE_ACTIVATION_FAILED             = 19,
    QCRIL_MBN_SW_STATE_ACTIVATION_NOT_NEEDED         = 20,
    QCRIL_MBN_SW_STATE_ACTIVATION_NOT_NEEDED_FOR_DEV = 21,
    QCRIL_MBN_SW_STATE_COUNTING_PENDING_CONFIGS      = 22,
    QCRIL_MBN_SW_STATE_CLEANUP_INACTIVE_CONFIG       = 23
} qcril_mbn_sw_state_type;

qcril_mbn_sw_state_type qcril_mbn_sw_cur_state = QCRIL_MBN_SW_STATE_BOOTUP_NEEDED;

char    qcril_mbn_sw_iccid[PDC_CONFIG_ID_SIZE_MAX_V01];
int     qcril_mbn_sw_iccid_len  = 0;

#define QCRIL_MBN_SW_MAX_INSTANCE_ID (4)

char     sw_config_id[QCRIL_MBN_SW_MAX_INSTANCE_ID][PDC_CONFIG_ID_SIZE_MAX_V01];
uint32_t sw_config_id_len[QCRIL_MBN_SW_MAX_INSTANCE_ID];

char     active_sw_config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
uint32_t active_sw_config_id_len;

int     qcril_mbn_cur_instance_id = 0;
int     qcril_mbn_cur_sub_id      = 0;
int     qcril_mbn_need_restart    = 0;
int     qcril_mbn_last_pending_sub = -1;
char    qcril_mbn_iccid_cache[QCRIL_MBN_SW_MAX_INSTANCE_ID][PROP_VALUE_MAX] = {{0}};
char    qcril_mbn_mcc_cache[QCRIL_MBN_SW_MAX_INSTANCE_ID][5] = {{0}};
char    qcril_mbn_mnc_cache[QCRIL_MBN_SW_MAX_INSTANCE_ID][5] = {{0}};
int     qcril_mbn_sub_id_cache[QCRIL_MBN_SW_MAX_INSTANCE_ID] = {0};
char    qcril_mbn_cur_instance_iccid[PROP_VALUE_MAX];
char    qcril_mbn_cur_instance_mcc[5];
char    qcril_mbn_cur_instance_mnc[5];
extern int qcril_mbn_max_apss_instance_id;
bool    qcril_mbn_feature_modem_disabled = false;
qcril_mbn_sw_modem_switch_state qcril_modem_switch_state;
int     qcril_check_pending_subid = 0;
int     qcril_pending_configs_count = 0;

pthread_mutex_t        qcril_mbn_sw_config_mutex;
pthread_mutexattr_t    qcril_mbn_sw_config_mutex_attr;

#define MBN_SW_CONFIG_LOCK() do { \
        QCRIL_LOG_INFO("LOCK MBN_SW_CONFIG_LOCK"); \
        pthread_mutex_lock(&qcril_mbn_sw_config_mutex); \
    }while(0)

#define MBN_SW_CONFIG_UNLOCK() do { \
        QCRIL_LOG_INFO("UNLOCK MBN_SW_CONFIG_LOCK"); \
        pthread_mutex_unlock(&qcril_mbn_sw_config_mutex); \
    }while(0)

IxErrnoType qcril_mbn_sw_select_config
(
    uint32_t  sub_id,
    pdc_config_type_enum_v01 config_type,
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01],
    uint32_t config_id_len
);

/*===========================================================================

  FUNCTION  qcril_mbn_sw_load_config

===========================================================================*/
/*!
    @brief
    Load config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_load_config
(
    char *file_name
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_get_sw_name_to_look_for

===========================================================================*/
/*!
    @brief
    Get sw_name to look for

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
char *qcril_mbn_sw_get_sw_name_to_look_for
(
    int max_subscriptions
);

IxErrnoType qcril_mbn_sw_get_sw_config_from_db
(
    char   **mbn_file_name,
    char   *device_type
);

/*===========================================================================

  FUNCTION  qcril_mbn_sw_delete_config

===========================================================================*/
/*!
    @brief
    Delete config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_delete_config
(
    char      *config_id,
    uint32_t  config_id_len
);

/*===========================================================================

  FUNCTION  qcril_mbn_sw_deactivate_config

===========================================================================*/
/*!
    @brief
    Deactivate config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_deactivate_config
(
    uint32_t                 sub_id,
    pdc_config_type_enum_v01 config_type
);


/*===========================================================================

  FUNCTION  qcril_mbn_sw_take_end_action

===========================================================================*/
/*!
    @brief
    Delete config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
static void qcril_mbn_sw_take_end_action();

/*===========================================================================

  FUNCTION qcril_qmi_sw_mbn_select_mbn

===========================================================================*/
/*!
    @brief
    Select MBN to be loaded

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_sw_mbn_select_mbn
(
    char   **mbn_file_name
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_update_sub_id_cache

===========================================================================*/
/*!
    @brief
    update sub_id cache

    @return
    None
*/
/*=========================================================================*/
void qcril_mbn_sw_update_sub_id_cache
(
    int instance,
    int temp_sub_id
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_set_cur_state

===========================================================================*/
/*!
    @brief
    Set sw mbn current state

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
static void qcril_mbn_sw_set_cur_state
(
    qcril_mbn_sw_state_type state
)
{
    QCRIL_LOG_DEBUG("new_state: %d", state);
    MBN_SW_CONFIG_LOCK();
    qcril_mbn_sw_cur_state = state;
    MBN_SW_CONFIG_UNLOCK();
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_get_cur_state

===========================================================================*/
/*!
    @brief
    Get sw mbn current state

    @return
    state
*/
/*=========================================================================*/
qcril_mbn_sw_state_type qcril_mbn_sw_get_cur_state
(
    void
)
{
    qcril_mbn_sw_state_type state;
    MBN_SW_CONFIG_LOCK();
    state = qcril_mbn_sw_cur_state;
    MBN_SW_CONFIG_UNLOCK();
    QCRIL_LOG_DEBUG("state: %d", state);
    return state;
}

/*===========================================================================

  FUNCTION  qcril_mbn_sw_activate_selected_config

===========================================================================*/
/*!
    @brief
    Activate selected_config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_activate_selected_config
(
    uint32_t  sub_id,
    pdc_config_type_enum_v01 config_type
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_is_completed_for_a_sub

===========================================================================*/
/*!
    @brief
    Check if sw mbn is completed from a sub

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
boolean qcril_mbn_sw_is_completed_for_a_sub
(
    void
)
{
    boolean result = FALSE;
    switch (qcril_mbn_sw_cur_state)
    {
        case QCRIL_MBN_SW_STATE_LOAD_FAILED:
        case QCRIL_MBN_SW_STATE_SELECTION_FAILED:
        case QCRIL_MBN_SW_STATE_SELECTION_SUCCESS:
        case QCRIL_MBN_SW_STATE_ACTIVATION_NOT_NEEDED:
        case QCRIL_MBN_SW_STATE_DEACTIVATION_FAILED:
        {
            result = TRUE;
            break;
        }
        default:
        {
            break;
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_is_completed

===========================================================================*/
/*!
    @brief
    Check if sw mbn is completed

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
boolean qcril_mbn_sw_is_completed
(
    void
)
{
    boolean result = FALSE;

    /*check sw_mbn_update property.if it is disabled there no need to check
      sw_mbn current state and return TRUE to process PDC OEM requests.
    */
    int is_enabled = 1;
    qmi_ril_get_property_value_from_integer(QCRIL_MBN_SW_FEATURE_ENABLED,
                                            &is_enabled, 0);
    if(!is_enabled) {
        return TRUE;
    }

    switch (qcril_mbn_sw_cur_state)
    {
        case QCRIL_MBN_SW_STATE_ACTIVATION_COMPLETED:
        case QCRIL_MBN_SW_STATE_CLEANUP_INACTIVE_CONFIG:
        case QCRIL_MBN_SW_STATE_ACTIVATION_NOT_NEEDED_FOR_DEV:
        {
            result = TRUE;
            break;
        }
        default:
        {
            break;
        }
    }

    return result;
}
/*===========================================================================

  FUNCTION qcril_mbn_sw_send_get_selected_resp

===========================================================================*/
/*!
    @brief
    Send get_selected response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_get_selected_resp
(
    RIL_Errno result,
    uint8_t  *config_id,
    uint32_t  config_id_len
)
{
    char    *file_name = NULL;
    uint8_t  load_config_id[PDC_CONFIG_ID_SIZE_MAX_V01] = {0};
    int      len = 0;
    int      start_pos   = 0;
    qcril_mbn_sw_state_type state = qcril_mbn_sw_get_cur_state();

    if(state != QCRIL_MBN_SW_STATE_GETTING_CURRENT_CONFIG)
    {
        QCRIL_LOG_DEBUG("Invalid state to proceed: %d", state);
        return E_SUCCESS;
    }
    // Only free it when state is expected. When MBN SW init starts,
    // it will free the MBN internal token
    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
               (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);

    if (result != RIL_E_SUCCESS)
    {
        /* select mbn file to load */
        QCRIL_LOG_DEBUG("Failed to get selected config for SUB%d", qcril_mbn_cur_instance_id);
        qcril_qmi_sw_mbn_select_mbn(&file_name);

        if (file_name)
        {
            if ((strlen(file_name) + 1) >= sizeof(load_config_id))
            {
                start_pos = (strlen(file_name) + 1) - sizeof(load_config_id);
            }

            len = snprintf((char*)load_config_id, PDC_CONFIG_ID_SIZE_MAX_V01, "%s%d",
                    file_name + start_pos, qcril_mbn_cur_instance_id);

            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_DELETE_LOADING_CONFIG);
            qcril_mbn_sw_delete_config((char *)load_config_id, len);
            free(file_name);
        }
        else
        {
            QCRIL_LOG_ERROR("Failed to get MBN file to be loaded for SUB%d", qcril_mbn_cur_instance_id);
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_LOAD_FAILED);
            qcril_mbn_sw_take_end_action();
        }
    }
    else
    {
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_GETTING_CURRENT_CONFIG_INFO);
        qcril_mbn_sw_retrieve_config_info((unsigned char *)config_id, config_id_len);
    }

    return E_SUCCESS;
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_query_selected_config_hndlr

===========================================================================*/
/*!
    @brief
    Query selected confuration

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_sw_query_selected_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno                     result = RIL_E_GENERIC_FAILURE;
    qcril_reqlist_public_type     qcril_req_info;
    qcril_mbn_sw_config_req_type *req = NULL;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_sw_config_req_type *)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_SW_GET_SELECTED_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            QCRIL_LOG_DEBUG("Get selected MBN config for SUB %d-%d",
                                     qcril_mbn_cur_instance_id, req->sub_id);
            result = qcril_qmi_pdc_get_selected_mbn_config(req->sub_id,
                                                           req->config_type);
        }
        else
        {
            QCRIL_LOG_ERROR("Failed to add MBN internal event in queue...");
        }

    } while (0);

    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_sw_send_get_selected_resp(result, NULL, 0);
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_mbn_sw_retrieve_selected_config

===========================================================================*/
/*!
    @brief
    Get selected_config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_retrieve_selected_config
(
    void
)
{
    qcril_mbn_sw_config_req_type req;
    int qcril_mbn_temp_sub_id = 0;

    memset(&req, 0, sizeof(req));
    MBN_SW_CONFIG_LOCK();
    qcril_mbn_cur_sub_id = qcril_qmi_pdc_get_sub_id(qcril_mbn_cur_instance_id);
    qcril_mbn_temp_sub_id = qcril_mbn_cur_sub_id;
    MBN_SW_CONFIG_UNLOCK();
    qcril_mbn_sw_update_sub_id_cache(qcril_mbn_cur_instance_id, qcril_mbn_temp_sub_id);

    req.sub_id      = qcril_mbn_temp_sub_id;
    req.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;

    qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_GETTING_CURRENT_CONFIG);
    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_SW_GET_SELECTED_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}
/*===========================================================================

  FUNCTION qcril_mbn_sw_send_get_config_info_resp

===========================================================================*/
/*!
    @brief
    Send get_config_indo response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_get_config_info_resp
(
    RIL_Errno result,
    boolean has_base_version,
    uint32_t base_version,
    boolean has_config_version,
    uint32_t config_version
)
{
    char *file_name = NULL;
    int is_updated = TRUE;

    uint32_t len         = 0;
    int start_pos   = 0;

    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
    int qcril_mbn_temp_sub_id = 0;
    qcril_mbn_sw_state_type state = qcril_mbn_sw_get_cur_state();

    QCRIL_NOTUSED(base_version);
    QCRIL_NOTUSED(has_base_version);

    if(state != QCRIL_MBN_SW_STATE_GETTING_CURRENT_CONFIG_INFO)
    {
        QCRIL_LOG_DEBUG("Invalid state to proceed: %d", state);
        return E_SUCCESS;
    }
    // Only free it when state is expected. When MBN SW init starts,
    // it will free the MBN internal token
    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
               (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);

    /* select mbn file to load */
    qcril_qmi_sw_mbn_select_mbn(&file_name);

    if (file_name)
    {
        if ((strlen(file_name) + 1) >= sizeof(config_id))
        {
            start_pos = (strlen(file_name) + 1) - sizeof(config_id);
        }

        len = snprintf((char*)config_id, PDC_CONFIG_ID_SIZE_MAX_V01, "%s%d",
                file_name + start_pos, qcril_mbn_cur_instance_id);
    }

    if (result != RIL_E_SUCCESS)
    {
        if (file_name)
        {
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_DELETE_LOADING_CONFIG);
            qcril_mbn_sw_delete_config((char *)config_id, len);
        }
        else
        {
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_LOAD_FAILED);
            qcril_mbn_sw_take_end_action();
        }
    }
    else
    {
        if (!file_name || !strlen(file_name))
        {
            is_updated = 0;
        }
        else if ((len == active_sw_config_id_len) &&
                !memcmp(active_sw_config_id, config_id, len) && has_config_version)
        {
            is_updated = qcril_mbn_db_is_sw_version_updated(file_name, config_version);
        }

        if (is_updated)
        {
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_DEACTIVATING_CURRENT_CONFIG);
            MBN_SW_CONFIG_LOCK();
            qcril_mbn_temp_sub_id = qcril_mbn_cur_sub_id;
            MBN_SW_CONFIG_UNLOCK();
            qcril_mbn_sw_deactivate_config(qcril_mbn_temp_sub_id,
                                           PDC_CONFIG_TYPE_MODEM_SW_V01);
        }
        else
        {
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATION_NOT_NEEDED);
            qcril_mbn_sw_take_end_action();
        }
    }

    if (file_name)
    {
        free(file_name);
    }
    return E_SUCCESS;
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_get_config_info_hndlr

===========================================================================*/
/*!
    @brief
    Handle get_config_info

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_sw_get_config_info_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno                     result = RIL_E_GENERIC_FAILURE;
    qcril_reqlist_public_type     qcril_req_info;
    qcril_mbn_sw_config_info     *req = NULL;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_sw_config_info*)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_SW_GET_CONFIG_INFO,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            result = qcril_qmi_pdc_get_config_info((char *)req->config_id,
                                                   req->config_id_len,
                                                   PDC_CONFIG_TYPE_MODEM_SW_V01);
        }
        else
        {
            QCRIL_LOG_ERROR("Failed to add MBN internal event in queue...");
        }

    } while (0);

    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_sw_send_get_config_info_resp(result, 0, 0, 0, 0);
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_mbn_sw_retrieve_config_info

===========================================================================*/
/*!
    @brief
    Get config_info

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_retrieve_config_info
(
    uint8_t *config_id,
    int32_t  config_id_len
)
{
    qcril_mbn_sw_config_info req;
    int qcril_mbn_temp_sub_id = 0;

    memset(&req, 0, sizeof(req));
    MBN_SW_CONFIG_LOCK();
    qcril_mbn_cur_sub_id = qcril_qmi_pdc_get_sub_id(qcril_mbn_cur_instance_id);
    qcril_mbn_temp_sub_id = qcril_mbn_cur_sub_id;
    MBN_SW_CONFIG_UNLOCK();
    qcril_mbn_sw_update_sub_id_cache(qcril_mbn_cur_instance_id, qcril_mbn_temp_sub_id);

    req.sub_id      = qcril_mbn_temp_sub_id;
    req.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
    // TODO: Verify
    if (config_id)
    {
      memcpy(req.config_id, config_id, config_id_len);
      memcpy(active_sw_config_id, config_id, config_id_len);
      memset(sw_config_id[qcril_mbn_cur_instance_id], 0,
             sizeof(sw_config_id[qcril_mbn_cur_instance_id]));
      memcpy(sw_config_id[qcril_mbn_cur_instance_id], config_id,
               config_id_len);
      sw_config_id_len[qcril_mbn_cur_instance_id] = config_id_len;
    }

    req.config_id_len   = config_id_len;
    active_sw_config_id_len = config_id_len;

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_SW_GET_CONFIG_INFO,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_deactivate_config_resp

===========================================================================*/
/*!
    @brief
    Send deactivaet_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_deactivate_config_resp
(
    RIL_Errno result
)
{
    qcril_mbn_sw_state_type state = qcril_mbn_sw_get_cur_state();

    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                       (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
    if(state == QCRIL_MBN_SW_STATE_DEACTIVATING_CURRENT_CONFIG)
    {
        if (result != RIL_E_SUCCESS)
        {
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_DEACTIVATION_FAILED);
            qcril_mbn_sw_take_end_action();
        }
        else
        {
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_DELETING_CURRENT_CONFIG);
            //Skip deleting current config as we are doing it at the end.
            //qcril_mbn_sw_delete_config((char *)active_sw_config_id, active_sw_config_id_len);
            qcril_mbn_sw_send_delete_config_resp(RIL_E_SUCCESS);
        }
    }
    else
    {
        QCRIL_LOG_DEBUG("Invalid state to proceed: %d", state);
    }

    return E_SUCCESS;
}

/*=========================================================================

  FUNCTION:  qcril_mbn_sw_deactivate_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_SW_DEACTIVATE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_deactivate_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno                       result = RIL_E_GENERIC_FAILURE;
    qcril_mbn_sw_config_req_type   *req = NULL;
    qcril_reqlist_public_type qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_sw_config_req_type *)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_SW_DEACTIVATE_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            result = qcril_qmi_pdc_deactivate_configuration_hdlr(req->sub_id,
                                                                 req->config_type);
        }
        else
        {
            QCRIL_LOG_ERROR("Failed to add MBN internal event in queue...");
        }

    } while (0);

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_mbn_sw_deactivate_config

===========================================================================*/
/*!
    @brief
    Deactivate config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_deactivate_config
(
    uint32_t                 sub_id,
    pdc_config_type_enum_v01 config_type
)
{
    qcril_mbn_sw_config_req_type req;
    int qcril_mbn_temp_sub_id = 0;

    memset(&req, 0, sizeof(req));
    MBN_SW_CONFIG_LOCK();
    qcril_mbn_cur_sub_id = qcril_qmi_pdc_get_sub_id(qcril_mbn_cur_instance_id);
    qcril_mbn_temp_sub_id = qcril_mbn_cur_sub_id;
    MBN_SW_CONFIG_UNLOCK();
    qcril_mbn_sw_update_sub_id_cache(qcril_mbn_cur_instance_id, qcril_mbn_temp_sub_id);

    QCRIL_NOTUSED(sub_id);
    QCRIL_NOTUSED(config_type);

    req.sub_id      = qcril_mbn_temp_sub_id;
    req.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_SW_DEACTIVATE_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}
/*===========================================================================

  FUNCTION qcril_mbn_sw_send_delete_config_resp

===========================================================================*/
/*!
    @brief
    Send delete_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_delete_config_resp
(
    RIL_Errno result
)
{
    char *file_name = NULL;

    int len         = 0;
    int start_pos   = 0;

    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
    qcril_mbn_sw_state_type state = qcril_mbn_sw_get_cur_state();

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_NOTUSED(result);

    if ( state != QCRIL_MBN_SW_STATE_DELETE_LOADING_CONFIG &&
         state != QCRIL_MBN_SW_STATE_DELETING_CURRENT_CONFIG )
    {
        QCRIL_LOG_DEBUG("Invalid state to proceed: %d", state);
        return E_SUCCESS;
    }
    // Only free it when state is expected. When MBN SW init starts,
    // it will free the MBN internal token
    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
               (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);

    /* select mbn file to load */
    qcril_qmi_sw_mbn_select_mbn(&file_name);

    if (state == QCRIL_MBN_SW_STATE_DELETING_CURRENT_CONFIG)
    {
        if (file_name && strlen(file_name))
        {
            if ((strlen(file_name) + 1) >= sizeof(config_id))
            {
                start_pos = (strlen(file_name) + 1) - sizeof(config_id);
            }

            len = snprintf((char*)config_id, PDC_CONFIG_ID_SIZE_MAX_V01, "%s%d",
                    file_name + start_pos, qcril_mbn_cur_instance_id);

            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_DELETE_LOADING_CONFIG);
            qcril_mbn_sw_delete_config((char *)config_id, len);

            if (file_name)
            {
                free(file_name);
            }
        }
        else
        {
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_LOADING_CONFIG);
            qcril_mbn_sw_load_config(file_name);
        }
    }
    else
    {
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_LOADING_CONFIG);
        qcril_mbn_sw_load_config(file_name);
    }

    QCRIL_LOG_FUNC_RETURN();
    return E_SUCCESS;
}

/*=========================================================================

  FUNCTION:  qcril_mbn_sw_delete_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_SW_DELETE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_delete_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno result = RIL_E_GENERIC_FAILURE;
    qcril_mbn_sw_config_id_info *req = NULL;
    qcril_reqlist_public_type qcril_req_info;
    int is_modem_switched = 0;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    MBN_SW_CONFIG_LOCK();
    is_modem_switched = (qcril_modem_switch_state ==
                                QCRIL_MBN_SW_MODEM_SWITCH_STATE_COMPLETED);
    MBN_SW_CONFIG_UNLOCK();
    do
    {
        req = (qcril_mbn_sw_config_id_info *)params_ptr->data;
        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_SW_DELETE_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            QCRIL_LOG_INFO("config_id %s, config_id_len %d", req->config_id, req->config_id_len);

            if (is_modem_switched && qcril_mbn_sw_get_cur_state() == QCRIL_MBN_SW_STATE_DELETING_CURRENT_CONFIG)
            {
                char* config_id = (char*)req->config_id;
                int config_id_len = req->config_id_len;
                int cur_state = qcril_mbn_sw_get_cur_state();
                QCRIL_LOG_INFO("is_modem_switched %d, current_state %d", is_modem_switched, cur_state);
                // skipping deleting current config for flex mapping case
                if (config_id && *config_id && (config_id[config_id_len-1]-'0' != qcril_mbn_cur_instance_id))
                {
                    result = RIL_E_SUCCESS;
                    qcril_mbn_sw_send_delete_config_resp(result);
                    break;
                }
            }
            else
            {
                result = qcril_qmi_pdc_delete_configuration_hdlr(req->config_id,
                                                                 req->config_id_len,
                                                                 PDC_CONFIG_TYPE_MODEM_SW_V01);
            }
        }
        else
        {
            QCRIL_LOG_ERROR("Failed to add MBN internal event in queue...");
        }

    } while (0);

    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_sw_send_delete_config_resp(result);
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_mbn_sw_delete_config

===========================================================================*/
/*!
    @brief
    Delete config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_delete_config
(
    char      *config_id,
    uint32_t  config_id_len
)
{
    qcril_mbn_sw_config_id_info req = {{0}, 0};

    if (config_id)
    {
      memcpy(req.config_id, config_id, config_id_len);
    }

    QCRIL_LOG_DEBUG("SUB%d-config id %s", qcril_mbn_cur_instance_id, config_id);
    QCRIL_LOG_DEBUG("config id len %d", config_id_len);
    req.config_id_len   = config_id_len;

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_SW_DELETE_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_cleanup_config_resp

===========================================================================*/
/*!
    @brief
    Send cleanup_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_cleanup_config_resp
(
    RIL_Errno result
)
{
    qcril_mbn_sw_state_type state = qcril_mbn_sw_get_cur_state();

    if(state != QCRIL_MBN_SW_STATE_CLEANUP_INACTIVE_CONFIG)
    {
        QCRIL_LOG_DEBUG("Invalid state to proceed: %d", state);
        return E_SUCCESS;
    }
    // Only free it when state is expected. When MBN SW init starts,
    // it will free the MBN internal token
    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
               (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);

    qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATION_COMPLETED);
    qcril_mbn_cur_instance_id = 0;

    if (result != RIL_E_SUCCESS)
    {
        QCRIL_LOG_INFO("Cleanup failed");
    }
    else
    {
        QCRIL_LOG_INFO("Cleanup success");
    }

    return E_SUCCESS;
}
/*===========================================================================

  FUNCTION  qcril_mbn_cleanup_inactive_configs

===========================================================================*/
/*!
    @brief
    Delete all inactive configs

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_cleanup_inactive_configs()
{
    int qcril_mbn_temp_sub_id = 0;

    MBN_SW_CONFIG_LOCK();
    qcril_mbn_cur_sub_id = qcril_qmi_pdc_get_sub_id(qcril_mbn_cur_instance_id);
    qcril_mbn_temp_sub_id = qcril_mbn_cur_sub_id;
    MBN_SW_CONFIG_UNLOCK();
    qcril_mbn_sw_update_sub_id_cache(qcril_mbn_cur_instance_id, qcril_mbn_temp_sub_id);

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_SW_CLEANUP_CONFIG,
                            NULL,
                            0,
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}

/*=========================================================================

  FUNCTION:  qcril_mbn_sw_cleanup_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_SW_CLEANUP_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_cleanup_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno                       result = RIL_E_GENERIC_FAILURE;
    qcril_reqlist_public_type       qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_NOTUSED(params_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_SW_CLEANUP_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                              QCRIL_DEFAULT_MODEM_ID,
                              QCRIL_DATA_ON_STACK,
                              QCRIL_EVT_QMI_RIL_PDC_DELETE_ALL,
                              NULL,
                              0,
                              (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
            result = RIL_E_SUCCESS;
        }
        else
        {
            QCRIL_LOG_ERROR("Failed to new reqlist for MBN_SW_CLEANUP...");
        }

    } while (0);

    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_sw_send_cleanup_config_resp(result);
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================

  FUNCTION:  qcril_mbn_sw_count_pending_configs_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_SW_COUNT_PENDING_CONFIGS

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_count_pending_configs_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno                   result = RIL_E_GENERIC_FAILURE;
    qcril_reqlist_public_type   qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    qcril_check_pending_subid = 0;
    qcril_pending_configs_count = 0;

    do
    {
        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_SW_COUNT_PENDING_CONFIGS,
                                    NULL,
                                    &qcril_req_info);
        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_DATA_ON_STACK,
                                    QCRIL_EVT_REQUEST_MBN_SW_QUERY_PENDING_CONFIG,
                                    (void*)&qcril_check_pending_subid,
                                    sizeof(qcril_check_pending_subid),
                                    (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);
            result = RIL_E_SUCCESS;
        }
        else
        {
            QCRIL_LOG_ERROR("Failed to new reqlist for COUNT_PENDING_CONFIG");

        }
    } while (0);

    if (result != RIL_E_SUCCESS)
    {
        // at least, we should have 1 pending MBN
        qcril_pending_configs_count = 1;
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATING_CONFIG);
        if (qcril_mbn_last_pending_sub == -1)
        {
            QCRIL_LOG_ERROR("Failed to get pending sub, use SUB0 as default");
            qcril_mbn_sw_activate_selected_config(0, PDC_CONFIG_TYPE_MODEM_SW_V01);

        }
        else
        {
            qcril_mbn_sw_activate_selected_config(qcril_mbn_last_pending_sub,
                    PDC_CONFIG_TYPE_MODEM_SW_V01);
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_take_end_action

===========================================================================*/
/*!
    @brief
    Take end action

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
static void qcril_mbn_sw_take_end_action
(
    void
)
{
    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_DEBUG("current instance id %d", qcril_mbn_cur_instance_id);
    if (qcril_mbn_cur_instance_id >= (qcril_mbn_max_apss_instance_id - 1))
    {
        if (qcril_mbn_need_restart)
        {
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_COUNTING_PENDING_CONFIGS);
            (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_DATA_ON_STACK,
                                    QCRIL_EVT_REQUEST_MBN_SW_COUNT_PENDING_CONFIGS,
                                    NULL,
                                    0,
                                    (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);
            qcril_mbn_need_restart = 0;
        }
        else
        {
            QCRIL_LOG_DEBUG("Reset modem switch state...");
            MBN_SW_CONFIG_LOCK();
            qcril_modem_switch_state = QCRIL_MBN_SW_MODEM_SWITCH_STATE_NONE;
            MBN_SW_CONFIG_UNLOCK();

            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_CLEANUP_INACTIVE_CONFIG);
            qcril_mbn_cur_instance_id = 0;
            qcril_mbn_cleanup_inactive_configs();
        }
    }
    else
    {
        qcril_mbn_sw_update_init(FALSE);
    }
    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_load_config_resp

===========================================================================*/
/*!
    @brief
    Send load_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_load_config_resp
(
    uint32_t  error
)
{
    int qcril_mbn_temp_sub_id = 0;
    int is_modem_switched = 0;
    int is_modem_switching = 0;
    MBN_SW_CONFIG_LOCK();
    is_modem_switching = (qcril_modem_switch_state !=
                                    QCRIL_MBN_SW_MODEM_SWITCH_STATE_NONE);
    is_modem_switched = (qcril_modem_switch_state ==
                                    QCRIL_MBN_SW_MODEM_SWITCH_STATE_COMPLETED);
    MBN_SW_CONFIG_UNLOCK();
    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                       (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);

    qcril_mbn_sw_state_type state = qcril_mbn_sw_get_cur_state();

    if (state != QCRIL_MBN_SW_STATE_LOADING_CONFIG &&
        state != QCRIL_MBN_SW_STATE_LOAD_INTERRUPTED)
    {
        QCRIL_LOG_DEBUG("Invalid state to proceed: %d", state);
        return E_SUCCESS;
    }
    else if(state == QCRIL_MBN_SW_STATE_LOAD_INTERRUPTED)
    {
        if(is_modem_switching && !is_modem_switched)
        {
            QCRIL_LOG_DEBUG("Load completed but flex mapping in progress...");
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_NONE);
        }
        else
        {
            QCRIL_LOG_DEBUG("Restart MBN loading from SUB%d",
                                            qcril_mbn_cur_instance_id);
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_NONE);
            qcril_mbn_sw_update_init(TRUE);
        }
    }
    else if ((error != RIL_E_SUCCESS) && !is_modem_switched)
    {
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_LOAD_FAILED);
        qcril_mbn_sw_take_end_action();
    }
    else
    {
        QCRIL_LOG_DEBUG("Select config for APPS SUB%d", qcril_mbn_cur_instance_id);
        qcril_mbn_need_restart = 1;
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_SELECTING_CONFIG);
        MBN_SW_CONFIG_LOCK();
        qcril_mbn_temp_sub_id = qcril_mbn_cur_sub_id;
        MBN_SW_CONFIG_UNLOCK();
        qcril_mbn_sw_select_config(qcril_mbn_temp_sub_id,
                                   PDC_CONFIG_TYPE_MODEM_SW_V01,
                                   (unsigned char *)sw_config_id[qcril_mbn_cur_instance_id],
                                   sw_config_id_len[qcril_mbn_cur_instance_id]);
    }

    return E_SUCCESS;
}
/*=========================================================================

  FUNCTION:  qcril_mbn_sw_load_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_SW_LOAD_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_load_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno result = RIL_E_GENERIC_FAILURE;
    qcril_mbn_sw_load_config_info *req = NULL;
    qcril_reqlist_public_type qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_sw_load_config_info *)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_SW_LOAD_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            QCRIL_LOG_DEBUG("Load config for SUB%d", req->sub_id);
            QCRIL_LOG_DEBUG("file_name %s", req->file_name);
            QCRIL_LOG_DEBUG("config_id %s", req->config_id);
            result = qcril_qmi_pdc_load_config_file(req->sub_id,
                                                    req->file_name,
                                                    (char *)req->config_id,
                                                    PDC_CONFIG_TYPE_MODEM_SW_V01);
        }
        else
        {
            QCRIL_LOG_ERROR("Failed to add MBN internal event in queue...");
        }

    } while (0);

    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_sw_send_load_config_resp(result);
    }

    if (req && req->file_name)
    {
        free(req->file_name);
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_mbn_sw_load_config

===========================================================================*/
/*!
    @brief
    Load config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_load_config
(
    char *file_name
)
{
    qcril_mbn_sw_load_config_info req;
    int qcril_mbn_temp_sub_id = 0;

    MBN_SW_CONFIG_LOCK();
    qcril_mbn_cur_sub_id = qcril_qmi_pdc_get_sub_id(qcril_mbn_cur_instance_id);
    qcril_mbn_temp_sub_id = qcril_mbn_cur_sub_id;
    MBN_SW_CONFIG_UNLOCK();
    qcril_mbn_sw_update_sub_id_cache(qcril_mbn_cur_instance_id, qcril_mbn_temp_sub_id);

    int len         = 0;
    int start_pos   = 0;

    req.sub_id          = qcril_mbn_temp_sub_id;
    req.file_name       = file_name;

    if (file_name)
    {
        QCRIL_LOG_DEBUG("SUB%d: File name %s",
                             qcril_mbn_cur_instance_id, file_name);

        if ((strlen(file_name) + 1) >= sizeof(req.config_id))
        {
            start_pos = (strlen(file_name) + 1) - sizeof(req.config_id);
        }

        len = snprintf((char*)req.config_id, PDC_CONFIG_ID_SIZE_MAX_V01, "%s%d",
                file_name + start_pos, qcril_mbn_cur_instance_id);

        req.config_id_len = len;
        strlcpy((char *)sw_config_id[qcril_mbn_cur_instance_id], (char *)req.config_id,
                 PDC_CONFIG_ID_SIZE_MAX_V01);
        sw_config_id_len[qcril_mbn_cur_instance_id] = len;
    }

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_SW_LOAD_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_select_config_resp

===========================================================================*/
/*!
    @brief
    Send select_config response

    @return
    None
*/
/*=========================================================================*/
void qcril_mbn_sw_send_select_config_resp
(
    RIL_Errno result
)
{
    qcril_mbn_sw_state_type state = qcril_mbn_sw_get_cur_state();
    if(state != QCRIL_MBN_SW_STATE_SELECTING_CONFIG)
    {
        QCRIL_LOG_DEBUG("Invalid state to proceed: %d", state);
        return;
    }
    // Only free it when state is expected. When MBN SW init starts,
    // it will free the MBN internal token
    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
               (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);

    QCRIL_LOG_DEBUG("Selection status for SUB%d: %d",
                          qcril_mbn_cur_instance_id, result);

    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_SELECTION_FAILED);
        qcril_mbn_sw_take_end_action();
    }
    else
    {
        // store last pending sub
        qcril_mbn_last_pending_sub = qcril_mbn_cur_instance_id;
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_SELECTION_SUCCESS);
        qcril_mbn_sw_take_end_action();
    }
}

/*=========================================================================

  FUNCTION:  qcril_mbn_sw_select_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_SW_SELECT_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_select_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno result = RIL_E_GENERIC_FAILURE;
    qcril_mbn_sw_select_config_info *req = NULL;
    qcril_reqlist_public_type        qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_sw_select_config_info *)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_SW_SELECT_CONFIG,
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
            QCRIL_LOG_ERROR("Failed to add MBN internal event in queue...");
        }

    } while (0);

    QCRIL_LOG_DEBUG("result: %d", result);
    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_sw_send_select_config_resp(result);
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_mbn_sw_select_config

===========================================================================*/
/*!
    @brief
    Select config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_select_config
(
    uint32_t  sub_id,
    pdc_config_type_enum_v01 config_type,
    uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01],
    uint32_t config_id_len
)
{
    qcril_mbn_sw_select_config_info req;

    memset(&req, 0, sizeof(req));
    req.sub_id          = sub_id;
    req.config_type     = config_type;
    memcpy(req.config_id, config_id, config_id_len);
    req.config_id_len   = config_id_len;

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_SW_SELECT_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}


/*===========================================================================

  FUNCTION qcril_mbn_sw_send_check_pending_response

===========================================================================*/
/*!
    @brief
    Sends the info that if there is any pending configuration

    @return
    None
*/
/*=========================================================================*/
void qcril_mbn_sw_send_check_pending_response
(
    uint8_t pending_valid
)
{
    qcril_mbn_sw_state_type state;

    QCRIL_LOG_FUNC_ENTRY();
    state = qcril_mbn_sw_get_cur_state();
    if (state != QCRIL_MBN_SW_STATE_COUNTING_PENDING_CONFIGS)
    {
        QCRIL_LOG_DEBUG("Invalid state to proceed: %d", state);
        return;
    }

    QCRIL_LOG_INFO("checking sub_id: %d, pending_valid: %d",
            qcril_check_pending_subid, pending_valid);
    if (pending_valid)
    {
        qcril_pending_configs_count++;
        qcril_mbn_last_pending_sub = qcril_check_pending_subid;
    }
    qcril_check_pending_subid++;
    if (qcril_check_pending_subid < qcril_mbn_max_apss_instance_id)
    {
        (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                                QCRIL_DEFAULT_MODEM_ID,
                                QCRIL_DATA_ON_STACK,
                                QCRIL_EVT_REQUEST_MBN_SW_QUERY_PENDING_CONFIG,
                                (void*)&qcril_check_pending_subid,
                                sizeof(qcril_check_pending_subid),
                                (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);
    }
    else
    {
        // at least, we should one pending configuration
        if (qcril_pending_configs_count == 0)
            qcril_pending_configs_count = 1;
        qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATING_CONFIG);
        if (qcril_mbn_last_pending_sub == -1)
        {
            QCRIL_LOG_ERROR("Failed to get pending sub, use SUB0 as default");
            qcril_mbn_sw_activate_selected_config(0, PDC_CONFIG_TYPE_MODEM_SW_V01);
        }
        else
        {
            qcril_mbn_sw_activate_selected_config(qcril_mbn_last_pending_sub,
                    PDC_CONFIG_TYPE_MODEM_SW_V01);
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_activate_config_resp

===========================================================================*/
/*!
    @brief
    Send activate_config response

    @return
    TRUE: if there is more expected activation indication
    FALSE: if all expected activation indications are reported
*/
/*=========================================================================*/
boolean qcril_mbn_sw_send_activate_config_resp
(
    RIL_Errno result,
    int       from_ind
)
{
    qcril_mbn_sw_state_type state = qcril_mbn_sw_get_cur_state();
    if(state != QCRIL_MBN_SW_STATE_ACTIVATING_CONFIG)
    {
        QCRIL_LOG_DEBUG("Invalid state to proceed: %d", state);
        return FALSE;
    }
    if ((result != RIL_E_SUCCESS) && !from_ind)
    {
        /* if we receive the error from activation response, set the pending count
         * as 0, since RIL will do clean up work soon */
        QCRIL_LOG_DEBUG("receive error reponse for activation, clear pending count");
        qcril_pending_configs_count = 0;
    }
    else
    {
        qcril_pending_configs_count--;
    }

    if (qcril_pending_configs_count > 0)
    {
        QCRIL_LOG_DEBUG("expecting more activation indications, current pending: %d",
                qcril_pending_configs_count);
        return TRUE;
    }

    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                       (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
    // no pending sub now, reset it as -1
    qcril_mbn_last_pending_sub = -1;

    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATION_FAILED);
        qcril_mbn_sw_take_end_action();
    }
    else
    {
        QCRIL_LOG_DEBUG("Activation completed");
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATION_COMPLETED);
        qcril_mbn_sw_take_end_action();
    }
    return FALSE;
}

/*=========================================================================

  FUNCTION:  qcril_mbn_sw_activate_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_SW_ACTIVATE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_activate_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno result = RIL_E_GENERIC_FAILURE;
    qcril_mbn_sw_config_req_type *req = NULL;
    qcril_reqlist_public_type qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        req = (qcril_mbn_sw_config_req_type *)params_ptr->data;

        if (!req)
        {
            break;
        }

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_SW_ACTIVATE_CONFIG,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            result = qcril_qmi_pdc_activate_configuration_hdlr(req->sub_id,
                                                            req->config_type);
        }
        else
        {
            QCRIL_LOG_ERROR("Failed to add MBN internal event in queue...");
        }

    } while (0);

    QCRIL_LOG_DEBUG("result: %d", result);
    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_sw_send_activate_config_resp(result, FALSE);
    }

    QCRIL_LOG_FUNC_RETURN();
}
/*===========================================================================

  FUNCTION  qcril_mbn_sw_activate_selected_config

===========================================================================*/
/*!
    @brief
    Activate selected_config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_activate_selected_config
(
    uint32_t  sub_id,
    pdc_config_type_enum_v01 config_type
)
{
    qcril_mbn_sw_select_config_info req;

    memset(&req, 0, sizeof(req));
    req.sub_id      = sub_id;
    req.config_type = config_type;

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_REQUEST_MBN_SW_ACTIVATE_CONFIG,
                            (void *) &req,
                            sizeof(req),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    return E_SUCCESS;
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_update_sim_info_cache

===========================================================================*/
/*!
    @brief
    update iccid mcc mnc cache

    @return
    None
*/
/*=========================================================================*/
void qcril_mbn_sw_update_sim_info_cache
(
    int instance,
    char* iccid,
    int iccid_len,
    char* mcc,
    int mcc_len,
    char* mnc,
    int mnc_len
)
{
    QCRIL_LOG_FUNC_ENTRY();
    MBN_SW_CONFIG_LOCK();
    if (iccid)
    {
        if (strcmp(qcril_mbn_iccid_cache[instance], iccid))
        {
            QCRIL_LOG_INFO("set iccid_%d to %s in cache", instance, iccid);
            strlcpy(qcril_mbn_iccid_cache[instance], iccid, iccid_len);
        }

    }
    else
    {
        QCRIL_LOG_INFO("clear iccid_%d in cache", instance);
        memset(qcril_mbn_iccid_cache[instance], 0, sizeof(qcril_mbn_iccid_cache[instance]));
    }

    if (mcc)
    {
        if (strcmp(qcril_mbn_mcc_cache[instance], mcc))
        {
            QCRIL_LOG_INFO("set mcc_%d to %s in cache", instance, mcc);
            strlcpy(qcril_mbn_mcc_cache[instance], mcc, mcc_len);
        }
    }
    else
    {
        QCRIL_LOG_INFO("clear mcc_%d in cache", instance);
        memset(qcril_mbn_mcc_cache[instance], 0, sizeof(qcril_mbn_mcc_cache[instance]));
    }

    if (mnc)
    {
        if (strcmp(qcril_mbn_mnc_cache[instance], mnc))
        {
            QCRIL_LOG_INFO("set mnc_%d to %s in cache", instance, mnc);
            strlcpy(qcril_mbn_mnc_cache[instance], mnc, sizeof(qcril_mbn_mnc_cache[instance]));
        }
    }
    else
    {
        QCRIL_LOG_INFO("clear mnc_%d in cache", instance);
        memset(qcril_mbn_mnc_cache[instance], 0, mnc_len);
    }
    MBN_SW_CONFIG_UNLOCK();

    if (iccid && mcc && mnc)
    {
        QCRIL_LOG_INFO("Updating settings");
        char my_iccid[QCRIL_UIM_ICCID_LEN];
        strlcpy(my_iccid, iccid, sizeof(my_iccid));
        settingsd_client_update_iccid_card(
                instance,
                (char(*)[QCRIL_MCC_MNC_MAX_SIZE])mcc,
                (char (*)[QCRIL_MCC_MNC_MAX_SIZE])mnc,&my_iccid );
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_update_sub_id_cache

===========================================================================*/
/*!
    @brief
    update sub_id cache

    @return
    None
*/
/*=========================================================================*/
void qcril_mbn_sw_update_sub_id_cache(int instance, int temp_sub_id)
{
    QCRIL_LOG_FUNC_ENTRY();
    MBN_SW_CONFIG_LOCK();
    QCRIL_LOG_INFO("set sub_id[%d] to %d", instance, temp_sub_id);
    qcril_mbn_sub_id_cache[instance] = temp_sub_id;
    MBN_SW_CONFIG_UNLOCK();
    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_is_sim_info_different_from_cache

===========================================================================*/
/*!
    @brief
    Check value is same as cache_value

    @return
    TRUE for value different
    FALSE for value same
*/
/*=========================================================================*/
boolean qcril_mbn_sw_is_sim_info_different_from_cache
(
   int instance,
   char* cache_value,
   char* value,
   const char* type
)
{
    boolean cache_is_different = FALSE;

    QCRIL_LOG_FUNC_ENTRY();
    MBN_SW_CONFIG_LOCK();
    QCRIL_LOG_DEBUG("temp_%s_%d: %s",
                    type,
                    instance,
                    value? value: "null");
    if (value && strlen(value))
    {
        // If value is valid (not "null" or "absent") and changed, need to cancel current mbn init
        if (strcmp(value, "absent") && strcmp(value, "null"))
        {
            if (strcmp(cache_value, value))
            {
                QCRIL_LOG_INFO("sw mbn init need to cancel due to %s_%d change\n"
                               "old %s: %s, new %s: %s",
                               type,
                               instance,
                               type,
                               cache_value,
                               type,
                               value);
                cache_is_different = TRUE;
            }
        }
    }
    MBN_SW_CONFIG_UNLOCK();
    QCRIL_LOG_FUNC_RETURN_WITH_RET(cache_is_different);
    return cache_is_different;
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_if_restart_needed

===========================================================================*/
/*!
    @brief
    Check if restart needed for sw_mbn_init

    @return
    TRUE for cancellation needed
    FALSE for otherwise
*/
/*=========================================================================*/
boolean qcril_mbn_sw_if_restart_needed(void)
{
    boolean need_restart = FALSE;
    int instance;
    char qcril_mbn_temp_iccid[PDC_CONFIG_ID_SIZE_MAX_V01] = {0};
    char qcril_mbn_temp_mcc[5] = {0};
    char qcril_mbn_temp_mnc[5] = {0};
    int  qcril_mbn_temp_sub_id = 0;

    QCRIL_LOG_FUNC_ENTRY();
    for (instance = 0; instance < qcril_mbn_max_apss_instance_id; instance++)
    {
       qcril_qmi_pdc_get_sim_info( instance,
                 qcril_mbn_temp_iccid, PROP_VALUE_MAX,
                 qcril_mbn_temp_mcc, sizeof(qcril_mbn_temp_mcc),
                 qcril_mbn_temp_mnc, sizeof(qcril_mbn_temp_mnc));

        qcril_mbn_temp_sub_id = qcril_qmi_pdc_get_sub_id(instance);

        // Check sim info cache
        if (qcril_mbn_sw_is_sim_info_different_from_cache(instance,
                                         qcril_mbn_iccid_cache[instance],
                                         qcril_mbn_temp_iccid,
                                         "iccid")
            || qcril_mbn_sw_is_sim_info_different_from_cache(instance,
                                                qcril_mbn_mcc_cache[instance],
                                                qcril_mbn_temp_mcc,
                                                "mcc")
            || qcril_mbn_sw_is_sim_info_different_from_cache(instance,
                                                qcril_mbn_mnc_cache[instance],
                                                qcril_mbn_temp_mnc,
                                                "mnc"))
        {
            need_restart = TRUE;
        }

        // Check sub_id cache
        MBN_SW_CONFIG_LOCK();
        if (qcril_mbn_temp_sub_id != qcril_mbn_sub_id_cache[instance])
        {
            QCRIL_LOG_INFO("sub_id[%d] is different. old value: %d, new value: %d",
                           instance,
                           qcril_mbn_sub_id_cache[instance],
                           qcril_mbn_temp_sub_id);
            need_restart = TRUE;
        }
        MBN_SW_CONFIG_UNLOCK();

        if (need_restart)
        {
            qcril_mbn_sw_update_sim_info_cache(instance,
                                      qcril_mbn_temp_iccid,
                                      PROP_VALUE_MAX,
                                      qcril_mbn_temp_mcc,
                                      sizeof(qcril_mbn_temp_mcc),
                                      qcril_mbn_temp_mnc,
                                      sizeof(qcril_mbn_temp_mnc));
            qcril_mbn_sw_update_sub_id_cache(instance, qcril_mbn_temp_sub_id);
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(need_restart);
    return need_restart;
} // qcril_mbn_sw_if_restart_needed

IxErrnoType qcril_mbn_sw_update_init
(
    boolean is_ssr_or_bootup
)
{
     return qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                     QCRIL_DEFAULT_MODEM_ID, QCRIL_DATA_ON_STACK,
                     QCRIL_EVT_REQUEST_VERIFY_MBN_SW_INIT,
                     (void *) &is_ssr_or_bootup, sizeof(is_ssr_or_bootup),
                     (RIL_Token)(intptr_t)QCRIL_TOKEN_ID_INTERNAL);
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_update_init_helper

===========================================================================*/
/*!
    @brief
    Verify and Initilize software mbn update

    @return
    None
*/
/*=========================================================================*/
void qcril_mbn_sw_update_init_helper
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
)
{
    int     start_init = 0;
    int     is_enabled = 1;
    boolean need_restart = 0;
    boolean *is_ssr_or_bootup = NULL;
    int     qcril_no_sim_info = 1;
    int     instance = 0;
    char    iccid[PROP_VALUE_MAX] = {0};
    char    mcc[5] = {0};
    char    mnc[5] = {0};

    char    qcril_mbn_temp_iccid[PROP_VALUE_MAX] = {0};
    QCRIL_NOTUSED(ret_ptr);

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();
    if(params_ptr == NULL)
    {
        QCRIL_LOG_ERROR("Inavlid input args...");
        return;
    }

    is_ssr_or_bootup = (boolean *)params_ptr->data;
    QCRIL_LOG_ERROR("is_ssr_or_bootup %d", *is_ssr_or_bootup);

    MBN_SW_CONFIG_LOCK();
    if(qcril_modem_switch_state == QCRIL_MBN_SW_MODEM_SWITCH_STATE_START ||
       qcril_modem_switch_state == QCRIL_MBN_SW_MODEM_SWITCH_STATE_APPLY)
    {
        QCRIL_LOG_DEBUG("Do not load MBNs now as flex mapping is still in progress...");
        MBN_SW_CONFIG_UNLOCK();
        return;
    }
    qcril_qmi_pdc_get_sim_info( qcril_mbn_cur_instance_id,
                 qcril_mbn_cur_instance_iccid, PROP_VALUE_MAX,
                 qcril_mbn_cur_instance_mcc, sizeof(qcril_mbn_cur_instance_mcc),
                 qcril_mbn_cur_instance_mnc, sizeof(qcril_mbn_cur_instance_mnc));

    strlcpy(qcril_mbn_temp_iccid, qcril_mbn_cur_instance_iccid, sizeof(qcril_mbn_temp_iccid));
    MBN_SW_CONFIG_UNLOCK();

    qmi_ril_get_property_value_from_integer(QCRIL_MBN_SW_FEATURE_ENABLED,
                                            &is_enabled, 0);

    // If SSR or bootup, we don't need to calculate need_restart, since sw_mbn_init is always needed
    if (*is_ssr_or_bootup && qcril_mbn_sw_get_cur_state() != QCRIL_MBN_SW_STATE_ACTIVATION_COMPLETED)
    {
        if (qcril_mbn_sw_get_cur_state() == QCRIL_MBN_SW_STATE_BOOTUP_NEEDED)
        {
            QCRIL_LOG_INFO("sw mbn init need to start due to first time bootup");
        }
        else if (qcril_mbn_sw_get_cur_state() == QCRIL_MBN_SW_STATE_LOAD_INTERRUPTED)
        {
            QCRIL_LOG_INFO("sw mbn init need to start due to load interruption");
        }
        else
        {
            QCRIL_LOG_INFO("sw mbn init need to restart due to SSR");
        }

        // Called only to update mcc/mnc/iccid/sub_id cache, not for calculating need_restart
        qcril_mbn_sw_if_restart_needed();

        qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                           (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
        //Reset any MBN flags or states as MBN init going to restart
        qcril_qmi_pdc_reset_mbn_state();
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_NONE);
        qcril_mbn_cur_instance_id = 0;
    }
    // For the other case (mcc/mnc/iccid/sub_id change), need to calculate need_restart
    else
    {
        need_restart = qcril_mbn_sw_if_restart_needed();
    }

    qcril_no_sim_info = 1;
    for (instance = 0;
         instance < qcril_mbn_max_apss_instance_id; instance++)
    {
        qcril_qmi_pdc_get_sim_info( instance,
                          iccid, PROP_VALUE_MAX,
                          mcc, sizeof(mcc),
                          mnc, sizeof(mnc));

        //Set Sub as ready , when either ICCID or MCC&MNC are present
        if((strlen(iccid) && strncmp(iccid, "null", PROP_VALUE_MAX-1)) ||
           ((strlen(mcc) && strncmp(mcc, "null", QCRIL_MCC_MNC_MAX_SIZE)) &&
            (strlen(mnc) && strncmp(mnc, "null", QCRIL_MCC_MNC_MAX_SIZE))))
        {
            qcril_no_sim_info = 0;
            break;
        }
    }

    if (!is_enabled)
    {
        QCRIL_LOG_INFO("Feature disabled");
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATION_NOT_NEEDED_FOR_DEV);
    }
    else if (qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID)
    {
        QCRIL_LOG_INFO("SW update will be done by primary rild");
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATION_NOT_NEEDED_FOR_DEV);
    }
    else if (qcril_no_sim_info)
    {
        QCRIL_LOG_DEBUG("No sim info for all SUBs");
    }
    // Change in mcc/mnc/iccid/sub_id should trigger restart of sw_mbn_init
    else if (need_restart)
    {
        QCRIL_LOG_INFO("restart needed due to mcc/mnc/iccid/sub_id change");

        // If current state is loading, wait until loading finishes
        if (qcril_mbn_sw_get_cur_state() == QCRIL_MBN_SW_STATE_LOADING_CONFIG)
        {
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_LOAD_INTERRUPTED);
        }
        else if (qcril_mbn_sw_get_cur_state() == QCRIL_MBN_SW_STATE_LOAD_INTERRUPTED)
        {
            QCRIL_LOG_INFO("Wait for the loading to be completed...");
        }
        // Otherwise set state to None and start init
        else {
            //Reset any MBN flags or states as MBN init going to restart
            qcril_qmi_pdc_reset_mbn_state();
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_NONE);
            start_init = 1;
            qcril_mbn_cur_instance_id = 0;
        }
    }
    // First time bootup or SSR or load interruption, will always start init
    else
    {
        start_init = 1;
    }

    if (start_init)
    {
        QCRIL_LOG_INFO("start_init for apps SUB%d", qcril_mbn_cur_instance_id);
        // if it's the first time for sw_mbn_update, need to disable auto selection in modem
        if (!qcril_mbn_feature_modem_disabled)
        {
            QCRIL_LOG_INFO("Disabling modem based mbn auto selection feature");
            qcril_mbn_feature_modem_disabled = true;
            (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_DATA_ON_STACK,
                                    QCRIL_EVT_REQUEST_MBN_SW_DISALBLE_MODEM_UPDATE,
                                    NULL,
                                    0,
                                   (RIL_Token)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
        }
        // otherwise, continue with mbn_sw_update_init
        else
        {
            QCRIL_LOG_INFO("No need to disable modem. Feature disabled already");
            (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_DATA_ON_STACK,
                                    QCRIL_EVT_REQUEST_MBN_SW_INIT,
                                    NULL,
                                    0,
                                   (RIL_Token)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return;
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_get_sw_name_to_look_for

===========================================================================*/
/*!
    @brief
    Get sw_name to look for

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
char *qcril_mbn_sw_get_sw_name_to_look_for
(
    int max_subscriptions
)
{
    char *sw_name =  NULL;
    if (max_subscriptions == 3)
    {
        sw_name = "LA_TS";
    }
    else if (max_subscriptions == 2)
    {
        sw_name = "LA_DS";
    }
    else
    {
        sw_name = "LA_SS";
    }

    return sw_name;
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_disable_modem_update_hndlr

===========================================================================*/
/*!
    @brief
    disable modem mbn auto selection feature

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_sw_disable_modem_update_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    RIL_Errno                     result          = RIL_E_GENERIC_FAILURE;
    qcril_reqlist_public_type     qcril_req_info;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                           (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);

        qcril_reqlist_default_entry(params_ptr->t,
                                    params_ptr->event_id,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                    QCRIL_EVT_REQUEST_MBN_SW_DISALBLE_MODEM_UPDATE,
                                    NULL,
                                    &qcril_req_info);

        if (!qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info))
        {
            result = qcril_qmi_pdc_disable_modem_update();
        }
        else
        {
            QCRIL_LOG_ERROR("Failed to add MBN internal event in queue...");
        }

    } while (0);

    if (result != RIL_E_SUCCESS)
    {
        qcril_mbn_sw_send_disable_modem_update_resp(result, false);
    }

    QCRIL_LOG_DEBUG("result %d", result);
    QCRIL_LOG_FUNC_RETURN();
}


/*===========================================================================

  FUNCTION qcril_mbn_sw_send_disable_modem_update_resp

===========================================================================*/
/*!
    @brief
    send response of disable modem update result, and trigger sw mbn update
    if the response is successful

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_disable_modem_update_resp
(
    RIL_Errno result,
    bool is_from_ind
)
{
    QCRIL_LOG_FUNC_ENTRY();

    qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                       (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);

    // if error received from indication, do not update mbn
    if (result != RIL_E_SUCCESS && is_from_ind)
    {
        QCRIL_LOG_ERROR("Disable modem update failed because of indication error");
        qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATION_NOT_NEEDED);
    }
    // else, proceed with mbn_sw_update
    else
    {
        if (result != RIL_E_SUCCESS && !is_from_ind)
        {
            QCRIL_LOG_ERROR("Modem feature not supported. Continue with sw mbn update");
        }
        else
        {
            QCRIL_LOG_INFO("Disable modem update successful");
        }
        QCRIL_LOG_INFO("Disable modem update successful");
        (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                                QCRIL_DEFAULT_MODEM_ID,
                                QCRIL_DATA_ON_STACK,
                                QCRIL_EVT_REQUEST_MBN_SW_INIT,
                                NULL,
                                0,
                               (RIL_Token)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
    }
    QCRIL_LOG_FUNC_RETURN();
    return E_SUCCESS;
}

/*===========================================================================

  FUNCTION qcril_mbn_sw_update_init_hdlr

===========================================================================*/
/*!
    @brief
    Software mbn update initialization handler

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_sw_update_init_hdlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
)
{
    int                    start_init   = 0;
    int                    is_modem_switched = 0;
    char                   qcril_mbn_temp_iccid[PROP_VALUE_MAX] = {0};
    char                   qcril_mbn_temp_mcc[5] = {0};
    char                   qcril_mbn_temp_mnc[5] = {0};
    boolean is_invalid_iccid = FALSE;
    boolean is_invalid_mcc_mnc = FALSE;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    do {

        if ((params_ptr == NULL) || (ret_ptr == NULL))
        {
            QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
            break;
        }

        if (qcril_mbn_sw_get_cur_state() == QCRIL_MBN_SW_STATE_NONE)
        {
            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_INITIALIZING);
            start_init = 1;
            QCRIL_LOG_DEBUG("Initializing SUB%d", qcril_mbn_cur_instance_id);
        }
        else if (qcril_mbn_sw_get_cur_state() == QCRIL_MBN_SW_STATE_ACTIVATING_CONFIG)
        {
            if (qcril_mbn_cur_instance_id >= (qcril_mbn_max_apss_instance_id - 1))
            {
                qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATION_COMPLETED);
                qcril_mbn_cur_instance_id = 0;
                QCRIL_LOG_DEBUG("Activation completed");
            }
            else
            {
                QCRIL_LOG_DEBUG("Invalid instance id %d", qcril_mbn_cur_instance_id);
            }
        }
        else if (qcril_mbn_sw_get_cur_state() == QCRIL_MBN_SW_STATE_ACTIVATION_COMPLETED)
        {
            MBN_SW_CONFIG_LOCK();
            is_modem_switched = (qcril_modem_switch_state ==
                                        QCRIL_MBN_SW_MODEM_SWITCH_STATE_COMPLETED);
            MBN_SW_CONFIG_UNLOCK();

            qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_INITIALIZING);
            start_init = 1;
            QCRIL_LOG_DEBUG("Initializing SUB%d", qcril_mbn_cur_instance_id);
        }
        else
        {
            if (qcril_mbn_sw_is_completed_for_a_sub())
            {
                if (qcril_mbn_cur_instance_id < (qcril_mbn_max_apss_instance_id - 1))
                {
                    qcril_mbn_cur_instance_id++;
                    qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_INITIALIZING);
                    start_init = 1;
                }

                QCRIL_LOG_DEBUG("Activating next sub SUB%d", qcril_mbn_cur_instance_id);
            }
            else
            {
                QCRIL_LOG_DEBUG("MBN load ongoing");
            }
        }

        if (start_init)
        {
            qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                               (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);
            MBN_SW_CONFIG_LOCK();
            qcril_qmi_pdc_get_sim_info( qcril_mbn_cur_instance_id,
                qcril_mbn_cur_instance_iccid, PROP_VALUE_MAX,
                qcril_mbn_cur_instance_mcc, sizeof(qcril_mbn_cur_instance_mcc),
                qcril_mbn_cur_instance_mnc, sizeof(qcril_mbn_cur_instance_mnc));

            strlcpy(qcril_mbn_temp_iccid, qcril_mbn_cur_instance_iccid, sizeof(qcril_mbn_temp_iccid));
            strlcpy(qcril_mbn_temp_mcc, qcril_mbn_cur_instance_mcc, sizeof(qcril_mbn_temp_mcc));
            strlcpy(qcril_mbn_temp_mnc, qcril_mbn_cur_instance_mnc, sizeof(qcril_mbn_temp_mnc));
            MBN_SW_CONFIG_UNLOCK();

            qcril_mbn_sw_update_sim_info_cache(qcril_mbn_cur_instance_id,
                                      qcril_mbn_temp_iccid,
                                      PROP_VALUE_MAX,
                                      qcril_mbn_temp_mcc,
                                      sizeof(qcril_mbn_temp_mcc),
                                      qcril_mbn_temp_mnc,
                                      sizeof(qcril_mbn_temp_mnc));


            if (qcril_mbn_cur_instance_id == QCRIL_DEFAULT_INSTANCE_ID)
            {
                /* load custom mbn files to db */
                if (qcril_mbn_sw_load_to_db() != E_SUCCESS)
                {
                    QCRIL_LOG_ERROR("MBN file info update in DB failed...");
                    qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATION_NOT_NEEDED);
                    qcril_mbn_cur_instance_id = (qcril_mbn_max_apss_instance_id - 1);
                    qcril_mbn_sw_take_end_action();
                    break;
                }
            }

            //check for invalid iccid
             if (!strlen(qcril_mbn_temp_iccid)
                || !strncmp(qcril_mbn_temp_iccid, "null", PROP_VALUE_MAX-1)
                || !strncmp(qcril_mbn_temp_iccid, "absent", PROP_VALUE_MAX-1))
            {
                is_invalid_iccid = TRUE;
            }

            //check for invalid mcc
            if (!strlen(qcril_mbn_temp_mcc)
                || !strncmp(qcril_mbn_temp_mcc, "null", QCRIL_MCC_MNC_MAX_SIZE))
            {
                is_invalid_mcc_mnc = TRUE;
            }

            //check for invalid mnc
            if (!strlen(qcril_mbn_temp_mnc)
                || !strncmp(qcril_mbn_temp_mnc, "null", QCRIL_MCC_MNC_MAX_SIZE))
            {
                is_invalid_mcc_mnc = TRUE;
            }

            QCRIL_LOG_DEBUG("SUB(%d) status is_invalid_iccid=%d is_invalid_mcc_mnc=%d ",
                            qcril_mbn_cur_instance_id, is_invalid_iccid, is_invalid_mcc_mnc);

            //Sub is not ready when both ICCID &MCC/MNC are invalid
            //proceed with update if either ICCID or MCC/MNC are valid
            if (is_invalid_iccid && is_invalid_mcc_mnc)
            {
                QCRIL_LOG_DEBUG("SUB%d is not yet ready...", qcril_mbn_cur_instance_id);
                qcril_mbn_sw_set_cur_state(QCRIL_MBN_SW_STATE_ACTIVATION_NOT_NEEDED);
                qcril_mbn_sw_take_end_action();
            }
            else
            {
                /* get current active sw config */
                qcril_mbn_sw_retrieve_selected_config();
            }
        }

    } while(0);

    QCRIL_LOG_FUNC_RETURN();
    return;
}

#ifdef QMI_RIL_UTF
#define SW_CFG_PATH "data/modem_config/mcfg_sw/"
#else
#define SW_CFG_PATH DATA_PARTITION_ROOT "radio/modem_config/mcfg_sw/"
#endif

/*===========================================================================

  FUNCTION qcril_mbn_sw_load_to_db

===========================================================================*/
/*!
    @brief
    Load sw mbn to db

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_load_to_db
(
    void
)
{
#define QCRIL_MBN_SW_MAX_FILES (100)
    char *sw_cfg_path     = SW_CFG_PATH;
    char *file_names[QCRIL_MBN_SW_MAX_FILES] = {NULL};
    int   number_of_files = 0;
    int   index           = 0;
    int   int_value       = 0;
    char  *str_value      = NULL;
    uint32_t capability   = 0;
    char  *carrier_name   = NULL;
    char  volte_info[8]   = {0};
    char  mkt_info[8]     = {0};
    char  lab_info[8]     = {0};
    int   iin_flag        = 0;
    int   iin_list_count  = 0;
    int  *iin_list        = NULL;
    int   long_iin_list_len = 0;
    char *long_iin_list   = NULL;
    int   mcc_mnc_flag    = 0;
    size_t mcc_mnc_count   = 0;    // count of mcc/mnc pair in mcc_mnc_list
    size_t mcc_mnc_list_len = 0;   // total length of mcc_mnc_list in bytes (for sanity check)
    uint16_t *mcc_mnc_list = NULL;// list[0]: mcc[0], list[1]: mnc[0], list[2]: mcc[1], ...

    unsigned char *meta_data   = NULL;
    unsigned int meta_data_len = 0;
    IxErrnoType ret = E_FAILURE;
    IxErrnoType res = E_FAILURE;
    IxErrnoType local_ret = E_FAILURE; // local return value for optional TLVs
    static   int mbn_loaded = 0;

    QCRIL_LOG_FUNC_ENTRY();

    if (access (sw_cfg_path, F_OK))
    {
        QCRIL_LOG_ESSENTIAL("QCRIL_ERROR:IO: No sw mbn config directory");
    }
    else if (mbn_loaded || qcril_qmi_sw_mbn_loaded_to_db())
    {
      QCRIL_LOG_INFO("SW mbn already loaded to DB");
      res = E_SUCCESS;
    }
    else
    {
      qcril_dir_retrieve_all_files_recursively(sw_cfg_path,
                                               file_names,
                                               &number_of_files,
                                               QCRIL_MBN_SW_MAX_FILES);

      QCRIL_LOG_INFO("Total number of files retrieved: %d", number_of_files);

      qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);

      qcril_db_begin();
      while (index < number_of_files)
      {
          QCRIL_LOG_INFO("File%d: %s", index+1, file_names[index]);
          ret = qcril_mbn_db_add_sw_mbn_file(file_names[index]);
          if (ret != E_SUCCESS)
          {
              QCRIL_LOG_DEBUG("db add sw mbn file failed");
          }

          ret = qcril_mbn_meta_retrieve_mbn_meta_data(file_names[index],
                                                &meta_data,
                                                &meta_data_len);
          if (ret != E_SUCCESS)
          {
              QCRIL_LOG_DEBUG("retrieve mbn meta from file failed");
              index++;
              continue;
          }

          ret = qcril_mbn_meta_retrieve_sw_qc_version(meta_data,
                                                meta_data_len,
                                                &int_value);
          if (ret != E_SUCCESS)
          {
              QCRIL_LOG_DEBUG("retrieve sw qc_version failed");
          }

          if (ret == E_SUCCESS)
          {
              ret = qcril_mbn_db_add_sw_mbn_qc_version(file_names[index],
                                                 int_value);
              if (ret != E_SUCCESS)
              {
                  QCRIL_LOG_DEBUG("db add sw qc_version failed");
                  break;
              }
          }

          ret = qcril_mbn_meta_retrieve_sw_oem_version(meta_data,
                                                meta_data_len,
                                                &int_value);
          if (ret != E_SUCCESS)
          {
              QCRIL_LOG_DEBUG("retrieve sw oem_version failed");
          }

          if (ret == E_SUCCESS)
          {
              ret = qcril_mbn_db_add_sw_mbn_oem_version(file_names[index],
                                                 int_value);
              if (ret != E_SUCCESS)
              {
                  QCRIL_LOG_DEBUG("db add sw mbn oem_version fails");
                  break;
              }
          }

          ret = qcril_mbn_meta_retrieve_sw_capability(meta_data,
                                                      meta_data_len,
                                                      &capability);

          if (ret == E_SUCCESS)
          {
              strlcpy(volte_info, (capability&QCRIL_MBN_SW_VOLTE_INFO_MASK)? "VOLTE": "NOV",
                      sizeof(volte_info));
              strlcpy(mkt_info, (capability&QCRIL_MBN_SW_MKT_INFO_MASK)? "SUBSIDI": "OPENMKT",
                      sizeof(mkt_info));
              strlcpy(lab_info, (capability&QCRIL_MBN_SW_LAB_INFO_MASK)? "LAB": "COMMERC",
                      sizeof(lab_info));
          }
          else
          {
              QCRIL_LOG_DEBUG("retrieve sw_capability failed");

              // fall back to old selection mechanism based on TLV 0x03
              ret = qcril_mbn_meta_retrieve_sw_carrier_name(meta_data,
                                                            meta_data_len,
                                                            &carrier_name);
              if (ret != E_SUCCESS)
              {
                  QCRIL_LOG_DEBUG("retrieve carrier_name failed");
              }

              if (ret == E_SUCCESS)
              {
                  // VoLTE/Non_VoLTE
                  if (strstr(carrier_name, "NoV") || strstr(carrier_name, "Non_VoLTE"))
                  {
                      strlcpy(volte_info, "NOV", sizeof(volte_info));
                  }
                  else if (strstr(carrier_name, "VoLTE") || strstr(carrier_name, "Volte"))
                  {
                      strlcpy(volte_info, "VOLTE", sizeof(volte_info));
                  }
                  // else volte_info will be ""

                  // OpenMkt/Subsidized
                  if (strstr(carrier_name, "OpenMkt"))
                  {
                      strlcpy(mkt_info, "OPENMKT", sizeof(mkt_info));
                  }
                  else if (strstr(carrier_name, "Subsidized"))
                  {
                      strlcpy(mkt_info, "SUBSIDI", sizeof(mkt_info));
                  }
                  // else mkt_info will be ""
              }
          }

          // Setting default values if we couldn't retrieve mkt_info and lab_info from above
          // mkt_info is "OPENMKT" by default since we never select subsidized MBN
          // lab_info is "Commerical" by default since we never select lab MBN
          if (strlen(mkt_info) == 0) strlcpy(mkt_info, "OPENMKT", sizeof(mkt_info));
          if (strlen(lab_info) == 0) strlcpy(lab_info, "COMMERC", sizeof(lab_info));

          QCRIL_LOG_DEBUG("volte_info: %s, mkt_info: %s, lab_info: %s",
                           volte_info, mkt_info, lab_info);

          ret = qcril_mbn_meta_retrieve_sw_iin_list(meta_data,
                                              meta_data_len,
                                              &iin_flag,
                                              &iin_list_count,
                                              &iin_list);
          if (ret != E_SUCCESS)
          {
              QCRIL_LOG_ERROR("retrieve sw iin failed");
          }

          if (ret == E_SUCCESS)
          {
              qcril_mbn_db_add_sw_mbn_iin(file_names[index],
                                          iin_flag, iin_list, iin_list_count,
                                          volte_info, mkt_info, lab_info);
              QCRIL_LOG_DEBUG("No need to check error cause as db add sw mbn iin can fail");
          }

          local_ret = qcril_mbn_meta_retrieve_sw_long_iin_list(meta_data,
                                              meta_data_len,
                                              &long_iin_list_len,
                                              &long_iin_list);
          if (local_ret != E_SUCCESS)
          {
              QCRIL_LOG_ERROR("long iin doesn't exist");
          }
          else
          {
              long_iin_list[long_iin_list_len - 1] = 0;
              qcril_mbn_db_add_sw_mbn_long_iin(file_names[index],
                                          long_iin_list, long_iin_list_len,
                                          volte_info, mkt_info, lab_info);
              QCRIL_LOG_DEBUG("No need to check error cause as db add sw mbn long iin can fail");
          }

          local_ret = qcril_mbn_meta_retrieve_sw_mcc_mnc_list(meta_data,
                                              meta_data_len,
                                              &mcc_mnc_flag,
                                              &mcc_mnc_count,
                                              &mcc_mnc_list_len,
                                              &mcc_mnc_list);
          if (local_ret != E_SUCCESS)
          {
              QCRIL_LOG_ERROR("mcc_mnc_list doesn't exist");
          }
          else
          {
              qcril_mbn_db_add_sw_mbn_mcc_mnc_list(file_names[index],
                                                         mcc_mnc_flag,
                                                         mcc_mnc_count,
                                                         mcc_mnc_list_len,
                                                         mcc_mnc_list,
                                                         volte_info,
                                                         mkt_info,
                                                         lab_info);
              QCRIL_LOG_DEBUG("No need to check error cause as db add mcc_mnc_list can fail");
          }

          if (str_value)
          {
              free(str_value);
              str_value = NULL;
          }

          if (carrier_name)
          {
              free(carrier_name);
              carrier_name = NULL;
          }

          if (meta_data)
          {
              free(meta_data);
              meta_data = NULL;
          }

          if (iin_list)
          {
              free(iin_list);
              iin_list = NULL;
          }

          if (long_iin_list)
          {
              free(long_iin_list);
              long_iin_list = NULL;
          }

          if (mcc_mnc_list)
          {
              free(mcc_mnc_list);
              mcc_mnc_list = NULL;
          }

          // If any mbn loaded to db successfully, set return value of this function to E_SUCCESS
          if (ret == E_SUCCESS && res == E_FAILURE)
          {
              res = E_SUCCESS;
          }

          index++;
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

      if (iin_list)
      {
          free(iin_list);
          iin_list = NULL;
      }
    }

    if (res == E_SUCCESS)
    {
        mbn_loaded = 1;
        qmi_ril_set_property_value_to_integer(QCRIL_MBN_SW_MBN_LOADED,
                                              mbn_loaded);
        qcril_db_update_cur_modem_version();
        qcril_db_commit();
        qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);
    }
    else
    {
        qcril_db_abort();
        qcril_db_recovery(QCRIL_DB_RECOVERY_RESTORE);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION qcril_qmi_sw_mbn_select_mbn

===========================================================================*/
/*!
    @brief
    Select MBN to be loaded

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_sw_mbn_select_mbn
(
    char   **mbn_file_name
)
{
    char    qcril_mbn_temp_iccid[PROP_VALUE_MAX] = {0};
    char    qcril_mbn_temp_mcc[5] = {0};
    char    qcril_mbn_temp_mnc[5] = {0};
    int     mcc = 0;
    int     mnc = 0;
    int     is_volte;
    int     is_openmkt;
    int     is_long_iin_mbn_needed;
    char    volte_info[8] = {0};
    char    mkt_info[8] = {0};
    char*   lab_info = "COMMERC";
    int     carrier_pref = 1;

    // get property value for volte_info and openmkt_info
    qmi_ril_get_property_value_from_integer(QCRIL_MBN_SW_VOLTE, &is_volte, 1);
    qmi_ril_get_property_value_from_integer(QCRIL_MBN_SW_OPENMKT, &is_openmkt, 1);
    qmi_ril_get_property_value_from_integer(QCRIL_MBN_SW_LONG_IIN, &is_long_iin_mbn_needed, 0);

    if (mbn_file_name)
    {
        MBN_SW_CONFIG_LOCK();
        strlcpy(qcril_mbn_temp_iccid, qcril_mbn_cur_instance_iccid, sizeof(qcril_mbn_temp_iccid));
        MBN_SW_CONFIG_UNLOCK();

        if (is_volte == 0)
        {
            strlcpy(volte_info, "NOV", sizeof(volte_info));
        }
        else // VOLTE by default
        {
            strlcpy(volte_info, "VOLTE", sizeof(volte_info));
        }

        if (is_openmkt == 0)
        {
            strlcpy(mkt_info, "SUBSIDI", sizeof(mkt_info));
        }
        else // OPENMKT by default
        {
            strlcpy(mkt_info, "OPENMKT", sizeof(mkt_info));
        }

        QCRIL_LOG_DEBUG("volte_info: %s, mkt_info: %s, lab_info: %s", volte_info, mkt_info, lab_info);

        if(is_long_iin_mbn_needed)
        {
            QCRIL_LOG_INFO("Long IIN MBN selection enabled");
            /* retrieve mbn file for 11-bit iin*/
            qcril_mbn_db_retrieve_sw_mbn_file_for_long_iccid(mbn_file_name,
                                                        qcril_mbn_temp_iccid,
                                                        volte_info,
                                                        mkt_info,
                                                        lab_info);
        }
        else
        {
            QCRIL_LOG_INFO("Long IIN MBN selection disabled");
        }

        if (!*mbn_file_name || !strlen(*mbn_file_name))
        {
            /* retrieve mbn file for mcc/mnc*/
            MBN_SW_CONFIG_LOCK();
            // remove leading zeros for mcc/mnc
            mcc = atoi(qcril_mbn_cur_instance_mcc);
            mnc = atoi(qcril_mbn_cur_instance_mnc);
            snprintf(qcril_mbn_temp_mcc, sizeof(qcril_mbn_temp_mcc), "%d", mcc);
            snprintf(qcril_mbn_temp_mnc, sizeof(qcril_mbn_temp_mnc), "%d", mnc);
            MBN_SW_CONFIG_UNLOCK();

            qcril_db_query_carrier_pref_from_mbn_imsi_exception_table(
                                                        qcril_mbn_temp_mcc,
                                                        qcril_mbn_temp_mnc,
                                                        &carrier_pref);

            if(carrier_pref)
            {
                qcril_mbn_db_query_sw_mbn_file_with_mcc_mnc(mbn_file_name,
                                                        qcril_mbn_temp_mcc,
                                                        qcril_mbn_temp_mnc,
                                                        volte_info,
                                                        mkt_info,
                                                        lab_info);
            }
        }

        if ((!*mbn_file_name || !strlen(*mbn_file_name)))
        {
            //Reset carrier pref to 1 and check the carrie_pref from DB
            carrier_pref = 1;
            qcril_db_query_carrier_pref_from_mbn_iccid_exception_table(
                                      qcril_mbn_temp_iccid, &carrier_pref);
            if(carrier_pref)
            {
                /* retrieve mbn file for iin*/
                qcril_mbn_db_retrieve_sw_mbn_file_for_iccid(mbn_file_name,
                       qcril_mbn_temp_iccid, volte_info, mkt_info, lab_info);
            }
            else
            {
                QCRIL_LOG_ERROR("No MBN file selected");
            }
        }
    }

} // qcril_qmi_sw_mbn_select_mbn

/*===========================================================================

  FUNCTION:  qcril_qmi_mbn_sw_init
===========================================================================*/
/*!
    @brief
    Initialize sw update subsystem.

    @return
    None.
*/
 /*=========================================================================*/

void qcril_qmi_mbn_sw_init
(
  void
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_modem_switch_state = QCRIL_MBN_SW_MODEM_SWITCH_STATE_NONE;
  pthread_mutexattr_init(&qcril_mbn_sw_config_mutex_attr);
  pthread_mutex_init(&qcril_mbn_sw_config_mutex, &qcril_mbn_sw_config_mutex_attr);

  QCRIL_LOG_FUNC_RETURN();

  return;
}

/*===========================================================================

  FUNCTION qcril_qmi_sw_mbn_modem_switch

===========================================================================*/
/*!
    @brief
    Inform sw mbn update subsystem about modem switch

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_sw_mbn_modem_switch(qcril_mbn_sw_modem_switch_state state)
{
    int instance = 0;
    int restart_needed = 0;
    char   iccid[PROP_VALUE_MAX] = {0};
    char   mcc[5] = {0};
    char   mnc[5] = {0};

    MBN_SW_CONFIG_LOCK();

    switch (state)
    {
        case QCRIL_MBN_SW_MODEM_SWITCH_STATE_START:
        case QCRIL_MBN_SW_MODEM_SWITCH_STATE_APPLY:
        {
            qcril_modem_switch_state = state;

            MBN_SW_CONFIG_UNLOCK();
            //Reset MBN SIM info cache, so that MBN update
            //will be forced once flex mapping is finished.
            for (instance = 0;
                 instance < qcril_mbn_max_apss_instance_id; instance++)
            {
                qcril_mbn_sw_update_sim_info_cache(instance,
                                               NULL, 0, NULL, 0, NULL, 0);
            }
            MBN_SW_CONFIG_LOCK();

            //Reset any MBN flags or states as MBN init going to restart
            qcril_qmi_pdc_reset_mbn_state();

            if (qcril_mbn_sw_cur_state == QCRIL_MBN_SW_STATE_LOADING_CONFIG ||
                qcril_mbn_sw_cur_state == QCRIL_MBN_SW_STATE_LOAD_INTERRUPTED)
            {
                qcril_mbn_sw_cur_state = QCRIL_MBN_SW_STATE_LOAD_INTERRUPTED;
            }
            else
            {
                qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                           (void *)(intptr_t)QCRIL_MBN_INTERNAL_TOKEN_ID);

                qcril_mbn_sw_cur_state = QCRIL_MBN_SW_STATE_NONE;
            }
            QCRIL_LOG_DEBUG("qcril_mbn_sw_set_cur_state new_state: %d",
                                                   qcril_mbn_sw_cur_state);

            qcril_mbn_cur_instance_id = 0;
            break;
        }

        case QCRIL_MBN_SW_MODEM_SWITCH_STATE_COMPLETED:
        {
            qcril_modem_switch_state = state;

            for (instance = 0;
                 instance < qcril_mbn_max_apss_instance_id; instance++)
            {
                qcril_qmi_pdc_get_sim_info( instance,
                                            iccid, sizeof(iccid),
                                            mcc, sizeof(mcc),
                                            mnc, sizeof(mnc));

                //Sub is not ready when both ICCID &MCC/MNC are invalid
                if ((!strlen(iccid) || !strncmp(iccid, "null", PROP_VALUE_MAX-1)) &&
                    ((!strlen(mcc) || !strncmp(mcc, "null", QCRIL_MCC_MNC_MAX_SIZE)) ||
                    (!strlen(mnc) || !strncmp(mnc, "null", QCRIL_MCC_MNC_MAX_SIZE))))
                {
                    QCRIL_LOG_DEBUG("SUB%d SIM is not ready yet...", instance);
                }
                else
                {
                    restart_needed = 1;
                    break;
                }
            }
            break;
        }

        default:
        {
            break;
        }
    }

    MBN_SW_CONFIG_UNLOCK();

    if( restart_needed )
    {
        qcril_mbn_sw_update_init(FALSE);
    }

} // qcril_qmi_sw_mbn_modem_switch

/*===========================================================================

  FUNCTION qcril_qmi_sw_mbn_reboot_cleanup

===========================================================================*/
/*!
    @brief
    Clean up sw mbn state

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_sw_mbn_reboot_cleanup (void)
{
    qcril_mbn_sw_cur_state = QCRIL_MBN_SW_STATE_BOOTUP_NEEDED;

    memset(qcril_mbn_sw_iccid, 0, PDC_CONFIG_ID_SIZE_MAX_V01);
    qcril_mbn_sw_iccid_len  = 0;

#ifdef QMI_RIL_UTF
    memset(sw_config_id, 0, sizeof(sw_config_id));
    memset(sw_config_id_len, 0, sizeof(sw_config_id_len));
#endif

    memset(active_sw_config_id, 0, PDC_CONFIG_ID_SIZE_MAX_V01);
    active_sw_config_id_len = 0;

    MBN_SW_CONFIG_LOCK();
    qcril_mbn_cur_instance_id = 0;
    qcril_mbn_need_restart    = 0;
    qcril_mbn_cur_sub_id      = 0;
    qcril_mbn_feature_modem_disabled = false;
    memset(qcril_mbn_iccid_cache, 0, sizeof(qcril_mbn_iccid_cache));
    memset(qcril_mbn_mcc_cache, 0, sizeof(qcril_mbn_mcc_cache));
    memset(qcril_mbn_mnc_cache, 0, sizeof(qcril_mbn_mnc_cache));
    memset(qcril_mbn_sub_id_cache, 0, sizeof(qcril_mbn_sub_id_cache));
    memset(qcril_mbn_cur_instance_iccid, 0, PROP_VALUE_MAX);
    memset(qcril_mbn_cur_instance_mcc, 0, sizeof(qcril_mbn_cur_instance_mcc));
    memset(qcril_mbn_cur_instance_mnc, 0, sizeof(qcril_mbn_cur_instance_mnc));
    MBN_SW_CONFIG_UNLOCK();

} // qcril_qmi_sw_mbn_reboot_cleanup

/*===========================================================================

  FUNCTION qcril_qmi_sw_mbn_db_loaded

===========================================================================*/
/*!
    @brief
    Check if sw db is loaded or not

    @return
    1 if db is loaded, 0 otherwise
*/
/*=========================================================================*/
int qcril_qmi_sw_mbn_loaded_to_db (void)
{
    return !qcril_db_is_sw_mbn_reevaluation_needed();
} // qcril_qmi_sw_mbn_db_loaded
