/** @file FwCommonLib.h
   
  Header for externally-available functions of the FwCommonLib
  
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2013/06/28     niting      Added WARN macro to prevent message in PRODMODE
 2012/10/04     mic         Added new FV IO wrapper, linked list and common macros
 2012/01/25     jd          Changed SetFlag/CheckFlag function signature
 2011/10/12     jthompso    Initial version
=============================================================================*/

#ifndef _FW_COMMON_LIB_H_
#define _FW_COMMON_LIB_H_

#include <Uefi.h>
#include <Library/FwCommonLib/esp_dpp.h>
#include <Library/FwCommonLib/FwCheck_FlagsStates.h>
#include <Library/FwCommonLib/FwFileIO_Block.h>
#include <Library/FwCommonLib/FwFileIO_Fat.h>
#include <Library/FwCommonLib/FwFileIO_Fv.h>
#include <Library/FwCommonLib/FwFileIoWrapper.h>
#include <Library/FwCommonLib/FwGpt_Helpers.h>
#include <Library/FwCommonLib/FwGpt_ReadWrite.h>
#include <Library/FwCommonLib/FwHandle_File.h>
#include <Library/FwCommonLib/FwHandle_Partition.h>
#include <Library/FwCommonLib/FwUtil_Common.h>
#include <Library/FwCommonLib/FwUtil_Copy.h>
#include <Library/FwCommonLib/FwLinkedList.h>
#include <Library/FwCommonLib/FwCrypto.h>


#define ERR_HANDLE(Status)\
  do { \
    if (EFI_ERROR (Status)) { \
      return Status; \
    } \
  } while (0)

#define ALIGN_TO_BLOCK(OrigSize,BlockSize,AlignedSize)  \
  do { \
       AlignedSize = OrigSize + (BlockSize - 1);\
       AlignedSize = AlignedSize & ~(BlockSize -1);\
     \
  } while (0) 

#define HANDLE_ERROR_LABEL(Status, Function, Label) \
  do {  \
    if(EFI_ERROR(Status))   \
    {   \
      DEBUG((EFI_D_ERROR,": %a returned %r\n", #Function, Status)); \
      goto Label; \
    }   \
  }while (0)

#define HANDLE_ERR_CONTINUE(Status, Function) \
  do {  \
    if(EFI_ERROR(Status))   \
    {   \
      DEBUG((EFI_D_ERROR,": %a returned %r...Continue\n", #Function, Status)); \
    }   \
  }while (0)

#define DEBUG_ERR_LABEL(MESSAGE, Label) \
  do {  \
       DEBUG((EFI_D_ERROR, MESSAGE)); \
       goto Label; \
  }while (0)

#define DEBUG_WARN_LABEL(MESSAGE, Label) \
  do {  \
       DEBUG((EFI_D_WARN, MESSAGE)); \
       goto Label; \
  }while (0)

#endif
