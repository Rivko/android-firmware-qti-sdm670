/** vi: tw=128 ts=3 sw=3 et
@file udog_hal_slpi.c
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
Copyright (c) 2017,2019 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/dog/src/udog_hal_slpi.c#3 $
$DateTime: 2019/10/17 07:34:44 $
$Change: 20979608 $
$Author: pwbldsvc $
===============================================================================*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

FUNCTIONS
  HAL_dogKick
  HAL_dogForceBite
  HAL_dogForceNMI
  HAL_dogDisableBark_NMI
  HAL_dogBarkISR

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "HALhwio.h"
#include "dog_hal.h"
#include "dog_timeouts.h"
#include DOG_HAL_HEADER
#include "msmhwiobase.h"
#include "err.h"
#include "qurt.h"

#if defined(ISLAND_SUPPORTED)
#include "utimer.h"
#include "uSleep_mode_trans.h"
uSleep_notification_cb_handle dog_usleep_handle;
utimer_type uImage_timer_dog;
#endif

volatile boolean dog_intentional_timeout_flag = FALSE;
#if defined(FEATURE_DISABLE_HW_WATCHDOG)
volatile boolean dog_hal_disable_for_bringup = TRUE;
#else
volatile boolean dog_hal_disable_for_bringup = FALSE;
#endif

/** =====================================================================
 * Function:
 *     HAL_dogBarkISR
 *
 * Description:
 *     This function is called when the watchdog bark interrupt occurs
 *
 * Parameters:
 *     callback_param : not used
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogBarkISR(unsigned int callback_param)
{
   if (qurt_island_get_status() == TRUE)
   {
      ERR_FATAL("DOG BARK island Starvation %d %d %d",
                HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_IN, // supply BITE timer value
                HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_IN, // supply BARK timer value
                HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_IN   // supply NMI/UNMASKED timer value
               );
   }
   else
   {
      ERR_FATAL("DOG BARK Starvation %d %d %d",
                HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_IN, // supply BITE timer value
                HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_IN, // supply BARK timer value
                HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_IN   // supply NMI/UNMASKED timer value
               );
   }
}

/** =====================================================================
 * Function:
 *     HAL_dogKick
 *
 * Description:
 *     This function kicks the hardware dog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogKick(void)
{
   if (!dog_intentional_timeout_flag)
   {
      HWIO_SSC_QDSP6SS_WDOG_RESET_OUT(1 << HWIO_SSC_QDSP6SS_WDOG_RESET_WDOG_RESET_SHFT);
   }
}

/** =====================================================================
* Function:
*     HAL_dogForceBite
*
* Description:
*     Force the watchdog to bite
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void HAL_dogForceBite(void)
{
   if(dog_hal_disable_for_bringup != TRUE)
   {
      // KICK DOG
      HWIO_SSC_QDSP6SS_WDOG_RESET_OUT(1 << HWIO_SSC_QDSP6SS_WDOG_RESET_WDOG_RESET_SHFT);
      
      // SET BARK TIMEOUT AS MAX
      HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_OUTM(HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK, HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK);
     
      // DISABLE NMI
      HWIO_SSC_QDSP6SS_WDOG_CTL_OUTM(HWIO_SSC_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_BMSK, 0 << HWIO_SSC_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_SHFT);
      
      // SET MINIMUM TIMEOUT
      HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_OUTM(HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_DATA_BMSK, DOG_BITE_ENABLE_TIMEOUT_NOW_SCLK);

      // ENABLE BITE
      HWIO_SSC_QDSP6SS_WDOG_CTL_OUTM(HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_BMSK, 1 << HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_SHFT);

      // KICK DOG
      HWIO_SSC_QDSP6SS_WDOG_RESET_OUT(1 << HWIO_SSC_QDSP6SS_WDOG_RESET_WDOG_RESET_SHFT);
      
      dog_intentional_timeout_flag = TRUE;                                          //Flag to block kicks
   }
}

/** =====================================================================
* Function:
*     HAL_dogForceNMI
*
* Description:
*     Force the watchdog to do an NMI
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void HAL_dogForceNMI(void)
{
   if(dog_hal_disable_for_bringup != TRUE)
   {
      // KICK DOG
      HWIO_SSC_QDSP6SS_WDOG_RESET_OUT(1 << HWIO_SSC_QDSP6SS_WDOG_RESET_WDOG_RESET_SHFT);
      
      // SET BARK TIMEOUT AS MAX
      HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_OUTM(HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK, HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK);
     
      // SET BITE TIMEOUT
      HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_OUTM(HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_DATA_BMSK, DOG_BITE_ENABLE_TIMEOUT_SCLK_FORCE);

      // SET NMI TIMEOUT
      HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_OUTM(HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_DATA_BMSK, DOG_NMI_ENABLE_TIMEOUT_SCLK_FORCE);

      // ENABLE NMI
      HWIO_SSC_QDSP6SS_WDOG_CTL_OUTM(HWIO_SSC_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_BMSK, 1 << HWIO_SSC_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_SHFT);
      
      // ENABLE BITE
      HWIO_SSC_QDSP6SS_WDOG_CTL_OUTM(HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_BMSK, 1 << HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_SHFT);

      // KICK DOG
      HWIO_SSC_QDSP6SS_WDOG_RESET_OUT(1 << HWIO_SSC_QDSP6SS_WDOG_RESET_WDOG_RESET_SHFT);
      
      dog_intentional_timeout_flag = TRUE;                                          //Flag to block kicks
   }
}

/** =====================================================================
* Function:
*     HAL_dogDisableBark_NMI
*
* Description:
*     Disable the watchdog Bark and NMI
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void HAL_dogDisableBark_NMI(void)
{
   if(dog_hal_disable_for_bringup != TRUE)
   {
      // SET BARK TIMEOUT AS MAX
      HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_OUTM(HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK, HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK);
    
      // DISABLE NMI
      HWIO_SSC_QDSP6SS_WDOG_CTL_OUTM(HWIO_SSC_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_BMSK, 0 << HWIO_SSC_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_SHFT);
   }
}

/** =====================================================================
* Function: Wrapper function
*     dog_disable_bark_nmi
*
* Description:
*     Disable the watchdog to Bark and NMI
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void dog_disable_bark_nmi(void)
{
   HAL_dogDisableBark_NMI();
}

/** =====================================================================
* Function: Wrapper function
*     dog_force_bite
*
* Description:
*       Force the watchdog to bite
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void dog_force_bite(void)
{
   HAL_dogForceBite();
}

/** =====================================================================
* Function: Wrapper function
*     dog_force_nmi
*
* Description:
*       Force the watchdog to do an NMI
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void dog_force_nmi(void)
{
   HAL_dogForceNMI();
}

/** =====================================================================
 * Function: Wrapper function
 *     dog_force_kick
 *
 * Description:
 *     This function kicks the hardware dog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void dog_force_kick(void)
{
   HAL_dogKick();
}

/* ISLAND_SUPPORTED is defined in dog_hal.scons*/
#if defined(ISLAND_SUPPORTED)
/** =====================================================================
* Function:
*     HAL_dogDeferKickTimerCb_island
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
static void HAL_dogDeferKickTimerCb_island(utimer_cb_data_type unused)
{
   HAL_dogKick();
}

/** =====================================================================
 * Function: dog_island_cb
 *
 * Description:
 *     callback to kick dog during island mode exit.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void dog_island_cb (uSleep_state_notification state)
{
   switch (state)
   {
      case USLEEP_STATE_ENTER:
      {
         dog_force_kick();
         if(UTE_SUCCESS != utimer_set_64(&uImage_timer_dog,
                                      (utimer_timetick_type)DOG_DEFER_KICK_TIME_MS,
                                      (utimer_timetick_type)DOG_DEFER_KICK_TIME_MS,
                                       UT_MSEC))
         {
             ERR_FATAL("Dog utimer_set_64 failed", 0, 0, 0);
         }
      }
      break;
      case USLEEP_STATE_EXIT:
      {
         utimer_clr_64(&uImage_timer_dog,UT_MSEC);
         dog_force_kick();
      }
      break;
      default:
         break;
   }
}

/** =====================================================================
 * Function:
 *     dog_island_init
 *
 * Description:
 *     This function is used for registration of callbacks for island mode entry and exit.
 *     And to start utimer to kick WDOG
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void dog_island_init(void)
{
   if(UTE_SUCCESS != utimer_def_osal(&uImage_timer_dog,
                                  UTIMER_FUNC1_CB_TYPE,
                                  HAL_dogDeferKickTimerCb_island, 0))
   {
       ERR_FATAL("Dog utimer_def_osal failed", 0, 0, 0);
   }

   if(UTE_SUCCESS != utimer_set_deferrable(&uImage_timer_dog, TRUE))
   {
       ERR_FATAL("Dog utimer_set_deferrable failed", 0, 0, 0);
   }

   /* Call the uSleep registration with the callback function.
    * TODO: Revisit ENTER and EXIT latencies ,if needed.*/
   dog_usleep_handle = uSleep_registerNotificationCallback(
                                               100, /* ENTER latency */
                                               100, /* EXIT latency */
                                               dog_island_cb);
}
#else
void dog_island_init(void)
{
   /*keep it blank for non-island builds*/
}
#endif