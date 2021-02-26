
/******************************************************************************
  @file    pdc_utils.c
  @brief   interfaces for communication with QMI PDC

  DESCRIPTION
    Provide interface for MBN operations

  ---------------------------------------------------------------------------

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <utils/Log.h>
#include "qcrili.h"
#include "pdc_utils.h"
#include "qmi_client_instance_defs.h"
#include "persistent_device_configuration_v01.h"
#include "qcril_qmi_client.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PDC_UTIL"

/* log macros */
#define PDC_LOG_INFO                RLOGI
#define PDC_LOG_ERROR               RLOGE

/* pdc client connection retry times */
#define PDC_CLIENT_RETRY_TIMES      20
#define PDC_CLIENT_RETRY_INTERVAL   5
#define PDC_LOAD_FRAME_SIZE         900
#define QMI_SYNC_REQ_DEF_TIMEOUT    10000

#define PDC_NOTUSED(var)            ((var) == (var))

#define TRUE        1
#define FALSE       0

struct pdc_client_info {
    qmi_idl_service_object_type     service_object;
    qmi_client_type                 qmi_client;
    qmi_client_os_params            os_params;
    qmi_client_recv_msg_async_cb    client_cbs;
} g_pdc_client_info;

#define PDC_CONFIG_LOCK()    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex)
#define PDC_CONFIG_UNLOCK()  pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex)

struct pdc_global_info {
    pthread_mutex_t         pdc_config_mutex;
    pthread_mutexattr_t     pdc_config_mutex_attr;

    /* used for LIST_REQ part */
    pthread_cond_t          pdc_list_cond;
    int                     is_pdc_list_handling;
    int                     is_pdc_list_waiting;
    int                     is_pdc_list_done;
    int                     get_list_result;
    pdc_config_list_info **config_list_holder;
    unsigned int            *num_of_configs;
    int                     list_ind_token;

    /* used for LOAD_REQ part */
    pthread_cond_t          pdc_load_cond;
    int                     is_pdc_load_handling;
    int                     is_pdc_load_waiting;
    int                     is_pdc_load_done;
    int                     load_config_result;
    int                     recv_bytes;
    int                     remain_bytes;

    /* used for GET_CONFIG_INFO part */
    pthread_cond_t          pdc_get_info_cond;
    int                     is_pdc_info_handling;
    int                     is_pdc_info_waiting;
    int                     is_pdc_info_done;
    int                     get_info_result;
    struct pdc_config_item_info* config_info_holder;

    /* used for DEACTIVATE_CONFIG part*/
    pthread_cond_t          pdc_deact_cond;
    int                     is_pdc_deact_handling;
    int                     is_pdc_deact_waiting;
    int                     is_pdc_deact_done;
    int                     deact_config_result;

    /* used for DELETE_CONFIG part */
    pthread_cond_t          pdc_del_cond;
    int                     is_pdc_del_handling;
    int                     is_pdc_del_waiting;
    int                     is_pdc_del_done;
    int                     del_config_result;

    /* used for GET_SEL_CONFIG part */
    pthread_cond_t          pdc_get_sel_cond;
    int                     is_pdc_get_sel_handling;
    int                     is_pdc_get_sel_waiting;
    int                     is_pdc_get_sel_done;
    int                     get_sel_result;
    struct pdc_config_pdc_info *get_sel_holder;

    /* used for SELECT_CONFIG part */
    pthread_cond_t          pdc_set_sel_cond;
    int                     is_pdc_set_sel_handling;
    int                     is_pdc_set_sel_waiting;
    int                     is_pdc_set_sel_done;
    int                     sel_config_result;

    /* used for ACTIVATE_CONFIG part */
    pthread_cond_t          pdc_act_cond;
    int                     is_pdc_act_handling;
    int                     is_pdc_act_waiting;
    int                     is_pdc_act_done;
    int                     act_config_result;

    /* used for SET_FEATURE part */
    pthread_cond_t          pdc_set_feature_cond;
    int                     is_pdc_set_feature_handling;
    int                     is_pdc_set_feature_waiting;
    int                     is_pdc_set_feature_done;
    int                     set_feature_result;

    /* add other parts if needed */
};
static struct pdc_global_info g_pdc_info;

/* handle QMI_QDC_LOAD_CONFIG_IND_V01 */
void qmi_pdc_load_config_ind_hdlr(void *ind_data_ptr, uint32_t ind_data_len)
{
    pdc_load_config_ind_msg_v01 *load_ind;
    int result = PDC_ERROR;

    PDC_NOTUSED(ind_data_len);
    if (ind_data_ptr == NULL) {
        PDC_LOG_ERROR("NULL pointer for ind_dat_ptr");
        pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
        g_pdc_info.load_config_result = PDC_ERROR;
        if (g_pdc_info.is_pdc_load_waiting)
            pthread_cond_signal(&g_pdc_info.pdc_load_cond);
        g_pdc_info.is_pdc_load_done = 1;
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return;
    }

    load_ind = (pdc_load_config_ind_msg_v01*)ind_data_ptr;
    if (load_ind->error != QMI_ERR_NONE_V01) {
        switch (load_ind->error) {
        case QMI_ERR_INVALID_ID_V01:
            PDC_LOG_ERROR("Invalid config id, maybe already exists");
            result = PDC_INVALID_ID;
            break;
        case QMI_ERR_NO_MEMORY_V01:
            PDC_LOG_ERROR("no memory in modem EFS");
            result = PDC_NO_MEMORY;
            break;
        default:
            result = PDC_ERROR;
            PDC_LOG_ERROR("load configuration error, code = %d", load_ind->error);
        }
    } else {
        if ((load_ind->frame_data_reset_valid) && (load_ind->frame_data_reset = 1)) {
            PDC_LOG_ERROR("PDC frame data is reset");
            result = PDC_FRAME_RESET;
        } else {
            result = PDC_SUCCESS;
            pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
            if (load_ind->remaining_config_size_valid) {
                g_pdc_info.remain_bytes = load_ind->remaining_config_size;
            }
            if (load_ind->received_config_size_valid) {
                g_pdc_info.recv_bytes = load_ind->received_config_size;
            }
            pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        }
    }

    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.load_config_result = result;
    if (g_pdc_info.is_pdc_load_waiting)
        pthread_cond_signal(&g_pdc_info.pdc_load_cond);
    g_pdc_info.is_pdc_load_done = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
}

/* QMI_PDC_DEACTIVATE_CONFIG_IND_V01 */
void qmi_pdc_deactivate_config_ind_hdlr(void *ind_data_ptr, uint32_t ind_data_len)
{
    pdc_deactivate_config_ind_msg_v01 *deact_ind;
    int result;

    PDC_NOTUSED(ind_data_len);
    if (ind_data_ptr == NULL) {
        PDC_LOG_ERROR("NULL pointer for ind_data_ptr");
        pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
        g_pdc_info.deact_config_result = PDC_ERROR;
        if (g_pdc_info.is_pdc_deact_waiting)
            pthread_cond_signal(&g_pdc_info.pdc_deact_cond);
        g_pdc_info.is_pdc_deact_done = 1;
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return;
    }

    deact_ind = (pdc_deactivate_config_ind_msg_v01*)ind_data_ptr;
    if (deact_ind->error != QMI_ERR_NONE_V01) {
        PDC_LOG_ERROR("QMI error, error code: %d", deact_ind->error);
        result = PDC_ERROR;
    } else {
        result = PDC_SUCCESS;
    }

    /* wake up the wait process */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.deact_config_result = result;
    if (g_pdc_info.is_pdc_deact_waiting)
        pthread_cond_signal(&g_pdc_info.pdc_deact_cond);
    g_pdc_info.is_pdc_deact_done = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
}

/* QMI_PDC_SET_SELECTED_CONFIG_IND_V01 */
void qmi_pdc_select_config_ind_hdlr(void *ind_data_ptr, uint32_t ind_data_len)
{
    pdc_set_selected_config_ind_msg_v01 *set_ind;
    int result;

    PDC_NOTUSED(ind_data_len);
    if (ind_data_ptr == NULL) {
        PDC_LOG_ERROR("NULL pointer for ind_data_ptr");
        pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
        g_pdc_info.sel_config_result = PDC_ERROR;
        if (g_pdc_info.is_pdc_set_sel_waiting)
            pthread_cond_signal(&g_pdc_info.pdc_set_sel_cond);
        g_pdc_info.is_pdc_set_sel_done = 1;
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return;
    }

    set_ind = (pdc_set_selected_config_ind_msg_v01*)ind_data_ptr;
    if (set_ind->error != QMI_ERR_NONE_V01) {
        PDC_LOG_ERROR("QMI error, error code: %d", set_ind->error);
        result = PDC_ERROR;
    } else {
        result = PDC_SUCCESS;
    }

    /* wake up the wait process */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.sel_config_result = result;
    if (g_pdc_info.is_pdc_set_sel_waiting)
        pthread_cond_signal(&g_pdc_info.pdc_set_sel_cond);
    g_pdc_info.is_pdc_set_sel_done = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
}

/* QMI_PDC_ACTIVATE_CONFIG_IND_V01 */
void qmi_pdc_activate_config_ind_hdlr(void *ind_data_ptr, uint32_t ind_data_len)
{
    pdc_activate_config_ind_msg_v01 *act_ind;
    int result;

    PDC_NOTUSED(ind_data_len);
    PDC_LOG_INFO("entering %s", __func__);
    if (ind_data_ptr == NULL) {
        PDC_LOG_ERROR("NULL pointer for ind_data_ptr");
        pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
        g_pdc_info.act_config_result = PDC_ERROR;
        if (g_pdc_info.is_pdc_act_waiting)
            pthread_cond_signal(&g_pdc_info.pdc_act_cond);
        g_pdc_info.is_pdc_act_done = 1;
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return;
    }

    act_ind = (pdc_activate_config_ind_msg_v01*)ind_data_ptr;
    if (act_ind->error != QMI_ERR_NONE_V01) {
        PDC_LOG_ERROR("QMI error, error code: %d", act_ind->error);
        result = PDC_ERROR;
    } else {
        result = PDC_SUCCESS;
    }

    /* wake up the wait process */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.act_config_result = result;
    if (g_pdc_info.is_pdc_act_waiting)
        pthread_cond_signal(&g_pdc_info.pdc_act_cond);
    g_pdc_info.is_pdc_act_done = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
}

/* QMI_PDC_SET_FEATURE_IND_V01 */
void qmi_pdc_set_feature_ind_hdlr(void *ind_data_ptr, uint32_t ind_data_len)
{
    pdc_set_feature_ind_msg_v01 *set_feature_ind;
    int result;

    PDC_NOTUSED(ind_data_len);
    PDC_LOG_INFO("entering %s", __func__);
    if (ind_data_ptr == NULL) {
        PDC_LOG_ERROR("NULL pointer for ind_data_ptr");
        result = PDC_ERROR;
    } else {
        set_feature_ind = (pdc_set_feature_ind_msg_v01*)ind_data_ptr;
        if (set_feature_ind->error != QMI_ERR_NONE_V01) {
            PDC_LOG_ERROR("QMI error, error code: %d", set_feature_ind->error);
            result = PDC_ERROR;
        } else {
            result = PDC_SUCCESS;
        }
    }

    /* wake up the wait process */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.set_feature_result = result;
    if (g_pdc_info.is_pdc_set_feature_waiting)
         pthread_cond_signal(&g_pdc_info.pdc_set_feature_cond);
    g_pdc_info.is_pdc_set_feature_done = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
}

/* QMI_PDC_GET_SELECTED_CONFIG_IND_V01 */
void qmi_pdc_get_selected_config_ind_hdlr(void *ind_data_ptr, uint32_t ind_data_len)
{
    pdc_get_selected_config_ind_msg_v01 *sel_ind;
    int result = PDC_SUCCESS;

    PDC_NOTUSED(ind_data_len);
    if (ind_data_ptr == NULL) {
        PDC_LOG_ERROR("NULL pointer for ind_data_ptr");
        pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
        g_pdc_info.get_sel_result = PDC_ERROR;
        if (g_pdc_info.is_pdc_get_sel_waiting)
            pthread_cond_signal(&g_pdc_info.pdc_get_sel_cond);
        g_pdc_info.is_pdc_get_sel_done = 1;
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return;
    }

    sel_ind = (pdc_get_selected_config_ind_msg_v01*)ind_data_ptr;
    if (sel_ind->error != QMI_ERR_NONE_V01) {
        PDC_LOG_ERROR("QMI error, error code: %d", sel_ind->error);
        result = PDC_ERROR;
    } else {
        pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
        /* have activated config */
        if (sel_ind->active_config_id_valid) {
            if (sel_ind->active_config_id_len > PDC_CONFIG_ID_SIZE_MAX_V01) {
                result = PDC_ERROR;
            } else {
                g_pdc_info.get_sel_holder->config_id_len = sel_ind->active_config_id_len;
                memcpy(g_pdc_info.get_sel_holder->config_id, sel_ind->active_config_id, g_pdc_info.get_sel_holder->config_id_len);

            }
        /* have selected but not activated config */
        } else if (sel_ind->pending_config_id_valid) {
#if 0
            if (sel_ind->pending_config_id_len > PDC_CONFIG_ID_SIZE_MAX_V01) {
                result = PDC_ERROR;
            } else {
                g_pdc_info.get_sel_holder->config_id_len = sel_ind->pending_config_id_len;
                memcpy(g_pdc_info.get_sel_holder->config_id, sel_ind->pending_config_id, g_pdc_info.get_sel_holder->config_id_len);
            }
#else
            PDC_LOG_INFO("has pending configuration");
            result = PDC_NOT_AVAIL;
#endif
        /* no available configs */
        } else {
            result = PDC_NOT_AVAIL;
        }
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
    }

    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.get_sel_result = result;
    if (g_pdc_info.is_pdc_get_sel_waiting)
        pthread_cond_signal(&g_pdc_info.pdc_get_sel_cond);
    g_pdc_info.is_pdc_get_sel_done = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
}

/* QMI_PDC_DELETE_CONFIG_IND_V01 */
void qmi_pdc_delete_config_ind_hdlr(void *ind_data_ptr, uint32_t ind_data_len)
{
    pdc_delete_config_ind_msg_v01 *del_ind;
    int result;

    PDC_NOTUSED(ind_data_len);
    if (ind_data_ptr == NULL) {
        PDC_LOG_ERROR("NULL pointer for ind_data_ptr");
        pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
        g_pdc_info.del_config_result = PDC_ERROR;
        if (g_pdc_info.is_pdc_del_waiting)
            pthread_cond_signal(&g_pdc_info.pdc_del_cond);
        g_pdc_info.is_pdc_del_done = 1;
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return;
    }

    del_ind = (pdc_delete_config_ind_msg_v01*)ind_data_ptr;
    if (del_ind->error != QMI_ERR_NONE_V01) {
        PDC_LOG_ERROR("QMI error, error code: %d", del_ind->error);
        result = PDC_ERROR;
    } else {
        result = PDC_SUCCESS;
    }

    /* wake up the wait process */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.del_config_result = result;
    if (g_pdc_info.is_pdc_del_waiting)
        pthread_cond_signal(&g_pdc_info.pdc_del_cond);
    g_pdc_info.is_pdc_del_done = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
}

/* handle QMI_PDC_GET_CONFIG_INFO_IND_V01 */
void qmi_pdc_get_config_info_ind_hdlr(void *ind_data_ptr, uint32_t ind_data_len)
{
    pdc_get_config_info_ind_msg_v01 *get_ind;

    PDC_NOTUSED(ind_data_len);
    if (ind_data_ptr == NULL) {
        PDC_LOG_ERROR("NULL pointer for ind_data_ptr");
        pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
        g_pdc_info.get_info_result = PDC_ERROR;
        if (g_pdc_info.is_pdc_info_waiting)
            pthread_cond_signal(&g_pdc_info.pdc_get_info_cond);
        g_pdc_info.is_pdc_info_done = 1;
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return;
    }

    get_ind = (pdc_get_config_info_ind_msg_v01*)ind_data_ptr;
    /* XXX:Ignore the qmi error code */
#if 0
    if (list_ind->error != QMI_ERR_NONE_V01) {
        PDC_LOG_ERROR("QMI error, error code: %d", get_ind->error);
        result = PDC_ERROR;
    } else {
        result = PDC_SUCCESS;
    }
#endif
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.config_info_holder->config_desc_valid = get_ind->config_desc_valid;
    if (get_ind->config_desc_valid) {
        g_pdc_info.config_info_holder->config_desc_len = get_ind->config_desc_len;
        memcpy(g_pdc_info.config_info_holder->config_desc,
                    get_ind->config_desc, get_ind->config_desc_len);
    }
    g_pdc_info.config_info_holder->config_version_valid = get_ind->config_version_valid;
    g_pdc_info.config_info_holder->config_version = get_ind->config_version;
    g_pdc_info.config_info_holder->base_version_valid = get_ind->base_version_valid;
    g_pdc_info.config_info_holder->base_version = get_ind->base_version;
    g_pdc_info.get_info_result = PDC_SUCCESS;
    if (g_pdc_info.is_pdc_info_waiting)
        pthread_cond_signal(&g_pdc_info.pdc_get_info_cond);
    g_pdc_info.is_pdc_info_done = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

}

void qmi_pdc_list_configs_follow_up_request(void *param) {

    (void)param;
    pdc_list_configs_req_msg_v01 qmi_request;
    pdc_list_configs_resp_msg_v01 qmi_response;
    qmi_client_error_type qmi_error;
    RIL_Errno res = RIL_E_GENERIC_FAILURE;

    /* Fill QMI request */
    memset( &qmi_request, 0, sizeof(qmi_request) );
    qmi_request.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
    qmi_request.config_type_valid = 1;

    qmi_request.ind_token_valid = 1;
    qmi_request.ind_token = g_pdc_info.list_ind_token;

    qmi_request.multi_support = 1;
    qmi_request.multi_support_valid = TRUE;

    qmi_error = qmi_client_send_msg_sync(g_pdc_client_info.qmi_client,
                                        QMI_PDC_LIST_CONFIGS_REQ_V01,
                                        &qmi_request,
                                        sizeof(qmi_request),
                                        &qmi_response,
                                        sizeof(qmi_response),
                                        QMI_SYNC_REQ_DEF_TIMEOUT);

    res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error,
                                                              &qmi_response.resp);

    if (res != RIL_E_SUCCESS) {
        PDC_LOG_ERROR("qmi send mssage error for list_configs, errno: %d %d",
                                                                qmi_error, res);
        pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
        g_pdc_info.get_list_result = PDC_ERROR;
        if (g_pdc_info.is_pdc_list_waiting)
            pthread_cond_signal(&g_pdc_info.pdc_list_cond);
        g_pdc_info.is_pdc_list_done = 1;
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
    }
}

/* handle QMI_PDC_LIST_CONFIGS_IND_V01*/
void qmi_pdc_list_configs_ind_hdlr(void *ind_data_ptr, uint32_t ind_data_len)
{
    pdc_list_configs_ind_msg_v01 *list_ind;
    uint32_t i;
    int result;
    bool get_list_complete = TRUE;

    PDC_NOTUSED(ind_data_len);
    if (ind_data_ptr == NULL) {
        PDC_LOG_ERROR("NULL pointer for ind_data_ptr");
        pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
        g_pdc_info.get_list_result = PDC_ERROR;
        if (g_pdc_info.is_pdc_list_waiting)
            pthread_cond_signal(&g_pdc_info.pdc_list_cond);
        g_pdc_info.is_pdc_list_done = 1;
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return;
    }

    list_ind = (pdc_list_configs_ind_msg_v01*)ind_data_ptr;
    if (list_ind->error != QMI_ERR_NONE_V01) {
        PDC_LOG_ERROR("QMI error, error code: %d", list_ind->error);
        result = PDC_ERROR;
    } else  if ((list_ind->config_list_valid) && (list_ind->config_list_len > 0)
            && (list_ind->config_list_len <= PDC_CONFIG_LIST_SIZE_MAX_V01)) {
        PDC_LOG_INFO("succeed to get config_list %d ",list_ind->config_list_len);

        pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);

        //update the num of configs
        *(g_pdc_info.num_of_configs) = *(g_pdc_info.num_of_configs) +
                                        list_ind->config_list_len;
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);


        for (i = 0; i < list_ind->config_list_len; i++) {
            //append the configs to linked list
            pdc_config_list_info *config_info = qcril_malloc(sizeof(pdc_config_list_info));
            if(config_info)
            {
                config_info->config_item.config_id_len = list_ind->config_list[i].config_id_len;
                memcpy(config_info->config_item.config_id,list_ind->config_list[i].config_id,
                                    list_ind->config_list[i].config_id_len);
                config_info->next = *(g_pdc_info.config_list_holder);

                pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
                *(g_pdc_info.config_list_holder) = config_info;
                pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
            } else {
                PDC_LOG_INFO("Unable to allocate memory for the list element: %d", i);
            }
        }

        if (list_ind->more_available_valid && list_ind->ind_token_valid
              && (list_ind->more_available != 0)) {
            //more configs avaiable , send the list request again
            get_list_complete = FALSE;
        }

        result = PDC_SUCCESS;
    } else if (!list_ind->config_list_valid ||
        ((list_ind->config_list_valid) && (list_ind->config_list_len == 0))) {
        PDC_LOG_INFO("None MBN available in modem EFS");
        result = PDC_NOT_AVAIL;
    } else {
        result = PDC_ERROR;
    }

    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.get_list_result = result;
    if (result == PDC_SUCCESS && !get_list_complete) {
        // start the next list request
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    qmi_pdc_list_configs_follow_up_request,NULL,NULL);

    } else {
        if (g_pdc_info.is_pdc_list_waiting)
            pthread_cond_signal(&g_pdc_info.pdc_list_cond);
        g_pdc_info.is_pdc_list_done = 1;
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
    }
}

/*=========================================================================
  FUNCTION:  qmi_pdc_unsol_ind_cb

===========================================================================*/
/*!
    @brief
    Callback for QMI PDC indications

    @return
    None.
 */
 /*=========================================================================*/
void qmi_pdc_unsol_ind_cb
(
    qmi_client_type       user_handle,
    unsigned int          msg_id,
    void                  *ind_buf,
    unsigned int          ind_buf_len,
    void                  *ind_cb_data
)
{
    uint32_t decoded_payload_len = 0;
    qmi_client_error_type qmi_err;
    void* decoded_payload = NULL;

    PDC_NOTUSED(user_handle);
    PDC_NOTUSED(ind_cb_data);
    qmi_err = qmi_idl_get_message_c_struct_len(g_pdc_client_info.service_object,
                                    QMI_IDL_INDICATION, msg_id, &decoded_payload_len);
    if (qmi_err != QMI_NO_ERR) {
        PDC_LOG_ERROR("failed to get message c_struct len");
        return;
    }

    if ((decoded_payload = malloc(decoded_payload_len)) == NULL) {
        PDC_LOG_ERROR("failed to allocate memory for payload");
        return;
    }

    qmi_err = qmi_client_message_decode(g_pdc_client_info.qmi_client, QMI_IDL_INDICATION,
                        msg_id, ind_buf, ind_buf_len, decoded_payload, (int)decoded_payload_len);
    if (qmi_err != QMI_NO_ERR) {
        PDC_LOG_ERROR("failed to decode qmi message, msg_id: 0x%02x", msg_id);
        free(decoded_payload);
        return;
    }

    switch (msg_id) {
    case QMI_PDC_LIST_CONFIGS_IND_V01:
        qmi_pdc_list_configs_ind_hdlr(decoded_payload, decoded_payload_len);
        break;
    case QMI_PDC_LOAD_CONFIG_IND_V01:
	    qmi_pdc_load_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;
    case QMI_PDC_GET_CONFIG_INFO_IND_V01:
        qmi_pdc_get_config_info_ind_hdlr(decoded_payload, decoded_payload_len);
        break;
    case QMI_PDC_DEACTIVATE_CONFIG_IND_V01:
        qmi_pdc_deactivate_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;
    case QMI_PDC_DELETE_CONFIG_IND_V01:
        qmi_pdc_delete_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;
    case QMI_PDC_GET_SELECTED_CONFIG_IND_V01:
        qmi_pdc_get_selected_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;
    case QMI_PDC_SET_SELECTED_CONFIG_IND_V01:
        qmi_pdc_select_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;
    case QMI_PDC_ACTIVATE_CONFIG_IND_V01:
        qmi_pdc_activate_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;
    case QMI_PDC_SET_FEATURE_IND_V01:
        qmi_pdc_set_feature_ind_hdlr(decoded_payload, decoded_payload_len);
        break;
    /* TODO: add other cases if needed */
    }

    free(decoded_payload);
}

int pdc_init()
{
    qmi_client_qmux_instance_type qmi_master_port;
    qmi_client_error_type client_err = QMI_NO_ERR;
    int retry, time_out = 4;

    memset(&g_pdc_info, 0x00, sizeof(g_pdc_info));
    pthread_mutexattr_init(&g_pdc_info.pdc_config_mutex_attr);
    pthread_mutex_init(&g_pdc_info.pdc_config_mutex, &g_pdc_info.pdc_config_mutex_attr);
    pthread_cond_init(&g_pdc_info.pdc_list_cond, NULL);
    pthread_cond_init(&g_pdc_info.pdc_load_cond, NULL);
    pthread_cond_init(&g_pdc_info.pdc_get_info_cond, NULL);
    pthread_cond_init(&g_pdc_info.pdc_deact_cond, NULL);
    pthread_cond_init(&g_pdc_info.pdc_del_cond, NULL);
    pthread_cond_init(&g_pdc_info.pdc_get_sel_cond, NULL);
    pthread_cond_init(&g_pdc_info.pdc_set_sel_cond, NULL);
    pthread_cond_init(&g_pdc_info.pdc_act_cond, NULL);
    pthread_cond_init(&g_pdc_info.pdc_set_feature_cond, NULL);

    /* PDC QMI client init */
    qmi_master_port = qmi_ril_client_get_master_port();
    g_pdc_client_info.service_object = pdc_get_service_object_v01();
    for (retry = 0; retry < PDC_CLIENT_RETRY_TIMES; retry++) {
        client_err = qmi_client_init_instance(g_pdc_client_info.service_object,
                                    qmi_master_port, qmi_pdc_unsol_ind_cb, NULL,
                                    &g_pdc_client_info.os_params, time_out,
                                    &g_pdc_client_info.qmi_client);
        if (client_err) {
            PDC_LOG_ERROR("qmi_client_init returned failure (%d) for PDC, retry times %d",
                                client_err, retry);
            sleep(PDC_CLIENT_RETRY_INTERVAL);
        } else {
            PDC_LOG_INFO("qmi_client_init succeeds for PDC");
            return PDC_SUCCESS;
        }
    }
    /* fail */
    g_pdc_client_info.qmi_client = NULL;
    return PDC_ERROR;
}

int pdc_get_config_lists(pdc_config_list_info **config_list, unsigned int *num_configs)
{
    pdc_list_configs_req_msg_v01 qmi_request;
    pdc_list_configs_resp_msg_v01 qmi_response;
    qmi_client_error_type qmi_error;
    int result = PDC_ERROR;
    RIL_Errno res = RIL_E_GENERIC_FAILURE;

    /* check if it is busy */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (g_pdc_info.is_pdc_list_handling) {
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return PDC_BUSY;
    }
    g_pdc_info.is_pdc_list_handling = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    /* Fill QMI request */
    memset( &qmi_request, 0, sizeof(qmi_request) );
    qmi_request.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
    qmi_request.config_type_valid = 1;

    qmi_request.multi_support = 1;
    qmi_request.multi_support_valid = TRUE;

    qmi_request.ind_token_valid = 1;
    qmi_request.ind_token = 1;

    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.config_list_holder = config_list;
    g_pdc_info.num_of_configs = num_configs;
    g_pdc_info.list_ind_token = 1;
    g_pdc_info.get_list_result = PDC_ERROR;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    qmi_error = qmi_client_send_msg_sync(g_pdc_client_info.qmi_client,
                                        QMI_PDC_LIST_CONFIGS_REQ_V01,
                                        &qmi_request,
                                        sizeof(qmi_request),
                                        &qmi_response,
                                        sizeof(qmi_response),
                                        QMI_SYNC_REQ_DEF_TIMEOUT);


    res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error,
                                                              &qmi_response.resp);
    if (res != RIL_E_SUCCESS) {
        PDC_LOG_ERROR("qmi send mssage error for list_configs, errno: %d %d",
                                                                qmi_error, res);
        g_pdc_info.is_pdc_list_handling = 0;
        return PDC_ERROR;
    }

    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (!g_pdc_info.is_pdc_list_done) {
        g_pdc_info.is_pdc_list_waiting = 1;
        pthread_cond_wait(&g_pdc_info.pdc_list_cond, &g_pdc_info.pdc_config_mutex);
    }

    g_pdc_info.config_list_holder = NULL;
    g_pdc_info.num_of_configs = NULL;
    result = g_pdc_info.get_list_result;
    g_pdc_info.is_pdc_list_waiting = 0;
    g_pdc_info.is_pdc_list_done = 0;
    g_pdc_info.is_pdc_list_handling = 0;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    return result;
}

int pdc_load_config_segment(uint8_t* buf, uint32_t size, uint32_t total_size, struct pdc_config_pdc_info* config_id, pdc_config_type_enum_v01 config_type, uint32_t* remain)
{
    pdc_load_config_req_msg_v01 qmi_request;
    pdc_load_config_resp_msg_v01 qmi_response;
    pdc_load_config_info_type_v01 *p_load_info;
    qmi_client_error_type qmi_error;
    int result = PDC_ERROR;
    RIL_Errno res = RIL_E_GENERIC_FAILURE;

    /* check if it is busy */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (g_pdc_info.is_pdc_load_handling) {
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return PDC_BUSY;
    }
    g_pdc_info.is_pdc_load_handling = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    /* Fill QMI request */
    memset(&qmi_request, 0, sizeof(qmi_request));
    p_load_info = &qmi_request.load_config_info;
    p_load_info->config_type = config_type;
    p_load_info->total_config_size = total_size;
    /* Fill configuration_id */
    if ((p_load_info->config_id_len = config_id->config_id_len) > PDC_CONFIG_ID_SIZE_MAX_V01) {
        return PDC_INVALID_ID;
    }
    memcpy(p_load_info->config_id, config_id->config_id, p_load_info->config_id_len);
    /* Fill the segment of configure content */
    memcpy(p_load_info->config_frame, buf, size);
    p_load_info->config_frame_len = size;

    /* send QMI load request to modem */
    qmi_error = qmi_client_send_msg_sync(g_pdc_client_info.qmi_client,
                                        QMI_PDC_LOAD_CONFIG_REQ_V01,
                                        &qmi_request,
                                        sizeof(qmi_request),
                                        &qmi_response,
                                        sizeof(qmi_response),
                                        QMI_SYNC_REQ_DEF_TIMEOUT);
    res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error,
                                                            &qmi_response.resp);
    if (res != RIL_E_SUCCESS) {
        PDC_LOG_ERROR("qmi send message error for load_configs, errno: %d %d",
                                                                qmi_error, res);
        g_pdc_info.is_pdc_load_handling = 0;
        return PDC_ERROR;
    }
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (!g_pdc_info.is_pdc_load_done) {
        g_pdc_info.is_pdc_load_waiting = 1;
        pthread_cond_wait(&g_pdc_info.pdc_load_cond, &g_pdc_info.pdc_config_mutex);
    }
    *remain = g_pdc_info.remain_bytes;
    result = g_pdc_info.load_config_result;
    g_pdc_info.is_pdc_load_waiting = 0;
    g_pdc_info.is_pdc_load_handling = 0;
    g_pdc_info.is_pdc_load_done = 0;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    return result;
}

int pdc_load_config(char* file_name, struct pdc_config_pdc_info* config_id, pdc_config_type_enum_v01 config_type)
{
    int fd = -1, result;
    uint8_t* buf;
    struct stat f_stat;
    ssize_t rd_len;
    uint32_t remain;

    /* open the file and get the stat info */
    if ((fd = open(file_name, O_RDONLY)) == -1) {
        PDC_LOG_ERROR("Failed to open file: %s %s", file_name, strerror(errno));
        return PDC_ERROR;
    }
    if (fstat(fd, &f_stat) == -1) {
        PDC_LOG_ERROR("Failed to fstat file: %s %s", file_name, strerror(errno));
        close(fd);
        return PDC_ERROR;
    }
    PDC_LOG_INFO("The MBN file size is %lld", (long long)f_stat.st_size);

    if ((buf = malloc(PDC_LOAD_FRAME_SIZE)) == NULL) {
        PDC_LOG_ERROR("Failed to allocate memory");
        close(fd);
        return PDC_ERROR;
    }

    do {
        do {
            rd_len = read(fd, buf, PDC_LOAD_FRAME_SIZE);
        } while ((rd_len == -1) && (errno == EINTR));

        if (rd_len == -1) {
            PDC_LOG_ERROR("Failed to read MBN file: %s %s", file_name, strerror(errno));
            result = PDC_ERROR;
            break;
        } else if (rd_len == 0) { /* reach file end */
            PDC_LOG_INFO("Reach file end: %s", file_name);
            result = PDC_SUCCESS;
            break;
        }
        result = pdc_load_config_segment(buf, (uint32_t)rd_len, f_stat.st_size, config_id, config_type, &remain);
        if (result != PDC_SUCCESS) {
            PDC_LOG_ERROR("Failed to load config segments, result: %d", result);
            break;
        }
        PDC_LOG_INFO("The remaining size: %d", remain);
        if (remain == 0) {
            break;
        }
    } while (TRUE);

    free(buf);
    close(fd);
    return result;
}

int pdc_delete_config(struct pdc_config_pdc_info* config_id, pdc_config_type_enum_v01 config_type)
{
    pdc_delete_config_req_msg_v01 qmi_request;
    pdc_delete_config_resp_msg_v01 qmi_response;
    qmi_client_error_type qmi_error;
    int result;
    RIL_Errno res = RIL_E_GENERIC_FAILURE;

    /* check if it is busy */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (g_pdc_info.is_pdc_del_handling) {
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return PDC_BUSY;
    }
    g_pdc_info.is_pdc_del_handling = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    /* Fill the QMI request structure */
    memset(&qmi_request, 0, sizeof(qmi_request));
    qmi_request.config_type = config_type;
    qmi_request.config_id_valid = 1;
    if ((qmi_request.config_id_len = config_id->config_id_len) > PDC_CONFIG_ID_SIZE_MAX_V01) {
        return PDC_INVALID_ID;
    }
    memcpy(qmi_request.config_id, config_id->config_id, qmi_request.config_id_len);

    /* send QMI reqeust */
    qmi_error = qmi_client_send_msg_sync(g_pdc_client_info.qmi_client,
                                        QMI_PDC_DELETE_CONFIG_REQ_V01,
                                        &qmi_request,
                                        sizeof(qmi_request),
                                        &qmi_response,
                                        sizeof(qmi_response),
                                        QMI_SYNC_REQ_DEF_TIMEOUT);
    res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error,
                                                            &qmi_response.resp);
    if (res != RIL_E_SUCCESS) {
        PDC_LOG_ERROR("qmi send mssage error for del_config, errno: %d %d",
                                                                qmi_error, res);
        g_pdc_info.is_pdc_del_handling = 0;
        return PDC_ERROR;
    }

    /* wait for IND */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (!g_pdc_info.is_pdc_del_done) {
        g_pdc_info.is_pdc_del_waiting = 1;
        pthread_cond_wait(&g_pdc_info.pdc_del_cond, &g_pdc_info.pdc_config_mutex);
    }
    result = g_pdc_info.del_config_result;
    g_pdc_info.is_pdc_del_waiting = 0;
    g_pdc_info.is_pdc_del_handling = 0;
    g_pdc_info.is_pdc_del_done = 0;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    return result;
}

int pdc_deactivate_config(uint32_t sub_id, pdc_config_type_enum_v01 config_type)
{
    pdc_deactivate_config_req_msg_v01 qmi_request;
    pdc_deactivate_config_resp_msg_v01 qmi_response;
    qmi_client_error_type qmi_error;
    int result;

    /* check if it is busy */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (g_pdc_info.is_pdc_deact_handling) {
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return PDC_BUSY;
    }
    g_pdc_info.is_pdc_deact_handling = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    /* Fill QMI request & send request */
    memset(&qmi_request, 0, sizeof(qmi_request));
    qmi_request.config_type = config_type;
    qmi_request.subscription_id_valid = 1;
    qmi_request.subscription_id = sub_id;
    qmi_error = qmi_client_send_msg_sync(g_pdc_client_info.qmi_client,
                                        QMI_PDC_DEACTIVATE_CONFIG_REQ_V01,
                                        &qmi_request,
                                        sizeof(qmi_request),
                                        &qmi_response,
                                        sizeof(qmi_response),
                                        QMI_SYNC_REQ_DEF_TIMEOUT);
    if (qmi_error != QMI_NO_ERR) {
        PDC_LOG_ERROR("qmi send mssage error for deactivate_config, errno: %d", qmi_error);
        return PDC_ERROR;
    }

    /* wait for IND */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (!g_pdc_info.is_pdc_deact_done) {
        g_pdc_info.is_pdc_deact_waiting = 1;
        pthread_cond_wait(&g_pdc_info.pdc_deact_cond, &g_pdc_info.pdc_config_mutex);
    }
    result = g_pdc_info.deact_config_result;
    g_pdc_info.is_pdc_deact_handling = 0;
    g_pdc_info.is_pdc_deact_waiting = 0;
    g_pdc_info.is_pdc_deact_done = 0;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    return result;
}

int pdc_get_config_info(struct pdc_config_pdc_info* config_id, struct pdc_config_item_info *config_info, pdc_config_type_enum_v01 config_type)
{
    pdc_get_config_info_req_msg_v01 qmi_request;
    pdc_get_config_info_resp_msg_v01 qmi_response;
    qmi_client_error_type qmi_error;
    int result;
    RIL_Errno res = RIL_E_GENERIC_FAILURE;

    /* check if it is busy */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (g_pdc_info.is_pdc_info_handling) {
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return PDC_BUSY;
    }
    g_pdc_info.is_pdc_info_handling = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    /* Fill QMI request */
    memset(&qmi_request, 0, sizeof(qmi_request));
    qmi_request.new_config_info.config_type = config_type;
    if ((qmi_request.new_config_info.config_id_len = config_id->config_id_len) > PDC_CONFIG_ID_SIZE_MAX_V01) {
        return PDC_INVALID_ID;
    }
    memcpy(qmi_request.new_config_info.config_id, config_id->config_id,
                    qmi_request.new_config_info.config_id_len);
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.config_info_holder = config_info;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
    /* sending request */
    qmi_error = qmi_client_send_msg_sync(g_pdc_client_info.qmi_client,
                                        QMI_PDC_GET_CONFIG_INFO_REQ_V01,
                                        &qmi_request,
                                        sizeof(qmi_request),
                                        &qmi_response,
                                        sizeof(qmi_response),
                                        QMI_SYNC_REQ_DEF_TIMEOUT);
    res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error,
                                                            &qmi_response.resp);
    if (res != RIL_E_SUCCESS) {
        PDC_LOG_ERROR("qmi send mssage error for get_config_info, errno: %d %d",
                                                                qmi_error, res);
        g_pdc_info.is_pdc_info_handling = 0;
        return PDC_ERROR;
    }
    /* waiting for IND */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (!g_pdc_info.is_pdc_info_done) {
        g_pdc_info.is_pdc_info_waiting = 1;
        pthread_cond_wait(&g_pdc_info.pdc_get_info_cond, &g_pdc_info.pdc_config_mutex);
    }
    result = g_pdc_info.get_info_result;
    g_pdc_info.config_info_holder = NULL;
    g_pdc_info.is_pdc_info_waiting = 0;
    g_pdc_info.is_pdc_info_handling = 0;
    g_pdc_info.is_pdc_info_done = 0;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    return result;
}

int pdc_get_current_config(struct pdc_config_pdc_info *conf_id, pdc_config_type_enum_v01 config_type, uint32_t sub_id)
{
    pdc_get_selected_config_req_msg_v01 qmi_request;
    pdc_get_selected_config_resp_msg_v01 qmi_response;
    qmi_client_error_type qmi_error;
    int result;
    RIL_Errno res = RIL_E_GENERIC_FAILURE;

    /* check if it is busy */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (g_pdc_info.is_pdc_get_sel_handling) {
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return PDC_BUSY;
    }
    g_pdc_info.is_pdc_get_sel_handling = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    /* Fill QMI request */
    memset(&qmi_request, 0, sizeof(qmi_request));
    qmi_request.config_type = config_type;
    if (config_type == PDC_CONFIG_TYPE_MODEM_SW_V01) {
        /* only SW MBN has sub_id */
        qmi_request.subscription_id_valid = 1;
        qmi_request.subscription_id = sub_id;
    }

    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    g_pdc_info.get_sel_holder = conf_id;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
    /* send QMI request */
    qmi_error = qmi_client_send_msg_sync(g_pdc_client_info.qmi_client,
                                        QMI_PDC_GET_SELECTED_CONFIG_REQ_V01,
                                        &qmi_request,
                                        sizeof(qmi_request),
                                        &qmi_response,
                                        sizeof(qmi_response),
                                        QMI_SYNC_REQ_DEF_TIMEOUT);

    res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error,
                                                            &qmi_response.resp);
    if (res != RIL_E_SUCCESS) {
        g_pdc_info.is_pdc_get_sel_handling = 0;
        PDC_LOG_ERROR("qmi send message error for get_sel_configs, errno: %d %d", qmi_error,res);
        return PDC_ERROR;
    }

    /* waiting for IND */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (!g_pdc_info.is_pdc_get_sel_done) {
        g_pdc_info.is_pdc_get_sel_waiting = 1;
        pthread_cond_wait(&g_pdc_info.pdc_get_sel_cond, &g_pdc_info.pdc_config_mutex);
    }
    g_pdc_info.get_sel_holder = NULL;
    result = g_pdc_info.get_sel_result;
    g_pdc_info.is_pdc_get_sel_waiting = 0;
    g_pdc_info.is_pdc_get_sel_done = 0;
    g_pdc_info.is_pdc_get_sel_handling = 0;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    return result;
}

int pdc_select_config(struct pdc_config_pdc_info *conf_id, pdc_config_type_enum_v01 config_type, uint32_t sub_id)
{
    pdc_set_selected_config_req_msg_v01 qmi_request;
    pdc_set_selected_config_resp_msg_v01 qmi_response;
    qmi_client_error_type qmi_error;
    int result;

    /* check if it is busy */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (g_pdc_info.is_pdc_set_sel_handling) {
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return PDC_BUSY;
    }
    g_pdc_info.is_pdc_set_sel_handling = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    /* Fill QMI request */
    memset(&qmi_request, 0, sizeof(qmi_request));
    qmi_request.new_config_info.config_type = config_type;
    if (conf_id->config_id_len <= PDC_CONFIG_ID_SIZE_MAX_V01) {
        qmi_request.new_config_info.config_id_len = conf_id->config_id_len;
        memcpy(qmi_request.new_config_info.config_id, conf_id->config_id,
                    qmi_request.new_config_info.config_id_len);
    } else {
        return PDC_INVALID_ID;
    }
    if (config_type == PDC_CONFIG_TYPE_MODEM_SW_V01) {
        /* only SW MBN has sub_id */
        qmi_request.subscription_id_valid = 1;
        qmi_request.subscription_id = sub_id;
    }

    /* send QMI request */
    qmi_error = qmi_client_send_msg_sync(g_pdc_client_info.qmi_client,
                                        QMI_PDC_SET_SELECTED_CONFIG_REQ_V01,
                                        &qmi_request,
                                        sizeof(qmi_request),
                                        &qmi_response,
                                        sizeof(qmi_response),
                                        QMI_SYNC_REQ_DEF_TIMEOUT);
    if (qmi_error != QMI_NO_ERR) {
        PDC_LOG_ERROR("qmi send message error for set_selected_config, errno: %d", qmi_error);
        return PDC_ERROR;
    }

    /* wait for IND */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (!g_pdc_info.is_pdc_set_sel_done) {
        g_pdc_info.is_pdc_set_sel_waiting = 1;
        pthread_cond_wait(&g_pdc_info.pdc_set_sel_cond, &g_pdc_info.pdc_config_mutex);
    }
    result = g_pdc_info.sel_config_result;
    g_pdc_info.is_pdc_set_sel_handling = 0;
    g_pdc_info.is_pdc_set_sel_waiting = 0;
    g_pdc_info.is_pdc_set_sel_done = 0;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    return result;
}

int pdc_activate_config(pdc_config_type_enum_v01 config_type, uint32_t sub_id)
{
    pdc_activate_config_req_msg_v01 qmi_request;
    pdc_activate_config_resp_msg_v01 qmi_response;
    qmi_client_error_type qmi_error;
    int result;

    /* check if it is busy */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (g_pdc_info.is_pdc_act_handling) {
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return PDC_BUSY;
    }
    g_pdc_info.is_pdc_act_handling = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    /* Fill QMI request */
    memset(&qmi_request, 0, sizeof(qmi_request));
    qmi_request.config_type = config_type;
    if (config_type == PDC_CONFIG_TYPE_MODEM_SW_V01) {
        /* only SW MBN has sub_id */
        qmi_request.subscription_id_valid = 1;
        qmi_request.subscription_id = sub_id;
    }

    /* send QMI request */
    qmi_error = qmi_client_send_msg_sync(g_pdc_client_info.qmi_client,
                                        QMI_PDC_ACTIVATE_CONFIG_REQ_V01,
                                        &qmi_request,
                                        sizeof(qmi_request),
                                        &qmi_response,
                                        sizeof(qmi_response),
                                        QMI_SYNC_REQ_DEF_TIMEOUT);
    if (qmi_error != QMI_NO_ERR) {
        PDC_LOG_ERROR("qmi send message error for activate_config, errno: %d", qmi_error);
        return PDC_ERROR;
    }

    /* wait for IND */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (!g_pdc_info.is_pdc_act_done) {
        g_pdc_info.is_pdc_act_waiting = 1;
        pthread_cond_wait(&g_pdc_info.pdc_act_cond, &g_pdc_info.pdc_config_mutex);
    }
    result = g_pdc_info.act_config_result;
    g_pdc_info.is_pdc_act_handling = 0;
    g_pdc_info.is_pdc_act_waiting = 0;
    g_pdc_info.is_pdc_act_done = 0;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    return result;
}

int pdc_set_feature(pdc_set_feature_req_msg_v01* qmi_request)
{
    pdc_set_feature_resp_msg_v01 qmi_response;
    qmi_client_error_type qmi_error;
    int result;

    /* check if it is busy */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (g_pdc_info.is_pdc_set_feature_handling) {
        pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);
        return PDC_BUSY;
    }
    g_pdc_info.is_pdc_set_feature_handling = 1;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    /* send QMI request */
    qmi_error = qmi_client_send_msg_sync(g_pdc_client_info.qmi_client,
                                        QMI_PDC_SET_FEATURE_REQ_V01,
                                        qmi_request,
                                        sizeof(*qmi_request),
                                        &qmi_response,
                                        sizeof(qmi_response),
                                        QMI_SYNC_REQ_DEF_TIMEOUT);
    if (qmi_error != QMI_NO_ERR) {

        PDC_LOG_ERROR("qmi send message error for set_feature, errno: %d", qmi_error);
        return PDC_ERROR;
    }

    /* wait for IND */
    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex);
    if (!g_pdc_info.is_pdc_set_feature_done) {
        g_pdc_info.is_pdc_set_feature_waiting = 1;
        pthread_cond_wait(&g_pdc_info.pdc_set_feature_cond, &g_pdc_info.pdc_config_mutex);
    }
    result = g_pdc_info.set_feature_result;
    g_pdc_info.is_pdc_set_feature_handling = 0;
    g_pdc_info.is_pdc_set_feature_waiting = 0;
    g_pdc_info.is_pdc_set_feature_done = 0;
    pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex);

    return result;
}

int pdc_enable_auto_selection(int enable, int slot)
{
    pdc_set_feature_req_msg_v01 qmi_request;
    int result;

    memset(&qmi_request, 0, sizeof(qmi_request));
    qmi_request.slot_id = slot;
    qmi_request.selection_mode_valid = 1;
    qmi_request.flex_mapping_valid = 1;
    if (enable) {
        qmi_request.selection_mode = PDC_SELECTION_MODE_HYBRID_V01;
        qmi_request.flex_mapping = 1;
    } else {
        qmi_request.selection_mode = PDC_SELECTION_MODE_DISABLED_V01;
        qmi_request.flex_mapping = 0;
    }
    result = pdc_set_feature(&qmi_request);

    return result;
}

void pdc_clean()
{
    if (g_pdc_client_info.qmi_client != NULL)
    {
        PDC_LOG_INFO("Release client connection to PDC");
        qmi_client_release(g_pdc_client_info.qmi_client);
    }
    g_pdc_client_info.qmi_client = NULL;
}
