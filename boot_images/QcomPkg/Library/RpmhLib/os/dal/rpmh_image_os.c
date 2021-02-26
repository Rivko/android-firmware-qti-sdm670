/*===========================================================================
                              rpmh_image_os.c

DESCRIPTION: 
  Non-HLOS OS functionality. One version of this file will exist for
  each supported image. Functionality includes locking, logging, verification 
  (core_verify), fatal errors, and interrupt management.


Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "CoreVerify.h"
#include "rpmh_os.h"
#include "rpmh_image_os.h"
#include "CoreHeap.h"

//#include <stdlib.h>

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

/* OS related objects stored in a single structure for debug ease */
rpmh_os_t rpmh_os;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================

/**
 * <!-- rpmh_os_init -->
 *
 * @brief Call before using any other rpm os API 
 */
void rpmh_os_init()
{
    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_RPMH_SYNC_TYPE, &(rpmh_os.lock), 0))
        ERR_FATAL("Couldn't create RPM lock.", 0, 0, 0);


    CORE_DAL_VERIFY(ULogFront_RealTimeInit(&(rpmh_os.log),
                                           "RPMhMasterLog",
                                           8192,
                                           ULOG_MEMORY_LOCAL,
                                           ULOG_LOCK_OS));
}

/**
 * <!-- rpmh_clock_toggle -->
 *
 * @brief Enable the given clock, which should be for RSC access  
 *
 * @param clk_name : The clock to enable
 * @param enable   : Whether to enable or disable the clock (TRUE to enable)
 */
void rpmh_clock_toggle (char *clk_name, boolean enable)
{
    ClockIdType clock_id;
    if(!rpmh_os.clock_handle)
    {
        // Attach to Clock Driver
        rpmh_core_verify( DAL_ClockDeviceAttach (DALDEVICEID_CLOCK, &(rpmh_os.clock_handle)) == DAL_SUCCESS);
    }

    rpmh_core_verify ( DalClock_GetClockId( rpmh_os.clock_handle, clk_name, &clock_id) == DAL_SUCCESS);
    if(enable)
    {
      rpmh_core_verify ( DalClock_EnableClock(rpmh_os.clock_handle, clock_id) == DAL_SUCCESS);
    }
    else
    {
      rpmh_core_verify ( DalClock_DisableClock(rpmh_os.clock_handle, clock_id) == DAL_SUCCESS);
    }
}

/**
 * <!-- rpmh_lock -->
 *
 * @brief Release the global RPMh driver lock  
 *
 * @param isr : Whether or not we are in an ISR context. Some OS's can't lock from in an ISR
 */
void rpmh_lock(boolean isr)
{
    DALSYS_SyncEnter(rpmh_os.lock);
}	

/**
 * <!-- rpmh_unlock -->
 *
 * @brief Release the global RPMh driver lock  
 *
 * @param isr : Whether or not we are in an ISR context. Some OS's can't lock from in an ISR
 */
void rpmh_unlock(boolean isr)
{
    DALSYS_SyncLeave(rpmh_os.lock);
}

/**
 * <!-- rpmh_get_core_verify_failures -->
 * @brief Used to track the # of core_verify failures for offtarget testing.  
 *
 * @return The # of core_verify failures so far in this run 
 */
#ifdef RPMH_OFFTARGET_TEST
uint32 get_core_verify_failures(void)
{
    return num_core_verify_failures;
}
#endif //RPMH_OFFTARGET_TEST

/**
 * <!-- rpmh_core_verify -->
 *
 * @brief Verify the given value is TRUE. Force a crash if FALSE  
 *
 * @param val : value to verify
 */
void rpmh_core_verify(boolean val)
{
    CORE_VERIFY(val);
}

/**
 * <!-- rpmh_core_verify_ptr -->
 *
 * @brief Verify the given pointer is valid. Force a crash if NULL  
 *
 * @param val : pointer to verify
 */
void rpmh_core_verify_ptr(void *val)
{
    CORE_VERIFY_PTR(val);
}

/**
 * <!-- rpmh_err_fatal -->
 *
 * @brief Unrecoverable error, force a crash  
 */
void rpmh_err_fatal(void *val)
{
    ERR_FATAL("RPMH_ERR_FATAL", 0, 0, 0);
}	

/**
 * <!-- rpmh_register_isr -->
 *
 * @brief Register a callback for the given IRQ    
 *
 * @param irq : the interrupt # to register for
 * @param isr : the callback function to trigger when the interrupt comes in
 * @param cb_data : data/context to pass the callback function
 * @param isr_type : indicates which type of callback this registration is for (completion or epcb timeout for now)
 */
void rpmh_register_isr (uint32 irq, rpmh_cb_fn isr, void* cb_data, rpmh_isr_type type)
{
  if (rpmh_os.isr_handle == NULL)
  {
    /* Create Incoming Interrupt Controller Handle */
//    rpmh_core_verify (DAL_DeviceAttach (DALDEVICEID_INTERRUPTCONTROLLER, &(rpmh_os.isr_handle))
  //                    == DAL_SUCCESS);
    

	rpmh_core_verify (DAL_InterruptControllerDeviceAttach (DALDEVICEID_INTERRUPTCONTROLLER, &(rpmh_os.isr_handle))
                      == DAL_SUCCESS);
  }

  /* Register the interrupt */
  if(rpmh_os.isr_handle)
  {
    //TODO: do we need trigger type options?	  
    rpmh_core_verify (DalInterruptController_RegisterISR (rpmh_os.isr_handle, irq, (DALIRQ)isr,
                                                          (DALIRQCtx)cb_data,
                                                          DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER)
                                                          == DAL_SUCCESS);
  }
}

/**
 * <!-- rpmh_deregister_isr -->
 *
 * @brief Deregister the corresponding ISR   
 *
 * @param irq : the interrupt # to deregister
 */
void rpmh_deregister_isr (uint32 irq)
{
  
  if (rpmh_os.isr_handle)
  {
    rpmh_core_verify (DalInterruptController_Unregister (rpmh_os.isr_handle, irq) == DAL_SUCCESS);
  }
}

/**
 * <!-- rpmh_get_timestamp -->
 *
 * @brief Retreive 64 bit current timestamp   
 *
 * @return 64 bit current timestamp   
 */
uint64 rpmh_get_timestamp (void)
{
  uint64 timer_val = 0;
#if 0
  if (rpmh_os.timer_handle == NULL)
  {
    rpmh_core_verify (DAL_TimerDeviceAttach (DALDEVICEID_TIMER, &(rpmh_os.timer_handle)) == DAL_SUCCESS);
  }  
  
  if (rpmh_os.timer_handle)
  {  
    rpmh_core_verify (DalTimer_GetTimerCount (rpmh_os.timer_handle, &timer_val, sizeof(uint64)) == 
                      DAL_SUCCESS);
  }
#endif
  return timer_val;
}

/**
 * <!-- rpmh_clz -->
 *
 * @brief count leading zeros implementation  
 *
 * @param num : Num to calculat clz on
 *
 * @return the number of leading zeros
 */
uint32 rpmh_clz (uint32 num)
{
  int i;
  for (i = 0; i < 32; i++)
  {
      if ((num & 0x80000000) != 0)
      {
          return i;
      }
      num <<= 1;
  }
  return 32;

  //return llvm.ctlz.i32(num);
}

/**
 * <!-- rpmh_malloc -->
 *
 * @brief Allocate buffer of the given size  
 *
 * @param size : size of the requested buffer
 */
void *rpmh_malloc(unsigned size)
{

  return Core_Malloc (size);
}

/**
* <!-- rpmh_free -->
*
* @brief Free the buffer pointed to by the given pointer  
*
* @param ptr : pointer to the buffer to free
*/
void rpmh_free(void *ptr)
{
  Core_Free (ptr);
}

/**
 * <!-- rpmh_realloc -->
 *
 * @brief Reallocate the given buffer to the given size  
 *
 * @param ptr  : pointer to the buffer to reallocate
 * @param size : size of the new buffer
 *
 * @return The new buffer location 
 */
void* rpmh_realloc(void *ptr, unsigned size)
{
  return Core_Realloc (ptr, size);
}

