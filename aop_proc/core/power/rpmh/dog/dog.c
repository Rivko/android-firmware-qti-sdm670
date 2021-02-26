/*============================================================================

FILE:      dog.c

DESCRIPTION: AOP watchdog driver

PUBLIC CLASSES: 

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

      Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/rpmh/dog/dog.c#1 $
$Date: 2018/02/16 $

============================================================================*/

#include <err.h>

#if 0 /* Some extra include from original code */
#include <DALDeviceId.h>
#include <DALSys.h>
#include "dog_hwio.h"
#include "CoreVerify.h"
#include "Chipinfo.h"
#endif

#include <string.h>
#include "time_service.h"
#include "dog.h"
#include "HALhwio.h"
#include "aop_hwio.h"
#include "rex.h"
#include "aop_interrupt_table.h"
#include "aop_qtmr_1_hwio.h"
#include "msg_ram_layout.h"
#define RUMI

void busywait (uint32 pause_time_us);

#define DOG_RUN_PRIORITY         200
#define WATCHDOG_TIMER_CLK_HZ    (32768)
#ifdef RUMI
/* Napali RUMI AOP - 900 KHz, PoR AOP 200 MHz - 200/0.9 = 222 */
#define WATCHDOG_MS_TO_TIMER(ms) ((int)(222*((ms*WATCHDOG_TIMER_CLK_HZ)/1000)))
#else /* RUMI */
#define WATCHDOG_MS_TO_TIMER(ms) ((int)((ms*WATCHDOG_TIMER_CLK_HZ)/1000))
#endif /* RUMI */

#define WATCHDOG_BARK_TIME_SCLK WATCHDOG_MS_TO_TIMER(10)
#define WATCHDOG_BITE_TIME_SCLK WATCHDOG_MS_TO_TIMER(30)
#define WATCHDOG_TASK_TIME (WATCHDOG_BARK_TIME_SCLK - WATCHDOG_MS_TO_TIMER(5))
#define SW_WDOG_FREEZE   0x0fee0fee

static dog_stats_type dog_stats;

#ifdef DISABLE_AOP_WATCHDOG
volatile boolean debug_disable_dog = TRUE;
#else
volatile boolean debug_disable_dog = FALSE;
#endif

volatile boolean sw_dog_frozen = TRUE;

static rex_crit_sect_type dog_critsect;

#ifdef USE_AOP_PMIC
static dog_stats_type pmic_dog_stats;
#endif /* USE_AOP_PMIC */

/* DOG TASK structures */
rex_tcb_type dog_run_tcb;
REX_DECLARE_STACK(dog_run_stack, 128);

static void dog_update_stats(dog_stats_type* dog, dog_event_type dog_event, uint32* source)
{
  dog->entries[dog->index].dog_event = dog_event;
  dog->entries[dog->index].dog_event_source = source;
  dog->entries[dog->index].timestamp = time_service_now();
  dog->index = (dog->index + 1) % DOG_STATS_ENTRIES;
}

static void dog_crude_delay( int x ){
  volatile int y;

  while( x-- ){
    y = 1000;
    while( y-- );
  }
}

#if defined __ARMCC_VERSION__
__asm void dog_bark_isr(void) __irq
{
  IMPORT notify_tz
  IMPORT abort

  bl notify_tz
  b abort
}
#elif defined __clang__
__asm void dog_bark_isr(void) __irq
{
  asm("bl notify_tz");
  asm("b abort");
}
#endif

void dog_set_bark_time(unsigned bark_time)
{
  rex_enter_crit_sect(&dog_critsect);
  while(!HWIO_INF(AOP_WDOG_BARK_TIME, SYNC_STATUS));
  HWIO_OUTF(AOP_WDOG_BARK_TIME, WDOG_BARK_VAL, bark_time);
  rex_leave_crit_sect(&dog_critsect);
}

void dog_set_bite_time(unsigned bite_time)
{
  rex_enter_crit_sect(&dog_critsect);
  while(!HWIO_INF(AOP_WDOG_BITE_TIME, SYNC_STATUS));
  HWIO_OUTF(AOP_WDOG_BITE_TIME, WDOG_BITE_VAL, bite_time);
  rex_leave_crit_sect(&dog_critsect);
}

void dog_enable(unsigned enabled)
{
  if(sw_dog_frozen)
  {	  
    return;
  }    

  enabled = enabled ? 3 : 0; // Identify appropriate settings.
  HWIO_OUT(AOP_WDOG_CTRL, enabled);
}

#ifdef USE_AOP_WATCHDOG_RECLAIM_POOL
__attribute__((section("dog_dram_reclaim_pool")))
#endif

// kick the dog and start the dog_task timer
void dog_task_run(void)
{
  if(sw_dog_frozen)
  {	  
    return;
  }    

  //pet the dog	
  dog_kick();

  //set timer
  uint64 dog_task_isr_time = time_service_now() + WATCHDOG_TASK_TIME;
  uint32 lo = (uint32)dog_task_isr_time;
  uint32 hi = (dog_task_isr_time >> 32);

  HWIO_OUT(AOP_F1_QTMR_V1_CNTP_CTL, 0);
  HWIO_OUT(AOP_F1_QTMR_V1_CNTP_CVAL_LO, lo);
  HWIO_OUT(AOP_F1_QTMR_V1_CNTP_CVAL_HI, hi);
  HWIO_OUT(AOP_F1_QTMR_V1_CNTP_CTL, 1);
}

static void dog_task_stop_internal(boolean disable_wdog)
{
  //pet the dog	
  dog_kick();

  //disable timer
  HWIO_OUT(AOP_F1_QTMR_V1_CNTP_CTL, 0);

  if(disable_wdog)
  {
    sw_dog_frozen = TRUE;
  }
  //suspend the dog task
  rex_suspend_task(&dog_run_tcb);
}

static void dog_run (dword param1)
{
  while(1)
  {  
    uint32 cookie = debug_disable_dog ? SW_WDOG_FREEZE : msg_ram_get_wdog_cookie();

    //switch in case we add additional cookie values later
    switch(cookie)
    {
      case SW_WDOG_FREEZE:
        if(!sw_dog_frozen)
        {      
          dog_task_stop_internal(TRUE);		
        }	
        break;
      default:
        //kick the dog and start the dog_task timer
        dog_task_run();

        //suspend the dog task
        rex_suspend_task(&dog_run_tcb);
        break;
    }
  }
}

void dog_task_isr(void)
{
  //disable the dog timer
  HWIO_OUT(AOP_F1_QTMR_V1_CNTP_CTL, 0);
  //schedule the dog_task. This makes sure rex is working such that it can task switch/pet the dog
  rex_resume_task(&dog_run_tcb);
}

void dog_task_stop(void)
{
  //lock if we are calling dog_task_stop from sleep
  rex_enter_crit_sect(&dog_critsect);	
  if(!sw_dog_frozen)
  {
    dog_task_stop_internal(FALSE);
  }
  rex_leave_crit_sect(&dog_critsect);	
}

void dog_init(void)
{
  static boolean already_init = FALSE;

  if(already_init)
    return;

  already_init = TRUE;

  rex_init_crit_sect(&dog_critsect);

  if(!debug_disable_dog)
  {  
    /* Start the dog ticking. */
    HWIO_OUTF(AOP_TIMERS_CLK_OFF_CTL, WDOG_TIMER_CLK_OFF, 0x0);
    dog_crude_delay(153); /* busywait(153); */
    dog_set_bark_time(WATCHDOG_BARK_TIME_SCLK);
    dog_set_bite_time(WATCHDOG_BITE_TIME_SCLK);

    /* Set interrupt vector */
    rex_set_interrupt_vector(AOP_WDOG_BARK_IRQ, (rex_isr_func_type)dog_bark_isr);
    rex_enable_interrupt(AOP_WDOG_BARK_IRQ);

    dog_enable(TRUE);

    //allow read/write access to timer registers
    HWIO_OUTI(AOP_QTMR_AC_CNTACRn_FG0, 1, 33);

    // Enable the QTimer interrupt
    rex_set_interrupt_vector(AOP_QTIMER_INTR_1, (rex_isr_func_type)dog_task_isr);
    rex_clear_interrupt (AOP_QTIMER_INTR_1);
    rex_enable_interrupt (AOP_QTIMER_INTR_1);

    // Enable the sw dog
    sw_dog_frozen = FALSE;

    // Create the dog task
    memset (&dog_run_tcb, 0, sizeof (dog_run_tcb));
    rex_def_task (&dog_run_tcb, dog_run_stack, REX_STACK_SIZE(dog_run_stack), DOG_RUN_PRIORITY, dog_run, 0);
  }      
}

void dog_kick(void)
{
  if(!sw_dog_frozen)
  {
    void *return_address = (void *)__return_address();	
    rex_enter_crit_sect(&dog_critsect);
    while(!HWIO_INF(AOP_WDOG_RESET, SYNC_STATUS));
    HWIO_OUTF(AOP_WDOG_RESET, WDOG_RESET, 0x1);
    dog_update_stats(&dog_stats, DOG_KICK, return_address);
    rex_leave_crit_sect(&dog_critsect);
  }
}

#ifdef USE_AOP_PMIC
/* PMIC wdog functions */

static void pmic_wdog_cfg(void)
{
    static bool pmic_wdog_configured = 0;
    void *return_address = (void *)__return_address();
    pmic_rtc_init();
    if (pmic_wdog_enable && !pmic_wdog_configured)
    {
        dog_update_stats(&pmic_dog_stats, PMIC_DOG_CFG, return_address);
#ifdef LONG_PMIC_BITE
        pm_pon_wdog_cfg(0, 17, 7, PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET);
#else
        pm_pon_wdog_cfg(0, 17, 1, PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET);
#endif
        pm_pon_wdog_enable(0, PM_ON);
        pmic_wdog_configured = 1;
    }
    else if (!pmic_wdog_enable && pmic_wdog_configured)
    {
        dog_update_stats(&pmic_dog_stats, PMIC_DOG_DECFG, return_address);
        pm_pon_wdog_enable(0, PM_OFF);
        pmic_wdog_configured = 0;
    }
}

void pmic_wdog_pet(bool unconditional)
{
    static uint64 last_kick_time = 0;
    void *return_address = (void *)__return_address();
    uint64 cur_time = time_service_now();
    pmic_wdog_cfg();
    if ( pmic_wdog_enable && ( unconditional || cur_time > (last_kick_time+PMIC_WDOG_MAX_SLEEP_TIME) ) )
    {
        dog_update_stats(&pmic_dog_stats, PMIC_DOG_KICK, return_address);
        last_kick_time = cur_time;
        CORE_VERIFY(pm_pon_wdog_pet(0) == PM_ERR_FLAG__SUCCESS);
    }
}

/* PMIC RTC Logging functions */

struct {
    uint32 pm_time;
    uint64 rpm_time;
} PMIC_timestamp;

void pmic_rtc_init(void)
{
    static int init = 0;
    if (init == 0)
    {
        if (pm_rtc_get_time(0,&(PMIC_timestamp.pm_time)) == PM_ERR_FLAG__SUCCESS) {
            PMIC_timestamp.rpm_time = time_service_now();
        } else {
            PMIC_timestamp.pm_time = 0xBAD0FA11;
        }
        init = 1;
    }
}
#endif /* USE_AOP_PMIC */

