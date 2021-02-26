/*==============================================================================

FILE:      TimetickUEFI.c

DESCRIPTION: This file implements the routines to read/write to the timetick 
             Qtimer registers. 

===========================================================================
             Copyright (c) 2012-2013 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================

                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/27/12   pbi      Created for Badger family support.
 02/07/13   pbi      Added Timetick Runtime library support.
 06/14/13   unr      Added QTimerBaseRunTime check to all functions
=============================================================================*/
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/ProcAsmLib.h>
#include "TimetickUEFI.h"
#include "TimerHWIO.h"
#include "HALhwio.h"


/*===========================================================================

                           DATA DEFINITIONS

============================================================================*/
/* Holds the QTimer registers base address*/
static UINTN       QTimerBase; 

/* Holds the QTimer registers runtime base address*/
static UINTN       QTimerBaseRunTime = 0; 

/*=============================================================================

FUNCTION Timetick_UpdateRunTimeAddress
 
=============================================================================*/
void Timetick_UpdateRunTimeAddress(UINTN address)
{
  QTimerBaseRunTime = address;

  return;
}


/*=============================================================================

FUNCTION Timetick_Init
 
=============================================================================*/
EFI_STATUS Timetick_Init(void)
{
  UINTN           QTimerACBase;

  if (QTimerBaseRunTime == 0)
  {
    /* Get the base addresses for the QTimer Counter and Access Control
       registers*/
    QTimerBase = PcdGet32(PcdQTimerV2BasePhys);
    QTimerACBase = PcdGet32(PcdQTimerAC_BasePhys);

    /* Provide access control to QTimer registers*/
    HWIO_QTMR_AC_CNTACR_n_OUTI(QTimerACBase,PcdGet32(PcdQTimerFrame),
                               HWIO_QTMR_AC_CNTACR_n_RMSK);
    HWIO_QTMR_CNTPL0ACR_OUTM((UINTN)PcdGet32(PcdQTimerV1BasePhys),
                            HWIO_QTMR_CNTPL0ACR_RMSK,
                            HWIO_QTMR_CNTPL0ACR_RMSK); 
    /* Enable user mode access to CP15 Qtimer registers through
       the register CNTKCTL*/
    WriteCNTKCTL (0x303);
  }

  return EFI_SUCCESS;

}/* END Timetick_Init */


/*=============================================================================

FUNCTION Timetick_Enable
 
=============================================================================*/
EFI_STATUS Timetick_Enable( Timetick_timer_Type nTimer, boolean enable)
{
  UINTN   QTimerAddr;

  if (nTimer != TIMETICK_QTIMER)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (QTimerBaseRunTime)
  {
     QTimerAddr = QTimerBaseRunTime;
  }
  else
  {
     QTimerAddr = QTimerBase;
  }

  HWIO_QTMR_CNTP_CTL_OUTM(QTimerAddr,
                    (HWIO_QTMR_CNTP_CTL_IMSK_BMSK |
                     HWIO_QTMR_CNTP_CTL_EN_BMSK),
                     enable);

  return EFI_SUCCESS;
}/* END Timetick_Enable */


/*==========================================================================

  FUNCTION      Timetick_Get

==========================================================================*/
EFI_STATUS Timetick_GetCount
(
   Timetick_timer_Type nTimer,
   Timetick_time_64Type *pTick
)
{
  UINT32   nCountLow, nCountHi1,nCountHi2;
  UINTN   QTimerAddr;


  if (nTimer != TIMETICK_QTIMER)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (QTimerBaseRunTime)
  {
     QTimerAddr = QTimerBaseRunTime;
  }
  else
  {
     QTimerAddr = QTimerBase;
  }
  do
  {
    nCountHi1 = HWIO_QTMR_CNTPCT_HI_IN(QTimerAddr);
    nCountLow = HWIO_QTMR_CNTPCT_LO_IN(QTimerAddr);

    /* repeat the above until the same value is read from the   
      high count register successively */
    nCountHi2 = HWIO_QTMR_CNTPCT_HI_IN(QTimerAddr);
  }while (nCountHi1 != nCountHi2);

  *pTick = (((uint64)nCountHi1 <<32)| nCountLow);

  return EFI_SUCCESS;
} /* END Timetick_Get */


/*==========================================================================

  FUNCTION      Timetick_SetMatchValue

==========================================================================*/
EFI_STATUS Timetick_SetMatchValue
(
Timetick_timer_Type nTimer,
  Timetick_time_64Type                      matchCount,
  Timetick_time_64Type                      *matchVal
)
{
  UINT32 matchValLow,matchValHi;
  UINTN   QTimerAddr;

  if (nTimer != TIMETICK_QTIMER)
  {
    return EFI_INVALID_PARAMETER;
  }

  matchValLow = matchCount & 0xFFFFFFFF;
  matchValHi = matchCount >> 32;

  if (QTimerBaseRunTime)
  {
     QTimerAddr = QTimerBaseRunTime;
  }
  else
  {
     QTimerAddr = QTimerBase;
  }

  /* We do not worry about guarding the critical section here since UEFI
     is single threaded */
  HWIO_QTMR_CNTP_CVAL_HI_OUT(QTimerAddr, matchValHi);
  HWIO_QTMR_CNTP_CVAL_LO_OUT(QTimerAddr, matchValLow);

  *matchVal = matchCount;
  return EFI_SUCCESS;

} /* END Timetick_SetMatchValue */


/*==========================================================================

  FUNCTION      Timetick_GetMatchValue

==========================================================================*/
EFI_STATUS Timetick_GetMatchValue
(
   Timetick_timer_Type nTimer,
   Timetick_time_64Type *pTick
)
{
  UINTN   QTimerAddr;
  UINT32 nCountCvalLow,nCountCvalHi;

  if (nTimer != TIMETICK_QTIMER)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (QTimerBaseRunTime)
  {
     QTimerAddr = QTimerBaseRunTime;
  }
  else
  {
     QTimerAddr = QTimerBase;
  }

  nCountCvalHi = HWIO_QTMR_CNTP_CVAL_HI_IN(QTimerAddr);
  nCountCvalLow = HWIO_QTMR_CNTP_CVAL_LO_IN(QTimerAddr);

  *pTick = (((uint64)nCountCvalHi <<32)| nCountCvalLow);

  return EFI_SUCCESS;

} /* END Timetick_GetMatchValue */


/*=============================================================================

FUNCTION Timetick_GetFreq

=============================================================================*/
EFI_STATUS Timetick_GetFreq(Timetick_timer_Type nTimer, UINT32 *nFreq) 
{

  if (nTimer != TIMETICK_QTIMER)
  {
    return EFI_INVALID_PARAMETER;
  }
  if (QTimerBaseRunTime)
  {
     *nFreq = HWIO_QTMR_CNTFRQ_IN(QTimerBaseRunTime); 
  }
  else
  {
     *nFreq = HWIO_QTMR_CNTFRQ_IN(QTimerBase); 
  }


  return EFI_SUCCESS;

}/* END Timetick_GetFreq */



