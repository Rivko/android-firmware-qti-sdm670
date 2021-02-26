/** 
  @file  EFIRamPartition.h
  @brief RamPartition EFI protocol interface.
*/
/*=============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/14/15   vk      Initial Revision
=============================================================================*/

#ifndef __EFIRAMPARTITION_H__
#define __EFIRAMPARTITION_H__

/** @cond */
typedef struct _EFI_RAMPARTITION_PROTOCOL EFI_RAMPARTITION_PROTOCOL;
/** @endcond */

/** @addtogroup efi_ramPartition_constants 
@{ */
/**
  Protocol version. 
*/
#define EFI_RAMPARTITION_PROTOCOL_REVISION 0x0000000000010000
/** @} */ /* end_addtogroup efi_ramPartition_constants */

/*  Protocol GUID definition */
/** @ingroup efi_ramPartition_protocol */
#define EFI_RAMPARTITION_PROTOCOL_GUID \
   { 0x5172FFB5, 0x4253, 0x7D51, { 0xC6, 0x41, 0xA7, 0x01, 0xF9, 0x73, 0x10, 0x3C } }
  
/** @cond */
/**
  External reference to the RAMPARTITION Protocol GUID defined 
  in the .dec file. 
*/
extern EFI_GUID gEfiRamPartitionProtocolGuid;
/** @endcond */

typedef struct _RamPartition
{
  UINT64 Base;
  UINT64 AvailableLength;
}RamPartitionEntry;

/** @} */ /* end_addtogroup efi_ramPartition_data_types */

/*==============================================================================

                             API IMPLEMENTATION

==============================================================================*/


/* ============================================================================
**  Function : EFI_RamPartition_GetRamPartitionVersion
** ============================================================================
*/
/** @ingroup efi_ramPartition_getRamVersion 
  @par Summary
  Gets the RAM Partition table version.
    
  @param[in]   This            Pointer to the EFI_RAMPARTITION_PROTOCOL instance.
  @param[out]  MajorVersion    Pointer to UINT32 which returns RAM partition table version  
  @param[out]  MinorVersion    Pointer to UINT32 which returns RAM partition table version  
                        
                       

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_RAMPARTITION_GETRAMPARTITIONVERSION)(
   IN EFI_RAMPARTITION_PROTOCOL *This,
   OUT UINT32                   *MajorVersion,
   OUT UINT32                   *MinorVersion
   );

/* ============================================================================
**  Function : EFI_RamPartition_GetHighestBankBit
** ============================================================================
*/
/** @ingroup efi_ramPartition_getHighestBankBit
  @par Summary
  Gets the RAM Partition table version.
    
  @param[in]   This            Pointer to the EFI_RAMPARTITION_PROTOCOL instance.
  @param[out]  HighestBankBit  Pointer to Highest Bank Bit   
                        
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_RAMPARTITION_GETHIGHESTBANKBIT)(
   IN EFI_RAMPARTITION_PROTOCOL *This,
   OUT UINT32                   *HighestBankBit
   );

/* ============================================================================
**  Function : EFI_RamPartition_GetRamPartitions
** ============================================================================
*/
/** @ingroup efi_ramPartition_getRamPartitions
  @par Summary
  Gets the Ram version as read from the hardware register.
    
  @param[in]   This       Pointer to the EFI_RAMPARTITION_PROTOCOL instance.
  @param[out]  pnVersion  Pointer to a UINT32 passed by the caller that
                          will be populated by the driver.

  @return
  EFI_SUCCESS          -- Function completed successfully. \n
  EFI_BUFFER_TOO_SMALL -- Returns number of partitions available
  EFI_PROTOCOL_ERROR   -- Error occurred during the operation.
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_RAMPARTITION_GETRAMPARTITIONS)(
   IN EFI_RAMPARTITION_PROTOCOL* This,
   OUT RamPartitionEntry         *RamPartitions,
   IN OUT UINT32                 *NumPartition
   );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_ramPartition_protocol 
  @par Summary
  Ram Information Protocol interface.

  @par Parameters
*/
struct _EFI_RAMPARTITION_PROTOCOL {
   UINT64                                  Revision;
   EFI_RAMPARTITION_GETRAMPARTITIONVERSION GetRamPartitionVersion;
   EFI_RAMPARTITION_GETHIGHESTBANKBIT      GetHighestBankBit;
   EFI_RAMPARTITION_GETRAMPARTITIONS       GetRamPartitions;
}; 

#endif /* __EFIRAMPARTITION_H__ */

