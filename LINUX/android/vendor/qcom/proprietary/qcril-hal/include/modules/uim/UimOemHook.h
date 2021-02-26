#ifndef UIM_OEM_HOOK_H
#define UIM_OEM_HOOK_H
/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "telephony/ril.h"
#include "framework/qcril_qmi_oem_events.h"
#include "modules/uim/qcril_uim_srvc.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* -----------------------------------------------------------------------------
   ENUM:      RIL_UiccVoltageStatus
   DESCRIPTION:
     Used to convey the SIM card's voltage status
-------------------------------------------------------------------------------*/
typedef enum
{
  UICC_VOLTAGE_START_ACTIVATION    = 0,
  UICC_VOLTAGE_ACTIVATED,
  UICC_VOLTAGE_START_DEACTIVATION,
  UICC_VOLTAGE_DEACTIVATED
} RIL_UiccVoltageStatus;

/* -----------------------------------------------------------------------------
   ENUM:      RIL_UiccPower
   DESCRIPTION:
     Used to convey respective power command to the SIM card
-------------------------------------------------------------------------------*/
typedef enum
{
  RIL_UICC_POWER_DOWN = 0,
  RIL_UICC_POWER_UP
} RIL_UiccPower;

/* -----------------------------------------------------------------------------
   ENUM:      RIL_TransportType
   DESCRIPTION:
     Used to convey respective transport type to the modem
-------------------------------------------------------------------------------*/
typedef enum
{
  RIL_UICC_REMOTE_TRANSPORT_OTHER = 0,
  RIL_UICC_REMOTE_TRANSPORT_BLUETOOTH,
  RIL_UICC_REMOTE_TRANSPORT_IP
} RIL_TransportType;

/* -----------------------------------------------------------------------------
   ENUM:      RIL_Usage
   DESCRIPTION:
     Used to convey respective usage to the modem
-------------------------------------------------------------------------------*/
typedef enum
{
  RIL_UICC_REMOTE_USAGE_REDUCED = 0,
  RIL_UICC_REMOTE_USAGE_NORMAL,
} RIL_Usage;

/* -----------------------------------------------------------------------------
   ENUM:      RIL_remoteEvent
   DESCRIPTION:
     Used to convey respective event info to the modem
-------------------------------------------------------------------------------*/
typedef enum
{
  RIL_UICC_REMOTE_CONNECTION_UNAVAILABLE = 0,
  RIL_UICC_REMOTE_CONNECTION_AVAILABLE,
  RIL_UICC_REMOTE_CARD_INSERTED,
  RIL_UICC_REMOTE_CARD_REMOVED,
  RIL_UICC_REMOTE_CARD_ERROR,
  RIL_UICC_REMOTE_CARD_RESET,
  RIL_UICC_REMOTE_CARD_WAKEUP
} RIL_remoteEvent;

/* -----------------------------------------------------------------------------
   ENUM:      RIL_APDUBehavior
   DESCRIPTION:
     Used to convey APDU behavior
-------------------------------------------------------------------------------*/
typedef enum
{
  RIL_UICC_APDU_BEHAVIOR_NO_GET_RESPONSE_FOR_WARNING = 0,
  RIL_UICC_APDU_BEHAVIOR_AUTOMATIC_GET_RESPONSE_FOR_WARNING
} RIL_APDUBehavior;

/* -----------------------------------------------------------------------------
   ENUM:      Ril_CardPresenceState
   DESCRIPTION:
     Used to convey card presence state
-------------------------------------------------------------------------------*/
typedef enum
{
  RIL_CARD_STATE_UNKNOWN   = 0,
  RIL_CARD_STATE_ABSENT    = 1,
  RIL_CARD_STATE_PRESENT   = 2
} Ril_CardPresenceState;

/* -----------------------------------------------------------------------------
   ENUM:      Ril_SlotState
   DESCRIPTION:
     Used to convey slot activity status
-------------------------------------------------------------------------------*/
typedef enum
{
  RIL_PHYSICAL_SLOT_STATE_INACTIVE  = 0x00,
  RIL_PHYSICAL_SLOT_STATE_ACTIVE    = 0x01
} Ril_SlotState;

typedef struct {
    int verifyAttempts;
} RIL_PersonalizationStatus;

/* -----------------------------------------------------------------------------
   RIL_PersonalizationReq
   DESCRIPTION:
     Used to activate & set the device's personalization data
-------------------------------------------------------------------------------*/
typedef struct {
    char * controlKey;
    RIL_PersoSubstate persoType;
} RIL_PersonalizationReq;

/* -----------------------------------------------------------------------------
   RIL_PersonalizationStatusResp
   DESCRIPTION:
     Used to convey the device's personalization status
-------------------------------------------------------------------------------*/
typedef struct {
    boolean has_gwNWPersoStatus;
    RIL_PersonalizationStatus gwNWPersoStatus;
    boolean has_gwNWSubsetPersoStatus;
    RIL_PersonalizationStatus gwNWSubsetPersoStatus;
    boolean has_gwSPPersoStatus;
    RIL_PersonalizationStatus gwSPPersoStatus;
    boolean has_gwCPPersoStatus;
    RIL_PersonalizationStatus gwCPPersoStatus;
    boolean has_gwSIMPersoStatus;
    RIL_PersonalizationStatus gwSIMPersoStatus;
    boolean has_gw1xNWType1PersoStatus;
    RIL_PersonalizationStatus gw1xNWType1PersoStatus;
    boolean has_gw1xNWType2PersoStatus;
    RIL_PersonalizationStatus gw1xNWType2PersoStatus;
    boolean has_gw1xRUIMPersoStatus;
    RIL_PersonalizationStatus gw1xRUIMPersoStatus;
    boolean has_gwSPNPersoStatus;
    RIL_PersonalizationStatus gwSPNPersoStatus;
    boolean has_gwSPEhplmnPersoStatus;
    RIL_PersonalizationStatus gwSPEhplmnPersoStatus;
    boolean has_gwICCIDPersoStatus;
    RIL_PersonalizationStatus gwICCIDPersoStatus;
    boolean has_gwIMPIPersoStatus;
    RIL_PersonalizationStatus gwIMPIPersoStatus;
} RIL_PersonalizationStatusResp;

/* Struct for OEMHook Depersonalization request info */
typedef struct {
  char      * deperso_type; /* null terminated */
  char      * deperso_code; /* null terminated */
} qcril_enter_deperso_req_msg;

/* Struct for OEMHook Depersonalization response info */
typedef struct {
  uint32_t number_of_retries;
} qcril_enter_deperso_resp_msg;

/* Struct for OEMHook Slot Send APDU request info */
typedef struct __attribute__ ((packed)) {
  uint32_t  slot_id;
  RIL_SIM_APDU    apdu_data;
} RIL_SlotStreamApduReq;

/* Struct for OEMHook Slot Send APDU response info */
typedef struct __attribute__ ((packed)){
  uint32_t              slot_id;
  RIL_Errno             ril_err;
  uint32_t              sw1;
  uint32_t              sw2;
  char                  simResponse[512];
  /* In hex string format ([a-fA-F0-9]*) Max 256*2 possible */
} RIL_SlotSendApduResp;

/* Struct for OEMHook Slot Unsol card state info */
typedef struct __attribute__ ((packed)){
  uint32_t        slot_id;
  RIL_CardState   card_state;
} RIL_SlotCardState;

/* Struct for OEMHook Slot card power request info */
typedef struct __attribute__ ((packed)) {
  uint32_t        slot_id;
  RIL_UiccPower   card_power;
} RIL_SlotCardPower;

/* Struct for OEMHook Slot card power response info */
typedef struct __attribute__ ((packed)){
  uint32_t        slot_id;
  RIL_Errno       ril_err;
} RIL_SlotCardPowerResp;

/* Struct for Physical to logical slot mapping request */
typedef struct __attribute__ ((packed)){
  uint32_t    array_len;
  uint32_t    slot_map[QMI_UIM_MAX_CARD_COUNT];
} RIL_PhySlotMap;

/* Struct for OEMHook set APDU behavior request */
typedef struct __attribute__ ((packed)){
  uint32_t              channel_id;
  RIL_APDUBehavior      apdu_behavior;
} RIL_SetAPDUBehavior;

typedef struct {
  boolean               isEnable;
  RIL_TransportType     transportType;
  RIL_Usage             usage;
  RIL_remoteEvent       remoteEvent;
} RIL_HookRemoteSimInfo;

typedef struct __attribute__ ((packed)){
  boolean         tlv_present;
  uint8_t         value;
} RIL_TerminalCapabilityLPATLV;

typedef struct
{
  Ril_CardPresenceState    card_state;
  Ril_SlotState            slot_state;
/* Logical slot is valid only when the slot state is ACTIVE */
  uint32_t                 logical_slot;
  uint32_t                 iccid_len;
  uint8_t                  iccid[QMI_UIM_MAX_ICCID_LEN];
} Ril_SlotStatus;

/* Struct for OEMHook Slot Unsol card state info */
typedef struct __attribute__ ((packed)){
  uint32_t         no_of_slots;
  Ril_SlotStatus   slot_status[QMI_UIM_MAX_CARD_COUNT];
} RIL_SlotsStatus;

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_voltage_status

===========================================================================*/
void qcril_uim_request_oem_hook_voltage_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_uicc_power

===========================================================================*/
void qcril_uim_request_oem_hook_uicc_power
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_slot_card_power

===========================================================================*/
void qcril_uim_request_oem_hook_slot_card_power
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_perso_reactivate

===========================================================================*/
void qcril_uim_request_oem_hook_perso_reactivate
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_perso_status

===========================================================================*/
void qcril_uim_request_oem_hook_perso_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_enter_perso_key

===========================================================================*/
void qcril_uim_request_oem_hook_enter_perso_key
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_get_sim_status

===========================================================================*/
void qcril_uim_request_oem_hook_get_sim_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_get_atr

===========================================================================*/
void qcril_uim_request_oem_hook_get_atr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_send_apdu

===========================================================================*/
void qcril_uim_request_oem_hook_send_apdu
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_switch_slot

===========================================================================*/
void qcril_uim_request_oem_hook_switch_slot
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_get_physical_slot_mapping

===========================================================================*/
void qcril_uim_request_oem_hook_get_physical_slot_mapping
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_set_apdu_behavior

===========================================================================*/
void qcril_uim_request_oem_hook_set_apdu_behavior
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_get_terminal_capability

===========================================================================*/
void qcril_uim_request_oem_hook_get_terminal_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_set_terminal_capability

===========================================================================*/
void qcril_uim_request_oem_hook_set_terminal_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_get_slots_status

===========================================================================*/
void qcril_uim_request_oem_hook_get_slots_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* UIM_OEM_HOOK_H */
