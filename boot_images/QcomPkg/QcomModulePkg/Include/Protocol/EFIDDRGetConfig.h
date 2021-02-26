/** 
  @file  EFIDDRGetConfig.h
  @brief DDRGetinfo EFI protocol interface.
*/
/*=============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who       what, where, why
 --------   ---       -----------------------------------------------------------
 05/01/17   daisond   Initial Revision
=============================================================================*/

#ifndef __EFIDDRGETINFO_H__
#define __EFIDDRGETINFO_H__
 
#include <DDRDetails.h>


/** @cond */
typedef struct _EFI_DDRGETINFO_PROTOCOL EFI_DDRGETINFO_PROTOCOL;
/** @endcond */

/** @addtogroup efi_ddrgetinfo_constants 
@{ */
/**
  Protocol version. 
*/
#define EFI_DDRGETINFO_PROTOCOL_REVISION 0x0000000000010000
/** @} */ /* end_addtogroup efi_ddrgetinfo_constants */

/*Both the protocl version and the structure version should be same */
#if (EFI_DDRGETINFO_PROTOCOL_REVISION != DDR_DETAILS_STRUCT_VERSION)
ASSERT(FALSE);
#endif

/*  Protocol GUID definition */
/** @ingroup efi_ddrgetinfo_protocol */
#define EFI_DDRGETINFO_PROTOCOL_GUID \
   { 0x1a7c0eb8, 0x5646, 0x45f7, { 0xab, 0x20, 0xea, 0xe5, 0xda, 0x46, 0x40, 0xa2 } }

  
/** @cond */
/**
  External reference to the DDRGetInfo Protocol GUID defined 
  in the .dec file. 
*/
extern EFI_GUID gEfiDDRGetInfoProtocolGuid;
/** @endcond */


/** @} */ /* end_addtogroup efi_ddrgetinfo_data_types */

/*==============================================================================

                             API IMPLEMENTATION

==============================================================================*/ 


/* ============================================================================
**  Function : EFI_DDRGETINFO_GETDDRDETAILS
** ============================================================================
*/
/** @ingroup efi_ddrgetinfo_GETDDRDETAILS
  @par Summary
  Gets the DDR Details
    
  @param[in]   This            Pointer to the EFI_DDRGETINFO_PROTOCOL instance.
  @param[out]  DetailsEntry    Pointer to DDR Details  
                        
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_DDRGETINFO_GETDDRDETAILS)(
   IN EFI_DDRGETINFO_PROTOCOL *This,
   OUT struct ddr_details_entry_info   *DetailsEntry
   );


/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_ddrgetinfo_protocol 
  @par Summary
  DDR Get Info Protocol interface.

  @par Parameters
*/
struct _EFI_DDRGETINFO_PROTOCOL {
   UINT64                                  Revision;
   EFI_DDRGETINFO_GETDDRDETAILS          GetDDRDetails;
}; 

#endif /* __EFIDDRGETINFO_H__ */


