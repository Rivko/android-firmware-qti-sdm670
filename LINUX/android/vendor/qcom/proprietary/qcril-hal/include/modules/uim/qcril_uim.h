#ifndef QCRIL_UIM_H
#define QCRIL_UIM_H
/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "framework/legacy.h"
#include "framework/Message.h"
#include "modules/uim/qcril_uim_srvc.h"

/*===========================================================================

                           DEFINES

===========================================================================*/

#define QCRIL_UIM_MAX_AID_SIZE                  20
#define QCRIL_UIM_MAX_PATH_SIZE                 10
#define QCRIL_UIM_MAX_SESSION_PRIORITY_TYPES     3
#define QCRIL_UIM_UPIN_STATE_REPLACES_PIN1       1
#define QCRIL_UIM_ICCID_LEN                     10
#define QCRIL_UIM_MAX_SELECT_RESP_COUNT          5
#define QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO         2

/*===========================================================================

                           TYPES

===========================================================================*/

/* -----------------------------------------------------------------------------
   ENUM:      QCRIL_UIM_INTERNAL_REQUEST_TYPE

   DESCRIPTION:
     UIM internal requests
-------------------------------------------------------------------------------*/
typedef enum
{
  UIM_INTERNAL_REQUEST_BASE         = 0x1000,
  UIM_INTERNAL_REQUEST_REFRESH_REGISTER,
  UIM_INTERNAL_REQUEST_CHANGE_SUBSCRIPTION,
  UIM_INTERNAL_REQUEST_GET_MCC_MNC,
  UIM_INTERNAL_REQUEST_GET_FDN_STATUS,
  UIM_INTERNAL_REQUEST_GET_PIN_STATUS,
  UIM_INTERNAL_REQUEST_SET_FDN_STATUS,
  UIM_INTERNAL_REQUEST_SET_PIN_STATUS,
  UIM_INTERNAL_REQUEST_VERIFY_PIN2,
  UIM_INTERNAL_REQUEST_SIM_IO_READ_BINARY,
  UIM_INTERNAL_REQUEST_SIM_IO_READ_RECORD,
  UIM_INTERNAL_REQUEST_SIM_IO_UPDATE_BINARY,
  UIM_INTERNAL_REQUEST_SIM_IO_UPDATE_RECORD,
  UIM_INTERNAL_REQUEST_SET_SERVICE_STATUS_FDN,
  UIM_INTERNAL_REQUEST_GET_ATR,

  UIM_OEM_HOOK_REQUEST_BASE        = 0x2000,
  UIM_OEM_HOOK_UICC_POWER_REQ,
  UIM_OEM_HOOK_SLOT_CARD_POWER_REQ,
  UIM_OEM_HOOK_SLOT_GET_CARD_STATE_REQ,
  UIM_OEM_HOOK_SLOT_GET_ATR_REQ,
  UIM_OEM_HOOK_UICC_VOLTAGE_STATUS_REQ,
  UIM_OEM_HOOK_SLOT_SEND_APDU_REQ,
  UIM_OEM_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ,
  UIM_OEM_HOOK_SWITCH_SLOT_REQ,
  UIM_OEM_HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ,
  UIM_OEM_HOOK_ENTER_DEPERSONALIZATION_REQ,
  UIM_OEM_HOOK_PERSONALIZATION_STATUS_REQ,
  UIM_OEM_HOOK_PERSONALIZATION_REACTIVATE_REQ,
  UIM_OEM_HOOK_GET_SLOTS_STATUS,
  UIM_OEM_HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ,
  UIM_OEM_HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ
} qcril_uim_internal_request_type;

/* -----------------------------------------------------------------------------
   ENUM:      QCRIL_UIM_PROV_SESSION_STATE_TYPE

   DESCRIPTION:
     Used to describe what session state a provisioning app can be in.
-------------------------------------------------------------------------------*/
typedef enum
{
  QCRIL_UIM_PROV_SESSION_NOT_ACTIVATED            = 0,
  QCRIL_UIM_PROV_SESSION_ACTIVATION_IN_PROGRESS,
  QCRIL_UIM_PROV_SESSION_ACTIVATED,
  QCRIL_UIM_PROV_SESSION_DEACTIVATION_IN_PROGESS
} qcril_uim_prov_session_state_type;


/* -----------------------------------------------------------------------------
   ENUM:      QCRIL_UIM_PHONEBOOK_FILE_TAG_TYPE

   DESCRIPTION:
     Used to denote various tag types that can be available in an EF PBR.
-------------------------------------------------------------------------------*/
typedef enum
{
  QCRIL_UIM_FILE_TAG_NONE                     = 0,
  QCRIL_UIM_FILE_TAG_CONST_TYPE_1             = 0xA8,
  QCRIL_UIM_FILE_TAG_CONST_TYPE_2             = 0xA9,
  QCRIL_UIM_FILE_TAG_CONST_TYPE_3             = 0xAA,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_ADN         = 0xC0,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_IAP         = 0xC1,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_EXT1        = 0xC2,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_SNE         = 0xC3,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_ANR         = 0xC4,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_PBC         = 0xC5,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_GRP         = 0xC6,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_AAS         = 0xC7,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_GAS         = 0xC8,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_UID         = 0xC9,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_EMAIL       = 0xCA,
  QCRIL_UIM_FILE_TAG_PRIMITIVE_EF_CCP1        = 0xCB
} qcril_uim_phonebook_file_tag_type;


/* -----------------------------------------------------------------------------
   ENUM:      QCRIL_UIM_FCI_VALUE_TYPE

   DESCRIPTION:
     Indicates the values for the template requested from the card in the SELECT
     command when the application is selected
-------------------------------------------------------------------------------*/
typedef enum
{
  QCRIL_UIM_FCI_VALUE_NO_DATA                   = 0,
  QCRIL_UIM_FCI_VALUE_FCP                       = 1,
  QCRIL_UIM_FCI_VALUE_FCI                       = 2,
  QCRIL_UIM_FCI_VALUE_FCI_WITH_INTERFACES       = 3,
  QCRIL_UIM_FCI_VALUE_FMD                       = 4,
  QCRIL_UIM_FCI_VALUE_FCI_FALLBACK_FCP          = 5
} qcril_uim_fci_value_type;


typedef struct
{
  enum request_type
  {
    /* Internal requests */
    UIM_REQUEST_NONE               = 0,
    UIM_REQUEST_RIL_EVENT,
    UIM_REQUEST_OEM_HOOK_EVENT,
    UIM_REQUEST_GET_MCC_MNC,
    UIM_REQUEST_GET_FACILITY_LOCK,
    UIM_REQUEST_SET_FACILITY_LOCK,
    UIM_REQUEST_GET_ATR
  };
  std::shared_ptr<Message>   msg_ptr;
  request_type               req_type;
  RIL_Token                  ril_token;
} UimRequestMsgData;

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_PHONEBOOK_FILE_INFO_TYPE

   DESCRIPTION:
     Structure that holds data about files known as a result of EF PBR read.
     Note: file_tag is value defined per Spec 31.102, Section 4.4.2.1
-------------------------------------------------------------------------------*/
typedef struct
{
  qcril_uim_phonebook_file_tag_type     file_tag;
  qmi_uim_file_id_type                  file_id_info;
} qcril_uim_phonebook_file_info_type;

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_REFRESH_INFO_TYPE

   DESCRIPTION:
     Structure that holds data about refresh information
-------------------------------------------------------------------------------*/
typedef struct
{
  boolean                               pbr_read_in_progress[QMI_UIM_MAX_CARD_COUNT];
  uint32                                registration_mask;
  struct
  {
    uint16                                num_files;
    qcril_uim_phonebook_file_info_type  * files_ptr;
  }                                       files_info[QMI_UIM_MAX_CARD_COUNT];
} qcril_uim_refresh_info_type;


/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_PROV_SESSION_INFO_TYPE

   DESCRIPTION:
     Structure that holds data for all the provisioning session types
-------------------------------------------------------------------------------*/
typedef struct
{
  qcril_uim_prov_session_state_type      session_state_gw_indexes[QCRIL_UIM_MAX_SESSION_PRIORITY_TYPES];
  qcril_uim_prov_session_state_type      session_state_1x_indexes[QCRIL_UIM_MAX_SESSION_PRIORITY_TYPES];
} qcril_uim_prov_session_info_type;

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_OPEN_CHANNEL_INFO_TYPE

   DESCRIPTION:
     Structure contains the information required to recreate an open channel
     request from the response in case a fallback is required
-------------------------------------------------------------------------------*/
typedef struct
{
  unsigned char              aid_buffer[QMI_UIM_MAX_AID_LEN];
  unsigned short             aid_size;
  qcril_uim_fci_value_type   fci_value;
  qmi_uim_slot_type          slot;
} qcril_uim_open_channel_info_type;

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_GET_MCC_MNC_REQ_TYPE

   DESCRIPTION:
     Structure contains the information to retrieve MCC and MNC
-------------------------------------------------------------------------------*/
typedef struct
{
  char              aid_buffer[QMI_UIM_MAX_AID_LEN+1];
  uint8             num_mnc_digits;
  uint16            file_id;
  qmi_uim_app_type  app_type;
} qcril_uim_get_mcc_mnc_req_type;

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_PBR_REFRESH_REG_INFO_TYPE

   DESCRIPTION:
     Structure contains the information on PBR read during refresh registration
-------------------------------------------------------------------------------*/
typedef struct
{
  uint32   dyn_mask;
  uint16   record_index;
  uint16   num_records;
  uint16   record_size;
} qcril_uim_pbr_refresh_reg_info_type;

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_SAP_INFO_TYPE

   DESCRIPTION:
     Structure contains the information of QCRIL's client ID for SAP APIs
-------------------------------------------------------------------------------*/
typedef struct
{
  uint8   client_id;
  int     sap_error;
} qcril_uim_sap_info_type;

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_PIN2_ORIGINAL_REQUEST_TYPE

   DESCRIPTION:
     Structure used to copy the original request when the PIN 2
     needs to be verified before executing the request
-------------------------------------------------------------------------------*/
typedef struct
{
  uint32                                     size;
  int                                        cmd;
  union
  {
    qmi_uim_read_transparent_params_type     read_transparent;
    qmi_uim_read_record_params_type          read_record;
    qmi_uim_write_transparent_params_type    write_transparent;
    qmi_uim_write_record_params_type         write_record;
    qmi_uim_set_service_status_params_type   set_service_status;
  }                                          data;
} qcril_uim_pin2_original_request_type;

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_ORIGINAL_REQUEST_TYPE

   DESCRIPTION:
     Structure used to copy relevant details in original request that is needed
     by qmi_uim_callback. Currently supports data only for pin operation APIs
     and change subscription API.
-------------------------------------------------------------------------------*/
typedef struct
{
  qcril_instance_id_e_type             instance_id;
  qcril_modem_id_e_type                modem_id;
  RIL_Token                            token;
  int                                  request_id;
  qmi_uim_session_type                 session_type;
  boolean                              is_sap;
  union
  {
    /* Change subscription */
    uint32                                 qcril_evt;
    /* Set FDN status */
    qmi_uim_service_status_type            fdn_status;
    /* Open logical channel */
    qcril_uim_open_channel_info_type       channel_info;
    /* Get MCC and MNC info */
    qcril_uim_get_mcc_mnc_req_type         mcc_mnc_req;
    /* Send APDU parameter */
    qmi_uim_send_apdu_params_type          send_apdu;
    /* Refresh registration for PBR */
    qcril_uim_pbr_refresh_reg_info_type    refresh_pbr;
    /* Client details for SAP APIs */
    qcril_uim_sap_info_type                sap_info;
    /* slot id */
    uint8                                  slot_id;
    /* PIN2 Original request */
    qcril_uim_pin2_original_request_type * orig_pin2_ptr;
    /* GBA event id */
    qcril_evt_e_type                       gba_event_id;
    /* Remote unlock operation type */
    qmi_uim_remote_unlock_operation_type   remote_unlock_op;
  }                                    data;
} qcril_uim_original_request_type;


/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_PIN1_INFO_TYPE

   DESCRIPTION:
     Structure that holds encrypted PIN1 value along with AID for the
     corresponding application and the card's ICCID
-------------------------------------------------------------------------------*/
typedef struct
{
  uint8          aid_len;
  uint8          aid_value[QMI_UIM_MAX_AID_LEN];
  uint8          iccid_len;
  uint8          iccid_data[QCRIL_UIM_ICCID_LEN];
  uint8          encrypted_pin1_len;
  uint8          encrypted_pin1[QMI_UIM_MAX_ENCRYPTED_PIN_LEN];
  uint8          slot;
  boolean        silent_verify_in_progress;
} qcril_uim_pin1_info_type;


/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_LONG_APDU_INFO_TYPE

   DESCRIPTION:
     Structure that holds the info that is required to handle the long APDU
     transaction scenario between the modem & QCRIL.
-------------------------------------------------------------------------------*/
typedef struct
{
  boolean                             in_use;
  uint16                              total_apdu_len;
  uint32                              token;
  uint16                              rx_len;
  uint8                             * apdu_ptr;
  qcril_uim_original_request_type   * original_request_ptr;
} qcril_uim_long_apdu_info_type;


/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_SELECT_RESPONSE_INFO_TYPE

   DESCRIPTION:
     Structure that holds the info that is required to handle the long APDU
     transaction scenario between the modem & QCRIL.
-------------------------------------------------------------------------------*/
typedef struct
{
  boolean                             in_use;
  uint8                               channel_id;
  uint8                               sw1;
  uint8                               sw2;
  uint16                              select_resp_len;
  uint8                             * select_resp_ptr;
} qcril_uim_select_response_info_type;


/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_STRUCT_TYPE

   DESCRIPTION:
     Global data with UIM data
-------------------------------------------------------------------------------*/
typedef struct
{
  qmi_uim_card_status_type              card_status;
  qcril_uim_prov_session_info_type      prov_session_info;
  qcril_uim_refresh_info_type           refresh_info;
  boolean                               silent_pin_verify_reqd;
  qcril_uim_pin1_info_type              pin1_info[QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO];
  qcril_uim_long_apdu_info_type         long_apdu_info;
  qcril_uim_select_response_info_type   select_response_info[QCRIL_UIM_MAX_SELECT_RESP_COUNT];
  qmi_uim_bool_type                     temp_unlock_status;
  struct timespec                       card_inserted_time;
} qcril_uim_struct_type;

extern qcril_uim_struct_type   qcril_uim;


/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_INDICATION_PARAMS_TYPE

   DESCRIPTION:
     Structure used to copy indications received from the
     modem
-------------------------------------------------------------------------------*/
typedef struct
{
  qcril_instance_id_e_type         instance_id;
  qcril_modem_id_e_type            modem_id;
  qmi_uim_indication_id_type       ind_id;
  qmi_uim_indication_data_type     ind_data;
  void                           * user_data;
  uint8                            payload;
} qcril_uim_indication_params_type;


/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_CALLBACK_PARAMS_TYPE

   DESCRIPTION:
     Structure used to copy the parameters of a callback
-------------------------------------------------------------------------------*/
typedef struct
{
  qmi_uim_rsp_data_type                      qmi_rsp_data;
  const void                               * orig_req_data;
  uint32                                     payload;
} qcril_uim_callback_params_type;


/*=========================================================================

  FUNCTION:  qcril_uim_response

===========================================================================*/
/*!
    @brief
    Send a response to framework for a specific request.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_response
(
  qcril_instance_id_e_type  instance_id,
  RIL_Token                 token,
  RIL_Errno                 result,
  void*                     rsp_ptr,
  size_t                    rsp_len,
  boolean                   remove_entry,
  char*                     logstr
);

/*=========================================================================

  FUNCTION:  qcril_uim_reset_state

===========================================================================*/
/*!
    @brief
    Reset state of QCRIL_UIM at power up and whenever modem resets.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_reset_state
(
  void
);


/*=========================================================================

  FUNCTION:  qcril_uim_init_state

===========================================================================*/
/*!
    @brief
    Initializes QMI_UIM service and state of QCRIL_UIM. Called during
    initial power up and whenever modem restarts.

    @return
    None.
*/
/*=========================================================================*/
int qcril_uim_init_state
(
  bool *uim_ready_state
);

/*=========================================================================

  FUNCTION:  qmi_uim_card_init_callback

===========================================================================*/
/*!
    @brief
    Special case callback for the intial card status QMI commands.

    @return
    None
*/
/*=========================================================================*/
void qmi_uim_card_init_callback
(
  qmi_uim_rsp_data_type        * rsp_data_ptr,
  void                         * user_data
);

/*=========================================================================

  FUNCTION:  qcril_uim_process_qmi_callback

===========================================================================*/
void qcril_uim_process_qmi_callback
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_process_qmi_indication

===========================================================================*/
void qcril_uim_process_qmi_indication
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_send_qmi_sync_msg

===========================================================================*/
/*!
    @brief
    QMI UIM send qmi sync message.

    @return
    None
*/
/*=========================================================================*/
int qcril_uim_send_qmi_sync_msg
(
  uint32                        msg_id,
  const void                  * params,
  qmi_uim_rsp_data_type       * rsp_data_ptr
);

/*=========================================================================

  FUNCTION:  qcril_uim_send_qmi_async_msg

===========================================================================*/
/*!
    @brief
    QMI UIM send qmi async message.

    @return
    None
*/
/*=========================================================================*/

int qcril_uim_send_qmi_async_msg
(
  qcril_uim_request_type     request_type,
  void                     * param_data_ptr,
  void                     * original_request_ptr
);

/*=========================================================================

  FUNCTION:  qcril_log_get_uim_token_id

===========================================================================*/
/*!
    @brief
    Prints token id.

    @return
    int
*/
/*=========================================================================*/
int qcril_log_get_uim_token_id
(
  RIL_Token t
);

/*===========================================================================

  FUNCTION:  qcril_uim_convert_err_value

===========================================================================*/
/*!
    @brief
    Function converts QCRIL error value type to RIL error

    @return
    RIL_Errno
*/
/*=========================================================================*/
RIL_Errno qcril_uim_convert_err_value
(
  int          qcril_err_code
);

/*===========================================================================

  FUNCTION:  qmi_uim_async_cb

===========================================================================*/
/*!
    @brief
    QMI callback function

    @return
    None
*/
/*=========================================================================*/
void qmi_uim_async_cb
(
  qmi_uim_rsp_data_type        * rsp_data,
  void                         * user_data
);

#endif /* QCRIL_UIM_H */
