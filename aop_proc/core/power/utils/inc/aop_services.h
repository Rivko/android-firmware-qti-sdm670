/*===========================================================================
                              op_scaling.c

DESCRIPTION:

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rex.h"

#define SERVICE_RESOURCE_CNOC     1   // Ensure CNOC is on before service
#define SERVICE_RESOURCE_DDR      2   // Ensure full path to DDR is on before service
#define SERVICE_RESOURCE_DDR_W_TO 4   // Either RESOURCE_DDR above or a TIMEOUT 2ms
#define SERVICE_RESOURCE_ABORT    8   // ABORT!
#define MAX_SERVICE_RESOURCE 8

// The following correspond to the position in the array
enum services {
  BCM_WORKAROUND,     // 0
  CX_COLLAPSE_LOCK,   // 1
  CX_COLLAPSE_UNLOCK, // 2
  DBG_SIG_LOCK,       // 3
  DBG_SIG_UNLOCK,     // 4
  QMP_MSG_AVAILABLE,  // 5
  LOG_ABORT_ISR,      // 6
  CRASH_DUMP_NEEDED,  // 7
  BCM_CHANGE_DONE,    // 8
  PDC_WA,             // 9
  NUM_SERVICES,       // 10
};

#define BCM_WORKAROUND_SEQ_SIG     (1 << BCM_WORKAROUND)
#define CX_COLLAPSE_SIG_LOCK_SIG   (1 << CX_COLLAPSE_LOCK)
#define CX_COLLAPSE_SIG_UNLOCK_SIG (1 << CX_COLLAPSE_UNLOCK)
#define DBG_SIG_PDC_LOCK_SIG       (1 << DBG_SIG_LOCK)
#define DBG_SIG_PDC_UNLOCK_SIG     (1 << DBG_SIG_UNLOCK)
#define QMP_MESSAGE_RECVD_SIG      (1 << QMP_MSG_AVAILABLE)
#define LOG_WATCHDOG_BITE_SIG      (1 << LOG_ABORT_ISR)
#define CRASH_DUMP_REQUIRED_SIG    (1 << CRASH_DUMP_NEEDED)
#define BCM_CHANGE_DONE_SIG        (1 << BCM_CHANGE_DONE)
#define PDC_AOP_SUMMARY_SIG        (1 << PDC_WA)

// Before adding a new signal/service above, add it to the waitmask below too
#define SERVICES_TIMER_SIGNAL      0x80000000 // Included in AOP_SERVICES_WAIT
#define RESOURCE_SIGNAL            0x40000000 // Included in AOP_SERVICES_WAIT


#define AOP_SERVICES_WAIT (SERVICES_TIMER_SIGNAL | RESOURCE_SIGNAL | ((1 << NUM_SERVICES)-1)) // The OR of the above signals

uint32_t aop_services_waitmask(void);

rex_tcb_type aop_services_tcb;

typedef struct service_descriptor {
  char     *name;
  uint32_t entry_resource_sets_needed;
  uint32_t current_resource_sets_needed;
  uint32_t (*perform_service)(void);
} SERVICE_DESCRIPTOR;

extern SERVICE_DESCRIPTOR *aop_services_ptr;
extern uint32_t           aop_services_count;

// This function is called when we have a new voltage wanted by the MC/SHUB FSM
extern void notify_new_service(uint32_t new_service);
 
// This function is called when we are notified that some resource is complete.
extern void signal_resource_done(void);
extern void target_aop_service_init(void);
void notify_new_service(uint32_t new_service);
void vote_resource_needs(uint32_t current_needs);
void aop_services_process(uint32_t rex_signals);

extern bool abort_service_now();
extern bool watchdog_interrupt_occurred;
