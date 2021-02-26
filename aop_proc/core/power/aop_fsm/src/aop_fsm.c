/*===========================================================================
                                 aop_fsm.c

DESCRIPTION:
  This file implements the finite state machine used by DDR_MGR, CX Collapse,
  and others in the AOP.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <string.h>
#include "aop_fsm.h"
#include "CoreVerify.h"

// aop_fsm_init
//
// This function initializes the finite state machine data structure.  It allocates the
// states array.  This function is called first for each FSM.  
//
// Paramters:
//   fsm:  The master FSM structure.
//   num_state: The number of states the FSM will have.
//   name: String name for debug purposes.
//
// Return:
//   none.
void aop_fsm_init(AOP_FSM *fsm, int num_states, const char *name)
{
  fsm->name = name;                     // Save the name.
  fsm->current_state = 0;               // Starting state is always 0.
  fsm->pending_events = 0;              // No events are pending.
  fsm->last_event = AOP_FSM_EVENT_NONE; // No events are serviced.
  fsm->num_states = num_states;         // Save the max number of states.
  fsm->processing_event = FALSE;        // Save the max number of states.

  // Allocate and initialize the state array.
  fsm->states = (AOP_FSM_STATE *) malloc(sizeof(AOP_FSM_STATE) * num_states);
  CORE_VERIFY(fsm->states != NULL);
  memset(fsm->states, 0, (sizeof(AOP_FSM_STATE) * num_states));
}

// aop_fsm_add_event
//
// This function adds an event which causes us to handle an event in a state.
// If save_for_later is true, the event is expected in a state, but not acted upon.
// Otherwise, we put this event into a linked list of known events which will cause us
// to call the exit_fn.   The exit_fn will cause a state exit and return the next_state.
//
// Paramters:
//   fsm:  The master FSM structure.
//   state: The number of states the FSM will have.
//   event: The event which we are expecting.
//   save_for_later: This event is allowed in the state, but no action is taken yet.
//   next_state: When this event occurs, next_state is the expected next_state.
//               NOTE: exit_fn can override the next_state clue if defined.
//
// Return:
//   none.

void aop_fsm_add_event(AOP_FSM *fsm, unsigned int state, unsigned int event,
                       unsigned int save_for_later, unsigned int next_state)
{
  if (save_for_later == false) 
  {
    // Allocate a new event.  Store necessary info.
    AOP_FSM_EVENT *new_event = malloc(sizeof(AOP_FSM_EVENT));
    CORE_VERIFY (new_event != NULL);
    new_event->event_id = event;
    new_event->save_for_later = save_for_later;
    new_event->next_state_const = next_state;

    // Put the new event into the linked list.
    new_event->next_event = fsm->states[state].known_events;
    fsm->states[state].known_events = new_event;
  }
  else
  {
    // Saved for later events are just put into the bitmask.
    fsm->states[state].saved_for_later |= event;
  }
}

// aop_fsm_init_state
//
// This function initializes a state in the finite state machine data structure.  Each
// state is defined by the entry function and exit function.   The events the state handles
// will be put into known_events.
//
// Paramters:
//   fsm:  The master FSM structure.
//   state: The number of states the FSM will have.
//   entry_fn: The function we call when entering this state.   If NULL, no fn will be called.
//   exit_fn: The function we call when exit this state, it chooses the next state.
//            If NULL, no fn will be called, the next_state is fixed.
//   name: String name for debug purposes.
//
// Return:
//   none.
void aop_fsm_init_state(AOP_FSM *fsm, unsigned int state,
                        PFV entry_fn, PFI exit_fn, const char *name)
{
  if (state >= fsm->num_states)
  {
    CORE_VERIFY(0);
  }
  fsm->states[state].name = name;
  fsm->states[state].entry_fn = entry_fn;
  fsm->states[state].exit_fn = exit_fn;
  fsm->states[state].known_events = NULL;
}

// aop_fsm_clear_pending
//
// Clear a pending event if it exists.  This function is used to indicate current processing would
// have accounted for a pending event.   Keeps the state machine from processing events if they are
// no longer necessary.
//
// Paramters:
//   fsm:  The master FSM structure.
//   events: The event bitmask to clear.
//
// Return:
//   Either AOP_FSM_EVENT_NONE if no events are to be processed, or one of the saved pending events.
void aop_fsm_clear_pending(AOP_FSM *fsm, unsigned int events)
{
  fsm->pending_events = fsm->pending_events & ~events;
}

// aop_fsm_get_next_event
//
// This function is called once we enter a new state and returns the next event, if any,
// to be processed.  It skips any events which would be saved_for_later in the current state.
//
// Paramters:
//   fsm:  The master FSM structure.
//
// Return:
//   Either AOP_FSM_EVENT_NONE if no events are to be processed, or one of the saved pending events.
unsigned int aop_fsm_get_next_event(AOP_FSM *fsm)
{
  unsigned int bit_mask = (fsm->last_event << 1);
  unsigned int non_save_for_later_events =
    (fsm->pending_events & ~fsm->states[fsm->current_state].saved_for_later);

  while (non_save_for_later_events)
  {
    // Check for wrapping
    if ((bit_mask == 0)  || (bit_mask > non_save_for_later_events)) {
      bit_mask = 1;
    }
    if (bit_mask & non_save_for_later_events) {
      fsm->pending_events = bit_mask ^ fsm->pending_events;
      fsm->last_event = bit_mask;
      return bit_mask;
    }
    bit_mask <<= 1;
  }

  return AOP_FSM_EVENT_NONE;
}

// aop_fsm_process_event
//
// This function is called when an event comes in/or was previously saved.
// It first checks if the event should be saved_for_later, and if so, saves it in
// fsm->pending_events.
// Otherwise, we search through known_events looking for the saved_event.  Once found,
// we exit the current state and enter the next state.  Once in the new state, we 
// search any pending_events and call fsm_process_event on the first such event.
//
// Paramters:
//   fsm:  The master FSM structure.
//   event: The event which we are expecting.
//
// Return:
//   none.
void aop_fsm_process_event(AOP_FSM *fsm, unsigned int event)
{
  AOP_FSM_EVENT *check_event;
  unsigned int next_state;

  // If the event is to be saved for later, save it off and return.  It will be
  // processed in a later state.
  if ((fsm->processing_event == TRUE) ||
      ((fsm->states[fsm->current_state].saved_for_later & event) != 0))
  {
    fsm->pending_events |= event;
    return;
  }

  // Indicate that we are now processing events.   Any event that comes in 
  // now will be processed after the current event.
  fsm->processing_event = TRUE;

  // The while loop only processes non-saved for later events.
  while (event != AOP_FSM_EVENT_NONE)
  {
    // Save this event as the last one being processed.
    fsm->last_event = event;

    // Look for the event in the expected list.
    check_event = fsm->states[fsm->current_state].known_events;
    while (check_event != NULL)
    {
      if (check_event->event_id == event)
      {
        if (fsm->states[fsm->current_state].exit_fn) {
          next_state = (*fsm->states[fsm->current_state].exit_fn)
                        (fsm, event, check_event->next_state_const);
        }
        else
        {
          next_state = check_event->next_state_const;
        }
        fsm->current_state = next_state;
        if (fsm->states[fsm->current_state].entry_fn) {
          (*fsm->states[fsm->current_state].entry_fn) (fsm, event);
        }
        break;
      }
      check_event = check_event->next_event;
    }
    // Get any other pending event.
    event = aop_fsm_get_next_event(fsm);
  }
  fsm->processing_event = FALSE;

  return;
}

