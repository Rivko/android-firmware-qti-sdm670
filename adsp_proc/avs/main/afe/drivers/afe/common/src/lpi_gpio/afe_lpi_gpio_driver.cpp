/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/lpi_gpio/afe_lpi_gpio_driver.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     afe_lpi_gpio_driver.cpp

DESCRIPTION: LPI GPIO configuration functions

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI)
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/lpi_gpio/afe_lpi_gpio_driver.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/06/17    sudhir      Created file

============================================================================*/


/*=====================================================================
 Includes
 ======================================================================*/
#include "adsp_error_codes.h"
#include "AFEDevCommon.h"

#ifndef SIM
extern "C"
{
#include "uTlmm.h"
#include "DDIInterruptController.h"
}
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#endif

/* FUNC_SEL = 2  from IPCAT. */
/* Curently we hardcoding FUNC_SEL =2. With experience of TLMM GPIO  Drive strength, PULL*/
/* values are always fixed, We never changed it. If it requires to have different values*/
/* from target to target need to expand device config for LPI GPIO structures. */
/* Time being we are hard coding.*/

#define UTLMM_GPIO_FUNC_SEL_2  2

/* 
   This function configures LPI GPIO pin 
    
   Following are sequence of steps: 
   1.  Configure UTLMM GPIO 
*/

ADSPResult afe_device_lpi_gpio_init(gpio_int_properties_t *gpio_int_prop_ptr)
{

#ifndef SIM
   uTlmmGpioSignalType     pin_config;
   bool_t                  is_gpio_cfg_success = FALSE;

   /* Clear the GPIO config flag */
   gpio_int_prop_ptr->is_gpio_configured = FALSE;

   /* UTLMM_GPIO_CFG(gpio, func, dir, pull, drive)
    * Currently all values hardcoded, these values not changed frequently. From 8974 we are using same values
    * So even for LPI GPIO, hardcoding the values. if they are changing from target to target, need to expand
    * current device config structure for lpi gpio, with this values.
    * DRIVE STRENGTH is dont care for input GPIO */

   pin_config = UTLMM_GPIO_CFG(gpio_int_prop_ptr->n_gpio,
                               UTLMM_GPIO_FUNC_SEL_2,
                               UTLMM_GPIO_INPUT,
                               UTLMM_GPIO_PULL_DOWN,
                               UTLMM_GPIO_2MA);

   /* Configure the GPIO */
   if (FALSE == (is_gpio_cfg_success = uTlmm_ConfigGpio(pin_config, UTLMM_GPIO_ENABLE)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "LPI GPIO[%lu] Config failed", gpio_int_prop_ptr->n_gpio);

      return ADSP_EFAILED;
   }

   /* Set the GPIO config flag to TRUE */
   gpio_int_prop_ptr->is_gpio_configured = TRUE; 

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "LPI GPIO[%lu] Config success", gpio_int_prop_ptr->n_gpio);

#endif

   return ADSP_EOK;
}

ADSPResult afe_device_lpi_gpio_deinit(gpio_int_properties_t *gpio_int_prop_ptr)
{
   /* Clear the GPIO config flag */
   gpio_int_prop_ptr->is_gpio_configured = FALSE;

   return ADSP_EOK;
}

