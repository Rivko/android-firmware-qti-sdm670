/******************************************************************************
#  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril.c
  @brief   qcril qmi core

  DESCRIPTION


******************************************************************************/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <signal.h>

#include <cutils/properties.h>
#include "ril.h"
#include "IxErrno.h"
#include <framework/qcril_event.h>
#include <translators/android/utils.h>
#include "qcrili.h"
#include "modules/nas/qcril_arb.h"
#include "qcril_log.h"
#include "qcril_reqlist.h"
#include "qcril_other.h"
#include "qcril_qmi_client.h"
#include "modules/nas/qcril_db.h"

#include "qcril_pbm.h"
#include "modules/nas/qcril_qmi_nas.h"
#include "modules/nas/qcril_qmi_prov.h"
#include "modules/nas/qcril_qmi_nas2.h"
#include "qcril_qmi_voice.h"
#include "qcril_qmi_pdc.h"
#include "qcril_mbn_hw_update.h"
#include "qcril_mbn_sw_update.h"

#include "modules/ims/qcril_qmi_ims_radio_agent.h"
#include "modules/ims/qcril_qmi_ims_flow_control.h"
#include "modules/ims/qcril_qmi_ims_misc.h"

#include "qcril_qmi_oem_events.h"
#include "qcril_qmi_oemhook_agent.h"
#include "qcril_qmi_oem_packing.h"
#include "qcril_qmi_oem_reqlist.h"

#include "modules/sms/qcril_qmi_sms.h"

#include "qcril_qmi_lte_direct_disc.h"

#include "mdm_detect.h"
#include "qcril_am.h"
#include "qmi_ril_peripheral_mng.h"

#include "qmi_ril_file_observer.h"

#include "qcril_qmi_radio_config_dispatch_helper.h"
#include "qcril_qmi_radio_config_socket.h"
#include "settings.h"

#ifdef QMI_RIL_UTF
void qcril_qmi_hal_uim_module_cleanup();
#endif
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_DECOMPOSE_QUEUE_SZ(x)   ( ((uint32_t)x & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_QUEUE_SZ_MASK) )

#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( caps_mask, queue_sz )   ( ((uint32_t)queue_sz & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_QUEUE_SZ_MASK) | caps_mask )

#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_SINGLE (1)
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_MULTIPLE_DEF (16)

#define QMI_RIL_FW_FLOW_CONTROL_CMP_MONIKERS_OLDER  (1)
#define QMI_RIL_FW_FLOW_CONTROL_CMP_MONIKERS_NEWER  (-1)
#define QMI_RIL_FW_FLOW_CONTROL_CMP_MONIKERS_EQUAL  (0)



#define FLOWCONTROL_ONREQUEST_NONE                                 (0)
#define FLOWCONTROL_ONREQUEST_DISPATCHED                           (1)
#define FLOWCONTROL_ONREQUEST_FLOWCONTROL_DEFERRED_BUSY            (2)
#define FLOWCONTROL_ONREQUEST_FLOWCONTROL_REJECTED_OVERFLOW        (4)
#define FLOWCONTROL_ONREQUEST_FLOWCONTROL_EXEMPT                   (8)
#define FLOWCONTROL_ONREQUEST_DISPATCHED_CMD_MAIN_THRD             (16)
#define FLOWCONTROL_ONREQUEST_DISPATCHED_CMD_DEDICATED_THRD        (32)


#define QCRIL_REG_ALL_STATES( event_id, handler )          \
  NULL, (uint32) event_id, handler, QCRIL_STATE_MASK_ALL_STATES

#define QCRIL_REG_ALL_ACTIVE_STATES( event_id, handler ) \
  NULL, (uint32) event_id, handler, QCRIL_STATE_MASK_ALL_ACTIVE_STATES

#define QCRIL_REG_SIM_OR_RUIM_READY_STATES( event_id, handler ) \
  NULL, (uint32) event_id, handler, QCRIL_STATE_MASK_SIM_OR_RUIM_READY_STATES

#define QCRIL_HT_ENTRIES_MAX 100

#define QCRIL_MDM_LOCK()      pthread_mutex_lock(&esoc_info.mdm_mutex);
#define QCRIL_MDM_UNLOCK()    pthread_mutex_unlock(&esoc_info.mdm_mutex);

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/* QCRIL internal info */
qcril_arb_state_struct_type *qcril_state = NULL;

/* QCRIL timer id */
pthread_mutex_t qcril_timer_id_mutex; /*!< Mutex to control access/update of QCRIL Timer ID*/
static uint16 qcril_timer_id;         /*!< Next QCRIL Timer ID */

/* QCRIL timer list */
pthread_mutex_t qcril_timed_callback_list_mutex;
static qcril_timed_callback_info *qcril_timed_callback_list = NULL;

static qmi_ril_atomic_lock_info_type qmi_ril_common_critical_section;

/* QCRIL Heap Memory list*/
qcril_heap_list_info *qcril_heap_memory_list = NULL;
uint32 heap_memory_id;

static boolean is_heap_memory_tracked;
static pthread_mutex_t qcril_heap_memory_list_mutex;
#define QCRIL_TRACK_HEAP_MEM             "persist.vendor.radio.track_heap_mem"

/* Time (1 second) to wait for the completion of modem restart before re-initiate QCRIL */
static const struct timeval TIMEVAL_DELAY = {1,0};
static const struct timeval HEAP_MEM_LIST_PRINT_TIMEVAL_DELAY = {60,0};

#define QCRIL_REQUEST_SUPPRESS_MAX_LEN 4
static qmi_ril_suppress_event_type qcril_request_suppress_list[QCRIL_REQUEST_SUPPRESS_MAX_LEN];

/* QCRIL request supress list mutex. */
pthread_mutex_t      qcril_request_supress_list_mutex;
pthread_mutexattr_t  qcril_request_supress_list_mutex_attr;

#define QMI_RIL_INI_RETRY_GAP_SEC     1

static qcril_instance_id_e_type            qmi_ril_process_instance_id;
static uint32_t                            qmi_ril_sim_slot; ///< sim slot associated w/this ril instance
static pthread_t                           qmi_ril_init_retry_thread_pid;

#define OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE    4
#define OEM_HOOK_QMI_TUNNELING_RESP_RESP_SZ_SIZE       4
#define OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE             2
#define OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE             2

#define OEM_HOOK_QMI_TUNNELING_REQ_MODEM_ID_SIZE       8
#define OEM_HOOK_QMI_TUNNELING_REQ_MSG_SIZE            4
#define OEM_HOOK_QMI_TUNNELING_ERROR_CODE_SIZE 2

#define OEM_HOOK_QMI_TUNNELING_RESP_OVERHEAD_SIZE      ( OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE +              \
                                                         OEM_HOOK_QMI_TUNNELING_RESP_RESP_SZ_SIZE +                    \
                                                         OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE +                               \
                                                         OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE +                               \
                                                         OEM_HOOK_QMI_TUNNELING_ERROR_CODE_SIZE )

#define OEM_HOOK_QMI_TUNNELING_RESULT_SIZE   7

#define OEM_HOOK_QMI_TUNNELING_IND_Q_STR_LEN_SIZE      (QCRIL_OTHER_OEM_NAME_LENGTH)
#define OEM_HOOK_QMI_TUNNELING_IND_EVENT_ID_SIZE       4
#define OEM_HOOK_QMI_TUNNELING_IND_PLD_LEN_SIZE        4

#define OEM_HOOK_QMI_TUNNELING_IND_OVERHEAD_SIZE       ( OEM_HOOK_QMI_TUNNELING_IND_Q_STR_LEN_SIZE + OEM_HOOK_QMI_TUNNELING_IND_EVENT_ID_SIZE + \
                                                         OEM_HOOK_QMI_TUNNELING_IND_PLD_LEN_SIZE + OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE + OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE )

#define MEM_PADDING "QCRILQMIqcrilqmi"
#define MEM_PADDING_SIZE 16

/* QCRIL android pending unsol resp */
typedef struct qmi_ril_android_pending_unsol_resp_type
{
  boolean valid;
  const char *logstr;
  uint8 instance_id;

  qmi_ril_fw_android_param_copy_approach_type param_copy_arrron;
  void* copied_params;
  size_t copied_param_len;

  int8 num_of_locker;
} qmi_ril_android_pending_unsol_resp_type;

typedef struct
{
  pthread_mutex_t pending_unsol_resp_mutex;
  qmi_ril_android_pending_unsol_resp_type* pending_unsol_resp_list;
} qmi_ril_android_pending_unsol_resp_overview_type;

typedef struct esoc_mdm_info {
    boolean                             pm_feature_supported;
    boolean                             esoc_feature_supported;
    int                                 esoc_fd;
    int                                 voting_state; // 0 - vote released; 1 - vote activated
    char                                link_name[MAX_NAME_LEN];
    char                                modem_name[MAX_NAME_LEN];
    char                                esoc_node[MAX_NAME_LEN];
    pthread_mutex_t                     mdm_mutex;
    pthread_mutexattr_t                 mdm_attr;
    MdmType                             type;
} qcril_mdm_info;

static qmi_ril_android_pending_unsol_resp_overview_type qmi_ril_android_pending_unsol_resp;

static void qcril_request_suppress_list_init();

static uint32_t qmi_ril_fw_android_request_get_handling_capabilities( int android_request_id );


/* EVENT REFACTOR - flow control
static void qmi_ril_fw_android_request_flow_control_trigger_remains(qcril_timed_callback_handler_params_type * handler_params);
*/

qmi_ril_fw_android_request_flow_control_overview_type qmi_ril_fw_android_request_flow_control_overview;
int qmi_ril_fw_android_request_flow_control_moniker_compare( uint32_t moniker1, uint32_t moniker2 );

static RIL_Errno qmi_ril_fw_android_request_render_execution( RIL_Token token,
                                                       qcril_evt_e_type android_request_id,
                                                       void * android_request_data,
                                                       int android_request_data_len,
                                                       qcril_instance_id_e_type  instance_id,
                                                       int * is_dedicated_thread);

static boolean qmi_ril_check_android_unsol_resp_dispatchable(int resp_id);
static void qmi_ril_init_android_unsol_resp_pending_list()
__attribute__((constructor(100)));

static void qmi_ril_android_pending_unsol_resp_lock( void );
static void qmi_ril_android_pending_unsol_resp_unlock( void );

static void qmi_ril_add_unsol_resp_to_pending_list(qcril_unsol_resp_params_type *param_ptr);
static qmi_ril_android_pending_unsol_resp_type* qmi_ril_get_unsol_resp_from_pending_list(int resp_id);
static void qmi_ril_free_pending_unsol_resp(qmi_ril_android_pending_unsol_resp_type* resp, int resp_id);
static void qmi_ril_bootup_actition_on_rild_atel_link_connect(void * params);
static boolean qcril_qmi_is_pm_voting_feature_supported();

qmi_ril_fw_android_request_kind_execution_overview_type* qmi_ril_fw_android_request_flow_control_find_busy_kind( qmi_ril_fw_android_request_kind_execution_overview_type* origin );

qmi_ril_fw_android_request_holder_type * qmi_ril_fw_android_request_flow_control_find_outstanding_request_within_kind( qmi_ril_fw_android_request_kind_execution_overview_type* origin );

int qcril_qmi_modem_power_voting_state();
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES


===========================================================================*/

void RIL_removeTimedCallback(void *param);

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES


===========================================================================*/

static void onRequest_rid( int request, void *data, size_t datalen, RIL_Token t );
static RIL_RadioState currentState_rid();
static int onSupports_rid( int request );
static void onCancel_rid( RIL_Token t );
static const char *getVersion_rid( void );

static RIL_Errno qmi_ril_core_init(void);
static RIL_Errno qmi_ril_initiate_core_init_retry(void);
static void * qmi_ril_core_init_kicker_thread_proc(void* empty_param);
static void qmi_ril_initiate_bootup(void);
static void qmi_ril_bootup_perform_core_or_start_polling(void * params);
static void qmi_ril_core_init_kicker_main_threaded_proc(void* empty_param);
// EVENT REFACTOR cleanup static void qcril_free_request_list_entry_deferred( qcril_timed_callback_handler_params_type * handler_params );

void qmi_ril_fw_android_request_flow_control_init();
void qmi_ril_fw_android_request_flow_control_abandon_requests_local_and_ring_zoned( qmi_ril_fw_android_request_kind_execution_overview_type* origin, RIL_Errno cause, int is_unbind_cleanup );
void qmi_ril_fw_android_request_flow_control_declare_family_ring( const int * android_request_ids );
void qmi_ril_fw_android_request_flow_control_overview_request_review_holders( qmi_ril_fw_android_request_kind_execution_overview_type* origin );
int qmi_ril_fw_android_request_flow_control_request_holders_have_same_param( qmi_ril_fw_android_request_holder_type* origin, qmi_ril_fw_android_request_holder_type* peer );

pthread_t qmi_ril_fw_get_main_thread_id();

static void qmi_ril_oem_hook_init();

boolean qcril_qmi_modem_power_is_voting_feature_supported();

static const RIL_RadioFunctions qcril_request_api[] = {
  { QCRIL_RIL_VERSION, onRequest_rid, currentState_rid, onSupports_rid, onCancel_rid, getVersion_rid }
};

static pthread_t qmi_ril_main_thread_id;

/* esoc info */
qcril_mdm_info                          esoc_info;

#ifdef RIL_SHLIB
struct RIL_Env *qcril_response_api[ QCRIL_MAX_INSTANCE_ID ]; /*!< Functions for ril to call */
#endif /* RIL_SHLIB */


/*===========================================================================

                                FUNCTIONS

===========================================================================*/


//===========================================================================
// qmi_ril_clear_timed_callback_list
//===========================================================================
void qmi_ril_clear_timed_callback_list()
{
  qcril_timed_callback_info *cur = NULL;
  qcril_timed_callback_info *next = NULL;
  QCRIL_MUTEX_LOCK( &qcril_timed_callback_list_mutex, "timed callback list mutex");
  cur = qcril_timed_callback_list;

  while ( NULL != cur )
  {
    if (cur->need_free && NULL != cur->extra_params)
    {
      qcril_free(cur->extra_params);
    }
    next = cur->next;
    qcril_free(cur);
    cur = next;
  }

  qcril_timed_callback_list = NULL;
  QCRIL_MUTEX_UNLOCK( &qcril_timed_callback_list_mutex, "timed callback list mutex");

  // add heap memory print callback back
  if ( 1 == is_heap_memory_tracked )
  {
    qcril_setup_timed_callback(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID,
                               qcril_print_heap_memory_list, &HEAP_MEM_LIST_PRINT_TIMEVAL_DELAY, NULL );
  }

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_clear_timed_callback_list

//===========================================================================
// qmi_ril_is_multi_sim_feature_supported
//===========================================================================
int qmi_ril_is_multi_sim_feature_supported()
{
   return ( qmi_ril_is_feature_supported(QMI_RIL_FEATURE_DSDS) ||
            qmi_ril_is_feature_supported(QMI_RIL_FEATURE_TSTS) );
} // qmi_ril_is_multi_sim_feature_supported

//===========================================================================
// qmi_ril_is_qcom_ril_version_supported
//===========================================================================
int qmi_ril_is_qcom_ril_version_supported(int version)
{
  int res = FALSE;

  #if defined(RIL_QCOM_VERSION)
  if( version > QMI_RIL_ZERO && RIL_QCOM_VERSION >= version )
  {
    res = TRUE;
  }
  #else
  QCRIL_NOTUSED(version);
  #endif

  return res;
} //qmi_ril_is_qcom_ril_version_supported

//===========================================================================
// ril_to_uim_is_tsts_enabled
//===========================================================================
int ril_to_uim_is_tsts_enabled(void)
{
  return qmi_ril_is_feature_supported( QMI_RIL_FEATURE_TSTS );
} // ril_to_uim_is_tsts_enabled

//===========================================================================
// ril_to_uim_is_dsds_enabled
//===========================================================================
int ril_to_uim_is_dsds_enabled(void)
{
  return (qmi_ril_is_feature_supported( QMI_RIL_FEATURE_DSDS ));
} // ril_to_uim_is_dsds_enabled

qcril_timed_callback_info **_qcril_find_timed_callback_locked(uint32 timer_id)
{
  qcril_timed_callback_info **i;

  for (i = &qcril_timed_callback_list; *i ; i = &((*i)->next)) {
    if ((*i)->timer_id == timer_id) {
      break;
    }
  }

  return i;

}
qcril_timed_callback_info *qcril_find_and_remove_timed_callback(uint32 timer_id)
{
  qcril_timed_callback_info **i = NULL, *ret = NULL;
  QCRIL_MUTEX_LOCK( &qcril_timed_callback_list_mutex, "timed callback list mutex");

  i = _qcril_find_timed_callback_locked(timer_id);

  ret = *i;
  if ( NULL != ret )
  {
    if ( NULL != ret->next )
    {
      ret->next->prev = ret->prev;
    }
    *i = ret->next;

    ret->next = NULL;
    ret->prev = NULL;
  }

  QCRIL_MUTEX_UNLOCK( &qcril_timed_callback_list_mutex, "timed callback list mutex");
  return ret;

}

void qcril_add_timed_callback(qcril_timed_callback_info *info)
{
  qcril_timed_callback_info **i;
  qcril_timed_callback_info *prev = NULL;
  QCRIL_MUTEX_LOCK( &qcril_timed_callback_list_mutex, "timed callback list mutex");

  for (i = &qcril_timed_callback_list; *i; i = &((*i)->next))
  {
    prev = *i;
  }
  *i = info;
  info->next = NULL;
  info->prev = prev;
  QCRIL_MUTEX_UNLOCK( &qcril_timed_callback_list_mutex, "timed callback list mutex");
}

/*=========================================================================
  FUNCTION:  qcril_timed_callback_dispatch

===========================================================================*/
/*!
    @brief
    Dispatch function for all timed callbacks

    @return
    void
*/
/*=========================================================================*/
void qcril_timed_callback_dispatch
(
  void *param
)
{
  uint32            timer_id = (uintptr_t) param;
  RIL_TimedCallback cb;
  qcril_timed_callback_info *info = qcril_find_and_remove_timed_callback(timer_id);

  if (info)
  {
    cb = info->callback;
    cb((void *)(uintptr_t)timer_id);
    qcril_free(info);
  }
} /* qcril_timed_callback_dispatch */

/*=========================================================================
  FUNCTION:  qcril_setup_timed_callback

===========================================================================*/
/*!
    @brief
    Setup RIL callback timer

    @return
    0 on success.
*/
/*=========================================================================*/
int qcril_setup_timed_callback
(
  qcril_instance_id_e_type instance_id,
  qcril_modem_id_e_type modem_id,
  RIL_TimedCallback callback,
  const struct timeval *relativeTime,
  uint32 *timer_id
)
{
  qcril_timed_callback_info *tcbinfo = NULL;
  IxErrnoType ret = E_FAILURE;
  uint32 the_timer_id;

  if ((instance_id < QCRIL_MAX_INSTANCE_ID) && (modem_id < QCRIL_MAX_MODEM_ID))
  {
      tcbinfo = qcril_malloc(sizeof(qcril_timed_callback_info));
      if (tcbinfo)
      {

        QCRIL_MUTEX_LOCK( &qcril_timer_id_mutex, "qcril_timer_id_mutex" );

        /* Most Significant 16 bits are the Instance ID + Modem ID and Least Significant 16 bits are the QCRIL Timer ID */
        the_timer_id = ( uint32 ) QCRIL_COMPOSE_USER_DATA( instance_id, modem_id, qcril_timer_id );
        qcril_timer_id++;
        if( 0 == qcril_timer_id )
        {
            qcril_timer_id = 1;
        }

        QCRIL_MUTEX_UNLOCK( &qcril_timer_id_mutex, "qcril_timer_id_mutex" );

        tcbinfo->timer_id = the_timer_id;
        tcbinfo->callback = callback;

        qcril_add_timed_callback(tcbinfo);

        if ( relativeTime != NULL )
        {
          QCRIL_LOG_DEBUG( "Sec - %d usec - %d", relativeTime->tv_sec, relativeTime->tv_usec);
        }
        else
        {
          QCRIL_LOG_DEBUG("Immediate call back");
        }

        qcril_response_api[ instance_id ]->RequestTimedCallback( qcril_timed_callback_dispatch,
                                                                 (void *)(uintptr_t)the_timer_id, relativeTime );

        QCRIL_LOG_DEBUG( "Set timer with ID %d", the_timer_id );

        if (timer_id)
        {
          *timer_id = the_timer_id;
        }
        ret = E_SUCCESS;
      }
      else
      {
        QCRIL_LOG_ERROR("Memory allocation failed..");
        ret = E_NO_MEMORY;
      }
  }
  else
  {
    QCRIL_LOG_ERROR("Internal error(E_FAILURE)..invalid instance_id %d or modem_id %d",
                    instance_id, modem_id);
    ret = E_FAILURE;
  }

  return ret;
} /* qcril_setup_timed_callback */

//===========================================================================
// qcril_timed_callback_dispatch_expra_params
//===========================================================================
static void qcril_timed_callback_dispatch_expra_params
(
  void *param
)
{
  uint32                                    timer_id = (uint32)(uintptr_t) param;
  qcril_timed_callback_type                 cb;
  qcril_timed_callback_info *               info = qcril_find_and_remove_timed_callback(timer_id);
  qcril_timed_callback_handler_params_type  handler_params;

  if (info)
  {
    memset( &handler_params, 0, sizeof( handler_params ) );
    handler_params.timer_id     = timer_id;
    handler_params.custom_param = info->extra_params;

    cb                          = info->callback;

    cb( &handler_params );

    qcril_free( info );
  }
} // qcril_timed_callback_dispatch_expra_params

//===========================================================================
// qcril_setup_timed_callback_ex_params
//===========================================================================
int qcril_setup_timed_callback_ex_params
(
  qcril_instance_id_e_type      instance_id,
  qcril_modem_id_e_type         modem_id,
  qcril_timed_callback_type     callback,
  void*                         extra_params,
  const struct timeval *        relativeTime,
  uint32 *                      timer_id
)
{
  return qcril_setup_timed_callback_ex_params_adv(instance_id, modem_id, callback, extra_params, FALSE, relativeTime, timer_id);
}

//===========================================================================
// qcril_setup_timed_callback_ex_params_adv
//===========================================================================
int qcril_setup_timed_callback_ex_params_adv
(
  qcril_instance_id_e_type      instance_id,
  qcril_modem_id_e_type         modem_id,
  qcril_timed_callback_type     callback,
  void*                         extra_params,
  boolean                       need_free,
  const struct timeval *        relativeTime,
  uint32 *                      timer_id
)
{
  qcril_timed_callback_info *tcbinfo = NULL;
  int ret = -1;
  uint32 the_timer_id;


  if ((instance_id < QCRIL_MAX_INSTANCE_ID) && (modem_id < QCRIL_MAX_MODEM_ID))
  {
      tcbinfo = qcril_malloc(sizeof(qcril_timed_callback_info));
      if (tcbinfo)
      {

        QCRIL_MUTEX_LOCK( &qcril_timer_id_mutex, "qcril_timer_id_mutex" );

        /* Most Significant 16 bits are the Instance ID + Modem ID and Least Significant 16 bits are the QCRIL Timer ID */
        the_timer_id = ( uint32 ) QCRIL_COMPOSE_USER_DATA( instance_id, modem_id, qcril_timer_id );
        qcril_timer_id++;
        if( 0 == qcril_timer_id )
        {
            qcril_timer_id = 1;
        }

        QCRIL_MUTEX_UNLOCK( &qcril_timer_id_mutex, "qcril_timer_id_mutex" );

        tcbinfo->timer_id     = the_timer_id;
        tcbinfo->callback     = (void*)callback;
        tcbinfo->extra_params = extra_params;
        tcbinfo->need_free    = need_free;

        qcril_add_timed_callback(tcbinfo);

        qcril_response_api[ instance_id ]->RequestTimedCallback( qcril_timed_callback_dispatch_expra_params,
                                                                 (void *)(uintptr_t)the_timer_id,
                                                                 relativeTime );

        QCRIL_LOG_DEBUG( "Set timer with ID %d. extra_params: %p. need_free: %s", the_timer_id, extra_params, need_free ? "true" : "false" );

        if (timer_id)
        {
          *timer_id = the_timer_id;
        }
        ret = 0;
      }
  }

  return ret;
} // qcril_setup_timed_callback_ex_params_adv


/*=========================================================================
  FUNCTION:  qcril_cancel_timed_callback

===========================================================================*/
/*!
    @brief
    Cancel RIL callback timer

    @return
    0 on success.
*/
/*=========================================================================*/
int qcril_cancel_timed_callback
(
  void *param
)
{
  uint32 timer_id = (uint32)(uintptr_t) param;
  qcril_timed_callback_info *info = qcril_find_and_remove_timed_callback(timer_id);
  int ret = -1;
  /*-----------------------------------------------------------------------*/

  if (info)
  {
    ret = 0;

    if (info->need_free && NULL != info->extra_params)
    {
      qcril_free(info->extra_params);
    }

    QCRIL_LOG_DEBUG( "Cancel timer with ID %d", info->timer_id );
    qcril_free(info);
  }

  return ret;
} /* qcril_cancel_timed_callback */

/*=========================================================================
  FUNCTION:  qcril_timed_callback_active

===========================================================================*/
/*!
    @brief
    Query state of the timed callback

    @return
    0 if timer is inactive. Non-zero Otherwise
*/
/*=========================================================================*/
int qcril_timed_callback_active
(
  uint32 timer_id
)
{
  /*-----------------------------------------------------------------------*/
  qcril_timed_callback_info **info = NULL;

  QCRIL_ASSERT( info );

  QCRIL_MUTEX_LOCK( &qcril_timed_callback_list_mutex, "timed callback list mutex" );

  info = _qcril_find_timed_callback_locked(timer_id);

  QCRIL_MUTEX_UNLOCK( &qcril_timed_callback_list_mutex, "timed callback list mutex" );

  QCRIL_ASSERT( info != NULL );

  return *info !=NULL;
} /* qcril_timed_callback_active */


// EVENT REFACTOR - cleanup
#if 1
//===========================================================================
// qcril_free_request_and_dispatch_follower_request_cb
//===========================================================================
void qcril_free_request_and_dispatch_follower_request_cb(qcril_timed_callback_handler_params_type * handler_params)
{
  qcril_free_req_and_dispatch_follower_req_payload_type *payload = NULL;
  qcril_request_resp_params_type                         resp_local;
  RIL_Token                                              follower_token;

  QCRIL_LOG_FUNC_ENTRY();

  if ( NULL != handler_params)
  {
    payload = handler_params->custom_param;

    if ( NULL != payload )
    {
      follower_token = qcril_reqlist_get_follower_token( payload->instance_id, payload->t );
      if ( QMI_RIL_ZERO != follower_token && qcril_reqlist_is_auto_respond_duplicate( payload->instance_id, follower_token ) )
      {
        // drop org
        qcril_reqlist_free( payload->instance_id, payload->t );
        // auto respond for duplicate
        resp_local          = *payload->data;
        resp_local.t        = follower_token; // substitute token
        resp_local.logstr   = NULL;
        qcril_send_request_response( &resp_local );
      }
      else
      {
        qcril_reqlist_free_and_dispatch_follower_req(payload->t, payload->token_id, payload->instance_id, NULL, QMI_RIL_ZERO );
      }

      if ( NULL != payload->data)
      {
        if (NULL != payload->data->resp_pkt)
        {
          qcril_free(payload->data->resp_pkt);
        }
        qcril_free(payload->data);
      }
      qcril_free(payload);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_free_request_and_dispatch_follower_request_cb

//===========================================================================
// qcril_free_request_list_entry_deferred
//===========================================================================
void qcril_free_request_list_entry_deferred( qcril_timed_callback_handler_params_type * handler_params )
{
  QCRIL_LOG_FUNC_ENTRY();

  if ( NULL != handler_params )
  {
    qcril_deferred_free_req_payload_type* payload = (qcril_deferred_free_req_payload_type*)handler_params;
    qcril_reqlist_free_deferred( payload->instance_id, payload->t, payload->token_id );
    qcril_free(handler_params);
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_free_request_list_entry_deferred
#endif

void qcril_send_empty_payload_request_response(qcril_instance_id_e_type instance_id, RIL_Token t, qcril_evt_e_type request_id, RIL_Errno ril_err_no)
{
  qcril_request_resp_params_type resp_param;

  /* EVENT REFACTOR TEST */
  if(qcril_qmi_ims_is_valid_event(request_id))
  {
    imsRadioSendMessage(t, ims_MsgType_RESPONSE, qcril_qmi_ims_map_event_to_request(request_id), qcril_qmi_ims_map_ril_error_to_ims_error(ril_err_no), NULL, 0);
  }
  else
  {
    qcril_default_request_resp_params( instance_id, t, request_id, ril_err_no, &resp_param );
    qcril_send_request_response( &resp_param );
  }
}

//===========================================================================
// qcril_is_internal_token
//===========================================================================
boolean qcril_is_internal_token(RIL_Token token)
{
    boolean result = FALSE;

    if ((token >= (RIL_Token)QCRIL_INTERNAL_REQ_TOKEN_BASE) &&
        (token <= (RIL_Token)QCRIL_INTERNAL_REQ_TOKEN_END))
    {
        result = TRUE;
    }

    return result;
}
/*===========================================================================

  FUNCTION: QCRIL_LOG_GET_TOKEN_ID

===========================================================================*/
/*!
    @brief
    Return the value of the Token ID.

    @return
    The value of Token ID
*/
/*=========================================================================*/
int32_t qcril_log_get_token_id
(
  RIL_Token t
)
{
  int32_t token_id = 0;

  /*-----------------------------------------------------------------------*/

  if ( t == NULL )
  {
    token_id = 0xFFFE;
  }
  else if ( t == ( void * ) QCRIL_TOKEN_ID_INTERNAL )
  {
    token_id = 0xFFFF;
  }
  else if (qcril_is_internal_token(t))
  {
    // only show low 32bits of token for 64 bit platform
    token_id = (int32_t)(long)t;
  }
  else
  {
    token_id =  (int32_t)((((long)*( (int32_t *) t )) - INT_MIN) % 10000);
  }

  return token_id;

} /* qcril_log_get_token_id */

//===========================================================================
//qcril_send_request_ack
//===========================================================================
void qcril_send_request_ack
(
  qcril_instance_id_e_type instance_id,
  RIL_Token token
)
{
  QCRIL_LOG_FUNC_ENTRY();
  if(QCRIL_RIL_VERSION >= 13)
  {
    if (token && !qcril_is_internal_token(token))
    {
       if(qcril_response_api[instance_id]->OnRequestAck != NULL)
       {
         qcril_response_api[instance_id]->OnRequestAck(token);
       }
       else
       {
         QCRIL_LOG_ERROR("No information received during init to send back the ack");
       }
    }
    else
    {
       QCRIL_LOG_DEBUG("invalid token");
    }
  }
  else
  {
    QCRIL_LOG_DEBUG("current RIL VERSION doesnt support this feature");
  }
  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_default_unsol_resp_params

===========================================================================*/
/*!
    @brief
    Set default values for unsolicted response parameters.

    @return
    None
*/
/*=========================================================================*/
void qcril_default_unsol_resp_params
(
  qcril_instance_id_e_type instance_id,
  int response_id,
  qcril_unsol_resp_params_type *param_ptr
)
{
  if(instance_id < QCRIL_MAX_INSTANCE_ID && param_ptr != NULL)
  {
      param_ptr->instance_id = instance_id;
      param_ptr->response_id = response_id;
      param_ptr->resp_pkt = NULL;
      param_ptr->resp_len = 0;
      param_ptr->logstr = NULL;
  }
  else
  {
    QCRIL_LOG_FATAL("CHECK FAILED");
  }
} /* qcril_default_unsol_resp_params */

/*=========================================================================
  FUNCTION:  qcril_send_unsol_response_epilog
===========================================================================*/
static void qcril_send_unsol_response_epilog(qcril_unsol_resp_params_type *param_ptr)
{
  QCRIL_LOG_FUNC_ENTRY();
  qcril_instance_id_e_type instance_id = param_ptr->instance_id;
  char label[ 512 ];

  /* Log event packet for Unsolicited response */
  if ( param_ptr->logstr != NULL)
  {
    /* EVENT REFACTOR REVIEW: UNSOL handling */
    QCRIL_SNPRINTF( label, sizeof( label ), "%s, %s", qcril_log_lookup_event_name( qcril_android_request_get_internal_event(param_ptr->response_id) ), param_ptr->logstr );
  }
  else
  {
    QCRIL_SNPRINTF( label, sizeof( label ), "%s", qcril_log_lookup_event_name( qcril_android_request_get_internal_event(param_ptr->response_id) ) );
  }

  QCRIL_LOG_CF_PKT_RIL_UNSOL_RES( instance_id, label );

  /* Send Unsolicted RIL response */
  QCRIL_LOG_DEBUG( "UI <--- %s (%p) --- RIL [RID %d, Len %d, %s]",
                   qcril_log_lookup_event_name( qcril_android_request_get_internal_event(param_ptr->response_id) ), param_ptr->response_id,
                   param_ptr->instance_id, param_ptr->resp_len, param_ptr->logstr );


  /* Print RIL Message */
  qcril_log_print_ril_message(qcril_android_request_get_internal_event(param_ptr->response_id), RIL__MSG_TYPE__UNSOL_RESPONSE,
                               param_ptr->resp_pkt, param_ptr->resp_len, RIL_E_SUCCESS);

  if ( param_ptr->instance_id < QCRIL_MAX_INSTANCE_ID )
  {
    qcril_response_api[ param_ptr->instance_id ]->OnUnsolicitedResponse( param_ptr->response_id, param_ptr->resp_pkt,
                                                                       param_ptr->resp_len );
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_send_unsol_response_epilog */

/*=========================================================================
  FUNCTION:  qcril_send_unsol_response

===========================================================================*/
/*!
    @brief
    Send RIL_onUnsolicitedResponse.

    @return
    None
*/
/*=========================================================================*/
void qcril_send_unsol_response
(
  qcril_unsol_resp_params_type *param_ptr
)
{
  boolean dispachable;
  qmi_ril_gen_operational_status_type cur_status;

  QCRIL_LOG_FUNC_ENTRY();
  do
  {
    if ( param_ptr == NULL || param_ptr->instance_id >= QCRIL_MAX_INSTANCE_ID )
    {
      QCRIL_LOG_FATAL("invalid param");
      break;
    }

    qmi_ril_android_pending_unsol_resp_lock();

    // check dispatchable, if not queue the resp
    dispachable = qmi_ril_check_android_unsol_resp_dispatchable(param_ptr->response_id);
    if ( !dispachable )
    {
      qmi_ril_add_unsol_resp_to_pending_list(param_ptr);
    }
    else
    {
       // handling RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED differently
       if ( RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED!= param_ptr->response_id)
       {
          cur_status = qmi_ril_get_operational_status();
          if ( cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_ONGOING ||
                cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_UNRESTRICTED ||
                cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_RESUMING )
          {
            qcril_send_unsol_response_epilog(param_ptr);
          }
#ifndef QMI_RIL_UTF
          else if((RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED == param_ptr->response_id)||
                  (RIL_UNSOL_SIM_REFRESH == param_ptr->response_id)||
                  (RIL_UNSOL_STK_PROACTIVE_COMMAND == param_ptr->response_id) ||
                  (RIL_UNSOL_DATA_CALL_LIST_CHANGED == param_ptr->response_id) )
          {
            qcril_send_unsol_response_epilog(param_ptr);
          }
#endif
          else
          {
            QCRIL_LOG_INFO("Invalid state (%d), Blocking unsol resp %d", cur_status, param_ptr->response_id);
          }
       }
    }

    qmi_ril_android_pending_unsol_resp_unlock();

    // send RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED after unsol_resp_unlock
    if ( dispachable && (RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED== param_ptr->response_id) )
    {
       qcril_send_unsol_response_epilog(param_ptr);
    }

  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN();

} /* qcril_send_unsol_response */

//===========================================================================
//qcril_hook_unsol_response
//===========================================================================
void qcril_hook_unsol_response
(
  qcril_instance_id_e_type instance_id,
  qcril_evt_e_type  unsol_evt,
  // Move the unsol events outof eventlist.h and into oem.h, include oem.h in qcrili.h (temp)
  // Change them into an enum, change type of this function.
  char *  data,
  uint32  data_len
)
{
  char *payload = NULL;
  uint32 index = 0;
  qcril_unsol_resp_params_type unsol_resp;

  int                                       is_qmi_idl_tunelling;
  uint16                                    message_id;
  uint16                                    service_id;
  uint32_t                                  tlv_stream_len;
  qmi_idl_service_object_type               qmi_idl_tunneling_service_object;
  qmi_client_error_type                     idl_err;
  uint32_t                                  encoded_fact;
  uint32_t *                                int32_param;
  uint16_t *                                int16_param;

  int unsol_event = qcril_qmi_oem_get_oemhook_msg(unsol_evt);
  // EVENT REFACTOR TEST
  payload = NULL;
  do
  {
    service_id = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_NONE;
    message_id = QMI_RIL_ZERO;

    switch ( unsol_event )
    {
      // * VT section
      case QCRIL_REQ_HOOK_VT_UNSOL_CALL_STATUS_IND:
        is_qmi_idl_tunelling = TRUE;
        service_id = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_VT;
        message_id  = IMS_VT_CALL_STATUS_IND_V01;
        break;

      // * eMBMS section
      case QCRIL_REQ_HOOK_EMBMS_UNSOL_RSSI_IND:
        is_qmi_idl_tunelling = TRUE;
        service_id  = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id  = QMI_EMBMS_UNSOL_RSSI_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_SVC_STATE:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_UNSOL_EMBMS_SERVICE_STATE_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_ACTIVE_TMGI:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_ACTIVE_TMGI_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_COVERAGE:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_UNSOL_BROADCAST_COVERAGE_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_OSS_WARNING:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_OOS_WARNING_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_AVAILABLE_TMGI:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_AVAILABLE_TMGI_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_CELL_INFO_CHANGED:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_CELL_INFO_CHANGED_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_RADIO_STATE_CHANGED:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_RADIO_STATE_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_SAI_LIST:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_SAI_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_SIB16_COVERAGE:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_UNSOL_SIB16_COVERAGE_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_E911_STATE_CHANGED:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_E911_STATE_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_CONTENT_DESC_CONTROL:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_UNSOL_CONTENT_DESC_UPDATE_PER_OBJ_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_EMBMS_STATUS:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_UNSOL_EMBMS_STATUS_IND_V01;
        break;

      case QCRIL_REQ_HOOK_EMBMS_UNSOL_GET_INTERESTED_TMGI_LIST:
        is_qmi_idl_tunelling = TRUE;
        service_id           = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS;
        message_id           = QMI_EMBMS_UNSOL_GET_INTERESTED_TMGI_LIST_REQ_V01;
        break;

      //presence
      case QCRIL_REQ_HOOK_IMS_PUBLISH_TRIGGER_IND_V01:
        is_qmi_idl_tunelling = TRUE;
        service_id  = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_PRESENCE;
        message_id  = QMI_IMSP_PUBLISH_TRIGGER_IND_V01;
        break;
      case QCRIL_REQ_HOOK_IMS_NOTIFY_XML_IND_V01:
        is_qmi_idl_tunelling = TRUE;
        service_id  = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_PRESENCE;
        message_id  = QMI_IMSP_NOTIFY_XML_IND_V01;
        break;

      case QCRIL_REQ_HOOK_IMS_NOTIFY_IND_V01:
        is_qmi_idl_tunelling = TRUE;
        service_id  = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_PRESENCE;
        message_id  = QMI_IMSP_NOTIFY_IND_V01;
        break;

      case QCRIL_REQ_HOOK_IMS_ENABLER_STATUS_IND:
        is_qmi_idl_tunelling = TRUE;
        service_id  = QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_PRESENCE;
        message_id  = QMI_IMSP_ENABLER_STATE_IND_V01;
        break;

      default:
        is_qmi_idl_tunelling = FALSE;
        break;
    }


    if ( is_qmi_idl_tunelling )
    {
      qmi_idl_tunneling_service_object = qmi_ril_oem_hook_qmi_idl_tunneling_get_service_object( service_id );

      if ( NULL != qmi_idl_tunneling_service_object )
      {
        idl_err = qmi_idl_get_max_message_len( qmi_idl_tunneling_service_object, QMI_IDL_INDICATION, message_id, &tlv_stream_len  );

        if ( QMI_NO_ERR == idl_err )
        {
          payload = (char *) qcril_malloc( tlv_stream_len + OEM_HOOK_QMI_TUNNELING_IND_OVERHEAD_SIZE );

          if ( NULL != payload )
          {
            QCRIL_LOG_DEBUG("max length = %d, rcvd struc len = %d, msg_id = %d", tlv_stream_len, data_len, message_id );

            encoded_fact = QMI_RIL_ZERO;
            idl_err = qmi_idl_message_encode( qmi_idl_tunneling_service_object,
                                    QMI_IDL_INDICATION,
                                    message_id,
                                    data,
                                    data_len,
                                    payload + OEM_HOOK_QMI_TUNNELING_IND_OVERHEAD_SIZE,
                                    tlv_stream_len,
                                    &encoded_fact );

            if ( QMI_NO_ERR == idl_err )
            {
              // complete the oem hook tunneling header

              // signature
              memcpy( payload, QCRIL_HOOK_OEM_NAME, OEM_HOOK_QMI_TUNNELING_IND_Q_STR_LEN_SIZE );

              // event id
              int32_param = (uint32_t*) ( payload + OEM_HOOK_QMI_TUNNELING_IND_Q_STR_LEN_SIZE );
              *int32_param = QCRIL_REQ_HOOK_UNSOL_GENERIC;

              // payload length
              int32_param = (uint32_t*) ( payload + OEM_HOOK_QMI_TUNNELING_IND_Q_STR_LEN_SIZE + OEM_HOOK_QMI_TUNNELING_IND_EVENT_ID_SIZE );
              *int32_param = encoded_fact + OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE + OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE;

              // service id
              int16_param = (uint16_t*) ( payload + OEM_HOOK_QMI_TUNNELING_IND_Q_STR_LEN_SIZE + OEM_HOOK_QMI_TUNNELING_IND_EVENT_ID_SIZE + OEM_HOOK_QMI_TUNNELING_IND_PLD_LEN_SIZE );
              *int16_param = service_id;

              // message id
              int16_param = (uint16_t*) ( payload + OEM_HOOK_QMI_TUNNELING_IND_Q_STR_LEN_SIZE + OEM_HOOK_QMI_TUNNELING_IND_EVENT_ID_SIZE +
                                         OEM_HOOK_QMI_TUNNELING_IND_PLD_LEN_SIZE + OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE );
              *int16_param = message_id;

              // finally
              qcril_default_unsol_resp_params( instance_id,
                                               (int) RIL_UNSOL_OEM_HOOK_RAW,
                                               &unsol_resp );

              unsol_resp.resp_pkt                   = ( void * ) payload;
              unsol_resp.resp_len                   = encoded_fact + OEM_HOOK_QMI_TUNNELING_IND_OVERHEAD_SIZE;
              qcril_qmi_print_hex( unsol_resp.resp_pkt , unsol_resp.resp_len);

              if (!qmi_ril_is_multi_sim_oem_hook_request(unsol_evt) &&
                   qmi_ril_is_feature_supported(QMI_RIL_FEATURE_OEM_SOCKET))
              {
                qcril_qmi_oemhook_agent_send_unsol(instance_id, unsol_resp.resp_pkt,
                                                unsol_resp.resp_len);
              }
              else
              {
                qcril_send_unsol_response( &unsol_resp );
              }
            }
            else
            {
              QCRIL_LOG_ERROR( "QMI IDL - failed to compose tlv stream err %d, actually encoded len %d ", (int) idl_err, (int) encoded_fact  );
              break;
            }

          }
          else
          {
            QCRIL_LOG_ERROR( "QMI IDL - failed to allocate payload tlv stream buf, size %d ", (int) tlv_stream_len  );
            break;
          }
        }
        else
        {
          QCRIL_LOG_ERROR( "QMI IDL - unsol event decode failed to obtain message len for msg id %d, idl err %d", (int) message_id, (int) idl_err  );
          break;
        }
      }
      else
      {
        QCRIL_LOG_ERROR( "QMI IDL - unsol event decode failed to obtain svc object for svc id %d ", (int) service_id   );
        break;
      }
    }
    else
    { // legacy stream
      payload = (char *) qcril_malloc( QCRIL_OTHER_OEM_NAME_LENGTH + sizeof(unsol_event) + sizeof(data_len) + data_len );
      if ( NULL != payload )
      {
        memcpy( payload, QCRIL_HOOK_OEM_NAME, QCRIL_OTHER_OEM_NAME_LENGTH );
        index += QCRIL_OTHER_OEM_NAME_LENGTH;

        memcpy( &payload[index], &unsol_event, sizeof(unsol_event) );
        index += sizeof(unsol_event);

        memcpy( &payload[index], &data_len, sizeof(data_len) );
        index += sizeof(data_len);

        memcpy( &payload[index], data, data_len );
        index += data_len;

        qcril_default_unsol_resp_params( instance_id, (int) RIL_UNSOL_OEM_HOOK_RAW, &unsol_resp );
        unsol_resp.resp_pkt = ( void * ) payload;
        unsol_resp.resp_len = index;

        if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_OEM_IND_TO_BOTH))
        {
          qcril_qmi_oemhook_agent_send_unsol(instance_id, unsol_resp.resp_pkt,
                                          unsol_resp.resp_len);
          qcril_send_unsol_response( &unsol_resp );
        }
        else if (!qmi_ril_is_multi_sim_oem_hook_request(unsol_evt) &&
             qmi_ril_is_feature_supported(QMI_RIL_FEATURE_OEM_SOCKET))
        {
          qcril_qmi_oemhook_agent_send_unsol(instance_id, unsol_resp.resp_pkt,
                                          unsol_resp.resp_len);
        }
        else
        {
          qcril_send_unsol_response( &unsol_resp );
        }
      }
      else
      {
        QCRIL_LOG_ERROR( "qcril_malloc returned NULL" );
        break;
      }
    }

  } while ( FALSE );

  if ( NULL != payload )
  {
    qcril_free( payload );
  }
} // qcril_hook_unsol_response

/*===========================================================================

  FUNCTION:  onRequest

===========================================================================*/
/*!
    @brief
    Call from RIL to us to make a RIL_REQUEST
    Must be completed with a call to RIL_onRequestComplete()
    RIL_onRequestComplete() may be called from any thread, before or after
    this function returns.
    Returning from this routine implies the radio is ready to process another
    command (whether or not the previous command has completed).

    @return
    None.
*/
/*=========================================================================*/
void onRequest
(
  qcril_instance_id_e_type  instance_id,
  int                       request,
  void                      *data,
  size_t                    datalen,
  RIL_Token                 t
)
{
  qcril_request_params_type                param;
  int * in_data_ptr,
        in_data_val;

  char                                     label[ 512 ];
  qcril_request_resp_params_type           resp;

  RIL_Errno                                audit_result = RIL_E_GENERIC_FAILURE;
  qmi_ril_oem_hook_request_details_type    oem_hook_req_details;
  void *                                   substituted_data;
  RIL_Errno                                info_fetch_result = RIL_E_SUCCESS;

  qmi_ril_fw_android_request_kind_execution_overview_type*
                                            android_request_kind_execution_overview = NULL;
  uint32_t                                  android_request_handling_caps;
  qmi_ril_fw_android_request_holder_type*   android_request_param_holder;
  qmi_ril_fw_android_request_holder_type*   android_request_prm_hldr_iter;
  uint32_t                                  cap_max_queue_sz;
  uint32_t                                  cap_queue_sz_iter;
  int                                       no_render;
  qmi_ril_fw_android_request_kind_execution_overview_type*
                                            already_pending_request_kind;
  int                                       clean_family;
  qmi_ril_fw_android_request_holder_type *  relative_request_holder;
  qmi_ril_fw_android_request_holder_type    inbound_request_holder_local;

  qmi_ril_fw_android_request_kind_execution_overview_type*
                                            already_awaiting_exec_request_kind;
  int                                       any_already_awaiting_exec;

  void*                                     android_request_param_copy;
  int                                       android_request_param_copy_len;
  uint32                                    android_request_param_4_bytes;
  int                                       android_request_param_4_bytes_len;
  qmi_ril_fw_android_param_copy_approach_type
                                            request_param_method;
  int                                       local_param_copy_success;


  char                                      log_essence[ QCRIL_MAX_LOG_MSG_SIZE ];
  char                                      log_addon[ QCRIL_MAX_LOG_MSG_SIZE ];
  uint32                                    flow_decision_flag = FLOWCONTROL_ONREQUEST_NONE;
  int                                       log_android_request_id = 0;
  qcril_evt_e_type                          log_pending_android_request_id = 0;
  int32_t                                   log_projected_token = 0;
  int32_t                                   log_pending_projected_token = 0;
  qmi_ril_fw_android_request_holder_type*   log_waiting_for_execution_exec_req_holder = NULL;
  int                                       log_dispatch_dedicated_thrd = FALSE;

  qcril_req_handler_type                    *event_handler = NULL;
  qcril_request_return_type                 ret;
  IxErrnoType                               fw_internal_err = E_FAILURE;

  /*-----------------------------------------------------------------------*/

  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );
  QCRIL_ASSERT( t != (void *) QCRIL_TOKEN_ID_INTERNAL );

  /*-----------------------------------------------------------------------*/

  memset( &oem_hook_req_details, 0, sizeof(oem_hook_req_details) );

  do
  {
          no_render                     = FALSE;

          substituted_data              = NULL;

          log_android_request_id        = request;
          log_projected_token           = qcril_log_get_token_id( t );

          memset ( &inbound_request_holder_local, 0, sizeof( inbound_request_holder_local ) );
          inbound_request_holder_local.param_info_and_len = QMI_RIL_FW_ANDROID_REQUEST_INFO_COMPOSE( QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE, QMI_RIL_ZERO );

          param.event_id_android  = request;
          param.event_id          = qcril_android_request_get_internal_event(request);
          param.data              = data;
          param.datalen           = datalen;
          param.t                 = t;
          param.instance_id       = instance_id;
          param.modem_id          = QCRIL_DEFAULT_MODEM_ID;
          if (!param.event_id) {
              QCRIL_LOG_DEBUG("Unable to find internal event for request %d", request);
              audit_result = RIL_E_REQUEST_NOT_SUPPORTED;
              break;
          }

          QCRIL_LOG_DEBUG( "UI --- %s (%p) ---> RIL [RID %d, token id %" PRId32 ", data len %d]",
                           qcril_log_lookup_event_name( param.event_id ), param.event_id, param.instance_id,
                           log_projected_token, param.datalen );
          /* Print RIL Message */
          qcril_log_print_ril_message(param.event_id, RIL__MSG_TYPE__REQUEST, param.data,
                                       param.datalen, info_fetch_result);

          /* check if request is suppressed */
          if (qcril_request_check_if_suppressed(param.event_id_android, &event_handler))
          {
              if (!event_handler)
              {
                  QCRIL_LOG_ERROR("Internal error..request %d is supressed but doesnt have an event handler associated", param.event_id_android);
                  audit_result = RIL_E_INTERNAL_ERR;
              }
              else
              {
                  event_handler(&param, &ret);
                  audit_result = RIL_E_SUCCESS;
              }

              QCRIL_LOG_INFO("RIL Request event id: %d is suppressed", param.event_id_android);
              break;
          }

          if ( ( param.event_id_android == RIL_REQUEST_DIAL ) || ( param.event_id_android == RIL_REQUEST_SETUP_DATA_CALL ) )
          {
            QCRIL_SNPRINTF( label, sizeof( label ), "%s - RID %d, Token id %" PRId32 "", qcril_log_lookup_event_name( param.event_id ),
                            param.instance_id, log_projected_token );
            // Use bold arrows for really interesting events
            QCRIL_LOG_CF_PKT_RIL_REQ2( param.instance_id, label );
          }
          else if ( ( ( param.event_id_android == RIL_REQUEST_RADIO_POWER ) ||
                      ( param.event_id_android == RIL_REQUEST_SCREEN_STATE ) ||
                      ( param.event_id_android == RIL_REQUEST_SET_MUTE ) ) && ( param.data != NULL ) )
          {
            in_data_ptr = ( int * ) param.data;
            if ( in_data_ptr != NULL )
            {
              in_data_val = *in_data_ptr;
            }
            else
            {
              in_data_val = 1;
            }

            QCRIL_SNPRINTF( label, sizeof( label ), "%s - %s, Token id %" PRId32 "",
                            qcril_log_lookup_event_name( param.event_id ), ( in_data_val == 0 ) ? "Off" : "On" ,
                            log_projected_token );
            QCRIL_LOG_CF_PKT_RIL_REQ( param.instance_id, label );
          }
          else
          {
            QCRIL_SNPRINTF( label, sizeof( label ), "%s - Token id %" PRId32 "",
                            qcril_log_lookup_event_name( param.event_id ), log_projected_token );
            QCRIL_LOG_CF_PKT_RIL_REQ( param.instance_id, label );
          }

          if ( param.event_id_android >= QCRIL_REQ_BASE )
          {
            // The request is out of range
            QCRIL_LOG_ERROR("Request not supported..request id is out of range");
            audit_result = RIL_E_REQUEST_NOT_SUPPORTED;
            break;
          }


          // print the recieved TLV byte stream
          QCRIL_LOG_DEBUG("data stream received from telephony");
          if(param.event_id_android != (int) RIL_REQUEST_IMS_SEND_SMS ){
                  qcril_qmi_print_hex(param.data,  param.datalen);
          } else {
                  qcril_qmi_print_hex(param.data,  sizeof(RIL_IMS_SMS_Message));
          }

          if ( param.event_id_android  == RIL_REQUEST_OEM_HOOK_RAW )
          {
            if (!qmi_ril_get_req_details_from_oem_req(&oem_hook_req_details,
                                                       &audit_result,
                                                        data,
                                                       &param,
                                                        datalen))
            {
              QCRIL_LOG_DEBUG("OEM HOOK RAW request %d not supported.",
                                           oem_hook_req_details.hook_req);
              break;
            }
            if (!qmi_ril_is_feature_supported(QMI_RIL_FEATURE_OEM_SOCKET) ||
                 qmi_ril_is_multi_sim_oem_hook_request(oem_hook_req_details.hook_req_event))
            {
              // oem hook qmi idl tunneling
              if ( oem_hook_req_details.is_qmi_tunneling )
              {
                if (!qmi_ril_parse_oem_req_tunnelled_message(&oem_hook_req_details,
                                                             &audit_result,
                                                             &param))
                {
                  break;
                }
              }
            }
            else
            {
              QCRIL_LOG_DEBUG("OEM HOOK RAW messages are supported through oem socket, "
                              "not through rild socket");
              audit_result = RIL_E_REQUEST_NOT_SUPPORTED;
              break;
            }
          }

          // flow control s
          qmi_ril_fw_android_request_flow_control_info_lock();
          do
          {
            QCRIL_LOG_DEBUG( "android request id %d", request );
            if ( request <= QMI_RIL_ZERO || request > QMI_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID )
            {
              QCRIL_LOG_ERROR("Request not supported..invalid req id");
              audit_result = RIL_E_REQUEST_NOT_SUPPORTED;
              break;
            }

            // EVENT REFACTOR TEST: test functionality of the indexing
            android_request_kind_execution_overview = &qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info[ request ];

            android_request_handling_caps = android_request_kind_execution_overview->nof_extra_holders_and_caps_and_dynamics;
            QCRIL_LOG_DEBUG( "handling caps %x hex", android_request_handling_caps );

            if ( android_request_handling_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FLOW_CONTROL_EXEMPT )
            {
              audit_result = RIL_E_SUCCESS;
              flow_decision_flag |= FLOWCONTROL_ONREQUEST_FLOWCONTROL_EXEMPT;
              break;
            }

            // store param
            request_param_method = qmi_ril_fw_create_android_live_params_copy (
                                                                                  request, // EVENT REFACTOR TEST: check impact to this API.
                                                                                  data,
                                                                                  datalen,
                                                                                  (void*)&android_request_param_4_bytes,
                                                                                  &android_request_param_4_bytes_len,
                                                                                  &android_request_param_copy,
                                                                                  &android_request_param_copy_len,
                                                                                  &fw_internal_err
                                                                               );
            QCRIL_LOG_DEBUG( "method %d, datalen %d", (int)request_param_method, datalen );
            switch ( request_param_method )
            {
              case QMI_RIL_ANDROID_PARAM_CPY_APPRON_4_BYTES_SNAPSHOT:
                inbound_request_holder_local.param_info_and_len = QMI_RIL_FW_ANDROID_REQUEST_INFO_COMPOSE(
                           QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_INT, android_request_param_4_bytes_len );
                memcpy( (void*)&inbound_request_holder_local.param_payload_holder.param_int, (void*)&android_request_param_4_bytes, sizeof(int) );
                local_param_copy_success = TRUE;
                break;

              case QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY:
                inbound_request_holder_local.param_info_and_len = QMI_RIL_FW_ANDROID_REQUEST_INFO_COMPOSE(
                          QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_PTR, android_request_param_copy_len );
                inbound_request_holder_local.param_payload_holder.param_ptr = android_request_param_copy;
                local_param_copy_success = TRUE;
                break;

              case QMI_RIL_ANDROID_PARAM_CPY_APPRON_EMPTY_NO_ACTION:
                // no payload
                inbound_request_holder_local.param_info_and_len = QMI_RIL_FW_ANDROID_REQUEST_INFO_COMPOSE(
                           QMI_RIL_FW_ANDROID_REQUEST_INFO_NO_PAYLOAD, QMI_RIL_ZERO );
                local_param_copy_success = TRUE;
                break;

              default: // failure
                local_param_copy_success = FALSE;
                break;
            }
            if ( !local_param_copy_success )
            {
              audit_result = qcril_qmi_map_internalerr_from_reqlist_new_to_ril_err(fw_internal_err);
              break;
            }

            // stote attributes
            inbound_request_holder_local.token = t;
            inbound_request_holder_local.moniker = qmi_ril_fw_android_request_flow_control_overview.common_moniker_book;
            qmi_ril_fw_android_request_flow_control_overview.common_moniker_book++;
            if ( QMI_RIL_ZERO == qmi_ril_fw_android_request_flow_control_overview.common_moniker_book )
            {
              qmi_ril_fw_android_request_flow_control_overview.common_moniker_book++;
            }
            // note QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE is now excluded in param_info_and_len


            // flow controlling for what's oustanding
            already_pending_request_kind = qmi_ril_fw_android_request_flow_control_find_busy_kind( android_request_kind_execution_overview );
            clean_family = FALSE;

            if ( android_request_handling_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_MULTIPLE_AUTO_DROP_ON_DIFF_PARAMS &&
                 NULL != already_pending_request_kind )
            {
              QCRIL_LOG_DEBUG( "auto drop check" );
              if ( already_pending_request_kind != android_request_kind_execution_overview )
              { // different android request id
                clean_family = TRUE;
              }
              else
              { // same android request id
                relative_request_holder = qmi_ril_fw_android_request_flow_control_find_outstanding_request_within_kind( android_request_kind_execution_overview );
                if ( NULL != relative_request_holder )
                {
                  if ( android_request_handling_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_SINGLE_ONLY )
                  {
                    clean_family = !qmi_ril_fw_android_request_flow_control_request_holders_have_same_param(
                                        &android_request_kind_execution_overview->holders.local_holder, &inbound_request_holder_local );
                  }
                  else if ( android_request_handling_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE )
                  {
                    cap_max_queue_sz = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_DECOMPOSE_QUEUE_SZ( android_request_handling_caps );
                    android_request_prm_hldr_iter = android_request_kind_execution_overview->holders.extra_holders;
                    for ( cap_queue_sz_iter = QMI_RIL_ZERO; cap_queue_sz_iter < cap_max_queue_sz && !clean_family ; cap_queue_sz_iter++ )
                    {
                      if ( !(android_request_prm_hldr_iter->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE) )
                      {
                        if ( !qmi_ril_fw_android_request_flow_control_request_holders_have_same_param( android_request_prm_hldr_iter, &inbound_request_holder_local ) )
                        {
                          clean_family = TRUE;
                        }
                        else
                        {
                          android_request_prm_hldr_iter++;
                        }
                      }
                    }
                  }
                }
              }
            }
            QCRIL_LOG_DEBUG( "clean family %d", clean_family );
            if ( clean_family )
            {
              qmi_ril_fw_android_request_flow_control_abandon_requests_local_and_ring_zoned( android_request_kind_execution_overview, RIL_E_CANCELLED, FALSE );
              already_pending_request_kind = NULL;
            }
            // if nothing pending, anything already ready for exec
            already_awaiting_exec_request_kind = NULL;
            if ( NULL == already_pending_request_kind )
            {
              any_already_awaiting_exec = qmi_ril_fw_android_request_flow_control_find_request_for_execution( android_request_kind_execution_overview,
                                                                                  &already_awaiting_exec_request_kind,
                                                                                  &log_waiting_for_execution_exec_req_holder );
              QCRIL_LOG_DEBUG( "already waiting for exec %d", any_already_awaiting_exec );
            }


            cap_max_queue_sz = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_DECOMPOSE_QUEUE_SZ( android_request_handling_caps );

            // identify slot to store android req data
            android_request_param_holder = NULL;
            if ( android_request_handling_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_SINGLE_ONLY )
            { // // single request supported
              if ( QMI_RIL_ZERO != android_request_kind_execution_overview->token_under_execution )
              { // single request supported and one already outstanding
                // action will be taken below on android_request_param_holder validation
              }
              else
              {
                android_request_param_holder = &android_request_kind_execution_overview->holders.local_holder;
              }
            }
            else if ( android_request_handling_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE  )
            {
              android_request_prm_hldr_iter = android_request_kind_execution_overview->holders.extra_holders;

              for ( cap_queue_sz_iter = QMI_RIL_ZERO; cap_queue_sz_iter < cap_max_queue_sz && NULL == android_request_param_holder; cap_queue_sz_iter++ )
              {
                if ( android_request_prm_hldr_iter->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE )
                {
                  android_request_param_holder = android_request_prm_hldr_iter;
                }
                else
                {
                  android_request_prm_hldr_iter++;
                }
              }
            }
            QCRIL_LOG_DEBUG( "android_request_param_holder %x hex", android_request_param_holder );
            if ( NULL == android_request_param_holder )
            { // no resources to accomodate the request
              QCRIL_LOG_ERROR("No resources..to accomodate the request");
              audit_result = RIL_E_NO_RESOURCES;
              flow_decision_flag |= FLOWCONTROL_ONREQUEST_FLOWCONTROL_REJECTED_OVERFLOW;
              break;
            }

            // store params and token
            *android_request_param_holder = inbound_request_holder_local;
            qmi_ril_fw_android_request_flow_control_overview_request_review_holders( android_request_kind_execution_overview );

            QCRIL_LOG_DEBUG( "pending req kind 0x%x hex, awaitingexec req kind 0x%x hex", already_pending_request_kind, already_awaiting_exec_request_kind );
            if ( NULL != already_pending_request_kind || NULL != already_awaiting_exec_request_kind )
            { // may not commence execution now, need to wait for completion of already pending requests
              audit_result = RIL_E_SUCCESS;
              no_render    = TRUE;
              if ( NULL != already_pending_request_kind )
              {
                flow_decision_flag |= FLOWCONTROL_ONREQUEST_FLOWCONTROL_DEFERRED_BUSY;
                log_pending_android_request_id = already_pending_request_kind->original_android_request_id;
                log_pending_projected_token    = qcril_log_get_token_id ( already_pending_request_kind->token_under_execution );
              }
              else if ( NULL != already_awaiting_exec_request_kind )
              {
                flow_decision_flag |= FLOWCONTROL_ONREQUEST_FLOWCONTROL_DEFERRED_BUSY;
                log_pending_android_request_id = already_awaiting_exec_request_kind->original_android_request_id;
                if ( NULL != log_waiting_for_execution_exec_req_holder )
                {
                  log_pending_projected_token    = qcril_log_get_token_id ( log_waiting_for_execution_exec_req_holder->token );
                }
              }
              break;
            }

            // mark commencing of execution
            android_request_kind_execution_overview->token_under_execution = t;

            flow_decision_flag |= FLOWCONTROL_ONREQUEST_DISPATCHED;

            QCRIL_LOG_DEBUG( "token under exec %" PRId32 "", qcril_log_get_token_id ( android_request_kind_execution_overview->token_under_execution )  );

            audit_result = RIL_E_SUCCESS;
          } while (FALSE);
          qmi_ril_fw_android_request_flow_control_info_unlock();

          QCRIL_LOG_DEBUG( "pre-exec token id %" PRId32 ", a-r-id %d, audit %d, nrender %d", log_projected_token,
                                                                                    param.event_id_android,
                                                                                    (int)audit_result,
                                                                                    (int)no_render );

          if ( no_render )
            break;
          // flow control - e

          if ( RIL_E_SUCCESS == audit_result )
          {
            audit_result = qmi_ril_fw_android_request_render_execution( param.t,
                                                                        param.event_id,
                                                                        param.data,
                                                                        param.datalen,
                                                                        param.instance_id,
                                                                        &log_dispatch_dedicated_thrd );
            if ( log_dispatch_dedicated_thrd )
            {
              flow_decision_flag |= FLOWCONTROL_ONREQUEST_DISPATCHED_CMD_DEDICATED_THRD;
            }
            else
            {
              flow_decision_flag |= FLOWCONTROL_ONREQUEST_DISPATCHED_CMD_MAIN_THRD;
            }
          }
          else
          {
            if ( !(inbound_request_holder_local.param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE) )
            {
              qmi_ril_fw_android_request_flow_control_release_holder_info_bookref( &inbound_request_holder_local, param.event_id);
            }
          }

    } while ( FALSE );

    if ( RIL_E_SUCCESS != audit_result )
    {
      qcril_default_request_resp_params( param.instance_id, param.t, param.event_id, audit_result, &resp );
      if ( ( oem_hook_req_details.hook_req > QCRIL_REQ_HOOK_BASE ) && ( oem_hook_req_details.hook_req < QCRIL_REQ_HOOK_MAX ) )
      {
          resp.rild_sock_oem_req = TRUE;
      }
      resp.flow_decision_flag = flow_decision_flag;
      qcril_send_request_response( &resp );
    } // otherwise it is taken that respective request handler has responded synchronously or will respond asynchronously

    QCRIL_LOG_DEBUG( "dispatch outcome = %d", audit_result );

    if ( substituted_data )
    {
      qcril_free( substituted_data );
    }

    if ( !( flow_decision_flag & FLOWCONTROL_ONREQUEST_DISPATCHED ) )
    { // not dispatched
      snprintf( log_essence, QCRIL_MAX_LOG_MSG_SIZE, "cmd %d, t_id %" PRId32 " ",
                              log_android_request_id, log_projected_token);

      if ( flow_decision_flag & FLOWCONTROL_ONREQUEST_FLOWCONTROL_DEFERRED_BUSY )
      {
        snprintf( log_addon, QCRIL_MAX_LOG_MSG_SIZE, "defer exec FC cmd %d (%s) t_id %" PRId32 "",
                  qcril_event_get_android_request(log_pending_android_request_id),
                  qcril_log_lookup_event_name(log_pending_android_request_id),
                  log_pending_projected_token );
        strlcat( log_essence, log_addon, sizeof( log_essence ) );
      }
      else if ( flow_decision_flag & FLOWCONTROL_ONREQUEST_FLOWCONTROL_REJECTED_OVERFLOW )
      {
        strlcat( log_essence, "exec rej no no cap", sizeof( log_essence ) );
      }
      else
      {
        strlcat( log_essence, "exec not performed", sizeof( log_essence ) );
      }
      snprintf( log_addon, QCRIL_MAX_LOG_MSG_SIZE, ",int_adt %d", (int) audit_result );
      strlcat( log_essence, log_addon, sizeof( log_essence ) );

      QCRIL_LOG_DEBUG( "%s", log_essence );
    }
} // onRequest

/*===========================================================================

  FUNCTION:  onRequest_rid

===========================================================================*/
/*!
    @brief
    Call from RIL instance RID0 to us to make a RIL_REQUEST
    Must be completed with a call to RIL_onRequestComplete()
    RIL_onRequestComplete() may be called from any thread, before or after
    this function returns.
    Returning from this routine implies the radio is ready to process another
    command (whether or not the previous command has completed).

    @return
    None.
*/
/*=========================================================================*/
static void onRequest_rid
(
  int request,
  void *data,
  size_t datalen,
  RIL_Token t
)
{
  /*-----------------------------------------------------------------------*/

  onRequest( qmi_ril_process_instance_id, request, data, datalen, t );

} /* onRequest_rid0 */

//===========================================================================
//qmi_ril_ssr_in_progress
//===========================================================================
int qmi_ril_ssr_in_progress( )
{
  int                                 res = FALSE;
  qmi_ril_gen_operational_status_type status;

  status = qmi_ril_get_operational_status();
  if ( ( status == QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDING ) ||
       ( status == QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDED ) ||
       ( status == QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_PENDING ) ||
       ( status == QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_RETRY ) )
  {
    res = TRUE;
  }

  QCRIL_LOG_INFO( "ssr in progress %d", res );
  return res;
} // qmi_ril_ssr_in_progress


/*===========================================================================

  FUNCTION:  qcril_get_current_radio_state

===========================================================================*/
/*!
    @brief
    Return current radio state.

    @return
    The current state of the RIL
*/
/*=========================================================================*/
RIL_RadioState qcril_get_current_radio_state()
{
  RIL_RadioState radio_state = RADIO_STATE_UNAVAILABLE;
  qmi_ril_gen_operational_status_type current_state;

  QCRIL_LOG_FUNC_ENTRY();

  current_state = qmi_ril_get_operational_status();
  switch (current_state)
  {
    case QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_ONGOING: // fallthrough
    case QMI_RIL_GEN_OPERATIONAL_STATUS_UNRESTRICTED: // fallthrough
    case QMI_RIL_GEN_OPERATIONAL_STATUS_RESUMING:
    case QMI_RIL_GEN_OPERATIONAL_STATUS_UNBIND:
      radio_state = qcril_qmi_nas_dms_get_current_power_state();
      break;

    default:
      radio_state = RADIO_STATE_UNAVAILABLE;
      break;
  }

  if (qcril_qmi_nas_modem_power_is_mdm_shdn_in_apm() &&
      0 == qcril_qmi_modem_power_voting_state() &&
      qcril_qmi_modem_power_is_voting_feature_supported())
  {
      radio_state = RADIO_STATE_OFF;
      QCRIL_LOG_INFO("setting RADIO STATE OFF");
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_state);
  return radio_state;
}

/*===========================================================================

  FUNCTION:  currentState

===========================================================================*/
/*!
    @brief
    Return current radio state of an instance.

    @return
    The current state of the RIL
*/
/*=========================================================================*/
static RIL_RadioState currentState
(
  qcril_instance_id_e_type instance_id
)
{
  char label[ 300 ];
  const char *radio_state_name = NULL;
  RIL_RadioState radio_state;

  /*-----------------------------------------------------------------------*/

  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );

  /*-----------------------------------------------------------------------*/

  radio_state = qcril_get_current_radio_state();

  radio_state_name = qcril_log_ril_radio_state_to_str(radio_state);

  QCRIL_LOG_DEBUG( "currentState() -> %s(%d)", radio_state_name, radio_state );

  QCRIL_SNPRINTF( label, sizeof( label ), "currentState() - %s", radio_state_name );
  QCRIL_LOG_CF_PKT_RIL_FN( instance_id, label );

  return radio_state;

} /* currentState() */


/*===========================================================================

  FUNCTION:  currentState_rid

===========================================================================*/
/*!
    @brief
    Synchronous call from the RIL to us to return current radio state of
    RIL instance RID0.
    RADIO_STATE_UNAVAILABLE should be the initial state.

    @return
    The current state of the RIL
*/
/*=========================================================================*/
static RIL_RadioState currentState_rid
(
  void
)
{
  /*-----------------------------------------------------------------------*/

  return currentState( QCRIL_DEFAULT_INSTANCE_ID );

} /* currentState_rid0() */


/*===========================================================================

  FUNCTION:  onSupports

===========================================================================*/
/*!
    @brief
    Determines if the RIL supports the given RIL_REQUEST by a RIL instance

    @return
    1 if the given RIL_REQUEST is supported
*/
/*=========================================================================*/
static int onSupports
(
  qcril_instance_id_e_type instance_id,
  int request
)
{
  int supported = 1;
  char label[ 80 ];

  /*-----------------------------------------------------------------------*/

  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );

  /*-----------------------------------------------------------------------*/

  /* Do a lookup for the entry */
  if ( qcril_hash_table_lookup( qcril_android_request_get_internal_event(request), NULL ) != E_SUCCESS )
  {
    supported = 0;
  }

  QCRIL_LOG_DEBUG( "onSupports() ->: %s (%d), %s", qcril_log_lookup_event_name( qcril_android_request_get_internal_event(request) ), request,
                   (supported>0)?"Supported":"Not Supported");

  QCRIL_SNPRINTF( label, sizeof( label ), "onSupports() -` %s %s",
                  qcril_log_lookup_event_name( qcril_android_request_get_internal_event(request) ), ( supported > 0 ) ? "Supported" : "Not Supported" );
  QCRIL_LOG_CF_PKT_RIL_FN( instance_id, label );

  return supported;

} /* onSupports() */


/*===========================================================================

  FUNCTION:  onSupports_rid

===========================================================================*/
/*!
    @brief
    Determines if the RIL supports the given RIL_REQUEST by
    RIL instance RID0.

    @return
    1 if the given RIL_REQUEST is supported
*/
/*=========================================================================*/
static int onSupports_rid
(
  int request
)
{
  /*-----------------------------------------------------------------------*/

  return onSupports( QCRIL_DEFAULT_INSTANCE_ID, request );

} /* onSupports_rid0() */


/*===========================================================================

  FUNCTION:  onCancel

===========================================================================*/
/*!
    @brief
    The RIL is supposed to make a "best effort" to cancel the outstanding
    RIL_REQUEST with the given RIL_Token.  For now the hope is that out
    "best effort" can be "no effort".

    @return
    The current state of the RIL
*/
/*=========================================================================*/
static void onCancel
(
  qcril_instance_id_e_type instance_id,
  RIL_Token t
)
{
  /*-----------------------------------------------------------------------*/

  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );

  /*-----------------------------------------------------------------------*/

  QCRIL_NOTUSED( t );

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_LOG_CF_PKT_RIL_FN( instance_id, "onCancel()" );

} /* onCancel() */


/*===========================================================================

  FUNCTION:  onCancel_rid

===========================================================================*/
/*!
    @brief
    The RIL is supposed to make a "best effort" to cancel the outstanding
    RIL_REQUEST with the given RIL_Token for RIL instance RID0.
    For now the hope is that out "best effort" can be "no effort".

    @return
    The current state of the RIL
*/
/*=========================================================================*/
static void onCancel_rid
(
  RIL_Token t
)
{
  /*-----------------------------------------------------------------------*/

  onCancel( QCRIL_DEFAULT_INSTANCE_ID, t );

} /* onCancel_rid0()*/


/*===========================================================================

  FUNCTION:  getVersion

===========================================================================*/
/*!
    @brief
    Used to query what version of the RIL is present

    @return
    A string describing this RIL version
*/
/*=========================================================================*/
static const char *getVersion
(
  qcril_instance_id_e_type instance_id
)
{
  char *version = "Qualcomm RIL 1.0";

  /*-----------------------------------------------------------------------*/

  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );

  /*-----------------------------------------------------------------------*/


  QCRIL_LOG_DEBUG( "version %s", version );

  QCRIL_LOG_CF_PKT_RIL_FN( instance_id, "getVersion() - Qualcomm RIL 1.0 (QMI)" );

  return version;

} /* getVersion() */


/*===========================================================================

  FUNCTION:  getVersion_rid

===========================================================================*/
/*!
    @brief
    Used to query what version of the RIL is present in RID0

    @return
    A string describing this RIL version
*/
/*=========================================================================*/
static const char *getVersion_rid
(
  void
)
{
  /*-----------------------------------------------------------------------*/

  qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, qmi_ril_bootup_actition_on_rild_atel_link_connect, NULL, NULL );

  return getVersion( QCRIL_DEFAULT_INSTANCE_ID );

} /* getVersion_rid0() */

/*===========================================================================

  FUNCTION:  qcril_delay_timed_cb

===========================================================================*/
/*!
    @brief
    Handle delay timer expiration.

    @return
    None.
*/
/*=========================================================================*/
static void qcril_delay_timed_cb
(
  void *param
)
{
  QCRIL_LOG_DEBUG( "Delay Timer expired with ID %d", (uint32)(uintptr_t) param );

}; /* qcril_delay_timed_cb */


/*===========================================================================

  FUNCTION:  qcril_init_state

===========================================================================*/
/*!
    @brief
    Initialize states of QCRIL.

    @return
    None.
*/
/*=========================================================================*/
static void qcril_init_state
(
  void
)
{
  qcril_instance_id_e_type instance_id;
  qcril_arb_state_info_struct_type *s_ptr = NULL;
  int len;
  char args[ PROPERTY_VALUE_MAX ];
  char *end_ptr = NULL;
  unsigned long ret_val;

  /*-----------------------------------------------------------------------*/

  /* Initialize TIMER ID */
  qcril_timer_id = 1;

  /* Allow cache */
  qcril_state = (qcril_arb_state_struct_type *) qcril_arb_allocate_cache( QCRIL_ARB_CACHE_STATE );

  if(qcril_state != NULL)
  {
  /* Initialize Timer ID mutex */
  pthread_mutex_init( &qcril_state->mutex, NULL );

  /* initialize Timed Callback list */
  pthread_mutex_init( &qcril_timed_callback_list_mutex, NULL);
  qcril_timed_callback_list = NULL;

  pthread_mutex_init( &qmi_ril_android_pending_unsol_resp.pending_unsol_resp_mutex, NULL);

  /* conditionally initialize heap memory tracker list*/
  is_heap_memory_tracked = 0;
  property_get( QCRIL_TRACK_HEAP_MEM, args, "" );
  len = strlen( args );
  if ( len > 0 )
  {
    ret_val = strtoul( args, &end_ptr, 0 );
    if ( ( errno == ERANGE ) && ( ret_val == ULONG_MAX ) )
    {
      QCRIL_LOG_ERROR( "Fail to convert QCRIL_TRACK_HEAP_MEM %s", QCRIL_TRACK_HEAP_MEM );
    }
    else if ( ret_val > 1 )
    {
      QCRIL_LOG_ERROR( "Invalid saved QCRIL_TRACK_HEAP_MEM %ld, use default", ret_val );
    }
    else
    {
      is_heap_memory_tracked = ( uint8 ) ret_val;
    }
  }
  if ( 1 == is_heap_memory_tracked )
  {
    pthread_mutex_init( &qcril_heap_memory_list_mutex, NULL);
    qcril_heap_memory_list = NULL;
    heap_memory_id = 0;
    qcril_setup_timed_callback(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID,
                               qcril_print_heap_memory_list, &HEAP_MEM_LIST_PRINT_TIMEVAL_DELAY, NULL );
  }

  memset( &qmi_ril_common_critical_section, 0, sizeof( qmi_ril_common_critical_section ) );
  pthread_mutexattr_init( &qmi_ril_common_critical_section.lock_mtx_atr );
  pthread_mutex_init( &qmi_ril_common_critical_section.lock_mutex, &qmi_ril_common_critical_section.lock_mtx_atr );

  pthread_mutexattr_init (&esoc_info.mdm_attr);
  pthread_mutex_init (&esoc_info.mdm_mutex, &esoc_info.mdm_attr);

  /* Initialize internal data */
  for ( instance_id = 0; instance_id < QCRIL_MAX_INSTANCE_ID; instance_id++ )
  {
    s_ptr = &qcril_state->info[ instance_id ];


    /* Stay in GW SIM Not Ready State till Modem reports an update on GW SIM State */
    s_ptr->pri_gw_sim_state = QCRIL_SIM_STATE_NOT_READY;
    s_ptr->sec_gw_sim_state = QCRIL_SIM_STATE_NOT_READY;

    /* Stay in CDMA SIM Not Ready State till Modem reports an update on CDMA SIM State */
    s_ptr->pri_cdma_sim_state = QCRIL_SIM_STATE_NOT_READY;
    s_ptr->sec_cdma_sim_state = QCRIL_SIM_STATE_NOT_READY;

    /* No RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED needed to be reported at power-up.
    ** RILD itself will invoke the unsol event when it's done register and query
    ** QCRIL for the Radio State. */
  }

  /* This is a workaround for a bug in ril.cpp where it starts ril_event_loop()
     before adding s_fdlisten to readFds.  When ril_event_loop first calls
     select() it is only waiting on s_fdWakeupRead.  Setting this timer wakes up
     the select, and when it blocks again s_fdlisten is in the fd_set.  Otherwise
     ril_event_loop() is blocked forever, even if Java connects to the socket. */
  qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, qcril_delay_timed_cb,
                                         &TIMEVAL_DELAY, NULL );
  }
  else
  {
    QCRIL_LOG_FATAL("FATAL : CHECK FAILED");
  }

} /* qcril_init_state */


/*===========================================================================

  FUNCTION:  qcril_init

===========================================================================*/
/*!
    @brief
    Initializes all QCRIL modules

    @return
    None.
*/
/*=========================================================================*/
void qcril_init
(
  int argc,
  char **argv
)
{
  (void) argc;
  (void) argv;
  /*-----------------------------------------------------------------------*/

  /* Initialize the Arbitration module. Should be done before any other initialization */
  qcril_arb_init();

  /* Initialize QCRIL states */
  qcril_init_state();

  // init oem handling fw
  qmi_ril_oem_hook_init();

  qcril_db_init();

  /* ###############################################################################################
        !!!IMPORTANT!!!
  ##################################################################################################

   (1) Use the state mutex to block QCRIL states update that could possibily triggered by any AMSS
       command callback, or AMSS event before the completion of radio state initialization.

   (2) Don't call qcril_process_event inside this block. Doing so, will end up in mutex deadlock.

  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                                            QCRIL STATES INITIALIZATION BEGIN
  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */


  QCRIL_MUTEX_LOCK( &qcril_state->mutex, "qcril_state_mutex" );

  qcril_reqlist_init();

  QCRIL_MUTEX_UNLOCK( &qcril_state->mutex, "qcril_state_mutex" );

  /* initialize supress list */
  qcril_request_suppress_list_init();

  /* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                                         QCRIL STATES INITIALIZATION END
     <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

  qmi_ril_qmi_client_pre_initialization_init();
  qmi_ril_qmi_client_pre_initialization_acquire();

#ifndef QMI_RIL_UTF
  qmi_ril_file_observer_init();
#endif

  // per technology pre-init
  qcril_ims_flow_control_pre_init();
#ifndef QMI_RIL_UTF
  qcril_am_pre_init();
#endif

#ifndef QMI_RIL_UTF
  qcril_qmi_radio_config_socket_init();
  settingsd_client_start();
#endif

#ifdef QMI_RIL_UTF
  qmi_ril_reset_baseband_rat_option();
  qcril_qmi_nas_pre_init();
#endif
  qcril_qmi_pdc_pre_init();
  qcril_qmi_client_bootup_init();

  QCRIL_LOG_FUNC_RETURN();

} /* qcril_init() */

//===========================================================================
//qmi_ril_initiate_bootup
//===========================================================================
void qmi_ril_initiate_bootup(void)
{
#ifndef QMI_RIL_UTF
  qmi_ril_bootup_perform_core_or_start_polling(NULL);
#else
  qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, qmi_ril_bootup_perform_core_or_start_polling, NULL, NULL );
#endif
} // qmi_ril_initiate_bootup

//===========================================================================
// qmi_ril_oem_hook_init
//===========================================================================
void qmi_ril_oem_hook_init()
{
  memset( &qmi_ril_oem_hook_overview, 0, sizeof( qmi_ril_oem_hook_overview ) );

  pthread_mutexattr_init( &qmi_ril_oem_hook_overview.overview_lock_mtx_atr );
  pthread_mutex_init( &qmi_ril_oem_hook_overview.overview_lock_mutex, &qmi_ril_oem_hook_overview.overview_lock_mtx_atr );
  qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root = NULL;
} // qmi_ril_oem_hook_init

//===========================================================================
//qmi_ril_bootup_perform_core_or_start_polling
//===========================================================================
void qmi_ril_bootup_perform_core_or_start_polling(void * params)
{
  RIL_Errno init_res;

  qmi_ril_main_thread_id = pthread_self();

  qmi_ril_set_thread_name( qmi_ril_fw_get_main_thread_id(), QMI_RIL_QMI_MAIN_THREAD_NAME);
  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_NOTUSED( params );

  qmi_ril_wave_modem_status(); // this should result in "modem unavailble" report

  qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_PENDING ); // for consistency

  qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_ONGOING );
  init_res = qmi_ril_core_init();
  QCRIL_LOG_INFO("sees %d from qmi_ril_core_init()", (int)init_res );
  if ( RIL_E_SUCCESS == init_res )
  {
    qmi_ril_wave_modem_status(); // this should trigger reporting of modem state to Android
    qmi_ril_core_init_enter_warp();
  }
  else
  {
    qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_PENDING );
    qmi_ril_initiate_core_init_retry();
  }

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_bootup_perform_core_or_start_polling
//qmi_ril_core_init
//===========================================================================
RIL_Errno qmi_ril_core_init(void)
{
  RIL_Errno res = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_FUNC_ENTRY();

  qcril_event_suspend(); // to ensure atomic init flow cross sub domains
  do
  {
    if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_OEM_SOCKET))
    {
      QCRIL_LOG_INFO( "%s Init OEM socket thread", __FUNCTION__ );
      qcril_qmi_oemhook_agent_init();
    }

    res = qcril_qmi_client_init();
    if ( RIL_E_SUCCESS != res )
      break;

    qcril_other_init();

#ifndef QMI_RIL_UTF
    // DATA STUB qcril_data_init();
#endif

    qcril_qmi_nas_dms_commmon_post_init();
    // after legacy qmi_client_init_complete, check early radio power
    qcril_qmi_nas_start_timer_if_early_radio_power_req_came();

    // disable data dormancy indication
    qcril_qmi_nas_disable_data_dormancy_indication();
  } while (FALSE);
  qcril_event_resume();

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);

  return res;
} // qmi_ril_core_init
//===========================================================================
//qmi_ril_core_init_enter_warp
//===========================================================================
void qmi_ril_core_init_enter_warp()
{
    qmi_ril_gen_operational_status_type cur_status;

    QCRIL_LOG_FUNC_ENTRY();
    cur_status = qmi_ril_get_operational_status();

    if( !( cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDING ||
         cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDED )
      )
    {
        // SMS REFACTOR (void)qcril_sms_perform_initial_configuration(); // SMS initialization done right away
        QCRIL_LOG_INFO( "QMI RIL! SMS init right away for both Non DSDS or DSDS" );

        if ( !qmi_ril_is_multi_sim_feature_supported() || qcril_qmi_nas_is_sim_provisioned() )
        {
            QCRIL_LOG_INFO( "!QMI RIL! 2nd phase init for NON - DSDS" );
            qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_UNRESTRICTED );
        }
    }
    QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_core_init_enter_warp

//===========================================================================
//qmi_ril_initiate_core_init_retry
//===========================================================================
RIL_Errno qmi_ril_initiate_core_init_retry(void)
{
  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  pthread_attr_t attr;
  int conf;

#ifdef QMI_RIL_UTF
  pthread_attr_init (&attr);
  conf = utf_pthread_create_handler(&qmi_ril_init_retry_thread_pid, &attr, qmi_ril_core_init_kicker_thread_proc, NULL);
#else
  pthread_attr_init (&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  conf = pthread_create(&qmi_ril_init_retry_thread_pid, &attr, qmi_ril_core_init_kicker_thread_proc, NULL);
#endif
  qmi_ril_set_thread_name(qmi_ril_init_retry_thread_pid, QMI_RIL_CORE_INIT_KICKER_THREAD_NAME);

  pthread_attr_destroy(&attr);

  res =  (conf < 0 ) ? RIL_E_GENERIC_FAILURE : RIL_E_SUCCESS;
  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);

  return res;
} // qmi_ril_initiate_core_init_retry

//===========================================================================
//qmi_ril_core_init_kicker_main_threaded_proc
//===========================================================================
void qmi_ril_core_init_kicker_main_threaded_proc(void* empty_param)
{
  RIL_Errno core_init_res = RIL_E_GENERIC_FAILURE;
  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_NOTUSED( empty_param );
  qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_ONGOING );
  core_init_res = qmi_ril_core_init();
  QCRIL_LOG_INFO( "iteration - %d", (int) core_init_res );

  if ( RIL_E_SUCCESS == core_init_res )
  {
    qmi_ril_core_init_enter_warp();
    qmi_ril_wave_modem_status(); // this should trigger reporting of modem state to Android
  }
  else
  {
    qmi_ril_set_operational_status( QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_PENDING );
    qmi_ril_initiate_core_init_retry();
  }

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_core_init_kicker_main_threaded_proc
//===========================================================================
//qmi_ril_core_init_kicker_thread_proc
//===========================================================================
void * qmi_ril_core_init_kicker_thread_proc(void* empty_param)
{
  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_NOTUSED( empty_param );
  sleep( QMI_RIL_INI_RETRY_GAP_SEC );
  qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, qmi_ril_core_init_kicker_main_threaded_proc, NULL, NULL );

  QCRIL_LOG_FUNC_RETURN();

  qmi_ril_clear_thread_name(pthread_self());
  return NULL;
}  // qmi_ril_core_init_kicker_thread_proc

/*===========================================================================

  FUNCTION:  qcril_release

===========================================================================*/
/*!
    @brief
    Release AMSS client objects.

    @return
    None.
*/
/*=========================================================================*/
void qcril_release
(
  void
)
{
  /*-----------------------------------------------------------------------*/

  QCRIL_LOG_FUNC_ENTRY();

  /* For QMI_VOICE, NAS/DMS, WMS */
  qcril_qmi_client_release();

  if ( qmi_ril_is_multi_sim_feature_supported() )
  {
      qcril_ipc_release();
  }
  qcril_log_cleanup();
  if (qcril_qmi_is_pm_voting_feature_supported())
  {
    qmi_ril_peripheral_mng_deinit();
  }
} /* qcril_release()*/


//===========================================================================
// qcril_get_empty_binary_data_type
//===========================================================================
qcril_binary_data_type qcril_get_empty_binary_data_type()
{
    qcril_binary_data_type res;
    res.len = 0;
    res.data = NULL;
    return res;
} // qcril_get_empty_binary_data_type

//===========================================================================
// qcril_is_binary_data_empty
//===========================================================================
boolean qcril_is_binary_data_empty(qcril_binary_data_type bin_data)
{
    return !bin_data.len || !bin_data.data;
} // qcril_is_binary_data_empty

//===========================================================================
// qcril_find_pattern
//===========================================================================
qcril_binary_data_type qcril_find_pattern(qcril_binary_data_type bin_data, const char *pattern)
{
    size_t i;
    for (i = 0; i < bin_data.len - strlen(pattern); ++i)
    {
        boolean match = TRUE;
        size_t j;
        for (j = 0; j < strlen(pattern); ++j)
        {
            if (bin_data.data[i + j] != pattern[j])
            {
                match = FALSE;
                break;
            }
        }
        if (match)
        {
            bin_data.data += i;
            bin_data.len -= i;
            return bin_data;
        }
    }
    return qcril_get_empty_binary_data_type();
} // qcril_find_pattern

/*===========================================================================

  FUNCTION: qcril_qmi_load_esoc_info

===========================================================================*/
/*!
    @brief
    Loads esoc info

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_load_esoc_info
(
    void
)
{
    struct dev_info devinfo;

    do {
        if (get_system_info(&devinfo) != RET_SUCCESS)
        {
            QCRIL_LOG_ERROR("Could not retrieve esoc info");
            break;
        }

        if (devinfo.num_modems != 1)
        {
            QCRIL_LOG_ERROR("Unexpected number of modems %d",
                             devinfo.num_modems);
            break;
        }

        /* Read esoc node, to be used if
         * peripheral manager is not supported */
        strlcpy(esoc_info.esoc_node,
                devinfo.mdm_list[0].powerup_node,
                sizeof(esoc_info.esoc_node));

        /* Read modem name, to be used to register with
         * peripheral manager */
        strlcpy(esoc_info.modem_name,
                devinfo.mdm_list[0].mdm_name,
                sizeof(esoc_info.modem_name));

        /* Read link name to find out the transport medium
         * to decide on qmi port */
        strlcpy(esoc_info.link_name,
                devinfo.mdm_list[0].mdm_link,
                sizeof(esoc_info.link_name));

        QCRIL_LOG_INFO("Read esoc info: modem name: %s, "
                       "link name: %s, esoc_node: %s",
                       esoc_info.modem_name, esoc_info.link_name,
                       esoc_info.esoc_node);

        esoc_info.type = devinfo.mdm_list[0].type;
        QCRIL_LOG_INFO("Mdm type (0-External, 1-Internal):%d",
                        esoc_info.type);

    } while (0);

    return;
}

/*===========================================================================

  function: qcril_qmi_get_esoc_modem_name

===========================================================================*/
/*!
    @brief
    returns esoc modem name

    @return
    esoc modem name
*/
/*=========================================================================*/
char *qcril_qmi_get_esoc_modem_name
(
    void
)
{
    char *modem_name= NULL;
    if (strlen(esoc_info.modem_name) > 0)
    {
        modem_name = esoc_info.modem_name;
    }

    return modem_name;
}

/*===========================================================================

  FUNCTION: qcril_qmi_load_esoc_and_register_with_pm

===========================================================================*/
/*!
    @brief
    Loads esoc info

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_load_esoc_and_register_with_pm
(
    void
)
{
    char* esoc_modem_name;
    qcril_qmi_load_esoc_info();
    esoc_modem_name = qcril_qmi_get_esoc_modem_name();
    QCRIL_LOG_INFO("load_esoc_and_register_with_pm: modem_name = %s",
                   ((NULL != esoc_modem_name)? esoc_modem_name: "NULL"));
    if (NULL != esoc_modem_name && !qmi_ril_peripheral_mng_init(esoc_modem_name))
    {
        QCRIL_LOG_INFO("peripheral manager feature is enabled");
        esoc_info.pm_feature_supported = TRUE;
    }
}

//=============================================================================
// FUNCTION: qmi_ril_get_stack_id
//
// DESCRIPTION:
// returns the modem stack id associated with current RIL instance
//
// RETURN: 0 | 1 | 2 - primary | secondary | tertiary stack id
//=============================================================================
qcril_modem_stack_id_e_type qmi_ril_get_stack_id
(
  qcril_instance_id_e_type instance_id
)
{
  QCRIL_NOTUSED(instance_id);
  return qcril_qmi_get_modem_stack_id();
}

int qcril_qmi_modem_power_voting_state()
{
    int ret;
    QCRIL_MDM_LOCK();
    ret = esoc_info.voting_state;
    QCRIL_MDM_UNLOCK();
    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

/*===========================================================================

  function: qcril_qmi_get_esoc_link_name

===========================================================================*/
/*!
    @brief
    returns esoc mdm link name

    @return
    esoc device node link name
*/
/*=========================================================================*/
char *qcril_qmi_get_esoc_link_name
(
    void
)
{
    char *link_name = NULL;
    if (strlen(esoc_info.link_name) > 0)
    {
        link_name = esoc_info.link_name;
    }

    return link_name;
}

/*===========================================================================

  function: qcril_qmi_get_esoc_node_name

===========================================================================*/
/*!
    @brief
    returns esoc node name

    @return
    esoc node name
*/
/*=========================================================================*/
char *qcril_qmi_get_esoc_node_name
(
    void
)
{
    char *modem_name= NULL;
    if (strlen(esoc_info.esoc_node) > 0)
    {
        modem_name = esoc_info.esoc_node;
    }

    return modem_name;
}
/*===========================================================================

  FUNCTION: qcril_qmi_modem_power_process_regular_shutdown

===========================================================================*/
/*!
    @brief
        release vote on modem

    @return
        None
*/
/*=========================================================================*/
void qcril_qmi_modem_power_process_regular_shutdown
(
    void
)
{
    QCRIL_MDM_LOCK();
    if (qcril_qmi_is_pm_voting_feature_supported())
    {
        qmi_ril_peripheral_mng_release_vote();
    }
    else if (qcril_qmi_is_esoc_voting_feature_supported())
    {
        close (esoc_info.esoc_fd);
    }

    QCRIL_MDM_UNLOCK();

    if (qcril_qmi_is_pm_voting_feature_supported())
    {
        QCRIL_LOG_INFO("released vote for modem %s",
                        qcril_qmi_get_esoc_modem_name()?
                            qcril_qmi_get_esoc_modem_name() : "null");
    }
    else if (qcril_qmi_is_esoc_voting_feature_supported())
    {
        QCRIL_LOG_INFO("released vote for  node %s fd %d",
                        qcril_qmi_get_esoc_node_name()?
                            qcril_qmi_get_esoc_node_name() : "null",
                        esoc_info.esoc_fd);
    }

} // qcril_qmi_modem_power_process_regular_shutdown

/*===========================================================================

  FUNCTION: qcril_qmi_vote_for_modem_up_using_pm

===========================================================================*/
/*!
    @brief
        adds vote for modem through peripheral manager,
        so that pil is loaded.

    @return
        None
*/
/*=========================================================================*/
void qcril_qmi_vote_for_modem_up_using_pm
(
    void
)
{
    char   *esoc_modem_name;
    int     ret;

    QCRIL_LOG_FUNC_ENTRY();

    esoc_modem_name = qcril_qmi_get_esoc_modem_name();
    if (esoc_modem_name)
    {
        ret = qmi_ril_peripheral_mng_vote();
        if (ret)
        {
            QCRIL_LOG_ERROR("Could not vote for modem %s", esoc_modem_name);
        }
        else
        {
            esoc_info.voting_state = 1;
            QCRIL_LOG_INFO("vote activated for modem %s", esoc_modem_name);
        }
    }
    else
    {
        QCRIL_LOG_ERROR("ESOC node is not available");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_vote_for_modem_up_using_pm

/*===========================================================================

  FUNCTION: qcril_qmi_vote_for_modem_up_using_esoc

===========================================================================*/
/*!
    @brief
        adds vote for modem using esoc , so that pil is loaded.

    @return
        None
*/
/*=========================================================================*/
void qcril_qmi_vote_for_modem_up_using_esoc
(
    void
)
{
    char *esoc_node_name;

    QCRIL_LOG_FUNC_ENTRY();

    esoc_node_name = qcril_qmi_get_esoc_node_name();
    if (esoc_node_name)
    {
        if (!access (esoc_node_name, F_OK))
        {
            QCRIL_LOG_INFO("esoc feature is enabled");
            esoc_info.esoc_feature_supported = TRUE;
            if(esoc_info.type == MDM_TYPE_EXTERNAL)
            {
                if(NULL != esoc_node_name)
                {
                    esoc_info.esoc_fd = open(esoc_node_name, O_RDONLY);
                }

                if (RIL_VALID_FILE_HANDLE > esoc_info.esoc_fd)
                {
                    esoc_info.esoc_feature_supported = FALSE;
                    QCRIL_LOG_ERROR("Can not open file %s", esoc_node_name);
                }
                else
                {
                    esoc_info.voting_state = 1;
                    QCRIL_LOG_INFO("vote activated for node %s, fd %d",
                                    esoc_node_name, esoc_info.esoc_fd);
                }
            }
            else
            {
                esoc_info.esoc_feature_supported = FALSE;
                QCRIL_LOG_INFO("Internal modem - esoc file open not required");
            }
        }
        else
        {
            QCRIL_LOG_ERROR("ESOC node %s not accessible", esoc_node_name);
        }
    }
    else
    {
        QCRIL_LOG_ERROR("ESOC node is not available");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_vote_for_modem_up_using_esoc

/*===========================================================================

  FUNCTION: qcril_qmi_modem_power_process_bootup

===========================================================================*/
/*!
    @brief
        adds vote for modem, so that pil is loaded.

    @return
        None
*/
/*=========================================================================*/
void qcril_qmi_modem_power_process_bootup
(
    void
)
{
    QCRIL_LOG_FUNC_ENTRY();

    qcril_qmi_nas_modem_power_load_apm_mdm_not_pwdn();
    if (qcril_qmi_is_pm_voting_feature_supported())
    {
        qcril_qmi_vote_for_modem_up_using_pm();
    }
    else
    {
        qcril_qmi_vote_for_modem_up_using_esoc();
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_modem_power_process_bootup

/*===========================================================================

  FUNCTION: qcril_qmi_is_pm_voting_feature_supported

===========================================================================*/
/*!
    @brief
    Retrieve if peripheral manager voting feature is supported

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
boolean qcril_qmi_is_pm_voting_feature_supported
(
    void
)
{
    boolean ret;
    ret = esoc_info.pm_feature_supported;
    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
} // qcril_qmi_is_pm_voting_feature_supported

void qcril_qmi_modem_power_set_voting_state(int state)
{
    QCRIL_MDM_LOCK();
    esoc_info.voting_state = state;
    QCRIL_MDM_UNLOCK();
    QCRIL_LOG_INFO("voting state set to %d", state);
}

/*===========================================================================

  FUNCTION: qcril_qmi_modem_power_process_apm_off

===========================================================================*/
/*!
    @brief
    vote to power up modem

    @return
    none
*/
/*=========================================================================*/
void qcril_qmi_modem_power_process_apm_off
(
    void
)
{
    QCRIL_LOG_FUNC_ENTRY();
    char *esoc_modem_name = NULL;
    char *esoc_node_name  = NULL;
    int   ret = -1;

    QCRIL_MDM_LOCK();
    if (qcril_qmi_is_pm_voting_feature_supported())
    {
        esoc_modem_name = qcril_qmi_get_esoc_modem_name();
        ret = qmi_ril_peripheral_mng_vote();
        if (!ret)
        {
            esoc_info.voting_state = 1;
        }
    }
    else if (qcril_qmi_is_esoc_voting_feature_supported())
    {
        esoc_node_name = qcril_qmi_get_esoc_node_name();
        if (NULL != esoc_node_name)
        {
            esoc_info.esoc_fd = open(esoc_node_name, O_RDONLY);
            if (esoc_info.esoc_fd >= RIL_VALID_FILE_HANDLE)
            {
                esoc_info.voting_state = 1;
            }
        }
    }

    QCRIL_MDM_UNLOCK();

    if (esoc_info.voting_state)
    {
        QCRIL_LOG_INFO("vote activated for modem: %s, node: %s",
                        esoc_modem_name ? esoc_modem_name : "null",
                        esoc_node_name ? esoc_node_name : " null");
    }
    else
    {
        QCRIL_LOG_ERROR("Could not vote for modem: %s, node: %s",
                        esoc_modem_name ? esoc_modem_name : "null",
                        esoc_node_name ? esoc_node_name : " null");
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION: qcril_qmi_is_esoc_voting_feature_supported

===========================================================================*/
/*!
    @brief
    Retrieve if esoc voting feature is supported

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
boolean qcril_qmi_is_esoc_voting_feature_supported
(
    void
)
{
    boolean ret;
    ret = esoc_info.esoc_feature_supported;
    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
} // qcril_qmi_is_esoc_voting_feature_supported
/*===========================================================================

    FUNCTION: qcril_qmi_modem_power_is_voting_feature_supported

===========================================================================*/
/*!
    @brief
    Retrieve if voting feature is supported

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
boolean qcril_qmi_modem_power_is_voting_feature_supported
(
    void
)
{
    boolean ret;
    ret = (qcril_qmi_is_esoc_voting_feature_supported() ||
            qcril_qmi_is_pm_voting_feature_supported());
    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
} // qcril_qmi_modem_power_is_voting_feature_supported

#ifdef RIL_SHLIB
/*===========================================================================

  FUNCTION:  RIL_Init

===========================================================================*/
/*!
    @brief
    Returns the current state of the RIL

    @return
    The current state of the RIL
*/
/*=========================================================================*/
const RIL_RadioFunctions *legacy_RIL_Init
(
  const struct RIL_Env *env,
  qcril_instance_id_e_type instance_id,
  int argc,
  char **argv
)
{
  qmi_ril_set_thread_name( pthread_self() , QMI_RIL_QMI_RILD_THREAD_NAME);

  /* Ignore SIGPIPE signal so that rild does not crash
     even if the other end of the socket is closed abruptly. */
  signal(SIGPIPE, SIG_IGN);

  qcril_qmi_nas_initiate_radio_power_process();
  qcril_log_init();

  /* Load eSOC info and register with peripheral manager */
  qcril_qmi_load_esoc_and_register_with_pm();

  qmi_ril_process_instance_id = instance_id;
  qmi_ril_sim_slot = 0; // use 1st slot as default

  if (qmi_ril_process_instance_id == QCRIL_SECOND_INSTANCE_ID)
  { // 2nd RIL instance - 1, only for DSDS or DSDA
    if ( qmi_ril_is_multi_sim_feature_supported() )
    {
       qmi_ril_sim_slot = QCRIL_SECOND_INSTANCE_ID; // use 2nd slot for DSDS/TSTS
    }
    else if ( qmi_ril_is_feature_supported(QMI_RIL_FEATURE_DSDA)  ||
              qmi_ril_is_feature_supported(QMI_RIL_FEATURE_DSDA2) )
    {
       qmi_ril_sim_slot = 0; // use 1st slot for DSDA
    }
    else
    {
       QCRIL_LOG_ERROR("Unsupported configuration, can't determine sim slot");
    }
  }
  else if (qmi_ril_process_instance_id == QCRIL_THIRD_INSTANCE_ID)
  { // 3rd RIL instance, only for TSTS
    if ( qmi_ril_is_feature_supported(QMI_RIL_FEATURE_TSTS) )
    {
       qmi_ril_sim_slot = QCRIL_THIRD_INSTANCE_ID; // use 3rd slot for TSTS
    }
    else
    {
       QCRIL_LOG_ERROR("Unsupported configuration, can't determine sim slot");
    }
  }

  qmi_ril_set_sim_slot(qmi_ril_sim_slot);
  qmi_ril_set_process_instance_id(qmi_ril_process_instance_id);

  if ( qmi_ril_is_multi_sim_feature_supported() )
  {
      qcril_ipc_init();
  }

  QCRIL_LOG_DEBUG( "RIL %d, running RIL_Init()", qmi_ril_process_instance_id );

  // o maintain compatibility with data and UIM code which use instance_id and may respond on "second instance" context
  qcril_response_api[ QCRIL_DEFAULT_INSTANCE_ID ] = (struct RIL_Env *) env;
  qcril_response_api[ QCRIL_SECOND_INSTANCE_ID ] = (struct RIL_Env *) env;
  // TODO_TSTS: Check if this is required. Seems not required.
  qcril_response_api[ QCRIL_THIRD_INSTANCE_ID ] = (struct RIL_Env *) env;

  // flow control
  qmi_ril_fw_android_request_flow_control_init();

  // unsol response dispatchable control
  qmi_ril_init_android_unsol_resp_pending_list();

  // Initialize the event thread */
  qcril_event_init();

  // Initialize QCRIL
  qcril_init(argc, argv);

  // Start event thread
  qcril_event_start();

  //core_handler_start();
  //cri_core_start();
  //util_timer_start();

  qcril_log_timer_init();

  // start bootup if applicable
  qmi_ril_initiate_bootup();

  return &qcril_request_api[ QCRIL_DEFAULT_INSTANCE_ID ];

} /* RIL_Init() */
#elif defined (PLATFORM_LTK)
int qcril_main()
{
  const RIL_RadioFunctions *rilapi = &qcril_api;

  /*-----------------------------------------------------------------------*/

  qcril_init(0, NULL);

} /* qcril_main() */
#else
int main(int argc, char *argv[])
{
  const RIL_RadioFunctions *rilapi = &qcril_api;

  /*-----------------------------------------------------------------------*/
  qcril_init(argc, argv);


  /* Verify QCRIL access function */
  qcril_unit_test( rilapi );

  /* Verify QCRIL Powerup Unit Test Suite */
  qcril_powerup_unit_test();

  /* Verify QCRIL State Transition Test Suite */
  qcril_state_transition_unit_test();

  /* Run ReqList Unit Test Suite */
  qcril_reqlist_unit_test();

  /* Run CM CList Unit Test Suite */
  qcril_cm_clist_unit_test();

  /* Run CM Phone Services Unit Test Suite */
  qcril_cm_phonesvc_unit_test( rilapi );

  /* Run CM Serving System Services Unit Test Suite */
  qcril_cm_srvsys_unit_test( rilapi );

  /* Run CM Call Services Unit Test Suite */
  qcril_cm_callsvc_unit_test( rilapi );

  /* Run CM Inband Services Unit Test Suite */
  qcril_cm_inband_unit_test( rilapi );

  /* Run Other Unit Test Suite */
  qcril_other_unit_test( rilapi );

  /* Run Oem Hook Unit Test Suite */
  qcril_oem_hook_unit_test( rilapi );

  return 0;

} /* qcril_main */
#endif /* RIL_SHLIB */

//===========================================================================
// qmi_ril_fw_android_request_get_handling_capabilities
//===========================================================================
uint32_t qmi_ril_fw_android_request_get_handling_capabilities( int android_request_id )
{
  uint32_t res;

  switch ( android_request_id )
  {
    case RIL_REQUEST_VOICE_RADIO_TECH:
    case RIL_REQUEST_SIGNAL_STRENGTH:
    case RIL_REQUEST_VOICE_REGISTRATION_STATE:
    case RIL_REQUEST_DATA_REGISTRATION_STATE:
    case RIL_REQUEST_IMS_REGISTRATION_STATE:
    case RIL_REQUEST_OPERATOR:
    case RIL_REQUEST_SCREEN_STATE:
    case RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER:
    case RIL_REQUEST_SEND_DEVICE_STATE:
    case RIL_REQUEST_SET_LOCATION_UPDATES:
    case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE:
    case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE:
    case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE:
    case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE:
    case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
    case RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE:
    case RIL_REQUEST_QUERY_TTY_MODE:
    case RIL_REQUEST_CDMA_SUBSCRIPTION:
    case RIL_REQUEST_DEVICE_IDENTITY:
    case RIL_REQUEST_BASEBAND_VERSION:
    case RIL_REQUEST_GET_IMEI:
    case RIL_REQUEST_GET_IMEISV:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF,
                                                               QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_MULTIPLE_DEF );
      break;

    case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_NO_AUTO_RESPONSE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF,
                                                               96
                                                               );
      break;

    case RIL_REQUEST_SETUP_DATA_CALL:
    case RIL_REQUEST_DEACTIVATE_DATA_CALL:
    case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE:
    case RIL_REQUEST_DATA_CALL_LIST:
    case RIL_REQUEST_GET_DATA_CALL_PROFILE:
    case RIL_REQUEST_SET_DATA_PROFILE:
#ifdef FEATURE_DATA_LQE
    case RIL_REQUEST_START_LCE:
    case RIL_REQUEST_STOP_LCE:
    case RIL_REQUEST_PULL_LCEDATA:
#endif
#if (RIL_QCOM_VERSION >= 2)
    case RIL_REQUEST_SETUP_QOS:
    case RIL_REQUEST_RELEASE_QOS:
    case RIL_REQUEST_GET_QOS_STATUS:
    case RIL_REQUEST_MODIFY_QOS:
    case RIL_REQUEST_SUSPEND_QOS:
    case RIL_REQUEST_RESUME_QOS:
#endif
    case RIL_REQUEST_OEM_HOOK_RAW:
    case RIL_REQUEST_SET_FACILITY_LOCK:
    case RIL_REQUEST_QUERY_FACILITY_LOCK:
    //case RIL_REQUEST_RADIO_POWER:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FLOW_CONTROL_EXEMPT, QMI_RIL_ZERO );
      break;

    case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC:
    case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_MULTIPLE_AUTO_DROP_ON_DIFF_PARAMS,
                                                               QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_MULTIPLE_DEF );
      break;

    case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
    case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING,
                                                               QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_MULTIPLE_DEF );
      break;

    case RIL_REQUEST_RADIO_POWER:
    case RIL_REQUEST_SHUTDOWN:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE, QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_MULTIPLE_DEF );
      break;

    case RIL_REQUEST_SET_UICC_SUBSCRIPTION:
    case RIL_REQUEST_GET_RADIO_CAPABILITY:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF,
                                                               QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_MULTIPLE_DEF );
      break;

    case RIL_REQUEST_SET_RADIO_CAPABILITY:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_SINGLE_ONLY
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF,
                                                               QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_SINGLE );
      break;

    case RIL_REQUEST_DTMF_START:
    case RIL_REQUEST_DTMF_STOP:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_NO_AUTO_RESPONSE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_VOICE_CALL_SPECIFIC_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FAMILY_RING_DEFINED_PAIR
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING,
                                                               96 );
      break;

    case RIL_REQUEST_DTMF:
    case RIL_REQUEST_CDMA_BURST_DTMF:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_NO_AUTO_RESPONSE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_VOICE_CALL_SPECIFIC_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING,
                                                               96 );
      break;

    case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
    case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
    case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
    case RIL_REQUEST_CONFERENCE:
    case RIL_REQUEST_EXPLICIT_CALL_TRANSFER:
    case RIL_REQUEST_SEPARATE_CONNECTION:
    case RIL_REQUEST_UDUB:
    case RIL_REQUEST_HANGUP:
    case RIL_REQUEST_ANSWER:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_NO_AUTO_RESPONSE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_VOICE_CALL_SPECIFIC_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING,
                                                               QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_MULTIPLE_DEF );
      break;

    case RIL_REQUEST_SET_INITIAL_ATTACH_APN:
    case RIL_REQUEST_ALLOW_DATA:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_NO_AUTO_RESPONSE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF,
                                                               96 );
      break;

    case RIL_REQUEST_SEND_SMS:
    case RIL_REQUEST_SEND_SMS_EXPECT_MORE:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_NO_AUTO_RESPONSE,
                                                               96 );
      break;

    case RIL_REQUEST_CDMA_SEND_SMS:
    case RIL_REQUEST_IMS_SEND_SMS:
    case RIL_REQUEST_WRITE_SMS_TO_SIM:
    case RIL_REQUEST_SMS_ACKNOWLEDGE:
    case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_NO_AUTO_RESPONSE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF,
                                                               96 );
      break;

    case RIL_REQUEST_SEND_USSD:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FLOW_CONTROL_EXEMPT, QMI_RIL_ZERO );
      break;

    case RIL_REQUEST_GET_CLIR:
    case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS:
    case RIL_REQUEST_SET_CALL_FORWARD:
    case RIL_REQUEST_QUERY_CALL_WAITING:
    case RIL_REQUEST_SET_CALL_WAITING:
    case RIL_REQUEST_CHANGE_BARRING_PASSWORD:
    case RIL_REQUEST_QUERY_CLIP:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_NO_AUTO_RESPONSE,
                                                               QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_MULTIPLE_DEF );
      break;

    case RIL_REQUEST_GET_CELL_INFO_LIST:
    case RIL_REQUEST_GET_ACTIVITY_INFO:
    case RIL_REQUEST_SET_LTE_BAND_PREF:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF,
                                                               QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_MULTIPLE_DEF );
      break;

    case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_SINGLE_ONLY,
                                                               QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_SINGLE );
      break;

    default:
      res = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_COMPOSE_CAPS( QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_SINGLE_ONLY
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF
                                                               | QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF,
                                                               QMI_RIL_FW_ANDROID_REQUEST_HNDL_QUEUE_SZ_SINGLE );
      break;
  }

  return res;
} // qmi_ril_fw_android_request_get_handling_capabilities


//===========================================================================
// qmi_ril_fw_android_request_render_execution
//===========================================================================
RIL_Errno qmi_ril_fw_android_request_render_execution( RIL_Token token,
                                                       qcril_evt_e_type request_id,
                                                       void * android_request_data,
                                                       int android_request_data_len,
                                                       qcril_instance_id_e_type  instance_id,
                                                       int * is_dedicated_thread )
{

  RIL_Errno                                audit_result;
  IxErrnoType                              event_execute_error;
  qcril_request_params_type                param;

  QCRIL_LOG_FUNC_ENTRY();

  memset( &param, 0, sizeof( param ) );
  param.event_id          = request_id;
  param.data              = android_request_data;
  param.datalen           = android_request_data_len;
  param.t                 = token;
  param.instance_id       = instance_id;
  param.modem_id          = QCRIL_DEFAULT_MODEM_ID;

  audit_result = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_DEBUG( "rendering exec for token id %" PRId32 "", qcril_log_get_token_id( token ) );

  event_execute_error = qcril_execute_event(&param, is_dedicated_thread);

  QCRIL_LOG_DEBUG("called the function");
  switch(event_execute_error)
  {
    case E_SUCCESS:
      audit_result = RIL_E_SUCCESS;
      break;
    case E_NO_MEMORY: //fallthrough
    case E_FAILURE:
      audit_result = RIL_E_GENERIC_FAILURE;
      break;
    case E_NOT_ALLOWED:
      audit_result = RIL_E_RADIO_NOT_AVAILABLE;
      break;
    case E_NOT_SUPPORTED:
      audit_result = RIL_E_REQUEST_NOT_SUPPORTED;
      break;
    default:
      audit_result = RIL_E_INTERNAL_ERR;
      break;
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET( (int)audit_result );

  return audit_result;
} // qmi_ril_fw_android_request_render_execution
//===========================================================================
// qmi_ril_fw_android_request_flow_control_info_lock
//===========================================================================
void qmi_ril_fw_android_request_flow_control_info_lock( void )
{
  pthread_mutex_lock( &qmi_ril_fw_android_request_flow_control_overview.guard.lock_mutex );
} // qmi_ril_fw_android_request_flow_control_info_lock
//===========================================================================
// qmi_ril_fw_android_request_flow_control_info_unlock
//===========================================================================
void qmi_ril_fw_android_request_flow_control_info_unlock( void )
{
  pthread_mutex_unlock( &qmi_ril_fw_android_request_flow_control_overview.guard.lock_mutex );
} // qmi_ril_fw_android_request_flow_control_info_unlock
//===========================================================================
// qmi_ril_fw_android_request_flow_control_trigger_remains
//===========================================================================
void qmi_ril_fw_android_request_flow_control_trigger_remains(qcril_timed_callback_handler_params_type * handler_params)
{
  int                                       android_request_id;

  qmi_ril_fw_android_request_kind_execution_overview_type*
                                            android_request_kind_execution_overview = NULL;
  qmi_ril_fw_android_request_kind_execution_overview_type*
                                            android_request_kind_execution_being_busy = NULL;
  uint32_t                                  android_request_handling_caps = 0;
  uint32_t                                  cap_max_queue_sz = 0;

  void*                                     invoke_data = NULL;
  int                                       invoke_data_len;

  RIL_Errno                                 audit_result;
  int                                       must_render;

  qmi_ril_fw_android_request_kind_execution_overview_type*
                                            android_request_kind_execution_exec_canidate = NULL;
  qmi_ril_fw_android_request_holder_type*   android_request_param_exec_candidate = NULL;
  int                                       oustanding_req_lookup_res;

  qcril_evt_e_type                          android_request_id_exec_candidate = NULL;

  qcril_evt_e_type                          log_android_req_id = 0;
  int32_t                                   log_projected_token_id = 0;
  int                                       log_id_dedicated_thread = FALSE;
  int                                       log_is_commenced = FALSE;


  QCRIL_LOG_FUNC_ENTRY();

  audit_result = RIL_E_GENERIC_FAILURE;

  if ( NULL != handler_params)
  {
    qcril_evt_e_type event_id = (qcril_evt_e_type)handler_params->custom_param;
    android_request_id = qcril_event_get_android_request(event_id);
    must_render        = FALSE;

    QCRIL_LOG_INFO( ".. android request id %d", (int)android_request_id );

    qmi_ril_fw_android_request_flow_control_info_lock();

    do
    {
      if ( android_request_id <= QMI_RIL_ZERO || android_request_id > QMI_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID )
        break;

      // EVENT REFACTOR TEST check index
      android_request_kind_execution_overview = &qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info[ android_request_id ];
      android_request_handling_caps = android_request_kind_execution_overview->nof_extra_holders_and_caps_and_dynamics;
      cap_max_queue_sz = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_DECOMPOSE_QUEUE_SZ( android_request_handling_caps );

      android_request_kind_execution_being_busy = qmi_ril_fw_android_request_flow_control_find_busy_kind( android_request_kind_execution_overview );
      if ( NULL != android_request_kind_execution_being_busy )
      { // occupied with request, will be retried as soon as that completes
        audit_result = RIL_E_SUCCESS;
        break;
      }

      oustanding_req_lookup_res = qmi_ril_fw_android_request_flow_control_find_request_for_execution(
                                                                                    android_request_kind_execution_overview,
                                                                                    &android_request_kind_execution_exec_canidate,
                                                                                    &android_request_param_exec_candidate
                                                                                    );
      QCRIL_LOG_INFO( ".. oustanding lookup res %d", (int)oustanding_req_lookup_res );
      if ( !oustanding_req_lookup_res )
      { // no request to exec
        audit_result = RIL_E_SUCCESS;
        break;
      }

      android_request_id_exec_candidate = android_request_kind_execution_exec_canidate->original_android_request_id;

      android_request_kind_execution_exec_canidate->token_under_execution = android_request_param_exec_candidate->token;

      QCRIL_LOG_DEBUG( "token under exec %" PRId32 "", qcril_log_get_token_id ( android_request_kind_execution_exec_canidate->token_under_execution )  );

      if ( android_request_param_exec_candidate->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_NO_PAYLOAD )
      {
        invoke_data     = NULL;
        invoke_data_len = QMI_RIL_ZERO;
      }
      else if ( android_request_param_exec_candidate->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_INT  )
      {
        invoke_data     = &android_request_param_exec_candidate->param_payload_holder.param_int;
        invoke_data_len = sizeof(int);
      }
      else if ( android_request_param_exec_candidate->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_PTR )
      {
        invoke_data     = android_request_param_exec_candidate->param_payload_holder.param_ptr;
        invoke_data_len = QMI_RIL_FW_ANDROID_REQUEST_INFO_DECOMPOSE_SZ( android_request_param_exec_candidate->param_info_and_len );
      }
      else // invalid
        break;

      must_render = TRUE;

      log_android_req_id     = android_request_kind_execution_exec_canidate->original_android_request_id;
      log_projected_token_id = qcril_log_get_token_id( android_request_param_exec_candidate->token );

    } while (FALSE);

    qmi_ril_fw_android_request_flow_control_info_unlock();

    if ( must_render )
    {
      audit_result = qmi_ril_fw_android_request_render_execution( android_request_param_exec_candidate->token,
                                                                  android_request_id_exec_candidate,
                                                                  invoke_data,
                                                                  invoke_data_len,
                                                                  qmi_ril_get_process_instance_id(),
                                                                  &log_id_dedicated_thread );

      log_is_commenced = TRUE;
    }

  }

  QCRIL_LOG_INFO( ".. audit res %d", (int)audit_result );

  if ( log_is_commenced )
  {
    QCRIL_LOG_DEBUG( "cmd %d (%s) exec t_id %" PRId32 ",thrd: %s,adt: %d",
                         qcril_event_get_android_request(log_android_req_id),
                         qcril_log_lookup_event_name(log_android_req_id),
                         log_projected_token_id,
                         log_id_dedicated_thread ? "ddctd" : "main",
                         (int) audit_result
                         );
  }

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_fw_android_request_flow_control_trigger_remains

//===========================================================================
// qmi_ril_fw_get_main_thread_id
//===========================================================================
pthread_t qmi_ril_fw_get_main_thread_id()
{
  return qmi_ril_main_thread_id;
} // qmi_ril_fw_get_main_thread_id
//===========================================================================
// qmi_ril_fw_android_request_flow_control_init
//===========================================================================
void qmi_ril_fw_android_request_flow_control_init()
{
  uint32_t  sz;
  uint32_t  android_request_id;
  uint32_t  android_request_caps;
  uint32_t  caps_nof_slots;
  qmi_ril_fw_android_request_kind_execution_overview_type*
            rec_overview = NULL;
  qmi_ril_fw_android_request_holder_type*
            holders = NULL;
  qmi_ril_fw_android_request_holder_type*
            holder_iter = NULL;
  uint32_t  iter;
  int       is_retore_mode_pref_support;

  static const int family_ring_nw_sel[] =
  {
    RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC,
    RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL,
    QMI_RIL_ZERO                                              // must be last one
  };

  static const int family_ring_mode_pref[] =
  {
    RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE,
    RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE,
    QMI_RIL_ZERO                                              // must be last one
  };

  static const int family_ring_nw_sel_and_mode_pref[] =
  {
    RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC,
    RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL,
    RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE,
    RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE,
    QMI_RIL_ZERO                                              // must be last one
  };

  static const int family_ring_dtmf[] =
  {
    RIL_REQUEST_DTMF,
    RIL_REQUEST_DTMF_START,
    RIL_REQUEST_DTMF_STOP,
    RIL_REQUEST_CDMA_BURST_DTMF,
    QMI_RIL_ZERO                                              // must be last one
  };

  static const int family_ring_incall_ss[] =
  {
    RIL_REQUEST_HANGUP,
    RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND,
    RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND,
    RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE,
    RIL_REQUEST_CONFERENCE,
    RIL_REQUEST_EXPLICIT_CALL_TRANSFER,
    RIL_REQUEST_SEPARATE_CONNECTION,
    RIL_REQUEST_UDUB,
    RIL_REQUEST_ANSWER,
    QMI_RIL_ZERO                                              // must be last one
  };

  static const int family_ring_common_ss[] =
  {
    RIL_REQUEST_QUERY_CALL_FORWARD_STATUS,
    RIL_REQUEST_SET_CALL_FORWARD,
    RIL_REQUEST_QUERY_CALL_WAITING,
    RIL_REQUEST_SET_CALL_WAITING,
    RIL_REQUEST_CHANGE_BARRING_PASSWORD,
    RIL_REQUEST_QUERY_CLIP,
    RIL_REQUEST_GET_CLIR,
    QMI_RIL_ZERO                                              // must be last one
  };

  static const int family_ring_send_sms_and_more[] =
  {
    RIL_REQUEST_SEND_SMS,
    RIL_REQUEST_SEND_SMS_EXPECT_MORE,
    QMI_RIL_ZERO                                              // must be last one
  };

  static const int family_ring_ps_attach_detach[] =
  {
    RIL_REQUEST_SET_INITIAL_ATTACH_APN,
    RIL_REQUEST_ALLOW_DATA,
    QMI_RIL_ZERO                                              // must be last one
  };

  QCRIL_LOG_FUNC_ENTRY();

  qmi_ril_get_property_value_from_integer(QCRIL_RESTORE_MODE_PREF_SUPPORT,
                                        &is_retore_mode_pref_support, 0);

  memset( &qmi_ril_fw_android_request_flow_control_overview, 0, sizeof( qmi_ril_fw_android_request_flow_control_overview ) );

  holders = NULL;

  // guard
  pthread_mutexattr_init( &qmi_ril_fw_android_request_flow_control_overview.guard.lock_mtx_atr );
  pthread_mutex_init( &qmi_ril_fw_android_request_flow_control_overview.guard.lock_mutex, &qmi_ril_fw_android_request_flow_control_overview.guard.lock_mtx_atr );

  // table
  sz = sizeof(qmi_ril_fw_android_request_kind_execution_overview_type) * (QMI_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID + 1);
  qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info =
                (qmi_ril_fw_android_request_kind_execution_overview_type *)qcril_malloc( sz );
  if ( NULL != qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info )
  {
    memset( qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info, 0, sz );
    for ( android_request_id = QMI_RIL_ZERO; android_request_id <= QMI_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID; android_request_id++ )
    {
      android_request_caps = qmi_ril_fw_android_request_get_handling_capabilities( android_request_id );
      caps_nof_slots = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_DECOMPOSE_QUEUE_SZ( android_request_caps );

      rec_overview = &qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info[ android_request_id ];
      rec_overview->holders.extra_holders = holders;

      if ( android_request_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE )
      {
        sz = sizeof(qmi_ril_fw_android_request_holder_type) * caps_nof_slots;
        holders = qcril_malloc( sz );
        if ( NULL != holders )
        {
          memset( holders, 0, sz );
          rec_overview->holders.extra_holders = holders;
          holder_iter = holders;
          for ( iter = QMI_RIL_ZERO; iter < caps_nof_slots; iter++ )
          {
            holder_iter->param_info_and_len = QMI_RIL_FW_ANDROID_REQUEST_INFO_COMPOSE( QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE, QMI_RIL_ZERO );
            holder_iter++;
          }
        }
      }
      else
      {
        rec_overview->holders.local_holder.param_info_and_len = QMI_RIL_FW_ANDROID_REQUEST_INFO_COMPOSE( QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE, QMI_RIL_ZERO );
      }

      rec_overview->nof_extra_holders_and_caps_and_dynamics = android_request_caps;
      rec_overview->original_android_request_id             = qcril_android_request_get_internal_event(android_request_id);
    }

    // family ring init
    if ( !is_retore_mode_pref_support )
    {
      qmi_ril_fw_android_request_flow_control_declare_family_ring( family_ring_nw_sel );
      qmi_ril_fw_android_request_flow_control_declare_family_ring( family_ring_mode_pref );
    }
    else
    {
      qmi_ril_fw_android_request_flow_control_declare_family_ring( family_ring_nw_sel_and_mode_pref);
    }
    qmi_ril_fw_android_request_flow_control_declare_family_ring( family_ring_dtmf );
    qmi_ril_fw_android_request_flow_control_declare_family_ring( family_ring_incall_ss );
    qmi_ril_fw_android_request_flow_control_declare_family_ring( family_ring_common_ss );
    qmi_ril_fw_android_request_flow_control_declare_family_ring( family_ring_send_sms_and_more );
    qmi_ril_fw_android_request_flow_control_declare_family_ring( family_ring_ps_attach_detach );

    qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info[ RIL_REQUEST_DTMF_START ].family_pair_android_request_id = RIL_REQUEST_DTMF_STOP;
    qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info[ RIL_REQUEST_DTMF_STOP ].family_pair_android_request_id = RIL_REQUEST_DTMF_START;

    qmi_ril_fw_android_request_flow_control_overview.common_moniker_book = QMI_RIL_ZERO;
    qmi_ril_fw_android_request_flow_control_overview.common_moniker_book++;
  }

  QCRIL_LOG_FUNC_RETURN();
}
//===========================================================================
// qmi_ril_fw_android_request_flow_control_find_busy_kind
//===========================================================================
qmi_ril_fw_android_request_kind_execution_overview_type* qmi_ril_fw_android_request_flow_control_find_busy_kind( qmi_ril_fw_android_request_kind_execution_overview_type* origin )
{
  qmi_ril_fw_android_request_kind_execution_overview_type* res = NULL;
  qmi_ril_fw_android_request_kind_execution_overview_type* iter;

  QCRIL_LOG_FUNC_ENTRY();

  if ( NULL != origin )
  {
    if ( !( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING )  )
    { // single node
      if ( QMI_RIL_ZERO != origin->token_under_execution )
      {
        res = origin;
      }
    }
    else
    { // ring
      iter = origin;
      do
      {
        QCRIL_LOG_INFO(".. token ptr 0x%x", iter->token_under_execution );
        if ( QMI_RIL_ZERO != iter->token_under_execution )
        {
          res = iter;
        }
        else
        {
          iter->nof_extra_holders_and_caps_and_dynamics |= QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK;
          iter = iter->family_ring;
        }
      } while ( NULL == res && !(QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK & iter->nof_extra_holders_and_caps_and_dynamics) );

      iter = origin;
      while ( QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK & iter->nof_extra_holders_and_caps_and_dynamics )
      {
        iter->nof_extra_holders_and_caps_and_dynamics &= ~QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK;
        iter = iter->family_ring;
      }
    }
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET((intptr_t)res);

  return res;
} // qmi_ril_fw_android_request_flow_control_find_busy_kind
//===========================================================================
// qmi_ril_fw_android_request_flow_control_find_outstanding_request_within_kind
//===========================================================================
qmi_ril_fw_android_request_holder_type * qmi_ril_fw_android_request_flow_control_find_outstanding_request_within_kind( qmi_ril_fw_android_request_kind_execution_overview_type* origin )
{
  qmi_ril_fw_android_request_holder_type *  res = NULL;
  uint32_t                                  idx;
  qmi_ril_fw_android_request_holder_type *  iter;
  uint32_t                                  nof_cap;

  if ( NULL != origin && QMI_RIL_ZERO != origin->chief_moniker )
  {
    if ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_SINGLE_ONLY )
    { // single
      if ( !(origin->holders.local_holder.param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE) )
      {
        res = &origin->holders.local_holder;
      }
    }
    else if ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE )
    {
      iter = origin->holders.extra_holders;
      nof_cap = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_DECOMPOSE_QUEUE_SZ ( origin->nof_extra_holders_and_caps_and_dynamics );
      for ( idx = 0; idx < nof_cap && NULL == res; idx ++ )
      {
        if ( !( iter->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE ) && ( iter->moniker == origin->chief_moniker ) )
        {
          res = iter;
        }
        else
        {
          iter++;
        }
      }
    }
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET((intptr_t)res);

  return res;
}
//===========================================================================
// qmi_ril_fw_android_request_flow_control_find_request_for_execution
//===========================================================================
int qmi_ril_fw_android_request_flow_control_find_request_for_execution( qmi_ril_fw_android_request_kind_execution_overview_type* origin,
                                                                        qmi_ril_fw_android_request_kind_execution_overview_type** exec_overview,
                                                                        qmi_ril_fw_android_request_holder_type ** exec_req_holder )
{
  int res = FALSE;
  qmi_ril_fw_android_request_kind_execution_overview_type*  res_exec_overview = NULL;
  qmi_ril_fw_android_request_holder_type *                  res_exec_req_holder = NULL;
  qmi_ril_fw_android_request_holder_type *                  holder_candidate = NULL;
  qmi_ril_fw_android_request_kind_execution_overview_type*  iter = NULL;
  uint32_t                                                  chief_of_chiefs_moniker;
  int                                                       moniker_cmp_res;
  qmi_ril_fw_android_request_kind_execution_overview_type*  pair = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  if ( NULL != origin )
  {
    chief_of_chiefs_moniker = QMI_RIL_ZERO;

    if ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING )
    { // ring case

      if ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FAMILY_RING_DEFINED_PAIR &&
           QMI_RIL_ZERO != origin->family_pair_android_request_id  )
      {
        pair = &qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info[ origin->family_pair_android_request_id ];
        holder_candidate = qmi_ril_fw_android_request_flow_control_find_outstanding_request_within_kind( pair );

        if ( NULL != holder_candidate )
        {
          res_exec_req_holder     = holder_candidate;
          res_exec_overview       = pair;
          res                     = TRUE;
        }
      }

      // look up through whole family ring
      if ( !res )
      {
        iter = origin;
        do
        {
          holder_candidate = qmi_ril_fw_android_request_flow_control_find_outstanding_request_within_kind( iter );

          if ( NULL != holder_candidate )
          {
            if ( QMI_RIL_ZERO == chief_of_chiefs_moniker )
            {
              chief_of_chiefs_moniker = iter->chief_moniker;
              res_exec_req_holder     = holder_candidate;
              res_exec_overview       = iter;
              res                     = TRUE;
            }
            else
            { // non empty chief_of_chiefs_moniker, do election
              moniker_cmp_res = qmi_ril_fw_android_request_flow_control_moniker_compare( iter->chief_moniker, chief_of_chiefs_moniker );
              if ( QMI_RIL_FW_FLOW_CONTROL_CMP_MONIKERS_OLDER == moniker_cmp_res )
              {
                chief_of_chiefs_moniker = iter->chief_moniker;
                res_exec_req_holder     = holder_candidate;
                res_exec_overview       = iter;
                res                     = TRUE;
              }
            }
          }

          iter->nof_extra_holders_and_caps_and_dynamics |= QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK;
          iter = iter->family_ring;
        } while ( !( QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK & iter->nof_extra_holders_and_caps_and_dynamics ) );

        iter = origin;
        while ( QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK & iter->nof_extra_holders_and_caps_and_dynamics )
        {
          iter->nof_extra_holders_and_caps_and_dynamics &= ~QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK;
          iter = iter->family_ring;
        }
      }

    }
    else
    { // one node only
      res_exec_req_holder = qmi_ril_fw_android_request_flow_control_find_outstanding_request_within_kind( origin );
      if ( NULL != res_exec_req_holder )
      {
        res_exec_overview = origin;
        res               = TRUE;
      }
    }
  }

  if ( NULL != exec_overview )
  {
    *exec_overview = res_exec_overview;
  }
  if ( NULL != exec_req_holder )
  {
    *exec_req_holder = res_exec_req_holder;
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET((int)res);

  return res;
} // qmi_ril_fw_android_request_flow_control_find_request_for_execution

//===========================================================================
// qmi_ril_fw_android_request_flow_control_release_holder_info_bookref
//===========================================================================
void qmi_ril_fw_android_request_flow_control_release_holder_info_bookref( qmi_ril_fw_android_request_holder_type* request_holder_org, qcril_evt_e_type android_request_id )
{
  qmi_ril_fw_android_param_copy_approach_type
        destroy_prm_used_approach;
  void* destroy_prm_four_byte_storage;
  void* destroy_prm_sub_created_custom_storage;

  QCRIL_LOG_FUNC_ENTRY();

  if ( request_holder_org && !( request_holder_org->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE ) )
  {
    if ( request_holder_org->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_NO_PAYLOAD )
    {
      destroy_prm_used_approach = QMI_RIL_ANDROID_PARAM_CPY_APPRON_EMPTY_NO_ACTION;
    }
    if ( request_holder_org->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_PTR )
    {
      destroy_prm_used_approach = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
    }
    else if ( request_holder_org->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_INT )
    {
      destroy_prm_used_approach = QMI_RIL_ANDROID_PARAM_CPY_APPRON_4_BYTES_SNAPSHOT;
    }
    else
    {
      destroy_prm_used_approach = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
    }

    if ( QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID != destroy_prm_used_approach )
    {
      destroy_prm_four_byte_storage           = (void*)&request_holder_org->param_payload_holder.param_int;
      destroy_prm_sub_created_custom_storage  = request_holder_org->param_payload_holder.param_ptr;

      qmi_ril_fw_destroy_android_live_params_copy( destroy_prm_used_approach,
                                                   (android_request_id),
                                                   destroy_prm_four_byte_storage,
                                                   destroy_prm_sub_created_custom_storage );
    }

    request_holder_org->token = QMI_RIL_ZERO;
    request_holder_org->param_payload_holder.param_ptr = NULL;
    request_holder_org->param_payload_holder.param_int = QMI_RIL_ZERO;
    request_holder_org->param_info_and_len = QMI_RIL_FW_ANDROID_REQUEST_INFO_COMPOSE( QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE, QMI_RIL_ZERO );
  }

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_fw_android_request_flow_control_release_holder_info_bookref
//===========================================================================
// qmi_ril_fw_android_request_flow_control_abandon_requests_local_only
//===========================================================================
void qmi_ril_fw_android_request_flow_control_abandon_requests_local_only( qmi_ril_fw_android_request_kind_execution_overview_type* origin, RIL_Errno cause, int is_unbind_cleanup )
{
  uint32_t                                          nof_cap;
  uint32_t                                          idx;
  qmi_ril_fw_android_request_holder_type*           iter;
  RIL_Token                                         cur_token = (RIL_Token)QCRIL_TOKEN_ID_INTERNAL;
  qcril_evt_e_type                                  android_request_id = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  if ( origin )
  {
    android_request_id = origin->original_android_request_id;
    if ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_SINGLE_ONLY )
    {
      /* Clean up during unbind:
         Clean all requests with cap SINGLE_ONLY marked as UNBIND_DROP_OFF.*/
      if ( (!is_unbind_cleanup && ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF) ) ||
           ( is_unbind_cleanup && ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF )))
      {
        if ( !(origin->holders.local_holder.param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE) )
        {
          cur_token = origin->holders.local_holder.token;
          int32_t cur_token_id = qcril_log_get_token_id( cur_token );
          int token_id_under_execution = qcril_log_get_token_id(origin->token_under_execution);

          qmi_ril_fw_android_request_flow_control_release_holder_info_bookref( &origin->holders.local_holder, android_request_id );

          if ( ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF ) ||
               ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF ) )
          {
            ( void ) qcril_reqlist_free( QCRIL_DEFAULT_INSTANCE_ID , cur_token );
          }
          QCRIL_LOG_DEBUG("abandon  token id %" PRId32 ", a-r-id %d", cur_token_id, android_request_id );
          qcril_response_api[ QCRIL_DEFAULT_INSTANCE_ID ]->OnRequestComplete( cur_token, // instance id is of relevance in QMI RIL while responding
                                                              cause,
                                                              NULL,
                                                              QMI_RIL_ZERO );

          QCRIL_LOG_INFO("token_under_execution %" PRId32 ", cur_token %" PRId32 "",  token_id_under_execution, cur_token_id);
          if ( QMI_RIL_ZERO != origin->token_under_execution && (origin->token_under_execution == cur_token))
          {
            origin->token_under_execution = QMI_RIL_ZERO;
          }
        }
      }
    }
    else if ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE )
    {
      /* Clean up during unbind:
         Clean all requests with cap ALLOW_MULTIPLE marked as UNBIND_DROP_OFF
         exception being RADIO_POWER.
      */
      if ( (!is_unbind_cleanup && ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF) ) ||
           ( is_unbind_cleanup && ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF )))
      {
        nof_cap = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_DECOMPOSE_QUEUE_SZ ( origin->nof_extra_holders_and_caps_and_dynamics );
        iter = origin->holders.extra_holders;
        for ( idx = QMI_RIL_ZERO; idx < nof_cap; idx++ )
        {
          if ( !(iter->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE) )
          {
            cur_token = iter->token;
            int cur_token_id = qcril_log_get_token_id( cur_token );
            int token_id_under_execution = qcril_log_get_token_id(origin->token_under_execution);

            qmi_ril_fw_android_request_flow_control_release_holder_info_bookref( iter, android_request_id );

            if( ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_DROP_OFF ) ||
                ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF ) )
            {
              ( void ) qcril_reqlist_free( QCRIL_DEFAULT_INSTANCE_ID , cur_token );
            }
            QCRIL_LOG_DEBUG("abandon  token id %" PRId32 ", a-r-id %d", cur_token_id, android_request_id );
            qcril_response_api[ QCRIL_DEFAULT_INSTANCE_ID ]->OnRequestComplete( cur_token, // instance id is of relevance in QMI RIL while responding
                                                                cause,
                                                                NULL,
                                                                QMI_RIL_ZERO );

            QCRIL_LOG_INFO("token_under_execution %" PRId32 ", cur_token %" PRId32 "",  token_id_under_execution, cur_token_id);
            if ( QMI_RIL_ZERO != origin->token_under_execution && (origin->token_under_execution == cur_token))
            {
                origin->token_under_execution = QMI_RIL_ZERO;
            }
          }

          iter++;
        }
      }
    }

    qmi_ril_fw_android_request_flow_control_overview_request_review_holders( origin );
  }

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_fw_android_request_flow_control_abandon_requests_local_only
//===========================================================================
// qmi_ril_fw_android_request_flow_control_abandon_requests_local_and_ring_zoned
//===========================================================================
void qmi_ril_fw_android_request_flow_control_abandon_requests_local_and_ring_zoned( qmi_ril_fw_android_request_kind_execution_overview_type* origin, RIL_Errno cause, int is_unbind_cleanup )
{
  qmi_ril_fw_android_request_kind_execution_overview_type* iter;
  QCRIL_LOG_FUNC_ENTRY();

  if ( origin )
  {
    if ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_IN_FAMILY_RING )
    {
      iter = origin;

      do
      {
        qmi_ril_fw_android_request_flow_control_abandon_requests_local_only( iter, cause, is_unbind_cleanup );

        iter->nof_extra_holders_and_caps_and_dynamics |= QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK;
        iter = iter->family_ring;

      } while ( !(QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK & iter->nof_extra_holders_and_caps_and_dynamics) );

      iter = origin;
      while ( QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK & iter->nof_extra_holders_and_caps_and_dynamics )
      {
        iter->nof_extra_holders_and_caps_and_dynamics &= ~QMI_RIL_FW_ANDROID_REQUEST_KIND_EXEC_OVERVIEW_DYNAMICS_RING_LOOP_MARK;
        iter = iter->family_ring;
      }

    }
    else
    {
      qmi_ril_fw_android_request_flow_control_abandon_requests_local_only( origin, cause, is_unbind_cleanup );
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_fw_android_request_flow_control_abandon_requests_local_and_ring_zoned
//===========================================================================
// qmi_ril_fw_android_request_flow_control_abandon_requests_family_main_thrd
//===========================================================================
void qmi_ril_fw_android_request_flow_control_abandon_requests_family_main_thrd( int android_request_id, RIL_Errno cause )
{
  qmi_ril_fw_android_request_kind_execution_overview_type* overview = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  qmi_ril_fw_android_request_flow_control_info_lock();

  do
  {
    if ( android_request_id <= QMI_RIL_ZERO || android_request_id > QMI_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID )
      break;

    overview = &qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info[ android_request_id ];

    if ( overview->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FLOW_CONTROL_EXEMPT )
      break;

    qmi_ril_fw_android_request_flow_control_abandon_requests_local_and_ring_zoned( overview, cause, FALSE );

  } while (FALSE);

  qmi_ril_fw_android_request_flow_control_info_unlock();

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_fw_android_request_flow_control_abandon_requests_family_main_thrd

//===========================================================================
// qmi_ril_abandon_all_oem_hook_requests
//===========================================================================
void qmi_ril_abandon_all_oem_hook_requests( RIL_Errno cause )
{
  qcril_reqlist_public_type       request_info;
  qcril_request_resp_params_type  resp;

  QCRIL_LOG_FUNC_ENTRY();

  while ( E_SUCCESS == qcril_reqlist_get_entry_from_group( QCRIL_DEFAULT_INSTANCE_ID,
                                                           &QCRIL_GRP_OEM_HOOK,
                                                           &request_info) )
  {
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                        request_info.t,
                                        request_info.request,
                                        cause,
                                        &resp );
    qcril_send_request_response( &resp );
  }

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qmi_ril_abandon_all_ims_requests
//===========================================================================
void qmi_ril_abandon_all_ims_requests( RIL_Errno cause )
{
  qcril_reqlist_public_type         request_info;

  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_ims_flow_control_event_queue(QCRIL_QMI_IMS_FLOW_CONTROL_CLEAR_LIST,
                                       QCRIL_DATA_NOT_ON_STACK, QCRIL_EVT_NONE,
                                       NULL, 0, 0);
  while ( E_SUCCESS == qcril_reqlist_get_entry_from_group( QCRIL_DEFAULT_INSTANCE_ID,
                                                           &QCRIL_GRP_IMS,
                                                           &request_info) )
  {
    imsRadioSendMessage(request_info.t, ims_MsgType_RESPONSE,
            qcril_qmi_ims_map_event_to_request(request_info.request),
            qcril_qmi_ims_map_ril_error_to_ims_error(cause), NULL, 0);
  }

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qmi_ril_fw_android_request_flow_control_abandon_all_requests_main_thrd
//===========================================================================
void qmi_ril_fw_android_request_flow_control_abandon_all_requests_main_thrd( RIL_Errno cause, int is_unbind_cleanup )
{
  int android_request_id;
  qmi_ril_fw_android_request_kind_execution_overview_type* overview = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  qmi_ril_fw_android_request_flow_control_info_lock();
  for ( android_request_id = QMI_RIL_ZERO;  android_request_id <= QMI_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID; android_request_id++ )
  {
    overview = &qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info[ android_request_id ];

    if ( !(overview->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FLOW_CONTROL_EXEMPT) )
    {
      qmi_ril_fw_android_request_flow_control_abandon_requests_local_and_ring_zoned( overview, cause, is_unbind_cleanup );
    }
  }
  qmi_ril_fw_android_request_flow_control_info_unlock();

  qmi_ril_abandon_all_ims_requests( cause );

  qmi_ril_abandon_all_oem_hook_requests( RIL_E_RADIO_NOT_AVAILABLE );

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_fw_android_request_flow_control_abandon_all_requests_main_thrd
//===========================================================================
// qmi_ril_fw_android_request_flow_control_request_holders_have_same_param
//===========================================================================
int qmi_ril_fw_android_request_flow_control_request_holders_have_same_param( qmi_ril_fw_android_request_holder_type* origin, qmi_ril_fw_android_request_holder_type* peer )
{
  int res = FALSE;
  uint32_t payload_sz_iter_org;
  uint32_t payload_sz_iter_peer;

  QCRIL_LOG_FUNC_ENTRY();

  if ( NULL != origin && NULL != peer )
  {
    if ( origin->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_NO_PAYLOAD && peer->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_NO_PAYLOAD )
    {
      res = TRUE;
    }
    else if ( origin->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_INT && peer->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_INT )
    {
      if ( origin->param_payload_holder.param_int == peer->param_payload_holder.param_int )
      {
        res = TRUE;
      }
    }
    else if ( origin->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_PTR && peer->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_PAYLOAD_PTR )
    {
      payload_sz_iter_org = QMI_RIL_FW_ANDROID_REQUEST_INFO_DECOMPOSE_SZ( origin->param_info_and_len );
      payload_sz_iter_peer = QMI_RIL_FW_ANDROID_REQUEST_INFO_DECOMPOSE_SZ( peer->param_info_and_len );
      if ( payload_sz_iter_org == payload_sz_iter_peer &&
           0 == memcmp( origin->param_payload_holder.param_ptr, peer->param_payload_holder.param_ptr, payload_sz_iter_org ) )
      {
        res = TRUE;
      }
    }
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET((int)res);
  return res;
} // qmi_ril_fw_android_request_flow_control_request_holders_have_same_param
//===========================================================================
// qmi_ril_fw_android_request_flow_control_declare_family_ring
//===========================================================================
void qmi_ril_fw_android_request_flow_control_declare_family_ring( const int * android_request_ids )
{
  const int * android_request_ids_iter;
  int cur;
  qmi_ril_fw_android_request_kind_execution_overview_type* overview_cur;
  qmi_ril_fw_android_request_kind_execution_overview_type* overview_prev;
  qmi_ril_fw_android_request_kind_execution_overview_type* overview_first;

  if ( NULL != android_request_ids )
  {
    overview_prev = NULL;
    overview_first = NULL;
    overview_cur = NULL;
    android_request_ids_iter = android_request_ids;

    while ( QMI_RIL_ZERO != ( cur = *android_request_ids_iter ) )
    {
      overview_cur = &qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info[ cur ];

      if ( NULL != overview_prev )
      {
        overview_prev->family_ring = overview_cur;
      }
      else
      {
        overview_first = overview_cur;
      }
      overview_prev = overview_cur;

      android_request_ids_iter++;
    }
    if ( NULL != overview_cur )
    { // non empty
      overview_cur->family_ring = overview_first;
    }

  }
} // qmi_ril_fw_android_request_flow_control_declare_family_ring
//===========================================================================
// qmi_ril_fw_android_request_flow_control_overview_request_review_holders
//===========================================================================
void qmi_ril_fw_android_request_flow_control_overview_request_review_holders( qmi_ril_fw_android_request_kind_execution_overview_type* origin )
{
  uint32_t                                chief_moniker_candidate;
  uint32_t                                idx;
  uint32_t                                nof_cap;
  qmi_ril_fw_android_request_holder_type* iter;
  int                                     cmp_res;

  QCRIL_LOG_FUNC_ENTRY();

  if ( NULL != origin )
  {
    if ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_SINGLE_ONLY )
    {
      if ( !(origin->holders.local_holder.param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE) )
      {
        origin->chief_moniker = origin->holders.local_holder.moniker;
      }
      else
      {
        origin->chief_moniker = QMI_RIL_ZERO;
      }
    }
    else if ( origin->nof_extra_holders_and_caps_and_dynamics & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE )
    {
      chief_moniker_candidate = QMI_RIL_ZERO;
      iter                    = origin->holders.extra_holders;

      nof_cap = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_DECOMPOSE_QUEUE_SZ( origin->nof_extra_holders_and_caps_and_dynamics );
      for ( idx = QMI_RIL_ZERO; idx < nof_cap; idx++ )
      {
        if ( !(iter->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE) )
        {
          if ( QMI_RIL_ZERO != chief_moniker_candidate )
          {
            cmp_res = qmi_ril_fw_android_request_flow_control_moniker_compare( iter->moniker, chief_moniker_candidate );
            if ( QMI_RIL_FW_FLOW_CONTROL_CMP_MONIKERS_OLDER == cmp_res )
            {
              chief_moniker_candidate = iter->moniker;
            }
          }
          else
          {
            chief_moniker_candidate = iter->moniker;
          }
        }
        iter++;
      }

      origin->chief_moniker = chief_moniker_candidate;
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_fw_android_request_flow_control_overview_request_review_holders
//===========================================================================
// qmi_ril_fw_android_request_flow_control_moniker_compare
//===========================================================================
int qmi_ril_fw_android_request_flow_control_moniker_compare( uint32_t moniker1, uint32_t moniker2 )
{
  int res;

  if ( moniker1 == moniker2 )
  {
    res = QMI_RIL_FW_FLOW_CONTROL_CMP_MONIKERS_EQUAL;
  }
  else
  { // non equal
    if ( moniker1 < moniker2 )
    {
      res = QMI_RIL_FW_FLOW_CONTROL_CMP_MONIKERS_OLDER;
    }
    else
    { // moniker2 is arithmetically lesser
      if ( moniker1 > 0xEFFFFFFF )
      {
        res = QMI_RIL_FW_FLOW_CONTROL_CMP_MONIKERS_OLDER;
      }
      else
      {
        res = QMI_RIL_FW_FLOW_CONTROL_CMP_MONIKERS_NEWER;
      }
    }
  }
  return res;
} // qmi_ril_fw_android_request_flow_control_moniker_compare
//===========================================================================
// qmi_ril_fw_android_request_flow_control_drop_legacy_book_records
//===========================================================================
void qmi_ril_fw_android_request_flow_control_drop_legacy_book_records( int voice_calls_related_only, int is_unbind_cleanup )
{
    (void)voice_calls_related_only;
    (void) is_unbind_cleanup;
#if 1 //EVENT REFACTOR - FLOW CONTROL
  int                       android_request_id;
  uint32_t                  android_request_caps;
  qcril_reqlist_public_type request_info;
  IxErrnoType               err_code;
  uint32_t                  cmp_mask;

  if ( voice_calls_related_only )
  {
    cmp_mask = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_LEGACY_VOICE_CALL_SPECIFIC_DROP_OFF;
  }
  else
  {
    if (is_unbind_cleanup == TRUE)
    {
      cmp_mask = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_UNBIND_DROP_OFF;
    }
    else
    {
      cmp_mask = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE;
    }
  }

  for ( android_request_id = QMI_RIL_ZERO; android_request_id <= QMI_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID; android_request_id++ )
  {
    android_request_caps = qmi_ril_fw_android_request_get_handling_capabilities( android_request_id );
    if ( cmp_mask & android_request_caps )
    {
      err_code = qcril_reqlist_query_by_request( QCRIL_DEFAULT_INSTANCE_ID, qcril_android_request_get_internal_event(android_request_id), &request_info );
      if ( E_SUCCESS == err_code )
      {
        qcril_reqlist_free( QCRIL_DEFAULT_INSTANCE_ID, request_info.t );
      }
    }
  }
#endif
} // qmi_ril_fw_android_request_flow_control_drop_legacy_book_records
//===========================================================================
// qmi_ril_fw_destroy_android_live_params_copy
//===========================================================================
void qmi_ril_fw_destroy_android_live_params_copy(qmi_ril_fw_android_param_copy_approach_type used_approach,
                                               qcril_evt_e_type event_id,
                                               void* four_byte_storage,
                                               void* sub_created_custom_storage)
{
  RIL_IMS_SMS_Message*  copied_android_send_ims_msg_params;
  RIL_CDMA_SMS_Message* copied_android_cdma_send_ims_param;
  char *copied_android_gw_smsc_address = NULL;
  char *copied_android_gw_pdu = NULL;
  char ** copied_android_gw_sms_ims_params;

  char** copied_cdma_dtmf_holder;
  char* copied_cdma_dtmf_str;
  char* copied_cdma_dtmf_on;
  char* copied_cdma_dtmf_off;

  RIL_SMS_WriteArgs*  copied_android_write_sms_to_sim_msg_params;
  char *copied_android_write_sms_to_sim_msg_smsc_address = NULL;
  char *copied_android_write_sms_to_sim_msg_pdu = NULL;

#if (QCRIL_RIL_VERSION < 15)
  RIL_InitialAttachApn* copied_android_initial_attach_apn_params;
#else
  RIL_InitialAttachApn_v15* copied_android_initial_attach_apn_params;
  char* copied_android_initial_attach_apn_roaming_protocol;
  char* copied_android_initial_attach_apn_mvno_type;
  char* copied_android_initial_attach_apn_mvno_match_data;
#endif
  char* copied_android_initial_attach_apn_apn;
  char* copied_android_initial_attach_apn_protocol;
  char* copied_android_initial_attach_apn_username;
  char* copied_android_initial_attach_apn_password;

  char *copied_android_manual_selection_mcc_mnc;
  char *copied_android_manual_selection_rat;
  char ** copied_android_manual_selection_params;

  RIL_CallForwardInfo* copied_android_query_call_fwd_status_params;
  char *copied_android_query_call_fwd_status_number = NULL;

  char** copied_change_barring_pwd_params;
  char *copied_ch_bar_pwd_faclity = NULL;
  char *copied_ch_bar_pwd_old_pwd = NULL;
  char *copied_ch_bar_pwd_new_pwd = NULL;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_INFO("action to destroy cloned Android request parameters, a-r-id %s, appron %d", qcril_log_lookup_event_name(event_id), (int)used_approach );

  int android_request_id = qcril_event_get_android_request(event_id);
  switch( android_request_id )
  {
    case RIL_REQUEST_IMS_SEND_SMS:
      if ( NULL != sub_created_custom_storage && QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY == used_approach )
      {
        copied_android_send_ims_msg_params = (RIL_IMS_SMS_Message*)sub_created_custom_storage;
        if ( RADIO_TECH_3GPP2 == copied_android_send_ims_msg_params->tech )
        {
          copied_android_cdma_send_ims_param = copied_android_send_ims_msg_params->message.cdmaMessage;
          if ( NULL != copied_android_cdma_send_ims_param )
          {
            qcril_free( copied_android_cdma_send_ims_param );
            copied_android_cdma_send_ims_param = NULL;
          }
        }
        else
        {
          copied_android_gw_sms_ims_params = copied_android_send_ims_msg_params->message.gsmMessage;

          if ( NULL != copied_android_gw_sms_ims_params )
          {
            copied_android_gw_smsc_address = copied_android_gw_sms_ims_params[0];
            copied_android_gw_pdu          = copied_android_gw_sms_ims_params[1];

            if ( NULL != copied_android_gw_smsc_address )
            {
              qcril_free( copied_android_gw_smsc_address );
              copied_android_gw_smsc_address = NULL;
            }
            if ( NULL != copied_android_gw_pdu )
            {
              qcril_free( copied_android_gw_pdu );
              copied_android_gw_pdu = NULL;
            }

            qcril_free( copied_android_gw_sms_ims_params );
            copied_android_gw_sms_ims_params = NULL;
          }


        }

        qcril_free( copied_android_send_ims_msg_params );
        copied_android_send_ims_msg_params = NULL;
      }
      break;

    case RIL_REQUEST_SEND_SMS:              // fallthrough
    case RIL_REQUEST_SEND_SMS_EXPECT_MORE:
      if ( NULL != sub_created_custom_storage && QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY == used_approach )
      {
        copied_android_gw_sms_ims_params = (char**)sub_created_custom_storage;
        copied_android_gw_smsc_address   = copied_android_gw_sms_ims_params[0];
        copied_android_gw_pdu            = copied_android_gw_sms_ims_params[1];

        QCRIL_LOG_DEBUG("sms allo 0x%x, 0x%x, 0x%x", copied_android_gw_sms_ims_params, copied_android_gw_smsc_address, copied_android_gw_pdu);

        if( NULL != copied_android_gw_pdu )
        {
            qcril_free( copied_android_gw_pdu );
            copied_android_gw_pdu = NULL;
        }

        if( NULL != copied_android_gw_smsc_address )
        {
            qcril_free( copied_android_gw_smsc_address );
            copied_android_gw_smsc_address = NULL;
        }

        qcril_free( copied_android_gw_sms_ims_params );
        copied_android_gw_sms_ims_params = NULL;
      }
      break;

    case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL:
      /* only legacy format with RAT needs */
      if ( qmi_ril_is_feature_supported(QMI_RIL_FEATURE_LEGACY_RAT) )
      {
        if ( NULL != sub_created_custom_storage && QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY == used_approach )
        {
          copied_android_manual_selection_params    = (char**)sub_created_custom_storage;
          copied_android_manual_selection_mcc_mnc   = copied_android_manual_selection_params[0];
          copied_android_manual_selection_rat       = copied_android_manual_selection_params[1];

          QCRIL_LOG_DEBUG("manual sel allo 0x%x, 0x%x, 0x%x", copied_android_manual_selection_params, copied_android_manual_selection_mcc_mnc, copied_android_manual_selection_rat);

          if( NULL != copied_android_manual_selection_mcc_mnc )
          {
              qcril_free( copied_android_manual_selection_mcc_mnc );
              copied_android_manual_selection_mcc_mnc = NULL;
          }

          if( NULL != copied_android_manual_selection_rat )
          {
              qcril_free( copied_android_manual_selection_rat );
              copied_android_manual_selection_rat = NULL;
          }

          qcril_free( copied_android_manual_selection_params );
          copied_android_manual_selection_params = NULL;
        }
      }
      else
      {
        if ( NULL != sub_created_custom_storage && QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY == used_approach )
        {
          qcril_free( sub_created_custom_storage );
          sub_created_custom_storage = NULL;
        }
      }
      break;

    case RIL_REQUEST_CDMA_BURST_DTMF:
      if ( NULL != sub_created_custom_storage && QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY == used_approach )
      {
        copied_cdma_dtmf_holder = (char**)sub_created_custom_storage;

        copied_cdma_dtmf_str = copied_cdma_dtmf_holder[0];
        copied_cdma_dtmf_on  = copied_cdma_dtmf_holder[1];
        copied_cdma_dtmf_off = copied_cdma_dtmf_holder[2];

        if( NULL != copied_cdma_dtmf_off )
        {
            qcril_free( copied_cdma_dtmf_off );
            copied_cdma_dtmf_off = NULL;
        }

        if( NULL != copied_cdma_dtmf_on )
        {
            qcril_free( copied_cdma_dtmf_on );
            copied_cdma_dtmf_on = NULL;
        }

        if( NULL != copied_cdma_dtmf_str )
        {
            qcril_free( copied_cdma_dtmf_str );
            copied_cdma_dtmf_str = NULL;
        }

        qcril_free( copied_cdma_dtmf_holder );
        copied_cdma_dtmf_holder = NULL;
      }
      break;

    case RIL_REQUEST_SET_INITIAL_ATTACH_APN:
      if ( NULL != sub_created_custom_storage && QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY == used_approach )
      {
#if (QCRIL_RIL_VERSION < 15)
        copied_android_initial_attach_apn_params = (RIL_InitialAttachApn*)sub_created_custom_storage;
#else
        copied_android_initial_attach_apn_params = (RIL_InitialAttachApn_v15*)sub_created_custom_storage;
#endif

        copied_android_initial_attach_apn_apn = copied_android_initial_attach_apn_params->apn;
        copied_android_initial_attach_apn_protocol = copied_android_initial_attach_apn_params->protocol;
        copied_android_initial_attach_apn_username = copied_android_initial_attach_apn_params->username;
        copied_android_initial_attach_apn_password = copied_android_initial_attach_apn_params->password;
#if (QCRIL_RIL_VERSION >= 15)
        copied_android_initial_attach_apn_roaming_protocol = copied_android_initial_attach_apn_params->roamingProtocol;
        copied_android_initial_attach_apn_mvno_type = copied_android_initial_attach_apn_params->mvnoType;
        copied_android_initial_attach_apn_mvno_match_data = copied_android_initial_attach_apn_params->mvnoMatchData;
#endif

        if ( NULL != copied_android_initial_attach_apn_apn )
        {
          qcril_free( copied_android_initial_attach_apn_apn );
          copied_android_initial_attach_apn_apn = NULL;
        }

        if ( NULL != copied_android_initial_attach_apn_protocol )
        {
          qcril_free( copied_android_initial_attach_apn_protocol );
          copied_android_initial_attach_apn_protocol = NULL;
        }

        if ( NULL != copied_android_initial_attach_apn_username )
        {
          qcril_free( copied_android_initial_attach_apn_username );
          copied_android_initial_attach_apn_username = NULL;
        }


        if ( NULL != copied_android_initial_attach_apn_password )
        {
          qcril_free( copied_android_initial_attach_apn_password );
          copied_android_initial_attach_apn_password = NULL;
        }

#if (QCRIL_RIL_VERSION >= 15)
        if ( NULL != copied_android_initial_attach_apn_roaming_protocol )
        {
          qcril_free( copied_android_initial_attach_apn_roaming_protocol );
          copied_android_initial_attach_apn_roaming_protocol = NULL;
        }

        if ( NULL != copied_android_initial_attach_apn_mvno_type )
        {
          qcril_free( copied_android_initial_attach_apn_mvno_type );
          copied_android_initial_attach_apn_mvno_type = NULL;
        }

        if ( NULL != copied_android_initial_attach_apn_mvno_match_data )
        {
          qcril_free( copied_android_initial_attach_apn_mvno_match_data );
          copied_android_initial_attach_apn_mvno_match_data = NULL;
        }
#endif

        qcril_free( copied_android_initial_attach_apn_params );
        copied_android_initial_attach_apn_params = NULL;
      }
      break;

    case RIL_REQUEST_WRITE_SMS_TO_SIM:
      if ( NULL != sub_created_custom_storage && QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY == used_approach )
      {
        copied_android_write_sms_to_sim_msg_params = (RIL_SMS_WriteArgs*)sub_created_custom_storage;

        copied_android_write_sms_to_sim_msg_smsc_address = copied_android_write_sms_to_sim_msg_params->smsc;
        copied_android_write_sms_to_sim_msg_pdu = copied_android_write_sms_to_sim_msg_params->pdu;

        if ( NULL != copied_android_write_sms_to_sim_msg_smsc_address )
        {
          qcril_free( copied_android_write_sms_to_sim_msg_smsc_address );
          copied_android_write_sms_to_sim_msg_smsc_address = NULL;
        }

        if ( NULL != copied_android_write_sms_to_sim_msg_pdu )
        {
          qcril_free( copied_android_write_sms_to_sim_msg_pdu );
          copied_android_write_sms_to_sim_msg_pdu = NULL;
        }

        qcril_free( copied_android_write_sms_to_sim_msg_params );
        copied_android_write_sms_to_sim_msg_params = NULL;
      }
      break;

    case RIL_REQUEST_SET_CALL_FORWARD:            // fallthrough
    case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS:
      if ( NULL != sub_created_custom_storage && QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY == used_approach )
      {
        copied_android_query_call_fwd_status_params = (RIL_CallForwardInfo*)sub_created_custom_storage;

        if ( NULL != copied_android_query_call_fwd_status_params )
        {
          copied_android_query_call_fwd_status_number = copied_android_query_call_fwd_status_params->number;

          if ( NULL != copied_android_query_call_fwd_status_number )
          {
            qcril_free( copied_android_query_call_fwd_status_number );
            copied_android_query_call_fwd_status_number = NULL;
          }
          qcril_free( copied_android_query_call_fwd_status_params );
          copied_android_query_call_fwd_status_params = NULL;
        }
      }
      break;

    case RIL_REQUEST_CHANGE_BARRING_PASSWORD:
      if ( NULL != sub_created_custom_storage && QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY == used_approach )
      {
        copied_change_barring_pwd_params = (char**)sub_created_custom_storage;

        if ( NULL != copied_change_barring_pwd_params )
        {
          copied_ch_bar_pwd_faclity = copied_change_barring_pwd_params[0];
          copied_ch_bar_pwd_old_pwd = copied_change_barring_pwd_params[1];
          copied_ch_bar_pwd_new_pwd = copied_change_barring_pwd_params[2];

          if ( NULL != copied_ch_bar_pwd_faclity )
          {
            qcril_free( copied_ch_bar_pwd_faclity );
            copied_ch_bar_pwd_faclity = NULL;
          }

          if ( NULL != copied_ch_bar_pwd_old_pwd )
          {
            qcril_free( copied_ch_bar_pwd_old_pwd );
            copied_ch_bar_pwd_old_pwd = NULL;
          }

          if ( NULL != copied_ch_bar_pwd_new_pwd )
          {
            qcril_free( copied_ch_bar_pwd_new_pwd );
            copied_ch_bar_pwd_new_pwd = NULL;
          }

          qcril_free( copied_change_barring_pwd_params );
          copied_change_barring_pwd_params = NULL;
        }
      }
      break;

    default:
      switch ( used_approach )
      {
        case QMI_RIL_ANDROID_PARAM_CPY_APPRON_4_BYTES_SNAPSHOT:
          if ( NULL != four_byte_storage )
          {
            memset( four_byte_storage, 0, 4 );
          }
          break;

        case QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY:
          if ( NULL != sub_created_custom_storage )
          {
            qcril_free( sub_created_custom_storage );
            sub_created_custom_storage = NULL;
          }
          break;

        case QMI_RIL_ANDROID_PARAM_CPY_APPRON_EMPTY_NO_ACTION: // no action
          break;

        default: // no action
          break;
      }
      break;
  }

  QCRIL_LOG_FUNC_RETURN();

} // qmi_ril_fw_destroy_android_live_params_copy

//===========================================================================
// qcril_qmi_mgr_voice_technology_updated
//===========================================================================
void qcril_qmi_mgr_voice_technology_updated(qcril_radio_tech_e_type new_voice_technology)
{
  qcril_arb_state_info_struct_type *s_ptr;

  QCRIL_LOG_FUNC_ENTRY();

  qmi_ril_enter_critical_section();
  s_ptr = &qcril_state->info[ QCRIL_DEFAULT_INSTANCE_ID ];
  s_ptr->voice_radio_tech = new_voice_technology;

  s_ptr = &qcril_state->info[ QCRIL_SECOND_INSTANCE_ID ];
  s_ptr->voice_radio_tech = new_voice_technology;

  // TODO_TSTS: This code seems not required. This info is used no where.
  s_ptr = &qcril_state->info[ QCRIL_THIRD_INSTANCE_ID ];
  s_ptr->voice_radio_tech = new_voice_technology;
  qmi_ril_leave_critical_section();

  QCRIL_LOG_FUNC_RETURN_WITH_RET(new_voice_technology);
} // qcril_qmi_mgr_modem_state_updated

//===========================================================================
// qmi_ril_android_pending_unsol_resp_lock
//===========================================================================
void qmi_ril_android_pending_unsol_resp_lock( void )
{
  pthread_mutex_lock( &qmi_ril_android_pending_unsol_resp.pending_unsol_resp_mutex );
} // qmi_ril_android_pending_unsol_resp_lock

//===========================================================================
// qmi_ril_android_pending_unsol_resp_unlock
//===========================================================================
void qmi_ril_android_pending_unsol_resp_unlock( void )
{
  pthread_mutex_unlock( &qmi_ril_android_pending_unsol_resp.pending_unsol_resp_mutex );
} // qmi_ril_android_pending_unsol_resp_unlock

//===========================================================================
// qmi_ril_init_android_unsol_resp_pending_list
//===========================================================================
void qmi_ril_init_android_unsol_resp_pending_list()
{
  QCRIL_LOG_FUNC_ENTRY();

  qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list
    = (qmi_ril_android_pending_unsol_resp_type *)
            qcril_malloc(sizeof(qmi_ril_android_pending_unsol_resp_type) *
                   (QMI_RIL_ANDROID_UNSOL_RESP_MAX_ID - RIL_UNSOL_RESPONSE_BASE + 1) );
  if ( NULL == qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list )
  {
    QCRIL_LOG_ERROR("malloc failed!");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_init_android_unsol_resp_pending_list

//===========================================================================
// qmi_ril_reset_android_unsol_resp_dispatchable_table
//===========================================================================
void qmi_ril_reset_android_unsol_resp_dispatchable_table()
{
  QCRIL_LOG_FUNC_ENTRY();

  qmi_ril_android_pending_unsol_resp_lock();
  int i;
  for ( i=0; i<QMI_RIL_ANDROID_UNSOL_RESP_MAX_ID - RIL_UNSOL_RESPONSE_BASE + 1; i++ )
  {
    qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list[i].num_of_locker = 0;
  }
  qmi_ril_android_pending_unsol_resp_unlock();

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_reset_android_unsol_resp_dispatchable_table

//===========================================================================
// qmi_ril_free_pending_unsol_resp
//===========================================================================
void qmi_ril_free_pending_unsol_resp(qmi_ril_android_pending_unsol_resp_type* resp, int resp_id)
{
  QCRIL_LOG_FUNC_ENTRY();

  if ( resp->valid )
  {
    resp->valid = FALSE;
    if ( resp->logstr )
    {
      qcril_free((void*) resp->logstr);
    }
    qmi_ril_fw_destroy_android_live_params_copy(resp->param_copy_arrron, qcril_android_request_get_internal_event(resp_id), resp->copied_params, resp->copied_params);
  }

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_free_pending_unsol_resp

//===========================================================================
// qmi_ril_suppress_android_unsol_resp
//===========================================================================
void qmi_ril_suppress_android_unsol_resp(int resp_id)
{
  QCRIL_LOG_FUNC_ENTRY();
  if ( resp_id < RIL_UNSOL_RESPONSE_BASE || resp_id > QMI_RIL_ANDROID_UNSOL_RESP_MAX_ID)
  {
    QCRIL_LOG_ERROR("invalid android_unsol_resp id %d", resp_id);
  }
  else
  {
    qmi_ril_android_pending_unsol_resp_lock();
    QCRIL_LOG_INFO("suppressing android_unsol_resp %d", resp_id);
    qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list[resp_id-RIL_UNSOL_RESPONSE_BASE].num_of_locker++;
    qmi_ril_android_pending_unsol_resp_unlock();
  }
  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_suppress_android_unsol_resp

//===========================================================================
// qmi_ril_unsuppress_android_unsol_resp
//===========================================================================
void qmi_ril_unsuppress_android_unsol_resp(int resp_id)
{
  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if ( resp_id < RIL_UNSOL_RESPONSE_BASE || resp_id > QMI_RIL_ANDROID_UNSOL_RESP_MAX_ID)
    {
      QCRIL_LOG_ERROR("invalid android_unsol_resp id %d", resp_id);
      break;
    }

    qmi_ril_android_pending_unsol_resp_lock();
    QCRIL_LOG_INFO("unsuppressing android_unsol_resp %d", resp_id);

    if ( qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list[resp_id-RIL_UNSOL_RESPONSE_BASE].num_of_locker > 0 )
    {
      QCRIL_LOG_INFO("unsol_resp %d previous locker number %d", resp_id, qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list[resp_id-RIL_UNSOL_RESPONSE_BASE].num_of_locker);
      qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list[resp_id-RIL_UNSOL_RESPONSE_BASE].num_of_locker--;
      if ( 0 == qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list[resp_id-RIL_UNSOL_RESPONSE_BASE].num_of_locker )
      {
        qmi_ril_android_pending_unsol_resp_type* pending_unsol_resp = qmi_ril_get_unsol_resp_from_pending_list(resp_id);
        if ( NULL != pending_unsol_resp )
        {
          qcril_unsol_resp_params_type resp_param;

          qcril_default_unsol_resp_params( pending_unsol_resp->instance_id,
                                           resp_id,
                                           &resp_param );

          resp_param.logstr = pending_unsol_resp->logstr;
          resp_param.resp_pkt = pending_unsol_resp->copied_params;
          resp_param.resp_len = pending_unsol_resp->copied_param_len;
          qcril_send_unsol_response_epilog(&resp_param);

          qmi_ril_free_pending_unsol_resp(pending_unsol_resp, resp_id);
        }
      }
    }
    else
    {
      QCRIL_LOG_DEBUG("Didn't supress android_unsol_resp %d", resp_id);
    }

    qmi_ril_android_pending_unsol_resp_unlock();
  } while ( FALSE );

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_unsuppress_android_unsol_resp

//===========================================================================
// qmi_ril_reset_unsol_resp_pending_list
//===========================================================================
void qmi_ril_reset_unsol_resp_pending_list()
{
  int i;

  QCRIL_LOG_FUNC_ENTRY();

  qmi_ril_android_pending_unsol_resp_lock();

  for ( i = RIL_UNSOL_RESPONSE_BASE; i <= QMI_RIL_ANDROID_UNSOL_RESP_MAX_ID; i++)
  {
    qmi_ril_free_pending_unsol_resp(&qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list[i-RIL_UNSOL_RESPONSE_BASE], i);
  }

  qmi_ril_android_pending_unsol_resp_unlock();
} // qmi_ril_reset_unsol_resp_pending_list

//===========================================================================
// qmi_ril_add_unsol_resp_to_pending_list
//===========================================================================
void qmi_ril_add_unsol_resp_to_pending_list(qcril_unsol_resp_params_type *param_ptr)
{
  QCRIL_LOG_FUNC_ENTRY();

  qmi_ril_android_pending_unsol_resp_type *resp = NULL;
  int resp_id = param_ptr->response_id;

  do
  {
    if ( resp_id < RIL_UNSOL_RESPONSE_BASE || resp_id > QMI_RIL_ANDROID_UNSOL_RESP_MAX_ID)
    {
      QCRIL_LOG_ERROR("invalid android_unsol_resp id %d", resp_id);
      break;
    }

    resp = &qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list[resp_id-RIL_UNSOL_RESPONSE_BASE];

    qmi_ril_free_pending_unsol_resp(resp, resp_id);

    // copy the unsol resp
    resp->valid = TRUE;
    resp->instance_id = param_ptr->instance_id;
    resp->copied_param_len = param_ptr->resp_len;

    if ( NULL != param_ptr->logstr )
    {
      size_t len = strlen(param_ptr->logstr);
      resp->logstr = qcril_malloc(len+1);
      if ( NULL != resp->logstr )
      {
        memcpy((void*) resp->logstr, (void*) param_ptr->logstr, len);
      }
      else
      {
        QCRIL_LOG_ERROR("malloc failed");
        resp->valid = FALSE;
        break;
      }
    }
    else
    {
      resp->logstr = NULL;
    }

    resp->param_copy_arrron = qmi_ril_fw_create_android_live_params_copy( resp_id,
                                                                          param_ptr->resp_pkt,
                                                                          param_ptr->resp_len,
                                                                          resp->copied_params,
                                                                          NULL,
                                                                          &resp->copied_params,
                                                                          NULL,
                                                                          NULL );

    if ( QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID == resp->param_copy_arrron )
    {
      resp->valid = FALSE;
      if ( NULL != resp->logstr )
      {
        qcril_free((void*) resp->logstr);
      }
    }

  } while ( FALSE );
  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_add_unsol_resp_to_pending_list

//===========================================================================
// qmi_ril_get_unsol_resp_from_pending_list
//===========================================================================
qmi_ril_android_pending_unsol_resp_type* qmi_ril_get_unsol_resp_from_pending_list(int resp_id)
{
  QCRIL_LOG_FUNC_ENTRY();

  qmi_ril_android_pending_unsol_resp_type *resp = NULL;

  do
  {
    if ( resp_id < RIL_UNSOL_RESPONSE_BASE || resp_id > QMI_RIL_ANDROID_UNSOL_RESP_MAX_ID)
    {
      QCRIL_LOG_ERROR("invalid android_unsol_resp id %d", resp_id);
      break;
    }

    resp = &qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list[resp_id-RIL_UNSOL_RESPONSE_BASE];
    if ( !resp->valid )
    {
      resp = NULL;
    }

  } while ( FALSE );

  QCRIL_LOG_FUNC_RETURN();
  return resp;
} // qmi_ril_get_unsol_resp_from_pending_list

//===========================================================================
// qmi_ril_check_android_unsol_resp_dispatchable
//===========================================================================
boolean qmi_ril_check_android_unsol_resp_dispatchable(int resp_id)
{
  boolean ret = FALSE;
  if ( resp_id >= RIL_UNSOL_RESPONSE_BASE && resp_id <= QMI_RIL_ANDROID_UNSOL_RESP_MAX_ID )
  {
    ret = qmi_ril_android_pending_unsol_resp.pending_unsol_resp_list[resp_id-RIL_UNSOL_RESPONSE_BASE].num_of_locker ? FALSE : TRUE;
  }
  else
  {
    QCRIL_LOG_ERROR("invalid android_unsol_resp id %d", resp_id);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET((int) ret);
  return ret;
} // qmi_ril_check_android_unsol_resp_dispatchable

//============================================================================
// FUNCTION: qmi_ril_retrieve_number_of_rilds
//
// DESCRIPTION:
// Returns the number of rilds supported on a target that supports mutiple rild scenario
//
// RETURN: number of rilds supported on a target that supports mutiple rild scenario
//============================================================================
int qmi_ril_retrieve_number_of_rilds()
{
    int num_of_rilds = 1;
    if ( qmi_ril_is_feature_supported(QMI_RIL_FEATURE_DSDS) )
    {
        num_of_rilds = 2;
    }
    else if( qmi_ril_is_feature_supported(QMI_RIL_FEATURE_TSTS) )
    {
        num_of_rilds = 3;
    }

    return num_of_rilds;
} //qmi_ril_retrieve_number_of_rilds
//===========================================================================
//qmi_ril_bootup_actition_on_rild_atel_link_connect
//===========================================================================
void qmi_ril_bootup_actition_on_rild_atel_link_connect(void * params)
{
  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED( params );
  // SMS REFACTOR: This should not be required. OEM HOOK RAW is being deprecated
  qcril_sms_post_ready_status_update();

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_bootup_actition_on_rild_atel_link_connect

/*===========================================================================

  FUNCTION  qcril_request_suppress_list_init

===========================================================================*/
/*!
    @brief
    Initialize suppress list and mutex.

    @return
    E_SUCCESS if success
*/
/*=========================================================================*/
void qcril_request_suppress_list_init
(
    void
)
{
    pthread_mutexattr_init(&qcril_request_supress_list_mutex_attr);
    pthread_mutex_init(&qcril_request_supress_list_mutex,
                       &qcril_request_supress_list_mutex_attr);
}

/*===========================================================================

  FUNCTION  qcril_request_check_if_suppressed

===========================================================================*/
/*!
    @brief
    Check if a request id is in suppressed list.

    @return
    TRUE is request is suppressed.
*/
/*=========================================================================*/
boolean qcril_request_check_if_suppressed
(
    uint32_t event_id,
    qcril_req_handler_type **event_handler
)
{
    boolean ret = FALSE;
    int     i;

    QCRIL_MUTEX_LOCK(&qcril_request_supress_list_mutex, "supress list mutex");

    do
    {
        for (i = 0; ((i < QCRIL_REQUEST_SUPPRESS_MAX_LEN) &&
                    (qcril_request_suppress_list[i].event_id != 0)); i++)
        {
            if (event_id == qcril_request_suppress_list[i].event_id)
            {
                if (event_handler)
                {
                    *event_handler = qcril_request_suppress_list[i].event_handler;
                }

                ret = TRUE;
                break;
            }
        }
    } while (FALSE);

    QCRIL_MUTEX_UNLOCK(&qcril_request_supress_list_mutex, "supress list mutex");
    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

/*===========================================================================

  FUNCTION  qcril_request_suppress_request

===========================================================================*/
/*!
    @brief
    Add request id to suppressed list.

    @return
    E_SUCCESS if success.
    E_FAILURE if failure.
*/
/*=========================================================================*/
int qcril_request_suppress_request
(
    uint32_t                     event_id,
    qcril_req_handler_type *event_handler
)
{
    boolean ret = E_FAILURE;
    int i;

    QCRIL_MUTEX_LOCK(&qcril_request_supress_list_mutex, "supress list mutex");

    do
    {
        for (i = 0; i < QCRIL_REQUEST_SUPPRESS_MAX_LEN; i++)
        {
            if (0 == qcril_request_suppress_list[i].event_id)
            {
                QCRIL_LOG_DEBUG("Supress %d", event_id);
                qcril_request_suppress_list[i].event_id      = event_id;
                qcril_request_suppress_list[i].event_handler = event_handler;
                ret = E_SUCCESS;
                break;
            }
        }

    } while (FALSE);

    QCRIL_MUTEX_UNLOCK(&qcril_request_supress_list_mutex, "supress list mutex");
    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

/*===========================================================================

  FUNCTION  qcril_request_unsuppress_request

===========================================================================*/
/*!
    @brief
    Remove request id from suppressed list.

    @return
    E_SUCCESS if success.
    E_FAILURE if failure.
*/
/*=========================================================================*/
int qcril_request_unsuppress_request
(
    uint32_t                     event_id,
    qcril_req_handler_type *event_handler
)
{
    boolean ret = E_FAILURE;
    int i;

    QCRIL_MUTEX_LOCK(&qcril_request_supress_list_mutex, "supress list mutex");

    do
    {
        for (i = 0; i < QCRIL_REQUEST_SUPPRESS_MAX_LEN; i++)
        {
            if ((event_id == qcril_request_suppress_list[i].event_id) &&
                 (event_handler == qcril_request_suppress_list[i].event_handler))
            {
                QCRIL_LOG_DEBUG("Unsupress %d", event_id);
                qcril_request_suppress_list[i].event_id      = 0;
                qcril_request_suppress_list[i].event_handler = NULL;
                ret = E_SUCCESS;
                break;
            }
        }

    } while (FALSE);

    QCRIL_MUTEX_UNLOCK(&qcril_request_supress_list_mutex, "supress list mutex");
    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

/*===========================================================================

  FUNCTION qcril_request_clean_up_suppress_list

===========================================================================*/
/*!
    @brief
    Clean up suppress list.

    @return
    None
*/
/*=========================================================================*/
void qcril_request_clean_up_suppress_list
(
    void
)
{
    int i;
    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_MUTEX_LOCK(&qcril_request_supress_list_mutex, "supress list mutex");
    for (i = 0; i < QCRIL_REQUEST_SUPPRESS_MAX_LEN; i++)
    {
        qcril_request_suppress_list[i].event_id      = 0;
        qcril_request_suppress_list[i].event_handler = NULL;
    }

    QCRIL_MUTEX_UNLOCK(&qcril_request_supress_list_mutex, "supress list mutex");
    QCRIL_LOG_FUNC_RETURN();
}

void qcril_unsol_oem_socket_connected()
{
    QCRIL_LOG_INFO( "Report unsol oem socket connected");
    qcril_hook_unsol_response( QCRIL_DEFAULT_INSTANCE_ID,
                               QCRIL_EVT_HOOK_UNSOL_OEM_SOCKET_CONNECTED,
                               NULL,
                               0);
}

#ifdef QMI_RIL_UTF
//============================================================================
// FUNCTION: qmi_ril_thread_shutdown
//
// DESCRIPTION:
// clears all global variables and releases all shared resources for reboot
//
// RETURN:
//============================================================================
int qmi_ril_threads_shutdown()
{
  //if (core_shutdown_for_reboot() != 0)
  //{
  //  QCRIL_LOG_ERROR("Could not successfully shutdown thread in core_handler.c");
  //}

  return 0;
}

//============================================================================
// FUNCTION: qmi_ril_reboot_cleanup
//
// DESCRIPTION:
// clears all global variables and releases all shared resources for reboot
//
// RETURN:
//============================================================================

int qmi_ril_reboot_cleanup()
{
  // Begin shutdown process
  qmi_ril_fw_android_request_flow_control_drop_legacy_book_records( FALSE, FALSE );
  qmi_ril_fw_android_request_flow_control_abandon_all_requests_main_thrd( RIL_E_CANCELLED, FALSE );
  qmi_ril_clear_timed_callback_list();
  // clean up core clients
  qcril_qmi_client_release();

#ifdef QMI_RIL_UTF
  qcril_qmi_hal_ims_module_cleanup();
  qcril_qmi_hal_pdc_module_cleanup();
  qcril_qmi_hal_nas_module_cleanup();
  qcril_qmi_hal_pbm_module_cleanup();
  qcril_qmi_hal_sms_module_cleanup();
  qcril_qmi_hal_uim_module_cleanup();
  qcril_qmi_hal_voice_module_cleanup();
#endif

  qmi_ril_reset_multi_sim_ftr_info();

  if (qcril_db_reset_cleanup() != 0)
  {
    QCRIL_LOG_ERROR("Could not successfully reset resources in qcril_db.c");
  }
  if (qcril_qmi_voice_reboot_cleanup() != 0)
  {
    QCRIL_LOG_ERROR("Could not successfully reset resources in qcril_qmi_voice.c");
  }
  qcril_qmi_nas_reboot_cleanup();
  qcril_qmi_hw_mbn_reboot_cleanup();
  qcril_qmi_sw_mbn_reboot_cleanup();
  // local file cleanup
  //qcril_free(qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info);

  return 0;
}

#endif

boolean qcril_is_heap_memory_tracked()
{
    return is_heap_memory_tracked;
}

