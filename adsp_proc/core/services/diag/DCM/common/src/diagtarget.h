#ifndef DIAGTARGET_H
#define DIAGTARGET_H
/*==========================================================================

                 Diagnostic Target Specific Prototypes

Description
  Prototypes for functions and data members that must be implemented to make
  the core diagnostics service work.
  

Copyright (c) 2001 - 2010, 2013,2015 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            Edit History

$Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagtarget.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/18/15   sa     Added support for time sync feature.
11/28/13   ph      Fixed the bug where diag_sleep_state won't be updated when diag
                   is switched to NRT mode.
09/27/13   xy      Removed code under FEATURE_WINCE  
10/29/10   vs     Added msg_get_time 
09/28/10   sg     Moved diag_time_get from diagtarget.h to diag_cfg.h
09/23/10   sg      Included diag_v.h to get the definition of 
                   DIAG_QDSP6_APPS_PROC on adsp
09/17/10   is      Migrate from deprecated clk driver APIs
05/11/10   is      Mainlined diag_stack
02/12/10   vs      Ported diag_time_get from depot
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
06/23/09   JV      Introduced OS abstraction layer
12/15/06   as      Fixed critical lint errors.
12/15/06   as      Fixed lint errors.
09/17/01   jal     Added prototype for diag_mode_change()
09/09/01    gr     Changed the featurization for the extern declaration
                   of the diag stack so that the stack is an array of type
                   rex_stack_word_type by default.
09/07/01    gr     Featurized the declaration of diag_stack. Restored
                   prototypes inadvertently deleted in the last check-in.
09/06/01    gr     Changed the extern declaration of diag_stack to reflect the
                   fact that the stack is now an array of type rex_stack_word_type.
06/27/01   lad     Added prototypes for diag_sleep_allow()/.._forbid().
04/06/01   lad     Added extern definitions of diag_tcb and diag_stack.
                   Changed diag_start_task() to diag_ready_start_task() to 
                   avoid confusion.
                   Added prototype for diag_kick_watchdog().
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"

#include "customer.h"

#include "osal.h"
#include "diag_v.h" /* For definition of DIAG_QDSP6_APPS_PROC*/

/* Declaration of the 8 bytes in the check block which is the security password.
** In DMSS targets, this symbol is located in the assemply firmware.
*/
extern uint8 const chk_security_code_ptr[];

/* The TCB and stack are defined outside the diag task.  In the MSMs, these are
 * defined in task.h/c
 */
extern osal_stack_word_type diag_stack [];

extern osal_tcb_t        diag_tcb;                    


/*===========================================================================

FUNCTION DIAG_READY_START_TASK

DESCRIPTION
  This procedure is called when the diag task is ready to start.

===========================================================================*/
void diag_ready_start_task (void);



/*===========================================================================

FUNCTION DIAG_SHUTDOWN

DESCRIPTION
  This procedure performs shutdown processing for the Diagnostic task.

  After this procedure is called the Diagnostic task will not be running.  
 
===========================================================================*/
void diag_shutdown (void); 



/*===========================================================================

FUNCTION DIAG_OFFLINE

DESCRIPTION
  This function performs processing required when the diag task receives
  a signal to go offline.

===========================================================================*/
void diag_offline (void);



/*===========================================================================
FUNCTION DIAG_POWERDOWN

DESCRIPTION
  This procedure sends the command to MC to power down the phone.

SIDE EFFECTS
  The phone will power down as a result of this call.
 
===========================================================================*/
void diag_powerdown (void);



/*===========================================================================

FUNCTION DIAG_KICK_WATCHDOG

DESCRIPTION
  This procedure reports to the watchdog and resets the timer.

===========================================================================*/
void diag_kick_watchdog (void);


/*===========================================================================

FUNCTION MSG_GET_TIME

DESCRIPTION
  This function gets the time-stamp for f3 debug messages.

===========================================================================*/

uint8 msg_get_time ( qword *ts );

/*===========================================================================

FUNCTION DIAG_TIME_INIT

DESCRIPTION
  Initializes the default time API to be used in diag logging (f3s/logs/events)
===========================================================================*/
boolean diag_time_init(void);



#endif /* DIAGTARGET_H */
