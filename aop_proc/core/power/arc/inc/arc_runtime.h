#ifndef ARC_RUNTIME_H
#define ARC_RUNTIME_H
/*===========================================================================
                             arc_runtime.h

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
#include "rpmh_arc.h"
#include "comdef.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define ARC_RUNTIME_SUCCESS (0)
#define ARC_RUNTIME_FAILURE (-1)

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

typedef enum arc_runtime_event_t
{
  ARC_RUNTIME_EVENT_AGGREGATION,
  ARC_RUNTIME_EVENT_PREWORK,
  ARC_RUNTIME_EVENT_POSTWORK,

  ARC_RUNTIME_EVENT_AGG_UNBLOCK,
  ARC_RUNTIME_EVENT_PREWORK_UNBLOCK,
  ARC_RUNTIME_EVENT_POSTWORK_UNBLOCK,
  
  ARC_RUNTIME_EVENT_NUM  
} arc_runtime_event_t;

typedef enum arc_runtime_state_enum_t
{
  ARC_RUNTIME_IDLE,
  ARC_RUNTIME_WAIT_AGG_UNBLOCK,
  ARC_RUNTIME_WAIT_PREWORK_UNBLOCK,
  ARC_RUNTIME_WAIT_POSTWORK_UNBLOCK,
} arc_runtime_state_enum_t;

struct arc_runtime_client_t;

typedef uint32 (*arc_runtime_cb_t) (struct arc_runtime_client_t *,
                                    arc_runtime_event_t event,
                                    uint32 event_data,
                                    uint32 cb_data);

typedef struct arc_runtime_client_t
{
  rpmh_arc_enum_t arc;
  uint32 cb_data;
  
  uint32 events;
  arc_runtime_cb_t cb;
  bool blocked;
  struct arc_runtime_client_t *next;
} arc_runtime_client_t;




//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================
/**
 * <!-- arc_runtime_init -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void arc_runtime_init (void);

/**
 * <!-- arc_runtime_register -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
arc_runtime_client_t *arc_runtime_register (rpmh_arc_enum_t arc, arc_runtime_cb_t cb, uint32 cb_data);

/**
 * <!-- arc_runtime_event_register -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
int arc_runtime_event_register (arc_runtime_client_t *client, arc_runtime_event_t event);

/**
 * <!-- arc_runtime_event_deregister -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
int arc_runtime_event_deregister (arc_runtime_client_t *client, arc_runtime_event_t event);


/**
 * <!-- arc_runtime_event_dispatch -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void arc_runtime_event_dispatch (uint32 arc, arc_runtime_event_t e);

/**
 * <!-- arc_runtime_client_event_dispatch -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void arc_runtime_client_event_dispatch (arc_runtime_client_t *client,
                                        arc_runtime_event_t e, uint32 ed);

#endif // ARC_RUNTIME_H
