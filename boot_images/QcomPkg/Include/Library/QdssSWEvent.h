#ifndef QDSSSWEVENT_H
#define QDSSSWEVENT_H

/*=============================================================================

  FILE:         QdssSWEvent.h

  DESCRIPTION:  

  ================================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
  ==============================================================================*/
#include <Uefi.h>

void QdssSWEvent(IN UINT32 SwEventIdNum);
void QdssSWEventData(IN UINT32 SwEventIdNum,
                     IN UINT32 NumberArgs,
                     ...);


#define _QDSS_SWEVENT_GET_ID(_id,...)  _id

#define _QDSS_SWEVENT_GET_ARGS(_id,...) __VA_ARGS__


#define _QDSS_SWEVENT_NUM_ARGS(...)                               \
   _QDSS_SWEVENT_NUM_ARGS_EVAL(__VA_ARGS__,15,14,13,12,11,10,9,   \
                               8,7,6,5,4,3,2,1,0)

#define _QDSS_SWEVENT_NUM_ARGS_EVAL(_id,_1,_2,_3,_4,_5,_6,_7,_8,        \
                                    _9,_10,_11,_12,_13,_14,_15,N,...) N


#define _QDSS_SWEVENT_VARGS(...)   QdssSWEventData \
   (_QDSS_SWEVENT_GET_ID(__VA_ARGS__),                \
    _QDSS_SWEVENT_NUM_ARGS(__VA_ARGS__),              \
    _QDSS_SWEVENT_GET_ARGS(__VA_ARGS__)) 

#define _QDSS_SWEVENT(_id)   QdssSWEvent(_id)

//If there is only 1 argument this returns _QDSS_SWEVENT
//If there are 2 to 15 arguments
#define _QDSS_SWEVENT_GET_MACRO_NAME(...)                   \
   _QDSS_SWEVENT_GET_MACRO_NAME_EVAL(__VA_ARGS__,           \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT_VARGS,   \
                                     _QDSS_SWEVENT)

#define _QDSS_SWEVENT_GET_MACRO_NAME_EVAL(_1,_2,_3,_4,_5,               \
                                          _6,_7,_8,_9,_10,              \
                                          _11,_12,_13,_14,_15,NAME,...)  NAME




/** 
    Puts out a software event in the QDSS trace output stream.

    @param[in] id                    Value identifying the software event.
    @param[in] arg1,arg2,...arg15    Optional arguments of type uint32.

    @detdesc
    Usage is as follows
    QDSS_SWEVENT(id)              Issues a SW event with no arguments
    QDSS_SWEVENT(id,arg1,arg2)    Issues a SW event with two arguments
  

    @par
    Event ID values are dynamically assigned during the build. An event
    table is generated that maps the value with a descriptor of the event.
    @par
    The request may be silently discarded if QDSS is not initialized or the 
    event is not enabled.

    @return
    None.

    @dependencies
    None.
*/

#define QDSS_SWEVENT(...)                                   \
   _QDSS_SWEVENT_GET_MACRO_NAME(__VA_ARGS__)(__VA_ARGS__)








#endif //QDSSSWEVENT_H
