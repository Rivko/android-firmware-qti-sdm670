/*==============================================================================

FILE:      Busywait.c

DESCRIPTION: This file implements the busywait routine using Qtimer registers. 

===========================================================================
             Copyright (c) 2014 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================

                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/14/14  pbitra   Created.
=============================================================================*/
#include <Uefi.h>
#include <Library/PcdLib.h>
#include "TimerHWIO.h"
#include "HALhwio.h"
#include "DALSys.h"


/*===========================================================================

                           DATA DEFINITIONS

============================================================================*/
/* Holds the QTimer registers base address*/
static UINTN       QTimerBase; 

/*Holds the QTimer frequency */
static UINT32       QTimerFreq;


/*=============================================================================

FUNCTION busywait_init
 
=============================================================================*/
DALResult busywait_init(void)
{
  UINTN           QTimerACBase;
 
  /* Get the base addresses for the QTimer Counter and Access Control
       registers*/
    QTimerBase = PcdGet32(PcdQTimerV1BasePhys);
    QTimerACBase = PcdGet32(PcdQTimerAC_BasePhys);

    /* Provide access control to QTimer registers*/
    HWIO_QTMR_AC_CNTACR_n_OUTI(QTimerACBase,PcdGet32(PcdQTimerFrame),
                               HWIO_QTMR_AC_CNTACR_n_RMSK);
    if(QTimerBase != 0)
    {
      QTimerFreq =  HWIO_QTMR_CNTFRQ_IN(QTimerBase);
    }
    
    return DAL_SUCCESS;

}/* END busywait_init */


/*==========================================================================

  FUNCTION      busywait
==========================================================================*/
void busywait
(
  UINT32 pause_time_us
)
{
  UINT32   start_count, delay_count;

  if (QTimerBase != 0 )
  {
    start_count = in_dword(QTimerBase);
    /*
     * Perform the delay and handle potential overflows of the timer.
     */
  
    delay_count = (pause_time_us * (uint64)QTimerFreq)/1000000;
    while ((HWIO_QTMR_CNTPCT_LO_IN(QTimerBase) - start_count) < delay_count);
  }
  return;
} /* END busywait */




