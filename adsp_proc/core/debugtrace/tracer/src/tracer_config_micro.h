#ifndef TRACER_CONFIG_MICRO_H
#define TRACER_CONFIG_MICRO_H

/*=============================================================================

FILE:         tracer_config_micro.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "comdef.h"
#include "tracer_config_int.h"

//=========================================================================//
/**
  @brief Base size for bit array to enable (disable) output per
         entity.
 */
typedef uint32 entity_op_ctrl_t;
typedef uint32 event_op_ctrl_t;

#define ENTITY_OP_CTRL_TOTAL_ITEMS \
      ((TRACER_ENTITY_MAX >> 3) / sizeof(entity_op_ctrl_t)) + \
      ((TRACER_ENTITY_MAX & ((sizeof(entity_op_ctrl_t) << 3) - 1)) ? \
      1 : 0)

#define EVENT_OP_CTRL_TOTAL_ITEMS \
      ((TRACER_EVENT_ID_MAX >> 3) / sizeof(event_op_ctrl_t)) + \
      ((TRACER_EVENT_ID_MAX & ((sizeof(event_op_ctrl_t) << 3) - 1)) ? \
      1 : 0)
//=========================================================================//
/**
  @brief Structure containing tracer configuration.
 */
typedef struct {

   /** Flag indicating the tracer is to suppress all trace output
    *  requests.
    */
   boolean output_enabled;

   /** Bit mask for processing (bit set) or suppressing (bit
    *  cleared) traces associated with an entity id. Entity Id
    *  value maps to corresponding bit position.
    */
   entity_op_ctrl_t entity_op_ctrl[ENTITY_OP_CTRL_TOTAL_ITEMS]; //bit array

   int32 entity_in_use_count;

   event_op_ctrl_t event_op_ctrl[EVENT_OP_CTRL_TOTAL_ITEMS]; //bit array

   uint32 event_in_use_count;

   uint32 systest_ctl_addr;
} tracer_config_t;


/*---------------------------------------------------------------------------
 * External Variable Declarations
 * ------------------------------------------------------------------------*/

/**  Mutex in not self initialzing. Fudge it.
 */
extern boolean bTracerMutexInitialized;

/** Flag indicating the tracer has been initialized and is ready
    to output trace data.
 */
extern volatile boolean bTracerInitialized;

/** Flag indicating the tracer has been initialized for and
    ready to output handleless SW events.
 */
extern boolean bTracerEventSimpleInitialized;

/** Tracer configuration parameters.
 */
extern tracer_config_t tracer_config;

/*---------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ------------------------------------------------------------------------*/
extern void tracer_init_client_list(void);

#if TRACER_TESTING_ON
int tracer_test(void);
#endif

//=========================================================================//
// Mapping of a value to its corresponding position in a bit array
// Base data type of array could be 8, 16, 32, or 64-bit ==>
// DataTypeBytes = 1, 2, 4, or 8 respectively.
//
// Array elements needed
//  = (int)(MAX_VALUES/8/typebytes) + ([MAX_VALUE & ((8*typebytes) - 1)] ? 1 : 0 )
//  = (MAX_VALUES >> 3) / typebytes) + ([MAX_VALUE & ((typebytes<<3)- 1)] ? 1 : 0 )
// Value = (DataTypeBytes * 8 * ArrayIndex) + BitPos
// BitPos = Value % (DataTypeBytes * 8)   <-- modulo or remainder of division
//    Since each is a multiple of 8 just grab the appropriate # of lsbs,
//    = Value & (((sizeof((pArray)[0])) << 3) - 1)
// ArrayIndex = (int) Value / (int)(DataTypeBytes * 8)  <-- Integer division

#define ARRAY_IDX(pArray, val)      \
   ( (int)(val) / ((int)(sizeof((pArray)[0]) << 3)) )

#define ELEM_BIT(type, pArray, val) \
   ((type)((val) & ((sizeof((pArray)[0]) << 3) - 1)))

#define MASK_BIT(type, pArray, val) \
   ((type)(((uint32)1) << ELEM_BIT(type, pArray, val)))

#define SET_ARRAY_BIT(type, pArray, value)      \
   {                                            \
      if (((value) >> 3) < ((sizeof((pArray)))))\
      {                                         \
         (pArray)[ARRAY_IDX(pArray, value)] |=  \
            MASK_BIT(type, pArray, value);      \
      }                                         \
   }

#define CLR_ARRAY_BIT(type, pArray, value)      \
   {                                            \
      if (((value) >> 3) < ((sizeof((pArray)))))\
      {                                         \
         pArray[ARRAY_IDX(pArray, value)] &=    \
            ~(MASK_BIT(type, pArray, value));   \
      }                                         \
   }

#define GET_ARRAY_BIT(type, pArray, value)      \
      (((pArray)[ARRAY_IDX(pArray, value)] &    \
      (MASK_BIT(type, pArray, value))) ? 1 : 0)

/* ------------------------------------------------------*/
/* **** Bound checking analysis for
   GET_ARRAY_BIT() macro and event_op_ctrl[] ****

event_op_ctrl_t event_op_ctrl[EVENT_OP_CTRL_TOTAL_ITEMS];

With event_op_ctrl_t as an unsigned 32-bit integer;
sizeof(event_op_ctrl_t) = 4

Here's the breakdown for determining EVENT_OP_CTRL_TOTAL_ITEMS:
   {((TRACER_EVENT_ID_MAX >> 3) / sizeof(event_op_ctrl_t))} + {1 if there is a remainder}
   {((TRACER_EVENT_ID_MAX / 8)  / 4                      )} + {1 if there is a remainder}
   {((TRACER_EVENT_ID_MAX / 32                           )} + {1 if there is a remainder}
 The remainder is computed as
  (TRACER_EVENT_ID_MAX & ((sizeof(event_op_ctrl_t) << 3) - 1))
  (TRACER_EVENT_ID_MAX & ((4                       *  8) - 1))
  (TRACER_EVENT_ID_MAX & 31)
  (bitwise AND gives the remainder of a 32-bit division)

So EVENT_OP_CTRL_TOTAL_ITEMS is
   either TRACER_EVENT_ID_MAX/32
   or     TRACER_EVENT_ID_MAX/32 + 1
The first value is if TRACER_EVENT_ID_MAX is an exact multiple of 32,
the later value is if MAX is not an exact multiple of 32.

Thus: event_op_ctrl[TRACER_EVENT_ID_MAX/32]
   or event_op_ctrl[TRACER_EVENT_ID_MAX/32 + 1]

ARRAY_IDX(event_op_ctrl, event_id)
  ( (int)(event_id) / ((int)(sizeof((event_op_ctrl)[0]) << 3)) )
  (       event_id  /       (4                          << 3)  )
  ( event_id  / 32 )

The GET_ARRAY_BIT macro reduces down to computing the index of event_op_ctrl[],
then computing the bitwise AND of that value with the appropriate bit desired.
Ignoring the bitwise AND part, you get the macro expanding to

GET_ARRAY_BIT(event_op_ctrl_t, tracer_config.event_op_ctrl, event_id))
  (event_op_ctrl)[ ARRAY_IDX(event_op_ctrl, event_id) ]
   event_op_ctrl[event_id/32]

In the GET_ARRAY_BIT macro, the index resolves to event_id/32.
So this matches with the size of the defined event_op_ctrl[] array.

In the event_id is checked to be < TRACER_EVENT_ID_MAX
before GET_ARRAY_BIT() is called.

GET_ARRAY_BIT() is called from functions:
1)  _event_op_set_all() and _event_op_set() which in turn are only called from
tracer_event_ctrl().
2)  tcfg_is_event_on()

Both tracer_event_ctrl() and tcfg_is_event_on() verify that
TRACER_EVENT_ID_MAX > event_id before GET_ARRAY_BIT is called.
*/



#endif //TRACER_CONFIG_MICRO_H
