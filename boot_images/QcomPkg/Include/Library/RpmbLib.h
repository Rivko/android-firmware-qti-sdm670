#ifndef __RPMBLIB_H__
#define __RPMBLIB_H__

/** @file RpmbLib.h
   
  This file provides SDCC's RPMB Library external definitions. 
  
  Copyright (c) 2011-2013, 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
 
**/

/*=============================================================================
                              EDIT HISTORY


when         who     what, where, why
----------   ---     ---------------------------------------------------------
2016-02-11   jt      Include RPMBReadData
2015-04-17   jt/rm   Add support for ufs rpmb
2013-02-19   jt      Added support for configuring GPT/RPMB partitions 
2012-04-03   bn      Added ProvisionRpmbKey function definition
2011-11-18   bn      Initial Version 

=============================================================================*/


/**
  Function: ProvisionRpmbTestKey
 
  Description:
  This function sends a QSEE command to TZ in order to provision
  the RPMB with the test key.
                      
  @return EFI_SUCCESS               Initialization successful.
  @return EFI_INVALID_PARAMETER     Invalid parameter. 
**/
EFI_STATUS
EFIAPI
ProvisionRpmbTestKey (UINT32 *Result);

/**
  Function: ConfigStorPartitions
 
  Description:
  This function parses the config file and updates the BufferPtr with the 
  partition addition information. 
                      
  @return EFI_SUCCESS               Initialization successful.    
  @return EFI_INVALID_PARAMETER     Invalid parameter.
  @return EFI_LOAD_ERROR            Failed to open the parser. 
  @return EFI_UNSUPPORTED           Failed to enumerate the key values 
**/
EFI_STATUS
EFIAPI
ConfigStorPartitions (UINT32 DevId, UINT8 *PartitionGuid, 
                      UINT32 Version, UINT32 *NumPartitions, UINT8 *BufferPtr,
                      VOID *BlkIoMedia);

/**
  Function: InitPartitionConfig
 
  Description:
  This function loads the partition config file from the FV.
                      
  @return EFI_SUCCESS               Initialization successful.        
**/
EFI_STATUS
EFIAPI
InitPartitionConfig(VOID);

/**
  Function: RPMBProvisioned
 
  Description:
  This function checks if RPMB is already provisioned or not.  
                      
  @return EFI_SUCCESS               RPMB already provisioned. 
  @return EFI_UNSUPPORTED           RPMB not yet provisioned. 
  @return EFI_DEVICE_ERROR          CG/RPMB read failure 
**/
EFI_STATUS
EFIAPI
RPMBProvisioned (VOID);


/**
  Function: RPMBReadData
 
  Description:
  This function reads BlockCount sectors starting at Lba from 
  the RPMB partition. 
                      
  @return EFI_SUCCESS               Data read successfully. 
  @return EFI_INVALID_PARAMETER     Invalid parameter. 
  @return EFI_DEVICE_ERROR          CG/RPMB read failure 
  @return EFI_OUT_OF_RESOURCES      Insufficient memory 
  @return EFI_UNSUPPORTED           RPMB is not provisioned 
**/
EFI_STATUS
EFIAPI
RPMBReadData (
   UINT32                Lba, 
   UINT32                BlockCount, 
   UINT8                 *ReadBuffer
   );

#endif /* __RPMBLIB_H__ */

