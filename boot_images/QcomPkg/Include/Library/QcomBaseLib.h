/** @file QcomBaseLib.h
   
  Library routines for Qcom Base Lib - setting/getting memory base and size.

  Copyright (c) 2011-2017, Qualcomm Technologies, Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/27/17   ai      Fix use of retail
 09/07/16   vk      Move extract module name function here
 09/06/16   ai      Add function to covert timer count to mS
 10/15/15   bh      Add function for debug scripts
 05/19/15   vk      Support STI 32 bit
 06/11/14   aus     Added CopyMemS function
 02/07/14   vk      Added stack canary check
 03/20/13   vk      Added GetInfoBlkPtr API
 03/14/13   yg      Added GetTimerCount APIs
 02/06/13   vk      Added LogMsg
 11/23/11   jz      Created

=============================================================================*/
#ifndef __QCOMBASELIB_H__
#define __QCOMBASELIB_H__

#include <Uefi.h>
#include <UefiInfoBlk.h>
#include <Library/QcomTargetLib.h>

typedef enum {
  DDR_SYSTEM_MEMORY,
  // add new type here
  MAX_MEMORY_TYPE
} MemType;

/**
  Store memory related info (base, size, type) in Hob
  
  @param MemoryType      Memory type
  @param MemoryBase      Pointer to the memory base
  @param MemorySize      Ponter to memory size in bytes
  
  @retval EFI_SUCCESS The memory info is stored
  
**/
EFI_STATUS
EFIAPI
SetMemoryInfo(
  IN MemType  MemoryType,
  IN UINT64   *MemoryBase,
  IN UINT64   *MemorySize
  );

/**
  Retrieve memory related info (base, size, type) in Hob
  
  @param MemoryType      Memory type
  @param MemoryBase      Pointer to the memory base
  @param MemorySize      Ponter to memory size in bytes
  
  @retval EFI_SUCCESS    The memory info is retrieved
  @retval EFI_NOT_FOUND  The memory info not found
  
**/
EFI_STATUS
EFIAPI
GetMemoryInfo(
  IN MemType  MemoryType,
  OUT UINTN  *MemoryBase OPTIONAL,
  OUT UINTN  *MemorySize OPTIONAL
  );

/**
  Display Cycle Counter value in decimal with message
  Call:   LogMsg("Start");
  Format: CC: 36610641 LMsg: Start
**/
VOID
LogMsg (CHAR8* Msg);

/**
  Display Cycle Counter, and one 32 bit data valuein decimal
  Call:   LogMsgData(0x300, "Start");
  Format: CC: 36610641 0x300 LMsg: Start
**/
VOID
LogMsgDataU32 (UINT32 UInt32Data, CHAR8* Msg);

/**
  GetTimerCountus();
   
  Get Running time in uS based on the Arch Timer Counter.
  This could get wrapped around based on the resolution and Speed. So
  use with caution.
  
  If the Timer is 32 bit running at 19.2 MHz, it would wrap around, to 0
  after 223 Sec
  
**/
UINT32
GetTimerCountus (VOID);

/**
  Converts counter returned by GetPerformanceCounter() into mS

**/
UINT32
ConvertTimerCountms (UINT64 TimerVal);

/**
  ConvertTimerCountms();
   
  Get Running time in mS based on the Arch Timer Counter.
  This could get wrapped around based on the resolution and Speed. So
  use with caution.
  
  If the Timer is 32 bit running at 19.2 MHz, it would wrap around, to 0
  after 223 Sec
  
**/
UINT32
GetTimerCountms (VOID);

/**
  Get pointer to UEFI InfoBlock in DXE environment

  @retval NULL on failure
**/
VOID* 
GetInfoBlkPtr (VOID);

/**
  Initialize Canary Value for Stack Protection
**/
VOID
InitStackCanary (VOID);

/**
  Safe CopyMem implementation to prevent buffer overflows by taking in both the destination and source lengths.
  
  @retval size of the copied buffer
**/
UINTN CopyMemS
(  
  IN VOID   *Destination,
  IN UINTN  DestLength, 
  IN const VOID   *Source,
  IN UINTN  SrcLength
);

UINTN BreakPointFunction (VOID);


/**
  Extract module name from given full path
  Caller to ensure sufficient buffer length in ModuleNamePtr
**/
CHAR8* ExtractModuleName (IN CHAR8* ImgPathPtr, IN CHAR8* ModuleNamePtr);

#endif /* __QCOMBASELIB_H__ */
