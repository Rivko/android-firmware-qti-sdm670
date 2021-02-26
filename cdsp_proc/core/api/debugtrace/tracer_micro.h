#ifndef TRACER_MICRO_H
#define TRACER_MICRO_H

/*=============================================================================

FILE:         tracer_micro.h

DESCRIPTION:  tracer interface for logging SW events from micro image.
              UTRACER_EVENT at the end is the only public macro

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "tracer.h"

#define _TRACER_EVENT_GET_ID(_id,...)  _id

#define _TRACER_EVENT_GET_ARGS(_id,...) __VA_ARGS__


#define _TRACER_EVENT_NUM_ARGS(...)                               \
   _TRACER_EVENT_NUM_ARGS_EVAL(__VA_ARGS__,15,14,13,12,11,10,9,   \
                               8,7,6,5,4,3,2,1,0)

#define _TRACER_EVENT_NUM_ARGS_EVAL(_id,_1,_2,_3,_4,_5,_6,_7,_8,        \
                                    _9,_10,_11,_12,_13,_14,_15,N,...) N


#define _TRACER_EVENT_SIMPLE_VARGS(...)   tracer_event_simple_vargs  \
   (_TRACER_EVENT_GET_ID(__VA_ARGS__),                               \
    _TRACER_EVENT_NUM_ARGS(__VA_ARGS__),                             \
    _TRACER_EVENT_GET_ARGS(__VA_ARGS__)) 

#define _TRACER_EVENT_SIMPLE(_id)   tracer_event_simple(_id)

//If there is only 1 argument this returns _TRACER_EVENT_SIMPLE
//If there are 2 to 15 arguments
#define _TRACER_EVENT_GET_MACRO_NAME(...)                         \
   _TRACER_EVENT_GET_MACRO_NAME_EVAL(__VA_ARGS__,                 \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE_VARGS,  \
                                     _TRACER_EVENT_SIMPLE)

#define _TRACER_EVENT_GET_MACRO_NAME_EVAL(_1,_2,_3,_4,_5,               \
                                          _6,_7,_8,_9,_10,              \
                                          _11,_12,_13,_14,_15,NAME,...)  NAME




/** 
  Puts out a software event in the QDSS trace output stream.
  Intented for use from sensor uImage

  @param[in] id                    Value identifying the software event.
  @param[in] arg1,arg2,...arg15    Optional arguments of type uint32.

  @detdesc
  Usage is as follows
  UTRACER_EVENT(id)              Issues a SW event with no arguments
  UTRACER_EVENT(id,arg1,arg2)    Issues a SW event with two arguments
  

  @par
  Event ID values are dynamically assigned during the build. An event
  table is generated that maps the value with a descriptor of the event.
  @par
  The request may be silently discarded if Tracer is not initialized,
  the Tracer port manager is not initialized, Tracer output is disabled,
  or output of the event ID is disabled.

  @return
  None.

  @dependencies
  None.
*/

#define UTRACER_EVENT(...)                                  \
   _TRACER_EVENT_GET_MACRO_NAME(__VA_ARGS__)(__VA_ARGS__)




#endif //TRACER_MICRO_H
