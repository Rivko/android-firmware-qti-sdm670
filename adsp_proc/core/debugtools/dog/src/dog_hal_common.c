/** vi: tw=128 ts=3 sw=3 et
@file dog_hal_common.c
@brief This file contains the API details for the Dog Services, API 2.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2017-2019 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/dog/src/dog_hal_common.c#5 $
$DateTime: 2019/10/17 07:34:44 $
$Change: 20979608 $
$Author: pwbldsvc $
===============================================================================*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

FUNCTIONS
  HAL_dogEnable
  HAL_dogDisable
  HAL_dogAutokick
  HAL_dogEnableBark
  HAL_dogDisableBark
  HAL_dogRegisterBark
  HAL_dogIsAutokicking
  HAL_dogInit

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "stdarg.h"
#include "stdlib.h"
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "HALhwio.h"
#include "dog_hal.h"
#include "dog_timeouts.h"
#include "msmhwiobase.h"
#include "DDIInterruptController.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "err.h"
#include "timer.h"
#include "sys_m_smsm.h"
#include "rcecb.h"
#include "rcinit.h"
#include "tms_utils_msg.h"

static struct dog_hw_internal_s 
{
  /* Kick/Pet timer for HW WDOG */
  timer_type defer_kick_timer;
  timer_group_type timer_group_d;

  /* One time Timer coupled tightly to RCINIT_RCECB_INITGROUPS.
  * A fail safe mechanism to detect RCINIT groups initialization has failed.
  * Its a non deferrable timer. */
  timer_type startup_grace_timer;

  volatile uint32_t scale_counter;      /* Resets based on DOG_SCALING_FACTOR */
  volatile uint32_t hb_counter;         /* Resets after 5 counts (to signal DOG HB every 1 second)*/
  
  qurt_mutex_t mutex;
  int mutex_inited;
} dog_hw_internal;

/* If this flag is set to TRUE, hw wdog will NOT be initialized.
*  This flag should be set to TRUE only for DEBUG purposes eg TRACE32 Instrument to Disable hw wdog At Runtime
*/
extern volatile boolean dog_hal_disable_for_bringup;
extern volatile boolean dog_intentional_timeout_flag;

/* Extern for signaling dog heartbeart task */
extern void dog_hb_event_signal(void);

extern void HAL_dogBarkISR(unsigned int callback_param);
extern void HAL_dogKick(void);
extern void HAL_dogForceBite(void);
extern void HAL_dogEnableOnly(void);
extern void dog_island_init(void);

extern void dog_hb_autokick(boolean enable_hb_autokick);

static void HAL_dogStartupSuccessCb(void);
static void HAL_dogStartupGraceFailureCb(timer_cb_data_type);
static void HAL_dogDeferKickTimerCb(timer_cb_data_type);

static inline void DOG_LOCK_INIT(void)
{
   if (!dog_hw_internal.mutex_inited)
   {
      qurt_pimutex_init(&dog_hw_internal.mutex);

      dog_hw_internal.mutex_inited = 1;
   }
}

static inline void DOG_LOCK(void)
{
   if (dog_hw_internal.mutex_inited)
   {
      qurt_pimutex_lock(&dog_hw_internal.mutex);
   }
}

static inline void DOG_UNLOCK(void)
{
   if (dog_hw_internal.mutex_inited)
   {
      qurt_pimutex_unlock(&dog_hw_internal.mutex);
   }
}


/** =====================================================================
* Function:
*     HAL_dogStartupSuccessCb
*
* Description:
*     This function is called after rc group initialization is done.  
*     It clears the timer and de-registers with RCvent for any future events.
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
static void HAL_dogStartupSuccessCb(void)
{
   time_timetick_type remaining_time = 0;

   /* Clear the timer */
   timer_stop(&dog_hw_internal.startup_grace_timer, T_NONE, &remaining_time);

   /* Undefine the timer */
   timer_undef(&dog_hw_internal.startup_grace_timer);

   /* Unregister with RC */
   (void)rcecb_unregister_name(RCINIT_RCECB_INITGROUPS, HAL_dogStartupSuccessCb);
}

/** =====================================================================
* Function:
*     HAL_dogStartupGraceFailureCb
*
* Description:
*     This function is called when the dog start up grace timer expires.  
*     It results in Error fatal.
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
static void HAL_dogStartupGraceFailureCb(timer_cb_data_type unused)
{
   time_timetick_type remaining_time = 0;

   /* Clear the timer */
   timer_stop(&dog_hw_internal.startup_grace_timer, T_NONE, &remaining_time);

   /* Undefine the timer */
   timer_undef(&dog_hw_internal.startup_grace_timer);

   ERR_FATAL("DOG detects stalled initialization, triage with IMAGE OWNER", 0, 0, 0);

}

/** =====================================================================
* Function:
*     HAL_dogDeferKickTimerCb
*
* Description:
*     timer driven function for hardware dog
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
static void HAL_dogDeferKickTimerCb(timer_cb_data_type unused)
{
   unsigned int dog_disabled;

   DOG_LOCK();

   /* Check if DOG is disabled due to LPM */
   HAL_dogIsAutokicking(&dog_disabled);
   
   if (!dog_disabled)
   {
#if defined(DOG_SCALING_FACTOR) && DOG_SCALING_FACTOR > 1
      /* Pet DOG as per DOG_SCALING_FACTOR */
      if(DOG_SCALING_FACTOR == ++dog_hw_internal.scale_counter)
      {
         dog_hw_internal.scale_counter = 0; /* Reset scale_counter */
         HAL_dogKick(); /* Pet DOG */
      }
#elif defined(DOG_SCALING_FACTOR) && DOG_SCALING_FACTOR == 1
      HAL_dogKick(); /* Pet DOG */
#endif

      /* Signal DOG HB every second (as per DOG_HB_KICK_PERIOD(5) * 200 ms) */
      if(DOG_HB_KICK_PERIOD == ++dog_hw_internal.hb_counter)
      {
         dog_hw_internal.hb_counter = 0; /* Reset hb_counter */
         dog_hb_event_signal(); /* Signal DOG_HB_EVENT */
      }
   }
   
   DOG_UNLOCK();
}

/** =====================================================================
 * Function:
 *     HAL_dogRegisterBark
 *
 * Description:
 *     Register an ISR for the watchdog bark interrupt.
 *
 * Parameters:
 *     none
 *
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogRegisterBark(void)
{
   static DalDeviceHandle* hInthandle;
   unsigned int islandMode  = 1;

   if (DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &hInthandle) || NULL == hInthandle)
   {
      ERR_FATAL("Dog cannot register with DAL", 0, 0, 0);
   }

   if ( DAL_SUCCESS != DalInterruptController_RegisterISR(
         hInthandle, 
         DOG_IRQ,
         (DALIRQ)HAL_dogBarkISR, 
         0,
         DALINTRCTRL_ENABLE_CONFIG_EX(DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER, 0, islandMode)))
   {
      ERR_FATAL("Dog bark interrupt cannot register with DAL", 0, 0, 0);
   }
}

/** =====================================================================
 * Function:
 *     HAL_dogAutokick
 *
 * Description:
 *     This function turns on dog autokick by pausing the HW, HB and SW timer 
 *     and disabling the HW wdog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogAutokick(boolean enable_autokick)
{
   unsigned int dog_disabled;

   if(dog_hal_disable_for_bringup != TRUE)
   {
      if (!dog_intentional_timeout_flag)
      {
         DOG_LOCK();

         HAL_dogIsAutokicking(&dog_disabled);

         HAL_dogKick();

         /* Check if we are switching the dog_disabled state */
         if (dog_disabled != enable_autokick)
         {
            if (enable_autokick)
            {
               /* Now auto-kicking.  Pause Watchdog's hardware timer and Disable WDOG */
              HAL_dogDisable();
              timer_pause(&dog_hw_internal.defer_kick_timer);
            }
            else
            {
               /* No longer auto-kicking.  Resume watchdog hardware timer and enable WDOG */
               timer_resume(&dog_hw_internal.defer_kick_timer);
               HAL_dogEnableOnly();
            }

            dog_hb_autokick(enable_autokick);
         }

         DOG_UNLOCK();
      }
   }
}

/** =====================================================================
* Function:
*     HAL_dogInit
*
* Description:
*      Initialize hardware dog
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void HAL_dogInit(void)
{
   DOG_LOCK_INIT();

#if defined(DOG_SCALING_FACTOR) && DOG_SCALING_FACTOR < 1
   dog_hal_disable_for_bringup = TRUE; //DOG disabled
#endif

   if(dog_hal_disable_for_bringup != TRUE)
   {
      HAL_dogEnableBark();                                                          /* Enable the watchdog bark timer. */

      HAL_dogRegisterBark();                                                        /* Register an ISR for the watchdog bark interrupt. */

      /* Set timer to kick the wdog as deferrable timer to avoid unnecessary wakeup of Q6 */
      timer_group_set_deferrable(&dog_hw_internal.timer_group_d, TRUE);

      if(TE_SUCCESS != timer_def_osal(&dog_hw_internal.defer_kick_timer,
                                     &dog_hw_internal.timer_group_d,
                                     TIMER_FUNC1_CB_TYPE,
                                     HAL_dogDeferKickTimerCb, NULL))
      {
          ERR_FATAL("Dog defer_kick_timer def failed", 0, 0, 0);
      }

      if(TE_SUCCESS != timer_set_64(&dog_hw_internal.defer_kick_timer,
                                   DOG_DEFER_KICK_TIME_MS,
                                   DOG_DEFER_KICK_TIME_MS,
                                   T_MSEC))
      {
          ERR_FATAL("Dog defer_kick_timer set failed", 0, 0, 0);
      }
      
      HAL_dogEnable();

      HAL_dogKick();

      dog_island_init();
   }

   if(sys_m_smsm_apps_set_cached(SYS_M_SUBSYS2AP_SMSM_ERR_HDL_RDY) != SYS_M_SMP2P_SUCCESS)
   {
       TMS_MSG_HIGH("SMP2P bit set/cached did not succeed");
   }
   else
   {
       TMS_MSG_HIGH("DOG SMP2P bit set/cached succeeded");
   }

   if(dog_hal_disable_for_bringup != TRUE)
   {
      /* Event indicates startup completes */
      (void)rcecb_register_name(RCINIT_RCECB_INITGROUPS, HAL_dogStartupSuccessCb); 

      /* Define watchdog timer */
      if(TE_SUCCESS != timer_def_osal(&dog_hw_internal.startup_grace_timer, 
                                     &timer_non_defer_group, 
                                     TIMER_FUNC1_CB_TYPE, 
                                     HAL_dogStartupGraceFailureCb, NULL ))
      {
       ERR_FATAL("Dog startup_grace_timer def failed", 0, 0, 0);
      }

      /* Set watchdog timer */
      if(TE_SUCCESS != timer_set_64(&dog_hw_internal.startup_grace_timer, 
                                   DOG_STARTUP_GRACE_TIME_SEC, 
                                   0, T_SEC ))
      {
       ERR_FATAL("Dog startup_grace_timer set failed", 0, 0, 0);
      }
   }
}

/** =====================================================================
 * Function: Wrapper function for HAL_dogForceBite
 *
 *
 * Description:
 *     This function dose Dog Bite in Single Threaded Mode (no mutex)
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void dog_intentional_timeout_stm(void)
{
   HAL_dogForceBite();                                                           //lower bite value
}
