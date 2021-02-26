/** 
  @file  EFIChargerExtn.h
  @brief Charger Extension UEFI Protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/21/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 03/05/13   dy      created

=============================================================================*/
#ifndef __EFICHARGEREXTN_H__
#define __EFICHARGEREXTN_H__

/** @ingroup efi_charger_extn_constants
  Protocol version.
*/
#define CHARGER_EXTN_REVISION 0x00010000


/* Charger Extension UEFI GUID */
/** @ingroup efi_charger_extn_protocol */
extern EFI_GUID gBatteryChargerExtnProtocolGuid;

/** @cond */
/*
  Charger ExtensionUEFI typedefs
*/
typedef struct _EFI_BATTERY_CHARGING_EXTN_PROTOCOL EFI_BATTERY_CHARGING_EXTN_PROTOCOL;
/** @endcond */

/** @ingroup efi_charger_extn_data_types
  Attached charger type. 
*/
typedef enum 
{
   EFI_CHARGER_ATTACHED_CHGR__NONE,    /**< No charger. */
   EFI_CHARGER_ATTACHED_CHGR__BATT,    /**< Battery. */
   EFI_CHARGER_ATTACHED_CHGR__USB,     /**< USB path. */
   EFI_CHARGER_ATTACHED_CHGR__DCIN,    /**< DCIN path. */
} EFI_CHARGER_ATTACHED_CHGR_TYPE;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_BATTERY_CHARGING_EXTN_GET_POWER_PATH */ 
/** @ingroup efi_charger_extn_get_power_path
  @par Summary
  Returns information about the current power path.

  @param[out] PowerPath        Current power path (battery, USB, or DC); see 
                               #EFI_CHARGER_ATTACHED_CHGR_TYPE for details.

  @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_UNSUPPORTED       -- No library function is linked.
*/
typedef
EFI_STATUS (EFIAPI * EFI_BATTERY_CHARGING_EXTN_GET_POWER_PATH)(
  OUT EFI_CHARGER_ATTACHED_CHGR_TYPE *PowerPath
);


/* EFI_BATTERY_CHARGING_EXTN_GET_BATTERY_PRESENCE */ 
/** @ingroup efi_charger_extn_get_battery_presence
  @par Summary
  Returns information about battery presence.
 
  @param[out] BatteryPresent   TRUE, if the battery is present; 
                               otherwise, FALSE.

  @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_UNSUPPORTED       -- No library function is linked.
*/
typedef
EFI_STATUS (EFIAPI * EFI_BATTERY_CHARGING_EXTN_GET_BATTERY_PRESENCE)(
  OUT BOOLEAN *BatteryPresent
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_charger_extn_protocol
  @par Summary
  Battery Charger Extension Protocol interface.

  @par Parameters
  @inputprotoparams{charger_extn_proto_params.tex} 
*/
struct _EFI_BATTERY_CHARGING_EXTN_PROTOCOL {
  EFI_BATTERY_CHARGING_EXTN_GET_POWER_PATH             GetPowerPath;
  EFI_BATTERY_CHARGING_EXTN_GET_BATTERY_PRESENCE       GetBatteryPresence;
};

#endif  /* __EFICHARGEREXTN_H__ */
