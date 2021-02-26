#ifndef RPMH_OS_H
#define RPMH_OS_H

/*===========================================================================
                             rpmh_os.h

DESCRIPTION:
  Provides common layer for OS specific functionality. One implementation of 
  this header for each supported image. Functionality includes but is not 
  limited to locking, logging, verification (core_verify), fatal errors, and 
  interrupt management. Note that logging is in the OS specific header file

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "comdef.h"
#include "rpmh_image_os.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//callback type for ISR registration
typedef void ( *rpmh_cb_fn )( void *cb_data );

typedef enum
{
  RPMH_ISR_COMPLETION = 0,
  RPMH_ISR_EPCB_TIMEOUT,
  RPMH_ISR_MAX,
} rpmh_isr_type;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- rpmh_os_init -->
 *
 * @brief Call before using any other rpm os API 
 */
void rpmh_os_init (void);

/**
 * <!-- rpmh_clock_toggle -->
 *
 * @brief Enable the given clock, which should be for RSC access  
 *
 * @param clk_name : The clock to enable
 * @param enable   : Whether to enable or disable the clock (TRUE to enable)
 */
void rpmh_clock_toggle(char *clk_name, boolean enable);

/**
 * <!-- rpmh_lock -->
 *
 * @brief Release the global RPMh driver lock  
 *
 * @param isr : Whether or not we are in an ISR context. Some OS's can't lock from in an ISR
 */
void rpmh_lock (boolean isr);

/**
 * <!-- rpmh_unlock -->
 *
 * @brief Release the global RPMh driver lock  
 *
 * @param isr : Whether or not we are in an ISR context. Some OS's can't lock from in an ISR
 */
void rpmh_unlock (boolean isr);

/**
 * <!-- rpmh_malloc -->
 *
 * @brief Allocate buffer of the given size  
 *
 * @param size : size of the requested buffer
 *
 * @return The allocated buffer 
 */
void *rpmh_malloc(unsigned size);

/**
 * <!-- rpmh_free -->
 *
 * @brief Free the buffer pointed to by the given pointer  
 *
 * @param ptr : pointer to the buffer to free
 */
void rpmh_free(void *ptr);

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
void* rpmh_realloc(void *ptr, unsigned size);

/**
 * <!-- rpmh_core_verify -->
 *
 * @brief Verify the given value is TRUE. Force a crash if FALSE  
 *
 * @param val : value to verify
 */
void rpmh_core_verify (boolean val);

/**
 * <!-- rpmh_core_verify_ptr -->
 *
 * @brief Verify the given pointer is valid. Force a crash if NULL  
 *
 * @param val : pointer to verify
 */
void rpmh_core_verify_ptr (void *val);

/**
 * <!-- rpmh_err_fatal -->
 *
 * @brief Unrecoverable error, force a crash  
 */
void rpmh_err_fatal (void *val);

/**
 * <!-- rpmh_get_timestamp -->
 *
 * @brief Retreive 64 bit current timestamp   
 *
 * @return 64 bit current timestamp   
 */
uint64 rpmh_get_timestamp (void);

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
void rpmh_register_isr (uint32 irq, rpmh_cb_fn isr, void* cb_data, rpmh_isr_type isr_type);

/**
 * <!-- rpmh_deregister_isr -->
 *
 * @brief Deregister the corresponding ISR   
 *
 * @param irq : the interrupt # to deregister
 */
void rpmh_deregister_isr (uint32 irq);

/**
 * <!-- rpmh_clz -->
 *
 * @brief count leading zeros implementation  
 *
 * @param num : Num to calculat clz on
 *
 * @return the number of leading zeros
 */
uint32 rpmh_clz (uint32 num);

/**
 * <!-- get_core_verify_failures -->
 * @brief Used to track the # of core_verify failures for offtarget testing.  
 *
 * @return The # of core_verify failures so far in this run 
 */
#ifdef RPMH_OFFTARGET_TEST
uint32 get_core_verify_failures (void);
#endif //RPMH_OFFTARGET_TEST

#endif /* RPMH_OS_H */

