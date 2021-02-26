/*=============================================================================

                utimer_client_test.c

GENERAL DESCRIPTION
      Utimer Apis Test Code that executes from Guest OS

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


$Header: //components/rel/core.qdsp6/2.1.c4/services/utimers/src/utimer_client_test.c#1 $ 
$DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
06/20/13   ab      Added file
=============================================================================*/

#ifdef FEATURE_UTIMER_CLIENT_TEST

//#include "msg.h"
#include "qurt.h"
#include "qurt_timer.h"
#include "utimer.h"
#include "uTimetick.h"
#include "timer.h"


typedef enum
{
    LPASS_TEST1 = 1,
    LPASS_TEST2 = 2,
    LPASS_TEST3 = 4,
}utimer_test_enum_type;

utimer_type utimer_client_test_timer;
utimer_type utimer_client_func_type[5];
uint64      utimer_client_started[5];
uint64      utimer_client_duration[5];
uint64      utimer_client_expired_times[5] = {0};
int         utimer_client_pid;

#define UTIMER_TEST_LOOPS 500

void utimer_client_test_cb(int data)
{
   //uint64 now, duration;
   //now = uTimetick_Get();	/* Note: Commenting because qurt_sysclock_get_hw_ticks(); seems to be not provided in uimage. need to use utimetick_get() api */
   //duration = (((now - utimer_client_started[data])* 100) /1920)/1000;
   //duration = duration;
   
   //if(duration > (utimer_client_duration[data] + 10))
      //MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"timer[%x] got expired after %d expected at %d", (utimer_client_pid<<8 | data), duration, utimer_client_duration[data]);
   //else
      //MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"timer[%x] expired in time %d, set for %d", (utimer_client_pid<<8 | data), duration, utimer_client_duration[data]);
   
   utimer_client_expired_times[data]++;
   utimer_client_duration[data] = 500; //now % 10000;
   utimer_set_64(&utimer_client_func_type[data], utimer_client_duration[data], 0, UT_MSEC);
   //utimer_client_started[data] = now;
   
   return;
} /* utimer_client_test_cb */

void utimer_client_periodic_test_cb(int data)
{
   //uint64 now, duration;
   //now = uTimetick_Get(); /* Note: Commenting because qurt_sysclock_get_hw_ticks(); seems to be not provided in uimage. need to use utimetick_get() api */
   //duration = (((now - utimer_client_started[data])* 100) /1920)/1000;
   //duration = duration;
   
   //if(duration > (utimer_client_duration[data] + 10))
      //MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"timer[%x] got expired after %d expected at %d", (utimer_client_pid<<8 | data), duration, utimer_client_duration[data]);
   //else
      //MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"timer[%x] expired in time %d, set for %d", (utimer_client_pid<<8 | data), duration, utimer_client_duration[data]);
   
   utimer_client_expired_times[data]++;
   utimer_client_duration[data] = 1000; /* Timer set for 1sec periodic duration */
   //utimer_set_64(&utimer_client_func_type[data], utimer_client_duration[data], 0, UT_MSEC);
   //utimer_client_started[data] = now;
   
   return;
} /* utimer_client_test_cb */

/*=============================================================================

FUNCTION UTIMER_CLIENT_TEST

DESCRIPTION
 This function is called from utimer_client.c to test timers from client space.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void utimer_client_test
(
  void *parameter
)
{
  unsigned int mask_fired=0;
  unsigned int i;
  qurt_anysignal_t utimer_client_test_cb_signal, utimer_test_signal;
  utimer_error_type status;
  //uint64 tt;
  
  utimer_client_pid = qurt_getpid();
  
  //wait on rcinit handshake
  //rcinit_handshake_startup();       /* Note: This is only for tasks that register with rcinit */
  
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

        qurt_anysignal_init (&utimer_client_test_cb_signal);

        /*define a timer for 1 sec*/
        utimer_def_osal(&utimer_client_test_timer, UTIMER_NATIVE_OS_SIGNAL_TYPE, &utimer_client_test_cb_signal, 0x1);
        utimer_def_osal(&utimer_client_func_type[0], UTIMER_FUNC1_CB_TYPE, &utimer_client_test_cb, 0);
        utimer_def_osal(&utimer_client_func_type[1], UTIMER_FUNC1_CB_TYPE, &utimer_client_test_cb, 1);
        utimer_def_osal(&utimer_client_func_type[2], UTIMER_FUNC1_CB_TYPE, &utimer_client_test_cb, 2);
        utimer_def_osal(&utimer_client_func_type[3], UTIMER_FUNC1_CB_TYPE, &utimer_client_test_cb, 3);
        utimer_def_osal(&utimer_client_func_type[4], UTIMER_FUNC1_CB_TYPE, &utimer_client_periodic_test_cb, 4);

        /*set the timer for 1 sec*/
        utimer_set_64(&utimer_client_test_timer, 1, 0, UT_SEC);
				  
        //status = utimer_set_deferrable(&utimer_client_test_timer, TRUE);
		//MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer set_defer status: 0x%x", utimer_client_pid, status);
		  
        utimer_set_64(&utimer_client_func_type[0], 1, 0, UT_SEC);
		
		//utimer_set_deferrable(&utimer_client_func_type[1], TRUE);
        utimer_set_64(&utimer_client_func_type[1], 1, 0, UT_SEC);
		
        utimer_set_64(&utimer_client_func_type[2], 1, 0, UT_SEC);
		
		//utimer_set_deferrable(&utimer_client_func_type[3], TRUE);
        utimer_set_64(&utimer_client_func_type[3], 1, 0, UT_SEC);

        /* Periodic test timer */
        utimer_set_64(&utimer_client_func_type[4], 1, 1, UT_SEC);
		
        utimer_client_started[0] = utimer_client_started[1] = utimer_client_started[2] = utimer_client_started[3] = uTimetick_Get();
        utimer_client_duration[0] = utimer_client_duration[1] = utimer_client_duration[2] = utimer_client_duration[3] = 30000; /* msecs */

        qurt_anysignal_wait(&utimer_client_test_cb_signal, 0x1);

        //MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer initial timer fired",utimer_client_pid);

        for(i=0; i < UTIMER_TEST_LOOPS; i++ )
        {

          status = utimer_set_64(&utimer_client_test_timer, 1, 0, UT_SEC);  //1sec
          //MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x utimer set for %dth time, status: 0x%x", utimer_client_pid, i+1,status);

          //timer_pause(&utimer_client_test_timer);
          //MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"ATS initial timer paused");

          //qurt_timer_sleep(1000000);

          //MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"ATS initial timer restarted");
          //timer_resume(&utimer_client_test_timer);

          qurt_anysignal_wait(&utimer_client_test_cb_signal, 0x1);

          //MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Utimer fired");

          status = utimer_set_64(&utimer_client_test_timer, 5, 0, UT_SEC);  //5secs
          //MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer before sleep. set status: 0x%x", utimer_client_pid, status);
          
          //timer_sleep(1000000, T_USEC, TRUE);                                //sleep for 1secs
          //MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer before stop", utimer_client_pid);
          
          status = utimer_stop(&utimer_client_test_timer, UT_TICK, NULL);               //clear
          //MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer stopped. status: 0x%x",utimer_client_pid, status);
          
          status = utimer_set_64(&utimer_client_test_timer, 1, 0, UT_SEC);
          //MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer 3rd set stauts: 0x%x",utimer_client_pid, status);

          status = utimer_undef(&utimer_client_test_timer);
          //MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer undef. Status: 0x%x",utimer_client_pid, status);
          
          status = utimer_clr_64(&utimer_client_test_timer, UT_SEC);
         // MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer Clr. remaining secs: 0x%x",utimer_client_pid, status);
          
          status = utimer_set_64(&utimer_client_test_timer, 1, 0, UT_SEC);
          //MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer 4th set stauts: 0x%x",utimer_client_pid, status);
          
          status = utimer_def_osal(&utimer_client_test_timer, UTIMER_NATIVE_OS_SIGNAL_TYPE, &utimer_client_test_cb_signal, 0x1);
          //MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer defined again. status: 0x%x, timer_idx: 0x%x",utimer_client_pid, status, (utimer_client_test_timer&0xFFFF));
		  
          status = status;
		  #if 0
		  tt = utimer_get_first_non_deferrable_timer_expiry_64();
		  //MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer non_def first expiry: 0x%.8x%.8x", utimer_client_pid, tt>>32, (uint32)tt);
		  
		  tt = utimer_defer_match_interrupt_64();
		  //MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer def_64 ticks: 0x%.8x%.8x", utimer_client_pid, tt>>32, (uint32)tt);
		  
		  //timer_sleep(tt/20, T_USEC, TRUE);
		  
		  //MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x Utimer calling undef_64: 0x%.8x%.8x", utimer_client_pid, tt>>32, (uint32)tt);
		  utimer_undefer_match_interrupt_64();
		  #endif /* #if 0 */
		}
        //MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%x utimer_test is done for %d loops..",utimer_client_pid, UTIMER_TEST_LOOPS);
		

      }
  }
}

#endif /* FEATURE_UTIMER_CLIENT_TEST */

