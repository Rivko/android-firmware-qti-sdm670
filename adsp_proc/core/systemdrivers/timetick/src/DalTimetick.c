/*==============================================================================

FILE:      DalTimetick.c

DESCRIPTION: This file implements the Timetick Device Driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2010 - 2013 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
                              EDIT HISTORY
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/timetick/src/DalTimetick.c#2 $
  $DateTime: 2017/07/31 00:20:19 $
  $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/23/11   pbi      Added support for PMU timer
 06/07/13   pbi      Qtimer Frame 1 for systemtimer
=========================================================================*/

/*========================================================================
                           INCLUDES
========================================================================*/
#include "DalTimetick.h"
#include "DDITimer.h"
#include "qurt_anysignal.h"
#include "qurt_sclk.h"
#include "TimerHWIO.h"
#include "HALhwio.h"
#include "DDIHWIO.h"
/*========================================================================

                           DATA DEFINITIONS

========================================================================*/

/*-----------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------*/

DALResult 
Timetick_DriverInit(TimetickDrvCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult 
Timetick_DriverDeInit(TimetickDrvCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

/*-------------------------------------------------------------------------
Following functions are declared in DalDevice Interface. 
-------------------------------------------------------------------------*/
DALResult 
Timetick_DeviceInit(TimetickClientCtxt *pCtxt)
{
  DALResult        result = DAL_SUCCESS;


  static DALSYSPropertyVar system_property_var; 
  /*declare property handle variable */
  static DALSYS_PROPERTY_HANDLE_DECLARE(timetick_property); 


  /* Get the propery handle to timetick DAL */
  if(pCtxt->pTimetickDevCtxt->DevId == 0)
  {
    if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr((char*)(pCtxt->pTimetickDevCtxt->strDeviceName),timetick_property))
    {
        return DAL_ERROR;
    }
    
  }else
  {
     if(DAL_SUCCESS != DALSYS_GetDALPropertyHandle(pCtxt->pTimetickDevCtxt->DevId,timetick_property))
     {
         return DAL_ERROR; 
     }
  }

  result = DALSYS_GetPropertyValue(timetick_property,
                                 "QTIMER_INTVECTNUM",
                         0,&system_property_var); 
  if(result != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }
  pCtxt->pTimetickDevCtxt->QTimerIntVectNum = system_property_var.Val.dwVal;  

  result = DALSYS_GetPropertyValue(timetick_property,
                                "QTIMER_BASE",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    pCtxt->pTimetickDevCtxt->QTimerBase = system_property_var.Val.dwVal;
  }
  else
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Attach to the HWIO DAL.                                               */
  /*-----------------------------------------------------------------------*/
  result = DAL_DeviceAttach(DALDEVICEID_HWIO, &pCtxt->pTimetickDevCtxt->phDalHWIO);
  if (result == DAL_SUCCESS)
  {
    result = DalHWIO_MapRegionByAddress(pCtxt->pTimetickDevCtxt->phDalHWIO, 
                              (uint8*)(system_property_var.Val.dwVal),
                              (uint8 **)&(pCtxt->pTimetickDevCtxt->QTimerBase));
  }
  if(result != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }
  result = DALSYS_GetPropertyValue(timetick_property,
                                "QTIMER_AC_OFFSET",
                        0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
   pCtxt->pTimetickDevCtxt->QTimerACBase = pCtxt->pTimetickDevCtxt->QTimerBase +
                      system_property_var.Val.dwVal;

  }
  else
  {
   return DAL_ERROR;
  }
  
  result = DALSYS_GetPropertyValue(timetick_property,
                                "QTIMER_FRAME",
                        0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
   pCtxt->pTimetickDevCtxt->QTimerFrame = system_property_var.Val.dwVal;
  }
  else
  {
   return DAL_ERROR;
  }

  /* Give access control to the QTimer Frame registers*/
  HWIO_QTMR_AC_CNTACR_n_OUTI(pCtxt->pTimetickDevCtxt->QTimerACBase,
                             pCtxt->pTimetickDevCtxt->QTimerFrame,
                             HWIO_QTMR_AC_CNTACR_n_RMSK);

    result = DALSYS_GetPropertyValue(timetick_property,
                                   "QTIMER_OFFSET",
                           0,&system_property_var); 
    if(result != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }

    pCtxt->pTimetickDevCtxt->QTimerBase = pCtxt->pTimetickDevCtxt->QTimerBase + 
                                          system_property_var.Val.dwVal;

    /* Create a sync object to guard critical sections */
    result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, 
                               &pCtxt->pTimetickDevCtxt->hSync, 
                               &pCtxt->pTimetickDevCtxt->mSyncObj);
    if(result != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }

  /* Unmask the QTimer. The default state is unmasked, but we have seen
   * instances in boot code where it is being masked. This step is to 
   * make sure it is unmasked when we are here.
   */
  HWIO_QTMR_CNTP_CTL_OUTM(pCtxt->pTimetickDevCtxt->QTimerBase,
                          HWIO_QTMR_CNTP_CTL_IMSK_BMSK,0);

  if((pCtxt->pTimetickDevCtxt->freq = (uint32)HWIO_QTMR_CNTFRQ_IN
      (pCtxt->pTimetickDevCtxt->QTimerBase)) == 0)
    {
      return DAL_ERROR;
    }

    return result;
}

DALResult 
Timetick_DeviceDeInit(TimetickClientCtxt *pCtxt)
{
    DALResult        result = DAL_SUCCESS;

    /* Delete the no-preemption sync object*/
    result = DALSYS_DestroyObject(pCtxt->pTimetickDevCtxt->hSync);

    return result;
}

DALResult 
Timetick_PowerEvent(TimetickClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult 
Timetick_Open(TimetickClientCtxt *pCtxt, uint32 dwaccessMode )
{
    return DAL_SUCCESS;
}

DALResult 
Timetick_Close(TimetickClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
Timetick_Info(TimetickClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
    return DAL_SUCCESS;
}
/*=============================================================================

                   INTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION CVT_TICKS_TO_MS

DESCRIPTION
  Converts the given ticks to milliseconds

DEPENDENCIES
  Valid ticks 

RETURN VALUE
  # of milliseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint32 cvt_ticks_to_ms
(
  uint32                ticks,
  uint32                nFreq
)
{
  uint32                milliseconds = 0;
    /* Seconds portion of duration */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Convert ticks to milliseconds, with rounding. */
  milliseconds = ((uint64)ticks * 1000 + nFreq/2) / nFreq;
 
  return milliseconds;

} /* cvt_ticks_to_ms */


/*=============================================================================

FUNCTION CVT_TIMETICK64_TO_MS

DESCRIPTION
  Converts the given ticks value to milliseconds

DEPENDENCIES
  Valid ticks

RETURN VALUE
  # of milliseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint64 cvt_timetick64_to_ms
(
  uint64                ticks,
    /* Duration to be converted into milliseconds */
  uint32                nFreq
)
{
  /* Seconds portion of duration */
  uint64                          milliseconds = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ticks >= nFreq )
  {
    /* Compute seconds portion of time period first, to avoid overflows */
    milliseconds  = (ticks / nFreq) * 1000;
    ticks        %= nFreq;
  }

  milliseconds += 
     (ticks * 1000 + nFreq/2) / nFreq;
    /* Convert remaining ticks to milliseconds, with rounding. */

  return milliseconds;

} /* cvt_timetick64_to_ms */



/*------------------------------------------------------------------------------
Following functions are extended in DalTimetick Interface. 
------------------------------------------------------------------------------*/

/*==========================================================================

  FUNCTION      Timetick_GetMinRearmSclk

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_GetMinRearmSclk
(
  TimetickClientCtxt      *pCtxt,
  DalTimetickTime32Type   *pMinRearm
)
{
  *pMinRearm = 0;

  return DAL_SUCCESS;

} /* END Timetick_GetMinRearmSclk */
/*==========================================================================

  FUNCTION      Timetick_CvtToTimetick32

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult 
Timetick_CvtToTimetick32
(
  TimetickClientCtxt      *pCtxt,
  DalTimetickTime32Type   time,
  DalTimetickUnitType     unit,
  DalTimetickTime32Type   *pTimeRet
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      time *= 60;
      /*lint -fallthrough */

    case T_MIN:
      /* Convert minutes to seconds */
      time *= 60;
      /*lint -fallthrough */

    case T_SEC:
      /* Seconds to ticks */
      time *= pCtxt->pTimetickDevCtxt->freq;
      break;

    case T_MSEC:
      /* Milliseconds to ticks */
      time = ((uint64)time * pCtxt->pTimetickDevCtxt->freq) / 1000;
      break;

    case T_USEC:
      /* Microseconds to ticks */
      time = ((uint64)time * pCtxt->pTimetickDevCtxt->freq) / 1000000;
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      DALSYS_LogEvent(pCtxt->pTimetickDevCtxt->DevId, 
                      DALSYS_LOGEVENT_INFO, 
                      "Timetick_CvtToTimetick32: Invalid timetick conversion %d", 
                      unit);
      break;
  }

  *pTimeRet = time;

   return DAL_SUCCESS;
} /* END Timetick_CvtToTimetick32 */


/*==========================================================================

  FUNCTION      Timetick_CvtFromTimetick32

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_CvtFromTimetick32
( 
  TimetickClientCtxt      *pCtxt, 
  DalTimetickTime32Type   time, 
  DalTimetickUnitType     unit, 
  DalTimetickTime32Type   *pTimeRet
) 
{
   /* Factor for converting to hours or minutes */
  uint64                          scale = 1;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /*lint -fallthrough */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /*lint -fallthrough */

    case T_SEC:
      /* Convert ticks to seconds (or minutes, or hours) */
      time /= pCtxt->pTimetickDevCtxt->freq * scale;
      break;

    case T_MSEC:
      /* Convert ticks to milliseconds */
      time = cvt_ticks_to_ms( time, pCtxt->pTimetickDevCtxt->freq );
      break;

    case T_USEC:
      /* Convert ticks to microseconds */
      time = cvt_ticks_to_ms( time * 10, pCtxt->pTimetickDevCtxt->freq ) * 100;
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:

     DALSYS_LogEvent(pCtxt->pTimetickDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                "Timetick_CvtFromTimetick32: Invalid timetick conversion %d", unit);
      break;
  }
  *pTimeRet = time;

  return DAL_SUCCESS;

}  /* END Timetick_CvtFromTimetick32 */

/*=============================================================================

  FUNCTION      Timetick_CvtToTimetick64

  DESCRIPTION   See DDITimetick.h
=============================================================================*/
DALResult Timetick_CvtToTimetick64
(
  TimetickClientCtxt    *pCtxt,
  DalTimetickTime64Type time,
  DalTimetickUnitType   unit,
  DalTimetickTime64Type *pTimeRet
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      time *= 60;
      /* fall-through */

    case T_MIN:
      /* Convert minutes to seconds */
      time *= 60;
      /* fall-through */

    case T_SEC:
      /* Seconds to ticks */
      time *= pCtxt->pTimetickDevCtxt->freq;
      break;

    case T_MSEC:
      /* Milliseconds to ticks */
      time = (time * pCtxt->pTimetickDevCtxt->freq)/ 1000;
      break;

    case T_USEC:
      /* Microseconds to ticks */
      time = ((time * pCtxt->pTimetickDevCtxt->freq) / 1000000);
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      DALSYS_LogEvent(pCtxt->pTimetickDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                "Timetick_CvtToTimetick64: Invalid timetick conversion %d", unit);
      break;

  }

  *pTimeRet = time;

  return DAL_SUCCESS;

} /* Timetick_CvtToTimetick64 */

/*=============================================================================

  FUNCTION      Timetick_CvtFromTimetick64

  DESCRIPTION   See DDITimetick.h
=============================================================================*/
DALResult Timetick_CvtFromTimetick64
(
  TimetickClientCtxt    *pCtxt,
  DalTimetickTime64Type time,
  DalTimetickUnitType   unit,
  DalTimetickTime64Type *pTimeRet
)
{
  /* Factor for converting to hours or minutes */
  uint64                          scale = 1;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /* fall-through */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /* fall-through */

    case T_SEC:
      /* Convert slow clocks to seconds (or minutes, or hours) */
      time /= pCtxt->pTimetickDevCtxt->freq * scale;
      break;

    case T_MSEC:
      /* Convert ticks to milliseconds */
      time = cvt_timetick64_to_ms( time, pCtxt->pTimetickDevCtxt->freq );
      break;

    case T_USEC:
      /* Convert ticks to microseconds */
      time = ((uint64)time * 1000000) / (uint64)pCtxt->pTimetickDevCtxt->freq;
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      DALSYS_LogEvent(pCtxt->pTimetickDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                "Timetick_CvtFromTimetick64: Invalid timetick conversion %d", unit);
      break;
  }

  *pTimeRet = time;

  return DAL_SUCCESS;

} /* Timetick_CvtFromTimetick64 */


/*==========================================================================

  FUNCTION      Timetick_GetRaw

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_GetRaw
(
  TimetickClientCtxt          *pCtxt,
  DalTimetickTime32Type         *pTick
)
{
  uint64 utimer = 0;

  if ( pTick == NULL )
  {
    return DAL_ERROR;
  }

  __asm__ volatile ("%[utimer] = UTIMER" : [utimer] "=r"(utimer));
  *pTick = (DalTimetickTime32Type)utimer;

  return DAL_SUCCESS;
}/* END Timetick_GetRaw */



/*==========================================================================

  FUNCTION      Timetick_UpdateFreq

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_UpdateFreq
(
  TimetickClientCtxt    *pCtxt,
  uint32                nFreq
)
{
  DALResult result = DAL_SUCCESS; 

  pCtxt->pTimetickDevCtxt->freq = nFreq;

  /* Trigger the Event for the Frequency Change notification*/
  result = DALSYS_EventCtrlEx(
    pCtxt->pTimetickDevCtxt->hFreqChgEvent,
    DALSYS_EVENT_CTRL_TRIGGER,
    NULL,
    &nFreq,
    sizeof(uint32)
  );


  return result;

} /* END Timetick_UpdateFreq */


/*=============================================================================

FUNCTION Timetick_Enable
 
DESCRIPTION See DDITimetick.h

=============================================================================*/


DALResult Timetick_Enable
(
    TimetickClientCtxt *pCtxt,
    boolean             enable
)
{
      HWIO_QTMR_CNTP_CTL_OUTM(pCtxt->pTimetickDevCtxt->QTimerBase,
                          HWIO_QTMR_CNTP_CTL_EN_BMSK,enable);
  return DAL_SUCCESS;
}/* END Timetick_Enable */


/*==========================================================================

  FUNCTION      Timetick_SetOffset

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_SetOffset
(
  TimetickClientCtxt          *pCtxt,
  DalTimetickTime32Type          timetickDelta
)
{

//  do nothing

  return DAL_SUCCESS;
} /* END Timetick_SetOffset */

 /*==========================================================================

  FUNCTION      Timetick_GetOffset

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_GetOffset
(
  TimetickClientCtxt           *pCtxt,
  DalTimetickTime32Type          *timetickDelta
)
{
  *timetickDelta = 0;
  return DAL_SUCCESS;
} /* END Timetick_GetOffset */

/*==========================================================================

  FUNCTION      Timetick_Get

  DESCRIPTION   See DDITimetick.h

==========================================================================*/
DALResult Timetick_Get
(
  TimetickClientCtxt          *pCtxt,
  DalTimetickTime32Type       *pTick
)
{

  uint64 utimer = 0;

  if ( pTick == NULL )
  {
    return DAL_ERROR;
  }

  __asm__ volatile ("%[utimer] = UTIMER" : [utimer] "=r"(utimer));
  *pTick = (DalTimetickTime32Type)utimer;

  return DAL_SUCCESS;

} /* END Timetick_Get */

/*==========================================================================

  FUNCTION      Timetick_InitTimetick64

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_InitTimetick64
(
  TimetickClientCtxt * pCtxt
)
{
  /* do nothing*/
  return DAL_SUCCESS;

} /* END Timetick_InitTimetick64 */

/*==========================================================================

  FUNCTION      Timetick_GetTimetick64

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_GetTimetick64
(
  TimetickClientCtxt    *pCtxt,
  uint64                *pTicks
)
{
  uint64 utimer = 0;

  if ( pTicks == NULL )
  {
    return DAL_ERROR;
  }

  __asm__ volatile ("%[utimer] = UTIMER" : [utimer] "=r"(utimer));
  *pTicks = utimer;

  return DAL_SUCCESS;

} /* END Timetick_GetTimetick64 */

/*==========================================================================

  FUNCTION      Timetick_GetElapsed

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult Timetick_GetElapsed
(
  TimetickClientCtxt      *pCtxt,
  DalTimetickTime32Type   start,
  DalTimetickUnitType     unit,
  uint32                  *pDiff
)
{
  /* Time Tick count */
  DalTimetickTime32Type                   end = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current timetick count value for the end of the interval */
  Timetick_Get(pCtxt, &end);

  /* Compute and return the difference between the timestamps */
  Timetick_Diff(pCtxt, start, end, unit, pDiff);

  return DAL_SUCCESS;
}/* END Timetick_GetElapsed */

/*==========================================================================

  FUNCTION      Timetick_Diff

  DESCRIPTION   See DDITimetick.h

==========================================================================*/
DALResult Timetick_Diff
(
  TimetickClientCtxt          *pCtxt,
  DalTimetickTime32Type       start,
  DalTimetickTime32Type       end,
  DalTimetickUnitType         unit,
  DalTimetickTime32Type       *pDiff
)
{
     /* Time Tick count */
  DalTimetickTime32Type                   delta;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Compute the difference between the timetick timestamps */
  delta = end - start;

  /* Compute and return the difference between the timestamps */
  Timetick_CvtFromTimetick32( pCtxt, delta, unit, pDiff);

  return DAL_SUCCESS;
}/* END Timetick_Diff */

/*=============================================================================

FUNCTION Timetick_GetMs

DESCRIPTION   See DDITimetick.h

=============================================================================*/

DALResult Timetick_GetMs
(
    TimetickClientCtxt    *pCtxt,
    DalTimetickTime32Type *pMsecs
)
{
  /* Current time (or delta) in ticks */
  DalTimetickTime32Type                   ticks = 0;

  /* Seconds since origin time */
  DalTimetickTime32Type                   secs = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Start critical section */
  DALSYS_SyncEnter(pCtxt->pTimetickDevCtxt->hSync);

  /* Get the elapsed time since the arbitrary origin */
  Timetick_Get(pCtxt, &ticks);
  
  ticks = ticks - pCtxt->pTimetickDevCtxt->ms_info.origin;

  /* Compute number of seconds that have elapsed. */
  secs = ticks / pCtxt->pTimetickDevCtxt->freq;

  /* If over a second has elapsed, advance origin. */
  if ( secs > 0 )
  {
    ticks                  -= secs * pCtxt->pTimetickDevCtxt->freq;
    pCtxt->pTimetickDevCtxt->ms_info.origin += secs * pCtxt->pTimetickDevCtxt->freq;
    pCtxt->pTimetickDevCtxt->ms_info.msec += secs * 1000;
  }

  /* Compute millisecond timestamp */
  *pMsecs = ticks * 1000 / pCtxt->pTimetickDevCtxt->freq + pCtxt->pTimetickDevCtxt->ms_info.msec;

  /* End critical section */
  DALSYS_SyncLeave(pCtxt->pTimetickDevCtxt->hSync);

  return DAL_SUCCESS;

} /* END Timetick_GetMs */

/*=============================================================================

FUNCTION Timetick_GetFreq

DESCRIPTION   See DDITimetick.h

=============================================================================*/
DALResult 
Timetick_GetFreq( TimetickClientCtxt * pCtxt, uint32 *nFreq) 
{
    *nFreq = pCtxt->pTimetickDevCtxt->freq;

    return DAL_SUCCESS;
}/* END Timetick_GetFreq */

/*=============================================================================

FUNCTION Timetick_UpdateBlastSigId

DESCRIPTION   See DDITimetick.h

=============================================================================*/
DALResult 
Timetick_UpdateBlastSigId( TimetickClientCtxt * pCtxt, uint32 nSigId) 
{
    pCtxt->pTimetickDevCtxt->blast_sig_id = nSigId;
    return DAL_SUCCESS;
}/* END Timetick_UpdateBlastSigId */

/*=============================================================================

FUNCTION Timetick_CvtToUsec

DESCRIPTION See DDITimetick.h

=============================================================================*/
DALResult 
Timetick_CvtToUsec
( 
  TimetickClientCtxt    *pCtxt, 
  DalTimetickTime32Type time, 
  DalTimetickUnitType   unit, 
  DalTimetickTime64Type *pTimeRet
) 
{
  DalTimetickTime64Type   usecs;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  usecs = (DalTimetickTime64Type)time;

  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      usecs *= 60;
      /* fall-through */
    case T_MIN:
      /* Convert minutes to seconds */
      usecs *= 60;
      /* fall-through */
    case T_SEC:
      /* Seconds to slow clocks */
      usecs *= 1000;
    case T_MSEC:
      /* Milliseconds to slow clocks */
      usecs = usecs * 1000;
    case T_USEC:
      break;
    case T_TICK:
      usecs = ((uint64)time * 1000000) / (uint64)pCtxt->pTimetickDevCtxt->freq;
      break;

    default:
      DALSYS_LogEvent(pCtxt->pTimetickDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                "Timetick_CvtToUsec: Invalid timetick conversion %d", unit);
      break;
  }

  *pTimeRet = usecs;

  return DAL_SUCCESS;
}/* Timetick_CvtToUsec*/

/*=============================================================================

FUNCTION Timetick_CvtFromUsec

DESCRIPTION See DDITimetick.h 
 
=============================================================================*/
DALResult 
Timetick_CvtFromUsec
( 
  TimetickClientCtxt    *pCtxt, 
  DalTimetickTime64Type time, 
  DalTimetickUnitType   unit, 
  DalTimetickTime32Type *pTimeRet
) 
{
  /* Factor for converting to hours or minutes */
  uint32                          scale = 1;

  /* Converted value in 32-bit integer */
  DalTimetickTime32Type           converted_time = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /* fall-through */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /* fall-through */

    case T_SEC:
      /* Convert slow clocks to seconds (or minutes, or hours) */
      scale *= 1000;

    case T_MSEC:
      scale *= 1000;

    case T_USEC:
      converted_time = (DalTimetickTime32Type)(time/scale);
      break;

    case T_TICK:
      converted_time = (time * pCtxt->pTimetickDevCtxt->freq) / 1000000;
      break;

    default:
      DALSYS_LogEvent(pCtxt->pTimetickDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                "Timetick_CvtFromUsec: Invalid timetick conversion %d", unit);
      break;

  }

  *pTimeRet = converted_time;

  return DAL_SUCCESS;

} /* Timetick_CvtFromUsec */



/*==========================================================================

  FUNCTION      Timetick_GetMatchValue

  DESCRIPTION   See DDITimetick.h

==========================================================================*/
DALResult Timetick_GetMatchValue
(
  TimetickClientCtxt          *pCtxt,
  DalTimetickTime32Type       *pTick
)
{

  *pTick = pCtxt->pTimetickDevCtxt->last_match_value;
  return DAL_SUCCESS;

} /* END Timetick_GetMatchValue */

/*==========================================================================

  FUNCTION      Timetick_TicksToPrecUs

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult 
Timetick_TicksToPrecUs
(
  TimetickClientCtxt    *pCtxt,
  DalTimetickTime32Type nTicks,
  uint64                *pus
)
{
  *pus = ((uint64)nTicks * 1000000) / (uint64)pCtxt->pTimetickDevCtxt->freq;

  return DAL_SUCCESS;
} /* END Timetick_TicksToPrecUs */

/*==========================================================================

  FUNCTION      Timetick_SetNextInterrupt64

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult 
Timetick_SetNextInterrupt64
(
  TimetickClientCtxt    *pCtxt,
  DalTimetickTime64Type  matchCount,
  DalTimetickTime64Type  ticksNow,
  DalTimetickTime64Type *matchVal
)
{
  uint32 matchValLow = 0,matchValHi = 0;

    matchValLow = matchCount & 0xFFFFFFFF;
    matchValHi = matchCount >> 32;

    /* Start critical section */
    DALSYS_SyncEnter(pCtxt->pTimetickDevCtxt->hSync);

    HWIO_QTMR_CNTP_CVAL_HI_OUT(
      pCtxt->pTimetickDevCtxt->QTimerBase, matchValHi);

    HWIO_QTMR_CNTP_CVAL_LO_OUT(pCtxt->pTimetickDevCtxt->QTimerBase,
                                matchValLow);

    /* End critical section */
    DALSYS_SyncLeave(pCtxt->pTimetickDevCtxt->hSync);

    *matchVal = matchCount;

  return DAL_SUCCESS;
} /* END Timetick_SetNextInterrupt64 */


/*==========================================================================

  FUNCTION      Timetick_GetMatchValue64

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult 
Timetick_GetMatchValue64
(
  TimetickClientCtxt    *pCtxt,
  DalTimetickTime64Type *pMatchVal
)
{

  DalTimetickTime32Type nCountCvalLow,nCountCvalHi;

  if (pCtxt->pTimetickDevCtxt->QTimerBase != NULL )
  {

    nCountCvalHi = HWIO_QTMR_CNTP_CVAL_HI_IN(
                                  pCtxt->pTimetickDevCtxt->QTimerBase);
    nCountCvalLow = HWIO_QTMR_CNTP_CVAL_LO_IN(
                                  pCtxt->pTimetickDevCtxt->QTimerBase);

    *pMatchVal = (((uint64)nCountCvalHi <<32)| nCountCvalLow);
  }
  return DAL_SUCCESS;
} /* END Timetick_GetMatchValue64 */  

/*=============================================================================

FUNCTION Timetick_RegFreqChangeNotification

DESCRIPTION   See DDITimetick.h

=============================================================================*/

DALResult Timetick_RegFreqChangeNotification
(
  TimetickClientCtxt *pCtxt,
  DALSYSCallbackFunc cbFunc,
  DALSYSCallbackFuncCtx cbFuncCtx
)
{
  DALResult result = DAL_SUCCESS;

   /* Create the frequency change notification event which is used to notify
    * the client that a frequency change has occurred. 
    */
   result = DALSYS_EventCreate(
     DALSYS_EVENT_ATTR_NORMAL | DALSYS_EVENT_ATTR_CALLBACK_EVENT,
      &pCtxt->pTimetickDevCtxt->hFreqChgEvent,
      &pCtxt->pTimetickDevCtxt->freqChgNotifyObject);
   if(result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(pCtxt->pTimetickDevCtxt->DevId, 
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      "Timetick could not create the frequency change event");
      return result;
   }

   /* Setup the frequency change event as a callback event */
   result = DALSYS_SetupCallbackEvent(
      pCtxt->pTimetickDevCtxt->hFreqChgEvent,
      cbFunc,
      cbFuncCtx);

   if(result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(pCtxt->pTimetickDevCtxt->DevId, 
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      "Timetick could not setup the frequency change event");
   }
   return result;
 }/* END Timetick_RegFreqChangeNotification */

/*=============================================================================

FUNCTION Timetick_GetInterruptVectorNum

DESCRIPTION   See DDITimetick.h

=============================================================================*/

DALResult Timetick_GetInterruptVectorNum
(
  TimetickClientCtxt *pCtxt,
  uint32 *num
)
{
  *num = pCtxt->pTimetickDevCtxt->QTimerIntVectNum;

  return DAL_SUCCESS;
 }/* END Timetick_GetInterruptVectorNum */

/*=============================================================================

FUNCTION Timetick_MaskInterrupt
 
DESCRIPTION See DDITimetick.h

=============================================================================*/

DALResult Timetick_MaskInterrupt
(
    TimetickClientCtxt *pCtxt,
    boolean             bMask
)
{
  uint32 nOutMask = bMask << HWIO_QTMR_CNTP_CTL_IMSK_SHFT;

  HWIO_QTMR_CNTP_CTL_OUTM(pCtxt->pTimetickDevCtxt->QTimerBase,
                          HWIO_QTMR_CNTP_CTL_IMSK_BMSK,nOutMask);
  return DAL_SUCCESS;
}/* END Timetick_MaskInterrupt */



