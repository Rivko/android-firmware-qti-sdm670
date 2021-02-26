/** @file
   
 Declaration of SecBootRegLib(Security register) Library Interfaces 

 Copyright (c) 2011, Qualcomm Technologies Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 07/18/11    shl     Initial version.

=============================================================================*/

#ifndef __SEC_BOOT_REG_LIB_H__
#define __SEC_BOOT_REG_LIB_H__
  
#define QCOM_SEC_BOOT_REG_EVENT_GUID \
  { \
     { 0x1d2d1996, 0xc860, 0x4640, { 0xb7, 0xa0, 0xe2, 0xe3, 0x30, 0x9e, 0x70, 0x87 } } \
  }

#define QCOM_MEASURE_BOOT_REG_EVENT_GUID \
  { \
     { 0x317b2dc1, 0x898, 0x4897, { 0xb1, 0x4f, 0x4e, 0xae, 0xc2, 0x82, 0xf7, 0x7f } } \
  }


extern EFI_GUID gQcomSecBootRegEventGuid;
extern EFI_GUID gQcomMeasureBootRegEventGuid;

/**
  This is called in BDS when register security framework is needed.

  @param  VOID            
  
  @retval Status   SignalEvent() return value.

**/
EFI_STATUS EFIAPI SetSecBootRegSignal(VOID);
EFI_STATUS EFIAPI SetMeasureBootRegSignal(VOID);

#endif
