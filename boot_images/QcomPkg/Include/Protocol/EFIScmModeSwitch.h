/** 
  file  EFIScmModeSwitch.h
  
  Runtime protocol for mode switch to 32-bit.

*/
/*=============================================================================
   Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------- 
 16/08/16   bd      Initial version
=============================================================================*/
#ifndef __EFISCM_MS_H__
#define __EFISCM_MS_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include <Uefi.h>

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @ingroup efi_scm_mode_swith_constants
  Protocol version.
*/
#define QCOM_SCM_MODE_SWITCH_PROTOCOL_REVISION 0x0000000000000001

/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/** @ingroup efi_scm_mode_swith_protocol */
extern EFI_GUID gQcomScmModeSwithProtocolGuid;

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/

typedef struct hlos_boot_args_s
{
  UINT64 el1_x0;
  UINT64 el1_x1;
  UINT64 el1_x2;
  UINT64 el1_x3;
  UINT64 el1_x4;
  UINT64 el1_x5;
  UINT64 el1_x6;
  UINT64 el1_x7;
  UINT64 el1_x8;
  UINT64 el1_elr;
} __attribute__ ((packed)) hlos_boot_args_t;

/* QCOM_SCM_SYS_CALL */ 
/** @ingroup efi_scm_mode_switch_sys_call
  @par Summary
  Call to perform mode switch. This function does not return upon success
   
  @param[in]     hlos_boot_info  Structure containing entry point, mode value and 
                                 arguments to entry point

  @return
  Error values  -- In case of failure; Does not return upon succes \n
*/

typedef
EFI_STATUS
(EFIAPI *QCOM_SWITCH_TO_32BIT_MODE)(
  IN  hlos_boot_args_t     hlos_boot_info
); 

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_scm_mode_switch_sys_call
  @par Summary
  Qualcomm Mode Switch Protocol interface.

  @par Parameters
  @inputprotoparams{scm_proto_params.tex} 
*/
typedef struct _QCOM_SCM_MODE_SWITCH_PROTOCOL {
  QCOM_SWITCH_TO_32BIT_MODE        SwitchTo32bitMode;
} QCOM_SCM_MODE_SWITCH_PROTOCOL;

#endif /* __EFISCM_MS_H__ */


