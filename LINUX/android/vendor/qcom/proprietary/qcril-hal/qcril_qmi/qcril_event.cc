/******************************************************************************
#  Copyright (c) 2008-2010, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/******************************************************************************
  @file    qcril_event.c
  @brief   qcril qmi - event queue

  DESCRIPTION
    This module queues QCRIL events generated as a result of RPC callbacks from
    AMSS (ARM9 processor) and processes them in a separate thread, thus allowing
    the RPC threads in the kernel to return before QCRIL processing is done.

******************************************************************************/



/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C" {

#include <stdlib.h>
#include <inttypes.h>

#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#ifdef QMI_RIL_UTF
#include <ril_utf_wake_lock_redef.h>
#else
#include <hardware_legacy/power.h>
#endif

#include "IxErrno.h"
#include "qcrili.h"
#include "qcril_qmi_client.h"
#include "qcril_pbm.h"
#include "qcril_qmi_lte_direct_disc.h"
#include "qcril_qmi_voice.h"
#include "qcril_qmi_pdc.h"
#include "qcril_qmi_radio_config_dispatch_helper.h"
#include "qcril_mbn_hw_update.h"
}

#include "modules/sms/qcril_qmi_sms.h"

#include <memory>
#include <vector>
#include <utility>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <framework/Dispatcher.h>
#include <framework/Message.h>
#include <framework/legacy.h>
#include <framework/qcril_log.h>
#include <framework/qcril_event.h>
#include <framework/Module.h>
#include <framework/SolicitedMessage.h>
#include <framework/add_message_id.h>
#include <modules/uim/UimOemHook.h>
#include <translators/android/utils.h>
#include "qcril_mbn_hw_update.h"
#include "qcril_mbn_sw_update.h"

#include "qcril_qmi_oem_events.h"
#include "qcril_qmi_nas.h"
#include "qcril_qmi_nas2.h"
#include "qcril_qmi_prov.h"
#include "modules/ims/qcril_qmi_ims_misc.h"
#include "modules/ims/qcril_qmi_imss_v02.h"
#include "modules/ims/qcril_qmi_imsa.h"
#include "modules/ims/qcril_qmi_imss.h"
#include "modules/ims/qcril_qmi_ims_radio_agent.h"
#include "modules/sms/qcril_qmi_sms.h"
#include "modules/uim/qcril_uim_gba.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define QCRIL_WAKE_LOCK_NAME "qcril"

QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_ANDROID_RIL_REQ);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_DEFAULT);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_MBN_HW);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_MBN_SW);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_NAS);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_NONE);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_OEM);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_OEM_HOOK);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_PBM);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_SMS);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_IMS);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_UIM);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_UIM_GSDI);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_UIM_MMGSDI);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_RC_UIM);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_VOICE);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_DATA);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_PROV);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_CM);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_CELL_INFO);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_EMBMS);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_LTED);
QCRIL_DEFINE_LEGACY_EVENT_GROUP(QCRIL_GRP_ANDROID_RIL_UNSOL);

#undef QCRIL_LEGACY_EVENT
#define QCRIL_LEGACY_EVENT QCRIL_DEFINE_LEGACY_EVENT

#undef QCRIL_LEGACY_INDICATION
#define QCRIL_LEGACY_INDICATION QCRIL_DEFINE_LEGACY_INDICATION

#include "qcril_eventlist.h"


typedef struct qcril_event_buf
{
  struct qcril_event_buf *next;
  struct qcril_event_buf *prev;
  qcril_instance_id_e_type instance_id;
  qcril_modem_id_e_type modem_id;
  qcril_evt_e_type event_id;
  void *data;
  size_t datalen;
  qcril_event_data_free_cb_t datafree;
  RIL_Token t;
  boolean data_must_be_freed;
} qcril_event_type;

static qcril_evt_e_type qmi_ril_fw_dedicated_thrd_exec_android_requests_set[] =
  {
    QCRIL_EVT_RIL_REQUEST_VOICE_RADIO_TECH,
    QCRIL_EVT_RIL_REQUEST_SIGNAL_STRENGTH,
    QCRIL_EVT_RIL_REQUEST_VOICE_REGISTRATION_STATE,
    QCRIL_EVT_RIL_REQUEST_DATA_REGISTRATION_STATE,
    QCRIL_EVT_RIL_REQUEST_IMS_REGISTRATION_STATE,
    QCRIL_EVT_RIL_REQUEST_OPERATOR,
    QCRIL_EVT_RIL_REQUEST_SET_LOCATION_UPDATES,
    QCRIL_EVT_RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE,
    QCRIL_EVT_RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC,
    QCRIL_EVT_RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL,
    QCRIL_EVT_RIL_REQUEST_SET_BAND_MODE,
    QCRIL_EVT_RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE,
    QCRIL_EVT_RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE,
    QCRIL_EVT_RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE,
    QCRIL_EVT_RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE,
    QCRIL_EVT_RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE,
    QCRIL_EVT_RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE,
    QCRIL_EVT_RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE,
    QCRIL_EVT_RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE,
    QCRIL_EVT_RIL_REQUEST_SET_UICC_SUBSCRIPTION,
    QCRIL_EVT_RIL_REQUEST_SET_RADIO_CAPABILITY,
    QCRIL_EVT_RIL_REQUEST_GET_RADIO_CAPABILITY,
    QCRIL_EVT_RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE,
    QCRIL_EVT_RIL_REQUEST_SET_TTY_MODE,
    QCRIL_EVT_RIL_REQUEST_QUERY_TTY_MODE,
    QCRIL_EVT_RIL_REQUEST_CDMA_SUBSCRIPTION,
    QCRIL_EVT_RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY,
    QCRIL_EVT_RIL_REQUEST_DEVICE_IDENTITY,
    QCRIL_EVT_RIL_REQUEST_BASEBAND_VERSION,
    QCRIL_EVT_RIL_REQUEST_GET_IMEI,
    QCRIL_EVT_RIL_REQUEST_GET_IMEISV,
    QCRIL_EVT_RIL_REQUEST_RADIO_POWER,
    QCRIL_EVT_RIL_REQUEST_SET_INITIAL_ATTACH_APN,
    QCRIL_EVT_RIL_REQUEST_WRITE_SMS_TO_SIM,
    QCRIL_EVT_RIL_REQUEST_GET_CELL_INFO_LIST,
    QCRIL_EVT_RIL_REQUEST_SET_LTE_BAND_PREF,
    QMI_RIL_ZERO
  };

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/


/*! static data for this module */
typedef struct
{
  pthread_t tid;
  pthread_mutex_t list_mutex;
  pthread_mutex_t startup_mutex;
  qcril_event_type list;
  fd_set readFds;
  int started;
  int fdWakeupRead;
  int fdWakeupWrite;

  pthread_mutex_t       activity_lock_mutex;
  pthread_mutexattr_t   activity_lock_mutex_atr;

} qcril_event_static_data_type;

static qcril_event_static_data_type qcril_event;

static pthread_cond_t qcril_event_startupCond = PTHREAD_COND_INITIALIZER;

#define QCRIL_EVT_NONE_ARGS NONE, NULL, &QCRIL_GRP_NONE, 0
QCRIL_EXPAND_LEGACY_EVENT_ARGS(QCRIL_DECLARE_LEGACY_EVENT,QCRIL_EVT_NONE_ARGS);

IxErrnoType qcril_execute_event(qcril_request_params_type *params_ptr,
                                int *is_dedicated_thread);


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/

class QcrilEventMessage: public SolicitedMessage<std::nullptr_t>, public add_message_id<QcrilEventMessage>
{
    private:
        qcril_event_type ev;

    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.legacy.event_dispatch";

        virtual ~QcrilEventMessage() {
            boolean to_check_ev_data_free = TRUE;

            if (ev.datafree)
            {
                to_check_ev_data_free = !ev.datafree(ev.event_id, ev.data, ev.datalen);
            }

            if ( to_check_ev_data_free && ev.data_must_be_freed && ev.data )
            {
              QCRIL_LOG_DEBUG("Freeing ev.data");
              qcril_free( ev.data );
            }

            ev.data = nullptr;
        }
        QcrilEventMessage(qcril_instance_id_e_type instance_id,
                qcril_modem_id_e_type modem_id,
                qcril_evt_e_type event_id,
                qcril_data_src_e_type data_src,
                void *data,
                size_t datalen,
                qcril_event_data_free_cb_t datafree,
                RIL_Token t):
            SolicitedMessage<std::nullptr_t>(get_class_message_id()),
            ev { NULL,
                NULL,
                instance_id,
                modem_id,
                event_id,
                data,
                datalen,
                datafree,
                t,
                data_src == QCRIL_DATA_ON_STACK }
        { 
            if (data == NULL) {
                datalen = 0;
            } else if ( data_src == QCRIL_DATA_ON_STACK ) {
                ev.data = qcril_malloc( datalen );
                if (!ev.data) {
                    QCRIL_LOG_ERROR("Failed to allocate memory");
                } else {
                    memcpy(ev.data, data, datalen);
                }
                QCRIL_ASSERT(ev.data != NULL);
            }
        }
        string dump() {
            return MESSAGE_NAME;
        }
        qcril_event_type &get_event() {
            return ev;
        }
};

class qcril_event_module: public Module
{
    public:
        virtual ~qcril_event_module() {}

        void handle_qcril_event(std::shared_ptr<QcrilEventMessage> msg) {
            IxErrnoType err_no = E_FAILURE;
            qcril_event_type *ev = &msg->get_event();

            //TODO REFACTOR: move this to the Dispatcher
            acquire_wake_lock( PARTIAL_WAKE_LOCK, QCRIL_WAKE_LOCK_NAME );

            int res = pthread_mutex_trylock( &qcril_event.activity_lock_mutex );

            if (res) {
                QCRIL_LOG_INFO("Proceeding with event handling even though activity lock is locked");
                // TODO REFACTOR: Maybe queue the events in the old queue when the mutex is locked
                // to still be able to suspend processing. When processing is resumed, the queue
                // will be processed, along with the new event
            }

            QCRIL_LOG_DEBUG( "RID %d MID %d De-queued event %s (%d)(obj 0x%p)",
                             ev->instance_id, ev->modem_id,
                             qcril_log_lookup_event_name(ev->event_id),
                             ev->event_id, ev );

            err_no = qcril_process_event( ev->instance_id, ev->modem_id, ev->event_id, ev->data, ev->datalen, ev->t );


            if (err_no != E_SUCCESS)
            {
              QCRIL_LOG_ERROR("Error processing event! (%d)", err_no);
            }

            if (!res)
            {
              pthread_mutex_unlock( &qcril_event.activity_lock_mutex );
            }

            release_wake_lock( QCRIL_WAKE_LOCK_NAME );
        }

        qcril_event_module() {
            mName = "qcril_event_module";
            mMessageHandler = {
                HANDLER(QcrilEventMessage, qcril_event_module::handle_qcril_event)
            };
        }

};

load_module<qcril_event_module> the_module;

/*===========================================================================

  FUNCTION:  qcril_event_queue_with_data_free

===========================================================================*/
/*!
    @brief
    Queues an event to be dispatched in the the event thread.
    This queue allows RPC callbacks from AMSS to return before the event
    is processed.

    @return
    E_SUCCESS If the queue operation was successful, approprate failure code otherwise.

*/
/*=========================================================================*/
IxErrnoType qcril_event_queue_with_data_free
(
  qcril_instance_id_e_type instance_id,
  qcril_modem_id_e_type modem_id,
  qcril_data_src_e_type data_src,
  qcril_evt_e_type event_id,
  void *data,
  size_t datalen,
  qcril_event_data_free_cb_t datafree,
  RIL_Token t
)
{
  qcril_event_type *ev;
  IxErrnoType result;

  /*-----------------------------------------------------------------------*/

  do
  {
    result = E_SUCCESS;
    std::shared_ptr<QcrilEventMessage> msg = std::make_shared<QcrilEventMessage>(
            instance_id,
            modem_id,
            event_id,
            data_src,
            data,
            datalen,
            datafree,
            t);
    if (!msg) {
      result = E_NO_MEMORY;
      QCRIL_LOG_ERROR("Failed to allocate memory, aborting!");
      QCRIL_ASSERT(0); // this is a noop in release build
      break;
    }
    ev = &msg->get_event();

    QCRIL_LOG_DEBUG( "RID %d MID %d Queued event %s (%d bytes)(obj 0x%" PRIxPTR ")",
                      instance_id, modem_id,
                      qcril_log_lookup_event_name(event_id),
                      datalen, (intptr_t)ev );
    msg->dispatch();
  } while(0);

  return result;
} /* qcril_event_queue_with_data_free */

boolean qcril_event_queue_default_data_free
(
 qcril_evt_e_type event_id,
 void *data,
 size_t datalen
)
{
    (void)event_id;
    (void)data;
    (void)datalen;

    return false;
}
/*===========================================================================

  FUNCTION:  qcril_event_queue

===========================================================================*/
/*!
    @brief
    Queues an event to be dispatched in the the event thread.
    This queue allows RPC callbacks from AMSS to return before the event
    is processed.

    @return
    E_SUCCESS If the queue operation was successful, approprate failure code otherwise.

*/
/*=========================================================================*/
IxErrnoType qcril_event_queue
(
  qcril_instance_id_e_type instance_id,
  qcril_modem_id_e_type modem_id,
  qcril_data_src_e_type data_src,
  qcril_evt_e_type event_id,
  void *data,
  size_t datalen,
  RIL_Token t
)
{
    return qcril_event_queue_with_data_free(
            instance_id,
            modem_id,
            data_src,
            event_id,
            data,
            datalen,
            qcril_event_queue_default_data_free,
            t);
}


/*===========================================================================

  FUNCTION:  qcril_event_init

===========================================================================*/
/*!
    @brief
    Creates a new thread for processing events received via RPC calls
    from AMSS.

    @return
    Void
*/
/*=========================================================================*/
void qcril_event_init( void )
{
  /*-----------------------------------------------------------------------*/

  pthread_mutexattr_init( &qcril_event.activity_lock_mutex_atr );
  pthread_mutex_init( &qcril_event.activity_lock_mutex,
                      &qcril_event.activity_lock_mutex_atr );


} /* qcril_event_init() */

/*===========================================================================

  FUNCTION:  qcril_event_start

===========================================================================*/
/*!
    @brief
    Starts the event loop. needs to wait until after RIL_Init is finished
    so that ril.cpp has some time to complete RIL_Register before we start
    calling onUnsolicitedResponse.

    @return
    Void
*/
/*=========================================================================*/
void qcril_event_start( void )
{
  QCRIL_MUTEX_LOCK( &qcril_event.startup_mutex, "[Main Thread] qcril_event.startup_mutex" );

  qcril_event.started = 2;

  pthread_cond_broadcast(&qcril_event_startupCond);

  QCRIL_MUTEX_UNLOCK( &qcril_event.startup_mutex, "[Main Thread] qcril_event.startup_mutex" );

} /* qcril_event_start() */
//===========================================================================
//qmi_ril_leave_critical_section
//===========================================================================
void qcril_event_suspend()
{
  pthread_mutex_lock( &qcril_event.activity_lock_mutex );
} // qcril_event_suspend
//===========================================================================
//qcril_event_resume
//===========================================================================
void qcril_event_resume()
{
  pthread_mutex_unlock( &qcril_event.activity_lock_mutex );
} // qcril_event_resume

/*===========================================================================

  FUNCTION:  qcril_dispatch_event

===========================================================================*/
/*!
    @brief
    Does processing common to onRequest and qcril_process_event with
    respect to calling the event handler and processing the return value
    from the event handler.

    @return
    E_SUCCESS of the event was handled normally
    E_NOT_ALLOWED if the event is not supported in the current state

*/
/*=========================================================================*/
IxErrnoType qcril_dispatch_event
(
  qcril_event_id_t *entry_ptr,
  qcril_request_params_type *params_ptr
)
{
  qcril_instance_id_e_type instance_id;
  qcril_modem_id_e_type modem_id;
  qcril_request_return_type ret;

  IxErrnoType res = E_NOT_ALLOWED;
  qmi_ril_gen_operational_status_type cur_status = qmi_ril_get_operational_status();

  if(params_ptr != NULL && (params_ptr->instance_id < QCRIL_MAX_INSTANCE_ID) )
  {
    // print the recieved date byte stream
    qcril_qmi_print_hex(params_ptr->data,  params_ptr->datalen);

    instance_id = params_ptr->instance_id;
    modem_id = params_ptr->modem_id;

    if ((TO_MASK(cur_status) & params_ptr->event_id->allowed_radio_states_mask))
    {
        res = E_SUCCESS;
    }
#if 0
    // TODO REARCH: Need to move all these to their corresponding modules.
    // TODO REARCH: Each module will have to provide a mask of states in which the
    // TODO REARCH: event can be processed
    switch ( cur_status )
    {
      case QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDING:
      case QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDED:
      case QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_PENDING:
      case QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_RETRY:
      case QMI_RIL_GEN_OPERATIONAL_STATUS_UNBIND:
        switch (params_ptr->event_id_android)
          {
              /*
          case QCRIL_EVT_INTERNAL_MMGSDI_MODEM_RESTART_START:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_PRE_SUSPEND_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_DATA_SUSPEND_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_UIM_SUSPEND_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_SUSPEND_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_PRE_SUSPEND_CON:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_DATA_SUSPEND_CON:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_UIM_SUSPEND_CON:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_SUSPEND_CON:
          if ( ( cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_PENDING ) ||
               ( cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_RETRY ) )
          {
            QCRIL_LOG_DEBUG("Dropping suspend event %d state %d", params_ptr->event_id, cur_status);
            res = E_NOT_ALLOWED;
          }
          else
          {
            res = E_SUCCESS;
          }
          break;

          case QCRIL_EVT_INTERNAL_MMGSDI_MODEM_RESTART_COMPLETE:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_PRE_RESUME_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_DATA_RESUME_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_UIM_RESUME_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_RESUME_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_PRE_RESUME_CON:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_DATA_RESUME_CON:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_UIM_RESUME_CON:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_RESUME_CON:
          if ( ( cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDING ) ||
               ( cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDED ) )
          {
            QCRIL_LOG_DEBUG("Dropping resume event %d state %d", params_ptr->event_id, cur_status);
            res = E_NOT_ALLOWED;
          }
          else
          {
            res = E_SUCCESS;
          }
          break;

          case QCRIL_EVT_DATA_EVENT_CALLBACK:
          case QCRIL_EVT_UIM_QMI_COMMAND_CALLBACK:
          case QCRIL_EVT_INTERNAL_UIM_VERIFY_PIN_COMMAND_CALLBACK:
          case QCRIL_EVT_INTERNAL_UIM_SAP_RESP:
          case QCRIL_EVT_UIM_RMT_QMI_COMMAND_CALLBACK:
          case QCRIL_EVT_UIM_RMT_QMI_INDICATION_CALLBACK:
          case QCRIL_EVT_UIM_HTTP_QMI_TRANSACTION_COMPLETED_CALLBACK:
          case QCRIL_EVT_UIM_HTTP_QMI_INDICATION_CALLBACK:
          case QCRIL_EVT_QMI_RIL_SEND_UNSOL_RADIO_STATE_CHANGED:
          case QCRIL_EVT_QMI_RIL_EMBMS_SEND_UNSOL_RADIO_STATE:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_CHECK_IF_SERVICE_UP:
          case QCRIL_EVT_QMI_RIL_SERVICE_DOWN:
          //case QCRIL_EVT_QMI_RIL_MODEM_RESTART_CHECK_IF_SSR_EXCL_SERVICE_UP:
          //case QCRIL_EVT_QMI_RIL_SSR_EXCL_SERVICE_DOWN:
          case QCRIL_EVT_GSTK_QMI_NOTIFY_CARD_ERROR:
            res = E_SUCCESS;
            break;

          case QCRIL_EVT_QMI_RIL_PDC_LOAD_CONFIGURATION:
          case QCRIL_EVT_QMI_RIL_PDC_SELECT_CONFIGURATION:
          case QCRIL_EVT_QMI_RIL_PDC_ACTIVATE_CONFIGURATION:
          case QCRIL_EVT_QMI_RIL_PDC_DELETE_CONFIGURATION:
          case QCRIL_EVT_QMI_RIL_PDC_LIST_CONFIGURATION:
          case QCRIL_EVT_QMI_RIL_PDC_DEACTIVATE_CONFIGURATION:
          case QCRIL_EVT_QMI_RIL_PDC_PARSE_DIFF_RESULT:
          case QCRIL_EVT_REQUEST_MBN_HW_INIT:
          case QCRIL_EVT_REQUEST_MBN_HW_DISABLED_INIT:
          case QCRIL_EVT_REQUEST_MBN_HW_GET_SELECTED_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_HW_DEACTIVATE_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_HW_DELETE_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_HW_LOAD_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_HW_SELECT_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_HW_ACTIVATE_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_SW_DISALBLE_MODEM_UPDATE:
          case QCRIL_EVT_REQUEST_MBN_SW_INIT:
          case QCRIL_EVT_REQUEST_MBN_SW_GET_SELECTED_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_SW_GET_CONFIG_INFO:
          case QCRIL_EVT_REQUEST_MBN_SW_DEACTIVATE_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_SW_DELETE_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_SW_LOAD_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_SW_SELECT_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_SW_ACTIVATE_CONFIG:
          case QCRIL_EVT_REQUEST_MBN_SW_CLEANUP_CONFIG:
            res = E_SUCCESS;
            break;
            */

          // allow tunelling specifically for RIL_REQUEST_SCREEN_STATE, RIL_REQUEST_SEND_DEVICE_STATE,
          // RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER and RIL_REQUEST_SHUTDOWN.
          // For RIL_REQUEST_SET_RADIO_CAPABILITY, handler will take care of returning error when
          // SSR is in progress.
          case RIL_REQUEST_SCREEN_STATE:
          case RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER:
          case RIL_REQUEST_SEND_DEVICE_STATE:
          case RIL_REQUEST_SHUTDOWN:
          case RIL_REQUEST_SET_RADIO_CAPABILITY:
            res = E_SUCCESS;
            break;

          case RIL_REQUEST_GET_SIM_STATUS:
            res = E_SUCCESS;
            break;

          case RIL_REQUEST_RADIO_POWER:
            if ( (qcril_qmi_nas_modem_power_is_mdm_shdn_in_apm() &&
                0 == qcril_qmi_modem_power_voting_state()) ||
                (cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_UNBIND) )
            {
                res = E_SUCCESS;
            }
            break;

            /*
          case QCRIL_EVT_HOOK_UPDATE_SUB_BINDING:
          case QCRIL_EVT_RIL_REQUEST_MANUAL_PROVISIONING:
          case QCRIL_EVT_INTERNAL_MMGSDI_ACTIVATE_SUBS:
          case QCRIL_EVT_INTERNAL_MMGSDI_DEACTIVATE_SUBS:
          case QCRIL_EVT_QMI_PROV_ACTIVATE_FOLLOW_UP:
          case QCRIL_EVT_QMI_PROV_DEACTIVATE_FOLLOW_UP:
          case QCRIL_EVT_CM_ACTIVATE_PROVISION_STATUS:
          case QCRIL_EVT_CM_DEACTIVATE_PROVISION_STATUS:
          case QCRIL_EVT_QMI_PROV_FM_START_STATUS:
          case QCRIL_EVT_QMI_REQUEST_POWER_RADIO:
          case QCRIL_EVT_INTERNAL_MMGSDI_CARD_POWER_DOWN:
          case QCRIL_EVT_INTERNAL_MMGSDI_CARD_POWER_UP:
          case QCRIL_EVT_CM_CARD_STATUS_UPDATED:
          case QCRIL_EVT_QMI_NAS_CARD_STATUS_UPDATE:
          case QCRIL_EVT_QMI_PROV_UPDATE_ICCID:
          case QCRIL_EVT_QMI_DMS_HANDLE_INDICATIONS:
#ifdef QMI_RIL_UTF
          case QCRIL_EVT_QMI_DSD_HANDLE_INDICATIONS:
#endif
          case QCRIL_EVT_QMI_VOICE_HANDLE_INDICATIONS:
          case QCRIL_EVT_QMI_NAS_HANDLE_INDICATIONS:
          case QCRIL_EVT_QMI_PDC_HANDLE_INDICATIONS:
          case QCRIL_EVT_QMI_NAS_HANDLE_ASYNC_CB:
          case QCRIL_EVT_QMI_DMS_HANDLE_ASYNC_CB:
          case QCRIL_EVT_QMI_VOICE_HANDLE_COMM_CALLBACKS:
          case QCRIL_EVT_QMI_IMSA_HANDLE_COMM_CALLBACKS:
          case QCRIL_EVT_QMI_IMSS_HANDLE_COMM_CALLBACKS:
          case QCRIL_EVT_QMI_RIL_POST_VOICE_RTE_CHANGE_IND:
          case QCRIL_EVT_UIM_QMI_INDICATION:
          case RIL_REQUEST_DIAL:
          case RIL_REQUEST_GET_RADIO_CAPABILITY:
          case RIL_REQUEST_SIM_IO:
          case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
            if ( cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_UNBIND )
            {
              QCRIL_LOG_DEBUG("Operational status %d, event id %d", qmi_ril_get_operational_status(), params_ptr->event_id);
              res = E_SUCCESS;
            }
            else
            {
              res = E_NOT_ALLOWED;
            }
            break;

            */
          default:
            res = E_NOT_ALLOWED;
            break;
          }
        break;


      case QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_ONGOING: // fallthrough
      case QMI_RIL_GEN_OPERATIONAL_STATUS_UNRESTRICTED: // fallthrough
      case QMI_RIL_GEN_OPERATIONAL_STATUS_RESUMING:
        /*
        switch (params_ptr->event_id)
        {
          case QCRIL_EVT_INTERNAL_MMGSDI_MODEM_RESTART_START:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_PRE_SUSPEND_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_DATA_SUSPEND_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_UIM_SUSPEND_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_SUSPEND_REQ:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_PRE_SUSPEND_CON:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_DATA_SUSPEND_CON:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_UIM_SUSPEND_CON:
          case QCRIL_EVT_QMI_RIL_MODEM_RESTART_RIL_CORE_FINAL_SUSPEND_CON:
          if ( cur_status == QMI_RIL_GEN_OPERATIONAL_STATUS_RESUMING )
          {
            QCRIL_LOG_DEBUG("Dropping suspend event %d state %d", params_ptr->event_id, cur_status);
            res = E_NOT_ALLOWED;
          }
          else
          {
            res = E_SUCCESS;
          }
          break;

          default:
          res = E_SUCCESS;
          break;
        }
        */
        break;

      default:
        res = E_NOT_ALLOWED;
        break;
    }
#endif
    if (E_SUCCESS == res)
      {

        qmi_ril_init_request_return(instance_id, &ret);
        /* Dispatch the request to the appropriate handler */
        if (entry_ptr->handler) {
        (entry_ptr->handler)(params_ptr, &ret);
        } else {
            QCRIL_LOG_DEBUG("No handler for %s", entry_ptr->name);
        }

        /* Handle state transition */
        if ( ret.pri_gw_sim_state_changed || ret.pri_cdma_sim_state_changed ||
             ret.sec_gw_sim_state_changed || ret.sec_cdma_sim_state_changed ||
             ret.ter_gw_sim_state_changed || ret.ter_cdma_sim_state_changed
           )
        {
          qcril_state_transition( instance_id, modem_id, params_ptr->event_id, &ret );
        }
      }
  }
  else
  {
    res = E_NOT_ALLOWED;
    QCRIL_LOG_FATAL("CHECK FAILED");
    if ( NULL != params_ptr )
    {
      QCRIL_LOG_FATAL(".. failure details instance id %d, modem id %d", (int)params_ptr->instance_id, (int)params_ptr->modem_id  );
    }
  }

  return res;

} // qcril_dispatch_event

static RIL_Errno ril_errno_from_ixerrno(IxErrnoType err_no)
{
  RIL_Errno ret;
  switch(err_no)
  {
    case E_SUCCESS:
      ret = RIL_E_SUCCESS;
      break;

    default:
      ret = RIL_E_GENERIC_FAILURE;
      break;
  }
  return ret;
}

/*===========================================================================

  FUNCTION:  qcril_process_event

===========================================================================*/
/*!
    @brief
    Dispatches all RIL events except RIL_REQUESTs, which are handled by
    onRequest.  This routine may be called from multiple threads but it
    blocks on a single semaphore that is shared with onRequest such that
    only one event is handled at a time by the RIL.

    @return
    E_SUCCESS of the event was handled normally
    E_NOT_SUPPORTED if the event_id was invalid
    E_NOT_ALLOWED if the event is not supported in the current state

*/
/*=========================================================================*/
IxErrnoType qcril_process_event
(
  qcril_instance_id_e_type instance_id,
  qcril_modem_id_e_type modem_id,
  qcril_evt_e_type event_id,
  void *data,
  size_t datalen,
  RIL_Token t
)
{
  IxErrnoType err_no;
  RIL_Errno e;
  qcril_request_params_type params;
  char event_name[ 100 ] = "";
  qcril_request_resp_params_type  resp;
  int is_dedicated_thread;

  /*-----------------------------------------------------------------------*/
  QCRIL_SNPRINTF( event_name, sizeof( event_name ), "%s(%p), RID %d, MID %d", qcril_log_lookup_event_name( event_id ),
                  event_id, instance_id, modem_id );

  if (!event_id)
  {
    return (E_NOT_SUPPORTED);
  }

  if (qcril_is_event_in_group(event_id, &QCRIL_GRP_UIM_MMGSDI) ||
      qcril_is_event_in_group(event_id, &QCRIL_GRP_PBM))
  {
    /* Internal events */
    QCRIL_LOG_CF_PKT_RIL_EVT( instance_id, event_name );
    QCRIL_LOG_DEBUG( "RIL --- %s ---> RIL", event_name );
  }
  else
  {
    /* AMSS events or callbacks */
    QCRIL_LOG_CF_PKT_MODEM_EVT( modem_id, event_name );

    QCRIL_LOG_DEBUG( "RIL <--- %s --- AMSS", event_name, event_id, instance_id, modem_id );
  }

  /* Initialize the input parameters for the request handler.  This allows for
     easy addition or deletion of parameters in the future. */
  params.instance_id = instance_id;
  params.modem_id = modem_id;
  params.event_id = event_id;
  params.data = data;
  params.datalen = datalen;
  params.t = t;

  err_no = qcril_execute_event( &params, &is_dedicated_thread );

  QCRIL_LOG_INFO( "Exit %s, err_no %d", qcril_log_lookup_event_name(event_id ), err_no );
  if ( E_SUCCESS != err_no )
  {
    if (qcril_is_event_in_group(params.event_id, &QCRIL_GRP_IMS))
    {
      imsRadioSendMessage(params.t, ims_MsgType_RESPONSE, qcril_qmi_ims_map_event_to_request(params.event_id),
            qcril_qmi_ims_map_ril_error_to_ims_error(ril_errno_from_ixerrno(err_no)), NULL, 0);
    }
    else if (qcril_is_event_in_group(params.event_id, &QCRIL_GRP_OEM_HOOK))
    {
      e = ril_errno_from_ixerrno(err_no);
      qcril_default_request_resp_params( instance_id, params.t, params.event_id, e, &resp );
      qcril_send_request_response( &resp );
    }
  }
  return err_no;

} /* qcril_process_event() */

IxErrnoType qcril_execute_event
(
  qcril_request_params_type *params_ptr,
  int                       *is_dedicated_thread
)
{
  /*For dedicated thread look up and execution */
  qcril_evt_e_type*                        dedicated_thrd_req_lookup_ptr;
  qcril_evt_e_type                         dedicated_thrd_req_lookup_val = nullptr;
  qmi_ril_dedicated_request_exec_params_type*
                                           dedicated_thrd_exec_params;
  pthread_attr_t                           dedicated_thrd_attr;
  pthread_t                                dedicated_thrd_thread_pid;
  int                                      dedicated_thrd_conf;

  qmi_ril_fw_android_param_copy_approach_type
                                           param_copy_approach;

  uint32                                   prepared_four_byte_storage;
  int                                      prepared_four_byte_storage_occupied;
  void*                                    prepared_sub_created_custom_storage;
  int                                      prepared_custom_storage_len;

  /* Entry pointer to dispatch the event */
  qcril_event_id_t *        entry_ptr;
  IxErrnoType error;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    entry_ptr = NULL;

    /* do a lookup for the entry */
    if ( qcril_hash_table_lookup( params_ptr->event_id, &entry_ptr ) != E_SUCCESS || NULL == entry_ptr )
    {
      /* The request is not supported */
      error = (E_NOT_SUPPORTED);
      break;
    }

    /* check if request requires dedicated thread for execution */
    dedicated_thrd_req_lookup_ptr = qmi_ril_fw_dedicated_thrd_exec_android_requests_set;
    do
    {
      dedicated_thrd_req_lookup_val = *dedicated_thrd_req_lookup_ptr;
      dedicated_thrd_req_lookup_ptr++;
    } while ( QMI_RIL_ZERO != dedicated_thrd_req_lookup_val && params_ptr->event_id != dedicated_thrd_req_lookup_val );

    /* If it needs to be started in a dedicated thread */
    if ( dedicated_thrd_req_lookup_val == params_ptr->event_id )
    {
      QCRIL_LOG_INFO("Need to spawn a dedicated thread for this event..Processing information");
      dedicated_thrd_exec_params = (qmi_ril_dedicated_request_exec_params_type *)qcril_malloc( sizeof(*dedicated_thrd_exec_params) );
      if ( NULL == dedicated_thrd_exec_params )
      {
        error = (E_NO_MEMORY);
        break;
      }

      //copy the params to dedicated exec params
      memset( dedicated_thrd_exec_params, 0, sizeof(*dedicated_thrd_exec_params) );
      dedicated_thrd_exec_params->event_id_android    = params_ptr->event_id_android;
      dedicated_thrd_exec_params->event_id    = params_ptr->event_id;
      dedicated_thrd_exec_params->token       = params_ptr->t;
      dedicated_thrd_exec_params->instance_id = params_ptr->instance_id;
      dedicated_thrd_exec_params->modem_id    = params_ptr->modem_id;

      int android_request_id = qcril_event_get_android_request(params_ptr->event_id);
      //copy the data to a storage, which will be destroyed once the thread is executed
      param_copy_approach = qmi_ril_fw_create_android_live_params_copy( android_request_id,
                                                                        params_ptr->data,
                                                                        params_ptr->datalen,
                                                                        (void*)&prepared_four_byte_storage,
                                                                        &prepared_four_byte_storage_occupied,
                                                                        &prepared_sub_created_custom_storage,
                                                                        &prepared_custom_storage_len,
                                                                        &error
                                                                       );

      dedicated_thrd_exec_params->param_copy_arrron = param_copy_approach;
      switch ( param_copy_approach )
      {
        case QMI_RIL_ANDROID_PARAM_CPY_APPRON_4_BYTES_SNAPSHOT:
          dedicated_thrd_exec_params->copied_params.four_bytes = prepared_four_byte_storage;
          break;

        case QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY:
          dedicated_thrd_exec_params->copied_params.dynamic_copy = prepared_sub_created_custom_storage;
          break;

        case QMI_RIL_ANDROID_PARAM_CPY_APPRON_EMPTY_NO_ACTION:
          // no action
          break;

        case QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID: // fallthrough
        default:
          qcril_free( dedicated_thrd_exec_params );  // rollback
          dedicated_thrd_exec_params = NULL;
          break;
      }
      if ( NULL == dedicated_thrd_exec_params )
      {
        break;
      }
      dedicated_thrd_exec_params->original_data_len = params_ptr->datalen;

      //Start a new thread qmi_ril_fw_dedicated_request_exec_thread
#ifdef QMI_RIL_UTF
      pthread_attr_init( &dedicated_thrd_attr );
      dedicated_thrd_conf = utf_pthread_create_handler(&dedicated_thrd_thread_pid, &dedicated_thrd_attr,
                            qmi_ril_fw_dedicated_request_exec_thread, (void*)dedicated_thrd_exec_params );

      pthread_attr_destroy( &dedicated_thrd_attr );
#else
      pthread_attr_init( &dedicated_thrd_attr );
      pthread_attr_setdetachstate( &dedicated_thrd_attr, PTHREAD_CREATE_DETACHED );
      dedicated_thrd_conf = pthread_create( &dedicated_thrd_thread_pid, &dedicated_thrd_attr, qmi_ril_fw_dedicated_request_exec_thread, (void*)dedicated_thrd_exec_params );
      pthread_attr_destroy( &dedicated_thrd_attr );
#endif

      //if thread creation failed, destroy the copied data
      if ( QMI_RIL_ZERO != dedicated_thrd_conf )
      { // failure, rollback
        QCRIL_LOG_ERROR( "dedicated thread launch failure %d", (int)dedicated_thrd_conf );
        qmi_ril_fw_destroy_android_live_params_copy( dedicated_thrd_exec_params->param_copy_arrron,
                                                     params_ptr->event_id,
                                                     (void*)(uintptr_t)dedicated_thrd_exec_params->copied_params.four_bytes,
                                                     dedicated_thrd_exec_params->copied_params.dynamic_copy );

        qcril_free( dedicated_thrd_exec_params );
        error = (E_FAILURE);
        break;

      }
      error = (E_SUCCESS);
      QCRIL_LOG_DEBUG( "Successfully routed to dedicated thrd, status %d", error );

      if( is_dedicated_thread )
      {
        *is_dedicated_thread = TRUE;
      }
    }
    else
    {
      //do dispatch internally
      QCRIL_LOG_INFO("Dispatching event/android request..");
      error = qcril_dispatch_event( entry_ptr, params_ptr );

      if( is_dedicated_thread )
      {
        *is_dedicated_thread = FALSE;
      }
    }
  }while(0);

  QCRIL_LOG_FUNC_RETURN_WITH_RET( (int)error );
  return error;
}

/*===========================================================================

  FUNCTION:  qcril_hash_table_lookup

===========================================================================*/
/*!
    @brief
    Finds the dispatch table entry for a given event_id

    @return
    E_SUCCESS if an entry is found
    E_FAILURE if no entry is found

*/
/*=========================================================================*/
IxErrnoType qcril_hash_table_lookup
(
  qcril_evt_e_type id,
  qcril_event_id_t **entry_ptr_ptr /*!< OUT table entry ptr, if found */
)
{
  IxErrnoType status = E_SUCCESS;

  do {
    if (!id || ! entry_ptr_ptr)
    {
        status = E_FAILURE;
        break;
    }
    *entry_ptr_ptr = id;
  } while(0);
  return (status);

} /* qcril_hash_table_lookup() */


/*===========================================================================

  FUNCTION:  qcril_update_event_table_entry

===========================================================================*/
/*!
    @brief
    Used to event with the new handler

    @return
    void
*/
/*=========================================================================*/
void qcril_update_event_table_entry(qcril_evt_e_type event_id, qcril_req_handler_type *handler)
{
    if(event_id != NULL)
    {
        event_id->handler = handler;
    }
}

/*===========================================================================

  FUNCTION: QCRIL_LOG_LOOKUP_EVENT_NAME

===========================================================================*/
/*!
    @brief
    Lookup the name of a QCRIL event

    @return
    The string respresenting the name of a QCRIL request
*/
/*=========================================================================*/
const char *qcril_log_lookup_event_name
(
  qcril_evt_e_type event_id
)
{
  /*-----------------------------------------------------------------------*/
    if (event_id) {
        return event_id->name;
    }
    return "Invalid";

} /* qcril_log_lookup_event_name */


boolean qcril_event_register_event(qcril_evt_e_type event_id )
{
    boolean ret = FALSE;
    if (event_id) {
        event_id->msg_id = REG_MSG(event_id->name);
        ret = TRUE;
    }
    return ret;
}

boolean qcril_is_event_in_group(qcril_evt_e_type event_id, qcril_evt_group group_id)
{
    return event_id ? event_id->group == group_id : FALSE;
}
