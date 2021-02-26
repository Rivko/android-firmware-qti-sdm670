#ifndef __CVD_TASK_H__
#define __CVD_TASK_H__

/*
   Copyright (c) 2014, 2016 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvd_task.h#1 $
  $Author: pwbldsvc $
*/

#include "elite_thrd_prio.h"

#define TASK_PRIORITY( task_pri_macro ) ( 255 - task_pri_macro )
  /**<
   * Note that Elite defines 255 as the highest thread priority and 0 as the
   * lowest thread priority. Whereas QuRT defines 0 as the highest thread 
   * priority. Therefore we must use ( 255 - Elite thread priority ) to 
   * determine the QuRT thread priority. The QuRT priority is being used when 
   * APR thread OSAL creates a thread using QuRT on the ADSP. CVS, CVP, MVM and
   * VOCSVC all use APR thread OSAL to create its thread.
   */

#define MVM_MED_TASK_NAME ( "MVM" )
#define MVM_MED_TASK_PRIORITY ( TASK_PRIORITY( elite_get_thrd_prio(ELITETHREAD_MVM_MED_PRIO_ID) ) )
#define MVM_MED_TASK_STACK_SIZE ( 4 * 1024 )

#define CVD_VFR_TASK_NAME ( "CVD_VFR" )
#define CVD_VFR_TASK_PRIORITY ( TASK_PRIORITY( elite_get_thrd_prio(ELITETHREAD_CVD_VFR_PRIO_ID) ) )
  /**<
   * CVD VFR task runs during voice call setup/teardown and it has the same latency
   * requirement as other MVM call setupt/teardown commands. CVD VFR task shall
   * have lower priority than CVS_MED/CVP_MED in order to avoid having this task
   * preempting vocoder packet exchange or VPCM packet exchange during
   * concurrent voice use cases.
   */
#define CVD_VFR_TASK_STACK_SIZE ( 8 * 1024 ) /* Default value. */

#define CVS_MAILBOX_TIMER_TASK_NAME ( "CVS_MB_TIMER" )
#define CVS_MAILBOX_TIMER_TASK_PRIORITY ( TASK_PRIORITY( elite_get_thrd_prio(ELITETHREAD_CVS_MAILBOX_TIMER_PRIO_ID) ) )
  /**<
    * The mailbox timer thread is not a service thread. It is created only upon
    * the first time when mailbox packet exchange is configured. The thread is
    * used to receive AVTimer signal and trigger CVS to dequeue the decoder
    * packet on behalf of the decoder before the decoder becomes active. The
    * thread becomes inactive as soon as the decoder becomes active. The thread
    * is destroyed (if it was created) upon cvs_deinit. The thread's priority
    * is immediately above CVS since CVS depends on this thread.
    */
#define CVS_MAILBOX_TIMER_TASK_STACK_SIZE ( 8 * 1024 ) /* Default value. */

#define VOCSVC_SERVICE_TASK_NAME ( "VOCSVC" )
#define VOCSVC_TASK_STACK_SIZE ( 2 * 1024 ) /* Default value. */

#define CVD_CAL_LOGGER_TASK_NAME ( "CVD_CAL_LOGGER" )
#define CVD_CAL_LOGGER_TASK_PRIORITY ( TASK_PRIORITY( elite_get_thrd_prio(ELITETHREAD_CVD_CAL_LOGGER_PRIO_ID) ) )
#define CVD_CAL_LOGGER_TASK_STACK_SIZE ( 2 * 1024 )

#endif /* __CVD_TASK_H__ */


