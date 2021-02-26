#ifndef _UATS_H_
#define _UATS_H_
/*==============================================================================
  FILE:         uATS.h

  OVERVIEW:     This file contains internal APIs exposed by the micro-Active Time 
                Solver.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/uATS/uATS.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "uSleep_solver.h"
#include "utimer.h"

/*==============================================================================
                                GLOBAL TYPE DECLARATIONS
  =============================================================================*/ 
/* List of Signals that cause the Active Time solver 
 * to re-evaluate its selection of low-power modes */
typedef enum
{
  USLEEP_SIGNAL_LATENCY             = 0x0001, /* Update in Latency Constraint         */
  USLEEP_SIGNAL_REGISTRY            = 0x0002, /* Update List of Enabled Synth Modes   */
  USLEEP_SIGNAL_HARD_DURATION       = 0x0004, /* Update idle hard duration            */
  USLEEP_SIGNAL_THRESH_TIMER        = 0x0008, /* Threshold timer expired              */
 
  USLEEP_SIGNAL_LAST                = 0x0010, /* MUST BE LAST STANDARD SIGNAL */

  /*SPECIAL INTERNAL SIGNALS
   *  Mask for all normal signals */ 
  USLEEP_SIGNAL_MASK_ALL       = (USLEEP_SIGNAL_LAST - 1),

  /*  Control signal to indicate ATS init is complete. */
  USLEEP_SIGNAL_INIT_COMPLETE  = (USLEEP_SIGNAL_LAST << 1)
} uATS_signalType;

/*==============================================================================
                              GLOBAL FUNCTION DECLARATIONS
  =============================================================================*/
/** 
 * uATS_GetSolverInput
 * 
 * @brief Returns the input parameters used by Sleep Active for 
 *        input to Active time solver
 *  
 * @return A pointer to the solver input struct
 */
uSleep_solver_input *uATS_getSolverInput(void);

/** 
 * uATS_GetSolverOutput 
 *  
 * @brief Returns the low-power mode selected by Active time 
 *        solver
 *  
 * @return A pointer to the last chosen solver output struct
 */
uSleep_solver_output *uATS_getSolverOutput(void);

/** 
 * uATS_GetSolverOutputModeLatency 
 *  
 * @brief Returns the enter+exit latency of the low-power mode selected
 *        by Active time solver
 *  
 * @return Enter+exit latency of the selected low-power mode
 */
unsigned int uATS_GetSolverOutputModeLatency(void);

/** 
 * uATS_performSolving 
 *  
 * @brief Main ATS function to invoke processing of any system restrictions to 
 *        select and configure the best low power mode to enter.
 *  
 * @note This function must be called at least once before attempting to enter low power mode. 
 *       It should also be called anytime there is a change in any system restriction that
 *       would affect the optimal low power mode to enter. 
 */
void uATS_performSolving(void);

/** 
 * uATS_SetSignal 
 *  
 * @brief Sets a signal type for uATS to process 
 *  
 * @param signals: One or more signals that uATS will process
 */
void uATS_SetSignal(uATS_signalType signals);

/** 
 * uATS_init 
 *  
 * @brief Initializes the Active Time Solver  
 */
void uATS_init(void);

/**
 * uATS_timerEventHandlerCB 
 * 
 * @brief Generic timer event callback function that sets a signale with the given 
 *        data parameter for uATS processing. 
 *
 * @param data:         Signal for uATS 
 * @param clientTimer:  Timer that has caused this CB to be invoked 
 */
void uATS_timerEventHandlerCB(utimer_cb_data_type data);

/**
 * uATS_timerThreshHandlerCB 
 * 
 * @brief Generic timer callback function that sets a signale with the given 
 *        data parameter for uATS processing. 
 *
 * @param data: Signal for uATS 
 */
void uATS_timerThreshHandlerCB(utimer_cb_data_type data);

/**
 * uATS_stopThreasholdTimer 
 * 
 * @brief Explicitly stops the threshold timer to prevent ATS from solving again.
 *        Typiclaly used when an exit is triggered to prevent uATS from needless running.
 */
void uATS_stopThreasholdTimer(void);

#endif /* _UATS_H_ */

