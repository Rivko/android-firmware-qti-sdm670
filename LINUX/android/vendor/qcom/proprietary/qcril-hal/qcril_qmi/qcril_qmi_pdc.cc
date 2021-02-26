/******************************************************************************
#  Copyright (c) 2013-2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_pdc.c
  @brief   qcril qmi - PDC

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI PDC.

******************************************************************************/


//===========================================================================
//
//                           INCLUDE FILES
//
//===========================================================================

#define TAG "QCRIL_QMI_PDC"

#include <errno.h>
#include <cutils/memory.h>
#include <cutils/properties.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include "telephony/ril.h"
#include "comdef.h"
#include "qcrili.h"
#include "qmi_errors.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_client.h"
#include "qcril_qmi_pdc.h"
#include "qmi_ril_platform_dep.h"
#include "qcril_file_utils.h"
#include "qcril_mbn_hw_update.h"
#include "qcril_mbn_sw_update.h"
#include "persistent_device_configuration_v01.h"
#include "qmi_ril_file_observer.h"
#include "modules/nas/qcril_qmi_nas.h"
#include "qcril_qmi_oem_events.h"

#include "framework/Log.h"
#include "modules/qmi/PdcModemEndPoint.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/mbn/MbnFileUpdateIndication.h"

#ifdef QMI_RIL_UTF
#include "../qcril/modules/mbn/MbnModule.h"
#endif

//===========================================================================
//
//                    INTERNAL DEFINITIONS AND TYPES
//
//===========================================================================

#define QCRIL_MODEM_MBN_FILE_PATH             "persist.vendor.radio.mbn_path"
#ifdef QMI_RIL_UTF
#define QCRIL_MBN_SIM_INFO_FILE               "data/misc/radio/iccid"
#define QCRIL_MBN_SIM_INFO_DIR                "data/misc/radio"
#else
#define QCRIL_MBN_SIM_INFO_FILE DATA_PARTITION_ROOT "radio/iccid"
#define QCRIL_MBN_SIM_INFO_DIR  DATA_PARTITION_ROOT "radio"
#endif

#define QCRIL_MODEM_MBN_DEFAULT_PATH        "/data/modem_config/"
#define QCRIL_PDC_FRAME_SIZE                900
#define QCRIL_PDC_MAX_SUBS_SUPPORT          3
#define QCRIL_PDC_ALL_CONFIGS_LEN           ((PDC_CONFIG_ID_SIZE_MAX_V01+1) * PDC_CONFIG_LIST_SIZE_MAX_V01+1)
#define QCRIL_MBN_ACTIVATION_STATUS_PROP_NAME "persist.vendor.radio.stack_id"

#define PDC_CONFIG_LOCK()    pthread_mutex_lock(&g_pdc_info.pdc_config_mutex)
#define PDC_CONFIG_UNLOCK()  pthread_mutex_unlock(&g_pdc_info.pdc_config_mutex)

// error code
#define QCRIL_PDC_NO_ERROR      0
#define QCRIL_PDC_GENERIC_FAIL  -1
#define QCRIL_PDC_NO_MEMORY     -2
#define QCRIL_PDC_NO_CONFIGS    -3
#define QCRIL_PDC_SELECT_FAIL   -4
#define QCRIL_PDC_COMMERCIAL_MBN_PREFIX "commercial"

// validation result related
#define QCRIL_QMI_PDC_INDEX_END         (0xffffffffL)
#define QCRIL_QMI_PDC_IS_LAST_INDEX(idx)   ((idx) == QCRIL_QMI_PDC_INDEX_END)

// default slot_id for disabling modem mbn update for all slots
#define QCRIL_PDC_DEFAULT_SLOT_ID       (0xFFFFFFFF)

typedef struct
{
  // user set info
  uint8_t config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
  uint32_t config_id_len;
  uint32_t conf_size;
  uint32_t load_size;
  int conf_fd;
  pdc_config_type_enum_v01 config_type;
} pdc_config_info_type;

typedef struct {
  // for loading configurations
  pdc_config_info_type pdc_config_info;
  char mbn_file_dir[ PROPERTY_VALUE_MAX ];
  pdc_config_info_resp_type_v01 cur_config_lists[ PDC_CONFIG_LIST_SIZE_MAX_V01 ];
  // for deleting all configurations
  uint32_t cur_config_len;
  uint8_t cur_delete_idx;
  // for select all SUBs
  uint8_t sub_count;
  uint8_t cur_select_sub;
  uint8_t sub_select_mask;
  // for deactivate all SUBs
  uint8_t cur_deact_sub;
  // is processing
  uint8_t is_under_processing;
  // is under cleanup
  uint8_t is_under_cleanup;

  // dump file description (MBN diff dump file: XML format)
  int dump_fd;
  char dump_file[QCRIL_DUMP_FILE_PATH_LEN];

  /* is retrieving mbn info */
  uint8_t is_retrieving_mbn_info;
  uint8_t  config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
  uint32_t config_id_len;

  /* Current active config_id */
  uint8_t  active_config_id[QCRIL_PDC_MAX_SUBS_SUPPORT][PDC_CONFIG_ID_SIZE_MAX_V01];
  uint32_t active_config_id_len[QCRIL_PDC_MAX_SUBS_SUPPORT];

  // for pretecting
  pthread_mutex_t        pdc_config_mutex;
  pthread_mutexattr_t    pdc_config_mutex_attr;
} pdc_global_info_type;

typedef struct {
  pdc_config_type_enum_v01  config_type;
  uint8_t                   multi_support;
  uint8_t                   ind_token_valid;
  uint32_t                  ind_token;
} qcril_pdc_list_config_params;

typedef enum
{
  QCRIL_PDC_MBN_INFO_NONE,
  QCRIL_PDC_MBN_INFO_RETRIEVE_ACTIVE_ID,
  QCRIL_PDC_MBN_INFO_RETRIEVE_CONFIG_INFO
} qcril_pdc_mbn_info_retrieval_state;

typedef enum
{
  QCRIL_PDC_MBN_TYPE_SW = 0,
  QCRIL_PDC_MBN_TYPE_HW,

  QCRIL_PDC_MBN_TYPE_MAX = 0xFFFFFFFF
} qcril_pdc_mbn_type;

static pdc_global_info_type g_pdc_info;

static boolean qcril_qmi_pdc_is_test_mbn
(
    void         *config_desc,
    unsigned int  config_desc_len
);

static void qcril_qmi_pdc_retrieve_mbn_info_for_config_id
(
    void *
);

qmi_client_error_type qmi_client_pdc_send_raw_sync(unsigned int msg_id,
                                                void *req_struct,int req_struct_len,
                                                void *resp_struct,unsigned int resp_struct_len) {
    qmi_client_error_type rc = QMI_INTERNAL_ERR;
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_DEBUG("%s", __FUNCTION__);

    std::shared_ptr<PdcModemEndPoint> mPdcEndPoint =
        ModemEndPointFactory<PdcModemEndPoint>::getInstance().buildEndPoint();
    if(mPdcEndPoint) {
        rc = mPdcEndPoint->sendRawSync(
                msg_id,req_struct,req_struct_len,resp_struct,resp_struct_len);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

qmi_client_error_type qmi_client_pdc_send_raw_async(unsigned int msg_id,
                                                void *req_struct,int req_struct_len,
                                                int resp_struct_len,
                                                qmiAsyncCbType resp_cb,
                                                void *resp_cb_data ) {
    qmi_client_error_type rc = QMI_INTERNAL_ERR;
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_DEBUG("%s", __FUNCTION__);

    (void)resp_cb;
    (void)resp_cb_data;

    std::shared_ptr<PdcModemEndPoint> mPdcEndPoint =
        ModemEndPointFactory<PdcModemEndPoint>::getInstance().buildEndPoint();
    if(mPdcEndPoint) {
        rc = mPdcEndPoint->sendRawAsync(msg_id,req_struct, req_struct_len, resp_struct_len,
                                    resp_cb, resp_cb_data, nullptr);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_load_configuraiton

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_LOAD_CONFIGURATION

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_load_configuration
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
)
{
  pdc_load_config_req_msg_v01 qmi_request;
  pdc_load_config_resp_msg_v01 qmi_response;
  pdc_load_config_info_type_v01 *p_load_info;
  qmi_client_error_type qmi_error;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  char payload;
  int rd_len;
  errno_enum_type reqlist_found;
  qcril_reqlist_public_type req_info;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);
  QCRIL_NOTUSED(params_ptr);

  memset( &qmi_request, 0, sizeof(qmi_request) );
  p_load_info = &qmi_request.load_config_info;
  p_load_info->config_type = g_pdc_info.pdc_config_info.config_type;
  p_load_info->total_config_size = g_pdc_info.pdc_config_info.conf_size;

  strlcpy((char *)p_load_info->config_id, (char *)g_pdc_info.pdc_config_info.config_id, PDC_CONFIG_ID_SIZE_MAX_V01);
  p_load_info->config_id_len = strlen((char *)p_load_info->config_id);


  // read loop in case that it is interrupt by signals
  // for example TASK_FREEZE when sleep */
  if ( g_pdc_info.pdc_config_info.conf_fd == -1)
  {
    rd_len = -1;
    QCRIL_LOG_ERROR("The MBN file descriptor is -1");
  }
  else
  {
    do
    {
      rd_len = read( g_pdc_info.pdc_config_info.conf_fd,
          p_load_info->config_frame, QCRIL_PDC_FRAME_SIZE );
    } while ( ( rd_len == -1 ) && ( errno == EINTR ) );
  }

  if ( rd_len == -1 ) // there is some error when read file
  {
    result = RIL_E_GENERIC_FAILURE;
    QCRIL_LOG_ERROR("failed to read MBN file");
  }
  else if (rd_len == 0) // reach file end
  {
    // should not reach here, since the indication handler
    // will not queue this event if all of this config has
    // been loaded. Treat it as a error (one case: the file
    // length is empty)
    result = RIL_E_GENERIC_FAILURE;
    QCRIL_LOG_ERROR("reach file end, should not happen");
  }
  else
  {
    p_load_info->config_frame_len = rd_len;
    g_pdc_info.pdc_config_info.load_size += rd_len;
    QCRIL_LOG_DEBUG("current_load_size %d", g_pdc_info.pdc_config_info.load_size);
    qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_LOAD_CONFIG_REQ_V01,&qmi_request,
                                sizeof( qmi_request ),&qmi_response,sizeof( qmi_response ));

   result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_error, &qmi_response.resp );
   if ( g_pdc_info.pdc_config_info.load_size >= g_pdc_info.pdc_config_info.conf_size )
   {
     QCRIL_LOG_INFO("load_size is %d, conf_size is %d",
         g_pdc_info.pdc_config_info.load_size, g_pdc_info.pdc_config_info.conf_size);
     close( g_pdc_info.pdc_config_info.conf_fd );
     g_pdc_info.pdc_config_info.conf_fd = -1;
   }
  }

  // on failure, send an unsol message to indicate error
  // on success, do nothing here, the load indication handler
  // will handle the remaining things
  if ( result != RIL_E_SUCCESS )
  {
    payload = QCRIL_PDC_GENERIC_FAIL;
    g_pdc_info.is_under_processing = 0;
#ifndef QMI_RIL_UTF
    qcril_hook_unsol_response( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_EVT_HOOK_UNSOL_PDC_CONFIG,
                                    (char *)(&payload), sizeof(payload));
#endif
    reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                     QCRIL_DEFAULT_MODEM_ID,
                                     QCRIL_EVT_REQUEST_MBN_HW_LOAD_CONFIG,
                                     &req_info );
    if (reqlist_found == E_SUCCESS)
    {
      qcril_mbn_hw_send_load_config_resp(result);
    }
    else
    {
      reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                       QCRIL_DEFAULT_MODEM_ID,
                                       QCRIL_EVT_REQUEST_MBN_SW_LOAD_CONFIG,
                                       &req_info );
      if (reqlist_found == E_SUCCESS)
      {
        qcril_mbn_sw_send_load_config_resp(result);
      }
    }
  }

  QCRIL_LOG_FUNC_RETURN();
}


/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_load_config_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PDC_LOAD_CONFIG_IND_V01

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_load_config_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
  pdc_load_config_ind_msg_v01 *load_ind;
  char payload;
  boolean load_completed = FALSE;
  errno_enum_type           reqlist_found;
  qcril_reqlist_public_type req_info;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ind_data_len);

  if ( ind_data_ptr != NULL )
  {
    load_ind = (pdc_load_config_ind_msg_v01*)ind_data_ptr;
    if ( load_ind->error == QMI_ERR_NONE_V01 )
    {
      // if modem pass the remaining info, we use it to determine
      // whether the load has completed
      if ( load_ind->remaining_config_size_valid )
      {
        QCRIL_LOG_INFO("The remaininng_config_size is %d", load_ind->remaining_config_size);
        QCRIL_LOG_INFO("The received_config_size is %d", load_ind->received_config_size);
        if ( load_ind->remaining_config_size == 0 )
        {
          load_completed = TRUE;
        }
        else
        {

        //TODO remove event loop
          // continue loading config
          qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                        QCRIL_DEFAULT_MODEM_ID,
                        QCRIL_DATA_ON_STACK,
                        QCRIL_EVT_QMI_RIL_PDC_LOAD_CONFIGURATION,
                        NULL,
                        QMI_RIL_ZERO,
                        (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
        }
      }
      else
      {
        if ( g_pdc_info.pdc_config_info.load_size >= g_pdc_info.pdc_config_info.conf_size )
        {
          load_completed = TRUE;
        }
        else
        {

          //TODO remove event
          // continue loading config
          qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                        QCRIL_DEFAULT_MODEM_ID,
                        QCRIL_DATA_ON_STACK,
                        QCRIL_EVT_QMI_RIL_PDC_LOAD_CONFIGURATION,
                        NULL,
                        QMI_RIL_ZERO,
                        (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
        }
      }
    }
    else // there is some error
    {
      // case 1: the config is already loaded
      if ( load_ind->error == QMI_ERR_INVALID_ID_V01 )
      {
        QCRIL_LOG_INFO("Invalid config id, maybe already exists");
        load_completed = TRUE;
      }
      // case 2: no enough space
      else if ( load_ind->error == QMI_ERR_NO_MEMORY_V01 )
      {
        QCRIL_LOG_INFO("no memory in modem EFS");
        g_pdc_info.is_under_processing = 0;
        payload = QCRIL_PDC_NO_MEMORY;
        qcril_hook_unsol_response( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_EVT_HOOK_UNSOL_PDC_CONFIG,
                                      (char *)(&payload), sizeof(payload) );
      }
      else
      {
        QCRIL_LOG_INFO("Failed to load configuration");
        g_pdc_info.is_under_processing = 0;
        payload = QCRIL_PDC_GENERIC_FAIL;
        qcril_hook_unsol_response( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_EVT_HOOK_UNSOL_PDC_CONFIG,
                                      (char *)(&payload), sizeof(payload) );
      }
    } // there is some error

    QCRIL_LOG_INFO("load completed %d", load_completed);
    if ( load_completed )
    {
      reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                       QCRIL_DEFAULT_MODEM_ID,
                                       QCRIL_EVT_REQUEST_MBN_HW_LOAD_CONFIG,
                                       &req_info );
      if (reqlist_found == E_SUCCESS)
      {
          qcril_mbn_hw_send_load_config_resp(load_ind->error);
          g_pdc_info.is_under_processing = 0;
      }
      else
      {
          reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                           QCRIL_DEFAULT_MODEM_ID,
                                           QCRIL_EVT_REQUEST_MBN_SW_LOAD_CONFIG,
                                           &req_info );
          if (reqlist_found == E_SUCCESS)
          {
              g_pdc_info.is_under_processing = 0;
              // ignore the error where config is already loaded for sw_mbn_update
              if (load_ind->error == QMI_ERR_INVALID_ID_V01 )
              {
                qcril_mbn_sw_send_load_config_resp(QMI_ERR_NONE_V01);
              }
              else
              {
                qcril_mbn_sw_send_load_config_resp(load_ind->error);
              }
          }
          else
          {
              // get first sub that needs to select
              g_pdc_info.cur_select_sub = 0;
              while ( g_pdc_info.cur_select_sub < g_pdc_info.sub_count )
              {
                if ( (1 << g_pdc_info.cur_select_sub) & g_pdc_info.sub_select_mask )
                  break;
                g_pdc_info.cur_select_sub++;
              }
              if ( g_pdc_info.cur_select_sub >= g_pdc_info.sub_count )
              {
                // no sub needs to select
                // all done, sent unsol msg to ATEL
                g_pdc_info.is_under_processing = 0;
                g_pdc_info.cur_select_sub = 0;
                payload = QCRIL_PDC_NO_ERROR;
                qcril_hook_unsol_response( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_EVT_HOOK_UNSOL_PDC_CONFIG,
                                                (char *)(&payload), sizeof(payload) );
              }
              else
              {

                //TODO remove event thread
                // kick the select config start
                qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                              QCRIL_DEFAULT_MODEM_ID,
                              QCRIL_DATA_ON_STACK,
                              QCRIL_EVT_QMI_RIL_PDC_SELECT_CONFIGURATION,
                              &g_pdc_info.pdc_config_info.config_type,
                              sizeof(g_pdc_info.pdc_config_info.config_type),
                              (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
              }
          }
      }
    }
  } //ind_data_ptr != NULL
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_select_config_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PDC_SET_SELECTED_CONFIG_IND_V01

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_select_config_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
  pdc_set_selected_config_ind_msg_v01 *set_ind;
  qcril_reqlist_public_type req_info;
  errno_enum_type           reqlist_found;
  char payload;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ind_data_len);

  payload = QCRIL_PDC_GENERIC_FAIL;
  if ( ind_data_ptr != NULL )
  {
    set_ind = (pdc_set_selected_config_ind_msg_v01*)ind_data_ptr;
    if ( set_ind->error == QMI_ERR_NONE_V01 )
    {
      QCRIL_LOG_INFO("select successful for SUB:%d", g_pdc_info.cur_select_sub);
      payload = QCRIL_PDC_NO_ERROR;
    }
    else // there is some error
    {
      QCRIL_LOG_ERROR("select indication error for SUB:%d", g_pdc_info.cur_select_sub);
      payload = QCRIL_PDC_SELECT_FAIL;
    }
  }
  else
  {
    QCRIL_LOG_ERROR("select indication param NULL");
    payload = QCRIL_PDC_GENERIC_FAIL;
  }

  reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                   QCRIL_DEFAULT_MODEM_ID,
                                   QCRIL_EVT_REQUEST_MBN_HW_SELECT_CONFIG,
                                   &req_info );
  if (reqlist_found == E_SUCCESS)
  {
      qcril_mbn_hw_send_select_config_resp(static_cast<RIL_Errno>(payload));
      g_pdc_info.is_under_processing = 0;
  }
  else
  {
      reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                       QCRIL_DEFAULT_MODEM_ID,
                                       QCRIL_EVT_REQUEST_MBN_SW_SELECT_CONFIG,
                                       &req_info );
      if (reqlist_found == E_SUCCESS)
      {
          if( (payload == QCRIL_PDC_NO_ERROR) &&
              (g_pdc_info.config_id_len < PDC_CONFIG_ID_SIZE_MAX_V01) )
          {
              if(g_pdc_info.cur_select_sub < QCRIL_PDC_MAX_SUBS_SUPPORT)
              {
                //Store current active config info
                memset(g_pdc_info.active_config_id[g_pdc_info.cur_select_sub], 0,
                     sizeof(g_pdc_info.active_config_id[g_pdc_info.cur_select_sub]));
                memcpy(g_pdc_info.active_config_id[g_pdc_info.cur_select_sub],
                       (unsigned char *)g_pdc_info.config_id,
                       g_pdc_info.config_id_len);
                g_pdc_info.active_config_id_len[g_pdc_info.cur_select_sub]
                                                     = g_pdc_info.config_id_len;

                QCRIL_LOG_INFO("Selected config for SUB%d is %s",
                              g_pdc_info.cur_select_sub,
                              g_pdc_info.active_config_id[g_pdc_info.cur_select_sub]);
              }
              else
              {
                QCRIL_LOG_ERROR("Invalid select SUB ID-%d", g_pdc_info.cur_select_sub);
              }
          }

          qcril_mbn_sw_send_select_config_resp(static_cast<RIL_Errno>(payload));
          g_pdc_info.is_under_processing = 0;
      }
      else
      {
          // get next sub that needs to be selected
          while (g_pdc_info.cur_select_sub < g_pdc_info.sub_count )
          {
            g_pdc_info.cur_select_sub++;
            if ( (1 << g_pdc_info.cur_select_sub) & g_pdc_info.sub_select_mask )
              break;
          }

          if ( g_pdc_info.cur_select_sub >= g_pdc_info.sub_count )
          {
            // all done, sent unsol msg to ATEL
            g_pdc_info.is_under_processing = 0;
            g_pdc_info.cur_select_sub = 0;
            qcril_hook_unsol_response( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_EVT_HOOK_UNSOL_PDC_CONFIG,
                                                (char *)(&payload), sizeof(payload) );
          }
          else
          {
            //TODO remove event thread
            // continue to select config
            qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                          QCRIL_DEFAULT_MODEM_ID,
                          QCRIL_DATA_ON_STACK,
                          QCRIL_EVT_QMI_RIL_PDC_SELECT_CONFIGURATION,
                          &g_pdc_info.pdc_config_info.config_type,
                          sizeof(g_pdc_info.pdc_config_info.config_type),
                          (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
          }
      }
  }
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_activate_config_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PDC_ACTIVATE_CONFIG_IND_V01

    @return
    None.

    @note
    This function should not be entered in, since modem will perform SSR
    before activation indication comes.
*/
/*=========================================================================*/
void qcril_qmi_pdc_activate_config_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
  pdc_activate_config_ind_msg_v01 *act_ind;
  qcril_reqlist_public_type req_info;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  boolean is_more_act_ind_expected;
  errno_enum_type reqlist_found;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ind_data_len);

  if ( ind_data_ptr != NULL )
  {
    act_ind = (pdc_activate_config_ind_msg_v01*)ind_data_ptr;
    if ( act_ind->error == QMI_ERR_NONE_V01 )
    {
      QCRIL_LOG_INFO("activate successful");
      result = RIL_E_SUCCESS;
    }
    else
    {
      QCRIL_LOG_ERROR("activate error, qmi error num: %d", act_ind->error);
    }

    reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                     QCRIL_DEFAULT_MODEM_ID,
                                     QCRIL_EVT_REQUEST_MBN_HW_ACTIVATE_CONFIG,
                                     &req_info );
    if (reqlist_found == E_SUCCESS)
    {
        qcril_mbn_hw_send_activate_config_resp(result);
        g_pdc_info.is_under_processing = 0;
    }
    else
    {
        reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                         QCRIL_DEFAULT_MODEM_ID,
                                         QCRIL_EVT_REQUEST_MBN_SW_ACTIVATE_CONFIG,
                                         &req_info );
        if (reqlist_found == E_SUCCESS)
        {
            is_more_act_ind_expected = qcril_mbn_sw_send_activate_config_resp(result, TRUE);
            if (!is_more_act_ind_expected)
            {
                g_pdc_info.is_under_processing = 0;
                QCRIL_LOG_INFO("No more activation indication expectd");
            }
        }
    }

  }

  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_delete_config_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PDC_DELETE_CONFIG_IND_V01

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_delete_config_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
  pdc_delete_config_ind_msg_v01 *del_ind;
  qcril_reqlist_public_type req_info;
  RIL_Errno result = RIL_E_SUCCESS;
  errno_enum_type reqlist_found;
  QCRIL_NOTUSED(ind_data_len);
  char payload;

  QCRIL_LOG_FUNC_ENTRY();
  if ( ind_data_ptr != NULL )
  {
    del_ind = (pdc_delete_config_ind_msg_v01*)ind_data_ptr;
    if ( del_ind->error == QMI_ERR_NONE_V01 )
    {
      QCRIL_LOG_INFO("delete successfully");
    }
    else
    {
      QCRIL_LOG_ERROR("failed to delete, error id = %d", del_ind->error);
      result = RIL_E_GENERIC_FAILURE;
    }
  }
  else // ind_data_ptr = NULL
  {
    QCRIL_LOG_ERROR("NULL parameter");
    result = RIL_E_GENERIC_FAILURE;
  }

  g_pdc_info.is_under_processing = 0;
  if (g_pdc_info.is_under_cleanup)
  {
    g_pdc_info.is_under_cleanup = 0;
    payload = (result == RIL_E_GENERIC_FAILURE) ? QCRIL_PDC_GENERIC_FAIL : QCRIL_PDC_NO_ERROR;
    qcril_hook_unsol_response(QCRIL_DEFAULT_INSTANCE_ID,
    QCRIL_EVT_HOOK_UNSOL_PDC_CLEAR_CONFIGS, (char *)(&payload), sizeof(payload));
  }
  else
  {
    reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                       QCRIL_DEFAULT_MODEM_ID, QCRIL_EVT_REQUEST_MBN_HW_DELETE_CONFIG,
                                       &req_info );
    if (reqlist_found == E_SUCCESS)
    {
        qcril_mbn_hw_send_delete_config_resp(result);
    }
    else
    {
        reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                           QCRIL_DEFAULT_MODEM_ID,
                                           QCRIL_EVT_REQUEST_MBN_SW_DELETE_CONFIG,
                                           &req_info );
        if (reqlist_found == E_SUCCESS)
        {
            qcril_mbn_sw_send_delete_config_resp(result);
        }
        else
        {
            reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                            QCRIL_DEFAULT_MODEM_ID,
                                            QCRIL_EVT_REQUEST_MBN_SW_CLEANUP_CONFIG,
                                            &req_info);
            if (reqlist_found == E_SUCCESS)
            {
                qcril_mbn_sw_send_cleanup_config_resp(result);
            }
        }
    }
  }
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_get_config_info_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PDC_GET_CONFIG_INFO_IND_V01

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_get_config_info_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
  pdc_get_config_info_ind_msg_v01 *get_ind = NULL;
  qcril_reqlist_public_type req_info;
  qcril_request_resp_params_type resp;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  errno_enum_type reqlist_found;
  QCRIL_NOTUSED(ind_data_len);

  boolean has_config_desc = FALSE;
  int8_t config_desc[PDC_CONFIG_DESC_SIZE_MAX_V01];
  uint32_t config_desc_len = 0;
  boolean has_base_version = FALSE;
  boolean has_config_version = FALSE;
  uint32_t base_version = 0;
  uint32_t config_version = 0;

  QCRIL_LOG_FUNC_ENTRY();

  if ( ind_data_ptr != NULL )
  {
    get_ind = (pdc_get_config_info_ind_msg_v01*)ind_data_ptr;
    // Ignore the qmi error code. Sometimes config_desc is reported although
    // there is qmi error. (Will be fixed by modem MCFG team later)
    if ( ( get_ind->config_desc_valid == 1) && ( get_ind->config_desc_len > 0 )
            && ( get_ind->config_desc_len < PDC_CONFIG_DESC_SIZE_MAX_V01 ) )
    {
      has_config_desc = TRUE;
      config_desc_len = get_ind->config_desc_len;
      memcpy(config_desc, get_ind->config_desc, config_desc_len);

      PDC_CONFIG_LOCK();
      if (QCRIL_PDC_MBN_INFO_RETRIEVE_CONFIG_INFO == g_pdc_info.is_retrieving_mbn_info)
      {
          g_pdc_info.is_retrieving_mbn_info = QCRIL_PDC_MBN_INFO_NONE;

          PDC_CONFIG_UNLOCK();
          if (qcril_qmi_pdc_is_test_mbn(config_desc, config_desc_len))
        {
              if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_SUPPRESS_REQ))
              {
                qcril_request_suppress_request(RIL_REQUEST_SET_INITIAL_ATTACH_APN,
                                                 NULL);
                qcril_request_suppress_request(RIL_REQUEST_SETUP_DATA_CALL, NULL);
              }
        }
          else
          {
              qcril_request_clean_up_suppress_list();
          }
      }
      else
      {
          PDC_CONFIG_UNLOCK();
      }
    }

    if (get_ind->base_version_valid == 1)
    {
      has_base_version = TRUE;
      base_version = get_ind->base_version;
    }

    if (get_ind->config_version_valid == 1)
    {
      has_config_version = TRUE;
      config_version = get_ind->config_version;
    }
  }
  else // ind_data_ptr = NULL
  {
    QCRIL_LOG_ERROR("NULL parameter");
  }

  // send response for QCRIL_EVT_HOOK_GET_META_INFO if has
  reqlist_found = qcril_reqlist_query_by_event( QCRIL_DEFAULT_INSTANCE_ID,
                                                QCRIL_DEFAULT_MODEM_ID,
                                                QCRIL_EVT_REQUEST_MBN_SW_GET_CONFIG_INFO,
                                                &req_info );
  if ( reqlist_found == E_SUCCESS )
  {
      g_pdc_info.is_under_processing = 0;
      result = RIL_E_SUCCESS;
      qcril_mbn_sw_send_get_config_info_resp(result, has_base_version, base_version,
                                            has_config_version, config_version);
  }
  else
  {
      reqlist_found = qcril_reqlist_query_by_event( QCRIL_DEFAULT_INSTANCE_ID,
                                                    QCRIL_DEFAULT_MODEM_ID,
                                                    QCRIL_EVT_HOOK_GET_META_INFO,
                                                    &req_info );
      if ( reqlist_found == E_SUCCESS )
      {
        result = (has_config_desc) ? RIL_E_SUCCESS : RIL_E_GENERIC_FAILURE;
        qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                           req_info.t,
                                           req_info.request,
                                           result,
                                           &resp );
        if ( has_config_desc )
        {
          resp.resp_pkt = (void *) config_desc;
          resp.resp_len = config_desc_len;
        }
        qcril_send_request_response( &resp );
      }
      else
      {
        QCRIL_LOG_INFO("unable to find reqlist by event QCRIL_EVT_HOOK_GET_META_INFO");
      }
  }

#ifndef QMI_RIL_UTF
  // send response for QCRIL_EVT_HOOK_GET_QC_VERSION_OF_CONFIGID if has
  reqlist_found = qcril_reqlist_query_by_event( QCRIL_DEFAULT_INSTANCE_ID,
                                                QCRIL_DEFAULT_MODEM_ID,
                                                QCRIL_EVT_HOOK_GET_QC_VERSION_OF_CONFIGID,
                                                &req_info );
  if ( reqlist_found == E_SUCCESS )
  {
    result = (has_base_version) ? RIL_E_SUCCESS : RIL_E_GENERIC_FAILURE;
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                       req_info.t,
                                       req_info.request,
                                       result,
                                       &resp );
    if ( has_base_version )
    {
      resp.resp_pkt = (void *) (&base_version);
      resp.resp_len = sizeof(base_version);
    }
    qcril_send_request_response( &resp );
  }
  else
  {
    QCRIL_LOG_INFO("unable to find reqlist by event QCRIL_EVT_HOOK_GET_QC_VERSION_OF_CONFIGID");
  }

  // send response for QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_CONFIGID if has
  reqlist_found = qcril_reqlist_query_by_event( QCRIL_DEFAULT_INSTANCE_ID,
                                                QCRIL_DEFAULT_MODEM_ID,
                                                QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_CONFIGID,
                                                &req_info );
  if ( reqlist_found == E_SUCCESS )
  {
    result = (has_config_version) ? RIL_E_SUCCESS : RIL_E_GENERIC_FAILURE;
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                       req_info.t,
                                       req_info.request,
                                       result,
                                       &resp );
    if ( has_config_version )
    {
      resp.resp_pkt = (void *) (&config_version);
      resp.resp_len = sizeof(config_version);
    }
    qcril_send_request_response( &resp );
  }
  else
  {
    QCRIL_LOG_INFO("unable to find reqlist by event QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_CONFIGID");
  }
#endif

  g_pdc_info.is_under_processing = 0;

  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_list_configs_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PDC_LIST_CONFIGS_IND_V01

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_list_configs_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
  pdc_list_configs_ind_msg_v01 *list_ind = NULL;
  qcril_pdc_list_config_params list_params;
  RIL_Errno result = RIL_E_SUCCESS;
  char* result_array = NULL;
  char simple_payload;
  uint32_t i, index;
  QCRIL_NOTUSED(ind_data_len);

  QCRIL_LOG_FUNC_ENTRY();

  if ( ind_data_ptr != NULL )
  {
    list_ind = (pdc_list_configs_ind_msg_v01*)ind_data_ptr;
    if ( list_ind->error == QMI_ERR_NONE_V01 )
    {
      if ( (list_ind->config_list_valid) && (list_ind->config_list_len > 0)
            && (list_ind->config_list_len <= PDC_CONFIG_LIST_SIZE_MAX_V01) )
      {
        QCRIL_LOG_INFO("total configuration count %d", list_ind->config_list_len);
      }
      else
      {
        QCRIL_LOG_ERROR("no valid config lists available");
        result = RIL_E_GENERIC_FAILURE;
      }
    }
    else // QMI ERROR
    {
      QCRIL_LOG_ERROR("QMI error, error code %d", list_ind->error);
      result = RIL_E_GENERIC_FAILURE;
    }
  }
  else // ind_data_ptr = NULL
  {
    QCRIL_LOG_ERROR("NULL parameter");
    result = RIL_E_GENERIC_FAILURE;
  }

  if (result == RIL_E_SUCCESS)
  {
    result_array = (char *)qcril_malloc(QCRIL_PDC_ALL_CONFIGS_LEN);
    if ( !result_array )
    {
        result = RIL_E_GENERIC_FAILURE;
    }
    else
    {
        // The first byte indicates the result;
        result_array[0] = 0;
        // The second byte indicates if there is more config available
        // Set 0 initially
        result_array[1] = 0;
        // The second byte indicates the config list length
        result_array[2] = list_ind->config_list_len;
        // Fill the configuration list
        index = 3;
        for (i = 0; i < list_ind->config_list_len; i++)
        {
            result_array[index++] = list_ind->config_list[i].config_id_len;
            memcpy(result_array+index, list_ind->config_list[i].config_id,
                    list_ind->config_list[i].config_id_len);
            index += list_ind->config_list[i].config_id_len;
        }
    }
  }

  if (result == RIL_E_SUCCESS)
  {
    if (list_ind->more_available_valid && list_ind->ind_token_valid
            && (list_ind->more_available != 0))
    {
        // The second byte indicates if there is more config available
        result_array[1] = 1;
        // start the next list request
        list_params.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
        list_params.multi_support = 1;
        list_params.ind_token_valid = 1;
        list_params.ind_token = list_ind->ind_token;
        (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_QMI_RIL_PDC_LIST_CONFIGURATION,
                            (void*) &list_params,
                            sizeof(list_params),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);

    }
    else
    {
        g_pdc_info.is_under_processing = 0;
    }
    qcril_hook_unsol_response(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_EVT_HOOK_UNSOL_PDC_LIST_CONFIG,
                                    result_array, index);
    free(result_array);
  }
  else
  {
    g_pdc_info.is_under_processing = 0;
    simple_payload = QCRIL_PDC_GENERIC_FAIL;
    qcril_hook_unsol_response(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_EVT_HOOK_UNSOL_PDC_LIST_CONFIG,
                                    (char *)(&simple_payload), sizeof(simple_payload));
  }

}

#ifndef QMI_RIL_UTF
/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_validate_config_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PDC_VALIDATE_CONFIG_IND_V01

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_validate_config_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
  pdc_validate_config_ind_msg_v01 *valid_ind;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  QCRIL_NOTUSED(ind_data_len);

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if ( !g_pdc_info.is_under_processing )
    {
      QCRIL_LOG_ERROR("ignore this indication, since error happens");
      break;
    }

    if ( ind_data_ptr != NULL )
    {
      valid_ind = (pdc_validate_config_ind_msg_v01*)ind_data_ptr;
      if ( valid_ind->error == QMI_ERR_NONE_V01 )
      {
        QCRIL_LOG_INFO("Validate success");
        result = RIL_E_SUCCESS;
      }
      else // QMI ERROR
      {
        QCRIL_LOG_ERROR("QMI error code %d", valid_ind->error);
        break;
      }
    }
    else // ind_data_ptr = NULL
    {
      QCRIL_LOG_ERROR("NULL parameter");
      break;
    }

    if ( !valid_ind->frame_index_valid || !valid_ind->result_frame_valid
              || valid_ind->result_frame_len > PDC_CONFIG_FRAME_SIZE_MAX_V01)
    {
      result = RIL_E_GENERIC_FAILURE;
      QCRIL_LOG_ERROR("frame_index valid: %d, result_frame valid: %d, frame_len: %d",
            valid_ind->frame_index_valid, valid_ind->result_frame_valid,
            valid_ind->result_frame_len);
      break;
    }

    if ( write(g_pdc_info.dump_fd, valid_ind->result_frame, valid_ind->result_frame_len)
                    != (ssize_t)valid_ind->result_frame_len )
    {
      result = RIL_E_GENERIC_FAILURE;
      QCRIL_LOG_ERROR("failed to write the diff result to dump file: %s", strerror(errno));
      break;
    }

    if (QCRIL_QMI_PDC_IS_LAST_INDEX(valid_ind->frame_index))
    {
      QCRIL_LOG_INFO("validation result is fully dumped to file");
      g_pdc_info.is_under_processing = 0;
      close(g_pdc_info.dump_fd);
      qcril_hook_unsol_response( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_EVT_HOOK_UNSOL_PDC_VALIDATE_DUMPED,
                                        (char *)g_pdc_info.dump_file, strlen(g_pdc_info.dump_file));
      //TODO remove event thread
      qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                         QCRIL_DEFAULT_MODEM_ID,
                         QCRIL_DATA_ON_STACK,
                         QCRIL_EVT_QMI_RIL_PDC_PARSE_DIFF_RESULT,
                         (void*)g_pdc_info.dump_file,
                         strlen(g_pdc_info.dump_file),
                         (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
    }
  } while (0);

  if ( (result == RIL_E_GENERIC_FAILURE) && (g_pdc_info.is_under_processing) )
  {
    g_pdc_info.is_under_processing = 0;
    // send a message to ATEL, indicating the failure
    qcril_qmi_mbn_diff_send_unsol_msg(result, -1, NULL, NULL, NULL);
  }

  QCRIL_LOG_FUNC_RETURN();
}
#endif

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_deactivate_config_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PDC_DEACTIVATE_CONFIG_IND_V01

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_deactivate_config_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
  pdc_deactivate_config_ind_msg_v01 *deact_ind;
  qcril_reqlist_public_type req_info;
  qcril_request_resp_params_type resp;
  RIL_Errno result;
  errno_enum_type reqlist_found;
  QCRIL_NOTUSED(ind_data_len);

  QCRIL_LOG_FUNC_ENTRY();

  if ( ind_data_ptr != NULL )
  {
    deact_ind = (pdc_deactivate_config_ind_msg_v01*)ind_data_ptr;
    if ( deact_ind->error == QMI_ERR_NONE_V01 )
    {
      QCRIL_LOG_INFO("Deactivate success for SUB%d", g_pdc_info.cur_deact_sub);
      result = RIL_E_SUCCESS;
    }
    else // QMI ERROR
    {
      QCRIL_LOG_ERROR("QMI error for SUB%d, error code %d", g_pdc_info.cur_deact_sub, deact_ind->error);
      result = RIL_E_GENERIC_FAILURE;
    }
  }
  else // ind_data_ptr = NULL
  {
    QCRIL_LOG_ERROR("NULL parameter");
    result = RIL_E_GENERIC_FAILURE;
  }

  reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                   QCRIL_DEFAULT_MODEM_ID,
                                   QCRIL_EVT_REQUEST_MBN_HW_DEACTIVATE_CONFIG,
                                   &req_info );
  if (reqlist_found == E_SUCCESS)
  {
      g_pdc_info.is_under_processing = 0;
      qcril_mbn_hw_send_deactivate_config_resp(result);
  }
  else
  {
      reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                       QCRIL_DEFAULT_MODEM_ID,
                                       QCRIL_EVT_REQUEST_MBN_SW_DEACTIVATE_CONFIG,
                                       &req_info );
      if (reqlist_found == E_SUCCESS)
      {
          g_pdc_info.is_under_processing = 0;
          qcril_mbn_sw_send_deactivate_config_resp(result);
      }
      else
      {
          // if the there is the error, we still do the delete work anyway
          // no need to check the result
          g_pdc_info.cur_deact_sub++;
          if ( g_pdc_info.cur_deact_sub >= g_pdc_info.sub_count )
          {
            if (g_pdc_info.is_under_cleanup)
            {
              QCRIL_LOG_DEBUG("is_under_cleanup: %d", g_pdc_info.is_under_cleanup);
              // kick the delete all start
              qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                             QCRIL_DEFAULT_MODEM_ID,
                             QCRIL_DATA_ON_STACK,
                             QCRIL_EVT_QMI_RIL_PDC_DELETE_ALL,
                             NULL,
                             QMI_RIL_ZERO,
                             (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
            }
            else
            {
              // send response for deactivate
              reqlist_found = qcril_reqlist_query_by_event( QCRIL_DEFAULT_INSTANCE_ID,
                                                            QCRIL_DEFAULT_MODEM_ID,
                                                            QCRIL_EVT_HOOK_DEACTIVATE_CONFIGS,
                                                            &req_info );
              if ( reqlist_found == E_SUCCESS )
              {
                result = RIL_E_SUCCESS;
                qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                                   req_info.t,
                                                   req_info.request,
                                                   result,
                                                   &resp );
                qcril_send_request_response( &resp );
              }
              else
              {
                QCRIL_LOG_ERROR("unable to find reqlist by event QCRIL_EVT_HOOK_DEACTIVATE_CONFIGS");
              }
              g_pdc_info.is_under_processing = 0;
            }
          }
          else
          {
            //TODO remove event thread
            // continue to deactivate the next SUB
            qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            QCRIL_EVT_QMI_RIL_PDC_DEACTIVATE_CONFIGURATION,
                            &g_pdc_info.pdc_config_info.config_type,
                            sizeof(g_pdc_info.pdc_config_info.config_type),
                            (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
          }
      }
  }
  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================

  FUNCTION:  qcril_qmi_pdc_respond_to_get_selected_config_req

===========================================================================*/
/*!
    @brief
    Handle QMI_PDC_GET_SELECTED_CONFIG_IND_V01

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_respond_to_get_selected_config_req
(
  uint8_t    *config_id,
  uint32_t    config_id_len,
  uint8_t     pending_valid,
  RIL_Errno   result
)
{
    qcril_reqlist_public_type req_info;
    errno_enum_type           reqlist_found;
    qcril_request_resp_params_type resp;

    QCRIL_LOG_FUNC_ENTRY();
    // send response
    reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                              QCRIL_DEFAULT_MODEM_ID,
                                              QCRIL_EVT_HOOK_QUERY_MODEM_TEST_MODE,
                                              &req_info );
    if ( reqlist_found == E_SUCCESS )
    {
        qcril_default_request_resp_params(QCRIL_DEFAULT_INSTANCE_ID,
                                          req_info.t,
                                          req_info.request,
                                          result,
                                          &resp );
        resp.resp_pkt = (void *) config_id;
        resp.resp_len = config_id_len;
        qcril_send_request_response( &resp );
    }
    else
    {
        QCRIL_LOG_DEBUG("unable to find reqlist by event "
                        "QCRIL_EVT_HOOK_QUERY_MODEM_TEST_MODE");

        reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                 QCRIL_DEFAULT_MODEM_ID,
                                 QCRIL_EVT_REQUEST_MBN_HW_GET_SELECTED_CONFIG,
                                 &req_info );
        QCRIL_LOG_INFO("reqlist_found %d", reqlist_found);
        if (reqlist_found == E_SUCCESS)
        {
            g_pdc_info.is_under_processing = 0;
            qcril_mbn_hw_send_get_selected_resp(result, config_id, config_id_len);
        }
        else
        {
            reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                     QCRIL_DEFAULT_MODEM_ID,
                                     QCRIL_EVT_REQUEST_MBN_SW_GET_SELECTED_CONFIG,
                                     &req_info );
            if (reqlist_found == E_SUCCESS)
            {
                g_pdc_info.is_under_processing = 0;
                qcril_mbn_sw_send_get_selected_resp(result, config_id, config_id_len);
            }
            else
            {
                reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                     QCRIL_DEFAULT_MODEM_ID,
                                     QCRIL_EVT_REQUEST_MBN_SW_COUNT_PENDING_CONFIGS,
                                     &req_info );
                if (reqlist_found == E_SUCCESS)
                {
                    qcril_mbn_sw_send_check_pending_response(pending_valid);

                }
            }
        }
    }
    QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================

  FUNCTION:  qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook

===========================================================================*/
/*!
    @brief
    Acquire pdc ownership

    @return
    True if success, else failure
*/
/*=========================================================================*/
boolean qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook
(
    void
)
{
    boolean result = FALSE;
    //TODO call legacy mbn state api from mbnmodule
    if (qcril_mbn_hw_is_completed() && qcril_mbn_sw_is_completed())
    {
        result = TRUE;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}

/*=========================================================================

  FUNCTION:  qcril_qmi_pdc_set_feature_ind_hndlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PDC_SET_FEATURE_IND_V01

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_set_feature_ind_hndlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
  pdc_set_feature_ind_msg_v01 *set_feature_ind;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ind_data_len);

  if ( ind_data_ptr != NULL )
  {
    set_feature_ind = (pdc_set_feature_ind_msg_v01*)ind_data_ptr;
    if ( set_feature_ind->error == QMI_ERR_NONE_V01 ||
         set_feature_ind->error == QMI_ERR_NO_EFFECT_V01)
    {
        result = RIL_E_SUCCESS;
    }
  }
  QCRIL_LOG_DEBUG("result %d", result);

  qcril_mbn_sw_send_disable_modem_update_resp(result, true);
  g_pdc_info.is_under_processing = 0;

  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_get_selected_config_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PDC_GET_SELECTED_CONFIG_IND_V01

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_get_selected_config_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
  pdc_get_selected_config_ind_msg_v01 *sel_ind = NULL;
  uint8_t config_id[PDC_CONFIG_ID_SIZE_MAX_V01] = {0};
  uint32_t config_id_len = 0;
  RIL_Errno result = RIL_E_SUCCESS;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ind_data_len);

  if ( ind_data_ptr != NULL )
  {
    sel_ind = (pdc_get_selected_config_ind_msg_v01*)ind_data_ptr;
    if ( sel_ind->error == QMI_ERR_NONE_V01 )
    {
      if ( sel_ind->active_config_id_valid ) // selected and activated
      {
        config_id_len = sel_ind->active_config_id_len;
        if ( config_id_len >= PDC_CONFIG_ID_SIZE_MAX_V01 )
        {
          result = RIL_E_GENERIC_FAILURE;
        }
        else
        {
          memcpy(config_id, sel_ind->active_config_id, config_id_len);

          if(g_pdc_info.cur_select_sub < QCRIL_PDC_MAX_SUBS_SUPPORT)
          {
            //Store current active config info
            QCRIL_LOG_INFO("Store active config for SUB%d as %s",
                         g_pdc_info.cur_select_sub, sel_ind->active_config_id);

            memset(g_pdc_info.active_config_id[g_pdc_info.cur_select_sub], 0,
                 sizeof(g_pdc_info.active_config_id[g_pdc_info.cur_select_sub]));
            memcpy(g_pdc_info.active_config_id[g_pdc_info.cur_select_sub],
                   (unsigned char *)sel_ind->active_config_id,
                   config_id_len);
            g_pdc_info.active_config_id_len[g_pdc_info.cur_select_sub] =
                                                                config_id_len;

          }
          else
          {
            QCRIL_LOG_ERROR("Invalid select SUB ID-%d", g_pdc_info.cur_select_sub);
          }

          if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_SUPPRESS_REQ))
          {
              PDC_CONFIG_LOCK();
              if ( g_pdc_info.is_retrieving_mbn_info == QCRIL_PDC_MBN_INFO_RETRIEVE_ACTIVE_ID )
              {
                memcpy(g_pdc_info.config_id, sel_ind->active_config_id, config_id_len);
                g_pdc_info.config_id_len = sel_ind->active_config_id_len;
                PDC_CONFIG_UNLOCK();
                qcril_setup_timed_callback(QCRIL_DEFAULT_INSTANCE_ID,
                                             QCRIL_DEFAULT_MODEM_ID,
                                             qcril_qmi_pdc_retrieve_mbn_info_for_config_id,
                                             NULL, NULL );
              }
              else
              {
                PDC_CONFIG_UNLOCK();
              }
          }

          QCRIL_LOG_INFO("current active config id is %s", config_id);
        }
      }
      else if ( sel_ind->pending_config_id_valid ) // selected but not activated
      {
        config_id_len = sel_ind->pending_config_id_len;
        if ( config_id_len >= PDC_CONFIG_ID_SIZE_MAX_V01 )
        {
          result = RIL_E_GENERIC_FAILURE;
        }
        else
        {
          memcpy(config_id, sel_ind->pending_config_id, config_id_len);

          if(g_pdc_info.cur_select_sub < QCRIL_PDC_MAX_SUBS_SUPPORT)
          {
            //Store current active config info
            QCRIL_LOG_INFO("Store active config for SUB%d as %s",
                         g_pdc_info.cur_select_sub, sel_ind->pending_config_id);

            memset(g_pdc_info.active_config_id[g_pdc_info.cur_select_sub], 0,
                 sizeof(g_pdc_info.active_config_id[g_pdc_info.cur_select_sub]));
            memcpy(g_pdc_info.active_config_id[g_pdc_info.cur_select_sub],
                   (unsigned char *)sel_ind->pending_config_id,
                   config_id_len);
            g_pdc_info.active_config_id_len[g_pdc_info.cur_select_sub] =
                                                                config_id_len;
          }
          else
          {
            QCRIL_LOG_ERROR("Invalid select SUB ID-%d", g_pdc_info.cur_select_sub);
          }
        }
        QCRIL_LOG_INFO("current select config id is %s", config_id);
      }
      else
      {
        QCRIL_LOG_ERROR("invalid config id");
        result = RIL_E_GENERIC_FAILURE;
      }
    }
    else // QMI ERROR
    {
      QCRIL_LOG_ERROR("no selected config id");
      result = RIL_E_GENERIC_FAILURE;
    }
  }
  else // ind_data_ptr = NULL
  {
    QCRIL_LOG_ERROR("NULL parameter");
    result = RIL_E_GENERIC_FAILURE;
  }

  qcril_qmi_pdc_respond_to_get_selected_config_req(config_id, config_id_len,
                        (sel_ind) ? sel_ind->pending_config_id_valid : 0, result);
  g_pdc_info.is_under_processing = 0;

  QCRIL_LOG_FUNC_RETURN();
}


/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_list_configuration_event_hdlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_LIST_CONFIGURATION

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_list_configuration_event_hdlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr // Output parameter
)
{
    qcril_pdc_list_config_params *params = NULL;
    RIL_Errno result = RIL_E_GENERIC_FAILURE;
    pdc_list_configs_req_msg_v01   qmi_request;
    pdc_list_configs_resp_msg_v01  qmi_response;
    qmi_client_error_type          qmi_error;

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED(params_ptr);
    QCRIL_NOTUSED(ret_ptr);

    do
    {
        params = (qcril_pdc_list_config_params*)params_ptr->data;
        if (!params)
        {
            break;
        }
        memset(&qmi_request, 0, sizeof(qmi_request));
        qmi_request.config_type = params->config_type;
        qmi_request.config_type_valid = TRUE;
        qmi_request.multi_support = params->multi_support;
        qmi_request.multi_support_valid = TRUE;
        if ((qmi_request.ind_token_valid = params->ind_token_valid) != 0)
        {
            qmi_request.ind_token = params->ind_token;
        }

        g_pdc_info.is_under_processing = 1;

        qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_LIST_CONFIGS_REQ_V01,
                               &qmi_request,
                               sizeof(qmi_request),
                               &qmi_response,
                               sizeof(qmi_response));

        result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error,
                                                                  &qmi_response.resp);
        if (result != RIL_E_SUCCESS)
        {
            g_pdc_info.is_under_processing = 0;
        }
    } while (0);

    QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_select_configuration

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_SELECT_CONFIGURATION

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_select_configuration
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
)
{
  char payload;
  RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
  pdc_config_type_enum_v01 pdc_mbn_type;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(params_ptr);
  QCRIL_NOTUSED(ret_ptr);

  if (params_ptr && (params_ptr->data != NULL) && (params_ptr->datalen > 0))
  {
      pdc_mbn_type = (pdc_config_type_enum_v01) ((uint32_t*)params_ptr->data)[0];
  }
  else
  {
      pdc_mbn_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
  }

  QCRIL_LOG_DEBUG("pdc_mbn_type: %d", pdc_mbn_type);
  if (pdc_mbn_type == PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01)
  {
      ril_req_res = RIL_E_GENERIC_FAILURE;
  }
  else
  {
      ril_req_res = qcril_qmi_pdc_set_select_configuration(
                                            g_pdc_info.cur_select_sub,
                                            pdc_mbn_type,
                                            g_pdc_info.pdc_config_info.config_id,
                                            g_pdc_info.pdc_config_info.config_id_len);
  }

  // on failure, send an unsol message to indicate error
  // on success, do nothing here, the select indication handler
  // will handle the remaining things
  if ( ril_req_res != RIL_E_SUCCESS )
  {
    payload = QCRIL_PDC_GENERIC_FAIL;
    qcril_hook_unsol_response(QCRIL_DEFAULT_INSTANCE_ID,
                              QCRIL_EVT_HOOK_UNSOL_PDC_CONFIG,
                              (char *)(&payload), sizeof(payload) );
  }

  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_convert_to_qmi_pdc_type

===========================================================================*/
/*!
    @brief
    Convert mbn type to qmi pdc interface mbn type

    @return
    qmi pdc type.
*/
/*=========================================================================*/
pdc_config_type_enum_v01 qcril_qmi_pdc_convert_to_qmi_pdc_type
(
    qcril_pdc_mbn_type mbn_type
)
{
    pdc_config_type_enum_v01 pdc_mbn_type = PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01;
    switch (mbn_type)
    {
        case QCRIL_PDC_MBN_TYPE_SW:
        {
            pdc_mbn_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
            break;
        }

        case QCRIL_PDC_MBN_TYPE_HW:
        {
            pdc_mbn_type = PDC_CONFIG_TYPE_MODEM_PLATFORM_V01;
            break;
        }

        default:
        {
            break;
        }

    }

    return pdc_mbn_type;

}

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_activate_configuration

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_ACTIVATE_CONFIGURATION

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_activate_configuration
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
)
{
  RIL_Errno ril_req_res;
  char payload;
  qcril_pdc_mbn_type mbn_type;
  pdc_config_type_enum_v01 pdc_mbn_type;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  if (params_ptr && (params_ptr->data != NULL) && (params_ptr->datalen > 0))
  {
      mbn_type = static_cast<qcril_pdc_mbn_type>(((uint32_t*)params_ptr->data)[0]);
  }
  else
  {
      mbn_type = QCRIL_PDC_MBN_TYPE_SW;
  }

  pdc_mbn_type = qcril_qmi_pdc_convert_to_qmi_pdc_type(mbn_type);

  QCRIL_LOG_DEBUG("pdc_mbn_type: %d", pdc_mbn_type);
  if (pdc_mbn_type == PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01)
  {
      ril_req_res = RIL_E_GENERIC_FAILURE;
  }
  else
  {
      ril_req_res = qcril_qmi_pdc_activate_configuration_hdlr(0, pdc_mbn_type);
  }

  // on failure, send an unsol message to indicate error
  // on success, do nothing here, the activate indication handler
  // will handle the remaining things
  if ( ril_req_res != RIL_E_SUCCESS )
  {
    g_pdc_info.is_under_processing = 0;
    payload = QCRIL_PDC_GENERIC_FAIL;
    qcril_hook_unsol_response( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_EVT_HOOK_UNSOL_PDC_CONFIG,
                                    (char *)(&payload), sizeof(payload) );
  }

  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_mbn_pdc_delete_all_sw_configs

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_DELETE_ALL

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_pdc_delete_all_sw_configs
(
  QCRIL_UNUSED(const qcril_request_params_type *const params_ptr),
  QCRIL_UNUSED(qcril_request_return_type *const ret_ptr) // Output parameter
)
{
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type req_info;
  errno_enum_type reqlist_found;
  char payload;

  QCRIL_LOG_FUNC_ENTRY();

  // If no config_id specified, MCFG will delete all inactive MBNs
  result = qcril_qmi_pdc_delete_configuration_hdlr(NULL, 0,
                        PDC_CONFIG_TYPE_MODEM_SW_V01);

  if (result == RIL_E_GENERIC_FAILURE)
  {
      QCRIL_LOG_INFO("delete completed, with error %d", result);
      if (g_pdc_info.is_under_cleanup)
      {
          g_pdc_info.is_under_cleanup = 0;
          g_pdc_info.is_under_processing = 0;
          payload = QCRIL_PDC_GENERIC_FAIL;
          qcril_hook_unsol_response(QCRIL_DEFAULT_INSTANCE_ID,
              QCRIL_EVT_HOOK_UNSOL_PDC_CLEAR_CONFIGS, (char *)(&payload), sizeof(payload));
      }
      else
      {
          reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                        QCRIL_DEFAULT_MODEM_ID,
                                        QCRIL_EVT_REQUEST_MBN_SW_CLEANUP_CONFIG,
                                        &req_info);
          if (reqlist_found == E_SUCCESS)
          {
              qcril_mbn_sw_send_cleanup_config_resp(result);
          }
      }
  }
}

/*=========================================================================
  FUNCTION:  qcril_mbn_pdc_query_pending_sw_config

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_SW_QUERY_PENDING_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_pdc_query_pending_sw_config
(
  const qcril_request_params_type *const params_ptr,
  QCRIL_UNUSED(qcril_request_return_type *const ret_ptr) // Output parameter
)
{
  uint32_t sub_id;
  RIL_Errno result  = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_FUNC_ENTRY();

  if (params_ptr && (params_ptr->data != NULL) && (params_ptr->datalen > 0))
  {
      sub_id = ((uint32_t*)params_ptr->data)[0];
  }
  else
  {
      sub_id = 0;
  }
  QCRIL_LOG_DEBUG("checking pending config for sub_id: %u", sub_id);

  result = qcril_qmi_pdc_get_selected_mbn_config(sub_id, PDC_CONFIG_TYPE_MODEM_SW_V01);

  if (result == RIL_E_GENERIC_FAILURE)
  {
      // failure will be treated as no pendings on this sub
      qcril_mbn_sw_send_check_pending_response(0);
  }

  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pdc_deactivate_configuration

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_DEACTIVATE_CONFIGURATION

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_deactivate_configuration
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
)
{
  RIL_Errno result  = RIL_E_GENERIC_FAILURE;
  pdc_config_type_enum_v01 pdc_mbn_type;
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_FUNC_ENTRY();

  if (params_ptr && (params_ptr->data != NULL) && (params_ptr->datalen > 0))
  {
      pdc_mbn_type = (pdc_config_type_enum_v01) ((uint32_t*)params_ptr->data)[0];
  }
  else
  {
      pdc_mbn_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
  }

  QCRIL_LOG_DEBUG("pdc_mbn_type: %d", pdc_mbn_type);
  if (pdc_mbn_type == PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01)
  {
      result = RIL_E_GENERIC_FAILURE;
  }
  else
  {
      result = qcril_qmi_pdc_deactivate_configuration_hdlr(g_pdc_info.cur_deact_sub,
                                                           pdc_mbn_type);
  }

  QCRIL_LOG_FUNC_RETURN();
}


//===========================================================================
// QCRIL_EVT_HOOK_GET_AVAILABLE_CONFIGS
//===========================================================================
void qcril_qmi_pdc_get_available_configs
(
  const qcril_request_params_type *const params_ptr,
  QCRIL_UNUSED(qcril_request_return_type *const ret_ptr)
)
{
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  qcril_request_resp_params_type resp;
  qcril_pdc_list_config_params list_params;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
    {
      QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscrition");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }

    if ( g_pdc_info.is_under_processing )
    {
      QCRIL_LOG_ERROR("QMI PDC is busy");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    g_pdc_info.is_under_processing = 1;

    list_params.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
    list_params.multi_support = 1;
    list_params.ind_token_valid = 0;

    (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                        QCRIL_DEFAULT_MODEM_ID,
                        QCRIL_DATA_ON_STACK,
                        QCRIL_EVT_QMI_RIL_PDC_LIST_CONFIGURATION,
                        (void*) &list_params,
                        sizeof(list_params),
                        (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID);
    result = RIL_E_SUCCESS;
  } while (0);

  qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                     params_ptr->t,
                                     params_ptr->event_id,
                                     result,
                                     &resp );
  qcril_send_request_response( &resp );

  QCRIL_LOG_FUNC_RETURN();
}


//===========================================================================
//
// param: config_id
// TODO: need to block the call if the previous set is not complete
//===========================================================================
RIL_Errno qcril_qmi_pdc_load_config_file
(
    uint32_t sub_id,
    char    *file_path,
    char    *config_id,
    pdc_config_type_enum_v01 config_type
)
{
  RIL_Errno result = RIL_E_SUCCESS;
  struct stat f_stat;
  size_t str_size;
  char mbn_file_path[QCRIL_MBN_FILE_PATH_LEN];

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if ( g_pdc_info.is_under_processing )
    {
      QCRIL_LOG_ERROR("QMI PDC is busy");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    g_pdc_info.is_under_processing = 1;

    memset(&g_pdc_info.pdc_config_info, 0, sizeof(pdc_config_info_type));
    g_pdc_info.pdc_config_info.conf_fd = -1;
    if (file_path)
    {
        g_pdc_info.sub_select_mask              = sub_id;
        g_pdc_info.pdc_config_info.config_type  = config_type;
        QCRIL_LOG_INFO("sub_select_mask is %d", g_pdc_info.sub_select_mask);

        // fetch file name
        str_size = strlcpy( mbn_file_path, file_path, QCRIL_MBN_FILE_PATH_LEN);
        QCRIL_LOG_INFO("mbn file path is %s", mbn_file_path);
        if ( str_size > QCRIL_MBN_FILE_PATH_LEN )
        {
            QCRIL_LOG_ERROR("invalid parameter: file name length too long");
            result = RIL_E_GENERIC_FAILURE;
            break;
        }

        str_size = strlcpy((char *)g_pdc_info.pdc_config_info.config_id,
                            config_id, PDC_CONFIG_ID_SIZE_MAX_V01);
        QCRIL_LOG_INFO("config id is %s", g_pdc_info.pdc_config_info.config_id);
        if ( str_size > PDC_CONFIG_ID_SIZE_MAX_V01 )
        {
            QCRIL_LOG_ERROR("valid paramter: config_id length too long");
            result = RIL_E_GENERIC_FAILURE;
            break;
        }
        g_pdc_info.pdc_config_info.config_id_len = str_size;
    }
    else
    {
        QCRIL_LOG_ERROR("invalid parameter");
        result = RIL_E_GENERIC_FAILURE;
        break;
    }

    g_pdc_info.sub_count = QCRIL_PDC_MAX_SUBS_SUPPORT;
    // fill the file descripter and the config size
    g_pdc_info.pdc_config_info.conf_fd = open( mbn_file_path, O_RDONLY );
    if ( g_pdc_info.pdc_config_info.conf_fd == -1 )
    {
      QCRIL_LOG_ERROR("Failed to open file %s: %s", mbn_file_path, strerror(errno));
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    if ( fstat( g_pdc_info.pdc_config_info.conf_fd, &f_stat ) == -1 )
    {
      QCRIL_LOG_ERROR("Failed to fstat file: %s", strerror(errno));
      close(g_pdc_info.pdc_config_info.conf_fd);
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    g_pdc_info.pdc_config_info.conf_size = f_stat.st_size;
    QCRIL_LOG_DEBUG("total_conf_size %d", g_pdc_info.pdc_config_info.conf_size);


    //TODO remove event thread
    // kick load config start
    qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                    QCRIL_DEFAULT_MODEM_ID,
                    QCRIL_DATA_ON_STACK,
                    QCRIL_EVT_QMI_RIL_PDC_LOAD_CONFIGURATION,
                    NULL,
                    QMI_RIL_ZERO,
                    (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );

  } while (0);

  if ( result == RIL_E_GENERIC_FAILURE )
  {
    g_pdc_info.is_under_processing = 0;
  }

  return result;
}

//===========================================================================
//QCRIL_EVT_HOOK_SET_MODEM_TEST_MODE
// param: config_id
// TODO: need to block the call if the previous set is not complete
//===========================================================================
void qcril_qmi_pdc_set_modem_test_mode
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
  qcril_request_resp_params_type resp;
  char mbn_file_path[QCRIL_MBN_FILE_PATH_LEN];
  RIL_Errno result = RIL_E_SUCCESS;
  size_t str_size;
  char* str_pos;
  qcril_pdc_mbn_type mbn_type = QCRIL_PDC_MBN_TYPE_SW;
  pdc_config_type_enum_v01 pdc_mbn_type;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  do
  {
    if (!qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook())
    {
        QCRIL_LOG_ERROR("no ownership");
        result = RIL_E_GENERIC_FAILURE;
        break;
    }

    if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
    {
      QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscrition");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }

    if ( params_ptr && (NULL != params_ptr->data) && (1 < params_ptr->datalen) )
    {
        QCRIL_LOG_INFO("data len is %d", params_ptr->datalen);

        mbn_type = *(qcril_pdc_mbn_type *)((uint8_t*)params_ptr->data + 1);
        pdc_mbn_type = qcril_qmi_pdc_convert_to_qmi_pdc_type(mbn_type);

        if (pdc_mbn_type == PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01)
        {
            result = RIL_E_GENERIC_FAILURE;
            break;
        }
        else
        {
            str_size = strlcpy( mbn_file_path, (const char*)(((char *)params_ptr->data) + 1 + sizeof(pdc_mbn_type)),
                                                    QCRIL_MBN_FILE_PATH_LEN);
            QCRIL_LOG_INFO("mbn file path is %s", mbn_file_path);
            if ( str_size > QCRIL_MBN_FILE_PATH_LEN )
            {
                QCRIL_LOG_ERROR("invalid parameter: file name length too long");
                result = RIL_E_GENERIC_FAILURE;
                break;
            }
            if ( (str_size + 1 + sizeof(pdc_mbn_type)) > params_ptr->datalen )
            {
                QCRIL_LOG_ERROR("invalid parameter: no config_id specified");
                result = RIL_E_GENERIC_FAILURE;
                break;
            }
        }
        // fetch config_id
        str_pos = ((char *)params_ptr->data) + 1 + sizeof(pdc_mbn_type) + str_size + 1;
        result = qcril_qmi_pdc_load_config_file(((uint8_t*)params_ptr->data)[0],
                                       mbn_file_path,
                                       str_pos,
                                       pdc_mbn_type);
    }
    else
    {
        QCRIL_LOG_ERROR("invalid parameter");
        result = RIL_E_GENERIC_FAILURE;
        break;
    }
  } while (0);

  if (params_ptr)
  {
      qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                         params_ptr->t,
                                         params_ptr->event_id,
                                         result,
                                         &resp );
      qcril_send_request_response( &resp );
  }
  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
//QCRIL_EVT_HOOK_QUERY_MODEM_TEST_MODE
//===========================================================================
void qcril_qmi_pdc_query_modem_test_mode
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
  qcril_request_resp_params_type resp;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type qcril_req_info;
  uint32_t sub_info;
  qcril_pdc_mbn_type mbn_type = QCRIL_PDC_MBN_TYPE_SW;
  pdc_config_type_enum_v01 pdc_mbn_type;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  do
  {
    if (!qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook())
    {
        QCRIL_LOG_ERROR("no ownership");
        break;
    }

    if (!(params_ptr && (params_ptr->data != NULL) && (params_ptr->datalen > 0)))
    {
        QCRIL_LOG_ERROR("Invalid params");
        break;
    }

    sub_info = ((uint8_t*)params_ptr->data)[0];
    mbn_type = *(qcril_pdc_mbn_type *)((uint8_t*)params_ptr->data + 1);
    pdc_mbn_type = qcril_qmi_pdc_convert_to_qmi_pdc_type(mbn_type);

    if (pdc_mbn_type == PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01)
    {
        result = RIL_E_GENERIC_FAILURE;
        break;
    }

    qcril_reqlist_default_entry( params_ptr->t,
                                 params_ptr->event_id,
                                 QCRIL_DEFAULT_MODEM_ID,
                                 QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                 QCRIL_EVT_HOOK_QUERY_MODEM_TEST_MODE,
                                 NULL,
                                 &qcril_req_info);
    if ( qcril_reqlist_new( QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info ) == E_SUCCESS )
    {
        result = qcril_qmi_pdc_get_selected_mbn_config(sub_info,
                                                     pdc_mbn_type);
    }
    else
    {
        QCRIL_LOG_ERROR("No memory to allocate reqlist");
    }
  } while (0);

  if ( result == RIL_E_GENERIC_FAILURE && params_ptr )
  {
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                    params_ptr->t,
                                    params_ptr->event_id,
                                    result,
                                    &resp );
    qcril_send_request_response( &resp );
  }

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_pdc_disable_modem_update
//===========================================================================
RIL_Errno qcril_qmi_pdc_disable_modem_update
(
   void
)
{
    pdc_set_feature_req_msg_v01 qmi_request;
    pdc_set_feature_resp_msg_v01  qmi_response;
    qmi_client_error_type qmi_error;
    RIL_Errno result = RIL_E_GENERIC_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
      if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
      {
        QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscrition");
        break;
      }

      if ( g_pdc_info.is_under_processing )
      {
        QCRIL_LOG_ERROR("QMI PDC is busy");
        break;
      }
      g_pdc_info.is_under_processing = 1;

      memset( &qmi_request, 0, sizeof(qmi_request) );
      qmi_request.slot_id = QCRIL_PDC_DEFAULT_SLOT_ID;
      qmi_request.selection_mode_valid = true;
      qmi_request.selection_mode = PDC_SELECTION_MODE_DISABLED_V01;
      qmi_request.flex_mapping_valid = true;
      qmi_request.flex_mapping = false;
      qmi_request.carrier_valid = true;
      qmi_request.carrier = PDC_SELECTION_OPEN_MARKET_V01;
      qmi_request.refresh_mode_valid = true;
      qmi_request.refresh_mode = PDC_REFRESH_MODE_ENABLED_V01;

      qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_SET_FEATURE_REQ_V01,
                                           &qmi_request, sizeof( qmi_request ),
                                           &qmi_response, sizeof( qmi_response ));

      result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
                                          qmi_error,
                                          &qmi_response.resp );
    } while (0);

    QCRIL_LOG_INFO("result %d", result);
    if ( result == RIL_E_GENERIC_FAILURE )
    {
      g_pdc_info.is_under_processing = 0;
    }

    QCRIL_LOG_FUNC_RETURN();

    return result;
}

//===========================================================================
// qcril_qmi_pdc_get_selected_mbn_config
//===========================================================================
RIL_Errno qcril_qmi_pdc_get_selected_mbn_config
(
    int                      sub_id,
    pdc_config_type_enum_v01 config_type
)
{
  pdc_get_selected_config_req_msg_v01 qmi_request;
  pdc_get_selected_config_resp_msg_v01 qmi_response;
  qmi_client_error_type qmi_error;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  uint32_t sub_info;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
    {
      QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscrition");
      break;
    }

    if ( g_pdc_info.is_under_processing )
    {
      QCRIL_LOG_ERROR("QMI PDC is busy");
      break;
    }
    g_pdc_info.is_under_processing = 1;

    sub_info = sub_id;
    g_pdc_info.cur_select_sub = sub_id;
    if ( sub_info > QCRIL_PDC_MAX_SUBS_SUPPORT )
    {
      QCRIL_LOG_ERROR("sub index too large");
      break;
    }

    memset( &qmi_request, 0, sizeof(qmi_request) );
    qmi_request.config_type = config_type;

    if (config_type != PDC_CONFIG_TYPE_MODEM_PLATFORM_V01)
    {
        qmi_request.subscription_id_valid = 1;
        qmi_request.subscription_id = sub_info;
    }

    qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_GET_SELECTED_CONFIG_REQ_V01,
                                        &qmi_request, sizeof( qmi_request ),
                                        &qmi_response, sizeof( qmi_response ));

    result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_error, &qmi_response.resp );
  } while (0);

  QCRIL_LOG_INFO("result %d", result);
  if ( result != RIL_E_SUCCESS )
  {
    g_pdc_info.is_under_processing = 0;
  }

  QCRIL_LOG_FUNC_RETURN();

  return result;
}

//===========================================================================
// QCRIL_EVT_HOOK_ACTIVATE_CONFIGS
//===========================================================================
void qcril_qmi_pdc_activate_configs
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  qcril_request_resp_params_type resp;
  uint32_t sub_info = 0;
  qcril_pdc_mbn_type mbn_type;
  pdc_config_type_enum_v01 pdc_mbn_type;
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if (!qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook())
    {
        QCRIL_LOG_ERROR("no ownership");
        break;
    }

    // base check
    if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
    {
      QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscrition");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    if ( g_pdc_info.is_under_processing )
    {
      QCRIL_LOG_ERROR("QMI PDC is busy");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    g_pdc_info.is_under_processing = 1;

    if (params_ptr && (params_ptr->data != NULL) && (params_ptr->datalen > 0))
    {
        sub_info = ((uint8_t*)params_ptr->data)[0];
        mbn_type = *(qcril_pdc_mbn_type *)((uint8_t*)params_ptr->data + 1);
    }
    else
    {
        sub_info = 0;
        mbn_type = QCRIL_PDC_MBN_TYPE_SW;
    }

    pdc_mbn_type = qcril_qmi_pdc_convert_to_qmi_pdc_type(mbn_type);

    if (pdc_mbn_type == PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01)
    {
        result = RIL_E_GENERIC_FAILURE;
    }
    else
    {
        result = qcril_qmi_pdc_activate_configuration_hdlr(0, pdc_mbn_type);
    }

  } while (0);

  g_pdc_info.is_under_processing = 0;
  if(params_ptr)
  {
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                       params_ptr->t,
                                       params_ptr->event_id,
                                       result,
                                       &resp );
    qcril_send_request_response( &resp );
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(result);
}

//===========================================================================
// QCRIL_EVT_HOOK_DEACTIVATE_CONFIGS
//===========================================================================
void qcril_qmi_pdc_deactivate_configs
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
  RIL_Errno result;
  qcril_request_resp_params_type resp;
  qcril_reqlist_public_type qcril_req_info;
  qcril_pdc_mbn_type mbn_type = QCRIL_PDC_MBN_TYPE_SW;
  pdc_config_type_enum_v01 pdc_mbn_type;
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_FUNC_ENTRY();

  if(!params_ptr)
  {
      QCRIL_LOG_ERROR("params_ptr is NULL");
      result = RIL_E_GENERIC_FAILURE;
  }
  else if (!qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook())
  {
      QCRIL_LOG_ERROR("no ownership");
      result = RIL_E_GENERIC_FAILURE;
  }
  else if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
  {
    QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscrition");
    result = RIL_E_GENERIC_FAILURE;
  }
  else if ( g_pdc_info.is_under_processing )
  {
    QCRIL_LOG_ERROR("QMI PDC is busy");
    result = RIL_E_GENERIC_FAILURE;
  }
  else
  {
    g_pdc_info.is_under_processing = 1;
    // FIXME: hard code sub_count as 3 when deactivate
    // 1. when deactivate all subs, by default we deactivate for SUB0, SUB1, SUB2,
    // in case we miss any. Because if one SUB still has active configuration, we
    // will fail to delete all configurations.
    // 2. And If any SUB is not supported during deactivate or there is no active
    // config on it, MCFG will report error in QMI deactivate indication. And RIL
    // will ignore this error.
    g_pdc_info.sub_count = QCRIL_PDC_MAX_SUBS_SUPPORT;
    g_pdc_info.cur_deact_sub = 0;
    QCRIL_LOG_INFO("The current maximum subscriptions is %d", g_pdc_info.sub_count);

    if ((params_ptr->data != NULL) && (params_ptr->datalen > 0))
    {
        mbn_type = static_cast<qcril_pdc_mbn_type>(((uint32_t*)params_ptr->data)[0]);
    }
    else
    {
        mbn_type = QCRIL_PDC_MBN_TYPE_SW;
    }

    pdc_mbn_type = qcril_qmi_pdc_convert_to_qmi_pdc_type(mbn_type);
    if (pdc_mbn_type == PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01)
    {
        QCRIL_LOG_ERROR("Invalid mbn type");
        result = RIL_E_GENERIC_FAILURE;
    }
    else
    {
      qcril_reqlist_default_entry( params_ptr->t,
                                   params_ptr->event_id,
                                   QCRIL_DEFAULT_MODEM_ID,
                                   QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                   QCRIL_EVT_HOOK_DEACTIVATE_CONFIGS,
                                   NULL,
                                   &qcril_req_info);
      if ( qcril_reqlist_new( QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info ) == E_SUCCESS )
      {
        qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                        QCRIL_DEFAULT_MODEM_ID,
                        QCRIL_DATA_ON_STACK,
                        QCRIL_EVT_QMI_RIL_PDC_DEACTIVATE_CONFIGURATION,
                        &pdc_mbn_type,
                        sizeof(pdc_mbn_type),
                        (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
        result = RIL_E_SUCCESS;
      }
      else
      {
        QCRIL_LOG_ERROR("No memory to allocate reqlist");
        result = RIL_E_GENERIC_FAILURE;
      }
    } // if (pdc_mbn_type != PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01)
  }

  if ( result == RIL_E_GENERIC_FAILURE && params_ptr )
  {
    g_pdc_info.is_under_processing = 0;
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                       params_ptr->t,
                                       params_ptr->event_id,
                                       result,
                                       &resp );
    qcril_send_request_response( &resp );
  }

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// QCRIL_EVT_HOOK_CLEANUP_LOADED_CONFIGS
//===========================================================================
void qcril_qmi_pdc_cleanup_loaded_configs
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  qcril_request_resp_params_type resp;
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_FUNC_ENTRY();

  if (!qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook())
  {
    QCRIL_LOG_ERROR("no ownership");
  }
  else if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
  {
    QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscrition");
  }
  else if ( g_pdc_info.is_under_processing )
  {
    QCRIL_LOG_ERROR("QMI PDC is busy");
  }
  else
  {
    g_pdc_info.is_under_processing = 1;
    g_pdc_info.is_under_cleanup = 1;
    // FIXME: hard code sub_count as 3 when deactivate
    // 1. when deactivate all subs, by default we deactivate for SUB0, SUB1, SUB2,
    // in case we miss any. Because if one SUB still has active configuration, we
    // will fail to delete all configurations.
    // 2. And If any SUB is not supported during deactivate or there is no active
    // config on it, MCFG will report error in QMI deactivate indication. And RIL
    // will ignore this error.
    g_pdc_info.sub_count = QCRIL_PDC_MAX_SUBS_SUPPORT;
    g_pdc_info.cur_deact_sub = 0;
    QCRIL_LOG_INFO("The current maximum subscriptions is %d", g_pdc_info.sub_count);

    //TODO remove event thread
    qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                    QCRIL_DEFAULT_MODEM_ID,
                    QCRIL_DATA_ON_STACK,
                    QCRIL_EVT_QMI_RIL_PDC_DEACTIVATE_CONFIGURATION,
                    NULL,
                    QMI_RIL_ZERO,
                    (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
    result = RIL_E_SUCCESS;
  }

  qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                     params_ptr->t,
                                     params_ptr->event_id,
                                     result,
                                     &resp );
  qcril_send_request_response( &resp );

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// QCRIL_EVT_HOOK_SEL_CONFIG
//===========================================================================
void qcril_qmi_pdc_select_configs
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
  RIL_Errno result = RIL_E_SUCCESS;
  qcril_request_resp_params_type resp;
  qcril_pdc_mbn_type mbn_type = QCRIL_PDC_MBN_TYPE_SW;
  pdc_config_type_enum_v01 pdc_mbn_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
  QCRIL_NOTUSED(ret_ptr);

#define QCRIL_PDC_SEL_CONFIG_CONFIG_ID_OFFSET (1 + sizeof(qcril_pdc_mbn_type))

  QCRIL_LOG_FUNC_ENTRY();
  do
  {
    if (!qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook())
    {
        QCRIL_LOG_ERROR("no ownership");
        break;
    }

    if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
    {
      QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscription");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }

    if ( g_pdc_info.is_under_processing )
    {
      QCRIL_LOG_ERROR("QMI PDC is busy");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    g_pdc_info.is_under_processing = 1;

    if ((NULL != params_ptr->data) && (1 < params_ptr->datalen))
    {
        if ((PDC_CONFIG_ID_SIZE_MAX_V01 + QCRIL_PDC_SEL_CONFIG_CONFIG_ID_OFFSET)
                                                    > params_ptr->datalen)
        {
          g_pdc_info.sub_select_mask = ((uint8_t*)params_ptr->data)[0];
          QCRIL_LOG_INFO("sub_select_mask is %d", g_pdc_info.sub_select_mask);

          mbn_type = *(qcril_pdc_mbn_type *)((uint8_t*)params_ptr->data + 1);
          memcpy(g_pdc_info.pdc_config_info.config_id,
                 (uint8_t*)params_ptr->data + QCRIL_PDC_SEL_CONFIG_CONFIG_ID_OFFSET,
                 params_ptr->datalen - 1  - sizeof(mbn_type));
          g_pdc_info.pdc_config_info.config_id_len = params_ptr->datalen - 1 - sizeof(mbn_type);
          pdc_mbn_type = qcril_qmi_pdc_convert_to_qmi_pdc_type(mbn_type);
        }
        else
        {
          QCRIL_LOG_ERROR("parameter error");
          result = RIL_E_GENERIC_FAILURE;
          break;
        }
    }
    else
    {
      QCRIL_LOG_ERROR("parameter error");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }

    // submask: each bit represents each SUB and the LSB is for SUB0
    //          value 1 means enable while value 0 means disable
    g_pdc_info.cur_select_sub = 0;
    g_pdc_info.sub_count = QCRIL_PDC_MAX_SUBS_SUPPORT;
    // if no SUB is selected, return directly
    if (!( g_pdc_info.sub_select_mask & ( (1 << g_pdc_info.sub_count) - 1) ))
    {
      QCRIL_LOG_ERROR("no selected subscription");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    // get first sub that needs to be selected
    while ( g_pdc_info.cur_select_sub < g_pdc_info.sub_count )
    {
      // found the first enabled SUB
      if ( (1 << g_pdc_info.cur_select_sub) & g_pdc_info.sub_select_mask )
        break;
      g_pdc_info.cur_select_sub++;
    }

    //TODO remove event thread
    // kick the select config start
    qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                  QCRIL_DEFAULT_MODEM_ID,
                  QCRIL_DATA_ON_STACK,
                  QCRIL_EVT_QMI_RIL_PDC_SELECT_CONFIGURATION,
                  &pdc_mbn_type,
                  sizeof(pdc_mbn_type),
                 (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
  } while (0);

  if ( result == RIL_E_GENERIC_FAILURE )
  {
    g_pdc_info.is_under_processing = 0;
  }
  qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                     params_ptr->t,
                                     params_ptr->event_id,
                                     result,
                                     &resp );
  qcril_send_request_response( &resp );
  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_pdc_get_config_info
//===========================================================================
RIL_Errno qcril_qmi_pdc_get_config_info
(
    char    *config_id,
    int      config_id_len,
    pdc_config_type_enum_v01 config_type
)
{
  pdc_get_config_info_req_msg_v01 qmi_request;
  pdc_get_config_info_resp_msg_v01 qmi_response;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  qmi_client_error_type qmi_error;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if ( g_pdc_info.is_under_processing )
    {
      QCRIL_LOG_ERROR("QMI PDC is busy");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    g_pdc_info.is_under_processing = 1;

    memset( &qmi_request, 0, sizeof(qmi_request) );
    qmi_request.new_config_info.config_type = config_type;
    if ((NULL != config_id) && (0 < config_id_len)
                  && (PDC_CONFIG_ID_SIZE_MAX_V01 > config_id_len))
    {
      qmi_request.new_config_info.config_id_len = config_id_len;
      memcpy( qmi_request.new_config_info.config_id,
                  config_id, config_id_len);
    }
    else
    {
      QCRIL_LOG_ERROR("invalid parameter for config_id");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }

    qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_GET_CONFIG_INFO_REQ_V01,
                                        &qmi_request, sizeof( qmi_request ),
                                        &qmi_response, sizeof( qmi_response ));

    result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
                                            qmi_error, &qmi_response.resp );
  } while (0);

  if ( result == RIL_E_GENERIC_FAILURE )
  {
    g_pdc_info.is_under_processing = 0;
  }

  QCRIL_LOG_FUNC_RETURN();

  return result;

}

//===========================================================================
// QCRIL_EVT_HOOK_GET_META_INFO
//===========================================================================
void qcril_qmi_pdc_get_meta_info_of_config
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type qcril_req_info;
  qcril_request_resp_params_type resp;
  qcril_pdc_mbn_type mbn_type;
  pdc_config_type_enum_v01 pdc_mbn_type;
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if(!params_ptr)
    {
        QCRIL_LOG_ERROR("params_ptr is NULL");
        result = RIL_E_GENERIC_FAILURE;
        break;
    }

    if (!qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook())
    {
        QCRIL_LOG_ERROR("no ownership");
        result = RIL_E_GENERIC_FAILURE;
        break;
    }

    if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
    {
      QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscrition");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }

    if (!((NULL != params_ptr->data) && (0 < params_ptr->datalen)
                  && ((PDC_CONFIG_ID_SIZE_MAX_V01 + sizeof(mbn_type))
                                                > params_ptr->datalen)))
    {
        QCRIL_LOG_ERROR("invalid parameter for config_id");
        result = RIL_E_GENERIC_FAILURE;
        break;
    }

    qcril_reqlist_default_entry( params_ptr->t,
                                 params_ptr->event_id,
                                 QCRIL_DEFAULT_MODEM_ID,
                                 QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                 QCRIL_EVT_HOOK_GET_META_INFO,
                                 NULL,
                                 &qcril_req_info);
    if ( qcril_reqlist_new( QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info ) == E_SUCCESS )
    {
      if (params_ptr && (params_ptr->data != NULL) && (params_ptr->datalen > 0))
      {
          mbn_type = static_cast<qcril_pdc_mbn_type>(((uint32_t*)params_ptr->data)[0]);
      }
      else
      {
          mbn_type = QCRIL_PDC_MBN_TYPE_SW;
      }

      pdc_mbn_type = qcril_qmi_pdc_convert_to_qmi_pdc_type(mbn_type);

      if (pdc_mbn_type == PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01)
      {
          result = RIL_E_GENERIC_FAILURE;
      }
      else
      {
          result = qcril_qmi_pdc_get_config_info( ((char *)params_ptr->data) + sizeof(pdc_mbn_type),
                                                 params_ptr->datalen - sizeof(pdc_mbn_type),
                                                 pdc_mbn_type);
      }
    }
    else
    {
      QCRIL_LOG_ERROR("No memory to allocate reqlist");
    }
  } while (0);

  if ( result == RIL_E_GENERIC_FAILURE && params_ptr )
  {
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                    params_ptr->t,
                                    params_ptr->event_id,
                                    result,
                                    &resp );
    qcril_send_request_response( &resp );
  }

  QCRIL_LOG_FUNC_RETURN();
}

#ifndef QMI_RIL_UTF
//===========================================================================
// QCRIL_EVT_HOOK_GET_QC_VERSION_OF_CONFIGID
//===========================================================================
void qcril_qmi_pdc_get_qc_version_of_configid
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
  pdc_get_config_info_req_msg_v01 qmi_request;
  pdc_get_config_info_resp_msg_v01 qmi_response;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type qcril_req_info;
  qmi_client_error_type qmi_error;
  qcril_request_resp_params_type resp;
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if (!qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook())
    {
        QCRIL_LOG_ERROR("no ownership");
        result = RIL_E_GENERIC_FAILURE;
        break;
    }

    if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
    {
      QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscrition");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }

    if ( g_pdc_info.is_under_processing )
    {
      QCRIL_LOG_ERROR("QMI PDC is busy");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    g_pdc_info.is_under_processing = 1;

    memset( &qmi_request, 0, sizeof(qmi_request) );
    qmi_request.new_config_info.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
    if ( (NULL != params_ptr->data) && (0 < params_ptr->datalen)
                  && (PDC_CONFIG_ID_SIZE_MAX_V01 > params_ptr->datalen) )
    {
      qmi_request.new_config_info.config_id_len = params_ptr->datalen;
      memcpy( qmi_request.new_config_info.config_id,
                  params_ptr->data, params_ptr->datalen );
    }
    else
    {
      QCRIL_LOG_ERROR("invalid parameter for config_id");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }

    qcril_reqlist_default_entry( params_ptr->t,
                                 params_ptr->event_id,
                                 QCRIL_DEFAULT_MODEM_ID,
                                 QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                 QCRIL_EVT_HOOK_GET_QC_VERSION_OF_CONFIGID,
                                 NULL,
                                 &qcril_req_info);
    if ( qcril_reqlist_new( QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info ) == E_SUCCESS )
    {
      qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_GET_CONFIG_INFO_REQ_V01,
                                              &qmi_request, sizeof( qmi_request ),
                                              &qmi_response, sizeof( qmi_response ));

      result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_error, &qmi_response.resp );
    }
    else
    {
      QCRIL_LOG_ERROR("No memory to allocate reqlist");
    }
  } while (0);

  if ( result == RIL_E_GENERIC_FAILURE )
  {
    g_pdc_info.is_under_processing = 0;
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                    params_ptr->t,
                                    params_ptr->event_id,
                                    result,
                                    &resp );
    qcril_send_request_response( &resp );
  }

  QCRIL_LOG_FUNC_RETURN();
}
#endif

//===========================================================================
// QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_CONFIGID
//===========================================================================
void qcril_qmi_pdc_get_oem_version_of_configid
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
  pdc_get_config_info_req_msg_v01 qmi_request;
  pdc_get_config_info_resp_msg_v01 qmi_response;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type qcril_req_info;
  qmi_client_error_type qmi_error;
  qcril_request_resp_params_type resp;
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if (!qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook())
    {
        QCRIL_LOG_ERROR("no ownership");
        result = RIL_E_GENERIC_FAILURE;
        break;
    }

    if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
    {
      QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscrition");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }

    if ( g_pdc_info.is_under_processing )
    {
      QCRIL_LOG_ERROR("QMI PDC is busy");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    g_pdc_info.is_under_processing = 1;

    memset( &qmi_request, 0, sizeof(qmi_request) );
    qmi_request.new_config_info.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
    if ( (NULL != params_ptr->data) && (0 < params_ptr->datalen)
                  && (PDC_CONFIG_ID_SIZE_MAX_V01 > params_ptr->datalen) )
    {
      qmi_request.new_config_info.config_id_len = params_ptr->datalen;
      memcpy( qmi_request.new_config_info.config_id,
                  params_ptr->data, params_ptr->datalen );
    }
    else
    {
      QCRIL_LOG_ERROR("invalid parameter for config_id");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }

    qcril_reqlist_default_entry( params_ptr->t,
                                 params_ptr->event_id,
                                 QCRIL_DEFAULT_MODEM_ID,
                                 QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                 QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_CONFIGID,
                                 NULL,
                                 &qcril_req_info);
    if ( qcril_reqlist_new( QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info ) == E_SUCCESS )
    {
      qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_GET_CONFIG_INFO_REQ_V01,
                                              &qmi_request, sizeof( qmi_request ),
                                              &qmi_response, sizeof( qmi_response ));

      result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_error, &qmi_response.resp );
    }
    else
    {
      QCRIL_LOG_ERROR("No memory to allocate reqlist");
    }
  } while (0);

  if ( result == RIL_E_GENERIC_FAILURE )
  {
    g_pdc_info.is_under_processing = 0;
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                    params_ptr->t,
                                    params_ptr->event_id,
                                    result,
                                    &resp );
    qcril_send_request_response( &resp );
  }

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// QCRIL_EVT_HOOK_VALIDATE_CONFIG
//===========================================================================
void qcril_qmi_pdc_validate_config
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
  pdc_validate_config_req_msg_v01 qmi_request;
  pdc_validate_config_resp_msg_v01 qmi_response;
  qmi_client_error_type qmi_error;
  qcril_request_resp_params_type resp;
  RIL_Errno result = RIL_E_GENERIC_FAILURE;
  size_t str_size;
  int len;
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_FUNC_ENTRY();
  do
  {
    g_pdc_info.dump_fd = -1;

    if (!qcril_qmi_pdc_acquire_pdc_ownership_for_oem_hook())
    {
        QCRIL_LOG_ERROR("no ownership");
        result = RIL_E_GENERIC_FAILURE;
        break;
    }

    if ( qmi_ril_get_process_instance_id() != QCRIL_DEFAULT_INSTANCE_ID )
    {
      QCRIL_LOG_ERROR("QMI PDC client is only available on primary subscrition");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    if ( g_pdc_info.is_under_processing )
    {
      QCRIL_LOG_ERROR("QMI PDC is busy");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }
    g_pdc_info.is_under_processing = 1;

    // fill qmi request structure & check parameter
    memset( &qmi_request, 0, sizeof(qmi_request) );
    qmi_request.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
    if ( (NULL != params_ptr->data) && (1 < params_ptr->datalen) )
    {
      QCRIL_LOG_INFO("data len is %d", params_ptr->datalen);
      // fetch sub_id
      qmi_request.subscription_id = ((uint8_t*)params_ptr->data)[0];
      qmi_request.subscription_id_valid = 1;
      QCRIL_LOG_INFO("sub_id is %d", qmi_request.subscription_id);
      // fetch config_id
      str_size = strlcpy( (char *)qmi_request.config_id,
            (const char*)(((char *)params_ptr->data)+1), PDC_CONFIG_ID_SIZE_MAX_V01 );
      if ( str_size > PDC_CONFIG_ID_SIZE_MAX_V01 )
      {
        QCRIL_LOG_ERROR("invalid parameter: config_id too long");
        break;
      }
      qmi_request.config_id_valid = 1;
      qmi_request.config_id_len = strlen((char *)qmi_request.config_id);
      QCRIL_LOG_INFO("config id is %s, config_id_len is %d",
                      qmi_request.config_id, qmi_request.config_id_len);
    }
    else
    {
      QCRIL_LOG_ERROR("invalid parameter");
      result = RIL_E_GENERIC_FAILURE;
      break;
    }

    // create DIFF dump file
    len = snprintf(g_pdc_info.dump_file, QCRIL_DUMP_FILE_PATH_LEN,
            QCRIL_DUMP_FILE_PREFIX"%d.xml", qmi_request.subscription_id);
    if ( len == QCRIL_DUMP_FILE_PATH_LEN )
    {
      QCRIL_LOG_ERROR("unexpected: dump length exceed");
      break;
    }
    // permission should be "radio:radio -rw-r--r--"
    g_pdc_info.dump_fd = open(g_pdc_info.dump_file, O_WRONLY | O_CREAT | O_TRUNC,
                                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if ( g_pdc_info.dump_fd == -1 )
    {
      QCRIL_LOG_ERROR("failed to create dump file: %s, %s",
                                    g_pdc_info.dump_file, strerror(errno));
      break;
    }

    // send validate request to MCFG
    qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_VALIDATE_CONFIG_REQ_V01,
                                            &qmi_request, sizeof( qmi_request ),
                                            &qmi_response, sizeof( qmi_response ));

    result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_error, &qmi_response.resp);
  } while (0);

  if ( result == RIL_E_GENERIC_FAILURE )
  {
    g_pdc_info.is_under_processing = 0;
    if ( g_pdc_info.dump_fd != -1 )
      close(g_pdc_info.dump_fd);
  }
  qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                     params_ptr->t,
                                     params_ptr->event_id,
                                     result,
                                     &resp );
  qcril_send_request_response( &resp );
  QCRIL_LOG_FUNC_RETURN();

}

/*=========================================================================

  FUNCTION: qcril_qmi_pdc_unsol_ind_cb_hndlr

===========================================================================*/
/*!
    @brief
    Callback for QMI PDC indications

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pdc_unsol_ind_cb_hndlr
(
    unsigned int   msg_id,
    unsigned char * decoded_payload,
    uint32_t decoded_payload_len
)
{
  QCRIL_LOG_FUNC_ENTRY();
  do {
      switch(msg_id)
      {
        case QMI_PDC_LOAD_CONFIG_IND_V01:
            // TODO: add load configure indication process
            qcril_qmi_pdc_load_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;

        case QMI_PDC_SET_SELECTED_CONFIG_IND_V01:
            qcril_qmi_pdc_select_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;

        case QMI_PDC_ACTIVATE_CONFIG_IND_V01:
            qcril_qmi_pdc_activate_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;

        case QMI_PDC_SET_FEATURE_IND_V01:
            qcril_qmi_pdc_set_feature_ind_hndlr(decoded_payload, decoded_payload_len);
        break;

        case QMI_PDC_GET_SELECTED_CONFIG_IND_V01:
            qcril_qmi_pdc_get_selected_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;

        case QMI_PDC_DEACTIVATE_CONFIG_IND_V01:
            qcril_qmi_pdc_deactivate_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;

        case QMI_PDC_LIST_CONFIGS_IND_V01:
            qcril_qmi_pdc_list_configs_ind_hdlr(decoded_payload, decoded_payload_len);
        break;

        case QMI_PDC_DELETE_CONFIG_IND_V01:
            qcril_qmi_pdc_delete_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;

        case QMI_PDC_GET_CONFIG_INFO_IND_V01:
            qcril_qmi_pdc_get_config_info_ind_hdlr(decoded_payload, decoded_payload_len);
        break;

#ifndef QMI_RIL_UTF
        case QMI_PDC_VALIDATE_CONFIG_IND_V01:
            qcril_qmi_pdc_validate_config_ind_hdlr(decoded_payload, decoded_payload_len);
        break;
#endif

        default:
            QCRIL_LOG_INFO("Unsupported QMI PDC indication %x hex", msg_id);
        break;
    }
  } while (0);

  QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION:  qcril_qmi_pdc_init

===========================================================================*/
/*!
    @brief
    Initialize the PDC subsystem of the RIL.

    @return
    None.
*/
 /*=========================================================================*/

qmi_client_error_type qcril_qmi_pdc_init
(
  void
)
{
  qmi_client_error_type qmi_err = QMI_NO_ERR;
  char property_name[ PROPERTY_NAME_MAX ];
  int sub_id = 0;

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_SNPRINTF( property_name, sizeof( property_name ), "%s", QCRIL_MODEM_MBN_FILE_PATH );
  property_get( property_name, g_pdc_info.mbn_file_dir, QCRIL_MODEM_MBN_DEFAULT_PATH );
  /* TODO: check if the data path is valid */

  for(sub_id = 0; sub_id < QCRIL_PDC_MAX_SUBS_SUPPORT; sub_id++)
  {
    memset(g_pdc_info.active_config_id[sub_id], 0,
           sizeof(g_pdc_info.active_config_id[sub_id]));
    g_pdc_info.active_config_id_len[sub_id] = 0;
  }

  pthread_mutexattr_init( &g_pdc_info.pdc_config_mutex_attr );
  pthread_mutex_init( &g_pdc_info.pdc_config_mutex, &g_pdc_info.pdc_config_mutex_attr );

  QCRIL_LOG_FUNC_RETURN_WITH_RET(qmi_err);

  return (qmi_err);
}

/*===========================================================================

  FUNCTION:  qcril_qmi_pdc_set_sim_info

===========================================================================*/
/*!
    @brief
    Set pdc iccid, mcc, mnc

    @return
    None.
*/
 /*=========================================================================*/

void qcril_qmi_pdc_set_sim_info
(
    char *iccid,
    char *mcc,
    char *mnc
)
{
  char file_name[100];
  char final_string[100];
  char curr_string[101];

#ifndef QMI_RIL_UTF
  QCRIL_LOG_FUNC_ENTRY();

  int  instance_id = qmi_ril_get_process_instance_id();
  int  ret = 0;

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_SNPRINTF(file_name, sizeof(file_name),
                 "%s_%d", QCRIL_MBN_SIM_INFO_FILE, instance_id);

  ret = qcril_file_read_string_from_file(file_name, curr_string,
                                         sizeof(curr_string),
                                         sizeof(curr_string) - 1);
  QCRIL_LOG_DEBUG("file : %s", file_name);
  QCRIL_LOG_DEBUG("curr_string : %s", curr_string);

  QCRIL_SNPRINTF(final_string, sizeof(final_string),
                 "%s,%s,%s",
                 iccid? iccid: "null",
                 mcc? mcc: "null",
                 mnc? mnc: "null");
  QCRIL_LOG_DEBUG("final_string : %s", final_string);

  //Write to file only if there is a change in SIM info
  //or if the file is not created. Otherwise it will
  //unnecessarily trigger MBN loading
  if ((ret != 0) ||
      strcmp(curr_string, final_string))
  {
    qcril_file_write_a_string_to_file(file_name, final_string);
  }
  else
  {
    QCRIL_LOG_DEBUG("No change in sim info");
  }
#endif
}

//===========================================================================
// qcril_qmi_pdc_sim_info_update_handler
//===========================================================================
void qcril_qmi_pdc_sim_info_update_handler
(
    const qmi_ril_file_observer_change_event event
)
{
    if (QMI_RIL_FILE_OBSERVER_CHANGE_CLOSE_WRITE == event)
    {
        if (qcril_mbn_hw_is_completed())
        {
            //qcril_mbn_sw_update_init(FALSE);
            std::shared_ptr<MbnFileUpdateIndication> ind_msg_ptr =
                            std::make_shared<MbnFileUpdateIndication>();
            if (ind_msg_ptr != nullptr) {
                ind_msg_ptr->broadcast();
            }
        }
    }
}

/*===========================================================================

  FUNCTION:  qcril_qmi_pdc_pre_init

===========================================================================*/
/*!
    @brief
    Pre init pdc

    @return
    None.
*/
 /*=========================================================================*/
void qcril_qmi_pdc_pre_init
(
  void
)
{
    char file_name[100];
    int  i;
    int  max_apss_sub;
    struct stat st;

    memset(&st, 0x0, sizeof(struct stat));
    qcril_qmi_pdc_set_sim_info(NULL, NULL, NULL);

    qcril_qmi_mbn_hw_init();
    qcril_qmi_mbn_sw_init();
    max_apss_sub = qcril_qmi_dms_get_max_apss_subscritions();

    //Register for file change events only on primary RIL
    //as MBN loading will happen only from primary rild.
    if ( qmi_ril_get_process_instance_id() == QCRIL_DEFAULT_INSTANCE_ID )
    {
        QCRIL_LOG_DEBUG("Register sim info file change events");
        for (i = 0; i < max_apss_sub; i++)
        {
            QCRIL_SNPRINTF(file_name,
                           sizeof(file_name),
                           "%s_%d", QCRIL_MBN_SIM_INFO_FILE,
                           i);

            if (stat(QCRIL_MBN_SIM_INFO_DIR, &st) == -1)
            {
                QCRIL_LOG_ERROR("Directory %s doesn't exist", QCRIL_MBN_SIM_INFO_DIR);
            }
            else
            {
                if (stat(file_name, &st) == -1)
                {
                    //Create and close the file if it doesnt exist. Then
                    //register for the file change events for this file.

                    //this function will create a file if not exists and write string to it
                    char final_string[100];
                    QCRIL_SNPRINTF(final_string, sizeof(final_string),
                                   "%s,%s,%s", "null", "null", "null");
                    QCRIL_LOG_DEBUG("final_string : %s", final_string);
                    qcril_file_write_a_string_to_file(file_name, final_string);
                }

                qmi_ril_file_observer_register_for_change_event(
                                                file_name,
                                                (qmi_ril_file_observer_state_change_handler)qcril_qmi_pdc_sim_info_update_handler);
            }
        }
    }
}

/*===========================================================================

  FUNCTION: qcril_qmi_pdc_get_sim_info

===========================================================================*/
/*!
    @brief
    Get pdc iccid, mcc, mnc

    @return
    None.
*/
 /*=========================================================================*/

void qcril_qmi_pdc_get_sim_info
(
    int   instance_id,
    char *iccid,
    int   icc_id_len,
    char  *mcc,
    char  mcc_len,
    char  *mnc,
    char  mnc_len
)
{
  char file_name[100];
  char string[101];
  char *str = NULL;
  char *lastPtr = NULL;
  RIL_Errno ret = RIL_E_SUCCESS;

  QCRIL_LOG_FUNC_ENTRY();

  if (iccid && icc_id_len && mcc && mcc_len && mnc && mnc_len)
  {
    QCRIL_SNPRINTF(file_name,
                   sizeof(file_name),
                   "%s_%d", QCRIL_MBN_SIM_INFO_FILE,
                   instance_id);
    memset(string, 0, sizeof(string));
    memset(iccid, 0, icc_id_len);
    memset(mcc, 0, mcc_len);
    memset(mnc, 0, mnc_len);

    ret = (RIL_Errno) qcril_file_read_string_from_file(file_name, string, sizeof(string),
                                           sizeof(string) - 1);
    QCRIL_LOG_DEBUG("property : %s", file_name);
    QCRIL_LOG_DEBUG("string : %s", string);

    if (!ret)
    {
        str = strtok_r (string, ",", &lastPtr);

        if (str)
        {
            strlcpy(iccid, str, icc_id_len);
        }

        str = strtok_r (NULL, ",", &lastPtr);
        if (str)
        {
            strlcpy(mcc, str, mcc_len);
        }

        str = strtok_r (NULL, ",", &lastPtr);
        if (str)
        {
            strlcpy(mnc, str, mnc_len);
        }

        QCRIL_LOG_DEBUG("iccid: %s", iccid);
        QCRIL_LOG_DEBUG("mcc: %s", mcc);
        QCRIL_LOG_DEBUG("mnc: %s", mnc);
    }
    else
    {
        QCRIL_LOG_DEBUG("cannot open file %s", file_name);
    }
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
}


/*===========================================================================

  FUNCTION  qcril_qmi_pdc_retrieve_current_mbn_info

===========================================================================*/
/*!
    @brief
    retrieve active mbn info

    @return
    0 on success
*/
/*=========================================================================*/
void qcril_qmi_pdc_retrieve_current_mbn_info
(
    void
)
{
    pdc_get_selected_config_req_msg_v01  qmi_request;
    pdc_get_selected_config_resp_msg_v01 qmi_response;
    qmi_client_error_type                qmi_error;
    RIL_Errno          result = RIL_E_GENERIC_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();
    memset(&qmi_request, 0, sizeof(qmi_request));

    qmi_request.config_type           = PDC_CONFIG_TYPE_MODEM_SW_V01;
    qmi_request.subscription_id_valid = 1;
    qmi_request.subscription_id       = 0;
    PDC_CONFIG_LOCK();
    /* update state that retrieveing mbn info */
    g_pdc_info.is_retrieving_mbn_info = QCRIL_PDC_MBN_INFO_RETRIEVE_ACTIVE_ID;
    PDC_CONFIG_UNLOCK();
    qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_GET_SELECTED_CONFIG_REQ_V01,
                                            &qmi_request, sizeof( qmi_request ),
                                            &qmi_response, sizeof( qmi_response ));

    result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_error, &qmi_response.resp );
    if (result != RIL_E_SUCCESS)
    {
        PDC_CONFIG_LOCK();

        /* update state that retrieveing active mbn info */
        g_pdc_info.is_retrieving_mbn_info = QCRIL_PDC_MBN_INFO_NONE;
        qcril_request_clean_up_suppress_list();
        PDC_CONFIG_UNLOCK();
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET(result);
}

/*===========================================================================

  FUNCTION:  qcril_qmi_pdc_get_sub_id

===========================================================================*/
/*!
    @brief
    Get pdc sub_id

    @return
    None.
*/
 /*=========================================================================*/
int qcril_qmi_pdc_get_sub_id
(
    int instance_id
)
{
  char property_name[ PROPERTY_NAME_MAX ];
  int sub_id;

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_LOG_DEBUG("instance id %d", instance_id);
  QCRIL_SNPRINTF(property_name,
                 sizeof( property_name ),
                 "%s_%d", QCRIL_MBN_ACTIVATION_STATUS_PROP_NAME,
                 instance_id);

  qmi_ril_get_property_value_from_integer(property_name, &sub_id, 0);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(sub_id);
  return sub_id;
}
/*===========================================================================

  FUNCTION:  qcril_qmi_pdc_set_sub_id

===========================================================================*/
/*!
    @brief
    Initialize the PDC subsystem of the RIL.

    @return
    None.
*/
 /*=========================================================================*/
void qcril_qmi_pdc_set_sub_id
(
  int sub_id
)
{
  char property_name[ PROPERTY_NAME_MAX ];

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_LOG_DEBUG("sub_id %d", sub_id);
  QCRIL_SNPRINTF(property_name,
                 sizeof( property_name ),
                 "%s_%d", QCRIL_MBN_ACTIVATION_STATUS_PROP_NAME,
                 qmi_ril_get_process_instance_id());
  qmi_ril_set_property_value_to_integer(property_name, sub_id);

}

/*===========================================================================

  FUNCTION  qcril_qmi_pdc_retrieve_mbn_info_for_config_id

===========================================================================*/
/*!
    @brief
    retrieve mbn info for config id

    @return
    0 on success
*/
/*=========================================================================*/
void qcril_qmi_pdc_retrieve_mbn_info_for_config_id
(
    void *arg
)
{
    pdc_get_config_info_req_msg_v01      qmi_request;
    pdc_get_config_info_resp_msg_v01     qmi_response;
    qmi_client_error_type                qmi_error;
    RIL_Errno                      result = RIL_E_GENERIC_FAILURE;
    uint8_t                              config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
    uint32_t                             config_id_len;
    QCRIL_NOTUSED(arg);

    QCRIL_LOG_FUNC_ENTRY();
    memset(&qmi_request, 0, sizeof(qmi_request));
    qmi_request.new_config_info.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;

    PDC_CONFIG_LOCK();
    g_pdc_info.is_under_processing = 1;
    memcpy(config_id, g_pdc_info.config_id, g_pdc_info.config_id_len);
    config_id_len = g_pdc_info.config_id_len;
    PDC_CONFIG_UNLOCK();

    if ((0 < config_id_len) &&
         (config_id_len <= PDC_CONFIG_ID_SIZE_MAX_V01))
    {
      qmi_request.new_config_info.config_id_len = config_id_len;
      memcpy(qmi_request.new_config_info.config_id,
                  config_id, config_id_len);
      PDC_CONFIG_LOCK();
      g_pdc_info.is_retrieving_mbn_info = QCRIL_PDC_MBN_INFO_RETRIEVE_CONFIG_INFO;
      PDC_CONFIG_UNLOCK();
      qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_GET_CONFIG_INFO_REQ_V01,
                                              &qmi_request, sizeof( qmi_request ),
                                              &qmi_response, sizeof( qmi_response ));

      result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_error, &qmi_response.resp );
    }

    if (result != RIL_E_SUCCESS)
    {
        PDC_CONFIG_LOCK();

        g_pdc_info.is_under_processing = 0;
        /* update state that retrieveing active mbn info */
        g_pdc_info.is_retrieving_mbn_info = QCRIL_PDC_MBN_INFO_NONE;
        qcril_request_clean_up_suppress_list();
        PDC_CONFIG_UNLOCK();
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET(result);
}

/*===========================================================================

  FUNCTION  qcril_qmi_pdc_is_test_mbn

===========================================================================*/
/*!
    @brief
    check if the mbn is test mbn

    @return
    0 on success
*/
/*=========================================================================*/
boolean qcril_qmi_pdc_is_test_mbn(void *data, unsigned int len)
{
    boolean ret = FALSE;
    char text[PDC_CONFIG_DESC_SIZE_MAX_V01 + 1];
    do
    {
        if ((data == NULL) || (len == 0) || (len > PDC_CONFIG_DESC_SIZE_MAX_V01))
        {
            break;
        }

        memset (text, 0, PDC_CONFIG_DESC_SIZE_MAX_V01 + 1);
        memcpy (text, data, len);

        if (!strcasestr(text, QCRIL_PDC_COMMERCIAL_MBN_PREFIX))
        {
            /* This is test mbn */
            ret = TRUE;
        }

    } while (FALSE);
    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

/*=========================================================================

  FUNCTION:  qcril_qmi_pdc_deactivate_configuration_hdlr

===========================================================================*/
/*!
    @brief
    Deactivate configuration

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_pdc_deactivate_configuration_hdlr
(
    uint32_t                 sub_id,
    pdc_config_type_enum_v01 config_type
)
{
    pdc_deactivate_config_req_msg_v01  qmi_request;
    pdc_deactivate_config_resp_msg_v01 qmi_response;
    qmi_client_error_type              qmi_error;
    RIL_Errno                          result = RIL_E_GENERIC_FAILURE;
    errno_enum_type                    reqlist_found = E_FAILURE;
    qcril_reqlist_public_type          req_info;
    qcril_request_resp_params_type     resp;

    QCRIL_LOG_FUNC_ENTRY();

    memset(&qmi_request, 0, sizeof(qmi_request));
    qmi_request.config_type       = config_type;

    if (config_type != PDC_CONFIG_TYPE_MODEM_PLATFORM_V01)
    {
        qmi_request.subscription_id_valid = 1;
        qmi_request.subscription_id = sub_id;
    }

    g_pdc_info.is_under_processing = 1;
    g_pdc_info.cur_deact_sub = sub_id;
    qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_DEACTIVATE_CONFIG_REQ_V01,
                                        &qmi_request, sizeof( qmi_request ),
                                        &qmi_response, sizeof( qmi_response ));

    result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error,
                                                              &qmi_response.resp);

    g_pdc_info.pdc_config_info.config_type = config_type;
    if (result != RIL_E_SUCCESS)
    {
        QCRIL_LOG_DEBUG("Error in deactivating config. Type: %d, sub_id: %d",
                config_type, sub_id);

        reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                        QCRIL_DEFAULT_MODEM_ID,
                                        QCRIL_EVT_REQUEST_MBN_HW_DEACTIVATE_CONFIG,
                                        &req_info );
        if (reqlist_found == E_SUCCESS)
        {
            g_pdc_info.is_under_processing = 0;
            qcril_mbn_hw_send_deactivate_config_resp(result);
        }
        else
        {
            reqlist_found = qcril_reqlist_query_by_event(QCRIL_DEFAULT_INSTANCE_ID,
                                            QCRIL_DEFAULT_MODEM_ID,
                                            QCRIL_EVT_REQUEST_MBN_SW_DEACTIVATE_CONFIG,
                                            &req_info );
            if (reqlist_found == E_SUCCESS)
            {
                g_pdc_info.is_under_processing = 0;
                qcril_mbn_sw_send_deactivate_config_resp(result);
            }
            else
            {
                // if the there is the error, we still do the delete work anyway
                // no need to check the result
                g_pdc_info.cur_deact_sub++;
                if ( g_pdc_info.cur_deact_sub >= g_pdc_info.sub_count )
                {
                  if (g_pdc_info.is_under_cleanup)
                  {
                    QCRIL_LOG_DEBUG("is_under_cleanup: %d", g_pdc_info.is_under_cleanup);
                    // kick the delete all start
                    qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_DATA_ON_STACK,
                                    QCRIL_EVT_QMI_RIL_PDC_DELETE_ALL,
                                    NULL,
                                    QMI_RIL_ZERO,
                                    (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
                  }
                  else
                  {
                    // send response for deactivate
                    reqlist_found = qcril_reqlist_query_by_event( QCRIL_DEFAULT_INSTANCE_ID,
                                                                  QCRIL_DEFAULT_MODEM_ID,
                                                                  QCRIL_EVT_HOOK_DEACTIVATE_CONFIGS,
                                                                  &req_info );
                    QCRIL_LOG_DEBUG("reqlist_found: %d. req_info.t: %u", reqlist_found, req_info.t);
                    if ( reqlist_found == E_SUCCESS && req_info.t != (RIL_Token)QCRIL_MBN_INTERNAL_TOKEN_ID)
                    {
                      result = RIL_E_SUCCESS;
                      qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                                         req_info.t,
                                                         req_info.request,
                                                         result,
                                                         &resp );
                      qcril_send_request_response( &resp );
                    }
                    else
                    {
                      QCRIL_LOG_ERROR("unable to find reqlist by event QCRIL_EVT_HOOK_DEACTIVATE_CONFIGS");
                    }
                    g_pdc_info.is_under_processing = 0;
                  }
                }
                else
                {
                  // continue to deactivate the next SUB
                  qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                                  QCRIL_DEFAULT_MODEM_ID,
                                  QCRIL_DATA_ON_STACK,
                                  QCRIL_EVT_QMI_RIL_PDC_DEACTIVATE_CONFIGURATION,
                                  &g_pdc_info.pdc_config_info.config_type,
                                  sizeof(g_pdc_info.pdc_config_info.config_type),
                                  (RIL_Token) QCRIL_MBN_INTERNAL_TOKEN_ID );
                }
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return result;
}

/*=========================================================================

  FUNCTION:  qcril_qmi_pdc_activate_configuration_hdlr

===========================================================================*/
/*!
    @brief
    Activate configuration

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_pdc_activate_configuration_hdlr
(
    uint32_t                 sub_id,
    pdc_config_type_enum_v01 config_type
)
{
    pdc_activate_config_req_msg_v01  qmi_request;
    pdc_activate_config_resp_msg_v01 qmi_response;
    qmi_client_error_type            qmi_error;
    RIL_Errno                        result = RIL_E_GENERIC_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    memset(&qmi_request, 0, sizeof(qmi_request));
    qmi_request.config_type       = config_type;

    if (config_type != PDC_CONFIG_TYPE_MODEM_PLATFORM_V01)
    {
        qmi_request.subscription_id_valid = 1;
        qmi_request.subscription_id = sub_id;
    }

    g_pdc_info.is_under_processing = 1;
    qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_ACTIVATE_CONFIG_REQ_V01,
                                        &qmi_request, sizeof( qmi_request ),
                                        &qmi_response, sizeof( qmi_response ));

    result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error,
                                                              &qmi_response.resp);

    if (result != RIL_E_SUCCESS)
    {
        g_pdc_info.is_under_processing = 0;
    }

    QCRIL_LOG_FUNC_RETURN();
    return result;
}


/*=========================================================================

  FUNCTION:  qcril_qmi_pdc_delete_configuration_hdlr

===========================================================================*/
/*!
    @brief
    Delete configuration

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_pdc_delete_configuration_hdlr
(
    uint8_t                 *config_id,
    uint32_t                 config_id_len,
    pdc_config_type_enum_v01 config_type
)
{
    pdc_delete_config_req_msg_v01  qmi_request;
    pdc_delete_config_resp_msg_v01 qmi_response;
    qmi_client_error_type          qmi_error;
    RIL_Errno                      result = RIL_E_GENERIC_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    do {
        // treat it as error if config_id_len is too large
        if (config_id_len >= PDC_CONFIG_ID_SIZE_MAX_V01)
        {
            break;
        }

        memset(&qmi_request, 0, sizeof(qmi_request));
        if (config_id && (config_id_len > 0))
        {
            qmi_request.config_id_len = config_id_len;
            memcpy(qmi_request.config_id, config_id, qmi_request.config_id_len);
            QCRIL_LOG_INFO("request to delete config id: %s",
                            qmi_request.config_id);
            qmi_request.config_id_valid = TRUE;
        }
        else
        {
            // config_id is null or config_id_len equals 0
            qmi_request.config_id_valid = FALSE;
            QCRIL_LOG_INFO("request to delete all MBNs, due to no config_id specified");
        }
        qmi_request.config_type       = config_type;

        qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_DELETE_CONFIG_REQ_V01,
                         &qmi_request,
                         sizeof(qmi_request),
                         &qmi_response,
                         sizeof(qmi_response));

        result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error,
                                                      &qmi_response.resp);
    } while (0);

    QCRIL_LOG_FUNC_RETURN();
    return result;
}

/*=========================================================================

  FUNCTION:  qcril_qmi_pdc_set_select_configuration

===========================================================================*/
/*!
    @brief
    Select a configuration

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_pdc_set_select_configuration
(
    uint32_t                 sub_id,
    pdc_config_type_enum_v01 config_type,
    uint8_t                 *config_id,
    uint32_t                 config_id_len
)
{
    pdc_set_selected_config_req_msg_v01     qmi_request;
    pdc_set_selected_config_resp_msg_v01    qmi_response;
    pdc_config_info_req_type_v01           *config_info;
    qmi_client_error_type                   qmi_error;
    RIL_Errno                               result = RIL_E_GENERIC_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    do {
        if ((!config_id) || (config_id_len >= PDC_CONFIG_ID_SIZE_MAX_V01))
        {
            break;
        }

        g_pdc_info.is_under_processing = 1;
        g_pdc_info.cur_select_sub = sub_id;
        memset(&qmi_request, 0, sizeof(qmi_request));

        config_info = &qmi_request.new_config_info;
        config_info->config_type   = config_type;
        config_info->config_id_len = config_id_len;
        memcpy(config_info->config_id, config_id, config_info->config_id_len);

        g_pdc_info.config_id_len = config_id_len;
        memcpy(g_pdc_info.config_id, config_id, g_pdc_info.config_id_len);

        if (config_type != PDC_CONFIG_TYPE_MODEM_PLATFORM_V01)
        {
            qmi_request.subscription_id_valid = 1;
            qmi_request.subscription_id = sub_id;
        }

        qmi_error = qmi_client_pdc_send_raw_sync(QMI_PDC_SET_SELECTED_CONFIG_REQ_V01,
                                            &qmi_request, sizeof( qmi_request ),
                                            &qmi_response, sizeof( qmi_response ));

        result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
                                                qmi_error, &qmi_response.resp);
    } while (0);

    if (result != RIL_E_SUCCESS)
    {
        g_pdc_info.is_under_processing = 0;
    }

    QCRIL_LOG_FUNC_RETURN();

    return result;
}

void qcril_qmi_pdc_reset_mbn_state()
{
  QCRIL_LOG_INFO("Reset PDC MBN state");
  g_pdc_info.is_under_processing = 0;
}

#ifdef QMI_RIL_UTF
void qcril_qmi_hal_pdc_module_cleanup()
{
    MbnModule *module = getMbnModule();
    module->qcrilHalPdcModuleCleanup();
}
#endif
