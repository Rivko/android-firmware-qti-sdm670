/** @file EFIAdapterInformationRegister.h
   
 Declaration of Adapter Information Register Driver Interface 

 Copyright (c) 2013-2015, Qualcomm Technologies, Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 02/25/15   vk        Fix LLVM warning
 02/03/14    llg    (Tech Pubs) Edited/added Doxygen comments and markup.
 06/11/13    shl    Initial version.

=============================================================================*/
#ifndef __EFI_ADAPTER_INFORMATION_REGISTER_PROTOCOL_H__
#define __EFI_ADAPTER_INFORMATION_REGISTER_PROTOCOL_H__

/** @cond */
typedef struct _EFI_ADAPTER_INFORMATION_REGISTER_PROTOCOL EFI_ADAPTER_INFORMATION_REGISTER_PROTOCOL;
/** @endcond */

/*===========================================================================
  Callback Function Prototype for Clients
===========================================================================*/
/** @addtogroup efi_adapterInfoRegister_callback_functions 
@{ */
/* AdapterInformationGetCallbackPtr */
/**
  @par Summary
  Gets adapter information callback routine for the client.

  @param[out] InformationBlock     Pointer to the buffer with the 
                                   InformationBlock structure that contains 
                                   details about the data specific to 
                                   InformationType.
  @param[out] InformationBlockSize Size of InformationBlock in bytes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure. @newpage
*/
typedef
EFI_STATUS
(EFIAPI *AdapterInformationGetCallbackPtr)(
  OUT VOID         **InformationBlock,
  OUT UINTN         *InformationBlockSize
);

/* AdapterInformationSetCallbackPtr */
/**
  @par Summary
  Sets adapter information callback routine for the client.

  @param[out] InformationBlock     Pointer to the buffer with the 
                                   InformationBlock structure that contains 
                                   details about the data specific to 
                                   InformationType.
  @param[out] InformationBlockSize Size of InformationBlock in bytes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef
EFI_STATUS
(EFIAPI *AdapterInformationSetCallbackPtr)(
  IN VOID         *InformationBlock,
  IN UINTN         InformationBlockSize
);
/** @} */ /* end_addtogroup efi_adapterInfoRegister_callback_functions */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_ADAPTER_INFORMATION_REGISTER */
/** @ingroup efi_adapter_information_register
  @par Summary
  Registers the callback function to support adapter information services.

  @param[in] This                    Pointer to the 
                                     EFI_ADAPTER_INFORMATION_REGISTER_PROTOCOL 
                                     instance.
  @param[in] InformationType         Pointer to an EFI_GUID that defines the 
                                     contents of InformationBlock. The caller 
                                     must use InformationType to specify the 
                                     information it wants from the service.
  @param[in] InformationGetCallback  Pointer to the 
                                     AdapterInformationGetCallbackPtr() 
                                     callback function; can be NULL.
                                     This parameter is mutually exclusive with 
                                     the InformationSetCallback parameter; one 
                                     of them must be NULL.
  @param[in] InformationSetCallback  Pointer to the 
                                     AdapterInformationSetCallbackPtr() 
                                     callback function; can be NULL.
                                     This parameter is mutually exclusive with 
                                     the InformationGetCallback parameter; one 
                                     of them must be NULL.

  @return
  EFI_SUCCESS           -- InformationType information was retrieved. \n
  @par
  EFI_DEVICE_ERROR      -- Device reported an error. \n
  @par
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. \n
  @par
  EFI_INVALID_PARAMETER -- Possible descriptions are:
                           - The <i>This</i> parameter is NULL
                           - The <i>InformationType</i> parameter is NULL
                           - The <i>GetInformationCallback</i> and 
                             <i>SetInformationCallback</i> parameters 
                             are both not NULL
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ADAPTER_INFORMATION_REGISTER)(
  IN  EFI_ADAPTER_INFORMATION_REGISTER_PROTOCOL  *This,
  IN  EFI_GUID                                   *InformationType,
  IN  AdapterInformationGetCallbackPtr            GetInformationCallback,
  IN  AdapterInformationSetCallbackPtr            SetInformationCallback
);


/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_adapterInfoRegister_protocol
  @par Summary
  Adapter Information Register Protocol interface.

  @par Parameters
  @inputprotoparams{adapterInfo_register_proto_params.tex} 
*/
struct _EFI_ADAPTER_INFORMATION_REGISTER_PROTOCOL
{
  UINT32                                   Revision;
  EFI_ADAPTER_INFORMATION_REGISTER         RegisterService;
};

/** @ingroup efi_adapterInfoRegister_constants
  Protocol version.
*/
#define ADAPTER_INFORMATION_REGISTER_REVISION 0x00010001

/** @ingroup efi_adapterInfoRegister_protocol */
extern EFI_GUID gAdapterInformationRegisterProtocolGuid;

#endif
