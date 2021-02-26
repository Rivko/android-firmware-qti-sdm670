/*
===========================================================================

FILE:         HALTimer.c

DESCRIPTION:  
  This is the generic hardware abstraction layer implementation for the
  timetick module.

===========================================================================
             Copyright (c) 2011-12 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================

                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/27/12   pbi      Removed HALTimerInternal.h after moving its contents to
                     HALTimer.h
 01/18/12   pbi      Added new API, HAL_timer_WriteMatchValue
 11/28/11   vishalo  Revert changes  HAL_timer_ReadCount(...) - fix UEFI boot
 03/15/11   pbi      Added a new function, HAL_timer_GetDivider.
                     Removed the obsolete logic to read the timer twice or
                     more to make sure the value is stable. 
 02/02/11   pbi      Added a new function, HAL_timer_ClearOnMatchEnable


=============================================================================*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "HALTimer.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */
/* Array of pointers to HAL_timer_InfoType structures */
HAL_timer_InfoType aTimerData[HAL_NUM_TIMERS];  


/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */


/* =======================================================================

  FUNCTION      HAL_timer_GetData

  DESCRIPTION   Retrieves the information related to the given timer from
                aTimerData.
 
======================================================================== */

static void HAL_timer_GetData(HAL_timer_Type timer, HAL_timer_InfoType **pData)
{
    *pData = &aTimerData[timer];
}/* End HAL_timer_GetData */

/* =======================================================================

  FUNCTION      HAL_timer_Init

  DESCRIPTION   See HALTimer.h
 
======================================================================== */

void HAL_timer_Init(void)
{
  HAL_timer_InfoType *pTimerInfo = &aTimerData[0];
  HAL_timer_InitPlatform(&pTimerInfo);
}/* End HAL_timer_Init */


/* =======================================================================

  FUNCTION      HAL_timer_Enable

  DESCRIPTION   See HALTimer.h
 
======================================================================== */


void HAL_timer_Enable (HAL_timer_Type nTimer, boolean bEn)
{
    HAL_timer_InfoType *pData;

    HAL_timer_GetData(nTimer, &pData);
    if ((void*)pData->nEnAddress != NULL)
    {
      if (bEn != 0) {
          outpdw(pData->nEnAddress, inpdw(pData->nEnAddress) |(1 << pData->nEnShift));
      }
      else {
          outpdw(pData->nEnAddress, inpdw(pData->nEnAddress) & ~(1 << pData->nEnShift));
      }
    }

} /* End HAL_timer_Enable */
/* =======================================================================

  FUNCTION      HAL_timer_ClearOnMatchEnable

  DESCRIPTION   See HALTimer.h
 
======================================================================== */


void HAL_timer_ClearOnMatchEnable (HAL_timer_Type nTimer, boolean bEn)
{
    HAL_timer_InfoType *pData;

    HAL_timer_GetData(nTimer, &pData);
    if ((void*)pData->nEnAddress != NULL)
    {
      if (bEn != 0) {
          outpdw(pData->nEnAddress, inpdw(pData->nEnAddress) |(1 << pData->nEnClearOnMatchShift));
      }
      else {
          outpdw(pData->nEnAddress, inpdw(pData->nEnAddress) & ~(1 << pData->nEnClearOnMatchShift));
      }
    }

} /* End HAL_timer_Enable */

/* =======================================================================

  FUNCTION      HAL_timer_ReadCount

  DESCRIPTION   See HALTimer.h
 
======================================================================== */
void HAL_timer_ReadCount (HAL_timer_Type nTimer, uint32 *pnCount)
{
     HAL_timer_InfoType *pData;
     /* Last value of "timetick", used to verify that the value read is stable */
     uint32 nLastCount;

     HAL_timer_GetData(nTimer, &pData);
     if (pData->nCountValAddress != NULL)
     {
         /* Reading the counter value once may not return an accurate value if the
           counter is in the processing of counting to the next value, and several
           bits are changing.  Instead, the counter is to be repeatedly read until 
           a consistent value is read.  */

        *pnCount = inpdw(pData->nCountValAddress);

        do {
          nLastCount = *pnCount;
         *pnCount = inpdw(pData->nCountValAddress);
        } while ((*pnCount - nLastCount) > 70);  
         /* Allow at most a ~10 us delay for a timer at 6.75 MHz */
         /* The counter has returned the same value twice in a row, and hence must
           be stable. */
     }


 } /* End HAL_timer_ReadCount */

/* =======================================================================

  FUNCTION      HAL_timer_SetMatchValue

  DESCRIPTION   See HALTimer.h
 
======================================================================== */

void HAL_timer_SetMatchValue 
(
  HAL_timer_Type nTimer, 
  uint32 nMatchCount, 
  uint32 nTicksNow, 
  uint32 nMinRearmSclk, 
  uint32 *pnMatchValWritten
)
{
  HAL_timer_InfoType *pData;

  HAL_timer_GetData(nTimer, &pData);

  if (pData->setMatchValueFunc != NULL)
  {
    pData->setMatchValueFunc(nMatchCount, nTicksNow, nMinRearmSclk, pnMatchValWritten);
  }
} /* End HAL_timer_SetMatchValue */

/* =======================================================================

  FUNCTION      HAL_timer_ReadMatchValue

  DESCRIPTION   See HALTimer.h
 
======================================================================== */
void HAL_timer_ReadMatchValue (HAL_timer_Type nTimer, uint32 *pnMatchValue)
{
     HAL_timer_InfoType *pData;

     HAL_timer_GetData(nTimer, &pData);
     if ((void*)pData->nMatchValAddress != NULL)
     {
         *pnMatchValue = inpdw(pData->nMatchValAddress);
     }

 } /* End HAL_timer_ReadMatchValue */

/* =======================================================================

  FUNCTION      HAL_timer_Clear

  DESCRIPTION   See HALTimer.h
 
======================================================================== */
void HAL_timer_Clear (HAL_timer_Type nTimer)
{
     HAL_timer_InfoType *pData;

     HAL_timer_GetData(nTimer, &pData);
     if ((void*)pData->nClearAddress != NULL)
     {
       /* writing anything clears the timer */
         outpdw(pData->nClearAddress, 0);
     }

 } /* End HAL_timer_Clear */


/* =======================================================================

  FUNCTION      HAL_timer_GetDivider

  DESCRIPTION   See HALTimer.h
 
======================================================================== */
void HAL_timer_GetDivider (HAL_timer_Type nTimer, uint32 *nDivider)
{
     HAL_timer_InfoType *pData;

     HAL_timer_GetData(nTimer, &pData);
     if ((void*)pData->nClkCntlAddress != NULL)
     {
       *nDivider = ((inpdw(pData->nClkCntlAddress) & pData->nClkCntlDivMask)
                     >>pData->nClkCntlDivShift)+ 1;
     }
     else
     {
       *nDivider = 1;
     }

 } /* End HAL_timer_GetDivider */

/* =======================================================================

  FUNCTION      HAL_timer_WriteMatchValue

  DESCRIPTION   See HALTimer.h
 
======================================================================== */
void HAL_timer_WriteMatchValue (HAL_timer_Type nTimer, uint32 pnMatchValue)
{
     HAL_timer_InfoType *pData;

     HAL_timer_GetData(nTimer, &pData);
     if ((void*)pData->nMatchValAddress != NULL)
     {
       outpdw(pData->nMatchValAddress, pnMatchValue);
     }

 } /* End HAL_timer_ReadMatchValue */
