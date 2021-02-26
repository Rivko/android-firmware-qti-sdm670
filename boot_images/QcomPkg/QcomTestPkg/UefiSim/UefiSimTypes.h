/** @file UefiSimTypes.h.c
   
  UEFI Types for simulator 
  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/06/12   vk      Initial Revision.

=============================================================================*/

#ifndef __UEFI_SIM_TYPES__H__
#define __UEFI_SIM_TYPES__H__

typedef unsigned char  byte;
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

typedef unsigned short word;
typedef unsigned long  dword;

typedef unsigned char  CHAR8;
typedef unsigned short CHAR16;
typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned long  UINT32;
typedef unsigned long long  UINT64;
typedef unsigned long  UINTN;
typedef long  INT32;
typedef long long  INT64;
typedef void  VOID;
typedef long  INTN;

#define BOOLEAN UINT8
#define ASSERT(x)

#define TRUE 1
#define FALSE 0

#ifndef NULL
  #define NULL (void*)0
#endif

#endif /* __TYPES__H__ */

