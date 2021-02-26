/*=============================================================================

                Timer_Client_Test.c

GENERAL DESCRIPTION
      Timer Client Test Code that runs from different processes

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


$Header: //components/rel/core.qdsp6/2.1/services/time/src/timer_client_test.c#1 $ 
$DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
06/20/13   ab      Add file
=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "msg.h"
#include "rcinit.h"
#include "msg.h"
#include "timetick.h"
#include "time_timetick.h"
  /* test purpose */
#include "qw.h"
#include "npa.h"
//#include "qurt_timer_defines.h"
#include "timer.h"
#include "timer_v.h"
#include "err.h"


typedef enum
{
    LPASS_TEST1 = 1,
    LPASS_TEST2=2,
    LPASS_TEST3=4,
}ats_timer_test_enum_type;

#define TIMER_TEST_LOOPS 500

qurt_anysignal_t timer_client_test_cb_signal;
timer_type timer_client_test_timer;
int timer_client_pid;

void timer_test_cb_func(timer_cb_data_type x)
{
   MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%d timer_test_cb_func: ATS client timer fired", x);
   qurt_anysignal_set( &timer_client_test_cb_signal, 0x1 );
}

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
void timer_client_test
(
  void *parameter
)
{
  unsigned int mask_fired=0;
  unsigned int i;
  qurt_anysignal_t ats_timer_test_signal;
  DALSYSEventHandle dalsys_test_handle;
  DALSYSEventObj  dalsys_test_obj;
  time_timetick_type tt;
  
  /*initialize the task signal*/
  qurt_anysignal_init (&ats_timer_test_signal);

  /*prime the task*/
  (void) qurt_anysignal_set( &ats_timer_test_signal, LPASS_TEST1 );
  
  timer_client_pid = qurt_getpid();
  
  timer_dalsys_sig_init(&dalsys_test_handle, &dalsys_test_obj);
  
  timer_register_for_non_defer_expiry_change(TIMER_DALSYS_EVENT_SIGNAL_TYPE, &dalsys_test_handle, 0);
    
  //loop forever
  for(;;)
  {
      mask_fired = qurt_anysignal_wait (&ats_timer_test_signal, LPASS_TEST1 | LPASS_TEST2 | LPASS_TEST3);

      if( mask_fired & LPASS_TEST1) 
      {

        qurt_anysignal_init (&timer_client_test_cb_signal);

        /*define a timer for 1 sec*/
        timer_def_osal(&timer_client_test_timer, NULL, TIMER_FUNC1_CB_TYPE, &timer_test_cb_func, timer_client_pid);

        /*set the timer for 1 sec*/
        timer_set_64(&timer_client_test_timer, 60, 0, T_SEC);
        
        qurt_anysignal_wait(&timer_client_test_cb_signal, 0x1);

        MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%d ATS initial timer fired",timer_client_pid);

        for(i=0; i < TIMER_TEST_LOOPS; i++ ) 
        {
          timer_set_64(&timer_client_test_timer, 1, 0, T_SEC);
          MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%d ATS client timer set for %dth time", timer_client_pid, i+1);

          timer_pause(&timer_client_test_timer);
          MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%d ATS client timer paused ",timer_client_pid);

          timer_sleep(1000000, T_USEC, FALSE);

          MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%d ATS client timer restarted", timer_client_pid);
          timer_resume(&timer_client_test_timer);

          qurt_anysignal_wait(&timer_client_test_cb_signal, 0x1);
          qurt_anysignal_clear (&timer_client_test_cb_signal, 0x1);
          
          //MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"%d ATS client timer fired",timer_client_pid);
        }
        
        timer_undef(&timer_client_test_timer);
        MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"ats_timer_test is Done..");
      }
      
      if( mask_fired & LPASS_TEST2) 
      {
        timer_type *test_timer[100];

        for(i=0; i<100; i++)
        {
           test_timer[i] = malloc(sizeof(timer_type));
           MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH, "%d timer malloced value: 0x%x, addr: 0x%x", timer_client_pid, (uint32) (*(uint32 *)test_timer[i]), (uint32)test_timer[i]);
        
           timer_def_osal(test_timer[i], NULL, TIMER_NO_NOTIFY_TYPE, NULL, NULL);
           MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH, "%d ATS client timer defined", timer_client_pid);
           
           timer_sleep(10000, T_USEC, FALSE);
        }

        for(i=0; i<100; i++)
        {
           timer_undef(test_timer[i]);
           free(test_timer[i]);
           MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH, "%d ATS client timer undef %d", timer_client_pid, i);
        }

      }
      if( mask_fired & LPASS_TEST3) 
      {
         timer_dalsys_sig_wait(&dalsys_test_handle);
         
         timer_dalsys_sig_reset(&dalsys_test_handle);

         tt = timer_get_first_non_deferrable_timer_expiry();
         
         MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH, "%d timer_get_first_non_deferrable_timer_expiry: 0x%x%x", timer_client_pid, tt>>32, tt);
      }
  }
}

