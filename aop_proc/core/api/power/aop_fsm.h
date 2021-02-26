#ifndef AOP_FSM_H
#define AOP_FSM_H
/*===========================================================================
                             aop_fsm.h

DESCRIPTION:
  Verbose description of what is defined by this header file. Typically
  describes the service provided by the structs, macros, etc. defined in
  this file.

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
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
#include "DALStdDef.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define AOP_FSM_EVENT_NONE 0x80000000

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
struct aop_fsm;
typedef void (*PFV)(struct aop_fsm *fsm, unsigned int event);
typedef unsigned int (*PFI)(struct aop_fsm *fsm, unsigned int event, unsigned int next_state);

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct aop_fsm_event
{
  unsigned int event_id;
  boolean save_for_later;
  PFI next_state_fn;
  unsigned int next_state_const;
  struct aop_fsm_event *next_event;
} AOP_FSM_EVENT;

typedef struct aop_fsm_state
{
  const char *name;
  PFV entry_fn;
  PFI exit_fn;
  unsigned int saved_for_later;
  AOP_FSM_EVENT *known_events;
} AOP_FSM_STATE;

typedef struct aop_fsm
{
  const char *name;
  unsigned int current_state;
  unsigned int pending_events;
  unsigned int last_event;
  unsigned int num_states;
  boolean processing_event;
  AOP_FSM_STATE *states;
} AOP_FSM;

//===========================================================================
//                             Function Definitions
//===========================================================================


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
void aop_fsm_init (AOP_FSM *fsm, int num_states, const char *name);

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
                        PFV entry_fn, PFI exit_fn, const char *name);

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
                       unsigned int save_for_later, unsigned int next_state);

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
//   Either FSM_EVENT_NONE if no events are to be processed, or one of the saved pending events.
void aop_fsm_clear_pending(AOP_FSM *fsm, unsigned int events);

// aop_fsm_get_next_event
//
// This function is called once we enter a new state and returns the next event, if any,
// to be processed.  It skips any events which would be saved_for_later in the current state.
//
// Paramters:
//   fsm:  The master FSM structure.
//
// Return:
//   Either FSM_EVENT_NONE if no events are to be processed, or one of the saved pending events.
unsigned int aop_fsm_get_next_event(AOP_FSM *fsm);


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
void aop_fsm_process_event(AOP_FSM *fsm, unsigned int event);

#endif // AOP_FSM_H



