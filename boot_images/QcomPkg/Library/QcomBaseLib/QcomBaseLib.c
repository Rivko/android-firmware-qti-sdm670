/******************************************************************//**
 * @file QcomBaseLib.c
 *
 * @brief QcomBaseLib functions
 *
 * Copyright (c) 2011, 2013-2017 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    --------------------------------------------------- 
01/27/17   ai      Fix use of retail
09/07/16   vk      Move extract module name function here
09/02/16   ai      Add counter to ms conversion function
07/06/16   vk      Remove stack canary
10/15/15   bh      Add break point function for simplified debug scripts
09/14/15   bh      Update stack canary value
06/25/15   bh      Update LogMsg to use GetTimer methods
06/11/14   aus     Added CopyMemS function
05/15/14   kedara  Update __stack_chk_fail
02/07/13   vk      Add stack canary check
11/26/13   vk      Update GetInfoBlkPtr to UINTN
03/20/13   vk      Add GetInfoBlkPtr API
03/14/13   yg      Add GetTimerCount APIs
03/12/13   yg      Improve readable LogMsg & use Timer
03/07/13   vk      LogMsg print Hex
03/07/13   vk      Fix print parameter in LogMsg
01/17/13   vk      Fix warning
11/21/11   jz      Initial checkin
========================================================================*/

#include <PiDxe.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/ProcAsmLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>

extern EFI_GUID gQcomProdmodeInfoGuid;

/* Information about memory base, size, type */
typedef struct {
  MemType    MemoryType;
  UINT64     MemoryBase;
  UINT64     MemorySize;
} MemoryInfoType;

UINT32 TimerFreq = 0;
UINT32 FactoruS = 0;
UINT32 FactormS = 0;
#ifdef _MSC_VER
#pragma warning(disable:4312)
#endif
#ifdef _MSC_VER
#pragma warning(default:4312)
#endif


UINTN
BreakPointFunction (VOID)
{
  static volatile UINTN BkptCnt = 0;
  ++BkptCnt;
  return BkptCnt;
}

/**
 * Extern reference to GUIDs.
 */
extern EFI_GUID gQcomMemoryInfoGuid;
extern EFI_GUID gEfiInfoBlkHobGuid;

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
  )
{
  MemoryInfoType MemInfo;

  ZeroMem(&MemInfo, sizeof(MemInfo));

  if (MemoryType < MAX_MEMORY_TYPE)
    MemInfo.MemoryType = MemoryType;

  if (MemoryBase)
    MemInfo.MemoryBase = *MemoryBase;

  if (MemorySize)
    MemInfo.MemorySize = *MemorySize;
  
  
  // Build HOB to pass up memory info
  BuildGuidDataHob (&gQcomMemoryInfoGuid, &MemInfo, sizeof(MemoryInfoType));

  return EFI_SUCCESS;
}

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
  )
{
  EFI_HOB_GUID_TYPE *GuidHob;
  MemoryInfoType    *MemInfo;
  UINTN             MemBase = 0, MemSize = 0, MemType = 0;
  
  GuidHob = GetFirstGuidHob(&gQcomMemoryInfoGuid);
  if (GuidHob == NULL)
  {
    DEBUG((EFI_D_INFO, "Memory Info Not Found\n"));
    return EFI_NOT_FOUND;
  }

  MemInfo = GET_GUID_HOB_DATA(GuidHob);

  if (MemInfo != NULL)
  {
    MemBase = MemInfo->MemoryBase;
    MemSize = MemInfo->MemorySize;
    MemType = MemInfo->MemoryType;
  }

  if (MemType != MemoryType)
    return EFI_NOT_FOUND;
    
  if (MemoryBase)
    *MemoryBase = MemBase;

  if (MemorySize)
    *MemorySize = MemSize;
  
  return EFI_SUCCESS;
}

#define TIMER_REF_MICRO_SEC           1000000
#define TIMER_REF_MILLI_SEC           1000

STATIC INT32
InitFactors (VOID)
{
  UINT64 TempFreq, StartVal, EndVal;

  TempFreq = GetPerformanceCounterProperties (&StartVal, &EndVal);
  /* This code doesn't support less than Ref timer Speed */
  if ((TempFreq <= TIMER_REF_MICRO_SEC) || (TempFreq <= TIMER_REF_MILLI_SEC))
    return -1;

  /* Assume the timer runs slower than 4GHz */
  if (TempFreq >= 0x100000000ULL)
    return -2;
    
 /* Assume the counter always counts up */
  if (StartVal >= EndVal)
    return -3;

  TimerFreq = (UINT32)(TempFreq & 0xFFFFFFFFULL);
  FactoruS  = (UINT32)DivU64x32(TimerFreq, TIMER_REF_MICRO_SEC);
  FactormS  = (UINT32)DivU64x32(TimerFreq, TIMER_REF_MILLI_SEC);

  return 0;
}

/* This log message consumes around 15uS */
VOID
LogMsg (CHAR8* Msg)
{
  DEBUG ((EFI_D_WARN, "%a : %d\n", Msg, GetTimerCountus()));
}

/* This log message consumes around 18uS */
VOID
LogMsgDataU32 (UINT32 UInt32Data, CHAR8* Msg)
{
  DEBUG ((EFI_D_WARN, "%a : %d 0x%x\n", Msg, GetTimerCountus(), UInt32Data));
}

/* Convert ArmArchCounter to time unit (mS, uS) based on given factor */
STATIC UINT32
ConvertTimerCountEx(UINT64 TimerVal, UINT32 *Factor)
{
  if (*Factor == 0)
    if (InitFactors () != 0)
      return 0;

  return (UINT32)DivU64x32(TimerVal, *Factor);
}

UINT32
ConvertTimerCountms(UINT64 TimerVal)
{
  return ConvertTimerCountEx(TimerVal, &FactormS);
}

UINT32
GetTimerCountus (VOID)
{
  return ConvertTimerCountEx(GetPerformanceCounter(), &FactoruS);
}

UINT32
GetTimerCountms (VOID)
{
  return ConvertTimerCountEx(GetPerformanceCounter(), &FactormS);
}


VOID* 
GetInfoBlkPtr (VOID)
{
  EFI_HOB_GUID_TYPE *GuidHob;
  UINTN **DataPtr;

  GuidHob = GetFirstGuidHob(&gEfiInfoBlkHobGuid);

  if (GuidHob == NULL)
    return NULL;

  DataPtr = GET_GUID_HOB_DATA(GuidHob);
  return  (VOID*) *DataPtr;
}

UINTN CopyMemS
(  
  IN VOID   *Destination,
  IN UINTN  DestLength, 
  IN const VOID   *Source,
  IN UINTN  SrcLength
) 
{
   if(DestLength >= SrcLength) {
      CopyMem(Destination, Source, SrcLength);
      return SrcLength;
    }
  
    CopyMem(Destination, Source, DestLength);
    return DestLength;
}  

CHAR8* ExtractModuleName (IN CHAR8* ImgPathPtr, IN CHAR8* ModuleNamePtr)
{
  CHAR8* ModulePtr = ImgPathPtr;
  UINTN  Len;

  if (ModulePtr == NULL || ModuleNamePtr == NULL) {
    return NULL;
  }

  Len = AsciiStrLen (ImgPathPtr);

  ModulePtr += Len;

  if (Len > 2)
    --ModulePtr;

  while (ModulePtr && *ModulePtr)
  {
    if ((*ModulePtr == '/') || (*ModulePtr == '\\'))
    {
      ++ModulePtr;
      break;
    }
    --ModulePtr;
  }

  if (ModulePtr == NULL) {
    return NULL;
  }

  AsciiStrCpy (ModuleNamePtr, ModulePtr);

  ModulePtr = ModuleNamePtr;

  while (ModulePtr && *ModulePtr)
  {
    if (*ModulePtr == '.')
    {
      *ModulePtr = 0;
      break;
    }
    ++ModulePtr;
  }
  
  return ModuleNamePtr;
}

/**
  Check ProdmodeInfo HOB.

  @retval BOOLEAN RetailMode - Yes/No
 */
BOOLEAN IsInRetailMode ( VOID )
{
  EFI_HOB_GUID_TYPE *GuidHob;
  BOOLEAN           *ProdmodeInfo;

  //Get Retail info from Hob
  GuidHob = GetFirstGuidHob(&gQcomProdmodeInfoGuid);
  if (GuidHob == NULL)
  {
    DEBUG((EFI_D_ERROR, "Prodmode HOB not found\n"));
    return FALSE;
  }

  ProdmodeInfo = GET_GUID_HOB_DATA(GuidHob);
  return *ProdmodeInfo;
}

