#ifndef DALTIMETICK_H
#define DALTIMETICK_H

/*=========================================================================

             M A I N   D A L   T I M E T I C K   D R I V E R   
                      H E A D E R   F I L E

DESCRIPTION
  Contains the majority of functionality for the DAL Timetick driver.  
  The API in this module MUST always remain target independent.

REFERENCES

Copyright (c) 2010- 2013 by QUALCOMM Technologies Incorporated. 
All Rights Reserved.


===========================================================================*/

/*==========================================================================

                        INCLUDE FILES FOR MODULE

==========================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDITimetick.h"


/*---------------------------------------------------------------------------
Declaring a "Timetick" Driver,Device and Client Context
---------------------------------------------------------------------------*/
typedef struct TimetickDrvCtxt TimetickDrvCtxt;
typedef struct TimetickDevCtxt TimetickDevCtxt;
typedef struct TimetickClientCtxt TimetickClientCtxt;
typedef struct DalTimetickFuncTable DalTimetickFuncTable;

/*---------------------------------------------------------------------------
Declaring a private "Timetick" Vtable
-----------------------------------------------------------------------------*/
typedef struct TimetickDALVtbl TimetickDALVtbl;
struct TimetickDALVtbl
{
	int (*Timetick_DriverInit)(TimetickDrvCtxt *);
	int (*Timetick_DriverDeInit)(TimetickDrvCtxt *);
};

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


#define DALTIMETICK_NUM_UINT32_DEVICES         0        

#define DALTIMETICK_NUM_STR_DEVICES     2

/* Number of timers handled by the timetick dal */
#define DALTIMETICK_NUM_DEVICES     (DALTIMETICK_NUM_UINT32_DEVICES + DALTIMETICK_NUM_STR_DEVICES)

#define DALTIMETICK_MAX_STRING_ID_LENGTH 20

/* Information used when acquiring the millisecond value */ 
typedef struct
{
  /* origin for millisecond tick measurements */
  uint32                          origin;

  /* Millisecond count at origin point */
  uint32                          msec;
}
timetick_ms_info_struct_type;

/*
 * TimetickDevCtxt
 *
 * Data structure containing all information about the timetick device
 *
 *  dwRefs:                       Reserved.
 *  DevId:                        DAL Timetick device id.
 *  dwDevCtxtRefIdx:              Number of clients using this device.
 *  pTimetickDrvCtxt:             Pointer to the driver context.
 *  Reserved:                     Reserved.
 *  DalTimetickFuncTable:         Function Table with mapping to HAL functions.
 *  ms_info:                      Info used to get msec.
 *  timetick_delta:               Difference between app, modem & 
                                  MPM timetick counters.
 *  freq:                         Info used for timetick conversion.
 *  timetick64_overflow           Overflow offset to maintain 64-bit timetick.
 *  timetick64_initialized        Flag to indicate 64-bit timetick 
                                  has been initialized.
 *  timetick64_base:              Timetick base value for 64-bit computation.
 */
struct TimetickDevCtxt
{
    //Base Members
    uint32                          dwRefs;                                    
    DALDEVICEID                     DevId;                                  
    uint32                          dwDevCtxtRefIdx; 
    TimetickDrvCtxt                 *pTimetickDrvCtxt;                             
    DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    uint32                          Reserved[16];
    //Timetick Dev state can be added by developers here

    timetick_ms_info_struct_type    ms_info;
    uint32                          freq;  /*freq in Hz*/
    uint32                          blast_sig_id; /* used for RGPT */
    DALSYSSyncHandle                hSync;  /* DAL SYS Sync variables*/
    DALSYSSyncObj                   mSyncObj;
    DalTimetickTime32Type           last_match_value; /* last successfully programmed match value */
    DALSYSEventHandle               hFreqChgEvent;
    uint32                          QTimerBase;
    uint32                          QTimerFrame;
    uint32                          QTimerACBase;
    DalDeviceHandle                 *phDalHWIO;     /*Handle for communicating with the HWIO DAL.*/
    DALSYS_EVENT_OBJECT(freqChgNotifyObject);
    char strDeviceName[DALTIMETICK_MAX_STRING_ID_LENGTH];  
    uint32 			    QTimerIntVectNum;
};

/*
 * TimetickDrvCtxt
 *
 * Data structure containing all information about an interrupt
 *
 *  TimetickDALVtbl:     Base Members.
 *  dwNumDev:            The number of devices in Timetick, this is 4.
 *  dwSizeDevCtxt:       Driver structure size for DALSYS.
 *  bInit:               Flag to indicate the driver has been initialized.
 *  dwRefs:              The number of clients using the driver.
 *  TimetickDevCtxt:     Timetick Device Contexts array.
 */
struct TimetickDrvCtxt
{
	//Base Members
	TimetickDALVtbl     TimetickDALVtbl;
	uint32              dwNumDev;
	uint32              dwSizeDevCtxt;
	uint32              bInit;
	uint32              dwRefs;
	TimetickDevCtxt     TimetickDevCtxt[DALTIMETICK_NUM_DEVICES];
    //Timetick Drv state can be added by developers here

};

/*---------------------------------------------------------------------------
Declaring a "Timetick" Client Context
---------------------------------------------------------------------------*/

/*
 * TimetickClientCtxt
 *
 * Data structure containing top-level Timetick access structures 
 * base members.
 *
 *  dwRefs:              References used internally.
 *  dwAccessMode:        SHARED for Timetick.
 *  pPortCtxt:           Handle to a Port context for RPC.
 *  pTimetickDevCtxt:    Handle to the Timetick device context.
 *  DalTimetickHandle:   Handle to the Timetick configuration structures.
 */
struct TimetickClientCtxt
{
   //Base Members
    uint32              dwRefs;                     
    uint32              dwAccessMode;  
    void                *pPortCtxt;
    TimetickDevCtxt     *pTimetickDevCtxt;            
    DalTimetickHandle   DalTimetickHandle; 
    //Timetick Client state can be added by developers here
};


/*==========================================================================

   GENERIC DAL API FOR DALSYS WHICH THIS DRIVER MAY OR MAY NOT IMPLEMENT

==========================================================================*/
DALResult Timetick_DriverInit(TimetickDrvCtxt *);
DALResult Timetick_DriverDeInit(TimetickDrvCtxt *);
DALResult Timetick_DeviceInit(TimetickClientCtxt *);
DALResult Timetick_DeviceDeInit(TimetickClientCtxt *);
DALResult Timetick_Reset(TimetickClientCtxt *);
DALResult Timetick_PowerEvent(TimetickClientCtxt *,
                              DalPowerCmd,
                              DalPowerDomain);
DALResult Timetick_Open(TimetickClientCtxt *, uint32);
DALResult Timetick_Close(TimetickClientCtxt *);
DALResult Timetick_Info(TimetickClientCtxt *,
                        DalDeviceInfo *, 
                        uint32);
DALResult Timetick_InheritObjects(TimetickClientCtxt *,
                                  DALInheritSrcPram *,
                                  DALInheritDestPram *);

/*==========================================================================

                     APPLICATION PROGRAMMING INTERFACE

==========================================================================*/


/*========================================================================

  FUNCTION      Timetick_GetMinRearmSclk

  DESCRIPTION   Returns the MIN_REARM_SCLK value

  PARAMETERS    *_h         - Device handle.
                *min_rearm  - MIN_REARM_SCLK value returned
                
  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_GetMinRearmSclk
( 
  TimetickClientCtxt *,  
  DalTimetickTime32Type *
);

/*========================================================================
  FUNCTION      Timetick_CvtToTimetick32

  DESCRIPTION   Converts the given time value to ticks

  PARAMETERS    *_h         - Device handle.
                time        - Time interval to convert to ticks
                unit        - Units of time interval
                *time_ret   - # of ticks which correspond to the 
                              given time value

                
  DEPENDENCIES  Valid timetick estimate

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_CvtToTimetick32
( 
  TimetickClientCtxt *, 
  DalTimetickTime32Type , 
  DalTimetickUnitType ,  
  DalTimetickTime32Type *
);


/*========================================================================

  FUNCTION      Timetick_CvtFromTimetick32

  DESCRIPTION   Converts the ticks to the given unit

  PARAMETERS    *_h         - Device handle.
                time        - Time interval to convert from ticks
                unit        - Units of time interval
                *time_ret   - Time in the unit requested
                
  DEPENDENCIES  Valid timetick estimate

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_CvtFromTimetick32
( 
  TimetickClientCtxt *, 
  DalTimetickTime32Type , 
  DalTimetickUnitType ,  
  DalTimetickTime32Type *
);

/*=============================================================================

  FUNCTION  Timetick_CvtToTimetick64

  DESCRIPTION  Converts the given time value to ticks

  PARAMETERS    *_h         - Device handle.
                time        - Time interval to convert from ticks
                unit        - Units of time interval
                *time_ret   - Time in the unit requested
                
  DEPENDENCIES  Valid timetick estimate

  RETURN VALUE  # of ticks which correspond to the given time value

  SIDE EFFECTS  None

=============================================================================*/

DALResult Timetick_CvtToTimetick64
(
  TimetickClientCtxt *,
  DalTimetickTime64Type,
  DalTimetickUnitType,
  DalTimetickTime64Type *
);
/*=============================================================================

  FUNCTION      Timetick_CvtFromTimetick64

  DESCRIPTION   Converts the slow clock time value to the given unit

  PARAMETERS    *_h         - Device handle.
                time        - Time interval to convert to ticks
                unit        - Units of time interval
                *time_ret   - Time in the unit requested
                
  DEPENDENCIES  Valid timetick estimate

  RETURN VALUE  Time in the unit requested

  SIDE EFFECTS  None


=============================================================================*/

DALResult Timetick_CvtFromTimetick64
(
  TimetickClientCtxt *,
  DalTimetickTime64Type,
  DalTimetickUnitType,
  DalTimetickTime64Type *
);

/*========================================================================

  FUNCTION      Timetick_Get

  DESCRIPTION   Returns the current timetick count, adjusted with the 
                timetick delta.

  PARAMETERS    *_h        - Device handle.
                *count     - timetick counter value
                
  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_Get
( 
  TimetickClientCtxt *,  
  DalTimetickTime32Type *
);


/*========================================================================

  FUNCTION      Timetick_GetRaw

  DESCRIPTION   Read the timetick counter, and return 
                the current count

  PARAMETERS    *_h         - Device handle.
                *tick       - timetick counter value
                
  DEPENDENCIES  Regional function.  It may only be called from time*.c

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_GetRaw
( 
  TimetickClientCtxt *,  
  DalTimetickTime32Type *      
);

/*========================================================================

  FUNCTION      Timetick_GetElapsed

  DESCRIPTION   Compute the time elapsed from a previous timetick value

  PARAMETERS    *_h         - Device handle.
                start       - Time tick value at start of interval
                unit        - Units to return time interval in
                *diff       - Elapsed time, in the unit provided
                
  DEPENDENCIES  Valid timetick estimate.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_GetElapsed
( 
  TimetickClientCtxt *, 
  DalTimetickTime32Type , 
  DalTimetickUnitType ,  
  DalTimetickTime32Type *
);

/*========================================================================

  FUNCTION      Timetick_Diff

  DESCRIPTION   Compute the difference between two tick counts

  PARAMETERS    *_h         - Device handle.
                start       - Time tick value at start of interval
                end         - Time tick value at end of interval
                unit        - Units to return time interval in
                *diff       - Time difference between the two slow clock 
                              tick counts, in the unit given
                
  DEPENDENCIES  Valid timetick estimate

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_Diff
( 
  TimetickClientCtxt *, 
  DalTimetickTime32Type , 
  DalTimetickTime32Type , 
  DalTimetickUnitType ,  
  DalTimetickTime32Type *
);


/*========================================================================

  FUNCTION      Timetick_InitTimetick64

  DESCRIPTION   This function initializes the 64-bit timetick system by 
                defining a timer to track the rollover of the counter. 
                This should ONLY be called once upon boot, preferably 
                before the first rollover of the timetick counter (~36.5 hrs.)

  PARAMETERS    *_h         - Device handle.
                unused      - dummy variable introduced to 
                              stick to the DDI Prototype signature
                
  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS   After this is called Timetick_GetTimetick64 can be used.

========================================================================*/
DALResult Timetick_InitTimetick64
( 
  TimetickClientCtxt *
);

/*========================================================================

  FUNCTION      Timetick_GetTimetick64

  DESCRIPTION   Read the timetick counter. Adds the rollover 
                offset to provide 64 bit dynamic range.


  PARAMETERS    *_h         - Device handle.
                * ticks     - 64-bit tick count
                
  DEPENDENCIES  TIMETICK_INIT_TIMETICK64 must be called first to initialize
                the timer.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_GetTimetick64
( 
  TimetickClientCtxt *,  
  DalTimetickTime64Type *
);

/*========================================================================

  FUNCTION      Timetick_GetMs

  DESCRIPTION   Returns a monotonically increasing millisecond count.

                The value returned is unrelated to CDMA, GPS, HDR, or 
                Time of Day time-stamps.  
                It will drift with respect to the above time references.
                *** This function sacrifices accuracy for speed ***

  PARAMETERS    *_h         - Device handle.
                *ms         - Millisecond tick count
                
  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_GetMs
( 
  TimetickClientCtxt *,  
  DalTimetickTime32Type *
);

/*========================================================================

  FUNCTION      Timetick_UpdateFreq

  DESCRIPTION   Stores the timetick frequency in Hz for use during timetick 
                calculations.

  PARAMETERS    *_h         - Device handle.
                freq        - The new timetick frequency (in Hz)
                
  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  Updates timetick.ms_info.freq information

========================================================================*/
DALResult Timetick_UpdateFreq
( 
  TimetickClientCtxt *, 
  uint32 
);

/*========================================================================

  FUNCTION      Timetick_SetOffset

  DESCRIPTION   Stores the offset between modem/application processor 
                timetick counter and the mpm's timetick counter.

                The modem/application processors' h/w counter, will have 
                an offset w.r.t. mpm's timetick counter, due to power 
                collapse. This difference is eliminated by adding an offset
                to the value read from the processor's timetick counter.

  PARAMETERS    *_h         - Device handle.
                timetick_delta  - Difference between modem/application
                                  processor's and mpm timetick counters
                
  DEPENDENCIES  Regional function.  It may only be called from time*.c

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_SetOffset
( 
  TimetickClientCtxt *, 
  DalTimetickTime32Type 
);

/*========================================================================

  FUNCTION      Timetick_GetOffset

  DESCRIPTION   Retrieves the offset between modem/application processor 
                timetick counter and the mpm's timetick counter.

                The modem/application processors' h/w counter, will have 
                an offset w.r.t. mpm's timetick counter, due to power 
                collapse. This difference is eliminated by adding an offset
                to the value read from the processor's timetick counter.

  PARAMETERS    *_h         - Device handle.
                *delta  -  Difference between modem/application
                           processor's and mpm timetick counters
                
  DEPENDENCIES  Regional function.  It may only be called from time*.c

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_GetOffset
( 
  TimetickClientCtxt *, 
  DalTimetickTime32Type *
);

/*========================================================================

  FUNCTION      Timetick_Enable

  DESCRIPTION   Enables/disables the interrupt for WakeUpTimer.

  PARAMETERS    *_h         - Device handle.
                boolean     - ehnable/disable
                
  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_Enable
(
  TimetickClientCtxt *, 
  boolean
);



/*========================================================================

  FUNCTION      Timetick_GetFreq

  DESCRIPTION   Returns the timetick frequency.

  PARAMETERS    *_h         - Device handle.
                uint32 *    - variable through which the freq is returned.
                
  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_GetFreq
(
  TimetickClientCtxt *, 
  uint32 * 
);

/*========================================================================

  FUNCTION      Timetick_UpdateBlastSigId

  DESCRIPTION   Updates the timetick driver with the blast signal Id

  PARAMETERS    *_h         - Device handle.
                uint32     -  blast signal id.
                
  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_UpdateBlastSigId
(
  TimetickClientCtxt *,
  uint32 
);

/*=============================================================================

  FUNCTION Timetick_CvtToUsec

  DESCRIPTION  Converts the given value to microseconds
 
  PARAMETERS    *_h         - Device handle.
                time        - Time interval to convert to microsecs
                unit        - Units of time interval
                *pTimeRet   - Time in microsecs
 
  DEPENDENCIES  Valid timetick frequency estimate

  RETURN VALUE  Timeticks in microsecnds which corresponds to the given time value

  SIDE EFFECTS  None

=============================================================================*/
DALResult 
Timetick_CvtToUsec
( 
  TimetickClientCtxt *, 
  DalTimetickTime32Type, 
  DalTimetickUnitType, 
  DalTimetickTime64Type * 
);

/*=============================================================================

  FUNCTION Timetick_CvtFromUsec

  DESCRIPTION  Converts the given microsecs value to ticks
 
  PARAMETERS    *_h         - Device handle.
                time        - Time in microsecs
                unit        - Units of time interval to convert to
                *pTimeRet   - Time interval converted from given microsecs
 
  DEPENDENCIES  Valid timetick frequency estimate

  RETURN VALUE  Time in the unit requested

  SIDE EFFECTS  None

=============================================================================*/
DALResult 
Timetick_CvtFromUsec
( 
  TimetickClientCtxt *, 
  DalTimetickTime64Type, 
  DalTimetickUnitType, 
  DalTimetickTime32Type *
);


/*========================================================================

  FUNCTION      Timetick_GetMatchValue

  DESCRIPTION   Returns the match value programmed.

  PARAMETERS    *_h        - Device handle.
                *count     - timetick match value
                
  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_GetMatchValue
( 
  TimetickClientCtxt *,  
  DalTimetickTime32Type *
);

/*========================================================================

  FUNCTION      Timetick_TicksToPrecUs

  DESCRIPTION   Converts the given ticks to precision microseconds.
                This function also supports the full range
                of timeticks.

  PARAMETERS    *_h         - Device handle.
                ticks       - Duration in ticks to be converted into 
                              microseconds
                *us         - # of microseconds which corresponds to the 
                             given time value
                
  DEPENDENCIES  Valid timetick frequency

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_TicksToPrecUs
(
  TimetickClientCtxt *,
  DalTimetickTime32Type,
  uint64 *
);


/*==========================================================================

  FUNCTION      Timetick_SetNextInterrupt64

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult 
Timetick_SetNextInterrupt64
(
  TimetickClientCtxt  *,
  DalTimetickTime64Type,
  DalTimetickTime64Type,
  DalTimetickTime64Type *
);


/*==========================================================================

  FUNCTION      Timetick_GetMatchValue64

  DESCRIPTION   See DDITimetick.h

==========================================================================*/

DALResult 
Timetick_GetMatchValue64
(
  TimetickClientCtxt   *,
  DalTimetickTime64Type *
);


/*==========================================================================

  FUNCTION      Timetick_RegFreqChangeNotification

  DESCRIPTION   See DDITimetick.h

==========================================================================*/
DALResult Timetick_RegFreqChangeNotification
(
  TimetickClientCtxt *,
  DALSYSCallbackFunc,
  DALSYSCallbackFuncCtx
);

/*==========================================================================

  FUNCTION      Timetick_GetInterruptVectorNum

  DESCRIPTION   See DDITimetick.h

==========================================================================*/
DALResult Timetick_GetInterruptVectorNum
(
  TimetickClientCtxt *,
  uint32*
);

/*========================================================================

  FUNCTION      Timetick_MaskInterrupt

  DESCRIPTION   Sets/Clears the interrupt mask bit.

  PARAMETERS    *_h         - Device handle.
                boolean     - set/clear mask
                
  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

========================================================================*/
DALResult Timetick_MaskInterrupt
(
  TimetickClientCtxt *, 
  boolean
);


#endif /*DALTimetick_H*/


