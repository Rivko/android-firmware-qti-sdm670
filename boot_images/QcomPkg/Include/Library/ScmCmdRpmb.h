/** @file ScmCmdRpmb.h
   
  Defines library functions for SCM commands used for RPMB's 
  Content Generator (CG). 

  Copyright (c) 2011, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 ----------   ---     -----------------------------------------------------------
 2011/11/12   bn      Initial revision.

=============================================================================*/
#ifndef _SCMCMDRPMB_H_
#define _SCMCMDRPMB_H_

#define BLOCK_SIZE   512

/**

  This code generates the Key Provision packet.

  @param KeyProvisionPkt            Pointer to the RPMB Key Provision Pkt.
                      
  @return EFI_SUCCESS               Key Provision packet created successfully.
  @return EFI_DEVICE_ERROR          The Provision packet could not be created
                                    due to some unknown errors.

**/
EFI_STATUS
EFIAPI
ScmCmdGetKeyProvisionPkt (
  OUT     VOID              *KeyProvisionPkt
  );

/**

  This code validates the Response packet and initializes the 
  Write Counter on first time it is called. 

  @param ResponsePkt                Pointer to the eMMC's Write Response packet
                      
  @return EFI_SUCCESS               Response Packet validated successfuly.
  @return EFI_DEVICE_ERROR          There's Write Counter mismatch or HMAC mismatch.

**/
EFI_STATUS
EFIAPI
ScmCmdProcessResponsePkt (
  IN      VOID              *ResponsePkt
  );

/**

  This code generates the RPMB Read packet. 

  @param StartSector                Start Sector to read from
  @param Readpkt                    Pointer to the RPMB Data frame formatted
                                    for authenticated Read command
 
  @return EFI_SUCCESS               Read Packet generated successfuly.
  @return EFI_DEVICE_ERROR          There's error generating the Read packet.

**/
EFI_STATUS
EFIAPI
ScmCmdGenerateReadPkt (
  IN      UINT32            StartSector,
  OUT     VOID              *ReadPkt
  );

/**

  This code sends the eMMC card information needed by the Content
  Generator (CG) in order to properly format the packet. 

  @param ReliableWriteCount         eMMC's Reliable Write Count supported
                      
  @return EFI_SUCCESS               Read Packet generated successfuly.
  @return EFI_DEVICE_ERROR          There's some unknown error.

**/
EFI_STATUS
EFIAPI
ScmCmdSetCardInfo (
  IN      UINT32            ReliableWriteCount
  );

/**

  This code locks the Generate Provision Key packet from future access
                      
  @return EFI_SUCCESS               Read Packet generated successfuly.
  @return EFI_DEVICE_ERROR          There's some unknown error.

**/
EFI_STATUS
EFIAPI
ScmCmdLockKeyProvision (VOID);

#endif /* _SCMCMDRPMB_H_ */
