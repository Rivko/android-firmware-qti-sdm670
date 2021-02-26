/******************************************************************************
  @file    qcril_data.h
  @brief

  DESCRIPTION


  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (c) 2009-2010, 2013-2015, 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_DATA_H
#define QCRIL_DATA_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "assert.h"
#include <telephony/ril.h>
#include <framework/legacy.h>
#include <framework/Log.h>
#include "qcril_data_defs.h"
#include <pthread.h> /* mutex */
#include "legacy/qcril_event_facade.h"
#include "data_system_determination_v01.h"


// ***** IMPORTANT *******
// NOTE: version.h always be included when using
// QCRIL_RIL_VERSION
// ***********************
#include "modules/android/version.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                           MACRO DEFINITIONS

===========================================================================*/


/* Scan from end of device name string to determine numeric instance */
#define GET_DEV_INSTANCE_FROM_NAME(index, dest)                          \
    do                                                                   \
    {                                                                    \
      dest = QCRIL_DATA_INVALID_DEV;                                     \
      if (TRUE == info_tbl[index].info_flg)                              \
      {                                                                  \
        char *ptr = NULL;                                                \
        ptr = strpbrk(info_tbl[index].call_info.dev_name, "0123456789"); \
        if (ptr)                                                         \
        {                                                                \
          unsigned int len = strlen(ptr);                                \
          if (len && QCRIL_DATA_MAX_DEVS_SIZE >= len)                    \
          { /* TODO:consider changing this to strtol()*/                 \
            dest = atoi(ptr);                                            \
          }                                                              \
        }                                                                \
      }                                                                  \
    } while (0)


/*===========================================================================

                           EXTERN DEFINITIONS

===========================================================================*/

extern qmi_client_type  global_qmi_dsd_hndl;
extern int global_subs_id;
extern int global_instance_id;
extern qmi_client_type global_qcril_qmi_wds_hndl;
extern boolean ignore_ril_tech;
extern int last_call_end_reason;
extern char * default_qmi_port;
extern boolean global_qcril_clat_supported;
#ifdef RIL_REQUEST_SET_INITIAL_ATTACH_APN
  extern unsigned int global_lte_attach_profile;
#endif
extern unsigned int qcril_data_mtu;

#if (QCRIL_RIL_VERSION >= 15)
/**
 * List of modem assisted keepalive elements
 */
extern qcril_data_ma_ka_list_elem_t* qcril_data_ma_ka_list_head;
#endif

/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/
#define QCRIL_CDMA_STRING "0"
#define QCRIL_UMTS_STRING "1"

/* The ril_tech string provided by Android Telephony is RIL_RadioTechnology+2 */
#define QCRIL_DATA_CONVERT_RIL_RADIOTECH(x)  (x+2)

#define QCRIL_DATA_NUM_RIL_RADIO_TECHS  (QCRIL_DATA_RIL_RADIO_TECH_LAST-2)

/* More specific radio technology values provided by Android Telephony */
typedef enum
{
  QCRIL_DATA_RIL_RADIO_TECH_UNKNOWN = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_UNKNOWN),
  QCRIL_DATA_RIL_RADIO_TECH_GPRS    = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_GPRS),
  QCRIL_DATA_RIL_RADIO_TECH_EDGE    = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_EDGE),
  QCRIL_DATA_RIL_RADIO_TECH_UMTS    = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_UMTS),
  QCRIL_DATA_RIL_RADIO_TECH_IS95A   = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_IS95A),
  QCRIL_DATA_RIL_RADIO_TECH_IS95B   = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_IS95B),
  QCRIL_DATA_RIL_RADIO_TECH_1xRTT   = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_1xRTT),
  QCRIL_DATA_RIL_RADIO_TECH_EVDO_0  = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_EVDO_0),
  QCRIL_DATA_RIL_RADIO_TECH_EVDO_A  = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_EVDO_A),
  QCRIL_DATA_RIL_RADIO_TECH_HSDPA   = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_HSDPA),
  QCRIL_DATA_RIL_RADIO_TECH_HSUPA   = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_HSUPA),
  QCRIL_DATA_RIL_RADIO_TECH_HSPA    = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_HSPA),
  QCRIL_DATA_RIL_RADIO_TECH_EVDO_B  = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_EVDO_B),
  QCRIL_DATA_RIL_RADIO_TECH_EHRPD   = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_EHRPD),
  QCRIL_DATA_RIL_RADIO_TECH_LTE     = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_LTE),
  QCRIL_DATA_RIL_RADIO_TECH_LTE_CA  = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_LTE_CA),
  QCRIL_DATA_RIL_RADIO_TECH_HSPAP   = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_HSPAP),
  QCRIL_DATA_RIL_RADIO_TECH_GSM     = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_GSM),
  QCRIL_DATA_RIL_RADIO_TECH_TDSCDMA = QCRIL_DATA_CONVERT_RIL_RADIOTECH(RADIO_TECH_TD_SCDMA),

  /* This should be the last entry */
  QCRIL_DATA_RIL_RADIO_TECH_LAST

} qcril_data_ril_radio_tech_t;

#define QCRIL_PROFILE_IDX_MAX_LEN 4

#ifndef PDP_FAIL_PARTIAL_RETRY_FAIL
#define PDP_FAIL_PARTIAL_RETRY_FAIL -7
#endif

typedef enum
{
  IS_DATA_ENABLED,
  IS_DATA_ROAMING_ENABLED,
  SET_RAT_PREF,
  GET_RAT_PREF
}qcril_evt_async_calling_func;

#define QMI_DSD_MAX_APN_LEN_V01 100

typedef struct qcril_data_cb_struct_s
{
  RIL_Token t;
  qcril_evt_e_type event_id;
  int msg_id;
  void *self;
  RIL_Errno err_val;
  uint64 apn_type_mask;
  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  int bind_sub;
  qcril_evt_async_calling_func calling_func;
  int rat_pref;
  int prev_rat_pref;
}qcril_data_cb_struct;

/*===========================================================================

                    MACROs

===========================================================================*/

#define QCRIL_DS_LOG_DBG_MEM( msg, val )  \
  QCRIL_LOG_DEBUG( ">>>Mem debug>>> %s [%p]", msg, (unsigned int *)val );

#define QCRIL_DS_LOG_DBG_DSI_HNDL( dsi_hndl ) \
  QCRIL_LOG_DEBUG( "dsi_hndl is [%p]", (unsigned int *) dsi_hndl );

/*---------------------------------------------------------------------------
   Utility
---------------------------------------------------------------------------*/
#define MINIMUM(a,b)    ((a) < (b) ? (a) : (b))
#define MAXIMUM(a,b)    ((a) > (b) ? (a) : (b))
/*---------------------------------------------------------------------------
   Helper macro for logging function entry and exit
---------------------------------------------------------------------------*/
#define QCRIL_DS_FUNC_ENTRY(msg)    \
  QCRIL_LOG_VERBOSE( "%s:%s Entering func:%s! [%s]", __func__, msg )

#define QCRIL_DS_FUNC_EXIT(msg)    \
  QCRIL_LOG_VERBOSE( "%s:%s Entering func:%s! [%s]", __func__, msg )

/* Soft ASSERT */
#define QCRIL_DS_ASSERT( expr, msg )                                                   \
    if ( !( expr ) ) {                                                                 \
      QCRIL_LOG_ERROR( "%s", "*****ASSERTION FAILED (soft)*****" ); \
      QCRIL_LOG_ERROR( "Cond %s [%s]", #expr, msg );                                \
      QCRIL_LOG_ERROR( "%s", "*********************************" ); \
    }

/* Hard ASSERT */
#define QCRIL_DS_ASSERT_H( expr, msg )                                                 \
    if ( !( expr ) ) {                                                                 \
      QCRIL_LOG_ERROR( "%s", "*****ASSERTION FAILED (hard)*****" ); \
      QCRIL_LOG_ERROR( "Cond %s [%s]", #expr, msg );                                \
      QCRIL_LOG_ERROR( "%s", "*********************************" ); \
      assert(0);                                                                       \
    }

/*---------------------------------------------------------------------------
   Helper macro for locking and unlocking mutex with debug msgs
---------------------------------------------------------------------------*/

#define QCRIL_DATA_MUTEX_LOCK( m ) \
    do { \
      int lock_err = 0; \
      QCRIL_LOG_ADDITIONAL(">>>>>> LOCK QCRIL_DATA MUTEX %x", m); \
      lock_err = pthread_mutex_lock(m); \
      QCRIL_LOG_ADDITIONAL("LOCK QCRIL_DATA MUTEX %x result=%d", m, lock_err); \
      QCRIL_DS_ASSERT_H( lock_err == 0, "pthread_mutex_lock failed" ); \
    } while (0)

#define QCRIL_DATA_MUTEX_UNLOCK( m ) \
    do { \
      int unlock_err = 0; \
      QCRIL_LOG_ADDITIONAL("<<<<<< UNLOCK QCRIL_DATA MUTEX %x", m); \
      unlock_err= pthread_mutex_unlock(m); \
      QCRIL_LOG_ADDITIONAL("UNLOCK QCRIL_DATA MUTEX %x result=%d", m, unlock_err); \
      QCRIL_DS_ASSERT_H( unlock_err == 0, "pthread_muxtex_unlock failed" ); \
    } while (0)

#define QCRIL_DATA_MUTEX_TRYLOCK( m, result ) \
    do { \
      QCRIL_LOG_ADDITIONAL(">>>>>> TRY LOCK QCRIL_DATA MUTEX %x", m); \
      result = pthread_mutex_trylock(m); \
      QCRIL_LOG_ADDITIONAL("TRYLOCK QCRIL_DATA MUTEX %x result=%d", m, result); \
    } while (0)

#if (QCRIL_RIL_VERSION >= 15)
typedef void (*keep_alive_unsol_fp)(uint32_t, RIL_KeepaliveStatusCode);
#endif /*(QCRIL_RIL_VERSION >= 15)*/

/*=========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

  FUNCTION:  qcril_data_init

===========================================================================*/
/*!
    @brief
    Initialize the DATA subsystem of the RIL.

    (1) Call init routine of Data Services Internal Module

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_init();

/*=========================================================================
  FUNCTION:  qcril_data_unsol_call_list_changed

===========================================================================*/
/*!
    @brief
    Remove the entry from the ReqList.

    @return
    None
*/
/*=========================================================================*/
void qcril_data_unsol_call_list_changed
(
  qcril_instance_id_e_type instance_id
);

/*=========================================================================
  FUNCTION:  qcril_data_unsol_tethered_mode_state_changed

===========================================================================*/
/*!
    @brief
    send RIL_UNSOL_RESPONSE_TETHERED_MODE_STATE_CHANGED to QCRIL

    @return
    None
*/
/*=========================================================================*/

void qcril_data_unsol_tethered_mode_state_changed
(
  qcril_instance_id_e_type instance_id,
  void                    *response,
  size_t                   response_len
);


/*===========================================================================

  FUNCTION:  qcril_data_process_screen_state_change

===========================================================================*/
/*!
    @brief
    Update qcril_data about the current screen status. QCRIL data can perform
    further optimization processing as part of this indication.

    @args[in] Screen state:
              0 - screen OFF
              1 - screen ON

    @return QCRIL_DS_SUCCESS on success
    @return QCRIL_DS_ERROR on failure.
*/
/*=========================================================================*/
int
qcril_data_process_screen_state_change
(
  boolean screen_state
);

/*===========================================================================

  FUNCTION:  qcril_data_toggle_dormancy_indications

===========================================================================*/
/*!
    @brief

    Handles request to turn ON/OFF dormancy indications. Typically called to
    turn off indications when in power save mode  and turn back on when out
    of power save mode

    @return QCRIL_DS_SUCCESS on success and QCRIL_DS_ERROR on failure.
*/
/*=========================================================================*/
int
qcril_data_toggle_dormancy_indications
(
  qcril_data_dormancy_ind_switch_type       dorm_ind_switch
);

/*===========================================================================

  FUNCTION:  qcril_data_toggle_limited_sys_indications

===========================================================================*/
/*!
    @brief

    Handles request to turn ON/OFF limited data system status change
    indications. Typically called to TURN-ON limited indications when
    in screen-off state. In screen-on state, this is TURNED-OFF so
    full data system status indications can be received.

    @return QCRIL_DS_SUCCESS on success and QCRIL_DS_ERROR on failure.
*/
/*=========================================================================*/
int
qcril_data_toggle_limited_sys_indications
(
  qcril_data_limited_sys_ind_switch_type       limited_sys_ind_switch
);

#ifdef RIL_REQUEST_SET_INITIAL_ATTACH_APN

#if (QCRIL_RIL_VERSION >= 15)
/*===========================================================================

  FUNCTION: qcril_data_request_set_lte_attach_profile_v15

===========================================================================*/
/*!
  @brief
  Handles RIL_REQUEST_SET_INITIAL_ATTACH_APN version 15.

  @return
  None.

*/
/*===========================================================================*/
RIL_Errno qcril_data_request_set_lte_attach_profile_v15
(
  RIL_InitialAttachApn_v15* attachInfo
);
#else
/*===========================================================================

  FUNCTION: qcril_data_request_set_lte_attach_profile

===========================================================================*/
/*!
  @brief
  Handles RIL_REQUEST_SET_INITIAL_ATTACH_APN.

  @return
  None.

*/
/*===========================================================================*/
RIL_Errno qcril_data_request_set_lte_attach_profile
(
  RIL_InitialAttachApn* attachInfo
);
#endif /* QCRIL_RIL_VERSION >= 15 */

#endif /* RIL_REQUEST_SET_INITIAL_ATTACH_APN */

/*===========================================================================

  FUNCTION:  qcril_data_update_mtu

===========================================================================*/
/*!
    @brief
    Changes MTU value on all active calls
    @return
    NONE
*/
  /*===========================================================================*/
void qcril_data_update_mtu
(
  unsigned int mtu
);
/*===========================================================================

  FUNCTION:  qcril_data_set_is_data_enabled

===========================================================================*/
/*!
    @brief
    Sets the data enabled to true or false via DSD indication passed by
    upper layers
    @return
    SUCCESS :- If QMI DSD returns rc = 0
    FAILURE :- If QMI DSD returns rc = negative value
*/
  /*===========================================================================*/

RIL_Errno qcril_data_set_is_data_enabled
(
  const qcril_request_params_type *const params_ptr,
  boolean is_data_enabled
);
/*===========================================================================

  FUNCTION:  qcril_data_set_is_data_roaming_enabled

===========================================================================*/
/*!
    @brief
    Sets the data roaming enabled to true or false via DSD indication passed
    by upper layers
    @return
    SUCCESS:- If QMI DSD returns rc = 0
    FAILURE:- If QMI DSD returns rc = negative value
*/
  /*===========================================================================*/

RIL_Errno qcril_data_set_is_data_roaming_enabled
(
  const qcril_request_params_type *const params_ptr,
  boolean is_data_roaming_enabled
);
/*===========================================================================

  FUNCTION:  qcril_data_set_apn_info

===========================================================================*/
/*!
    @brief
    Sets the APN type and APN name via DSD indication passed by upper layers
    @return
    SUCCESS:- If QMI DSD returns rc = 0
    FAILURE:- If QMI DSD returns rc = negative value
*/
  /*===========================================================================*/

RIL_Errno qcril_data_set_apn_info
(
  const qcril_request_params_type *const params_ptr,
  char *type,
  char *name,
  int32 is_apn_valid
);


/*
===========================================================================*/
/*!
    @brief
    RX listening thread async function that needs to add a function to
     the event queue

    @return
*/
/*=========================================================================*/

void qcril_data_async_cb
(
  qmi_client_type                clnt_handle,
  unsigned int                   msg_id,
  void                          *resp_c_struct,
  unsigned int                   resp_c_struct_len,
  void                          *resp_cb_data,
  qmi_client_error_type          transp_err
);

/*===========================================================================

  FUNCTION:  qcril_data_async_hdlr

===========================================================================*/
/*!
    @brief
    Function called by QCRIL event queue thread
    @return
*/
/*=========================================================================*/
void qcril_data_async_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

/*===========================================================================

  FUNCTION:  qcril_data_async_hdlr

===========================================================================*/
/*!
    @brief
    Function called by QCRIL event queue thread
    @return
*/
/*=========================================================================*/
void qcril_data_set_rat_pref_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

/*=========================================================================
  FUNCTION:  qcril_data_util_create_setup_rsp

===========================================================================*/
/*!
    @brief
    Create RIL REQ SETUP response.

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    NONE
*/
/*=========================================================================*/
void qcril_data_util_create_setup_rsp
(
  qcril_data_call_info_tbl_type *info_tbl_ptr,
  qcril_data_call_response_type *rsp_ptr
);

/*=========================================================================
  FUNCTION:  qcril_data_response_data_call_failure

===========================================================================*/
/*!
    @brief
    Remove the entry from the ReqList. Send a RIL_E_SETUP_DATA_CALL_FAILURE
    cause code response.

    @return
    None
*/
/*=========================================================================*/

void qcril_data_response_data_call_failure
(
 qcril_instance_id_e_type instance_id,
 RIL_Token t,
 qcril_evt_e_type request,
 RIL_DataCallFailCause response
);

/*=========================================================================
  FUNCTION:  qcril_data_response_setup_data_call_failure

===========================================================================*/
/*!
    @brief
    Generate setup data call error response based on RIL API version.

    @return
    None
*/
/*=========================================================================*/

void qcril_data_response_setup_data_call_failure
(
  qcril_data_call_info_tbl_type * info_tble_ptr,
  qcril_instance_id_e_type instance_id,
  RIL_Token t,
  RIL_DataCallFailCause response
);

/*===========================================================================*/
/*!
    @brief
    Update the MTU in the call_info table

    @return
    None
*/
/*=========================================================================*/

void qcril_data_util_update_mtu
(
  qcril_data_call_info_tbl_type  *info_tbl_ptr
);

/*=========================================================================
  FUNCTION:  qcril_data_util_fill_call_params

===========================================================================*/
/*!
    @brief
    Uses QMI WDS service to get current call params and fill into
    the call table

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    TRUE: Call params retrieval success
    FALSE: Call params retrieval failure
*/
/*=========================================================================*/

boolean qcril_data_util_fill_call_params
(
  qcril_data_call_info_tbl_type *info_tbl_ptr
);



/*===========================================================================

  FUNCTION:  qcril_data_iface_ioctl

===========================================================================*/
/*!
    @brief
    executes the given ioctl on the given interface

    if ioctl is QCRIL_DATA_IOCTL_DORMANCY_INDICATIONS_ON,
    and if current dormancy status has changed from the last
    global status we were aware of, dorm_status_changed would be
    set to TRUE, and call_state would be set to appropriate
    PHYSLINK status.
    CALL_ACTIVE_PHYSLINK_DOWN
    CALL_ACTIVE_PHYSLINK_UP

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    SUCCESS
    FAILURE
*/
/*=========================================================================*/
int qcril_data_iface_ioctl
(
  qcril_data_call_info_tbl_type * info_tbl_ptr,
  int ioctl,
  boolean * dorm_status_changed,
  int * call_state
);

/*===========================================================================
  FUNCTION:  qcril_data_get_numeric_ril_technology
===========================================================================*/
/*!
  @brief
  Maps the given RIL technology string to a numeric value

  @param
  ril_tech - RIL technology string

  @return
  Numeric value corresponding to the ril_tech string
*/
/*=========================================================================*/
qcril_data_ril_radio_tech_t
qcril_data_get_numeric_ril_technology
(
  const char *ril_tech
);

/*===========================================================================

  FUNCTION:  qcril_data_net_cb

===========================================================================*/
/*!
    @brief
    Called on call control events from Data Services (DSI).

    @pre Before calling, dsi_cb_tbl_mutex must not be locked by the
         calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_net_cb(
  dsi_hndl_t     dsi_hndl,
  void          *user_data,
  dsi_net_evt_t      net_evt,
  dsi_evt_payload_t *payload
);

/*===========================================================================

  FUNCTION:  qcril_data_start_response_timer

===========================================================================*/
/*!
    @brief
    Create and start a response timer for setup data call request

    @pre Before calling, info_tbl_mutex must be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_start_response_timer
(
  qcril_data_call_info_tbl_type *info_tbl_ptr
);

/*===========================================================================

  FUNCTION:  qcril_data_stop_data_call

===========================================================================*/
/*!
    @brief
    Performs the steps required to stop a data call.

    @pre Before calling, info_tbl_mutex must be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/

IxErrnoType qcril_data_stop_data_call
(
  qcril_data_call_info_tbl_type *info_tbl_ptr
);

/*===========================================================================

  FUNCTION:  qcril_data_is_embms_active

===========================================================================*/
/*!
    @brief
    Checks if an eMBMS call is active already

    @pre
    None

    @return
    TRUE if eMBMS call is active, FALSE if not
*/
/*=========================================================================*/
int qcril_data_is_embms_active
(
  void
);

/*===========================================================================
  FUNCTION:  qcril_data_handle_embms_events

===========================================================================*/
/*!
    @brief
    Handles embms events. This is an internal function.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void
qcril_data_handle_embms_events
(
  qcril_instance_id_e_type       instance_id,
  qcril_data_call_info_tbl_type *info_tbl_ptr,
  qcril_data_event_data_t       *evt_info_ptr
);


/*===========================================================================

  FUNCTION:   qcril_data_set_rat_preference

===========================================================================*/
/*!
    @brief
    This function sets the RAT preference

    @return
    RIL_E_SUCCESS         :- If we modem accepts this setting
    RIL_E_GENERIC_FAILURE :- If modem rejects this setting
*/
/*=========================================================================*/
RIL_Errno qcril_data_set_rat_preference
(
  int rat_pref_type
);

/*===========================================================================

  FUNCTION:   qcril_data_get_rat_preference

===========================================================================*/
/*!
    @brief
    This function gets the current RAT preference

    @return
     If query is success,

       Current RAT preference on the modem
         DSD_RAT_PREFERENCE_INACTIVE_V01 = 0,
         DSD_RAT_PREFERENCE_CELLULAR_ONLY_V01 = 1,
         DSD_RAT_PREFERENCE_WIFI_ONLY_V01 = 2,
         DSD_RAT_PREFERENCE_CELLULAR_PREFERRED_V01 = 3,
         DSD_RAT_PREFERENCE_WIFI_PREFERRED_V01 = 4,

     else return -1

*/
/*=========================================================================*/
int qcril_data_get_rat_preference
(
  void
);

#ifdef FEATURE_DATA_LQE
/*===========================================================================

  FUNCTION:  qcril_data_lqe_init

===========================================================================*/
/*!
    @brief

    Do LQE init and register for a client. This must be called only once

    @pre Before calling, lqe_mutex must be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
int qcril_data_lqe_init
(
  int instance_id,
  int modem_id
);

/*===========================================================================

  FUNCTION:  qcril_data_lqe_release

===========================================================================*/
/*!
    @brief

    Do LQE release and unregister the client. This must be called after lqe_init()

    @pre Before calling, lqe_mutex must be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
int qcril_data_lqe_release(void);

#endif /* FEATURE_DATA_LQE */

/*===========================================================================

  FUNCTION:  qcril_data_get_dds_sub_info

===========================================================================*/
/*!
    @brief
    Query the current DDS sub ID from modem

    @pre None

    @return
      if modem query is successful then returns sub id
      else returns -1
*/
/*=========================================================================*/
DDSSubIdInfo qcril_data_get_dds_sub_info(void);

/*===========================================================================

  FUNCTION:  qcril_data_request_dds_switch

===========================================================================*/
/*!
    @brief
    Request DDS switch
    @pre None

    @return RIL_Errno

*/
/*=========================================================================*/
RIL_Errno qcril_data_request_dds_switch(DDSSubIdInfo dds_sub_info);

/*===========================================================================

  FUNCTION:  qcril_data_request_data_call_list

===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_DATA_CALL_LIST.

    @return
    PDP Context List
    None.
*/
/*=========================================================================*/
void qcril_data_request_data_call_list(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_data_request_setup_data_call

===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_SETUP_DATA_CALL

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_request_setup_data_call
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_data_command_hdlr

===========================================================================*/
/*!
    @brief
    Command handler for QCRIL Data

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_command_hdlr(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*===========================================================================
  FUNCTION:  qcril_data_event_hdlr

===========================================================================*/
/*!
    @brief
    Registered with QCRIL to be called by QCRIL on event
    QCRIL_EVT_DATA_EVENT_CALLBACK

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_event_hdlr(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

/*===========================================================================

  FUNCTION:  qcril_data_async_hdlr

===========================================================================*/
/*!
    @brief
    Function called by QCRIL event queue thread
    @return
*/
/*=========================================================================*/
void qcril_data_async_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

/*===========================================================================

  FUNCTION:  qcril_data_set_rat_pref_hdlr

===========================================================================*/
/*!
    @brief
    Function called by QCRIL event queue thread especially for
    Setting RAT preference
    @return
*/
/*=========================================================================*/
void qcril_data_set_rat_pref_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

/*===========================================================================
  FUNCTION:  qcril_data_wds_event_hdlr

===========================================================================*/
/*!
    @brief
    Registered with QCRIL to be called by QCRIL on event
    QCRIL_EVT_DATA_WDS_EVENT_CALLBACK

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_wds_event_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

/*===========================================================================
  FUNCTION:  qcril_data_dsd_event_hdlr

===========================================================================*/
/*!
    @brief
    Registered with QCRIL to be called by QCRIL on event
    QCRIL_EVT_DATA_DSD_EVENT_CALLBACK

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_dsd_event_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

/*===========================================================================
  FUNCTION:  qcril_data_qmi_wds_suspend_hdlr

===========================================================================*/
/*!
    @brief
    Registered with QCRIL to be called by QCRIL on event QCRIL_EVT_DATA_WDS_SUSPEND_CALLBACK

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_qmi_wds_suspend_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

/*===========================================================================
  FUNCTION:  qcril_data_qmi_wds_resume_hdlr

===========================================================================*/
/*!
    @brief
    Registered with QCRIL to be called by QCRIL on event QCRIL_EVT_DATA_WDS_RESUME_CALLBACK

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_qmi_wds_resume_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

/*===========================================================================
  FUNCTION:  qcril_data_qmi_dsd_suspend_hdlr

===========================================================================*/
/*!
    @brief
    Registered with QCRIL to be called by QCRIL on event QCRIL_EVT_DATA_DSD_SUSPEND_CALLBACK

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_qmi_dsd_suspend_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

/*===========================================================================
  FUNCTION:  qcril_data_qmi_dsd_resume_hdlr

===========================================================================*/
/*!
    @brief
    Registered with QCRIL to be called by QCRIL on event QCRIL_EVT_DATA_DSD_RESUME_CALLBACK

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_qmi_dsd_resume_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

/*===========================================================================
  FUNCTION:  qcril_data_lqe_event_hdlr

===========================================================================*/
/*!
    @brief
    Registered with QCRIL to be called by QCRIL on event
    QCRIL_EVT_DATA_LQE_EVENT_CALLBACK

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_lqe_event_hdlr(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);

#if (QCRIL_RIL_VERSION >=15)
/*===========================================================================
  FUNCTION:  qcril_data_keepalive_event_hdlr

===========================================================================*/
/*!
    @brief
    @return
*/
/*=========================================================================*/
void qcril_data_keepalive_event_hdlr(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
);
#endif

/*===========================================================================

  FUNCTION:  qcril_data_embms_enable_data_req

===========================================================================*/
/*!
    @brief

    Handles QCRIL_EVT_QMI_RIL_EMBMS_ENABLE_DATA_REQ.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
void qcril_data_embms_enable_data_req
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION:  qcril_data_lqe_get_info

===========================================================================*/
/*!
    @brief

    Handles RIL_REQUEST_PULL_LCEDATA.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
void qcril_data_lqe_get_info
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION:  qcril_data_lqe_start

===========================================================================*/
/*!
    @brief

    Handles RIL_REQUEST_START_LCE.

    @pre Before calling, lqe_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
void qcril_data_lqe_start
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION:  qcril_data_lqe_stop

===========================================================================*/
/*!
    @brief

    Handles RIL_REQUEST_STOP_LCE.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
void qcril_data_lqe_stop
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION:   qcril_data_set_apn_info

===========================================================================*/
/*!
    @brief
    sets the apn info table

    @return
    RIL_E_SUCCESS         :- If QMI DSD returns rc = 0
    RIL_OEM_errors :- If QMI DSD returns rc = negative
*/
/*=========================================================================*/

RIL_Errno qcril_data_set_apn_info(
const qcril_request_params_type *const params_ptr,
char *type,
char *name,
int32 is_apn_valid);

/*===========================================================================

    FUNCTION:  qcril_data_request_set_data_profile

    ===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_SET_DATA_PROFILE

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/

void qcril_data_request_set_data_profile
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION: qcril_data_is_attach_required

===========================================================================*/
/*!
  @brief
  Checks if the attach profile parameters sent by telephony with the
  attach profile parameters on the modem. LTE attach would be required
  only if there is any change in the attach APN parameters

  @return
  TRUE if any of the parameters doesn't match
  FALSE if all the parameters match

*/
/*===========================================================================*/
LOCAL boolean qcril_data_is_attach_required
(
  unsigned int  profile_id,
  const char    **param_strings
);

/*===========================================================================

  FUNCTION:  qcril_data_set_ril_profile_id

===========================================================================*/
/*!
    @brief
    Sets the RIL provided profile id (technology specific) into the
    dsi_hndl

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    SUCCESS
    FAILURE
*/
/*=========================================================================*/
// static int qcril_data_set_ril_profile_id
// (
//   int info_tbl_index,
//   const char * ril_profile,
//   const char * ril_tech
// );

/*===========================================================================

  FUNCTION:  qcril_data_init

===========================================================================*/
/*!
    @brief
    Initialize the DATA subsystem of the RIL.

    (1) Call init routine of Data Services Internal Module

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_init();
/*===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_DEACTIVATE_DATA_CALL.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_request_deactivate_data_call
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

 FUNCTION:  qcril_data_request_omh_profile_info

===========================================================================*/
/*!
  @brief
    Handles RIL_REQUEST_GET_DATA_CALL_PROFILE.
    On Success a list of structs containg the  OMH app type and app priorities
    is sent to RIL as response.
  @return
*/
/*=========================================================================*/

void qcril_data_request_omh_profile_info
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_data_process_stack_switch
(
  qcril_modem_stack_id_e_type old_stack_id,
  qcril_modem_stack_id_e_type new_stack_id,
  qcril_instance_id_e_type instance_id
);
/*===========================================================================

  FUNCTION:  qcril_data_set_is_data_enabled

===========================================================================*/
/*!
    @brief
    sends data enabled to modem if qcril passes the parameter either true
    or false.

    @return
    RIL_E_SUCCESS         :- If QMI DSD returns rc = 0
    RIL_OEM_Erros :- If QMI DSD returns rc = negative
*/
/*=========================================================================*/
RIL_Errno qcril_data_set_is_data_enabled
(
  const qcril_request_params_type *const params_ptr,
  boolean is_data_enabled
);
/*===========================================================================

  FUNCTION:   qcril_data_set_quality_measurement_info

===========================================================================*/
/*!
    @brief
    This function configures the Modem with RAT specific quailiy measurement
    parameters

    @return
    None

*/
/*=========================================================================*/
qmi_response_type_v01 qcril_data_set_quality_measurement
(
  dsd_set_quality_measurement_info_req_msg_v01 *info
);
/*===========================================================================

  FUNCTION:  qcril_data_process_qcrilhook_go_dormant

===========================================================================*/
/*!
    @brief

    Handles RIL_REQUEST_OEM_HOOK_RAW - QCRIL_EVT_HOOK_DATA_GO_DORMANT.
    The input to this handler can be a name of an interface, in which
    case this routine will issue dormancy command on the specified interface.
    If no input is provided, Dormancy is issued on all Active interfaces.
    The request is considered to be successful on receipt of PHSYLINK_DOWN
    indication from DSS.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure

    Notes: This handler assumes that dev names are enumerated as rmnet[0,1,2].
*/
/*=========================================================================*/
void qcril_data_process_qcrilhook_go_dormant
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);
/*===========================================================================

  FUNCTION:  qcril_data_request_last_data_call_fail_cause

===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE.
    This function calls
    @return
    None.
*/
/*=========================================================================*/
void qcril_data_request_last_data_call_fail_cause
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

/*===========================================================================
  FUNCTION:  QCRIL_DATA_GET_CALL_END_REASON

===========================================================================*/
/*!
    @brief
    Provides call end reason based on IP family type and roaming status.
    For dual-IP call failures, chooses the temporary call end reason (if
    available)
    @pre Before calling, info_tbl_mutex must not be locked by the calling
    thread.

    @return
    QCRIL_DS_SUCCESS on success, QCRIL_DS_ERROR on error
*/
/*=========================================================================*/
int qcril_data_get_call_end_reason
(
  qcril_data_call_info_tbl_type * info_tbl_ptr,
  int * call_end_reason
);

#if (QCRIL_RIL_VERSION >= 15)
/*===========================================================================

  FUNCTION:  qcril_data_get_ma_ka_elem_by_handle

===========================================================================*/
/*!
    @brief
    Get ptr to an element if it exists in local cache by handle

    @return
    qcril_data_ma_ka_list_elem_t* or NULL
*/
/*=========================================================================*/
qcril_data_ma_ka_list_elem_t *qcril_data_get_ma_ka_elem_by_handle(uint32_t handle);

/*===========================================================================

  FUNCTION:  qcril_data_get_ma_ka_elem_by_cid

===========================================================================*/
/*!
    @brief
    Get ptr to an element if it exists in local cache by call id

    @return
    qcril_data_ma_ka_list_elem_t* or NULL
*/
/*=========================================================================*/
qcril_data_ma_ka_list_elem_t *qcril_data_get_ma_ka_elem_by_cid(int cid);

/*===========================================================================

  FUNCTION:  qcril_data_ma_ka_create_element

===========================================================================*/
/*!
    @brief
    Given a handle, create a new element

    @return
    qcril_data_ma_ka_list_elem_t*
*/
/*=========================================================================*/
qcril_data_ma_ka_list_elem_t* qcril_data_ma_ka_create_element(uint32_t handle,
  RIL_Token token);

/*===========================================================================

  FUNCTION:  qcril_data_ma_ka_insert_ka_elem

===========================================================================*/
/*!
    @brief
    insert given elem at head

    @return
    void
*/
/*=========================================================================*/
void qcril_data_insert_ma_ka_elem(qcril_data_ma_ka_list_elem_t* elem);

/*===========================================================================

  FUNCTION:  qcril_data_ma_ka_delete_elem

===========================================================================*/
/*!
    @brief
    Given handle, remove from cache if found

    @return
    void
*/
/*=========================================================================*/
void qcril_data_ma_ka_delete_elem(uint32_t handle);


/*===========================================================================

  FUNCTION:  qcrili_data_ma_ka_cleanup

===========================================================================*/
/*!
    @brief
    Clean up keepalive sessions if cid >= 0.
    To clean up all sessions regardless of call if, pass in cid of -1.

    @return
    none
*/
/*=========================================================================*/
void qcrili_data_ma_ka_cleanup(const int cid);

/*===========================================================================

  FUNCTION:  qcril_data_ma_ka_delete_elem_by_cid

===========================================================================*/
/*!
    @brief
    Given cid, remove from cache if found

    @return
    void
*/
/*=========================================================================*/
void qcril_data_ma_ka_delete_elems_by_cid(int cid);

/*===========================================================================

  FUNCTION:  qcril_data_ma_ka_send_unsol

===========================================================================*/
/*!
    @brief

    @return
    void
*/
/*=========================================================================*/
void qcril_data_ma_ka_send_unsol(uint32_t handle, RIL_KeepaliveStatusCode status_code);

/*===========================================================================

  FUNCTION:  qcril_data_start_modem_assist_keepalive

===========================================================================*/
/*!
    @brief
    Start modem assisted keepalive feature request

    @return
    void
*/
/*=========================================================================*/
void qcril_data_start_modem_assist_keepalive(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_data_stop_modem_assist_keepalive

===========================================================================*/
/*!
    @brief
    Send request to modem to stop kepalive timer

    @return
    void
*/
/*=========================================================================*/
void qcril_data_stop_modem_assist_keepalive(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_data_stop_keepalive_for_handle

===========================================================================*/
/*!
    @brief
    Send request to modem to stop kepalive timer per handle

    @return
    void
*/
/*=========================================================================*/
int qcril_data_stop_keepalive_for_handle(
  uint32_t handle,
  bool bSendUnsol
);
#endif /* QCRIL_RIL_VERSION >= 15 */

bool qcril_data_device_shutdown();

#ifdef __cplusplus
}
#endif

#endif /* QCRIL_DATA_H */
