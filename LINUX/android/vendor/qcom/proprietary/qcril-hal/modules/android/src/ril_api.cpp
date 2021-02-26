/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
    extern "C" {
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <comdef.h>
#include <telephony/ril.h>
    }
#include <framework/Module.h>
#include <framework/Log.h>
#include <framework/legacy.h>
#include <framework/IxErrno.h>
#include <framework/QcrilInitMessage.h>
#include <framework/Resource.h>
#include <modules/android/version.h>
#include <modules/android/ril_api.h>
#include <modules/android/ClientConnectedMessage.h>
#include <modules/android/ril_message_factory.h>
#include "android_ril_module.h"

#define TAG "RILQ_ril_api"
#define QMI_RIL_QMI_RILD_THREAD_NAME                    "rild"

    static void onRequest( int request, void *data, size_t datalen, RIL_Token t );
    static int onSupports ( qcril_instance_id_e_type instance_id, int request);
    static RIL_RadioState currentState_rid();
    static int onSupports_rid( int request );
    static void onCancel_rid( RIL_Token t );
    static const char *getVersion_rid( void );

    static RIL_RadioState currentState
    (
      qcril_instance_id_e_type instance_id
    );

    static const RIL_RadioFunctions qcril_request_api[] = {
      { QCRIL_RIL_VERSION, onRequest, currentState_rid, onSupports_rid, onCancel_rid, getVersion_rid }
    };
    static RIL_Env *qcril_response_api[QCRIL_MAX_INSTANCE_ID];
    //static void qmi_ril_init_android_unsol_resp_pending_list();

    //static void qmi_ril_initiate_bootup(void);
    //static void qmi_ril_bootup_perform_core_or_start_polling(void * params);

    /*===========================================================================

      FUNCTION:  onRequest

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
    void onRequest
    (
      int request,
      void *data,
      size_t datalen,
      RIL_Token t
    )
    {
        QCRIL_LOG_DEBUG("onRequest. token: %d (%p). id: %d", *(uint32_t *)t, t, request);
        get_android_ril_module().dispatch_request(qmi_ril_get_process_instance_id(), request, data, datalen, t);
    } /* onRequest_rid0 */


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
    extern "C" const RIL_RadioFunctions *RIL_Init
    (
      const struct RIL_Env *env,
      int argc,
      char **argv
    )
    {
        int client_id = 0;
        char **c_argv = argv;
        /* Ignore SIGPIPE signal so that rild does not crash
           even if the other end of the socket is closed abruptly. */
        signal(SIGPIPE, SIG_IGN);

#ifdef QMI_RIL_UTF
        client_id = atoi(argv[2]);
#else
        int opt = -1;
        while ( -1 != (opt = getopt(argc, argv, "p:d:s:c:"))) {
            switch (opt) {
                case 'c':
                    client_id = atoi(optarg);
                    QCRIL_LOG_INFO( "RIL client opt: %d, running RIL_Init()", client_id);
                    break;
                default:
                    QCRIL_LOG_INFO("RIL client opt: %d not handled intreseted only -c option", opt);
                    break;
            }
        }
#endif
        qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;
        switch(client_id)
        {
            case 1:
                instance_id = QCRIL_DEFAULT_INSTANCE_ID;
                break;
            case 2:
                instance_id = QCRIL_SECOND_INSTANCE_ID;
                break;
            case 3:
                instance_id = QCRIL_THIRD_INSTANCE_ID;
                break;
        }

#ifndef QMI_RIL_UTF
        Logger::setPrefix("RIL[" + std::to_string((int)(instance_id)) + "]");
#endif
        QCRIL_LOG_INFO( "RIL client : %d, running RIL_Init()", client_id);

        // o maintain compatibility with data and UIM code which use instance_id and may respond on "second instance" context
        qcril_response_api[ QCRIL_DEFAULT_INSTANCE_ID ] = (struct RIL_Env *) env;
        qcril_response_api[ QCRIL_SECOND_INSTANCE_ID ] = (struct RIL_Env *) env;
        // TODO_TSTS: Check if this is required. Seems not required.
        qcril_response_api[ QCRIL_THIRD_INSTANCE_ID ] = (struct RIL_Env *) env;

        get_android_ril_module().ril_init(instance_id, env, argc, c_argv);

    QCRIL_LOG_DEBUG( "RIL %d, running RIL_Init()", qmi_ril_get_process_instance_id() );
    return &qcril_request_api[ QCRIL_DEFAULT_INSTANCE_ID ];

} /* RIL_Init() */
#endif

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
RIL_RadioState currentState_rid
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
int onSupports
(
  qcril_instance_id_e_type instance_id,
  int request
)
{
  int supported = 0;

  supported = get_android_ril_module().is_request_supported(instance_id, request);
  //TODO REARCH: QCRIL_LOG_CF_PKT_RIL_FN( instance_id, label );

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
int onSupports_rid
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
void onCancel
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

  QCRIL_LOG_CF_PKT_RIL_FN( instance_id, (char *)"onCancel()" );

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
void onCancel_rid
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
const char *getVersion
(
  qcril_instance_id_e_type instance_id
)
{
  const char *version = "Qualcomm RIL 1.0";

  /*-----------------------------------------------------------------------*/

  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );

  /*-----------------------------------------------------------------------*/


  QCRIL_LOG_DEBUG( "version %s", version );

  QCRIL_LOG_CF_PKT_RIL_FN( instance_id, (char *)"getVersion() - Qualcomm RIL 1.0 (QMI)" );

  return version;

} /* getVersion() */

//===========================================================================
//qmi_ril_bootup_actition_on_rild_atel_link_connect
//===========================================================================
void qmi_ril_bootup_actition_on_rild_atel_link_connect(void * params)
{
  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED( params );
  //qcril_sms_post_ready_status_update();
  auto msg = std::make_shared<ClientConnectedMessage>(qmi_ril_get_process_instance_id());

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_bootup_actition_on_rild_atel_link_connect

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
const char *getVersion_rid
(
  void
)
{
  /*-----------------------------------------------------------------------*/

  get_android_ril_module().setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID, qmi_ril_bootup_actition_on_rild_atel_link_connect, NULL, NULL );

  return getVersion( QCRIL_DEFAULT_INSTANCE_ID );

} /* getVersion_rid0() */


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
RIL_RadioState currentState
(
  qcril_instance_id_e_type instance_id
)
{
    return get_android_ril_module().currentState(instance_id);
} /* currentState() */


void android_ril_on_request_ack(qcril_instance_id_e_type instance_id,
        RIL_Token token)
{
  QCRIL_LOG_FUNC_ENTRY();
  if(QCRIL_RIL_VERSION >= 13)
  {
    if (token /*&& !qcril_is_internal_token(token)*/)
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

void android_ril_on_request_complete(qcril_instance_id_e_type instance_id,
        RIL_Token token_id,
        RIL_Errno e,
        void *response,
        size_t response_len)
{
    QCRIL_LOG_FUNC_ENTRY("Completed request for token (valid: %s) %d with error: %d",
            token_id ? "true" : "false",
            token_id ? *(int *)token_id : -1,
            e);

    if (token_id) {
        qcril_response_api[instance_id]->OnRequestComplete(token_id, e, response, response_len);
    } else {
        QCRIL_LOG_INFO("Not sending NULL token");
    }

    QCRIL_LOG_FUNC_RETURN();
}

void android_ril_on_request_complete(qcril_instance_id_e_type instance_id,
        RIL_Token token_id,
        RIL_Errno e,
        std::shared_ptr<void> response,
        size_t response_len)
{
    android_ril_on_request_complete(instance_id, token_id, e, response.get(), response_len);
}

void android_ril_setup_timed_callback
(
 qcril_instance_id_e_type instance_id,
 RIL_TimedCallback callback,
 void *param,
 const struct timeval *relativeTime
)
{
    if (qcril_response_api[instance_id] && qcril_response_api[instance_id]->RequestTimedCallback)
    {
        qcril_response_api[instance_id]->RequestTimedCallback(callback, param, relativeTime);
    }
    else
    {
        QCRIL_LOG_ERROR( "Unable to call RequestTimedCallback. qcril_response_api[instance_id]: %p. ->RequestTimedCallback: ");
    }
}


void android_ril_on_request_complete(std::shared_ptr<ril_response_type> rsp)
{
    if (rsp)
    {
        android_ril_on_request_complete(rsp->instance_id,
                rsp->t,
                rsp->e,
                rsp->response,
                rsp->response_len);
    }
    else
    {
        QCRIL_LOG_ERROR("Requested to send response but no valid ril_response_type given");
    }
}


