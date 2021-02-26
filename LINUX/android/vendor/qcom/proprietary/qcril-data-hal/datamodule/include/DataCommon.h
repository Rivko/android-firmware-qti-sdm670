/**
* Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef __DATA_COMMON_H
#define __DATA_COMMON_H

#include "framework/legacy.h"
// ***** IMPORTANT *******
// NOTE: version.h always be included when using
// QCRIL_RIL_VERSION
// ********************
#include "modules/android/version.h"

/*! @brief Preferred Data Technology
*/
typedef enum
{
  QCRIL_DATA_PREF_DATA_TECH_INVALID = -1,
  QCRIL_DATA_PREF_DATA_TECH_UNKNOWN = 0,
  QCRIL_DATA_PREF_DATA_TECH_FIRST   = QCRIL_DATA_PREF_DATA_TECH_UNKNOWN,
  QCRIL_DATA_PREF_DATA_TECH_CDMA    = 1,
  QCRIL_DATA_PREF_DATA_TECH_EVDO    = 2,
  QCRIL_DATA_PREF_DATA_TECH_GSM     = 3,
  QCRIL_DATA_PREF_DATA_TECH_UMTS    = 4,
  QCRIL_DATA_PREF_DATA_TECH_EHRPD   = 5,
  QCRIL_DATA_PREF_DATA_TECH_LTE     = 6,
  QCRIL_DATA_PREF_DATA_TECH_TDSCDMA = 7,
  QCRIL_DATA_PREF_DATA_TECH_MAX
} qcril_data_pref_data_tech_e_type;

/* Event notifications */
typedef enum
{
  QCRIL_DATA_EVT_CALL_CONNECTED,
  QCRIL_DATA_EVT_CALL_PHYSLINK_UP,
  QCRIL_DATA_EVT_CALL_PHYSLINK_DOWN,
  QCRIL_DATA_EVT_CALL_RELEASED,
  QCRIL_DATA_EVT_MAX
} qcril_data_net_evt_t;

/*Switch to toggle Dormancy indications*/
typedef enum
{
  DORMANCY_INDICATIONS_MIN = 0x01,
  DORMANCY_INDICATIONS_OFF = DORMANCY_INDICATIONS_MIN,
  DORMANCY_INDICATIONS_ON  = 0x02,
  DORMANCY_INDICATIONS_MAX = DORMANCY_INDICATIONS_ON
}qcril_data_dormancy_ind_switch_type;

/* Switch to toggle data limited system status indications */
typedef enum
{
  LIMITED_SYS_INDICATIONS_MIN = 0x01,
  LIMITED_SYS_INDICATIONS_OFF = LIMITED_SYS_INDICATIONS_MIN,
  LIMITED_SYS_INDICATIONS_ON  = 0x02,
  LIMITED_SYS_INDICATIONS_MAX = LIMITED_SYS_INDICATIONS_ON
}qcril_data_limited_sys_ind_switch_type;

typedef enum{
    QCRIL_DS_ERROR = -1,
    QCRIL_DS_SUCCESS = 0
}RetVal;

#define QCRIL_DATA_NUM_OMH_PROFILES_EXPECTED  (6)

/* Data Call Profile: Simple IP User Profile Parameters*/
typedef struct {
  int  profileId;
  int  priority;       /* priority. [0..255], 0 - highest */
} RIL_DataCallProfileInfo;

typedef struct
{
  int num_profiles;
  RIL_DataCallProfileInfo profile_info[QCRIL_DATA_NUM_OMH_PROFILES_EXPECTED];
} OmhProfileInfo;

typedef struct{
  int result; //success/failure
  int error_cause; //valid value if result is failure
} DDSStatusInfo;

typedef enum{
  DSD_DDS_DURATION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_DDS_DURATION_PERMANANT_V01 = 0x00, /**<  Permanent - DEFAULT*/
  DSD_DDS_DURATION_TEMPORARY_V01 = 0x01, /**<  Temporary  */
  DSD_DDS_DURATION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
} dsd_dds_switch_type;

typedef struct{
  int dds_sub_id; /* -1 ->invalid , 0-> PRIMARY , 1-> SECONDARY 2-> TERTIARY */
  dsd_dds_switch_type switch_type;
} DDSSubIdInfo;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
/*===========================================================================

  FUNCTION:  qcril_data_embms_activate_tmgi

===========================================================================*/
/*!
    @brief

    Handles QCRIL_EVT_HOOK_EMBMS_ACTIVATE_TMGI.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
void qcril_data_embms_activate_tmgi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION:  qcril_data_embms_deactivate_tmgi

===========================================================================*/
/*!
    @brief

    Handles QCRIL_EVT_HOOK_EMBMS_DEACTIVATE_TMGI.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
void qcril_data_embms_deactivate_tmgi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION:  qcril_data_embms_get_available_tmgi

===========================================================================*/
/*!
    @brief

    Handles QCRIL_EVT_HOOK_EMBMS_GET_AVAILABLE_TMGI.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
void qcril_data_embms_get_available_tmgi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION:  qcril_data_embms_get_active_tmgi

===========================================================================*/
/*!
    @brief

    Handles QCRIL_EVT_HOOK_EMBMS_GET_ACTIVE_TMGI.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
void qcril_data_embms_get_active_tmgi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION:  qcril_data_embms_activate_deactivate_tmgi

===========================================================================*/
/*!
    @brief

    Handles QCRIL_EVT_HOOK_EMBMS_ACTIVATE_DEACTIVATE_TMGI

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
void qcril_data_embms_activate_deactivate_tmgi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION:  qcril_data_embms_content_desc_update

===========================================================================*/
/*!
    @brief

    Handles QCRIL_EVT_HOOK_EMBMS_CONTENT_DESC_UPDATE.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
void qcril_data_embms_content_desc_update
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION:  qcril_data_embms_send_interested_list

===========================================================================*/
/*!
    @brief

    Handles QCRIL_EVT_HOOK_EMBMS_SEND_INTERESTED_TMGI_LIST.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return

    None on Success, Generic failure response on Failure
*/
/*=========================================================================*/
void qcril_data_embms_send_interested_list
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __DATA_COMMON_H */
