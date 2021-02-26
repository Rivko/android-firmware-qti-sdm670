#ifndef AOP_CX_COLLAPSE_H
#define AOP_CX_COLLAPSE_H
/*===========================================================================
                             aop_cx_collapse.h

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
#include "unpa/unpa.h"
#include "aop_fsm.h"
#include "arc_runtime.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct cx_collapse_t
{
  AOP_FSM fsm;
  unpa_client *arc_cx_npa_client;
  unpa_client *vrm_cx_npa_client;
  unpa_client *arc_mx_npa_client;

  arc_runtime_client_t *arc_cx_runtime;
} cx_collapse_t;

typedef enum cx_collapse_states_enum
{
  CX_COLLAPSE_STATE_IDLE,
  CX_COLLAPSE_STATE_STAGE_1,
  CX_COLLAPSE_STATE_STAGE_2,
  CX_COLLAPSE_STATE_STAGE_3,
  CX_COLLAPSE_STATE_STAGE_4,
  CX_COLLAPSE_STATE_STAGE_5,

  CX_COLLAPSE_STATE_LAST,
  CX_COLLAPSE_STATE_NUM_STATES = CX_COLLAPSE_STATE_LAST
} cx_collapse_states_enum;

typedef enum cx_collapse_events_enum
{
  CX_COLLAPSE_EVENT_CX_PRE,

  CX_COLLAPSE_EVENT_CX_POST,

  CX_COLLAPSE_EVENT_APM,
  
  CX_COLLAPSE_EVENT_LAST,
  CX_COLLAPSE_NUM_EVENTS = CX_COLLAPSE_EVENT_LAST
} cx_collapse_events_enum;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================


#endif // AOP_CX_COLLAPSE_H
