/*!
  @file
  qcril_data_req_resp.c

  @brief
  Handles RIL requests for DATA services specifically RIL requests and
  responses.

*/

/*===========================================================================

  Copyright (c) 2008-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //linux/pkgs/proprietary/qc-ril/main/source/qcril_data.c#17 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/11/16: Created file and migrated functions from qcril_data_netctrl.c

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <stdio.h>
#include <string.h>
#ifdef QCRIL_DATA_OFFTARGET
  #include <netinet/in.h>
  #include <errno.h>
  #include<linux/sockios.h>
#endif
#include <net/if.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#ifdef QCRIL_DATA_OFFTARGET
   #include <signal.h>
#else
   #include <asm-generic/siginfo.h>
#endif
#include <linux/socket.h>

#include <telephony/ril.h>
#include "IxErrno.h"
#include "qcril_reqlist.h"
#include "dsi_netctrl.h"
#include "qmi_platform.h"
#include "qcril_data_qmi_wds.h"
#include "qdp.h"


#ifdef QCRIL_DATA_OFFTARGET
  #include "qcril_data_stubs.h"
#else
  #include "qcril_event_facade.h" /* QCRIL_ARB_* defines*/
#endif

#include "qcril_data.h"
#include "qcril_data_defs.h"
#include "qmi_wds_srvc.h"
#include "qcril_data_utils.h"
#include "DataCommon.h"

#define HACK_MODEM_QUERY_SEARCH_UNAVAIL
#include <string.h>

#include <cutils/properties.h>
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
   // Don't have system properties on Ubunto x86, use property server
   #include <sys/system_properties.h>
#endif
#include "ds_string.h"
#include "data_system_determination_v01.h"
#include "qmi_client_instance_defs.h"
#include "ds_util.h"

// ***** IMPORTANT *******
// NOTE: version.h always be included when using
// QCRIL_RIL_VERSION
// ***********************
#include "modules/android/version.h"

#ifndef QCRIL_RIL_VERSION
#error QCRIL_RIL_VERSION needs to be defined!
#endif


#define QCRIL_DATA_PROPERTY_TIMEOUT             "persist.vendor.radio.datacall_timeout"
#define QCRIL_DATA_PROPERTY_TIMEOUT_SIZE        (6)

#define QCRIL_DATA_PROPERTY_TIMEOUT_INVALID  (0)

#define QCRIL_DATA_CALL_OBJ_MATCH_FOUND      (SUCCESS)
#define QCRIL_DATA_CALL_OBJ_MATCH_NOT_FOUND  (FAILURE)

#define QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT  (10000)
#define QCRIL_QMI_DSD_TIMEOUT                (30000)

#define QMI_WDS_CDMA_PROFILE_APP_TYPE_TLV_ID 0x1E
#define QMI_WDS_CDMA_PROFILE_APP_TYPE_TLV_SIZE 4


#define QCRIL_NETWORK_INFO_LEN 10

#define QCRIL_DATA_PROP_SIZE     PROPERTY_VALUE_MAX

#ifdef RIL_REQUEST_SET_INITIAL_ATTACH_APN

#define QCRIL_PROPERTY_DEFAULT_PROFILE       "persist.vendor.radio.attach.profile"
#define QCRIL_LTE_DEFAULT_PROFILE_VALUE      "0"
#define QCRIL_PROPERTY_DEFAULT_PROFILE_SIZE  (5)
#define QCRIL_LTE_ATTACH_SUCCESS             (0)
#define QCRIL_LTE_ATTACH_FAILURE             (1)
#define QCRIL_LTE_SKIP_ATTACH                (2)

#endif /* RIL_REQUEST_SET_INITIAL_ATTACH_APN */

#define IPCONV( ip_addr , t ) ( ( ip_addr >> (24 - 8 * t)) & 0xFF)

#define QCRIL_MAX_DEV_NAME_SIZE             DS_CALL_INFO_DEV_NAME_MAX_LEN
#define QCRIL_DATA_MAX_DEVS                 16
#define QCRIL_DATA_MAX_DEVS_SIZE            2  /* Digits required */
#define QCRIL_DATA_INVALID_DEV              (-1)

extern char *strcasestr(const char *s1, const char *s2);
typedef enum
{
  QCRIL_DATA_IFACE_IOCTL_GO_DORMANT,
  QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_OFF,
  QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_ON
} qcril_data_iface_ioctl_type;

/* add 1 for ending NULL character */
typedef char qcril_data_go_dormant_params_type[QCRIL_MAX_DEV_NAME_SIZE+1];


#define QCRIL_DATA_SUGGESTEDRETRYTIME  (-1)      /* no value suggested, see ril.h */

/*! @brief Typedef variables internal to module qcril_data.c
*/
typedef struct
{
  unsigned char rflag;  /* determines RIL Token validity */
  RIL_Token     rt;     /* valid RIL token if flag is set */
  int           request;
  void         *data;
  void         *self;
} qcril_data_net_cb_info_t;

#define QCRIL_DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(t)  ((t == QCRIL_DATA_RIL_RADIO_TECH_IS95A)  || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_IS95B)  || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_1xRTT)  || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_EVDO_0) || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_EVDO_A) || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_EVDO_B))

#define QCRIL_DATA_IS_RIL_RADIO_TECH_EHRPD(t) (t == QCRIL_DATA_RIL_RADIO_TECH_EHRPD)

#define QCRIL_DATA_IS_RIL_RADIO_TECH_CDMA(t)  (QCRIL_DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(t) ||        \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_EHRPD))

static char *available_apn_types[] = {
    "*",
    "default",
    "ims",
    "mms",
    "dun",
    "supl",
    "hipri",
    "fota",
    "cbs",
    "ia",
    "emergency",
    "internet"
  };

#define QCRIL_DATA_MAX_APN_TYPES (sizeof(available_apn_types)/sizeof(available_apn_types[0]))

typedef struct qcril_data_list_elem_t {
  char name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  uint64 mask;
  struct qcril_data_list_elem_t *next;
}qcril_data_list_elem_t;

#if (QCRIL_RIL_VERSION >= 15)
qcril_data_ma_ka_list_elem_t* qcril_data_ma_ka_list_head = NULL;
#endif

/*===========================================================================

                    INTERNAL DEFINITIONS

===========================================================================*/

static int qcril_data_set_ril_profile_id
(
  int info_tbl_index,
  const char * ril_profile,
  const char * ril_tech
);

static void qcril_data_response_setup_data_call_failure_generic_err
(
  qcril_instance_id_e_type  instance_id,
  RIL_Token                 t
);

static void
qcril_data_store_call_params
(
  qcril_data_call_info_tbl_type  *info_tbl_ptr,
  const char                     *ril_tech,
  const char                     *ril_profile,
  const char                     *ril_apn,
  const char                     *ril_user,
  const char                     *ril_pass,
  const char                     *ril_auth_pref,
  const char                     *ril_ipfamily,
  const char                     *ril_ip_roaming,
  const char                     *ril_apn_types,
  const char                     *ril_roaming_allowed
);

static int qcril_data_apn_based_profile_look_up_using_qdp
(
  const char * ril_apn,
  const char * ril_ipfamily,
  const char * ril_ip_roaming,
  const char * ril_tech,
  const char * ril_profile,
  const char * ril_user,
  const char * ril_pass,
  const char * ril_auth_pref,
  int * umts_profile_id,
  qdp_profile_pdn_type *umts_profile_pdn_type,
  int * cdma_profile_id,
  qdp_profile_pdn_type *cdma_profile_pdn_type,
  RIL_DataCallFailCause * cause,
  boolean * abort_call
);

static int qcril_data_set_nai
(
  int info_tbl_index,
  const char * ril_user,
  const char * ril_pass,
  const char * ril_auth_pref
);

static void qcril_data_partial_retry_hdlr
(
  union sigval sval
);

static void qcrili_data_send_apn_info_qmi(
  const char *apn_name,
  dsd_apn_type_enum_v01 apn_type);

static void qcril_data_handle_set_apn_info_req(
  const char *apn_name,
  int ril_apn_types_mask
  );

/*===========================================================================

  FUNCTION:  qcril_data_create_retry_timer

===========================================================================*/
/*!
    @brief
    Create a partial retry timer for use with Dual-IP calls

    @pre Before calling, info_tbl_mutex must be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
static void qcril_data_create_retry_timer
(
  qcril_data_call_info_tbl_type *info_tbl_ptr
);

static boolean qcrili_data_is_apn_type_found(RIL_ApnTypes apn_type, int apn_types_mask);

/*===========================================================================

                    LOCALS

===========================================================================*/
#if (QCRIL_RIL_VERSION >= 15)
/*===========================================================================

  FUNCTION:  qcril_data_get_ma_ka_elem_by_handle

===========================================================================*/
/*!
    @brief
    Get ptr to an element if it exists in local cache

    @return
    qcril_data_ma_ka_list_elem_t* or NULL
*/
/*=========================================================================*/
qcril_data_ma_ka_list_elem_t *qcril_data_get_ma_ka_elem_by_handle(uint32_t handle)
{
  QCRIL_LOG_DEBUG("%s() handle = %d", __func__, handle);

  qcril_data_ma_ka_list_elem_t *cur = qcril_data_ma_ka_list_head;
  while (cur != NULL) {
    if (cur->handle == handle)
    {
        return cur;
    }
    cur = cur->next;
  }
  return NULL;
}

/*===========================================================================

  FUNCTION:  qcril_data_get_ma_ka_elem_by_cid

===========================================================================*/
/*!
    @brief
    Get ptr to an element if it exists in local cache

    @return
    qcril_data_ma_ka_list_elem_t* or NULL
*/
/*=========================================================================*/
qcril_data_ma_ka_list_elem_t *qcril_data_get_ma_ka_elem_by_cid(int cid)
{
  qcril_data_ma_ka_list_elem_t *cur = qcril_data_ma_ka_list_head;
  while (cur != NULL) {
    if (cur->cid == cid)
    {
        return cur;
    }
    cur = cur->next;
  }
  return NULL;
}

/*===========================================================================

  FUNCTION:  qcril_data_ma_ka_create_element

===========================================================================*/
/*!
    @brief
    Give token and handle, create a new element

    @return
    uint64 mask
*/
/*=========================================================================*/
qcril_data_ma_ka_list_elem_t* qcril_data_ma_ka_create_element(uint32_t handle,
  RIL_Token token)
{
  // FIXME TODO this needs to also copy the other params in for duplicate request
  // detection
  qcril_data_ma_ka_list_elem_t* elem = NULL;
  elem = (qcril_data_ma_ka_list_elem_t*) calloc(sizeof(qcril_data_ma_ka_list_elem_t), 1);
  if (elem != NULL) {
      elem->next = NULL;
      elem->status = QCRIL_DATA_KA_STATUS_NONE;
      elem->handle = handle;
      elem->token = token;
  } else {
    QCRIL_LOG_ERROR("%s(): failed to allocate!", __func__);
  }
  return elem;
}

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
void qcril_data_insert_ma_ka_elem(qcril_data_ma_ka_list_elem_t* elem)
{
  if (elem == NULL) {
    QCRIL_LOG_ERROR("%s(): cannot insert null element");
    return;
  }
  elem->next = qcril_data_ma_ka_list_head;
  qcril_data_ma_ka_list_head = elem;
  QCRIL_LOG_DEBUG("%s(): inserted handle %d", __func__, elem->handle);

  return;
}


/*===========================================================================

  FUNCTION:  qcril_data_ma_ka_delete_elem

===========================================================================*/
/*!
    @brief
    Given token, remove from cache if found

    @return
    void
*/
/*=========================================================================*/
void qcril_data_ma_ka_delete_elem(uint32_t handle)
{
  // Handle empty list
  if (qcril_data_ma_ka_list_head == NULL) {
    QCRIL_LOG_INFO("(%s) List is empty, cannot delete '%d'", __func__, handle);
    return;
  }

  // If only one element in list and it's the one
  // we want then purge list
  if ((qcril_data_ma_ka_list_head->next == NULL) &&
      (qcril_data_ma_ka_list_head->handle == handle)) {
    free(qcril_data_ma_ka_list_head);
    qcril_data_ma_ka_list_head = NULL;
    QCRIL_LOG_DEBUG("Removed '%d', List is now empty", handle);
  }
  // Deleting first element of a list with two or more
  // elements
  else if ((qcril_data_ma_ka_list_head->next != NULL) &&
      (qcril_data_ma_ka_list_head->handle == handle)) {
    qcril_data_ma_ka_list_elem_t * tmp = qcril_data_ma_ka_list_head;
    qcril_data_ma_ka_list_head = qcril_data_ma_ka_list_head->next;
    free(tmp);
  }
  else
  {
    // Deleting from the middle of a list or the end of the list
    qcril_data_ma_ka_list_elem_t *e = qcril_data_ma_ka_list_head;
    qcril_data_ma_ka_list_elem_t *next = e->next;
    while (next != NULL) {
      if (next->handle == handle)
      {
        qcril_data_ma_ka_list_elem_t *tmp = next;
        e->next = next->next;
        QCRIL_LOG_DEBUG("Cleaned up handle '%d'", handle);
        free(tmp);
        break;
      } else
      {
        e = next;
        next = next->next;
      }
    }
  }
}

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
void qcril_data_ma_ka_delete_elems_by_cid(int cid)
{
  if (cid < 0) {
    QCRIL_LOG_ERROR("Call ID cannot be negative");
    return;
  }

  // Handle empty list
  if (qcril_data_ma_ka_list_head == NULL) {
    QCRIL_LOG_INFO("List is empty");
    return;
  }

  // If only one element in list and it's the one
  // we want then purge list
  if ((qcril_data_ma_ka_list_head->next == NULL) &&
      (qcril_data_ma_ka_list_head->cid == cid)) {

    qcril_data_stop_keepalive_for_handle(qcril_data_ma_ka_list_head->handle, true);

    QCRIL_LOG_DEBUG("Removing '%d', List is now empty", qcril_data_ma_ka_list_head->handle);
    free(qcril_data_ma_ka_list_head);
    qcril_data_ma_ka_list_head = NULL;

  }

  qcril_data_ma_ka_list_elem_t * curr = qcril_data_ma_ka_list_head;
  qcril_data_ma_ka_list_elem_t * prev = NULL;

  while (curr != NULL) {
    qcril_data_ma_ka_list_elem_t *next = curr->next;

    // Deleting first element of list
    if ((curr == qcril_data_ma_ka_list_head) &&
        (curr->cid == cid)) {
      // update head to next element
      qcril_data_ma_ka_list_head = qcril_data_ma_ka_list_head->next;

      qcril_data_stop_keepalive_for_handle(curr->handle, true);

      // prev is not affected while we are deleting the first node
      QCRIL_LOG_DEBUG("Deleting first element, handle = %d for cid = %d",
        curr->handle, curr->cid);
      free(curr);
    // Deleting from middle or end of list
    } else if ((curr != qcril_data_ma_ka_list_head) &&
      (curr->cid == cid)) {

      qcril_data_stop_keepalive_for_handle(curr->handle, true);

      QCRIL_LOG_DEBUG("Deleting from middle, handle = %d for cid = %d",
        curr->handle, curr->cid);
      // link previous and next
      prev->next = next;

      // no update to prev here since we delete curr
      free(curr);

    } else if (curr->cid != cid) {
      // Advance prev since curr is not deleted
      prev = curr;
      QCRIL_LOG_DEBUG("Mo match for handle = %d for cid = %d",
        curr->handle, curr->cid);
    }

    // IMPORTANT
    // ---------
    // we always ensure that curr is advanced regardless of
    // whether we deleted a node or not
    curr = next;
  }
}
#endif /* (QCRIL_RIL_VERSION >= 15)  */

static boolean qcrili_data_is_apn_type_found(RIL_ApnTypes apn_type, int apn_types_mask)
{
  if ((apn_type & apn_types_mask) == apn_type) {
    QCRIL_LOG_DEBUG("%s(): apn type %x was enabled", __func__, apn_type);
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

  FUNCTION:   qcril_data_handle_legacy_apn_info_req

===========================================================================*/
/*!
    @brief
    Handle the legacy apn info for supported types: default and ims

    @return
    RIL_E_SUCCESS         :- If QMI DSD returns rc = 0
    RIL_OEM_errors :- If QMI DSD returns rc = negative
*/
/*=========================================================================*/
static void qcril_data_handle_set_apn_info_req(
  const char *apn_name,
  int ril_apn_types_mask
  )
{
  QCRIL_LOG_DEBUG("%s(): apn_name= %s "
   "apn_types= %d",
    __func__,
    apn_name,
    ril_apn_types_mask);

  do {
    if (NULL == apn_name || ril_apn_types_mask <= 0)
    {
      QCRIL_LOG_ERROR("%s", "Invalid inputs");
      break;
    }

    if (qcrili_data_is_apn_type_found(RIL_APN_TYPE_ALL, ril_apn_types_mask))
      {
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_DEFAULT_V01);
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_IMS_V01);
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_MMS_V01);
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_DUN_V01);
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_SUPL_V01);
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_HIPRI_V01);
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_FOTA_V01);
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_CBS_V01);
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_IA_V01);
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_EMERGENCY_V01);
        break;
      }
      if (qcrili_data_is_apn_type_found(RIL_APN_TYPE_DEFAULT, ril_apn_types_mask))
      {
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_DEFAULT_V01);
      }
      if (qcrili_data_is_apn_type_found(RIL_APN_TYPE_IMS, ril_apn_types_mask))
      {
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_IMS_V01);
      }
      if (qcrili_data_is_apn_type_found(RIL_APN_TYPE_MMS, ril_apn_types_mask))
      {
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_MMS_V01);
      }
      if (qcrili_data_is_apn_type_found(RIL_APN_TYPE_DUN, ril_apn_types_mask))
      {
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_DUN_V01);
      }
      if (qcrili_data_is_apn_type_found(RIL_APN_TYPE_SUPL, ril_apn_types_mask))
      {
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_SUPL_V01);
      }
      if (qcrili_data_is_apn_type_found(RIL_APN_TYPE_HIPRI, ril_apn_types_mask))
      {
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_HIPRI_V01);
      }
      if (qcrili_data_is_apn_type_found(RIL_APN_TYPE_FOTA, ril_apn_types_mask))
      {
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_FOTA_V01);
      }
      if (qcrili_data_is_apn_type_found(RIL_APN_TYPE_CBS, ril_apn_types_mask))
      {
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_CBS_V01);
      }
      if (qcrili_data_is_apn_type_found(RIL_APN_TYPE_IA, ril_apn_types_mask))
      {
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_IA_V01);
      }
      if (qcrili_data_is_apn_type_found(RIL_APN_TYPE_EMERGENCY, ril_apn_types_mask))
      {
        qcrili_data_send_apn_info_qmi(apn_name, DSD_APN_TYPE_EMERGENCY_V01);
      }

  } while(0);
}

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
)
{

  QCRIL_LOG_DEBUG("%s","Debug : is_data_enabled ENTRY");

  qmi_client_error_type rc;
  qcril_data_cb_struct *cb_data = NULL;
  qmi_txn_handle txn_handle;
  dsd_notify_data_settings_req_msg_v01  data_settings_req_msg;
  dsd_notify_data_settings_resp_msg_v01 *data_settings_resp_msg = NULL;
  RIL_Errno call_fail_cause =  RIL_E_GENERIC_FAILURE;
  qcril_request_resp_params_type resp;

  if(!params_ptr)
  {
    QCRIL_LOG_ERROR("%s","Bad input parameter");
    return RIL_E_INVALID_ARGUMENTS;
  }
  do
  {
    if(global_qmi_dsd_hndl == NULL)
    {
      QCRIL_LOG_ERROR("%s","DSD Client unavailable");
      call_fail_cause = RIL_E_OEM_ERROR_1;
      break;
    }


    cb_data = (qcril_data_cb_struct *)
              malloc(sizeof(qcril_data_cb_struct));

    data_settings_resp_msg = (dsd_notify_data_settings_resp_msg_v01*)
                             malloc(sizeof(dsd_notify_data_settings_resp_msg_v01));

    if(!data_settings_resp_msg || !cb_data)
    {
      QCRIL_LOG_ERROR("Unable to allocate memory");
      call_fail_cause = RIL_E_NO_MEMORY;
      break;
    }

    memset(cb_data,0,sizeof(qcril_data_cb_struct));
    memset(&data_settings_req_msg, 0, sizeof(data_settings_req_msg));
    memset(data_settings_resp_msg, 0, sizeof(dsd_notify_data_settings_resp_msg_v01));

    data_settings_req_msg.data_service_switch_valid = TRUE;
    data_settings_req_msg.data_service_switch = is_data_enabled;

    QCRIL_LOG_DEBUG("Setting data_enabled =%d subs_id = %d",
                      data_settings_req_msg.data_service_switch,
                      global_subs_id);

    cb_data->t = params_ptr->t;
    cb_data->event_id = params_ptr->event_id;
    cb_data->msg_id = QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01;
    cb_data->self = (void *)cb_data;
    cb_data->calling_func= IS_DATA_ENABLED;
    QCRIL_LOG_DEBUG("Debug : Token [%d] event_id [%d] self [%u]", cb_data->t, params_ptr->event_id, cb_data->self);
    QCRIL_LOG_DEBUG("Debug : calling_func [%d] ", cb_data->calling_func);

    rc = qmi_client_send_msg_async(global_qmi_dsd_hndl,
                                    QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01,
                                    (void *)&data_settings_req_msg,
                                    sizeof(data_settings_req_msg),
                                    (void *)data_settings_resp_msg,
                                    sizeof(dsd_notify_data_settings_resp_msg_v01),
                                    qcril_data_async_cb,
                                    (void *)cb_data,
                                    &txn_handle);

    if (QMI_NO_ERR != rc)
    {
      QCRIL_LOG_ERROR("failed to send QMI message, err=%d",
                      rc);
      call_fail_cause = RIL_E_OEM_ERROR_2;
    } else
    {
      call_fail_cause = RIL_E_SUCCESS;
    }
  } while(0);

  if ( call_fail_cause != RIL_E_SUCCESS )
  {
      QCRIL_DATA_RELEASE_STORAGE(cb_data);
      QCRIL_DATA_RELEASE_STORAGE(data_settings_resp_msg);
      qcril_default_request_resp_params(QCRIL_DEFAULT_INSTANCE_ID,
                                        params_ptr->t,
                                        params_ptr->event_id,
                                        call_fail_cause,
                                          &resp);
      qcril_send_request_response(&resp);
  }
  return call_fail_cause;
}


/*===========================================================================

  FUNCTION:   qcril_data_set_is_data_roaming_enabled

===========================================================================*/
/*!
    @brief
    sends roaming status to modem if user sets it manually via UI

    @return
    RIL_E_SUCCESS         :- If QMI DSD returns rc = 0
    RIL_OEM_errors :- If QMI DSD returns rc = negative
*/
/*=========================================================================*/

RIL_Errno qcril_data_set_is_data_roaming_enabled
(
  const qcril_request_params_type *const params_ptr,
  boolean is_data_roaming_enabled
)
{
  qmi_client_error_type rc;
  qcril_data_cb_struct *cb_data = NULL;
  qmi_txn_handle txn_handle;
  dsd_notify_data_settings_req_msg_v01  data_settings_req_msg;
  dsd_notify_data_settings_resp_msg_v01 *data_settings_resp_msg = NULL;
  RIL_Errno call_fail_cause = RIL_E_GENERIC_FAILURE;
  qcril_request_resp_params_type resp;

  if(!params_ptr)
  {
     QCRIL_LOG_ERROR("%s","Bad input parameter");
     return RIL_E_INVALID_ARGUMENTS;
  }
  do {
    if(global_qmi_dsd_hndl == NULL)
    {
      QCRIL_LOG_ERROR("%s","DSD Client unavailable");
      call_fail_cause = RIL_E_OEM_ERROR_1;
      break;
    }

    cb_data = (qcril_data_cb_struct *)
              malloc(sizeof(qcril_data_cb_struct));

    data_settings_resp_msg = (dsd_notify_data_settings_resp_msg_v01*)
                             malloc(sizeof(dsd_notify_data_settings_resp_msg_v01));

    if(!data_settings_resp_msg || !cb_data)
    {
      QCRIL_LOG_ERROR("Unable to allocate memory");
      call_fail_cause = RIL_E_NO_MEMORY;
      break;
    }

    memset(cb_data,0,sizeof(qcril_data_cb_struct));
    memset(&data_settings_req_msg, 0, sizeof(data_settings_req_msg));
    memset(data_settings_resp_msg, 0, sizeof(dsd_notify_data_settings_resp_msg_v01));

    data_settings_req_msg.data_service_roaming_switch_valid = TRUE;
    data_settings_req_msg.data_service_roaming_switch = is_data_roaming_enabled;

    QCRIL_LOG_DEBUG("Setting data_roaming_enabled =%d,subs_id = %d",
                      data_settings_req_msg.data_service_roaming_switch,
                      global_subs_id );

    cb_data->t = params_ptr->t;
    cb_data->event_id = params_ptr->event_id;
    cb_data->msg_id = QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01;
    cb_data->self = (void *)cb_data;
    cb_data->calling_func = IS_DATA_ROAMING_ENABLED;
    QCRIL_LOG_DEBUG("Debug : Token [%d] event_id [%d] self [%u]", cb_data->t, params_ptr->event_id, cb_data->self);
    QCRIL_LOG_DEBUG("Debug : calling_func [%d] ", cb_data->calling_func);

    rc = qmi_client_send_msg_async(global_qmi_dsd_hndl,
                                    QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01,
                                    (void *)&data_settings_req_msg,
                                    sizeof(data_settings_req_msg),
                                    (void *)data_settings_resp_msg,
                                    sizeof(dsd_notify_data_settings_resp_msg_v01),
                                    qcril_data_async_cb,
                                    (void *)cb_data,
                                    &txn_handle);

    if (QMI_NO_ERR != rc)
    {
      QCRIL_LOG_ERROR("failed to send QMI message, err=%d",
                      rc);
      call_fail_cause = RIL_E_OEM_ERROR_2;
    } else
    {
      call_fail_cause = RIL_E_SUCCESS;
    }
  } while(0);

  if ( call_fail_cause != RIL_E_SUCCESS )
  {
      QCRIL_DATA_RELEASE_STORAGE(cb_data);
      QCRIL_DATA_RELEASE_STORAGE(data_settings_resp_msg);
      qcril_default_request_resp_params(QCRIL_DEFAULT_INSTANCE_ID,
                                        params_ptr->t,
                                        params_ptr->event_id,
                                        call_fail_cause,
                                          &resp);
      qcril_send_request_response(&resp);
  }
  return call_fail_cause;
}

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

RIL_Errno qcril_data_set_apn_info
(
  const qcril_request_params_type *const params_ptr,
  char *type,
  char *name,
  int32 is_apn_valid
)
{
  (void) type;
  (void) name;
  (void) is_apn_valid;

  qcril_request_resp_params_type resp;
  QCRIL_LOG_DEBUG("%s() is unsupported", __func__);
  if(!params_ptr)
  {
     QCRIL_LOG_ERROR("%s","Bad input parameter");
     return RIL_E_INVALID_ARGUMENTS;
  }
  qcril_default_request_resp_params(QCRIL_DEFAULT_INSTANCE_ID,
                                      params_ptr->t,
                                      params_ptr->event_id,
                                      RIL_E_SUCCESS,
                                      &resp);
  qcril_send_request_response(&resp);

  return RIL_E_SUCCESS; // setting to avoid compile time error
}


/*=========================================================================
  FUNCTION:  qcril_data_response_generic_failure

===========================================================================*/
/*!
    @brief
    Remove the entry from the ReqList. Send E_GENERIC_FAILURE response.

    @return
    None
*/
/*=========================================================================*/
void qcril_data_response_generic_failure
(
  qcril_instance_id_e_type instance_id,
  RIL_Token t,
  qcril_evt_e_type request,
  RIL_Errno call_end_cause
)
{
  qcril_request_resp_params_type resp;

  /*-----------------------------------------------------------------------*/

  QCRIL_LOG_ERROR( "%s", "qcril_data_response_generic_failure: ENTRY" );

  /* Remove the entry from ReqList */
  (void) qcril_reqlist_free( instance_id, t );

  /* Send GenericFailure as the response to the RIL command */
  qcril_default_request_resp_params( instance_id, t, request, call_end_cause, &resp );
  qcril_send_request_response( &resp );

  QCRIL_LOG_ERROR( "%s", "qcril_data_response_generic_failure: EXIT" );
} /* qcril_data_response_generic_failure */

#if !(QCRIL_RIL_VERSION >= 6)
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
)
{
  qcril_request_resp_params_type resp;
  char * resp_ptr = NULL;

  QCRIL_LOG_DEBUG("%s", "qcril_data_response_data_call_failure: ENTRY");

  /* Successfully remove the entry from ReqList */
  (void) qcril_reqlist_free(instance_id, t);

  /* Send FAILURE as the response to the RIL command */
  qcril_default_request_resp_params(instance_id,
                                    t,
                                    request,
                                    RIL_E_SETUP_DATA_CALL_FAILURE,
                                    &resp);

  /* Send error code response as string value */
  asprintf( &resp_ptr, "%d", response );
  resp.resp_pkt = &resp_ptr;
  resp.resp_len = sizeof(resp_ptr);

  qcril_send_request_response(&resp);
  QCRIL_DATA_RELEASE_STORAGE( resp_ptr );

  QCRIL_LOG_DEBUG("%s", "qcril_data_response_data_call_failure: EXIT");
} /* qcril_data_response_data_call_failure */
#endif /* !(QCRIL_RIL_VERSION >= 6) */


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
  qcril_data_call_info_tbl_type * info_tbl_ptr,
  qcril_instance_id_e_type instance_id,
  RIL_Token t,
  RIL_DataCallFailCause response
)
{
  (void)info_tbl_ptr;
  (void)response;

  /* Invoke the proper error response based on RIL version */
#if (QCRIL_RIL_VERSION >= 6)
  qcril_data_call_response_type  resp;

  qcril_data_util_update_call_state( info_tbl_ptr, CALL_INACTIVE, response );
  qcril_data_util_create_setup_rsp( info_tbl_ptr, &resp );
  qcril_data_response_success( instance_id,
                               t,
                               QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL,
                               (void *) ( &(resp.setup_rsp) ),
                               sizeof( resp.setup_rsp ) );

#elif (QCRIL_RIL_VERSION >= 4)
  qcril_data_response_data_call_failure( instance_id,
                                         t,
                                         QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL,
                                         response );
#else
  qcril_data_response_generic_failure( instance_id,
                                       t,
                                       QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL,
                                       RIL_E_INTERNAL_ERR);
#endif

} /* qcril_data_response_setup_data_call_failure */



/*=========================================================================
  FUNCTION:  qcril_data_response_setup_data_call_failure_generic_err

===========================================================================*/
/*!
    @brief
    Generate setup data call generic error response based on RIL API version.

    @return
    None
*/
/*=========================================================================*/
static void qcril_data_response_setup_data_call_failure_generic_err
(
  qcril_instance_id_e_type  instance_id,
  RIL_Token                 t
)
{
#if (QCRIL_RIL_VERSION >= 6)

#if (QCRIL_RIL_VERSION >= 11)

  /* Response to setup data call request */
  RIL_Data_Call_Response_v11  setup_rsp;

#elif (QCRIL_RIL_VERSION >= 10)

  /* Response to setup data call request */
  RIL_Data_Call_Response_v9  setup_rsp;

#else

  /* Response to setup data call request */
  RIL_Data_Call_Response_v6  setup_rsp;

#endif /* (QCRIL_RIL_VERSION >= 11)  */

  qcril_request_resp_params_type resp;

  memset( &setup_rsp, 0x0, sizeof(setup_rsp) );
  setup_rsp.status = PDP_FAIL_OEM_DCFAILCAUSE_10;
  setup_rsp.suggestedRetryTime = QCRIL_DATA_SUGGESTEDRETRYTIME;
  setup_rsp.cid = -1;

  qcril_default_request_resp_params( instance_id,
                                     t,
                                     QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL,
                                     RIL_E_SUCCESS,
                                     &resp );
  resp.resp_pkt = (void *)&(setup_rsp);
  resp.resp_len = sizeof(setup_rsp);
  qcril_send_request_response( &resp );

#elif (QCRIL_RIL_VERSION >= 4)
   qcril_data_response_data_call_failure( instance_id,
                                          t,
                                          QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL,
                                          PDP_FAIL_OEM_DCFAILCAUSE_10 );
#else
   qcril_data_response_generic_failure( instance_id,
                                        t,
                                        QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL,
                                        RIL_E_INTERNAL_ERR);
#endif
} /* qcril_data_response_setup_data_call_failure_generic_err */



/*=========================================================================
  FUNCTION:  qcril_data_response_success

===========================================================================*/
/*!
    @brief
    Remove the entry from the ReqList. Send RIL_E_SUCCESS response.

    @return
    None
*/
/*=========================================================================*/
void qcril_data_response_success
(
  qcril_instance_id_e_type instance_id,
  RIL_Token t,
  qcril_evt_e_type request,
  void *response,
  size_t response_len
)
{
  qcril_request_resp_params_type resp;

  /*-----------------------------------------------------------------------*/

  QCRIL_LOG_DEBUG( "%s", "qcril_data_response_success: ENTRY" );

  /* Successfully remove the entry from ReqList */
  (void) qcril_reqlist_free( instance_id, t );

  /* Send SUCCESS as the response to the RIL command */
  qcril_default_request_resp_params( instance_id, t, request, RIL_E_SUCCESS, &resp );
  resp.resp_pkt = response;
  resp.resp_len = response_len;
  qcril_send_request_response( &resp );

  QCRIL_LOG_DEBUG( "%s", "qcril_data_response_success: EXIT" );
} /* qcril_data_response_success */


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
)
{
  qcril_unsol_resp_params_type unsol_resp;
#if (QCRIL_RIL_VERSION >= 6)

#if (QCRIL_RIL_VERSION >= 11)

  RIL_Data_Call_Response_v11 *active_call_table = NULL;

#elif (QCRIL_RIL_VERSION >= 10)

  RIL_Data_Call_Response_v9 *active_call_table = NULL;

#else

  RIL_Data_Call_Response_v6 *active_call_table = NULL;

#endif /* (QCRIL_RIL_VERSION >= 11)  */

#else
  RIL_Data_Call_Response    *active_call_table = NULL;
#endif /* (QCRIL_RIL_VERSION >= 6) */

  size_t response_len;
  /*-----------------------------------------------------------------------*/

  QCRIL_LOG_DEBUG( "%s", "qcril_data_unsol_call_list_changed: ENTRY" );

  QCRIL_LOG_DEBUG( "%s", "sending RIL_UNSOL_DATA_CALL_LIST_CHANGED" );
  qcril_data_get_active_call_list((void**)&active_call_table, &response_len);

  qcril_default_unsol_resp_params( instance_id, (int) RIL_UNSOL_DATA_CALL_LIST_CHANGED, &unsol_resp );
  unsol_resp.resp_pkt = active_call_table;
  unsol_resp.resp_len = response_len;
  qcril_send_unsol_response( &unsol_resp );

  /* free memeory */
  if(NULL != active_call_table)
  {
    QCRIL_LOG_DEBUG("%s", "qcril_data_unsol_call_list_changed: free memory");
    qcril_free(active_call_table);
    active_call_table = NULL;
  }
  QCRIL_LOG_DEBUG( "%s", "qcril_data_unsol_call_list_changed: EXIT with succ" );
  return;

}


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
)
{
  qcril_unsol_resp_params_type unsol_resp;

  /*-----------------------------------------------------------------------*/

  QCRIL_LOG_DEBUG( "%s", "qcril_data_unsol_tethered_mode_state_changed: ENTRY" );

  QCRIL_LOG_DEBUG( "%s", "sending RIL_UNSOL_RESPONSE_TETHERED_MODE_STATE_CHANGED" );

  qcril_default_unsol_resp_params( instance_id, (int) RIL_UNSOL_RESPONSE_TETHERED_MODE_STATE_CHANGED, &unsol_resp );
  unsol_resp.resp_pkt = ( void * ) response;
  unsol_resp.resp_len = response_len;
  qcril_send_unsol_response( &unsol_resp );

  QCRIL_LOG_DEBUG( "%s", "qcril_data_unsol_tethered_mode_state_changed: EXIT with succ" );
  return;

}


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
)
{
  qcril_instance_id_e_type instance_id = global_instance_id;
  qcril_data_go_dormant_params_type dev_name;
  int dev_instance = QCRIL_DATA_INVALID_DEV;
  int i, ret_val;
  int rmnet_physlink_down[QCRIL_DATA_MAX_DEVS];
  qcril_data_iface_ioctl_type ioctl = QCRIL_DATA_IFACE_IOCTL_GO_DORMANT;
  RIL_Errno call_end_cause = RIL_E_INTERNAL_ERR;

  boolean dorm_status_changed = FALSE;
  int call_state;

  QCRIL_LOG_DEBUG( "%s","Entered: qcril_data_process_qcrilhook_go_dormant");
  QCRIL_LOG_DEBUG( " request = %d", params_ptr->event_id);
  QCRIL_LOG_DEBUG( " Data Length = %d", params_ptr->datalen);
  QCRIL_LOG_DEBUG( " token = %d", params_ptr->t);

  QCRIL_DS_ASSERT( params_ptr != NULL, "validate params_ptr" );
  QCRIL_DS_ASSERT( ret_ptr    != NULL, "validate ret_ptr" );

  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

  if ( ( params_ptr == NULL ) || ( ret_ptr == NULL ) )
  {
    QCRIL_LOG_ERROR( "BAD input, params_ptr [%p], ret_ptr [%p]",
                     (unsigned int *)params_ptr, (unsigned int *)ret_ptr );
    call_end_cause = RIL_E_INVALID_ARGUMENTS;
    goto err_label;
  }

  instance_id = params_ptr->instance_id;
  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );
  if (instance_id >= QCRIL_MAX_INSTANCE_ID)
  {
    call_end_cause = RIL_E_INVALID_ARGUMENTS;
    goto err_label;
  }

  for(i=0; i<QCRIL_DATA_MAX_DEVS; i++)
  {
    rmnet_physlink_down[i] = FALSE;
  }

  if (params_ptr->datalen == 0)
  {
    /*Issue Go Dormant on all active interfaces*/
    QCRIL_LOG_DEBUG( "%s","RIL provided NULL dev name will issue Dormancy on all active interfaces");

    for( i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++ )
    {
      /* TODO: find out why this check needs to be removed. CR 238473 */
      /* (call_tbl[ info_tbl[i].index ].active  == CALL_ACTIVE_PHYSLINK_UP) &&*/
      GET_DEV_INSTANCE_FROM_NAME(i, dev_instance);
      if (dev_instance < 0 || dev_instance >= QCRIL_DATA_MAX_DEVS)
      {
        QCRIL_LOG_ERROR("invalid dev_instance [%d] derived for index [%d]",
                        dev_instance, i);
        continue;
      }
      if(VALIDATE_LOCAL_DATA_OBJ(&info_tbl[i]) &&
         rmnet_physlink_down[dev_instance] == FALSE)
      {
        QCRIL_LOG_DEBUG( "selected index = %d",i);
        if ((ret_val = qcril_data_iface_ioctl(&info_tbl[i],
                                              ioctl,
                                              &dorm_status_changed,
                                              &call_state))
            == FAILURE)
        {
          QCRIL_LOG_ERROR( "%s","Request to issue Dormancy failed.");
          call_end_cause = RIL_E_OEM_ERROR_3;
          goto err_label;
        }
        rmnet_physlink_down[dev_instance] = TRUE;
      }
      else
      {
        QCRIL_LOG_ERROR("Can not issue Dormancy for index [%d]"     \
                        "rmnet_phylink_down [%d]",
                        i, rmnet_physlink_down[dev_instance]);
      }
    }/* for() */
  }
  else if (params_ptr->datalen > 0 && params_ptr->datalen <= QCRIL_MAX_DEV_NAME_SIZE)
  {
    /* prepare dev_name buffer */
    memset(&dev_name, 0, sizeof(dev_name));
    /* copy bytes received from RIL */
    memcpy(&dev_name, params_ptr->data,QCRIL_MAX_DEV_NAME_SIZE);
    /* put a null char at the end as required by strncmp function later */
    dev_name[params_ptr->datalen] = '\0';

    for( i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++ )
    {
      GET_DEV_INSTANCE_FROM_NAME(i, dev_instance);
      if (dev_instance < 0 || dev_instance >= QCRIL_DATA_MAX_DEVS)
      {
        QCRIL_LOG_ERROR("invalid dev_instance [%d] derived for index [%d]",
                        dev_instance, i);
        continue;
      }

      /*search info table for the specified interface*/
      if (VALIDATE_LOCAL_DATA_OBJ((&info_tbl[i])) &&
          (!strncmp(info_tbl[i].call_info.dev_name,dev_name,QCRIL_MAX_DEV_NAME_SIZE)) &&
          rmnet_physlink_down[dev_instance] == FALSE)
      {
        QCRIL_LOG_DEBUG( "selected index = %d",i);
        if ((ret_val = qcril_data_iface_ioctl(&info_tbl[i],
                                              ioctl,
                                              &dorm_status_changed,
                                              &call_state))
            == FAILURE)
        {
          QCRIL_LOG_ERROR( "%s","Request to issue Dormancy failed.");
          call_end_cause = RIL_E_OEM_ERROR_3;
          goto err_label;
        }
        rmnet_physlink_down[dev_instance] = TRUE;
      }
      else
      {
        QCRIL_LOG_ERROR("Can not issue Dormancy for index [%d] with dev_name: %s"     \
                        "rmnet_phylink_down [%d]",
                        i, dev_name,rmnet_physlink_down[dev_instance]);
      }
    } /*for*/
  } /*else*/
  else
  {
    QCRIL_LOG_ERROR( "%s","qcril_data_process_qcrilhook_go_dormant: Bad input received");
    call_end_cause = RIL_E_INVALID_ARGUMENTS;
    goto err_label;
  }

  QCRIL_LOG_INFO( "%s","qcril_data_process_qcrilhook_go_dormant: EXIT with SUCCESS");

  qcril_data_response_success( instance_id,
                               params_ptr->t,
                               params_ptr->event_id,
                               NULL,
                               0 );
  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  return;

err_label:
  //post generic failure resp
  QCRIL_LOG_INFO( "%s","qcril_data_process_qcrilhook_go_dormant: EXIT with ERROR");
  if (NULL != params_ptr)
  {
    qcril_data_response_generic_failure( instance_id,
                                         params_ptr->t,
                                         params_ptr->event_id,
                                         call_end_cause);
  }
  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  return;
}

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
)
{
  QCRIL_LOG_INFO("screen state changed [%d] no longer supported", screen_state);
  return QCRIL_DS_SUCCESS;

} /* qcril_data_process_screen_state_change() */

#ifdef RIL_REQUEST_SET_INITIAL_ATTACH_APN
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
)
{
  /* If there is a parameter match for the given profile_id then there's no need
     for an attach */
  if (QDP_SUCCESS == qdp_match_lte_attach_profile_params(profile_id,
                                                         param_strings))
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*===========================================================================

  FUNCTION: qcril_datai_request_set_lte_attach_profile

===========================================================================*/
/*!
  @brief
  Internal function handles RIL_REQUEST_SET_INITIAL_ATTACH_APN.

  @return
  None.

*/
/*===========================================================================*/
static RIL_Errno qcril_datai_request_set_lte_attach_profile
(
#if (QCRIL_RIL_VERSION >= 15)
  RIL_InitialAttachApn_v15* attachInfo
#else
  RIL_InitialAttachApn* attachInfo
#endif
)
{
  unsigned int                      i;
  int                               rc;
  const char                        *ril_apn;
  const char                        *ril_user;
  const char                        *ril_pass;
  int                               auth_type;
  const char                        *ril_auth_type = NULL;
  const char                        *ril_ipfamily = NULL;
#if (QCRIL_RIL_VERSION >= 15)
  const char                        *ril_ip_roaming = NULL;
  int                               ril_apn_types = 0;
#endif
  const char                        *qdp_params[QDP_RIL_PARAM_MAX];
  unsigned int                      lte_profile_id = QDP_INVALID_PROFILE;
  unsigned int                      input_profile_id = QDP_INVALID_PROFILE;
  qdp_profile_pdn_type              qdp_lte_profile_pdn_type = QDP_PROFILE_PDN_TYPE_INVALID;
  qdp_error_info_t                  error_info;

  unsigned int                      max_attach_pdns;
  int                               error_code;
  wds_get_lte_attach_pdn_list_resp_msg_v01 prev_pdn_list;

  /* Profile information stored locally. In case
   * lte attach fails this information will be
   * used to restore the profile on the modem */
  qdp_profile_info_type             prof_params;
  int                               reti = QCRIL_LTE_ATTACH_SUCCESS;
  char                              args[PROP_VALUE_MAX];
  int                               ril_user_len =0;
  int                               ril_pass_len =0;
  QCRIL_LOG_VERBOSE( "%s", "qcril_data_request_set_lte_attach_profile: ENTRY" );

  if ( attachInfo == NULL )
  {
    QCRIL_LOG_ERROR( "%s", "BAD input, attachInfo");
    return QCRIL_LTE_ATTACH_FAILURE;
  }

  /* Get the data */
  ril_apn       = attachInfo->apn;
  ril_user      = attachInfo->username;
  ril_pass      = attachInfo->password;
  auth_type     = attachInfo->authtype;
  ril_ipfamily  = attachInfo->protocol;
#if (QCRIL_RIL_VERSION >= 15)
  ril_ip_roaming = attachInfo->roamingProtocol;
  ril_apn_types = attachInfo->supportedTypesBitmask;
#endif

  do
  {
    if (NULL == ril_apn)
    {
      /* TODO: Case when provided APN is NULL */
      QCRIL_LOG_ERROR("%s","ril_apn (provided NULL) is mandatory param for "
                      "looking up profile.");
      reti = QCRIL_LTE_ATTACH_FAILURE;
      break;
    }

    memset(qdp_params, 0, sizeof(qdp_params));

    /* build up QDP parameters */
    /* prepare APN */
    if (strlen(ril_apn) > (QMI_WDS_APN_NAME_MAX_V01))
    {
      QCRIL_LOG_ERROR("RIL provided invalid APN [%s] "
                      "APN length [%d] exceeds max allowed [%d]",
                      ril_apn, strlen(ril_apn), QMI_WDS_APN_NAME_MAX_V01);
      reti = QCRIL_LTE_ATTACH_FAILURE;
      break;
    }
    qdp_params[QDP_RIL_APN] = ril_apn;

    QCRIL_LOG_DEBUG("qdp_param APN = [%s]", qdp_params[QDP_RIL_APN]);

    /* prepare tech */
    qdp_params[QDP_RIL_TECH] = QDP_RIL_3GPP;

    /* prepare ip family */
    if (ril_ipfamily != NULL && strlen(ril_ipfamily) > 0)
    {
      if (strlen(ril_ipfamily) > QCRIL_IP_FAMILY_STR_MAX)
      {
        QCRIL_LOG_ERROR("RIL provided invalid ip family [%s] "
                        "ip family length [%d] exceeds max allowed [%d]",
                        ril_ipfamily, strlen(ril_ipfamily), QCRIL_IP_FAMILY_STR_MAX);
        reti = QCRIL_LTE_ATTACH_FAILURE;
        break;
      }
      qdp_params[QDP_RIL_IP_FAMILY] = ril_ipfamily;

      QCRIL_LOG_DEBUG("qdp param IP_FAMILY = [%s]", qdp_params[QDP_RIL_IP_FAMILY]);
    }

#if (QCRIL_RIL_VERSION >= 15)
    /* prepare roaming ip */
    if (ril_ip_roaming != NULL && strlen(ril_ip_roaming) > 0)
    {
      if (strlen(ril_ip_roaming) > QCRIL_IP_FAMILY_STR_MAX)
      {
        QCRIL_LOG_ERROR("RIL provided invalid roaming ip [%s] "
                        "ip roaming length [%d] exceeds max allowed [%d]",
                        ril_ip_roaming, strlen(ril_ip_roaming), QCRIL_IP_FAMILY_STR_MAX);
        reti = QCRIL_LTE_ATTACH_FAILURE;
        break;
      }
      qdp_params[QDP_RIL_IP_ROAMING] = ril_ip_roaming;

      QCRIL_LOG_DEBUG("qdp param IP_ROAMING = [%s]", qdp_params[QDP_RIL_IP_ROAMING]);
    }
#endif /* QCRIL_RIL_VERSION >= 15 */

    /* prepare auth */
    switch (auth_type)
    {
    case WDS_PROFILE_AUTH_PROTOCOL_NONE_V01:
        ril_auth_type = QDP_RIL_PAP_CHAP_NOT_ALLOWED;
        break;
    case WDS_PROFILE_AUTH_PROTOCOL_PAP_V01:
        ril_auth_type = QDP_RIL_PAP_ONLY_ALLOWED;
        break;
    case WDS_PROFILE_AUTH_PROTOCOL_CHAP_V01:
        ril_auth_type = QDP_RIL_CHAP_ONLY_ALLOWED;
        break;
    case WDS_PROFILE_AUTH_PROTOCOL_PAP_CHAP_V01:
        ril_auth_type = QDP_RIL_PAP_CHAP_BOTH_ALLOWED;
        break;
    default:
        QCRIL_LOG_DEBUG("Unknown auth_type [%d]",
                        auth_type);
        ril_auth_type = QDP_RIL_PAP_CHAP_NOT_ALLOWED;
        break;
    }

    qdp_params[QDP_RIL_AUTH] = ril_auth_type;
    QCRIL_LOG_DEBUG("qdp param RIL_AUTH = [%s]", qdp_params[QDP_RIL_AUTH]);

    /* Prepare username */
    if (ril_user != NULL && strlen(ril_user) > 0)
    {
      if (strlen(ril_user) > QMI_WDS_USER_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("RIL provided username exceeds max allowed [%d]",
                        QMI_WDS_USER_NAME_MAX_V01);
      }
      else
      {
        qdp_params[QDP_RIL_NAI] = ril_user;
        ril_user_len = strlen(ril_user)+1;
        QCRIL_LOG_DEBUG("qdp param USERNAME = [%s]", qdp_params[QDP_RIL_NAI]);
      }
    }

    /* prepare password */
    if (ril_pass != NULL && strlen(ril_pass) > 0)
    {
      if (strlen(ril_pass) > QMI_WDS_PASSWORD_MAX_V01)
      {
        QCRIL_LOG_ERROR("RIL provided password exceeds max allowed [%d]",
                        QMI_WDS_PASSWORD_MAX_V01);
      }
      else
      {
        qdp_params[QDP_RIL_PASSWORD] = ril_pass;
        ril_pass_len = strlen(ril_pass)+1;
        QCRIL_LOG_DEBUG("qdp param PASSWORD = [%s]", qdp_params[QDP_RIL_PASSWORD]);
      }
    }

    memset( &error_info, 0x0, sizeof(error_info) );
    memset( &prof_params, 0,  sizeof(prof_params) );

    if (E_SUCCESS == qcril_data_qmi_wds_get_lte_max_attach_pdn_num(global_qcril_qmi_wds_hndl,
                                                                   &max_attach_pdns))
    {
      /* Query existing attach profiles on modem */
      memset(&prev_pdn_list, 0, sizeof(prev_pdn_list));
      if(E_SUCCESS != qcril_data_qmi_wds_get_lte_attach_pdn_list(global_qcril_qmi_wds_hndl,
                                                                 &prev_pdn_list))
      {
        QCRIL_LOG_ERROR("%s", "qcril_data_qmi_wds_get_lte_attach_pdn_list failed");
        reti = QCRIL_LTE_ATTACH_FAILURE;
      }
      else
      {
        /* Check all profiles in the attach pdn list and compare with incoming APN
         * If the profile is present, send RIL_E_SKIP_LTE_REATTACH. If not, proceed
         * with profile lookup */
        for (i=0; i<prev_pdn_list.attach_pdn_list_len; i++)
        {
          if ( FALSE == qcril_data_is_attach_required(prev_pdn_list.attach_pdn_list[i],
                                                      qdp_params))
          {
            /* The profile already exists on modem and there are no changes to it,
             * skip reattach */
            QCRIL_LOG_DEBUG("%s", "Reattach not required!");
            reti = QCRIL_LTE_SKIP_ATTACH;
            break;
          }
        }
      }

      // Skip further lookup and attach list update if
      // APN already attached
      if (reti == QCRIL_LTE_SKIP_ATTACH) {
        QCRIL_LOG_DEBUG("Skipping PDN list update since already attached");
        break;
      }

      if (prev_pdn_list.attach_pdn_list_len != 0)
      {
         input_profile_id = prev_pdn_list.attach_pdn_list[0];
      }

      /* Lookup profile using QDP */
      rc = qdp_lte_attach_profile_lookup( qdp_params,
                                          &lte_profile_id,
                                          &qdp_lte_profile_pdn_type,
                                          &input_profile_id,
                                          &prof_params,
                                          &error_info);

      if( QDP_SUCCESS != rc )
      {
        QCRIL_LOG_ERROR("Not able to look 3gpp profile id [%d], "
                        "returned by QDP, lookup error[%d] tech[%d]",
                        lte_profile_id, error_info.error, error_info.tech);
        reti = QCRIL_LTE_ATTACH_FAILURE;
        break;
      }

      if (lte_profile_id != QDP_INVALID_PROFILE)
      {
        wds_set_lte_attach_pdn_list_req_msg_v01 new_pdn_list;
        memset(&new_pdn_list,0,sizeof(wds_set_lte_attach_pdn_list_req_msg_v01));

        new_pdn_list.attach_pdn_list_len = prev_pdn_list.attach_pdn_list_len;

        if (0 == new_pdn_list.attach_pdn_list_len )
        {
          new_pdn_list.attach_pdn_list_len = 1;
        }
        for(i=0; i<new_pdn_list.attach_pdn_list_len; i++)
        {
          new_pdn_list.attach_pdn_list[i] = prev_pdn_list.attach_pdn_list[i];
        }

        new_pdn_list.attach_pdn_list[0] = lte_profile_id;
        if(E_SUCCESS != qcril_data_qmi_wds_set_lte_attach_pdn_list(global_qcril_qmi_wds_hndl,
                                                                   &new_pdn_list))
        {
          QCRIL_LOG_ERROR("LTE attach PDN for profile [%d] failed",
                          lte_profile_id);

          /* Reset profile on the modem */
          rc = qdp_modify_profile(&prof_params, &error_code);

          if (QDP_SUCCESS != rc)
          {
            QCRIL_LOG_ERROR("Unable to restore profile on modem for profile id [%d]",
                            lte_profile_id);
          }

          /* Release the profile */
          rc = qdp_profile_release(lte_profile_id);
          if (QDP_SUCCESS != rc)
          {
            QCRIL_LOG_ERROR("%s", "Profile could either be modem profile or there "
                            "was an error during profile release");
          }

          /* Attach failed */
          reti = QCRIL_LTE_ATTACH_FAILURE;
          break;
        }

        /* If this is an attach request with a new APN, we would need to release
         * the old profile */
        if (QDP_INVALID_PROFILE != global_lte_attach_profile
            && global_lte_attach_profile != lte_profile_id)
        {
          QCRIL_LOG_DEBUG("[%d] global_lte_attach_profile   [%d] lte_profile_id" ,
                            global_lte_attach_profile,lte_profile_id);
        }

        /* Save the profile in the global LTE profile variable */
        global_lte_attach_profile = lte_profile_id;
      }
    }
    else
    {
      /* Fallback mechanism */
      memset( args, 0x0, sizeof(args) );
      rc = property_get( QCRIL_PROPERTY_DEFAULT_PROFILE,
                         args,
                         QCRIL_LTE_DEFAULT_PROFILE_VALUE);

      if ( QCRIL_PROPERTY_DEFAULT_PROFILE_SIZE < rc )
      {
        QCRIL_LOG_ERROR("Modem does not support this feature!");
        return RIL_E_REQUEST_NOT_SUPPORTED;
      }

      rc = ds_atoi(args);

      if ( rc <= 0 )
      {
        QCRIL_LOG_ERROR("Modem does not support this feature!");
        return RIL_E_REQUEST_NOT_SUPPORTED;
      }

      /* Set the default profile to the property value */
      lte_profile_id = rc;

      /* Check if LTE attach is required */
      if ( FALSE == qcril_data_is_attach_required( lte_profile_id,
                                                   qdp_params ) )
      {
        /* Attach is not required since no paramaeters changed */
        QCRIL_LOG_DEBUG("%s", "Reattach not required!");
        return RIL_E_SKIP_LTE_REATTACH;
      }

      rc = qdp_lte_attach_profile_lookup( qdp_params,
                                          &lte_profile_id,
                                          &qdp_lte_profile_pdn_type,
                                          &input_profile_id,
                                          &prof_params,
                                          &error_info);

      if( QDP_SUCCESS != rc )
      {
        QCRIL_LOG_ERROR("Not able to look 3gpp profile id [%d], "
                        "returned by QDP, lookup error[%d] tech[%d]",
                        lte_profile_id, error_info.error, error_info.tech);
        reti = QCRIL_LTE_ATTACH_FAILURE;
        break;
      }

      /* Save the profile in the global LTE profile variable. In case of
       * the fallback case for LTE attach, even if am attach is triggered
       * with a new APN, we would still be updating the default profile id
       * on the modem. So there is no need to free the profile on the modem */
      global_lte_attach_profile = lte_profile_id;

    }
  } while (0);


#if (QCRIL_RIL_VERSION >= 15)
  /* send apn type info to modem after all profile creation and update is done*/
  if ((reti != QCRIL_LTE_ATTACH_FAILURE) && (ril_apn != NULL))
  {
    QCRIL_LOG_DEBUG("qcril_data_handle_set_apn_info_req on attach");
    qcril_data_handle_set_apn_info_req(ril_apn, ril_apn_types);
  }
#endif

  if (QCRIL_LTE_SKIP_ATTACH == reti) {
   return RIL_E_SKIP_LTE_REATTACH;
  }
  else if (QCRIL_LTE_ATTACH_SUCCESS != reti)
  {
    QCRIL_LOG_ERROR( "%s", "EXIT with FAILURE" );
    return RIL_E_OEM_ERROR_3;
  }
  else
  {
    QCRIL_LOG_DEBUG( "%s", "EXIT with SUCCESS" );
    return RIL_E_SUCCESS;
  }

} /* qcril_datai_request_set_lte_attach_profile */

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
)
{
  return qcril_datai_request_set_lte_attach_profile(attachInfo);
}
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
)
{
  return qcril_datai_request_set_lte_attach_profile(attachInfo);
}
#endif /* QCRIL_RIL_VERSION >= 15 */

#endif /* RIL_REQUEST_SET_INITIAL_ATTACH_APN */



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
)
{
  qcril_instance_id_e_type  instance_id;
  qcril_modem_id_e_type     modem_id;
  int                       i, apn_len;
  dsi_call_param_value_t    apn_info;
  dsi_call_param_value_t    ipfamily_info;
  dsi_call_param_value_t    techpref_info;
  dsi_call_param_value_t    disallow_roaming_info;
  const char               *ril_apn;
  const char               *ril_user;
  const char               *ril_pass;
  const char               *ril_auth_pref = NULL;
  const char               *ril_tech = NULL;
  const char               *ril_common_tech = NULL;
  const char               *ril_profile = NULL;
  const char               *ril_ipfamily = NULL;
  const char               *ril_ip_roaming = NULL;
  const char               *ril_apn_types = NULL;
  const char               *ril_roaming_allowed = NULL;
  char                      tmp_apn[ DS_CALL_INFO_APN_MAX_LEN + 1 ];
  qcril_reqlist_u_type      u_info;
  qcril_reqlist_public_type reqlist_entry;
  int                       umts_profile_id = 0;
  int                       cdma_profile_id = 0;
  int                       call_end_reason = PDP_FAIL_OEM_DCFAILCAUSE_4;
  boolean abort_call = FALSE;
  qcril_data_ril_radio_tech_t  numeric_ril_tech = QCRIL_DATA_RIL_RADIO_TECH_UNKNOWN;

  QCRIL_LOG_VERBOSE( "%s", "qcril_data_request_setup_data_call: ENTRY" );

  QCRIL_DS_ASSERT( params_ptr != NULL, "validate params_ptr" );
  QCRIL_DS_ASSERT( ret_ptr    != NULL, "validate ret_ptr" );
  QCRIL_DS_ASSERT ( ( params_ptr->datalen % 4 ) == 0, "validate datalen" );

  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

  if ( ( params_ptr == NULL ) || ( ret_ptr == NULL ) || ( ( params_ptr->datalen % 4 ) != 0) )
  {
    QCRIL_LOG_ERROR( "BAD input, params_ptr [%p], ret_ptr [%p], datalen [%d]",
                     (unsigned int *)params_ptr, (unsigned int *)ret_ptr,
                     params_ptr ? params_ptr->datalen : 0 );
    goto err_bad_input;
  }

  instance_id = params_ptr->instance_id;
  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );
  if (instance_id >= QCRIL_MAX_INSTANCE_ID)
  {
    goto err_bad_input;
  }

  modem_id = params_ptr->modem_id;
  QCRIL_ASSERT( modem_id < QCRIL_MAX_MODEM_ID );
  if (modem_id >= QCRIL_MAX_MODEM_ID)
  {
    goto err_bad_input;
  }

  {
    qcril_reqlist_public_type req;

    /* Block SETUP_DATA_CALL request if there is a
     * DEACTIVATE_DATA_CALL request in progress. */
    if( E_SUCCESS == qcril_reqlist_query_by_request( instance_id,
                                                     QCRIL_EVT_RIL_REQUEST_DEACTIVATE_DATA_CALL,
                                                     &req ) )
    {
      /* Found pending deactivate request, generate error response */
      QCRIL_LOG_ERROR("%s", "Blocking SETUP_DATA_CALL due to pending DEACTIVATE");
      qcril_data_response_setup_data_call_failure_generic_err(instance_id,
                                                             params_ptr->t);
      goto err_bad_input;
    }
  }

  memset( &u_info,   0, sizeof( qcril_reqlist_u_type ) );
  memset( tmp_apn,   0, DS_CALL_INFO_APN_MAX_LEN + 1 );
  memset( &apn_info, 0, sizeof( dsi_call_param_value_t ) );

  ril_apn  = ((const char **)params_ptr->data)[2];
  ril_user = ((const char **)params_ptr->data)[3];
  ril_pass = ((const char **)params_ptr->data)[4];
  ril_tech = ((const char **)params_ptr->data)[0];
  ril_profile = ((const char **)params_ptr->data)[1];
  ril_auth_pref = ((const char **)params_ptr->data)[5];
  ril_ipfamily = ((const char **)params_ptr->data)[6];
#if (QCRIL_RIL_VERSION >= 15)
  ril_ip_roaming = ((const char **)params_ptr->data)[7];
  ril_apn_types = ((const char **)params_ptr->data)[8];
  ril_roaming_allowed = ((const char **)params_ptr->data)[14];
#endif

  if (ril_tech != NULL)
  {
    QCRIL_LOG_DEBUG ("RIL provided tech pref [%s]", ril_tech);

#if (QCRIL_RIL_VERSION >= 6)
    numeric_ril_tech = qcril_data_get_numeric_ril_technology(ril_tech);
    ril_common_tech = QCRIL_DATA_IS_RIL_RADIO_TECH_CDMA(numeric_ril_tech) ?
                      QCRIL_CDMA_STRING :
                      QCRIL_UMTS_STRING;

    if(QCRIL_DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(numeric_ril_tech))
    {
      if( ril_apn != NULL )
      {
        ril_apn = NULL;
        QCRIL_LOG_DEBUG ("Disregarding RIL APN for CDMA related tech [%d]", ril_tech);
      }
    }

#else
    ril_common_tech = ril_tech;
#endif
    QCRIL_LOG_DEBUG ("RIL - Common tech [%s]", ril_common_tech);
  }
  else
  {
    QCRIL_LOG_DEBUG ("%s", "RIL did not provide tech pref");
  }

  if ( (ril_user != NULL) && (ril_pass != NULL) )
  {
    QCRIL_LOG_DEBUG( "RIL provided USERNAME len [%d], PASSWORD len [%d]",
                      strlen( ril_user), strlen( ril_pass ) );
  }

  if ( ril_apn != NULL )
  {
    /* APN len calculations */
    apn_len = MINIMUM( strlen( ril_apn ), DS_CALL_INFO_APN_MAX_LEN );
    memcpy( tmp_apn, ril_apn, apn_len );
    tmp_apn[ apn_len ] = '\0';
    QCRIL_LOG_DEBUG( "RIL APN [%s]", ril_apn );
  }
  else
  {
    tmp_apn[0] = '\0';
    QCRIL_LOG_DEBUG( "%s", "RIL APN is NULL, Use NULL string." );
  }

  if (!ril_auth_pref)
  {
    QCRIL_LOG_DEBUG("%s","No Authentication Preference provided.");
  }
  else
  {
    QCRIL_LOG_DEBUG("RIL provided authentication preference %s",ril_auth_pref);
  }

  for (i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++)
  {
    if (!VALIDATE_LOCAL_DATA_OBJ( (&info_tbl[i])))
    {
      break;
    }
  }
  if ( i == MAX_CONCURRENT_UMTS_DATA_CALLS )
  {
    QCRIL_LOG_ERROR( "%s", "no free info_tbl entry" );
    goto err_label;
  }

    /* The corresponding dsi_cb_tbl entry should have already been
       cleaned up. But, perform cleanup just in case */
    qcril_data_clear_dsi_cb_tbl_entry(&dsi_cb_tbl[i]);

    /* Allocate CID, copy APN and other call params */
    info_tbl[i].info_flg    = FALSE; /* mark call_info and dev_name as invalid */
    info_tbl[i].index       = i;
    info_tbl[i].cid         = i;
    info_tbl[i].suggestedRetryTime = QCRIL_DATA_SUGGESTEDRETRYTIME;
    memset(info_tbl[i].call_info.apn,
           0,
           sizeof(info_tbl[i].call_info.apn));

    strlcpy( info_tbl[i].call_info.apn,
             tmp_apn,
             sizeof(info_tbl[i].call_info.apn));
    info_tbl[i].instance_id = instance_id;
    info_tbl[i].modem_id    = modem_id;
    info_tbl[i].pend_tok    = params_ptr->t;
    info_tbl[i].pend_req    = QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL;
    QCRIL_LOG_INFO( "DEBUG:  %s step %d - info_tbl_ptr->pend_tok[0x%x] info_tbl_ptr->pend_req[0x%X]",
                    __func__, 0, info_tbl[i].pend_tok, info_tbl[i].pend_req );
    info_tbl[i].qmi_wds_hndl    = QCRIL_DATA_HNDL_INVALID;
    info_tbl[i].qmi_wds_hndl_v4 = QCRIL_DATA_HNDL_INVALID;
    info_tbl[i].qmi_wds_hndl_v6 = QCRIL_DATA_HNDL_INVALID;
    info_tbl[i].dsi_hndl =  QCRIL_DSI_HNDL_INVALID;
    info_tbl[i].qdp_umts_profile_id = QCRIL_INVALID_PROFILE_ID;
    info_tbl[i].qdp_cdma_profile_id = QCRIL_INVALID_PROFILE_ID;
    info_tbl[i].qdp_3gpp_profile_pdn_type  = QDP_PROFILE_PDN_TYPE_INVALID;
    info_tbl[i].qdp_3gpp2_profile_pdn_type = QDP_PROFILE_PDN_TYPE_INVALID;
    info_tbl[i].self        = &info_tbl[i];
    info_tbl[i].is_partial_retry = FALSE;
    info_tbl[i].last_addr_count  = 0;
    info_tbl[i].partial_retry_count = 0;

    /* Save the RIL provided call paramters */
    qcril_data_store_call_params(&info_tbl[i],
                                 ril_tech,
                                 ril_profile,
                                 ril_apn,
                                 ril_user,
                                 ril_pass,
                                 ril_auth_pref,
                                 ril_ipfamily,
                                 ril_ip_roaming,
                                 ril_apn_types,
                                 ril_roaming_allowed);

    /* store default call fail error */
    info_tbl[i].call_info.inactiveReason = PDP_FAIL_OEM_DCFAILCAUSE_4;

    /* The timer shouldn't be running. If for some reason it is
       running then stop it */
    qcril_data_util_stop_timer(&info_tbl[i].timer_id);

    /* The timer shouldn't be running. If for some reason it is
       running then stop it */
    qcril_data_util_stop_timer(&info_tbl[i].retry_timer_id);

    if ( ( info_tbl[i].dsi_hndl =
           dsi_get_data_srvc_hndl( qcril_data_net_cb,
                                   ( void *)&dsi_cb_tbl[i] ) ) == QCRIL_DSI_HNDL_INVALID )
    {
      QCRIL_LOG_ERROR( "%s", "unable to get dsi hndl" );
      goto err_label;
    }

    /* Update the dsi_cb_tbl[] entry with the required state information */
    qcril_data_update_dsi_cb_tbl_entry(&dsi_cb_tbl[i],
                                       instance_id,
                                       modem_id,
                                       params_ptr->t,
                                       info_tbl[i].dsi_hndl,
                                       &info_tbl[i]);

    QCRIL_LOG_DEBUG("info_tbl[%d] has reserved dsi_hndl[0x%x]",
                    i, info_tbl[i].dsi_hndl);

    QCRIL_DS_LOG_DBG_MEM( "dsi_hndl", info_tbl[i].dsi_hndl );

    /* check if APN is provided  */
    if (ril_apn != NULL)
    {
      RIL_DataCallFailCause cause = PDP_FAIL_NONE;
      umts_profile_id = cdma_profile_id = 0;

      if( FAILURE ==
          qcril_data_apn_based_profile_look_up_using_qdp( ril_apn,
                                                          ril_ipfamily,
                                                          ril_ip_roaming,
                                                          ril_common_tech,
                                                          ril_profile,
                                                          ril_user,
                                                          ril_pass,
                                                          ril_auth_pref,
                                                          &umts_profile_id,
                                                          &info_tbl[i].qdp_3gpp_profile_pdn_type,
                                                          &cdma_profile_id,
                                                          &info_tbl[i].qdp_3gpp2_profile_pdn_type,
                                                          &cause,
                                                          &abort_call ))
      {
        QCRIL_LOG_ERROR("%s", "qcril_data_apn_based_profile_look_up_using_qdp failed");
      }

      /* Check if profile lookup failed with abort call status */
      if( abort_call )
      {
        QCRIL_LOG_ERROR("%s", "profile lookup failed with abort call status");
        info_tbl[i].call_info.inactiveReason = cause;
        goto err_label;
      }

      if(umts_profile_id > 0)
      {
        info_tbl[i].qdp_umts_profile_id = umts_profile_id;
        /* we have both 3GPP and 3GPP2 profiles */
        /* set umts profile id */
        apn_info.buf_val = NULL;
        apn_info.num_val = umts_profile_id;
        QCRIL_LOG_VERBOSE("Setting umts profile id [%d] on info_tbl [%d]",
                          umts_profile_id, i);
        if ( ( dsi_set_data_call_param( info_tbl[i].dsi_hndl ,
                                        DSI_CALL_INFO_UMTS_PROFILE_IDX,
                                        &apn_info ) ) != DSI_SUCCESS )
        {
          QCRIL_LOG_ERROR("unable to set umts profile id on info_tbl  index [%d]",
                          info_tbl[i].index );
          goto err_label;
        }
      }

      if (cdma_profile_id >0)
      {
        info_tbl[i].qdp_cdma_profile_id = cdma_profile_id;
        /* set cdma profile id */
        apn_info.buf_val = NULL;
        apn_info.num_val = cdma_profile_id;
        QCRIL_LOG_VERBOSE("Setting cdma profile id [%d] on info_tbl [%d]",
                          cdma_profile_id, i);
        if ( ( dsi_set_data_call_param( info_tbl[i].dsi_hndl ,
                                        DSI_CALL_INFO_CDMA_PROFILE_IDX,
                                        &apn_info ) ) != DSI_SUCCESS )
        {
          QCRIL_LOG_ERROR("unable to set cdma profile id on info_tbl  index [%d]",
                          info_tbl[i].index );
          goto err_label;
        }
      }

      if (cdma_profile_id ==0 || umts_profile_id == 0)
      {
        /* set APN on the dsi_hndl */
        apn_info.buf_val = (void *)ril_apn;
        apn_info.num_val = strlen( ril_apn );
        QCRIL_LOG_VERBOSE( "RIL provided APN len [%d], APN string [%s]",
                           apn_info.num_val, (char *) apn_info.buf_val );
        if ( ( dsi_set_data_call_param( info_tbl[i].dsi_hndl ,
                                        DSI_CALL_INFO_APN_NAME,
                                        &apn_info ) ) != DSI_SUCCESS )
        {
          QCRIL_LOG_ERROR( "unable to set APN, index [%d]", info_tbl[i].index );
          goto err_label;
        }
      }
    }
    else
    {
      QCRIL_LOG_VERBOSE( "%s", "RIL did not provide APN, not setting any APN in start_nw_params" );
    }

    /* use the RIL profile id only if
     * APN was not provided */
    if (ril_apn == NULL)
    {
      /* set profile id in the dsi store */
      if (ril_profile == NULL)
      {
          QCRIL_LOG_ERROR("%s", "NULL profile (params->data[1]) received");
          QCRIL_LOG_ERROR("$s", "RIL interface advises to use value 0");
          goto err_label;
      }

      if (SUCCESS != qcril_data_set_ril_profile_id(
            i,
            ril_profile,
            ril_common_tech))
      {
        QCRIL_LOG_ERROR("could not set ril profile id in info_tbl index [%d]",
                        i);
        goto err_label;
      }
    }

    /* Update dsi tech pref to cdma so that route lookup returns cdma iface */

#if (QCRIL_RIL_VERSION >= 6)
    if (QCRIL_DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(numeric_ril_tech))
#else
    if ((ril_common_tech != NULL) && (!strcmp(ril_common_tech, QCRIL_CDMA_STRING)))
#endif
    {
      techpref_info.buf_val = NULL;
      techpref_info.num_val = DSI_RADIO_TECH_CDMA;

      if ( ( DSI_SUCCESS !=
             dsi_set_data_call_param( info_tbl[i].dsi_hndl,
                                      DSI_CALL_INFO_TECH_PREF,
                                      &techpref_info ) ) )
      {
        QCRIL_LOG_ERROR("unable to set tech pref, index [%d]", info_tbl[i].index );
        goto err_label;
      }
    }

    /* set nai */
    if (SUCCESS != qcril_data_set_nai(
          i,
          ril_user,
          ril_pass,
          ril_auth_pref))
    {
      QCRIL_LOG_ERROR("could not set ril nai in info_tbl index [%d]",
                      i);
      goto err_label;
    }

    /* set ip family (v4/v6) */
    if (ril_ipfamily != NULL && strlen(ril_ipfamily)>0)
    {
      ipfamily_info.buf_val = NULL;

      if (strcmp(ril_ipfamily, QCRIL_DATA_IP_FAMILY_V4) == 0)
      {
        ipfamily_info.num_val = DSI_IP_VERSION_4;
      }
      else if (strcmp(ril_ipfamily, QCRIL_DATA_IP_FAMILY_V6) == 0)
      {
        ipfamily_info.num_val = DSI_IP_VERSION_6;
        /*-------------------------------------------------------------------
          Add IPv4 automatically if CLAT is enabled on this profile
        -------------------------------------------------------------------*/
        if( global_qcril_clat_supported )
        {
          wds_profile_identifier_type_v01 prof_id;
          wds_get_profile_settings_resp_msg_v01 prof_params;

          memset( &prof_params, 0, sizeof( prof_params ) );

          prof_id.profile_type = ( ril_common_tech == QCRIL_CDMA_STRING ) ?
            WDS_PROFILE_TYPE_3GPP2_V01 : WDS_PROFILE_TYPE_3GPP_V01;
          prof_id.profile_index = ( ril_common_tech == QCRIL_CDMA_STRING ) ?
            cdma_profile_id : umts_profile_id;
          QCRIL_LOG_DEBUG("%s(): querying profile type %d index %d",
                          __func__,
                          prof_id.profile_type,
                          prof_id.profile_index);
          if(E_SUCCESS != qcril_data_qmi_wds_query_profile(global_qcril_qmi_wds_hndl,
                                                           &prof_id,
                                                           &prof_params))
          {
            QCRIL_LOG_ERROR("query_profile for profile index %d failed",
                            prof_id.profile_index);
          }
          else if(prof_params.clat_enabled)
          {
            QCRIL_LOG_DEBUG("%s(): Automatic IPv4 request for profile index %d" \
                "IPv6 -> Dual-IP", __func__, prof_id.profile_index);
            ipfamily_info.num_val = DSI_IP_VERSION_4_6;
          }
          else
          {
            QCRIL_LOG_DEBUG("%s(): XLAT not enabled on profile index %d",
                            __func__, prof_id.profile_index);
          }
        }
      }
      else if (strcmp(ril_ipfamily, QCRIL_DATA_IP_FAMILY_V4V6) == 0)
      {
  #if (QCRIL_RIL_VERSION >= 6)
        ipfamily_info.num_val = DSI_IP_VERSION_4_6;
  #else
        QCRIL_LOG_DEBUG("%s", "respond to QCRIL as data call setup FAILURE");
        qcril_data_response_setup_data_call_failure( &info_tbl[i],
                                                     instance_id,
                                                     params_ptr->t,
                                                     PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED );
        goto cleanup;
  #endif /* (QCRIL_RIL_VERSION >= 6) */
      }
      else
      {
        QCRIL_LOG_ERROR("ril passed invalid ip family string [%s]",
                        ril_ipfamily);
        goto err_label;
      }

      /* if home and roam IP are different, submit as IPV4V6 */
      if (ril_ip_roaming != NULL &&
          strcasecmp(ril_ipfamily, ril_ip_roaming) != 0)
      {
        QCRIL_LOG_INFO("%s", "home and roam IP are different, use IPv4v6");
        ipfamily_info.num_val = DSI_IP_VERSION_4_6;
      }

      if ( ( dsi_set_data_call_param( info_tbl[i].dsi_hndl ,
                                      DSI_CALL_INFO_IP_VERSION,
                                      &ipfamily_info) ) != DSI_SUCCESS )
      {
        QCRIL_LOG_ERROR("unable to set ip family, index [%d]", info_tbl[i].index );
        goto err_label;
      }

      info_tbl[i].dsi_ip_version = ipfamily_info.num_val;
    }
  #if (QCRIL_RIL_VERSION >= 6)
    /* Default to a Dual-IP call */
    else
    {
      ipfamily_info.num_val = DSI_IP_VERSION_4_6;
      ipfamily_info.buf_val = NULL;

      if ( ( dsi_set_data_call_param( info_tbl[i].dsi_hndl ,
                                      DSI_CALL_INFO_IP_VERSION,
                                      &ipfamily_info) ) != DSI_SUCCESS )
      {
        QCRIL_LOG_ERROR("unable to set ip family, index [%d]", info_tbl[i].index );
        goto err_label;
      }

      info_tbl[i].dsi_ip_version = DSI_IP_VERSION_4_6;
    }
  #endif /* (QCRIL_RIL_VERSION >= 6) */

  if (ril_apn != NULL && ril_apn_types != NULL)
  {
    QCRIL_LOG_DEBUG("qcril_data_handle_set_apn_info_req on call bringup");
    qcril_data_handle_set_apn_info_req(ril_apn, atoi(ril_apn_types));
  }

  if (ril_roaming_allowed != NULL)
  {
    disallow_roaming_info.buf_val = NULL;
    disallow_roaming_info.num_val = atoi(ril_roaming_allowed) ? 0 : 1;

    if ( ( dsi_set_data_call_param( info_tbl[i].dsi_hndl ,
                                    DSI_CALL_INFO_DISALLOW_ROAMING,
                                    &disallow_roaming_info) ) != DSI_SUCCESS )
    {
      QCRIL_LOG_ERROR("unable to set disallow_roaming, index [%d]", info_tbl[i].index );
      goto err_label;
    }
  }

  /* Insert local info tbl ref in reqlist */
  u_info.ds.info = (void *) &info_tbl[i];
  qcril_reqlist_default_entry( params_ptr->t,
                               params_ptr->event_id,
                               modem_id,
                               QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                               QCRIL_EVT_DATA_EVENT_CALLBACK,
                               &u_info,
                               &reqlist_entry );
  if ( qcril_reqlist_new( instance_id, &reqlist_entry ) != E_SUCCESS )
  {
    /* Fail to insert entry to ReqList */
    QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
    return;
  }

  QCRIL_LOG_VERBOSE( "%s", "reqlist node insert complete, setup data call" );

  /* Start data call */
  if ( ( dsi_start_data_call( info_tbl[i].dsi_hndl ) ) != DSI_SUCCESS )
  {
    QCRIL_LOG_ERROR( "unable to setup data call, index [%d]", info_tbl[i].index );
    QCRIL_DS_LOG_DBG_MEM( "dsi_hndl", info_tbl[i].dsi_hndl );
    qcril_data_get_call_end_reason( &info_tbl[i], &call_end_reason );
    info_tbl[i].call_info.inactiveReason = call_end_reason;
    last_call_end_reason = call_end_reason;
    qcril_reqlist_free( instance_id, params_ptr->t );
    goto err_label;
  }

  /* Create and start a data call setup response timer to make sure that
     we don't wait forever for the data call setup response */
  QCRIL_LOG_DEBUG("starting response timer for info_tbl [%#x]",&info_tbl[i]);
  qcril_data_start_response_timer(&info_tbl[i]);

  /* For a Dual-IP call create the partial retry timer */
  if (DSI_IP_VERSION_4_6 == ipfamily_info.num_val)
  {
    QCRIL_LOG_DEBUG("creating retry timer for info_tbl [%#x]",&info_tbl[i]);
    qcril_data_create_retry_timer(&info_tbl[i]);
  }

  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  QCRIL_LOG_DEBUG("%s", "qcril_data_request_setup_data_call: EXIT with suc");

  return;

err_label:
  QCRIL_LOG_DEBUG("%s", "respond to QCRIL with failure");
  qcril_data_response_setup_data_call_failure( &info_tbl[i],
                                               instance_id,
                                               params_ptr->t,
                                               info_tbl[i].call_info.inactiveReason );
#if QCRIL_RIL_VERSION < 6
cleanup:
#endif
  /* clean up if we had reserved an info tbl entry */
  if (i < MAX_CONCURRENT_UMTS_DATA_CALLS    &&
      VALIDATE_LOCAL_DATA_OBJ(&info_tbl[i]) &&
      info_tbl[i].pend_tok == params_ptr->t)
  {
    qcril_data_cleanup_call_state(&info_tbl[i]);
  }
err_bad_input:
  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  QCRIL_LOG_ERROR("%s", "qcril_data_request_setup_data_call: EXIT with FAILURE");
  return;

}/* qcril_data_request_setup_data_call() */


/*===========================================================================

  FUNCTION:  qcril_data_store_call_params

===========================================================================*/
/*!
    @brief
    This function stores the given call parameters in the corresponding
    info_tbl entry

    @pre Before calling, info_tbl_mutex must be locked by the calling thread

    @param
      info_tbl_ptr  [in] - info_tbl entry to copy the call paramters to
      ril_tech      [in] - Technology
      ril_profile   [in] - Profile Number
      ril_apn       [in] - APN
      ril_user      [in] - Username
      ril_pass      [in] - Password
      ril_auth_pref [in] - Authentication Preference
      ril_ipfamily  [in] - IP Family
      ril_ip_roaming[in] - Roaming IP Type
      ril_apn_types [in] - APn Types
      ril_roaming_allowed [in] - If Roaming is Allowed

    @return
      None
*/
/*=========================================================================*/
static void
qcril_data_store_call_params
(
  qcril_data_call_info_tbl_type  *info_tbl_ptr,
  const char                     *ril_tech,
  const char                     *ril_profile,
  const char                     *ril_apn,
  const char                     *ril_user,
  const char                     *ril_pass,
  const char                     *ril_auth_pref,
  const char                     *ril_ipfamily,
  const char                     *ril_ip_roaming,
  const char                     *ril_apn_types,
  const char                     *ril_roaming_allowed
)
{
  if (!info_tbl_ptr)
  {
    QCRIL_LOG_ERROR("%s", "bad parameter");
    return;
  }

  QCRIL_LOG_DEBUG("%s", "storing RIL call params");

  /* Free the memory for call_params if it hasn't been freed */
  QCRIL_DATA_RELEASE_STORAGE(info_tbl_ptr->call_params);

  /* Allocate storage for call parameters */
  info_tbl_ptr->call_params = malloc(sizeof(qcril_data_call_params_type));

  if (!info_tbl_ptr->call_params)
  {
    QCRIL_LOG_ERROR("%s", "failed to allocate call_params");
    return;
  }

  /* Initialize the memory */
  memset(info_tbl_ptr->call_params, 0, sizeof(qcril_data_call_params_type));

  /* Copy the technology */
  if (ril_tech)
  {
    QCRIL_LOG_DEBUG("copying ril_tech=%s", ril_tech);

    strlcpy(info_tbl_ptr->call_params->ril_tech,
            ril_tech,
            sizeof(info_tbl_ptr->call_params->ril_tech));
  }

  /* Copy the profile */
  if (ril_profile)
  {
    QCRIL_LOG_DEBUG("copying ril_profile=%s", ril_profile);

    strlcpy(info_tbl_ptr->call_params->ril_profile,
            ril_profile,
            sizeof(info_tbl_ptr->call_params->ril_profile));
  }

  /* Copy the APN */
  if (ril_apn)
  {
    QCRIL_LOG_DEBUG("copying ril_apn=%s", ril_apn);

    strlcpy(info_tbl_ptr->call_params->ril_apn,
            ril_apn,
            sizeof(info_tbl_ptr->call_params->ril_apn));
  }

  /* Copy the Username */
  if (ril_user)
  {
    QCRIL_LOG_DEBUG("copying ril_user=%s", ril_user);

    strlcpy(info_tbl_ptr->call_params->ril_user,
            ril_user,
            sizeof(info_tbl_ptr->call_params->ril_user));
  }

  /* Copy the Password */
  if (ril_pass)
  {
    QCRIL_LOG_DEBUG("copying ril_pass=%s", ril_pass);

    strlcpy(info_tbl_ptr->call_params->ril_pass,
            ril_pass,
            sizeof(info_tbl_ptr->call_params->ril_pass));
  }

  /* Copy the Authentication Preference */
  if (ril_auth_pref)
  {
    QCRIL_LOG_DEBUG("copying ril_auth_pref=%s", ril_auth_pref);

    strlcpy(info_tbl_ptr->call_params->ril_auth_pref,
            ril_auth_pref,
            sizeof(info_tbl_ptr->call_params->ril_auth_pref));
  }

  /* Copy the IP Family */
  if (ril_ipfamily)
  {
    QCRIL_LOG_DEBUG("copying ril_ipfamily=%s", ril_ipfamily);

    strlcpy(info_tbl_ptr->call_params->ril_ipfamily,
            ril_ipfamily,
            sizeof(info_tbl_ptr->call_params->ril_ipfamily));
  }

  /* Copy the Roaming IP Type */
  if (ril_ip_roaming)
  {
    QCRIL_LOG_DEBUG("copying ril_ip_roaming=%s", ril_ip_roaming);

    strlcpy(info_tbl_ptr->call_params->ril_ip_roaming,
            ril_ip_roaming,
            sizeof(info_tbl_ptr->call_params->ril_ip_roaming));
  }

  /* Copy APN Types */
  if (ril_apn_types)
  {
    QCRIL_LOG_DEBUG("copying ril_apn_types=%s", ril_apn_types);

    strlcpy(info_tbl_ptr->call_params->ril_apn_types,
            ril_apn_types,
            sizeof(info_tbl_ptr->call_params->ril_apn_types));
  }

  /* Copy Roaming Allowed Flag */
  if (ril_roaming_allowed)
  {
    QCRIL_LOG_DEBUG("copying ril_roaming_allowed=%s", ril_roaming_allowed);

    strlcpy(info_tbl_ptr->call_params->ril_roaming_allowed,
            ril_roaming_allowed,
            sizeof(info_tbl_ptr->call_params->ril_roaming_allowed));
  }
}


/*===========================================================================

  FUNCTION:  qcril_data_apn_based_profile_look_up_using_qdp

===========================================================================*/
/*!
    @brief
    Looks up the 3GPP and 3GPP2 profile IDs based on the APN string
    and IP_FAMILY. QDP considers RIL provided optional technology and
    profile_id to determine which profiles(if any) need to be looked
    up or created.

    @return
    SUCCESS
    FAILURE
*/
/*=========================================================================*/
static int qcril_data_apn_based_profile_look_up_using_qdp
(
  const char * ril_apn,
  const char * ril_ipfamily,
  const char * ril_ip_roaming,
  const char * ril_tech,
  const char * ril_profile,
  const char * ril_user,
  const char * ril_pass,
  const char * ril_auth_pref,
  int * umts_profile_id,
  qdp_profile_pdn_type *umts_profile_pdn_type,
  int * cdma_profile_id,
  qdp_profile_pdn_type *cdma_profile_pdn_type,
  RIL_DataCallFailCause * cause,
  boolean * abort_call
)
{
  int ret = FAILURE;
  const char * qdp_params[QDP_RIL_PARAM_MAX];
  qdp_tech_t current_tech = QDP_NOTECH;
  qdp_error_info_t error_info;
  int rc;

  QCRIL_LOG_DEBUG("%s","qcril_data_apn_based_profile_look_up_using_qdp: ENTRY");

  do
  {
    if( NULL == umts_profile_id       ||
        NULL == umts_profile_pdn_type ||
        NULL == cdma_profile_id       ||
        NULL == cdma_profile_pdn_type ||
        NULL == cause                 ||
        NULL == abort_call )
    {
      QCRIL_LOG_ERROR("%s","NULL placeholders provided");
      break;
    }

    /* only APN is mandatory RIL param. Rest are optional RIL params */
    if (NULL == ril_apn)
    {
      QCRIL_LOG_ERROR("%s","ril_apn (provided NULL) is mandatory param for "
                      "this function.");
      break;
    }

    memset(qdp_params, 0, sizeof(qdp_params));

    /* build up QDP parameters */
    /* prepare APN */
    if (strlen(ril_apn) > (QMI_WDS_APN_NAME_MAX_V01))
    {
      QCRIL_LOG_ERROR("RIL provided invalid APN [%s] "
                      "APN length [%d] exceeds max allowed [%d]",
                      ril_apn, strlen(ril_apn), QMI_WDS_APN_NAME_MAX_V01);
      break;
    }
    qdp_params[QDP_RIL_APN] = ril_apn;

    QCRIL_LOG_DEBUG("qdp_param APN = [%s]", qdp_params[QDP_RIL_APN]);
    /* prepare technology */
    if (ril_tech != NULL && strlen(ril_tech)>0)
    {
      if (0 == strncmp(QDP_RIL_3GPP, ril_tech, QDP_RIL_TECH_LEN))
      {
        current_tech = QDP_3GPP;
      }
      else if(0 == strncmp(QDP_RIL_3GPP2, ril_tech, QDP_RIL_TECH_LEN))
      {
        current_tech = QDP_3GPP2;
      }
      else
      {
        QCRIL_LOG_ERROR("RIL provided unsupported technology [%s]", ril_tech);
        break;
      }

      /* do not use RIL technology for profile look up */
#ifndef FEATURE_QCRIL_FUSION
      /* for targets that are not Fusion I devices, we rely on
       * ro.baseband property to determine if the ril tech should be
       * ignored or not (done at power up) */
      if (FALSE == ignore_ril_tech)
      {
        if (strlen(ril_tech) > QDP_RIL_TECH_LEN)
        {
          QCRIL_LOG_ERROR("RIL provided invalid technology [%s] "
                          "tech length [%d] exceeds max allowed [%d]",
                          ril_tech, strlen(ril_tech), QDP_RIL_TECH_LEN);
          break;
        }
        qdp_params[QDP_RIL_TECH] = ril_tech;
        QCRIL_LOG_DEBUG("qdp_param TECH = [%s]", qdp_params[QDP_RIL_TECH]);
      }
      else
      {
        QCRIL_LOG_DEBUG("%s","ignore_ril_tech is set to TRUE");
        QCRIL_LOG_DEBUG("%s","qdp param TECH not set on purpose in order to" \
                        " enforce 3gpp and 3gpp2 profile look up");
      }
#else
      QCRIL_LOG_DEBUG("%s","qdp param TECH not set on purpose in order to"
                           " enforce 3gpp and 3gpp2 profile look up");
#endif
    }

    /* prepare profile id */
    if (ril_profile != NULL && strlen(ril_profile)>0)
    {
      if (strlen(ril_profile) > QCRIL_PROFILE_ID_STR_MAX)
      {
        QCRIL_LOG_ERROR("RIL provided invalid profile id [%s] "
                        "profile id length [%d] exceeds max allowed [%d]",
                        ril_profile, strlen(ril_profile), QCRIL_PROFILE_ID_STR_MAX);
        break;
      }
      qdp_params[QDP_RIL_PROFILE_ID] = ril_profile;

      QCRIL_LOG_DEBUG("qdp param PROFILE_ID = [%s]", qdp_params[QDP_RIL_PROFILE_ID]);
    }
    /* prepare IP family version */
    if (ril_ipfamily != NULL && strlen(ril_ipfamily) > 0)
    {
      if (strlen(ril_ipfamily) > QCRIL_IP_FAMILY_STR_MAX)
      {
        QCRIL_LOG_ERROR("RIL provided invalid ip family [%s] "
                        "ip family length [%d] exceeds max allowed [%d]",
                        ril_ipfamily, strlen(ril_ipfamily), QCRIL_IP_FAMILY_STR_MAX);
        break;
      }
      qdp_params[QDP_RIL_IP_FAMILY] = ril_ipfamily;

      QCRIL_LOG_DEBUG("qdp param IP_FAMILY = [%s]", qdp_params[QDP_RIL_IP_FAMILY]);
    }

    /* prepare roaming IP */
    if (ril_ip_roaming != NULL && strlen(ril_ip_roaming) > 0)
    {
      if (strlen(ril_ip_roaming) > QCRIL_IP_FAMILY_STR_MAX)
      {
        QCRIL_LOG_ERROR("RIL provided invalid roaming ip [%s] "
                        "roaming ip length [%d] exceeds max allowed [%d]",
                        ril_ip_roaming, strlen(ril_ip_roaming), QCRIL_IP_FAMILY_STR_MAX);
        break;
      }
      qdp_params[QDP_RIL_IP_ROAMING] = ril_ip_roaming;

      QCRIL_LOG_DEBUG("qdp param IP_ROAMING = [%s]", qdp_params[QDP_RIL_IP_ROAMING]);
    }

    /* Prepare username */
    if (ril_user != NULL && strlen(ril_user) > 0)
    {
      if (strlen(ril_user) > QMI_WDS_USER_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("RIL provided username exceeds max allowed [%d]",
                        QMI_WDS_USER_NAME_MAX_V01);
      }
      else
      {
        qdp_params[QDP_RIL_NAI] = ril_user;
        QCRIL_LOG_DEBUG("qdp param USERNAME = [%s]", qdp_params[QDP_RIL_NAI]);
      }
    }

    /* prepare password */
    if (ril_pass != NULL && strlen(ril_pass) > 0)
    {
      if (strlen(ril_pass) > QMI_WDS_PASSWORD_MAX_V01)
      {
        QCRIL_LOG_ERROR("RIL provided password exceeds max allowed [%d]",
                        QMI_WDS_PASSWORD_MAX_V01);
      }
      else
      {
        qdp_params[QDP_RIL_PASSWORD] = ril_pass;
        QCRIL_LOG_DEBUG("qdp param PASSWORD = [%s]", qdp_params[QDP_RIL_PASSWORD]);
      }
    }

    /* prepare authtype */
    if (ril_auth_pref != NULL && strlen(ril_auth_pref) > 0)
    {
      if (strlen(ril_auth_pref) > QCRIL_DATA_AUTH_PREF_MAX_LEN-1)
      {
        QCRIL_LOG_ERROR("RIL provided auth exceeds max allowed [%d]",
                        QCRIL_DATA_AUTH_PREF_MAX_LEN);
      }
      else
      {
        qdp_params[QDP_RIL_AUTH] = ril_auth_pref;
        QCRIL_LOG_DEBUG("qdp param AUTH type = [%s]", qdp_params[QDP_RIL_AUTH]);
      }
    }
    *umts_profile_id = *cdma_profile_id = 0;
    memset( &error_info, 0x0, sizeof(error_info) );
    *abort_call = FALSE;

    rc = qdp_profile_look_up( qdp_params,
                              (unsigned int *)umts_profile_id,
                              umts_profile_pdn_type,
                              (unsigned int *)cdma_profile_id,
                              cdma_profile_pdn_type,
                              &error_info );

    if( QDP_SUCCESS == rc )
    {
      QCRIL_LOG_DEBUG("successfully looked up 3gpp profile id [%d], type [%d] and "
                      "3gpp2 profile id [%d], type [%d] using ril params, "
                      "lookup_error[%d], abort call [%d]",
                      *umts_profile_id, *umts_profile_pdn_type,
                      *cdma_profile_id, *cdma_profile_pdn_type,
                      error_info.error, error_info.tech);
    }
    else
    {
      QCRIL_LOG_DEBUG("not able to look up both profile ids. "
                      "3gpp profile id [%d], type [%d], and 3gpp2 profile id [%d], type [%d] "
                      "returned by QDP, lookup error[%d] tech[%d]",
                      *umts_profile_id, *cdma_profile_id,
                      *umts_profile_pdn_type, *cdma_profile_pdn_type,
                      error_info.error, error_info.tech);

      /* Check if the technology reporting failed lookup matches the
       * tech reported by RIL API as current service.  On match, abort
       * the call setup request. */
      if( (QDP_ERROR_NONE != error_info.error) &&
          (current_tech == error_info.tech) )
      {
        QCRIL_LOG_DEBUG("lookup[%d] and RIL[%d] tech match, abort call.",
                        error_info.tech, current_tech);
        *abort_call = TRUE;

        /* Map lookup_error to RIL API error */
        switch( error_info.error )
        {
          case QDP_ERROR_ONLY_IPV4_ALLOWED:
            *cause = PDP_FAIL_ONLY_IPV4_ALLOWED;
            break;
          case QDP_ERROR_ONLY_IPV6_ALLOWED:
            *cause = PDP_FAIL_ONLY_IPV6_ALLOWED;
            break;
          case QDP_ERROR_NONE:
            *cause = PDP_FAIL_NONE;
            break;
          default:
            QCRIL_LOG_ERROR("unsupported lookup_error[%d]", error_info.error);
        }
      }
      break;
    }
    ret = SUCCESS;
  } while (0);

  QCRIL_LOG_DEBUG("%s","qcril_data_apn_based_profile_look_up_using_qdp: EXIT");

  return ret;
}


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
static int qcril_data_set_ril_profile_id
(
  int info_tbl_index,
  const char * ril_profile,
  const char * ril_tech
)
{
  int ret = FAILURE;
  int reti = SUCCESS;
  dsi_call_param_value_t profile_id;
  dsi_call_param_value_t call_type;
  int profile_param_id = 0;

  do
  {
    /* sanity checking */
    if (NULL == ril_profile)
    {
      QCRIL_LOG_ERROR("%s","NULL ril_profile received");
      QCRIL_LOG_ERROR("%s","Ignoring profile id for SETUP_DATA_CALL");
      return SUCCESS;
    }

    if ((RIL_DATA_PROFILE_DEFAULT == atoi(ril_profile)))
    {
      QCRIL_LOG_ERROR("default profile id [%d] provided. " \
                      "no need to set in dsi_hndl", atoi(ril_profile));
      ret = SUCCESS;
      break;
    }

    if(ril_tech == NULL)
    {
      QCRIL_LOG_ERROR("RIL provided profile id without specifying" \
                      " the technology (CDMA/UMTS). This profile id" \
                      " [%s] will be ignored", ril_profile);
      break;
    }

    /* prepare dsi parameter with profile id */
    memset( &profile_id, 0, sizeof( dsi_call_param_value_t ) );
    profile_id.buf_val = NULL;
    profile_id.num_val = atoi(ril_profile);
    QCRIL_LOG_VERBOSE("RIL provided PROFILE ID Number [%d]",
                      profile_id.num_val);

    /* adjust the profile id according to ril.h */
    if (profile_id.num_val < RIL_DATA_PROFILE_OEM_BASE)
    {
      reti = SUCCESS;
      switch(profile_id.num_val)
      {
      case RIL_DATA_PROFILE_TETHERED:
        QCRIL_LOG_VERBOSE("RIL provided [%d] profile id. Tethered call "
                          "will be used", profile_id.num_val);
        call_type.buf_val = NULL;
        call_type.num_val = DSI_CALL_TYPE_TETHERED;
        if (dsi_set_data_call_param(info_tbl[info_tbl_index].dsi_hndl,
                                    DSI_CALL_INFO_CALL_TYPE,
                                    &call_type) != DSI_SUCCESS)
        {
          QCRIL_LOG_ERROR("Couldn't set call_type [%d]",
                          call_type.num_val);
          reti = FAILURE;
          break;
        }
        break;
      default:
        QCRIL_LOG_ERROR("RIL provided [%d] profile id. This is currently "
                        "not supported", profile_id.num_val);
      }
      if (reti != SUCCESS)
      {
        break;
      }
    }
    else
    {
      /* adjust for the OEM base */
      profile_id.num_val -= RIL_DATA_PROFILE_OEM_BASE;
      QCRIL_LOG_VERBOSE("profile_id.num_val %d will be used (android provided %d)",
                        profile_id.num_val,
                        profile_id.num_val + RIL_DATA_PROFILE_OEM_BASE);
      /* figure out whether this is umts or cdma profile id */
      if (!strcmp(ril_tech, QCRIL_CDMA_STRING))
      {
        profile_param_id = DSI_CALL_INFO_CDMA_PROFILE_IDX;
      }
      else if (!strcmp(ril_tech, QCRIL_UMTS_STRING))
      {
        profile_param_id = DSI_CALL_INFO_UMTS_PROFILE_IDX;
      }
      else
      {
        QCRIL_LOG_ERROR("RIL provided incorrect/malformed technology [%s]",
                        ril_tech);
        break;
      }

      /* now set the profile id onto dsi_hndl */
      if ( ( dsi_set_data_call_param(info_tbl[info_tbl_index].dsi_hndl,
                                     profile_param_id,
                                     &profile_id ) ) != DSI_SUCCESS )
      {
        QCRIL_LOG_ERROR("unable to set profile id [%d], info_tbl index [%d]",
                        profile_id.num_val, info_tbl_index);
        break;
      }
    }


    ret = SUCCESS;
  } while (0);

  if (SUCCESS == ret)
  {
    QCRIL_LOG_DEBUG("%s", "qcril_data_set_ril_profile_id successful");

  }
  else
  {
    QCRIL_LOG_DEBUG("%s", "qcril_data_set_ril_profile_id failed");
  }

  return ret;
}


/*===========================================================================

  FUNCTION:  qcril_data_set_nai

===========================================================================*/
/*!
    @brief
    Sets the RIL provided NAI (username, password, auth_pref)
    on the dsi_hndl

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    SUCCESS
    FAILURE
*/
/*=========================================================================*/
static int qcril_data_set_nai
(
  int info_tbl_index,
  const char * ril_user,
  const char * ril_pass,
  const char * ril_auth_pref
)
{
  int ret = FAILURE;
  int reti = SUCCESS;
  dsi_call_param_value_t username_info, password_info,authpref_info;

  do
  {

    memset( &username_info, 0, sizeof( dsi_call_param_value_t ) );
    memset( &password_info, 0, sizeof( dsi_call_param_value_t ) );
    memset( &authpref_info, 0, sizeof( dsi_call_param_value_t ) );

    if(ril_auth_pref != NULL && strlen(ril_auth_pref) !=0)
    {
      /* set auth_pref */
      authpref_info.buf_val = NULL;
      authpref_info.num_val = atoi(ril_auth_pref);
      reti = SUCCESS;
      switch(authpref_info.num_val)
      {
      case 0:
        authpref_info.num_val = DSI_AUTH_PREF_PAP_CHAP_NOT_ALLOWED;
        break;
      case 1:
        authpref_info.num_val = DSI_AUTH_PREF_PAP_ONLY_ALLOWED;
        break;
      case 2:
        authpref_info.num_val = DSI_AUTH_PREF_CHAP_ONLY_ALLOWED;
        break;
      case 3:
        authpref_info.num_val = DSI_AUTH_PREF_PAP_CHAP_BOTH_ALLOWED;
        break;
      default:
        QCRIL_LOG_ERROR("invalid auth pref received [%d]", authpref_info.num_val);
        reti = FAILURE;
        break;
      }
      if (SUCCESS != reti)
      {
        break;
      }
      if((dsi_set_data_call_param(info_tbl[info_tbl_index].dsi_hndl,
                                  DSI_CALL_INFO_AUTH_PREF,
                                  &authpref_info) ) != DSI_SUCCESS)
      {
        QCRIL_LOG_ERROR("unable to set AUTH PREF [%d], info_tbl index [%d]",
                        authpref_info.num_val,info_tbl_index);
        break;
      }
    }

    /* set username */
    if ((ril_user != NULL) && strlen(ril_user) != 0)
    {
      username_info.buf_val = (void *)ril_user;
      username_info.num_val = strlen( ril_user );
      QCRIL_LOG_VERBOSE( "RIL provided UserName, len [%d]", username_info.num_val);
      if ( ( dsi_set_data_call_param( info_tbl[info_tbl_index].dsi_hndl ,
                                      DSI_CALL_INFO_USERNAME,
                                     &username_info ) ) != DSI_SUCCESS )
      {
        QCRIL_LOG_ERROR("unable to set username [%s], info_tbl index [%d]",
                        ril_user, info_tbl_index);
        break;
      }
    }

    /* set password */
    if ((ril_pass != NULL) && strlen(ril_pass) != 0)
    {
      password_info.buf_val = (void *)ril_pass;
      password_info.num_val = strlen( ril_pass );
      QCRIL_LOG_VERBOSE( "RIL provided Password, len [%d]", password_info.num_val);

      if ( ( dsi_set_data_call_param( info_tbl[info_tbl_index].dsi_hndl ,
                                      DSI_CALL_INFO_PASSWORD,
                                      &password_info ) ) != DSI_SUCCESS )
      {
        QCRIL_LOG_ERROR("unable to set password [%s], info_tbl index [%d]",
                        ril_pass,info_tbl_index);
        break;
      }
    }
    ret = SUCCESS;
  }while(0);

  if (SUCCESS == ret)
  {
    QCRIL_LOG_DEBUG("%s", "qcril_data_set_nai successful");

  }
  else
  {
    QCRIL_LOG_DEBUG("%s", "qcril_data_set_nai failed");
  }

  return ret;
}


/*===========================================================================

  FUNCTION:  qcril_data_create_retry_timer

===========================================================================*/
/*!
    @brief
    Create a partial retry timer for use with Dual-IP calls

    @pre Before calling, info_tbl_mutex must be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
static void qcril_data_create_retry_timer
(
  qcril_data_call_info_tbl_type *info_tbl_ptr
)
{
  struct sigevent sigev;

  QCRIL_LOG_FUNC_ENTRY();

  if (!VALIDATE_LOCAL_DATA_OBJ(info_tbl_ptr))
  {
    QCRIL_LOG_ERROR( "invalid info_tbl_ptr:[%p] ",
                     (unsigned int *)info_tbl_ptr);
    goto bail;
  }

  /* The timer_id should be invalid, if not log an error and delete it */
  if (QCRIL_DATA_INVALID_TIMERID != info_tbl_ptr->retry_timer_id)
  {
    QCRIL_LOG_ERROR( "deleting stale retry_timer_id:[%#"PRIxPTR"] ",
                     (uintptr_t)info_tbl_ptr->retry_timer_id);

    qcril_data_util_stop_timer(&info_tbl_ptr->retry_timer_id);
  }

  sigev.sigev_notify            = SIGEV_THREAD;
  sigev.sigev_notify_attributes = NULL;
  sigev.sigev_value.sival_ptr   = info_tbl_ptr;
  sigev.sigev_notify_function   = qcril_data_partial_retry_hdlr;

  /* Create the timer */
  if (-1 == timer_create(CLOCK_REALTIME, &sigev, &info_tbl_ptr->retry_timer_id))
  {
    QCRIL_LOG_ERROR( "failed to create timer for info_tbl_ptr:[%p] ",
                     (unsigned int *)info_tbl_ptr);
    goto bail;
  }

  QCRIL_LOG_VERBOSE( "timer creation success: [%#x]", info_tbl_ptr->retry_timer_id );

bail:
  QCRIL_LOG_FUNC_RETURN();
}


// FIXME
// As an enhancement, instead of relying on the order of the
// masks use a list of pairs like the following:
// [
//   {"default", QMI_DSD_APN_TYPE_MASK_DEFAULT_V01},
//   {"ims", QMI_DSD_APN_TYPE_MASK_IMS_V01},
//   ...
// ]
void enable_mask_bit(const char *apn_type, uint64 *mask)
{
  if (apn_type == NULL || mask == NULL)
  {
    QCRIL_LOG_ERROR("%s", "enable_mask_bit: invalid params");
    return;
  }

  if (strcmp(available_apn_types[0], apn_type) == 0)
  {
    *mask = *mask | (
      QMI_DSD_APN_TYPE_MASK_DEFAULT_V01 |
      QMI_DSD_APN_TYPE_MASK_IMS_V01 |
      QMI_DSD_APN_TYPE_MASK_MMS_V01 |
      QMI_DSD_APN_TYPE_MASK_DUN_V01 |
      QMI_DSD_APN_TYPE_MASK_SUPL_V01 |
      QMI_DSD_APN_TYPE_MASK_HIPRI_V01 |
      QMI_DSD_APN_TYPE_MASK_FOTA_V01 |
      QMI_DSD_APN_TYPE_MASK_CBS_V01 |
      QMI_DSD_APN_TYPE_MASK_IA_V01 |
      QMI_DSD_APN_TYPE_MASK_EMERGENCY_V01
      );
  } else if (strcmp(available_apn_types[1], apn_type) == 0)
  {
    *mask = *mask | QMI_DSD_APN_TYPE_MASK_DEFAULT_V01;
  } else if (strcmp(available_apn_types[2], apn_type) == 0)
  {
    *mask = *mask | QMI_DSD_APN_TYPE_MASK_IMS_V01;
  } else if (strcmp(available_apn_types[3], apn_type) == 0)
  {
    *mask = *mask | QMI_DSD_APN_TYPE_MASK_MMS_V01;
  } else if (strcmp(available_apn_types[4], apn_type) == 0)
  {
    *mask = *mask | QMI_DSD_APN_TYPE_MASK_DUN_V01;
  } else if (strcmp(available_apn_types[5], apn_type) == 0)
  {
    *mask = *mask | QMI_DSD_APN_TYPE_MASK_SUPL_V01;
  } else if (strcmp(available_apn_types[6], apn_type) == 0)
  {
    *mask = *mask | QMI_DSD_APN_TYPE_MASK_HIPRI_V01;
  } else if (strcmp(available_apn_types[7], apn_type) == 0)
  {
    *mask = *mask | QMI_DSD_APN_TYPE_MASK_FOTA_V01;
  } else if (strcmp(available_apn_types[8], apn_type) == 0)
  {
    *mask = *mask | QMI_DSD_APN_TYPE_MASK_CBS_V01;
  } else if (strcmp(available_apn_types[9], apn_type) == 0)
  {
    *mask = *mask | QMI_DSD_APN_TYPE_MASK_IA_V01;
  } else if (strcmp(available_apn_types[10], apn_type) == 0)
  {
    *mask = *mask | QMI_DSD_APN_TYPE_MASK_EMERGENCY_V01;
  } else if (strcmp(available_apn_types[11], apn_type) == 0)
  {
    *mask = *mask | QMI_DSD_APN_TYPE_MASK_DEFAULT_V01;
  }
}

/*===========================================================================

  FUNCTION:  qcril_data_partial_retry_hdlr

===========================================================================*/
/*!
    @brief
    Handler for Dual-IP partial retries

    @return
    None.
*/
/*=========================================================================*/
static void qcril_data_partial_retry_hdlr
(
  union sigval sval
)
{
  qcril_data_call_info_tbl_type *info_tbl_ptr = sval.sival_ptr;
  dsi_call_param_value_t    partial_retry;

  QCRIL_LOG_FUNC_ENTRY();

  /* Lock the mutex before cleanup of info_tbl */
  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

  if (!VALIDATE_LOCAL_DATA_OBJ(info_tbl_ptr))
  {
    QCRIL_LOG_ERROR( "invalid info_tbl_ptr [%p] ",
                     (unsigned int *)info_tbl_ptr);
    goto err_label;
  }
  else if (FALSE == qcril_data_util_is_partial_retry_allowed(info_tbl_ptr))
  {
    info_tbl_ptr->partial_retry_count = 0;
    goto err_label;
  }

  /* If the timer hasn't been canceled right after timing out */
  if (QCRIL_DATA_INVALID_TIMERID != info_tbl_ptr->retry_timer_id)
  {
    QCRIL_LOG_DEBUG ("partial retry handler for timer [%#x], cid [%d]",
                     info_tbl_ptr->retry_timer_id,
                     info_tbl_ptr->cid);

    /* Let DSI library know that this is a partial retry */
    partial_retry.buf_val = NULL;
    partial_retry.num_val = TRUE;

    if ( ( dsi_set_data_call_param( info_tbl_ptr->dsi_hndl ,
                                    DSI_CALL_INFO_PARTIAL_RETRY,
                                    &partial_retry) ) != DSI_SUCCESS )
    {
      QCRIL_LOG_ERROR("unable to set partial retry call param, cid [%d]",
                      info_tbl_ptr->cid );
      info_tbl_ptr->is_partial_retry = FALSE;
      goto err_label;
    }

    /* Start data call */
    if ( ( dsi_start_data_call( info_tbl_ptr->dsi_hndl ) ) != DSI_SUCCESS )
    {
      QCRIL_LOG_ERROR("unable to start data call, cid [%d]",
                      info_tbl_ptr->cid );
      info_tbl_ptr->is_partial_retry = FALSE;
      qcril_data_util_schedule_partial_retry_attempt(info_tbl_ptr, TRUE, TRUE);
      goto err_label;
    }
  }

  info_tbl_ptr->is_partial_retry = TRUE;
  qcril_data_start_response_timer(info_tbl_ptr);

err_label:
  /* Unock the mutex of info_tbl */
  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);

  QCRIL_LOG_FUNC_RETURN();
}


/*===========================================================================

  FUNCTION:  qcril_data_request_deactivate_data_call

===========================================================================*/
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
)
{
  qcril_instance_id_e_type  instance_id;
  qcril_modem_id_e_type     modem_id;
  int                       i;
  qcril_reqlist_u_type      u_info;
  qcril_reqlist_public_type reqlist_entry;
  int                       cid;
  qcril_evt_e_type          pend_req = DS_RIL_REQ_INVALID;
  const char               *cid_ptr;
  RIL_Errno                 call_end_cause = RIL_E_INTERNAL_ERR;
  IxErrnoType               stop_data_err = E_FAILURE;

  QCRIL_LOG_DEBUG( "%s", "qcril_data_request_deact_data_call: ENTRY" );

  QCRIL_DS_ASSERT( params_ptr != NULL, "validate params_ptr" );
  QCRIL_DS_ASSERT( ret_ptr    != NULL, "validate ret_ptr" );
  if ( ( params_ptr == NULL ) || ( ret_ptr == NULL ) )
  {
    QCRIL_LOG_ERROR("%s", "BAD input");
    goto err_bad_input;
  }

  instance_id = params_ptr->instance_id;
  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );
  if (instance_id >= QCRIL_MAX_INSTANCE_ID)
  {
    goto err_bad_input;
  }

  modem_id = params_ptr->modem_id;
  QCRIL_ASSERT( modem_id < QCRIL_MAX_MODEM_ID );
  if (modem_id >= QCRIL_MAX_MODEM_ID)
  {
    goto err_bad_input;
  }

  cid_ptr  = ((const char **)params_ptr->data)[0];

  memset( &u_info, 0, sizeof( qcril_reqlist_u_type ) );

  cid    = strtol( cid_ptr, NULL, 10 );

  QCRIL_LOG_DEBUG( "RIL says CID [%d], len [%d]",
                   cid, params_ptr->datalen );

  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

  for( i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++ )
  {
    QCRIL_LOG_DEBUG("info_tbl CID [%d], index [%d]", info_tbl[i].cid, i);

    if( ( VALIDATE_LOCAL_DATA_OBJ( &info_tbl[i] ) ) && ( info_tbl[i].cid == cid ) )
    {
      QCRIL_LOG_DEBUG("found matching CID [%d], index [%d]", cid, i);
      break;
    }
  }/* for() */

  if ( i == MAX_CONCURRENT_UMTS_DATA_CALLS )
  {
    QCRIL_LOG_ERROR( "no valid CID [%d] match found", cid );
    call_end_cause = RIL_E_INVALID_CALL_ID;
    goto err_label;
  }

  /* Check whether REQ is pending */
  if ( qcril_data_util_is_req_pending( &info_tbl[i], &pend_req ) )
  {
    QCRIL_LOG_INFO( "UNEXPECTED RIL REQ pend [%s], cid [%d], index [%d]",
                    qcril_log_lookup_event_name( pend_req ), info_tbl[i].cid, info_tbl[i].index );
    /* if deactivate request is already pending, ignore another one */
    if (pend_req == QCRIL_EVT_RIL_REQUEST_DEACTIVATE_DATA_CALL)
    {
        QCRIL_LOG_INFO("QCRIL_EVT_RIL_REQUEST_DEACTIVATE_DATA_CALL already pending, cid [%d], index [%d]",
                          info_tbl[i].cid, info_tbl[i].index);
    }
    call_end_cause = RIL_E_INVALID_STATE;
    goto err_label;
  }

  /* Stop the setup data call response timer if it is running */
  qcril_data_util_stop_timer(&info_tbl[i].timer_id);

  /* Stop the retry timer if it is running */
  qcril_data_util_stop_timer(&info_tbl[i].retry_timer_id);

  /*----------------------------------------------------------------------
    Call deactivation
  ----------------------------------------------------------------------*/
  info_tbl[i].instance_id = instance_id;
  info_tbl[i].modem_id    = modem_id;
  info_tbl[i].pend_tok    = params_ptr->t;
  info_tbl[i].pend_req    = QCRIL_EVT_RIL_REQUEST_DEACTIVATE_DATA_CALL;
  QCRIL_LOG_INFO( "DEBUG:  %s step %d - info_tbl_ptr->pend_tok[0x%x] info_tbl_ptr->pend_req[0x%X]",
                  __func__, 0, info_tbl[i].pend_tok, info_tbl[i].pend_req );

  /* Update the dsi_cb_tbl[] entry with the new state information */
  qcril_data_update_dsi_cb_tbl_entry(&dsi_cb_tbl[i],
                                     instance_id,
                                     modem_id,
                                     params_ptr->t,
                                     dsi_cb_tbl[i].dsi_hndl,
                                     dsi_cb_tbl[i].info_tbl_ptr);

  /* Insert local info tbl ref in reqlist */

  u_info.ds.info = ( void *) &info_tbl[i];

  qcril_reqlist_default_entry( params_ptr->t,
                               QCRIL_EVT_RIL_REQUEST_DEACTIVATE_DATA_CALL,
                               modem_id,
                               QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                               QCRIL_EVT_DATA_EVENT_CALLBACK,
                               &u_info,
                               &reqlist_entry );
  if ( qcril_reqlist_new( instance_id, &reqlist_entry ) != E_SUCCESS )
  {
    /* Fail to add entry */
    QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
    return;
  }

  /* Stop data call */
  stop_data_err = qcril_data_stop_data_call(&info_tbl[i]);
  if (E_SUCCESS != stop_data_err)
  {
    /* Cleanup call state */
    if(E_INVALID_ARG == stop_data_err)
    {
      call_end_cause = RIL_E_INVALID_ARGUMENTS;
    }
    else
    {
      call_end_cause = RIL_E_OEM_ERROR_3;
    }
    qcril_data_cleanup_call_state( &info_tbl[i] );
    goto err_label;
  }

  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  QCRIL_LOG_DEBUG("%s", "qcril_data_request_deact_data_call: EXIT with suc");
  return;

err_label:
  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  qcril_data_response_generic_failure( instance_id, params_ptr->t, QCRIL_EVT_RIL_REQUEST_DEACTIVATE_DATA_CALL,
                                         call_end_cause);
  QCRIL_LOG_DEBUG("%s", "respond to QCRIL as generic failure");

err_bad_input:
  QCRIL_LOG_ERROR("%s", "qcril_data_request_deact_data_call: EXIT with err");
  return;

} /* qcril_data_request_deactivate_data_call() */



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
)
{
  qcril_instance_id_e_type      instance_id;
  qcril_data_call_response_type response;

  QCRIL_LOG_VERBOSE( "%s", "qcril_data_request_last_data_call_fail_cause: ENTRY" );

  QCRIL_DS_ASSERT( params_ptr != NULL, "validate params_ptr" );
  QCRIL_DS_ASSERT( ret_ptr    != NULL, "validate ret_ptr" );
  if ( ( params_ptr == NULL ) || ( ret_ptr == NULL ) )
  {
    QCRIL_LOG_ERROR("%s", "BAD input");
    goto err_bad_input;
  }

  instance_id = params_ptr->instance_id;
  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );
  if (instance_id >= QCRIL_MAX_INSTANCE_ID)
  {
    goto err_bad_input;
  }

  memset( &response, 0, sizeof ( qcril_data_call_response_type ) );

  response.cause_code = last_call_end_reason;
  response.size = sizeof( response.cause_code );

  QCRIL_LOG_VERBOSE( "send cause code [%u], size [%d] ",
                     response.cause_code, response.size );

  qcril_data_response_success( instance_id,
                               params_ptr->t,
                               QCRIL_EVT_RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE,
                               (void *) &(response.cause_code),
                               response.size );

  QCRIL_LOG_DEBUG("%s", "qcril_data_request_last_data_call_fail_cause: EXIT with suc");
  return;

//err_label:
  QCRIL_LOG_DEBUG("%s", "respond to QCRIL as generic failure");
  qcril_data_response_generic_failure( instance_id, params_ptr->t, QCRIL_EVT_RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE,
                                        RIL_E_INTERNAL_ERR);
err_bad_input:
  QCRIL_LOG_ERROR("%s", "qcril_data_request_last_data_fail_cause: EXIT with err");
  return;

} /* qcril_data_request_last_data_call_fail_cause() */



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
void qcril_data_request_data_call_list
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  qcril_instance_id_e_type      instance_id;
  qcril_data_call_response_type response;

  QCRIL_DS_ASSERT( params_ptr != NULL, "validate params_ptr" );
  QCRIL_DS_ASSERT( ret_ptr    != NULL, "validate ret_ptr" );
  if ( ( params_ptr == NULL ) || ( ret_ptr == NULL ) )
  {
    QCRIL_LOG_ERROR("%s", "BAD input");
    goto err_bad_input;
  }

  instance_id = params_ptr->instance_id;
  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );
  if (instance_id >= QCRIL_MAX_INSTANCE_ID)
  {
    goto err_bad_input;
  }

  memset( &response, 0, sizeof( qcril_data_call_response_type ) );

  QCRIL_LOG_DEBUG( "%s", "qcril_data_request_data_call_list: ENTRY" );

  qcril_data_get_active_call_list((void**)&(response.list), &response.size);

  QCRIL_DS_LOG_DBG_MEM( "response", response.list );

  QCRIL_LOG_DEBUG( "len [%d]", response.size );

  qcril_data_response_success( instance_id,
                               params_ptr->t,
                               QCRIL_EVT_RIL_REQUEST_DATA_CALL_LIST,
                               (void *) response.list,
                               response.size );

  /* free memory */
  QCRIL_DATA_RELEASE_STORAGE(response.list);

  QCRIL_LOG_DEBUG("%s", "qcril_data_request_data_call_list: EXIT with suc");
  return;

//err_label:
  qcril_data_response_generic_failure( instance_id, params_ptr->t, QCRIL_EVT_RIL_REQUEST_DATA_CALL_LIST,
                                      RIL_E_INTERNAL_ERR);
  QCRIL_LOG_DEBUG("%s", "respond to QCRIL as generic failure");
err_bad_input:
  QCRIL_LOG_ERROR("%s", "qcril_data_request_data_call_list: EXIT with err");
  return;

} /* qcril_data_request_data_call_list() */



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
)
{
  qcril_instance_id_e_type      instance_id;
  qcril_data_call_response_type response;
  int rc;
  int ret = FAILURE;
  qmi_client_type qmi_wds_omh_hndl = QCRIL_DATA_HNDL_INVALID;
  /*get_profile_list() and query_profile() related*/
  wds_profile_type_enum_v01 profile_tech;
  unsigned int num_elements_expected = QCRIL_DATA_NUM_OMH_PROFILES_EXPECTED;
  wds_profile_info_type_v01 result_list[QCRIL_DATA_NUM_OMH_PROFILES_EXPECTED];
  RIL_Errno call_end_cause;

  unsigned long app_type;
  /*-----------------------------------------------------------------------*/
  if ( ( params_ptr == NULL ) || ( ret_ptr == NULL ) )
  {
    QCRIL_LOG_ERROR("%s", "BAD input");
    return;
  }
  instance_id = params_ptr->instance_id;
  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );
  /*-----------------------------------------------------------------------*/

  memset( &response, 0, sizeof( qcril_data_call_response_type ) );

  QCRIL_LOG_DEBUG( "%s", "qcril_data_request_omh_profile_info: ENTRY" );

  /* Both RIL and QMI app types match the RUIM spec,
   * so no need of any mapping here, before calling the QMI API.
   */
  app_type = *((int *)params_ptr->data);

  QCRIL_LOG_DEBUG( "qcril_data_request_omh_profile_info: "
                         "RIL provided app type of %d",(int)app_type );

  /*get qmi wds cilent handle*/
  if(QMI_NO_ERR != (rc = qcril_data_qmi_wds_init_qmi_client(default_qmi_port,
                                                            NULL,
                                                            global_subs_id,
                                                            &qmi_wds_omh_hndl)))
  {
    QCRIL_LOG_ERROR("qcril_data_qmi_wds_init_qmi_client failed with rc [%d]", rc);
    call_end_cause = RIL_E_OEM_ERROR_1;
    goto bail;
  }

  QCRIL_LOG_DEBUG("qmi_wds_omh_hndl [%d] initialized", qmi_wds_omh_hndl);

  memset(&result_list, 0, sizeof(result_list) );
  profile_tech = WDS_PROFILE_TYPE_3GPP2_V01;

  /* query cdma profile id. */
  if(E_SUCCESS != qcril_data_qmi_wds_get_profile_list(qmi_wds_omh_hndl,
                                                      profile_tech,
                                                      &num_elements_expected,
                                                      result_list))
  {
    QCRIL_LOG_ERROR("%s", "qcril_data_qmi_wds_get_profile_list failed");
    call_end_cause = RIL_E_OEM_ERROR_3;
    goto release_qmi_resources;
  }
  else
  {
    QCRIL_LOG_DEBUG("qcril_data_qmi_wds_get_profile_list for profile_tech [%d] " \
                    "returned [%d] profile ids",
                    profile_tech, num_elements_expected);

    if (num_elements_expected == 0)
    {
      qcril_data_response_success( instance_id,
                                   params_ptr->t,
                                   params_ptr->event_id,
                                   NULL,
                                   0);
      ret = SUCCESS;
      goto release_qmi_resources;
    }
    else if (num_elements_expected > 0)
    {
      unsigned int i;
      wds_profile_identifier_type_v01 prof_id;
      wds_get_profile_settings_resp_msg_v01 prof_params;

      for (i = 0; i < num_elements_expected; i++)
      {
        prof_id.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
        prof_id.profile_index = result_list[i].profile_index;
        if(E_SUCCESS != qcril_data_qmi_wds_query_profile(qmi_wds_omh_hndl,
                                                         &prof_id,
                                                         &prof_params))
        {
          QCRIL_LOG_ERROR("query_profile for profile %d failed", prof_id.profile_index);
          call_end_cause = RIL_E_OEM_ERROR_3;
          goto release_qmi_resources;
        }
        else
        {
          if (prof_params.app_priority_valid)
          {
            response.omh_profile_info.profile_info[response.
              omh_profile_info.num_profiles].profileId =
              result_list[i].profile_index;
            response.omh_profile_info.profile_info[response.
              omh_profile_info.num_profiles].priority =
              prof_params.app_priority;
            response.omh_profile_info.num_profiles++;
          }
          else
          {
            QCRIL_LOG_ERROR("App priority is not set in the profile %d",
                             result_list[i].profile_index);
            continue;

          }
        }
      }/*for*/

      /*Response is already populated. Now send it.*/
      response.size = sizeof(response.omh_profile_info);

      qcril_data_response_success( instance_id,
                                   params_ptr->t,
                                   params_ptr->event_id,
                                   (void *) ( &(response.omh_profile_info) ),
                                   response.size );

    }/*else-if*/
  }/*else*/

  ret = SUCCESS;

release_qmi_resources:
  if (qmi_wds_omh_hndl != QCRIL_DATA_HNDL_INVALID)
  {
    if(E_SUCCESS != qcril_data_qmi_wds_release_qmi_client(qmi_wds_omh_hndl))
    {
      QCRIL_LOG_ERROR("couldn't release qmi wds hndl [%p] ", qmi_wds_omh_hndl);
    }
  }

bail:
  if (ret == SUCCESS)
  {
    QCRIL_LOG_DEBUG("%s", "qcril_data_request_omh_profile_info: EXIT with success");
  }
  else if (ret == FAILURE)
  {
    qcril_data_response_generic_failure(instance_id,
                                        params_ptr->t,
                                        params_ptr->event_id,
                                        call_end_cause);

    QCRIL_LOG_DEBUG("%s", "respond to RIL with generic failure \n");
    QCRIL_LOG_ERROR("%s", "qcril_data_request_omh_profile_info: EXIT with err \n");
  }
  return;
}


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
)
{

#define IMS_CLASS 1
#define ADMN_CLASS 2
#define INET_CLASS 3
#define APP_CLASS 4

  const char               *ril_apn;
  const char               *ril_user;
  const char               *ril_pass;
  const char               *ril_ipfamily = NULL;
  const char               *ril_ip_roaming = NULL;
  const char               *ril_auth_type = NULL;
  int                      ril_maxConnstime;
  int                      ril_waitTime;
  int                      ril_maxConns;
  int                      ril_enabled;
  unsigned int             i=0;
  unsigned int             j=0;
  int                      rc;
  unsigned int             umts_profile_id_list[QDP_NUM_PROFILES_EXPECTED_MAX];
  uint8                    umts_profile_list_len;
  unsigned int             cdma_profile_id_list[QDP_NUM_PROFILES_EXPECTED_MAX];
  uint8                    cdma_profile_list_len;

  wds_profile_auth_protocol_enum_v01  auth_type;

  qcril_request_resp_params_type  resp;

  /* stores QDP profile PDN types */
  qdp_profile_pdn_type     umts_profile_pdn_type;
  qdp_profile_pdn_type     cdma_profile_pdn_type;
  qdp_error_info_t         error_info;
  uint8 apn_class;

  qcril_instance_id_e_type  instance_id;
  qcril_modem_id_e_type     modem_id;

  const char * qdp_params[QDP_RIL_PARAM_MAX];

  wds_modify_profile_settings_req_msg_v01  new_p_params;
  qdp_tech_t                  tech_type;

  QCRIL_LOG_DEBUG( "%s", "qcril_data_request_set_data_profile: ENTRY" );

  QCRIL_DS_ASSERT( params_ptr != NULL, "validate params_ptr" );
  QCRIL_DS_ASSERT( ret_ptr != NULL, "validate ret_ptr" );


  if ( ( params_ptr == NULL ) || ( ret_ptr == NULL ) )
  {
    QCRIL_LOG_ERROR( "BAD input, params_ptr [%p], ret_ptr [%p], datalen [%d]",
            (unsigned int *)params_ptr, (unsigned int *)ret_ptr,
            params_ptr ? params_ptr->datalen : 0 );
    goto err_bad_input;
  }

  instance_id = params_ptr->instance_id;
  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );
  if (instance_id >= QCRIL_MAX_INSTANCE_ID)
  {
    goto err_bad_input;
  }

  modem_id = params_ptr->modem_id;
  QCRIL_ASSERT( modem_id < QCRIL_MAX_MODEM_ID );
  if (modem_id >= QCRIL_MAX_MODEM_ID)
  {
    goto err_bad_input;
  }

  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

#if (QCRIL_RIL_VERSION >= 15)
  RIL_DataProfileInfo_v15 **data_profile_arr = (RIL_DataProfileInfo_v15 **)params_ptr->data;
#else
  RIL_DataProfileInfo **data_profile_arr = (RIL_DataProfileInfo **)params_ptr->data;
#endif

  QCRIL_LOG_DEBUG("params_ptr->datalen [%d] ",params_ptr->datalen);

  do
  {

#if (QCRIL_RIL_VERSION >= 15)
    RIL_DataProfileInfo_v15 *data_profile_info = data_profile_arr[i];
#else
    RIL_DataProfileInfo *data_profile_info = data_profile_arr[i];
#endif

    if (NULL == data_profile_info)
    {
      QCRIL_LOG_ERROR("%s","Invalid Input");
      goto err_bad_input;
    }

    /* Convert Profile ID to APN class as we expect the same from Telephony */
    switch(data_profile_info->profileId)
    {
      case 0:
       apn_class = INET_CLASS;
       break;
      case 2:
       apn_class = IMS_CLASS;
       break;
      case 3:
       apn_class = ADMN_CLASS;
       break;
      case 4:
       apn_class = APP_CLASS;
       break;
      default:
        QCRIL_LOG_ERROR("%s","Invalid Input");
        goto err_bad_input;
    }

    ril_apn       = data_profile_info->apn;
    ril_ipfamily  = data_profile_info->protocol;
#if (QCRIL_RIL_VERSION >= 15)
    ril_ip_roaming = data_profile_info->roamingProtocol;
#endif
    auth_type     = (wds_profile_auth_protocol_enum_v01)data_profile_info->authType;
    ril_user      = data_profile_info->user;
    ril_pass      = data_profile_info->password;

    ril_maxConns  = data_profile_info->maxConns;
    ril_maxConnstime = data_profile_info->maxConnsTime;
    ril_waitTime = data_profile_info->waitTime;
    ril_enabled = data_profile_info->enabled;

    memset(qdp_params, 0, sizeof(qdp_params));

    if (NULL == ril_apn || 0 == strlen(ril_apn))
    {
      QCRIL_LOG_ERROR("%s","ril_apn provided is NULL");
    }
    else if (strlen(ril_apn) > QMI_WDS_APN_NAME_MAX_V01)
    {
      QCRIL_LOG_ERROR("RIL provided invalid APN [%s] "
                      "APN length [%d] exceeds max allowed [%d]",
                      ril_apn, strlen(ril_apn), QMI_WDS_APN_NAME_MAX_V01);
      goto err_bad_input;
    }
    qdp_params[QDP_RIL_APN] = ril_apn;

    QCRIL_LOG_DEBUG("qdp_param APN = [%s]", qdp_params[QDP_RIL_APN]);

    /* prepare ip family */
    if (ril_ipfamily != NULL && strlen(ril_ipfamily) > 0)
    {
      if (strlen(ril_ipfamily) > QCRIL_IP_FAMILY_STR_MAX)
      {
        QCRIL_LOG_ERROR("RIL provided invalid ip family [%s] "
                        "ip family length [%d] exceeds max allowed [%d]",
                        ril_ipfamily, strlen(ril_ipfamily), QCRIL_IP_FAMILY_STR_MAX);
        goto err_bad_input;
      }

      qdp_params[QDP_RIL_IP_FAMILY] = ril_ipfamily;

      QCRIL_LOG_DEBUG("qdp param IP_FAMILY = [%s]", qdp_params[QDP_RIL_IP_FAMILY]);
    }

    /* prepare roaming ip */
    if (ril_ip_roaming != NULL && strlen(ril_ip_roaming) > 0)
    {
      if (strlen(ril_ip_roaming) > QCRIL_IP_FAMILY_STR_MAX)
      {
        QCRIL_LOG_ERROR("RIL provided invalid roaming ip [%s] "
                        "roaming ip length [%d] exceeds max allowed [%d]",
                        ril_ip_roaming, strlen(ril_ip_roaming), QCRIL_IP_FAMILY_STR_MAX);
        goto err_bad_input;
      }

      qdp_params[QDP_RIL_IP_ROAMING] = ril_ip_roaming;

      QCRIL_LOG_DEBUG("qdp param IP_ROAMING = [%s]", qdp_params[QDP_RIL_IP_ROAMING]);
    }

    /* prepare auth */
    switch (auth_type)
    {
    case WDS_PROFILE_AUTH_PROTOCOL_NONE_V01:
        ril_auth_type = QDP_RIL_PAP_CHAP_NOT_ALLOWED;
        break;
    case WDS_PROFILE_AUTH_PROTOCOL_PAP_V01:
        ril_auth_type = QDP_RIL_PAP_ONLY_ALLOWED;
        break;
    case WDS_PROFILE_AUTH_PROTOCOL_CHAP_V01:
        ril_auth_type = QDP_RIL_CHAP_ONLY_ALLOWED;
        break;
    case WDS_PROFILE_AUTH_PROTOCOL_PAP_CHAP_V01:
        ril_auth_type = QDP_RIL_PAP_CHAP_BOTH_ALLOWED;
        break;
    default:
        QCRIL_LOG_DEBUG("Unknown auth_type [%d]",
                        auth_type);
        goto err_bad_input;
    }

    qdp_params[QDP_RIL_AUTH] = ril_auth_type;

    QCRIL_LOG_DEBUG("qdp param RIL_AUTH = [%s]", qdp_params[QDP_RIL_AUTH]);

    /* Prepare username */
    if (ril_user != NULL && strlen(ril_user) > 0)
    {
      if (strlen(ril_user) > QMI_WDS_USER_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("RIL provided username exceeds max allowed [%d]",
                        QMI_WDS_USER_NAME_MAX_V01);
      }
      else
      {
        qdp_params[QDP_RIL_NAI] = ril_user;
        QCRIL_LOG_DEBUG("qdp param USERNAME = [%s]", qdp_params[QDP_RIL_NAI]);
      }
    }

    /* prepare password */
    if (ril_pass != NULL && strlen(ril_pass) > 0)
    {
      if (strlen(ril_pass) > QMI_WDS_PASSWORD_MAX_V01)
      {
        QCRIL_LOG_ERROR("RIL provided password exceeds max allowed [%d]",
                        QMI_WDS_PASSWORD_MAX_V01);
      }
      else
      {
        qdp_params[QDP_RIL_PASSWORD] = ril_pass;
        QCRIL_LOG_DEBUG("qdp param PASSWORD = [%s]", qdp_params[QDP_RIL_PASSWORD]);
      }
    }

    /* Look up for a profile that matches APN class */
    rc = qdp_profile_look_up_by_param( qdp_params,
                                       QDP_RIL_CLASS,
                                       apn_class,
                                       &umts_profile_id_list[0],
                                       &umts_profile_list_len,
                                       &umts_profile_pdn_type,
                                       &cdma_profile_id_list[0],
                                       &cdma_profile_list_len,
                                       &cdma_profile_pdn_type,
                                       &tech_type,
                                       &error_info );

    for(j = 0; j < umts_profile_list_len; j++)
    {
      int qmi_err_code;

      memset(&new_p_params, 0, sizeof(new_p_params));

      if(umts_profile_id_list[j] == QDP_INVALID_PROFILE)
      {
        break;
      }
      if(tech_type == QDP_3GPP)
      {
        qdp_3gpp_profile_update_params(qdp_params,
                                       umts_profile_id_list[j],
                                       &new_p_params);

         new_p_params.apn_class_valid = TRUE;
         new_p_params.apn_class = apn_class;

         if ( ril_maxConnstime > 0 ) {
           QCRIL_LOG_DEBUG("Filling in maxConnTimer param received through RIL PARAMS");
           new_p_params.max_pdn_conn_timer_valid = TRUE;
           new_p_params.max_pdn_conn_timer = ril_maxConnstime;
         }

         if ( ril_maxConns > 0 ) {
           QCRIL_LOG_DEBUG("Filling in maxConn param received through RIL PARAMS");
           new_p_params.max_pdn_conn_per_block_valid = TRUE;
           new_p_params.max_pdn_conn_per_block = ril_maxConns;
         }

         if ( ril_waitTime > 0 ) {
           QCRIL_LOG_DEBUG("Filling in waitTimer param received through RIL PARAMS");
           new_p_params.pdn_req_wait_interval_valid = TRUE;
           new_p_params.pdn_req_wait_interval = ril_waitTime;
         }

         new_p_params.apn_disabled_flag_valid = TRUE;
         new_p_params.apn_disabled_flag = (ril_enabled == 0)? 1:0;

     }
     if (QDP_SUCCESS != qdp_profile_update_ex( &new_p_params,
                                               &qmi_err_code ))
     {
       QCRIL_LOG_ERROR("Profile update failed for profile [%d], err [%d]",
                       umts_profile_id_list[j], qmi_err_code);
     }
    }

    for(j = 0; j < cdma_profile_list_len; j++)
    {
      int qmi_err_code;

       memset(&new_p_params, 0, sizeof(new_p_params));

      if(cdma_profile_id_list[j] == QDP_INVALID_PROFILE)
      {
        break;
      }

       qdp_3gpp2_profile_update_params(qdp_params,
                                       cdma_profile_id_list[j],
                                       &new_p_params);

       new_p_params.apn_class_3gpp2_valid = TRUE;
       new_p_params.apn_class_3gpp2 = apn_class;

       new_p_params.apn_enabled_3gpp2_valid = TRUE;
       new_p_params.apn_enabled_3gpp2 = ril_enabled;

       if (QDP_SUCCESS != qdp_profile_update_ex( &new_p_params,
                                                 &qmi_err_code ))
       {
         QCRIL_LOG_ERROR("Profile update failed for profile [%d], err [%d]",
                          cdma_profile_id_list[j], qmi_err_code);
       }
    }

    i++;

  }while(i < (params_ptr->datalen/sizeof(RIL_DataProfileInfo *)));

  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);

  QCRIL_LOG_DEBUG("%s", "qcril_data_request_set_data_profile: EXIT with SUCCESS");

  qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                     params_ptr->t,
                                     params_ptr->event_id,
                                     RIL_E_SUCCESS,
                                     &resp );

  return qcril_send_request_response( &resp );


err_bad_input:

  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);

  QCRIL_LOG_ERROR("%s", "qcril_data_request_setup_data_call: EXIT with FAILURE");

    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                       params_ptr->t,
                                       params_ptr->event_id,
                                       RIL_E_OEM_ERROR_3,
                                       &resp );

    return qcril_send_request_response( &resp );

}

#if (QCRIL_RIL_VERSION >= 15)

/*===========================================================================

  FUNCTION:  qcrili_data_is_valid_callid

===========================================================================*/
/*!
    @brief
    Check if given call id/context id is valid

    @return
    TRUE if found
    FALSE otherwise
*/
/*=========================================================================*/
static boolean qcrili_data_is_valid_callid(int cid)
{
  int i = 0;
  for(; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++ )
  {
    QCRIL_LOG_DEBUG("<---info_tbl CID [%d], index [%d]--->", info_tbl[i].cid, i);
    if( ( VALIDATE_LOCAL_DATA_OBJ( &info_tbl[i] ) ) && ( info_tbl[i].cid == cid ) )
    {
      QCRIL_LOG_DEBUG("found matching CID [%d], index [%d]", cid, i);
      break;
    }
  }
  if ( i == MAX_CONCURRENT_UMTS_DATA_CALLS )
  {
    QCRIL_LOG_ERROR( "not a valid CID [%d]", cid );
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

  FUNCTION:  qcrili_data_get_apn_name_for_cid

===========================================================================*/
/*!
    @brief
    Find apn name for given call id/context id

    @return
    int - cid
*/
/*=========================================================================*/
static int qcrili_data_get_apn_name_for_cid(int cid, char *apn_name, size_t len)
{
  int i = 0;
  int rc = E_NO_ENTRY;
  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);
  for( i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++ )
  {
    QCRIL_LOG_DEBUG("info_tbl CID [%d], index [%d]", info_tbl[i].cid, i);

    if( ( VALIDATE_LOCAL_DATA_OBJ( &info_tbl[i] ) ) && ( info_tbl[i].cid == cid ) )
    {
      QCRIL_LOG_DEBUG("found matching CID [%d], index [%d]", cid, i);
      strlcpy(apn_name, info_tbl[i].call_params->ril_apn, len);
      rc = E_SUCCESS;
      break;
    }
  }
  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  if ( i == MAX_CONCURRENT_UMTS_DATA_CALLS )
  {
    QCRIL_LOG_ERROR( "no valid CID [%d] match found", cid );
  }
  return rc;
}

/*===========================================================================

  FUNCTION:  qcrili_data_get_apn_name_for_cid

===========================================================================*/
/*!
    @brief
    Find apn name for given call id/context id

    @return
    int - cid
*/
/*=========================================================================*/
static void qcrili_data_generate_start_keepalive_request(const RIL_KeepaliveRequest* in,
  wds_modem_assisted_ka_start_req_msg_v01* out) {
  memset(out, 0, sizeof(wds_modem_assisted_ka_start_req_msg_v01));
  // we only support the one type right now
  out->keep_alive_type = WDS_KEEPALIVE_TYPE_NAT_V01;
  if (in->type == NATT_IPV4) {
    // data is in network byte order (big-endian), convert to host order (little-endian)
    uint32_t src = 0;
    uint32_t dest = 0;
    memcpy(&src, in->sourceAddress, sizeof(uint32_t));
    memcpy(&dest, in->destinationAddress, sizeof(uint32_t));
    out->source_ipv4_address = ntohl(src);
    out->dest_ipv4_address = ntohl(dest);
    out->dest_ipv4_address_valid = 1;
    out->source_ipv4_address_valid = 1;
  } else {
    // no conversion needed for IPv6
    memcpy(&out->source_ipv6_address, &in->sourceAddress, QMI_WDS_IPV6_ADDR_LEN_V01*sizeof(uint8_t));
    memcpy(&out->dest_ipv6_address, &in->destinationAddress, QMI_WDS_IPV6_ADDR_LEN_V01*sizeof(uint8_t));
    out->dest_ipv6_address_valid = 1;
    out->source_ipv6_address_valid = 1;
  }
  if (in->maxKeepaliveIntervalMillis > 0) {
    out->timer_value = in->maxKeepaliveIntervalMillis;
    out->timer_value_valid = 1;
  }
  out->dest_port = (uint16_t) in->destinationPort;
  out->dest_port_valid = 1;
  out->source_port = (uint16_t) in->sourcePort;
  out->source_port_valid = 1;
  char apn_name[QMI_WDS_APN_NAME_MAX_V01] = {0};
  int rc = qcrili_data_get_apn_name_for_cid(in->cid, apn_name, QMI_WDS_APN_NAME_MAX_V01);
  if (rc == E_SUCCESS) {
    strlcpy(out->apn_name, apn_name, QMI_WDS_APN_NAME_MAX_V01);
    out->apn_name_valid = 1;
  }
}

/*===========================================================================

  FUNCTION:  qcrili_data_generate_stop_keepalive_request

===========================================================================*/
/*!
    @brief
    Helper function generate message

    @return
    void
*/
/*=========================================================================*/
static void qcrili_data_generate_stop_keepalive_request(uint32_t in,
  wds_modem_assisted_ka_stop_req_msg_v01* out) {
  memset(out, 0, sizeof(wds_modem_assisted_ka_stop_req_msg_v01));
  out->keep_alive_handle = in;
}

/*===========================================================================

  FUNCTION:  qcrili_data_match_addr

===========================================================================*/
/*!
    @brief
    Helper function to match addresses

    @return
    void
*/
/*=========================================================================*/
static boolean qcrili_data_match_addr(const char* a, const char *b, size_t len)
{
  if ((a == NULL) && (b == NULL)) return TRUE;
  if (((a == NULL) && (b != NULL)) ||
     ((a != NULL) && (b == NULL))) return FALSE;
  return (strncmp(a, b, len) == 0)? TRUE:FALSE;
}

/*===========================================================================

  FUNCTION:  qcrili_data_match_port

===========================================================================*/
/*!
    @brief
    Helper function to match ports

    @return
    void
*/
/*=========================================================================*/
static boolean qcrili_data_match_port(int a, int b)
{
  return (a == b)? TRUE:FALSE;
}

/*===========================================================================

  FUNCTION:  qcrili_data_match_keepalive_type

===========================================================================*/
/*!
    @brief
    Helper function to match types

    @return
    void
*/
/*=========================================================================*/
static boolean qcrili_data_match_keepalive_type(RIL_KeepaliveType a, RIL_KeepaliveType b)
{
  return (a == b)? TRUE:FALSE;
}

/*===========================================================================

  FUNCTION:  qcrili_data_ma_ka_duplicate_req_detected

===========================================================================*/
/*!
    @brief
    Check if an incoming request is a duplicate

    @return
    boolean
*/
/*=========================================================================*/
static boolean qcrili_data_ma_ka_duplicate_req_detected(const RIL_KeepaliveRequest* req)
{
  /*
  if the src addr/port and dest addr/port + the NAT type matches
  then we treat it as a duplicate request.
  */
  QCRIL_LOG_DEBUG("Checking for duplicate request");
  qcril_data_ma_ka_list_elem_t *cur = qcril_data_ma_ka_list_head;

  while (cur != NULL) {
    if (qcrili_data_match_addr(req->destinationAddress, cur->destinationAddress, MAX_INADDR_LEN) &&
      qcrili_data_match_port(req->destinationPort, cur->destinationPort) &&
      qcrili_data_match_addr(req->sourceAddress, cur->sourceAddress, MAX_INADDR_LEN) &&
      qcrili_data_match_port(req->sourcePort, cur->sourcePort) &&
      qcrili_data_match_keepalive_type(req->type, cur->type) &&
      req->cid == cur->cid)
    {
        QCRIL_LOG_DEBUG("matched for handle %d", cur->handle);
        return TRUE;
    }
    cur = cur->next;
  }
  return FALSE;
}

/*===========================================================================

  FUNCTION:  qcrili_data_ma_ka_bad_req_detected

===========================================================================*/
/*!
    @brief
    Check if an incoming request is a malformed

    @return
    boolean
*/
/*=========================================================================*/
static boolean qcrili_data_ma_ka_bad_req_detected(const RIL_KeepaliveRequest* req)
{
 // Check invalid cid, port range (max is 65535), etc
 if (!qcrili_data_is_valid_callid(req->cid) || (req->destinationPort > 65535)) {
  return TRUE;
 }
 return FALSE;
}

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
  )
{
  RIL_Errno ret = RIL_E_NO_RESOURCES;
  uint32_t handle = 0;

  if ((params_ptr == NULL) || (ret_ptr == NULL)) {
    QCRIL_LOG_ERROR("%s(): Invalid parameters", __func__);
    qcril_data_response_generic_failure(params_ptr->instance_id,
      params_ptr->t,
      params_ptr->event_id,
      RIL_E_INVALID_ARGUMENTS);
    return;
  }
  RIL_KeepaliveRequest *req = (RIL_KeepaliveRequest*)params_ptr->data;

  do {
    // Reject any duplicate or invalid request
    if (qcrili_data_ma_ka_duplicate_req_detected(req) || qcrili_data_ma_ka_bad_req_detected(req)) {
      QCRIL_LOG_ERROR("%s():Cannot start request, malformed or duplicate request detected!", __func__);
      ret = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    wds_modem_assisted_ka_start_req_msg_v01 qmi_req;
    qcrili_data_generate_start_keepalive_request(req, &qmi_req);
    // Send request to modem to initiate kepalive timer
    int rc = qcril_data_ma_ka_send_start_request(global_qcril_qmi_wds_hndl, &qmi_req, &handle);
    if (rc != E_SUCCESS) {
      break;
    }
    ret = RIL_E_SUCCESS;
  }while(0);

  if (ret != RIL_E_SUCCESS) {
    QCRIL_LOG_DEBUG("%s(): sending err = %d", __func__, ret);
    qcril_data_response_generic_failure(params_ptr->instance_id,
        params_ptr->t,
        params_ptr->event_id,
        ret);
  } else {
    qcril_data_ma_ka_list_elem_t *e = qcril_data_ma_ka_create_element(handle, params_ptr->t);
    if (e == NULL) {
      QCRIL_LOG_ERROR("%s(): Out of memory, aborting...", __func__);
      qcril_data_response_generic_failure(params_ptr->instance_id,
        params_ptr->t,
        params_ptr->event_id,
        ret);
    } else {
      e->type = req->type;
      strncpy(e->sourceAddress, req->sourceAddress, MAX_INADDR_LEN);
      e->sourcePort = req->sourcePort;
      strncpy(e->destinationAddress, req->destinationAddress, MAX_INADDR_LEN);
      e->destinationPort = req->destinationPort;
      e->cid = req->cid;
      e->status = QCRIL_DATA_KA_STATUS_REQUESTED;
      qcril_data_insert_ma_ka_elem(e);

	  QCRIL_LOG_DEBUG("Sending keepalive resp for handle %d", handle);
      qcril_data_ma_ka_send_succ_resp(e->handle,
        e->token,
        params_ptr->event_id,
        e->status);
    }
    QCRIL_LOG_DEBUG("Handled keepalive request");
  }
}

/*===========================================================================

  FUNCTION:  qcril_data_stop_modem_assist_keepalive

===========================================================================*/
/**
 * @brief      Send request to modem to stop kepalive timer
 *
 * @param[in]  params_ptr  The parameters pointer
 * @param      ret_ptr     The ret pointer
 */
void qcril_data_stop_modem_assist_keepalive(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
  )
{
  RIL_Errno ret = RIL_E_INTERNAL_ERR;
  do {
    if ((params_ptr == NULL) || (ret_ptr == NULL)) {
      QCRIL_LOG_ERROR("%s(): Invalid parameters", __func__);
      ret = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    uint32_t *handle = (uint32_t*)params_ptr->data;
    //validate handle before processing further
    if ((handle == NULL) || qcril_data_get_ma_ka_elem_by_handle(*handle) == NULL)
    {
      QCRIL_LOG_ERROR("%s(): Invalid parameters", __func__);
      ret = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    // try to clear up the cache
    qcril_data_ma_ka_delete_elem(*handle);

    int rc = qcril_data_stop_keepalive_for_handle(*handle, false);
    if (rc != E_SUCCESS) {
      break;
    }
    ret = RIL_E_SUCCESS;
  } while(0);

  if (ret != RIL_E_SUCCESS) {
    QCRIL_LOG_DEBUG("%s(): sending err = %d", __func__, ret);
    qcril_data_response_generic_failure(params_ptr->instance_id,
                                        params_ptr->t,
                                        params_ptr->event_id,
                                        ret);
  } else {
    QCRIL_LOG_DEBUG("%s(): send succ resp", __func__);
    qcril_request_resp_params_type  resp;
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                     params_ptr->t,
                                     params_ptr->event_id,
                                     RIL_E_SUCCESS,
                                     &resp );
    qcril_data_response_success(params_ptr->instance_id,
                                params_ptr->t,
                                params_ptr->event_id,
                                &resp,
                                sizeof(resp));
  }
}

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
)
{
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_DEBUG( "%s", "qcril_data_keepalive_event_hdlr: ENTRY" );
  /* Input Validation */
  if (params_ptr == NULL) {
    QCRIL_LOG_ERROR( "Invalid payload, bailing!");
    return;
  }

  wds_modem_assisted_ka_status_ind_msg_v01* data = ( wds_modem_assisted_ka_status_ind_msg_v01* )params_ptr->data;

  QCRIL_LOG_INFO("%s","QCRIL_EVT_DATA_KEEPALIVE_EVENT_CALLBACK arrived "
                 "in the qcril_event thread");
  qcril_data_handle_keepalive_ind(data->status, data->keep_alive_handle);

  QCRIL_LOG_DEBUG( "%s", "qcril_data_keepalive_event_hdlr: EXIT" );
}

void qcril_data_handle_keepalive_ind(wds_ka_result_type_enum_v01 status, uint32_t handle)
{
  RIL_KeepaliveStatusCode code;
  if (status == WDS_KEEP_ALIVE_RESULT_OK_V01) {
    QCRIL_LOG_DEBUG("Keep alive active for handle %d", handle);
    code = KEEPALIVE_ACTIVE;
  } else {
    QCRIL_LOG_DEBUG("Keep alive inactive for handle %d", handle);
    code = KEEPALIVE_INACTIVE;
  }

  qcril_data_ma_ka_send_unsol(handle, code);

  if (status != WDS_KEEP_ALIVE_RESULT_OK_V01) {
    qcril_data_ma_ka_delete_elem(handle);
  }
  else if (status == WDS_KEEP_ALIVE_RESULT_OK_V01) {
    qcril_data_ma_ka_list_elem_t *e = qcril_data_get_ma_ka_elem_by_handle(handle);
    if (e) {
      e->status = QCRIL_DATA_KA_STATUS_RUNNING;
    }
  }
}

/*===========================================================================

    FUNCTION: qcril_data_stop_keepalive_for_handle

===========================================================================*/
/*!
    @brief
    stop keepalive per handle, send UnSol if bSendUnsol is true

    @return
    boolean
*/
/*=========================================================================*/
int qcril_data_stop_keepalive_for_handle(uint32_t handle, bool bSendUnsol)
{
  wds_modem_assisted_ka_stop_req_msg_v01 qmi_req;

  qcrili_data_generate_stop_keepalive_request(handle, &qmi_req);
  int rc = qcril_data_ma_ka_send_stop_request(global_qcril_qmi_wds_hndl, &qmi_req);
  if (rc != E_SUCCESS) {
    QCRIL_LOG_DEBUG("%s():send stop for handle %d failure(%d)", __func__, handle, rc);
  }

  if(bSendUnsol){
    qcril_data_ma_ka_send_unsol(handle, KEEPALIVE_INACTIVE);
  }
  return rc;
}
#endif /* (QCRIL_RIL_VERSION >= 15) */

/*===========================================================================

    FUNCTION: qcrili_data_send_apn_info_qmi

    ===========================================================================*/
/*!
    @brief
    Internal function to send apn info request to modem

    @return
    None.
*/
/*=========================================================================*/
static void qcrili_data_send_apn_info_qmi(
  const char *apn_name,
  dsd_apn_type_enum_v01 apn_type)
{
  dsd_set_apn_info_req_msg_v01 data_setting_set_apn_param;
  dsd_set_apn_info_resp_msg_v01 data_setting_set_apn_param_resp;

  memset(&data_setting_set_apn_param, 0, sizeof(data_setting_set_apn_param));
  memset(&data_setting_set_apn_param_resp, 0,
    sizeof(dsd_set_apn_info_resp_msg_v01));
  strlcpy( &data_setting_set_apn_param.apn_info.apn_name[0], apn_name, QMI_DSD_MAX_APN_LEN_V01);

  // indicate that the apn_invalid flag is set
  data_setting_set_apn_param.apn_invalid_valid = FALSE;
  data_setting_set_apn_param.apn_info.apn_type = apn_type;

  if (global_qmi_dsd_hndl == NULL) {
    QCRIL_LOG_ERROR("%s(): NULL dsd handle!", __func__);
    return;
  }

  int rc = qmi_client_send_msg_sync(global_qmi_dsd_hndl,
                                  QMI_DSD_SET_APN_INFO_REQ_V01,
                                  (void *)&data_setting_set_apn_param,
                                  sizeof(data_setting_set_apn_param),
                                  (void *)&data_setting_set_apn_param_resp,
                                  sizeof(dsd_set_apn_info_resp_msg_v01),
                                  QCRIL_DATA_QMI_TIMEOUT);
  if (rc != QMI_NO_ERR) {
    QCRIL_LOG_ERROR(
      "%s(): Failed to send QMI_DSD_SET_APN_INFO_REQ_V01 for "
      " apn_name: %s, type: %d with rc = %d",
      __func__, apn_name, apn_type, rc);
  } else {
    QCRIL_LOG_DEBUG("%s(): Successfully sent QMI_DSD_SET_APN_INFO_REQ_V01 for type %d", __func__, apn_type)
  }
}

/*===========================================================================

    FUNCTION: qcril_data_device_shutdown

    ===========================================================================*/
/*!
    @brief
    device is being shutdown, notify to modem that wifi is turning off

    @return
    true  : success for triggering wifi switch off to modem
    false : failure for wifi switch off to modem
*/
/*=========================================================================*/
bool qcril_data_device_shutdown()
{
  qmi_client_error_type retVal = QMI_NO_ERR;
  dsd_notify_data_settings_req_msg_v01 dataSettingsReq;
  dsd_notify_data_settings_resp_msg_v01 dataSettingsRsp;

  memset(&dataSettingsReq, 0, sizeof(dsd_notify_data_settings_req_msg_v01));
  memset(&dataSettingsRsp, 0, sizeof(dsd_notify_data_settings_resp_msg_v01));

  dataSettingsReq.wifi_switch_valid = true;
  dataSettingsReq.wifi_switch = false;
  retVal = qmi_client_send_msg_sync(global_qmi_dsd_hndl,
                                    QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01,
                                    &dataSettingsReq,
                                    sizeof(dataSettingsReq),
                                    &dataSettingsRsp,
                                    sizeof(dataSettingsRsp),
                                    QCRIL_DATA_QMI_TIMEOUT);

  if (QMI_NO_ERR != retVal) {
    QCRIL_LOG_DEBUG("Sending of notify data setting failed retVal %d", retVal);
    if (QMI_SERVICE_ERR == retVal) {
      QCRIL_LOG_DEBUG("result %d error %d", dataSettingsRsp.resp.result, dataSettingsRsp.resp.error);
    }
    return false;
  }

  return true;
}
