/******************************************************************************
#  Copyright (c) 2009-2010, 2013, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_client.c

  @brief
  Client initializations and common functions defined here.

*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/10   sk      Initial Changes

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <errno.h>
#include <cutils/memory.h>
#include <cutils/properties.h>
#include "qcril_qmi_client.h"
#include "common_v01.h"
#include "voice_service_v02.h"
#include "network_access_service_v01.h"
#include "wireless_messaging_service_v01.h"
#include "device_management_service_v01.h"
#include "wireless_data_service_v01.h"
#include "phonebook_manager_service_v01.h"
#include "specific_absorption_rate_v01.h"
#include "ip_multimedia_subsystem_application_v01.h"
#include "qcril_qmi_voice.h"
#include "modules/nas/qcril_qmi_nas.h"
#include "qcril_reqlist.h"
#include "qcril_pbm.h"
#include "modules/sms/qcril_qmi_sms.h"
#include "modules/nas/qcril_arb.h"
#include "modules/ims/qcril_qmi_ims.h"
#include "qcril_am.h"
#include "qmi_errors.h"
#include "qcril_qmi_pdc.h"
#include "qmi_ril_platform_dep.h"
#include "ip_multimedia_subsystem_presence_v01.h"
#include "radio_frequency_radiated_performance_enhancement_v01.h"
#include "ip_multimedia_subsystem_settings_v01.h"
#include "persistent_device_configuration_v01.h"
#include "lte_v01.h"
#include "qcril_qmi_lte_direct_disc.h"
#include <framework/Log.h>
#include "modules/qmi/VoiceModemEndPoint.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/nas/qcril_nas_legacy.h"
#include "modules/qmi/EndpointStatusIndMessage.h"
#include "qcril_pbm_legacy.h"

#ifdef QMI_RIL_UTF
extern "C" {
#include <ril_utf_wake_lock_redef.h>
}
#else
#include <hardware_legacy/power.h>
#endif

#define TAG "QCRIL_QMI_CLIENT"

/*===========================================================================

                    INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define MAX_TRIES_FOR_QMI_INIT 10
#ifdef QMI_RIL_UTF
#define MAX_TRIES_FOR_QMI_IMSA_INIT 1
#else
#define MAX_TRIES_FOR_QMI_IMSA_INIT 20
#endif

#ifdef QMI_RIL_UTF
#define MAX_TRIES_FOR_QMI_IMSS_INIT 1
#else
#define MAX_TRIES_FOR_QMI_IMSS_INIT 20
#endif

#define RESUMER_ACTION_DELAY 1

#define RESUMER_RETRY_DELAY  1

#define QCRIL_MAX_BIND_RETRY_COUNT (10)

#define  QCRIL_BIND_RETRY_TIMER (500*1000)

#define QCRIL_PRE_CLIENT_INIT_WAKE_LOCK_NAME "qcril_pre_client_init"
#define QCRIL_PRE_CLIENT_INIT_WAKE_LOCK_HELD_PROP "ril.qcril_pre_init_lock_held"
#define QMI_RIL_DO_NOT_INIT_CSVT "persist.vendor.radio.do_not_init_csvt"

// cache handling
#define CLIENT_CACHE_LOCK()                            { pthread_mutex_lock(&client_info.cache_lock_mutex); }
#define CLIENT_CACHE_UNLOCK()                          { pthread_mutex_unlock(&client_info.cache_lock_mutex); }

#define IMS_CLIENT_CACHE_LOCK()       { pthread_mutex_lock(&qcril_ims_client_cache_lock_mutex); }
#define IMS_CLIENT_CACHE_UNLOCK()     { pthread_mutex_unlock(&qcril_ims_client_cache_lock_mutex); }

typedef struct
{
  int transition_index;
} qmi_ril_modem_reset_round_info_type;



/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/


qcril_qmi_client_private_info_type client_info;

static qmi_ril_modem_reset_round_info_type qmi_ril_modem_reset_round_info;

// TODO: change to CPP file and introduce vectors
static const int QCRIL_MODEM_RESET_CYCLE_COUNT = 5;
static qcril_evt_e_type qmi_ril_modem_reset_suspend_evt_cycle[QCRIL_MODEM_RESET_CYCLE_COUNT];
static qcril_evt_e_type qmi_ril_modem_reset_resume_evt_cycle[QCRIL_MODEM_RESET_CYCLE_COUNT];

static uint8 qmi_ril_qmi_client_pre_init_lock_acq_decision;

static uint8 qmi_ril_qmi_client_pre_initialization_lock_held;

static pthread_mutex_t       qcril_ims_client_cache_lock_mutex;
static pthread_mutexattr_t   qcril_ims_client_cache_lock_mutex_atr;

/*===========================================================================

                   INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
static void qcril_qmi_client_common_empty_unsol_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
);

static void qmi_ril_enter_suspend(void);
static void qmi_ril_enter_resume(void);
static void qmi_ril_next_suspending_action(void);
static void qmi_ril_next_resuming_action(void);
static RIL_Errno qcril_qmi_init_core_client_handles( void );
static void qcril_qmi_release_client_handles(void);
static RIL_Errno qmi_ril_resumer_initiate(void);
static void* qmi_ril_resumer_deferred_action_thread_proc(void * param);
static void qmi_ril_resume_retry_handler(void * param);
static void qmi_ril_pre_resume_main_threaded(void * param);
static void qmi_ril_suspend_init_work_handler();
static void qcril_qmi_cb_thread_name_init();
static void qcril_qmi_qmi_thread_name_init_cb
(
  qmi_client_type              user_handle,
  unsigned int                 msg_id,
  void                        *resp_c_struct,
  unsigned int                 resp_c_struct_len,
  void                        *resp_cb_data,
  qmi_client_error_type        transp_err
);

static void qmi_ril_ssr_perform_final_post_ssr_init(void * param);
static void qmi_ril_suspended_main_threaded(void * param);
static void qmi_ril_final_suspend_main_threaded(void * param);

#if 0
static void qmi_ril_qmi_req_log( qmi_idl_service_object_type service_object,
                                 int message_id,
                                 void* encoded_qmi_bytestream,
                                 int encoded_qmi_bytestream_len );
static void qmi_ril_qmi_resp_log( qmi_idl_service_object_type service_object,
                                  int message_id,
                                  void* encoded_qmi_bytestream,
                                  int encoded_qmi_bytestream_len );
static void qmi_ril_qmi_ind_log( int service_id,
                                 int message_id,
                                 void* encoded_qmi_bytestream,
                                 int encoded_qmi_bytestream_len );
#endif

static void qmi_ril_qmi_client_pre_initialization_set();
static void qmi_ril_qmi_client_pre_initialization_get();

static void qcril_qmi_service_down_event
(
  qmi_client_type clnt,
  qmi_client_error_type error,
  void *error_cb_data
);

static void qcril_qmi_service_up_event
(
  qmi_client_type clnt,
  qmi_idl_service_object_type svc_obj,
  qmi_client_notify_event_type service_event,
  void *cb_data
);

int qcril_qmi_modem_power_voting_state();

/* data to check if service is up or not */
typedef struct {
    qmi_idl_service_object_type   svc_obj;
    void                         *cb_data;
} qcril_check_service_up_data;

extern "C" void qcril_qmi_modem_power_process_bootup();

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
//===========================================================================
// qcril_qmi_client_bootup_init
//===========================================================================
void qcril_qmi_client_init_tables(void)  {
    qmi_ril_modem_reset_suspend_evt_cycle[0] =  QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_SUSPEND_REQ;
    qmi_ril_modem_reset_suspend_evt_cycle[1] =  QCRIL_EVT_NONE; // this must be final

    qmi_ril_modem_reset_resume_evt_cycle[0] =  QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_PRE_RESUME_REQ;
    qmi_ril_modem_reset_resume_evt_cycle[1] =  QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_RESUME_REQ;
    qmi_ril_modem_reset_resume_evt_cycle[2] =  QCRIL_EVT_NONE; // this must be final
}


//===========================================================================
// qcril_qmi_client_bootup_init
//===========================================================================
void qcril_qmi_client_bootup_init(void)
{
    pthread_mutexattr_init(&qcril_ims_client_cache_lock_mutex_atr);
    pthread_mutex_init(&qcril_ims_client_cache_lock_mutex,
                       &qcril_ims_client_cache_lock_mutex_atr);
}

boolean is_qcril_qmi_client_ready(qcril_qmi_client_e_type svc_type)
{
    boolean ret = FALSE;
    if (svc_type >= QCRIL_QMI_CLIENT_FIRST && svc_type < QCRIL_QMI_CLIENT_LAST) {
        ret = (client_info.qmi_svc_clients[svc_type] != NULL);
    }
    return ret;
}

//===========================================================================
//qcril_qmi_client_init
//===========================================================================
RIL_Errno qcril_qmi_client_init( void )
{
  RIL_Errno res = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_FUNC_ENTRY();

  /* Start modem or vote for start modem */
  qcril_qmi_modem_power_process_bootup();
  qcril_qmi_client_init_tables();


  memset(&client_info, 0, sizeof(client_info));

  do
  {
      QCRIL_LOG_DEBUG( "Client connecting to QMI FW" );

      // QMI VOICE command callback
      //client_info.client_cbs[QCRIL_QMI_CLIENT_VOICE] = qcril_qmi_voice_command_cb;

      // Get IDL service objects
      client_info.service_objects[QCRIL_QMI_CLIENT_VOICE] = voice_get_service_object_v02();
      client_info.service_objects[QCRIL_QMI_CLIENT_NAS] = nas_get_service_object_v01();
      client_info.service_objects[QCRIL_QMI_CLIENT_WMS] = wms_get_service_object_v01();
      client_info.service_objects[QCRIL_QMI_CLIENT_WDS] = wds_get_service_object_v01();
      client_info.service_objects[QCRIL_QMI_CLIENT_DMS] = dms_get_service_object_v01();
      /*client_info.service_objects[QCRIL_QMI_CLIENT_UIM] = uim_get_service_object_v01();*/
      client_info.service_objects[QCRIL_QMI_CLIENT_PBM] =  pbm_get_service_object_v01();
      client_info.service_objects[QCRIL_QMI_CLIENT_RF_SAR] =  sar_get_service_object_v01();
#ifndef QMI_RIL_UTF
      client_info.service_objects[QCRIL_QMI_CLIENT_IMS_VT] =   ims_qmi_get_service_object_v01();
#endif
      client_info.service_objects[QCRIL_QMI_CLIENT_IMS_PRESENCE] = imsp_get_service_object_v01();
      client_info.service_objects[QCRIL_QMI_CLIENT_IMSA] = imsa_get_service_object_v01();
      client_info.service_objects[QCRIL_QMI_CLIENT_RFPE] = rfrpe_get_service_object_v01();
      client_info.service_objects[QCRIL_QMI_CLIENT_IMS_SETTING] = imss_get_service_object_v01();
#ifdef QMI_RIL_UTF
      client_info.service_objects[QCRIL_QMI_CLIENT_DSD] = dsd_get_service_object_v01();
#endif
      client_info.service_objects[QCRIL_QMI_CLIENT_LTE] = lte_get_service_object_v01();

      pthread_mutexattr_init(&client_info.cache_lock_mtx_atr);
      pthread_mutex_init(&client_info.cache_lock_mutex, &client_info.cache_lock_mtx_atr);

      res = qcril_qmi_init_core_client_handles();

      if (RIL_E_SUCCESS != res)
          break;
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);

  return res;

} // qcril_qmi_client_init

//===========================================================================
//qmi_ril_client_get_master_port
//===========================================================================
qmi_client_qmux_instance_type qmi_ril_client_get_master_port(void)
{
    qmi_client_qmux_instance_type qmi_default_port;
    qmi_default_port = (qmi_client_qmux_instance_type)QMI_CLIENT_INSTANCE_ANY;

    QCRIL_LOG_INFO("using port %d", qmi_default_port);
    return qmi_default_port;
} // qmi_ril_client_get_master_port

//===========================================================================
//qcril_qmi_init_core_client_handles
//===========================================================================
RIL_Errno qcril_qmi_init_core_client_handles( void )
{
  qmi_client_error_type client_err = QMI_NO_ERR;
  //int   num_tries = 0;
  qmi_client_qmux_instance_type qmi_master_port;
  int   idx;
  boolean imsp_init = FALSE;

  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  RIL_SubscriptionType sub_num;
  qcril_instance_id_e_type cur_proc_instance;
  RIL_Errno bind_res;

  char* log_failed_qmi_svc = NULL;
  //int num_of_services = 0;
  //int do_not_init_csvt = 0;
  int time_out = 4;

  char property_name[ PROPERTY_VALUE_MAX ];
  uint8_t stack_id = 0;
  int temp_int = 0;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
        if ( qmi_ril_is_multi_sim_feature_supported() )
        {
            cur_proc_instance = qmi_ril_get_process_instance_id();

            snprintf( property_name, sizeof(property_name), "%s%d", QMI_RIL_MULTI_SIM_STACK_ID, cur_proc_instance );
            qmi_ril_get_property_value_from_integer(property_name, &temp_int, QCRIL_MODEM_MAX_STACK_ID);
            stack_id = (uint8_t) temp_int;
            QCRIL_LOG_INFO( "[MSIM].. stack id prop  %s - %d", property_name, (RIL_SubscriptionType)stack_id );
            if ( ( stack_id >= QCRIL_MODEM_MAX_STACK_ID) )
            {
                QCRIL_LOG_INFO( "[MSIM].. bind to subscription based on instance %d", (int) cur_proc_instance );
                switch ( cur_proc_instance )
                {
                  case QCRIL_THIRD_INSTANCE_ID:
                    sub_num = RIL_SUBSCRIPTION_3;
                    break;
                  case QCRIL_SECOND_INSTANCE_ID:
                    sub_num = RIL_SUBSCRIPTION_2;
                    break;
                  case QCRIL_DEFAULT_INSTANCE_ID: // fallthrough
                  default:
                    sub_num = RIL_SUBSCRIPTION_1;
                    break;
                }
                stack_id = (uint8_t) sub_num;
            }
            qcril_qmi_nas_update_modem_stack_id(stack_id);

            auto shared_msg = std::make_shared<StackIdIndMessage>(
                    qmi_ril_get_process_instance_id(), stack_id);
            Dispatcher::getInstance().dispatch(shared_msg);
        }

        for ( idx = 0; idx < QCRIL_QMI_CLIENT_MAX; idx ++ )
        {
            // TODO Research: move VOICE out of part of SSR temporarily
            //Do not reset states for services which are not part of SSR
            if( idx != QCRIL_QMI_CLIENT_IMSA &&
                idx != QCRIL_QMI_CLIENT_IMS_SETTING &&
                idx != QCRIL_QMI_CLIENT_VOICE)
            {
              client_info.qmi_svc_clients[ idx ] = NULL;
              client_info.client_state[ idx ] = QCRIL_QMI_SERVICE_NOT_CONNECTED;
            }
        }

        qmi_master_port = qmi_ril_client_get_master_port();

        // Initialize the QMI DMS client
        client_err = qmi_client_init_instance(client_info.service_objects[QCRIL_QMI_CLIENT_DMS],
                                              qmi_master_port,
                                              qcril_qmi_dms_unsolicited_indication_cb,
                                              NULL,
                                              &client_info.os_params[QCRIL_QMI_CLIENT_DMS],
                                              time_out,
                                              &client_info.qmi_svc_clients[QCRIL_QMI_CLIENT_DMS]);
        QCRIL_LOG_INFO("qmi_client_init_instance returned (%d) for DMS ",
                        client_err);
        if (client_err)
        {
          QCRIL_LOG_INFO("qmi_client_init_instance returned failure(%d) for DMS ",
                          client_err);
          log_failed_qmi_svc = (char*)"QMI DMS";
          break;
        }
        else
        {
          client_info.client_state[QCRIL_QMI_CLIENT_DMS] = QCRIL_QMI_SERVICE_CONNECTED;
          qmi_ril_qmi_client_pre_initialization_release();
          auto shared_msg = std::make_shared<EndpointStatusIndMessage>
              (ModemEndPoint::State::OPERATIONAL, REG_MSG("DMS_ENDPOINT_STATUS_IND"));
          if (shared_msg)
          {
            Dispatcher::getInstance().dispatch(shared_msg);
          }
        }

        client_info.client_cbs[QCRIL_QMI_CLIENT_DMS] = qcril_qmi_dms_command_cb;

#ifdef QMI_RIL_UTF
        // Initialize the QMI DSD client - UTF builds only
        client_err = qmi_client_init_instance(client_info.service_objects[QCRIL_QMI_CLIENT_DSD],
                                              qmi_master_port,
                                              qcril_qmi_dsd_ind_cb,
                                              NULL,
                                              &client_info.os_params[QCRIL_QMI_CLIENT_DSD],
                                              time_out,
                                              &client_info.qmi_svc_clients[QCRIL_QMI_CLIENT_DSD]);
        if (client_err)
        {
          QCRIL_LOG_INFO("qmi_client_init_instance returned failure(%d) for DSD",
                         client_err);
          log_failed_qmi_svc = (char*)"QMI DSD";
          break;
        }
        else
        {
          client_info.client_state[QCRIL_QMI_CLIENT_DSD] = QCRIL_QMI_SERVICE_CONNECTED;
        }
#endif
        // Initialize the QMI RF SAR client
        client_err = qmi_client_init_instance(client_info.service_objects[QCRIL_QMI_CLIENT_RF_SAR],
                                              qmi_master_port,
                                              qcril_qmi_client_common_empty_unsol_ind_cb,
                                              NULL,
                                              &client_info.os_params[QCRIL_QMI_CLIENT_RF_SAR],
                                              time_out,
                                              &client_info.qmi_svc_clients[QCRIL_QMI_CLIENT_RF_SAR]);
        QCRIL_LOG_INFO("qmi_client_init_instance returned (%d) for RF SAR ",
                        client_err);
        if (client_err)
        {
          // do not return failure if QMI RF SAR initialization fails
          QCRIL_LOG_INFO("qmi_client_init_instance returned failure(%d) for RF SAR ",
                          client_err);
          client_info.qmi_svc_clients[ QCRIL_QMI_CLIENT_RF_SAR ] = NULL;
        }
        else
        {
          client_info.client_state[QCRIL_QMI_CLIENT_RF_SAR] = QCRIL_QMI_SERVICE_CONNECTED;
        }

#ifdef FEATURE_QCRIL_OEMHOOK_IMS_VT
#ifndef QMI_RIL_UTF
        // Initialize the QMI IMS VT client
        client_err = qmi_client_init_instance(client_info.service_objects[QCRIL_QMI_CLIENT_IMS_VT],
                                              qmi_master_port,
                                              qcril_qmi_ims_vt_unsol_ind_cb,
                                              NULL,
                                              &client_info.os_params[QCRIL_QMI_CLIENT_IMS_VT],
                                              time_out,
                                              &client_info.qmi_svc_clients[QCRIL_QMI_CLIENT_IMS_VT]);
        if ( client_err )
        {
          QCRIL_LOG_INFO("qcril_qmi_vt_init returned failure(%d)",client_err);
          client_info.qmi_svc_clients[QCRIL_QMI_CLIENT_IMS_VT] = NULL;
        }
#endif

        // QMI IMS VT command callback
        client_info.client_cbs[QCRIL_QMI_CLIENT_IMS_VT] = qcril_qmi_ims_vt_command_cb;
#endif

#ifdef FEATURE_QCRIL_OEMHOOK_IMS_PRESENCE
        // Initialize the QMI IMS client
        client_err = qmi_client_init_instance(client_info.service_objects[QCRIL_QMI_CLIENT_IMS_PRESENCE],
                                              qmi_master_port,
                                              qcril_qmi_ims_presence_unsol_ind_cb,
                                              NULL,
                                              &client_info.os_params[QCRIL_QMI_CLIENT_IMS_PRESENCE],
                                              time_out,
                                              &client_info.qmi_svc_clients[QCRIL_QMI_CLIENT_IMS_PRESENCE]);
        if ( client_err )
        {
          QCRIL_LOG_ERROR("qcril_qmi_presence_init returned failure(%d)",client_err);
          client_info.qmi_svc_clients[QCRIL_QMI_CLIENT_IMS_PRESENCE] = NULL;
        }
        else
        {
          imsp_init = TRUE;
        }

        // QMI IMS PRESENCE command callback
        client_info.client_cbs[QCRIL_QMI_CLIENT_IMS_PRESENCE] = qcril_qmi_ims_presence_command_cb;
#endif

        // Initialize the QMI RFPE client
        client_err = qmi_client_init_instance(client_info.service_objects[QCRIL_QMI_CLIENT_RFPE],
                                              qmi_master_port,
                                              qcril_qmi_client_common_empty_unsol_ind_cb,
                                              NULL,
                                              &client_info.os_params[QCRIL_QMI_CLIENT_RFPE],
                                              time_out,
                                              &client_info.qmi_svc_clients[QCRIL_QMI_CLIENT_RFPE]);
        QCRIL_LOG_INFO("qmi_client_init_instance returned (%d) for RFRPE ",
                        client_err);
        if (client_err)
        {
          // do not return failure if QMI RFRPE initialization fails
          QCRIL_LOG_INFO("qmi_client_init_instance returned failure(%s) for RFRPE ",
                          qmi_errstr(client_err));
          client_info.qmi_svc_clients[ QCRIL_QMI_CLIENT_RFPE ] = NULL;
        }
        else
        {
          client_info.client_state[QCRIL_QMI_CLIENT_RFPE] = QCRIL_QMI_SERVICE_CONNECTED;
        }



#ifdef FEATURE_QCRIL_LTE_DIRECT
        // Initialize the QMI LTE client
        client_err = qmi_client_init_instance(client_info.service_objects[QCRIL_QMI_CLIENT_LTE],
                                              qmi_master_port,
                                              qcril_qmi_lte_direct_disc_unsol_ind_cb,
                                              NULL,
                                              &client_info.os_params[QCRIL_QMI_CLIENT_LTE],
                                              time_out,
                                              &client_info.qmi_svc_clients[QCRIL_QMI_CLIENT_LTE]);
        QCRIL_LOG_INFO("qmi_client_init_instance returned (%d) for LTE ",
                        client_err);
        if (client_err)
        {
          // do not return failure if QMI LTE initialization fails
          QCRIL_LOG_INFO("qmi_client_init_instance returned failure(%s) for QMI LTE",
                          qmi_errstr(client_err));
          client_info.qmi_svc_clients[QCRIL_QMI_CLIENT_LTE] = NULL;
        }
        else
        {
          client_info.client_state[QCRIL_QMI_CLIENT_LTE] = QCRIL_QMI_SERVICE_CONNECTED;
        }
        // QMI LTE command callback
        client_info.client_cbs[QCRIL_QMI_CLIENT_LTE] = qcril_qmi_lte_direct_disc_command_cb;
#endif

        CLIENT_CACHE_LOCK();
        client_info.num_of_active_clients = 0;
        for ( idx = 0; idx < QCRIL_QMI_CLIENT_MAX; idx ++ )
        {
            //TODO Research: move VOICE out of part of SSR temporarily
            //Do not register down events for services which are not included for SSR.
            if ((client_info.client_state[idx] == QCRIL_QMI_SERVICE_CONNECTED) &&
                 (idx != QCRIL_QMI_CLIENT_IMSA && idx != QCRIL_QMI_CLIENT_IMS_SETTING &&
                  idx != QCRIL_QMI_CLIENT_VOICE))
            {
                client_err = qmi_client_register_error_cb(client_info.qmi_svc_clients[idx],
                                                          qcril_qmi_service_down_event,
                                                          (void*)(intptr_t)idx);

                QCRIL_LOG_INFO("error call back registration res - %d index - %d", client_err, idx);
                if( client_err == QMI_NO_ERR )
                {
                    client_info.num_of_active_clients++;
                }
            }
        }

        client_info.max_active_clients       = client_info.num_of_active_clients;
        client_info.qmi_client_init_complete = TRUE;
        QCRIL_LOG_INFO("Max active clients %d", client_info.max_active_clients);
        CLIENT_CACHE_UNLOCK();

        // Bind to subsciption based on property
        if ( qmi_ril_is_multi_sim_feature_supported() )
        {
            QCRIL_LOG_INFO( "[MSIM]..invoking bind for subscription %d", (int) stack_id );
            bind_res = qcril_qmi_client_dsds_bind_to_subscription( (RIL_SubscriptionType)stack_id );
            QCRIL_LOG_INFO( ".. bind res %d", (int) bind_res );
        }

        //QCRIL_LOG_INFO("Initing Voice client...");
        //res = qcril_qmi_voice_init();
        res = RIL_E_SUCCESS;

        QCRIL_LOG_INFO("Initing DMS client...");
        qcril_qmi_dms_init();

        QCRIL_LOG_INFO("Initing  LteDirectDiscovery client...");
        qcril_qmi_lte_direct_init();

        // IMS_Presense
        if (imsp_init)
        {
          QCRIL_LOG_INFO("Initing IMS client...");
          qcril_qmi_ims_init();
        }

        if ( qmi_ril_is_multi_sim_feature_supported() )
        {
            qcril_qmi_nas_multi_sim_init();
        }
        else
        {
            qcril_qmi_nas_get_device_capability(TRUE, TRUE, TRUE, TRUE);
        }

        qcril_qmi_nas_update_embms_status();
        qcril_qmi_nas_embms_send_embms_status(qcril_qmi_nas_get_embms_status());

        qcril_qmi_cb_thread_name_init();

  } while (FALSE);


  if ( RIL_E_SUCCESS == res )
  {
    if (qcril_qmi_nas_modem_power_is_mdm_shdn_in_apm())
    {
      qcril_qmi_nas_modem_power_ril_resumed();
    }
    QCRIL_LOG_INFO( "Client init successful " );
  }
  else if ( log_failed_qmi_svc )
  {
    QCRIL_LOG_ESSENTIAL( "Client init failed, details: %s", log_failed_qmi_svc);
  }
  else
  {
    QCRIL_LOG_ESSENTIAL( "Post QMI client initialization failure");
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);

  return res;

} // qcril_qmi_init_core_client_handles

/*=========================================================================
  FUNCTION:  qcril_qmi_client_is_client_initialized
===========================================================================*/
/*!
    @brief
    Check if the client for a QMI service is initialized.

    @return
    True if the client is connected, False otherwise.
*/
/*=========================================================================*/
bool qcril_qmi_client_is_client_initialized(qcril_qmi_client_e_type client)
{
  return client_info.client_state[client] == QCRIL_QMI_SERVICE_CONNECTED;
} /* qcril_qmi_client_is_client_initialized */

//===========================================================================
// qcril_qmi_client_destroy_mutex
//===========================================================================
void qcril_qmi_client_destroy_mutex
(
  void
)
{
    pthread_mutex_destroy(&client_info.cache_lock_mutex);
    pthread_mutexattr_destroy(&client_info.cache_lock_mtx_atr);
}// qcril_qmi_client_destroy_mutex

//===========================================================================
//qcril_qmi_client_release
//===========================================================================
void qcril_qmi_client_release
(
  void
)
{
  qcril_qmi_release_client_handles();
  qcril_qmi_client_destroy_mutex();

  qcril_qmi_nas_dms_commost_post_cleanup();
  qcril_qmi_voice_post_cleanup();
#ifndef QMI_RIL_UTF
  qcril_am_post_cleanup();
#endif

}// qcril_qmi_client_release

//===========================================================================
//qcril_qmi_release_client_handles
//===========================================================================
void qcril_qmi_release_client_handles(void)
{
   int idx;
   QCRIL_LOG_FUNC_ENTRY();
   if (client_info.qmi_client_init_complete)
   {
      client_info.qmi_client_init_complete = FALSE;
   }

   for (idx = QCRIL_QMI_CLIENT_FIRST; idx < QCRIL_QMI_CLIENT_LAST; idx++)
   {
      if(client_info.qmi_svc_clients[idx] &&
         idx != QCRIL_QMI_CLIENT_IMSA && idx != QCRIL_QMI_CLIENT_IMS_SETTING &&
         idx != QCRIL_QMI_CLIENT_VOICE)
      {
         qmi_client_release(client_info.qmi_svc_clients[idx]);
         client_info.qmi_svc_clients[idx] = NULL;
      }
   }

   QCRIL_LOG_FUNC_RETURN();

}// qcril_qmi_release_client_handles


/*=========================================================================
  FUNCTION:  qcril_qmi_client_map_qmi_err_to_ril_err

===========================================================================*/
/*!
    @brief
    Map QMI error to RIL error.

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_client_map_qmi_err_to_ril_err
(
  qmi_error_type_v01 qmi_err
)
{
  RIL_Errno ril_err;

  switch(qmi_err)
  {
    case QMI_ERR_NONE_V01: //fallthrough
    case QMI_ERR_NO_EFFECT_V01:
      ril_err = RIL_E_SUCCESS;
      break;

    case QMI_ERR_FDN_RESTRICT_V01:
      ril_err = RIL_E_FDN_CHECK_FAILURE;
      break;

    case QMI_ERR_MALFORMED_MSG_V01:
      ril_err = RIL_E_INTERNAL_ERR;
      break;

    case QMI_ERR_INTERNAL_V01:
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_DEVICE_NOT_READY_V01:
      ril_err = RIL_E_INVALID_MODEM_STATE;
      break;

    case QMI_ERR_NO_MEMORY_V01:
      ril_err = RIL_E_NO_MEMORY;
      break;

    case QMI_ERR_ARG_TOO_LONG_V01: //fallthrough
    case QMI_ERR_MISSING_ARG_V01: //fallthrough
    case QMI_ERR_INVALID_ARG_V01:
      ril_err = RIL_E_INVALID_ARGUMENTS;
      break;

    case QMI_ERR_ENCODING_V01:
      ril_err = RIL_E_ENCODING_ERR;
      break;

    case QMI_ERR_SMSC_ADDR_V01:
      ril_err = RIL_E_INVALID_SMSC_ADDRESS;
      break;

    case QMI_ERR_DEVICE_STORAGE_FULL_V01:
      ril_err = RIL_E_NO_RESOURCES;
      break;

    case QMI_ERR_OP_DEVICE_UNSUPPORTED_V01:
      ril_err = RIL_E_REQUEST_NOT_SUPPORTED;
      break;

    case QMI_ERR_NETWORK_NOT_READY_V01:
      ril_err = RIL_E_NETWORK_NOT_READY;
      break;

    case QMI_ERR_NOT_PROVISIONED_V01:
      ril_err = RIL_E_NOT_PROVISIONED;
      break;

    case QMI_ERR_INVALID_OPERATION_V01:
    case QMI_ERR_INVALID_TRANSITION_V01:
    case QMI_ERR_OP_NETWORK_UNSUPPORTED_V01:
      ril_err = RIL_E_OPERATION_NOT_ALLOWED;
      break;

    case QMI_ERR_NO_ENTRY_V01:
      ril_err = RIL_E_NO_SUCH_ENTRY;
      break;

    case QMI_ERR_ACK_NOT_SENT_V01:
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_CARD_CALL_CONTROL_FAILED_V01:
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_NO_SUBSCRIPTION_V01:
      ril_err = RIL_E_NO_SUBSCRIPTION;
      break;

    case QMI_ERR_NO_NETWORK_FOUND_V01:
      ril_err = RIL_E_NO_NETWORK_FOUND;
      break;

    case QMI_ERR_DEVICE_IN_USE_V01:
      ril_err = RIL_E_DEVICE_IN_USE;
      break;

    case QMI_ERR_CALL_FAILED_V01:
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_INCOMPATIBLE_STATE_V01:
      ril_err = RIL_E_INVALID_MODEM_STATE;
      break;

    case QMI_ERR_ABORTED_V01:
      ril_err = RIL_E_CANCELLED;
      break;

    case QMI_ERR_NO_RADIO_V01:
    /* Nas and voice qmi documents have different perception
       for this error QMI_ERR_NO_RADIO. Hence using MODEM_ERR
       as a common ground */
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_NOT_SUPPORTED_V01:
    /* This error is sent by the modem when the Request is
       "currently" not supported. Using MODEM_ERR as this
        pertains to modem not being able to perform the request
        currently
        */
      ril_err = RIL_E_MODEM_ERR;
      break;

    case QMI_ERR_NETWORK_ABORTED_V01:
      ril_err = RIL_E_ABORTED;
      break;

    case QMI_ERR_INFO_UNAVAILABLE_V01:
      ril_err = RIL_E_NOT_PROVISIONED;
      break;

    case QMI_ERR_HARDWARE_RESTRICTED_V01:
      ril_err = RIL_E_SYSTEM_ERR;
      break;

    default:
      ril_err = RIL_E_MODEM_ERR;
      break;
  }
  return ril_err;
}/* qcril_qmi_client_map_qmi_err_to_ril_err */


/*=========================================================================
  FUNCTION:  qcril_qmi_client_send_msg_async

===========================================================================*/
/*!
    @brief
    Send QMI command asynchronously.

    @return
    None.
*/
/*=========================================================================*/
errno_enum_type qcril_qmi_client_send_msg_async
(
  qcril_qmi_client_e_type      svc_type,
  unsigned long                msg_id,
  void                         *req_c_struct,
  int                          req_c_struct_len,
  void                         *resp_c_struct,
  int                          resp_c_struct_len,
  void                         *resp_cb_data
)
{
  qmi_client_error_type qmi_error;
  errno_enum_type ret = E_FAILURE;
  qmi_txn_handle txn_handle;

 /* check if the client is initialized already */

  if( svc_type < QCRIL_QMI_CLIENT_LAST )
  {
    if (NULL != client_info.qmi_svc_clients[svc_type])
    {
      qmi_error =  qmi_client_send_msg_async_with_shm(client_info.qmi_svc_clients[svc_type],
                                      msg_id,
                                      req_c_struct,
                                      req_c_struct_len,
                                      resp_c_struct,
                                      resp_c_struct_len,
                                      client_info.client_cbs[svc_type],
                                      resp_cb_data,
                                      &txn_handle);
      if (qmi_error!= QMI_NO_ERR )
      {
        QCRIL_LOG_ERROR("Failed to send async qmi msg 0x%02x w/%s",
                         msg_id, qmi_errstr(qmi_error));
      }
      else
      {
        ret = E_SUCCESS;
      }
    }
    else
    {
       QCRIL_LOG_INFO("svc %d is not initialized", (int) svc_type);
    }
  }
  return ret;
} /* qcril_qmi_client_send_msg_async */

/*=========================================================================
  FUNCTION:  qcril_qmi_client_send_msg_async_ex

===========================================================================*/
/*!
    @brief
    Send QMI command asynchronously.

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_client_send_msg_async_ex
(
  qcril_qmi_client_e_type      svc_type,
  unsigned long                msg_id,
  void                         *req_c_struct,
  int                          req_c_struct_len,
  void                         *resp_c_struct,
  int                          resp_c_struct_len,
  void                         *resp_cb_data
)
{
  qmi_txn_handle txn_handle;

  RIL_Errno ril_err = RIL_E_SYSTEM_ERR;
  qmi_client_error_type qmi_transport_error;

 /* check if the client is initialized already */

  if( svc_type >= QCRIL_QMI_CLIENT_LAST )
  {
    QCRIL_LOG_ERROR("Invalid service %d, context msg 0x%x", svc_type, msg_id);
  }
  else
  {
    if (NULL != client_info.qmi_svc_clients[svc_type])
    {
      qmi_transport_error =  qmi_client_send_msg_async_with_shm(
                                      client_info.qmi_svc_clients[svc_type],
                                      msg_id,
                                      req_c_struct,
                                      req_c_struct_len,
                                      resp_c_struct,
                                      resp_c_struct_len,
                                      client_info.client_cbs[svc_type],
                                      resp_cb_data,
                                      &txn_handle);
      ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_transport_error, NULL);
      if ( RIL_E_SUCCESS != ril_err )
      {
        QCRIL_LOG_ERROR("error %d / %d, context msg hex %x, service %d ",ril_err, qmi_transport_error, msg_id, svc_type );
      }
    }
    else
    {
       QCRIL_LOG_INFO("svc %d is not initialized", (int) svc_type);
    }
  }

  return ril_err;
} /* qcril_qmi_client_send_msg_async_ex */


/*=========================================================================
  FUNCTION:  qcril_qmi_client_send_msg_sync

===========================================================================*/
/*!
    @brief
    Callback to handle CM Phone command callback.

    @return
    None.
*/
/*=========================================================================*/
errno_enum_type qcril_qmi_client_send_msg_sync
(
  qcril_qmi_client_e_type      svc_type,
  unsigned long                msg_id,
  void                         *req_c_struct,
  int                          req_c_struct_len,
  void                         *resp_c_struct,
  int                          resp_c_struct_len
)
{
  qmi_client_error_type qmi_error;
  errno_enum_type ret = E_FAILURE;

  if( svc_type < QCRIL_QMI_CLIENT_LAST )
  {
    if (NULL != client_info.qmi_svc_clients[svc_type])
    {
      qmi_error = qmi_client_send_msg_sync_with_shm(client_info.qmi_svc_clients[svc_type],
                              msg_id,
                              req_c_struct,
                              req_c_struct_len,
                              resp_c_struct,
                              resp_c_struct_len,
                              QCRIL_QMI_SYNC_REQ_DEF_TIMEOUT
                               );

      if (qmi_error!= QMI_NO_ERR )
      {
        QCRIL_LOG_ERROR("Failed to send sync qmi msg 0x%02x w/%s",
                         msg_id, qmi_errstr(qmi_error));
      }
      else
      {
        ret = E_SUCCESS;
      }
    }
    else
    {
       QCRIL_LOG_INFO("svc %d is not initialized", (int) svc_type);
    }
  }
  return ret;
} /* qcril_qmi_client_send_msg_sync */

/*=========================================================================
  FUNCTION:  qcril_qmi_client_send_msg_sync_ex

===========================================================================*/
/*!
    @brief
    Callback to handle CM Phone command callback.

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_client_send_msg_sync_ex
(
  qcril_qmi_client_e_type      svc_type,
  unsigned long                msg_id,
  void                         *req_c_struct,
  int                          req_c_struct_len,
  void                         *resp_c_struct,
  int                          resp_c_struct_len,
  int                          timeout_msecs
)
{
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  qmi_client_error_type qmi_transport_error;

  qmi_response_type_v01* resp_err = NULL;

  if( svc_type >= QCRIL_QMI_CLIENT_LAST )
  {
    QCRIL_LOG_ERROR("Invalid service %d, context msg 0x%x", svc_type, msg_id);
    ril_err = RIL_E_GENERIC_FAILURE;
  }
  else
  {
    if (NULL != client_info.qmi_svc_clients[svc_type])
    {
      qmi_transport_error = qmi_client_send_msg_sync_with_shm(
                              qcril_qmi_client_get_user_handle( svc_type ),
                              msg_id,
                              req_c_struct,
                              req_c_struct_len,
                              resp_c_struct,
                              resp_c_struct_len,
                              timeout_msecs
                               );

      if ( QCRIL_QMI_CLIENT_WMS == svc_type )
      { //Need to properly locate the resp field of specific QMI WMS response messages when the response message does not start with the resp field
        switch(msg_id)
        {
          case QMI_WMS_RAW_WRITE_REQ_V01:
            resp_err = &((wms_raw_write_resp_msg_v01*)resp_c_struct)->resp;
            break;

          case QMI_WMS_RAW_SEND_REQ_V01:
            resp_err = &((wms_raw_send_resp_msg_v01*)resp_c_struct)->resp;
            break;

          case QMI_WMS_RAW_READ_REQ_V01:
            resp_err = &((wms_raw_read_resp_msg_v01*)resp_c_struct)->resp;
            break;

          case QMI_WMS_GET_MESSAGE_PROTOCOL_REQ_V01:
            resp_err = &((wms_get_message_protocol_resp_msg_v01*)resp_c_struct)->resp;
            break;

          case QMI_WMS_LIST_MESSAGES_REQ_V01:
            resp_err = &((wms_list_messages_resp_msg_v01*)resp_c_struct)->resp;
            break;

          case QMI_WMS_GET_ROUTES_REQ_V01:
            resp_err = &((wms_get_routes_resp_msg_v01*)resp_c_struct)->resp;
            break;

          case QMI_WMS_GET_SMSC_ADDRESS_REQ_V01:
            resp_err = &((wms_get_smsc_address_resp_msg_v01*)resp_c_struct)->resp;
            break;

          case QMI_WMS_GET_STORE_MAX_SIZE_REQ_V01:
            resp_err = &((wms_get_store_max_size_resp_msg_v01*)resp_c_struct)->resp;
            break;

          case QMI_WMS_GET_DOMAIN_PREF_REQ_V01:
            resp_err = &((wms_get_domain_pref_resp_msg_v01*)resp_c_struct)->resp;
            break;

          case QMI_WMS_GET_MESSAGE_WAITING_REQ_V01:
            resp_err = &((wms_get_message_waiting_resp_msg_v01*)resp_c_struct)->resp;
            break;

          default:
            resp_err = (qmi_response_type_v01*)resp_c_struct;
            break;
        }
      }
      else
      {
        resp_err = (qmi_response_type_v01*)resp_c_struct;
      }
      ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_transport_error, resp_err );

      if ( RIL_E_SUCCESS != ril_err )
      {
        QCRIL_LOG_ERROR("error %d / %d / %d / %d, context msg hex %x, service %d ",
                       ril_err,
                       qmi_transport_error,
                       resp_err->result,
                       resp_err->error,
                       msg_id,
                       svc_type );
      }
    }
    else
    {
      QCRIL_LOG_INFO("svc %d is not initialized", (int) svc_type);
    }
  }

  return ril_err;

} /* qcril_qmi_client_send_msg_sync_ex */


/*=========================================================================
  FUNCTION:  qcril_qmi_client_get_service_object

===========================================================================*/
/*!
    @brief
    Get the service object for the specific service type

    @return
    None.
*/
/*=========================================================================*/
qmi_idl_service_object_type qcril_qmi_client_get_service_object
(
  qcril_qmi_client_e_type        svc_type
)
{
  qmi_idl_service_object_type svc_obj = NULL;

  if( svc_type < QCRIL_QMI_CLIENT_LAST )
  {
    svc_obj = client_info.service_objects[svc_type];
  }
  return svc_obj;
}/* qcril_qmi_client_get_service_object */


/*=========================================================================
  FUNCTION:  qcril_qmi_client_get_user_handle

===========================================================================*/
/*!
    @brief
    Get the user handle for the specific service type

    @return
    None.
*/
/*=========================================================================*/
qmi_client_type qcril_qmi_client_get_user_handle
(
  qcril_qmi_client_e_type        svc_type
)
{
  qmi_client_type clnt_type = NULL;

  if( svc_type < QCRIL_QMI_CLIENT_LAST )
  {
    clnt_type = client_info.qmi_svc_clients[svc_type];
  }
  return clnt_type;
}/* qcril_qmi_client_get_user_handle */


qmi_client_type qcril_qmi_client_get_client_handle
(
  qcril_qmi_client_e_type client
)
{
  return client_info.qmi_svc_clients[client];
}
//===========================================================================
// qcril_qmi_util_convert_qmi_response_codes_to_ril_result
//===========================================================================
RIL_Errno qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_client_error_type qmi_transport_error, qmi_response_type_v01* qmi_service_response)
{
    return qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(qmi_transport_error, qmi_service_response, QCRIL_QMI_ERR_CTX_NONE, NULL);
}
//===========================================================================
// qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex
//===========================================================================
RIL_Errno qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(qmi_client_error_type qmi_transport_error,
                                                                     qmi_response_type_v01* qmi_service_response,
                                                                     qmi_ril_err_context_e_type context,
                                                                     void* any)
{
    int res;

    int res_settled;

    voice_dial_call_resp_msg_v02* dial_call_resp_msg;
    voice_orig_ussd_resp_msg_v02* ussd_resp_msg;
    voice_manage_ip_calls_resp_msg_v02 *mng_ip_call_rsp_msg;
    qmi_ril_err_ctx_ss_resp_data_type* ss_resp_info;

    switch ( qmi_transport_error )
    {
        case QMI_NO_ERR:                // fallthrough
        case QMI_SERVICE_ERR:
            res_settled = FALSE;

            QCRIL_LOG_INFO("ctx check %d", (int) context );
            switch (context)
            {
                case QCRIL_QMI_ERR_CTX_MNG_IP_TXN:
                    mng_ip_call_rsp_msg = (voice_manage_ip_calls_resp_msg_v02 *) any;
                    QCRIL_LOG_INFO("ctx mng_ip %p", mng_ip_call_rsp_msg);
                    if ( mng_ip_call_rsp_msg && mng_ip_call_rsp_msg->failure_cause_valid )
                    {
                        QCRIL_LOG_INFO("ctx mng_ip failure_cause 0x%x", mng_ip_call_rsp_msg->failure_cause);
                        switch ( mng_ip_call_rsp_msg->failure_cause )
                        {
                            case QMI_FAILURE_CAUSE_HOLD_RESUME_FAILED_V02:
                                res = RIL_E_GENERIC_FAILURE; //RIL_E_HOLD_RESUME_FAILED;
                                res_settled = TRUE;
                                break;
                            case QMI_FAILURE_CAUSE_HOLD_RESUME_CANCELED_V02:
                                res = RIL_E_GENERIC_FAILURE; //RIL_E_HOLD_RESUME_CANCELED;
                                res_settled = TRUE;
                                break;
                            case QMI_FAILURE_CAUSE_REINVITE_COLLISION_V02:
                                res = RIL_E_GENERIC_FAILURE; //RIL_E_REINVITE_COLLISION;
                                res_settled = TRUE;
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case QCRIL_QMI_ERR_CTX_DIAL_TXN:
                    dial_call_resp_msg    = (voice_dial_call_resp_msg_v02*)any;
                    QCRIL_LOG_INFO("ctx dial %p", dial_call_resp_msg );
                    if ( dial_call_resp_msg && dial_call_resp_msg->cc_result_type_valid )
                    {
                        QCRIL_LOG_INFO("ctx dial cc result type %d", (int) dial_call_resp_msg->cc_result_type );
                        switch ( dial_call_resp_msg->cc_result_type )
                        {
                            case VOICE_CC_RESULT_TYPE_VOICE_V02:
                                res = RIL_E_DIAL_MODIFIED_TO_DIAL;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_SUPS_V02:
                                res = RIL_E_DIAL_MODIFIED_TO_SS;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_USSD_V02:
                                res = RIL_E_DIAL_MODIFIED_TO_USSD;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_VT_V02:
                                res = RIL_E_DIAL_MODIFIED_TO_DIAL_VIDEO;
                                res_settled = TRUE;
                                break;

                            default:
                                // skip
                                break;
                        }
                    }
                    break;

                case QCRIL_QMI_ERR_CTX_DIAL_VIDEO_TXN:
                    dial_call_resp_msg    = (voice_dial_call_resp_msg_v02*)any;
                    QCRIL_LOG_INFO("ctx dial video %p", dial_call_resp_msg );
                    if ( dial_call_resp_msg && dial_call_resp_msg->cc_result_type_valid )
                    {
                        QCRIL_LOG_INFO("ctx dial cc result type %d",
                                       (int) dial_call_resp_msg->cc_result_type);
                        switch ( dial_call_resp_msg->cc_result_type )
                        {
                            case VOICE_CC_RESULT_TYPE_VOICE_V02:
                                res = RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_SUPS_V02:
                                res = RIL_E_DIAL_VIDEO_MODIFIED_TO_SS;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_USSD_V02:
                                res = RIL_E_DIAL_VIDEO_MODIFIED_TO_USSD;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_VT_V02:
                                res = RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO;
                                res_settled = TRUE;
                                break;

                            default:
                                // skip
                                break;
                        }
                    }
                    break;

                case QCRIL_QMI_ERR_CTX_SEND_SS_TXN:
                    ss_resp_info          = (qmi_ril_err_ctx_ss_resp_data_type*)any;
                    if ( ss_resp_info && ss_resp_info->cc_result_type_valid )
                    {
                        switch ( *ss_resp_info->cc_result_type )
                        {
                            case VOICE_CC_RESULT_TYPE_VOICE_V02:
                                res = RIL_E_SS_MODIFIED_TO_DIAL;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_SUPS_V02:
                                res = RIL_E_SS_MODIFIED_TO_SS;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_USSD_V02:
                                res = RIL_E_SS_MODIFIED_TO_USSD;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_VT_V02:
                                res = RIL_E_SS_MODIFIED_TO_DIAL_VIDEO;
                                res_settled = TRUE;
                                break;

                            default:
                                // skip
                                break;
                        }
                    }
                    break;

                case QCRIL_QMI_ERR_CTX_SEND_USSD_TXN:
                    ussd_resp_msg         = (voice_orig_ussd_resp_msg_v02*)any;
                    if ( ussd_resp_msg && ussd_resp_msg->cc_result_type_valid )
                    {
                        switch ( ussd_resp_msg->cc_result_type )
                        {
                            case VOICE_CC_RESULT_TYPE_VOICE_V02:
                                res = RIL_E_USSD_MODIFIED_TO_DIAL;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_SUPS_V02:
                                res = RIL_E_USSD_MODIFIED_TO_SS;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_USSD_V02:
                                res = RIL_E_USSD_MODIFIED_TO_USSD;
                                res_settled = TRUE;
                                break;

                            case VOICE_CC_RESULT_TYPE_VT_V02:
                                res = RIL_E_USSD_MODIFIED_TO_DIAL_VIDEO;
                                res_settled = TRUE;
                                break;

                            default:
                                // skip
                                break;
                        }
                    }
                    break;

                default:
                    // skip
                    break;
            }

            if ( !res_settled )
            {

                if ( NULL == qmi_service_response )
                {
                    res = RIL_E_SUCCESS;
                }
                else
                {
                    switch ( qmi_service_response->result )
                    {
                        case QMI_RESULT_SUCCESS_V01:
                            res = RIL_E_SUCCESS;
                            break;

                        case QMI_RESULT_FAILURE_V01:
                            if ( qmi_service_response )
                            {
                                switch ( qmi_service_response->error )
                                {
                                    case QMI_ERR_NONE_V01:
                                    case QMI_ERR_NO_EFFECT_V01:
                                        res = RIL_E_SUCCESS;
                                        break;

                                    case QMI_ERR_NO_NETWORK_FOUND_V01:
                                        switch ( context )
                                        {
                                            case QCRIL_QMI_ERR_CTX_CELL_LOCATION_TXN:
                                                res = RIL_E_SUCCESS;
                                                break;
                                            default:
                                                res = RIL_E_NO_NETWORK_FOUND;
                                            break;
                                        }
                                        break;

                                    case QMI_ERR_NO_MEMORY_V01:
                                        res = RIL_E_NO_MEMORY;
                                        break;

                                    case QMI_ERR_INTERNAL_V01:
                                        res = RIL_E_MODEM_ERR;
                                        break;

                                    case QMI_ERR_MALFORMED_MSG_V01:
                                        res = RIL_E_INTERNAL_ERR;
                                        break;


                                    case QMI_ERR_NOT_PROVISIONED_V01:
                                        res = RIL_E_NOT_PROVISIONED;
                                        break;

                                    case QMI_ERR_MISSING_ARG_V01:
                                    case QMI_ERR_ARG_TOO_LONG_V01://fallthrough
                                    case QMI_ERR_INVALID_ARG_V01:
                                        res = RIL_E_INVALID_ARGUMENTS;
                                        break;

                                    case QMI_ERR_OP_DEVICE_UNSUPPORTED_V01:
                                        res = RIL_E_REQUEST_NOT_SUPPORTED;
                                        break;

                                    case QMI_ERR_CLIENT_IDS_EXHAUSTED_V01: //fallthrough
                                    case QMI_ERR_UNABORTABLE_TRANSACTION_V01: //fallthrough
                                    case QMI_ERR_INVALID_CLIENT_ID_V01: //fallthrough
                                    case QMI_ERR_INVALID_HANDLE_V01:
                                    case QMI_ERR_INVALID_PROFILE_V01:
                                    case QMI_ERR_OUT_OF_CALL_V01:
                                    case QMI_ERR_INVALID_TX_ID_V01:
                                    case QMI_ERR_NO_FREE_PROFILE_V01:
                                    case QMI_ERR_INVALID_PDP_TYPE_V01:
                                    case QMI_ERR_INVALID_TECH_PREF_V01:
                                    case QMI_ERR_CARD_CALL_CONTROL_FAILED_V01:
                                    case QMI_ERR_ACK_NOT_SENT_V01:
                                    case QMI_ERR_CALL_FAILED_V01:
                                        res = RIL_E_MODEM_ERR;
                                        break;

                                    case QMI_ERR_ABORTED_V01:
                                        res = RIL_E_CANCELLED;
                                        break;

                                    case QMI_ERR_FDN_RESTRICT_V01:
                                        res = RIL_E_FDN_CHECK_FAILURE;
                                        break;

                                    case QMI_ERR_INFO_UNAVAILABLE_V01:
                                        switch (context)
                                        {
                                            case QCRIL_QMI_ERR_TOLERATE_NOT_FOUND:
                                                res = RIL_E_SUCCESS;
                                                break;

                                            default:
                                                res = RIL_E_NOT_PROVISIONED;
                                                break;
                                        }
                                        break;

                                    case QMI_ERR_DEVICE_NOT_READY_V01:
                                    {
                                        switch (context)
                                        {
                                            case QCRIL_QMI_ERR_TOLERATE_NOT_READY:
                                                res = RIL_E_SUCCESS;
                                                break;

                                            default:
                                                res = RIL_E_INVALID_MODEM_STATE;
                                                break;
                                        }

                                        break;
                                    }

                                    case QMI_ERR_ENCODING_V01:
                                        res = RIL_E_ENCODING_ERR;
                                        break;

                                    case QMI_ERR_SMSC_ADDR_V01:
                                        res = RIL_E_INVALID_SMSC_ADDRESS;
                                        break;

                                    case QMI_ERR_DEVICE_STORAGE_FULL_V01:
                                        res = RIL_E_NO_RESOURCES;
                                        break;

                                    case QMI_ERR_NETWORK_NOT_READY_V01:
                                        res = RIL_E_NETWORK_NOT_READY;
                                        break;

                                    case QMI_ERR_INVALID_OPERATION_V01:
                                    case QMI_ERR_INVALID_TRANSITION_V01:
                                    case QMI_ERR_OP_NETWORK_UNSUPPORTED_V01:
                                        res = RIL_E_OPERATION_NOT_ALLOWED;
                                        break;

                                    case QMI_ERR_NO_ENTRY_V01:
                                        res = RIL_E_NO_SUCH_ENTRY;
                                        break;

                                    case QMI_ERR_DEVICE_IN_USE_V01:
                                        res = RIL_E_DEVICE_IN_USE;
                                        break;

                                    case QMI_ERR_INCOMPATIBLE_STATE_V01:
                                        res = RIL_E_INVALID_MODEM_STATE;
                                        break;

                                    case QMI_ERR_NO_RADIO_V01:
                                     /* Nas and voice qmi documents have different perception
                                        for this error QMI_ERR_NO_RADIO. Hence using MODEM_ERR
                                        as a common ground */
                                        res = RIL_E_MODEM_ERR;
                                        break;

                                    case QMI_ERR_NOT_SUPPORTED_V01:
                                    /* This error is sent by the modem when the Request is
                                       "currently" not supported. Using MODEM_ERR as this
                                        pertains to modem not being able to perform the request
                                        currently */
                                        res = RIL_E_MODEM_ERR;
                                        break;

                                    case QMI_ERR_NETWORK_ABORTED_V01:
                                        res = RIL_E_ABORTED;
                                        break;

                                    case QMI_ERR_NO_SUBSCRIPTION_V01:
                                        res = RIL_E_NO_SUBSCRIPTION;
                                        break;

                                    case QMI_ERR_HARDWARE_RESTRICTED_V01:
                                        res = RIL_E_SYSTEM_ERR;
                                        break;

                                    case QMI_ERR_INVALID_QMI_CMD_V01:
                                        res = RIL_E_REQUEST_NOT_SUPPORTED;
                                        break;

                                    default:
                                        res = RIL_E_MODEM_ERR;
                                        break;
                                }
                            }
                            else
                            {
                                res = RIL_E_SYSTEM_ERR;
                            }
                            break;

                        default:
                            res = RIL_E_SYSTEM_ERR;
                            break;
                    }
                }
            }
            break;

        case QMI_INTERNAL_ERR:
            res = RIL_E_SYSTEM_ERR;
            break;

        default:
            res = RIL_E_SYSTEM_ERR;
            break;
    }

    return (RIL_Errno)res;
} // qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex

//===========================================================================
// qcril_qmi_bind_subscription_helper
//===========================================================================
RIL_Errno qcril_qmi_bind_subscription_helper( qcril_qmi_client_e_type svc_type,
                                              unsigned long           msg_id,
                                              void                    *req_c_struct,
                                              int                     req_c_struct_len,
                                              void                    *resp_c_struct,
                                              int                     resp_c_struct_len )
{
    int           i = 0;
    RIL_Errno     res = RIL_E_GENERIC_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        res = qcril_qmi_client_send_msg_sync_ex( svc_type,
                                                 msg_id,
                                                 req_c_struct,
                                                 req_c_struct_len,
                                                 resp_c_struct,
                                                 resp_c_struct_len,
                                                 QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT
                                                  );
        if ( res == RIL_E_SUCCESS )
        {
            QCRIL_LOG_INFO("binding for service - %d success", svc_type);
            break;
        }

        i++;
        QCRIL_LOG_INFO("binding for service - %d failed with err - %d retry count - %d", svc_type, res, i);
        usleep(QCRIL_BIND_RETRY_TIMER); // sleep for 500 ms

    } while(i < QCRIL_MAX_BIND_RETRY_COUNT );

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

//===========================================================================
// qcril_qmi_map_internalerr_from_reqlist_new_to_ril_err
// Maps internal error returned from functions related to creating a
// new element in reqlist to RIL error
//===========================================================================
RIL_Errno qcril_qmi_map_internalerr_from_reqlist_new_to_ril_err(IxErrnoType error)
{
  RIL_Errno ret;
  switch(error)
  {
    case E_SUCCESS:
      ret = RIL_E_SUCCESS;
      break;
    case E_NOT_ALLOWED:
      ret = RIL_E_INVALID_STATE; //needs to be changed after internal discussion
      break;
    case E_NO_MEMORY:
      ret = RIL_E_NO_MEMORY;
      break;
    case E_NO_RESOURCES:
      ret = RIL_E_NO_RESOURCES;
      break;
    case E_RADIO_NOT_AVAILABLE:
      ret = RIL_E_RADIO_NOT_AVAILABLE;
      break;
    case E_INVALID_ARG:
      ret = RIL_E_INVALID_ARGUMENTS;
      break;
    default:
      ret = RIL_E_INTERNAL_ERR;
      break;
  }
  return ret;
} // qcril_qmi_map_internalerr_from_reqlist_new_to_ril_err

//===========================================================================
// qcril_qmi_client_dsds_bind_to_subscription
//===========================================================================
RIL_Errno qcril_qmi_client_dsds_bind_to_subscription( RIL_SubscriptionType sub_num )
{
  char property_name[ PROPERTY_VALUE_MAX ];
  char property_value[ PROPERTY_VALUE_MAX ];
  int result;

  dms_bind_subscription_req_msg_v01 dms_bind_request;
  dms_bind_subscription_resp_msg_v01 dms_bind_resp;
  qmi_lte_bind_subscription_req_msg_v01  lte_bind_request;
  qmi_lte_bind_subscription_resp_msg_v01 lte_bind_resp;

  memset( &dms_bind_request, 0, sizeof( dms_bind_request ) );
  memset( &lte_bind_request, 0, sizeof( lte_bind_request ) );

  QCRIL_LOG_INFO( "sub_num: %d", (int) sub_num );

  auto shared_msg = std::make_shared<StackIdIndMessage>(
        qmi_ril_get_process_instance_id(), sub_num);
  Dispatcher::getInstance().dispatch(shared_msg);

  if (RIL_SUBSCRIPTION_3 == sub_num)
  {
    dms_bind_request.bind_subs = DMS_TERTIARY_SUBS_V01;
    lte_bind_request.subscription = LTE_TERTIARY_SUBSCRIPTION_V01;
  }
  else if (RIL_SUBSCRIPTION_2 == sub_num)
  {
    dms_bind_request.bind_subs = DMS_SECONDARY_SUBS_V01;
    lte_bind_request.subscription = LTE_SECONDARY_SUBSCRIPTION_V01;
  }
  else
  {
    dms_bind_request.bind_subs = DMS_PRIMARY_SUBS_V01;
    lte_bind_request.subscription = LTE_PRIMARY_SUBSCRIPTION_V01;
  }

  // dms
  RIL_Errno res = qcril_qmi_bind_subscription_helper( QCRIL_QMI_CLIENT_DMS,
                                              QMI_DMS_BIND_SUBSCRIPTION_REQ_V01,
                                              (void*)&dms_bind_request,
                                              sizeof( dms_bind_request ),
                                              (void*) &dms_bind_resp,
                                              sizeof( dms_bind_resp )
                                              );
  QCRIL_LOG_INFO( " ..dms %d", (int) res );

  // lted
  if (client_info.client_state[QCRIL_QMI_CLIENT_LTE] == QCRIL_QMI_SERVICE_CONNECTED)
  {
    res =  qcril_qmi_client_send_msg_sync_ex( QCRIL_QMI_CLIENT_LTE,
                                              QMI_LTE_BIND_SUBSCRIPTION_REQ_V01,
                                              &lte_bind_request,
                                              sizeof( lte_bind_request ),
                                              (void*) &lte_bind_resp,
                                              sizeof( lte_bind_resp ),
                                              QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT
                                              );
    QCRIL_LOG_INFO( " ..lte %d", (int) res );
  }
  else
  {
    QCRIL_LOG_INFO( " .. qmi lte is not intialized");
  }

  qcril_qmi_nas_update_modem_stack_id((uint8_t)sub_num);

  QCRIL_SNPRINTF( property_name, sizeof(property_name), "%s%d", QMI_RIL_MULTI_SIM_STACK_ID, qmi_ril_get_process_instance_id() );
  QCRIL_SNPRINTF( property_value, sizeof(property_value), "%d", (int) sub_num);
  result = property_set(property_name, property_value);
  QCRIL_LOG_INFO( ".. [MSIM] stack id prop %s - %s, %d", property_name, property_value, (int) result );
  if ( E_SUCCESS != result)
  {
      QCRIL_LOG_ERROR( "Failed to set stack prop!");
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);

  return res;
}  //  qcril_qmi_client_dsds_bind_to_subscription

RIL_Errno qcril_qmi_client_dsds_cri_client_reinit( RIL_SubscriptionType sub_num )
{
  RIL_Errno res = RIL_E_SUCCESS;
  QCRIL_NOTUSED(sub_num);

  return res;
}

//===========================================================================
// qcril_qmi_client_dsds_cri_client_reset
//===========================================================================
RIL_Errno qcril_qmi_client_dsds_cri_client_reset()
{
  RIL_Errno res = RIL_E_SUCCESS;

  return res;
}  // qcril_qmi_client_dsds_cri_client_reset

//===========================================================================
// qcril_qmi_client_is_available
//===========================================================================
int qcril_qmi_client_is_available(void)
{
  return client_info.qmi_client_init_complete;
} // qcril_qmi_client_is_available

//===========================================================================
// qmi_ril_next_suspending_action
//===========================================================================
void qmi_ril_next_suspending_action(void)
{
    qcril_evt_e_type evt_to_deal_with;

    QCRIL_LOG_FUNC_ENTRY();

    evt_to_deal_with = qmi_ril_modem_reset_suspend_evt_cycle[ qmi_ril_modem_reset_round_info.transition_index ];
    QCRIL_LOG_INFO( " ..evt to deal with 0x%x", evt_to_deal_with );

    if ( QCRIL_EVT_NONE == evt_to_deal_with )
    { // suspend done
        qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID,
                                QCRIL_DEFAULT_MODEM_ID,
                                qmi_ril_suspended_main_threaded,
                                NULL,  // immediate
                                NULL );
    }
    else
    { // suspending - next step
        qmi_ril_modem_reset_round_info.transition_index++;
        qcril_event_queue( qmi_ril_get_process_instance_id(),
                           QCRIL_DEFAULT_MODEM_ID,
                           QCRIL_DATA_ON_STACK,
                           evt_to_deal_with,
                           NULL,
                           QMI_RIL_ZERO,
                           (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
    }
} // qmi_ril_next_suspending_action
//===========================================================================
// qmi_ril_suspended_main_threaded
//===========================================================================
void qmi_ril_suspended_main_threaded(void * param)
{
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED( param );

    qmi_ril_fw_android_request_flow_control_drop_legacy_book_records( FALSE, FALSE );
    qmi_ril_fw_android_request_flow_control_abandon_all_requests_main_thrd( RIL_E_CANCELLED, FALSE );

    QCRIL_LOG_INFO( "QMI RIL suspended" );
    qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDED );
    qcril_qmi_nas_initiate_radio_power_process();

    if (!qcril_qmi_nas_modem_power_is_mdm_shdn_in_apm() ||
          1 == qcril_qmi_modem_power_voting_state())
    {
        qcril_qmi_register_for_up_event();
    }
    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_suspended_main_threaded

//===========================================================================
// qmi_ril_suspending_con_handler
//===========================================================================
void qmi_ril_suspending_con_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
    qcril_modem_restart_con_type * confim_details;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_NOTUSED(ret_ptr);

    if(params_ptr != NULL && params_ptr->data != NULL)
    {
    QCRIL_LOG_INFO( ".. event %d", params_ptr->event_id );

    confim_details = (qcril_modem_restart_con_type *)params_ptr->data;

    QCRIL_LOG_INFO( ".. details 0x%x / %d ", confim_details->evt_originator, confim_details->result );
    if ( RIL_E_SUCCESS == confim_details->result )
    {
        qmi_ril_next_suspending_action();
    }
    else
    {
        QCRIL_LOG_INFO( "SUSPENDING FAILED, RIL halted" );
        qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_HALTED );
    }
    }
    else
    {
        QCRIL_LOG_FATAL("FATAL : CHECK FAILED");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_suspending_con_handler

//===========================================================================
// qmi_ril_core_final_suspend_handler
//===========================================================================
void qmi_ril_core_final_suspend_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED( params_ptr );
    QCRIL_NOTUSED(ret_ptr);

    qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            qmi_ril_final_suspend_main_threaded,
                            NULL,  // immediate
                            NULL );


    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_core_final_suspend_handler
//===========================================================================
// qmi_ril_final_suspend_main_threaded
//===========================================================================
void qmi_ril_final_suspend_main_threaded(void * param)
{
    qcril_modem_restart_con_type outcome;

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED( param );

    qmi_ril_clear_timed_callback_list();


    qcril_qmi_release_client_handles();

    memset( &outcome, 0, sizeof(outcome) );
    outcome.evt_originator = QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_SUSPEND_REQ;
    outcome.result         = RIL_E_SUCCESS;
    qcril_modem_restart_confirm_suspend_resume_step( QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_SUSPEND_CON, &outcome );

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_final_suspend_main_threaded

//===========================================================================
// qcril_modem_restart_confirm_suspend_resume_step
//===========================================================================
void qcril_modem_restart_confirm_suspend_resume_step( qcril_evt_e_type confirmation_evt, qcril_modem_restart_con_type * details )
{

    QCRIL_LOG_INFO( "confirmation_evt: 0x%x", confirmation_evt );
    QCRIL_ASSERT( details != NULL );

    qcril_event_queue( qmi_ril_get_process_instance_id(),
                       QCRIL_DEFAULT_MODEM_ID,
                       QCRIL_DATA_ON_STACK,
                       confirmation_evt,
                       details,
                       sizeof( *details ),
                       (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );

} // qcril_modem_restart_confirm_suspend_resume_step
//===========================================================================
// qmi_ril_enter_suspend
//===========================================================================
void qmi_ril_enter_suspend(void)
{
  qmi_ril_gen_operational_status_type cur_state;
  qmi_ril_gen_operational_status_type next_state;

  QCRIL_LOG_FUNC_ENTRY();
  cur_state = qmi_ril_get_operational_status();
  QCRIL_LOG_INFO( " ..where we are %d", (int) cur_state );
  next_state = QMI_RIL_GEN_OPERATIONAL_STATUS_UNKNOWN;
  qmi_ril_suspend_init_work_handler();

  switch (cur_state)
  {
      case QMI_RIL_GEN_OPERATIONAL_STATUS_UNRESTRICTED:
      case QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_ONGOING:
      case QMI_RIL_GEN_OPERATIONAL_STATUS_RESUMING:
      case QMI_RIL_GEN_OPERATIONAL_STATUS_UNBIND:
          next_state = QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDING;
          break;

      case QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_PENDING:
          // qmi_todo: suspend ini retry logic
          next_state = QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDED;
          break;

      default:
          // fasttrack service
          next_state = QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDED;
          break;
  }

  QCRIL_LOG_INFO( " ..proposed state %d", (int) next_state );
  if ( QMI_RIL_GEN_OPERATIONAL_STATUS_UNKNOWN != next_state )
  {
      qmi_ril_set_operational_status( next_state );
      if (!qcril_qmi_nas_modem_power_is_mdm_shdn_in_apm() ||
          1 == qcril_qmi_modem_power_voting_state())
      {
          qcril_qmi_nas_initiate_radio_state_changed_ind();
          qcril_qmi_nas_embms_send_radio_state(RADIO_STATE_NOT_AVAILABLE_V01);
      }

      switch ( next_state )
      {
          case QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDING:
              qmi_ril_modem_reset_round_info.transition_index = QMI_RIL_ZERO;
              qmi_ril_next_suspending_action();
              break;

          default:
              // nothing
              break;
      }
  }
} // qmi_ril_enter_suspend

//===========================================================================
// qmi_ril_wave_modem_status
//===========================================================================
void qmi_ril_wave_modem_status(void)
{
    qcril_unsol_resp_params_type unsol_resp_params;

    qcril_default_unsol_resp_params( QCRIL_DEFAULT_INSTANCE_ID, RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, &unsol_resp_params );
    qcril_send_unsol_response( &unsol_resp_params );

    qcril_qmi_voice_ims_send_unsol_radio_state_change();

    qcril_qmi_lte_direct_disc_on_radio_state_change();
}

//===========================================================================
// qmi_ril_send_modem_restart_notification
//===========================================================================
void qmi_ril_send_modem_restart_notification(char* reason) {
    QCRIL_LOG_FUNC_ENTRY();
    qcril_unsol_resp_params_type unsol_resp_params;
    qcril_default_unsol_resp_params( QCRIL_DEFAULT_INSTANCE_ID, RIL_UNSOL_MODEM_RESTART, &unsol_resp_params );
    if (reason) {
        unsol_resp_params.resp_pkt = reason;
        unsol_resp_params.resp_len = strlen(reason) + 1;
    } else {
        unsol_resp_params.resp_pkt = (void*) MODEM_RESTART_REASON_DEFAULT;
        unsol_resp_params.resp_len = strlen(MODEM_RESTART_REASON_DEFAULT) + 1;
    }
    qcril_send_unsol_response( &unsol_resp_params );
    QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qmi_ril_enter_resume
//===========================================================================
void qmi_ril_enter_resume(void)
{
  qmi_ril_gen_operational_status_type cur_state;
  qmi_ril_gen_operational_status_type next_state;

  QCRIL_LOG_FUNC_ENTRY();
  cur_state = qmi_ril_get_operational_status();
  QCRIL_LOG_INFO( " ..where we are %d", (int) cur_state );

  // Reset pending unsol list if any
  qmi_ril_reset_android_unsol_resp_dispatchable_table();
  qmi_ril_reset_unsol_resp_pending_list();

  switch (cur_state)
  {
      case QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDED:
          next_state = QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_PENDING;
          break;

      default:
          // no change
          next_state = QMI_RIL_GEN_OPERATIONAL_STATUS_UNKNOWN;
          break;
  }

  QCRIL_LOG_INFO( " ..proposed state %d", (int) next_state );
  if ( QMI_RIL_GEN_OPERATIONAL_STATUS_UNKNOWN != next_state )
  {
      qmi_ril_set_operational_status( next_state );

      switch ( next_state )
      {
          case QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_PENDING:
              qmi_ril_modem_reset_round_info.transition_index = QMI_RIL_ZERO;
              qmi_ril_next_resuming_action();
              break;

          default:
              // nothing
              break;
      }
  }
  else
  {
      qmi_ril_resumer_initiate();
  }
  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_enter_resume

//===========================================================================
// qmi_ril_resumer_initiate
//===========================================================================
RIL_Errno qmi_ril_resumer_initiate(void)
{
    RIL_Errno res;
    pthread_attr_t attr;
    pthread_t      thread_pid;
    int conf;

    QCRIL_LOG_FUNC_ENTRY();

#ifdef QMI_RIL_UTF
    pthread_attr_init (&attr);
    conf = utf_pthread_create_handler(&thread_pid, &attr, qmi_ril_resumer_deferred_action_thread_proc, NULL);
    pthread_attr_destroy(&attr);
#else
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    conf = pthread_create(&thread_pid, &attr, qmi_ril_resumer_deferred_action_thread_proc, NULL);
    pthread_attr_destroy(&attr);
#endif
    qmi_ril_set_thread_name(thread_pid, QMI_RIL_RESUMER_DEFERRED_ACTION_THREAD_NAME);
    QCRIL_LOG_INFO( ".. conf, pid %d, %d", (int)conf, (int) thread_pid );

    res =  (conf < 0 ) ? RIL_E_GENERIC_FAILURE : RIL_E_SUCCESS;

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);

    return res;
} // qmi_ril_resumer_initiate
//===========================================================================
// qmi_ril_resumer_deferred_action_thread_proc
//===========================================================================
void * qmi_ril_resumer_deferred_action_thread_proc(void * param)
{
    qmi_ril_gen_operational_status_type cur_state;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_NOTUSED(param);

    sleep( RESUMER_ACTION_DELAY );
    QCRIL_LOG_INFO( "awaken" );

    cur_state = qmi_ril_get_operational_status();
    QCRIL_LOG_INFO( " ..where we are %d", (int) cur_state );
    switch (cur_state)
    {
        case QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDED:
            qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_PENDING );
            qmi_ril_modem_reset_round_info.transition_index = QMI_RIL_ZERO;
            qmi_ril_next_resuming_action();
            break;

        case QMI_RIL_GEN_OPERATIONAL_STATUS_RESUMING:       // fallthrough
        case QMI_RIL_GEN_OPERATIONAL_STATUS_UNRESTRICTED:
            // no action, no need for resume kick
            break;

        default:
            // we got to wait and see
            qmi_ril_resumer_initiate();
            break;
    }

    QCRIL_LOG_FUNC_RETURN();

    qmi_ril_clear_thread_name(pthread_self());
    return NULL;
} // qmi_ril_resumer_deferred_action_thread_proc

//===========================================================================
// qmi_ril_next_resuming_action
//===========================================================================
void qmi_ril_next_resuming_action(void)
{
    qcril_evt_e_type evt_to_deal_with;

    QCRIL_LOG_FUNC_ENTRY();

    evt_to_deal_with = qmi_ril_modem_reset_resume_evt_cycle[ qmi_ril_modem_reset_round_info.transition_index ];
    QCRIL_LOG_INFO( " ..evt to deal with 0x%x", evt_to_deal_with );

    if ( QCRIL_EVT_NONE == evt_to_deal_with )
    { // resume done, retranslate action to main thread
      qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID,
                                  QCRIL_DEFAULT_MODEM_ID,
                                  qmi_ril_ssr_perform_final_post_ssr_init,
                                  NULL,  // immediate
                                  NULL );
    }
    else
    { // resume - next step
        qmi_ril_modem_reset_round_info.transition_index++;
        qcril_event_queue( qmi_ril_get_process_instance_id(),
                           QCRIL_DEFAULT_MODEM_ID,
                           QCRIL_DATA_ON_STACK,
                           evt_to_deal_with,
                           NULL,
                           QMI_RIL_ZERO,
                           (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
    }

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_next_resuming_action

//===========================================================================
// qmi_ril_ssr_perform_final_post_ssr_init
//===========================================================================
void qmi_ril_ssr_perform_final_post_ssr_init(void * param)
{
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED( param );

    QCRIL_LOG_INFO( "QMI RIL resumed" );

    qcril_qmi_nas_set_bootup_power_optimization_state(QCRIL_POWER_OPT_BOOT_UP_TRIGGER_COMPLETE);
    qmi_ril_wave_modem_status(); // this should trigger reporting of modem state to Android
    qmi_ril_core_init_enter_warp( );

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_ssr_perform_final_post_ssr_init


//===========================================================================
// qmi_ril_resuming_con_handler
//===========================================================================
void qmi_ril_resuming_con_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
    qcril_modem_restart_con_type * confim_details;
    qmi_ril_gen_operational_status_type cur_state;

    const struct timeval retry_delay = { RESUMER_ACTION_DELAY , 0 };

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED(ret_ptr);

    if( params_ptr != NULL && params_ptr->data != NULL)
    {
        QCRIL_LOG_INFO( ".. event %d", params_ptr->event_id );

        confim_details = (qcril_modem_restart_con_type *)params_ptr->data;

        QCRIL_LOG_INFO( ".. details 0x%x / %d ", confim_details->evt_originator, confim_details->result );
        cur_state = qmi_ril_get_operational_status();
        QCRIL_LOG_INFO( " ..where we are %d", (int) cur_state );

        if ( RIL_E_SUCCESS == confim_details->result )
        {
            qmi_ril_next_resuming_action();
        }
        else
        {
            QCRIL_LOG_ERROR( "RESUMING FAILED" );
            switch ( cur_state )
            {
                case QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_RETRY: // rollback, rollback and restart
                    QCRIL_LOG_INFO( "resume retry initiate" );
                    qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID,
                                                QCRIL_DEFAULT_MODEM_ID,
                                                qmi_ril_resume_retry_handler,
                                                &retry_delay,
                                                NULL );
                    break;

                default:
                    QCRIL_LOG_INFO( "RIL halted." );
                    qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_HALTED );
                    break;
            }
        }
    }
    else
    {
        QCRIL_LOG_FATAL("CHECK FAILED");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_resuming_con_handler
//===========================================================================
// qmi_ril_resume_retry_handler
//===========================================================================
void qmi_ril_resume_retry_handler(void * param)
{
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED( param );

    qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_PENDING );
    qmi_ril_modem_reset_round_info.transition_index = QMI_RIL_ZERO;
    qmi_ril_next_resuming_action();

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_resume_retry_handler
//===========================================================================
// qmi_ril_pre_resume_main_threaded
//===========================================================================
void qmi_ril_pre_resume_main_threaded(void * param)
{
    qcril_modem_restart_con_type outcome;

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED( param );

    qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_RESUMING );

    memset( &outcome, 0, sizeof(outcome) );
    outcome.result = qcril_qmi_init_core_client_handles();
    QCRIL_LOG_INFO( ".. outcome of qcril_qmi_init_core_client_handles() - %d", (int) outcome.result );
    if (RIL_E_SUCCESS != outcome.result)
    { // rollback
        qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_RETRY );
    }
    else
    {
        qcril_qmi_nas_dms_commmon_post_init();
    }

    outcome.evt_originator = QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_PRE_RESUME_REQ;
    qcril_modem_restart_confirm_suspend_resume_step( QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_PRE_RESUME_CON, &outcome );

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_pre_resume_main_threaded
//===========================================================================
// qmi_ril_core_pre_resume_handler
//===========================================================================
void qmi_ril_core_pre_resume_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED( params_ptr );
    QCRIL_NOTUSED(ret_ptr);

    qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            qmi_ril_pre_resume_main_threaded,
                            NULL,  // immediate
                            NULL );

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_core_pre_resume_handler

//===========================================================================
// qmi_ril_core_final_resume_handler
//===========================================================================
void qmi_ril_core_final_resume_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
    qcril_modem_restart_con_type outcome;

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED( params_ptr );
    QCRIL_NOTUSED(ret_ptr);

    qcril_qmi_nas_embms_send_radio_state(RADIO_STATE_AVAILABLE_V01);
    // operation mode - go

    memset( &outcome, 0, sizeof(outcome) );
    outcome.evt_originator = QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_RESUME_REQ;
    outcome.result         = RIL_E_SUCCESS;
    qcril_modem_restart_confirm_suspend_resume_step( QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_RESUME_CON, &outcome );

    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_core_final_resume_handler

/*===========================================================================

  FUNCTION: qcril_qmi_check_if_service_is_up

===========================================================================*/
/*!
    @brief
    checks if a service is up

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_check_if_service_is_up
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
    int rc;
    qmi_service_info info;

    qcril_check_service_up_data *qcril_service_up_data = NULL;

    QCRIL_NOTUSED(ret_ptr);

    if (params_ptr)
    {
        qcril_service_up_data = (qcril_check_service_up_data *)params_ptr->data;
    }

    if (qcril_service_up_data &&
        (client_info.client_state[(intptr_t)qcril_service_up_data->cb_data] ==
                                               QCRIL_QMI_SERVICE_NOT_AVAILABLE))
    {
        QCRIL_LOG_INFO("number of active clients %d", client_info.num_of_active_clients);
        if (QMI_NO_ERR == (rc = qmi_client_get_service_instance(qcril_service_up_data->svc_obj,
                                                          qmi_ril_client_get_master_port(),
                                                          &info)))
        {
            client_info.client_state[(intptr_t)qcril_service_up_data->cb_data] =
                                                             QCRIL_QMI_SERVICE_CONNECTED;
            qmi_client_release(client_info.notifier[(intptr_t)qcril_service_up_data->cb_data]);
            client_info.notifier[(intptr_t)qcril_service_up_data->cb_data] = NULL;

            CLIENT_CACHE_LOCK();
            client_info.num_of_active_clients++;
            CLIENT_CACHE_UNLOCK();
        }
        else
        {
            QCRIL_LOG_ERROR("qmi_client_get_service_instance return  %x %d",
                                           qcril_service_up_data->svc_obj, rc);
        }

        CLIENT_CACHE_LOCK();
        if (client_info.num_of_active_clients == client_info.max_active_clients)
        {
            CLIENT_CACHE_UNLOCK();
            QCRIL_LOG_INFO("resuming qcril");
            qmi_ril_enter_resume();
        }
        else
        {
            CLIENT_CACHE_UNLOCK();
        }
    }

    return;
}

/*===========================================================================

  FUNCTION: qcril_qmi_service_up_event

===========================================================================*/
/*!
    @brief
    processes service up event

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_service_up_event
(
  qmi_client_type  clnt,
  qmi_idl_service_object_type   svc_obj,
  qmi_client_notify_event_type  service_event,
  void                         *cb_data
)
{
    qcril_check_service_up_data qcril_service_up_data;
    QCRIL_NOTUSED(clnt);

    memset(&qcril_service_up_data, 0, sizeof(qcril_service_up_data));
    QCRIL_LOG_INFO("qcril_qmi_service_up_event 0x%x %d",
                             (intptr_t) cb_data, service_event);

    if (service_event == QMI_CLIENT_SERVICE_COUNT_INC)
    {
        if (client_info.client_state[(intptr_t)cb_data] == QCRIL_QMI_SERVICE_NOT_AVAILABLE)
        {
            qcril_service_up_data.svc_obj = svc_obj;
            qcril_service_up_data.cb_data = cb_data;

            qcril_event_queue( qmi_ril_get_process_instance_id(),
                               QCRIL_DEFAULT_MODEM_ID,
                               QCRIL_DATA_ON_STACK,
                               QCRIL_EVT_QMI_RIL_MODEM_RESTART_CHECK_IF_SERVICE_UP,
                               &qcril_service_up_data,
                               sizeof(qcril_service_up_data),
                               (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION: qcril_qmi_service_down_event

===========================================================================*/
/*!
    @brief
    processes service down event

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_service_down_event
(
  qmi_client_type       clnt,
  qmi_client_error_type error,
  void                 *error_cb_data
)
{
    int srv_index = (intptr_t) error_cb_data;
    QCRIL_LOG_INFO("qcril_qmi_service_down_event %d", (int) srv_index);

    QCRIL_NOTUSED(clnt);
    QCRIL_NOTUSED(error);
    QCRIL_LOG_INFO("qcril_qmi_service_down_event 0x%x", (intptr_t) error_cb_data);

    if ((client_info.client_state[srv_index] == QCRIL_QMI_SERVICE_CONNECTED) ||
        !client_info.num_of_active_clients)
    {
      qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_DATA_ON_STACK,
                                    QCRIL_EVT_QMI_RIL_SERVICE_DOWN,
                                    &srv_index,
                                    sizeof(srv_index),
                                    (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
    }
}

/*===========================================================================

  FUNCTION: qcril_qmi_handle_event_service_down

===========================================================================*/
/*!
    @brief
    processes service down event

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_handle_event_service_down(
const qcril_request_params_type *const params_ptr,
qcril_request_return_type       *const ret_ptr
)
{
  int srv_index;

  QCRIL_NOTUSED( ret_ptr );

  if(params_ptr)
  {
    srv_index = *((int*)params_ptr->data);

    QCRIL_LOG_INFO("qcril_qmi_handle_event_service_down %d", (int) srv_index);

    if (client_info.num_of_active_clients)
    {
      CLIENT_CACHE_LOCK();

      if (client_info.client_state[srv_index] == QCRIL_QMI_SERVICE_CONNECTED)
      {
          client_info.client_state[srv_index] = QCRIL_QMI_SERVICE_NOT_AVAILABLE;
          client_info.num_of_active_clients--;
      }

      QCRIL_LOG_INFO("number of active clients %d", client_info.num_of_active_clients);
      if (!client_info.num_of_active_clients)
      {
          CLIENT_CACHE_UNLOCK();
          qmi_ril_qmi_client_pre_initialization_acquire();
          qmi_ril_send_modem_restart_notification(NULL);
          qmi_ril_enter_suspend();
      }
      else
      {
          CLIENT_CACHE_UNLOCK()
      }
    }
  }
} // qcril_qmi_handle_event_service_down

/*===========================================================================

  FUNCTION: qcril_qmi_register_for_up_event

===========================================================================*/
/*!
    @brief
    Register for qmi notifier event

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_register_for_up_event
(
    void
)
{
    int                  idx;
    int                  rc = 0;

    QCRIL_LOG_FUNC_ENTRY();
    for (idx = QCRIL_QMI_CLIENT_FIRST; idx < QCRIL_QMI_CLIENT_LAST; idx++)
    {
        //TODO Research: move VOICE out of part of SSR temporarily
        //Do not register up events for services which are not included for SSR.
        if (client_info.client_state[idx] == QCRIL_QMI_SERVICE_NOT_AVAILABLE &&
            (idx != QCRIL_QMI_CLIENT_IMSA && idx != QCRIL_QMI_CLIENT_IMS_SETTING &&
            idx != QCRIL_QMI_CLIENT_VOICE))
        {
            if (!client_info.notifier[idx])
            {
                rc = qmi_client_notifier_init(client_info.service_objects[idx],
                                              &client_info.os_params[idx],
                                              &client_info.notifier[idx]);
                QCRIL_LOG_INFO("qmi_client_notifier_init return %d", (int) rc);
            }

            if (!rc)
            {
                rc = qmi_client_register_notify_cb(client_info.notifier[idx],
                                              qcril_qmi_service_up_event,
                                              (void*)(intptr_t)idx);
                QCRIL_LOG_ERROR("qmi_client_register_notify_cb %d", (int) rc);
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return;
}

void qcril_qmi_release_services()
{
    int iter;
    CLIENT_CACHE_LOCK();
    for (iter = QCRIL_QMI_CLIENT_FIRST; iter < QCRIL_QMI_CLIENT_LAST; iter++)
    {
        if (client_info.client_state[iter] == QCRIL_QMI_SERVICE_CONNECTED)
        {
            //TODO Research: move VOICE out of part of SSR temporarily
            //Do not change state and count for services which are not part of SSR
            if( iter != QCRIL_QMI_CLIENT_IMSA &&
                iter != QCRIL_QMI_CLIENT_IMS_SETTING &&
                iter != QCRIL_QMI_CLIENT_VOICE)
            {
              client_info.client_state[iter] = QCRIL_QMI_SERVICE_NOT_AVAILABLE;
              client_info.num_of_active_clients--;
            }
        }
    }
    CLIENT_CACHE_UNLOCK();
    QCRIL_LOG_INFO("released all services");
    qmi_ril_enter_suspend();
}

//===========================================================================
// qcril_qmi_client_common_empty_unsol_ind_cb
//===========================================================================
void qcril_qmi_client_common_empty_unsol_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
)
{
    QCRIL_NOTUSED(user_handle);
    QCRIL_NOTUSED(ind_buf);
    QCRIL_NOTUSED(ind_buf_len);
    QCRIL_NOTUSED(ind_cb_data);
    QCRIL_LOG_INFO( "msg_id %d", (int) msg_id );
} // qcril_qmi_client_common_empty_unsol_ind_cb

//===========================================================================
// qmi_ril_suspend_init_work_handler
//===========================================================================
void qmi_ril_suspend_init_work_handler()
{
    uint8_t slot = 0;
    QCRIL_LOG_FUNC_ENTRY();

    qmi_ril_reset_android_unsol_resp_dispatchable_table();
    qmi_ril_reset_unsol_resp_pending_list();

    qcril_qmi_nas_cancel_srv_domain_camped_timer_helper();
    qcril_qmi_nas_cancel_wait_for_pbm_ind_timer();
    if (!qcril_qmi_nas_modem_power_is_mdm_shdn_in_apm() ||
        1 == qcril_qmi_modem_power_voting_state())
    {
        qcril_qmi_nas_cancel_radio_power_process(QMI_RIL_DMS_RADIO_PWR_CANCEL_SSR);
    }

    for(slot = 0; slot < qmi_ril_retrieve_number_of_rilds(); slot++)
    {
        qcril_qmi_nas_dms_update_card_status(qmi_ril_get_process_instance_id(), slot, FALSE, QCRIL_CARD_STATUS_UNKNOWN);
    }

    qcril_qmi_arb_reset_pref_data_snapshot();
    qcril_qmi_drop_sig_info_cache();
    qcril_qmi_nas_cancel_sys_sel_pref_tmr();
    QCRIL_LOG_FUNC_RETURN();
} //qmi_ril_suspend_init_work_handler

//===========================================================================
// qcril_qmi_cb_thread_name_init
//===========================================================================
void qcril_qmi_cb_thread_name_init()
{
    RIL_Errno res = RIL_E_GENERIC_FAILURE;
    dms_get_device_mfr_resp_msg_v01 * qmi_response;
    qmi_client_error_type qmi_client_error;
    qmi_txn_handle txn_handle;

    qmi_response = (dms_get_device_mfr_resp_msg_v01*)qcril_malloc( sizeof(*qmi_response) );
    if ( qmi_response )
    {
        qmi_client_error =  qmi_client_send_msg_async( qcril_qmi_client_get_user_handle( QCRIL_QMI_CLIENT_DMS ),
                                                           QMI_DMS_GET_DEVICE_MFR_REQ_V01,
                                                           NULL,
                                                           QMI_RIL_ZERO,
                                                           (void*) qmi_response,
                                                           sizeof( *qmi_response ),
                                                           qcril_qmi_qmi_thread_name_init_cb,
                                                           NULL,
                                                           &txn_handle
                                                            );

        res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_client_error, NULL );
        QCRIL_LOG_INFO("posted req %d, %d", (int) qmi_client_error, (int) res );

        if( RIL_E_SUCCESS != res )
        {
            qcril_free( qmi_response );
        }
    }
} // qcril_qmi_cb_thread_name_init


//===========================================================================
// qcril_qmi_qmi_thread_name_init_cb
//===========================================================================
void qcril_qmi_qmi_thread_name_init_cb
(
  qmi_client_type              user_handle,
  unsigned int                 msg_id,
  void                        *resp_c_struct,
  unsigned int                 resp_c_struct_len,
  void                        *resp_cb_data,
  qmi_client_error_type        transp_err
)
{
    dms_get_device_mfr_resp_msg_v01 * qmi_response   = (dms_get_device_mfr_resp_msg_v01 *) resp_c_struct;
    RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;

    qmi_ril_set_thread_name(pthread_self(), QMI_RIL_QMI_CALLBACK_THREAD_NAME);
    QCRIL_LOG_INFO("transp_err %d", (int) transp_err);

    QCRIL_NOTUSED(user_handle);
    QCRIL_NOTUSED(msg_id);
    QCRIL_NOTUSED(resp_c_struct_len);
    QCRIL_NOTUSED(resp_cb_data);

    if ( qmi_response )
    {
        ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( transp_err, &qmi_response->resp );
        if( RIL_E_SUCCESS == ril_req_res )
        {
            QCRIL_LOG_INFO( "Device Manufacturer : %s",qmi_response->device_manufacturer );
        }
        qcril_free( qmi_response );
    }

    QCRIL_LOG_FUNC_RETURN();
}  // qcril_qmi_qmi_thread_name_init_cb

//===========================================================================
// qmi_ril_qmi_client_get_qmi_service_name
//===========================================================================
const char* qmi_ril_qmi_client_get_qmi_service_name( qcril_qmi_client_e_type qmi_service )
{
  const char * service_name_res = NULL;
  const char * const service_names[QCRIL_QMI_CLIENT_MAX] =
  {
    [QCRIL_QMI_CLIENT_VOICE]        = "QMI Voice",
    [QCRIL_QMI_CLIENT_NAS]          = "QMI NAS",
    [QCRIL_QMI_CLIENT_WMS]          = "QMI WMS",
    [QCRIL_QMI_CLIENT_WDS]          = "QMI WDS",
    [QCRIL_QMI_CLIENT_DMS]          = "QMI DMS",
    [QCRIL_QMI_CLIENT_UIM]          = "QMI UIM",
    [QCRIL_QMI_CLIENT_PBM]          = "QMI PBM",
    [QCRIL_QMI_CLIENT_RF_SAR]       = "QMI RF SAR",
    [QCRIL_QMI_CLIENT_IMS_VT]       = "QMI IMS VT",
    [QCRIL_QMI_CLIENT_IMS_PRESENCE] = "QMI IMS Presence",
    [QCRIL_QMI_CLIENT_IMSA]         = "QMI IMSA",
    [QCRIL_QMI_CLIENT_RFPE]         = "QMI RFPE",
    [QCRIL_QMI_CLIENT_IMS_SETTING]  = "QMI IMS Setting",
    [QCRIL_QMI_CLIENT_PDC]          = "QMI PDC",
    [QCRIL_QMI_CLIENT_DSD]          = "QMI DSD",
    [QCRIL_QMI_CLIENT_LTE]          = "QMI LTE",
  };

  if ( qmi_service < QCRIL_QMI_CLIENT_MAX )
  {
    service_name_res = service_names[ qmi_service ];
  }

  if ( !service_name_res )
  {
    service_name_res = "unknown";
  }

  return service_name_res;
} // qmi_ril_qmi_client_get_qmi_service_name
//===========================================================================
// qmi_ril_qmi_client_log_request
//===========================================================================
void qmi_ril_qmi_client_log_request(
                                    qcril_qmi_client_e_type qmi_service,
                                    int                     message_id,
                                    void*                   message_req_c_struct_payload,
                                    int                     message_req_c_struct_payload_len,
                                    char*                   log_extra
                                    )
{
  const char*                       svc_name;
  uint32_t                          qmi_message_len;
  qmi_idl_service_object_type       svc_object;
  int32_t                           qmi_idl_err;
  char                              log_essence[ QCRIL_MAX_LOG_MSG_SIZE ];
  char                              log_addon[ QCRIL_MAX_LOG_MSG_SIZE ];
  void*                             encoded_qmi_bytestream;
  uint32_t                          encoded_qmi_bytestream_len;

  QCRIL_NOTUSED(log_extra);

  if ( qmi_service < QCRIL_QMI_CLIENT_MAX )
  {
    svc_name   = qmi_ril_qmi_client_get_qmi_service_name ( qmi_service );

    snprintf( log_essence, QCRIL_MAX_LOG_MSG_SIZE,
                "QMI_IO: send to %s msg id %d", svc_name, message_id );

    svc_object = client_info.service_objects [ qmi_service ];

    qmi_idl_err = qmi_idl_get_max_message_len( svc_object, QMI_IDL_REQUEST, message_id, &qmi_message_len );

    if ( QMI_IDL_LIB_NO_ERR == qmi_idl_err )
    {
      encoded_qmi_bytestream = qcril_malloc( qmi_message_len );
      if ( NULL != encoded_qmi_bytestream )
      {
        encoded_qmi_bytestream_len = QMI_RIL_ZERO;
        qmi_idl_err = qmi_idl_message_encode( svc_object,
                                              QMI_IDL_REQUEST,
                                              message_id,
                                              message_req_c_struct_payload,
                                              message_req_c_struct_payload_len,
                                              encoded_qmi_bytestream,
                                              qmi_message_len,
                                              &encoded_qmi_bytestream_len
                                             );
        if ( QMI_IDL_LIB_NO_ERR == qmi_idl_err )
        {
          snprintf( log_addon, QCRIL_MAX_LOG_MSG_SIZE,
                        " encoded len %d", encoded_qmi_bytestream_len );
          strlcat( log_essence, log_addon, sizeof( log_essence ) );

//          qcril_qmi_print_hex( encoded_qmi_bytestream, encoded_qmi_bytestream_len );
        }
        qcril_free( encoded_qmi_bytestream );
      }
    }

    QCRIL_LOG_ESSENTIAL( "%s", log_essence );
  }

} // qmi_ril_qmi_client_log_request

//===========================================================================
// qmi_ril_qmi_client_get_qmi_service_name_from_obj
//===========================================================================
const char* qmi_ril_qmi_client_get_qmi_service_name_from_obj( qmi_idl_service_object_type service_object )
{
  int qmi_service;
  int found;
  const char * service_name_res;

  found = FALSE;
  for ( qmi_service = QCRIL_QMI_CLIENT_FIRST; qmi_service < QCRIL_QMI_CLIENT_LAST; qmi_service++ )
  {
    if ( client_info.service_objects [ qmi_service ] == service_object )
    {
      found = TRUE;
      break;
    }
  }

  if ( found )
  {
    service_name_res = qmi_ril_qmi_client_get_qmi_service_name ((qcril_qmi_client_e_type)qmi_service);
  }
  else
  {
    service_name_res = "unknown";
  }

  return service_name_res;
} // qmi_ril_qmi_client_get_qmi_service_name_from_obj

//===========================================================================
// qmi_ril_qmi_req_log
//===========================================================================
#if 0
void qmi_ril_qmi_req_log( qmi_idl_service_object_type service_object,
                          int message_id,
                          void* encoded_qmi_bytestream,
                          int encoded_qmi_bytestream_len )
{
  QCRIL_LOG_DEBUG( "req %s, msg id: %d, e len:%d",
                       qmi_ril_qmi_client_get_qmi_service_name_from_obj(service_object) ,
                       message_id,
                       encoded_qmi_bytestream_len
                     );

  if ( NULL != encoded_qmi_bytestream && encoded_qmi_bytestream_len > 0 )
  {
    qcril_qmi_print_hex( encoded_qmi_bytestream, encoded_qmi_bytestream_len );
  }
} // qmi_ril_qmi_req_log

//===========================================================================
// qmi_ril_qmi_resp_log
//===========================================================================
void qmi_ril_qmi_resp_log( qmi_idl_service_object_type service_object,
                           int message_id,
                           void* encoded_qmi_bytestream,
                           int encoded_qmi_bytestream_len )
{
  QCRIL_LOG_DEBUG( "rsp - %s, msg id: %d, e len:%d",
                       qmi_ril_qmi_client_get_qmi_service_name_from_obj(service_object) ,
                       message_id,
                       encoded_qmi_bytestream_len
                     );

  if ( NULL != encoded_qmi_bytestream && encoded_qmi_bytestream_len > 0 )
  {
    qcril_qmi_print_hex( encoded_qmi_bytestream, encoded_qmi_bytestream_len );
  }
} // qmi_ril_qmi_resp_log

//===========================================================================
// qmi_ril_qmi_ind_log
//===========================================================================
void qmi_ril_qmi_ind_log( int service_id,
                          int message_id,
                          void* encoded_qmi_bytestream,
                          int encoded_qmi_bytestream_len )
{
  char * svc_name;

  switch ( service_id )
  {
    case QMI_WDS_SERVICE:
      svc_name = "QMI WDS";
      break;

    case QMI_DMS_SERVICE:
      svc_name = "QMI DMS";
      break;

    case QMI_NAS_SERVICE:
      svc_name = "QMI NAS";
      break;

    case QMI_QOS_SERVICE:
      svc_name = "QMI QOS";
      break;

    case QMI_WMS_SERVICE:
      svc_name = "QMI WMS";
      break;

    case QMI_VOICE_SERVICE:
      svc_name = "QMI Voice";
      break;

    case QMI_CAT_SERVICE:
      svc_name = "QMI CAT";
      break;

    case QMI_UIM_SERVICE:
      svc_name = "QMI UIM";
      break;

    case QMI_PBM_SERVICE:
      svc_name = "QMI PBM";
      break;

    case QMI_SAR_SERVICE:
      svc_name = "QMI SAR";
      break;

    case QMI_IMS_VIDEO_SERVICE:
      svc_name = "QMI IMS Video";
      break;

    case QMI_IMS_VT_SERVICE:
      svc_name = "QMI IMS VT";
      break;

    case QMI_IMS_PRESENCE_SERVICE:
      svc_name = "QMI IMS Presence";
      break;

    case QMI_RFPE_SERVICE:
      svc_name = "QMI RFPE";
      break;

    case 0x46: //QMI_LTE_SERVICE
      svc_name = "QMI LTE";
      break;

    default:
      svc_name = "unknown";
      break;
  }

  QCRIL_LOG_DEBUG( "ind %s, msg id: %d, e len:%d",
                       svc_name,
                       message_id,
                       encoded_qmi_bytestream_len
                     );

  if ( NULL != encoded_qmi_bytestream && encoded_qmi_bytestream_len > 0 )
  {
    qcril_qmi_print_hex( encoded_qmi_bytestream, encoded_qmi_bytestream_len );
  }
} // qmi_ril_qmi_ind
#endif

//===========================================================================
// qmi_ril_qmi_client_pre_initialization_init
//===========================================================================
void qmi_ril_qmi_client_pre_initialization_init()
{
    QCRIL_LOG_DEBUG("entry");

    qmi_ril_enter_critical_section();
    qmi_ril_qmi_client_pre_initialization_get();
    qmi_ril_leave_critical_section();

    QCRIL_LOG_DEBUG("exit");
} //qmi_ril_qmi_client_pre_initialization_init

//===========================================================================
// qmi_ril_qmi_client_pre_initialization_acquire
//===========================================================================
void qmi_ril_qmi_client_pre_initialization_acquire()
{
    QCRIL_LOG_ESSENTIAL("entry");

    qmi_ril_enter_critical_section();
    if(FALSE == qmi_ril_qmi_client_pre_initialization_lock_held &&
       TRUE == qmi_ril_qmi_client_pre_init_lock_acq_decision)
    {
        acquire_wake_lock( PARTIAL_WAKE_LOCK, QCRIL_PRE_CLIENT_INIT_WAKE_LOCK_NAME );
        qmi_ril_qmi_client_pre_initialization_lock_held = TRUE;
        qmi_ril_qmi_client_pre_initialization_set();
    }
    qmi_ril_leave_critical_section();

    QCRIL_LOG_ESSENTIAL("exit");
} //qmi_ril_qmi_client_pre_initialization_acquire

//===========================================================================
// qmi_ril_qmi_client_pre_initialization_release
//===========================================================================
void qmi_ril_qmi_client_pre_initialization_release()
{
    QCRIL_LOG_ESSENTIAL("entry");

    qmi_ril_enter_critical_section();
    if(TRUE == qmi_ril_qmi_client_pre_initialization_lock_held)
    {
        release_wake_lock( QCRIL_PRE_CLIENT_INIT_WAKE_LOCK_NAME );
        qmi_ril_qmi_client_pre_initialization_lock_held = FALSE;
        qmi_ril_qmi_client_pre_initialization_set();
    }
    qmi_ril_leave_critical_section();

    QCRIL_LOG_ESSENTIAL("exit");
} //qmi_ril_qmi_client_pre_initialization_release

//===========================================================================
// qmi_ril_qmi_client_pre_initialization_set
//===========================================================================
void qmi_ril_qmi_client_pre_initialization_set()
{
    char args[ PROPERTY_VALUE_MAX ];

    QCRIL_LOG_INFO( "qmi_ril_qmi_client_pre_initialization_lock_held %d", qmi_ril_qmi_client_pre_initialization_lock_held );

    QCRIL_SNPRINTF( args, sizeof( args ), "%d", qmi_ril_qmi_client_pre_initialization_lock_held );

    if ( property_set( QCRIL_PRE_CLIENT_INIT_WAKE_LOCK_HELD_PROP, args ) != E_SUCCESS )
    {
      QCRIL_LOG_ERROR( "Fail to save %s to system property", QCRIL_PRE_CLIENT_INIT_WAKE_LOCK_HELD_PROP );
    }
    else
    {
      QCRIL_LOG_INFO("Set %s property to %s", QCRIL_PRE_CLIENT_INIT_WAKE_LOCK_HELD_PROP, args);
    }
} //qmi_ril_qmi_client_pre_initialization_set

//===========================================================================
// qmi_ril_qmi_client_pre_initialization_get
//===========================================================================
void qmi_ril_qmi_client_pre_initialization_get()
{
    int temp_len;
    unsigned long ret_val;
    char args[ PROPERTY_VALUE_MAX ];
    char baseband[ PROPERTY_VALUE_MAX ];
    char *end_ptr;

    qmi_ril_qmi_client_pre_initialization_lock_held = FALSE;
    property_get( QCRIL_PRE_CLIENT_INIT_WAKE_LOCK_HELD_PROP, args, "" );
    property_get( QMI_RIL_SYS_PROP_NAME_BASEBAND, baseband, "" );
    temp_len = strlen( args );

    if(!strncmp(baseband, "apq", 3))
    {
        qmi_ril_qmi_client_pre_init_lock_acq_decision = FALSE;
    }
    else
    {
        qmi_ril_qmi_client_pre_init_lock_acq_decision = TRUE;
    }

    if ( temp_len > 0 )
    {
      ret_val = strtoul( args, &end_ptr, 0 );
      if ( ( errno == ERANGE ) && ( ret_val == ULONG_MAX ) )
      {
        QCRIL_LOG_ERROR( "Fail to convert QCRIL_PRE_CLIENT_INIT_WAKE_LOCK_HELD_PROP %s", args );
      }
      else if ( ret_val > 1 )
      {
        QCRIL_LOG_ERROR( "Invalid saved QCRIL_PRE_CLIENT_INIT_WAKE_LOCK_HELD_PROP %ld, use default", ret_val );
      }
      else
      {
        qmi_ril_qmi_client_pre_initialization_lock_held = ( uint8 ) ret_val;
      }
    }
    QCRIL_LOG_DEBUG( "QCRIL_PRE_CLIENT_INIT_WAKE_LOCK_HELD_PROP=%d", qmi_ril_qmi_client_pre_initialization_lock_held);
} //qmi_ril_qmi_client_pre_initialization_get


