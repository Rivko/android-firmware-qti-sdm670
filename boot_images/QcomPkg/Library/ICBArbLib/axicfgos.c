/**
 * @file axicfgos.c
 * @note Internal source file for Bus Configuration. Implements platform independent functions
 * 
 *                REVISION  HISTORY
 *  
 * This section contains comments describing changes made to this file. Notice
 * that changes are listed in reverse chronological order.
 * 
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBArbLib/axicfgos.c#1 $ 
 * $DateTime: 2017/09/18 09:26:13 $ 
 * $Author: pwbldsvc $ 
 * 
 * when         who     what, where, why
 * ----------   ---     ---------------------------------------------------
 * 05/03/2016   sds     Add support for power domains during configuration
 * 03/12/2014   sds     Change the hardware base address to a uint8_t*.
 * 12/05/2011   dj      Created file
 *  
 *  
 * Copyright (c) 2011, 2014, 2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
 */ 

#include "DALSys.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "DDIHWIO.h"
#include "axicfgos.h"
#include "string.h" 
#include "icb_soc_data.h" 
#include "DDIClock.h"



/* internal data structure */
struct AxiCfgOs_Info 
{
  char * DevStr;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  DalDeviceHandle *hHWIO;
  DalDeviceHandle *hClock;

  DALSYSSyncHandle  hMutex;  /*<<sync handle to mutex*/
  DALSYSSyncObj  mutexSyncObj;/*<<sync object for mutex;*/
};


DALResult 
AxiCfg_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);


/* internal private data */
static bool bInitialized=false;
static struct AxiCfgOs_Info Info;

static DALDEVICEID DalAxiCfg_DeviceId[1] = { DALDEVICEID_AXICFG };

/*------------------------- public data ----------------------------------*/ 

const DALREG_DriverInfo 
DALAxiCfg_DriverInfo = { AxiCfg_DeviceAttach,
                         1,
                         DalAxiCfg_DeviceId
                       };


/*------------------------- public funs ----------------------------------*/ 
DALResult 
AxiCfg_DeviceAttach(const char * pDevSting, DALDEVICEID id ,DalDeviceHandle ** pHandle)
{
  //does nothing
  (void)pDevSting;
  (void)pHandle;
  return DAL_SUCCESS;
}

/**
  @copydoc AxiCfgOs_Init
 */   
bool AxiCfgOs_Init( void )
{
  if( !bInitialized )
  {
    
    Info.DevStr = "/icb/arb";
    
    /* Get default configuration handle from config data */
    if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr( Info.DevStr, Info.hProp ))
    { 
      return bInitialized;
    }
     
    /*create sync object*/
    if( DAL_SUCCESS != DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE,
                                          &Info.hMutex,
                                          &Info.mutexSyncObj) )
    {
      return bInitialized;
    }

    /*attach hwio driver*/
    if( DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_HWIO, &Info.hHWIO) )
    {
      return bInitialized;
    }

    /*attach clock driver*/
    if( DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_CLOCK, &Info.hClock) )
    {
      return bInitialized;
    }

    bInitialized = true;

  }
  return bInitialized;
}

/**
  @copydoc AxiCfgOs_MutexLock
 */   
void AxiCfgOs_MutexLock( void )
{
  DALSYS_SyncEnter( Info.hMutex );
}

/**
  @copydoc AxiCfgOs_MutexUnlock
 */   
void AxiCfgOs_MutexUnlock( void  )
{
  DALSYS_SyncLeave( Info.hMutex );
}

/**
  @copydoc AxiCfgOs_Mmap
 */   
uint8_t *AxiCfgOs_Mmap( const char * pBaseName, uint8_t *pBaseAddr, uint32_t uRegionSize )
    
{
  uint8_t * pBasePtr = NULL;
  DALResult eResult =  DalHWIO_MapRegion(Info.hHWIO, pBaseName , &pBasePtr ); 

  if( DAL_SUCCESS == eResult && NULL != pBasePtr) 
  {
    return pBasePtr;
  }
  return NULL; 
}


/**
  @copydoc AxiCfgOs_GetProperty 
 */   
void * AxiCfgOs_GetProperty( const char * pString )
{
  if( AxiCfgOs_Init() )
  {
    DALSYSPropertyVar PropVar;
    if ( DAL_SUCCESS == DALSYS_GetPropertyValue( Info.hProp, pString, 0, &PropVar) ) 
    {
      return (void*)PropVar.Val.pStruct;
    }
  }
  return NULL;
}

/**
  @copydoc AxiCfgOs_GetClockId
  */
bool AxiCfgOs_GetClockId( const char * pName, ClockIdType * puId )
{
  DALResult res; 

  res = DalClock_GetClockId(Info.hClock, pName, puId);

  return (res == DAL_SUCCESS);
}

/**
  @copydoc AxiCfgOs_EnableClock
  */
void AxiCfgOs_EnableClock( ClockIdType uId )
{
  DalClock_SetClockFrequency(Info.hClock, uId, 1, CLOCK_FREQUENCY_KHZ_AT_LEAST, NULL);
  DalClock_EnableClock(Info.hClock, uId);
}

/**
  @copydoc AxiCfgOs_DisableClock
  */
void AxiCfgOs_DisableClock( ClockIdType uId )
{
  DalClock_DisableClock(Info.hClock, uId);
}

/**
  @copydoc AxiCfgOs_GetPowerDomainId
  */
bool AxiCfgOs_GetPowerDomainId( const char * pName, ClockPowerDomainIdType * puId )
{
  DALResult res; 

  res = DalClock_GetPowerDomainId(Info.hClock, pName, puId);

  return (res == DAL_SUCCESS);
}

/**
  @copydoc AxiCfgOs_EnablePowerDomain
  */
void AxiCfgOs_EnablePowerDomain( ClockPowerDomainIdType uId )
{
  DalClock_EnablePowerDomain(Info.hClock, uId);
}

/**
  @copydoc AxiCfgOs_DisableClock
  */
void AxiCfgOs_DisablePowerDomain( ClockPowerDomainIdType uId )
{
  DalClock_DisablePowerDomain(Info.hClock, uId);
}
