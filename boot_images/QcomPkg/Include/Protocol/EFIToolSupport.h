/** @file
   
 Declaration of TOOLSUPPORT(Random Number Generator) DXE Driver Interface 

 Copyright (c) 2012 - 2014 Qualcomm Technologies, Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 08/05/14    vk      Update copyright
 08/01/14    vk      LLVM warning cleanup
 09/11/13    shl     Added TzFuseMilestone in the protocol
 05/21/13    shl     Added NeedQcomPkProtection in the protocol
 03/15/13    shl     Added HandleMorPpi in the protocol
 07/17/12    shl     Initial version.

=============================================================================*/
#ifndef __EFI_TOOLSUPPORT_PROTOCOL_H__
#define __EFI_TOOLSUPPORT_PROTOCOL_H__

typedef struct _EFI_QCOM_TOOLSUPPORT_PROTOCOL EFI_QCOM_TOOLSUPPORT_PROTOCOL;

/** 
   Function: Getdata
   
   Returns data implementation.
    
*/
typedef EFI_STATUS (EFIAPI *EFI_TOOLSUPPORT_GET_DATA) ( IN EFI_QCOM_TOOLSUPPORT_PROTOCOL *This,
                    IN OUT UINT8 *Buffer );


/** 
   Function: GetdataLen
   
   Returns data length implementation.
    
*/
typedef EFI_STATUS (EFIAPI *EFI_TOOLSUPPORT_GET_DATA_LEN) ( IN EFI_QCOM_TOOLSUPPORT_PROTOCOL *This,
                    IN OUT UINT8 *PtrLen );

/** 
   Function: HandleMorPpi 
*/
typedef EFI_STATUS (EFIAPI *EFI_TOOLSUPPORT_HANDLE_MOR_PPI) ( IN EFI_QCOM_TOOLSUPPORT_PROTOCOL *This );

/** 
   Function: NeedQcomPkProtection 
*/
typedef BOOLEAN (EFIAPI *EFI_TOOLSUPPORT_NEED_QCOM_PK_PROTECTION) ( IN EFI_QCOM_TOOLSUPPORT_PROTOCOL *This );

/** 
   Function: TzFuseMilestone 
*/
typedef EFI_STATUS (EFIAPI *EFI_TOOLSUPPORT_TZ_FUSE_MILESTONE) ( IN EFI_QCOM_TOOLSUPPORT_PROTOCOL *This );



/** 
  QCOM TOOLSUPPORT Protocol  
  */
struct _EFI_QCOM_TOOLSUPPORT_PROTOCOL
{
  EFI_TOOLSUPPORT_GET_DATA                  GetDataProtocol;
  EFI_TOOLSUPPORT_GET_DATA_LEN              GetDataLenProtocol;
  EFI_TOOLSUPPORT_HANDLE_MOR_PPI            HandleMorPpi;
  EFI_TOOLSUPPORT_NEED_QCOM_PK_PROTECTION   NeedQcomPkProtection;
  EFI_TOOLSUPPORT_TZ_FUSE_MILESTONE         TzFuseMilestone;  
}; 

/**
  Protocol version
*/
#define TOOLSUPPORT_REVISION 0x00010001

extern EFI_GUID gQcomToolSupportProtocolGuid;


#endif
