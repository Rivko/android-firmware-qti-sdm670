#ifndef QURT_TIMER_DEFINES_H
#define QURT_TIMER_DEFINES_H
/*=============================================================================

                 qurt_TimerDefines.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   qurt_timer constants and macros 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/



/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/

/**
 Define magic                                                          */
#define QURT_TIMER_MAGIC                   0xadadadad
#define QURT_TIMER_EXPIRED                 0xedededed
#define QURT_TIMER_INVALID                 0xdfdfdfdf
#define QURT_TIMER_ATTR_MAGIC              0xaeaeaeae
#define QURT_TIMER_ATTR_INVALID            QURT_TIMER_INVALID

/**
 Supported timer types                                           
*/
#define QURT_TIMER_ABSOLUTE_EXPIRY          1
#define QURT_TIMER_RELATIVE_DURATION        2

/** 
 Sleep Timer Calculation Margin for round/chunc difference                 
*/
#define QURT_TIMETICK_CALCULATION_MARGIN    1

#define QURT_TIMER_CLIENT_ACK_SIGMASK       0x1

/** 
  qurt_timer ipc related defines                                               
*/
#define QURT_TIMER_IPC_CREATE              1
#define QURT_TIMER_IPC_DELETE              2
#define QURT_TIMER_IPC_GET_ATTR            3

/**
  SLEEP_TIMER 
*/
#define QURT_TIMER_IPC_SLEEP               4
#define QURT_TIMER_IPC_GET_TICKS           5
#define QURT_TIMER_IPC_GROUP_EN            6
#define QURT_TIMER_IPC_GROUP_DIS           7
#define QURT_TIMER_IPC_INTERRUPT           8
#define QURT_TIMER_IPC_PC                  9
#define QURT_TIMER_IPC_RESTART            10
#define QURT_TIMER_IPC_STOP               11

/*=============================================================================
                        TYPEDEFS
=============================================================================*/

typedef unsigned long long              qurt_timetick_type;
typedef unsigned long long              qurt_timetick_word_t;

#endif /* QURT_TIMER_DEFINES_H */
