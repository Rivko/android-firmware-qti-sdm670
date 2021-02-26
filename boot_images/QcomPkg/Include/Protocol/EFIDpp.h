/** 
  @file  EFIDpp.h 
  @brief DPP UEFI Protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2012-2015 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/
  
/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/16/15   mic	   Use updated DppProtocol 
07/16/13   mic     Added FAT-based DPP protocol support
04/30/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/05/12   rs      Added support for deletion of DPP items 
09/20/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
03/25/12   jd      Initial revision

=============================================================================*/

#ifndef __EFIDPP_H__
#define __EFIDPP_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @ingroup efi_dpp_constants
Protocol version. */
#define EFI_DPP_PROTOCOL_REVISION 0x0000000000010002

/* Protocol GUID definition */
/** @ingroup efi_dpp_protocol */
#define EFI_DPP_PROTOCOL_GUID \
  {0x42d430c0, 0xae55, 0x4a6c, {0xa7, 0x95, 0xc6, 0x3e, 0x68, 0x7a, 0x15, 0x49}}


/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/** @cond */
/* External reference to the DPP Protocol GUID. */
extern EFI_GUID gEfiDppProtocolGuid;


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/* Protocol declaration */
typedef struct _EFI_DPP_PROTOCOL EFI_DPP_PROTOCOL;
/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_SETDPP */
/** @ingroup efi_dpp_setdpp
  @par Summary
  Takes data as specified by type and name, and stores it in the device 
  provisioning partition.
   
  @param[in]  This              Pointer to the EFI_DPP_PROTOCOL instance.
  @param[in]  Type              Unicode string specifying the provision data 
                                type, e.g. L"QCOM" or L"OEM".
  @param[in]  Name              Unicode string specifying the provision data 
                                name, e.g. L"WLAN.PROVISION".
  @param[in]  Data              Pointer to the buffer that contains the data 
                                to be written.
  @param[in]  DataSize          Size, in bytes, of the provision data.
  
  
  @par Description
  This function takes in the provision data type, name, and data payload, and 
  stores the provision data in the device provisioning partition. If previous 
  provision data with the same type and name already exists in the device 
  provisioning partition, it is replaced by the new data.
  
  
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. \n
  EFI_ABORTED           -- Data corruption was detected. \n
  EFI_XXXX              -- Other errors; refer to @xhyperref{R1,[R1]} for 
                           details.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_SETDPP)(
  IN  EFI_DPP_PROTOCOL  *This,
  IN  CHAR16            *Type,
  IN  CHAR16            *Name,
  IN  VOID              *Data,
  IN  UINTN              DataSize
);

/* EFI_GETDPP */
/** @ingroup efi_dpp_getdpp
  @par Summary
  Retrieves provision data as specified by type and name, and stores it in a 
  provided buffer.
   
  @param[in]  This              Pointer to the EFI_DPP_PROTOCOL instance.
  @param[in]  Type              Unicode string specifying the provision data 
                                type, e.g. L"QCOM" or L"OEM".
  @param[in]  Name              Unicode string specifying the provision data 
                                name, e.g. L"WLAN.PROVISION".
  @param[out] Data              Pointer to the caller-allocated buffer that 
                                is to store the retrieved provision data. If 
                                NULL is passed in, this function returns the 
                                provision data size.
  @param[out] DataSize          Upon successful return, the size of the 
                                provision data retrieved.
  
  
  @par Description
  This function takes in the provision data type and name, and retrieves 
  corresponding provision data from the device provisioning partition. If the 
  specified provision data does not exist in device provisioning partition, 
  EFI_NOT_FOUND is returned.
    
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. \n
  EFI_ABORTED           -- Data corruption was detected. \n
  EFI_NOT_FOUND         -- Specified provision data was not found. \n
  EFI_XXXX              -- Other errors; refer to @xhyperref{R1,[R1]} for 
                           details.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_GETDPP)(
  IN  EFI_DPP_PROTOCOL  *This,
  IN  CHAR16            *Type,
  IN  CHAR16            *Name,
  OUT VOID              *Data,
  OUT UINTN             *DataSize
);

/* EFI_ERASEDPP */
/** @ingroup efi_dpp_erasedpp
  @par Summary
  Erases the entire device provisioning partition.
   
  @param[in]  This          Pointer to the EFI_DPP_PROTOCOL instance.

  @par Description
  This function erases the entire device provisioning partition. All data in 
  the device provisioning partition will be lost. Given the current device 
  provisioning partition size, erasing is a time-consuming operation.
    
  @return
  EFI_SUCCESS          -- Function completed successfully. \n
  EFI_OUT_OF_RESOURCES -- Request could not be completed due to lack of 
                          resources.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ERASEDPP)(
  IN  EFI_DPP_PROTOCOL  *This
);

/* EFI_DELETEITEMDPP */
/** @ingroup efi_dpp_deleteitemdpp
  @par Summary
  Deletes specific device provisioning data.
   
  @param[in]  This              Pointer to the EFI_DPP_PROTOCOL instance.
  @param[in]  Type              Unicode string specifying the provision data 
                                type, e.g. L"QCOM" or L"OEM".
  @param[in]  Name              Unicode string specifying the provision data 
                                name, e.g. L"WLAN.PROVISION".

  @par Description
  This function deletes a particular device provisioning partition item. 

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. \n
  EFI_NOT_FOUND         -- Item to be deleted was not found in the device 
                           provisioning partition. \n
  EFI_XXXX              -- Other errors; refer to @xhyperref{R1,[R1]} for 
                           details.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_DELETEITEMDPP)(
  IN  EFI_DPP_PROTOCOL  *This,
  IN  CHAR16            *Type,
  IN  CHAR16            *Name
);
/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_dpp_protocol
  @par Summary
  Device Provisioning Partition (DPP) Protocol interface.

  @par Parameters
  @inputprotoparams{dpp_proto_params.tex} 
*/
struct _EFI_DPP_PROTOCOL {
  UINT64            Revision;
  EFI_SETDPP        SetDPP;
  EFI_GETDPP        GetDPP;
  EFI_ERASEDPP      EraseDPP;
  EFI_DELETEITEMDPP DeleteItemDPP;
};

#endif	/* __EFIDPP_H__ */

