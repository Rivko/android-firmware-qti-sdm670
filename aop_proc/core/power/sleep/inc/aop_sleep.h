#ifndef AOP_SLEEP_H
#define AOP_SLEEP_H
/*===========================================================================
                             aop_sleep.h

DESCRIPTION:
  Verbose description of what is defined by this header file. Typically
  describes the service provided by the structs, macros, etc. defined in
  this file.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <string.h>

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
enum aop_sleep_signal_enum
{
  AOP_SLEEP_SIG_PDC_LOCK_REQ = 1 << 0,
  AOP_SLEEP_SIG_RPMH_IDLE = 1 << 1,
  AOP_SLEEP_SIG_PDC_UNLOCK_REQ = 1 << 2,
  BCM_WORKAROUND_SEQ_GEN1 = 1 << 3,
  AOP_RAMDUMP = 1 << 5,
  DBG_SIG_PDC_LOCK_REQ = 1 << 6,
  DBG_SIG_PDC_UNLOCK_REQ = 1 << 7,
  CX_COLLAPSE_SIG_LOCK_REQ = 1 << 8,
  CX_COLLAPSE_SIG_UNLOCK_REQ = 1 << 9,
};

typedef enum aop_sleep_state_enum_type
{
  AOP_SLEEP_IDLE,
  AOP_SLEEP_LOCK,
  AOP_SLEEP_HW_IDLE,
  AOP_SLEEP_WFI_ENTER,
  AOP_SLEEP_WFI_EXIT,
  AOP_SLEEP_UNLOCK
} aop_sleep_state_enum_type;

// Pointer to a function returning Void taking a uint64 - used for timers.
typedef void (*PFV_W64)(uint64_t);

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- aop_register_timer_cb -->
 *
 * @brief Register a function to be called from the idle thread after a period of time
 *
 * @param[in] period   - The number of ticks in the future to set the timer
 * @param[in] callback - The function to call.
 * 
**/
int aop_register_timer_cb(uint32 period, PFV_W64 callback);

/**
 * <!-- aop_cancel_timer_cb -->
 *
 * @brief Cancel a previously registered callback function
 *
 * @param[in] callback - The function to have been called.
 * 
**/
int aop_cancel_timer_cb(PFV_W64 callback);

/**
 * <!-- aop_busy -->
 *
 * @brief This function is used to prevent AOSS sleep if AOP is likely busy.
 *
 * @return none
 */
void aop_busy(uint32 thread_mask, bool busy);

#define VOLTAGE_FSM_MASK   0x01
#define FREQUENCY_FSM_MASK 0x02

#endif // AOP_SLEEP_H
