/**
  @file EFIQcomHardwareInterrupt.h
  @brief Qualcomm Technologies, Hardware Interrupt Interface
*/
/*=============================================================================
  Copyright (c) 2010, 2012, 2014 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/20/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 12/17/10   niting  Initial Revision

=============================================================================*/

#ifndef __EFIQCOM_HARDWARE_INTERRUPT_H__
#define __EFIQCOM_HARDWARE_INTERRUPT_H__

#include <Protocol/HardwareInterrupt.h>

/** @cond */
typedef struct _EFI_QCOM_HARDWARE_INTERRUPT_PROTOCOL EFI_QCOM_HARDWARE_INTERRUPT_PROTOCOL;
/** @endcond */

/**@ingroup efi_hw_interrupt_data_types
  Hardware interrupt source configuration. 
*/
typedef UINTN HARDWARE_INTERRUPT_SOURCE_CONFIG;

/* Revision 1.0 */
/**@ingroup efi_hw_interrupt_constants
  Protocol version.
*/
#define QCOM_HARDWARE_INTERRUPT_PROTOCOL_REVISION 0x0000000000010000

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* HARDWARE_INTERRUPT_SET_SOURCE_CONFIG */ 
/* Tech Pubs note: in description for Config parameter, added a space after 
   TRIGGER_ to aid in document formatting */
/** @ingroup efi_hw_interrupt_set_source_config
  @par Summary
  Configures the trigger type for the interrupt to edge or level sensitive.

  @param[in]  This    Pointer to the EFI_QCOM_HARDWARE_INTERRUPT_PROTOCOL 
                      instance.
  @param[in]  Source  Hardware source of the interrupt.
  @param[in]  Config  Interrupt trigger configuration: 
                      - INTERRUPT_DXE_SOURCE_CONFIG_TRIGGER_ LEVEL
                      - INTERRUPT_DXE_SOURCE_CONFIG_TRIGGER_ EDGE @tablebulletend

  @return
  EFI_SUCCESS      -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Hardware could not be programmed.

**/
typedef
EFI_STATUS
(EFIAPI *HARDWARE_INTERRUPT_SET_SOURCE_CONFIG)(
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source,
  IN HARDWARE_INTERRUPT_SOURCE_CONFIG   Config
  );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_hw_interrupt_protocol
  @par Summary
  Qualcomm Hardware Interrupt Protocol interface. 

  @par Parameters
  @inputprotoparams{hw_interrupt_proto_params.tex} 
*/
struct _EFI_QCOM_HARDWARE_INTERRUPT_PROTOCOL {
  UINT64                               Revision;
  EFI_HARDWARE_INTERRUPT_PROTOCOL      *HardwareInterruptProtocol;
  HARDWARE_INTERRUPT_SET_SOURCE_CONFIG SetInterruptSourceConfig;
};

/** @ingroup efi_hw_interrupt_protocol */
extern EFI_GUID gQcomHardwareInterruptProtocolGuid;

#endif


