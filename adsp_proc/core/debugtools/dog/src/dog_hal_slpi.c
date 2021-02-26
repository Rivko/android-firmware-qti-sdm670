/** vi: tw=128 ts=3 sw=3 et
@file dog_hal_slpi.c
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
Copyright (c) 2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/dog/src/dog_hal_slpi.c#2 $
$DateTime: 2017/08/17 11:25:19 $
$Change: 14115431 $
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

#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "HALhwio.h"
#include "dog_hal.h"
#include "dog_timeouts.h"
#include DOG_HAL_HEADER
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

/* If this flag is set to TRUE, hw wdog will NOT be initialized.
*  This flag should be set to TRUE only for DEBUG purposes eg TRACE32 Instrument to Disable hw wdog At Runtime
*/
extern volatile boolean dog_hal_disable_for_bringup;
extern volatile boolean dog_intentional_timeout_flag;

/** =====================================================================
 * Function:
 *     HAL_dogEnable
 *
 * Description:
 *     This function enables the hardware dog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogEnable(void)
{
   if(dog_hal_disable_for_bringup != TRUE)
   {
      // SET NMI TIMEOUT
      HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_OUTM(HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_DATA_BMSK, DOG_NMI_ENABLE_TIMEOUT_SCLK);

      // ENABLE NMI
      HWIO_SSC_QDSP6SS_WDOG_CTL_OUTM(HWIO_SSC_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_BMSK, 1 << HWIO_SSC_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_SHFT);

      // SET BITE TIMEOUT
      HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_OUTM(HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_DATA_BMSK, DOG_BITE_ENABLE_TIMEOUT_SCLK);

      // ENABLE WDOG
      HWIO_SSC_QDSP6SS_WDOG_CTL_OUTM(HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_BMSK, 1 << HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_SHFT);
   }
}

/** =====================================================================
 * Function:
 *     HAL_dogEnableOnly
 *
 * Description:
 *     This function enables only the hardware dog bit
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogEnableOnly(void)
{
   if(dog_hal_disable_for_bringup != TRUE)
   {      
      // ENABLE WDOG
      HWIO_SSC_QDSP6SS_WDOG_CTL_OUTM(HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_BMSK, 1 << HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_SHFT);
   }
}

/** =====================================================================
 * Function:
 *     HAL_dogDisable
 *
 * Description:
 *     This function disables the hardware dog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogDisable(void)
{
   if (!dog_intentional_timeout_flag)
   {
      // DISABLE WDOG
      HWIO_SSC_QDSP6SS_WDOG_CTL_OUTM(HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_BMSK, 0 << HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_SHFT);
   }
}

/** =====================================================================
 * Function:
 *     HAL_dogEnableBark
 *
 * Description:
 *     Enable the watchdog bark timer.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogEnableBark(void)
{
   // SET BARK TIMEOUT
   HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_OUTM(HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK, DOG_BARK_ENABLE_TIMEOUT_SCLK);
}

/** =====================================================================
 * Function:
 *     HAL_dogDisableBark
 *
 * Description:
 *     Disable the watchdog bark timer.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogDisableBark(void)
{
   // SET BARK TIMEOUT "NEVER"
   HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_OUTM(HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK, HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK);
}

/** =====================================================================
* Function:
*     HAL_dogIsAutokicking
*
* Description:
*     Check if dog is autokicking i.e wdog is disabled
*
* Parameters:
*     unsigned int status pointer
*
* Returns:
*     pointer set to 1 if autokicking(wdog disabled), set to 0 (wdog enabled) otherwise
* =====================================================================  */
void HAL_dogIsAutokicking(unsigned int* status)
{
   if (NULL != status)
   {
      if (0 != (HWIO_SSC_QDSP6SS_WDOG_CTL_INM(HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_BMSK) & HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_BMSK))
      {
         *status = 0; //wdog is enabled so its NOT "autokicking"
      }
      else
      {
         *status = 1; //wdog is disabled so it is "autokicking"
      }
   }
}
