/*===============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  
  FILE: 		EFIPIL.h
  DESCRIPTION:	
  
  REVISION HISTORY
  when       who     	what, where, why
  --------   ---     	--------------------------------------------------------
  06/14/17   yw     	initial version

================================================================================*/

#ifndef __EFIPIL_H__
#define __EFIPIL_H__

#define EFI_PIL_PROTOCOL_VERSION 0x0000000000010001

#define EFI_PIL_PROTOCOL_GUID \
  { 0x3bec87db, 0x4e76, 0x491c, { 0x96, 0x6c, 0x7c, 0xa5, 0x81, 0x2a, 0x64, 0xc9 } }
  
extern EFI_GUID gEfiPilProtocolGuid;

typedef EFI_STATUS (EFIAPI *EFI_PIL_INIT)(CONST CHAR8* subsys);
typedef EFI_STATUS (EFIAPI *EFI_PIL_GET_SUBSYS)(CONST CHAR8* subsys);
typedef EFI_STATUS (EFIAPI *EFI_PIL_PUT_SUBSYS)(CONST CHAR8* subsys);
typedef EFI_STATUS (EFIAPI *EFI_PIL_PING_SUBSYS)(CONST CHAR8* subsys);

typedef struct _EFI_PIL_PROTOCOL {
   UINT64                           Version;
   EFI_PIL_INIT						Init;
   EFI_PIL_GET_SUBSYS				GetSubsys;
   EFI_PIL_PUT_SUBSYS				PutSubsys;
   EFI_PIL_PING_SUBSYS				PingSubsys;
}EFI_PIL_PROTOCOL;

#endif