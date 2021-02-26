/** 
  @file  EFICardInfo.h
  @brief Sdcc Card Info Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2011-2016 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---     -----------------------------------------------------------
 2016/09/20  jt      Update serial number size 
 2016/09/13  jt      Remove SdccCardInfo, update GUID 
 2016/08/25  jt      Update revision 
 2016/06/28  jt      Add get/set boot LUN APIs 
 2016/01/26  jt      Update mem card info 
 2012/05/03  vishalo Merge in Techpubs Doxyegen comments
 2011/04/19  llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 2012/01/26  vishalo Merge in Techpubs Doxyegen comments
 2011/11/15  bn      Code clean up  
 2011/10/26  bn      Added RPMB support   
 2011/10/21  llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 2011/06/28  bn      Rename file to EFICardInfo.h 
 2011/05/19  bn      Added Card Info Protocol pointer
 2011/04/13  bn      Initial version.
 
=============================================================================*/
#ifndef __EFICARDINFO_H__
#define __EFICARDINFO_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_cardInfo_constants 
@{ */
/**
  Protocol version.
*/
#define EFI_MEM_CARD_INFO_PROTOCOL_REVISION 0x0000000000010003
/** @} */ /* end_addtogroup efi_cardInfo_constants */

/*  Protocol GUID definition */
/** @ingroup efi_cardInfo_protocol */ 
#define EFI_CARD_INFO_PROTOCOL_GUID \
   { 0x85C1F7D2, 0xBCE6, 0x4F31, { 0x8F, 0x4D, 0xD3, 0x7E, 0x03, 0xD0, 0x5E, 0xAA } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the EFICardInfo Protocol GUID.
 */
extern EFI_GUID gEfiMemCardInfoProtocolGuid;

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  Protocol declaration.
 */
typedef struct _EFI_MEM_CARDINFO_PROTOCOL    EFI_MEM_CARDINFO_PROTOCOL;
/** @endcond */

/** @addtogroup efi_cardInfo_data_types 
@{ */
/**
 UFS/SDCC unified Card information.
*/
typedef struct _CARD_INFO{
   UINT16   mfr_id;                  /**< Manufacturer ID. */
   UINT16   oem_id;                  /**< Original equipment manufacturer ID. */
   UINT8    mfr_date[8];             /**< Manufacture date. */
   UINT8    product_serial_num[252]; /**< Product serial number. */
   UINT32   serial_num_len;          /**< Product serial number length. */
   UINT8    inquiry_str[29];         /**< Output from SCSI inquiry command. */
   UINT32   rpmb_size_in_byte;       /**< Replay protected memory block partition 
                                     size in bytes. */
   UINT32   reliable_write_count;    /**< Reliable write count. */
   UINT8    card_type[4];            /**< Type of the card. 'UFS' or 'MMC' */
}/** @cond */  MEM_CARD_INFO  /** @endcond */;
/** @} */ /* end_addtogroup efi_cardInfo_data_types */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/

/* GET_MEM_CARD_INFO */ 
/** @ingroup efi_cardInfo_get_card_info
  @par Summary
  Gets the card information. 

  @param[in]   This              Pointer to the 
                                 EFI_MEM_CARDINFO_PROTOCOL
                                 instance.
  @param[out]  card_info         Pointer to a variable that the driver returns 
                                 with the retrieved card information; see 
                                 #_MEM_CARD_INFO for details.
   
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid.
*/
typedef
EFI_STATUS
(EFIAPI *GET_MEM_CARD_INFO)(
   IN  EFI_MEM_CARDINFO_PROTOCOL *This,
   OUT MEM_CARD_INFO*   card_info
);

/* GET_BOOT_LU */ 
/** @ingroup efi_cardInfo_get_boot_lu
  @par Summary
  Gets the BOOT LU 

  @param[in]   This              Pointer to the 
                                 EFI_MEM_CARDINFO_PROTOCOL
                                 instance.
  @param[out]  BootLU            Pointer to a variable 
                                 that indicates the BOOT LU 
                                 (0 = disable BOOT LU, 1 = BOOT for A, 
								  2 = BOOT for B)
   
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid.
*/
typedef
EFI_STATUS
(EFIAPI *GET_BOOT_LU)(
   IN  EFI_MEM_CARDINFO_PROTOCOL *This,
   OUT UINT32                    *BootLU
);

/* SET_BOOT_LU */ 
/** @ingroup efi_cardInfo_set_boot_lu
  @par Summary
  Set the BOOT LU. 

  @param[in]   This              Pointer to the 
                                 EFI_MEM_CARDINFO_PROTOCOL
                                 instance.
  @param[in]   BootLU            Boot LU to switch to
								 (0 = disable BOOT LU, 1 = BOOT for A, 
								  2 = BOOT for B) 
   
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid.
*/
typedef
EFI_STATUS
(EFIAPI *SET_BOOT_LU)(
   IN  EFI_MEM_CARDINFO_PROTOCOL *This,
   IN  UINT32                    BootLU
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/

/** @ingroup efi_cardInfo_protocol
  @par Summary
  Storage Card Information Protocol interface.

  @par Parameters
  @inputprotoparams{card_info_proto_params.tex}   
*/
struct _EFI_MEM_CARDINFO_PROTOCOL 
{
   UINT64              Revision;
   GET_MEM_CARD_INFO   GetCardInfo;
   GET_BOOT_LU         GetBootLU; 
   SET_BOOT_LU         SetBootLU; 
};

#endif	/* __EFICARDINFO_H__ */

