/*======================================================================
@file  DALTimer.c

DAL timer driver

        Copyright © 2013 Qualcomm Technologies Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary
 ======================================================================*/
#include <DALStdDef.h>
#include <DALSysCmn.h>
#include <DALSysTypes.h>
#include <DALTimer.h>
#include <qurt.h>
#include <qurt_timer.h>
#include <qurt_sclk.h>

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult Timer_DriverInit(TimerDrvCtxt *pCtxt)
{
        return DAL_SUCCESS;
}
DALResult Timer_DriverDeInit(TimerDrvCtxt *pCtxt)
{
        return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are declared in IDevice Interface. 
------------------------------------------------------------------------------*/
DALResult Timer_DeviceInit(TimerClientCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult Timer_DeviceDeInit(TimerClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult Timer_PowerEvent(TimerClientCtxt *pCtxt, DalPowerCmd PowerCmd, DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult Timer_Open(TimerClientCtxt *pCtxt, uint32 dwaccessMode )
{
    return DAL_SUCCESS;
}

DALResult Timer_Close(TimerClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult Timer_Info(TimerClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 deviceInfoLen)
{
    return DAL_SUCCESS;
}

DALResult Timer_Register( TimerClientCtxt        * pCtxt,   
                          const DALSYSEventHandle  hEvent,   
                          uint32                   time) 
{
   return DALSYS_TimerStart( hEvent, time );
}

DALResult Timer_UnRegister( TimerClientCtxt         * pCtxt,   
                            const DALSYSEventHandle   hEvent) 
{
   return DALSYS_TimerStop( hEvent );
}

DALResult Timer_GetTimerCount( TimerClientCtxt         * pCtxt,
                               void                    * timer_count,
                               uint32                  size)
{
    if(timer_count && size)
    {
        unsigned long long count = qurt_sysclock_get_hw_ticks();
        if(size == sizeof(unsigned long long))
        {
            *((unsigned long long*)timer_count) = count;
            return DAL_SUCCESS;
        }
        else if(size == sizeof(uint32))
        {
            // cast from 64-bit to 32-bit 
            // truncation may occur
            *((unsigned int *)timer_count) = count;
            return DAL_SUCCESS;
        }
    }
    return DAL_ERROR;
}

