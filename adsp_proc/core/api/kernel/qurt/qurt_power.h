#ifndef QURT_POWER_H
#define QURT_POWER_H
/**
  @file qurt_power.h 
  @brief  Prototypes of power API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file 
12/12/12   cm      (Tech Pubs) Edited/added Doxygen comments and markup.
=============================================================================*/



static inline int qurt_power_shutdown_prepare(void){ return 0;}

/**@ingroup func_qurt_power_shutdown_fail_exit  
  Returns from Power Collapse mode when power collapse cannot proceed.

  This function unmasks the global interrupt. This operation is used only when the thread is 
  recovering from a failed power collapse operation (Section @xref{sec:powerShutdownEnter}).
 
  @return
  QURT_EOK -- Operation was successfully performed.

  @dependencies
  None.
 */
#define  qurt_power_shutdown_fail_exit qurt_power_exit

/**@ingroup func_qurt_power_shutdown_enter
  @xreflabel{sec:powerShutdownEnter}   
  Triggers a power collapse. \n
  This function perform following power collapse sequence
  Check pending intterrupt, if interrupt pending(31), transfer interrupt to l2vic.
  Clearing pending interrupt. It also umasks global interrupt. 
  When all hardware threads have entered Wait mode, the Hexagon processor can enter
  Power-collapse mode.
  Upon WARM boot, the API returns to caller. The API returns immediately, due to error condition

  The type indicates whether the L2 cache is retained during the power collapse.

  @param[in]  type    The L2 cache retention type. Values: \n
                      - QURT_POWER_SHUTDOWN_TYPE_L2NORET \n
                      - QURT_POWER_SHUTDOWN_TYPE_L2RET  @tablebulletend 
  
  @return
  QURT_ESTM -- Processor is not in Single-threaded mode.
  QURT_EOK -- Operation was successfully performed. This value indicates that the processor has
              returned from Power-collapse mode. \n

  @dependencies
  None.
 */
int qurt_power_shutdown_enter (int type);

/**@ingroup func_qurt_power_shutdown_exit
  Undoes state changes made preparing for power collapse.\n
  This function unmasks the global interrupts.
 
  @return
  QURT_EOK --Operation was successfully performed.

  @dependencies
  None.
 */
#define qurt_power_shutdown_exit qurt_power_exit

/**@ingroup qurt_power_exit
  Undoes state changes made preparing for exit single thread mode  .\n
  This function unmasks the global interrupts.

  @return
  QURT_EOK --Operation was successfully performed.

  @dependencies
  None.
 */
int qurt_power_exit(void);


/**@ingroup func_qurt_power_apcr_enter
  @xreflabel{sec:qurt_power_apcr_enter}   
  Triggers a APCR. \n
  This function perform following power collapse sequence
  mask the global interrupt, write QDSP6SS interrupt block register,
  disables interrupt request from L2VIC to QDSP6,
  If interrupt pending(31), transfer interrupt to l2vic, and clear pending interrupt
  It also umasks global interrupt, and puts current thread to wait 

  When all hardware threads have entered Wait mode, the Hexagon processor can enter
  APCR Power-collapse mode.

  @return
  QURT_ESTM -- Processor is not in Single-threaded mode.
  QURT_EOK -- Operation was successfully performed. This value indicates that the processor has
              returned from Power-collapse mode. \n

  @dependencies
  None.
 */
int qurt_power_apcr_enter (void);

/* 
  Determines if all other hardware threads in the QuRT system are in Wait or
  Stop mode. 

  If so, mask the global interrupt and return the result value QURT_EOK.
  If any hardware thread is not in Wait or Stop mode, then do nothing and return the
  result value QURT_ESTM.
 
  @return
  QURT_EOK -- Operation was successfully performed.\n
  QURT_ESTM -- Processor is not in Single-threaded mode.

  @dependencies
  None.
 */
int qurt_power_tcxo_prepare (void);

/* 
  Unmasks the global interrupt.
  This operation is used only when the thread is recovering from a failed TCXO shutdown
  operation
 
  @return
  QURT_EOK -- Operation was successfully performed.

  @dependencies
  None.
 */
int qurt_power_tcxo_fail_exit (void);

/*  
  Checks for pending interrupts.
  If any active (i.e., registered) interrupts are pending, return a bitmask value indicating the
  active pending interrupts.

  If no pending interrupts exist, change the interrupt mask to the value read from symbol
  QURTK_tcxo_intmask (which is specified as part of QuRT configuration). \n
  @note1hang The default value of QURTK_tcxo_intmask is 0xffffffff, which specifies
  that no interrupts are enabled.
  
  @return
  QURT_EOK -- Operation was successfully performed. \n
  Non-zero -- IPEND value masking out invalid interrupts.

  @dependencies
  None.
 */
int qurt_power_tcxo_enter (void);

/*  
  Restores the interrupt mask to the value defined when the operation
  qurt_power_tcxo_enter() was previously called.

  @return
  QURT_EOK -- Operation was successfully performed.

  @dependencies
  None.
 */
int qurt_power_tcxo_exit (void);

/**@ingroup power_management
  Enables or disables the wait override.

  The wait override causes a thread waiting for the idle state to remain suspended when the
  idle state occurs (instead of being awakened).

  The “override” status completely controlled by client who calls qurt_power_override_wait_for_idle().
  The behavior will be, “if a task is blocked on qurt_power_wait_for_idle and the last 
  qurt_power_override_wait_for_idle” is called with “0” (or it was never called since the system start up),
  the task gets scheduled when the entire system is idle. 

  For more information on wait overrides and the idle state, see qurt_power_wait_for_idle.\n

  @note1hang This function is used to perform power management.
 
   @input
   0 - Disable wait override 
   1 - Enable wait override

   @return
   None.
 
   @dependencies
   None.
*/
void qurt_power_override_wait_for_idle(int enable);

/**@ingroup power_management
   Suspends the current caller thread until the QuRT system enters the idle state, where all other
   threads in the system are also suspended. If the wait override is disabled, the suspended thread 
   is then awakened.

   If this function is called when all other threads in the system are already suspended, then the
   function returns immediately and the caller thread continues executing.
   
   The system is put to STM (Single Threaded Mode) by masking global interrupt when this function
   returns.

   Note: This function should not be called when another thread is already waiting for the idle state.
   If it is, the thread already waiting will be suspended forever.\n

  @note1hang This function is used to perform power management.
 
  @return
  returns 0 if nobody cancelled wait for STM
  returns 1 if sombody cancelled wait for STM through qurt_power_cancel_wait_for_idle call 

  @dependencies
  None.
 */
int qurt_power_wait_for_idle (void);

/**@ingroup power_management
   qurt_power_cancel_wait_for_idle()deregisters the thread that is waiting for all QuRT system to enter
   idle state (STM). This is done by putting the thread waiting for STM back in ready queue. If this
   API is called when no thread is waiting for STM to occur, API will simply return

  @note1hang This function is used to perform power management.
 
  @return
  None.

  @dependencies
  None.
 */
void qurt_power_cancel_wait_for_idle (void);

/**@ingroup func_qurt_power_wait_for_active
  Suspends the current caller thread until any other thread in the QuRT system is
  awakened. When this happens the caller thread is awakened.

  If any other thread is ready when this function is called, the caller thread is immediately
  scheduled. \n
 
  @return
  None.

  @dependencies
  This function is used to perform power management (Section @xref{sec:power_management}). It is useful
             only when paired with qurt_power_wait_for_idle().
 */
void qurt_power_wait_for_active (void);


/**@ingroup func_qurt_system_ipend_get
  Gets the IPEND register.\n
  Returns the current value of the Hexagon processor IPEND register.

  The return value is a mask value which identifies the individual interrupts that are
  pending.

  The bit order of the mask value is identical to the order defined for the IPEND register. A
  mask bit value of&nbsp;1 indicates that the corresponding interrupt is pending, and 0 that the
  corresponding interrupt is not pending.
 
  @return
  Integer -- IPEND register value; the pending interrupt.

  @dependencies
  None.
 */
unsigned int qurt_system_ipend_get (void);

/*
   Sets the AVS configuration register. 
 
   @param avscfg_value    AVS configuration value to be set.
  
   @return          
   None.

   @dependencies
   None.
 */
void qurt_system_avscfg_set(unsigned int avscfg_value);

/*
   Clears the IPEND register.
   Gets the AVS configuration register. 
   
   @return
   AVS configuration register value.

   @dependencies
   None.
 */
unsigned int qurt_system_avscfg_get(void);


/**@ingroup func_qurt_system_vid_get  
  Gets the VID register. \n
  Returns the current value of the Hexagon processor VID register.

  The return value is the vector number of a second-level interrupt that has been accepted by
  the Hexagon processor core.  
 
  @return 
  Integer -- VID register value; a vector number of L2 interrupt is accepted by the processor.
          Valid range is 0-1023.

  @dependencies
  None.
 */
unsigned int qurt_system_vid_get(void);

/*
   Gets the number of power collapses and counts of processor cycles 
   for entering and exiting most recent power collapse. If there 
   no power collapse has occured yet, processor cycles numbers will 
   be zero. 

   @param[out] enter_pcycles  Number of processor cycles for entering most
                              recent power collapse.
   @param[out] exit_pcycles  Number of processor cycles for exiting most
                             recent power collapse.
   @return      
   Zero -- No power collapses have occurred.
   Non-zero -- Number of power collapses that have occurred since
                the processor was reset

   @dependencies
   None.
 */ 
#define qurt_get_shutdown_pcycles qurt_power_shutdown_get_pcycles

/*
   Gets the number of power collapses and counts of processor cycles 
   for entering and exiting most recent power collapse. If there 
   no power collapse has occured yet, processor cycles numbers will 
   be zero. 

   @param[out] enter_pcycles  Number of processor cycles for entering most
                              recent power collapse.
   @param[out] exit_pcycles  Number of processor cycles for exiting most
                             recent power collapse.
   @return      
   Zero -- No power collapses have occurred.
   Non-zero -- Number of power collapses that have occurred since
                the processor was reset

   @dependencies
   None.
 */ 
int qurt_power_shutdown_get_pcycles( unsigned long long *enter_pcycles,  unsigned long long *exit_pcycles );

int qurt_system_tcm_set_size(unsigned int new_size);

/*
   Gets the raw time ticks of power collapses and counts of timer ticks 
   for entering and exiting most recent power collapse. If there 
   no power collapse has occured yet, these timetick numbers will 
   be zero. 

   @param[out] before_pc_ticks  The raw and absolute time ticks before the 
                               most recent power collapse is about to happen in hw.
   @param[out] after_wb_ticks   The raw and absolute time ticks right after 
                               the most recent warm boot is started.
   @return      
   Zero -- No power collapses have occurred.
   Non-zero -- Number of power collapses that have occurred since
                the processor was reset

   @dependencies
   None.
 */ 
int qurt_power_shutdown_get_hw_ticks( unsigned long long *before_pc_ticks,  unsigned long long *after_wb_ticks );

#endif /* QURT_POWER_H */
