/*=============================================================================

                Ats_Time_Test.c

GENERAL DESCRIPTION
      ATS Timer Apis Test Code that executes from Guest OS

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009 - 2014
      by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.qdsp6/2.1/services/utimers/src/utimer_test.c#1 $ 
$DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
06/20/13   ab      Added file
=============================================================================*/
//#include "msg.h"
#include "rcinit.h"
//#include "msg.h"
#include "qurt_timer.h"
#include "timetick.h"
#include "time_timetick.h"
#include "uTimetick.h"
#include "qurt.h"
  /* test purpose */
#include "qw.h"
//#include "../../../core/kernel/qurt/libs/qurt/include/public/qurt_cycles.h"
#include "npa.h"
//#include "qurt_timer_defines.h"
#include "utimer.h"
#include "timer.h"
//#include "timer_v.h"

/* Todo: Remove these test files from scon compilation after final testing */

typedef enum
{
    LPASS_TEST1 = 1,
    LPASS_TEST2 = 2,
    LPASS_TEST3 = 4,
}utimer_test_enum_type;

utimer_type utimer_test_timer;
utimer_type utimer_func_type[4];
uint64      utimer_started[4];
uint64      utimer_duration[4];

#define UTIMER_TEST_LOOPS 500

void utimer_test_cb(int data)
{
   uint64 now, duration;
   now = uTimetick_Get();	/* Note: Commenting because qurt_sysclock_get_hw_ticks(); seems to be not provided in uimage. need to use utimetick_get() api */
   duration = (((now - utimer_started[data])* 100) /1920)/1000;
   duration = duration;
   
   //if(duration > (utimer_duration[data] + 10))
      //MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"timer[%d] got expired after %d expected at %d", data, duration, utimer_duration[data]);
   //else
      //MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"timer[%d] expired in time %d, set for %d", data, duration, utimer_duration[data]);
   
   utimer_duration[data] = now % 10000;
   utimer_set_64(&utimer_func_type[data], utimer_duration[data], 0, UT_MSEC);
   utimer_started[data] = now;
   
   return;
} /* utimer_test_cb */

/*=============================================================================

FUNCTION TIMER_GET_TIMETICK_HANDLE

DESCRIPTION
 This function is called from timer_init to pass timetick handle to timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void utimer_test_task
(
  void
)
{
  unsigned int mask_fired=0;
  unsigned int i;
  qurt_anysignal_t utimer_test_cb_signal, utimer_test_signal;
  utimer_error_type status;
  uint64 tt;
    
  
  //wait on rcinit handshake
  rcinit_handshake_startup();       /* Note: This is only for tasks that register with rcinit */
  
  /*initialize the task signal*/
  qurt_anysignal_init (&utimer_test_signal);

  /*prime the task*/
  (void) qurt_anysignal_set( &utimer_test_signal, LPASS_TEST1 );
    
  //loop forever
  for(;;)
  {
      mask_fired = qurt_anysignal_wait (&utimer_test_signal, LPASS_TEST1 | LPASS_TEST2 | LPASS_TEST3);

      if( mask_fired & LPASS_TEST1) 
      {

        qurt_anysignal_init (&utimer_test_cb_signal);

        /*define a timer for 1 sec*/
        utimer_def_osal(&utimer_test_timer, UTIMER_NATIVE_OS_SIGNAL_TYPE, &utimer_test_cb_signal, 0x1);
        utimer_def_osal(&utimer_func_type[0], UTIMER_FUNC1_CB_TYPE, &utimer_test_cb, 0);
        utimer_def_osal(&utimer_func_type[1], UTIMER_FUNC1_CB_TYPE, &utimer_test_cb, 1);
        utimer_def_osal(&utimer_func_type[2], UTIMER_FUNC1_CB_TYPE, &utimer_test_cb, 2);
        utimer_def_osal(&utimer_func_type[3], UTIMER_FUNC1_CB_TYPE, &utimer_test_cb, 3);

        /*set the timer for 1 sec*/
        utimer_set_64(&utimer_test_timer, 30, 0, UT_SEC);
				  
        //status = utimer_set_deferrable(&utimer_test_timer, TRUE);
		//MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer set_defer status: 0x%x", status);
		  
        utimer_set_64(&utimer_func_type[0], 30, 0, UT_SEC);
		
        utimer_set_64(&utimer_func_type[1], 30, 0, UT_SEC);
		//utimer_set_deferrable(&utimer_func_type[1], TRUE);
		
        utimer_set_64(&utimer_func_type[2], 30, 0, UT_SEC);
		
        utimer_set_64(&utimer_func_type[3], 30, 0, UT_SEC);
		//utimer_set_deferrable(&utimer_func_type[3], TRUE);
		
        utimer_started[0] = utimer_started[1] = utimer_started[2] = utimer_started[3] = uTimetick_Get();
        utimer_duration[0] = utimer_duration[1] = utimer_duration[2] = utimer_duration[3] = 30000; /* msecs */

        qurt_anysignal_wait(&utimer_test_cb_signal, 0x1);

        //MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer initial timer fired");

        for(i=0; i < UTIMER_TEST_LOOPS; i++ )
        {

          status = utimer_set_64(&utimer_test_timer, 1, 0, UT_SEC);  //1sec
          //MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"utimer set for %dth time, status: 0x%x", i+1,status);

          //timer_pause(&utimer_test_timer);
          //MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"ATS initial timer paused");

          //qurt_timer_sleep(1000000);

          //MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"ATS initial timer restarted");
          //timer_resume(&utimer_test_timer);

          qurt_anysignal_wait(&utimer_test_cb_signal, 0x1);

          //MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer fired");

          status = utimer_set_64(&utimer_test_timer, 5, 0, UT_SEC);  //5secs
          //MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer before sleep. set status: 0x%x", status);
          
          timer_sleep(1000000, T_USEC, FALSE);                                   //sleep for 1secs
          //MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer before stop");
          
          status = utimer_stop(&utimer_test_timer, UT_TICK, NULL);               //clear
          //MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer stopped. status: 0x%x",status);
          
          status = utimer_set_64(&utimer_test_timer, 1, 0, T_SEC);
          //MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer 3rd set stauts: 0x%x",status);

          status = utimer_undef(&utimer_test_timer);
          //MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer undef. Status: 0x%x",status);
          
          status = utimer_set_64(&utimer_test_timer, 1, 0, T_SEC);
          //MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer 4th set stauts: 0x%x",status);
          
          status = utimer_def_osal(&utimer_test_timer, UTIMER_NATIVE_OS_SIGNAL_TYPE, &utimer_test_cb_signal, 0x1);
          //MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer defined again. status: 0x%x, timer_idx: 0x%x",status, (utimer_test_timer&0xFFFF));
		  
		  tt = utimer_get_first_non_deferrable_timer_expiry();
		  //MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer non_def first expiry: 0x%.8x%.8x", tt>>32, (uint32)tt);
		  
		  tt = utimer_defer_match_interrupt_64();
		  //MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer def_64 ticks: 0x%.8x%.8x", tt>>32, (uint32)tt);
		  
		  timer_sleep(tt/20, T_USEC, TRUE);
		  
		  //MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer calling undef_64: 0x%.8x%.8x", tt>>32, (uint32)tt);
		  utimer_undefer_match_interrupt_64();
          
          status = status;  //For removing compiler warning
		}
        //MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"utimer_test is done for %d loops..",UTIMER_TEST_LOOPS);
		

      }
  }
}



