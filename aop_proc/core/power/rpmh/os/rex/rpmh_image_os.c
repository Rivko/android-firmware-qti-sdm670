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
#include "rex.h"
#include "rpmh_drv_config.h"
#include "time_service.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------
const drv_config_t *drv; 
//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------
int lock_ref_count = 0;
boolean in_isr = FALSE;
//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

/* OS related objects stored in a single structure for debug ease */
rpmh_os_t rpmh_os;

rpmh_cb_fn rex_completion_cb;
rpmh_cb_fn rex_timeout_cb;

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
    drv = &DRV_CONFIG_DATA->drvs[0]; 
    rex_init_crit_sect(&(rpmh_os.lock));
}

void rpmh_enter_isr()
{
  rpmh_os.in_isr = TRUE;
}

void rpmh_exit_isr()
{
  rpmh_os.in_isr = FALSE;
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
// stub for now as AOP doesn't have any local RSC clocks
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
  //on AOP we don't lock in an ISR context	
  if(!isr)	
  {  
    rex_enter_crit_sect(&(rpmh_os.lock));
    rex_disable_interrupt((rex_vect_type)drv->in_irq_num);
  }  
  lock_ref_count++;
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
  lock_ref_count--;
  rpmh_core_verify(lock_ref_count >= 0);

  //on AOP we don't lock in an ISR context	
  if(!isr)	
  {	  
    if(lock_ref_count == 0)
    {
      rex_enable_interrupt((rex_vect_type)drv->in_irq_num);
    }
    rex_leave_crit_sect(&(rpmh_os.lock));
  }    
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

void rpmh_internal_cb(int irq)
{
  for (int i = 0; i < DRV_CONFIG_DATA->num_drvs; i++)
  {
    const drv_config_t *drv = &DRV_CONFIG_DATA->drvs[i]; 
    if(drv->in_irq_num == irq)
    {
      rex_completion_cb((void *)drv->drv_id);
      return;
    }
    else if(drv->epcb_timeout_irq_num == irq)
    {
      rex_timeout_cb((void *)drv->drv_id);
      return;
    }
  }
}

/**
 * <!-- rpmh_register_isr -->
 *
 * @brief Register a callback for the given IRQ    
 *
 * @param irq : the interrupt # to register for
 * @param isr : the callback function to trigger when the interrupt comes in
 * @param cb_data : data/context to pass the callback function
 * @param isr_type : indicates which type of callback this registration is for (completion or ipcb timeout for now)
 */
void rpmh_register_isr (uint32 irq, rpmh_cb_fn isr, void* cb_data, rpmh_isr_type isr_type)
{
  switch(isr_type)
  {
    case(RPMH_ISR_COMPLETION):
      rex_completion_cb = isr;
      break;      
    case(RPMH_ISR_EPCB_TIMEOUT):	    
      rex_timeout_cb = isr;
      break;      
    default:
      break;
  }	  
  rex_set_interrupt_vector((rex_vect_type)irq, (rex_isr_func_type)rpmh_internal_cb);
  rex_clear_interrupt((rex_vect_type)irq);
  rex_enable_interrupt((rex_vect_type)irq);
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
  rex_disable_interrupt((rex_vect_type)irq);
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
  return time_service_now();
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
  return __clz(num);
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

  return malloc (size);
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
  free (ptr);
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
  return realloc (ptr, size);
}

