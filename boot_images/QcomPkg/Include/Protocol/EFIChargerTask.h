#ifndef __EFICHARGERTASK_H__
#define __EFICHARGERTASK_H__

/**
  @file EFIChargerTask.h
  @brief Charger UEFI Protocol definitions
*/
/*=============================================================================
  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/25/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 10/03/11   dy      Remove Revision from Protocol struct 
 06/11/11   dy      created

=============================================================================*/

/** @ingroup efi_charger_task_constants
  Battery charger current is not supported. 
*/
#define EFI_BATTERY_CHARGE_CURRENT_NOT_SUPPORTED 0xFFFFFFFF

/* Charger UEFI typedefs */
/** @cond */
typedef struct _EFI_QCOM_CHARGER_TASK_PROTOCOL  EFI_QCOM_CHARGER_TASK_PROTOCOL;
/** @endcond */

/* Charger UEFI GUID */
/** @ingroup efi_charger_task_protocol */
extern EFI_GUID gChargerTaskProtocolGuid;

/** @cond */
typedef struct _EFI_CHARGER_TASK_PROTOCOL EFI_CHARGER_TASK_PROTOCOL;
/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_CHARGER_TASK_EVENT_HANDLER */
/** @ingroup efi_charger_task_event_handler
  @par Summary
  Polls the charger application logic.

  @param[in] Event    Event to be polled.
  @param[in] Context  Data passed in the poll.

  @return
  None.
*/
typedef
VOID (EFIAPI * EFI_CHARGER_TASK_EVENT_HANDLER)(
  IN EFI_EVENT       Event,
  IN VOID           *Context 
);
  
/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_charger_task_protocol
  @par Summary
  Charger Task Protocol interface.
  
  @par Parameters
  @inputprotoparams{charger_task_proto_params.tex} 
*/

#if 0
struct _EFI_CHARGER_TASK_PROTOCOL {
  EFI_CHARGER_TASK_EVENT_HANDLER   ChargerTaskEventHandler;
};
#endif

#endif  /* __EFICHARGERTASK_H__ */
