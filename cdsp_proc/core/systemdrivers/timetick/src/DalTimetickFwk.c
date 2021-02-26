/*==============================================================================

FILE:      DalTimetickFwk.c

DESCRIPTION: This file implements a Timetick DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2010-2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DalTimetick.h"
#include "string.h" //for memset warning
#include <stringl/stringl.h>

DALResult 
Timetick_DalTimetick_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
Timetick_DalTimetick_AddRef(DalTimetickHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
Timetick_DalTimetick_Detach(uint32 ClientHandle, DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	 if(0 == dwref)	{
		//Release the client ctxt;
        DALSYS_Free(h->pClientCtxt);
	}
	return dwref;
}

static DALResult 
Timetick_DalTimetick_Init(uint32 ClientHandle, DalDeviceHandle* h)
{  
   return Timetick_DeviceInit(h->pClientCtxt);
}

static DALResult 
Timetick_DalTimetick_DeInit(uint32 ClientHandle, DalDeviceHandle* h)
{
   return Timetick_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
Timetick_DalTimetick_PowerEvent(uint32 ClientHandle, DalDeviceHandle* h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return Timetick_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
Timetick_DalTimetick_Open(uint32 ClientHandle, DalDeviceHandle* h, uint32 mode)
{
    return Timetick_Open(h->pClientCtxt,mode);
}

static DALResult 
Timetick_DalTimetick_Close(uint32 ClientHandle, DalDeviceHandle* h)
{
    return Timetick_Close(h->pClientCtxt);
}

static DALResult 
Timetick_DalTimetick_Info(uint32 ClientHandle, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
  DALResult nResult;
  DalDeviceInfo sInfo;

  if( DalSys_CopyFromUser(ClientHandle, &sInfo, info, sizeof(sInfo)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  sInfo.Version = DALTIMETICK_INTERFACE_VERSION;
  nResult = Timetick_Info(h->pClientCtxt,&sInfo,sizeof(sInfo));

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, info, &sInfo, sizeof(sInfo)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}

static DALResult 
Timetick_DalTimetick_SysRequest(uint32 ClientHandle, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTimetick Interface. 
------------------------------------------------------------------------------*/
static DALResult 
Timetick_DalTimetick_GetMinRearmSclk(uint32 ClientHandle, DalDeviceHandle* h,  DalTimetickTime32Type   *pMinRearm)
{
  DALResult nResult;
  DalTimetickTime32Type nMinRearm;

  nResult = Timetick_GetMinRearmSclk(((DalTimetickHandle *)h)->pClientCtxt, &nMinRearm);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, pMinRearm, &nMinRearm, sizeof(nMinRearm)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_CvtToTimetick32(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type  time, DalTimetickUnitType  unit, DalTimetickTime32Type *  time_ret) 
{
  DALResult nResult;
  DalTimetickTime32Type nTime;

  nResult = Timetick_CvtToTimetick32(((DalTimetickHandle *)h)->pClientCtxt, time, unit, &nTime);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, time_ret, &nTime, sizeof(nTime)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_CvtFromTimetick32(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type  time, DalTimetickUnitType  unit, DalTimetickTime32Type *  time_ret) 
{
  DALResult nResult;
  DalTimetickTime32Type nTime;

  nResult = Timetick_CvtFromTimetick32(((DalTimetickHandle *)h)->pClientCtxt, time, unit, &nTime);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, time_ret, &nTime, sizeof(nTime)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_GetRaw(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type *  tick) 
{
  DALResult nResult;
  DalTimetickTime32Type nTick;

  nResult = Timetick_GetRaw(((DalTimetickHandle *)h)->pClientCtxt, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, tick, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult
Timetick_DalTimetick_UpdateFreq(uint32 ClientHandle, DalDeviceHandle* h,   uint32 nFreq)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return Timetick_UpdateFreq(((DalTimetickHandle *)h)->pClientCtxt, nFreq);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
Timetick_DalTimetick_InitTimetick64(uint32 ClientHandle, DalDeviceHandle* h) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return Timetick_InitTimetick64(((DalTimetickHandle *)h)->pClientCtxt);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
Timetick_DalTimetick_GetTimetick64(uint32 ClientHandle, DalDeviceHandle* h,  DalTimetickTime64Type * ticks) 
{
  DALResult nResult;
  DalTimetickTime64Type nTick;

  nResult = Timetick_GetTimetick64(((DalTimetickHandle *)h)->pClientCtxt, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, ticks, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_Enable(uint32 ClientHandle, DalDeviceHandle* h, boolean  enable) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return Timetick_Enable(((DalTimetickHandle *)h)->pClientCtxt, enable);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
Timetick_DalTimetick_SetOffset(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type  timetick_delta) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return Timetick_SetOffset(((DalTimetickHandle *)h)->pClientCtxt, timetick_delta);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
Timetick_DalTimetick_GetOffset(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type  *timetick_delta) 
{
  DALResult nResult;
  DalTimetickTime32Type nTick;

  nResult = Timetick_GetOffset(((DalTimetickHandle *)h)->pClientCtxt, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, timetick_delta, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_Get(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type *  count) 
{
  DALResult nResult;
  DalTimetickTime32Type nTick;

  nResult = Timetick_Get(((DalTimetickHandle *)h)->pClientCtxt, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, count, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_GetElapsed(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type  start, DalTimetickUnitType  unit, DalTimetickTime32Type *  diff) 
{
  DALResult nResult;
  DalTimetickTime32Type nTick;

  nResult = Timetick_GetElapsed(((DalTimetickHandle *)h)->pClientCtxt, start, unit, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, diff, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_Diff(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type  start, DalTimetickTime32Type  end, DalTimetickUnitType  unit, DalTimetickTime32Type *  diff) 
{
  DALResult nResult;
  DalTimetickTime32Type nTick;

  nResult = Timetick_Diff(((DalTimetickHandle *)h)->pClientCtxt, start, end, unit, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, diff, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_GetMs(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type *  ms) 
{
  DALResult nResult;
  DalTimetickTime32Type nTick;

  nResult = Timetick_GetMs(((DalTimetickHandle *)h)->pClientCtxt, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, ms, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_GetFreq(uint32 ClientHandle, DalDeviceHandle* h, uint32  *freq) 
{
  DALResult nResult;
  DalTimetickTime32Type nTick;

  nResult = Timetick_GetFreq(((DalTimetickHandle *)h)->pClientCtxt, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, freq, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_CvtToTimetick64(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime64Type  time,  DalTimetickUnitType unit, DalTimetickTime64Type *time_ret)
{
  DALResult nResult;
  DalTimetickTime64Type nTick;

  nResult = Timetick_CvtToTimetick64(((DalTimetickHandle *)h)->pClientCtxt, time, unit, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, time_ret, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_CvtFromTimetick64(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime64Type  time,  DalTimetickUnitType unit, DalTimetickTime64Type *time_ret)
{
  DALResult nResult;
  DalTimetickTime64Type nTick;

  nResult = Timetick_CvtFromTimetick64(((DalTimetickHandle *)h)->pClientCtxt, time, unit, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, time_ret, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_UpdateBlastSigId(uint32 ClientHandle, DalDeviceHandle* h, uint32  sig_id)
{
	return DAL_ERROR;
}
static DALResult 
Timetick_DalTimetick_CvtToUsec(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type time, DalTimetickUnitType unit, DalTimetickTime64Type * time_ret)
{
  DALResult nResult;
  DalTimetickTime64Type nTick;

  nResult = Timetick_CvtToUsec(((DalTimetickHandle *)h)->pClientCtxt, time, unit, &nTick );

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, time_ret, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}

static DALResult
Timetick_DalTimetick_CvtFromUsec(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime64Type time, DalTimetickUnitType unit, DalTimetickTime32Type * time_ret)
{
  DALResult nResult;
  DalTimetickTime32Type nTick;

  nResult = Timetick_CvtFromUsec(((DalTimetickHandle *)h)->pClientCtxt, time, unit, &nTick );

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, time_ret, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_GetMatchValue(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type *  match_value) 
{
  DALResult nResult;
  DalTimetickTime32Type nTick;

  nResult = Timetick_GetMatchValue(((DalTimetickHandle *)h)->pClientCtxt, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, match_value, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_TicksToPrecUs(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime32Type  nTicks, uint64 *pus) 
{
  DALResult nResult;
  uint64 nTick;

  nResult = Timetick_TicksToPrecUs(((DalTimetickHandle *)h)->pClientCtxt, nTicks, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, pus, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_SetNextInterrupt64(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime64Type  match_count, DalTimetickTime64Type  ticks_now,  DalTimetickTime64Type * match_val) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
      return Timetick_SetNextInterrupt64(((DalTimetickHandle *)h)->pClientCtxt, match_count, ticks_now, match_val);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
Timetick_DalTimetick_GetMatchValue64(uint32 ClientHandle, DalDeviceHandle* h, DalTimetickTime64Type * match_val) 
{
  DALResult nResult;
  DalTimetickTime64Type nTick;

  nResult = Timetick_GetMatchValue64(((DalTimetickHandle *)h)->pClientCtxt, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, match_val, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_RegFreqChangeNotification(uint32 ClientHandle, DalDeviceHandle* h, DALSYSCallbackFunc cbFunc, DALSYSCallbackFuncCtx cbFuncCtx)
{
	return DAL_ERROR;
}
static DALResult 
Timetick_DalTimetick_GetInterruptVectorNum(uint32 ClientHandle, DalDeviceHandle* h, uint32 *num)
{
  DALResult nResult;
  uint32 nTick;

  nResult = Timetick_GetInterruptVectorNum(((DalTimetickHandle *)h)->pClientCtxt, &nTick);

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientHandle, num, &nTick, sizeof(nTick)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}
static DALResult 
Timetick_DalTimetick_MaskInterrupt(uint32 ClientHandle, DalDeviceHandle* h, boolean bMask)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
      return Timetick_MaskInterrupt(((DalTimetickHandle *)h)->pClientCtxt, bMask);
  }
  else
  {
    return DAL_ERROR;
  }
}
static void 
Timetick_InitInterface(TimetickClientCtxt* pclientCtxt)
{
    static const DalTimetick vtbl = {
       {
          Timetick_DalTimetick_Attach,
          Timetick_DalTimetick_Detach,
          Timetick_DalTimetick_Init,
          Timetick_DalTimetick_DeInit,
          Timetick_DalTimetick_Open,
          Timetick_DalTimetick_Close,
          Timetick_DalTimetick_Info,
          Timetick_DalTimetick_PowerEvent,
          Timetick_DalTimetick_SysRequest
        } ,
        Timetick_DalTimetick_GetMinRearmSclk,
        Timetick_DalTimetick_CvtToTimetick32,
        Timetick_DalTimetick_CvtFromTimetick32,
        Timetick_DalTimetick_GetRaw,
        Timetick_DalTimetick_Get,
        Timetick_DalTimetick_GetElapsed,
        Timetick_DalTimetick_Diff,
        Timetick_DalTimetick_InitTimetick64,
        Timetick_DalTimetick_GetTimetick64,
        Timetick_DalTimetick_GetMs,
        Timetick_DalTimetick_UpdateFreq,
        Timetick_DalTimetick_SetOffset,
        Timetick_DalTimetick_GetOffset,
        Timetick_DalTimetick_Enable,
        Timetick_DalTimetick_GetFreq,
        Timetick_DalTimetick_CvtToTimetick64,
        Timetick_DalTimetick_CvtFromTimetick64,
        Timetick_DalTimetick_UpdateBlastSigId,
        Timetick_DalTimetick_CvtToUsec,
        Timetick_DalTimetick_CvtFromUsec,
        Timetick_DalTimetick_GetMatchValue,
        Timetick_DalTimetick_TicksToPrecUs,
        Timetick_DalTimetick_SetNextInterrupt64,
        Timetick_DalTimetick_GetMatchValue64,
        Timetick_DalTimetick_RegFreqChangeNotification,
        Timetick_DalTimetick_GetInterruptVectorNum,
        Timetick_DalTimetick_MaskInterrupt
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalTimetickHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalTimetickHandle.pVtbl  = &vtbl;
	pclientCtxt->DalTimetickHandle.dwVtblen = sizeof(vtbl)/sizeof(void (*)(void));
    pclientCtxt->DalTimetickHandle.pClientCtxt = pclientCtxt;
}

DALResult 
Timetick_DalTimetick_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
	static TimetickDrvCtxt drvCtxt = {{Timetick_DriverInit,
									   Timetick_DriverDeInit
									  },DALTIMETICK_NUM_DEVICES,
									  sizeof(TimetickDevCtxt)};
    TimetickClientCtxt *pclientCtxt = NULL;
    if (phDalDevice == NULL)
    {
      return DAL_ERROR;
    }
    *phDalDevice = NULL;

    nErr = DALSYS_Malloc(sizeof(TimetickClientCtxt),(void **)&pclientCtxt);
    if(nErr != DAL_SUCCESS)
    {
      return nErr;
    }
    DALSYS_memset(pclientCtxt,0,sizeof(TimetickClientCtxt));


           nErr = DALFW_AttachToStringDevice(pszArg, (DALDrvCtxt *)&drvCtxt, (DALClientCtxt *)pclientCtxt);
           if(nErr == DAL_SUCCESS)
           {
               strlcpy(pclientCtxt->pTimetickDevCtxt->strDeviceName, pszArg,DALTIMETICK_MAX_STRING_ID_LENGTH );
           }
      Timetick_InitInterface(pclientCtxt);
	  Timetick_DalTimetick_AddRef(&(pclientCtxt->DalTimetickHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalTimetickHandle);
    return nErr;
}

