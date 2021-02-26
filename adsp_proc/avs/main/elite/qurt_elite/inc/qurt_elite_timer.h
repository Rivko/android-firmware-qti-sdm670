/*========================================================================
Copyright (c) 2010-2011, 2013-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */
/**
@file qurt_elite_timer.h

@brief This file contains utilities for timers. One-shot, period and sleep
timers are provided.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_timer_intro
The following functions provide one-shot, period, and sleep timers:
 - qurt_elite_timer_delete()
 - qurt_elite_timer_get_duration()
 - qurt_elite_timer_create()
 - qurt_elite_timer_create2()
 - qurt_elite_timer_sleep()
 - qurt_elite_timer_get_time()
 - qurt_elite_timer_get_time_in_msec()
 - qurt_elite_timer_oneshot_start_absolute()
 - qurt_elite_timer_oneshot_start_duration()
 - qurt_elite_timer_periodic_start()
 - qurt_elite_timer_oneshot_cancel()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite_timer.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
08/24/15   sw      (Tech Pubs) Edited Doxygen comments for 8952.
09/08/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4; edited
                   Doxygen comments/markup for 2.6.
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/10   brs     Edited for doxygen-to-latex process.
02/04/10   mwc     Created file.
========================================================================== */

#ifndef QURT_ELITE_TIMER_H
#define QURT_ELITE_TIMER_H

#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

/** @addtogroup qurt_elite_timer
@{ */

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/

/** Maximum duration value for ATS timers. */
#define ATS_TIMER_MAX_DURATION qurt_sysclock_max_duration()

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/****************************************************************************
** Timers
*****************************************************************************/

/** Valid timer types.
*/
typedef enum {
    QURT_ELITE_TIMER_ONESHOT_DURATION=0, /**< Duration-based one-shot timer. */
    QURT_ELITE_TIMER_ONESHOT_ABSOLUTE,   /**< Absolute one-shot timer. */
    QURT_ELITE_TIMER_PERIODIC            /**< Periodic timer. */
} qurt_elite_timer_duration_t;

/** Valid timer clock sources.
*/
typedef enum {
    QURT_ELITE_TIMER_USER      /**< General purpose timer for users/clients. */
} qurt_elite_timer_src_t;

/** Valid timer expiration handlers.
*/
typedef enum {
    QURT_ELITE_TIMER_NONE = 0, /**< No handler; start the timer. */
    QURT_ELITE_TIMER_SIGNAL,   /**< Signal generated to the calling thread. */
    QURT_ELITE_TIMER_CALLBACK  /**< Callback to a specified function.
                                    Currently not implemented. @newpage */
} qurt_elite_timer_hdlr_t;

/** QuRT_Elite timer structure.
*/
typedef struct {
    unsigned int qTimerObj;
    /**< Timer object. */ 

    unsigned int uTimerType;
    /**< Timer type; see #qurt_elite_timer_duration_t. */

    unsigned int      timer_sigmask;
    /**< Signal mask of the timer signal to be set when the timer expires. */

    qurt_elite_channel_t*     pChannel;
    /**< Pointer to the channel that waits for the timer signal. */

    boolean  istimerCreated;
    /**< Specifies whether the timer is created.

         @values
         - TRUE -- Created
         - FALSE -- Not created @tablebulletend */

    uint64_t duration;
    /**< Duration (in microseconds) of the timer.

          - For periodic timers, the duration value is the period of the
            timer.
          - For absolute timers, the duration value is the value of the
            absolute expiry time minus the time at start of the timer.
            @tablebulletend*/
                                                
    boolean is_deferrable_group;
    /**< Specifies whether the timer is deferrable.

         @values
         - TRUE -- Deferrable
         - FALSE -- Non-deferrable @tablebulletend */

}qurt_elite_timer_t;

/** QuRT_Elite timer attributes structure. The client sets the value, which
    is passed into qurt_elite_timer_create() or qurt_elite_timer_create2().
*/
typedef struct {
    boolean is_deferrable_group;
    /**< Specifies whether the timer is deferrable.

         @values
         - TRUE -- Deferrable
         - FALSE -- Non-deferrable @tablebulletend */

} qurt_elite_timer_attr_t;

/**
  Deletes an existing timer.

  @datatypes
  qurt_elite_timer_t

  @param[in] pTimer   Pointer to the QuRT_Elite timer object.

  @return
  Indication of success (0) or failure (nonzero).

  @dependencies
  The timer object must be created using qurt_elite_timer_create() or
  qurt_elite_timer_create2().
*/
int qurt_elite_timer_delete(qurt_elite_timer_t *pTimer);

/**
  Gets the duration of the specified timer.

  @datatypes
  qurt_elite_timer_t

  @param[in] pTimer   Pointer to the QuRT_Elite timer object.

  @return
  Duration of the timer, in microseconds.

  @dependencies
  The timer object must be created using qurt_elite_timer_create() or
  qurt_elite_timer_create2().
  @newpage
*/
unsigned long long qurt_elite_timer_get_duration(qurt_elite_timer_t *pTimer);

/**
  Creates a timer in the default deferrable timer group.

  @datatypes
  qurt_elite_timer_t \n
  #qurt_elite_timer_duration_t \n
  #qurt_elite_timer_src_t \n
  qurt_elite_signal_t

  @param[in] pTimer      Pointer to the QuRT_Elite timer object.
  @param[in] timerType   One of the following:
                          - #QURT_ELITE_TIMER_ONESHOT_DURATION
                          - #QURT_ELITE_TIMER_PERIODIC
                          - #QURT_ELITE_TIMER_ONESHOT_ABSOLUTE
                            @tablebulletend
  @param[in] clockSource Clock source is #QURT_ELITE_TIMER_USER.
  @param[in] pSignal     Pointer to the signal to be generated when the timer
                         expires.

  @detdesc
  The caller must allocate the memory for the timer structure and pass the
  pointer to this function.
  @par
  After calling this function, call the appropriate start functions based on
  the type of timer.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  This function must be called before arming the timer. @newpage
*/
int qurt_elite_timer_create(qurt_elite_timer_t *pTimer, qurt_elite_timer_duration_t timerType, qurt_elite_timer_src_t clockSource,
                   qurt_elite_signal_t *pSignal);

/**
  Creates a timer with attributes (pTimerAttr) passed in by the caller.

  @datatypes
  qurt_elite_timer_t \n
  #qurt_elite_timer_duration_t \n
  qurt_elite_timer_attr_t \n
  #qurt_elite_timer_src_t \n
  qurt_elite_signal_t

  @param[in] pTimer      Pointer to the QuRT_Elite timer object.
  @param[in] timerType   One of the following:
                          - #QURT_ELITE_TIMER_ONESHOT_DURATION
                          - #QURT_ELITE_TIMER_PERIODIC
                          - #QURT_ELITE_TIMER_ONESHOT_ABSOLUTE
                            @tablebulletend
  @param[in] pTimerAttr  Timer attributes set by clients.
  @param[in] clockSource Clock source is #QURT_ELITE_TIMER_USER.
  @param[in] pSignal     Pointer to the signal to be generated when the timer
                         expires.

  @detdesc
  The caller must allocate the memory for the timer structure and pass the
  pointer to this function.
  @par
  After calling this function, call the appropriate start functions based on
  the type of timer.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  This function must be called before arming the timer. @newpage
*/
int qurt_elite_timer_create2(qurt_elite_timer_t *pTimer, qurt_elite_timer_duration_t timerType,
        qurt_elite_timer_attr_t *pTimerAttr, qurt_elite_timer_src_t clockSource, qurt_elite_signal_t *pSignal);

/**
  Creates a synchronous sleep timer. Control returns to the callee
  after the timer expires.

  @param[in] llMicrosec  Duration the timer will sleep.

  @return
  Always returns 0. The return type is int for backwards compatibility.

  @dependencies
  None.
*/
int qurt_elite_timer_sleep(int64_t llMicrosec);

/**
  Gets the wall clock time.

  @return
  Returns the wall clock time in microseconds.

  @dependencies
  None.
*/
uint64_t qurt_elite_timer_get_time(void);

/**
  Gets the wall clock time in milliseconds.

  @return
  Returns the wall clock time in milliseconds.

  @dependencies
  None. @newpage
*/
uint64_t qurt_elite_timer_get_time_in_msec(void);


/**
  Restarts the absolute one-shot timer.

  @datatypes
  qurt_elite_timer_t
 
  @param[in] pTimer  Pointer to the QuRT_Elite timer object.
  @param[in] time    Absolute time of the timer, in microseconds.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must be created using qurt_elite_timer_create() or
  qurt_elite_timer_create2().
*/
int qurt_elite_timer_oneshot_start_absolute(qurt_elite_timer_t *pTimer, int64_t time);


/**
  Restarts the duration-based one-shot timer.

  @datatypes
  qurt_elite_timer_t

  @param[in] pTimer    Pointer to the QuRT_Elite timer object.
  @param[in] duration  Duration of the timer, in microseconds.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must be created using qurt_elite_timer_create() or
  qurt_elite_timer_create2(). @newpage
*/
int qurt_elite_timer_oneshot_start_duration(qurt_elite_timer_t *pTimer, int64_t duration);


/**
  Starts the periodic timer.

  @datatypes
  qurt_elite_timer_t

  @param[in] pTimer    Pointer to the QuRT_Elite timer object.
  @param[in] duration  Duration of the timer, in microseconds.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must be created using qurt_elite_timer_create() or
  qurt_elite_timer_create2().
*/
int qurt_elite_timer_periodic_start(qurt_elite_timer_t *pTimer, int64_t duration);


/**
  Cancels the duration-based one-shot timer.

  @datatypes
  qurt_elite_timer_t

  @param[in] pTimer   Pointer to the QuRT_Elite timer object.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must be created using qurt_elite_timer_create() or
  qurt_elite_timer_create2(). @newpage
*/
int qurt_elite_timer_oneshot_cancel(qurt_elite_timer_t *pTimer);

/** @} */ /* end_addtogroup qurt_elite_timer */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_TIMER_H

