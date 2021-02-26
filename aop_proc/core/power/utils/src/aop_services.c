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

#include "aop_fsm.h"
#include "unpa/unpa_resource.h"
#include "aop_services.h"

#define AOP_SERVICES_PRIORITY 105
#define AOP_SERVICES_TIMEOUT  (1920000)

// This code uses a very simple state machine.    When there are no requests pending, the
// IDLE State:   Used when no requests are pending
// IDLE->RESOURCE_WAIT State:  This state is entered when the first request comes in.
//   RESOURE_WAIT
//     Entry Actions:
//       Foreach Pending Request
//         Ensure any resources required to ENTER the workaround are available.
//         If they have not yet been requested, request them
//         If All Resources for Entry are available - 
//           Signal the FSM to EXIT the RESOURCE_WAIT state
//     Exit Actions:
//       Foreach Pending Request
//         If All Resources for Entry are available - 
//           Mark the request as no longer pending
//           Call the workaround
//             Note: The workaround returns any resources it may require, and may also
//                   update resources other workarounds may require.
//       Compile the list of currently required resources
//         If this set has not been voted, vote them
//       If there are any pending Requests, return to the RESOURCE_WAIT state
//       If all pending requests are done, return to the IDLE state.
//
typedef enum {
  AOP_SERVICES_IDLE,            // No Pending Requests
  AOP_SERVICES_RESOURCE_WAIT,   // Requests Pending/In Process
  NUM_SERVICES_FSM_STATES
} OPSCALING_FSM_STATES;

typedef enum {
  AOP_SERVICES_NEW            = 1, // Event sent when a new request comes in
  AOP_SERVICES_RESOURCE_READY = 2  // Event sent when a resource is READY, causes the 
                                   // FSM to evaluate if it can process any requests
} SEQUENCER_FSM_EVENTS;

static AOP_FSM aop_services_fsm;
static uint32_t pending_services;       // Track which services are pending.  Used before we evaluate their needs.
static uint32_t in_progress_services;   // Track which services are in progress (Services being acted upon/waiting for resources)

// The next two variables are shared from the target specific code.   They are a bitmask of what
// has been voted by this code, and what is currently on.
extern uint32_t pending_resource_sets;  // Tracks which resource requests are pending
extern uint32_t resource_sets_on;       // Tracks which resource requests are available
extern uint64_t time_service_now(void);
extern void aop_log_msg32(char *message, uint32 data);


// The following are the target specific services which need to be provided.
SERVICE_DESCRIPTOR *aop_services_ptr = NULL;
uint32_t           aop_services_count = 0;

uint64_t service_timeout_time = ~0;
bool     watchdog_interrupt_occurred = FALSE;

/**
 * <!-- abort_service_now -->
 *
 * @brief This function is called by any routine polling to see if we need them
 *        to abort their busywait.
 *
 * @param[in] None.
 * 
 * @return    True if we want the workaround to abort and give control to the main thread.
**/
bool abort_service_now()
{
  uint64_t now = time_service_now();
  if (watchdog_interrupt_occurred || (now > service_timeout_time))
  {
    uint64_t elapsed = now - service_timeout_time;
    aop_log_msg32("TIMEOUT!", watchdog_interrupt_occurred << 31 | (elapsed & 0x7FFFFFFF));
    return TRUE;
  }
  return FALSE;
}

/**
 * <!-- set_abort_time clear_abort_time -->
 *
 * @brief This function is called by any routine polling to see if we need them
 *        to abort their busywait.
 *
 * @param[in] None.
 * 
 * @return    None.
**/
void set_abort_time()
{
  service_timeout_time = time_service_now() + AOP_SERVICES_TIMEOUT;
}
void clear_abort_time()
{
  service_timeout_time = ~0;
}

/**
 * <!-- notify_new_service -->
 *
 * @brief This function is called when we have a new service being requested
 *
 * @param[in] new_service - Target specific new service being requested.
 * 
 * @note This function must be called in the thread context as it runs the FSM.
**/
void notify_new_service(uint32_t new_service)
{  
  // Add this service to the list of pending services
  pending_services |= new_service;

  // Ensure the FSM evaluates any new requests when its able.
  aop_fsm_process_event(&aop_services_fsm, AOP_SERVICES_NEW);
}

/**
 * <!-- signal_resource_done -->
 *
 * @brief Signals the FSM that a resource has completed, and the FSM should 
 *        exit the current state to evaluate any services that may be required.
 *        Also used when there are no services required for a new request.
 *
 * @param None
 * 
 * @note Resource states are managed by the target specific code.
**/
void signal_resource_done(void)
{
  aop_fsm_process_event(&aop_services_fsm, AOP_SERVICES_RESOURCE_READY);
}

/**
 * <!-- service_resource_entry -->
 *
 * @brief Ensures the resource requests are correct for any newly pending services
 *        and evaluates if any pending requests are ready for service
 *
 * @param[in] fsm   - Not used
 * @param[in] event - Not used.
 * 
**/
// We received a new request, make sure the resources we need are on before servicing any.
// If the resources are on, service it in the _exit routine.
void service_resource_entry (AOP_FSM *fsm, unsigned int event)
{
  int service;
  uint32_t current_needs = 0;

  in_progress_services |= pending_services;
  pending_services = 0;
 
  for (service = 0; service < aop_services_count; service++)
  {
    // If we have a new service pending
    if (in_progress_services & (1 << service))
    {
      // Determine the new needs
      uint32_t service_needs = aop_services_ptr[service].entry_resource_sets_needed;
      aop_services_ptr[service].current_resource_sets_needed |= aop_services_ptr[service].entry_resource_sets_needed;

      // If everything we need is ON, signal that we can proceed.
      if ((service_needs & resource_sets_on) == service_needs)
      {
        signal_resource_done();
      }
    }

    current_needs |= aop_services_ptr[service].current_resource_sets_needed;
  }

  // Now that processing is completed, Update what our current needs are.
  if (current_needs != pending_resource_sets)
  {
    vote_resource_needs(current_needs);
  }
}


/**
 * <!-- service_resource_exit -->
 *
 * @brief An event has occurred which means we should check if there is a service to do
 *
 * @param[in] fsm   - Not used
 * @param[in] event - Not used.
 * 
**/
// We got an event while waiting for a new voltage.
unsigned int service_resource_exit (AOP_FSM *fsm, unsigned int event, unsigned int next_state)
{
  uint32_t current_needs = 0;
  uint32 service;

  // Check each service if it can be run.
  for (service = 0; service < aop_services_count; service++)
  {
    // If we have a service pending
    if (in_progress_services & (1 << service))
    {
      // Determine the needs to run the service.   If all the needs are on
      // we will run the service.
      uint32_t service_needs = aop_services_ptr[service].entry_resource_sets_needed;
      if ((service_needs & resource_sets_on) == service_needs)
      {
        // Log that we are running the service (if logging is on)
        aop_log_msg32(aop_services_ptr[service].name, service << 24 | service_needs << 8 | resource_sets_on);

        // Mark it as no longer pending/in progress, we will service it.
        in_progress_services &= ~(1 << service);

        if (aop_services_ptr[service].perform_service != NULL)
        {
        // The funciton returns what resource shoudl remain on for this service.  It may be better for the service
        // to just do that itself.   The service already needs to update the current needs for any sister services
        // such as lock/unlock
          set_abort_time();
        aop_services_ptr[service].current_resource_sets_needed = aop_services_ptr[service].perform_service();
          clear_abort_time();
      }
      }
      else
      {
        // Still waiting for resources.
        aop_log_msg32(" WAITING", service << 24 | service_needs << 8 | resource_sets_on);
      }
    }
  }

  // Now that processing is completed, Update what our current needs are.
  for (service = 0; service < aop_services_count; service++)
  {
    current_needs |= aop_services_ptr[service].current_resource_sets_needed;
  }
  if (current_needs != pending_resource_sets)
  {
    vote_resource_needs(current_needs);
  }

  // The next_state defaults to waiting for more services. 
  // If there are no more services, return to IDLE
  if (in_progress_services == 0)
  {
    next_state = AOP_SERVICES_IDLE;
  }
  return next_state;
}

rex_tcb_type aop_services_tcb;
REX_DECLARE_STACK(aop_services_stack, 172 * sizeof(rex_stack_word_type));

/**
 * <!-- aop_services_main_loop -->
 *
 * @brief This function is the main thread, when we get a REX signal, call the
 *        target specific signal handler which maps rex signals to services.
 *
 * @param[in] unused - Parameter from rex - not used by us.
 * 
**/
void aop_services_main_loop(dword unused)
{
  unsigned long rex_signals = 0;
  while (TRUE)
  {
    rex_signals = rex_wait(aop_services_waitmask());
    if (rex_signals == 0) {
      continue;
    }
    rex_clr_sigs(&aop_services_tcb, rex_signals);
    aop_services_process(rex_signals);
  }
}

/**
 * <!-- service_fsm_init -->
 *
 * @brief Initialize the FSM and the rex thread which will perform the work.
 *
 * @param[in] callback_data - Client data passed as a part of client registration
 * @param[in] req_data      - RPMh request id associated with this callback
 * 
**/
void service_fsm_init(void)
{
  aop_fsm_init(&aop_services_fsm, NUM_SERVICES_FSM_STATES, "Workaround FSM");
  aop_fsm_init_state(&aop_services_fsm, AOP_SERVICES_IDLE,
                 NULL, NULL, "Idle");
  aop_fsm_init_state(&aop_services_fsm, AOP_SERVICES_RESOURCE_WAIT,
                 &service_resource_entry,
                 &service_resource_exit, "Wait for Resources");

  // Add each of the events to each of the states.
  // NEW_WANT can occur at any time.
  aop_fsm_add_event(&aop_services_fsm, AOP_SERVICES_IDLE,          AOP_SERVICES_NEW,
                    FALSE, AOP_SERVICES_RESOURCE_WAIT);
  aop_fsm_add_event(&aop_services_fsm, AOP_SERVICES_RESOURCE_WAIT, AOP_SERVICES_NEW,
                    FALSE, AOP_SERVICES_RESOURCE_WAIT);

  // Resource ready should only occur if we are waiting for a resource
  aop_fsm_add_event(&aop_services_fsm, AOP_SERVICES_RESOURCE_WAIT, AOP_SERVICES_RESOURCE_READY,
                    FALSE, AOP_SERVICES_RESOURCE_WAIT);

  memset(&aop_services_tcb, 0, sizeof(aop_services_tcb));
  rex_def_task(&aop_services_tcb, aop_services_stack,
               REX_STACK_SIZE(aop_services_stack),
               AOP_SERVICES_PRIORITY, &aop_services_main_loop, 0);

  target_aop_service_init();
}
