/**
 @file FBPTLib.c
 
 @brief FBPT related functions

 Copyright (c) 2012-2016 by Qualcomm Technologies, Inc. 
 All Rights Reserved.
 
 **/

/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    ---------------------------------------------------
04/15/16   vk      Exit after trying to read MPM timer, for RUMI
09/10/15   vk      PRESIL for RUMI
04/09/15   vk      Return unsupported if FBPTBase is not set
10/27/14   jb      Change FBPT base to uint64
07/09/14   vk      Fix crash reading 8 byte from MPM register
05/08/14   vk      Add 64 bit support  
04/09/14   vk      Add basic table
10/31/13   vk      Stub out
06/20/12   vishalo Initial checkin
========================================================================*/

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/ProcAsmLib.h>
#include <Library/FBPT.h>

#define HWIO_IN(x) (*((volatile UINT32*)(x)))

/* Per SWI, timer will overflow after 2^31-1 = 2147483647 */
#define LOG_MAX_COUNT_VAL  2147483647
#define COUNTER_OVERFLOW   0xFFFFFFFF
#define RETRY_COUNT        1000


/**
  Get number of cycles from power on

  @return Number of cycles
          0xFFFFFFFF if overflow detected
**/
UINT32 
BootGetCounter(VOID)
{
  volatile UINT32 curr_count;
  volatile UINT32 last_count;
  volatile UINT32 retry_count;

  retry_count = RETRY_COUNT;

  /*Grab current time count*/
  curr_count = HWIO_IN((UINTN)PcdGet32(PcdTimeTickClkAddress));

  /*Keep grabbing the time until a stable count is given*/
  do 
  {
    last_count = curr_count;
    curr_count = HWIO_IN((UINTN)PcdGet32(PcdTimeTickClkAddress));
    
    retry_count--;
    if (retry_count == 0)
      return curr_count;

  } while (curr_count != last_count);

  if(curr_count < LOG_MAX_COUNT_VAL)
  {
    return curr_count;
  }
  else 
    return COUNTER_OVERFLOW;
}

/**
  Get Time from power on 

  @return Time in nanoseconds
          0 if overflow detected
**/
UINT32 
BootGetTimeNanoSec(VOID)
{
  UINT32 TimeTickCount;
  TimeTickCount = BootGetCounter();

  if(TimeTickCount == COUNTER_OVERFLOW)
    return 0;

  /*
  Clock frequency is 32.768 KHz
  1 / (32.768 KHz) = 30517.5781 nanoseconds ~= 30518 ns
  */
  return (TimeTickCount * 30518);
}

/**
  Convert ARM Performance Monitor counter cycle
  count to time in nano seconds.
 
  @param CycleCount     Number of PMon cycles
  @return Time NanoSeconds
**/
UINT64
CycleToNanoSec(UINT32 CycleCount)
{
  UINT64 Time;
  UINT64 Scale;
  UINT32 AppsProcClkMhz;

  /* CPU frequency in MHz is converted into nseconds * 2^10. Multiplication by 2^10 is done so that the 
     value is correct upto 3 decimal places after the integer division. */  
  AppsProcClkMhz = PcdGet32(PcdAppsProcFrequencyMhz);
  Scale = ((1000 << 10)/AppsProcClkMhz);
  
  /* Time in nseconds = Number of cycles * Scale * 64 because cycle couter set to count every 64 */
  /* Right shift by 10 (used in calculation of scale) and Left shift 6 (multiply by 64) => Right shift by 4 */
  Time = ( ((UINT64) CycleCount * Scale) >> 4); 
  return  Time;
}

/**
  Creates FBPT structure, and initialize Reset Time field
 

  @return EFI_STATUS    Success or failure
**/
EFI_STATUS
InitializeFBPT (VOID)
{
  STATIC UefiPerfRecord_t *UefiPerfRecord;
  STATIC UINTN FBPTPayloadMemoryBase = 0;

  FBPTPayloadMemoryBase =  (UINTN)PcdGet64(PcdFBPTPayloadBaseOffset);
  UefiPerfRecord =  (UefiPerfRecord_t*) FBPTPayloadMemoryBase;

  if (FBPTPayloadMemoryBase == 0)
    return EFI_UNSUPPORTED;

  /* Fill FBPT Header */
  UefiPerfRecord->UefiFBPTHeader.Signature[0] = 'F';
  UefiPerfRecord->UefiFBPTHeader.Signature[1] = 'B';
  UefiPerfRecord->UefiFBPTHeader.Signature[2] = 'P';
  UefiPerfRecord->UefiFBPTHeader.Signature[3] = 'T';
  UefiPerfRecord->UefiFBPTHeader.Length = sizeof(UefiPerfRecord_t);
 
  /* Perf Data Record */
  UefiPerfRecord->UefiFBBPDataRecord.PerformanceRecordType = 2;
  UefiPerfRecord->UefiFBBPDataRecord.RecordLength = 48;
  UefiPerfRecord->UefiFBBPDataRecord.Revision = 2;
  UefiPerfRecord->UefiFBBPDataRecord.Reserved = 0;
  UefiPerfRecord->UefiFBBPDataRecord.ResetEnd = CycleToNanoSec(ReadCycleCntReg()); 

  /* Dummy values for now */
  UefiPerfRecord->UefiFBBPDataRecord.OSLoaderLoadImageStart =  0ULL;
  UefiPerfRecord->UefiFBBPDataRecord.OSLoaderStartImageStart = 0ULL;
  UefiPerfRecord->UefiFBBPDataRecord.ExitBootServicesEntry =   0ULL;
  UefiPerfRecord->UefiFBBPDataRecord.ExitBootServicesExit =    0ULL;
  
  return EFI_SUCCESS;
}
