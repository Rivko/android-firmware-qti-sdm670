/** @file
   
 Declaration of MorPpiDxe Interface 

 Copyright (c) 2010-2014, Qualcomm Technologies, Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 12/03/14    sk      Added to change library to driver
 09/17/14    sk      Changed to Protocol/EfiMorPpi.h.
 10/18/11    shl     Initial version.

=============================================================================*/
#ifndef _EFIMORPPI_H_
#define _EFIMORPPI_H_

/** @cond */
typedef struct _EFI_QCOM_MORPPI_PROTOCOL EFI_QCOM_MORPPI_PROTOCOL;
/** @endcond */

/* External reference to the gQcomMorPpiProtocolGuid GUID */
/** @cond */
extern EFI_GUID gQcomMorPpiProtocolGuid;
/** @endcond */

/*===========================================================================
  FUNCTION DECLARATIONS
===========================================================================*/
/* EFI_MORPPI_PROCESS_MORPPI */ 
/** @ingroup efi_morppi_process_morppi
  @par Summary
  This is called to handle Memory Overwrite Request / Physical Presence Interface.

  "A general description of the scheme is that after any type of
  Host Platform Reset event (with the exception of a CPU-only 
  reset that is used by some chipsets to turn off a CPU feature
  without re-setting other Host Platform components), if signaled
  to do so by the OS, the POST BIOS must, prior to executing any
  non-BIOS code, overwrite system memory."
  
  @param[in]     This                  Pointer to the EFI_QCOM_MORPPI_PROTOCOL 
                                       instance.
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_NOT_FOUND         -- Operation not found. \n
*/
typedef EFI_STATUS (EFIAPI *EFI_MORPPI_PROCESS_MORPPI)( IN EFI_QCOM_MORPPI_PROTOCOL *This);



//EFIAPI  EFI_STATUS ProcessMorPpi( VOID );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_morppi_protocol
  @par Summary
  Memory Overwrite and Physical Presence Interface Protocol interface. 

  @par Parameters
  @inputprotoparams{morppi_proto_params.tex} 
*/
struct _EFI_QCOM_MORPPI_PROTOCOL
{
  EFI_MORPPI_PROCESS_MORPPI ProcessMorPpi;
}; 

#endif /* _EFIMORPPI_H_ */

