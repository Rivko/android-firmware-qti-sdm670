#ifndef QURT_FASTINT_H
#define QURT_FASTINT_H

/**
  @file qurt_fastint.h
  @brief <description>
*/
/*======================================================================
                        qurt_cond.h

GENERAL DESCRIPTION
  Prototypes of Kernel fast interrupt API functions      

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*======================================================================
 
 											 EDIT HISTORY FOR FILE
 
 	 This section contains comments describing changes made to the
 	 module. Notice that changes are listed in reverse chronological
 	 order.
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
  2012-12-16    cm          Tech pubs edits
 ======================================================================*/
/*=====================================================================
 Functions
======================================================================*/

/*======================================================================*/
/**@ingroup func_qurt_fastint_register
  Registers a fast interrupt by enabling the specified interrupt to call an 
  interrupt callback procedure.

  @note1hang On the V5 Hexagon processor, interrupts must be specified using the
             L2VIC interrupt numbers. Otherwise, use the L1 interrupt numbers.

  Fast interrupt callback procedures are user-defined functions which accept a single integer
  argument and return no result. When the function is called, the argument value passed is
  the interrupt number. Thus a single function can handle several different interrupts.

  QuRT reserves some interrupts for internal use -- the remainder are available for use by
  user programs, and thus are valid interrupt numbers. If the specified interrupt number is
  outside the valid range, the register operation returns the status value QURT_EINT.
    
  @note1hang Only one thread can be registered at a time to a specific interrupt. Attempting
             to register an already-registered interrupt returns the status value QURT_EVAL.
                  
  <b> Callback procedures</b> \n
  Callback procedures must be minimal and fast: while a fast interrupt is being serviced, it is
  prevented from being serviced on another hardware thread, and the interrupt cannot be
  serviced again until its callback procedure returns. \n
  
  @note1hang Up to one edge-triggered interrupt is pending while a callback procedure
             executes; any subsequent interrupts may be lost.  \n
  
  Fast interrupt callback procedures have the following restrictions: \n
  - They do not support nested interrupt handling. \n
  - They can be registered only for first-level interrupts. \n
  - They always execute with better-than-the-best-possible thread priority. \n
  - They cannot include spin (i.e., polling) code. \n
  - They do not support kernel calls that require a real thread context (such as blocking
     calls). Attempts to use such calls result in an error. \n
  
  Fast interrupt callback procedures should be designed to perform the minimal necessary
  management of the hardware, and then perform some operation (such as releasing a
  semaphore) to signal a normal thread to perform any additional processing required.

  @param[in] intno  Interrupt number.\n Valid range is 1, 10-31 (simulator only).
  @param[in] fn     Interrupt callback procedure. 

  @return
  QURT_EOK -- Fast interrupt was successfully registered. \n
  QURT_EINVALID -- Interrupt was already registered. \n
  QURT_EINT -- Invalid interrupt number. Values:\n
             - L1 interrupt > 31 \n
             - Interrupt reserved for QuRT  

  @dependencies
  None.

*/
/* ======================================================================*/
unsigned int qurt_fastint_register(int intno, void (*fn)(int));

/*======================================================================*/
/**@ingroup func_qurt_fastint_deregister  
  Deregisters the fast interrupt by disabling the specified interrupt from calling 
  any callback procedure.
	
  @param[in] intno  L1 interrupt number.
                    Valid range is 1, 10-31 (simulator only).
	
  @return
  QURT_EOK -- Interrupt deregister was successful. \n
  QURT_EINT -- Invalid interrupt number (not registered). \n
  QURT_EINVALID -- Interrupt was already deregistered.  

  @dependencies
  None.
*/
/* ======================================================================*/
unsigned int qurt_fastint_deregister(int intno);



/*======================================================================*/
/**@ingroup func_qurt_isr_register
  Registers the interrupt service routine by enabling the specified interrupt
  to call a long callback procedure.

  @param[in] intno  Interrupt number to register. Valid range is 1, 10-31 (simulator only).  
  @param[in] fn     Interrupt callback procedure. 
    
  @return
  QURT_EOK -- Fast interrupt was successfully registered. \n
  QURT_EINVALID -- Interrupt was already registered. \n
  QURT_EINT -- Invalid interrupt number. Values: \n
               - L1 interrupt > 31 \n
               - Interrupt reserved for QuRT   
		 
  @dependencies
  The ISR interrupt handler has the same limitation as the fast interrupt handler.
*/
/* ======================================================================*/
unsigned int qurt_isr_register(int intno, void (*fn)(int));


/*======================================================================*/
/**@ingroup func_qurt_isr_deregister
  Deregisters the interrupt service routine by disabling the specified interrupt 
  from calling any long callback procedure.
	
  @param[in] intno  Level-one interrupt number to deregister. Valid range is 1, 10-31 
                    (simulator only). 
	
  @return 				
  QURT_EOK -- Interrupt deregister was successful. \n
  QURT_EINT -- Invalid interrupt number (not registered). \n
  QURT_EINVALID -- Invalid interrupt number (already deregistered).

  @dependencies
  None.
*/
/* ======================================================================*/
unsigned int qurt_isr_deregister(int intno);


#endif /* QURT_FASTINT_H */
